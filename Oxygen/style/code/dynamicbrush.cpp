/***************************************************************************
 *   Copyright (C) 2006-2007 by Thomas Lübking                             *
 *   thomas.luebking@web.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QApplication>
#include <QDesktopWidget>
#include <QResizeEvent>
#include <QTimer>
#include <QImage>
#include <QPainter>
#include <QX11Info>
// #include <QDebug>

#define GL_GLEXT_PROTOTYPES
#include <QGLPixelBuffer>
#include <GL/glut.h>

#include <X11/Xatom.h>
#include "oxrender.h"

#include "dynamicbrush.h"

#define SETBACKGROUND(_pix_)\
   const_cast<QBrush*>(&QApplication::palette().brush(QPalette::Active, QPalette::Window))->setTexture(_pix_);\
   const_cast<QBrush*>(&QApplication::palette().brush(QPalette::Inactive, QPalette::Window))->setTexture(_pix_);\
   const_cast<QBrush*>(&QApplication::palette().brush(QPalette::Disabled, QPalette::Window))->setTexture(_pix_)

#ifndef CLAMP
#define CLAMP(x,l,u) (x) < (l) ? (l) :\
(x) > (u) ? (u) :\
(x)
#endif

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

static Atom oxygen_isQtWindow = XInternAtom(QX11Info::display(), "OXYGEN_IS_QT_WINDOW", False);

void (DynamicBrush::*updateBrush)();

DynamicBrush::DynamicBrush(Mode mode, QObject *parent) : QObject(parent), _mode(mode), _glShadow(0)
{
   _timer = new QTimer(this);
   connect (_timer, SIGNAL(timeout()), this, SLOT(wipeBackground()));
   switch (_mode)
   {
   case Tiled: updateBrush = &DynamicBrush::updateBrushTiled; break;
   case XRender: updateBrush = &DynamicBrush::updateBrushRender; break;
   case OpenGL: updateBrush = &DynamicBrush::updateBrushGL; break;
   case Tiled2: updateBrush = &DynamicBrush::updateBrushEdMetal; break;
   }
}

DynamicBrush::DynamicBrush(Pixmap pixmap, Pixmap shadow, int bgYoffset, QObject *parent) : QObject(parent), _pixmap(pixmap), _shadow(shadow), _bgYoffset(bgYoffset), _mode(Tiled) 
{
   _timer = new QTimer(this);
   connect (_timer, SIGNAL(timeout()), this, SLOT(wipeBackground()));
   updateBrush = &DynamicBrush::updateBrushTiled;
}

#define SATURATION_COLOR(R,G,B) \
   grey = (299 * R + 587 * G + 114 * B) / 1000; \
   delta = 255 - grey; \
   grey = (grey *(10 - 5)) / 10; \
   iGrey = 255 - grey;\
   destR = (iGrey * (srcR - delta) + grey * R) / 255; \
   destG = (iGrey * (srcG - delta) + grey * G) / 255; \
   destB = (iGrey * (srcB - delta) + grey * B) / 255
    
#define SATURATION_COLOR2(S,R,G,B) \
   int max = 255+(int)(0.65*(100-S)); \
   destR = CLAMP((srcR + R - 128), 0, max); \
   destG = CLAMP((srcG + G - 128), 0, max); \
   destB = CLAMP((srcB + B - 128), 0, max); \
   destR = (S*destR + (100-S)*R)/100; \
   destG = (S*destG + (100-S)*G)/100; \
   destB = (S*destB + (100-S)*B)/100

#define SATURATION_COLOR3(S,R,G,B) \
   destR = (S*(srcR + R - 128)/100 + R)/2; \
   destG = (S*(srcG + G - 128)/100 + G)/2; \
   destB = (S*(srcB + B - 128)/100 + B)/2

#define COLOR_SPACE\
      destR = CLAMP(destR, 0, 255);\
      destG = CLAMP(destG, 0, 255);\
      destB = CLAMP(destB, 0, 255)

static QPixmap *tint(const QImage &img, const QColor& c)
{
   QImage *dest = new QImage( img.width(), img.height(), 32, 0 );
   unsigned int *data = ( unsigned int * ) img.bits();
   unsigned int *destData = ( unsigned int* ) dest->bits();
   int total = img.width() * img.height();
   int srcR = c.red() - 128;
   int srcG = c.green() - 128;
   int srcB = c.blue() - 128;
   int destR, destG, destB;
   int current;
   
   // grain/merge from the gimp. TODO: use mmx/sse here
   for ( current = 0 ; current < total ; ++current )
   {
      destR = srcR + qRed( data[ current ] );
      destG = srcG + qGreen( data[ current ] );
      destB = srcB + qBlue( data[ current ] );
      COLOR_SPACE;
      destData[ current ] = qRgba( destR, destG, destB, qAlpha(data[ current ]) );
   }
   QPixmap *pix = new QPixmap(*dest);
   delete dest;
   return pix;
}

DynamicBrush::DynamicBrush(const QImage &leftCenter, const QImage &leftTile, QObject *parent) : QObject(parent), _mode(Tiled2), _glShadow(0)
{
   _timer = new QTimer(this);
   connect (_timer, SIGNAL(timeout()), this, SLOT(wipeBackground()));
   updateBrush = &DynamicBrush::updateBrushEdMetal;
   QColor c = QApplication::palette().color(QPalette::Active, QPalette::Window);
   QMatrix mirror;
   mirror.setMatrix ( -1, 0, 0, 1, 0, 0);
   _center[0][0] = tint(leftCenter, c);
   _center[0][1] = tint(leftCenter.mirrored(true, false), c);
   _tile[0][0] = tint(leftTile, c);
   _tile[0][1] = tint(leftTile.mirrored(true, false), c);
   c = c.dark(110);
   _center[1][0] = tint(leftCenter, c);
   _center[1][1] = tint(leftCenter.mirrored(true, false), c);
   _tile[1][0] = tint(leftTile, c);
   _tile[1][1] = tint(leftTile.mirrored(true, false), c);
}

void DynamicBrush::setMode(Mode mode)
{
   if (mode == _mode) return;
   _mode = mode;
   switch (_mode)
   {
   case Tiled: updateBrush = &DynamicBrush::updateBrushTiled; break;
   case XRender: updateBrush = &DynamicBrush::updateBrushRender; break;
   case OpenGL: updateBrush = &DynamicBrush::updateBrushGL; break;
   case Tiled2: updateBrush = &DynamicBrush::updateBrushEdMetal; break;
   }
}

bool DynamicBrush::eventFilter ( QObject * object, QEvent * ev )
{
   /* Rule out non widgets (this should never happen, but who knows...)*/
   if (!qobject_cast<QWidget*>(object))
   {
      object->removeEventFilter(this);
      return false;
   }
   
   /* we're not interested in hidden elements */
   if (!((QWidget*)object)->isVisible()) return false;
      
   /* Test for the currently demanded bg size */
   QSize size;
   bool triggerUpdate = false;
   if (ev->type() == QEvent::UpdateRequest)
      size = ((QWidget*)object)->topLevelWidget()->size();
   else if (((QWidget*)object)->isWindow())
   {
      if (ev->type() == QEvent::Resize)
      {
         triggerUpdate = true;
         size = ((QResizeEvent*)ev)->size();
      }
      else if ( ev->type() == QEvent::WindowActivate ||
                ev->type() == QEvent::WindowDeactivate )
         size = ((QWidget*)object)->size();
      else if (ev->type() == QEvent::Show)
      {
         size = ((QWidget*)object)->size();
         /* we might get a new deco, so tell it that this it a oxygen styled qt window */
         int one = 1;
         XChangeProperty(QX11Info::display(), ((QWidget*)object)->winId(), oxygen_isQtWindow, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &(one), 1L);
         triggerUpdate = true;
      }
      else if (ev->type() == QEvent::Hide)
         size = QSize();
      else return false;
   }
   else return false;
   
   _timer->start(25000); // the timer will wipe the background, if it's not updated since 25 secs (maybe the window is completely covered)

   /* we don't need a bg pix for the moment, let's save some RAM */
   if (size.isNull())
      wipeBackground();

   /* In case the demanded size differs from the one we have: make an update ;)*/
   if (_size.width() != size.width() ||
       (_mode != Tiled2 && _size.height() != size.height()))
   {
      _size = size;
       (this->*updateBrush)();
   }
   /* Maybe we better force an update */
   if (triggerUpdate) ((QWidget*)object)->update();
   
   return false;
}

