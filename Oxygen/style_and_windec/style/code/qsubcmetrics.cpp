#include <QApplication>
#include <QTabBar>
#include <QStyleOption>
#include <QStyleOptionTab>
#include "oxygen.h"

using namespace Oxygen;

QRect OxygenStyle::subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget) const
{
   QRect ret;
   switch (control)
   {
   case CC_SpinBox: // A spinbox, like QSpinBox
      if (const QStyleOptionSpinBox *spinbox =
          qstyleoption_cast<const QStyleOptionSpinBox *>(option))
      {
         QSize bs;
         int fw = spinbox->frame ? pixelMetric(PM_SpinBoxFrameWidth, spinbox, widget) : 0;
         bs.setHeight(qMax(8, spinbox->rect.height()/2));
            // 1.6 -approximate golden mean
         bs.setWidth(qMax(18, qMin(bs.height() * 8 / 5, spinbox->rect.width() / 4)));
         bs = bs.expandedTo(QApplication::globalStrut());
         int x = spinbox->rect.width() - bs.width();
         switch (subControl)
         {
         case SC_SpinBoxUp:
            ret = QRect(x, 0, bs.width(), bs.height());
            break;
         case SC_SpinBoxDown:
            ret = QRect(x, bs.height(), bs.width(), bs.height());
            break;
         case SC_SpinBoxEditField:
            ret = QRect(fw, fw, x - fw, spinbox->rect.height() - 2*fw);
            break;
         case SC_SpinBoxFrame:
            ret = spinbox->rect;
         default:
            break;
         }
         ret = visualRect(spinbox->direction, spinbox->rect, ret);
      }
   case CC_ComboBox: // A combobox, like QComboBox
      if (const QStyleOptionComboBox *cb =
          qstyleoption_cast<const QStyleOptionComboBox *>(option))
      {
         int x = cb->rect.x(),
            y = cb->rect.y(),
            wi = cb->rect.width(),
            he = cb->rect.height();
         int margin = cb->frame ? 4 : 0;

         switch (subControl)
         {
         case SC_ComboBoxFrame:
            ret = cb->rect;
            break;
         case SC_ComboBoxArrow:
            x += wi; wi = (int)((he - 2*margin)*1.4);//1.618
            x -= margin + wi; // golden mean
            if (cb->editable)
               y += margin;
            else if (cb->state & State_On)
               y += margin-1;
            else
               y += margin-2;
            ret.setRect(x, y, wi, he - 2*margin);
            break;
         case SC_ComboBoxEditField:
            wi -= (int)((he - 2*margin)*1.4) + 2*margin;
            ret.setRect(x+margin, y+margin, wi, he - 2*margin);
            break;
         case SC_ComboBoxListBoxPopup:
            ret = cb->rect;
            if (cb->editable)
            {
               ret.adjust(3,0,-3,0);
               ret.moveTop(ret.y()-3);
            }
            else
            {
               ret.adjust(8,0,-8,0);
               ret.moveTop(ret.y()+1);
            }
            break;
         default:
            break;
         }
         ret = visualRect(cb->direction, cb->rect, ret);
      }
      break;
   case CC_ScrollBar: // A scroll bar, like QScrollBar
      if (const QStyleOptionSlider *scrollbar =
          qstyleoption_cast<const QStyleOptionSlider *>(option))
      {
         int sbextent = pixelMetric(PM_ScrollBarExtent, scrollbar, widget);
         int maxlen = ((scrollbar->orientation == Qt::Horizontal) ?
                       scrollbar->rect.width() : scrollbar->rect.height()) - (sbextent * 2);
         int sliderlen;
         
            // calculate slider length
         if (scrollbar->maximum != scrollbar->minimum)
         {
            uint range = scrollbar->maximum - scrollbar->minimum;
            sliderlen = (qint64(scrollbar->pageStep) * maxlen) / (range + scrollbar->pageStep);
            
            int slidermin = pixelMetric(PM_ScrollBarSliderMin, scrollbar, widget);
            if (sliderlen < slidermin || range > INT_MAX / 2)
               sliderlen = slidermin;
            if (sliderlen > maxlen)
               sliderlen = maxlen;
         }
         else
            sliderlen = maxlen;
        
         int sliderstart = sliderPositionFromValue(scrollbar->minimum,
            scrollbar->maximum, scrollbar->sliderPosition, maxlen - sliderlen,
            scrollbar->upsideDown);
         switch (subControl)
         {
         case SC_ScrollBarSubLine:            // top/left button
            if (scrollbar->orientation == Qt::Horizontal)
            {
               int buttonWidth = qMin(scrollbar->rect.width() / 2, sbextent);
               ret.setRect(scrollbar->rect.width() - 2*buttonWidth, 0, buttonWidth, sbextent);
            }
            else
            {
               int buttonHeight = qMin(scrollbar->rect.height() / 2, sbextent);
               ret.setRect(0, scrollbar->rect.height() - 2*buttonHeight, sbextent, buttonHeight);
            }
            break;
         case SC_ScrollBarAddLine:            // bottom/right button
            if (scrollbar->orientation == Qt::Horizontal)
            {
               int buttonWidth = qMin(scrollbar->rect.width()/2, sbextent);
               ret.setRect(scrollbar->rect.width() - buttonWidth, 0, buttonWidth, sbextent);
            } else
            {
               int buttonHeight = qMin(scrollbar->rect.height()/2, sbextent);
               ret.setRect(0, scrollbar->rect.height() - buttonHeight, sbextent, buttonHeight);
            }
            break;
         case SC_ScrollBarSubPage:            // between top/left button and slider
            if (scrollbar->orientation == Qt::Horizontal)
               ret.setRect(0, 0, sliderstart, sbextent);
            else
               ret.setRect(0, 0, sbextent, sliderstart);
            break;
         case SC_ScrollBarAddPage:            // between bottom/right button and slider
            if (scrollbar->orientation == Qt::Horizontal)
               ret.setRect(sliderstart + sliderlen, 0, maxlen - sliderstart - sliderlen, sbextent);
            else
               ret.setRect(0, sliderstart + sliderlen, sbextent, maxlen - sliderstart - sliderlen);
            break;
         case SC_ScrollBarGroove:
            if (scrollbar->orientation == Qt::Horizontal)
               ret.setRect(0, 0, scrollbar->rect.width() - sbextent * 2, scrollbar->rect.height());
            else
               ret.setRect(0, 0, scrollbar->rect.width(), scrollbar->rect.height() - sbextent * 2);
            break;
         case SC_ScrollBarSlider:
            if (scrollbar->orientation == Qt::Horizontal)
               ret.setRect(sliderstart, 0, sliderlen, sbextent);
            else
               ret.setRect(0, sliderstart, sbextent, sliderlen);
            break;
         default:
            break;
         }
         ret = visualRect(scrollbar->direction, scrollbar->rect, ret);
      }
      break;
//    case CC_Slider: // A slider, like QSlider
//    case CC_ToolButton: // A tool button, like QToolButton
   case CC_TitleBar: // A Title bar, like what is used in Q3Workspace
      if (const QStyleOptionTitleBar *tb =
          qstyleoption_cast<const QStyleOptionTitleBar *>(option))
      {
         const int controlMargin = 0;
         const int controlHeight = tb->rect.height()/* - controlMargin *2*/;
         const int delta = 8*controlHeight/5;
         int offset = 0;
         
         bool isMinimized = tb->titleBarState & Qt::WindowMinimized;
         bool isMaximized = tb->titleBarState & Qt::WindowMaximized;
         
         switch (subControl)
         {
         case SC_TitleBarLabel:
            if (tb->titleBarFlags & (Qt::WindowTitleHint | Qt::WindowSystemMenuHint))
            {
               ret = tb->rect;
               ret.adjust(delta, 0, 0, 0);
               if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
                  ret.adjust(delta, 0, -delta, 0);
               if (tb->titleBarFlags & Qt::WindowMinimizeButtonHint)
                  ret.adjust(delta, 0, 0, 0);
               if (tb->titleBarFlags & Qt::WindowMaximizeButtonHint)
                  ret.adjust(delta, 0, 0, 0);
               if (tb->titleBarFlags & Qt::WindowShadeButtonHint)
                  ret.adjust(0, 0, -delta, 0);
               if (tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
                  ret.adjust(0, 0, -delta, 0);
            }
            break;
         case SC_TitleBarMaxButton:
            if (!isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
               offset += delta;
            else if (subControl == SC_TitleBarMaxButton)
               break;
         case SC_TitleBarMinButton:
            if (!isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
               offset += delta;
            else if (subControl == SC_TitleBarMinButton)
               break;
         case SC_TitleBarNormalButton:
            if (isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
               offset += delta;
            else if (isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
               offset += delta;
            else if (subControl == SC_TitleBarNormalButton)
               break;
         case SC_TitleBarCloseButton:
            if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
               offset += controlMargin;
            else if (subControl == SC_TitleBarCloseButton)
               break;
            ret.setRect(tb->rect.left() + offset, tb->rect.top() + controlMargin, delta, controlHeight);
            break;
            
         case SC_TitleBarContextHelpButton:
            if (tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
               offset += delta;
         case SC_TitleBarShadeButton:
            if (!isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
               offset += delta;
            else if (subControl == SC_TitleBarShadeButton)
               break;
         case SC_TitleBarUnshadeButton:
            if (isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
               offset += delta;
            else if (subControl == SC_TitleBarUnshadeButton)
               break;
         case SC_TitleBarSysMenu:
            if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
               offset += delta + controlMargin;
            else if (subControl == SC_TitleBarSysMenu)
               break;
            ret.setRect(tb->rect.right() - offset, tb->rect.top() + controlMargin, controlHeight, controlHeight);
            break;
         default:
            break;
         }
         ret = visualRect(tb->direction, tb->rect, ret);
      }
      break;
   case CC_Q3ListView: // Used for drawing the Q3ListView class
   case CC_Dial: // A dial, like QDial
   default:
      ret = QCommonStyle::subControlRect ( control, option, subControl, widget);
   }
   return ret;
}

QRect OxygenStyle::subElementRect ( SubElement element, const QStyleOption * option, const QWidget * widget) const
{
   switch (element)
   {
   case SE_PushButtonContents: // Area containing the label (icon with text or pixmap)
      return visualRect(option->direction, option->rect, option->rect.adjusted(4,4,-4,-4));
//    case SE_PushButtonFocusRect: // Area for the focus rect (usually larger than the contents rect)
//    case SE_CheckBoxIndicator: // Area for the state indicator (e.g., check mark)
//    case SE_CheckBoxContents: // Area for the label (text or pixmap)
//    case SE_CheckBoxFocusRect: // Area for the focus indicator
//    case SE_CheckBoxClickRect: // Clickable area, defaults to SE_CheckBoxFocusRect
//    case SE_RadioButtonIndicator: // Area for the state indicator
//    case SE_RadioButtonContents: // Area for the label
//    case SE_RadioButtonFocusRect: // Area for the focus indicator
//    case SE_RadioButtonClickRect: // Clickable area, defaults to SE_RadioButtonFocusRect
//    case SE_ComboBoxFocusRect: // Area for the focus indicator
//    case SE_SliderFocusRect: // Area for the focus indicator
//    case SE_Q3DockWindowHandleRect: // Area for the tear-off handle
   case SE_ProgressBarGroove: // Area for the groove
      return option->rect;
   case SE_ProgressBarContents: // Area for the progress indicator
   case SE_ProgressBarLabel: // Area for the text label
      return option->rect.adjusted(2,2,-2,-2);
//    case SE_DialogButtonAccept: // Area for a dialog's accept button
//    case SE_DialogButtonReject: // Area for a dialog's reject button
//    case SE_DialogButtonApply: // Area for a dialog's apply button
//    case SE_DialogButtonHelp: // Area for a dialog's help button
//    case SE_DialogButtonAll: // Area for a dialog's all button
//    case SE_DialogButtonRetry: // Area for a dialog's retry button
//    case SE_DialogButtonAbort: // Area for a dialog's abort button
//    case SE_DialogButtonIgnore: // Area for a dialog's ignore button
//    case SE_DialogButtonCustom: // Area for a dialog's custom widget area (in the button row)
   case SE_HeaderArrow: //  
   {
      int x,y,w,h;
      option->rect.getRect(&x,&y,&w,&h);
      int margin = 2;// ;) pixelMetric(QStyle::PM_HeaderMargin, opt, widget);
      QRect r;
      if (option->state & State_Horizontal)
         r.setRect(x + w - margin * 2 - (h / 2), y + h/4 + margin, h / 2, h/2 - margin*2);
      else
         r.setRect(x + 5, y, h / 2, h - margin * 2);
      r = visualRect(option->direction, option->rect, r);
      return r;
   }
//    case SE_HeaderLabel: //  
//    case SE_TabWidgetLeftCorner: //  
//    case SE_TabWidgetRightCorner: //  
//    case SE_TabWidgetTabBar: //  
   case SE_TabWidgetTabContents: //  
      if (const QStyleOptionTabWidgetFrame *twf =
          qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option))
      {
         QRect r = subElementRect ( SE_TabWidgetTabPane, option, widget);
         QStyleOptionTab tabopt;
         tabopt.shape = twf->shape;
         int baseHeight = pixelMetric(PM_TabBarBaseHeight, &tabopt, widget);
         switch (twf->shape)
         {
         case QTabBar::RoundedNorth:
         case QTabBar::TriangularNorth:
            r.adjust(2, 2+baseHeight, -2, -2);
            break;
         case QTabBar::RoundedSouth:
         case QTabBar::TriangularSouth:
            r.adjust(2, 2, -2, -2-baseHeight);
            break;
         case QTabBar::RoundedEast:
         case QTabBar::TriangularEast:
            r.adjust(2, 2, -2-baseHeight, -2);
            break;
         case QTabBar::RoundedWest:
         case QTabBar::TriangularWest:
            r.adjust(2+baseHeight, 2, -2, -2);
         }
         return r;
      }
   case SE_TabWidgetTabPane: //  
      return option->rect;
   case SE_ToolBoxTabContents: // Area for a toolbox tab's icon and label
      return QRect(); //kill the rect, we paint the content ourself
//    case SE_ViewItemCheckIndicator: // Area for a view item's check mark
//    case SE_TabBarTearIndicator: // Area for the tear indicator on a tab bar with scroll arrows.
   default:
      return QCommonStyle::subElementRect ( element, option, widget);
   }
}
