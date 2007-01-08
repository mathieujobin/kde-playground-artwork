#!/bin/bash

#Prepare the layout
mkdir oxygen
mkdir oxygen/cursors
cat > oxygen/index.theme << EOF
[Icon Theme]
Name = Oxygen
Comment = The Oxygen mouse theme. Oxygenize your desktop!
EOF

#SVG -> PNG
mkdir oxygen/tmp-sources
cd oxygen/tmp-sources

for icon in $(ls ../../svgs/*.svg); do
inkscape --without-gui --export-png=$( echo $icon | sed s/......svgs.//|sed s/.svg// ).png --export-dpi=72 --export-background-opacity=0 --export-width=32 --export-height=32 $icon > /dev/null
done

#PNG+config file = cursor file
#Warning, still in ./oxygen/tmp-sources
cp ../../configs/* .

for cursor in $(ls *.in); do
xcursorgen $cursor ../cursors/$( echo $cursor | sed s/.in// )
done

cd ../..
rm -rf oxygen/tmp-sources

#Package and remove all the temp stuff
tar -cf oxygen$(date '+%F-%H-%M').tar oxygen
gzip oxygen$(date '+%F-%H-%M').tar
rm -rf oxygen

