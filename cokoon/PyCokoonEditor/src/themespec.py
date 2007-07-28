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

from PyQt4 import QtXml
from PyQt4.QtCore import QObject, QFile
from PyQt4.QtGui import QMessageBox

import math

from cokoon import Cokoon

class ThemeSpecItem:
  def __init__(self):
    self.id = ''
    self.providedVariables = []
    self.providedSpecials = []
    self.requiredExpressions = []
    self.requiredTiles = []
    self.states = []            # Format: [Levels of [StateIds]]

def _nextColumnBase(currentColumnId):
  return math.ceil(math.log(currentColumnId+1,2) )
def _columnItemId(n,columnBase):
  return int(n*(2**columnBase))



class ThemeSpec:
  def __init__(self,themeSpecFile):
    self.readThemeSpec(themeSpecFile)
  def clearThemeSpec(self):
    self.name = ''
    self.items = []             # Format: [ThemeSpecItem]
    self.clearThemeSpecMappings()

  def clearThemeSpecMappings(self):
    self.specObjNameToId = {}
    self.specVariableToId = {}
    self.specIdentifierToId = {}
    self.specObjIdToStateLevels = {}
    self.specObjIdBase = 0

  def readThemeSpec(self,themeSpecFile):
    self.clearThemeSpec()
    handler = ThemeSpecHandler(self)
    reader = QtXml.QXmlSimpleReader()
    reader.setContentHandler(handler)
    reader.setErrorHandler(handler)
    
    file = QFile(themeSpecFile)
    if not (file.open(QFile.ReadOnly | QFile.Text)):
      # TODO: do throw an exception etc.
      print "Cannot read file "
      print themeSpecFile
      print file.errorString()
      return
    xmlInputSource = QtXml.QXmlInputSource(file)
    ret = reader.parse(xmlInputSource, True)
    if ret:
      self.genCokoonSpecMappings()
#     self.debug()

  def debug(self):
    """Used to print out some information about the ThemeSpec"""
    print "ThemeSpec.debug:"
    print "self.name",self.name
    print "len(self.items)",len(self.items)
    print "len(self.specObjNameToId)",len(self.specObjNameToId)
    print "len(self.specVariableToId)",len(self.specVariableToId)
    print "len(self.specIdentifierToId)",len(self.specIdentifierToId)
    print "self.specObjIdBase",self.specObjIdBase

  def genCokoonSpecMappings(self):
    self.clearThemeSpecMappings()

    maxObjAccessId = 0                # used to compute the objIdBase...

    objectId = 0
    identifierId = 0
    variableId = 0
    for i in self.items:
      self.specObjNameToId[i.id] = objectId

      # identifiers address expression, special_cell, tile
      for exp in i.requiredExpressions:
        self.specIdentifierToId[exp[0]] = identifierId
        identifierId += 1
      for special in i.providedSpecials:
        self.specIdentifierToId[special] = identifierId
        identifierId += 1
      for tile in i.requiredTiles:
        self.specIdentifierToId[tile] = identifierId
        identifierId += 1
      for var in i.providedVariables:
        self.specVariableToId[var[0]] = variableId
        variableId += 1

      objAccessId = objectId    # used compute objIdBase

      stateLevelList = []

      highestIndex = len(self.items)
      colBase = _nextColumnBase(highestIndex)
      highestStateIndex = highestIndex
      for stLvl in i.states:
        stateLevel = {}
        stateCount = 0
        for st in stLvl:
          highestStateIndex = _columnItemId(stateCount,colBase)
#           print str(objectId), " ", st, "/", stateCount, " - ", highestStateIndex
          stateLevel[st] = highestStateIndex
          stateLevel[stateCount] = highestStateIndex

          objAccessId += highestStateIndex
#           print "objAccessId:",objAccessId, " maxObjAccessId:",maxObjAccessId
          if objAccessId > maxObjAccessId:
            maxObjAccessId = objAccessId

          stateCount += 1
