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
    DocumentPrivate(Document *doc) : m_doc(doc)
    {
    }

    /**
     * Implementation of the abstract ExpressionVariableIndexBridge interface.
     */
    virtual int getVariableIndex(const QString &id) const
    {
        return m_doc->mapToId(Document::VariableDecl, id);
    }

    Document *m_doc;

    QHash<int, Object*> objIndex;
    QHash<int, Object*> specIndex;

    // theme spec declaration (multiple string/int IDs can point to
    // one Object, e.g. with declarations like Button.*.*
    QHash<QString, int> declObjNames;
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
                QString objIdStr = attributes.value("id");
                QString objImplStr = attributes.value("implement");
                QString objExtendStr = attributes.value("extend");

                const Object *extendObj = 0;
                if(! objExtendStr.isEmpty() ) {
                    int extendId = m_doc->mapToId(Document::ObjectNameDecl,
                                                  objExtendStr);
                    if(extendId < 0) {
                        qCritical("%s: Object '%s' can not be extended, because it does not exist",
                                  qPrintable(objIdStr), qPrintable(objExtendStr));
                        return false;
                    }
                    extendObj = m_doc->obj(extendId);
                }

                m_currentObj = new Object(m_doc, objIdStr, extendObj);

                int objIdRet = m_doc->insertObject(m_currentObj);

                if (objIdRet<0) {
                    // error... cleanup: delete the object
                    delete m_currentObj;
                    m_currentObj = 0;

                    qCritical("%s: Error while inserting the object into the theme document.", qPrintable(objIdStr) );
                    return false;
                }


                /*
                 * Map specification object states to the object
                 */
                QStringList objStatesString = objImplStr.split(".");
                // translate the string state list to integer state IDs
                int stateObjId = m_doc->mapToId(Document::ObjectStateDecl,
                                             objStatesString.at(0) );
                if (stateObjId == -1) {
                    if (objStatesString.size() > 1) {
                        // only objects defined in the theme spec can have 'states'
                        qCritical("%s: invalid state id '%s': state object not defined in the theme specification.", qPrintable(objIdStr), qPrintable(objImplStr) );
                        return false;
                    }
                }

                QList<int> objStates;
                for (int i = 1; i < objStatesString.size(); ++i) {
                    int stateId = -1;
                    QString stateStr = objStatesString.at(i);
                    if (stateStr == "*") {
                        stateId = -1;
                    } else {
                        stateId = m_doc->mapObjectStateToId(stateObjId, i-1,
                                                            stateStr);
                        if (stateId == -1) {
                            qCritical("%s: state '%s' not defined in the theme specification.", qPrintable(objImplStr), qPrintable(stateStr));
                            return false;
                        }
                    }

                    objStates.append(stateId);
                }

                int objSpecMappingRet = m_doc->declareSpecMapping(objIdRet, stateObjId, objStates);

                if (objSpecMappingRet<0) {
                    qCritical("%s: Error while declaring the specification mapping.", qPrintable(objIdStr) );
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
                const int sourceId = m_doc->mapToId(Document::IdentifierDecl, sourceIdStr);
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
                const int layoutId = m_doc->mapToId(Document::IdentifierDecl, layoutIdStr);
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
                    const int tileId = m_doc->mapToId(Document::IdentifierDecl, tileIdStr);
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
                    const int id = m_doc->mapToId(Document::IdentifierDecl,
                                                        attributes.value("special_id") );
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
            int idx = m_doc->mapToId(Document::IdentifierDecl, id);
            if (idx == -1) {
                idx = m_doc->declareIdMapping(Document::IdentifierDecl, id);
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

        DocumentContext m_context;
        bool m_inTheme;
        Object *m_currentObj;
        TileLayout *m_currentLayout;
        SvgTileSource *m_currentSvgSource;

        Document *m_doc;
};


Document::Document()
    : d( new DocumentPrivate(this) )
{
}

Document::~Document()
{
    clear();
}

void Document::clear()
{
    foreach(Object *o, d->objIndex) {
        delete o;
    }
    d->objIndex.clear();

    d->specIndex.clear();

    d->declObjNames.clear();
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

const Object *Document::specObj(int specStateId) const
{
    if(! d->specIndex.contains(specStateId))
        return 0;

    return d->specIndex[specStateId];
}

const Object *Document::obj(const QString &id) const
{
    int objId = mapToId(ObjectNameDecl, id);
    qDebug() << "Document::obj" << id << "->" << objId;
    return obj(objId);
}

const Object *Document::obj(int id) const
{
    return d->objIndex[id];
}

int Document::insertObject(Object *obj)
{
    if (!obj) {
        qFatal("Document::insertObject: object to insert is 0!");
        return -1;
    }

    if (mapToId(Document::ObjectNameDecl, obj->id() ) != -1) {
        qCritical("Document::insertObject: %s: Object id has already been defined",
                  qPrintable(obj->id() ));
        return -1;
    }

    int objId = declareIdMapping(ObjectNameDecl, obj->id());

    if (objId < 0) {
        qCritical("Document::insertObject: %s: Unable to declare Object id",
                  qPrintable(obj->id() ));
        return -1;
    }

    qDebug() << "insertObject:" << objId << "->" << obj->id();
    d->objIndex.insert(objId, obj);

    return objId;
}

int Document::declareSpecMapping(int objId, int stateId)
{
    Object *o = d->objIndex.value(objId);
    if (o) {
        qDebug() << "declareSpecMapping:" << o->id() << objId << " -> " << stateId;
        d->specIndex.insert(stateId, o);
        return objId;
    }
    return -1;
}

int Document::declareSpecMapping(int objId, int stateObjId, const QList<int> &objStates)
{
//     QString statesStr = QString("obj: %1 states:").arg(objId);
//     foreach(int s, objStates) {
//         statesStr += QString(" %1").arg(s);
//     }
//     qDebug("Document::insertObject: %s", qPrintable(statesStr));

    // Traverse the objStates list. If all states are set (!= -1),
    // then every loop is skipped using continue at the beginning.
    // Otherwise (-1, meaning a wildcard), insertObject calls itself
    // recursively for all states in the state level, and
    // skips the rest of the current insertObject call using return;
    for (int i = 0; i < objStates.size(); ++i) {
        if (objStates.at(i) != -1) {
            continue;
        } else {

            // current state level is a wildcard (-1)

            if (mapObjectStateToId(stateObjId, i, 0) == -1) {
                qFatal("Document::insertObject: state level %d not defined in the theme specification.", i);
            }

            int j = 0;
            int stateId = -1;
            QList<int> newObjStates = objStates;
            while ((stateId = mapObjectStateToId(stateObjId, i, j++)) != -1) {
                newObjStates[i] = stateId;
                int ret = declareSpecMapping(objId, stateObjId, newObjStates);

                // Break here in case of an error.
                if (ret == -1) {
                    qCritical("Document::declareSpecMapping: Error while inserting the expanded object state into the theme document." );
                    return -1;
                }
            }

            // Multiple items inserted successfully
            return 0;
        }
    }

    // No more wildcards in the item. insert the mapping directly now
    // construct the final state-id
    int objStateAccessIndex = stateObjId;
    foreach(int stId, objStates) {
        objStateAccessIndex += stId;
    }

    if (d->specIndex.contains(objStateAccessIndex) ) {
        qCritical("Document::declareSpecMapping: Could not insert mapping because numerical id '%d' has already been defined.", objStateAccessIndex );
        return -1;
    } else {
        return declareSpecMapping(objId, objStateAccessIndex);
    }

}

int Document::mapToId(DeclarationType type, const QString &str) const
{
    switch(type) {
    case ObjectNameDecl:
        if (d->declObjNames.contains(str) )
            return d->declObjNames.value(str);
        else
            return -1;
    case ObjectStateDecl:
        return -1;
    case VariableDecl:
        if (d->declVariables.contains(str) )
            return d->declVariables.value(str);
        else
            return -1;
    case IdentifierDecl:
        if (d->declId.contains(str) )
            return d->declId.value(str);
        else
            return -1;
    }
    return -1;
}


int Document::customIdMappingBase(DeclarationType ) const
{
    return 0;
}

int Document::declareIdMapping(DeclarationType type, const QString &str)
{
    switch(type) {
    case ObjectNameDecl:
    {
        int idx = customIdMappingBase(ObjectNameDecl) + d->declObjNames.size();
        d->declObjNames.insert(str, idx);
        /* qDebug() << "declare objectName " << str << idx; */
        return idx;
    }
    case ObjectStateDecl:
        return -1;              // Themes can't declare states
    case VariableDecl:
        return -1; // Themes can't declare variables
    case IdentifierDecl:
    {
        int idx = customIdMappingBase(IdentifierDecl) + d->declId.size();
        d->declId.insert(str, idx);
        // qDebug() << "declare " << id << idx;
        return idx;
    }
    }

    return -1;
}

int Document::objectStateLevels(int) const
{
    return -1;
}

int Document::objectStateLevelStates(int, int) const
{
    return -1;
}

void Document::drawLayers(int objId,
                          QPainter *p, int left, int top, int width, int height,
                          const ExpressionVariableBridge *vars,
                          const SpecialCellBridge *cells) const
{
    const Object *o = specObj( objId );
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
    const Object *o = specObj( objId );
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
    const Object *o = specObj( objId );
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
    const Object *o = specObj( objId );
    if ( !o ) {
        qCritical("Object '%d' does not exist; unable to get expression value '%d'.",
                 objId, expressionId);
        return QVariant();
    }

    return o->evalExpression( expressionId, vars );
}

}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
