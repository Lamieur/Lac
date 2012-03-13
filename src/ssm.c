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
 *  SSM v2.2 (shared string manager)                                       *
 *                                                                         *
 *  Copyright(C) 1996 Melvin Smith (Fusion) for EnvyMUD 2.2                *
 *                                                                         *
 *  <msmith@falcon.mercer.peachnet.edu>                                    *
 *                                                                         *
 *  Lam: kupa roznych zmian. Sterta lancuchow jest teraz jednym            *
 *   kawalkiem, jest o niebo szybsza, a do tego w ogole nie "fragmentuje   *
 *   sie", nie ma wiec mozliwosci zatkania sie jej seria szybkich zmian    *
 *   duzych lancuchow. Uzyskalem to kosztem nieco wiekszego narzutu        *
 *   zuzycia pamieci, ale bylo warto. Mam tez sprawdzacz lancuchow         *
 *   wykrywajacy w warunkach laboratoryjnych wycieki pamieci lub           *
 *   wielokrotne zwalnianie jej.                                           *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"

#if !defined( ultrix ) && !defined( apollo ) && !defined( __minix )
# include <memory.h>
#endif

/* Lam 21.10.2003: inType byl short int, co powodowalo, ze najwiekszy lancuch
		   lub wolne miejsce moglo miec maksymalnie 64 KiB wielkosci */
#define intType        int
#define uintType       unsigned intType
#define intTypeSize  ( sizeof( intType ) )
#define addrType       void *
#define addrTypeSize ( sizeof( addrType ) )
#define addrSizeMask ( sizeof( addrType ) - 1 )

typedef struct BE BufEntry;

/* Lam 21.10.2003: z jednej listy w jedna strone robie dwie w dwie strony.
   W obecnym stanie krain Laca powoduje to na starcie ponad pol megabajta
   zajetej pamieci wiecej, ale licze na olbrzymie przyspieszenie dzialania muda */
struct BE
{
    BufEntry		*next_entry;
    BufEntry		*prev_entry;
    BufEntry		*next_free;
    BufEntry		*prev_free;
    uintType		size;	/* size of the chunk (regardless of NULL CHAR) */
    intType		usage;	/* how many pointers to the string */
#if defined( DEBUG_SSM )
    intType		ref;	/* ilosc odwolan ze sprawdzacza */
    intType		diff;	/* roznica ref - usage, zeby nie zglaszac wiele razy */
    const char		*fun;	/* funkcja, ktora stworzyla ten lancuch */
    intType		fline;	/* linia w tej funkcji */
#endif
    char		buf[ 1 ];	/* chunk starts here */
};

/*
 * This is for the temporary hashing of strings at bootup to speedup
 * comparison/crunching of the string space. The temp_string_hash will
 * be freed after boot_done() is called.
 */
typedef struct TH TempHash;

struct TH
{
    TempHash *next;
    uintType  len;
    char     *str;
};

TempHash **temp_string_hash;
#define TH_LEN 256
#define TH_LEN_AND 255
#define TH_CHR 64 /* 2 ^ 6, stad 6 w TH_ADDR */
#define TH_CHR_AND 63
#define TH_ADDR( len, chr ) ( ( len << 6 ) + chr )

/* These are the original Merc vars in db.c */
extern bool  fBootDb;
char         str_empty[ 1 ];
char        *string_space;
char        *top_string;
long         nAllocString;
long         sAllocString;
long         nOverFlowString;
long         sOverFlowString;

bool         Full;

volatile bool operacja; /* Lam 2.12.2000: semafor */

#define ZALOZ_BLOKADE_NA_STERTE { if ( operacja ) while ( operacja ) ; \
	operacja = TRUE; }
#define ZDEJMIJ_BLOKADE_ZE_STERTY { operacja = FALSE; }
/* A to wersja powodujaca, ze mud uruchamia sie godzinami:
#define ZALOZ_BLOKADE_NA_STERTE { char bufek[ MIL ]; if ( operacja ) while ( operacja ) ; \
	operacja = TRUE; sprintf( bufek, "poczatek %s", __FUNCTION__ ); if ( !fBootDb ) ssm_check_string_space( bufek ); }
#define ZDEJMIJ_BLOKADE_ZE_STERTY { char bufek[ MIL ]; sprintf( bufek, "poczatek %s", __FUNCTION__ ); if ( !fBootDb ) ssm_check_string_space( bufek ); operacja = FALSE; }
*/

void          temp_hash_add( char * );
char         *temp_hash_find( const char * );

/*
 * ssm_buf_head points to start of shared space,
 * ssm_buf_free points to next free block
 */
BufEntry *ssm_buf_head, *ssm_buf_free;

/* To allocate more memory increase SSM_MEM_SIZE in merc.h. */
unsigned int MAX_STRING = SSM_MEM_SIZE * 1024;
int HEADER_SIZE;

/*
 * Lam 21.10.2003: Zamiast tworzyc 85 kawalkow po 64 KiB, tworze jeden, ktory
 * potem bedzie sie normalnie fragmentowal podczas uzywania.
 */
void init_string_space( void )
{
    string_space = (char *) malloc( MAX_STRING );
    if ( !string_space )
    {
	bug( "[SSM] Can't allocate shared string space.", 0 );
	exit( 1 );
    }

    top_string   = string_space + MAX_STRING - 1;
    ssm_buf_head = (BufEntry *) string_space;
    HEADER_SIZE  = (int) ( (char*) &ssm_buf_head->buf[ 0 ] - (char*) ssm_buf_head );

/* Lam 21.10.2003: tworze jeden wielki wolny blok zamiast wielu malych
   teraz bedzie tak: */
    ssm_buf_head->usage = 0;
    ssm_buf_head->size = MAX_STRING - HEADER_SIZE;
    ssm_buf_head->next_entry = ssm_buf_head->prev_entry
	= ssm_buf_head->next_free = ssm_buf_head->prev_free = NULL;
#if defined( DEBUG_SSM )
    ssm_buf_head->diff = 0;
    ssm_buf_head->ref = 0;
    ssm_buf_head->fun = "";
    ssm_buf_head->fline = 0;
#endif

    ssm_buf_free        = ssm_buf_head;
    temp_string_hash    = (TempHash **) calloc( sizeof( TempHash * ),
						TH_LEN * TH_CHR );

    return;
}


