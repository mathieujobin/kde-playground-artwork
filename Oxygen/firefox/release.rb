#!/usr/bin/env ruby
#
# Generates a release tarball for the firefox theme
#
# Copyright (C) 2007-2008 Harald Sitter <harald@oxygen-icons.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License or (at your option) version 3 or any later version
# accepted by the membership of KDE e.V. (or its successor approved
# by the membership of KDE e.V.), which shall act as a proxy
# defined in Section 14 of version 3 of the license.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

require 'fileutils'

if $*[0] == nil
    puts("please provide a version number")
    exit 1
else
    version = $*[0]
end

jar = "Oxygen-#{version}.jar"
releasedir = "release-#{version}"
system("svn export Oxygen #{releasedir}")

Dir.chdir(releasedir)

file = File.new( "install.rdf", File::RDWR )
str = file.read()
file.rewind()
file.truncate( 0 )
str.sub!( /<em:version>.*<\/em:version>/, "<em:version>#{version}</em:version>" )
file << str
file.close()

system("zip -r ../#{jar} *")
Dir.chdir("../")

FileUtils.rm_r(releasedir)

hash = %x[sha1sum #{jar}].chomp!
puts hash
hash = hash.split(" ")[0]

c = File.open("install.html").read
c = c.gsub(/a href=\".*\"/, "a href=\"#{jar}\"")
c = c.gsub(/hash=\"sha1:.*\"/,"hash=\"sha1:#{hash}\"")
File.new("install.html","w").print(c)

exit 0
