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

#ifndef COKOON_TILELAYOUT_H
#define COKOON_TILELAYOUT_H

#include <QString>
#include <QList>
#include <QPair>

#include "expression_p.h"
#include "object_p.h"

class QPainter;

namespace Cokoon {

class ObjectPropExpression;
class Object;
class LayoutCellContent;

/**
 * A layout consisting of columns and rows. Columns/rows can be fixed or
 * expanding in size (see LayoutBehavior).
 *
 * E.g. a button with a fixed button border could have the following layout:
 * @verbatim
 _____________________________________
| f |      h                      | f | f: hor.+vert. fixed
|___|_____________________________|___| h: hor. expanding
| v |      h + v                  | v | v: vert. expanding
|___|_____________________________|___|
| f |      h                      | f |
|___|_____________________________|___|@endverbatim
 * Pseudo code:
 * @code
 * addCol(Fixed, 5);
 * addCol(Expanding);
 * addCol(Fixed, 5);
 * addRow(Fixed, 5);
 * addTileCell( ... );
 * addTileCell( ... );
 * addTileCell( ... );
 * addRow(Expanding);
 * ...
 * addRow(Fixed, 5);
 * ...
 * @endcode
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class TileLayout
{
    public:
        /**
         * How a row/cell should affect the layout.
         */
        enum LayoutBehavior {
            Fixed,      ///< Size is fixed.
            Expanding   ///< Fills the remaining space.
        };

        /**
         * Construct. Must be created on the heap using new!
         * The theme object @p obj needs to be specified. It will
         * then automatically store the object and take over memory management
         * etc.
         * Access the layout using Object::layout()
         */
        TileLayout(Object *obj, int id);
        /**
         * Destruct.
         */
        ~TileLayout();

        /**
         * @return The id of this layout
         */
        int id() const;

        /**
         * @return The theme object in which the layout is stored.
         */
        Object *obj() const;

        /**
         * Add a new layout column to the layout and specify how it has to
         * behave in the layout. This method needs to be called
         * before addRow() and addCell() !
         * @param b The layout behavior of this row.
         * @param fixedSize If layout behavior is set to Fixed, the size has to
         * be specified as well.
         */
        void addCol(LayoutBehavior b, ObjectPropExpression *fixedSize = 0);
        /**
         * Add a new layout row to the layout and specify how it has to
         * behave in the layout. This method needs to be called after
         * addCol(), before addCell() !
         * @param b The layout behavior of this row.
         * @param fixedSize If layout behavior is set to Fixed, the size has to
         * be specified as well.
         */
        void addRow(LayoutBehavior b, ObjectPropExpression *fixedSize = 0);
        /**
         * Add a Tile cell to the current layout row. This method needs
         * to be called after addRow() (and thus after addCol() ).
         * @param tileObj  The object in which the layout is stored. Explanation
         * of Tile::Tile() @p sourceObj does apply for this argument as well.
         * @param tileId The id of the tile. An empty string means that it is
         * an empty cell which won't be painted.
         */
        void addTileCell(const Object *tileObj, int tileId);
        /**
         * Add an empty cell to the current layout row.
         */
        void addEmptyCell();
        /**
         * Add a special cell to the current layout row. By providing
         * a proper SpecialCellBridge, this allows for cells which are painted
         * by the library client. This special cell is identified by @p id .
         */
        void addSpecialCell(int id);

        /**
         * Clear the layout.
         */
        void clear();

        /**
         * Paint the layout.
         * @param p The painter to use.
         * @param left Left of the area in which the layer should be layouted to.
         * @param top Top of the area in which the layer should be layouted to.
         * @param width Width of the area in which the layer layout should fit into.
         * @param height Height of the area in which the layer layout should fit into.
         * @param vars Expression variables made accessible for tiles, tile source etc.
         */
        void paint(const ExecContext *c, QPainter *p,
                   int left, int top, int width, int height) const;

    private:
        bool cellInsertCheck();

        void calcCellSizes(const ExecContext *c, int availableSize, QList<QPair<LayoutBehavior, ObjectPropExpression*> > cells, QList<int> &sizes) const;
        QList<QPair<LayoutBehavior, ObjectPropExpression*> > m_cols;
        QList<QPair<LayoutBehavior, ObjectPropExpression*> > m_rows;
        QList<QList<LayoutCellContent* > > m_grid;

        int m_id;
        Object *m_obj;
        bool m_valid;

        class TileLayoutPrivate * d;
};


}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
