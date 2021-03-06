AC_INIT([genius], [1.0.24])
#AX_IS_RELEASE([git-directory])
AX_IS_RELEASE([always])
AC_CONFIG_SRCDIR([src/calc.c])
AM_INIT_AUTOMAKE([dist-xz])

AC_CONFIG_MACRO_DIRS([m4])

AM_CONFIG_HEADER(config.h)

dnl make sure we keep ACLOCAL_FLAGS around for maintainer builds to work
AC_SUBST(ACLOCAL_AMFLAGS, "$ACLOCAL_FLAGS")

AM_MAINTAINER_MODE

AC_PROG_INTLTOOL([0.21])

dnl ================= Requirements ================================================

GTK_REQUIRED=2.20.0
GLIB_REQUIRED=2.26.0
PANGO_REQUIRED=1.22.0
GTKSOURCEVIEW_REQUIRED=0.3.0
GTKSOURCEVIEW2_REQUIRED=2.0.2
GIO_REQUIRED=2.16.0

dnl ===============================================================================

AC_ARG_ENABLE(extra-gcc-optimization,
[  --disable-extra-gcc-optimization  Compile without extra GCC optimization (if using GCC)],
use_extraopt="$enableval",use_extraopt=yes)
AC_PROG_CC
if test "x$GCC" = "xyes" -a "x$use_extraopt" = "xyes" ; then
	echo "Turning on extra gcc flags -finline-functions -frename-registers"
	CFLAGS="-finline-functions -frename-registers $CFLAGS"
fi
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LEX
AM_PROG_LIBTOOL
AC_PROG_YACC

AM_BINRELOC

AX_COMPILER_FLAGS([WARN_CFLAGS],[WARN_LDFLAGS])

dnl ================= Translation Stuff ==============================================

GETTEXT_PACKAGE=genius
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",[gettext package name])
AC_SUBST(GETTEXT_PACKAGE)

ALL_LINGUAS="ar az bs ca cs da de dz el en_CA en_GB eo es eu fr ga gl hi hr hu id it ja ko lv mk ml nb ne nl oc pa pt_BR pt ro ru rw sl sq sr@latin sr sv zh_CN zh_TW"
AM_GLIB_GNU_GETTEXT

dnl ===============================================================================

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

# glib-genmarshal
AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

dnl Check for termcap
AC_CHECK_LIB(termcap, tgetent,[TERMCAP_LIB="-ltermcap"],
	     [AC_CHECK_LIB(ncurses, tgetent,
                           [TERMCAP_LIB="-lncurses"
                            AC_DEFINE(USE_NCURSES,[1], [Use ncurses library])
                            AC_CHECK_HEADER(ncurses/curses.h, [
					    AC_DEFINE(INC_NCURSES,[1],[ncurses include])])], [
			   AC_MSG_ERROR(Termcap library not found)])])
AC_SUBST(TERMCAP_LIB)

dnl Check for readline
AC_CHECK_LIB(readline,readline, [READLINE_LIB="-lreadline"],
	     AC_MSG_ERROR(Readline library not found),
	     $TERMCAP_LIB)
AC_SUBST(READLINE_LIB)

GMP_LIB=""
AC_CHECK_LIB(gmp, __gmpz_init_set,[GMP_LIB=-lgmp], [
	AC_CHECK_LIB(gmp2, __gmpz_init_set_d,[GMP_LIB=-lgmp2
		AC_DEFINE(HAVE_GMP2_INCLUDE_DIR,[1],[Have gmp2 include directory])],
		AC_MSG_ERROR(GMP Library not found))])
AC_SUBST(GMP_LIB)

