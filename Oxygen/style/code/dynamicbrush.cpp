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
#include <QDebug>

#define GL_GLEXT_PROTOTYPES
#include <QGLFramebufferObject>
// #include <QGLPixelBuffer>
// #include <GL/glut.h>

#include <X11/Xatom.h>
#include <cmath>

#include "oxrender.h"

#include "dynamicbrush.h"
#include "dynamicbrush.moc"

// #include "endian.h"

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

static QPixmap tint(const QImage &img, const QColor& c)
{
   QImage *dest = new QImage( img.size(), QImage::Format_RGB32 );
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
   QPixmap pix = QPixmap::fromImage(*dest);
   delete dest;
   return pix;
}

static QPixmap mirror(const QPixmap &pix, Qt::Orientation o)
{
   Display *dpy = QX11Info::display();
   QPixmap mirror(pix.size());
   GC gc = XCreateGC( dpy, mirror.handle(), 0, 0 );
   if (o == Qt::Horizontal)
   {
      int x = pix.width() - 1;
      for (int i = 0; i < pix.width(); ++i)
         XCopyArea( dpy, pix.handle(), mirror.handle(), gc,
                    x-i, 0, 1, pix.height(), i, 0 );
   }
   else
   {
      int y = pix.height() - 1;
      for (int i = 0; i < pix.height(); ++i)
         XCopyArea( dpy, pix.handle(), mirror.handle(), gc,
                     0, y-i, pix.width(), 1, 0, i );
   }
   XFreeGC ( dpy , gc );
   return mirror;
}

static Atom oxygen_isQtWindow = XInternAtom(QX11Info::display(), "OXYGEN_IS_QT_WINDOW", False);

static GLuint _background;

void (DynamicBrush::*updateBrush)();

DynamicBrush::DynamicBrush(Mode mode, QObject *parent) : QObject(parent), _mode(mode), _glContext(0)
{
   _timerBgWipe = new QTimer(this);
   connect (_timerBgWipe, SIGNAL(timeout()), this, SLOT(wipeBackground()));
   switch (_mode)
   {
   case Tiled: updateBrush = &DynamicBrush::updateBrushTiled; break;
   case XRender: updateBrush = &DynamicBrush::updateBrushRender; break;
   case QtGradient: updateBrush = &DynamicBrush::updateBrushQt; break;
   case OpenGL:
      updateBrush = &DynamicBrush::updateBrushGL;
      _glContext = new QGLWidget( QGLFormat::defaultFormat(), 0 );
      initGL();
      break;
   case Tiled2: updateBrush = &DynamicBrush::updateBrushEdMetal; break;
   case VerticalGradient: updateBrush = &DynamicBrush::updateBrushVerticalGradient; break;
   case HorizontalGradient: updateBrush = &DynamicBrush::updateBrushHorizontalGradient; break;
   }
}

DynamicBrush::DynamicBrush(OXPixmap pixmap, OXPixmap shadow, int bgYoffset, QObject *parent) : QObject(parent), _pixmap(pixmap), _shadow(shadow), _bgYoffset(bgYoffset), _mode(Tiled), _glContext(0)
{
   _timerBgWipe = new QTimer(this);
   connect (_timerBgWipe, SIGNAL(timeout()), this, SLOT(wipeBackground()));
   updateBrush = &DynamicBrush::updateBrushTiled;
}

