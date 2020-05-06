#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2016 Luigi Toscano <luigi.toscano@tiscali.it>
#
# SPDX-License-Identifier: LGPL-3.0-or-later

$XGETTEXT $(find . ! -path './localFileConfiguration/*' -a \( -name \*.qml -o -name \*.cpp -o -name \*.h \)) -o $podir/elisa.pot
