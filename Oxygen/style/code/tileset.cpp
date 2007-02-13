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

#include <QPainter>
#include "tileset.h"

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

using namespace Tile;

static QPixmap nullPix;

Set::Set(const QPixmap &pix, int xOff, int yOff, int width, int height, int rx, int ry)
{
   if (pix.isNull())
   {
      _isBitmap = false;
      return;
   }
   _isBitmap = pix.isQBitmap();
   rxf = pix.width()*rx;
   ryf = pix.height()*ry;
   
   int rOff = pix.width() - xOff - width;
   int bOff = pix.height() - yOff - height;
   int amount = 32/width+1;
   int amount2 = 32/height+1;
   int i;
   
   QPainter p;
   
#define initPixmap(_SECTION_,_WIDTH_,_HEIGHT_)\
   pixmap[_SECTION_] = QPixmap(_WIDTH_, _HEIGHT_);\
   pixmap[_SECTION_].fill(Qt::transparent); p.begin(&pixmap[_SECTION_])
   
   initPixmap(TopLeft, xOff, yOff);
   p.drawPixmap(0, 0, pix, 0, 0, xOff, yOff);
   p.end();
   
   initPixmap(TopMid, amount*width, yOff);
   for (i = 0; i < amount; i++)
      p.drawPixmap(i*width, 0, pix, xOff, 0, width, yOff);
   p.end();
   
   initPixmap(TopRight, rOff, yOff);
   p.drawPixmap(0, 0, pix, xOff+width, 0, rOff, yOff);
   p.end();
   
   //----------------------------------
   initPixmap(MidLeft, xOff, amount2*height);
   for (i = 0; i < amount2; i++)
      p.drawPixmap(0, i*height, pix, 0, yOff, xOff, height);
   p.end();
   
   initPixmap(MidMid, amount*width, amount2*height);
   for (i = 0; i < amount; i++)
      for (int j = 0; j < amount2; j++)
         p.drawPixmap(i*width, j*height, pix, xOff, yOff, width, height);
   p.end();
   
   initPixmap(MidRight, rOff, amount2*height);
   for (i = 0; i < amount2; i++)
      p.drawPixmap(0, i*height, pix, xOff+width, yOff, rOff, height);
   p.end();
   
   //----------------------------------
   
   initPixmap(BtmLeft, xOff, bOff);
   p.drawPixmap(0, 0, pix, 0, yOff+height, xOff, bOff);
   p.end();
   
   initPixmap(BtmMid, amount*width, bOff);
   for (i = 0; i < amount; i++)
      p.drawPixmap(i*width, 0, pix, xOff, yOff+height, width, bOff);
   p.end();
   
   initPixmap(BtmRight, rOff, bOff);
   p.drawPixmap(0, 0, pix, xOff+width, yOff+height, rOff, bOff);
   p.end();
   
#undef initPixmap
}

QRect Set::rect(const QRect &rect, PosFlags pf) const
{
   QRect ret = rect;
   if (pf == Center)
      ret.adjust(width(MidLeft),height(TopMid),-width(TopMid),-height(BtmMid));
   else if (pf == Left)
      ret.setRight(ret.left()+width(MidLeft));
   else if (pf == Top)
      ret.setBottom(ret.top()+height(TopMid));
   else if (pf == Right)
      ret.setLeft(ret.right()-width(MidRight));
   else if (pf == Bottom)
      ret.setTop(ret.bottom()-height(BtmMid));
   return ret;
}

void Set::render(const QRect &r, QPainter *p, PosFlags pf) const
{
#define DRAW_PIXMAP p->drawPixmap
#define DRAW_TILED_PIXMAP p->drawTiledPixmap

#include "t_render1.cpp"
#undef DRAW_PIXMAP
#undef DRAW_TILED_PIXMAP
}

static Window root = RootWindow (QX11Info::display(), DefaultScreen (QX11Info::display()));

Picture Set::render(const QSize &s, PosFlags pf) const
{
   return render(s.width(), s.height(), pf);
}

