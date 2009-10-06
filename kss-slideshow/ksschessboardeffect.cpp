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
#include "ksschessboardeffect.h"

KSlideShowChessBoardEffect::KSlideShowChessBoardEffect (
    QPaintDevice* device,
    QObject* parent = 0 )
        : KSlideShowEffect ( device, parent )
{
    setCurveShape ( QTimeLine::LinearCurve );
}

KSlideShowChessBoardEffect::init()
{
    int width = targetWidth();

    m_tilesPerRow = ( width + m_tileWidth - 1 ) / m_tileWidth;
    setFrameCount ( m_tilesPerRow );
    m_lastFrame = 0;

    miy = 0;         // 0 or mdy for growing tiling effect
    my  = mj & 1 ? 0 : mdy; // 0 or mdy for shrinking tiling effect
}

KSlideShowChessBoardEffect::nextStep ( QPainter* painter, int frame )
{
    while ( m_lastFrame < ( frame - 1 ) ) {
        m_lastFrame++;
        nextStep ( painter, m_lastFrame );
    }
    m_lastFrame = frame;

    const int rightSweepXOffset = frame * m_tileWidth;
    const int leftSweepXOffset = targetWidth() - ( rightSweepOffset );
    const int rightSweepYOffset = m_tileHeight * ( frame % 2 );
    const int leftSweepYOffset = m_tileHeight - rightSweepYOffset;

    QPainter p ( this );
    for ( y = 0; y < targetWidth(); y += ( m_tileHeight << 1 ) ) {
        p.drawPixmap ( rightSweepXOffset, y + rightSweepYOffset,
                       mNextScreen,
                       rightSweepXOffset, y + rightSweepYOffset,
                       m_tileWidth, m_tileHeight );
        p.drawPixmap ( leftSweepXOffset, y + leftSweepYOffset,
                       mNextScreen,
                       leftSweepXOffset, y + leftSweepYOffset,
                       m_tileWidth, m_tileHeight );
    }
}

#include "ksschessboardeffect.moc"

// vim: sw=4 sts=4 et tw=100
