# - Try to find the libvlc library
# Once done this will define
#
#  LIBVLC_FOUND - system has the libvlc library
#  LIBVLC_INCLUDE_DIRS - the libvlc includes
#  LIBVLC_LIBRARIES - The libraries needed to use libvlc

# Copyright (c) 2006, Laurent Montel, <montel@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

#reset vars
set(LIBVLC_LIBRARIES LIBVLC_LIBRARIES-NOTFOUND)
set(LIBVLC_INCLUDE_DIRS LIBVLC_INCLUDE_DIRS-NOTFOUND)

find_package(PkgConfig)
pkg_check_modules(PC_LIBVLC libvlc>=3.0)

find_path(LIBVLC_INCLUDE_DIRS libvlc.h
    HINTS
    ${PC_LIBVLC_INCLUDEDIR}
    ${PC_LIBVLC_INCLUDE_DIRS}
    PATH_SUFFIXES
    vlc
)

find_library(LIBVLC_LIBRARIES NAMES vlc
    HINTS
    ${PC_LIBVLC_LIBDIR}
    ${PC_LIBVLC_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBVLC DEFAULT_MSG
    LIBVLC_INCLUDE_DIRS LIBVLC_LIBRARIES)
