/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   ___           _______     ________      |                             *
 *  |###|         |#######|   |########| (R) |            GLACA            *
 *  |###|        |###|^|###| |###|^^|###|    |                             *
 *  |###|        |###| |###| |###|   ^^^     |                             *
 *  |###|        |###|_|###| |###|           |            Vigud            *
 *  |###|        |#########| |###|           |             Lam             *
 *  |###|        |###|^|###| |###|           |                             *
 *  |###|        |###| |###| |###|   ___     |                             *
 *  |###|______  |###| |###| |###|__|###|    |                             *
 *  |##########| |###| |###|  |########|     |                             *
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
 *  lpl.c przystosowany do potrzeb GLACY                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "glaca.h"
#include <wchar.h>
#include <wctype.h>
#include <iconv.h>

/* Lam 11.10.2001 */
struct
{
    int uni;
    unsigned char iso;
    char *lac;
} uni_lac[ ] =
{
/* tylko potrzebne Lacowi znaki z Unikodu */
    {	0x0d3,	'Ó',	"`O"	}, {	0x0f3,	'ó',	"`o"	},
    {	0x104,	'¡',	"`A"	}, {	0x105,	'±',	"`a"	},
    {	0x106,	'Æ',	"`C"	}, {	0x107,	'æ',	"`c"	},
    {	0x118,	'Ê',	"`E"	}, {	0x119,	'ê',	"`e"	},
    {	0x141,	'£',	"`L"	}, {	0x142,	'³',	"`l"	},
    {	0x143,	'Ñ',	"`N"	}, {	0x144,	'ñ',	"`n"	},
    {	0x15a,	'¦',	"`S"	}, {	0x15b,	'¶',	"`s"	},
    {	0x179,	'¬',	"`X"	}, {	0x17a,	'¼',	"`x"	},
    {	0x17b,	'¯',	"`Z"	}, {	0x17c,	'¿',	"`z"	},
    {	0x000,	'\0',	NULL	}
};

extern iconv_t iso_uni;
extern iconv_t uni_iso;

/* uwaga, ta funkcja nie obsluguje unikodu ;) */
char polska( char *znak )
{
    int nr;
    char pom;

    for ( nr = 0; nr < 18; nr++ )
	if ( *polska_tablica[ 0 ][ nr ] == *znak )
	{
	    *znak = 0;
	    return *polska_tablica[ STANDARD ][ nr ];
	}

    pom = *znak; /* wyciecie `, jesli wystepuje po nim niepoprawna literka */
    *znak = 0;

    return pom;
}


char angielska( char *znak )
{
    int nr;
    char pom;

    if ( *znak == '`' )
	return '`';

    for ( nr = 0; nr < 18; nr++ )
	if ( *polska_tablica[ STANDARD ][ nr ] == *znak )
	{
	    *znak = *polska_tablica[ 0 ][ nr ];
	    return '`';
	}

    pom = *znak;
    *znak = 0;

    return pom;
}


char *pols_z_lac_len( const char *wej, int len )
{
    char znak;
    int zamien = 0;
    static char buf[ 8192 ];
    static char unibuf[ 8192 ];
    size_t wielkosc_b = 8192;
    size_t wielkosc_u = 8192;
    char *wskaznik_b = buf;
    char *wskaznik_u = unibuf;
    char *wyj = buf;

    while ( *wej )
    {
	znak = *wej++;
	if ( znak == '`' && !zamien )
	    zamien = 1;
	else
	{
	    if ( zamien && znak != '`' )
		*wyj++ = polska( &znak );
	    if ( znak )
		*wyj++ = znak;
	    zamien = 0;
	}
    }

    if ( zamien )
	*wyj++ = '`';
    *wyj = '\0';
    /* Lam 19.1.2006 dla mapy: w tym momencie mamy tekst w ISO, kazdy znak to
       jeden bajt i moge sobie szybciutko przyciac do dowolnej wielkosci */
    if ( len >= 0 )
	buf[ len ] = '\0';
    iconv( iso_uni, &wskaznik_b, &wielkosc_b, &wskaznik_u, &wielkosc_u );

    return unibuf;
}


char *pols_z_lac( const char *wej )
{
    return pols_z_lac_len( wej, -1 );
}


char *lac_z_pols( const char *wej )
{
    char znak;
    static char buf[ 8192 ];
    static char unibuf[ 8192 ];
    size_t wielkosc_b = 8192;
    size_t wielkosc_u = 8192;
    char *wskaznik_b = buf;
    char *wskaznik_u = unibuf;
    char *wyj = buf;

    strcpy( buf, wej );
    iconv( uni_iso, &wskaznik_b, &wielkosc_b, &wskaznik_u, &wielkosc_u );
    wej = unibuf;
    while ( *wej )
    {
	znak = *wej++;
	*wyj++ = angielska( &znak );
	if ( znak )
	    *wyj++ = znak;
    }

    *wyj = '\0';

    return buf;
}


/* na razie tylko dla str_infix_uni */
bool str_prefix_wc( const wchar_t *astr, const wchar_t *bstr )
{
    for ( ; *astr; astr++, bstr++ )
	if ( towlower( *astr ) != towlower( *bstr ) )
	    return TRUE;

    return FALSE;
}


/* wcsstr( ) rozroznia wielkosc liter */
bool str_infix_wc( const wchar_t *astr, const wchar_t *bstr )
{
    wint_t c0;
    int  sstr2 = wcslen( bstr );
    int  ichar;

    if ( ( c0 = towlower( astr[ 0 ] ) ) == '\0' )
	return FALSE;

    for ( ichar = 0; ichar <= sstr2; ichar++ )
    {
	if ( c0 == towlower( bstr[ ichar ] ) && !str_prefix_wc( astr, bstr + ichar ) )
	    return FALSE;
    }

    return TRUE;
}


/* potrzebuje setlocale( ) do dzialania, inaczej mbstowcs( ) sraja -1 */
bool str_infix_uni( const char *astr, const char *bstr )
{
    wchar_t wca[ 512 ];
    wchar_t wcb[ 512 ];

    if ( mbstowcs( wca, astr, 512 ) == (size_t) -1 )
	return TRUE;

    if ( mbstowcs( wcb, bstr, 512 ) == (size_t) -1 )
	return TRUE;

    return str_infix_wc( wca, wcb );
}
