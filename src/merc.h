#define LAC_VERSION "Lac 1.4.1"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   ___           _______     ________      |                             *
 *  |###|         |#######|   |########| (R) |             Lac             *
 *  |###|        |###|^|###| |###|^^|###|    |                             *
 *  |###|        |###| |###| |###|   ^^^     |    ekipa programistyczna:   *
 *  |###|        |###|_|###| |###|           |   Lam, Qwert, Ulryk, Vigud  *
 *  |###|        |#########| |###|           |      Tissarius, Cadaver     *
 *  |###|        |###|^|###| |###|           |        Alandar, Gimza       *
 *  |###|        |###| |###| |###|   ___     |    trzy grosze dolozyli:    *
 *  |###|______  |###| |###| |###|__|###|    |      Varda, Nop, Fuyara     *
 *  |##########| |###| |###|  |########|     |     Albi, Malven, Muzgus    *
 *   ^^^^^^^^^^   ^^^   ^^^    ^^^^^^^^      |                             *
 * ----------------------------------------------------------------------- *
 *  LACMud improvements copyright (C) 1997 - 2012 by Leszek Matok et al.   *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *   Mitchell Tse.                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *   Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                    *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *   Chastain, Michael Quan, and Mitchell Tse.                             *
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *   Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.  *
 * ----------------------------------------------------------------------- *
 *  In order to use any part of this Lac Diku Mud, you must comply with    *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', the Envy license in 'license.nvy', as well as the Lac   *
 *  license in 'license.lac'. In particular, you may not remove either of  *
 *  these copyright notices.                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#if defined( __NetBSD__ )
# define _NETBSD_SOURCE
#endif

/* Nasze standardowe _XOPEN_SOURCE 600 zawiera POSIX 2001, na co Solaris 10
   reaguje wymaganiem trybu zgodnosci z C99. Oszukujemy go w ten sposob: */
#if defined( sun ) || defined( __sun )
# define __EXTENSIONS__
# define _XOPEN_SOURCE 500
#endif

/* Vigud: spotkalismy takiego Tru64, gdzie cc czy nawet c89 przechodzi bez
   problemu, ale wystepowaly problemy z gcc i oto obejscie jednego z nich: */
#if defined( __osf__ )
# define _POSIX_PII_SOCKET
#endif

/* Vigud: we wszystkich unixach, z ktorymi spotkalem sie do tej pory,
   definiowanie tego typu rzeczy oznacza "uzywam jakiejs funkcji z tego zbioru,
   chce miec do niej dostep", a kolejne define _COSTAM powoduja zwiekszenie
   dostepu do roznych funkcji. We FreeBSD (i pochodnych) jest odwrotnie: kazde
   uzycie define _COSTAM oznacza okreslenie zbioru dostepnych funkcji do
   _COSTAM. Dlatego na tych systemach najlepiej jest niczego nie definiowac
   - wtedy zbior dostepnych rzeczy jest najwiekszy. */
#if !defined( __FreeBSD__ ) && !defined( __NetBSD__ ) && \
    !defined( __OpenBSD__ ) && !defined( __DragonFly__ )
# define _BSD_SOURCE
#endif

/* Vigud: Minix nie zna _XOPEN_SOURCE. */
#if defined( __minix )
# define _POSIX_C_SOURCE 200112L
#endif

/* Vigud: byloby 500, ale mamy strerror_r(). */
#if !defined( _XOPEN_SOURCE ) && defined( _BSD_SOURCE )
# define _XOPEN_SOURCE 600
#endif

/* Vigud: czy to nie jest zadanie dla configure? Tak czy owak, TCC w Gentoo nie
   ustawia unix, wiec musialem dopisac tutaj defined( linux ), zeby dzialalo. */
#if !defined( unix )
# if defined( __NetBSD__ ) || defined( __HAIKU__ ) || defined( __unix__ ) || \
     defined( __minix )    || defined( linux )     || defined( __unix )   || \
     defined( _AIX )       || defined( __linux__ ) || defined( __APPLE__ )
#  define unix
# endif
#endif

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if defined( amiga ) || defined( __AROS__ )
# define AmigaTCP
#endif

#if defined( _WIN32 ) || defined( __WIN32__ ) || defined( __WINDOWS__ ) || defined( __TOS_WIN__ )
# if !defined( WIN32 )
#  define WIN32
# endif
#endif

#if defined( WIN32 ) || defined( MSDOS ) || defined( AmigaTCP )
# if defined( unix )
#  undef unix
# endif
#endif

/* bez tych linijek nie kompilowalyby sie typy danych uzywane w roznych
   strukturach muda. Mozna je umieszczac w kazdym pliku .c, ale po co? */
#include <time.h>
#include <stdio.h>
/* te wystepowaly w wiekszosci plikow .c, tym sposobem wciagne je do .gch */
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>

#if defined( unix ) || defined( __MINGW32__ ) || defined( __DJGPP__ )
# include <unistd.h>
#endif

/* Vigud: tam, gdzie nie ma dostepu do configure, wylaczamy co sie da
   i zgadujemy rozmiar inta. */
#if !defined( SIZEOF_INT )
# define NO_PTHREADS
# define NO_WMEMSET
# define NO_STRERROR_R
# define NOCRYPT
# define getpid() 0
# define SIZEOF_INT 4
#endif

#if !defined( NO_PTHREADS )
# if defined( unix ) || defined( __MINGW32__ )
#  include <pthread.h>
# else
#  define NO_PTHREADS
# endif
#endif

#if defined( WIN32 ) && !defined( __MINGW32__ )
# include <windows.h>
# include <io.h>
# include <direct.h>
# if defined( ABSOLUTE )
#  undef ABSOLUTE
# endif
# if defined( _MSC_VER )
#   define unlink _unlink
#   define chdir _chdir
#   define rmdir _rmdir
#   define read _read
#   define open _open
#   define close _close
#   if (_MSC_VER < 1400)
#     define ftime _ftime
#     define stat _stat
#   endif
#   if (_MSC_VER >= 1400)
#     define timeb _timeb
#   endif
# endif
#endif

/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if defined( NOCRYPT )
# define crypt( s1, s2 ) ( s1 )
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* plik konfiguracyjny */
#include "config.h"

/* zgodnosc z kompilatorami innymi niz GCC lub GCC starszym niz 3.4 */
#if !defined( __GNUC__ ) || __GNUC__ < 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ < 4 )
# if defined( __attribute__ )
#  undef __attribute__
# endif
# define __attribute__( x )
#endif

/*
 * Lam: w pewnych przypadkach chce, aby kod przestal sie kompilowac w zaleznosci
 * od jakiejs sytuacji. Jesli zobaczysz kiedys:
 * error: zero width for bit-field 'jakis_dziwny_komunikat_bledu'
 * to znaczy, ze piszesz cos nie tak jak trzeba :)
 */
#define COMPILER_ASSERT( warunek, komunikat ) \
	{ struct x { unsigned int komunikat: ( warunek ); }; }

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined( TRADITIONAL )
# define const
# define args( list )			( )
# define DECLARE_DO_FUN( fun )		void fun( )
# define DECLARE_SPEC_FUN( fun )	bool fun( )
# define DECLARE_SPELL_FUN( fun )	bool fun( )
# define DECLARE_WEAR_OFF_FUN( fun )	void fun( )	/* Lam */
# define DECLARE_GAME_FUN( fun )	void fun( )
# define DECLARE_CLAN_FUN( fun )	void fun( )	/* Malven */
# define DECLARE_PROG_FUN( fun )	void fun( )	/* Lam */
# define DECLARE_HTML_FUN( fun )	void fun( )
#else
# define args( list )			list
# define DECLARE_DO_FUN( fun )		DO_FUN    fun
# define DECLARE_SPEC_FUN( fun )	SPEC_FUN  fun
/* W przypadku kompilacji Glacy, glaca.o zawiera puste funkcje, ktore latwiej
   zostawic zadeklarowane niz usuwac z tablic w const.c.
   Tak mozna wyciac magic.c i pokrewne, bo nie potrzeba tych funkcji Glacy. */
# if defined( KOMPILUJE_GLACA_C )
#  define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun; CZAR( fun ) { return FALSE; }
#  define DECLARE_WEAR_OFF_FUN( fun )	WEAR_OFF_FUN fun; SPADANIE( fun ) { return; }
# else
#  define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#  define DECLARE_WEAR_OFF_FUN( fun )	WEAR_OFF_FUN fun /* Lam */
# endif
# define DECLARE_GAME_FUN( fun )	GAME_FUN  fun
# define DECLARE_CLAN_FUN( fun )	CLAN_FUN  fun   /* Malven */
# define DECLARE_PROG_FUN( fun )	PROG_FUN  fun	/* Lam */
# define DECLARE_HTML_FUN( fun )	HTML_FUN  fun
#endif


/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if     !defined( FALSE )
# define FALSE    0
#endif

#if     !defined( TRUE )
# define TRUE     1
#endif

#if	!defined( BERR )
# define BERR   255
#endif

#if defined( _AIX )
# if !defined( const )
# define const
# endif
typedef int	bool;
# define unix
#else
/* typedef unsigned char		bool; - nie kompilowalo sie */
/* #define bool unsigned char */
/* aktualnie taka wersja pozwala uniknac ostrzezenia gcc "passing arg * of *
   with different width due to prototype", ktore nie ma sensu moim zdaniem */
typedef int	bool;
#endif


#define sf_family sins.sin.sin_family
#define sf_port sins.sin.sin_port
#define sf_addr sins.sin.sin_addr
#if defined( IPV6 )
# define sf_addr6 sins.sin6.sin6_addr
#endif


/* --- ponizej juz definicje muda --- */

/*
 * Lam: tutaj definicje liczb odpowiadajacych poszczegolnym bitom.
 * fread_number( ) akceptuje liczby w takim formacie. Przydatne do ustawiania
 * flag roznym rzeczom w krainach, bez meczacego siedzenia nad numerkami
 * bxx = 2 do potegi xx, czyli 1 przes. w lewo o xx, czyli bit nr xx
 */
#define		b00	( 1 <<  0 )
#define		b01	( 1 <<  1 )
#define		b02	( 1 <<  2 )
#define		b03	( 1 <<  3 )
#define		b04	( 1 <<  4 )
#define		b05	( 1 <<  5 )
#define		b06	( 1 <<  6 )
#define		b07	( 1 <<  7 )
#define		b08	( 1 <<  8 )
#define		b09	( 1 <<  9 )
#define		b10	( 1 << 10 )
#define		b11	( 1 << 11 )
#define		b12	( 1 << 12 )
#define		b13	( 1 << 13 )
#define		b14	( 1 << 14 )
#define		b15	( 1 << 15 )
#define		b16	( 1 << 16 )
#define		b17	( 1 << 17 )
#define		b18	( 1 << 18 )
#define		b19	( 1 << 19 )
#define		b20	( 1 << 20 )
#define		b21	( 1 << 21 )
#define		b22	( 1 << 22 )
#define		b23	( 1 << 23 )
#define		b24	( 1 << 24 )
#define		b25	( 1 << 25 )
#define		b26	( 1 << 26 )
#define		b27	( 1 << 27 )
#define		b28	( 1 << 28 )
#define		b29	( 1 << 29 )
#define		b30	( 1 << 30 )
#define		b31	( (unsigned) 1 << 31 )


/* teraz jednak beda rzadzily wektory bitow, a to dla nich: */
/* #define BITS_PER_INT ( sizeof( int ) * 8 ) */
#define BITS_PER_INT ( SIZEOF_INT * 8 ) /* SIZEOF_INT znajdzie nam configure */
#if BITS_PER_INT == 16
# error Lac nie skompiluje sie takim gownianym kompilatorem.
#else
# if BITS_PER_INT == 32
#  define MOVE_BITS 5
#  define INT_MASK 31
# else
/* wszystkie 64-bitowe maszyny i tak maja "int" na 32 b */
#  if BITS_PER_INT == 64
#   define MOVE_BITS 6
#   define INT_MASK 63
#  else
#   error Lac nie zna takich mocarnych architektur!
#  endif
# endif
#endif


/* gcc rozumie \e i \E, ale inne kompilatory nie */
#define ESC '\033'
#define ESCS "\033"


/*
 * Ok here we define strdup so it can no longer be confused
 * with str_dup.  Suggested by erwin@pip.dknet.dk - Kahn.
 *
 * Lam: najpierw undef, zeby egcs nie robil afery
 */
#undef strdup
#define strdup STRDUP_ERROR__USE_STR_DUP!

#if !defined( NO_STRERROR_R )
# undef perror
# define perror PERROR_TO_CIENIZNA__TOCAJ_LAC_PERROR
#endif



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		b00	/* Auto set for mobs    */
#define ACT_SENTINEL		b01	/* Stays in one room    */
#define ACT_SCAVENGER		b02	/* Picks up objects     */
#define ACT_NO_HUNT		b03	/* Nie do wytropienia	*/ /* Lam */
#define ACT_NO_TRACK		b04	/* Nie gania za graczami*/ /* Lam */
#define ACT_AGGRESSIVE		b05	/* Attacks PC's         */
#define ACT_STAY_AREA		b06	/* Won't leave area     */
#define ACT_WIMPY		b07	/* Flees when hurt      */
#define ACT_PET			b08	/* Auto set for pets    */
#define ACT_TRAIN		b09	/* Can train PC's       */
#define ACT_PRACTICE		b10	/* Can practice PC's    */
#define ACT_GAMBLE		b11	/* (Wycofane)		*/
#define ACT_PEACEFUL		b12	/* Nie do zaatakowania	*/ /* Lam */
#define ACT_NO_CHARM		b13	/* Nie do oczarowania	*/ /* Lam */
#define ACT_NO_GIFTS		b14	/* Nie bierze prezentow */ /* Ulryk */
#define ACT_DONT_COME_BACK	b15	/* b01 nie wraca	*/ /* Lam */
#define ACT_DOOM_PROOF		b16	/* Zagladoodporny       */ /* Lam */

#define ACT_MOVED		b17	/* Dont ever set!       */

#define ACT_NOT_RACIST		b18	/* Nie jest rasista	*/ /* Lam */
#define ACT_NIE_POMAGA		b19	/* Nie pomaga grupie	*/ /* Lam */

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		0
#define AFF_INVISIBLE		1
#define AFF_DETECT_EVIL		2
#define AFF_DETECT_INVIS	3
#define AFF_DETECT_MAGIC	4
#define AFF_DETECT_HIDDEN	5
#define AFF_HOLD		6
#define AFF_SANCTUARY		7
#define AFF_FAERIE_FIRE		8
#define AFF_INFRARED		9
#define AFF_CURSE		10
#define AFF_CHANGE_SEX		11
#define AFF_POISON		12
#define AFF_PROTECT		13
#define AFF_POLYMORPH		14
#define AFF_SNEAK		15
#define AFF_HIDE		16
#define AFF_SLEEP		17
#define AFF_CHARM		18
#define AFF_FLYING		19
#define AFF_PASS_DOOR		20
#define AFF_WATERWALK		21 /* nieuzywane */
#define AFF_SUMMONED		22 /* nieuzywane */
#define AFF_MUTE		23
#define AFF_GILLS		24
#define AFF_VAMP_BITE		25
#define AFF_GHOUL		26
#define AFF_FLAMING		27
#define AFF_BLACK_AURA		28	/* Lam */
#define AFF_DISORIENTATED	29	/* Skrytobojca */
#define AFF_SPOKOJ		30	/* Qwert */
#define AFF_LODOWA_TARCZA	31	/* Qwert */
#define AFF_PRZYSPIESZENIE	32	/* Qwert */
#define AFF_SPOWOLNIENIE	33	/* Qwert */
#define AFF_REGENERACJA		34	/* Qwert */
#define AFF_DEGENERACJA		35	/* Qwert */
#define AFF_DEZORIENTACJA	36	/* Qwert */
#define AFF_SLUGA		37	/* Ulryk */
#define AFF_GROZA		38	/* Ulryk */
#define AFF_MROK		39	/* Ulryk */
#define AFF_TARCZA_UMARLYCH	40	/* Ulryk */
#define AFF_POSTAC_NIETOPERZA	41	/* Lam */
#define AFF_ANIOL_STROZ		42	/* Gimza */
#define AFF_SWIETA_AURA		43	/* Gimza */
#define AFF_WRZACA_KREW		44	/* Gimza */
#define AFF_DOLADOWANIE		45	/* Gimza */
#define AFF_ILUZJA		46	/* Ulryk */
#define AFF_BRZECZY		47	/* Ulryk */
#define AFF_SZAL_BOJOWY		48	/* Ulryk */
#define AFF_ZABURZENIE_ENERGII	49	/* Ulryk */
#define AFF_BLOKADA_MENTALNA	50	/* Ulryk */
#define AFF_KONTROLA_NAD_WODA	51	/* Lam */
#define AFF_PRZEMIESZCZENIE	52	/* Lam */
#define AFF_TARCZA_ZLA		53	/* Lam */
#define MAX_AFF_FLAG		53 /* zawsze uaktualnic! */
#define AFF_FLAGS_SIZE   ( MAX_AFF_FLAG / BITS_PER_INT + 1 )


/*
 * Alandar: jak kumulowane sa wplywy
 */
#define ADD_AFFECT	0	/* czas i modyfikator liniowo */
#define ENH_AFFECT	1	/* czas i modyfikator malejaco */
#define ENH_TIME	2	/* modyfikator liniowo, a czas malejaco */
#define ENH_MOD		3	/* czas malejaco, a modyfikator liniowo */
#define ENH_MAX		4	/* Lam: odswiezanie - dobijanie do maksimum */


/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL	0
#define SEX_MALE	1
#define SEX_FEMALE	2



/*
 * Item types.
 * Used in #OBJECTS.
 */
/* najpierw to, co mamy z Diku */
#define ITEM_LIGHT                    1
#define ITEM_SCROLL                   2
#define ITEM_WAND                     3
#define ITEM_STAFF                    4
#define ITEM_WEAPON                   5
/* wolne 6 - w Diku: FIRE_WEAPON */
/* wolne 7 - w Diku: MISSILE */
#define ITEM_TREASURE                 8
#define ITEM_ARMOR                    9
#define ITEM_POTION                  10
/* wolne 11 - w Diku: WORN */
#define ITEM_FURNITURE               12
#define ITEM_TRASH                   13
/* wolne 14 - w Diku: TRAP */
#define ITEM_CONTAINER               15
#define ITEM_SWISTEK		     16 /* Lam 23.1.2006 - w Diku: NOTE */
#define ITEM_DRINK_CON               17 /* w Diku: LIQUID_CONTAINER */
#define ITEM_KEY                     18
#define ITEM_FOOD                    19
#define ITEM_MONEY                   20
/* wolne 21 - w Diku: PEN */
#define ITEM_BOAT                    22
/* nowosci Merca */
#define ITEM_CORPSE_NPC              23
#define ITEM_CORPSE_PC               24
#define ITEM_FOUNTAIN                25
#define ITEM_PILL                    26
/* wolne 27-29 */
/* nowosci Laca - zostawilem miejsce na jakies nowosci nowych
wersji Envy (dla zgodnosci krain) */
#define ITEM_PORTAL                  30
#define ITEM_SPELLBOOK		     31 /* Lam 25.9.98 (Lac 1.4.-8) */
#define ITEM_BEACON		     32 /* Lam 8.1.99 (1.4.-5, szamani) */
#define ITEM_SHOVEL		     33 /* Lam 24.3.99 (lopata do kopania) */
#define ITEM_DEVICE		     34 /* Lam 23.1.2006 */
#define ITEM_BANDYTA		     35 /* jednoreki bandyta */
/* po dodaniu do tej listy KONIECZNIE dodaj do create_object( ) (db.c).
   Uzupelnij tez typy_przedmiotow (const.c) */
#define ITEM_MAX		     36

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		0
#define ITEM_HUM		1
#define ITEM_POM		2 /* Tissarius: przedmiot nalezacy do pomieszczenia */
#define ITEM_PRIVATE		3 /* przedmiot osobisty */
#define ITEM_EVIL		4
#define ITEM_INVIS		5
#define ITEM_MAGIC		6
#define ITEM_NODROP		7
#define ITEM_QUEST		8 /* Ulryk: moj pierwszy natchniony kod */
#define ITEM_ANTI_GOOD		9
#define ITEM_ANTI_EVIL		10
#define ITEM_ANTI_NEUTRAL	11
#define ITEM_NOREMOVE		12
#define ITEM_INVENTORY		13
#define ITEM_POISONED		14 /* tylko bronie, zatrute ostrze */
#define ITEM_VAMPIRE_BANE	15
#define ITEM_HOLY		16 /* zadaje obrazenia wampirowi (podczas brania) */
/* lacowe nowosci */
#define ITEM_LOYAL		17 /* nie da sie krasc, rozbrojenie do inw. */
#define ITEM_NORANDVALUES	18 /* nie losuja sie value[ ] */
#define ITEM_NO_LOCATE		19 /* lokalizowanie nie dziala (zadania) */
#define ITEM_BURIED		20 /* zakopane, trzeba odkopac */
#define ITEM_ANTI_MALE		21 /* antymeskie */
#define ITEM_ANTI_FEMALE	22 /* antyzenskie */
#define ITEM_ANTI_NEUTER	23 /* antynijakie */
#define ITEM_INVERT_RACES	24
#define ITEM_INVERT_CLASSES	25
#define ITEM_VANISH		26 /* znika przy wychodzeniu, jak ITEM_KEY */
#define ITEM_REINFORCED		27 /* Fuyara */
#define ITEM_FLAME		28 /* Qwert */
#define ITEM_CHAOS		29 /* Qwert */
#define ITEM_ICY		30 /* Qwert */
#define ITEM_MANA_LEECH         31 /* Gimza: bron ssaca mane z przeciwnika */
#define ITEM_PLAYER_SHOP	32 /* Lam */
#define ITEM_MAX_EXTRA_FLAG	32 /* zawsze uaktualnic! */
#define ITEM_EXTRA_FLAGS_SIZE   ( ITEM_MAX_EXTRA_FLAG / BITS_PER_INT + 1 )

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		b00
#define ITEM_WEAR_FINGER	b01
#define ITEM_WEAR_NECK		b02
#define ITEM_WEAR_BODY		b03
#define ITEM_WEAR_HEAD		b04
#define ITEM_WEAR_LEGS		b05
#define ITEM_WEAR_FEET		b06
#define ITEM_WEAR_HANDS		b07
#define ITEM_WEAR_ARMS		b08
#define ITEM_WEAR_SHIELD	b09
#define ITEM_WEAR_ABOUT		b10
#define ITEM_WEAR_WAIST		b11
#define ITEM_WEAR_WRIST		b12
#define ITEM_WIELD		b13
#define ITEM_HOLD		b14
#define ITEM_WEAR_FACE		b15


/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE                    0
#define APPLY_STR                     1
#define APPLY_DEX                     2
#define APPLY_INT                     3
#define APPLY_WIS                     4
#define APPLY_CON                     5
#define APPLY_SEX                     6
#define APPLY_CLASS                   7
#define APPLY_LEVEL                   8
#define APPLY_AGE                     9
#define APPLY_HEIGHT                 10
#define APPLY_WEIGHT                 11
#define APPLY_MANA                   12
#define APPLY_HIT                    13
#define APPLY_MOVE                   14
#define APPLY_GOLD                   15
#define APPLY_EXP                    16
#define APPLY_AC                     17
#define APPLY_HITROLL                18
#define APPLY_DAMROLL                19
#define APPLY_SAVING_PARA            20
#define APPLY_SAVING_ROD             21
#define APPLY_SAVING_PETRI           22
#define APPLY_SAVING_BREATH          23
#define APPLY_SAVING_SPELL           24
#define APPLY_RACE                   25
#define APPLY_SIZE                   26

/*
 * Typy wartosci, uzywane w typy_przedmiotow[ ] do okreslenia jak traktowac dana
 * wartosc.
 */
#define VAL_NORMAL      0
#define VAL_SPELL       1
#define VAL_MATERIAL    2
#define VAL_STRING      3
#define VAL_LIQUID      4

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		b00
#define CONT_PICKPROOF		b01
#define CONT_CLOSED		b02
#define CONT_LOCKED		b03




/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		b00
#define ROOM_NO_HUNT		b01 /* Lam - wypelniam dziury */
#define ROOM_NO_MOB		b02
#define ROOM_INDOORS		b03
#define ROOM_UNDERGROUND 	b04
#define ROOM_INVERT_RACES	b05 /* Lam */
#define ROOM_INVERT_CLASSES	b06 /* Lam */
#define ROOM_NO_TELEPORT	b07 /* Lam */
#define ROOM_FOG		b08 /* Lam */
/* #define ROOM_PRIVATE		b09 - nie uzywac ponownie -Lam */
#define ROOM_SAFE		b10
/* #define ROOM_SOLITARY	b11 - nie uzywac ponownie -Lam */
#define ROOM_PET_SHOP		b12
#define ROOM_NO_RECALL		b13
#define ROOM_CONE_OF_SILENCE	b14
#define ROOM_CLAN_HP		b15 /* Lam - nie uzywac w krainach! */
#define ROOM_CLAN_MANA		b16 /* Lam - jw. */
#define ROOM_NOTE_READ		b17 /* Lam - do czytania notek - nie glodnieje
						sie, ale tez sie nie odradza */
#define ROOM_PURGATORY		b18 /* Lam - odradza sie, ale nie glodnieje */
#define ROOM_REFRESH		b19 /* Lam - nie dla krain! Do ogniska */
#define ROOM_NO_SUMMON		b20 /* Lam - bez czarow przywolujacych, ale
					dziala / (powrot) */


/*
 * Directions.
 * Used in #ROOMS.
 *
 * Lam 31.5.98: kierunki posrednie: ne, se, nw, sw
 */
#define DIR_NORTH		0
#define DIR_EAST		1
#define DIR_SOUTH		2
#define DIR_WEST		3
#define DIR_UP			4
#define DIR_DOWN		5
#define DIR_NORTHEAST		6
#define DIR_SOUTHEAST		7
#define DIR_NORTHWEST		8
#define DIR_SOUTHWEST		9
#define MAX_DIR			10


/*
 * Flagi wyjsc. Wyjscia sa zrobione zle, nalezy je poprawic. Ponizej numery
 * wewnetrzne Laca, w krainach maja inne odpowiedniki.
 */
#define EX_ISDOOR		b00	/* to drzwi */
#define EX_CLOSED		b01	/* zamkniete */
#define EX_LOCKED		b02	/* zamkniete na klucz */
#define EX_BASHED		b03	/* wywazone */
#define EX_BASHPROOF		b04	/* nie da sie wywazyc */
#define EX_PICKPROOF		b05	/* nie dzialaja wytrychy */
#define EX_PASSPROOF		b06	/* nie da sie przenikac */
#define EX_DIG			b07	/* da sie odkopac/zakopac */
#define EX_BURIED		b08	/* jest zakopane */
#define EX_DONTRESET		b09	/* nie ma miec resetu, prog/spec */
#define EX_BEZKLAMKI		b10	/* z tej strony nie da sie otworzyc */
#define EX_BEZZAMKA		b11	/* z tej strony nie da sie odkluczyc */

/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE                   0	/* Gimza: w przypadku nowych SECT pamietac o dodaniu */
#define SECT_CITY                     1	/* tlumaczenia do typy_podloza! */
#define SECT_FIELD                    2
#define SECT_FOREST                   3
#define SECT_HILLS                    4
#define SECT_MOUNTAIN                 5
#define SECT_WATER_SHALLOW            6
#define SECT_WATER_DEEP               7
#define SECT_UNDERWATER               8
#define SECT_AIR                      9
#define SECT_DESERT                  10
#define SECT_MAX                     11

extern const   int     movement_loss   [ SECT_MAX ];

#define CZY_ZIEMIA( room )	 ( room->sector_type != SECT_WATER_SHALLOW \
				&& room->sector_type != SECT_WATER_DEEP \
				&& room->sector_type != SECT_UNDERWATER \
				&& room->sector_type != SECT_AIR )
#define CZY_WODA( room )	 ( room->sector_type > SECT_MOUNTAIN \
				&& room->sector_type < SECT_AIR )
