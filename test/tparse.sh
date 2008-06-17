#!/bin/sh

: ${builddir:=.}

exec "$builddir/tparse" "$builddir/init.d/foo"