Picture Set::render(int W, int H, PosFlags pf) const
{
   Display *dpy = QX11Info::display();
   QRect r(0,0,W,H);
   Pixmap xpix = XCreatePixmap (dpy, root, W, H, 32);
   if (!pixmap)
      return X::None;
   Picture pict = XRenderCreatePicture (dpy, xpix, XRenderFindStandardFormat (dpy, PictStandardARGB32), 0, 0);
   if (!pict)
   {
      XFreePixmap (dpy, xpix); return X::None;
   }
   
#define DRAW_PIXMAP(_DX_, _DY_, _PIX_, _SX_, _SY_, _W_, _H_) \
   XRenderComposite(dpy, PictOpSrc, _PIX_.x11PictureHandle(), X::None, pict, _SX_, _SY_, 0, 0, _DX_, _DY_, _W_, _H_);
//    XCopyArea( dpy, _PIX_.handle(), pix->handle(), gc, _SX_, _SY_, _W_, _H_, _DX_, _DY_ )
#define DRAW_TILED_PIXMAP(_DX_, _DY_, _W_, _H_, _PIX_)\
      for (int x = 0; x < _W_; x += _PIX_.width())\
         for (int y = 0; y < _H_; y += _PIX_.height())\
            XRenderComposite(dpy, PictOpSrc, _PIX_.x11PictureHandle(), X::None, pict,\
                             0, 0, 0, 0, x+_DX_, y+_DY_,\
                             MIN(_PIX_.width(), _W_-x), MIN(_PIX_.height(), _H_-y));

#include "t_render1.cpp"
   XFreePixmap (dpy, xpix);
   return pict;
#undef DRAW_PIXMAP
#undef DRAW_TILED_PIXMAP
}

void Set::outline(const QRect &r, QPainter *p, QColor c, PosFlags pf) const
{
   p->save();
   p->setRenderHint(QPainter::Antialiasing);
   QPen pen = p->pen();
   pen.setColor(c); pen.setWidth(0);
   p->setPen(pen);
   p->setBrush(Qt::NoBrush);
   p->drawRoundRect(r/*.adjusted(1,1,-1,-1)*/, rxf/r.width(), ryf/r.height());
   p->restore();
}

Tile::Mask::Mask(const QPixmap &pix, int xOff, int yOff, int width, int height,
          int dx1, int dy1, int dx2, int dy2, int rx, int ry): Set(pix, xOff, yOff, width, height, rx, ry)
{
   _dx[0] = dx1; _dx[1] = dx2; _dy[0] = dy1; _dy[1] = dy2;
   _hasCorners = !pix.isNull();
}

QRect Tile::Mask::bounds(const QRect &rect, PosFlags pf) const
{
   QRect ret = rect;
   if (pf & Left)
      ret.setLeft(ret.left()+_dx[0]);
   if (pf & Top)
      ret.setTop(ret.top()+_dy[0]);
   if (pf & Right)
      ret.setRight(ret.right()+_dx[1]);
   if (pf & Bottom)
      ret.setBottom(ret.bottom()+_dy[1]);
   return ret;
}


const QPixmap &Tile::Mask::corner(PosFlags pf) const
{
   if (pf == (Top | Left))
      return pixmap[TopLeft];
   if (pf == (Top | Right))
      return pixmap[TopRight];
   if (pf == (Bottom | Right))
      return pixmap[BtmRight];
   if (pf == (Bottom | Left))
      return pixmap[BtmLeft];
   
   qWarning("requested impossible corner %d",pf);
   return nullPix;
}

QRegion Tile::Mask::clipRegion(const QRect &rect, PosFlags pf) const
{
   QRegion ret(rect.adjusted(_dx[0], _dy[0], _dx[1], _dy[1]));
   QRect cRect; int w,h;
   if (matches(Top | Left, pf))
   {
      cRect = QRect(rect.x(), rect.y(), width(TopLeft), height(TopLeft));
      ret -= cRect;
   }
   if (matches(Top | Right, pf))
   {
      w = width(TopRight);
      cRect = QRect(rect.right()-w+1, rect.y(), w, height(TopRight));
      ret -= cRect;
   }
   if (matches(Bottom | Left, pf))
   {
      h = height(BtmLeft);
      cRect = QRect(rect.x(), rect.bottom()-h+1, width(BtmLeft), h);
      ret -= cRect;
   }
   if (matches(Bottom | Right, pf))
   {
      w = width(BtmRight); h = height(BtmRight);
      cRect = QRect(rect.right()-w+1, rect.bottom()-h+1, w, h);
      ret -= cRect;
   }
   return ret;
}

