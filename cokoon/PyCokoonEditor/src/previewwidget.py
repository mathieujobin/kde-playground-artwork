
from PyQt4 import QtCore
from PyQt4 import QtGui

from thememodel import ThemeModel


class PreviewWidget(QtGui.QWidget):
  def __init__(self, themeModel, parent=None):
    QtGui.QWidget.__init__(self, parent)
    self.setThemeModel(themeModel)
    
  def paintEvent(self, event):
    QtGui.QWidget.paintEvent(self,event)
#     painter = QtGui.QPainter(self)
#     id = self.cokoonDoc.getIdentifierIndex("Button.normal.normal")
#     print id
#     self.cokoonDoc.drawLayers(id, painter, 0, 0, self.width(), self.height() )

  def setThemeModel(self, themeModel):
    self.model = themeModel

