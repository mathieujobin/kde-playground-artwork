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



    setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin,
                        theme.getExpValue(sObjId, Exp_TabBarTab_ContentsMargin).toInt() );
    setWidgetLayoutProp(WT_TabBar, TabBar::TabFocusMargin,
                        theme.getExpValue(sObjId, Exp_TabBarTab_FocusMargin).toInt() );
    setWidgetLayoutProp(WT_TabBar, TabBar::TabTextToIconSpace,
                        theme.getExpValue(sObjId, Exp_TabBarTab_TextToIconSpace).toInt() );
    setWidgetLayoutProp(WT_TabBar, TabBar::BaseHeight,
                        theme.getExpValue(sObjId, Exp_TabBar_BaseHeight).toInt() );
    setWidgetLayoutProp(WT_TabBar, TabBar::BaseOverlap,
                        theme.getExpValue(sObjId, Exp_TabBar_BaseOverlap).toInt() );
    setWidgetLayoutProp(WT_TabBar, TabBar::ScrollButtonWidth,
                        theme.getExpValue(sObjId, Exp_TabBar_ScrollButtonWidth).toInt() );

    setWidgetLayoutProp(WT_Slider, Slider::HandleThickness,
                        theme.getExpValue(sObjId, Exp_SliderHandle_Thickness).toInt() );
    setWidgetLayoutProp(WT_Slider, Slider::HandleLength,
                        theme.getExpValue(sObjId, Exp_SliderHandle_Length).toInt() );

    setWidgetLayoutProp(WT_ProgressBar, ProgressBar::GrooveMargin,
                        theme.getExpValue(sObjId, Exp_ProgressBar_GrooveMargin).toInt() );
    setWidgetLayoutProp(WT_ProgressBar, ProgressBar::SideText,
                        theme.getExpValue(sObjId, Exp_ProgressBar_SideText).toInt() );
    setWidgetLayoutProp(WT_ProgressBar, ProgressBar::BusyIndicatorSize,
                        theme.getExpValue(sObjId, Exp_ProgressBar_BusyIndicatorSize).toInt() );

    setWidgetLayoutProp(WT_Header, Header::ContentsMargin,
                        theme.getExpValue(sObjId, Exp_Header_ContentsMargin).toInt() );
    setWidgetLayoutProp(WT_Header, Header::TextToIconSpace,
                        theme.getExpValue(sObjId, Exp_Header_TextToIconSpace).toInt() );
