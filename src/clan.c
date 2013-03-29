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
 * Pierwsze klany napisal Lam. Potem zostaly zablokowane, a wiele rzeczy   *
 * przepisal Malven, a potem po nim Ulryk.				   *
 * Klanowe rzeczy sa jeszcze w: save.c (wiadomo), db.c (wczytywanie listy  *
 * klanow przy starcie), nanny.c (nanny - zabezpieczenie przed usunieciem  *
 * klanu i byciem jego czlonkiem dalej)					   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"
#include <errno.h>

bool	check_clan_name		args( ( CHAR_DATA *ch, char *arg ) );
int	zmien_stopien_klanowy	args( ( CHAR_DATA *ch, int stopien ) );
void	real_write_clan		args( ( CLAN_DATA *clan, int flagi ) );
void	write_clan_list		args( ( void ) );
void	czytaj_urny		args( ( void ) );
void	pokaz_karte_klanu	args( ( CHAR_DATA *ch, CLAN_DATA *clan ) );
bool	clan_authorized		args( ( CHAR_DATA *ch,
					const struct clan_cmd_type *cmd ) );

/*
 * Nowa struktura klanowa
 */
CLAN_DATA *clan_new( void )
{
    CLAN_DATA *clan;
    int i;

    if ( !clan_free )
    {
	clan = alloc_perm( sizeof( *clan ) );
	top_clan++;
    }
    else
    {
	clan      = clan_free;
	clan_free = clan->next;
    }

    clan->next        = NULL;
    clan->status      = CLAN_STAT_USZKODZONY;
    clan->name        = str_dup( "" );
    clan->dop         = str_dup( "" );
    clan->dir         = str_dup( "" );
    clan->info        = str_dup( "" );
    clan->statut      = str_dup( "" );
    clan->skrot       = str_dup( "" );
    clan->data_zal    = str_dup( "" );
    clan->member_list = NULL;
    clan->rel_list    = NULL;
    clan->gold        = 0;
    clan->punkty      = 0;
    clan->urna        = NULL;
/*  TODO:
    clan->room        = 0;
    clan->flags       = 0; */

    for ( i = 0; i < 10; i++ )
    {
	clan->lev_name_mm[ i ] = str_dup( clan_lev_name_mm[ i ] );
	clan->lev_name_zm[ i ] = str_dup( clan_lev_name_zm[ i ] );
	clan->lev_name_md[ i ] = str_dup( clan_lev_name_md[ i ] );
	clan->lev_name_zd[ i ] = str_dup( clan_lev_name_zd[ i ] );
	clan->lev_name_mn[ i ] = str_dup( clan_lev_name_mn[ i ] );
	clan->lev_name_zn[ i ] = str_dup( clan_lev_name_zn[ i ] );
    }

    return clan;
}


CLAN_MEMBER_DATA *clan_member_new( void )
{
    CLAN_MEMBER_DATA *member;

    if ( !clan_member_free )
    {
	member = alloc_perm( sizeof( *member ) );
	top_clan_member++;
    }
    else
    {
	member           = clan_member_free;
	clan_member_free = member->next;
    }

    member->name      = str_dup( "" );
    member->ch        = NULL;
    member->level     = CLAN_NONE;
    member->zloto     = 0;
    member->punkty    = 0;
    member->seen      = 0;

    return member;
}


CLAN_REL_DATA *clan_rel_new( void )
{
    CLAN_REL_DATA *rel;

    if ( !clan_rel_free )
    {
	rel = alloc_perm( sizeof( *rel ) );
	top_clan_rel++;
    }
    else
    {
	rel           = clan_rel_free;
	clan_rel_free = rel->next;
    }

    rel->next	= NULL;
    rel->clan	= NULL;
    rel->war	= FALSE;
    rel->wpkill	= 0;
    rel->wpdead	= 0;
    rel->pkill	= 0;
    rel->pdead	= 0;

    return rel;
}


/*
 * Nazwa klanu moze byc podana w nawiasach kwadratowych.
 */
bool is_clan_member( CHAR_DATA *ch, CLAN_DATA *clan )
{
    if ( IS_NPC( ch ) )
	return FALSE;

    if ( !ch->pcdata->clan_member
      || ch->pcdata->clan_member->level < CLAN_MEMBER )
	return FALSE;

    if ( ch->pcdata->clan == clan )
	return TRUE;

    return FALSE;
}


bool is_any_clan_member( CHAR_DATA *ch )
{
    return !IS_NPC( ch )
	&& ch->pcdata->clan_member
	&& ch->pcdata->clan_member->level >= CLAN_MEMBER;
}


bool is_any_clan_member2( CHAR_DATA *ch )
{
    return !IS_NPC( ch ) && ch->pcdata->clan;
}


bool is_same_clan( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( IS_NPC( ch ) || IS_NPC( victim ) )
	return FALSE;

    if ( !ch->pcdata->clan_member
      || ch->pcdata->clan_member->level < CLAN_MEMBER
      || !victim->pcdata->clan_member
      || victim->pcdata->clan_member->level < CLAN_MEMBER )
	return FALSE;

    if ( ch->pcdata->clan == victim->pcdata->clan )
	return TRUE;

    return FALSE;
}


/*
 * Sprawdza czy klan jest widoczny dla gracza na liscie klanow
 */
bool can_see_clan( CHAR_DATA *ch, CLAN_DATA *clan )
{
    if ( clan->status == CLAN_STAT_AKTYWNY )
	return TRUE;

    if ( !ch )
	return FALSE;

    if ( IS_SET( ch->info, INF_CLAN_GUARD )
      || ( get_trust( ch ) >= L_SEN ) )
	return TRUE;

    if ( ch->pcdata->clan == clan )
	return TRUE;

    return FALSE;
}


bool check_clan_name( CHAR_DATA *ch, char *arg )
{
    CLAN_DATA *cl;
    int        i, dlugosc;

    if ( ( cl = clan_lookup( arg ) ) )
    {
	switch ( cl->status )
	{
	    default:
		send_to_char( "Klan o takiej nazwie ju`z istnieje!\n\r", ch );
		break;
	    case CLAN_STAT_NOWY:
		send_to_char( "Kto`s ci`e uprzedzi`l i zarejestrowa`l ju`z taki klan.\n\r", ch );
		break;
	    case CLAN_STAT_ZAWIESZONY:
		send_to_char( "Klan o takiej nazwie istnieje, ale zosta`l zawieszony.\n\r", ch );
		break;
	    case CLAN_STAT_USUNIETY:
		send_to_char( "Nie mo`zesz wybra`c takiej nazwy z przyczyn historycznych.\n\r", ch );
		break;
	}
	return FALSE;
    }

    dlugosc = strlen( arg );
    for ( i = 0; i < dlugosc; i++ )
	if ( !isalpha( (unsigned char) arg[ i ] )
	  && ( arg[ i ] != '`' )
	  && ( arg[ i ] != ' ' ) )
	{
	    send_to_char( "Taka nazwa dla klanu jest niedopuszczalna.\n\r", ch );
	    return FALSE;
	}

    if ( czy_zajete_imie( arg ) )
    {
	send_to_char( "Nie mo`zesz u`zy`c tej nazwy dla swojego klanu.\n\r", ch );
	return FALSE;
    }

    return TRUE;
}


/*
 * Szuka klanu o podanej nazwie
 */
CLAN_DATA *clan_lookup( char *arg )
{
    CLAN_DATA *clan;

    for ( clan = clan_list; clan; clan = clan->next )
	if ( !str_cmp( clan->name, arg ) )
	    break;

    return clan; /* ew. zwroci NULL jak nie znajdzie */
}


/*
 * Szuka klanu dopasowujac do podanej nazwy
 */
CLAN_DATA *clan_lookup2( char *arg )
{
    CLAN_DATA *clan;

    for ( clan = clan_list; clan; clan = clan->next )
	if ( !str_prefix( arg, clan->name ) )
	    break;

    return clan; /* ew. zwroci NULL jak nie znajdzie */
}


/*
 * Znajduje relacje klanu A do klanu B
 */
CLAN_REL_DATA *clan_rel_lookup( CLAN_DATA *clana, CLAN_DATA *clanb )
{
    CLAN_REL_DATA *rel;

    for ( rel = clana->rel_list; rel; rel = rel->next )
	if ( rel->clan == clanb )
	    break;

    return rel;
}


/*
 * Uaktualnia listy stosunkow klanowych gdy pojawia sie nowy klan
 * lub stary zostaje usuniety.
 */
void check_clan_rel( )
{
    CLAN_DATA     *clan, *clan2;
    CLAN_REL_DATA *rel, *rel2, *rel_next;
    CLAN_REL_DATA *rel_prev = NULL;
    char           buf[ MIL ];

    for ( clan = clan_list; clan; clan = clan->next )
    {
	/* klany uszkodzone nie robia zadnych zmian w strukturach,
	   klany nowe nie maja stosunkow do innych klanow */
	if ( ( clan->status == CLAN_STAT_USZKODZONY )
	  || ( clan->status == CLAN_STAT_NOWY ) )
	    continue;

	for ( rel = clan->rel_list; rel; rel = rel_next )
	{
	    rel_next = rel->next;

	    /* Czy relacja wskazuje na jakis klan? */
	    if ( !rel->clan )
	    {
		bug( "Brak klanu w clan_rel_data.", 0 );
		if ( !rel_prev )
		    clan->rel_list = rel_next;
		else
		    rel_prev->next = rel_next;

		rel->next = clan_rel_free;
		clan_rel_free = rel;
		continue;
	    }

	    /* sprawdzanie, czy oba klany maja zgodne informacje
	       o wzajemnych stosunkach */
	    if ( rel->clan->status != CLAN_STAT_USZKODZONY
	      && rel->clan->status != CLAN_STAT_NOWY
	      && ( rel2 = clan_rel_lookup( rel->clan, clan ) ) )
		if ( ( rel->pkill != rel2->pdead )
		  || ( rel->pdead != rel2->pkill )
		  || ( rel->war != rel2->war )
		  || ( rel->wpkill != rel2->wpdead )
		  || ( rel->wpdead != rel2->wpkill ) )
		{
		    sprintf( buf, "check_clan_rel: Klany \"%s\" i \"%s\" maja niezgodne informacje o wzajemnych stosunkach!",
		    clan->name, rel->clan->name );
		    bug( buf, 0 );
		}
	}

	/* Sprawdzenie, czy klanowi A nie brakuje stosunkow
	   do nowopowstalych klanow. Wylaczone ze sprawdzania
	   klany skasowane. */
	if ( clan->status == CLAN_STAT_USUNIETY )
	    continue;

	for ( clan2 = clan_list; clan2; clan2 = clan2->next )
	{
	    if ( ( clan2->status == CLAN_STAT_USZKODZONY )
	      || ( clan2->status == CLAN_STAT_USUNIETY )
	      || ( clan2->status == CLAN_STAT_NOWY )
	      || ( clan2 == clan ) )
		continue;

	    /* sprawdzenie czy klan A ma wpisane stosunki do klanu B */
	    if ( !( rel = clan_rel_lookup( clan, clan2 ) ) )
	    {
		/* utworzenie relacji klanu A do klanu B */
		rel = clan_rel_new( );

		if ( ( rel2 = clan_rel_lookup( clan2, clan ) ) )
		{
		    sprintf( buf, "Brak relacji klanu \"%s\" do klanu \"%s\", choc odwrotna istnieje. Naprawiam.", clan->name, clan2->name );
		    bug( buf, 0 );

		    rel->clan   = clan2;
		    rel->war    = rel2->war;
		    rel->pkill  = rel2->pdead;
		    rel->pdead  = rel2->pkill;
		    rel->wpkill = rel2->wpdead;
		    rel->wpdead = rel2->wpkill;
		}
		else
		{
		    rel->clan   = clan2;
		    rel->war    = FALSE;
		    rel->pkill  = 0;
		    rel->pdead  = 0;
		    rel->wpkill = 0;
		    rel->wpdead = 0;

		    rel2 = clan_rel_new( );
		    rel2->clan   = clan;
		    rel2->war    = FALSE;
		    rel2->pkill  = 0;
		    rel2->pdead  = 0;
		    rel2->wpkill = 0;
		    rel2->wpdead = 0;

		    rel2->next = clan2->rel_list;
		    clan2->rel_list = rel2;
		}

		rel->next = clan->rel_list;
		clan->rel_list = rel;
	    }
	}
    }

    return;
}


