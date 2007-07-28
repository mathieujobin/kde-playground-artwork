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

from PyQt4 import QtGui
from PyQt4 import QtCore

class ThemeSpecSelector(QtGui.QWidget):
  def __init__(self,model,parent=None):
    QtGui.QWidget.__init__(self,parent)
    self.objBox = QtGui.QComboBox(self)
    self.stateLayout = QtGui.QHBoxLayout(self)
    self.stateLayout.addWidget(self.objBox)
    self.stateLayout.addSpacing(5)
    self.stateWidgets = []
    self.connect(self.objBox,QtCore.SIGNAL("activated(int)"),self.updateStateBoxes)
    self.connect(self.objBox,QtCore.SIGNAL("activated(int)"),self.slotSelectionChanged)
    self.setThemeModel(model)
  def setThemeModel(self,model):
    self.model = model
    self.updateObjBox()
  def updateObjBox(self):
    self.objBox.clear()
    spec = self.model.spec
    if spec==None:
      return
    for i in spec.items:
      self.objBox.addItem(i.id)
    self.updateStateBoxes()
  def updateStateBoxes(self):
    # Clear...
    for w in self.stateWidgets:
      self.stateLayout.removeWidget(w)
    self.stateWidgets = []
    # Init...
    cur = self.objBox.currentIndex()
    if cur >= 0:
      for stateLvl in self.model.spec.items[cur].states:
        w = QtGui.QComboBox(self)
        self.connect(w,QtCore.SIGNAL("activated(int)"),self.slotSelectionChanged)
        for state in stateLvl:
          w.addItem(state)
        self.stateWidgets.append(w)
        self.stateLayout.addWidget(w)
  def slotSelectionChanged(self):
    stateText = self.objBox.currentText()
    for w in self.stateWidgets:
      stateText = stateText + "." + w.currentText()
    print "currently selected:", stateText
    self.emit(QtCore.SIGNAL("specStateChanged"), (stateText))
