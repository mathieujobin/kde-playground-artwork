#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QStyleOptionButton>
#include <QPainter>
#include <QPainterPath>
#include <QX11Info>
#include "oxygen.h"
#include "dynamicbrush.h"

#include "inlinehelp.cpp"

using namespace Oxygen;

extern int bgYoffset_;
extern Pixmap shadowPix;
extern Config config;

/**gradients or glosses*/
inline const QPixmap OxygenStyle::colorRun(const QColor &c, const int size, Qt::Orientation o, Orientation3D o3D, bool smooth) const
{
   if (smooth)
      return gradient(c, size, o, o3D);
   return gloss(c, size, o, o3D);
}

void OxygenStyle::drawPrimitive ( PrimitiveElement pe, const QStyleOption * option, QPainter * painter, const QWidget * widget) const
{
   Q_ASSERT(option);
   Q_ASSERT(painter);
#define RECT option->rect
#define PAL option->palette
   
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
#define HOVERCOLOR emphasize(PAL.color(cr))
      const QStyleOptionButton *opt = qstyleoption_cast<const QStyleOptionButton*>(option);
      painter->save();
//       bool isIconButton = opt && opt->text.isEmpty() && !opt->icon.isNull();
      bool isOn = option->state & State_On;
      bool isDefault = opt && (opt->features & QStyleOptionButton::DefaultButton);
      
      QColor c = btnBgColor(PAL, isEnabled, hasFocus, hover);
      
      Orientation3D o3D = !isEnabled ? Sunken : Relief;
      if (config.raisedButtons) o3D = (Orientation3D)((int)o3D + 1);
      
      QRegion innerRegion;
      
      if (config.roundButtons)
         innerRegion = round_frames[o3D].innerRegion(RECT);
      else
         innerRegion = frames[o3D].innerRegion(RECT);
      painter->setClipRegion(innerRegion);
      QRect ir = innerRegion.boundingRect();
      
      if (sunken || isOn) // pressed
      {
      // might be faster to no clip the inner button away when painting the outer part
      // TODO: profile and in case leave clipping
         QRect sunkenRect(ir.x()+2,ir.y()+2,ir.width()-4,ir.height()-4);
         painter->setClipRegion(innerRegion.subtract(sunkenRect));
         painter->drawTiledPixmap(ir, gloss(c, ir.height(),Qt::Vertical, Raised));
         painter->setClipRect(sunkenRect);
         painter->drawTiledPixmap(ir, gradient(c, ir.height(), Qt::Vertical, Sunken));
      }
      else // basic, hovered
         painter->drawTiledPixmap(ir, gloss(c, ir.height(), Qt::Vertical, (!config.inverseButtons || hover || isDefault) ? Raised : Sunken));

      painter->setClipping(false);
      renderFrame( painter, RECT, o3D, Full, widget, false, config.roundButtons );
      painter->restore();
      break;
#undef HOVERCOLOR
   }
   case PE_PanelButtonTool: // Panel for a Tool button, used with QToolButton.
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
         renderFrame( painter, RECT, Sunken, Full, widget, hasFocus, true );
      }
      else if (hover)
         renderFrame( painter, RECT, Relief, Full, widget, hasFocus, true );
      break;
   }
   case PE_PanelLineEdit: // Panel for a QLineEdit.
   {
      if (const QLineEdit* le = qobject_cast<const QLineEdit*>(widget))
         if (!le->hasFrame())
         {
            painter->fillRect(RECT, PAL.color(QPalette::Base));
            break;
         }
      QRect r2(RECT.x()+4, RECT.y()+4, RECT.width()-8, RECT.height()-8);
//       if (option->state & State_On)
//          painter->drawTiledPixmap(r2, gradient(PAL.color(QPalette::Base).dark(110), RECT.height()*2, Qt::Vertical, Raised));
//       else
         painter->fillRect(r2, PAL.color(QPalette::Base));
      if (hasFocus)
         renderFrame( painter, RECT, Sunken, Full, widget, true);
//       else
      {
         r2 = QRect(RECT.x()+2, RECT.y()+2, RECT.width()-4, RECT.height()-4);
         renderFrame( painter, r2, Sunken, Full, widget, false);
      }
      break;
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
      painter->setBrush(painter->pen().color());
      
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
         QRect rect = RECT; rect.adjust(2,2,-2,-2);

         QColor bc = btnBgColor(PAL, isEnabled, hasFocus, hover);
         QColor fc = btnFgColor(PAL, isEnabled, hasFocus, hover);
         
         Orientation3D o3d = (hover || !config.inverseButtons) ? Raised : Sunken;
         
         if (hasFocus)
         {
            renderFrame( painter, RECT, Sunken, Full, widget, true );
         }
         
         QRegion innerRegion;
         QRect ir;
         if (sunken) //pressed
         {
            innerRegion = frames[Sunken].innerRegion(rect);
            ir = innerRegion.boundingRect();
            painter->save();
            painter->setClipRegion(innerRegion);
            painter->drawTiledPixmap(ir, gradient(bc, ir.height(),Qt::Vertical, Sunken));
            painter->restore();
            renderFrame( painter, rect, Sunken, Full );
            break;
         }
         
         if (option->state & State_Off)
            innerRegion = frames[Raised].innerRegion(rect);
         else// if (option->state & State_On)
            innerRegion = frames[Sunken].innerRegion(rect);
         
         ir = innerRegion.boundingRect();
         
         painter->save();
         painter->setClipRegion(innerRegion);
         painter->drawTiledPixmap(ir, colorRun( bc, ir.height(), Qt::Vertical, o3d, !isEnabled));
         
         if (option->state & State_On)
         {
            int w2 = (ir.width()>>1);
            int xoff = ((ir.width()-w2)>>1);
            if (xoff*2+w2 != ir.width())
               w2++;
            int h2 = (ir.height()>>1);
            int yoff = ((ir.height()-h2)>>1);
            if (yoff*2+h2 != ir.height())
               h2++;
            painter->setClipRegion( QRect(ir.x()+xoff, ir.y()+yoff-1, w2, h2));
            painter->drawTiledPixmap(ir, colorRun(fc, ir.height(), Qt::Vertical, o3d, !isEnabled));
            painter->restore();
            renderFrame( painter, rect, Sunken, Full );
         }
         else if ( option->state & State_Off )
         {
            painter->restore();
            renderFrame( painter, rect, Raised, Full );
         }
         else // TRISTATE
         {
            int h3 = ir.height()/3;
            if (!(h3 % 2)) h3++;
            painter->setClipRegion(innerRegion.intersect(QRect(RECT.x(),ir.y()+((ir.height()-h3)>>1),rect.width(),h3)));
            painter->drawTiledPixmap(ir, colorRun(fc, ir.height(), Qt::Vertical, o3d, !isEnabled));
            painter->restore();
            renderFrame( painter, rect, Sunken, Full );
         }
         break;
      }
   case PE_IndicatorRadioButton: // Exclusive on/off indicator, for example, a QRadioButton.
   {
      if (isEnabled)
      {
         painter->save();
         QRegion innerRegion = round_frames[Sunken].innerRegion(RECT);
         QRect ir = innerRegion.boundingRect();
         painter->setClipRegion(innerRegion);
         if (option->state & State_On) // active 'n dead
         {
            painter->drawTiledPixmap(ir, gradient(PAL.color(QPalette::Window), ir.height(), Qt::Vertical, Sunken));
            // drop
            painter->setRenderHint ( QPainter::Antialiasing );
            painter->setPen(PAL.color(QPalette::WindowText));
            painter->setBrush(PAL.color(QPalette::WindowText));
            painter->drawEllipse ( RECT.adjusted(2*RECT.width()/5, 2*RECT.width()/5, -2*RECT.width()/5, -2*RECT.height()/5));
         }
         else if (sunken)
            painter->drawTiledPixmap(ir, gradient(PAL.color(QPalette::Window), ir.height(), Qt::Vertical, Sunken));
         else
            painter->drawTiledPixmap(ir, gloss(btnBgColor(PAL, isEnabled, hasFocus, hover), ir.height(), Qt::Vertical, Raised));
         painter->setClipping(false);
         renderFrame(painter, RECT, Sunken, Full, widget, false, true);
         if (hasFocus)
         {
            painter->setRenderHint ( QPainter::Antialiasing );
            painter->setBrush(Qt::NoBrush);
            QPen pen = painter->pen();
//             pen.setWidth(2);
            QColor c = PAL.color(QPalette::Highlight);
            pen.setColor(c); painter->setPen(pen);
            painter->drawEllipse ( RECT.adjusted(1,1,-1,-1) );
            c.setAlpha(128); pen.setColor(c); painter->setPen(pen);
            painter->drawEllipse ( RECT );
         }
         painter->restore();
      }
      else
      {
         bool hadAntiAlias = painter->renderHints() & QPainter::Antialiasing;
         painter->setRenderHint ( QPainter::Antialiasing );
         painter->setPen(PAL.color(QPalette::WindowText));
         painter->drawEllipse ( RECT.adjusted(2,2,-2,-2) );
         painter->setRenderHint ( QPainter::Antialiasing, hadAntiAlias );
      }
   }
      break;
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
         renderFrame( painter, RECT, Sunken, Full, widget, true);
