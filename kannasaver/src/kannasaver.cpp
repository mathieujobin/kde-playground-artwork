/***************************************************************************
 *   Copyright 2004 Mathias Homann <Mathias.Homann@eregion.de              *
 *   Copyright 2009 Frederik Schwarzer <schwarzerf@gmail.com>              *
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

#include "kannasaver.h"
#include "setupdlg.h"
#include "kana.h"
#include "settings.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kconfigdialog.h>
#include <kglobal.h>
#include <klocale.h>

#include <QCheckBox>
#include <QColor>
#include <QFontDatabase>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QPainter>
#include <QRadioButton>

#include <stdlib.h>
#include <time.h>

#ifdef DEBUG
#include <iostream.h>
#endif

static KAboutData *s_aboutData = 0;

class KannasaverInterface : public KScreenSaverInterface
{
    public:
        virtual KAboutData *aboutData() {
            return s_aboutData;
        }

        /** function to create screen saver object */
        virtual KScreenSaver *create( WId id )
        {
            return new Kannasaver( id );
        }

        /** function to create setup dialog for screen saver */
        virtual QDialog *setup()
        {
            return new SetupDlg();
        }
};


int main( int argc, char *argv[] )
{
    s_aboutData = new KAboutData( "kannasaver.kss", 0,
                                ki18n("Kannasaver"), "1.2",
                                ki18n("A screensaver that shows Japanese characters."),
                                KAboutData::License_GPL,
                                ki18n("Copyright 2004 Mathias Homann<br/>Copyright 2009 Frederik Schwarzer"),
                                KLocalizedString(),
                                "foo" );
    s_aboutData->addAuthor( ki18n("Mathias Homann"),
                            ki18n("Original author of Kannasaver."),
                            "Mathias.Homann@eregion.de");
    s_aboutData->addAuthor( ki18n("Frederik Schwarzer"),
                            ki18n("Port to KDE4"),
                            "schwarzerf@gmail.com");

    KannasaverInterface kss;
    return kScreenSaverMain( argc, argv, kss );
}



Kannasaver::Kannasaver( WId id ) : KScreenSaver( id )
{
    QFontDatabase fdb;
    // FIXME: Is there a way to tell katakana and hiragana apart here? (schwarzer)
    QStringList KatakanaFonts(fdb.families(QFontDatabase::Japanese));
    QStringList HiraganaFonts(fdb.families(QFontDatabase::Japanese));

    // find all fonts that have both charsets, and are smoothly scalable
    for(QStringList::Iterator s = HiraganaFonts.begin(); s != HiraganaFonts.end(); ++s)
    		if(fdb.isSmoothlyScalable(*s) && !(KatakanaFonts.filter(*s).empty())) {
#ifdef DEBUG
			cout << *s << endl; //
#endif
			UsableFontList+=*s;
		};


    srand(time(0));
    readSettings();
    blank();
}

Kannasaver::~Kannasaver()
{}


//! read configuration settings from config file
void Kannasaver::readSettings()
{
    KConfigDialog* dialog = new KConfigDialog( this, "settings", Settings::self() );
}


void Kannasaver::blank()
{

    if(UsableFontList.empty()) {
    	QMessageBox mb(tr2i18n("Kannasaver"), tr2i18n("There are no usable (That is, capable of Hiragana or Katakana, and\n soft-scalable) fonts on your system. You must get and install\nsome before using this screensaver. SuSE comes with several\nsuitable fonts, you just have to install them in yast. Users\nof other Linux distributions should google for one of the following:\n\"Kochi Gothic\" or \"Baekmuk Gulim\"."),
	QMessageBox::Critical,QMessageBox::Ok, Qt::NoButton, Qt::NoButton);
	if(mb.exec()==QMessageBox::Ok) exit(-1);
    }

    kanaFont = new QFont(KanaFontName);

    // use the systemwide default font for the romaji
    romajiFont = new QFont();

    int myheight = height();

    // kana are shown in 1/4 of screen height
    kanaFont->setPixelSize(myheight/4);

    // romaji are shown in 1/10 of screen height
    romajiFont->setPixelSize(myheight/10);

#ifdef DEBUG
    cout << "Kana Font: " << kanaFont->family() << endl ;
    cout << "Romaji Font: " << romajiFont->family() << endl ;
#endif

    //schwarzer: just temporarily -- setBackgroundColor( QColor(Qt::black) );
    //schwarzer: just temporarily -- erase();
    counter=0;

    the_timer = new QTimer();
    connect(the_timer, SIGNAL(timeout()), SLOT(draw_kana()));
    the_timer->start(5000);
}


/*!
    \fn kannasaver::draw_kana()
 */
void Kannasaver::draw_kana()
{
    QPainter paint(this);

    int mywidth = width();
    int myheight = height();
    bool style=(CharSetToUse==2?rand()%2:CharSetToUse);
    counter = (int) rand()%(SaverMode==2?104:(SaverMode==1?74:46));

#ifdef DEBUG
    cout  << "Stil: " << style << "; Char: " << kanatable[counter].pRomaji << " " ;
    cout << (style?kanatable[counter].pKatakana:kanatable[counter].pHiragana)<< endl;
#endif


    paint.setClipping(false);
    paint.setPen( Qt::white );
    paint.eraseRect(0,0,mywidth,myheight);

    paint.setFont(*kanaFont);
    paint.drawText(mywidth/3,myheight/3,
    		       mywidth/3,myheight/3,
                   Qt::AlignCenter,
		   (style?
		   	(QString::fromUtf8(kanatable[counter].pKatakana)):(QString::fromUtf8(kanatable[counter].pHiragana))));

    paint.setFont(*romajiFont);
    paint.drawText(3*(mywidth/4),3*(myheight/4),mywidth/4,myheight/4,
		   Qt::AlignCenter,
		   QString::fromUtf8(kanatable[counter].pRomaji, -1));


}


// vim: expandtab:tabstop=3:shiftwidth=3
// kate: space-indent on; indent-width 3