#define CZY_POD_WODA( room )	 ( room->sector_type == SECT_UNDERWATER )
#define CZY_DNO_POD_WODA( room ) ( room->sector_type == SECT_UNDERWATER \
				&& ( !room->exit[ DIR_DOWN ] \
				  || !room->exit[ DIR_DOWN ]->to_room ) )
#define CZY_POWIETRZE( room )	 ( room->sector_type == SECT_AIR )

/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE                    -1
#define WEAR_LIGHT                    0
#define WEAR_FINGER_L                 1
#define WEAR_FINGER_R                 2
#define WEAR_NECK_1                   3
#define WEAR_NECK_2                   4
#define WEAR_BODY                     5
#define WEAR_HEAD                     6
#define WEAR_LEGS                     7
#define WEAR_FEET                     8
#define WEAR_HANDS                    9
#define WEAR_ARMS                    10
#define WEAR_SHIELD                  11
#define WEAR_ABOUT                   12
#define WEAR_WAIST                   13
#define WEAR_WRIST_L                 14
#define WEAR_WRIST_R                 15
#define WEAR_WIELD                   16
#define WEAR_HOLD                    17
#define WEAR_WIELD_2                 18         /* Thelonius */
#define WEAR_FACE		     19		/* Lam */
#define MAX_WEAR                     20



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/



/*
 * Conditions.
 */
#define COND_DRUNK                    0
#define COND_FULL                     1
#define COND_THIRST                   2



/*
 * Positions.
 */
#define POS_DEAD                      0
#define POS_MORTAL                    1
#define POS_INCAP                     2
#define POS_STUNNED                   3
#define POS_SLEEPING                  4
#define POS_RESTING                   5
#define POS_FIGHTING                  6
#define POS_STANDING                  7



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		b00	/* Don't EVER set. */
#define PLR_BOUGHT_PET		b01
#define PLR_SHOPKEEPER		b02	/* Lam */
#define PLR_SHOP_BREAK		b03	/* Lam */
#define PLR_UZYJ_WYJDZ		b04	/* Lam */
/* wolne 5-11 */
#define PLR_HOLYLIGHT		b12
#define PLR_WIZINVIS		b13
#define PLR_NO_COLOUR		b14
#define PLR_SILENCE		b15
#define PLR_NO_EMOTE		b16
#define PLR_MOVED		b17	/* 131072, musi byc rowne ACT_MOVED */
#define PLR_NO_TELL		b18
#define PLR_LOG			b19
#define PLR_DENY		b20
#define PLR_FREEZE		b21
#define PLR_THIEF		b22
#define PLR_KILLER		b23
#define PLR_LOVE		b24     /* Qwert */
#define PLR_AFK			b25
#define PLR_CHCE_BYC_MORDERCA   b26
#define PLR_ZABOJCA		b27     /* Qwert */
#define PLR_WAMPIR_OD_URODZENIA b28
#define PLR_BOSKA_KUKLA		b29	/* Vigud */

/*
 * Bity ch->info
 */
#define INF_BUDOWNICZY		b02
#define INF_QUESTMASTER		b03
#define INF_LAMER		b04 /* na wniosek RenTora */
#define INF_MORS		b05 /* :) */
#define INF_CLAN_GUARD		b06 /* Cadaver: straznicy porzadku klanowego */
#define INF_STOPIEN_SHOW	b07 /* pokazywanie stopnia niezaleznie od poziomu */
#define INF_KARTOGRAF		b08
#define INF_LACSAKRA		b09

#define CZY_OPCJA( ch, op )	( ch->pcdata && IS_SET( ch->pcdata->opcje, op ) )
/*
 * Bity ch->pcdata->opcje. Zastepstwo ch->act w przypadku opcji.
 */
#define OPC_AUTOEXIT		b00	/* czy ktos ma to wylaczone? */
#define OPC_AUTOGOLD		b01
#define OPC_AUTOLOOT		b02
#define OPC_AUTOSAC		b03
#define OPC_AUTOSPLIT		b04
#define OPC_BLANK		b05
#define OPC_BRIEF		b06
#define OPC_CHYBIANIE		b07
#define OPC_CUTCP		b08
#define OPC_HINT		b09	/* Lam */
#define OPC_MIW			b10	/* Lam */
#define OPC_NO_SEPARATOR	b11	/* Lam */
#define OPC_PROMPT		b12
#define OPC_GWIAZDKI		b13	/* Lam */
#define OPC_SHOWLEVEL		b14
#define OPC_SHOWTICK		b15
/* #define OPC_TELNET_GA	b16 - archaizm */
#define OPC_UNIKI		b17
#define OPC_WRAP		b18	/* Lam */
#define OPC_CISZA		b19	/* Lam */
#define OPC_WCHODZENIE          b20     /* Ulryk */
#define OPC_ANGIELSKIE		b21	/* Lam */
#if defined( MAM_AUTOKLATWE )
# define OPC_AUTOKLATWA		b22	/* Lam */
#endif
#define OPC_POJEMNIKI		b23	/* Lam */
#define OPC_AUTOWCIEL		b24	/* Lam */
#define OPC_RUCH		b25	/* Lam */
#define OPC_CZASPP		b26	/* Vigud */
/* ... */
#define OPC_ODWROC		b31	/* Lam dla act( ) */

/*
 * Bity ch->pcdata->temp_flags - temp_flags nie zapisuja sie w pliku
 */
#define TEMPFLAG_DELETE		b00 /* chce sie skasowac		*/
#define TEMPFLAG_RESTART	b01 /* zamierza restart			*/
#define TEMPFLAG_BACKUP		b02 /* zrobiona kopia przy zapisie	*/
#define TEMPFLAG_DARK		b03 /* ciemno mu			*/
#define TEMPFLAG_BLIND		b04 /* oczy mu sie przyzwyczajaja	*/
#define TEMPFLAG_WOLACZ		b05 /* musi podac wolacz wchodzac	*/
#define TEMPFLAG_VAMPIRE        b06 /* moze wybrac rase wampira		*/
#define TEMPFLAG_MORDERCA       b07 /* w tym pulsie zostal morderca	*/
#define TEMPFLAG_ZABOJCA        b08 /* w tym pulsie zostal zabojca	*/
#define TEMPFLAG_WYMUSZENIE	b09 /* wymuszenie zmiany imienia	*/
#define TEMPFLAG_KMNKT_BWWU	b10 /* komunikat dla BWWU tracacych um. */


/*
 * Channel bits.
 */
#define CHANNEL_AUCTION		b00
#define CHANNEL_CHAT		b01
#define CHANNEL_GOD		b02
#define CHANNEL_HERO		b03
#define CHANNEL_MUSIC		b04
/* #define CHANNEL_QUESTION	b05 */
#define CHANNEL_SHOUT		b06
#define CHANNEL_YELL		b07
/* #define CHANNEL_NCHAT	b08 */
#define CHANNEL_GRATS		b09
#define CHANNEL_FLAME		b10
#define CHANNEL_CLAN		b11
#define CHANNEL_IM		b12
#define CHANNEL_IMMORTAL	b13
#define CHANNEL_NOIC		b31

/*
 * Bity okreslaja, co w przedmiocie nie pasuje do postaci.
 */
#define NOTHING_WRONG		0
#define WRONG_RACE		b00
#define WRONG_CLASS		b01
#define WRONG_PERSON		b02



/*
 * Structure types.
 */
typedef struct	affect_data		AFFECT_DATA;
typedef struct	airobj_data		AIROBJ_DATA;
typedef struct	airchar_data		AIRCHAR_DATA;	/* Alandar 12.11.2003 */
typedef struct	alias_data		ALIAS_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct	auction_data		AUCTION_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct	clan_data		CLAN_DATA;
typedef struct	clan_member_data	CLAN_MEMBER_DATA;
typedef struct	cmd_type		CMD_TYPE;
typedef struct	deathtrap_data		DEATHTRAP_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct	fight_data		FIGHT_DATA;
typedef struct	healer_data		HEALER_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	imiona_data		IMIONA_DATA;
typedef struct	kill_data		KILL_DATA;
typedef struct	koles_data		KOLES_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	note_data		NOTE_DATA;
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data			PC_DATA;
typedef struct	refuse_data		REFUSE_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	restart_data		RESTART_DATA;
typedef struct	restrykcje_data		RESTRYKCJE_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	schemat_data		SCHEMAT_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	sorted_skill_data	SORTED_SKILL_DATA;
typedef struct	tell_data		TELL_DATA;
typedef struct	timer_data		TIMER_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;
typedef struct	who_descriptor_data	WHO_DESCRIPTOR_DATA;
typedef struct	zone_data		ZONE_DATA;
typedef struct	zone_list		ZONE_LIST;
typedef struct	zwod_data		ZWOD_DATA;

typedef struct	mob_prog_data		MPROG_DATA;
typedef struct	mob_trig_prog_data	MTRIG_DATA;
typedef struct	mpquest_data		MPQUEST_DATA;
typedef struct	room_rand_prog_data	RRAND_DATA;
/*
typedef struct	mob_prog_act_list	MPROG_ACT_LIST;
*/
typedef struct	typ_zmiennej		TYP_ZMIENNEJ;
typedef	struct	stosmp			STOSMP;



/*
 * Function types.
 */
typedef void DO_FUN                     args( ( CHAR_DATA *ch,
					       char *argument ) );
typedef bool SPEC_FUN                   args( ( CHAR_DATA *ch ) );
/* Vigud: FIXME: powinno byc unsigned int level */
typedef bool SPELL_FUN                  args( ( int sn, int level,
					       CHAR_DATA *ch, void *vo,
					       int usage ) );
typedef void WEAR_OFF_FUN		args( ( CHAR_DATA *ch ) );
typedef void GAME_FUN                   args( ( CHAR_DATA *ch,
						CHAR_DATA *croupier,
						char *argument ) );
typedef void TIMER_FUN			args( ( TIMER_DATA *timer ) );
typedef void CLAN_FUN                   args( ( CHAR_DATA *ch,		/* Malven */
					       char *argument ) );
typedef void PROG_FUN		args( ( char* argument, CHAR_DATA* ch,
				       CHAR_DATA* actor, OBJ_DATA* obj,
				       CHAR_DATA* rndm, CHAR_DATA* vict,
				       OBJ_DATA* v_obj ) );
typedef void HTML_FUN		args( ( WHO_DESCRIPTOR_DATA *d ) );

/* Lam */
#define KOMENDA( nazwa )	 void nazwa( CHAR_DATA *ch, char *argument )
#define CZAR( nazwa )		 bool nazwa( int sn, int level, \
						CHAR_DATA *ch, void *vo, \
						int usage )
#define SPADANIE( nazwa )	 void nazwa( CHAR_DATA *ch )
#define SPECJALNOSC( nazwa )	 bool nazwa( CHAR_DATA *ch )
#define GRA( nazwa )		 void nazwa( CHAR_DATA *ch, \
					CHAR_DATA *croupier, char *argument )
#define FUNKCJA_CZASOWA( nazwa ) void nazwa( TIMER_DATA *timer )
#define KOMENDA_KLANOWA( nazwa ) void nazwa( CHAR_DATA *ch, char *argument )
#define KOMENDA_PROGOWA( nazwa ) void nazwa( char* argument, CHAR_DATA* ch, \
				       CHAR_DATA* actor, OBJ_DATA* obj, \
				       CHAR_DATA* rndm, CHAR_DATA* vict, \
				       OBJ_DATA* v_obj )
#define HTML( nazwa )		void nazwa( WHO_DESCRIPTOR_DATA *d )

/*
 * Player character key data struct
 * Stuff for new error trapping of corrupt pfiles.
 */
struct key_data
{
    char        key[ 13 ];      /* Increase if you make a key > 12 chars */
    int         string;         /* TRUE for string, FALSE for int        */
    int         deflint;        /* Default value                         */
    void *	deflptr;	/* Default pointer                       */
    void *      ptrs[ 7 ];      /* Increase if you have > 6 parms/line   */
};

#define MAND            3344556 /* Magic # for manditory field           */
#define SPECIFIED       3344557 /* Key was used already.                 */
#define DEFLT           3344558 /* Use default from fread_char_obj       */


struct choice_field_data
{
	int value;
	char *name;
	bool available;
	bool auth_required;
};

struct bit_field_data
{
	int bitv;
	char *name;
	bool available;
	bool auth_required;
	char *tooltip;
};

struct bitvector_field_data
{
	unsigned int bitn;
	char *name;
	bool available;
	bool auth_required;
	char *tooltip;
};


struct bledomat_data
{
	bool wlaczony;
	char *adres;
	char *uzytkownik;
	char *haslo;
	char *baza;
	char *przedrostek;
	int bledy;
	int literowki;
	int pomysly;
};


/*
 * Lam 30.6.98: nowe banowanie
 */
struct ban_data
{
    BAN_DATA *  next;
    char *      host;
    char *	user;
    int		type;
    char *	banned;
};



/*
 * Zapamietywanie wiadomosci wyslanych do gracza po jego odejsciu od klawiatury
 */
struct tell_data
{
    TELL_DATA  *next;
    char       *tell;
};


/*
 * Zadania mobprogowe
 */
struct mpquest_data
{
    MPQUEST_DATA *next;
    int		  vnum;
    int		  stage;
    int		  data[ 5 ];
    char	 *title;
    char	 *descr;
};


/*
 * Lam 24.8.98
 */
struct timer_data
{
    TIMER_DATA	*next;
    CHAR_DATA	*ch;
    int		time;
    bool	paused;	/* Alandar: do zatrzymywania odliczania timera */
    TIMER_FUN	*fun;
    union
    {
	int	i;
	void *	p;
    } arg[ 4 ];
    /* 4 argumenty wystarcza, mozna tam umieszczac wszelkie typy danych */
};

/*
 * Lam 28.3.99
 */
struct airobj_data
{
    AIROBJ_DATA	*next;
    OBJ_DATA *obj;
/*    int wysokosc; */
};

/*
 * Lam 28.3.99
 */
struct airchar_data
{
    AIRCHAR_DATA *next;
    CHAR_DATA *chr;
    int wysokosc;
    bool slaby_plywak;
};


/*
 * Time and weather stuff.
 */
#define SUN_DARK                    0
#define SUN_RISE                    1
#define SUN_LIGHT                   2
#define SUN_SET                     3

#define SKY_CLOUDLESS               0
#define SKY_CLOUDY                  1
#define SKY_RAINING                 2
#define SKY_LIGHTNING               3

struct time_info_data
{
    int         hour;
    int         day;
    int         month;
    int         year;
};

struct weather_data
{
    int         mmhg;
    int         change;
    int         sky;
    int         sunlight;
};

/*
 * Stale szpicla
 */
#define WIZ_ON		b00	/* wlaczenie/wylaczenie szpicla */
#define WIZ_DEATH	b01	/* smierc graczy */
#define WIZ_LEVEL	b02	/* robienie poziomow */
#define WIZ_LOGIN	b03	/* wchodzenie/wychodzenie */
#define WIZ_LINKS	b04	/* utrata polaczenia */
#define WIZ_MURDER	b05	/* nieregulaminowe ataki graczy, sciaganie */
#define WIZ_IDEA	b06	/* pomysl/literowka/blad */
#define WIZ_DEBUG	b07	/* rozne odpluskwiacze */
#define WIZ_IMMCMD	b08	/* oprocz logowania, zastapione WIZ_POL_* */
#define WIZ_PROGBUG	b09	/* progbug( ) */
#define	WIZ_POL_SMIE	b10	/* czy widac w ogole smiertelnikow */
#define WIZ_POL_SMWSZ	b11	/* czy widac wszystkie, a nie tylko logowane */
#define WIZ_POL_BOH	b12	/* czy widac bohaterskie/boskie */
#define WIZ_POL_BOHWSZ	b13	/* wszystkie, a nie tylko logowane */
#define WIZ_POL_PROG	b14	/* czy pokazywac polecenia podczas progow */
#define WIZ_ODM		b15	/* odmiana postaci przez przypadki */
#define WIZ_NOWI	b16	/* zakladanie nowych postaci */
#define WIZ_BUG		b17	/* zglaszanie bledow */


/*
 * Connected state for a channel.
 */
#define CON_PLAYING			 0
#define CON_GET_NAME			 1
#define CON_GET_OLD_PWD			 2
#define CON_CONFIRM_NEW_NAME		 3
#define CON_GET_NEW_PWD			 4
#define CON_CONFIRM_NEW_PWD		 5
#define CON_GET_NEW_RACE		 6
#define CON_CONFIRM_NEW_RACE		 7
#define CON_GET_NEW_SEX			 8
#define CON_GET_NEW_CLASS		 9
#define CON_CONFIRM_CLASS		10
#define CON_GET_MULTI			11  /* multi-profesje */
#define CON_CONFIRM_MULTI		12  /* jw. */
#define CON_GET_POLSKIE			13
#define CON_CONFIRM_POLSKIE		14
#define CON_READ_MOTD			15
#define CON_JUZ_GRAL_PWD_OK		16  /* juz gral, najpierw haslo */
#define CON_GET_OLD_PWD_GRAL		17  /* mowi, ze gral, ale czy to on? */

/* nie wstawiac niczego miedzy ponizsze 18-22, bo one sa obslugiwane inaczej
   przy utracie polaczenia (comm.c)! */
/* ustawiac ponizsze tylko po save_char_obj( d->character )! */
#define CON_GET_OLD_PWD_DELETE		18  /* chce sie skasowac, wiec haslo */
#define CON_GET_OLD_PWD_RESTART		19  /* restart (boski) - jw. */
#define CON_GET_OLD_PWD_PWD		20
#define CON_GET_NEW_PWD_PWD		21
#define CON_CONFIRM_NEW_PWD_PWD		22

/* Lam 29.11.98: nowe zakladanie postaci */
#define CON_GET_NEW_NAME		23
#define CON_GET_ODMIANA			24
#define CON_CONFIRM_ODMIANA		25
/* Lam 31.3.2002: strefy */
#define CON_GET_NEW_STREFA		26
#define CON_CONFIRM_STREFA		27
#define CON_READ_REGULAMIN		28
#define CON_UMOWA			29
#define CON_GET_WOLACZ			30
#define CON_CONFIRM_WOLACZ		31
/* adres email przy zakladaniu postaci */
#define CON_GET_EMAIL			32
#define CON_CONFIRM_EMAIL		33

/* Ulryk 08.02.2004 */
struct stats_type
{
    int         day [ MAX_DAYS ];   /* najwiecej graczy w ciagu ostatnich dni */
    int         total;              /* najwiecej graczy od startu */
};

/* Lam 10.1.2003 */
struct holiday_type
{
    int		month;		/* miesiac, 1-17 */
    int		day;		/* dzien w miesiacu, 1-35 */
    char	*name;		/* nazwa swieta */
    char	*message;	/* komunikat wysylany graczom o polnocy */
};

struct material_type
{
    char	*name;
    char	*dopelniacz;
    int		 minlevel;
    int		 maxlevel;
    int		 ac; /* ( ac / 8 ) * value[ 0 ] */
    int		 value; /* jw. * cost */
    int		 weight; /* jw. * weight */
    bool	 vampburn; /* czy pali wampiry */
/*  int		 forms; */
/* bity - jakich form nie moze miec */
};

#define		FORMA_KOLCZUGA	b00
#define		FORMA_PLYTOWA	b01

/*
 * Struktura dla typy_przedmiotow (const.c)
 */
struct object_type
{
    char *name;
    char *name_pl;
    char *value[ 6 ];
    bool  spell[ 6 ];
};

/*
 * Struktura dla kierunki (const.c)
 */
struct dir_type
{
    char *name;
    char *skrot;
    char *skrotd;
    char *krotki;
    char *biernik;
    char *z_dopelniacz;
    int   odwrotny;
    char *na_krew;
};


struct spec_type
{
    SPEC_FUN *fun;
    char *name;
};


struct game_type
{
    GAME_FUN *fun;
    char *name;
    int bilans;
    int grano;
};


/*
 * Race structures
 */
struct race_type
{
    char *              who_name;
    char *		dopelniacz;
    char *		celownik;
    char *		biernik;
    char *		narzednik;
    char *		miejscownik;
    char *		wolacz;
    char *              name;/* polska bez polskich znakow */
    char *              ang; /* angielska nazwa - zeby wiedzial, jaka rasa */
    int                 race_abilities;
    int			oddychanie;
    int			poruszanie;
    int                 size;
    int			weight;
    int                 str_mod;
    int                 int_mod;
    int                 wis_mod;
    int                 dex_mod;
    int                 con_mod;
    int                 hp_gain;
    int                 mana_gain;
    int                 move_gain;
    int                 thirst_mod;
    int                 hunger_mod;
    char *              dmg_message;
    int			rodzaj;
    char *		hate;
    char *		kom_wejscia; /* Lam */
    char *		kom_wyjscia; /* Lam */
    int			wearlocs; /* Lam */
    int			food;	/* Alandar */
    int			parts; /* Lam */
    int *		zr; /* Lam: zapamietana rasa */
};


struct czesc_ciala
{
    int *wsk;
    int vnum;
    int masa; /* proporcja; <masa> razy ciezar rasy, podzielic przez 1024 */
    bool rasa_w_opisie;
    char *komunikat;
};


/* czesci ciala przedstawiciela danej rasy */
#define P_NONE		  0 /* bezczesciowy */
#define P_FLAKI		b00
#define P_GLOWA		b01
#define P_RECE		b02
#define P_NOGI		b03
#define P_SERCE		b04
#define P_OGON		b05
#define P_SKRZYDLA	b06
#define P_USZY		b07
#define P_LAPA		b08
#define P_LEB		b09 /* niektorzy nie zasluguja na glowe :) */
#define P_ZAB		b10
#define P_KIEL		b11
#define P_ROG		b12
#define P_SZPON		b13
#define P_PLETWA	b14
#define P_JEZYK		  0 /* nie umiem wytlumaczyc jego odpadania od ciala */

#define P_HS		( P_FLAKI|P_GLOWA|P_RECE|P_NOGI|P_SERCE|P_JEZYK|P_USZY|P_ZAB )
/* wszystkie czesci czlekopodobne (Homo Sapiens) */

/* race_type->wearlocs, czyli gdzie dana rasa nie moze niczego zakladac */
#define WRL_HS		0
#define WRL_TRZYMANE	ITEM_HOLD
#define WRL_PALEC	ITEM_WEAR_FINGER
#define WRL_SZYJA	ITEM_WEAR_NECK
#define WRL_CIALO	ITEM_WEAR_BODY
#define WRL_GLOWA	ITEM_WEAR_HEAD
#define WRL_NOGI	ITEM_WEAR_LEGS
#define WRL_STOPY	ITEM_WEAR_FEET
#define WRL_DLONIE	ITEM_WEAR_HANDS
#define WRL_RAMIONA	ITEM_WEAR_ARMS
#define WRL_TARCZA	ITEM_WEAR_SHIELD
#define WRL_WOKOL	ITEM_WEAR_ABOUT
#define WRL_PAS		ITEM_WEAR_WAIST
#define WRL_NADGARSTEK	ITEM_WEAR_WRIST
#define WRL_BRON	ITEM_WIELD
#define WRL_TWARZ	ITEM_WEAR_FACE

/* Race ability bits */
#define RACE_NO_ABILITIES		  0
#define RACE_PC_AVAIL			b00
/* #define RACE_WATERBREATH		b01 - ODD_WODA */
/* #define RACE_FLY			b02 - POR_POWIETRZE */
/* #define RACE_SWIM			b03 - POR_WODA */
/* #define RACE_WATERWALK		b04 - POR_WODA | POR_POWIETRZE */
#define RACE_PASSDOOR			b05
#define RACE_INFRAVISION		b06
#define RACE_DETECT_EVIL		b07
#define RACE_DETECT_INVIS		b08
#define RACE_DETECT_HIDDEN		b09
#define RACE_PROTECTION			b10
#define RACE_SANCT			b11
#define RACE_WEAPON_WIELD		b12
#define RACE_MUTE			b13
#define RACE_NO_MOUTH			b14	/* Alandar: nie moze jesc */
#define RACE_POISONIMM			b15	/* Alandar: odporne na trucizne */
#define RACE_OPENS_DOORS		b16	/* Baarian */
#define RACE_NO_BLOOD			b17	/* Muzgus */
#define RACE_PROTECT_GOOD		b18	/* Lam */
#define RACE_NO_CORPSE			b19     /* Ulryk: nie zostawia zwlok */
#define RACE_NO_EYES			b20	/* Lam: nie da sie oslepic */
/* #define RACE_NIE_ODDYCHA		b21 - wystarczy ustawic oba ODD_ */
#define RACE_BEZ_SLUCHU			b22	/* Lam */
#define RACE_BEZ_WECHU			b23	/* Lam */
#define RACE_WOLI_LATAC			b24	/* Lam */
#define RACE_WOLI_PLYWAC		b25	/* Lam */

/* oddychanie */
#define ODD_POWIETRZEM			b01
#define ODD_WODA			b02

/* poruszanie */
#define POR_POD_WODA			b01 /* nurkowanie */
#define POR_PO_WODZIE			b02 /* plywanie po powierzchni */
#define POR_LAD				b03 /* poruszanie ladem */
#define POR_POWIETRZE			b04 /* latanie, implikuje lad i nad woda */
/* chociaz latanie implikuje lad i nad woda, warto ustawic LAD | POWIETRZE,
   dzieki temu bedziemy mogli zrobic, ze moby z samym POWIETRZE beda odfruwac
   nawet z pomieszczen ladowych, a LAD | POWIETRZE odchodzic */

/* zmiana koncepcji: bez LAD postac z samym POWIETRZE nie moglaby chodzic po
   dnie wody */


/*
 * Ulryk: struktura wspomagajaca prace zintegrowanego serwera WWW
 */
struct html_page_type
{
    char           *name;
    char           *short_link;
    char           *long_link;
    HTML_FUN       *fun;
};

/*
 * Drunkeness communication structure.
 */
struct struckdrunk
{
    int                 min_drunk_level;
    int                 number_of_rep;
    char               *replacement[ 11 ];
};


/*
 * Descriptor (channel) structure.
 *
 * Sporo zmian wprowadzonych przez Lama
 * Lam 6.11.2000: liczniki wejscia i wyjscia
 * Lam 24.11.2001: char * -> char[ ], zeby welcome( ) nie psulo sterty SSM-a
 */
struct descriptor_data
{
    DESCRIPTOR_DATA *   next;
    DESCRIPTOR_DATA *   snoop_by;
    DESCRIPTOR_DATA *	nast_na_poz;
    CHAR_DATA *         character;
    CHAR_DATA *         original;
    char		host		[ 256 ];
    int			port;
    char		login		[ 384 ];
    unsigned int        descriptor;
/*    unsigned int	addr; */
    int                 connected;
    int			idle;
    bool                fcommand;
    char                inbuf           [ BUF_LIMIT_INBUF ];
    char                incomm          [ MAX_INPUT_LENGTH ];
    char                inlast          [ MAX_INPUT_LENGTH ];
    char                predlast        [ MAX_INPUT_LENGTH ];
/*    int                 repeat; */
    int			polskie;
    char *		showstr_head;
    char *		showstr_point;
    char *		outbuf;
    int                 outsize;
    int                 outtop;
    char		user		[ CONN_USER_LEN ];
    char		term_type	[ CONN_TERM_LEN ];
    char		lsc_host	[ 256 ];
    char		lsc_user	[ 128 ];
    char		ipaddr		[ 32 ];
    int			szer;	/* pobrane z NAWS */
    int			wys;	/* jw. */
    int			wej_znaki;
    int			wej_linie;
    int			wyj_znaki;
    int			wyj_linie;
#if !defined( MSDOS )
    volatile int	welcome_state;
# if !defined( NO_PTHREADS )
    pthread_t		thread;
# endif
#endif
    bool		telnet_state;
    bool		nowe_dane;
    bool		wait;
/*    bool		char_mode; */
    char		kolor;
};

/* stale d->welcome_state */
#define WELCOME_DONE	0
#define WELCOME_PENDING	1
#define WELCOME_BAN	2

