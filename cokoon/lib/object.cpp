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

#include <QDebug>

#include "expression_p.h"
#include "tile_p.h"
#include "tilesource_p.h"
#include "tilelayout_p.h"
#include "document.h"
#include "object_p.h"

namespace Cokoon {

    CustomExpBase::CustomExpBase( const Object *o)
        : obj( o ), objCtxt( 0 )
    {
    }

    const Object *CustomExpBase::objContext() const
    {
        return objCtxt;
    }

    QVariant CustomExpBase::evaluate(const Object *objContext, const ExpressionVariableBridge *vars) const
    {
        objCtxt = objContext;

        QVariant v = Expression::evaluate( vars );

        objCtxt = 0;

        return v;
    }

    const Object *CustomExpBase::parseObjectReference( QString &ref ) const
    {
        if ( !obj || !obj->doc() )
            return 0;

        if ( !ref.startsWith( '.' ) ) {
            QString itemIdStr = ref.section( '.', 0, 0 );
            int itemId = obj->doc()->declarationItemId( itemIdStr );

            if ( itemId == -1 )
                return 0;

            int itemStateLevels = obj->doc()->declarationItemStateLevels( itemId );

            if ( itemStateLevels < 0 )
                return 0;

            QString objectIdStr = ref.section( '.', 0, itemStateLevels );

            const Object *o = obj->doc()->obj( objectIdStr );

            if ( !o )
                return 0;

            ref = "." + ref.section( '.', itemStateLevels+1 );
        }

        return 0;
    }

    const Object *CustomExpBase::parseObjectPropReference( const Object *parentObj, QString &ref ) const
    {
        if ( ref.startsWith( ".o." ) ) {
            ref = ref.right( ref.length()-2 );
            return parentObj;
        } else {
            return parseObjectReference( ref );
        }
    }

    ExpressionReference *CustomExpBase::createObjectReference( const Object *o, const QString &ref ) const
    {
        if ( !obj )
            return 0;

        if ( ref.startsWith( ".t." ) ) {

            QString tileIdStr = ref.section( '.', 2, 2 );
            int tileId = obj->doc()->getIdentifierIndex(tileIdStr);

            const Tile *t = obj->tile( tileId );
            if (t) {
                QString tileExpression = ref.section( '.', 3, 3 );

                ExpressionReference *tileRef = TileExpression::tileReference( this, o, tileId, tileExpression );
                if ( tileRef )
                    return tileRef;
            }

        } else if ( ref == ".width" ) {
            return new ObjectRef( this, o, ObjectRef::ObjectWidth );
        } else if ( ref == ".height" ) {
            return new ObjectRef( this, o, ObjectRef::ObjectHeight );
        } else  {

            QString expressionIdStr = ref.section( '.', 1, 1 );
            int expressionId = obj->doc()->getIdentifierIndex( expressionIdStr );

            const Expression *e = obj->expression( expressionId );
            if ( e ) {
                return new ObjectExpressionRef( this, o, expressionId );
            }
        }

        return 0;
    }

    ObjectExpression::ObjectExpression(const Object *o,
                                       const QString &expr,
                                       const ExpressionVariableIndexFactory *varIdxs)
        : CustomExpBase( o )
    {
        parse( expr, varIdxs );
    }

    ExpressionReference *ObjectExpression::getReference(const QString &r) const
    {
        if ( !obj )
            return 0;

        QString ref = r;

        const Object *o = parseObjectReference( ref );

        return createObjectReference( o, ref );
    }

    ObjectPropExpression::ObjectPropExpression(const Object *o,
                                               const QString &expr,
                                               const ExpressionVariableIndexFactory *varIdxs)
        : CustomExpBase( o )
    {
        parse( expr, varIdxs );
    }

    ExpressionReference *ObjectPropExpression::getReference(const QString &r) const
    {
        if ( !obj )
            return 0;

        QString ref = r;

        const Object *o = parseObjectPropReference( obj, ref );

        return createObjectReference( o, ref );
    }

    TileExpression::TileExpression(const Object *o, int tId,
                                   const QString &expr,
                                   const ExpressionVariableIndexFactory *varIdxs)
        : CustomExpBase( o ), tileId( tId )
    {
        parse( expr, varIdxs );
    }

    ExpressionReference *TileExpression::getReference(const QString &r) const
    {
        if ( !obj )
            return 0;

        // enable references ".left",".top",".width",".height" --
        // without any object reference, means references happen
        // within one tile...
        ExpressionReference *interTileRef = tileReference( this, obj, tileId, r.right( r.length()-1 ) );
        if ( interTileRef )
            return interTileRef;


        QString ref = r;

        const Object *o = parseObjectPropReference( obj, ref );

        return createObjectReference( o, ref );
    }

