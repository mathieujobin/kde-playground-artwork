#!/bin/bash

echo $( echo $1 | cut -d . -f -1 )

  inkscape --without-gui --export-png="cr128-"$( echo $1 | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=128 --export-height=128 $1 > /dev/null


  convert -filter Sinc -resize 64x64 "cr128-"$( echo $1 | cut -d . -f -1 ).png "./cr64-"$( echo $1 | cut -d . -f -1 ).png 
  convert -filter Sinc -resize 48x48 "cr128-"$( echo $1 | cut -d . -f -1 ).png "./cr48-"$( echo $1 | cut -d . -f -1 ).png
  convert -filter Sinc -resize 32x32 "cr128-"$( echo $1 | cut -d . -f -1 ).png "./cr32-"$( echo $1 | cut -d . -f -1 ).png 
  convert -filter Sinc -resize 22x22 "cr128-"$( echo $1 | cut -d . -f -1 ).png "./cr22-"$( echo $1 | cut -d . -f -1 ).png
  convert -filter Sinc -resize 16x16 "cr128-"$( echo $1 | cut -d . -f -1 ).png "./cr16-"$( echo $1 | cut -d . -f -1 ).png



exit 0
