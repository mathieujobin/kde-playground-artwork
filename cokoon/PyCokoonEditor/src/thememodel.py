
from PyQt4 import QtCore
from PyQt4.QtXml import QDomDocument
from PyQt4.QtCore import QFile, QTextStream, QAbstractItemModel

from themespec import ThemeSpec, ThemeSpecDocument

class InvalidCokoonThemeException(Exception):
  '''Exception raised when an error occurs while handling a Cokoon Theme'''
  def __init__(self, msg, domElement):
    Exception.__init__(self)
    self.msg = msg
    self.domElement = domElement

class CokoonDomNode:
  def __init__(self, node, parent = None):
    '''initialises the node. with dom (QDomNode), 
    and a parent node'''
    self.node = node
    self.row = -1
    self.parent = parent
    self._childItems = None
  def isNull(self):
    return self.node.isNull()
  def children(self):
    '''returns the list of child nodes'''
    if self._childItems == None:
      self._updateChildItems()
    return self._childItems
  def child(self, i):
    '''returns the i-th child element'''
    if self._childItems == None:
      self._updateChildItems()
    if i >= 0 and len(self._childItems)>i:
      return self._childItems[i]
    return None
  def _updateChildItems(self):
    '''update the cache of child items'''
    self._childItems = []
    child = self.node.firstChild()
    count = 0
    while not child.isNull():
      n = self.createChildNode(child)
      if n:
        n.row = count
        self._childItems.append(n)
        ++count;
      child = child.nextSibling()

  def createChildNode(self, node):
    '''factory method which can be used by subclasses
    to implement the child() functionality.
    returns a CokoonDomNode if the node is suitable as a child, 
    or None if the node should be ignored'''
    if node.isComment():
      return CokoonDomComment(node, self)
    return None

  def id(self):
    return self.node.nodeName()
  def displayId(self):
    return self.id()
  def displayValue(self):
    attributes = QtCore.QStringList()
    attributeMap = self.node.attributes()
    for i in range(0, attributeMap.count()):
      attribute = attributeMap.item(i)
      attributes.append(attribute.nodeName() + "=\"" + \
                        attribute.nodeValue() + "\"")

    return attributes.join(" ")

class CokoonDomComment(CokoonDomNode):
  def __init__(self, node, parent):
    CokoonDomNode.__init__(self, node, parent)
    if not node.isComment():
      raise InvalidCokoonThemeException('QDomComment expected', self)
  def text(self):
    return self.node.nodeValue()
#    TODO: implement comment.
    return '...comment...';
  def displayValue(self):
    return self.text().split("\n").join(" ")

class CokoonDomElement(CokoonDomNode):
  def __init__(self, node, parent, tagName):
    CokoonDomNode.__init__(self, node, parent)
    if not node.isElement():
      raise InvalidCokoonThemeException('QDomDocument expected', self)
    self.element = node.toElement()
    self.tag = tagName
    if tagName and self.element.tagName() != tagName:
#      print 'tag %s expected,  but got %s'%(tagName, self.element.tagName())
      raise InvalidCokoonThemeException('tag %s expected'%(tagName, ), self)
  def _domAttr(self, attributeName):
    '''helper method which tries to extract the attribute attributeName
    and throws an InvalidCokoonThemeException if no such attribute exists'''
    attr = self.element.attribute(attributeName)
    if attr==None:
      raise InvalidCokoonThemeException('%s attribute %s not specified'%(self.tag, attributeName), self)
    return attr

class CokoonThemeSource(CokoonDomElement):
  def __init__(self, node, parent):
    CokoonDomElement.__init__(self, node, parent, 'source')
  def id(self):   return self._domAttr('id')
  def type(self): return self._domAttr('type')
  def file(self): return self._domAttr('file')
#  TODO: <replace_style_attr element_id="rect2788" attr="fill" value="ColorTitleBar"/>...

