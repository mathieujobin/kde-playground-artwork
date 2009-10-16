/***************************************************************************
 *   Kannasaver - A Screensaver for KDE showin Japanese characters         *
 *   Copyright (C) 2009  Frederik Schwarzer <schwarzerf@gmail.com>         *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

/**
 * @file setupdlg.h
 * @author Frederik Schwarzer <schwarzerf@gmail.com>
 * @brief Class declaration for the setup dialog
 */

#ifndef SETUPDLG_H
#define SETUPDLG_H

#include <KDialog>

class SetupDlgWidget;

/**
 * @brief A dialog to setup the screensaver
 */
class SetupDlg : public KDialog
{
   Q_OBJECT
   public:
      explicit SetupDlg( QWidget *parent = 0 );
      ~SetupDlg();
   private:
      void init();

      SetupDlgWidget *ui;
};

#endif

// vim: expandtab:tabstop=3:shiftwidth=3
// kate: space-indent on; indent-width 3