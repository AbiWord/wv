#!/bin/sh

if test "x$SHELL" = "x"; then
  SHELL=/bin/sh
  export SHELL
fi

current_path=`echo $PATH | sed 's/:/ /g'`
for i in $current_path; do
  if test -d $i; then
    if test "$LIBTOOLIZE" = ""; then
      if test -x $i/libtoolize; then
        LIBTOOLIZE=$i/libtoolize
        export LIBTOOLIZE
        echo "Found: $LIBTOOLIZE"
      fi
    fi
    if test "$AUTOCONF" = ""; then
      if test -x $i/autoconf; then
        AUTOCONF=$i/autoconf
        export AUTOCONF
        echo "Found: $AUTOCONF"
      fi
    fi
    if test "$AUTOHEADER" = ""; then
      if test -x $i/autoheader; then
        AUTOHEADER=$i/autoheader
        export AUTOHEADER
        echo "Found: $AUTOHEADER"
      fi
    fi
  else
    echo "warning: non-directory or unexpected characters in PATH"
    echo "- found: '$i'"
  fi
done
if test "$LIBTOOLIZE" = ""; then
  echo "Can't find libtoolize"
  exit 1
fi
if test "$AUTOCONF" = ""; then
  echo "Can't find autoconf"
  exit 1
fi
if test "$AUTOHEADER" = ""; then
  echo "Can't find autoheader"
  exit 1
fi

accheck=`$SHELL $AUTOCONF --version | grep 2.13`
if test "x$accheck" != "x"; then
    echo "error: you appear to be using autoconf 2.13"
    echo "       the automake build system requires autoconf >= 2.50"
    exit 1
fi

aclocal --version > /dev/null 2> /dev/null || {
    echo "error: aclocal not found"
    exit 1
}
automake --version > /dev/null 2> /dev/null || {
    echo "error: automake not found"
    exit 1
}

amcheck=`automake --version | grep 'automake (GNU automake) 1.5'`
if test "x$amcheck" = "xautomake (GNU automake) 1.5"; then
    echo "warning: you appear to be using automake 1.5"
    echo "         this version has a bug - GNUmakefile.am dependencies are not generated"
fi

$SHELL $LIBTOOLIZE --force --copy || {
    echo "error: libtoolize failed"
    exit 1
}
aclocal || {
    echo "error: aclocal failed"
    exit 1
}
$SHELL $AUTOHEADER || {
    echo "error: autoheader failed"
    exit 1
}
automake -a --foreign || {
    echo "warning: automake failed"
}
$SHELL $AUTOCONF || {
    echo "error: autoconf failed"
    exit 1
}
