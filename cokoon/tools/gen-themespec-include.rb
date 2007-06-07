#!/usr/bin/env ruby

require "rexml/document"

class ThemeSpec
  def initialize(specFileName)
    i = File.new( $*[0] )
    @doc = REXML::Document.new i
  end

  def specName
    @doc.root.attributes["name"]
  end

  def specVersion
    @doc.root.attributes["version"]
  end

  def specItems
    items = Array.new
    itemsAndStates = Array.new

    @doc.elements.each("cokoon_spec/item") { |eItem|
      itemId = eItem.attributes["id"]

      items << itemId

      stateList = Array.new()
      eItem.elements.each("states") { |eStates|
        stateListNode = Array.new()

        eStates.elements.each("state") { |eState|
          stateListNode << eState.attributes["id"]
        }

        stateList << stateListNode
      }

      itemsAndStates << [itemId, stateList]
    }

    return itemsAndStates
  end

  def specStates
    states = Array.new()

    @doc.elements.each("cokoon_spec/item") { |eItem|
      itemId = eItem.attributes["id"]

      stateList = Array.new()
      eItem.elements.each("states") { |eStates|
        stateListNode = Array.new()

        eStates.elements.each("state") { |eState|
          stateListNode << eState.attributes["id"]
        }

        stateList << stateListNode
      }

      states << stateList
    }

    return states
  end

  def specVariables
    attributeList("id","cokoon_spec/item/provided/variable")
  end

  def specExpressions
    attributeList("id","cokoon_spec/item/required/expression")
  end

  def specTiles
    attributeList("id","cokoon_spec/item/required/tile")
  end

  def specSpecialCells
    attributeList("id","cokoon_spec/item/provided/special_cell")
  end


  def attributeList(attr,elementPath)
    list = Array.new()
    @doc.elements.each(elementPath) { |v|
      list << v.attributes[attr]
    }
    return list
  end

end

if $*.size != 2
	puts "usage: gen-themespec-include.rb spec.xml output-path"
	exit();
end

if !FileTest.exist?($*[0])
	puts "Theme Specification input file " + i.filename + " does not exist."
	exit();
end

spec = ThemeSpec.new($*[0])

i = File.new( $*[0] )

#
# Read theme spec...
#

# get items and states
items = spec.attributeList("id","cokoon_spec/item")
states = spec.specStates
itemsAndStates = spec.specItems

#
# Generate include file...
#

o = File.new( $*[1] + "/" + spec.specName + ".h", 'w')

o.puts "#ifndef " + spec.specName + "_H"
o.puts "#define " + spec.specName + "_H"

o.puts "namespace " + spec.specName + " {"

# variables/expressions/item/state enums for index-based access...

o.puts "namespace Var {"
o.puts "enum Variables {"
spec.specVariables.each { |v|
	o.puts "  " + v + ","
}
o.puts "  Variables__Count"
o.puts "};"
o.puts "}"
o.puts "enum Identifiers {"
spec.specExpressions.each { |e|
	o.puts "  Exp_" + e + ","
}
spec.specTiles.each { |e|
	o.puts "  Tile_" + e + ","
}
spec.specSpecialCells.each { |e|
	o.puts "  Special_" + e + ","
}
o.puts "  Identifiers__Count"
o.puts "};"

# helper method...
def Math.logn( x, n ); Math.log( x ) / Math.log( n ); end
# Used in combination with columnItemId().
def nextColumnBase(currentColumnId)
  return Math.logn(currentColumnId+1,2).ceil()
end
# Given are items (numbered from 0 up) in lists, which are itself
# arranged in columns. This function takes an item number n, and a
# column base (obtained using nextColumnBase() ).  It returns a new id
# for the item, which - when added to items from other columns - is
# again a unique id which represents the set of those items.
def columnItemId(n,columnBase)
  return n*(2**columnBase)
end

def writeEnum(indentLevel, fd, name, elements)
  i = "  "*indentLevel
  cnt = 0
  fd.puts i + "enum " + name + " {"
  elements.each { |e|
    sep = ","
    sep = "" if (cnt >= elements.size-1)
    cnt += 1
    fd.puts i + "  " + e + sep
  }
  fd.puts i + "};"
end

def writeSwitch(indentLevel, fd, switchVar, cases)
  i = "  "*indentLevel
  o.puts i + "switch(objId) {"
  cases.each { |c|
    o.puts i*2 + c.key() + ": {"
    o.puts i*2 + c.value()
    o.puts i*2 + "break;"
    o.puts i*2 + "}"
  }
end


#extend the Enumerable mixin..
module Enumerable
  def mapValueAndIndex
    cnt = 0
    map { |el|
      val = yield(el, cnt)
      cnt += 1
      val
    }
  end
end

colBase = 0
highestIndex = 0

## write items enum ##

elements = items.mapValueAndIndex { |v,i|
  highestIndex = columnItemId(i,colBase)
  v + " = " + highestIndex.to_s
}
elements << "Items__Count"
writeEnum(0, o, "Items", elements)

## write states enums ##

