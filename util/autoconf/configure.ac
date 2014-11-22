dnl
dnl configure.ac dla Laca 1.4
dnl
dnl wer. 1.1  Lam 24.5.2000  Lac 1.4.-3-pre7
dnl	(pierwsza wersja)
dnl wer. 1.2  Lam 23.11.2001 Lac 1.4.-3-pre17
dnl	(autoconf-2.52, CYGWIN, gtk+)
dnl wer. 1.3  Lam 5.12.2001  Lac 1.4.-3-pre20
dnl	(pkg-config i gtk+-2.0)
dnl wer. 1.4  Lam 3.9.2002   Lac 1.4.-2-pre3
dnl	(--enable-ipv6 zamiast #define)
dnl wer. 1.5  Lam 20.1.2004  Lac 1.4.-1-pre7
dnl	(backtrace_symbols z execinfo.h)
dnl wer. 1.6  Lam 6.11.2004  Lac 1.4.0-pre3
dnl	(strerror_r)
dnl wer. 1.7  Lam 8.11.2004  Lac 1.4.0-pre3
dnl	(sizeof( int ), sizeof( wchar_t ))
dnl wer. 1.8  Lam 11.11.2004 Lac 1.4.0-pre3
dnl	(bzero niepotrzebne)
dnl wer. 1.9  Lam 19.1.2005  Lac 1.4.0-pre7
dnl	(crypt.h)
dnl wer. 1.10 Lam 22.1.2005  Lac 1.4.0-pre7
dnl	(mysql)
dnl wer. 1.11 Lam 18.1.2006  Lac 1.4.0-pre30
dnl	(olanie GTK+ 1.x, wykrywanie gtkspell, crypt nie wymagany)
dnl wer. 1.12 Lam 17.4.2006  Lac 1.4.0-pre38
dnl	(obsluga kompilatorow innych niz gcc, opcje dla HP aC++)
dnl wer. 1.13 Lam 19.10.2006 Lac 1.4.0-pre45
dnl	(printstack)
dnl wer. 1.14 Lam 17.7.2007  Lac 1.4.0-pre50
dnl	(libglade dla glacy)
dnl wer. 1.15 Lam 2.1.2009   Lac 1.4.0-pre55
dnl	(wynik szukania backtrace nie byl zapamietywany)
dnl wer. 1.16 Lam 5.2.2009   Lac 1.4.0-pre56
dnl	(obsluga mingw32 pod Linuksem, strasznie sztywno, ale dziala)
dnl wer. 1.17 Lam 19.11.2011 Lac 1.4.1
dnl	(dodanie -Wextra -Wno-unused-parameter dla gcc)
dnl wer. 1.18 Lam 17.12.2011 Lac 1.4.1
dnl	(lepsza obsluga pthreads, -g dla kompilatora Compaq C)
dnl

m4_include([mysql.m4])
m4_include([pkg.m4])
m4_include([ax_compiler_vendor.m4])
m4_include([ax_append_compile_flags.m4])
m4_include([ax_append_flag.m4])
m4_include([ax_check_compile_flag.m4])
m4_include([ax_check_define.m4])
m4_include([ax_pthread.m4])

AC_REVISION($revision LAC-configure.ac-1.18$)

AC_INIT(merc.h)

LAC=`grep LAC\_VERSION $srcdir/merc.h | sed -e 's/\#define\ LAC_VERSION\ //g' -e 's/\"//g'`

echo
echo $LAC wita w swojej konfiguracji!
echo Za chwile sprobujemy ustawic parametry kompilacji Laca na tym systemie.
echo Niniejszy skrypt wykona mase calkiem niepotrzebnych czynnosci, ale
echo byc moze w wyniku swego dzialania uraczy nas dzialajacym plikiem Makefile?
echo

AC_PROG_CC

AX_COMPILER_VENDOR
case x"$ax_cv_c_compiler_vendor" in
	x"gnu")
		DEBUG_FLAGS="-g3"
		WARN_FLAGS="-Wall"
		AX_APPEND_COMPILE_FLAGS(["-Wextra -Wno-unused-parameter"], [WARN_FLAGS])
		;;
	x"hp")
dnl HP-UX na PA-RISC-u ma inny kompilator niz ten sam HP-UX na Itanium.
dnl Mozna je odrozniach po `what $CC` i grepujac sobie, ale dla systemow,
dnl ktore testowalem, wystarczy na razie sprawdzenie, czy obsluguje -V.
		if $CC -V >/dev/null 2>&1
		then
			DEBUG_FLAGS="-g0"
			WARN_FLAGS="-AC99"
		else
			DEBUG_FLAGS="-g"
			WARN_FLAGS="-AC99"
		fi