/*
 * Usuniecie struktury klanu z listy klanow.
 * UWAGA: Klany skasowane sa na liscie klanow i maja swoje struktury. Ich sie
 *        nie usuwa ta funkcja.
 */
void clan_remove( CLAN_DATA *clan )
{
    CLAN_DATA *prev;
    int        i;

    /* znalezienie klanu do usuniecia na liscie i usuniecie go z listy */
    if ( clan == clan_list )
	clan_list = clan->next;
    else
    {
	for ( prev = clan_list; prev->next; prev = prev->next )
	    if ( prev->next == clan )
		break;

	if ( !prev->next )
	{
	    bug( "clan_remove: clan not found (null prev)", 0 );
	    return;
	}

	prev->next = clan->next;
    }

    /* wyczyszczenie klanu */
    clan->status      = 0;
    free_string( clan->name );
    free_string( clan->dop );
    free_string( clan->dir );
    free_string( clan->info );
    free_string( clan->statut );
    free_string( clan->skrot );
    free_string( clan->data_zal );
/*  TODO
    clan->room        = 0;
    clan->flags       = 0; */

    for ( i = 0; i < 10; i++ )
    {
	free_string( clan->lev_name_mm[ i ] );
	free_string( clan->lev_name_zm[ i ] );
	free_string( clan->lev_name_md[ i ] );
	free_string( clan->lev_name_zd[ i ] );
	free_string( clan->lev_name_mn[ i ] );
	free_string( clan->lev_name_zn[ i ] );
    }

    /* wyczyszczenie list */
    {
	CLAN_MEMBER_DATA  *member, *member_next;
	CLAN_REL_DATA     *rel, *rel_next;

	for ( member = clan->member_list; member; member = member_next )
	{
	    member_next = member->next;
	    free_string( member->name );
	    member->ch = NULL;
	    member->level = CLAN_NONE;
	    member->next = clan_member_free;
	    member->zloto = 0;
	    member->punkty = 0;
	    member->seen  = 0;
	    clan_member_free = member;
	}
	clan->member_list = NULL;

	for ( rel = clan->rel_list; rel; rel = rel_next )
	{
	    rel_next = rel->next;
	    rel->clan   = NULL;
	    rel->war    = FALSE;
	    rel->wpkill = 0;
	    rel->wpdead = 0;
	    rel->pkill  = 0;
	    rel->pdead  = 0;
	    rel->next = clan_rel_free;
	    clan_rel_free = rel;
	}
	clan->rel_list    = NULL;
    }

    /* wpisanie klanu na liste odzyskowa */
    clan->next = clan_free;
    clan_free = clan;

    return;
}


void usun_z_klanu( CHAR_DATA *ch )
{
    CLAN_MEMBER_DATA *czlonek;
    CLAN_MEMBER_DATA *prev = NULL;
    CLAN_DATA *clan;

    if ( IS_NPC( ch ) )
    {
	bug( "usun_z_klanu: nie na mobach", 0 );
	return;
    }

    if ( !( clan = ch->pcdata->clan ) )
    {
	bug( "usun_z_klanu: ch nie jest w klanie", 0 );
	return;
    }

    for ( czlonek = clan->member_list; czlonek; czlonek = czlonek->next )
    {
	if ( czlonek != ch->pcdata->clan_member )
	{
	    prev = czlonek;
	    continue;
	}

	break;
    }

    if ( !czlonek )
    {
	bug( "usun_z_klanu: nie znalazlem czlonka", 0 );
	return;
    }

    if ( !prev )
	clan->member_list = czlonek->next;
    else
	prev->next = czlonek->next;

    free_string( czlonek->name );
    czlonek->ch = NULL;
    czlonek->level = CLAN_NONE;
    czlonek->zloto = 0;
    czlonek->punkty = 0;
    czlonek->seen = 0;
    czlonek->next = clan_member_free;
    clan_member_free = czlonek;

    write_clan( clan, CLAN_WRITE_LISTA );

    ch->pcdata->clan = NULL;
    ch->pcdata->clan_member = NULL;

    return;
}


#define BRAK_KLANU	1
#define BRAK_MIEJSCA	2

int zmien_stopien_klanowy( CHAR_DATA *ch, int stopien )
{
    CLAN_MEMBER_DATA *member, *prev, *next, *src, *dst;
    int               i;

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
	return BRAK_KLANU;

    /* sprawdzenie ilosci czlonkow majacych juz ten stopien */
    i = 0;
    for ( member = ch->pcdata->clan->member_list;
	member; member = member->next )
    {
	if ( member->level == stopien )
	    i++;
	else if ( member->level > stopien )
	    break;
    }

    if ( i >= max_clan_level_members[ stopien ] )
	return BRAK_MIEJSCA;

    prev = src = dst = NULL;
    for ( member = ch->pcdata->clan->member_list; member; member = next )
    {
	next = member->next;

	if ( !src && ch->pcdata->clan_member == member )
	{
	    if ( !prev )
		ch->pcdata->clan->member_list = member->next;
	    else
		prev->next = member->next;

	    member->next = NULL;
	    src = member;
	    continue;
	}

	if ( !dst && member->level >= stopien )
	{
	    if ( !prev )
		dst = ch->pcdata->clan->member_list;
	    else
		dst = prev;
	}

	if ( src && dst )
	    break;

	prev = member;
    }

    if ( !src )
    {
	bug( "Brak ch->pcdata->clan_member na klanowej member_list.", 0 );
	src = ch->pcdata->clan_member;
    }

    if ( !dst )
    {
	bug( "Zmiana poziomu na najwyzszy w klanie!", 0 );
	dst = prev;
    }

    src->next = dst->next;
    dst->next = src;
    src->level = stopien;

    write_clan( ch->pcdata->clan, CLAN_WRITE_LISTA );

    return 0;
}


/*
 * Wczytuje plik z informacjami o klanie, odswieza istniejace info
 * jesli bylo juz wczesniej wczytane. Zwraca TRUE jesli sie powiedzie.
 */
bool load_clan_statut( CLAN_DATA *clan )
{
    FILE *fp;
    char path[ MAX_INPUT_LENGTH ];
    int status;
    char *lanc;

    zamknij_reserve( fpReserve );
    sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	CLAN_STATUT_FILE );

    if ( !( fp = fopen( path, "r" ) ) )
    {
	fpReserve = fopen( NULL_FILE, "r" );
	return FALSE;
    }

    lanc = fread_string( fp, &status );
    if ( lanc && !status )
    {
	free_string( clan->statut );
	clan->statut = lanc;
    }

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    return ( lanc && !status );
}


/*
 * Wrzuca str do pliku z logiem klanowym
 */
void clan_log( CLAN_DATA *clan, char *str )
{
    FILE *fp;
    char *strtime;
    char  path[ MAX_INPUT_LENGTH ];

    sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	CLAN_LOG_FILE );

    zamknij_reserve( fpReserve );

    if ( !( fp = fopen( path, "a" ) ) )
    {
	lac_perror( path );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }

    strtime = ctime( &current_time );
    strtime[ strlen( strtime ) - 1 ] = '\0';

    fprintf( fp, "%s :: %s\n", strtime, str );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


/*
 * Rzeczywiste zapisanie jednego klanu, jesli po raz pierwszy to utworzenie
 * dla niego katalogu.
 * Jesli chcesz zapisac klany to uzyj funkcji write_clan lub write_clans,
 * ktore dodatkowo aktualizuja plik z lista zapisanych klanow.
 */