itemsAndStates.each { |itemAndStateList|
  itemName = itemAndStateList[0]
  stateList = itemAndStateList[1]

  colBase = nextColumnBase(highestIndex)
  highestStateIndex = highestIndex

  stateLevel = 0

  stateList.each { |s|
    elements = s.mapValueAndIndex { |v,i|
      highestStateIndex = columnItemId(i,colBase)
      itemName + stateLevel.to_s + "_" + v + " = " + highestStateIndex.to_s
    }
    writeEnum(0, o, itemName + "States" + stateLevel.to_s, elements)

    colBase = nextColumnBase(highestStateIndex)

    stateLevel += 1
  }

}


o.puts "class SpecDocument : public Cokoon::Document {"
o.puts "protected:"
o.puts "int mapToId(Cokoon::Document::DeclarationType type,const QString &str) const {"
o.puts "  switch(type) {"
o.puts "  case Cokoon::Document::ObjectNameDecl:"
o.puts "    if (false) { return -1; }"
items.each {
  |v|
o.puts '    else if (str == "' + v + '") { return ' + v + '; }'
}
o.puts '    break;'

o.puts "  case Cokoon::Document::VariableDecl:"
o.puts "    if (false) { return -1; }"
spec.specVariables.each {
  |v|
o.puts '    else if (str == "' + v + '") { return Var::' + v + '; }'
}
o.puts '    break;'
o.puts "  case Cokoon::Document::IdentifierDecl:"
o.puts "    if (false) { return -1; }"
spec.specExpressions.each {
  |v|
o.puts '    else if (str == "' + v + '") { return Exp_' + v + '; }'
}
spec.specTiles.each {
  |v|
o.puts '    else if (str == "' + v + '") { return Tile_' + v + '; }'
}
spec.specSpecialCells.each {
  |v|
o.puts '    else if (str == "' + v + '") { return Special_' + v + '; }'
}
o.puts "    break;"
o.puts "  }"
o.puts "return Cokoon::Document::mapToId(type,str); }"
o.puts "int customIdMappingBase(Cokoon::Document::DeclarationType type) const {"

o.puts "  switch(type) {"
o.puts "  case Cokoon::Document::ObjectNameDecl: return Items__Count;"
o.puts "  case Cokoon::Document::VariableDecl:   return Var::Variables__Count;"
o.puts "  case Cokoon::Document::IdentifierDecl: return Identifiers__Count;"
o.puts "  }"

o.puts "return Cokoon::Document::customIdMappingBase(type); }"

o.puts "    int objectStateLevels(int objId) const {"
o.puts "        switch(objId) {"
itemsAndStates.each { |itemAndState|
i = itemAndState[0]
s = itemAndState[1]
o.puts "        case " + i + ":"
o.puts "            return " + s.size.to_s + ";"
}
o.puts "        default: return -1;"
o.puts "        }"
o.puts "    }"
o.puts "    int objectStateLevelStates(int itemId, int stateLevel) const {"
o.puts "        switch(itemId) {"
itemsAndStates.each { |itemAndState|
i = itemAndState[0]
s = itemAndState[1]
o.puts "        case " + i + ":"
o.puts "            switch(stateLevel) {"
cnt = 0;
itemAndState[1].each { |state|
o.puts "            case " + cnt.to_s + ": return " + state.size.to_s + ";"
cnt += 1
}
o.puts "            default: return -1;"
o.puts "            }"

}
o.puts "        default: return -1;"
o.puts "        }"
o.puts "    }"

o.puts "    int mapObjectStateToId(int itemId, int stateLevel, const QString &stateName) const {"
o.puts "        switch(itemId) {"
itemsAndStates.each { |itemAndState|
i = itemAndState[0]
s = itemAndState[1]
o.puts "        case " + i + ":"
#itemCount += 1
o.puts "            switch(stateLevel) {"
cnt = 0;
itemAndState[1].each { |state|
o.puts "            case " + cnt.to_s + ":"
cnt2 = 0
state.each {
  |stNode|
  o.puts '              if (stateName == "' + stNode + '") { return ' + i + cnt.to_s + '_' + stNode + '; }'
#  o.puts '              if (stateName == "' + stNode + '") { return ' + cnt2.to_s + '; }'
  cnt2 += 1
}
cnt += 1
o.puts "                else return -1;"
o.puts "             break;"
}
o.puts "            }"
o.puts "            break;"
}
o.puts "        default: return -1;"
o.puts "        }"

o.puts "    }"
o.puts "    int mapObjectStateToId(int itemId, int stateLevel, int stateNumber) const {"
o.puts "        switch(itemId) {"
itemsAndStates.each { |itemAndState|
i = itemAndState[0]
s = itemAndState[1]
o.puts "        case " + i + ":"
o.puts "            switch(stateLevel) {"
cnt = 0
itemAndState[1].each { |state|
o.puts "            case " + cnt.to_s + ":"
cnt2 = 0
o.puts "                switch(stateNumber) {"
state.each {
  |stNode|
  o.puts "                case " + cnt2.to_s + ': return ' + i + cnt.to_s + "_" + stNode + ';'
  cnt2 += 1
}

cnt += 1

o.puts "                default: return -1;"
o.puts "                }"
o.puts "             break;"
}

o.puts "            }"
}
o.puts "        default: return -1;"
o.puts "        }"

o.puts "    }"

o.puts "};"

o.puts "}"
o.puts "#endif // " + spec.specName + "_H"



