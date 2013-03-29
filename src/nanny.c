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


#include "merc.h"

#include <errno.h>
#if defined( MAM_CRYPT_H ) && !defined( NOCRYPT )
# include <crypt.h>
#endif

#if defined( ultrix )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name,
					bool nowa ) );
bool	poprawne_haslo		args( ( CHAR_DATA *ch, char *arg ) );
void	bledne_haslo		args( ( DESCRIPTOR_DATA *d ) );
void	komunikat_blokady_imienia args( ( CHAR_DATA *ch, int powod ) );
bool	czy_lsc			args( ( DESCRIPTOR_DATA *d, char *argument ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );

/*
 * Lam 28.2.98: sprawdzanie, czy haslo jest wystarczajaco trudne
 */
bool poprawne_haslo( CHAR_DATA *ch, char *arg )
{
    /* haslo jak imie */
    if ( !str_cmp( ch->name, arg ) )
	return FALSE;

    /* czesto uzywane hasla. Wpisz swoje typy :) */
    if ( is_name( arg, "123456 qwerty haslo password letmein dupa dupa.8" ) )
	return FALSE;

    /* mozna dopisac inne testy na sile hasla */

    return TRUE;
}


void bledne_haslo( DESCRIPTOR_DATA *d )
{
    send_to_char( "Has`lo si`e nie zgadza, podejrzewam pr`ob`e w`lamania!\n\r\n\r"
	"Je`sli nie jeste`s w`lamywaczem, sprawd`x, czy:\n\r"
	"* dobrze wpisujesz has`lo - wszystkie literki musz`a si`e zgadza`c,\n\r"
	"* prawid`lowo wpisujesz wielkie i ma`le litery - Lac je rozr`o`znia,\n\r"
	"* w zwi`azku z powy`zszym, czy nie masz aktywnego Caps Locka (tryb du`zych liter),\n\r"
	"* je`sli has`lo zawiera polskie litery, upewnij si`e, `ze masz taki sam standard,\n\r"
	"  kt`ory ma ustawiona posta`c,\n\r"
	"* je`sli has`lo zawiera polskie litery, ale nie mo`zesz ich u`zywa`c na tym\n\r"
	"  komputerze, zamiast alt-literka wpisz `` i literk`e, np. ``z``o``lw, ``xrebi``e.\n\r\n\r"
	"Je`sli jeste`s w`lamywaczem, daj spok`oj tej postaci i za`l`o`z w`lasn`a!\n\r",
	d->character );
    sprintf( log_buf, "Bledne haslo dla postaci %s.", d->character->name );
    log_string( log_buf );
    close_socket( d, TRUE );

    return;
}


/*
 * Lam: funkcja uzywana do sprawdzenia, czy taka postac mogla kiedykolwiek
 * istniec, a konkretnie czy ma prawo istniec taki plik. Chodzi o to, zeby
 * wulgaryzmow nie dopuszczac dla nowych graczy, ale starzy musza byc nadal
 * obslugiwani.
 *
 * 6.11.1999 po wykryciu bledu naruszajacego bezpieczenstwo muda
 */
bool czy_zdatny_plik( char *name )
{
    char *pc;

    if ( strlen_pl( name ) > MAX_DLUG_IMIENIA )
	return FALSE;

    for ( pc = name; *pc != '\0'; pc++ )
	if ( ( !isalpha( (unsigned char) *pc ) && ( *pc != '`' || pc[ 1 ] == '\0' ) )
	  || ( *pc == '`' && pc[ 1 ] == '`' ) )
	{
	    return FALSE;
	}

    return TRUE;
}


/*
 * Lam: ta funkcja zwracala bool, a teraz int:
 * - liczby dodatnie to powody z data/impowod.txt, POWIEKSZONE O JEDEN
 * - liczby ujemne to wbudowane dodatkowe powody
 * - zero to poprawne imie
 */
int check_parse_name( char *name, bool newname )
{
    IMIONA_DATA *im;
    int i;

    /* standardowe ograniczenia dlugosci imienia */
    if ( strlen_pl( name ) < 3
      || strlen_pl( name ) > MAX_DLUG_IMIENIA )
    {
	return -1;
    }

#if defined( MSDOS )
    if ( strlen_pl( name ) > 8 )
	return -1;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha( (unsigned char) *pc ) && ( ( *pc != '`' ) || ( pc[ 1 ] == '\0' ) ) )
		return -1;
	    if ( ( *pc == '`' ) && ( pc[ 1 ] == '`' ) )
		return -1;
	    if ( LOWER( *pc ) != 'i' && LOWER( *pc ) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return -1;
    }

    for ( im = *LISTA_IMION( name ); im; im = im->nast )
	if ( !str_cmp( name, im->imie ) )
	    return im->powod + 1; /* bo powody zaczynaja sie od 0 */

    if ( czy_bluzg1( name ) )
	return 3; /* powod 2 + 1 */

    if ( znajdz_miodka( name ) )
	return 1; /* 0, w miodku za duzo roznych smieci, zeby cos wybrac */

    for ( i = 0; i < MAX_RACE; i++ )
	if ( !str_cmp( name, race_table[ i ].who_name )
	  || !str_cmp( name, race_table[ i ].ang ) )
	{
	    return 2; /* powod 1 + 1 */
	}

    /* nazywanie graczy tak jak moby w sumie powinno byc dopuszczalne, i tak
       uzywamy skrotow... Niejednokrotnie gracze sie myla */
    if ( newname )
    {
	MOB_INDEX_DATA *pMobIndex;
	int             iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	    for ( pMobIndex = mob_index_hash[ iHash ];
		  pMobIndex;
		  pMobIndex = pMobIndex->next )
	    {
		if ( is_name2( name, pMobIndex->player_name ) )
		    return -2;
	    }
    }

    return 0;
}


/*
 * To bierze powod zwrocony przez check_parse_name( )!
 */
void komunikat_blokady_imienia( CHAR_DATA *ch, int powod )
{
    switch ( powod )
    {
	default:
	    powod--; /* numery powodow sa powiekszone o 1 */
	    if ( powod < 0 || powod >= MAX_POWODY )
		bug( "Kwas powodow blokad imion.", 0 );
	    ch_printf( ch, "\n\r%s\n\r%s\n\rObierz inne: ",
		powody[ powod ].objasnienie,
		powody[ powod ].tytul );
	    break;

	case -1:
	    send_to_char( "Twe imi`e nie spe`lnia kryteri`ow i nie mo`zesz go obra`c.\n\rObierz inne: ", ch );
	    break;

	case -2:
	    send_to_char( "To imi`e jest ju`z u`zywane, tote`z nie mo`zesz go obra`c.\n\rObierz inne: ", ch );
	    break;
    }

    return;
}


/*
 * sprawdza, czy zamiast imienia rlacd wyslal Lac Secret Code
 */
bool czy_lsc( DESCRIPTOR_DATA *d, char *argument )
{
    /* LSC nie skonfigurowany */
    if ( !lsc_init_str_len )
	return FALSE;

    return !strncmp( argument, lsc_clear_str, strlen( lsc_clear_str ) );
/* ponizsza wersja powinna dzialac gdyby bylo dopuszczalne stosowanie znakow
   niedrukowalnych w wejsciu. Ale dopuszczalne sa tylko polskie literki,
   a i to nie przy podawaniu imienia. Nie chce zbytnio kombinowac w comm.c,
   wiec nie ma mozliwosci odczytania user@host dzieki rlacd, jesli wlaczone
   jest NO_PTHREADS. To malo istotne. */
/*
    static char buf[ MSL ];
    int i;

    strcpy( buf, argument );
    if ( strncmp( buf, lsc_init_str, lsc_init_str_len ) )
	return FALSE;

    for ( i = lsc_init_str_len; buf[ i ] && buf[ i ] != lsc_delimiter; i++ )
	;
    if ( buf[ i ] == lsc_delimiter )
	buf[ i ] = '\0';
    else
	return FALSE;

    free_string( d->user );
    d->user = str_dup( &buf[ lsc_init_str_len ] );
    free_string( d->host );
    d->host = str_dup( &buf[ i + 1 ] );

    return TRUE;
*/
}


/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *ch;
    NOTE_DATA *pnote;
    ZONE_DATA *strefa;
    char      *pwdnew;
/*  char      *classname; */
    char      *p;
    char       buf [ MAX_STRING_LENGTH ];
    char       strsave[ MAX_STRING_LENGTH ];
    char       helpbuf[ MIL ];
    int        iClass;
    int        iRace;
    int        lines;
    int        notes;
    int        fOld;
    int	       pl_tmp;
    int	       pzl;
#if defined( DODATKOWE_W_NIANI )
    char       buf1[ MAX_STRING_LENGTH ];
    int        sn;
    int        sn_tmp;