class CokoonThemeExpression(CokoonDomElement):
  def __init__(self, node, parent):
    CokoonDomElement.__init__(self, node, parent, 'expression')
  def id(self):    return self._domAttr('id')
  def value(self): return self._domAttr('value')

class CokoonThemeTile(CokoonDomElement):
  def __init__(self, node, parent):
    CokoonDomElement.__init__(self, node, parent, 'tile')
  def id(self):    return self._domAttr('id')
  def source_id(self): return self._domAttr('source_id')
  def mode_hor(self): return self._domAttr('mode_hor')
  def mode_vert(self): return self._domAttr('mode_vert')
  def left(self): return self._domAttr('left')
  def top(self): return self._domAttr('top')
  def width(self): return self._domAttr('width')
  def height(self): return self._domAttr('height')

class CokoonThemeLayer(CokoonDomElement):
  def __init__(self, node, parent):
    CokoonDomElement.__init__(self, node, parent, 'layer')
  def layout_id(self):    return self._domAttr('layout_id')

class CokoonThemeLayout(CokoonDomElement):
  def __init__(self, node, parent):
    CokoonDomElement.__init__(self, node, parent, 'layout')
  def id(self):    return self._domAttr('id')
#  TODO: column,  row,  cell

class CokoonThemePaint(CokoonDomElement):
  def __init__(self, node, parent):
    CokoonDomElement.__init__(self, node, parent, 'paint')
  def createChildNode(self, node):
    '''reimplemented from CokoonDomNode'''
    if node.isElement():
      tag = node.toElement().tagName()
      if tag == 'layer':
        return CokoonThemeLayer(node, self)
    return CokoonDomElement.createChildNode(self, node)
#
#  def layers(self):
#    items = []
#    child = self.node.firstChildElement('layer')
#    while not child.isNull():
#      tag = child.tagName()
#      if tag=='layer':
#        items.append(CokoonThemeLayer(child, self))
#    return items

class CokoonThemeObject(CokoonDomElement):
  def __init__(self, node, parent):
    CokoonDomElement.__init__(self, node, parent, 'object')
  def id(self):
    return self._domAttr('id')
  def createChildNode(self, node):
    '''reimplemented from CokoonDomNode'''
    if node.isElement():
      tag = node.toElement().tagName()
      if tag=='source':
        return CokoonThemeSource(node, self)
      elif tag=='expression':
        return CokoonThemeExpression(node, self)
      elif tag=='tile':
        return CokoonThemeTile(node, self)
      elif tag=='layout':
        return CokoonThemeLayout(node, self)
      elif tag=='paint':
        return CokoonThemePaint(node, self)
    return CokoonDomElement.createChildNode(self, node)

#  def items(self, tagName=''):
#    items = []
#    child = self.node.firstChildElement(tagName)
#    while not child.isNull():
#      tag = child.tagName()
#      if tag=='source':
#        items.append(CokoonThemeSource(child, self))
#      elif tag=='expression':
#        items.append(CokoonThemeExpression(child, self) )
#      elif tag=='tile':
#        items.append(CokoonThemeTile(child, self))
#      elif tag=='layout':
#        items.append(CokoonThemeLayout(child, self))
#      elif tag=='paint':
#        items.append(CokoonThemePaint(child, self))
#      child = child.nextSiblingElement(tagName)
#    return items 

class CokoonTheme(CokoonDomElement):
  def __init__(self, node, parent):
    CokoonDomElement.__init__(self, node, parent, 'cokoon_theme')
  def name(self):
    return self._domAttr('name')
  def spec(self):
    return self._domAttr('spec')
  def version(self):
    return self._domAttr('version')
  def createChildNode(self, node):
    '''reimplemented from CokoonDomNode'''
    if node.isElement():
      tag = node.toElement().tagName()
      if tag=='object':
        return CokoonThemeObject(node, self)
    return CokoonDomElement.createChildNode(self, node)

