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

#include "mainwindow.h"
#include "themeitemview.h"
#include "specificationstateselector.h"
#include "previewvariableeditor.h"
#include "themeattributeeditor.h"

#include <kstandarddirs.h>
#include <QCloseEvent>
#include <QFileDialog>
#include <KDE/KApplication>
#include <KDE/KGlobal>
#include <KDE/KComponentData>
#include <KDE/KAboutData>
#include <KDE/KLocale>
#include <KDE/KActionCollection>
#include <KDE/KStandardAction>
#include <QMessageBox>
#include <QApplication>
#include <QDockWidget>

namespace Cokoon {

    MainWindow::MainWindow()
    {
        QWidget *cw = new QWidget(this);
        m_cwUi.setupUi(cw);
        setCentralWidget(cw);

        // Dock widgets
        m_themeView = new ThemeItemView(this);
        m_themeView->setModel(&m_itemViewModel);
        QDockWidget *dock = new QDockWidget(i18n("Theme"),this);
        dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dock->setWidget(m_themeView);
        addDockWidget(Qt::RightDockWidgetArea,dock);

        m_specSelector = new SpecificationStateSelector(this);
        dock = new QDockWidget(i18n("Preview Selector"),this);
        dock->setAllowedAreas(Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea);
        dock->setWidget(m_specSelector);
        addDockWidget(Qt::TopDockWidgetArea,dock);

        m_previewVars = new PreviewVariableEditor(this);
        dock = new QDockWidget(i18n("Preview Variables"),this);
        dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea|Qt::TopDockWidgetArea|Qt::BottomDockWidgetArea);
        dock->setWidget(m_previewVars);
        addDockWidget(Qt::LeftDockWidgetArea,dock);

        m_attrEditor = new ThemeAttributeEditor(this);
        dock = new QDockWidget(i18n("Theme Attributes"),this);
        dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dock->setWidget(m_attrEditor);
        addDockWidget(Qt::RightDockWidgetArea,dock);

        // Connections
        m_itemViewModel.setModel(&m_model);
        connect(&m_model,SIGNAL(modelModified()),this,SLOT(documentWasModified()));

        m_cwUi.preview->setVariableEditor(m_previewVars);
        connect(m_specSelector,SIGNAL(currentItemChanged(int)),
                m_previewVars,SLOT(setCurrentItem(int)));
        connect(m_previewVars,SIGNAL(variableValueChanged(int)),
                m_cwUi.preview,SLOT(update()));
        connect(m_specSelector,SIGNAL(objectIdChanged(int)),
                m_cwUi.preview,SLOT(setObjectId(int)));

        connect(m_themeView,SIGNAL(themeElementChanged(ThemeDomNode *)),
                m_attrEditor,SLOT(setThemeElement(ThemeDomNode *)));

        // Actions
        setupActions();
    }

    MainWindow::~MainWindow()
    {
    }

    void MainWindow::closeEvent ( QCloseEvent * event )
    {
        if (maybeSave()) {
            // TODO writeSettings()
            event->accept();
        } else {
            event->ignore();
        }
    }

    void MainWindow::newFile()
    {
        if(maybeSave()) {
            m_model.clear();
            documentWasModified();
        }
    }

    void MainWindow::open()
    {
        if (maybeSave()) {
            QString fileName = QFileDialog::getOpenFileName(this);
            if (!fileName.isEmpty())
                loadFile(fileName);
        }
    }

    bool MainWindow::save()
    {
        if (m_model.currentFileName().isEmpty()) {
            return saveAs();
        } else {
            return m_model.save(m_model.currentFileName());
        }
    }

    bool MainWindow::saveAs()
    {
        QString fileName = QFileDialog::getSaveFileName(this,i18n("Save As"),
                                                       m_model.currentFileName());
        if (fileName.isEmpty())
            return false;

        return m_model.save(fileName);
    }

    void MainWindow::documentWasModified()
    {
        setWindowModified(m_model.modified());

        QString showFileName = m_model.currentFileName();
        if (showFileName.isEmpty()) {
            showFileName = "untitled";
        }

        setWindowTitle(i18n("%1[*] - %2", showFileName, KGlobal::mainComponent().aboutData()->programName()));
    }

    void MainWindow::setupActions()
    {
        KStandardAction::open(this, SLOT(saveAs()), actionCollection());
        KStandardAction::saveAs(this, SLOT(saveAs()), actionCollection());
        KStandardAction::save(this, SLOT(save()), actionCollection());
        KStandardAction::quit(this, SLOT(quit()), actionCollection());

        setupGUI();
    }

    bool MainWindow::maybeSave()
    {
        if (m_model.modified()) {
            QMessageBox::StandardButton ret =
                QMessageBox::warning(this,KGlobal::mainComponent().aboutData()->programName(),
                                     i18n("The document has been modified.\nDo you want to save your changes?"),
                                     QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
                                     QMessageBox::Yes);
            if (ret == QMessageBox::Yes) {
                return save();
            } else if (ret==QMessageBox::Cancel) {
                return false;
            }
        }
        return true;
    }

    bool MainWindow::loadFile(const QString &fileName) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        if (!m_model.loadTheme(fileName)) {
            QMessageBox::warning(this,KGlobal::mainComponent().aboutData()->programName(),
                                 i18n("Cannot open theme file %1", fileName));
            return false;
        }
        QApplication::restoreOverrideCursor();
        documentWasModified();
        m_cwUi.preview->setCokoonDocument(m_model.cokoonDocument());
        m_previewVars->setSpecification(m_model.specification());
        m_specSelector->setSpecification(m_model.specification());
        return true;
    }

    void MainWindow::quit()
    {
        if(maybeSave()) {
            close();
        }
    }

}

#include "mainwindow.moc"
