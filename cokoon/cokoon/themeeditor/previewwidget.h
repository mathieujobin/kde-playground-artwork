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

#ifndef COKOON_PREVIEWWIDGET_H
#define COKOON_PREVIEWWIDGET_H

#import <QWidget>

class QPaintEvent;

namespace Cokoon {

    class Document;
    class PreviewVariableEditor;

/**
 * Edit values of variables, which will be the input of the
 * theme preview.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class Q_DECL_EXPORT PreviewWidget : public QWidget
{
    Q_OBJECT
 public:

    PreviewWidget(QWidget *parent = 0);
    virtual ~PreviewWidget();

    void setVariableEditor(const PreviewVariableEditor *vars);
    void setCokoonDocument(const Document *doc);

    virtual void paintEvent ( QPaintEvent * event );

 public slots:
    /**
     * Set the current specification item index
     */
    void setObjectId(int id);

 private:
    const PreviewVariableEditor *m_vars;
    const Document *m_doc;
    int m_id;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
