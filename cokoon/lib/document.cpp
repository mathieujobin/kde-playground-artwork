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

#include <QtXml>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QXmlDefaultHandler>

#include <QFileInfo>
#include <QDir>
#include <QtAlgorithms>

#include "document.h"

#include "expression_p.h"
#include "tile_p.h"
#include "tilesource_p.h"
#include "object_p.h"
#include "tilelayout_p.h"

namespace Cokoon {


class DocumentPrivate : public ExpressionVariableIndexFactory
{
public:
    /**
     * Implementation of the abstract ExpressionVariableIndexBridge interface.
     */
    virtual int getVariableIndex(const QString &id) const
    {
        if (declVariables.contains(id) )
            return declVariables.value(id);
        else
            return -1;
    }

    QHash<QString, Object*> objects;
    QHash<int, Object*> specIndex;

    // theme spec declaration (multiple string/int IDs can point to
    // one Object, e.g. with declarations like Button.*.*
    QHash<QString, int> objItems;
    QList<QList<QHash<QString,int> > > objStates;

    QHash<QString, int> declVariables;
    QHash<QString, int> declId;

};

class DocumentHandler : public QXmlDefaultHandler
{
    private:
        enum DocumentContext {
            DocumentStart,
            Theme,
            Obj,
            ObjExpression,
            ObjTile,
            ObjLayout,
            ObjLayoutColumn,
            ObjLayoutRow,
            ObjLayoutRowTile,
            ObjPaint,
            ObjPaintLayer,
            ObjPixmapSource,
            ObjBitmapSource,
            ObjSvgSource,
            ObjSvgSourceReplaceStyle,
            Unknown
        };

    public:
        DocumentHandler(Document *doc) : m_doc(doc)
        {
        };

        virtual bool startDocument()
        {
            m_context = DocumentStart;
            m_currentObj = 0;
            m_currentLayout = 0;
            m_currentSvgSource = 0;
            return true;
        }

