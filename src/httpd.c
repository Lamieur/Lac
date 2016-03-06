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
 *  Ulryk 2004: nowa, ulepszona wersja zintegrowanego serwera www          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"
#include "interp.h"
#include "const.h"
#include "comm.h"


char	*http2lac	args( ( char *arg ) );
void	get_help_topic	args( ( char *arg, char *topic ) );
int	zrob_odnosnik	args( ( char *arg, char *link ) );
bool	html_header	args( ( WHO_DESCRIPTOR_DATA *d, const char *tytul ) );
bool	html_foot	args( ( WHO_DESCRIPTOR_DATA *d ) );
bool	html_time	args( ( WHO_DESCRIPTOR_DATA *d ) );
bool	html_weather	args( ( WHO_DESCRIPTOR_DATA *d ) );
HTML( html_clans );


const   struct    html_page_type       html_page_table      [ ] =
{
    {
	"/kto",		"kto gra",	"Zobacz list\352 aktualnie graj\261cych",	html_who
    },

    {
	"/pomoc",	"pomoc",	"Poczytaj pomoc",	html_help
    },

    {
	"/polecenia",	"polecenia",	"Zobacz list\352 polece\361",	html_commands
    },

    {
	"/profesje",	"profesje",	"Zobacz list\352 profesji",	html_slist
    },

    {
	"/krainy",	"krainy",	"Zobacz list\352 krain",	html_areas
    },
#if defined( KLANY )
    {
	"/klany",	"klany",	"Zobacz list\352 klan\363w",	html_clans
    },
#endif
/*
    {
	"/nowosci",	"nowo¶ci",	"Zapoznaj siê z nowo¶ciami", html_news
    },
*/
/* ponizej umieszczac jedynie adresy z NULLowymi opisami */
    {
	"/robots.txt",	NULL,		NULL,			html_404
    },

    {
	"/favicon.ico",	NULL,		NULL,			html_404
    },

    {
	NULL,		NULL,		NULL,			NULL
    }
};

char *  const   html_day_name        [] =
{
    "ksi\352\277yca", "byka", "k\263amstwa", "gromu", "wolno\266ci",
    "wielkich bog\363w", "s\263o\361ca"
};

char *  const   html_month_name      [] =
{
    "zimy", "wilka", "lodowego olbrzyma", "dawnej pot\352gi",
    "wielkiej walki", "wiosny", "przyrody", "b\263aho\266ci", "smoka",
    "s\263o\361ca", "ciep\263a", "bitwy", "ciemnych odcieni", "cieni",
    "d\263ugich cieni", "dawnej ciemno\266ci", "wielkiego z\263a"
};


HTML_FUN *szukaj_strony( char *arg )
{
    int i;

    for ( i = 0; html_page_table[ i ].name; i++ )
	if ( !str_prefix( html_page_table[ i ].name, arg ) )
	    return html_page_table[ i ].fun;

    return html_index;
}


