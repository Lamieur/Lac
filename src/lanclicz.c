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
 *  Funkcje obslugi lancuchow i liczb.                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "merc.h"


/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    switch ( number_bits( 2 ) )
    {
	case 0:  number -= 1; break;
	case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}


/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
    int power;
    int number;

    if ( ( to = to - from + 1 ) <= 1 )
	return from;

    for ( power = 2; power < to; power <<= 1 )
	;

    while ( ( number = number_mm( ) & ( power - 1 ) ) >= to )
	;

    return from + number;
}


/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    int percent;

    while ( ( percent = number_mm( ) & ( 128-1 ) ) > 99 )
	;

    return 1 + percent;
}


/*
 * Generate a random door.
 */
int number_door( void )
{
    int door;

/*  while ( ( door = number_mm( ) & ( 8-1 ) ) > 5 ) */
    while ( ( door = number_mm( ) & ( 15 ) ) > 9 )
	;

    return door;
}


int number_bits( int width )
{
    return number_mm( ) & ( ( 1 << width ) - 1 );
}


/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static  int     rgiState[ 2 + 55 ];

void init_mm( void )
{
    int *piState;
    int  iState;

    piState     = &rgiState[ 2 ];

    piState[ -2 ] = 55 - 55;
    piState[ -1 ] = 55 - 24;

    piState[ 0 ]  = ( (int) current_time ) & ( ( 1 << 30 ) - 1 );
    piState[ 1 ]  = 1;

    for ( iState = 2; iState < 55; iState++ )
    {
	piState[ iState ] = ( piState[ iState - 1 ] + piState[ iState - 2 ] )
			& ( ( 1 << 30 ) - 1 );
    }

    return;
}


int number_mm( void )
{
    int *piState;
    int  iState1;
    int  iState2;
    int  iRand;

    piState		= &rgiState[ 2 ];
    iState1		= piState[ -2 ];
    iState2		= piState[ -1 ];
    iRand		= ( piState[ iState1 ] + piState[ iState2 ] )
			& ( ( 1 << 30 ) - 1 );
    piState[ iState1 ]	= iRand;

    if ( ++iState1 == 55 )
	iState1 = 0;

    if ( ++iState2 == 55 )
	iState2 = 0;

    piState[ -2 ]	= iState1;
    piState[ -1 ]	= iState2;

    return iRand >> 6;
}


/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int idice;
    int sum;

    switch ( size )
    {
	case 0: return 0;
	case 1: return number;
    }

    for ( idice = 0, sum = 0; idice < number; idice++ )
	sum += number_range( 1, size );

    return sum;
}


/*
 * Simple linear interpolation.
 */
/*int interpolate( int level, int value_00, int value_47 )
{
    return value_00 + level * ( value_47 - value_00 ) / 47;
}*/

int interpolate( int level, int value_00, int value_100 )
{
    return value_00 + level * ( value_100 - value_00 ) / 100;
}


/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 *
 * Lam 19.2.2005: jest to funkcja kontrolujaca (przynajmniej teoretycznie)
 *		wszystkie lancuchy wprowadzane przez graczy i zapisywane
 *		w plikach. Dlatego jest idealna do blokady { na koncu linii.
 */
void smash_tilde( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
	if ( *str == '~' )
	    *str = '-';

	if ( *str == '{' && *( str + 1 ) == '\0' )
	{
	    *str = '\0';
	    break;
	}

	if ( *str == '{' && *( str + 1 ) == '{' )
	    str++;
    }

    return;
}


/*
 * Lam 26.10.2004: zwraca prawde, jesli astr jest alfabetycznie wczesniej niz
 *			bstr, nie zwraca uwagi na wielkosc liter
 * NIE OBSLUGUJE POROWNYWANIA LANCUCHA Z POLSKIMI Z TAKIM BEZ POLSKICH LITER
 */
