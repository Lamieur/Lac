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
 *  Obsluga wyrazen i operacji na zmiennych w progach.                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"


void	usun_zm_zwod		args( ( TYP_ZMIENNEJ **lista, TYP_ZMIENNEJ *zm,
					CHAR_DATA *mob ) );
STOSMP *nowy_kawalek_stosmp	args( ( void ) );
void	wywal_kawalek_stosmp	args( ( STOSMP *kawalek ) );
void	na_stosmp		args( ( STOSMP **stos, STOSMP *kawalek ) );
void	na_koniec_stosmp	args( ( STOSMP **glowa_stos, STOSMP *kawalek ) );
STOSMP	*ze_stosmp		args( ( STOSMP **stos ) );
int	znajdz_dzialanie	args( ( char *op ) );
bool	uruchom_funkcje		args( ( CHAR_DATA *mob, char *nazwa, char *txt1,
					int *wart1, int *typ1, void **wsk1,
					char *txt2, int *wart2, int *typ2,
					void **wsk2, CHAR_DATA *actor,
					OBJ_DATA *obj, CHAR_DATA *rndm,
					CHAR_DATA *vict, OBJ_DATA *v_obj ) );
void	wykrec			args( ( STOSMP **stoswej, STOSMP **stoswyj ) );
void	pomin_stosmp		args( ( STOSMP **stos ) );
STOSMP	*licz_stosmp		args( ( STOSMP **stos, CHAR_DATA *mob,
					CHAR_DATA *actor, OBJ_DATA *obj,
					CHAR_DATA *rndm, CHAR_DATA *vict,
					OBJ_DATA *v_obj ) );
void	wymiec_smieci		args( ( STOSMP **s1, STOSMP **s2 ) );
void	int_aint		args( ( CHAR_DATA *ch, char *argument,
					CHAR_DATA *actor, OBJ_DATA *obj,
					CHAR_DATA *rndm, CHAR_DATA *vict,
					OBJ_DATA *v_obj, bool do_krainy ) );
void	str_astr		args( ( CHAR_DATA *ch, char *argument,
					CHAR_DATA *actor, OBJ_DATA *obj,
					CHAR_DATA *rndm, CHAR_DATA *vict,
					OBJ_DATA *v_obj, bool do_krainy ) );
void	chr_achr		args( ( CHAR_DATA *ch, char *argument,
					CHAR_DATA *actor, OBJ_DATA *obj,
					CHAR_DATA *rndm, CHAR_DATA *vict,
					OBJ_DATA *v_obj, bool do_krainy ) );
void	obj_aobj		args( ( CHAR_DATA *ch, char *argument,
					CHAR_DATA *actor, OBJ_DATA *obj,
					CHAR_DATA *rndm, CHAR_DATA *vict,
					OBJ_DATA *v_obj, bool do_krainy ) );

#define ILOSC_DZIALAN 14

/* musza byc wieksze od 0, kolejnosc nie ma znaczenia, nie moga sie powtarzac */
#define DZIAL_AND	 1
#define DZIAL_OR	 2
#define DZIAL_RAZY	 3
#define DZIAL_DZIEL	 4
#define DZIAL_DOD	 5
#define DZIAL_ODEJ	 6
#define DZIAL_ROWN	 7
#define DZIAL_NIEROWN	 8
#define DZIAL_MNIEJ	 9
#define DZIAL_WIEK	10
#define DZIAL_MNIEJROWN	11
#define DZIAL_WIEKROWN	12
#define DZIAL_I		13
#define DZIAL_LUB	14

/* w oblicz_wyrazenie na s1 pamietany jest numer dzialania z prio w ->typ,
   a przy przenoszeniu do s2 wrzucany jest typ z tabelki */
struct
{
    char znaczek[ 3 ];
    int prio;
    int typ;
} dzialania[ ILOSC_DZIALAN ] =
{
    { "&",	1,	DZIAL_AND	},
    { "|",	1,	DZIAL_OR	},
    { "*",	2,	DZIAL_RAZY	},
    { "/",	2,	DZIAL_DZIEL	},
    { "+",	3,	DZIAL_DOD	},
    { "-",	3,	DZIAL_ODEJ	},
    { "==",	4,	DZIAL_ROWN	},
    { "!=",	4,	DZIAL_NIEROWN	},
    { "<",	4,	DZIAL_MNIEJ	},
    { ">",	4,	DZIAL_WIEK	},
    { "<=",	4,	DZIAL_MNIEJROWN	},
    { ">=",	4,	DZIAL_WIEKROWN	},
    { "&&",	5,	DZIAL_I		},
    { "||",	5,	DZIAL_LUB	}
};


TYP_ZMIENNEJ *wolne_zmienne; /* lista usunietych zmiennych */

/*
 * Tworzy zmienna o zadanych wartosciach
 */
TYP_ZMIENNEJ *nowa_zmienna( char *nazwa, int typ, int wartosc, void *wsk )
{
    char buf[ MIL ];
    TYP_ZMIENNEJ *zm;

    if ( !wolne_zmienne )
    {
	zm = alloc_perm( sizeof( TYP_ZMIENNEJ ) );
	top_zmienna++;
    }
    else
    {
	zm = wolne_zmienne;
	wolne_zmienne = zm->nast;
    }

    strcpy( buf, nazwa );
    buf[ 30 ] = '\0';
    strcpy( zm->nazwa, buf );
    zm->typ = typ;
    if ( typ == TZ_CALK )
	zm->wartosc.i = wartosc;
    else
	zm->wartosc.p = wsk;

    return zm;
}


/*
 * Wpisuje zmienna na liste odzyskowa; jesli zmienna jest lancuchem, zwalnia po
 * nim pamiec
 */
void usun_zmienna( TYP_ZMIENNEJ *zm )
{
    char *lanc;

    zm->nast = wolne_zmienne;
    wolne_zmienne = zm;
    switch ( zm->typ )
    {
	default:
		bug( "usun_zmienna: zly typ zmiennej %d", zm->typ );
		return;
	case TZ_CALK:
	case TZ_POST:
	case TZ_PRZE:
		zm->wartosc.p = NULL;
		break;
	case TZ_ZNAK:
		lanc = (char *) zm->wartosc.p;
		free_string( lanc );
		zm->wartosc.i = 0;
		break;
    }
    zm->typ = TZ_CALK;
    zm->nazwa[ 0 ] = '\0';

    return;
}


/*
 * Zabieranie zwod postaci, ktora byla zmienna na liscie, tylko jesli zm to
 * ostatnia zmienna - mozna uzywac po usunieciu zm lub przed zmiana
 */
void usun_zm_zwod( TYP_ZMIENNEJ **lista, TYP_ZMIENNEJ *zm, CHAR_DATA *mob )
{
    TYP_ZMIENNEJ *pzl;
    CHAR_DATA *ch;

    if ( mob && zm->typ == TZ_POST )
    {
	for ( pzl = *lista; pzl; pzl = pzl->nast )
	    if ( pzl != zm
	      && pzl->wartosc.p == zm->wartosc.p )
		break;

	/* czy to byla ostatnia zmienna na tej liscie pokazujaca na ofiare? */
	if ( !pzl )
	{
	    ch = zm->wartosc.p;
/*	    ch_printf( ch, "Wywalam ci zwod, zmienna %s.\n\r", zm->nazwa ); */
	    wywal_zwod( &ch->zwod_zmienne, mob );
	}
    }

    return;
}


/*
 * Usuwa z listy, ew. zmieniajac takze liste (dlatego ma taki typ)
 */
void usun_z_listy( TYP_ZMIENNEJ **lista, TYP_ZMIENNEJ *zm, CHAR_DATA *mob )
{
    TYP_ZMIENNEJ *pzl;

    if ( !zm )
    {
	bug( "usun_z_listy: !zm", 0 );
	return;
    }

    if ( *lista == zm )
	*lista = zm->nast;
    else
    {
	for ( pzl = *lista; pzl; pzl = pzl->nast )
	    if ( pzl->nast == zm )
		break;

	if ( !pzl )
	{
	    bug( "usun_z_listy: nie znalazlem zmiennej w liscie", 0 );
	    return;
	}

	pzl->nast = zm->nast;
    }

    usun_zm_zwod( lista, zm, mob );
    usun_zmienna( zm );

    return;
}


/*
 * Szuka zmiennej o zadanej nazwie w podanej liscie
 */
TYP_ZMIENNEJ *znajdz_zmienna( TYP_ZMIENNEJ *lista, char *nazwa )
{
    TYP_ZMIENNEJ *zm;

    for ( zm = lista; zm; zm = zm->nast )
	if ( !str_cmp( zm->nazwa, nazwa ) )
	    return zm;

    return NULL;
}


/*
 * Ustawia zmiennej zadana wartosc. jesli zmienna nie istnieje, tworzy ja.
 * Zauwaz uzycie str_dup dla zmiennych znakowych
 */
void zmien_zmienna( TYP_ZMIENNEJ **lista, char *nazwa, int typ,
		int wartosc, void *wsk, CHAR_DATA *mob )
{
    TYP_ZMIENNEJ *zm = znajdz_zmienna( *lista, nazwa );
    char *lanc;

    if ( typ == TZ_ZNAK )
    {
	if ( !zm )
	{
	    zm = nowa_zmienna( nazwa, typ, 0, str_dup( wsk ) );
	    zm->nast = *lista;
	    *lista = zm;
	}
	else
	{
	    if ( zm->typ == TZ_ZNAK )
	    {
		lanc = zm->wartosc.p;
		free_string( lanc );
	    }
	    usun_zm_zwod( lista, zm, mob );
	    zm->wartosc.p = str_dup( wsk );
	}
    }
    else if ( typ == TZ_CALK )
    {
	if ( !zm )
	{
	    zm = nowa_zmienna( nazwa, typ, wartosc, wsk );
	    zm->nast = *lista;
	    *lista = zm;
	}
	else
	{
	    if ( zm->typ == TZ_ZNAK )
	    {
		lanc = zm->wartosc.p;
		free_string( lanc );
	    }
	    usun_zm_zwod( lista, zm, mob );
	    zm->wartosc.i = wartosc;
	}
    }
    else if ( typ == TZ_POST
	   || typ == TZ_PRZE )
    {
	if ( !zm )
	{
	    zm = nowa_zmienna( nazwa, typ, wartosc, wsk );
	    zm->nast = *lista;
	    *lista = zm;
	}
	else
	{
	    if ( zm->typ == TZ_ZNAK )
	    {
		lanc = zm->wartosc.p;
		free_string( lanc );
	    }
	    usun_zm_zwod( lista, zm, mob );
	    zm->wartosc.p = wsk;
	}
    }
    else
    {
	bug( "zmien_zmienna: nieznany typ %d", typ );
	return;
    }

    zm->typ = typ;

    return;
}


/*
 * Stos dla oblicz_wyrazenie. top_stosmp w db.c, ale liste zostawiam sobie tu
 */
STOSMP *stos_wolnych;

STOSMP *nowy_kawalek_stosmp( void )
{
    STOSMP *kawalek;

    if ( stos_wolnych )
    {
	kawalek = stos_wolnych;
	stos_wolnych = kawalek->nast;
    }
    else
    {
	kawalek = alloc_perm( sizeof( STOSMP ) );
	top_stosmp++;
    }

    kawalek->typ = 0;
    kawalek->wart = 0;
    kawalek->wsk = NULL;
    kawalek->txt[ 0 ] = '\0';

    return kawalek;
}