/* Lam 20.10.2003 */
int ssm_max_free_entry( )
{
    BufEntry *ptr;
    unsigned int max = 0;

    for ( ptr = ssm_buf_free; ptr; ptr = ptr->next_free )
	if ( ptr->usage == 0 && ptr->size >= max )
	    max = ptr->size;

    return max;
}


int ssm_num_free_entries( )
{
    BufEntry *ptr;
    int num = 0;

    for ( ptr = ssm_buf_free; ptr; ptr = ptr->next_free )
	if ( ptr->usage == 0 )
	    num++;

    return num;
}


#if defined( DEBUG_SSM )
/* pierwotnie bylo powyzej, wywolywane przez "pamiec" */
void ssm_check_string_space( char *naglowek )
{
    BufEntry *ptr;
    int num = 0, anum = 0;
    int wsize;

    for ( ptr = ssm_buf_free; ptr; ptr = ptr->next_free )
	if ( ptr->usage == 0 )
	    num++;

    for ( ptr = ssm_buf_head; ptr; ptr = ptr->next_entry )
    {
	if ( ptr->usage == 0 )
	    anum++;
	if ( ptr->next_entry )
	{
	    wsize = (int) ptr->next_entry - (int) ptr - HEADER_SIZE;
	    if ( ptr->size != wsize )
	    {
		sprintf( log_buf, "[SSM]: %s: blok o wielkosci %d i uzyciu %d",
		    naglowek, ptr->size, ptr->usage );
		bug( log_buf, 0 );
		bug( ptr->buf, 0 );
		ptr->size = wsize;
	    }
	}
	if ( ptr->usage < 0 )
	{
	    sprintf( log_buf, "[SSM]: %s: blok o uzyciu %d",
		naglowek, ptr->usage );
	    bug( log_buf, 0 );
	    bug( ptr->buf, 0 );
	    ptr->usage = 1000;
	}
    }

    if ( anum != num )
    {
	sprintf( log_buf, "[SSM]: %s: ssm_num_free_entries: Anum %d != num %d!",
	    naglowek, anum, num );
	bug( log_buf, 0 );
    }

    return;
}


void ssm_check_entry( char *naglowek, BufEntry *ptr )
{
    int wsize;

    if ( ptr->next_entry )
    {
	wsize = (int) ptr->next_entry - (int) ptr - HEADER_SIZE;
	if ( ptr->size != wsize )
	{
	    sprintf( log_buf, "[SSM]: %s: blok o wielkosci %d i uzyciu %d",
		naglowek, ptr->size, ptr->usage );
	    bug( log_buf, 0 );
	    bug( ptr->buf, 0 );
	    ptr->size = wsize;
	}
	if ( ptr->next_entry->prev_entry != ptr )
	{
	    sprintf( log_buf, "[SSM]: %s: ptr->next_entry->prev_entry != ptr! ptr 0x%x next 0x%x prev 0x%x",
		naglowek, (unsigned int) ptr, (unsigned int) ptr->next_entry,
		(unsigned int) ptr->next_entry->prev_entry );
	    bug( log_buf, 0 );
	}
    }

    if ( ptr->prev_entry )
    {
	if ( ptr->prev_entry->next_entry != ptr )
	{
	    sprintf( log_buf, "[SSM]: %s: ptr->prev_entry->next_entry != ptr! ptr 0x%x prev 0x%x next 0x%x",
		naglowek, (unsigned int) ptr, (unsigned int) ptr->prev_entry,
		(unsigned int) ptr->prev_entry->next_entry );
	    bug( log_buf, 0 );
	}
    }

    if ( ptr->usage == 0 )
    {
	if ( ptr->next_free )
	{
	    if ( ptr->next_free->prev_free != ptr )
	    {
		sprintf( log_buf, "[SSM]: %s: ptr->next_free->prev_free != ptr! ptr 0x%x next 0x%x prev 0x%x",
		    naglowek, (unsigned int) ptr, (unsigned int) ptr->next_free,
		    (unsigned int) ptr->next_free->prev_free );
		bug( log_buf, 0 );
	    }
	}

	if ( ptr->prev_free )
	{
	    if ( ptr->prev_free->next_free != ptr )
	    {
		sprintf( log_buf, "[SSM]: %s: ptr->prev_free->next_free != ptr! ptr 0x%x prev 0x%x next 0x%x",
		    naglowek, (unsigned int) ptr, (unsigned int) ptr->prev_free,
		    (unsigned int) ptr->prev_free->next_free );
		bug( log_buf, 0 );
	    }
	}
    }

    if ( ptr->usage < 0 )
    {
	sprintf( log_buf, "[SSM]: %s: blok o uzyciu %d",
	    naglowek, ptr->usage );
	bug( log_buf, 0 );
	bug( ptr->buf, 0 );
	ptr->usage = 1000;
    }

    return;
}


#define SCE( naglowek, plik, linia, funkcja, wsk ) \
{ char bufek[ MIL ]; sprintf( bufek, "%s: [%s:%d %s()]", naglowek, plik, linia, funkcja ); \
ssm_check_entry( bufek, wsk ); }
#endif /* DEBUG_SSM */

/*
 * Dup a string into shared space. If string exists, the usage count
 * gets incremented and the reference is returned. If the string does
 * not exist in heap, space is allocated and usage is 1.
 * This is a linked list first fit algorithm, so strings can be
 * freed. Upon bootup, there is a seperate hash table constructed in order
 * to do crunching, then the table is destroyed.
 *
 * Lam: teraz lancuch jest wsadzany tam, gdzie dokladnie pasuje, a jesli nie
 * pasuje dokladnie, to tam, gdzie akurat wejdzie. To powinno ograniczyc czeste
 * defragowanie sterty (troche ograniczyc). No i najwazniejsze: lancuch jest
 * porownywany ze wszystkim co napotka po drodze, zeby mniej zajac w pamieci,
 * jesli juz gdzies tam jest.
 *
 * Lam: wylaczylem to, bo sie boje o szybkosc... musze sprawdzic profilerem.
 *
 * Lam 30.3.98: okazuje sie, ze sporo blokow ma usage < 0 (blad przy moim
 * szukaniu istniejacych lancuchow, ale to nie ja ustawiam usage ujemne, po
 * prostu zapomnialem).
 */
