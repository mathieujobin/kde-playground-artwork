/*
 * Copyright 2008  Alex Merry <alex.merry@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef KSSCHESSBOARDEFFECT_H
#define KSSCHESSBOARDEFFECT_H

#include "kslideshoweffect.h"

/**
 * Sweeps in from both sides in a chessboard effect, with the left sweep
 * filling the gaps left by the right sweep and vice-versa.
 */
class KSlideShowChessBoardEffect : public KSlideShowEffect
{
public:
    KSlideShowChessBoardEffect ( QPaintDevice* device, QObject* parent = 0 )
            : KSlideShowEffect ( device, parent ) {
    }
    ~KSlideShowChessBoardEffect() {}

    void init();

    void nextStep ( QPainter* painter, int frame );

private:
    static const int m_tileWidth = 8;
    static const int m_tileHeight = 8;

    int m_tilesPerRow;
    int m_lastFrame;
}

#endif // KSSCHESSBOARDEFFECT_H