void real_write_clan( CLAN_DATA *clan, int flagi )
{
    FILE        *fp;
    char         nazwa[ MAX_INPUT_LENGTH ];
    char         buf[ MAX_INPUT_LENGTH ];
    char         buf1[ MAX_INPUT_LENGTH ];
    char         path[ MAX_INPUT_LENGTH ];
    int          n = -1;

    /* Klan uszkodzony sie nie zapisuje, jego pliki pozostaja nietkniete tak,
       aby jakis bog/spk mogl do nich zajrzec i zobaczyc co sie stalo. Usuniety
       tez sie nie zapisuje - jego pliki maja odzwierciedlac stan w chwili
       skasowania. */
    if ( ( clan->status == CLAN_STAT_USZKODZONY )
      || ( clan->status == CLAN_STAT_USUNIETY ) )
	return;

    nazwa[ 0 ] = '\0';

    /* jesli klan nie ma jeszcze katalogu, to znaczy,
       ze to pierwszy zapis i trzeba go mu zalozyc */
    while ( !clan->dir[ 0 ] )
    {
	char        *c;
	int          i = 0;
	bool         pol = FALSE;
	CLAN_DATA   *cl = NULL;

/*
 * Wygenerowanie nazwy katalogu.
 * Dla systemow unixopodobnych jest to nazwa klanu bez polskich ogonkow,
 * a dla innych systemow brzydka nazwa powstala przez obciecie nazwy
 * klanu do MAX_CLAN_DIR - 2, usunieciu polskich znakow, zamianie
 * wielkich liter na male i zastapieniu spacji znakiem '_'.
 */
	if ( !nazwa[ 0 ] )
	{
#if defined ( unix ) || defined ( linux )
	    for ( c = clan->name; *c; c++ )
	    {
		if ( *c == '`' )
		{
		    pol = TRUE;
		    continue;
		}
		else if ( pol && *c == 'x' )
		    nazwa[ i ] = 'z';
		else if ( pol && *c == 'X' )
		    nazwa[ i ] = 'Z';
		else
		    nazwa[ i ] = *c;

		pol = FALSE;
		i++;
	    }

	    nazwa[ i ] = '\0';
#else
	    for ( c = clan->name; *c; c++ )
	    {
		if ( *c == '`' )
		{
		    pol = TRUE;
		    continue;
		}
		else if ( *c == ' ' )
		    nazwa[ i ] = '_';
		else if ( pol && tolower( *c ) == 'x' )
		    nazwa[ i ] = 'z';
		else
		    nazwa[ i ] = tolower( *c );

		pol = FALSE;
		if ( ++i == MAX_CLAN_DIR - 2 )
		    break;
	    }

	    nazwa[ i ] = '\0';
#endif
	}

/*
 * Teraz sprawdzam czy wygenerowana nazwa jest unikalna i jesli nie
 * to dolaczam 2-cyfrowa liczbe porzadkowa.
 */
	if ( n == -1 )
	    for ( cl = clan_list; cl; cl = cl->next )
		if ( !strcmp( nazwa, cl->dir ) )
		    break;

	if ( ( n >= 0 ) || cl )
	{
	    for ( i = n; i <= 99; i++ )
	    {
		sprintf( buf, "%s%02d", nazwa, i );

		for ( cl = clan_list; cl; cl = cl->next )
		    if ( !strcmp( buf, cl->dir ) )
			break;

		if ( !cl )
		    break;
	    }

	    n = i + 1;

	    if ( i == 100 )
	    {
		sprintf( buf1, "real_write_clan: mimo usilnych staran nie potrafie zapisac klanu \"%s\"!",
		    cl->name );
		bug( buf1, 0 );
		return;
	    }

	    free_string( clan->dir );
	    clan->dir = str_dup( buf );
	}
	else
	{
	    free_string( clan->dir );
	    clan->dir = str_dup( nazwa );
	    n = 0;
	}

	/* zalozenie katalogu - nazwa w clan->dir */
	sprintf( path, "%s%s", CLAN_DIR, clan->dir );
	sprintf( buf, MKDIR_CMD, path );  /* MKDIR_CMD w config.h */

	if ( system( buf ) == -1 )
	{
	    /* okazuje sie, ze mimo, iz klanu o takiej nazwie katalogu nie ma na
	       liscie klanow, to katalog taki juz istnieje - szukamy dalej */
	    sprintf( buf1, "real_write_clan: katalog '%s' nie przypisany zadnemu klanowi!",
		path );
	    bug( buf1, 0 );
	    free_string( clan->dir );
	    clan->dir = str_dup( "" );
	    continue;
	}
    }

    zamknij_reserve( fpReserve );

/*
 * Zapisanie danych klanu
 */
    if ( clan->status == CLAN_STAT_POMOC_USUWANY )
	clan->status = CLAN_STAT_USUNIETY;

    if ( IS_SET( flagi, CLAN_WRITE_DANE ) )
    {
	int i;

	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_DANE_FILE );

	if ( !( fp = fopen( path, "w" ) ) )
	{
	    lac_perror( path );
	    fpReserve = fopen( NULL_FILE, "r" );
	    return;
	}

	fprintf( fp, "#CLAN    %s~\n",   clan->name );
	fprintf( fp, "Dop      %s~\n",   clan->dop );
	fprintf( fp, "Skrot    %s~\n",   clan->skrot );
	fprintf( fp, "Status   %d\n",    clan->status );
	fprintf( fp, "Info     %s~\n",   clan->info );
	fprintf( fp, "DataZal  %s~\n",   clan->data_zal );
/*      TODO
	fprintf( fp, "Room     %d\n",    clan->room  );
	fprintf( fp, "Flags    %d\n",    clan->flags ); */
	fprintf( fp, "Gold     %d\n",    clan->gold  );
	fprintf( fp, "Punkty   %d\n",    clan->punkty );
	/* nazwy stopni klanowych */
	for ( i = 0; i < 10; i++ )
	{
	    fprintf( fp, "LvNamMM  %d %s~\n", i, clan->lev_name_mm[ i ] );
	    fprintf( fp, "LvNamZM  %d %s~\n", i, clan->lev_name_zm[ i ] );
	    fprintf( fp, "LvNamMD  %d %s~\n", i, clan->lev_name_md[ i ] );
	    fprintf( fp, "LvNamZD  %d %s~\n", i, clan->lev_name_zd[ i ] );
	    fprintf( fp, "LvNamMN  %d %s~\n", i, clan->lev_name_mn[ i ] );
	    fprintf( fp, "LvNamZN  %d %s~\n", i, clan->lev_name_zn[ i ] );
	}

	fclose( fp );
    }

/*
 * Zapisanie listy klanowiczow
 */
    if ( IS_SET( flagi, CLAN_WRITE_LISTA ) )
    {
	CLAN_MEMBER_DATA *member;

	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_LISTA_FILE );

	if ( !( fp = fopen( path, "w" ) ) )
	{
	    lac_perror( path );
	    fpReserve = fopen( NULL_FILE, "r" );
	    return;
	}

	fprintf( fp, "#CLAN %s~\n", clan->name );

	for ( member = clan->member_list; member; member = member->next )
	    fprintf( fp, "%d %s~ %d %d %d\n", member->level, member->name,
		member->zloto, member->punkty, (unsigned int) member->seen );

	fclose( fp );
    }

/*
 * Zapisanie listy stosunkow miedzyklanowych
 */
    if ( IS_SET( flagi, CLAN_WRITE_STOSUNKI ) )
    {
	CLAN_REL_DATA *rel;

	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_STOSUNKI_FILE );

	if ( !( fp = fopen( path, "w" ) ) )
	{
	    lac_perror( path );
	    fpReserve = fopen( NULL_FILE, "r" );
	    return;
	}

	fprintf( fp, "#CLAN %s~\n", clan->name );

	for ( rel = clan->rel_list; rel; rel = rel->next )
	    fprintf( fp, "%s~ %d %d %d\n", rel->clan->name, rel->war, rel->pkill, rel->pdead );

	fclose( fp );
    }

/*
 * TODO: Zapisanie skrzyni
 */
    if ( IS_SET( flagi, CLAN_WRITE_SKRZYNIA ) )
    {
    }

    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


/*
 * Zapisanie pliku z polozeniami plikow klanowych
 */