void wywal_kawalek_stosmp( STOSMP *kawalek )
{
    kawalek->nast = stos_wolnych;
    stos_wolnych = kawalek;

    return;
}


void na_stosmp( STOSMP **stos, STOSMP *kawalek )
{
    kawalek->nast = *stos;
    *stos = kawalek;

    return;
}


void na_koniec_stosmp( STOSMP **glowa_stos, STOSMP *kawalek )
{
    kawalek->nast = NULL;
    ( *glowa_stos )->nast = kawalek;
    *glowa_stos = kawalek;

    return;
}


STOSMP *ze_stosmp( STOSMP **stos )
{
    STOSMP *kawalek;

    kawalek = *stos;
    *stos = kawalek->nast;

    return kawalek;
}


/* ----------------================<*> ! <*>================---------------- */
/*
 * Sprawdza, czy lancuch moze byc nazwa zmiennej
 */
bool czy_nazwa_zmiennej( char *nazwa )
{
    if ( *nazwa == '!' ) /* moze byc zanegowana */
	nazwa++;

    if ( !isalpha( (int) *nazwa ) && *nazwa != '_' ) /* pierwsza musi byc litera/_ */
	return FALSE;

    return TRUE; /* wszystkie inne przypadki moga (na razie) byc zmiennymi */
}


/*
 * Sprawdza, czy lancuch jest liczba
 */
bool czy_liczba( char *nazwa )
{
    if ( isalpha( (int) *nazwa ) )
	return FALSE;

    /* ~ - + i podobne powinny miec mozliwosc bycia obsluzonymi przez funkcje
       analogiczna do 'atoi', ale tylko na ~ pozwalaja obecnie stosowane funkcje
       (trzebaby je pisac od nowa z uwzglednieniem ujemnych i dodatnich, ale
       i tak nie trzeba pisac "+", a -10 mozna zrobic poprzez ( 0 - 10 )) */
    if ( *nazwa == '~' )
	return TRUE;

    /* to, czy liczba rzeczywiscie jest liczba, musi sprawdzac funkcja
       przeliczajaca systemy (kiedy powstanie) */
    if ( isdigit( (int) *nazwa ) )
	return TRUE;

    return FALSE;
}


/*
 * Sprawdza, czy dany zestaw znakow nadaje sie tuz po nazwie zmiennej
 */
bool czy_zdatny_operator_int( char *op )
{
    const char *ops[ 7 ] = { "=", "-=", "+=", "*=", "/=", "&=", "|=" };
    int i;

    for ( i = 0; i < 7; i++ )
	if ( !str_cmp( op, ops[ i ] ) )
	    return TRUE;

    return FALSE;
}


bool czy_zdatny_operator_str( char *op )
{
    const char *ops[ 2 ] = { "=", "+=" };
    int i;

    for ( i = 0; i < 2; i++ )
	if ( !str_cmp( op, ops[ i ] ) )
	    return TRUE;

    return FALSE;
}


/*
 * Szuka dzialania. Sprawdza, czy dany zestaw znaczkow nadaje sie wewnatrz
 * wyrazenia. Zwraca pozycje dzialania w tablicy albo -1.
 */
int znajdz_dzialanie( char *op )
{
    int i;

    for ( i = 0; i < ILOSC_DZIALAN; i++ )
	if ( !str_cmp( op, dzialania[ i ].znaczek ) )
	    return i;

    return -1;
}


/*
 * Zwraca nastepny skladnik wyrazenia (nazwe zmiennej, liczbe, dzialanie).
 * Parametry takie same jak w one_argument
 */
#define POMIN while ( *c && strchr( sep2, *c ) ) c++;
char *nastepny_skladnik( char *wyrazenie, char *skladnik )
{
    const char *sep1 = "+-*/=|&!<>"; /* dzialania, uwaga na ! */
    const char *sep2 = " \t\n\r"; /* separatory */
    const char *sep3 = "()"; /* nawiasy; moze kwadratowe tez? */
    const char *sep4 = "'\""; /* oznaczenie lancucha */
    int czy_dzialanie; /* czego szukamy? */
    char *c = wyrazenie;
    char buf1[ MIL ];
    char *b = buf1;
    char txt = '\0';

    if ( !c || !*c )
    {
	*skladnik = '\0';
	return c;
    }

    POMIN

    if ( !*c )
    {
	*skladnik = '\0';
	return c;
    }

    if ( strchr( sep3, *c ) ) /* kazdy nawias jest oddzielny */
    {
	*skladnik = *c;
	skladnik++, c++;
	*skladnik = '\0';
	POMIN
	return c;
    }
    else if ( *c == ',' ) /* przecinek jw. */
    {
	*skladnik = *c;
	skladnik++, c++;
	*skladnik = '\0';
	POMIN
	return c;
    }
    else if ( strchr( sep4, *c ) ) /* oznaczenie tekstu */
    {
	*skladnik++ = *c; /* oblicz_wyrazenie dostanie znak, ze to lancuch */
	txt = *c++;
    }

    czy_dzialanie = FALSE;
    if ( strchr( sep1, *c ) )
    {
	if ( c[ 0 ] != '!' || !isalpha( (int) c[ 1 ] ) )
	    czy_dzialanie = TRUE;
    }

    if ( txt ) /* sprawdzane na poczatku, zeby zignorowac czy_dzialanie */
    {
	while ( ( *b = *c ) )
	    if ( *b == txt )
		break;
	    else
		b++, c++;

	if ( *b )
	{
	    POMIN
	    *b = '\0';
	    c++;
	}
	strcpy( skladnik, buf1 );
	return c;
    }
    else if ( czy_dzialanie )
    {
	while ( ( *b = *c ) )
	    /* - dzialanie typu = = dzialalo jak ==, && ! jak &&! itd.
	    if ( strchr( sep2, *c ) )
		POMIN
	    else */
	    if ( !strchr( sep1, *c ) )
		break;
	    else
		b++, c++;

	if ( *b )
	{
	    POMIN
	    *b = '\0';
	}
	strcpy( skladnik, buf1 );
	return c;
    }
    else
    {
	if ( *c == '!' )
	{
	    *b = *c;
	    b++;
	    c++;
	}

	while ( ( *b = *c )
	     && !strchr( sep1, *c )
	     && !strchr( sep2, *c )
	     && !strchr( sep3, *c )
	     && ( *c != ',' ) ) /* specjalny separator - przecinek */
		b++, c++;

	if ( *b )
	{
	    POMIN
	    *b = '\0';
	}
	strcpy( skladnik, buf1 );
	return c;
    }

    bug( "nastepny_skladnik() nie powinno tu wejsc!", 0 );
    *skladnik = '\0';

    return wyrazenie;
}


#define ZLA_FUNKCJA {								\
	progbug( mob, "uruchom_funkcje: zla funkcja/argumenty: %s()", nazwa );	\
	return TRUE; }

/*
 * Argumentow niemalo...
 * Zwraca informacje, czy wystapil blad (oblicz_wyrazenie powinno od razu
 * wyskoczyc i zwrocic zero. progbug lokalnie, bo tu wiecej wiadomo)
 */
