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
#include "commands.h"
#include "themeitemview.h"
#include "previewdialog.h"
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
#include <QKeySequence>
#include <kshortcut.h>
#include <kaction.h>

namespace Cokoon {

    MainWindow::MainWindow()
    {
        // Dock widgets
        m_themeView = new ThemeItemView(this);
        m_themeView->setModel(&m_model);
        QDockWidget *dock = new QDockWidget(i18n("Theme"),this);
        dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dock->setWidget(m_themeView);
        addDockWidget(Qt::RightDockWidgetArea,dock);

        m_attrEditor = new ThemeAttributeEditor(this);
        setCentralWidget(m_attrEditor);

        m_previewDialog = new PreviewDialog(0);
        m_previewDialog->show();

        // Connections
        m_attrEditor->setModel(&m_model);
        connect(&m_model,SIGNAL(modelModified()),this,SLOT(documentWasModified()));

        connect(m_themeView,SIGNAL(themeElementChanged(ThemeDomNode *)),
                m_attrEditor,SLOT(setThemeElement(ThemeDomNode *)));

        // Actions
        setupActions();
    }

    MainWindow::~MainWindow()
    {
        m_previewDialog->close();
        delete m_previewDialog;
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
        // update actions
        // TODO

        // update window state, title etc.
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

        KStandardAction::undo(this, SLOT(undo()), actionCollection());
        KStandardAction::redo(this, SLOT(redo()), actionCollection());

        KAction *deleteNode = new KAction(KIcon("edit-delete"), i18n("&Delete Node"), this);
        deleteNode->setShortcut(KShortcut(QKeySequence(QKeySequence::Delete)));
        actionCollection()->addAction("deleteNode",deleteNode);
        connect(deleteNode, SIGNAL(triggered(bool)), SLOT(nodeDelete()));

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
        m_previewDialog->setCokoonDocument(m_model.cokoonDocument());
        m_previewDialog->setSpecification(m_model.specification());
        return true;
    }

    void MainWindow::quit()
    {
        if(maybeSave()) {
            close();
        }
    }

    void MainWindow::undo()
    {
        m_model.undoStack().undo();
    }

    void MainWindow::redo()
    {
        m_model.undoStack().redo();
    }

    void MainWindow::nodeUp()
    {
        ThemeDomNode *n = m_themeView->currentNode();
        if (n) {
            m_model.moveNodeUp(n->nodePath());
        }
    }
    void MainWindow::nodeDown()
    {
        ThemeDomNode *n = m_themeView->currentNode();
        if (n) {
            m_model.moveNodeDown(n->nodePath());
        }
    }
    void MainWindow::nodeDelete()
    {
        ThemeDomNode *n = m_themeView->currentNode();
        qDebug() << "nodeDelete" << n;
        if (n) {
            DeleteNodeCommand *c =
                new DeleteNodeCommand(&m_model,n->nodePath());
            m_model.undoStack().push(c);

            // TODO
//             m_model.deleteNode(n->nodePath());
        }
    }


}

#include "mainwindow.moc"
