/***************************************************************************
 *   Copyright (C) 2006-2007 by Thomas Lübking                             *
 *   thomas.luebking@web.de                                                *
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

#include <QComboBox>
#include <QPainter>
#include <QTime>
// #include <QPixmapCache>
#include <QStyleOptionComplex>
#include <cmath>
#include "oxygen.h"

using namespace Oxygen;

extern Config config;
extern Dpi dpi;

#include "inlinehelp.cpp"


void OxygenStyle::drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget) const
{
#define PAL option->palette
#define RECT option->rect
#define COLOR(_TYPE_) option->palette.color(QPalette::_TYPE_)
   
   Q_ASSERT(option);
   Q_ASSERT(painter);
   
   bool sunken = option->state & State_Sunken;
   bool isEnabled = option->state & State_Enabled;
   bool hover = isEnabled && (option->state & State_MouseOver);
   bool hasFocus = option->state & State_HasFocus;
   
   switch ( control )
   {
   case CC_SpinBox: // A spinbox, like QSpinBox
      if (const QStyleOptionSpinBox *sb =
          qstyleoption_cast<const QStyleOptionSpinBox *>(option))
      {
         QStyleOptionSpinBox copy = *sb;
         int uh = 0;
         
         if (sb->frame && (sb->subControls & SC_SpinBoxFrame))
            drawPrimitive ( PE_PanelLineEdit, sb, painter, widget );
         
         GradientType gt;
         Tile::PosFlags pf;
         
         if (sb->subControls & SC_SpinBoxUp)
         {
            copy.subControls = SC_SpinBoxUp;
            copy.rect = subControlRect(CC_SpinBox, sb, SC_SpinBoxUp, widget);
            uh = copy.rect.height();
            
            pf = Tile::Top | Tile::Left | Tile::Right;
            isEnabled = sb->stepEnabled & QAbstractSpinBox::StepUpEnabled;
            hover = sb->activeSubControls == SC_SpinBoxUp;
            
            shadows.button[1].render(copy.rect, painter, pf);
            copy.rect.adjust(2,2,-2,0);
            gt = (isEnabled && hover) ? (sunken ? GradSunken : GradGloss) : GradButton;
            fillWithMask(painter, copy.rect, gradient(COLOR(Button), copy.rect.height()*2, Qt::Vertical, gt),
                         &masks.button, pf | Tile::Center);
            
            int dx = copy.rect.width()/5, dy = copy.rect.height()/5;
            copy.rect.adjust(dx, dy,-dx,-dy);
            
            painter->setPen(isEnabled ? (hover ? COLOR(Highlight) : COLOR(ButtonText)) :
                            PAL.color(QPalette::Disabled, QPalette::ButtonText));
            drawPrimitive(PE_IndicatorSpinUp, &copy, painter, widget);
         }
         
         if (sb->subControls & SC_SpinBoxDown)
         {
            copy.subControls = SC_SpinBoxDown;
            copy.rect = subControlRect(CC_SpinBox, sb, SC_SpinBoxDown, widget);
            
            pf = Tile::Bottom | Tile::Left | Tile::Right;
            isEnabled = sb->stepEnabled & QAbstractSpinBox::StepDownEnabled;
            hover = sb->activeSubControls == SC_SpinBoxDown;
            
            shadows.button[1].render(copy.rect, painter, pf);
            copy.rect.adjust(2,0,-2,-2);
            gt = (isEnabled && hover) ? (sunken ? GradSunken : GradGloss) : GradButton;
            fillWithMask(painter, copy.rect, gradient(COLOR(Button), copy.rect.height()*2, Qt::Vertical, gt), &masks.button,
                         pf | Tile::Center, false, QPoint(0,-uh));
            
            int dx = copy.rect.width()/5, dy = copy.rect.height()/5;
            copy.rect.adjust(dx, dy,-dx,-dy);
            
            painter->setPen(isEnabled ? (hover ? COLOR(Highlight) : COLOR(ButtonText)) :
                            PAL.color(QPalette::Disabled, QPalette::ButtonText));
            drawPrimitive(PE_IndicatorSpinDown, &copy, painter, widget);
         }
      }
      break;
   case CC_GroupBox:
      if (const QStyleOptionGroupBox *groupBox =
          qstyleoption_cast<const QStyleOptionGroupBox *>(option))
      {
         if (groupBox->subControls & QStyle::SC_GroupBoxFrame)
         {
            QStyleOptionFrameV2 frame;
            frame.QStyleOption::operator=(*groupBox);
            frame.features = groupBox->features;
            frame.lineWidth = groupBox->lineWidth;
            frame.midLineWidth = groupBox->midLineWidth;
            frame.rect = subControlRect(CC_GroupBox, option, SC_GroupBoxFrame, widget);
            drawPrimitive(PE_FrameGroupBox, &frame, painter, widget);
         }
         
         // Draw title
         if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty())
         {
            QRect textRect = subControlRect(CC_GroupBox, option, SC_GroupBoxLabel, widget);
            QColor textColor = groupBox->textColor;
            if (textColor.isValid())
               painter->setPen(textColor);
            QFont tmpfnt = painter->font(); tmpfnt.setBold(true);
            painter->setFont ( tmpfnt );
            int alignment = Qt::AlignCenter; //int(groupBox->textAlignment);
            if (!styleHint(QStyle::SH_UnderlineShortcut, option, widget))
               alignment |= Qt::TextHideMnemonic;
            else
               alignment |= Qt::TextShowMnemonic;
            
            drawItemText(painter, textRect,  alignment, groupBox->palette, isEnabled, groupBox->text,
                           textColor.isValid() ? QPalette::NoRole : QPalette::Foreground);
            
            textRect.setTop(textRect.top()+(textRect.height()-shadows.line.thickness())/2);
            int x = textRect.right()+dpi.$4;
            textRect.setRight(textRect.left()-dpi.$4);
            textRect.setLeft(qMin(RECT.x()+RECT.width()/4,textRect.x()-(textRect.x()-RECT.x())/2));
            shadows.line.render(textRect, painter, Tile::Left|Tile::Center);
            textRect.setLeft(x);
            textRect.setRight(qMax(RECT.right()-RECT.width()/4,x+(RECT.right()-x)/2));
            shadows.line.render(textRect, painter, Tile::Right|Tile::Center);
         }
         
            // Draw checkbox
         if (groupBox->subControls & SC_GroupBoxCheckBox)
         {
            QStyleOptionButton box;
            box.QStyleOption::operator=(*groupBox);
            box.rect = subControlRect(CC_GroupBox, option, SC_GroupBoxCheckBox, widget);
            drawPrimitive(PE_IndicatorCheckBox, &box, painter, widget);
         }
      }
      break;
   case CC_ComboBox: // A combobox, like QComboBox
      if (const QStyleOptionComboBox *cmb =
          qstyleoption_cast<const QStyleOptionComboBox *>(option))
      {
         if ((cmb->subControls & SC_ComboBoxFrame) && cmb->frame)
         {
            if (cmb->editable)
               drawPrimitive(PE_PanelLineEdit, option, painter, widget);
            else
               fillWithMask(painter, RECT,
                            gradient(COLOR(Window).dark(110), RECT.height(), Qt::Vertical, GradSunken), &masks.tab);
         }
         if ((cmb->subControls & SC_ComboBoxArrow) &&
             (!(widget && qobject_cast<const QComboBox*>(widget)) || ((const QComboBox*)widget)->count() > 0))
         {
            painter->save();
            QStyleOptionComboBox tmpOpt = *cmb;
            hover = hover && (cmb->activeSubControls == SC_ComboBoxArrow);
            QRect ar = subControlRect(CC_ComboBox, cmb, SC_ComboBoxArrow, widget);
            if (!cmb->editable)
            {
               tmpOpt.rect =  ar;
               drawPrimitive(PE_PanelButtonBevel, &tmpOpt, painter, widget);
               if (hover)
                  painter->setPen( COLOR(WindowText) );
               else 
                  painter->setPen( midColor(COLOR(Window), COLOR(WindowText)));
            }
            else
            {
               if (hover && !sunken)
                  painter->setPen( COLOR(Text));
               else
                  painter->setPen( midColor(COLOR(Base), COLOR(Text)) );
            }
            tmpOpt.rect =  ar.adjusted(ar.width()/3,ar.height()/3,-ar.width()/3,-ar.height()/3);
            painter->setRenderHint ( QPainter::Antialiasing, true );
            drawPrimitive(PE_IndicatorArrowDown, &tmpOpt, painter, widget);
            painter->restore();
         }
      }
      break;
//    case CC_ScrollBar: // A scroll bar, like QScrollBar
   case CC_Slider: // A slider, like QSlider
      if (const QStyleOptionSlider *slider =
          qstyleoption_cast<const QStyleOptionSlider *>(option))
      {
         QRect groove = QCommonStyle::subControlRect(CC_Slider, slider, SC_SliderGroove, widget);
         QRect handle = QCommonStyle::subControlRect(CC_Slider, slider, SC_SliderHandle, widget);
         // this is a workaround, qslider doesn't send the direction
         bool inverse = (widget && widget->layoutDirection() == Qt::RightToLeft) xor slider->upsideDown;
         // TODO: activate this one, remove the above
//          bool inverse = option->direction == Qt::RightToLeft;
         hover = hover && (slider->activeSubControls & SC_SliderHandle);
         sunken = sunken && (slider->activeSubControls & SC_SliderHandle);
         
         if ((slider->subControls & SC_SliderGroove) && groove.isValid())
         {
            QRect r; Tile::PosFlags pf = 0;
            QColor c = hasFocus ? COLOR(Highlight) : COLOR(ButtonText);
            if ( slider->orientation == Qt::Horizontal )
            {
               groove.adjust(0,handle.height()/3,0,-handle.height()/3);
               r = groove;
               r.setRight(handle.left()+3);
               pf = Tile::Top | Tile::Left | Tile::Bottom | Tile::Center;
               if (inverse)
                  fillWithMask(painter, r, gradient(COLOR(Window), r.height(), Qt::Vertical, GradSunken), &masks.button, pf);
               else
               {
                  shadows.button[1].render(r, painter);
                  r.adjust(2,1,-2,-2);
                  fillWithMask(painter, r, gradient(c, r.height(), Qt::Vertical, GradGloss), &masks.button, pf);
               }
               r = groove;
               r.setLeft(handle.right()-3);
               pf = Tile::Top | Tile::Right | Tile::Bottom | Tile::Center;
               if (inverse)
               {
                  shadows.button[1].render(r, painter);
                  r.adjust(2,1,-2,-2);
                  fillWithMask(painter, r, gradient(c, r.height(), Qt::Vertical, GradGloss), &masks.button, pf);
               }
               else
                  fillWithMask(painter, r, gradient(COLOR(Window), r.height(), Qt::Vertical, GradSunken), &masks.button, pf);
            }
            else // Vertical
            {
               groove.adjust(handle.width()/3,0,-handle.width()/3,0);
               r = groove;
               r.setBottom(handle.top()+3);
               pf = Tile::Top | Tile::Left | Tile::Right | Tile::Center;
               if (inverse)
               {
                  shadows.button[1].render(r, painter);
                  r.adjust(2,1,-2,-2);
                  fillWithMask(painter, r, gradient(c, r.width(), Qt::Horizontal, GradGloss), &masks.button, pf);
               }
               else
                  fillWithMask(painter, r, gradient(COLOR(Window), r.width(), Qt::Horizontal, GradSunken), &masks.button, pf);
               r = groove;
               r.setTop(handle.bottom()-3);
               pf = Tile::Left | Tile::Right | Tile::Bottom | Tile::Center;
               if (inverse)
                  fillWithMask(painter, r, gradient(COLOR(Window), r.width(), Qt::Horizontal, GradSunken), &masks.button, pf);
               else
               {
                  shadows.button[1].render(r, painter);
                  r.adjust(2,1,-2,-2);
                  fillWithMask(painter, r, gradient(c, r.width(), Qt::Horizontal, GradGloss), &masks.button, pf);
               }
            }
         }
         
         if (slider->subControls & SC_SliderTickmarks)
         {
            QStyleOptionSlider tmpSlider = *slider;
            tmpSlider.subControls = SC_SliderTickmarks;
            QCommonStyle::drawComplexControl(control, &tmpSlider, painter, widget);
         }
         
         if (slider->subControls & SC_SliderHandle)
         {
            // shadow
            QPoint xy = handle.topLeft();
            painter->drawPixmap(xy, shadows.radio[isEnabled]);
            // gradient
            xy += QPoint(dpi.$2,dpi.$2);
            GradientType gt = sunken ? GradSunken : (hover ?  GradGloss : GradButton);
            fillWithMask(painter, xy,
                     gradient(isEnabled?COLOR(Button):COLOR(Window), handle.height()-dpi.$4, Qt::Vertical, gt),
                     masks.radio);
            // focus indicator
            if (hasFocus)
            {
               painter->save();
               painter->setBrush(gradient(COLOR(Highlight), handle.height()-dpi.$4, Qt::Vertical, gt));
               painter->setPen(Qt::NoPen);
               painter->setRenderHint ( QPainter::Antialiasing );
               int w = handle.width()/3; int h = handle.height()/3;
               painter->setBrushOrigin(xy);
               painter->drawEllipse ( handle.adjusted(w,h,-w,-h) );
               painter->restore();
            }
         }
      }
      break;
   case CC_ToolButton: // A tool button, like QToolButton
      if (widget && widget->parent() && qobject_cast<QTabBar*>(widget->parent()))
      {
      // this is a qtabbar scrollbutton and needs to be erased...
         if (config.bgMode > Scanlines)
            painter->drawPixmap(RECT.topLeft(), widget->palette().brush(QPalette::Window).texture(), RECT.translated(widget->mapTo ( widget->topLevelWidget(), QPoint(0,0))));
         else
            painter->fillRect(RECT, PAL.brush(QPalette::Window));
//          painter->drawTiledPixmap(RECT.adjusted(0,2,0,-2), gradient(PAL.color(QPalette::Window), RECT.height()-4, Qt::Vertical, Raised));
      }
      if (const QStyleOptionToolButton *toolbutton
          = qstyleoption_cast<const QStyleOptionToolButton *>(option))
      {
         QRect button, menuarea;
         button = subControlRect(control, toolbutton, SC_ToolButton, widget);
         menuarea = subControlRect(control, toolbutton, SC_ToolButtonMenu, widget);
         
         State bflags = toolbutton->state;
             
         if (bflags & State_AutoRaise)
         {
            if (!hover)
            {
               bflags &= ~State_Raised;
            }
         }
         
         State mflags = bflags;
             
         if (toolbutton->activeSubControls & SC_ToolButton)
            bflags |= State_Sunken;
         if (toolbutton->activeSubControls & SC_ToolButtonMenu)
            mflags |= State_Sunken;
             
         QStyleOption tool(0);
         tool.palette = toolbutton->palette;
         if (toolbutton->subControls & SC_ToolButton)
         {
            if (isEnabled && (bflags & (State_Sunken | State_On | State_Raised | State_HasFocus)))
            {
               tool.rect = button;
               tool.state = bflags;
               drawPrimitive(PE_PanelButtonTool, &tool, painter, widget);
            }
         }
         
         if (toolbutton->subControls & SC_ToolButtonMenu)
         {
            tool.rect = menuarea;
            tool.state = mflags;
            if (mflags & (State_Sunken | State_On | State_Raised))
               drawPrimitive(PE_IndicatorButtonDropDown, &tool, painter, widget);
            drawPrimitive(PE_IndicatorArrowDown, &tool, painter, widget);
         }

         QStyleOptionToolButton label = *toolbutton;
         int fw = pixelMetric(PM_DefaultFrameWidth, option, widget);
         label.rect = button.adjusted(fw, fw, -fw, -fw);
//          label.rect = button.adjusted(4, 4, -4, -4);
         drawControl(CE_ToolButtonLabel, &label, painter, widget);
      }
      break;
   case CC_TitleBar: // A Title bar, like what is used in Q3Workspace
      if (const QStyleOptionTitleBar *tb =
          qstyleoption_cast<const QStyleOptionTitleBar *>(option))
      {
         painter->fillRect(RECT, PAL.brush(QPalette::Window));
         QRect ir;
         if (option->subControls & SC_TitleBarLabel)
         {
            ir = subControlRect(CC_TitleBar, tb, SC_TitleBarLabel, widget);
            painter->setPen(PAL.color(QPalette::WindowText));
            painter->drawText(ir.x() + 2, ir.y(), ir.width() - 2, ir.height(), Qt::AlignCenter | Qt::TextSingleLine, tb->text);
         }
         
         
#define PAINT_WINDOW_BUTTON(_scbtn_, _spbtn_)\
         {\
            tmpOpt.rect = subControlRect(CC_TitleBar, tb, _scbtn_, widget);\
            if (tb->activeSubControls & _scbtn_)\
               tmpOpt.state = tb->state;\
            else\
               tmpOpt.state &= ~(State_Sunken | State_MouseOver);\
            pm = standardPixmap ( _spbtn_, &tmpOpt, widget );\
            painter->drawPixmap(tmpOpt.rect.topLeft(), pm);\
         }
         
         QPixmap pm;
         QStyleOptionTitleBar tmpOpt = *tb;
         if (tb->subControls & SC_TitleBarCloseButton)
            PAINT_WINDOW_BUTTON(SC_TitleBarCloseButton, SP_TitleBarCloseButton)
         
         if (tb->subControls & SC_TitleBarMaxButton
             && tb->titleBarFlags & Qt::WindowMaximizeButtonHint)
         {
            if (tb->titleBarState & Qt::WindowMaximized)
               PAINT_WINDOW_BUTTON(SC_TitleBarNormalButton, SP_TitleBarNormalButton)
            else
               PAINT_WINDOW_BUTTON(SC_TitleBarMaxButton, SP_TitleBarMaxButton)
         }
         
         if (tb->subControls & SC_TitleBarMinButton
             && tb->titleBarFlags & Qt::WindowMinimizeButtonHint)
         {
            if (tb->titleBarState & Qt::WindowMinimized)
               PAINT_WINDOW_BUTTON(SC_TitleBarNormalButton, SP_TitleBarNormalButton)
            else
               PAINT_WINDOW_BUTTON(SC_TitleBarMinButton, SP_TitleBarMinButton)
         }
         
         if (tb->subControls & SC_TitleBarNormalButton &&
             tb->titleBarFlags & Qt::WindowMinMaxButtonsHint)
            PAINT_WINDOW_BUTTON(SC_TitleBarNormalButton, SP_TitleBarNormalButton)
         
         if (tb->subControls & SC_TitleBarShadeButton)
            PAINT_WINDOW_BUTTON(SC_TitleBarShadeButton, SP_TitleBarShadeButton)
         
         if (tb->subControls & SC_TitleBarUnshadeButton)
            PAINT_WINDOW_BUTTON(SC_TitleBarUnshadeButton, SP_TitleBarUnshadeButton)
         
         if (tb->subControls & SC_TitleBarContextHelpButton
             && tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
            PAINT_WINDOW_BUTTON(SC_TitleBarContextHelpButton, SP_TitleBarContextHelpButton)
         
         if (tb->subControls & SC_TitleBarSysMenu
             && tb->titleBarFlags & Qt::WindowSystemMenuHint)
         {
            if (!tb->icon.isNull())
            {
               ir = subControlRect(CC_TitleBar, tb, SC_TitleBarSysMenu, widget);
               tb->icon.paint(painter, ir);
            }
            else
               PAINT_WINDOW_BUTTON(SC_TitleBarSysMenu, SP_TitleBarMenuButton)
         }
      }
      break;
//    case CC_Q3ListView: // Used for drawing the Q3ListView class
   case CC_Dial: // A dial, like QDial
      if (const QStyleOptionSlider *dial =
          qstyleoption_cast<const QStyleOptionSlider *>(option))
      {
         painter->save();
         QRect rect = RECT;
         if (rect.width() > rect.height()) {
            rect.setLeft(rect.x()+(rect.width()-rect.height())/2); rect.setWidth(rect.height());
         }
         else {
            rect.setTop(rect.y()+(rect.height()-rect.width())/2); rect.setHeight(rect.width());
         }
         
         int d = rect.width()/6;
         int r = rect.width()/2-2*d/3;
         qreal a;
         if (dial->maximum == dial->minimum)
            a = M_PI / 2;
         else if (dial->dialWrapping)
            a = M_PI * 3 / 2 - (dial->sliderValue - dial->minimum) * 2 * M_PI
            / (dial->maximum - dial->minimum);
         else
            a = (M_PI * 8 - (dial->sliderValue - dial->minimum) * 10 * M_PI
                  / (dial->maximum - dial->minimum)) / 6;
         
         QPoint cp((int)(r * cos(a)), -(int)(r * sin(a)));
         cp += rect.center();
         
         // the huge ring
         painter->setBrushOrigin(rect.topLeft());
         painter->setBrush(gradient(PAL.background().color(), rect.height(), Qt::Vertical, GradSunken));
         painter->setPen(Qt::NoPen);
         painter->drawEllipse(rect);
         // the inner bevel
         painter->setBrush(gradient(PAL.background().color(), rect.height(), Qt::Vertical, GradSimple));
         rect.adjust(d,d,-d,-d);
         painter->drawEllipse(rect);
         // the value
         QFont fnt = painter->font(); fnt.setPixelSize( 2*rect.height()/3 ); painter->setFont(fnt);
         painter->setBrush(Qt::NoBrush); painter->setPen(PAL.foreground().color());
         drawItemText(painter, rect,  Qt::AlignCenter, PAL, isEnabled, QString::number(dial->sliderValue));
         // the drop
         painter->setPen(Qt::NoPen);
         painter->setRenderHint( QPainter::Antialiasing );
         rect = QRect(0,0,2*d/3,2*d/3);
         rect.moveCenter(cp);
         painter->setBrushOrigin(rect.topLeft());
         painter->setBrush(gradient(COLOR(Highlight), rect.height(), Qt::Vertical, GradButton));
         painter->drawEllipse(rect);
         painter->restore();
      }
      break;
   default:
      QCommonStyle::drawComplexControl( control, option, painter, widget );
   } // switch
#undef RECT
#undef PAL
#undef COLOR
}
