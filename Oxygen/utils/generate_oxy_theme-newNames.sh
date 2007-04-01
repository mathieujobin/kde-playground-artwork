#!/bin/bash

sizes="128 64 48 32 22 16"
min_actions=48
min_small=22
folders="apps devices emblems mimetypes places"
date=`date '+%F-%H-%M'`
curdir=`pwd`

#create every size dir and within the size dir create all the icon folders
for size in $sizes; do
   prefix="../oxygen/${size}x${size}"
   folders2=$folders
   if [ $size -le $min_actions ]; then
      folders2="actions "$folders
   fi
   for folder in $folders2; do
      if [ ! -d $prefix/$folder ]; then
         mkdir -p $prefix/$folder
      fi
   done
done

echo "WARNING: This version of the script requires much more resources but gives much better results"
echo "-----------------------------"

for size in $sizes; do
   folders2=$folders
   
   # ===== whether we want action icons for this size
   if [ $size -le $min_actions ]; then
      folders2="actions "$folders
   fi
   # ====== folder list now complete
      
   prefix="../oxygen/${size}x${size}/"
   echo "Exporting to ${size}..." # and smaller (if available)"
   for folder in $folders2; do
      for icon in $(ls $folder/*.svg); do
         
         # create a png icon name
         pngicn="${prefix}${icon%.svg}.png"
         
         # ====== shall we use a small icon if available?
         if [ $size -le $min_small ]; then
            smallicon="$folder/small/${size}x${size}${icon#${folder}}"
            if [ -e "$smallicon" ]; then icon="$smallicon"; fi
         fi
         # ===== small icon decision taken
               
         # only update new items!
         if ( [ ! -e "$pngicn" ] || [ $(stat -c%Y "$icon") -gt $(stat -c%Y "$pngicn") ] ); then
            # the inkscape call
            inkscape --without-gui --export-png="$pngicn" --export-dpi=72 --export-background-opacity=0 --export-width=$size --export-height=$size "$icon" > /dev/null
         fi
            
      done
   done
done

echo "-----------------------------"
echo "Creating index.theme"

#create theme file
cd ../oxygen/
cat > index.theme <<EOF
[Icon Theme]
Name=Oxygen
Name[x-test]=xxOxygenxx
Comment=Oxygen Team - 2006
Comment[nl]=Oxygen-team - 2006
Comment[sv]=Oxygen-gruppen - 2006
Comment[x-test]=xxOxygen Team - 2006xx
DisplayDepth=32

Inherits=hicolor

Example=folder
LinkOverlay=link
LockOverlay=lockoverlay
ShareOverlay=share
ZipOverlay=zip
DesktopDefault=48
DesktopSizes=16,22,32,48,64,128
ToolbarDefault=22
ToolbarSizes=16,22,32,48
MainToolbarDefault=22
MainToolbarSizes=16,22,32,48
SmallDefault=16
SmallSizes=16
PanelDefault=32
PanelSizes=16,22,32,48,64,128
Directories=16x16/actions,16x16/apps,16x16/devices,16x16/places,16x16/mimetypes,22x22/actions,22x22/apps,22x22/places,22x22/mimetypes,32x32/actions,32x32/apps,32x32/devices,32x32/places,32x32/mimetypes,48x48/actions,48x48/apps,48x48/devices,48x48/places,48x48/mimetypes,64x64/actions,64x64/apps,64x64/devices,64x64/places,64x64/mimetypes,128x128/actions,128x128/apps,128x128/devices,128x128/places,128x128/mimetypes,scalable/actions,scalable/apps,scalable/categories,scalable/devices,scalable/emblems,scalable/mimetypes,scalable/places

[16x16/actions]
Size=16
Context=Actions
Type=Threshold

[16x16/apps]
Size=16
Context=Applications
Type=Threshold

[16x16/devices]
Size=16
Context=Devices
Type=Threshold

[16x16/places]
Size=16
Context=Places
Type=Threshold

[16x16/mimetypes]
Size=16
Context=MimeTypes
Type=Threshold

[22x22/actions]
Size=22
Context=Actions
Type=Threshold

[22x22/apps]
Size=22
Context=Applications
Type=Threshold

[22x22/places]
Size=22
Context=Places
Type=Threshold

[22x22/mimetypes]
Size=22
Context=MimeTypes
Type=Threshold

[32x32/actions]
Size=32
Context=Actions
Type=Threshold

[32x32/apps]
Size=32
Context=Applications
Type=Threshold

[32x32/devices]
Size=32
Context=Devices
Type=Threshold

[32x32/places]
Size=32
Context=Places
Type=Threshold

[32x32/mimetypes]
Size=32
Context=MimeTypes
Type=Threshold

[48x48/actions]
Size=48
Context=Actions
Type=Threshold

[48x48/apps]
Size=48
Context=Applications
Type=Threshold

[48x48/devices]
Size=48
Context=Devices
Type=Threshold

[48x48/places]
Size=48
Context=Places
Type=Threshold

[48x48/mimetypes]
Size=48
Context=MimeTypes
Type=Threshold

[64x64/actions]
Size=64
Context=Actions
Type=Threshold

[64x64/apps]
Size=64
Context=Applications
Type=Threshold

[64x64/devices]
Size=64
Context=Devices
Type=Threshold

[64x64/places]
Size=64
Context=Places
Type=Threshold

[64x64/mimetypes]
Size=64
Context=MimeTypes
Type=Threshold

[128x128/actions]
Size=128
Context=Actions
Type=Threshold

[128x128/apps]
Size=128
Context=Applications
Type=Threshold

[128x128/devices]
Size=128
Context=Devices
Type=Threshold

[128x128/places]
Size=128
Context=Places
Type=Threshold

[128x128/mimetypes]
Size=128
Context=MimeTypes
Type=Threshold

[scalable/actions]
Size=48
Type=Scalable
MinSize=1
MaxSize=256
Context=Actions

[scalable/apps]
Size=48
Type=Scalable
MinSize=1
MaxSize=256
Context=Applications

[scalable/categories]
Size=48
Type=Scalable
MinSize=1
MaxSize=256
Context=FileSystems

[scalable/devices]
Size=48
Type=Scalable
MinSize=1
MaxSize=256
Context=Devices

[scalable/emblems]
Size=48
Type=Scalable
MinSize=1
MaxSize=256
Context=Emblems

[scalable/mimetypes]
Size=48
Type=Scalable
MinSize=1
MaxSize=256
Context=MimeTypes

[scalable/places]
Size=48
Type=Scalable
MinSize=1
MaxSize=256
Context=FileSystems
EOF

echo "-----------------------------"
echo "Creating Oxygen_$date.tar.gz"

#change dir to basedir -1 and create a tar for gzipping
cd ../
tar -cf oxygen_$date.tar oxygen
gzip oxygen_$date.tar
# rm -rf oxygen
echo "---------------------------"
echo "Done"

exit 0