void DynamicBrush::wipeBackground()
{
   _size = QSize();
   QPixmap qPix;
      SETBACKGROUND(qPix);
}

void DynamicBrush::updateBrushTiled()
{
   Display *dpy = QX11Info::display();
   int ttbh = 22;
   int height = _bgYoffset - ttbh;
   int width = _size.width();
   int xb = QApplication::desktop()->width() - width;
   int x = xb/3;
   int w_2 = (width >> 1);
   /* The following is a bit tricky.
   we want a complex background including three different gradients (light sources)
   to gain this, we created a desktop sized pixmap and now extract parts from it to
   create a new window sized pixmap.
   In our particular case we want to shape away 2/3 from the sides and 1/3 from the
   center - in the upper region. below that, we take a center part
   (i.e.shape away 1/2 of the difference from each side, this could be smarter)
   */
   QPixmap qPix(_size);
   // first the left and right top part ==================
   GC gc = XCreateGC( dpy, qPix.handle(), 0, 0 );
   XCopyArea( dpy, _pixmap, qPix.handle(), gc, x, ttbh, w_2, height, 0, 0 );
   x = (x<<1) + w_2;
   XCopyArea( dpy, _pixmap, qPix.handle(), gc, x, ttbh, width-w_2, height, w_2, 0 );
   // ===================================
   // now the bottom center part ===========================
   XCopyArea( dpy, _pixmap, qPix.handle(), gc, xb>>1, _bgYoffset, width, _size.height() - height, 0, height );
   XFreeGC ( dpy , gc );
   
   /* update  the brush textures*/
   SETBACKGROUND(qPix);
}


