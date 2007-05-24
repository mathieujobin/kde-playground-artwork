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

#include <Q3ScrollView>
#include <QAbstractScrollArea>
#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QHeaderView>
#include <QMenuBar>
#include <QStyleOptionTab>
#include <QStyleOptionHeader>
#include <QStyleOptionSlider>
#include <QStyleOptionProgressBarV2>
#include <QStyleOptionToolBox>
#include <QPainter>
#include "oxygen.h"

#include <QtDebug>

using namespace Oxygen;

extern Config config;
extern Dpi dpi;
// extern HoverFades hoverWidgets;q
extern int complexStep;

#include "inlinehelp.cpp"
#include "makros.h"

static const int windowsItemFrame	= 1; // menu item frame width
static const int windowsItemHMargin	= 3; // menu item hor text margin
static const int windowsItemVMargin	= 1; // menu item ver text margin
static const int windowsRightBorder	= 12; // right border on windows

static void drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton,
                      const QRect &rect, QPainter *painter, const QWidget *widget = 0)
{
   QStyle::PrimitiveElement pe;
   switch (toolbutton->arrowType) {
   case Qt::LeftArrow:
      pe = QStyle::PE_IndicatorArrowLeft;
      break;
   case Qt::RightArrow:
      pe = QStyle::PE_IndicatorArrowRight;
      break;
   case Qt::UpArrow:
      pe = QStyle::PE_IndicatorArrowUp;
      break;
   case Qt::DownArrow:
      pe = QStyle::PE_IndicatorArrowDown;
      break;
   default:
      return;
   }
   QStyleOption arrowOpt;
   arrowOpt.rect = rect;
   arrowOpt.palette = toolbutton->palette;
   arrowOpt.state = toolbutton->state;
   style->drawPrimitive(pe, &arrowOpt, painter, widget);
}

bool scrollAreaHovered(const QWidget* slider) {
//    bool scrollerActive = false;
   QWidget *scrollWidget = const_cast<QWidget*>(slider);
   if (!scrollWidget->isEnabled())
      return false;
   while (scrollWidget &&
          !(qobject_cast<QAbstractScrollArea*>(scrollWidget) ||
          qobject_cast<Q3ScrollView*>(scrollWidget)))
      scrollWidget = const_cast<QWidget*>(scrollWidget->parentWidget());
   bool isActive = true;
   if (scrollWidget) {
//       QAbstractScrollArea* scrollWidget = (QAbstractScrollArea*)daddy;
      QPoint tl = scrollWidget->mapToGlobal(QPoint(0,0));
      QRegion scrollArea(tl.x(),tl.y(),scrollWidget->width(),scrollWidget->height());
      QList<QAbstractScrollArea*> scrollChilds = scrollWidget->findChildren<QAbstractScrollArea*>();
      for (int i = 0; i < scrollChilds.size(); ++i) {
         QPoint tl = scrollChilds[i]->mapToGlobal(QPoint(0,0));
         scrollArea -= QRegion(tl.x(), tl.y(), scrollChilds[i]->width(), scrollChilds[i]->height());
      }
      QList<Q3ScrollView*> scrollChilds2 = scrollWidget->findChildren<Q3ScrollView*>();
      for (int i = 0; i < scrollChilds2.size(); ++i) {
         QPoint tl = scrollChilds[i]->mapToGlobal(QPoint(0,0));
         scrollArea -= QRegion(tl.x(), tl.y(), scrollChilds2[i]->width(), scrollChilds2[i]->height());
      }
//       scrollerActive = scrollArea.contains(QCursor::pos());
      isActive = scrollArea.contains(QCursor::pos());
   }
   return isActive;
}