bool uruchom_funkcje( CHAR_DATA *mob, char *nazwa,
		char *txt1, int *wart1, int *typ1, void **wsk1,
		char *txt2, int *wart2, int *typ2, void **wsk2,
		CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    CHAR_DATA *ifchr = NULL;
    OBJ_DATA *ifobj = NULL;
    int zwrot_typ = 0; /* ustawia sie samo na -1 jesli zwrot_txt[ 0 ] */
    int zwrot_wart = 0;
    void *zwrot_wsk = NULL;
    static char zwrot_txt[ MIL * 2 ];
    bool neguj = FALSE;
    int arg = *wart1;

    zwrot_txt[ 0 ] = '\0';

    if ( *nazwa == '!' )
    {
	neguj = TRUE;
	nazwa++;
    }

    if ( *typ1 == -1 && !*typ2 )
    {
	if ( !str_cmp( nazwa, "varexists" ) )
		zwrot_wart = znajdz_zmienna_moba( mob, txt1 ) ? 1 : 0;
	else if ( !str_cmp( nazwa, "strlen" ) )
		zwrot_wart = strlen( txt1 );
	else if ( !str_cmp( nazwa, "atoi" ) )
		zwrot_wart = atoi( txt1 );
	else if ( !str_cmp( nazwa, "czybluzg" ) )
		zwrot_wart = czy_bluzg( txt1 ) ? TRUE : FALSE;
	else if ( !str_cmp( nazwa, "get_char_room" ) )
	{
	    CHAR_DATA *znajda = get_char_room( mob, txt1 );
	    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
	    zwrot_wsk = znajda;
	    zwrot_typ = -3;
	}
	else if ( !str_cmp( nazwa, "get_char_room2" ) )
	{
	    CHAR_DATA *znajda = get_char_room2( mob, txt1 );
	    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
	    zwrot_wsk = znajda;
	    zwrot_typ = -3;
	}
	else if ( !str_cmp( nazwa, "get_char_world" ) )
	{
	    CHAR_DATA *znajda = get_char_world( mob, txt1 );
	    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
	    zwrot_wsk = znajda;
	    zwrot_typ = -3;
	}
	else if ( !str_cmp( nazwa, "get_char_world2" ) )
	{
	    CHAR_DATA *znajda = get_char_world2( mob, txt1 );
	    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
	    zwrot_wsk = znajda;
	    zwrot_typ = -3;
	}
	else if ( !str_cmp( nazwa, "get_char_world_pc" ) )
	{
	    CHAR_DATA *znajda = get_char_world_pc( mob, txt1 );
	    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
	    zwrot_wsk = znajda;
	    zwrot_typ = -3;
	}
	else if ( !str_cmp( nazwa, "get_char_area" ) )
	{
	    CHAR_DATA *znajda = get_char_area( mob, txt1 );
	    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
	    zwrot_wsk = znajda;
	    zwrot_typ = -3;
	}
	else if ( !str_cmp( nazwa, "get_char_area2" ) )
	{
	    CHAR_DATA *znajda = get_char_area2( mob, txt1 );
	    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
	    zwrot_wsk = znajda;
	    zwrot_typ = -3;
	}
	else if ( !str_cmp( nazwa, "dirnum" ) )
	{
	    /* fragment skopiowany z db.c */
	    switch ( *txt1 )
	    {
		default:
		    zwrot_wart = -1;
		    break;
		case 'n': case 'N':
		    if ( txt1[ 1 ] == 'e' || txt1[ 1 ] == 'E' )
			zwrot_wart = 6;
		    else if ( txt1[ 1 ] == 'w' || txt1[ 1 ] == 'W' )
			zwrot_wart = 8;
		    else zwrot_wart = 0;
		    break;
		case 's': case 'S':
		    if ( txt1[ 1 ] == 'e' || txt1[ 1 ] == 'E' )
			zwrot_wart = 7;
		    else if ( txt1[ 1 ] == 'w' || txt1[ 1 ] == 'W' )
			zwrot_wart = 9;
		    else zwrot_wart = 2;
		    break;
		case 'e': case 'E': zwrot_wart = 1; break;
		case 'w': case 'W': zwrot_wart = 3; break;
		case 'u': case 'U':
		case 'g': case 'G': zwrot_wart = 4; break;
		case 'd': case 'D': zwrot_wart = 5; break;
	    }
	}
	else ZLA_FUNKCJA;
    }
    else if ( *typ1 == -1 && *typ2 == -1 )
    {
	if ( !str_cmp( nazwa, "str_cmp" ) )
		zwrot_wart = str_cmp( txt1, txt2 );
	else if ( !str_cmp( nazwa, "str_prefix" ) )
		zwrot_wart = str_prefix( txt1, txt2 );
	else if ( !str_cmp( nazwa, "str_infix" ) )
		zwrot_wart = str_infix( txt1, txt2 );
	else if ( !str_cmp( nazwa, "str_suffix" ) )
		zwrot_wart = str_suffix( txt1, txt2 );
	else if ( !str_cmp( nazwa, "one_argument" ) )
	{
	    char tymek[ MIL ];
	    strcpy( zwrot_txt, one_argument2( txt1, tymek ) );
	    zwrot_typ = -1; /* w przypadku pustego zwrot_txt trzeba ustawic recznie */
	    zmien_zmienna( &mob->zmienne, txt2, TZ_ZNAK, 0, tymek, mob );
	}
	else if ( !str_cmp( nazwa, "laccrypt" ) )
		strcpy( zwrot_txt, lac_sha1_crypt( txt1, txt2 ) );
	else ZLA_FUNKCJA;
    }
    else if ( !*typ1 && !*typ2 )
    {
	/* bez zadnych argumentow */
	if ( !str_cmp( nazwa, "secttype" ) )
		zwrot_wart = mob->in_room->sector_type;
	else if ( !str_cmp( nazwa, "onground" ) )
		zwrot_wart = CZY_ZIEMIA( mob->in_room ) ? TRUE : FALSE;
	else if ( !str_cmp( nazwa, "inwater" ) )
		zwrot_wart = CZY_WODA( mob->in_room ) ? TRUE : FALSE;
	else if ( !str_cmp( nazwa, "seabed" ) )
		zwrot_wart = CZY_DNO_POD_WODA( mob->in_room ) ? TRUE : FALSE;
	else if ( !str_cmp( nazwa, "inair" ) )
		zwrot_wart = CZY_POWIETRZE( mob->in_room ) ? TRUE : FALSE;
/*	else if ( !str_cmp( nazwa, "licz_graczy" ) )
		zwrot_wart = licz_graczy( ); - pokazywaloby razem z BN */
	else if ( !str_cmp( nazwa, "hour" ) )
		zwrot_wart = mob->in_room->area->time_info.hour;
	else if ( !str_cmp( nazwa, "atime" ) )
		zwrot_wart = wiek_krainy( mob->in_room->area );
	else if ( !str_cmp( nazwa, "mmhg" ) )
		zwrot_wart = mob->in_room->area->weather_info.mmhg;
	else if ( !str_cmp( nazwa, "sun" ) )
		zwrot_wart = mob->in_room->area->weather_info.sunlight;
	else if ( !str_cmp( nazwa, "sunlight" ) )
		zwrot_wart = mob->in_room->area->weather_info.sunlight;
	else if ( !str_cmp( nazwa, "sky" ) )
		zwrot_wart = mob->in_room->area->weather_info.sky;
	else if ( !str_cmp( nazwa, "wind" ) )
		zwrot_wart = mob->in_room->area->weather_info.change < 0 ? TRUE : FALSE;
	else if ( !str_cmp( nazwa, "random" ) ) /* Lam: rand() historycznie, random() dla MobC */
		zwrot_wart = number_percent( );
	else if ( !str_cmp( nazwa, "tm_hour" ) )
		zwrot_wart = localtime( &current_time )->tm_hour;
	else if ( !str_cmp( nazwa, "tm_min" ) )
		zwrot_wart = localtime( &current_time )->tm_min;
	else if ( !str_cmp( nazwa, "tm_sec" ) )
		zwrot_wart = localtime( &current_time )->tm_sec;
	else if ( !str_cmp( nazwa, "tm_wday" ) )
		zwrot_wart = localtime( &current_time )->tm_wday;
	else if ( !str_cmp( nazwa, "tm_mday" ) )
		zwrot_wart = localtime( &current_time )->tm_mday;
	else if ( !str_cmp( nazwa, "tm_yday" ) )
		zwrot_wart = localtime( &current_time )->tm_yday;
	else if ( !str_cmp( nazwa, "tm_mon" ) )
		zwrot_wart = localtime( &current_time )->tm_mon;
	else if ( !str_cmp( nazwa, "tm_year" ) )
		zwrot_wart = localtime( &current_time )->tm_year;
	else if ( !str_cmp( nazwa, "get_mob_vnum_room" ) )
	{
	    CHAR_DATA *znajda;
	    for ( znajda = mob->in_room->people; znajda; znajda = znajda->next_in_room )
		if ( znajda->pIndexData && znajda->pIndexData->vnum == arg && can_see( mob, znajda ) )
		    break;
	    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
	    zwrot_wsk = znajda;
	    zwrot_typ = -3;
	}
	else if ( !str_cmp( nazwa, "get_first_char_room" ) )
	{
	    CHAR_DATA *znajda = mob->in_room->people;
	    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
	    zwrot_wsk = znajda;
	    zwrot_typ = -3;
	}

	/* jeden lub dwa argumenty liczbowe */
	else if ( !str_cmp( nazwa, "rand" ) )
		zwrot_wart = ( number_percent( ) <= arg );
	else if ( !str_cmp( nazwa, "itoa" ) )
		sprintf( zwrot_txt, "%d", arg );
	else if ( !str_cmp( nazwa, "liczba" ) )
		zwrot_wart = liczba( arg );
	else if ( !str_cmp( nazwa, "itopl" ) )
		strcpy( zwrot_txt, itopl( arg, *wart2 ) );
	else if ( !str_cmp( nazwa, "doorvnum" ) ) /* Lam */
	{
	    ROOM_INDEX_DATA *gdzie;
	    int door;
	    /* czyli nie mozna z oddali sprawdzac pomieszczen o vnumach
	       0-9 w kierunku 0 (polnoc) :) */
	    if ( !*wart2 && arg < 10 )
	    {
		gdzie = mob->in_room;
		door = arg;
	    }
	    else
	    {
		gdzie = get_room_index( arg );
		if ( !gdzie )
		{
		    progbug( mob, "doorvnum(%s,%s): nie ma pomieszczenia %s", txt1, txt2, txt1 );
		    return TRUE;
		}
		door = *wart2;
	    }
	    if ( door < 0 || door > 9 )
	    {
		progbug( mob, "doorvnum(%s,%s): bledny kierunek %d", txt1, txt2, door );
		return TRUE;
	    }
	    zwrot_wart = gdzie->exit[ door ] ? gdzie->exit[ door ]->vnum : 0;
	}
	else if ( !str_cmp( nazwa, "isclosed" ) ) /* Lam */
	{
	    ROOM_INDEX_DATA *gdzie;
	    int door;

	    if ( !*wart2 && arg < 10 )
	    {
		gdzie = mob->in_room;
		door = arg;
	    }
	    else
	    {
		gdzie = get_room_index( arg );
		if ( !gdzie )
		{
		    progbug( mob, "isclosed(%s,%s): nie ma pomieszczenia %s", txt1, txt2, txt1 );
		    return TRUE;
		}
		door = *wart2;
	    }
	    if ( door < 0 || door > 9 )
	    {
		progbug( mob, "isclosed(%s,%s): bledny kierunek %d", txt1, txt2, door );
		return TRUE;
	    }
	    if ( !gdzie->exit[ door ] )
	    {
		progbug( mob, "isclosed(%s,%s): nie ma wyjscia w tym kierunku", txt1, txt2 );
		return TRUE;
	    }
	    zwrot_wart = IS_SET( gdzie->exit[ door ]->exit_info, EX_CLOSED )
			? TRUE : FALSE;
	}
	else if ( !str_cmp( nazwa, "islocked" ) ) /* Lam */
	{
	    ROOM_INDEX_DATA *gdzie;
	    int door;

	    if ( !*wart2 && arg < 10 )
	    {
		gdzie = mob->in_room;
		door = arg;
	    }
	    else
	    {
		gdzie = get_room_index( arg );
		if ( !gdzie )
		{
		    progbug( mob, "islocked(%s,%s): nie ma pomieszczenia %s", txt1, txt2, txt1 );
		    return TRUE;
		}
		door = *wart2;
	    }
	    if ( door < 0 || door > 9 )
	    {
		progbug( mob, "islocked(%s,%s): bledny kierunek %d", txt1, txt2, door );
		return TRUE;
	    }
	    if ( !gdzie->exit[ door ] )
	    {
		progbug( mob, "islocked(%s,%s): nie ma wyjscia w tym kierunku", txt1, txt2 );
		return TRUE;
	    }
	    zwrot_wart = IS_SET( gdzie->exit[ door ]->exit_info, EX_LOCKED )
			? TRUE : FALSE;
	}
	else if ( !str_cmp( nazwa, "mload" ) ) /* Lam 4.1.2003 */
	{
	    MOB_INDEX_DATA *pMobIndex;
	    CHAR_DATA      *victim;

	    if ( arg == 3 || arg == 0 )
		return TRUE;

	    if ( !( pMobIndex = get_mob_index( arg ) ) )
		return TRUE;

	    victim = create_mobile( pMobIndex );
	    char_to_room( victim, mob->in_room );
	    zwrot_wsk = victim;
	    zwrot_typ = -3;
	}
	else if ( !str_cmp( nazwa, "oload" ) ) /* Lam 2.10.2004 */
	{
	    OBJ_INDEX_DATA *pObjIndex;
	    OBJ_DATA       *tobj;
	    int            pzm;

	    if ( !arg )
		return TRUE;

	    pzm = *wart2;
	    if ( pzm <= 0 || pzm > get_trust( mob ) )
		pzm = get_trust( mob );

	    if ( !( pObjIndex = get_obj_index( arg ) ) )
		return TRUE;

	    tobj = create_object( pObjIndex, pzm );
	    if ( CAN_WEAR( tobj, ITEM_TAKE )
	      && !IS_OBJ_STAT( pObjIndex, ITEM_BURIED ) )
	    {
		obj_to_char( tobj, mob );
	    }
	    else
		obj_to_room( tobj, mob->in_room );

	    zwrot_wsk = tobj;
	    zwrot_typ = -4;
	}
	else if ( !str_cmp( nazwa, "isdeathtrap" ) ) /* Lam */
	{
	    ROOM_INDEX_DATA *gdzie = get_room_index( arg );

	    if ( !gdzie )
	    {
		progbug( mob, "if isdeathtrap(%s): nie ma takiego pomieszczenia", txt1 );
		return TRUE;
	    }
	    zwrot_wart = gdzie->trap ? TRUE : FALSE;
	}
	else if ( !str_cmp( nazwa, "roomisdark" ) ) /* Lam 7.5.2000 */
	{
	    ROOM_INDEX_DATA *gdzie = get_room_index( arg );

	    if ( !gdzie )
	    {
		progbug( mob, "if roomisdark(%s): nie ma takiego pomieszczenia", txt1 );
		return TRUE;
	    }
	    zwrot_wart = room_is_dark( gdzie );
	}
	else if ( !str_cmp( nazwa, "roomisprivate" ) ) /* Lam 7.5.2000 */
	{
	    ROOM_INDEX_DATA *gdzie = get_room_index( arg );

	    if ( !gdzie )
	    {
		progbug( mob, "if roomisprivate(%s): nie ma takiego pomieszczenia", txt1 );
		return TRUE;
	    }
	    zwrot_wart = room_is_private( gdzie );
	}
	else if ( !str_cmp( nazwa, "samearea" ) )
	{
	    ROOM_INDEX_DATA *gdzie = get_room_index( arg );

	    if ( !gdzie )
	    {
		progbug( mob, "if samearea(%s): nie ma takiego pomieszczenia", txt1 );
		return FALSE;
	    }
	    zwrot_wart = ( gdzie->area == mob->in_room->area ) ? TRUE : FALSE;
	}
	else if ( !str_cmp( nazwa, "roomexists" ) )
	{
	    ROOM_INDEX_DATA *gdzie = get_room_index( arg );

	    zwrot_wart = gdzie ? TRUE : FALSE;
	}
	else if ( !str_cmp( nazwa, "ismobhere" ) ) /* Lam */
	{
	    /* if ismobhere( ) sprawdza czy w tym samym pomieszczeniu
	       znajduje sie mob o vnumie podanym w nawiasach */
	    CHAR_DATA *tmpch;

	    for ( tmpch = mob->in_room->people; tmpch; tmpch = tmpch->next_in_room )
		if ( !tmpch->deleted && IS_NPC( tmpch )
		  && tmpch->pIndexData->vnum == arg )
		    zwrot_wart = TRUE;
	}
	else if ( !str_cmp( nazwa, "isobjhere" ) ) /* Lam */
	{
	    /* if isobjhere( ) sprawdza czy w tym samym pomieszczeniu
	       znajduje sie przedmiot o vnumie podanym w nawiasach */
	    OBJ_DATA *tmpobj;
	    for ( tmpobj = mob->in_room->contents; tmpobj; tmpobj = tmpobj->next_content )
		if ( !tmpobj->deleted && tmpobj->pIndexData->vnum == arg )
		{
		    zwrot_wart = TRUE;
		    break;
		}
	}
	else if ( !str_cmp( nazwa, "canseeobjhere" ) ) /* Lam 29.1.2004 */
	{
	    /* wersja powyzszego sprawdzajaca, czy mob widzi przedmiot */
	    OBJ_DATA *tmpobj;

	    for ( tmpobj = mob->in_room->contents; tmpobj; tmpobj = tmpobj->next_content )
		if ( !tmpobj->deleted
		  && tmpobj->pIndexData->vnum == arg
		  && can_see_obj( mob, tmpobj ) )
		{
		    zwrot_wart = TRUE;
		    break;
		}
	}
	else if ( !str_cmp( nazwa, "isobjinv" ) ) /* Lam 29.1.2004 */
	{
	    /* if isobjinv( ) sprawdza czy w inwentarzu moba znajduje sie
	       przedmiot o vnumie podanym w nawiasach */
	    OBJ_DATA *tmpobj;

	    for ( tmpobj = mob->carrying; tmpobj; tmpobj = tmpobj->next_content )
		if ( !tmpobj->deleted
		  && tmpobj->pIndexData->vnum == arg
		  && tmpobj->wear_loc == WEAR_NONE
		  && can_see_obj( mob, tmpobj ) )
		{
		    zwrot_wart = TRUE;
		    break;
		}
	}
	else if ( !str_cmp( nazwa, "isobjeq" ) ) /* Lam 29.1.2004 */
	{
	    /* if isobjeq( ) sprawdza czy w ekwipunku moba znajduje sie
	       przedmiot o vnumie podanym w nawiasach */
	    OBJ_DATA *tmpobj;

	    for ( tmpobj = mob->carrying; tmpobj; tmpobj = tmpobj->next_content )
		if ( !tmpobj->deleted
		  && tmpobj->pIndexData->vnum == arg
		  && tmpobj->wear_loc != WEAR_NONE
		  && can_see_obj( mob, tmpobj ) )
		{
		    zwrot_wart = TRUE;
		    break;
		}
	}
	else
	    ZLA_FUNKCJA;
    }
/*
    else if ( *typ1 == -2
	   && *typ2 == -2 )
    {
    }
*/
    else if ( *typ1 == -2 || *typ1 == -3 || *typ1 == -4 )
    {
	/*
	 * Lam 15.6.98
	 * Wzorowalem sie na Smaugu. Oczywiscie ja napisalem to lepiej ;)
	 */
	if ( *typ1 == -3 )
	    ifchr = (CHAR_DATA *) *wsk1;
	else if ( *typ1 == -4 )
	    ifobj = (OBJ_DATA *) *wsk1;
	else switch ( txt1[ 1 ] )
	{
	    case 'i':  ifchr = mob;	break;
	    case 'n':  ifchr = actor;	break;
	    case 'r':  ifchr = rndm;	break;
	    case 'o':  ifobj = obj;	break;
	    case 't':  ifchr = vict;	break;
	    case 'p':  ifobj = v_obj;	break;
	    default:  progbug( mob, "zly znak po $ w ifchecku: %s(%s,%s)", nazwa, txt1, txt2 );
		return TRUE;
	}

	/* ponizsze ifchecki wymagaja przynajmniej jednego */
	if ( !ifchr && !ifobj )
	{
	    if ( !str_cmp( nazwa, "deleted" ) )
		zwrot_wart = TRUE;
	    else
		ZLA_FUNKCJA;
	}

	/* Lam 15.6.98: to, co tu zajmuje 2 ekrany, wczesniej zajmowalo 17 KiB
	   tekstu... Teraz jest mniejsze i latwiejsze w rozbudowie.
	   Lam 14.10.99: MobC, przerzucenie calosci z mob_prog.c */

	/* Lam: musi byc to TRUE : FALSE ze wzgledu na fakt, ze powinnismy
	   zwracac 0 albo 1 a nie 0 albo cokolwiek */
	if ( ifchr )
	{
		if ( !str_cmp( nazwa, "deleted" ) )		zwrot_wart = ifchr->deleted;
		else if ( !str_cmp( nazwa, "cansee" ) )		zwrot_wart = can_see( mob, ifchr ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "cantalk" ) )	zwrot_wart = CAN_TALK( ifchr ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "canhear" ) )	{ ROOM_INDEX_DATA *gdzie = *wart2 ? get_room_index( *wart2 ) : ifchr->in_room;
					if ( !gdzie ) { progbug( mob, "if canhear(%s,%s): nie ma takiego pomieszczenia", txt1, txt2 ); return TRUE; }
					zwrot_wart = CAN_HEAR_ROOM( ifchr, gdzie ) ? TRUE : FALSE; }
		else if ( !str_cmp( nazwa, "cansmell" ) )	zwrot_wart = CAN_SMELL( ifchr ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "is_safe" ) )	zwrot_wart = is_safe( mob, ifchr, FALSE ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "name_expand" ) )	strcpy( zwrot_txt, name_expand( mob, ifchr ) );
		else if ( !str_cmp( nazwa, "clan" ) )		if ( !ifchr->pcdata->clan ) strcpy( zwrot_txt, "-" );
								else strcpy( zwrot_txt, (char *) ifchr->pcdata->clan->name );
		else if ( !str_cmp( nazwa, "clanstat" ) )	zwrot_wart = get_clanstat( ifchr );
		else if ( !str_cmp( nazwa, "zone" ) )		strcpy( zwrot_txt, POCHODZENIE_M( ifchr ) );
		else if ( !str_cmp( nazwa, "recall" ) )		zwrot_wart = PRZYWOLANIE_P( ifchr );
		else if ( !str_cmp( nazwa, "temple" ) )		zwrot_wart = SWIATYNIA( ifchr );
		else if ( !str_cmp( nazwa, "ispc" ) )		zwrot_wart = IS_NPC( ifchr ) ? FALSE : TRUE;
		else if ( !str_cmp( nazwa, "isnpc" ) )		zwrot_wart = IS_NPC( ifchr ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "isgood" ) )		zwrot_wart = IS_GOOD( ifchr ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "isneutral" ) )	zwrot_wart = IS_NEUTRAL( ifchr ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "isevil" ) )		zwrot_wart = IS_EVIL( ifchr ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "isfight" ) )	zwrot_wart = ifchr->fighting ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "ishero" ) )		zwrot_wart = ( !IS_NPC( ifchr ) && ifchr->level >= LEVEL_HERO ) ? TRUE : FALSE; /* Lam 23.9.2000, 27.1.2004: falsz dla mobow */
		else if ( !str_cmp( nazwa, "isimmort" ) )	zwrot_wart = ( !IS_NPC( ifchr ) && ifchr->level >= LEVEL_IMMORTAL ) ? TRUE : FALSE; /* Lam 30.4.2000: > na >=, 27.1.2004: zaufanie na poziom, falsz dla mobow */
		else if ( !str_cmp( nazwa, "ischarmed" ) )	zwrot_wart = IS_AFFECTED( ifchr, AFF_CHARM ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "isfollow" ) )	zwrot_wart = ( ifchr->master != NULL && ifchr->master->in_room == ifchr->in_room ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "isactflag" ) )	zwrot_wart = ( ifchr->act & ( 1 << *wart2 ) ) ? TRUE : FALSE; /* Lam 16.1.2003 */
		else if ( !str_cmp( nazwa, "isaffected" ) )	zwrot_wart = IS_AFFECTED( ifchr, *wart2 ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "isblind" ) )	zwrot_wart = !can_see_room( ifchr, ifchr->in_room ); /* Lam */
		else if ( !str_cmp( nazwa, "isflying" ) )	zwrot_wart = IS_FLYING( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "hasinfra" ) )	zwrot_wart = HAS_INFRAVISION( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "hasdethidden" ) )	zwrot_wart = HAS_DETECT_HIDDEN( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "hasdetinvis" ) )	zwrot_wart = HAS_DETECT_INVIS( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "hasdetevil" ) )	zwrot_wart = HAS_DETECT_EVIL( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "haspart" ) )	zwrot_wart = ( race_table[ ifchr->race ].parts & ( 1 << *wart2 ) ) ? TRUE : FALSE; /* Lam 10.1.2009 */
		else if ( !str_cmp( nazwa, "ispassing" ) )	zwrot_wart = IS_PASSING( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "hasprotection" ) )	zwrot_wart = HAS_PROTECTION( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "hasprotgood" ) )	zwrot_wart = HAS_PROTECT_GOOD( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "hassanctuary" ) )	zwrot_wart = HAS_SANCTUARY( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "savesspell" ) )	zwrot_wart = saves_spell( *wart2, ifchr );
		else if ( !str_cmp( nazwa, "isvampire" ) )	zwrot_wart = CZY_WAMPIR( ifchr ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "hitprcnt" ) )	zwrot_wart = ifchr->hit * 100 / UMAX( 1, ifchr->max_hit ); /* poprawione przez Lama */
		else if ( !str_cmp( nazwa, "inroom" ) )		zwrot_wart = ifchr->in_room->vnum;
		else if ( !str_cmp( nazwa, "sameroom" ) )	zwrot_wart = ( ifchr->in_room == mob->in_room ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "samearea" ) )	zwrot_wart = ( ifchr->in_room->area == mob->in_room->area ) ? TRUE : FALSE;
		else if ( !str_cmp( nazwa, "isoutside" ) )	zwrot_wart = IS_OUTSIDE( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "sex" ) )		zwrot_wart = ifchr->sex;
		else if ( !str_cmp( nazwa, "age" ) )		zwrot_wart = get_age( ifchr ); /* Lam 20.12.1999 */
		else if ( !str_cmp( nazwa, "position" ) )	zwrot_wart = ifchr->position;
		else if ( !str_cmp( nazwa, "isawake" ) )	zwrot_wart = IS_AWAKE( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "nooxygen" ) )	zwrot_wart = CZY_BRAK_TLENU( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "sunburns" ) )	zwrot_wart = CZY_PALACE_SLONCE( ifchr ) ? TRUE : FALSE; /* Lam */
		else if ( !str_cmp( nazwa, "level" ) )		zwrot_wart = ifchr->level;
		else if ( !str_cmp( nazwa, "trust" ) )		zwrot_wart = get_trust( ifchr ); /* Lam */
		else if ( !str_cmp( nazwa, "class" ) )		strcpy( zwrot_txt, (char *) class_table[ ifchr->klasa ].who_name );
		else if ( !str_cmp( nazwa, "race" ) )		strcpy( zwrot_txt, (char *) race_table[ ifchr->race ].ang ); /* Lam */
		else if ( !str_cmp( nazwa, "racepl" ) )		strcpy( zwrot_txt, (char *) race_table[ ifchr->race ].name ); /* Lam */
		else if ( !str_cmp( nazwa, "racesize" ) )	zwrot_wart = ROZMIAR( ifchr );
		else if ( !str_cmp( nazwa, "goldamt" ) )	zwrot_wart = ifchr->gold;
		else if ( !str_cmp( nazwa, "alignment" ) )	zwrot_wart = ifchr->alignment;
		else if ( !str_cmp( nazwa, "str" ) )		zwrot_wart = get_curr_str( ifchr ); /* Lam */
		else if ( !str_cmp( nazwa, "wis" ) )		zwrot_wart = get_curr_wis( ifchr );
		else if ( !str_cmp( nazwa, "int" ) )		zwrot_wart = get_curr_int( ifchr );
		else if ( !str_cmp( nazwa, "dex" ) )		zwrot_wart = get_curr_dex( ifchr );
		else if ( !str_cmp( nazwa, "con" ) )		zwrot_wart = get_curr_con( ifchr );
		else if ( !str_cmp( nazwa, "carry_weight" ) )	zwrot_wart = ifchr->carry_weight;		/* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "carry_number" ) )	zwrot_wart = ifchr->carry_number;		/* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "can_carry_n" ) )	zwrot_wart = can_carry_n( ifchr );		/* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "can_carry_w" ) )	zwrot_wart = can_carry_w( ifchr );		/* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "hitroll1" ) )	zwrot_wart = get_hitroll( ifchr, WEAR_WIELD );	/* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "hitroll2" ) )	zwrot_wart = get_hitroll( ifchr, WEAR_WIELD_2 );/* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "damroll1" ) )	zwrot_wart = get_damroll( ifchr, WEAR_WIELD );	/* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "damroll2" ) )	zwrot_wart = get_damroll( ifchr, WEAR_WIELD_2 );/* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "dualweapon" ) )	zwrot_wart = get_eq_char( ifchr, WEAR_WIELD_2 ) || ( ( !IS_NPC( ifchr ) && ifchr->pcdata->learned[ gsn_dual ] ) && !( get_eq_char( ifchr, WEAR_SHIELD ) || get_eq_char( ifchr, WEAR_HOLD ) || get_eq_char( ifchr, WEAR_LIGHT ) ) ) ? TRUE : FALSE; /* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "iswearing" ) )	{ if ( *wart2 < 0 || *wart2 > MAX_WEAR ) ZLA_FUNKCJA; zwrot_wart = get_eq_char( ifchr, *wart2 ) ? TRUE : FALSE; } /* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "number" ) )		zwrot_wart = IS_NPC( ifchr ) ? ifchr->pIndexData->vnum : 0;
		else if ( !str_cmp( nazwa, "name" ) )		strcpy( zwrot_txt, ifchr->name );
		else if ( !str_cmp( nazwa, "spec" ) )		strcpy( zwrot_txt, spec_name( ifchr->spec_fun ) );
		else if ( !str_cmp( nazwa, "doingquest" ) )	zwrot_wart = find_mpquest( ifchr, *wart2 ) ? TRUE : FALSE;	/* Lam 18.8.98 */
		else if ( !str_cmp( nazwa, "queststage" ) ){ MPQUEST_DATA *mpq; mpq = find_mpquest( ifchr, *wart2 );	/* Lam 18.8.98 */
					if ( !mpq ) zwrot_wart = 0; else zwrot_wart = mpq->stage; }
		else if ( !str_cmp( nazwa, "questdata0" ) ){ MPQUEST_DATA *mpq; mpq = find_mpquest( ifchr, *wart2 );	/* Lam 13.6.99 */
					if ( !mpq ) zwrot_wart = 0; else zwrot_wart = mpq->data[ 0 ]; }
		else if ( !str_cmp( nazwa, "questdata1" ) ){ MPQUEST_DATA *mpq; mpq = find_mpquest( ifchr, *wart2 );	/* Lam 13.6.99 */
					if ( !mpq ) zwrot_wart = 0; else zwrot_wart = mpq->data[ 1 ]; }
		else if ( !str_cmp( nazwa, "questdata2" ) ){ MPQUEST_DATA *mpq; mpq = find_mpquest( ifchr, *wart2 );	/* Lam 13.6.99 */
					if ( !mpq ) zwrot_wart = 0; else zwrot_wart = mpq->data[ 2 ]; }
		else if ( !str_cmp( nazwa, "questdata3" ) ){ MPQUEST_DATA *mpq; mpq = find_mpquest( ifchr, *wart2 );	/* Lam 13.6.99 */
					if ( !mpq ) zwrot_wart = 0; else zwrot_wart = mpq->data[ 3 ]; }
		else if ( !str_cmp( nazwa, "questdata4" ) ){ MPQUEST_DATA *mpq; mpq = find_mpquest( ifchr, *wart2 );	/* Lam 13.6.99 */
					if ( !mpq ) zwrot_wart = 0; else zwrot_wart = mpq->data[ 4 ]; }
		else if ( !str_cmp( nazwa, "carryobjtype" ) )
		{
		    OBJ_DATA *obj;
		    for ( obj = ifchr->carrying; obj; obj = obj->next_content )
			if ( obj->item_type == *wart2 )
			{
			    zwrot_wart = TRUE;
			    break;
			}
		    if ( !obj )
			zwrot_wart = FALSE;
		}
		else if ( !str_cmp( nazwa, "get_next_char_room" ) )
		{
		    CHAR_DATA *znajda = ifchr->next_in_room;
		    if ( !znajda ) return TRUE; /* chr nie ustawi zmiennej, if zwroci falsz */
		    zwrot_wsk = znajda;
		    zwrot_typ = -3;
		}
		else if ( *typ2 == -1 )
		{
		    /* tutaj wszystkie funkcje fun( postac, tekst ) */
		    if ( !str_cmp( nazwa, "affduration" ) )
		    {
			AFFECT_DATA *paf;
			for ( paf = ifchr->affected; paf; paf = paf->next )
			{
			    if ( paf->deleted )
				continue;
			    if ( !str_cmp( txt2, skill_table[ paf->type ].pl_name ) )
				break;
			}
			zwrot_wart = ( paf ) ? paf->duration : -2;
		    }
		    else
			ZLA_FUNKCJA;
		}
		else
		    ZLA_FUNKCJA;
	}
	else if ( ifobj )
	{
		if ( !str_cmp( nazwa, "deleted" ) )		zwrot_wart = ifobj->deleted;
		else if ( !str_cmp( nazwa, "cansee" ) )		zwrot_wart = can_see_obj( mob, ifobj );
		else if ( !str_cmp( nazwa, "name_expand" ) )	strcpy( zwrot_txt, name_expand_obj( mob, ifobj ) );
		else if ( !str_cmp( nazwa, "objtype" ) )	zwrot_wart = ifobj->item_type;
		else if ( !str_cmp( nazwa, "weight" ) )		zwrot_wart = obj->weight; /* Lam 7.5.2000 */
		else if ( !str_cmp( nazwa, "objweight" ) )	zwrot_wart = get_obj_weight( ifobj ); /* Lam 29.4.2000 */
		else if ( !str_cmp( nazwa, "objval0" ) )	zwrot_wart = ifobj->value[ 0 ].v;
		else if ( !str_cmp( nazwa, "objval1" ) )	zwrot_wart = ifobj->value[ 1 ].v;
		else if ( !str_cmp( nazwa, "objval2" ) )	zwrot_wart = ifobj->value[ 2 ].v;
		else if ( !str_cmp( nazwa, "objval3" ) )	zwrot_wart = ifobj->value[ 3 ].v;
		else if ( !str_cmp( nazwa, "objval4" ) )	zwrot_wart = ifobj->value[ 4 ].v;
		else if ( !str_cmp( nazwa, "objval5" ) )	zwrot_wart = ifobj->value[ 5 ].v;
		else if ( !str_cmp( nazwa, "number" ) )		zwrot_wart = ifobj->pIndexData->vnum;
		else if ( !str_cmp( nazwa, "rodzaj" ) )		zwrot_wart = ifobj->rodzaj;
		else if ( !str_cmp( nazwa, "name" ) )		strcpy( zwrot_txt, ifobj->name );
		else if ( !str_cmp( nazwa, "owner" ) )		strcpy( zwrot_txt, ifobj->owner ); /* Lam 3.6.2005 */
		else if ( !str_cmp( nazwa, "level" ) )		zwrot_wart = ifobj->level; /* Lam 3.6.2005 */
		else if ( !str_cmp( nazwa, "isextraflag" ) )	zwrot_wart = ( IS_SET_V( ifobj->extra_flags, *wart2 ) ) ? TRUE : FALSE; /* Lam 3.6.2005 */
		else if ( !str_cmp( nazwa, "iswearflag" ) )	zwrot_wart = ( ifobj->wear_flags & ( 1 << *wart2 ) ) ? TRUE : FALSE; /* Lam 3.6.2005 */
		else ZLA_FUNKCJA;
	}
	/* else ZLA_FUNKCJA - Lam 13.6.2000 to bylo powodem nie dzialania deleted( ) */
    }
    else
	ZLA_FUNKCJA;

    strcpy( txt1, zwrot_txt );
    *wart1 = neguj ? !zwrot_wart : zwrot_wart;
    *typ1 = zwrot_typ;
    *wsk1 = zwrot_wsk;
    if ( !zwrot_typ && zwrot_txt[ 0 ] )
	*typ1 = -1;

    return FALSE;
}


