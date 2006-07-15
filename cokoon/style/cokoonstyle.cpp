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

#include<QPushButton>
#include<QComboBox>
#include<QAbstractSpinBox>
#include<QCheckBox>
#include<QRadioButton>
#include<QSplitterHandle>
#include<QTabBar>

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

    // TODO: only set when they exist...
    const int sObjId = CokoonStyleSpec::LayoutProperties;
    setWidgetLayoutProp(WT_Generic, Generic::DefaultFrameWidth,
                        theme.getExpValue(sObjId, Exp_Generic_DefaultFrameWidth).toInt() );
    setWidgetLayoutProp(WT_PushButton, PushButton::DefaultIndicatorMargin,
                        theme.getExpValue(sObjId, Exp_Button_DefaultIndicatorMargin).toInt() );
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin,
                        theme.getExpValue(sObjId, Exp_Button_ContentsMargin).toInt() );
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin,
                        theme.getExpValue(sObjId, Exp_Button_FocusMargin).toInt() );
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftHorizontal,
                        theme.getExpValue(sObjId, Exp_Button_PressedShiftHorizontal).toInt() );
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftVertical,
                        theme.getExpValue(sObjId, Exp_Button_PressedShiftVertical).toInt() );

    setWidgetLayoutProp(WT_CheckBox, CheckBox::Size,
                        theme.getExpValue(sObjId, Exp_CheckBox_Size).toInt() );
    setWidgetLayoutProp(WT_RadioButton, RadioButton::Size,
                        theme.getExpValue(sObjId, Exp_RadioButton_Size).toInt() );

    setWidgetLayoutProp(WT_ComboBox, ComboBox::FrameWidth,
                        theme.getExpValue(sObjId, Exp_ComboBox_FrameWidth).toInt() );
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonWidth,
                        theme.getExpValue(sObjId, Exp_ComboBox_ButtonWidth).toInt() );
    setWidgetLayoutProp(WT_ComboBox, ComboBox::FocusMargin,
                        theme.getExpValue(sObjId, Exp_ComboBox_FocusMargin).toInt() );

    setWidgetLayoutProp(WT_SpinBox, SpinBox::FrameWidth,
                        theme.getExpValue(sObjId, Exp_SpinBox_FrameWidth).toInt() );
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonWidth,
                        theme.getExpValue(sObjId, Exp_SpinBox_ButtonWidth).toInt() );

    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::BarWidth,
                        theme.getExpValue(sObjId, Exp_ScrollBar_BarWidth).toInt() );
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::SingleButtonHeight,
                        theme.getExpValue(sObjId, Exp_ScrollBar_ButtonHeight).toInt() );

    // TODO: disable double buttons for now only...
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::DoubleBotButton, 0 );
}

CokoonStyle::~CokoonStyle()
{
}

void CokoonStyle::polish(QWidget* widget)
{
    if (qobject_cast<QPushButton*>(widget)
        || qobject_cast<QComboBox*>(widget)
        || qobject_cast<QAbstractSpinBox*>(widget)
        || qobject_cast<QCheckBox*>(widget)
        || qobject_cast<QRadioButton*>(widget)
        || qobject_cast<QSplitterHandle*>(widget)
        || qobject_cast<QTabBar*>(widget)
       ) {
        widget->setAttribute(Qt::WA_Hover);
       }

       KStyle::polish(widget);
}

void CokoonStyle::unpolish(QWidget* widget)
{
    if (qobject_cast<QPushButton*>(widget)
        || qobject_cast<QComboBox*>(widget)
        || qobject_cast<QAbstractSpinBox*>(widget)
        || qobject_cast<QCheckBox*>(widget)
        || qobject_cast<QRadioButton*>(widget)
        || qobject_cast<QSplitterHandle*>(widget)
        || qobject_cast<QTabBar*>(widget)
       ) {
        widget->setAttribute(Qt::WA_Hover, false);
       }

       KStyle::unpolish(widget);
}

