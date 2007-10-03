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

#ifndef COKOON_PREVIEWDIALOG_H
#define COKOON_PREVIEWDIALOG_H

#import <QWidget>
#import "ui_previewdialog.h"

namespace Cokoon {

    class DocumentSpecification;
    class Document;

    class PreviewDialog : public QWidget
    {
Q_OBJECT
    public:
        PreviewDialog(QWidget *parent = 0);
        virtual ~PreviewDialog();

    public slots:
        void setSpecification(const DocumentSpecification *spec);
        void setCokoonDocument(const Document *doc);

    private:
        Ui_PreviewDialog m_ui;
    };


}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
