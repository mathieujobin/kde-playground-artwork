#include <QWidget>
#include <QSlider>
#include <QStyleOptionTabWidgetFrame>
#include <QTabBar>
#include <QTabWidget>
#include "oxygen.h"

using namespace Oxygen;

int OxygenStyle::pixelMetric ( PixelMetric pm, const QStyleOption * option, const QWidget * widget ) const
{
#if 1
   switch ( pm )
   {
   case PM_ButtonMargin: // Amount of whitespace between push button labels and the frame
      return 4;
   case PM_ButtonDefaultIndicator: // Width of the default-button indicator frame
      return 2;
   case PM_MenuButtonIndicator: // Width of the menu button indicator proportional to the widget height
      return 7;
   case PM_ButtonShiftHorizontal: // Horizontal contents shift of a button when the button is down
   case PM_ButtonShiftVertical: // Vertical contents shift of a button when the button is down
      return 0;
   case PM_DefaultFrameWidth: // Default frame width (usually 2)
      if (widget && widget->inherits("QComboBoxPrivateContainer"))
         return 1;
      else
         return 4;
   case PM_SpinBoxFrameWidth: // Frame width of a spin box, defaults to PM_DefaultFrameWidth
   case PM_ComboBoxFrameWidth: // Frame width of a combo box, defaults to PM_DefaultFrameWidth.
      return 4;
//    case PM_MDIFrameWidth: // Frame width of an MDI window
//    case PM_MDIMinimizedWidth: // Width of a minimized MDI window
   case PM_MaximumDragDistance: // Some feels require the scroll bar or other sliders to jump back to the original position when the mouse pointer is too far away while dragging; a value of -1 disables this behavior
      return -1;
   case PM_ScrollBarExtent: // Width of a vertical scroll bar and the height of a horizontal scroll bar
      return 19;
   case PM_ScrollBarSliderMin: // The minimum height of a vertical scroll bar's slider and the minimum width of a horizontal scroll bar's slider
      return 40;
   case PM_SliderThickness: // Total slider thickness
      return 24;
   case PM_SliderControlThickness: // Thickness of the slider handle
   case PM_SliderLength: // Length of the slider
      return 18;
//    case PM_SliderTickmarkOffset: // The offset between the tickmarks and the slider
   case PM_SliderSpaceAvailable: // The available space for the slider to move
   {
      if (!widget)
         return 0;
      if ( qobject_cast<const QSlider*>(widget) && ((const QSlider*)widget)->orientation() == Qt::Horizontal )
         return (widget->width() - 18);
      else
         return (widget->height() - 18);
   }
   case PM_DockWidgetSeparatorExtent: // Width of a separator in a horizontal dock window and the height of a separator in a vertical dock window
      return 2;
   case PM_DockWidgetHandleExtent: // Width of the handle in a horizontal dock window and the height of the handle in a vertical dock window
      return 6;
   case PM_DockWidgetFrameWidth: // Frame width of a dock window
   case PM_MenuBarPanelWidth: // Frame width of a menubar, defaults to PM_DefaultFrameWidth
      return 1;
//       return 0;
   case PM_MenuBarItemSpacing: // Spacing between menubar items
   case PM_MenuBarHMargin: // Spacing between menubar items and left/right of bar
      return 10;
   case PM_MenuBarVMargin: // Spacing between menubar items and top/bottom of bar
      return 2;
   case PM_ToolBarFrameWidth: // Width of the frame around toolbars
      return 4;
   case PM_ToolBarHandleExtent: // Width of a toolbar handle in a horizontal toolbar and the height of the handle in a vertical toolbar
      return 6;
   case PM_ToolBarItemMargin: // Spacing between the toolbar frame and the items
      return 4;
   case PM_ToolBarItemSpacing: // Spacing between toolbar items
      return 0;
   case PM_ToolBarSeparatorExtent: // Width of a toolbar separator in a horizontal toolbar and the height of a separator in a vertical toolbar
      return 2;
   case PM_ToolBarExtensionExtent: // Width of a toolbar extension button in a horizontal toolbar and the height of the button in a vertical toolbar
      return 16;
   case PM_TabBarTabOverlap: // Number of pixels the tabs should overlap
      return 0;
   case PM_TabBarTabHSpace: // Extra space added to the tab width
      return 18;
   case PM_TabBarTabVSpace: // Extra space added to the tab height
      return 10;
   case PM_TabBarBaseHeight: // Height of the area between the tab bar and the tab pages
   case PM_TabBarBaseOverlap: // Number of pixels the tab bar overlaps the tab bar base
      if (qobject_cast<const QTabWidget*>(widget) && !widget->children().isEmpty())
      {
         QTabBar *tabBar = 0L;
         foreach(QObject *obj, widget->children())
         {
            if (qobject_cast<QTabBar*>(obj))
            {
               tabBar = (QTabBar*)obj;
               break;
            }
         }
         if (!tabBar || !tabBar->isVisible())
            return 0;
         if (const QStyleOptionTabWidgetFrame *twf =
             qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option))
         {
            if (twf->shape == QTabBar::RoundedEast ||
                twf->shape == QTabBar::TriangularEast ||
                twf->shape == QTabBar::RoundedWest ||
                twf->shape == QTabBar::TriangularWest)
               return tabBar->width();
         }
         return tabBar->height();
      }
      return 16;
   case PM_TabBarScrollButtonWidth: //  
      return 16;
   case PM_TabBarTabShiftHorizontal: // Horizontal pixel shift when a tab is selected
   case PM_TabBarTabShiftVertical: // Vertical pixel shift when a tab is selected
      return 0;
//    case PM_ProgressBarChunkWidth: // Width of a chunk in a progress bar indicator
   case PM_SplitterWidth: // Width of a splitter
      return 9;
   case PM_TitleBarHeight: // Height of the title bar
      return 18;
   case PM_IndicatorWidth: // Width of a check box indicator
   case PM_IndicatorHeight: // Height of a checkbox indicator
      return 25;
   case PM_ExclusiveIndicatorWidth: // Width of a radio button indicator
   case PM_ExclusiveIndicatorHeight: // Height of a radio button indicator
      return 17;
   case PM_MenuPanelWidth: // Border width (applied on all sides) for a QMenu
      return 1;
   case PM_MenuHMargin: // Additional border (used on left and right) for a QMenu
      return 2;
   case PM_MenuVMargin: // Additional border (used for bottom and top) for a QMenu
      return 1;
//    case PM_MenuScrollerHeight: // Height of the scroller area in a QMenu
//    case PM_MenuTearoffHeight: // Height of a tear off area in a QMenu
//    case PM_MenuDesktopFrameWidth: //  
//    case PM_CheckListButtonSize: // Area (width/height) of the checkbox/radio button in a Q3CheckListItem
//    case PM_CheckListControllerSize: // Area (width/height) of the controller in a Q3CheckListItem
//    case PM_DialogButtonsSeparator: // Distance between buttons in a dialog buttons widget
//    case PM_DialogButtonsButtonWidth: // Minimum width of a button in a dialog buttons widget
//    case PM_DialogButtonsButtonHeight: // Minimum height of a button in a dialog buttons widget
//    case PM_HeaderMarkSize: //  
//    case PM_HeaderGripMargin: //  
   case PM_HeaderMargin: //  
      return 2;
   case PM_SpinBoxSliderHeight: // The height of the optional spin box slider
      return 4;
//    case PM_DefaultTopLevelMargin: //  
//    case PM_DefaultChildMargin: //  
//    case PM_DefaultLayoutSpacing: //  
   case PM_ToolBarIconSize: // Default tool bar icon size, defaults to PM_SmallIconSize
      return 32;
   case PM_SmallIconSize: // Default small icon size
      return 22;
   case PM_LargeIconSize: // Default large icon size
      return 32;
   case PM_FocusFrameHMargin: // Horizontal margin that the focus frame will outset the widget by.
   case PM_FocusFrameVMargin: // Vertical margin that the focus frame will outset the widget by.
      return 4;
//    case PM_IconViewIconSize: //  
//    case PM_ListViewIconSize: //  
//    case PM_ToolTipLabelFrameWidth: //  
   default:
#endif
      return QCommonStyle::pixelMetric( pm, option, widget );
   } // switch
}
