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

#ifndef COKOON_SPECIFICATION_H
#define COKOON_SPECIFICATION_H

#include <QString>
#include <QHash>
#include <QXmlStreamReader>
#include <QIODevice>
#include "document.h"
#include <QStringList>

class QPainter;

namespace Cokoon {

class Q_DECL_EXPORT SpecificationItemVariable
{
 public:
    SpecificationItemVariable(const QString &i,const QString &t, const QString &d)
        : id(i),type(t),previewDefaultValue(d) {};
    virtual ~SpecificationItemVariable();

    QString id;
    QString type;
    QString previewDefaultValue;
};

class Q_DECL_EXPORT SpecificationItemStateLevel
{
 public:
    SpecificationItemStateLevel() {};
    virtual ~SpecificationItemStateLevel();

    QList<QString> states;
};

class Q_DECL_EXPORT SpecificationItem
{
 public:
    SpecificationItem() {};
    virtual ~SpecificationItem();

    QString id;
    int previewDefaultWidth;
    int previewDefaultHeight;

    QList<SpecificationItemVariable*> providedVariables;
    QList<QString> providedSpecialCells;
    QList<QString> requiredExpressions;
    QList<QString> requiredTiles;
    QList<SpecificationItemStateLevel*> stateLevels;
};

/**
 * Contains the data of a theme specification.
 * This data is used mainly for inspecting specifications (e.g. in theme editor
 * applications, in order to find out which items there are), and to implement
 * the Document specification interface.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class Q_DECL_EXPORT Specification : protected QXmlStreamReader
{
 public:

    /**
     * Construct.
     */
    Specification();
    /**
     * Destruct.
     */
    virtual ~Specification();

    /**
     * Read XML file @p fileName and construct a theme specification from it.
     */
    bool loadSpecification(QIODevice *device);

    /**
     * Read XML file @p fileName and construct a theme specification from it.
     */
    bool loadSpecification(const QString &fileName);

    /**
     * Deletes all items.
     */
    void clear();

    /**
     * List of all specification items.
     */
    QList<SpecificationItem*> items() const;

    /**
     * Specification name.
     */
    const QString &specName() const;

    /**
     * Specification version.
     */
    const QString &specVersion() const;

 protected:
    void readUnknownElement();
    void readCokoonSpec();
    void readItem();
    void readProvided(SpecificationItem *item);
    void readRequired(SpecificationItem *item);
    void readStateLevel(SpecificationItem *item);
    void readSpecialCell(SpecificationItem *item);
    void readVariable(SpecificationItem *item);
    void readTile(SpecificationItem *item);
    void readExpression(SpecificationItem *item);
    void readState(SpecificationItemStateLevel *statelevel);
    void skipElement();
 private:
    QString attr(const QString &attrName);

    QList<SpecificationItem*> m_items;
    QString m_name;
    QString m_version;
};

/**
 * Translates the data gathered by Specification so that it's
 * suitable for the interface of Document.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class Q_DECL_EXPORT DocumentSpecification
{
 public:

    /**
     * Construct.
     */
    DocumentSpecification(const Specification *spec);
    /**
     * Destruct.
     */
    virtual ~DocumentSpecification();

    void setSpecification(const Specification *spec);

    const QStringList &variables() const;
    const QStringList &identifiers() const;
    /**
     * How the identifiers will be named in the compiled C++ header file
     */
    const QStringList &enumIdentifiers() const;

    int itemStateLevels(int itemId) const;
    int itemStateLevelStates(int itemId, int stateLevel) const;
    int mapItemStateToId(int itemId, int stateLevel, const QString &stateName) const;
    int mapItemStateToId(int itemId, int stateLevel, int stateIndex) const;
    int customIdMappingBase(Document::DeclarationType type) const;
    int mapToId(Document::DeclarationType type, const QString &str) const;

 private:
    void addVariableMapping(const QString &id);
    void addIdentifierMapping(const QString &id, const QString &enumPrefix);

    int nextColumnBase(int currentColumnId);
    int culumnItemId(int n, int columnBase);

    const Specification *m_spec;

    //// mappings based on m_items...
    QStringList m_variables;
    QStringList m_identifiers;
    QStringList m_enumIdentifiers;

    QMap<QString,int> m_itemToId;            // Item Id/Name <--> specItemId
    QMap<int,SpecificationItem*> m_idToItem; // specId <--> Item
    QMap<QString,int> m_variableToId;        /* Variable Id <--> specVariableId */
    QMap<QString,int> m_identifierToId;      /* Identifier Id <--> specIdentifierId */
    QMap<int,QList<int> > m_stateLevelIdExponents; /* specItemId <--> Item Exponent list */
};


}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
