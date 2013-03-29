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
 *  Polecenia dla niesmiertelnych. Boskie umiejetnosci. Manipulacje        *
 *  wszelkimi strukturami muda, graczami, zachowaniami serwera, funkcje    *
 *  sprawdzajace stan wielu rzeczy i pozwalajace miec na nie wiekszy       *
 *  wplyw. Dla graczy tajemnica.                                           *
 * ----------------------------------------------------------------------- *
 *  Teraz act_wiz.c takze jest po polsku! W czerwcu spolszczylem nazwy     *
 *  polecen, a dzisiaj, 13.11.1999, przetlumaczylem wiekszosc komunikatow  *
 *  (200 sztuk). Zostawilem sporo w do_mset, do_oset, i w kilku miejscach  *
 *  nazwy flag/pol/umiejetnosci. Ale i to sie kiedys zmieni.               *
 *  13.5.2000: Qwert podeslal wersje prawie calkiem polska.                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "merc.h"

#if !defined( WIN32 )
#include <sys/time.h>
#else
int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
#endif

int	co_ma_zmieniac		args( ( char *gdzie, bool pszuk ) ); /* dla do_oaff */
void	ustaw_wartosc		args( ( OBJ_DATA *obj, int ktora, char *arg,
					CHAR_DATA *ch ) ); /* dla do_oset */
void	real_goto		args( ( CHAR_DATA *ch,
					ROOM_INDEX_DATA *location ) );
char	*conn_str		args( ( int conn ) );
char	*opisz_przedmiot	args( ( CHAR_DATA *ch, OBJ_DATA *obj,
					int obj_counter ) );
OBJ_DATA *obj_clon		args( ( OBJ_DATA* tmp_obj, bool all ) );
int	znajdz_powod		args( ( char *arg ) );
void	rng_test		args( ( CHAR_DATA *ch ) );

/* Lam 17.6.2003 dla fstata */
/* Lam 21.8.2003: tu nie mialo byc \n na koncu */
char *ascdata( const struct tm *timeptr )
{
    static char result[ 128 ];

    (void) sprintf( result, "%d.%d.%d",
	timeptr->tm_mday, timeptr->tm_mon + 1, 1900 + timeptr->tm_year );

    return result;
}


/* Conversion of Immortal powers to Immortal skills done by Thelonius */
KOMENDA( do_wizhelp )
{
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];
    char       buf1[ MAX_STRING_LENGTH ];
    int        cmd;
    int        col;

    rch = get_char( ch );

    if ( !authorized( rch, "bumi" ) )
	return;

    buf1[ 0 ] = '\0';
    col       = 0;

    for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
    {
	if ( cmd_table[ cmd ].level < LEVEL_HERO
	    || !is_name( cmd_table[ cmd ].name, rch->pcdata->immskll ) )
	{
	    continue;
	}

	sprintf( buf, "%s", wyrownaj( cmd_table[ cmd ].name, -11 ) );
	strcat( buf1, buf );

	if ( ++col % 7 == 0 )
	    strcat( buf1, "\n\r" );
    }

    if ( col % 7 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );

    return;
}


/*
 * Szpicel - pomysl sciagniety z ROM-a przez Lama (po czesci rowniez wykonanie)
 */
KOMENDA( do_wiznet )
{
    char arg[ MAX_STRING_LENGTH ];
    CHAR_DATA *rch;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Jeste`s mobem. Moby nie mog`a u`zywa`c szpicla.\n\r", ch );
	return;
    }

    rch = get_char( ch );

    if ( !authorized( rch, "szpicel" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	if ( IS_SET( ch->pcdata->wiznet, WIZ_ON ) )
	{
	    send_to_char( "[ Zdarzenie] Znaczenie.\n\r", ch );

	    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_LEVEL )
		? "[+POZIOM   ] Widzisz, gdy kto`s zdobywa poziom.\n\r"
		: "[-poziom   ] Masz gdzie`s zdobywanie poziom`ow przez graczy.\n\r", ch );

	    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_NOWI )
		? "[+NOWI     ] Widzisz, kiedy kto`s zak`lada now`a posta`c.\n\r"
		: "[-nowi     ] Nie widzisz zak`ladania nowych postaci.\n\r", ch );

	    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_LOGIN )
		? "[+WJAZD    ] Widzisz, kiedy gracze wchodz`a/wychodz`a z gry.\n\r"
		: "[-wjazd    ] Nie widzisz wchodzenia/wychodzenia graczy z gry.\n\r", ch );

	    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_LINKS )
		? "[+PO`L`ACZ   ] Widzisz, gdy kto`s traci/odzyskuje po`l`aczenie.\n\r"
		: "[-po`l`acz   ] Nie rusza ci`e tracenie/odzyskiwanie po`l`acze`n.\n\r", ch );

	    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_DEATH )
		? "[+`SMIER`C   ] Widzisz `smier`c graczy.\n\r"
		: "[-`smier`c   ] Nie martwisz si`e `smierci`a graczy.\n\r", ch );

	    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_MURDER )
		? "[+ZAB`OJCY  ] Widzisz perypetie zab`ojc`ow (zab`ojstwa i `sci`aganie).\n\r"
		: "[-zab`ojcy  ] Nie interesuj`a ci`e przygody zab`ojc`ow.\n\r", ch );

	    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_IDEA )
		? "[+POMYS`LY  ] Widzisz u`zycie polece`n pomys`l/liter`owka/b`l`ad.\n\r"
		: "[-pomys`ly  ] Nie obchodzi ci`e zdanie graczy o mudzie.\n\r", ch );

	    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_ODM )
		? "[+ODMIANA  ] Otrzymujesz powiadomienia o zmianach odmiany imion graczy.\n\r"
		: "[-odmiana  ] Nie chcesz wiedzie`c, kiedy gracze odmieniaj`a swe imiona.\n\r", ch );

	    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_DEBUG )
		? "[+BZDETY   ] Widzisz r`o`zne informacje o pracy serwera.\n\r"
		: "[-bzdety   ] Lac niech sobie co`s robi, nie musisz wiedzie`c.\n\r", ch );

	    if ( get_trust( rch ) >= 106 )
	    {
		send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_BUG )
		    ? "[+B`L`AD     ] Widzisz b`l`edy Laca.\n\r"
		    : "[-b`l`ad     ] Od naprawiania b`l`ed`ow Laca s`a inni.\n\r", ch );

		send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_PROGBUG )
		    ? "[+PROGBUG  ] Widzisz b`l`edy prog`ow zg`laszane przez progbug( ).\n\r"
		    : "[-progbug  ] Nie obchodz`a ci`e cudze progi, a twoje s`a bezb`l`edne.\n\r", ch );
	    }

	    if ( get_trust( rch ) >= L_DIR )
	    {
		send_to_char( "Zg`laszanie u`zywania polece`n:\n\r", ch );

		if ( IS_SET( ch->pcdata->wiznet, WIZ_POL_SMIE ) )
		    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_POL_SMWSZ )
			? "  Wszystkie polecenia: wszystkie.\n\r"
			: "  Wszystkie polecenia: logowane.\n\r", ch );
		else
		    send_to_char( "  Wszystkie polecenia: wcale.\n\r", ch );

		if ( IS_SET( ch->pcdata->wiznet, WIZ_POL_BOH ) )
		    send_to_char( IS_SET( ch->pcdata->wiznet, WIZ_POL_BOHWSZ )
			? "  Polecenia bohater`ow: wszystkie.\n\r"
			: "  Polecenia bohater`ow: logowane.\n\r", ch );
		else
		    send_to_char( "  Polecenia bohater`ow: wcale.\n\r", ch );

		if ( !*ch->pcdata->szpic_wybr )
		    send_to_char( "  Wybrane polecenia: (brak).\n\r", ch );
		else
		    ch_printf( ch, "  Wybrane polecenia: %s.\n\r", ch->pcdata->szpic_wybr );

		if ( !*ch->pcdata->szpic_wykl )
		    send_to_char( "  Wykluczone polecenia: (brak).\n\r", ch );
		else
		    ch_printf( ch, "  Wykluczone polecenia: %s.\n\r", ch->pcdata->szpic_wykl );

		if ( IS_SET( ch->pcdata->wiznet, WIZ_POL_PROG ) )
		    send_to_char( "  Polecenia wykonywane podczas prog`ow: widoczne.\n\r", ch );
		else
		    send_to_char( "  Polecenia wykonywane podczas prog`ow: ukryte.\n\r", ch );
	    }

	    send_to_char( "\n\rMasz w`l`aczonego szpicla. Aby go wy`l`aczy`c napisz \"szpicel wy`l`acz\".\n\r", ch );
	}
	else send_to_char( "Masz wy`l`aczonego szpicla. Aby go w`l`aczy`c napisz \"szpicel w`l`acz\".\n\r", ch );

	return;
    }
    else
    {
	char buf[ MAX_STRING_LENGTH ];
	int  bit;
	bool fSet;

	if ( !*arg )
	{
	    send_to_char( "Sk`ladnia: szpicel [ w`l`acz | wy`l`acz | -zdarzenie | +zdarzenie | polecenia ... ]\n\r", ch );
	    return;
	}
	else if ( arg[ 0 ] == '+' )
	{
	    fSet = TRUE;
	}
	else if ( arg[ 0 ] == '-' )
	{
	    fSet = FALSE;
	}
	else if ( !str_prefix( arg, "w`l`acz" ) )
	{
	    send_to_char( "Szpicel obiecuje sprawowa`c si`e dobrze.\n\r", ch );
	    SET_BIT( ch->pcdata->wiznet, WIZ_ON );
	    return;
	}
	else if ( !str_prefix( arg, "wy`l`acz" ) )
	{
	    send_to_char( "Szpiclowi przykro jest, `ze ci`e zawi`od`l.\n\r", ch );
	    REMOVE_BIT( ch->pcdata->wiznet, WIZ_ON );
	    return;
	}
	else if ( !str_prefix( arg, "polecenia" ) && get_trust( rch ) >= L_DIR )
	{
	    char *bzdet;
	    const CMD_TYPE *polec;
	    char nazpol[ MIL ];

	    argument = one_argument( argument, arg );

	    if ( !*arg )
		send_to_char( "Sk`ladnia: szpicel polecenia [ wszystkie ... | bohater`ow ... | + ... | - ... ]\n\r", ch );

	    else if ( arg[ 0 ] == '+' )
	    {
		if ( arg[ 1 ] != '\0' )
		{
		    bzdet = &arg[ 1 ];
		    strcpy( buf, bzdet );
		    one_argument( buf, arg );
		    bzdet = arg;
		}
		else
		{
		    one_argument( argument, arg );
		    bzdet = arg;
		}

		if ( !*bzdet )
		{
		    send_to_char( "Jakie polecenie chcesz wybra`c?\n\r", ch );
		    return;
		}

		if ( !( polec = znajdz_polecenie( bzdet, MAX_LEVEL ) ) )
		{
		    ch_printf( ch, "Nie znam polecenia \"%s\".\n\r", bzdet );
		    return;
		}

		if ( is_name2( polec->name, ch->pcdata->szpic_wykl ) )
		{
		    char bufek[ MSL ];
		    bool bylo = FALSE;

		    bzdet = ch->pcdata->szpic_wykl;
		    strcpy( bufek, "" );
		    for ( ; ; )
		    {
			bzdet = one_argument2( bzdet, arg );

			if ( !arg[ 0 ] )
			    break;

			if ( str_cmp( arg, polec->name ) )
			{
			    if ( bylo )
				strcat( bufek, " " );

			    if ( *arg == '\'' )
				strcpy( nazpol, "\"'\"" );
			    else if ( *arg == '"' )
				strcpy( nazpol, "'\"'" );
			    else
				strcpy( nazpol, arg );

			    strcat( bufek, nazpol );
			    bylo = TRUE;
			}
		    }
		    free_string( ch->pcdata->szpic_wykl );
		    ch->pcdata->szpic_wykl = str_dup( bufek );
		    ch_printf( ch, "Usuwasz polecenie \"%s\" z wykluczonych.\n\r", polec->name );
		    return;
		}

		if ( is_name2( polec->name, ch->pcdata->szpic_wybr ) )
		{
		    ch_printf( ch, "Polecenie \"%s\" masz ju`z w`sr`od wybranych.\n\r", polec->name );
		    return;
		}

		/* tu ewentualnie mogloby sie znalezc sprawdzenie, czy
		   polecenie jest juz wybrane przez wsz/boh wsz/log, ale teraz
		   mozna wybrac polecenia, ktore zostana, kiedy zrobie na
		   moment wsz/boh wcale, bo mi cos bedzie przeszkadzalo */
		if ( *polec->name == '\'' )
		    strcpy( nazpol, "\"'\"" );
		else if ( *polec->name == '"' )
		    strcpy( nazpol, "'\"'" );
		else
		    strcpy( nazpol, polec->name );

		if ( *ch->pcdata->szpic_wybr )
		{
		    char bufek[ MSL ];

		    strcpy( bufek, ch->pcdata->szpic_wybr );
		    strcat( bufek, " " );
		    strcat( bufek, nazpol );
		    free_string( ch->pcdata->szpic_wybr );
		    ch->pcdata->szpic_wybr = str_dup( bufek );
		}
		else
		{
		    free_string( ch->pcdata->szpic_wybr );
		    ch->pcdata->szpic_wybr = str_dup( nazpol );
		}

		ch_printf( ch, "Wybierasz polecenie \"%s\".\n\r", polec->name );
		return;
	    }
	    else if ( arg[ 0 ] == '-' )
	    {
		if ( arg[ 1 ] != '\0' )
		{
		    bzdet = &arg[ 1 ];
		    strcpy( buf, bzdet );
		    one_argument( buf, arg );
		    bzdet = arg;
		}
		else
		{
		    one_argument( argument, arg );
		    bzdet = arg;
		}

		if ( !*bzdet )
		{
		    send_to_char( "Jakie polecenie chcesz wykluczy`c?\n\r", ch );
		    return;
		}

		if ( !( polec = znajdz_polecenie( bzdet, MAX_LEVEL ) ) )
		{
		    ch_printf( ch, "Nie znam polecenia \"%s\".\n\r", bzdet );
		    return;
		}

		if ( is_name2( polec->name, ch->pcdata->szpic_wybr ) )
		{
		    char bufek[ MSL ];
		    bool bylo = FALSE;

		    bzdet = ch->pcdata->szpic_wybr;
		    strcpy( bufek, "" );

		    for ( ; ; )
		    {
			bzdet = one_argument2( bzdet, arg );
			if ( !arg[ 0 ] )
			    break;
			if ( str_cmp( arg, polec->name ) )
			{
			    if ( bylo )
				strcat( bufek, " " );

			    if ( *arg == '\'' )
				strcpy( nazpol, "\"'\"" );
			    else if ( *arg == '"' )
				strcpy( nazpol, "'\"'" );
			    else
				strcpy( nazpol, arg );

			    strcat( bufek, nazpol );
			    bylo = TRUE;
			}
		    }

		    free_string( ch->pcdata->szpic_wybr );
		    ch->pcdata->szpic_wybr = str_dup( bufek );
		    ch_printf( ch, "Usuwasz polecenie \"%s\" z wybranych.\n\r", polec->name );
		    return;
		}

		if ( is_name2( polec->name, ch->pcdata->szpic_wykl ) )
		{
		    ch_printf( ch, "Polecenie \"%s\" masz ju`z w`sr`od wykluczonych.\n\r", polec->name );
		    return;
		}

		if ( *polec->name == '\'' )
		    strcpy( nazpol, "\"'\"" );
		else if ( *polec->name == '"' )
		    strcpy( nazpol, "'\"'" );
		else
		    strcpy( nazpol, polec->name );

		if ( *ch->pcdata->szpic_wykl )
		{
		    char bufek[ MSL ];

		    strcpy( bufek, ch->pcdata->szpic_wykl );
		    strcat( bufek, " " );
		    strcat( bufek, nazpol );
		    free_string( ch->pcdata->szpic_wykl );
		    ch->pcdata->szpic_wykl = str_dup( bufek );
		}
		else
		{
		    free_string( ch->pcdata->szpic_wykl );
		    ch->pcdata->szpic_wykl = str_dup( nazpol );
		}

		ch_printf( ch, "Wykluczasz polecenie \"%s\".\n\r", polec->name );
		return;
	    }
	    else if ( !str_prefix( arg, "wszystkie" ) )
	    {
		one_argument( argument, arg );

		if ( !str_prefix( arg, "wcale" ) )
		{
		    REMOVE_BIT( ch->pcdata->wiznet, WIZ_POL_SMIE );
		    send_to_char( "Widok wszystkich polece`n wy`l`aczony.\n\r", ch );
		}
		else if ( !str_prefix( arg, "logowane" ) )
		{
		    SET_BIT( ch->pcdata->wiznet, WIZ_POL_SMIE );
		    REMOVE_BIT( ch->pcdata->wiznet, WIZ_POL_SMWSZ );
		    send_to_char( "W`l`aczony widok wszystkich logowanych polece`n.\n\r", ch );
		}
		else if ( !str_prefix( arg, "wszystkie" ) )
		{
		    SET_BIT( ch->pcdata->wiznet, WIZ_POL_SMIE );
		    SET_BIT( ch->pcdata->wiznet, WIZ_POL_SMWSZ );
		    send_to_char( "W`l`aczony widok WSZYSTKICH polece`n!\n\r", ch );
		}
		else
		    send_to_char( "Sk`ladnia: szpicel polecenia wszystkie < wcale | logowane | wszystkie >\n\r", ch );

		return;
	    }
	    else if ( !str_prefix( arg, "bohater`ow" ) )
	    {
		one_argument( argument, arg );

		if ( !str_prefix( arg, "wcale" ) )
		{
		    REMOVE_BIT( ch->pcdata->wiznet, WIZ_POL_BOH );
		    send_to_char( "Widok polece`n bohater`ow wy`l`aczony.\n\r", ch );
		}
		else if ( !str_prefix( arg, "logowane" ) )
		{
		    SET_BIT( ch->pcdata->wiznet, WIZ_POL_BOH );
		    REMOVE_BIT( ch->pcdata->wiznet, WIZ_POL_BOHWSZ );
		    send_to_char( "W`l`aczony widok logowanych polece`n bohater`ow.\n\r", ch );
		}
		else if ( !str_prefix( arg, "wszystkie" ) )
		{
		    SET_BIT( ch->pcdata->wiznet, WIZ_POL_BOH );
		    SET_BIT( ch->pcdata->wiznet, WIZ_POL_BOHWSZ );
		    send_to_char( "W`l`aczony widok wszystkich polece`n bohater`ow.\n\r", ch );
		}
		else
		    send_to_char( "Sk`ladnia: szpicel polecenia wszystkie < wcale | logowane | wszystkie >\n\r", ch );

		return;
	    }
	    else if ( !str_prefix( arg, "progowe" ) )
	    {
		one_argument( argument, arg );

		if ( !str_prefix( arg, "ukryte" )
		  || !str_prefix( arg, "nie" ) )
		{
		    REMOVE_BIT( ch->pcdata->wiznet, WIZ_POL_PROG );
		    send_to_char( "Polecenia wykonywane podczas prog`ow ukryte.\n\r", ch );
		}
		else if ( !str_prefix( arg, "widoczne" )
		       || !str_prefix( arg, "tak" ) )
		{
		    SET_BIT( ch->pcdata->wiznet, WIZ_POL_PROG );
		    send_to_char( "Polecenia wykonywane podczas prog`ow widoczne.\n\r", ch );
		}
		else
		    send_to_char( "Sk`ladnia: szpicel polecenia progowe < widoczne | ukryte >\n\r", ch );

		return;
	    }
	    else
		send_to_char( "Sk`ladnia: szpicel polecenia [ wszystkie ... | bohater`ow ... | + ... | - ... ]\n\r", ch );

	    return;
	}
	else
	{
	    send_to_char( "Sk`ladnia: szpicel [ w`l`acz | wy`l`acz | -zdarzenie | +zdarzenie | polecenia ... ]\n\r", ch );
	    return;
	}

	if ( !arg[ 1 ] )
	{
	    STC( "Kt`ore zdarzenie prze`l`aczy`c?\n\r", ch );
	    return;
	}
	else if ( !str_prefix( arg + 1, "poziom" ) )	bit = WIZ_LEVEL;
	else if ( !str_prefix( arg + 1, "nowi" ) )	bit = WIZ_NOWI;
	else if ( !str_prefix( arg + 1, "wjazd" ) )	bit = WIZ_LOGIN;
	else if ( !str_prefix( arg + 1, "po`l`acz" ) )	bit = WIZ_LINKS;
	else if ( !str_prefix( arg + 1, "`smier`c" ) )	bit = WIZ_DEATH;
	else if ( !str_prefix( arg + 1, "zab`ojcy" ) )	bit = WIZ_MURDER;
	else if ( !str_prefix( arg + 1, "pomys`ly" ) )	bit = WIZ_IDEA;
	else if ( !str_prefix( arg + 1, "bzdety" ) )	bit = WIZ_DEBUG;
	else if ( !str_prefix( arg + 1, "odmiana" ) )	bit = WIZ_ODM;
	else if ( !str_prefix( arg + 1, "b`l`ad" )
	       && ( get_trust( ch ) >= 106 ) )		bit = WIZ_BUG;
	else if ( !str_prefix( arg + 1, "progbug" )
	       && ( get_trust( ch ) >= 106 ) )		bit = WIZ_PROGBUG;
	else
	{
	    send_to_char( "Nie znam takiego zdarzenia.\n\r", ch );
	    return;
	}

	if ( fSet )
	{
	    SET_BIT( ch->pcdata->wiznet, bit );
	    sprintf( buf, "Zdarzenie \"%s\" masz W`L`ACZONE.\n\r", arg + 1 );
	    send_to_char( buf, ch );
	}
	else
	{
	    REMOVE_BIT( ch->pcdata->wiznet, bit );
	    sprintf( buf, "Zdarzenie \"%s\" masz WY`L`ACZONE.\n\r", arg + 1 );
	    send_to_char( buf, ch );
	}
    }

    return;
}


/* ta funkcja sciagnieta niemal zywcem, a potem ulepszona wiele razy */
void wiznet( const char *string, CHAR_DATA *ch, OBJ_DATA *obj, void *arg,
		int flag, int no_flag, int min_level )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *rch;
    CMD_TYPE *cmd = (CMD_TYPE *) arg;
    bool pokaz;
    int zauf;
    char linia[ MSL ]; /* choc wydaje mi sie, ze 2 * MIL powinno wystarczyc */

    sprintf( linia, "{B<{YSzpicel{B>{R:{x %s", string );

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected != CON_PLAYING
	  || !d->character
	  || d->character->deleted )
	{
	    continue;
	}

	rch = get_char( d->character );
	zauf = get_trust( rch );

	if ( ( flag == WIZ_IMMCMD )
	  && IS_HERO( rch )
	  && IS_SET( rch->pcdata->wiznet, WIZ_ON )
	  && ( rch != ch )
	  && ( zauf >= L_DIR )
	  && ( zauf >= min_level ) )
	{
	    if ( prog_nest && !IS_SET( rch->pcdata->wiznet, WIZ_POL_PROG ) )
		pokaz = FALSE;
	    else if ( is_name2( cmd->name, rch->pcdata->szpic_wykl ) )
		pokaz = FALSE;
	    else if ( is_name2( cmd->name, rch->pcdata->szpic_wybr ) )
		pokaz = TRUE;
	    else if ( cmd->level >= 100 )
	    {
		if ( !IS_SET( rch->pcdata->wiznet, WIZ_POL_BOH ) )
		    pokaz = FALSE;
		else if ( IS_SET( rch->pcdata->wiznet, WIZ_POL_BOHWSZ ) )
		    pokaz = TRUE;
		else if ( cmd->log ) /* na pewno ALWAYS, NEVER tu nie dotrze */
		    pokaz = TRUE;
		else
		    pokaz = FALSE;
	    }
	    else
	    {
		if ( !IS_SET( rch->pcdata->wiznet, WIZ_POL_SMIE ) )
		    pokaz = FALSE;
		else if ( IS_SET( rch->pcdata->wiznet, WIZ_POL_SMWSZ ) )
		    pokaz = TRUE;
		else if ( cmd->log )
		    pokaz = TRUE;
		else
		    pokaz = FALSE;
	    }

	    if ( pokaz )
		super_act( 0, ZM_ZAWSZE, linia, d->character, obj, ch, TO_CHAR, TRUE );
	}
	else if ( ( flag != WIZ_IMMCMD )
	  && IS_HERO( rch )
	  && IS_SET( rch->pcdata->wiznet, WIZ_ON )
	  && ( !flag || IS_SET( rch->pcdata->wiznet, flag ) )
	  && ( !no_flag || !IS_SET( rch->pcdata->wiznet, no_flag ) )
	  && ( zauf >= min_level )
	  && ( rch != ch ) )
	{
	    super_act( 0, ZM_ZAWSZE, linia, d->character, obj, ch, TO_CHAR, TRUE );
	}
    }

    return;
}


/*
 * Lam
 * Qwert 02.11.2000: tablica flag
 */
KOMENDA( do_info )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1[ MAX_INPUT_LENGTH ];
    int        bit = 0;
    int        tmp;
    bool       fSet;
    static struct
    {
	char *flaga;
	int bit;
    }
    inform[ ] =
    {
	{ "budowniczy",	INF_BUDOWNICZY },
	{ "zada`ndajec",INF_QUESTMASTER },
	{ "laik",	INF_LAMER },
	{ "mors",	INF_MORS },
	{ "spk",	INF_CLAN_GUARD },
	{ "stopie`n",	INF_STOPIEN_SHOW },
	{ "kartograf",	INF_KARTOGRAF },
	{ "lacsakra",	INF_LACSAKRA },
	{ "", 0 }
    };

    rch = get_char( ch );

    if ( !authorized( rch, "info" ) )
	return;

    argument = one_argument( argument, arg1 );

    if ( arg1[ 0 ] == '\0' || argument[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: info <posta`c> <+/-><t`lumacz|xphile|budowniczy|zada`ndajec|laik|mors|spk|stopie`n|kartograf|lacsakra>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie na mobach.\n\r", ch );
	return;
    }

    if ( argument[ 0 ] == '+' )
	fSet = TRUE;
    else if ( argument[ 0 ] == '-' )
	fSet = FALSE;
    else
	return;

    while ( isspace( (int) *++argument ) )
	;

    if ( !*argument )
    {
	send_to_char( "Sk`ladnia: info <posta`c> <+/-><t`lumacz|xphile|budowniczy|zada`ndajec|laik|mors|spk|stopie`n|kartograf|lacsakra>.\n\r", ch );
	return;
    }

    for ( tmp = 0; inform[ tmp ].bit; tmp++ )
	if ( !str_prefix( argument, inform[ tmp ].flaga ) )
	{
	    bit = inform[ tmp ].bit;
	    break;
	}

    if ( !inform[ tmp ].bit )
    {
	send_to_char( "Nie istnieje taka flaga.\n\r", ch );
	return;
    }

    if ( fSet )
    {
	SET_BIT( victim->info, bit );
	send_to_char( "Ustawiono.\n\r", ch );
	return;
    }

    REMOVE_BIT( victim->info, bit );
    send_to_char( "Usuni`eto.\n\r", ch );

    return;
}


KOMENDA( do_bamfin )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "wskok" ) )
	return;

    if ( !IS_NPC( ch ) )
    {
	if ( !*argument )
	{
	    ch_printf( ch, "Tw`oj wskok: %s %s.{x\n\r", ch->short_descr,
			ch->pcdata->bamfin[ 0 ] ? ch->pcdata->bamfin
			: "pojawia si`e w`sr`od rozb`lysk`ow `swiat`la" );
	    return;
	}

	if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
	send_to_char( "Zmieniasz sobie \"wskok\".\n\r", ch );
    }

    return;
}


KOMENDA( do_bamfout )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "wyskok" ) )
	return;

    if ( !IS_NPC( ch ) )
    {
	if ( !*argument )
	{
	    ch_printf( ch, "Tw`oj wyskok: %s %s.{x\n\r", ch->short_descr,
			ch->pcdata->bamfout[ 0 ] ? ch->pcdata->bamfout
			: "pojawia si`e w`sr`od rozb`lysk`ow `swiat`la" );
	    return;
	}

	if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->bamfout );
	ch->pcdata->bamfout = str_dup( argument );
	send_to_char( "Zmieniasz sobie \"wyskok\".\n\r", ch );
    }

    return;
}


KOMENDA( do_deny )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];
    char       kto[ MIL * 2 ];
    char       buf[ MIL ];
    time_t     dokiedy = 0;

    rch = get_char( ch );

    if ( !authorized( rch, "wyrzu`c" ) )
	return;

    argument = one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo wyrzuci`c z gry na sta`le?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie na mobach.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( !str_prefix( "-na ", argument ) )
    {
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg );

	dokiedy = policz_czas( arg );
    }

    if ( dokiedy > 0 )
	 sprintf( buf, " (do %s)", asctime_pl_krotko_nonl( localtime( &dokiedy ) ) );
    else
	strcpy( buf, "" );

    if ( *argument )
	sprintf( kto, "%s %s%s: %s", rch->name, ascdata( localtime( &current_time ) ), buf, argument );
    else
	sprintf( kto, "%s %s%s", rch->name, ascdata( localtime( &current_time ) ), buf );

    SET_BIT( victim->act, PLR_DENY );
    victim->pcdata->deny.do_kiedy = dokiedy;
    free_string( victim->pcdata->deny.od );
    victim->pcdata->deny.od = str_dup( kto );
    send_to_char( "Masz zabroniony wst`ep do muda!\n\r", victim );
    send_to_char( "Wyrzucono!\n\r", ch );

    /* postaram sie zapewnic, ze wyjdzie natychmiast */
    if ( victim->position <= POS_STUNNED )
    {
	victim->hit = 1;
	update_pos( victim );
    }

    if ( auction->item
      && ( victim == auction->buyer
	|| victim == auction->seller ) )
    {
	do_auction( ch, "stop" );
    }

    if ( victim->fighting )
	stop_fighting( victim, TRUE );

    do_quit( victim, "zupe`lnie" );

    return;
}


/*
 * Nowa skladnia: rozlacz <gracz> albo rozlacz <numer polaczenia>
 * Druga forma dziala na postacie nie w grze.
 */
KOMENDA( do_disconnect )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "roz`l`acz" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo roz`l`aczy`c?\n\r", ch );
	return;
    }

/*
 * Lam 23.3.98: przerwanie dowolnego polaczenia, nawet nie grajacego, po numerze
 */
    if ( is_number( arg ) )
    {
	unsigned int num = atoi( arg );

	for ( d = descriptor_list; d; d = d->next )
	    if ( d->descriptor == num )
	    {
		if ( d->character
		  && get_trust( d->character ) > get_trust( ch ) )
		{
		    send_to_char( "Nie wolno ci roz`l`acza`c swojego prze`lo`zonego.\n\r", ch );
		    return;
		}
		close_socket( d, TRUE );
		send_to_char( "Roz`l`aczono.\n\r", ch );
		return;
	    }

	send_to_char( "Nie ma takiego po`l`aczenia.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( !victim->desc )
    {
	act( ZM_ZAWSZE, "$N nie jest po`l`aczon$Y.", ch, NULL, victim, TO_CHAR );
	return;
    }

/* Lam: wazny test */
    if ( get_trust( victim ) > get_trust( ch ) )
    {
	send_to_char( "Nie mo`zesz roz`l`aczy`c swojego prze`lo`zonego.\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
	if ( d == victim->desc )
	{
	    close_socket( d, TRUE );
	    send_to_char( "Roz`l`aczono.\n\r", ch );
	    return;
	}

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Nie ma takiego po`l`aczenia!\n\r", ch );

    return;
}


KOMENDA( do_pardon )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "wybacz" ) )
	return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: wybacz <posta`c> <morderca|z`lodziej|zab`ojca>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Moby s`a niewybaczalne :)\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "morderca" ) )
    {
	if ( IS_SET( victim->act, PLR_KILLER ) )
	{
	    REMOVE_BIT( victim->act, PLR_KILLER );
	    free_string( ch->pcdata->killer_from );
	    ch->pcdata->killer_from = str_dup( "" );
	    send_to_char( "Usuni`eta flaga mordercy.\n\r",   ch     );
	    send_to_char( "Ju`z nie jeste`s MORDERC`A.\n\r", victim );
	}
	return;
    }
    /* tego nie nalezy uzywac, ale jest zeby nie mowiono,
       ze bogowie czegos nie potrafia :) */
    if ( !str_cmp( arg2, "zab`ojca" ) )
    {
	if ( IS_SET( victim->act, PLR_ZABOJCA ) )
	{
	    REMOVE_BIT( victim->act, PLR_ZABOJCA );
	    free_string( ch->pcdata->zabojca_from );
	    ch->pcdata->zabojca_from = str_dup( "" );
	    send_to_char( "Usuni`eta flaga zab`ojcy.\n\r",   ch     );
	    send_to_char( "Ju`z nie jeste`s ZAB`OJC`A.\n\r", victim );
	}
	return;
    }

    if ( !str_cmp( arg2, "z`lodziej" ) )
    {
	if ( IS_SET( victim->act, PLR_THIEF  ) )
	{
	    REMOVE_BIT( victim->act, PLR_THIEF  );
	    free_string( ch->pcdata->thief_from );
	    ch->pcdata->thief_from = str_dup( "" );
	    send_to_char( "Usuni`eta flaga z`lodzieja.\n\r",   ch     );
	    send_to_char( "Ju`z nie jeste`s Z`LODZIEJEM.\n\r", victim );
	}
	return;
    }

    send_to_char( "Sk`ladnia: wybacz <posta`c> <morderca|z`lodziej|zab`ojca>.\n\r", ch );

    return;
}


KOMENDA( do_echo )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "echo" ) )
	return;

    if ( argument[ 0 ] == '\0' )
    {
	send_to_char( "Co ma powt`orzy`c echo?\n\r", ch );
	return;
    }

    strcat( argument, "\n\r" );
    send_to_all_char( argument );

    return;
}


KOMENDA( do_oglos )
{
    CHAR_DATA *rch;
    char       buf[ MSL ];

    buf[ 0 ] = '\0';
    rch = get_char( ch );

    if ( !authorized( rch, "+" ) )
	return;

    if ( argument[ 0 ] == '\0' )
    {
	send_to_char( "Co chcesz og`losi`c?\n\r", ch );
	return;
    }

    sprintf( buf, "{YOG`LOSZENIE{B:{x %s\n\r", argument );
    send_to_all_char( buf );

    return;
 }


KOMENDA( do_recho )
{
    CHAR_DATA       *rch;
    DESCRIPTOR_DATA *d;

    rch = get_char( ch );

    if ( !authorized( rch, "pecho" ) )
	return;

    if ( argument[ 0 ] == '\0' )
    {
	send_to_char( "Co ma powt`orzy`c echo w pomieszczeniu?\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
	if ( d->connected == CON_PLAYING
	    && d->character->in_room == ch->in_room )
	{
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}

    return;
}


KOMENDA( do_teleecho )
{
    char arg[ MIL ];
    CHAR_DATA *ofiara;

    if ( !authorized( get_char( ch ), "teleecho" ) )
	return;

    argument = one_argument( argument, arg );

    if ( !*argument )
    {
	STC( "Komu i co ma powt`orzy`c echo postaci?\n\r", ch );
	return;
    }

    if ( !( ofiara = get_char_world_pc( ch, arg ) ) )
    {
	STC( "Nie widzisz takiej postaci w `swiecie.\n\r", ch );
	return;
    }

    ch_printf( ofiara, "%s\n\r", argument );
    ch_printf( ch, "Przekazujesz %s: %s\n\r", PERSC( ofiara, ch, FALSE ), argument );

    return;
}


KOMENDA( do_cecho )
{
    CHAR_DATA       *rch;
    DESCRIPTOR_DATA *d;
    char             arg1[ MAX_INPUT_LENGTH ];
    int              chan;

    rch = get_char( ch );

    if ( !authorized( rch, "kecho" ) )
	return;

    argument = one_argument( argument, arg1 );

    if ( argument[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: kecho <kana`l> <text>\n\r", ch );
	return;
    }

    if ( !str_prefix( arg1, "gratulacje" ) )
    {
	send_to_char( "Echujesz na kana`l GRATULACJE.\n\r", ch );
	chan = CHANNEL_GRATS;
    }
    else if ( !str_prefix( arg1, "gaw`eda" ) )
    {
	send_to_char( "Echujesz na kana`l GAW`EDA.\n\r", ch );
	chan = CHANNEL_CHAT;
    }
    else if ( !str_prefix( arg1, "klan" ) )
    {
	send_to_char( "Echujesz na kana`l KLAN.\n\r", ch );
	chan = CHANNEL_CLAN;
    }
    else if ( !str_prefix( arg1, "bohater" )
      || *arg1 == ':' )
    {
	send_to_char( "Echujesz na kana`l bohater`ow (:).\n\r", ch );
	chan = CHANNEL_HERO;
    }
    else if ( !str_prefix( arg1, "nie`smiert" )
      || *arg1 == '>' )
    {
	send_to_char( "Echujesz na kana`l nie`smiertelnych (>).\n\r", ch );
	chan = CHANNEL_IMMORTAL;
    }
    else
    {
	send_to_char( "Nieznany/nie obs`lugiwany kana`l. [znam gratulacje/gaw`eda/klan/:/>]\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	rch = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING
	  && !IS_SET( rch->deaf, chan )
	  && !( chan == CHANNEL_HERO && !IS_HERO( rch ) )
	  && !( chan == CHANNEL_IMMORTAL && get_trust( rch ) < 103 ) )
	{
	    upper_window( rch );
	    send_to_char( argument, rch );
	    send_to_char( "\n\r",   rch );
	    lower_window( rch );
	}
    }

    return;
}


ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;

    if ( !str_cmp( arg, "/" ) ) /* Lam */
	return get_room_index( PRZYWOLANIE( ch ) );

    if ( is_number( arg ) )
	return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world_pc( ch, arg ) ) ) /* Lam: _pc */
	return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg ) ) )
	return obj->in_room;

    return NULL;
}