/*
 * Lam 29.8.2006
 *
 * Wykreca stos dla celow && i || (przywraca oryginalna kolejnosc argumentow),
 * zeby potem nie latac 10 razy po dlugich listach szukajac wskaznikow na
 * lewa strone wyrazenia, ktora jest gdzies daleko.
 *
 * To wydaje sie kompletnie zbedna czynnoscia, ale nie umiem wymyslic lepszego
 * algorytmu niz ten (Turskiego, ktory nie przewiduje pomijania obliczania
 * fragmentow wyrazenia). Nie jest to oczywiscie ogromna strata mocy procesora,
 * skoro najdluzsze wyrazenie we wszystkich progach Laca robi stos dlugosci 17,
 * a przecietnie nie wychodzi ponad 5 elementow.
 */
void wykrec( STOSMP **stoswej, STOSMP **stoswyj )
{
    STOSMP *kawalek = ze_stosmp( stoswej );

    if ( kawalek->typ > 0 )
    {
	/* bledy sprawdze kiedy indziej, ten niech sie chociaz nie wywala */
	if ( *stoswej )
	    wykrec( stoswej, stoswyj );
	if ( *stoswej )
	    wykrec( stoswej, stoswyj );
    }

    na_stosmp( stoswyj, kawalek );

    return;
}


