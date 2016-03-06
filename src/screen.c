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
 *  Lam: Tu zbieram funkcje odpowiedzialne za zarzadzanie ekranem gracza   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"
#include "db.h"


char		*kolor_literki		args( ( char literka ) );
char		literka_koloru		args( ( char *kolor ) );
SCHEMAT_DATA	*schemat_lookup		args( ( const char *arg ) );

#if defined( MSDOS )
# include <conio.h>
#endif

/*
 * Lam 5.4.98
 */
KOMENDA( do_clear )
{
#if defined( MSDOS )
    clrscr( );
#else
    char buf[ MSL ];
    int wys_term, wys_rozm, szer_term, i;

    if ( !ch->desc )
	return;

    wys_term = ( ch->desc->wys == 0 ) ? 25 : ch->desc->wys;
    wys_rozm = ( ch->desc->wys == 0 ) ?  9 : ch->desc->wys * 35 / 100;

    /* naprawde czyscimy ekran */
    sprintf( buf, ESCS "[0;%dr" ESCS "[H" ESCS "[2J", wys_term );
    write_to_buffer( ch->desc, buf, 0 );
    /* jesli ma ONW, przechodzimy do dolnego okna */
    if ( CZY_OPCJA( ch, OPC_MIW ) )
    {
	/* Lam 30.6.2003: ilosc kresek w linii oddzielajacej rowniez zalezna
	   od rozmiaru ekranu
	   Lam 21.1.2004: przy 0 linii szerokosci pojawialo sie "><", zakladam,
	   ze nikt nie ma tak naprawde mniej niz 60 */
	szer_term = ch->desc->szer ? ch->desc->szer : 60;
	sprintf( buf, ESCS "[%d;1H>", wys_rozm + 1 );
	for ( i = 0; i < szer_term - 3; i++ )
	    strcat( buf, "-" );
	strcat( buf, "<" );
	write_to_buffer( ch->desc, buf, 0 );
	sprintf( buf, ESCS "[%d;%dr" ESCS "[%d;1H", wys_rozm + 2, wys_term, wys_term );
	write_to_buffer( ch->desc, buf, 0 );
    }

    return;
#endif
}


/*
 * Lam 5.4.98: tutaj ustawiamy graczowi gorne okno, w ktorym beda mu sie
 * pokazywac kanaly/telepatia, zapamietujac pozycje w dolnym.
 */
void upper_window( CHAR_DATA *ch )
{
    char buf[ MSL ];
    int wys_rozm;

    if ( !ch->desc /* zdarza sie przy "powiedz" */
      || !CZY_OPCJA( ch, OPC_MIW ) )
	return;

    wys_rozm = ( ch->desc->wys == 0 ) ?  9 : ch->desc->wys * 35 / 100;

    sprintf( buf, ESCS "7" ESCS "[0;%dr" ESCS "[%d;1H", wys_rozm, wys_rozm );
    write_to_buffer( ch->desc, buf, 0 );

    return;
}


/*
 * Lam 5.4.98: wracamy do okna, do pozycji z dolnego okna
 */
void lower_window( CHAR_DATA *ch )
{
    char buf[ MSL ];
    int wys_term, wys_rozm;

    if ( !ch->desc
      || !CZY_OPCJA( ch, OPC_MIW ) )
	return;

    wys_term = ( ch->desc->wys == 0 ) ? 25 : ch->desc->wys;
    wys_rozm = ( ch->desc->wys == 0 ) ?  9 : ch->desc->wys * 35 / 100;

    sprintf( buf, ESCS "[%d;%dr" ESCS "8", wys_rozm + 2, wys_term );
    write_to_buffer( ch->desc, buf, 0 );

    return;
}


/*
 * Lam 5.4.98: okno na wiadomosci
 */
KOMENDA( do_miw )
{
    char buf[ MSL ];
    int wys_term, wys_rozm;

    if ( IS_NPC( ch ) || !ch->desc )
	return;

    if ( CZY_OPCJA( ch, OPC_MIW ) )
    {
	REMOVE_BIT( ch->pcdata->opcje, OPC_MIW );
	do_clear( ch, "" );
	send_to_char( "Powracasz do standardowego trybu wy`swietlania.\n\r", ch );
	return;
    }

    wys_term = ( ch->desc->wys == 0 ) ? 25 : ch->desc->wys;
    wys_rozm = ( ch->desc->wys == 0 ) ?  9 : ch->desc->wys * 35 / 100;

    SET_BIT( ch->pcdata->opcje, OPC_MIW );
    do_clear( ch, "" );
    sprintf( buf, ESCS "[%d;%dr" ESCS "[%d;1H", wys_rozm + 2, wys_term, wys_term );
    write_to_buffer( ch->desc, buf, 0 );
    send_to_char( "Teraz wiadomo`sci widzisz w oddzielnym oknie.\n\r", ch );
    send_to_char( "{RJe`sli sobie tego nie `zyczysz, napisz \"ONW\" ponownie.{x\n\r", ch );

    return;
}


