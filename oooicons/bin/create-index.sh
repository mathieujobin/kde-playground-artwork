#! /bin/bash
#
# Create index of the finished images
#
# Jan Holesovsky <kendy@openoffice.org>, 2005

# Initialization
BASE_DIR=`dirname $0 | sed 's#bin$##'`
[ "$BASE_DIR" = "" ] && BASE_DIR="."

BUILD_DIR="$BASE_DIR/build"

[ -d "$BUILD_DIR/crystal" ] || { echo "Cannot find '$BUILD_DIR/crystal', aborting." ; exit 1 ; }
[ -d "$BUILD_DIR/mono" ] || { echo "Cannot find '$BUILD_DIR/mono', aborting." ; exit 1 ; }
[ -d "$BUILD_DIR/reference" ] || { echo "Cannot find '$BUILD_DIR/reference', aborting." ; exit 1 ; }

cd "$BUILD_DIR"

(
    ALL=0
    CRYSTAL_NEEDED=0
    MONO_NEEDED=0
    echo -e "<html>\n<body>"
    echo "<h2>Status of the OOo icons from `date --iso-8601`</h2>"
    echo "<p>O = original, C = Crystal, M = Mono</p>"
    echo "<table border=\"1\">"
    echo "<tr><th colspan=\"2\">TODO</th><th></th><th>O</th><th>C</th><th>M</th><th></th><th>Name</tr>"
    for REF in `find reference -name "lc[0-9_]*.png"`
    do
	PTH=${REF%/*}
	[ "${PTH%/*}" = "reference/res/commandimagelist" ] && continue

	BASE=${REF##*/}
	BASE=${BASE#lc_}
	BASE=${BASE%.png}

	CRYSTAL=${REF/reference/crystal}
	CRYSTAL_X=
	CRYSTAL_IMG="<img src=\"$CRYSTAL\">"
	MONO=${REF/reference/mono}
	MONO_X=
	MONO_IMG="<img src=\"$MONO\">"
	[ -e "$CRYSTAL" ] || { CRYSTAL_X="C" ; CRYSTAL_IMG=" " ; CRYSTAL_NEEDED=$(( $CRYSTAL_NEEDED + 1 )) ; }
	[ -e "$MONO" ] || { MONO_X="M" ; MONO_IMG=" " ; MONO_NEEDED=$(( $MONO_NEEDED + 1 )) ; }

        echo "<tr><td align=center>$CRYSTAL_X</td><td align=center>$MONO_X</td><td></td><td><img src=\"$REF\"></td><td>$CRYSTAL_IMG</td><td>$MONO_IMG</td><td></td><td>$BASE</td>"

	ALL=$(( $ALL + 1 ))
    done
    echo "</table>"
    echo "<table>"
    echo "<tr><td>Number of all icons:</td><td>$ALL</td></tr>"
    echo "<tr><td>Number of Crystal icons to do:</td><td>$CRYSTAL_NEEDED</td></tr>"
    echo "<tr><td>Number of Mono icons to do:</td><td>$MONO_NEEDED</td></tr>"
    echo "</table>"
    echo -e "</table>\n</body>\n</html>"
) > index.html