/* stale d->telnet_state */
#define TELNET_NORM	0
#define TELNET_IAC	1
#define TELNET_SB	2
#define TELNET_TTYPE0	3
#define TELNET_TTYPE	4
#define TELNET_NAWS1	5
#define TELNET_NAWS2	6
#define TELNET_NAWS3	7
#define TELNET_NAWS4	8
#define TELNET_IGNTOSE	9
#define TELNET_IGNTOSE2	10
#define TELNET_DWDW	11 /* DO/WILL/DONT/WONT */

/*
 * Analogiczna struktura do polaczen z portem HTTP (pierwotnie tylko who)
 */
struct who_descriptor_data
{
    WHO_DESCRIPTOR_DATA *next;
    unsigned int	descriptor;
    char		host[ 256 ];
    char                odnosnik[ MIL ];
    char                zapytanie[ MIL ];
    char                klient[ MSL ];
    char                referer[ MSL ];
    int			idle;
    char		*outbuf;
    int			outsize;
    int			outtop;
    bool		cts; /* clear to send */
};

/*
 * Attribute bonus structures.
 */
struct str_app_type
{
    int                 todam;
    int                 carry;
    int                 wield;
    int                 uwalnianie;
};

struct int_app_type
{
    int                 learn;
    int			mana;
};

struct wis_app_type
{
    int                 practice;
    int                 opc;
};

struct dex_app_type
{
    int			defensive;
    int			tohit;
};

struct con_app_type
{
    int                 hitp;
    double              wyparowanie;
    int                 shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM             0
#define TO_NOTVICT          1
#define TO_VICT             2
#define TO_CHAR             3
#define ACT_TELEP         b06



/*
 * Help table types.
 */
struct help_data
{
    HELP_DATA *         next;
    AREA_DATA *		area;
    int                 level;
    char *              keyword;
    char *              text;
};



/*
 * Shop types.
 */
/* nie przenosze do config.h, bo by nikt nie zrozumial co to znaczy */
#define MAX_TRADE        5

struct shop_data
{
    int         keeper;                 /* Vnum of shop keeper mob      */
    int         buy_type [ MAX_TRADE ]; /* Item types shop will buy     */
    int         profit_buy;             /* Cost multiplier for buying   */
    int         profit_sell;            /* Cost multiplier for selling  */
    int         open_hour;              /* First opening hour           */
    int         close_hour;             /* First closing hour           */
    bool	quest_shop;		/* Lam 1.1.2004			*/
};


/*
 * Lam: Sprzedawcy czarow - sciagalem ze sklepow, ale tylko troche ;)
 */
struct healer_data
{
    int		keeper;			/* vnum of spell-selling mob	*/
    int		spells [ 5 ];		/* Spells to cast		*/
    int		cost   [ 5 ];		/* Costs of spell casting	*/
};


/*
 * Per-class stuff.
 */
struct class_type
{
    char        who_name      [ MIL ];  /* Three-letter name for 'who'	*/
    char	pl_name       [ MIL ];  /* Wersja polska do "kto"	*/
    char	long_name     [ MIL ];  /* Wersja do "kimjest"		*/
    char	ang           [ MIL ];	/* Stara nazwa angielska	*/
    int         attr_prime;             /* Prime attribute		*/
    int        *weapon;                 /* First weapon			*/
    int         skill_adept;            /* Maximum skill level		*/
    int         thac0_00;               /* Thac0 for level  0		*/
    int         thac0_47;               /* Thac0 for level 47		*/
    int         hp_min;                 /* Min hp gained on leveling	*/
    int         hp_max;                 /* Max hp gained on leveling	*/
    double	bonus;			/* Doswiadczenie		*/
    bool        fMana;                  /* Class gains mana on level	*/
    char       *races;			/* Rasy mogace nim byc		*/
};



/*
 * Pojedyncza notka.
 * Lam 8.4.98: dodana mozliwosc glosowania
 * Lam 28.8.98: kategorie
 */
struct note_data
{
    NOTE_DATA * next;
    char *      sender;
    char *      date;
    char *      to_list;
    char *      subject;
    char *      text;
    char *	yes;	/* glosujacy za "tak" */
    char *	no;	/* glosujacy za "nie" */
    char *	abst;	/* wstrzymujacy sie   */
    int		yesnum, nonum, abstnum; /* liczniki */
    int		vote;	/* czy glosowanie? */
    int		cat;	/* kategoria */
    time_t      date_stamp;
};

/* stale note_data->vote */
#define VOTE_NO		0	/* nie da sie glosowac   */
#define VOTE_OPEN	1	/* otwarte glosowanie    */
#define VOTE_CLOSED	2	/* zakonczone glosowanie */


/*
 * Lam (5.5.97?): aliasy
 */
struct alias_data
{
    ALIAS_DATA *	next;
    char *		name;
    char *		todo;
};


/*
 * Malven: klany, na bazie klanow Lama
 */
#define MAX_CLAN_NAME	    40
#define MAX_CLAN_DOPELNIACZ 40
#define MAX_PLAYER_NAME     12  /* Malven: 12 za check_parse_name( ) w nanny.c */

#if defined( unix )
# define MAX_CLAN_DIR   MAX_CLAN_NAME
#else
# define MAX_CLAN_DIR   8
#endif

/*
 * Stany klanow - Malven:
 * uszkodzony - brak jakiegos pliku lub niezbednego klucza w pliku dane.cln
 * usuniety - czlonkowie zostaja usunieci z klanu
 * zawieszony - przez SPK lub bogow, czlonkowie pozostaja w klanie ale wszystkie
 *		polecenia zablokowane
 * nowy - zanim zostanie zatwierdzony przez SPK, polecenia zablokowane
 * aktywny - zatwierdzony przez SPK
 * pomoc_usuwany - status pomocniczy wykorzystywany przy usuwaniu klanu, taka
 *		wartosc nie moze istniec w pliku
 */
#define CLAN_STAT_USZKODZONY		0
#define CLAN_STAT_USUNIETY		1
#define CLAN_STAT_ZAWIESZONY		2
#define CLAN_STAT_NOWY			3
#define CLAN_STAT_AKTYWNY		4
#define CLAN_STAT_POMOC_USUWANY		-1

/*
 * Stale przynaleznosci do klanu (ch->pcdata->clan_level)
 */
#define CLAN_NONE	 -1  /* bez klanu */
#define CLAN_GUEST        0  /* kandydat do zapisania sie */
#define CLAN_MEMBER       1  /* normalny czlonek klanu */
#define CLAN_HERO         5  /* bohater klanu */
#define CLAN_MASTER       7  /* mistrz klanu */

/*
 * Flagi klanu (clan->flags)
 * clanroom nie ma flagi, tylko clan->room to pokazuje. Potem juz tymi bitami:
 */
#define CLAN_FLAG_BEDROOM	b00  /* pomieszczenie do leczenia */
#define CLAN_FLAG_HP		b01  /* pomieszczenie z zyciem */
#define CLAN_FLAG_MANA		b02  /* pomieszczenie z mana */
#define CLAN_FLAG_LIBRARY	b03  /* klan ma biblioteke */
#define CLAN_FLAG_PIT		b04  /* klan posiada wlasna urne */

/*
 * Malven: flagi polecen klanowych
 */
#define CLAN_CMD_SPKONLY	b00  /* tylko dla SPK i bogow */
#define CLAN_CMD_UKRYTE		b01  /* polecenie nie wyswietla sie na liscie polecen */
#define CLAN_CMD_OOC		b02  /* moze byc wykonywane poza postacia */
#define CLAN_CMD_NOIC		b03  /* nie moze byc wykonywane w postaci */
#define CLAN_CMD_SPK		b04  /* SPK i bogowie moga wykonywac na specjalnych warunkach */

/*
 * Flagi zapisu
 */
#define CLAN_WRITE_ALL          b00 | b01 | b02 | b03 /* zapisanie wszystkiego */
#define CLAN_WRITE_DANE		b00  /* podstawowe dane klanu */
#define CLAN_WRITE_LISTA	b01  /* lista klanowiczow */
#define CLAN_WRITE_STOSUNKI	b02  /* stosunki miedzyklanowe */
#define CLAN_WRITE_SKRZYNIA	b03  /* skrzynia klanowa */

/*
 * znaczenia zmiennej rel->pokoj
 */
#define CLAN_POKOJ_BRAK		0	/* nie bylo deklaracji pokoju */
#define CLAN_POKOJ_DEKLARACJA	1	/* zlozona deklaracja pokoju */
#define CLAN_POKOJ_ODRZUCONY	2	/* odrzucona deklaracja pokoju */
#define CLAN_POKOJ_MAX		2

/*
 * Malven: klanowe zmienne globalne w const.c
 */
extern const char * clan_lev_name_mm[ 10 ];
extern const char * clan_lev_name_zm[ 10 ];
extern const char * clan_lev_name_md[ 10 ];
extern const char * clan_lev_name_zd[ 10 ];
extern const char * clan_lev_name_mn[ 10 ];
extern const char * clan_lev_name_zn[ 10 ];
extern const char * clan_stat_name[ 5 ];
extern const int    max_clan_level_members[ 10 ];

/*
 * Malven: stosunki miedzyklanowe
 */
/*CLAN_REL_DATA * clan_rel_lookup( CLAN_DATA *clana, CLAN_DATA *clanb );*/

typedef struct clan_rel_data  CLAN_REL_DATA;

struct clan_rel_data
{
    CLAN_REL_DATA * next;	/* nastepny na liscie */
    CLAN_DATA     * clan;	/* drugi klan */
    bool war;
    int             stosunek;	/* stosunek do drugiego klanu */
    int             wpkill;	/* liczba zabojstw w trakcie tej wojny */
    int             wpdead;     /* liczba zgonow w trakcie tej wojny */
    int	            pkill;	/* liczba zabojstw w sumie */
    int	            pdead;	/* liczba zgonow w sumie */
};

/*
 * Struktura listy klanowiczow
 */
struct clan_member_data
{
    CLAN_MEMBER_DATA  * next;		/* nastepny klanowicz */
    CHAR_DATA	      * ch;		/* jesli gra, to nam na niego wskaze */
    char              * name;		/* imie */
    short               level;		/* poziom klanowy */
    int			zloto;		/* wplacone zloto */
    int			punkty;		/* wplacone punkty klanowe */
    time_t		seen;		/* kiedy ostatnio byl widziany */
#if 0
    int			skll;		/* umiejetnosci klanowe */
    int			pkill;		/* tyle razy zabil dla klanu */
    int			pdead;		/* tyle razy zginal za klan */
#endif
};

/*
 * Struktura klanu
 */
struct clan_data
{
    CLAN_DATA	      * next;         /* nastepny klan na liscie */
    int			status;       /* stan klanu */
    char	      * name;         /* pelna nazwa klanu */
    char	      * dop;          /* dopelniacz nazwy */
    char	      * dir;          /* katalog klanu */
    char	      * info;         /* informacja o klanie */
    char              * statut;       /* statut klanu */
    char	      *	skrot;        /* skrot nazwy klanu */
    char	      * data_zal;     /* data zalozenia klanu */
    CLAN_MEMBER_DATA  * member_list;  /* lista czlonkow wraz ze stopniami */
    CLAN_REL_DATA     * rel_list;     /* lista stosunkow z innymi klanami */
    char	      * lev_name_mm[ 10 ]; /* nazwy stopni - meski mianownik */
    char	      * lev_name_zm[ 10 ]; /* zenski mianownik */
    char	      * lev_name_md[ 10 ]; /* meski dopelniacz */
    char	      * lev_name_zd[ 10 ]; /* zenski dopelniacz */
    char	      * lev_name_mn[ 10 ]; /* meski narzednik */
    char	      * lev_name_zn[ 10 ]; /* zenski narzednik */
    int			gold;         /* konto bankowe klanu */
    int			punkty;       /* punkty klanowe */
#if 0
    char	      * area;	      /* kraina z twierdza klanowa */
    int			room;         /* pomieszczenie klanowe */
    int			flags;        /* flagi klanu i pomieszczenia */
#endif
    OBJ_DATA	      * urna;         /* urna klanowa */
};

/*
 * Struktura polecenia klanowego w tablicy
 */
struct clan_cmd_type
{
    char * const	name;        /* nazwa polecenia */
    CLAN_FUN *		clan_fun;    /* funkcja realizujaca polecenie */
    int			clan_level;  /* min. stopien konieczny do wykonania */
    int			clan_stat;   /* min. status klanu konieczny do wyk. */
    int			flags;       /* flagi polecenia klanowego */
};


/*
 * An affect.
 */
struct affect_data
{
    AFFECT_DATA *       next;
    CHAR_DATA *		caster; /* Lam */
    int                 type;
    int                 duration;
    int                 location;
    int                 modifier;
    int                 bitvector[ AFF_FLAGS_SIZE ];
    int			level; /* Lam */
    bool                deleted;
};


/*
 * A kill structure (indexed by level).
 */
struct kill_data
{
    int                 number;
    int                 killed;
};


struct schemat_data
{
    SCHEMAT_DATA      * next;         /* nastepny schemat na liscie */
    char	      * nazwa;        /* nazwa schematu */
    char	      * schemat;      /* sam schemat */
};


/*
 * Gimza: odmow
 */
struct refuse_data
{
    REFUSE_DATA		*next;	     	      /* nastepne zablokowane polecenie */
    char		refused_cmd[ 30 ];    /* nazwa polecenia */
    char		who[ 40 ];   	      /* kto zabanowal */
    char		why[ 80 ];	      /* za co zabanowal */
    char		date[ 15 ];	      /* data akcji */
};

/*
 * Lam 3.6.2004: historia restartow
 */
struct restart_data
{
    RESTART_DATA	*nast;
    char		*wpis;
};

/*
 * Ulryk 18.03.2007: ustrukturalizowane restrykcje
 */
struct restrykcje_data
{
    char	* od;		/* od kiedy, od kogo itp... to, co dotychczas */
    time_t	  do_kiedy;	/* do kiedy */
};

/* Lam 17.10.2006: zablokowane imiona */
struct imiona_data
{
    IMIONA_DATA	*nast;
    char	*imie;
    char	*blokujacy;
    int		powod;
    time_t	kiedy;
};

#define LISTA_IMION( imie ) ( &imiona[ strlen_pl( imie ) ][ UPPER( *( imie ) ) & 63 ] )
#define OST_IMIE( imie ) ( &imiona_ost[ strlen_pl( imie ) ][ UPPER( *( imie ) ) & 63 ] )


struct powody_data
{
    char	*skrot;
    char	*nazwa;
    char	*tytul;
    char	*objasnienie;
};


struct auction_data
{
    OBJ_DATA	*item;
    CHAR_DATA	*seller;
    CHAR_DATA	*buyer;
    int		bet;
    int		going;
    int		pulse;
    int		starting;
    int         zastaw;
};


/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct mob_index_data
{
    MOB_INDEX_DATA *    next;
    MOB_INDEX_DATA *	next_in_area;	/* Lam */
    AREA_DATA *		index_area;	/* Lam */
    SPEC_FUN *          spec_fun;
    GAME_FUN *		game_fun;	/* Lam */
    SHOP_DATA *         pShop;
    HEALER_DATA *	pHealer;
    MPROG_DATA *	mobprogs;
    char *              player_name;
    char *              short_descr;
    char *              long_descr;
    char *              dopelniacz;
    char *              celownik;
    char *              biernik;
    char *              narzednik;
    char *              miejscownik;
    char *              wolacz;
    char *              description;
    int                 vnum;
    int			progtypes;
    int			resetcount;	/* Lam */
    int                 count;
    int                 killed;
    unsigned int        sex;
    int                 level;
    int                 act;
    int                 affected_by[ AFF_FLAGS_SIZE ];
    int                 alignment;
    int                 race;
    int			zamoznosc;	/* Lam */
    int			grupa;		/* Lam */
    int			mod_size;	/* Lam */
};


struct zwod_data
{
    ZWOD_DATA *		next;
    CHAR_DATA *		ch;
};


/*
 * One character (PC or NPC).
 */
struct char_data
{
    CHAR_DATA *         next;
    CHAR_DATA *         next_in_room;
    CHAR_DATA *         master;
    ZWOD_DATA *		zwod_master;	/* Lam */
    CHAR_DATA *         leader;
    ZWOD_DATA *		zwod_leader;	/* Lam */
    CHAR_DATA *         reply;
    ZWOD_DATA *		zwod_reply;	/* Lam */
    CHAR_DATA *		retell;		/* Lam */
    ZWOD_DATA *		zwod_retell;	/* Lam */
    CHAR_DATA *         monitor;	/* Lam */
    ZWOD_DATA *		zwod_monitor;	/* Lam */
    CHAR_DATA *		hunting;	/* Lam */
    ZWOD_DATA *		zwod_hunting;	/* Lam */
    CHAR_DATA *		hating;		/* Lam */
    ZWOD_DATA *		zwod_hating;	/* Lam */
    CHAR_DATA *		fearing;	/* Lam */
    ZWOD_DATA *		zwod_fearing;	/* Lam */
    CHAR_DATA *		debugger;	/* Lam */
    ZWOD_DATA *		zwod_debugger;	/* Lam */
    FIGHT_DATA *        fighting;       /* Ulryk */
    SPEC_FUN *          spec_fun;
    MOB_INDEX_DATA *    pIndexData;
    AREA_DATA *		area;		/* Lam */
    DESCRIPTOR_DATA *   desc;
    AFFECT_DATA *       affected;
    ZWOD_DATA *		zwod_affected;	/* Lam */
    ALIAS_DATA *	alias;		/* Lam */
    TELL_DATA *		tell;		/* Lam */
    MPQUEST_DATA *	mpquest;	/* Lam */
    TYP_ZMIENNEJ *	zmienne;	/* Lam */
    ZWOD_DATA *		zwod_zmienne;	/* Lam */
    NOTE_DATA *         pnote;
    OBJ_DATA *          carrying;
    OBJ_DATA *          przechowalnia;	/* Lam */
    ROOM_INDEX_DATA *   in_room;
    ROOM_INDEX_DATA *	reset_room;	/* Lam */
    PC_DATA *           pcdata;
    OBJ_DATA *		pojazd;		/* Lam */
/*    MPROG_ACT_LIST *	mpact;
    int			mpactnum; */	/* Lam: act_proga nie ma */
    char *              name;
    char *              short_descr;
    char *              dopelniacz;	/* Lam */
    char *              celownik;	/* Lam */
    char *              biernik;	/* Lam */
    char *              narzednik;	/* Lam */
    char *              miejscownik;	/* Lam */
    char *              wolacz;		/* Lam */
    char *              long_descr;
    char *		long_descr_orig;/* Fuyara */
    char *		false_name;     /* disguise */
    char *              description;
    int                 race;
    int                 level;
    int                 trust;
    int                 played;
    time_t              logon;
    time_t              save_time;
    time_t              last_note;
    int                 timer;
    int                 wait;
    int                 hit;
    int                 max_hit;
    int			hit_remainder;	/* Lam */
    int                 mana;
    int                 max_mana;
    int			mana_remainder;	/* Lam */
    int                 move;
    int                 max_move;
    int			move_remainder;	/* Lam */
    int                 mod_str;	/* Lam 13.1.2004: mod_* z pc_data */
    int                 mod_int;
    int                 mod_wis;
    int                 mod_dex;
    int                 mod_con;
    int                 gold;
    int                 exp;
    int			qp;		/* Lam */
    int                 act;
    int                 affected_by[ AFF_FLAGS_SIZE ];
    int                 info;		/* Lam */
    int                 carry_weight;
    int                 carry_number;
    int                 saving_throw;
    int                 hitroll;
    int                 damroll;
    int                 armor;
    int                 marmor;		/* magiczna zbroja */
    int                 wimpy;
    int                 deaf;
    int			practice;
    int			alignment;
    unsigned int	sex;
    int			klasa;
    int			position;
    int			polskie;	/* Lam: standard polskich znakow */
    int			kolor;		/* Lam */
    int			podswietl;	/* Vigud */
    int			odmiana;	/* Lam */
    int			brzeczenie;
    char                separator;	/* Albi i Lam */
    char		reply_type;	/* Lam */
    char		retell_type;	/* Lam */
    bool                deleted;
};


/*
 * Data which only PC's have.
 */
struct pc_data
{
    PC_DATA	      * next;
    ZONE_DATA	      * strefa;		/* Lam */
    FILE	      * fLog;		/* Ulryk */
    ROOM_INDEX_DATA   * was_in_room;	/* przeniesione z char_data */
    ROOM_INDEX_DATA   * ic_in_room;	/* Lam */
    REFUSE_DATA	      * refuse;		/* Gimza */
    RESTART_DATA      * restart;	/* Lam */
    CLAN_DATA	      * clan;		/* Ulryk */
    CLAN_MEMBER_DATA  * clan_member;	/* Ulryk */
    struct
    {
	char   * pochodzenie;
	char   * strefa;
	int	 rasa;
	int	 ile;		/* liczba zwampirzen */
	int	 moc;		/* jak bardzo rozcienczona ma krew */
    }                   wampiryzm;
    char	      * pwd;
    char	      * bamfin;
    char	      * bamfout;
    char	      * immskll;
    char	      * title;
    char	      * prompt;
    char	      * fprompt;	/* Lam */
    char	      * stopien;	/* Lam */
    char	      * login;		/* Lam */
    char	      * last_login;	/* Lam */
    char	      * email;		/* Lam */
    RESTRYKCJE_DATA	love;		/* Qwert */
    RESTRYKCJE_DATA	silence;	/* Lam */
    RESTRYKCJE_DATA	noemote;	/* Lam */
    RESTRYKCJE_DATA	nocolour;	/* Lam */
    RESTRYKCJE_DATA	notell;		/* Lam */
    RESTRYKCJE_DATA	log;		/* Lam */
    RESTRYKCJE_DATA	freeze;		/* Lam */
    RESTRYKCJE_DATA	deny;
    char	      * thief_from;	/* Lam */
    char	      * killer_from;	/* Lam */
    char	      * zabojca_from;	/* Qwert */
    char	      * ignore;		/* Lam */
    char	      * pojedynek;	/* Lam */
    char	      * nienawisc;	/* Qwert */
    char	      * szpic_wybr;	/* Lam */
    char	      * szpic_wykl;	/* Lam */
    int			wiznet;		/* Lam */
    int                 killed;		/* Lam */
    int                 dead;		/* Lam */
    int			pkilled;	/* Lam */
    int			pdead;		/* Lam */
    int			samobojstwa;	/* Lam */
    int			wykrwawienia;	/* Lam */
    int                 perm_str;
    int                 perm_int;
    int                 perm_wis;
    int                 perm_dex;
    int                 perm_con;
    int                 condition       [ 3 ];
    int                 pagelen;
    int			bank;		/* Lam */
    int			hint;		/* Lam */
    int                 learned         [ MAX_SKILL ];
    int			used		[ MAX_SKILL ];	/* Lam */
    int			learned_level	[ MAX_SKILL ];	/* Lam */
    char		kolor		[ MIL ];	/* Lam */
    int			multi		[ 5 ];		/* Lam */
    int			temp_flags;	/* Lam */
    int			opcje;		/* Lam */
    int			wizinvis;	/* Lam */
    int                 niesmpoz;	/* Ulryk */
    int			ic_position;	/* Lam */
    int                 hp_angel;	/* Gimza */
    int			odwleczenie;	/* Lam */
    int			ost_end;	/* Tissarius */
    char	      * ostatnie[ MAX_OSTATNIE ];	/* Tissarius */
    bool                switched;
};

struct fight_data
{
    CHAR_DATA *vch;
    FIGHT_DATA *next;
};

/*
struct mob_prog_act_list
{
    MPROG_ACT_LIST * next;
    char *           buf;
    CHAR_DATA *      ch;
    OBJ_DATA *       obj;
    void *           vo;
};
*/

struct mob_trig_prog_data /* Lam */
{
    MTRIG_DATA	*next;
    CHAR_DATA	*ch;
    CHAR_DATA	*actor;
    CHAR_DATA	*vict;
    char	arg[ MIL ];
    int		time;
};

struct mob_prog_data
{
    MPROG_DATA *next;
    int		type;
    char *	arglist;
    char *	comlist;
    MPROG_DATA *parent; /* Lam 10.4.2002: do in_file_proga w kzapisz */
};

struct room_rand_prog_data /* Alandar 03.02.2004 */
{
    RRAND_DATA		*next;
    ROOM_INDEX_DATA	*room;
};

extern int prog_nest; /* mob_prog.c */

#define ERROR_PROG		 -1
#define IN_FILE_PROG		  0

/* #define M_ACT_PROG		b00 */
#define M_SPEECH_PROG		b01
#define M_RAND_PROG		b02
#define M_FIGHT_PROG		b03
#define M_DEATH_PROG		b04
#define M_HITPRCNT_PROG		b05
#define M_ENTRY_PROG		b06
#define M_GREET_PROG		b07
#define M_ALL_GREET_PROG	b08
#define M_GIVE_PROG		b09
#define M_BRIBE_PROG		b10
#define M_TRIG_PROG		b11
#define M_SOC_PROG		b12
#define M_TIME_PROG		b13
#define M_TELL_PROG		b14
#define M_SAY_PROG		b15
#define M_STEAL_PROG		b16
#define M_STEAL_FAIL_PROG	b17
#define M_ASLEEP_TIME_PROG	b18
#define M_DESCR_PROG		b19
#define M_FEED_PROG		b20 /* Alandar 21.12.2004: reaguje na karmienie */
#define M_WATER_PROG		b21 /* Alandar 21.12.2004: reaguje na pojenie */
#define M_SPELL_VICTIM_PROG	b22
#define M_LONG_DESCR_PROG	b23
#define M_AGGR_ATTACK_PROG	b24

#define R_SPEECH_PROG		b00
#define R_GREET_PROG		b01
#define R_LOOK_PROG		b02
#define R_DO_PROG		b03
#define R_DESCR_PROG		b04
#define R_DROP_PROG		b05
#define R_TIME_PROG		b06
#define R_RAND_PROG		b07
#define R_UNDUG_PROG		b08

#define E_PASS_PROG		b00
#define E_OPEN_PROG		b01
#define E_CLOSE_PROG		b02
#define E_LOCK_PROG		b03
#define E_UNLOCK_PROG		b04
#define E_PICK_PROG		b05
#define E_BASH_PROG		b06
#define E_DIG_PROG		b07
#define E_BOUNCE_PROG		b08

#define O_TAKE_PROG		b00
#define O_DROP_PROG		b01
#define O_WEAR_PROG		b02
#define O_REMOVE_PROG		b03
#define O_BUY_PROG		b04
#define O_SELL_PROG		b05
#define O_USE_PROG		b06
#define O_GIVE_PROG		b07
#define O_PUT_PROG		b08
#define O_DIG_PROG		b09
#define O_PUTIN_PROG		b10
#define O_SACR_PROG		b11
#define O_DONATE_PROG		b12
#define O_STEAL_PROG		b13
#define O_STEAL_FAIL_PROG	b14
#define O_PUTIN_GOLD_PROG	b15 /* Tissarius 22.10.2001 */
#define O_OPEN_PROG		b16 /* Alandar 21.12.2004: do pojemnikow */
#define O_CLOSE_PROG		b17 /* jw. */
#define O_DESCR_PROG		b18 /* Lam, #2169 */
#define O_LONG_DESCR_PROG	b19 /* jw. */

/* typ zmiennej */
struct typ_zmiennej
{
    TYP_ZMIENNEJ *nast;
    char nazwa[ 31 ];   /* max. 30 znakow */
    int typ;            /* 0 - int, 1 - char * */
    union
    {
	int i;        /* dla lancuchow wskaznik, dla liczb wartosc */
	void *p;
    } wartosc;
};


/* fragment stosu dla oblicz_wyrazenie */
struct stosmp
{
    STOSMP *nast;
    int typ; /* ujemny - typ, >= 0 - priorytet */
    int wart;
    void *wsk;
    char txt[ MIL + 4 ]; /* prowizorka, takie trzymaja sie najdluzej ;) */
};


/*
 * Liquids.
 */
struct liq_type
{
    char	       *liq_name;
    char               *liq_biernik;
    char               *liq_narzednik;
    char               *liq_color;
    int                 liq_affect [ 3 ];
};

/*
 * Alandar 22.11.2004: typy i wlasnosci jedzenia
 */
#define	PAPU_DOWOLNE	( PAPU_WSZYSTKO | PAPU_MACZNE | PAPU_SLODKIE \
			| PAPU_MIESNE | PAPU_RYBNE | PAPU_MLECZNE \
			| PAPU_ROSLINNE )