/*
 * Lam 2.4.2000: obsluga konfigurowalnych kolorow. Koniecznie male literki!
 */
static struct
{
    char *kolor;
    char literka;
} nazwy_kolorow[ ] =
{
    { "zielony",		'g'	},
    { "czerwony",		'r'	},
    { "niebieski",		'b'	},
    { "r`o`zowy",		'm'	},
    { "morski",		'c'	},
    { "`z`o`lty",		'y'	},
    { "bia`ly",		'w'	},
    { "standardowy",	'x'	},
    { NULL,			'-'	}
};

char *kolor_literki( char literka )
{
    int i;

    for ( i = 0; nazwy_kolorow[ i ].kolor; i++ )
	if ( nazwy_kolorow[ i ].literka == literka
	  || UPPER( nazwy_kolorow[ i ].literka ) == literka )
	{
	    return nazwy_kolorow[ i ].kolor;
	}

    return "(b`l`ad)";
}


char literka_koloru( char *kolor )
{
    int i;
    bool jasnosc = FALSE;
    char arg[ MIL ];

    if ( strlen( kolor ) == 1 )
    {
	for ( i = 0; nazwy_kolorow[ i ].kolor; i++ )
	    if ( nazwy_kolorow[ i ].literka == *kolor
	      || UPPER( nazwy_kolorow[ i ].literka ) == *kolor )
	    {
		return *kolor;
	    }

	return '-';
    }

    one_argument( kolor, arg );

    if ( !str_cmp( arg, "domy`slny" ) )
	return '+';

    if ( !str_cmp( arg, "jasny" ) )
    {
	kolor = one_argument( kolor, arg );
	jasnosc = TRUE;
    }

    for ( i = 0; nazwy_kolorow[ i ].kolor; i++ )
	if ( !str_cmp( nazwy_kolorow[ i ].kolor, kolor ) )
	{
	    if ( jasnosc && nazwy_kolorow[ i ].literka != 'x' )
		return UPPER( nazwy_kolorow[ i ].literka );
	    else
		return nazwy_kolorow[ i ].literka;
	}

    return '-';
}


/* pozycja nie jest pozycja w tablicy, dzieki temu mozna zmieniac kolejnosc
   na liscie, zachowujac zgodnosc plikow postaci */
static struct
{
    char *nazwa;
    int pozycja;
    char kod;
    char domyslny;
} kolory_graczowe[ ] =
{
    { "pomoc",		18,	'?',	'G'	},
    { "pomieszczenie",	1,	'Q',	'B'	},
    { "wyj`scia",	2,	'e',	'G'	},
    { "postacie",	3,	'E',	'm'	},
    { "przedmioty",	4,	'o',	'g'	},
    { "walka",		5,	'f',	'x'	},
    { "chybianie",	6,	'F',	'x'	},
    { "uniki",		7,	'O',	'x'	},
    { "szepty",		8,	't',	'M'	},
    { "telepatia",	9,	'T',	'M'	},
    { "mowa",		0,	'q',	'Y'	},
    { "gaw`eda",	10,	'.',	'G'	},
    { "`spiew",		11,	'u',	'y'	},
    { "wrzask",		12,	'S',	'R'	},
    { "krzyk",		13,	's',	'B'	},
    { "gratulacje",	14,	'U',	'C'	},
    { "kl`atwa",	15,	'k',	'r'	},
    { "klan",		16,	'K',	'G'	},
    { "grupa",		17,	';',	'c'	},
    { NULL,		0,	'-',	'-'	}
};

char kolor_gracza( CHAR_DATA *ch, const char *nazwa )
{
    int i;
    char kol = '-';

    if ( IS_NPC( ch ) && ch->desc && ch->desc->original )
	ch = ch->desc->original;

    for ( i = 0; kolory_graczowe[ i ].nazwa; i++ )
	if ( !str_cmp( nazwa, kolory_graczowe[ i ].nazwa ) )
	{
	    kol = IS_NPC( ch )
		? kolory_graczowe[ i ].domyslny
		: ch->pcdata->kolor[ kolory_graczowe[ i ].pozycja ];

	    if ( kol == '-' )
		kol = kolory_graczowe[ i ].domyslny;
	    return kol;
	}

    bug( "kolor_gracza: blad", 0 );

    return 'x';
}


/*
 * Lam 8.12.2000: zmieniam to na wersje zdecydowanie lepsza. Teraz bedzie to
 * dodatkowy zestaw kolorow obslugiwany przez colour( ) z comm.c. Proste
 * w obsludze, sprzyjajace progom i aliasom z kolorowym gadaniem.
 */