#endif
    int        parse_name_tmp;

    DESCRIPTOR_DATA *dold;


    while ( isspace( (unsigned char) *argument ) )
	argument++;

    /* This is here so we wont get warnings.  ch = NULL anyways - Kahn */
    ch          = d->character;

    /* Lam: nie wiedzialem gdzie to wrzucic zeby bylo pewne, wiec jest tu */
    czy_rekord_polaczen( );

    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d, TRUE );
	return;

    case CON_GET_NAME:
	if ( argument[ 0 ] == '\0' )
	{
	    write_to_buffer( d, "Nie to nie!\n\r", 0 );
	    close_socket( d, TRUE );
	    return;
	}

	if ( czy_lsc( d, argument ) )
	    return;

	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    if ( help_namehelp[ 0 ] == '.' )
		write_to_buffer( d, help_namehelp + 1, 0 );
	    else
		write_to_buffer( d, help_namehelp, 0 );

	    write_to_buffer( d, "Podaj swe imie (albo \"nowa\"): ", 0 );
	    return;
	}

	if ( is_name2( argument, "nowa nowy nowe" ) )
	{
	    if ( ch )
		free_char( ch, TRUE );
	    ch = nowa_postac( d, zwielkoliteruj( argument ) );
	    send_to_char( "\n\r", ch );
	    /* Instalator Laca, aplet ze strony Laca */
	    if ( !str_cmp( "xterm-lac-iso", d->term_type )
	      || !str_cmp( "vt320-jta", d->term_type ) )
	    {
		ch->polskie = 2;
		send_to_char( "Proponuje ci uzycie polskich liter zgodnych z Polska Norma. Sprawdz, czy\n\r"
			"wszystkie polskie litery wyswietlaja sie poprawnie.\n\r"
			"Test: P`o`xn`a noc`a jasno`s`c ksi`e`zyca ol`sni`la gra`n.\n\r"
			"P`O`XN`A NOC`A JASNO`S`C KSI`E`ZYCA OL`SNI`LA GRA`N.\n\r"
			"Czy propozycja okazala sie prawidlowa? [T/N]: ", ch );
		d->connected = CON_CONFIRM_POLSKIE;
	    }
	    else if ( !str_cmp( "zmud", d->term_type ) )
	    {
		ch->polskie = 3;
		send_to_char( "Proponuje ci uzycie polskich liter w standardzie Windows. Sprawdz, czy\n\r"
			"wszystkie polskie litery wyswietlaja sie poprawnie.\n\r"
			"Test: P`o`xn`a noc`a jasno`s`c ksi`e`zyca ol`sni`la gra`n.\n\r"
			"P`O`XN`A NOC`A JASNO`S`C KSI`E`ZYCA OL`SNI`LA GRA`N.\n\r"
			"Czy propozycja okazala sie prawidlowa? [T/N]: ", ch );
		d->connected = CON_CONFIRM_POLSKIE;
	    }
	    else if ( !str_prefix( "decafmud-", d->term_type ) )
	    {
		ch->polskie = 4;
		send_to_char( "Proponuje ci uzycie polskich liter w standardzie UTF-8. Sprawdz, czy\n\r"
			"wszystkie polskie litery wyswietlaja sie poprawnie.\n\r"
			"Test: P`o`xn`a noc`a jasno`s`c ksi`e`zyca ol`sni`la gra`n.\n\r"
			"P`O`XN`A NOC`A JASNO`S`C KSI`E`ZYCA OL`SNI`LA GRA`N.\n\r"
			"Czy propozycja okazala sie prawidlowa? [T/N]: ", ch );
		d->connected = CON_CONFIRM_POLSKIE;
	    }
	    else
	    {
		do_help( ch, "polskiexxx" );
		write_to_buffer( d, "\n\rWybierz standard polskich liter [ 0.brak  1.dos  2.iso  3.win  4.utf ]: ", 0 );
		d->connected = CON_GET_POLSKIE;
	    }
	    return;
	}

	if ( !czy_zdatny_plik( argument ) )
	{
	    write_to_buffer( d, "\n\rWatpie w istnienie takiej postaci.\n\r\n\rPodaj imie albo \"nowa\": ", 0 );
	    return;
	}
	argument[ 0 ] = UPPER( argument[ 0 ] );
	if ( argument[ 0 ] == '`' )
	    argument[ 1 ] = UPPER( argument[ 1 ] );

	if ( ch )
	    free_char( ch, TRUE );
	ch   = nowa_postac( d, zwielkoliteruj( argument ) );
	fOld = load_char_obj( d, argument );

	if ( fOld < 0 )
	{
	    send_to_char( "Nie udalo sie wczytac tej postaci!\n\r", ch );
	    close_socket( d, TRUE );
	    return;
	}

	if ( check_reconnect( d, FALSE ) )
	    fOld = TRUE;
	else
	{
	    if ( wizlock && !IS_HERO( ch ) )
	    {
		send_to_char( "Mud jest dost`epny tylko dla bog`ow. Precz, `smiertelniku!\n\r", ch );
		close_socket( d, TRUE );
		return;
	    }
	    if ( ch->level <= numlock && numlock != 0 )
	    {
		send_to_char( "Za niski poziom, `zeby wej`s`c do gry.\n\r\n\r", ch );
		if ( ch->level == 0 )
		{
		    send_to_char( "Zak`ladanie nowych postaci zosta`lo chwilowo zablokowane.\n\r", ch );
		    send_to_char( "Spr`obuj po`l`aczy`c si`e p`o`xniej, aby za`lo`zy`c swoj`a posta`c.\n\r", ch );
		    send_to_char( "Podpisano: Rada Bog`ow Laca.\n\r\n\r", ch );
		}
		close_socket( d, TRUE );
		return;
	    }
	}

	if ( check_playing( d, ch->name, FALSE ) )
	    return;

	if ( fOld )
	{
	    /* Old player */
	    if ( ch->sex == 2 )
		STC( "Oka`z, `ze jeste`s t`a, za kt`or`a si`e podajesz.\n\rHas`lo: ", ch );
	    else
		STC( "Oka`z, `ze jeste`s tym, za kt`orego si`e podajesz.\n\rHas`lo: ", ch );
	    write_to_buffer( d, (char *) echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PWD;
	    return;
	}
	else
	{
	    write_to_buffer( d, "\n\rZeby zalozyc nowa postac, wpisz \"nowa\".\n\r\n\rPodaj imie albo \"nowa\": ", 0 );
	    free_char( ch, TRUE );
	    d->character = NULL;
	    return;
	}
	break;

    case CON_GET_NEW_NAME:
	if ( argument[ 0 ] == '\0' )
	{
	    write_to_buffer( d, "Nie to nie!\n\r", 0 );
	    close_socket( d, TRUE );
	    return;
	}

	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    if ( !ch )
		ch = nowa_postac( d, zwielkoliteruj( argument ) );

	    do_help( ch, "#nimi`e" );
	    STC( "\n\rPodaj swoje nowe imi`e: ", ch );

	    if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) )
	    {
		free_char( ch, TRUE );
		d->character = NULL;
	    }
	    return;
	}

	if ( clan_lookup( argument ) )
	{
	    if ( !ch )
		ch = nowa_postac( d, zwielkoliteruj( argument ) );

	    send_to_char( "To imi`e jest zarezerwowane.\n\rObierz inne: ", ch );

	    if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) )
	    {
		free_char( ch, TRUE );
		d->character = NULL;
	    }
	    return;
	}

	parse_name_tmp = check_parse_name( argument, TRUE );
	if ( parse_name_tmp )
	{
	    if ( !ch )
		ch = nowa_postac( d, zwielkoliteruj( argument ) );
	    komunikat_blokady_imienia( ch, parse_name_tmp );

	    if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) )
	    {
		free_char( ch, TRUE );
		d->character = NULL;
	    }
	    return;
	}
#if !defined( MSDOS )
	if ( check_ban( d->host, d->ipaddr, d->user, 1 )
	  && ( !ch || !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) ) )
	{
	    write_to_buffer( d, "\n\rNie wolno ci zakladac nowych postaci!"
		"\n\rNie masz wstepu na Laca.\n\r\n\r", 0 );
	    close_socket( d, TRUE );
	    return;
	}
#endif

	argument[ 0 ] = UPPER( argument[ 0 ] );
	if ( argument[ 0 ] == '`' )
	    argument[ 1 ] = UPPER( argument[ 1 ] );

	if ( ch && IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) )
	{
	    if ( czy_zajete_imie( argument ) )
	    {
		send_to_char( "Taka posta`c ju`z istnieje!\n\rWybierz inne imi`e: ", ch );
		return;
	    }

	    STC( "\n\r", ch );
	    do_help( ch, "imiona" );
	    ch_printf( ch, "\n\rCzy twoje imi`e jest s`luszne i chcesz si`e zwa`c %s (T/N)? ", zwielkoliteruj( argument ) );
	    /* tu zamieszanie, bo chce zostawic imie w ->name dopoki istnieje
	       taki plik na dysku. */
	    strcpy( helpbuf, ch->name );
	    zmien_imie_postaci( ch, argument );
	    free_string( ch->name );
	    ch->name = str_dup( helpbuf );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}

	if ( czy_zajete_imie( argument ) )
	{
	    if ( !ch )
		ch = nowa_postac( d, zwielkoliteruj( argument ) );
	    send_to_char( "Taka posta`c ju`z istnieje!\n\rWybierz inne imi`e: ", ch );
	    free_char( ch, TRUE );
	    d->character = NULL;
	    return;
	}

	if ( ch )
	    free_char( ch, TRUE );
	ch = nowa_postac( d, zwielkoliteruj( argument ) );
	STC( "\n\r", ch );
	do_help( ch, "imiona" );
	ch_printf( ch, "\n\rCzy twoje imi`e jest s`luszne i chcesz si`e zwa`c %s (T/N)? ", zwielkoliteruj( argument ) );
	d->connected = CON_CONFIRM_NEW_NAME;
	return;

    /* Cadaver 7 XII 2001 */
    /* Lam 13.1.2003: wydawalo sie, ze dziala, ale dzialalo tylko w najprostszym
       przypadku, czyli: ktos sie laczy, jest w grze, podaje haslo, rozlacza
       tamtego i wchodzi. Jesli w miedzyczasie pierwsza postac wyszla/rozlaczyla
       sie/cokolwiek, albo jesli gracz potem podal, ze nie chce tamtej
       rozlaczyc, ujawnial sie blad (w pierwszym przypadku zwis, w drugim
       klonowanie postaci) */
    case CON_GET_OLD_PWD_GRAL:

#if !defined( MSDOS )
	write_to_buffer( d, "\n\r", 2 );