void OxygenStyle::drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget) const
{
   Q_ASSERT(option);
   Q_ASSERT(painter);
   
   bool sunken = option->state & State_Sunken;
   bool isEnabled = option->state & State_Enabled;
   bool hover = isEnabled && (option->state & State_MouseOver);
   bool hasFocus = option->state & State_HasFocus;
   
   switch ( element ) {
   case CE_PushButton: // A QPushButton, draws case CE_PushButtonBevel, case CE_PushButtonLabel and PE_FrameFocusRect
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option)) {
         QStyleOptionButton tmpBtn = *btn;
         if (btn->features & QStyleOptionButton::Flat) { // more like a toolbtn
            //TODO: handle focus indication here (or in the primitive...)!
            drawPrimitive(PE_PanelButtonTool, option, painter, widget);
         }
         else
            drawControl(CE_PushButtonBevel, &tmpBtn, painter, widget);
//          tmpBtn.rect = subElementRect(SE_PushButtonContents, btn, widget);
         tmpBtn.rect = btn->rect.adjusted(dpi.$4,dpi.$2,-dpi.$4,-dpi.$4);
         drawControl(CE_PushButtonLabel, &tmpBtn, painter, widget);
      }
      break;
   case CE_PushButtonBevel: // The bevel and default indicator of a QPushButton.
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option)) {
         drawPrimitive(PE_PanelButtonCommand, option, painter, widget);
         if (btn->features & QStyleOptionButton::HasMenu) {
//             int mbi = pixelMetric(PM_MenuButtonIndicator, btn, widget);
            QStyleOptionButton newBtn = *btn;
            int sz = (RECT.height()-dpi.$6)/2;
            newBtn.rect = QRect(RECT.right() - (dpi.$4+sz), (RECT.height()-sz)/2+dpi.$2, sz, sz);
            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(midColor(COLOR(Window),COLOR(WindowText)));
            drawPrimitive(PE_IndicatorArrowDown, &newBtn, painter, widget);
            painter->restore();
         }
      }
      break;
   case CE_PushButtonLabel: // The label (icon with text or pixmap) of a QPushButton
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option)) {
         QRect ir = btn->rect;
         uint tf = Qt::AlignVCenter | Qt::TextShowMnemonic;
         if (!styleHint(SH_UnderlineShortcut, btn, widget))
            tf |= Qt::TextHideMnemonic;
         
         if (!btn->icon.isNull()) {
            QIcon::Mode mode = isEnabled ? QIcon::Normal
               : QIcon::Disabled;
            if (mode == QIcon::Normal && hasFocus)
               mode = QIcon::Active;
            QIcon::State state = QIcon::Off;
            if (btn->state & State_On)
               state = QIcon::On;
            QPixmap pixmap = btn->icon.pixmap(btn->iconSize, mode, state);
            int pixw = pixmap.width();
            int pixh = pixmap.height();
            
            //Center the icon if there is no text
            QPoint point;
            if (btn->text.isEmpty())
               point = QPoint(ir.x() + ir.width() / 2 - pixw / 2, ir.y() + ir.height() / 2 - pixh / 2);
            else
               point = QPoint(ir.x() + 2, ir.y() + ir.height() / 2 - pixh / 2);
            
            if (btn->direction == Qt::RightToLeft)
               point.rx() += pixw;
            
            painter->drawPixmap(visualPos(btn->direction, btn->rect, point), pixmap);
            
            if (btn->direction == Qt::RightToLeft)
               ir.translate(-4, 0);
            else
               ir.translate(pixw + 4, 0);
            ir.setWidth(ir.width() - (pixw + 4));
                // left-align text if there is
            if (!btn->text.isEmpty())
               tf |= Qt::AlignLeft;
         }
         else
            tf |= Qt::AlignHCenter;
         
         painter->save();
         QColor fc, bc;
         if (btn->features & QStyleOptionButton::Flat) {
            bc = COLOR(Window);
            fc = COLOR(WindowText);
         }
         else {
            bc = btnBgColor(PAL, isEnabled, hover || hasFocus);
            fc = btnFgColor(PAL, isEnabled, hover || hasFocus);
         }
         if (qGray(bc.rgb()) < 148) { // dark background, let's paint an emboss
            painter->setPen(bc.dark(103));
            ir.moveTop(ir.top()-1);
            drawItemText(painter, ir, tf, PAL, isEnabled, btn->text);
            ir.moveTop(ir.top()+1);
         }
         painter->setPen(fc);
         drawItemText(painter, ir, tf, PAL, isEnabled, btn->text);
         painter->restore();
      }
      break;
   case CE_DockWidgetTitle: // Dock window title.
      if (const QStyleOptionDockWidget *dwOpt =
          qstyleoption_cast<const QStyleOptionDockWidget *>(option)) {
         QRect textRect;
         int x3 = RECT.right()-7;
         if (dwOpt->floatable)
            x3 -= 18;
         if (dwOpt->closable)
            x3 -= 18;
         int x2 = x3;
         if (!dwOpt->title.isEmpty()) {
            drawItemText(painter, RECT, Qt::AlignCenter, PAL, isEnabled, dwOpt->title, QPalette::WindowText);
            textRect = painter->boundingRect ( RECT, Qt::AlignCenter, dwOpt->title );
            x2 = textRect.x()-8;
         }

         const Tile::Line &line = shadows.line[0][Sunken];
         textRect.setTop(textRect.top()+(textRect.height()-line.thickness())/2);
         int x = textRect.right()+dpi.$4;
         textRect.setRight(textRect.left()-dpi.$4);
         textRect.setLeft(qMin(RECT.x()+RECT.width()/4,textRect.x()-(textRect.x()-RECT.x())/2));
         line.render(textRect, painter, Tile::Left|Tile::Center);
         textRect.setLeft(x);
         textRect.setRight(qMax(RECT.right()-RECT.width()/4,x+(RECT.right()-x)/2));
         line.render(textRect, painter, Tile::Right|Tile::Center);
         //TODO: hover?
      }
      break;
   case CE_Splitter: // Splitter handle; see also QSplitter.
      drawPrimitive(PE_IndicatorDockWidgetResizeHandle,option,painter,widget);
      break;
   case CE_RadioButton: // A QRadioButton, draws a PE_ExclusiveRadioButton, a case CE_RadioButtonLabel
   case CE_CheckBox: // A QCheckBox, draws a PE_IndicatorCheckBox, a case CE_CheckBoxLabel
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option)) {
         QStyleOptionButton subopt = *btn;
         if (element == CE_RadioButton) {
            subopt.rect = subElementRect(SE_RadioButtonIndicator, btn, widget);
            drawPrimitive(PE_IndicatorRadioButton, &subopt, painter, widget);
            subopt.rect = subElementRect(SE_RadioButtonContents, btn, widget);
            drawControl(CE_RadioButtonLabel, &subopt, painter, widget);
         }
         else {
            subopt.rect = subElementRect(SE_CheckBoxIndicator, btn, widget);
            drawPrimitive(PE_IndicatorCheckBox, &subopt, painter, widget);
            subopt.rect = subElementRect(SE_CheckBoxContents, btn, widget);
            drawControl(CE_CheckBoxLabel, &subopt, painter, widget);
         }
      }
      break;
