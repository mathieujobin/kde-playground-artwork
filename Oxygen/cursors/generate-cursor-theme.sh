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


#PNG+config file = cursor file


tar -cf oxygen.tar oxygen
gzip oxygen.tar
rm oxygen.tar
rm -rf oxygen