char *http2lac( char *arg )
{
    char *p = arg;
    char  buf[ MSL ];
    int   i = 0;


    if ( *p == '\0' )
	return arg;

    while ( *p != '\0' )
    {
	if ( *p == '+' )
	    buf[ i ] = ' ';
	else if ( !str_prefix( "%A1", p ) || ( !str_prefix( "%B1", p ) ) )
	{
	    buf[ i ] = '`';
	    i++;
	    buf[ i ] = 'a';
	    p += 2;
	}
	else if ( !str_prefix( "%C6", p ) || ( !str_prefix( "%E6", p ) ) )
	{
	    buf[ i ] = '`';
	    i++;
	    buf[ i ] = 'c';
	    p += 2;
	}
	else if ( !str_prefix( "%CA", p ) || ( !str_prefix( "%EA", p ) ) )
	{
	    buf[ i ] = '`';
	    i++;
	    buf[ i ] = 'e';
	    p += 2;
	}
	else if ( !str_prefix( "%A3", p ) || ( !str_prefix( "%B3", p ) ) )
	{
	    buf[ i ] = '`';
	    i++;
	    buf[ i ] = 'l';
	    p += 2;
	}
	else if ( !str_prefix( "%D1", p ) || ( !str_prefix( "%F1", p ) ) )
	{
	    buf[ i ] = '`';
	    i++;
	    buf[ i ] = 'n';
	    p += 2;
	}
	else if ( !str_prefix( "%D3", p ) || ( !str_prefix( "%F3", p ) ) )
	{
	    buf[ i ] = '`';
	    i++;
	    buf[ i ] = 'o';
	    p += 2;
	}
	else if ( !str_prefix( "%A6", p ) || ( !str_prefix( "%B6", p ) ) )
	{
	    buf[ i ] = '`';
	    i++;
	    buf[ i ] = 's';
	    p += 2;
	}
	else if ( !str_prefix( "%AC", p ) || ( !str_prefix( "%BC", p ) ) )
	{
	    buf[ i ] = '`';
	    i++;
	    buf[ i ] = 'x';
	    p += 2;
	}
	else if ( !str_prefix( "%AF", p ) || ( !str_prefix( "%BF", p ) ) )
	{
	    buf[ i ] = '`';
	    i++;
	    buf[ i ] = 'z';
	    p += 2;
	}
	else if ( !str_prefix( "%20", p ) )
	{
	    buf[ i ] = ' ';
	    p += 2;
	}
	else if ( !str_prefix( "%21", p ) )
	{
	    buf[ i ] = '!';
	    p += 2;
	}
	else if ( !str_prefix( "%22", p ) )
	{
	    buf[ i ] = '\"';
	    p += 2;
	}
	else if ( !str_prefix( "%26", p ) )
	{
	    buf[ i ] = '&';
	    p += 2;
	}
	else if ( !str_prefix( "%27", p ) )
	{
	    buf[ i ] = '\'';
	    p += 2;
	}
	else if ( !str_prefix( "%2C", p ) )
	{
	    buf[ i ] = ',';
	    p += 2;
	}
	else if ( !str_prefix( "%2F", p ) )
	{
	    buf[ i ] = '/';
	    p += 2;
	}
	else if ( !str_prefix( "%3B", p ) )
	{
	    buf[ i ] = ';';
	    p += 2;
	}
	else if ( !str_prefix( "%3C", p ) )
	{
	    buf[ i ] = '<';
	    p += 2;
	}
	else if ( !str_prefix( "%3D", p ) )
	{
	    buf[ i ] = '=';
	    p += 2;
	}
	else if ( !str_prefix( "%3F", p ) )
	{
	    buf[ i ] = '?';
	    p += 2;
	}
	else if ( !str_prefix( "%60", p ) )
	{
	    buf[ i ] = '`';
	    p += 2;
	}
	else if ( !str_prefix( "%2E", p ) )
	{
	    buf[ i ] = '.';
	    p += 2;
	}
	else
	    buf[ i ] = *p;

	i++;
	p++;
    }
    buf[ i ] = '\0';
    strcpy( arg, buf );

    return arg;
}


void http_log( WHO_DESCRIPTOR_DATA *d )
{
    char *strtime;

    strtime = ctime( &current_time );
    strtime[ strlen( strtime ) - 1 ] = '\0';
    fprintf( httpdlog, "%s :: %s :: %s :: UA: %s :: Ref: %s\n",
	     strtime,
	     d->host,
	     http2lac( d->zapytanie ),
	     d->klient,
	     d->referer );

    return;
}


void get_help_topic( char *arg, char *topic )
{
    char *p = arg;

    while ( ( *p++ != '\0' ) && ( *p != '=' ) && ( *p != '/' ) )
	;

    if ( *p == '\0' )
	return;

    strcpy( topic, ++p );
    topic = http2lac( topic );

    return;
}


bool get_who_data( char *arg, WHO_DESCRIPTOR_DATA *d )
{
    char *p;
    int   i = 0;

    if ( !( p = strstr( arg, "HTTP/" ) ) || str_prefix( "GET", arg ) )
    {
       strcpy( d->zapytanie, "(blad)" );
       strcpy( d->klient, "(brak danych)" );
       strcpy( d->referer, "(brak)" );
       return FALSE;
    }

    strncpy( d->odnosnik, arg + 4, URANGE( 0, p - arg - 5, (int) ( sizeof d->odnosnik ) - 1 ) );
    d->odnosnik[ ( sizeof d->odnosnik ) - 1 ] = '\0';

    while ( !isspace( (int) *p ) )
	p++;

    strncpy( d->zapytanie, arg, URANGE( 0, p - arg, (int) ( sizeof d->zapytanie ) - 1 ) );
    d->zapytanie[ ( sizeof d->zapytanie ) - 1 ] = '\0';

    if ( !( p = strstr( arg, "User-Agent:" ) ) )
	strcpy( d->klient, "(brak danych)" );
    else
    {
	p += 12;

	while ( *p != '\n' && *p != '\0' && ( i < MAX_STRING_LENGTH ) )
	    d->klient[ i++ ] = *p++;

	d->klient[ i ? i - 1 : i ] = '\0';
    }

    if ( !( p = strstr( arg, "Referer:" ) ) )
    {
       strcpy( d->referer, "(brak)" );
       return TRUE;
    }

    p += 9;
    i = 0;

    while ( *p != '\n' && *p != '\0' && ( i < MAX_STRING_LENGTH ) )
	d->referer[ i++ ] = *p++;

    d->referer[ i ? i - 1 : i ] = '\0';

    return TRUE;
}


