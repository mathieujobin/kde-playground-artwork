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

#ifndef COKOON_TEST_H
#define COKOON_TEST_H

#include <QObject>

class CokoonTest: public QObject
{
Q_OBJECT
    private slots:
        void evaluate();
        void loadThemeDocument();
        void themeExpressionReferences();
};

#endif
// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
