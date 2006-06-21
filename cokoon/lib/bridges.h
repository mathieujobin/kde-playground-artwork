/* This file is part of the coKoon theme library
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

#ifndef COKOON_BRIDGES_H
#define COKOON_BRIDGES_H

#include <QString>
#include <QVariant>

#include <kdelibs_export.h>

class QPainter;

namespace Cokoon {

/**
 * This class is used as bridge between the theme engine and expressions. It
 * is used to provide values for variables.
 * @sa Expression::evaluate()
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class KDEFX_EXPORT ExpressionVariableBridge
{
    public:
        /**
         * Default constructor.
         */
        ExpressionVariableBridge();
        /**
         * Destruct.
         */
        virtual ~ExpressionVariableBridge();

        /**
         * @returns The value of the variable at index @p idx . Invalid if
         * the variable does not exist.
         */
        virtual QVariant getVariableValue(int idx) const = 0;
};

/**
 * This bridge provides library clients with the ability to to paint special
 * content at a location defined by the theme.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class KDEFX_EXPORT SpecialCellBridge
{
    public:
        /**
         * Default constructor.
         */
        SpecialCellBridge();
        /**
         * Destruct.
         */
        virtual ~SpecialCellBridge();

        /**
         * Paint the special cell @p id at the specified coordinates.
         */
        virtual void paintCell(int id, QPainter *p,
                               int left, int top, int width, int height) const = 0;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