#  def objects(self):
#    objs = []
#    child = self.node.firstChildElement("object")
#    while not child.isNull():
#      objs.append(CokoonThemeObject(child, self))
#      child = child.nextSiblingElement("object")
#    return objs
#  def object(self, objId):
#    for o in self.objects():
#      if o.id == objId:

class ThemeModel(QAbstractItemModel):
  '''This model implements a tree-like model which
  represents a Cokoon theme.'''

  def __init__(self):
    QAbstractItemModel.__init__(self, )
    self.doc = QDomDocument()
    self.rootElement = None
    self._modified = False

    self.spec = None
    self.cokoonDoc = None

  def loadFile(self, file):
    '''Loads a theme description file (must be QFile)'''
    if not self.doc.setContent(file):
      self.clear()
      return False

    
    ## theme model initialization
    self.rootElement = CokoonTheme(self.doc.firstChildElement(), None)
    self.rootElement.row = 0

    ## make sure there is always a corresponding theme specification
    # TODO: do not hardcode the spec directory path...
    self.spec = ThemeSpec("/usr/share/apps/cokoon/specs/" + self.theme().spec() + ".xml")
    self.cokoonDoc = ThemeSpecDocument(self.spec)
    self.cokoonDoc.loadTheme(file.fileName())

    ## theme modifications...
    self._modified = False
    
    return True

  def theme(self):
    return self.rootElement

  def clear(self):
    '''Clears the current document'''
    self.doc.setContent("")
    self.rootElement = None

  def save(self, file):
    '''Saves the theme description to file (must be QFile)'''
    outstr = QTextStream(file)
    outstr << self.doc.toString()

  def isModified(self):
    '''Whether the document has been modified'''
    return self._modified

  def flags(self, index):
    '''reimplemented from QAbstractItemModel'''
    if not index.isValid():
      return QtCore.Qt.ItemIsEnabled
    
    return QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable

  def headerData(self, section, orientation, role):
    if orientation == QtCore.Qt.Horizontal and role == QtCore.Qt.DisplayRole:
      if section == 0:
          return QtCore.QVariant(self.tr("Item"))
      elif section == 1:
          return QtCore.QVariant(self.tr("ID"))
      elif section == 2:
          return QtCore.QVariant(self.tr("Value"))
      else:
          return QtCore.QVariant()
    
    return QtCore.QVariant()

  def data(self, index, role):
    '''reimplemented from QAbstractItemModel'''
    if not index.isValid():
      return QtCore.QVariant()
    if role != QtCore.Qt.DisplayRole:
      return QtCore.QVariant()
    
    item = index.internalPointer()

    node = item.node

    if index.column() == 0:
      return QtCore.QVariant(node.nodeName())
    
    elif index.column() == 1:
      return QtCore.QVariant(item.displayId() )
    elif index.column() == 2:
      return QtCore.QVariant(item.displayValue() )

    else:
      return QtCore.QVariant()

  def index(self, row, column, parent):
    '''reimplemented from QAbstractItemModel'''
    if not parent.isValid():
      parentItem = self.rootElement
    else:
      parentItem = parent.internalPointer()

    childItem = parentItem.child(row)
    if childItem:
      return self.createIndex(row, column, childItem)
    else:
      return QtCore.QModelIndex()

  def parent(self, child):
    '''reimplemented from QAbstractItemModel'''
    if not child.isValid():
      return QtCore.QModelIndex()
    
    childItem = child.internalPointer()
    parentItem = childItem.parent
    
    if not parentItem or parentItem == self.rootElement:
      return QtCore.QModelIndex()
    
    return self.createIndex(parentItem.row, 0, parentItem)

  def columnCount(self, parent):
    '''reimplemented from QAbstractItemModel'''
    return 3

  def rowCount(self, parent):
    '''reimplemented from QAbstractItemModel'''
    if not parent.isValid():
      parentItem = self.rootElement
    else:
      parentItem = parent.internalPointer()
    
    return len(parentItem.children())