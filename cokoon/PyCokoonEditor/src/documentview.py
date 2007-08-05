'''
 Cokoon theme editor
 Copyright (C) 2007 Sandro Giessl <giessl@kde.org>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; see the file COPYING.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
'''

from PyQt4 import QtCore
from PyQt4 import QtGui

from thememodel import ThemeModel

class DocumentView(QtGui.QTreeView):
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)
    self.setAllColumnsShowFocus(True)
    self.setRootIsDecorated(False)

  def selectionChanged(self, selected, deselected ):
    print "selectionChanged..."
    modelIndex = selected.indexes()[0]
    themeElement = modelIndex.internalPointer()
    self.emit(QtCore.SIGNAL("themeElementSelected"), (themeElement))
