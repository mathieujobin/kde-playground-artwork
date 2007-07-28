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
from thememodel import *
import ui_edit_object
import ui_edit_tile
import ui_edit_comment
import ui_edit_expression
import ui_edit_layout
import ui_edit_source

class AttrEditor(QtGui.QWidget):
  def __init__(self,element,parent=None):
    QtGui.QWidget.__init__(self,parent)
    self.el = element
    self.attrs = []
  def setUi(self,ui):
    self.ui = ui
    ui.setupUi(self)
  def setAttrWidget(self,attrName,widget):
    self.attrs.append([attrName,widget])
    widget.setText(self.el._domAttr(attrName))
    self.connect(widget, QtCore.SIGNAL("editingFinished()"),self.valueChanged)
  def valueChanged(self):
    for a in self.attrs:
      self.el._setDomAttr(a[0],a[1].text())

class ObjectEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_object.Ui_Form())
    self.setAttrWidget("id",self.ui.identifier)
    self.setAttrWidget("implement",self.ui.implement)
    self.setAttrWidget("extend",self.ui.extend)

class TileEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_tile.Ui_Form())
    self.setAttrWidget("id",self.ui.identifier)
    self.setAttrWidget("top",self.ui.top)
    self.setAttrWidget("left",self.ui.left)
    self.setAttrWidget("width",self.ui.width)
    self.setAttrWidget("height",self.ui.height)

class CommentEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_comment.Ui_Form())

class ExpressionEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_expression.Ui_Form())
    self.setAttrWidget("id",self.ui.identifier)
    self.setAttrWidget("value",self.ui.value)

class LayoutEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_layout.Ui_Form())

class SourceEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_source.Ui_Form())
    self.setAttrWidget("id",self.ui.identifier)
    self.setAttrWidget("file",self.ui.file)

class AttributeEditWidget(QtGui.QWidget):
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)

    self.widgetLayout = QtGui.QHBoxLayout(self)

    self.widget = None
    self.switchThemeElement(None)

  def switchThemeElement(self, el):
    if el != None:
      print el.id()

      if self.widget != None:
        self.widgetLayout.removeWidget(self.widget)
        del(self.widget)
        self.widget = None

      print type(el)
      if el.node.isComment():
        self.widget = CommentEditor(el,self)
      elif el.tag == "object":
        self.widget = ObjectEditor(el,self)
      elif el.tag == "tile":
        self.widget = TileEditor(el,self)
      elif el.tag == "expression":
        self.widget = ExpressionEditor(el,self)
      elif el.tag == "layout":
        self.widget = LayoutEditor(el,self)
      elif el.tag == "source":
        self.widget = SourceEditor(el,self)


      if self.widget != None:
        self.widgetLayout.addWidget(self.widget)
