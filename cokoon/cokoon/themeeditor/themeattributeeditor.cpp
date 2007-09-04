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

#include "thememodel.h"
#include "themeattributeeditor.h"

namespace Cokoon {

    EditWidget::EditWidget(ThemeDomElement *el, QWidget *parent)
        : QWidget(parent), m_element(el)
    {
    }

    EditWidget::~EditWidget()
    {
        clear();
    }

    ThemeDomElement *EditWidget::element() const
    {
        return m_element;
    }

    void EditWidget::clear()
    {
        foreach(TextAttrMapping m,m_textAttrs) {
            delete m.second;
        }
        foreach(SelectAttrMapping m,m_selectAttrs) {
            delete m.second;
        }
        m_textAttrs.clear();
        m_selectAttrs.clear();
    }

    void EditWidget::setTextAttrMapping(const QString &attrName,QLineEdit *w)
    {
        m_textAttrs.append(TextAttrMapping(attrName,w));
        w->setText(m_element->getAttr(attrName));
        connect(w,SIGNAL(editingFinished()),this,SLOT(textValueChanged()));
    }
    void EditWidget::setComboAttrMapping(const QString &attrName,QComboBox *cb, const QStringList &cbValues)
    {
        m_selectAttrs.append(SelectAttrMapping(attrName,cb));
        QString currentValue = m_element->getAttr(attrName);
        int curIndex = 0;
        foreach(const QString &s, cbValues) {
            cb->addItem(s);
            if (s == currentValue)
                curIndex = cb->count()-1;
        }
        cb->setCurrentIndex(curIndex);
        connect(cb,SIGNAL(activated(int)),this,SLOT(comboValueChanged()));
    }

    void EditWidget::textValueChanged()
    {
        foreach(const TextAttrMapping &m,m_textAttrs) {
            const QString &name = m.first;
            const QString &text = m.second->text();
            if (m_element->getAttr(name)!=text) {
                m_element->setAttr(name,text);
            }
        }
    }
    void EditWidget::comboValueChanged()
    {
        foreach(const SelectAttrMapping &m,m_selectAttrs) {
            const QString &name = m.first;
            const QString &text = m.second->currentText();
            if (m_element->getAttr(name)!=text) {
                if (text.isEmpty()) {
                    m_element->removeAttr(name);
                } else {
                    m_element->setAttr(name,text);
                }
            }
        }
    }

    class ObjectEditor : public EditWidget
    {
    public:
        ObjectEditor(ThemeDomElement *el,QWidget *parent)
            : EditWidget(el,parent)
        {
            m_ui.setupUi(this);
            setTextAttrMapping("id",m_ui.identifier);
            setTextAttrMapping("implement",m_ui.implement);
            QStringList extendObjs = element()->themeModel()->theme()->objects();
            extendObjs.insert(0, "");
            setComboAttrMapping("extend",m_ui.extend,extendObjs);
        }
    private:
        Ui_edit_object m_ui;
    };

    class ThemeEditor : public EditWidget
    {
    public:
        ThemeEditor(ThemeDomElement *el,QWidget *parent)
            : EditWidget(el,parent)
        {
            m_ui.setupUi(this);
            setTextAttrMapping("name",m_ui.name);
            setTextAttrMapping("version",m_ui.version);
            setTextAttrMapping("spec",m_ui.spec);
        }
    private:
        Ui_edit_theme m_ui;
    };

    class TileEditor : public EditWidget
    {
    public:
        TileEditor(ThemeDomElement *el,QWidget *parent)
            : EditWidget(el,parent)
        {
            m_ui.setupUi(this);
            setTextAttrMapping("id",m_ui.identifier);
            setTextAttrMapping("top",m_ui.top);
            setTextAttrMapping("left",m_ui.left);
            setTextAttrMapping("width",m_ui.width);
            setTextAttrMapping("height",m_ui.height);
            // TODO: source object/id; hor/vert modes
//             QStringList sourceObjs;
//             sourceObjs << "";
//             foreach(const QString &s, element()->themeModel()->theme()->objects()) {
//                 if (s!=
//                 sourceObjs << s;
//             }
//             QStringList sourceIds;
        }
    private:
        Ui_edit_tile m_ui;
    };

    class ExpressionEditor : public EditWidget
    {
    public:
        ExpressionEditor(ThemeDomElement *el,QWidget *parent)
            : EditWidget(el,parent)
        {
            m_ui.setupUi(this);
            setTextAttrMapping("id",m_ui.identifier);
            setTextAttrMapping("value",m_ui.value);
        }
    private:
        Ui_edit_expression m_ui;
    };

    class LayoutEditor : public EditWidget
    {
    public:
        LayoutEditor(ThemeDomElement *el,QWidget *parent)
            : EditWidget(el,parent)
        {
            m_ui.setupUi(this);
            // TODO
//             setTextAttrMapping("name",m_ui.name);
//             setTextAttrMapping("version",m_ui.version);
//             setTextAttrMapping("spec",m_ui.spec);
        }
    private:
        Ui_edit_layout m_ui;
    };

    class SourceEditor : public EditWidget
    {
    public:
        SourceEditor(ThemeDomElement *el,QWidget *parent)
            : EditWidget(el,parent)
        {
            m_ui.setupUi(this);
            setTextAttrMapping("id",m_ui.identifier);
            setTextAttrMapping("file",m_ui.file);
            QStringList types;
            types << "svg" << "pixmap" << "bitmap";
            setComboAttrMapping("type",m_ui.type,types);
        }
    private:
        Ui_edit_source m_ui;
    };


    void CommentEditor::commentValueChanged()
    {
        m_el->domNode().setNodeValue(m_ui.comment->toPlainText());
    }

    ThemeAttributeEditor::ThemeAttributeEditor(QWidget *parent)
        : QWidget(parent), m_currentElement(0), m_currentWidget(0)
    {
        m_layout = new QHBoxLayout(this);
    }
    ThemeAttributeEditor::~ThemeAttributeEditor()
    {
        clear();
    }

    void ThemeAttributeEditor::clear()
    {
        if (m_currentWidget)
            m_layout->removeWidget(m_currentWidget);
        delete m_currentWidget;
        m_currentWidget = 0;
    }

    void ThemeAttributeEditor::setThemeElement(ThemeDomNode *element)
    {
        qDebug() << "ThemeAttributeEditor::set" << element;

        clear();

        if (!element) {
            return;
        }

        if (ThemeComment *c = dynamic_cast<ThemeComment*>(element)) {
            m_currentWidget = new CommentEditor(c,this);
        } else if (ThemeObject *o = dynamic_cast<ThemeObject*>(element)) {
            m_currentWidget = new ObjectEditor(o,this);
        } else if (ThemeTile *o = dynamic_cast<ThemeTile*>(element)) {
            m_currentWidget = new TileEditor(o,this);
        } else if (ThemeExpression *o = dynamic_cast<ThemeExpression*>(element)) {
            m_currentWidget = new ExpressionEditor(o,this);
        } else if (ThemeLayout *o = dynamic_cast<ThemeLayout*>(element)) {
            m_currentWidget = new LayoutEditor(o,this);
        } else if (ThemeSource *o = dynamic_cast<ThemeSource*>(element)) {
            m_currentWidget = new SourceEditor(o,this);
        } else if (Theme *o = dynamic_cast<Theme*>(element)) {
            m_currentWidget = new ThemeEditor(o,this);
        }

        if (m_currentWidget) {
            m_layout->addWidget(m_currentWidget);
        }
    }


}

#include "themeattributeeditor.moc"
