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
 * ----------------------------------------------------------------------- *
 *  Plik konfiguracyjny pozwalajacy zmieniac rzeczy dajace sie zmienic     *
 *  wylacznie podczas kompilacji. Niektore rzeczy powinny dac sie zmieniac *
 *  w data/system.txt, ale chwilowo tak nie jest.                          *
 *  Przenioslem tu czesc merc.h sluzaca do zmian konfiguracyjnych          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* klany Malvena/Ulryka - undef blokuje polecenie dla poziomow < 106 oraz
   liste klanow w serwerze HTTP, poza tym wszystko jest kompilowane */
#undef KLANY


/* serwer www, do tworzenia odnosnikow w zintegrowanym serwerze www */
#define WWW_HOST    "lac.pl"
#define MUD_HOST    "lac.pl"
/* sprawdzacz uszkodzonych odnosnikow */
#undef LINK_CHECK


/* czy kraina musi posiadac wpis 'Zone' w sekcji #AREA?
   jesli nie, kraina dostanie jako strefe pierwsza strefe z pliku strefy.txt
   Lac/glaca zawsze zapisuje strefe w pliku .are, nawet jesli nie wymaga
   strefy w pliku wejsciowym */
#define WYMAGANE_STREFY


/* czy niania oferuje wybor dodatkowych umiejetnosci? Lac wycofal sie z tego
   i jego profesje nie zawieraja juz tych umiejetnosci. Zmiana tej definicji
   wplywa tez na obsluge czytania pola MltSkll z plikow postaci */
#undef DODATKOWE_W_NIANI


/* czy istnieje i dziala opcja autoklatwa? */
#undef MAM_AUTOKLATWE


/* czy istnieje polecenie "skasuj"? */
#undef MAM_SKASUJ


/* po jakim czasie noty sa kasowane i przenoszone do pliku notes.old? */
#define DLUGOSC_ZYCIA_NOTY	 30 /* w dniach */


/* #define IMUD aby wlaczyc IMa */
#undef IMUD
#if defined( IMUD )
# define IMUD_HOST "lac.pl" /* ew. "212.160.191.1" */
# define IMUD_PORT 1979
#endif


/* Lam 30.11.2000: sprawdzacz lancuchow, obciaza procesor, wlaczac tylko
   w razie klopotow z lancuchami, ale nie na dzialajacym mudzie na serwerze.
   Kiedy to pisalem, na moim Celeronie 366 MHz zajmowalo to 30% procesora.
   Teraz, w 2007, na Core 2 Duo 6400 (2,13 GHz) zajmuje jakies 4% razem z tona
   dodatkowych testow (pierwotnie to bylo tylko szukanie wyciekow, teraz
   jeszcze sprawdza, czy nie psuja sie listy). */
#undef DEBUG_SSM
/* Ten sam efekt mozna osiagnac za pomoca valgrinda, ale do tego nalezy
   wylaczyc zupelnie SSM (tzn. wylaczyc mu uzycie sterty, zeby od razu
   przeszedl w tryb przepelnionej i wszystko alokowal w systemie). */

/* Lam 31.12.2007: Valgrind (a dokladnie valgrind --tool=memcheck) ma pomocne
   testy wyciekow pamieci, jednak nasze metody alokacji pamieci "z gory"
   i dzielenia jej pozniej udaremniaja wszelkie proby pomocy ze strony
   Valgrinda. Wlacz ALLOC_VALGRIND, aby przejsc w tryb alokacji wylacznie
   funkcjami systemowymi. */
#undef ALLOC_VALGRIND


/*
 * String and memory management parameters.
 */
#define MAX_VNUM               131072 /* musi sie dzielic przez 8! */
#define MAX_KEY_HASH             1024
#define MAX_STRING_LENGTH        4096
#define MAX_INPUT_LENGTH          320 /* Lam: bylo 160 */
/* przydatne skroty */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH

#define CONN_USER_LEN		  128
#define CONN_TERM_LEN		  128