char kolor_gracza_c( CHAR_DATA *ch, const char kod )
{
    int i;
    char kol = '-';

    if ( IS_NPC( ch ) && ch->desc && ch->desc->original )
	ch = ch->desc->original;

    for ( i = 0; kolory_graczowe[ i ].nazwa; i++ )
	if ( kod == kolory_graczowe[ i ].kod )
	{
	    kol = IS_NPC( ch )
		? kolory_graczowe[ i ].domyslny
		: ch->pcdata->kolor[ kolory_graczowe[ i ].pozycja ];

	    if ( kol == '-' )
		kol = kolory_graczowe[ i ].domyslny;
	    return kol;
	}

    return '-';
}


SCHEMAT_DATA *schemat_lookup( const char *arg )
{
    SCHEMAT_DATA *schemat;

    for ( schemat = schemat_list; schemat; schemat = schemat->next )
	if ( !str_prefix( arg, schemat->nazwa ) )
	    break;

    return schemat; /* ew. zwroci NULL, jesli nie znajdzie */
}


KOMENDA( do_barwy )
{
    int i;
    int pol = sizeof( kolory_graczowe ) / sizeof( kolory_graczowe[ 1 ] ) / 2;
    char kol;
    char zdarz[ MIL ];
    char schem[ MIL ];
    SCHEMAT_DATA *schemat;

    if ( IS_NPC( ch ) )
    {
	STC( "Mobom nie wolno konfigurowa`c barw.\n\r", ch );
	return;
    }

    if ( !*argument )
    {
	STC( "{YKonfigurowalne s`a dla ciebie kolory nast`epuj`acych zdarze`n:{x\n\r", ch );

	for ( i = 0; i < pol; i++ )
	{
	    kol = kolor_gracza( ch, kolory_graczowe[ i ].nazwa );
	    ch_printf( ch, "  %s: {%c%s{x",
		wyrownaj( kolory_graczowe[ i ].nazwa, 15 ),
		kol, wyrownaj( kolor_literki( kol ), -15 ) );

	    if ( kolory_graczowe[ i + pol ].nazwa )
	    {
		kol = kolor_gracza( ch, kolory_graczowe[ i + pol ].nazwa );
		ch_printf( ch, "  %s: {%c%s{x\n\r",
		    wyrownaj( kolory_graczowe[ i + pol ].nazwa, 15 ),
		    kol, wyrownaj( kolor_literki( kol ), -15 ) );
	    }
	    else
	    {
		STC( "\n\r", ch );
		break;
	    }
	}
	return;
    }

    argument = one_argument( argument, zdarz );
    if ( !*zdarz || !str_prefix( zdarz, "?" ) || !str_prefix( zdarz, "pomoc " ) )
    {
	STC( "Sk`ladnia: barwy <zdarzenie> [jasny] <kolor>\n\r", ch );
	STC( "  barwy <zdarzenie> domy`slny\n\r", ch );
	STC( "  barwy schemat lista\n\r", ch );
	STC( "  barwy schemat poka`z <nazwa>|domy`slny\n\r", ch );
	STC( "  barwy schemat ustaw <nazwa>|domy`slny\n\r", ch );
	return;
    }

    /* Cadaver 13.12.2001 */
    if ( !str_prefix( zdarz, "schemat" ) )
    {
	argument = one_argument( argument, zdarz );

	if ( !*zdarz )
	{
	    STC( "Sk`ladnia: barwy schemat lista\n\r", ch );
	    STC( "  barwy schemat poka`z <nazwa>|domy`slny\n\r", ch );
	    STC( "  barwy schemat ustaw <nazwa>|domy`slny\n\r", ch );
	    return;
	}

	if ( !str_prefix( zdarz, "lista" ) )
	{
	    STC( "Dost`epne schematy barw:\n\r", ch );
	    for ( schemat = schemat_list; schemat; schemat = schemat->next )
		ch_printf( ch, "%s\n\r", schemat->nazwa );
	    return;
	}

	if ( !str_prefix( zdarz, "poka`z" ) )
	{
	    one_argument( argument, schem );
	    schemat = schemat_lookup( schem );

	    if ( !schemat )
	    {
		STC( "Dost`epne schematy barw:\n\r", ch );
		for ( schemat = schemat_list; schemat; schemat = schemat->next )
		    ch_printf( ch, "%s\n\r", schemat->nazwa );
		return;
	    }

	    ch_printf( ch, "Schemat \"%s\":\n\r", schemat->nazwa );

	    for ( i = 0; i < pol; i++ )
	    {
		kol = schemat->schemat[ kolory_graczowe[ i ].pozycja ] == '-'
		    ? kolory_graczowe[ i ].domyslny
		    : schemat->schemat[ kolory_graczowe[ i ].pozycja ];
		ch_printf( ch, "  %s: {%c%s{x",
		    wyrownaj( kolory_graczowe[ i ].nazwa, 15 ),
		    kol, wyrownaj( kolor_literki( kol ), -15 ) );

		if ( kolory_graczowe[ i + pol ].nazwa )
		{
		    kol = schemat->schemat[ kolory_graczowe[ i + pol ].pozycja ] == '-'
			? kolory_graczowe[ i + pol ].domyslny
			: schemat->schemat[ kolory_graczowe[ i + pol ].pozycja ];
		    ch_printf( ch, "  %s: {%c%s{x\n\r",
			wyrownaj( kolory_graczowe[ i + pol ].nazwa, 15 ),
			kol, wyrownaj( kolor_literki( kol ), -15 ) );
		}
		else
		{
		    STC( "\n\r", ch );
		    break;
		}
	    }
	    return;
	}

	if ( !str_prefix( zdarz, "ustaw" ) )
	{
	    one_argument( argument, schem );
	    schemat = schemat_lookup( schem );

	    if ( !schemat )
	    {
		STC( "Dost`epne schematy barw:\n\r", ch );
		for ( schemat = schemat_list; schemat; schemat = schemat->next )
		    ch_printf( ch, "%s\n\r", schemat->nazwa );
		return;
	    }

	    ch_printf( ch, "Ustawiasz schemat barw \"%s\".\n\r", schemat->nazwa );
	    strcpy( ch->pcdata->kolor, schemat->schemat );
	    return;
	}
	STC( "Sk`ladnia: barwy schemat lista\n\r", ch );
	STC( "  barwy schemat poka`z <nazwa>|domy`slny\n\r", ch );
	STC( "  barwy schemat ustaw <nazwa>|domy`slny\n\r", ch );
	return;
    }

    if ( !*argument )
    {
	STC( "Sk`ladnia: barwy <zdarzenie> [jasny] <kolor>\n\r", ch );
	STC( "  barwy <zdarzenie> domy`slny\n\r", ch );
	return;
    }

    for ( i = 0; kolory_graczowe[ i ].nazwa; i++ )
	if ( !str_prefix( zdarz, kolory_graczowe[ i ].nazwa ) )
	    break;

    if ( !kolory_graczowe[ i ].nazwa )
    {
	STC( "Takiemu zdarzeniu nie mo`zesz przypisywa`c kolor`ow.\n\r", ch );
	return;
    }

    kol = literka_koloru( argument );
    if ( kol == '-' && strcmp( argument, "-" ) )
    {
	STC( "Nie znam takiego koloru.\n\r", ch );
	return;
    }

    if ( kol == '+' || kol == '-' )
    {
	ch_printf( ch, "Zdarzeniu \"%s\" przypisujesz kolor {%cdomy`slny{x.\n\r",
	    kolory_graczowe[ i ].nazwa, kolory_graczowe[ i ].domyslny );
	ch->pcdata->kolor[ kolory_graczowe[ i ].pozycja ] = '-';
	return;
    }

    ch_printf( ch, "Zdarzeniu \"%s\" przypisujesz kolor {%c%s{x.\n\r",
	kolory_graczowe[ i ].nazwa, kol, kolor_literki( kol ) );
    ch->pcdata->kolor[ kolory_graczowe[ i ].pozycja ] = kol;

    return;
}


