/* This file is part of the Cokoon theme library
 * Copyright (C) 2007 Sandro Giessl <giessl@kde.org>
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

#include <QtAlgorithms>

#include <math.h>

#include "specification.h"
#include <QFile>

namespace Cokoon {

    SpecificationItem::~SpecificationItem()
    {
        qDeleteAll(providedVariables);
        qDeleteAll(stateLevels);
    }
    SpecificationItemStateLevel::~SpecificationItemStateLevel() {}
    SpecificationItemVariable::~SpecificationItemVariable() {}

    Specification::Specification()
        : m_name(QString()), m_version(QString())
    {
        clear();
    }

    Specification::~Specification()
    {
        clear();
    }

    void Specification::clear()
    {
        qDeleteAll(m_items);
        m_items.clear();
    }

    QList<SpecificationItem*> Specification::items() const
    {
        return m_items;
    }

    const QString &Specification::specName() const
    {
        return m_name;
    }

    const QString &Specification::specVersion() const
    {
        return m_version;
    }

    const QStringList &DocumentSpecification::variables() const
    {
        return m_variables;
    }

    const QStringList &DocumentSpecification::identifiers() const
    {
        return m_identifiers;
    }

    const QStringList &DocumentSpecification::enumIdentifiers() const
    {
        return m_enumIdentifiers;
    }

    bool Specification::loadSpecification(const QString &fileName)
    {
        clear();

        QFile file(fileName);

        if (! file.exists() ) {
            qCritical("File '%s' does not exist.",
                      qPrintable(fileName) );
            return false;
        }

        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qCritical("File '%s' can not be opened: %s", qPrintable(fileName), qPrintable(file.errorString()));
            return false;
        }

        return loadSpecification(&file);
    }

    bool Specification::loadSpecification(QIODevice *device) {
        clear();

        setDevice(device);

        while (!atEnd()) {
            readNext();
            if (isStartElement()) {
//                 qDebug() << "loadSpecification: name:" << name().toString();
                if (name() == "cokoon_spec" )
                    readCokoonSpec();
                else
                    raiseError(QObject::tr("The file is not a Cokoon Specification file."));
            }
        }

//         qDebug() << "spec errors:" << errorString();


        return !error();
    }

    void Specification::readUnknownElement()
    {
        Q_ASSERT(isStartElement());

        while (!atEnd()) {
            readNext();

            if (isEndElement())
                break;

            if (isStartElement())
                readUnknownElement();
        }
    }

    void Specification::readCokoonSpec() {
        Q_ASSERT(isStartElement() && name() == "cokoon_spec");

        m_name = attr("name");
        m_version = attr("version");

        while (!atEnd()) {
            readNext();

            if (isEndElement())
                break;

            if (isStartElement()) {
//                 qDebug() << "cokoon_spec::" << name().toString();
                if (name() == "item")
                    readItem();
                else
                    readUnknownElement();
            }
        }
    }

    QString Specification::attr(const QString &attrName) {
        return attributes().value(attrName).toString();
    }

    void Specification::readItem() {
        Q_ASSERT(isStartElement() && name() == "item");

        SpecificationItem *item = new SpecificationItem();
        item->id = attr("id");

        bool ok;
        const QString pw = attr("preview_width");
        const QString ph = attr("preview_height");
        if (!pw.isEmpty()) {
            item->previewDefaultWidth = pw.toInt(&ok);
            if (!ok) {
                raiseError(QObject::tr("preview_width must be a number."));
            }
        }
        if (!ph.isEmpty()) {
            item->previewDefaultHeight = ph.toInt(&ok);
            if (!ok) {
                raiseError(QObject::tr("preview_height must be a number."));
            }
        }


        while (!atEnd()) {
            readNext();

            if (isEndElement())
                break;

            if (isStartElement()) {
//                 qDebug() << "  item::" << name().toString();
                if (name() == "provided")
                    readProvided(item);
                else if (name() == "required")
                    readRequired(item);
                else if (name() == "states")
                    readStateLevel(item);
                else
                    readUnknownElement();
            }
        }

        m_items.append(item);
    }

    void Specification::readProvided(SpecificationItem *item) {
        Q_ASSERT(item && isStartElement() && name() == "provided");

        while (!atEnd()) {
            readNext();

            if (isEndElement())
                break;

            if (isStartElement()) {
                if (name() == "special_cell")
                    readSpecialCell(item);
                else if (name() == "variable")
                    readVariable(item);
                else
                    readUnknownElement();
            }
        }
    }

    void Specification::readRequired(SpecificationItem *item) {
        Q_ASSERT(item && isStartElement() && name() == "required");

        while (!atEnd()) {
            readNext();

            if (isEndElement())
                break;

            if (isStartElement()) {
                if (name() == "tile")
                    readTile(item);
                else if (name() == "expression")
                    readExpression(item);
                else
                    readUnknownElement();
            }
        }
    }

    void Specification::readSpecialCell(SpecificationItem *item) {
        Q_ASSERT(item && isStartElement() && name() == "special_cell");
        const QString id(attr("id"));
        item->providedSpecialCells.append(id);
        skipElement();
    }

    void Specification::readTile(SpecificationItem *item) {
        Q_ASSERT(item && isStartElement() && name() == "tile");
        const QString id(attr("id"));
        item->requiredTiles.append(id);
        skipElement();
    }

    void Specification::readExpression(SpecificationItem *item) {
        Q_ASSERT(item && isStartElement() && name() == "expression");
        const QString id(attr("id"));
        item->requiredExpressions.append(id);
        skipElement();
    }

    void Specification::readState(SpecificationItemStateLevel *lvl) {
        Q_ASSERT(lvl && isStartElement() && name() == "state");

//         qDebug() << "append state " << attributes().value("id").toString();
        lvl->states.append(attributes().value("id").toString() );

        skipElement();
    }


    void Specification::readVariable(SpecificationItem *item) {
        Q_ASSERT(item && isStartElement() && name() == "variable");
        SpecificationItemVariable *v =
            new SpecificationItemVariable(attributes().value("id").toString(),
                                          attributes().value("type").toString(),
                                          attributes().value("preview_default").toString() );
        item->providedVariables.append(v);
        skipElement();
    }

    void Specification::skipElement()
    {
        Q_ASSERT(isStartElement());
        while (!atEnd()) {
            readNext();
            if (isEndElement())
                break;
            if (isStartElement())
                readUnknownElement();
        }

    }


    void Specification::readStateLevel(SpecificationItem *item) {
        Q_ASSERT(item && isStartElement() && name() == "states");

        SpecificationItemStateLevel *lvl = new SpecificationItemStateLevel();

        while (!atEnd()) {
            readNext();

            if (isEndElement())
                break;

            if (isStartElement()) {
                if (name() == "state")
                    readState(lvl);
                else
                    readUnknownElement();
            }
        }

        item->stateLevels.append(lvl);
    }

    DocumentSpecification::DocumentSpecification()
    {
    }

    DocumentSpecification::~DocumentSpecification()
    {
    }

#define LOGN(x,n) log(x)/log(n)
#define _nextColumnExponent(currentLevelId) (int)ceil(LOGN(currentLevelId+1,2))
#define _columnStateId(n,columnExponent) (int)(n*pow(2,columnExponent))

    bool DocumentSpecification::loadSpecification(const QString &fileName)
    {
        if (!Specification::loadSpecification(fileName))
            return false;

        m_itemToId.clear();
        m_idToItem.clear();
        m_variableToId.clear();
        m_identifierToId.clear();
        m_stateLevelIdExponents.clear();
        m_variables.clear();
        m_identifiers.clear();
        m_enumIdentifiers.clear();

        int itemId = 0;         // keep track of the item ID, counted from 0 up

//         qDebug() << "setSpecification " << spec << "items:" << spec->items();

        foreach(SpecificationItem *i, items()) {
//             qDebug() << "docSpec item" << i->id;
            foreach(SpecificationItemVariable *v, i->providedVariables) {
                addVariableMapping(v->id);
            }
            foreach(QString id, i->providedSpecialCells) {
                addIdentifierMapping(id, "Special");
            }
            foreach(QString id, i->requiredTiles) {
                addIdentifierMapping(id, "Tile");
            }
            foreach(QString id, i->requiredExpressions) {
                addIdentifierMapping(id, "Exp");
            }

            // item mapping
            m_itemToId[i->id] = itemId;


            // item stateLevel exponent mappings...
            QList<int> currentItemExponents;
            int currentLevelStateId = items().size()-1;
            foreach(SpecificationItemStateLevel *stLvl, i->stateLevels) {
//                 qDebug() << "docSpec item stateLvl";
                int curExp = _nextColumnExponent(currentLevelStateId);
                currentItemExponents.append(curExp);
                currentLevelStateId = _columnStateId(stLvl->states.size(),curExp);
            }
            m_stateLevelIdExponents[itemId] = currentItemExponents;
//             qDebug() << "item" << itemId << "append stateLevelIdExponents:" << currentItemExponents;

            itemId += 1;

        }

        return true;
    }

    void DocumentSpecification::addVariableMapping(const QString &id) {
        if (!m_variableToId.contains(id) ) {
            int idx = m_variableToId.size();
            m_variableToId[id] = idx;
            m_variables << id;
        }
    }
    void DocumentSpecification::addIdentifierMapping(const QString &id, const QString &enumPrefix) {
        if (!m_identifierToId.contains(id) ) {
            int idx = m_identifierToId.size();
            m_identifierToId[id] = idx;
            m_identifiers << id;
            m_enumIdentifiers << (enumPrefix + "_" + id);
        }

    }

    int DocumentSpecification::itemStateLevels(int itemId) const
    {
        SpecificationItem *i = items().value(itemId);
        if (i) {
            return i->stateLevels.size();
        } else {
            return -1;
        }
    }
    int DocumentSpecification::itemStateLevelStates(int itemId, int stateLevel) const
    {
        SpecificationItem *i = items().value(itemId);
        if (i) {
            SpecificationItemStateLevel *lvl = i->stateLevels.value(stateLevel);
            if (lvl) {
                return lvl->states.size();
            }
        }
        return -1;
    }
    int DocumentSpecification::mapItemStateToId(int itemId, int stateLevel, const QString &stateName) const
    {
        SpecificationItem *i = items().value(itemId);
        QList<int> exps = m_stateLevelIdExponents.value(itemId);
        if (i) {
            SpecificationItemStateLevel *lvl = i->stateLevels.value(stateLevel);
            if (lvl) {
                int stateIndex = lvl->states.indexOf(stateName);
                if (stateIndex != -1 && stateLevel < exps.size() ) {
                    return _columnStateId(stateIndex,exps[stateLevel]);
                }
            }
        }
        return -1;
    }
    int DocumentSpecification::mapItemStateToId(int itemId, int stateLevel, int stateIndex) const
    {
        SpecificationItem *i = items().value(itemId);
        QList<int> exps = m_stateLevelIdExponents.value(itemId);
        if (i) {
            SpecificationItemStateLevel *lvl = i->stateLevels.value(stateLevel);
            if (lvl) {
                if (stateIndex>=0 && stateIndex<lvl->states.size() &&
                    stateLevel < exps.size() ) {
                    return _columnStateId(stateIndex,exps[stateLevel]);
                }
            }
        }
        return -1;
    }

    int DocumentSpecification::customIdMappingBase(Document::DeclarationType type) const
    {
        switch(type) {
        case Document::ObjectStateDecl:
            return items().size();
        case Document::VariableDecl:
            return m_variableToId.size();
        case Document::IdentifierDecl:
            return m_identifierToId.size();
        case Document::ObjectNameDecl:
            return 0;
        }
    }
    int DocumentSpecification::mapToId(Document::DeclarationType type, const QString &str) const
    {
        switch(type) {
        case Document::ObjectStateDecl:
            if (m_itemToId.contains(str))
                return m_itemToId[str];
            break;
        case Document::VariableDecl:
            if (m_variableToId.contains(str))
                return m_variableToId[str];
            break;
        case Document::IdentifierDecl:
            if (m_identifierToId.contains(str))
                return m_identifierToId[str];
            break;
        case Document::ObjectNameDecl:
            break;
        }
        return -1;
    }


}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
