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

from cokoon import Cokoon

from thememodel import ThemeModel
from themespecselector import ThemeSpecSelector

class PreviewSettingsWidget(QtGui.QWidget,Cokoon.ExpressionVariableBridge):
  def __init__(self,model,parent=None):
    QtGui.QWidget.__init__(self,parent)
    self.currentItem = -1
    self.layout = QtGui.QGridLayout(self)
    self.widgetLabels = []
    self.widgets = []
    self.setThemeModel(model)
  def setThemeModel(self,model):
    self.model = model
  def updateSpecItem(self,itemId):
    self.currentItem = itemId
    # Clear...
    for w in self.widgets:
      self.layout.removeWidget(w)
    for l in self.widgetLabels:
      self.layout.removeWidget(l)
    self.widgetLabels = []
    self.widgets = []
    # Init...
    self.addRow("width","Int")  # Height and width are handled specially,
    self.addRow("height","Int") # they are given during the drawLayers() call
    if itemId >= 0:
      for var in self.model.spec.items[itemId].providedVariables:
        self.addRow(var[0],var[1])
  def addRow(self,varId,varType):
    label = QtGui.QLabel(varId+" ("+varType+"):",self)
    widget = None
    if varType=="Int":
      widget = QtGui.QSpinBox(self)
      self.connect(widget,QtCore.SIGNAL("valueChanged(int)"),self.slotSettingsChanged)
    else:
      widget = QtGui.QLineEdit(self)
      self.connect(widget,QtCore.SIGNAL("textChanged(const QString&)"),self.slotSettingsChanged)
    row = len(self.widgets)
    self.layout.addWidget(label,row,0)
    self.layout.addWidget(widget,row,1)
    self.widgetLabels.append(label)
    self.widgets.append(widget)
  def getWidth(self):
    if len(self.widgets)>=1:
      return int(self.widgets[0].value())
    else:
      return 100
  def getHeight(self):
    if len(self.widgets)>=2:
      return int(self.widgets[1].value())
    else:
      return 100
  def getVariableValue(self,varIndex):
    """implemented from ExpressionVariableBridge.
    Returns QVariant(value)"""
    if vars != None and varIndex>=0 and (len(self.widgets)-2)>varIndex:
      val = QtCore.QVariant(QVariant.Int) # TODO: other variable types as Int
      val.setValue(self.widgets[varIndex-2].value())
    else:
      return QtCore.QVariant()
  def slotSettingsChanged(self):
    self.emit(QtCore.SIGNAL("settingsChanged"), ())

class PreviewDisplayWidget(QtGui.QWidget):
  def __init__(self, preview, parent=None):
    QtGui.QWidget.__init__(self, parent)
    self.preview = preview

  def paintEvent(self, event):
    doc = self.preview.model.cokoonDoc
    curId = self.preview.currentId
    if doc != None and curId != -1:
        painter = QtGui.QPainter(self)
        w = self.preview.settings.getWidth()
        h = self.preview.settings.getHeight()
        s = self.preview.settings # TODO: get the variable bridge to work!
        doc.drawLayers(curId,painter,0,0,w, h)
        return

    # else...
    QtGui.QWidget.paintEvent(self,event)

class PreviewWidget(QtGui.QWidget):
  def __init__(self, themeModel, parent=None):
    QtGui.QWidget.__init__(self, parent)
    self.specSelector = ThemeSpecSelector(themeModel,self)
    self.setThemeModel(themeModel)
    self.currentId = -1
    self.settings = PreviewSettingsWidget(themeModel,self)
    self.display = PreviewDisplayWidget(self,self)
    self.connect(self.specSelector, QtCore.SIGNAL("specStateChanged"),
                 self.setCurrentThemeElement)
    self.connect(self.specSelector, QtCore.SIGNAL("specItemChanged"),
                 self.settings.updateSpecItem)
    self.connect(self.settings, QtCore.SIGNAL("settingsChanged"),
                 self.display.update)
    l = QtGui.QVBoxLayout(self)
    l2 = QtGui.QHBoxLayout(self)
    l2.addWidget(self.specSelector)
    l2.addStretch(2)
    l.addLayout(l2)
    l.addWidget(self.settings)
    l.addWidget(self.display,1)


  def setThemeModel(self, themeModel):
    self.model = themeModel
    self.specSelector.setThemeModel(themeModel)

  def setCurrentThemeElement(self,specStateString):
    self.currentId = self.model.spec.mapObjectStringToSpecIndex(QtCore.QString(specStateString) )
    self.display.update()
