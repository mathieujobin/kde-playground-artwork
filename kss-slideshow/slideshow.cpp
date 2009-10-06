/* Slide Show Screen Saver
 *  (C) 1999 Stefan Taferner <taferner@kde.org>
 *  (C) 2001 Martin R. Jones <mjones@kde.org>
 *  (C) 2003 Chris Howells <howells@kde.org>
 *  (C) 2003 Sven Leiber <s.leiber@web.de>
 *
 * This code is under GPL
 *
 * 2001/03/04 Converted to libkscreensaver by Martin R. Jones.
 */

#include "slideshow.h"

#include "kslideshowsetup.h"

#include <QColor>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPolygon>


#include <kaboutdata.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kimageio.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <krandom.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>


#define SLIDESHOW_VERSION "2.3.0"
static const char version[] = SLIDESHOW_VERSION;
static const char description[] = I18N_NOOP ( "KSlideshow" );

static KAboutData* s_aboutData = 0;

// libkscreensaver interface
class KSlideShowSaverInterface : public KScreenSaverInterface
{
public:
    virtual KAboutData* aboutData() {
        return s_aboutData;
    }

    virtual KScreenSaver* create ( WId id ) {
        return new kSlideShowSaver ( id );
    }

    virtual QDialog* setup() {
        return new kSlideShowSetup( s_aboutData );
    }
};

int main ( int argc, char *argv[] )
{
    s_aboutData = new KAboutData ( "kslideshow.kss", 0, ki18n ( "SlideShow" ),
                                   version, ki18n ( description ), KAboutData::License_GPL,
                                   ki18n ( "(c) 1999-2009, The KDE Team" ) );
    s_aboutData->addAuthor ( ki18n ( "Stefan Taferner" ), KLocalizedString(), "taferner@kde.org" );
    s_aboutData->addAuthor ( ki18n ( "Chris Howells" ), KLocalizedString(), "howells@kde.org" );
    s_aboutData->addAuthor ( ki18n ( "Sven Leiber" ), KLocalizedString(), "s.leiber@web.de" );

    KSlideShowSaverInterface kss;
    return kScreenSaverMain ( argc, argv, kss );
}


//=============================================================================
//  Class kSlideShowSaver
//=============================================================================
kSlideShowSaver::kSlideShowSaver( WId id )
    : KScreenSaver( id )
    , mFileIdx( 0 )
    , mActiveEffect( 0 )
{
    /*
    registerEffects();
    */

    readConfig();

    mEffectTimeLine.setDuration( 1500 );
    mEffectTimeLine.setCurveShape( QTimeLine::LinearCurve );
    connect( &mEffectTimeLine, SIGNAL( finished() ), SLOT( effectFinished() ) );
    connect( &mEffectTimeLine, SIGNAL( valueChanged( qreal ) ), SLOT( effectValueChanged( qreal ) ) );

    mNextSlideTime.setSingleShot( true );
    connect( &mNextSlideTime, SIGNAL( timeout() ), SLOT( startTransition() ) );
    mNextSlideTime.start( 10 );

    QDesktopWidget *d = QApplication::desktop();
    if ( geometry() == d->geometry() && d->numScreens() > 1 ) {
        for ( int i = 0; i < d->numScreens(); ++i ) {
            QRect s = d->screenGeometry ( i );
            mGeoList.append ( ScreenGeo ( s.width(), s.height(), s.topLeft().x(), s.topLeft().y() ) );
        }
    } else {
        mGeoList.append ( ScreenGeo ( width(), height(), 0, 0 ) );
    }
    setAttribute ( Qt::WA_NoSystemBackground );
    createNextScreen();
}