char *real_str_dup( const char *str, const char *plik, int linia,
			const char *funkcja )
{
    BufEntry *ptr;
    BufEntry *ptr2;
    char     *str_new;
    unsigned int len;
    unsigned int rlen;

    if ( !str )
    {
	bug( "str_dup: duplikowanie nulli", 0 );
	return &str_empty[ 0 ];
    }

    if ( !*str )
	return &str_empty[ 0 ];

    ZALOZ_BLOKADE_NA_STERTE

    if ( str > string_space && str < top_string )
    {
	ptr = (BufEntry *)( str - HEADER_SIZE );
	if ( ptr->usage <= 0 )
	{
	    char buf[ MSL ];

	    sprintf( buf, "str_dup() [%s:%d %s()]: zly lancuch (ptr->usage %d)",
			plik, linia, funkcja, ptr->usage );
	    bug( buf, 0 );
	    bug( str, 0 );
	    sprintf( buf, "[BZDET] str_dup() [%s:%d %s()]: z`ly `la`ncuch (ptr->usage %d)",
			plik, linia, funkcja, ptr->usage );
	    wiznet( "$t", NULL, (OBJ_DATA *) buf, NULL, WIZ_DEBUG, 0, 106 );

	    /* to spowoduje, ze zatka pamiec i nigdy nie zwolni, ale wole to od
	       "sypania sie" */
	    ptr->usage = 1000;
	}

	ptr->usage++;
	ZDEJMIJ_BLOKADE_ZE_STERTY
	return (char *) str;
    }

    rlen = len = (int)strlen( str ) + 1;

    /*
     * Round up to machine dependant address size.
     * Don't remove this, because when the BufEntry struct is overlaid
     * the struct must be aligned correctly.
     */

    if ( ( len + HEADER_SIZE ) & addrSizeMask )
	len += addrTypeSize - ( ( len + HEADER_SIZE ) & addrSizeMask );

    if ( ssm_buf_free )
    {
	/* najpierw szukamy, gdzie da sie wepchnac w razie czego */
	for ( ptr = ssm_buf_free; ptr; ptr = ptr->next_free )
	    if ( ptr->usage == 0 && ptr->size >= len )
		break;

	/* a teraz staramy sie wpasowac dokladnie */
	/* bierzemy jak najmniejszy blok, w ktory da sie wepchnac lancuch,
	   a w ogole najlepiej znalezc idealnie takiej samej dlugosci */
	if ( ptr )
	{
	    for ( ptr2 = ssm_buf_free; ptr2; ptr2 = ptr2->next_free )
	    {
		if ( ptr2->size >= len && ptr2->size < ptr->size )
		{
		    ptr = ptr2;
		    if ( ptr->size == len )
			break;
		}
	    }
	}

	if ( !ptr )
	{
	    str_new = (char *)malloc( rlen );
	    strcpy( str_new, str );
	    sOverFlowString += rlen;
	    nOverFlowString++;
	    ZDEJMIJ_BLOKADE_ZE_STERTY
	    return str_new;
	}
	/* Lam 21.10.2003: jesli nie bedzie miejsca na co najmniej 4 literki
	   w nastepnym lancuchu, nie ma sensu dodawac wskaznikow do listy, to
	   by tylko spowolnilo jej przechodzenie, nigdy nie dajac lancucha */
	else if ( (signed) ( ptr->size - len ) > HEADER_SIZE + 4 )
	{
	    BufEntry *temp;

#if defined( DEBUG_SSM )
	    SCE( "str_dup, przed dzieleniem bloku, ptr", plik, linia, funkcja, ptr );
#endif
	    /* WARNING! - DONT REMOVE THE CASTS BELOW! - Fusion */
	    temp = (BufEntry *)( (char *) ptr + HEADER_SIZE + len );
	    temp->size		= ptr->size - ( len + HEADER_SIZE );
	    temp->next_entry	= ptr->next_entry;
	    temp->prev_entry	= ptr;
	    ptr->next_entry	= temp;
	    if ( temp->next_entry )
		temp->next_entry->prev_entry = temp;
	    temp->next_free	= ptr->next_free;
	    temp->prev_free	= ptr->prev_free;
	    if ( temp->next_free )
		temp->next_free->prev_free = temp;
	    if ( temp->prev_free )
		temp->prev_free->next_free = temp;
	    temp->usage		= 0;
#if defined( DEBUG_SSM )
	    temp->diff		= 0;
	    temp->ref		= 0;
	    temp->fun		= "";
	    temp->fline		= 0;
#endif
	    ptr->size		= len;
	    ptr->usage		= 1;
	    if ( !ssm_buf_free
	      || ( ssm_buf_free > temp )
	      || ( ssm_buf_free == ptr ) )
		ssm_buf_free = temp;
	    ptr->next_free = NULL;
	    ptr->prev_free = NULL;
#if defined( DEBUG_SSM )
	    SCE( "str_dup, po dzieleniu bloku, ptr", plik, linia, funkcja, ptr );
	    SCE( "str_dup, po dzieleniu bloku, temp", plik, linia, funkcja, temp );
	    if ( temp->next_entry ) SCE( "str_dup, po dzieleniu bloku, temp->next", plik, linia, funkcja, temp->next_entry );
	    if ( ptr->prev_entry ) SCE( "str_dup, po dzieleniu bloku, ptr->prev", plik, linia, funkcja, ptr->prev_entry );
#endif
	}
	else
	{
#if defined( DEBUG_SSM )
	    SCE( "str_dup, przed zajeciem bloku, ptr", plik, linia, funkcja, ptr );
#endif
	    ptr->usage = 1;
	    if ( ptr == ssm_buf_free )
	    {
		ssm_buf_free = ptr->next_free;
		if ( ptr->next_free )
		    ptr->next_free->prev_free = NULL;
	    }
	    else
	    {
		ptr->prev_free->next_free = ptr->next_free;
		if ( ptr->next_free )
		    ptr->next_free->prev_free = ptr->prev_free;
	    }
	    ptr->next_free = NULL;
	    ptr->prev_free = NULL;
#if defined( DEBUG_SSM )
	    SCE( "str_dup, po zajeciu bloku, ptr", plik, linia, funkcja, ptr );
#endif
	}

#if defined( DEBUG_SSM )
	ptr->fun	= funkcja;
	ptr->fline	= linia; /* plik sie znajdzie po funkcji */
#endif
	str_new = (char *)&ptr->buf[ 0 ];
	strcpy( str_new, str );
	nAllocString++;
	sAllocString += ptr->size + HEADER_SIZE;
    }
    else
    {
	/* A one time toggle just for bugging purposes */
	if ( !Full )
	{
#if defined( DEBUG_SSM )
	    bug( "[SSM] Koniec miejsca na stercie na lancuchy! UWAGA! Lancuchy alokowane poza sterta nie sa sprawdzane pod katem wyciekow!", 0 );
#else
	    bug( "[SSM] Koniec miejsca na stercie na lancuchy!", 0 );
#endif
	    wiznet( "[BZDET] [SSM] Koniec sterty na lancuchy!", NULL, NULL, NULL, WIZ_DEBUG, 0, 106 );
	    Full = 1;
	}

	str_new = (char *)malloc( rlen );
	strcpy( str_new, str );
	sOverFlowString += rlen;
	nOverFlowString++;
    }

    ZDEJMIJ_BLOKADE_ZE_STERTY

    return str_new;
}


