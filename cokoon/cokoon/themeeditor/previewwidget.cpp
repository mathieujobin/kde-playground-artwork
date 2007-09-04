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
#include <QPaintEvent>
#include <QPainter>
#include "previewvariableeditor.h"
#include "specificationstateselector.h"

#include "previewwidget.h"

namespace Cokoon {

    PreviewWidget::PreviewWidget(QWidget *parent)
        :QWidget(parent), m_vars(0), m_doc(0), m_id(-1)
    {
    }

    PreviewWidget::~PreviewWidget()
    {
    }

    void PreviewWidget::setVariableEditor(const PreviewVariableEditor *vars)
    {
        m_vars = vars;
        update();
    }

    void PreviewWidget::setCokoonDocument(const Document *doc)
    {
        m_doc = doc;
        update();
    }

    void PreviewWidget::paintEvent ( QPaintEvent * event )
    {
        qDebug() << "paintEvent" << m_doc << m_id;
        if (m_doc && m_id>=0) {
            QPainter p(this);
            int w = 100;
            int h = 100;
            m_doc->drawLayers(m_id,&p,0,0,w,h,m_vars);
        } else {
            QWidget::paintEvent(event);
        }
    }

    void PreviewWidget::setObjectId(int id)
    {
        m_id = id;
        update();
    }

}

#include "previewwidget.moc"
