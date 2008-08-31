///////////////////////////////////////////////////////////////////////
// -------------------
// DeKorator window decoration for KDE
// -------------------
// Copyright (C) 2005  moty rahamim <moty.rahamim@gmail.com>
//
// Based on Example-0.8, some ideas and code have been taken from plastik and KCMicons
//
// Example window decoration for KDE
// Copyright (c) 2003, 2004 David Johnson <david@usermode.org>
//
// Plastik KWindowSystem window decoration
// Copyright (C) 2003-2005 Sandro Giessl <sandro@giessl.com>
//
// KCMicons for KDE
// Copyright (c) 2000 Antonio Larrosa <larrosa@kde.org>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.
///////////////////////////////////////////////////////////////////////

//#include <config.h>

#include <stdlib.h>
#include <unistd.h>

#include <qfileinfo.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QPixmap>

#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kservice.h>
#include <klocale.h>
#include <ksimpleconfig.h>
#undef Unsorted

#include <k3listview.h>
#include <kurlrequesterdlg.h>
#include <kmessagebox.h>
#include <kprogressdialog.h>
#include <kiconloader.h>

#include <kio/job.h>
#include <kio/netaccess.h>
#include <ktar.h>
/*
#ifdef HAVE_LIBART
#include <ksvgiconengine.h>
#endif
*/
#include <qdir.h>
#include <qpainter.h>
#include <qregexp.h>

#include "themes.h"
#include <kurlrequester.h>


//////////////////////////////////////////////////////////////////////////////
// IconThemesConfig()
// ----------
//

IconThemesConfig::IconThemesConfig( QWidget *parent, K3ListView *themesView /*, QPushButton *removethemeBtn*/ )
{
    parent_ = parent;
    themesView_ = themesView;
    //removeThemeBtn_ = removethemeBtn;

    loadThemes();
}

//////////////////////////////////////////////////////////////////////////////
// ~IconThemesConfig()
// ----------
//

IconThemesConfig::~IconThemesConfig()
{}

//////////////////////////////////////////////////////////////////////////////
// iconThemeItem()
// ----------
//

Q3ListViewItem *IconThemesConfig::iconThemeItem( const QString &name )
{
    Q3ListViewItem * item;
    for ( item = themesView_->firstChild(); item ; item = item->nextSibling() )
        if ( item->text( 0 )  == name ) return item;

    return 0L;
}

//////////////////////////////////////////////////////////////////////////////
// loadThemes()
// ----------
//