/*
 * If string is in shared space, decrement usage, if usage then is 0,
 * free the chunk and attempt to merge with next node. Other
 * strings are freed with standard free.
 * Never call free/delete externally on a shared string.
 *
 * Lam 5.3.98: co dwudziesty free_string sam defraguje sterte. Okazalo sie przy
 * okazji, ze wcale nie laczyl blokow (po zakonczeniu wczytywania krain)
 */
void real_free_string( char *str, const char *plik, int linia,
			const char *funkcja )
{
    BufEntry *ptr;

    if ( !str )
    {
	char buf[ MSL ];

	sprintf( buf, "free_string() [%s:%d %s()]: zwalnianie NULLa",
			plik, linia, funkcja );
	bug( buf, 0 );
	sprintf( buf, "[BZDET] free_string() [%s:%d %s()]: zwalnianie NULLa",
			plik, linia, funkcja );
	wiznet( "$t", NULL, (OBJ_DATA *) buf, NULL, WIZ_DEBUG, 0, 106 );
	return;
    }

    if ( str == &str_empty[ 0 ] )
	return;

    ZALOZ_BLOKADE_NA_STERTE
    if ( str > string_space && str < top_string )
    {
	ptr = (BufEntry *)( str - HEADER_SIZE );

#if defined( DEBUG_SSM )
	SCE( "free_string, przed zmniejszeniem uzycia, ptr", plik, linia, funkcja, ptr );
#endif
	if ( --ptr->usage > 0 )
	{
	    ZDEJMIJ_BLOKADE_ZE_STERTY
	    return;
	}
	else if ( ptr->usage < 0 )
	{
	    char buf[ MSL ];

	    sprintf( buf, "free_string() [%s:%d %s()]: wielokrotne zwalnianie? (ptr->usage %d)",
			plik, linia, funkcja, ptr->usage );
	    bug( buf, 0 );
	    bug( (char*) &ptr->buf[ 0 ], 0 );
	    sprintf( buf, "[BZDET] free_string() [%s:%d %s()]: wielokrotne zwalnianie? (ptr->usage %d)",
			plik, linia, funkcja, ptr->usage );
	    wiznet( "$t", NULL, (OBJ_DATA *) buf, NULL, WIZ_DEBUG, 0, 106 );
	    ptr->usage = 1000; /* niech sobie wisi, moze ma wiecej odwolan */
	    ZDEJMIJ_BLOKADE_ZE_STERTY
	    return;
	}

#if defined( DEBUG_SSM )
	SCE( "free_string, po zmniejszeniu uzycia, ptr", plik, linia, funkcja, ptr );
#endif
	sAllocString -= ( ptr->size + HEADER_SIZE );
	nAllocString--;

	if ( !ssm_buf_free || ( ssm_buf_free > ptr ) )
	{
	    ptr->next_free = ssm_buf_free;
	    if ( ssm_buf_free )
		ssm_buf_free->prev_free = ptr;
	    ssm_buf_free = ptr;
	    ptr->prev_free = NULL;
#if defined( DEBUG_SSM )
	    SCE( "free_string, po wstawieniu zamiast ssm_buf_free, ptr", plik, linia, funkcja, ptr );
#endif
	}
	else
	{
	    BufEntry *pop, *nast;
	    for ( pop = ssm_buf_free; pop; pop = nast )
	    {
		nast = pop->next_free;
		if ( pop < ptr && nast > ptr )
		{
		    ptr->next_free = nast;
		    ptr->prev_free = pop;
		    nast->prev_free = pop->next_free = ptr;
		    break;
		}
		else if ( !nast )
		{
		    ptr->prev_free = pop;
		    ptr->next_free = NULL;
		    pop->next_free = ptr;
		    break;
		}
	    }
	    if ( !pop )
		bug( "free_string: nie moge zaktualizowac listy wolnych blokow!", 0 );
#if defined( DEBUG_SSM )
	    SCE( "free_string, po aktualizacji listy, ptr", plik, linia, funkcja, ptr );
	    if ( nast ) SCE( "free_string, po aktualizacji listy, nast", plik, linia, funkcja, nast );
	    if ( pop ) SCE( "free_string, po aktualizacji listy, pop", plik, linia, funkcja, pop );
#endif
	}

	if ( fBootDb )
	{
	    TempHash *tptr;
	    TempHash *walk;
	    int ihashl = strlen( str ) & TH_LEN_AND;
	    int ihashc = *str & TH_CHR_AND;

	    for ( tptr = temp_string_hash[ TH_ADDR( ihashl, ihashc ) ]; tptr; tptr = tptr->next )
	    {
		if ( tptr->str != str )
		    continue;
		else if ( tptr == temp_string_hash[ TH_ADDR( ihashl, ihashc ) ] )
		    temp_string_hash[ TH_ADDR( ihashl, ihashc ) ] = tptr->next;
		else
		    for ( walk = temp_string_hash[ TH_ADDR( ihashl, ihashc ) ]; walk;
			walk = walk->next )
		    {
			if ( walk->next == tptr )
			{
			    walk->next = tptr->next;
			    break;
			}
		    }

		free( tptr );
		break;
	    }
	}
	else
	{
	    BufEntry *pzl;

	    if ( ( pzl = ptr->next_entry ) && pzl == ptr->next_free )
	    {
		ptr->next_free = pzl->next_free;
		if ( ptr->next_free )
		    ptr->next_free->prev_free = ptr;
		ptr->next_entry = pzl->next_entry;
		if ( ptr->next_entry )
		    ptr->next_entry->prev_entry = ptr;
		ptr->size += pzl->size + HEADER_SIZE;
#if defined( DEBUG_SSM )
		SCE( "free_string, po polaczeniu z nastepnym, ptr", plik, linia, funkcja, ptr );
#endif
	    }

	    if ( ( pzl = ptr->prev_entry ) && pzl == ptr->prev_free )
	    {
		pzl->next_free = ptr->next_free;
		if ( pzl->next_free )
		    pzl->next_free->prev_free = pzl;
		pzl->next_entry = ptr->next_entry;
		if ( pzl->next_entry )
		    pzl->next_entry->prev_entry = pzl;
		pzl->size += ptr->size + HEADER_SIZE;
#if defined( DEBUG_SSM )
		SCE( "free_string, po polaczeniu z poprzednim, pzl", plik, linia, funkcja, pzl );
#endif
	    }
	}
	ZDEJMIJ_BLOKADE_ZE_STERTY
	return;
    }

    ZDEJMIJ_BLOKADE_ZE_STERTY
    sOverFlowString -= strlen( str ) + 1;
    nOverFlowString--;
    if ( sOverFlowString < 0 || nOverFlowString < 0 )
    {
	bug( "SSM:free_string: string free from outside SS space.", 0 );
	bug( str, 0 );
    }
    free( str );

    return;
}