int zrob_odnosnik( char *arg, char *link )
{
    char buf[ MAX_STRING_LENGTH ];
    char buf1[ MAX_STRING_LENGTH ];
    char buf3[ MAX_STRING_LENGTH ];
    int i, j, z;

    for ( i = 0; arg[ i ] != '\0' && arg[ i ] != '{'; i++ )
	buf3[ i ] = arg[ i ];
    buf3[ i ] = '\0';
    z = i;

#if defined( LINK_CHECK )
{
    HELP_DATA *help;
    char buf2[ MSL ];
    char *pomwsk;
    bool zmienilem = FALSE;

    pomwsk = buf3;
    while ( *pomwsk == ' ' )
	pomwsk++;
    /* to jest "na chwile", bedzie dzialac dopoki nie mamy pomocy z "  " */
    while ( *pomwsk )
	if ( *pomwsk == ' ' && *( pomwsk + 1 ) == ' ' )
	{
	    *pomwsk = '\0';
	    zmienilem = TRUE;
	}
	else
	    pomwsk++;

    pomwsk = buf3;
    while ( *pomwsk == ' ' )
	pomwsk++;

    for ( help = help_first; help; help = help->next )
	if ( is_name2( pomwsk, help->keyword ) )
	    break;

    if ( !help )
    {
	sprintf( buf2, "Bledny odnosnik '%s'", pomwsk );
	log_string( buf2 );
    }

    if ( zmienilem )
    {
	pomwsk = buf3;
	while ( *pomwsk )
	    pomwsk++;
	*pomwsk = ' ';
    }
}
#endif /* defined( LINK_CHECK ) */

    colourconv( buf1, buf3, NULL, sizeof( buf1 ) );
    j = 0;
    for ( i = 0; buf1[ i ] != '\0'; i++ )
    {
	/* Lam: do_slist podkresla cala linie, ale odnosnik ma prowadzic
		do prawdziwej pomocy, bez spacji */
	if ( !j && buf1[ i ] == ' ' )
	    continue;

	if ( buf1[ i ] == ' '
	  && ( buf1[ i + 1 ] == ' '
	    || buf1[ i + 1 ] == '\0' ) )
	{
	    continue;
	}

	if ( buf1[ i ] < 65
	  || buf1[ i ] > 122
	  || ( buf1[ i ] > 90 && buf1[ i ] < 97 ) )
	{
	    sprintf( &buf[ j ], "%%%02X", (unsigned char) buf1[ i ] );
	    j += 3;
	}
	else
	{
	    buf[ j ] = buf1[ i ];
	    j++;
	}
    }
    buf[ j ] = '\0';

    sprintf( link, "<a href=\"/pomoc?p=%s\">%s</a>",
		buf, lac2html( buf3, FALSE ) );

    return z;
}


/* UWAGA: FIXME: TODO:
   - nie sprawdza wielkosci buf (i jej nie zna)
   - wywolujacy moze nie miec miejsca na zmiany && w &amp;&amp;
   - GCC pozwala podac tej funkcji staly lancuch z segmentu kodu - blad
   Uwazajcie na te funkcje i naprawcie to kiedys! */
