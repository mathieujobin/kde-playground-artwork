#!/bin/bash

sizes="128x128 64x64 48x48 32x32 22x22 16x16"
folders="actions apps devices filesystems mimetypes"
smallexport="no"
smallfolders="apps/small filesystems/small"
smallsize="22"
date=`date '+%F-%H-%M'`

#create every size dir and within the size dir create all the icon folders
for size in $sizes; do
  prefix="../oxygen/$size"
  for folder in $folders; do
    mkdir -p $prefix/$folder
    echo "$prefix/$folder created"
  done
done
if [ "$smallexport" == "yes" ]; then
    prefix="../oxygen/${smallsize}x${smallsize}"
    for folder in $smallfolders; do
      mkdir -p $prefix/$folder
      echo "$prefix/$folder created"
    done
fi

echo "-----------------------------"
echo "Exporting to 128 and smaller (if available)"

#resize the 128x128 png's to all needed sizes defined in sizes
for folder in $folders; do
  for icon in $(ls $folder/*.svg); do
    inkscape --without-gui --export-png="../oxygen/128x128/"$( echo $icon | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=128 --export-height=128 $icon > /dev/null
  done
done

#export small versions if available
if [ "$smallexport" == "yes" ]; then
  for folder in $smallfolders; do
    for icon in $(ls $folder/*.svg); do
      inkscape --without-gui --export-png="../oxygen/${smallsize}x${smallsize}/"$( echo $icon | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=${smallsize} --export-height=${smallsize} $icon > /dev/null
    done
  done
fi

cd ../oxygen/128x128

echo "-----------------------------"
echo "Scaling PNGs, be patient DUDE"

for i in $(ls */*.png); do 
  convert -filter Sinc -resize 64x64 $i "../64x64/"$i 
  convert -filter Sinc -resize 48x48 $i "../48x48/"$i
  convert -filter Sinc -resize 32x32 -sharpen 1 $i "../32x32/"$i 
  convert -filter Sinc -resize 22x22 -sharpen 1 -contrast $i "../22x22/"$i
  convert -filter Sinc -resize 16x16 -sharpen 1 -contrast $i "../16x16/"$i
done

#do a resize for small exported icons
if [ "$smallexport" == "yes" ]; then
  cd ../${smallsize}x${smallsize}

  #first fix the directory which is currently size/iconfolder/smallsize
  for folder in $smallfolders; do
    mv $folder/* $folder/..
    rm -r $folder
  done

  #resize to 16x16
  for i in $(ls */*.png); do 
      convert -filter Sinc -resize 16x16 -sharpen 1 -contrast $i "../16x16/"$i
  done
fi

echo "-----------------------------"
echo "Creating index.theme"

#create theme file
cd ../
cat > index.theme <<EOF
[Icon Theme]
Name=Oxygen
Comment=Oxygen Team - 2006
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
Directories=16x16/actions,16x16/apps,16x16/devices,16x16/filesystems,16x16/mimetypes,22x22/actions,22x22/apps,22x22/filesystems,22x22/mimetypes,32x32/actions,32x32/apps,32x32/devices,32x32/filesystems,32x32/mimetypes,48x48/actions,48x48/apps,48x48/devices,48x48/filesystems,48x48/mimetypes,64x64/actions,64x64/apps,64x64/devices,64x64/filesystems,64x64/mimetypes,128x128/actions,128x128/apps,128x128/devices,128x128/filesystems,128x128/mimetypes
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
[16x16/filesystems]
Size=16
Context=FileSystems
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
[22x22/filesystems]
Size=22
Context=FileSystems
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
[32x32/filesystems]
Size=32
Context=FileSystems
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
[48x48/filesystems]
Size=48
Context=FileSystems
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
[64x64/filesystems]
Size=64
Context=FileSystems
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
[128x128/filesystems]
Size=128
Context=FileSystems
Type=Threshold
[128x128/mimetypes]
Size=128
Context=MimeTypes
Type=Threshold
EOF

echo "-----------------------------"
echo "Creating Oxygen_$date.tar.gz"

#change dir to basedir -1 and create a tar for gzipping
cd ../
tar -cf oxygen_$date.tar oxygen
gzip oxygen_$date.tar
rm -rf oxygen
echo "---------------------------"
echo "Done"

exit 0
