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

#ifndef COKOON_PREVIEWVARIABLEEDITOR_H
#define COKOON_PREVIEWVARIABLEEDITOR_H

#include "../bridges.h"
#include "ui_previewvariableeditor.h"

namespace Cokoon {

    class SpecificationItem;
    class DocumentSpecification;
    class VariableModel;

/**
 * Edit values of variables, which will be the input of the
 * theme preview.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class Q_DECL_EXPORT PreviewVariableEditor : public QWidget, public ExpressionVariableBridge
{
    Q_OBJECT
 public:

    /**
     * Construct.
     */
    PreviewVariableEditor(QWidget *parent = 0);
    /**
     * Destruct.
     */
    virtual ~PreviewVariableEditor();

    void setSpecification(const DocumentSpecification *spec);

    /**
     * Implement abstract interface.
     */
    virtual QVariant getVariableValue(int idx) const;

 signals:
    void variableValueChanged(int variableIndex);

 public slots:
    /**
     * Set the current specification item index
     */
    void setCurrentItem(int itemIndex);

 private slots:
    void emitVarValChanged(int index);
 private:
    friend class VariableModel;
    void clear();
    QList<QVariant> m_varValues;
    const DocumentSpecification *m_spec;
    VariableModel *m_variableModel;
    Ui_PreviewVariableEditor m_ui;
    SpecificationItem *m_currentItem;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