void write_clan_list( void )
{
    FILE *fp;
    CLAN_DATA *clan;

    zamknij_reserve( fpReserve );
    if ( !( fp = fopen( CLAN_KLANY_FILE, "w" ) ) )
    {
	lac_perror( CLAN_KLANY_FILE );
	fpReserve = fopen( NULL_FILE, "r" );
	return;
    }

    /* klany uszkodzone rowniez zapisuja sie na liscie klanow */
    for ( clan = clan_list; clan; clan = clan->next )
	if ( *clan->dir )
	    fprintf( fp, "%s~\n", clan->dir );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


/*
 * Zapisanie pojedynczego klanu
 */
void write_clan( CLAN_DATA *clan, int flagi )
{
    real_write_clan( clan, flagi );
    write_clan_list( );

    return;
}


/*
 * Zapisanie wszystkich klanow
 */
void write_clans( void )
{
    CLAN_DATA *clan;

    for ( clan = clan_list; clan; clan = clan->next )
	real_write_clan( clan, CLAN_WRITE_ALL );
    write_clan_list( );

    return;
}


/*
 * Lam 16.3.98: zapisywanie urn
 */
void zapisz_urny( void )
{
    FILE *fp;
    CLAN_DATA *clan;

    zamknij_reserve( fpReserve );

    if ( !( fp = fopen( CLAN_PITS, "w" ) ) )
	lac_perror( CLAN_PITS );
    else
	for ( clan = clan_list; clan; clan = clan->next )
	{
	    if ( !clan->urna || !clan->urna->contains )
		continue;
	    fprintf( fp, "#CLAN %s~\n", clan->name );
	    fwrite_obj( NULL, clan->urna->contains, fp, 0, FALSE );
	}

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


/*
 * Lam 16.3.98: czytanie urn.
 * Jesli ktos z pliku usunal klan albo zabral mu urne, rzeczy beda leciec do
 * ogolnej urny na oltarzu.
 */
void czytaj_urny( void )
{
    FILE *fp;
    CLAN_DATA *clan = NULL;
    OBJ_DATA *obj;
    bool czy_do_oltarzowej = FALSE;

    /* na sztywno Leander */
    for ( obj = object_list; obj; obj = obj->next )
	if ( !obj->deleted && is_name( "#urna_na_dary_Leander", obj->name ) )
	    break;

    if ( !obj )
    {
	bug( "czytaj_urny: swiat zostal pozbawiony urny na oltarzu!", 0 );
	return;
    }

    zamknij_reserve( fpReserve );

    if ( !( fp = fopen( CLAN_PITS, "r" ) ) )
	lac_perror( CLAN_PITS );
    else
    {
	char *klucz;
	int   status;
	char  letter;
	char *pomoc;

	for ( ; ; )
	{
	    letter = fread_letter( fp );
	    if ( letter != '#' )
	    {
		if ( !feof( fp ) )
		    bug( "czytaj_urny: # not found.", 0 );
		break;
	    }
	    klucz = fread_word( fp, &status, TRUE );
	    if ( !str_cmp( klucz, "CLAN" ) )
	    {
		pomoc = fread_string( fp, &status );
		if ( strlen( pomoc ) != 3 )
		{
		    bug( "czytaj_urny: klan nie trzyliterowy!", 0 );
		    czy_do_oltarzowej = TRUE;
		    free_string( pomoc );
		    continue;
		}
		if ( !( clan = clan_lookup( pomoc ) ) )
		{
		    bug( "czytaj_urny: nie znalazlem klanu!", 0 );
		    czy_do_oltarzowej = TRUE;
		    free_string( pomoc );
		    continue;
		}
		if ( !clan->urna )
		{
		    bug( "czytaj_urny: klan bez urny!", 0 );
		    czy_do_oltarzowej = TRUE;
		    free_string( pomoc );
		    continue;
		}
		czy_do_oltarzowej = FALSE;
		free_string( pomoc );
		continue;
	    }
	    if ( str_cmp( klucz, "OBJECT" ) )
	    {
		bug( "czytaj_urny: #OBJECT not found", 0 );
		break;
	    }
	    if ( !czy_do_oltarzowej )
		fread_obj( clan->urna, fp, 2 );
	    else
		fread_obj( obj, fp, 2 );
	}

	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


void real_clan_lista( CHAR_DATA *ch, char *argument, WHO_DESCRIPTOR_DATA *d )
{
    CLAN_DATA *clan;
    char       buf [ MAX_STRING_LENGTH * 2 ];
    char       buf1[ MAX_STRING_LENGTH * 2 ];

    buf[ 0 ] = buf1[ 0 ] = '\0';
    if ( clan_list )
    {
	for ( clan = clan_list; clan; clan = clan->next )
	    if ( can_see_clan( ch, clan ) )
	    {
		if ( ch )
		    sprintf( buf1, "- %s", clan->name );
		else
		    sprintf( buf1, "- <a href=\"/klany?p=%s\">%s</a>",
			clan->name, clan->name );
		strcat( buf, buf1 );
		if ( *clan->skrot )
		{
		    sprintf( buf1, " (%s{x)", clan->skrot );
		    strcat( buf, buf1 );
		}
		if ( clan->status < CLAN_STAT_AKTYWNY )
		{
		    sprintf( buf1, "   {r(%s){x", clan_stat_name[ clan->status ] );
		    strcat( buf, buf1 );
		}
		if ( ch )
		    strcat( buf, "\n\r" );
		else
		    strcat( buf, "<br>" );
	    }
    }

    if ( ch )
    {
	if ( buf[ 0 ] == '\0' )
	    strcpy( buf1, "Nie znam `zadnych klan`ow.\n\r" );
	else
	    strcpy( buf1, "Lista klan`ow:\n\r" );
	strcat( buf1, buf );
	send_to_char( buf1, ch );
    }
    else
    {
	if ( buf[ 0 ] == '\0' )
	    strcpy( buf1, "Nie znam `zadnych klan`ow.<br>" );
	else
	    strcpy( buf1, "Lista klan`ow:<br>" );
	strcat( buf1, buf );
	colourconv( buf, buf1, 0, sizeof( buf ) );
	STH( d, buf );
    }

    return;
}


void real_clan_info( CHAR_DATA *ch, CLAN_DATA *cl, WHO_DESCRIPTOR_DATA *d )
{
    CLAN_MEMBER_DATA *czlonek;
    char wyjbuf[ 2 * MAX_STRING_LENGTH ];
    char buf[ 4 * MAX_STRING_LENGTH ];
    int  licznik = 0;

    if ( !( cl->info ) || !( *cl->info ) )
	sprintf( wyjbuf, "Klan {W%s{x nie udost`epnia informacji o sobie.\n\r", cl->dop );
    else
    {
	sprintf( wyjbuf, "Informacje klanu {W%s{x:\n\r", cl->dop );
	strcat( wyjbuf, cl->info );
	strcat( wyjbuf, "{x" );
    }

    for ( czlonek = cl->member_list; czlonek; czlonek = czlonek->next )
	licznik++;

    sprintf( buf, "\n\rKlan zosta`l za`lo`zony %s i zrzesza obecnie %d %s.\n\r",
	cl->data_zal,
	licznik, liczba( licznik ) == 0
	    ? "os`ob" : liczba( licznik ) == 1 ? "osob`e" : "osoby" );
    strcat( wyjbuf, buf );

    if ( ch && !IS_NPC( ch ) && ch->pcdata->clan == cl )
    {
	sprintf( buf, "{xTw`oj klan ma w banku {W%d{x z`lota i {W%d{x punkt`ow klanowych.\n\r", cl->gold, cl->punkty );
	strcat( wyjbuf, buf );
    }

    if ( ch )
	send_to_char( wyjbuf, ch );
    else
    {
	colourconv( buf, lac2html( wyjbuf, FALSE ), 0, sizeof( buf ) );
	STH( d, buf );
    }

    return;
}


/*
 * Polecenia klanowe - Malven, Ulryk
 */
DECLARE_CLAN_FUN( clan_fun_dolacz );	/* klan dolacz <klan> */
DECLARE_CLAN_FUN( clan_fun_info );	/* klan informacje [+<tekst>|-|<klan>]*/
DECLARE_CLAN_FUN( clan_fun_karta );
DECLARE_CLAN_FUN( clan_fun_kto );	/* klan kto [<klan>] */
DECLARE_CLAN_FUN( clan_fun_lista );	/* klan lista */
DECLARE_CLAN_FUN( clan_fun_mow );	/* klan mow <tekst> */
DECLARE_CLAN_FUN( clan_fun_nowy );	/* klan nowy <nazwa> */
DECLARE_CLAN_FUN( clan_fun_odswiez );	/* klan odswiez <klan> */
DECLARE_CLAN_FUN( clan_fun_pomoc );	/* klan pomoc [<temat>] */
DECLARE_CLAN_FUN( clan_fun_pokoj );
DECLARE_CLAN_FUN( clan_fun_podda );
DECLARE_CLAN_FUN( clan_fun_poddaj );
DECLARE_CLAN_FUN( clan_fun_przekaz );	/* klan przekaz <ile> */
DECLARE_CLAN_FUN( clan_fun_rezygnu );	/* klan rezygnu */
DECLARE_CLAN_FUN( clan_fun_rezygnuj );  /* klan rezygnuj */
DECLARE_CLAN_FUN( clan_fun_skasu );	/* klan skasu */
DECLARE_CLAN_FUN( clan_fun_skasuj );	/* klan skasuj */
DECLARE_CLAN_FUN( clan_fun_statut );	/* klan statut [<klan>] */
DECLARE_CLAN_FUN( clan_fun_status );	/* klan stan <klan> [<stan>] */
DECLARE_CLAN_FUN( clan_fun_stopien );	/* klan stopien <postac> <stopien> */
DECLARE_CLAN_FUN( clan_fun_ustawienia );/* klan ustawienia [...] */
DECLARE_CLAN_FUN( clan_fun_umi );
DECLARE_CLAN_FUN( clan_fun_umiustaw );
DECLARE_CLAN_FUN( clan_fun_wojn );
DECLARE_CLAN_FUN( clan_fun_wojna );
DECLARE_CLAN_FUN( clan_fun_wplac );	/* klan wplac <ile> */
DECLARE_CLAN_FUN( clan_fun_wpusc );	/* klan wpusc <postac> */
DECLARE_CLAN_FUN( clan_fun_wykop );	/* klan wykop <postac> */
DECLARE_CLAN_FUN( clan_fun_zatwierdz ); /* klan zatwierdz <klan> */


const  struct  clan_cmd_type  clan_cmd_table  [ ] =
{
    { "do`l`acz",	clan_fun_dolacz,	-1,  0,			0 },
    { "informacje",	clan_fun_info,		-1,  0,			CLAN_CMD_OOC },
    { "kto",		clan_fun_kto,		 1,  CLAN_STAT_AKTYWNY,	CLAN_CMD_OOC | CLAN_CMD_SPK },
    { "karta",		clan_fun_karta,		-1,  0,			CLAN_CMD_SPKONLY | CLAN_CMD_OOC },
    { "lista",		clan_fun_lista,		-1,  0,			CLAN_CMD_OOC },
    { "m`ow",		clan_fun_mow,		 1,  CLAN_STAT_AKTYWNY, CLAN_CMD_OOC | CLAN_CMD_NOIC },
    { "nowy",		clan_fun_nowy,		-1,  0,			0 },
    { "od`swie`z",	clan_fun_odswiez,	-1,  0,			CLAN_CMD_SPKONLY | CLAN_CMD_OOC },
    { "pomoc",		clan_fun_pomoc,     	-1,  0,			CLAN_CMD_OOC },
    { "przeka`z",	clan_fun_przekaz,	 1,  CLAN_STAT_AKTYWNY,	0 },
    { "rezygnu",	clan_fun_rezygnu,	 0,  CLAN_STAT_AKTYWNY, CLAN_CMD_UKRYTE },
    { "rezygnuj",	clan_fun_rezygnuj,	 0,  CLAN_STAT_AKTYWNY, 0 },
    { "skasu",		clan_fun_skasu,		 8,  CLAN_STAT_NOWY,	CLAN_CMD_UKRYTE },
    { "skasuj",		clan_fun_skasuj,	 8,  CLAN_STAT_NOWY,	0 },
    { "statut",		clan_fun_statut,	-1,  0,			CLAN_CMD_OOC },
    { "stan",		clan_fun_status, 	-1,  0,			CLAN_CMD_SPKONLY | CLAN_CMD_OOC },
    { "stopie`n",	clan_fun_stopien,	 7,  CLAN_STAT_AKTYWNY,	0 },
    { "ustawienia",	clan_fun_ustawienia,	 9,  CLAN_STAT_NOWY,	CLAN_CMD_OOC },
    { "umi",		clan_fun_umi,		10,  CLAN_STAT_AKTYWNY,	0 },
    { "umiustaw",	clan_fun_umiustaw,	10,  CLAN_STAT_AKTYWNY,	0 },
    { "wp`la`c",	clan_fun_wplac,		 1,  CLAN_STAT_AKTYWNY,	0 },
    { "wpu`s`c",	clan_fun_wpusc,		 5,  CLAN_STAT_AKTYWNY,	0 },
    { "wykop",		clan_fun_wykop,		 5,  CLAN_STAT_AKTYWNY,	0 },
    { "zatwierd`x",	clan_fun_zatwierdz,	-1,  0,			CLAN_CMD_SPKONLY | CLAN_CMD_OOC },
    { "",		NULL,			 0,  0,			0 }
};


KOMENDA_KLANOWA( clan_fun_skasu )
{
    STC( "Napisz {Wklan skasuj{x je`sli naprawd`e chcesz rozwi`aza`c sw`oj klan.\n\r", ch );

    return;
}


KOMENDA_KLANOWA( clan_fun_rezygnu )
{
    STC( "Je`zeli chcesz wyst`api`c z klanu napisz {Wklan rezygnuj{x.\n\r", ch );

    return;
}


void do_clan_help( CHAR_DATA *ch, char *argument );
KOMENDA_KLANOWA( clan_fun_pomoc )
{
    do_clan_help( ch, argument );

    return;
}


KOMENDA_KLANOWA( clan_fun_nowy )
{
    char buf[ MAX_STRING_LENGTH ];
    CLAN_DATA *cl;
    CHAR_DATA *mob = NULL;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *rch;
    int koszt = 1;

    if ( is_any_clan_member2( ch ) )
    {
	send_to_char( "Nale`zysz ju`z do klanu.\n\r", ch );
	return;
    }

    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	if ( IS_NPC( mob ) && mob->pIndexData->vnum == MOB_VNUM_SECRETARY )
	    break;

    if ( !mob )
    {
	send_to_char( "Tu nie mo`zesz zarejestrowa`c swojego klanu.\n\r", ch );
	return;
    }

    if ( strlen( argument ) < 3 )
    {
	send_to_char( "Musisz poda`c nazw`e klanu d`lugo`sci przynajmniej 3 znak`ow.\n\r", ch );
	return;
    }

    if ( strlen( argument ) > MAX_CLAN_NAME )
    {
	sprintf( buf, "Nazwa klanu mo`ze mie`c najwy`zej %d znak`ow.\n\r", MAX_CLAN_NAME );
	send_to_char( buf, ch );
	return;
    }

    if ( !check_clan_name( ch, argument ) )
	return;

    if ( ch->level < L_SEN )
    {
	koszt = WYDATEK( ch->level, 1000 );
	if ( ch->gold < koszt )
	{
	    ch_printf( ch, "Masz za ma`lo z`lota, aby za`lo`zy`c klan (potrzebujesz %d).\n\r", koszt );
	    return;
	}

	if ( ch->qp < 50 )
	{
	    send_to_char( "Masz za ma`lo punkt`ow zada`n, aby za`lo`zy`c klan (potrzebujesz 50).\n\r", ch );
	    return;
	}

	ch->gold -= koszt;
	ch->qp -= 50;
    }

    cl = clan_new( );
    cl->next = clan_list;
    clan_list = cl;

    free_string( cl->name );
    cl->name = str_dup( argument );
    free_string( cl->dop );
    cl->dop = str_dup( argument );
    sprintf( buf, "%d.%02d.%dr",
	ch->in_room->area->time_info.day,
	ch->in_room->area->time_info.month,
	ch->in_room->area->time_info.year );
    free_string( cl->data_zal );
    cl->data_zal = str_dup( buf );
    cl->status = CLAN_STAT_NOWY;

    /* ustawienie mistrza-zalozyciela */
    ch->pcdata->clan = cl;

    cl->member_list = clan_member_new( );
    free_string( cl->member_list->name );
    cl->member_list->name = str_dup( ch->name );
    cl->member_list->ch = ch;
    cl->member_list->level = 9;
    cl->member_list->zloto += koszt;
    cl->member_list->punkty += 50;

    ch->pcdata->clan_member = cl->member_list;

    write_clan( cl, CLAN_WRITE_ALL );
    send_to_char( "Teraz postaraj si`e o zatwierdzenie twojego klanu.\n\rPatrz \"klan pomoc formalno`sci\".\n\r", ch );

    /* poinformowanie SPK i bogow */
    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected != CON_PLAYING )
	    continue;

	rch = get_char( d->character );

	if ( rch != ch && ( IS_SET( rch->info, INF_CLAN_GUARD )
	  || ( get_trust( rch ) >= L_SEN ) ) )
	{
	    ch_printf( rch, "{W%s{x za`lo`zy`l%s nowy klan o nazwie {W%s{x!\n\r",
		ch->name, ch->sex == 2 ? "a" : "", cl->name );
	}
    }

    return;
}


KOMENDA_KLANOWA( clan_fun_skasuj )
{
    CLAN_DATA        *clan;
    CLAN_MEMBER_DATA *czlonek;
    CHAR_DATA        *rch, *mob = NULL;
    DESCRIPTOR_DATA  *d;
    char              path[ MAX_STRING_LENGTH ];

    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	if ( IS_NPC( mob ) && mob->pIndexData->vnum == MOB_VNUM_SECRETARY )
	    break;

    if ( !mob )
    {
	send_to_char( "Tu nie mo`zesz rozwi`aza`c swojego klanu.\n\r", ch );
	return;
    }

    if ( !( clan = ch->pcdata->clan ) )
    {
	send_to_char( "To niemo`zliwe.\n\r", ch );
	return;
    }

    /* Odklanowanie czlonkow klanu - czlonkowie aktualnie niegrajacy
       beda o tym zawiadomieni przy wejsciu, a grajacy beda zawiadomieni
       od razu. */
    for ( czlonek = clan->member_list; czlonek;  czlonek = czlonek->next )
	if ( czlonek->ch )
	{
	    send_to_char( "{WOpuszczasz usuwany klan.{x\n\r", czlonek->ch );
	    czlonek->ch->pcdata->clan = NULL;
	    czlonek->ch->pcdata->clan_member = NULL;
	    czlonek->seen = current_time;
	    czlonek->ch = NULL;
	    continue;
	}

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected != CON_PLAYING )
	    continue;

	rch = get_char( d->character );

	/* poinformowanie SPK i bogow */
	if ( !IS_NPC( rch ) && ( IS_SET( rch->info, INF_CLAN_GUARD )
	  || ( get_trust( rch ) >= L_SEN ) ) )
	{
	    ch_printf( rch, "{W%s{x skasowa`l%s klan {W%s{x!\n\r",
		ch->name, ch->sex == 2 ? "a" : "", clan->name );
	}
    }

    /* Klany nowe usuwaja sie calkowicie */
    if ( clan->status == CLAN_STAT_NOWY )
    {
	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_DANE_FILE );

	if ( unlink( path ) == -1 && errno != ENOENT )
	    lac_perror( path );

	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_STATUT_FILE );

	if ( unlink( path ) == -1 && errno != ENOENT )
	    lac_perror( path );

	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_LISTA_FILE );

	if ( unlink( path ) == -1 && errno != ENOENT )
	    lac_perror( path );

	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_STOSUNKI_FILE );

	if ( unlink( path ) == -1 && errno != ENOENT )
	    lac_perror( path );

	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_SKRZYNIA_FILE );

	if ( unlink( path ) == -1 && errno != ENOENT )
	    lac_perror( path );

	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_LOG_FILE );

	if ( unlink( path ) == -1 && errno != ENOENT )
	    lac_perror( path );

	/* Usuwanie katalogu */
	sprintf( path, "%s%s", CLAN_DIR, clan->dir );

	if ( rmdir( path ) == -1 && errno != ENOENT )
	    lac_perror( path );

	clan_remove( clan );
	write_clan_list( );
    }
    else
    {
	/* ustawiamy stan klanu na pomoc_usuwany, funkcja zapisujaca
	   ustawi mu stan usuniety i zapisze go po raz ostatni */
	clan->status = CLAN_STAT_POMOC_USUWANY;
	write_clan( clan, CLAN_WRITE_ALL );
    }

    /* dostosowanie list stosunkow miedzyklanowych */
    check_clan_rel( );

    send_to_char( "Zrobione.\n\r", ch );

    return;
}


