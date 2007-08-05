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
    self.attrs = []             # [[attrName,widget],...]
    self.comboAttrs = []        # [[attrName,widget],...]
  def setUi(self,ui):
    self.ui = ui
    ui.setupUi(self)
  def setTextAttrMapping(self,attrName,widget):
    '''associate a line edit widget with a DOM attribute'''
    self.attrs.append([attrName,widget])
    widget.setText(self.el._domAttr(attrName))
    self.connect(widget, QtCore.SIGNAL("editingFinished()"),self.textValueChanged)
  def setComboAttrMapping(self,attrName,comboBox,comboValues):
    '''associate a combo box with a DOM attribute'''
    self.comboAttrs.append([attrName,comboBox])
    currentValue = self.el._domAttr(attrName)
    currentIndex = 0
    for val in comboValues:
      comboBox.addItem(val)
      if val == currentValue:
        currentIndex = comboBox.count()-1
    comboBox.setCurrentIndex(currentIndex)
    self.connect(comboBox, QtCore.SIGNAL("activated(int)"),self.comboValueChanged)
  def textValueChanged(self):
    for a in self.attrs:
      attr = a[0]
      text = a[1].text()
      if self.el._domAttr(attr) != text:
        self.el._setDomAttr(attr,text)
  def comboValueChanged(self):
    for a in self.comboAttrs:
      attr = a[0]
      text = a[1].currentText()
      if self.el._domAttr(attr) != text:
        if text.isEmpty():
          self.el.element.removeAttribute(attr)
        else:
          self.el._setDomAttr(attr,text)


class ThemeEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_theme.Ui_Form())
    self.setTextAttrMapping("name",self.ui.name)
    self.setTextAttrMapping("version",self.ui.version)
    self.setTextAttrMapping("spec",self.ui.spec)

class ObjectEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_object.Ui_Form())
    self.setTextAttrMapping("id",self.ui.identifier)
    self.setTextAttrMapping("implement",self.ui.implement)
    # extend
    extendObjs = [""]
    theme = self.el.parent
    for obj in theme.objects():
      extendObjs.append(obj.id())
    self.setComboAttrMapping("extend",self.ui.extend,extendObjs)


class TileEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_tile.Ui_Form())
    self.setTextAttrMapping("id",self.ui.identifier)
    self.setTextAttrMapping("top",self.ui.top)
    self.setTextAttrMapping("left",self.ui.left)
    self.setTextAttrMapping("width",self.ui.width)
    self.setTextAttrMapping("height",self.ui.height)

    # source object/id
    sourceObjs = [""]
    sourceIds = []
    theme = self.el.parent.parent
    for obj in theme.objects():
      sourceObjs.append(obj.id())
      for source in obj.sources():
        if not (source.id() in sourceIds):
          sourceIds.append(source.id())

    self.setComboAttrMapping("source_object",self.ui.sourceObject,sourceObjs)
    self.setComboAttrMapping("source_id",self.ui.sourceId,sourceIds)

    # hor/vert modes
    modes = [QtCore.QString("normal"),QtCore.QString("centered"),QtCore.QString("tiled"),QtCore.QString("stretched")]
    self.setComboAttrMapping("mode_hor",self.ui.horizontal,modes)
    self.setComboAttrMapping("mode_vert",self.ui.vertical,modes)

class CommentEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_comment.Ui_Form())
    self.ui.comment.setPlainText(self.el.node.nodeValue())
    self.connect(self.ui.comment, QtCore.SIGNAL("textChanged()"),self.commentValueChanged)
  def commentValueChanged(self):
    c = self.el.node.setNodeValue(self.ui.comment.toPlainText())

class ExpressionEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_expression.Ui_Form())
    self.setTextAttrMapping("id",self.ui.identifier)
    self.setTextAttrMapping("value",self.ui.value)

class LayoutEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_layout.Ui_Form())

class SourceEditor(AttrEditor):
  def __init__(self,el,parent=None):
    AttrEditor.__init__(self,el,parent)
    self.setUi(ui_edit_source.Ui_Form())
    self.setTextAttrMapping("id",self.ui.identifier)
    self.setTextAttrMapping("file",self.ui.file)
    types = [QtCore.QString("svg"),QtCore.QString("pixmap"),QtCore.QString("bitmap")]
    self.setComboAttrMapping("type",self.ui.type,types)


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