KOMENDA( do_transfer )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    ROOM_INDEX_DATA *location;
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "przenie`s" ) )
	return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: przenie`s <posta`c> [pomieszczenie]\n\r", ch );
	return;
    }

    if ( czy_wszyscy( arg1 ) )
    {
	for ( d = descriptor_list; d; d = d->next )
	    if ( d->connected == CON_PLAYING
		&& d->character != ch
		&& d->character->in_room
		&& can_see( ch, d->character ) )
	    {
		char buf[ MAX_STRING_LENGTH ];

		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_transfer( ch, buf );
	    }

	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[ 0 ] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( !( location = find_location( ch, arg2 ) ) )
	{
	    send_to_char( "Nie znam takiego miejsca.\n\r", ch );
	    return;
	}

	if ( room_is_private( location ) )
	{
	    send_to_char( "Pomieszczenie jest zaj`ete.\n\r", ch );
	    return;
	}

	if ( location->trap )
	{
	    send_to_char( "Nie wolno przenosi`c graczy do pu`lapek!\n\r", ch );
	    return;
	}

	if ( location->vnum == ROOM_VNUM_POZA_SWIATEM )
	{
	    STC( "Nie przenosi si`e graczy poza posta`c.\n\r", ch );
	    return;
	}
    }

    if ( !( victim = get_char_world_pc( ch, arg1 ) ) )
    {
	send_to_char( "Nie ma takiej postaci.\n\r", ch );
	return;
    }

    if ( !victim->in_room )
    {
	send_to_char( "Tej postaci nie mo`zesz przenie`s`c.\n\r", ch );
	return;
    }

    if ( victim->in_room->vnum  == ROOM_VNUM_POZA_SWIATEM )
    {
	send_to_char( "Nie wolno przenosi`c graczy spoza `swiata.\n\r", ch );
	return;
    }

    if ( victim->fighting )
	stop_fighting( victim, TRUE );

    PRZERWIJ_KOPANIE( ch, FALSE );
    PRZERWIJ_KARMPOJ( ch, FALSE );

    act( ZM_WZROK | ZM_WID_CHAR, "$n znika w chmurze dymu.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    act( ZM_WZROK | ZM_WID_CHAR, "$n pojawia si`e w`sr`od dymu.", victim, NULL, NULL, TO_ROOM );

    if ( ch != victim )
	act( ZM_PRZYT, "$n ci`e przenosi.", ch, NULL, victim, TO_VICT );

    do_look( victim, "auto" );
    send_to_char( "Przeniesiono.\n\r", ch );

    return;
}


KOMENDA( do_at )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *wch;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    char             arg[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "wpom" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: wpom <miejsce> <polecenie>\n\r", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char( "Nie znam takiego miejsca.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) && get_trust( rch ) < 108 )
    {
	send_to_char( "To pomieszczenie jest ju`z zape`lnione.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	STC( "Przerywasz walk`e.\n\r", ch );
	stop_fighting( ch, TRUE );
    }

    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument, INTERP_AT );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( wch->deleted )
	    continue;

	if ( wch == ch )
	{
	    if ( ch->fighting )
	    {
		STC( "`Ladnie to tak walczy`c na odleg`lo`s`c? Oka`z troch`e honoru!\n\r", ch );
		stop_fighting( ch, TRUE );
	    }

	    char_from_room( ch );
	    char_to_room( ch, original );
	    break;
	}
    }

    return;
}


void real_goto( CHAR_DATA *ch, ROOM_INDEX_DATA *location )
{
    /* zaufanie 108 warte tyle, co poziom 106 */
    if ( get_trust( get_char( ch ) ) < L_DIR && !can_enter_room( ch, location, TRUE ) )
	return;

    if ( location->vnum == ROOM_VNUM_POZA_SWIATEM )
    {
	STC( "Napisz \"wyjd`x\", aby si`e tam przenie`s`c.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	STC( "Przerywasz walk`e.\n\r", ch );
	stop_fighting( ch, TRUE );
    }

    PRZERWIJ_KOPANIE( ch, TRUE );
    PRZERWIJ_KARMPOJ( ch, TRUE );

    if ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "$n $T.{x", ch, NULL,
	    ( ch->pcdata && ch->pcdata->bamfout[ 0 ] != '\0' )
	    ? ch->pcdata->bamfout : "znika w`sr`od rozb`lysk`ow `swiat`la",  TO_ROOM );
    }
    else if ( !IS_NPC( ch ) )
    {
	CHAR_DATA *tch;

	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	    if ( tch != ch && get_trust( tch ) >= ch->pcdata->wizinvis
	      && IS_SET( tch->act, PLR_HOLYLIGHT ) )
	    {
		act( ZM_WZROK | ZM_WID_CHAR, "$n $t.{x", ch,
		  ( ch->pcdata && ch->pcdata->bamfout[ 0 ] != '\0' )
		  ? ch->pcdata->bamfout : "znika w`sr`od rozb`lysk`ow `swiat`la", tch, TO_VICT );
	    }
    }

    char_from_room( ch );
    char_to_room( ch, location );

    if ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "$n $T.{x", ch, NULL,
	    ( ch->pcdata && ch->pcdata->bamfin[ 0 ] != '\0' )
	    ? ch->pcdata->bamfin : "pojawia si`e w`sr`od rozb`lysk`ow `swiat`la", TO_ROOM );
    }
    else if ( !IS_NPC( ch ) )
    {
	CHAR_DATA *tch;

	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	    if ( tch != ch && get_trust( tch ) >= ch->pcdata->wizinvis
	      && IS_SET( tch->act, PLR_HOLYLIGHT ) )
	    {
		act( ZM_WZROK | ZM_WID_CHAR, "$n $t.{x", ch,
		  ( ch->pcdata && ch->pcdata->bamfin[ 0 ] != '\0' )
		  ? ch->pcdata->bamfin : "pojawia si`e w`sr`od rozb`lysk`ow `swiat`la", tch, TO_VICT );
	    }
    }

    do_look( ch, "auto" );

    return;
}


KOMENDA( do_goto )
{
    ROOM_INDEX_DATA *location;
    char arg[ MAX_INPUT_LENGTH ];

    if ( !authorized( get_char( ch ), "le`cdo" ) )
	return;

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Dok`ad polecie`c?\n\r", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char( "Nie ma takiego miejsca w `swiecie.\n\r", ch );
	return;
    }

    real_goto( ch, location );

    return;
}


/* Lam 3.11.2005 */
KOMENDA( do_lecnowy )
{
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];

    if ( !authorized( get_char( ch ), "le`cnowy" ) )
	return;

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Dok`ad polecie`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( victim->in_room == ch->in_room )
    {
	STC( "Do tej postaci nie musisz daleko lecie`c.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	STC( "Wybrana posta`c jest mobem, a le`cnowy s`lu`zy do pomocy graczom.\n\r", ch );
	return;
    }

    if ( victim->level > 5 )
    {
	STC( "Wybrana posta`c ma poziom wy`zszy ni`z 5, a le`cnowy s`lu`zy do pomocy nowicjuszom.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) > 20 )
    {
	STC( "Wybrana posta`c ma zaufanie ponad 20, a le`cnowy s`lu`zy do pomocy nowicjuszom.\n\r", ch );
	return;
    }

    real_goto( ch, victim->in_room );

    return;
}


KOMENDA( do_rstat )
{
    OBJ_DATA        *obj;
    CHAR_DATA       *rch;
    ROOM_INDEX_DATA *location;
    AFFECT_DATA     *paf;
    char             buf [ MAX_STRING_LENGTH ];
    char             buf1[ MAX_STRING_LENGTH ];
    char             arg [ MAX_INPUT_LENGTH  ];
    int              door;

    rch = get_char( ch );

    if ( !authorized( rch, "pomstat" ) )
	return;

    one_argument( argument, arg );
    location = ( arg[ 0 ] == '\0' ) ? ch->in_room : find_location( ch, arg );

    if ( !location )
    {
	send_to_char( "Nie ma takiego pomieszczenia.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( location )
      && get_trust( rch ) < 108 )
    {
	send_to_char( "To pomieszczenie jest zaj`ete.\n\r", ch );
	return;
    }

    buf1[ 0 ] = '\0';

    sprintf( buf, "Nazwa: \"%s\".\n\rKraina: \"%s\" (%s).\n\r",
	    location->name,
	    location->area->name,
	    location->area->file_name );
    strcat( buf1, buf );

    sprintf( buf,
	    "Vnum: %d.  Pod`lo`ze: %d (%s).  `Xr`od`la `swiat`la: %d.  Pojemno`s`c: %d.\n\r",
	    location->vnum,
	    location->sector_type, typy_podloza[ location->sector_type ],
	    location->light,
	    location->pojemnosc );
    strcat( buf1, buf );

    sprintf( buf,
	    "Flagi: %d (%s).\n\r",
	    location->room_flags,
	    room_flag_name_pl( location->room_flags ) );
    strcat( buf1, buf );

    sprintf( buf,
	    "Opis:\n\r%s",
	    location->description );
    strcat( buf1, buf );

    if ( location->trap )
    {
	sprintf( buf, "{RPu`lapka{x, typ %d, `la`ncuch pu`lapki:\n\r%s",
		location->trap->trap_type, location->trap->trap_string );
	strcat( buf1, buf );
    }

    if ( location->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "Dodatkowe opisy: " );

	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, "\"" );
	    strcat( buf1, ed->keyword );

	    if ( ed->next )
		strcat( buf1, "\" " );
	    else
		strcat( buf1, "\"" );
	}

	strcat( buf1, "\n\r" );
    }

    strcat( buf1, "Postacie:" );

    /* Yes, we are reusing the variable rch.  - Kahn */
    for ( rch = location->people; rch; rch = rch->next_in_room )
	/* Heh.  Thanks to Zavod for this little security fix */
	if ( can_see( ch, rch ) )
	{
	    strcat( buf1, " " );
	    one_argument( rch->name, buf );
	    strcat( buf1, buf );

	    if ( strlen( buf1 ) > MSL * 2 / 5 )
		break;
	}

    strcat( buf1, "\n\rPrzedmioty:" );

    for ( obj = location->contents; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj ) )
	{
	    strcat( buf1, " " );
	    one_argument( obj->name, buf );
	    strcat( buf1, buf );

	    if ( strlen( buf1 ) > MSL * 4 / 5 )
		break;
	}

    strcat( buf1, "\n\r" );

    for ( door = 0; door <= 9; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[ door ] ) )
	{
	    sprintf( buf,
		    "Drzwi: %d (%s).  Do: %d.  Klucz: %d.  Flagi: %s.\n\r",
		    door,
		    kierunki[ door ].krotki,
		    pexit->to_room ? pexit->to_room->vnum : 0,
		    pexit->key,
		    exit_info_name_pl( pexit->exit_info ) );
	    strcat( buf1, buf );
	    sprintf( buf,
		    "  S`lowo: \"%s\". Mianownik: \"%s\". Biernik: \"%s\".\n\r  Opis: %s\n\r",
		    pexit->keyword,
		    pexit->short_descr,
		    pexit->biernik,
		    pexit->description[ 0 ] != '\0' ? pexit->description
						  : "(brak)." );
	    strcat( buf1, buf );

	    if ( pexit->mobprogs )
		strcat( buf1, "Wyj`scie posiada progi. Napisz wpstat.\n\r" );
	}
    }

    for ( paf = location->affected; paf; paf = paf->next )
    {
	sprintf( buf, "Wp`lyw: \"%s\" dzia`la jeszcze przez %d g.\n\r",
		skill_table[paf->type].pl_name,
		paf->duration );
	strcat( buf1, buf );
    }

    if ( location->mobprogs )
	strcat( buf1, "Pomieszczenie ma progi. Napisz pompstat.\n\r" );

    send_to_char( buf1, ch );

    return;
}


char *item_type_name_pl( OBJ_DATA *obj )
{
    OBJ_DATA *in_obj;
    char buf[ MSL ];

    if ( obj->item_type >= ITEM_MAX
      || !*typy_przedmiotow[ obj->item_type ].name )
    {
	for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by || in_obj->owned_by )
	    sprintf( buf, "item_type_name_pl: nieznany typ %d: przedmiot %s (%d), w posiadaniu: %s",
		obj->item_type, obj->name, obj->pIndexData->vnum,
		obj->carried_by ? obj->carried_by->name : obj->owned_by->name );
	else
	    sprintf( buf, "item_type_name_pl: nieznany typ %d: przedmiot %s (%d)",
		obj->item_type, obj->name, obj->pIndexData->vnum );

	bug( buf, 0 );
	return "(nieznane)";
    }

    return typy_przedmiotow[ obj->item_type ].name_pl;
}


KOMENDA( do_ostat )
{
    OBJ_DATA    *obj;
    CHAR_DATA   *rch;
    AFFECT_DATA *paf;
    char         buf [ MAX_STRING_LENGTH ];
    char         buf1[ MAX_STRING_LENGTH ];
    char         arg [ MAX_INPUT_LENGTH  ];
    int          pzl;

    if ( ch )
    {
	rch = get_char( ch );

	if ( !authorized( rch, "pstat" ) )
	    return;
    }
    else
	rch = NULL;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: pstat <przedmiot>\n\r", ch );
	return;
    }

    buf1[ 0 ] = '\0';

    /* Lam 9.7.2006: sprytniejsze wykrywanie vnumow + obsluga wiecej niz
       pierwszego z listy */
    number_argument( arg, buf );

    if ( is_number( buf ) )
    {
	if ( !( obj = get_obj_world_vnum( ch, arg ) ) )
	{
	    send_to_char( "Nie widz`e takiego przedmiotu.\n\r", ch );
	    return;
	}
    }
    else if ( !( obj = get_obj_world( ch, arg ) ) )
    {
	send_to_char( "Nie widz`e takiego przedmiotu.\n\r", ch );
	return;
    }

    sprintf( buf, "Nazwa: %s.\n\r",
	    obj->name );
    strcat( buf1, buf );

    sprintf( buf, "Vnum: %d.  Typ: %s.   W`la`sciciel: \"%s\".\n\r",
	    obj->pIndexData->vnum, item_type_name_pl( obj ), obj->owner );
    strcat( buf1, buf );

    sprintf( buf, "Opis: %s\n\rOpis akcji: %s\n\r",
	    obj->description, *obj->action ? obj->action : "(brak)" );
    strcat( buf1, buf );

    sprintf( buf, "Mianownik: {W%s{x  Dope`lniacz: {W%s{x\n\rCelownik: {W%s{x  Biernik: {W%s{x\n\rNarz`ednik: {W%s{x  Miejscownik: {W%s{x\n\r",
	   obj->short_descr, obj->dopelniacz, obj->celownik, obj->biernik,
	   obj->narzednik, obj->miejscownik );
    strcat( buf1, buf );

    sprintf( buf, "Bity ubrania: %d, %s.  Ubrane: %d.  Wielko`s`c: %d.\n\r",
		obj->wear_flags, wear_flag_name_pl( obj->wear_flags ),
		obj->wear_loc,
		obj->pIndexData->wielkosc /* na razie nie kopiowane */ );
    strcat( buf1, buf );

    if ( *obj->anti_class || *obj->anti_race )
    {
	sprintf( buf, "Anty-profesje: %s. Anty-rasy: %s.\n\r",
		*obj->anti_class ? obj->anti_class : "(brak)",
		*obj->anti_race ? obj->anti_race : "(brak)" );
	strcat( buf1, buf );
    }
    else
	strcat( buf1, "Brak anty-ras/profesji.\n\r" );

    sprintf( buf, "Dodatk. bity: %s.\n\r",
		/* obj->extra_flags, */ extra_bit_name_pl( obj->extra_flags, rch ) );
    strcat( buf1, buf );

    sprintf( buf, "Poziom: %d  Warto`s`c: %d.  Czas: %d.  Ci`e`zar: %1.2f/%1.2f.\n\r",
	    obj->level, obj->cost, obj->timer,
	    (double) obj->weight / 4, get_obj_weight( obj ) / (double) 4 );
    strcat( buf1, buf );

    sprintf( buf,
	    "W pom.: %d.  W przedm.: %s.  Trzym.: %s (%d).  Przech.: %s.\n\r",
	    !obj->in_room    ?        0 : obj->in_room->vnum,
	    !obj->in_obj     ? "(brak)" : obj->in_obj->short_descr,
	    !obj->carried_by ? "(brak)" : obj->carried_by->name,
	    ( obj->carried_by && obj->carried_by->pIndexData ) ? obj->carried_by->pIndexData->vnum : 0,
	    !obj->owned_by   ? "(brak)" : obj->owned_by->name );
    strcat( buf1, buf );
/*
    sprintf( buf, "Values: %d %d %d %d.\n\r",
	    obj->value[ 0 ].v, obj->value[ 1 ].v, obj->value[ 2 ].v, obj->value[ 3 ].v );
    strcat( buf1, buf );
*/

    for ( pzl = 0; pzl < 6; pzl++ )
    {
	if ( typy_przedmiotow[ obj->item_type ].spell[ pzl ] == VAL_SPELL )
	    sprintf( buf, "Warto`s`c %d: %s%s'%s'.\n\r",
		pzl,
		typy_przedmiotow[ obj->item_type ].value[ pzl ],
		typy_przedmiotow[ obj->item_type ].value[ pzl ][ 0 ] ?
		    ": " : "",
		( obj->value[ pzl ].v >= 0 && obj->value[ pzl ].v < MAX_SKILL )
		    ? skill_table[ obj->value[ pzl ].v ].pl_name : "(brak)" );
	else if ( typy_przedmiotow[ obj->item_type ].spell[ pzl ] == VAL_MATERIAL )
	    sprintf( buf, "Warto`s`c %d: %s%s'%s'.\n\r",
		pzl,
		typy_przedmiotow[obj->item_type].value[ pzl ],
		typy_przedmiotow[obj->item_type].value[ pzl ][ 0 ] ?
		    ": " : "",
		( ( obj->value[ pzl ].v > 0 ) && ( obj->value[ pzl ].v < MAX_MATERIAL ) )
		? material_table[ obj->value[ pzl ].v ].name
		: "nic" );
	/* Alandar 04.10.2004: obsluga wartosci przedmiotow bedacych opisami */
	else if ( typy_przedmiotow[ obj->item_type ].spell[ pzl ] == VAL_STRING )
	    sprintf( buf, "Warto`s`c %d: %s%s'%s'.\n\r",
		pzl,
		typy_przedmiotow[ obj->item_type ].value[ pzl ],
		typy_przedmiotow[ obj->item_type ].value[ pzl ][ 0 ] ?
		    ": " : "",
		obj->value[ pzl ].p ? (char *) obj->value[ pzl ].p : "(brak)" );
	/* Alandar 12.10.2004: wyswietlanie nazw plynow */
	else if ( typy_przedmiotow[ obj->item_type ].spell[ pzl ] == VAL_LIQUID )
	    sprintf( buf, "Warto`s`c %d: %s%s'%s'.\n\r",
		pzl,
		typy_przedmiotow[ obj->item_type ].value[ pzl ],
		typy_przedmiotow[ obj->item_type ].value[ pzl ][ 0 ] ?
		    ": " : "",
		( ( obj->value[ pzl ].v >= 0 ) && ( obj->value[ pzl ].v < LIQ_MAX ) )
		? liq_table_pl[ obj->value[ pzl ].v ].liq_name
		: "(b`ledny p`lyn)" );
	else
	    sprintf( buf, "Warto`s`c %d: %s%s%d.\n\r",
		pzl,
		typy_przedmiotow[ obj->item_type ].value[ pzl ],
		typy_przedmiotow[ obj->item_type ].value[ pzl ][ 0 ] ?
		    ": " : "",
		obj->value[ pzl ].v );
	strcat( buf1, buf );
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	sprintf( buf, "[Przedm] Wp`lywa na %s o %d.\n\r",
		affect_loc_name_pl_b( paf->location ), paf->modifier );
	strcat( buf1, buf );
    }

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    {
	sprintf( buf, "[Indeks] Wp`lywa na %s o %d.\n\r",
		affect_loc_name_pl_b( paf->location ), paf->modifier );
	strcat( buf1, buf );
    }

    if ( obj->pIndexData->mobprogs )
	strcat( buf1, "Przedmiot ma progi. Napisz ppstat, aby si`e o nich czego`s dowiedzie`c.\n\r" );

    if ( ch )
	send_to_char( buf1, ch );
    else
    {
	static CHAR_DATA pustak;

	rch = &pustak;
	rch->polskie = 2;
	colourconv( buf, buf1, rch, sizeof( buf ) );
	printf( "%s", buf );
    }

    return;
}


/*
 * Qwert 4.5.2000: wersja polska
 * Lam 4.5.2000: kilka poprawek spolszczenia, nowe polskie rozbijanie flag act
 * i affected_by.
 */
KOMENDA( do_mstat )
{
    CHAR_DATA   *rch;
    CHAR_DATA   *victim;
    AFFECT_DATA *paf;
    char         buf [ MAX_STRING_LENGTH ];
    /* Lam 15.4.2002: zwiekszenie bufora: */
    char         buf1[ MAX_STRING_LENGTH * 4 ];
    char         arg [ MAX_INPUT_LENGTH  ];
    char        *poch;

    rch = get_char( ch );

    if ( !authorized( rch, "mstat" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: mstat <posta`c>?\n\r", ch );
	return;
    }

    number_argument( arg, buf );

    if ( is_number( buf ) )
    {
	if ( !( victim = get_mob_world_vnum( ch, arg ) ) )
	{
	    send_to_char( "Nie ma nikogo takiego.\n\r", ch );
	    return;
	}
    }
    else if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "Nie ma nikogo takiego.\n\r", ch );
	return;
    }

    buf1[ 0 ] = '\0';

    poch = POCHODZENIE( victim );
    sprintf( buf, "Imi`e: %s.  Pochodzi %s %s.\n\r",
	    victim->name,
	    czy_ze( poch ) ? "ze" : "z",
	    poch );
    strcat( buf1, buf );

    sprintf( buf, "Rasa: %s (wiel. %d).  Profesja: %s (%d).  Poziom: %d  Zaufanie: %d\n\r",
		race_table[ victim->race ].who_name,
		ROZMIAR( victim ),
		IS_NPC( victim ) ? "(mob)" : class_table[ victim->klasa ].long_name,
		victim->klasa, victim->level, victim->trust );
    strcat( buf1, buf );

    sprintf( buf, "Vnum: %d.  P`le`c: %s.  W pom: %d.\n\r",
	    IS_NPC( victim ) ? victim->pIndexData->vnum : 0,
	    victim->sex == SEX_MALE    ? "m`eska"   :
	    victim->sex == SEX_FEMALE  ? "`ze`nska" : "nijaka",
	    !victim->in_room           ? 0          : victim->in_room->vnum );
    strcat( buf1, buf );

    /* Malven */
    if ( IS_NPC( victim ) )
	strcat( buf1, "Klan: (mob), poziom: (brak).\n\r" );
    else if ( !victim->pcdata->clan )
	strcat( buf1, "Klan: (brak), poziom: (brak).\n\r" );
    else
    {
	sprintf( buf, "Klan: \"%s\", poziom: %d (%s).\n\r",
	    victim->pcdata->clan->name,
	    victim->pcdata->clan_member->level,
	    victim->sex == SEX_MALE ?
		victim->pcdata->clan->lev_name_mm[ victim->pcdata->clan_member->level ]
	      : victim->pcdata->clan->lev_name_zm[ victim->pcdata->clan_member->level ] );
	strcat( buf1, buf );
    }

    sprintf( buf, "Si`l: %d.  Int: %d.  M`ad: %d.  Zr`e: %d.  Bud: %d.\n\r",
	    get_curr_str( victim ),
	    get_curr_int( victim ),
	    get_curr_wis( victim ),
	    get_curr_dex( victim ),
	    get_curr_con( victim ) );
    strcat( buf1, buf );

    sprintf( buf, "`Zycie: %d/%d.  Mana: %d/%d.  Ruch: %d/%d.  `Cwiczenia: %d.\n\r",
	    victim->hit,         victim->max_hit,
	    victim->mana,        victim->max_mana,
	    victim->move,        victim->max_move,
	    victim->practice );
    strcat( buf1, buf );

    sprintf( buf,
	"Moralno`s`c: %d.  Z`loto: %d (i %d w pojemnikach).  Bank: %d.\n\r",
	    victim->alignment, victim->gold,
	    zloto_w_pojemnikach( victim->carrying ),
	    IS_NPC( victim ) ? 0 : victim->pcdata->bank );
    strcat( buf1, buf );

    /* Qwert: pozwolilem sobie dodac cos od siebie */
    if ( !IS_NPC( victim ) )
    {
	sprintf( buf,
	"Pkt Do`sw.: %d.  Pkt Zada`n: %d.\n\r",
	    victim->exp,		victim->qp );
	strcat( buf1, buf );
	sprintf( buf,
	"Zab.: %d (%d gr.)  Zgony: %d (%d gr., %d wykr., %d sam.)\n\r",
	    victim->pcdata->killed,	victim->pcdata->pkilled,
	    victim->pcdata->dead,	victim->pcdata->pdead,
	    victim->pcdata->wykrwawienia, victim->pcdata->samobojstwa );
	strcat( buf1, buf );
    }

    sprintf( buf, "Pozycja: %d.  Tch`orz: %d.  KP: %d.  MKP: %d.  OPC: %d.\n\r",
	victim->position, victim->wimpy, victim->armor, victim->marmor, victim->saving_throw );
    strcat( buf1, buf );

    sprintf( buf, "premia celno`sci: %d  premia obra`ze`n: %d.\n\r",
	    get_hitroll( victim, WEAR_WIELD ),
	    get_damroll( victim, WEAR_WIELD ) );

    if ( IS_NPC( victim )
      || victim->level >= skill_table[ gsn_dual ].skill_level[ victim->klasa ] )
    {
	strcat( buf1, "Pierwsza bro`n: " );
    }
    else
	buf[ 0 ] = 'P';

    strcat( buf1, buf );

    if ( get_eq_char( victim, WEAR_WIELD_2 )
      || ( ( !IS_NPC( victim ) && victim->pcdata->learned[ gsn_dual ] )
	&& !( get_eq_char( victim, WEAR_SHIELD )
	  || get_eq_char( victim, WEAR_HOLD )
	  || get_eq_char( victim, WEAR_LIGHT ) ) ) )
    {
	sprintf( buf, "Druga bro`n  :  premia celno`sci: %d  premia obra`ze`n: %d.\n\r",
		get_hitroll( victim, WEAR_WIELD_2 ),
		get_damroll( victim, WEAR_WIELD_2 ) );
	strcat( buf1, buf );
    }

/* Lam: mi to niepotrzebne
    if ( !IS_NPC( victim ) )
    {
	sprintf( buf, "Wielko`s`c strony: %d.\n\r", victim->pcdata->pagelen );
	strcat( buf1, buf );
    }
*/

    sprintf( buf, "Walka: %s.  Polowanie: %s.\n\rNienawi`s`c: %s.  Strach: %s.\n\r",
	    victim->fighting ? victim->fighting->vch->name : "(brak)",
	    victim->hunting ? victim->hunting->name : "(brak)",
	    victim->hating ? victim->hating->name : "(brak)",
	    victim->fearing ? victim->fearing->name : "(brak)" );
    strcat( buf1, buf );

    /* Qwert: tu bylo drugi raz saving_throw */
    if ( !IS_NPC( victim ) )
    {
	sprintf( buf,
		"Pragnienie: %d.  G`l`od: %d.  Nietrze`xwo`s`c: %d.\n\r",
		victim->pcdata->condition[ COND_THIRST ],
		victim->pcdata->condition[ COND_FULL ],
		victim->pcdata->condition[ COND_DRUNK ] );
	strcat( buf1, buf );
    }

    sprintf( buf, "Rzeczy: %d/%d.  Ci`e`zar: %d/%d.\n\r",
	    victim->carry_number, can_carry_n( victim ),
	    victim->carry_weight, can_carry_w( victim ) );
    strcat( buf1, buf );

    sprintf( buf, "Wiek: %d.  Godz.: %d.  Timer: %d.\n\r",
	    get_age( victim ),
	    ( victim->played + (int) ( current_time - victim->logon ) ) / 3600,
	    victim->timer );
    strcat( buf1, buf );

    sprintf( buf, "Act: %d, %s.\n\rWp`lywy: %s.\n\r",
	victim->act, act_flag_name_pl( victim->act, ch ),
	affect_bit_name_pl( victim->affected_by ) );
    strcat( buf1, buf );

    sprintf( buf, "Monitor: %s.  Mistrz: %s.  Lider: %s.\n\r",
	    victim->monitor     ? victim->monitor->name  : "(brak)",
	    victim->master      ? victim->master->name   : "(brak)",
	    victim->leader      ? victim->leader->name   : "(brak)" );
    strcat( buf1, buf );

    sprintf( buf, "D`lugi opis: %s\n\r",
	    victim->long_descr[ 0 ] != '\0' ? victim->long_descr
					  : "(brak)" );
    strcat( buf1, buf );

    if ( !IS_NPC( victim ) )
    {
	sprintf( buf, "D`lugi opis 2: %s\n\r",
		victim->long_descr_orig[ 0 ] != '\0' ? victim->long_descr_orig
						   : "(brak)" );
	strcat( buf1, buf );

	sprintf( buf, "Fa`lszywe imi`e: %s\n\r",
		victim->false_name[ 0 ] != '\0' ? victim->false_name
						   : "(brak)" );
	strcat( buf1, buf );
    }

    sprintf( buf, "Mianownik: {W%s{x;  Dope`lniacz: {W%s{x;\n\rCelownik: {W%s{x;  Biernik: {W%s{x;\n\rNarz`ednik: {W%s{x;  Miejscownik: {W%s{x;\n\rWo`lacz: {W%s{x.\n\r",
	victim->short_descr, victim->dopelniacz, victim->celownik,
	victim->biernik, victim->narzednik, victim->miejscownik, victim->wolacz );
    strcat( buf1, buf );

    if ( IS_NPC( victim ) && victim->pIndexData->pShop )
    {
	int itype;
	bool cokolwiek = FALSE;

	sprintf( buf, "Mob prowadzi sklep, w kt`orym sprzedaje za %d%% i skupuje za %d%% warto`sci.\n\r",
		victim->pIndexData->pShop->profit_buy,
		victim->pIndexData->pShop->profit_sell );
	strcat( buf1, buf );

	sprintf( buf, "Sklep jest otwarty od %d do %d, a skupuje przedmioty typ`ow:",
		victim->pIndexData->pShop->open_hour,
		victim->pIndexData->pShop->close_hour );

	for ( itype = 0; itype < MAX_TRADE; itype++ )
	    if ( victim->pIndexData->pShop->buy_type[ itype ]
	      && victim->pIndexData->pShop->buy_type[ itype ] < ITEM_MAX
	      && *typy_przedmiotow[ victim->pIndexData->pShop->buy_type[ itype ] ].name )
	    {
		strcat( buf, " " );
		strcat( buf, typy_przedmiotow[ victim->pIndexData->pShop->buy_type[ itype ] ].name_pl );
		cokolwiek = TRUE;
	    }

	if ( !cokolwiek )
	    strcat( buf, " (brak).\n\r" );
	else
	    strcat( buf, ".\n\r" );

	strcat( buf1, buf );
    }

    if ( IS_NPC( victim ) && victim->pIndexData->pHealer )
	strcat( buf1, "Mob jest sprzedawc`a czar`ow.\n\r" );

    if ( IS_NPC( victim ) && victim->spec_fun )
    {
	sprintf( buf, "Mob ma specjalno`s`c \"%s\".\n\r",
		spec_name( victim->spec_fun ) );
	strcat( buf1, buf );
    }

    if ( IS_NPC( victim ) && victim->pIndexData->game_fun )
    {
	sprintf( buf, "Mob prowadzi gr`e \"%s\".\n\r",
		game_name( victim->pIndexData->game_fun ) );
	strcat( buf1, buf );
    }

    if ( IS_NPC( victim )
      && victim->pIndexData->mobprogs )
    {
	strcat( buf1, "Mob ma progi. Napisz mpstat, by czego`s si`e o nich dowiedzie`c.\n\r" );
    }

    for ( paf = victim->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;

	sprintf( buf,
		"Wp`lyw: \"%s\", na %s o %d na %d g., %s",
		skill_table[(int) paf->type].pl_name,
		affect_loc_name_pl_b( paf->location ),
		paf->modifier,
		paf->duration,
		affect_bit_name_pl( paf->bitvector ) );
	strcat( buf1, buf );

	if ( paf->level )
	{
	    sprintf( buf, ", poziom %d", paf->level );
	    strcat( buf1, buf );
	}

	if ( paf->caster && can_see( ch, paf->caster ) )
	{
	    sprintf( buf, ", rzucaj`acy %s", paf->caster->short_descr );
	    strcat( buf1, buf );
	}

	strcat( buf1, ".\n\r" );
    }
    send_to_char( buf1, ch );

    return;
}


/*
   Alucard 3.2.2004: Przepisalem cala funkcje i dodalem duzo roznych rzeczy
   (-fla, -aff, -spec) oraz wyswietlanie vnuma pokoju, w ktorym mob aktualnie
   sie znajduje.
*/
extern int top_mob_index;
KOMENDA( do_mfind )
{
    CHAR_DATA      *rch, *victim;
    MOB_INDEX_DATA *pMobIndex;
    char            buf [ MAX_STRING_LENGTH     ];
    char            buf1[ MAX_STRING_LENGTH * 8 ];
    char            arg [ MAX_INPUT_LENGTH      ];
    int             vnum;
    int             nMatch;
    char           *lista_flag, *biezaca_flaga;
    bool            fFlag = FALSE, fRace = FALSE, fAff = FALSE, fAll = FALSE;
    bool            fSpec = FALSE;
    bool            found = FALSE;
    int             flaga = 0, affect = 0, race = 0;
    int             licznik = 0;
    SPEC_FUN       *spec = NULL;
    bool            jest_opcja = TRUE;
    int             room_vnum = 0;
    int             min = 0, max = 999;

    rch = get_char( ch );

    if ( !authorized( rch, "mszukaj" ) )
	return;

    while ( jest_opcja )
    {
	jest_opcja = FALSE;
	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "-fla" ) )
	{
	    if ( fFlag )
	    {
		STC( "Mo`zna szuka`c tylko jednej flagi jednocze`snie.\n\r", ch );
		return;
	    }

	    argument = one_argument( argument, arg );

	    lista_flag = act_flag_name_pl( -1, rch );

	    if ( !*arg || !str_cmp( arg, "info" ) )
	    {
		STC( "Dost`epne flagi:\n\r", ch );
		STC( lista_flag, ch );
		STC( "\n\r", ch );
		return;
	    }

	    biezaca_flaga = lista_flag;
	    found = FALSE;

	    for ( licznik = 0; *biezaca_flaga != '\0'; licznik++  )
	    {
		if ( !str_prefix( arg, biezaca_flaga ) )
		{
		    found = TRUE;
		    break;
		}

		biezaca_flaga = lista_flag + licznik;

		if ( biezaca_flaga[ 0 ] == ' ' )
		    ++flaga;
	    }

	    if ( !found )
	    {
		STC( "Podana flaga nie istnieje.\n\r", ch );
		return;
	    }

	    fFlag = TRUE;
	    flaga = ( 1 << flaga );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-wp`l" ) )
	{
	    int all_affect_bits[ AFF_FLAGS_SIZE ];
	    int pil;

	    if ( fAff )
	    {
		STC( "Mo`zna szuka`c tylko jednej flagi jednocze`snie.\n\r", ch );
		return;
	    }

	    argument = one_argument( argument, arg );

	    for ( pil = 0; pil < AFF_FLAGS_SIZE; pil++ )
		all_affect_bits[ pil ] = -1;

	    lista_flag = affect_bit_name_pl( all_affect_bits );

	    if ( !*arg || !str_cmp( arg, "info" ) )
	    {
		STC( "Dost`epne wp`lywy:\n\r", ch );
		STC( lista_flag, ch );
		STC( "\n\r", ch );
		return;
	    }

	    biezaca_flaga = lista_flag;
	    found = FALSE;

	    for ( licznik = 0; affect <= MAX_AFF_FLAG; licznik++ )
	    {
		if ( !str_prefix( arg, biezaca_flaga ) )
		{
		    found = TRUE;
		    break;
		}

		biezaca_flaga = lista_flag + licznik;

		if ( biezaca_flaga[ 0 ] == ' ' )
		    ++affect;
	    }

	    if ( !found )
	    {
		STC( "Podany wp`lyw nie istnieje.\n\r", ch );
		return;
	    }

	    fAff = TRUE;

	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-rasa" ) )
	{
	    if ( fRace )
	    {
		STC( "Mo`zna szuka`c tylko jednej rasy jednocze`snie.\n\r", ch );
		return;
	    }

	    argument = one_argument( argument, arg );
	    fRace = TRUE;

	    race = race_lookup( arg );

	    if ( race < 0 )
	    {
		sprintf( buf, "Nie ma rasy \"%s\".\n\r", arg );
		send_to_char( buf, ch );
		return;
	    }

	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-spec" ) )
	{
	    if ( fSpec )
	    {
		STC( "Mo`zna szuka`c tylko jednej specjalno`sci jednocze`snie.\n\r", ch );
		return;
	    }

	    argument = one_argument( argument, arg );

	    if ( !*arg || !str_cmp( arg, "info" ) )
	    {
		STC( "Dost`epne specjalno`sci:\n\r", ch );
		STC( spec_list, ch );
		STC( "\n\r", ch );
		return;
	    }

	    spec = spec_lookup( arg );

	    if ( spec == 0 )
	    {
		sprintf( buf, "Nie ma specjalno`sci \"%s\".\n\r", arg );
		send_to_char( buf, ch );
		return;
	    }

	    fSpec = TRUE;
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-min" ) )
	{
	    argument = one_argument( argument, arg );
	    min = atoi( arg );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-maks" ) )
	{
	    argument = one_argument( argument, arg );
	    max = atoi( arg );
	    jest_opcja = TRUE;
	}

	if ( arg[ 0 ] == '\0' )
	{
	    STC( "Sk`ladnia: mszukaj [-fla <flaga>] [-wp`l <wp`lyw>] [-rasa <rasa>]\n\r"
		"                  [-spec <spec>] [-min <min. poziom>] [-maks <maks. poziom>]\n\r"
		"                  <nazwa|wsz>\n\r"
		" mszukaj -<kategoria> info wy`swietla dost`epne mo`zliwo`sci.\n\r", ch );
	    return;
	}
    }

    buf1[ 0 ] = '\0';
    fAll    = !str_cmp( arg, "wsz" );
    found   = FALSE;
    nMatch  = 0;

    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pMobIndex->player_name ) )
	    {
		if ( fFlag && !IS_SET( pMobIndex->act, flaga ) )
		    continue;

		if ( fAff
		  && !IS_SET_V( pMobIndex->affected_by, affect ) )
		    continue;

		if ( fSpec && !( pMobIndex->spec_fun == spec ) )
		    continue;

		if ( fRace && !( pMobIndex->race == race ) )
		    continue;

		if ( !( pMobIndex->level >= min ) || !( pMobIndex->level <= max ) )
		    continue;

		found = TRUE;

		room_vnum = 0;

		for ( victim = char_list; victim; victim = victim->next )
		    if ( IS_NPC( victim )
		      && ( victim->pIndexData == pMobIndex )
		      && ( victim->in_room != NULL ) )
		    {
			room_vnum = victim->in_room->vnum;
			break; /* Lam: szukalo ostatniego, szuka pierwszego */
		    }

		if ( room_vnum )
		{
		    sprintf( buf, "[%5d] (%s) (%3d) %s [w %5d]\n\r",
			pMobIndex->vnum, wyrownaj( race_table[ pMobIndex->race ].who_name, -10 ),
			pMobIndex->level, pMobIndex->short_descr, room_vnum );
		}
		else
		{
		    sprintf( buf, "[%5d] (%s) (%3d) %s [(brak)]\n\r",
			pMobIndex->vnum, wyrownaj( race_table[ pMobIndex->race ].who_name, -10 ),
			pMobIndex->level, pMobIndex->short_descr );
		}

		if ( !fAll
		  || ( fAll && fRace )
		  || ( fAll && fFlag )
		  || ( fAll && fAff ) )
		{
		    strcat( buf1, buf );
		}
		else
		    send_to_char( buf, ch );
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "Nie ma nikogo odpowiadaj`acego twojemu zapytaniu.\n\r", ch );
	return;
    }

    if ( !fAll
      || ( fAll && fRace )
      || ( fAll && fFlag )
      || ( fAll && fAff ) )
    {
	send_to_char( buf1, ch );
    }

    return;
}