/*
 * Pomijanie prawej strony && i || w miare potrzeb.
 */
void pomin_stosmp( STOSMP **stos )
{
    STOSMP *kawalek = ze_stosmp( stos );

    if ( kawalek->typ > 0 )
    {
	pomin_stosmp( stos );
	pomin_stosmp( stos );
    }

    wywal_kawalek_stosmp( kawalek );

    return;
}


/*
 * Sporo argumentow jak na funkcje rekurencyjna, ale nie szkodzi
 */
STOSMP *licz_stosmp( STOSMP **stos, CHAR_DATA *mob, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
			OBJ_DATA *v_obj )
{
    STOSMP *kawalek;

    if ( !*stos )
    {
	progbug( mob, "oblicz_wyrazenie: za malo argumentow dla dzialania" );
	return NULL;
    }

    kawalek = ze_stosmp( stos );

    if ( kawalek->typ > 0 )
    {
	STOSMP *prawy;
	STOSMP *lewy = licz_stosmp( stos, mob, actor, obj, rndm, vict, v_obj );
	if ( !lewy )
	{
	    progbug( mob, "oblicz_wyrazenie: urwane dzialanie (blad lub brak lewej strony)" );
	    wywal_kawalek_stosmp( kawalek );
	    return NULL;
	}

	if ( kawalek->typ == DZIAL_LUB
	  && !lewy->typ
	  && lewy->wart )
	{
	    wywal_kawalek_stosmp( lewy );
	    pomin_stosmp( stos );
	    kawalek->typ = 0;
	    kawalek->wart = 1;
	    return kawalek;
	}

	if ( kawalek->typ == DZIAL_I
	  && !lewy->typ
	  && !lewy->wart )
	{
	    wywal_kawalek_stosmp( lewy );
	    pomin_stosmp( stos );
	    kawalek->typ = 0;
	    kawalek->wart = 0;
	    return kawalek;
	}

	prawy = licz_stosmp( stos, mob, actor, obj, rndm, vict, v_obj );
	if ( !prawy )
	{
	    progbug( mob, "oblicz_wyrazenie: urwane dzialanie (blad lub brak prawej strony)" );
	    wywal_kawalek_stosmp( lewy );
	    wywal_kawalek_stosmp( kawalek );
	    return NULL;
	}

	if ( kawalek->typ == 100 )
	{
	    if ( uruchom_funkcje( mob, kawalek->txt,
		    lewy->txt, &lewy->wart, &lewy->typ, &lewy->wsk,
		    prawy->txt, &prawy->wart, &prawy->typ, &prawy->wsk,
		    actor, obj, rndm, vict, v_obj ) )
	    {
		wywal_kawalek_stosmp( kawalek );
		wywal_kawalek_stosmp( lewy );
		wywal_kawalek_stosmp( prawy );
		return NULL;
	    }
	    else
	    {
		wywal_kawalek_stosmp( kawalek );
		wywal_kawalek_stosmp( prawy );
		return lewy;
	    }
	}
	else if ( lewy->typ != prawy->typ )
	{
	    progbug( mob, "oblicz_wyrazenie: nie wykonuje sie dzialan miedzy danymi roznych typow!" );
	    wywal_kawalek_stosmp( kawalek );
	    wywal_kawalek_stosmp( lewy );
	    wywal_kawalek_stosmp( prawy );
	    return NULL;
	}
	else if ( lewy->typ == -1 )
	{
	    if ( kawalek->typ == DZIAL_ROWN )
	    {
		kawalek->wart = ( !str_cmp( lewy->txt, prawy->txt ) );
		kawalek->typ = 0;
	    }
	    else if ( kawalek->typ == DZIAL_NIEROWN )
	    {
		kawalek->wart = ( str_cmp( lewy->txt, prawy->txt ) );
		kawalek->typ = 0;
	    }
	    else
	    {
		wywal_kawalek_stosmp( kawalek );
		wywal_kawalek_stosmp( lewy );
		wywal_kawalek_stosmp( prawy );
		progbug( mob, "oblicz_wyrazenie: pomiedzy lancuchami mozna stosowac tylko '==' albo '!='" );
		return NULL;
	    }
	}
	else if ( !lewy->typ ) /* dzialanie miedzy liczbami */
	{
	    if (      kawalek->typ == DZIAL_ROWN )
		lewy->wart = ( ( lewy->wart == prawy->wart ) ? 1 : 0 );
	    else if ( kawalek->typ == DZIAL_NIEROWN )
		lewy->wart = ( ( lewy->wart == prawy->wart ) ? 0 : 1 );
	    else if ( kawalek->typ == DZIAL_WIEK )
		lewy->wart = ( ( lewy->wart > prawy->wart ) ? 1 : 0 );
	    else if ( kawalek->typ == DZIAL_MNIEJ )
		lewy->wart = ( ( lewy->wart < prawy->wart ) ? 1 : 0 );
	    else if ( kawalek->typ == DZIAL_WIEKROWN )
		lewy->wart = ( ( lewy->wart >= prawy->wart ) ? 1 : 0 );
	    else if ( kawalek->typ == DZIAL_MNIEJROWN )
		lewy->wart = ( ( lewy->wart <= prawy->wart ) ? 1 : 0 );
	    else if ( kawalek->typ == DZIAL_I )
		lewy->wart = ( ( lewy->wart && prawy->wart ) ? 1 : 0 );
	    else if ( kawalek->typ == DZIAL_LUB )
		lewy->wart = ( ( lewy->wart || prawy->wart ) ? 1 : 0 );
	    else if ( kawalek->typ == DZIAL_DOD )
		lewy->wart += prawy->wart;
	    else if ( kawalek->typ == DZIAL_ODEJ )
		lewy->wart -= prawy->wart;
	    else if ( kawalek->typ == DZIAL_RAZY )
		lewy->wart *= prawy->wart;
	    else if ( kawalek->typ == DZIAL_DZIEL )
		lewy->wart /= prawy->wart;
	    else if ( kawalek->typ == DZIAL_AND )
		lewy->wart &= prawy->wart;
	    else if ( kawalek->typ == DZIAL_OR )
		lewy->wart |= prawy->wart;
	    else
	    {
		progbug( mob, "oblicz_wyrazenie: znany ale nie znany operator?" /* ": %s", kawalek->txt */ );
		wywal_kawalek_stosmp( kawalek );
		wywal_kawalek_stosmp( lewy );
		wywal_kawalek_stosmp( prawy );
		return NULL;
	    }
	    wywal_kawalek_stosmp( kawalek );
	    wywal_kawalek_stosmp( prawy );
	    return lewy;
	}
	else
	{
	    progbug( mob, "oblicz_wyrazenie: nie umiem wykonywac dzialan na lancuchach i postaciach" );
	    wywal_kawalek_stosmp( kawalek );
	    wywal_kawalek_stosmp( lewy );
	    wywal_kawalek_stosmp( prawy );
	    return NULL;
	}
	wywal_kawalek_stosmp( lewy );
	wywal_kawalek_stosmp( prawy );
    }
    else if ( kawalek->typ == -100 )
    {
	TYP_ZMIENNEJ *zm;
	bool negacja = kawalek->txt[ 0 ] == '!' ? TRUE : FALSE;

	if ( negacja )
	    zm = znajdz_zmienna_moba( mob, &kawalek->txt[ 1 ] );
	else
	    zm = znajdz_zmienna_moba( mob, kawalek->txt );

	if ( !zm )
	{
	    progbug( mob, "oblicz_wyrazenie: zmienna '%s' nie istnieje", kawalek->txt );
	    wywal_kawalek_stosmp( kawalek );
	    return NULL;
	}

	if ( zm->typ != TZ_CALK && negacja )
	{
	    progbug( mob, "oblicz_wyrazenie: negacja zmiennej innego typu niz calkowity, ignoruje!" );
	    negacja = FALSE;
	}

	if ( zm->typ == TZ_CALK )
	{
	    kawalek->wart = negacja ? !zm->wartosc.i : zm->wartosc.i;
	    kawalek->typ = 0;
	}
	else if ( zm->typ == TZ_ZNAK )
	{
	    if ( strlen( zm->wartosc.p ) >= MIL )
		strcpy( kawalek->txt, "(za d`lugi tekst)" );
	    else
		strcpy( kawalek->txt, zm->wartosc.p );
	    kawalek->typ = -1;
	}
	else if ( zm->typ == TZ_POST )
	{
	    kawalek->typ = -3;
	    kawalek->wsk = zm->wartosc.p;
	}
	else if ( zm->typ == TZ_PRZE )
	{
	    kawalek->typ = -4;
	    kawalek->wsk = zm->wartosc.p;
	}
	else
	    progbug( mob, "oblicz_wyrazenie: zmienna '%s' nieznanego typu", kawalek->txt );
    }

    return kawalek;
}