AC_CHECK_LIB(mpfr, mpfr_j0, [],
	[AC_MSG_ERROR([New enough MPFR (2.3.0+) not found, see http://www.mpfr.org])],
	"$GMP_LIB")

# No schemas yet so ignore this section for now
#AC_PATH_PROG(GCONFTOOL, gconftool-2)
#dnl Specify the gconf configuration source,
#dnl default to xml::$(sysconfdir)/gconf/gconf.xml.defaults
#
#GCONF_CONFIG_SOURCE=
#
#AC_ARG_ENABLE(gconf-source, [  --enable-gconf-source=sourceaddress Where to install schema files.], GCONF_CONFIG_SOURCE=$enable_gconf_source,)
#if test "x$GCONF_CONFIG_SOURCE" = "x"; then
#        GCONF_CONFIG_SOURCE="xml::\${sysconfdir}/gconf/gconf.xml.defaults"
#fi
#
#AC_SUBST(GCONF_CONFIG_SOURCE)
#AC_SUBST(INSTALL_GCONF_CONFIG_SOURCE)

AC_ARG_ENABLE(gtksourceview,
[  --disable-gtksourceview Do not use GtkSourceView library even if detected],
use_gtksourceview="$enableval",use_gtksourceview=yes)

AC_ARG_ENABLE(gnome,
[  --disable-gnome         Compile without the GNOME frontend (no GUI, no plotting)],
use_gnome="$enableval",use_gnome=yes)

# Used for the 'genius' binary
PKG_CHECK_MODULES(GENIUS_NOGUI, glib-2.0 >= $GLIB_REQUIRED gmodule-2.0 >= $GLIB_REQUIRED)
AC_SUBST(GENIUS_NOGUI_CFLAGS)
AC_SUBST(GENIUS_NOGUI_LIBS)

# glib
PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

GENIUS_MODULES="gtk+-2.0 >= $GTK_REQUIRED gio-2.0 >= $GIO_REQUIRED"

if test "x$use_gnome" = "xyes" ; then
  echo " 
Testing for presence of GNOME (GTK and GIO is enough) and GtkSourceView:
Note:
      You can compile without GNOME by using the --disable-gnome option
"
  PKG_CHECK_MODULES(GENIUS, $GENIUS_MODULES)

  if test "x$use_gtksourceview" = "xyes" ; then
    PKG_CHECK_MODULES(GSV, gtksourceview-2.0 >= $GTKSOURCEVIEW2_REQUIRED, have_gtksourceview2=yes, have_gtksourceview2=no)

    if test "x$have_gtksourceview2" != "xyes"; then
      PKG_CHECK_MODULES(GSV, gtksourceview-1.0 >= $GTKSOURCEVIEW_REQUIRED, have_gtksourceview=yes, have_gtksourceview=no)
    fi

    if test "x$have_gtksourceview2" = "xyes"; then
      AC_DEFINE(HAVE_GTKSOURCEVIEW,[1],[have gtksourceview])
      AC_DEFINE(HAVE_GTKSOURCEVIEW2,[1],[have gtksourceview2])
    elif test "x$have_gtksourceview" = "xyes"; then
      AC_DEFINE(HAVE_GTKSOURCEVIEW,[1],[have gtksourceview])
    else
      use_gtksourceview=no
    fi
  else
    PKG_CHECK_MODULES(GENIUS, $GENIUS_MODULES)
  fi

  VICIOUS_MODULES="gtk+-2.0 >= $GTK_REQUIRED"
  VICIOUS_UI_LIBS="libviciousui.a"
  
  GNOME_GENIUS="gnome-genius\$(EXEEXT)"
  GENIUS_READLINE_HELPER_FIFO="genius-readline-helper-fifo\$(EXEEXT)"
  GNOME_GENIUS_DESKTOP_IN="gnome-genius.desktop.in"

else
  use_gtksourceview=no
  VICIOUS_MODULES="glib-2.0 >= $GLIB_REQUIRED gmodule-2.0 >= $GLIB_REQUIRED"
  VICIOUS_UI_LIBS=
  GENIUS_CFLAGS="$GENIUS_NOGUI_CFLAGS"
  GENIUS_LIBS="$GENIUS_NOGUI_LIBS"
  GNOME_GENIUS=
  GENIUS_READLINE_HELPER_FIFO=
  GNOME_GENIUS_DESKTOP_IN=
fi

AC_SUBST(VICIOUS_UI_LIBS)
AC_SUBST(GENIUS_CFLAGS)
AC_SUBST(GENIUS_LIBS)
AC_SUBST(GSV_CFLAGS)
AC_SUBST(GSV_LIBS)
AC_SUBST(GNOME_GENIUS)
AC_SUBST(GENIUS_READLINE_HELPER_FIFO)
AC_SUBST(GNOME_GENIUS_DESKTOP_IN)

# This is used for vicious-extensions
PKG_CHECK_MODULES(VICIOUS, $VICIOUS_MODULES)
AC_SUBST(VICIOUS_CFLAGS)
AC_SUBST(VICIOUS_LIBS)

dnl ******************************
dnl More compiler warnings
dnl ******************************

dnl Warnings done above

#AC_ARG_ENABLE(more-warnings,
#[  --enable-more-warnings  Maximum compiler warnings],
#set_more_warnings="$enableval",[
#if test -f $srcdir/.git/config; then
#	is_cvs_version=true
#	set_more_warnings=yes
#else
#	set_more_warnings=no
#fi
#])
#warning_flags=
#realsave_CFLAGS="$CFLAGS"
#AC_MSG_CHECKING(for more warnings)
#if test "$GCC" = "yes" -a "$set_more_warnings" != "no"; then
#	AC_MSG_RESULT(yes)
#	warning_flags="-Wall -Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith"
#	CFLAGS="$warning_flags $CFLAGS"
#
#	for option in -Wno-sign-compare; do
#		SAVE_CFLAGS="$CFLAGS"
#		CFLAGS="$CFLAGS $option"
#		AC_MSG_CHECKING([whether gcc understands $option])
#		AC_TRY_COMPILE([], [],
#			has_option=yes,
#			has_option=no,)
#		CFLAGS="$SAVE_CFLAGS"
#		AC_MSG_RESULT($has_option)
#		if test $has_option = yes; then
#		  warning_flags="$warning_flags $option"
#		fi
#		unset has_option
#		unset SAVE_CFLAGS
#	done
#	unset option
#else
#	AC_MSG_RESULT(no)
#fi
#CFLAGS="$realsave_CFLAGS"
#EXTRA_WARNING_CFLAGS="$warning_flags"
#	-DG_DISABLE_DEPRECATED \
#	-DGDK_PIXBUF_DISABLE_DEPRECATED \
#	-DGDK_DISABLE_DEPRECATED \
#	-DGTK_DISABLE_DEPRECATED \
#	-DGNOME_DISABLE_DEPRECATED \
#
#AC_SUBST(EXTRA_WARNING_CFLAGS)

dnl wordexp check, we use glob otherwise but that's not perfect
dnl warn the user, we do this on the end so that the user notices
AC_CHECK_FUNC(wordexp,AC_DEFINE(HAVE_WORDEXP,[1],[Have the wordexp function]),
  	      [AC_MSG_WARN([*** You don't have wordexp, glob will be used instead ***])])

AC_PATH_PROG(GTK_UPDATE_ICON_CACHE, gtk-update-icon-cache)
if test -z "$GTK_UPDATE_ICON_CACHE"; then
    AC_MSG_ERROR([Could not find gtk-update-icon-cache])
fi

#dnl scrollkeeper checks 
#AC_ARG_ENABLE([scrollkeeper],
#	[AC_HELP_STRING([--disable-scrollkeeper],
#		[do not make updates to the scrollkeeper database])],,
#		enable_scrollkeeper=yes)
#AM_CONDITIONAL([ENABLE_SK],[test "$enable_scrollkeeper" = "yes"])
#
#AC_PATH_PROG(SCROLLKEEPER_CONFIG, scrollkeeper-config,no)
#if test x$SCROLLKEEPER_CONFIG = xno; then
#  AC_MSG_ERROR(Couldn't find scrollkeeper-config. Please install the scrollkeeper package)
#fi

dnl mime checks
AC_ARG_ENABLE(update-mimedb,
	AC_HELP_STRING([--disable-update-mimedb],
		[do not update mime database after installation]),,
		enable_update_mimedb=yes)
AM_CONDITIONAL(ENABLE_UPDATE_MIMEDB, test x$enable_update_mimedb = xyes)


####
# VTE STUFF, this is all temporary pending a port to gtk 3.0 in the star trek future

AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums])
AC_DEFINE_UNQUOTED(VTE_DEFAULT_EMULATION,"xterm",[The default terminal type to be emulated.])
VTE_DEFAULT_EMULATION="xterm"
AC_SUBST(VTE_DEFAULT_EMULATION)

# Check for headers.
AC_CHECK_HEADERS(sys/select.h sys/syslimits.h sys/termios.h sys/un.h sys/wait.h stropts.h termios.h wchar.h)
AC_HEADER_TIOCGWINSZ

# Check for PTY handling functions.
AC_CHECK_FUNCS([cfmakeraw fork setsid setpgid getpgid getpt grantpt unlockpt ptsname ptsname_r tcgetattr tcsetattr])

# Pull in the right libraries for various functions which might not be
# bundled into an exploded libc.
AC_CHECK_FUNC(socket,[have_socket=1],AC_CHECK_LIB(socket,socket,[have_socket=1; LIBS="$LIBS -lsocket"]))
AC_CHECK_FUNC(socketpair,[have_socketpair=1],AC_CHECK_LIB(socket,socketpair,[have_socketpair=1; LIBS="$LIBS -lsocket"]))
AC_CHECK_FUNC(recvmsg,[have_recvmsg=1],AC_CHECK_LIB(socket,recvmsg,[have_recvmsg=1; LIBS="$LIBS -lsocket -lnsl"]))
if test x$have_socket = x1 ; then
	AC_DEFINE(HAVE_SOCKET,1,[Define if you have the socket function.])
fi
if test x$have_socketpair = x1 ; then
	AC_DEFINE(HAVE_SOCKETPAIR,1,[Define if you have the socketpair function.])
fi
if test x$have_recvmsg = x1 ; then
	AC_DEFINE(HAVE_RECVMSG,1,[Define if you have the recvmsg function.])
fi
AC_CHECK_FUNC(floor,,AC_CHECK_LIB(m,floor,LIBS=["$LIBS -lm"]))
AC_CHECK_FUNCS([ceil floor])

# Look for tgetent

AC_CHECK_FUNC([tgetent],[vte_cv_termcap_lib=libc],
  [AC_CHECK_LIB([ncurses],[tgetent],[vte_cv_termcap_lib=libncurses],
    [AC_CHECK_LIB([tinfo],[tgetent],[vte_cv_termcap_lib=libtinfo],
      [AC_CHECK_LIB([curses],[tgetent],[vte_cv_termcap_lib=libcurses],
        [AC_CHECK_LIB([termcap],[tgetent],[vte_cv_termcap_lib=libtermcap],
          [vte_cv_termcap_lib=])])])])])

case "$vte_cv_termcap_lib" in
  libc)       # FIXME: which headers to include here?
              ;;
  libncurses) # We need ncurses.h and term.h, or ncurses/curses.h and ncurses/term.h
              AC_CHECK_HEADERS([ncurses.h term.h],[],
                [AC_CHECK_HEADERS([ncurses/curses.h ncurses/term.h],[],
                  [AC_MSG_ERROR([ncurses headers not found])])])
              LIBS="-lncurses $LIBS"
              ;;
  libtinfo)   # Need ncurses/curses.h and ncurses/term.h
              AC_CHECK_HEADERS([ncurses.h term.h],[],
                [AC_CHECK_HEADERS([ncurses/curses.h ncurses/term.h],[],
                  [AC_MSG_ERROR([ncurses headers not found])])])
              LIBS="-ltinfo $LIBS"
              ;;
  libcurses)  # Need curses.h and term.h
              AC_CHECK_HEADERS([curses.h term.h],[],
                [AC_MSG_ERROR([curses headers not found])],
                [[#ifdef HAVE_CURSES_H
                  #include <curses.h>
                  #endif]])
              LIBS="-lcurses $LIBS"
              ;;
  libtermcap) # Need termcap.h
              AC_CHECK_HEADERS([termcap.h],[],
                [AC_MSG_ERROR([termcap headers not found])])
              LIBS="-ltermcap $LIBS"
              ;;
  *)          AC_MSG_ERROR([tgetent not found in any library]) ;;
esac

# Search for the required modules.

# We have a direct dependency on X11 on gdk-x11, see bug #613525
AC_MSG_CHECKING([for GDK target])
GDK_TARGET="$($PKG_CONFIG --variable target gdk-2.0)"
AC_MSG_RESULT([$GDK_TARGET])
case "$GDK_TARGET" in
  x11) PLATFORM_PKGS="x11 cairo-xlib" ;;
  *) PLATFORM_PKGS="" ;;