dnl Mozna wpisac +w1, ale bardzo mocno sra, trzeba by +W podopisywac, glownie
dnl dla argumentow funkcji, ktorych sie nie uzywa, kazdy kompilator ma inne
dnl numery ostrzezen dla +W dodatkowo.
dnl Mozna tez zerknac na +wsecurity +W20119 (nowsza wersja).
		;;
	x"dec")
		DEBUG_FLAGS="-g"
		WARN_FLAGS=""
		;;
	*)
		DEBUG_FLAGS=""
		WARN_FLAGS=""
		;;
esac

AC_CHECK_PROG(MAKE, gmake, gmake)
if test x"$MAKE" = x; then
  AC_CHECK_PROG(MAKE, make, make)
fi
if test x"$MAKE" = x; then
  AC_MSG_ERROR(Bez make nie skompilujesz LACa.)
fi

AC_SUBST(MAKE)

AC_CHECK_PROG(UNAME, uname, uname)

AC_CACHE_CHECK(system type, lac_cv_var_system_type, lac_cv_var_system_type=`$UNAME -s`)
AC_CACHE_CHECK(system release, lac_cv_var_system_release, lac_cv_var_system_release=`$UNAME -r`)

AC_CHECK_HEADERS(stdio.h time.h sys/types.h sys/socket.h netinet/in.h netdb.h string.h stdlib.h stdarg.h unistd.h crypt.h wchar.h ucontext.h)

dnl autoconf ma swoje DEFS, ale nie uzywam tego, wiec troche nieoptymalnie:
if test x"$ac_cv_header_crypt_h" = x"yes"
then
	DEFINES="$DEFINES -DMAM_CRYPT_H"
fi

AC_ARG_ENABLE(ipv6,
[  --enable-ipv6           Enable IPv6 support],
        if test x$enableval = xyes; then
                want_ipv6=yes
        else
                if test "x$enableval" = xno; then
                        want_ipv6=no
                else
                        want_ipv6=yes
                fi
        fi,
        want_ipv6=no)

PKG_CHECK_MODULES( GTK, gtk+-2.0 >= 2.12 gtkspell-2.0, lac_z_gtk2="yes", [
	DEFINES="$DEFINES -DBEZ_GTKSPELL"
	lac_bez_gtkspell="yes"
	PKG_CHECK_MODULES( GTK, gtk+-2.0 >= 2.12, lac_z_gtk2="yes", echo "Na tym systemie nie skompilujesz glacy!" ) ] )

AM_PATH_MYSQLCLIENT()

AC_CHECK_FUNCS(socket)

AC_CHECK_FUNCS(connect)
if test x"$ac_cv_func_connect" = x"no"; then
  if test x"$ac_cv_func_socket" = x"no"; then
    AC_CHECK_LIB(socket, socket, LIBS="$LIBS -lsocket",)
  fi

  dnl this is for isc.  need the nsl_s library as well.
  AC_CHECK_LIB(inet, socket, libnsl=1; LIBS="$LIBS -linet -lnsl_s",)

  AC_CHECK_LIB(nsl, gethostname, LIBS="$LIBS -lnsl",)
fi  

dnl Haiku:
if test x"$ac_cv_func_socket" = x"no"; then
  AC_CHECK_LIB(network, socket, LIBS="$LIBS -lnetwork",)
fi

dnl Solaris:
AC_CHECK_FUNCS(inet_aton)
if test x"$ac_cv_func_inet_aton" = x"no"; then
  AC_CHECK_LIB(resolv, inet_aton, LIBS="$LIBS -lresolv",)
fi

AC_CHECK_LIB(dgc, inet_addr, LIBS="$LIBS -ldgc",)
AC_CHECK_FUNCS(gethostbyname)
if test x"$ac_cv_func_gethostbyname" = x"no"; then
  AC_CHECK_LIB(resolv, gethostbyname, LIBS="$LIBS -lresolv",)
fi

