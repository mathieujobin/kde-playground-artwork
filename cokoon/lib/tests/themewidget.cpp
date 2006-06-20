/* This file is part of the coKoon test suite
 * Copyright (C) 2006 Sandro Giessl <giessl@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QApplication>
#include <QPainter>

#include "../document.h"

#include "ThemeWidgetSpec.h"

#include "themewidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ThemeWidget w(app.arguments().at(1) );
    w.show();
    return app.exec();
}

ThemeWidget::ThemeWidget(const QString &fileName)
{
    setWindowTitle(tr("Themed widget test..."));
    resize(400,400);

    ThemeWidgetSpec::setupItems(&m_doc);
    m_doc.loadTheme(fileName);
}
void ThemeWidget::paintEvent ( QPaintEvent * event )
{
    QPainter p(this);

    m_doc.drawLayers( ThemeWidgetSpec::Widget, &p, 10, 10, width()-20, height()-20 );
}

#include "themewidget.moc"

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