#define	PAPU_ZADNE	0
#define	PAPU_WSZYSTKO	b00
#define	PAPU_MACZNE	b01
#define	PAPU_SLODKIE	b02
#define	PAPU_MIESNE	b03
#define	PAPU_RYBNE	b04
#define	PAPU_MLECZNE	b05
#define	PAPU_ROSLINNE	b06

/*
 * Struktura informacji o drogach prowadzacych do roznych krain.
 */
struct koles_type
{
    char	*nazwa;
    char	*droga;
};

struct koles_data
{
    KOLES_DATA		*nast;
    int			 vnum;
    int			 droga;
    struct koles_type	 drogi[ MAX_TRACK ];
};

struct miodek_data
{
    char	*slowo;
    char	*zastepstwo;
};

/*
 * Extra description data for a room or object.
 */
struct extra_descr_data
{
    EXTRA_DESCR_DATA *next;        /* Next in list                     */
    char             *keyword;     /* Keyword in look/examine          */
    char             *description; /* What to see                      */
    bool              deleted;
};



/*
 * Prototype for an object.
 */
struct obj_index_data
{
    OBJ_INDEX_DATA *    next;
    OBJ_INDEX_DATA *	next_in_area;	/* Lam */
    AREA_DATA *		area;		/* Lam */
    AFFECT_DATA *       affected;
    MPROG_DATA *	mobprogs;
    int			progtypes;
    char *              name;
    char *              short_descr;
    char *              dopelniacz;
    char *              celownik;
    char *              biernik;
    char *              narzednik;
    char *              miejscownik;
    char *              description;
    char *		look_descr;	/* Lam: zamiast extra_descr */
    char *              action;
    char *		anti_class;	/* Lac 1.4.-7 */
    char *		anti_race;	/* Lac 1.4.-7 */
    int                 vnum;
    int                 item_type;
    int                 extra_flags[ ITEM_EXTRA_FLAGS_SIZE ];
    int                 wear_flags;
    int                 count;
    int                 weight;
    /*int                 cost;*/ /* Vigud */	/* Unused */
    int                 cena;			/* Lam: to dla sklepow */
    int                 poziom;			/* to tez */
    int			qp;			/* Lam 9.4.2000 */
    int                 rodzaj; /* 1 meski, 2 zenski, 3 niemeskoosobowy */
    /* Lam 7.11.2004: Lac musi sie kompilowac na systemach 64-bitowych, ale
       nie chce zmieniac int na int albo long long (z #ifami), zeby nie
       zmieniac wszystkich printfow w kodzie w zaleznosci od architektury. */
    union
    {
	int		v;
	void *		p;
    } value[ 6 ];				/* 4 --> 6, Lac 1.4.-7 */
    int			min_reset_level;	/* Lam 16.5.2000 */
    int			wielkosc;		/* Lam 22.11.2000 */
    bool		w_sklepie;		/* Lam 13.5.2005 */
};



/*
 * Przedmiot.
 */
struct obj_data
{
    OBJ_DATA *          next;
    OBJ_DATA *          next_content;
    OBJ_DATA *          contains;
    OBJ_DATA *          in_obj;
    CHAR_DATA *         carried_by;
    CHAR_DATA *         owned_by;	/* przechowalnia */
    AFFECT_DATA *       affected;
    OBJ_INDEX_DATA *    pIndexData;
    ROOM_INDEX_DATA *   in_room;
    char *              name;
    char *              short_descr;
    char *              dopelniacz;
    char *              celownik;
    char *              biernik;
    char *              narzednik;
    char *              miejscownik;
    char *              description;
    char *		look_descr;	/* Lam */
    char *              action;
    char *		anti_class;	/* Lam 3.5.2003: kopiowanie do przedmiotu */
    char *		anti_race;	/* jw. */
    char *              owner;          /* Ulryk 04.01.2005: przedmioty osobiste */
    int			item_type;
    int                 extra_flags[ ITEM_EXTRA_FLAGS_SIZE ];
    int                 wear_flags;
    int			wear_loc;
    int                 weight;
    int                 cost;
    int			plr_shop_cost;
    int                 level;
    int                 timer;
    int			rodzaj; /* 1 meski, 2 zenski, 3 niemeskoosobowy */
    union
    {
	int		v;
	void *		p;
    } value[ 6 ];
/*    int                 value   [ 6 ]; */
    bool                deleted;
};



/*
 * Exit data.
 */
struct exit_data
{
    EXIT_DATA *		next;		/* tylko do listy wolnych wyjsc */
    ROOM_INDEX_DATA *   to_room;
    int                 vnum;
    int                 exit_info;
    int                 key;
    int                 rodzaj; /* 0 nijaki, 1 meski, 2 zenski, 3 niemos. */
    char *              keyword;
    char *              short_descr;	/* ... jest zamkniety/a, sa zamkniete */
    char *              biernik;	/* otwierasz ... */
    char *              description;
    MPROG_DATA *	mobprogs;	/* eprogs Lam */
    int			progtypes;
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct reset_data
{
    RESET_DATA *        next;
    char                command;
    int                 arg1;
    int                 arg2;
    int                 arg3;
    char *		string;
};


/*
 * Lam 27.11.2000: wreszcie dlugo wyczekiwane strefy
 */
struct zone_data
{
    ZONE_DATA *next;
    char *nazwa;
    char *dopelniacz;
    char *celownik;
    char *biernik;
    char *narzednik;
    char *miejscownik;
    char *stolica;
    char *stol_dop;
    char *stol_cel;
    char *stol_bie;
    char *stol_narz;
    char *stol_mie;
    char *wladca;
    char *namiestnik;
    char *rasy;
    int przywolanie;
    int swiatynia;
    int szkola;
    int ile_krain;
    int ile_mobow;
    int ile_przedmiotow;
    int ile_pomieszczen;
};


struct zone_list
{
    ZONE_DATA *zone;
    ZONE_LIST *next;
};


/*
 * Area definition.
 */
struct area_data
{
    AREA_DATA *         next;
    RESET_DATA *        reset_first;
    RESET_DATA *        reset_last;
    TYP_ZMIENNEJ *	zmienne;	/* Lam */
    ZONE_LIST *		strefy;		/* Lam */
    ROOM_INDEX_DATA *	first_room;	/* Lam */
    ROOM_INDEX_DATA *	last_room;	/* Lam */
    MOB_INDEX_DATA *	first_mob;	/* Lam */
    MOB_INDEX_DATA *	last_mob;	/* Lam */
    OBJ_INDEX_DATA *	first_obj;	/* Lam */
    OBJ_INDEX_DATA *	last_obj;	/* Lam */
    char *              name;
    char *		full_name;	/* Lam */
    char *		file_name;	/* Lam */
    char *		reset_msg;	/* Lam */
    char *		opis;		/* Lam */
    char *		revision;	/* Lam */
    int			version;	/* Lam */
    int                 recall;
    int                 age;
    int                 nplayer;
    int			numofrooms;	/* Lam */
    int			numofmobs;	/* Lam */
    int			numofobjs;	/* Lam */
    int                 lowroomvnum;    /* Lam */
    int                 highroomvnum;   /* Lam */
    int                 lowmobvnum;     /* Lam */
    int                 highmobvnum;    /* Lam */
    int                 lowobjvnum;     /* Lam */
    int                 highobjvnum;    /* Lam */
    int			lowmoblevel;	/* Lam */
    int			highmoblevel;	/* Lam */
    int			lowgroupvnum;	/* Lam */
    int			highgroupvnum;	/* Lam */
    int			flags;		/* Lam */
    TIME_INFO_DATA	time_info;	/* Lam */
    WEATHER_DATA	weather_info;	/* Lam */
    bool		odnowiona;	/* Lam */
};

#define AREA_NOT_LISTED b00
#define AREA_NO_TELEPORT b01


/*
 * Room type.
 */
struct room_index_data
{
    ROOM_INDEX_DATA *   next;
    ROOM_INDEX_DATA *	next_in_area;	/* Lam */
    ROOM_INDEX_DATA *   next_track;
    ROOM_INDEX_DATA *   track_came_from;
    CHAR_DATA *         people;
    OBJ_DATA *          contents;
    EXTRA_DESCR_DATA *  extra_descr;
    AREA_DATA *		area;
    DEATHTRAP_DATA *	trap;		/* Lam */
    EXIT_DATA *         exit[ 10 ];	/* Lam (bylo 6) */
    MPROG_DATA *	mobprogs;	/* Lam */
    AFFECT_DATA *       affected;
    int			progtypes;	/* Lam */
    char *              name;
    char *		miejscownik;	/* Lam */
    char *              description;
    char *		anti_race;	/* Lam */
    char *		anti_class;	/* Lam */
    int                 vnum; /* Vigud: FIXME: to powinno byc unsigned */
    int                 room_flags;
    /* ponizsze do zapisywania w pliku krainy bez wplywow np. strefa ciszy */
    int                 original_room_flags; /* Lam */
    /* pojemnosc zamiast obecnych w Envy flag SOLITARY i PRIVATE */
    int			pojemnosc;	/* Lam */
    int			strata_ruchu;	/* Lam */
    int                 light;
    int                 sector_type;
};


/*
 * Smiertelne pulapki, ktore mozna umieszczac w pomieszczeniach.
 * Lam 22.5.99
 * Zauwaz, ze moze byc tylko jedna pulapka w pomieszczeniu.
 */
struct deathtrap_data
{
    int			trap_type;
    char *		trap_string;
};


/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED		  -1
#define TYPE_UNDEF_ZZ		  -2 /* jak UNDEFINED, ale liczy zwykly zgon */
#define TYPE_HIT		1000

/* argument typ_zgonu dla raw_kill */
#define ZGON_ZWYKLY		   0
#define ZGON_GRACZ		   1
#define ZGON_SAMOBOJSTWO	   2
#define ZGON_WYKRWAWIENIE	   3


/*
 * Ulryk: flagi umiejetnosci i czarow
 */
#define SKILL_NORMAL		   0
#define SKILL_NO_DISPEL		 b00
#define SKILL_FRIENDLY_DISPEL	 b01
#define SKILL_INVISIBLE		 b02
#define SKILL_KUPOWALNE		 b03
#define SKILL_NPC		 b04 /* Lam */
#define SKILL_PSI_SELF		 b05 /* Lam */
#define SKILL_DEF_VICT		 b06 /* Lam */
#define SKILL_NO_COST		 b07 /* Lam */


/*
 *  Target types.
 */
#define TAR_IGNORE                  0
#define TAR_CHAR_OFFENSIVE          1
#define TAR_CHAR_DEFENSIVE          2
#define TAR_CHAR_SELF               3
#define TAR_OBJ_INV                 4


/*
 * Usage types.
 */
#define USAGE_CAST		 0 /* do_cast, zwykle czarowanie */
#define USAGE_HEALER		 1 /* do_buy, kupowanie czarow */
#define USAGE_WAND		 2 /* do_zap, wyzwalanie rozdzek */
#define USAGE_STAFF		 3 /* do_brandish, wyzwalanie bulaw i podobnych */
#define USAGE_PILL		 4 /* do_eat, jedzenie pigulek */
#define USAGE_POTION		 5 /* do_drink (stare do_quaff), picie mikstur */
#define USAGE_SCROLL		 6 /* do_recite, recytacja pergaminow */
#define USAGE_SHIELD		 7 /* damage, odpowiadanie tarcz na ataki */
#define USAGE_WEAPON		 8 /* damage, magiczne bronie */
#define USAGE_BREATH		 9 /* pc_breathe, specjalnosci graczy */
#define USAGE_SPECIAL		10 /* spec_*, specjalnosci mobow */
#define USAGE_RECUR		11 /* rekurencja, czar jako dodatek drugiego */
#define USAGE_GAME		12 /* wygrana w kole fortuny */


/*
 * Czary/umiejetnosci. Spolszczone: Lam i Grzechu 10.9.98
 */
struct skill_type
{
    char *	pl_name;
    char *	short_name;
    char *      old_name;		   /* Name of skill               */
    int         skill_level [ MAX_CLASS ]; /* Level needed by class       */
    char *	needed;                    /* Lam: wymagana umiejetnosc   */
    int		multi_level [ MAX_CLASS ]; /* Lam: poziom dla multi-profesji */
    SPELL_FUN * spell_fun;                 /* Spell pointer (for spells)  */
    int         target;                    /* Legal targets               */
    int         minimum_position;          /* Position for caster / user  */
    int *       pgsn;                      /* Pointer to associated gsn   */
    int         min_mana;                  /* Minimum mana used           */
    int         beats;                     /* Waiting time after use      */
    int		to_practice;		   /* Lam: ile mozna to cwiczyc   */
    int		to_use;			   /* Lam: ile trzeba uzyc do nauki */
    int 	rodzaj;			   /* Lam: do komunikatow przy walce */
    char *      noun_damage;               /* Damage message              */
    char *      msg_off;                   /* Wear off message            */
    char *      msg_off_others;		   /* Lam 20.11.2000 na wzor EOS  */
    WEAR_OFF_FUN *wear_off_fun;		   /* Lam 1.4.2006		  */
    int         flags;                     /* Ulryk 24.09.2003            */
};


/* Lam 26.10.2004 */
struct sorted_skill_data
{
    SORTED_SKILL_DATA *next;
    int sn;
};


/*
 * These are skill_lookup return values for common skills and spells.
 */
extern  int     gsn_backstab;
extern  int     gsn_berserk;                    /* by Thelonius */
extern	int	gsn_blokowanie_tarcza;		/* Lam */
extern	int	gsn_cause_light;
extern  int     gsn_change;                     /* Lam */
extern  int     gsn_circle;                     /* by Thelonius */
extern  int     gsn_cone_of_silence;
extern  int	gsn_cushion;			/* Lam */
extern  int     gsn_breathe_water;              /* by Thelonius */
extern  int     gsn_burning_hands;
extern  int     gsn_disarm;
extern  int     gsn_dodge;
extern  int	gsn_enhanced_whois;		/* Lam */
extern  int     gsn_fast_flee;			/* Lam */
extern  int     gsn_fast_healing;		/* Lam */
extern  int     gsn_fetch;			/* Lam */
extern  int     gsn_flamestrike;
extern  int     gsn_fourth_attack;		/* Lam */
extern  int     gsn_haggle;			/* Lam */
extern  int     gsn_hide;
extern  int     gsn_hunt;			/* Lam - z Wurma */
extern	int	gsn_kontrola_nad_woda;		/* Lam */
extern	int	gsn_kumulacja_mocy;		/* Qwert */
extern	int	gsn_krwawe_lzy;			/* Lam */
extern  int     gsn_longscan;			/* Lam */
extern  int     gsn_meditation;			/* Lam */
extern  int     gsn_no_trip;			/* Lam */
extern	int	gsn_overhear;			/* Lam */
extern	int	gsn_pajeczyna;
extern  int     gsn_peek;
extern  int     gsn_pick_lock;
extern	int	gsn_plywanie;			/* Lam */
extern	int	gsn_podglad;			/* Lam */
extern  int     gsn_poison_weapon;              /* by Thelonius */
extern  int     gsn_profesjonalizm;
extern  int     gsn_scrolls;                    /* by Binky / Thelonius */
extern  int     gsn_snare;                      /* by Binky / Thelonius */
extern  int     gsn_sneak;
extern  int     gsn_staves;                     /* by Binky / Thelonius */
extern  int     gsn_steal;
extern  int     gsn_strong_wield;		/* Lam */
extern	int	gsn_taranowanie;		/* Lam */
extern	int	gsn_telepathy;			/* Lam */
extern  int     gsn_untangle;                   /* by Thelonius */
extern  int     gsn_wands;                      /* by Binky / Thelonius */

extern  int     gsn_bash;
extern  int     gsn_dual;                       /* by Thelonius */
extern  int     gsn_enhanced_damage;
extern  int     gsn_kick;
extern  int     gsn_parry;
extern  int     gsn_rescue;
extern  int     gsn_second_attack;
extern  int     gsn_third_attack;
extern  int     gsn_fury;                       /* Qwert */

extern  int     gsn_blindness;
extern  int     gsn_dazzle;
extern  int     gsn_charm_person;
extern  int     gsn_curse;
extern  int     gsn_invis;
extern  int     gsn_mass_invis;
extern  int     gsn_mute;                       /* by Thelonius */
extern  int     gsn_poison;
extern  int     gsn_sleep;
extern  int     gsn_polymorph_other;
extern  int     gsn_turn_undead;
extern  int     gsn_rozpalanie_ognisk;           /* Ulryk */
extern	int	gsn_blokada_mentalna;		 /* Ulryk */


/*
 * Psionicist gsn's (by Thelonius).
 */
extern  int     gsn_chameleon;
extern  int     gsn_domination;
extern  int     gsn_heighten;
extern  int     gsn_shadow;


extern  int     gsn_stake;

/*
 * Race gsn's (by Kahn).
 */
extern	int	gsn_vampiric_bite;
extern	int	gsn_postac_nietoperza;

/*
 * Alandar: spadanie
 */
extern int	gsn_uderzenie_w;
extern int	gsn_uderzenie_o;
extern int	gsn_upadek;

/*
 * Fuyara: skrytobojca
 */
extern	int	gsn_anatomy;
extern	int	gsn_appraise;
extern	int	gsn_assasinate;
extern	int	gsn_assemble;
extern	int	gsn_awareness;
extern	int	gsn_concentrate;
extern	int	gsn_critical_hit;
extern	int	gsn_cumulate_strength;
extern	int	gsn_death_tech;
extern	int	gsn_disguise;
extern	int	gsn_dragon_tech;
extern	int	gsn_enhanced_kick;
extern	int	gsn_flash;
extern	int	gsn_high_efficiency;
extern	int	gsn_initiation;
extern	int	gsn_jump;
extern	int	gsn_mighty_hands;
extern	int	gsn_power_tech;
extern	int	gsn_pull;
extern	int	gsn_reinforce_blade;
extern	int	gsn_runes;
extern	int	gsn_rune_flare;
extern	int	gsn_rune_fire;
extern	int	gsn_rune_ice;
extern	int	gsn_rune_poison;
extern	int	gsn_search;
extern	int	gsn_speed_tech;
extern	int	gsn_strike;
extern	int	gsn_stun;
extern	int	gsn_whirl;
extern	int	gsn_wraith_morph;
extern	int	gsn_walka_na_oslep;

/*
 * Qwert: mnich
 */

extern  int     gsn_aura_pokoju;
extern  int     gsn_duchowe_wsparcie;
extern  int     gsn_bio_przyspieszenie;
extern  int     gsn_przeniesienie_sily;
extern  int     gsn_ostrze_ognia;
extern  int     gsn_ostrze_chaosu;
extern  int     gsn_ostrze_lodu;
extern  int     gsn_przeniesienie_umyslu;
extern  int     gsn_lodowa_tarcza;
extern  int     gsn_lodowy_kolec;
/*extern  int     gsn_powiekszenie_mocy;*/
extern  int     gsn_dezorientacja;
extern  int     gsn_przeklenstwo_natury;
extern  int     gsn_przyspieszenie;
extern  int     gsn_spowolnienie;
extern  int     gsn_zakazenie;
/*extern  int     gsn_rytual_smierci;*/
extern  int     gsn_kukla;
/*extern  int     gsn_przekazanie_przedmiotu;*/
extern  int     gsn_regeneracja;
extern  int     gsn_degeneracja;
/*extern  int     gsn_wywolanie_strachu;*/
extern  int     gsn_hipnoza;
extern  int     gsn_rewers;
/*extern  int     gsn_przekazanie_bolu;*/
extern  int     gsn_uwolnienie_umyslow;
extern  int     gsn_znamie_smierci;

/*
 * Ulryk: nekromanta
 */
extern  int     gsn_porazajacy_dotyk;
extern  int     gsn_balsamowanie;
extern  int     gsn_sprowadzenie_szkieletu;
extern  int     gsn_mroczny_plomien;
extern  int     gsn_kontrola_umyslu;
extern  int     gsn_wskrzeszenie;
extern  int     gsn_pocalunek_smierci;
extern  int     gsn_eksplozja;
extern  int     gsn_stworzenie_mumii;

/* Lam 5.11.2004: zeby nie uzywac skill_lookup w special.c */
extern	int	gsn_acid_breath;
extern	int	gsn_armor;
extern	int	gsn_bless;
extern	int	gsn_combat_mind;
extern	int	gsn_cure_light;
extern	int	gsn_fire_breath;
extern	int	gsn_frost_breath;
extern	int	gsn_gas_breath;
extern	int	gsn_lightning_breath;
extern	int	gsn_mana_gift;
extern	int	gsn_refresh;
extern	int	gsn_shield;
extern	int	gsn_teleport;

/*
 * Gimza: paladyn
 */
extern	int	gsn_aniol_stroz;
extern	int	gsn_rozblysk;
extern	int	gsn_zaglada;
extern	int	gsn_swiety_msciciel;


/*
 * Lam 29.1.2004: zapamietane rasy
 */
extern	int	zr_bog;
extern	int	zr_byk;
extern	int	zr_harpia;
extern	int	zr_jednorozec;
extern	int	zr_jez;
extern	int	zr_martwiak;
extern	int	zr_minotaur;
extern	int	zr_niedzwiedz;
extern	int	zr_nietoperz;
extern	int	zr_pajeczak;
extern	int	zr_papuga;
extern	int	zr_przedmiot;
extern	int	zr_shambler;
extern	int	zr_smok;
extern	int	zr_troll;
extern	int	zr_wampir;
extern	int	zr_waz;


/*
 * Zamiast zdefiniowanych vnumow, sa czytane z pliku.
 * Dzialanie podobne do gsn'ow
 * Lam 23.3.99
 */
extern int      MOB_VNUM_MUMIA;         /* Ulryk */
extern int      MOB_VNUM_ZOMBIE;        /* Ulryk */
extern int	MOB_VNUM_SZKIELET;	/* Ulryk */
extern int	MOB_VNUM_KUKLA;		/* Qwert */
extern int	MOB_VNUM_CITYGUARD;
extern int	MOB_VNUM_VAMPIRE;
extern int	MOB_VNUM_ULT;
extern int	MOB_VNUM_SECRETARY;
extern int	MOB_VNUM_MIDGAARD_MAYOR;
extern int	MOB_VNUM_BANKIER;
/* extern int	MOB_VNUM_KOLES; - teraz definiowane w drogi.txt */
extern int	MOB_VNUM_PRZECHOWALNIA;
extern int	MOB_VNUM_WARSZTAT;
extern int	MOB_VNUM_SUPERMOB;
extern int	OBJ_VNUM_MONEY_ONE;
extern int	OBJ_VNUM_MONEY_SOME;
extern int	OBJ_VNUM_CORPSE_NPC;
extern int	OBJ_VNUM_CORPSE_PC;
extern int	OBJ_VNUM_FLAKI;
extern int	OBJ_VNUM_GLOWA;
extern int	OBJ_VNUM_REKA;
extern int	OBJ_VNUM_NOGA;
extern int	OBJ_VNUM_SERCE;
extern int	OBJ_VNUM_OGON;
extern int	OBJ_VNUM_SKRZYDLO;
extern int	OBJ_VNUM_JEZYK;
extern int	OBJ_VNUM_UCHO;
extern int	OBJ_VNUM_LAPA;
extern int	OBJ_VNUM_LEB;
extern int	OBJ_VNUM_ZAB;
extern int	OBJ_VNUM_KIEL;
extern int	OBJ_VNUM_ROG;
extern int	OBJ_VNUM_SZPON;
extern int	OBJ_VNUM_PLETWA;
extern int	OBJ_VNUM_MUSHROOM;
extern int	OBJ_VNUM_LIGHT_BALL;
extern int	OBJ_VNUM_SPRING;
extern int	OBJ_VNUM_EGG;
extern int	OBJ_VNUM_PORTAL;
extern int	OBJ_VNUM_BRAMA;
extern int	OBJ_VNUM_DROGOWSKAZ;
extern int      OBJ_VNUM_CHRUST;
extern int	OBJ_VNUM_SCHOOL_MACE;
extern int	OBJ_VNUM_SCHOOL_DAGGER;
extern int	OBJ_VNUM_SCHOOL_SWORD;
extern int	OBJ_VNUM_SCHOOL_VEST;
extern int	OBJ_VNUM_SCHOOL_SHIELD;
/* extern int	OBJ_VNUM_SCHOOL_BANNER; */
extern int	OBJ_VNUM_SCHOOL_CLUB;
extern int	OBJ_VNUM_BLACK_POWDER;
extern int	OBJ_VNUM_STAKE;
extern int	OBJ_VNUM_KREW;
extern int	OBJ_VNUM_KALUZA_KRWI;
extern int	OBJ_VNUM_PALAD_MLOT;
extern int	OBJ_VNUM_PALAD_MIECZ;
extern int	OBJ_VNUM_SILVER_POWDER;
extern int	OBJ_VNUM_JEDZ_MIN;
extern int	OBJ_VNUM_JEDZ_MAX;
extern int	ROOM_VNUM_LIMBO;
extern int	ROOM_VNUM_CHAT;
extern int	ROOM_VNUM_TEMPLE;
extern int	ROOM_VNUM_ALTAR;
extern int	ROOM_VNUM_SCHOOL;
extern int	ROOM_VNUM_GRAVEYARD_A;
extern int	ROOM_VNUM_PURGATORY_A;
extern int	ROOM_VNUM_POZA_SWIATEM;
/* extern int	ROOM_VNUM_PUNKT; - calkowicie niepotrzebne */


/*
 * Utility macros.
 */
#define UMIN( a, b )            ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define UMAX( a, b )            ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define URANGE( a, b, c )       ( ( b ) < ( a ) ? ( a )                       \
						: ( ( b ) > ( c ) ? ( c )     \
								  : ( b ) ) )
#define ABSOLUTE( a )		( ( a ) > 0 ? ( a ) : 0 - ( a ) )
#define LOWER( c )              ( ( c ) >= 'A' && ( c ) <= 'Z'                \
						? ( c ) + 'a' - 'A' : ( c ) )
#define UPPER( c )              ( ( c ) >= 'a' && ( c ) <= 'z'                \
						? ( c ) + 'A' - 'a' : ( c ) )
#define IS_SET( flag, bit )     ( ( flag ) &   ( bit ) )
#define SET_BIT( var, bit )     ( ( var )  |=  ( bit ) )
#define REMOVE_BIT( var, bit )  ( ( var )  &= ~( bit ) )

/*
 * Wektory bitow, wersja Lama, powstala w wyniku burzy mozgow z Ulrykiem
 * trwajacej pol nocy.
 *
 * Nasza implementacja tym sie rozni od innych, ktore widzielismy, ze umozliwia
 * rozna wielkosc kazdego z wektorow - jeden wektor moze miec wielkosc 1 dopoki
 * nie ma potrzeby jego rozszerzenia, inny 2, a inny 3. Porownaj to ze
 * Smaugiem, gdzie kazdy wektor zajmuje 16 bajtow, a kiedy chocby w jednym
 * skonczy sie miejsce, trzeba powiekszac wszystkie.
 */
#define IS_SET_V( flag, bit ) \
	( ( flag[ ( bit ) >> MOVE_BITS ] >> ( ( bit ) & INT_MASK ) ) & 1 )
/* chcialbym COMPILER_ASSERT( ( bit ) <= sizeof( flag ) * BITS_PER_INT, za_duza_wartosc_flagi_dla_SET_BIT_V );
   ale wtedy bit nie moze byc zmienna, a musi stala (typu AFF_BLABLA)
   gcc z -Wunreachable-code bezblednie wykrywa te linie, gdzie nie ma potrzeby
   sprawdzania, ale czy mozna wykryc te, gdzie wlasnie jest? ;) */
