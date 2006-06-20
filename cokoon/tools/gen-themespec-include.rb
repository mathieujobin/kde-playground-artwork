#!/usr/bin/env ruby

require "rexml/document"

if $*.size != 2
	puts "usage: gen-themespec-include.rb spec.xml output-path"
	exit();
end

if !FileTest.exist?($*[0])
	puts "Theme Specification input file " + i.filename + " does not exist."
	exit();
end

i = File.new( $*[0] )

#
# Read theme spec...
#

doc = REXML::Document.new i

specName    = doc.root.attributes["name"]
specVersion = doc.root.attributes["version"]

# get items and states
items = Array.new()
states = Array.new()

doc.elements.each("cokoon_spec/item") { |eItem|
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

	states << stateList
}

# get variables
variablesSet = Hash.new()
doc.elements.each("cokoon_spec/item/provided/variable") { |eVar|
	variablesSet[eVar.attributes["id"] ] = true
}
variables = variablesSet.keys()
#get expressions
expressionsSet = Hash.new()
doc.elements.each("cokoon_spec/item/required/expression") { |eExp|
	expressionsSet[eExp.attributes["id"] ] = true
}
expressions = expressionsSet.keys()
#get tiles
tilesSet = Hash.new()
doc.elements.each("cokoon_spec/item/required/tile") { |eTile|
	tilesSet[eTile.attributes["id"] ] = true
}
tiles = tilesSet.keys()
#get special cells
specialsSet = Hash.new()
doc.elements.each("cokoon_spec/item/provided/special_cell") { |eSp|
	specialsSet[eSp.attributes["id"] ] = true
}
specials = specialsSet.keys()


#
# Generate include file...
#

o = File.new( $*[1] + "/" + specName + ".h", 'w')

o.puts "#ifndef " + specName + "_H"
o.puts "#define " + specName + "_H"

o.puts "namespace " + specName + " {"

# variables/expressions/item/state enums for index-based access...

o.puts "namespace Var {"
o.puts "enum Variables {"
variables.each { |v|
	o.puts "  " + v + ","
}
o.puts "  Variables__count"
o.puts "};"
o.puts "}"
o.puts "enum Identifiers {"
expressions.each { |e|
	o.puts "  Exp_" + e + ","
}
tiles.each { |e|
	o.puts "  Tile_" + e + ","
}
specials.each { |e|
	o.puts "  Special_" + e + ","
}
o.puts "  Identifiers__Count"
o.puts "};"

o.puts "enum Items {"
0.upto(items.size-1) { |i|
	itemStr = items[i]
	itemStr += " = " + ((i+1)*10**0).to_s
	itemStr += "," if i != (items.size-1)
	o.puts "  " + itemStr
}
o.puts "};"

itemIdxBase = 0
itemCount = items.size
while itemCount>10
	itemCount /= 10
	itemIdxBase += 1
end

0.upto(items.size-1) { |i|
	itemName = items[i]

	stateIdxBase = itemIdxBase+1

	0.upto(states[i].size-1) { |j|
		o.puts "enum " + itemName + "States" + j.to_s + " {"
		0.upto(states[i][j].size-1) { |z|
			stateStr = itemName + j.to_s + "_" + states[i][j][z]
			stateStr += " = " + ((z+1)*10**stateIdxBase).to_s
			stateStr += "," if z != (states[i][j].size-1)
			o.puts "  " + stateStr
		}
		o.puts "};"

		stateIdxBase += 1
		stateCount = states[i][j].size
		while stateCount>10
			stateCount /= 10
			stateIdxBase += 1
		end
	}
}

o.puts "void setupItems(Cokoon::Document *doc) {"
o.puts "  QList<QHash<QString,int> > itemStateList;"
o.puts "  QHash<QString,int> itemStateListStates;"
0.upto(items.size-1) { |i|

	o.puts "  itemStateList.clear();"

	0.upto(states[i].size-1) { |j|
		o.puts "  itemStateListStates.clear();"
		states[i][j].each { |state|
			o.puts '  itemStateListStates.insert("' + state + '", ' + items[i]+ j.to_s + "_"+state + ');'
		}
		o.puts "  itemStateList.append(itemStateListStates);"
  }

	o.puts '  doc->declareObject("' + items[i] + '", itemStateList);'
	
}
variables.each { |v|
	o.puts '  doc->declareVariable("' + v + '", Var::' + v + ');'
}
expressions.each { |v|
	o.puts '  doc->declareIdentifier("' + v + '", Exp_' + v + ');'
}
tiles.each { |v|
	o.puts '  doc->declareIdentifier("' + v + '", Tile_' + v + ');'
}
specials.each { |v|
	o.puts '  doc->declareIdentifier("' + v + '", Special_' + v + ');'
}

o.puts "}"
o.puts "}"
o.puts "#endif // " + specName + "_H"



