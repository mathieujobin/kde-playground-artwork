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
#include <QDesktopWidget>
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
//       bool isIconButton = opt && opt->text.isEmpty() && !opt->icon.isNull();
      bool isOn = option->state & State_On;
      bool isDefault = opt && (opt->features & QStyleOptionButton::DefaultButton);
      GradientType gt = !isEnabled ? GradButtonDisabled : (sunken ? GradSunken : (hover ? GradGloss : GradButton));
      QRect shadowRect = !isEnabled ? RECT.adjusted($2,$1,-$2,-$1) : RECT;
      
      QColor c = !isEnabled ? COLOR(Window) :  COLOR(Button);

      shadows.button[isEnabled].render(shadowRect, painter, Tile::Ring);
      QRect ir = RECT.adjusted($2,$1,-$2,-$2);
      fillWithMask(painter, ir, gradient(c, ir.height(), Qt::Vertical, gt), &masks.button);
      if (hasFocus)
         masks.button.outline(ir, painter, COLOR(Highlight));
      frames.button[isEnabled].render(ir,painter,Tile::Ring);
      painter->drawPixmap(ir.right()-16*ir.height()/9,ir.y(), btnAmbient(ir.height()));
      int off = ir.width()/6;
      lights.top.render(ir.adjusted(off,0,-off,0),painter,Tile::Left|Tile::Top|Tile::Right);
      break;
   }
   case PE_PanelButtonTool: // Panel for a Tool button, used with QToolButton.
   {
      if (sunken || (option->state & QStyle::State_On))
      {
         if (sunken) hover = false;
         const QPixmap *fill = &gradient(COLOR(Window),RECT.height(),Qt::Vertical, hover ? GradSimple : GradSunken);
         fillWithMask(painter, RECT, *fill, &masks.round[Sunken]);
         renderFrame( painter, RECT, Sunken, Tile::Ring, widget, hasFocus, true );
      }
      else if (hover)
         renderFrame( painter, RECT, Relief, Tile::Ring, widget, hasFocus, true );
      break;
   }
   case PE_PanelLineEdit: // Panel for a QLineEdit.
   {
      if (const QLineEdit* le = qobject_cast<const QLineEdit*>(widget))
      {
         // spinboxes allready have a lineedit as global frame
         if (widget->parentWidget() && widget->parentWidget()->inherits("QSpinBox"))
            break;
         // no frame, so don't paint one
         if (!le->hasFrame()) {
            painter->fillRect(RECT, COLOR(Base)); break;
         }
      }
      fillWithMask(painter, RECT, COLOR(Base), &masks.button);
      shadows.sunken.render(RECT, painter);
      if (hasFocus)
         masks.button.outline(RECT.adjusted(1,1,-1,0), painter, COLOR(Highlight));
   }
   case PE_FrameFocusRect: // Generic focus indicator.
   {
      QPen pen = painter->pen();
      QPen oldPen = pen;
      pen.setStyle(Qt::DashDotLine);
      pen.setColor(PAL.color(QPalette::Highlight));
      painter->setPen(pen);
      painter->setPen(oldPen);
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
      if (RECT.width() <  RECT.height())
      {
         x[0] = RECT.x(); x[1] = RECT.right(); x[2] = x[0] + RECT.width()/2;
         int h = RECT.width();
         y[0] = RECT.y() + (RECT.height()-h)/2; y[1] = y[0] + h;
      }
      else
      {
         int w = RECT.height();
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
      if (RECT.height() <  RECT.width())
      {
         y[0] = RECT.y(); y[1] = RECT.bottom(); y[2] = y[0] + RECT.height()/2;
         int w = RECT.height();
         x[0] = RECT.x() + (RECT.width()-w)/2; x[1] = x[0] + w;
      }
      else
      {
         int h = RECT.width();
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
      GradientType gt = !isEnabled ? GradButtonDisabled : (sunken ? GradSunken : (hover ? GradGloss : GradButton));

      int $2 = dpi.$2;
      shadows.button[isEnabled].render(isEnabled?RECT:RECT.adjusted($2,dpi.$1,-$2,-dpi.$1),painter,Tile::Ring);
      
      QColor c = isEnabled ? COLOR(Button) : COLOR(Window);
      
      QRect ir = RECT.adjusted($2,dpi.$1,-$2,-$2);
      fillWithMask(painter, ir, gradient(c, ir.height(), Qt::Vertical, gt), &masks.button);
      if (hasFocus)
         masks.button.outline(ir, painter, COLOR(Highlight));
      frames.button[isEnabled].render(ir,painter,Tile::Ring);
      
      if (!(sunken || (option->state & State_Off)))
      {
         c = isEnabled ? COLOR(ButtonText) : midColor(COLOR(Window), COLOR(WindowText));
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
      painter->drawPixmap(ir.right()-16*ir.height()/9,ir.y(), btnAmbient(ir.height()));
      break;
   }
   case PE_IndicatorRadioButton: // Exclusive on/off indicator, for example, a QRadioButton.
   {
      bool isOn = option->state & State_On;
      sunken = sunken || isOn;
      hover = hover && !isOn;
      GradientType gt = sunken ? GradSunken : (hover ? GradGloss : GradButton);
      QPoint xy = RECT.topLeft();
      painter->drawPixmap(xy, shadows.radio[isEnabled]);
      int $2 = dpi.$2;
      xy += QPoint($2,dpi.$1);
      fillWithMask(painter, xy,
                     gradient(isEnabled?COLOR(Button):COLOR(Window), RECT.height(), Qt::Vertical, gt),
                     masks.radio, xy);
      if (hasFocus)
      {
         painter->save();
         painter->setRenderHint(QPainter::Antialiasing);
         painter->setPen(COLOR(Highlight));
         painter->setBrush(Qt::NoBrush);
         painter->drawEllipse(RECT.adjusted($2,dpi.$1,-$2,-dpi.$3));
         painter->restore();
      }
      if (isOn)
      {
         QColor c = isEnabled ? COLOR(ButtonText) : midColor(COLOR(Window),COLOR(WindowText));
         xy += QPoint($2,$2);
         fillWithMask(painter, xy, gradient(c, RECT.height(), Qt::Vertical, GradGloss), masks.radioIndicator, xy);
      }
      break;
   }
   case PE_Q3DockWindowSeparator: // Item separator for Qt 3 compatible dock window and toolbar contents.
      break;
   case PE_Frame: // Generic frame; see also QFrame.
      if (widget && widget->inherits("QComboBoxPrivateContainer"))
      {
         QPen oldPen = painter->pen();
         painter->setPen(PAL.color(QPalette::Window).dark(120));
         painter->drawLine(RECT.x(), RECT.top(), RECT.x(), RECT.bottom());
         painter->drawLine(RECT.x(), RECT.bottom(), RECT.right(), RECT.bottom());
         painter->drawLine(RECT.right(), RECT.top(), RECT.right(), RECT.bottom());
         
         painter->setPen(PAL.color(QPalette::Base));
         painter->drawLine(RECT.x(), RECT.top(), RECT.right(), RECT.top());
//          QPen pen = painter->pen();
//          QVector<qreal> dashes; dashes << 8 << 16;
//          pen.setWidth(2);
//          pen.setDashPattern(dashes);
//          pen.setColor(midColor(PAL.color(QPalette::Base),PAL.color(QPalette::Text)));
//          painter->setPen(pen);
//          painter->drawLine(RECT.x(), RECT.top(), RECT.right(), RECT.top());
         painter->setPen(oldPen);
         break;
      }
      if (hasFocus)
         renderFrame( painter, RECT, Sunken, Tile::Ring, widget, true);
//       else
      {
         QRect rect = RECT.adjusted(2,2,-2,-2); // this is ok, as we set the frame to 4px anyway ;)
         renderFrame( painter, rect, sunken ? Sunken :
                   (option->state & QStyle::State_Raised) ?
                   Raised : Relief, Tile::Ring);
      }
      break;
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
         QRect rect(RECT);
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
            break;
         case QTabBar::RoundedSouth:
         case QTabBar::TriangularSouth:
            rect.adjust(offset,0,-offset,0);
            rect.setTop(rect.bottom()-baseHeight);
            pf = Tile::Left | Tile::Right | Tile::Bottom;
            break;
         case QTabBar::RoundedEast:
         case QTabBar::TriangularEast:
            rect.adjust(0,offset,0,-offset);
            rect.setLeft(rect.right()-baseHeight);
            pf = Tile::Top | Tile::Right | Tile::Bottom;
            o = Qt::Horizontal;
            break;
         case QTabBar::RoundedWest:
         case QTabBar::TriangularWest:
            rect.adjust(0,offset,0,-offset);
            rect.setWidth(baseHeight);
            pf = Tile::Top | Tile::Left | Tile::Bottom;
            o = Qt::Horizontal;
            break;
         }
         shadows.tab.render(rect, painter, pf);
         fillWithMask(painter, rect.adjusted(2,1,-2,0), gradient(hasFocus?COLOR(Highlight):COLOR(WindowText),
            baseHeight, o, GradGloss), &masks.tab, pf | Tile::Center);
         rect = RECT.adjusted(0,baseHeight-1,0,0);
         shadows.tab.render(rect, painter, Tile::Ring);
      }
      break;
   case PE_FrameLineEdit: // Panel frame for line edits.
      shadows.sunken.render(RECT,painter);
      if (hasFocus)
         masks.button.outline(RECT.adjusted(1,1,-1,0), painter, COLOR(Highlight));
   case PE_FrameGroupBox: // Panel frame around group boxes.
   {
      int darkness = 5; // compromise ;) - shouldn't happen anyway
      if (widget)
      {
         // find proper color value for the bottom (because of out general gradient)
         // 0% -> darkness = 0, 100% -> darkness = 10
         // we could also simply create a gradient into translucent - but that can get slow... :(
         QWidget *tlw = widget->topLevelWidget();
         QPoint zero(0,0); zero = widget->mapTo(tlw, zero);
         darkness = (10*(zero.y()+widget->height()))/tlw->height();
//          if (zero.x()+widget->width()-tlw->width()/2 > tlw->width()/2-zero.x())
//          darkness = (darkness*(tlw->width()-zero.x()))/(zero.x()+widget->width());
//          qWarning("%d", darkness);
      }
      QRect rect = RECT.adjusted(dpi.$4,dpi.$2,-dpi.$4,-dpi.$32);
      QColor c = COLOR(Window).dark(100+darkness);
      fillWithMask(painter, rect, gradient(c, rect.height(), Qt::Vertical, GradGroup), &masks.button,
                   Tile::Left|Tile::Top|Tile::Right);
      rect = RECT.adjusted(dpi.$4,0,-dpi.$4,0); rect.setTop(rect.bottom()-dpi.$32+dpi.$1);
      painter->drawTiledPixmap(rect, gradient(c, rect.height(), Qt::Horizontal, GradGroup));
// //       rect = RECT; rect.setBottom(rect.bottom()-rect.height()/6);
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
   case PE_IndicatorToolBarHandle: // The handle of a toolbar.
      if (!(widget && widget->parentWidget()) || widget->parentWidget()->underMouse())
      {
         QPen pen = painter->pen();
         QPen oldPen = pen;
         pen.setColor(hover ? PAL.color(QPalette::Highlight) : midColor(PAL.color(QPalette::Window), PAL.color(QPalette::WindowText)));
         pen.setWidth(2);
         if (pe == PE_IndicatorToolBarHandle)
            pen.setStyle(Qt::DashDotDotLine);
         else
         {
            QVector<qreal> dashes;
            dashes << 1 << 4;
            pen.setDashPattern(dashes);
         }
         painter->setPen(pen);
         if (RECT.width() > RECT.height())
         {
            int y = RECT.y()+(RECT.height()>>1)-1;
            if (pe == PE_IndicatorToolBarHandle)
               painter->drawLine(RECT.x(),y,RECT.right(),y);
            else
               painter->drawLine(RECT.x()+RECT.width()/3,y,RECT.right()-RECT.width()/3,y);
         }
         else
         {
            int x = RECT.x()+(RECT.width()>>1)-1;
            if (pe == PE_IndicatorToolBarHandle)
               painter->drawLine(x,RECT.y(),x,RECT.bottom());
            else
               painter->drawLine(x,RECT.y()+RECT.height()/3,x,RECT.bottom()-RECT.height()/3);
         }
         painter->setPen(oldPen);
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