KOMENDA_KLANOWA( clan_fun_lista )
{
    real_clan_lista( ch, argument, 0 );

    return;
}


KOMENDA_KLANOWA( clan_fun_statut )
{
    char       buf[ MAX_STRING_LENGTH ];
    CLAN_DATA *cl;

    if ( !argument[ 0 ] )
    {
	if ( !is_any_clan_member( ch ) )
	{
	    send_to_char( "Podaj nazw`e klanu.\n\r", ch );
	    return;
	}

	cl = ch->pcdata->clan;
    }
    else if ( !( cl = clan_lookup2( argument ) )
	   || !can_see_clan( ch, cl ) )
    {
	send_to_char( "Nie znam takiego klanu.\n\r", ch );
	return;
    }

    if ( !( cl->statut ) || !( *cl->statut ) )
	ch_printf( ch, "Klan {W%s{x nie ma statutu.\n\r", cl->dop );
    else
    {
	sprintf( buf, "Statut klanu {W%s{x:\n\r", cl->dop );
	send_to_char( buf, ch );
	send_to_char( cl->statut, ch );
	send_to_char( "{x", ch );
    }

    return;
}


KOMENDA_KLANOWA( clan_fun_info )
{
    char              buf[ MAX_STRING_LENGTH ];
    CLAN_DATA        *cl;

    if ( !argument[ 0 ] )
    {
	if ( !is_any_clan_member( ch ) )
	{
	    send_to_char( "Podaj nazw`e klanu.\n\r", ch );
	    return;
	}

	cl = ch->pcdata->clan;
    }
    else if ( argument[ 0 ] == '+' || argument[ 0 ] == '-' )
    {
	if ( !is_any_clan_member( ch )
	  || ch->pcdata->clan_member->level < CLAN_MASTER )
	{
	    send_to_char( "Nie jeste`s w stanie publikowa`c informacji klanowych.\n\r", ch );
	    return;
	}

	cl = ch->pcdata->clan;
	smash_tilde( argument );
	if ( *argument == '-' )
	{
	    int last;
	    int pos;

	    if ( cl->info )
		strcpy( buf, cl->info );

	    if ( !cl->info || !buf[ 0 ] )
	    {
		send_to_char( "Nie masz sk`ad usuwa`c.\n\r", ch );
		return;
	    }

	    last = 0;
	    for ( pos = 0; buf[ pos ]; pos++ )
		if ( buf[ pos ] == '\r' && buf[ pos + 1 ] != '\0' )
		    last = pos;

	    buf[ last + 1 ] = '\0';
	    if ( !last )
		buf[ 0 ] = '\0';
	    free_string( cl->info );
	    cl->info = str_dup( buf );
	    write_clan( cl, CLAN_WRITE_DANE );
	    ASTC( "Zmieni`l$o`s informacj`e o swoim klanie.", ch );
	    return;
	}
	if ( *argument == '+' )
	{
	    if ( cl->info )
		strcpy( buf, cl->info );
	    argument++;
	    while ( isspace( (unsigned char) *argument ) )
		argument++;
	}
	if ( strlen( buf ) + strlen( argument ) >= MAX_STRING_LENGTH - 3 )
	{
	    send_to_char( "Informacja zbyt d`luga.\n\r", ch );
	    return;
	}
	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( cl->info );
	cl->info = str_dup( buf );
	write_clan( cl, CLAN_WRITE_DANE );
	ASTC( "Zmieni`l$o`s informacj`e o swoim klanie.", ch );
	return;
    }
    else if ( !( cl = clan_lookup2( argument ) )
	   || !can_see_clan( ch, cl ) )
    {
	send_to_char( "Nie znam takiego klanu.\n\r", ch );
	return;
    }

    real_clan_info( ch, cl, 0 );

    return;
}


KOMENDA_KLANOWA( clan_fun_status )
{
    CLAN_DATA *clan;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    int        status = 0;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( !arg1[ 0 ] )
    {
	STC( "Kt`ory klan ci`e interesuje?\n\r", ch );
	return;
    }

    if ( !( clan = clan_lookup2( arg1 ) ) )
    {
	STC( "Nie znam takiego klanu.\n\r", ch );
	return;
    }

    switch ( clan->status )
    {
	case CLAN_STAT_USZKODZONY:
	    STC( "Ten klan jest uszkodzony!\n\r", ch );
	    return;
	case CLAN_STAT_POMOC_USUWANY:
	case CLAN_STAT_USUNIETY:
	    STC( "Ten klan zosta`l skasowany.\n\r", ch );
	    return;
    }

    if ( !arg2[ 0 ] )
    {
	ch_printf( ch, "Klan {W%s{x ma stan {W%s (%d){x.\n\r",
	    clan->dop, clan_stat_name[ clan->status ], clan->status );
	return;
    }

    if ( is_number( arg2 ) )
	status = atoi( arg2 );
    else
	for ( status = 4; status >= 0; status-- )
	    if ( !str_prefix( arg2, clan_stat_name[ status ] ) )
		break;

    switch ( status )
    {
	default:
	    STC( "Taki stan nie istnieje.\n\r", ch );
	    return;
	case CLAN_STAT_USZKODZONY:
	    STC( "Stanu \"uszkodzony\" nie mo`zna ustawia`c.\n\r", ch );
	    return;
	case CLAN_STAT_USUNIETY:
	    STC( "Je`sli na pewno chcesz to zrobi`c, u`zyj polecenia klan skasuj.\n\r", ch );
	    return;
	case CLAN_STAT_NOWY:
	    STC( "Stanu \"nowy\" nie mo`zna ustawia`c.\n\r", ch );
	    return;
	case CLAN_STAT_ZAWIESZONY:
	case CLAN_STAT_AKTYWNY:
	    break;
    }

    if ( status == clan->status )
    {
	STC( "To nic nie zmienia.\n\r", ch );
	return;
    }

    clan->status = status;
    check_clan_rel( );
    write_clan( clan, CLAN_WRITE_DANE | CLAN_WRITE_STOSUNKI );

    ch_printf( ch, "Ustawiasz klanowi {W%s{x stan {W%s (%d){x.\n\r",
	clan->dop, clan_stat_name[ clan->status ], clan->status );

    return;
}


KOMENDA_KLANOWA( clan_fun_zatwierdz )
{
    CLAN_DATA *clan;
    char       buf[ MAX_INPUT_LENGTH ];

    if ( !*argument )
    {
	STC( "Kt`ory klan ci`e interesuje?\n\r", ch );
	return;
    }

    if ( !( clan = clan_lookup2( argument ) ) )
    {
	STC( "Nie znam takiego klanu.\n\r", ch );
	return;
    }

    if ( clan->status == CLAN_STAT_NOWY )
    {
	sprintf( buf, "%s aktywny", argument );
	clan_fun_status( ch, buf );
    }
    else
	STC( "Mo`zesz zatwierdza`c jedynie nowe klany.\n\r", ch );

    return;
}