//    case CE_CheckBoxLabel: // The label (text or pixmap) of a QCheckBox
//    case CE_RadioButtonLabel: // The label (text or pixmap) of a QRadioButton
   case CE_TabBarTab: // The tab and label within a QTabBar
      if (const QStyleOptionTab *tab =
          qstyleoption_cast<const QStyleOptionTab *>(option)) {
         // do we have to exclude the scrollers?
         bool needRestore = false;
         if (widget && (RECT.right() > widget->width())) {
            painter->save();
            needRestore = true;
            QRect r = RECT;
            r.setRight(widget->width() -
                  2*pixelMetric(PM_TabBarScrollButtonWidth,option,widget));
            painter->setClipRect(r);
         }
         // paint shape and label
         drawControl(CE_TabBarTabShape, tab, painter, widget);
         drawControl(CE_TabBarTabLabel, tab, painter, widget);
         if (needRestore)
            painter->restore();
      }
      break;
   case CE_TabBarTabShape: // The tab shape within a tab bar
      if (const QStyleOptionTab *tab =
          qstyleoption_cast<const QStyleOptionTab *>(option)) {
         const bool selected = option->state & State_Selected;
         IndexedFadeInfo *info = 0;
         int index = -1, hoveredIndex = -1, step = 0;

         // fade animation stuff
         if (widget)
         if (const QTabBar* tbar = qobject_cast<const QTabBar*>(widget)) {
            // NOTICE: the index increment is IMPORTANT to make sure it's no "0"
            index = tbar->tabAt(RECT.topLeft()) + 1; // is the action for this item!
            hoveredIndex = hover ? index : tbar->tabAt(tbar->mapFromGlobal(QCursor::pos())) + 1;
            info = const_cast<IndexedFadeInfo *>(indexedFadeInfo(widget, hoveredIndex));
         }
         if (info)
            step = info->step(index);
         
         // maybe we're done here?!
         if (!(step || hover || selected || sunken))
            break;
         
         int size = 0;
         const int $2 = dpi.$2, $4 = dpi.$4;
         Tile::PosFlags pf = Tile::Ring;
         Qt::Orientation o = Qt::Vertical;
         QPoint off;
         QRect rect = RECT, fillRect = RECT;
         int shape = tab->shape;
         if (widget && !(widget->parentWidget() &&
             qobject_cast<QTabWidget*>(widget->parentWidget()))) {
            //NOTICE: this is elegant but NOT invariant against enum changes!!!
            shape%2 ? --shape : ++shape;
         }
         switch ((QTabBar::Shape)shape) {
         case QTabBar::RoundedNorth:
         case QTabBar::TriangularNorth:
            if (selected) {
               pf &= ~Tile::Bottom;
               fillRect.adjust($2, $4, -$2, 0);
               rect.adjust(0, $2, 0, 0);
            }
            else {
               rect = RECT.adjusted($4,$4,-$4,-$4);
               size = RECT.height()-$2; off.setY($2);
            }
            break;
         case QTabBar::RoundedSouth:
         case QTabBar::TriangularSouth:
            if (selected) {
               pf &= ~Tile::Top;
               fillRect.adjust($2, 0, -$2, -dpi.$6);
               rect.adjust(0, 0, 0, -$2);
            }
            else {
               rect = RECT.adjusted($4,$4,-$4,-dpi.$6);
               size = RECT.height()-$4; off.setY(dpi.$3);
            }
            break;
         case QTabBar::RoundedEast:
         case QTabBar::TriangularEast:
            if (selected) {
               pf &= ~Tile::Left;
               fillRect.adjust(0, $2, -dpi.$6, -$4);
               rect.adjust(0, 0, -$4, 0);
            }
            else {
               o = Qt::Horizontal;
               rect = RECT.adjusted($4,$4,-dpi.$6, -$4);
               size = RECT.width()-$4; off.setX(dpi.$3);
            }
            break;
         case QTabBar::RoundedWest:
         case QTabBar::TriangularWest:
            if (selected) {
               pf &= ~Tile::Right;
               fillRect.adjust(dpi.$6, $2, 0, -$4);
               rect.adjust($4, 0, 0, 0);
            }
            else {
               o = Qt::Horizontal;
               rect = RECT.adjusted(dpi.$6,$4,-$4, -$4);
               size = RECT.width()-$4; off.setX(dpi.$3);
            }
            break;
         }
         if (selected) {
            QPoint zero = fillRect.topLeft();
            if (widget)
               zero = widget->mapTo(widget->topLevelWidget(), zero);
            fillWithMask(painter, fillRect, PAL.brush(QPalette::Window),
                         &masks.tab, pf | Tile::Center, false, zero);
            shadows.tab.render(rect, painter, pf);
            break;
         }
         else {
            QColor c = step ? midColor(PAL.color(config.role_tab[0]),
                                       PAL.color(config.role_tab[1]),
                                       6-step, step) :
                  PAL.color(config.role_tab[1]);
            fillWithMask(painter, rect,
                         gradient(c, size, o, sunken ? GradSunken : config.gradientStrong),
                         &masks.button, Tile::Full, false, off);
         }
      }
      break;
   case CE_TabBarTabLabel: // The label within a tab
      if (const QStyleOptionTab *tab =
          qstyleoption_cast<const QStyleOptionTab *>(option)) {
         painter->save();
         QStyleOptionTabV2 tabV2(*tab);
         QRect tr = tabV2.rect; bool verticalTabs = false;
         bool east = false;
         bool selected = tabV2.state & State_Selected;
         if (selected) hover = false;
         int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
         bool bottom = false;
         bool reverse = (option->direction == Qt::RightToLeft);
         
         int shape = tab->shape;
         if (widget && !(widget->parentWidget() &&
             qobject_cast<QTabWidget*>(widget->parentWidget()))) {
            //NOTICE: this is elegant but NOT invariant against enum changes!!!
            shape%2 ? --shape : ++shape;
         }
         switch((QTabBar::Shape)shape) {
         case QTabBar::RoundedNorth:
         case QTabBar::TriangularNorth:
            if (selected) tr.setTop(tr.top()+2);
            break;
         case QTabBar::RoundedSouth:
         case QTabBar::TriangularSouth:
            bottom = true;
            if (selected) tr.setBottom(tr.bottom()-2);
            break;
         case QTabBar::RoundedEast:
         case QTabBar::TriangularEast:
            if (selected) tr.setRight(tr.right()-2);
            verticalTabs = true; east = true;
            break;
         case QTabBar::RoundedWest:
         case QTabBar::TriangularWest:
            if (selected) tr.setLeft(tr.left()+2);
            verticalTabs = true;
            break;
         }
         
         if (verticalTabs) {
            int newX, newY, newRot;
            if (east) {
               newX = tr.width(); newY = tr.y(); newRot = 90;
            }
            else {
               newX = 0; newY = tr.y() + tr.height(); newRot = -90;
            }
            tr.setRect(0, 0, tr.height(), tr.width());
            QMatrix m;
            m.translate(newX, newY); m.rotate(newRot);
            painter->setMatrix(m, true);
         }
         
         if (!tabV2.icon.isNull()) {
            QSize iconSize = tabV2.iconSize;
            if (!iconSize.isValid()) {
               int iconExtent = pixelMetric(PM_SmallIconSize);
               iconSize = QSize(iconExtent, iconExtent);
            }
            QPixmap tabIcon = tabV2.icon.pixmap(iconSize, (isEnabled) ? QIcon::Normal : QIcon::Disabled);
            painter->drawPixmap(tr.left() + 6, tr.center().y() - tabIcon.height() / 2, tabIcon);
            tr.setLeft(tr.left() + iconSize.width() + 4);
         }
         // color adjustment
         QColor cF, cB;

         if (selected) {
            QFont tmpFnt = painter->font();
            tmpFnt.setBold(true);
            painter->setFont(tmpFnt);
            cF = COLOR(WindowText); cB = COLOR(Window);
         }
         else if (hover) {
            cF = PAL.color(config.role_tab[0]);
            cB = PAL.color(config.role_tab[1]);
         }
         else {
            cF = PAL.color(config.role_tab[1]);
            cB = PAL.color(config.role_tab[0]);
         }
         if (verticalTabs) {
            QPixmap pixmap(tr.size());
            pixmap.fill(Qt::transparent);
            QPainter pixPainter(&pixmap);
            if (qGray(cB.rgb()) < 148) { // dark background, let's paint an emboss
               pixPainter.setPen(cB.dark(120));
               drawItemText(&pixPainter, pixmap.rect().translated(0,-1), alignment, PAL, isEnabled, tab->text);
            }
            pixPainter.setPen( cF);
            drawItemText(&pixPainter, pixmap.rect(), alignment, PAL, isEnabled, tab->text);
            QStyleOptionTab::SelectedPosition selPos = east ?
               QStyleOptionTab::PreviousIsSelected :
               QStyleOptionTab::NextIsSelected;
            if (tab->selectedPosition == selPos) {
               painter->drawPixmap(-dpi.$2, 0, tabShadow(tr.height()-dpi.$2, false));
            }
            drawItemPixmap(painter, tr, alignment, pixmap);
         }
         else {
            if (qGray(cB.rgb()) < 148) // dark background, let's paint an emboss
            {
               painter->setPen(cB.dark(120));
               tr.moveTop(tr.top()-1);
               drawItemText(painter, tr, alignment, PAL, isEnabled, tab->text);
               tr.moveTop(tr.top()+1);
            }
            painter->setPen(cF);
            drawItemText(painter, tr, alignment, PAL, isEnabled, tab->text);
            QStyleOptionTab::SelectedPosition selPos = reverse ?
               QStyleOptionTab::NextIsSelected :
               QStyleOptionTab::PreviousIsSelected;
            if (tab->selectedPosition == selPos) {
               if (bottom) tr.adjust(0,dpi.$2,0,0);
               painter->drawPixmap(RECT.x()-dpi.$2, RECT.y(), tabShadow(RECT.height()-dpi.$2, bottom));
            }
         }
         painter->restore();
      }
      break;
      case CE_ProgressBar: // CE_ProgressBarGroove, CE_ProgressBarContents, CE_ProgressBarLabel
      if (const QStyleOptionProgressBar *pb
          = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
         QStyleOptionProgressBarV2 subopt = *pb;
         // groove
         subopt.rect = subElementRect(SE_ProgressBarGroove, pb, widget);
         drawControl(CE_ProgressBarGroove, &subopt, painter, widget);
         // contents
         subopt.rect = subElementRect(SE_ProgressBarContents, pb, widget);
         drawControl(CE_ProgressBarContents, &subopt, painter, widget);
#if 0
         // label?
         if (false && pb->textVisible) {
            subopt.rect = subElementRect(SE_ProgressBarLabel, pb, widget);
            drawControl(CE_ProgressBarLabel, &subopt, painter, widget);
         }
#endif
      }
      break;
   case CE_ProgressBarGroove: // The groove where the progress indicator is drawn in a QProgressBar
      if (const QStyleOptionProgressBarV2 *pb =
            qstyleoption_cast<const QStyleOptionProgressBarV2*>(option)) {
         int size = RECT.height();
         Qt::Orientation o = Qt::Vertical;
         if (pb->orientation == Qt::Vertical) {
            size = RECT.width();
            o = Qt::Horizontal;
         }
         const QPixmap &groove = gradient(PAL.color(config.role_progress[0]), size,
                                          o, config.glassProgress ? GradGlass : GradSunken);
         fillWithMask(painter, RECT.adjusted(0,0,0,-dpi.$2), groove, &masks.button, Tile::Full);
         if (!animationUpdate)
            shadows.lineEdit[isEnabled].render(RECT, painter);
      }
      break;
   case CE_ProgressBarContents: // The progress indicator of a QProgressBar
      if (const QStyleOptionProgressBarV2 *pb =
            qstyleoption_cast<const QStyleOptionProgressBarV2*>(option)) {
         double val = pb->progress; val /= (pb->maximum - pb->minimum);
         if (val == 0.0)
            break;
         QRect r = RECT; int size = r.height();
         Qt::Orientation o = Qt::Vertical;
         bool reverse = option->direction == Qt::RightToLeft;
         if (pb->invertedAppearance)
            reverse = !reverse;
         int step = progressStep(widget);
         if (pb->orientation == Qt::Vertical) {
            size = r.width();
            o = Qt::Horizontal;
            r.setTop(r.bottom() -
                  (int)(val*RECT.height()));
         }
         else if (reverse) {
            r.setLeft(r.right() -
                  (int)(val*RECT.width()));
         }
         else {
            r.setRight(r.left() +
                     (int)(val*RECT.width()));
         }
         const bool unicolor = config.role_progress[0] == config.role_progress[1];
         const QColor c1 = (pb->progress == pb->maximum || unicolor ) ?
               PAL.color(config.role_progress[0]) :
               midColor( PAL.color(config.role_progress[0]),
                         PAL.color(config.role_progress[1]), 10, 10+step/2);
         const QColor c2 = unicolor ?
               PAL.color(config.role_progress[0]).light(85+step) :
               midColor( PAL.color(config.role_progress[0]),
                                     PAL.color(config.role_progress[1]), 1, 2);
         const QPixmap &chunk1 = gradient(c1, size, o, config.gradientStrong);
         const QPixmap &chunk2 = gradient(c2, size, o, config.gradientStrong);
         
         QPixmap pix; QPainter p;
         if (pb->orientation == Qt::Horizontal) {
            pix = QPixmap(2*size, size);
            p.begin(&pix);
            p.fillRect(0,0,size,size, chunk1);
            p.fillRect(size,0,size,size, chunk2);
         }
         else {
            pix = QPixmap(size, 2*size);
            p.begin(&pix);
            p.fillRect(0,0,size,size, chunk1);
            p.fillRect(0,size,size,size, chunk2);
         }
         p.end();
         
         fillWithMask(painter, r, pix, &masks.button/*, Tile::Full, false, off*/);
      }
      break;
   case CE_ProgressBarLabel: // The text label of a QProgressBar
      break;
      if (const QStyleOptionProgressBarV2 *progress =
          qstyleoption_cast<const QStyleOptionProgressBarV2*>(option)) {
         if (!animationUpdate)
            painter->save();
         QFont fnt = painter->font();
         fnt.setBold(true);
         painter->setFont(fnt);
         QRect rect = RECT;
         double val = progress->progress;
         bool reverse = option->direction == Qt::RightToLeft;
         if (progress->invertedAppearance) reverse = !reverse;
         val = val / (progress->maximum - progress->minimum);
         QMatrix m; QPalette::ColorRole role;
         if (progress->orientation == Qt::Vertical) {
            rect.setRect(RECT.x(), RECT.y(), RECT.height(), RECT.width());
            if (progress->bottomToTop) {
               m.translate(0.0, RECT.height()); m.rotate(-90);
            }
            else {
               m.translate(RECT.width(), 0.0); m.rotate(90);
            }
         }
         if ( val > 0.0 ) {
            int s;
            QRect cr;
            if (progress->orientation == Qt::Vertical) {
               s = qMin( RECT.height(), ( int ) (val * RECT.height() ) );
               if ( s > 1 )
                  cr = QRect(RECT.x(), RECT.bottom()-s+1, RECT.width(), s);
            }
            else {
               s = qMin( RECT.width(), ( int ) (val * RECT.width() ) );
               if ( s > 1 ) {
//                   QRect progressRect = RECT;
                  cr = RECT;
                  if (reverse) {
                     cr.setLeft(RECT.right()-s+1);
                     cr.setWidth(s);
//                      int left = progressRect.x()-progressRect.height();
//                      for (int i = 0; i < progressRect.height(); i++)
//                         cr += QRect(left+i,progressRect.y()+i,progressRect.height(),1);
                  }
                  else
//                   {
                     cr.setWidth(s);
/*                     int right = progressRect.x()+progressRect.width();
                     for (int i = 0; i < progressRect.height(); i++)
                        cr += QRect(right-i-1, progressRect.y()+i, progressRect.height(),1);
                  }
                  cr = QRegion(progressRect) - cr;*/
               }
            }
//             painter->setClipRegion(cr);
            painter->setClipRect(cr);
            painter->setMatrix(m);
#if 1
            role = hasFocus?QPalette::HighlightedText:QPalette::Window;
#else
            role = hasFocus?QPalette::HighlightedText:QPalette::WindowText;
#endif
            drawItemText(painter, rect, Qt::AlignCenter | Qt::TextSingleLine, PAL, isEnabled,
                         progress->text, role);
            painter->resetMatrix();
            painter->setClipRegion(QRegion(RECT).subtract(cr));
         }
         if (animationUpdate)
            break;
         painter->setMatrix(m);
#if 1
         role = hasFocus?QPalette::Highlight:QPalette::WindowText;
#else
         role = hasFocus?QPalette::Highlight:QPalette::Window;
#endif
         drawItemText(painter, rect, Qt::AlignCenter | Qt::TextSingleLine, PAL, isEnabled,
                      progress->text, role);
         painter->restore();
      }
      break;
   case CE_ToolButtonLabel: // A tool button's label
      if (const QStyleOptionToolButton *toolbutton
          = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
         // Arrow type always overrules and is always shown
         bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
         if ((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty() ||
             toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) {
            drawItemText(painter, RECT, Qt::AlignCenter | Qt::TextShowMnemonic, PAL, isEnabled, toolbutton->text, QPalette::WindowText);
         }
         else {
            QPixmap pm;
            QSize pmSize = toolbutton->iconSize;
            if (!toolbutton->icon.isNull()) {
               QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
               QIcon::Mode mode;
               if (!isEnabled)
                  mode = QIcon::Disabled;
               else if (hover && (option->state & State_AutoRaise))
                  mode = QIcon::Active;
               else
                  mode = QIcon::Normal;
               pm = toolbutton->icon.pixmap(RECT.size().boundedTo(toolbutton->iconSize), mode, state);
               pmSize = pm.size();
            }
                    
            if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly) {
               painter->setFont(toolbutton->font);
               QRect pr = RECT, tr = RECT;
               int alignment = Qt::TextShowMnemonic;
               
               if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                  int $3 = dpi.$3;
                  int fh = painter->fontMetrics().height();
                  pr.adjust(0, $3, 0, -fh - $3);
                  tr.adjust(0, pr.bottom(), 0, -$3);
                  if (!hasArrow)
                     drawItemPixmap(painter, pr, Qt::AlignCenter, pm);
                  else
                     drawArrow(this, toolbutton, pr, painter, widget);
                  alignment |= Qt::AlignCenter;
               }
               else {
                  pr.setWidth(pmSize.width() + dpi.$8);
                  tr.adjust(pr.right(), 0, 0, 0);
                  if (!hasArrow)
                     drawItemPixmap(painter, pr, Qt::AlignCenter, pm);
                  else
                     drawArrow(this, toolbutton, pr, painter, widget);
                  alignment |= Qt::AlignLeft | Qt::AlignVCenter;
               }
               drawItemText(painter, tr, alignment, PAL, isEnabled, toolbutton->text, QPalette::WindowText);
            }
            else {
               if (hasArrow)
                  drawArrow(this, toolbutton, RECT.adjusted(dpi.$5,dpi.$5,-dpi.$5,-dpi.$5), painter, widget);
               else
                  drawItemPixmap(painter, RECT, Qt::AlignCenter, pm);
            }
         }
      }
      break;
   case CE_MenuBarItem: // A menu item in a QMenuBar
      if (const QStyleOptionMenuItem *mbi =
          qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
         QPalette::ColorRole cr = QPalette::WindowText;
         IndexedFadeInfo *info = 0;
         QAction *action = 0, *activeAction = 0;
         int step = 0;
         if (widget)
         if (const QMenuBar* mbar = qobject_cast<const QMenuBar*>(widget)) {
            action = mbar->actionAt(RECT.topLeft()); // is the action for this item!
            activeAction = mbar->activeAction();
            info = const_cast<IndexedFadeInfo *>(indexedFadeInfo(widget, (long int)activeAction));
         }
         if (info &&
             (!activeAction || !activeAction->menu() || activeAction->menu()->isHidden()))
            step = info->step((long int)action);
         if (step || option->state & State_Selected) {
            QRect r = RECT;
            if (step) {
               step = 6-step;
               int dx = step*r.width()/18, dy = step*r.height()/18;
               r.adjust(dx, dy, -dx, -dy);
            }
            QRect rect = r;
            if (sunken)
               rect.adjust(dpi.$2, dpi.$1, -dpi.$2, -dpi.$2);
            shadows.button[sunken][true].render(rect, painter);
            rect = r.adjusted(dpi.$3, dpi.$2, -dpi.$3, -dpi.$3);
            const QPixmap &fill = gradient(CONF_COLOR(role_popup[0]), rect.height(), Qt::Vertical, config.gradient);
            fillWithMask(painter, rect, fill, &masks.button);
            masks.button.outline(rect, painter, Qt::white, true);
            cr = config.role_popup[1];
         }
         QPixmap pix = mbi->icon.pixmap(pixelMetric(PM_SmallIconSize), isEnabled ? QIcon::Normal : QIcon::Disabled);
         uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
         if (!pix.isNull())
            drawItemPixmap(painter,mbi->rect, alignment, pix);
         else
            drawItemText(painter, mbi->rect, alignment, mbi->palette, isEnabled, mbi->text, cr);
      }
      break;
   case CE_MenuBarEmptyArea: // The empty area of a QMenuBar
      break;
   case CE_MenuItem: // A menu item in a QMenu
        // Draws one item in a popup menu.
      if (const QStyleOptionMenuItem *menuItem =
          qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
         bool isCombo = widget && widget->inherits("QComboBox");
         bool selected = menuItem->state & State_Selected;
         QPalette::ColorRole fgr = QPalette::WindowText;
         QPalette::ColorRole bgr = QPalette::Window;
         if (isCombo) {
         //combos send the combo, not the listview as widget, thus we must force this here
            bgr = config.role_popup[0];
            fgr = config.role_popup[1];
         }
         else if (widget) {
            bgr = widget->backgroundRole();
            fgr = widget->foregroundRole();
         }
         
         if (selected && isEnabled) {
            QPalette::ColorRole hlp = bgr; bgr = fgr; fgr = hlp;
         }

         QColor bg = PAL.color(bgr);
         QColor fg = isEnabled ? PAL.color(fgr) : midColor(PAL.color(bgr),PAL.color(fgr),2,1);

         if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
            int dx = RECT.width()/6, dy = (RECT.height()-shadows.line[0][Sunken].thickness())/2;
            shadows.line[0][Sunken].render(RECT.adjusted(dx,dy,-dx,-dy), painter);
            if (!menuItem->text.isEmpty()) {
               painter->setFont(menuItem->font);
               drawItemText(painter, RECT, Qt::AlignCenter, PAL, isEnabled, menuItem->text, fgr);
            }
            break;
         }
         painter->save();
         bool checkable = !isCombo && 
                (menuItem->checkType != QStyleOptionMenuItem::NotCheckable);
         bool checked = checkable && menuItem->checked;
         
         if (selected && isEnabled)
            fillWithMask(painter, RECT,
                         gradient(bg, RECT.height(), Qt::Vertical, config.gradient),
                         &masks.button, Tile::Full);

         // Text and icon, ripped from windows style
         const QStyleOptionMenuItem *menuitem = menuItem;
         int iconCol = config.showMenuIcons*menuitem->maxIconWidth;
         
         if (config.showMenuIcons && !menuItem->icon.isNull()) {
            QRect vCheckRect = visualRect(option->direction, RECT,
                                       QRect(RECT.x(), RECT.y(), iconCol, RECT.height()));
            QIcon::Mode mode = isEnabled ? (selected ? QIcon::Active : QIcon::Normal) : QIcon::Disabled;
            QPixmap pixmap = menuItem->icon.pixmap(pixelMetric(PM_SmallIconSize), mode, checked ? QIcon::On : QIcon::Off);
            
            QRect pmr(QPoint(0, 0), pixmap.size());
            pmr.moveCenter(vCheckRect.center());
            
            painter->drawPixmap(pmr.topLeft(), pixmap);
         }
             
         painter->setPen ( fg );
         painter->setBrush ( Qt::NoBrush );
         
         int x, y, w, h;
         RECT.getRect(&x, &y, &w, &h);
         int tab = menuitem->tabWidth;
         int cDim = 2*(RECT.height() - dpi.$4)/3;
         int xm = windowsItemFrame + iconCol + windowsItemHMargin;
         int xpos = RECT.x() + xm;
         QRect textRect(xpos,
                        y + windowsItemVMargin,
                        w - xm - menuItem->menuHasCheckableItems*(cDim+dpi.$7) - windowsRightBorder - tab + 1,
                        h - 2 * windowsItemVMargin);
         QRect vTextRect = visualRect(option->direction, RECT, textRect);
         QString s = menuitem->text;
         if (!s.isEmpty()) {
            // draw text
            int t = s.indexOf('\t');
            const int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
            if (t >= 0) {
               QRect vShortcutRect = visualRect(option->direction, RECT,
                     QRect(textRect.topRight(), QPoint(textRect.right()+tab, textRect.bottom())));
               painter->drawText(vShortcutRect, text_flags | Qt::AlignLeft, s.mid(t + 1));
               s = s.left(t);
            }
            if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem) {
               QFont font = menuitem->font;
               font.setBold(true);
               painter->setFont(font);
            }
            painter->drawText(vTextRect, text_flags | Qt::AlignLeft, s.left(t));
         }
         // Arrow
         if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {
            // draw sub menu arrow
            PrimitiveElement arrow =
               (option->direction == Qt::RightToLeft) ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
            
            int dim = (RECT.height() - dpi.$4)/2;
            xpos = RECT.x() + RECT.width() - dpi.$7 - dim;
            
            QStyleOptionMenuItem tmpOpt = *menuItem;
            tmpOpt.rect = visualRect(option->direction, RECT, QRect(xpos, RECT.y() + (RECT.height() - dim)/2, dim, dim));
            
            drawPrimitive(arrow, &tmpOpt, painter, widget);
         }
         else if (!isCombo && checkable) { // Checkmark
            xpos = RECT.right() - dpi.$7 - cDim;
            QRect checkRect(xpos, RECT.y() + (RECT.height() - cDim)/2, cDim, cDim);
            checkRect = visualRect(menuItem->direction, menuItem->rect, checkRect);
            if (menuItem->checkType & QStyleOptionMenuItem::Exclusive) {
               // Radio button
               painter->setRenderHint ( QPainter::Antialiasing );
               painter->drawEllipse ( checkRect );
               if (checked || sunken) {
                  painter->setBrush ( fg );
                  painter->drawEllipse ( checkRect.adjusted(checkRect.width()/4, checkRect.height()/4, -checkRect.width()/4, -checkRect.height()/4) );
                  painter->setBrush ( Qt::NoBrush );
               }
            }
            else {
               // Check box
               painter->setBrush ( Qt::NoBrush );
               QStyleOptionMenuItem tmpOpt = *menuItem;
               tmpOpt.rect = checkRect;
               if (checked) tmpOpt.state |= State_On; else tmpOpt.state &= ~State_On;
               drawPrimitive(PE_IndicatorMenuCheckMark, &tmpOpt, painter, widget);
            }
         }
         painter->restore();
      }
      break;
   case CE_MenuScroller: { // Scrolling areas in a QMenu when the style supports scrolling
      QPalette::ColorRole bg = widget ? widget->backgroundRole() : QPalette::Window;
      if (option->state & State_DownArrow) {
         painter->drawTiledPixmap(RECT, gradient(PAL.color(QPalette::Active, bg),
            RECT.height()*2, Qt::Vertical, sunken ? GradSunken : GradSimple), QPoint(0,RECT.height()));
         drawPrimitive(PE_IndicatorArrowDown, option, painter, widget);
      }
      else {
         painter->drawTiledPixmap(RECT, gradient(PAL.color(QPalette::Active, bg),
            RECT.height()*2, Qt::Vertical, sunken ? GradSunken : GradSimple));
         drawPrimitive(PE_IndicatorArrowUp, option, painter, widget);
      }
      break;
   }
