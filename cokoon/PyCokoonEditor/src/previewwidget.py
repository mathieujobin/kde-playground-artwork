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


class PreviewWidget(QtGui.QWidget):
  def __init__(self, themeModel, parent=None):
    QtGui.QWidget.__init__(self, parent)
    self.setThemeModel(themeModel)
    self.currentId = 34         # TODO: -1

  def paintEvent(self, event):
    doc = self.model.cokoonDoc
    if doc != None:
      if self.currentId != -1:
        painter = QtGui.QPainter(self)
        doc.drawLayers(self.currentId,painter,0,0,self.width(), self.height())
        return

    # else...
    QtGui.QWidget.paintEvent(self,event)

  def setThemeModel(self, themeModel):
    self.model = themeModel

  def setCurrentThemeElement(self,specStateString):
    self.currentId = self.model.spec.mapObjectStringToSpecIndex(QtCore.QString(specStateString) )
    self.update()
