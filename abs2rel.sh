#!/bin/sh

# Determine the relative path between 2 directories. This is modeled
# on the perl File::Spec abs2rel function.

if test "x$1" = x || test "x$2" = x; then
	echo "error: two paths must be supplied to $0" >&2
	exit 1
fi

dir1=$1
dir2=$2

# all shell builtins except for sed; let user override
SED=${SED-sed}

# if either path is not absolute, prefix the current directory
case "$dir1" in
/*) ;;
*)  dir1=$PWD/$dir1 ;;
esac
case "$dir2" in
/*) ;;
*)  dir2=$PWD/$dir2 ;;
esac

# strip multiple / and trailing /
dir1=`echo $dir1 | $SED 's,//*,/,g;s,\(.\)/$,\1,'`
dir2=`echo $dir2 | $SED 's,//*,/,g;s,\(.\)/$,\1,'`

# special cases
if test "$dir1" = "$dir2"; then
	# same directory, no relative path needed
	exit 0
elif test "$dir1" = /; then
	# convert all components of $2 to ..
	dir2=`echo $dir2 | $SED 's,/[^/]*,../,g'`
	echo ${dir2%/}
	exit 0
elif test "$dir2" = /; then
	# strip leading / of $1 to convert to relative
	echo ${dir1#/}
	exit 0
fi

# find the common prefix
pre1=
pre2=
while :; do
	# find the leading dir component
	pre1=${dir1#/}
	pre1=/${pre1%%/*}
	pre2=${dir2#/}
	pre2=/${pre2%%/*}

	# if the leading components match, strip them from the suffixes
	if test "$pre1" = "$pre2"; then
		dir1=${dir1#$pre1}
		dir2=${dir2#$pre2}

		# if either suffix is empty, we're done
		if test "x$dir1" = x; then
			break
		elif test "x$dir2" = x; then
			# dir1 is a subdir of dir2; convert to relative
			dir1=${dir1#/}
			break
		fi
	else
		# we've found the common prefix
		break
	fi
done

# convert remaining components of $dir2 to ..
rem=
if test "x$dir2" != x; then
	rem=`echo $dir2 | $SED 's,/[^/]*,../,g'`
	rem=${rem%/}
fi

# print converted $dir2 + remainder of $dir1
echo $rem$dir1