        virtual bool startElement(const QString &/*namespaceURI*/,
                                  const QString &/*localName*/,
                                  const QString &qName,
                                  const QXmlAttributes &attributes)
        {
            if (m_context == DocumentStart && qName == "cokoon_theme") {
                m_context = Theme;
            } else if (m_context == Theme && qName == "object") {
                QString objId = attributes.value("id");
                QString objInherit = attributes.value("inherit");

                QStringList objIdStates = objId.split(".");

                // auto-complete the inheritance string
                if (!objInherit.isEmpty() ) {
                    QStringList inheritNodes = objInherit.split(".");

                    for (int i = 0; i < inheritNodes.size(); ++i) {
                        if (inheritNodes[i].isEmpty() ) {
                            if (!objIdStates[i].isEmpty() && objIdStates[i] != "*") {
                                inheritNodes[i] = objIdStates[i];
                            } else {
                                qCritical("%s: Unable to complete 'inherit' (%s): The according section must be specified in 'id'.", qPrintable(objId), qPrintable(objInherit) );
                                return false;
                            }
                        }
                    }

                    objInherit = inheritNodes.join(".");
                }

                if (objId.indexOf("*") != -1) {
                    // multiple objects defined at once; expand ids and for each
                    // id insert the same object. Thanks to docRefCount this
                    // will not cause any trouble.
                    QString itemIdStr = objIdStates.at(0);

                    int itemId = m_doc->declarationItemId(itemIdStr);
                    if (itemId == -1) {
                        qDebug() << itemIdStr;
                        qCritical("%s: Multiple object id invalid: First node must be specified and exist.", qPrintable(objId) );
                        return false;
                    }

                    m_currentObj = new Object(m_doc, objInherit );

                    insertMultiObjectDef(itemId, objIdStates, 1, m_currentObj);

                } else {
                    m_currentObj = new Object(m_doc, objInherit );
                    m_doc->insertObject(objId, m_currentObj);
                }

                if (m_currentObj->docRefCount() < 1) {
                    delete m_currentObj;
                    m_currentObj = 0;
                    return false;
                }

                m_context = Obj;
            } else if (m_context == Obj && m_currentObj && qName == "expression") {
                QString expressionIdStr = attributes.value("id");
                int expressionId = declareAndGetIdIndex(expressionIdStr);
                ObjectExpression *expression = new ObjectExpression( m_currentObj,
                                                                     attributes.value("value"), m_doc->d );
                if (! expression->isValid() ) {
                    delete expression; expression = 0;
                    return false;
                }
                m_currentObj->addExpression(expressionId, expression);
                m_context = ObjExpression;
            } else if (m_context == Obj && m_currentObj && qName == "source") {
                const QString sourceIdStr = attributes.value("id");
                const int sourceId = declareAndGetIdIndex(sourceIdStr);
                QString sourceType = attributes.value("type");

                const QString fileName = attributes.value("file");
                if (!checkSourceFile(fileName) ) {
                    return false;
                }

                if (sourceType == "svg") {
                    m_currentSvgSource = new SvgTileSource(m_currentObj, sourceId, fileName);
                    m_context = ObjSvgSource;
                } else if (sourceType == "pixmap") {
                    new PixmapTileSource(m_currentObj, sourceId, fileName);
                    m_context = ObjPixmapSource;
                } else if (sourceType == "bitmap") {
                    new BitmapTileSource(m_currentObj, sourceId, fileName);
                    m_context = ObjBitmapSource;
                } else {
                    qCritical("%d: Tile source type '%s' unknown.", sourceId, qPrintable(sourceType) );
                    return false;
                }

            } else if (m_context == ObjSvgSource && m_currentSvgSource && qName == "replace_style_attr") {
                QString elemId = attributes.value("element_id");
                QString attr = attributes.value("attr");
                if (elemId.isEmpty() || attr.isEmpty() ) {
                    qCritical("%d: Invalid replacement attribute.", m_currentSvgSource->id() );
                    return false;
                } else {
                    ObjectPropExpression *val = new ObjectPropExpression( m_currentObj, attributes.value("value"),
                                                                          m_doc->d );
                    if ( !val->isValid() ) {
                        delete val; val = 0;
                        return false;
                    }
                    m_currentSvgSource->addStyleReplacementExpression(elemId, attr, val);
                }
                m_context = ObjSvgSourceReplaceStyle;
            } else if (m_context == Obj && m_currentObj && qName == "tile") {
                QString tileIdStr = attributes.value("id");
                int tileId = declareAndGetIdIndex(tileIdStr );
                Tile::PaintMode tileHor = Tile::Normal, tileVert = Tile::Normal;
                if (!tilePaintMode(attributes.value("mode_hor"),tileHor) ) {
                    qCritical("%s: Unknown attribute value for mode_hor.", qPrintable(tileIdStr) );
                    return false;
                }
                if (!tilePaintMode(attributes.value("mode_vert"),tileVert) ) {
                    qCritical("%s: Unknown attribute value for mode_vert.", qPrintable(tileIdStr) );
                    return false;
                }
                const Object *sourceObj = 0;
                const QString sourceObjStr = attributes.value("source_object");
                if (! propObjStore(sourceObjStr, sourceObj) ) {
                    return false;
                }

                // check if the source exists
                const QString sourceIdStr = attributes.value("source_id");
                const int sourceId = m_doc->getIdentifierIndex(sourceIdStr);
                const Object *so = sourceObj;
                if (!so) {
                    so = m_currentObj;
                }
                if (! so->tileSource(sourceId) ) {
                    qCritical("TileSource '%s' (source_object '%s') does not exist.",
                              qPrintable(sourceIdStr), qPrintable(sourceObjStr) );
                    return false;
                }

                TileExpression *tsLeft = new TileExpression( m_currentObj, tileId, attributes.value("left"), m_doc->d );
                TileExpression *tsTop = new TileExpression( m_currentObj, tileId, attributes.value("top"), m_doc->d );
                TileExpression *tsWidth = new TileExpression( m_currentObj, tileId, attributes.value("width"), m_doc->d );
                TileExpression *tsHeight = new TileExpression( m_currentObj, tileId, attributes.value("height"), m_doc->d );
                if (! (tsLeft->isValid() && tsTop->isValid() && tsWidth->isValid() && tsHeight->isValid() ) ) {
                    delete tsLeft; tsLeft = 0;
                    delete tsTop; tsTop = 0;
                    delete tsWidth; tsWidth = 0;
                    delete tsHeight; tsHeight = 0;
                    return false;
                }
                new Tile(m_currentObj, tileId,
                              tileHor, tileVert,
                              sourceObj, sourceId,
                              tsLeft, tsTop, tsWidth, tsHeight);
                m_context = ObjTile;
            } else if (m_context == Obj && m_currentObj && qName == "layout") {
                QString layoutIdStr = attributes.value("id");
                const int layoutId = declareAndGetIdIndex(layoutIdStr);
                m_currentLayout = new TileLayout(m_currentObj, layoutId);
                m_context = ObjLayout;
            } else if (m_context == Obj && m_currentObj && qName == "paint") {
                m_context = ObjPaint;
            } else if (m_context == ObjPaint && m_currentObj && qName == "layer") {
                const QString layoutObjStr = attributes.value("layout_object");
                const Object *layoutObj;
                if (! propObjStore(layoutObjStr, layoutObj) ) {
                    return false;
                }

                // check if the layout exists
                const QString layoutIdStr = attributes.value("layout_id");
                const int layoutId = m_doc->getIdentifierIndex(layoutIdStr);
                const Object *lo = layoutObj;
                if (!lo) {
                    lo = m_currentObj;
                }
                if (! lo->layout(layoutId) ) {
                    qCritical("TileLayout '%s' (layout_object '%s') does not exist.",
                              qPrintable(layoutIdStr), qPrintable(layoutObjStr) );
                    return false;
                }

                m_currentObj->addPaintLayer(layoutObj, layoutId);
                m_context = ObjPaintLayer;
            } else if (m_context == ObjLayout && m_currentLayout && qName == "column") {
                TileLayout::LayoutBehavior colType = TileLayout::Expanding;
                if (! layoutBehavior(attributes.value("type"), colType) ) {
                    qCritical("%d: column: Unknown value for attribute 'type'.", m_currentLayout->id() );
                    return false;
                }
                ObjectPropExpression *colSize = 0;
                if (colType == TileLayout::Fixed) {
                    colSize = new ObjectPropExpression( m_currentObj, attributes.value("fixed_size"),
                                                        m_doc->d );
                    if (! colSize->isValid() ) {
                        delete colSize; colSize = 0;
                        return false;
                    }
                }
                m_currentLayout->addCol(colType, colSize);
                m_context = ObjLayoutColumn;
            } else if (m_context == ObjLayout && m_currentLayout && qName == "row") {
                TileLayout::LayoutBehavior rowType = TileLayout::Expanding;
                if (! layoutBehavior(attributes.value("type"), rowType) ) {
                    qCritical("%d: row: Unknown value for attribute 'type'.", m_currentLayout->id() );
                    return false;
                }
                ObjectPropExpression *rowSize = 0;
                if (rowType == TileLayout::Fixed) {
                    rowSize = new ObjectPropExpression( m_currentObj, attributes.value("fixed_size"),
                                                        m_doc->d );
                    if (! rowSize->isValid() ) {
                        delete rowSize; rowSize = 0;
                        return false;
                    }
                }
                m_currentLayout->addRow(rowType, rowSize);
                m_context = ObjLayoutRow;
            } else if (m_context == ObjLayoutRow && m_currentLayout && qName == "cell") {
                QString cellType = attributes.value("type");
                if (cellType == "empty") {
                    m_currentLayout->addEmptyCell();
                } else if (cellType == "tile") {
                    const QString tileObjStr = attributes.value("tile_object");
                    const Object *tileObj;
                    if (! propObjStore(tileObjStr, tileObj) ) {
                        return false;
                    }

                    // check if the tile exists
                    const QString tileIdStr = attributes.value("tile_id");
                    const int tileId = m_doc->getIdentifierIndex(tileIdStr);
                    const Object *to = tileObj;
                    if (!to) {
                        to = m_currentLayout->obj();
                    }
                    if (! to->tile(tileId) ) {
                        qCritical("Tile '%s' (tile_object '%s') does not exist.",
                                  qPrintable(tileIdStr), qPrintable(tileObjStr) );
                        return false;
                    }

                    m_currentLayout->addTileCell(tileObj, tileId);
                } else if (cellType == "special") {
                    const int id = m_doc->getIdentifierIndex(attributes.value("special_id") );
                    if (id == -1) {
                        qCritical("special_id not specified or invalid!");
                        return false;
                    }

                    m_currentLayout->addSpecialCell(id);
                } else {
                    qCritical("%d: cell: Unknown type '%s' for attribute 'cell'.", m_currentLayout->id(), qPrintable(cellType) );
                    return false;
                }
                m_context = ObjLayoutRowTile;
            } else {
                qCritical("Unexpected start of element '%s'.", qPrintable(qName) );
                m_context = Unknown;
                return false;
            }

            return true;
        };

