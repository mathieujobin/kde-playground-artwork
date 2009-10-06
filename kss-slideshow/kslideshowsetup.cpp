/*
 * Copyright 1999 Stefan Taferner <taferner@kde.org>
 * Copyright 2001 Martin R. Jones <mjones@kde.org>
 * Copyright 2003 Chris Howells <howells@kde.org>
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

#include "kslideshowsetup.h"

#include "slideshow.h"
#include "ui_slideshowcfg.h"

#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QSpinBox>


#include <kaboutapplicationdialog.h>
#include <KAboutData>
#include <KConfig>
#include <KDebug>
#include <KFileDialog>

class SlideShowCfg : public QWidget, public Ui::SlideShowCfg
{
public:
    SlideShowCfg ( QWidget *parent = 0L ) : QWidget ( parent ) {
        setupUi ( this );
    }
};


//=============================================================================
//  Class kSlideShowSetup
//=============================================================================
kSlideShowSetup::kSlideShowSetup ( KAboutData *aboutData, QWidget *aParent )
        : KDialog ( aParent )
        , mAboutData( aboutData )
{
    setCaption ( i18n ( "Setup Slide Show Screen Saver" ) );
    setButtons ( Ok | Cancel | Help );
    setDefaultButton ( Ok );
    setModal ( true );
    setButtonText ( Help, i18n ( "A&bout" ) );

    QWidget *main = new QWidget ( this );
    setMainWidget ( main );
    cfg = new SlideShowCfg();
    cfg->setupUi ( main );

    cfg->mPreview->setFixedSize ( 220, 170 );
    cfg->mPreview->show();    // otherwise saver does not get correct size
    mSaver = new kSlideShowSaver ( cfg->mPreview->winId() );

    cfg->mDirChooser->setMode ( KFile::Directory | KFile::ExistingOnly );
    connect ( cfg->mDirChooser, SIGNAL ( returnPressed ( const QString & ) ),
              SLOT ( slotDirSelected ( const QString & ) ) );
    connect ( cfg->mDirChooser, SIGNAL ( urlSelected ( const KUrl & ) ),
              SLOT ( slotDirSelected ( const KUrl & ) ) );
    connect ( this, SIGNAL ( okClicked() ), this, SLOT ( slotOk() ) );
    connect ( this, SIGNAL ( helpClicked() ), this, SLOT ( slotHelp() ) );
    readSettings();
}

kSlideShowSetup::~kSlideShowSetup()
{
    delete mSaver;
}

//-----------------------------------------------------------------------------
void kSlideShowSetup::readSettings()
{
    KConfigGroup config ( KGlobal::config(), "Settings" );

    cfg->mCbxRandom->setChecked ( config.readEntry ( "ShowRandom", true ) );
    cfg->mCbxZoom->setChecked ( config.readEntry ( "ZoomImages", false ) );
    cfg->mCbxShowName->setChecked ( config.readEntry ( "PrintName", true ) );
    cfg->mDelay->setValue ( config.readEntry ( "Delay", 20 ) );
    cfg->mDirChooser->setUrl ( config.readPathEntry ( "Directory", QString() ) );
    cfg->mCbxSubdirectory->setChecked ( config.readEntry ( "SubDirectory", false ) );
    cfg->mCbxRandomPosition->setChecked ( config.readEntry ( "RandomPosition", false ) );
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::writeSettings()
{
    KConfigGroup config ( KGlobal::config(), "Settings" );

    config.writeEntry ( "ShowRandom", cfg->mCbxRandom->isChecked() );
    config.writeEntry ( "ZoomImages", cfg->mCbxZoom->isChecked() );
    config.writeEntry ( "PrintName",  cfg->mCbxShowName->isChecked() );
    config.writeEntry ( "Delay", cfg->mDelay->value() );
    config.writePathEntry ( "Directory", cfg->mDirChooser->url().path() );
    config.writeEntry ( "SubDirectory", cfg->mCbxSubdirectory->isChecked() );
    config.writeEntry ( "RandomPosition", cfg->mCbxRandomPosition->isChecked() );

    config.sync();

    if ( mSaver ) {
        mSaver->readConfig();
        mSaver->restart();
    }
}


//-----------------------------------------------------------------------------

void kSlideShowSetup::slotDirSelected ( const KUrl & )
{
    writeSettings();
}

void kSlideShowSetup::slotDirSelected ( const QString & )
{
    writeSettings();
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotOk()
{
    writeSettings();
    accept();
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotHelp()
{
    KAboutApplicationDialog mAbout ( mAboutData, this );
    mAbout.exec();
}

#include "kslideshowsetup.moc"