void wymiec_smieci( STOSMP **s1, STOSMP **s2 )
{
    if ( s1 && *s1 )
	while ( *s1 )
	    wywal_kawalek_stosmp( ze_stosmp( s1 ) );

    if ( s2 && *s2 )
	while ( *s2 )
	    wywal_kawalek_stosmp( ze_stosmp( s2 ) );

    return;
}


/*
 * Funkcja wraz z dzialajaca aktualna skladnia pojawila sie 10.10.1999. Od
 * tamtej pory nie nastapila zadna zmiana skladni.
 *
 * 28.8.2006 lwia czesc aparatu obliczajacego wyrazenia zostala przerobiona na
 * stosy celem poprawy szybkosci i umozliwienia pomijania prawej strony wyrazen
 * && i ||. Teraz wartosci zmiennych i wyniki funkcji sa obliczane dopiero kiedy
 * jest taka potrzeba (wczesniej najpierw byly zamieniane wszystkie zmienne,
 * potem liczone funkcje, potem wykonywane dzialania). Dzieki temu nie trzeba
 * uzywac "or" po "if" lub zagniezdzac "if" w sobie, co oczywiscie dzialalo
 * doskonale, ale ludzie oczekiwali odwzorowania dzialania wyrazen w C.
 */
bool oblicz_wyrazenie( CHAR_DATA *mob, char *wyrazenie, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
			OBJ_DATA *v_obj, int *zwr_typ, int *zwr_wart,
			char *zwr_txt, void **zwr_wsk )
{
    STOSMP *s1;
    STOSMP *s2;
    char nazwa_funkcji[ MIL + 4 ];
    char zsyp[ MIL + 4 ];
    char txt[ MIL + 4 ];
    int dzial;
    int nawias = 0; /* poziom zagniezdzenia nawiasow */
    int funkcja = 0; /* czy przetwarzamy wnetrze funkcji? */
    int fun_nawias = -1; /* bo nawias moze byc rowny 0 */
    int fun_arg = 0;
    bool wykorzystany = FALSE; /* stosowane do nawiasow i funkcji */
    char *pok = wyrazenie;

    s1 = s2 = NULL;

    while ( *pok )
    {
	pok = nastepny_skladnik( pok, txt );
	if ( !*txt )
	    break;

	if ( txt[ 0 ] == ',' )
	{
	    if ( funkcja <= 0 )
	    {
		progbug( mob, "oblicz_wyrazenie: po co przecinek poza argumentami funkcji?" );
		wymiec_smieci( &s1, &s2 );
		return FALSE;
	    }

	    if ( fun_arg++ )
	    {
		progbug( mob, "oblicz_wyrazenie: za duzo argumentow funkcji (dwa przecinki?)" );
		wymiec_smieci( &s1, &s2 );
		return FALSE;
	    }

	    /* usuniecie udawanego nawiasu i wciagniecie zawartosci */
	    while ( s1 && s1->typ != 101 )
	    {
		na_stosmp( &s2, ze_stosmp( &s1 ) );
		s2->typ = dzialania[ s2->typ ].typ;
		if ( !wykorzystany )
		{
		    progbug( mob, "oblicz_wyrazenie: niezle mnie cos zakrecilo" );
		    wymiec_smieci( &s1, &s2 );
		    return FALSE;
		}
	    }

	    if ( !s1 )
	    {
		progbug( mob, "oblicz_wyrazenie: cos pozornie niemozliwego" );
		wymiec_smieci( &s1, &s2 );
		return FALSE;
	    }

	    wywal_kawalek_stosmp( ze_stosmp( &s1 ) );

	    if ( !wykorzystany ) /* nie bylo niczego, wstawic zero */
		na_stosmp( &s2, nowy_kawalek_stosmp( ) );

	    na_stosmp( &s1, nowy_kawalek_stosmp( ) );
	    s1->typ = 101; /* udawany nawias dla drugiego argumentu */
	    wykorzystany = FALSE;
	    continue;
	}
	else if ( txt[ 0 ] == '(' )
	{
	    nawias++;
	    wykorzystany = FALSE;
	    if ( nawias > 100 )
	    {
		progbug( mob, "oblicz_wyrazenie: zbyt zagniezdzone nawiasy" );
		wymiec_smieci( &s1, &s2 );
		return FALSE;
	    }
	    na_stosmp( &s1, nowy_kawalek_stosmp( ) );
	    s1->typ = 101;
	    continue;
	}
	else if ( txt[ 0 ] == ')' )
	{
	    nawias--;

	    if ( nawias < 0 )
	    {
		progbug( mob, "oblicz_wyrazenie: zbyt duzo nawiasow zamykajacych" );
		wymiec_smieci( &s1, &s2 );
		return FALSE;
	    }

	    while ( s1 && s1->typ != 101 )
	    {
		na_stosmp( &s2, ze_stosmp( &s1 ) );
		s2->typ = dzialania[ s2->typ ].typ;
	    }

	    if ( !s1 )
	    {
		progbug( mob, "oblicz_wyrazenie: zbyt duzo nawiasow zamykajacych [2, dziwne]" );
		wymiec_smieci( &s1, &s2 );
		return FALSE;
	    }

	    wywal_kawalek_stosmp( ze_stosmp( &s1 ) );

	    if ( fun_nawias != -1
	      && nawias == fun_nawias ) /* koniec funkcji */
	    {
		funkcja--;
		fun_nawias = -1;
		/* kazda funkcja jest traktowana jak dzialanie miedzy dwoma jej
		   argumentami */
		if ( !wykorzystany
		  || fun_arg < 1 )
		{
		    /* jesli byla jedna liczba, nie potrzeba przecinka */
		    if ( !wykorzystany && !fun_arg )
			na_stosmp( &s2, nowy_kawalek_stosmp( ) );
		    na_stosmp( &s2, nowy_kawalek_stosmp( ) );
		}
		na_stosmp( &s2, nowy_kawalek_stosmp( ) );
		s2->typ = 100;
		strcpy( s2->txt, nazwa_funkcji );
	    }
	    else if ( !wykorzystany )
	    {
		progbug( mob, "oblicz_wyrazenie: pusta zawartosc nawiasow" );
		wymiec_smieci( &s1, &s2 );
		return FALSE;
	    }
	    continue;
	}
	else if ( strlen( txt ) >= MIL )
	{
	    progbug( mob, "oblicz_wyrazenie: zbyt dlugi skladnik: %s", txt );
	    wymiec_smieci( &s1, &s2 );
	    return FALSE;
	}

	if ( txt[ 0 ] == '"'
	  || txt[ 0 ] == '\'' )
	{
	    wykorzystany = TRUE;

	    na_stosmp( &s2, nowy_kawalek_stosmp( ) );
	    s2->typ = -1;
	    strcpy( s2->txt, &txt[ 1 ] ); /* pominiecie apostrofu/cudzyslowu */
	}
	else if ( txt[ 0 ] == '$' )
	{
	    na_stosmp( &s2, nowy_kawalek_stosmp( ) );
	    s2->typ = -2;
	    strcpy( s2->txt, txt );
	    wykorzystany = TRUE;
	}
	else if ( ( dzial = znajdz_dzialanie( txt ) ) != -1 )
	{
	    char txt_tmp[ MIL ];

	    /* glupia symulacja negacji */
	    if ( !wykorzystany && dzialania[ dzial ].typ == DZIAL_ODEJ )
	    {
		nastepny_skladnik( pok, txt_tmp );
		if ( *txt_tmp && czy_liczba( txt_tmp ) )
		{
		    pok = nastepny_skladnik( pok, txt );
		    na_stosmp( &s2, nowy_kawalek_stosmp( ) );
		    s2->wart = - atoi( txt );
		    wykorzystany = TRUE;
		}
	    }
	    else
	    {
		wykorzystany = TRUE;

		if ( !s2 )
		{
		    progbug( mob, "oblicz_wyrazenie: dzialanie znienacka" );
		    wymiec_smieci( &s1, &s2 );
		    return FALSE;
		}

		while ( s1 && dzialania[ s1->typ ].prio <= dzialania[ dzial ].prio )
		{
		    na_stosmp( &s2, ze_stosmp( &s1 ) );
		    /* na s1 byl typ z priorytetem, teraz zostanie sam typ do
		       obliczen */
		    s2->typ = dzialania[ s2->typ ].typ;
		}

		na_stosmp( &s1, nowy_kawalek_stosmp( ) );
		/* strcpy( s1->txt, txt ); dla wyswietlania w do_test */
		s1->typ = dzial;
	    }
	}
	else if ( czy_nazwa_zmiennej( txt )
	       && ( *pok == '(' ) )
	{
	    funkcja++;
	    fun_nawias = nawias++;
	    if ( funkcja > 1 )
	    {
		progbug( mob, "oblicz_wyrazenie: nie obsluguje zagniezdzania funkcji" );
		wymiec_smieci( &s1, &s2 );
		return FALSE;
	    }
	    pok = nastepny_skladnik( pok, zsyp ); /* pominiecie nawiasu */
	    strcpy( nazwa_funkcji, txt ); /* zapamietanie nazwy funkcji */
	    fun_arg = 0;
	    wykorzystany = FALSE;
	    na_stosmp( &s1, nowy_kawalek_stosmp( ) );
	    s1->typ = 101; /* udawany nawias wokol tresci argumentow funkcji */
	}
	else if ( czy_nazwa_zmiennej( txt ) )
	{
	    na_stosmp( &s2, nowy_kawalek_stosmp( ) );
	    s2->typ = -100;
	    strcpy( s2->txt, txt ); /* w txt[ 0 ] moze zostac negacja */
	    wykorzystany = TRUE;
	}
	else if ( czy_liczba( txt ) )
	{
	    na_stosmp( &s2, nowy_kawalek_stosmp( ) );
	    s2->wart = atoi( txt );
	    wykorzystany = TRUE;
	}
	else
	{
	    progbug( mob, "oblicz_wyrazenie: nie wiem co z tym zrobic: %s", txt );
	    wymiec_smieci( &s1, &s2 );
	    return FALSE;
	}
    }

    while ( s1 )
    {
	na_stosmp( &s2, ze_stosmp( &s1 ) );
	s2->typ = dzialania[ s2->typ ].typ;
    }

    /* ma sens przy uruchamianiu z do_test( )
    for ( s1 = s2; s1; s1 = s1->nast )
    {
	if ( s1->typ == 0 )
	    ch_printf( mob, "%d  ", s1->wart );
	else if ( s1->typ == -1 )
	    ch_printf( mob, "\"%s\"  ", s1->txt );
	else
	    ch_printf( mob, "%s  ", s1->txt );
    }
    STC( "\n\r", mob );
    */

    if ( nawias )
    {
	progbug( mob, "oblicz_wyrazenie: nie domkniety nawias?" );
	wymiec_smieci( &s1, &s2 );
	return FALSE;
    }

    if ( !s2 )
    {
	progbug( mob, "oblicz_wyrazenie: puste wyrazenie?" );
	wymiec_smieci( &s1, &s2 );
	return FALSE;
    }

    /* nie chcialem tego, ale stalo sie - dzialania ida od lewej, ale argumenty
       od prawej, czyli ( A || ( B && C ) ) daje || && C B A, ale najpierw
       trzeba sprawdzic A. Dlatego musze wykrecic: */
    wykrec( &s2, &s1 );

    /* jw.
    for ( s2 = s1; s2; s2 = s2->nast )
    {
	if ( s2->typ == 0 )
	    ch_printf( mob, "%d  ", s2->wart );
	else if ( s2->typ == -1 )
	    ch_printf( mob, "\"%s\"  ", s2->txt );
	else
	    ch_printf( mob, "%s  ", s2->txt );
    }
    STC( "\n\r", mob );
    */

    s2 = licz_stosmp( &s1, mob, actor, obj, rndm, vict, v_obj );

    /* licz_stosmp w przypadku bledu zwraca NULL i ma prawo pozostawic smieci
       na s1 */
    if ( !s2 )
    {
	wymiec_smieci( &s1, NULL );
	return FALSE;
    }

    *zwr_wart = s2->wart;
    *zwr_typ = s2->typ;
    *zwr_wsk = s2->wsk;
    strcpy( zwr_txt, s2->txt );
    wywal_kawalek_stosmp( s2 );

    return TRUE;
}


