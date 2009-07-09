#!/usr/bin/env ruby
#
# Copyright (C) 2009 Harald Sitter <apachelogger@ubuntu.com>
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

begin
    require 'RMagick'
rescue LoadError
    puts "Pardon mon ami, but you really should install librmagick-ruby.\nkthxbai"
    exit 1
end

Dir.glob("Oxygen/chrome/browser/icons/*/*.png").each do |file|
    dir, name = file.split("/")[-2..-1]
    next if File.exist?("Oxygen/chrome/browser/icons/disabled/#{dir}/#{name}")
    i = Magick::ImageList.new(file)
    i.quantum_operator(Magick::MultiplyQuantumOperator, 0.50, Magick::AlphaChannel)
    i.colorspace=Magick::GRAYColorspace
    i.write("Oxygen/chrome/browser/icons/disabled/#{dir}/#{name}")
    puts "#{name} done"
end
