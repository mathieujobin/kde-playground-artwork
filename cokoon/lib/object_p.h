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

#ifndef COKOON_THEMEOBJECT_H
#define COKOON_THEMEOBJECT_H

#include <QString>
#include <QPair>
#include <QHash>

#include "bridges.h"
#include "expression_p.h"

class QPainter;

namespace Cokoon {

class Document;
class TileLayout;
class Tile;
class TileSource;
class Object;
class ExecContext;

class CustomExpBase : public Expression
{
public:
    CustomExpBase(const Object *o);

    /**
     * Evaluate the expression aware of execution context @p
     * c. E.g. it is evaluated with the specified variables and
     * references can be resolved in the context of the calling Object.
     */
    QVariant evaluate(const Object *objContext, const ExpressionVariableBridge *vars) const;

    /**
     * This is only set during evaluation in evaluate(), otherwise 0.
     */
    const Object *objContext() const;

    /**
     * When the reference contains an absolute object id, read it an
     * and return the object. Also remove this part from the @p ref
     * string.
     */
    const Object *parseObjectReference( QString &ref ) const;
    /**
     * When the reference contains an absolute object id, read it an
     * and return the object. The relative object identifier ".o" is
     * also allowed and means the parent object @p parentObj . Also
     * remove this part from the @p ref string.
     */
    const Object *parseObjectPropReference( const Object *parentObj, QString &ref ) const;
    /**
     * Creates a suitable reference, relative to object @p o. The reference @ref must be
     * without object reference.
     *
     * Supported references are:
     * @li '.width', '.height': The size of the paint area. When not called
     * during the paint() operation, an invalid value is returned.
     * @li '.[expression id]': Value of an expression (stored in
     * object @p o)
     * @li '.t.[tile name].(left|top|width|height)': 'view port' coordinates
     * of tiles (stored in object @p o).
     */
    ExpressionReference *createObjectReference( const Object *o, const QString &ref ) const;
protected:
    const Object * const obj;

private:
    mutable const Object *objCtxt;
};

class ObjectExpression : public CustomExpBase
{
public:
    ObjectExpression(const Object *o,
		     const QString &expr,
		     const ExpressionVariableIndexFactory *varIdxs = 0);

    virtual ExpressionReference *getReference(const QString &ref) const;
};

class ObjectPropExpression : public CustomExpBase
{
public:
    ObjectPropExpression(const Object *o,
			 const QString &expr,
			 const ExpressionVariableIndexFactory *varIdxs = 0);

    virtual ExpressionReference *getReference(const QString &ref) const;
};

class TileExpression : public CustomExpBase
{
public:
    TileExpression(const Object *o,
		   int tileId,
		   const QString &expr,
		   const ExpressionVariableIndexFactory *varIdxs = 0);

    virtual ExpressionReference *getReference(const QString &ref) const;

    static ExpressionReference *tileReference( const CustomExpBase *e,const Object *o,
					       int tileId, const QString &ref );

protected:
    const int tileId;
};


/**
 * A reference pointing to variables stored in Object.
 */
class ObjectRef : public ExpressionReference
{
public:
    enum ObjectVars {
	ObjectWidth,
	ObjectHeight
    };

    ObjectRef(const CustomExpBase *e, const Object *o, ObjectVars v);

    virtual QString ref() const;
    virtual QVariant getReferenceValue(const ExpressionVariableBridge *vars) const;
protected:
    const Object * const obj;
    const ObjectVars var;

    const CustomExpBase * const exp;
};

/**
 * A reference pointing to expressions stored in Object.
 */
class ObjectExpressionRef : public ExpressionReference
{
public:
    ObjectExpressionRef(const CustomExpBase *e, const Object *o, int expressionId);

    virtual QString ref() const;
    virtual QVariant getReferenceValue(const ExpressionVariableBridge *vars) const;
protected:
    const Object * const obj;
    const int expId;

    const CustomExpBase * const exp;
};

/**
 * A reference pointing to the left/top/widht/height expressions in Tile.
 */
class TileRef : public ExpressionReference
{
public:
    enum TileVars {
	TileLeft,
	TileTop,
	TileWidth,
	TileHeight
    };

    TileRef(const CustomExpBase *e, const Object *o, int tileId, TileVars v);

    virtual QString ref() const;
    virtual QVariant getReferenceValue(const ExpressionVariableBridge *vars) const;
protected:
    const Object * const obj;
    const int tileId;
    const TileVars var;