bool alf_wczes( const char *astr, const char *bstr )
{
    if ( !astr )
    {
	bug( "Str_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( !bstr )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
	if ( LOWER( *astr ) != LOWER( *bstr ) )
	{
	    /* lewa strona polska, prawa normalna */
	    if ( *astr == '`' )
	    {
		astr++;
		if ( LOWER( *astr ) == 'x' )
		    return FALSE; /* 'z' < '`x' */
		return ( LOWER( *astr ) < LOWER( *bstr ) );
	    }
	    /* prawa strona polska, lewa normalna */
	    else if ( *bstr == '`' )
	    {
		bstr++;
		if ( LOWER( *bstr ) == 'x' )
		    return TRUE; /* 'z' < '`x' */
		return ( LOWER( *astr ) <= LOWER( *bstr ) ); /* 'z' < '`z' */
	    }
	    /* obie normalne: zwykle porownanie, czy <
		obie polskie: to samo dziala, `x < `z
		jedna to \0: krotszy ciag wyjdzie <, prawidlowo */
	    else return ( LOWER( *astr ) < LOWER( *bstr ) );
	}

    return FALSE; /* sa rowne, cokolwiek */
}


/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
    if ( !astr )
    {
	bug( "Str_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( !bstr )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
	if ( LOWER( *astr ) != LOWER( *bstr ) )
	{ /* uwaga: od razu wiadomo ze tylko 1 lancuch ma polska literke */
	    if ( !*astr || !*bstr )
		return TRUE;

	    if ( *astr == '`' )
	    {
		astr++;
		if ( LOWER( *astr ) == LOWER( *bstr ) )
		    continue;
		if ( LOWER( *astr ) == 'x' && LOWER( *bstr ) == 'z' )
		    continue;
		return TRUE;
	    }

	    if ( *bstr == '`' )
	    {
		bstr++;
		if ( LOWER( *astr ) == LOWER( *bstr ) )
		    continue;
		if ( LOWER( *astr ) == 'z' && LOWER( *bstr ) == 'x' )
		    continue;
		return TRUE;
	    }

	    return TRUE;
	}

    return FALSE;
}


/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( !astr )
    {
	bug( "Str_prefix: null astr.", 0 );
	return TRUE;
    }

    if ( !bstr )
    {
	bug( "Str_prefix: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr; astr++, bstr++ )
	if ( LOWER( *astr ) != LOWER( *bstr ) )
	{
	    if ( !*astr || !*bstr )
		return TRUE;

	    if ( *astr == '`' )
	    {
		astr++;
		if ( LOWER( *astr ) == LOWER( *bstr )
		  || ( LOWER( *astr ) == 'x' && LOWER( *bstr ) == 'z' ) )
		{
		    continue;
		}
		return TRUE;
	    }

	    if ( *bstr == '`' )
	    {
		bstr++;
		if ( LOWER( *astr ) == LOWER( *bstr )
		  || ( LOWER( *astr ) == 'z' && LOWER( *bstr ) == 'x' ) )
		{
		    continue;
		}
		return TRUE;
	    }

	    return TRUE;
	}

    return FALSE;
}


/* 2 ponizsze funkcje zostaly spolszczone automatycznie jako ze uzywaja
   str_cmp i str_prefix do swoich celow - Lam 5.9.98 */

/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    char c0;
    int  sstr2;
    int  ichar;

    if ( ( c0 = LOWER( astr[ 0 ] ) ) == '\0' )
	return FALSE;

    sstr2 = strlen( bstr );

    for ( ichar = 0; ichar <= sstr2; ichar++ )
	if ( c0 == LOWER( bstr[ ichar ] ) && !str_prefix( astr, bstr + ichar ) )
	    return FALSE;

    return TRUE;
}


/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 * FIXME: ta funkcja zle dziala (liczy dlugosci, a moga byc rozne z polskimi
 * literami). Aktualnie uzywana wylacznie do porownywania domen w check_ban,
 * wiec nie jest to problemem.
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen( astr );
    sstr2 = strlen( bstr );
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
	return FALSE;

    return TRUE;
}


/*
 * Zastepuje strstr, obslugujac polskie znaczki
 * Lam 22.3.99
 * kolejnosc parametrow taka jak w strstr(), odwrotna od str_* powyzej
 */
char *str_str( char *bstr, const char *astr )
{
    int sstr2;
    int ichar;

    if ( !astr[ 0 ] )
	return bstr;

    sstr2 = strlen( bstr );

    for ( ichar = 0; ichar <= sstr2; ichar++ )
	if ( !str_prefix( astr, bstr + ichar ) )
	    return bstr + ichar;

    return NULL;
}


/*
 * Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
    static char strcap [ MAX_STRING_LENGTH ];
	   int  i;
	   int  j = 0;

    for ( i = 0; str[ i ] != '\0'; i++ )
	if ( str[ i ] != '`' )
	    strcap[ j++ ] = LOWER( str[ i ] );
	else if ( str[ i+1 ] == 'x' || str[ i+1 ] == 'X' )
	{
	    i++;
	    strcap[ j++ ] = 'z';
	}

    strcap[ j ] = '\0';
    strcap[ 0 ] = UPPER( strcap[ 0 ] );

    return strcap;
}


char *zwielkoliteruj( const char *str )
{
    static char buf[ MSL ];
    int  i, j = 0;

    for ( i = 0; str[ i ] != '\0'; i++ )
	buf[ j++ ] = LOWER( str[ i ] );

    buf[ j ] = '\0';
    if ( buf[ 0 ] == '`' )
	buf[ 1 ] = UPPER( buf[ 1 ] );
    else
	buf[ 0 ] = UPPER( buf[ 0 ] );

    return buf;
}


/*
 * Funkcja ta pomaga ustalic koncowke fleksyjna na podstawie podanej liczby.
 * Zwracane wartosci na przykladzie wyrazu "notka":
 * 0, jesli "ek" (0 notek, 12 notek, 55 notek)
 * 1, jesli "ka" (1 notka)
 * 2, jesli "ki" (2 notki, 22 notki)
 */
int liczba( int l )
{
    if ( l < 0 )
	l = 0 - l;

    /* 1 i 0 to latwe przypadki */
    if ( l == 1 || l == 0 )
	return l;

    /* reszta z dzielenia przez 10 wieksza niz 4 lub 0 lub 1 */
    /* zgadza sie rowniez z "nascie" */
    if ( l % 10 > 4 || l % 10 < 2 )
	return 0;

    /* wylapanie liczb "nascie" */
    if ( l % 100 > 4 && l % 100 < 21 )
	return 0;

    /* nic innego nie pozostaje */
    return 2;
}


/*
 * Lam 19.10.2001
 * zamienia liczbe na postac slowna
 * podziekowania: janekp dal mi natchnienie do napisania tego
 * PS. nie chcialo mi sie pisac obslugi ujemnych, chociaz to by zajelo 3 linijki
 */
char *itopl( unsigned int l, int plec )
{
    const struct
    {
	char *poj; /* mianownik liczby pojedynczej */
	char *mn; /* mianownik liczby mnogiej */
	char *dopmn; /* dopelniacz liczby mnogiej */
    } liczby[ ] =
    {
	{ "", "", "" },
	{ "tysi`ac", "tysi`ace", "tysi`ecy" },
	{ "milion", "miliony", "milion`ow" },
	{ "miliard", "miliardy", "miliard`ow" },
	/* maksymalnie dla 31bitowego inta moze wyjsc 2 miliardy... */
	{ "bilion", "biliony", "bilion`ow" },
	{ "biliard", "biliardy", "biliard`ow" },
	{ NULL, NULL, NULL }
    };

    /* Lam 5.12.2003: obsluga plci, 1 i 2 obslugiwane specjalnie dalej */
    const char *liczby1[ ] = { "*zero*", "*jeden*", "*dwa*", "trzy", "cztery",
			"pi`e`c", "sze`s`c", "siedem", "osiem", "dziewi`e`c",
			"dziesi`e`c", "jedena`scie", "dwana`scie",
			"trzyna`scie", "czterna`scie", "pi`etna`scie",
			"szesna`scie", "siedemna`scie", "osiemna`scie",
			"dziewi`etna`scie" };
    const char *liczby10[ ] = { "*zero*", "dziesi`e`c", "dwadzie`scia",
			"trzydzie`sci", "czterdzie`sci", "pi`e`cdziesi`at",
			"sze`s`cdziesi`at", "siedemdziesi`at",
			"osiemdziesi`at", "dziewi`e`cdziesi`at" };
    const char *liczby100[ ] = { "*zero*", "sto", "dwie`scie", "trzysta",
			"czterysta", "pi`e`cset", "sze`s`cset", "siedemset",
			"osiemset", "dziewi`e`cset" };

    static char zwrot[ MAX_STRING_LENGTH ];
    char tymczas[ MAX_STRING_LENGTH ];
    int r, u, poziom = 0;

    if ( !l )
    {
	strcpy( zwrot, "zero" );
	return zwrot;
    }

    zwrot[ 0 ] = '\0';

    if ( l == 1 )
    {
	switch ( plec )
	{
	    case 2: strcpy( zwrot, "jedna" ); break; /* jedna tarcza */
	    case 0: strcpy( zwrot, "jedno" ); break; /* jedno jajko */
	    default: strcpy( zwrot, "jeden" ); break; /* jeden miecz */
	}
	return zwrot;
    }

    while ( l )
    {
	tymczas[ 0 ] = '\0';
	r = l % 1000;
	l = l / 1000;
	u = r / 100;
	if ( u ) /* sto, Xset */
	{
	    strcat( tymczas, liczby100[ u ] );
	    strcat( tymczas, " " );
	}
	u = ( r % 100 ) / 10;
	if ( u > 1 ) /* dwadziescia, Xdziesiat */
	{
	    strcat( tymczas, liczby10[ u ] );
	    strcat( tymczas, " " );
	    u = r % 10;
	    if ( u ) /* dwadziescia X, *dziesiat X, sto X, *set X */
	    {
		if ( u == 2 )
		{
		    switch ( plec )
		    {
			case 2: strcat( tymczas, "dwie" ); break; /* dwie tarcze */
			default: strcat( tymczas, "dwa" ); break; /* dwa jajka, dwa miecze */
		    }
		}
		else if ( u == 1 )
		    strcat( tymczas, "jeden" );
		else
		    strcat( tymczas, liczby1[ u ] );
		strcat( tymczas, " " );
	    }
	}
	else if ( r % 100 ) /* nascie, 1-10 */
	{
	    u = r % 100;
	    if ( u == 2 )
	    {
		switch ( plec )
		{
		    case 2: strcat( tymczas, "dwie" ); break; /* dwie tarcze */
		    default: strcat( tymczas, "dwa" ); break; /* dwa jajka, dwa miecze */
		}
	    }
	    else if ( u == 1 )
		strcat( tymczas, "jeden" );
	    else
		strcat( tymczas, liczby1[ r % 100 ] );
	    strcat( tymczas, " " );
	}

	/* usun ponizsze jesli ma pokazywac "jeden miliard jeden milion"
	   zamiast "miliard milion" */
	if ( r == 1 && poziom ) tymczas[ 0 ] = '\0';

	if ( r )
	{
	    if ( liczba( r ) == 2 ) strcat( tymczas, liczby[ poziom ].mn );
	    else if ( liczba( r ) == 1 ) strcat( tymczas, liczby[ poziom ].poj );
	    else strcat( tymczas, liczby[ poziom ].dopmn );
	}

	if ( tymczas[ 0 ] )
	    strcat( tymczas, " " );
	if ( !poziom )
	    while ( tymczas[ strlen( tymczas ) - 1 ] == ' ' )
		tymczas[ strlen( tymczas ) - 1 ] = '\0';

	strcat( tymczas, zwrot );
	strcpy( zwrot, tymczas );

	poziom++;
    }

    return zwrot;
}


/*
 * Lam 18.5.2005
 * ciezar podany po lacowemu, w cwiartkach kilogramow
 */
char *slowny_ciezar( int ciezar )
{
    int kg = ciezar / 4;
    int reszta = ciezar % 4;
    int pzl;
    char *przed, *po, *jedn;
    static char wyjscie[ 1024 ];

    if ( !kg )
    {
	switch ( reszta )
	{
	    default: return "zupe`lnie nic";
	    case 1: return "prawie nic";
	    case 2: return "p`o`l kilograma";
	    case 3: return "prawie kilogram";
	}
    }

    if ( kg == 1 )
    {
	switch ( reszta )
	{
	    case 0: return "kilogram";
	    case 1: return "ponad kilogram";
	    case 2: return "p`o`ltora kilograma";
	}
    }

    przed = "";
    po = "";
    if ( reszta == 3 )
    {
	kg++;
	przed = "prawie ";
    }
    else if ( reszta == 1 )
	przed = "ponad ";

    if ( reszta == 2 )
    {
	po = "i p`o`l ";
	jedn = "kilograma";
    }
    else
    {
	pzl = liczba( kg );
	if ( pzl == 1 )
	    jedn = "kilogram";
	else if ( pzl == 2 )
	    jedn = "kilogramy";
	else
	    jedn = "kilogram`ow";
    }

    sprintf( wyjscie, "%s%s %s%s", przed, itopl( kg, 1 ), po, jedn );

    return wyjscie;
}


/*
 * Dosyc pozno napisalem te funkcje. Zwraca ona dlugosc lancucha z polskimi
 * literami w standardzie lacowym z `. Uzywana glownie wtedy kiedy trzeba
 * wyrownywac tekst zawierajacy polskie literki.
 * (c) Lam 6.9.98
 * Lam 18.10.2002: dopiero dzisiaj wyszedl na jaw fundamentalny i trywialny
 * blad logiki w tej funkcji, przez ktory ona zle dzialala.
 */
int strlen_pl( const char *txt )
{
    int pzl;

    for ( pzl = 0; *txt; )
	if ( *txt == '`' )
	{
	    txt++;
	    if ( *txt == '`' )
	    {
		txt++;
		pzl++;
	    }
	}
	else
	{
	    txt++;
	    pzl++;
	}

    return pzl;
}


/* wersja wycinajaca kolory */
int strlen_pl_kol( const char *txt )
{
    int pzl;

    for ( pzl = 0; *txt; )
	if ( *txt == '`' )
	{
	    txt++;
	    if ( *txt == '`' )
	    {
		txt++;
		pzl++;
	    }
	}
	else if ( *txt == '{' )
	{
	    txt++;
	    if ( *txt == '{' )
		pzl++;
	    /* Uwaga: trudno okreslic, jakiej dlugosci jest lancuch, jesli na
	       koncu ma { (ten znak moze albo zezrec kolejny, albo pokazac
	       { i cos za nia). To powinna sprawdzac funkcja wywolujaca. */
	    if ( *txt )
		txt++;
	}
	else
	{
	    txt++;
	    pzl++;
	}

    return pzl;
}


/* wersja wycinajaca kolory i liczaca $y jako jeden znak (przemiel_opis) */
int strlen_pl_kol_miel( const char *txt )
{
    int pzl;

    for ( pzl = 0; *txt; )
    {
	if ( *txt == '`' )
	{
	    txt++;
	    if ( *txt == '`' )
	    {
		txt++;
		pzl++;
	    }
	}
	/* przemiel_opis podmienia literki a/e/y, ale inne zostawia, nie
	   wycina nieznanych "kodow", wiec tak wystarczy: */
	if ( *txt == '$' )
	{
	    txt++;
	    if ( *txt == '$' )
	    {
		txt++;
		pzl++;
	    }
	}
	else if ( *txt == '{' )
	{
	    txt++;
	    if ( *txt == '{' )
		pzl++;
	    txt++;
	}
	else
	{
	    txt++;
	    pzl++;
	}
    }

    return pzl;
}


/*
 * Jesli strlen_pl juz mamy, nie ma problemu z wyrownywaniem tekstu. Sluzy do
 * tego ponizsza funkcja. Przykladowo zamiast 'printf( "%10s", text )' teraz
 * nalezy pisac 'printf( "%s", wyrownaj( text, 10 ) )'. Argument funkcji
 * wyrownaj jest taki sam jak miedzy % i s w printf() - ujemny robi spacje
 * z drugiej strony.
 * (c) Lam 11.9.98
 * Lam 5.4.2000: 4 bufory, zeby mozna bylo w *printf stosowac kilka razy
 */
char *wyrownaj( const char *text, int dlug )
{
    bool minus = FALSE;
    static char zwrot[ 4 ][ MIL ];
    static int ktory;
    char spacje[ MIL ];
    int dltmp = strlen_pl_kol( text );

    if ( ++ktory > 3 )
	ktory = 0;

    if ( dlug < 0 )
    {
	dlug = 0 - dlug; /* chyba szybciej niz abs(), bo i tak mam ifa powyzej */
	minus = TRUE;
    }

    if ( dltmp >= dlug )
    {
	strcpy( zwrot[ ktory ], text );
	return zwrot[ ktory ];
    }

    memset( spacje, ' ', dlug - dltmp );
    spacje[ dlug - dltmp ] = '\0';
    if ( minus )
    {
	strcpy( zwrot[ ktory ], text );
	strcat( zwrot[ ktory ], spacje );
    }
    else
    {
	strcpy( zwrot[ ktory ], spacje );
	strcat( zwrot[ ktory ], text );
    }

    return zwrot[ ktory ];
}


/*
 * Lam 6.1.2008: szuka adresu bajtu w ciagu, ktory odpowiada kolejnej literze.
 * Tym sposobem moge przyciac tekst na piatym znaku, albo wyswietlic od piatego
 * znaku.
 */
int znajdz_adres_pl_kol( const char *txt, int literka )
{
    int pzl;
    const char *start = txt;

    pzl = 0;
    while ( *txt && pzl != literka ) /* wnetrze petli zwiekszy pzl najwyzej o 1 */
    {
	if ( *txt == '`' )
	{
	    txt++;
	    if ( *txt == '`' )
	    {
		txt++;
		pzl++;
	    }
	}
	else if ( *txt == '{' )
	{
	    txt++;
	    if ( *txt == '{' )
		pzl++;
	    /* Uwaga: trudno okreslic, jakiej dlugosci jest lancuch, jesli na
	       koncu ma { (ten znak moze albo zezrec kolejny, albo pokazac
	       { i cos za nia). To powinna sprawdzac funkcja wywolujaca. */
	    if ( *txt )
		txt++;
	}
	else
	{
	    txt++;
	    pzl++;
	}
    }

    return txt - start;
}


/*
 * Formatuje liczbe w polski sposob, z kropkami jako separatorami tys/mln
 * Lam 24.9.98 (uzycie: do_punktacja)
 */
char *formatuj_liczbe( int arg )
{
    static char zwrot[ MIL ];
    bool tak = FALSE;

    zwrot[ 0 ] = '\0';

    if ( arg / 1000000000 )
    {
	sprintf( zwrot, "%d.", arg / 1000000000 );
	tak = TRUE;
    }

    if ( ( arg % 1000000000 ) / 1000000 || tak )
    {
	if ( tak )
	    sprintf( zwrot + strlen( zwrot ), "%03d.", ( arg % 1000000000 ) / 1000000 );
	else
	{
	    sprintf( zwrot, "%d.", arg / 1000000 );
	    tak = TRUE;
	}
    }

    if ( ( arg % 1000000 ) / 1000 || tak )
    {
	if ( tak )
	    sprintf( zwrot + strlen( zwrot ), "%03d.", ( arg % 1000000 ) / 1000 );
	else
	{
	    sprintf( zwrot, "%d.", arg / 1000 );
	    tak = TRUE;
	}
    }

    if ( tak )
	sprintf( zwrot + strlen( zwrot ), "%03d", arg % 1000 );
    else
	sprintf( zwrot, "%d", arg );

    return &zwrot[ 0 ];
}


/*
 * Wyciagasz z pudelka, wyciagasz ze skrzyni.
 * "ze" jesli pierwsza litera wyrazu to 's', 'z', '`s', '`x', '`z', a druga
 * jest spolgloska.
 * Nie wiem, czy jest taka zasada, tak sobie to wymyslilem. W razie czego
 * zglaszac blad.
 * Lam 12.10.2002
 */
bool czy_ze( const char *slowo )
{
    bool polska = ( *slowo == '`' );

    if ( polska )
	slowo++;

    /* Lam 7.3.2006: jesli na poczatku jest "sz", traktuje jak jedna litere
       (bo to jedna gloska), pomijam "s", dalsza czesc algorytmu zadziala
       dla samego "z" tak samo dobrze */
    if ( !polska && *slowo == 's' && *( slowo + 1 ) == 'z' )
	slowo++;

    if ( *slowo == 's' || *slowo == 'z' || ( polska && *slowo == 'x' ) )
    {
	/* samogloski z ` zawsze zostaja samogloskami, czy to `o czy `u */
	if ( *++slowo == '`' )
	    slowo++;

	if ( !strchr( "aeiouy", *slowo ) )
	    return TRUE;
    }

    return FALSE;
}


/* Lam 29.3.2006 */
bool czy_we( const char *slowo )
{
    if ( *slowo == 'w' )
    {
	/* samogloski z ` zawsze zostaja samogloskami, czy to `o czy `u */
	if ( *++slowo == '`' )
	    slowo++;

	if ( !strchr( "aeiouy", *slowo ) )
	    return TRUE;
    }

    return FALSE;
}


/*
 * Lam 13.12.2002 (piatek)
 * Na potrzeby sprawdzania bluzgow, w przyszlosci dla progow
 */
void odkoloruj( const char *arg, char *bufor )
{
    while ( *arg )
    {
	if ( *arg == '{' )
	{
	    arg++;

	    /* Lam 12.10.2004: wywalalo takze {{ */
	    if ( *arg == '{' )
	    {
		*bufor++ = '{';
		*bufor++ = '{';
	    }
	    if ( *arg )
		arg++;

	    continue;
	}

	*bufor++ = *arg++;
    }

    *bufor = '\0';
}


int ile_kolorow( const char *arg )
{
    int ile = 0;

    while ( *arg )
    {
	if ( *arg == '{' )
	{
	    arg++;

	    if ( *arg != '{' )
		ile++;

	    if ( *arg )
		arg++;

	    continue;
	}

	arg++;
    }

    return ile;
}


/* Lam j.w. */
bool czy_bluzg1( const char *arg )
{
    int i;

    for ( i = 0; offensive_table[ i ]; i++ )
	if ( !str_cmp( arg, offensive_table[ i ] ) )
	    return TRUE;

    return FALSE;
}


bool czy_bluzg( const char *arg )
{
    char odkolorowane[ MIL ];
    char wyraz[ MIL ];
    char *wsk = odkolorowane;

    odkoloruj( arg, odkolorowane );
    while ( *wsk )
    {
	if ( !isalpha( (int) *wsk ) && *wsk != '`' )
	    *wsk = ' ';
	wsk++;
    }

    wsk = odkolorowane;
    do
    {
	wsk = one_argument( wsk, wyraz );

	if ( *wyraz && czy_bluzg1( wyraz ) )
	    return TRUE;
    }
    while ( *wyraz );

    return FALSE;
}


/* Lam 15.12.2002 */
struct miodek_data *znajdz_miodka( const char *arg )
{
    int i;

    for ( i = 0; miodek_table[ i ].slowo; i++ )
	if ( !str_cmp( arg, miodek_table[ i ].slowo ) )
	    return &miodek_table[ i ];

    return NULL;
}


/* Lam 15-20.12.2002, pomoc przy komunikacie - Seasee */
bool miodek( CHAR_DATA *ch, char *arg )
{
    char odkolorowane[ MIL ], wyraz[ MIL ], podpowiedz[ MSL ];
    char *wsk = odkolorowane;
    struct miodek_data *miod;

    odkoloruj( arg, odkolorowane );
    while ( *wsk )
    {
	if ( !isalnum( (int) *wsk ) && *wsk != '`' )
	    *wsk = ' ';
	wsk++;
    }

    podpowiedz[ 0 ] = '\0';
    wsk = odkolorowane;
    do
    {
	wsk = one_argument( wsk, wyraz );
	if ( *wyraz && ( miod = znajdz_miodka( wyraz ) ) )
	{
	    if ( *podpowiedz )
		strcat( podpowiedz, ", " );
	    strcat( podpowiedz, miod->zastepstwo );
	}
    } while ( *wyraz );

    if ( *podpowiedz )
    {
	ASTC( "Twoje bezbo`zne s`lowa staj`a ci kamieniem w gardle! Jeste`s tak przera`zon$y swoim", ch );
	STC( "niecnym post`epkiem, `ze przez chwil`e nie mo`zesz niczego z siebie wydusi`c.\n\r", ch );
	ch_printf( ch, "Aby unikn`a`c tego w przysz`lo`sci, mo`zesz pisa`c na przyk`lad: %s.\n\r", podpowiedz );
	WAIT_STATE( ch, 3 * PULSE_PER_SECOND ); /* 3 sekundy wystarcza? */
	return TRUE;
    }

    return FALSE;
}


bool miodek_opisowy( CHAR_DATA *ch, char *arg )
{
    char odkolorowane[ MIL ], wyraz[ MIL ], podpowiedz[ MSL ];
    char *wsk = odkolorowane;
    struct miodek_data *miod;

    odkoloruj( arg, odkolorowane );
    while ( *wsk )
    {
	if ( !isalnum( (int) *wsk ) && *wsk != '`' )
	    *wsk = ' ';
	wsk++;
    }

    podpowiedz[ 0 ] = '\0';
    wsk = odkolorowane;
    do
    {
	wsk = one_argument( wsk, wyraz );
	if ( *wyraz && ( miod = znajdz_miodka( wyraz ) ) )
	{
	    if ( *podpowiedz )
		strcat( podpowiedz, "{x,{y " );
	    strcat( podpowiedz, miod->zastepstwo );
	}
    } while ( *wyraz );

    if ( *podpowiedz )
    {
	ASTC( "Nie mo`zesz u`zywa`c tak bezbo`znych s`l`ow! Jeste`s tak przera`zon$y swym niecnym", ch );
	STC( "post`epkiem, `ze przez chwil`e nie mo`zesz niczego napisa`c. Aby unikn`a`c tego w\n\r", ch );
	ch_printf( ch, "przysz`lo`sci, pisz poprawnie. W szczeg`olno`sci:\n\r{c-> {y%s{x.\n\r", podpowiedz );
	WAIT_STATE( ch, 3 * PULSE_PER_SECOND ); /* 3 sekundy wystarcza? */
	return TRUE;
    }

    return FALSE;
}


/*
 * Lam 4.11.2005 - czasami nie da sie dobrze odplciowic tematu pomocy lub
 * jakiegos opisu.
 */
char *przemiel_opis( CHAR_DATA *ch, char *opis )
{
    /* wzorowane na act( ) */
    static char const y_a_e[ ] = { 'e', 'y', 'a' }; /* zabit[y/a/e] */
    static char const a_o[ ] = { 'o', ' ', 'a' }; /* uciek`l/uciek`la/uciek`lo */
    static char const e_a_e[ ] = { 'e', 'e', 'a' }; /* uciek`l[e/a/e]`s */
    static char wyjscie[ MSL * 4 ];
    char *p = wyjscie;
    int i = 0;
    /* Vigud 5.3.2011: zmiana URANGE na UMIN, ch->sex nie moze byc ujemne */
    int plec = ch ? UMIN( ch->sex, 2 ) : 1;

    while ( *opis )
    {
	if ( *opis == '$' )
	{
	    opis++;
	    switch ( *opis )
	    {
		case 'e': *p = e_a_e[ plec ]; break;
		case 'a': *p = a_o[ plec ]; break;
		case 'y': *p = y_a_e[ plec ]; break;
		default: *p = *opis; break;
	    }
	    if ( *p != ' ' ) /* a_o[ 1 ], czyli $a dla faceta skroci opis */
	    {
		p++;
		i++;
	    }
	    opis++;
	}
	else
	{
	    *p++ = *opis++;
	    i++;
	}

	if ( i == MSL * 4 - 2 )
	{
	    bug( "Za dlugi opis", 0 );
	    break;
	}
    }

    *p = '\0';

    return wyjscie;
}


/*
 * We wszystkich funkcjach gdzie bylo 'all', teraz bedzie
 * 'wszystko' albo 'wszyscy' (dlatego dwie funkcje). Chodzi o to, zeby
 * dalo sie skrocic do 3 znakow. Ew. moglbym to zrobic makrami, ale nie
 * wiem sam gdzie bym tego chcial uzyc, wiec sa funkcje.
 * Lam 24.9.98
 */
bool czy_wszystko( char *arg )
{
    if ( strlen( arg ) < 3
      || str_prefix( arg, "wszystko" ) )
    {
	return FALSE;
    }

    return TRUE;
}


bool czy_wszyscy( char *arg )
{
    if ( strlen( arg ) < 3
      || str_prefix( arg, "wszyscy" ) )
    {
	return FALSE;
    }

    return TRUE;
}


bool czy_monety( char *arg ) /* Tissarius */
{
    return ( strlen( arg ) >= 3 && !str_prefix( arg, "monety" ) )
	|| !str_cmp( "moneta", arg ) || !str_cmp( "monet`e", arg );
}


/* dla not do wszystkich i do kogos */
bool czy_do_wszystkich( char *arg )
{
    char jeden[ MIL ];

    for ( ; ; )
    {
	arg = one_argument( arg, jeden );
	if ( !*jeden )
	    return FALSE;
	if ( strlen( jeden ) < 3 )
	    continue;
	if ( !str_prefix( jeden, "wszyscy" )
	  || !str_prefix( jeden, "wszystkich" ) )
	{
	    return TRUE;
	}
    }

    return FALSE;
}


/*
 * See if a string is one of the names of an object.
 * New is_name sent in by Alander.
 */
bool is_name( const char *str, char *namelist )
{
    char name[ MAX_INPUT_LENGTH ];

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[ 0 ] == '\0' )
	    return FALSE;
	if ( !str_prefix( str, name ) )
	    return TRUE;
    }

    return FALSE;
}


