
from PyQt4 import QtXml
from PyQt4.QtCore import QObject, QFile
from PyQt4.QtGui import QMessageBox

class ThemeSpecItem:
  def __init__(self):
    self.id = ''
    self.requiredExpressions = []
    self.states = []

class ThemeSpec:
  def __init__(self,themeSpecFile):
    self.readThemeSpec(themeSpecFile)
  def clearThemeSpec(self):
    self.name = ''
    self.items = {}
  def readThemeSpec(self,themeSpecFile):
    self.clearThemeSpec()
    
    handler = ThemeSpecHandler(self)
    reader = QtXml.QXmlSimpleReader()
    reader.setContentHandler(handler)
    reader.setErrorHandler(handler)
    
    file = QFile(themeSpecFile)
    if not (file.open(QFile.ReadOnly | QFile.Text)):
      QMessageBox.warning(self, tr("CokoonEditor"),
        tr("Cannot read file %s:\n%s." % [fileName, file.errorString]))
      return
    xmlInputSource = QtXml.QXmlInputSource(file)
    ret = reader.parse(xmlInputSource, True)
    if ret:
      print self.items

class ThemeSpecHandler(QtXml.QXmlDefaultHandler):
  def __init__(self,themeSpec):
    QtXml.QXmlDefaultHandler.__init__(self)
    
    self.themeSpec = themeSpec
    self.metCokoonSpecTag = False
    self.currentTag = None
    self.currentItem = None
    self.currentStateList = None
  def startElement(self,namespaceURI, localName, qName, attributes):
    if (not self.metCokoonSpecTag) and qName != "cokoon_spec":
      self.errorStr = "The file is not an Cokoon Theme Specification file."
      return False

    if qName == "cokoon_spec":
      self.themeSpec.name = attributes.value("name")
      self.metCokoonSpecTag = True
    elif qName == "item":
      self.currentItem = ThemeSpecItem()
      self.currentItem.id = attributes.value("id")
    elif qName == "expression":
      exp = (attributes.value("id"), attributes.value("type"))
      self.currentItem.requiredExpressions.append(exp)
    elif qName == "states":
      self.currentStateList = []
    elif qName == "state":
      self.currentStateList.append( attributes.value("id") )
    
    # TODO ! provided: special_cell, variable
    return True

  def endElement(self,namespaceURI, localName, qName):
    if qName == "item":
      print self.currentItem
      self.themeSpec.items[self.currentItem.id] = self.currentItem
      self.currentItem = None
    elif qName == "states":
      self.currentItem.states.append(self.currentStateList)
      self.currentStateList = None
    return True

  def fatalError(self,exception):
    print ("Parse error at line %s, column %s:\n%s" %
          (exception.lineNumber(), exception.columnNumber(),exception.message()))
#    QMessageBox.information(, "SAX Bookmarks",
#        "Parse error at line %s, column %s:\n%s" %
#          (exception.lineNumber, exception.columnNumber,exception.message) )
    return False

  def errorString(self):
    return self.errorStr