        virtual bool endElement(const QString &/*namespaceURI*/,
                                const QString &/*localName*/,
                                const QString &qName)
        {
            if (m_context == ObjLayoutRowTile && qName == "cell") {
                m_context = ObjLayoutRow;
            } else if (m_context == ObjLayoutRow && qName == "row") {
                m_context = ObjLayout;
            } else if (m_context == ObjLayoutColumn && qName == "column") {
                m_context = ObjLayout;
            } else if (m_context == ObjLayout && qName == "layout") {
                m_currentLayout = 0;
                m_context = Obj;
            } else if (m_context == ObjPaintLayer && qName == "layer") {
                m_context = ObjPaint;
            } else if (m_context == ObjPaint && qName == "paint") {
                m_context = Obj;
            } else if (m_context == ObjTile && qName == "tile") {
                m_context = Obj;
            } else if ( (m_context == ObjPixmapSource || m_context == ObjBitmapSource)
                         && qName == "source") {
                m_context = Obj;
            } else if (m_context == ObjSvgSource && qName == "source") {
                m_currentSvgSource = 0;
                m_context = Obj;
            } else if (m_context == ObjSvgSourceReplaceStyle && qName == "replace_style_attr") {
                m_context = ObjSvgSource;
            }else if (m_context == ObjExpression && qName == "expression") {
                m_context = Obj;
            } else if (m_context == Obj && qName == "object") {
                m_currentObj = 0;
                m_context = Theme;
            } else if (m_context == Theme && qName == "cokoon_theme") {
                m_context = DocumentStart;
            } else {
                qCritical("Unexpected end of element '%s'.", qPrintable(qName) );
                m_context = Unknown;
                return false;
            }

            return true;
        };