/*
 * Ciag dalszy obslugi zmiennych
 */
TYP_ZMIENNEJ *znajdz_zmienna_moba( CHAR_DATA *mob, char *nazwa )
{
    TYP_ZMIENNEJ *zm;

    zm = znajdz_zmienna( mob->zmienne, nazwa );
    if ( !zm && IS_NPC( mob ) )
	zm = znajdz_zmienna( mob->area->zmienne, nazwa );

    return zm; /* NULL, jesli nie znalazl */
}


void int_aint( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
		OBJ_DATA *v_obj, bool do_krainy )
{
/*  const char *ops[ 7 ] = { "=", "-=", "+=", "*=", "/=", "&=", "|=" }; */
    char nazwa[ MIL ];
    char polecenie[ MIL ];
    int wynik, typw;
    void *wynwsk;
    char txt[ MIL ];
    TYP_ZMIENNEJ *zm;
    TYP_ZMIENNEJ **dokad;
    int stara;

    if ( do_krainy )
	dokad = &ch->area->zmienne;
    else
	dokad = &ch->zmienne;

    argument = nastepny_skladnik( argument, nazwa );
    if ( !nazwa[ 0 ] )
    {
	progbug( ch, "MobC: (a)int: podaj nazwe zmiennej debilu!" );
	return;
    }
    zm = znajdz_zmienna( *dokad, nazwa );
    stara = ( zm && zm->typ == TZ_CALK ) ? zm->wartosc.i : 0;

    argument = nastepny_skladnik( argument, polecenie );
    if ( !polecenie[ 0 ] || !*argument )
    {
	progbug( ch, "MobC: (a)int: powiedz, co mam zrobic ze zmienna '%s'", nazwa );
	return;
    }

    if ( !czy_zdatny_operator_int( polecenie ) )
    {
	progbug( ch, "MobC: (a)int: nie znam operatora '%s'", polecenie );
	return;
    }

    /* nie zmieni zadnej zmiennej w wyniku bledu obliczania wyrazenia */
    if ( !oblicz_wyrazenie( ch, argument, actor, obj, rndm, vict, v_obj, &typw, &wynik, txt, &wynwsk ) )
	return;

    if ( typw )
    {
	progbug( ch, "MobC: (a)int: wyrazenie nie zwrocilo liczby calkowitej!" );
	return; /* rowniez nie zmieni zmiennej */
    }

    /* pierwsza litera wystarczy (druga to '='). Dzieki temu moge uzyc switcha */
    switch ( *polecenie )
    {
	default: progbug( ch, "MobC: (a)int: dziwaczny blad" ); return;
	case '=': 				break;
	case '+': wynik += stara;		break;
	case '-': wynik = stara - wynik;	break;
	case '*': wynik *= stara;		break;
	case '/': wynik = stara / wynik;	break;
	case '&': wynik &= stara;		break;
	case '|': wynik |= stara;		break;
    }

    progdebug( ch, "{WZmiana zmiennej{x ca`lkowitej %s na %d\n\r", nazwa, wynik );
    zmien_zmienna( dokad, nazwa, TZ_CALK, wynik, NULL, do_krainy ? NULL : ch );

    return;
}