DynamicBrush::DynamicBrush(const QImage &leftCenter, const QImage &leftTile, QObject *parent) : QObject(parent), _mode(Tiled2),_glContext(0)
{
   _timerBgWipe = new QTimer(this);
   connect (_timerBgWipe, SIGNAL(timeout()), this, SLOT(wipeBackground()));
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

DynamicBrush::~DynamicBrush()
{
   glDeleteLists( _background, 1 );
//    QObject::~QObject();
}

void DynamicBrush::setMode(Mode mode)
{
   if (mode == _mode) return;
   if (_mode == OpenGL)
   {
      glDeleteLists( _background, 1 );
      delete _glContext;
      _glContext = 0L;
   }
   _mode = mode;
   switch (_mode)
   {
   case Tiled: updateBrush = &DynamicBrush::updateBrushTiled; break;
   case XRender: updateBrush = &DynamicBrush::updateBrushRender; break;
   case QtGradient: updateBrush = &DynamicBrush::updateBrushQt; break;
   case OpenGL:
      updateBrush = &DynamicBrush::updateBrushGL;
      if (!_glContext)
      {
         _glContext = new QGLWidget( QGLFormat::defaultFormat(), 0 );
         initGL();
      }
      break;
   case Tiled2: updateBrush = &DynamicBrush::updateBrushEdMetal; break;
   case VerticalGradient: updateBrush = &DynamicBrush::updateBrushVerticalGradient; break;
   case HorizontalGradient: updateBrush = &DynamicBrush::updateBrushHorizontalGradient; break;
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
   
   _timerBgWipe->start(7777); // the timer will wipe the background, if it's not updated since 7.777 secs (maybe the window is completely covered)

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
   _timerBgWipe->stop();
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

void DynamicBrush::updateBrushVerticalGradient()
{
   QPixmap qPix(32, _size.height());
   QColor c(QApplication::palette().color(QPalette::Window));
   qPix.fill(c);
   int h_8 = qPix.height()/8;
   QLinearGradient lg(QPoint(0,0), QPoint(0, h_8));
   lg.setColorAt(0, c.dark(105)); lg.setColorAt(1, c);
   QPainter p(&qPix);
   p.fillRect(0,0,32,h_8,lg);
   lg = QLinearGradient(QPoint(0,qPix.height()-h_8), QPoint(0, qPix.height()-1));
   lg.setColorAt(0, c); lg.setColorAt(1, c.light(105));
   p.fillRect(0,qPix.height()-h_8,32,h_8,lg);
   SETBACKGROUND(qPix);
}

void DynamicBrush::updateBrushHorizontalGradient()
{
   QPixmap qPix(_size.width(), 32);
   QColor c(QApplication::palette().color(QPalette::Window));
   qPix.fill(c);
   QColor cl = c.light(105);
   int w_8 = qPix.width()/8;
   QLinearGradient lg(QPoint(0,0), QPoint(w_8, 0));
   lg.setColorAt(0, cl); lg.setColorAt(1, c);
   QPainter p(&qPix);
   p.fillRect(0,0,w_8,32,lg);
   lg = QLinearGradient(QPoint(qPix.width()-w_8,0), QPoint(qPix.width()-1, 0));
   lg.setColorAt(0, c); lg.setColorAt(1, cl);
   p.fillRect(qPix.width()-w_8,0,w_8,32,lg);
   SETBACKGROUND(qPix);
}


void DynamicBrush::updateBrushEdMetal()
{
   if (!_glShadow.isNull())
      _glShadow = QPixmap();
   
   Display *dpy = QX11Info::display();
   int width = _size.width();
   int height = _center[0][0].height();
   float factor = MIN(1.0, ((float)width)/(2*_tile[0][0].width()+2*_center[0][0].width()));
   int wt = (int) (factor * _tile[0][0].width());
   int wc = (int) (factor * _center[0][0].width());
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
      XCopyArea( dpy, _tile[0][0].handle(), qPix.handle(), gc,
                 0, 0, wt, height, i, 0 );
   if (i != 0)
   {
      i = -i+1;
      XCopyArea( dpy, _tile[0][0].handle(), qPix.handle(), gc,
                  _tile[0][0].width()-i, 0, i, height, 0, 0 );
   }
   
   // left center ==================
   XCopyArea( dpy, _center[0][0].handle(), qPix.handle(), gc,
              0, 0, wc, height, x, 0 );
   x += wc;
   // right center ==================
   XCopyArea( dpy, _center[0][1].handle(), qPix.handle(), gc,
              _center[0][1].width() - wc, 0, wc, height, x, 0 );
   x += wc;
   // right tile ==================
   for (i = x; i < width-wt; i+=wt)
      XCopyArea( dpy, _tile[0][1].handle(), qPix.handle(), gc,
                 0, 0, wt, height, i, 0 );
   if (i != width)
      XCopyArea( dpy, _tile[0][1].handle(), qPix.handle(), gc,
                  0, 0, width-i, height, i, 0 );
   
   XFreeGC ( dpy , gc );
   
   /* update  the brush textures*/
   SETBACKGROUND(qPix);
}

#include <QTime>
#define _PROFILESTART_ QTime timer; int time; timer.start();
#define _PROFILERESTART_ timer.restart();
#define _PROFILESTOP_(_STRING_) time = timer.elapsed(); qDebug("%s: %d",_STRING_,time);

void DynamicBrush::initGL()
{
   _glContext->makeCurrent();
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_DEPTH_TEST);
   _glContext->doneCurrent();
//    glNewList( _background, GL_COMPILE );
// 
//    glEndList();
}

QPixmap DynamicBrush::glPixmap(const QRect &rect, int darkness)
{
   // so we'll use false colors here, avoiding image swap orgies
# if __BYTE_ORDER == __BIG_ENDIAN // OpenGL talks RGBA; Qt wants ARGB, do sth. about read type, need ppc arch for testing
#define _COLORS_ &a,&r,&g,&b
   double r,g,b,a;
#else // OpenGL talks ABGR; Qt wants ARGB
#define _COLORS_ &b,&g,&r
   double r,g,b;
#endif
   if (darkness)
      QApplication::palette().color(QPalette::Window).dark(100+darkness).getRgbF(_COLORS_);
   else
      QApplication::palette().color(QPalette::Window).getRgbF(_COLORS_);
   _glContext->makeCurrent();
   int width = 128, height = 128; // texture_2d must be 2^n, we start with 2^7 to save some time here ;)
   while (width < _size.width()) width = (width<<1);
   while (height < _size.height()) height = (height<<1);

//    if (!QGLFramebufferObject::hasOpenGLFramebufferObjects())
//    {
//       qWarning("ARRGHHH... system doesn't support framebuffers!?!");
//       return QPixmap();
//    }
   
   QGLFramebufferObject *buffer = new QGLFramebufferObject(width, height );
   if (!buffer->isValid())
   {
      qWarning("ARRGHHH... failed to create a texture buffer!");
      return QPixmap();
   }
   if (!buffer->bind())
   {
      qWarning("ARRGHHH... failed to bind gl rendering to texture buffer!");
      return QPixmap();
   }
   
   glViewport(0, 0, (GLsizei) _size.width(), (GLsizei) _size.height());
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
//    gluOrtho2D (0.0, 30.0, 30.0, 0.0); //lrbt
   GLfloat fw = 30.0/_size.width(), fh = 30.0/_size.height();
   // the statement is vertically mirrored, as is the pbuffer to a qimage
   // we simply avoid a software mirror and paint "mirrored" in gl (no one will see ;)
   glOrtho( rect.x()*fw, rect.right()*fw, rect.y()*fh, rect.bottom()*fh, -1, 1 );
   glMatrixMode(GL_MODELVIEW);
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
      glVertex2f (15.0, 30.0);
      glVertex2f (30.0, 5.0);
   glEnd();
   glBegin (GL_TRIANGLE_FAN);
      r *= 1.1; g *= 1.1; b *=1.1;
      glColor4f (r, g, b, .8);
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
//    glCallList(_background);
   glFlush();
   
   /* ================================================================
      Hi complex...

      the qimage -> qpixmap conversion is slow,
      so we try to avoid it by using knowledge of the special
      structure of the image

      #1. the upper 5/30 mirror horizontal, mirroring a pixmap is faster than
      double conversion (at least for big images, small are not so important)

      #2. the lower left tiles horizontally, so we convert only 32xheight pixles
      and tile them together as pixmaps

      #3. only the lower right is unique - so we need to convert it completely

      the thing gets worse as we need to be able to extract parts for
      eg. the groupbox shadows... i hate my life ;P
   
      IF ANYONE EVER TOUCHES THIS CODE AND THERE IS "glExtPixmapFromTexture" OR SIMILAR:
      THIS WOULD BE BY FAR BETTER!!!
   ================================================================ */

   QPixmap pix(rect.size());
   int w_2 = (_size.width()+1)/2;
   int yOff = 5*_size.height()/30;
   int rlw = w_2 - rect.x();
   int rYOff = yOff-rect.y();
   int w,h;
   QPixmap tmp;
   QPainter p(&pix);
   
   // top --------------------
   if (rect.y() < yOff)
   {
      w = MAX(w_2-rect.x(), rect.right()-w_2+1);
      h = yOff-rect.y();
      QImage *top = new QImage(w, h, QImage::Format_RGB32);
      glReadPixels(w_2-w, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, top->bits());
      tmp = QPixmap::fromImage(*top, Qt::ColorOnly|Qt::OrderedDither);
      p.drawPixmap(rect.x(),0, tmp, rect.x(),0,w_2-rect.x(),-1);
      tmp = mirror(tmp, Qt::Horizontal);
      p.drawPixmap(w_2,0, tmp,0,0,rect.right()-w_2+1,-1);
      delete top;
   }
   // bottom left --------------------
   if (rlw > 0)
   {
      h = rect.bottom() - yOff + 1;
      QImage *btmLeft = new QImage(32,h, QImage::Format_RGB32);
      glReadPixels(0, yOff, 32, h, GL_RGBA, GL_UNSIGNED_BYTE, btmLeft->bits());
      tmp = QPixmap::fromImage(*btmLeft, Qt::ColorOnly|Qt::OrderedDither);
      p.drawTiledPixmap(0,rYOff,rlw,h, tmp);
      delete btmLeft;
   }
   // bottom right --------------------
   w_2 = (_size.width()-1)/2;
   w = rect.right() - w_2;
   if (w > 0)
   {
      h = rect.bottom() - yOff + 1;
      QImage *btmRight = new QImage(w,h, QImage::Format_RGB32);
      glReadPixels(w_2, yOff, w, h, GL_RGBA, GL_UNSIGNED_BYTE, btmRight->bits());
      tmp = QPixmap::fromImage(*btmRight, Qt::ColorOnly|Qt::OrderedDither);
      delete btmRight;
      p.drawPixmap(rlw,rYOff, tmp);
   }
   p.end();
   delete buffer;
   return pix;
}

void DynamicBrush::updateBrushGL()
{
   QPixmap qPix = glPixmap(QRect(QPoint(0,0),_size));
   SETBACKGROUND(qPix);
}

void DynamicBrush::updateBrushRender()
{
   QColor c = QApplication::palette().color(QPalette::Window);
   QPixmap qPix(_size);
   Display *dpy = QX11Info::display();
   ColorArray ca = ColorArray() << c << c.dark(110);
   OXPicture lgp = OXRender::gradient(QPoint(0,0), QPoint(0,qPix.height()), ca);
   XRenderComposite (dpy, PictOpSrc, lgp, None, qPix.x11PictureHandle(),
                     0, 0, 0, 0, 0, 0, qPix.width(), qPix.height());
   XRenderFreePicture(dpy, lgp);
   ca.replace(0, QColor(255,255,255,180)); ca.replace(1, QColor(255,255,255,0));
   float f[2] = {
         pow(0.5, (float)qPix.height()/qPix.width()),
         pow(0.5, (float)qPix.width()/qPix.height())};
   lgp = OXRender::gradient(QPoint(qPix.width(), qPix.height()),
                            QPoint(qPix.width()*f[0], qPix.height()*f[1]), ca);
   XRenderComposite (dpy, PictOpOver, lgp, None, qPix.x11PictureHandle(),
                     0, 0, 0, 0, 0, 0, qPix.width(), qPix.height());
   XRenderFreePicture(dpy, lgp);
   int h = qPix.height()/6;
   int r = -(pow(qPix.width(),2)/4+pow(h,2))/(-2*h);
   lgp = OXRender::gradient(QPoint(qPix.width()/2, 0), 0,
                            QPoint(qPix.width()/2,h-r), r, ca);
   XRenderComposite (dpy, PictOpOver, lgp, None, qPix.x11PictureHandle(),
                     0, 0, 0, 0, 0, 0, qPix.width(), qPix.height());
   XRenderFreePicture(dpy, lgp);
   SETBACKGROUND(qPix);
}

void DynamicBrush::updateBrushQt()
{
   QColor c = QApplication::palette().color(QPalette::Window);
   QPixmap qPix(_size);
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
   SETBACKGROUND(qPix);
}

QPixmap DynamicBrush::shadow(const QRect &rect)
{
   switch (_mode)
   {
   case Tiled2:
   {
      QPixmap qPix(rect.size());
      int width = _size.width();
      int height = rect.height();
      float factor = MIN(1.0, ((float)width)/(2*_tile[0][0].width()+2*_center[0][0].width()));
      int wt = (int) (factor * _tile[0][0].width());
      int wc = (int) (factor * _center[0][0].width());
      int x = width/2 - wc;
      QPainter p(&qPix);
      //left
      p.drawTiledPixmap(-rect.x(), 0, x, height, _tile[1][0], x % wt);
      //left center
      x -= rect.x();
      p.drawTiledPixmap(x, 0, wc, height, _center[1][0]);
      // right center
      x += wc;
      p.drawTiledPixmap(x, 0, wc, height, _center[1][1], _center[1][0].width()-wc);
      // right
      x += wc;
      p.drawTiledPixmap(x, 0, rect.right()-x, height, _tile[1][1]);
      p.end();
      return qPix;
   }
   case Tiled:
   {
      QPixmap qPix(rect.size());
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
      return qPix;
   }
   case OpenGL:
      if (_glShadow.isNull() || _lastShadowRect != rect )
      {
         _lastShadowRect = rect;
         _glShadow = glPixmap(rect, 10);
      }
      return _glShadow;
   case QtGradient:
   case XRender:
      if (_glShadow.isNull() || _lastShadowRect != rect )
      {
         _glShadow = QPixmap(rect.size());
         _lastShadowRect = rect;
         QColor c = QApplication::palette().color(QPalette::Window).light(105);
         ColorArray ca = ColorArray() << c << c.dark(110);
         QPoint center(_glShadow.width()/2, _glShadow.height());
         int r = sqrt(pow(_glShadow.width()/2,2)+pow(_glShadow.height(),2));
         OXPicture rg = OXRender::gradient(center, 0, center, r, ca);
         XRenderComposite (QX11Info::display(), PictOpSrc, rg, None, _glShadow.x11PictureHandle(),
                           0, 0, 0, 0, 0, 0, _glShadow.width(), _glShadow.height());
         XRenderFreePicture(QX11Info::display(), rg);
      }
      return _glShadow;
   }
   return QPixmap();
}

#if 0 // this is some stored code - yet not used, but maybe later on

static bool eraseWidget(const QWidget *widget, const QPaintDevice *device, bool pseudoTransparent, const QRegion &clipRegion)
{
   const QWidget *bgWidget = (pseudoTransparent && widget->parentWidget()) ? widget->parentWidget() : widget;
   if (bgWidget->backgroundRole() == Qt::NoBackground || !widget->palette().brush( QPalette::Active, bgWidget->backgroundRole() ).texture().isNull())
      return false;
   
   OXPixmap _bgPx;
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

void DynamicBrush::setXPixmap(OXPixmap pixmap, OXPixmap shadow)
{
   if (pixmap != -1)
      _pixmap = pixmap;
   if (shadow != -1)
      _shadow = shadow;
}
// cause of cmake
#include "dynamicbrush.moc"

