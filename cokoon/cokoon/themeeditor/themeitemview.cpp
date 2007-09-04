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
#include "themeitemview.h"

namespace Cokoon {

    ThemeItemView::ThemeItemView(QWidget *parent)
        : QTreeView(parent)
    {
    }
    ThemeItemView::~ThemeItemView()
    {
    }

    void ThemeItemView::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
    {
        QModelIndex index = selected.indexes()[0];
        ThemeDomNode *el = static_cast<ThemeDomNode*>(index.internalPointer());
        emit themeElementChanged(el);
    }
}

#include "themeitemview.moc"
