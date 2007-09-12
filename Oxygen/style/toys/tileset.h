/*
 * Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef TILESET_H
#define TILESET_H

#include <QtGui/QPixmap>
#include <QtCore/QRect>

class TileSet
{
public:
    TileSet() {}
    TileSet(const QPixmap &pix, int w1, int h1, int w2, int h2);
    TileSet(const TileSet&);

    virtual ~TileSet() {}

    TileSet& operator=(const TileSet&);

    void render(const QRect &rect, QPainter &p) const;

protected:
    void initPixmap(int s, const QPixmap&, int w, int h, const QRect &region);
    QPixmap _pixmap[9];
    int _w1, _w3, _h1, _h3;
};

#endif //TILESET_H
