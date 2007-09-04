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

#include <math.h>

#include "../specification.h"
#include "previewvariableeditor.h"

namespace Cokoon {

    class VariableModel : public QAbstractTableModel
    {
    public:
        VariableModel(PreviewVariableEditor *d)
            :doc(d)
        {
        }

        ~VariableModel()
        {
        }

        void setChanged()
        {
            reset();
//             qDebug() << "index 0 0:" << index(0,0);
//             emit dataChanged(index(0,0), index(rowCount()-1,columnCount()-1));
        }

        int rowCount ( const QModelIndex & /* parent = QModelIndex() */ ) const
        {
            Q_ASSERT(doc);
            return doc->m_varValues.size();
        }

        int columnCount ( const QModelIndex & parent = QModelIndex() ) const
        {
            return 2;
        }

        QVariant data ( const QModelIndex & index, int role) const
        {
            Q_ASSERT(doc);

            const SpecificationItem *i = doc->m_currentItem;
            if (!i)
                return QVariant();
            const SpecificationItemVariable *v = i->providedVariables.value(index.row());
            if (!v)
                return QVariant();

            switch (index.column()) {
            case 0: {
                if (role == Qt::DisplayRole) {
                    return v->id + " (" + v->type + ")";
                }
                break;
            }
            case 1: {
                QVariant val = doc->m_varValues.value(index.row());

                if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
                    return val;
                }
                else if (role == Qt::DecorationRole && val.type()==QVariant::Color) {
                    return val;
                }
//                     if (i) {
//                         const SpecificationItemVariable *v =
//                             i->providedVariables.value(index.row());
//                         if(v && v->type=="Color") {
//                             return QColor(value);
//                         }
//                     }
//                 }
                break;
            }
            default:
                break;
            }

            return QVariant();
        }

        bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
        {
            Q_ASSERT(doc);

            if (role == Qt::EditRole) {
                int row = index.row();
                if (index.column()==1 && row >= 0 && row < doc->m_varValues.size() ) {
                    if (data(index,role).type() == QVariant::Color) {
                        doc->m_varValues[row] = value.value<QColor>();
                    } else {
                        doc->m_varValues[row] = value;
                    }
                    doc->emitVarValChanged(row);
                }
                return true;
            }
            return false;
        }

        Qt::ItemFlags flags ( const QModelIndex & index ) const
        {
            if (index.column() == 1) {
                return Qt::ItemIsEnabled|Qt::ItemIsEditable;
            }
            return QAbstractTableModel::flags(index);
        }

        QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const
        {
            if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
                return QVariant();
            switch(section) {
            case 0:
                return QString(tr("Variable"));
            case 1:
                return QString(tr("Value"));
            default:
                return QVariant();
            }
        }

    private:
        PreviewVariableEditor *doc;
    };

    PreviewVariableEditor::PreviewVariableEditor(QWidget *parent)
        : QWidget(parent), m_spec(0), m_variableModel(new VariableModel(this)), m_currentItem(0)
    {
        m_ui.setupUi(this);
        m_ui.variables->verticalHeader()->hide();
//         m_ui.variables->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    }

    PreviewVariableEditor::~PreviewVariableEditor()
    {
        delete m_variableModel;
    }

    void PreviewVariableEditor::clear()
    {
        m_varValues.clear();
    }

    void PreviewVariableEditor::setSpecification(const DocumentSpecification *spec)
    {
        m_spec = spec;
        setCurrentItem(0);
    }

    QVariant PreviewVariableEditor::getVariableValue(int idx) const
    {
        return m_varValues.value(idx);
    }

    void PreviewVariableEditor::setCurrentItem(int itemIndex)
    {
        Q_ASSERT(m_spec);

        clear();
        m_currentItem = m_spec->items().value(itemIndex);
        if(m_currentItem) {
            QList<SpecificationItemVariable*> vars = m_currentItem->providedVariables;
            foreach(SpecificationItemVariable *v, vars) {
                QString valueString = v->previewDefaultValue;
                QVariant val;
                if (v->type == "Color") {
                    val = QColor(valueString);
                } else if (v->type == "Int") {
                    bool ok;
                    int intVal = valueString.toInt(&ok);
                    if (ok)
                        val = intVal;
                    else
                        val = 0;
                } else if (v->type == "Double") {
                    bool ok;
                    double dblVal = valueString.toDouble(&ok);
                    if (ok)
                        val = dblVal;
                    else
                        val = 0.0;
                }

                m_varValues.append(val);
            }
        }

        m_ui.variables->setModel(m_variableModel);
        m_variableModel->setChanged();
        m_ui.variables->update();
    }

    void PreviewVariableEditor::emitVarValChanged(int index)
    {
        emit variableValueChanged(index);
    }

}

#include "previewvariableeditor.moc"
