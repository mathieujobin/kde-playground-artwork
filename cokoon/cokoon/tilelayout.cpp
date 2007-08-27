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

#include <QStack>
#include <QVector>

#include <QDebug>

#include "bridges.h"
#include "expression_p.h"
#include "tile_p.h"
#include "object_p.h"
#include "document.h"
#include "tilelayout_p.h"

namespace Cokoon {

class LayoutCellContent
{
    public:
        virtual ~LayoutCellContent() {};
};

class TileCellContent : public LayoutCellContent
{
    public:
        TileCellContent(const Object *tileObj, const int tileId) : obj(tileObj), id(tileId)
        {
        };

        const Object *obj;
        const int id;
};

class SpecialCellContent : public LayoutCellContent
{
    public:
        SpecialCellContent(const int specialId) : id(specialId)
        {
        };

        const int id;
};

TileLayout::TileLayout(Object *obj, int id)
    : m_id(id), m_obj(obj), m_valid(false)
{
    if (!m_obj) {
        qFatal("TileLayout '%d' can not be constructed without valid Object!", id );
        return;
    }

    m_obj->insertLayout(this);

    if (id < 0) {
        qCritical("Invalid layout id '%d'!", id);
        return;
    }

    m_valid = true;
}

int TileLayout::id() const
{
    return m_id;
}

Object *TileLayout::obj() const
{
    return m_obj;
}

TileLayout::~TileLayout()
{
    clear();
}

void TileLayout::clear()
{
    QPair<LayoutBehavior, ObjectPropExpression*> l;
    foreach (l, m_cols) {
        delete l.second;
    }
    m_cols.clear();
    foreach (l, m_rows) {
        delete l.second;
    }
    m_rows.clear();
    QList<LayoutCellContent* > row;
    foreach (row, m_grid) {
        LayoutCellContent* cell;
        foreach (cell, row) {
            delete cell;
        }
    }
    m_grid.clear();
}

void TileLayout::addCol(LayoutBehavior b, ObjectPropExpression *fixedSize)
{
    if (!m_rows.isEmpty() ) {
        qCritical("%d: TileLayout::addCol: Columns can only be added before rows have been added!", id() );
        return;
    }

    if (b == Fixed && !fixedSize) {
        qCritical("%d: TileLayout::addCol: Layout Behavior 'Fixed' selected, but no fixed size provided!", id() );
        return;
    }

    m_cols.append(QPair<LayoutBehavior, ObjectPropExpression*>(b, fixedSize) );
}

void TileLayout::addRow(LayoutBehavior b, ObjectPropExpression *fixedSize)
{
    if (b == Fixed && !fixedSize) {
        qCritical("%d: TileLayout::addRow: Layout Behavior 'Fixed' selected, but no fixed size provided!", id() );
        return;
    }

    m_rows.append(QPair<LayoutBehavior, ObjectPropExpression*>(b, fixedSize) );
    m_grid.append(QList<LayoutCellContent* >() );
}

bool TileLayout::cellInsertCheck()
{
    if (m_grid.isEmpty() ) {
        qCritical("%d: TileLayout::addRowCell: Can not add cell because no row has been created!", id() );
        return false;
    }

    if (m_grid.last().size() >= m_cols.size() ) {
        qCritical("%d: TileLayout::addRowCell: Can not add cell because the last Column has been reached!", id() );
        return false;
    }

    return true;
}

void TileLayout::addTileCell(const Object *tileObj, int tileId)
{
    if (!cellInsertCheck() )
        return;

    m_grid.last().append(new TileCellContent(tileObj, tileId) );
}

void TileLayout::addEmptyCell()
{
    if (!cellInsertCheck() )
        return;

    m_grid.last().append(0);
}

void TileLayout::addSpecialCell(int id)
{
    if (!cellInsertCheck() )
        return;

    m_grid.last().append(new SpecialCellContent(id) );
}

void TileLayout::paint(const ExecContext *context, QPainter *p,
                       int left, int top, int width, int height) const
{
    if (!context)
        return;

    QList<int> cw;
    QList<int> rh;
    calcCellSizes(context, width, m_cols, cw);
    calcCellSizes(context, height, m_rows, rh);

    int paintX = left;
    int paintY = top;

    int nc = m_cols.size();
    int nr = m_rows.size();
    for (int r = 0; r < nr; ++r) {

        paintX = left;
        for (int c = 0; c < nc; ++c) {
            const LayoutCellContent *cell = m_grid[r][c];
            if (cell) {
                if (const TileCellContent *tc = dynamic_cast<const TileCellContent*>(cell) ) {
                    const Object *i;
                    if (tc->obj == 0) {
                        i = context->obj();
                    } else {
                        i = tc->obj;
                    }
                    ExecContext ctxt(i, context->vars(), context->cells() );

                    const Tile *t;
                    if (i && (t = i->tile(tc->id ) ) ) {
                        t->paint(&ctxt, p, paintX,paintY, cw[c], rh[r]);
                    } else {
                        qCritical("TileLayout %d: col %d, row %d: Tile '%d' does not exist!",
                                  id(), c, r, tc->id );
                    }
                } else if (const SpecialCellContent *sc = dynamic_cast<const SpecialCellContent*>(cell) ) {
                    const SpecialCellBridge *b = context->cells();

                    if (b) {
                        b->paintCell(sc->id, p, paintX, paintY, cw[c], rh[r]);
                    } else {
                        qCritical("TileLayout %d: can not paint special cell '%d' because no SpecialCellBridge is provided!",
                                  id(), sc->id);
                    }
                }
            } else {
//                 qDebug() << c << r << ":" << cw[c] << "x" << rh[r] << ": empty";
            }

            paintX += cw[c];
        }

        paintY += rh[r];
    }
}

void TileLayout::calcCellSizes(const ExecContext *c, int available, QList<QPair<LayoutBehavior, ObjectPropExpression*> > cells, QList<int> &sizes) const
{
    if (!c)
        return;

    sizes.clear();

    int fixedSize = 0;
    QStack<int> expandingCells;

    QPair<LayoutBehavior, ObjectPropExpression*> cell;
    for (int i = 0; i < cells.size(); ++i) {
        cell = cells[i];
        if (cell.first == Fixed) {
            int s = cell.second->evaluate(c->obj(), c->vars() ).toInt();
            fixedSize += s;
            sizes.append(s);
        } else {
            expandingCells.push(i);
            sizes.append(0);
        }
    }

    if (expandingCells.size() >= 1) {
        int expandingCellSize = (available-fixedSize)/expandingCells.size();
        // TODO: distribute the division rest on the expandiong objects as good as possible
        while (! expandingCells.isEmpty() ) {
            sizes[expandingCells.pop()] = expandingCellSize;
        }
    }
}

}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
