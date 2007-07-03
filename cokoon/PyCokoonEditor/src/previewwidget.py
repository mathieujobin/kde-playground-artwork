
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

  def setCurrentThemeElement(self,el):
    if el!=None:
      self.currentId = self.model.spec.mapObjectStringToSpecIndex(el.id())
      self.update()