void CokoonStyle::drawKStylePrimitive(WidgetType widgetType, int primitive,
                                       const QStyleOption* opt,
                                       QRect r, QPalette pal, State flags,
                                       QPainter* p,
                                       const QWidget* widget,
                                       KStyle::Option* kOpt) const
{
    bool reverseLayout = opt->direction == Qt::RightToLeft;

    bool enabled = flags & State_Enabled;
    bool mouseOver = flags & State_MouseOver;
    const bool hasFocus = flags & State_HasFocus;

    switch (widgetType)
    {
        case WT_PushButton:
        {
            switch (primitive)
            {
                case PushButton::Panel:
                {
                    int themeObjId = CokoonStyleSpec::Button;

                    themeObjId += CokoonStyleSpec::Button0_normal; // TODO: Default buttons!

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::Button1_disabled;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::Button1_pressed;
                    } else if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::Button1_prelight;
                    } else {
                        themeObjId += CokoonStyleSpec::Button1_normal;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );
                }

                case PushButton::DefaultButtonFrame:
                {
                    // Cokoon Themes should draw the focus indicator from bevel...
                }
            }
        }
        break;

        case WT_CheckBox:
        {
            switch(primitive)
            {
                case CheckBox::CheckOn:
                case CheckBox::CheckOff:
                case CheckBox::CheckTriState:
                {
                    int themeObjId = CokoonStyleSpec::CheckBox;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::CheckBox0_disabled;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::CheckBox0_pressed;
                    } else if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::CheckBox0_prelight;
                    } else {
                        themeObjId += CokoonStyleSpec::CheckBox0_normal;
                    }

                    if ( primitive == CheckBox::CheckOn ) {
                        themeObjId += CokoonStyleSpec::CheckBox1_on;
                    } else if ( primitive == CheckBox::CheckOff ) {
                        themeObjId += CokoonStyleSpec::CheckBox1_off;
                    } else {
                        themeObjId += CokoonStyleSpec::CheckBox1_tristate;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );
                    return;
                }
            }
        }
        break;

        case WT_RadioButton:
        {
            switch(primitive)
            {
                case RadioButton::RadioOn:
                case RadioButton::RadioOff:
                {
                    int themeObjId = CokoonStyleSpec::RadioButton;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::RadioButton0_disabled;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::RadioButton0_pressed;
                    } else if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::RadioButton0_prelight;
                    } else {
                        themeObjId += CokoonStyleSpec::RadioButton0_normal;
                    }

                    if ( primitive == RadioButton::RadioOn ) {
                        themeObjId += CokoonStyleSpec::RadioButton1_on;
                    } else {
                        themeObjId += CokoonStyleSpec::RadioButton1_off;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );
                    return;
                }
            }
        }
        break;

        case WT_LineEdit:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    const bool isReadOnly = flags & State_ReadOnly;

                    int themeObjId = CokoonStyleSpec::LineEdit;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::LineEdit0_disabled;
                    } else if ( hasFocus && !isReadOnly && enabled ) {
                        themeObjId += CokoonStyleSpec::LineEdit0_focused;
                    } else {
                        themeObjId += CokoonStyleSpec::LineEdit0_normal;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );
                    return;
                }

                case LineEdit::Panel:
                {
                    if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
                    {
                        const int lineWidth(panel->lineWidth);
                        p->fillRect(r.adjusted(lineWidth, lineWidth, -lineWidth, -lineWidth),
                                    pal.base());

                        if (lineWidth > 0)
                            drawPrimitive(PE_FrameLineEdit, panel, p, widget);
                    }
                }
            }

        }
        break;

        case WT_ComboBox:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    bool editable = false;
                    if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt) )
                        editable = cb->editable;


                    int themeObjId = CokoonStyleSpec::ComboBox;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::ComboBox0_disabled;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::ComboBox0_pressed;
                    } else if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::ComboBox0_prelight;
                    } else {
                        themeObjId += CokoonStyleSpec::ComboBox0_normal;
                    }

                    if ( hasFocus ) {
                        themeObjId += CokoonStyleSpec::ComboBox1_focused;
                    } else {
                        themeObjId += CokoonStyleSpec::ComboBox1_normal;
                    }

                    // TODO: editable state

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case ComboBox::EditField:
                {
                    // empty
                    return;
                }

                case ComboBox::Button:
                {
                    // painted in the frame...
                    return;
                }
            }

        }
        break;

        case WT_SpinBox:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    int themeObjId = CokoonStyleSpec::SpinBox;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::SpinBox0_disabled;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::SpinBox0_pressedUp;
                        // TODO pressedDown, ...
                    } else if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::SpinBox0_prelightUp;
                        // TODO prelightDown, ...
                    } else {
                        themeObjId += CokoonStyleSpec::SpinBox0_normal;
                    }

                    if ( hasFocus ) {
                        themeObjId += CokoonStyleSpec::SpinBox1_focused;
                    } else {
                        themeObjId += CokoonStyleSpec::SpinBox1_normal;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case SpinBox::EditField:
                {
                    // empty
                    return;
                }

                case SpinBox::UpButton:
                {
                    // painted in the frame...
                    return;
                }

                case SpinBox::DownButton:
                {
                    // painted in the frame...
                    return;
                }
            }

        }
        break;


        case WT_ScrollBar:
        {
            bool down = (flags & State_Sunken);

            switch (primitive)
            {
                case ScrollBar::SingleButtonVert:
                case ScrollBar::SingleButtonHor:
                {
                    bool hor = flags & State_Horizontal;
                    bool on = flags&State_On;
                    bool down = flags&State_Sunken;


                    int themeObjId = CokoonStyleSpec::ScrollBarButton;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::ScrollBarButton0_disabled;
                    } else if ( on || down ) {
                        themeObjId += CokoonStyleSpec::ScrollBarButton0_pressed;
                    } else {
                        themeObjId += CokoonStyleSpec::ScrollBarButton0_normal;
                    }

                    // TODO: need to find out if it's an addPage or subPage button for Left,Right,Up,Down...
                    if ( hor ) {
                        themeObjId += CokoonStyleSpec::ScrollBarButton1_left;
                    } else {
                        themeObjId += CokoonStyleSpec::ScrollBarButton1_up;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }
                break;

                case ScrollBar::GrooveAreaVert:
                case ScrollBar::GrooveAreaHor:
                {
                    bool hor = flags & State_Horizontal;
                    bool on = flags&State_On;
                    bool down = flags&State_Sunken;


                    int themeObjId = CokoonStyleSpec::ScrollBarGroove;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::ScrollBarGroove0_disabled;
                    } else if ( on || down ) {
                        themeObjId += CokoonStyleSpec::ScrollBarGroove0_pressed;
                    } else {
                        themeObjId += CokoonStyleSpec::ScrollBarGroove0_normal;
                    }

                    if ( hor ) {
                        themeObjId += CokoonStyleSpec::ScrollBarGroove1_horizontal;
                    } else {
                        themeObjId += CokoonStyleSpec::ScrollBarGroove1_vertical;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case ScrollBar::SliderVert:
                case ScrollBar::SliderHor:
                {
                    bool hor = flags & State_Horizontal;
                    bool on = flags&State_On;
                    bool down = flags&State_Sunken;


                    int themeObjId = CokoonStyleSpec::ScrollBarHandle;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::ScrollBarHandle0_disabled;
                    } else if ( on || down ) {
                        themeObjId += CokoonStyleSpec::ScrollBarHandle0_pressed;
                    } else {
                        themeObjId += CokoonStyleSpec::ScrollBarHandle0_normal;
                    }

                    if ( hor ) {
                        themeObjId += CokoonStyleSpec::ScrollBarHandle1_horizontal;
                    } else {
                        themeObjId += CokoonStyleSpec::ScrollBarHandle1_vertical;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

            }

        }
        break;

    }

    switch (primitive)
    {
        case Generic::Frame:
        {
            // WT_Generic and other fallen-through frames...
            // QFrame, Qt item views, etc.: sunken..

            int themeObjId = CokoonStyleSpec::GenericFrame;

            if (flags & State_Sunken) {
                themeObjId += CokoonStyleSpec::GenericFrame0_sunken;
            } else if (flags & State_Raised) {
                themeObjId += CokoonStyleSpec::GenericFrame0_raised;
            } else {
                themeObjId += CokoonStyleSpec::GenericFrame0_plain;
            }

            if ( hasFocus ) {
                themeObjId += CokoonStyleSpec::GenericFrame1_focused;
            } else {
                themeObjId += CokoonStyleSpec::GenericFrame1_normal;
            }

            theme.drawLayers( themeObjId, p,
                              r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

            return;
        }
    }

    // default fallback
    KStyle::drawKStylePrimitive(widgetType, primitive, opt,
                                r, pal, flags, p, widget, kOpt);
}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
