#include <QStyleOptionComboBox>
#include <QStyleOptionMenuItem>
#include "oxygen.h"

using namespace Oxygen;

extern Config config;
static const int windowsArrowHMargin = 6; // arrow horizontal margin

QSize OxygenStyle::sizeFromContents ( ContentsType ct, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget ) const
{
   switch ( ct )
   {
//    case CT_CheckBox: // A check box, like QCheckBox
   case CT_ComboBox: // A combo box, like QComboBox
      if (const QStyleOptionComboBox *cb =
          qstyleoption_cast<const QStyleOptionComboBox *>(option))
      {
         int hgt = contentsSize.height() + (cb->frame ? 8 : 0);
         return QSize(contentsSize.width()+(int)(hgt*1.4), hgt);
      }
//    case CT_DialogButtons: //
//       return QSize((contentsSize.width()+16 < 80) ? 80 : contentsSize.width()+16, contentsSize.height()+10);
//    case CT_Q3DockWindow: //  
//    case CT_HeaderSection: // A header section, like QHeader
//    case CT_LineEdit: // A line edit, like QLineEdit
//    case CT_Menu: // A menu, like QMenu
//    case CT_Q3Header: // A Qt 3 header section, like Q3Header
//    case CT_MenuBar: // A menu bar, like QMenuBar
   case CT_MenuBarItem: // A menu bar item, like the buttons in a QMenuBar
      return QSize(qMax(contentsSize.width()+12, (contentsSize.height()+2)*8/5), contentsSize.height()+2);
   case CT_MenuItem: // A menu item, like QMenuItem
      if (const QStyleOptionMenuItem *menuItem =
          qstyleoption_cast<const QStyleOptionMenuItem *>(option))
      {
         bool checkable = menuItem->menuHasCheckableItems;
         int maxpmw = menuItem->maxIconWidth;
         int w = contentsSize.width(), h = contentsSize.height();
         QSize sz;
         if (menuItem->menuItemType == QStyleOptionMenuItem::Separator)
            sz = QSize (10, menuItem->text.isEmpty() ? 2 : menuItem->fontMetrics.lineSpacing());
         else
         {
            h = qMax(h, menuItem->fontMetrics.lineSpacing());
            if (!menuItem->icon.isNull())
               h = qMax(h, menuItem->icon.pixmap(pixelMetric(PM_SmallIconSize), QIcon::Normal).height() + 4);
         
            if (menuItem->text.contains('\t'))
               w += 12;
            if (maxpmw > 0)
               w += maxpmw + 6;
            if (checkable && maxpmw < 20)
               w += 20 - maxpmw;
            if (checkable || maxpmw > 0)
               w += 2;
            w += 12;
            if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu)
               w += 2 * windowsArrowHMargin;
            if (menuItem->menuItemType == QStyleOptionMenuItem::DefaultItem)
            {
                // adjust the font and add the difference in size.
                // it would be better if the font could be adjusted in the getStyleOptions qmenu func!!
               QFontMetrics fm(menuItem->font);
               QFont fontBold = menuItem->font;
               fontBold.setBold(true);
               QFontMetrics fmBold(fontBold);
               w += fmBold.width(menuItem->text) - fm.width(menuItem->text);
            }
            sz = QSize(w, h);
         }
         return sz;
      }
      break;
//    case CT_ProgressBar: // A progress bar, like QProgressBar
   case CT_PushButton: // A push button, like QPushButton
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option))
      {
         if (btn->text.isEmpty())
//             4px for the frame + 2px padding -> 6px per side
            return ( QSize( contentsSize.width() + 12, contentsSize.height() + 12 ) );
         else
         {
            int heightAdd = config.roundButtons ? (config.raisedButtons ? 3 : 5) : 7;
            return QSize((contentsSize.width()+16 < 80) ? 80 : contentsSize.width()+16, contentsSize.height()+heightAdd);
         }
      }
//    case CT_RadioButton: // A radio button, like QRadioButton
//    case CT_SizeGrip: // A size grip, like QSizeGrip
//    case CT_Slider: // A slider, like QSlider
//    case CT_ScrollBar: // A scroll bar, like QScrollBar
//    case CT_SpinBox: // A spin box, like QSpinBox
//    case CT_Splitter: // A splitter, like QSplitter
//    case CT_TabBarTab: // A tab on a tab bar, like QTabBar
//    case CT_TabWidget: // A tab widget, like QTabWidget
   case CT_ToolButton: // A tool button, like QToolButton
      // get ~goldem mean ratio
      return QSize(qMax(contentsSize.width()+6, (contentsSize.height()+5)*8/5), contentsSize.height()+5);
   default: ;
   } // switch
   return QCommonStyle::sizeFromContents( ct, option, contentsSize, widget );
}