void DynamicBrush::updateBrushEdMetal()
{
   if (_glShadow)
   {
      delete _glShadow; _glShadow = 0L;
   }
   Display *dpy = QX11Info::display();
   int width = _size.width();
   int height = _center[0][0]->height();
   float factor = MIN(1.0, ((float)width)/(2*_tile[0][0]->width()+2*_center[0][0]->width()));
   int wt = (int) (factor * _tile[0][0]->width());
   int wc = (int) (factor * _center[0][0]->width());
   int x = width/2 - wc;
   int i;
   /* we tile this together from the tile and the center images
      The full center should be used for window.width >= 2*tileWidth + centerpartWidth
      below, the center/tile parts percentage is adjusted to the window percentage
      relative to the above value
   */
   QPixmap qPix(width, height);
   GC gc = XCreateGC( dpy, qPix.handle(), 0, 0 );
   // left tile ==================
   for (i = x-wt; i > -1; i-=wt)
      XCopyArea( dpy, _tile[0][0]->handle(), qPix.handle(), gc,
                 0, 0, wt, height, i, 0 );
   if (i != 0)
   {
      i = -i+1;
      XCopyArea( dpy, _tile[0][0]->handle(), qPix.handle(), gc,
                  _tile[0][0]->width()-i, 0, i, height, 0, 0 );
   }
   
   // left center ==================
   XCopyArea( dpy, _center[0][0]->handle(), qPix.handle(), gc,
              0, 0, wc, height, x, 0 );
   x += wc;
   // right center ==================
   XCopyArea( dpy, _center[0][1]->handle(), qPix.handle(), gc,
              _center[0][1]->width() - wc, 0, wc, height, x, 0 );
   x += wc;
   // right tile ==================
   for (i = x; i < width-wt; i+=wt)
      XCopyArea( dpy, _tile[0][1]->handle(), qPix.handle(), gc,
                 0, 0, wt, height, i, 0 );
   if (i != width)
      XCopyArea( dpy, _tile[0][1]->handle(), qPix.handle(), gc,
                  0, 0, width-i, height, i, 0 );
   
   XFreeGC ( dpy , gc );
   
   /* update  the brush textures*/
   SETBACKGROUND(qPix);
}