#endif

	write_to_buffer( d, (char *) echo_on_str, 0 );

	/* Lam 13.1.2003: byl czy nie byl, jesli podal zle haslo, nalezy go
	   rozlaczyc! To wszystko po to, zeby nie dalo sie sprawdzic, czy ktos
	   byl w grze, jesli go rozlaczylo. Teraz z petli przeniesione tu. */
	if ( ( strlen( ch->pcdata->pwd ) != 32
	    && strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	  || ( strlen( ch->pcdata->pwd ) == 32
	    && strcmp( lac_sha1_crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) ) )
	{
	    bledne_haslo( d );
	    return;
	}

	for ( dold = descriptor_list; dold; dold = dold->next )
	{
	    if ( dold != d
	      && dold->character
	      && dold->connected != CON_GET_NAME
	      && !str_cmp( ch->name, dold->original
			  ? dold->original->name : dold->character->name )
	      && !dold->character->deleted )
	    {
		/* dold->original z lenistwa nie ogladam, kogos z opetaniem
		   nikt nie wyrzuci */
		if ( !IS_NPC( dold->character )
		  && IS_SET( dold->character->act, PLR_DENY )
		  && !sprawdz_deny( dold->character ) )
		{
		    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
		    log_string( log_buf );
		    sprintf( log_buf, "{M$N@%s{x - WYRZUCONY gracz usi`luje wej`s`c!", d->host );
		    wiznet( log_buf, ch, NULL, NULL, WIZ_LOGIN, 0, 0 );
		    send_to_char( "Has`lo si`e zgadza, ale twojej postaci zabroniono dost`epu do gry. `Zegnaj!\n\r", ch );
		    close_socket( d, TRUE );
		    /* zakladam, ze wywalenie dold jest niemozliwe, najlepiej
		      wrzucic probe wyrzucenia gdzies w plynne_odradzanie :) */
		    return;
		}

		STC( "Ju`z grasz.\n\rCzy mimo to pragniesz si`e po`l`aczy`c? [T/N]: ", ch );
		d->connected = CON_JUZ_GRAL_PWD_OK;
		return;
	    }
	}
	/* Lam 13.1.2003: tu nieobslugiwany przez Cadavera przypadek: brak
	   postaci podczas proby jej rozlaczenia. Poniewaz tamta postac byla
	   w uzyciu gdy ta byla ladowana z dysku, nie mozemy wejsc do gry ta
	   (moze to byc wersja o 2 poziomy nizsza albo o 5 smierci mniej
	   poobijana). Nalezaloby sprawdzic, czy poprzednia postac stracila
	   polaczenie (check_reconnect), a jesli wyszla zupelnie, wywalic te
	   i zaladowac jeszcze raz z dysku. Ale poniewaz to zbyt rzadka
	   sytuacja, bede jednak leniwy: */
	STC( "Lac straci`l rachub`e twoich po`l`acze`n. Spr`obuj po`l`aczy`c si`e jeszcze raz (i tylko jednym po`l`aczeniem).\n\r", ch );
	close_socket( d, TRUE );
	break;

    case CON_JUZ_GRAL_PWD_OK:

	switch ( *argument )
	{
	case 't': case 'T':
#if !defined( MSDOS )
	    write_to_buffer( d, "\n\r", 2 );
#endif

	    for ( dold = descriptor_list; dold; dold = dold->next )
		if ( dold != d
		    && dold->character
		    && dold->connected != CON_GET_NAME
		    && !str_cmp( ch->name, dold->original
				? dold->original->name : dold->character->name )
		    && !dold->character->deleted )
		{
		    ASTC( "W`la`snie postanowi`l$o`s po`l`aczy`c si`e ponownie sk`adin`ad, wi`ec ci`e wywalam!\n\r", dold->character );
		    close_socket( dold, TRUE );
		    send_to_char( "U`smierci`lem twoje poprzednie po`l`aczenie. ", ch );
		    if ( check_reconnect( d, TRUE ) )
			return;

		    ASTC( "\n\rOkazuje si`e, `ze ponowne po`l`aczenie nie jest mo`zliwe. Najprawdopodobniej\n\rprzed utrat`a po`l`aczenia nie zd`a`zy`l$o`s zacz`a`c gra`c. Teraz oba po`l`aczenia\n\rzosta`ly zamkni`ete, po`l`acz si`e jeszcze raz, a wszystko powr`oci do normy.\n\r", ch );
		    close_socket( d, TRUE );
		    return;
		}

	    send_to_char( "Nie potrafi`e po`l`aczy`c ci`e powt`ornie, chyba ci`e ju`z nie ma, twoja poprzednia\n\rsesja umar`la ze staro`sci, albo co`s w tym stylu. Spr`obuj po`l`aczy`c si`e jeszcze\n\rraz, powinno by`c wszystko w porz`adku.\n\r", ch );
	    close_socket( d, TRUE );
	    return;

	case 'n': case 'N':
	    send_to_char( "Podaj inne imi`e: ", ch );
	    free_char( ch, TRUE );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    send_to_char( "Potwierd`x albo zaprzecz [T/N]: ", ch );
	    break;
	}
	break;


    case CON_GET_OLD_PWD:
#if !defined( MSDOS )
	write_to_buffer( d, "\n\r", 2 );
#endif

	pzl = strlen( ch->pcdata->pwd );
	if ( ( pzl != 32
	    && strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	  || ( pzl == 32
	    && strcmp( lac_sha1_crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) ) )
	{
	    bledne_haslo( d );
	    return;
	}

	if ( IS_SET( ch->act, PLR_DENY ) && !sprawdz_deny( ch ) )
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
	    log_string( log_buf );
	    sprintf( log_buf, "{M$N@%s{x - WYRZUCONY gracz usi`luje wej`s`c!", d->host );
	    wiznet( log_buf, ch, NULL, NULL, WIZ_LOGIN, 0, 0 );
	    send_to_char( "Has`lo si`e zgadza, ale twojej postaci zabroniono dost`epu do gry. `Zegnaj!\n\r", ch );
	    close_socket( d, TRUE );
	    return;
	}

	/* Konwerter hasel, ustawia haslo na podane przez gracza, ale moze je
	   zepsuc, jesli gracz mial haslo dluzsze niz 8 znakow, a crypt z DES
	   nie zauwazyl, ze gracz popelnil literowke w znaku dalszym niz 8
	   (bardzo malo prawdopodobne, ze gracz ma dlugie haslo, a na dodatek
	   myli sie akurat, kiedy nastepuje konwersja, ale moze sie zdarzyc).
	   Dopisek - zdarzylo sie oczywiscie Valdrabowi! :)
	   Haslo z 0 znakow zostawiam - wtedy wchodzi do gry bez hasla, dostaje
	   komunikat i musi uzyc polecenia "haslo" z potwierdzeniem hasla. */
	if ( pzl != 32 && pzl != 0 )
	{
	    free_string( ch->pcdata->pwd );
	    ch->pcdata->pwd = str_dup( lac_sha1_crypt( argument, ch->name ) );
	}

	write_to_buffer( d, (char *) echo_on_str, 0 );

	if ( check_reconnect( d, TRUE ) )
	    return;

	/* Lam 5.3.2007 */
	parse_name_tmp = check_parse_name( ch->name, FALSE );
	if ( parse_name_tmp )
	{
	    SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE );
	    sprintf( log_buf, "Do gry probuje wejsc %s, wymuszam zmiane imienia.",
		ch->name );
	    log_string( log_buf );
	}

	if ( !ch->pcdata->strefa )
	{
	    strcpy( buf, "\n\rNie masz okre`slonego miejsca zamieszkania. Musisz je teraz wybra`c.\n\r\n\rWybierz krain`e [" );
	    pzl = 0;
	    for ( strefa = zone_first; strefa; strefa = strefa->next )
		if ( is_name2( race_table[ ch->race ].name, strefa->rasy )
		  || is_name2( race_table[ ch->race ].ang, strefa->rasy ) )
		{
		    if ( pzl > 0 )
			strcat( buf, " " );
		    else
			ch->pcdata->strefa = strefa;
		    strcat( buf, strefa->nazwa );
		    pzl++;
		}

	    strcat( buf, "]: " );
	    if ( !pzl )
		ch->pcdata->strefa = zone_first;
	    if ( pzl < 2 )
	    {
		sprintf( buf, "Jedyn`a dost`epn`a dla ciebie krain`a jest %s.\n\r\n\r",
		    ch->pcdata->strefa->nazwa );
		STC( buf, ch );
		sprintf( helpbuf, "#strefa_%s", ch->pcdata->strefa->nazwa );
		do_help( ch, helpbuf );
	    }
	    else
	    {
		STC( buf, ch );
		d->connected = CON_GET_NEW_STREFA;
		break;
	    }
	}

	if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) )
	{
	    komunikat_blokady_imienia( ch, parse_name_tmp );
	    d->connected = CON_GET_NEW_NAME;
	    break;
	}

	if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WOLACZ ) )
	{
	    REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_WOLACZ );
	    STC( "W zwi`azku z wprowadzeniem obowi`azku posiadania wo`lacza, podaj go teraz.\n\r", ch );
	    ch->odmiana = 6;
	    d->connected = CON_GET_WOLACZ;
	    odmiana_prompt( ch );
	    break;
	}

	lines = ch->pcdata->pagelen;
	ch->pcdata->pagelen = 20;
	if ( IS_HERO( ch ) )
	    do_help( ch, "nog`loszenie" );
	do_help( ch, "og`loszenie" );
	ch->pcdata->pagelen = lines;
	d->connected = CON_READ_MOTD;
	break;

    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 't': case 'T':
	    if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) )
		send_to_char( "\n\rA zatem nowa dusza przybywa do naszego `swiata. Imi`e, kt`ore jest ci tak\n\r"
		    "drogie, pozostanie dla nas `swi`ete i stanie si`e muzyk`a dla naszych uszu.\n\r", ch );

	    send_to_char( "\n\rOdmie`n teraz swoje imi`e. Je`sli jako przypadki nie podasz przypadk`ow\n\r", ch );
	    send_to_char( "twojego imienia, twoja posta`c mo`ze zosta`c usuni`eta bez ostrze`zenia!\n\r\n\r", ch );

	    ch->odmiana = 1;
	    d->connected = CON_GET_ODMIANA;
	    /* przy wymuszeniu postaci, ktora nie miala wolacza, i tak o niego
	       spytamy */
	    REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_WOLACZ );
	    odmiana_prompt( ch );
	    break;

	case 'n': case 'N':
	    send_to_char( "Jednak nie chcesz si`e tak zwa`c.\n\rPodaj lepsze imi`e: ", ch );
	    if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) )
	    {
		free_char( d->character, TRUE );
		d->character = NULL;
	    }
	    d->connected = CON_GET_NEW_NAME;
	    break;

	default:
	    send_to_char( "Potwierd`x albo zaprzecz [T/N]: ", ch );
	    break;
	}
	break;


    case CON_GET_ODMIANA:
	odmiana_postaci( ch, argument );
	if ( !ch->odmiana )
	{
	    send_to_char( "\n\rCzy podana przez ciebie odmiana jest poprawna? [T/N]: ", ch );
	    d->connected = CON_CONFIRM_ODMIANA;
	    return;
	}
	odmiana_prompt( ch );
	break;

    case CON_CONFIRM_ODMIANA:
	switch ( *argument )
	{
	case 't': case 'T':
	    if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) )
	    {
		/* po to trzymalem w ->name stare imie */
#if !defined( macintosh ) && !defined( MSDOS )
		sprintf( strsave, PLAYER_DIR "%s" DIR_SEPARATOR "%s",
		    initial( ch->name ), capitalize( ch->name ) );
#else
		sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
#endif
		zmien_wlasciciela_wszedzie( ch, ch->short_descr );
		sprintf( log_buf, "Wymuszona zmiana imienia: %s -> %s",
			ch->name, ch->short_descr );
		log_string( log_buf );
		/* i juz nie jest potrzebne */
		free_string( ch->name );
		ch->name = str_dup( ch->short_descr );

		REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE );
		save_char_obj( ch );
		unlink( strsave );

		lines = ch->pcdata->pagelen;
		ch->pcdata->pagelen = 20;
		if ( IS_HERO( ch ) )
		    do_help( ch, "nog`loszenie" );
		do_help( ch, "og`loszenie" );
		ch->pcdata->pagelen = lines;
		d->connected = CON_READ_MOTD;
		return;
	    }
	    do_help( ch, "#nhas`lo" );
	    sprintf( buf, "\n\rPodaj mi has`lo dla %s: %s",
		    ch->dopelniacz, (char *) echo_off_str );
	    send_to_char( buf, ch );
	    d->connected = CON_GET_NEW_PWD;
	    break;

	case 'n': case 'N':
	    send_to_char( "Spr`obuj odmieni`c si`e ponownie.\n\r", ch );
	    d->character->odmiana = 1;
	    d->connected = CON_GET_ODMIANA;
	    odmiana_prompt( ch );
	    break;

	default:
	    send_to_char( "Potwierd`x albo zaprzecz [T/N]: ", ch );
	    break;
	}
	break;


    case CON_GET_NEW_PWD:
#if !defined( MSDOS )
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#has`lo" );
	    send_to_char( "\n\rPodaj has`lo: ", ch );
	    return;
	}

	if ( strlen( argument ) < 5 )
	{
	    send_to_char(
	       "Has`lo musi mie`c przynajmniej pi`e`c znak`ow d`lugo`sci.\n\rHas`lo: ",
		ch );
	    return;
	}

	if ( !poprawne_haslo( ch, argument ) )
	{
	    send_to_char(
		"Nowe has`lo jest zbyt `latwe do odgani`ecia, spr`obuj innego.\n\rHas`lo: ",
		ch );
	    return;
	}

	pwdnew = lac_sha1_crypt( argument, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	    if ( *p == '~' )
	    {
		send_to_char(
		    "Nowe has`lo jest raczej niedopuszczalne, spr`obuj innego.\n\rHas`lo: ",
		    ch );
		return;
	    }

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd = str_dup( pwdnew );
	send_to_char( "Powt`orz prosz`e has`lo: ", ch );
	d->connected = CON_CONFIRM_NEW_PWD;
	break;

    case CON_CONFIRM_NEW_PWD:
#if !defined( MSDOS )
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( lac_sha1_crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    send_to_char( "Has`la si`e nie zgadzaj`a!\n\rWpisz has`lo od nowa: ", ch );
	    d->connected = CON_GET_NEW_PWD;
	    return;
	}

	write_to_buffer( d, (char *) echo_on_str, 0 );

	do_help( ch, "#adres" );
	send_to_char( "\n\rPodaj zatem sw`oj adres poczty elektronicznej, lub wpisz NIE: ", ch );
	d->connected = CON_GET_EMAIL;
	break;

    case CON_GET_EMAIL:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#adres" );
	    send_to_char( "\n\rPodaj zatem sw`oj adres poczty elektronicznej, lub wpisz NIE: ", ch );

	    return;
	}

	if ( !*argument
	  || ( !czy_mail( argument ) && str_cmp( argument, "nie" ) ) )
	{
	    send_to_char( "\n\rPodaj sw`oj adres poczty elektronicznej, lub wpisz NIE: ", ch );
	    return;
	}

	if ( czy_mail( argument ) )
	{
	    free_string( ch->pcdata->email );
	    ch->pcdata->email = str_dup( argument );
	    ch_printf( ch, "\n\rTw`oj adres poczty elektronicznej to: %s", ch->pcdata->email );
	    send_to_char( "\n\rCzy podany przez ciebie adres jest poprawny? [T/N] ", ch );
	}

	if ( !str_cmp( argument, "nie" ) )
	{
	    free_string( ch->pcdata->email );
	    ch->pcdata->email = str_dup( "" );
	    send_to_char( "\n\rCzy na pewno nie chcesz podawa`c swojego adresu? [T/N] ", ch );
	}

	d->connected = CON_CONFIRM_EMAIL;
	break;

    case CON_CONFIRM_EMAIL:
	switch ( *argument )
	{
	case 't': case 'T':
	    do_help( ch, "#p`le`c" );
	    send_to_char( "\n\rJaka jest twoja p`le`c [M`eska/`Ze`nska]? ", ch );
	    d->connected = CON_GET_NEW_SEX;
	    break;

	case 'n': case 'N':
	    send_to_char( "\n\rPodaj zatem sw`oj adres poczty elektronicznej, lub wpisz NIE: ", ch );
	    d->connected = CON_GET_EMAIL;
	    break;

	default:
	    send_to_char( "\n\rPotwierd`x albo zaprzecz [T/N]: ", ch );
	    break;
	}
	break;

    case CON_GET_NEW_SEX:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#p`le`c" );
	    send_to_char( "\n\rJaka jest twoja p`le`c [M`eska/`Ze`nska]? ", ch );
	    return;
	}

	if ( *argument == '`' )
	    argument++;
	switch ( *argument )
	{
	    case 'm': case 'M': ch->sex = SEX_MALE;    break;
	    /* case 'f': case 'F': */
	    case 'z': case 'Z': ch->sex = SEX_FEMALE;  break;
	    /* case 'n': case 'N': ch->sex = SEX_NEUTRAL; break; */
	    default:
		send_to_char( "To nie jest p`le`c.\n\rJakiej p`lci jeste`s [M`eska/`Ze`nska]? ", ch );
		return;
	}

	do_help( ch, "#rasy_laca" );
	strcpy( buf, "\n\rWybierz ras`e:\n\r[" );
	for ( iRace = 0, pzl = 0; iRace < MAX_RACE; iRace++ )
	{
	    if ( !IS_SET( race_table[ iRace ].race_abilities, RACE_PC_AVAIL ) )
		continue;
	    if ( pzl > 0 )
		strcat( buf, " " );
	    strcat( buf, race_table[ iRace ].who_name );
	    pzl++;
	}
	strcat( buf, "]\n\r: " );
	send_to_char( buf, ch );
	d->connected = CON_GET_NEW_RACE;
	break;

    case CON_GET_NEW_RACE:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#rasa" );
	    strcpy( buf, "\n\rWybierz ras`e:\n\r[" );
	    for ( iRace = 0, pzl = 0; iRace < MAX_RACE; iRace++ )
	    {
		if ( !IS_SET( race_table[ iRace ].race_abilities, RACE_PC_AVAIL )
		  && ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_VAMPIRE )
		    || iRace != zr_wampir ) )
		    continue;
		if ( pzl > 0 )
		    strcat( buf, " " );
		strcat( buf, race_table[ iRace ].who_name );
		pzl++;
	    }
	    strcat( buf, "]\n\r: " );
	    send_to_char( buf, ch );
	    return;
	}

	for ( iRace = 0; iRace < MAX_RACE; iRace++ )
	    if ( ( !str_prefix( argument, race_table[ iRace ].name )
		|| !str_prefix( argument, race_table[ iRace ].ang ) )
	      && ( IS_SET( race_table[ iRace ].race_abilities, RACE_PC_AVAIL )
		|| ( iRace == zr_wampir
		  && IS_SET( ch->pcdata->temp_flags, TEMPFLAG_VAMPIRE ) ) ) )
	    {
		ch->race = iRace;
		break;
	    }

	if ( iRace == MAX_RACE )
	{
	    send_to_char( "To nie rasa.\n\rJakiej rasy jeste`s? ", ch );
	    return;
	}

	write_to_buffer( d, "\n\r", 0 );
	sprintf( helpbuf, "#rasa_%s", race_table[ ch->race ].who_name );
	do_help( ch, helpbuf ); /* zmiana z name */
	if ( ch->sex == 2 )
	    send_to_char( "\n\rJeste`s ca`lkowicie pewna swej decyzji? ", ch );
	else
	    send_to_char( "\n\rJeste`s ca`lkowicie pewien swej decyzji? ", ch );
	d->connected = CON_CONFIRM_NEW_RACE;
	break;

    case CON_CONFIRM_NEW_RACE:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#prasa" );
	    if ( ch->sex == 2 )
		send_to_char( "\n\rJeste`s ca`lkowicie pewna swej decyzji? ", ch );
	    else
		send_to_char( "\n\rJeste`s ca`lkowicie pewien swej decyzji? ", ch );
	    return;
	}

	switch ( argument[ 0 ] )
	{
	  case 't': case 'T':
	      break;
	  default:
	      do_help( ch, "#rasy_laca" );
	      strcpy( buf, "\n\rWybierz ras`e:\n\r[" );
	      for ( iRace = 0, pzl = 0; iRace < MAX_RACE; iRace++ )
	      {
		  if ( !IS_SET( race_table[ iRace ].race_abilities, RACE_PC_AVAIL )
		    && ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_VAMPIRE )
		      || iRace != zr_wampir ) )
		      continue;
		  if ( pzl > 0 )
		      strcat( buf, " " );
		  strcat( buf, race_table[ iRace ].who_name );
		  pzl++;
	      }
	      strcat( buf, "]\n\r: " );
	      send_to_char( buf, ch );
	      d->connected = CON_GET_NEW_RACE;
	      return;
	}

	strcpy( buf, "\n\rWybierz krain`e [" );
	pzl = 0;
	for ( strefa = zone_first; strefa; strefa = strefa->next )
	    if ( is_name2( race_table[ ch->race ].name, strefa->rasy )
	      || is_name2( race_table[ ch->race ].ang, strefa->rasy ) )
	    {
		if ( pzl > 0 )
		    strcat( buf, " " );
		else
		    ch->pcdata->strefa = strefa;
		strcat( buf, strefa->nazwa );
		pzl++;
	    }

	strcat( buf, "]: " );
	if ( !pzl )
	    ch->pcdata->strefa = zone_first;
	if ( pzl < 2 )
	{
	    sprintf( buf, "Jedyn`a dost`epn`a dla ciebie krain`a jest %s.\n\r\n\r",
		ch->pcdata->strefa->nazwa );
	    STC( buf, ch );
	    sprintf( helpbuf, "#strefa_%s", ch->pcdata->strefa->nazwa );
	    do_help( ch, helpbuf );
	    sprintf( buf, "\n\rCzy na pewno chcesz zosta`c mieszka%s %s? ",
		ch->sex == 2 ? "nk`a" : "`ncem",
		ch->pcdata->strefa->dopelniacz );
	    STC( buf, ch );
	    d->connected = CON_CONFIRM_STREFA;
	    break;
	}
	STC( buf, ch );
	d->connected = CON_GET_NEW_STREFA;
	break;

    case CON_GET_NEW_STREFA:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    send_to_char( "\n\rPo wybraniu krainy zobaczysz jej opis.\n\r", ch );
	    send_to_char( "\n\rWybierz krain`e: ", ch );
	    return;
	}

	for ( strefa = zone_first; strefa; strefa = strefa->next )
	    if ( !str_prefix( argument, strefa->nazwa )
	      && ( is_name2( race_table[ ch->race ].name, strefa->rasy )
		|| is_name2( race_table[ ch->race ].ang, strefa->rasy ) ) )
		break;

	if ( !strefa )
	{
	    send_to_char( "Nie mo`zesz wybra`c takiej krainy. Jak`a krain`e wybierasz? ", ch );
	    return;
	}

	ch->pcdata->strefa = strefa;
	write_to_buffer( d, "\n\r", 0 );
	sprintf( helpbuf, "#strefa_%s", ch->pcdata->strefa->nazwa );
	do_help( ch, helpbuf );
	sprintf( buf, "\n\rCzy na pewno chcesz zosta`c mieszka%s %s? ",
		ch->sex == 2 ? "nk`a" : "`ncem",
		ch->pcdata->strefa->dopelniacz );
	STC( buf, ch );
	d->connected = CON_CONFIRM_STREFA;
	break;

    /* Lam: wraca do wyboru rasy w przypadku "n" */
    case CON_CONFIRM_STREFA:
	switch ( argument[ 0 ] )
	{
	    case 't': case 'T':
		break;
	    default:
		if ( ch->level )
		{
		    strcpy( buf, "\n\rWybierz krain`e [" );
		    pzl = 0;
		    for ( strefa = zone_first; strefa; strefa = strefa->next )
			if ( is_name2( race_table[ ch->race ].name, strefa->rasy )
			  || is_name2( race_table[ ch->race ].ang, strefa->rasy ) )
			{
			    if ( pzl > 0 )
				strcat( buf, " " );
			    else
				ch->pcdata->strefa = strefa;
			    strcat( buf, strefa->nazwa );
			    pzl++;
			}

		    strcat( buf, "]: " );
		    STC( buf, ch );
		    d->connected = CON_GET_NEW_STREFA;
		    return;
		}

		do_help( ch, "#rasy_laca" );
		strcpy( buf, "\n\rWybierz ras`e:\n\r[" );
		for ( iRace = 0, pzl = 0; iRace < MAX_RACE; iRace++ )
		{
		    if ( !IS_SET( race_table[ iRace ].race_abilities, RACE_PC_AVAIL )
		      && ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_VAMPIRE )
			|| iRace != zr_wampir ) )
			continue;
		    if ( pzl > 0 )
			strcat( buf, " " );
		    strcat( buf, race_table[ iRace ].who_name );
		    pzl++;
		}
		strcat( buf, "]\n\r: " );
		send_to_char( buf, ch );
		d->connected = CON_GET_NEW_RACE;
		return;
	}

	if ( ch->level )
	{
	    if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WYMUSZENIE ) )
	    {
		parse_name_tmp = check_parse_name( argument, FALSE );
		if ( parse_name_tmp )
		    komunikat_blokady_imienia( ch, parse_name_tmp );
		else
		{
		    cbug( "zepsute wymuszenie", 0 );
		    STC( "Podaj nowe imi`e: ", ch );
		}
		d->connected = CON_GET_NEW_NAME;
		break;
	    }

	    if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_WOLACZ ) )
	    {
		REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_WOLACZ );
		STC( "W zwi`azku z wprowadzeniem obowi`azku posiadania wo`lacza, podaj go teraz.\n\r", ch );
		ch->odmiana = 6;
		d->connected = CON_GET_WOLACZ;
		odmiana_prompt( ch );
		break;
	    }

	    lines = ch->pcdata->pagelen;
	    ch->pcdata->pagelen = 20;
	    if ( IS_HERO( ch ) )
		do_help( ch, "nog`loszenie" );
	    do_help( ch, "og`loszenie" );
	    ch->pcdata->pagelen = lines;
	    d->connected = CON_READ_MOTD;
	    break;
	}

	strcpy( buf, "\n\rWybierz profesj`e [" );
	pzl = 0;
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	    if ( is_name2( race_table[ ch->race ].name, class_table[ iClass ].races )
	      || is_name2( race_table[ ch->race ].ang, class_table[ iClass ].races ) )
	    {
		if ( pzl > 0 )
		    strcat( buf, " " );
		strcat( buf, class_table[ iClass ].long_name );
		pzl++;
	    }

	strcat( buf, "]: " );
	send_to_char( buf, ch );
	d->connected = CON_GET_NEW_CLASS;
	break;

    case CON_GET_NEW_CLASS:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#prof" );

	    strcpy( buf, "\n\rWybierz profesj`e [" );
	    pzl = 0;
	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		if ( is_name2( race_table[ ch->race ].name, class_table[ iClass ].races )
		  || is_name2( race_table[ ch->race ].ang, class_table[ iClass ].races ) )
		{
		    if ( pzl > 0 )
			strcat( buf, " " );
		    strcat( buf, class_table[ iClass ].long_name );
		    pzl++;
		}

	    strcat( buf, "]: " );
	    send_to_char( buf, ch );

	    return;
	}

	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	    if ( !str_prefix( argument, class_table[ iClass ].long_name )
	      && ( is_name2( race_table[ ch->race ].name, class_table[ iClass ].races )
	        || is_name2( race_table[ ch->race ].ang, class_table[ iClass ].races ) ) )
	    {
		ch->klasa = iClass;
		break;
	    }


	if ( iClass == MAX_CLASS )
	{
	    send_to_char(
		"Nie mo`zesz wybra`c takiej profesji.\n\rJaka jest naprawd`e twoja profesja? ", ch );
	    return;
	}

	write_to_buffer( d, "\n\r", 0 );

/*	WYLACZONE (przez Lama):
	switch ( ch->klasa )
	{
	    default: classname = "";			break;
	    case 0:  classname = "Mag";			break;
	    case 1:  classname = "Kleryk";		break;
	    case 2:  classname = "Z`lodziej";		break;
	    case 3:  classname = "Wojownik";		break;
	    case 4:  classname = "Psionik";		break;
	    case 5:  classname = "Szpieg";		break;
	    case 6:  classname = "Szaman";		break;
	    case 7:  classname = "Skrytob`ojca";	break;
	    case 8:  classname = "Mnich";		break;
	}
	if ( classname != "" )
	    do_help( ch, classname );
	else
	    bug( "Nanny CON_GET_NEW_CLASS:  ch->klasa (%d) not valid",
		ch->klasa ); */

	/* Lam 4.12.2000 */
	sprintf( helpbuf, "#prof_%s", class_table[ ch->klasa ].who_name );
	do_help( ch, helpbuf );

	if ( ch->sex == 2 )
	    send_to_char( "\n\rCzy jeste`s `swi`ecie przekonana o s`luszno`sci swego wyboru? ", ch );
	else
	    send_to_char( "\n\rCzy jeste`s `swi`ecie przekonany o s`luszno`sci swego wyboru? ", ch );
	d->connected = CON_CONFIRM_CLASS;
	break;

    case CON_CONFIRM_CLASS:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#pprof" );
	    if ( ch->sex == 2 )
		send_to_char( "\n\rCzy jeste`s `swi`ecie przekonana o s`luszno`sci swego wyboru? ", ch );
	    else
		send_to_char( "\n\rCzy jeste`s `swi`ecie przekonany o s`luszno`sci swego wyboru? ", ch );
	    return;
	}

	switch ( argument[ 0 ] )
	{
	  case 't': case 'T':
	      break;
	  default:
	      strcpy( buf, "\n\rWybierz profesj`e [" );
	      pzl = 0;
	      for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		  if ( is_name2( race_table[ ch->race ].name, class_table[ iClass ].races )
		    || is_name2( race_table[ ch->race ].ang, class_table[ iClass ].races ) )
		  {
		      if ( pzl > 0 )
			  strcat( buf, " " );
		      strcat( buf, class_table[ iClass ].long_name );
		      pzl++;
		  }

	      strcat( buf, "]: " );
	      send_to_char( buf, ch );
	      d->connected = CON_GET_NEW_CLASS;
	      return;
	}

