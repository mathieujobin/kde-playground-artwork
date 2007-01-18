#include <Q3ScrollView>
#include <QAbstractScrollArea>
#include <QApplication>
#include <QHeaderView>
#include <QStyleOptionTab>
#include <QStyleOptionHeader>
#include <QStyleOptionSlider>
#include <QStyleOptionProgressBarV2>
#include <QStyleOptionToolBox>
#include <QPainter>
#include "oxygen.h"

#include "inlinehelp.cpp"

using namespace Oxygen;

extern Config config;

/**gradients or glosses*/
inline const QPixmap OxygenStyle::colorRun(const QColor &c, const int size, Qt::Orientation o, Orientation3D o3D, bool smooth) const
{
   if (smooth)
      return gradient(c, size, o, o3D);
   return gloss(c, size, o, o3D);
}

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

void OxygenStyle::drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget) const
{
   Q_ASSERT(option);
   Q_ASSERT(painter);
#define RECT option->rect
#define PAL option->palette
   
   bool sunken = option->state & State_Sunken;
   bool isEnabled = option->state & State_Enabled;
   bool hover = isEnabled && (option->state & State_MouseOver);
   bool hasFocus = option->state & State_HasFocus;
   
   switch ( element )
   {
   case CE_PushButton: // A QPushButton, draws case CE_PushButtonBevel, case CE_PushButtonLabel and PE_FrameFocusRect
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option))
      {
         QStyleOptionButton tmpBtn = *btn;
         if (btn->features & QStyleOptionButton::Flat) // more like a toolbtn
         {
            if (sunken || (option->state & QStyle::State_On))
            {
               QRegion innerRegion = round_frames[Sunken].innerRegion(RECT);
               painter->save();
               painter->setClipRegion(innerRegion);
               QRect ir = innerRegion.boundingRect();
               if (sunken) hover = false;
               painter->drawTiledPixmap(ir, gradient(PAL.color(QPalette::Window),ir.height(),Qt::Vertical, hover ? Raised : Sunken));
               painter->restore();
               renderFrame( painter, RECT, Sunken, Full, widget, false, true );
               if (hasFocus)
                  renderFrame( painter, RECT, Relief, Full, widget, true, true );
            }
            else
            {
               if (hover)
               {
                  QRegion innerRegion = round_frames[Relief].innerRegion(RECT);
                  painter->save();
                  painter->setClipRegion(innerRegion);
                  QRect ir = innerRegion.boundingRect();
                  painter->drawTiledPixmap(ir, gradient(PAL.color(QPalette::Window),ir.height(),Qt::Vertical, Raised ));
                  painter->restore();
               }
               renderFrame( painter, RECT, Relief, Full, widget, false, true );
               if (hasFocus)
                  renderFrame( painter, RECT, Relief, Full, widget, true, true );
            }
         }
         else
         {
            if (!config.roundButtons)
               tmpBtn.rect = btn->rect.adjusted(2,2,-2,-2);
            if (hasFocus)
               renderFrame( painter, RECT, Sunken, Full, widget, true, config.roundButtons);
            drawControl(CE_PushButtonBevel, &tmpBtn, painter, widget);
         }
//          tmpBtn.rect = subElementRect(SE_PushButtonContents, btn, widget);
         drawControl(CE_PushButtonLabel, &tmpBtn, painter, widget);
      }
      break;
   case CE_PushButtonBevel: // The bevel and default indicator of a QPushButton.
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option))
      {
         drawPrimitive(PE_PanelButtonCommand, option, painter, widget);
         if (btn->features & QStyleOptionButton::HasMenu)
         {
            int mbi = pixelMetric(PM_MenuButtonIndicator, btn, widget);
            QRect ir = btn->rect;
            QStyleOptionButton newBtn = *btn;
            newBtn.rect = QRect(ir.right() - mbi, ir.height() - 20, mbi, ir.height() - 4);
            drawPrimitive(PE_IndicatorArrowDown, &newBtn, painter, widget);
         }
      }
      break;
   case CE_PushButtonLabel: // The label (icon with text or pixmap) of a QPushButton
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option)) 
      {
         QRect ir = btn->rect;
         uint tf = Qt::AlignVCenter | Qt::TextShowMnemonic;
         if (!styleHint(SH_UnderlineShortcut, btn, widget))
            tf |= Qt::TextHideMnemonic;
         
         if (!btn->icon.isNull()) 
         {
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
         
         bool isDefault = btn->features & QStyleOptionButton::DefaultButton;
         painter->save();
         QColor fc;
         if (btn->features & QStyleOptionButton::Flat)
            fc = PAL.color(QPalette::WindowText);
         else
         {
            QColor bc = btnBgColor(PAL, isEnabled, hasFocus, hover);
            fc = btnFgColor(PAL, isEnabled, hasFocus, hover);

            if (qGray(bc.rgb()) < 170) // dark background, let's paint an emboss
            {
               painter->setPen(bc.dark(120));
               ir.moveTop(ir.top()-1);
               drawItemText(painter, ir, tf, PAL, isEnabled, btn->text);
               ir.moveTop(ir.top()+1);
            }
         }
         if (isDefault)
         {
            QFont tmpFnt = painter->font(); tmpFnt.setBold(true);
            painter->setFont(tmpFnt);
         }
         painter->setPen(fc);
         drawItemText(painter, ir, tf, PAL, isEnabled, btn->text);
         painter->restore();
      }
      break;
   case CE_DockWidgetTitle: // Dock window title.
      if (const QStyleOptionDockWidget *dwOpt =
          qstyleoption_cast<const QStyleOptionDockWidget *>(option))
      {
         QRect textRect;
         int x3 = RECT.right()-7;
         if (dwOpt->floatable)
            x3 -= 18;
         if (dwOpt->closable)
            x3 -= 18;
         int x2 = x3;
         if (!dwOpt->title.isEmpty())
         {
            drawItemText(painter, RECT, Qt::AlignCenter, PAL, isEnabled, dwOpt->title, QPalette::WindowText);
            textRect = painter->boundingRect ( RECT, Qt::AlignCenter, dwOpt->title );
            x2 = textRect.x()-8;
         }

         if (hover)
         {
            QPen pen = painter->pen();
            QPen oldPen = pen;
            if (dwOpt->movable)
               pen.setStyle(Qt::DotLine);
            else if (dwOpt->floatable)
               pen.setStyle(Qt::DashLine);
            else
               break;
            pen.setColor(midColor(PAL.color(QPalette::Window), PAL.color(QPalette::WindowText),2));
            pen.setWidth(2);
            painter->setPen(pen);
            int y = RECT.y() + RECT.height()/2 - 2;
            painter->drawLine( RECT.x()+4, y, x2, y ); y += 4;
            painter->drawLine( RECT.x()+4, y, x2, y );
            if (textRect.isValid())
            {
               painter->drawLine( textRect.right()+9, y, x3, y ); y -= 4;
               painter->drawLine( textRect.right()+9, y, x3, y );
            }
            painter->setPen(oldPen);
         }
      }
      break;
   case CE_Splitter: // Splitter handle; see also QSplitter.
   {
      QPen pen = painter->pen();
      QPen oldPen = pen;
      pen.setStyle(Qt::DotLine); //TODO use common shape here
      pen.setWidth(2);
      if (hover)
         pen.setColor(PAL.color(QPalette::Highlight));
      else
         pen.setColor(midColor(PAL.color(QPalette::Window),PAL.color(QPalette::WindowText)));
      
      if (RECT.width() > RECT.height())
      {
         int y = RECT.y()+(RECT.height()>>1)-1;
         painter->drawLine(RECT.x()+RECT.width()/3, y, RECT.right()-RECT.width()/3, y);
      }
      else
      {
         int y = (RECT.y()+RECT.height()/5)<<1; int x = RECT.x()+(RECT.width()>>1)-1;
         painter->drawLine(x, y, x, y+RECT.height()/5);
      }
      painter->setPen(oldPen);
      break;
   }
   case CE_RadioButton: // A QRadioButton, draws a PE_ExclusiveRadioButton, a case CE_RadioButtonLabel
   case CE_CheckBox: // A QCheckBox, draws a PE_IndicatorCheckBox, a case CE_CheckBoxLabel
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option))
      {
         QStyleOptionButton subopt = *btn;
         if (element == CE_RadioButton)
         {
            subopt.rect = subElementRect(SE_RadioButtonIndicator, btn, widget);
            drawPrimitive(PE_IndicatorRadioButton, &subopt, painter, widget);
            subopt.rect = subElementRect(SE_RadioButtonContents, btn, widget);
            drawControl(CE_RadioButtonLabel, &subopt, painter, widget);
         }
         else
         {
            subopt.rect = subElementRect(SE_CheckBoxIndicator, btn, widget);
            drawPrimitive(PE_IndicatorCheckBox, &subopt, painter, widget);
            subopt.rect = subElementRect(SE_CheckBoxContents, btn, widget);
            drawControl(CE_CheckBoxLabel, &subopt, painter, widget);
         }
      }
      break;
