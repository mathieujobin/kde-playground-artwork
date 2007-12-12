/* This file is NOT part of KDE ;)
   Copyright (C) 2007 Thomas Lübking <thomas.luebking@web.de>

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

#include <QtGui/QScrollBar>
#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>

#include "ui_uidemo.h"

int main(int argc, char **argv)
{
    KAboutData about("uidemo", 0, ki18n("uidemo"), "0.1",
                     ki18n("UI demo application"),
                     KAboutData::License_GPL, ki18n("Copyright 2007 Thomas Lübking"));
    about.addAuthor( ki18n("Thomas Lübking"), KLocalizedString(), "thomas.luebking@web.de" );
    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;

    QMainWindow *window = new QMainWindow;
    Ui::MainWindow ui;
    ui.setupUi(window);
    ui.label_13->setBackgroundRole(QPalette::Base);
    ui.label_14->setBackgroundRole(QPalette::Base);
    ui.label_15->setBackgroundRole(QPalette::Base);
    ui.label_16->setBackgroundRole(QPalette::Base);
    QActionGroup *ag = new QActionGroup(window);
    ag->addAction(ui.actionIcons);
    ag->addAction(ui.actionList);
    ag->addAction(ui.actionDetails);
    ui.actionIcons->setChecked(true);
    window->show();

    return app.exec();
}
