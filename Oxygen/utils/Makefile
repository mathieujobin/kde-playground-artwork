#!/usr/bin/make -f
#
# Makefile for the KDE oxygen theme
# by Raphaël Pinson <raphink@kubuntu.org>
#
# 
# Usage:
# 
# To build a source tarball:
# make distclean && make sourcedist 
#
# To build and make a tarball (including png):
# make clean && make dist
#
# To build and install into your system:
# make clean && make && sudo make install
#
#
# Copyright (C) 2006, Raphaël Pinson
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

THEME=oxygen
ICONSDIR=$(DESTDIR)/usr/share/icons
OXYGENDIR=$(ICONSDIR)/$(THEME)
BUILDDIR=$(CURDIR)/$(THEME)
SVGDIR=scalable
PNGSECTDIR=$(BUILDDIR)/$${size}x$${size}/$${section}
SVGSECTDIR=$(BUILDDIR)/$(SVGDIR)/$${section}

DATE=`/bin/date '+%Y%m%d'`
INDEX=$(CURDIR)/index.theme
TARBALL=$(CURDIR)/../kde-icons-$(THEME)_svn$(DATE).tar.gz

MKDIR=/bin/mkdir -p
RM=/bin/rm -rf
TAR=/bin/tar cvzf
LS=/bin/ls
GREP=/bin/grep
TOUCH=/usr/bin/touch
CP=/bin/cp
FIND=/usr/bin/find

# CONVERT=rsvg-convert -h $${size} -w $${size} -d 72 -p 72 -o $${pngfile} $${svgfile}
CONVERT=inkscape --without-gui --export-png=$${pngfile} --export-dpi=72 --export-background-opacity=0 --export-width=$${size} --export-height=$${size} $${svgfile} >/dev/null

SECTIONS=actions apps devices filesystems mimetypes
SIZES=16 22 32 48 64 128
SMALL=16 22

build: build-stamp

install:
	$(MKDIR) $(OXYGENDIR)
	$(CP) -r $(BUILDDIR)/* $(OXYGENDIR)

distclean:
	-$(FIND) $(CURDIR) -name .svn | xargs $(RM)

clean: distclean
	$(RM) $(BUILDDIR)
	$(RM) build-stamp

sourcedist:
	$(TAR) $(CURDIR)/../$(TARBALL) $(CURDIR)

dist: build-stamp
	$(TAR) $(TARBALL) $(THEME)

build-stamp:
	$(MKDIR) $(BUILDDIR)
	$(CP) $(INDEX) $(BUILDDIR)
	for section in $(SECTIONS); do \
	  $(MKDIR) $(SVGSECTDIR); \
	  $(CP) -r $(CURDIR)/svg/$${section}/* $(SVGSECTDIR); \
	  for size in $(SIZES); do \
	    $(MKDIR) $(PNGSECTDIR); \
	    $(LS) $(CURDIR)/svg/$${section} | $(GREP) ".svg$$" | while read image; do \
	      pngfile="$(PNGSECTDIR)/`basename $${image} svg`png"; \
	      svgfile="$(CURDIR)/svg/$${section}/$${image}"; \
	      $(CONVERT); \
	    done; \
	  done; \
	  for size in $(SMALL); do \
	    $(MKDIR) $(PNGSECTDIR); \
	    $(LS) $(CURDIR)/svg/$${section}/small/$${size}x$${size} | $(GREP) ".svg$$" | while read image; do \
	      pngfile="$(PNGSECTDIR)/`basename $${image} svg`png"; \
	      svgfile="$(CURDIR)/svg/$${section}/small/$${size}x$${size}/$${image}"; \
	      $(CONVERT); \
	    done; \
	  done; \
	done
	$(TOUCH) $@