//    case CE_MenuTearoff: // A menu item representing the tear off section of a QMenu
   case CE_MenuEmptyArea: // The area in a menu without menu items
   case CE_MenuHMargin: // The horizontal extra space on the left/right of a menu
   case CE_MenuVMargin: { // The vertical extra space on the top/bottom of a menu
//       QPalette::ColorRole role = QPalette::Window;
//       if (widget)
//          role = qobject_cast<const QComboBox*>(widget) ?
//          QPalette::WindowText : widget->backgroundRole();
//       painter->fillRect(RECT, PAL.brush(role));
      break;
   }
   case CE_Q3DockWindowEmptyArea: // The empty area of a QDockWidget
      break;
   case CE_ToolBoxTab: // The toolbox's tab area
      if (const QStyleOptionToolBox* tbt =
          qstyleoption_cast<const QStyleOptionToolBox*>(option)) {
         QPalette::ColorRole role = QPalette::WindowText, bgRole = QPalette::Window;
         if (isEnabled) {
            if (option->state & State_Selected) {
               painter->drawTiledPixmap(RECT, gradient(COLOR(WindowText), RECT.height(), Qt::Vertical, config.gradientStrong));
               QFont f(painter->font());
               f.setBold(true);
               painter->setFont(f);
               bgRole = QPalette::WindowText;
               role = QPalette::Window;
            }
            else
               painter->drawTiledPixmap(RECT, gradient(COLOR(Window), RECT.height(), Qt::Vertical, sunken ? GradSunken : hover?config.gradientStrong:config.gradient));
         }
         /**i WANT (read this TrottelTech: WANT!) this to be color swapped on select (and centered as sugar on top)
         now as the toolboxbutton is a private class and it's selected member is as well, i cannot overwrite the paint event
         so instead i repeat a null rect for selected tabs contents (from subElementRect query), what makes the widget abort the content painting
         then i paint it instead... works ;)  */
         QPixmap pm = tbt->icon.pixmap(pixelMetric(QStyle::PM_SmallIconSize), isEnabled ? QIcon::Normal : QIcon::Disabled);
         
         QRect cr = option->rect.adjusted(0, 0, -30, 0);
         QRect tr, ir;
         int ih = 0;
         if (pm.isNull()) {
            tr = cr;
            tr.adjust(4, 0, -8, 0);
         }
         else {
            int iw = pm.width() + 4;
            ih = pm.height();
            ir = QRect(cr.left() + 4, cr.top(), iw + 2, ih);
            tr = QRect(ir.right(), cr.top(), cr.width() - ir.right() - 4, cr.height());
         }
         
         QString txt = tbt->text;
         if (painter->fontMetrics().width(txt) > tr.width()) {
            txt = txt.left(1);
            int ew = painter->fontMetrics().width("...");
            int i = 1;
            while (painter->fontMetrics().width(txt) + ew + painter->fontMetrics().width(tbt->text[i])  < tr.width())
               txt += tbt->text[i++];
            txt += "...";
         }
         
         if (ih)
            painter->drawPixmap(ir.left(), (RECT.height() - ih) / 2, pm);

             if (qGray(PAL.color(bgRole).rgb()) < 128) { // dark background, let's paint an emboss
            painter->save();
            painter->setPen(PAL.color(bgRole).dark(120));
            tr.moveTop(tr.top()-1);
            drawItemText(painter, tr, Qt::AlignCenter | Qt::TextShowMnemonic, PAL, isEnabled, txt);
            tr.moveTop(tr.top()+1);
            painter->restore();
         }

         drawItemText(painter, tr, Qt::AlignCenter | Qt::TextShowMnemonic, PAL, isEnabled, txt, role);
      }
      break;