char *lac2html( char *buf, bool mysql )
{
    char buf2[ MAX_STRING_LENGTH * 25 ];
    char buf3[ MAX_STRING_LENGTH ];
    char c[ 2 ];
    int  i = 0;

    buf2[ 0 ] = '\0';
    c[ 1 ] = '\0';

    /* konwersja <, >, &, \n\r */

    for ( i = 0; buf[ i ] != '\0'; i++ )
    {
	if ( buf[ i ] == '<' )
	    strcat( buf2, "&lt;" );
	else if ( buf[ i ] == '>' )
	    strcat( buf2, "&gt;" );
	else if ( buf[ i ] == '&' )
	    strcat( buf2, "&amp;" );
	else if ( buf[ i ] == '\n'
	       || buf[ i ] == '\r' )
	{
	    /* Lam: MSIE nie umie obsluzyc <br><br> jako linii odstepu */
	    if ( strlen( buf2 ) > 4
	      && !str_cmp( &buf2[ strlen( buf2 ) - 4 ], "<br>" ) )
		strcat( buf2, " <br>" );
	    else
		strcat( buf2, "<br>" );
	    if ( buf[ i ] == '\n' && buf[ i + 1 ] == '\r' )
		i++;
	}
	else if ( buf[ i ] == '{' && buf[ i + 1 ] == '?' )
	{
	    i += 1;
	    i += zrob_odnosnik( buf + i + 1, buf3 );
	    strcat( buf2, buf3 );
	}
	else if ( buf[ i ] == '\'' && mysql )
	    strcat( buf2, "\\'" );
	else if ( buf[ i ] == '\\' && mysql )
	    strcat( buf2, "\\\\" );
	else
	{
	    c[ 0 ] = buf[ i ];
	    strcat( buf2, c );
	}
    }
    strcpy( buf, buf2 );

    return buf;
}


bool html_header( WHO_DESCRIPTOR_DATA *d, const char *tytul )
{
    char  buf[ MAX_STRING_LENGTH ];
    int   day;

    day     = time_info.day + 1;

    sprintf( buf, "HTTP/1.1 200 OK\n"
		"Date: godzina %d, dzie\361 %s, %d dzie\361 miesi\261ca %s\n"
		"Server: " LAC_VERSION "\n"
		"Connection: close\n"
		"Content-Type: text/html; charset=iso-8859-2\n\n",
	    time_info.hour,
	    html_day_name[ day % 7 ],
	    day,
	    html_month_name[ time_info.month ] );

    /* Lam: duze uproszczenie, ale pod Linuksem zawsze mi sie udaje wypchnac wszystko */
    STH( d, buf );

    sprintf( buf,
	    "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\
\"http://www.w3.org/TR/html4/loose.dtd\">\n\
<html>\n<head><title>%s</title>\n\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-2\">\n\
<meta name=\"Language\" content=\"pl\">\n\
<meta name=\"Keywords\" content=\"Lac,MUD,wojownik,mag,kleryk,paladyn,gra\">\n\
<STYLE TYPE=\"text/css\">\n\
<!--\n\
body      { margin-left: 1cm; margin-right: 1cm }\n\
a         { color: #0de50d; text-decoration: none }\n\
a:visited { color: #0de50d; text-decoration: none }\n\
a:hover   { color: #e01818; text-decoration: none }\n\
--></STYLE></head>\n\
<body bgcolor=\"#000000\" text=\"#aaaaaa\">\n\
<center><h1><b>Lac</b></h1><h2>prawdziwie polski mud</h2>\n\
<hr size=4 width=\"100%%\">\n\
<h2>Witaj na stronie informacyjnej generowanej przez Laca.</h2>\n\
<h2><a href=\"telnet://%s:4000\">Po\263\261cz si\352!</a> albo <a href=\"http://%s/\">zajrzyj na stron\352.</a></h2></center>\n",
	    tytul, MUD_HOST, WWW_HOST );

    STH( d, buf );

    return TRUE;
}


bool html_foot( WHO_DESCRIPTOR_DATA *d )
{
    char buf[ MAX_STRING_LENGTH ];
    char buf2[ MAX_INPUT_LENGTH ];
    int  i;

    sprintf( buf, "<center><hr size=4 width=\"100%%\">\n\
		<a href=\"/\">spis tre\266ci</a> |\n" );

    for ( i = 0; html_page_table[ i ].name; i++ )
    {
	if ( html_page_table[ i ].short_link )
	{
	    sprintf( buf2, "<a href=\"%s\">%s</a>%s",
		html_page_table[ i ].name,
		html_page_table[ i ].short_link,
		( html_page_table[ i + 1 ].short_link ) ? " |\n" : "\n" );
	    strcat( buf, buf2 );
	}
    }
    strcat( buf, "</center></body></html>" );

    STH( d, buf );

    return TRUE;
}