#define SET_BIT_V( flag, bit ) \
	do {  if ( ( bit ) <= sizeof( flag ) * BITS_PER_INT ) \
	    flag[ ( bit ) >> MOVE_BITS ] |= ( (unsigned) 1 << ( ( bit ) & INT_MASK ) ); \
	    else bug( "SET_BIT_V: ogromny bit", 0 ); \
	} while ( 0 )
#define REMOVE_BIT_V( flag, bit ) \
	do {  if ( ( bit ) <= sizeof( flag ) * BITS_PER_INT ) \
	    flag[ ( bit ) >> MOVE_BITS ] &= ~( 1 << ( ( bit ) & INT_MASK ) ); \
	    else bug( "SET_BIT_V: ogromny bit", 0 ); \
	} while ( 0 )
#define CLEAR_BITS( flag ) \
	do {	unsigned int i; \
		for ( i = 0; i < ( sizeof( flag ) / SIZEOF_INT ); i++ ) \
			flag[ i ] = 0; } while ( 0 )
#define COPY_BITS( to, from ) \
	do {	unsigned int i; \
		COMPILER_ASSERT( sizeof( from ) == sizeof( to ), niezgodna_wielkosc_flag_w_COPY_BITS ); \
		for ( i = 0; i < ( sizeof( from ) / SIZEOF_INT ); i++ ) \
			to[ i ] = from[ i ]; } while ( 0 )
#define OR_BITS( to, from ) \
	do {	unsigned int i; \
		COMPILER_ASSERT( sizeof( from ) == sizeof( to ), niezgodna_wielkosc_flag_w_OR_BITS ); \
		for ( i = 0; i < ( sizeof( from ) / SIZEOF_INT ); i++ ) \
			to[ i ] |= from[ i ]; } while ( 0 )
#define AND_BITS( to, from ) \
	do {	unsigned int i; \
		COMPILER_ASSERT( sizeof( from ) == sizeof( to ), niezgodna_wielkosc_flag_w_AND_BITS ); \
		for ( i = 0; i < ( sizeof( from ) / SIZEOF_INT ); i++ ) \
			to[ i ] &= from[ i ]; } while ( 0 )
#define AND_NOT_BITS( to, from ) \
	do {	unsigned int i; \
		COMPILER_ASSERT( sizeof( from ) == sizeof( to ), niezgodna_wielkosc_flag_w_AND_NOT_BITS ); \
		for ( i = 0; i < ( sizeof( from ) / SIZEOF_INT ); i++ ) \
			to[ i ] &= ~from[ i ]; } while ( 0 )
/* nie sprawdza, czy nie ma roznicy wielkosci :( FIXME: zrobic funkcje */
#define SAME_BITS( a, b ) ( !memcmp( a, b, sizeof( a ) ) )
#define fread_vector( flag, file, stat ) \
	do { CLEAR_BITS( flag ); \
	real_fread_vector( flag, sizeof( flag ) / SIZEOF_INT, file, stat ); \
	} while ( 0 )
#define rozpisz_flagi_v( flag, czy ) \
	rozpisz_flagi_v_fun( flag, sizeof( flag ) * 8, czy )

/*
 * Teoretycznie jest mozliwe, ze miedzy fclose a fopen wstrzeli sie np.
 * connect( ) z welcome( ) w innym watku, co jest mozliwe przy jakims porzadnym
 * "pakietowaniu", a kolejne fopen( ) nie otworzy fpReserve/fpBugReserve.
 * Zabezpieczam sie przed tym do pewnego stopnia - i tak jesli plik nie zostal
 * poprzednio pomyslnie otwarty i nie wyjdzie fclose, funkcje korzystajace
 * z tego moga miec problem z wlasnym fopen( ). No ale bez tego byla mozliwosc
 * wywalenia Laca doprowadzajac do dwukrotnego fclose( fpReserve ).
 */
#define zamknij_reserve( fp ) \
	do {	if ( fp ) \
		{ \
			fclose( fp ); \
			fp = NULL; \
		} } while ( 0 )

/*
 * Makra dla splaszczenia systemu monetarnego.
 */
#define ZYSK( poziom, mnoznik )		\
			( ( przelicznik_zysku[ URANGE( 0, poziom, 111 ) ] \
						* mnoznik ) >> 10 )
#define WYDATEK( poziom, mnoznik )	\
			( ( przelicznik_wydatkow[ URANGE( 0, poziom, 111 ) ] \
						* mnoznik ) >> 10 )


/*
 * Character macros.
 */
#define NIEUDANE( kto ) STC( "Nie uda`lo ci si`e.\n\r", kto )
#define NIEUDANE_BC( kto ) do { if IS_AFFECTED( kto, AFF_DETECT_MAGIC ) \
			STC( "To niczego nie da.\n\r", kto ); \
		   else STC( "Nie uda`lo ci si`e.\n\r", kto ); } while ( 0 )
#define WZMOCNIONY( kto, sn, warunek ) do { if ( warunek ) \
		{ ch_printf( ch, "Zwi`ekszasz moc czaru \"%s\".\n\r", \
				skill_table[ sn ].pl_name ); \
		return TRUE; } } while ( 0 )
#define DAMAGEBEZWALKI( ch, victim, usage ) \
				 ( ch == victim \
				&& ( usage == USAGE_PILL \
				  || usage == USAGE_POTION ) )

#define IS_NPC( ch )            ( IS_SET( ( ch )->act, ACT_IS_NPC ) )
#define IS_IMMORTAL( ch )       ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
#define IS_HERO( ch )           ( get_trust( ch ) >= LEVEL_HERO )
#define IS_AFFECTED( ch, bit )	( IS_SET_V( ( ch )->affected_by, ( bit ) ) )

#define IS_GOOD( ch )           ( ch->alignment >=  350 )
#define IS_EVIL( ch )           ( ch->alignment <= -350 )
#define IS_NEUTRAL( ch )        ( !IS_GOOD( ch ) && !IS_EVIL( ch ) )

#define IS_AWAKE( ch )          ( ch->position > POS_SLEEPING )
#define ZDOLNY_DO_WALKI( ch )	( ( ch )->position > POS_STUNNED )
#define GET_AC( ch )            ( UMAX( 0, ch->armor ) )
#define GET_MAC( ch )		( UMAX( 0, ch->marmor ) )
#define GET_ST( ch )		( UMAX( 0, ch->saving_throw ) )
/* Ulryk */
#define LICZ_BU( ch, wield )	( zetnij( interpolate( ch->level, 40, 55 ),  \
					interpolate( ch->level, 75, 100 ),   \
					get_hitroll( ch, wield ) ) )
#define LICZ_BO( ch, wield )	( zetnij( interpolate( ch->level, 20, 200 ), \
					interpolate( ch->level, 10, 100 ),   \
					get_damroll( ch, wield ) ) )
#define LICZ_OPC( ch )		( ( zetnij( 50, 33, GET_ST( ch ) )           \
				+ ( IS_NPC( ch )                             \
				 || class_table[ ch->klasa ].fMana ) )       \
				? wis_app[ get_curr_wis( ch ) ].opc : 0 )
#define LICZ_MINUS_TT( ch )	( ( zetnij( 34, 100, GET_AC( ch ) )          \
				  + zetnij( 34,                              \
				    str_app[ get_curr_str( ch ) ].carry / 10,\
				    ch->carry_weight ) ) / 2 )
#define LICZ_WYPAROWANIE_KP( ch, dam )                                       \
				( dam - zetnij( 2 * dam / 3, 500,            \
				GET_MAC( ch ) + 2 * GET_AC( ch ) ) )
#define LICZ_WYPAROWANIE_OPC( ch, dam )                                      \
				( dam - zetnij( 3 * dam / 4, 50,             \
				  LICZ_OPC( ch ) ) )

#define IS_OUTSIDE( ch )        ( !IS_SET(                                   \
				    ( ch )->in_room->room_flags,             \
				    ROOM_INDOORS ) )
#define IS_HASTE( ch, czas )	( IS_AFFECTED( ch, AFF_PRZYSPIESZENIE )    \
				  ? ( ( czas ) / 2 ) : 0 )
#define IS_SLOW( ch, czas )	( IS_AFFECTED( ch, AFF_SPOWOLNIENIE )      \
				  ? ( ( czas ) / 2 ) : 0 )

/* Lam: do { } while ( 0 ) bez srednika, zeby mozna bylo uzywac jak funkcji */
#define WAIT_STATE( ch, pulse ) do { ( ch )->wait = UMAX( ( ch )->wait,      \
						      ( pulse -		     \
						IS_HASTE( ch, pulse ) +      \
						IS_SLOW( ch, pulse ) ) );    \
				PRZERWIJ_KOPANIE( ch, TRUE ); } while ( 0 )
#define PRZERWIJ_KOPANIE( ch, czy_widac )				     \
	{ if ( find_timer( ch, finish_digging ) )      			     \
	  { if ( czy_widac )						     \
	      send_to_char( "{RPrzerywasz przekopywanie pod`lo`za.{x\n\r", ch ); \
	    del_timer( find_timer( ch, finish_digging ) ); } }
#define PRZERWIJ_KARMPOJ( xch, czy_widac )	{ TIMER_DATA *timer;	\
	for ( timer = timer_list; timer; timer = timer->next )		\
	    if ( ( timer->fun == przestan_karmic 			\
		|| timer->fun == przestan_poic )			\
	      && ( ( CHAR_DATA * ) timer->arg[ 0 ].p == xch		\
		|| timer->ch == xch ) )					\
	    {	del_timer( timer );					\
		if ( timer->ch == xch && czy_widac )			\
		    timer->fun( timer ); } }


/* Lam: moby maja 0 dla kazdego czaru */
#define MANA_COST( ch, sn )     ( IS_NPC( ch ) ? 0 \
				: UMAX ( skill_table[ sn ].min_mana, \
				    100 / ( 2 + UMAX ( 0, ch->level - \
				    ch->pcdata->learned_level[ sn ] ) ) ) \
				  * ( IS_AFFECTED( ch, AFF_ZABURZENIE_ENERGII ) \
				      ? 2 : 1 ) \
				  / ( ch->pcdata->learned[ gsn_profesjonalizm ] \
				      ? 2 : 1 ) )

/*
 * Lam, zeby nie powtarzac, dla czarow SKILL_NO_COST.
 *
 * Uwaga! Czary SKILL_NO_COST MUSZA uzywac RZUCENIE_CZARU( ) ZANIM nastapia
 * jakiekolwiek ich efekty! Oprocz tego, ze najpierw musi sie wywolac
 * say_spell( ) z tego makra, to opoznienie_czaru( ) zawiera nauke
 * profesjonalizmu.
 */
#define RZUCENIE_CZARU( ch, sn, usage ) do { \
    if ( usage == USAGE_CAST ) \
    {	ch->mana -= MANA_COST( ch, sn ); \
	opoznienie_czaru( ch, sn ); \
	say_spell( ch, sn ); \
    } } while ( 0 )

#define SPELL_LEVEL( ch, level )( ( ch ) && !IS_NPC( ch ) && ( level ) >= L_DIR \
					? ( level ) \
					: URANGE( 1, ( level ), LEVEL_HERO ) )

#define IS_SWITCHED( ch )	( ch->pcdata->switched )

#define CZY_UTF_8( ch )		( ch->polskie == POLSKIE_UTF_8 )

#define STREFA( ch )		( IS_NPC( ch ) \
				? ch->area->strefy \
				  ? ch->area->strefy->zone \
				  : NULL \
				: ch->pcdata->strefa )
#define POCHODZENIE( ch )	( IS_NPC( ch ) \
				? ( ch->area->strefy \
				  ? ch->area->strefy->zone->dopelniacz \
				  : "(brak strefy krainy!)" ) \
				: ( ch->pcdata->strefa \
				  ? ch->pcdata->strefa->dopelniacz \
				  : "(brak strefy gracza)" ) )
/* do progow, wszedzie indziej uzywany dopelniacz */
#define POCHODZENIE_M( ch )	( IS_NPC( ch ) \
				? ( ch->area->strefy \
				  ? ch->area->strefy->zone->nazwa \
				  : "(brak)" ) \
				: ( ch->pcdata->strefa \
				  ? ch->pcdata->strefa->nazwa \
				  : "(brak)" ) )
#define PRZYWOLANIE( ch )	( ch->in_room->area->recall \
				? ch->in_room->area->recall \
				: ( IS_NPC( ch ) \
				  ? ( ch->area->strefy \
				    ? ch->area->strefy->zone->przywolanie \
				    : ROOM_VNUM_TEMPLE ) \
				  : ( ch->pcdata->strefa \
				    ? ch->pcdata->strefa->przywolanie \
				    : ROOM_VNUM_TEMPLE ) ) )
/* do progowego recall( ), bez uwzgledniania krainy */
#define PRZYWOLANIE_P( ch )	( IS_NPC( ch ) \
				  ? ( ch->area->strefy \
				    ? ch->area->strefy->zone->przywolanie \
				    : ROOM_VNUM_TEMPLE ) \
				  : ( ch->pcdata->strefa \
				    ? ch->pcdata->strefa->przywolanie \
				    : ROOM_VNUM_TEMPLE ) )
/* do progowego temple( ) */
#define SWIATYNIA( ch )		( IS_NPC( ch ) \
				  ? ( ch->area->strefy \
				    ? ch->area->strefy->zone->swiatynia \
				    : ROOM_VNUM_TEMPLE ) \
				  : ( ch->pcdata->strefa \
				    ? ch->pcdata->strefa->swiatynia \
				    : ROOM_VNUM_TEMPLE ) )

/* czy jest w postaci? */
#define WP( ch )		 ( ch->in_room \
				&& ch->in_room->vnum != ROOM_VNUM_POZA_SWIATEM )
/* czy jest poza postacia? */
#define PP( ch )		( !WP( ch ) )

#define CZY_WAMPIR( ch )	 ( ch->race == zr_wampir \
				|| IS_AFFECTED( ch, AFF_POSTAC_NIETOPERZA ) )

/* Tyche, via Vigud: Digital Mars was unable to expand the PERS macros when the
   FIGHTING macro was passed to it. */
#if defined( __DMC__ )
#define FIGHTING( ch )		fighting_func( ch )
#else
#define FIGHTING( ch )		( ch->fighting ? ch->fighting->vch : NULL )
#endif
/* brak tlenu jesli:
  - jest pod woda i nie ma oddychania woda
  - nie jest ani pod woda, ani w wodzie, a nie ma oddychania powietrzem
    (bo wszedzie poza woda jest u nas powietrze)
  kazdy inny przypadek - umie pobierac tlen (na wodzie ma wybor nosnika) */
#define CZY_BRAK_TLENU_W_POM( ch, room ) \
	       ( !IS_IMMORTAL( ch ) \
	      && ( ( room->sector_type == SECT_UNDERWATER \
		  && !IS_AFFECTED( ch, AFF_GILLS ) \
		  && !IS_SET( race_table[ ch->race ].oddychanie, ODD_WODA ) ) \
		|| ( !IS_SET( race_table[ ch->race ].oddychanie, ODD_POWIETRZEM ) \
		  && !CZY_WODA( room ) ) ) )
#define CZY_BRAK_TLENU( ch ) CZY_BRAK_TLENU_W_POM( ch, ch->in_room )
/* przy zmianach w CZY_PALACE_SLONCE zaktualizowac wykrywacz w reset_area! */
#define CZY_PALACE_SLONCE_W_POM( ch, room ) \
	       ( CZY_WAMPIR( ch ) \
	      && ( room->area->weather_info.sunlight == SUN_RISE \
		|| room->area->weather_info.sunlight == SUN_LIGHT ) \
	      && !IS_SET( room->room_flags, ROOM_UNDERGROUND ) \
	      && room->sector_type != SECT_UNDERWATER \
	      && !IS_IMMORTAL( ch ) )
#define CZY_PALACE_SLONCE( ch ) CZY_PALACE_SLONCE_W_POM( ch, ch->in_room )
#define IS_MUTE( ch ) \
		 ( WP( ch ) \
		&& ( IS_AFFECTED( ch, AFF_MUTE ) \
		  || IS_SET( race_table[ ch->race ].race_abilities, RACE_MUTE ) \
		  || CZY_BRAK_TLENU( ch ) ) )
#define CAN_TALK( ch ) \
		 ( !IS_MUTE( ch ) \
		&& ( !ch->in_room \
		  || !IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) ) )
#define CAN_HEAR_ROOM( ch, room ) \
		 ( IS_AWAKE( ch ) \
		&& !IS_SET( room->room_flags, ROOM_CONE_OF_SILENCE ) \
		&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_BEZ_SLUCHU ) )
#define CAN_HEAR( ch )	CAN_HEAR_ROOM( ch, ch->in_room )
#define CAN_SMELL( ch ) \
		 ( IS_AWAKE( ch ) && !CZY_BRAK_TLENU( ch ) \
		&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_BEZ_WECHU ) )
#define HAS_INFRAVISION( ch ) \
		 ( IS_AFFECTED( ch, AFF_INFRARED ) \
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_INFRAVISION ) )
/* zakladamy, ze rasy ze skrzydlami lataja dzieki nim, czyli jesli postac nie
   ma wplywu "latanie", nie moze spac (chwilowo moze odpoczywac), bo spadnie */
#define IS_FLYING( ch ) \
		 ( IS_AFFECTED( ch, AFF_FLYING ) \
		|| ( IS_SET( race_table[ ch->race ].poruszanie, POR_POWIETRZE ) \
		  && ( !IS_SET( race_table[ ch->race ].parts, P_SKRZYDLA ) \
		    || IS_AWAKE( ch ) ) ) )
#define HAS_DETECT_HIDDEN( ch ) \
		 ( IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) \
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_HIDDEN ) )
#define HAS_DETECT_INVIS( ch ) \
		 ( IS_AFFECTED( ch, AFF_DETECT_INVIS ) \
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_INVIS ) )
#define HAS_DETECT_EVIL( ch ) \
		 ( IS_AFFECTED( ch, AFF_DETECT_EVIL ) \
		|| ( IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_EVIL ) \
		  && IS_GOOD( ch ) ) \
		|| ( ch->klasa == CLASS_PALADYN ) )
#define IS_PASSING( ch ) \
		 ( IS_AFFECTED( ch, AFF_PASS_DOOR ) \
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_PASSDOOR ) )
#define HAS_PROTECTION( ch ) \
		 ( IS_AFFECTED( ch, AFF_PROTECT ) \
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_PROTECTION ) )
#define HAS_PROTECT_GOOD( ch ) \
		 ( IS_AFFECTED( ch, AFF_TARCZA_ZLA ) \
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_PROTECT_GOOD ) )
#define HAS_SANCTUARY( ch ) \
		 ( IS_AFFECTED( ch, AFF_SANCTUARY ) \
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_SANCT ) )
#define ROZMIAR( ch ) \
		 ( ( !IS_NPC( ch ) && ch->race == zr_wampir \
		  && ch->pcdata->wampiryzm.rasa != -1 ) \
		   ? race_table[ ch->pcdata->wampiryzm.rasa ].size \
		   : ( !IS_NPC( ch ) \
		     ? race_table[ ch->race ].size \
		     : race_table[ ch->race ].size + ch->pIndexData->mod_size ) )

#define CZY_MA_TWARZ( ch ) \
		( !IS_SET( race_table[ ch->race ].wearlocs, WRL_TWARZ ) )

/*
 * Object macros.
 */
#define CAN_WEAR( obj, part )   ( IS_SET( ( obj )->wear_flags, ( part ) ) )
#define IS_OBJ_STAT( obj, stat )( IS_SET_V( ( obj )->extra_flags, ( stat ) ) )



/*
 * Description macros.
 */
/*
#define PERS( ch, looker )      ( can_see( looker, ( ch ) ) ?                \
				( IS_NPC( ch ) ? ( ch )->short_descr         \
					       : ( ch )->name ) : "someone" )
*/
#define PERS( ch, looker, tel )  ( ( ( tel ) \
					? can_see_who( ( looker ), ( ch ) ) \
					: can_see( ( looker ), ( ch ) ) ) ? \
				   ( ch )->short_descr  : "kto`s" )
#define PERSD( ch, looker, tel ) ( ( ( tel ) \
					? can_see_who( ( looker ), ( ch ) ) \
					: can_see( ( looker ), ( ch ) ) ) ? \
				   ( ch )->dopelniacz   : "kogo`s" )
#define PERSC( ch, looker, tel ) ( ( ( tel ) \
					? can_see_who( ( looker ), ( ch ) ) \
					: can_see( ( looker ), ( ch ) ) ) ? \
				   ( ch )->celownik     : "komu`s" )
#define PERSB( ch, looker, tel ) ( ( ( tel ) \
					? can_see_who( ( looker ), ( ch ) ) \
					: can_see( ( looker ), ( ch ) ) ) ? \
				   ( ch )->biernik      : "kogo`s" )
#define PERSN( ch, looker, tel ) ( ( ( tel ) \
					? can_see_who( ( looker ), ( ch ) ) \
					: can_see( ( looker ), ( ch ) ) ) ? \
				   ( ch )->narzednik    : "kim`s" )
#define PERSM( ch, looker, tel ) ( ( ( tel ) \
					? can_see_who( ( looker ), ( ch ) ) \
					: can_see( ( looker ), ( ch ) ) ) ? \
				   ( ch )->miejscownik  : "kim`s" )
#define PERSW( ch, looker, tel ) ( ( ( tel ) \
					? can_see_who( ( looker ), ( ch ) ) \
					: can_see( ( looker ), ( ch ) ) ) ? \
				   ( ch )->wolacz       : "ktosiu" )


/*
 * Structure for a command in the command lookup table.
 */
struct cmd_type
{
    char * const	name;
    char * const	old_name; /* Lam: Lac 1.4.-9 */
    DO_FUN *		do_fun;
    int			position;
    int			level;
    int			log;
    int			restr; /* Lam */
};

struct prog_cmd_type
{
    char * const	name;
    PROG_FUN *		fun;
};

/*
 * Lam 6.11.2000
 */
struct pose_data
{
    char *to_char;
    char *to_room;
};


/*
 * Global constants. (const.c)
 */
extern	const	char *			tablica_kolorow[ 4 ][ MAX_COLOUR ];
extern	const	char *			polska_tablica[ 5 ][ 18 ];

extern	const	char *			day_name	[ ];
extern	const	char *			month_name	[ ];
extern	const	struct holiday_type	holiday_table	[ ];

extern  const   struct  str_app_type    str_app         [ 31 ];
extern  const   struct  int_app_type    int_app         [ 31 ];
extern  const   struct  wis_app_type    wis_app         [ 31 ];
extern  const   struct  dex_app_type    dex_app         [ 31 ];
extern  const   struct  con_app_type    con_app         [ 31 ];

extern  const   struct  class_type      class_table     [ MAX_CLASS   ];
extern  const   struct  cmd_type        cmd_table       [ ];
extern	const	int			dir_order	[ MAX_DIR ];
extern	const	struct	dir_type	kierunki	[ MAX_DIR ];
extern	const	struct	object_type	typy_przedmiotow[ ITEM_MAX    ];
extern  const   struct  liq_type        liq_table_pl    [ LIQ_MAX     ];
extern  const	char			*typy_podloza	[ ];
extern		struct	skill_type	skill_table	[ MAX_SKILL   ];
extern  char *  const                   title_table     [ MAX_CLASS   ]
							[ 2 ];
extern	const	struct	material_type	material_table	[ MAX_MATERIAL ];
/* extern	struct  koles_type      koles_table     [ MAX_TRACK ]; */
extern  const   struct  race_type       race_table      [ MAX_RACE ];
extern		struct	czesc_ciala	czesci_ciala	[ ];
extern  const   struct  html_page_type  html_page_table [ ];
extern  const   struct  struckdrunk     drunk           [ ];
extern	const	int			przelicznik_zysku[ 112 ];
extern	const	int			przelicznik_wydatkow[ 112 ];

extern	char *	const			lac_nowosci;

/*
 * Global variables. (db.c)
 */
extern		MOB_INDEX_DATA    *	mob_index_hash	[ MAX_KEY_HASH ];
extern		OBJ_INDEX_DATA    *	obj_index_hash	[ MAX_KEY_HASH ];
extern		ROOM_INDEX_DATA   *	room_index_hash	[ MAX_KEY_HASH ];

extern		ZONE_DATA	  *	zone_first;
extern		AREA_DATA	  *	area_first;
extern		AREA_DATA	  *	area_czysciec; /* Lam */
extern          HELP_DATA         *     help_first;
extern          SHOP_DATA         *     shop_first;
extern		HEALER_DATA	  *	healer_first;

extern		SCHEMAT_DATA	  *	schemat_list;
extern          BAN_DATA          *     ban_list;
extern          CHAR_DATA         *     char_list;
extern          DESCRIPTOR_DATA   *     descriptor_list;
extern          NOTE_DATA         *     note_list;
extern		CLAN_DATA	  *	clan_list;
extern          OBJ_DATA          *     object_list;
extern		MTRIG_DATA	  *	mtrig_list;
extern		AIROBJ_DATA	  *	airobj_list;
extern		AIRCHAR_DATA	  *	airchar_list;
extern		RRAND_DATA	  *	rrand_list;
extern		SORTED_SKILL_DATA *	sorted_skill_list;

extern          AFFECT_DATA       *     affect_free;
extern          BAN_DATA          *     ban_free;
extern          CHAR_DATA         *     first_free_char;
extern		CHAR_DATA	  *	last_free_char;
extern          DESCRIPTOR_DATA   *     descriptor_free;
extern          EXTRA_DESCR_DATA  *     extra_descr_free;
extern          NOTE_DATA         *     note_free;
extern		CLAN_DATA	  *	clan_free;
extern		CLAN_MEMBER_DATA  *	clan_member_free;
extern		CLAN_REL_DATA	  *	clan_rel_free;
extern          OBJ_DATA          *     obj_free;
extern          PC_DATA           *     pcdata_free;
extern          FIGHT_DATA        *     fight_free;
extern		ZWOD_DATA         *	zwod_free;
extern		int			top_fight;
extern		int			top_clan;
extern		int			top_clan_member;
extern		int			top_clan_rel;
extern		int			top_mprog;
extern		int			top_stosmp;
extern		int			top_zmienna;
extern		int			top_imiona;

extern          time_t                  current_time;
extern          bool                    fLogAll;
extern          FILE *                  fpReserve;
extern          FILE *                  fpBugReserve;
extern          KILL_DATA               kill_table      [ ];
extern		char			bug_buf		[ ];
extern		char			log_buf		[ ];
extern		char			posdead_buf	[ ];
extern		long			mud_age;
/* ponizsze trzymaja czas Midgaardu */
extern          TIME_INFO_DATA          time_info;
extern          WEATHER_DATA            weather_info;
extern          time_t                  down_time;
extern          time_t                  warning1;
extern          time_t                  warning2;
extern          bool                    Reboot;

extern		AUCTION_DATA *		auction;
extern		char *			help_greeting;
extern		char *			help_namehelp;

extern		bool			imud_down; /* Lam */
extern		char			lsc_init_str	[ MAX_INPUT_LENGTH ];
extern		char			lsc_delimiter;
extern		char			lsc_clear_str	[ MAX_INPUT_LENGTH ];
extern unsigned	int			lsc_init_str_len;

extern struct	stats_type		max_play;
extern		int			max_play_all_time;
extern		char			max_play_all_time_time [ 200 ];
extern struct 	stats_type		max_desc;
extern		int			max_desc_all_time;
extern		char			max_desc_all_time_time [ 200 ];

