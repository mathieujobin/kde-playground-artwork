#! /bin/bash
#
# Create a debug set of icons for OOo
#
# Jan Holesovsky <kendy@openoffice.org>, 2004

# Initialization
BASE_DIR=`dirname $0 | sed 's#bin$##'`
[ "$BASE_DIR" = "" ] && BASE_DIR="."

if [ "$#" -lt 2 ]
then
    echo "Usage: `basename $0` images.zip kde_dir"
    echo "  images.zip - OpenOffice.org images archive, like <OOo>/share/config/images.zip"
    echo "  kde_dir    - Path to the KDE cvs (to use the images from there)"
    exit 1
fi

IMAGELISTS="imagelists/images-from-kde.txt \
	    imagelists/images-crystalsvg.txt"
#IMAGELISTS="imagelists/images-from-gnome-ooo.txt \
#	    imagelists/images-from-kde.txt \
#	    imagelists/images-crystalsvg.txt"
IMAGESZIP="$1"
KDEDIR="$2"
SVGDIR="$BASE_DIR/crystalsvg"

BUILD_DIR="$BASE_DIR/build"

OUTPUT="crystal"
OUTDIR="$BUILD_DIR/$OUTPUT"
OUTNAME="$BUILD_DIR/ooo_crystal_images-6.tar.bz2"

#GNOMEDIR="$BUILD_DIR/ooo-icons-OOO_1_1-10-png"
#GNOMEZIP="$BASE_DIR/src/ooo-icons-OOO_1_1-10-png.tar.bz2"

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
#[ "$GNOMEDIR" != "/" ] && rm -rf "$GNOMEDIR"
[ "$REFERENCEDIR" != "/" ] && rm -rf "$REFERENCEDIR"
rm "$OUTNAME"

mkdir -p "$OUTDIR"
mkdir -p "$REFERENCEDIR"

# OOo icons there
unzip "$IMAGESZIP" -d "$REFERENCEDIR"
#tar xfj "$GNOMEZIP" -C "$BUILD_DIR"

# KDE icons to $OUTDIR
echo "Copying new images" 1>&2
cat $IMAGELISTS | while read NAME SIZE DIR FILES THE_REST
do
    rm -f "$TMPFILE"
    case "$NAME" in
	\#*) ;;
	"") ;;
	*)  # Ignored images
	    if [ "$SIZE" = "-" ] ; then
		true # nothing to do
	    # Temporarily ignored images
	    elif [ "$SIZE" = "svg" ] ; then
		echo "WARNING: '$NAME' temporarily ignored."
	    # SVG images
	    elif [ "$SIZE" = "SVG" -a -f "$SVGDIR/$DIR/$NAME.svg" ] ; then
		if [ -f "$REFERENCEDIR/$FILES/lc_$NAME.png" ] ; then
		    inkscape -z "$SVGDIR/$DIR/$NAME.svg" -w 22 -h 22 -e "$TMPFILE" > /dev/null 2>&1
		    correct_and_copy "$TMPFILE" "$FILES/lc_$NAME.png"
		    rm -f "$TMPFILE"
		    inkscape -z "$SVGDIR/$DIR/$NAME.svg" -w 16 -h 16 -e "$TMPFILE" > /dev/null 2>&1
		    copy_image "$TMPFILE" "$FILES/sc_$NAME.png"
		elif [ -f "$REFERENCEDIR/$FILES/$NAME.png" ] ; then
		    inkscape -z "$SVGDIR/$DIR/$NAME.svg" -w 22 -h 22 -e "$TMPFILE" > /dev/null 2>&1
		    correct_and_copy "$TMPFILE" "$FILES/$NAME.png"
		    rm -f "$TMPFILE"
		    if [ -f "$REFERENCEDIR/$FILES/${NAME/lc/sc}.png" ] ; then
			inkscape -z "$SVGDIR/$DIR/$NAME.svg" -w 16 -h 16 -e "$TMPFILE" > /dev/null 2>&1
			copy_image "$TMPFILE" "$FILES/${NAME/lc/sc}.png"
		    fi
		else
		    echo "WARNING: '$FILES/lc_$NAME.png' did not exist in images.zip" 1>&2
		fi
