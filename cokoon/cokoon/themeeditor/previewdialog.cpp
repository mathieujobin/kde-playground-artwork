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

#include "previewdialog.h"

namespace Cokoon {

    PreviewDialog::PreviewDialog(QWidget *parent)
        : QWidget(parent)
    {
        m_ui.setupUi(this);

        m_ui.preview->setVariableEditor(m_ui.varEditor);

        connect(m_ui.varEditor,SIGNAL(variableValueChanged(int)),
                m_ui.preview,SLOT(update()));
        connect(m_ui.varEditor,SIGNAL(sizeValueChanged()),
                m_ui.preview,SLOT(update()));
        connect(m_ui.specSelector,SIGNAL(objectIdChanged(int)),
                m_ui.preview,SLOT(setObjectId(int)));
        connect(m_ui.specSelector,SIGNAL(currentItemChanged(int)),
                m_ui.varEditor,SLOT(setCurrentItem(int)));
    }

    PreviewDialog::~PreviewDialog()
    {
    }

    void PreviewDialog::setSpecification(const DocumentSpecification *spec)
    {
        m_ui.varEditor->setSpecification(spec);
        m_ui.specSelector->setSpecification(spec);
    }

    void PreviewDialog::setCokoonDocument(const Document *doc)
    {
        m_ui.preview->setCokoonDocument(doc);
    }


}

#include "previewdialog.moc"
