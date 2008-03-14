#!/bin/sh

srcdir=`dirname "$0"`
test -z "$srcdir" && srcdir=.
origdir=`pwd`

( cd "$srcdir" && ${AUTORECONF:-autoreconf} -iv ) || exit $?

if [ -z "$NOCONFIGURE" ]; then
	"$srcdir"/configure "$@"
fi
