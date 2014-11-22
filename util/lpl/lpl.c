/*
 * lpl (lacowe polskie litery)
 *
 * Lam 31.12.1999
 *
 * Program zamieniajacy polskie litery w plikach Laca na normalne ISO-8859-2
 * i na odwrot, w miedzyczasie odpalajac edytor do edycji pliku.
 *
 * 31.3.2002: poprawiony blad ze zmienna EDITOR
 * 4.7.2006: dodana opcja, aby nie odpalal edytora (do automatycznego
 *		sprawdzania, czy ktos np. nie napisal `te zamiast t`e)
 */
#define _GNU_SOURCE
#include <stdio.h>	/* printf( ) */
#include <stdlib.h>	/* getenv( ) */
#include <string.h>	/* strncpy( ) */
#include <sys/stat.h>	/* umask( ) */
#include <locale.h>	/* setlocale( ) */
#include <langinfo.h>	/* nl_langinfo( ) */

/*
 * Tu konfiguruje sie standard polskich liter (domyslnie LIN)
 */
#define DOS		1
#define LIN		2
#define WIN		3
#define UTF		4
/* konfiguracja edytora, jesli EDITOR nie daje odpowiedzi */
#define ZAPASOWY_EDYTOR	"joe"


const char *polska_tablica[ 5 ][ 18 ] =
{
	{
		/* Odpowiedniki angielskie */
		"A", "a", "C", "c", "E", "e",
		"L", "l", "N", "n", "O", "o",
		"S", "s", "X", "x", "Z", "z"
	},
	{
		/* DOS, CP 852 */
		"§", "•", "è", "Ü", "®", "©",
		"ù", "à", "„", "‰", "‡", "¢",
		"ó", "ò", "ç", "´", "Ω", "æ"
	},
	{
		/* Linux, ISO 8859-2 */
		"°", "±", "∆", "Ê", " ", "Í",
		"£", "≥", "—", "Ò", "”", "Û",
		"¶", "∂", "¨", "º", "Ø", "ø"
	},
	{
		/* Windows, CP 1250 */
		"•", "π", "∆", "Ê", " ", "Í",
		"£", "≥", "—", "Ò", "”", "Û",
		"å", "ú", "è", "ü", "Ø", "ø"
	},
	{
		/* taki mam w xterm -u8 */
		"ƒÑ", "ƒÖ", "ƒÜ", "ƒá", "ƒò", "ƒô",
		"≈Å", "≈Ç", "≈É", "≈Ñ", "√ì", "√≥",
		"≈ö", "≈õ", "≈π", "≈∫", "≈ª", "≈º"
	}
};


const char *polska( char *znak, int standard )
{
	int nr;

	if ( *znak == ' ' )
	{
		*znak = 0;
		return "†";
	}

	for ( nr = 0; nr < 18; nr++ )
	{
		if ( *polska_tablica[ 0 ][ nr ] == *znak )
		{
			*znak = 0;
			return polska_tablica[ standard ][ nr ];
		}
	}
	/* wyciecie `, jesli wystepuje po nim niepoprawna literka */
	return "";
}


char angielska( char *znak, int standard )
{
	int nr;
	char pom;

	if ( *znak == '`' )
		return '`';

	if ( *znak == '†' )
	{
		*znak = ' ';
		return '`';
	}

	for ( nr = 0; nr < 18; nr++ )
	{
		if ( *polska_tablica[ standard ][ nr ] == *znak
		  && ( standard != UTF
		    || polska_tablica[ standard ][ nr ][ 1 ] == znak[ 1 ] ) )
		{
			*znak = *polska_tablica[ 0 ][ nr ];
			return '`';
		}
	}
	pom = *znak;
	*znak = 0;
	return pom;
}


int main( int argc, char **argv )
{
	char editor[ 256 ];
	char sysparm[ 1024 ];
	char nazwapliku[ 512 ];
	char nazwatym[ 512 ];
	FILE *plikwe;
	FILE *plikwy;
	char *kaka;
	char znak;
	char znaki[ 3 ];
	int zamien = 0;
	int tryb = 0;
	int standard = LIN;

	umask( 066 );
	kaka = getenv( "EDITOR" );
	strncpy( editor, kaka ? kaka : ZAPASOWY_EDYTOR, 255 );
	if ( editor[ 0 ] == '\0'
	  || !strcmp( basename( editor ), basename( argv[ 0 ] ) ) )
		strcpy( editor, ZAPASOWY_EDYTOR );

	setlocale( LC_ALL, "" );
	kaka = nl_langinfo( CODESET );
	if ( !strcmp( kaka, "UTF-8" ) )
		standard = UTF; /* w przeciwnym razie zostaje ISO-8859-2 */

	if ( argc == 3 )
	{
		if ( !strcmp( argv[ 2 ], "-n" ) )
			tryb = 1;
		else
		{
			printf( "Uøycie: lpl <plik> [-n].\n\r" );
			return -1;
		}
		argc = 2;
	}

	if ( argc != 2 )
	{
		printf( "Uøycie: lpl <plik> [-n].\n\r" );
		return -1;
	}

	strncpy( nazwapliku, argv[ 1 ], 511 );
	sprintf( nazwatym, "/tmp/Lac-lpl-%s-XXXXXX", editor );
/*
	if ( !mktemp( nazwatym ) )
*/
	if ( !mkstemp( nazwatym ) )
	{
		perror( "lpl" );
		return -1;
	}

	plikwe = fopen( nazwapliku, "r" );
	plikwy = fopen( nazwatym, "w" );

	if ( !plikwe )
	{
		perror( nazwapliku );
		exit( 1 );
	}

	if ( !plikwy )
	{
		perror( nazwatym );
		exit( 1 );
	}

	while ( !feof( plikwe ) )
	{
		znak = getc( plikwe );
		if ( feof( plikwe ) )
			break;
		if ( znak == '\r' ) /* pozbycie sie dosowych koncow linii */
			continue;
		if ( znak == '`' && !zamien )
		{
			zamien = 1;
		}
		else
		{
			if ( zamien && znak != '`' )
				fprintf( plikwy, "%s", polska( &znak, standard ) );
			if ( znak )
				putc( znak, plikwy );
			zamien = 0;
		}
	}

	if ( zamien ) /* zeby nie obcinal ` na koncu pliku */
		putc( '`', plikwy );
	fclose( plikwy );
	fclose( plikwe );

	if ( tryb == 0 )
	{
		sprintf( sysparm, "%s %s", editor, nazwatym );
		system( sysparm );
	}

	plikwe = fopen( nazwatym, "r" );
	plikwy = fopen( nazwapliku, "w" );

	while ( !feof( plikwe ) )
	{
		znaki[ 0 ] = getc( plikwe );
		if ( standard == UTF
		  && ( znaki[ 0 ] & 128 ) )
			znaki[ 1 ] = getc( plikwe );
		if ( feof( plikwe ) )
			break;
		putc( angielska( znaki, standard ), plikwy );
		if ( znaki[ 0 ] )
			putc( znaki[ 0 ], plikwy );
	}

	fclose( plikwy );
	fclose( plikwe );

	sprintf( sysparm, "rm -f %s*", nazwatym ); /* * zeby usunac takze plik~ */
	system( sysparm );

	return 0;
}