/*
 * Pomysl ze Zlego, wykonanie lepsze.
 * Nie wiem po co jest statyczny bufor, do ktorego zwracany jest wskaznik,
 * przeciez swobodnie moznaby zwracac wskaznik na odpowiedni fragment
 * w argumencie...
 *
 * Jednak na razie nieuzywane.
char* lac_getopt( char* argument, char* optname )
{
    char*		t;
    char		buf[ MAX_INPUT_LENGTH ];
    static char		arg[ MAX_INPUT_LENGTH ];

    t = argument;
    while ( *t )
    {
	t = one_argument( t, buf );
	if ( *t == '-' )
	{
	    t = one_argument( t, arg );

	    if ( !str_cmp( optname, &buf[ 1 ] ) )
		return arg;
	}
    }

    return "";
}
*/


#define OFSTC( a, b ) { if ( b ) send_to_char( a, b ); else printf( "%s", a ); }

/*
 * Lam 27.4.99: ofind -aff (obecnie pszukaj -dzi)
 * Qwert 02.11.2000 pszukaj -gdz
 * Cadaver 15-30.12.2001 pszukaj -min i -maks poziom
 * Qwert 2.2.2004 pszukaj -typ -fla
 * Lam 5.2.2006: swiatlo idzie do -gdz 16, 15 to twarz
 * Lam 2006: pszuk -prof -rasa -mat
 */
extern int top_obj_index;
KOMENDA( do_ofind )
{
    CHAR_DATA      *rch = NULL;
    OBJ_INDEX_DATA *pObjIndex;
    FILE           *plik = NULL;
    AFFECT_DATA    *paf;
    char            buf[ MAX_STRING_LENGTH ];
    char            buf1[ MAX_STRING_LENGTH * 8 ];
    char            arg[ MAX_INPUT_LENGTH ];
    int             vnum;
    int             nMatch;
    int             aff = APPLY_NONE;
    bool            dopliku = FALSE;
    bool            fAll = FALSE;
    bool            found;
    int             gdzie = 0;
    bool            fMinL = FALSE;
    int             minpoz = 0;
    bool            fMaxL = FALSE;
    int             maxpoz = 999;
    bool            jest_opcja = TRUE;
    bool            odwrot = FALSE;
    bool            fType = FALSE;
    int             typek = 0;
    int             licznik, sumator;
/*  int             zaduzo = 0; */
    char            *fbuf, *sflag;
    bool            fFlag = FALSE;
    int             flaga[ ITEM_EXTRA_FLAGS_SIZE ];
    int             zbitaflaga[ ITEM_EXTRA_FLAGS_SIZE ];
    bool            fSklep = FALSE;
    bool            fRasa = FALSE;
    int             szrasa = -1;
    bool            fProf = FALSE;
    int             szprof = -1;
    bool            fMaterial = FALSE;
    int             szmat = -1;
    bool            fCzar = FALSE;
    int             szczar = -1;

    CLEAR_BITS( flaga );

    if ( ch )
    {
	rch = get_char( ch );

	if ( !authorized( rch, "pszukaj" ) )
	    return;
    }

    while ( jest_opcja )
    {
	jest_opcja = FALSE;
	one_argument( argument, arg );

	if ( !str_cmp( arg, "-plik" ) )
	{
	    argument = one_argument( argument, arg );
	    dopliku = TRUE;
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-sklep" ) )
	{
	    argument = one_argument( argument, arg );
	    fSklep = TRUE;
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-tinfo" ) )
	{
	    for ( licznik = 0; licznik < ITEM_MAX; licznik++ )
		if ( typy_przedmiotow[ licznik ].name_pl[ 0 ] )
		{
		    OFSTC( typy_przedmiotow[ licznik ].name_pl, ch );
		    OFSTC( " ", ch );
		}

	    OFSTC( "\n\r", ch );
	    return;
	}

	if ( !str_cmp( arg, "-typ" ) )
	{
	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );

	    if ( arg[ 0 ] == '!' )
		odwrot = TRUE;

	    for ( licznik = 0; licznik < ITEM_MAX; licznik++ )
	    {
		if ( ( !odwrot
		    && !str_prefix( arg, typy_przedmiotow[ licznik ].name_pl ) )
		  || ( odwrot
		    && !str_prefix( arg + 1, typy_przedmiotow[ licznik ].name_pl ) ) )
		{
		     fType = TRUE;
		     typek = licznik;
		     break;
		}
	    }

	    if ( !fType )
	    {
		    OFSTC( "Podany typ przedmiotu nie istnieje.\n\r", ch );
		    return;
	    }

	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-finfo" ) )
	{
	    int all_extra_bits[ ITEM_EXTRA_FLAGS_SIZE ];
	    int pil;

	    for ( pil = 0; pil < ITEM_EXTRA_FLAGS_SIZE; pil++ )
		all_extra_bits[ pil ] = -1;

	    fbuf = extra_bit_name_pl( all_extra_bits, rch );
	    OFSTC( "Dost`epne flagi:\n\r", ch );
	    OFSTC( fbuf, ch );
	    OFSTC( "\n\r", ch );
	    return;
	}

	if ( !str_cmp( arg, "-fla" ) )
	{
	    unsigned int tymflaga = 0;
	    int all_extra_bits[ ITEM_EXTRA_FLAGS_SIZE ];
	    int pil;

	    for ( pil = 0; pil < ITEM_EXTRA_FLAGS_SIZE; pil++ )
		all_extra_bits[ pil ] = -1;

	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );

	    fbuf = extra_bit_name_pl( all_extra_bits, rch );
	    sflag = fbuf;

	    found = FALSE;

	    for ( licznik = 0; *sflag != '\0'; licznik++  )
	    {
		if ( !str_prefix( arg, sflag ) )
		{
		    found = TRUE;
		    break;
		}

		sflag = fbuf + licznik;

		if ( sflag[ 0 ] == ' ' )
		    ++tymflaga;
	    }

	    if ( !found )
	    {
		OFSTC( "Podana flaga nie istnieje.\n\r", ch );
		return;
	    }

	    fFlag = TRUE;
	    /* pewnie szukanie w tablicy z handler.c byloby lepsze */
	    SET_BIT_V( flaga, tymflaga );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-min" ) )
	{
	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );

	    if ( !is_number( arg ) )
	    {
		    OFSTC( "Argument <poziom> musi by`c liczb`a z zakresu 1-110.\n\r", ch );
		    return;
	    }

	    minpoz = atoi( arg );

	    if ( minpoz < 0 || minpoz > 110 )
	    {
		OFSTC( "Argument <poziom> musi by`c liczb`a z zakresu 1-110.\n\r", ch );
		return;
	    }

	    if ( minpoz > maxpoz )
	    {
		OFSTC( "Argument dla min nie mo`ze by`c wi`ekszy od argumentu dla maks.\n\r", ch );
		return;
	    }

	    fMinL = TRUE;
	    one_argument( argument, arg );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-maks" ) )
	{
	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );

	    if ( !is_number( arg ) )
	    {
		OFSTC( "Argument <poziom> musi by`c liczb`a z zakresu 1-110.\n\r", ch );
		    return;
	    }

	    maxpoz = atoi( arg );

	    if ( maxpoz < 0 || maxpoz > 110 )
	    {
		OFSTC( "Argument <poziom> musi by`c liczb`a z zakresu 1-110.\n\r", ch );
		return;
	    }

	    if ( minpoz > maxpoz )
	    {
		OFSTC( "Argument dla min nie mo`ze by`c wi`ekszy od argumentu dla maks.\n\r", ch );
		return;
	    }

	    fMaxL = TRUE;
	    one_argument( argument, arg );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-dzi" ) )
	{
	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );
	    aff = co_ma_zmieniac( arg, TRUE );
	    one_argument( argument, arg );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-gdz" ) )
	{
	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );

	    if ( !is_number( arg ) )
	    {
		OFSTC( "Argument <miejsce> musi by`c liczb`a z zakresu 1-15.\n\r", ch );
		return;
	    }

	    gdzie = atoi( arg );

	    if ( gdzie < 1 || gdzie > 16 )
	    {
		OFSTC( "Argument <miejsce> musi by`c liczb`a z zakresu 1-15.\n\r", ch );
		return;
	    }

	    gdzie = 1 << gdzie;
	    one_argument( argument, arg );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-rasa" ) )
	{
	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );
	    fRasa = TRUE;
	    szrasa = race_lookup( arg );

	    if ( szrasa == -1 )
	    {
		OFSTC( "Nie ma takiej rasy.\n\r", ch );
		return;
	    }

	    one_argument( argument, arg );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-prof" ) )
	{
	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );
	    fProf = TRUE;

	    for ( szprof = 0; szprof < MAX_CLASS; szprof++ )
		if ( !str_prefix( arg, class_table[ szprof ].long_name ) )
		    break;

	    if ( szprof == MAX_CLASS )
	    {
		OFSTC( "Nie ma takiej profesji.\n\r", ch );
		return;
	    }

	    one_argument( argument, arg );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-mat" ) )
	{
	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );
	    fMaterial = TRUE;
	    szmat = material_lookup_short( arg );

	    if ( szmat < 0 )
	    {
		OFSTC( "Nie ma takiego materia`lu.\n\r", ch );
		return;
	    }

	    one_argument( argument, arg );
	    jest_opcja = TRUE;
	}

	if ( !str_cmp( arg, "-czar" ) )
	{
	    argument = one_argument( argument, arg );
	    argument = one_argument( argument, arg );
	    fCzar = TRUE;
	    szczar = skill_lookup_pl( arg );

	    if ( szczar < 0 )
	    {
		OFSTC( "Nie ma takiej umiej`etno`sci.\n\r", ch );
		return;
	    }

	    one_argument( argument, arg );
	    jest_opcja = TRUE;
	}
    }

    if ( arg[ 0 ] == '\0' )
    {			/* Qwert: typ inwentarz (b00) ma wszystko */
	OFSTC( "Sk`ladnia: pszukaj [-plik] [-sklep] [-min <poz>] [-maks <poz>] [-typ [!]<typ>]\n\r"
		  "                  [-dzi <co>] [-fla <flaga>] [-gdz <miejsce>] [-mat <materia`l>]\n\r"
		  "                  [-prof <prof>] [-rasa <rasa>] [-czar <czar>] <nazwa|wsz>\n\r"
		  " <co>: si`l m`ad int bud zr`e `zycie mana ruch kp celno`s`c obra`zenia opc\n\r"
		  " <miejsce> : 1-palec 2-szyja 3-cia`lo 4-g`lowa 5-nogi 6-stopy 7-r`ece \n\r"
		  "             8-ramiona 9-tarcza 10-owini`ete 11-biodra 12-nadg. 13-bro`n\n\r"
		  "             14-trzymane 15-twarz 16-u`zywane jako `swiat`lo\n\r"
		  " -typ !<nazwa_typu> poda typy prz. r`o`zne od <nazwa_typu>\n\r"
		  " pszukaj -finfo wy`swietla dost`epne flagi.\n\r"
		  " pszukaj -tinfo wy`swietla dost`epne typy przedmiot`ow.\n\r",
		ch );
	return;
    }

    buf1[ 0 ] = '\0';
    fAll    = !str_cmp( arg, "wsz" );
    found   = FALSE;
    nMatch  = 0;

    if ( dopliku && !( plik = fopen( PSZUKAJ_FILE, "w" ) ) )
    {
	OFSTC( "Nie uda`lo si`e otworzy`c pliku " PSZUKAJ_FILE " do zapisu.\n\r", ch );
	return;
    }

    if ( dopliku )
    {
	strcpy( buf, "vnum~mianownik~kraina~min. poziom~typ~ci`e`zar~wielko`s`c~cena X~poziom X~cena Q~flagi~ubranie~antyrasy~antyprofesje~w0~w0~w1~w1~w2~w2~w3~w3~w4~w4~w5~w5~" );

	for ( licznik = 0; licznik <= APPLY_RACE; licznik++ )
	    sprintf( buf + strlen( buf ), "na %s~", affect_loc_name_pl_b( licznik ) );

	fprintf( plik, "%s\n", buf );
    }

    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pObjIndex->name ) )
	    {
		/* if ( zaduzo > 500 )  OSTRZEZENIE
		    break;                     */

		if ( gdzie && ( gdzie != b16 ) && !IS_SET( pObjIndex->wear_flags, gdzie ) )
			continue;

		/* Lam: nie ma wear_flag do uzywania jako swiatlo, do_wear
		   sprawdza typ przedmiotu */
		/*
		if ( fWear && !fLight && pObjIndex->item_type == ITEM_LIGHT )
		    continue;
		 */
		if ( gdzie && ( gdzie == b16 ) && pObjIndex->item_type != ITEM_LIGHT )
		    continue;

		if ( fType && pObjIndex->item_type != typek && !odwrot )
		    continue;

		if ( fType && pObjIndex->item_type == typek && odwrot )
		    continue;

		/* Lam 5.6.2005: kilka -fla oznacza "ORAZ" */
		if ( fFlag )
		{
		    COPY_BITS( zbitaflaga, pObjIndex->extra_flags );
		    AND_BITS( zbitaflaga, flaga );
		    if ( !SAME_BITS( zbitaflaga, flaga ) )
			continue;
		}

		if ( fMaxL && pObjIndex->min_reset_level > maxpoz )
		    continue;

		if ( fMinL && pObjIndex->min_reset_level < minpoz )
		    continue;

		if ( fSklep && !pObjIndex->w_sklepie )
		    continue;

		if ( fRasa
		  && ( ( !IS_OBJ_STAT( pObjIndex, ITEM_INVERT_RACES )
		      && is_name2( race_table[ szrasa ].ang, pObjIndex->anti_race ) )
		    || ( IS_OBJ_STAT( pObjIndex, ITEM_INVERT_RACES )
		      && !is_name2( race_table[ szrasa ].ang, pObjIndex->anti_race ) ) ) )
		    continue;

		if ( fProf
		  && ( ( !IS_OBJ_STAT( pObjIndex, ITEM_INVERT_CLASSES )
		      && is_name2( class_table[ szprof ].who_name, pObjIndex->anti_class ) )
		    || ( IS_OBJ_STAT( pObjIndex, ITEM_INVERT_CLASSES )
		      && !is_name2( class_table[ szprof ].who_name, pObjIndex->anti_class ) ) ) )
		    continue;

		if ( fMaterial
		  && ( ( pObjIndex->item_type != ITEM_ARMOR
		      && pObjIndex->item_type != ITEM_WEAPON )
		    || pObjIndex->value[ 5 ].v != szmat ) )
		    continue;

		if ( fCzar )
		{
		    int pzl;
		    bool zczarami = FALSE;

		    for ( pzl = 0; pzl < 6; pzl++ )
			if ( typy_przedmiotow[ pObjIndex->item_type ].spell[ pzl ] == VAL_SPELL )
			{
			    zczarami = TRUE;
			    if ( pObjIndex->value[ pzl ].v == szczar )
				break;
			}

		    /* pszuk -typ jedzenie -czar otrucie nie zadziala */
		    if ( !zczarami || pzl == 6 )
			continue;
		}

		if ( !ch && PRZERABIACZ )
		{
		    char bufcio[ MIL ];
		    OBJ_DATA *przedmiocio = create_object( pObjIndex, 100 );

		    printf( ESCS "[H" ESCS "[2J\n\r\n\r\n\r" );
		    sprintf( bufcio, "%d", pObjIndex->vnum );
		    do_ostat( NULL, bufcio );
		    /* printf( ESCS "[1;1HStara waga: %d\n\rNowa waga : ", pObjIndex->weight );
		    scanf( "%d", &pObjIndex->weight ); */
		    printf( ESCS "[1;1HObecnie brzeczy: %s\n\rCzy ma brzeczec: ",
			IS_OBJ_STAT( pObjIndex, ITEM_HUM ) ? "TAK" : "nie" );

		    while ( !scanf( "%s", bufcio ) )
			;

		    if ( UPPER( *bufcio ) == 'T' )
			SET_BIT_V( pObjIndex->extra_flags, ITEM_HUM );
		    else
			REMOVE_BIT_V( pObjIndex->extra_flags, ITEM_HUM );

		    printf( ESCS "[H" ESCS "[2J" );
		    extract_obj( przedmiocio );
		}

		if ( dopliku )
		{
		    found = TRUE;

		    sprintf( buf, "%d~%s~%s~%d~%s~%d~%d~%d~%d~%d~%s~%s~%s~%s~",
			pObjIndex->vnum,
			pObjIndex->short_descr,
			pObjIndex->area->file_name,
			pObjIndex->min_reset_level,
			typy_przedmiotow[ pObjIndex->item_type ].name_pl,
			pObjIndex->weight,
			pObjIndex->wielkosc,
			pObjIndex->cena,
			pObjIndex->poziom,
			pObjIndex->qp,
			extra_bit_name_pl( pObjIndex->extra_flags, ch ),
			wear_flag_name_pl( pObjIndex->wear_flags ),
			pObjIndex->anti_race,
			pObjIndex->anti_class );

		    for ( licznik = 0; licznik < 6; licznik++ )
		    {
			sprintf( buf + strlen( buf ), "%s~", typy_przedmiotow[ pObjIndex->item_type ].value[ licznik ] );
			if ( typy_przedmiotow[ pObjIndex->item_type ].spell[ licznik ] == VAL_SPELL )
			    sprintf( buf + strlen( buf ), "%s~",
				 pObjIndex->value[ licznik ].v > 0 && pObjIndex->value[ licznik ].v < MAX_SKILL
				 ? skill_table[ pObjIndex->value[ licznik ].v ].pl_name : "(brak)" );
			else if ( typy_przedmiotow[ pObjIndex->item_type ].spell[ licznik ] == VAL_MATERIAL )
			    sprintf( buf + strlen( buf ), "%s~",
				 pObjIndex->value[ licznik ].v > 0 && pObjIndex->value[ licznik ].v < MAX_MATERIAL
				 ? material_table[ pObjIndex->value[ licznik ].v ].name : "(nic)" );
			/* Alandar 6.10.2004: obsluga wartosci bedacych opisami */
			else if ( typy_przedmiotow[ pObjIndex->item_type ].spell[ licznik ] == VAL_STRING )
			    sprintf( buf + strlen( buf ), "%s~",
				 pObjIndex->value[ licznik ].p
				 ? ( char * ) pObjIndex->value[ licznik ].p : "(brak)" );
			/* Alandar 12.10.2004: wypisywanie nazw napojow */
			else if ( typy_przedmiotow[ pObjIndex->item_type ].spell[ licznik ] == VAL_LIQUID )
			    sprintf( buf + strlen( buf ), "%s~",
				 pObjIndex->value[ licznik ].v >= 0 && pObjIndex->value[ licznik ].v < LIQ_MAX
				 ? liq_table_pl[ pObjIndex->value[ licznik ].v ].liq_name : "(b`l`edny p`lyn)" );
			else
			    sprintf( buf + strlen( buf ), "%d~", pObjIndex->value[ licznik ].v );
		    }

		    for ( licznik = 0; licznik <= APPLY_RACE; licznik++ )
		    {
			sumator = 0;

			for ( paf = pObjIndex->affected; paf; paf = paf->next )
			    if ( paf->location == licznik )
				sumator += paf->modifier;

			sprintf( buf + strlen( buf ), "%d~", sumator );
		    }

		    fprintf( plik, "%s\n", buf );
		}
		else if ( aff != APPLY_NONE )
		{
		    for ( paf = pObjIndex->affected; paf; paf = paf->next )
			if ( paf->location == aff )
			    break;

		    if ( paf )
		    {
			found = TRUE;
			/* zaduzo++; OSTRZEZENIE */
			sprintf( buf, "[%5d] (%4d) <%3d> %s\n\r",
				pObjIndex->vnum,
				paf->modifier,
				pObjIndex->min_reset_level,
				pObjIndex->short_descr );
			if ( !fAll
			  || ( fAll && gdzie )
			  || ( fAll && fType )
			  || ( fAll && fFlag ) )
			{
			    strcat( buf1, buf );
			}
			else
			    OFSTC( buf, ch );
		    }
		}
		else
		{
		    found = TRUE;
		    /* zaduzo++; OSTRZEZENIE */
		    if ( fMinL || fMaxL )
		    {
			sprintf( buf, "[%5d] <%3d> %s\n\r",
			    pObjIndex->vnum,
			    pObjIndex->min_reset_level,
			    pObjIndex->short_descr );
		    }
		    else
		    {
			sprintf( buf, "[%5d] %s\n\r",
				pObjIndex->vnum, pObjIndex->short_descr );
		    }

		    if ( !fAll
		      || ( fAll && gdzie )
		      || ( fAll && fType )
		      || ( fAll && fFlag ) )
		    {
			strcat( buf1, buf );
		    }
		    else
			OFSTC( buf, ch );
		}
	    }
	}
    }

    if ( dopliku )
    {
	OFSTC( "Wyj`scie przekierowane do pliku " PSZUKAJ_FILE ".\n\r", ch );
	fclose( plik );
    }

    if ( !found )
	OFSTC( "Nie ma niczego odpowiadaj`acego twojemu zapytaniu.\n\r", ch );

    if ( dopliku || !found )
	return;

    if ( !fAll
      || ( fAll && gdzie )
      || ( fAll && fType )
      || ( fAll && fFlag ) )
    {
	OFSTC( buf1, ch );
    }

    return;
}

#undef OFSTC


/*
 * Qwert 02.11.2000: mgdzie vnum
 * Lam 3.1.2004: przepelnienie bufora, zliczanie pasujacych ponad bufor,
 *		zliczanie numeru dla argumentow numer.mob
 */
KOMENDA( do_mwhere )
{
    CHAR_DATA *victim;
    CHAR_DATA *nast;
    char       buf[ MAX_STRING_LENGTH ];
    char       buf1[ MAX_STRING_LENGTH * 8 ];
    char       arg[ MAX_INPUT_LENGTH ];
    bool       found;
    bool       za_duzo = FALSE;
    bool       szukamvnum = FALSE;
    bool       w_pomieszczeniu = TRUE;
    int        numerek = 0;
    int        ilu_wyswietlono = 0;
    int        ilu_bylo = 0;

    if ( !authorized( get_char( ch ), "mgdzie" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: mgdzie <imi`e> \n\r", ch );
	send_to_char( "          mgdzie <vnum> \n\r", ch );
	return;
    }

    if ( is_number( arg ) )
    {
	numerek = atoi( arg );
	szukamvnum = TRUE;
    }

    buf1[ 0 ] = '\0';
    found  = FALSE;

    for ( victim = ch->in_room->people; victim; victim = nast )
    {
	if ( ( !szukamvnum
	    && can_see( ch, victim )
	    && victim->in_room
	    && ( w_pomieszczeniu || ch->in_room != victim->in_room )
	    && is_name( arg, victim->name ) )
	  || ( szukamvnum
	    && IS_NPC( victim )
	    && can_see( ch, victim )
	    && ( w_pomieszczeniu || ch->in_room != victim->in_room )
	    && victim->in_room
	    && victim->pIndexData->vnum == numerek ) )
	{
	    found = TRUE;
	    ilu_bylo++;

	    if ( !za_duzo )
	    {
		ilu_wyswietlono++;

		if ( IS_NPC( victim ) )
		    sprintf( buf, "%3d. [%5d] %s [%5d] %s\n\r",
			ilu_bylo,
			victim->pIndexData->vnum,
			wyrownaj( victim->short_descr, -28 ),
			victim->in_room->vnum,
			victim->in_room->name );
		else
		    sprintf( buf, "%3d. [gracz] %s [%5d] %s\n\r",
			ilu_bylo,
			wyrownaj( victim->short_descr, -28 ),
			victim->in_room->vnum,
			victim->in_room->name );

		strcat( buf1, buf );

		if ( strlen( buf1 ) > MSL * 7 )
		{
		    strcpy( buf, "Za du`zo tekstu!\n\r" );
		    strcat( buf1, buf );
		    za_duzo = TRUE;
		}
	    }
	}

	if ( w_pomieszczeniu )
	{
	    if ( victim->next_in_room )
		nast = victim->next_in_room;
	    else
	    {
		nast = char_list;
		w_pomieszczeniu = FALSE;
	    }
	}
	else
	    nast = victim->next;
    }

    if ( !found && !szukamvnum )
    {
	act_pp( ZM_ZAWSZE, "Nie widzisz `zadnego \"$T\".", ch, NULL, arg, TO_CHAR );
	return;
    }

    if ( !found && szukamvnum )
    {
	STC( "Mob o tym vnumie nie znajduje si`e w grze.\n\r", ch );
	return;
    }

    if ( za_duzo )
    {
	sprintf( buf, "{WWy`swietlono %d mob`ow spo`sr`od %d znalezionych!{x\n\r",
		ilu_wyswietlono, ilu_bylo );
	strcat( buf1, buf );
    }

    send_to_char( buf1, ch );

    return;
}


KOMENDA( do_reboo )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "jebut" ) )
	return;

    send_to_char( "Je`sli chcesz zrobi`c jebuta, napisz to w ca`lo`sci.\n\r", ch );

    return;
}


KOMENDA( do_reboot )
{
    CHAR_DATA *rch;
    char       buf[ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "jebut" ) )
	return;

    if ( ch->desc && ch->desc->original )
/*      do_return( ch, "" ); piesek nie restartuje, zapisze sie tez postac */
    {
	send_to_char( "Powracasz do swojego cia`la.\n\r", ch );
	rch                                  = ch->desc->original;
	ch->desc->original->pcdata->switched = FALSE;
	ch->desc->character                  = ch->desc->original;
	ch->desc->original                   = NULL;
	ch->desc->character->desc            = ch->desc;
	ch->desc                             = NULL;
	ch                                   = rch;
    }

    merc_down = TRUE;
    sprintf( buf, "%s restartuje gr`e.\n\r", ch->short_descr );
    send_to_all_char( buf );

    if ( auction->item )
	do_auction( ch, "stop" );

    end_of_game( );

    return;
}


KOMENDA( do_shutdow )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "wy`l`acz" ) )
	return;

    send_to_char( "Je`sli naprawd`e musisz wy`l`aczy`c gr`e, napisz to w ca`lo`sci.\n\r", ch );

    return;
}


KOMENDA( do_shutdown )
{
    CHAR_DATA *rch;
    char       buf[ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "wy`l`acz" ) )
	return;

    if ( ch->desc && ch->desc->original )
    {
	send_to_char( "Powracasz do swojego cia`la.\n\r", ch );
	rch                                  = ch->desc->original;
	ch->desc->original->pcdata->switched = FALSE;
	ch->desc->character                  = ch->desc->original;
	ch->desc->original                   = NULL;
	ch->desc->character->desc            = ch->desc;
	ch->desc                             = NULL;
	ch                                   = rch;
    }

    sprintf( buf, "%s ko`nczy gr`e.", ch->short_descr );
    append_file( ch, SHUTDOWN_FILE, buf );
    strcat( buf, "\n\r" );
    send_to_all_char( buf );
    merc_down = TRUE;

    if ( auction->item )
	do_auction( ch, "stop" );

    end_of_game( );

    return;
}


KOMENDA( do_snoop )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "pluskwa" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Komu pod`l`aczy`c pluskw`e?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widz`e nikogo takiego.\n\r", ch );
	return;
    }

    if ( !victim->desc )
    {
	send_to_char( "Twoja ofiara nie jest po`l`aczonym graczem.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Usuwasz wszystkie pluskwy.\n\r", ch );

	for ( d = descriptor_list; d; d = d->next )
	    if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;

	return;
    }

    if ( victim->desc->snoop_by )
    {
	send_to_char( "Zaj`ete!\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( ch->desc )
	for ( d = ch->desc->snoop_by; d; d = d->snoop_by )
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "Nie wolno zap`etla`c podgl`adania.\n\r", ch );
		return;
	    }

    victim->desc->snoop_by = ch->desc;
    send_to_char( "Pluskwa w`l`aczona.\n\r", ch );

    return;
}


KOMENDA( do_switch )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "op`etaj" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo op`eta`c?\n\r", ch );
	return;
    }

    if ( !ch->desc )
	return;

    if ( ch->desc->original )
    {
	ASTC( "Ju`z kogo`s op`eta`l$o`s.", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "Nie widz`e takiej postaci.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Zrobione. :)\n\r", ch );
	return;
    }

    /*
     * Pointed out by Da Pub (What Mud)
     */
    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Nie wolno ci op`etywa`c graczy!\n\r", ch );
	return;
    }

    if ( victim->desc )
    {
	send_to_char( "Ta posta`c jest ju`z zaj`eta.\n\r", ch );
	return;
    }

    ch->pcdata->switched  = TRUE;
    ch->desc->character   = victim;
    ch->desc->original    = ch;
    victim->desc          = ch->desc;
    ch->desc              = NULL;
    victim->polskie       = ch->polskie;
    victim->kolor	  = ch->kolor;
    send_to_char( "Op`etujesz posta`c.\n\r", victim );

    return;
}


KOMENDA( do_return )
{
    if ( !ch->desc )
	return;

    if ( !ch->desc->original )
    {
	ASTC( "Nikogo nie op`eta`l$o`s.", ch );
	return;
    }

/* Note that we dont check for immortal ability to have return here.
 * We assume we will automatically allow immortals with switch to return.
 * Dont want to have our immortals stuck in a mobile's body do we?  :)
 * -Kahn */

    send_to_char( "Powracasz do swojego cia`la.\n\r", ch );
    ch->desc->original->pcdata->switched = FALSE;
    ch->desc->character                  = ch->desc->original;
    ch->desc->original                   = NULL;
    ch->desc->character->desc            = ch->desc;
    ch->desc                             = NULL;

    return;
}


KOMENDA( do_mload )
{
    CHAR_DATA      *rch;
    CHAR_DATA      *victim;
    MOB_INDEX_DATA *pMobIndex;
    char            arg[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "m`laduj" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || !is_number( arg ) )
    {
	send_to_char( "Sk`ladnia: m`laduj <vnum>.\n\r", ch );
	return;
    }

    if ( atoi( arg ) == 3 )
    {
	send_to_char( "Nie wolno `ladowa`c supermoba -Lam\n\r", ch );
	return;
    }

    if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
    {
	send_to_char( "Nie ma moba o tym vnumie.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    send_to_char( "Za`ladowano.\n\r", ch );
    act( ZM_WZROK, "$n stworzy`l$a $#!", ch, NULL, victim, TO_ROOM );

    return;
}


KOMENDA( do_oload )
{
    OBJ_DATA       *obj;
    CHAR_DATA      *rch;
    OBJ_INDEX_DATA *pObjIndex;
    char            arg1[ MAX_INPUT_LENGTH ];
    char            arg2[ MAX_INPUT_LENGTH ];
    int             level;

    rch = get_char( ch );

    if ( !authorized( rch, "p`laduj" ) )
	return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' || !is_number( arg1 ) )
    {
	send_to_char( "Sk`ladnia: p`laduj <vnum> [poziom].\n\r", ch );
	return;
    }

    if ( arg2[ 0 ] == '\0' )
    {
	level = get_trust( ch );
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
	if ( !is_number( arg2 ) )
	{
	    send_to_char( "Sk`ladnia: p`laduj <vnum> [poziom].\n\r", ch );
	    return;
	}
	level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
	{
	    send_to_char( "Nie mo`zesz nada`c poziomu wy`zszego ni`z twoje zaufanie.\n\r", ch );
	    return;
	}
    }

    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
	send_to_char( "Nie ma przedmiotu o tym vnumie.\n\r", ch );
	return;
    }

    if ( IS_SET_V( pObjIndex->extra_flags, ITEM_BURIED ) )
    {
	STC( "Ten przedmiot s`lu`zy wy`l`acznie do resetowania na ziemi, gdy`z jest zakopany.\n\r", ch );
	return;
    }

    /* Cadaver 6.12.2001: */
    if ( level < UMIN( 100, pObjIndex->min_reset_level ) && get_trust( ch ) < L_SEN )
    {
	send_to_char( "Nie mo`zesz nada`c poziomu ni`zszego ni`z nominalny.\n\r", ch );
	return;
    }

    obj = create_object( pObjIndex, level );

    if ( CAN_WEAR( obj, ITEM_TAKE ) )
    {
	obj_to_char( obj, ch );
    }
    else
    {
	obj_to_room( obj, ch->in_room );
	act( ZM_WZROK, "$n stworzy`l$a $j!", ch, obj, NULL, TO_ROOM );
    }

    send_to_char( "Za`ladowano.\n\r", ch );

    return;
}


KOMENDA( do_purge )
{
    OBJ_DATA  *obj;
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "wymie`c" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	/* "wymiec" */
	OBJ_DATA  *obj_next;
	CHAR_DATA *vnext;

	for ( victim = ch->in_room->people; victim; victim = vnext )
	{
	    vnext = victim->next_in_room;

	    if ( victim->deleted )
		continue;

	    if ( IS_NPC( victim ) && victim != ch )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->deleted )
		continue;

	    extract_obj( obj );
	}

	send_to_char( "Oczyszczono.\n\r", ch );
	act( ZM_WZROK, "$n oczyszcza pomieszczenie!", ch, NULL, NULL, TO_ROOM);
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "Nie widz`e takiej postaci.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Nie na graczach!\n\r", ch );
	return;
    }

    act( ZM_ZAWSZE, "Wymiatasz $#.", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "$n wymiata $#.", ch, NULL, victim, TO_NOTVICT );
    extract_char( victim, TRUE );

    return;
}


KOMENDA( do_advance )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    int        level;
    int        iLevel;

    rch = get_char( ch );

    if ( !authorized( rch, "awans" ) )
	return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Sk`ladnia: awans <posta`c> <poziom>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char( "W pomieszczeniu nie ma tego gracza.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie na mobach.\n\r", ch );
	return;
    }

    level = atoi( arg2 );

    if ( level < 1 || level > MAX_LEVEL )
    {
	char buf[ MAX_STRING_LENGTH ];

	sprintf( buf, "Poziom jest z zakresu od 1 do %d.\n\r", MAX_LEVEL );
	send_to_char( buf, ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Nie mo`zesz stworzy`c boga wi`ekszego od siebie.\n\r", ch );
	return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
    if ( level <= victim->level )
    {
	int sn;

	send_to_char( "Obni`zanie poziomu!\n\r", ch );
	send_to_char( "**** O NIE! ****\n\r",    victim );
	victim->level    = 1;
	victim->exp      = EXP_PER_LEVEL;
	victim->max_hit  = 10;
	victim->max_mana = 100;
	victim->max_move = 100;

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	    victim->pcdata->learned[ sn ] = 0;

	victim->practice = 0;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
	advance_level( victim );
	/* Lam 22.3.99: boska niewidka */
	REMOVE_BIT( victim->act, PLR_WIZINVIS );
	victim->pcdata->wizinvis = 0;
    }
    else
    {
	send_to_char( "Zwi`ekszanie poziomu!\n\r", ch );
	send_to_char( "**** O TAK! ****\n\r", victim );
    }

    for ( iLevel = victim->level; iLevel < level; iLevel++ )
    {
	send_to_char( "Zdobywasz poziom! ", victim );
	victim->level += 1;
	advance_level( victim );
    }

    victim->exp = EXP_PER_LEVEL * UMAX( 1, victim->level );

    if ( ch != victim )
	victim->trust = 0;

    return;
}


KOMENDA( do_trust )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    int        level;

    rch = get_char( ch );

    if ( !authorized( rch, "zaufaj" ) )
	return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Sk`ladnia: zaufaj <posta`c> <poziom>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
	return;
    }

    level = atoi( arg2 );

    if ( level < 0 || level > MAX_LEVEL )
    {
	char buf[ MAX_STRING_LENGTH ];

	sprintf( buf, "Zaufanie jest z przedzia`lu od 1 do %d.\n\r", MAX_LEVEL );
	send_to_char( buf, ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Nie mo`zesz stworzy`c boga wi`ekszego od siebie.\n\r", ch );
	return;
    }

    victim->trust = level;

    return;
}


KOMENDA( do_restore )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "odn`ow" ) )
	return;

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo odnowi`c [imi`e/wsz/pc]?\n\r", ch );
	return;
    }

    /* Restore All feature coded by Katrina */
    if ( !str_cmp( arg, "wsz" ) )
    {
	for ( victim = char_list; victim; victim = victim->next )
	{
	    if ( victim->deleted )
		continue;

	    victim->hit  = UMAX( victim->max_hit, victim->hit );
	    victim->mana = UMAX( victim->max_mana, victim->mana );
	    victim->move = UMAX( victim->max_move, victim->move );
	    update_pos( victim );
	    act( ZM_ZAWSZE, "$n ci`e odnowi`l$a.", ch, NULL, victim, TO_VICT );
	}
	ASTC( "Ale`z jeste`s wspania`lomy`sln$y!", ch );
    }
    else if ( !str_cmp( arg, "pc" ) ) /* Lam */
    {
	for ( victim = char_list; victim; victim = victim->next )
	{
	    if ( victim->deleted || IS_NPC( victim ) )
		continue;

	    victim->hit  = UMAX( victim->max_hit, victim->hit );
	    victim->mana = UMAX( victim->max_mana, victim->mana );
	    victim->move = UMAX( victim->max_move, victim->move );
	    update_pos( victim );
	    act( ZM_ZAWSZE, "$n ci`e odnowi`l$a.", ch, NULL, victim, TO_VICT );
	}
	send_to_char( "Ale`z z ciebie `laskawca!\n\r", ch );
    }
    else
    {
	if ( !( victim = get_char_world2( ch, arg ) ) )
	{
	    send_to_char( "Nie widzisz takiej postaci (nie u`zywaj skr`ot`ow).\n\r", ch );
	    return;
	}

	victim->hit  = UMAX( victim->max_hit, victim->hit );
	victim->mana = UMAX( victim->max_mana, victim->mana );
	victim->move = UMAX( victim->max_move, victim->move );
	update_pos( victim );
	act( ZM_ZAWSZE, "$n ci`e odnowi`l$a.", ch, NULL, victim, TO_VICT );
	send_to_char( "Zrobione.\n\r", ch );
    }

    return;
}