/*
 * Read and allocate space for a string from a file.
 * This replaces db.c fread_string
 * This is modified version of Furey's fread_string from Merc
 */
char *real_fread_string( FILE *fp, int *status, const char *plik, int linia,
		const char *funkcja )
{
    static char buf[ MAX_STRING_LENGTH * 16 ];
    char *ptr = buf;
    int c;
    bool spacja;

    *status = 0;
    spacja = FALSE;

    do { c = getc( fp ); }
    while ( isspace( c ) );

    if ( c == '~' )
	return &str_empty[ 0 ];

    ungetc( c, fp );

    for ( ; ; )
    {
	c = getc( fp );
	*ptr = c;
	if ( *ptr < 0 && c != EOF )
	{
	    sprintf( bug_buf, "Polska literka w krainie (kod %d, \\0%o)?", c, c );
	    bug( bug_buf, 0 );
	}
	switch ( c )
	{
	    default:
		ptr++;
		spacja = FALSE;
		break;

	    case ' ':
		ptr++;
		spacja = TRUE;
		break;

	    case EOF:
		bug( "Fread_string: EOF", 0 );
		*status = 1;
		return NULL;

	    case '\n':
		ptr++;
		/* Glaca ani konwerter nie potrzebuja \n\r na koncu linii. */
		if ( !TylkoKonwersja )
		    *ptr++ = '\r';
		if ( spacja && fBootDb )
		    cbug( "Spacja na koncu linii", 0 );
		spacja = FALSE;
		break;

	    case '\r':
		break;

	    case '~':
		/* dobre, ale pomoc-l.are zawiera takie legalne :)
		if ( spacja )
		    cbug( "Spacja przed tylda", 0 ); */
		*ptr = '\0';
		if ( fBootDb )
		{
		    ptr = temp_hash_find( buf );
		    if ( ptr )
		    {
			return real_str_dup( ptr, plik, linia, funkcja );
		    }

		    ptr = real_str_dup( buf, plik, linia, funkcja );
		    temp_hash_add( ptr );
		    return ptr;
		}

		return real_str_dup( buf, plik, linia, funkcja );
	}
    }

    cbug( "niespodziewany przypadek", 0 );

    return NULL;
}


/*
 * Lam 10.4.2002 do czytania i pisania komentarzy w resetach, z tego powodu nie
 * pomija bialych znakow
 */
char *fread_string_eol( FILE *fp, int *status )
{
    static char buf[ MAX_STRING_LENGTH * 16 ];
    char *ptr = buf;
    int c;

    *status = 0;

/*
    do { c = getc( fp ); }
    while ( isspace( c ) );
*/
    c = getc( fp );

    if ( ( *ptr++ = c ) == '\n' )
	return &str_empty[ 0 ];

    for ( ; ; )
    {
	c = getc( fp );
	*ptr = c;
	switch ( c )
	{
	    default:
		ptr++;
		break;

	    case EOF:
		bug( "Fread_string_eol: EOF", 0 );
		*status = 1;
		return NULL;

	    case '\n':
	    case '\r':
		*ptr = '\0';
		if ( fBootDb )
		{
		    ptr = temp_hash_find( buf );
		    if ( ptr )
		    {
			return str_dup( ptr );
		    }

		    ptr = str_dup( buf );
		    temp_hash_add( ptr );
		    return ptr;
		}

		return str_dup( buf );
	}
    }

    cbug( "niespodziewany przypadek", 0 );

    return NULL;
}


/* Lookup the string in the boot-time hash table. */
char *temp_hash_find( const char *str )
{
    TempHash *ptr;
    int       len;
    int       ihashl, ihashc;

    if ( !fBootDb || !*str )
	return 0;

    len = strlen( str );
    ihashl = len & TH_LEN_AND;
    ihashc = *str & TH_CHR_AND;

    for ( ptr = temp_string_hash[ TH_ADDR( ihashl, ihashc ) ]; ptr; ptr = ptr->next )
    {
	if ( *ptr->str != *str )
	    continue;
	else if ( strcmp( ptr->str, str ) )
	    continue;
	else
	    return ptr->str;
    }

    return 0;
}