//       else
      {
         QRect rect = RECT.adjusted(2,2,-2,-2); // this is ok, as we set the frame to 4px anyway ;)
         renderFrame( painter, rect, sunken ? Sunken :
                   (option->state & QStyle::State_Raised) ?
                   Raised : Relief, Full);
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
//       renderFrame( painter, RECT, config.tabwidget3D, Full, widget, false, true );
//       painter->setClipRegion(round_frames[config.tabwidget3D].innerRegion(RECT), Qt::IntersectClip);
      if (const QStyleOptionTabWidgetFrame *twf =
          qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option))
      {
         QRect rect(RECT);
         int baseHeight = pixelMetric( PM_TabBarBaseHeight, option, widget );
         Qt::Orientation o = Qt::Vertical;
         switch (twf->shape)
         {
         case QTabBar::RoundedNorth:
         case QTabBar::TriangularNorth:
            rect.setHeight(baseHeight);
//             renderFrame( painter, rect.adjusted(2,0,-2,0), Sunken, Bottom);
            break;
         case QTabBar::RoundedSouth:
         case QTabBar::TriangularSouth:
            rect.setTop(rect.bottom()-baseHeight);
//             renderFrame( painter, rect.adjusted(2,0,-2,0), Sunken, Top);
            break;
         case QTabBar::RoundedEast:
         case QTabBar::TriangularEast:
            rect.setLeft(rect.right()-baseHeight);
            o = Qt::Horizontal;
//             renderFrame( painter, rect.adjusted(0,2,0,-2), Sunken, Left);
            break;
         case QTabBar::RoundedWest:
         case QTabBar::TriangularWest:
            rect.setWidth(baseHeight);
            o = Qt::Horizontal;
//             renderFrame( painter, rect.adjusted(0,2,0,-2), Sunken, Right);
            break;
         }
//          QRect r2 = rect.adjusted(2,2,-2,-2);
         renderFrame( painter, rect, config.tabwidget3D, Full, widget, false, true );
         painter->setClipRegion(round_frames[config.tabwidget3D].innerRegion(rect), Qt::IntersectClip);
         painter->drawTiledPixmap(rect.adjusted(2,2,-2,-2), gloss(PAL.color(config.tabColor), baseHeight-4, o, Raised));
      }
      break;
   case PE_FrameLineEdit: // Panel frame for line edits.
      if (!hasFocus)
      {
         QRect rect(RECT); rect.adjust(2,2,-2,-2);
         renderFrame( painter, rect, Sunken, Full);
      }
      else
         renderFrame( painter, RECT, Sunken, Full, widget, true);
      break;
   case PE_FrameGroupBox: // Panel frame around group boxes.
   {
      QRegion innerRegion = round_frames[Sunken].innerRegion(RECT);
      painter->save();
      painter->setClipRegion(innerRegion);
#if 1
#define GRECT RECT.translated(widget->mapTo ( widget->topLevelWidget(), QPoint(0,0) ))
      if (_bgBrush && widget)
         painter->drawPixmap(RECT, _bgBrush->shadow(GRECT));
#undef GRECT
      else 
#endif
      if (config.bgMode == Scanlines)
         painter->drawTiledPixmap(RECT, *_scanlines[2]);
      else
         painter->drawTiledPixmap(RECT, gradient(PAL.color(QPalette::Window), RECT.height()*3/2, Qt::Vertical, Sunken));
      painter->restore();
      renderFrame( painter, RECT, Sunken, Full, widget, hasFocus, true);
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
         painter->setBrush(painter->pen().color());
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
   {
      QRegion innerRegion = frames[Relief].innerRegion(RECT);
      painter->save();
      painter->setClipRegion(innerRegion);
      QRect ir = innerRegion.boundingRect();
      painter->drawTiledPixmap(ir, gradient(PAL.color(QPalette::Background), ir.height(), Qt::Vertical, Raised));
      painter->restore();
      renderFrame( painter, RECT, Relief, Full);
      break;
   }
   case PE_IndicatorTabTear: // An indicator that a tab is partially scrolled out of the visible tab bar when there are many tabs.
      break;
   default:
      QCommonStyle::drawPrimitive( pe, option, painter, widget );
   } // switch
#undef RECT
#undef PAL
}
