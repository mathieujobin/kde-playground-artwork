#! /bin/bash
#
# Check imagelists for completeness
#
# Jan Holesovsky <kendy@openoffice.org>, 2004

# Initialization
BASE_DIR=`dirname $0 | sed 's#bin$##'`
[ "$BASE_DIR" = "" ] && BASE_DIR="."

IMAGELIST="$BASE_DIR/imagelists/images-crystalsvg.txt"
SVGDIR="$BASE_DIR/crystalsvg"

TMPPREFIX="/tmp/check-imagelists$$"
TMPSVGLIST="${TMPPREFIX}_svgs.txt"
TMPIMAGELIST="${TMPPREFIX}_imgs.txt"
TMPDIFF="${TMPPREFIX}.diff"

# List the images from imagelist
cat "$IMAGELIST" | while read NAME SIZE DIR FILES THE_REST
do
    [ "$DIR" = "" ] && DIR="."
    case "$NAME" in
	\#*) ;;
	"")  ;;
	*)   echo "$DIR/$NAME" ;;
    esac
done | sort | uniq > "$TMPIMAGELIST"

# List all svgs
( cd "$SVGDIR" ; find . -name "*.svg" ) | sed -e 's/\.svg//' | sort | uniq > "$TMPSVGLIST"

# Diff & show
diff --normal "$TMPIMAGELIST" "$TMPSVGLIST" > "$TMPDIFF"

if [ "$1" = "--create" ] ; then
    for I in `grep "^>" "$TMPDIFF" | sed -e 's#^> \./##'` ; do
	[ -e "$BASE_DIR/build/reference/res/commandimagelist/lc_$I.png" ] && echo -e "$I\t\t\tSVG\t.\tres/commandimagelist"
    done
else
    echo "Files missing in imagelists/images-crystalsvg.txt:"
    grep "^>" "$TMPDIFF" | sed -e 's#^> \./#  crystalsvg/#' -e 's/$/.svg/'

    echo
    echo "Files present in imagelists/images-crystalsvg.txt, but missing in crystalsvg:"
    grep "^<" "$TMPDIFF" | sed -e 's#^< \./#  crystalsvg/#' -e 's/$/.svg/'
fi

# Cleanup
rm "$TMPSVGLIST" "$TMPIMAGELIST" "$TMPDIFF"