QPixmap DynamicBrush::glPixmap(const QRect &rect, int darkness)
{
   double r,g,b;
   if (darkness)
      QApplication::palette().color(QPalette::Window).dark(100+darkness).getRgbF(&r,&g,&b);
   else
      QApplication::palette().color(QPalette::Window).getRgbF(&r,&g,&b);
   QGLPixelBuffer pbuffer(_size);
   pbuffer.makeCurrent();
   glViewport(0, 0, (GLsizei) _size.width(), (GLsizei) _size.height());
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
//    gluOrtho2D (0.0, 30.0, 30.0, 0.0); //lrbt
   GLfloat fw = 30.0/_size.width(), fh = 30.0/_size.height();
   gluOrtho2D (rect.x()*fw, rect.right()*fw, rect.bottom()*fh, rect.y()*fh);
   glMatrixMode(GL_MODELVIEW);
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_DEPTH_TEST);
   glBegin (GL_QUADS);
      glColor4f (r*0.8, g*0.8, b*0.8, 1.0);
      glVertex2f (0.0, 30.0);
      glVertex2f (30.0, 30.0);
      glColor4f (r, g, b, 1.0);
      glVertex2f (30.0, 0.0);
      glVertex2f (0.0, 0.0);
   glEnd();
   glBegin (GL_TRIANGLES);
      glColor4f (r, g, b, 0.6);
      glVertex2f (30.0, 30.0);
      glColor4f (r, g, b, 0.0);
      glVertex2f (0.0, 30.0);
      glVertex2f (30.0, 0.0);
   glEnd();
   glBegin (GL_TRIANGLE_FAN);
      r *= 1.1; g *= 1.1; b *=1.1;
      glColor4f (r, g, b, 1.0);
      glVertex2f (15.0, 00.0);
      glColor4f (r, g, b, 0.0);
      glVertex2f (30.0, 0.0);
      glVertex2f (26.0, 3.0);
      glVertex2f (22.0, 3.5);
      glVertex2f (17.0, 5.0);
      glVertex2f (13.0, 5.0);
      glVertex2f (8.0, 3.5);
      glVertex2f (4.0, 3.0);
      glVertex2f (0.0, 0.0);
   glEnd();
   return QPixmap::fromImage(pbuffer.toImage());
}

void DynamicBrush::updateBrushGL()
{
   QPixmap qPix = glPixmap(QRect(QPoint(0,0),_size));
   SETBACKGROUND(qPix);
}

void DynamicBrush::updateBrushRender()
{
//    XRenderCreateRadialGradient (Display *dpy,
//                                      const XRadialGradient *gradient,
//                                      const XFixed *stops,
//                                      const XRenderColor *colors,
//                                      int nstops);
   QColor c = QApplication::palette().color(QPalette::Window);
   QPixmap qPix(_size);
#if 1
   Display *dpy = QX11Info::display();
   OXLinearGradient(lg1, 0,0,qPix.width(),qPix.height());
   XRenderColor cs[3];
   OXRender::setColor(cs[0], /*c*/Qt::red);
   OXRender::setColor(cs[1], /*c.dark(110)*/Qt::white);
   OXRender::setColor(cs[2], Qt::blue);
   XFixed stops[3] = {0,0,1};
   Picture lgp = XRenderCreateLinearGradient(dpy, &lg1, stops, cs, 3);
//    XRenderPictureAttributes pa; pa.repeat = True;
//    XRenderChangePicture (dpy, lgp, CPRepeat, &pa);
   XRenderComposite (dpy, PictOpSrc, lgp, None, qPix.x11PictureHandle(),
                     0, 0, 0, 0, 0, 0, qPix.width(), qPix.height());
   XRenderFreePicture(dpy, lgp);
#else
   QLinearGradient lg1(QPoint(0,0), QPoint(0, _size.height()));
   lg1.setColorAt(0, c);
   lg1.setColorAt(1, c.dark(110));
   QLinearGradient lg2(QPoint(_size.width()/2, _size.height()/2), QPoint(_size.width(), _size.height()));
   QColor ct = c; ct.setAlpha(0);
   lg2.setColorAt(0, ct);
   ct.setAlpha(140);
   lg2.setColorAt(1, ct);
   QPainter p(&qPix);
   p.setPen(Qt::NoPen);
   p.setBrush(lg1);
   p.drawRect( 0, 0, qPix.width(), qPix.height());
   QPoint points[3] =
   {
      QPoint(_size.width(), 0),
      QPoint(_size.width(), _size.height()),
      QPoint(0, _size.height())
   };
   p.setBrush(lg2);
   p.drawPolygon(points, 3);
   p.end();
#endif
   SETBACKGROUND(qPix);
}

