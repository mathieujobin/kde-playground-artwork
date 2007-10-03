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
#include <qundostack.h>

namespace Cokoon {

    class Document;
    class DocumentSpecification;
    class DocumentSpecificationDocument;
    class Theme;
    class ThemeModel;

    class ThemeDomNode
{
 public:
    ThemeDomNode(const QDomNode &node, const ThemeDomNode *parent, ThemeModel *themeModel);
    ThemeDomNode(const QDomNode &node, const ThemeDomNode *parent);
    virtual ~ThemeDomNode();
    QList<ThemeDomNode*> children() const;
    virtual QDomNode domNode() const;
    const ThemeDomNode *parent() const;
    ThemeModel *themeModel() const;
    virtual QString displayText() const = 0;
    virtual QString displayId() const;
    /**
     * Factory method for subclasses
     */
    virtual ThemeDomNode *createChildNode(const QDomNode &node) const;

    int childIndex() const;
    QString nodePath() const;
    QModelIndex modelIndex(int column = 0) const;

    QDomNode deleteChild(int index);
    bool insertChild(int index, const QDomNode &node);

 private:
    void invalidateCache();
    QDomNode m_node;
    const ThemeDomNode *m_parent;
    ThemeModel *m_theme;
    mutable QList<ThemeDomNode*> m_children;
};

 class ThemeDomElement : public ThemeDomNode
{
 public:
    ThemeDomElement(const QDomNode &node, const ThemeDomNode *parent, ThemeModel *themeModel, const QString &tag);
    ThemeDomElement(const QDomNode &node, const ThemeDomNode *parent, const QString &tag);
    QString displayText() const;
    QString displayId() const;

    QString getAttr(const QString &attrName) const;
    void setAttr(const QString &attrName, const QString &value);
    void removeAttr(const QString &attrName);
    virtual QDomElement domElement() const;

    ThemeDomElement *getElement(const QString &nodePath) const;
};

 class ThemeComment : public ThemeDomNode
{
 public:
    ThemeComment(const QDomNode &node, const ThemeDomNode *parent);
    QString displayText() const {
        return "# Comment";
    }
};

 class ThemeSourceReplace : public ThemeDomElement
{
 public:
    ThemeSourceReplace(const QDomNode &node, const ThemeDomNode *parent);
};

 class ThemeSource : public ThemeDomElement
{
 public:
    ThemeSource(const QDomNode &node, const ThemeDomNode *parent);
    virtual ThemeDomNode *createChildNode(const QDomNode &node) const;
};

 class ThemeExpression : public ThemeDomElement
{
 public:
    ThemeExpression(const QDomNode &node, const ThemeDomNode *parent);
};

 class ThemeTile : public ThemeDomElement
{
 public:
    ThemeTile(const QDomNode &node, const ThemeDomNode *parent);
};

 class ThemeLayer : public ThemeDomElement
{
 public:
    ThemeLayer(const QDomNode &node, const ThemeDomNode *parent);
};

 class ThemeLayoutCell : public ThemeDomElement
{
 public:
    ThemeLayoutCell(const QDomNode &node, const ThemeDomNode *parent);
};

 class ThemeLayoutColumn : public ThemeDomElement
{
 public:
    ThemeLayoutColumn(const QDomNode &node, const ThemeDomNode *parent);
};

 class ThemeLayoutRow : public ThemeDomElement
{
 public:
    ThemeLayoutRow(const QDomNode &node, const ThemeDomNode *parent);
    virtual ThemeDomNode *createChildNode(const QDomNode &node) const;
};

 class ThemeLayout : public ThemeDomElement
{
 public:
    ThemeLayout(const QDomNode &node, const ThemeDomNode *parent);
    virtual ThemeDomNode *createChildNode(const QDomNode &node) const;
};

 class ThemePaint : public ThemeDomElement
{
 public:
    ThemePaint(const QDomNode &node, const ThemeDomNode *parent);
    ThemeDomNode *createChildNode(const QDomNode &node) const;
};

 class ThemeObject : public ThemeDomElement
{
 public:
    ThemeObject(const QDomNode &node, const ThemeDomNode *parent);
    ThemeDomNode *createChildNode(const QDomNode &node) const;
};

 class Theme : public ThemeDomElement
{
 public:
    Theme(const QDomNode &node, ThemeModel *model);
    virtual ThemeDomNode *createChildNode(const QDomNode &node) const;
    QString name() { return getAttr("name"); }
    QString spec() { return getAttr("spec"); }
    QString version() { return getAttr("version"); }
    QStringList objects();
};

    class ThemeModel : public QAbstractItemModel
{
    Q_OBJECT
 public:
    ThemeModel();
    virtual ~ThemeModel();
    void clear();
    bool loadTheme(const QString &fileName);
    bool save(const QString &fileName);
    const QString &currentFileName() const;

    Theme *theme() const;
    ThemeDomNode *getThemeNode(const QString &nodePath) const;
    ThemeDomElement *getThemeElement(const QString &nodePath) const;
    QModelIndex getNodeModelIndex(const QString &nodePath) const;

    DocumentSpecification *specification();
    Document *cokoonDocument();

    bool modified() const;
    void setModified(bool modified);
    void setThemeNodeEdited(const QString &domNodePath);

    QUndoStack &undoStack();

    // theme modification...
    QDomNode deleteChild(int index);
    /**
     * Deletes a theme item. Keeps undo information.
     * @returns The child's old QDomNode representation
     */
    bool deleteNode(const QString &domNodePath);
    bool moveNodeUp(const QString &domNodePath);
    bool moveNodeDown(const QString &domNodePath);

    // QAbstractItemModel interface...
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

 signals:
    void modelModified();
    void themeNodeEdited(const QString &domNodePath);
    void themeNodeInserted(const QString &parentDomNodePath,int index);
    void themeNodeDeleted(const QString &parentDomNodePath,int index);
 protected slots:
    void currentFileModifiedOnDisk(const QString &path);
 protected:
    friend class DeleteNodeCommand;
    friend class InsertNodeCommand;
    friend class ThemeDomNode;

    void activateDirWatch();
    void deactivateDirWatch();

/*     QDomNode deleteNodeChild(const QString &parentNodePath, int childIndex); */
/*     bool insertNodeChild(const QString &parentNodePath, int childIndex, QDomNode node); */

    bool m_modified;
    Theme *m_theme;
    DocumentSpecification *m_themeSpecification;
    DocumentSpecificationDocument *m_cokoonDocument;
    QDomDocument m_domDocument;
    QString m_currentFileName;

    QString m_dirWatchFile;

    QUndoStack m_undoStack;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