        bool fatalError(const QXmlParseException &exception)
        {
            qWarning("line %i, column %i: %s",
                     exception.lineNumber(), exception.columnNumber(),
                     qPrintable(exception.message()));

            return true;
        }

    private:
        int declareAndGetIdIndex(const QString &id) {
            int idx = m_doc->getIdentifierIndex(id);
            if (idx == -1) {
                idx = m_doc->declareIdentifier(id);
            }
            return idx;
        }
        bool propObjStore(const QString &objId, const Object *&obj ) {
            obj = 0;
            if (!objId.isEmpty() ) {
                obj = m_doc->obj(objId);
                if (!obj) {
                    qCritical("Object '%s' does not exist.", qPrintable(objId) );
                    return false;
                }
            }
            return true;
        }
        bool checkSourceFile(const QString &fileName)
        {
            QFileInfo fi(fileName);
            if (fi.isAbsolute() ) {
                qCritical("No absolute file path '%s' allowed.", qPrintable(fileName));
                return false;
            }

            if (! fi.exists() ) {
                qCritical("File '%s' does not exist.", qPrintable(fileName) );
                return false;
            }

            return true;
        }
        bool layoutBehavior(const QString &typeAttr, TileLayout::LayoutBehavior &beh)
        {
            if (typeAttr == "fixed") {
                beh = TileLayout::Fixed;
            } else if (typeAttr == "expanding") {
                beh = TileLayout::Expanding;
            } else {
                return false;
            }
            return true;
        }