/* limity bufora wyjsciowego */
#define BUF_MIN_PREDKOSC	 1024
#define BUF_PROG_PRZEPELNIENIA	32000
#define BUF_LIMIT_ROZMIARU     128000
/* wielkosc bufora wejsciowego */
#define BUF_LIMIT_INBUF           MSL /* bylo MIL * 4 */

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
/* ilosc pamieci zajmowanej przez sterte na lancuchy, w kilobajtach */
#define SSM_MEM_SIZE		 5760	/* Used in ssm.c */

#define EXP_PER_LEVEL            1000
#define MAX_SKILL                 321
#define MAX_CLASS                  11
#define MAX_RACE		  125
#define MAX_LEVEL                 110	/* ilosc poziomow. Zmiana tu nie wystarczy */

#define MAX_HINT		  128	/* maks. liczba podpowiedzi */
#define MAX_QUIT		   64	/* maks. liczba komunikatow wyjscia */
#define MAX_TRACK		  128	/* maks. liczba pamietanych drog */
#define MAX_POSE		   64   /* maks. liczba poz dla jednej profesji */
#define MAX_OFFENSIVE		 2048	/* maks. liczba bluzgow */
#define MAX_MIODEK		 2048	/* maks. liczba slow Miodka */
#define MAX_OSTATNIE		   10	/* Tissarius */
/*#define MAX_MASKOWANIE		   16    maks. liczba opisow maskowania */
#define MAX_COLOUR		   17	/* Lam - ilosc kodow kolorow */
#define MAX_POWODY		   16	/* ilosc powodow blokady imienia */
#define MAX_DLUG_IMIENIA	   12	/* imiona graczy maja od 3 do 12 liter */

#define MAX_DAYS                    7   /* ilosc zapamietywanych statystyk */

#define L_DIR                     108
#define L_SEN                     106   /* 106 to juz bogowie */
#define L_JUN                     103
#define L_APP                     101	/* od 101 da sie cwiczyc wszystkie um. */
#define LEVEL_IMMORTAL            103	/* od 103 jest sie niesmiertelnym */
#define LEVEL_HERO                100	/* od 100 nie zdobywa sie dosw. */
#define LEVEL_STOPIEN		  105	/* od 105 widac stopien */

#define PULSE_PER_SECOND            4
#define PULSE_VIOLENCE            (  2 * PULSE_PER_SECOND )
#define PULSE_MOBILE              (  5 * PULSE_PER_SECOND )
#define PULSE_TICK                ( 30 * PULSE_PER_SECOND )
#define PULSE_AREA                ( 60 * PULSE_PER_SECOND )
#define PULSE_AUCTION		  ( 10 * PULSE_PER_SECOND )

/* odwleczenie wychodzenia po wcieleniu/ataku */
#define ODWLECZENIE		  ( 15 * PULSE_PER_SECOND )

#define MAX_MATERIAL		   75

#define LIQ_WATER        0
#define LIQ_BLOOD	13
#define LIQ_MAX         50

#define CLASS_MAG		0
#define CLASS_KLERYK		1
#define CLASS_ZLODZIEJ		2
#define CLASS_WOJOWNIK		3
#define CLASS_PSIONIK		4
#define CLASS_SZPIEG		5
#define CLASS_SZAMAN		6
#define CLASS_SKRYTOBOJCA	7
#define CLASS_MNICH		8
#define CLASS_NEKROMANTA        9
#define CLASS_PALADYN    	10

/* ktory wiersz tablicy polska_tablica[ ] zawiera UTF-8 */
#define POLSKIE_UTF_8		4


/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 *
 * Lam 13.6.98: DATA_DIR do trzymania wszystkiego zamiast katalogu area
 * Lam 22.8.98: BACKUP_DIR na kopie zapasowe plikow postaci
 */
