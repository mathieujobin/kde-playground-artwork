#!/bin/bash

for apps in $(ls apps/*.svg); do inkscape --export-png="../oxygen/128x128/"$( echo $apps | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=128 --export-height=128 $apps;done
for actions in $(ls actions/*.svg); do inkscape --export-png="../oxygen/128x128/"$( echo $actions | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=128 --export-height=128 $actions;done
for filesystems in $(ls filesystems/*.svg); do inkscape --export-png="../oxygen/128x128/"$( echo $filesystems | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=128 --export-height=128 $filesystems;done
for devices in $(ls devices/*.svg); do inkscape --export-png="../oxygen/128x128/"$( echo $devices | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=128 --export-height=128 $devices;done
for mimetypes in $(ls mimetypes/*.svg); do inkscape --export-png="../oxygen/128x128/"$( echo $mimetypes | cut -d . -f -1 ).png --export-dpi=72 --export-background-opacity=0 --export-width=128 --export-height=128 $mimetypes;done

cd ../oxygen/128x128

echo -----------------------------
echo Scaling PNGs, be patient DUDE

for i in $(ls */*.png); do convert -resize 64x64 $i "../64x64/"$i && convert -resize 48x48 $i "../48x48/"$i && convert -resize 32x32 -sharpen 2 $i "../32x32/"$i && convert -resize 22x22 -sharpen 2 $i "../22x22/"$i && convert -resize 16x16 -sharpen 2 $i "../16x16/"$i;done

echo ---------------------------
echo Creating Oxygen_DATE.tar.gz
tar czf $HOME/oxygen_"$(date  '+%F-%H-%M')".tar.gz ../../
echo ---------------------------
echo Done