#if defined( DODATKOWE_W_NIANI )
/* wyswietlenie mozliwych umiejetnosci */
	strcpy( buf, "\n\rOpr`ocz umiej`etno`sci standardowo dost`epnych twojej klasie, mo`zesz dodatkowo\n\r"
		"wybra`c 5, kt`ore pomog`a ci w grze. Wybierz je spo`sr`od poni`zszych:\n\r" );
	pzl = 0;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	    if ( skill_table[ sn ].multi_level[ ch->klasa ] < L_APP )
	    {
		if ( pzl % 4 == 0 )
		    strcat( buf, "\n\r" );
		sprintf( buf1, "%s", wyrownaj( skill_table[ sn ].pl_name, 19 ) );
		strcat( buf, buf1 );
		pzl++;
	    }

	strcat( buf, "\n\r\n\rJe`sli nie wiesz co wybra`c, naci`snij Enter, a je`sli wiesz, wpisz nazw`e\n\rpierwszej z `z`adanych umiej`etno`sci: " );
	send_to_char( buf, ch );

	d->connected = CON_GET_MULTI;
	break;

    case CON_GET_MULTI:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#umiej" );
	    send_to_char( "\n\rWybierz umiej`etno`s`c: ", ch );
	    return;
	}

	if ( argument[ 0 ] == '\0' )
	{
	    if ( ch->pcdata->multi[ 0 ] < 1 )
		send_to_char( "\n\rCzy na pewno nie chcesz wybra`c sobie `zadnych dodatkowych umiej`etno`sci? ", ch );
	    else
	    {
		if ( ch->sex == 2 )
		    send_to_char( "\n\rCzy wybra`la`s ju`z te umiej`etno`sci, kt`ore chcesz? ", ch );
		else
		    send_to_char( "\n\rCzy wybra`le`s ju`z te umiej`etno`sci, kt`ore chcesz? ", ch );
	    }
	    d->connected = CON_CONFIRM_MULTI;
	}
	else
	{
	    sn = skill_lookup_pl( argument );
	    if ( ( sn < 0 ) || ( skill_table[ sn ].multi_level[ ch->klasa ] > L_APP ) )
	    {
		send_to_char( "Wybieraj z listy, albo Enter: ", ch );
		return; /* d->connected i tak jest w porzadku */
	    }
	    /* wybor skillu */
	    sprintf( buf, "Wybra`l$o`s umiej`etno`s`c \"%s\".\n\r", skill_table[ sn ].pl_name );
	    ASTC( buf, ch );
	    sn_tmp = sn;

/* wyswietlenie mozliwych umiejetnosci */
	    strcpy( buf, "\n\rDo wyboru masz:\n\r" );
	    pzl = 0;
	    for ( sn = 0; sn < MAX_SKILL; sn++ )
		if ( skill_table[ sn ].multi_level[ ch->klasa ] < L_APP )
		{
		    if ( pzl % 4 == 0 )
			strcat( buf, "\n\r" );
		    sprintf( buf1, "%s", wyrownaj( skill_table[ sn ].pl_name, 19 ) );
		    strcat( buf, buf1 );
		    pzl++;
		}

	    send_to_char( buf, ch );

	    for ( sn = 0; sn < 5; sn++ )
		if ( ch->pcdata->multi[ sn ] == sn_tmp )
		    break; /* juz byl wybrany */
		else if ( ch->pcdata->multi[ sn ] < 1 )
		{
		    ch->pcdata->multi[ sn ] = sn_tmp;
		    break;
		}

	    pzl = 1; /* pzl bedzie mi liczyc, ktory teraz trzeba wybrac */
	    if ( ch->sex == 2 )
		strcpy( buf, "\n\r\n\rWybra`la`s ju`z:\n\r" );
	    else
		strcpy( buf, "\n\r\n\rWybra`le`s ju`z:\n\r" );
	    for ( sn = 0; sn < 5; sn++ )
		if ( ch->pcdata->multi[ sn ] > 0 )
		{
		    if ( sn == 4 )
			strcat( buf, "\n\r" );
		    sprintf( buf1, "%s", wyrownaj( skill_table[ ch->pcdata->multi[ sn ] ].pl_name, 19 ) );
		    strcat( buf, buf1 );
		    pzl++;
		}

	    if ( pzl < 6 )
	    {
		sprintf( buf1, "\n\r\n\rWybierz %d umiej`etno`s`c (z 5), albo Enter: ", pzl );
		strcat( buf, buf1 );
	    }
	    else
	    {
		if ( ch->sex == 2 )
		    strcat( buf, "\n\r\n\rWybra`la`s ju`z 5 umiej`etno`sci. Czy jeste`s pewna, `ze wybra`la`s s`lusznie? " );
		else
		    strcat( buf, "\n\r\n\rWybra`le`s ju`z 5 umiej`etno`sci. Czy jeste`s pewien, `ze wybra`le`s s`lusznie? " );
		d->connected = CON_CONFIRM_MULTI;
	    }
	    send_to_char( buf, ch );
	}
	break;

    case CON_CONFIRM_MULTI:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#pumiej" );
	    if ( ch->pcdata->multi[ 0 ] < 1 )
		send_to_char( "\n\rCzy na pewno nie chcesz wybra`c sobie `zadnych dodatkowych umiej`etno`sci? ", ch );
	    else
	    {
		if ( ch->sex == 2 )
		    send_to_char( "\n\rCzy wybra`la`s ju`z te umiej`etno`sci, kt`ore chcia`la`s? ", ch );
		else
		    send_to_char( "\n\rCzy wybra`le`s ju`z te umiej`etno`sci, kt`ore chcia`le`s? ", ch );
	    }
	    return;
	}

	switch ( argument[ 0 ] )
	{
	    case 't': case 'T':
		break;
	    default:
/* wyzerowanie */
		ch->pcdata->multi[ 0 ] = 0;
		ch->pcdata->multi[ 1 ] = 0;
		ch->pcdata->multi[ 2 ] = 0;
		ch->pcdata->multi[ 3 ] = 0;
		ch->pcdata->multi[ 4 ] = 0;
/* wyswietlenie mozliwych umiejetnosci */
		strcpy( buf, "\n\rOpr`ocz umiej`etno`sci standardowo dost`epnych twojej klasie, mo`zesz dodatkowo\n\r"
			"wybra`c 5, kt`ore pomog`a ci w grze. Wybierz je spo`sr`od poni`zszych:\n\r" );
		pzl = 0;
		for ( sn = 0; sn < MAX_SKILL; sn++ )
		    if ( skill_table[ sn ].multi_level[ ch->klasa ] < L_APP )
		    {
			if ( pzl % 4 == 0 )
			    strcat( buf, "\n\r" );
			sprintf( buf1, "%s", wyrownaj( skill_table[ sn ].pl_name, 19 ) );
			strcat( buf, buf1 );
			pzl++;
		    }

		strcat( buf, "\n\r\n\rJe`sli nie wiesz co wybra`c, naci`snij Enter, a je`sli wiesz, wpisz nazw`e\n\rpierwszej z `z`adanych umiej`etno`sci: " );
		send_to_char( buf, ch );

		d->connected = CON_GET_MULTI;
		return;
	}

#endif /* DODATKOWE_W_NIANI */

	sprintf( log_buf, "%s (%s@%s): nowa postac.", ch->name,
							d->user, d->host );
	log_string( log_buf );
	sprintf( log_buf, "Za chwil`e do gry wkroczy nowa posta`c, {Y$N{x, %s %s urodzony w %s.",
		race_table[ ch->race ].who_name,
		class_table[ ch->klasa ].long_name,
		ch->pcdata->strefa->stol_mie );
	wiznet( log_buf, ch, NULL, NULL, WIZ_NOWI, 0, 0 );

	STC( "\n\r\n\rZapoznaj si`e teraz z regulaminem:\n\r\n\r", ch );
	do_help( ch, "regulamin" );
	STC( "\n\rPo przeczytaniu, naci`snij ENTER. ", ch );
	d->connected = CON_READ_REGULAMIN;
	break;

    case CON_READ_REGULAMIN:
	STC( "\n\r", ch );
	do_help( ch, "umowa" );
	STC( "\n\rCzy akceptujesz umow`e? ", ch );
	d->connected = CON_UMOWA;
	break;

    case CON_UMOWA:
	if ( !str_cmp( argument, "nie" ) )
	{
	    STC( "Bardzo nam przykro, `ze rezygnujesz z gry. `Zegnaj, ale zapraszamy ponownie!\n\r", ch );
	    close_socket( d, TRUE );
	    return;
	}
	if ( str_cmp( argument, "tak" ) )
	{
	    STC( "Napisz pe`lne \"tak\" lub \"nie\", albo roz`l`acz si`e.\n\rCzy akceptujesz umow`e? ", ch );
	    return;
	}

	STC( "\n\r\n\r", ch );
	do_help( ch, "og`loszenie" );
	d->connected = CON_READ_MOTD;
	break;

    case CON_GET_POLSKIE:

	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    do_help( ch, "#polskie" );
	    write_to_buffer( d, "\n\rWybierz standard polskich liter [ 0.brak  1.dos  2.iso  3.win 4.utf ]: ", 0 );
	    return;
	}

	if ( !is_number( argument ) )
	{
	    if ( !str_prefix( argument, "brak" ) ) /* takze "" */
		pl_tmp = 0;
	    else if ( !str_prefix( argument, "dos" ) )
		pl_tmp = 1;
	    else if ( !str_prefix( argument, "iso" ) )
		pl_tmp = 2;
	    else if ( !str_prefix( argument, "win" ) )
		pl_tmp = 3;
	    else if ( !str_prefix( argument, "utf" ) )
		pl_tmp = 4;
	    else
	    {
		write_to_buffer( d, "Nie znam takiego standardu. Sprobuj raz jeszcze: ", 0 );
		return;
	    }
	}
	else
	    pl_tmp = atoi( argument );
	if ( pl_tmp < 0 || pl_tmp > 4 )
	{
	    write_to_buffer( d, "Nie ma takiego standardu! Podaj 0,1,2,3,4: ", 0 );
	    return;
	}
	ch->polskie = pl_tmp;
	send_to_char( "\n\rTest: P`o`xn`a noc`a jasno`s`c ksi`e`zyca ol`sni`la gra`n.\n\r"
		"P`O`XN`A NOC`A JASNO`S`C KSI`E`ZYCA OL`SNI`LA GRA`N.\n\r"
		"Czy twoj wybor okazal sie sluszny? [T/N]: ", ch );

	d->connected = CON_CONFIRM_POLSKIE;
	break;

    case CON_CONFIRM_POLSKIE:
	if ( !str_cmp( argument, "pomoc" ) || *argument == '?' )
	{
	    pl_tmp = ch->polskie;
	    ch->polskie = 0;
	    do_help( ch, "#ppolskie" );
	    ch->polskie = pl_tmp;
	    write_to_buffer( d, "\n\rCzy twoj wybor okazal sie sluszny? [T/N]: ", 0 );
	    return;
	}

	switch ( argument[ 0 ] )
	{
	    case 't': case 'T': break;
	    default:
	    {
		ch->polskie = 0;
		write_to_buffer( d, "\n\r", 2 );
		do_help( ch, "polskiexxx" );
		write_to_buffer( d, "\n\rWybierz standard polskich liter [ 0.brak  1.dos  2.iso  3.win  4.utf ]: ", 0 );
		d->connected = CON_GET_POLSKIE;
		return;
	    }
	}

	send_to_char( "\n\rPodaj swoje nowe imi`e: ", ch );
	d->polskie = ch->polskie;
	d->connected = CON_GET_NEW_NAME;
	break;

    case CON_READ_MOTD:
	if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_RESTART ) )
	{
	    ch->next        = char_list;
	    char_list       = ch;
	}
	d->connected    = CON_PLAYING;

	if ( CZY_OPCJA( ch, OPC_MIW ) )
	    do_clear( ch, "" ); /* sam przechodzi do dolnego okna */

	if ( ch->pcdata->last_login && *ch->pcdata->last_login )
	{
	    if ( ch->sex == 2 )
		send_to_char( "\n\rOstatnio gra`la`s: ", ch );
	    else
		send_to_char( "\n\rOstatnio gra`le`s: ", ch );
	    send_to_char( ch->pcdata->last_login, ch );
	    send_to_char( "\n\r", ch );
	}

	send_to_char(
 "\n\rBogowie sprowadzaj`a twoje cia`lo z za`swiat`ow i `l`acz`a ci`e z nim w`at`lymi wi`ezami.\n\r\n\r",
	    ch );


	if ( ch->level == 0 )
	{
	    OBJ_DATA *obj;

	    switch ( class_table[ ch->klasa ].attr_prime )
	    {
		case APPLY_STR: ch->pcdata->perm_str = 16; break;
		case APPLY_INT: ch->pcdata->perm_int = 16; break;
		case APPLY_WIS: ch->pcdata->perm_wis = 16; break;
		case APPLY_DEX: ch->pcdata->perm_dex = 16; break;
		case APPLY_CON: ch->pcdata->perm_con = 16; break;
		default: bug( "Blad podstawowego atrybutu profesji (profesja %d)", ch->klasa );
	    }

	    ch->gold    = 2760 + number_fuzzy( 4 ) * number_fuzzy( 5 ) * 11;
	    ch->pcdata->bank = 0;
	    ch->hit     = ch->max_hit;
	    ch->mana    = ch->max_mana;
	    ch->move    = ch->max_move;
	    ch->alignment = 0;
	    if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_VAMPIRE ) )
		REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_VAMPIRE );

	    if ( ch->race == zr_wampir )
		SET_BIT( ch->act, PLR_WAMPIR_OD_URODZENIA );
	    else
	    {
		REMOVE_BIT( ch->act, PLR_WAMPIR_OD_URODZENIA );
		ch->pcdata->wampiryzm.rasa = -1;
		free_string( ch->pcdata->wampiryzm.strefa );
		ch->pcdata->wampiryzm.strefa = str_dup( "" );
		free_string( ch->pcdata->wampiryzm.pochodzenie );
		ch->pcdata->wampiryzm.pochodzenie = str_dup( "" );
		ch->pcdata->wampiryzm.moc = 0;
	    }
	    ch->level   = 1;
	    ch->exp     = 1000;
	    if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_RESTART ) )
	    {
		sprintf( buf, " %s",
		    title_table[ ch->klasa ]
		    [ ch->sex == SEX_FEMALE ? 1 : 0 ] );
		set_title( ch, buf );

		/* na nowych graczy nie czekaja setki nowych notek :) */
		ch->last_note = current_time;
	    }
	    REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_RESTART );
	    /* przy pierwszej sesji nie robi kopii zapasowej */
	    if ( !ch->in_room )
		SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_BACKUP );

	    if ( ch->in_room )
		char_from_room( ch ); /* restart */
	    if ( ch->pcdata->strefa )
		char_to_room( ch, get_room_index( ch->pcdata->strefa->szkola ) );
	    else
		char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );

	    /* ubieranie dopiero w pomieszczeniu */
	    /* obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BANNER ), 0 );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_LIGHT ); */

	    obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_VEST   ), 0 );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_BODY );

	    obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SHIELD ), 0 );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_SHIELD );

	    obj = create_object( get_obj_index( *class_table[ ch->klasa ].weapon ), 0 );
	    obj_to_char( obj, ch );
	    equip_char( ch, obj, WEAR_WIELD );
	}
	else if ( ch->in_room )
	{
	    ROOM_INDEX_DATA *pomieszczenie = ch->in_room;

	    ch->in_room = NULL;
	    char_to_room( ch, pomieszczenie );
	}
	else if ( IS_IMMORTAL( ch ) )
	    char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
	else
	    char_to_room( ch, get_room_index( PRZYWOLANIE( ch ) ) );

	if ( PP( ch ) )
	{
	    CHAR_DATA *rch;

	    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
		if ( !rch->deleted
		  && rch != ch
		  && !IS_NPC( rch )
		  && CZY_OPCJA( rch, OPC_WCHODZENIE )
/*                && ( !IS_SET( ch->act, PLR_WIZINVIS )
		    || get_trust( rch ) >= ch->pcdata->wizinvis ) */
		  && ( get_trust( ch ) < LEVEL_IMMORTAL
		    || get_trust( rch ) >= get_trust( ch ) ) )
		{
		    act_pp( ZM_WZROK | ZM_WID_CHAR, "$n wchodzi do gry.", ch, NULL, rch, TO_VICT );
		}

	}
	else
	/* chyba tylko wchodzenie do szkoly jest mozliwe z pominieciem miejsca
	   poza swiatem */
	{
	    if ( IS_SET( ch->act, PLR_WIZINVIS ) )
		act( ZM_WZROK | ZM_WID_CHAR, "{B### (BN) $n wchodzi do gry.{x", ch, NULL, NULL, TO_ROOM );
	    else
		act( ZM_WZROK | ZM_WID_CHAR, "{B### $n wchodzi do gry.{x", ch, NULL, NULL, TO_ROOM );
	}

	sprintf( log_buf, "%s (%s@%s) wchodzi do gry (pol. %d).", ch->name,
			d->user, d->host, d->descriptor );
	log_string( log_buf );
	if ( IS_SET( ch->act, PLR_WIZINVIS ) )
	    sprintf( buf, "{M$N {W(BN: %d){x wchodzi do gry.", ch->pcdata->wizinvis );
	else
	    sprintf( buf, "{M$N{x wchodzi do gry." );
	wiznet( buf, ch, NULL, NULL, WIZ_LOGIN, 0,
		( get_trust( ch ) > 108 ) ? 108 : get_trust( ch ) );
	if ( ch->pcdata->clan )
	    ch->pcdata->clan_member->ch = ch;

