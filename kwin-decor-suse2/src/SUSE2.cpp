/* SuSE KWin window decoration
  Copyright (C) 2005 Gerd Fleischer <gerdfleischer@web.de>
  Copyright (C) 2005 Adrian Schroeter <adrian@suse.de>

  based on the window decoration "Plastik" and "Web":
  Copyright (C) 2003 Sandro Giessl <ceebx@users.sourceforge.net>
  Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
  Boston, MA  02110-1301  USA.
*/

//#include <kdebug.h>

#include <QPainter>
#include <QBitmap>
#include <QList>
#include <QPixmap>
#include <QApplication>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include "misc.h"
#include "shadow.h"
#include "SUSE2.h"
#include "SUSE2client.h"
#include "SUSE2button.h"

namespace KWinSUSE2
{

SUSE2Handler::SUSE2Handler()
{
    memset(m_pixmaps, 0, sizeof(QPixmap *) * NumButtonStatus * NumButtonIcons); // set elements to 0

//    KGlobal::locale()->insertCatalogue("kwin_clients");
//    KGlobal::locale()->insertCatalogue("kwin_SUSE2");

    reset(0);
}

SUSE2Handler::~SUSE2Handler()
{
    for (int t = 0; t < NumButtonStatus; ++t)
        for (int i = 0; i < NumButtonIcons; ++i)
            delete m_pixmaps[t][i];
}

bool SUSE2Handler::reset(unsigned long /*changed*/)
{
    // we assume the active font to be the same as the inactive font since the control
    // center doesn't offer different settings anyways.
    m_titleFont = KDecoration::options()->font(true, false); // not small
    m_titleFontTool = KDecoration::options()->font(true, true); // small

    switch(KDecoration::options()->preferredBorderSize( this )) {
        case BorderTiny:
            m_borderSize = 3;
            break;
        case BorderLarge:
            m_borderSize = 8;
            break;
        case BorderVeryLarge:
            m_borderSize = 12;
            break;
        case BorderHuge:
            m_borderSize = 18;
            break;
        case BorderVeryHuge:
            m_borderSize = 27;
            break;
        case BorderOversized:
            m_borderSize = 40;
            break;
        case BorderNormal:
        default:
            m_borderSize = 4;
    }

    // check if we are in reverse layout mode
    m_reverse = QApplication::isRightToLeft();

    // read in the configuration
    readConfig();

    for (int t = 0; t < NumButtonStatus; ++t) {
        for (int i = 0; i < NumButtonIcons; ++i) {
            if (m_pixmaps[t][i]) {
                delete m_pixmaps[t][i];
                m_pixmaps[t][i] = 0;
            }
        }
    }

    /// I dont want to do smart things here
    /// just for cases that do not happen very often.
    return true;
}

KDecoration *SUSE2Handler::createDecoration( KDecorationBridge *bridge )
{
        return (new SUSE2Client( bridge, this ))->decoration();
}

bool SUSE2Handler::supports( Ability ability ) const
{
    switch( ability )
    {
        case AbilityAnnounceButtons:
        case AbilityButtonMenu:
        case AbilityButtonOnAllDesktops:
        case AbilityButtonSpacer:
        case AbilityButtonHelp:
        case AbilityButtonMinimize:
        case AbilityButtonMaximize:
        case AbilityButtonClose:
        case AbilityButtonAboveOthers:
        case AbilityButtonBelowOthers:
        case AbilityButtonShade:
            return true;
        default:
            return false;
    };
}

void SUSE2Handler::readConfig()
{
    // create a config object
    KConfig configFile("kwinSUSE2rc");
    const KConfigGroup config(&configFile, "General");

    // grab settings
    QString alignValue = config.readEntry("TitleAlignment", "AlignLeft");
    if (alignValue == "AlignLeft")         m_titleAlign = Qt::AlignLeft;
    else if (alignValue == "AlignHCenter") m_titleAlign = Qt::AlignHCenter;
    else if (alignValue == "AlignRight")   m_titleAlign = Qt::AlignRight;

    QString roundValue = config.readEntry("RoundedCorners", "NotMaximized");
    if (roundValue == "RoundAlways")       m_roundCorners = 1;
    else if (roundValue == "NotMaximized") m_roundCorners = 2;
    else if (roundValue == "RoundNever")   m_roundCorners = 3;

    m_menuClose = config.readEntry("CloseOnMenuDoubleClick", true);
    m_titleShadow = config.readEntry("TitleShadow", true);

    QFontMetrics fm(m_titleFont);  // active font = inactive font
    int addSpace = config.readEntry("AddSpace", 4);
    // The title should stretch with bigger font sizes!
    m_titleHeight = qMax(16, fm.height() + addSpace);

    fm = QFontMetrics(m_titleFontTool);  // active font = inactive font
    m_titleHeightTool = qMax(13, fm.height() ); // don't care about the shadow etc.

    m_titlebarStyle = config.readEntry("TitleBarStyle", 0);

    m_buttonType = config.readEntry("TitleBarButtonType", 2);
    m_customColors = config.readEntry("CustomColors", false);
    m_useTitleProps = config.readEntry("UseTitleProps", false);
    m_animateButtons = config.readEntry("AnimateButtons", true);
    m_redCloseButton = config.readEntry("RedCloseButton", false);
    m_iconSize = (config.readEntry("IconSize", 45))/100.0;
    m_customIconColors = config.readEntry("CustomIconColors", false);
    QColor afgcolor = QColor(10, 20, 40);
    m_aFgColor = config.readEntry("AFgColor", afgcolor);
    QColor abgcolor = QColor(210, 220, 240);
    m_aBgColor = config.readEntry("ABgColor", abgcolor);
    QColor ifgcolor = QColor(40, 40, 40);
    m_iFgColor = config.readEntry("IFgColor", ifgcolor);
    QColor ibgcolor = QColor(240, 240, 240);
    m_iBgColor = config.readEntry("IBgColor", ibgcolor);
    m_iconShadow = config.readEntry("IconShadow", false);

    m_titleLogo      = config.readEntry("TitleBarLogo", false);
    m_titleLogoOffset = config.readEntry("TitleBarLogoOffset", 3);
    m_titleLogoURL   = config.readEntry("TitleBarLogoURL", KStandardDirs::locate("data", "kwin/pics/titlebar_decor.png"));
}

QColor SUSE2Handler::getColor(KWinSUSE2::ColorType type, const bool active) const
{
    switch (type) {
        case TitleGradientFrom:
            return KDecoration::options()->color(ColorTitleBar, active);
            break;
        case TitleGradientTo:
            return KDecoration::options()->color(ColorTitleBlend, active);
            break;
        case WindowContour:
            return KDecoration::options()->color(ColorTitleBar, active).dark(200);
            break;
        case TitleFont:
            return KDecoration::options()->color(ColorFont, active);
            break;
        case BtnBg:
            return KDecoration::options()->color(ColorButtonBg, active);
            break;
        default:
            return Qt::black;
    }
}

const QPixmap &SUSE2Handler::buttonPixmap(ButtonIcon type, int size, ButtonStatus status)
{
    if (m_pixmaps[status][type]) {
        if (status != Shadow && m_pixmaps[status][type]->size() == QSize(size, size))
            return *m_pixmaps[status][type];
        else if (status == Shadow && m_pixmaps[status][type]->size() == QSize(size+4, size+4))
            return *m_pixmaps[status][type];
    }

    // no matching pixmap found, create a new one...

    delete m_pixmaps[status][type];
    m_pixmaps[status][type] = 0;

    QColor aDecoFgDark = alphaBlendColors(getColor(TitleGradientTo, true), Qt::black, 50);
    QColor aDecoFgLight = alphaBlendColors(getColor(TitleGradientTo, true), Qt::white, 50);
    QColor iDecoFgDark = alphaBlendColors(getColor(TitleGradientTo, false), Qt::black, 50);
    QColor iDecoFgLight = alphaBlendColors(getColor(TitleGradientTo, false), Qt::white, 50);

    if (m_customIconColors && !m_useTitleProps) {
        aDecoFgDark = m_aFgColor;
        aDecoFgLight = m_aBgColor;
        iDecoFgDark = m_iFgColor;
        iDecoFgLight = m_iBgColor;
    }

    QPixmap icon = IconEngine::icon(type, size);
    QImage img = icon.toImage();

    QPixmap *pixmap;
    QImage tmpImage;
    ShadowEngine se;
    QPainter painter;
    QPixmap tmpShadow;
    switch (status) {
        case ActiveUp:
            if (m_useTitleProps)
                tmpImage = recolorImage(&img, getColor(TitleFont, true));
            else
                tmpImage = recolorImage(&img, aDecoFgDark);

            pixmap = new QPixmap(QPixmap::fromImage(tmpImage));
            break;
        case ActiveDown:
            if (m_useTitleProps)
                tmpImage = recolorImage(&img, getColor(TitleFont, true));
            else
                tmpImage = recolorImage(&img, aDecoFgLight);

            pixmap = new QPixmap(QPixmap::fromImage(tmpImage));
            break;
        case InactiveUp:
            if (m_useTitleProps)
                tmpImage = recolorImage(&img, getColor(TitleFont, false));
            else
                tmpImage = recolorImage(&img, iDecoFgDark);

            pixmap = new QPixmap(QPixmap::fromImage(tmpImage));
            break;
        case InactiveDown:
            if (m_useTitleProps)
                tmpImage = recolorImage(&img, getColor(TitleFont, false));
            else
                tmpImage = recolorImage(&img, iDecoFgLight);

            pixmap = new QPixmap(QPixmap::fromImage(tmpImage));
            break;
        case Shadow:
            // prepare shadow
            tmpShadow = QPixmap(icon.width()+4, icon.height()+4);
            tmpShadow.fill(QColor(0,0,0));
            tmpShadow.setMask(tmpShadow.createHeuristicMask(true));
            painter.begin(&tmpShadow);
            painter.setPen(Qt::white);
            painter.drawPixmap(0,0, icon);
            painter.end();
            tmpImage = se.makeShadow(tmpShadow, QColor(0, 0, 0));
            pixmap = new QPixmap(QPixmap::fromImage(tmpImage));
            break;
        default:
            pixmap = new QPixmap();
    }

    m_pixmaps[status][type] = pixmap;
    return *pixmap;
}

QList< SUSE2Handler::BorderSize >
SUSE2Handler::borderSizes() const
{
    // the list must be sorted
    return QList< BorderSize >() << BorderTiny << BorderNormal <<
        BorderLarge << BorderVeryLarge <<  BorderHuge <<
        BorderVeryHuge << BorderOversized;
}

static SUSE2Handler *handler = 0;

SUSE2Handler *Handler()
{
    return handler;
}

} // KWinSUSE2

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C"
{
    KDE_EXPORT KDecorationFactory *create_factory()
    {
        KWinSUSE2::handler = new KWinSUSE2::SUSE2Handler();
        return KWinSUSE2::handler;
    }
}

// kate: space-indent on; indent-width 4; replace-tabs on;