//     setWidgetLayoutProp(WT_Header, Header::MarkSize,
//                         theme.getExpValue(sObjId, Exp_Header_MarkSize).toInt() );

    setWidgetLayoutProp(WT_ToolBoxTab, ToolBoxTab::Margin,
                        theme.getExpValue(sObjId, Exp_ToolBoxTab_Margin).toInt() );

    setWidgetLayoutProp(WT_ToolBar, ToolBar::HandleExtent,
                        theme.getExpValue(sObjId, Exp_ToolBarHandle_Extent).toInt() );
    setWidgetLayoutProp(WT_ToolBar, ToolBar::ExtensionExtent,
                        theme.getExpValue(sObjId, Exp_ToolBar_ExtensionExtent).toInt() );

    setWidgetLayoutProp(WT_ToolBar, ToolBar::SeparatorExtent,
                        theme.getExpValue(sObjId, Exp_ToolBarSeparator_Extent).toInt() );

    setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemMargin,
                        theme.getExpValue(sObjId, Exp_ToolBar_ItemMargin).toInt() );
    setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemSpacing,
                        theme.getExpValue(sObjId, Exp_ToolBar_ItemSpacing).toInt() );

    setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin,
                        theme.getExpValue(sObjId, Exp_MenuBarItem_Margin).toInt() );

    setWidgetLayoutProp(WT_Menu, Menu::FrameWidth,
                        theme.getExpValue(sObjId, Exp_MenuFrame_Width).toInt() );

    setWidgetLayoutProp(WT_MenuItem, MenuItem::Margin,
                        theme.getExpValue(sObjId, Exp_MenuItem_Margin).toInt() );
    setWidgetLayoutProp(WT_MenuItem, MenuItem::CheckSpace,
                        theme.getExpValue(sObjId, Exp_MenuItem_CheckSpace).toInt() );
    setWidgetLayoutProp(WT_MenuItem, MenuItem::IconWidth,
                        theme.getExpValue(sObjId, Exp_MenuItem_IconWidth).toInt() );
    setWidgetLayoutProp(WT_MenuItem, MenuItem::IconSpace,
                        theme.getExpValue(sObjId, Exp_MenuItem_IconSpace).toInt() );
    setWidgetLayoutProp(WT_MenuItem, MenuItem::AccelSpace,
                        theme.getExpValue(sObjId, Exp_MenuItem_AccelSpace).toInt() );
    setWidgetLayoutProp(WT_MenuItem, MenuItem::ArrowSpace,
                        theme.getExpValue(sObjId, Exp_MenuItem_ArrowSpace).toInt() );
    setWidgetLayoutProp(WT_MenuItem, MenuItem::ArrowWidth,
                        theme.getExpValue(sObjId, Exp_MenuItem_ArrowWidth).toInt() );
    setWidgetLayoutProp(WT_MenuItem, MenuItem::MinHeight,
                        theme.getExpValue(sObjId, Exp_MenuItem_MinHeight).toInt() );
    setWidgetLayoutProp(WT_MenuItem, MenuItem::Margin,
                        theme.getExpValue(sObjId, Exp_MenuItem_Margin).toInt() );
    setWidgetLayoutProp(WT_MenuItem, MenuItem::SeparatorHeight,
                        theme.getExpValue(sObjId, Exp_MenuItemSeparator_Height).toInt() );

    setWidgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin,
                        theme.getExpValue(sObjId, Exp_ToolButton_ContentsMargin).toInt() );
    setWidgetLayoutProp(WT_ToolButton, ToolButton::FocusMargin,
                        theme.getExpValue(sObjId, Exp_ToolButton_FocusMargin).toInt() );

    setWidgetLayoutProp(WT_Splitter, Splitter::Width,
                        theme.getExpValue(sObjId, Exp_Splitter_Width).toInt() );

    setWidgetLayoutProp(WT_DockWidget, DockWidget::FrameWidth,
                        theme.getExpValue(sObjId, Exp_DockWidget_FrameWidth).toInt() );
    setWidgetLayoutProp(WT_DockWidget, DockWidget::SeparatorExtent,
                        theme.getExpValue(sObjId, Exp_DockWidget_SeparatorExtent).toInt() );

    setWidgetLayoutProp(WT_Window, Window::TitleHeight,
                        theme.getExpValue(sObjId, Exp_Window_TitleHeight).toInt() );
    setWidgetLayoutProp(WT_Window, Window::NoTitleFrame,
                        theme.getExpValue(sObjId, Exp_Window_NoTitleFrame).toInt() );
    setWidgetLayoutProp(WT_Window, Window::TitleMargin,
                        theme.getExpValue(sObjId, Exp_Window_TitleMargin).toInt() );
    setWidgetLayoutProp(WT_Window, Window::ButtonWidth,
                        theme.getExpValue(sObjId, Exp_Window_ButtonWidth).toInt() );
    setWidgetLayoutProp(WT_Window, Window::ButtonSpace,
                        theme.getExpValue(sObjId, Exp_Window_ButtonSpace).toInt() );
    setWidgetLayoutProp(WT_Window, Window::ButtonToTextSpace,
                        theme.getExpValue(sObjId, Exp_Window_ButtonToTextSpace).toInt() );


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
                                       const QRect &r, const QPalette &pal,
                                       State flags, QPainter* p,
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


        case WT_Slider:
        {
            switch (primitive)
            {
                case Slider::HandleVert:
                case Slider::HandleHor:
                {
                    int themeObjId = CokoonStyleSpec::SliderHandle;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::SliderHandle0_disabled;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::SliderHandle0_pressed;
                    } else if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::SliderHandle0_prelight;
                    } else {
                        themeObjId += CokoonStyleSpec::SliderHandle0_normal;
                    }

                    if ( flags & State_Horizontal ) {
                        themeObjId += CokoonStyleSpec::SliderHandle1_horizontal;
                    } else {
                        themeObjId += CokoonStyleSpec::SliderHandle1_vertical;
                    }

                    if ( hasFocus ) {
                        themeObjId += CokoonStyleSpec::SliderHandle2_focused;
                    } else {
                        themeObjId += CokoonStyleSpec::SliderHandle2_notFocused;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case Slider::GrooveHor:
                case Slider::GrooveVert:
                {
                    int themeObjId = CokoonStyleSpec::SliderGroove;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::SliderGroove0_disabled;
                    } else {
                        themeObjId += CokoonStyleSpec::SliderGroove0_normal;
                    }

                    if ( flags & State_Horizontal ) {
                        themeObjId += CokoonStyleSpec::SliderGroove1_horizontal;
                    } else {
                        themeObjId += CokoonStyleSpec::SliderGroove1_vertical;
                    }

                    if ( hasFocus ) {
                        themeObjId += CokoonStyleSpec::SliderHandle2_focused;
                    } else {
                        themeObjId += CokoonStyleSpec::SliderHandle2_notFocused;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                default:
                    break;
            }
        }
        break;


        case WT_ProgressBar:
        {
            switch (primitive)
            {
                case ProgressBar::Indicator:
                {
                    int themeObjId = CokoonStyleSpec::ProgressBarBar;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::ProgressBarBar0_disabled;
                    } else {
                        themeObjId += CokoonStyleSpec::ProgressBarBar0_normal;
                    }

                    if ( flags & State_Horizontal ) {
                        themeObjId += CokoonStyleSpec::ProgressBarBar1_horizontal;
                    } else {
                        themeObjId += CokoonStyleSpec::ProgressBarBar1_vertical;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case ProgressBar::Groove:
                {
                    int themeObjId = CokoonStyleSpec::ProgressBarGroove;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::ProgressBarGroove0_disabled;
                    } else {
                        themeObjId += CokoonStyleSpec::ProgressBarGroove0_normal;
                    }

                    if ( flags & State_Horizontal ) {
                        themeObjId += CokoonStyleSpec::ProgressBarGroove1_horizontal;
                    } else {
                        themeObjId += CokoonStyleSpec::ProgressBarGroove1_vertical;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                default:
                    break;
            }
        }
        break;


        case WT_TabBar:
        {
            switch (primitive)
            {
                case TabBar::EastTab:
                case TabBar::WestTab:
                case TabBar::SouthTab:
                case TabBar::NorthTab:
                {
                    const QStyleOptionTabV2 *tabOpt = qstyleoption_cast<const QStyleOptionTabV2 *>(opt);
                    if (!tabOpt)
                        break;


                    int themeObjId = CokoonStyleSpec::TabBarTab;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::TabBarTab0_disabled;
                    } else if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::TabBarTab0_prelight;
                    } else {
                        themeObjId += CokoonStyleSpec::TabBarTab0_normal;
                    }

                    if ( flags&State_Selected ) {
                        themeObjId += CokoonStyleSpec::TabBarTab1_selected;
                    } else {
                        switch (tabOpt->selectedPosition) {
                            case QStyleOptionTab::PreviousIsSelected:
                                themeObjId += CokoonStyleSpec::TabBarTab1_previousIsSelected;
                                break;
                            case QStyleOptionTab::NextIsSelected:
                                themeObjId += CokoonStyleSpec::TabBarTab1_nextIsSelected;
                                break;
                            default:
                            case QStyleOptionTab::NotAdjacent:
                                themeObjId += CokoonStyleSpec::TabBarTab1_notAdjacent;
                                break;
                        }
                    }

                    switch (tabOpt->position) {
                        case QStyleOptionTab::Beginning:
                            themeObjId += CokoonStyleSpec::TabBarTab2_beginning;
                            break;
                        case QStyleOptionTab::Middle:
                            themeObjId += CokoonStyleSpec::TabBarTab2_middle;
                            break;
                        case QStyleOptionTab::End:
                            themeObjId += CokoonStyleSpec::TabBarTab2_end;
                            break;
                        default:
                        case QStyleOptionTab::OnlyOneTab:
                            themeObjId += CokoonStyleSpec::TabBarTab2_onlyOne;
                            break;
                    }

                    switch (primitive) {
                        case TabBar::EastTab:
                            themeObjId += CokoonStyleSpec::TabBarTab3_east;
                            break;
                        case TabBar::WestTab:
                            themeObjId += CokoonStyleSpec::TabBarTab3_west;
                            break;
                        case TabBar::SouthTab:
                            themeObjId += CokoonStyleSpec::TabBarTab3_south;
                            break;
                        default:
                        case TabBar::NorthTab:
                            themeObjId += CokoonStyleSpec::TabBarTab3_north;
                            break;
                    }

                    if (hasFocus) {
                        themeObjId += CokoonStyleSpec::TabBarTab4_focused;
                    } else {
                        themeObjId += CokoonStyleSpec::TabBarTab4_notFocused;
                    }

                    switch (tabOpt->shape) {
                        case QTabBar::TriangularNorth:
                        case QTabBar::TriangularSouth:
                        case QTabBar::TriangularEast:
                        case QTabBar::TriangularWest:
                            themeObjId += CokoonStyleSpec::TabBarTab5_triangular;
                            break;
                        default:
                            themeObjId += CokoonStyleSpec::TabBarTab5_rounded;
                            break;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );


                    return;
                }

                default:
                    break;
            }
        }
        break;


        case WT_Header:
        {
            switch (primitive)
            {
                case Header::SectionHor:
                case Header::SectionVert:
                {
                    const QStyleOptionHeader *hdOpt = qstyleoption_cast<const QStyleOptionHeader *>(opt);
                    if (!hdOpt)
                        break;


                    int themeObjId = CokoonStyleSpec::Header;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::Header0_disabled;
                    } else if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::Header0_prelight;
                    } else {
                        themeObjId += CokoonStyleSpec::Header0_normal;
                    }

                    if ( flags&State_On || flags&State_Sunken ) {
                        themeObjId += CokoonStyleSpec::Header1_selected;
                    } else {
                        switch (hdOpt->selectedPosition) {
                            case QStyleOptionHeader::NextAndPreviousAreSelected:
                                themeObjId += CokoonStyleSpec::Header1_nextAndPreviousAreSelected;
                                break;
                            case QStyleOptionHeader::PreviousIsSelected:
                                themeObjId += CokoonStyleSpec::Header1_previousIsSelected;
                                break;
                            case QStyleOptionHeader::NextIsSelected:
                                themeObjId += CokoonStyleSpec::Header1_nextIsSelected;
                                break;
                            default:
                            case QStyleOptionHeader::NotAdjacent:
                                themeObjId += CokoonStyleSpec::Header1_notAdjacent;
                                break;
                        }
                    }

                    switch (hdOpt->position) {
                        case QStyleOptionHeader::Beginning:
                            themeObjId += CokoonStyleSpec::Header2_beginning;
                            break;
                        case QStyleOptionHeader::Middle:
                            themeObjId += CokoonStyleSpec::Header2_middle;
                            break;
                        case QStyleOptionHeader::End:
                            themeObjId += CokoonStyleSpec::Header2_end;
                            break;
                        default:
                        case QStyleOptionHeader::OnlyOneSection:
                            themeObjId += CokoonStyleSpec::Header2_onlyOne;
                            break;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }
            }
        }
        break;

        case WT_ToolBoxTab:
        {
            switch (primitive)
            {
                case ToolBoxTab::Panel:
                {
                    int themeObjId = CokoonStyleSpec::ToolBoxTab;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::ToolBoxTab0_disabled;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::ToolBoxTab0_pressed;
                    } else {
                        themeObjId += CokoonStyleSpec::ToolBoxTab0_normal;
                    }

                    if ( flags & State_Selected ) {
                        themeObjId += CokoonStyleSpec::ToolBoxTab1_selected;
                    } else {
                        themeObjId += CokoonStyleSpec::ToolBoxTab1_selected;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }
            }
        }
        break;

        case WT_ToolBar:
        {
            switch (primitive)
            {
                case ToolBar::HandleHor:
                case ToolBar::HandleVert:
                {
                    int themeObjId = CokoonStyleSpec::ToolBarSeparator;

                    if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::ToolBarHandle0_prelight;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::ToolBarHandle0_pressed;
                    } else {
                        themeObjId += CokoonStyleSpec::ToolBarHandle0_normal;
                    }

                    if ( flags & State_Horizontal ) {
                        themeObjId += CokoonStyleSpec::ToolBarHandle1_horizontal;
                    } else {
                        themeObjId += CokoonStyleSpec::ToolBarHandle1_vertical;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case ToolBar::Separator:
                {
                    int themeObjId = CokoonStyleSpec::ToolBarSeparator;

                    if ( flags & State_Horizontal ) {
                        themeObjId += CokoonStyleSpec::ToolBarSeparator0_horizontal;
                    } else {
                        themeObjId += CokoonStyleSpec::ToolBarSeparator0_vertical;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );
                    return;
                }

                case ToolBar::PanelHor:
                {
                    // TODO
                    return;
                }
                case ToolBar::PanelVert:
                {
                    // TODO
                    return;
                }
            }
        }
        break;

        case WT_MenuBarItem:
        {
            switch (primitive)
            {
                case MenuBarItem::Panel:
                {
                    int themeObjId = CokoonStyleSpec::MenuBarItem;

                    if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::MenuBarItem0_prelight;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::MenuBarItem0_pressed;
                    } else {
                        themeObjId += CokoonStyleSpec::MenuBarItem0_normal;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }
            }
        }
        break;

        case WT_Menu:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    int themeObjId = CokoonStyleSpec::MenuFrame;
                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case Menu::Background:
                {
                    // TODO!
                    p->fillRect( r, pal.color(QPalette::Background).light( 105 ) );
                    return;
                }

                case Menu::TearOff:
                {
                    // TODO
                    return;
                }

                case Menu::Scroller:
                {
                    // TODO
                    return;
                }
            }
        }
        break;

        case WT_MenuItem:
        {
            switch (primitive)
            {
                case MenuItem::Separator:
                {
                    int themeObjId = CokoonStyleSpec::MenuItemSeparator;
                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case MenuItem::ItemIndicator:
                {
                    int themeObjId = CokoonStyleSpec::MenuItem;

                    themeObjId += CokoonStyleSpec::MenuItem0_prelight;

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case MenuItem::CheckColumn:
                {
                    // TODO
                    return;
                }

                case MenuItem::CheckOn:
                {
                    drawKStylePrimitive(WT_CheckBox, CheckBox::CheckOn, opt, r, pal, flags, p, widget);
                    return;
                }

                case MenuItem::CheckOff:
                {
                    
                    drawKStylePrimitive(WT_CheckBox, CheckBox::CheckOff, opt, r, pal, flags, p, widget);
                    return;
                }

                case MenuItem::RadioOn:
                {
                    drawKStylePrimitive(WT_RadioButton, RadioButton::RadioOn, opt, r, pal, flags, p, widget);
                    return;
                }

                case MenuItem::RadioOff:
                {
                    drawKStylePrimitive(WT_RadioButton, RadioButton::RadioOff, opt, r, pal, flags, p, widget);
                    return;
                }

                case MenuItem::CheckIcon:
                {
                    // empty since for now only checkAlongsideicon... is supported
                    return;
                }
            }
        }
        break;

        case WT_ToolButton:
        {
            switch (primitive)
            {
                case ToolButton::Panel:
                {
                    int themeObjId = CokoonStyleSpec::ToolButton;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::ToolButton0_disabled;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::ToolButton0_pressed;
                    } else if (mouseOver) {
                        themeObjId += CokoonStyleSpec::ToolButton0_prelight;
                    }else {
                        themeObjId += CokoonStyleSpec::ToolButton0_normal;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }
            }

        }
        break;

        case WT_Splitter:
        {
            switch (primitive)
            {
                case Splitter::HandleHor:
                case Splitter::HandleVert:
                {
                    int themeObjId = CokoonStyleSpec::Splitter;

                    if ( mouseOver ) {
                        themeObjId += CokoonStyleSpec::Splitter0_prelight;
                    } else if ( (flags & State_On) || (flags & State_Sunken) ) {
                        themeObjId += CokoonStyleSpec::Splitter0_pressed;
                    } else {
                        themeObjId += CokoonStyleSpec::Splitter0_normal;
                    }

                    if ( flags & State_Horizontal ) {
                        themeObjId += CokoonStyleSpec::Splitter1_horizontal;
                    } else {
                        themeObjId += CokoonStyleSpec::Splitter1_vertical;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }
            }
        }
        break;

        case WT_DockWidget:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    int themeObjId = CokoonStyleSpec::DockWidgetFrame;

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case DockWidget::TitlePanel:
                {
                    int themeObjId = CokoonStyleSpec::DockWidgetTitle;

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }
            }
        }
        break;

        case WT_Window:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    int themeObjId = CokoonStyleSpec::WindowFrame;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::WindowFrame0_inactive;
                    } else {
                        themeObjId += CokoonStyleSpec::WindowFrame0_active;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case Window::TitlePanel:
                {
                    int themeObjId = CokoonStyleSpec::WindowTitle;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::WindowTitle0_inactive;
                    } else {
                        themeObjId += CokoonStyleSpec::WindowTitle0_active;
                    }

                    theme.drawLayers( themeObjId, p,
                                      r.left(), r.top(), r.width(), r.height(),
                                      /*this, this*/ 0,0 );

                    return;
                }

                case Window::ButtonMin:
                case Window::ButtonMax:
                case Window::ButtonRestore:
                case Window::ButtonClose:
                case Window::ButtonShade:
                case Window::ButtonUnshade:
                case Window::ButtonHelp:
                {
                    int themeObjId = CokoonStyleSpec::WindowButton;

                    if ( !enabled ) {
                        themeObjId += CokoonStyleSpec::WindowButton0_inactive;
                    } else {
                        themeObjId += CokoonStyleSpec::WindowButton0_active;
                    }

                    KStyle::TitleButtonOption* bOpt =
                            extractOption<KStyle::TitleButtonOption*>(kOpt);
                    if (bOpt->active) {
                        themeObjId += CokoonStyleSpec::WindowButton1_pressed;
                    } else {
                        themeObjId += CokoonStyleSpec::WindowButton1_normal;
                    }

                    switch (primitive) {
                        case Window::ButtonMin:
                            themeObjId += CokoonStyleSpec::WindowButton2_minimize;
                            break;
                        case Window::ButtonMax:
                            themeObjId += CokoonStyleSpec::WindowButton2_maximize;
                            break;
                        case Window::ButtonRestore:
                            themeObjId += CokoonStyleSpec::WindowButton2_restore;
                            break;
                        case Window::ButtonClose:
                            themeObjId += CokoonStyleSpec::WindowButton2_close;
                            break;
                        case Window::ButtonShade:
                            themeObjId += CokoonStyleSpec::WindowButton2_shade;
                            break;
                        case Window::ButtonUnshade:
                            themeObjId += CokoonStyleSpec::WindowButton2_unshade;
                            break;
                        default:
                        case Window::ButtonHelp:
                            themeObjId += CokoonStyleSpec::WindowButton2_help;
                            break;
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
