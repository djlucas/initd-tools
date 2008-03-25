#!/bin/sh

: ${srcdir:=.}
: ${builddir:=.}

exec "$builddir/tparse-dir" "$srcdir/init.d"