KOMENDA_KLANOWA( clan_fun_dolacz )
{
    CLAN_DATA        *clan;
    CLAN_MEMBER_DATA *member;

    if ( is_any_clan_member2( ch ) )
    {
	send_to_char( "Nale`zysz ju`z do klanu.\n\r", ch );
	return;
    }

    if ( !argument[ 0 ] )
    {
	ASTC( "Do jakiego klanu zdecydowa`l$o`s si`e przy`l`aczy`c?", ch );
	return;
    }

    if ( !( clan = clan_lookup2( argument ) )
      || !can_see_clan( ch, clan ) )
    {
	send_to_char( "Nie ma takiego klanu.\n\r", ch );
	return;
    }

    switch ( clan->status )
    {
	case CLAN_STAT_USZKODZONY:
	    STC( "To niemo`zliwe, poniewa`z klan jest uszkodzony.\n\r", ch );
	    return;
	case CLAN_STAT_USUNIETY:
	    STC( "Nie mo`zesz do`l`aczy`c do klanu, kt`ory jest ju`z tylko histori`a.\n\r", ch );
	    return;
	case CLAN_STAT_ZAWIESZONY:
	    STC( "Obecnie klan jest zawieszony. Mo`zesz spr`obowa`c za jaki`s czas.\n\r", ch );
	    return;
	case CLAN_STAT_NOWY:
	    STC( "Poczekaj, niech najpierw bogowie zatwierdz`a ten klan.\n\r", ch );
	    return;
	default:
	    ch_printf( ch, "Teraz jeste`s kandydat%s do klanu. Popro`s cz`lonk`ow klanu o przyj`ecie.\n\r", ch->sex == 2 ? "k`a" : "em" );
    }

    ch->pcdata->clan = clan;

    member = clan_member_new( );
    free_string( member->name );
    member->name = str_dup( ch->name );
    member->ch = ch;
    member->level = CLAN_GUEST;

    member->next      = clan->member_list;
    clan->member_list = member;

    ch->pcdata->clan_member = member;

    write_clan( clan, CLAN_WRITE_LISTA );

    return;
}


KOMENDA_KLANOWA( clan_fun_rezygnuj )
{
    if ( !is_any_clan_member2( ch ) )
    {
	send_to_char( "Najpierw trzeba nale`ze`c do klanu.\n\r", ch );
	return;
    }

    if ( ch->pcdata->clan_member->level >= CLAN_MASTER )
    {
	send_to_char( "Mistrz nie mo`ze opu`sci`c klanu.\n\r", ch );
	return;
    }

    if ( ch->pcdata->clan_member->level != CLAN_GUEST )
	send_to_char( "`Zegnasz si`e ze swoim klanem.\n\r", ch );
    else
	send_to_char( "Ju`z nie jeste`s kandydatem do klanu.\n\r", ch );

    /* TODO: ACT_ZDRAJCA */
    usun_z_klanu( ch );

    return;
}


KOMENDA_KLANOWA( clan_fun_kto )
{
    CLAN_MEMBER_DATA *member;
    CLAN_DATA        *clan = NULL;
    char              buf [ MAX_STRING_LENGTH ];
    char              buf1[ MAX_STRING_LENGTH * 3 ];
    char              buf2[ MAX_STRING_LENGTH * 3 ];

    buf1[ 0 ] = buf2[ 0 ] = '\0';

    if ( !*argument )
	clan = ch->pcdata->clan_member
	    && ch->pcdata->clan_member->level >= CLAN_MEMBER
		? ch->pcdata->clan : NULL;
    else if ( IS_SET( ch->info, INF_CLAN_GUARD )
	   || ( get_trust( ch ) >= L_SEN ) )
	clan = clan_lookup2( argument );

    if ( !clan )
    {
	if ( IS_SET( ch->info, INF_CLAN_GUARD )
	  || ( get_trust( ch ) >= L_SEN ) )
	    send_to_char( "Podaj nazw`e klanu.\n\r", ch );
	else
	    send_to_char( "Nie jeste`s w klanie.\n\r", ch );
	return;
    }

    strcat( buf1, "Imi`e:        Funkcja w klanie:        Ostatnio w grze:\n\r" );

    for ( member = clan->member_list; member; member = member->next )
	if ( member->level == CLAN_GUEST )
	{
	    sprintf( buf, "%s\n\r", member->name );
	    strcat( buf2, buf );
	}
	else
	{
	    sprintf( buf, "%s ({W%d{x) %s %s\n\r",
		wyrownaj( member->name, -12 ), member->level,
		wyrownaj( clan->lev_name_mm[ member->level ], -20 ),
		member->ch ? "teraz"
		  : ascdata( localtime( &member->seen ) ) );
	    strcat( buf1, buf );
	}

    if ( ch->pcdata->clan == clan
      && ch->pcdata->clan_member->level >= CLAN_HERO && *buf2 )
    {
	strcat( buf1, "\n\rLista oczekuj`acych kandydat`ow:\n\r" );
	strcat( buf1, buf2 );
    }

    send_to_char( buf1, ch );

    return;
}


KOMENDA_KLANOWA( clan_fun_mow )
{
    do_clan_chat( ch, argument );

    return;
}


KOMENDA_KLANOWA( clan_fun_wplac )
{
    CHAR_DATA *mob;
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    int amount;
    int amount_prov;
    CLAN_DATA *cl;

    if ( !( cl = ch->pcdata->clan ) )
    {
	send_to_char( "Nie mog`e tego zrobi`c!", ch );
	return;
    }

    if ( cl->status < CLAN_STAT_AKTYWNY )
    {
	send_to_char( "Na razie klan nie posiada konta w banku.\n\r", ch );
	return;
    }

    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	if ( IS_NPC( mob ) && ( mob->pIndexData->vnum == MOB_VNUM_BANKIER ) )
	    break;

    if ( !mob )
    {
	send_to_char( "Nie ma tu bankiera.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Ile z`lota wp`laci`c?\n\r", ch );
	return;
    }

    amount = atoi( arg );

    if ( amount == 0 )
    {
	send_to_char( "Chcesz co`s wp`laci`c czy nie?\n\r", ch );
	return;
    }
    if ( amount < 0 )
    {
	send_to_char( "Marzenie!\n\r", ch );
	return;
    }

    amount_prov = amount + amount / 20;
    if ( amount_prov > ch->gold )
    {
	sprintf( buf, "Z prowizj`a b`edzie to %s. Nie masz tyle przy sobie.\n\r", ZLOTO_M( amount_prov ) );
	send_to_char( buf, ch );
	return;
    }

    ch->gold -= amount_prov;
    cl->gold += amount;
    ch->pcdata->clan_member->zloto += amount;
    write_clan( cl, CLAN_WRITE_DANE | CLAN_WRITE_LISTA );

    sprintf( buf, "Razem z prowizj`a zap`laci`l$o`s %s.", ZLOTO_B( amount_prov ) );
    ASTC( buf, ch );

    return;
}


KOMENDA_KLANOWA( clan_fun_przekaz )
{
    CHAR_DATA *mob;
    char       arg[ MAX_INPUT_LENGTH ];
    char       buf[ MAX_STRING_LENGTH ];
    int        amount;
    CLAN_DATA *cl;

    if ( !( cl = ch->pcdata->clan ) )
    {
	send_to_char( "Nie mog`e tego zrobi`c!", ch );
	return;
    }

    if ( cl->status != CLAN_STAT_AKTYWNY )
    {
	send_to_char( "Na razie klan nie posiada konta z punktami.\n\r", ch );
	return;
    }

    /* TODO: sekretarka */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	if ( IS_NPC( mob ) && ( mob->pIndexData->vnum == MOB_VNUM_SECRETARY ) )
	    break;

    if ( !mob )
    {
	send_to_char( "Nie ma tu sekretarki.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Ile punkt`ow przekaza`c?\n\r", ch );
	return;
    }

    amount = atoi( arg );

    if ( amount == 0 )
    {
	send_to_char( "Zdecyduj si`e, czy chcesz przekaza`c jakie`s punkty.\n\r", ch );
	return;
    }
    if ( amount < 0 )
    {
	send_to_char( "Marzenie!\n\r", ch );
	return;
    }
    if ( amount > ch->qp )
    {
	send_to_char( "Nie masz tylu punkt`ow.\n\r", ch );
	return;
    }

    ch->qp     -= amount;
    cl->punkty += amount;
    ch->pcdata->clan_member->punkty += amount;
    /* FIXME: liczba( ) */
    sprintf( buf, "Przekazujesz klanowi %d punkt`ow.\n\r", amount );
    send_to_char( buf, ch );
    write_clan( cl, CLAN_WRITE_DANE | CLAN_WRITE_LISTA );

    return;
}