#if defined( IMUD )
	if ( !imud_down )
	{
	    char bufek[ 200 ];

	    sprintf( bufek, "player_on '%s'\n", ch->short_descr );
	    write_to_imud_buffer( bufek );
	}
#endif

	do_look( ch, "auto" );

	/* Ulryk: sprawdzamy, czy pod nieobecnosc nie wygasly restrykcje */
	sprawdz_restrykcje( ch );

	/* Malven: sprawy klanowe. Przenioslem to przed liczenie notek, zeby
	   przy notkach klanowych juz bylo wszystko cacy */
	if ( ch->pcdata->clan )
	{
	    CLAN_DATA *clan = ch->pcdata->clan;

	    /* gracz wchodzi, a jego klan usuniety */
	    if ( clan->status == CLAN_STAT_USUNIETY )
	    {
		if ( ch->pcdata->clan_member->level == CLAN_GUEST )
		    ASTC( "\n\rKlan, do kt`orego chcia`l$o`s nale`ze`c, zosta`l usuni`ety. Jeste`s niesklanowan$y.", ch );
		else
		    ASTC( "\n\rKlan, do kt`orego nale`za`l$o`s, zosta`l usuni`ety. Jeste`s niesklanowan$y.", ch );
		ch->pcdata->clan = NULL;
		ch->pcdata->clan_member = NULL;
	    }
	    else if ( clan->status == CLAN_STAT_USZKODZONY )
	    {
		if ( ch->pcdata->clan_member->level == CLAN_GUEST )
		{
		    ASTC( "\n\rSekretarka klanowa posia`la gdzie`s wyj`atkowo wa`zne dokumenty dotycz`ace klanu,\n\rdo kt`orego chcesz nale`ze`c i jego dzia`lalno`s`c jest chwilowo wstrzymana.\n\rJeste`s niesklanowan$y.", ch );
		    ch->pcdata->clan = NULL;
		    ch->pcdata->clan_member = NULL;
		}
		else
		    ASTC( "\n\rSekretarka klanowa posia`la gdzie`s wyj`atkowo wa`zne dokumenty dotycz`ace twojego\n\rklanu i jego dzia`lalno`s`c jest chwilowo wstrzymana.", ch );
	    }
	}

	/* liczenie nowych notek */
	notes = 0;

	for ( pnote = note_list; pnote; pnote = pnote->next )
	    if ( is_note_to( ch, pnote )
	      && str_cmp( ch->name, pnote->sender )
	      && pnote->date_stamp > ch->last_note )
	    {
		notes++;
	    }

	if ( notes == 1 )
	    send_to_char( "\n\rCzeka na ciebie nowa notka.\n\r", ch );
	else if ( notes > 1 )
	{
	    if ( !liczba( notes ) )
		sprintf( buf, "\n\rCzeka na ciebie %d nowych notek.\n\r", notes );
	    else
		sprintf( buf, "\n\rCzekaj`a na ciebie %d nowe notki.\n\r", notes );
	    send_to_char( buf, ch );
	}

	/* liczenie notek prywatnych */
	pzl = 0;
	notes = 0;

	for ( pnote = note_list; pnote; pnote = pnote->next )
	    if ( str_cmp( ch->name, pnote->sender )
	      && is_name2( ch->name, pnote->to_list )
	      && pnote->date_stamp > ch->last_note )
	    {
		notes++;
	    }

	if ( notes )
	{
	    ch_printf( ch, "Prywatne: %d  ", notes );
	    pzl = 1;
	}

	notes = 0;

	for ( pnote = note_list; pnote; pnote = pnote->next )
	    if ( str_cmp( ch->name, pnote->sender )
	      && ( is_name2( class_table[ ch->klasa ].who_name, pnote->to_list )
		|| is_name2( class_table[ ch->klasa ].ang, pnote->to_list ) )
	      && pnote->date_stamp > ch->last_note )
	    {
		notes++;
	    }

	if ( notes )
	{
	    ch_printf( ch, "Profesjowe: %d  ", notes );
	    pzl = 1;
	}

	notes = 0;

	if ( ch->pcdata->clan && ch->pcdata->clan_member->level >= CLAN_MEMBER )
	{
	    for ( pnote = note_list; pnote; pnote = pnote->next )
		if ( str_cmp( ch->name, pnote->sender )
		  && is_name2( ch->pcdata->clan->name, pnote->to_list )
		  && pnote->date_stamp > ch->last_note )
		{
		    notes++;
		}

		if ( notes )
		{
		    ch_printf( ch, "Klanowe: %d  ", notes );
		    pzl = 1;
		}
	}	/* Cadaver 30.07.2001, 19.09.2001: notki klanowe */

	if ( pzl )
	    send_to_char( "\n\r", ch );

	if ( IS_SET( ch->act, PLR_UZYJ_WYJDZ ) )
	{
	    REMOVE_BIT( ch->act, PLR_UZYJ_WYJDZ );
	    ASTC( "\n\r{YUwaga! Swoj`a ostatni`a gr`e zako`nczy`l$o`s roz`l`aczeniem. Jest to niebezpieczne!\n\r"
		"Nast`epnym razem opu`s`c posta`c poleceniem \"{?WYJD`X{x{Y\".\n\rAby pozna`c wi`ecej szczeg`o`l`ow, zapoznaj si`e z pomoc`a {?ROZ`L`ACZENIE{x{Y.", ch );
	}

	/* Lam 12.2.2004: powiadomienie o trwajacym zadaniu */
	do_quest( ch, "autowej" );

	if ( ch->pcdata->pwd[ 0 ] == '\0' )
	    send_to_char( "\n\r{RUwaga! Nie masz has`la! Napisz \"has`lo\"!{x\n\r", ch );

	if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_KMNKT_BWWU ) )
	    STC( "\n\r                                 {R---===*===---{x\n\r"
		"{WUwaga, BWWU zmieniaj`a swoj`a form`e! Dost`epne dla BWWU staj`a si`e tylko\n\r"
		"umiej`etno`sci profesji, kt`orymi posta`c uko`nczy`la wcze`sniej gr`e.{x\n\r"
		"Je`sli ta posta`c wy`cwiczy`la takie umiej`etno`sci, utraci`la je, odzyskuj`ac\n\r"
		"po jednej sesji `cwicze`n za ka`zd`a utracon`a umiej`etno`s`c. Koniecznie przeczytaj\n\r"
		"pomoc {?POZIOMY{x i {?BWWU{x.\n\r"
		"                                 {R---===*===---{x\n\r\n\r", ch );

	czy_rekord_graczy( ); /* Lam 11.3.98 */

	break;

    case CON_GET_OLD_PWD_PWD:
	/* jesli !*ch->pcdata->pwd, to haslo bylo wyczyszczone w pliku i nie
	   zostalo poddane konwersji */
	if ( *ch->pcdata->pwd
	  && strcmp( lac_sha1_crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    send_to_char( (char *) echo_on_str, ch );
	    send_to_char( "\n\rHas`lo si`e nie zgadza!\n\r", ch );
	    if ( ch->pcdata->was_in_room )
	    {
		if ( ch->in_room )
		    char_from_room( ch );
		char_to_room( ch, ch->pcdata->was_in_room );
	    }
	    d->connected = CON_PLAYING;
	    return;
	}
	send_to_char( "\n\rPodaj nowe has`lo: ", ch );
	d->connected = CON_GET_NEW_PWD_PWD;
	break;

    case CON_GET_NEW_PWD_PWD:
	send_to_char( "\n\r", ch );

	if ( strlen( argument ) < 5 )
	{
	    send_to_char(
       "Has`lo musi mie`c przynajmniej pi`e`c znak`ow d`lugo`sci.\n\rHas`lo: ", ch );
	    return;
	}

	if ( !poprawne_haslo( ch, argument ) )
	{
	    send_to_char(
	"Nowe has`lo jest zbyt `latwe do odgani`ecia, spr`obuj innego.\n\rHas`lo: ", ch );
	    return;
	}

	pwdnew = lac_sha1_crypt( argument, ch->name );
	/* w tym algorytmie niemozliwe, ale na wszelki wypadek */
	for ( p = pwdnew; *p != '\0'; p++ )
	    if ( *p == '~' )
	    {
		send_to_char(
	    "Nowe has`lo jest raczej niedopuszczalne, spr`obuj innego.\n\rHas`lo: ", ch );
		return;
	    }

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd = str_dup( pwdnew );
	send_to_char( "Powt`orz prosz`e has`lo: ", ch );
	d->connected = CON_CONFIRM_NEW_PWD_PWD;
	break;

    case CON_CONFIRM_NEW_PWD_PWD:
	send_to_char( "\n\r", ch );

	if ( strcmp( lac_sha1_crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    send_to_char( "Has`la si`e nie zgadzaj`a!\n\rPowt`orz has`lo: ", ch );
	    d->connected = CON_GET_NEW_PWD_PWD;
	    return;
	}

	send_to_char( (char *) echo_on_str, ch );
	send_to_char( "Has`lo zmienione!\n\r", ch );
	if ( ch->pcdata->was_in_room )
	{
	    if ( ch->in_room )
		char_from_room( ch );
	    char_to_room( ch, ch->pcdata->was_in_room );
	}
	d->connected = CON_PLAYING;
	break;

    case CON_GET_OLD_PWD_DELETE:
    /* najpierw gracz wraca do gry, nastepnie robi do_quit, a potem jest juz
       tylko jego skasowanie, o ile zna haslo */
	send_to_char( (char *) &echo_on_str[ 0 ], ch );
	send_to_char( "\n\r", ch );
	if ( ch->pcdata->was_in_room )
	{
	    if ( ch->in_room )
		char_from_room( ch );
	    char_to_room( ch, ch->pcdata->was_in_room );
	}
	d->connected = CON_PLAYING;

	if ( strcmp( lac_sha1_crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    send_to_char( "Has`lo si`e nie zgadza. NIE SKASOWA`LEM CI`E.\n\r", ch );
	    return;
	}

	wiznet( "{R$N{x kasuje sw`a posta`c. :(", ch, NULL, NULL, WIZ_LOGIN, 0, 0 );
	if ( IS_SET( ch->act, PLR_LOVE ) )
	    do_quit( ch, "zupe`lnie" );
	else if ( IS_HERO( ch ) )
	{
	    sprintf( strsave, "mv %s%s%s%s %s%s%s%s",
		PLAYER_DIR, initial( ch->name ), "/", capitalize( ch->name ),
		PLAYER_DIR, "del", "/", capitalize( ch->name ) );
	    do_quit( ch, "zupe`lnie" );
	    if ( system( strsave ) )
		bug( "nanny()[delete]: Nie udalo sie przeniesc postaci.", 0 );
	}
	else
	{
	    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ), "/",
			capitalize( ch->name ) );
	    do_quit( ch, "zupe`lnie" );
	    if ( remove( strsave ) )
		bug( "nanny()[delete]: Nie dalo sie skasowac postaci", 0 );
	}
	return;

    case CON_GET_OLD_PWD_RESTART:
    {
	char bufek00[ MAX_STRING_LENGTH ];
	RESTART_DATA *restart;

	send_to_char( (char *) &echo_on_str[ 0 ], ch );
	send_to_char( "\n\r", ch );
	if ( strcmp( lac_sha1_crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    if ( ch->pcdata->was_in_room )
	    {
		if ( ch->in_room )
		    char_from_room( ch );
		char_to_room( ch, ch->pcdata->was_in_room );
	    }
	    d->connected = CON_PLAYING;
	    send_to_char( "Has`lo si`e nie zgadza. Nie mo`zesz rozpocz`a`c gry od nowa.\n\r", ch );
	    return;
	}

	wiznet( "{Y$N{x rozpoczyna gr`e od nowa!", ch, NULL, NULL, WIZ_LOGIN, 0, 0 );

	zabij_wszystkie_slugi( ch );
	affect_strip( ch, gsn_kontrola_umyslu );
	die_follower( ch );
	/* Lam 18.10.2000: rozproszenie magii */
	{
	    AFFECT_DATA *paf;

	    for ( paf = ch->affected; paf; paf = paf->next )
	    {
		if ( paf->deleted )
		    continue;

		/* skopiowane z update.c */
		if ( paf->type == gsn_disguise )
		{
		    char zapas[ MIL ];

		    strcpy( zapas, ch->long_descr );
		    free_string( ch->long_descr );
		    ch->long_descr = str_dup( ch->long_descr_orig );
		    free_string( ch->long_descr_orig );
		    ch->long_descr_orig = str_dup( zapas );
		}

		affect_remove( ch, paf, TRUE );
	    }
	}
	/* #2493, 17.2.2009 */
	if ( *ch->false_name )
	    free_string( ch->false_name );
	if ( *ch->long_descr_orig )
	    free_string( ch->long_descr_orig );
	/* czasem nalezaloby ch->long_descr takze wyczyscic, jesli ktos mial
	   ustawiony bedac mocarzem, ale powiedzmy, ze pozostawimy to ocenie
	   bogow w zaleznosci od przypadku */

	/* pozbywanie sie przedmiotow skopiowane z extract_char */
	{
	    OBJ_DATA *obj1, *obj2;

	    obj1 = get_eq_char( ch, WEAR_WIELD   );
	    obj2 = get_eq_char( ch, WEAR_WIELD_2 );
	    if ( obj1 )
		extract_obj( obj1 );
	    if ( obj2 )
		extract_obj( obj2 );
	    for ( obj1 = ch->carrying; obj1; obj1 = obj2 )
	    {
		obj2 = obj1->next_content;
		if ( obj1->deleted )
		    continue;
		extract_obj( obj1 );
	    }
	    for ( obj1 = ch->przechowalnia; obj1; obj1 = obj2 )
	    {
		obj2 = obj1->next_content;
		if ( obj1->deleted )
		    continue;
		extract_obj( obj1 );
	    }
	}

	/* Lam 3.6.2004: historia restartow */
	if ( !ch->pcdata->restart )
	{
	    restart = calloc( sizeof( RESTART_DATA ), 1 );
	    ch->pcdata->restart = restart;
	}
	else
	{
	    for ( restart = ch->pcdata->restart; restart->nast; restart = restart->nast )
		;

	    restart->nast = calloc( sizeof( RESTART_DATA ), 1 );
	    restart = restart->nast;
	}

	sprintf( bufek00, "%s%3d %s %s%s %s",
		ch->trust > ch->level ? "<" : "(",
		ch->level,
		class_table[ ch->klasa ].pl_name,
		wyrownaj( race_table[ ch->race ].who_name, -11 ),
		ch->trust > ch->level ? ">" : ")",
		ascdata( localtime( &current_time ) ) );
	restart->wpis = str_dup( bufek00 );

	/* zakladamy NOWA POSTAC */
	if ( IS_SET( ch->act, PLR_WAMPIR_OD_URODZENIA )
	  && ch->level >= LEVEL_HERO )
	{
	    free_string( ch->pcdata->wampiryzm.strefa );
	    ch->pcdata->wampiryzm.strefa = str_dup( "" );
	    ch->pcdata->wampiryzm.rasa = -1;
	    ch->pcdata->wampiryzm.moc = 0;
	    REMOVE_BIT( ch->act, PLR_WAMPIR_OD_URODZENIA );
	}
	ch->trust = ch->trust > ch->level ? ch->trust : ch->level;
	ch->level = 0;
	ch->exp = 0; /* Lam 28.4.2001 w wyniku pewnego bledu */

	/* Lam 29.12.2002, na wysokich poziomach zarabia sie wiecej pz i po
	   restarcie to by dawalo przewage */
	ch->qp = 0;
	/* Ulryk 27.09.2005: restart usuwa z klanu. Byc moze warto bedzie to
	   zmienic w przyszlosci. Na razie bedzie tak.

	   Lam 17.2.2009: wycofalem to - byla to metoda na pozbycie klanu
	   zalozycieli/mistrzow, czego kod wrecz nie obsluguje. Niewazne, ze
	   w ten sposob mozna byc w klanie ponizej poziomu 15.

	if ( ch->pcdata->clan )
	    usun_z_klanu( ch ); */
	ch->pcdata->perm_str = ch->pcdata->perm_wis = ch->pcdata->perm_int =
		ch->pcdata->perm_con = ch->pcdata->perm_dex = 13;
	ch->pcdata->condition[ COND_FULL ] = ch->pcdata->condition[ COND_THIRST ] = 48;
	ch->pcdata->condition[ COND_DRUNK ] = 0;
	ch->pcdata->killed = ch->pcdata->dead = ch->pcdata->pkilled =
		ch->pcdata->pdead = ch->pcdata->samobojstwa =
		ch->pcdata->wykrwawienia = 0;
	ch->armor = 100;
	ch->practice = 21;
	ch->hit = ch->max_hit = 20;
	ch->mana = ch->max_mana = 100;
	ch->move = ch->max_move = 100;
	ch->wimpy = 0;
	if ( ch->master )
	    stop_follower( ch );

	/* pozbycie sie boskosci */
	bufek00[ 0 ] = '\0';
	if ( is_name2( "bumi", ch->pcdata->immskll ) )
	    strcat( bufek00, "bumi " );
	if ( is_name2( "szpicel", ch->pcdata->immskll ) )
	    strcat( bufek00, "szpicel " );
	if ( is_name2( ":", ch->pcdata->immskll ) )
	    strcat( bufek00, ": " );
	if ( is_name2( ">", ch->pcdata->immskll ) )
	    strcat( bufek00, "> " );
	if ( is_name2( "-", ch->pcdata->immskll ) )
	    strcat( bufek00, "- " );
	free_string( ch->pcdata->immskll );
	ch->pcdata->immskll = str_dup( bufek00 );
	REMOVE_BIT( ch->act, PLR_HOLYLIGHT );
	REMOVE_BIT( ch->act, PLR_WIZINVIS );
	ch->pcdata->wizinvis = 0;
	REMOVE_BIT( ch->act, PLR_THIEF );
	REMOVE_BIT( ch->act, PLR_KILLER );
	REMOVE_BIT( ch->act, PLR_ZABOJCA );
	/* wciaz pozostaje snoop, ale nie ma co sie tym przejmowac */

	/* 20.5.98, czyli niestety pozniejsza poprawka */
	{
	    int sn;

	    for ( sn = 0; sn < MAX_SKILL; sn++ )
	    {
		ch->pcdata->learned[ sn ] = 0;
		ch->pcdata->used[ sn ] = 0;
		ch->pcdata->learned_level[ sn ] = 0;
	    }
	}

	/* Lam 3.6.2004: bylo sporo protestow, bo nie pozwalalem powtarzac zadan
	   po restarcie, a kasowalem PZ, ugialem sie w koncu */
	while ( ch->mpquest )
	    del_mpquest( ch, ch->mpquest );

	/* bedzie od nowa wybieral multi nowa postacia */
	ch->pcdata->multi[ 0 ] =
	ch->pcdata->multi[ 1 ] =
	ch->pcdata->multi[ 2 ] =
	ch->pcdata->multi[ 3 ] =
	ch->pcdata->multi[ 4 ] = 0;

	/* zaczynamy od nowej rasy */
	if ( CZY_WAMPIR( ch ) )
	{
	    if ( is_affected( ch, gsn_postac_nietoperza ) )
		affect_strip( ch, gsn_postac_nietoperza );

	    SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_VAMPIRE );
	}

	do_help( ch, "#rasy_laca" );
	strcpy( buf, "\n\rWybierz ras`e:\n\r[" );
	for ( iRace = 0, pzl = 0; iRace < MAX_RACE; iRace++ )
	{
	    if ( !IS_SET( race_table[ iRace ].race_abilities, RACE_PC_AVAIL )
	      && ( iRace != zr_wampir
		|| !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_VAMPIRE ) ) )
	    {
		continue;
	    }

	    if ( pzl > 0 )
		strcat( buf, " " );
	    strcat( buf, race_table[ iRace ].who_name );
	    pzl++;
	}
	strcat( buf, "]\n\r: " );
	send_to_char( buf, ch );
	d->connected = CON_GET_NEW_RACE;
	break;
    }

    case CON_GET_WOLACZ:
	odmiana_postaci( ch, argument );
	if ( ch->odmiana )
	{
	    odmiana_prompt( ch );
	    break;
	}
	STC( "\n\rCzy tw`oj wo`lacz na pewno jest poprawny? [T/N]: ", ch );
	d->connected = CON_CONFIRM_WOLACZ;
	break;

    case CON_CONFIRM_WOLACZ:
	switch ( *argument )
	{
	case 't': case 'T':
	    lines = ch->pcdata->pagelen;
	    ch->pcdata->pagelen = 20;
	    if ( IS_HERO( ch ) )
		do_help( ch, "nog`loszenie" );
	    do_help( ch, "og`loszenie" );
	    ch->pcdata->pagelen = lines;
	    d->connected = CON_READ_MOTD;
	    break;

	case 'n': case 'N':
	    STC( "Spr`obuj poda`c sw`oj wo`lacz ponownie.\n\r"
	         "Je`sli niepoprawny jest inny przypadek, odmie`n si`e po wej`sciu do gry.\n\r", ch );
	    d->character->odmiana = 6;
	    d->connected = CON_GET_WOLACZ;
	    odmiana_prompt( ch );
	    break;

	default:
	    send_to_char( "Potwierd`x albo zaprzecz [T/N]: ", ch );
	    break;
	}
	break;
    }

    return;
}


