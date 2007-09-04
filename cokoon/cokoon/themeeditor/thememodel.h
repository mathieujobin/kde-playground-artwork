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

#ifndef COKOON_THEMEMODEL_H
#define COKOON_THEMEMODEL_H

#include <QDomNode>
#include <QDomElement>
#include <QDomDocument>
#include <QAbstractItemModel>

namespace Cokoon {

    class Document;
    class DocumentSpecification;
    class DocumentSpecificationDocument;
    class Theme;
    class ThemeModel;

    class ThemeDomNode
{
 public:
    ThemeDomNode(const QDomNode &node, ThemeDomNode *parent, ThemeModel *themeModel);
    ThemeDomNode(const QDomNode &node, ThemeDomNode *parent);
    virtual ~ThemeDomNode();
    QList<ThemeDomNode*> children();
    virtual QDomNode domNode() const;
    ThemeDomNode *parent() const;
    ThemeModel *themeModel() const;
    virtual QString displayText() const = 0;
    virtual QString displayId() const;
    virtual ThemeDomNode *createChildNode(const QDomNode &node);

    int itemModelRow;

 private:
    QDomNode m_node;
    ThemeDomNode *m_parent;
    ThemeModel *m_theme;
    mutable QList<ThemeDomNode*> m_children;
};

 class ThemeDomElement : public ThemeDomNode
{
 public:
    ThemeDomElement(const QDomNode &node, ThemeDomNode *parent, ThemeModel *themeModel, const QString &tag);
    ThemeDomElement(const QDomNode &node, ThemeDomNode *parent, const QString &tag);
    QString displayText() const;
    QString displayId() const;

    QString getAttr(const QString &attrName) const;
    void setAttr(const QString &attrName, const QString &value);
    void removeAttr(const QString &attrName);
    virtual QDomElement domElement() const;
};

 class ThemeComment : public ThemeDomNode
{
 public:
    ThemeComment(const QDomNode &node, ThemeDomNode *parent);
    QString displayText() const {
        return "# Comment";
    }
};

 class ThemeSourceReplace : public ThemeDomElement
{
 public:
    ThemeSourceReplace(const QDomNode &node, ThemeDomNode *parent);
};

 class ThemeSource : public ThemeDomElement
{
 public:
    ThemeSource(const QDomNode &node, ThemeDomNode *parent);
    virtual ThemeDomNode *createChildNode(const QDomNode &node);
};

 class ThemeExpression : public ThemeDomElement
{
 public:
    ThemeExpression(const QDomNode &node, ThemeDomNode *parent);
};

 class ThemeTile : public ThemeDomElement
{
 public:
    ThemeTile(const QDomNode &node, ThemeDomNode *parent);
};

 class ThemeLayer : public ThemeDomElement
{
 public:
    ThemeLayer(const QDomNode &node, ThemeDomNode *parent);
};

 class ThemeLayoutCell : public ThemeDomElement
{
 public:
    ThemeLayoutCell(const QDomNode &node, ThemeDomNode *parent);
};

 class ThemeLayoutColumn : public ThemeDomElement
{
 public:
    ThemeLayoutColumn(const QDomNode &node, ThemeDomNode *parent);
};

 class ThemeLayoutRow : public ThemeDomElement
{
 public:
    ThemeLayoutRow(const QDomNode &node, ThemeDomNode *parent);
    virtual ThemeDomNode *createChildNode(const QDomNode &node);
};

 class ThemeLayout : public ThemeDomElement
{
 public:
    ThemeLayout(const QDomNode &node, ThemeDomNode *parent);
    virtual ThemeDomNode *createChildNode(const QDomNode &node);
};

 class ThemePaint : public ThemeDomElement
{
 public:
    ThemePaint(const QDomNode &node, ThemeDomNode *parent);
    ThemeDomNode *createChildNode(const QDomNode &node);
};

 class ThemeObject : public ThemeDomElement
{
 public:
    ThemeObject(const QDomNode &node, ThemeDomNode *parent);
    ThemeDomNode *createChildNode(const QDomNode &node);
};

 class Theme : public ThemeDomElement
{
 public:
    Theme(const QDomNode &node, ThemeModel *model);
    virtual ThemeDomNode *createChildNode(const QDomNode &node);
    QString name() { return getAttr("name"); }
    QString spec() { return getAttr("spec"); }
    QString version() { return getAttr("version"); }
    QStringList objects();
};

    class ThemeModel : public QObject
{
    Q_OBJECT
 public:
    ThemeModel();
    virtual ~ThemeModel();
    void clear();
    bool loadTheme(const QString &fileName);
    bool save(const QString &fileName);
    const QString &currentFileName() const;

    Theme *theme();
    DocumentSpecification *specification();
    Document *cokoonDocument();

    bool modified() const;
    void setModified(bool modified = true);
 signals:
    void modelModified();
 protected:
    bool m_modified;
    Theme *m_theme;
    DocumentSpecification *m_themeSpecification;
    DocumentSpecificationDocument *m_cokoonDocument;
    QDomDocument m_domDocument;
    QString m_currentFileName;
};

 class ThemeItemTreeModel : public QAbstractItemModel
{
Q_OBJECT
 public:
    ThemeItemTreeModel();
    virtual ~ThemeItemTreeModel();

    void setModel(ThemeModel *model);

    // QAbstractItemModel interface...
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
 public slots:
    void slotReset();
 private:
    ThemeModel *m_theme;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