esac

VTE_PKGS="glib-2.0 >= $GLIB_REQUIRED gobject-2.0 pango >= $PANGO_REQUIRED gtk+-2.0 >= $GTK_REQUIRED gobject-2.0 gio-2.0 gio-unix-2.0 $PLATFORM_PKGS"
PKG_CHECK_MODULES([VTE],[$VTE_PKGS])
AC_SUBST([VTE_PKGS])

wcs_funcs_includes="
#ifdef HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#else
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
#endif
#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif
"

AC_CHECK_TYPES(wint_t, AC_DEFINE(HAVE_WINT_T, , [Defined when the wint_t type is supported]), ,$wcs_funcs_includes)


########

AC_OUTPUT([
genius.spec
Makefile
src/Makefile
lib/Makefile
lib/calculus/Makefile
lib/combinatorics/Makefile
lib/equation_solving/Makefile
lib/functions/Makefile
lib/linear_algebra/Makefile
lib/misc/Makefile
lib/sets/Makefile
lib/number_theory/Makefile
lib/statistics/Makefile
lib/symbolic/Makefile
lib/commutative_algebra/Makefile
ve/Makefile
vte/Makefile
vte/src/Makefile
vte/termcaps/Makefile
gtkextra/Makefile
examples/Makefile
pixmaps/Makefile
pixmaps/8x8/Makefile
pixmaps/16x16/Makefile
pixmaps/22x22/Makefile
pixmaps/24x24/Makefile
pixmaps/32x32/Makefile
pixmaps/48x48/Makefile
pixmaps/256x256/Makefile
pixmaps/scalable/Makefile
po/Makefile.in
help/Makefile
])

echo "
Configuration:

	Source code location:	     ${srcdir}
	Compiler:		     ${CC} 
	GNOME frontend:		     ${use_gnome}
	Using GtkSourceView:	     ${use_gtksourceview}
"