extern	char *				hint_table[ MAX_HINT ];
extern	char *				quit_table[ MAX_QUIT ];
extern	char *				offensive_table[ MAX_OFFENSIVE ];
extern	struct miodek_data		miodek_table[ MAX_OFFENSIVE ];
/*extern  char *			maskowanie_table[ MAX_MASKOWANIE ];
extern	int				maskowanie_count; */
extern	int				quit_count;
extern	struct pose_data		pose_table[ MAX_CLASS ][ MAX_POSE ];
extern	int				pose_count[ MAX_CLASS ];
extern	int				skill_count[ MAX_CLASS ];
extern	int				multi_count[ MAX_CLASS ];
extern	int				ilosc_czesci_ciala[ MAX_RACE ];
extern	struct	powody_data		powody[ MAX_POWODY ];
extern	IMIONA_DATA *			imiona[ MAX_DLUG_IMIENIA + 1 ][ 64 ];
extern	bool				ODPLUSKWIACZ;
extern	bool				PRZERABIACZ;
extern  char				HTTPD_log[ MIL ];
extern  FILE *				httpdlog;
struct progi_type
{
    char *name;
    int   trig;
};
extern struct progi_type progi[ 4 ][ 26 ];

/* act_info.c */
extern const int where_order[ ];

/* comm.c */
extern	bool	merc_down;
extern	int	numlock;
extern	bool	TylkoKonwersja;
extern	bool	wizlock;
extern	unsigned const char echo_on_str[ ];
extern	unsigned const char echo_off_str[ ];
extern	unsigned const char keepalive_str[ ];

/* handler.c */
extern	TIMER_DATA	*timer_list;
extern	int		wzmocnienie_affect_join;
extern	struct bitvector_field_data extra_bit_values[ ];
extern	struct bitvector_field_data affect_bit_values[ ];
extern	struct bit_field_data exit_info_flags[ ];
extern	struct bit_field_data mob_act_flags[ ];
extern	struct bit_field_data room_flags_table[ ];
extern	struct choice_field_data apply_values[ ];

/* magic.c */
extern char *target_name;

/* mob_prog.c */
extern bool descr_prog_in_progress;
extern char descr_prog_buffer[ 2 * MSL ];

/* special.c */
extern char spec_list[ MSL ];
extern struct spec_type spec_table[ ];

/* ssm.c */
extern unsigned int MAX_STRING;

/* update.c */
extern int pulse_point;
extern int pulse_point_max;


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN( do_advance      );
DECLARE_DO_FUN( do_aexits	);		/* Lam */
DECLARE_DO_FUN( do_affects	);		/* Lam */
DECLARE_DO_FUN( do_afk          );
DECLARE_DO_FUN( do_alias	);		/* Lam */
DECLARE_DO_FUN( do_allow        );
DECLARE_DO_FUN( do_answer       );
DECLARE_DO_FUN( do_appraise	);		/* Fuyara */
DECLARE_DO_FUN( do_areas        );
DECLARE_DO_FUN( do_asave	);		/* Lam */
DECLARE_DO_FUN( do_assemble	);		/* Fuyara */
DECLARE_DO_FUN( do_astat        );		/* Lam */
DECLARE_DO_FUN( do_at           );
DECLARE_DO_FUN( do_auction      );
DECLARE_DO_FUN( do_awareness	);		/* Fuyara */
DECLARE_DO_FUN( do_backstab     );
DECLARE_DO_FUN( do_balsamuj     );              /* dla nekromanty */
DECLARE_DO_FUN( do_bamfin       );
DECLARE_DO_FUN( do_bamfout      );
DECLARE_DO_FUN( do_ban		);		/* Lam */
DECLARE_DO_FUN( do_barwy	);		/* Lam */
DECLARE_DO_FUN( do_bash         );
DECLARE_DO_FUN( do_licz		);		/* Lam */
DECLARE_DO_FUN( do_beep         );
DECLARE_DO_FUN( do_berserk      );              /* by Thelonius */
DECLARE_DO_FUN( do_bezkoloru	);		/* Lam */
DECLARE_DO_FUN( do_bkukla	);		/* Vigud */
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN( do_brandish     );
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN( do_bug          );
DECLARE_DO_FUN( do_burry	);		/* Lam */
DECLARE_DO_FUN( do_buy          );
DECLARE_DO_FUN( do_cast         );
DECLARE_DO_FUN( do_cecho	);		/* Lam */
DECLARE_DO_FUN( do_chameleon    );              /* by Thelonius */
DECLARE_DO_FUN( do_change       );		/* Lam */
DECLARE_DO_FUN( do_channels     );
DECLARE_DO_FUN( do_chat         );
DECLARE_DO_FUN( do_chrust       );              /* Ulryk */
DECLARE_DO_FUN( do_circle_skill );              /* by Thelonius * do_circle() istnieje w bibliotece Allegro, wiec zmienilem tu */
DECLARE_DO_FUN( do_clan         );		/* Lam */
DECLARE_DO_FUN( do_clan_chat	);		/* Lam */
DECLARE_DO_FUN( do_clear	);		/* Lam */
DECLARE_DO_FUN( do_close        );
DECLARE_DO_FUN( do_colour       );      /* Colour Command By Lope */
DECLARE_DO_FUN( do_commands     );
DECLARE_DO_FUN( do_compare      );
DECLARE_DO_FUN( do_concentrate	);		/* Fuyara */
DECLARE_DO_FUN( do_config       );
DECLARE_DO_FUN( do_consider     );
DECLARE_DO_FUN( do_count	);		/* Lam */
DECLARE_DO_FUN( do_credits      );
DECLARE_DO_FUN( do_cumulate_strength );		/* Fuyara */
DECLARE_DO_FUN( do_czytaj	);		/* Lam */
DECLARE_DO_FUN( do_death_tech	);		/* Fuyara */
DECLARE_DO_FUN( do_delet	);		/* Lam */
DECLARE_DO_FUN( do_delete	);		/* Lam */
DECLARE_DO_FUN( do_delete_disabled );		/* Lam */
DECLARE_DO_FUN( do_deny         );
DECLARE_DO_FUN( do_description  );
DECLARE_DO_FUN( do_dig		);		/* Lam */
DECLARE_DO_FUN( do_disarm       );
DECLARE_DO_FUN( do_disconnect   );
DECLARE_DO_FUN( do_disguise	);		/* Fuyara */
DECLARE_DO_FUN( do_donate       );
DECLARE_DO_FUN( do_dostosuj     );
DECLARE_DO_FUN( do_down         );
DECLARE_DO_FUN( do_dragon_tech	);		/* Fuyara */
DECLARE_DO_FUN( do_drink        );
DECLARE_DO_FUN( do_drop         );
DECLARE_DO_FUN( do_east         );
DECLARE_DO_FUN( do_eat          );
DECLARE_DO_FUN( do_echo         );
DECLARE_DO_FUN( do_email        );              /* Lam */
DECLARE_DO_FUN( do_emote        );
DECLARE_DO_FUN( do_enter        );              /* Lam */
DECLARE_DO_FUN( do_epstat	);		/* Lam */
DECLARE_DO_FUN( do_equipment    );
DECLARE_DO_FUN( do_examine      );
DECLARE_DO_FUN( do_exits        );
DECLARE_DO_FUN( do_exp		);		/* Lam */
DECLARE_DO_FUN( do_fee          );
DECLARE_DO_FUN( do_feed         );
DECLARE_DO_FUN( do_fill         );
DECLARE_DO_FUN( do_flame        );              /* Lam */
DECLARE_DO_FUN( do_flash	);		/* Fuyara */
DECLARE_DO_FUN( do_flee         );
DECLARE_DO_FUN( do_follow       );
DECLARE_DO_FUN( do_force        );
DECLARE_DO_FUN( do_fprompt	);		/* Lam */
DECLARE_DO_FUN( do_freeze       );
DECLARE_DO_FUN( do_frombank     );		/* Lam */
DECLARE_DO_FUN( do_fstat	);		/* Lam */
DECLARE_DO_FUN( do_fury		);		/* Qwert */
DECLARE_DO_FUN( do_get          );
DECLARE_DO_FUN( do_give         );
DECLARE_DO_FUN( do_gladuj	);		/* Lam */
DECLARE_DO_FUN( do_godtalk	);		/* Lam */
DECLARE_DO_FUN( do_goto         );
DECLARE_DO_FUN( do_graj		);		/* Lam */
DECLARE_DO_FUN( do_grats        );              /* Lam */
DECLARE_DO_FUN( do_group        );
DECLARE_DO_FUN( do_gtell        );
DECLARE_DO_FUN( do_hasl		);		/* Lam */
DECLARE_DO_FUN( do_haslo	);		/* Lam */
DECLARE_DO_FUN( do_heighten     );              /* Thelonius */
DECLARE_DO_FUN( do_help         );
DECLARE_DO_FUN( do_hide         );
DECLARE_DO_FUN( do_hold		);		/* Lam */
DECLARE_DO_FUN( do_holylight    );
DECLARE_DO_FUN( do_hunt		);		/* z Wurma */
DECLARE_DO_FUN( do_idea         );
DECLARE_DO_FUN( do_ignore	);		/* Lam */
DECLARE_DO_FUN( do_imctl	);		/* Lam */
DECLARE_DO_FUN( do_immcmd	);		/* Lam */
DECLARE_DO_FUN( do_immtalk      );
DECLARE_DO_FUN( do_imtlset      );
DECLARE_DO_FUN( do_imud		);		/* Lam */
DECLARE_DO_FUN( do_info		);		/* Lam */
DECLARE_DO_FUN( do_inventory    );
DECLARE_DO_FUN( do_invis        );
DECLARE_DO_FUN( do_kick         );
DECLARE_DO_FUN( do_kill         );
DECLARE_DO_FUN( do_laguj	);		/* Qwert */
DECLARE_DO_FUN( do_lecnowy	);		/* Lam */
DECLARE_DO_FUN( do_list         );
DECLARE_DO_FUN( do_lock         );
DECLARE_DO_FUN( do_log          );
DECLARE_DO_FUN( do_look         );
DECLARE_DO_FUN( do_love         );              /* Qwert */
DECLARE_DO_FUN( do_memory       );
DECLARE_DO_FUN( do_mistat	);		/* Alucard */
DECLARE_DO_FUN( do_mfind        );
DECLARE_DO_FUN( do_miw		);		/* Lam */
DECLARE_DO_FUN( do_mklon	);		/* Winchester */
DECLARE_DO_FUN( do_mlista	);		/* Lam */
DECLARE_DO_FUN( do_mload        );
DECLARE_DO_FUN( do_monitor      );              /* Lam */
DECLARE_DO_FUN( do_mowdo	);		/* Lam */
DECLARE_DO_FUN( do_mpodpluskw	);		/* Lam */
DECLARE_DO_FUN( do_mpstat       );
DECLARE_DO_FUN( do_mset         );
DECLARE_DO_FUN( do_mstat        );
DECLARE_DO_FUN( do_murder       );
DECLARE_DO_FUN( do_music        );
DECLARE_DO_FUN( do_mwhere       );
DECLARE_DO_FUN( do_nakarm	);		/* Alandar */
DECLARE_DO_FUN( do_napoj	);		/* Alandar */
DECLARE_DO_FUN( do_nchat        );              /* Lam */
DECLARE_DO_FUN( do_newlock      );
DECLARE_DO_FUN( do_nietoperz	);		/* Lam */
DECLARE_DO_FUN( do_noemote      );
DECLARE_DO_FUN( do_north	);
DECLARE_DO_FUN( do_northeast	);		/* Lam */
DECLARE_DO_FUN( do_northwest	);		/* Lam */
DECLARE_DO_FUN( do_note         );
DECLARE_DO_FUN( do_notell       );
DECLARE_DO_FUN( do_nowosci	);		/* Lam */
DECLARE_DO_FUN( do_numlock      );
DECLARE_DO_FUN( do_oaff		);		/* Lam */
DECLARE_DO_FUN( do_odbierz	);		/* Lam */
DECLARE_DO_FUN( do_odmiana	);
DECLARE_DO_FUN( odmiana_postaci	);		/* Lam */
DECLARE_DO_FUN( do_odwampirz    );
DECLARE_DO_FUN( do_ofind        );
DECLARE_DO_FUN( do_oglos        );
DECLARE_DO_FUN( do_ognisko      );
DECLARE_DO_FUN( do_oklon	);		/* Winchester */
DECLARE_DO_FUN( do_oload        );
DECLARE_DO_FUN( do_open         );
DECLARE_DO_FUN( do_opstat	);		/* Lam */
DECLARE_DO_FUN( do_order        );
DECLARE_DO_FUN( do_oset         );
DECLARE_DO_FUN( do_ostat        );
DECLARE_DO_FUN( do_ostatnie     );		/* Tissarius */
DECLARE_DO_FUN( do_outfit	);		/* Lam */
DECLARE_DO_FUN( do_owhere       );
DECLARE_DO_FUN( do_pagelen      );
DECLARE_DO_FUN( do_pardon       );
DECLARE_DO_FUN( do_peace        );
DECLARE_DO_FUN( do_pick         );
DECLARE_DO_FUN( do_plista	);
DECLARE_DO_FUN( do_podgladaj	);		/* Lam */
DECLARE_DO_FUN( do_podswietlaj	);		/* Vigud */
DECLARE_DO_FUN( do_poison_weapon);		/* by Thelonius */
DECLARE_DO_FUN( do_pojedynek	);		/* Lam */
DECLARE_DO_FUN( do_polskie	);		/* Lam */
DECLARE_DO_FUN( do_pomlista	);
DECLARE_DO_FUN( do_pomszukaj	);		/* Lam */
DECLARE_DO_FUN( do_pose         );
DECLARE_DO_FUN( do_postaw	);		/* Thelonius, nowa: Lam */
DECLARE_DO_FUN( do_power_tech	);		/* Fuyara */
DECLARE_DO_FUN( do_poziomy	);		/* Lam */
DECLARE_DO_FUN( do_practice     );
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN( do_przechowaj   );		/* Lam */
DECLARE_DO_FUN( do_przeladuj	);		/* Lam */
DECLARE_DO_FUN( do_pull		);		/* Fuyara */
DECLARE_DO_FUN( do_punktacja	);		/* Lam */
DECLARE_DO_FUN( do_purge        );
DECLARE_DO_FUN( do_push		);		/* Lam */
DECLARE_DO_FUN( do_put          );
DECLARE_DO_FUN( do_quest	);		/* Lam/Aldar/Quad/Winchester */
DECLARE_DO_FUN( do_qui          );
DECLARE_DO_FUN( do_quit         );
DECLARE_DO_FUN( do_wciel	);		/* Lam */
DECLARE_DO_FUN( do_wymus	);		/* Lam */
DECLARE_DO_FUN( do_qwertalk	);		/* Lam */
DECLARE_DO_FUN( do_racestat	);		/* Lam */
DECLARE_DO_FUN( do_realias	);		/* Lam */
DECLARE_DO_FUN( do_reboo        );
DECLARE_DO_FUN( do_reboot       );
DECLARE_DO_FUN( do_recall	);	DECLARE_DO_FUN( real_recall	);
DECLARE_DO_FUN( do_recho        );
DECLARE_DO_FUN( do_recite       );
DECLARE_DO_FUN( do_refuse       );		/* Gimza */
DECLARE_DO_FUN( do_register     );
DECLARE_DO_FUN( do_reinforce_blade);		/* Fuyara */
DECLARE_DO_FUN( do_rekord	);		/* Lam */
DECLARE_DO_FUN( do_remove       );
DECLARE_DO_FUN( do_rename	);		/* EA, sciagnal Lam */
DECLARE_DO_FUN( do_rent         );
DECLARE_DO_FUN( do_reply        );
DECLARE_DO_FUN( do_report       );
DECLARE_DO_FUN( do_rescue       );
DECLARE_DO_FUN( do_rest         );
DECLARE_DO_FUN( do_restar	);		/* Lam */
DECLARE_DO_FUN( do_restart	);		/* Lam */
DECLARE_DO_FUN( do_restore      );
DECLARE_DO_FUN( do_retell	);		/* Lam */
DECLARE_DO_FUN( do_return       );
DECLARE_DO_FUN( do_rhist	);		/* Lam */
DECLARE_DO_FUN( do_rozblysk     );
DECLARE_DO_FUN( do_rpstat	);		/* Lam */
DECLARE_DO_FUN( do_rset         );
DECLARE_DO_FUN( do_rstat        );
DECLARE_DO_FUN( do_runes	);		/* Fuyara */
DECLARE_DO_FUN( do_sacrifice    );
DECLARE_DO_FUN( do_save         );
DECLARE_DO_FUN( do_say          );
DECLARE_DO_FUN( do_scan         );              /* Lam i Albi */
DECLARE_DO_FUN( do_search	);		/* Fuyara */
DECLARE_DO_FUN( do_sell         );
DECLARE_DO_FUN( do_separator    );              /* Lam i Albi */
DECLARE_DO_FUN( do_shadow       );              /* by Thelonius */
DECLARE_DO_FUN( do_shout        );
DECLARE_DO_FUN( do_shutdow      );
DECLARE_DO_FUN( do_shutdown     );
DECLARE_DO_FUN( do_silence      );
DECLARE_DO_FUN( do_sklep	);		/* Lam */
DECLARE_DO_FUN( do_sla          );
DECLARE_DO_FUN( do_slay         );
DECLARE_DO_FUN( do_sleep        );
DECLARE_DO_FUN( do_slist        );
DECLARE_DO_FUN( do_slookup      );
DECLARE_DO_FUN( do_snare        );              /* by Binky / Thelonius */
DECLARE_DO_FUN( do_sneak        );
DECLARE_DO_FUN( do_snoop        );
DECLARE_DO_FUN( do_socials      );
DECLARE_DO_FUN( do_south	);
DECLARE_DO_FUN( do_southeast	);		/* Lam */
DECLARE_DO_FUN( do_southwest	);		/* Lam */
DECLARE_DO_FUN( do_speed_tech	);		/* Fuyara */
DECLARE_DO_FUN( do_spells       );
DECLARE_DO_FUN( do_split        );
DECLARE_DO_FUN( do_sset         );
DECLARE_DO_FUN( do_sstime       );
DECLARE_DO_FUN( do_stake        );
DECLARE_DO_FUN( do_stand        );
DECLARE_DO_FUN( do_steal        );
DECLARE_DO_FUN( do_stopien	);		/* Lam */
DECLARE_DO_FUN( do_strike	);		/* Fuyara */
DECLARE_DO_FUN( do_stun		);		/* Fuyara */
DECLARE_DO_FUN( do_switch       );
DECLARE_DO_FUN( do_taranuj	);		/* Lam */
DECLARE_DO_FUN( do_tell         );
DECLARE_DO_FUN( do_kogobic	);		/* Vigud */
DECLARE_DO_FUN( do_test		);		/* Lam */
DECLARE_DO_FUN( do_time         );
DECLARE_DO_FUN( do_kalendarz	);		/* Lam */
DECLARE_DO_FUN( do_teleciap	);		/* Lam */
DECLARE_DO_FUN( do_teleecho	);		/* Lam */
DECLARE_DO_FUN( do_title        );
DECLARE_DO_FUN( do_tobank       );		/* Lam */
DECLARE_DO_FUN( do_train        );
DECLARE_DO_FUN( do_transfer     );
DECLARE_DO_FUN( do_trust        );
DECLARE_DO_FUN( do_typo         );
DECLARE_DO_FUN( do_ulista	);		/* Lam */
DECLARE_DO_FUN( do_unalias	);		/* Lam */
DECLARE_DO_FUN( do_unlock       );
DECLARE_DO_FUN( do_untangle     );              /* by Thelonius */
DECLARE_DO_FUN( do_up           );
DECLARE_DO_FUN( do_users        );
DECLARE_DO_FUN( do_uzyj		);		/* Lam */
DECLARE_DO_FUN( do_value        );
DECLARE_DO_FUN( do_version	);		/* Lam */
DECLARE_DO_FUN( do_visible      );
DECLARE_DO_FUN( do_vnumy	);		/* Lam */
DECLARE_DO_FUN( do_vomit	);		/* Lam */
DECLARE_DO_FUN( do_wake         );
DECLARE_DO_FUN( do_wear         );
DECLARE_DO_FUN( do_weather      );
DECLARE_DO_FUN( do_west         );
DECLARE_DO_FUN( do_where        );
DECLARE_DO_FUN( do_whirl	);		/* Fuyara */
DECLARE_DO_FUN( do_whisper	);		/* Lam */
DECLARE_DO_FUN( do_who          );
DECLARE_DO_FUN( do_whois        );
DECLARE_DO_FUN( do_wimpy        );
DECLARE_DO_FUN( do_wizhelp      );
DECLARE_DO_FUN( do_wizlock      );
DECLARE_DO_FUN( do_wiznet	); 		/* Lam */
DECLARE_DO_FUN( do_wraith_morph	);		/* Fuyara */
DECLARE_DO_FUN( do_wsiadz	);		/* Lam */
DECLARE_DO_FUN( do_wylej	);		/* Lam */
DECLARE_DO_FUN( do_wysiadz	);		/* Lam */
DECLARE_DO_FUN( do_yell         );
DECLARE_DO_FUN( do_zadstat      );              /* Ulryk */
DECLARE_DO_FUN( do_zadustaw     );              /* Ulryk */
DECLARE_DO_FUN( do_zap          );


/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(      spell_null              );
DECLARE_SPELL_FUN(      spell_acid_blast        );
DECLARE_SPELL_FUN(      spell_alchemy           );	/* Lam */
DECLARE_SPELL_FUN(      spell_armor             );
DECLARE_SPELL_FUN(	spell_black_aura	);	/* Lam */
DECLARE_SPELL_FUN(      spell_bless             );
DECLARE_SPELL_FUN(      spell_blindness         );
DECLARE_WEAR_OFF_FUN(	wear_off_blindness	);
DECLARE_SPELL_FUN(	spell_brama		);	/* Lam */
DECLARE_SPELL_FUN(      spell_burning_hands     );
DECLARE_SPELL_FUN(      spell_breathe_water     );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_call_lightning    );
DECLARE_SPELL_FUN(      spell_cause_critical    );
DECLARE_SPELL_FUN(      spell_cause_light       );
DECLARE_SPELL_FUN(      spell_cause_serious     );
DECLARE_SPELL_FUN(      spell_change_sex        );
DECLARE_SPELL_FUN(      spell_charm_person      );
DECLARE_SPELL_FUN(      spell_chill_touch       );
DECLARE_SPELL_FUN(      spell_colour_spray      );
DECLARE_SPELL_FUN(      spell_cone_of_silence   );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_continual_light   );
DECLARE_SPELL_FUN(      spell_control_weather   );
DECLARE_SPELL_FUN(      spell_create_food       );
DECLARE_SPELL_FUN(      spell_create_spring     );
DECLARE_SPELL_FUN(      spell_create_water      );
DECLARE_SPELL_FUN(      spell_cure_blindness    );
DECLARE_SPELL_FUN(      spell_cure_critical     );
DECLARE_SPELL_FUN(      spell_cure_light        );
DECLARE_SPELL_FUN(      spell_cure_poison       );
DECLARE_SPELL_FUN(      spell_cure_serious      );
DECLARE_SPELL_FUN(      spell_mana_gift         );	/* Lam */
DECLARE_SPELL_FUN(      spell_curse             );
DECLARE_SPELL_FUN(      spell_destroy_cursed    );
DECLARE_SPELL_FUN(      spell_detect_evil       );
DECLARE_WEAR_OFF_FUN(	wear_off_detect_evil	);
DECLARE_SPELL_FUN(      spell_detect_hidden     );
DECLARE_WEAR_OFF_FUN(	wear_off_detect_hidden	);
DECLARE_SPELL_FUN(      spell_detect_invis      );
DECLARE_WEAR_OFF_FUN(	wear_off_detect_invis	);
DECLARE_SPELL_FUN(      spell_detect_magic      );
DECLARE_SPELL_FUN(      spell_detect_poison     );
DECLARE_SPELL_FUN(      spell_dispel_evil       );
DECLARE_SPELL_FUN(      spell_dispel_magic      );      /* by Thelonius */
DECLARE_SPELL_FUN(	spell_dream		);	/* Lam */
DECLARE_SPELL_FUN(	spell_drogowskaz	);	/* Lam */
DECLARE_SPELL_FUN(      spell_earthquake        );
DECLARE_SPELL_FUN(      spell_enchant_weapon    );
DECLARE_SPELL_FUN(      spell_energy_drain      );
DECLARE_SPELL_FUN(      spell_exorcise          );
DECLARE_SPELL_FUN(      spell_faerie_fire       );
DECLARE_SPELL_FUN(      spell_faerie_fog        );
DECLARE_SPELL_FUN(      spell_finger_of_death   );	/* Lam */
DECLARE_SPELL_FUN(      spell_fireball          );
DECLARE_SPELL_FUN(      spell_flamestrike       );
DECLARE_SPELL_FUN(      spell_flaming           );
DECLARE_SPELL_FUN(      spell_fly               );
DECLARE_SPELL_FUN(      spell_gate              );
DECLARE_SPELL_FUN(      spell_giant_strength    );
DECLARE_SPELL_FUN(      spell_harm              );
DECLARE_SPELL_FUN(      spell_heal              );
DECLARE_SPELL_FUN(      spell_identify          );
DECLARE_SPELL_FUN(      spell_infravision       );
DECLARE_WEAR_OFF_FUN(	wear_off_infravision	);
DECLARE_SPELL_FUN(      spell_invis             );
DECLARE_SPELL_FUN(      spell_know_alignment    );
DECLARE_SPELL_FUN(	spell_kontrola_nad_woda	);	/* Lam */
DECLARE_SPELL_FUN(	spell_krwawe_lzy	);	/* Lam */
DECLARE_SPELL_FUN(	spell_laser_eyes	);	/* Lam */
DECLARE_SPELL_FUN(      spell_lightning_bolt    );
DECLARE_SPELL_FUN(      spell_locate_object     );
DECLARE_SPELL_FUN(      spell_magic_missile     );
DECLARE_SPELL_FUN(      spell_mass_heal         );
DECLARE_SPELL_FUN(      spell_mass_invis        );
DECLARE_SPELL_FUN(	spell_mass_protection	);	/* Lam */
DECLARE_SPELL_FUN(	spell_mass_refresh	);	/* Lam */
DECLARE_SPELL_FUN(	spell_mass_shield	);	/* Lam */
DECLARE_SPELL_FUN(	spell_moon_arrow	);	/* Lam */
DECLARE_SPELL_FUN(      spell_mute              );      /* by Thelonius */
DECLARE_WEAR_OFF_FUN(	wear_off_mute		);
DECLARE_SPELL_FUN(	spell_okno		);	/* Lam */
DECLARE_SPELL_FUN(	spell_pajeczyna		);	/* Lam */
DECLARE_SPELL_FUN(      spell_pass_door         );
DECLARE_SPELL_FUN(      spell_plasma_blast      );	/* Lam */
DECLARE_SPELL_FUN(      spell_poison            );
DECLARE_SPELL_FUN(      spell_polymorph_other   );
DECLARE_SPELL_FUN(      spell_portal            );	/* Lam */
DECLARE_SPELL_FUN(      spell_protection        );
DECLARE_SPELL_FUN(	spell_ray_of_truth	);	/* Lam */
DECLARE_SPELL_FUN(      spell_recharge_item     );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_refresh           );
DECLARE_SPELL_FUN(      spell_remove_alignment  );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_remove_curse      );
DECLARE_SPELL_FUN(	spell_remove_invis	);	/* Lam */
DECLARE_SPELL_FUN(      spell_remove_silence    );      /* by Thelonius */
DECLARE_SPELL_FUN(	spell_rytual_zycia	);	/* Lam */
DECLARE_SPELL_FUN(      spell_sanctuary         );
DECLARE_SPELL_FUN(      spell_shocking_grasp    );
DECLARE_SPELL_FUN(      spell_shield            );
DECLARE_SPELL_FUN(      spell_sleep             );
DECLARE_SPELL_FUN(      spell_stone_skin        );
DECLARE_SPELL_FUN(      spell_summon            );
DECLARE_SPELL_FUN(	spell_sunbeam		);	/* Lam */
DECLARE_SPELL_FUN(      spell_teleport          );
DECLARE_SPELL_FUN(	spell_teleport_group	);	/* Lam */
DECLARE_SPELL_FUN(      spell_turn_undead       );
DECLARE_SPELL_FUN(      spell_ventriloquate     );
DECLARE_SPELL_FUN(      spell_weaken            );
DECLARE_SPELL_FUN(      spell_word_of_recall    );
DECLARE_SPELL_FUN(      spell_acid_breath       );
DECLARE_SPELL_FUN(      spell_fire_breath       );
DECLARE_SPELL_FUN(      spell_frost_breath      );
DECLARE_SPELL_FUN(      spell_gas_breath        );
DECLARE_SPELL_FUN(      spell_lightning_breath  );
DECLARE_SPELL_FUN(      spell_kocie_ruchy       );
DECLARE_SPELL_FUN(      spell_iluzja            );
DECLARE_SPELL_FUN(	spell_zaburzenie_energii );
DECLARE_SPELL_FUN(	spell_blokada_mentalna	);
DECLARE_SPELL_FUN(	spell_odblokowanie_mentalne );
/* DECLARE_SPELL_FUN(	spell_vampiric_bite	); */