#         print stateLevel
        stateLevelList.append(stateLevel)
        colBase = _nextColumnBase(highestStateIndex)
      self.specObjIdToStateLevels[objectId] = stateLevelList

      objectId += 1

    self.specObjIdBase = maxObjAccessId

  def mapObjectStringToSpecIndex(self,str):
    '''Maps specification identifiers in the form of objName.state1.state2...stateN to the numeric spec ID'''
    strSplit = str.split(".")
    print strSplit
    objId = self.specObjNameToId[strSplit[0]]
    print "objId:",objId
    if objId==None or objId<0:
      return -1

    specId = objId

    objStateLevels = self.specObjIdToStateLevels[objId]
    print "objStateLevels:",objStateLevels
    if objStateLevels==None:
      return -1

    cnt = 0
    for st in strSplit[1:]:
      objStateLevelStates = objStateLevels[cnt]
      stLvlId = objStateLevelStates[st]
      print stLvlId
      specId = specId + stLvlId
      cnt=cnt+1

    return specId

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
    elif qName == "special_cell":
      special = attributes.value("id")
      self.currentItem.providedSpecials.append(special)
    elif qName == "tile":
      tile = attributes.value("id")
      self.currentItem.requiredTiles.append(tile)
    elif qName == "variable":
      var = (attributes.value("id"), attributes.value("type"))
      self.currentItem.providedVariables.append(var)
    elif qName == "states":
      self.currentStateList = []
    elif qName == "state":
      self.currentStateList.append( attributes.value("id") )
    
    # TODO ! provided: special_cell, variable
    return True

  def endElement(self,namespaceURI, localName, qName):
    if qName == "item":
      self.themeSpec.items.append(self.currentItem)
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


class ThemeSpecDocument(Cokoon.Document):
  def __init__(self, themeSpec):
    Cokoon.Document.__init__(self)
    self.spec = themeSpec

  def mapToId(self, type,str):
    print "ThemeSpecDocument.mapToId",type,str
    if type == Cokoon.Document.ObjectStateDecl and (str in self.spec.specObjNameToId):
      return self.spec.specObjNameToId[str]
    elif type == Cokoon.Document.VariableDecl and (str in self.spec.specVariableToId):
      return self.spec.specVariableToId[str]
    elif type == Cokoon.Document.IdentifierDecl and (str in self.spec.specIdentifierToId):
      return self.spec.specIdentifierToId[str]
    else:
      return Cokoon.Document.mapToId(self, type,str)

  def objectStateLevels(self, objId):
    if (objId in self.spec.specObjIdToStateLevels):
      return len(self.spec.specObjIdToStateLevels[objId])
    else:
      return -1

  def objectStateLevelStates(self, objId,stateLevel):
    if (objId in self.spec.specObjIdToStateLevels):
      stLvls = self.spec.specObjIdToStateLevels[objId]
      if len(stLvls) > stateLevel:
        return len(stLvls[stateLevel])

    return -1

  def mapObjectStateToId(self, objId,stateLevel,stateName):
    ''' this method is overloaded, stateName can be int or PyQt4.QtCore.QString '''
    if True:
      if (objId in self.spec.specObjIdToStateLevels):
        stLvls = self.spec.specObjIdToStateLevels[objId]
        if len(stLvls) > stateLevel:
          if stateName in stLvls[stateLevel]:
            return stLvls[stateLevel][stateName]
    return -1

  def customIdMappingBase(self, type):
    if type == self.ObjectStateDecl:
      # TODO: change this in gen-themespec.rb as well!
      print "Object ID Base:", self.spec.specObjIdBase
      return self.spec.specObjIdBase
    elif type == self.VariableDecl:
      return len(self.spec.specVariableToId)
    elif type == self.IdentifierDecl:
      return len(self.spec.specIdentifierToId)

    return Cokoon.Document.customIdMappingBase(self, type)

