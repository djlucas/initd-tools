#!/bin/sh

: ${srcdir:=.}
: ${builddir:=.}

exec "$builddir/tparse" "$srcdir/init.d/foo"