bool is_name2( const char *str, char *namelist )
{
    char name[ MAX_INPUT_LENGTH ];

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[ 0 ] == '\0' )
	    return FALSE;
	if ( !str_cmp( str, name ) )
	    return TRUE;
    }

    return FALSE;
}


bool czy_mail( char *arg )
{
    bool fAt = FALSE;
    bool fDot = FALSE;

    for ( ; *arg; arg++ )
    {
	if ( *arg == '@' )
	    fAt = TRUE;

	if ( *arg == '.' && fAt )
	    fDot = TRUE;

	if ( *arg == '~' )
	    return FALSE; /* Lam */
    }

    return fAt && fDot;
}


int zetnij( int limit, int tempo, int x )
{
    return limit - limit * tempo / ( tempo + UMAX( 0, x ) );
}


char *zlota_moneta( int ile, bool fMianownik, bool fIle )
{
    static char bufek[ 4 ][ MAX_INPUT_LENGTH ];
    static int  numer;
    int licz = liczba( ile );

    if ( ++numer > 3 ) numer = 0;
    bufek[ numer ][ 0 ] = '\0';

    if ( fIle )
	sprintf( bufek[ numer ], "%d ", ile );

    if ( fMianownik )
    {
	if ( licz == 1 )
	    strcat( bufek[ numer ], "z`lota moneta" );
	else if ( licz == 2 )
	    strcat( bufek[ numer ], "z`lote monety" );
	else
	    strcat( bufek[ numer ], "z`lotych monet" );
    }
    else
    {
	if ( licz == 1 )
	    strcat( bufek[ numer ], "z`lot`a monet`e" );
	else if ( licz == 2 )
	    strcat( bufek[ numer ], "z`lote monety" );
	else
	    strcat( bufek[ numer ], "z`lotych monet" );
    }

    return bufek[ numer ];
}