QPixmap DynamicBrush::shadow(const QRect &rect)
{
   QPixmap qPix(rect.size());
   switch (_mode)
   {
   case Tiled2:
   {
      int width = _size.width();
      int height = rect.height();
      float factor = MIN(1.0, ((float)width)/(2*_tile[0][0]->width()+2*_center[0][0]->width()));
      int wt = (int) (factor * _tile[0][0]->width());
      int wc = (int) (factor * _center[0][0]->width());
      int x = width/2 - wc;
      QPainter p(&qPix);
      //left
      p.drawTiledPixmap(-rect.x(), 0, x, height, *_tile[1][0], x % wt);
      //left center
      x -= rect.x();
      p.drawTiledPixmap(x, 0, wc, height, *_center[1][0]);
      // right center
      x += wc;
      p.drawTiledPixmap(x, 0, wc, height, *_center[1][1], _center[1][0]->width()-wc);
      // right
      x += wc;
      p.drawTiledPixmap(x, 0, rect.right()-x, height, *_tile[1][1]);
      p.end();
      break;
   }
   case Tiled:
   {
      Drawable x11d = qPix.handle();
      Display *dpy = QX11Info::display();
      GC gc = XCreateGC( dpy, x11d, 0, 0 );
      
      int ttbh = 20;
      int W_TopOff = _bgYoffset - ttbh;
      int W_xBottom = QApplication::desktop()->width() - _size.width();
      int W_xTop = W_xBottom/3;
      int W_w_2 = (_size.width()>>1);
      // topleft geometry : W_xTop, ttbh, W_w_2, W_TopOff
      // topright geometry : (W_xTop<<1) + W_w_2, ttbh, windowSize.width() - W_w_2, W_TopOff
      // bottom geometry: W_xBottom, yoff, windowSize.width(), windowSize.height() - yoff
      
      // top left ----------------
      if (rect.x() < W_w_2 && rect.y() < W_TopOff)
      {
         XCopyArea( dpy, _shadow, x11d, gc,
                     W_xTop + rect.x(), ttbh + rect.y(),
                     //widget->width() - (rect.x() + rect.width() - W_w_2),
                     W_w_2 - rect.x(),
                     //widget->height() - (rect.y() + rect.height() - W_TopOff),
                     W_TopOff - rect.y(),
                     0, 0 );
      }
      
      // top right ----------------
      if (rect.right() > W_w_2 && rect.y() < W_TopOff)
      {
         W_xTop = (W_xTop<<1) + W_w_2;
         int x, xOff, w;
         if (rect.x() < W_w_2)
         {
            w = rect.right() - W_w_2;
            x = W_w_2 - rect.x();
            xOff = W_xTop;
         }
         else
         {
            w = rect.width();
            x = 0;
            xOff = W_xTop + (rect.x() - W_w_2);
         }
         XCopyArea( dpy, _shadow, x11d, gc,
                     xOff, ttbh + rect.y(), w, W_TopOff - rect.y(),
                     x, 0 );
      }
      
      // bottom center part ----------------
      if (rect.bottom() > W_TopOff)
      {
         int h, y, yOff;
         if (rect.y() < W_TopOff)
         {
            h = rect.y() + rect.height() - W_TopOff;
            y = W_TopOff - rect.y();
            yOff = _bgYoffset;
         }
         else
         {
            h = rect.height();
            y = 0;
            yOff = rect.y() + ttbh;
         }
         XCopyArea( dpy, _shadow, x11d, gc,
                     W_xBottom + rect.x(), yOff, rect.width(), h,
                     0, y );
      }
      XFreeGC ( dpy , gc );
      break;
   }
   case OpenGL:
      if (!_glShadow || _lastShadowRect != rect )
      {
         _lastShadowRect = rect;
         qPix = glPixmap(rect, 10);
         _glShadow = new QPixmap(qPix);
      }
      return *_glShadow;
   case XRender:
      if (!_glShadow || _lastShadowRect != rect )
      {
         _lastShadowRect = rect;
         QColor c = QApplication::palette().color(QPalette::Window).dark(110);
         QLinearGradient lg1(QPoint(0,-rect.y()), QPoint(0, _size.height()-rect.y()));
         lg1.setColorAt(0, c);
         lg1.setColorAt(1, c.dark(110));
         QLinearGradient lg2(QPoint(_size.width()/2-rect.x(), _size.height()/2-rect.y()), QPoint(_size.width()-rect.x(), _size.height()-rect.y()));
         QColor ct = c; ct.setAlpha(0);
         lg2.setColorAt(0, ct);
         ct.setAlpha(140);
         lg2.setColorAt(1, ct);
         QPainter p(&qPix);
         p.setPen(Qt::NoPen);
         p.setBrush(lg1);
         p.drawRect( 0, 0, qPix.width(), qPix.height());
         QPoint points[3] =
         {
            QPoint(rect.right(), 0),
            QPoint(rect.right(), rect.bottom()),
            QPoint(rect.x(), rect.bottom())
         };
         p.setBrush(lg2);
         p.drawPolygon(points, 3);
         p.end();
         _glShadow = new QPixmap(qPix);
      }
      return *_glShadow;
   }
   return qPix;
}

