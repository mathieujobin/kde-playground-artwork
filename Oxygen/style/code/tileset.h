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

#ifndef TILESET_H
#define TILESET_H

#include <QBitmap>
#include <QRegion>

class TileSet
{
public:
   enum Tile {TopLeft = 0, TopRight, BtmLeft, BtmRight, TopMid, MidLeft, MidRight, BtmMid, Center };
   TileSet(const QPixmap &pix, int xOff, int yOff, int width, int height,
           int top, int bottom, int left, int right, QRegion *topLeft = 0, QRegion *topRight = 0, QRegion *btmLeft = 0, QRegion *btmRight = 0);
   TileSet(){for (int i=0; i < 4; i++) border[i] = 0L;}
   ~TileSet()
   {
//       for (int i=0; i < 4; i++)
//          if (border[i]) delete border[i];
   }
   inline const QPixmap &pixmap( Tile pos ) const
   {
      return ( pixmaps[ ( int ) pos ] );
   }
   inline const QBitmap &mask( Tile pos ) const
   {
      return ( masks[ ( int ) pos ] );
   }
   inline int width(Tile pos) const
   {
      return pixmaps[pos].width();
   }
   inline int height(Tile pos) const
   {
      return pixmaps[pos].height();
   }
   inline QRegion *corner(Tile Pos) const
   {
      if (Pos < TopMid)
         return border[Pos];
      else
         return 0L;
   }
   const QRegion outerRegion(const QRect &r) const;
   const QRegion innerRegion(const QRect &r) const;
private:
   QPixmap pixmaps[ 9 ];
   QBitmap masks[9];
   QRegion *border[4];
   int size[4];
};
#endif //TILESET_H
