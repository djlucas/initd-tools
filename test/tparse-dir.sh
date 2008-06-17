#!/bin/sh

: ${builddir:=.}

exec "$builddir/tparse-dir" "$builddir/init.d"