    ExpressionReference *TileExpression::tileReference( const CustomExpBase *e, const Object *o, int tileId, const QString &ref )
    {
        if ( ref == "left" ) {
            return new TileRef( e, o, tileId, TileRef::TileLeft );
        } else if ( ref == "top" ) {
            return new TileRef( e, o, tileId, TileRef::TileTop );
        } else if ( ref == "width" ) {
            return new TileRef( e, o, tileId, TileRef::TileWidth );
        } else if ( ref == "height" ) {
            return new TileRef( e, o, tileId, TileRef::TileHeight );
        }

        return 0;
    }


    ObjectRef::ObjectRef(const CustomExpBase *e, const Object *o, ObjectVars v)
        : obj( o ), var( v ), exp( e )
    {
    }

    QString ObjectRef::ref() const
    {
        return QString( "ObjectRef(%1)" ).arg( var );
    }

    QVariant ObjectRef::getReferenceValue(const ExpressionVariableBridge *) const
    {
        if ( !exp )
            return QVariant();

        const Object *o = 0;
        if ( obj ) {
            o = obj;
        } else {
            o = exp->objContext();
        }

        if ( !o )
            return QVariant();

        switch( var ) {
        case ObjectWidth:
            return o->m_paintWidth;
        case ObjectHeight:
            return o->m_paintHeight;
        }

        return QVariant();
    }

    ObjectExpressionRef::ObjectExpressionRef(const CustomExpBase *e, const Object *o, int expressionId)
        : obj( o ), expId( expressionId ), exp( e )
    {
    }

    QString ObjectExpressionRef::ref() const
    {
        return QString( "ObjectExpressionRef(%1)" ).arg( expId );
    }

    QVariant ObjectExpressionRef::getReferenceValue(const ExpressionVariableBridge *vars) const
    {
        if ( !exp )
            return QVariant();

        const Object *o = 0;
        if ( obj ) {
            o = obj;
        } else {
            o = exp->objContext();
        }

        if ( !o )
            return QVariant();

        return o->evalExpression( expId, vars );
    }

    TileRef::TileRef(const CustomExpBase *e, const Object *o, int tileId, TileVars v)
        : obj( o ), tileId( tileId ), var( v ), exp( e )
    {
    }

    QString TileRef::ref() const
    {
        return QString( "TileRef(%1,%2)" ).arg( tileId ).arg( var );
    }

