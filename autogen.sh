#!/bin/bash
### autogen.sh with sensible comments ###############################

# Use this script to bootstrap your build AFTER checking it out from
# source control. You should not have to use it for anything else.

### Checking GNU Autotools ############################################
AC_VER=2.69
AM_VER=1.15
LT_VER=2.4.6

if [ `uname` == 'Darwin' ]; then
  if ! which gsort >/dev/null 2>&1; then
    brew install coreutils || exit 1
  fi
  export SORT=gsort
else
  export SORT=sort
fi

verlte() {
  [  "$1" = `echo -e "$1\n$2" | $SORT -V | head -n 1` ]
}

verlt() {
  [ "$1" = "$2" ] && return 1 || verlte $1 $2
}

function check_tool()
{
  tool=$1
  ver=$2

  if ! which $1 >/dev/null 2>&1; then
    echo "$1 not installed. version $ver is required"
    exit 1
  fi
  v=`$1 --version | head -n 1 | awk '{print $NF}'`
  if verlt $v $ver; then
    echo "Error: $1 version $ver is required. found $v"
    exit 1
  elif [ "$v" != "$ver" ]; then
    echo "Warning: $1 version $ver is required. found $v"
  fi
}
check_tool autoconf $AC_VER
check_tool automake $AM_VER
check_tool glibtool $LT_VER

### GNU Autoconf Archive ############################################
get_aclocal_dir_list () {
    aclocal_dir=`aclocal --print-ac-dir`
    echo $aclocal_dir
    aclocal_dirlist="$aclocal_dir/dirlist"
    if test -e $aclocal_dirlist; then
        cat $aclocal_dirlist | while read dir; do
            if test -d "$dir"; then
                echo $dir
            fi
        done
    fi
}
aclocal_dir_list=`get_aclocal_dir_list`

if test -z "`find $aclocal_dir_list -name ax_add_am_macro_static.m4`"; then
  echo "GNU Autoconf Archive not installed. Downloading it..."
  if [ ! -e .downloads/autoconf-archive-2015.09.25.tar.xz ]; then
    mkdir -p .downloads
    wget -P .downloads http://ftpmirror.gnu.org/autoconf-archive/autoconf-archive-2015.09.25.tar.xz || exit 1
  fi
fi

mkdir -p m4
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
