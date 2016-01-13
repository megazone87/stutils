#!/bin/sh
### autogen.sh with sensible comments ###############################

# Use this script to bootstrap your build AFTER checking it out from
# source control. You should not have to use it for anything else.

mkdir -p m4
### GNU Autoconf Archive ############################################
echo "Getting GNU Autoconf Archive macros"
if [ ! -e .downloads/autoconf-archive-2015.09.25.tar.xz ]; then
  mkdir -p .downloads
  wget -P .downloads http://ftpmirror.gnu.org/autoconf-archive/autoconf-archive-2015.09.25.tar.xz || exit 1
fi

tar xf .downloads/autoconf-archive-2015.09.25.tar.xz -C m4/ --strip-components=2 autoconf-archive-2015.09.25/m4 || exit 1

### AUTOTOOLS #######################################################
# Runs autoconf, autoheader, aclocal, automake, autopoint, libtoolize
echo "Regenerating autotools files"
autoreconf --install --force --symlink || exit 1

### INTLTOOL ########################################################
# Run after autopoint or glib-gettextize
#echo "Setting up intltool"
#intltoolize --copy --force --automake || exit 1

echo "Now run ./configure, make, make check and make install."