    const CustomExpBase * const exp;
};


/**
 * Store properties such as expressions, layouts, tiles, tile sources,
 * paint layers.
 * These properties can be inherited by another object; this happens
 * transparently when using the access methods.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class Object
{
    public:
        /**
         * Construct.
         */
        Object(Document *doc, const QString &inherit = QString() );
        /**
         * Destruct.
         */
        ~Object();

        /**
         * @return The inherited object.
         */
        const Object *inherit() const;

        /**
         * @return The theme document in which the object is stored.
         */
        Document *doc() const;

        /**
         * @return true if everything with this object is ok or false if there
         * was some problem initializing it (e.g. inherited object not existing)
         */
        bool isValid() const;

        /**
         * Add reference to a layout which defines how this object is painted.
         * @param layoutObj  The object in which the layout is stored. Explanation
         * of Tile::Tile() @p sourceObj does apply for this argument as well.
         * @param layoutId The id of the referenced layout
         */
        void addPaintLayer(const Object *layoutObj, int layoutId);

        /**
         * Paint all 'paint layers'.
         * @sa TileLayout::paint()
         */
        void paint(QPainter *p,
                   int left, int top, int width, int height,
                   const ExpressionVariableBridge *vars = 0,
                   const SpecialCellBridge *cells = 0) const;

        /**
         * Store expression @p e (identified by @p id ) in this object.
         */
        void addExpression(int id, const ObjectExpression *e);

        /**
         * Clear the stored items: paint layer references, expressions, tiles,
         * tile sources, layouts.
         */
        void clear();

        /**
         * Property can be inherited by another object.
         * Make sure you provide the expression with a suitable expression reference
         * bridge (like Object).
         * @return The expression identified by @p id .
         * @sa expressionReferenceBridge()
         */
        const ObjectExpression *expression(int id) const;
        /**
         * Property can be inherited by another object.
         * @return The tile identified by @p id .
         */
        const Tile *tile(int id) const;
        /**
         * Property can be inherited by another object.
         * @return The tile source identified by @p id .
         */
        const TileSource *tileSource(int id) const;
        /**
         * Property can be inherited by another object.
         * @return The layout identified by @p id .
         */
        const TileLayout *layout(int id) const;

        /**
         * Preferred way to evaluate a expression. This method automatically
         * provides the expression with a suitable expression reference bridge
         * (Object, i.e. "this").
         * @param id The id of the expression.
         * @param vars Used to provide the expression with variable values.
         * @return The evaluation of the expression.
         * @sa expression()
         */
        QVariant evalExpression(int id,
                                const ExpressionVariableBridge *vars = 0) const;

        /**
         * One object can be referenced by different ids in the document.
         * @returns The number of document references to this object.
         */
        int docRefCount() const;
    private:
        friend class Document;
        friend class TileLayout;
        friend class Tile;
        friend class TileSource;

        /**
         * Stores the layout @p layout and takes over memory management.
         */
        void insertLayout(TileLayout *layout);
        /**
         * Stores the tile @p tile and takes over memory management.
         */
        void insertTile(Tile *tile);
        /**
         * Stores the tile source @p source and takes over memory management.
         */
        void insertTileSource(TileSource *source);

        // can inherit another object.
        const Object *m_inherit;

        // if false
        bool m_paintLayersSet;
        // these are the layouts which paint the object. can have no paintable
        // layer at all.
        QList<QPair<const Object*, int> > m_paintLayers;

        // Objects can store these properties.
        QHash<int, const ObjectExpression*> m_expressions;
        QHash<int, Tile*> m_tiles;
        QHash<int, TileSource*> m_sources;
        QHash<int, TileLayout*> m_layouts;

        Document *m_doc;

        bool m_valid;

        int m_docRefCount;

        // set while painting, to provide .width and .height references.
        // make the ref bridge friend to allow access...
	friend class ObjectRef;
        mutable QVariant m_paintWidth;
        mutable QVariant m_paintHeight;

        class ObjectPrivate * d;
};

/**
 * This class is used to track in which context an object property (Tile,
 * TileLayout, TileSource etc.) is executed in, to get object inheritance
 * right.
 * In these classes, it usually is the first parameter to methods which
 * are executing an Expression, or which need access to other properties.
 */
class ExecContext
{
    public:
        /**
         * Construct.
         * @param obj The object which is the 'caller'.
         * @param vars Variables which might be used in expressions.
         * @param cells Bridge to specially painted cell content...
         */
        ExecContext(const Object *obj, const ExpressionVariableBridge *vars,
                   const SpecialCellBridge *cells);
        const Object *obj() const;
        const ExpressionVariableBridge *vars() const;
        const SpecialCellBridge *cells() const;
    private:
        const Object *m_obj;
        const ExpressionVariableBridge *m_vars;
        const SpecialCellBridge *m_cells;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