KOMENDA( do_freeze )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    char kto[ MIL * 2 ];
    time_t dokiedy = 0;
    char buf[ MIL ];

    rch = get_char( ch );

    if ( !authorized( rch, "mro`zonka" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo zamrozi`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widz`e takiego gracza.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Tylko gracze nadaj`a si`e na mro`zonki.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( !str_prefix( "-na ", argument ) )
    {
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg );

	dokiedy = policz_czas( arg );
    }

    if ( IS_SET( victim->act, PLR_FREEZE ) )
    {
	REMOVE_BIT( victim->act, PLR_FREEZE );
	victim->pcdata->freeze.do_kiedy = 0;
	free_string( victim->pcdata->freeze.od );
	victim->pcdata->freeze.od = str_dup( "" );
	send_to_char( "Usuni`eta flaga MRO`ZONKA.\n\r", ch );
	send_to_char( "Znowu mo`zesz gra`c.\n\r", victim );
    }
    else
    {
	if ( dokiedy > 0 )
	    sprintf( buf, " (do %s)", asctime_pl_krotko_nonl( localtime( &dokiedy ) ) );
	else
	    strcpy( buf, "" );

	if ( *argument )
	    sprintf( kto, "%s %s%s: %s", rch->name, ascdata( localtime( &current_time ) ), buf, argument );
	else
	    sprintf( kto, "%s %s%s", rch->name, ascdata( localtime( &current_time ) ), buf );

	SET_BIT( victim->act, PLR_FREEZE );
	victim->pcdata->freeze.do_kiedy = dokiedy;
	free_string( victim->pcdata->freeze.od );
	victim->pcdata->freeze.od = str_dup( kto );
	send_to_char( "Ustawiona flaga MRO`ZONKA.\n\r", ch );
	send_to_char( "Nie mo`zesz NICZEGO robi`c!\n\r", victim );
    }

    save_char_obj( victim );

    return;
}


KOMENDA( do_log )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    char kto[ MIL * 2 ];
    time_t dokiedy = 0;

    rch = get_char( ch );

    if ( !authorized( rch, "log" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo logowa`c? Sk`ladnia: log <posta`c|wsz>\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "wsz" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Od tej chwili nie wszystko si`e loguje.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "Od tej chwili loguje si`e wszystko!\n\r",  ch );
	}
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widz`e takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie loguj mob`ow.\n\r", ch );
	return;
    }

    if ( !str_prefix( "-na ", argument ) )
    {
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg );

	dokiedy = policz_czas( arg );
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET( victim->act, PLR_LOG ) )
    {
	REMOVE_BIT( victim->act, PLR_LOG );
	victim->pcdata->log.do_kiedy = 0;
	free_string( victim->pcdata->log.od );
	victim->pcdata->log.od = str_dup( "" );

	if ( victim->pcdata->fLog )
	    fclose( victim->pcdata->fLog );

	victim->pcdata->fLog = NULL;
	send_to_char( "LOG usuni`ety.\n\r", ch );
    }
    else
    {
	char logfile[ MSL ];
	char buf[ MIL ];

	if ( dokiedy > 0 )
	    sprintf( buf, " (do %s)", asctime_pl_krotko_nonl( localtime( &dokiedy ) ) );
	else
	    strcpy( buf, "" );

	if ( *argument )
	    sprintf( kto, "%s %s%s: %s", rch->name, ascdata( localtime( &current_time ) ), buf, argument );
	else
	    sprintf( kto, "%s %s%s", rch->name, ascdata( localtime( &current_time ) ), buf );

	SET_BIT( victim->act, PLR_LOG );
	victim->pcdata->log.do_kiedy = dokiedy;
	free_string( victim->pcdata->log.od );
	victim->pcdata->log.od = str_dup( kto );

	sprintf( logfile, "../log/player/%s.log",
		 capitalize( victim->name ) );

	if ( !( victim->pcdata->fLog = fopen( logfile, "a" ) ) )
	{
	    sprintf( buf, "do_log: fopen %s: ", victim->name );
	    bug( buf, 0 );
	    lac_perror( logfile );
	    STC( "LOG ustawiony, ale plik nie otwarty!\n\r", ch );
	}
	else
	{
	    setvbuf( victim->pcdata->fLog, (char *) NULL, _IOLBF, 0 );
	    send_to_char( "LOG ustawiony.\n\r", ch );
	}
    }

    return;
}


/*
 * Qwert 18.03.2001
 */
 KOMENDA( do_love )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    char kto[ MIL * 2 ];
    time_t dokiedy = 0;
    char buf[ MIL ];

    rch = get_char( ch );

    if ( !authorized( rch, "ukochaj" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo obdarzy`c mi`lo`sci`a?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Moby tego nie potrzebuj`a.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( !str_prefix( "-na ", argument ) )
    {
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg );

	dokiedy = policz_czas( arg );
    }

    if ( IS_SET( victim->act, PLR_LOVE ) )
    {
	REMOVE_BIT( victim->act, PLR_LOVE );
	victim->pcdata->love.do_kiedy = 0;
	free_string( victim->pcdata->love.od );
	victim->pcdata->love.od = str_dup( "" );
	send_to_char( "Flaga mi`lo`sci zdj`eta.\n\r", ch );
    /*    send_to_char( "Teraz ju`z nikt ci`e nie kocha.\n\r", victim ); */
    }
    else
    {
	if ( dokiedy > 0 )
	    sprintf( buf, " (do %s)", asctime_pl_krotko_nonl( localtime( &dokiedy ) ) );
	else
	    strcpy( buf, "" );

	if ( *argument )
	    sprintf( kto, "%s %s%s: %s", rch->name, ascdata( localtime( &current_time ) ), buf, argument );
	else
	    sprintf( kto, "%s %s%s", rch->name, ascdata( localtime( &current_time ) ), buf );

	SET_BIT( victim->act, PLR_LOVE );
	victim->pcdata->love.do_kiedy = dokiedy;
	free_string( victim->pcdata->love.od );
	victim->pcdata->love.od = str_dup( kto );
    /*    send_to_char( "Teraz wszyscy ci`e kochaj`a!\n\r", victim ); */
	send_to_char( "Ustawiona flaga mi`lo`sci.\n\r", ch );
    }

    return;
}


KOMENDA( do_noemote )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    char kto[ MIL * 2 ];
    time_t dokiedy = 0;
    char buf[ MIL ];

    rch = get_char( ch );

    if ( !authorized( rch, "bezemocji" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo pozbawi`c emocji?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Moby i tak nie emocjonuj`a si`e przesadnie.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( !str_prefix( "-na ", argument ) )
    {
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg );

	dokiedy = policz_czas( arg );
    }

    if ( IS_SET( victim->act, PLR_NO_EMOTE ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_EMOTE );
	victim->pcdata->noemote.do_kiedy = 0;
	free_string( victim->pcdata->noemote.od );
	victim->pcdata->noemote.od = str_dup( "" );
	send_to_char( "BEZ EMOCJI zdj`ete.\n\r", ch );
	send_to_char( "Znowu mo`zesz okazywa`c swoje emocje.\n\r", victim );
    }
    else
    {
	if ( dokiedy > 0 )
	    sprintf( buf, " (do %s)", asctime_pl_krotko_nonl( localtime( &dokiedy ) ) );
	else
	    strcpy( buf, "" );

	if ( *argument )
	    sprintf( kto, "%s %s%s: %s", rch->name, ascdata( localtime( &current_time ) ), buf, argument );
	else
	    sprintf( kto, "%s %s%s", rch->name, ascdata( localtime( &current_time ) ), buf );

	SET_BIT( victim->act, PLR_NO_EMOTE );
	victim->pcdata->noemote.do_kiedy = dokiedy;
	free_string( victim->pcdata->noemote.od );
	victim->pcdata->noemote.od = str_dup( kto );
	send_to_char( "Nie mo`zesz okazywa`c swych emocji!\n\r", victim );
	send_to_char( "Ustawiono BEZ EMOCJI.\n\r", ch );
    }

    return;
}


/*
 * Lam 12.10.2004: skopiowane z bezemocji
 */
KOMENDA( do_bezkoloru )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    char kto[ MIL * 2 ];
    time_t dokiedy = 0;
    char buf[ MIL ];

    rch = get_char( ch );

    if ( !authorized( rch, "bezkoloru" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo pozbawi`c kolor`ow?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Moby i tak nie koloryzuj`a tak jak gracze.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( !str_prefix( "-na ", argument ) )
    {
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg );

	dokiedy = policz_czas( arg );
    }

    if ( IS_SET( victim->act, PLR_NO_COLOUR ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_COLOUR );
	victim->pcdata->nocolour.do_kiedy = 0;
	free_string( victim->pcdata->nocolour.od );
	victim->pcdata->nocolour.od = str_dup( "" );
	send_to_char( "BEZ KOLORU zdj`ete.\n\r", ch );
	send_to_char( "Znowu mo`zesz u`zywa`c kolor`ow.\n\r", victim );
    }
    else
    {
	if ( dokiedy > 0 )
	    sprintf( buf, " (do %s)", asctime_pl_krotko_nonl( localtime( &dokiedy ) ) );
	else
	    strcpy( buf, "" );

	if ( *argument )
	    sprintf( kto, "%s %s%s: %s", rch->name, ascdata( localtime( &current_time ) ), buf, argument );
	else
	    sprintf( kto, "%s %s%s", rch->name, ascdata( localtime( &current_time ) ), buf );

	SET_BIT( victim->act, PLR_NO_COLOUR );
	victim->pcdata->nocolour.do_kiedy = dokiedy;
	free_string( victim->pcdata->nocolour.od );
	victim->pcdata->nocolour.od = str_dup( kto );
	send_to_char( "Nie mo`zesz ju`z u`zywa`c kolor`ow!\n\r", victim );
	send_to_char( "Ustawiono BEZ KOLORU.\n\r", ch );
    }

    return;
}


KOMENDA( do_notell )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    char kto[ MIL * 2 ];
    time_t dokiedy = 0;
    char buf[ MIL ];

    rch = get_char( ch );

    if ( !authorized( rch, "bezmowy" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo pozbawi`c mowy?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widz`e nikogo takiego.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie na mobach.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( !str_prefix( "-na ", argument ) )
    {
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg );

	dokiedy = policz_czas( arg );
    }

    if ( IS_SET( victim->act, PLR_NO_TELL ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_TELL );
	victim->pcdata->notell.do_kiedy = 0;
	free_string( victim->pcdata->notell.od );
	victim->pcdata->notell.od = str_dup( "" );
	send_to_char( "BEZ MOWY usuni`ete.\n\r", ch );
	send_to_char( "Zn`ow mo`zesz m`owi`c.\n\r", victim );
    }
    else
    {
	if ( dokiedy > 0 )
	    sprintf( buf, " (do %s)", asctime_pl_krotko_nonl( localtime( &dokiedy ) ) );
	else
	    strcpy( buf, "" );

	if ( *argument )
	    sprintf( kto, "%s %s%s: %s", rch->name, ascdata( localtime( &current_time ) ), buf, argument );
	else
	    sprintf( kto, "%s %s%s", rch->name, ascdata( localtime( &current_time ) ), buf );

	SET_BIT(    victim->act, PLR_NO_TELL );
	victim->pcdata->notell.do_kiedy = dokiedy;
	free_string( victim->pcdata->notell.od );
	victim->pcdata->notell.od = str_dup( kto );
	send_to_char( "BEZ MOWY ustawione.\n\r", ch );
	send_to_char( "Nie mo`zesz si`e porozumiewa`c!\n\r", victim );
    }

    return;
}


KOMENDA( do_silence )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    char kto[ MIL * 2 ];
    time_t dokiedy = 0;
    char buf[ MIL ];

    rch = get_char( ch );

    if ( !authorized( rch, "ucisz" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo uciszy`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widz`e takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie na mobach.\n\r", ch );
	return;
    }

    if ( victim != ch && get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( !str_prefix( "-na ", argument ) )
    {
	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg );

	dokiedy = policz_czas( arg );
    }

    if ( IS_SET( victim->act, PLR_SILENCE ) )
    {
	REMOVE_BIT( victim->act, PLR_SILENCE );
	victim->pcdata->silence.do_kiedy = 0;
	free_string( victim->pcdata->silence.od );
	victim->pcdata->silence.od = str_dup( "" );
	send_to_char( "Znowu mo`zesz u`zywa`c kana`l`ow.\n\r", victim );
	send_to_char( "Zdj`eta flaga UCISZONY.\n\r", ch );
    }
    else
    {
	if ( dokiedy > 0 )
	    sprintf( buf, " (do %s)", asctime_pl_krotko_nonl( localtime( &dokiedy ) ) );
	else
	    strcpy( buf, "" );

	if ( *argument )
	    sprintf( kto, "%s %s%s: %s", rch->name, ascdata( localtime( &current_time ) ), buf, argument );
	else
	    sprintf( kto, "%s %s%s", rch->name, ascdata( localtime( &current_time ) ), buf );

	SET_BIT( victim->act, PLR_SILENCE );
	victim->pcdata->silence.do_kiedy = dokiedy;
	free_string( victim->pcdata->silence.od );
	victim->pcdata->silence.od = str_dup( kto );
	send_to_char( "Nie mo`zesz u`zywa`c kana`l`ow!\n\r", victim );
	send_to_char( "Ustawiona flaga UCISZONY.\n\r", ch );
    }

    return;
}


KOMENDA( do_peace )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "pok`oj" ) )
	return;

    /* Yes, we are reusing rch.  -Kahn */
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch->fighting )
	    stop_fighting( rch, TRUE );

	if ( rch->hunting )
	{
	    wywal_zwod( &rch->hunting->zwod_hunting, rch );
	    rch->hunting = NULL;
	}
	if ( rch->hating )
	{
	    wywal_zwod( &rch->hating->zwod_hating, rch );
	    rch->hating = NULL;
	}
	if ( rch->fearing )
	{
	    wywal_zwod( &rch->fearing->zwod_fearing, rch );
	    rch->fearing = NULL;
	}
    }

    send_to_char( "Zaprowadzasz pok`oj.\n\r", ch );

    return;
}


BAN_DATA *              ban_free;
BAN_DATA *              ban_list;

KOMENDA( do_ban )
{
    BAN_DATA  *pban;
    CHAR_DATA *rch;
    char       buf  [ MSL * 10 ];
    char       bufek[ MSL ];
    char       host [ MIL ];
    char       user [ MIL ];
    char       type [ MIL ];
    int        inttype;

    if ( IS_NPC( ch ) )
	return;

    rch = get_char( ch );

    if ( !authorized( rch, "banuj" ) )
	return;

    if ( !*argument )
    {
	strcpy( buf, "{YCzas                | Banodawca   | {Bw/n {Y| {Mkonto{C@adres{x\n\r" );
	strcat( buf, "{Y--------------------+-------------+-----+------------------------------------{x\n\r" );
	for ( pban = ban_list; pban; pban = pban->next )
	{
	    sprintf( bufek, "{Y%s{Y| {B%s{Y| {M%s{C@%s{x\n\r",
		pban->banned,
		pban->type ? "nowi" : "wsz ",
		pban->user,
		pban->host );
	    strcat( buf, bufek );
	}
	send_to_char( buf, ch );
	return;
    }

    argument = one_argument( argument, type );
    argument = one_argument( argument, host );
    smash_tilde( host );
    one_argument( argument, user );
    smash_tilde( user );

    if ( !type[ 0 ] || !host[ 0 ] )
    {
	send_to_char( "Sk`ladnia: banuj <wsz/nowi> <maszyna> [u`zytkownik]\n\r", ch );
	return;
    }

    if ( !str_prefix( type, "wsz" ) )
	inttype = 0;
    else if ( !str_prefix( type, "nowi" ) )
	inttype = 1;
    else
    {
	do_ban( ch, "help" );
	return;
    }

    for ( pban = ban_list; pban; pban = pban->next )
    {
	if ( !str_cmp( host, pban->host )
	  && !*pban->user && pban->type == 0 )
	{
	    /* to jest zupelny ban na caly serwer, niewazne jest wiec, czy ch
	       chce banowac serwer, czy uzytkownika na nim */
	    send_to_char( "Ten system ju`z jest zabanowany.\n\r", ch );
	    return;
	}

	if ( !str_cmp( host, pban->host )
	  && !str_cmp( user, pban->user ) )
	{
	    send_to_char( "Ten serwer/u`zytkownik jest ju`z zabanowany.\n\r", ch );
	    return;
	}
    }

    if ( !ban_free )
    {
	pban            = alloc_perm( sizeof( *pban ) );
    }
    else
    {
	pban            = ban_free;
	ban_free        = ban_free->next;
    }

    pban->host  = str_dup( host );
    pban->user  = str_dup( user );
    pban->type  = inttype;
    sprintf( bufek, "%s%-12s", asctime_progbug( localtime( &current_time ) ), rch->name );
    pban->banned = str_dup( bufek );
    pban->next  = ban_list;
    ban_list    = pban;
    send_to_char( "Zabanowano.\n\r", ch );
    ban_update( );

    return;
}


KOMENDA( do_allow )
{
    BAN_DATA  *prev;
    BAN_DATA  *curr;
    CHAR_DATA *rch;
    char       host[ MIL ];
    char       user[ MIL ];

    rch = get_char( ch );

    if ( !authorized( rch, "dopu`s`c" ) )
	return;

    argument = one_argument( argument, host );
    one_argument( argument, user );

    if ( host[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: dopu`s`c <maszyna> [u`zytkownik]\n\r", ch );
	return;
    }

    prev = NULL;
    for ( curr = ban_list; curr; prev = curr, curr = curr->next )
    {
	if ( !str_cmp( host, curr->host )
	  && !str_cmp( user, curr->user ) )
	{
	    if ( !prev )
		ban_list   = ban_list->next;
	    else
		prev->next = curr->next;

	    free_string( curr->host );
	    free_string( curr->user );
	    free_string( curr->banned );
	    curr->next  = ban_free;
	    ban_free    = curr;
	    send_to_char( "Dopuszczono.\n\r", ch );
	    ban_update( );
	    return;
	}
    }

    send_to_char( "Nie ma takiego bana. Co`s ci si`e pomyli`lo.\n\r", ch );

    return;
}


KOMENDA( do_wizlock )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "bloknie`sm" ) )
	return;

    wizlock = !wizlock;

    if ( wizlock )
	send_to_char( "Blokada nie`smiertelna!\n\r", ch );
    else
	send_to_char( "Usuni`eta nie`smiertelna blokada.\n\r", ch );

    return;
}


KOMENDA( do_slookup )
{
    CHAR_DATA *rch;
    char       buf [ MSL ];
    char       buf1[ MSL * 8 ]; /* Lam: musialem zwiekszyc 25.2.98 i 1.3.2000 */
    char       arg [ MIL ];
    int        sn;
    int        pzl;

    if ( ch )
	rch = get_char( ch );

    if ( ch && !authorized( rch, "uszukaj" ) )
	return;

    one_argument( argument, arg );
    if ( ch && arg[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: uszukaj <(numer)|nazwa|wsz>\n\r", ch );
	return;
    }

    if ( !ch || !str_cmp( arg, "wsz" ) )
    {
	strcpy( buf1, "  Nr                               " );
	for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
	{
	    sprintf( buf, "%s", wyrownaj( class_table[ pzl ].pl_name, 4 ) );
	    strcat( buf1, buf );
	}
	if ( ch )
	    strcat( buf1, "\n\r" );
	else
	    strcat( buf1, "\n" );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( !skill_table[ sn ].pl_name )
		break;

	    sprintf( buf, "%4d Um.: %s ",
		    sn, wyrownaj( skill_table[ sn ].pl_name, -24 ) );
	    strcat( buf1, buf );

	    for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
	    {
		if ( skill_table[ sn ].skill_level[ pzl ] < L_JUN )
		    sprintf( buf, " %3d", skill_table[ sn ].skill_level[ pzl ] );
		else if ( skill_table[ sn ].multi_level[ pzl ] < L_JUN )
		    sprintf( buf, "   +" );
		else
		    sprintf( buf, "   -" );

		strcat( buf1, buf );
	    }

	    if ( ch )
		strcat( buf1, "\n\r" );
	    else
		strcat( buf1, "\n" );
	}

	if ( ch )
	    send_to_char( buf1, ch );
	else
	{
	    FILE *fp;
	    char wyjbuf[ MSL * 8 ];

	    if ( !( fp = fopen( SKILL_FILE, "w" ) ) )
	    {
		lac_perror( SKILL_FILE );
		return;
	    }

	    fprintf( fp, "Podczas ostatniego uruchomienia Lac wczytal nastepujace profesje:\n\n" );
	    fprintf( fp, "Nazwa                    " );
	    for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
		fprintf( fp, "%4s", class_table[ pzl ].who_name );
	    fprintf( fp, "\nUmiejetnosci wlasciwe    " );
	    for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
		fprintf( fp, "%4d", skill_count[ pzl ] );
	    fprintf( fp, "\nUmiejetnosci dodatkowe   " );
	    for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
		fprintf( fp, "%4d", multi_count[ pzl ] );
	    fprintf( fp, "\nPozy                     " );
	    for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
		fprintf( fp, "%4d", pose_count[ pzl ] );

	    fprintf( fp, "\n\nPodczas ostatniego uruchomienia Lac wczytal nastepujacy rozklad umiejetnosci:\n\n" );
	    colourconv( wyjbuf, buf1, NULL, sizeof( wyjbuf ) );

	    if ( fwrite( wyjbuf, strlen( wyjbuf ), 1, fp ) != 1 )
		lac_perror( SKILL_FILE );

	    fclose( fp );
	}
    }
    else
    {
	if ( is_number( arg ) )
	{
	    sn = atoi( arg );
	    if ( sn >= 0
	      && sn < MAX_SKILL
	      && skill_table[ sn ].pl_name )
	    {
		strcpy( buf1, "  Nr                               " );
		for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
		{
		    sprintf( buf, "%s", wyrownaj( class_table[ pzl ].pl_name, 4 ) );
		    strcat( buf1, buf );
		}
		strcat( buf1, "\n\r" );
		sprintf( buf, "%4d Um.: %s ",
			sn, wyrownaj( skill_table[ sn ].pl_name, -24 ) );
		strcat( buf1, buf );

		for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
		{
		    if ( skill_table[ sn ].skill_level[ pzl ] < L_JUN )
			sprintf( buf, " %3d", skill_table[ sn ].skill_level[ pzl ] );
		    else if ( skill_table[ sn ].multi_level[ pzl ] < L_JUN )
			sprintf( buf, "   +" );
		    else
			sprintf( buf, "   -" );

		    strcat( buf1, buf );
		}

		strcat( buf1, "\n\r" );
		send_to_char( buf1, ch );
		return;
	    }
	}

	if ( skill_lookup_ang( arg ) < 0 )
	{
	    send_to_char( "Nie istnieje taka umiej`etno`s`c.\n\r", ch );
	    return;
	}

	strcpy( buf1, "  Nr                               " );
	for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
	{
	    sprintf( buf, "%s", wyrownaj( class_table[ pzl ].pl_name, 4 ) );
	    strcat( buf1, buf );
	}
	strcat( buf1, "\n\r" );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( !skill_table[ sn ].old_name )
		break;

	    if ( str_prefix( arg, skill_table[ sn ].old_name )
	      && str_prefix( arg, skill_table[ sn ].pl_name )
	      && str_prefix( arg, skill_table[ sn ].short_name ) )
		continue;

	    sprintf( buf, "%4d Um.: %s ",
		    sn, wyrownaj( skill_table[ sn ].pl_name, -24 ) );
	    strcat( buf1, buf );

	    for ( pzl = 0; pzl < MAX_CLASS; pzl++ )
	    {
		if ( skill_table[ sn ].skill_level[ pzl ] < L_JUN )
		    sprintf( buf, " %3d", skill_table[ sn ].skill_level[ pzl ] );
		else if ( skill_table[ sn ].multi_level[ pzl ] < L_JUN )
		    sprintf( buf, "   +" );
		else
		    sprintf( buf, "   -" );

		strcat( buf1, buf );
	    }

	    strcat( buf1, "\n\r" );
	}
	send_to_char( buf1, ch );
    }

    return;
}


KOMENDA( do_sset )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    char       arg3[ MAX_INPUT_LENGTH ];
    int        value;
    int        sn;
    bool       fAll;
    bool       fMax;

    rch = get_char( ch );

    if ( !authorized( rch, "uustaw" ) )
	return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: uustaw <posta`c> <nazwa> <warto`s`c>\n\r", ch );
	send_to_char( "albo:     uustaw <posta`c> wsz     <warto`s`c>\n\r", ch );
	send_to_char( "albo:     uustaw <posta`c> maks    <warto`s`c>\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg1 ) ) )
    {
	send_to_char( "Nie widz`e takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie wolno ustawia`c umiej`etno`sci mobom.\n\r", ch );
	return;
    }

    if ( ch->level <= victim->level && ch != victim )
    {
	send_to_char( "Nie wolno dawa`c umiej`etno`sci bogom r`ownym/wy`zszym.\n\r",
		ch );
	return;
    }

    fMax = !str_cmp( arg2, "maks" );
    fAll = fMax || !str_cmp( arg2, "wsz" );
    sn   = 0;

    if ( !fAll && ( sn = skill_lookup_ang( arg2 ) ) < 0 )
    {
	send_to_char( "Nie ma takiej umiej`etno`sci.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Warto`s`c musi by`c liczb`a.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
	send_to_char( "Warto`s`c musi wynosi`c od 0 do 100.\n\r", ch );
	return;
    }

    if ( fAll )
    {
	if ( get_trust( ch ) < L_DIR )
	{
	    send_to_char( "Tylko prawdziwi bogowie mog`a u`zywa`c wsz/maks.\n\r", ch );
	    return;
	}

	for ( sn = 1; sn < MAX_SKILL; sn++ ) /* bylo sn = 0, ale to reserved */
	    if ( skill_table[ sn ].pl_name ) /* czy istnieje? */
		if ( fMax
		  || skill_table[ sn ].skill_level[ victim->klasa ] <= LEVEL_HERO )
		{
		    victim->pcdata->learned[ sn ] = value;
		    victim->pcdata->learned_level[ sn ] = UMIN( skill_table[ sn ].skill_level[ victim->klasa ], victim->level );
		    if ( victim->level >= L_SEN )
			victim->pcdata->learned_level[ sn ] = L_APP;
		}

    }
    else
    {
	if ( sn == 0 && value != 0 )
	{
	    send_to_char( "Nie pr`obuj rozdawa`c ludziom zarezerwowanej umiej`etno`sci!\n\r", ch );
	    return;
	}

	victim->pcdata->learned[ sn ] = value;
	victim->pcdata->learned_level[ sn ] = UMIN( skill_table[ sn ].skill_level[ victim->klasa ], victim->level );
    }

    return;
}


/*
 * Qwert 02.11.2000 mustaw <osoba> d`lugi -czy`s`c
 */
KOMENDA( do_mset )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg1[ MAX_INPUT_LENGTH  ];
    char       arg2[ MAX_INPUT_LENGTH  ];
    char       arg3[ MAX_INPUT_LENGTH  ];
    int        value;
    int        max;

    rch = get_char( ch );

    if ( !authorized( rch, "mustaw" ) )
	return;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: mustaw <posta`c> <pole>    <warto`s`c>\n\r",       ch );
	send_to_char( "albo:     mustaw <posta`c> <`la`ncuch> <warto`s`c>\n\r",       ch );
	send_to_char( "\n\r", ch );
	send_to_char( "Pola:\n\r", ch );
	send_to_char( "  si`l int m`ad zr`e bud profesja p`le`c rasa poziom\n\r", ch );
	send_to_char( "  z`loto bank `zycie mana ruch `cwiczenia moralno`s`c\n\r", ch );
	send_to_char( "  pragnienie nietrze`xwo`s`c g`l`od", ch );
	send_to_char( "\n\r", ch );
	send_to_char( "`La`ncuchy:\n\r", ch );
	send_to_char( "  imi`e kr`otki d`lugi tytu`l specjalno`s`c, (przypadki)\n\r", ch );
	send_to_char( "\n\r", ch );
	send_to_char( "UWAGA: mustaw <osoba> d`lugi -czy`s`c kasuje d`lugi opis.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "Nie widz`e takiej postaci.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */
    if ( !str_prefix( arg2, "si`l" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

/* Wielki blamaz Envy: ch->klasa zamiast victim->klasa.
	if ( class_table[ ch->klasa ].attr_prime == APPLY_STR )
	    max = 25;
	else
	    max = 18; */
	max = 18 + race_table[ victim->race ].str_mod;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Zakres si`ly to od 3 do %d.\n\r", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_str = value;
	return;
    }

    if ( !str_prefix( arg2, "int" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

	max = 18 + race_table[ victim->race ].int_mod;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Zakres inteligencji to od 3 do %d.\n\r", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_int = value;
	return;
    }

    if ( !str_prefix( arg2, "m`ad" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

	max = 18 + race_table[ victim->race ].wis_mod;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Zakres m`adro`sci to od 3 do %d.\n\r", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_wis = value;
	return;
    }

    if ( !str_prefix( arg2, "zr`e" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

	max = 18 + race_table[ victim->race ].dex_mod;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Zakres zr`eczno`sci to od 3 do %d.\n\r", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_dex = value;
	return;
    }

    if ( !str_prefix( arg2, "bud" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

	max = 18 + race_table[ victim->race ].con_mod;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Zakres budowy to od 3 do %d.\n\r", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_con = value;
	return;
    }

    /* nienumerowe wpisywanie podpatrzone ze Zlego, przy okazji pomyslalem
       o mobach */
    if ( !str_prefix( arg2, "profesja" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

	if ( value < -1 || value >= MAX_CLASS )
	{
	    sprintf( buf, "Zakres profesji to od 0 do %d.\n", MAX_CLASS-1 );
	    send_to_char( buf, ch );
	    return;
	}

	if ( value == -1 )
	{
	    for ( value = 0; value < MAX_CLASS; value++ )
		if ( !str_cmp( class_table[ value ].pl_name, arg3 )
		  || !str_cmp( class_table[ value ].ang, arg3 ) )
		    break;
	    if ( value == MAX_CLASS )
	    {
		send_to_char( "Nie istnieje taka profesja.\n\r", ch );
		return;
	    }
	}

	victim->klasa = value;
	return;
    }

    if ( !str_prefix( arg2, "p`le`c" ) )
    {
	if ( IS_AFFECTED( victim, AFF_CHANGE_SEX ) )
	{
	    send_to_char( "Ta osoba jest pod dzia`laniem czaru \"zmiana p`lci\".\n\r", ch );
	    send_to_char( "Spr`obuj p`o`xniej.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Zakres p`lci wynosi od 0 do 2.\n\r", ch );
	    return;
	}

	victim->sex = value;

	return;
    }

    if ( !str_prefix( arg2, "rasa" ) )
    {
	OBJ_DATA *wield;
	OBJ_DATA *wield2;
	int       race;

	/* zeby nie zostawialo broni PP */
	if ( PP( victim ) )
	{
	    STC( "Nie wolno zmienia`c ras graczom poza postaci`a.\n\r", ch );
	    return;
	}

	if ( IS_AFFECTED( victim, AFF_POLYMORPH ) )
	{
	    send_to_char( "Ta osoba jest pod dzia`laniem czaru \"przemiana\".\n\r",
			 ch );
	    send_to_char( "Spr`obuj p`o`xniej albo rozprosz ju`z teraz.\n\r", ch );
	    return;
	}

	if ( is_affected( victim, gsn_postac_nietoperza ) )
	    affect_strip( victim, gsn_postac_nietoperza );

	race = race_lookup( arg3 );

	if ( race < 0 )
	{
	    send_to_char( "Z`la rasa.\n\r", ch );
	    return;
	}

	/* Lam: teraz nawet ponizej 108 poziomu mozna ustawic dowolna rase, ale
	   tylko mobom. Wczesniej nikomu. */
	if ( !IS_SET( race_table[ race ].race_abilities, RACE_PC_AVAIL )
	  && get_trust( ch ) < L_DIR
	  && !IS_NPC( victim ) )
	{
	    STC( "Nie mo`zesz ustawia`c rasy niedost`epnej dla graczy.\n\r", ch );
	    return;
	}

	victim->race = race;
	if ( CZY_WAMPIR( victim ) )
	{
	    /* nie zwiekszam licznika bycia wampirem */
	    /* ale sprawdz_wampira ustawi na 1 z 0  --Lam */
	    victim->alignment = UMIN( victim->alignment, 0 );
	    sprawdz_wampira( ch );
	}

	if ( ( wield = get_eq_char( victim, WEAR_WIELD ) )
	    && !IS_SET( race_table[ victim->race ].race_abilities,
		       RACE_WEAPON_WIELD ) )
	{
	    act( ZM_ZAWSZE, "Upuszczasz $j.", victim, wield, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, "$n upuszcza $j.", victim, wield, NULL, TO_ROOM );
	    obj_from_char( wield );
	    obj_to_room( wield, victim->in_room );
	}

	if ( ( wield2 = get_eq_char( victim, WEAR_WIELD_2 ) )
	    && !IS_SET( race_table[ victim->race ].race_abilities,
		       RACE_WEAPON_WIELD ) )
	{
	    act( ZM_ZAWSZE, "Upuszczasz $j.", victim, wield2, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, "$n upuszcza $j.", victim, wield2, NULL, TO_ROOM );
	    obj_from_char( wield2 );
	    obj_to_room( wield2, victim->in_room );
	}

	check_race_wearlocs( victim ); /* Lam 7.6.99 */
	check_airchar( victim ); /* Lam 28.9.2005 */

	return;
    }

    if ( !str_prefix( arg2, "poziom" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Nie na graczach.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 110 )
	{
	    send_to_char( "Poziom waha si`e od 0 do 110.\n\r", ch );
	    return;
	}
	victim->level = value;
	return;
    }

    if ( !str_prefix( arg2, "z`loto" ) )
    {
	if ( value < 0 || value > 100000000 )
	{
	    send_to_char( "Mi`edzy 0 a 100 000 000 z`lota powinno wystarczy`c.\n\r", ch );
	    return;
	}

	victim->gold = value;
	return;
    }

    if ( !str_prefix( arg2, "bank" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Moby nie maj`a kont bankowych.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100000000 )
	{
	    send_to_char( "Mi`edzy 0 a 100 000 000 z`lota powinno wystarczy`c.\n\r", ch );
	    return;
	}

	victim->pcdata->bank = value;
	return;
    }

    if ( !str_prefix( arg2, "`zycie" ) )
    {
	/* Lam: zabezpieczenie przed ustawianiem ujemnego zycia */
	if ( value < 1 || value > 30000 )
	{
	    send_to_char( "Zakres `zycia wynosi od 1 do 30 000 punkt`ow.\n\r", ch );
	    return;
	}
	victim->max_hit = value;
	return;
    }

    if ( !str_prefix( arg2, "mana" ) )
    {
	if ( value < 1 || value > 30000 )
	{
	    send_to_char( "Zakres many wynosi od 1 do 30 000 punkt`ow.\n\r", ch );
	    return;
	}

	victim->max_mana = value;
	return;
    }

    if ( !str_prefix( arg2, "ruch" ) )
    {
	if ( value < 1 || value > 30000 )
	{
	    send_to_char( "Zakres ruchu wynosi od 1 do 30 000 pkt ruchu.\n\r", ch );
	    return;
	}

	victim->max_move = value;
	return;
    }

    if ( !str_prefix( arg2, "`cwiczenia" ) )
    {
	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Zakres `cwicze`n wynosi od 0 do 100 sesji.\n\r", ch );
	    return;
	}

	victim->practice = value;
	return;
    }

    if ( !str_prefix( arg2, "moralno`s`c" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Zakres moralno`sci wynosi od -1000 do 1000.\n\r", ch );
	    return;
	}

	victim->alignment = value;
	return;
    }

    if ( !str_prefix( arg2, "pragnienie" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

	if ( ( value < 0 || value > 48 ) )
	{
	    send_to_char( "Zakres pragnienia wynosi od 0 do 48.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_THIRST] = value;
	return;
    }

    if ( !str_prefix( arg2, "nietrze`xwo`s`c" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 48 )
	{
	    send_to_char( "Zakres nietrze`xwo`sci wynosi od 0 do 48.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_DRUNK] = value;
	return;
    }

    if ( !str_prefix( arg2, "g`l`od" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

	if ( ( value < 0 || value > 48 ) )
	{
	    send_to_char( "Zakres g`lodu wynosi od 0 do 48.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_FULL] = value;
	return;
    }

    if ( !str_prefix( arg2, "imi`e" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Nie na graczach.\n\r", ch );
	    return;
	}

	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->name );
	victim->name = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "kr`otki" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->short_descr );
	victim->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "dope`lniacz" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->dopelniacz );
	victim->dopelniacz = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "celownik" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->celownik );
	victim->celownik = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "biernik" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->biernik );
	victim->biernik = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "narz`ednik" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->narzednik );
	victim->narzednik = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "miejscownik" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->miejscownik );
	victim->miejscownik = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "wolacz" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->wolacz );
	victim->wolacz = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "d`lugi" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

    /* poniewaz usuwa maskowanie, ustawianie zmienia wlasciwy opis */
	if ( is_affected( victim, gsn_disguise ) )
	{
	    if ( *victim->long_descr_orig )
	    {
		char zapas[ MIL ];

		strcpy( zapas, victim->long_descr );
		free_string( victim->long_descr );
		victim->long_descr = str_dup( victim->long_descr_orig );
		free_string( victim->long_descr_orig );
		victim->long_descr_orig = str_dup( zapas );
	    }
	    affect_strip( victim, gsn_disguise );
	}

	 /* Qwert: teraz mozna kasowac oba opisy (maskowanie) */
	if ( !str_cmp( arg3, "-czy`s`c" )
	  || !str_cmp( arg3, "brak" )
	  || !str_cmp( arg3, "null" ) )
	{
	    free_string( victim->long_descr );
	    victim->long_descr = str_dup( "" );
	    free_string( victim->long_descr_orig );
	    victim->long_descr_orig = str_dup( "" );
	    free_string( victim->false_name );
	    victim->false_name = str_dup( "" );
	    return;
	}

	free_string( victim->long_descr );
	victim->long_descr = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "tytu`l" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie na mobach.\n\r", ch );
	    return;
	}

	set_title( victim, arg3 );
	return;
    }

    if ( !str_prefix( arg2, "specjalno`s`c" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Nie na graczach.\n\r", ch );
	    return;
	}

	if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
	{
	    send_to_char( "Nie istniej`aca specjalno`s`c.\n\r", ch );
	    return;
	}

	return;
    }

    /*
     * Generate usage message.
     */
    do_mset( ch, "" );
    return;
}


/*
 * Alandar 6.10.2004: funkcja stworzona po to, aby uniknac szesciokrotnego
 * powtarzania tego samego kodu. Obsluga ciagow przy ustawianiu wartosci
 * opisowych oraz konwersja nazw czarow i materialow na numery.
 */