/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
    if ( *arg == '\0' )
	return FALSE;

    if ( *arg == '+' || *arg == '-' )
	arg++;

    for ( ; *arg != '\0'; arg++ )
	if ( !isdigit( (int) *arg ) )
	    return FALSE;

    return TRUE;
}


/*
 * Lam 20.4.2003
 */
bool is_ipaddr( char *arg )
{
    if ( *arg == '\0' )
	return FALSE;

    for ( ; *arg != '\0'; arg++ )
	if ( !isdigit( (int) *arg ) && *arg != '.' )
	    return FALSE;

    return TRUE;
}


/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int   number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}

    strcpy( arg, argument );

    return 1;
}


/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace( (int) *argument ) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd
	  || ( cEnd == ' ' && isspace( (int) *argument ) ) )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER( *argument );
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace( (int) *argument ) )
	argument++;

    return argument;
}


/*
 * Ta wersja nie zabija wielkich liter.
 */
char *one_argument2( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace( (int) *argument ) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd
	  || ( cEnd == ' ' && isspace( (int) *argument ) ) )
	{
	    argument++;
	    break;
	}
	*arg_first = *argument;
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace( (int) *argument ) )
	argument++;

    return argument;
}


/* Lam */
int drzwi_d( char *slowo )
{
    char pom[ 2 ];

    if ( *slowo >= '0' && *slowo <= '9' )
	return *slowo - '0';
    else
    {
	pom[ 0 ] = UPPER( slowo[ 0 ] );
	pom[ 1 ] = UPPER( slowo[ 1 ] );
	switch ( pom[ 0 ] )
	{
	    default:
		break;
	    case 'N':
		if ( pom[ 1 ] == 'E' )
		    return 6;
		else if ( pom[ 1 ] == 'W' )
		    return 8;
		else return 0;
	    case 'S':
		if ( pom[ 1 ] == 'E' )
		    return 7;
		else if ( pom[ 1 ] == 'W' )
		    return 9;
		else return 2;
	    case 'E': return 1;
	    case 'W': return 3;
	    case 'U':
	    case 'G': return 4;
	    case 'D': return 5;
	}
    }

    return -1;
}


/* Lam: chcialem uzyc metody K&R, ale znalazlem to:
   http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel */
int zlicz_bity( unsigned int liczba )
{
    liczba = liczba - ( ( liczba >> 1 ) & 0x55555555 );
    liczba = ( liczba & 0x33333333 ) + ( ( liczba >> 2 ) & 0x33333333 );

    return ( ( ( liczba + ( liczba >> 4 ) ) & 0xf0f0f0f ) * 0x1010101 ) >> 24;
}


void zjedz_entery_z_konca( char **lancuch )
{
    char buf[ MSL ];
    int i, j;

    i = j = strlen( *lancuch ) - 1;
    strcpy( buf, *lancuch );

    while ( i > -1
	 && ( buf[ i ] == '\n'
	   || buf[ i ] == '\r' ) )
    {
	i--;
    }

    if ( i != j )
    {
	buf[ i + 1 ] = '\0';
	free_string( *lancuch );
	*lancuch = str_dup( buf );
    }

    return;
}
