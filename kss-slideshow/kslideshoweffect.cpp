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

#include "kslideshoweffect.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

#include <KDebug>

KSlideShowEffect::KSlideShowEffect()
    : m_targetWidth( 0 )
    , m_targetHeight( 0 )
    , m_progress( 0.0 )
{
}

void KSlideShowEffect::initialize ( const QPixmap& currentImage, const QPixmap& nextImage )
{
    m_startingPixmap = currentImage;
    m_targetPixmap = nextImage;
    m_targetWidth = nextImage.width();
    m_targetHeight = nextImage.height();

    if ( m_startingPixmap.size() != m_targetPixmap.size() )
    {
        kWarning() << "Old pixmap size does not equal new pixmap size";
    }

    m_progress = 0.0;
    reset();
}

void KSlideShowEffect::resize ( const QPixmap& currentImage, const QPixmap& nextImage )
{
    QSize oldSize( m_targetWidth, m_targetHeight );

    m_startingPixmap = currentImage;
    m_targetPixmap = nextImage;
    m_targetWidth = nextImage.width();
    m_targetHeight = nextImage.height();

    if ( m_startingPixmap.size() != m_targetPixmap.size() )
    {
        kWarning() << "Old pixmap size does not equal new pixmap size";
    }

    resizeEvent( oldSize, m_targetPixmap.size() );
}

QRect KSlideShowEffect::advanceTo ( qreal newProgress )
{
    qreal oldProgress = m_progress;
    m_progress = newProgress;
    QRect changedArea;
    progressChangedEvent( oldProgress, newProgress, &changedArea );
    return changedArea;
}

void KSlideShowEffect::paint ( QPainter* p, QPaintEvent* event )
{
    if ( m_progress <= 0.0 )
    {
        QRect bound = event ? event->rect() : pixmapRect();
        p->drawPixmap( bound.topLeft(), m_startingPixmap, bound );
    }
    else if ( m_progress >= 1.0 )
    {
        QRect bound = event ? event->rect() : pixmapRect();
        p->drawPixmap( bound.topLeft(), m_targetPixmap, bound );
    }
    else
    {
        if ( event )
        {
            paintEvent( p, event );
        }
        else
        {
            QPaintEvent tempEvent( pixmapRect() );
            paintEvent( p, &tempEvent );
        }
    }
}

// vim: sw=4 sts=4 et tw=100