void mv_int( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    int_aint( ch, argument, actor, obj, rndm, vict, v_obj, FALSE );
    tail_chain( );

    return;
}


void mv_aint( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    if ( !IS_NPC( ch ) )
    {
	progbug( ch, "MobC: aint: aint niewykonalny" );
	return;
    }

    int_aint( ch, argument, actor, obj, rndm, vict, v_obj, TRUE );

    return;
}


void str_astr( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
		OBJ_DATA *v_obj, bool do_krainy )
{
    char nazwa[ MIL ];
    char polecenie[ MIL ];
    int wynik, typw;
    void *wynwsk;
    char txt[ 2 * MIL ];
    TYP_ZMIENNEJ *zm;
    TYP_ZMIENNEJ **dokad;
    char stara[ 2 * MIL ];

    if ( do_krainy )
	dokad = &ch->area->zmienne;
    else
	dokad = &ch->zmienne;

    argument = nastepny_skladnik( argument, nazwa );
    if ( !nazwa[ 0 ] )
    {
	progbug( ch, "MobC: (a)str: podaj nazwe zmiennej debilu!" );
	return;
    }
    zm = znajdz_zmienna( *dokad, nazwa );

    stara[ 0 ] = '\0';
    if ( zm && zm->typ == TZ_ZNAK )
	strcpy( stara, zm->wartosc.p );

    argument = nastepny_skladnik( argument, polecenie );
    if ( !polecenie[ 0 ] || !*argument )
    {
	progbug( ch, "MobC: (a)str: powiedz, co mam zrobic ze zmienna '%s'", nazwa );
	return;
    }

    if ( !czy_zdatny_operator_str( polecenie ) )
    {
	progbug( ch, "MobC: (a)str: nie znam operatora '%s'", polecenie );
	return;
    }

    /* nie zmieni zadnej zmiennej w wyniku bledu obliczania wyrazenia */
    if ( !oblicz_wyrazenie( ch, argument, actor, obj, rndm, vict, v_obj, &typw, &wynik, txt, &wynwsk ) )
	return;

    if ( typw != -1 )
    {
	progbug( ch, "MobC: (a)str: wyrazenie nie zwrocilo lancucha!" );
	return; /* rowniez nie zmieni zmiennej */
    }


    /* pierwsza litera wystarczy (druga to '='). Dzieki temu moge uzyc switcha */
    switch ( *polecenie )
    {
	default: progbug( ch, "MobC: (a)str: dziwaczny blad" ); return;
	case '=': break;
	case '+':
		txt[ MIL - 1 ] = '\0';
		strcat( stara, txt );
		strcpy( txt, stara );
		break;
    }

    txt[ MIL - 1 ] = '\0';
    progdebug( ch, "{WZmiana zmiennej{x `la`ncuchowej %s na \"%s\"\n\r", nazwa, txt );
    zmien_zmienna( dokad, nazwa, TZ_ZNAK, 0, txt, do_krainy ? NULL : ch );

    return;
}


void mv_str( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    str_astr( ch, argument, actor, obj, rndm, vict, v_obj, FALSE );
    tail_chain( );

    return;
}


void mv_astr( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    if ( !IS_NPC( ch ) )
    {
	progbug( ch, "MobC: astr: astr niewykonalny" );
	return;
    }

    str_astr( ch, argument, actor, obj, rndm, vict, v_obj, TRUE );

    return;
}


void chr_achr( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
		OBJ_DATA *v_obj, bool do_krainy )
{
    char nazwa[ MIL ];
    char polecenie[ MIL ];
    int wynik, typw;
    void *wynwsk;
    TYP_ZMIENNEJ *zm;
    TYP_ZMIENNEJ **dokad;
    char txt[ MIL ];

    if ( do_krainy )
	dokad = &ch->area->zmienne;
    else
	dokad = &ch->zmienne;

    argument = nastepny_skladnik( argument, nazwa );
    if ( !nazwa[ 0 ] )
    {
	progbug( ch, "MobC: (a)chr: podaj nazwe zmiennej debilu!" );
	return;
    }
    zm = znajdz_zmienna( *dokad, nazwa );

    argument = nastepny_skladnik( argument, polecenie );
    if ( !polecenie[ 0 ] || !*argument )
    {
	progbug( ch, "MobC: (a)chr: powiedz, co mam zrobic ze zmienna '%s'", nazwa );
	return;
    }

    if ( str_cmp( polecenie, "=" ) )
    {
	progbug( ch, "MobC: (a)chr: nie znam operatora '%s' (znam tylko '=')", polecenie );
	return;
    }

    if ( !oblicz_wyrazenie( ch, argument, actor, obj, rndm, vict, v_obj, &typw, &wynik, txt, &wynwsk ) )
    {
	/* nie dosc, ze nie zmieni zmiennej, to jeszcze ja usunie, jesli byla */
	if ( zm )
	{
	    progdebug( ch, "{WUsuni`ecie zmiennej{x %s\n\r", nazwa );
	    usun_z_listy( dokad, zm, do_krainy ? NULL : ch );
	}
	return;
    }

    if ( typw == -2 )
    {
	switch ( txt[ 1 ] )
	{
	    case 'i': wynwsk = ch; break;
	    case 'n': wynwsk = actor; break;
	    case 'r': wynwsk = rndm; break;
	    case 't': wynwsk = vict; break;
	    case '0': wynwsk = NULL; break;
	    default:
		progbug( ch, "MobC: (a)chr: $%c to nie postac!", txt[ 1 ] );
		return;
	}

	if ( !wynwsk )
	{
	    if ( zm )
	    {
		progdebug( ch, "{WUsuni`ecie zmiennej{x %s\n\r", nazwa );
		usun_z_listy( dokad, zm, do_krainy ? NULL : ch );
	    }
	    else
		progdebug( ch, "{WBez zmiany zmiennej{x %s\n\r", nazwa );
	    return;
	}
    }
    else if ( typw != -3 )
    {
	progbug( ch, "MobC: (a)chr: wyrazenie nie zwrocilo postaci!" );
	return; /* rowniez nie zmieni zmiennej */
    }

    progdebug( ch, "{WZmiana zmiennej{x postaciowej %s\n\r", nazwa );
    zmien_zmienna( dokad, nazwa, TZ_POST, 0, wynwsk, do_krainy ? NULL : ch );
    if ( !do_krainy )
    {
	CHAR_DATA *ofiara = wynwsk;
	dod_zwod( &ofiara->zwod_zmienne, ch );
/*	ch_printf( ofiara, "Dodaje cie do zmiennej %s, dostajesz zwod.\n\r", nazwa ); */
    }

    return;
}


void mv_chr( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    chr_achr( ch, argument, actor, obj, rndm, vict, v_obj, FALSE );
    tail_chain( );

    return;
}


void mv_achr( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    if ( !IS_NPC( ch ) )
    {
	progbug( ch, "MobC: achr: achr niewykonalny" );
	return;
    }

    chr_achr( ch, argument, actor, obj, rndm, vict, v_obj, TRUE );

    return;
}


/* 2.10.2004 */
void obj_aobj( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
		OBJ_DATA *v_obj, bool do_krainy )
{
    char nazwa[ MIL ];
    char polecenie[ MIL ];
    int wynik, typw;
    void *wynwsk;
    TYP_ZMIENNEJ *zm;
    TYP_ZMIENNEJ **dokad;
    char txt[ MIL ];

    if ( do_krainy )
	dokad = &ch->area->zmienne;
    else
	dokad = &ch->zmienne;

    argument = nastepny_skladnik( argument, nazwa );
    if ( !nazwa[ 0 ] )
    {
	progbug( ch, "MobC: (a)obj: podaj nazwe zmiennej debilu!" );
	return;
    }
    zm = znajdz_zmienna( *dokad, nazwa );

    argument = nastepny_skladnik( argument, polecenie );
    if ( !polecenie[ 0 ] || !*argument )
    {
	progbug( ch, "MobC: (a)obj: powiedz, co mam zrobic ze zmienna '%s'", nazwa );
	return;
    }

    if ( str_cmp( polecenie, "=" ) )
    {
	progbug( ch, "MobC: (a)obj: nie znam operatora '%s' (znam tylko '=')", polecenie );
	return;
    }

    if ( !oblicz_wyrazenie( ch, argument, actor, obj, rndm, vict, v_obj, &typw, &wynik, txt, &wynwsk ) )
    {
	/* nie dosc, ze nie zmieni zmiennej, to jeszcze ja usunie, jesli byla */
	if ( zm )
	{
	    progdebug( ch, "{WUsuni`ecie zmiennej{x %s\n\r", nazwa );
	    usun_z_listy( dokad, zm, do_krainy ? NULL : ch );
	}
	return;
    }

    if ( typw == -2 )
    {
	switch ( txt[ 1 ] )
	{
	    case 'o': wynwsk = obj; break;
	    case 'p': wynwsk = v_obj; break;
	    case '0': wynwsk = NULL; break;
	    default:
		progbug( ch, "MobC: (a)obj: $%c to nie przedmiot!", txt[ 1 ] );
		return;
	}

	if ( !wynwsk )
	{
	    if ( zm )
	    {
		progdebug( ch, "{WUsuni`ecie zmiennej{x %s\n\r", nazwa );
		usun_z_listy( dokad, zm, do_krainy ? NULL : ch );
	    }
	    else
		progdebug( ch, "{WBez zmiany zmiennej{x %s\n\r", nazwa );
	    return;
	}
    }
    else if ( typw != -4 )
    {
	progbug( ch, "MobC: (a)obj: wyrazenie nie zwrocilo przedmiotu!" );
	return; /* rowniez nie zmieni zmiennej */
    }

    progdebug( ch, "{WZmiana zmiennej{x przedmiotowej %s\n\r", nazwa );
    zmien_zmienna( dokad, nazwa, TZ_PRZE, 0, wynwsk, do_krainy ? NULL : ch );

    return;
}


void mv_obj( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    obj_aobj( ch, argument, actor, obj, rndm, vict, v_obj, FALSE );
    tail_chain( );

    return;
}


void mv_aobj( CHAR_DATA *ch, char *argument, CHAR_DATA *actor,
		OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    if ( !IS_NPC( ch ) )
    {
	progbug( ch, "MobC: aobj: aobj niewykonalny" );
	return;
    }

    obj_aobj( ch, argument, actor, obj, rndm, vict, v_obj, TRUE );

    return;
}
