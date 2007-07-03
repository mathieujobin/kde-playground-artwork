
from PyQt4 import QtCore
from PyQt4 import QtGui

from thememodel import ThemeModel

class AttributeEditWidget(QtGui.QWidget):
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)

    self.switchThemeElement(None)

  def switchThemeElement(self, el):
    print el, dir(el)
    if el != None:
      print el.id()
      self.label = QtGui.QLabel(el.id(), self)
