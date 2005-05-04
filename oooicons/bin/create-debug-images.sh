#! /bin/bash
#
# Create a debug set of icons for OOo
#
# Jan Holesovsky <kendy@openoffice.org>, 2004

if [ "$#" -lt 1 ]
then
    echo "Usage: `basename $0` images.zip"
    echo "  images.zip - OpenOffice.org images archive, like <OOo>/share/config/images.zip"
    exit 1
fi

# Initialization
BIN_DIR=`dirname $0`

BASE_DIR=`dirname $0 | sed 's#bin$##'`
[ "$BASE_DIR" = "" ] && BASE_DIR="."

OOO_IMAGES="$1"
OOO_DIR=$BASE_DIR/build/ooo

OUT_NAME=images_debug
BUILD_DIR=$BASE_DIR/build

OUT_DIR=$BUILD_DIR/$OUT_NAME
OUT_TXT=$OUT_DIR/mapping.txt
OUTPUT=$BUILD_DIR/$OUT_NAME.zip

TMP_PNG=/tmp/mkdebug_tmp.png

# Do not overwrite existing
[ "$OOO_DIR" != "/" ] && rm -rf "$OOO_DIR"
[ "$OUT_DIR" != "/" ] && rm -rf "$OOO_DIR"

# Prepare the dirs
mkdir -p "$OOO_DIR"
mkdir -p "$OUT_DIR"

unzip -d "$OOO_DIR" "$OOO_IMAGES"

# Create the debug icons
NUM=1
:> $OUT_TXT

( cd "$OOO_DIR" ; find . -name "*.png" ) | while read LOC
do
    D=`dirname $LOC`
    [ -d $OUT_DIR/$D ] || mkdir -p $OUT_DIR/$D

    SIZE=`identify $OOO_DIR/$LOC -format "%wx%h"`
    [ "$SIZE" = "" ] && continue
	
    echo "Creating $LOC (no. $NUM, $SIZE)..."

    echo -e "$NUM\t${LOC#./}" >> $OUT_TXT

    convert NULL: -depth 32 -geometry "30x16!" -draw "text 0,12 \"$NUM\"" $TMP_PNG
    convert $TMP_PNG -geometry "${SIZE}!" "$OUT_DIR/$LOC"

    NUM=$(( $NUM+1 ))
done

# Cleanup
rm "$TMP_PNG"

# Zip it
( WD=`pwd` ; cd "$OUT_DIR" ; zip -r "$WD/$OUTPUT" * )

echo
echo "The result is stored in $OUTPUT."