#if defined( MSDOS )
#define PLAYER_DIR      "..\\player\\"	/* Player files                 */
#define BACKUP_DIR      "..\\backup\\"
#define CLASS_DIR       "..\\class\\"	/* Klasy                        */
/* DJGPP zdaje sie miec takie wbudowane bajery, Lam 1.3.2006 zamiast proto.are */
#define NULL_FILE       "/dev/null"	/* To reserve one stream        */
#define MOB_DIR         "MOBProgs\\"	/* MOBProg files                */
#define DATA_DIR        "..\\data\\"
#define DESC_DIR        "..\\data\\opisy\\"
#define CLAN_DIR        "..\\data\\klany\\" /* Klany Malvena */
#endif

#if !defined( MSDOS ) && ( defined( unix ) || defined( linux ) )
#define PLAYER_DIR      "../player/"    /* Player files                 */
#define BACKUP_DIR      "../backup/"	/* Kopie plikow postaci -Lam    */
#define CLASS_DIR       "../class/"     /* Klasy                        */
#define NULL_FILE       "/dev/null"     /* To reserve one stream        */
#define MOB_DIR         "MOBProgs/"     /* MOBProg files                */
#define DATA_DIR        "../data/"
#define DESC_DIR        "../data/opisy/"
#define CLAN_DIR        "../data/klany/" /* Klany Malvena */
#define HTTPDLOG_DIR	"../log/httpd/"	/* logowanie httpd - Ulryk	*/
#endif

#if defined( AmigaTCP )
#define PLAYER_DIR      "envy:player/"  /* Player files                 */
#define BACKUP_DIR      "envy:backup/"
#define CLASS_DIR       "envy:class/"   /* Klasy                        */
#define NULL_FILE       "proto.are"     /* To reserve one stream        */
#define MOB_DIR         "MOBprogs/"     /* MOBProg files                */
#define DATA_DIR        "envy:data/"
#define DESC_DIR        "envy:data/opisy/"
#define CLAN_DIR        "envy:data/klany/" /* Klany Malvena */
#define HTTPDLOG_DIR	"envy:log/httpd/"
#endif

#if defined( WIN32 )
#define PLAYER_DIR      "..\\player\\"  /* Player files                 */
#define BACKUP_DIR      "..\\backup\\"
#define CLASS_DIR       "..\\class\\"   /* Klasy                        */
#define MOB_DIR         "MOBprogs\\"    /* MOBProg files                */
#define NULL_FILE       "nul"           /* To reserve one stream        */
#define DATA_DIR        "..\\data\\"
#define DESC_DIR        "..\\data\\opisy\\"
#define CLAN_DIR        "..\\data\\klany\\" /* Klany Malvena */
#define HTTPDLOG_DIR	"..\\log\\httpd\\"
#endif


/*
 * Malven, 12.11.2003 - Wykorzystuje do zakladania katalogow klanowych.
 * Tutaj nalezy dopisac odpowiednie wpisy w przypadku uruchamiania Laca
 * na innych systemach.
 */
#if defined( MSDOS ) || defined( WIN32 )
#define DIR_SEPARATOR	"\\"
#define MKDIR_CMD	"md \"%s\""
#endif

#if !defined( MSDOS ) && ( defined( linux ) || defined( unix ) )
#define DIR_SEPARATOR	"/"
#define MKDIR_CMD	"mkdir \"%s\""
#endif

#if defined( AmigaTCP )
#define DIR_SEPARATOR	"/"
#define MKDIR_CMD	"makedir \"%s\""
#endif

/* Tu zapisuja sie krainy w save_area( ). */
#define SAVE_DIR	"save" DIR_SEPARATOR


