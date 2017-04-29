#!/bin/sh
LANGS="cs de el es fr pt_BR ru sv"

FIGUREBASENAMES=""
CFIGURES=""
for n in `ls C/figures/*.png 2>/dev/null` ; do
	CFIGURES="$CFIGURES $n"
	FIGUREBASENAMES="$FIGUREBASENAMES `basename $n`"
done

EXTRAFILES="$CFIGURES"

for lang in $LANGS ; do 
	LANGBASENAMES=""
	for n in `ls $lang/figures/*.png 2>/dev/null` ; do
		LANGBASENAMES="$LANGBASENAMES `basename $n`"
	done
	LANGBASENAMES=`echo $LANGBASENAMES $FIGUREBASENAMES | sed 's/ /\n/g' | sort | uniq`
	LANGBASENAMES=`echo $LANGBASENAMES`

	THEFIGURES=""

	echo
	echo $lang

	for b in $LANGBASENAMES ; do
		if test -e $lang/figures/$b ; then
			THEFIGURES="$THEFIGURES $lang/figures/$b"
			EXTRAFILES="$EXTRAFILES $lang/figures/$b"
		else
			THEFIGURES="$THEFIGURES C/figures/$b"
		fi
	done

	echo "$THEFIGURES"
done

echo
echo EXTRA: $EXTRAFILES

cat > foo.out <<EOF
###################################################################
# THIS FILE IS AUTOGENERATED DO NOT EDIT.  EDIT make-makefile-am.sh
###################################################################

# Text version of the manual
manualdir = \$(datadir)/genius
manual_DATA = genius.txt

EXTRA_DIST = $EXTRAFILES
EOF