#	    # Gnome images
#	    elif [ "$SIZE" = "GNOME" -a -f "$GNOMEDIR/$DIR/$NAME.png" ] ; then
#		copy_image "$GNOMEDIR/$DIR/$NAME.png" "$FILES"
	    # 22x22
	    elif [ "$SIZE" = "22x22" -a -f "$SVGDIR/$DIR/$NAME.svg" ] ; then
		inkscape -z "$SVGDIR/$DIR/$NAME.svg" -w 22 -h 22 -e "$TMPFILE" > /dev/null 2>&1
		correct_and_copy "$TMPFILE" "$FILES"
	    elif [ "$SIZE" = "22x22" -a -f "$KDEDIR/$DIR/cr22-$NAME.png" ] ; then
		correct_and_copy "$KDEDIR/$DIR/cr22-$NAME.png" "$FILES"
	    elif [ "$SIZE" = "22x22" -a -f "$KDEDIR/$DIR/cr32-$NAME.png" ] ; then
		convert "$KDEDIR/$DIR/cr32-$NAME.png" -geometry "22x22>" "$TMPFILE"
		correct_and_copy "$TMPFILE" "$FILES"
	    # 16x16
	    elif [ "$SIZE" = "16x16" -a -f "$SVGDIR/$DIR/$NAME.svg" ] ; then
		inkscape -z "$SVGDIR/$DIR/$NAME.svg" -w 16 -h 16 -e "$TMPFILE" > /dev/null 2>&1
		copy_image "$TMPFILE" "$FILES"
	    elif [ "$SIZE" = "16x16" -a -f "$KDEDIR/$DIR/cr16-$NAME.png" ] ; then
		copy_image "$KDEDIR/$DIR/cr16-$NAME.png" "$FILES"
	    elif [ "$SIZE" = "16x16" -a -f "$KDEDIR/$DIR/cr22-$NAME.png" ] ; then
		convert "$KDEDIR/$DIR/cr22-$NAME.png" -geometry "16x16>" "$TMPFILE"
		copy_image "$TMPFILE" "$FILES"
	    elif [ "$SIZE" = "16x16" -a -f "$KDEDIR/$DIR/cr32-$NAME.png" ] ; then
		convert "$KDEDIR/$DIR/cr32-$NAME.png" -geometry "16x16>" "$TMPFILE"
		copy_image "$TMPFILE" "$FILES"
	    # 32x32
	    elif [ "$SIZE" = "32x32" -a -f "$KDEDIR/$DIR/cr32-$NAME.png" ] ; then
		copy_image "$KDEDIR/$DIR/cr32-$NAME.png" "$FILES"
	    # 26x26
	    # FIXME: 26x26 bitmaps should not be needed
	    elif [ "$SIZE" = "26x26" -a -f "$KDEDIR/$DIR/cr26-$NAME.png" ] ; then
		copy_image "$KDEDIR/$DIR/cr26-$NAME.png" "$FILES"
	    # the rest...
	    elif [ -f "$KDEDIR/$DIR/$NAME.png" ] ; then
		copy_image "$KDEDIR/$DIR/$NAME.png" "$FILES"
	    else
		echo "ERROR: '$NAME' not found (in '$DIR')" 1>&2
	    fi
	    ;;
    esac
done

# Correct size and color of the rest
#echo "Correcting size of the rest" 1>&2
#find "$OUTDIR" -name "*.png" | while read FILE
#do
#    SIZE=`identify -format '%wx%h' "$FILE"`
#    case "$SIZE" in
#	2[56]x2[56])
##	    convert "$FILE" -transparent "#ff00ff" XPM:"$TMPXPM"
##
##	    sed -e 's/c black/c #000000/' \
##		-e 's/c #800000/c #991f1f/' \
##		-e 's/c #008000/c #1f9928/' \
##		-e 's/c #808000/c #99661f/' \
##		-e 's/c navy/c #1f5299/' \
##		-e 's/c #800080/c #831f99/' \
##		-e 's/c #008080/c #1f9985/' \
##		-e 's/c #C0C0C0/c #e0e0e0/' \
##		-e 's/c #808080/c #999999/' \
##		-e 's/c red/c #e6a1a1/' \
##		-e 's/c green/c #a1e6a7/' \
##		-e 's/c yellow/c #e6e6a1/' \
##		-e 's/c blue/c #a1bee6/' \
##		-e 's/c magenta/c #d9a1e6/' \
##		-e 's/c cyan/c #a1e6da/' \
##		-e 's/c gray100/c #ffffff/' < "$TMPXPM" > "$TMPXPM2"
##
##	    convert "$TMPXPM2" -depth 32 -geometry "22x22>" "$TMPFILE"
#	    convert "$FILE" -geometry "22x22>" "$TMPFILE"
#	    correct_size "$TMPFILE" "$FILE"
#	    ;;
#    esac
#done

# Cleanup
rm -f $TMPFILE $TMPFILE_CORR $TMPXPM $TMPXPM2

# Tarbz it
echo "Creating tarball" 1>&2
tar cfj "$OUTNAME" -C "$BUILD_DIR" "$OUTPUT"

echo
echo "The result is stored in $OUTNAME."