//    case CE_SizeGrip: // Window resize handle; see also QSizeGrip.
   case CE_Header: // A header
   if (const QStyleOptionHeader *header =
       qstyleoption_cast<const QStyleOptionHeader *>(option)) {
      // init
      const QRegion clipRegion = painter->clipRegion();
      painter->setClipRect(option->rect);
      QStyleOptionHeader subopt = *header;
      const QHeaderView* hdv = qobject_cast<const QHeaderView*>(widget);
      // extend the sunken state on sorting headers
      sunken = sunken ||
             (hdv && hdv->isClickable () && hdv->sortIndicatorSection() == header->section);
      if (sunken)
         subopt.state |= State_Sunken;
      
      // base
      drawControl(CE_HeaderSection, &subopt, painter, widget);
          
      // label
      subopt.rect = subElementRect(SE_HeaderLabel, header, widget);
      if (subopt.rect.isValid())
         drawControl(CE_HeaderLabel, &subopt, painter, widget);
          
      // sort Indicator on sorting or (inverted) on hovered headers
      if (hover && hdv && hdv->isSortIndicatorShown()) {
         if (subopt.sortIndicator == QStyleOptionHeader::SortDown)
            subopt.sortIndicator = QStyleOptionHeader::SortUp;
          else
            subopt.sortIndicator = QStyleOptionHeader::SortDown;
      }
      if (subopt.sortIndicator != QStyleOptionHeader::None) {
         subopt.rect = subElementRect(SE_HeaderArrow, option, widget);
         painter->save();
         if (hover)
            painter->setPen(COLOR(Text));
         else
            painter->setPen(Qt::NoPen);
         const QPixmap &fill = gradient(hover?COLOR(Base):midColor(COLOR(Text),COLOR(Base)), RECT.height(), Qt::Vertical, sunken?GradSunken:config.gradient);
         painter->setBrush(fill);
         drawPrimitive(PE_IndicatorHeaderArrow, &subopt, painter, widget);
         painter->restore();
      }
      painter->setClipRegion(clipRegion);
      break;
   }
   case CE_HeaderSection: { // A header section
      if (sunken)
         painter->drawTiledPixmap(RECT, gradient(COLOR(Text), RECT.height(), Qt::Vertical, GradSunken));
      else {
         QRect r = RECT; r.setWidth(RECT.width()-1);
         painter->drawTiledPixmap(r, gradient(COLOR(Text), RECT.height(), Qt::Vertical, hover?config.gradientStrong:config.gradient));
         r = RECT; r.setLeft(r.right()-dpi.$1);
         painter->drawTiledPixmap(r, gradient(COLOR(Text), RECT.height(), Qt::Vertical, GradSunken));
      }
      break;
   }
   case CE_HeaderLabel: { // The header's label
      const QStyleOptionHeader* hopt = qstyleoption_cast<const QStyleOptionHeader*>(option);
      QRect rect = option->rect;
      if ( !hopt->icon.isNull() ) {
         QPixmap pixmap = hopt->icon.pixmap( 22,22, isEnabled ? QIcon::Normal : QIcon::Disabled );
         int pixw = pixmap.width();
         int pixh = pixmap.height();
         
         QRect pixRect = option->rect;
         pixRect.setY( option->rect.center().y() - (pixh - 1) / 2 );
         // "pixh - 1" because of tricky integer division
         drawItemPixmap ( painter, pixRect, Qt::AlignCenter, pixmap );
         rect.setLeft( rect.left() + pixw + 2 );
      }
      if (hopt->text.isEmpty())
         break;
      painter->save();
      QColor bg = COLOR(Text), fg = COLOR(Base);
      if (qGray(bg.rgb()) < 148) { // dark background, let's paint an emboss
         rect.moveTop(rect.top()-1);
         painter->setPen(bg.dark(120));
         drawItemText ( painter, rect, Qt::AlignCenter, PAL, isEnabled, hopt->text);
         rect.moveTop(rect.top()+1);
      }
      painter->setPen(fg);
      drawItemText ( painter, rect, Qt::AlignCenter, PAL, isEnabled, hopt->text);
      painter->restore();
      break;
   }
   case CE_ScrollBarAddLine: // Scroll bar line increase indicator. (i.e., scroll down); see also QScrollBar.
      if (option->state & State_Item) { // combobox scroller
         painter->save();
         painter->setPen(hover?COLOR(Text):midColor(COLOR(Base),COLOR(Text)));
         QStyleOption opt = *option;
         opt.rect = RECT.adjusted(RECT.width()/4,RECT.height()/4,-RECT.width()/4,-RECT.height()/4);
         if (option->state & QStyle::State_Horizontal)
            drawPrimitive (PE_IndicatorArrowRight, &opt, painter, widget);
         else
            drawPrimitive (PE_IndicatorArrowDown, &opt, painter, widget);
         painter->restore();
         break;
      }
   case CE_ScrollBarSubLine: // Scroll bar line decrease indicator (i.e., scroll up).
      if (option->state & State_Item) { // combobox scroller
         painter->save();
         painter->setPen(hover?COLOR(Text):midColor(COLOR(Base),COLOR(Text)));
         QStyleOption opt = *option;
         opt.rect = RECT.adjusted(RECT.width()/4,RECT.height()/4,-RECT.width()/4,-RECT.height()/4);
         if (option->state & QStyle::State_Horizontal)
            drawPrimitive (PE_IndicatorArrowLeft, &opt, painter, widget);
         else
            drawPrimitive (PE_IndicatorArrowUp, &opt, painter, widget);
         painter->restore();
         break;
      }
      if (const QStyleOptionSlider *opt =
            qstyleoption_cast<const QStyleOptionSlider *>(option)) {
         bool alive = isEnabled && ((element == CE_ScrollBarAddLine &&
                                     opt->sliderValue < opt->maximum) ||
                                    (element == CE_ScrollBarSubLine &&
                                     opt->sliderValue > opt->minimum));
         QPoint xy = RECT.topLeft();
         if (sunken || !alive)
            painter->drawPixmap(xy+QPoint(dpi.$1,dpi.$1), shadows.radio[1][hover]);
         else
            painter->drawPixmap(xy, shadows.radio[0][hover]);
         xy += QPoint(dpi.$2,dpi.$1);
         int sz = dpi.ExclusiveIndicator - dpi.$4;
         fillWithMask(painter, xy, gradient(btnBgColor(PAL, alive, hover, complexStep),
                                            sz, Qt::Vertical,
                                            alive?config.gradient:GradSunken), masks.radio);
         break;
      }
   case CE_ScrollBarSubPage: // Scroll bar page decrease indicator (i.e., page up).
   case CE_ScrollBarAddPage: {// Scolllbar page increase indicator (i.e., page down).
      if (option->state & State_Item) // combobox scroller
         break;
      Qt::Orientation direction; int size, c; QRect r;
      if (option->state & QStyle::State_Horizontal) {
         r = RECT.adjusted(0,RECT.height()/3,0,-RECT.height()/3);
         size = r.height(); direction = Qt::Vertical;
         c = r.y()+r.height()/2;
      }
      else {
         r = RECT.adjusted(RECT.width()/3,0,-RECT.width()/3,0);
         size = r.width(); direction = Qt::Horizontal;
         c = r.x()+r.width()/2;
      }
      painter->drawTiledPixmap(r, gradient(COLOR(Window), size, direction, GradSunken));
      break;
   }
   case CE_ScrollBarSlider: // Scroll bar slider.
      if (option->state & State_Item) {
         painter->fillRect(RECT.adjusted(dpi.$2,0,-dpi.$2,0), (hover||sunken)?COLOR(Text):midColor(COLOR(Base),COLOR(Text),8,1));
         break;
      }
      if (/*const QStyleOptionSlider *opt =*/
          qstyleoption_cast<const QStyleOptionSlider *>(option)) {
         Qt::Orientation direction; int size; QRect r;
   
         // the groove (add or sub page or if min == max, i.e. no slide usefull)
         if (!isEnabled/*(opt->minimum < opt->maximum)*/) {
            if (option->state & QStyle::State_Horizontal) {
               r = RECT.adjusted(0,RECT.height()/3,0,-RECT.height()/3);
               size = r.height(); direction = Qt::Vertical;
            }
            else {
               r = RECT.adjusted(RECT.width()/3,0,-RECT.width()/3,0);
               size = r.width(); direction = Qt::Horizontal;
            }
            fillWithMask(painter, r, gradient(COLOR(Window), size, direction, GradSunken), &masks.button);
            break;
         }
         
         // we need to paint a slider
         hover = hover || sunken;
         r = RECT;
         if (sunken) r.adjust(dpi.$1,dpi.$1,-dpi.$1,-dpi.$2);
         shadows.button[sunken][hover].render(r, painter);
         r = RECT.adjusted(dpi.$2,dpi.$1,-dpi.$2,-dpi.$3);
         if (option->state & QStyle::State_Horizontal) {
            size = r.height(); direction = Qt::Vertical;
         }
         else {
            size = r.width(); direction = Qt::Horizontal;
         }

         // slider base
         fillWithMask(painter, r, gradient(btnBgColor(PAL, isEnabled, hover, complexStep),
                                           size, direction, config.gradBtn), &masks.button);
	 masks.button.outline(r, painter, Qt::white, true);
         
         // and maybe a "scrollarea hovered indicator"
         if (hover && !complexStep)
            break;
         int step = hoverStep(widget);
         if (step || scrollAreaHovered(widget)) {
            int dx, dy, off = sunken?dpi.$1:0;
            if (option->state & QStyle::State_Horizontal) {
               dx = r.width()/10+dpi.$1; dy = r.height()/3;
               r.adjust(dx,dy+off,-dx,-dy+off);
               size = r.height();
            }
            else {
               dx = r.width()/3; dy = r.height()/10+dpi.$1;
               r.adjust(dx+off,dy,-dx+off,-dy);
               size = r.width();
            }
            painter->drawTiledPixmap(r, gradient(btnBgColor(PAL, isEnabled, true, step),
                                                 size, direction, config.gradBtn));
         }
      }
      break;
