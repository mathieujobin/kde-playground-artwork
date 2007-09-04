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
#include <QHeaderView>

#include <QFile>
#include <QTextStream>
#include <kstandarddirs.h>

#include "../specification.h"
#include "thememodel.h"
#include "documentspecificationdocument.h"

namespace Cokoon {

    ThemeDomNode::ThemeDomNode(const QDomNode &node, ThemeDomNode *parent)
        : m_node(node), m_parent(parent), m_theme(parent->themeModel())
    {
        qDebug() << "ThemeDomNode(..,..,parent:" << parent << "parent->thememodel:" << parent->themeModel();
    }

    ThemeDomNode::ThemeDomNode(const QDomNode &node, ThemeDomNode *parent, ThemeModel *thememodel)
        : m_node(node), m_parent(parent), m_theme(thememodel)
    {
        qDebug() << "ThemeDomNode(..,..,themeModel:" << thememodel;
    }

    ThemeDomNode::~ThemeDomNode()
    {
        qDeleteAll(m_children);
    }

    QList<ThemeDomNode*> ThemeDomNode::children()
    {
        if (m_children.isEmpty()) {
            QDomNode child = domNode().firstChild();
            int count = 0;
            while (!child.isNull()) {
                ThemeDomNode *n = createChildNode(child);
                if (n!=0) {
//                     qDebug() << " child" << n->displayText();
                    m_children.append(n);
                    ++count;
                }
                child = child.nextSibling();
            }
        }
        return m_children;
    }

    ThemeDomNode *ThemeDomNode::parent() const
    {
        return m_parent;
    }

    ThemeModel *ThemeDomNode::themeModel() const
    {
        return m_theme;
    }

    QDomNode ThemeDomNode::domNode() const
    {
        return m_node;
    }

    QString ThemeDomNode::displayId() const
    {
        return "";
    }

    ThemeDomNode *ThemeDomNode::createChildNode(const QDomNode &node)
    {
        if (node.isComment()) {
            return new ThemeComment(node, this);
        }
        return 0;
    }

    ThemeDomElement::ThemeDomElement(const QDomNode &node, ThemeDomNode *parent, const QString &tag)
        : ThemeDomNode(node, parent)
    {
        Q_ASSERT(node.isElement());
        Q_ASSERT(tag == domElement().tagName());
    }

    ThemeDomElement::ThemeDomElement(const QDomNode &node, ThemeDomNode *parent, ThemeModel *thememodel, const QString &tag)
        : ThemeDomNode(node, parent, thememodel)
    {
        Q_ASSERT(node.isElement());
        Q_ASSERT(tag == domElement().tagName());
    }

    QString ThemeDomElement::displayText() const {
        return domElement().tagName();
    }
    QString ThemeDomElement::displayId() const {
        return getAttr("id");
    }
    QString ThemeDomElement::getAttr(const QString &attrName) const {
        return domElement().attribute(attrName);
    }
    void ThemeDomElement::removeAttr(const QString &attrName)  {
        domElement().removeAttribute(attrName);
        themeModel()->setModified(true);
    }
    void ThemeDomElement::setAttr(const QString &attrName, const QString &value) {
        domElement().setAttribute(attrName,value);
        themeModel()->setModified(true);
    }

    QDomElement ThemeDomElement::domElement() const
    {
        return domNode().toElement();
    }