/*
 * Add a reference in the temporary hash table.
 * String is still in the linked list structure but
 * reference is kept here for quick lookup at boot time;
 */
void temp_hash_add( char *str )
{
    TempHash *add;
    int      len;
    int      ihashl, ihashc;

    if ( !fBootDb || !*str || str <= string_space || str >= top_string )
	return;

    len   = strlen( str );
    ihashl = len & TH_LEN_AND;
    ihashc = *str & TH_CHR_AND;
    add   = (TempHash *) malloc( sizeof( TempHash ) );
    add->next = temp_string_hash[ TH_ADDR( ihashl, ihashc ) ];
    temp_string_hash[ TH_ADDR( ihashl, ihashc ) ] = add;
    add->len = len;
    add->str = str;

    return;
}


#undef POKAZ_TH
/* Free the temp boot string hash table */
void boot_done( void )
{
    TempHash *ptr, *next;
    int ihashl, ihashc;

    for ( ihashl = 0; ihashl < TH_LEN; ihashl++ )
    {
#if defined( POKAZ_TH )
	fprintf( stderr, "dlug: %3d: ", ihashl );
#endif
	for ( ihashc = 0; ihashc < TH_CHR; ihashc++ )
#if defined( POKAZ_TH )
	{
	    int ile = 0;
#endif
	    for ( ptr = temp_string_hash[ TH_ADDR( ihashl, ihashc ) ]; ptr; ptr = next )
	    {
		next = ptr->next;
		free( ptr );
#if defined( POKAZ_TH )
		ile++;
#endif
	    }
#if defined( POKAZ_TH )
	    fprintf( stderr, "%4d", ile );
	    if ( ile > 2000 ) fprintf( stderr, "(%c %c)", (char) ihashc, (char) ihashl );
	}
	fprintf( stderr, "\n\r" );
#endif
    }

    free( temp_string_hash );
    temp_string_hash = 0; /* Bug check in case someone accesses later */

    return;
}


#if defined( DEBUG_SSM )

extern OBJ_INDEX_DATA *obj_index_hash[ MAX_KEY_HASH ];
extern ROOM_INDEX_DATA *room_index_hash[ MAX_KEY_HASH ];
extern AREA_DATA *area_first;
extern KOLES_DATA *lista_kolesi;
extern ZONE_DATA *zone_first;
extern char *daPrompt;


/*
 * Lam 30.11.2000: sprawdzacz lancuchow.
 *
 * Zdarzaja sie sytuacje, gdy pewien lancuch zostaje zwolniony, a wciaz gdzies
 * pozostaje do niego wskaznik, ktory w pewnym momencie jest ponownie uzywany,
 * lub zwalniany. Jest to blad mogacy w wyniku powodowac powazne straty danych,
 * uszkodzenia plikow postaci, przeklamania nazw przedmiotow, postaci (w zwiazku
 * z czym - bledne zapisywanie, nadpisywanie cudzych postaci kopiami swojej
 * i tego typu przyjemne rzeczy).
 *
 * Az tak straszne rzeczy nie zdarzaja sie na szczescie (raz sie zdarzylo
 * z powodu bledu, ktory zostal natychmiast wykryty i poprawiony), ale
 * wciaz gdzies tkwi blad objawiajacy sie czasami i musze go dorwac!
 *
 * Inna sytuacja to ta, gdy gdzies w kodzie lancuch jest tworzony za pomoca
 * str_dup( ) albo fread_string( ), ale wskaznik do niego jest zapominany
 * i lancuch nigdy nie jest zwalniany.
 *
 * Dopisek: przed sytuacja z pozostawaniem lancuchow po free_string
 * zabezpieczylem sie dopisujac do makra free_string zerowanie wskaznika.
 * Najczesciej zadziala jak nalezy. Co najwyzej bedzie wywalac muda. Ale wole
 * dostac core i odpluskwiac niz obserwowac nagle zmiany lancuchow w losowych
 * miejscach w kodzie.
 */
void czysc_odwolania( void )
{
    BufEntry *p;

    for ( p = ssm_buf_head; p; p = p->next_entry )
	p->ref = 0;

    return;
}


void oznacz( char *lancuch )
{
    BufEntry *p;

    if ( !lancuch )
	return;

    if ( lancuch < string_space || lancuch > top_string )
	return;

    p = (BufEntry *) ( lancuch - HEADER_SIZE );
    p->ref++;

    return;
}


void zrzut_kwasow_lancuchow( void )
{
    FILE *fp;
    BufEntry *p;
    bool czy_plik = FALSE;

    for ( p = ssm_buf_head; p; p = p->next_entry )
	if ( p->usage != p->ref && ( p->usage - p->ref ) != p->diff )
	{
	    if ( !czy_plik )
	    {
		zamknij_reserve( fpBugReserve );
		czy_plik = TRUE;
		if ( !( fp = fopen( SSM_FILE, "a" ) ) )
		{
		    lac_perror( SSM_FILE ); /* to moze niezle zapchac logi */
		    fpBugReserve = fopen( NULL_FILE, "r" );
		    czy_plik = FALSE;
		}
		if ( czy_plik )
		    fprintf( fp, "-S-S-M-> %s", (char *) asctime_pl( localtime( &current_time ) ) );
	    }

	    if ( czy_plik )
		fprintf( fp, "%s %p usage %d ref %d fun %s:%d |%s\n",
		    p->usage < p->ref ? "!" : " ", (void *) p,
		    p->usage, p->ref,
		    p->fun, p->fline, p->buf );
	    if ( p->usage < p->ref )
		p->usage = p->ref;
	    p->diff = p->usage - p->ref;
	}

    if ( !czy_plik )
	return;
    fprintf( fp, "<-S-S-M- %s", (char *) asctime_pl( localtime( &current_time ) ) );
    fclose( fp );
    fpBugReserve = fopen( NULL_FILE, "r" );

    return;
}