void ustaw_wartosc( OBJ_DATA *obj, int ktora, char *arg, CHAR_DATA *ch )
{
    int wartosc;

    if ( typy_przedmiotow[ obj->item_type ].spell[ ktora ] == VAL_SPELL )
    {
	if ( is_number( arg ) )
	    wartosc = atoi( arg );
	else
	    wartosc = skill_lookup_ang( arg );

	if ( wartosc < 0 || wartosc >= MAX_SKILL )
	    send_to_char( "Nie ma takiego czaru.\n\r", ch );
	else /* zeby dalo sie usunac czar ustawiajac 0 */
	    obj->value[ ktora ].v = wartosc == 0 ? -1 : wartosc;
    }
    else if ( typy_przedmiotow[ obj->item_type ].spell[ ktora ] == VAL_MATERIAL )
    {
	if ( is_number( arg ) )
	    wartosc = atoi( arg );
	else
	    wartosc = material_lookup_short( arg );

	if ( wartosc <= 0 || wartosc >= MAX_MATERIAL )
	    send_to_char( "Nie ma takiego materia`lu.\n\r", ch );
	else
	    obj->value[ ktora ].v = wartosc;
    }
    else if ( typy_przedmiotow[ obj->item_type ].spell[ ktora ] == VAL_STRING )
    {
	char *lanc = ( char * ) obj->value[ ktora ].p;
	if ( lanc )
	    free_string( lanc );
	obj->value[ ktora ].p = ( void * ) str_dup( arg );
    }
    else if ( typy_przedmiotow[ obj->item_type ].spell[ ktora ] == VAL_LIQUID )
    {
	if ( is_number( arg ) )
	    wartosc = atoi( arg );
	else
	    wartosc = liquid_lookup( arg );

	if ( wartosc < 0 || wartosc >= LIQ_MAX )
	    send_to_char( "Nie ma takiego p`lynu.\n\r", ch );
	else
	    obj->value[ ktora ].v = wartosc;
    }
    else
	obj->value[ ktora ].v = atoi( arg );

    return;
}


/*
 * Qwert 03.11.2000  pustaw <obj> ekstra +flaga
 * Uwaga: w przypadku zmiany tablicy: extra_bit_name_pl, zmienic tez tutaj
 */
KOMENDA( do_oset )
{
    OBJ_DATA  *obj;
    CHAR_DATA *rch;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    char       arg3[ MAX_INPUT_LENGTH ];
    int        value;
    char      *bufor; /* nie jest niezbedny, ale zwieksza czytelnosc */
    int        all_extra_bits[ ITEM_EXTRA_FLAGS_SIZE ];
    int        pil;

    for ( pil = 0; pil < ITEM_EXTRA_FLAGS_SIZE; pil++ )
	all_extra_bits[ pil ] = -1;

    rch = get_char( ch );

    if ( !authorized( rch, "pustaw" ) )
	return;

    bufor = extra_bit_name_pl( all_extra_bits, rch );

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( !str_cmp( arg1, "ekstra" ) && !str_cmp( arg2, "info" ) )
    {
	STC( "\n\r", ch );
	STC( bufor, ch );
	STC( "\n\r", ch );
	return;
    }

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: pustaw <przedmiot> <pole>    <warto`s`c>\n\r", ch );
	send_to_char( "          pustaw <przedmiot> <`la`ncuch> <warto`s`c>\n\r", ch );
	send_to_char( "lub:      pustaw <przedmiot> ekstra <+|->flaga \n\r", ch );
	send_to_char( "\n\r",                                           ch );
	send_to_char( "Pola:\n\r",                                      ch );
	send_to_char( "  warto`s`c[0-5] (lub w[0-5])\n\r",		ch );
	send_to_char( "  ekstra ubranie poziom ci`e`zar koszt czas\n\r",ch );
	send_to_char( "\n\r",                                           ch );
	send_to_char( "`La`ncuchy:\n\r",                                ch );
	send_to_char( "  nazwa kr`otki d`lugi akcja\n\r",		ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "UWAGA: \"pustaw ekstra info\" podaje nazwy flag\n\r", ch );
	return;
    }

    if ( !( obj = get_obj_world( ch, arg1 ) ) )
    {
	send_to_char( "Nic takiego nie istnieje.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    if ( !str_prefix( arg2, "ekstra" ) && arg3[ 0 ] != '\0' )
    {
	bool       fSet;
	bool       found = FALSE;
	int        count;
	unsigned int bit = 0;
	char       *tmpbuf;
	int        tmp_e_f[ 2 ];

	/* Lam: Uwaga! Mocno zalezne od extra_bit_name_pl( ) */
	if ( arg3[ 0 ] == '+' )
	    fSet = TRUE;
	else if ( arg3[ 0 ] == '-' )
	    fSet = FALSE;
	else
	{
	    STC( "Sk`ladnia: ... ekstra +flaga albo -flaga\n\r", ch );
	    return;
	}

	tmpbuf = bufor;

	for ( count = 0; *tmpbuf != '\0'; count++  )
	{
	    if ( !str_prefix( arg3 + 1, tmpbuf ) )
	    {
		found = TRUE;
		break;
	    }

	    tmpbuf = bufor + count;

	    if ( tmpbuf[ 0 ] == ' ' )
		++bit; /* Lam: nie lepiej one_argument( )? */
	}

	if ( !found )
	{
	    STC( "Nie istnieje taka flaga.\n\r", ch );
	    return;
	}

	CLEAR_BITS( tmp_e_f );
	SET_BIT_V( tmp_e_f, bit );
	tmpbuf = extra_bit_name_pl( tmp_e_f, rch );

	if ( fSet )
	{
	    SET_BIT_V( obj->extra_flags, bit );
	    STC( "Dodana flaga: ", ch );
	    STC( tmpbuf, ch );
	    STC("\n\r", ch );
	    return;
	}

	REMOVE_BIT_V( obj->extra_flags, bit );
	STC( "Usuni`eta flaga: ", ch );
	STC( tmpbuf, ch );
	STC( "\n\r", ch );
	return;
    }

    /*
     * Set something.
     */
    if ( !str_prefix( arg2, "warto`s`c0" ) || !str_prefix( arg2, "w0" ) )
    {
	ustaw_wartosc( obj, 0, arg3, ch );
	return;
    }

    if ( !str_prefix( arg2, "warto`s`c1" ) || !str_prefix( arg2, "w1" ) )
    {
	ustaw_wartosc( obj, 1, arg3, ch );
	return;
    }

    if ( !str_prefix( arg2, "warto`s`c2" ) || !str_prefix( arg2, "w2" ) )
    {
	ustaw_wartosc( obj, 2, arg3, ch );
	return;
    }

    if ( !str_prefix( arg2, "warto`s`c3" ) || !str_prefix( arg2, "w3" ) )
    {
	ustaw_wartosc( obj, 3, arg3, ch );
	return;
    }

    if ( !str_prefix( arg2, "warto`s`c4" ) || !str_prefix( arg2, "w4" ) )
    {
	ustaw_wartosc( obj, 4, arg3, ch );
	return;
    }

    if ( !str_prefix( arg2, "warto`s`c5" ) || !str_prefix( arg2, "w5" ) )
    {
	ustaw_wartosc( obj, 5, arg3, ch );
	return;
    }

    if ( !str_prefix( arg2, "ubranie" ) )
    {
	obj->wear_flags = value;
	return;
    }

    if ( !str_prefix( arg2, "poziom" ) )
    {
	obj->level = value;
	return;
    }

    if ( !str_prefix( arg2, "ci`e`zar" ) )
    {
	if ( obj->carried_by != NULL && !IS_NPC( obj->carried_by ) )
	{
	    send_to_char(
		"Nie mo`zna zmienia`c ci`e`zaru przedmiotu trzymanego przez gracza.\n\r",
			 ch );
	    return;
	}

	obj->weight = value;
	return;
    }

    if ( !str_prefix( arg2, "koszt" ) )
    {
	obj->cost = value;
	return;
    }

    if ( !str_prefix( arg2, "czas" ) )
    {
	obj->timer = value;
	return;
    }

    if ( !str_prefix( arg2, "nazwa" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->name );
	obj->name = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "kr`otki" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->short_descr );
	obj->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "dope`lniacz" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->dopelniacz );
	obj->dopelniacz = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "celownik" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->celownik );
	obj->celownik = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "biernik" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->biernik );
	obj->biernik = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "narz`ednik" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->narzednik );
	obj->narzednik = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "miejscownik" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->miejscownik );
	obj->miejscownik = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "d`lugi" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->description );
	obj->description = str_dup( arg3 );
	return;
    }

    if ( !str_prefix( arg2, "akcja" ) )
    {
	if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->action );
	obj->action = str_dup( arg3 );
	return;
    }

    /*
     * Generate usage message.
     */
    do_oset( ch, "" );

    return;
}


KOMENDA( do_rset )
{
    CHAR_DATA       *rch;
/*    CHAR_DATA       *person; */
    ROOM_INDEX_DATA *location;
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char             arg3[ MAX_INPUT_LENGTH ];
    int              value;

    rch = get_char( ch );

    if ( !authorized( rch, "pomustaw" ) )
	return;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: pomustaw <pomieszczenie> <pole> <wartosc>\n\r", ch );
	send_to_char( "\n\r",                                           ch );
	send_to_char( "Pola:\n\r",                                      ch );
	send_to_char( "  flagi pod`lo`ze pojemno`s`c\n\r",              ch );
	return;
    }

    if ( !( location = find_location( ch, arg1 ) ) )
    {
	send_to_char( "Nie ma takiego miejsca.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Warto`s`c musi by`c liczb`a.\n\r", ch );
	return;
    }

    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_prefix( arg2, "flagi" ) )
    {
	location->room_flags = value;
	return;
    }

    if ( !str_prefix( arg2, "pod`lo`ze" ) )
    {
	location->sector_type = value;
	return;
    }

    /* Qwert 07.03.2001 */
    if ( !str_prefix( arg2, "pojemno`s`c" ) )
    {
	location->pojemnosc = value;
	return;
    }

    /*
     * Generate usage message.
     */
    do_rset( ch, "" );

    return;
}


char *conn_str( int conn )
{
   switch ( conn )
   {
	case CON_PLAYING:		return "  GRA";
	case CON_GET_NAME:		return " IMI`E";
	case CON_GET_OLD_PWD:		return "sHAS`L";
	case CON_CONFIRM_NEW_NAME:	return "pIMI`E";
	case CON_GET_NEW_PWD:		return "nHAS`L";
	case CON_CONFIRM_NEW_PWD:	return "pHAS`L";
	case CON_GET_NEW_RACE:		return " RASA";
	case CON_CONFIRM_NEW_RACE:	return "pRASA";
	case CON_GET_NEW_SEX:		return " P`LE`C";
	case CON_GET_NEW_CLASS:		return " PROF";
	case CON_CONFIRM_CLASS:		return "pPROF";
	case CON_GET_MULTI:		return " UMIE";
	case CON_CONFIRM_MULTI:		return "pUMIE";
	case CON_GET_POLSKIE:		return "  POL";
	case CON_CONFIRM_POLSKIE:	return " cPOL";
	case CON_READ_MOTD:		return " MOTD";
	case CON_JUZ_GRAL_PWD_OK:	return "GRA`L!";
	case CON_GET_OLD_PWD_GRAL:	return "HAS`LO";
	case CON_GET_OLD_PWD_DELETE:	return "KASUJ";
	case CON_GET_OLD_PWD_RESTART:	return "RSTRT";
	case CON_GET_OLD_PWD_PWD:	return "sHAS`L";
	case CON_GET_NEW_PWD_PWD:	return "nHAS`L";
	case CON_CONFIRM_NEW_PWD_PWD:	return "pHAS`L";
	case CON_GET_NEW_NAME:		return "nIMI`E";
	case CON_GET_ODMIANA:		return "  ODM";
	case CON_CONFIRM_ODMIANA:	return " cODM";
	case CON_GET_NEW_STREFA:	return "  STR";
	case CON_CONFIRM_STREFA:	return " cSTR";
	case CON_READ_REGULAMIN:	return "REGUL";
	case CON_UMOWA:			return "UMOWA";
	case CON_GET_WOLACZ:		return "  WO`L";
	case CON_CONFIRM_WOLACZ:	return " pWO`L";
	case CON_GET_EMAIL:		return "  ADR";
	case CON_CONFIRM_EMAIL:		return " pADR";
	default:			return "B`L`AD!";
   }
}


const char *krotko_bezczynnosc( int idle )
{
    static char wyj[ 16 ];
    static char * const lit[ 5 ] = { "s", "m", "g", "d", "m" };
    static int dzielniki[ 5 ]    = {  60,  60,  24,  30,   0 };
    int ile[ 5 ];
    int i;

    if ( idle < 0 )
	return "-?";

    if ( !idle )
	return "0";

    if ( idle < 4 )
    {
	sprintf( wyj, "%d/4s", idle );
	return wyj;
    }

    idle /= 4;

    if ( idle < 60 )
    {
	sprintf( wyj, "%ds", idle );
	return wyj;
    }

    for ( i = 0; dzielniki[ i ]; i++ )
    {
	ile[ i ] = idle % dzielniki[ i ];
	idle /= dzielniki[ i ];

	if ( !idle )
	    break;
    }

    if ( idle )
	ile[ i ] = idle;

    sprintf( wyj, "%d%s%d%s", ile[ i ], lit[ i ], ile[ i - 1 ], lit[ i - 1 ] );

    return wyj;
}


KOMENDA( do_users )
{
    CHAR_DATA       *rch;
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH * 12 ];
    char             buf2[ MAX_STRING_LENGTH ];
    char             bufrlacd[ 400 ]; /* wlasc. maksymalnie moze miec 390 */
    int              count, licz;
    bool             stop;

    rch = get_char( ch );

    if ( !authorized( rch, "po`l`aczenia" ) )
	return;

    count = 0;
    buf[ 0 ] = '\0';
    stop = FALSE;
    strcpy( buf, "{B[{mPlk  Po`l`aczenie  {YPo  Za{C  Bezcz{B] {gImi`e{x\n\r" );
    for ( d = descriptor_list; d; d = d->next )
    {
	/* dyrektorzy widza wszystkich */
	if ( ( d->character || ch->level >= L_DIR )
	  && ( !d->character || can_see( ch, d->character ) ) )
	{
	    count++;

	    if ( stop )
		continue;

	    if ( *d->lsc_host )
		sprintf( bufrlacd, "  [%s@%s]", d->lsc_user, d->lsc_host );
	    else
		bufrlacd[ 0 ] = '\0';

#if !defined( NO_PTHREADS )
	    sprintf( buf2, "{B[{m%3d %2d - {R%s  {Y%3d %3d{C%7s{B] {g%s{x %3dx%-3d %s\n\r\t%s@%s%s\n\r",
		    d->descriptor,
		    d->connected,
#if !defined( MSDOS )
		    ( d->welcome_state == WELCOME_DONE )
		      ? conn_str( d->connected ) : " {GWELC",
#else
		    conn_str( d->connected ),
#endif
		    d->original ? d->original->level :
		     d->character ? d->character->level : 0,
		    d->original ? d->original->trust :
		     d->character ? d->character->trust : 0,
		    krotko_bezczynnosc( d->idle ),
		    d->original  ? d->original->name  :
		    d->character ? wyrownaj( d->character->name, -12 ) : "(brak)      ",
		    d->szer,
		    d->wys,
		    d->term_type,
		    d->user,
		    d->host,
		    bufrlacd );
#else /* NO_PTHREADS */
	    sprintf( buf2, "{B[{m%3d %2d - {R%s  {Y%3d %3d{C%7s{B] {g%s{x %s%s\n\r",
		    d->descriptor,
		    d->connected,
#if !defined( MSDOS )
		    ( d->welcome_state == WELCOME_DONE )
		      ? conn_str( d->connected ) : " {GWELC",
#else
		    conn_str( d->connected ),
#endif
		    d->original  ? d->original->level :
		     d->character ? d->character->level : 0,
		    d->original  ? d->original->trust :
		     d->character ? d->character->trust : 0,
		    krotko_bezczynnosc( d->idle ),
		    d->original  ? d->original->name  :
		    d->character ? wyrownaj( d->character->name, -12 ) : "(brak)      ",
		    /* d->user, */ d->host, bufrlacd );
#endif /* NO_PTHREADS */
	    strcat( buf, buf2 );
	    if ( strlen( buf ) > ( sizeof( buf ) - MSL ) )
	    {
		strcat( buf, "(wi`ecej nie zdo`lam wy`swietli`c, ale licz`e wszystkich)\n\r" );
		stop = TRUE;
	    }
	}
    }

    licz = liczba( count );
    sprintf( buf2, "{Y%d{x u`zytkowni%s\n\r", count, licz == 1 ? "k" :
	licz == 2 ? "cy" : "k`ow" );
    strcat( buf, buf2 );

    count = 0;
    stop = FALSE;
    for ( rch = char_list; rch; rch = rch->next )
	if ( !IS_NPC( rch ) && !rch->desc && can_see( ch, rch ) )
	{
	    /* Lam 11.11.2003: brakowalo can_see! */
	    if ( !stop )
	    {
		if ( !count )
		    sprintf( buf2, "Postacie graczy bez po`l`acze`n: {g" );
		else
		    strcpy( buf2, "{x, {g" );

		strcat( buf, buf2 );
		strcat( buf, rch->name );

		/* przekroczyc moze te liczbe o najwyzej 17, 25 potrzebne na
		   wyswietlenie ilosci ponizej */
		if ( strlen( buf ) > ( sizeof( buf ) - 60 ) )
		{
		    strcat( buf, " {R(i inne)" );
		    stop = TRUE;
		}
	    }
	    count++;
	}

    if ( count )
    {
	licz = liczba( count );
	sprintf( buf2, "{x ({Y%d{x posta%s).\n\r", count, licz == 1 ? "`c" :
	    licz == 2 ? "cie" : "ci" );
	strcat( buf, buf2 );
    }

    send_to_char( buf, ch );

    return;
}


/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
KOMENDA( do_force )
{
    CHAR_DATA *rch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        trust;
    int        cmd;

    rch = get_char( ch );

    if ( !authorized( rch, "zmu`s" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
    {
	send_to_char( "Zmusi`c kogo do czego?\n\r", ch );
	return;
    }

    /*
     * Look for command in command table.
     */
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	if ( argument[ 0 ] == cmd_table[ cmd ].name[ 0 ]
	    && !str_prefix( argument, cmd_table[ cmd ].name )
	    && ( cmd_table[ cmd ].level > trust ) )
	{
	    ASTC( "Przecie`z sam$a nie potrafisz tego zrobi`c!", ch );
	    return;
	}

    if ( !str_cmp( arg, "wsz" ) )
    {
	CHAR_DATA *vch;

	for ( vch = char_list; vch; vch = vch->next )
	{
	    if ( vch->deleted )
		continue;

	    if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch ) )
	    {
		act( ZM_ZAWSZE, "$n zmusza ci`e do zrobienia \"$t\".", ch, argument, vch, TO_VICT );
		interpret( vch, argument, INTERP_FORCE );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( !( victim = get_char_world_pc( ch, arg ) ) )
	{
	    send_to_char( "Gdzie ty go widzisz?\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Ju`z si`e robi!\n\r", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    ASTC( "Sam$a to zr`ob!", ch );
	    return;
	}

	act( ZM_ZAWSZE, "$n zmusza ci`e do zrobienia \"$t\".", ch, argument, victim, TO_VICT );
	interpret( victim, argument, INTERP_FORCE );
    }

    send_to_char( "Zmuszono.\n\r", ch );

    return;
}


/*
 * New routines by Dionysos.
 */
/*
 * Lam 22.3.99: poziom boskiej niewidzialnosci
 */
KOMENDA( do_invis )
{
    if ( IS_NPC( ch ) )
	return;

    if ( !authorized( ch, "bniewidka" ) )
	return;

    if ( ch->pcdata->wizinvis < 1 || ch->pcdata->wizinvis > get_trust( ch ) )
	ch->pcdata->wizinvis = get_trust( ch );

    if ( !*argument )
    {
	if ( IS_SET( ch->act, PLR_WIZINVIS ) )
	{
	    REMOVE_BIT( ch->act, PLR_WIZINVIS );
	    send_to_char( "Przywracasz sobie materialno`s`c.\n\r", ch );

	    if ( WP( ch ) )
		act( ZM_WZROK | ZM_WID_CHAR, "$n powoli staje si`e widzialn$0.", ch, NULL, NULL, TO_ROOM );

	    /* Ulryk 2.10.2005: sprawdzanie rekordu graczy */
	    czy_rekord_graczy( );
	    czy_rekord_polaczen( );
	}
	else
	{
	    ch_printf( ch, "Powoli rozp`lywasz si`e w powietrzu (boska niewidzialno`s`c, poziom %d).\n\r",
				ch->pcdata->wizinvis );

	    if ( WP( ch ) )
		act( ZM_WZROK | ZM_WID_CHAR, "$n powoli staje si`e niewidzialn$0.",  ch, NULL, NULL, TO_ROOM );

	    SET_BIT( ch->act, PLR_WIZINVIS );
	}
    }
    else
    {
	int poziom = atoi( argument );

	if ( poziom < 1 || poziom > get_trust( ch ) )
	    poziom = get_trust( ch );

	ch->pcdata->wizinvis = poziom;
	ch_printf( ch,
		"Zmieniasz poziom swej boskiej niewidzialno`sci na %d.\n\r",
		ch->pcdata->wizinvis );
    }

    return;
}


/*
 * Vigud 14.9.2010: "boska kukla", opcja pozwalajaca graczom widziec postac
 * z boska niewidka, ale tylko na gawedzie, "kto" i w telepatii
 */
KOMENDA( do_bkukla )
{
    if ( IS_NPC( ch ) )
	return;

    if ( !authorized( ch, "bkuk`la" ) )
	return;

    if ( IS_SET( ch->act, PLR_BOSKA_KUKLA ) )
    {
	REMOVE_BIT( ch->act, PLR_BOSKA_KUKLA );
	STC( "Boska kuk`la wy`l`aczona.\n\r", ch );
    }
    else
    {
	SET_BIT( ch->act, PLR_BOSKA_KUKLA );
	STC( "Boska kuk`la w`l`aczona.\n\r", ch );
    }

    return;
}


KOMENDA( do_holylight )
{
    if ( IS_NPC( ch ) )
	return;

    if ( !authorized( ch, "nadwzrok" ) )
	return;

    if ( IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
	REMOVE_BIT( ch->act, PLR_HOLYLIGHT );
	send_to_char( "Wy`l`aczasz sw`oj nadwzrok.\n\r", ch );
    }
    else
    {
	SET_BIT( ch->act, PLR_HOLYLIGHT );
	send_to_char( "W`l`aczasz sw`oj nadwzrok.\n\r", ch );
	REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_BLIND );
    }

    return;
}


char *opisz_przedmiot( CHAR_DATA *ch, OBJ_DATA *obj, int obj_counter )
{
    OBJ_DATA   *in_obj;
    static char buf[ MAX_STRING_LENGTH ];

    buf[ 0 ] = '\0';

    for ( in_obj = obj; in_obj->in_obj;
	 in_obj = in_obj->in_obj );

    if ( in_obj->carried_by && can_see( ch, in_obj->carried_by ) )
    {
	sprintf( buf, "%3d. %5d %s [%d] w inwentarzu %s w #%d.\n\r",
		obj_counter, obj->pIndexData->vnum,
		obj->short_descr, obj->level,
		PERSD( in_obj->carried_by, ch, FALSE ),
		in_obj->carried_by->in_room->vnum );
    }
    else if ( in_obj->owned_by && can_see( ch, in_obj->owned_by ) )
    {
	sprintf( buf, "%3d. %5d %s [%d] w przechowalni %s.\n\r",
		obj_counter, obj->pIndexData->vnum,
		obj->short_descr, obj->level,
		PERSD( in_obj->owned_by, ch, FALSE ) );
    }
    else
    {
	sprintf( buf, "%3d. %5d %s [%d] %s (#%4d).\n\r",
		obj_counter, obj->pIndexData->vnum,
		obj->short_descr, obj->level,
		( !in_obj->in_room ) ?
		"(gdzie`s)" : in_obj->in_room->miejscownik,
		( !in_obj->in_room ) ?
		0 : in_obj->in_room->vnum );
    }

    buf[ 0 ] = UPPER( buf[ 0 ] );

    return buf;
}


/*
 * Idea from Talen of Vego's do_where command
 *
 * Qwert 02.11.2000 nowa skladnia pgdzie
 * Uwaga: bez wlaczonego nadwzroku przedmiot jest znajdowany ale nie pokazany.
 * Nie dodawalem komunikatu.
 */
KOMENDA( do_owhere )
{
    OBJ_DATA  *obj;
    char       buf1[ MAX_STRING_LENGTH * 10 ];
    char       buf [ MAX_STRING_LENGTH      ];
    char       arg [ MAX_INPUT_LENGTH       ];
    int        obj_counter = 1;
    bool       found = FALSE;
    bool       szukamvnum = FALSE;
    int        numerek = 0;
    int        limit = 100; /* przy zmianie limitu zwiekszyc rozmiar buf1 */

    if ( !authorized( get_char( ch ), "pgdzie" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia:  pgdzie <przedmiot>.\n\r", ch );
	send_to_char( "           pgdzie <vnum przedmiotu>.\n\r", ch );
	return;
    }
    else
    {
	if ( is_number( arg ) )
	{
	    numerek = atoi( arg);
	    szukamvnum = TRUE;
	}

	buf1[ 0 ] = '\0';

	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
	    if ( obj_counter > limit || szukamvnum )
		break;

	    if ( obj->deleted
	      || !can_see_obj( ch, obj )
	      || !is_name( arg, obj->name ) )
	    {
		continue;
	    }

	    strcpy( buf, opisz_przedmiot( ch, obj, obj_counter ) );

	    if ( *buf )
	    {
		found = TRUE;
		strcat( buf1, buf );
		obj_counter++;
	    }
	}

	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
	    if ( obj_counter > limit || szukamvnum )
		break;

	    if ( obj->deleted
	      || !can_see_obj( ch, obj )
	      || obj->wear_loc != WEAR_NONE
	      || !is_name( arg, obj->name ) )
	    {
		continue;
	    }

	    strcpy( buf, opisz_przedmiot( ch, obj, obj_counter ) );

	    if ( *buf )
	    {
		found = TRUE;
		strcat( buf1, buf );
		obj_counter++;
	    }
	}

	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
	    if ( obj_counter > limit || szukamvnum )
		break;

	    if ( obj->deleted
	      || !can_see_obj( ch, obj )
	      || obj->wear_loc == WEAR_NONE
	      || !is_name( arg, obj->name ) )
	    {
		continue;
	    }

	    strcpy( buf, opisz_przedmiot( ch, obj, obj_counter ) );

	    if ( *buf )
	    {
		found = TRUE;
		strcat( buf1, buf );
		obj_counter++;
	    }
	}

	for ( obj = object_list; obj; obj = obj->next )
	{
	    if ( obj_counter > limit )
		break;

	    if ( obj->deleted )
		continue;

	    if ( !szukamvnum )
	    {
		if ( !can_see_obj( ch, obj )
		  || !is_name( arg, obj->name )
		  || obj->in_room == ch->in_room
		  || obj->carried_by == ch )
		{
		    continue;
		}
	    }
	    else
	    {
		if ( !can_see_obj( ch, obj ) || obj->pIndexData->vnum != numerek )
		    continue;
	    }

	    strcpy( buf, opisz_przedmiot( ch, obj, obj_counter ) );
	    if ( *buf )
	    {
		found = TRUE;
		strcat( buf1, buf );
		obj_counter++;
	    }
	}

	if ( obj_counter > limit )
	{
	    sprintf( buf, "Oto pierwsze %d wyst`apie`n. Zaw`e`x kryteria poszukiwa`n!\n\r", limit );
	    strcat( buf1, buf );
	}

	send_to_char( buf1, ch );
    }

    if ( !found )
	send_to_char( "Nie ma niczego takiego.\n\r", ch );

    return;
}


KOMENDA( do_numlock ) /* Globi */
{
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg1[ MAX_INPUT_LENGTH  ];
    int        temp;

    rch = get_char( ch );

    if ( !authorized( rch, "blokpoziom" ) )
	return;

    one_argument( argument, arg1 );

    temp = atoi( arg1 );

    if ( arg1[ 0 ] == '\0' ) /* Prints out the current value */
    {
	sprintf( buf, "Aktualny minimalny poziom:  %d.\n\r", numlock );
	send_to_char( buf, ch );
	return;
    }

    if ( ( temp < 0 ) || ( temp > LEVEL_IMMORTAL ) )
    {
	sprintf( buf, "Poziom musi mie`sci`c si`e mi`edzy 0 a %d.\n\r", LEVEL_IMMORTAL );
	send_to_char( buf, ch );
	return;
    }

    numlock = temp;  /* Only set numlock if arg supplied and within range */

    if ( numlock != 0 )
    {
	sprintf( buf, "Gra zablokowana dla poziom`ow ni`zszych od %d.\n\r", numlock );
	send_to_char( buf, ch );
    }
    else
	send_to_char( "Gra otwarta dla wszystkich.\n\r", ch );

    return;

}


KOMENDA( do_newlock )
{
    CHAR_DATA *rch;
    char       buf[ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "bloknowi" ) )
	return;

    if ( numlock != 0 && get_trust( ch ) < L_SEN )
    {
	send_to_char( "Nie wolno ci teraz zmienia`c tego ustawienia.\n\r",
		     ch );
	sprintf( buf, "Gra zamkni`eta dla poziom`ow ni`zszych od %d.\n\r", numlock );
	send_to_char( buf, ch );
	return;
    }

    if ( numlock != 0 )
    {
	sprintf( buf, "Gra zamkni`eta dla poziom`ow ni`zszych od %d.\n\r", numlock );
	send_to_char( buf, ch );
	send_to_char( "Zmieniasz ustawienie na: ", ch );
    }

    if ( numlock == 1 )
    {
	numlock = 0;
	send_to_char( "Gra dost`epna dla wszystkich.\n\r", ch );
    }
    else
    {
	numlock = 1;
	send_to_char( "Gra niedost`epna nowym graczom.\n\r", ch );
    }

    return;
}


KOMENDA( do_sstime )
{
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg1[ MAX_INPUT_LENGTH  ];
    int        number;

    rch = get_char( ch );

    if ( !authorized( rch, "jebustaw" ) )
	return;

    smash_tilde( argument );
    one_argument( argument, arg1 );

    if ( arg1[ 0 ] != '\0' && !str_prefix( arg1, "jebut" ) )
    {
	Reboot = !Reboot;
	sprintf( buf, "Metoda: %s.\n\r", Reboot ? "Jebut" : "Wy`l`aczenie" );
	send_to_char( buf, ch );
	return;
    }

    number   = atoi( arg1 );

    if ( arg1[ 0 ] == '\0' || !is_number( arg1 ) || number < 0 )
    {
	send_to_char( "Sk`ladnia: jebustaw <warto`s`c>|jebut\n\r", ch );
	send_to_char( "\n\r",                                             ch );
	send_to_char( "Warto`s`c to ilo`s`c minut do jebuta/zako`nczenia pracy.", ch );
	send_to_char( " 0 - wy`l`aczenie\n\r",                            ch );
	send_to_char( "'Jebut' prze`l`acza tryb ko`nca pracy.\n\r",       ch );
	send_to_char( "\n\r",                                             ch );

	if ( down_time > 0 )
	{
	    sprintf( buf, "Ostrze`zenie 1:  %d mt (%d s).\n\r",
		    UMAX( ( (int) warning1 - (int) current_time + 1 ) / 60, 0 ),
		    UMAX( ( (int) warning1 - (int) current_time ), 0 ) );
	    send_to_char( buf,                                            ch );
	    sprintf( buf, "Ostrze`zenie 2:  %d mt (%d s).\n\r",
		    UMAX( ( (int) warning2 - (int) current_time + 1 ) / 60, 0 ),
		    UMAX( ( (int) warning2 - (int) current_time ), 0 ) );
	    send_to_char( buf,                                            ch );
	    sprintf( buf, "%s%d m (%d s).\n\r",
		    Reboot ? "Jebut:          " : "Wy`l`aczenie:     ",
		    UMAX( ( (int) down_time - (int) current_time ) / 60, 0 ),
		    UMAX( ( (int) down_time - (int) current_time ), 0 ) );
	    send_to_char( buf,                                            ch );
	}
	else
	    send_to_char( "Automatyczny jebut wy`l`aczony.\n\r",         ch );

	return;
    }

    /* Set something */

    if ( number > 0 )
    {
	down_time = current_time + ( number * 60 );
	if ( number == 1 )
	{
	    warning2  = 0;
	    /* sekunda wiecej, zeby minuty zdazyly sie dobrze policzyc */
	    warning1  = down_time - 61;
	}
	else if ( number < 3 )
	{
	    warning2  = down_time - 61;
	    warning1  = warning2  - 60;
	}
	else if ( number < 6 )
	{
	    warning2  = down_time - 61;
	    warning1  = warning2  - 120;
	}
	else
	{
	    warning2  = down_time - 121;
	    warning1  = warning2  - 180;
	}

	sprintf( buf, "%s nast`api za %d m (%d s).\n\r",
		Reboot ? "Jebut" : "Wy`l`aczenie",
		( (int) down_time - (int) current_time ) / 60,
		( (int) down_time - (int) current_time ) );
	send_to_char( buf, ch );
	sprintf( buf, "1. ostrze`zenie za %d m (%d s).\n\r",
		( (int) warning1 - (int) current_time + 1 ) / 60,
		( (int) warning1 - (int) current_time ) );
	send_to_char( buf, ch );

	if ( warning2 )
	{
	    sprintf( buf, "2. ostrze`zenie za %d m (%d s).\n\r",
		( (int) warning2 - (int) current_time + 1 ) / 60,
		( (int) warning2 - (int) current_time ) );
	    send_to_char( buf, ch );
	}

    }
    else
    {
	down_time = 0;
	sprintf( buf, "Auto%s wy`l`aczony.\n\r",
		Reboot ? "jebut" : "stop" );
	send_to_char( buf, ch );
    }

    return;
}


/*
 * Pierwsza wersja: Lam 27.6.1999
 * Nowa wersja, napisana od zera: Lam 21.2.2008
 */
void konwersja_bumi( CHAR_DATA *ch )
{
    char nowe[ MSL * 2 ];
    char jedno[ MIL ];
    char *reszta;
    int pzl;

    if ( IS_NPC( ch ) )
    {
	bug( "Konwersja_bumi(): NPC", 0 );
	return;
    }

    reszta = ch->pcdata->immskll;
    if ( !*reszta )
	return;

    nowe[ 0 ] = '\0';

    while ( TRUE )
    {
	reszta = one_argument( reszta, jedno );

	if ( !*jedno )
	    break;

	/* Wykrywanie duplikatow, czesc pierwsza (dwa razy identyczna wersja,
	   niewazne, czy przed, czy po konwersji). */
	if ( is_name2( jedno, reszta ) )
	    continue;

	for ( pzl = 0; *cmd_table[ pzl ].name; pzl++ )
	    if ( !str_cmp( cmd_table[ pzl ].name, jedno ) )
		break;

	if ( !*cmd_table[ pzl ].name )
	{
	    for ( pzl = 0; *cmd_table[ pzl ].name; pzl++ )
		if ( !str_cmp( cmd_table[ pzl ].old_name, jedno ) )
		    break;

	    if ( *cmd_table[ pzl ].name )
	    {
		/* Wykrywanie duplikatow, czesc druga (skonwertowane na nowa
		   wersje, ale mogla juz taka byc). */
		if ( !is_name2( cmd_table[ pzl ].name, nowe )
		  && !is_name2( cmd_table[ pzl ].name, reszta ) )
		{
		    strcat( nowe, cmd_table[ pzl ].name );
		    strcat( nowe, " " );
		}
	    }
	    else
	    {
		sprintf( bug_buf, "Bumi '%s' w pliku postaci %s, a nie w cmd_table. Usuwam.", jedno, ch->name );
		cbug( bug_buf, 0 );
	    }
	}
	else if ( cmd_table[ pzl ].level >= LEVEL_HERO )
	{
	    strcat( nowe, jedno );
	    strcat( nowe, " " );
	}
	else
	{
	    sprintf( bug_buf, "Bumi '%s' w pliku postaci %s, ale to nie bumi. Usuwam.", jedno, ch->name );
	    cbug( bug_buf, 0 );
	}
    }

    free_string( ch->pcdata->immskll );
    ch->pcdata->immskll = str_dup( nowe );
/*    STC( "\n\r\n\r{RTwoje {Gboskie umiej`etno`sci{R zosta`ly {Gspolszczone{R. Napisz \"{Ybumi{R\", aby je zobaczy`c.{x\n\r\n\r", ch ); */
    return;
}


/*
 * Modifications contributed by
 * Canth (phule@xs4all.nl)
 * Maniac (v942346@si.hhs.nl)
 * Vego (v942429@si.hhs.nl)
 * Lam <lam@lac.pl>
 */
