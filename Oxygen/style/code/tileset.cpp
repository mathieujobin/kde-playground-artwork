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

TileSet::TileSet(const QPixmap &pix, int xOff, int yOff, int width, int height, int top, int bottom, int left, int right, QRegion *topLeft, QRegion *topRight, QRegion *btmLeft, QRegion *btmRight)
{
   size[0] = top; size[1] = left; size[2] = bottom; size[3] = right;
   
   if (topLeft)
      border[TopLeft] = new QRegion(*topLeft);
   else
      border[TopLeft] = 0;
   if (topRight)
      border[TopRight] = new QRegion(*topRight);
   else
      border[TopRight] = 0;
   if (btmLeft)
      border[BtmLeft] = new QRegion(*btmLeft);
   else
      border[BtmLeft] = 0;
   if (btmRight)
      border[BtmRight] = new QRegion(*btmRight);
   else
      border[BtmRight] = 0;
   
   int rOff = pix.width() - xOff - width;
   int bOff = pix.height() - yOff - height;
   int amount = 32/width+1;
   int amount2 = 32/height+1;
   int i;
   
   QPainter p;
   QColor trans(0,0,0,0);
   
   pixmaps[TopLeft] = QPixmap(xOff, yOff);
   pixmaps[TopLeft].fill(trans); p.begin(&pixmaps[TopLeft]);
   p.drawPixmap(0, 0, pix, 0, 0, xOff, yOff);
   p.end();
//    masks[TopLeft] = alphaMask(pixmaps[TopLeft]);
//    masks[TopLeft].setMask(masks[TopLeft]);
   
   pixmaps[TopMid] = QPixmap(amount*width, yOff);
   pixmaps[TopMid].fill(trans); p.begin(&pixmaps[TopMid]);
   for (i = 0; i < amount; i++)
      p.drawPixmap(i*width, 0, pix, xOff, 0, width, yOff);
   p.end();
//    masks[TopMid] = alphaMask(pixmaps[TopMid]);
//    masks[TopMid].setMask(masks[TopMid]);
   
   pixmaps[TopRight] = QPixmap(rOff, yOff);
   pixmaps[TopRight].fill(trans); p.begin(&pixmaps[TopRight]);
   p.drawPixmap(0, 0, pix, xOff+width, 0, rOff, yOff);
   p.end();
//    masks[TopRight] = alphaMask(pixmaps[TopRight]);
//    masks[TopRight].setMask(masks[TopRight]);
   
   //----------------------------------
   pixmaps[MidLeft] = QPixmap(xOff, amount2*height);
   pixmaps[MidLeft].fill(trans); p.begin(&pixmaps[MidLeft]);
   for (i = 0; i < amount2; i++)
      p.drawPixmap(0, i*height, pix, 0, yOff, xOff, height);
   p.end();
//    masks[MidLeft] = alphaMask(pixmaps[MidLeft]);
//    masks[MidLeft].setMask(masks[MidLeft]);
   
      // TODO: this is a bit of waste as we assume a *frame* to be empty inside ;)
   pixmaps[Center] = QPixmap(amount*width, amount2*height);
   pixmaps[Center].fill(trans); p.begin(&pixmaps[Center]);
   for (i = 0; i < amount; i++)
      for (int j = 0; i < amount2; i++)
         p.drawPixmap(i*width, j*height, pix, xOff, yOff, width, height);
   p.end();
//    masks[Center] = alphaMask(pixmaps[Center]);
//    masks[Center].setMask(masks[Center]);
   pixmaps[MidRight] = QPixmap(rOff, amount2*height);
   pixmaps[MidRight].fill(trans); p.begin(&pixmaps[MidRight]);
   for (i = 0; i < amount2; i++)
      p.drawPixmap(0, i*height, pix, xOff+width, yOff, rOff, height);
   p.end();
//    masks[MidRight] = alphaMask(pixmaps[MidRight]);
//    masks[MidRight].setMask(masks[MidRight]);
   
   //----------------------------------
   
   pixmaps[BtmLeft] = QPixmap(xOff, bOff);
   pixmaps[BtmLeft].fill(trans); p.begin(&pixmaps[BtmLeft]);
   p.drawPixmap(0, 0, pix, 0, yOff+height, xOff, bOff);
   p.end();
//    masks[BtmLeft] = alphaMask(pixmaps[BtmLeft]);
//    masks[BtmLeft].setMask(masks[BtmLeft]);
   
   pixmaps[BtmMid] = QPixmap(amount*width, bOff);
   pixmaps[BtmMid].fill(trans); p.begin(&pixmaps[BtmMid]);
   for (i = 0; i < amount; i++)
      p.drawPixmap(i*width, 0, pix, xOff, yOff+height, width, bOff);
   p.end();
//    masks[BtmMid] = alphaMask(pixmaps[BtmMid]);
//    masks[BtmMid].setMask(masks[BtmMid]);
   
   pixmaps[BtmRight] = QPixmap(rOff, bOff);
   pixmaps[BtmRight].fill(trans); p.begin(&pixmaps[BtmRight]);
   p.drawPixmap(0, 0, pix, xOff+width, yOff+height, rOff, bOff);
   p.end();
//    masks[BtmRight] = alphaMask(pixmaps[BtmRight]);
//    masks[BtmRight].setMask(masks[BtmRight]);
}

