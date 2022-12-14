/* This file is NOT part of KDE ;)
   Copyright (C) 2007 Thomas L?bking <thomas.luebking@web.de>

   This application is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QApplication>

#include "ui_demo.h"

int main( int argc, char **argv ) {
    QApplication a(argc,argv);
    QDialog *window = new QDialog;
    Ui::Dialog ui;
    ui.setupUi(window);
    window->show();
    return a.exec();
}

