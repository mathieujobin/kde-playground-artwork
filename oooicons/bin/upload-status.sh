#! /bin/bash
#
# Upload the status to the server.
#
# Jan Holesovsky <kendy@openoffice.org>, 2004

if [ "$#" -lt 1 ]
then
    echo "Usage: `basename $0` where"
    echo "  where - login@server:path"
    exit 1
fi

# Initialization
BASE_DIR=`dirname $0 | sed 's#bin$##'`
[ "$BASE_DIR" = "" ] && BASE_DIR="."

BUILD_DIR="$BASE_DIR/build"

[ -d "$BUILD_DIR/crystal" ] || { echo "Cannot find '$BUILD_DIR/crystal', aborting." ; exit 1 ; }
[ -d "$BUILD_DIR/mono" ] || { echo "Cannot find '$BUILD_DIR/mono', aborting." ; exit 1 ; }
[ -d "$BUILD_DIR/reference" ] || { echo "Cannot find '$BUILD_DIR/reference', aborting." ; exit 1 ; }
[ -f "$BUILD_DIR/index.html" ] || { echo "Cannot find '$BUILD_DIR/index.html', aborting." ; exit 1 ; }

chmod -R go+r "$BUILD_DIR/crystal" "$BUILD_DIR/mono" "$BUILD_DIR/reference" "$BUILD_DIR/index.html"

WHERE="$1"

rsync -avz "$BUILD_DIR/crystal/" "$WHERE/crystal/"
rsync -avz "$BUILD_DIR/mono/" "$WHERE/mono/"
rsync -avz "$BUILD_DIR/reference/" "$WHERE/reference/"
rsync -avz "$BUILD_DIR/index.html" "$WHERE/index.html"
