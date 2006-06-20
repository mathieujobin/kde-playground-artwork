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

#ifndef COKOON_TILE_H
#define COKOON_TILE_H

#include <QString>

#include "object_p.h"

class QPainter;

namespace Cokoon {

class Tile;
class TileSource;
class Object;
class ExecContext;

/**
 * A cell in a TileLayout. A Tile specifies the 'view port' within a TileSource
 * to get an pixmap, which is then painted according to the PaintMode.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class Tile
{
    public:
        /**
         * How the tile is painted horizontally/vertically. This is mainly relevant
         * when the size of the layout cell differs from the size of the tile's
         * 'view port'.
         */
        enum PaintMode {
            Normal,    ///< Align the tile top/left.
            Centered,  ///< Align the tile centered.
            Tiled,     ///< Align the tile top/left and repeat to fill the whole area.
            Stretched  ///< Stretch the tile to fit the whole area.
        };

        /**
         * Construct.
         * @param obj The parent object where this Tile plugs into.
         * @param id The id of the object.
         * @param horPaintMode How the tile is painted horizontally.
         * @param vertPaintMode How the tile is painted vertically.
         * @param sourceObj Object where the TileSource is stored. 0 means
         * the TileSource is stored in the same object, and the ExecContext
         * will be the one of the 'calling' object (this or an inherited one).
         * A valid pointer means it is stored in the specified Object; the
         * ExecContext object will always change to it as well.
         * @param sourceId The id of the source.
         * @param sLeft TileExpression of the left of the 'source view port' where the
         * pixmap will be grabbed from.
         * @param sTop TileExpression of the top of the 'source view port'.
         * @param sWidth TileExpression of the width of the 'source view port'.
         * @param sHeight TileExpression of the height of the 'source view port'.
         */
        Tile(Object *obj, int id,
                  PaintMode horPaintMode, PaintMode vertPaintMode,
                  const Object *sourceObj, int sourceId,
                  const TileExpression *sLeft, const TileExpression *sTop,
                  const TileExpression *sWidth, const TileExpression *sHeight);

        /**
         * Destruct.
         */
        ~Tile();

        /**
         * @return The id of this tile
         */
        int id() const;

        /**
         * @return The theme object in which the tile is stored.
         */
        Object *obj() const;

        /**
         * Paint a specific tile within the given area.
         */
        void paint(const ExecContext *c, QPainter *p,
                   int left, int top, int width, int height) const;

        /**
         * @returns The pixmap from the source.
         */
        QPixmap pixmap(const ExecContext *c) const;

    private:
        QVariant eval(const ExecContext *c, const TileExpression *e) const;
        PaintMode m_horPaintMode;
        PaintMode m_vertPaintMode;

        const Object *m_sourceObj;
        int m_sourceId;

        // Area in the source where the tile is generated from
	friend class TileRef;
        const TileExpression *m_sLeft;
        const TileExpression *m_sTop;
        const TileExpression *m_sHeight;
        const TileExpression *m_sWidth;

        int m_id;
        Object *m_obj;
        bool m_valid;

        class TilePrivate * d;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