Line::Line(const QPixmap &pix, Qt::Orientation o, int d1, int d2)
{
   _o = o;
   QPainter p;
   if (o == Qt::Horizontal)
   {
      pixmap[0] = QPixmap(d1,pix.height());
      pixmap[0].fill(Qt::transparent);
      p.begin(&pixmap[0]);
      p.drawPixmap(0,0,pix,0,0,d1,pix.height());
      p.end();
      
      int d = pix.width()-d1+d2;
      pixmap[1] = QPixmap(MAX(32,d),pix.height());
      pixmap[1].fill(Qt::transparent);
      p.begin(&pixmap[1]);
      for (int i = 0; i+d <= width(1); i+=d)
         p.drawPixmap(i,0,pix,d1,0,MIN(d,width(1)-i),pix.height());
      p.end();
      
      pixmap[2] = QPixmap(-d2,pix.height());
      pixmap[2].fill(Qt::transparent);
      p.begin(&pixmap[2]);
      p.drawPixmap(0,0,pix,pix.width()+d2,0,-d2,pix.height());
      p.end();
   }
   else
   {
      pixmap[0] = QPixmap(pix.width(),d1);
      pixmap[0].fill(Qt::transparent);
      p.begin(&pixmap[0]);
      p.drawPixmap(0,0,pix,0,0,pix.width(),d1);
      p.end();
      
      int d = pix.height()-d1+d2;
      pixmap[1] = QPixmap(pix.width(),MIN(32,d));
      pixmap[1].fill(Qt::transparent);
      p.begin(&pixmap[1]);
      for (int i = 0; i+d < height(1); i+=d)
         p.drawPixmap(0,i,pix,0,d1,pix.width(),MIN(d,height(1)-i));
      p.end();
      
      pixmap[2] = QPixmap(pix.width(),-d2);
      pixmap[2].fill(Qt::transparent);
      p.begin(&pixmap[2]);
      p.drawPixmap(0,0,pix,0,pix.height()+d2,pix.width(),-d2);
      p.end();
   }
}

void Line::render(const QRect &rect, QPainter *p, PosFlags pf) const
{
   int d0,d2;
   if (_o == Qt::Horizontal)
   {
      if (rect.width() >= width(0)+width(2))
      {
         p->drawTiledPixmap(rect.x()+width(0), rect.y(),
                            rect.width()-width(0)-width(2), height(1),
                            pixmap[1]);
         d0 = width(0); d2 = width(2);
      }
      else
      {
         d0 = width(0)*rect.width()/(width(0)+width(2));
         d2 = rect.width()-d0;
      }
      p->drawPixmap(rect.x(),rect.y(),pixmap[0],0,0,d0,height(0));
      p->drawPixmap(rect.right()+1-d2,rect.y(),pixmap[2],width(2)-d2,0,d2,height(2));
   }
   else
   {
      if (rect.height() >= height(0)+height(2))
      {
         p->drawTiledPixmap(rect.x(), rect.y()+height(0),
                            width(1), rect.height()-height(0)-height(2),
                            pixmap[1]);
         d0 = height(0); d2 = height(2);
      }
      else
      {
         d0 = height(0)*rect.height()/(height(0)+height(2));
         d2 = rect.height()-d0;
      }
      p->drawPixmap(rect.x(),rect.y(),pixmap[0],0,0,width(0),d0);
      p->drawPixmap(rect.x(),rect.bottom()+1-d2,pixmap[2],0,height(2)-d2,width(2),d2);
   }
}

#define FILL_PIX(_NUM_)\
   pixmap[_NUM_].fill(Qt::transparent);\
   p.begin(&pixmap[_NUM_]);\
   p.fillRect(0,0,pixmap[_NUM_].width(),pixmap[_NUM_].height(),lg);\
   p.end()

#define MAKE_ARC(_NUM_,_CLR_,_X_,_Y_,_ANG_)\
      pixmap[_NUM_] = QPixmap(4,4);\
      pixmap[_NUM_].fill(Qt::transparent);\
      p.begin(&pixmap[_NUM_]);\
      p.setRenderHint(QPainter::Antialiasing);\
      p.setPen(QColor(_CLR_,_CLR_,_CLR_,_alpha));\
      p.drawArc(_X_,_Y_,8,8,_ANG_<<4,90<<4);\
      p.setRenderHint(QPainter::Antialiasing, false);\
      p.end()

#define SET_GRAD(_C1_,_C2_)\
   lg.setColorAt ( 0, QColor(_C1_,_C1_,_C1_,_alpha) );\
   lg.setColorAt ( 1, QColor(_C2_,_C2_,_C2_,_alpha) )

Nuno::Nuno(int alpha)
{

   _alpha = alpha;
   QPainter p;
   MAKE_ARC(0,255,0,0,90); //tl
   MAKE_ARC(1,212,-4,0,0); //tr
   MAKE_ARC(2,100,-4,-4,270); //br
   MAKE_ARC(3,215,0,-4,180); //bl

   p.setPen(Qt::NoPen);
   QLinearGradient lg( 0, 1, 37, 1 );
   
   //bottom 1
   SET_GRAD(175,138);
   pixmap[4] = QPixmap(37,1);
   FILL_PIX(4);

   //bottom 2
   SET_GRAD(136,100);
   pixmap[5] = QPixmap(37,1);
   FILL_PIX(5);

   //right 1
   lg.setStart(1,0); lg.setFinalStop(1,31);
   SET_GRAD(170,139);
   pixmap[6] = QPixmap(1,31);
   FILL_PIX(6);

   //right 2
   SET_GRAD(137,107);
   pixmap[7] = QPixmap(1,31);
   FILL_PIX(7);
}

