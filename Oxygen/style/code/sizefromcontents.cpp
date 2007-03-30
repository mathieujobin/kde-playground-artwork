/***************************************************************************
 *   Copyright (C) 2006-2007 by Thomas L�bking                             *
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

#include <QStyleOptionComboBox>
#include <QStyleOptionMenuItem>
#include "oxygen.h"

using namespace Oxygen;
extern Dpi dpi;

extern Config config;
static const int windowsArrowHMargin = 6; // arrow horizontal margin

QSize OxygenStyle::sizeFromContents ( ContentsType ct, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget ) const
{
   switch ( ct )
   {
//    case CT_CheckBox: // A check box, like QCheckBox
   case CT_ComboBox: // A combo box, like QComboBox
      if (const QStyleOptionComboBox *cb =
          qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
         int margin = cb->frame ? dpi.$2 : 0;
         int hgt = contentsSize.height() + 2*margin;
         margin = cb->editable ? dpi.$10 : dpi.$8;
         return QSize(contentsSize.width()+margin+(int)(hgt/1.1), hgt);
      }
//    case CT_DialogButtons: //
//       return QSize((contentsSize.width()+16 < 80) ? 80 : contentsSize.width()+16, contentsSize.height()+10);
//    case CT_Q3DockWindow: //  
   case CT_HeaderSection: // A header section, like QHeader
      if (const QStyleOptionHeader *hdr =
          qstyleoption_cast<const QStyleOptionHeader *>(option))
      {
         QSize sz;
         int margin = dpi.$2;
         int iconSize = hdr->icon.isNull() ? 0 : pixelMetric(QStyle::PM_SmallIconSize, hdr, widget);
         QSize txt = hdr->fontMetrics.size(0, hdr->text);
         sz.setHeight(qMax(iconSize, txt.height()) + dpi.$4);
         sz.setWidth((iconSize?margin+iconSize:0) +
                     (hdr->text.isNull()?0:margin+txt.width()) +
                     ((hdr->sortIndicator == QStyleOptionHeader::None)?0:margin+8*option->rect.height()/5) +
                     margin);
         return sz;
      }
   case CT_LineEdit: // A line edit, like QLineEdit
      return contentsSize + QSize(dpi.$4,dpi.$5);
   case CT_MenuBarItem: // A menu bar item, like the buttons in a QMenuBar
      return QSize(qMax(contentsSize.width()+dpi.$18, (contentsSize.height()+dpi.$4)*8/5), contentsSize.height()+dpi.$6);
   case CT_MenuItem: // A menu item, like QMenuItem
      if (const QStyleOptionMenuItem *menuItem =
          qstyleoption_cast<const QStyleOptionMenuItem *>(option))
      {
         bool checkable = menuItem->menuHasCheckableItems;
         int maxpmw = menuItem->maxIconWidth;
         int w = contentsSize.width(), h = contentsSize.height();
         QSize sz;
         if (menuItem->menuItemType == QStyleOptionMenuItem::Separator)
            sz = QSize (10, menuItem->text.isEmpty() ? 6 : menuItem->fontMetrics.lineSpacing());
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
   case CT_PushButton: // A push button, like QPushButton
      if (const QStyleOptionButton *btn =
          qstyleoption_cast<const QStyleOptionButton *>(option)) {
         if (btn->text.isEmpty())
//             3px for shadow & outline + 1px padding -> 4px per side
            return ( QSize( contentsSize.width() + dpi.$8, contentsSize.height() + dpi.$8 ) );
         else {
            QSize sz = contentsSize;
            return QSize((sz.width()+dpi.$20 < dpi.$80) ? dpi.$80 : contentsSize.width()+dpi.$20,
                         contentsSize.height()+dpi.$6);
         }
      }
//    case CT_RadioButton: // A radio button, like QRadioButton
//    case CT_SizeGrip: // A size grip, like QSizeGrip

   case CT_Menu: // A menu, like QMenu
   case CT_Q3Header: // A Qt 3 header section, like Q3Header
   case CT_MenuBar: // A menu bar, like QMenuBar
   case CT_ProgressBar: // A progress bar, like QProgressBar
   case CT_Slider: // A slider, like QSlider
   case CT_ScrollBar: // A scroll bar, like QScrollBar
   case CT_SpinBox: // A spin box, like QSpinBox
      return contentsSize;
//    case CT_Splitter: // A splitter, like QSplitter
   case CT_TabBarTab: // A tab on a tab bar, like QTabBar
      return contentsSize + QSize(dpi.$12,0);
   case CT_TabWidget: // A tab widget, like QTabWidget
      return contentsSize + QSize(dpi.$8,dpi.$10);
   case CT_ToolButton: // A tool button, like QToolButton
   {
      // get ~goldem mean ratio
      int w = qMax(contentsSize.width()+dpi.$6, (contentsSize.height()+dpi.$5)*7/5);
      if (qstyleoption_cast<const QStyleOptionComplex *>(option) &&
          (static_cast<const QStyleOptionComplex *>(option)->subControls & SC_ToolButtonMenu))
         w += pixelMetric(PM_MenuButtonIndicator, option, widget) + dpi.$8;
      return QSize(w, contentsSize.height()+dpi.$5);
   }
   default: ;
   } // switch
   return QCommonStyle::sizeFromContents( ct, option, contentsSize, widget );
}