bool html_time( WHO_DESCRIPTOR_DATA *d )
{
    char buf[ MAX_STRING_LENGTH ];
    int  day;

    day = time_info.day + 1;

    sprintf( buf,
	    "<p>Jest godzina %d, dzie\361 %s, %d dzie\361 miesi\261ca %s.</p>\n",
	    time_info.hour,
	    html_day_name[ day % 7 ],
	    day,
	    html_month_name[ time_info.month ] );

    STH( d, buf );

    return TRUE;
}


bool html_weather( WHO_DESCRIPTOR_DATA *d )
{
	   char         buf     [ MAX_STRING_LENGTH ];
    static char * const sky_look[ 4 ] =
    {
	"bezchmurne",
	"pochmurne",
	"deszczowe",
	"przecinane b\263yskawicami"
    };

    sprintf( buf,
	    "<p>Niebo jest %s i %s.</p>\n",
	    sky_look[ weather_info.sky ],
	    weather_info.change >= 0
	    ? "wieje ciep\263y wiaterek z po\263udnia"
	    : "wieje zimny wiatr z p\363\263nocy" );

    STH( d, buf );

    return TRUE;
}


HTML( html_index )
{
    char buf[ MAX_STRING_LENGTH ];
    char buf2[ MAX_INPUT_LENGTH ];
    int  i;

    if ( !html_header( d, LAC_VERSION ) )
	return;

    sprintf( buf, "<center>\n" );

    for ( i = 0; html_page_table[ i ].name; i++ )
	if ( html_page_table[ i ].long_link )
	{
	    sprintf( buf2, "<a href=\"%s\">%s</a><br>\n",
		 html_page_table[ i ].name,
		 html_page_table[ i ].long_link );
	    strcat( buf, buf2 );
	}

    strcat( buf, "<a href=\"http://forum.lac.pl/\">Odwied\274 forum</a><br>\n"
	    "<a href=\"http://opisomat.lac.pl/\">Odwied\274 opisomat</a><br>\n"
	    "<a href=\"http://bledomat.lac.pl/\">Odwied\274 b\263\352domat</a><br>\n"
	    "</center>\n" );

    STH( d, buf );

    html_foot( d );

    return;
}


HTML( html_who )
{
    char buf[ MAX_STRING_LENGTH ];

    if ( !html_header( d, "Gracze Laca" )
      || !html_time( d )
      || !html_weather( d ) )
    {
	return;
    }

    sprintf( buf, "<center><h3><b>Gracze Laca</b></h3></center>\n"
		"<pre><span style=\"color: #aaaaaa;\">" );

    STH( d, buf );

    real_who( NULL, "", d );
    html_foot( d );

    return;
}


HTML( html_help )
{
    char buf   [ MAX_STRING_LENGTH ];
    char temat [ MAX_INPUT_LENGTH  ];
    char haslo [ MAX_INPUT_LENGTH  ];
    char skroty[ MAX_STRING_LENGTH ];

    temat[ 0 ] = haslo[ 0 ] = skroty[ 0 ] = '\0';

    get_help_topic( d->odnosnik, temat );
    if ( temat[ 0 ] == '\0' )
	sprintf( temat, "podsumowaniexxx" );

    sprawdz_hasla( temat, haslo, skroty, 1 );

    /* wolalbym liste slow kluczowych dla pomocy z poziomem 0, ktore i tak
       to pokazuja */
    colourconv( buf, ( *haslo && str_cmp( haslo, "podsumowaniexxx" ) )
			? lac2html( haslo, FALSE ) : "Pomoc",
		NULL, sizeof( buf ) );
    if ( !html_header( d, buf ) )
	return;

    strcpy( buf,
	    "<form action=\"/pomoc\" method=\"get\">\n\
	     <input name=\"p\">\n\
	     <input type=\"submit\" value=\"Szukaj\">\n\
	     </form>\n\
	     <pre><span>" );

    STH( d, buf );

    if ( haslo[ 0 ] != '\0' )
	real_help( 0, haslo, skroty, d );
    else
    {
	char  skroty_html[ MAX_STRING_LENGTH * 5 ];

	colourconv( skroty_html, lac2html( skroty, FALSE ), NULL,
		sizeof( skroty_html ) );
	STH( d, skroty_html );
    }

    sprintf( buf, "</span></pre>" );
    STH( d, buf );

    html_foot( d );

    return;
}