//    case CE_CheckBoxLabel: // The label (text or pixmap) of a QCheckBox
//    case CE_RadioButtonLabel: // The label (text or pixmap) of a QRadioButton
//    case CE_TabBarTab: // The tab and label within a QTabBar
   case CE_TabBarTabShape: // The tab shape within a tab bar
      if (const QStyleOptionTab *tab =
          qstyleoption_cast<const QStyleOptionTab *>(option))
      {
         Qt::Orientation o = Qt::Vertical;
         PosFlags pf = 0; int size = 0;
         QWidget *tabbar = widget ? widget->parentWidget() : 0L;
         bool restorePainter = false;

         switch (tab->shape)
         {
         case QTabBar::RoundedNorth:
         case QTabBar::TriangularNorth:
         case QTabBar::RoundedSouth:
         case QTabBar::TriangularSouth:
            pf = (Left | Right); size = RECT.height();
            if (tabbar && (widget->mapToParent(RECT.topLeft()).x() < 3 || widget->mapToParent(RECT.topRight()).x() > tabbar->width()-3))
            {
               painter->save();
               QRect rect = tabbar->rect(); rect.setHeight(RECT.height());
               painter->setClipRegion(round_frames[Raised].innerRegion(rect), Qt::IntersectClip);
               restorePainter = true;
            }
            break;
         case QTabBar::RoundedEast:
         case QTabBar::TriangularEast:
         case QTabBar::RoundedWest:
         case QTabBar::TriangularWest:
            o = Qt::Horizontal; pf = (Top | Bottom); size = RECT.width();
            if (tabbar && (widget->mapToParent(RECT.topLeft()).y() < 3 || widget->mapToParent(RECT.bottomLeft()).y() > tabbar->height()-3))
            {
               painter->save();
               QRect rect = tabbar->rect(); rect.setHeight(RECT.height());
               painter->setClipRegion(round_frames[Raised].innerRegion(rect), Qt::IntersectClip);
               restorePainter = true;
            }
         }
         
         if (option->state & State_Selected) // active tab
         {
            painter->drawTiledPixmap(RECT, gradient(PAL.color(QPalette::Window), size, o, Sunken));
            renderFrame( painter, RECT, Sunken, pf); // side shadow
         }
         else // basic
         {
            size -=4;
            painter->drawTiledPixmap((o == Qt::Vertical) ? RECT.adjusted(0,2,0,-2) : RECT.adjusted(2,0,-2,0),
                                     gloss(PAL.color(hover ? config.tabTextColor : config.tabColor), size, o, Raised));
         }
         if (restorePainter) painter->restore();
      }
      break;
   case CE_TabBarTabLabel: // The label within a tab
      if (const QStyleOptionTab *tab =
          qstyleoption_cast<const QStyleOptionTab *>(option))
      {
         painter->save();
         QStyleOptionTabV2 tabV2(*tab);
         QRect tr = tabV2.rect;//.adjusted(0,2,0,0);
         bool verticalTabs = tabV2.shape == QTabBar::RoundedEast
            || tabV2.shape == QTabBar::RoundedWest
            || tabV2.shape == QTabBar::TriangularEast
            || tabV2.shape == QTabBar::TriangularWest;
         bool selected = tabV2.state & State_Selected;
         if (verticalTabs)
         {
            int newX, newY, newRot;
            if (tabV2.shape == QTabBar::RoundedEast || tabV2.shape == QTabBar::TriangularEast)
            {
               newX = tr.width(); newY = tr.y(); newRot = 90;
            }
            else
            {
               newX = 0; newY = tr.y() + tr.height(); newRot = -90;
            }
            tr.setRect(0, 0, tr.height(), tr.width());
            QMatrix m;
            m.translate(newX, newY); m.rotate(newRot);
            painter->setMatrix(m, true);
         }
         
         int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
         
         if (!tabV2.icon.isNull())
         {
            QSize iconSize = tabV2.iconSize;
            if (!iconSize.isValid())
            {
               int iconExtent = pixelMetric(PM_SmallIconSize);
               iconSize = QSize(iconExtent, iconExtent);
            }
            QPixmap tabIcon = tabV2.icon.pixmap(iconSize, (isEnabled) ? QIcon::Normal : QIcon::Disabled);
            painter->drawPixmap(tr.left() + 6, tr.center().y() - tabIcon.height() / 2, tabIcon);
            tr.setLeft(tr.left() + iconSize.width() + 4);
         }
         // color adjustment
         QColor cF; QPalette::ColorRole crB;
         if (selected)
         {
            cF = PAL.color(QPalette::WindowText); crB = QPalette::Window;
         }
         else if (hover)
         {
            cF = PAL.color(config.tabColor); crB = config.tabTextColor;
         }
         else
         {
            cF = midColor(PAL.color(config.tabColor), PAL.color(config.tabTextColor),1,3);
            crB = config.tabColor;
         }
         if (verticalTabs)
         {
            QPixmap pixmap(tr.size());
            pixmap.fill(Qt::transparent);
            QPainter pixPainter(&pixmap);
            if (qGray(PAL.color(crB).rgb()) < 128) // dark background, let's paint an emboss
            {
               pixPainter.setPen(PAL.color(crB).dark(120));
               drawItemText(&pixPainter, pixmap.rect().translated(0,-1), alignment, PAL, isEnabled, tab->text);
            }
            pixPainter.setPen( cF);
            drawItemText(&pixPainter, pixmap.rect(), alignment, PAL, isEnabled, tab->text);
            drawItemPixmap(painter, tr, alignment, pixmap);
         }
         else
         {
            if (qGray(PAL.color(crB).rgb()) < 128) // dark background, let's paint an emboss
            {
               painter->setPen(PAL.color(crB).dark(120));
               tr.moveTop(tr.top()-1);
               drawItemText(painter, tr, alignment, PAL, isEnabled, tab->text);
               tr.moveTop(tr.top()+1);
            }
            painter->setPen(cF);
            drawItemText(painter, tr, alignment, PAL, isEnabled, tab->text);
         }
         painter->restore();
      }
      break;
      case CE_ProgressBar: // CE_ProgressBarGroove, CE_ProgressBarContents, CE_ProgressBarLabel
      if (const QStyleOptionProgressBar *pb
          = qstyleoption_cast<const QStyleOptionProgressBar *>(option))
      {
         QStyleOptionProgressBarV2 subopt = *pb;
//          if (!anmiationUpdate)
         {
            subopt.rect = subElementRect(SE_ProgressBarGroove, pb, widget);
            drawControl(CE_ProgressBarGroove, &subopt, painter, widget);
         }
         subopt.rect = subElementRect(SE_ProgressBarContents, pb, widget);
         drawControl(CE_ProgressBarContents, &subopt, painter, widget);
         if (pb->textVisible)
         {
            subopt.rect = subElementRect(SE_ProgressBarLabel, pb, widget);
            drawControl(CE_ProgressBarLabel, &subopt, painter, widget);
         }
      }
      break;
   case CE_ProgressBarGroove: // The groove where the progress indicator is drawn in a QProgressBar
      {
         if (const QStyleOptionProgressBarV2 *progress =
             qstyleoption_cast<const QStyleOptionProgressBarV2*>(option))
         {
            if (progress->orientation == Qt::Horizontal)
               painter->drawTiledPixmap(RECT, gradient(PAL.color(config.progressTextColor), RECT.height(), Qt::Vertical, Sunken));
            else
               painter->drawTiledPixmap(RECT, gradient(PAL.color(config.progressTextColor), RECT.width(), Qt::Horizontal, Sunken));
         renderFrame( painter, RECT, Sunken, Full /*???*/);
         }
         break;
      }
   case CE_ProgressBarContents: // The progress indicator of a QProgressBar
      if (const QStyleOptionProgressBarV2 *progress =
          qstyleoption_cast<const QStyleOptionProgressBarV2*>(option))
         {
            double val = progress->progress;
            bool reverse = option->direction == Qt::RightToLeft;
            if (progress->invertedAppearance) reverse = !reverse;
            val = val / (progress->maximum - progress->minimum);
            
            if ( val > 0.0 )
            {
               int s;
               QRect progressRect = RECT;
               QRegion cr;
               // vertical progressbar, shake it baby ;P
               if (progress->orientation == Qt::Vertical)
               {
                  s = qMin( RECT.height(), ( int ) (val * RECT.height() ) );
                  if ( s > 1 )
                  {
                     progressRect.setTop(RECT.bottom()-s+1);
                     progressRect.setHeight(s);
                     QPixmap pix(progressRect.width(), progressRect.width()<<1);
                     QPainter paint(&pix);
                     paint.drawTiledPixmap(0,0,pix.width(),pix.height()>>1, gradient(PAL.color(config.progressColor), pix.width(), Qt::Horizontal, Raised));
                     paint.drawTiledPixmap(0,pix.height()>>1,pix.width(),(pix.height()>>1)+1, gloss(PAL.color(config.progressColor), pix.width(), Qt::Horizontal, Raised));
                     paint.end();
                     painter->drawTiledPixmap(progressRect, pix, QPoint(0, progressShift));
                  }
               }
               else
               {
                  s = qMin( RECT.width()+RECT.height(), ( int ) (val * (RECT.width()+RECT.height()) ) );
                  if ( s > 1 )
                  {
//                      s += progressRect.height();
                     QRegion diag;
                     int xOff;
                     int shapeOff;
                     if (reverse)
                     {
                        progressRect.setLeft(RECT.right()-s+1);
                        progressRect.setWidth(s);
                        xOff = progressShift;
                        shapeOff = progressRect.x()+progressRect.height();
                        for (int i = 0; i < progressRect.height() ; i++)
                           diag += QRect(i,i,progressRect.height(),1);
                     }
                     else
                     {
                        progressRect.setWidth(s);
                        xOff = progressRect.x()-progressShift;
                        shapeOff = progressRect.right()+/*1+*/progressRect.height();
                        for (int i = 0; i < progressRect.height() ; i++)
                           diag += QRect(progressRect.height()-i-1,i,progressRect.height(),1);
                     }
                     
                     // render into pixmap first to avoid complex region shifts, unifications
                     QPixmap pix((progressRect.height()<<1)-1,progressRect.height());
                     QPainter paint(&pix);
                     cr = diag;
                     diag.translate(-pix.width(),0);
                     cr += diag;
                     paint.drawTiledPixmap(0,0,pix.width(),pix.height(), gradient(PAL.color(config.progressColor), pix.height(), Qt::Vertical, Raised));
                     paint.setClipRegion(cr);
                     paint.drawTiledPixmap(0,0,pix.width(),pix.height(), gloss(PAL.color(config.progressColor), pix.height(), Qt::Vertical, Raised));
                     paint.end();
                     // done
                     diag.translate(shapeOff, progressRect.y());
                     cr = progressRect;
                     cr -= diag;
                     painter->setClipRegion(cr);
                     painter->drawTiledPixmap(RECT, pix, QPoint(xOff,0));
                  }
               }
            }
         }
      break;
   case CE_ProgressBarLabel: // The text label of a QProgressBar
      if (const QStyleOptionProgressBarV2 *progress =
          qstyleoption_cast<const QStyleOptionProgressBarV2*>(option))
      {
         if (!anmiationUpdate)
            painter->save();
         QFont fnt = painter->font();
         fnt.setBold(true);
         painter->setFont(fnt);
         QRect rect = RECT;
         double val = progress->progress;
         bool reverse = option->direction == Qt::RightToLeft;
         if (progress->invertedAppearance) reverse = !reverse;
         val = val / (progress->maximum - progress->minimum);
         QMatrix m;
         if (progress->orientation == Qt::Vertical)
         {
            rect.setRect(RECT.x(), RECT.y(), RECT.height(), RECT.width());
            if (progress->bottomToTop)
            {
               m.translate(0.0, RECT.height()); m.rotate(-90);
            }
            else
            {
               m.translate(RECT.width(), 0.0); m.rotate(90);
            }
         }
         if ( val > 0.0 )
         {
            int s;
            QRegion cr;
            if (progress->orientation == Qt::Vertical)
            {
               s = qMin( RECT.height(), ( int ) (val * RECT.height() ) );
               if ( s > 1 )
                  cr = QRect(RECT.x(), RECT.bottom()-s+1, RECT.width(), s);
            }
            else
            {
               s = qMin( RECT.width()+RECT.height(), ( int ) (val * (RECT.width()+RECT.height()) ) );
               if ( s > 1 )
               {
//                   s += RECT.height();
                  QRect progressRect = RECT;
                  if (reverse)
                  {
                     progressRect.setLeft(RECT.right()-s+1);
                     progressRect.setWidth(s);
                     int left = progressRect.x()-progressRect.height();
                     for (int i = 0; i < progressRect.height(); i++)
                        cr += QRect(left+i,progressRect.y()+i,progressRect.height(),1);
                  }
                  else
                  {
                     progressRect.setWidth(s);
                     int right = progressRect.x()+progressRect.width();
                     for (int i = 0; i < progressRect.height(); i++)
                        cr += QRect(right-i-1, progressRect.y()+i, progressRect.height(),1);
                  }
                  cr = QRegion(progressRect) - cr;
               }
            }
            painter->setClipRegion(cr);
            painter->setMatrix(m);
            drawItemText(painter, rect, Qt::AlignCenter | Qt::TextSingleLine, PAL, isEnabled,
                         progress->text, config.progressTextColor);
            painter->resetMatrix();
            painter->setClipRegion(QRegion(RECT).subtract(cr));
         }
         if (anmiationUpdate)
            break;
         painter->setMatrix(m);
         drawItemText(painter, rect, Qt::AlignCenter | Qt::TextSingleLine, PAL, isEnabled,
                      progress->text, config.progressColor);
         painter->restore();
      }
      break;
   case CE_ToolButtonLabel: // A tool button's label
      if (const QStyleOptionToolButton *toolbutton
          = qstyleoption_cast<const QStyleOptionToolButton *>(option))
      {
         // Arrow type always overrules and is always shown
         bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
         if ((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty() ||
             toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly)
         {
            drawItemText(painter, RECT, Qt::AlignCenter | Qt::TextShowMnemonic, PAL, isEnabled, toolbutton->text, QPalette::WindowText);
         }
         else
         {
            QPixmap pm;
            QSize pmSize = toolbutton->iconSize;
            if (!toolbutton->icon.isNull())
            {
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
                    
            if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly)
            {
               painter->setFont(toolbutton->font);
               QRect pr = RECT, tr = RECT;
               int alignment = Qt::TextShowMnemonic;
               
               if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
               {
                  int fh = painter->fontMetrics().height();
                  pr.adjust(0, 3, 0, -fh - 3);
                  tr.adjust(0, pr.bottom(), 0, -3);
                  if (!hasArrow)
                     drawItemPixmap(painter, pr, Qt::AlignCenter, pm);
                  else
                     drawArrow(this, toolbutton, pr, painter, widget);
                  alignment |= Qt::AlignCenter;
               }
               else
               {
                  pr.setWidth(pmSize.width() + 8);
                  tr.adjust(pr.right(), 0, 0, 0);
                  if (!hasArrow)
                     drawItemPixmap(painter, pr, Qt::AlignCenter, pm);
                  else
                     drawArrow(this, toolbutton, pr, painter, widget);
                  alignment |= Qt::AlignLeft | Qt::AlignVCenter;
               }
               drawItemText(painter, tr, alignment, PAL, isEnabled, toolbutton->text, QPalette::WindowText);
            }
            else
            {
               if (hasArrow)
                  drawArrow(this, toolbutton, RECT, painter, widget);
               else
                  drawItemPixmap(painter, RECT, Qt::AlignCenter, pm);
            }
         }
      }
      break;
   case CE_MenuBarItem: // A menu item in a QMenuBar
      if (const QStyleOptionMenuItem *mbi =
          qstyleoption_cast<const QStyleOptionMenuItem *>(option))
      {
         QPalette::ColorRole cr = QPalette::WindowText;
         if (option->state & State_Selected)
         {
            painter->save();
            painter->setRenderHint ( QPainter::Antialiasing );
            painter->setPen ( PAL.color(QPalette::WindowText) );
            painter->setBrush ( PAL.color(QPalette::WindowText) );
            if (RECT.width() > RECT.height())
               painter->drawRoundRect(sunken ? RECT.adjusted(0,0,0,RECT.height()/5) : RECT,
                                      35*RECT.height()/RECT.width(), 35);
            else
               painter->drawRoundRect(sunken ? RECT.adjusted(0,0,0,RECT.height()/5) : RECT,
                                      35, 35*RECT.width()/RECT.height());
            painter->restore();
            cr = QPalette::Window;
         }
         uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
      
         QPixmap pix = mbi->icon.pixmap(pixelMetric(PM_SmallIconSize), isEnabled ? QIcon::Normal : QIcon::Disabled);
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
          qstyleoption_cast<const QStyleOptionMenuItem *>(option))
      {
         QPalette::ColorRole fg = QPalette::WindowText;
         QPalette::ColorRole bg = QPalette::Window;
         if (widget)
         {
            bg = widget->backgroundRole();
            fg = widget->foregroundRole();
         }
         
         if (menuItem->menuItemType == QStyleOptionMenuItem::Separator)
         {
            painter->drawTiledPixmap(RECT, gradient(PAL.color(QPalette::Active, bg), RECT.height(), Qt::Vertical, Raised));
            if (!menuItem->text.isEmpty())
            {
               painter->setFont(menuItem->font);
               drawItemText(painter, RECT, Qt::AlignCenter, PAL, isEnabled, menuItem->text, fg);
            }
            break;
         }
         painter->save();
         bool selected = menuItem->state & State_Selected;
         bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;
         bool checked = menuItem->checked;
         
         painter->fillRect(RECT, PAL.color(QPalette::Active, bg));
         if (selected)
         {
            painter->setRenderHint ( QPainter::Antialiasing );
            painter->setPen ( PAL.color(fg) );
            if (isEnabled)
               painter->setBrush ( PAL.color(fg) );
            if (RECT.width() > RECT.height())
               painter->drawRoundRect(RECT, 35*RECT.height()/RECT.width(), 35);
            else
               painter->drawRoundRect(RECT, 35, 35*RECT.width()/RECT.height());
         }
        
         // Check
         QRect checkRect(option->rect.left() + 7, option->rect.center().y() - 6, 13, 13);
         checkRect = visualRect(menuItem->direction, menuItem->rect, checkRect);
         if (checkable)
         {
            if (menuItem->checkType & QStyleOptionMenuItem::Exclusive)
            {
               // Radio button
               painter->setRenderHint ( QPainter::Antialiasing );
               painter->setPen ( PAL.color(selected ? bg : fg) );
               painter->drawEllipse ( checkRect );
               if (checked || sunken)
               {
                  painter->setBrush ( painter->pen().color() );
                  painter->drawEllipse ( checkRect.adjusted(checkRect.width()/4, checkRect.height()/4, -checkRect.width()/4, -checkRect.height()/4) );
               }
            }
            else
            {
               // Check box
               painter->setPen ( PAL.color(selected ? bg : fg) );
               QStyleOptionMenuItem tmpOpt = *menuItem;
               tmpOpt.rect = checkRect;
               if (checked) tmpOpt.state |= State_On; else tmpOpt.state &= ~State_On;
               drawPrimitive(PE_IndicatorMenuCheckMark, &tmpOpt, painter, widget);
            }
         }
         
         // Text and icon, ripped from windows style
         const QStyleOptionMenuItem *menuitem = menuItem;
         int checkcol = qMax(menuitem->maxIconWidth, checkable*20);
         QRect vCheckRect = visualRect(option->direction, menuitem->rect,
                                       QRect(menuitem->rect.x(), menuitem->rect.y(),
                                             checkcol, menuitem->rect.height()));
         if (!menuItem->icon.isNull())
         {
            QIcon::Mode mode = isEnabled ? (selected ? QIcon::Active : QIcon::Normal) : QIcon::Disabled;
            QPixmap pixmap = menuItem->icon.pixmap(pixelMetric(PM_SmallIconSize), mode, checked ? QIcon::On : QIcon::Off);
            
            QRect pmr(QPoint(0, 0), pixmap.size());
            pmr.moveCenter(vCheckRect.center());
            
            painter->drawPixmap(pmr.topLeft(), pixmap);
         }
         
         int x, y, w, h;
         menuitem->rect.getRect(&x, &y, &w, &h);
         int tab = menuitem->tabWidth;
         
         QColor discol;
         if (!isEnabled)
         {
            discol = midColor(PAL.color(bg),PAL.color(fg),2,1);
            painter->setPen(discol);
         }
         else
            painter->setPen(PAL.color(selected ? bg : fg));
         
         int xm = windowsItemFrame + checkcol + windowsItemHMargin;
         int xpos = menuitem->rect.x() + xm;
         QRect textRect(xpos, y + windowsItemVMargin, w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
         QRect vTextRect = visualRect(option->direction, menuitem->rect, textRect);
         QString s = menuitem->text;
         if (!s.isEmpty())
         {
            // draw text
            int t = s.indexOf('\t');
#define text_flags Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine
            if (t >= 0)
            {
               QRect vShortcutRect = visualRect(option->direction, RECT, QRect(textRect.topRight(), RECT.bottomRight()));
               painter->drawText(vShortcutRect, text_flags /*| Qt::AlignRight*/, s.mid(t + 1));
               s = s.left(t);
            }
            if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
            {
               QFont font = menuitem->font;
               font.setBold(true);
               painter->setFont(font);
            }
            painter->drawText(vTextRect, text_flags | Qt::AlignLeft, s.left(t));
         }
#undef text_flags
            // Arrow
         if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu)
         {
            // draw sub menu arrow
            PrimitiveElement arrow =
               (option->direction == Qt::RightToLeft) ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
            
            int dim = (RECT.height() - 4) / 2;
            int xpos = RECT.x() + RECT.width() - 7 - dim;
            
            QStyleOptionMenuItem tmpOpt = *menuItem;
            tmpOpt.rect = visualRect(option->direction, RECT, QRect(xpos, RECT.y() + (RECT.height() - dim)/2, dim, dim));
            
            drawPrimitive(arrow, &tmpOpt, painter, widget);
         }
         painter->restore();
      }
      break;
   case CE_MenuScroller: // Scrolling areas in a QMenu when the style supports scrolling
   {
      QPalette::ColorRole bg = widget ? widget->backgroundRole() : QPalette::Window;
      if (option->state & State_DownArrow)
      {
         painter->drawTiledPixmap(RECT, gradient(PAL.color(QPalette::Active, bg),
            RECT.height()*2, Qt::Vertical, sunken ? Sunken : Raised), QPoint(0,RECT.height()));
         drawPrimitive(PE_IndicatorArrowDown, option, painter, widget);
      }
      else
      {
         painter->drawTiledPixmap(RECT, gradient(PAL.color(QPalette::Active, bg),
            RECT.height()*2, Qt::Vertical, sunken ? Sunken : Raised));
         drawPrimitive(PE_IndicatorArrowUp, option, painter, widget);
      }
      break;
   }
//    case CE_MenuTearoff: // A menu item representing the tear off section of a QMenu
   case CE_MenuEmptyArea: // The area in a menu without menu items
      painter->fillRect(RECT,PAL.color(widget?widget->backgroundRole():QPalette::Window));
      break;
   case CE_MenuHMargin: // The horizontal extra space on the left/right of a menu
   case CE_MenuVMargin: // The vertical extra space on the top/bottom of a menu
      break;
   case CE_Q3DockWindowEmptyArea: // The empty area of a QDockWidget
      break;
   case CE_ToolBoxTab: // The toolbox's tab area
      if (const QStyleOptionToolBox* tbt =
          qstyleoption_cast<const QStyleOptionToolBox*>(option))
      {
         QPalette::ColorRole role = QPalette::WindowText, bgRole = QPalette::Window;
         if (isEnabled)
         {
            if (option->state & State_Selected)
            {
               bgRole = QPalette::WindowText;
               painter->drawTiledPixmap(RECT, gradient(PAL.color(bgRole), RECT.height(), Qt::Vertical, Raised));
               role = QPalette::Window;
               QFont f(painter->font());
               f.setBold(true);
               painter->setFont(f);
            }
            else
               painter->drawTiledPixmap(RECT, colorRun(PAL.color(bgRole), RECT.height(), Qt::Vertical, sunken ? Sunken : Raised, !hover));
         }
            
         /**i WANT (read this TrottelTech: WANT!) this to be color swapped on select (and centered as sugar on top)
         now as the toolboxbutton is a private class and it's selected member is as well, i cannot overwrite the paint event
         so instead i repeat a null rect for selected tabs contents (from subElementRect query), what makes the widget abort the content painting
         then i paint it instead... works ;)  */
         QPixmap pm = tbt->icon.pixmap(pixelMetric(QStyle::PM_SmallIconSize), isEnabled ? QIcon::Normal : QIcon::Disabled);
         
         QRect cr = option->rect.adjusted(0, 0, -30, 0);
         QRect tr, ir;
         int ih = 0;
         if (pm.isNull())
         {
            tr = cr;
            tr.adjust(4, 0, -8, 0);
         }
         else
         {
            int iw = pm.width() + 4;
            ih = pm.height();
            ir = QRect(cr.left() + 4, cr.top(), iw + 2, ih);
            tr = QRect(ir.right(), cr.top(), cr.width() - ir.right() - 4, cr.height());
         }
         
         QString txt = tbt->text;
         if (painter->fontMetrics().width(txt) > tr.width())
         {
            txt = txt.left(1);
            int ew = painter->fontMetrics().width("...");
            int i = 1;
            while (painter->fontMetrics().width(txt) + ew + painter->fontMetrics().width(tbt->text[i])  < tr.width())
               txt += tbt->text[i++];
            txt += "...";
         }
         
         if (ih)
            painter->drawPixmap(ir.left(), (RECT.height() - ih) / 2, pm);
         
         if (qGray(PAL.color(bgRole).rgb()) < 128) // dark background, let's paint an emboss
         {
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
   if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option))
   {
      QRegion clipRegion = painter->clipRegion();
      painter->setClipRect(option->rect);
      drawControl(CE_HeaderSection, header, painter, widget);
      QStyleOptionHeader subopt = *header;
      subopt.rect = subElementRect(SE_HeaderLabel, header, widget);
      if (subopt.rect.isValid())
         drawControl(CE_HeaderLabel, &subopt, painter, widget);
      if (header->sortIndicator != QStyleOptionHeader::None)
      {
         subopt.rect = subElementRect(SE_HeaderArrow, option, widget);
         QPen oldPen = painter->pen();
         painter->setPen(PAL.color(QPalette::Base));
         drawPrimitive(PE_IndicatorHeaderArrow, &subopt, painter, widget);
         painter->setPen(oldPen);
      }
      painter->setClipRegion(clipRegion);
      break;
   }
   case CE_HeaderSection: // A header section
   {
   // NOT emphasize, as base/text is usually white/black -> no emphasize possible...
#define _COLOR_ PAL.color(QPalette::Text)
      const QHeaderView* header = qobject_cast<const QHeaderView*>(widget);
      const QStyleOptionHeader* hopt = qstyleoption_cast<const QStyleOptionHeader*>(option);
      if (sunken)
         painter->drawTiledPixmap(RECT, gloss(_COLOR_, RECT.height(), Qt::Vertical, Sunken));
      else if (!hover && (header && header->isClickable () && hopt && header->sortIndicatorSection() == hopt->section))
         painter->drawTiledPixmap(RECT, gradient(_COLOR_, RECT.height(), Qt::Vertical, Sunken));
      else
      {
         int x,y,w,h;
         RECT.getRect(&x,&y,&w,&h);
         w--;
         if (hover)
            painter->drawTiledPixmap(x,y,w,h, gloss(_COLOR_, RECT.height(), Qt::Vertical, Raised));
         else
            painter->drawTiledPixmap(x,y,w,h, gradient(_COLOR_, RECT.height(), Qt::Vertical, Raised));
         painter->drawTiledPixmap(x+w,y,1,h, gradient(_COLOR_, RECT.height(), Qt::Vertical, Sunken));
      }
      break;
#undef _COLOR_
   }
   case CE_HeaderLabel: // The header's label
   {
      const QStyleOptionHeader* hopt = qstyleoption_cast<const QStyleOptionHeader*>(option);
      QRect rect = option->rect;
      if ( !hopt->icon.isNull() )
      {
         QPixmap pixmap = hopt->icon.pixmap( 22,22, isEnabled ? QIcon::Normal : QIcon::Disabled );
         int pixw = pixmap.width();
         int pixh = pixmap.height();
         
         QRect pixRect = option->rect;
         pixRect.setY( option->rect.center().y() - (pixh - 1) / 2 );
      // "pixh - 1" because of tricky integer division
         drawItemPixmap ( painter, pixRect, Qt::AlignCenter, pixmap );
         rect.setLeft( rect.left() + pixw + 2 );
      }
      painter->save();
      if (qGray(PAL.color(QPalette::Text).rgb()) < 128) // dark background, let's paint an emboss
      {
         rect.moveTop(rect.top()-1);
         painter->setPen(PAL.color(QPalette::Text).dark(120));
         drawItemText ( painter, rect, Qt::AlignCenter, PAL, isEnabled, hopt->text);
         rect.moveTop(rect.top()+1);
      }
      painter->setPen(PAL.color(QPalette::Base));
      drawItemText ( painter, rect, Qt::AlignCenter, PAL, isEnabled, hopt->text);
      painter->restore();
      break;
   }
   case CE_ScrollBarAddLine: // Scroll bar line increase indicator. (i.e., scroll down); see also QScrollBar.
   case CE_ScrollBarSubLine: // Scroll bar line decrease indicator (i.e., scroll up).
      if (const QStyleOptionSlider *opt =
            qstyleoption_cast<const QStyleOptionSlider *>(option))
      {
         if (!(opt->minimum < opt->maximum))
         {
            if (option->state & QStyle::State_Horizontal)
               painter->drawTiledPixmap(RECT, gradient(PAL.color(config.scrollbarBg), RECT.height(), Qt::Vertical, Sunken));
            else
               painter->drawTiledPixmap(RECT, gradient(PAL.color(config.scrollbarBg), RECT.width(), Qt::Horizontal, Sunken));
            break;
         }
         Qt::Orientation direction; int size; PrimitiveElement arrow = (PrimitiveElement)0;
         if (option->state & QStyle::State_Horizontal)
         {
            direction = Qt::Vertical;
            size = RECT.height();
            if (element == CE_ScrollBarAddLine && opt->sliderValue < opt->maximum)
               arrow = (option->direction == Qt::RightToLeft) ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
            else if (element == CE_ScrollBarSubLine && opt->sliderValue > opt->minimum)
               arrow = (option->direction == Qt::RightToLeft) ? PE_IndicatorArrowRight : PE_IndicatorArrowLeft;
         }
         else
         {
            direction = Qt::Horizontal;
            size = RECT.width();
            if (element == CE_ScrollBarAddLine && opt->sliderValue < opt->maximum)
               arrow = PE_IndicatorArrowDown;
            else if (element == CE_ScrollBarSubLine && opt->sliderValue > opt->minimum)
               arrow = PE_IndicatorArrowUp;
         }
         if (!arrow)
         {
            painter->drawTiledPixmap(RECT, gloss(PAL.color(config.scrollbarBg), size, direction, Raised));
            break;
         }
         else
            painter->drawTiledPixmap(RECT, colorRun(PAL.color(config.scrollbarBg), size, direction, sunken ? Sunken : Raised, sunken));
         painter->save();
         if (hover)
            painter->setPen(PAL.color(config.scrollbarFg));
         else
            painter->setPen(midColor(PAL.color(config.scrollbarBg),PAL.color(config.scrollbarFg),1,2));
         QStyleOption tmpOpt = *option;
         tmpOpt.rect.adjust(RECT.width()/3,RECT.height()/3,-RECT.width()/3,-RECT.height()/3);
         drawPrimitive(arrow, &tmpOpt, painter, widget);
         painter->restore();
         break;
      }
   case CE_ScrollBarAddPage: // Scolllbar page increase indicator (i.e., page down).
   case CE_ScrollBarSubPage: // Scroll bar page decrease indicator (i.e., page up).
   case CE_ScrollBarSlider: // Scroll bar slider.
      if (const QStyleOptionSlider *opt =
          qstyleoption_cast<const QStyleOptionSlider *>(option))
      {
      Qt::Orientation direction; int size; QRect innerRect;
      if (option->state & QStyle::State_Horizontal)
      {
         direction = Qt::Vertical;
         size = RECT.height();
      }
      else
      {
         direction = Qt::Horizontal;
         size = RECT.width();
      }

      // the groove (add or sub page or if min == max, i.e. no slide usefull)
      if (!(element == CE_ScrollBarSlider && opt->minimum < opt->maximum))
      {
         painter->drawTiledPixmap(RECT, gradient(PAL.color(config.scrollbarBg), size, direction, Sunken));
         if (element != CE_ScrollBarSlider)
         {
            int size2 = 2*size/5;
            int off1, off2;
            
            if (element == CE_ScrollBarAddPage)
            {
               off1 = 0; off2 = -size/2;
            }
            else
            {
               off1 = size/2; off2 = 0;
            }
            if (option->state & QStyle::State_Horizontal)
               innerRect = RECT.adjusted(off1, size2+1, off2, -size2+1);
            else
               innerRect = RECT.adjusted(size2+1, off1, -size2+1, off2);
            painter->drawTiledPixmap(innerRect, gradient(PAL.color(config.scrollbarBg), size2, direction, Raised));
         }
         break;
      }
         
      // we need to paint a slider
      innerRect = RECT.adjusted(1, 1, -1, -1);
      painter->drawTiledPixmap(RECT, gradient(PAL.color(config.scrollbarBg), size, direction, Sunken));
      size -= 2;
      if (sunken)
         painter->drawTiledPixmap(innerRect, gradient(PAL.color(config.scrollbarFg), size, direction, Sunken));
      else if (hover)
         painter->drawTiledPixmap(innerRect, gloss(PAL.color(config.scrollbarFg), size, direction, Raised));
      else
      {
//          bool scrollerActive = false;
         QWidget *scrollWidget = const_cast<QWidget*>(widget);
         if (!scrollWidget->isEnabled())
            break;
         while (scrollWidget && !(qobject_cast<QAbstractScrollArea*>(scrollWidget) || qobject_cast<Q3ScrollView*>(scrollWidget)))
            scrollWidget = const_cast<QWidget*>(scrollWidget->parentWidget());
         bool isActive = true;
         if (scrollWidget)
         {
//             QAbstractScrollArea* scrollWidget = (QAbstractScrollArea*)daddy;
            QPoint tl = scrollWidget->mapToGlobal(QPoint(0,0));
            QRegion scrollArea(tl.x(),tl.y(),scrollWidget->width(),scrollWidget->height());
            QList<QAbstractScrollArea*> scrollChilds = scrollWidget->findChildren<QAbstractScrollArea*>();
            for (int i = 0; i < scrollChilds.size(); ++i)
            {
               QPoint tl = scrollChilds[i]->mapToGlobal(QPoint(0,0));
               scrollArea -= QRegion(tl.x(), tl.y(), scrollChilds[i]->width(), scrollChilds[i]->height());
            }
            QList<Q3ScrollView*> scrollChilds2 = scrollWidget->findChildren<Q3ScrollView*>();
            for (int i = 0; i < scrollChilds2.size(); ++i)
            {
               QPoint tl = scrollChilds[i]->mapToGlobal(QPoint(0,0));
               scrollArea -= QRegion(tl.x(), tl.y(), scrollChilds2[i]->width(), scrollChilds2[i]->height());
            }
//             scrollerActive = scrollArea.contains(QCursor::pos());
            isActive = scrollArea.contains(QCursor::pos());
         }
         if (isActive)
            painter->drawTiledPixmap(innerRect, gradient(PAL.color(config.scrollbarFg), size, direction, Sunken));
      }
   }
      break;
//    case CE_ScrollBarFirst: // Scroll bar first line indicator (i.e., home).
//    case CE_ScrollBarLast: // Scroll bar last line indicator (i.e., end).
   case CE_RubberBand: // Rubber band used in such things as iconview.
   {
      painter->save();
      QColor c = PAL.color(QPalette::Highlight);
      painter->setPen(c);
      c.setAlpha(160);
      painter->setBrush(c);
      painter->restore();
      break;
   }
   case CE_FocusFrame: // Focus Frame that can is style controled.
      break;
   case CE_ComboBoxLabel: // The label of a non-editable QComboBox
      if (const QStyleOptionComboBox *cb =
          qstyleoption_cast<const QStyleOptionComboBox *>(option))
      {
         QRect editRect = subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, widget);
         painter->save();
         painter->setClipRect(editRect);
         if (!cb->currentIcon.isNull())
         {
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
         if (!cb->currentText.isEmpty() && !cb->editable)
         {
            editRect.adjust(1, (cb->state & State_On ) ? 1 : 0, -1, 0);
            drawItemText(painter, editRect, Qt::AlignLeft | Qt::AlignVCenter, PAL, isEnabled, cb->currentText);
         }
         painter->restore();
      }
      break;
   case CE_ToolBar:
      break;
   default:
         QCommonStyle::drawControl( element, option, painter, widget );
   } // switch
#undef RECT
#undef PAL
}
