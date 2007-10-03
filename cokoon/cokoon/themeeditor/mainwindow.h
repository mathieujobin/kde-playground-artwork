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

#ifndef COKOONEDITOR_MAINWINDOW_H
#define COKOONEDITOR_MAINWINDOW_H

#include <KDE/KXmlGuiWindow>
#include "thememodel.h"

class QCloseEvent;

namespace Cokoon {

    class ThemeItemView;
    class SpecificationStateSelector;
    class PreviewVariableEditor;
    class ThemeAttributeEditor;
    class PreviewDialog;

    class MainWindow : public KXmlGuiWindow
    {
Q_OBJECT
    public:
        MainWindow();
        virtual ~MainWindow();

        virtual void closeEvent ( QCloseEvent * event );
    public slots:
        void newFile();
        void open();
        bool save();
        bool saveAs();
        bool maybeSave();
        bool loadFile(const QString &fileName);
        void quit();
        void undo();
        void redo();

        void nodeUp();
        void nodeDown();
        void nodeDelete();

        void documentWasModified();
    private:
        void setupActions();

        ThemeModel m_model;
        ThemeItemView *m_themeView;
        ThemeAttributeEditor *m_attrEditor;

        PreviewDialog *m_previewDialog;
    };


}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