HTML( html_areas )
{
    ZONE_DATA *strefa;
    char       buf[ MAX_STRING_LENGTH ];

    if ( !html_header( d, "Krainy Laca" ) )
	return;

    sprintf( buf, "<pre><span>" );
    STH( d, buf );

    for ( strefa = zone_first; strefa; strefa = strefa->next )
    {
	char wyjbuf [ MSL * 3 ];

	sprintf( buf, "<br><b>Krainy</b> {B%s{x (w`ladca: %s%s%s)<br>",
		 strefa->dopelniacz,
		 str_cmp( "dupa", strefa->wladca ) ?
		 strefa->wladca : "ukryty",
		 strefa->namiestnik && *strefa->namiestnik ?
		 "; namiestnik: " : "",
		 strefa->namiestnik ? strefa->namiestnik : "" );

	colourconv( wyjbuf, buf, NULL, sizeof( wyjbuf ) );
	STH( d, wyjbuf );

	real_areas( NULL, strefa->nazwa, d );
    }

    sprintf( buf, "</span></pre>" );
    STH( d, buf );

    html_foot( d );

    return;
}


HTML( html_maps )
{
    return;
}


HTML( html_news )
{
    char buf [ MAX_STRING_LENGTH ];
    char buf2[ MAX_STRING_LENGTH * 3 ];

    if ( !html_header( d, "Nowo\266ci Laca" ) )
	return;

    strcpy( buf, "<pre><span>" );
    STH( d, buf );

    strcpy( buf, lac_nowosci );
    colourconv( buf2, lac2html( buf, FALSE ), NULL, sizeof( buf2 ) );
    STH( d, buf2 );

    strcpy( buf, "</span></pre>" );
    STH( d, buf );

    html_foot( d );

    return;
}


/* Lam 1.5.2004 */
HTML( html_slist )
{
    char buf[ MSL ], buf1[ MSL ], buf2[ MSL ], profesja[ MIL ];
    int p;

    profesja[ 0 ] = '\0';

    get_help_topic( d->odnosnik, profesja );

    if ( !*profesja )
    {
	if ( !html_header( d, "Lista profesji Laca" ) )
	    return;

	strcpy( buf, "<span>" );
	STH( d, buf );

	strcpy( buf, "Lista profesji:<br>\n" );

	for ( p = 0; p < MAX_CLASS; p++ )
	    if ( *class_table[ p ].races )
	    {
		sprintf( buf2, "- {?%s{x (%s)",
			class_table[ p ].long_name,
			class_table[ p ].pl_name );
		colourconv( buf1, lac2html( buf2, FALSE ), NULL, sizeof( buf1 ) );
		strcat( buf, buf1 );

		sprintf( buf1, " (<a href=\"/profesje/%s\">lista umiej\352tno\266ci</a>)<br>\n",
			class_table[ p ].who_name );
		strcat( buf, buf1 );
	    }

	STH( d, buf );

	strcpy( buf, "</span>" );
	STH( d, buf );
    }
    else
    {
	for ( p = 0; p < MAX_CLASS; p++ )
	    if ( !str_cmp( profesja, class_table[ p ].who_name ) )
		break;

	if ( p == MAX_CLASS )
	    strcpy( buf, "Umiej\352tno\266ci profesji" );
	else
	    sprintf( buf, "%s: umiej\352tno\266ci", class_table[ p ].long_name );

	if ( !html_header( d, buf ) )
	    return;

	strcpy( buf, "<pre><span>" );
	STH( d, buf );

	real_slist( NULL, profesja, d );

	strcpy( buf, "</span></pre>" );
	STH( d, buf );
    }

    html_foot( d );

    return;
}


/*
 * Lam 18.8.2005
 */
