#! /bin/bash
#
# Create color separations and call potrace on them
#
# Jan Holesovsky <kendy@openoffice.org>, 2004

if [ "$#" -lt 2 ]
then
    echo "Usage: `basename $0` bitmap svg [color]"
    echo "  bitmap  - input bitmap to trace"
    echo "  svg     - output file name"
    echo "  color   - the tranparent color"
    exit 1
fi

TMP_PREFIX="/tmp/color-potrace$$"
#TMP_PREFIX="color-potrace"

TMP_XPM="${TMP_PREFIX}.xpm"
TMP_XPM2="${TMP_PREFIX}2.xpm"
TMP_BMP="${TMP_PREFIX}.bmp"

OUT_SVG="$2"

SCALE="2"

# reduce the number of colors and create .xpm
TRANSPARENT=
[ "$3" != "" ] && TRANSPARENT="-transparent $3"
convert $1 $TRANSPARENT +dither -colors 16 $TMP_XPM

# read header
while read HEADER
do
    echo "$HEADER" | grep '^"[0-9]\+ [0-9]\+ [0-9]\+ [0-9]\+"' > /dev/null 2>&1 && break
done < $TMP_XPM

[ -z "$HEADER" ] && { echo "Cannot read xpm header. Exiting..." ; exit 1 ; }

set `echo $HEADER | sed 's/^"\(.*\)",$/\1/'`
WIDTH="$1"
HEIGHT="$2"
NUMCOLORS="$3"
NUMCHARS="$4"

WPT="$((($SCALE*$WIDTH*2835)/1000))"
HPT="$((($SCALE*$HEIGHT*2835)/1000))"

# create the .svg file
echo '<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 20010904//EN"
 "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">
<svg version="1.0" xmlns="http://www.w3.org/2000/svg"
 width="'"$WPT"'pt" height="'"$HPT"'pt" viewBox="0 0 '"$WPT"' '"$HPT"'"
 preserveAspectRatio="xMidYMid meet">
<metadata>
Created by potrace 1.4, written by Peter Selinger 2001-2004
Color separation by color-potrace.sh, written by Jan Holesovsky

This file is based on an OpenOffice.org icon, which is licensed
under LGPL and SISSL.
</metadata>' > "$OUT_SVG"

OIFS="$IFS"
IFS=\"

# separate the colors
grep '^".\{'$NUMCHARS'\} c ' $TMP_XPM | sed 's/^"\(.\{'$NUMCHARS'\}\) c \([^"]*\).*/\1"\2/' | \
while read SIGN VALUE
do
    # FIXME: The following line does not work for $NUMCHARS > 1
    NUMBER=`tr -c -d "$SIGN" < "$TMP_XPM" | wc -c`
    echo "$NUMBER\"$SIGN\"$VALUE"
done | sort -r | \
while read  NUMBER SIGN VALUE
do
    if [ "$VALUE" != "None" -a "$VALUE" != "none" ]
    then
	ESC='\'
	echo "$SIGN" | grep '^[0-9+%=<>]' > /dev/null 2>&1 && ESC=""
	    
	sed -e 's/^"'"${ESC}${SIGN}"' c .*/""'"${ESC}${SIGN}"' c black",/' \
	    -e 's/^"\([^"]\{'"$NUMCHARS"'\}\) c .*/"\1 c white",/' \
	    -e 's/^""/"/' \
	    < $TMP_XPM > $TMP_XPM2
	
	if [ "$WIDTH" -lt "32" -o "$HEIGHT" -lt "32" ]
	then
	    convert "$TMP_XPM2" -filter Point -geometry "200%" "$TMP_BMP"
	else
	    convert "$TMP_XPM2" "$TMP_BMP"
	fi

	potrace -W $(($SCALE*$WIDTH))mm -H $(($SCALE*$HEIGHT))mm --svg -o - "$TMP_BMP" | \
	    sed -e '/<g/,/<\/g>/!d' \
		-e 's/"#000000"/"'"$VALUE"'"/' >> "$OUT_SVG"
    fi
done
IFS="$OIFS"

echo "</svg>" >> "$OUT_SVG"

# cleanup
rm $TMP_XPM $TMP_XPM2 $TMP_BMP