#if 0 // this is some stored code - yet not used, but maybe later on

static bool eraseWidget(const QWidget *widget, const QPaintDevice *device, bool pseudoTransparent, const QRegion &clipRegion)
{
   const QWidget *bgWidget = (pseudoTransparent && widget->parentWidget()) ? widget->parentWidget() : widget;
   if (bgWidget->backgroundRole() == Qt::NoBackground || !widget->palette().brush( QPalette::Active, bgWidget->backgroundRole() ).texture().isNull())
      return false;
   
   Pixmap _bgPx;
   if (bgWidget->palette().color(QPalette::Active, bgWidget->backgroundRole()) == qApp->palette().color( QPalette::Active, QPalette::Background))
      _bgPx = bgPix;
   else if (bgWidget->palette().color(QPalette::Active, bgWidget->backgroundRole()) == groupShadowColor_)
      _bgPx = _shadow;
   else
      return false;

   if (!_bgPx)
      return false;
   
   QPoint windowOffset = widget->backgroundOffset();
   int ttbh = 20;
   QSize windowSize = widget->topLevelWidget()->size();
   int W_TopOff = _bgYoffset - ttbh;
   int W_xBottom = qApp->desktop()->width() - windowSize.width();
   int W_xTop = W_xBottom/3;
   int W_w_2 = (windowSize.width()>>1);
   // topleft geometry : W_xTop, ttbh, W_w_2, W_TopOff
   // topright geometry : (W_xTop<<1) + W_w_2, ttbh, windowSize.width() - W_w_2, W_TopOff
   // bottom geometry: W_xBottom, yoff, windowSize.width(), windowSize.height() - yoff
   
   // painting
   GC gc = XCreateGC( QX11Info::display(), device->handle(), 0, 0 );
   
   if (!clipRegion.isNull())
   {
      int num;
      XRectangle *rects = (XRectangle *)qt_getClipRects( clipRegion, num );
      XSetClipRectangles( QX11Info::display(), gc, 0, 0, rects, num, YXBanded );
   }
   
   // top left
   if (windowOffset.x() < W_w_2 && windowOffset.y() < W_TopOff)
   {
      
      XCopyArea( QX11Info::display(), _bgPx, device->handle(), gc,
                 W_xTop + windowOffset.x(),
                 ttbh + windowOffset.y(),
      //widget->width() - (windowOffset.x() + widget->width() - W_w_2),
                 W_w_2 - windowOffset.x(),
      //widget->height() - (windowOffset.y() + widget->height() - W_TopOff),
                 W_TopOff - windowOffset.y(),
                 0, 0 );
   }
   
   // top right
   if (windowOffset.x() + widget->width() > W_w_2
       && windowOffset.y() < W_TopOff)
   {
      W_xTop = (W_xTop<<1) + W_w_2;
      int x, xOff, w;
      if (windowOffset.x() < W_w_2)
      {
         w = windowOffset.x() + widget->width() - W_w_2;
         x = W_w_2 - windowOffset.x();
         xOff = W_xTop;
      }
      else
      {
         w = widget->width();
         x = 0;
         xOff = W_xTop + (windowOffset.x() - W_w_2);
      }
      XCopyArea( QX11Info::display(), _bgPx, device->handle(), gc,
                 xOff,
                 ttbh + windowOffset.y(),
                 w,
                 W_TopOff - windowOffset.y(),
                 x, 0 );
   }
   
   // bottom center part
   if (windowOffset.y() + widget->height() > W_TopOff)
   {
      int h, y, yOff;
      if (windowOffset.y() < W_TopOff)
      {
         h = windowOffset.y() + widget->height() - W_TopOff;
         y = W_TopOff - windowOffset.y();
         yOff = _bgYoffset;
      }
      else
      {
         h = widget->height();
         y = 0;
         yOff = windowOffset.y() + ttbh;
      }
      XCopyArea( QX11Info::display(), _bgPx, device->handle(), gc,
                 W_xBottom + windowOffset.x(),
                 yOff,
                 widget->width(), h,
                 0, y );
   }
   
   XFreeGC ( QX11Info::display() , gc );
   return true;
}
#endif

void DynamicBrush::setXPixmap(Pixmap pixmap, Pixmap shadow)
{
   if (pixmap != -1)
      _pixmap = pixmap;
   if (shadow != -1)
      _shadow = shadow;
}