//----------------------------------------------------------------------------
kSlideShowSaver::~kSlideShowSaver()
{
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::readConfig()
{
    KConfigGroup config ( KGlobal::config(), "Settings" );
    mShowRandom = config.readEntry ( "ShowRandom", true );
    mZoomImages = config.readEntry ( "ZoomImages", false );
    mPrintName = config.readEntry ( "PrintName", true );
    mDirectory = config.readPathEntry ( "Directory", KGlobal::dirs()->findDirs ( "wallpaper", "" ).last() );
    mDelay = config.readEntry ( "Delay", 10 ) * 1000;
    mSubdirectory = config.readEntry ( "SubDirectory", false );
    mRandomPosition = config.readEntry ( "RandomPosition", false );

    loadDirectory();
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::restart()
{
    mActiveEffect = 0;
    update();
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::startTransition()
{
    kDebug() << "Moving to the next slide";
    mActiveEffect = 0;

    QPixmap currentScreen = mNextScreen;
    loadNextImage();
    createNextScreen();

    if ( mEffects.count() )
    {
        if ( mEffects.count() == 1 )
        {
            mActiveEffect = mEffects[0];
        }
        else
        {
            int pos = KRandom::random() % mEffects.count();
            mActiveEffect = mEffects[KRandom::random() % pos];
        }
        mActiveEffect->initialize( currentScreen, mNextScreen );
        mEffectTimeLine.start();
    }
    else
    {
        mNextSlideTime.start( mDelay );
        update();
    }
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::paintEvent ( QPaintEvent * event )
{
    QPainter p ( this );
    if ( mActiveEffect )
    {
        mActiveEffect->paint( &p, event );
    }
    else
    {
        p.drawPixmap ( 0, 0, mNextScreen, 0, 0, mNextScreen.width(), mNextScreen.height() );
    }
}

void kSlideShowSaver::effectFinished()
{
    kDebug() << "Effect finished";
    mActiveEffect = 0;
    mNextSlideTime.start( mDelay );
}

void kSlideShowSaver::effectValueChanged( qreal newValue )
{
    if ( mActiveEffect )
    {
        QRect updateArea = mActiveEffect->advanceTo( newValue );
        update( updateArea );
    }
}



//----------------------------------------------------------------------------
void kSlideShowSaver::createNextScreen()
{
    QPainter p;
    int ww, wh, iw, ih, x, y;
    double fx, fy;

    if ( mNextScreen.size() != size() )
        mNextScreen = QPixmap ( size() );

    mNextScreen.fill ( Qt::black );

    p.begin ( &mNextScreen );

    foreach ( const ScreenGeo& geo, mGeoList ) {
        loadNextImage();

        iw = mImage.width();
        ih = mImage.height();
        ww = geo.mW;
        wh = geo.mH;

        if ( mFileList.isEmpty() ) {
            p.setPen ( QColor ( "white" ) );
            p.drawText ( 20 + ( KRandom::random() % ( ww >> 1 ) ), 20 + ( KRandom::random() % ( wh >> 1 ) ),
                         i18n ( "No images found" ) );
        } else {
            if ( mZoomImages ) {
                fx = ( double ) ww / iw;
                fy = ( double ) wh / ih;
                if ( fx > fy ) fx = fy;
                if ( fx > 2 ) fx = 2;
                iw = ( int ) ( iw * fx );
                ih = ( int ) ( ih * fx );
                QImage scaledImg = mImage.scaled ( iw, ih,
                                                   Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

                x = ( ( ww - iw ) >> 1 ) + geo.mXorg;
                y = ( ( wh - ih ) >> 1 ) + geo.mYorg;

                p.drawImage ( x, y, scaledImg );
            } else {
                if ( iw >= ww || ih >= wh ) {
                    fx = ( double ) ww / iw;
                    fy = ( double ) wh / ih;
                    if ( fx > fy ) fx = fy;
                    if ( fx > 2 ) fx = 2;
                    iw = ( int ) ( iw * fx );
                    ih = ( int ) ( ih * fx );
                    QImage scaledImg = mImage.scaled ( iw, ih,
                                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

                    x = ( ( ww - iw ) >> 1 ) + geo.mXorg;
                    y = ( ( wh - ih ) >> 1 ) + geo.mYorg;

                    p.drawImage ( x, y, scaledImg );
                } else {
                    if ( mRandomPosition ) {
                        x = ( KRandom::random() % ( ww - iw ) ) + geo.mXorg;
                        y = ( KRandom::random() % ( wh - ih ) ) + geo.mYorg;
                    } else {
                        x = ( ( ww - iw ) >> 1 ) + geo.mXorg;
                        y = ( ( wh - ih ) >> 1 ) + geo.mYorg;
                    }

                    // bitBlt(&mNextScreen, x, y, &mImage, 0, 0, iw, ih);
                    p.drawImage ( x, y, mImage );
                }
            }

            if ( mPrintName ) {
                p.setPen ( QColor ( "black" ) );
                for ( x = 9; x <= 11; x++ )
                    for ( y = 21; y >= 19; y-- )
                        p.drawText ( x + geo.mXorg, wh - y + geo.mYorg, mImageName );
                p.setPen ( QColor ( "white" ) );
                p.drawText ( 10 + geo.mXorg, wh - 20 + geo.mYorg, mImageName );
            }
        }
    }
    p.end();
}


//----------------------------------------------------------------------------
void kSlideShowSaver::loadNextImage()
{
    QString fname;
    int num;

nexttry:

    num = mFileList.count();
    if ( num <= 0 ) { //no files in the directory
        return;
    }

    if ( mShowRandom ) {
        mFileIdx = KRandom::random() % num;
        fname = mFileList.takeAt ( mFileIdx );
        if ( num == 1 ) { //we're about to run out of images
            mFileList = mRandomList;
        }
    } else {
        if ( mFileIdx >= num ) mFileIdx = 0;
        fname = mFileList[mFileIdx];
    }

    if ( !mImage.load ( fname ) ) {
        kDebug() << "Failed to load image " << fname;
        mFileList.removeAll ( fname );
        mRandomList.removeAll ( fname );
        goto nexttry;
    }
    mFileIdx++;

    mImageName = QFileInfo ( fname ).baseName();
    kDebug() << "Loaded" << mImageName;
}


//----------------------------------------------------------------------------
void kSlideShowSaver::loadDirectory()
{
    mFileIdx = 0;
    mFileList.clear();
    traverseDirectory ( mDirectory );
    mRandomList = mFileList;
}

void kSlideShowSaver::traverseDirectory ( const QString &dirName )
{
    QDir dir ( dirName );
    if ( !dir.exists() ) {
        return ;
    }
    dir.setFilter ( QDir::Dirs | QDir::Files );

    QFileInfoList fileinfolist = dir.entryInfoList();
    QFileInfoList::Iterator it = fileinfolist.begin();
    while ( it != fileinfolist.end() ) {
        if ( it->fileName() == "." || it->fileName() == ".." ) {
            ++it;
            continue;
        }
        if ( it->isDir() && it->isReadable() && mSubdirectory ) {
            traverseDirectory ( it->filePath() );
        } else {
            if ( !it->isDir() ) {
                mFileList.append ( it->filePath() );
            }
        }
        ++it;
    }
}


#include "slideshow.moc"

