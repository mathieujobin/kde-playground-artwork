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

class IntSettingWidget(QtGui.QSpinBox):
  def __init__(self,default,parent=None):
    QtGui.QSpinBox.__init__(self,parent)
    self.setRange(-10000000,1000000)
    self.connect(self,QtCore.SIGNAL("valueChanged(int)"),self.slotChanged)
    if default==None or default=="":
      self.setValue(0)
    else:
      self.setValue(int(default))
  def slotChanged(self):
    self.emit(QtCore.SIGNAL("valueChanged"),())
  def getVariantValue(self):
    return QtCore.QVariant(int(self.value()))
class DoubleSettingWidget(QtGui.QDoubleSpinBox):
  def __init__(self,default,parent=None):
    QtGui.QDoubleSpinBox.__init__(self,parent)
    self.setRange(-10000000,1000000)
    self.connect(self,QtCore.SIGNAL("valueChanged(double)"),self.slotChanged)
    if default==None or default=="":
      self.setValue(0.0)
    else:
      self.setValue(float(default))
  def slotChanged(self):
    self.emit(QtCore.SIGNAL("valueChanged"),())
  def getVariantValue(self):
    return QtCore.QVariant(float(self.value()))
class ColorSettingWidget(QtGui.QPushButton):
  def __init__(self,defaultColorString,parent=None):
    QtGui.QPushButton.__init__(self,parent)
    if defaultColorString==None or defaultColorString=="":
      self.setValue(QtGui.QColor("#000000"))
    else:
      self.setColor(QtGui.QColor(defaultColorString))
    self.connect(self,QtCore.SIGNAL("clicked(bool)"),self.colorRequester)
  def colorRequester(self):
    col = QtGui.QColorDialog.getColor(self.color)
    print "the selected color:",col.name()
    if col.isValid():
      self.setColor(col)
  def setColor(self,color):
    self.color = color
    self.setText(self.color.name())
    self.emit(QtCore.SIGNAL("valueChanged"),())
  def getVariantValue(self):
    return QtCore.QVariant(self.color)

class PreviewSettingsWidget(QtGui.QWidget):
  def __init__(self,model,parent=None):
    QtGui.QWidget.__init__(self,parent)
    self.currentItem = -1
    self.layout = QtGui.QGridLayout(self)
    self.widgetLabels = []
    self.widgets = []
    self.idToValueStringMap = {}
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
    self.idToValueStringMap = {} # id -> valueString function pointer
    # Init...
    if itemId >= 0:
      item = self.model.spec.items[itemId]
      self.addRow("width","Int",item.preview_width)  # Height and width are handled specially,
      self.addRow("height","Int",item.preview_height) # they are given during the drawLayers() call
      for var in item.providedVariables:
        varIdString = var[0]
        varType = var[1]
        varDefault = var[2]
        varId = None
        if varIdString in self.model.spec.specVariableToId:
          varId = self.model.spec.specVariableToId[varIdString]
        w = self.addRow(varIdString,var[1],varDefault,varId)

  def addRow(self,varIdString,varType,default,varId=None):
    label = QtGui.QLabel(varIdString+" ("+varType+"):",self)
    widget = None
    valueFunctionPtr = None
    if varType=="Int":
      widget = IntSettingWidget(default,self)
    elif varType=="Double":
      widget = DoubleSettingWidget(default,self)
    elif varType=="Color":
      widget = ColorSettingWidget(default,self)

    if varId != None and widget != None:
      self.idToValueStringMap[varId] = widget.getVariantValue

    if widget != None:
      self.connect(widget,QtCore.SIGNAL("valueChanged"),self.slotSettingsChanged)
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
    """Returns QVariant of the variable, or an invalid QVariant if the variable doe not exist"""
    if varIndex in self.idToValueStringMap:
      return self.idToValueStringMap[varIndex]()
    else:
      return QtCore.QVariant()
  def slotSettingsChanged(self):
    self.emit(QtCore.SIGNAL("settingsChanged"), ())

class SettingsVariableBridge(Cokoon.ExpressionVariableBridge):
  def __init__(self,settingsWidget):
    Cokoon.ExpressionVariableBridge.__init__(self)
    self.settings = settingsWidget
  def getVariableValue(self,varIndex):
    return self.settings.getVariableValue(varIndex)

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
        s2 = SettingsVariableBridge(self.preview.settings)
        doc.drawLayers(curId,painter,0,0,w, h, s2)
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