        bool tilePaintMode(const QString &typeAttr, Tile::PaintMode &m)
        {
            if (typeAttr == "normal") {
                m = Tile::Normal;
            } else if (typeAttr == "centered") {
                m = Tile::Centered;
            } else if (typeAttr == "tiled") {
                m = Tile::Tiled;
            } else if (typeAttr == "stretched") {
                m = Tile::Stretched;
            } else {
                return false;
            }
            return true;
        }

        bool insertMultiObjectDef(int itemId, QStringList objStates, int section, Object *obj)
        {
            while (section < objStates.count() &&
                   m_doc->declarationStates(itemId,section-1).contains(objStates[section]) ) {
                ++section;
            }

            if (section >= objStates.count() ) {
                QString id = objStates.join(".");
                // definitions of multiple objects can never overwrite already existing
                // objects.
                if (m_doc->obj(id) == 0) {
                    m_doc->insertObject(id, obj);
                }/* else {
                    qDebug() << "Skipping inserting object" << id << "because it has already been defined.";
                }*/
            } else if (objStates[section] == "*") {
                QHashIterator<QString,int> i(m_doc->declarationStates(itemId,section-1) );
                while (i.hasNext()) {
                    i.next();

                    objStates[section] = i.key();
                    if (! insertMultiObjectDef(itemId, objStates, section, obj) )
                        return false;
                }
            } else {
                qCritical("%s: Multiple object id invalid: state '%s' (sec %d) has not been declared.", qPrintable(objStates.join(".")), qPrintable(objStates[section]), section );
                return false;
            }

            return true;
        }

        DocumentContext m_context;
        bool m_inTheme;
        Object *m_currentObj;
        TileLayout *m_currentLayout;
        SvgTileSource *m_currentSvgSource;

        Document *m_doc;
};


Document::Document()
    : d( new DocumentPrivate )
{
}

Document::~Document()
{
    clear();
}

void Document::clear()
{
    Object *o;
    foreach(o, d->objects) {
        unrefObj(o);
    }
    d->objects.clear();
    foreach(o, d->specIndex) {
        unrefObj(o);
    }
    d->specIndex.clear();

    d->objItems.clear();
    d->objStates.clear();
    d->declVariables.clear();
    d->declId.clear();
}

void Document::loadTheme(const QString &fileName)
{
    DocumentHandler handler(this);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    QFile file(fileName);

    if (! file.exists() ) {
        qCritical("File '%s' does not exist.",
                  qPrintable(fileName) );
        return;
    }

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qCritical("File '%s' can not be opened: %s", qPrintable(fileName), qPrintable(file.errorString()));
        return;
    }

    QFileInfo fileInfo(fileName);
    QDir::setCurrent(fileInfo.absolutePath() );

    QXmlInputSource xmlInputSource(&file);
    if (! reader.parse(xmlInputSource) ) {
        qCritical("%s: %s", qPrintable(fileName), qPrintable(handler.errorString()) );
        return;
    }
}

const Object *Document::obj(const QString &id) const
{
    return d->objects.value(id);
}

const Object *Document::obj(int id) const
{
//     qDebug() << "requested obj id: " << id;
    return d->specIndex.value(id);
}

void Document::refObj(Object *o)
{
    if (!o)
        return;
    o->m_docRefCount += 1;
}

void Document::unrefObj(Object *o)
{
    if (!o)
        return;
    if (o->m_docRefCount > 1)
        o->m_docRefCount -= 1;
    else
        delete o;
}