#define O( x ) oznacz( x )

void walk_helps( void )
{
    HELP_DATA *h;

    for ( h = help_first; h; h = h->next )
    {
	O( h->keyword );
	O( h->text );
    }

    return;
}


void walk_rooms( void )
{
    ROOM_INDEX_DATA *r;
    EXTRA_DESCR_DATA *e;
    MPROG_DATA *m;
    int i, w;

    for ( i = 0; i < MAX_KEY_HASH; i++ )
	for ( r = room_index_hash[ i ]; r; r = r->next )
	{
	    O( r->name );
	    O( r->miejscownik );
	    O( r->description );
	    O( r->anti_race );
	    O( r->anti_class );
	    if ( r->trap )
		O( r->trap->trap_string );
	    for ( w = 0; w < 10; w++ )
		if ( r->exit[ w ] )
		{
		    O( r->exit[ w ]->keyword );
		    O( r->exit[ w ]->short_descr );
		    O( r->exit[ w ]->biernik );
		    O( r->exit[ w ]->description );
		    for ( m = r->exit[ w ]->mobprogs; m; m = m->next )
		    {
			O( m->arglist );
			O( m->comlist );
		    }
		}
	    for ( e = r->extra_descr; e; e = e-> next )
	    {
		O( e->keyword );
		O( e->description );
	    }
	    for ( m = r->mobprogs; m; m = m->next )
	    {
		O( m->arglist );
		O( m->comlist );
	    }
	}

    return;
}


void walk_mob_indexes( void )
{
    MOB_INDEX_DATA *m;
    MPROG_DATA *p;
    int i;

    for ( i = 0; i < MAX_KEY_HASH; i++ )
	for ( m = mob_index_hash[ i ]; m; m = m->next )
	{
	    O( m->player_name );
	    O( m->short_descr );
	    O( m->long_descr );
	    O( m->dopelniacz );
	    O( m->celownik );
	    O( m->biernik );
	    O( m->narzednik );
	    O( m->miejscownik );
	    O( m->wolacz );
	    O( m->description );
	    for ( p = m->mobprogs; p; p = p->next )
	    {
		O( p->arglist );
		O( p->comlist );
	    }
	}

    return;
}


void walk_obj_indexes( void )
{
    OBJ_INDEX_DATA *o;
    MPROG_DATA *p;
    int i;

    for ( i = 0; i < MAX_KEY_HASH; i++ )
	for ( o = obj_index_hash[ i ]; o; o = o->next )
	{
	    O( o->name );
	    O( o->short_descr );
	    O( o->dopelniacz );
	    O( o->celownik );
	    O( o->biernik );
	    O( o->narzednik );
	    O( o->miejscownik );
	    O( o->description );
	    O( o->look_descr );
	    O( o->action );
	    O( o->anti_class );
	    O( o->anti_race );
	    for ( p = o->mobprogs; p; p = p->next )
	    {
		O( p->arglist );
		O( p->comlist );
	    }
	    if ( typy_przedmiotow[ o->item_type ].spell[ 4 ] == VAL_STRING )
		O( o->value[ 4 ].p );
	}

    return;
}


void walk_one_char( CHAR_DATA *c )
{
    PC_DATA *p;
    TELL_DATA *t;
    NOTE_DATA *n;
    ALIAS_DATA *a;
    MPQUEST_DATA *q;
    RESTART_DATA *r;
    TYP_ZMIENNEJ *z;
    int i;

    O( c->name );
    O( c->short_descr );
    O( c->dopelniacz );
    O( c->celownik );
    O( c->biernik );
    O( c->narzednik );
    O( c->miejscownik );
    O( c->wolacz );
    O( c->long_descr );
    O( c->long_descr_orig );
    O( c->false_name );
    O( c->description );
    if ( ( p = c->pcdata ) )
    {
	O( p->pwd );
	O( p->bamfin );
	O( p->bamfout );
	O( p->immskll );
	O( p->title );
	O( p->prompt );
	O( p->fprompt );
	O( p->stopien );
	O( p->login );
	O( p->last_login );
	O( p->email );
	O( p->love.od );
	O( p->silence.od );
	O( p->noemote.od );
	O( p->nocolour.od );
	O( p->notell.od );
	O( p->log.od );
	O( p->freeze.od );
	O( p->deny.od );
	O( p->thief_from );
	O( p->killer_from );
	O( p->zabojca_from );
	O( p->ignore );
	O( p->pojedynek );
	O( p->nienawisc );
	O( p->szpic_wybr );
	O( p->szpic_wykl );
	for ( r = p->restart; r; r = r->nast )
	    O( r->wpis );
	for ( i = 0; i < MAX_OSTATNIE; i++ )
	    O( p->ostatnie[ i ] );
    }
    for ( t = c->tell; t; t = t->next )
	O( t->tell );
    if ( ( n = c->pnote ) )
    {
	O( n->sender );
	O( n->date );
	O( n->to_list );
	O( n->subject );
	O( n->text );
	O( n->yes );
	O( n->no );
	O( n->abst );
    }
    for ( a = c->alias; a; a = a->next )
    {
	O( a->name );
	O( a->todo );
    }
    for ( q = c->mpquest; q; q = q->next )
    {
	O( q->title );
	O( q->descr );
    }
    for ( z = c->zmienne; z; z = z->nast )
	if ( z->typ == TZ_ZNAK )
	    O( z->wartosc.p );

    return;
}


void walk_chars( void )
{
    CHAR_DATA *c;

    for ( c = char_list; c; c = c->next )
	walk_one_char( c );

    return;
}


void walk_objs( void )
{
    OBJ_DATA *o;

    for ( o = object_list; o; o = o->next )
    {
	    O( o->name );
	    O( o->short_descr );
	    O( o->dopelniacz );
	    O( o->celownik );
	    O( o->biernik );
	    O( o->narzednik );
	    O( o->miejscownik );
	    O( o->description );
	    O( o->action );
	    O( o->anti_class );
	    O( o->anti_race );
	    O( o->owner );
	    if ( typy_przedmiotow[ o->item_type ].spell[ 4 ] == VAL_STRING )
		O( o->value[ 4 ].p );
    }

    return;
}