/*
 * Psi spell_functions, in magic.c (by Thelonius).
 */
DECLARE_SPELL_FUN(      spell_adrenaline_control);
DECLARE_SPELL_FUN(      spell_agitation         );
DECLARE_SPELL_FUN(      spell_awe               );
DECLARE_SPELL_FUN(      spell_ballistic_attack  );
DECLARE_SPELL_FUN(      spell_cell_adjustment   );
DECLARE_SPELL_FUN(      spell_combat_mind       );
DECLARE_SPELL_FUN(      spell_complete_healing  );
DECLARE_SPELL_FUN(      spell_control_flames    );
DECLARE_SPELL_FUN(      spell_death_field       );
DECLARE_SPELL_FUN(      spell_detonate          );
DECLARE_SPELL_FUN(      spell_disintegrate      );
DECLARE_SPELL_FUN(      spell_displacement      );
DECLARE_SPELL_FUN(      spell_domination        );
DECLARE_SPELL_FUN(      spell_ectoplasmic_form  );
DECLARE_SPELL_FUN(      spell_ego_whip          );
DECLARE_SPELL_FUN(      spell_energy_containment);
DECLARE_SPELL_FUN(      spell_enhance_armor     );
DECLARE_SPELL_FUN(      spell_enhanced_strength );
DECLARE_SPELL_FUN(      spell_flesh_armor       );
DECLARE_SPELL_FUN(      spell_inertial_barrier  );
DECLARE_SPELL_FUN(      spell_inflict_pain      );
DECLARE_SPELL_FUN(      spell_intellect_fortress);
DECLARE_SPELL_FUN(      spell_lend_health       );
DECLARE_SPELL_FUN(      spell_mental_barrier    );
DECLARE_SPELL_FUN(      spell_mind_thrust       );
DECLARE_SPELL_FUN(      spell_project_force     );
DECLARE_SPELL_FUN(      spell_psionic_blast     );
DECLARE_SPELL_FUN(      spell_psychic_crush     );
DECLARE_SPELL_FUN(      spell_psychic_drain     );
DECLARE_SPELL_FUN(      spell_psychic_healing   );
DECLARE_SPELL_FUN(      spell_share_strength    );
DECLARE_SPELL_FUN(      spell_thought_shield    );
DECLARE_SPELL_FUN(      spell_ultrablast        );
DECLARE_SPELL_FUN(	spell_bariera_zla	);

/*
 * Qwert
 */
DECLARE_SPELL_FUN(	spell_aura_pokoju	  );
DECLARE_SPELL_FUN(	spell_duchowe_wsparcie	  );
DECLARE_SPELL_FUN(	spell_bio_przyspieszenie  );
DECLARE_SPELL_FUN(	spell_przeniesienie_sily  );
DECLARE_SPELL_FUN(	spell_ostrze_lodu	  );
DECLARE_SPELL_FUN(	spell_ostrze_chaosu	  );
DECLARE_SPELL_FUN(	spell_ostrze_ognia	  );
DECLARE_SPELL_FUN(	spell_przeniesienie_umyslu);
DECLARE_SPELL_FUN(	spell_lodowa_tarcza	  );
DECLARE_SPELL_FUN(	spell_lodowy_kolec	  );
DECLARE_SPELL_FUN(	spell_sztorm_lodowy	  );
DECLARE_SPELL_FUN(	spell_przekonanie	  );
DECLARE_SPELL_FUN(	spell_dezorientacja	  );
DECLARE_SPELL_FUN(	spell_oczyszczenie	  );
DECLARE_SPELL_FUN(	spell_przeklenstwo_natury );
DECLARE_SPELL_FUN(	spell_przyspieszenie	  );
DECLARE_SPELL_FUN(	spell_splot_many	  );
DECLARE_SPELL_FUN(	spell_spowolnienie	  );
DECLARE_SPELL_FUN(	spell_zakazenie		  );
DECLARE_SPELL_FUN(	spell_rytual_smierci	  );
DECLARE_SPELL_FUN(	spell_sciana_ognia	  );
DECLARE_SPELL_FUN(	spell_kukla	          );
DECLARE_SPELL_FUN(	spell_przekazanie_przedmiotu);
DECLARE_SPELL_FUN(	spell_degeneracja	  );
DECLARE_SPELL_FUN(	spell_regeneracja	  );
DECLARE_SPELL_FUN(	spell_porazenie		  );
DECLARE_SPELL_FUN(	spell_wywolanie_strachu	  );
DECLARE_SPELL_FUN(	spell_hipnoza		  );
DECLARE_SPELL_FUN(	spell_rewers		  );
DECLARE_SPELL_FUN(	spell_przekazanie_bolu	  );
DECLARE_SPELL_FUN(	spell_uwolnienie_umyslow  );
DECLARE_SPELL_FUN(	spell_znamie_smierci	  );
DECLARE_SPELL_FUN(	spell_przejscie		  );
DECLARE_SPELL_FUN(	spell_wyzwolenie_mocy 	  );
DECLARE_SPELL_FUN(	spell_kumulacja_mocy	  );

/*
 * Ulryk: nekromanta
 */
DECLARE_SPELL_FUN(      spell_mroczny_znak        );
DECLARE_SPELL_FUN(      spell_sprowadzenie_szkieletu );
DECLARE_SPELL_FUN(      spell_odeslanie           );
DECLARE_SPELL_FUN(      spell_wejrzenie           );
DECLARE_SPELL_FUN(      spell_wzmocnienie_umyslu  );
DECLARE_SPELL_FUN(      spell_nekromantyczna_zbroja );
DECLARE_SPELL_FUN(      spell_mroczny_plomien     );
DECLARE_SPELL_FUN(      spell_kontrola_umyslu     );
DECLARE_SPELL_FUN(      spell_wyssanie_duszy      );
DECLARE_SPELL_FUN(      spell_wskrzeszenie        );
DECLARE_SPELL_FUN(      spell_pocalunek_smierci   );
DECLARE_SPELL_FUN(      spell_eksplozja           );
DECLARE_SPELL_FUN(      spell_kradziez_cienia     );
DECLARE_SPELL_FUN(      spell_aura_strachu        );
DECLARE_SPELL_FUN(      spell_strefa_cienia       );
DECLARE_WEAR_OFF_FUN(	wear_off_strefa_cienia    );
DECLARE_SPELL_FUN(      spell_stworzenie_mumii    );
DECLARE_SPELL_FUN(      spell_koszmar             );
DECLARE_SPELL_FUN(      spell_przeklety_deszcz    );
DECLARE_SPELL_FUN(      spell_tarcza_umarlych     );
DECLARE_SPELL_FUN(      spell_wycienczenie        );
DECLARE_SPELL_FUN(      spell_cienisty_pocisk     );
DECLARE_SPELL_FUN(	spell_tarcza_zla          );

/*
 * Gimza: paladyn
 */
DECLARE_SPELL_FUN(      spell_pozytywne_doladowanie );
DECLARE_SPELL_FUN(      spell_cud                 );
DECLARE_SPELL_FUN(      spell_nalozenie_rak       );
DECLARE_SPELL_FUN(      spell_tarcza_archaniolow  );
DECLARE_SPELL_FUN(      spell_sila_jednosci       );
DECLARE_SPELL_FUN(      spell_boski_pyl           );
DECLARE_SPELL_FUN(      spell_spadajace_gwiazdy   );
DECLARE_SPELL_FUN(      spell_aniol_stroz         );
DECLARE_SPELL_FUN(      spell_stworzenie_mlota    );
DECLARE_SPELL_FUN(      spell_niezlomna_wiara     );
DECLARE_SPELL_FUN(      spell_swieta_aura         );
DECLARE_SPELL_FUN(      spell_wrzaca_krew         );
DECLARE_SPELL_FUN(      spell_strazniczy_glif     );
DECLARE_SPELL_FUN(      spell_jednosc_obronna     );
DECLARE_SPELL_FUN(      spell_kradziez_zycia      );
DECLARE_SPELL_FUN(      spell_stworzenie_grabiezcy);
DECLARE_SPELL_FUN(      spell_jednosc_bojowa      );
DECLARE_SPELL_FUN(      spell_duchowa_zbroja      );



/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(	spec_breath_any		);
DECLARE_SPEC_FUN(	spec_breath_acid	);
DECLARE_SPEC_FUN(	spec_breath_fire	);
DECLARE_SPEC_FUN(	spec_breath_frost	);
DECLARE_SPEC_FUN(	spec_breath_gas		);
DECLARE_SPEC_FUN(	spec_breath_lightning	);
DECLARE_SPEC_FUN(	spec_cast_adept		);
DECLARE_SPEC_FUN(	spec_cast_cleric	);
DECLARE_SPEC_FUN(       spec_cast_ghost         );
DECLARE_SPEC_FUN(	spec_cast_mage		);
DECLARE_SPEC_FUN(       spec_cast_psionicist    );
DECLARE_SPEC_FUN(	spec_cast_undead	);
DECLARE_SPEC_FUN(	spec_executioner	);
DECLARE_SPEC_FUN(	spec_fido		);
DECLARE_SPEC_FUN(	spec_guard		);
DECLARE_SPEC_FUN(	spec_janitor		);
DECLARE_SPEC_FUN(	spec_mayor		);
DECLARE_SPEC_FUN(	spec_poison		);
DECLARE_SPEC_FUN(	spec_repairman		);
DECLARE_SPEC_FUN(	spec_thief		);
DECLARE_SPEC_FUN(	spec_healer		); /* Lam 1997 */
DECLARE_SPEC_FUN(	spec_waker		); /* Qwert 12.11.2000 */
DECLARE_SPEC_FUN(	spec_unicorn		); /* Ulryk 03.03.2002 */
DECLARE_SPEC_FUN(       spec_parrot             ); /* Ulryk 26.02.2003 */
DECLARE_SPEC_FUN(       spec_vampire_elder      ); /* Ulryk 20.11.2003 */


DECLARE_GAME_FUN(	gra_czarna_czerwona	);
DECLARE_GAME_FUN(	gra_kolo_fortuny	);
DECLARE_GAME_FUN(	gra_marynarz		);
DECLARE_GAME_FUN(	gra_nwt			);
DECLARE_GAME_FUN(	gra_pewna_reka		);
DECLARE_GAME_FUN(	gra_pkn			);
DECLARE_GAME_FUN(	gra_siodemka		);
DECLARE_GAME_FUN(	gra_szczurek		);
DECLARE_GAME_FUN(	gra_trzy_kubki		);
DECLARE_GAME_FUN(	gra_wojna_kart		);
DECLARE_GAME_FUN(	gra_wojna_kosci		);
DECLARE_GAME_FUN(	gra_zgadnij_karte	);


/*
 * Malven: klany
 */
/*DECLARE_CLAN_FUN(	xxx			);*/


/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD      CHAR_DATA
#define MIDT    MOB_INDEX_DATA
#define OD      OBJ_DATA
#define AD      AFFECT_DATA
#define ED      EXTRA_DESCR_DATA
#define OID     OBJ_INDEX_DATA
#define RID     ROOM_INDEX_DATA
#define SF      SPEC_FUN
#define TID	TIMER_DATA
#define AOD	AIROBJ_DATA
#define ACD	AIRCHAR_DATA
#define TLD	TELL_DATA

/* act_comm.c */
void    add_follower    args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void    stop_follower   args( ( CHAR_DATA *ch ) );
void    die_follower    args( ( CHAR_DATA *ch ) );
bool    is_same_group   args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
bool    is_note_to      args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	talk_auction	args( ( char *argument ) );
bool	can_quit_now	args( ( CD *ch, bool fCicho ) );
char	*makedrunk	args( ( char *string, CHAR_DATA *ch ) );
REFUSE_DATA *refuse_lookup args( ( CHAR_DATA *ch, char *arg ) );
void	rewrite_all_notes args( ( void ) );


/* act_info.c */
void    show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
					bool fShort, bool fShowNothing,
					bool krocej, bool inwentarz,
					bool fDontCombine,
					int ilosc, char *argument ) );
void	pokaz_pomieszczenie	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pom,
					bool skrot ) );
void    real_help       args( ( CHAR_DATA *ch, char *arg, char *skroty,
				WHO_DESCRIPTOR_DATA *d ) );
void	real_slist	args( ( CHAR_DATA *ch, char *arg,
				WHO_DESCRIPTOR_DATA *d ) );
void    sprawdz_hasla   args( ( char *arg, char *haslo,
				char *skroty, int poziom ) );
void	real_who	args( ( CHAR_DATA *ch, char *argument,
				WHO_DESCRIPTOR_DATA *who_d ) );
void	odmiana_prompt  args( ( CHAR_DATA *ch ) );
void    set_title       args( ( CHAR_DATA *ch, char *title ) );
bool    check_blind     args( ( CHAR_DATA *ch ) );
char  * asctime_pl      args( ( const struct tm *timeptr ) );
char  * asctime_pl_nonl args( ( const struct tm *timeptr ) );
char  * asctime_pl_krotko args( ( const struct tm *timeptr ) );
char  * asctime_pl_krotko_nonl args( ( const struct tm *timeptr ) );
void	wczytaj_bledomat args( ( void ) );

/* act_move.c */
int     find_door       args( ( CHAR_DATA *ch, char *arg, bool drzwi ) );
void    move_char       args( ( CHAR_DATA *ch, int door, bool wyciaganie ) );
void	po_wejsciu	args( ( CHAR_DATA *ch, bool ostroznosc ) );
FUNKCJA_CZASOWA( finish_digging );
FUNKCJA_CZASOWA( finish_doom );
bool	can_push_pull	args( ( CHAR_DATA *ch, CHAR_DATA *victim,
				ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to,
				int door ) );
void	wchodzenie_do_pomieszczenia args( ( CHAR_DATA *ch,
		ROOM_INDEX_DATA *in_room, ROOM_INDEX_DATA *to_room, int door,
		bool pokaz_kierunek ) );

/* act_obj.c */
int	advatoi		args( ( char *s ) );
bool    remove_obj      args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void    zmien_poziom    args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    check_wield     args( ( CHAR_DATA *ch ) );
void    zmien_brzeczenie args( ( CHAR_DATA *ch, bool fOn, bool fCiche ) );
FUNKCJA_CZASOWA( przestan_karmic );
FUNKCJA_CZASOWA( przestan_poic );

/* act_wiz.c */
RID   * find_location args( ( CHAR_DATA *ch, char *arg ) );
void	wiznet		args( ( const char *string, CHAR_DATA *ch, OBJ_DATA *obj,
				void *arg, int flag, int no_flag,
				int min_level ) );
void	konwersja_bumi	args( ( CHAR_DATA *ch ) );
char  * ascdata		args( ( const struct tm *timeptr ) );
const char *krotko_bezczynnosc args( ( int idle ) );

/* comm.c */
int	comm_main	args( ( bool edytor, int argc, char **argv ) );
bool	check_ban	args( ( char *host, char *ip, char *user, int type ) );
void    close_socket    args( ( DESCRIPTOR_DATA *dclose, bool lost ) );
void    read_from_buffer        args( ( DESCRIPTOR_DATA *d, bool msg ) );
void    write_to_buffer args( ( DESCRIPTOR_DATA *d, const char *txt,
				int length ) );
int     write_to_descriptor     args( ( int desc, const void *txt, int length ) );
void	write_to_who_buffer args( ( WHO_DESCRIPTOR_DATA *d, const char *txt,
				int length ) );
void	write_to_imud_buffer args( ( const char *txt ) );
void	ch_printf	args( ( CD *ch, const char *fmt, ... ) ) __attribute__( ( format( printf, 2, 3 ) ) );
void    send_to_all_char args( ( const char *text ) );
void    send_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
void    show_string     args( ( DESCRIPTOR_DATA *d, char *input ) );
void    super_act	args( ( unsigned int opcja, int zmysly,
				const char *format, CHAR_DATA *ch,
				const void *arg1, const void *arg2, int type,
				bool ooc ) );
/* Lam: uzywac ponizszych, dzieki czemu bedzie mozna zmieniac super_act
	do woli, nie zmieniajac niczego innego w kodzie, a jedynie makra */
#define act( z, f, ch, a1, a2, t )    super_act( 0, z, f, ch, a1, a2, t, FALSE )
#define act_pp( z, f, ch, a1, a2, t ) super_act( 0, z, f, ch, a1, a2, t, TRUE )
#define STC( t, ch )	send_to_char( t, ch )
#define ASTC( t, ch )	super_act( 0, ZM_ZAWSZE, t, ch, NULL, NULL, TO_CHAR, TRUE )
#define STH( d, t )	write_to_who_buffer( d, t, 0 )

/*
 * ZMYSLY, z braku komentarzy wprowadzam w dokladnie takiej formie, jaka
 * zaproponowalem kwartal temu :(
 * opis w doc/zmysly.txt
 * Lam 9.3.2006
 */
#define ZM_WZROK	b00
#define ZM_SLUCH	b01
#define ZM_WECH		b02
#define ZM_DOTYK	b03
#define ZM_ZAWSZE	b04
#define ZM_PRZYT	( ZM_WZROK | ZM_SLUCH | ZM_WECH | ZM_DOTYK )
#define ZM_WID_CHAR	b05
#define ZM_WID_VICT	b06
#define ZM_WID_OBJ1	b07
#define ZM_WID_OBJ2	b08
#define ZM_W_WZROK	b09
#define ZM_W_SLUCH	b10
#define ZM_W_WECH	b11
#define ZM_W_DOTYK	b12
#define ZM_W_PRZYT	( ZM_W_WZROK | ZM_W_SLUCH | ZM_W_WECH | ZM_W_DOTYK )
#define ZM_W_WID_CHAR	b13
#define ZM_W_WID_VICT	b14
#define ZM_W_WID_OBJ1	b15
#define ZM_W_WID_OBJ2	b16
#define ZM_DYLEMAT	0

/*
 * Kolory i polskie znaki
 */
int     colour          args( ( char type, CHAR_DATA *ch, char *string ) );
int     spolszcz        args( ( char type, int polskie, char *string ) );
void    colourconv      args( ( char *buffer, const char *txt,
				CHAR_DATA *ch, unsigned int size ) );
void    send_to_char_bw args( ( const char *txt, CHAR_DATA *ch ) );
#if defined ( MSDOS ) && !defined( NO_SOUND )
void	damage_sound	args( ( void ) );
void	death_sound	args( ( void ) );
#endif


/* alias.c - Lam */
ALIAS_DATA * alias_lookup args( ( CHAR_DATA * ch, char *arg ) );

/* clan.c */
CLAN_DATA *clan_lookup2	args( ( char *arg ) );
bool	can_see_clan	args( ( CD *ch, CLAN_DATA *clan ) );
void	real_clan_lista	args( ( CD *ch, char *argument, WHO_DESCRIPTOR_DATA *d ) );
void    real_clan_info	args( ( CD *ch, CLAN_DATA *cl, WHO_DESCRIPTOR_DATA *d ) );
void    check_clan_rel	args( ( void ) );
CLAN_REL_DATA * clan_rel_new args( ( void ) ) __attribute__( ( warn_unused_result ) );
CLAN_MEMBER_DATA * clan_member_new args( ( void ) ) __attribute__( ( warn_unused_result ) );
CLAN_DATA * clan_new	args( ( void ) ) __attribute__( ( warn_unused_result ) );
void    clan_remove     args( ( CLAN_DATA *clan ) );
void	usun_z_klanu	args( ( CHAR_DATA *ch ) );
CLAN_REL_DATA * clan_rel_lookup args( ( CLAN_DATA *clana, CLAN_DATA *clanb ) );
CLAN_DATA * clan_lookup args( ( char *arg ) );
bool    is_clan_member  args( ( CD *ch, CLAN_DATA *clan ) );
bool    is_any_clan_member   args( ( CD *ch ) );
bool    is_any_clan_member2  args( ( CD *ch ) );
bool    is_same_clan    args( ( CD *ch, CD *victim ) );
void    check_new_clan  args( ( CD *ch ) );
bool    load_clan_statut args( ( CLAN_DATA *clan ) );
void    clan_log        args( ( CLAN_DATA *clan, char *str ) );
void	write_clan      args( ( CLAN_DATA *clan, int flagi ) );
void	write_clans     ( void );
void	zapisz_urny     ( void );


/* db.c, ssm.c */
bool	load_area_file	args( ( FILE *fp, CHAR_DATA *ch ) );
void    real_areas      args( ( CHAR_DATA *ch, char *argument,
				WHO_DESCRIPTOR_DATA *d ) );
void    boot_db         args( ( bool edytor ) );
void    area_update     args( ( void ) );
CD *    new_character   args( ( bool player ) ) __attribute__( ( warn_unused_result ) );
SHOP_DATA *new_shop	args( ( void ) ) __attribute__( ( warn_unused_result ) );
HEALER_DATA *new_healer	args( ( void ) ) __attribute__( ( warn_unused_result ) );
ALIAS_DATA * new_alias  args( ( void ) ) __attribute__( ( warn_unused_result ) );
void    del_alias	args( ( CHAR_DATA *ch, ALIAS_DATA * alias ) );
REFUSE_DATA *new_refuse args( ( void ) ) __attribute__( ( warn_unused_result ) );
void    del_refuse	args( ( CHAR_DATA *ch, REFUSE_DATA *refuse ) );
TLD   * new_tell	args( ( void ) ) __attribute__( ( warn_unused_result ) );
void    del_tell	args( ( CHAR_DATA *ch, TELL_DATA *tell ) );
MPQUEST_DATA *new_mpquest args( ( void ) ) __attribute__( ( warn_unused_result ) );
void    del_mpquest	args( ( CHAR_DATA *ch, MPQUEST_DATA *mpquest ) );
MPQUEST_DATA *find_mpquest args( ( CHAR_DATA *ch, int vnum ) );
AD *    new_affect      args( ( void ) ) __attribute__( ( warn_unused_result ) );
OD *    new_object      args( ( void ) ) __attribute__( ( warn_unused_result ) );
ED *    new_extra_descr args( ( void ) ) __attribute__( ( warn_unused_result ) );
void    del_extra_descr	args( ( EXTRA_DESCR_DATA *ed ) );
MPROG_DATA *new_mprog	args( ( void ) ) __attribute__( ( warn_unused_result ) );
void    del_mprog	args( ( MPROG_DATA *prog ) );
CD *    create_mobile   args( ( MOB_INDEX_DATA *pMobIndex ) ) __attribute__( ( warn_unused_result ) );
OD *    create_object   args( ( OBJ_INDEX_DATA *pObjIndex, int level ) ) __attribute__( ( warn_unused_result ) );
void	dod_zwod	args( ( ZWOD_DATA **lista, CHAR_DATA *cel ) );
void	wywal_zwod	args( ( ZWOD_DATA **lista, CHAR_DATA *cel ) );
void    clear_char      args( ( CHAR_DATA *ch ) );
void    free_char       args( ( CHAR_DATA *ch, bool sprawdz ) );
char *  get_extra_descr args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MIDT *  get_mob_index   args( ( int vnum ) );
OID *   get_obj_index   args( ( int vnum ) );
RID *   get_room_index  args( ( int vnum ) );
char    fread_letter    args( ( FILE *fp ) );
int     fread_number    args( ( FILE *fp, int *status ) );
char *  real_fread_string args( ( FILE *fp, int *status, const char *plik,
				int linia, const char *funkcja ) ) __attribute__( ( warn_unused_result ) );
char *  fread_string_eol args( ( FILE *fp, int *status ) ) __attribute__( ( warn_unused_result ) );
void    fread_to_eol    args( ( FILE *fp ) );
char *  fread_word      args( ( FILE *fp, int *status, bool czy_wielo ) );
void	real_fread_vector args( ( int *flag, unsigned int size, FILE *fp, int *status ) );
void *  alloc_mem       args( ( int sMem ) ) __attribute__( ( warn_unused_result ) );
void *  alloc_perm      args( ( int sMem ) ) __attribute__( ( warn_unused_result ) );
void    free_mem        args( ( void *pMem, int sMem ) );
char *  real_str_dup    args( ( const char *str, const char *plik, int linia,
				const char *funkcja ) ) __attribute__( ( warn_unused_result ) );
void    real_free_string args( ( char *pstr, const char *plik, int linia,
				const char *funkcja ) );
/* dzieki temu widac, ktora funkcja wywoluje str_dup( )/free_string( ) */
#ifdef DOPALANY_BUG
# define str_dup( s ) real_str_dup( s, __FILE__, __LINE__, __FUNCTION__ )
# define fread_string( str, status ) real_fread_string( str, status, __FILE__, __LINE__, __FUNCTION__ )
# define free_string( s ) { real_free_string( s, __FILE__, __LINE__, __FUNCTION__ ); s = NULL; }
#else
# define str_dup( s ) real_str_dup( s, "?", 0, "?" )
# define fread_string( str, status ) real_fread_string( str, status, "?", 0, "?" )
# define free_string( s ) { real_free_string( s, "?", 0, "?" ); s = NULL; }
#endif
void    append_file     args( ( CHAR_DATA *ch, char *file, char *str ) );
void    real_bug        args( ( const char *str, int param, const char *plik,
				int linia, const char *funkcja, bool zrzut,
				bool zrob_core ) );
/* dzieki temu widac, ktora funkcja wywoluje bug( )
   cbug jak czysta wersja bug - nie pokazuje zrzutu stosu wywolan
   dbug robi core, ale nie przerywa gry */
#ifdef DOPALANY_BUG
# define bug( s, p ) real_bug( s, p, __FILE__, __LINE__, __FUNCTION__, TRUE, FALSE )
# define cbug( s, p ) real_bug( s, p, __FILE__, __LINE__, __FUNCTION__, FALSE, FALSE )
# define dbug( s, p ) real_bug( s, p, __FILE__, __LINE__, __FUNCTION__, TRUE, TRUE )
#else
# define bug( s, p ) real_bug( s, p, "?", 0, "?", TRUE, FALSE )
# define cbug( s, p ) real_bug( s, p, "?", 0, "?", FALSE, FALSE )
# define dbug( s, p ) real_bug( s, p, "?", 0, "?", TRUE, TRUE )
#endif
void	lac_perror	args( ( const char *str ) );
void    log_string      args( ( const char *str ) );
void    log_string_ch   args( ( CHAR_DATA *ch, const char *str ) );
void    tail_chain      args( ( void ) );
char *	mprog_type_to_name	args( ( int type, int gdzie ) );
KOLES_DATA *	znajdz_kolesia	args( ( int vnum ) );
OID *	new_obj_index	args( ( int vnum, AREA_DATA *area ) ) __attribute__( ( warn_unused_result ) );
MIDT *	new_mob_index	args( ( int vnum, AREA_DATA *area ) ) __attribute__( ( warn_unused_result ) );
RID *	new_room	args( ( int vnum, AREA_DATA *area ) ) __attribute__( ( warn_unused_result ) );
void	del_room	args( ( ROOM_INDEX_DATA *pRoomIndexData ) );
EXIT_DATA *new_exit	args( ( void ) ) __attribute__( ( warn_unused_result ) );
void	del_exit	args( ( EXIT_DATA *ex ) );
RESET_DATA *new_reset	args( ( void ) ) __attribute__( ( warn_unused_result ) );
void	del_reset	args( ( RESET_DATA *reset ) );
ZONE_DATA *znajdz_strefe args( ( char *nazwa ) );
ZONE_DATA *znajdz_strefe_skrot args( ( char *nazwa ) );
ZONE_DATA *znajdz_strefe_po_stolicy args( ( char *nazwa ) );
ZONE_DATA *znajdz_strefe_po_vnumie args( ( int vnum ) );
ZONE_LIST *dodaj_strefe args( ( ZONE_LIST *strefy, ZONE_DATA *strefa ) );
bool    strefa_nalezy args( ( ZONE_DATA *strefa, ZONE_LIST *lista ) );
int	ssm_max_free_entry	args( ( void ) );
int	ssm_num_free_entries	args( ( void ) );
void	init_string_space	args( ( void ) );
void	boot_done		args( ( void ) );
#if defined( DEBUG_SSM )
void	zrzut_kwasow_lancuchow	args( ( void ) );
void	oznacz_wszystko		args( ( void ) );
#endif
IMIONA_DATA *nowe_imie		args( ( void ) );
void	dodaj_imie		args( ( IMIONA_DATA *im ) );
void	zwolnij_imie		args( ( IMIONA_DATA *im ) );
void	zapisz_imiona		args( ( void ) );

