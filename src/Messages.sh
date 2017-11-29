#!/usr/bin/env bash

$XGETTEXT $(find . ! -path './localFileConfiguration/*' -a \( -name \*.qml -o -name \*.cpp -o -name \*.h \)) -o $podir/elisa.pot
