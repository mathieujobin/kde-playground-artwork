#!/bin/bash

ICONPATH="../../../../KDE/kdebase/runtime/pics/oxygen"

icon="16x16/status/dialog-warning.png"
cp ${ICONPATH}/${icon} Oxygen/browser/safebrowsing/warning16x16.png
cp ${ICONPATH}/${icon} Oxygen/mozapps/update/warning.gif

cp ${ICONPATH}/22x22/status/dialog-warning.png Oxygen/browser/safebrowsing/warning24x24.png

cp ${ICONPATH}/16x16/places/user-identity.png Oxygen/mozapps/profile/profileicon.png

icon="16x16/actions/rss.png"
cp ${ICONPATH}/${icon} Oxygen/browser/feeds/feedIcon16.png
cp ${ICONPATH}/${icon} Oxygen/browser/page-livemarks.png

cp ${ICONPATH}/32x32/actions/rss.png Oxygen/browser/feeds/feedIcon.png

cp ${ICONPATH}/32x32/apps/preferences-desktop-theme.png Oxygen/mozapps/extensions/themeGeneric.png
