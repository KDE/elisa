#!/usr/bin/env bash

$XGETTEXT $(find . -name \*.qml -o -name \*.cpp) -o $podir/kcm_elisa_local_file.pot