KOMENDA_KLANOWA( clan_fun_wpusc )
{
    CHAR_DATA *victim;

    if ( !argument[ 0 ] )
    {
	STC( "Kogo chcesz przyj`a`c do klanu?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc_tele( ch, argument ) ) )
    {
	STC( "Nie ma nikogo takiego.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Tylko gracze mog`a nale`ze`c do klanu.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Przecie`z jeste`s ju`z w klanie!\n\r", ch );
	return;
    }

    if ( victim->pcdata->clan == ch->pcdata->clan
      && victim->pcdata->clan_member->level == CLAN_GUEST )
    {
	if ( ch->pcdata->clan->gold < WYDATEK( victim->level, 50 ) )
	{
	    send_to_char( "Klan ma za ma`lo z`lota.\n\r", ch );
	    return;
	}

	if ( ch->pcdata->clan->punkty < 5 )
	{
	    send_to_char( "Klan ma za ma`lo punk`ow klanowych.\n\r", ch );
	    return;
	}

	/* wpuszczenie osobnika */
	switch ( zmien_stopien_klanowy( victim, CLAN_MEMBER ) )
	{
	    case 0:
		ch->pcdata->clan->gold -= WYDATEK( victim->level, 50 );
		ch->pcdata->clan->punkty -= 5;
		ch_printf( ch, "Zrobione. Wyp`lacasz z kasy klanowej %s oraz 5 punkt`ow.\n\r", ZLOTO_B( WYDATEK( victim->level, 50 ) ) );
		act( ZM_ZAWSZE, "$N przyjmuje ci`e do klanu.", victim, NULL, ch, TO_CHAR );
		break;
	    case BRAK_KLANU: /* niemozliwe */
		STC( "Nie mo`zesz znale`x`c zg`loszenia.\n\r", ch );
		bug( "BRAK_KLANU", 0 );
		break;
	    case BRAK_MIEJSCA:
		STC( "Nie ma ju`z miejsca w klanie.\n\r", ch );
		break;
	    default:
		send_to_char( "Zrobione?\n\r", ch );
		bug( "Nieobslugiwany wynik", 0 );
	}
    }
    else if ( ch->pcdata->clan == victim->pcdata->clan )
	STC( "To nie kandydat, a pe`lnoprawny cz`lonek.\n\r", ch );
    else
	STC( "`Zeby przyj`a`c kogo`s do klanu, ten kto`s musi najpierw tego chcie`c.\n\r", ch );

    return;
}


KOMENDA_KLANOWA( clan_fun_wykop )
{
    CHAR_DATA *victim;

    if ( !argument[ 0 ] || !( victim = get_char_world2( ch, argument ) )
      || !can_see( ch, victim ) )
    {
	send_to_char( "Kogo chcesz wyrzuci`c z klanu? (pami`etaj o nie stosowaniu skr`ot`ow)\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	ASTC( "Nie mo`zesz sam$a siebie wykopa`c.", ch );
	return;
    }

    if ( !is_same_clan( ch, victim ) )
    {
	send_to_char( "Nie jeste`scie w jednym klanie!\n\r", ch );
	return;
    }

    if ( victim->pcdata->clan_member->level >= ch->pcdata->clan_member->level )
    {
	STC( "Masz za nisk`a pozycj`e w klanie, aby to zrobi`c.\n\r", ch );
	return;
    }

    /* TODO: ACT_BANITA */
    usun_z_klanu( victim );

    send_to_char( "Zrobione.\n\r", ch );
    if ( WP( victim ) )
	act( ZM_ZAWSZE, "{W$N wyrzuca ci`e z klanu!{x", victim, NULL, ch, TO_CHAR );
    else
	act_pp( ZM_ZAWSZE, "{W$N wyrzuca ci`e z klanu!{x", victim, NULL, ch, TO_CHAR );

    return;
}


KOMENDA_KLANOWA( clan_fun_stopien )
{
    CHAR_DATA *victim;
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    int stary_stopien, stopien = 0;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( !arg1[ 0 ] )
    {
	STC( "Komu zamierzasz zmieni`c stopie`n klanowy?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc_tele( ch, arg1 ) ) )
    {
	STC( "Nie ma nikogo takiego.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	STC( "Nie mo`zesz zmienia`c w`lasnej pozycji w klanie.\n\r", ch );
	return;
    }

    if ( !is_same_clan( ch, victim ) ) /* w tym sprawdzanie, czy PC */
    {
	STC( "Nie nale`zycie do tego samego klanu.\n\r", ch );
	return;
    }

    if ( victim->pcdata->clan_member->level >= ch->pcdata->clan_member->level )
    {
	STC( "Masz za nisk`a pozycj`e w klanie, aby to zrobi`c.\n\r", ch );
	return;
    }

    if ( !arg2[ 0 ] )
    {
	STC( "Jaki stopie`n chcesz ustawi`c?\n\r", ch );
	return;
    }

    /* TODO: mozliwosc podawania nazwy stopnia klanowego */
    stopien = atoi( arg2 );
    stary_stopien = victim->pcdata->clan_member->level;

    if ( stopien < CLAN_MEMBER )
	STC( "Usi`lujesz w ten spos`ob wykopa`c t`e osob`e?\n\r", ch );
    else if ( stopien >= ch->pcdata->clan_member->level )
	STC( "Nie masz prawa awansowa`c innych na tak wysoki stopie`n.\n\r", ch );
    else if ( stopien == stary_stopien )
	STC( "Gotowe!\n\r", ch );
    else
	switch ( zmien_stopien_klanowy( victim, stopien ) )
	{
	    case 0:
		STC( "Zrobione.\n\r", ch );
		if ( stopien < stary_stopien )
		    sprintf( buf, "$N degraduje ci`e do stopnia {W%s{x ({W%d{x).",
			victim->sex == SEX_FEMALE
			    ? ch->pcdata->clan->lev_name_zd[ stopien ]
			    : ch->pcdata->clan->lev_name_md[ stopien ],
			stopien );
		else
		    sprintf( buf, "$N awansuje ci`e do stopnia {W%s{x ({W%d{x).",
			victim->sex == SEX_FEMALE
			    ? ch->pcdata->clan->lev_name_zd[ stopien ]
			    : ch->pcdata->clan->lev_name_md[ stopien ],
			stopien );

		act( ZM_ZAWSZE, buf, victim, NULL, ch, TO_CHAR );
		break;
	    case BRAK_KLANU: /* niemozliwe */
		STC( "Nie mo`zesz znale`x`c dokument`ow.\n\r", ch );
		bug( "BRAK_KLANU", 0 );
		return;
	    case BRAK_MIEJSCA:
		ch_printf( ch, "Tw`oj klan nie pomie`sci jeszcze %s %s.\n\r",
		    victim->sex == SEX_FEMALE ? "jednej" : "jednego",
		    victim->sex == SEX_FEMALE
			? ch->pcdata->clan->lev_name_zd[ stopien ]
			: ch->pcdata->clan->lev_name_md[ stopien ] );
		return;
	    default:
		STC( "Zrobione?\n\r", ch );
		bug( "Nieobslugiwany wynik", 0 );
	}

    return;
}


KOMENDA_KLANOWA( clan_fun_umi )
{
    return;
}


KOMENDA_KLANOWA( clan_fun_umiustaw )
{
    return;
}


KOMENDA_KLANOWA( clan_fun_odswiez )
{
    CLAN_DATA *clan;

    if ( !argument[ 0 ] )
    {
	send_to_char( "Statut kt`orego klanu chcesz od`swie`zy`c?\n\r", ch );
	return;
    }

    if ( !( clan = clan_lookup2( argument ) ) )
    {
	send_to_char( "Nie ma takiego klanu.\n\r", ch );
	return;
    }

    if ( load_clan_statut( clan ) )
	send_to_char( "Zrobione.\n\r", ch );
    else
	send_to_char( "Brak pliku ze statutem klanu.\n\r", ch );

    return;
}


void pokaz_karte_klanu( CHAR_DATA *ch, CLAN_DATA *clan )
{
    char buf[ 4 * MAX_STRING_LENGTH ];
    char bufek[ MAX_STRING_LENGTH ];
    int	i;

    /* Lam: dodalem wszedzie {c na poczatku i {x na koncu linii. Bez tego
       kolory rozjezdzaly sie w miejscach, gdzie karta byla dzielona na strony. */
    buf[ 0 ] = bufek[ 0 ] = '\0';
    strcat( buf, "{c+----------------------------------------------------------------------------+{x\n\r{c|                                {GKarta klanu{c                                 |{x\n\r{c| {GDane klanu:{c                                                                |{x\n\r" );
    sprintf( bufek, "{c|   {gNazwa: {Y%s{c                          |{x\n\r", wyrownaj( clan->name, 0 - MAX_CLAN_NAME ) );
    strcat( buf, bufek );
    sprintf( bufek, "{c|   {gDope`lniacz: {Y%s{c                     |{x\n\r", wyrownaj( clan->dop, 0 - MAX_CLAN_DOPELNIACZ ) );
    strcat( buf, bufek );
    sprintf( bufek, "{c|   {gSkr`ot: {Y%s{c                                                           |{x\n\r", *clan->skrot ? wyrownaj( clan->skrot, -7 ) : "(brak) " );
    strcat( buf, bufek );
    strcat( buf, "{c|                                                                            |{x\n\r{c| {GStopnie klanowe:{c                                                           |{x\n\r" );
    for ( i = 0; i < 10; i++ )
    {
        sprintf( bufek, "{c|  {gStopie`n: {Y%d{g          mianownik           dope`lniacz            narz`ednik   {c|{x\n\r", i );
        strcat( buf, bufek );
        sprintf( bufek, "{c|    {gm`eski: {Y%s %s %s   {c|{x\n\r",
            wyrownaj( clan->lev_name_mm[ i ], 20 ),
            wyrownaj( clan->lev_name_md[ i ], 20 ),
            wyrownaj( clan->lev_name_mn[ i ], 20 ) );
        strcat( buf, bufek );
        sprintf( bufek, "{c|   {g`ze`nski: {Y%s %s %s   {c|{x\n\r",
            wyrownaj( clan->lev_name_zm[ i ], 20 ),
            wyrownaj( clan->lev_name_zd[ i ], 20 ),
            wyrownaj( clan->lev_name_zn[ i ], 20 ) );
        strcat( buf, bufek );
    }
    strcat( buf, "{c+----------------------------------------------------------------------------+{x\n\r" );

    send_to_char( buf, ch );

    return;
}


KOMENDA_KLANOWA( clan_fun_karta )
{
    CLAN_DATA *clan;

    if ( !*argument )
    {
	send_to_char( "Podaj nazw`e klanu.\n\r", ch );
	return;
    }

    if ( !( clan = clan_lookup2( argument ) ) )
    {
	send_to_char( "Nie ma takiego klanu.\n\r", ch );
	return;
    }

    pokaz_karte_klanu( ch, clan );

    return;
}


KOMENDA_KLANOWA( clan_fun_ustawienia )
{
    char	arg[ MAX_INPUT_LENGTH ];
    char	arg2[ MAX_INPUT_LENGTH ];
    CLAN_DATA  *clan;
    bool	fMeski;
    int		licznik = 0;

    if ( !( clan = ch->pcdata->clan ) )
    {
	bug( "brak klanu!", 0 );
	return;
    }

    if ( !*argument )
    {
	pokaz_karte_klanu( ch, clan );
	return;
    }

    argument = one_argument( argument, arg );

    if ( !str_prefix( arg, "dope`lniacz" ) )
    {
	if ( clan->status != CLAN_STAT_NOWY )
	{
	    send_to_char( "Zmienia`c dope`lniacz klanu mo`zna jedynie przed jego akceptacj`a.\n\r", ch );
	    return;
	}

	if ( !*argument )
	{
	    send_to_char( "Jaki ma by`c dope`lniacz?\n\r", ch );
	    return;
	}

	if ( strlen( argument ) >= MAX_CLAN_DOPELNIACZ )
	{
	    send_to_char( "Za d`lugi dope`lniacz.\n\r", ch );
	    return;
	}

	free_string( clan->dop );
	clan->dop = str_dup( argument );
	send_to_char( "Zrobione.\n\r", ch );
    }
    else if ( !str_prefix( arg, "skr`ot" ) )
    {
	if ( clan->status != CLAN_STAT_NOWY )
	{
	    send_to_char( "Zmienia`c skr`ot klanu mo`zna jedynie przed jego akceptacj`a.\n\r", ch );
	    return;
	}

	if ( !*argument )
	{
	    send_to_char( "Jaki ma by`c skr`ot nazwy klanu?\n\r", ch );
	    return;
	}

	if ( strlen_pl_kol( argument ) > 5 )
	{
	    send_to_char( "Skr`ot nie mo`ze mie`c wi`ecej ni`z 5 znak`ow.\n\r", ch );
	    return;
	}

	free_string( clan->skrot );
	clan->skrot = str_dup( argument );
	send_to_char( "Zrobione.\n\r", ch );
    }
    else if ( !str_prefix( arg, "stopie`n" ) )
    {
	argument = one_argument( argument, arg2 );
	if ( !*arg2 || !arg2[ 1 ] || !arg2[ 2 ] )
	{
	    send_to_char( "Z`la sk`ladnia.\n\r", ch );
	    return;
	}

	switch ( arg2[ 1 ] )
	{
	    case 'm':
	    case 'M':
		fMeski = TRUE;
		break;
	    case 'z':
	    case 'Z':
		fMeski = FALSE;
		break;
	    default:
		send_to_char( "Z`la sk`ladnia.\n\r", ch );
		return;
	}
	if ( !isdigit( (unsigned char) arg2[ 2 ] ) )
	{
	    send_to_char( "Z`la sk`ladnia.\n\r", ch );
	    return;
	}
	else
	    licznik = arg2[ 2 ] - '0';

	switch ( arg2[ 0 ] )
	{
	    case 'm':
	    case 'M':
		if ( fMeski )
		{
		    free_string( clan->lev_name_mm[ licznik ] );
		    clan->lev_name_mm[ licznik ] = str_dup( argument );
		}
		else
		{
		    free_string( clan->lev_name_zm[ licznik ] );
		    clan->lev_name_zm[ licznik ] = str_dup( argument );
		}
		break;
	    case 'd':
	    case 'D':
		if ( fMeski )
		{
		    free_string( clan->lev_name_md[ licznik ] );
		    clan->lev_name_md[ licznik ] = str_dup( argument );
		}
		else
		{
		    free_string( clan->lev_name_zd[ licznik ] );
		    clan->lev_name_zd[ licznik ] = str_dup( argument );
		}
		break;
	    case 'n':
	    case 'N':
		if ( fMeski )
		{
		    free_string( clan->lev_name_mn[ licznik ] );
		    clan->lev_name_mn[ licznik ] = str_dup( argument );
		}
		else
		{
		    free_string( clan->lev_name_zn[ licznik ] );
		    clan->lev_name_zn[ licznik ] = str_dup( argument );
		}
		break;
	    default:
		send_to_char( "Z`la sk`ladnia.\n\r", ch );
		return;
	}
	send_to_char( "Zrobione.\n\r", ch );
    }
    else
	send_to_char( "Sk`ladnia: klan ustawienia dope`lniacz|skr`ot <tekst>\n\rSk`ladnia: klan ustawienia stopie`n <m|d|n><m|z><0-9> <stopie`n>\n\r", ch );

    return;
}


/*
 * Malven: Nowe centrum sterowania klanami
 */
bool clan_authorized( CHAR_DATA *ch, const struct clan_cmd_type *cmd )
{
    /* sprawdzenie czy polecenie to nie jest tylko dla SPK i bogow */
    if ( IS_SET( cmd->flags, CLAN_CMD_SPKONLY )
      && !IS_SET( ch->info, INF_CLAN_GUARD )
      && ( get_trust( ch ) < L_SEN ) )
	return FALSE;

    if ( IS_SET( cmd->flags, CLAN_CMD_SPK )
      && ( IS_SET( ch->info, INF_CLAN_GUARD )
	|| ( get_trust( ch ) >= L_SEN ) ) )
	return TRUE;

    /* sprawdzenie stopnia klanowego */
    if ( cmd->clan_level != CLAN_NONE && ( !ch->pcdata->clan_member
      || ch->pcdata->clan_member->level < cmd->clan_level ) )
	return FALSE;

    return TRUE;
}


void do_clan_help( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];
    const struct clan_cmd_type *cmd;
    bool found = FALSE;

    if ( !( *argument ) )
	do_help( ch, "#klan_pomoc" );
    else
    {
	for ( cmd = clan_cmd_table; *cmd->name; cmd++ )
	    if ( !str_prefix( argument, cmd->name ) )
	    {
		found = TRUE;
		break;
	    }

	/* Lam: nowa wersja pozwala zrobic "klan pomoc formalnosci" */
	/* if ( !found || !clan_authorized( ch, cmd ) ) */
	if ( found && !clan_authorized( ch, cmd ) )
	{
	    send_to_char( "Nie pomog`e ci z tym.\n\r", ch );
	    return;
	}

	/* Lam: takie podejscie pozwala skracac nazwy polecen, ale powyzsze
	   "klan pomoc formalnosci" trzeba wpisac w calosci. FIXME */
	if ( found )
	    sprintf( buf, "#klan_%s", cmd->name );
	else
	    sprintf( buf, "#klan_%s", argument );
	do_help( ch, buf );
    }

    return;
}


void do_clan( CHAR_DATA *ch, char *argument )
{
    char         arg1[ MAX_INPUT_LENGTH ];
    char         arg2[ MAX_INPUT_LENGTH ];
    char         buf[ MAX_STRING_LENGTH ];
    bool         found;
    CLAN_DATA   *clan;
    const struct clan_cmd_type *cmd;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Tylko gracze mog`a by`c cz`lonkami klan`ow.\n\r", ch );
	return;
    }

#if !defined( KLANY )
    if ( ch->level < L_SEN )
    {
	STC( "Ta wersja Laca nie pozwala zak`lada`c klan`ow.\n\r", ch );
	return;
    }
    send_to_char( "{WUWAGA{x: Prosz`e nie zak`lada`c klan`ow na publicznym serwerze Laca.\n\r", ch );
#endif

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    clan = ch->pcdata->clan;

    if ( !clan && ch->pcdata->clan_member )
    {
	STC( "Jeste`s cz`lonkiem klanu, kt`ory nie istnieje. Polecenie \"klan\" zablokowane.\n\r", ch );
	bug( "Gracz jest czlonkiem klanu, ktory nie istnieje.", 0 );
	return;
    }

    /* polecenie klan bez parametrow */
    if ( arg1[ 0 ] == '\0' )
    {
	char  buf1[ MAX_STRING_LENGTH ];
	int   col = 0;

	buf1[ 0 ] = '\0';
	strcpy( buf, "Dost`epne ci polecenia klanowe:\n\r" );
	for ( cmd = clan_cmd_table; *cmd->name; cmd++ )
	{
	    if ( !clan_authorized( ch, cmd ) )
		continue;

	    /* czy polecenie nie jest przypadkiem ukryte? */
	    if ( IS_SET( cmd->flags, CLAN_CMD_UKRYTE ) )
		continue;

	    /* sprawdzenie poziomu gracza przy poleceniach nowy i dolacz */
	    if ( ( !strcmp( cmd->name, "nowy" ) )
	      && ( ( ch->level < 20 ) || ( ch->pcdata->clan_member ) ) )
		continue;

	    if ( ( !strcmp( cmd->name, "do`l`acz" ) )
	      && ( ( ch->level < 15 ) || ( ch->pcdata->clan_member ) ) )
		continue;

	    if ( ( !strcmp( cmd->name, "ustawienia" ) )
	      && ( clan->status != CLAN_STAT_NOWY ) )
		continue;

	    /* sprawdzenie stanu klanu */
	    if ( ( cmd->clan_level == CLAN_NONE )
	      || ( IS_SET( cmd->flags, CLAN_CMD_SPK )
		&& ( IS_SET( ch->info, INF_CLAN_GUARD )
		  || ( get_trust( ch ) >= L_SEN ) ) )
	      || ( clan->status >= cmd->clan_stat ) )
	    {
		/* TODO: klanumi */
		sprintf( buf1, "%s", wyrownaj( cmd->name, -15 ) );
		if ( col % 5 == 0 )
		    strcat( buf, "  " );
		strcat( buf, buf1 );
		if ( ++col % 5 == 0 )
		    strcat( buf, "\n\r" );
	    }
	}
	if ( col % 5 )
	    strcat( buf, "\n\r");
	send_to_char( buf, ch );

	/* TODO: uodpornic to na glupie stopnie klanowe */
	if ( ch->pcdata->clan && ch->pcdata->clan_member->level > CLAN_GUEST )
	{
	    sprintf( buf, "Jeste`s {W%s{x ({W%d{x) klanu {W%s{x.\n\r",
	      ch->sex == SEX_MALE ?
		clan->lev_name_mn[ ch->pcdata->clan_member->level ]
		: clan->lev_name_zn[ ch->pcdata->clan_member->level ],
	      ch->pcdata->clan_member->level,
	      clan->dop );
	    send_to_char( buf, ch );
	}
	else if ( ch->pcdata->clan
	       && ch->pcdata->clan_member->level == CLAN_GUEST )
	{
	    sprintf( buf, "Jeste`s {W%s{x na cz`lonka klanu {W%s{x.\n\r",
	      ch->sex == SEX_MALE ?
		clan->lev_name_mn[ CLAN_GUEST ]
		: clan->lev_name_zn[ CLAN_GUEST ],
	      clan->dop );
	    send_to_char( buf, ch );
	}
	else
	    send_to_char( "Nie jeste`s cz`lonkiem `zadnego klanu.\n\r", ch );

	return;
    }

/*
 * Wyszukanie polecenia klanowego w tabeli,
 * sprawdzenie czy dozwolone i uruchomienie jego funkcji.
 */
    found = FALSE;
    for ( cmd = clan_cmd_table; *cmd->name; cmd++ )
	if ( !str_prefix( arg1, cmd->name ) )
	{
	    found = TRUE;
	    break;
	}

    if ( found )
    {
	/* sprawdzenie stopnia klanowego */
	if ( clan_authorized( ch, cmd ) )
	{
	    /* sprawdzenie poziomu gracza przy poleceniach nowy i dolacz */
	    if ( !strcmp( cmd->name, "nowy" ) )
	    {
		if ( ch->level < 20 )
		{
		    send_to_char( "Aby za`lo`zy`c nowy klan musisz mie`c 20 poziom.\n\r", ch );
		    return;
		}
	    }
	    else if ( !strcmp( cmd->name, "do`l`acz" ) )
	    {
		if ( ch->level < 15 )
		{
		    send_to_char( "Aby do`l`aczy`c do klanu musisz mie`c 15 poziom.\n\r", ch );
		    return;
		}
	    }

	    /* sprawdzenie stanu */
	    if ( ( cmd->clan_level == CLAN_NONE )
	      || ( IS_SET( cmd->flags, CLAN_CMD_SPK )
		&& ( IS_SET( ch->info, INF_CLAN_GUARD )
		  || ( get_trust( ch ) >= L_SEN ) ) )
	      || ( clan->status >= cmd->clan_stat ) )
	    {
		/* TODO: klanumi */

		/* sprawdzenie czy gracz jest PP/WP */
		if ( IS_SET( cmd->flags, CLAN_CMD_NOIC )
		  && WP( ch ) && ch->level < L_SEN )
		{
		    STC( "Nie mo`zesz wykonywa`c tego polecenia klanowego w postaci.\n\r", ch );
		    return;
		}

		if ( !IS_SET( cmd->flags, CLAN_CMD_OOC )
		  && PP( ch ) )
		{
		    STC( "Nie mo`zesz wykonywa`c tego polecenia klanowego poza postaci`a.\n\r", ch );
		    return;
		}

		/* i w koncu wykonanie polecenia */
		cmd->clan_fun( ch, arg2 );
	    }
	    else
	    {
		if ( !clan )
		    STC( "Nie jeste`s w `zadnym klanie.\n\r", ch );
		else if ( clan->status == CLAN_STAT_USZKODZONY )
		    STC( "Z powodu ba`laganu w papierach twojego klanu to polecenie jest zablokowane.\n\r", ch );
		else if ( clan->status == CLAN_STAT_ZAWIESZONY )
		    STC( "Tw`oj klan jest zawieszony. Polecenie zablokowane.\n\r", ch );
		else
		{
		    sprintf( buf, "Aby wykona`c to polecenie tw`oj klan musi mie`c stan co najmniej {W%s{x.\n\r", clan_stat_name[ cmd->clan_stat ] );
		    send_to_char( buf, ch );
		}
	    }
	}
	else if ( clan && ch->pcdata->clan_member->level < cmd->clan_level
	       && ch->pcdata->clan_member->level >= CLAN_GUEST )
	{
	    sprintf( buf, "Aby wykona`c to polecenie musisz by`c {W%s{x ({W%d{x).\n\r",
		ch->sex == SEX_MALE ?
		    clan->lev_name_mn[ cmd->clan_level ]
		    : clan->lev_name_zn[ cmd->clan_level ],
		cmd->clan_level );
	    send_to_char( buf, ch );
	}
	else
	    send_to_char( "Nie znam takiego polecenia klanowego.\n\r", ch );
    }
    else
	send_to_char( "Nie znam takiego polecenia klanowego.\n\r", ch );

    return;
}