#define AREA_LIST	"AREA.LST"		/* Lista krain		*/
#define AVNUM_FILE	DATA_DIR "vnums.are"	/* do_vnumy		*/
#define BAN_FILE	DATA_DIR "ban.txt"	/* For banned site save	*/
#define BUG_FILE	DATA_DIR "BUGS.TXT"	/* bug( )		*/
#define CORPSE_FILE	DATA_DIR "corpse.txt"	/* zapisywanie cial	*/
#define DOWN_TIME_FILE	DATA_DIR "jebut.txt"	/* For auto-shutdown	*/
#define FIX_EXITS_FILE	DATA_DIR "fixexits.txt"
#define HINT_FILE	DATA_DIR "hints.txt"	/* podpowiedzi		*/
#define IDEA_FILE	DATA_DIR "IDEAS.TXT"	/* For 'idea'		*/
#define NOTE_FILE	DATA_DIR "notes.txt"	/* For 'notes'		*/
#define OLD_NOTE_FILE	DATA_DIR "notes.old"	/* Stare noty		*/
#define PCBUG_FILE	DATA_DIR "PCBUGS.TXT"	/* For 'bug'		*/
#define PROGBUG_FILE	DATA_DIR "PROGBUGS.TXT" /* progbug( )		*/
#define PRZYP_FILE	DATA_DIR "ODMIANA.TXT"	/* logowanie odmiany	*/
#define QUIT_FILE	DATA_DIR "wyjdz.txt"	/* komunikaty wyjscia	*/
#define RACESTAT_FILE	DATA_DIR "rasy.txt"	/* do_racestat		*/
#define SCHEMATY_FILE	DATA_DIR "schematy.txt"	/* schematy barw	*/
#define SHUTDOWN_FILE	"SHUTDOWN.TXT"		/* For 'shutdown'	*/
#define SKILL_FILE	DATA_DIR "umiej.txt"	/* do_slookup		*/
#define SSM_FILE	DATA_DIR "ssm.txt"	/* wyjscie bledow SSM-a	*/
#define SYSTEM_FILE	DATA_DIR "system.txt"	/* rekordy, wiek muda	*/
#define NSYSTEM_FILE	DATA_DIR "system.now"	/* podczas zapisu	*/
#define TRACK_FILE	DATA_DIR "drogi.txt"	/* drogi do krain	*/
#define TYPO_FILE	DATA_DIR "TYPOS.TXT"	/* For 'typo'		*/
#define VNUM_FILE	DATA_DIR "vnums.txt"	/* predefiniowane vnumy */
#define ZONE_FILE	DATA_DIR "strefy.txt"	/* strefy		*/
#define ZONES_FILE	DATA_DIR "strefy.inf"	/* info o strefach	*/
#define OFFENSIVE_FILE	DATA_DIR "bluzg.txt"	/* wulgaryzmy		*/
#define MIODEK_FILE	DATA_DIR "miodek.txt"	/* Miodek		*/
#define PSZUKAJ_FILE	DATA_DIR "pszukaj.txt"	/* pszukaj -plik	*/
#define BLEDOMAT_FILE	DATA_DIR "bledomat.txt"	/* ustawienia bledomatu */
#define IMIONA_FILE	DATA_DIR "imlista.txt"	/* blokady imion	*/
#define IMNOWE_FILE	DATA_DIR "imnowe.txt"	/* jw.			*/
#define POWODY_FILE	DATA_DIR "impowod.txt"	/* jw.			*/
#define LSC_FILE	DATA_DIR "lsc.txt"	/* konfiguracja LSC	*/
/*#define MASKOWANIE_FILE DESC_DIR "mask.txt"      maskowanie           */

/*
 * Klany Malvena:
 * Pliki kazdego klanu trzymane sa w osobnym katalogu CLAN_DIR/clan->dir/
 * gdzie clan->dir to nazwa katalogu klanu.
 */
#define CLAN_DANE_FILE	         "dane.cln"     /* podstawowe dane      */
#define CLAN_STATUT_FILE         "statut.cln"   /* statut klanu         */
#define CLAN_LISTA_FILE          "lista.cln"    /* lista czlonkow       */
#define CLAN_STOSUNKI_FILE       "stosunki.cln" /* stos. miedzyklanowe  */
#define CLAN_SKRZYNIA_FILE       "skrzynia.cln" /* skrzynia klanowa     */
#define CLAN_LOG_FILE            "log.cln"      /* log klanowy          */

#define CLAN_KLANY_FILE CLAN_DIR "klany.lst"    /* Lista klanow         */

/* ponizej stare - usunac, kiedy juz bedzie mozna */
#define CLAN_PITS	DATA_DIR "clan.obj"	/* Urny klanowe		*/
