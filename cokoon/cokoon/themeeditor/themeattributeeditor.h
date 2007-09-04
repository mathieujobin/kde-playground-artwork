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

#ifndef COKOON_THEMEATTRIBUTEEDITOR_H
#define COKOON_THEMEATTRIBUTEEDITOR_H

#include <QWidget>

#include "ui_edit_comment.h"
#include "ui_edit_layout.h"
#include "ui_edit_source.h"
#include "ui_edit_tile.h"
#include "ui_edit_expression.h"
#include "ui_edit_object.h"
#include "ui_edit_theme.h"


class QHBoxLayout;
class QComboBox;
class QLineEdit;

namespace Cokoon {

    class ThemeDomElement;

    class EditWidget : public QWidget
    {
Q_OBJECT
    public:
        EditWidget(ThemeDomElement *el, QWidget *parent);
        ~EditWidget();

        ThemeDomElement *element() const;

    protected:
        void setTextAttrMapping(const QString &attrName,QLineEdit *w);
        void setComboAttrMapping(const QString &attrName,QComboBox *cb, const QStringList &cbValues);

    protected slots:
        void textValueChanged();
        void comboValueChanged();

    private:
        void clear();
        ThemeDomElement *m_element;
        typedef QPair<QString,QLineEdit*> TextAttrMapping;
        typedef QPair<QString,QComboBox*> SelectAttrMapping;
        QList<TextAttrMapping> m_textAttrs;
        QList<SelectAttrMapping> m_selectAttrs;
    };

    class CommentEditor : public QWidget
    {
        Q_OBJECT
    public:
        CommentEditor(ThemeComment *el,QWidget *parent)
            : QWidget(parent), m_el(el)
        {
            m_ui.setupUi(this);
            m_ui.comment->setPlainText(m_el->domNode().nodeValue());
            connect(m_ui.comment,SIGNAL(textChanged()),this,SLOT(commentValueChanged()));
        }
    private slots:
        void commentValueChanged();
    private:
        ThemeComment *m_el;
        Ui_edit_comment m_ui;
    };


class Q_DECL_EXPORT ThemeAttributeEditor : public QWidget
{
    Q_OBJECT
 public:

    ThemeAttributeEditor(QWidget *parent = 0);
    virtual ~ThemeAttributeEditor();

 public slots:
    void setThemeElement(ThemeDomNode *element);

 private:
    void clear();
    QHBoxLayout *m_layout;
    ThemeDomNode *m_currentElement;
    QWidget *m_currentWidget;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
