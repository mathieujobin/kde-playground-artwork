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

#ifndef KSLIDESHOWEFFECT_H
#define KSLIDESHOWEFFECT_H

#include <QPixmap>
#include <QRect>

class QPainter;
class QPaintEvent;

class KSlideShowEffect
{
public:
    KSlideShowEffect();

    virtual ~KSlideShowEffect() {}

    /**
     * Ready the effect to be used.
     *
     * Must be called each time the transition effect will be used.
     *
     * @p currentImage and @p nextImage must be the same size, and must
     * be the same size as the QPainter's paint device for paint().
     *
     * @param currentImage  the image being transitioned from
     * @param nextImage     the image being transitioned to
     */
    void initialize( const QPixmap& currentImage, const QPixmap& nextImage );

    /**
     * Advances the effect progress.
     *
     * @p progress should always be greater than or equal to the value
     * at the previous call.
     *
     * @param progress  the new progress position; must be 0 to 1 inclusive
     * @return          the area of the paint device that will need updating
     */
    QRect advanceTo( qreal progress );

    /**
     * Updates the effect when the paint device size has changed.
     *
     * This should be triggered when the paintable area changes while
     * the effect is still transitioning.
     *
     * As with initialize(), @p currentImage and @p nextImage should be
     * the same size as the QPainter's paint device for paint().
     *
     * @param currentImage  the resized image being transitioned from
     * @param nextImage     the resized image being transitioned to
     */
    void resize( const QPixmap& currentImage, const QPixmap& nextImage );

    /**
     * Paints the current frame of the effect.
     *
     * @param painter     the painter to use
     * @param paintEvent  provides information about the area that needs updating;
     *                    if not given, the whole area will be updated
     */
    void paint( QPainter* painter, QPaintEvent* paintEvent = 0 );

protected:
    virtual void reset() = 0;
    virtual void progressChangedEvent( qreal oldProgress, qreal newProgress, QRect* changedArea ) = 0;
    virtual void paintEvent( QPainter* painter, QPaintEvent* paintEvent ) = 0;
    virtual void resizeEvent( const QSize& oldSize, const QSize& newSize ) = 0;

    QPixmap m_targetPixmap;
    QPixmap m_startingPixmap;
    int m_targetWidth;
    int m_targetHeight;
    qreal m_progress;

private:
    QRect pixmapRect();
};

inline QRect KSlideShowEffect::pixmapRect()
{
    return QRect( 0, 0, m_targetWidth, m_targetHeight );
}

#endif // KSLIDESHOWEFFECT_H
