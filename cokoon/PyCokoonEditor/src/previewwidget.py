
from PyQt4 import QtCore
from PyQt4 import QtGui
from cokoon import Cokoon

class PreviewWidget(QtGui.QWidget):
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)
    self.cokoonDoc = Cokoon.Document()
    self.loadTheme("/home/sandro/DatenNoBackup/pyqt4/PyCokoonEditor/Tests/TestTheme/TestTheme.xml")
    
  def paintEvent(self, event):
    painter = QtGui.QPainter(self)
    id = self.cokoonDoc.getIdentifierIndex("Button.normal.normal")
    print id
    self.cokoonDoc.drawLayers(id, painter, 0, 0, self.width(), self.height() )
    
  def loadTheme(self, fileName):
    self.cokoonDoc.loadTheme(fileName)
