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

#ifndef COKOON_TILESOURCE_H
#define COKOON_TILESOURCE_H

#include <QBitmap>
#include <QPixmap>
#include <QList>
#include <QtSvg/QSvgRenderer>
class QPalette;

#include "expression_p.h"
#include "object_p.h"

namespace Cokoon {

class Object;

/**
 * Serves Tile with pixmaps.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class TileSource
{
    public:
        /**
         * Construct. Must be created on the heap using new!
         * The theme object @p obj needs to be specified. It will
         * then automatically store the object and take over memory management
         * etc.
         * Access the tile source using Object::tileSource()
         */
        TileSource(Object *obj, int id);
        /**
         * Destruct.
         */
        virtual ~TileSource();

        /**
         * @return The id of this tile source
         */
        int id() const;

        /**
         * @return The theme object in which the tile source is stored.
         */
        Object *obj() const;

        /**
         * @param c Access to variables, and the 'calling' object.
         * @param left Left coordinate of the target area.
         * @param top Top coordinate of the target area.
         * @param width Width of the target area.
         * @param height Height of the target area.
         * @param scaleWidth The actual width of the resulting pixmap. The
         * 'view port' will be scaled to fit it.
         * @param scaleHeight The actual height of the resulting pixmap. The
         * 'view port' will be scaled to fit it.
         * @param vars Gives expressions in the tile source access to variables.
         * @return A section of the source pixmap
         */
        virtual QPixmap pixmap(const ExecContext *c, int left, int top, int width, int height,
                               int scaleWidth, int scaleHeight) const = 0;

    private:

        int m_id;
        Object *m_obj;
        bool m_valid;

        class TileSourcePrivate * d;
};

class SvgSourceReplaceData;
/**
 * A TileSource for the scalable vector graphics format. This is especially
 * nice since stretching happens without loss of quality.
 *
 * In order to respect color schemes (or doing advanced things like
 * modifying the opacity of elements, etc.), SVK style attributes can
 * be replaced with expression values.
 *
 * TODO: This is based on Qt 4.1's QSvgRenderer. As soon as the animation
 * API is actually implemented, I need to look into coKoon animation integration...
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class SvgTileSource : public TileSource
{
    public:
        SvgTileSource(Object *obj, int id, const QString &fileName);
        virtual ~SvgTileSource();

        /**
         * Specifies an attribute of the 'style' xml attribute, which should
         * be replaced by @p expression .
         * @param idAttr The attribute 'id' of the element in which the replacement should happen.
         * @param styleAttr Within 'style' the attribute which's value should be replaced.
         * @param expression The replacement value.
         */
        void addStyleReplacementExpression(const QString &idAttr,
                                           const QString &styleAttr,
                                           const ObjectPropExpression *expression);

        /**
         * Inherit abstract method.
         */
        virtual QPixmap pixmap(const ExecContext *c, int left, int top, int width, int height,
                               int scaleWidth, int scaleHeight) const;

    private:
        mutable QByteArray m_svgData;
        mutable QList<SvgSourceReplaceData*> m_replaceAttrs;
        mutable QSvgRenderer *m_svg;

        const QString m_fileName;

        class SvgTileSourcePrivate * d;
};

/**
 * A TileSource for pixmap graphics.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class PixmapTileSource : public TileSource
{
    public:
        PixmapTileSource(Object *obj, int id, const QString &fileName);

        /**
         * Implement abstract method.
         */
        virtual QPixmap pixmap(const ExecContext *c, int left, int top, int width, int height,
                               int scaleWidth, int scaleHeight) const;

    private:
        QPixmap m_pm;

        class PixmapTileSourcePrivate * d;
};

/**
 * A TileSource for bitmap graphics.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class BitmapTileSource : public TileSource
{
    public:
        BitmapTileSource(Object *obj, int id, const QString &fileName);

        /**
         * Implement abstract method.
         */
        virtual QPixmap pixmap(const ExecContext *c, int left, int top, int width, int height,
                               int scaleWidth, int scaleHeight) const;

    private:
        QBitmap m_bm;

        class BitmapTileSourcePrivate * d;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
