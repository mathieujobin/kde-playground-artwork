/* coKoon theme KWin decoration
  Copyright (C) 2006 Sandro Giessl <giessl@kde.org>

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
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
 */

#include <QDebug>

#include <QApplication>
#include <QBitmap>
#include <QPainter>
#include <QPixmapCache>

#include <klocale.h>
#include <kconfig.h>

#include "cokoondecoration.h"
#include <kconfiggroup.h>
#include <kstandarddirs.h>


using namespace CokoonDecorationSpec;

namespace KWinCokoon
{

Handler::Handler()
    : componentData("CokoonDecoration")
{
    reset(0);
}

Handler::~Handler()
{
}

bool Handler::reset(unsigned long changed)
{
    // check if we are in reverse layout mode
    m_reverse = QApplication::isRightToLeft();

    // read in the configuration
    readConfig();

    QString res("cokoon/decorations/"+themeName+"/"+themeName+".xml");
    qDebug() << res;
    QString themeFile = KStandardDirs::locate("data", res);
    qDebug() << "CokoonStyle: using theme file " << themeFile;

    // load theme...
    theme.clear();
    // TODO: locate themes using .desktop file
    theme.loadTheme(themeFile);

    // theme settings
    // TODO: WindowTitleShadow: 0 none, 1 normal, perhaps more fancy shadows.
    const int sObjId = CokoonDecorationSpec::Settings;
    menuClose = theme.getExpValue(sObjId, Exp_MenuDoubleClickClose).toBool();
    customBtnSize = theme.getExpValue(sObjId, Exp_CustomButtonSize).toBool();
    respectBorderSize = theme.getExpValue(sObjId, Exp_RespectBorderSizeFactor ).toBool();

    switch (theme.getExpValue( sObjId, Exp_WindowTitleAlignment).toInt() ) {
    case 1:
        titleAlignment = Qt::AlignCenter;
        break;
    case 2:
        titleAlignment = Qt::AlignRight;
        break;
    default:
        titleAlignment = Qt::AlignLeft;
    }

    // set border size
    switch(KDecoration::options()->preferredBorderSize( this )) {
        case BorderTiny:
            borderSize = 0.5;
            break;
        case BorderLarge:
            borderSize = 2;
            break;
        case BorderVeryLarge:
            borderSize = 3;
            break;
        case BorderHuge:
            borderSize = 4;
            break;
        case BorderVeryHuge:
            borderSize = 5;
            break;
        case BorderOversized:
            borderSize = 7;
            break;
        case BorderNormal:
        default:
            borderSize = 1;
    }

    // Hard reset means decorations are deleted+newly created. Not necessary
    // in many cases.
    bool hardReset = true;
    if (changed & SettingColors || changed & SettingFont)
    {
        hardReset = false;
    } else if (changed & SettingButtons) {
        // taken care of by KCommonDecoration
        hardReset = false;
    }

    if (hardReset) {
        return true;
    } else {
        resetDecorations(changed);
        return false;
    }
}

KDecoration* Handler::createDecoration( KDecorationBridge* bridge )
{
        return (new Client( bridge, this ))->decoration();
}

bool Handler::supports( Ability ability ) const
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

void Handler::readConfig()
{
    KConfig config(componentData);
    KConfigGroup generalGroup(&config, "General");
    themeName = generalGroup.readEntry("theme", "TestTheme" );
}

QList< Handler::BorderSize >
Handler::borderSizes() const
{
    if (!respectBorderSize)
        return QList<BorderSize>() << BorderNormal;
    else
        return QList< BorderSize >() << BorderTiny << BorderNormal <<
                BorderLarge << BorderVeryLarge <<  BorderHuge <<
                BorderVeryHuge << BorderOversized;
}





Client::Client(KDecorationBridge* bridge, Handler* handler)
    : KCommonDecoration(bridge, handler), m_handler(handler),
    m_titleFont(options()->font(true, isToolWindow() ) )
{
    updCaption();
}

Client::~Client()
{
}

QString Client::visibleName() const
{
    return i18n("Cokoon Theme");
}

QString Client::defaultButtonsLeft() const
{
    return "M"; // TODO: let button order be defined by the theme somehow...
}

QString Client::defaultButtonsRight() const
{
    return "HIAX";
}

bool Client::decorationBehaviour(DecorationBehaviour behaviour) const
{
    switch (behaviour) {
        case DB_MenuClose:
            return m_handler->menuClose;

        case DB_WindowMask:
            return true;

        default:
            return KCommonDecoration::decorationBehaviour(behaviour);
    }
}

void Client::updCaption()
{
    const int maxCaptionLength = 300; // truncate captions longer than this!
    m_caption = caption();
    if (m_caption.length() > maxCaptionLength) {
        m_caption.truncate(maxCaptionLength);
        m_caption.append(" [...]");
    }
}

void Client::captionChange()
{
    updCaption();

    KCommonDecoration::captionChange();
}

QVariant Client::getVariableValue(int idx) const
{
    if (!m_handler)
        return QVariant();

    switch(idx) {
        case CokoonDecorationSpec::Var::BorderSizeFactor:
            return m_handler->borderSize;

        case CokoonDecorationSpec::Var::ButtonGroupLeftWidth:
            return buttonsLeftWidth();

        case CokoonDecorationSpec::Var::ButtonGroupRightWidth:
            return buttonsRightWidth();

        case CokoonDecorationSpec::Var::WindowTitleHeight:
        {
            QFontMetrics fm(m_titleFont);
            return fm.height();
        }

        case CokoonDecorationSpec::Var::WindowTitleWidth:
        {
            QFontMetrics fm(m_titleFont);
            return fm.width(m_caption);
        }

        case CokoonDecorationSpec::Var::ColorTitleBar:
            return options()->color(ColorTitleBar, isActive() );

        case CokoonDecorationSpec::Var::ColorTitleBlend:
            return options()->color(ColorTitleBlend, isActive() );

        case CokoonDecorationSpec::Var::ColorFont:
            return options()->color(ColorFont, isActive() );

        case CokoonDecorationSpec::Var::ColorButtonBg:
            return options()->color(ColorButtonBg, isActive() );

        case CokoonDecorationSpec::Var::ColorFrame:
            return options()->color(ColorFrame, isActive() );

        case CokoonDecorationSpec::Var::ColorHandle:
            return options()->color(ColorHandle, isActive() );

        default:
            return QVariant();
    }
}

void Client::paintCell(int id, QPainter *p,
               int left, int top, int width, int height) const
{
    switch (id) {
        case Special_WindowTitle:
        {

            QColor c = m_handler->theme.getExpValue( themeObjectId(),
                                                     Exp_WindowTitleColor, this).value<QColor>();

            const QString cacheKey = "$cokoonDecTtl" +
                    QString::number(c.rgba() ) + m_caption;
            QPixmap pm;
            if (!QPixmapCache::find(cacheKey, pm) ) {
    //             qDebug() << "missed cache key:" << qPrintable(cacheKey);

                QFontMetrics fm(m_titleFont);

                pm = QPixmap(fm.width(m_caption), fm.height() );
                pm.fill(Qt::transparent);
                QPainter painter(&pm);

                painter.setFont(m_titleFont);
                painter.setPen(c);
                painter.drawText(pm.rect(), m_caption );

                QPixmapCache::insert(cacheKey, pm);
            }

            const int pmW = pm.width();
            const int pmH = pm.height();
            int l = left;
            if (width > pmW) {
                switch(m_handler->titleAlignment) {

                    case Qt::AlignCenter:
                        l = left + (width-pmW)/2;
                        break;
                    case Qt::AlignRight:
                        l = left + width - pmW;
                        break;
                    case Qt::AlignLeft:
                    default:
                        l = left;
                        break;
                }
            } else {
                l = left;
            }
            const int w = qMin(pmW, width);
            p->drawPixmap(QRect(l,top,w,pmH), pm, QRect(0,0,w,pmH) );

            break;
        }

        case Special_WindowIcon:
        {
            icon().paint(p, QRect(left, top, width, height) );

            break;
        }

        default:
            qCritical("Special cell item '%d' not implemented!", id );
    }
}

int Client::layoutMetric(LayoutMetric lm, bool respectWindowState, const KCommonDecorationButton *btn) const
{
    const int objId = themeObjectId();

    switch (lm) {
        case LM_BorderLeft:
            return m_handler->theme.getExpValue( objId, Exp_BorderLeft,this).toInt();

        case LM_BorderRight:
            return m_handler->theme.getExpValue( objId, Exp_BorderRight,this).toInt();

        case LM_BorderBottom:
            return m_handler->theme.getExpValue( objId, Exp_BorderBottom,this).toInt();

        case LM_TitleEdgeTop:
            return m_handler->theme.getExpValue( objId, Exp_TitleEdgeTop,this).toInt();

        case LM_TitleEdgeBottom:
            return m_handler->theme.getExpValue( objId, Exp_TitleEdgeBottom,this).toInt();

        case LM_TitleEdgeLeft:
            return m_handler->theme.getExpValue( objId, Exp_TitleEdgeLeft,this).toInt();

        case LM_TitleEdgeRight:
            return m_handler->theme.getExpValue( objId, Exp_TitleEdgeRight,this).toInt();

        case LM_TitleBorderLeft:
        case LM_TitleBorderRight:
            return m_handler->theme.getExpValue( objId, Exp_TitleBorders,this).toInt();

        case LM_ButtonWidth:
        case LM_ButtonHeight:
            if (m_handler->customBtnSize) {
                const Button *b = dynamic_cast<const Button*>(btn);
                if (!b)
                    return 0;

                if (lm == LM_ButtonWidth)
                    return m_handler->theme.getExpValue( b->themeObjectId(),
                                                         Exp_ButtonWidth, this ).toInt();
                else
                    return m_handler->theme.getExpValue( b->themeObjectId(),
                                                         Exp_ButtonHeight, this ).toInt();
            }

            // fall through to LM_TitleHeight intended

        case LM_TitleHeight:
            return m_handler->theme.getExpValue( objId, Exp_TitleHeight,this).toInt();

        case LM_ButtonSpacing:
            return m_handler->theme.getExpValue( objId, Exp_ButtonSpacing,this).toInt();

        case LM_ButtonMarginTop:
            return m_handler->theme.getExpValue( objId, Exp_ButtonMarginTop,this).toInt();

        case LM_ExplicitButtonSpacer:
            return m_handler->theme.getExpValue( objId, Exp_ExplicitButtonSpacer,this).toInt();

        default:
            return KCommonDecoration::layoutMetric(lm, respectWindowState, btn);
    }
}

KCommonDecorationButton *Client::createButton(ButtonType type)
{
    switch (type) {
        case MenuButton:
            return new Button(MenuButton, this);

        case OnAllDesktopsButton:
            return new Button(OnAllDesktopsButton, this);

        case HelpButton:
            return new Button(HelpButton, this);

        case MinButton:
            return new Button(MinButton, this);

        case MaxButton:
            return new Button(MaxButton, this);

        case CloseButton:
            return new Button(CloseButton, this);

        case AboveButton:
            return new Button(AboveButton, this);

        case BelowButton:
            return new Button(BelowButton, this);

        case ShadeButton:
            return new Button(ShadeButton, this);

        default:
            return 0;
    }
}

void Client::init()
{
//     s_titleFont = isToolWindow() ? Handler()->titleFontTool() : Handler()->titleFont();

    KCommonDecoration::init();
}

QRegion Client::cornerShape(WindowCorner corner)
{
    const int objId = themeObjectId();

    QBitmap bm;
    switch (corner) {
        case WC_TopLeft:
            bm = m_handler->theme.getTilePixmap( objId, Tile_WindowMaskTopLeft, this);
            break;

        case WC_TopRight:
            bm = m_handler->theme.getTilePixmap( objId, Tile_WindowMaskTopRight, this);
            break;

        case WC_BottomLeft:
            bm = m_handler->theme.getTilePixmap( objId, Tile_WindowMaskBottomLeft, this);
            break;

        case WC_BottomRight:
            bm = m_handler->theme.getTilePixmap( objId, Tile_WindowMaskBottomRight, this);
            break;
    }

    int tw = bm.width();
    int th = bm.height();

    if (tw <= 0 || th <= 0) {
        return QRegion();
    }

    QRegion r(bm);

    switch (corner) {
        case WC_TopLeft:
            return r;

        case WC_TopRight:
            return r.translated(width()-tw, 0);

        case WC_BottomLeft:
            return r.translated(0, height() - th);

        case WC_BottomRight:
            return r.translated(width() - tw, height() - th);

        default:
            return QRegion();
    }
}

int Client::themeObjectId() const
{
    int themeObjId = CokoonDecorationSpec::Decoration;

    if (isActive() )
        themeObjId += CokoonDecorationSpec::Decoration0_active;
    else
        themeObjId += CokoonDecorationSpec::Decoration0_inactive;

    switch (maximizeMode() ) {
        case MaximizeRestore:
            themeObjId += CokoonDecorationSpec::Decoration1_normal;
            break;
        case MaximizeFull:
            themeObjId += CokoonDecorationSpec::Decoration1_max;
            break;
        case MaximizeHorizontal:
            themeObjId += CokoonDecorationSpec::Decoration1_maxHor;
            break;
        case MaximizeVertical:
            themeObjId += CokoonDecorationSpec::Decoration1_maxVert;
            break;
    }

    if (isToolWindow() )
        themeObjId += CokoonDecorationSpec::Decoration2_tool;
    else
        themeObjId += CokoonDecorationSpec::Decoration2_normal;

    if (isShade() )
        themeObjId += CokoonDecorationSpec::Decoration3_shaded;
    else
        themeObjId += CokoonDecorationSpec::Decoration3_normal;

    return themeObjId;
}

void Client::paintEvent(QPaintEvent */*e*/)
{
//     QRegion region = e->region();

    QPainter painter(widget() );

    m_handler->theme.drawLayers( themeObjectId(), &painter,
                                 0, 0, width(), height(), this, this );
}

void Client::reset( unsigned long changed )
{
    if (changed & SettingColors)
    {
        // repaint the whole thing
        widget()->update();
        updateButtons();
    } else if (changed & SettingFont) {
        // font has changed -- update title height and font
//         s_titleFont = isToolWindow() ? Handler()->titleFontTool() : Handler()->titleFont();

        updateLayout();

        // then repaint
        widget()->update();
    }

    KCommonDecoration::reset(changed);
}





Button::Button(ButtonType type, Client *parent)
    : KCommonDecorationButton(type, parent),
    hover(false),
    m_handler(parent->m_handler),
    m_client(parent)
{
    // no need to reset here as the button will be resetted on first resize.
}

Button::~Button()
{
}

void Button::reset(unsigned long changed)
{
    this->update();
}

void Button::enterEvent(QEvent *e)
{
    QAbstractButton::enterEvent(e);

    hover = true;

    update();
}

void Button::leaveEvent(QEvent *e)
{
    QAbstractButton::leaveEvent(e);

    hover = false;

    update();
}

int Button::themeObjectId() const
{
    int themeObjId = CokoonDecorationSpec::Button;

    switch(type() ) {
        case HelpButton:
            themeObjId += CokoonDecorationSpec::Button0_help;
            break;
        case MaxButton:
            switch (decoration()->maximizeMode() ) {
                case KDecoration::MaximizeRestore:
                    themeObjId += CokoonDecorationSpec::Button0_maximize;
                    break;
                case KDecoration::MaximizeFull:
                    themeObjId += CokoonDecorationSpec::Button0_maxRestore;
                    break;
                case KDecoration::MaximizeHorizontal:
                    themeObjId += CokoonDecorationSpec::Button0_maxHorRestore;
                    break;
                case KDecoration::MaximizeVertical:
                    themeObjId += CokoonDecorationSpec::Button0_maxVertRestore;
                    break;
            }
            break;
        case MinButton:
            themeObjId += CokoonDecorationSpec::Button0_minimize;
            break;
        case CloseButton:
            themeObjId += CokoonDecorationSpec::Button0_close;
            break;
        case MenuButton:
            themeObjId += CokoonDecorationSpec::Button0_menu;
            break;
        case OnAllDesktopsButton:
            if (decoration()->isOnAllDesktops() ) {
                themeObjId += CokoonDecorationSpec::Button0_allDesktopsRestore;
            } else {
                themeObjId += CokoonDecorationSpec::Button0_allDesktops;
            }
            break;
        case AboveButton:
            if (decoration()->keepAbove() ) {
                themeObjId += CokoonDecorationSpec::Button0_aboveRestore;
            } else {
                themeObjId += CokoonDecorationSpec::Button0_above;
            }
            break;
        case BelowButton:
            if (decoration()->keepBelow() ) {
                themeObjId += CokoonDecorationSpec::Button0_belowRestore;
            } else {
                themeObjId += CokoonDecorationSpec::Button0_below;
            }
            break;
        case ShadeButton:
            if (decoration()->isShade() ) {
                themeObjId += CokoonDecorationSpec::Button0_shadeRestore;
            } else {
                themeObjId += CokoonDecorationSpec::Button0_shade;
            }
            break;
        default:
            return 0;
    }

    if (isDown() )
        themeObjId += CokoonDecorationSpec::Button1_pressed;
    else if (hover)
        themeObjId += CokoonDecorationSpec::Button1_prelight;
    else
        themeObjId += CokoonDecorationSpec::Button1_normal;

    if (decoration()->isActive() )
        themeObjId += CokoonDecorationSpec::Button2_active;
    else
        themeObjId += CokoonDecorationSpec::Button2_inactive;

    if (decoration()->isToolWindow() )
        themeObjId += CokoonDecorationSpec::Button3_tool;
    else
        themeObjId += CokoonDecorationSpec::Button3_normal;

    return themeObjId;
}

void Button::paintEvent(QPaintEvent *)
{
    QPainter painter(this );

    m_handler->theme.drawLayers( themeObjectId(), &painter,
                                 0, 0, width(), height(), m_client, m_client );
}

} // KWinCokoon

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C"
{
    KDE_EXPORT KDecorationFactory *create_factory()
    {
        return new KWinCokoon::Handler();
    }
}

#include "cokoondecoration.moc"

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
