#!/bin/bash

echo $( echo $1 | cut -d . -f -1 )

  inkscape --without-gui --export-png="cr88-"$( echo $1 | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=88 --export-height=88 $1 > /dev/null

  inkscape --without-gui --export-png="cr22-"$( echo $1 | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=22 --export-height=22 $1 > /dev/null

  inkscape --without-gui --export-png="cr16-"$( echo $1 | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=16 --export-height=16 $1 > /dev/null

exit 0