const QRegion TileSet::innerRegion(const QRect &r) const
{
   QRegion tmpRegion;
   
   QRegion ret(r.x()+size[1], r.y()+size[0], r.width()-(size[1]+size[3]), r.height()-(size[0]+size[2]));

   if (border[TopLeft])
   {
      tmpRegion = *border[TopLeft];
      tmpRegion.translate(r.x(), r.y());
      ret = ret.subtract(tmpRegion);
   }
   if (border[BtmLeft])
   {
      tmpRegion = *border[BtmLeft];
      tmpRegion.translate(r.x(), r.bottom() - tmpRegion.boundingRect().height() + 1);
      ret = ret.subtract(tmpRegion);
   }
   if (border[TopRight])
   {
      tmpRegion = *border[TopRight];
      tmpRegion.translate(r.right() - tmpRegion.boundingRect().width() + 1, r.y());
      ret = ret.subtract(tmpRegion);
   }
   if (border[BtmRight])
   {
      tmpRegion = *border[BtmRight];
      tmpRegion.translate(r.right()-tmpRegion.boundingRect().width() + 1,
                          r.bottom()-tmpRegion.boundingRect().height() + 1);
      ret = ret.subtract(tmpRegion);
   }
   return ret;
}

const QRegion TileSet::outerRegion(const QRect &r) const
{
   QRegion tmpRegion;

   QRegion ret = QRegion(r).subtract(QRect(r.x()+size[1], r.y()+size[0], r.width()-(size[1]+size[3]), r.height()-(size[0]+size[2])));
   
   if (border[TopLeft])
   {
      tmpRegion = *border[TopLeft];
      tmpRegion.translate(r.x(), r.y());
      ret = ret.unite(tmpRegion);
   }
   if (border[BtmLeft])
   {
      tmpRegion = *border[BtmLeft];
      tmpRegion.translate(r.x(), r.bottom() - tmpRegion.boundingRect().height() + 1);
      ret = ret.unite(tmpRegion);
   }
   if (border[TopRight])
   {
      tmpRegion = *border[TopRight];
      tmpRegion.translate(r.right() - tmpRegion.boundingRect().width() + 1, r.y());
      ret = ret.unite(tmpRegion);
   }
   if (border[BtmRight])
   {
      tmpRegion = *border[BtmRight];
      tmpRegion.translate(r.right()-tmpRegion.boundingRect().width() + 1,
                          r.bottom()-tmpRegion.boundingRect().height() + 1);
      ret = ret.unite(tmpRegion);
   }
   return ret;
}
