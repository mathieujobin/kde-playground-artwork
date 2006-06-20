#!/usr/bin/env ruby

require "rexml/document"

if $*.size < 1
	puts "usage: strip-svg.rb file.svg [other.svg]+"
	exit();
end

$*.each { |fileName|
	if !FileTest.exist?(fileName)
		puts "SVG file " + i.filename + " does not exist."
		exit();
	end
	
	file = ""
	
	IO.foreach(fileName) { |l|
		file += l
	}
	
	file.gsub!(/xmlns:inkscape.*=\".*\"\s*/i, "")
	file.gsub!(/xmlns:sodipodi.*=\".*\"\s*/i, "")
	file.gsub!(/inkscape:.*=\".*\"\s*/i, "")
	file.gsub!(/sodipodi:.*=\".*\"\s*/i, "")
	file.gsub!(/<sodipodi:[^>]*>\s/i, "")
	file.gsub!(/<!--.*-->\s/i, "")
	
	o = File.new( fileName, 'w')
	o.puts file
	o.close
}