HTML( html_commands )
{
    char buf[ MSL ], buf1[ MSL * 3 ], buf2[ MSL * 6 ];
    int cmd, col;

    if ( !html_header( d, "Polecenia Laca" ) )
	return;

    strcpy( buf, "<pre><span>" );
    STH( d, buf );

    strcpy( buf, "{WWszystkie polecenia dost\352pne w Lacu:{x\n\n" );
    colourconv( buf2, lac2html( buf, FALSE ), NULL, sizeof( buf2 ) );
    STH( d, buf2 );

    buf1[ 0 ] = '\0';
    col = 0;
    for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	if ( cmd_table[ cmd ].level <  LEVEL_HERO
	  && !IS_SET( cmd_table[ cmd ].restr, CMD_NOTLISTED ) )
	{
	    sprintf( buf, "{?%s{x", wyrownaj( cmd_table[ cmd ].name, -16 ) );
	    strcat( buf1, buf );
	    if ( ++col % 5 == 0 )
		strcat( buf1, "\n\r" );
	}

    if ( col % 5 != 0 )
	strcat( buf1, "\n\r" );

    colourconv( buf2, lac2html( buf1, FALSE ), NULL, sizeof( buf2 ) );
    STH( d, buf2 );

    strcpy( buf, "</span></pre>" );
    STH( d, buf );

    html_foot( d );

    return;
}


HTML( html_clans )
{
    char buf[ MSL ];
    char klan[ MIL ];

    klan[ 0 ] = '\0';

    get_help_topic( d->odnosnik, klan );

    if ( !html_header( d, *klan ? "Informacja o klanie" : "Klany Laca" ) )
	return;

    strcpy( buf, "<pre><span>" );
    STH( d, buf );

    if ( !*klan )
	real_clan_lista( NULL, "", d );
    else
    {
	char       buf1[ MAX_STRING_LENGTH ];
	CLAN_DATA *cl;

	if ( !( cl = clan_lookup2( klan ) )
	  || !can_see_clan( NULL, cl ) )
	{
	    STH( d, "Nie znam takiego klanu.<br></span></pre>" );
	    html_foot( d );
	    return;
	}

	if ( !( cl->statut ) || !( *cl->statut ) )
	    sprintf( buf, "Klan {W%s{x nie ma statutu.\n\r\n\r", cl->dop );
	else
	{
	    sprintf( buf, "Statut klanu {W%s{x:\n\r", cl->dop );
	    strcat( buf, cl->statut );
	    strcat( buf, "{x\n\r" );
	}

	colourconv( buf1, lac2html( buf, FALSE ), NULL, sizeof( buf1 ) );
	STH( d, buf1 );

	real_clan_info( NULL, cl, d );
	STH( d, "<a href=\"/klany\">&lt;&lt; wr\363\346</a><br>" );
    }

    strcpy( buf, "</span></pre>" );
    STH( d, buf );

    html_foot( d );

    return;
}


HTML( html_404 )
{
   char buf[ MAX_STRING_LENGTH ];
   int  day;

    day     = time_info.day + 1;

    sprintf( buf, "HTTP/1.1 404 Not Found\n"
		"Date: godzina %d, dzie\361 %s, %d dzie\361 miesi\261ca %s\n"
		"Server: " LAC_VERSION "\n"
		"Connection: close\n"
		"Content-Type: text/html; charset=iso-8859-2\n\n",
	    time_info.hour,
	    html_day_name[ day % 7 ],
	    day,
	    html_month_name[ time_info.month ] );

    STH( d, buf );

    sprintf( buf,
	    "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\
\"http://www.w3.org/TR/html4/loose.dtd\">\n\
<html>\n<head><title>B\263\261d 404: %s</title>\n\
<STYLE TYPE=\"text/css\">\n\
<!--\n\
body      { margin-left: 1cm; margin-right: 1cm }\n\
a         { color: #0de50d; text-decoration: none }\n\
a:visited { color: #0de50d; text-decoration: none }\n\
a:hover   { color: #e01818; text-decoration: none }\n\
p         { margin: 0px 2cm; }\n\
--></STYLE></head>\n\
<body bgcolor=\"#000000\" text=\"#aaaaaa\">\n\
<center><h1><b>Lac</b></h1><hr size=4 width=\"100%%\"></center>\n\
<h2>Podana przez ciebie strona nie istnieje.</h2>\n\
<p>Mo\277liwe, \277e poda\263e\266/a\266 b\263\352dny adres strony, strona zosta\263a usuni\352ta lub\n\
zmieni\263a lokalizacj\352. Wr\363\346 na <a href=\"/\">stron\352</a> g\263\363wn\261.</p>\n\
<br>\n<h3>B\263\261d 404</h3>\n\
<p>%s</p>\n",
	    LAC_VERSION, LAC_VERSION );

    STH( d, buf );

    html_foot( d );

    return;
}
