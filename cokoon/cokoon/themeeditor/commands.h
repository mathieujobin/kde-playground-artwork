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

#ifndef COKOON_COMMANDS_H
#define COKOON_COMMANDS_H

#include <QDomNode>
#include <QDomElement>
#include <QDomDocument>
#include <QAbstractItemModel>
#include <QUndoCommand>

namespace Cokoon {

    class ThemeModel;

    class ModelCommand : public QUndoCommand
    {
    public:
        ModelCommand(ThemeModel* model);
        virtual ~ModelCommand();
        ThemeModel *model() const;
    private:
        ThemeModel *m_model;
    };

    class EditAttributeCommand : public ModelCommand
    {
    public:
        EditAttributeCommand(ThemeModel* model, const QString &domNodePath, const QString &attrName, const QString &newValue);
        virtual void undo();
        virtual void redo();

        const QString &oldValue() const;
        const QString &newValue() const;
        const QString &domNodePath() const;
    private:
        QString m_path;
        QString m_attr;
        QString m_newVal;
        QString m_oldVal;
    };

    class RemoveAttributeCommand : public ModelCommand
    {
    public:
        RemoveAttributeCommand(ThemeModel* model, const QString &domNodePath, const QString &attrName);
        virtual void undo();
        virtual void redo();
    private:
        QString m_path;
        QString m_attr;
        QString m_newVal;
        QString m_oldVal;
    };

    class EditObjectIdCommand : public EditAttributeCommand
    {
    public:
        EditObjectIdCommand(ThemeModel* model, const QString &domNodePath, const QString &newValue);
        virtual void undo();
        virtual void redo();
    };

    class DeleteNodeCommand : public ModelCommand
    {
    public:
        DeleteNodeCommand(ThemeModel* model, const QString &domNodePath);
        virtual void undo();
        virtual void redo();
    private:
        QString m_parentPath;
        int m_childIndex;
        QDomNode m_oldNode;
    };

    class InsertNodeCommand : public ModelCommand
    {
    public:
        InsertNodeCommand(ThemeModel* model, const QString &parentDomNodePath, int childIndex, QDomNode newNode);
        virtual void undo();
        virtual void redo();
    private:
        QString m_parentPath;
        int m_childIndex;
        QDomNode m_newNode;
    };

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
