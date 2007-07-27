
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