    ThemeComment::ThemeComment(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomNode(node, parent) {
        Q_ASSERT(node.isComment());
    }
    ThemeSource::ThemeSource(const QDomNode &node, ThemeDomNode *parent)
        : ThemeDomElement(node, parent, "source") {}
    ThemeSourceReplace::ThemeSourceReplace(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "replace_style_attr") {}
    ThemeExpression::ThemeExpression(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "expression") {}
   ThemeTile::ThemeTile(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "tile") {}
    ThemeLayer::ThemeLayer(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "layer") {}
    ThemeLayoutCell::ThemeLayoutCell(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "cell") {}
    ThemeLayoutColumn::ThemeLayoutColumn(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "column") {}
    ThemeLayoutRow::ThemeLayoutRow(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "row") {}
    ThemeLayout::ThemeLayout(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "layout") {}
    ThemePaint::ThemePaint(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "paint") {}
    ThemeObject::ThemeObject(const QDomNode &node, ThemeDomNode *parent)
     : ThemeDomElement(node, parent, "object") {}

    Theme::Theme(const QDomNode &node, ThemeModel *model)
        : ThemeDomElement(node, 0, model, "cokoon_theme")
    {
    }


    ThemeDomNode *ThemeLayoutRow::createChildNode(const QDomNode &node) {
        if (node.isElement() && node.toElement().tagName()=="cell")
                return new ThemeLayoutCell(node, this);
        return ThemeDomElement::createChildNode(node);
    }
    ThemeDomNode *ThemeLayout::createChildNode(const QDomNode &node) {
        if (node.isElement()) {
            if (node.toElement().tagName()=="column")
                return new ThemeLayoutColumn(node, this);
            else if (node.toElement().tagName()=="row")
                return new ThemeLayoutRow(node, this);
        }
        return ThemeDomElement::createChildNode(node);
    }
    ThemeDomNode *ThemeObject::createChildNode(const QDomNode &node) {
        if (node.isElement()) {
            if (node.toElement().tagName()=="source")
                return new ThemeSource(node, this);
            else if (node.toElement().tagName()=="expression")
                return new ThemeExpression(node, this);
            else if (node.toElement().tagName()=="tile")
                return new ThemeTile(node, this);
            else if (node.toElement().tagName()=="layout")
                return new ThemeLayout(node, this);
            else if (node.toElement().tagName()=="paint")
                return new ThemePaint(node, this);
        }
        return ThemeDomElement::createChildNode(node);
    }
    ThemeDomNode *ThemeSource::createChildNode(const QDomNode &node)
    {
        if (node.isElement() && node.toElement().tagName()=="replace_style_attr")
            return new ThemeSourceReplace(node, this);
        return ThemeDomElement::createChildNode(node);
    }
    ThemeDomNode *ThemePaint::createChildNode(const QDomNode &node)
    {
        if (node.isElement() && node.toElement().tagName()=="layer")
            return new ThemeLayer(node, this);
        return ThemeDomElement::createChildNode(node);
    }
    ThemeDomNode *Theme::createChildNode(const QDomNode &node) {
        if (node.isElement() && node.toElement().tagName()=="object")
            return new ThemeObject(node, this);
        return ThemeDomElement::createChildNode(node);
    }

    QStringList Theme::objects() {
        QStringList o;
        foreach(ThemeDomNode *c,children()) {
            const ThemeObject *to = dynamic_cast<const ThemeObject*>(c);
            if (to) {
                o << to->getAttr("id");
            }
        }
        return o;
    }


    ThemeModel::ThemeModel()
        : m_modified(false),m_theme(0),m_themeSpecification(0),m_cokoonDocument(0)
    {
    }

    ThemeModel::~ThemeModel()
    {
        clear();
    }

    void ThemeModel::clear()
    {
        m_currentFileName = QString();
        delete m_theme;
        delete m_themeSpecification;
        delete m_cokoonDocument;
        m_theme = 0;
        m_themeSpecification = 0;
        m_cokoonDocument = 0;
    }

    bool ThemeModel::loadTheme(const QString &fileName)
    {
        clear();

        m_currentFileName = fileName;
        QFile file(fileName);

        if (! file.exists() ) {
            qCritical("File '%s' does not exist.",
                      qPrintable(fileName) );
            return false;
        }

//         if (!file.open(QFile::ReadOnly | QFile::Text)) {
//             qCritical("File '%s' can not be opened: %s", qPrintable(fileName), qPrintable(file.errorString()));
//             return false;
//         }

        if (!m_domDocument.setContent(&file))
            return false;

        m_theme = new Theme(m_domDocument.documentElement(),this);

        QString specResource("cokoon/specs/"+m_theme->spec()+".xml");
        QString specFileName = KStandardDirs::locate("data",specResource);
        qDebug() << specResource << specFileName;

        m_themeSpecification = new DocumentSpecification();
        if (!m_themeSpecification->loadSpecification(specFileName)) {
            return false;
        }

        m_cokoonDocument = new DocumentSpecificationDocument(m_themeSpecification);
//         m_cokoonDocument->loadTheme(fileName);

        setModified(false);

        return true;
    }

    bool ThemeModel::save(const QString &fileName)
    {
        m_currentFileName = fileName;
        QFile file(fileName);

        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            qCritical("File '%s' can not be opened: %s", qPrintable(fileName), qPrintable(file.errorString()));
            return false;
        }

        QTextStream stream(&file);
        stream << m_domDocument.toString();
        return true;
    }

