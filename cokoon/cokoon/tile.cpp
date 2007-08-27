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

#include <QPixmap>
#include <QPainter>

#include <QDebug>

#include "expression_p.h"
#include "document.h"
#include "object_p.h"
#include "tilesource_p.h"
#include "tile_p.h"

namespace Cokoon {

Tile::Tile(Object *obj, int id,
                  PaintMode horPaintMode, PaintMode vertPaintMode,
                  const Object *sourceObj, int sourceId,
                  const TileExpression *sLeft, const TileExpression *sTop,
                  const TileExpression *sWidth, const TileExpression *sHeight)
    : m_horPaintMode(horPaintMode), m_vertPaintMode(vertPaintMode),
      m_sourceObj(sourceObj), m_sourceId(sourceId),
      m_sLeft(sLeft), m_sTop(sTop), m_sHeight(sHeight), m_sWidth(sWidth),
                m_id(id), m_obj(obj), m_valid(false)
{
    if (!m_obj) {
        qFatal("Tile '%d' can not be constructed without valid Object!", id );
        return;
    }

    m_obj->insertTile(this);

    if (id < 0) {
        qCritical("Invalid tile id '%d'!", id);
        return;
    }

    // TODO: check existance of the source!

    m_valid = true;
}

Tile::~Tile()
{
    delete m_sLeft;
    delete m_sTop;
    delete m_sHeight;
    delete m_sWidth;
}

int Tile::id() const
{
    return m_id;
}

Object *Tile::obj() const
{
    return m_obj;
}

void Tile::paint(const ExecContext *c, QPainter *p,
                        int left, int top, int width, int height) const
{
    if (!c)
        return;
//     qDebug() << id() << ": paint:" << left << top << "," << width << "x" << height;

    const Object *si;
    if (m_sourceObj == 0)
        si = c->obj();
    else
        si = m_sourceObj;
    ExecContext sourceContext(si, c->vars(), c->cells() );

    const TileSource *s;
    if (si && (s = si->tileSource(m_sourceId)) ) {
        int sLeft = eval(c,m_sLeft).toInt();
        int sTop = eval(c,m_sTop).toInt();
        int sWidth = eval(c,m_sWidth).toInt();
        int sHeight = eval(c,m_sHeight).toInt();

//         QPixmap pm = s->pixmap(sLeft, sTop, sWidth, sHeight, vars);

        int stretchH = sHeight;
        int stretchW = sWidth;
        if (m_horPaintMode == Stretched)
            stretchW = width;
        if (m_vertPaintMode == Stretched)
            stretchH = height;
        QPixmap pm = s->pixmap(&sourceContext, sLeft, sTop, sWidth, sHeight, stretchW, stretchH);

        if (!pm.isNull() ) {
//             if (m_horPaintMode == m_vertPaintMode ) {
//                 switch (m_horPaintMode) {
//                     case Normal:
//                         p->drawPixmap(left,top, pm);
//                         break;
//                     case Tiled:
                        p->drawTiledPixmap(left,top,width,height, pm);
//                         break;
//                     default:
//                         qDebug() << "PaintMode" << m_horPaintMode << "not implemented yet";
//                 }
//             } else {
//                 qDebug() << "horPaintMode != vertPaintMode not implemented yet";
//             }
        }

    } else {
        // TODO: complain about non existing source!!!!
    }
}

QPixmap Tile::pixmap(const ExecContext *c) const
{
    if (!c)
        return 0;

    const Object *si;
    if (m_sourceObj == 0)
        si = c->obj();
    else
        si = m_sourceObj;
    ExecContext sourceContext(si, c->vars(), c->cells() );

    const TileSource *s;
    if (si && (s = si->tileSource(m_sourceId)) ) {
        int sLeft = eval(c,m_sLeft).toInt();
        int sTop = eval(c,m_sTop).toInt();
        int sWidth = eval(c,m_sWidth).toInt();
        int sHeight = eval(c,m_sHeight).toInt();

//         QPixmap pm = s->pixmap(sLeft, sTop, sWidth, sHeight, vars);

        return s->pixmap(&sourceContext, sLeft, sTop, sWidth, sHeight, sWidth, sHeight);
    }
    return QPixmap();
}

QVariant Tile::eval(const ExecContext *c, const TileExpression *e) const
{
    if (!c || !e)
        return QVariant();

    return e->evaluate(c->obj(), c->vars() );
}

}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
