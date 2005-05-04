#! /bin/bash
#
# Create a debug set of icons for OOo
#
# Jan Holesovsky <kendy@openoffice.org>, 2004

# Initialization
BASE_DIR=`dirname $0 | sed 's#bin$##'`
[ "$BASE_DIR" = "" ] && BASE_DIR="."

if [ "$#" -lt 1 ]
then
    echo "Usage: `basename $0` images.zip"
    echo "  images.zip - OpenOffice.org images archive, like <OOo>/share/config/images.zip"
    exit 1
fi

IMAGESZIP="$1"
SVGDIR="$BASE_DIR/mono"

BUILD_DIR="$BASE_DIR/build"

OUTPUT="mono"
OUTDIR="$BUILD_DIR/$OUTPUT"
OUTNAME="$BUILD_DIR/ooo_mono_images-1.tar.bz2"

REFERENCEDIR="$BUILD_DIR/reference"

TMPPREFIX="/tmp/create-ooo-images$$"
TMPFILE="${TMPPREFIX}.png"
TMPFILE_CORR="${TMPPREFIX}_corr.png"
TMPXPM="${TMPPREFIX}.xpm"
TMPXPM2="${TMPPREFIX}2.xpm"

# Functions
# copy_image: copies $1 to $2, where $2 are more files delimited by ":"
function copy_image()
{
    if [ -e "$1" ] ; then
	chmod a-x "$1" 2> /dev/null
	echo "$2" | tr ':' '\n' | while read FILE
        do
	    for F in `cd $REFERENCEDIR && echo $FILE` ; do
		if [ -f "$REFERENCEDIR/$F" ] ; then
		    D=`dirname "$OUTDIR/$F"`
		    [ -d "$D" ] || mkdir -p "$D"
		    cp "$1" "$OUTDIR/$F"
		fi
	    done
	done
    fi
}
# correct_size: converts 22x22 icon to 24x24
function correct_size()
{
    convert NULL: -depth 32 -geometry "24x24" \
	    -background "#ffffff00" -transparent "#ffffff00" \
	    -region "22x22+1+1" \
	    -draw "image Over 0,0 0,0 ${1#./}" "$2"
}
# correct_and_copy: do correct_size and copy_image in one step
function correct_and_copy
{
    correct_size "$1" "$TMPFILE_CORR"
    copy_image "$TMPFILE_CORR" "$2"
}

# Clean
echo "Cleaning & unpacking" 1>&2
[ "$OUTDIR" != "/" ] && rm -rf "$OUTDIR"
[ "$REFERENCEDIR" != "/" ] && rm -rf "$REFERENCEDIR"
rm "$OUTNAME"

mkdir -p "$OUTDIR"
mkdir -p "$REFERENCEDIR"

# OOo icons there
unzip "$IMAGESZIP" -d "$REFERENCEDIR"

# KDE icons to $OUTDIR
echo "Copying new images" 1>&2
( cd "$REFERENCEDIR" ; find . -name "lc[0-9_]*.png" ) | while read NAME
do
    DIR=${NAME%/*}
    [ "$DIR" = "$NAME" ] && DIR=""
    NAME=${NAME##*/}
    NAME=${NAME#lc_}
    NAME=${NAME%.png}

    if [ -f "$SVGDIR/$NAME.svg" ] ; then
	if [ -f "$REFERENCEDIR/$DIR/lc_$NAME.png" ] ; then
	    rm -f "$TMPFILE"
	    inkscape -z "$SVGDIR/$NAME.svg" -w 22 -h 22 -e "$TMPFILE" > /dev/null 2>&1
	    correct_and_copy "$TMPFILE" "$DIR/lc_$NAME.png"
	    rm -f "$TMPFILE"
	    inkscape -z "$SVGDIR/$NAME.svg" -w 16 -h 16 -e "$TMPFILE" > /dev/null 2>&1
	    copy_image "$TMPFILE" "$DIR/sc_$NAME.png"
	elif [ -f "$REFERENCEDIR/$DIR/$NAME.png" ] ; then
	    rm -f "$TMPFILE"
	    inkscape -z "$SVGDIR/$NAME.svg" -w 22 -h 22 -e "$TMPFILE" > /dev/null 2>&1
	    correct_and_copy "$TMPFILE" "$DIR/$NAME.png"
	    if [ -f "$REFERENCEDIR/$DIR/${NAME/lc/sc}.png" ] ; then
		rm -f "$TMPFILE"
		inkscape -z "$SVGDIR/$NAME.svg" -w 16 -h 16 -e "$TMPFILE" > /dev/null 2>&1
		copy_image "$TMPFILE" "$DIR/${NAME/lc/sc}.png"
	    fi
	else
	    echo "WARNING: '$DIR/lc_$NAME.png' did not exist in images.zip" 1>&2
	fi
    fi
done

# Cleanup
rm -f $TMPFILE $TMPFILE_CORR $TMPXPM $TMPXPM2

# Tarbz it
echo "Creating tarball" 1>&2
tar cfj "$OUTNAME" -C "$BUILD_DIR" "$OUTPUT"

echo
echo "The result is stored in $OUTNAME."