void Document::insertObject(const QString &objId, Object *obj)
{
    if (!obj) {
        qFatal("Document::insertObject: item to insert is 0!");
        return;
    }

    if (d->objects.contains(objId) ) {
        qCritical("Document::insertObject: Could not insert object because id '%s' has already been defined.", qPrintable(objId) );
        return;
    }

    d->objects.insert(objId, obj);
    refObj(obj);

    // try to insert the same object with numerical id as well.
    QStringList objStates = objId.split(".");
    int itemId = declarationItemId(objStates[0] );
    if (itemId != -1) {
        bool isSpecObject = true;
        int specIndex = itemId+1;

        QHash<QString,int> stateLvl;
        for (int i = 0; isSpecObject && i < d->objStates.at(itemId).size(); ++i) {
            if ((i+1) >= objStates.size() ||
                 !d->objStates.at(itemId).at(i).contains(objStates.at(i+1) ) ) {
                isSpecObject = false;
                continue;
            } else {
                specIndex += d->objStates.at(itemId).at(i).value(objStates.at(i+1) );
            }
        }

        if (isSpecObject) {
            if (d->specIndex.contains(specIndex) ) {
                qCritical("Document::insertObject: Could not insert object because numerical id '%d' has already been defined.", specIndex );
            } else {
                d->specIndex.insert(specIndex, obj);
                refObj(obj);
            }
        }
    }
}

int Document::declareObject(const QString &objName, QList<QHash<QString,int> > objStates)
{
    int objId = d->objStates.size();
    d->objItems.insert(objName, objId );
    d->objStates.append(objStates);

    return objId;
}

void Document::declareVariable(const QString &varId, int varIndex)
{
    d->declVariables.insert(varId, varIndex);
}

int Document::declareIdentifier(const QString &id, int index)
{
    if (index != -1) {
        d->declId.insert(id, index);
//         qDebug() << "declare " << id << index;
        return index;
    } else {
        int idx = d->declId.size();
        d->declId.insert(id, idx);
//         qDebug() << "declare " << id << idx;
        return idx;
    }
}

int Document::getVariableIndex(const QString &id) const
{
    return d->getVariableIndex( id );
}

int Document::getIdentifierIndex(const QString &id) const
{
    if (d->declId.contains(id) )
        return d->declId.value(id);
    else
        return -1;
}

int Document::declarationItemId(const QString &objName) const
{
    if (d->objItems.contains(objName) )
        return d->objItems.value(objName);
    else
        return -1;
}

    int Document::declarationItemStateLevels( int itemId ) const
    {
        return d->objStates.at( itemId ).size();
    }

const QHash<QString,int> Document::declarationStates(int objId, int level)
{
    if (d->objStates.size() > objId && d->objStates.at(objId).size() > level ) {
        return d->objStates.at(objId).at(level);
    } else {
        return QHash<QString,int>();
    }
}

void Document::drawLayers(int objId,
                          QPainter *p, int left, int top, int width, int height,
                          const ExpressionVariableBridge *vars,
                          const SpecialCellBridge *cells) const
{
    const Object *o = obj( objId );
    if ( o )
        o->paint( p, left, top, width, height, vars, cells );
    else
        qCritical("Object '%d' does not exist; unable to paint layers.", objId);
}

void Document::drawLayout(int objId, int layoutId,
                          QPainter *p, int left, int top, int width, int height,
                          const ExpressionVariableBridge *vars,
                          const SpecialCellBridge *cells) const
{
    const Object *o = obj( objId );
    if ( !o )
        return;

    const TileLayout *l = o->layout( layoutId );
    if ( l ) {
        ExecContext context(o, vars, cells);
        l->paint( &context, p, left, top, width, height );
    }
}

QPixmap Document::getTilePixmap(int objId, int tileId,
                                const ExpressionVariableBridge *vars) const
{
    const Object *o = obj( objId );
    if ( !o )
        return QPixmap();

    const Tile *t = o->tile( tileId );
    if ( t ) {
        ExecContext context(o, vars, 0);
        return t->pixmap( &context );
    }

    return QPixmap();
}

QVariant Document::getExpValue(int objId, int expressionId,
                               const ExpressionVariableBridge *vars) const
{
    const Object *o = obj( objId );
    if ( !o ) {
        qCritical("Object '%d' does not exist; unable to get expression value '%d'.",
                 objId, expressionId);
        return QVariant();
    }

    return o->evalExpression( expressionId, vars );
}

}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
