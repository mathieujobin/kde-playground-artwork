
from PyQt4 import QtCore
from PyQt4 import QtGui

from thememodel import ThemeModel

class DocumentView(QtGui.QTreeView):
  def __init__(self, parent=None):
    QtGui.QWidget.__init__(self, parent)

  def selectionChanged(self, selected, deselected ):
    print "selectionChanged..."
    modelIndex = selected.indexes()[0]
    themeElement = modelIndex.internalPointer()
    self.emit(QtCore.SIGNAL("themeElementSelected"), (themeElement))