KOMENDA( do_imtlset )
{

    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1[ MAX_INPUT_LENGTH  ];
    char       buf [ MAX_STRING_LENGTH*2 ];
    char       buf1[ MAX_STRING_LENGTH*2 ];
    bool       fAll = FALSE;
    int        cmd;
    int        col  = 0;

    rch = get_char( ch );

    if ( !authorized( rch, "bumustaw" ) )
	return;

    argument = one_argument( argument, arg1 );

    if ( arg1[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: bumustaw <posta`c> +|- <boska umiej`etno`s`c>\n\r", ch );
	send_to_char( "albo:     bumustaw <posta`c> +|- wsz\n\r",             ch );
	send_to_char( "albo:     bumustaw <posta`c>\n\r",                     ch );
	return;
    }

    if ( !( victim = get_char_world( rch, arg1 ) ) )
    {
	send_to_char( "Nie widz`e takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie na mobach.\n\r", ch );
	return;
    }

    if ( get_trust( rch ) <= get_trust( victim ) && rch != victim )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( argument[ 0 ] == '+' || argument[ 0 ] == '-' )
    {
	buf[ 0 ] = '\0';
	smash_tilde( argument );

	argument = one_argument( argument, arg1 );

	if ( !str_cmp( argument, "wsz" ) )
	    fAll = TRUE;

	if ( arg1[ 0 ] == '+' )
	{
	    if ( !fAll )
	    {
		if ( victim->pcdata->immskll )
		    sprintf( buf, "%s", victim->pcdata->immskll );

		if ( is_name2( argument, victim->pcdata->immskll ) )
		{
		    send_to_char( "Ta umiej`etno`s`c by`la ju`z ustawiona.\n\r", ch );
		    return;
		}

	    }

	    for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	    {
		if ( cmd_table[ cmd ].level > get_trust( rch ) )
		    continue;

		if ( fAll )
		{
		    if ( cmd_table[ cmd ].level <= get_trust( victim )
		      && cmd_table[ cmd ].level >= LEVEL_HERO
		      && !is_name2( cmd_table[ cmd ].name,
				     "jebu wy`l`ac zar`zni restar restart wr`o`c" ) )
		    {
			strcat( buf, cmd_table[ cmd ].name );
			strcat( buf, " " );
		    }
		}
		else /* Add only one skill */
		{
		    if ( !str_cmp( argument, cmd_table[ cmd ].name ) )
			break;
		}
	    }

	    if ( !fAll )
	    {
		if ( cmd_table[ cmd ].name[ 0 ] == '\0'
		  || is_name2( argument, "jebu wy`l`ac zar`zni restar restart wr`o`c" ) )
		{
		    send_to_char( "To nie jest boska umiej`etno`s`c.\n\r", ch );
		    return;
		}

		strcat( buf, argument );
		strcat( buf, " " );
	    }
	}
	else /* arg1[ 0 ] == '-' */
	{
	    if ( fAll )
	    {
		free_string( victim->pcdata->immskll );
		victim->pcdata->immskll = str_dup( "" );
		send_to_char( "Usuni`eto wszystkie boskie umiej`etno`sci.\n\r", ch );
		return;
	    }
	    else /* Remove one skill */
	    {
		/* Tu pojawialy sie pady, wiec wprowadzilem buf8, na ktory
		   wskazuje teraz buf2. Nie wiem, czy to pomoze, ale sprobowac
		   warto. Przyczyna bledow wedlug mnie bylo przepelnienie
		   bufora, ktory system dawal buf2 na stosie. Teraz bufor ten to
		   buf8 i ma dlugosc 8192 (4096 * 2). */
		char *buf2;
		char  buf8[ MAX_STRING_LENGTH * 2 ]; /* Lam */
		char  arg3[ MAX_INPUT_LENGTH ];
		char  arg2[ MAX_INPUT_LENGTH ];

		one_argument( argument, arg2 );

		sprintf( buf8, "%s", victim->pcdata->immskll );
		buf2 = &buf8[ 0 ];

		if ( !is_name2( arg2, victim->pcdata->immskll ) )
		{
		    send_to_char( "Nie by`lo takiej umiej`etno`sci.\n\r", ch );
		    return;
		}

		while ( buf2[ 0 ] != '\0' )
		{
		    buf2 = one_argument( buf2, arg3 );
		    if ( str_cmp( arg3, arg2 ) )
		    {
			strcat( buf, arg3 );
			strcat( buf, " " );
		    }
		}
	    }
	}

	free_string( victim->pcdata->immskll );
	victim->pcdata->immskll = str_dup( buf );
    }

    sprintf( buf, "Boskie umiej`etno`sci postaci \"%s\":\n\r", victim->name );
    send_to_char( buf, ch );
    buf1[ 0 ] = '\0';

    for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
    {
	if ( cmd_table[ cmd ].level < LEVEL_HERO
	  || !is_name( cmd_table[ cmd ].name, victim->pcdata->immskll ) )
	{
	    continue;
	}

	sprintf( buf, "%s", wyrownaj( cmd_table[ cmd ].name, -11 ) );
	strcat( buf1, buf );

	if ( ++col % 7 == 0 )
	    strcat( buf1, "\n\r" );
    }

    if ( col % 7 != 0 )
	strcat( buf1, "\n\r" );
    send_to_char( buf1, ch );

    return;
}


/* Ponizsza funkcja jest dzielem Erwina A. sciagnietym przez Lama */
KOMENDA( do_rename )
{
    char old_name[ MAX_INPUT_LENGTH ],
	 new_name[ MAX_INPUT_LENGTH ],
	 strsave [ MAX_INPUT_LENGTH ];

    CHAR_DATA* victim;

    if ( !authorized( get_char( ch ), "przemianuj" ) )
	return;

    argument = one_argument( argument, old_name ); /* find new/old name */
    one_argument( argument, new_name );

    /* Trivial checks */
    if ( !old_name[ 0 ] )
    {
	send_to_char( "Kogo przemianowa`c?\n\r", ch );
	return;
    }

    victim = get_char_world_pc( ch, old_name );
    /* Uwaga! Dopuszczalne skroty! */

    if ( !victim )
    {
	send_to_char( "Posta`c musi by`c obecna w mudzie.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nazwy mob`ow zmienia si`e w krainach!\n\r", ch );
	return;
    }

    /* allow rename self new_name,but otherwise only lower level */
    if ( ( victim != ch ) && ( get_trust( victim ) >= get_trust( ch ) ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    if ( !victim->desc || ( victim->desc->connected != CON_PLAYING ) )
    {
	send_to_char( "Ten gracz straci`l po`l`aczenie.\n\r", ch );
	return;
    }

    if ( !new_name[ 0 ] )
    {
	send_to_char(" Jakie nada`c nowe imi`e?\n\r", ch );
	return;
    }

    /* Lam: ponizej w oryginale bylo !check_parse_name, u mnie odwrotnie - 0 to
       imie, ktore sie nadaje */
    if ( check_parse_name( new_name, TRUE ) )
    {
	send_to_char( "Nowe imi`e nie nadaje si`e dla gracza.\n\r", ch );
	return;
    }

    if ( czy_zajete_imie( new_name ) )
    {
	send_to_char( "Taka posta`c ju`z istnieje.\n\r", ch );
	return;
    }

#if !defined( macintosh ) && !defined( MSDOS )
    sprintf( strsave, PLAYER_DIR "%s" DIR_SEPARATOR "%s",
	initial( victim->name ), capitalize( victim->name ) );
#else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name ) );
#endif

    zmien_wlasciciela_wszedzie( victim, new_name );
    zmien_imie_postaci( victim, new_name );

    save_char_obj( victim );

    /* unlink the old file */
    unlink( strsave ); /* unlink does return a value.. but we do not care */

    ASTC( "Pomy`slnie dokona`l$o`s ponownego aktu baptystycznego.", ch );

    victim->position = POS_STANDING; /* nie powinno mu przeszkadzac */
    act_pp( ZM_ZAWSZE, "$n zmienia twoje imi`e na \"$N\"!", ch, NULL, victim, TO_VICT );

    return;
} /* do_rename */


/*
 * Lam 1.4.98: lista boskich polecen, dostepna wszystkim od poziomu 101
 */
KOMENDA( do_immcmd )
{
    CHAR_DATA *rch;
    int cmd, lv, col = 0;
    int value1, value2;
    bool pik;
    bool ma;
    char pim[ MAX_INPUT_LENGTH ], pom[ MAX_INPUT_LENGTH ];
    char out[ MAX_STRING_LENGTH ];
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "bpol" ) )
	return;

    /* 13.12.2001 Cadaver: bpol <poziom1> [poziom2] */
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    value1 = is_number( arg1 ) ? atoi( arg1 ) : LEVEL_HERO;
    value2 = is_number( arg2 ) ? atoi( arg2 ) : MAX_LEVEL;

    value1 = URANGE( LEVEL_HERO, value1, MAX_LEVEL );
    value2 = URANGE( LEVEL_HERO, value2, MAX_LEVEL );

    if ( !*arg2 && *arg1 )
    {
	sprintf( out, "Polecenia dost`epne nie`smiertelnym na poziomie %d:", value1 );
	value2 = value1;
    }

    if ( value2 < value1 )
    {
	lv = value2;
	value2 = value1;
	value1 = lv;
    }

    if ( !*arg1 || *arg2 )
	sprintf( out, "Polecenia dost`epne nie`smiertelnym na poziomach %d - %d:", value1, value2 );

    for ( lv = value1; lv <= value2; lv++ )
    {
	pik = FALSE;
	col = 0;
	sprintf( pim, "Poziom {y%3d{x:\n\r", lv );
	for ( cmd = 0; cmd_table[ cmd ].name[ 0 ]; cmd++ )
	{
	    if ( cmd_table[ cmd ].level == lv )
	    {
		if ( !pik )
		{
		    strcat( out, "\n\r" );
		    strcat( out, pim );
		    pik = TRUE;
		}
		else if ( !( ++col % 7 ) )
		    strcat( out, "\n\r" );

		ma = ( ( is_name( cmd_table[ cmd ].name, rch->pcdata->immskll )
			|| is_name2( cmd_table[ cmd ].name, "wr`o`c" ) )
			&& cmd_table[ cmd ].level <= get_trust( ch ) );

		sprintf( pom, "%s%s%s", ma ? "{g" : "",
		    wyrownaj( cmd_table[ cmd ].name, 11 ), ma ? "{x" : "" );

		strcat( out, pom );
	    }
	}
    }
    strcat( out, "\n\r" );
    send_to_char( out, ch );

    return;
}


/*
 * Lam 3.4.98: Flag Stat
 */
KOMENDA( do_fstat )
{
    char buf[ MAX_STRING_LENGTH ];
    CHAR_DATA *vt, *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "fstat" ) )
	return;

    vt = ch;

    if ( *argument && !( vt = get_char_world_pc( ch, argument ) ) )
    {
	send_to_char( "Gdzie ty go widzisz?\n\r", ch );
	return;
    }
    if ( IS_NPC( vt ) )
    {
	send_to_char( "To mob.\n\r", ch );
	return;
    }

    sprintf( buf,  "Posta`c:{G %s {x\n\r"
#if defined( MAM_SKASUJ )
		"MI`LO`S`C    [%c] %s\n\r"
#endif
		"UCISZENIE [%c] %s\n\rBEZEMOCJI [%c] %s\n\r"
		"BEZKOLORU [%c] %s\n\rBEZMOWY   [%c] %s\n\r"
		"LOG       [%c] %s\n\rMRO`ZONKA  [%c] %s\n\r"
		"Z`LODZIEJ  [%c] %s\n\rMORDERCA  [%c] %s\n\rZAB`OJCA   [%c] %s\n\r",
		vt->name,
#if defined( MAM_SKASUJ )
		IS_SET( vt->act, PLR_LOVE     ) ? 'X' : ' ',
		    vt->pcdata->love.od,
#endif
		IS_SET( vt->act, PLR_SILENCE  ) ? 'X' : ' ',
		    vt->pcdata->silence.od,
		IS_SET( vt->act, PLR_NO_EMOTE ) ? 'X' : ' ',
		    vt->pcdata->noemote.od,
		IS_SET( vt->act, PLR_NO_COLOUR ) ? 'X' : ' ',
		    vt->pcdata->nocolour.od,
		IS_SET( vt->act, PLR_NO_TELL  ) ? 'X' : ' ',
		    vt->pcdata->notell.od,
		IS_SET( vt->act, PLR_LOG      ) ? 'X' : ' ',
		    vt->pcdata->log.od,
		IS_SET( vt->act, PLR_FREEZE   ) ? 'X' : ' ',
		    vt->pcdata->freeze.od,
		IS_SET( vt->act, PLR_THIEF    ) ? 'X' : ' ',
		    vt->pcdata->thief_from,
		IS_SET( vt->act, PLR_KILLER   ) ? 'X' : ' ',
		    vt->pcdata->killer_from,
		IS_SET( vt->act, PLR_ZABOJCA  ) ? 'X' : ' ',
		    vt->pcdata->zabojca_from );
    send_to_char( buf, ch );

    return;
}