#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

/*
	Copyright (C) 1998, 2009
	Paul E. Jones <paulej@packetizer.com>

	Freeware Public License (FPL)

	This software is licensed as "freeware."  Permission to distribute
	this software in source and binary forms, including incorporation
	into other products, is hereby granted without a fee.  THIS SOFTWARE
	IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESSED OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE.  THE AUTHOR SHALL NOT BE HELD
	LIABLE FOR ANY DAMAGES RESULTING FROM THE USE OF THIS SOFTWARE, EITHER
	DIRECTLY OR INDIRECTLY, INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA
	OR DATA BEING RENDERED INACCURATE.
*/


/*
 *  sha1.h
 *
 *  Copyright (C) 1998, 2009
 *  Paul E. Jones <paulej@packetizer.com>
 *  All Rights Reserved
 *
 *****************************************************************************
 *  sha1.h 12 2009-06-22 19:34:25Z paulej
 *****************************************************************************
 *
 *  Description:
 *      This class implements the Secure Hashing Standard as defined
 *      in FIPS PUB 180-1 published April 17, 1995.
 *
 *      Many of the variable names in the SHA1Context, especially the
 *      single character names, were used because those were the names
 *      used in the publication.
 *
 *      Please read the file sha1.c for more information.
 *
 */