void walk_descriptors( void )
{
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d; d = d->next )
    {
	O( d->host );
	O( d->login );
	O( d->user );
	O( d->term_type );
	O( d->lsc_host );
	O( d->lsc_user );
	O( d->showstr_head );
	/* jesli postac nie jest jeszcze w char_list */
	if ( d->character && !d->character->next )
	    walk_one_char( d->character );
    }

    return;
}


void walk_quits( void )
{
    int i;

    for ( i = 0; i <= quit_count; i++ )
	O( quit_table[ i ] );

    return;
}


void walk_poses( void )
{
    int i, k;

    for ( k = 0; k < MAX_CLASS; k++ )
	for ( i = 0; i < pose_count[ k ]; i++ )
	{
	    O( pose_table[ k ][ i ].to_char );
	    O( pose_table[ k ][ i ].to_room );
	}

    return;
}


void walk_hints( void )
{
    int i;

    for ( i = 0; hint_table[ i ]; i++ )
	O( hint_table[ i ] );

    return;
}


void walk_tracks( void )
{
    KOLES_DATA *k;
    int i;

    for ( k = lista_kolesi; k; k = k->nast )
	for ( i = 0; k->drogi[ i ].nazwa; i++ )
	{
	    O( k->drogi[ i ].nazwa );
	    O( k->drogi[ i ].droga );
	}

    return;
}


void walk_areas( void )
{
    AREA_DATA *a;
    RESET_DATA *r;
    TYP_ZMIENNEJ *z;

    for ( a = area_first; a; a = a->next )
    {
	O( a->name );
	O( a->full_name );
	O( a->reset_msg );
	O( a->opis );
	O( a->file_name );
	O( a->revision );
	for ( r = a->reset_first; r; r = r->next )
	    O( r->string );
	for ( z = a->zmienne; z; z = z->nast )
	    if ( z->typ == TZ_ZNAK )
		O( z->wartosc.p );
    }

    return;
}


void walk_clans( void )
{
    CLAN_DATA *c;
    CLAN_MEMBER_DATA *m;
    int i;

    for ( c = clan_list; c; c = c->next )
    {
	O( c->name );
	O( c->dop );
	O( c->dir );
	O( c->info );
	O( c->statut );
	for ( i = 0; i < 10; i++ )
	{
	    O( c->lev_name_mm[ i ] );
	    O( c->lev_name_zm[ i ] );
	    O( c->lev_name_md[ i ] );
	    O( c->lev_name_zd[ i ] );
	    O( c->lev_name_mn[ i ] );
	    O( c->lev_name_zn[ i ] );
	}
	for ( m = c->member_list; m; m = m->next )
	    O( m->name );
    }

    return;
}


void walk_notes( void )
{
    NOTE_DATA *n;

    for ( n = note_list; n; n = n->next )
    {
	O( n->sender );
	O( n->date );
	O( n->to_list );
	O( n->subject );
	O( n->text );
	O( n->yes );
	O( n->no );
	O( n->abst );
    }

    return;
}


void walk_bans( void )
{
    BAN_DATA *b;

    for ( b = ban_list; b; b = b->next )
    {
	O( b->banned );
	O( b->user );
	O( b->host );
    }

    return;
}


void walk_zones( void )
{
    ZONE_DATA *z;

    for ( z = zone_first; z; z = z->next )
    {
	O( z->nazwa );
	O( z->dopelniacz );
	O( z->celownik );
	O( z->biernik );
	O( z->narzednik );
	O( z->miejscownik );
	O( z->stolica );
	O( z->stol_dop );
	O( z->stol_cel );
	O( z->stol_bie );
	O( z->stol_narz );
	O( z->stol_mie );
	O( z->wladca );
	O( z->namiestnik );
	O( z->rasy );
    }

    return;
}


void walk_miodek_bluzg( void )
{
    int i;

    for ( i = 0; miodek_table[ i ].slowo; i++ )
    {
	O( miodek_table[ i ].slowo );
	O( miodek_table[ i ].zastepstwo );
    }

    for ( i = 0; offensive_table[ i ]; i++ )
	O( offensive_table[ i ] );

    return;
}


void walk_schematy( void )
{
    SCHEMAT_DATA *s;

    for ( s = schemat_list; s; s = s->next )
    {
	O( s->nazwa );
	O( s->schemat );
    }

    return;
}


#if defined( BLEDOMAT )
void walk_bledomat( void )
{
    extern struct bledomat_data bledomat;
    O( bledomat.adres );
    O( bledomat.uzytkownik );
    O( bledomat.haslo );
    O( bledomat.baza );
    O( bledomat.przedrostek );

    return;
}
#endif


void walk_powody( void )
{
    int i;

    for ( i = 0; i < MAX_POWODY; i++ )
	if ( powody[ i ].skrot )
	{
	    O( powody[ i ].skrot );
	    O( powody[ i ].nazwa );
	    O( powody[ i ].tytul );
	    O( powody[ i ].objasnienie );
	}

    return;
}


void walk_imiona( void )
{
    int i, j;
    IMIONA_DATA *im;

    for ( i = 0; i <= MAX_DLUG_IMIENIA; i++ )
	for ( j = 0; j < 64; j++ )
	    for ( im = imiona[ i ][ j ]; im; im = im->nast )
	    {
		O( im->imie );
		O( im->blokujacy );
	    }

    return;
}


void oznacz_wszystko( void )
{
    czysc_odwolania( );

    O( daPrompt );
    walk_quits( );
    walk_poses( );
    walk_hints( );
    walk_tracks( );
    walk_clans( );
    walk_notes( );
    walk_bans( );
    walk_miodek_bluzg( );
    walk_schematy( );
    walk_powody( );
    walk_imiona( );
#if defined( BLEDOMAT )
    walk_bledomat( );
#endif

    walk_zones( );
    walk_areas( );
    walk_helps( );
    walk_rooms( );
    walk_mob_indexes( );
    walk_obj_indexes( );

    walk_chars( );
    walk_objs( );
    walk_descriptors( );

    return;
}


#endif /* defined( DEBUG_SSM ) */