/* fight.c */
int     xp_compute           args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
bool	czy_grupa_wrogow args( ( CHAR_DATA *ch, CHAR_DATA *gch ) );
bool    group_gain      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    violence_update args( ( void ) );
void    multi_hit       args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    set_fighting    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_fighting     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt,
				int wpn ) );
void    damage          args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			       int dt, int wpn, bool czy_bez_walki ) );
void    raw_kill        args( ( CHAR_DATA *ch, CHAR_DATA *victim,
				bool czy_bez_walki, bool czy_dodac_zgon,
				int typ_zgonu ) );
void    stop_fighting   args( ( CHAR_DATA *ch, bool fBoth ) );
void    stop_fighting_char
			args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    update_pos      args( ( CHAR_DATA *victim ) );
void    check_killer    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_safe         args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool czy ) );
bool	czy_morderstwo	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    licensed        args( ( CHAR_DATA *ch ) );
/*bool  registered      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );*/
void	death_cry	args( ( CHAR_DATA *ch, int dt, int pozycja ) );
void    pp_get          args( ( CHAR_DATA *ch, char *victim_name ) ); /* Qwert */
void	strata_dosw_za_smierc	args( ( CHAR_DATA *victim ) );
void    dodaj_zabojce   args( ( CHAR_DATA *ofiara, CHAR_DATA *zabojca ) );
bool    ocen_przedmiot  args( ( int v1, int v2, int v3 ) );
CHAR_DATA *fighting_func args( ( CHAR_DATA *ch ) );


/* gry.c */
GAME_FUN *game_lookup	args( ( const char *name ) );
char *	game_name	args( ( GAME_FUN *fun ) );
extern struct game_type game_table[ ];


/* handler.c, niektore przeniesione do lanclicz.c */
TID   * new_timer	args( ( void ) ) __attribute__( ( warn_unused_result ) );
void	del_timer	args( ( TIMER_DATA *timer ) );
TID   * find_timer	args( ( CHAR_DATA *ch, TIMER_FUN *fun ) );
void	run_timer	args( ( TIMER_DATA *timer ) );
void	timer_update	args( ( void ) );
AOD   * new_airobj	args( ( OBJ_DATA *obj ) );
AOD   * find_airobj	args( ( OBJ_DATA *obj ) );
void	del_airobj	args( ( AIROBJ_DATA *airobj ) );
ACD   * new_airchar	args( ( CHAR_DATA *chr ) );
ACD   * find_airchar	args( ( CHAR_DATA *chr ) );
void	check_airchar	args( ( CHAR_DATA *chr ) );
void	del_airchar	args( ( AIRCHAR_DATA *airchar ) );

int	liczba		args( ( int l ) );
char  * itopl		args( ( unsigned int l, int plec ) );
char  * slowny_ciezar	args( ( int ciezar ) );
int	strlen_pl	args( ( const char *txt ) );
int	strlen_pl_kol	args( ( const char *txt ) );
int	strlen_pl_kol_miel args( ( const char *txt ) );
int	znajdz_adres_pl_kol args( ( const char *txt, int literka ) );
char  * wyrownaj	args( ( const char *text, int dlug ) );
char  * formatuj_liczbe args( ( int arg ) );
bool	czy_ze		args( ( const char *slowo ) );
bool	czy_we		args( ( const char *slowo ) );
bool	czy_wszystko	args( ( char *arg ) );
bool	czy_wszyscy	args( ( char *arg ) );
bool	czy_monety	args( ( char *arg ) );
bool	czy_do_wszystkich args( ( char *arg ) );
bool    ma_w_multi      args( ( CHAR_DATA *ch, int sn ) );
void	wypelnij_poziomy args( ( CHAR_DATA *ch, int *poziomy ) );
bool	moze_uzyc	args( ( CHAR_DATA *ch, int sn ) );
void	odkoloruj	args( ( const char *arg, char *bufor ) );
int	ile_kolorow	args( ( const char *arg ) );
bool	czy_bluzg1	args( ( const char *arg ) );
bool	czy_bluzg	args( ( const char *arg ) );
struct miodek_data *znajdz_miodka args( ( const char *arg ) );
bool	miodek		args( ( CHAR_DATA *ch, char *arg ) );
bool    miodek_opisowy  args( ( CHAR_DATA *ch, char *arg ) );
char *	przemiel_opis	args( ( CHAR_DATA *ch, char *opis ) );
int	drzwi_d		args( ( char *slowo ) );

int     get_trust       args( ( CHAR_DATA *ch ) );
int	skill_get_trust	args( ( CHAR_DATA *ch ) );
int     get_age         args( ( CHAR_DATA *ch ) );
int     get_clanstat    args( ( CHAR_DATA *ch ) );
int     get_curr_str    args( ( CHAR_DATA *ch ) );
int     get_curr_int    args( ( CHAR_DATA *ch ) );
int     get_curr_wis    args( ( CHAR_DATA *ch ) );
int     get_curr_dex    args( ( CHAR_DATA *ch ) );
int     get_curr_con    args( ( CHAR_DATA *ch ) );
int     get_hitroll     args( ( CHAR_DATA *ch, int wpn ) );
int     get_damroll     args( ( CHAR_DATA *ch, int wpn ) );
int     can_carry_n     args( ( CHAR_DATA *ch ) );
int     can_carry_w     args( ( CHAR_DATA *ch ) );

bool    is_name         args( ( const char *str, char *namelist ) );
bool    is_name2        args( ( const char *str, char *namelist ) );
void    affect_to_char  args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_remove   args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool checks ) );
void    affect_strip    args( ( CHAR_DATA *ch, int sn ) );
bool    is_affected     args( ( CHAR_DATA *ch, int sn ) );
bool	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf, int how ) );
void    affect_to_room  args( ( ROOM_INDEX_DATA *room, AFFECT_DATA *paf ) );
bool    room_is_affected args( ( ROOM_INDEX_DATA *room, int sn ) );
void    affect_from_room args( ( ROOM_INDEX_DATA *room, int sn ) );
void	sprawdz_przyzwyczajanie_ch args( ( CHAR_DATA *dch, bool czy_ciemnia ) );
void	sprawdz_przyzwyczajanie args( ( ROOM_INDEX_DATA *room ) );
void    char_from_room  args( ( CHAR_DATA *ch ) );
void    char_to_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_char     args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    obj_from_char   args( ( OBJ_DATA *obj ) );
void    obj_to_przechowalnia    args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    obj_from_przechowalnia  args( ( OBJ_DATA *obj ) );
int     apply_ac        args( ( OBJ_DATA *obj, int iWear ) );
OD *    get_eq_char     args( ( CHAR_DATA *ch, int iWear ) );
int	czy_pasuje	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
bool	can_equip_at	args( ( OBJ_DATA *obj, int at ) );
bool	can_equip_index_at args( ( OBJ_INDEX_DATA *obj, int at ) );
bool	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void    save_equip_char args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void    unequip_char    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void    unequip_char_save args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	check_race_wearlocs args( ( CHAR_DATA *ch ) );
void    sprawdz_ubranie args( ( CHAR_DATA *ch ) );
int     count_obj_list  args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void    obj_from_room   args( ( OBJ_DATA *obj ) );
void    obj_to_room     args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_obj      args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void    obj_from_obj    args( ( OBJ_DATA *obj ) );
void    extract_obj     args( ( OBJ_DATA *obj ) );
void    extract_char    args( ( CHAR_DATA *ch, bool fPull ) );
const char *name_expand args( ( CHAR_DATA *viewer, CHAR_DATA *ch ) );
const char *name_expand_obj args( ( CHAR_DATA *viewer, OBJ_DATA *obj ) );
CD *    get_char_room   args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world  args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world_pc args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world_pc_tele args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_room2  args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world2 args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_area   args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_area2  args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_mob_world_vnum args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_type    args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *    get_obj_list    args( ( CHAR_DATA *ch, char *argument,
			       OBJ_DATA *list ) );
OD *    get_obj_carry   args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_przechowalnia args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_wear    args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_here    args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_world   args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_world_vnum args( ( CHAR_DATA *ch, char *argument ) ); /* Tissarius */
OD *    create_money    args( ( int amount ) );
int     get_obj_number  args( ( OBJ_DATA *obj ) );
int     get_obj_weight  args( ( OBJ_DATA *obj ) );
int     get_obj_weight_vis_only  args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool    room_is_dark    args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    room_is_private args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_enter_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room,
				bool pokaz ) );
bool	jest_w_lodzi	args( ( CHAR_DATA *ch ) );
int	can_move	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *in_room,
				ROOM_INDEX_DATA *to_room, int door, bool czy ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_dla_okna args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_who	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_see_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *  item_type_name_pl args( ( OBJ_DATA *obj ) );
char *  extra_bit_name_pl args( ( int *extra_flags, CHAR_DATA *ch ) );
char *  extra_bit_name_pl_dla_ludzi args( ( int *extra_flags, CHAR_DATA *ch ) );
bool	extra_bit_avail	args( ( int *extra_flags ) );
char *  affect_loc_name_pl_b args( ( int location ) );
bool	affect_loc_avail args( ( int location ) );
char *	affect_bit_name_pl args( ( int *vector ) );
bool	affect_bit_avail args( ( int *vector ) );
char *  wear_flag_name_pl args( ( int wear_flags ) );
char *	act_flag_name_pl args( ( int act, CHAR_DATA *ch ) );
bool	act_flag_avail	args( ( int act ) );
char *	room_flag_name_pl args( ( int room ) );
char *	exit_info_name_pl args( ( int flag ) );
CD   *  get_char        args( ( CHAR_DATA *ch ) );
bool    longstring      args( ( CHAR_DATA *ch, char *argument ) );
bool    authorized      args( ( CHAR_DATA *ch, char *skllnm ) );
bool    is_authorized   args( ( CHAR_DATA *ch, char *skllnm ) );
void    end_of_game     args( ( void ) );
int     material_lookup args( ( const char *material ) );
int     material_lookup_short args( ( const char *material ) );
int	liquid_lookup	args( ( const char *liquid ) );
int     race_lookup     args( ( const char *race ) );
int     affect_lookup   args( ( const char *race ) );
int zloto_w_pojemnikach	args( ( OBJ_DATA *lista ) );
void	wywal_smieci	args( ( CHAR_DATA *ch ) );
int	licz_graczy	args( ( void ) );
int	licz_polaczenia	args( ( void ) );
void	czy_rekord_graczy	args( ( void ) );
void	czy_rekord_polaczen	args( ( void ) );
void    sprawdz_wampira		args( ( CHAR_DATA *ch ) );
void    zmien_wlasciciela	args( ( OBJ_DATA *obj, char *stare_imie, char *imie ) );
void	zmien_wlasciciela_wszedzie args( ( CHAR_DATA *victim, char *new_name ) );
void	zmien_imie_postaci	args( ( CHAR_DATA *victim, char *new_name ) );
void	zapisz_ustawienia	args( ( void ) );
void	czytaj_ustawienia	args( ( void ) );
bool	czy_mail		args( ( char *arg ) );
int	zetnij			args( ( int limit, int tempo, int x ) );
char   *zlota_moneta		args( ( int ile, bool fMianownik, bool fIle ) );
#define ZLOTO_M( ile )		( zlota_moneta( ile, TRUE, TRUE ) )
#define ZLOTO_B( ile )		( zlota_moneta( ile, FALSE, TRUE ) )
bool	czy_zajete_imie		args( ( char *imie ) );
int	wiek_krainy		args( ( AREA_DATA *pArea ) );
void	popraw_czas_krainy	args( ( AREA_DATA *pArea ) );
void	sprawdz_restrykcje	args( ( CD *ch ) );
bool	sprawdz_deny		args( ( CD *ch ) );
time_t	policz_czas		args( ( char *arg ) );
bool	czy_mozna_kopac		args( ( int sector_type, CHAR_DATA *ch ) );


/* httpd.c */
HTML_FUN *szukaj_strony		args( ( char *arg ) );
void	http_log		args( ( WHO_DESCRIPTOR_DATA *d ) );
bool	get_who_data		args( ( char *arg, WHO_DESCRIPTOR_DATA *d ) );
char	*lac2html		args( ( char *buf, bool mysql ) );
DECLARE_HTML_FUN( html_index );
DECLARE_HTML_FUN( html_who   );
DECLARE_HTML_FUN( html_help  );
DECLARE_HTML_FUN( html_areas );
DECLARE_HTML_FUN( html_news  );
DECLARE_HTML_FUN( html_maps  );
DECLARE_HTML_FUN( html_slist );
DECLARE_HTML_FUN( html_commands );
#if defined( KLANY )
DECLARE_HTML_FUN( html_clans );
#endif
DECLARE_HTML_FUN( html_404 );

/* hunt.c */
int	find_path	args( ( ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to,
				int max_depth, bool same_area,
				bool through_doors ) );
void    auto_hunt       args( ( CHAR_DATA *ch ) );
void	hunt_victim	args( ( CHAR_DATA *ch ) );

/* trzeci parametr interpret( ) */
#define INTERP_NORMAL	  0
#define INTERP_FORCE	  1
#define INTERP_AT	  2
#define INTERP_MANY	  3
#define INTERP_ORDER	  4
#define INTERP_MPROG	100
#define INTERP_MPFORCE	101
#define INTERP_MPAT	102
#define CMD_NOTLISTED	b05
/* interp.c */
bool    interpret       args( ( CHAR_DATA *ch, char *argument, int zastos ) );
bool    is_number       args( ( char *arg ) );
bool    is_ipaddr       args( ( char *arg ) );
int     number_argument args( ( char *argument, char *arg ) );
char *  one_argument    args( ( char *argument, char *arg_first ) );
char *  one_argument2   args( ( char *argument, char *arg_first ) );
const CMD_TYPE *znajdz_polecenie args( ( char *argument, int poziom ) );

/* lanclicz.c */
void	init_mm		args( ( void ) );
int     number_fuzzy    args( ( int number ) );
int     number_range    args( ( int from, int to ) );
int     number_percent  args( ( void ) );
int     number_door     args( ( void ) );
int     number_bits     args( ( int width ) );
int     number_mm       args( ( void ) );
int     dice            args( ( int number, int size ) );
int     interpolate     args( ( int level, int value_00, int value_32 ) );
int	zlicz_bity	args( ( int liczba ) );
void    smash_tilde     args( ( char *str ) );
bool	alf_wczes	args( ( const char *astr, const char *bstr ) );
bool    str_cmp         args( ( const char *astr, const char *bstr ) );
bool    str_prefix      args( ( const char *astr, const char *bstr ) );
bool    str_infix       args( ( const char *astr, const char *bstr ) );
bool    str_suffix      args( ( const char *astr, const char *bstr ) );
char *	str_str		args( ( char *bstr, const char *astr ) );
char *  capitalize      args( ( const char *str ) );
char *  zwielkoliteruj  args( ( const char *str ) );
void	zjedz_entery_z_konca args( ( char **lancuch ) );

/* magic.c */
int     skill_lookup_ang args( ( const char *name ) );
int     skill_lookup_pl args( ( const char *name ) );
void	say_spell	args( ( CHAR_DATA *ch, int sn ) );
bool    saves_spell     args( ( int level, CHAR_DATA *victim ) );
void	opoznienie_czaru args( ( CHAR_DATA *ch, int sn ) );
void    obj_cast_spell  args( ( int sn, int level, CHAR_DATA *ch,
			       CHAR_DATA *victim, OBJ_DATA *obj,
			       char *arg, int usage ) );

/* mob_prog.c */
char *	mprog_next_command	args( ( char* clist, char *cmnd ) );
void	mprog_process_cmnd	args( ( char* cmnd, CHAR_DATA* mob,
				       CHAR_DATA* actor, OBJ_DATA* obj,
				       CHAR_DATA* rndm, CHAR_DATA* vict,
				       OBJ_DATA* v_obj ) );
void	mprog_wordlist_check	args( ( char * arg, CHAR_DATA *mob,
					CHAR_DATA* actor, OBJ_DATA* object,
					CHAR_DATA* vict, OBJ_DATA* v_obj,
					int type, MPROG_DATA *program,
					int gdzie ) );
void	mprog_percent_check	args( ( CHAR_DATA *mob, CHAR_DATA* actor,
					OBJ_DATA* object, CHAR_DATA *vict,
					OBJ_DATA *v_obj, int type,
					MPROG_DATA *program, int gdzie ) );
/* void	mprog_act_trigger	args( ( char* buf, CHAR_DATA* mob,
					CHAR_DATA* ch, OBJ_DATA* obj,
					void* vo ) ); */
void	mprog_bribe_trigger	args( ( CHAR_DATA* mob, CHAR_DATA* ch,
					int amount ) );
void	mprog_entry_trigger	args( ( CHAR_DATA* mob ) );
void	mprog_give_trigger	args( ( CHAR_DATA* mob, CHAR_DATA* ch,
					OBJ_DATA* obj ) );
void	mprog_greet_trigger	args( ( CHAR_DATA* mob ) );
void	mprog_leave_trigger	args( ( CHAR_DATA* mob ) );
void	mprog_fight_trigger	args( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void	mprog_hitprcnt_trigger	args( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
bool	mprog_death_trigger	args( ( CHAR_DATA* mob, CHAR_DATA* ch ) ) __attribute__( ( warn_unused_result ) );
void	mprog_random_trigger	args( ( CHAR_DATA* mob ) );
void	mprog_speech_trigger	args( ( CHAR_DATA *ch,	char *txt,
					CHAR_DATA *vict ) );
void	mprog_time_trigger	args( ( CHAR_DATA *ch ) );
void	mprog_asleep_time_trigger args( ( CHAR_DATA *ch ) );
void	mprog_soc_trigger	args( ( char *txt, CHAR_DATA *mob,
					CHAR_DATA *actor ) );
void	mprog_steal_trigger	args( ( CHAR_DATA *mob, CHAR_DATA *ch,
					OBJ_DATA *obj ) );
void	mprog_steal_fail_trigger args( ( CHAR_DATA *mob, CHAR_DATA *ch,
					OBJ_DATA *obj ) );
bool	mprog_descr_trigger	args( ( CHAR_DATA *mob, CHAR_DATA *ch ) );
void    mprog_feed_trigger	args( ( CHAR_DATA *mob, CHAR_DATA *ch,
					OBJ_DATA *obj ) );
void	mprog_water_trigger	args( ( CHAR_DATA *mob, CHAR_DATA *ch,
					OBJ_DATA *obj ) );
void	mprog_spell_victim_trigger args( ( char *txt, CHAR_DATA *mob,
					CHAR_DATA *actor ) );
bool	mprog_long_descr_trigger args( ( CHAR_DATA *mob, CHAR_DATA *ch,
					char *buf ) );
void	mprog_aggr_attack_trigger args( ( CHAR_DATA *mob, CHAR_DATA *ch ) );

void	rprog_greet_trigger	args( ( CHAR_DATA *ch ) );
void	rprog_speech_trigger	args( ( char *txt, CHAR_DATA *ch ) );
void	rprog_look_trigger	args( ( CHAR_DATA *ch ) );
bool	rprog_do_trigger	args( ( CHAR_DATA *ch, char *cmd,
					char *arg, bool skrot ) );
bool	rprog_descr_trigger	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room,
					char *txt ) );
void	rprog_drop_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	rprog_time_trigger	args( ( ROOM_INDEX_DATA *room ) );
void	rprog_random_trigger	args( ( ROOM_INDEX_DATA *room ) );
void	rprog_undug_trigger	args( ( CHAR_DATA *ch ) );

void	eprog_pass_trigger	args( ( CHAR_DATA *ch, int door ) );
void	eprog_open_trigger	args( ( CHAR_DATA *ch, int door ) );
void	eprog_close_trigger	args( ( CHAR_DATA *ch, int door ) );
void	eprog_lock_trigger	args( ( CHAR_DATA *ch, int door ) );
void	eprog_unlock_trigger	args( ( CHAR_DATA *ch, int door ) );
void	eprog_pick_trigger	args( ( CHAR_DATA *ch, int door ) );
void	eprog_bash_trigger	args( ( CHAR_DATA *ch, int door ) );
void	eprog_dig_trigger	args( ( CHAR_DATA *ch, int door ) );
bool	eprog_bounce_trigger	args( ( CHAR_DATA *ch, int door ) );

void	oprog_take_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	oprog_drop_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	oprog_wear_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	oprog_remove_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	oprog_buy_trigger	args( ( CHAR_DATA *ch, CHAR_DATA *sklep,
					OBJ_DATA *obj ) );
void	oprog_sell_trigger	args( ( CHAR_DATA *ch, CHAR_DATA *sklep,
					OBJ_DATA *obj ) );
void	oprog_use_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj,
					char *argument ) );
void	oprog_give_trigger	args( ( CHAR_DATA *ch, CHAR_DATA *adresat,
					OBJ_DATA *obj ) );
void	oprog_put_and_putin_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj,
					OBJ_DATA *container ) );
void	oprog_dig_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	oprog_steal_trigger	args( ( CHAR_DATA *ch, CHAR_DATA *vict,
					OBJ_DATA *obj ) );
void	oprog_steal_fail_trigger args( ( CHAR_DATA *ch, CHAR_DATA *vict,
					OBJ_DATA *obj ) );
void	oprog_putin_gold_trigger	args( ( CHAR_DATA *ch, int amount,
					OBJ_DATA *container ) );
void	oprog_open_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	oprog_close_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	oprog_descr_trigger	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
bool	oprog_long_descr_trigger args( ( OBJ_DATA *obj, CHAR_DATA *ch,
					char *buf ) );

char *	asctime_progbug		args( ( const struct tm *timeptr ) );
void	progbug			args( ( CHAR_DATA *mob, const char *str, ... ) ) __attribute__( ( format( printf, 2, 3 ) ) );
void	progdebug		args( ( CHAR_DATA *mob, const char *str, ... ) ) __attribute__( ( format( printf, 2, 3 ) ) );

/* MobC: mob_c.c, mob_var.c */
/* typy zmiennych */
#define TZ_CALK		0 /* calkowita */
#define TZ_ZNAK		1 /* znakowa (lancuch) */
#define TZ_POST		2 /* postac */
#define TZ_PRZE		3 /* przedmiot */

TYP_ZMIENNEJ *nowa_zmienna( char *nazwa, int typ, int wartosc, void *wsk ) __attribute__( ( warn_unused_result ) );
void usun_zmienna( TYP_ZMIENNEJ *zm );
void usun_z_listy( TYP_ZMIENNEJ **lista, TYP_ZMIENNEJ *zm, CHAR_DATA *mob );
TYP_ZMIENNEJ *znajdz_zmienna( TYP_ZMIENNEJ *lista, char *nazwa );
void zmien_zmienna( TYP_ZMIENNEJ **lista, char *nazwa, int typ, int wartosc,
		void *wsk, CHAR_DATA *mob );
bool czy_nazwa_zmiennej( char *nazwa );
bool czy_liczba( char *nazwa );
bool czy_zdatny_operator_int( char *op );
bool czy_zdatny_operator_str( char *op );
char *nastepny_skladnik( char *wyrazenie, char *skladnik );
bool oblicz_wyrazenie( CHAR_DATA *mob, char *wyrazenie, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm,
			CHAR_DATA *vict, OBJ_DATA *v_obj, int *zwr_typ,
			int *zwr_wart, char *zwr_txt, void **zwr_wsk );
void mv_int( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm,
			CHAR_DATA *vict, OBJ_DATA *v_obj );
void mv_aint( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm,
			CHAR_DATA *vict, OBJ_DATA *v_obj );
void mv_str( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm,
			CHAR_DATA *vict, OBJ_DATA *v_obj );
void mv_astr( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm,
			CHAR_DATA *vict, OBJ_DATA *v_obj );
void mv_chr( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm,
			CHAR_DATA *vict, OBJ_DATA *v_obj );
void mv_achr( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm,
			CHAR_DATA *vict, OBJ_DATA *v_obj );
void mv_obj( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm,
			CHAR_DATA *vict, OBJ_DATA *v_obj );
void mv_aobj( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm,
			CHAR_DATA *vict, OBJ_DATA *v_obj );
TYP_ZMIENNEJ *znajdz_zmienna_moba( CHAR_DATA *mob, char *nazwa );
int interpretuj_zmysly args( ( CHAR_DATA *ch, const char *argument ) );



/* nanny.c */
int	check_parse_name args( ( char *name, bool newname ) );
bool	czy_zdatny_plik	args( ( char *name ) );
char *	lac_sha1_crypt	args( ( const char *haslo, const char *salt ) );


/* save.c */
#define FREAD_OBJ_DB	0
#define FREAD_OBJ_VNUM	1
#define FREAD_OBJ_BLAD	2
char *  initial		args( ( const char *str ) );
void	save_char_obj	args( ( CHAR_DATA *ch ) );
int	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );
CD *	nowa_postac	args( ( DESCRIPTOR_DATA *d, char *name ) ) __attribute__( ( warn_unused_result ) );
int	fread_obj	args( ( void *parametr, FILE *fp, int use ) );
void	fwrite_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp,
				int iNest, bool own ) );

/* socjalne.c */
bool	check_social	args( ( CHAR_DATA *ch, char *command,
				char *argument ) );

/* special.c */
SF *    spec_lookup     args( ( const char *name ) );
char *  spec_name	args( ( SPEC_FUN *fun ) );
void	build_spec_list args( ( void ) );
bool    papuguj         args( ( CHAR_DATA *ch, char *argument ) );

/* update.c */
void    advance_level   args( ( CHAR_DATA *ch ) );
void    demote_level    args( ( CHAR_DATA *ch ) );
void    gain_exp        args( ( CHAR_DATA *ch, int gain ) );
/* cwiczenie umiejetnosci do 100% */
void	uzyj		args( ( CHAR_DATA *ch, int sn, const char *co ) );
void    gain_condition  args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler_before args( ( void ) );
void    update_handler  args( ( void ) );
void    ban_update      args( ( void ) );
void	auction_update	args( ( void ) );
void	komunikaty_ustroju args( ( CHAR_DATA *ch ) );

/* kzapisz.c */
void	save_area	args( ( AREA_DATA *area, bool save_dir ) );
char *	fix_string	args( ( const char *txt, bool enter, bool formatuj ) );
char *rozpisz_flagi_v_fun args( ( int *flagi, int ilebitow, bool czy_bez_b00 ) );
extern	char *	const	wearloc_str [ ];

/* screen.c */
void	upper_window	args( ( CHAR_DATA *ch ) );
void	lower_window	args( ( CHAR_DATA *ch ) );
char	kolor_gracza	args( ( CHAR_DATA *ch, const char *nazwa ) );
char	kolor_gracza_c	args( ( CHAR_DATA *ch, const char kod ) );
SCHEMAT_DATA * lookup_schemat args( ( const char *arg ) );

/* necro.c */
void    zwolnij_sluge           args( ( CHAR_DATA *ch ) );
void    zabij_wszystkie_slugi   args( ( CHAR_DATA *ch ) );

#undef	TLD
#undef	AOD
#undef	ACD
#undef	TID
#undef	CD
#undef	MIDT
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef	ED
#undef	AD
#ifdef __cplusplus
}
#endif
