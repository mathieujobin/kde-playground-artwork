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

#include <QPixmapCache>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "object_p.h"
#include "tilesource_p.h"

namespace Cokoon {

TileSource::TileSource(Object *obj, int id)
    : m_id(id), m_obj(obj), m_valid(false)
{
    if (!m_obj) {
        qFatal("TileSource '%d' can not be constructed without valid Object!", id );
        return;
    }

    m_obj->insertTileSource(this);

    if (id < 0) {
        qCritical("Invalid tile source id '%d'!", id);
        return;
    }

    m_valid = true;
}

int TileSource::id() const
{
    return m_id;
}

Object *TileSource::obj() const
{
    return m_obj;
}

TileSource::~TileSource()
{
}

PixmapTileSource::PixmapTileSource(Object *obj, int id, const QString &fileName)
    : TileSource(obj,id), m_pm(QPixmap(fileName) )
{
//     qDebug() << "PixmapTileSource: " << fileName << " " << m_pm.width() << "x" << m_pm.height();
    // TODO: check if the file exists!
}

QPixmap PixmapTileSource::pixmap(const ExecContext *,
                                 int left, int top, int width, int height,
                                 int scaleWidth, int scaleHeight) const
{
    // TODO
//     qDebug() << id() << "pixmap:" << left << top << "," << width << "x" << height;
    return m_pm.copy(left, top, width, height);
}

BitmapTileSource::BitmapTileSource(Object *obj, int id, const QString &fileName)
    : TileSource(obj,id), m_bm(QBitmap(fileName) )
{
}

QPixmap BitmapTileSource::pixmap(const ExecContext *,
                                 int left, int top, int width, int height,
                                 int scaleWidth, int scaleHeight) const
{
    return m_bm.copy(left, top, width, height);
}

class SvgSourceReplaceData {
    public:
        SvgSourceReplaceData(const QString &i,
                             bool sa,
                             const QString &a,
                             const ObjectPropExpression *e)
            : id(i), styleAttr(sa), attr(a), expr(e)
        {
        };

        ~SvgSourceReplaceData()
        {
            delete expr;
        };

        QString id;

        // Whether an attribute of a 'style' attribute should be replaced. Otherwise
        // a normal xml attribute will be replaced...
        bool styleAttr;
        QString attr;
        const ObjectPropExpression *expr;

        QVariant oldExprValue;
};

SvgTileSource::SvgTileSource(Object *obj, int id, const QString &fileName)
    : TileSource(obj,id), m_svg(0), m_fileName(fileName)
{
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qCritical("File '%s' can not be opened: %s", qPrintable(fileName), qPrintable(f.errorString()));
        return;
    }

    m_svgData = f.readAll();
}

SvgTileSource::~SvgTileSource()
{
    qDeleteAll(m_replaceAttrs);
    m_replaceAttrs.clear();
    delete m_svg;
}

void SvgTileSource::addStyleReplacementExpression(const QString &idAttr,
                                   const QString &styleAttr,
                                   const ObjectPropExpression *expression)
{
    SvgSourceReplaceData *d = new SvgSourceReplaceData(idAttr,true,
            styleAttr,expression);
    m_replaceAttrs.append(d);
}

QPixmap SvgTileSource::pixmap(const ExecContext *c,
                              int left, int top, int width, int height,
                              int scaleWidth, int scaleHeight) const
{
    if (!c) {
        return QPixmap();
    }

    QString cacheKey;
    QTextStream(&cacheKey) << "$cokoonSvg" << m_fileName << left << " " <<
            top << " " << width << " " << height << " " << scaleWidth <<
            " " << scaleHeight;

    SvgSourceReplaceData *r;
    foreach (r, m_replaceAttrs) {
        if (r->expr)
            cacheKey += r->id + r->expr->evaluate(c->obj(), c->vars() ).toString();
    }

    QPixmap pm;
    if (!QPixmapCache::find(cacheKey, pm) ) {
//         qDebug() << "missed cache key:" << qPrintable(cacheKey);

        bool svgDirty = false;
        SvgSourceReplaceData *r;
        foreach (r, m_replaceAttrs) {
            QVariant newVal;
            if (r->expr)
                newVal = r->expr->evaluate(c->obj(), c->vars() );
            else
                continue;

            if (newVal != r->oldExprValue) {
//                 qDebug() << "replace (" << newVal << "differs from" << r->oldExprValue << ")";

                int idx = 0;
                bool endReplacement = false;
                while (!endReplacement) {
                    idx = m_svgData.indexOf("id=\"" + r->id + "\"", idx);

                    if (idx == -1) {
                        endReplacement = true;
                        continue;
                    }

                    ++idx;

                    int tagBegin = m_svgData.lastIndexOf("<", idx);

                    if (tagBegin == -1) {
                        endReplacement = true; // invalid document...?
                        continue;
                    }

                    int max = m_svgData.length();
                    int i = tagBegin;
                    QString s = r->attr + ":";
                    while (!endReplacement && i < max && m_svgData.at(i)!='>') {
                        if (m_svgData.at(i) == s.at(0) &&
                            m_svgData.mid(i, s.length()) == s ) {
                            const int attrValStart = m_svgData.indexOf(":", i) + 1;
                            const int end1 = m_svgData.indexOf(";", attrValStart);
                            const int end2 = m_svgData.indexOf("\"", attrValStart);
                            int attrValEnd;
                            if (end1 != -1 && end2 != -1) {
                                attrValEnd = qMin(end1, end2);
                            } else if (end1 != -1) {
                                attrValEnd = end1;
                            } else {
                                attrValEnd = end2;
                            }
                            qDebug() << "start:" << attrValStart << " end:" << attrValEnd;
                            if (attrValStart != -1 && attrValEnd != -1) {
                                int l = attrValEnd-attrValStart;
//                             qDebug() << "replace" << (attrValStart) << (l) << m_svgData.mid(attrValStart, l) << "width" << newValString;
                                m_svgData.replace(attrValStart, l, newVal.toString().toAscii() );

                                r->oldExprValue = newVal;
                                svgDirty = true;
                                endReplacement = true;
                            }
                            }
                            ++i;
                    }

                }

            }
        }

        if (svgDirty) {
            delete m_svg;
            m_svg = 0;
        }

        if (!m_svg) {
            m_svg = new QSvgRenderer(m_svgData);
        }

        if (!m_svg->isValid() )
            return pm;

        m_svg->setViewBox(QRect(left,top,width,height) );

        pm = QPixmap(scaleWidth, scaleHeight);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        m_svg->render(&p);

        QPixmapCache::insert(cacheKey, pm);
    }

//     qDebug() << id() << "pixmap:" << left << top << "," << width << "x" << height;
    return pm;
}

}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