    const QString &ThemeModel::currentFileName() const
    {
        return m_currentFileName;
    }

    Theme *ThemeModel::theme()
    {
        return m_theme;
    }

    DocumentSpecification *ThemeModel::specification()
    {
        return m_themeSpecification;
    }

    Document *ThemeModel::cokoonDocument()
    {
        return m_cokoonDocument;
    }

    void ThemeModel::setModified(bool modified)
    {
        m_modified = modified;
        m_cokoonDocument->clear();
        m_cokoonDocument->loadTheme(m_domDocument.toString(), m_currentFileName);
        emit modelModified();
    }

    bool ThemeModel::modified() const
    {
        return m_modified;
    }

    ThemeItemTreeModel::ThemeItemTreeModel()
        :m_theme(0)
    {
    }
    ThemeItemTreeModel::~ThemeItemTreeModel()
    {
    }

    void ThemeItemTreeModel::setModel(ThemeModel *model)
    {
        m_theme = model;
        connect(m_theme,SIGNAL(modelModified()),this,SLOT(slotReset()));
    }

    void ThemeItemTreeModel::slotReset()
    {
        reset();
    }

    QVariant ThemeItemTreeModel::headerData ( int section, Qt::Orientation orientation, int role ) const
    {
        if (orientation==Qt::Horizontal && role==Qt::DisplayRole) {
            switch(section) {
            case 0:
                return "Element";
            case 1:
                return "ID";
            default:
                return QVariant();
            }
        }
        return QVariant();
    }
    Qt::ItemFlags ThemeItemTreeModel::flags ( const QModelIndex & index ) const
    {
        if (!index.isValid())
            return Qt::ItemIsEnabled;
        else
            return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
    }
    QVariant ThemeItemTreeModel::data ( const QModelIndex & index, int role ) const
    {
        if (index.isValid() && role==Qt::DisplayRole) {
            const ThemeDomNode *item = static_cast<const ThemeDomNode *>(index.internalPointer());
            if (item) {
                switch(index.column()) {
                case 0:
                    return item->displayText();
                case 1:
                    return item->displayId();
                default:
                    break;
                }
            }
        }
        return QVariant();
    }
    QModelIndex ThemeItemTreeModel::index ( int row, int column, const QModelIndex & parent ) const
    {
        if (!m_theme || !m_theme->theme())
            return QModelIndex();

        if (!parent.isValid()) {
            m_theme->theme()->itemModelRow = 0;
            return createIndex(row,column,m_theme->theme());
        } else {
            ThemeDomNode *parentItem =
                static_cast<ThemeDomNode *>(parent.internalPointer());
            ThemeDomNode *childItem = parentItem->children().value(row);
            if (childItem) {
                childItem->itemModelRow = row;
                return createIndex(row,column,childItem);
            }
            else
                return QModelIndex();
        }
    }
    QModelIndex ThemeItemTreeModel::parent( const QModelIndex & index ) const
    {
        if (index.isValid()) {
            const ThemeDomNode *item = static_cast<const ThemeDomNode*>(index.internalPointer());
            if (item) {
                ThemeDomNode *parent = item->parent();
                if (parent)
                    return createIndex(parent->itemModelRow,0,parent);
            }
        }
        return QModelIndex();
    }
    int ThemeItemTreeModel::columnCount ( const QModelIndex & /*parent*/ ) const
    {
        return 2;
    }
    int ThemeItemTreeModel::rowCount ( const QModelIndex & parent ) const
    {
        if (parent.isValid()) {
            ThemeDomNode *themeItem = static_cast<ThemeDomNode *>(parent.internalPointer());
            if (themeItem)
                return themeItem->children().size();
        }
        return 1;
    }


}

#include "thememodel.moc"
