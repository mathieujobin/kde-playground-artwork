#!/usr/bin/env ruby
#
# Update icon urls based on array of data
#
# Copyright (C) 2009 Harald Sitter <harald@oxygen-icons.org>
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

replaces = {
    "gtk-dialog-error" => "dialog-error",
    "gtk-dialog-warning" => "dialog-warning",
    "gtk-dialog-info" => "dialog-information",
    "gtk-info" => "dialog-information",
    "gtk-close" => "dialog-close",
    "gtk-cancel" => "dialog-cancel",
    "gtk-no" => "dialog-cancel",
    "gtk-yes" => "dialog-ok",
    "gtk-ok" => "dialog-ok",
    "gtk-apply" => "dialog-ok-apply",

    "gtk-go-forward-ltr" => "go-next",
    "gtk-go-forward-rtl" => "go-previous",
    "gtk-go-back-ltr" => "go-previous",
    "gtk-go-back-rtl" => "go-next",
    "gtk-go-up" => "go-up",
    "gtk-goto-first-ltr" => "go-first",
    "gtk-goto-first-rtl" => "go-last",
    "gtk-goto-last-ltr" => "go-last",
    "gtk-goto-last-rtl" => "go-first",
    "gtk-home" => "go-home",

    "gtk-clear" => "edit-clear",
    "gtk-find" => "edit-find",
    "gtk-undo" => "edit-undo",
    "gtk-redo" => "edit-redo",
    "gtk-cut" => "edit-cut",
    "gtk-copy" => "edit-copy",
    "gtk-paste" => "edit-paste",
    "gtk-delete" => "edit-delete",
    "gtk-select-all" => "edit-select-all",

    "gtk-help" => "help-contents",

    "gtk-open" => "document-open",
    "gtk-save" => "document-save",
    "gtk-print" => "document-print",
    "gtk-print-preview" => "document-print-preview",
    "gtk-save-as" => "document-save-as",
    "gtk-revert-to-saved" => "document-revert",

    "gtk-select-font" => "preferences-desktop-font",

    "gtk-color-picker" => "color-picker",

    "gtk-network" => "network-workgroup",

    "gtk-add" => "list-add",
    "gtk-remove" => "list-remove",

    "gtk-refresh" => "view-refresh",

    "gtk-properties" => "configure",
    "gtk-preferences" => "configure",

    "gtk-file" => "text-x-generic",
    "gtk-directory" => "folder",

    "gtk-stop" => "process-stop",

    "gtk-media-pause" => "media-playback-pause",
    "gtk-media-play" => "media-playback-play",

    "gtk-help" => "help-about",
    "gtk-about" => "help-about",

    "gtk-zoom-in" => "zoom-in",
    "gtk-zoom-out" => "zoom-out",
    "gtk-zoom-100" => "zoom-original",
}

def rep(c,size,r,ry)
    c = c.gsub(
        "(\"moz-icon://stock/#{r}?size=#{size}\")",
        "(\"chrome://browser/skin/icons/#{ry}.png\")"
    )

    if File.exist?("Oxygen/chrome/browser/icons/disabled/#{ry}.png")
        c = c.gsub(
            "(\"moz-icon://stock/#{r}?size=#{size}&state=disabled\")",
            "(\"chrome://browser/skin/icons/disabled/#{ry}.png\")"
        )
    end

    return c
end

for file in Dir.glob("Oxygen/**/**.css")
    c = File.open(file).read
    replaces.each do | r, ryc |
        ry = "16/" + ryc
        ["menu","button","dialog","dnd"].each do | size |
            c = rep(c,size,r,ry)
        end

        ry = "24/" + ryc
        c = rep(c,"toolbar",r,ry)
    end
    File.new(file,"w").print(c)
end