/* Ulryk 19.11.2003 */
KOMENDA( do_odwampirz )
{
    CHAR_DATA *wampir;
    char       arg[ MAX_INPUT_LENGTH ];
    bool       odwampirzyc = FALSE;

    if ( !authorized( get_char( ch ), "odwampirz" ) )
	return;

    argument = one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: odwampirz <posta`c>\n\r          odwampirz -tak <posta`c>\n\r\n\rOdwampirz bez argumentu \"-tak\" wy`swietli tylko informacje na temat postaci.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "-tak" ) )
    {
	odwampirzyc = TRUE;
	one_argument( argument, arg );
    }

    if ( !( wampir = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( wampir ) )
    {
	send_to_char( "Nie na mobach!\n\r", ch );
	return;
    }

    if ( !CZY_WAMPIR( wampir ) )
    {
	send_to_char( "To nie wampir.\n\r", ch );
	return;
    }

    if ( odwampirzyc )
    {
	if ( wampir->pcdata->wampiryzm.rasa < 0
	  || wampir->pcdata->wampiryzm.rasa >= MAX_RACE
	  || ( !wampir->pcdata->wampiryzm.strefa )
	  || *wampir->pcdata->wampiryzm.strefa == '\0' )
	{
	    send_to_char( "Nie mo`zesz tego zrobi`c (brak danych do odwampirzenia).\n\r", ch );
	    return;
	}

	if ( IS_SET( wampir->act, PLR_WAMPIR_OD_URODZENIA ) )
	{
	    STC( "Nie mo`zesz tego zrobi`c (posta`c po restarcie wampirem).\n\r", ch );
	    return;
	}

	affect_strip( ch, gsn_polymorph_other ); /* Lam 19.11.2003 */
	affect_strip( ch, gsn_postac_nietoperza );
	wampir->race = wampir->pcdata->wampiryzm.rasa;
	wampir->pcdata->wampiryzm.rasa = -1;
	wampir->pcdata->strefa = znajdz_strefe( wampir->pcdata->wampiryzm.strefa );
	free_string( wampir->pcdata->wampiryzm.strefa );
	wampir->pcdata->wampiryzm.strefa = str_dup( "" );
	free_string( wampir->pcdata->wampiryzm.pochodzenie );
	wampir->pcdata->wampiryzm.pochodzenie = str_dup( "" );
	wampir->pcdata->wampiryzm.moc = -1;
	send_to_char( "Rasa przywr`ocona.\n\r", ch );
	send_to_char( "Bogowie przywr`ocili ci tw`a star`a ras`e.\n\r", wampir );
	return;
    }

    ch_printf( ch, "{GImi`e{x:%30s%s\n\r", "", wampir->short_descr );
    ch_printf( ch, "{YPoprzednia rasa{x:%19s%s\n\r", "",
	       wampir->pcdata->wampiryzm.rasa < MAX_RACE
	    && wampir->pcdata->wampiryzm.rasa >= 0 ?
	       race_table[ wampir->pcdata->wampiryzm.rasa ].who_name :
	       "brak danych" );
    ch_printf( ch, "{YObywatelstwo{x:%22s%s\n\r", "",
	       wampir->pcdata->wampiryzm.strefa
	    && *wampir->pcdata->wampiryzm.strefa != '\0' ?
	       wampir->pcdata->wampiryzm.strefa :
	       "brak danych" );
    ch_printf( ch, "{YPokrewie`nstwo i data zwampirzenia{x: %s\n\r",
	       wampir->pcdata->wampiryzm.pochodzenie
	    && *wampir->pcdata->wampiryzm.pochodzenie ?
	       wampir->pcdata->wampiryzm.pochodzenie :
	       "brak danych" );
    ch_printf( ch, "{YStopie`n rozcie`nczenia krwi{x: %8d\n\r",
	       wampir->pcdata->wampiryzm.moc );
    ch_printf( ch, "{YJest wampirem {G%d{Y raz.{x\n\r",
	       wampir->pcdata->wampiryzm.ile );

    return;
}


KOMENDA( do_rekord )
{
    int  i;
    char buf[ MAX_STRING_LENGTH ];
    char buf2[ MAX_INPUT_LENGTH ];

    if ( !authorized( get_char( ch ), "rekord" ) )
	return;

    buf[ 0 ] = '\0';

    sprintf( buf, "Rekord graczy od startu: %2d\n\r", max_play.total );

    for ( i = 0; i < MAX_DAYS; i++ )
    {
	sprintf( buf2, "%s%2d dni temu", ( i % 2 ) ? "{W" : "{x", i );
	strcat( buf, ( i == 0 ) ? "       Dzi`s"
		   : ( i == 1 ) ? "{W    wczoraj"
		   : buf2 );
    }

    strcat( buf, "\n\r" );

    for ( i = 0; i < MAX_DAYS; i++ )
    {
	sprintf( buf2, "%s%11d", ( i % 2 ) ? "{W" : "{x", max_play.day[ i ] );
	strcat( buf, buf2 );
    }

    strcat( buf, "\n\r" );
    sprintf( buf2, "Rekord muda: {Y%2d{x, zanotowany %s\r\n\r",
	     max_play_all_time, max_play_all_time_time );
    strcat( buf, buf2 );

    sprintf( buf2, "Rekord po`l`acze`n od startu: %2d\n\r", max_desc.total );
    strcat( buf, buf2 );

    for ( i = 0; i < MAX_DAYS; i++ )
    {
	sprintf( buf2, "%s%2d dni temu", ( i % 2 ) ? "{W" : "{x", i );
	strcat( buf, ( i == 0 ) ? "       Dzi`s"
		   : ( i == 1 ) ? "{W    wczoraj"
		   : buf2 );
    }

    strcat( buf, "\n\r" );

    for ( i = 0; i < MAX_DAYS; i++ )
    {
	sprintf( buf2, "%s%11d", ( i % 2 ) ? "{W" : "{x", max_desc.day[ i ] );
	strcat( buf, buf2 );
    }

    strcat( buf, "\n\r" );
    sprintf( buf2, "Rekord muda: {Y%2d{x, zanotowany %s\r",
	     max_desc_all_time, max_desc_all_time_time );
    strcat( buf, buf2 );

    send_to_char( buf, ch );

    return;
}


/*
 * Rozpoczynanie gry od nowa. Skopiowalem wlasne delete.
 * Lam 1.5.98: dodatkowe sprawdzanie, czy level < LEVEL_HERO, to dla postaci
 * z zaufaniem (np. takich po restarcie).
 *
 * Lam 20.5.98: jednak zaufani moga restartowac do woli
 */
KOMENDA( do_restar )
{
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Restart nie jest dla mob`ow.\n\r", ch );
	return;
    }

    if ( *argument ) /* sztuczka */
	REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_RESTART );

    send_to_char( "Musisz napisa`c RESTART w ca`lo`sci je`sli chcesz zacz`a`c gr`e od nowa.\n\r", ch );

    return;
}


KOMENDA( do_restart )
{
    if ( IS_NPC( ch ) || !ch->desc )
    {
	send_to_char( "Restart nie jest dla mob`ow.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_RESTART ) )
    {
	if ( *argument )
	{
	    REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_RESTART );
	    send_to_char( "Jednak nie chcesz zacz`a`c gry od nowa...\n\r", ch );
	    return;
	}
	else
	{
	    /* czy mozemy wyjsc z gry? */
	    if ( !can_quit_now( ch, FALSE ) )
		return;

	    PRZERWIJ_KOPANIE( ch, FALSE );
	    PRZERWIJ_KARMPOJ( ch, FALSE );

	    save_char_obj( ch );

	    /* wymaganie hasla chyba jest tu oczywiste */
	    send_to_char( "Podaj has`lo, `zebym wiedzia`l, `ze to na pewno ty.\n\r"
			  "Has`lo: ", ch );
	    send_to_char( (char *) &echo_off_str[ 0 ], ch );
	    ch->pcdata->was_in_room = ch->in_room;
	    char_from_room( ch );
	    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
	    ch->desc->connected = CON_GET_OLD_PWD_RESTART;
	    return;
	}
    }

    if ( *argument )
	send_to_char( "Napisz restart bez argumentu, aby rozpocz`a`c gr`e od nowa.\n\r", ch );
    else
    {
	send_to_char(
		"Je`sli naprawd`e chcesz rozpocz`a`c gr`e od nowa, napisz RESTART raz jeszcze.\n\r"
		"Za`lo`zysz wtedy now`a posta`c, kt`ora jednak wci`a`z b`edzie posiada`la zaufanie\n\r"
		"odpowiadaj`ace twemu obecnemu poziomowi. Tylko niekt`ore umiej`etno`sci pozostan`a\n\r"
		"przy tobie, z wi`ekszo`sci`a si`e po`zegnasz. Doradzam wyb`or innej kombinacji\n\r"
		"rasy/profesji ni`z obecnie, najlepiej trudniejszej!\n\r"
		"Je`sli napiszesz restart z parametrem, flaga pragnienia restartu zostanie\n\r"
		"z ciebie zdj`eta.\n\r", ch );
	SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_RESTART );
    }

    return;
}


/*
 * Robienie dowolnych przedmiotow z poziomu muda, a nie w krainach.
 */

/* zamienia lancuch na liczbe APPLY_* */
int co_ma_zmieniac( char *gdzie, bool pszuk )
{
    if ( !gdzie || !*gdzie			) return APPLY_NONE;

    if ( !str_prefix( gdzie, "si`l" )		) return APPLY_STR;
    if ( !str_prefix( gdzie, "m`ad" )		) return APPLY_WIS;
    if ( !str_prefix( gdzie, "int" )		) return APPLY_INT;
    if ( !str_prefix( gdzie, "bud" )		) return APPLY_CON;
    if ( !str_prefix( gdzie, "zr`e" )		) return APPLY_DEX;
    if ( !str_prefix( gdzie, "`zycie" )		) return APPLY_HIT;
    if ( !str_prefix( gdzie, "mana" )		) return APPLY_MANA;
    if ( !str_prefix( gdzie, "ruch" )		) return APPLY_MOVE;
    if ( !str_prefix( gdzie, "kp" )		) return APPLY_AC;
    if ( !str_prefix( gdzie, "celno`s`c" )	) return APPLY_HITROLL;
    if ( !str_prefix( gdzie, "obra`zenia" )	) return APPLY_DAMROLL;
    if ( !str_prefix( gdzie, "opc" )		) return APPLY_SAVING_SPELL;

    if ( pszuk ) /* Lam: tych da sie tylko szukac, a nie ustawiac */
    {
	if ( !str_prefix( gdzie, "p`le`c" )	) return APPLY_SEX;
	if ( !str_prefix( gdzie, "profesja" )	) return APPLY_CLASS;
	if ( !str_prefix( gdzie, "poziom" )	) return APPLY_LEVEL;
	if ( !str_prefix( gdzie, "wiek" )	) return APPLY_AGE;
	if ( !str_prefix( gdzie, "z`loto" )	) return APPLY_GOLD;
	if ( !str_prefix( gdzie, "do`swiadczenie"))return APPLY_EXP;
	if ( !str_prefix( gdzie, "rasa" )	) return APPLY_RACE;
    }

    return APPLY_NONE;
}


KOMENDA( do_oaff )
{
    char objname[ MAX_INPUT_LENGTH ];
    char cco[ MAX_INPUT_LENGTH ];
    int co;
    int ile;
    OBJ_DATA *obj;
    AFFECT_DATA *af;
    bool nowy = FALSE;

    if ( !authorized( get_char( ch ), "pdzia`l" ) )
	return;

    argument = one_argument( argument, objname );
    argument = one_argument( argument, cco );
    if ( !*argument || !cco[ 0 ] || !objname[ 0 ] || !is_number( argument ) )
    {
	send_to_char(	"U`zycie: pdzia`l <przedmiot> <co> <ile> - dodaje/zmienia\n\r"
			"        pdzia`l <przedmiot> <co> 0     - usuwa\n\r"
			" <co>: si`l m`ad int bud zr`e `zycie mana ruch kp celno`s`c obra`zenia opc\n\r"
			"Sprawd`x dzia`lanie przedmiotu poleceniem pstat.\n\r"
			"Zmieni`c mo`zna jedynie przedmiot, a nie jego indeks.\n\r"
			, ch );
	return;
    }

    if ( !( obj = get_obj_carry( ch, objname ) ) )
    {
	send_to_char( "Przedmiot musisz mie`c w inwentarzu.\n\r", ch );
	return;
    }

    if ( ( co = co_ma_zmieniac( cco, FALSE ) ) == APPLY_NONE )
    {
	send_to_char( "Nie rozumiem, co ma zmienia`c ten przedmiot.\n\r", ch );
	return;
    }

    /* wczesniej juz bylo sprawdzone, czy is_number( arg ) */
    ile = atoi( argument );

    /* znajdz wplyw, jesli jest */
    for ( af = obj->affected; af; af = af->next )
	if ( af->location == co )
	    break;

    if ( ile ) /* CHCEMY ZMIENIC */
    {
	if ( !af )
	{
	    /* takiego nie bylo, to go zrobmy */
	    nowy = TRUE;
	    af = new_affect();
	    af->type = 0; /* zarezerwowane */
	    af->duration = -1;
	    af->location = co; /* najwazniejsza rzecz */
	}

	af->modifier = ile;

	if ( nowy )
	{
	    af->next = obj->affected;
	    obj->affected = af;
	}
    }
    else /* CHCEMY USUNAC */
    {
	/* Lam: nie ustawiam af->deleted, tylko od razu usuwam. Niby Envy
	   poradzilaby sobie z usunieciem (list_update to uwzglednia), jednak
	   petle szukajace wplywow przedmiotow nie uwzgledniaja flagi deleted.
	   Komenda "%oaff <obj> hp 0|wear <obj>" ubralaby postac razem
	   z wplywem! Przy zdjeciu juz by wplywu nie bylo, bo list_update
	   usunela go tuz po ubraniu. Jednak tylko dlatego, ze dzialo sie to
	   wszystkow tym samym pulsie. */
	AFFECT_DATA *prev;

	if ( !af )
	{
	    send_to_char( "Takiego wp`lywu nie usuniesz, bo go nie by`lo.\n\r", ch );
	    return;
	}

	if ( obj->affected == af )
	{
	    obj->affected = af->next;
	}
	else
	{
	    for ( prev = obj->affected; prev; prev = prev->next )
		if ( prev->next == af )
		    break;
	    if ( !prev )
		bug( "do_oaff: usuwanie wplywu: nie ma go w liscie!", 0 );
	    /* powyzsze i tak sie nie zdarzy */
	    prev->next = af->next;
	}

	af->next = affect_free;
	affect_free = af;
    }

    send_to_char( "Zrobione.\n\r", ch );

    return;
}


/*
 * Lam 31.5.98: jest dobrze zabezpieczona - nie mozna dac sobie ani komus na
 * rownym poziomie doswiadczenia, nie mozna dac wiecej niz 1000 punktow. Dziala
 * na niesmiertelnych. Mozna zabierac doswiadczenie i w konsekwencji obnizac
 * poziom.
 */
void lv_hlp( CHAR_DATA *ch ); /* update.c */
KOMENDA( do_exp )
{
    CHAR_DATA *vict;
    char arg1[ MIL ];
    char buf[ MSL ];
    int gain;

    if ( !authorized( get_char( ch ), "do`swiadcz" ) )
	return;

    if ( !*argument )
    {
	send_to_char( "Kogo do`swiadczy`c?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );

    if ( !( vict = get_char_world_pc( ch, arg1 ) )
      || IS_NPC( vict ) )
    {
	send_to_char( "Nie ma takiej postaci.\n\r", ch );
	return;
    }

    if ( vict == ch )
    {
	if ( ch->sex == 2 )
	    send_to_char( "Spryciula z ciebie :)\n\r", ch );
	else
	    send_to_char( "Spryciarz z ciebie :)\n\r", ch );

	return;
    }

    if ( get_trust( vict ) >= get_trust( ch ) )
    {
	send_to_char( "Tego ci nie wolno.\n\r", ch );
	return;
    }

    if ( !*argument || !is_number( argument ) )
    {
	send_to_char( "Podaj liczb`e punkt`ow do`swiadczenia.\n\r", ch );
	return;
    }

    gain = atoi( argument );

    if ( gain > EXP_PER_LEVEL || gain < -EXP_PER_LEVEL )
    {
	ch_printf( ch, "Nie mo`zesz da`c/zabra`c wi`ecej ni`z %d d`s.\n\r", EXP_PER_LEVEL );
	return;
    }

    ch_printf( vict, "%s obdarowuje ci`e %d pkt do`swiadczenia.\n\r", ch->short_descr, gain );

    vict->exp = UMAX( EXP_PER_LEVEL, vict->exp + gain );
    vict->exp = UMIN( 110 * EXP_PER_LEVEL, vict->exp );

    while ( gain > 0
	 && vict->exp >= EXP_PER_LEVEL * ( vict->level + 1 ) )
    {
	sprintf( buf, "{M$@{x uda`lo si`e osi`agn`a`c poziom {Y%d{x!", vict->level + 1 );
	wiznet( buf, vict, NULL, NULL, WIZ_LEVEL, 0, 101 );
	send_to_char( "{GGratuluj`e! UDA`LO CI SI`E ZDOBY`C POZIOM!{x\n\r", vict );
	vict->level++;
	advance_level( vict );
	lv_hlp( vict );
    }

    while ( gain < 0
	 && vict->exp < EXP_PER_LEVEL * vict->level )
    {
	sprintf( buf, "{M$N{x traci poziom {Y%d{x!", vict->level );
	wiznet( buf, vict, NULL, NULL, WIZ_LEVEL, 0, 101 );
	send_to_char( "{GPrzykro mi, tracisz poziom.{x\n\r", vict );
	demote_level( vict );
/*        vict->level--; robi demote_level */
    }

    send_to_char( "Do`swiadczenie przyznane.\n\r", ch );

    return;
}


/*
 * Lam 6.7.98: prezentuje rozlozenie ilosci mobow/przedmiotow na poziomy.
 * Ladnie pokazuje, przystosowane jednak wylacznie do 110 poziomow.
 * poziomy 111 i 0 pokazuja moby wykraczajace poza zakres <1;110>
 */
KOMENDA( do_count )
{
    MOB_INDEX_DATA *mobi;
    CHAR_DATA *mob;
    OBJ_DATA *obj;
    char wyjscie[ MSL ];
    int ilosc[ 112 ];
    int licz;
    int gdzie;
    int i;
    bool m_o = FALSE;
    bool ind = FALSE;

    if ( !authorized( get_char( ch ), "ilo`s`c" ) )
	return;

    one_argument( argument, wyjscie );
    if ( !str_prefix( wyjscie, "indeks`ow" ) )
    {
	ind = TRUE;
	argument = one_argument( argument, wyjscie );
    }

    if ( !*argument )
    {
	send_to_char( "Sk`ladnia: ilo`s`c <[\"indeks`ow\"] \"mob`ow\"|\"przedmiot`ow\">\n\r", ch );
	return;
    }

    if ( !str_prefix( argument, "mob`ow" ) )
	m_o = TRUE;
    else if ( str_prefix( argument, "przedmiot`ow" ) )
    {
	send_to_char( "Sk`ladnia: ilo`s`c mob`ow | ilo`s`c przedmiot`ow\n\r", ch );
	return;
    }

    for ( licz = 0; licz <= 111; licz++ )
	ilosc[ licz ] = 0;

    if ( m_o )
    {
	if ( ind )
	{
	    for ( i = 0; i < MAX_KEY_HASH; i++ )
		for ( mobi = mob_index_hash[ i ]; mobi; mobi = mobi->next )
		    ilosc[ URANGE( 0, mobi->level, 111 ) ]++;

	    strcpy( wyjscie, "Roz`lo`zenie indeks`ow mob`ow na poziomy:\n\r" );
	}
	else
	{
	    for ( mob = char_list; mob; mob = mob->next )
	    {
		if ( mob->deleted )
		    continue;
		ilosc[ URANGE( 0, mob->level, 111 ) ]++;
	    }

	    strcpy( wyjscie, "Roz`lo`zenie mob`ow na poziomy:\n\r" );
	}
    }
    else
    {
	if ( ind )
	{
	    STC( "Indeksy przedmiot`ow nie maj`a poziom`ow.\n\r", ch );
	    return;
	}

	for ( obj = object_list; obj; obj = obj->next )
	{
	    if ( obj->deleted )
		continue;

	    /* Lam 20.3.2004: nie zlicza przedmiotow, ktorych ch nie widzi */
	    if ( !can_see_obj( ch, obj ) )
		continue;

	    ilosc[ URANGE( 0, obj->level, 111 ) ]++;
	}
	strcpy( wyjscie, "Roz`lo`zenie przedmiot`ow na poziomy:\n\r" );
    }

    gdzie = 111;

    for ( licz = 111; licz >= 0; licz-- )
    {
	sprintf( wyjscie + strlen( wyjscie ), "{W%3d{x:%5d  ", gdzie, ilosc[ gdzie ] );
	gdzie -= 16;

	if ( gdzie < 0 )
	    gdzie += 111;

	if ( !( licz % 7 ) )
	    strcat( wyjscie, "\n\r" );
    }

    send_to_char( wyjscie, ch );

    return;
}


/*
 * Lam i Winchester 20-21.8.98 (pomysl RenTora)
 */
KOMENDA( do_aexits )
{
    AREA_DATA *area;
    ROOM_INDEX_DATA *room, *room2;
    EXIT_DATA *pexit;
    OBJ_DATA *obj;
    int dir;
    char buf[ MSL * 4 ], buf1[ MSL * 2 ], buf2[ MSL * 2 ];
    bool bylo_wyjscie = FALSE, byl_portal = FALSE;

    if ( !authorized( get_char( ch ), "kwyj`scia" ) )
	return;

    if ( !*argument )
	area = ch->in_room->area;
    else
    {
	for ( area = area_first; area; area = area->next )
	    if ( !str_prefix( argument, area->file_name ) )
		break;

	if ( !area )
	{
	    send_to_char( "Podaj nazw`e krainy.\n\r", ch );
	    return;
	}
    }

    sprintf( buf, "{GWyj`scia z krainy \"%s\":{x", area->file_name );
    sprintf( buf1, "\n\r{YWyj`scia:{x\n\r" );
    sprintf( buf2, "\n\r{YPortale:{x\n\r" );

    for ( room = area->first_room; room; room = room->next_in_area )
    {
	for ( dir = 0; dir < 10; dir++ )
	{
	    pexit = room->exit[ dir_order[ dir ] ];

	    if ( pexit && pexit->to_room && pexit->to_room->area != area )
	    {
		bylo_wyjscie = TRUE;
		sprintf( buf1 + strlen( buf1 ),
			"Wyj`scie z %d:%-2s do %-5d (%s)\n\r",
					room->vnum, kierunki[ dir ].skrotd,
					pexit->to_room->vnum,
					pexit->to_room->area->file_name );
	    }
	}

	for ( obj = room->contents; obj; obj = obj->next_content )
	{
	    if ( obj->deleted || obj->item_type != ITEM_PORTAL )
		continue;

	    room2 = get_room_index( obj->value[ 1 ].v );

	    if ( room2 && room2->area != area )
	    {
		byl_portal = TRUE;
		sprintf( buf2 + strlen( buf2 ),
			"Portal  w %d - %-15s - do %-5d (%s)\n\r",
					room->vnum, obj->short_descr,
					room2->vnum,
					room2->area->file_name );
	    }
	}
    }

    if ( !byl_portal && !bylo_wyjscie )
    {
	strcat( buf, " {R`zadne!{x\n\r" );
	send_to_char( buf, ch );
	return;
    }

    if ( bylo_wyjscie )
	strcat( buf, buf1 );

    if ( byl_portal )
	strcat( buf, buf2 );

    send_to_char( buf, ch );

    return;
}


KOMENDA( do_stopien )
{
    CHAR_DATA *vict;
    char arg1[ MIL ];

    if ( !authorized( get_char( ch ), "stopie`n" ) )
	return;

    if ( !*argument )
    {
	send_to_char( "Komu zmieni`c stopie`n?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );

    if ( !( vict = get_char_world_pc( ch, arg1 ) )
      || IS_NPC( vict ) )
    {
	send_to_char( "Nie ma takiej postaci.\n\r", ch );
	return;
    }

    if ( get_trust( vict ) >= get_trust( ch ) && ch != vict )
    {
	send_to_char( "Tego ci nie wolno.\n\r", ch );
	return;
    }

    if ( !*argument )
    {
	send_to_char( "Podaj nowy stopie`n.\n\r", ch );
	return;
    }

    if ( strlen_pl( argument ) > 13 )
    {
	send_to_char( "Najwy`zej 13 znak`ow.\n\r", ch );
	return;
    }

    if ( strstr( argument, "{" ) )
    {
	send_to_char( "Kolory nie s`a dopuszczalne.\n\r", ch );
	return;
    }

    smash_tilde( argument );
    free_string( vict->pcdata->stopien );

    if ( !str_cmp( argument, "null" )
      || !str_cmp( argument, "brak" ) )
    {
	vict->pcdata->stopien = str_dup( "" );
    }
    else
	vict->pcdata->stopien = str_dup( argument );

    send_to_char( "Stopie`n nadany.\n\r", ch );

    return;
}

/* komentarz Lama do ponizszego dziela Winchestera: przedmiotom nie klonuja
   sie ani wplywy, ani opisy. Nie ma to znaczenia, jesli sa brane
   z obj->pIndexData, ale jesli sa dodane odpowiednio przez pdzial i pustaw,
   to mamy problem (trzeba wszystko robic dwa razy). */
/* Winchester */

OBJ_DATA *obj_clon( OBJ_DATA* tmp_obj, bool all )
{
    OBJ_DATA *obj, *t_obj;
    int i;

    obj                 = new_object();

    obj->pIndexData     = tmp_obj->pIndexData;
    obj->level          = tmp_obj->level;
    obj->wear_loc       = tmp_obj->wear_loc;
    obj->name           = str_dup( tmp_obj->name );
    obj->short_descr    = str_dup( tmp_obj->short_descr );
    obj->dopelniacz     = str_dup( tmp_obj->dopelniacz );
    obj->celownik       = str_dup( tmp_obj->celownik );
    obj->biernik        = str_dup( tmp_obj->biernik );
    obj->narzednik      = str_dup( tmp_obj->narzednik );
    obj->miejscownik    = str_dup( tmp_obj->miejscownik );
    obj->description    = str_dup( tmp_obj->description );
    obj->look_descr     = str_dup( tmp_obj->look_descr );
    obj->action         = str_dup( tmp_obj->action );
    obj->anti_class     = str_dup( tmp_obj->anti_class );
    obj->anti_race      = str_dup( tmp_obj->anti_race );
    obj->owner          = str_dup( tmp_obj->owner );
    obj->item_type      = tmp_obj->item_type;
    COPY_BITS( obj->extra_flags, tmp_obj->extra_flags );
    obj->wear_flags     = tmp_obj->wear_flags;

    /* Alandar: obsluga wartosci bedacych opisami */
    for ( i = 0; i < 6; i++ )
	if ( typy_przedmiotow[ obj->item_type ].spell[ i ] == VAL_STRING )
	{
	    if ( tmp_obj->value[ i ].p )
		obj->value[ i ].p = (void *) str_dup( ( char * ) tmp_obj->value[ i ].p );
	    else
		obj->value[ i ].p = NULL;
	}
	else
	    obj->value[ i ].v	= tmp_obj->value[ i ].v;

    obj->weight         = tmp_obj->weight;
    obj->rodzaj         = tmp_obj->rodzaj;
    obj->cost           = tmp_obj->cost;
    obj->item_type      = tmp_obj->item_type;

    obj->next           = object_list;
    object_list         = obj;

    if ( all )
	for ( t_obj = tmp_obj->contains; t_obj; t_obj = t_obj->next_content )
	    obj_to_obj( obj_clon( t_obj, TRUE ), obj );

    return obj;
}


KOMENDA( do_mklon )
{
    char       tmp1[ MAX_INPUT_LENGTH ];
    char       tmp2[ MAX_INPUT_LENGTH ];
    CHAR_DATA *ofiara;
    CHAR_DATA *mob;
    OBJ_DATA  *obj;
    int        i, max_cp = 0;
    int        all = 0;

    argument = one_argument( argument, tmp1 );
    argument = one_argument( argument, tmp2 );

    if ( !authorized( get_char( ch ), "mklon" ) )
	return;

    if ( !*tmp1 || !str_cmp( tmp1, "pomoc" ) || !str_cmp( tmp1, "?" ) )
    {
	send_to_char( "{GSk`ladnia:{W mklon < kogo > [ ile ] [ + ].{x\n\r", ch );
	return;
    }

    if ( !( ofiara = get_char_room( ch, tmp1 ) ) )
    {
	send_to_char( "Nie widz`e przy tobie ochotnika do sklonowania.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ofiara ) )
    {
	send_to_char( "Nie wolno ci klonowa`c graczy!\n\r", ch );
	return;
    }

    if ( !*tmp2 )
    {
	max_cp = 1;
    }
    else if ( *tmp2 == '+' )
    {
	max_cp = 1;
	all = 1;
    }
    else
    {
	if ( !( max_cp = atoi( tmp2 ) ) || max_cp > 5 )
	{
	    send_to_char( "Nie mo`zesz sklonowa`c takiej ilo`sci mob`ow!\n\r", ch );
	    return;
	}

	if ( *argument == '+' )
	    all = 1;
    }

    for ( i = 1; i <= max_cp; i++ )
    {
	mob                 = new_character( FALSE );

	mob->pIndexData     = ofiara->pIndexData;
	mob->area           = ofiara->area;
	mob->reset_room     = ofiara->reset_room;
	mob->name           = str_dup( ofiara->name );
	mob->short_descr    = str_dup( ofiara->short_descr );
	mob->long_descr     = str_dup( ofiara->long_descr );
	mob->long_descr_orig= str_dup( ofiara->long_descr_orig );
	mob->false_name     = str_dup( ofiara->false_name );
	mob->description    = str_dup( ofiara->description );
	mob->dopelniacz     = str_dup( ofiara->dopelniacz );
	mob->celownik       = str_dup( ofiara->celownik );
	mob->biernik        = str_dup( ofiara->biernik );
	mob->narzednik      = str_dup( ofiara->narzednik );
	mob->miejscownik    = str_dup( ofiara->miejscownik );
	mob->wolacz	    = str_dup( ofiara->wolacz );
	mob->spec_fun       = ofiara->spec_fun;
	mob->separator      = '|';
	mob->level          = ofiara->level;
	mob->act            = ofiara->act;
	mob->trust          = ofiara->trust;
	COPY_BITS( mob->affected_by, ofiara->affected_by );
	mob->alignment      = ofiara->alignment;
	mob->sex            = ofiara->sex;
	mob->race           = ofiara->race;
	mob->gold           = ofiara->gold;
	mob->exp            = ofiara->exp;
	mob->armor          = ofiara->armor;
	mob->marmor         = ofiara->marmor;
	mob->max_hit        = ofiara->max_hit;
	mob->hit            = mob->max_hit;
	mob->max_mana       = ofiara->max_mana;
	mob->mana           = mob->max_mana;
	mob->max_move       = ofiara->max_move;
	mob->move           = mob->max_move;
	mob->saving_throw   = ofiara->saving_throw;
	mob->hitroll        = ofiara->hitroll;
	mob->damroll        = ofiara->damroll;

	mob->next           = char_list;
	char_list           = mob;

	char_to_room( mob, ch->in_room );

	if ( all )
	    for ( obj = ofiara->carrying; obj; obj = obj->next_content )
		obj_to_char( obj_clon( obj, TRUE ), mob );
    }

    return;
}

KOMENDA( do_oklon )
{
    char tmp1[ MAX_INPUT_LENGTH ];
    char tmp2[ MAX_INPUT_LENGTH ];
    OBJ_DATA *ofiara;
    int i, max_cp = 0;
    int all = 0;

    argument = one_argument( argument, tmp1 );
    argument = one_argument( argument, tmp2 );

    if ( !authorized( get_char( ch ), "pklon" ) )
	return;

    if ( !*tmp1 || !str_cmp( tmp1, "pomoc" ) || !str_cmp( tmp1, "?" ) )
    {
	send_to_char( "{GSk`ladnia:{W pklon < co > [ ile ] [ + ].{x\n\r", ch );
	return;
    }

    if ( !( ofiara = get_obj_carry( ch, tmp1 ) ) )
    {
	send_to_char( "Nie nosisz w inwentarzu tej rzeczy do sklonowania.\n\r", ch );
	return;
    }

    if ( !*tmp2 )
	max_cp = 1;
    else if ( *tmp2 == '+' )
	all = 1;
    else
    {
	if ( !( max_cp = atoi( tmp2 ) ) || max_cp > 5 )
	{
	    send_to_char( "Nie mo`zesz sklonowa`c takiej ilo`sci przedmiot`ow!\n\r", ch );
	    return;
	}

	if ( *argument == '+' )
	    all = 1;
    }

    for ( i = 1; i <= max_cp; i++ )
	obj_to_char( obj_clon( ofiara, all ), ch );

    return;
}


/*
 * Lam 22.12.98
 *     26.6.99: zapisywanie do pliku vnums.are przy starcie muda
 *     20.3.2005: vnumy <kraina> z wbudowanym szukaniem wolnych
 *     2.9.2005: vnumy -mob/-prz
 * bardzo powolna funkcja (z powodu tysiecy uzyc get_room_index)
 */
KOMENDA( do_vnumy )
{
    ROOM_INDEX_DATA *room;
    MOB_INDEX_DATA *mob;
    OBJ_INDEX_DATA *obj;
    AREA_DATA *area = NULL;
    int anum;
    int rnum;
    int numod, numdo;
    int ilosc;
    int gmeram = 0; /* 0 - w pomieszczeniach, 1 - w mobach, 2 - w przedmiotach */
    char buf[ MSL ];
    FILE *fp = NULL;
    AREA_DATA *kraina = NULL;
    bool jest;

    if ( ch && !authorized( get_char( ch ), "vnumy" ) )
	return;

    one_argument( argument, buf );

    if ( *buf == '-' )
    {
	if ( !str_prefix( buf, "-mob`ow" ) )
	{
	    gmeram = 1;
	}
	else if ( !str_prefix( buf, "-przedmiot`ow" ) )
	{
	    gmeram = 2;
	}
	else
	{
	    STC( "Sk`ladnia: vnumy [-mob|-prz] [kraina.are].\n\r", ch );
	    return;
	}

	argument = one_argument( argument, buf );
    }

    if ( *argument )
    {
	for ( kraina = area_first; kraina; kraina = kraina->next )
	    if ( !str_prefix( argument, kraina->file_name ) )
		break;

	if ( !kraina )
	{
	    STC( "Sk`ladnia: vnumy [-mob|-prz] [kraina.are].\n\r", ch );
	    return;
	}
    }

    if ( ch )
    {
	if ( kraina )
	    ch_printf( ch, "{MPrzynale`zno`s`c vnum`ow do krainy {R%s{M:{x\n\r", kraina->file_name );
	else if ( gmeram == 2 )
	    send_to_char( "{MPrzynale`zno`s`c vnum`ow {Rprzedmiot`ow{M do krain:{x\n\r", ch );
	else if ( gmeram == 1 )
	    send_to_char( "{MPrzynale`zno`s`c vnum`ow {Rmob`ow{M do krain:{x\n\r", ch );
	else
	    send_to_char( "{MPrzynale`zno`s`c vnum`ow {Rpomieszcze`n{M do krain:{x\n\r", ch );
    }
    else
    {
	if ( !( fp = fopen( AVNUM_FILE, "w" ) ) )
	{
	    lac_perror( AVNUM_FILE );
	    return;
	}
	fprintf( fp, "Podczas ostatniego uruchomienia, Lac wczytal nastepujace krainy:\n\n" );
    }

    /* przechodzi vnumami 0-99999, powinno zalezec od MAX_VNUM */
    for ( anum = 0; anum < 1000; anum++ )
    {
	buf[ 0 ] = '\0';
	for ( rnum = 100 * anum; rnum < 100 * anum + 100; rnum++ )
	{
	    jest = FALSE;

	    if ( gmeram == 1
	      && ( mob = get_mob_index( rnum ) )
	      && ( area = mob->index_area ) )
	    {
		jest = TRUE;
	    }

	    if ( gmeram == 2
	      && ( obj = get_obj_index( rnum ) )
	      && ( area = obj->area ) )
	    {
		jest = TRUE;
	    }

	    if ( gmeram == 0
	      && ( room = get_room_index( rnum ) )
	      && ( area = room->area ) )
	    {
		jest = TRUE;
	    }

	    if ( jest
	      && ( !kraina || kraina == area )
	      && str_infix( area->file_name, buf ) )
	    {
		sprintf( buf + strlen( buf ), "%s ", area->file_name );

		if ( kraina )
		    break;
	    }
	}

	if ( rnum < 100 * anum + 100 ) /* przerwana petla - znaleziona kraina */
	{
	    if ( ch )
		strcat( buf, "{x\n\r{cWolne vnumy w tym przedziale:{x\n\r" );
	    else
		strcat( buf, "{x\n{cWolne vnumy w tym przedziale:{x\n" );

	    ilosc = 0;
	    numod = numdo = -1;

	    for ( rnum = 100 * anum; rnum < 100 * anum + 100; rnum++ )
		if ( ( gmeram == 0 && !get_room_index( rnum ) )
		  || ( gmeram == 1 && !get_mob_index( rnum ) )
		  || ( gmeram == 2 && !get_obj_index( rnum ) ) )
		{
		    if ( numod == -1 )
		    {
			numod = numdo = rnum;
			sprintf( buf + strlen( buf ), "%d", rnum );
		    }
		    else if ( numdo + 1 == rnum )
		    {
			numdo = rnum;
		    }
		    else if ( numod == numdo )
		    {
			numod = numdo = rnum;
			sprintf( buf + strlen( buf ), " %d", rnum );
		    }
		    else
		    {
			sprintf( buf + strlen( buf ), "-%d %d", numdo, rnum );
			numod = numdo = rnum;
		    }

		    ilosc++;
		}

	    if ( numod != numdo )
		sprintf( buf + strlen( buf ), "-%d", numdo );

	    sprintf( buf + strlen( buf ), " ({yrazem %d{x)", ilosc );
	}

	if ( buf[ 0 ] )
	{
	    if ( ch )
	    {
		if ( kraina )
		    STC( "{C", ch );

		ch_printf( ch, "%3d00 - %3d99: %s\n\r", anum, anum, buf );
	    }
	    else
		fprintf( fp, "%3d00 - %3d99: %s\n", anum, anum, buf );
	}
    }

    if ( !ch )
	fclose( fp );

    return;
}


/*
 * Lam 27.11.2000
 */
KOMENDA( do_pomszukaj )
{
    ROOM_INDEX_DATA *pom;
    char buf[ MSL ];
    char buf1[ MSL*4 ];
    char arg[ MIL ];
    int vnum;
    bool found = FALSE;

    if ( !authorized( get_char( ch ), "pomszukaj" ) )
	return;

    one_argument( argument, arg );

    if ( !*arg )
    {
	STC( "Sk`ladnia: pomszukaj <fragment nazwy pomieszczenia>\n\r", ch );
	return;
    }

    if ( strlen_pl( arg ) <= 3 )
    {
	STC( "Podaj fragment nazwy szukanego pomieszczenia d`lu`zszy ni`z 3 znaki.\n\r", ch );
	return;
    }

    buf1[ 0 ] = '\0';

    /* mozna napisac szukanie po kolei, bo tak jest wolno, ale za to ladniej */
    for ( vnum = 0; vnum < 65536; vnum++ )
	if ( ( pom = get_room_index( vnum ) ) )
	    if ( !str_infix( arg, pom->name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r", pom->vnum, pom->name );
		strcat( buf1, buf );
	    }

    if ( !found )
	STC( "Nie znalaz`lem pomieszczenia o pasuj`acej nazwie.\n\r", ch );
    else
	STC( buf1, ch );

    return;
}


/*
 * Lam 22.11.2000: napisalem to polecenie w dobrej wierze. Wielokrotnie
 * gracze pytali mnie, czy jak oni wychodza z gry, to ja moge wgrac ich postac
 * i zrobic im co chce, a ja odpowiadalem negatywnie. Nigdy tego nie chcialem,
 * jednakze czasem zdarzaja sie sytuacje, gdy chcialbym moc pomoc graczowi,
 * ktorego akurat nie ma w grze, np. wybaczyc morderstwo. Nie naduzywac.
 */
KOMENDA( do_gladuj )
{
    static DESCRIPTOR_DATA d0;
    DESCRIPTOR_DATA d = d0;
    CHAR_DATA *vch;
    int pomieszczenie = 0;
    bool istnial;
    char arg[ MIL ];

    if ( !authorized( get_char( ch ), "g`laduj" ) )
	return;

    one_argument( argument, arg );

    if ( !*arg )
    {
	STC( "Czyj`a posta`c za`ladowa`c?\n\r", ch );
	return;
    }

    strcpy( arg, zwielkoliteruj( arg ) );

    for ( vch = char_list; vch; vch = vch->next )
    {
	if ( vch->deleted )
	    continue;

	if ( !IS_NPC( ch ) && is_name2( arg, vch->name ) )
	{
	    STC( "Ten gracz jest obecnie w grze.\n\r", ch );
	    return;
	}
    }

    vch = nowa_postac( &d, arg );
    istnial = load_char_obj( &d, arg );
    vch->desc = NULL;

    if ( !istnial )
    {
	STC( "Taka posta`c nie istnieje.\n\r", ch );
	free_char( vch, TRUE );
	return;
    }

    if ( IS_SET( vch->act, PLR_DENY ) )
	STC( "Uwaga! `Ladujesz gracza wyrzuconego z gry (PLR_DENY)!\n\r", ch );

    sprintf( log_buf, "%s laduje postac \"%s\".", ch->name, vch->name );
    log_string( log_buf );

    if ( vch->pcdata && vch->pcdata->ic_in_room )
	pomieszczenie = vch->pcdata->ic_in_room->vnum;
    else
	pomieszczenie = ROOM_VNUM_GRAVEYARD_A;

    vch->in_room = NULL;

    if ( get_trust( vch ) >= get_trust( ch ) )
    {
	sprintf( log_buf, "%s broni sie przed zaladowaniem!", vch->name );
	log_string( log_buf );
	char_to_room( vch, ch->in_room );
	act( ZM_WZROK, "Nagle pojawia si`e tu $n, {rbardzo {Rrozgniewan$y{x!", vch, NULL, NULL, TO_ROOM );
	do_say( vch, "Jaki`z to b`lazen `smie przerywa`c m`oj sen tym razem?" );
	do_say( vch, "Ostrzegam, {Rzostawcie mnie w spokoju!" );
	act( ZM_WZROK, "$n zamyka oczy, po czym rozp`lywa si`e w powietrzu.", vch, NULL, NULL, TO_ROOM );
	char_from_room( vch );
	free_char( vch, TRUE );
	STC( "Mo`ze lepiej nie pr`obuj wi`ecej `ladowa`c tej postaci?\n\r", ch );
	return;
    }

    vch->next = char_list;
    char_list = vch;

    /* brakuje tu kilku rzeczy z nanny.c, np. sprawdzania klanu, sprawdzania
       rekordu graczy itp. */

    char_to_room( vch, ch->in_room );
    act( ZM_WZROK | ZM_WID_CHAR, "{WPowietrze zaczyna si`e iskrzy`c, a w`sr`od trzask`ow pojawia si`e tu $n!{x", vch, NULL, NULL, TO_ROOM );
    act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz jakie`s trzaski.", vch, NULL, NULL, TO_ROOM );
    ch_printf( ch, "Posta`c za`ladowana. Poprzednie pomieszczenie postaci ma vnum {G%d{x.\n\r", pomieszczenie );

    return;
}


/*
 * Qwert 26.11.2000: pozwala opoznic gracza o ustalony czas w sekundach lub
 * opoznic wszystkich. Gracz otrzymuje komunikat, ze zostaje przez ciebie
 * obdarowany lagiem.
 */
KOMENDA( do_laguj )
{
    CHAR_DATA          *rch;
    CHAR_DATA          *poor = NULL;
    DESCRIPTOR_DATA    *d;
    char               arg1[ MAX_INPUT_LENGTH ];
    char               arg2[ MAX_INPUT_LENGTH ];
    int                na_ile;
    bool               lag;

    rch = get_char( ch );

    if ( !authorized( rch, "laguj" ) )
	return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: laguj <osoba|wsz> <lag>.\n\r", ch );
	return;
    }

    if ( !is_number( arg2 ) )
    {
	send_to_char( "Argument <lag> musi by`c liczb`a sekund.\n\r", ch );
	return;
    }

    na_ile = atoi( arg2 );

    if ( na_ile <= 0 || na_ile > 250 )
    {
	send_to_char( "Nie mo`zesz da`c laga wi`ekszego ni`z 250 sekund.\n\r", ch );
	return;
    }

    lag = !str_cmp( arg1, "wsz" );

    if ( !lag && !( poor = get_char_world_pc( ch, arg1 ) ) )
    {
	send_to_char( "Nie ma nikogo takiego.\n\r", ch );
	return;
    }

    if ( !lag && IS_NPC( poor ) )
    {
	send_to_char( "W jakim celu chcesz lagowa`c moba?\n\r", ch );
	return;
    }

    if ( !lag && ( ch == poor ) )
    {
	send_to_char( "Zmie`n `l`acze na gorsze je`sli to ci nie pasuje...\n\r", ch );
	return;
    }

    /* Lam */
    if ( !lag && ( get_trust( poor ) >= get_trust( ch ) ) )
    {
	STC( "Mo`zesz lagowa`c tylko ni`zszych od siebie.\n\r", ch );
	return;
    }

    if ( lag && get_trust( ch ) < L_DIR )
    {
	send_to_char( "Tylko prawdziwi bogowie mog`a lagowa`c wszystkich.\n\r", ch );
	return;
    }

    if ( lag )
    {
	for ( d = descriptor_list; d; d = d->next )
	    if ( d->connected == CON_PLAYING
		&& d->character != ch
		&& d->character->in_room
		&& can_see( ch, d->character )
		&& ( get_trust( ch ) > get_trust( d->character ) ) )
	    {
		d->character->wait = UMAX( na_ile * 4, d->character->wait );
		act( ZM_ZAWSZE, "$n w swej `laskawo`sci obdarowuje ci`e lagiem.", ch, NULL, d->character, TO_VICT );
	    }
    }
    else
    {
	poor->wait = UMAX( na_ile * 4, poor->wait );
	act( ZM_ZAWSZE, "$n w swej `laskawo`sci obdarowuje ci`e lagiem.", ch, NULL, poor, TO_VICT );
    }

    send_to_char( "Lag wys`lany.\n\r", ch );

    return;
}


/*
 * Lam 4.12.2000
 */
KOMENDA( do_racestat )
{
    FILE *fp = NULL;
/*  HELP_DATA *help; */
    int i, j;
    char buf[ MSL ];
    char buf1[ MSL * 4 ];
    int max;
    bool byla;

    if ( !ch )
    {
	if ( !( fp = fopen( RACESTAT_FILE, "w" ) ) )
	{
	    lac_perror( RACESTAT_FILE );
	    return;
	}
    }
    else
	if ( !authorized( get_char( ch ), "rstat" ) )
	    return;

    if ( ch )
	strcpy( buf1, "Rasa         ang       wielk  waga  si`l int m`ad zr`e bud   +`z  +m  +r  prag g`l`od\n\r" );
    else
	fprintf( fp, "Rasa         ang       wielk  waga  sil int mad zre bud   +z  +m  +r  prag glod\n" );

    max = 0;

    for ( i = 0; i < MAX_RACE; i++ )
    {
	if ( *argument
	  && str_prefix( argument, race_table[ i ].who_name )
	  && str_prefix( argument, race_table[ i ].ang ) )
	{
	    continue;
	}

	sprintf( buf, "%s %-12s %2d %5d  %3d %3d %3d %3d %3d  %3d %3d %3d  %4d %4d\n%s",
		wyrownaj( ch ? race_table[ i ].who_name : race_table[ i ].name, -12 ),
		race_table[ i ].ang,
		race_table[ i ].size,
		race_table[ i ].weight / 4,
		UMIN( 23 + race_table[ i ].str_mod, 26 ),
		UMIN( 23 + race_table[ i ].int_mod, 26 ),
		UMIN( 23 + race_table[ i ].wis_mod, 26 ),
		UMIN( 23 + race_table[ i ].dex_mod, 26 ),
		UMIN( 23 + race_table[ i ].con_mod, 26 ),
		race_table[ i ].hp_gain,
		race_table[ i ].mana_gain,
		race_table[ i ].move_gain,
		race_table[ i ].thirst_mod,
		race_table[ i ].hunger_mod,
		ch ? "\r" : "" );

	if ( ch )
	    strcat( buf1, buf );
	else
	    fprintf( fp, "%s", buf );
/*
	for ( help = help_first; help; help = help->next )
	    if ( is_name2( race_table[ i ].who_name, help->keyword ) )
		break;
	if ( !ch && !help )
	    fprintf( fp, "Rasa '%s' nie ma pomocy.\n", race_table[ i ].who_name );
*/
	if ( max < race_table[ i ].size )
	    max = race_table[ i ].size;
    }

    if ( ch )
	STC( buf1, ch );
    else
    {
	fprintf( fp, "\n\n" );

	for ( j = 0; j <= max; j++ )
	{
	    fprintf( fp, "Rasy o wielkosci %d:", j );
	    byla = FALSE;

	    for ( i = 0; i < MAX_RACE; i++ )
		if ( race_table[ i ].size == j )
		{
		    fprintf( fp, " %s", race_table[ i ].name );
		    byla = TRUE;
		}

	    if ( byla )
		fprintf( fp, "\n" );
	    else
		fprintf( fp, " (brak)\n" );
	}
	fclose( fp );
    }

    return;
}


/*
 * Lam 23.12.2003
 * Alucard 4.2.2004: dodalem wyswietlanie vnuma i czy mob jest zaladowany oraz
 * zmienilem wciecia
 */
KOMENDA( do_mlista )
{
    AREA_DATA *a;
    MOB_INDEX_DATA *mi;
    int poz;
    int rozmiar_b1, rozmiar_b;
    char buf1[ MSL * 4 ];
    char buf[ MSL ];
    bool czy_zaladowany;

    if ( !authorized( get_char( ch ), "mlista" ) )
	return;

    if ( !*argument )
    {
	STC( "Podaj nazw`e pliku krainy.\n\r", ch );
	return;
    }

    buf1[ 0 ] = '\0';

    for ( a = area_first; a; a = a->next )
	if ( !str_prefix( argument, a->file_name ) )
	{
	    sprintf( buf1, "Postacie zamieszkuj`ace krain`e %s (* - agr, - - tch):\n\r", a->file_name );
	    rozmiar_b1 = strlen( buf1 );
	    for ( poz = UMIN( 110, a->highmoblevel );
		  poz >= a->lowmoblevel; poz-- )
	    {
		for ( mi = a->first_mob; mi; mi = mi->next_in_area )
		    if ( ( poz == 110 && mi->level >= 110 )
		      || mi->level == poz )
		    {
			/* Lam 20.1.2006: obecna wersja jest szybka, ale efekt
			   uboczny jest taki, ze resety danego indeksu w innych
			   krainach tez sa liczone - w razie potrzeby,
			   poprzednia wersja jest w komentarzu ponizej */
			czy_zaladowany = mi->resetcount;
			/* czy_zaladowany = FALSE;

			for ( pReset = a->reset_first; pReset; pReset = pReset->next )
			    if ( ( pReset->command == 'M' )
			      && ( pReset->arg1 == mi->vnum ) )
			    {
				czy_zaladowany = TRUE;
				break;
			    } */

			sprintf( buf, "%s%s[%3d] [v %5d] %s%s\n\r",
			    IS_SET( mi->act, ACT_AGGRESSIVE ) ? "*" : " ",
			    IS_SET( mi->act, ACT_WIMPY ) ? "-" : " ",
			    mi->level,
			    mi->vnum,
			    mi->short_descr,
			    czy_zaladowany ? "" : " [nie `ladowany]" );
			rozmiar_b = strlen( buf );

			if ( rozmiar_b1 + rozmiar_b < MAX_STRING_LENGTH * 4 )
			{
			    strcat( buf1, buf );
			    rozmiar_b1 += rozmiar_b;
			}
			else
			{
			    bug( "Zbyt ma`ly bufor.", 0 );
			    STC( "Ta kraina ma za du`zo postaci.\n\r", ch );
			    return;
			}
		    }
	    }
	    break;
	}

    if ( *buf1 )
	STC( buf1, ch );
    else
	STC( "Nie znalaz`lem krainy.\n\r", ch );

    return;
}


/*
 * Lam 13.7.2006
 * Od mlisty rozni sie zasadniczo, bo biega nie po indeksach, a po odnawianych
 * w krainie mobach (chocby byly z innych krain).
 *
 * Niezla bylaby mozliwosc pogrupowania ich indeksami (jak mlista) z jakims
 * specjalnym oznaczeniem, ze nie wszystkie moby z indeksu sa ubrane tak samo
 * dobrze. To by sie przydalo tylko w bardzo duzych krainach z wieloma mobami
 * robionymi z jednego indeksu.
 */
KOMENDA( do_ulista )
{
    AREA_DATA *a;
    MOB_INDEX_DATA *mi;
    RESET_DATA *res;
    int i;
    int rozmiar_b1, rozmiar_b;
    char buf1[ MSL * 4 ];
    char buf[ MSL ];
    bool ubrany[ MAX_WEAR ]; /* przyszlosciowo, moge dodac dowolne pola */

    if ( !authorized( get_char( ch ), "ulista" ) )
	return;

    if ( !*argument )
    {
	STC( "Podaj nazw`e pliku krainy.\n\r", ch );
	return;
    }

    buf1[ 0 ] = '\0';
    mi = FALSE;

    for ( a = area_first; a; a = a->next )
	if ( !str_prefix( argument, a->file_name ) )
	{
	    sprintf( buf1, "[pzm] [v  vnum] c n s b (cia`lo, nogi, stopy, bro`n), kraina {W%s{x\n\r", a->file_name );
	    rozmiar_b1 = strlen( buf1 );
	    for ( res = a->reset_first; res; res = res->next )
	    {
		if ( res->command == 'M' )
		{
		    if ( mi )
		    {
			sprintf( buf, "[%3d] [v%6d] %c %c %c %c %s\n\r",
			    mi->level,
			    mi->vnum,
			    ubrany[ WEAR_BODY ] ? 'X' : '-',
			    ubrany[ WEAR_LEGS ] ? 'X' : '-',
			    ubrany[ WEAR_FEET ] ? 'X' : '-',
			    /* WIELD2 nie sprawdzam celowo, jesli nosi bron
			       tylko w drugiej rece, niech przelozy */
			    ubrany[ WEAR_WIELD ] ? 'X' : '-',
			    mi->short_descr );
			rozmiar_b = strlen( buf );

			if ( rozmiar_b1 + rozmiar_b < MAX_STRING_LENGTH * 4 )
			{
			    strcat( buf1, buf );
			    rozmiar_b1 += rozmiar_b;
			}
			else
			{
			    bug( "Zbyt ma`ly bufor.", 0 );
			    STC( "Ta kraina ma za du`zo postaci.\n\r", ch );
			    return;
			}
		    }
		    mi = get_mob_index( res->arg1 );

		    for ( i = 0; i < MAX_WEAR; i++ )
			ubrany[ i ] = FALSE;
		}

		if ( res->command == 'E' )
		    ubrany[ res->arg3 ] = TRUE;
	    }
	    break;
	}

    /* to powinno byc funkcja */
    if ( mi )
    {
	sprintf( buf, "[%3d] [v %5d] %c %c %c %c %s\n\r",
	    mi->level,
	    mi->vnum,
	    ubrany[ WEAR_BODY ] ? 'X' : '-',
	    ubrany[ WEAR_LEGS ] ? 'X' : '-',
	    ubrany[ WEAR_FEET ] ? 'X' : '-',
	    /* WIELD2 nie sprawdzam celowo, jesli nosi bron tylko w drugiej
	       rece, niech przelozy */
	    ubrany[ WEAR_WIELD ] ? 'X' : '-',
	    mi->short_descr );
	rozmiar_b = strlen( buf );

	if ( rozmiar_b1 + rozmiar_b < MAX_STRING_LENGTH * 4 )
	    strcat( buf1, buf );
	else
	{
	    bug( "Zbyt ma`ly bufor.", 0 );
	    STC( "Ta kraina ma za du`zo postaci.\n\r", ch );
	    return;
	}
    }

    if ( *buf1 )
	STC( buf1, ch );
    else
	STC( "Nie znalaz`lem krainy.\n\r", ch );

    return;
}


/* Alucard 4.2.2004 */
KOMENDA( do_mistat )
{
    MOB_INDEX_DATA *pMobIndex;
    RESET_DATA     *pReset;
    AREA_DATA      *pArea;
    CHAR_DATA      *victim;
    char            buf [ MAX_STRING_LENGTH     ];
    char            buf1[ MAX_STRING_LENGTH * 4 ];
    char            arg [ MAX_INPUT_LENGTH      ];
    char            resets[ MAX_INPUT_LENGTH ];
    int             vnum;

    if ( !authorized( get_char( ch ), "mistat" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: mistat <posta`c>\n\r"
		      "          mistat <vnum>\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) )
    {
	pMobIndex = victim->pIndexData;

	if ( !pMobIndex )
	{
	    STC( "Gracze nie maj`a indeks`ow.\n\r", ch );
	    return;
	}

	vnum = victim->pIndexData->vnum;
    }
    else
    {
	vnum = atoi( arg );

	if ( !( pMobIndex = get_mob_index( vnum ) ) )
	{
	    send_to_char( "Nie ma nikogo takiego.\n\r", ch );
	    return;
	}
    }

    pArea = pMobIndex->index_area;
    resets[ 0 ] = '\0';

    if ( pArea != NULL )
	for ( pReset = pArea->reset_first; pReset; pReset = pReset->next )
	    if ( ( pReset->command == 'M' ) && ( pReset->arg1 == vnum ) )
	    {
		sprintf( buf, "%d ", pReset->arg3 );
		strcat( resets, buf );
	    }

    if ( resets[ 0 ] == '\0' )
	sprintf( resets, "(brak)" );

    buf1[ 0 ] = '\0';

    sprintf( buf, "Imi`e: %s.\n\r",
	     pMobIndex->player_name );
    strcat( buf1, buf );

    sprintf( buf, "Rasa: %s (%s).  Poziom: %d.\n\r",
	     race_table[ pMobIndex->race ].who_name,
	     race_table[ pMobIndex->race ].ang,
	     pMobIndex->level );
    strcat( buf1, buf );

    sprintf( buf, "Vnum: %d.  P`le`c: %s.  Resety: %s\n\r",
	     vnum,
	     pMobIndex->sex == SEX_MALE    ? "m`eska"   :
	     pMobIndex->sex == SEX_FEMALE  ? "`ze`nska" : "nijaka",
	     resets );
    strcat( buf1, buf );

    sprintf( buf, "Ilo`s`c: %d  Zgon`ow: %d\n\r",
	     pMobIndex->count, pMobIndex->killed );
    strcat( buf1, buf );

    sprintf( buf, "Zamo`zno`s`c: %d  Moralno`s`c: %d\n\r",
	     pMobIndex->zamoznosc, pMobIndex->alignment );
    strcat( buf1, buf );

    sprintf( buf, "Act: %d, %s.\n\rWp`lywy: %s.\n\r",
	     pMobIndex->act, act_flag_name_pl( pMobIndex->act, ch ),
	     affect_bit_name_pl( pMobIndex->affected_by ) );
    strcat( buf1, buf );

    sprintf( buf, "D`lugi opis: %s\n\r",
	     pMobIndex->long_descr[ 0 ] != '\0' ? pMobIndex->long_descr
					      : "(brak)" );
    strcat( buf1, buf );

    sprintf( buf, "Mianownik: {W%s{x;  Dope`lniacz: {W%s{x;\n\rCelownik: {W%s{x;  Biernik: {W%s{x;\n\rNarz`ednik: {W%s{x;  Miejscownik: {W%s{x;\n\rWo`lacz: {W%s{x.\n\r",
	pMobIndex->short_descr, pMobIndex->dopelniacz, pMobIndex->celownik,
	pMobIndex->biernik, pMobIndex->narzednik, pMobIndex->miejscownik,
    pMobIndex->wolacz );
    strcat( buf1, buf );

    if ( pMobIndex->pShop )
    {
	int itype;
	bool cokolwiek = FALSE;

	sprintf( buf, "Mob prowadzi sklep, w kt`orym sprzedaje za %d%% i skupuje za %d%% warto`sci.\n\r",
		pMobIndex->pShop->profit_buy,
		pMobIndex->pShop->profit_sell );
	strcat( buf1, buf );

	sprintf( buf, "Sklep jest otwarty od %d do %d, a skupuje przedmioty typ`ow:",
		pMobIndex->pShop->open_hour,
		pMobIndex->pShop->close_hour );

	for ( itype = 0; itype < MAX_TRADE; itype++ )
	    if ( pMobIndex->pShop->buy_type[ itype ]
	      && pMobIndex->pShop->buy_type[ itype ] < ITEM_MAX
	      && *typy_przedmiotow[ pMobIndex->pShop->buy_type[ itype ] ].name )
	    {
		strcat( buf, " " );
		strcat( buf, typy_przedmiotow[ pMobIndex->pShop->buy_type[ itype ] ].name_pl );
		cokolwiek = TRUE;
	    }

	if ( !cokolwiek )
	    strcat( buf, " (brak).\n\r" );
	else
	    strcat( buf, ".\n\r" );

	strcat( buf1, buf );
    }

    if ( pMobIndex->spec_fun )
    {
	sprintf( buf, "Mob ma specjalno`s`c \"%s\".\n\r",
		 spec_name( pMobIndex->spec_fun ) );
	strcat( buf1, buf );
    }

    if ( pMobIndex->game_fun )
    {
	sprintf( buf, "Mob prowadzi gr`e \"%s\".\n\r",
		 game_name( pMobIndex->game_fun ) );
	strcat( buf1, buf );
    }

    send_to_char( buf1, ch );

    return;
}


/* A trivial rehack of do_mstat.  This doesnt show all the data, but just
 * enough to identify the mob and give its basic condition.  It does however,
 * show the MOBprograms which are set.
 */
KOMENDA( do_mpstat )
{
    char        bigbuf[ MSL * 10 ];
    char        buf[ MAX_STRING_LENGTH ];
    char        arg[ MAX_INPUT_LENGTH  ];
    MPROG_DATA *mprg;
    CHAR_DATA  *victim;

    if ( !authorized( get_char( ch ), "mpstat" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: mpstat <posta`c>?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "Nie widz`e takiej postaci.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Tylko moby maj`a mobprogi.\n\r", ch );
	return;
    }

    if ( !( victim->pIndexData->progtypes ) )
    {
	send_to_char( "Ten mob nie ma prog`ow.\n\r", ch );
	return;
    }

    bigbuf[ 0 ] = '\0';
    sprintf( buf, "{MImi`e: %s.  Vnum: %d.\n\r",
	victim->name, victim->pIndexData->vnum );
    strcat( bigbuf, buf );

    sprintf( buf, "Kr`otki: %s.\n\rD`lugi: %s\n\r",
	    victim->short_descr,
	    victim->long_descr[ 0 ] != '\0' ?
	    victim->long_descr : "(brak)" );
    strcat( bigbuf, buf );

    sprintf( buf, "`Zycie: %d/%d.  Mana: %d/%d.  Ruch: %d/%d.\n\r",
	victim->hit,         victim->max_hit,
	victim->mana,        victim->max_mana,
	victim->move,        victim->max_move );
    strcat( bigbuf, buf );

    sprintf( buf,
	"Poz: %d.  Moralno`s`c: %d.  KP: %d.  MKP: %d.  Z`loto: %d.{x\n\r",
	victim->level, victim->alignment,
	GET_AC( victim ), GET_MAC( victim ), victim->gold );
    strcat( bigbuf, buf );

    for ( mprg = victim->pIndexData->mobprogs;
	  mprg != NULL;
	  mprg = mprg->next )
    {
	if ( mprg->parent )
	{
	    sprintf( buf, "({g%s{x) ", mprg->parent->arglist );
	    strcat( bigbuf, buf );
	}

	sprintf( buf, "{W>%s %s{x\n\r%s\n\r",
		mprog_type_to_name( mprg->type, 0 ),
		mprg->arglist,
		mprg->comlist );
	strcat( bigbuf, buf );
    }

    send_to_char( bigbuf, ch );

    return;
}


/*
 * Lam 15.6.98: rpstat
 * Lam 12.5.99: nawet jesli pomieszczenie nie ma progow, wyswietlane sa progi
 * wyjsc
 */
KOMENDA( do_rpstat )
{
    char buf[ MSL * 10 ];
    char arg[ MIL ];
    MPROG_DATA *mprg;
    ROOM_INDEX_DATA *location;
    int door;
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "pompstat" ) )
	return;

    one_argument( argument, arg );
    location = !arg[ 0 ] ? ch->in_room : find_location( ch, arg );

    if ( !location )
    {
	send_to_char( "Nie ma takiego pomieszczenia.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( location )
      && get_trust( rch ) < 108 )
    {
	send_to_char( "To pomieszczenie jest chwilowo zaj`ete.\n\r", ch );
	return;
    }

    /* zmiana ch_printf na sprintf na zyczenie RenTora (bez przewijania) */
    sprintf( buf, "{YNazwa: %s.\n\rVnum: %d.  Pod`lo`ze: %d.  `Swiat`lo: %d.{x\n\r",
      location->name, location->vnum, location->sector_type, location->light );

    if ( !location->mobprogs )
    {
	strcpy( buf, "{RPomieszczenie nie ma prog`ow.{x\n\r" );
/*	return; */
    }

    for ( mprg = location->mobprogs; mprg; mprg = mprg->next )
    {
	if ( mprg->parent )
	    sprintf( buf + strlen( buf ), "({g%s{x) ", mprg->parent->arglist );
	sprintf( buf + strlen( buf ), "{W>%s %s{x\n\r%s\n\r",
		mprog_type_to_name( mprg->type, 1 ),
		mprg->arglist,
		mprg->comlist );
    }

    for ( door = 0; door < 10; door++ )
	if ( location->exit[ door ] && location->exit[ door ]->mobprogs )
	    sprintf( buf + strlen( buf ), "{MWyj`scie %d ma progi.{x\n\r",
									door );

    send_to_char( buf, ch );

    return;
}


/*
 * Lam 16.7.98: epstat
 */
KOMENDA( do_epstat )
{
    char buf[ MSL * 10 ];
    char arg[ MIL ];
    MPROG_DATA *mprg;
    int door;

    if ( !authorized( get_char( ch ), "wpstat" ) )
	return;

    one_argument( argument, arg );

    if ( !arg[ 0 ] )
    {
	send_to_char( "Sk`ladnia: wpstat <kierunek>.\n\r", ch );
	return;
    }

    if ( is_number( arg ) )
	door = atoi( arg );
    else
	for ( door = 0; door < 10; door++ )
	    if ( !str_cmp( kierunki[ door ].skrot, arg ) )
		break;

    if ( door < 0 || door > 9 )
    {
	do_epstat( ch, "" );
	return;
    }

    sprintf( buf, "{GPomieszczenie: %s.\n\rVnum: %d.  Wyj`scie na %s (%d).{x\n\r",
	ch->in_room->name, ch->in_room->vnum, kierunki[ door ].biernik, door );

    if ( !ch->in_room->exit[ door ] )
    {
	send_to_char( "{RPomieszczenie nie ma wyj`scia w t`e stron`e!{x\n\r", ch );
	return;
    }

    if ( !ch->in_room->exit[ door ]->mobprogs )
    {
	send_to_char( "{RWyj`scie nie ma prog`ow.{x\n\r", ch );
	return;
    }

    for ( mprg = ch->in_room->exit[ door ]->mobprogs; mprg; mprg = mprg->next )
    {
	if ( mprg->parent )
	    sprintf( buf + strlen( buf ), "({g%s{x) ", mprg->parent->arglist );

	sprintf( buf + strlen( buf ), "{W>%s %s{x\n\r%s\n\r",
		mprog_type_to_name( mprg->type, 2 ),
		mprg->arglist,
		mprg->comlist );
    }

    send_to_char( buf, ch );

    return;
}


/*
 * Lam 16.7.98: opstat
 */
KOMENDA( do_opstat )
{
    char buf[ MSL * 10 ];
    char arg[ MIL ];
    OBJ_DATA *obj;
    MPROG_DATA *mprg;

    if ( !authorized( get_char( ch ), "ppstat" ) )
	return;

    one_argument( argument, arg );

    if ( !arg[ 0 ] )
    {
	send_to_char( "Sk`ladnia: ppstat <przedmiot>.\n\r", ch );
	return;
    }

    if ( !( obj = get_obj_world( ch, arg ) ) )
    {
	send_to_char( "Nic takiego nie istnieje!\n\r", ch );
	return;
    }

    sprintf( buf, "{BPrzedmiot: %s.\n\rVnum: %d.{x\n\r",
	obj->name, obj->pIndexData->vnum );

    if ( !obj->pIndexData->mobprogs )
    {
	send_to_char( "{RPrzedmiot nie ma prog`ow.{x\n\r", ch );
	return;
    }

    for ( mprg = obj->pIndexData->mobprogs; mprg; mprg = mprg->next )
    {
	if ( mprg->parent )
	    sprintf( buf + strlen( buf ), "({g%s{x) ", mprg->parent->arglist );
	sprintf( buf + strlen( buf ), "{W>%s %s{x\n\r%s\n\r",
		mprog_type_to_name( mprg->type, 3 ),
		mprg->arglist,
		mprg->comlist );
    }

    send_to_char( buf, ch );

    return;
}


/*
 * Lam 14.11.1999: mpodpluskw, wykonywane przez niesmiertelnego w celu
 * odpluskwiania mobprogow moba
 */
KOMENDA( do_mpodpluskw )
{
    char arg[ MIL ];
    CHAR_DATA *victim;
    ZWOD_DATA *zwod;
    int pzl = 0;

    if ( !authorized( get_char( ch ), "mpodpluskw" ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	STC( "Sk`ladnia: {Ympodpluskw <posta`c>{x (mpodpluskw si`e, aby anulowa`c odpluskwianie)\n\r", ch );
	STC( "Aktualnie odpluskwiasz:\n\r", ch );
	for ( zwod = ch->zwod_debugger; zwod; zwod = zwod->next )
	{
	    ch_printf( ch, "  Mob #%-9d [w pomieszczeniu #%d]\n\r",
			zwod->ch->pIndexData->vnum,
			zwod->ch->in_room ? zwod->ch->in_room->vnum : 0 );
	    pzl = 1;
	}

	if ( !pzl )
	    STC( "  nikogo nie odpluskwiasz!\n\r", ch );

	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	STC( "Nie widz`e takiej postaci.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	for ( zwod = ch->zwod_debugger; zwod; zwod = zwod->next )
	{
	    if ( zwod->ch->debugger != ch )
		bug( "zwod->ch->debugger != ch", 0 );

	    zwod->ch->debugger = NULL;

	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_debugger;
		ch->zwod_debugger = NULL;
		break;
	    }
	}

	STC( "Ko`nczysz ewentualne odpluskwianie mob`ow.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	STC( "Tylko moby maj`a mobprogi!\n\r", ch );
	return;
    }

    if ( !( victim->pIndexData->progtypes ) )
    {
	STC( "Ten mob nie ma prog`ow.\n\r", ch );
	return;
    }

    if ( victim->debugger )
    {
	if ( victim->debugger == ch )
	{
	    STC( "Ju`z odpluskwiasz tego moba.\n\r", ch );
	    return;
	}
	else
	{
	    STC( "Ten mob jest ju`z przez kogo`s odpluskwiany.\n\r", ch );
	    return;
	}
    }

    victim->debugger = ch;
    dod_zwod( &ch->zwod_debugger, victim );
    STC( "Rozpoczynasz odpluskwianie!\n\r", ch );

    return;
}


/* Lam */
KOMENDA( do_asave )
{
    if ( !authorized( get_char( ch ), "kzapisz" ) )
	return;

    send_to_char( "U`zywaj \"lac -k\".\n\r", ch );
    return;

    ch_printf( ch, "Zapisuj`e krain`e \"%s\".\n\r", ch->in_room->area->file_name );
    save_area( ch->in_room->area, TRUE );

    return;
}


KOMENDA( do_zadstat )
{
    CHAR_DATA    *victim;
    MPQUEST_DATA *mpquest;
    char          arg[ MAX_INPUT_LENGTH ];
    char          bigbuf[ MAX_STRING_LENGTH * 4 ];
    char          buf[ MAX_STRING_LENGTH ];

    if ( !authorized( get_char( ch ), "zadstat" ) )
	return;

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Informacje o czyich zadaniach chcesz zobaczy`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz nikogo takiego.\n\r", ch );
	return;
    }

    if ( !victim->mpquest )
    {
	STC( "Brak danych o progowych zadaniach.\n\r", ch );
	return;
    }

    strcpy( bigbuf, "  Vnum  Etap  Warto`s`c0  Warto`s`c1  Warto`s`c2  Warto`s`c3  Warto`s`c4\n\r" );

    for ( mpquest = victim->mpquest; mpquest; mpquest = mpquest->next )
    {
	sprintf( buf, "%6d%6d%10d%10d%10d%10d%10d",
		    mpquest->vnum, mpquest->stage,
		    mpquest->data[ 0 ], mpquest->data[ 1 ],
		    mpquest->data[ 2 ], mpquest->data[ 3 ],
		    mpquest->data[ 4 ] );
	strcat( bigbuf, buf );

	if ( *mpquest->title )
	{
	    strcpy( buf, mpquest->title );
	    buf[ znajdz_adres_pl_kol( buf, 16 ) ] = '\0';
	    strcat( bigbuf, " " );
	    strcat( bigbuf, buf );
	}

	strcat( bigbuf, "\n\r" );
    }

    STC( bigbuf, ch );

    return;
}


KOMENDA( do_zadustaw )
{
    CHAR_DATA    *victim;
    MPQUEST_DATA *quest;
    char          arg[ MAX_INPUT_LENGTH ];
    int           qvnum;
    int           etap = 0;
    int           dane[ 5 ];
    int           i;
    bool          fEtap = FALSE;
    bool          fDane[ 5 ] = { FALSE, FALSE, FALSE, FALSE, FALSE };

    if ( !authorized( get_char( ch ), "zadustaw" ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia:\n\rzadustaw <kto> <vnum> [-etap <numer>|-dane <0-4> <ile>[-dane <0-4> <ile>[...]]]\n\r", ch );
	send_to_char( "zadustaw <kto> <vnum> -usu`n\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz nikogo takiego.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie na mobach.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || ( !is_number( arg ) ) )
    {
	send_to_char( "Brak vnumu zadania.\n\r", ch );
	return;
    }

    qvnum = atoi( arg );
    quest = find_mpquest( victim, qvnum );

    if ( !quest )
    {
	act( ZM_ZAWSZE, "$N nie robi tego zadania.", ch, NULL, victim, TO_CHAR );
	return;
    }

    while ( argument && *argument )
    {
	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "-usu`n" ) )
	{
	    if ( fEtap || fDane[ 0 ] || fDane[ 1 ] || fDane[ 2 ]
	      || fDane[ 3 ] || fDane[ 4 ] )
	    {
		STC( "Albo ustawiasz, albo usuwasz zadanie.\n\r", ch );
		return;
	    }

	    ch_printf( ch, "Usuwasz zadanie %d.\n\r", qvnum );
	    del_mpquest( victim, quest );
	    return;
	}
	else if ( !str_cmp( arg, "-etap" ) )
	{
	    if ( fEtap )
	    {
		send_to_char( "Etap ju`z by`l okre`slony.\n\r", ch );
		return;
	    }

	    fEtap = TRUE;

	    argument = one_argument( argument, arg );

	    if ( arg[ 0 ] == '\0' || ( !is_number( arg ) ) )
	    {
		send_to_char( "B`l`ad sk`ladni. Wpisz {Wzadustaw{x, aby sprawdzi`c poprawn`a sk`ladni`e.\n\r", ch );
		return;
	    }

	    etap = atoi( arg );
	}
	else if ( !str_cmp( arg, "-dane" ) )
	{
	    int tmp;

	    argument = one_argument( argument, arg );

	    if ( arg[ 0 ] == '\0' || ( !is_number( arg ) ) )
	    {
		send_to_char( "B`l`ad sk`ladni. Wpisz {Wzadustaw{x, aby sprawdzi`c poprawn`a sk`ladni`e.\n\r", ch );
		return;
	    }

	    tmp = atoi( arg );

	    if ( tmp < 0 || tmp > 4 )
	    {
		send_to_char( "B`l`ad sk`ladni. Wpisz {Wzadustaw{x, aby sprawdzi`c poprawn`a sk`ladni`e.\n\r", ch );
		return;
	    }

	    if ( fDane[ tmp ] )
	    {
		send_to_char( "Powt`orzone dane.\n\r", ch );
		return;
	    }

	    fDane[ tmp ] = TRUE;

	    argument = one_argument( argument, arg );

	    if ( arg[ 0 ] == '\0' || ( !is_number( arg ) ) )
	    {
		send_to_char( "B`l`ad sk`ladni. Wpisz {Wzadustaw{x, aby sprawdzi`c poprawn`a sk`ladni`e.\n\r", ch );
		return;
	    }

	    dane[ tmp ] = atoi( arg );
	}
	else
	{
	    send_to_char( "B`l`ad sk`ladni. Wpisz {Wzadustaw{x, aby sprawdzi`c poprawn`a sk`ladni`e.\n\r", ch );
	    return;
	}
    }

    if ( !( fEtap
         || fDane[ 0 ]
         || fDane[ 1 ]
         || fDane[ 2 ]
         || fDane[ 3 ]
         || fDane[ 4 ] ) )
    {
	send_to_char( "Niczego nie zmieniasz.\n\r", ch );
	return;
    }


    if ( fEtap )
	quest->stage = etap;

    for ( i = 0; i < 5; i++ )
	if ( fDane[ i ] ) quest->data[ i ] = dane[ i ];

    send_to_char( "Zrobione.\n\r", ch );

    return;
}


/*
 * Lam 3.6.2004
 */
KOMENDA( do_rhist )
{
    CHAR_DATA    *victim;
    RESTART_DATA *restart;
    char          arg[ MAX_INPUT_LENGTH ];
    char          buf[ MAX_STRING_LENGTH * 2 ];

    if ( !authorized( get_char( ch ), "rhist" ) )
	return;

    one_argument( argument, arg );

    if ( !*arg )
    {
	send_to_char( "Historia czyich restart`ow ci`e interesuje?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz nikogo takiego.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "To mob od zawsze i na zawsze.\n\r", ch );
	return;
    }

    if ( !victim->pcdata->restart )
    {
	ch_printf( ch, "To pierwsza gra %s.\n\r", victim->dopelniacz );
	return;
    }

    sprintf( buf, "Historia restart`ow %s:\n\r", victim->dopelniacz );

    for ( restart = victim->pcdata->restart; restart; restart = restart->nast )
    {
	strcat( buf, restart->wpis );
	strcat( buf, "\n\r" );
    }

    send_to_char( buf, ch );

    return;
}


/*
 * Lam 22.10.2004
 */
KOMENDA( do_teleciap )
{
    ZWOD_DATA *zwod;
    char buf[ MSL ];

    if ( !authorized( get_char( ch ), "teleciap" ) )
	return;

    buf[ 0 ] = '\0';

    if ( ch->zwod_retell )
	for ( zwod = ch->zwod_retell; zwod; zwod = zwod->next )
	{
	    zwod->ch->retell = NULL;

	    if ( can_see( ch, zwod->ch ) )
	    {
		strcat( buf, " " );
		strcat( buf, zwod->ch->short_descr );
	    }

	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_retell;
		ch->zwod_retell = NULL;
		break;
	    }
	}

    if ( ch->zwod_reply )
	for ( zwod = ch->zwod_reply; zwod; zwod = zwod->next )
	{
	    zwod->ch->reply = NULL;

	    /* is_name2 moze stwarzac problemy z mobami o wielowyrazowych
	       imionach (wyskoczy dwa razy) */
	    if ( can_see( ch, zwod->ch ) && !is_name2( zwod->ch->short_descr, buf ) )
	    {
		strcat( buf, " " );
		strcat( buf, zwod->ch->short_descr );
	    }

	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_reply;
		ch->zwod_reply = NULL;
		break;
	    }
	}

    if ( buf[ 0 ] )
	ch_printf( ch, "Ciapni`eto nast`epuj`ace ofiary:%s.\n\r", buf );
    else
	STC( "Nikogo nie ciapni`eto.\n\r", ch );

    return;
}


/*
 * Ulryk: 19.5.2005
 * pomlista: wyswietla pomieszczenia w krainie
 * plista: wyswietla przedmioty w krainie
 * Zabezpieczenie przed przepelnieniem bufora specjalnie wyswietla taki
 * komunikat, by osoba, ktora go doswiadczy, jak najszybciej powiadomila bogow.
 */
KOMENDA( do_pomlista )
{
    AREA_DATA       *a;
    ROOM_INDEX_DATA *ri;
    EXIT_DATA       *e;
    char             buf[ MAX_STRING_LENGTH ];
    char             buf1[ MAX_STRING_LENGTH * 4 ];
    int              kier;
    int              rozmiar_b1, rozmiar_b;
    bool             wyjscie;
    bool             jest = FALSE;

    if ( !authorized( get_char( ch ), "pomlista" ) )
	return;

    if ( !*argument )
    {
	STC( "Podaj nazw`e pliku krainy.\n\r", ch );
	return;
    }

    buf1[ 0 ] = '\0';

    for ( a = area_first; a; a = a->next )
	if ( !str_prefix( argument, a->file_name ) )
	{
	    sprintf( buf1, "Pomieszczenia w krainie %s. (* - wyj`scie z krainy)\n\r", a->file_name );
	    rozmiar_b1 = strlen( buf1 );

	    for ( ri = a->first_room; ri; ri = ri->next_in_area )
	    {
		wyjscie = FALSE;
		jest = TRUE;

		for ( kier = 0; kier < 10; kier++ )
		{
		    e = ri->exit[ kier ];

		    if ( e && e->to_room && e->to_room->area != a )
			wyjscie = TRUE;
		}

		sprintf( buf, "%s[v %5d] %s{x\n\r",
			 wyjscie ? "*" : " ", ri->vnum, ri->name );
		rozmiar_b = strlen( buf );

		if ( rozmiar_b1 + rozmiar_b < MAX_STRING_LENGTH * 4 )
		{
		    strcat( buf1, buf );
		    rozmiar_b1 += rozmiar_b;
		}
		else
		{
		    bug( "Zbyt ma`ly bufor.", 0 );
		    STC( "Ta kraina ma za du`zo pomieszcze`n.\n\r", ch );
		    return;
		}
	    }
	    break;
	}

    if ( a )
    {
	if ( jest )
	    STC( buf1, ch );
	else
	    ch_printf( ch, "W \"%s\" nie ma pomieszcze`n.\n\r", a->file_name );
    }
    else
	STC( "Nie znalaz`lem krainy.\n\r", ch );

    return;
}


KOMENDA( do_plista )
{
    AREA_DATA       *a;
    OBJ_INDEX_DATA  *oi;
    RESET_DATA      *r;
    char             buf[ MAX_STRING_LENGTH ];
    char             buf1[ MAX_STRING_LENGTH * 4 ];
    int              rozmiar_b1, rozmiar_b;
    bool             ladowany;
    bool             jest = FALSE;

    if ( !authorized( get_char( ch ), "plista" ) )
	return;

    if ( !*argument )
    {
	STC( "Podaj nazw`e pliku krainy.\n\r", ch );
	return;
    }

    buf1[ 0 ] = '\0';

    for ( a = area_first; a; a = a->next )
	if ( !str_prefix( argument, a->file_name ) )
	{
	    sprintf( buf1, "Przedmioty w krainie %s.\n\r", a->file_name );
	    rozmiar_b1 = strlen( buf1 );

	    for ( oi = a->first_obj; oi; oi = oi->next_in_area )
	    {
		ladowany = FALSE;
		jest = TRUE;

		for ( r = a->reset_first; r; r = r->next )
		    if ( ( r->command == 'O'
			|| r->command == 'P'
			|| r->command == 'G'
			|| r->command == 'E' )
		      && ( r->arg1 == oi->vnum ) )
		    {
			ladowany = TRUE;
			break;
		    }

		sprintf( buf, "[v %5d] [%3d] %s%s{x\n\r",
			  oi->vnum, oi->min_reset_level, oi->short_descr,
			  ladowany ? "" : " [nie `ladowany]" );
		rozmiar_b = strlen( buf );

		if ( rozmiar_b1 + rozmiar_b < MAX_STRING_LENGTH * 4 )
		{
		    strcat( buf1, buf );
		    rozmiar_b1 += rozmiar_b;
		}
		else
		{
		    bug( "Zbyt ma`ly bufor.", 0 );
		    STC( "Ta kraina ma za du`zo przedmiot`ow.\n\r", ch );
		    return;
		}
	    }
	    break;
	}

    if ( a )
    {
	if ( jest )
	    STC( buf1, ch );
	else
	    ch_printf( ch, "W \"%s\" nie ma przedmiot`ow.\n\r", a->file_name );
    }
    else
	STC( "Nie znalaz`lem krainy.\n\r", ch );

    return;
}


REFUSE_DATA *refuse_lookup( CHAR_DATA *ch, char *arg )
{
    REFUSE_DATA *refuse;

    if ( IS_NPC( ch ) )
	return NULL;

    if ( !ch->pcdata->refuse )
	return NULL;

    for ( refuse = ch->pcdata->refuse ; refuse ; refuse = refuse->next )
	if ( !str_cmp( arg, refuse->refused_cmd ) )
	    return refuse;

    return NULL;
}


KOMENDA( do_refuse )
{
    REFUSE_DATA *ref1;
    REFUSE_DATA *ref2;
    CHAR_DATA *rch;
    CHAR_DATA *victim;

    char buf [ MAX_STRING_LENGTH ];
    char arg1[ 40 ];
    char arg2[ 40 ];
    char arg3[ 40 ];
    int cmd;
    bool found = FALSE;

    rch = get_char( ch );

    if ( !authorized( rch, "odm`ow" ) )
	return;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( strlen( arg1 ) > 20
      || strlen( arg2 ) > 20
      || strlen( arg3 ) > 20
      || strlen( argument ) > 80 )
    {
	send_to_char( "Zbyt d`lugie argumenty.\n\r", ch );
	return;
    }

    if ( arg1[ 0 ] == '\0'
      && arg2[ 0 ] == '\0'
      && arg3[ 0 ] == '\0'
      && argument[ 0 ] == '\0' )
    {
	send_to_char( "Co chcesz zrobi`c?\n\r", ch );
	send_to_char( "Aby wy`swietli`c list`e blokad napisz: odm`ow <imie_gracza>.\n\r", ch );
	send_to_char( "Aby zablokowa`c polecenie napisz: odm`ow + <imie_gracza> <polecenie> <pow`od blokady>.\n\r", ch );
	send_to_char( "Aby odblokowa`c polecenie napisz: odm`ow - <imie_gracza> <polecenie>.\n\r", ch );
	return;
    }

    /* Gimza: dodanie blokady */
    if ( arg1[ 0 ] == '+'
      && arg2[ 0 ] != '\0'
      && arg3[ 0 ] != '\0'
      && argument[ 0 ] != '\0' )
    {
	if ( !( victim = get_char_world_pc( ch, arg2 ) ) )
	{
	    send_to_char( "Nie widz`e takiego gracza.\n\r", ch );
	    return;
	}

	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Mobom nie mo`zna niczego odm`owi`c.\n\r", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Nie masz prawa tego uczyni`c.\n\r", ch );
	    return;
	}

	if ( refuse_lookup( victim, arg3 ) )
	{
	    send_to_char( "Ten gracz ma ju`z zablokowane to polecenie.\n\r", ch );
	    return;
	}

	for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	    if ( !str_cmp( arg3, cmd_table[ cmd ].name ) )
	    {
		found = TRUE;
		break;
	    }

	if ( !found )
	{
	    send_to_char( "Nie mo`zesz zablokowa`c polecenia, kt`ore nie istnieje.\n\r", ch );
	    return;
	}

	ref1 = new_refuse( );
	strcpy( ref1->refused_cmd, arg3 );
	strcpy( ref1->who, ch->short_descr );
	strcpy( ref1->why, argument );
	strcpy( ref1->date, ascdata( localtime( &current_time ) ) );

	if ( !victim->pcdata->refuse )
	{
	    victim->pcdata->refuse = ref1;
	    ref1->next = NULL;
	    return;
	}

	ref2 = victim->pcdata->refuse;

	while ( ref2->next )
	    ref2 = ref2->next;

	ref1->next = NULL;
	ref2->next = ref1;
	STC( "Zrobione.\n\r", ch );
	return;
    }
    /* Gimza: usuniecie blokady */
    else if ( arg1[ 0 ] == '-' && arg2[ 0 ] != '\0' && arg3[ 0 ] != '\0' )
    {
	if ( !( victim = get_char_world_pc( ch, arg2 ) ) )
	{
	    send_to_char( "Nie widz`e takiego gracza.\n\r", ch );
	    return;
	}

	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Moby nie maj`a blokad.\n\r", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Nie masz prawa tego uczyni`c.\n\r", ch );
	    return;
	}

	if ( !victim->pcdata->refuse )
	{
	    send_to_char( "Ten gracz nie ma `zadnych blokad.\n\r", ch );
	    return;
	}

	if ( ( ref1 = refuse_lookup( victim, arg3 ) ) )
	{
	    del_refuse( victim, ref1 );
	    return;
	}
	else
	{
	    send_to_char( "Ten gracz nie ma takiej blokady.\n\r", ch );
	    return;
	}
	return;
    }
    /* Gimza: lista blokad */
    else if ( arg1[ 0 ] != '\0' && arg2[ 0 ] == '\0' && arg3[ 0 ] == '\0' && argument[ 0 ] == '\0' )
    {
	if ( !( victim = get_char_world_pc( ch, arg1 ) ) )
	{
	    send_to_char( "Nie widz`e takiego gracza.\n\r", ch );
	    return;
	}

	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Nie mo`zesz wy`swietli`c listy blokad moba.\n\r", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Nie masz prawa tego uczyni`c.\n\r", ch );
	    return;
	}

	sprintf( buf, "{gLista blokad dla gracza {w%s{x:\n\r", victim->short_descr );
	send_to_char( buf, ch );

	if ( !victim->pcdata->refuse )
	{
	    send_to_char( "Ten gracz nie ma `zadnych blokad.\n\r", ch );
	    return;
	}

	send_to_char( "{rKomenda        Wystawi`l     Data       Pow`od {x\n\r", ch );

	for ( ref1 = victim->pcdata->refuse ; ref1 ; ref1 = ref1->next )
	{
	    sprintf( buf, "%s%s%s%s\n\r", wyrownaj( ref1->refused_cmd, -15 ), wyrownaj( ref1->who, -13 ), wyrownaj( ref1->date, -11 ),
			ref1->why );

	    send_to_char( buf, ch );
	}
	return;
    }
    else
    {
	send_to_char( "B`l`edna sk`ladnia. Napisz samo ODM`OW, aby dowiedzie`c si`e wi`ecej.\n\r", ch );
	return;
    }

    return;
}


/* lokalna, nigdzie indziej nieuzywana */
int znajdz_powod( char *arg )
{
    int i;

    for ( i = 0; i < MAX_POWODY; i++ )
	if ( powody[ i ].skrot
	  && !str_prefix( arg, powody[ i ].skrot ) )
	{
	    return i;
	}

    return -1;
}


/*
 * Lam
 */
KOMENDA( do_wymus )
{
    char arg[ MIL ];
    int i, j;
    IMIONA_DATA *im;
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "wymu`s" ) )
	return;

    argument = one_argument( argument, arg );

    if ( !*arg )
	strcpy( arg, "pomoc" );

    if ( !str_prefix( arg, "imiona" ) )
    {
	if ( !*argument || !czy_zdatny_plik( argument ) )
	{
	    STC( "Podaj skr`ot imienia.\n\r", ch );
	    return;
	}

	STC( "Imi`e           Blokuj`acy      Pow`od           Czas\n\r", ch );

	j = UPPER( *( argument ) ) & 63;

	for ( i = 0; i <= MAX_DLUG_IMIENIA; i++ )
	    for ( im = imiona[ i ][ j ]; im; im = im->nast )
		if ( !str_prefix( argument, im->imie ) )
		{
		    ch_printf( ch, "%s%s%s %s\n\r",
			wyrownaj( im->imie, -15 ),
			wyrownaj( im->blokujacy, -15 ),
			wyrownaj( powody[ im->powod ].skrot, -15 ),
			asctime_pl_krotko_nonl( localtime( &im->kiedy ) ) );
		}

	return;
    }

    if ( !str_prefix( arg, "powody" ) )
    {
	STC( "Skr`ot          Nazwa\n\r", ch );

	for ( i = 0; i < MAX_POWODY; i++ )
	    if ( powody[ i ].skrot )
	    {
		ch_printf( ch, "%s%s\n\r",
		    wyrownaj( powody[ i ].skrot, -15 ),
		    powody[ i ].nazwa );
	    }

	return;
    }

    if ( !str_prefix( arg, "dodaj" ) )
    {
	argument = one_argument( argument, arg );

	if ( !*arg || !czy_zdatny_plik( arg ) )
	{
	    STC( "Podaj imi`e do zakazania.\n\r", ch );
	    return;
	}

	if ( !*argument
	  || ( i = znajdz_powod( argument ) ) == -1 )
	{
	    STC( "Podaj skr`ot powodu blokady imienia.\n\r", ch );
	    return;
	}

	for ( im = *LISTA_IMION( arg ); im; im = im->nast )
	    if ( !str_cmp( arg, im->imie ) )
	    {
		ch_printf( ch, "Imi`e %s jest ju`z zakazane (i to od %s).\n\r",
			im->imie,
			asctime_pl_krotko_nonl( localtime( &im->kiedy ) ) );
		return;
	    }

	im = nowe_imie( );
	im->imie = str_dup( zwielkoliteruj( arg ) );
	im->powod = i;
	im->blokujacy = str_dup( rch->name ); /* rch */
	im->kiedy = current_time;
	dodaj_imie( im );

	ch_printf( ch, "Dodajesz imi`e %s, pow`od: %s.\n\r", im->imie,
		powody[ im->powod ].nazwa );

	zapisz_imiona( );
	return;
    }

    if ( get_trust( rch ) >= 106 && !str_prefix( arg, "cofnij" ) )
    {
	if ( !*argument || !czy_zdatny_plik( argument ) )
	{
	    STC( "Podaj imi`e do odblokowania.\n\r", ch );
	    return;
	}

	for ( im = *LISTA_IMION( argument ); im; im = im->nast )
	    if ( !str_cmp( argument, im->imie ) )
		break;

	if ( !im )
	{
	    STC( "Takie imi`e nie jest zakazane.\n\r", ch );
	    return;
	}

	ch_printf( ch, "Dopuszczasz imi`e %s, kt`ore by`lo zakazane od %s.\n\r",
		im->imie, asctime_pl_krotko_nonl( localtime( &im->kiedy ) ) );
	zwolnij_imie( im );
	zapisz_imiona( );
	return;
    }

    STC( "Sk`ladnia: wymu`s powody | wymu`s imiona <skr`ot> | wymu`s dodaj <imi`e> <pow`od>\n\r", ch );

    if ( get_trust( rch ) >= 106 )
	STC( "Sk`ladnia: wymu`s cofnij <imi`e>\n\r", ch );

    return;
}


/*
 * Vigud
 */
KOMENDA( do_kogobic )
{
    /* Vigud: mogl byc 1 bufor, ale Lam boi sie goto */
    char arg1[ MIL ], arg2[ MIL ], arg3[ MIL ], arg4[ MIL ];
    char kraina[ MIL ];
    CHAR_DATA *player, *mob;
    int max_results, min_xp, xp, pl_lvl;
    int results = 1;

    if ( !authorized( get_char( ch ), "kogobi`c" ) )
	return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    one_argument( argument, arg4 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
    {
	STC( "Sk`ladnia: kogobi`c <maks. wynik`ow> <gracz> <min. do`sw.> [maks. poziom moba]\n\r", ch );
	return;
    }

    if ( !( player = get_char_world_pc_tele( ch, arg2 ) )
      || IS_NPC( player ) )
    {
	send_to_char( "Nikt taki nie gra obecnie.\n\r", ch );
	return;
    }

    max_results = atoi( arg1 );
    min_xp = atoi( arg3 );
    pl_lvl = arg4[ 0 ] ? atoi( arg4 ) : player->level;

    for ( mob = char_list; mob; mob = mob->next )
	if ( !mob->deleted
	  && ( mob->pIndexData )
	  && ( mob->level <= pl_lvl )
	  && ( xp = xp_compute( player, mob ) )
	  && ( xp >= min_xp ) )
	{
	    if ( strefa_nalezy( STREFA( player ), mob->pIndexData->index_area->strefy ) )
		sprintf( kraina, "{W%s{x", mob->pIndexData->index_area->full_name );
	    else
		strcpy( kraina, mob->pIndexData->index_area->full_name );

	    ch_printf( ch, "{G%3d d`s.{x za {y[%3d]{x %s (#%d) w: %s.\n",
		UMIN( xp, 250 ),
		mob->level,
		mob->biernik,
		mob->pIndexData->vnum,
		kraina );

	    if ( ++results > max_results )
		break;
	}

    return;
}

/* to udowadnia, ze number_percent() daje rowne szanse kazdej liczbie */
void rng_test( CHAR_DATA *ch )
{
    unsigned int wyniki[ 101 ];
    unsigned int long i, min, max;
    struct timeval czasprzed, czaspo;

    for ( i = 0; i < 101; i++ )
	wyniki[ i ] = 0;

    gettimeofday( &czasprzed, NULL );
    for ( i = 0; i < 1000000000; i++ )
	wyniki[ number_percent( ) ]++;
    gettimeofday( &czaspo, NULL );

    max = min = wyniki[ 1 ];
    for ( i = 1; i < 101; i++ )
    {
	if ( wyniki[ i ] > max )
	    max = wyniki[ i ];
	if ( wyniki[ i ] < min )
	    min = wyniki[ i ];
    }
    ch_printf( ch, "max = %lu, min = %lu\n\r", max, min );
    ch_printf( ch, "(max-min)*100/limit = %f\n\r", ( max - min ) / 10000000.0 );
    ch_printf( ch, "Czas: %f\n\r", ( ( czaspo.tv_sec * 1000000 + czaspo.tv_usec )
	- ( czasprzed.tv_sec * 1000000 + czasprzed.tv_usec ) ) / 1000000.0 );

    return;    
}


/*
 * Lam 21.5.98: lubie testowac rozne rzeczy, a nie chce mi sie za kazdym razem
 * pisac nowych polecen.
 */
KOMENDA( do_test )
{
    STC( "Znowu niczego tu nie ma.\n\r", ch );

    return;

#if 0 /* wyszukiwarka przegietych przedmiotow */
    struct slot { OBJ_INDEX_DATA *obj; int value, bit; };
    const int conv[ ] =
    {
	0, /* APPLY_NONE */
	0, /* APPLY_STR */
	0, /* APPLY_DEX */
	0, /* APPLY_INT */
	0, /* APPLY_WIS */
	0, /* APPLY_CON */
	0, /* APPLY_SEX */
	0, /* APPLY_CLASS */
	0, /* APPLY_LEVEL */
	0, /* APPLY_AGE */
	0, /* APPLY_HEIGHT */
	0, /* APPLY_WEIGHT */
	0, /* APPLY_MANA */
	0, /* APPLY_HIT */
	0, /* APPLY_MOVE */
	0, /* APPLY_GOLD */
	0, /* APPLY_EXP */
	1, /* APPLY_AC */
	0, /* APPLY_HITROLL */
	0, /* APPLY_DAMROLL */
	0, /* APPLY_SAVING_PARA */
	0, /* APPLY_SAVING_ROD */
	0, /* APPLY_SAVING_PETRI */
	0, /* APPLY_SAVING_BREATH */
	10, /* APPLY_SAVING_SPELL */
	0, /* APPLY_RACE */
	0 /* APPLY_SIZE */
    };

    unsigned int i, p, lvl;
    struct slot table[ 16 ];
    OBJ_INDEX_DATA *obj;
    AFFECT_DATA *aff;

    lvl = ( *argument ) ? atoi( argument ) : 110;
    for ( i = 0; i < 16; i++ )
    {
	table[ i ].obj = NULL;
	table[ i ].value = 0;
	table[ i ].bit = ( 1 << i );
    }

    for ( i = 0; i < MAX_KEY_HASH; i++ )
	for ( obj = obj_index_hash[ i ]; obj; obj = obj->next )
	{
	    int value = 0;

	    if ( lvl < obj->min_reset_level )
		continue;

	    for ( aff = obj->affected; aff; aff = aff->next )
		value += conv[ aff->location ] * aff->modifier;

	    if ( value > 0 )
		for ( p = 0; p < 16; p++ )
		    if ( obj->wear_flags & table[ p ].bit
		      && table[ p ].value < value )
		    {
			table[ p ].value = value;
			table[ p ].obj = obj;
		    }
	}

    for ( i = 0; i < 16; i++ )
	if ( table[ i ].obj )
	{
	    ch_printf( ch, "[%d] #%d %s = %d\n\r", i, table[ i ].obj->vnum,
		table[ i ].obj->short_descr, table[ i ].value );
	}
#endif

/*
    STC( "rng_test.\n\r", ch );
    rng_test( ch );
 */

/*
    int i;
    MOB_INDEX_DATA *pMobIndex;
    int hash;

    i = 0;
    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
	for ( pMobIndex  = mob_index_hash[ hash ];
	      pMobIndex;
	      pMobIndex  = pMobIndex->next )
	{
	    if ( !IS_AFFECTED( pMobIndex, AFF_MUTE )
	      && !IS_SET( race_table[ pMobIndex->race ].race_abilities, RACE_MUTE ) )
		i++;
	}

    ch_printf( ch, "Ilosc gadaj`acych indeks`ow mob`ow: %d.\n\r", i );
*/

/* to jest sprawdzacz brakujacych tematow pomocy, momentami wyrzuca glupoty,
   wez na to poprawke i uzyj glowy interpretujac jego wyniki :)
    int i;
    HELP_DATA *help;

    for ( i = 0; cmd_table[ i ].name[ 0 ]; i++ )
    {
	for ( help = help_first; help; help = help->next )
	    if ( is_name2( cmd_table[ i ].name, help->keyword ) )
		break;

	if ( !help )
	{
	    ch_printf( ch, "Polecenie '%s' od poziomu %d nie ma pomocy.\n\r",
		cmd_table[ i ].name, cmd_table[ i ].level );
	}
    }

    for ( i = 0; i < MAX_SKILL; i++ )
    {
	for ( help = help_first; help; help = help->next )
	    if ( is_name2( skill_table[ i ].pl_name, help->keyword ) )
		break;

	if ( !help )
	{
	    ch_printf( ch, "Umiej`etno`s`c '%s' nie ma pomocy.\n\r",
		 skill_table[ i ].pl_name );
	    do_slookup( ch, skill_table[ i ].pl_name );
	}
    } */

#if 0 /* bylo if defined( LINK_CHECK ) */
    char bufek[ MSL ];

    for ( help = help_first; help; help = help->next )
    {
	strcpy( bufek, help->text );
	lac2html( bufek );
    }
#endif

/* Przepisujac oblicz_wyrazenie, sprawdzilem, czy oprocz funkcjonalnosci,
   nastapila jakas poprawa w dzialaniu, liczac glupie dlugie wyrazenie
   10000000 (dziesiec milionow) razy. Wyniki:
   stary kod z 1999: 384 s
   nowy kod:         270 s
   dla porownania, nowy kod skompilowany z -O3: 188 s */
/*
    int typ, wart;
    char txt[ MSL ];
    void *wsk;
    int i;
    struct timeval czasprzed, czaspo;

    gettimeofday( &czasprzed, NULL );
    for ( i = 0; i < 10000000; i++ )
	oblicz_wyrazenie( ch, "(2+2)*2+(2*(2/2)/2)+2*2==sex($n)+level($n)", ch, NULL, ch, ch, NULL, &typ, &wart, txt, &wsk );
    gettimeofday( &czaspo, NULL );
    ch_printf( ch, "Czas: %ld\n\r", (long int) czaspo.tv_sec - czasprzed.tv_sec );
*/
/* To sprawdzalo poprawnosc oblicz_wyrazenie, kiedy to pisalem.
   Uwaga, pewne funkcje maja prawo nie dzialac nie z poziomu proga. */
/*
    if ( !oblicz_wyrazenie( ch, argument, ch, NULL, ch, ch, NULL, &typ, &wart, txt, &wsk ) )
	STC( "Wyst`api`l b`l`ad oblicz_wyrazenie( ).\n\r", ch );
    else
	ch_printf( ch, "typ: %d, wart: %d, txt: %s\n\r", typ, wart, txt );
*/

}