    QVariant TileRef::getReferenceValue(const ExpressionVariableBridge *vars) const
    {
        if ( !exp )
            return QVariant();

        const Object *o = 0;
        if ( obj ) {
            o = obj;
        } else {
            o = exp->objContext();
        }

        if ( !o )
            return QVariant();

        const Tile *t = o->tile( tileId );

        switch( var ) {
        case TileLeft:
            if ( t->m_sLeft )
                return t->m_sLeft->evaluate( o, vars );
            break;
        case TileTop:
            if ( t->m_sTop )
                return t->m_sTop->evaluate( o, vars );
            break;
        case TileWidth:
            if ( t->m_sWidth )
                return t->m_sWidth->evaluate( o, vars );
            break;
        case TileHeight:
            if ( t->m_sHeight )
                return t->m_sHeight->evaluate( o, vars );
            break;
        }

        return QVariant();
    }


QVariant Object::evalExpression(int id,
                        const ExpressionVariableBridge *vars) const
{
//     qDebug() << "eval\t" << id;
    const ObjectExpression *e = expression(id);
    if (e) {
        return e->evaluate(this, vars);
    } else {
        return QVariant();
    }
}

Object::Object(Document *doc, const QString &inherit)
    : m_inherit(0), m_paintLayersSet(false), m_doc(doc), m_valid(false),
      m_docRefCount(0), m_paintWidth(QVariant()), m_paintHeight(QVariant())
{
    if (!m_doc) {
        qFatal("Object can not be constructed without valid Document!" );
        return;
    }

    if (!inherit.isEmpty()) {
        m_inherit = doc->obj(inherit);
        if (m_inherit == 0) {
            qCritical("Object can not inherit '%s' because it does not exist!", qPrintable(inherit) );
            return;
        }
    }

    m_valid = true;
}

Object::~Object()
{
//     qDebug() << " del obj... " << this;
    clear();
}

void Object::clear()
{
    m_paintLayersSet = false;
    m_paintLayers.clear();

    const ObjectExpression* e;
    foreach (e, m_expressions) {
        delete e;
    }
    m_expressions.clear();
    Tile* t;
    foreach (t, m_tiles) {
        delete t;
    }
    m_tiles.clear();
    TileSource* s;
    foreach (s, m_sources) {
        delete s;
    }
    m_sources.clear();
    TileLayout* l;
    foreach (l, m_layouts) {
        delete l;
    }
    m_layouts.clear();
}

bool Object::isValid() const
{
    return m_valid;
}

Document *Object::doc() const
{
    return m_doc;
}

void Object::addPaintLayer(const Object *layoutObj, int id)
{
    m_paintLayers.append(QPair<const Object*,int>(layoutObj, id) );

    m_paintLayersSet = true;
}

void Object::addExpression(int id, const ObjectExpression *e)
{
    if (m_expressions.contains(id) ) {
        qDebug() << "Object::addExpression: replacing already existing item '" << id << "'";
        delete m_expressions.value(id);
    }

    m_expressions.insert(id, e);
}

const ObjectExpression *Object::expression(int id) const
{
    if (m_expressions.contains(id) ) {
        return m_expressions.value(id);
    } else if (m_inherit) {
        return m_inherit->expression(id);
    } else {
        return 0;
    }
}

const Tile *Object::tile(int id) const
{
    if (m_tiles.contains(id) ) {
        return m_tiles.value(id);
    } else if (m_inherit) {
        return m_inherit->tile(id);
    } else {
        return 0;
    }
}

const TileSource *Object::tileSource(int id) const
{
    if (m_sources.contains(id) ) {
        return m_sources.value(id);
    } else if (m_inherit) {
        return m_inherit->tileSource(id);
    } else {
        return 0;
    }
}

const TileLayout *Object::layout(int id) const
{
    if (m_layouts.contains(id) ) {
        return m_layouts.value(id);
    } else if (m_inherit) {
        return m_inherit->layout(id);
    } else {
        return 0;
    }
}

void Object::insertLayout(TileLayout *l)
{
    if (!l) {
        qFatal("Object::insertLayout: item to insert is 0!");
        return;
    }

    if (m_layouts.contains(l->id() ) ) {
        qDebug() << "Object::insertLayout: replacing already existing item '" << l->id() << "'";
        delete m_layouts.value(l->id() );
    }

    m_layouts.insert(l->id(), l);
}

void Object::insertTile(Tile *tile)
{
    if (!tile) {
        qFatal("Object::insertTile: tile to insert is 0!");
        return;
    }

    if (m_tiles.contains(tile->id() ) ) {
        qDebug() << "Object::insertTile: replacing already existing item '" << tile->id() << "'";
        delete m_tiles.value(tile->id() );
    }

    m_tiles.insert(tile->id(), tile);
}

void Object::insertTileSource(TileSource *source)
{
    if (!source) {
        qFatal("Object::insertTileSource: tile source to insert is 0!");
        return;
    }

    if (m_sources.contains(source->id() ) ) {
        qDebug() << "Object::insertTileSource: replacing already existing item '" << source->id() << "'";
        delete m_sources.value(source->id() );
    }

    m_sources.insert(source->id(), source);
}

void Object::paint(QPainter *p,
                      int left, int top, int width, int height,
                      const ExpressionVariableBridge *vars,
                      const SpecialCellBridge *cells) const
{
    m_paintWidth = width;
    m_paintHeight = height;

    // layer inheritance resolution...
    bool done = false;
    const Object *o = this;
    while (!done && o != 0) {

        // paint layers set for object o. paint and exit the loop
        if (o->m_paintLayersSet) {
            QPair<const Object*, int> l;
            foreach (l, o->m_paintLayers) {
                const Object *i;
                if (l.first == 0)
                    i = this;
                else
                    i = l.first;
                ExecContext context(i, vars, cells);

                if (i) {
                    const TileLayout *layout = i->layout(l.second );
                    if (layout) {
                        layout->paint(&context, p,left,top,width,height);
                    } else {
                        qCritical("Object::paint: TileLayout '%d' does not exist!",
                                  l.second );
                    }
                }
            }
            done = true;
            continue;
        }

        // layers were not set, try inherited object...
        o = o->m_inherit;
    }


    m_paintWidth.clear();
    m_paintHeight.clear();
}

int Object::docRefCount() const
{
    return m_docRefCount;
}

const Object *Object::inherit() const
{
    return m_inherit;
}

ExecContext::ExecContext(const Object *obj, const ExpressionVariableBridge *vars,
                        const SpecialCellBridge *cells)
    : m_obj(obj), m_vars(vars), m_cells(cells)
{
}

const Object *ExecContext::obj() const
{
    return m_obj;
}

const ExpressionVariableBridge *ExecContext::vars() const
{
    return m_vars;
}

const SpecialCellBridge *ExecContext::cells() const
{
    return m_cells;
}

}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