if test "x$want_ipv6" = "xyes"; then
	AC_MSG_CHECKING([for sockaddr_in6])
	AC_CACHE_VAL(lac_cv_type_sockaddr_in6,
		[AC_TRY_COMPILE([
			#include <sys/types.h>
			#include <sys/socket.h>
			#include <netinet/in.h>
			#include <netdb.h>
			#include <arpa/inet.h>],
			[struct in6_addr i;],
			lac_cv_type_sockaddr_in6=yes,
			lac_cv_type_sockaddr_in6=no,
			)])
	AC_MSG_RESULT($lac_cv_type_sockaddr_in6)
	if test x"$lac_cv_type_sockaddr_in6" = x"yes"; then
		AC_CHECK_FUNCS(getnameinfo)
		if test x"$ac_cv_func_getnameinfo" = x"yes"; then
			AC_CHECK_FUNCS(inet_ntop)
			if test x"$ac_cv_func_inet_ntop" = x"yes"; then
				lac_have_ipv6=yes
				DEFINES="$DEFINES -DIPV6"
			fi
		fi
	fi
fi

AC_MSG_CHECKING([for backtrace_symbols in execinfo.h])
AC_CACHE_VAL(lac_cv_backtrace,
	[AC_TRY_COMPILE([
		#include <execinfo.h>
		#include <stdio.h>
		#include <stdlib.h>],
		[backtrace( 0, 0 ); backtrace_symbols( 0, 0 );],
		lac_cv_backtrace=yes,
		lac_cv_backtrace=no,
		)])
AC_MSG_RESULT($lac_cv_backtrace)
if test x"$lac_cv_backtrace" = x"yes"; then
	DEFINES="$DEFINES -DBACKTRACE"
	if test x"$ax_cv_c_compiler_vendor" = x"gnu"; then
		LIBS="$LIBS -rdynamic"
	fi
else
	AC_CHECK_FUNC(printstack,
		lac_cv_printstack=yes
		DEFINES="$DEFINES -DPRINTSTACK"
		if test x"$ax_cv_c_compiler_vendor" = x"gnu"; then
			LIBS="$LIBS -rdynamic"
		fi, )
fi

AC_CHECK_FUNC(gettimeofday, DEFINES="$DEFINES -DMAM_GETTIMEOFDAY", )

dnl AC_CHECK_FUNCS(iconv)
dnl if test x"$ac_cv_func_iconv" = x"no"; then
dnl   AC_CHECK_LIB(iconv, iconv,
dnl     GTK_LIBS="$GTK_LIBS -liconv", )
dnl fi

AC_CHECK_DEFINE(__MINGW32__, MINGW32=yes)
if test x"$MINGW32" = x"yes"
then
dnl Mingw32 ma obsluge pthreads, super! Problem w tym, ze to wlacza w Lacu
dnl welcome( ), a mi sie szczerze nie chce dopisywac tam obslugi Windows, skoro
dnl kompiluje tym tylko Glace.
  echo "Omitting pthread test on mingw32."
  DEFINES="$DEFINES -DNO_PTHREADS"
else
dnl AC_CHECK_LIB(pthread, pthread_create,
dnl  LIBS="$LIBS -lpthread"; DEFINES="$DEFINES -D_REENTRANT",
dnl  DEFINES="$DEFINES -DNO_PTHREADS")
AX_PTHREAD(LIBS="$LIBS $PTHREAD_LIBS"
     DEFINES="$DEFINES $PTHREAD_CFLAGS"
     CC="$PTHREAD_CC"
     lac_cv_pthreads=yes,
     DEFINES="$DEFINES -DNO_PTHREADS")
fi

AC_CHECK_FUNC(nanosleep, DEFINES="$DEFINES -DNANOSLEEP", )
AC_CHECK_FUNC(wmemset, ac_lac_wmemset=yes, DEFINES="$DEFINES -DNO_WMEMSET" )
AC_FUNC_STRERROR_R
if test "$ac_cv_func_strerror_r" = "no"; then
  DEFINES="$DEFINES -DNO_STRERROR_R"
fi
if test "$ac_cv_func_strerror_r_char_p" = "yes"; then
  DEFINES="$DEFINES -DGNU_STRERROR_R"
fi

if test x"$ax_cv_c_compiler_vendor" = x"gnu"; then
  DEFINES="$DEFINES -DDOPALANY_BUG"
fi

AC_CHECK_SIZEOF(int)
dnl AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(wchar_t)
DEFINES="$DEFINES -DSIZEOF_INT=$ac_cv_sizeof_int -DSIZEOF_WCHAR_T=$ac_cv_sizeof_wchar_t"

lac_cv_nocrypt=no
AC_CHECK_FUNCS(crypt)
if test x"$ac_cv_func_crypt" = x"no"; then
  dnl Brak crypt(), szukam w bibliotece crypt...
  AC_CHECK_LIB(crypt, crypt,
    LIBS="$LIBS -lcrypt",
    lac_cv_nocrypt=yes; DEFINES="$DEFINES -DNOCRYPT"; echo ...--- Lac nie bedzie mogl wczytywac postaci z haslami szyfrowanymi DES ---...)
fi
dnl Okropna proteza, poniewaz QNX ma niezgodna implementacje crypt()
if test x"$lac_cv_nocrypt" = x"no"; then
  dnl Potrzebuje dolaczyc merc.h zeby podefiniowal jakies _XOPEN_SOURCE itp.
  dnl a robimy to w zaleznosci od systemu. W przeciwnym razie, unistd.h moze
  dnl wcale nie zadeklarowac crypt() na wielu systemach.
  dnl Z tego tez powodu przenioslem ten blok na koniec, zeby byly zdefiniowane
  dnl wszystkie DEFINES i program dal sie skompilowac z merc.h i uruchomic.
  ac_save_CFLAGS="$CFLAGS"
  ac_save_LDFLAGS="$LDFLAGS"
  CFLAGS="$CFLAGS $DEFINES"
  LDFLAGS="$LDFLAGS $L_FLAGS"

  AC_MSG_CHECKING([for crypt's standard compliance])
  AC_TRY_RUN([
    #include "merc.h"
    #include <unistd.h>
    #include <string.h>
    int main( void )
    {
      if ( strcmp( crypt( "dupa", "jasia" ), "jadowogE/wcAg" ) ) return 254;
      return ( *crypt( "", "" ) ? 255 : 0 );
    }
    ],
    AC_MSG_RESULT([yes]),
    AC_MSG_RESULT([no])
    lac_cv_nocrypt=yes; DEFINES="$DEFINES -DNOCRYPT"; echo ...--- Lac nie bedzie mogl wczytywac postaci z haslami szyfrowanymi DES ---...)

  CFLAGS="$ac_save_CFLAGS"  
  LDFLAGS="$ac_save_LDFLAGS"
fi


dnl AC_CHECK_LIB(wsock32, WSAStartup,
dnl   LIBS="$LIBS -lwsock32", )
dnl strasznie dretwo rozwiazane, ale chwilowo dziala:
if test x"$MINGW32" = x"yes"
then
	LIBS="$LIBS -lws2_32"
	DEBUG_FLAGS=""
	GTK_CFLAGS="$GTK_CFLAGS -mwindows"
	GTK_LIBS="$GTK_LIBS -liconv -mwindows"
fi

echo
echo Podsumowanie opcji kompilacji:
if test x"$lac_cv_nocrypt" = x"yes"; then
	echo "Szyfrowanie hasel:         tylko SHA-1 (bez wczytywania starych plikow)"
else
	echo "Szyfrowanie hasel:         SHA-1 (domyslnie), DES (wczytywanie starych plikow)"
fi
if test x"$lac_cv_backtrace" = x"yes"; then
	echo "Obsluga stosu wywolan:     tak (backtrace)"
else
	if test x"$lac_cv_printstack" = x"yes"; then
		echo "Obsluga stosu wywolan:     tak (printstack)"
	else
		echo "Obsluga stosu wywolan:     nie"
	fi
fi
if test x"$lac_cv_pthreads" = x"yes"; then
	echo "Obsluga watkow:            tak"
else
	echo "Obsluga watkow:            nie"
fi
if test "x$mysqlclient_fail" = "x" ; then
	LIBS="$LIBS $MYSQLCLIENT_LIBS"
	DEFINES="$DEFINES -DBLEDOMAT $MYSQLCLIENT_CFLAGS"
	L_FLAGS="$L_FLAGS $MYSQLCLIENT_LDFLAGS"
	echo "Obsluga MySQL (bledomat):  tak"
else
	echo "Obsluga MySQL (bledomat):  nie"
fi
if test x"$want_ipv6" = x"yes"; then
	if test x"$lac_have_ipv6" = x"yes"; then
		echo "Obsluga IPv6:              tak"
	else
		echo "Obsluga IPv6:              nie (niedostepna)"
	fi
else
	echo "Obsluga IPv6:              nie (nie wlaczona)"
fi
if test x"$lac_z_gtk2" = x"yes"; then
	echo "Mozliwa kompilacja glacy:  tak"
	if test x"$lac_bez_gtkspell" = x"yes"; then
		echo "Sprawdzanie pisowni:       nie"
	else
		echo "Sprawdzanie pisowni:       tak"
	fi
else
	echo "Mozliwa kompilacja glacy:  nie"
fi
echo

AC_SUBST(DEFINES)
AC_SUBST(LIBS)
AC_SUBST(L_FLAGS)
AC_SUBST(DEBUG_FLAGS)
AC_SUBST(WARN_FLAGS)
AC_SUBST(EXEEXT)

AC_OUTPUT(Makefile)

echo
echo Konfiguracja zakonczona.
echo Jesli wytworzony przez skrypt Makefile nie dziala, napisz do Lama!
echo Napisz teraz \`make\` i baw sie dobrze!