/*
 * Kolory. Funkcja napisana na nowo przez Lama.
 * Zmienia: wylaczone->wlaczone->namiastki->wylaczone
 */
KOMENDA( do_colour )
{
    char arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[ 0 ] )
    {
	send_to_char( "Napisz kolor, aby zmieni`c ustawienia koloru.\n\r", ch );
	return;
    }

    if ( !ch->kolor )
    {
	ch->kolor = 2;
	send_to_char( "{bK{ro{yl{co{mr{gy{x masz {rW`L`ACZONE{x, ale czad!\n\r", ch );
    }
    else if ( ch->kolor == 1 )
    {
	ch->kolor = 0;
	send_to_char( "Kolory masz wy`l`aczone, ale blama`z...\n\r", ch );
    }
    else
    {
	ch->kolor = 1;
	send_to_char( "Teraz widzisz tylko namiastki kolor`ow.\n\r", ch );
    }

    return;
}


KOMENDA( do_podswietlaj )
{
    char arg[ MAX_INPUT_LENGTH ];
    static char *opcje[ 3 ] =
    {
	"Lac niczego ci nie pod`swietla kolorem gracza.\n\r",
	"Lac pod`swietla ci kolorem gracza tylko imiona.\n\r",
	"Lac pod`swietla ci kolorem gracza ca`l`a linijk`e.\n\r"
    };

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
	ch->podswietl = URANGE( 0, ch->podswietl, 2 );
    else
	ch->podswietl = URANGE( 0, atoi( arg ), 2 );

    STC( opcje[ ch->podswietl ], ch );

    return;
}
