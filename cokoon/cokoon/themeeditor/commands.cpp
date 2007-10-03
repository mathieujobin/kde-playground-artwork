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
#include "klocale.h"

#include "commands.h"
#include "thememodel.h"

namespace Cokoon {

    ModelCommand::ModelCommand(ThemeModel* model)
        : m_model(model)
    {
        Q_ASSERT(model);
    }
    ModelCommand::~ModelCommand()
    {
    }
    ThemeModel *ModelCommand::model() const
    {
        return m_model;
    }

    EditAttributeCommand::EditAttributeCommand(ThemeModel* mdl, const QString &domNodePath, const QString &attrName, const QString &newValue)
        : ModelCommand(mdl), m_path(domNodePath), m_attr(attrName), m_newVal(newValue)
    {
        setText(i18n("Edit Attribute %1").arg(m_attr));

        ThemeDomElement *el = model()->getThemeElement(m_path);
        if (el) {
            m_oldVal = el->getAttr(m_attr);
        }
    }
    void EditAttributeCommand::undo()
    {
        ThemeDomElement *el = model()->getThemeElement(m_path);
        if (el) {
            el->setAttr(m_attr,m_oldVal);
        }
    }
    void EditAttributeCommand::redo()
    {
        ThemeDomElement *el = model()->getThemeElement(m_path);
        if (el) {
            el->setAttr(m_attr,m_newVal);
        }
    }

    const QString &EditAttributeCommand::oldValue() const
    {
        return m_oldVal;
    }
    const QString &EditAttributeCommand::newValue() const
    {
        return m_newVal;
    }
    const QString &EditAttributeCommand::domNodePath() const
    {
        return m_path;
    }

    RemoveAttributeCommand::RemoveAttributeCommand(ThemeModel* mdl, const QString &domNodePath, const QString &attrName)
        : ModelCommand(mdl), m_path(domNodePath), m_attr(attrName)
    {
        setText(i18n("Remove Attribute %1").arg(m_attr));

        ThemeDomElement *el = model()->getThemeElement(m_path);
        if (el) {
            m_oldVal = el->getAttr(m_attr);
        }
    }
    void RemoveAttributeCommand::undo()
    {
        ThemeDomElement *el = model()->getThemeElement(m_path);
        if (el) {
            el->setAttr(m_attr,m_newVal);
        }
    }
    void RemoveAttributeCommand::redo()
    {
        ThemeDomElement *el = model()->getThemeElement(m_path);
        if (el) {
            el->removeAttr(m_attr);
        }
    }

    EditObjectIdCommand::EditObjectIdCommand(ThemeModel* model, const QString &domNodePath, const QString &newValue)
        : EditAttributeCommand(model,domNodePath,"id",newValue)
    {
        // TODO!
    }
    void EditObjectIdCommand::undo()
    {
        EditAttributeCommand::undo();
    }
    void EditObjectIdCommand::redo()
    {
        EditAttributeCommand::redo();
    }

    DeleteNodeCommand::DeleteNodeCommand(ThemeModel* mdl, const QString &domNodePath)
        : ModelCommand(mdl)
    {
        ThemeDomNode *n = model()->getThemeNode(domNodePath);
        if(n) {
            m_oldNode = n->domNode();
            if (n->parent()) {
                m_parentPath = n->parent()->nodePath();
                m_childIndex = n->parent()->children().indexOf(n);
            } else {
                m_parentPath = "";
                m_childIndex = -1;
            }
        }
    }
    void DeleteNodeCommand::undo()
    {
        ThemeDomNode *o = model()->getThemeNode(m_parentPath);
        if(o) {
            o->insertChild(m_childIndex,m_oldNode);
        }
    }
    void DeleteNodeCommand::redo()
    {
        ThemeDomNode *o = model()->getThemeNode(m_parentPath);
        if(o) {
            o->deleteChild(m_childIndex);
        }
    }


    InsertNodeCommand::InsertNodeCommand(ThemeModel* mdl, const QString &parentDomNodePath, int childIndex, QDomNode newNode)
        : ModelCommand(mdl), m_parentPath(parentDomNodePath),
          m_childIndex(childIndex), m_newNode(newNode)
    {
    }
    void InsertNodeCommand::undo()
    {
        ThemeDomNode *o = model()->getThemeNode(m_parentPath);
        if(o) {
            o->deleteChild(m_childIndex);
        }
    }
    void InsertNodeCommand::redo()
    {
        ThemeDomNode *o = model()->getThemeNode(m_parentPath);
        if(o) {
            o->insertChild(m_childIndex,m_newNode);
        }
    }

}

