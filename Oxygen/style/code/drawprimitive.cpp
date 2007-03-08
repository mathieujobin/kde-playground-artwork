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

#include <QApplication>
#include <QAbstractScrollArea>
#include <QDesktopWidget>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QStyleOptionButton>
#include <QPainter>
#include <QPainterPath>
#include <QX11Info>
#include "oxygen.h"
#include "oxrender.h"
#include "dynamicbrush.h"

#include <QtDebug>

#include "inlinehelp.cpp"

using namespace Oxygen;

extern int bgYoffset_;
extern Pixmap shadowPix;
extern Config config;
extern Dpi dpi;

void OxygenStyle::drawPrimitive ( PrimitiveElement pe, const QStyleOption * option, QPainter * painter, const QWidget * widget) const
{
   Q_ASSERT(option);
   Q_ASSERT(painter);
#define RECT option->rect
#define PAL option->palette
#define COLOR(_TYPE_) option->palette.color(QPalette::_TYPE_)
   
   bool sunken = option->state & State_Sunken;
   bool isEnabled = option->state & State_Enabled;
   bool hover = isEnabled && (option->state & State_MouseOver);
   bool hasFocus = option->state & State_HasFocus;
   bool up = false;
   
   
   switch ( pe )
   {
   case PE_FrameDefaultButton: // This frame around a default button, e.g. in a dialog.
      // we swap colors instead, frame only on focus!
      break;
   case PE_PanelButtonCommand: // Button used to initiate an action, for example, a QPushButton.
   case PE_PanelButtonBevel: // Generic panel with a button bevel.
   {
      int $1 = dpi.$1, $2 = dpi.$2;
      const QStyleOptionButton *opt = qstyleoption_cast<const QStyleOptionButton*>(option);
      bool isOn = option->state & State_On;
      bool isDefault = opt && (opt->features & QStyleOptionButton::DefaultButton);
      GradientType gt = GradButton;
      if (!isEnabled) gt = GradButtonDisabled;
      else if (sunken) gt = GradSunken;
      else if (hover) gt = GradGloss;
      else if (isDefault) gt = GradSimple;
      
      QColor c = btnBgColor(PAL, isEnabled, hasFocus, isDefault);

      shadows.button[isEnabled].render(RECT, painter, Tile::Ring);
      QRect ir = RECT.adjusted($2,$1,-$2,-dpi.$3);
      fillWithMask(painter, ir, gradient(c, ir.height(), Qt::Vertical, gt), &masks.button);
      frames.button[isEnabled].render(ir,painter,Tile::Ring);
//       painter->drawPixmap(ir.right()-16*ir.height()/9,ir.y(), btnAmbient(ir.height()));
      int off = ir.width()/6;
      lights.top.render(ir.adjusted(off,0,-off,0),painter,Tile::Left|Tile::Center|Tile::Right);
      break;
   }
   case PE_PanelButtonTool: // Panel for a Tool button, used with QToolButton.
   {
      if (sunken || (option->state & State_On))
      {
         if (sunken) hover = false;
         fillWithMask(painter, RECT, gradient(COLOR(Window),RECT.height(),Qt::Vertical, hover ? GradButton : GradSunken), &masks.button);
         shadows.lineEdit[1].render(RECT, painter);
      }
      else if (hover)
      {
         fillWithMask(painter, RECT.adjusted(dpi.$2,dpi.$1,-dpi.$2,0), gradient(COLOR(Window), RECT.height(), Qt::Vertical, GradButton), &masks.button);
         shadows.group.render(RECT, painter, Tile::Ring);
      }
      break;
   }
   case PE_PanelLineEdit: // Panel for a QLineEdit.
   {
      // spinboxes and combos allready have a lineedit as global frame
      if (widget && widget->parentWidget() &&
          (widget->parentWidget()->inherits("QSpinBox") ||
           widget->parentWidget()->inherits("QComboBox")))
         break;
      if (qstyleoption_cast<const QStyleOptionFrame *>(option) &&
          static_cast<const QStyleOptionFrame *>(option)->lineWidth < 1)
      {
         painter->fillRect(RECT, COLOR(Base)); break;
      }
      QRect rect = RECT.adjusted(0,0,0,-dpi.$2);
      fillWithMask(painter, rect, hasFocus?midColor(COLOR(Base),COLOR(Highlight),20,1):COLOR(Base), &masks.button);
      shadows.lineEdit[isEnabled].render(rect, painter);
      break;
   }
   case PE_FrameFocusRect: // Generic focus indicator.
   {
      painter->save();
      painter->setBrush(Qt::NoBrush);
      painter->setPen(COLOR(Highlight));
      painter->drawRect(RECT.adjusted(0,0,-1,-1));
      painter->restore();
      break;
   }
   case PE_IndicatorArrowUp: // Generic Up arrow.
   case PE_IndicatorSpinUp: // Up symbol for a spin widget, for example a QSpinBox.
   case PE_IndicatorSpinPlus: // Increase symbol for a spin widget.
      up = true;
   case PE_IndicatorHeaderArrow: // Arrow used to indicate sorting on a list or table header
   case PE_IndicatorButtonDropDown: // indicator for a drop down button, for example, a tool button that displays a menu.
   case PE_IndicatorArrowDown: // Generic Down arrow.
   case PE_IndicatorSpinDown: // Down symbol for a spin widget.
   case PE_IndicatorSpinMinus: // Decrease symbol for a spin widget.
   {
      if (const QStyleOptionHeader* hopt = qstyleoption_cast<const QStyleOptionHeader*>(option))
      {
         if (hopt->sortIndicator == QStyleOptionHeader::None)
            break;
         up = hopt->sortIndicator == QStyleOptionHeader::SortUp;
      }
      bool hadAntiAlias = painter->renderHints() & QPainter::Antialiasing;
      QBrush oldBrush = painter->brush();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setBrush(painter->pen().brush());
      
      // we want a golden mean cut arrow ;) 1:1.6180339887498948482
      int x[3], y[2];
      if (RECT.width() <  8*RECT.height()/5)
      {
         x[0] = RECT.x(); x[1] = RECT.right(); x[2] = x[0] + RECT.width()/2;
         int h = 5*RECT.width()/8;
         y[0] = RECT.y() + (RECT.height()-h)/2; y[1] = y[0] + h;
      }
      else
      {
         int w = 8*RECT.height()/5;
         x[0] = RECT.x() + (RECT.width()-w)/2; x[1] = x[0] + w; x[2] = x[0] + w/2;
         y[0] = RECT.y(); y[1] = RECT.bottom();
      }
      if (up)
      {
         const QPoint points[3] =  { QPoint(x[0], y[1]), QPoint(x[1], y[1]), QPoint(x[2], y[0]) };
         painter->drawPolygon(points, 3);
      }
      else
      {
         const QPoint points[3] =  { QPoint(x[0], y[0]), QPoint(x[1], y[0]), QPoint(x[2], y[1]) };
         painter->drawPolygon(points, 3);
      }
      painter->setBrush(oldBrush);
      painter->setRenderHint(QPainter::Antialiasing, hadAntiAlias);
      break;
   }
   case PE_IndicatorArrowRight: // Generic Right arrow.
      up = true;
   case PE_IndicatorArrowLeft: // Generic Left arrow.
   {
      bool hadAntiAlias = painter->renderHints() & QPainter::Antialiasing;
      QBrush oldBrush = painter->brush();
      painter->setRenderHint(QPainter::Antialiasing);
      painter->setBrush(painter->pen().color());
      int x[2], y[3];
      if (RECT.height() <  8*RECT.width()/5)
      {
         y[0] = RECT.y(); y[1] = RECT.bottom(); y[2] = y[0] + RECT.height()/2;
         int w = 5*RECT.height()/8;
         x[0] = RECT.x() + (RECT.width()-w)/2; x[1] = x[0] + w;
      }
      else
      {
         int h = 8*RECT.width()/5;
         y[0] = RECT.y() + (RECT.height()-h)/2; y[1] = y[0] + h; y[2] = y[0] + h/2;
         x[0] = RECT.x(); x[1] = RECT.right();
      }
      if (up) //right
      {
         const QPoint points[3] =  { QPoint(x[0], y[0]), QPoint(x[0], y[1]), QPoint(x[1], y[2]) };
         painter->drawPolygon(points, 3);
      }
      else
      {
         const QPoint points[3] =  { QPoint(x[0], y[2]), QPoint(x[1], y[0]), QPoint(x[1], y[1]) };
         painter->drawPolygon(points, 3);
      }
      painter->setBrush(oldBrush);
      painter->setRenderHint(QPainter::Antialiasing, hadAntiAlias);
      break;
   }
   case PE_IndicatorCheckBox: // On/off indicator, for example, a QCheckBox.
   {
      GradientType gt = GradButton;
      if (!isEnabled) gt = GradButtonDisabled;
      else if (sunken) gt = GradSunken;
      else if (hover) gt = GradGloss;

      int $2 = dpi.$2; int $1 = dpi.$1;

      shadows.button[isEnabled].render(RECT,painter,Tile::Ring);
      
      QColor c = btnBgColor(PAL, isEnabled, hasFocus);
      
      QRect ir = RECT.adjusted($2,dpi.$1,-$2,-dpi.$3);
      fillWithMask(painter, ir, gradient(c, ir.height(), Qt::Vertical, gt), &masks.button);
//       if (hasFocus)
//          masks.button.outline(ir, painter, COLOR(Highlight));
      frames.button[isEnabled].render(ir,painter,Tile::Ring);
      
      if (!(sunken || (option->state & State_Off)))
      {
         c = btnFgColor(PAL, isEnabled, hasFocus);
         QPen oldPen = painter->pen();
         bool hadAntiAlias = painter->renderHints() & QPainter::Antialiasing;
         painter->setRenderHint(QPainter::Antialiasing);
         QPen pen(gradient(c, ir.height(), Qt::Vertical, gt), ir.width()/6, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin);
         painter->setPen(pen);
         int $3 = dpi.$3;
         QRect r = ir.adjusted($3,$3,-$3,-$3);
         switch (config.checkType)
         {
         default:
         case 0:
            painter->drawLine(r.x(),r.bottom(),r.right(),r.y());
            if (option->state & State_On)
               painter->drawLine(r.x(),r.y(),r.right(),r.bottom());
            break;
         case 1:
         {
            const QPoint points[4] = {
            QPoint(r.right(), r.top()),
            QPoint(r.x()+r.width()/3, r.bottom()),
            QPoint(r.x(), r.bottom()-r.height()/3),
            QPoint(r.x()+r.width()/3, r.bottom()-r.height()/5)
            };
            painter->drawPolygon(points, 4);
            break;
         }
         case 2:
            fillWithMask(painter, r, pen.brush(), &masks.button);
         }
         painter->setPen(oldPen);
         painter->setRenderHint(QPainter::Antialiasing, hadAntiAlias);
      }
//       painter->drawPixmap(ir.right()-16*ir.height()/9,ir.y(), btnAmbient(ir.height()));
      break;
   }
   case PE_IndicatorRadioButton: // Exclusive on/off indicator, for example, a QRadioButton.
   {
      bool isOn = option->state & State_On;
      sunken = sunken || isOn;
      hover = hover && !isOn;
      int $2 = dpi.$2;
      
      QColor c = btnBgColor(PAL, isEnabled, hasFocus);
      GradientType gt = hover ? GradRadialGloss : GradButton;
      QPoint xy = RECT.topLeft();
      painter->drawPixmap(xy, shadows.radio[isEnabled]);
      
      xy += QPoint($2,dpi.$1);
      int sz = dpi.ExclusiveIndicator - dpi.$4;
      fillWithMask(painter, xy, gradient(c, sz, Qt::Vertical, gt), masks.radio);
//       if (hasFocus)
//       {
//          painter->save();
//          painter->setPen(COLOR(Highlight)); painter->setBrush(Qt::NoBrush);
//          painter->setRenderHint(QPainter::Antialiasing);
//          painter->drawEllipse(xy.x(), xy.y(), sz, sz);
//          painter->restore();
//       }
      xy += QPoint($2,$2);
      sz -= dpi.$4;
      if (isEnabled)
         fillWithMask(painter, xy, gradient(c, sz, Qt::Vertical, GradSunken), masks.radioGroove);
      if (isOn)
      {
         QColor c = btnFgColor(PAL, isEnabled, hasFocus);
         xy += QPoint($2,$2);
         fillWithMask(painter, xy, gradient(c, sz, Qt::Vertical, GradRadialGloss), masks.radioIndicator);
      }
      break;
   }
   case PE_Q3DockWindowSeparator: // Item separator for Qt 3 compatible dock window and toolbar contents.
      break;
   case PE_Frame: { // Generic frame; see also QFrame.
      if (widget) {
         // handle the
         if (widget->inherits("QComboBoxPrivateContainer")) {
            QPen oldPen = painter->pen();
            painter->setPen(PAL.color(QPalette::Window).dark(120));
            painter->drawLine(RECT.x(), RECT.top(), RECT.x(), RECT.bottom());
            painter->drawLine(RECT.x(), RECT.bottom(), RECT.right(), RECT.bottom());
            painter->drawLine(RECT.right(), RECT.top(), RECT.right(), RECT.bottom());
            
            painter->setPen(PAL.color(QPalette::Base));
            painter->drawLine(RECT.x(), RECT.top(), RECT.right(), RECT.top());
            painter->setPen(oldPen);
            break;
         }
         
         bool inverse = true, niceFrame = false; QRect rect = RECT; const QRect *outerRect = 0;
         
         const Tile::Mask *mask; const Tile::Set *shadow;
         if (sunken) {
            shadow = &shadows.lineEdit[isEnabled];
            mask = &masks.button;
         }
         else if (option->state & State_Raised) {
            shadow = &shadows.tab;
            mask = &masks.tab;
         }
         else {
            shadow = &shadows.relief;
            mask = &masks.button;
         }
         
         QPoint zero; QPalette::ColorRole role = widget->backgroundRole();
         if (qobject_cast<const QFrame*>(widget)) { // frame, can be killed unless...
            if (widget->inherits("QTextEdit")) { // ...it's a TextEdit!
               niceFrame = true;
               inverse = false; role = QPalette::Base;
            }
            else { // maybe we need to corect a textlabels margin
               if (const QLabel* label = qobject_cast<const QLabel*>(widget))
                  if (label->text() != QString() && label->margin() < dpi.$3)
                     const_cast<QLabel*>(label)->setMargin(dpi.$3);
               break; // painted on visual frame
            }
         }
         else if (qobject_cast<const VisualFrame*>(widget)) {
            niceFrame = true;
            zero = widget->mapTo(widget->topLevelWidget(), QPoint(0,0));
            if (!sunken) {
               outerRect = &RECT;
               if (option->state & State_Raised)
                  rect = RECT.adjusted(dpi.$2,dpi.$1,-dpi.$2,-dpi.$4);
               else
                  rect = RECT.adjusted(dpi.$2,dpi.$2,-dpi.$2,-dpi.$2);
            }
         }
         if (niceFrame) {
            fillWithMask(painter, rect, PAL.brush(role), mask, Tile::Full, false, zero, inverse, outerRect);
            shadow->render(RECT, painter);
            break;
         }
      }
      // fallback, we cannot paint shaped frame contents
      if (sunken)
         shadows.sunken.render(RECT,painter);
//       else if (option->state & State_Raised)
//          shadows.raised.render(RECT,painter);
//       else
//          shadows.raised.render(RECT,painter);
      break;
   }
   case PE_FrameMenu: // Frame for popup windows/menus; see also QMenu.
   {
      QPen oldPen = painter->pen();
      QColor c = PAL.color(widget?widget->backgroundRole():QPalette::Window);
      painter->setPen(c);
      painter->drawLine(RECT.x(), RECT.top(), RECT.right(), RECT.top());
      painter->setPen(c.dark(110));
      painter->drawLine(RECT.x(), RECT.top(), RECT.x(), RECT.bottom());
      painter->drawLine(RECT.x(), RECT.bottom(), RECT.right(), RECT.bottom());
      painter->drawLine(RECT.right(), RECT.top(), RECT.right(), RECT.bottom());
      painter->setPen(oldPen);
      break;
   }
   case PE_PanelMenuBar: // Panel for menu bars.
   case PE_FrameDockWidget: // Panel frame for dock windows and toolbars.
      break;
   case PE_FrameTabWidget: // Frame for tab widgets.
      if (const QStyleOptionTabWidgetFrame *twf =
          qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option))
      {
         QRect rect(RECT), tabRect(RECT);
         int baseHeight = pixelMetric( PM_TabBarBaseHeight, option, widget )-1;
         int offset = 8;
         Qt::Orientation o = Qt::Vertical;
         Tile::PosFlags pf = 0;
         switch (twf->shape)
         {
         case QTabBar::RoundedNorth:
         case QTabBar::TriangularNorth:
            rect.adjust(offset,0,-offset,0);
            rect.setHeight(baseHeight);
            pf = Tile::Left | Tile::Right | Tile::Top;
            tabRect.setTop(tabRect.top()+(baseHeight-dpi.$1));
            break;
         case QTabBar::RoundedSouth:
         case QTabBar::TriangularSouth:
            rect.adjust(offset,0,-offset,0);
            rect.setTop(rect.bottom()-baseHeight);
            pf = Tile::Left | Tile::Right | Tile::Bottom;
            tabRect.setBottom(tabRect.bottom()-(baseHeight-dpi.$1));
            break;
         case QTabBar::RoundedEast:
         case QTabBar::TriangularEast:
            rect.adjust(0,offset,0,-offset);
            rect.setLeft(rect.right()-baseHeight);
            pf = Tile::Top | Tile::Right | Tile::Bottom;
            o = Qt::Horizontal;
            tabRect.setRight(tabRect.right()-(baseHeight-dpi.$1));
            break;
         case QTabBar::RoundedWest:
         case QTabBar::TriangularWest:
            rect.adjust(0,offset,0,-offset);
            rect.setWidth(baseHeight);
            pf = Tile::Top | Tile::Left | Tile::Bottom;
            o = Qt::Horizontal;
            tabRect.setLeft(tabRect.left()+(baseHeight-dpi.$1));
            break;
         }
         shadows.tab.render(rect, painter, pf);
         rect.adjust(dpi.$3,dpi.$2,-dpi.$3,0);
         fillWithMask(painter, rect, gradient(COLOR(WindowText), baseHeight, o, GradGloss), &masks.tab, pf | Tile::Center);
         masks.tab.outline(rect, painter, COLOR(WindowText).dark(110), pf);
         shadows.tab.render(tabRect, painter, Tile::Ring);
      }
      break;
   case PE_FrameLineEdit: // Panel frame for line edits.
      shadows.lineEdit[isEnabled].render(RECT.adjusted(0,0,0,-dpi.$2),painter);
   case PE_FrameGroupBox: // Panel frame around group boxes.
   {
//       int darkness = 5; // compromise ;) - shouldn't happen anyway
//       if (widget)
//       {
//          // find proper color value for the bottom (because of out general gradient)
//          // 0% -> darkness = 0, 100% -> darkness = 10
//          // we could also simply create a gradient into translucent - but that can get slow... :(
//          QWidget *tlw = widget->topLevelWidget();
//          QPoint zero(0,0); zero = widget->mapTo(tlw, zero);
//          darkness = (10*(zero.y()+widget->height()))/tlw->height();
//       }
      QRect rect = RECT.adjusted(dpi.$4,dpi.$2,-dpi.$4,-dpi.$32);
      QColor c = COLOR(Window);
      fillWithMask(painter, rect, gradient(c, rect.height(), Qt::Vertical, GradGroup), &masks.button,
                   Tile::Left|Tile::Top|Tile::Right);
      rect = RECT.adjusted(dpi.$4,0,-dpi.$4,0); rect.setTop(rect.bottom()-dpi.$32+dpi.$1);
      painter->drawTiledPixmap(rect, gradient(c, rect.height(), Qt::Horizontal, GradGroup));
      shadows.group.render(RECT, painter, Tile::Ring);
      break;
   }
//    case PE_FrameButtonBevel: // Panel frame for a button bevel
//    case PE_FrameButtonTool: // Panel frame for a tool button
   case PE_FrameStatusBar: // Frame for a section of a status bar; see also QStatusBar.
      break;
   case PE_FrameWindow: // Frame around a MDI window or a docking window.
   {
      painter->save();
      painter->setPen(PAL.color(QPalette::Window).dark(110));
      painter->drawRect(RECT);
      painter->restore();
      break;
   }
   case PE_Q3Separator: // Qt 3 compatible generic separator.
      break;
   case PE_Q3CheckListIndicator: // Qt 3 compatible Checkbox part of a list view item.
   case PE_IndicatorMenuCheckMark: // Check mark used in a menu.
   case PE_IndicatorViewItemCheck: // On/off indicator for a view item
   {
      QRect rect;
      if (RECT.width() > RECT.height())
         rect.setRect(RECT.x()+(RECT.width()-RECT.height())/2, RECT.y(), RECT.height(), RECT.height());
      else
         rect.setRect(RECT.x(), RECT.y()+(RECT.height()-RECT.width())/2, RECT.width(), RECT.width());
      int off = rect.width()/4;
      painter->drawRect(rect.adjusted(0, off, -off, 0));
      if (option->state & State_On)
      {
         bool hadAntiAlias = painter->renderHints() & QPainter::Antialiasing;
         QBrush oldBrush = painter->brush();
         painter->setRenderHint(QPainter::Antialiasing);
         painter->setBrush(painter->pen().brush());
         const QPoint points[4] = 
         {
            QPoint(rect.right(), rect.top()),
            QPoint(rect.x()+rect.width()/3, rect.bottom()),
            QPoint(rect.x(), rect.bottom()-rect.height()/3),
               QPoint(rect.x()+rect.width()/3, rect.bottom()-rect.height()/5)
         };
         painter->drawPolygon(points, 4);
         painter->setRenderHint(QPainter::Antialiasing, hadAntiAlias);
         painter->setBrush(oldBrush);
      }
      break;
   }
   case PE_Q3CheckListExclusiveIndicator: // Qt 3 compatible Radio button part of a list view item.
      painter->save();
      painter->setRenderHint ( QPainter::Antialiasing );
      painter->drawEllipse ( RECT );
      _PRINTFLAGS_;
      if (option->state & State_On)
      {
         painter->setBrush ( painter->pen().color() );
         painter->drawEllipse ( RECT.adjusted(RECT.width()/4, RECT.height()/4, -RECT.width()/4, -RECT.height()/4) );
      }
      painter->restore();
      break;
//    case PE_IndicatorProgressChunk: // Section of a progress bar indicator; see also QProgressBar.
//    case PE_Q3CheckListController: // Qt 3 compatible Controller part of a list view item.
   case PE_IndicatorBranch: // Lines used to represent the branch of a tree in a tree view.
   {
      painter->save();
      int mid_h = RECT.x() + RECT.width() / 2;
      int mid_v = RECT.y() + RECT.height() / 2;
      int bef_h = mid_h;
      int bef_v = mid_v;
      int aft_h = mid_h;
      int aft_v = mid_v;
      
      QColor oc = painter->pen().color();
      painter->setPen(widget ?
                      midColor( PAL.color(widget->backgroundRole()), PAL.color(widget->foregroundRole())) :
                      midColor( PAL.color(QPalette::Base), PAL.color(QPalette::Text)) );
      static const int decoration_size = 9;
      if (option->state & State_Children)
      {
         int delta = decoration_size / 2 + 2;
         bef_h -= delta;
         bef_v -= delta;
         aft_h += delta;
         aft_v += delta;
         QStyleOption tmpOpt = *option;
         tmpOpt.rect = QRect(bef_h+2, bef_v+2, decoration_size, decoration_size);
         drawPrimitive(option->state & State_Open ? PE_IndicatorArrowDown :
                       option->direction == Qt::RightToLeft ? PE_IndicatorArrowLeft :
                       PE_IndicatorArrowRight, &tmpOpt, painter, widget);
      }
      if (RECT.x() ==  -1) // this is for the first col and i don't see why we'd need a line here
      {
         painter->restore();
         break;
      }
      
      if (option->state & (State_Item | State_Sibling))
         painter->drawLine(mid_h, RECT.y(), mid_h, bef_v);
      if (option->state & State_Sibling)
         painter->drawLine(mid_h, aft_v, mid_h, RECT.bottom());
      if (option->state & State_Item)
      {
         if (option->direction == Qt::RightToLeft)
            painter->drawLine(RECT.left(), mid_v, bef_h, mid_v);
         else
            painter->drawLine(aft_h, mid_v, RECT.right(), mid_v);
      }
      painter->restore();
      break;
   }
   case PE_IndicatorDockWidgetResizeHandle: // Resize handle for dock windows.
   {
      QPoint *points; int num;
      int $12 = dpi.$12, $6 = dpi.$6;
      if (RECT.width() > RECT.height()) {
         int x = RECT.left()+RECT.width()/3;
         int y = RECT.top()+(RECT.height()-$6)/2;
         num = RECT.width()/(3*$12);
         if ((RECT.width()/3) % $12) ++num;
         points = new QPoint[num];
         for (int i = 0; i < num; ++i) {
            points[i] = QPoint(x,y); x += $12;
         }
      }
      else {
         int x = RECT.left()+(RECT.width()-$6)/2;
         int y = RECT.top()+RECT.height()/3;
         num = RECT.height()/(3*$12);
         if ((RECT.height()/3) % $12) ++num;
         points = new QPoint[num];
         for (int i = 0; i < num; ++i) {
            points[i] = QPoint(x,y); y += $12;
         }
      }
      painter->save();
      painter->setPen(Qt::NoPen);
      const QPixmap *fill; int cnt = num/2, imp = 1;
      QPalette::ColorRole role = QPalette::WindowText;
      if (hover) {
         role = QPalette::Highlight;
         imp = 8;
      }
      if (num%2)
      {
         fill = &gradient(midColor(COLOR(Window), PAL.color(role), 3, imp), $6, Qt::Vertical, GradSunken);
         fillWithMask(painter, points[cnt], *fill, masks.notch);
      }
      --num;
      for (int i = 0; i < cnt; ++i)
      {
         fill = &gradient(midColor(COLOR(Window), PAL.color(role), 3+cnt-i, imp), $6, Qt::Vertical, GradSunken);
         fillWithMask(painter, points[i], *fill, masks.notch);
         fillWithMask(painter, points[num-i], *fill, masks.notch);
      }
      painter->restore();
      delete points;
      break;
   }
   case PE_IndicatorToolBarHandle: // The handle of a toolbar.
      if (!(widget && widget->parentWidget()) ||
          widget->parentWidget()->underMouse()) {
         painter->save();
         QRect rect = RECT; bool line = false; int dx, dy;
         if (RECT.width() > RECT.height()) {
            line = (RECT.width() > 9*RECT.height()/2);
            if (line) {
               dx = 3*RECT.height()/2; dy = 0;
            }
            rect.setLeft(rect.left()+(rect.width()-rect.height())/2);
            rect.setWidth(rect.height());
         }
         else {
            line = (RECT.height() > 3*RECT.width());
            if (line) {
               dx = 0; dy = 3*RECT.width()/2;
            }
            rect.setTop(rect.top()+(rect.height()-rect.width())/2);
            rect.setHeight(rect.width());
         }
         QColor c = hover?COLOR(Highlight):COLOR(Window).dark(110);
         painter->setRenderHint(QPainter::Antialiasing);
         painter->setBrush(gradient(c, rect.height(), Qt::Vertical, GradSunken));
         painter->setPen(Qt::NoPen);
         painter->setBrushOrigin(rect.topLeft());
         painter->drawEllipse(rect);
         if (line) {
            int $1 = dpi.$1;
            rect.adjust($1,$1,-$1,-$1);
            painter->setBrush(gradient(c, rect.height(), Qt::Vertical, GradSunken));
            rect.translate(-dx,-dy);
            painter->setBrushOrigin(rect.topLeft());
            painter->drawEllipse(rect);
            rect.translate( 2*dx, 2*dy);
            painter->setBrushOrigin(rect.topLeft());
            painter->drawEllipse(rect);
         }
         painter->restore();
      }
      break;
   case PE_IndicatorToolBarSeparator: // The separator in a toolbar.
      break;
   case PE_PanelToolBar: // The panel for a toolbar.
      break;
//    case PE_PanelTipLabel: // The panel for a tip label.
   case PE_FrameTabBarBase: // The frame that is drawn for a tabbar, ususally drawn for a tabbar that isn't part of a tab widget
      if (const QStyleOptionTabBarBase *tbb
            = qstyleoption_cast<const QStyleOptionTabBarBase *>(option))
      {
         QRegion region(tbb->rect);
         region -= tbb->selectedTabRect;
         painter->save();
         painter->setClipRegion(region);
         int size; Qt::Orientation o;
         switch (tbb->shape)
         {
         case QTabBar::RoundedNorth:
         case QTabBar::TriangularNorth:
         case QTabBar::RoundedSouth:
         case QTabBar::TriangularSouth:
            o = Qt::Vertical;
            size = RECT.height();
            break;
         case QTabBar::RoundedWest:
         case QTabBar::TriangularWest:
         case QTabBar::RoundedEast:
         case QTabBar::TriangularEast:
            o = Qt::Horizontal;
            size = RECT.height();
            break;
         }
         fillWithMask(painter, RECT, gradient(COLOR(WindowText), size, o, GradGloss), &masks.tab);
         shadows.tab.render(RECT, painter);
         painter->restore();
      }
      break;
   case PE_IndicatorTabTear: // An indicator that a tab is partially scrolled out of the visible tab bar when there are many tabs.
      break;
   default:
      QCommonStyle::drawPrimitive( pe, option, painter, widget );
   } // switch
#undef RECT
#undef PAL
#undef COLOR
}