void IconThemesConfig::loadThemes()
{
    themesView_->clear();
    m_themeNames.clear();
    QStringList themelist;
    QString name;
    QString place;
    QStringList::Iterator it, itj;
    QStringList themesDirs;

    // if no local deKorator folders
    if ( ! QDir( QDir::homePath() + "/.kde/share/apps/" ).exists( "deKorator" ) )
    {
        qWarning( "no dirs, will make them..." );
        KIO::mkdir( QDir::homePath() + "/.kde/share/apps/deKorator/" );
        KIO::mkdir( QDir::homePath() + "/.kde/share/apps/deKorator/themes/" );
    }
    else
    {
        qWarning( "exist" );
    }

    themesDirs = KGlobal::dirs() ->findDirs( "data", "deKorator/themes" ) ;

    for ( it = themesDirs.begin(); it != themesDirs.end(); ++it )
    {
        QDir dir = QDir( *it );
        themelist = dir.entryList( QString("*-theme").split(';') );

        for ( itj = themelist.begin(); itj != themelist.end(); ++itj )
        {
            place = *it;

            name = ( QString ) * itj;

            themesView_->insertItem( new Q3ListViewItem( themesView_, name ) );

            m_themeNames.insert( name, place );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// installNewTheme()
// ----------
//

void IconThemesConfig::installNewTheme()
{

    KUrl themeURL = KUrlRequesterDialog::getUrl( QString::null, parent_,
                    i18n( "Drag or Type Theme URL" ) );
    kDebug() << themeURL.prettyUrl() << endl;

    if ( themeURL.url().isEmpty() )
        return ;

    // themeTmpFile contains the name of the downloaded file
    QString themeTmpFile;

    if ( !KIO::NetAccess::download( themeURL, themeTmpFile, parent_ ) )
    {
        QString sorryText;
        if ( themeURL.isLocalFile() )
            sorryText = i18n( "Unable to find the deKorator theme archive %1." );
        else
            sorryText = i18n( "Unable to download deKorator theme archive;\n"
                              "please check that address %1 is correct." );
        KMessageBox::sorry( parent_, sorryText.arg( themeURL.prettyUrl() ) );
        return ;
    }

    //
    QStringList themesNames = findThemeDirs( themeTmpFile );
    if ( themesNames.isEmpty() )
    {
        QString invalidArch( i18n( "The file is not a valid deKorator theme archive." ) );
        KMessageBox::error( parent_, invalidArch );

        KIO::NetAccess::removeTempFile( themeTmpFile );
        return ;
    }

    if ( !installThemes( themesNames, themeTmpFile ) )
    {
        //FIXME: make me able to know what is wrong....
        // QStringList instead of bool?
        QString somethingWrong =
            i18n( "A problem occurred during the installation process; "
                  "however, most of the themes in the archive have been installed" );
        KMessageBox::error( parent_, somethingWrong );
    }

    KIO::NetAccess::removeTempFile( themeTmpFile );

    loadThemes();

    QString cur = themesNames.at( 0 );

    Q3ListViewItem *item = iconThemeItem( cur );
    themesView_->setSelected( item, true );
}

//////////////////////////////////////////////////////////////////////////////
// installThemes()
// ----------
//

bool IconThemesConfig::installThemes( const QStringList &themes, const QString &archiveName )
{
    bool everythingOk = true;
    QString localThemesDir = QDir::homePath() + "/.kde/share/apps/deKorator/themes/";
    //
    KProgressDialog progressDiag( parent_);//, "themeinstallprogress",
                                /*  i18n( "Installing icon themes" ),
                                  QString::null,
                                  true );*/
    progressDiag.setAutoClose( true );
    progressDiag.progressBar() ->setRange(0, themes.count() );
    progressDiag.show();

    KTar archive( archiveName );
    archive.open( QIODevice::ReadOnly );
    kapp->processEvents();

    const KArchiveDirectory* rootDir = archive.directory();

    KArchiveDirectory* currentTheme;
    for ( QStringList::ConstIterator it = themes.begin(); it != themes.end(); ++it )
    {
        progressDiag.setLabelText(
            i18n( "<qt>Installing <strong>%1</strong> theme</qt>" )
            .arg( *it ) );
        kapp->processEvents();

        if ( progressDiag.wasCancelled() )
            break;

        currentTheme = dynamic_cast<KArchiveDirectory*>(
                           const_cast<KArchiveEntry*>(
                               rootDir->entry( *it ) ) );
        if ( currentTheme == NULL )
        {
            // we tell back that something went wrong, but try to install as much
            // as possible
            everythingOk = false;
            continue;
        }

        currentTheme->copyTo( localThemesDir + *it );
	progressDiag.progressBar() ->setValue(progressDiag.progressBar() ->value() + 1 );
    }

    archive.close();
    return everythingOk;
}

//////////////////////////////////////////////////////////////////////////////
// findThemeDirs()
// ----------
//

QStringList IconThemesConfig::findThemeDirs( const QString &archiveName )
{
    QStringList foundThemes;

    KTar archive( archiveName );
    archive.open( QIODevice::ReadOnly );
    const KArchiveDirectory* themeDir = archive.directory();

    KArchiveEntry* possibleDir = 0L;
    KArchiveDirectory* subDir = 0L;

    // iterate all the dirs looking for an index.theme or index.desktop file
    QStringList entries = themeDir->entries();
    for ( QStringList::Iterator it = entries.begin(); it != entries.end(); ++it )
    {
        QString name = ( QString ) * it;
        QRegExp rxp = QRegExp( "*-theme", Qt::CaseSensitive, QRegExp::Wildcard );
        bool i = name.contains( rxp );
        if ( !i )
        {
        //    continue;
        }

        possibleDir = const_cast<KArchiveEntry*>( themeDir->entry( *it ) );
        if ( possibleDir->isDirectory() )
        {
            subDir = dynamic_cast<KArchiveDirectory*>( possibleDir );
            if ( subDir && ( subDir->entry( "deco" ) != NULL && subDir->entry( "buttons" ) != NULL && subDir->entry( "masks" ) != NULL ) )
            {
                foundThemes.append( subDir->name() );
                //qWarning( "found" );
            }
        }
    }

    archive.close();

    return foundThemes;
}

//////////////////////////////////////////////////////////////////////////////
// removeSelectedTheme()
// ----------
//

void IconThemesConfig::removeSelectedTheme()
{
    Q3ListViewItem * selected = themesView_->selectedItem();
    if ( !selected )
        return ;

    QString question = i18n( "<qt>Are you sure you want to remove the "
                             "<strong>%1</strong> theme?<br>"
                             "<br>"
                             "This will delete the files installed by this theme.</qt>" ).
                       arg( selected->text( 0 ) );

    int r = KMessageBox::warningContinueCancel( parent_, question, i18n( "Confirmation" ), KStandardGuiItem::del() );
    if ( r != KMessageBox::Continue ) return ;

    QString delTheme = selected->text( 0 );
    QString deldirStr = QDir::homePath() + "/.kde/share/apps/deKorator/themes/" + delTheme;
    QDir dir = QDir( deldirStr );
    dir.rename( deldirStr, deldirStr + "del" );

    KIO::NetAccess::del( KUrl( deldirStr + "del" ), parent_ );

    loadThemes();
}

//////////////////////////////////////////////////////////////////////////////
// themeSelected()
// ----------
//

void IconThemesConfig::themeSelected( Q3ListViewItem *item, QLabel *previewLabel, QPushButton *btn )
{
    QPixmap topLeftCornerBg, leftButtonsBg, leftTitleBg, midTitleBg, rightTitleBg, rightButtonsBg, topRightCornerBg;
    QPainter painter;
    int w, h, x;
    QString dirName( m_themeNames[ item->text( 0 ) ] );

    if ( dirName.contains( "home" ) )
    {
        //qWarning("true");
        btn->setEnabled( true );
    }
    else
    {
        btn->setEnabled( false );
        //qWarning("false");
    }

    QString dirNameStr = dirName + item->text( 0 ) + "/deco/" ;
    topLeftCornerBg.load( dirNameStr + "topLeftCornerBg.png" );
    leftButtonsBg.load( dirNameStr + "leftButtonsBg" );
    leftTitleBg.load( dirNameStr + "leftTitleBg.png" );
    midTitleBg.load( dirNameStr + "midTitleBg.png" );
    rightTitleBg.load( dirNameStr + "rightTitleBg.png" );
    rightButtonsBg.load( dirNameStr + "rightButtonsBg.png" );
    topRightCornerBg.load( dirNameStr + "topRightCornerBg.png" );

    w = topLeftCornerBg.width() + leftButtonsBg.width() + leftTitleBg.width() + midTitleBg.width() + rightTitleBg.width() + rightButtonsBg.width() + topRightCornerBg.width() + 8;
    h = midTitleBg.height() + 8;
    QPixmap previewPix(w, h);
    //qWarning("%d",w);
    painter.begin( &previewPix );

    // paint outer rect
    painter.fillRect( 0, 0, w, h, QColor( 20, 20, 20 ) );

    painter.setPen( QColor( 60, 60, 60 ) );
    painter.drawRect( 0, 0, w , h );

    painter.setPen( QColor( 80, 80, 80 ) );
    painter.drawRect( 1, 1, w - 2 , h - 2 );

    painter.setPen( QColor( 180, 180, 180 ) );
    painter.drawLine( w - 1, 1, w - 1, h - 1 );
    painter.drawLine( 1, h - 1, w - 2, h - 1 );

    painter.setPen( QColor( 150, 150, 150 ) );
    painter.drawLine( w - 2, 2, w - 2, h - 2 );
    painter.drawLine( 2, h - 2, w - 3, h - 2 );


    // paint deco tiles
    x = 4;
    painter.drawPixmap( x, 4, topLeftCornerBg );

    x += topLeftCornerBg.width();
    painter.drawPixmap( x, 4, leftButtonsBg );

    x += leftButtonsBg.width();
    painter.drawPixmap( x , 4, leftTitleBg );

    x += leftTitleBg.width();
    painter.drawPixmap( x, 4, midTitleBg );

    x += midTitleBg.width();
    painter.drawPixmap( x, 4, rightTitleBg );

    x += rightTitleBg.width();
    painter.drawPixmap( x, 4, rightButtonsBg );

    x += rightButtonsBg.width();
    painter.drawPixmap( x, 4, topRightCornerBg );

    painter.setPen( QColor( 0, 0, 0 ) );
    painter.drawLine( 4, h - 5, w - 4 , h - 5 );

    painter.end();

    previewLabel->resize( w, h );
    previewLabel->setPixmap( previewPix );
}

//////////////////////////////////////////////////////////////////////////////
// setTheme()
// ----------
//

void IconThemesConfig::setTheme( KUrlRequester *framesPath, KUrlRequester *buttonsPath, KUrlRequester *masksPath )
{
    Q3ListViewItem * selected = themesView_->selectedItem();
    if ( !selected )
        return ;

    QString dirName( m_themeNames[ selected->text( 0 ) ] );

    QString setTheme = selected->text( 0 );
    QString setThemeStr = dirName + setTheme;

    framesPath->setUrl( setThemeStr + "/deco" );
    buttonsPath->setUrl( setThemeStr + "/buttons" );
    masksPath->setUrl( setThemeStr + "/masks" );
}




#include "themes.moc"