//    case CE_ScrollBarFirst: // Scroll bar first line indicator (i.e., home).
//    case CE_ScrollBarLast: // Scroll bar last line indicator (i.e., end).
   case CE_RubberBand: // Rubber band used in such things as iconview.
   {
      painter->save();
      QColor c = COLOR(Highlight);
      painter->setPen(c);
      c.setAlpha(100);
      painter->setBrush(c);
      painter->drawRect(RECT);
      painter->restore();
      break;
   }
   case CE_FocusFrame: // Focus Frame that can is style controled.
      break;
   case CE_ComboBoxLabel: // The label of a non-editable QComboBox
      if (const QStyleOptionComboBox *cb =
          qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
         QRect editRect = subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, widget);
         painter->save();
         painter->setClipRect(editRect);
         // icon
         if (!cb->currentIcon.isNull()) {
            QIcon::Mode mode = isEnabled ? QIcon::Normal
               : QIcon::Disabled;
            QPixmap pixmap = cb->currentIcon.pixmap(cb->iconSize, mode);
            QRect iconRect(editRect);
            iconRect.setWidth(cb->iconSize.width() + 4);
            iconRect = alignedRect(QApplication::layoutDirection(), Qt::AlignLeft | Qt::AlignVCenter, iconRect.size(), editRect);
/*            if (cb->editable)
               painter->fillRect(iconRect, opt->palette.brush(QPalette::Base));*/
            drawItemPixmap(painter, iconRect, Qt::AlignCenter, pixmap);
            
            if (cb->direction == Qt::RightToLeft)
               editRect.translate(-4 - cb->iconSize.width(), 0);
            else
               editRect.translate(cb->iconSize.width() + 4, 0);
         }
         // text
         if (!cb->currentText.isEmpty() && !cb->editable) {
            editRect.adjust(3,0, -3, 0);
            const QComboBox* combo = widget ?
               qobject_cast<const QComboBox*>(widget) : 0;
            if (combo && combo->view() && ((QWidget*)(combo->view()))->isVisible())
               painter->setPen(config.role_popup[0] != QPalette::Window ?
                               CONF_COLOR(role_popup[1]) : COLOR(Text));
            else if (hover)
               painter->setPen(COLOR(Text));
            else
               painter->setPen(midColor(COLOR(Base), COLOR(Text), isEnabled?1:2, 2));
            painter->drawText(editRect, Qt::AlignCenter, cb->currentText);
         }
         painter->restore();
      }
      break;
   case CE_ToolBar:
      break;
   default:
         QCommonStyle::drawControl( element, option, painter, widget );
   } // switch
}
