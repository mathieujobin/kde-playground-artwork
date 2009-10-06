/*
 * Copyright 1999 Stefan Taferner <taferner@kde.org>
 * Copyright 2001 Martin R. Jones <mjones@kde.org>
 * Copyright 2003 Sven Leiber <s.leiber@web.de>
 * Copyright 2009 Alex Merry <alex.merry@kdemail.net>
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


#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QImage>
#include <QPixmap>
#include <QStringList>
#include <QTimeLine>
#include <QTimer>

#include <kscreensaver.h>

#include "kslideshoweffect.h"

class kSlideShowSaver: public KScreenSaver
{
    Q_OBJECT
public:
    kSlideShowSaver ( WId id );
    virtual ~kSlideShowSaver();

    void readConfig();

    void restart();

protected:
    void paintEvent( QPaintEvent * );

private:
    /** Load list of images from directory */
    void loadDirectory();

    /** Helper for loadDirectory() */
    void traverseDirectory ( const QString &dirName );

    /** Load next image from list. If the file cannot be read
         it is automatically removed from the file list.
          mImage contains the image after loading. */
    void loadNextImage();

    /** Set loaded image to next-screen buffer. */
    void createNextScreen();

private slots:
    void effectFinished();
    void effectValueChanged( qreal newValue );
    void startTransition();

private:
    struct ScreenGeo {
        ScreenGeo( int w, int h, int x, int y ) : mW ( w ), mH ( h ), mXorg ( x ), mYorg ( y ) {};
        int mW;
        int mH;
        int mXorg;
        int mYorg;
    };
    QList<ScreenGeo> mGeoList;
    QTimer mNextSlideTime;
    QStringList mFileList;
    QStringList mRandomList;
    int mFileIdx;
    QImage mImage;
    QPixmap mNextScreen;
    QString mImageName;
    QList<KSlideShowEffect*> mEffects;
    KSlideShowEffect* mActiveEffect;
    QTimeLine mEffectTimeLine;

    // config settings:
    bool mShowRandom;
    bool mZoomImages;
    bool mPrintName;
    bool mSubdirectory;
    bool mRandomPosition;
    int mDelay;
    QString mDirectory;
};


#endif /*SLIDESHOW_H*/