/*
 * This structure will hold context information for the hashing operation
 */
typedef struct SHA1Context
{
    uint32
	digest[ 5 ],		/* Message Digest (output)		*/
	length_low,		/* Message length in bits		*/
	length_high;		/* Message length in bits		*/

    int
	block_index,		/* Index into message block array	*/
	computed,		/* Is the digest computed?		*/
	corrupted;		/* Is the message digest corruped?	*/

    uint8 block[ 64 ];		/* 512-bit message blocks		*/
} SHA1Context;

/*
 * Function Prototypes
 */
void SHA1Reset( SHA1Context * );
int SHA1Result( SHA1Context * );
void SHA1Input( SHA1Context *, const uint8 *, unsigned );


/*
 *  sha1.c
 *
 *  Copyright (C) 1998, 2009
 *  Paul E. Jones <paulej@packetizer.com>
 *  All Rights Reserved
 *
 *****************************************************************************
 *  sha1.c 12 2009-06-22 19:34:25Z paulej
 *****************************************************************************
 *
 *  Description:
 *      This file implements the Secure Hashing Standard as defined
 *      in FIPS PUB 180-1 published April 17, 1995.
 *
 *      The Secure Hashing Standard, which uses the Secure Hashing
 *      Algorithm (SHA), produces a 160-bit message digest for a
 *      given data stream.  In theory, it is highly improbable that
 *      two messages will produce the same message digest.  Therefore,
 *      this algorithm can serve as a means of providing a "fingerprint"
 *      for a message.
 *
 *  Portability Issues:
 *      SHA-1 is defined in terms of 32-bit "words".  This code was
 *      written with the expectation that the processor has at least
 *      a 32-bit machine word size.  If the machine word size is larger,
 *      the code should still function properly.  One caveat to that
 *      is that the input functions taking characters and character
 *      arrays assume that only 8 bits of information are stored in each
 *      character.
 *
 *  Caveats:
 *      SHA-1 is designed to work with messages less than 2^64 bits
 *      long. Although SHA-1 allows a message digest to be generated for
 *      messages of any number of bits less than 2^64, this
 *      implementation only works with messages with a length that is a
 *      multiple of the size of an 8-bit character.
 *
 */

/*
 * Define the circular shift macro
 */
#define SHIFT_LEFT( bits, word ) \
	( ( ( word ) << ( bits ) ) | ( ( word ) >> ( 32 - ( bits ) ) ) )

/*
 * Function prototypes
 */
void SHA1ProcessMessageBlock( SHA1Context * );
void SHA1PadMessage( SHA1Context * );

/*
 * This function will initialize the SHA1Context in preparation for computing
 * a new message digest.
 */
void SHA1Reset( SHA1Context *context )
{
    context->length_low =
    context->length_high =
    context->block_index =
    context->computed =
    context->corrupted = 0;

    context->digest[ 0 ] = 0x67452301;
    context->digest[ 1 ] = 0xEFCDAB89;
    context->digest[ 2 ] = 0x98BADCFE;
    context->digest[ 3 ] = 0x10325476;
    context->digest[ 4 ] = 0xC3D2E1F0;

    return;
}


/*
 * This function will return the 160-bit message digest into the digest array
 * within the SHA1Context provided
 */
int SHA1Result( SHA1Context *context )
{
    if ( context->corrupted )
	return 0;

    if ( !context->computed )
    {
	SHA1PadMessage( context );
	context->computed = 1;
    }

    return 1;
}


/*
 * This function accepts an array of octets as the next portion of the message.
 */
void SHA1Input( SHA1Context *context, const uint8 *message_array, unsigned length )
{
    if ( !length )
	return;

    if ( context->computed || context->corrupted )
    {
	context->corrupted = 1;
	return;
    }

    while ( length-- && !context->corrupted )
    {
	context->block[ context->block_index++ ] = *message_array;

	context->length_low += 8;
	if ( context->length_low == 0 )
	{
	    context->length_high++;
	    if ( context->length_high == 0 )
		/* Message is too long */
		context->corrupted = 1;
	}

	if ( context->block_index == 64 )
	    SHA1ProcessMessageBlock( context );

	message_array++;
    }

    return;
}


/*
 * This function will process the next 512 bits of the message stored in the
 * block array.
 */
void SHA1ProcessMessageBlock( SHA1Context *context )
{
    int t;		/* Loop counter                 */
    uint32
	temp,		/* Temporary word value         */
	W[ 80 ],	/* Word sequence                */
	A, B, C, D, E;	/* Word buffers                 */

    /* Initialize the first 16 words in the array W */
    for ( t = 0; t < 16; t++ )
    {
	W[ t ]  = ( ( uint32 ) context->block[ t * 4     ] ) << 24;
	W[ t ] |= ( ( uint32 ) context->block[ t * 4 + 1 ] ) << 16;
	W[ t ] |= ( ( uint32 ) context->block[ t * 4 + 2 ] ) << 8;
	W[ t ] |= ( ( uint32 ) context->block[ t * 4 + 3 ] );
    }

    for ( t = 16; t < 80; t++ )
	W[ t ] = SHIFT_LEFT( 1, W[ t - 3 ] ^ W[ t - 8 ] ^ W[ t - 14 ] ^ W[ t - 16 ] );

    A = context->digest[ 0 ];
    B = context->digest[ 1 ];
    C = context->digest[ 2 ];
    D = context->digest[ 3 ];
    E = context->digest[ 4 ];

#define LOOP( constant, logical_function )			\
    {								\
	temp =  SHIFT_LEFT( 5, A ) +				\
		( logical_function ) +				\
		E + W[ t ] + constant;				\
	E = D;							\
	D = C;							\
	C = SHIFT_LEFT( 30, B );				\
	B = A;							\
	A = temp;						\
    }

    for ( t = 0; t < 20; t++ )
	LOOP( 0x5A827999, ( B & C ) | ( ~B & D ) );

    for ( t = 20; t < 40; t++ )
	LOOP( 0x6ED9EBA1, B ^ C ^ D );

    for ( t = 40; t < 60; t++ )
	LOOP( 0x8F1BBCDC, ( B & C ) | ( B & D ) | ( C & D ) );

    for ( t = 60; t < 80; t++ )
	LOOP( 0xCA62C1D6, B ^ C ^ D );

#undef LOOP

    context->digest[ 0 ] += A;
    context->digest[ 1 ] += B;
    context->digest[ 2 ] += C;
    context->digest[ 3 ] += D;
    context->digest[ 4 ] += E;

    context->block_index = 0;

    return;
}


/*
 * According to the standard, the message must be padded to an even 512 bits.
 * The first padding bit must be a '1'. The last 64 bits represent the length of
 * the original message. All bits in between should be 0. This function will pad
 * the message according to those rules by filling the block array accordingly.
 * It will also call SHA1ProcessMessageBlock( ) appropriately. When it returns,
 * it can be assumed that the message digest has been computed.
 */
void SHA1PadMessage( SHA1Context *context )
{
    context->block[ context->block_index++ ] = 0x80;

    /* Check to see if the current message block is too small to hold the
       initial padding bits and length. If so, we will pad the block, process
       it, and then continue padding into a second block. */
    if ( context->block_index > 55 )
    {
	while ( context->block_index < 64 )
	    context->block[ context->block_index++ ] = 0;

	SHA1ProcessMessageBlock( context );
    }

    while ( context->block_index < 56 )
	context->block[ context->block_index++ ] = 0;

    /* Store the message length as the last 8 octets */
    context->block[ 56 ] = context->length_high >> 24;
    context->block[ 57 ] = context->length_high >> 16;
    context->block[ 58 ] = context->length_high >>  8;
    context->block[ 59 ] = context->length_high;

    context->block[ 60 ] = context->length_low  >> 24;
    context->block[ 61 ] = context->length_low  >> 16;
    context->block[ 62 ] = context->length_low  >>  8;
    context->block[ 63 ] = context->length_low;

    SHA1ProcessMessageBlock( context );

    return;
}


/*
 * Lam 31.10.2005
 *
 * Jak nazwa sugeruje, robi sha1sum na hasle. Robi to uzywajac powyzszego kodu.
 *
 * Zeby nie wygladalo jak sha1sum(1), a takze zajmowalo 30, a nie 40 znakow,
 * napisalem takie smieszne konwertowanie na znaczki 33-97 w smiesznej
 * kolejnosci. Zakres 33-97 nie zawiera spacji (ktora na poczatku lancucha
 * nie zostalaby wczytana przez fread_string) ani tyldki (ktora by popsula
 * plik postaci). Jest wiec fajne, na pierwszy rzut oka w ogole nie wyglada
 * jak suma SHA-1, zajmuje mniej znaczkow i zadne lamacze hasel nie lykna
 * bez kopiowania kodu :) Zawsze jakies utrudnienie :)
 */
char *lac_sha1_crypt( const char *haslo, const char *salt )
{
    int i, j, n;
    char sol[ 3 ];
    static char zwrot[ 33 ];
    char *p;
    SHA1Context ctx;

    p = zwrot;
    if ( strlen( salt ) < 2 )
	strcpy( sol, "xx" );
    else
    {
	sol[ 0 ] = salt[ 0 ];
	sol[ 1 ] = salt[ 1 ];
	sol[ 2 ] = '\0';
    }
    *p++ = sol[ 0 ];
    *p++ = sol[ 1 ];

    SHA1Reset( &ctx );

    SHA1Input( &ctx, (uint8 *) haslo, strlen( haslo ) );
    SHA1Input( &ctx, (uint8 *) sol, strlen( sol ) );

    SHA1Result( &ctx );

    for ( i = 0; i < 5; i++ )
    {
	n = ctx.digest[ i ];
	for ( j = 0; j < 6; j++ )
	{
	    *p++ = ( n & 63 ) + 33;
	    n >>= 6;
	}
    }

    *p = '\0';

    return zwrot;
}