#undef FILL_PIX
#undef MAKE_ARC

void Nuno::render(const QRect &r, QPainter *p, PosFlags pf) const
{
   int xOff = 0, yOff = 0, rOff = 0, bOff = 0;
   
   if (matches(Top|Left, pf))
   {
      p->drawPixmap(r.x(),r.y(),pixmap[0]);
      xOff = width(0); yOff = height(0);
   }
   
   if (matches(Top|Right, pf))
   {
      p->drawPixmap(r.right()-width(1)+1,r.y(),pixmap[1]);
      rOff = width(1); yOff = height(1);
   }
   
   if (matches(Bottom|Left, pf))
   {
      p->drawPixmap(r.x(),r.bottom()-height(3)+1,pixmap[3]);
      xOff = width(3); bOff = height(3);
   }
   
   if (matches(Bottom|Right, pf))
   {
      p->drawPixmap(r.right()-width(2)+1,r.bottom()-height(2)+1,pixmap[2]);
      rOff = width(2); bOff = height(2);
   }
   
   p->save();
   if (pf & Top)
   {
      p->setPen(QColor(255,255,255,_alpha));
      p->drawLine(r.x()+xOff, r.y(), r.right()-rOff, r.y());
   }
   if (pf & Left)
   {
      p->setPen(QColor(255,255,255,_alpha));
      p->drawLine(r.x(), r.y()+yOff, r.x(), r.bottom()-bOff);
   }
   if (pf & Bottom)
   {
      int w = r.width()-xOff-rOff;
      if (w < width(4)+width(5))
      {
         QHash<int, QPixmap> *cache = &(const_cast<Nuno*>( this )->_lines[0]);
         QHash<int, QPixmap>::const_iterator it;
         it = cache->find(w);
         if (it == cache->end())
         {
            QPixmap pix(w,1); pix.fill(Qt::transparent);
            QLinearGradient lg( 0, 1, w, 1 );
            lg.setColorAt ( 0, QColor(175,175,175,_alpha) );
            lg.setColorAt ( 1, QColor(100,100,100,_alpha) );
            QPainter p(&pix); p.fillRect(0,0,w,1,lg); p.end();
            it = cache->insert(w, pix);
         }
         p->drawPixmap(r.x()+xOff,r.bottom(),it.value());
      }
      else
      {
         p->drawPixmap(r.x()+xOff,r.bottom(),pixmap[4]);
         p->drawPixmap(r.right()-rOff-width(5),r.bottom(),pixmap[5]);
      }
      if (r.width() > xOff+width(4)+width(5)+rOff)
      {
         p->setPen(QColor(137,137,137,_alpha));
         p->drawLine(r.x()+xOff+width(4),r.bottom(),r.right()-rOff-width(5),r.bottom());
      }
   }
   if (pf & Right)
   {
      int h = r.height()-yOff-bOff;
      if (h < height(6)+height(7))
      {
         QHash<int, QPixmap> *cache = &(const_cast<Nuno*>( this )->_lines[1]);
         QHash<int, QPixmap>::const_iterator it;
         it = cache->find(h);
         if (it == cache->end())
         {
            QPixmap pix(1,h);
            QLinearGradient lg( 1, 0, 1, h ); pix.fill(Qt::transparent);
            lg.setColorAt ( 0, QColor(170,170,170,_alpha) );
            lg.setColorAt ( 1, QColor(107,107,107,_alpha) );
            QPainter p(&pix); p.fillRect(0,0,1,h,lg); p.end();
            it = cache->insert(h, pix);
         }
         p->drawPixmap(r.right(),r.y()+yOff,it.value());
      }
      else
      {
         p->drawPixmap(r.right(),r.y()+yOff,pixmap[6]);
         p->drawPixmap(r.right(),r.bottom()-bOff-height(7),pixmap[7]);
      }
      if (r.height() > yOff+height(6)+height(7)+bOff)
      {
         p->setPen(QColor(138,138,138,_alpha));
         p->drawLine(r.right(),r.y()+yOff+height(6),r.right(),r.bottom()-bOff-height(7));
      }
   }
   p->restore();
}
