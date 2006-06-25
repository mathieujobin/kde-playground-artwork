/* coKoon theme widget style
   Copyright © 2006 Sandro Giessl <giessl@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include <QPainter>
#include <QStyleOption>

#include <QDebug>

#include "cokoonstyle.h"

#include "CokoonStyleSpec.h"

K_EXPORT_STYLE("Cokoon", CokoonStyle)

using namespace CokoonStyleSpec;

CokoonStyle::CokoonStyle()
{
    // load theme...
    theme.clear();
    CokoonStyleSpec::setupItems(&theme);
    // TODO: !!! locate themes using .desktop file or something...
    theme.loadTheme("/home/k4dev/src/playground-artwork/cokoon/style/TestTheme/TestTheme.xml");

    const int sObjId = CokoonStyleSpec::ButtonSettings;
    setWidgetLayoutProp(WT_PushButton, PushButton::DefaultIndicatorMargin,
                        theme.getExpValue(sObjId, Exp_DefaultIndicatorMargin).toInt() );
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin,
                        theme.getExpValue(sObjId, Exp_ContentsMargin).toInt() );
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin,
                        theme.getExpValue(sObjId, Exp_FocusMargin).toInt() );
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftHorizontal,
                        theme.getExpValue(sObjId, Exp_PressedShiftHorizontal).toInt() );
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftVertical,
                        theme.getExpValue(sObjId, Exp_PressedShiftVertical).toInt() );




    setWidgetLayoutProp(WT_Generic, Generic::DefaultFrameWidth, 2);
}

CokoonStyle::~CokoonStyle()
{
}

void CokoonStyle::drawKStylePrimitive(WidgetType widgetType, int primitive,
                                       const QStyleOption* opt,
                                       QRect r, QPalette pal, State flags,
                                       QPainter* p,
                                       const QWidget* widget,
                                       KStyle::Option* kOpt) const
{
//     bool reverseLayout = opt->direction == Qt::RightToLeft;
// 
//     bool enabled = flags & State_Enabled;

    switch (widgetType)
    {
        case WT_PushButton:
        {
            switch (primitive)
            {
                case Generic::Bevel:
                {
                    qDebug() << "Paint Button...!!!";

                    int themeObjId = CokoonStyleSpec::Button;

                    themeObjId += CokoonStyleSpec::Button0_normal; // TODO: Default buttons!

                    if ( !(flags & State_Enabled) ) {
                        themeObjId += CokoonStyleSpec::Button1_disabled;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::Button1_pressed;
                    } else if ( flags & State_MouseOver ) {
                        themeObjId += CokoonStyleSpec::Button1_prelight;
                    } else {
                        themeObjId += CokoonStyleSpec::Button1_normal;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case PushButton::DefaultButtonBevel:
                {
                    // Cokoon Themes should draw the focus indicator from bevel...
                    return;
                }
            }
        }
        break;



    }

    // default fallback
    KStyle::drawKStylePrimitive(widgetType, primitive, opt,
                                r, pal, flags, p, widget, kOpt);
}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
