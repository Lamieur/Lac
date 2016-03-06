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
#include "handler.h"
#if defined( BLEDOMAT )
# include <mysql.h>
#endif


/* 14.12.2001: Lam, zamiast starego kodu Nopa */

/* definicja elementu stosu */
typedef struct stos STOS;
struct stos
{
    char op;	/* operacja, w s2 moze byc zerem */
    float wart;     /* wartosc - uzywane tylko w s2 */
    STOS *nast;
};

/*
 * Local functions.
 */
const char *char_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	sekretarka_ook_ost	args( ( CHAR_DATA *ch, CHAR_DATA *victim, char *argument ) );
void	sprawdz_zdatnosc	args( ( CHAR_DATA *ch, char *skl, bool *blad ) );
char	*licz_nastepny_skladnik	args( ( char *wyrazenie, char *skladnik ) );
void    note_attach     args( ( CHAR_DATA *ch ) );
void    note_remove     args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void    talk_channel    args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb, const char *verb2,
			    int poziom ) );
void	zglos_blad	args( ( CHAR_DATA *ch, char *opis, char *dopeln, char *plik,
			    int kategoria, char *argument ) );
void	real_tell	args( ( CHAR_DATA *ch, CHAR_DATA *victim, char *argument,
			    bool can_be_ignored, int tell_type ) );
bool	zglos_do_bazy	args( ( CHAR_DATA *ch, int kategoria, char *argument ) );
void	wywal_monitory	args( ( CHAR_DATA *ch ) );
bool	pusty_stos	args( ( STOS *s ) );
void	na_stos		args( ( STOS **s, char c ) );
void	na_stos_w	args( ( STOS **s, float w ) );
char	glowa		args( ( STOS *s ) );
char	ze_stosu	args( ( STOS **s ) );
float	ze_stosu_w	args( ( STOS **s ) );
bool	licz_czy_liczba	args( ( char *w ) );
int	priorytet	args( ( CHAR_DATA *ch, char c, bool *blad ) );
float	wynik		args( ( CHAR_DATA *ch, STOS **s, bool *blad ) );

#define ILOSC_KATEGORII 4
const char kategorie_not[ ILOSC_KATEGORII ][ 40 ] =
{
    "Nowo`sci",
    "Wszystko",
    "Bzdety",
    "Kawa`ly"
};

bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( !str_cmp( ch->name, pnote->sender ) )
	return TRUE;

    if ( czy_do_wszystkich( pnote->to_list ) )
	return TRUE;

    if ( get_trust( ch ) >= L_SEN
      && ( is_name2( "bogowie",   pnote->to_list )
	|| is_name2( "bog`ow",    pnote->to_list ) ) )
	return TRUE;

    if ( IS_HERO( ch ) && ( is_name2( "bohaterowie",   pnote->to_list )
			 || is_name2( "bohater`ow",    pnote->to_list ) ) )
	return TRUE;

    if ( is_name2( ch->name, pnote->to_list ) )
	return TRUE;

    if ( is_name2( class_table[ ch->klasa ].who_name, pnote->to_list ) )
	return TRUE; /* notka do klasy */

    /* Malven, 22.09.2003; Ulryk, 2005 */
    if ( !IS_NPC( ch ) && ch->pcdata->clan
      && ch->pcdata->clan_member->level > 0 )
    {
	if ( is_name2( ch->pcdata->clan->name, pnote->to_list ) )
	    return TRUE; /* Cadaver 30.07.2001, 19.09.2001: notka do klanu */
    }

    return FALSE;
}


const char *char_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( !str_cmp( ch->name, pnote->sender ) )
	return "{R->";

    if ( czy_do_wszystkich( pnote->to_list ) )
    {
	if ( is_name2( ch->name, pnote->to_list ) )
	    return " *";
	else
	    return "  ";
    }

    if ( get_trust( ch ) >= L_SEN && ( is_name2( "bogowie", pnote->to_list )
				    || is_name2( "bog`ow",  pnote->to_list ) ) )
	return "{Y!!";

    if ( IS_HERO( ch ) && ( is_name2( "bohaterowie",   pnote->to_list )
			 || is_name2( "bohater`ow",    pnote->to_list ) ) )
	return "{Y++";

    if ( is_name2( ch->name, pnote->to_list ) )
	return "{R**";

    if ( is_name2( class_table[ ch->klasa ].who_name, pnote->to_list ) )
	return "{Y--";

    /* Malven, 22.09.2003; Ulryk, 2005 */
    if ( ch->pcdata->clan && ch->pcdata->clan_member->level > 0 )
    {
	if ( is_name2( ch->pcdata->clan->name, pnote->to_list ) )
	    return "{Y==";
    } /* Cadaver 30.07.2001, 19.09.2001: notki klanowe */

    return "{G? ";
}


void note_attach( CHAR_DATA *ch )
{
    NOTE_DATA *pnote;

    if ( ch->pnote )
	return;

    if ( !note_free )
	pnote     = alloc_perm( sizeof( *ch->pnote ) );
    else
    {
	pnote     = note_free;
	note_free = note_free->next;
    }

    pnote->next         = NULL;
    pnote->sender       = str_dup( ch->short_descr );
    pnote->date         = str_dup( "" );
    pnote->to_list      = str_dup( "" );
    pnote->subject      = str_dup( "" );
    pnote->yes          = str_dup( "" );
    pnote->no           = str_dup( "" );
    pnote->abst         = str_dup( "" );
    pnote->vote		= VOTE_NO;
    pnote->yesnum	= 0;
    pnote->nonum	= 0;
    pnote->abstnum	= 0;
    pnote->text         = str_dup( "" );
    ch->pnote           = pnote;

    return;
}


/*
 * Lam: wczesniej to bylo wszedzie kopiowane. Co prawda jest to (kopiowanie)
 * latwy sposob na dogonienie rozmiarem smauga, ale nie chce mi sie za kazda
 * zmiana dopisywac ja w 10 miejscach.
 */
void rewrite_all_notes( void )
{
    NOTE_DATA *pnote;
    FILE *fp;

    zamknij_reserve( fpReserve );
    if ( !( fp = fopen( NOTE_FILE, "w" ) ) )
	lac_perror( NOTE_FILE );
    else
    {
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    fprintf( fp, "Sender  %s~\n", pnote->sender     );
	    fprintf( fp, "Date    %s~\n", pnote->date       );
	    fprintf( fp, "Stamp   %ld\n", (unsigned long)pnote->date_stamp );
	    fprintf( fp, "To      %s~\n", pnote->to_list    );
	    fprintf( fp, "Subject %s~\n", pnote->subject    );
	    if ( pnote->vote != VOTE_NO )
	    {
		fprintf( fp, "Vote    %d\n",  pnote->vote );
		fprintf( fp, "Votes   %d %d %d\n", pnote->yesnum, pnote->nonum, pnote->abstnum );
		fprintf( fp, "Yes     %s~\n", pnote->yes );
		fprintf( fp, "No      %s~\n", pnote->no );
		fprintf( fp, "Abstain %s~\n", pnote->abst );
	    }
	    fprintf( fp, "Text\n%s~\n\n", pnote->text       );
	}
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    NOTE_DATA *prev;
    char      *to_list;
    char       to_new [ MAX_INPUT_LENGTH ];
    char       to_one [ MAX_INPUT_LENGTH ];

    /*
     * Build a new to_list.
     * Strip out this recipient.
     */
    to_new[ 0 ]   = '\0';
    to_list     = pnote->to_list;
    while ( *to_list != '\0' )
    {
	to_list = one_argument( to_list, to_one );
	if ( to_one[ 0 ] != '\0' && str_cmp( ch->name, to_one ) )
	{
	    strcat( to_new, " "    );
	    strcat( to_new, to_one );
	}
    }

    /*
     * Just a simple recipient removal?
     */
    if ( ( ch->level < 108 )
      && str_cmp( ch->name, pnote->sender )
      && to_new[ 0 ] != '\0' )
    {
	free_string( pnote->to_list );
	pnote->to_list = str_dup( to_new + 1 );
	return;
    }

    /*
     * Remove note from linked list.
     */
    if ( pnote == note_list )
	note_list = pnote->next;
    else
    {
	for ( prev = note_list; prev; prev = prev->next )
	{
	    if ( prev->next == pnote )
		break;
	}

	if ( !prev )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}

	prev->next = pnote->next;
    }

    free_string( pnote->text    );
    free_string( pnote->subject );
    free_string( pnote->to_list );
    free_string( pnote->date    );
    free_string( pnote->sender  );
    free_string( pnote->yes	);
    free_string( pnote->no	);
    free_string( pnote->abst	);
    pnote->vote    = VOTE_NO;
    pnote->nonum   = 0;
    pnote->yesnum  = 0;
    pnote->abstnum = 0;
    pnote->next = note_free;
    note_free   = pnote;

    rewrite_all_notes();

    return;
}


/* Date stamp idea comes from Alander of ROM */
/*
 * Lam 20.3.98: duzy buf1, zabezpieczenie przed padaniem muda przy "nota czytaj
 * wsz"
 * Lam 10.3.98: glosowanie
 * Lam 11.3.98: zmiana daty stempla jesli taki sam
 */
KOMENDA( do_note )
{
    NOTE_DATA       *pnote;
    DESCRIPTOR_DATA *d;
    char       buf  [ MAX_STRING_LENGTH    ];
    char       buf1 [ MAX_STRING_LENGTH*8  ];
    char       arg  [ MAX_INPUT_LENGTH     ];
    char       votebuf [ MAX_STRING_LENGTH ];
    int        vnum;
    int        anum;
    int        pos;    /* \                        */
/*  int        count;      > pomocne przy 'note -' */
    int        last;   /* /                        */

    if ( IS_NPC( ch ) )
	return;

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( arg[ 0 ] == '\0' )
    {
	strcpy( arg, "czytaj" );
	do_note( ch, arg );
	return;
    }

    if ( !str_prefix( arg, "lista" ) )
    {
	vnum    = 0;
	buf1[ 0 ] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	    if ( is_note_to( ch, pnote ) )
	    {
		sprintf( buf, "%s{b[{M%s{Y%3d{r%s{b] {R%s{x: {g%s{x\n\r",
			char_note_to( ch, pnote ),
			pnote->vote == VOTE_OPEN ? "+" :
			  pnote->vote == VOTE_CLOSED ? "-" : " ",
			vnum,
			( pnote->date_stamp > ch->last_note
			 && str_cmp( pnote->sender, ch->name ) ) ? "N" : " ",
			pnote->sender, pnote->subject );
		strcat( buf1, buf );
		vnum++;
	    }

	send_to_char( buf1, ch );
	return;
    }

    if ( !str_prefix( arg, "czytaj" ) )
    {
	bool fAll;

	if ( argument[ 0 ] != '\0' && !str_prefix( argument, "wszystkie" ) )
	{
	    fAll = TRUE;
	    anum = 0;
	}
	else if ( argument[ 0 ] == '\0' || !str_prefix( argument, "nast`epna" ) )
	  /* read next unread note */
	{
	    vnum    = 0;
	    buf1[ 0 ] = '\0';
	    for ( pnote = note_list; pnote; pnote = pnote->next )
	    {
		if ( is_note_to( ch, pnote )
		  && str_cmp( ch->name, pnote->sender )
		  && ch->last_note < pnote->date_stamp )
		{
		    break;
		}
		else if ( is_note_to( ch, pnote ) )
		    vnum++;
	    }
	    if ( pnote )
	    {
		sprintf( buf, "{b[{y%3d{b] {R%s{x: {g%s{x\n\r{g%s{x\n\r{yDo: {B%s{x\n\r",
			vnum,
			pnote->sender,
			pnote->subject,
			pnote->date,
			pnote->to_list );
		strcat( buf1, buf );
		strcat( buf1, pnote->text );
		if ( pnote->vote )
		{
		    strcat( buf1, "{M-----------------------------------------------------------------------------{x\n\r" );
		    if ( pnote->vote == VOTE_OPEN )
			sprintf( buf, "{GG`losowanie na not`e jest wci`a`z otwarte.{x\n\r" );
		    else
			sprintf( buf, "{GG`losowanie zosta`lo zamkni`ete.{x\n\r" );
		    strcat( buf1, buf );
		    sprintf( buf, "{BOddano g`losy: {GTak: {Y%d{G  Nie: {Y%d{G  Wstrzyma`lo si`e: {Y%d{x\n\r",
			pnote->yesnum, pnote->nonum, pnote->abstnum );
		    strcat( buf1, buf );
		}
		ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		send_to_char( buf1, ch );
		return;
	    }
	    send_to_char( "Nie masz nieprzeczytanych not.\n\r", ch );
	    return;
	}
	else if ( is_number( argument ) )
	{
	    fAll = FALSE;
	    anum = atoi( argument );
	}
	else
	{
	    send_to_char( "Kt`or`a not`e chcesz przeczyta`c?\n\r", ch );
	    return;
	}

	vnum    = 0;
	buf1[ 0 ] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) )
	    {
		if ( vnum == anum || fAll )
		{
		    sprintf( buf, "{b[{y%3d{b] {R%s{x: {g%s{x\n\r{g%s{x\n\r{yDo: {B%s{x\n\r",
			    vnum,
			    pnote->sender,
			    pnote->subject,
			    pnote->date,
			    pnote->to_list );
		    if ( strlen( buf1 ) + strlen( buf )
			 + strlen( pnote->text ) < MAX_STRING_LENGTH * 7 )
		    {
			strcat( buf1, buf );
			strcat( buf1, pnote->text );
			/* ponizsze na pewno zajmuje mniej niz MSL */
			if ( pnote->vote )
			{
			    strcat( buf1, "{M-----------------------------------------------------------------------------{x\n\r" );
			    if ( pnote->vote == VOTE_OPEN )
				sprintf( buf, "{GG`losowanie na not`e jest wci`a`z otwarte.{x\n\r" );
			    else
				sprintf( buf, "{GG`losowanie zosta`lo zamkni`ete.{x\n\r" );
			    strcat( buf1, buf );
			    sprintf( buf, "{BOddano g`losy: {GTak: {Y%d{G  Nie: {Y%d{G  Wstrzyma`lo si`e: {Y%d{x\n\r",
				pnote->yesnum, pnote->nonum, pnote->abstnum );
			    strcat( buf1, buf );
			}
		    }
		    else
		    {
			send_to_char( "Nie jestem w stanie wy`swietli`c takiej ilo`sci tekstu.\n\r", ch );
			return;
		    }
		    if ( !fAll )
			send_to_char( buf1, ch );
		    else
			strcat( buf1, "\n\r" );
		    ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		    if ( !fAll )
			return;
		}
		vnum++;
	    }
	}

	if ( !fAll )
	    send_to_char( "Nie ma takiej noty.\n\r", ch );
	else
	    send_to_char( buf1, ch );
	return;
    }

    if ( !str_prefix( arg, "g`losowanie" ) )
    {
	char varg [ MAX_INPUT_LENGTH ];
	int numer;

	argument = one_argument( argument, varg );
	numer = atoi( varg );
	if ( !*argument || !varg[ 0 ] || !is_number( varg ) || numer < 0 )
	{
	    send_to_char( "Sk`ladnia: nota g`losowanie <numer> <otw`orz|zamknij|tak|nie|wstrzymaj>\n\r", ch );
	    return;
	}
	/* znalezc notke o numerze numer */
	vnum = 0;
	for ( pnote = note_list; pnote; pnote = pnote->next )
	    if ( is_note_to( ch, pnote ) )
	    {
		if ( vnum == numer )
		    break;
		vnum++;
	    }

	if ( !pnote )
	{
	    send_to_char( "Nie ma takiej noty.\n\r", ch );
	    return;
	}
	if ( !str_prefix( argument, "otw`orz" ) )
	{
	    if ( !str_cmp( pnote->sender, ch->name ) )
	    {
		pnote->vote = VOTE_OPEN;
		send_to_char( "G`losowanie otwarte.\n\r", ch );
	    }
	    else
		send_to_char( "Nie jeste`s autorem noty.\n\r", ch );
	    return;
	}
	if ( !str_prefix( argument, "zamknij" ) )
	{
	    if ( !str_cmp( pnote->sender, ch->name ) )
	    {
		pnote->vote = VOTE_CLOSED;
		send_to_char( "G`losowanie zamkni`ete.\n\r", ch );
	    }
	    else
		send_to_char( "Nie jeste`s autorem noty.\n\r", ch );
	    return;
	}
	if ( pnote->vote != VOTE_OPEN )
	{
	    send_to_char( "Na t`e not`e nie mo`zna g`losowa`c.\n\r", ch );
	    return;
	}
	/* zabezpieczenie przed przepelnianiem buforow */
	if ( strlen( pnote->yes )  + strlen( ch->name ) > MAX_STRING_LENGTH - 12
	  || strlen( pnote->abst ) + strlen( ch->name ) > MAX_STRING_LENGTH - 12
	  || strlen( pnote->no )   + strlen( ch->name ) > MAX_STRING_LENGTH - 12 )
	{
	    send_to_char( "Oddano ju`z wystarczaj`aco du`zo g`los`ow. G`losowanie zamkni`ete.\n\r", ch );
	    pnote->vote = VOTE_CLOSED;
	    return;
	}
	if ( is_name2( ch->name, pnote->yes )
	  || is_name2( ch->name, pnote->no )
	  || is_name2( ch->name, pnote->abst ) )
	{
	    ASTC( "Ju`z raz g`losowa`l$o`s na t`e not`e.", ch );
	    return;
	}
	if ( !str_prefix( argument, "tak" ) )
	{
	    pnote->yesnum++;
	    strcpy( votebuf, pnote->yes );
	    strcat( votebuf, " " );
	    strcat( votebuf, ch->name );
	    free_string( pnote->yes );
	    pnote->yes = str_dup( votebuf );
	    ASTC( "Odda`l$o`s sw`oj g`los.", ch );
	    return;
	}
	if ( !str_prefix( argument, "nie" ) )
	{
	    pnote->nonum++;
	    strcpy( votebuf, pnote->no );
	    strcat( votebuf, " " );
	    strcat( votebuf, ch->name );
	    free_string( pnote->no );
	    pnote->no = str_dup( votebuf );
	    ASTC( "Odda`l$o`s sw`oj g`los.", ch );
	    return;
	}
	if ( !str_prefix( argument, "wstrzymaj" ) )
	{
	    pnote->abstnum++;
	    strcpy( votebuf, pnote->abst );
	    strcat( votebuf, " " );
	    strcat( votebuf, ch->name );
	    free_string( pnote->abst );
	    pnote->abst = str_dup( votebuf );
	    ASTC( "Odda`l$o`s sw`oj g`los.", ch );
	    return;
	}
	strcpy( arg, "g`losowanie" );
	do_note( ch, arg ); /* wyprodukuje info o skladni */
	return;
    }

    if ( !str_cmp( arg, "+" ) )
    {
	note_attach( ch );
	strcpy( buf, ch->pnote->text );
	if ( strlen( buf ) + strlen( argument ) >= MAX_STRING_LENGTH - 200 )
	{
	    send_to_char( "Za d`luga nota.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r"   );
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char( "Dodajesz lini`e noty.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "-" ) )
    {
	if ( !ch->pnote )
	{
	    send_to_char( "Nie piszesz `zadnej noty.\n\r", ch );
	    return;
	}

	if ( !*ch->pnote->text )
	{
	    send_to_char( "Nie masz sk`ad usuwa`c.\n\r", ch );
	    return;
	}

	strcpy( buf, ch->pnote->text );

	last = 0;

	for ( pos = 0; buf[ pos ] != '\0'; pos++ )
	    if ( buf[ pos ] == '\r' && buf[ pos + 1 ] != '\0' )
		last = pos;

	buf[ last + 1 ] = '\0';
	if ( last == 0 ) buf[ 0 ] = '\0';

	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char( "Usuwasz lini`e noty.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "tytu`l" )
      || !str_prefix( arg, "temat" ) )
    {
	note_attach( ch );
	free_string( ch->pnote->subject );
	ch->pnote->subject = str_dup( argument );
	ASTC( "Zmieni`l$o`s tytu`l noty.", ch );
	return;
    }

    if ( !str_prefix( arg, "do" ) )
    {
	note_attach( ch );
	free_string( ch->pnote->to_list );
	ch->pnote->to_list = str_dup( argument );
	ASTC( "Zmieni`l$o`s adresata noty.", ch );
	if ( is_name2( "all", argument ) )
	    ASTC( "Chyba zapomnia`l$o`s, `ze pisze si`e \"wszyscy\" a nie \"all\"...", ch );
	return;
    }

    if ( !str_prefix( arg, "czy`s`c" ) )
    {
	if ( ch->pnote )
	{
	    free_string( ch->pnote->text    );
	    free_string( ch->pnote->subject );
	    free_string( ch->pnote->to_list );
	    free_string( ch->pnote->date    );
	    free_string( ch->pnote->sender  );
	    ch->pnote->next     = note_free;
	    note_free           = ch->pnote;
	    ch->pnote           = NULL;
	}

	ASTC( "Wyczy`sci`l$o`s notk`e.", ch );
	return;
    }

    if ( !str_prefix( arg, "poka`z" ) )
    {
	if ( !ch->pnote )
	{
	    send_to_char( "Nie piszesz `zadnej noty.\n\r", ch );
	    return;
	}

	sprintf( buf, "%s: %s\n\rDo: %s\n\r",
		ch->pnote->sender,
		ch->pnote->subject,
		ch->pnote->to_list );
	send_to_char( buf, ch );
	send_to_char( ch->pnote->text, ch );
	return;
    }

    if ( !str_prefix( arg, "wy`slij" ) )
    {
	FILE *fp;
	char *strtime;

	if ( !ch->pnote )
	{
	    send_to_char( "Nie piszesz `zadnej noty.\n\r", ch );
	    return;
	}

	if ( !str_cmp( ch->pnote->to_list, "" ) )
	{
	    send_to_char(
	      "Musisz okre`sli`c adresata [imi`e/wszyscy/bogowie].\n\r",
			 ch );
	    return;
	}

	if ( !str_cmp( ch->pnote->subject, "" ) )
	{
	    send_to_char( "Musisz poda`c temat noty.\n\r", ch );
	    return;
	}

	ch->pnote->next                 = NULL;
	strtime                         = asctime_pl( localtime( &current_time ) );
	strtime[ strlen( strtime ) - 1 ] = '\0';
	free_string( ch->pnote->date );
	ch->pnote->date                 = str_dup( strtime );
	ch->pnote->date_stamp           = current_time;

	if ( !note_list )
	    note_list   = ch->pnote;
	else
	{
	    for ( pnote = note_list; pnote->next; pnote = pnote->next )
		;
	    if ( pnote->date_stamp >= ch->pnote->date_stamp )
		ch->pnote->date_stamp++; /* Lam */
	    pnote->next = ch->pnote;
	}
	pnote           = ch->pnote;
	ch->pnote       = NULL;

	zamknij_reserve( fpReserve );
	if ( !( fp = fopen( NOTE_FILE, "a" ) ) )
	    lac_perror( NOTE_FILE );
	else /* zapisanie notki, glosowania na nia i tak nie ma */
	{
	    fprintf( fp, "Sender  %s~\n", pnote->sender     );
	    fprintf( fp, "Date    %s~\n", pnote->date       );
	    fprintf( fp, "Stamp   %ld\n", (unsigned long)pnote->date_stamp );
	    fprintf( fp, "To      %s~\n", pnote->to_list    );
	    fprintf( fp, "Subject %s~\n", pnote->subject    );
	    fprintf( fp, "Text\n%s~\n\n", pnote->text       );
	    fclose( fp );
	}
	fpReserve = fopen( NULL_FILE, "r" );

	for ( d = descriptor_list; d; d = d->next )
	    if ( d->connected == CON_PLAYING
	      && is_note_to( d->character, pnote )
	      && str_cmp( d->character->name, pnote->sender ) )
		send_to_char( "{YMasz now`a not`e!{x\n\r", d->character );

	send_to_char( "Nota zosta`la wys`lana.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "usu`n" ) )
    {
	if ( !is_number( argument ) )
	{
	    send_to_char( "Kt`or`a not`e usun`a`c?\n\r", ch );
	    return;
	}

	anum = atoi( argument );
	vnum = 0;
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && vnum++ == anum )
	    {
		note_remove( ch, pnote );
		send_to_char( "Nota zosta`la usuni`eta.\n\r", ch );
		return;
	    }
	}

	send_to_char( "Nie ma takiej noty.\n\r", ch );
	return;
    }

    send_to_char( "Co? Napisz \"pomoc nota\", aby dowiedzie`c si`e wi`ecej o notach.\n\r", ch );

    return;
}


/*
 * Ustawianie adresow postaciom.
 */
KOMENDA( do_email )
{
    char         buf [MAX_STRING_LENGTH];

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Od kiedy moby maj`a emaile?\n\r", ch );
	return;
    }

    if ( argument[ 0 ] == '\0' )
    {
	if ( !ch->pcdata->email || ( *ch->pcdata->email == '\0' ) )
	    send_to_char( "Nie znam twojego adresu pocztowego.\n\r", ch );
	else
	{
	    sprintf( buf, "Tw`oj adres pocztowy to: %s\n\r", ch->pcdata->email );
	    send_to_char( buf, ch );
	}
	return;
    }

    if ( czy_mail( argument ) )
    {
	free_string( ch->pcdata->email );
	ch->pcdata->email = str_dup( argument );
	sprintf( buf, "Tw`oj adres pocztowy to: %s\n\r", ch->pcdata->email );
	send_to_char( buf, ch );
    }
    else
	send_to_char( "To nie wygl`ada na poprawny adres poczty elektronicznej.\n\r", ch );

    return;
}


/*
 * How to make a string look drunk by Apex <robink@htsa.hva.nl>
 * Modified and enhanced by Maniac from Mythran
 *
 * Lam 14.6.2003: statyczny bufor zwracany przez funkcje zamiast przepelniania
 * buforow zmieniajac je bez sensu. Jeden z najidiotyczniejszych kawalkow kodu
 * z Envy.
 *
 * Lam 27.3.2005: wiele buforow na wypadek zapetlenia mowy speech_progami,
 * skrocenie wyniku na sile do MIL.
 */
char *makedrunk( char *string, CHAR_DATA *ch )
{
    static char buf[ 12 ][ MIL ];
    static int ktory;
    char temp;
    int  randomnum;
    int  drunkpos;
    int  drunklevel = 0;
    int  pos        = 0;

    if ( ++ktory > 11 )
	ktory = 0;

    if ( IS_NPC( ch ) || !WP( ch ) )
    {
	strcpy( buf[ ktory ], string );
	return buf[ ktory ];
    }

    /* Check how drunk a person is... */
    if ( IS_AFFECTED( ch, AFF_DEZORIENTACJA ) )
	drunklevel = number_range( 1, 30 );
    else
	drunklevel = ch->pcdata->condition[ COND_DRUNK ];

    if ( drunklevel == 0 )
    {
	strcpy( buf[ ktory ], string );
	return buf[ ktory ];
    }

    buf[ ktory ][ 0 ] = '\0';
    do {
	temp     = UPPER( *string );
	drunkpos = temp - 'A';

	if ( ( temp >= 'A' ) && ( temp <= 'Z' ) )
	{
	    if ( drunklevel > drunk[ drunkpos ].min_drunk_level )
	    {
		randomnum = number_range( 0, drunk[ drunkpos ].number_of_rep );
		strcpy( &buf[ ktory ][ pos ],
		       drunk[ drunkpos ].replacement[ randomnum ] );
		pos += strlen( drunk[ drunkpos ].replacement[ randomnum ] );
	    }
	    else
		buf[ ktory ][ pos++ ] = *string;
	}
	else
	{
	    if ( ( temp >= '0' ) && ( temp <= '9' ) )
	    {
		temp         = '0' + number_range( 0, 9 );
		buf[ ktory ][ pos++ ] = temp;
	    }
	    else
		buf[ ktory ][ pos++ ] = *string;
	}
    } while ( *++string && pos < MIL - 9 );
    /* MIL - 9, bo musi wystarczyc na kolejna literke, ktora moze dac "kukeleku"
       (8 liter) */

    buf[ ktory ][ pos ] = '\0';
    /* tak to wygladalo w Envy. Pad przy mowieniu czegokolwiek zbyt dlugiego
       bedac bardzo pijanym, a takze zawsze przy rzucaniu czaru "brama" bedac
       pod wplywem
    strcpy( string, buf ); */

    return buf[ ktory ];
}


/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel,
		  const char *verb, const char *verb2, int poziom )
{
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH ];
    char             bufI[ MAX_STRING_LENGTH ];
    int              position;
    int              vict_type = TO_VICT;

    if ( !strlen_pl_kol( argument ) )
    {
	ch_printf( ch, "To chcesz %s, czy nie?\n\r", verb2 );
	bug( "Brak argumentu.", 0 );
	return;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
    {
	sprintf( buf, "Nie mo`zesz %s.\n\r", verb2 );
	send_to_char( buf, ch );
	return;
    }

    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "Ruszaj`a si`e twe wargi, ale niczego nie s`lycha`c...\n\r", ch );
	return;
    }

    if ( IS_SET( ch->deaf, channel ) )
    {
	REMOVE_BIT( ch->deaf, channel );
	send_to_char( "Ten kana`l by`l wy`l`aczony. W`l`aczam ten kana`l.\n\r", ch );
    }

    /*
     * Lam 15.12.2002
     */
    if ( miodek( ch, argument ) )
	return;

    /*
     * Make the words look drunk if needed...
     */
    argument = makedrunk( argument, ch );

    if ( IS_SET( channel, CHANNEL_NOIC ) )
    {
	vict_type |= ACT_TELEP;
	REMOVE_BIT( channel, CHANNEL_NOIC );
    }

    switch ( channel )
    {
    default:
	sprintf( bufI, "%ssz: \"%s\"\n\r", verb, argument );
	bufI[ 0 ] = UPPER( bufI[ 0 ] );
	sprintf( buf, "$n %s: \"$t\"", verb );
	break;

/*    case CHANNEL_AUCTION:
	sprintf( bufI, "{y%ssz: \"%s{y\"{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "{y$n %s: \"$t{y\"{x", verb );
	break; */

    case CHANNEL_IM: /* Lam: InterMud */
	sprintf( bufI, "{W%ssz: \"%s{W\"{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "${W$n$}W %s: \"$t$}W\"{x", verb );
	break;

    case CHANNEL_MUSIC:
	sprintf( bufI, "{u%ssz: \"%s{u\"{x\n\r", verb, argument );
	/* Buf[ 3 ] bo buf[ 2 ] to ` od `spiewa */
	bufI[ 3 ] = UPPER( bufI[ 3 ] );
	sprintf( buf, "${u$n$}u %s: \"$t$}u\"{x", verb );
	break;

/*
    case CHANNEL_QUESTION:
	sprintf( bufI, "{y%ssz: \"%s{y\"{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "{y$n %s: \"$t{y\"{x", verb );
	break;
*/

    case CHANNEL_SHOUT:
	sprintf( bufI, "{S%ssz: \"%s{S\"{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "${S$n$}S %s: \"$t$}S\"{x", verb );
	break;

    case CHANNEL_YELL:
	sprintf( bufI, "{s%ssz: \"%s{s\"{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "${s$n$}s %s: \"$t$}s\"{x", verb );
	break;

    case CHANNEL_CHAT:
	sprintf( bufI, "{.%ssz: \"%s{.\"{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "${.$n$}. %s: \"$t$}.\"{x", verb );
	break;

/*
    case CHANNEL_NCHAT:
	sprintf( bufI, "{M%ssz: \"%s{M\"{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "{M$n %s: \"$t{M\"{x", verb );
	break;
*/

    case CHANNEL_FLAME:
	sprintf( bufI, "{k%ssz: \"%s{k\"{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "${k$n$}k %s: \"$t$}k\"{x", verb );
	break;

    case CHANNEL_GRATS:
	sprintf( bufI, "{U%ssz %s{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "${U$n$}U %s $t{x", verb );
	break;

    case CHANNEL_HERO:
	sprintf( buf, "{c[${Y$n{c]{B:{x $t{x" );
	position        = ch->position;
	ch->position    = POS_STANDING;
	upper_window( ch );
	super_act( 0, ZM_ZAWSZE, buf, ch, argument, NULL, TO_CHAR, TRUE );
	lower_window( ch );
	ch->position    = position;
	break;

    case CHANNEL_IMMORTAL:
	if ( poziom > 103 )
	    sprintf( buf, "{y<${C$n{c:%d{y>{B:{x $t{x", poziom );
	else
	    sprintf( buf, "{y<${C$n{y>{B:{x $t{x" );
	position        = ch->position;
	ch->position    = POS_STANDING;
	upper_window( ch );
	super_act( 0, ZM_ZAWSZE, buf, ch, argument, NULL, TO_CHAR, TRUE );
	lower_window( ch );
	ch->position    = position;
	break;

    case CHANNEL_GOD:
	sprintf( buf, "{R-${G$n{R-{x $t{x" );
	position        = ch->position;
	ch->position    = POS_STANDING;
	upper_window( ch );
	super_act( 0, ZM_ZAWSZE, buf, ch, argument, NULL, TO_CHAR, TRUE );
	lower_window( ch );
	ch->position    = position;
	break;

    case CHANNEL_CLAN:
	sprintf( bufI, "{K%ssz: \"%s{K\"{x\n\r", verb, argument );
	bufI[ 2 ] = UPPER( bufI[ 2 ] );
	sprintf( buf, "${K$n$}K %s: \"$t$}K\"{x", verb );
	break;
    }

    if ( channel != CHANNEL_HERO && channel != CHANNEL_GOD
      && channel != CHANNEL_IMMORTAL )
    {
	upper_window( ch );
	send_to_char( bufI, ch );
	lower_window( ch );
    }

#if defined( IMUD )
    if ( channel == CHANNEL_IM && !imud_down )
    {
	/* Lam 8.5.98: wazne apostrofy! */
	sprintf( bufI, "imud '%s' %s\n", ch->short_descr, argument );
	write_to_imud_buffer( bufI ); /* Lam 25.4.98 */
	sprintf( bufI, "ircsay <%s> %s\n", ch->short_descr, argument );
	write_to_imud_buffer( bufI ); /* Lam 10.5.98 */
    }
#endif

    for ( d = descriptor_list; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( d->connected == CON_PLAYING
	  && vch != ch
	  && !IS_SET( och->deaf, channel )
	  && !IS_SET( och->in_room->room_flags, ROOM_CONE_OF_SILENCE )
	  && ( ( ( PP( och )
		|| skill_get_trust( och ) >= 106 )
	      && channel != CHANNEL_YELL
	      && channel != CHANNEL_MUSIC )
	    || ( WP( och )
	      && ( channel == CHANNEL_YELL
		|| channel == CHANNEL_MUSIC )
	      && CAN_HEAR( och ) ) ) )
	{
	    if ( get_trust( och ) < poziom )
		continue;
	    if ( channel == CHANNEL_HERO
	      && !is_authorized( get_char( och ), ":" ) )
		continue;
	    if ( channel == CHANNEL_IMMORTAL
	      && !is_authorized( get_char( och ), ">" ) )
		continue;
	    if ( ( channel == CHANNEL_YELL || channel == CHANNEL_MUSIC )
	      && ( vch->in_room->area != ch->in_room->area
		/* wymaga sluchu */
		|| !CAN_HEAR( vch ) ) )
		continue;
	    if ( channel == CHANNEL_CLAN
	      && !is_same_clan( get_char( och ), get_char( ch ) ) )
		continue;
	    if ( !IS_NPC( och ) && is_name2( ch->name, och->pcdata->ignore ) )
		continue;

	    upper_window( vch );
	    super_act( 0, ZM_ZAWSZE, buf, ch, argument, vch, vict_type, TRUE );
	    lower_window( vch );
	}
    }

    return;
}


void talk_auction( char *argument )
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *original;

    sprintf( buf, "{B<AUKCJA>:{x %s", argument );

    for ( d = descriptor_list; d; d = d->next )
    {
	original = d->original ? d->original : d->character; /* if switched */
	if ( ( d->connected == CON_PLAYING )
	  && !IS_SET( original->deaf, CHANNEL_AUCTION )
/*	  && CAN_HEAR( original ) */
	  && WP( original ) )
	    act( ZM_ZAWSZE, buf, original, NULL, NULL, TO_CHAR );
    }

    return;
}


KOMENDA( do_chat )
{
    const char *gadki[ 8 ] =
    {
	"nawija",
	"powiada",
	"stwierdza",
	"gaw`edzi",
	"przemawia",
	"o`swiadcza",
	"oznajmia",
	"nawija"
    };
    const char *pytanka[ 4 ] =
    {
	"pyta",
	"zapytuje",
	"docieka",
	"wypytuje"
/*
	"wnika"
*/
    };
    bool pytanko = FALSE;

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co chcesz powiedzie`c?\n\r", ch );
	return;
    }

    if ( czy_bluzg( argument ) )
    {
#if defined( MAM_AUTOKLATWE )
	if ( CZY_OPCJA( ch, OPC_AUTOKLATWA ) )
	    talk_channel( ch, argument, CHANNEL_FLAME | CHANNEL_NOIC, "klnie", "kl`a`c", 0 );
	else
	    STC( "Takie s`lowa s`a dopuszczalne na kanale KL`ATWA. Mo`ze u`zyj opcji AUTOKL`ATWA?\n\r", ch );
#else
	STC( "Takie s`lowa s`a dopuszczalne tylko na kanale KL`ATWA.\n\r", ch );
#endif
    }
    else
    {
	if ( strchr( argument, '?' ) )
	{
	    pytanko = TRUE;

	    if ( strchr( argument, '!' )
	      && strchr( argument, '!' ) < strchr( argument, '?' ) )
		pytanko = FALSE;

	    if ( strchr( argument, '.' )
	      && strchr( argument, '.' ) < strchr( argument, '?' ) )
		pytanko = FALSE;
	}

	talk_channel( ch, argument, CHANNEL_CHAT | CHANNEL_NOIC,
		pytanko ? pytanka[ number_bits( 2 ) ] : gadki[ number_bits( 3 ) ],
		"nawija`c", 0 );
    }

    return;
}


/*
 * kanaly Lama
 */

/* Lam 25.4.98: InterMud */
KOMENDA( do_imud )
{
    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co chcesz wys`la`c do IMa?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_IM, "imudzi", "imudzi`c", 0 );

    return;
}


KOMENDA( do_flame )
{
    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co klniesz?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_FLAME | CHANNEL_NOIC, "klnie", "kl`a`c", 0 );

    return;
}


KOMENDA( do_grats )
{
    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co gratulujesz?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_GRATS | CHANNEL_NOIC, "gratuluje", "gratulowa`c", 0 );

    return;
}


/*
 * Alander's new channels.
 */
KOMENDA( do_music )
{
    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co `spiewasz?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_MUSIC, "`spiewa", "`spiewa`c", 0 );

    return;
}


KOMENDA( do_shout )
{
    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co wrzeszczysz?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_SHOUT | CHANNEL_NOIC, "wrzeszczy", "wrzeszcze`c", 0 );
    WAIT_STATE( ch, 12 );

    return;
}


KOMENDA( do_yell )
{
    if ( !IS_NPC( ch ) && ch->level < 3 )
    {
	send_to_char( "Krzycze`c mo`zna od 3 poziomu, zdob`ad`x go i krzycz do woli.\n\r", ch );
	return;
    }

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co krzyczysz?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_YELL, "krzyczy", "krzycze`c", 0 );

    return;
}


KOMENDA( do_immtalk )
{
    if ( !authorized( get_char( ch ), ":" ) )
	return;

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co chcesz powiedzie`c bohaterom?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_HERO | CHANNEL_NOIC, "immtalk",
	"u`zywa`c kana`lu bohater`ow", 100 );

    return;
}


/*
 * Lam 18.4.2000
 * Lam 27.1.2004: opcjonalny poziom
 * Ulryk 31.1.2004: zapamietywanie domyslnego poziomu
 */
KOMENDA( do_qwertalk )
{
    CHAR_DATA *och = get_char( ch );
    int poziom = IS_NPC( och ) ? 103 : och->pcdata->niesmpoz;
    char arg[ MIL ];

    if ( !authorized( och, ">" ) )
	return;

    if ( get_trust( och ) > 103 )
    {
	one_argument( argument, arg );
	if ( is_number( arg ) )
	{
	    poziom = atoi( arg );
	    if ( poziom < 103 )
	    {
		STC( "Najni`zszy mo`zliwy poziom to 103.\n\r", ch );
		return;
	    }
	    if ( get_trust( och ) < poziom )
	    {
		STC( "Najwy`zszy mo`zliwy poziom to tw`oj poziom.\n\r", ch );
		return;
	    }
	    argument = one_argument( argument, arg );

	    if ( !( *argument ) && !IS_NPC( och ) )
	    {
		och->pcdata->niesmpoz = poziom;
		ch_printf( ch, "Zmieniasz poziom > na %d.\n\r", poziom );
		return;
	    }
	}
    }

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co chcesz powiedzie`c nie`smiertelnym?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_IMMORTAL | CHANNEL_NOIC, "qwertalk",
	"u`zywa`c kana`lu nie`smiertelnych", poziom );

    return;
}


/* Lam 17.8.98 */
KOMENDA( do_godtalk )
{
    if ( !authorized( get_char( ch ), "-" ) )
	return;

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co chcesz powiedzie`c bogom?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_GOD, "godtalk", "u`zywa`c kana`lu bog`ow", 106 );

    return;
}


KOMENDA( do_clan_chat )
{
    if ( !is_any_clan_member( ch ) )
    {
	send_to_char( "Najpierw musisz nale`ze`c do klanu.\n\r", ch );
	return;
    }

    if ( ch->pcdata->clan_member->level == CLAN_GUEST )
    {
	send_to_char( "Jeszcze nie nale`zysz do klanu.\n\r", ch );
	return;
    }

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co chcesz powiedzie`c klanowiczom?\n\r", ch );
	return;
    }

    talk_channel( ch, argument, CHANNEL_CLAN | CHANNEL_NOIC, "klanuje", "klanowa`c", 0 );

    return;
}


/*
 * Lam 3.4.98: ignoruj - ignorowanie wszelkich oznak aktywnosci oratorskiej
 * danej postaci (na zyczenie Suchego :))
 */
KOMENDA( do_ignore )
{
    char *imie, *list;
    char  imie1 [ MAX_INPUT_LENGTH ];
    char  tmp   [ MAX_INPUT_LENGTH ];
    bool  minus = FALSE;
    char  ignore[ MAX_STRING_LENGTH ];
    char  newign[ MAX_STRING_LENGTH ];
    int   licznik = 0;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Moby nikogo nie ignoruj`a.\n\r", ch );
	return;
    }

    if ( !*argument )
    {
	if ( *ch->pcdata->ignore )
	{
	    strcpy( newign, "Ignorujesz:\n\r" );
	    strcpy( ignore, ch->pcdata->ignore );
	    list = &ignore[ 0 ];
	    for ( ; ; )
	    {
		list = one_argument2( list, tmp );
		if ( !tmp[ 0 ] )
		    break;
		if ( licznik + strlen( tmp ) + 1 > 79 )
		{
		    strcat( newign, "\n\r" );
		    licznik = 0;
		}
		strcat( newign, tmp );
		strcat( newign, " " );
		licznik += strlen( tmp ) + 1;
	    }
	    strcat( newign, "\n\r" );
	    send_to_char( newign, ch );
	}
	else
	    send_to_char( "Nikogo nie ignorujesz.\n\r", ch );
	return;
    }

    argument = one_argument( argument, imie1 );
    imie = &imie1[ 0 ];
    if ( *imie == '-' )
    {
	imie++;
	minus = TRUE;
    }
    if ( !*imie )
    {
	one_argument( argument, imie1 );
	imie = &imie1[ 0 ];
    }

    if ( minus )
    {
	if ( !is_name2( imie, ch->pcdata->ignore ) )
	{
	    send_to_char( "Nikogo takiego nie ignorujesz.\n\r", ch );
	    return;
	}
	strcpy( ignore, ch->pcdata->ignore );
	strcpy( newign, "" );
	list = &ignore[ 0 ];
	for ( ; ; )
	{
	    list = one_argument( list, tmp );
	    if ( !tmp[ 0 ] )
		break;
	    if ( str_cmp( tmp, imie ) ) /* nie imie, wiec mozna przepisac */
	    {
		strcat( newign, tmp );
		strcat( newign, " " );
	    }
	}
	free_string( ch->pcdata->ignore );
	ch->pcdata->ignore = str_dup( newign );
    }
    else
    {
	if ( check_parse_name( imie, FALSE ) ) /* ograniczenie spacji, tyldy itd. */
	{
	    send_to_char( "Nie mo`zesz ignorowa`c kogo`s takiego.\n\r", ch );
	    return;
	}
	if ( !str_cmp( imie, ch->name ) )
	{
	    send_to_char( "Po prostu si`e nie odzywaj.\n\r", ch );
	    return;
	}
	if ( is_name2( imie, ch->pcdata->ignore ) )
	{
	    send_to_char( "Bardziej nie da si`e ju`z ignorowa`c jednej postaci.\n\r", ch );
	    return;
	}
	if ( strlen( ch->pcdata->ignore ) + strlen( imie ) > 1000 )
	{
	    send_to_char( "Wi`ecej postaci nie mo`zesz ignorowa`c.\n\r", ch );
	    return;
	}
	strcpy( ignore, ch->pcdata->ignore );
	strcat( ignore, imie );
	strcat( ignore, " " );
	free_string( ch->pcdata->ignore );
	ch->pcdata->ignore = str_dup( ignore );
    }

    do_ignore( ch, "" ); /* pokaze liste */

    return;
}


KOMENDA( do_say )
{
    /* Vigud, 24.1.2013: bez tej zmiany, gadanie mobow (z randprogow) zajmuje
       30% czasu procesora przeznaczonego na Laca. Spowodowane jest to przede
       wszystkim sprawdzaniem poprawnosci kazdego wypowiedzianego slowa przez
       funkcje miodek(). Doszlismy do wniosku, ze nie ma sensu, zeby moby
       gadaly, jesli nikt tego nie widzi i nie ma proga, ktory moglby zareagowac
       na to gadanie. */
    if ( IS_NPC( ch )
      && !IS_SET( ch->in_room->progtypes, R_SPEECH_PROG ) )
    {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch == ch )
		continue;

	    if ( !IS_NPC( vch ) )
		break;

	    if ( IS_SET( vch->pIndexData->progtypes, M_SPEECH_PROG ) )
		break;
	}

	if ( !vch )
	    return;
    }

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co m`owisz?\n\r", ch );
	return;
    }

    /* Lam 20.12.2002: jednak uciszone postacie nie beda mogly mowic */
    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE )
      || ( !IS_NPC( ch ) && ( IS_SET( ch->act, PLR_SILENCE )
			   || IS_SET( ch->act, PLR_NO_TELL ) ) ) )
    {
	send_to_char( "Ruszasz wargami, ale nie s`lycha`c d`xwi`eku.\n\r", ch );
	return;
    }

    if ( miodek( ch, argument ) )
	return;

    argument = makedrunk( argument, ch );

    act( ZM_ZAWSZE, "{qM`owisz: \"$T{q\"{x", ch, NULL, argument, TO_CHAR );
    act( ZM_SLUCH, "${q$n$}q m`owi: \"$T$}q\"{x", ch, NULL, argument, TO_ROOM );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_SLUCH, "{q$n porusza ustami.{x", ch, NULL, NULL, TO_ROOM );

    if ( !IS_NPC( ch ) )
    {
	CHAR_DATA *papuga;
	for ( papuga = ch->in_room->people;
	      papuga;
	      papuga = papuga->next_in_room )

	    if ( papuga->spec_fun == spec_lookup( "spec_parrot" )
	      && papuga->race == zr_papuga
	      && !number_bits( 2 )
	      && papuga != ch )
		papuguj( papuga, argument );
    }

    rprog_speech_trigger( argument, ch );

    if ( ch->deleted )
	return;

    mprog_speech_trigger( ch, argument, NULL );

    return;
}


void sekretarka_ook_ost( CHAR_DATA *ch, CHAR_DATA *victim, char *argument )
{
    if ( IS_NPC( victim ) )
	return;

    if ( IS_SET( victim->act, PLR_AFK ) )
    {
	TELL_DATA *tell;
	TELL_DATA *temp;

	super_act( 0, ZM_ZAWSZE, "$N jest nieobecn$Y. Informacja zosta`la zapami`etana.", ch, NULL, victim, TO_CHAR | ACT_TELEP, TRUE );
	tell = new_tell();
	tell->tell = str_dup( argument );

	if ( !victim->tell )
	{
	    tell->next = victim->tell;
	    victim->tell = tell;
	}
	else
	{
	    for ( temp = victim->tell; temp; temp = temp->next )
		if ( !temp->next ) break;
	    temp->next = tell;
	    tell->next = NULL;
	}
    }

    if ( victim->pcdata->ostatnie[ victim->pcdata->ost_end ] )
	free_string( victim->pcdata->ostatnie[ victim->pcdata->ost_end ] );

    victim->pcdata->ostatnie[ victim->pcdata->ost_end++ % MAX_OSTATNIE ] = str_dup( argument );

    if ( victim->pcdata->ost_end % MAX_OSTATNIE == 0 )
	victim->pcdata->ost_end = 0;

    return;
}


#define TELEPATIA	0
#define SZEPT		1
#define MOWA		2

void real_tell( CHAR_DATA *ch, CHAR_DATA *victim, char *argument,
		bool can_be_ignored, int tell_type )
{
    CHAR_DATA *ucho;
    char buf[ MSL ];
    int position;
    char *orzeczenie, *okolicznik;
    char kolorek;

    if ( ( !IS_NPC( ch ) && ( IS_SET( ch->act, PLR_SILENCE )
			   || IS_SET( ch->act, PLR_NO_TELL ) ) )
	|| ( skill_get_trust( ch ) < 106 && skill_get_trust( victim ) < 106
	  && ( ( WP( ch ) && PP( victim ) )
	    || ( PP( ch ) && WP( victim ) ) ) ) )
    {
	send_to_char( "Twoja informacja nie dotar`la.\n\r", ch );
	return;
    }

    if ( can_be_ignored
      && !IS_NPC( victim )
      && is_name2( ch->name, victim->pcdata->ignore ) )
    {
	ch_printf( ch, "%s nie chce ci`e s`lysze`c.\n\r", victim->short_descr );
	return;
    }

    if ( !IS_NPC( victim ) && !victim->desc )
    {
	super_act( 0, ZM_ZAWSZE, "$N straci`l$A po`l`aczenie, nie trud`x si`e wi`ec.",
		ch, NULL, victim,
		TO_CHAR | ( tell_type == TELEPATIA ? ACT_TELEP : 0 ), TRUE );
	return;
    }

    if ( ( IS_NPC( ch ) || !IS_IMMORTAL( ch ) ) && !IS_AWAKE( victim ) )
    {
	super_act( 0, ZM_ZAWSZE, "$Z nie mo`ze ci`e us`lysze`c.",
		ch, NULL, victim,
		TO_CHAR | ( tell_type == TELEPATIA ? ACT_TELEP : 0 ), TRUE );
	return;
    }

    if ( miodek( ch, argument ) )
	return;

    if ( tell_type != TELEPATIA )
	argument = makedrunk( argument, ch );

    if ( ch == victim )
    {
	/* zawsze myslenie, po co ma ktos podsluchiwac pomylek */
	super_act( 0, ZM_ZAWSZE, "{TMy`slisz sobie: \"$t{T\"{x", ch, argument, NULL, TO_CHAR, TRUE );
	return; /* bez ostatnich, odpowiadania, ponawiania */
    }

    if ( tell_type == SZEPT )
    {
	kolorek = 't';
	orzeczenie = "szepcze";
	okolicznik = "";

	/* Lam 20.4.98: podsluchiwanie */
	for ( ucho = ch->in_room->people; ucho; ucho = ucho->next_in_room )
	{
	    if ( ucho->deleted
	      || ucho == ch
	      || ucho == victim
	      || !IS_AWAKE( ucho ) )
	    {
		continue;
	    }

	    if ( IS_NPC( ucho )
	      || ucho->pcdata->learned[ gsn_overhear ] < number_percent( ) )
	    {
		sprintf( buf, "${t%s$}t szepcze co`s %s%s.{x",
		    PERS( ch, ucho, FALSE ),
		    IS_SET( race_table[ victim->race ].parts, P_USZY ) ? "na ucho " : "",
		    PERSC( victim, ucho, FALSE ) );

		act( ZM_ZAWSZE, buf, ch, NULL, ucho, TO_VICT );
	    }
	    else
	    {
		sprintf( buf, "{tPods`luchujesz jak ${t%s$}t szepcze %s: \"%s{t\"{x",
		    PERS( ch, ucho, FALSE ),
		    PERSC( victim, ucho, FALSE ),
		    argument );

		act( ZM_ZAWSZE, buf, ch, NULL, ucho, TO_VICT );
		uzyj( ucho, gsn_overhear, "pods`luchiwaniu szept`ow" );
	    }
	}
    }
    else
    {
	kolorek = ( tell_type == TELEPATIA ) ? 'T' : 'q';
	orzeczenie = "m`owi";
	okolicznik = ( tell_type == TELEPATIA && ( WP( ch ) || WP( victim ) ) )
			? " telepatycznie" : "";
    }

    /* komunikat dla ch */
    sprintf( buf, "{%c%ssz $@%s: \"$t{%c\"{x", kolorek, orzeczenie,
		okolicznik, kolorek );
    buf[ 2 ] = UPPER( buf[ 2 ] );
    super_act( 0, ZM_ZAWSZE, buf, ch, argument, victim,
	TO_CHAR | ( tell_type == TELEPATIA ? ACT_TELEP : 0 ), TRUE );

    /* komunikat dla ofiary */
    position = victim->position;
    victim->position = POS_STANDING;
    upper_window( victim );
    sprintf( buf, "${%c$n$}%c %s ci%s: \"$t$}%c\"{x", kolorek, kolorek, orzeczenie,
		okolicznik, kolorek );
    super_act( 0, tell_type == TELEPATIA ? ZM_ZAWSZE : ZM_SLUCH,
	buf, ch, argument, victim,
	TO_VICT | ( tell_type == TELEPATIA ? ACT_TELEP : 0 ), TRUE );
    lower_window( victim );
    victim->position = position;

    /* ewentualny komunikat dla innych, szept obslugiwany wyzej, tu mowa */
    if ( tell_type == MOWA )
    {
	sprintf( buf, "${%c$n$}%c %s $@%s: \"$t$}%c\"{x", kolorek, kolorek, orzeczenie,
		okolicznik, kolorek );
	/* moze zechcemy, zeby dzialalo tez PP */
	super_act( 0, ZM_SLUCH, buf, ch, argument, victim, TO_NOTVICT, TRUE );
    }

    if ( victim->reply )
	wywal_zwod( &victim->reply->zwod_reply, victim );
    victim->reply = ch;
    dod_zwod( &ch->zwod_reply, victim );
    victim->reply_type = tell_type;

    if ( ch->retell )
	wywal_zwod( &ch->retell->zwod_retell, ch );
    ch->retell = victim;
    dod_zwod( &victim->zwod_retell, ch );
    ch->retell_type = tell_type;

    if ( IS_NPC( victim ) )
    {
	mprog_speech_trigger( ch, argument, victim );
	return;
    }

    sprintf( buf, "{%c%s %s ci%s: \"%s{%c\"{x\n\r", kolorek,
		PERS( ch, victim, tell_type == TELEPATIA ), orzeczenie,
		okolicznik, argument, kolorek );
    buf[ 2 ] = UPPER( buf[ 2 ] );
    sekretarka_ook_ost( ch, victim, buf );

    return;
}


KOMENDA( do_tell )
{
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );

    if ( !*arg || !strlen_pl_kol( argument ) )
    {
	send_to_char( "Powiedzie`c komu co?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     * Lam: get_char_world_pc najpierw szuka graczy, a potem ew. moby
     */
    if ( !( victim = get_char_world_pc_tele( ch, arg ) )
	|| ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && WP( ch )
      && ch->pcdata->learned[ gsn_telepathy ] < number_percent( ) )
    {
	send_to_char( "Nie uda`lo ci si`e porozumie`c telepatycznie.\n\r", ch );
	WAIT_STATE( ch, 4 );
	return;
    }

    real_tell( ch, victim, argument, TRUE, TELEPATIA );

    return;
}


KOMENDA( do_mowdo )
{
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );

    if ( !*arg || !strlen_pl_kol( argument ) )
    {
	send_to_char( "Powiedzie`c komu co?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_MUTE( ch )
	|| IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "Ruszasz wargami, ale niczego nie s`lycha`c.\n\r", ch );
	return;
    }

    /* tak jak zwykla mowa, to jest na glos i ignorowac mozna tylko ucieczka */
    real_tell( ch, victim, argument, FALSE, MOWA );

    return;
}


/*
 * Lam 4.4.98: wysyla wiadomosc do tej samej postaci, do ktorej ostatnio
 * wysylano wiadomosc za pomoca innych polecen, starajac sie uzyc tego samego
 * sposobu komunikacji
 */
KOMENDA( do_retell )
{
    CHAR_DATA *victim;

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co powiedzie`c?\n\r", ch );
	return;
    }

    /* tu zmiana z szukania po imieniu na branie z ch->retell */
    if ( !( victim = ch->retell )
      || ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    /* zmiana szeptow/mowy na telepatie w przypadku pojscia PP, w przypadku
       wyjscia z pomieszczenia, a takze znikniecia rozmowcy */
    if ( PP( ch ) || PP( victim )
      || victim->in_room != ch->in_room
      || !can_see( ch, victim ) )
	ch->retell_type = TELEPATIA;

    if ( ch->retell_type != TELEPATIA
      && ( IS_MUTE( ch )
	|| IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) ) )
    {
	send_to_char( "Ruszasz wargami, ale niczego nie s`lycha`c.\n\r", ch );
	return;
    }

    if ( ch->retell_type == TELEPATIA && !IS_NPC( ch ) && WP( ch )
      && ch->pcdata->learned[ gsn_telepathy ] < number_percent( ) )
    {
	send_to_char( "Nie uda`lo ci si`e porozumie`c telepatycznie.\n\r", ch );
	WAIT_STATE( ch, 4 );
	return;
    }

    real_tell( ch, victim, argument, FALSE, ch->retell_type );

    return;
}


KOMENDA( do_reply )
{
    CHAR_DATA *victim;

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co odpowiedzie`c?\n\r", ch );
	return;
    }

    /* tu zmiana z szukania po imieniu na branie z ch->reply */
    if ( !( victim = ch->reply )
	|| ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    /* zmiana szeptow/mowy na telepatie w przypadku pojscia PP i w przypadku
       wyjscia z pomieszczenia */
    if ( PP( ch ) || PP( victim ) || victim->in_room != ch->in_room )
	ch->reply_type = TELEPATIA;

    if ( ch->reply_type != TELEPATIA
      && ( IS_MUTE( ch )
	|| IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) ) )
    {
	send_to_char( "Ruszasz wargami, ale niczego nie s`lycha`c.\n\r", ch );
	return;
    }

    if ( ch->reply_type == TELEPATIA && !IS_NPC( ch ) && WP( ch )
      && ch->pcdata->learned[ gsn_telepathy ] < number_percent( ) )
    {
	send_to_char( "Nie uda`lo ci si`e porozumie`c telepatycznie.\n\r", ch );
	WAIT_STATE( ch, 4 );
	return;
    }

    real_tell( ch, victim, argument, FALSE, ch->reply_type );

    return;
}


/* Lam 20.4.98: szept w pomieszczeniu, bez cwiczenia telepatii */
KOMENDA( do_whisper )
{
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );

    if ( !*arg || !strlen_pl_kol( argument ) )
    {
	send_to_char( "Szepn`a`c komu co?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "Ruszasz wargami, ale niczego nie s`lycha`c.\n\r", ch );
	return;
    }

    real_tell( ch, victim, argument, TRUE, SZEPT );

    return;
}


KOMENDA( do_emote )
{
    char buf[ MAX_STRING_LENGTH ];

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
    {
	send_to_char( "Nie mo`zesz tego robi`c!\n\r", ch );
	return;
    }

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co robisz?\n\r", ch );
	return;
    }

    strcpy( buf, argument );
    if ( isalpha( (unsigned char) buf[ strlen( buf ) - 1 ] ) )
	strcat( buf, "." );

    /* tu dylemat oczywisty - mozna uzywac emocji do opisywania wszystkiego,
       odglosow, zapachow i wszelkich dziwnych odczuc, ale nie da sie podac
       kilku wersji dla odbiorcow roznych zmyslow. Przyjmuje, ze emocje
       opisuja czynnosci widzialne, odbieralne wzrokiem */
    act( ZM_WZROK | ZM_WID_CHAR, "$n $T{x", ch, NULL, buf, TO_ROOM );
    act( ZM_ZAWSZE, "$n $T{x", ch, NULL, buf, TO_CHAR );

    return;
}


/*
 * Lam 6.11.2000: pozy w plikach profesji (katalog class)
 * TODO: FIXME: dostepnosc poz na poszczegolnych poziomach (niektore pozy
 * swiadcza o potedze i powinny byc dostepne od wyzszych poziomow)
 */
KOMENDA( do_pose )
{
    int pose;

    if ( IS_NPC( ch ) )
	return;

    if ( !pose_count[ ch->klasa ] )
    {
	ASTC( "Rozgl`adasz si`e bezradnie.", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rozgl`ada si`e bezradnie.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    pose = number_range( 0, pose_count[ ch->klasa ] - 1 );

    act( ZM_ZAWSZE, pose_table[ ch->klasa ][ pose ].to_char, ch, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, pose_table[ ch->klasa ][ pose ].to_room, ch, NULL, NULL, TO_ROOM );

    return;
}


struct bledomat_data bledomat;

#if defined( BLEDOMAT )
#define BLAD_SKLADNI_BLEDOMATU { log_string( "Blad skladni " BLEDOMAT_FILE ". Bledomat wylaczony." ); fclose( fp ); return; }

void wczytaj_bledomat( void )
{
    FILE *fp;
    char *klucz;
    int status;

    bledomat.wlaczony = FALSE;
    if ( !( fp = fopen( BLEDOMAT_FILE, "r" ) ) )
    {
	log_string( "Brak pliku " BLEDOMAT_FILE ". Bledomat wylaczony." );
	return;
    }

    klucz = fread_word( fp, &status, TRUE );
    if ( status || str_cmp( klucz, "Adres" ) )
	BLAD_SKLADNI_BLEDOMATU
    bledomat.adres = fread_string( fp, &status );
    if ( status )
	BLAD_SKLADNI_BLEDOMATU

    klucz = fread_word( fp, &status, TRUE );
    if ( status || str_cmp( klucz, "Uzytkownik" ) )
	BLAD_SKLADNI_BLEDOMATU
    bledomat.uzytkownik = fread_string( fp, &status );
    if ( status )
	BLAD_SKLADNI_BLEDOMATU

    klucz = fread_word( fp, &status, TRUE );
    if ( status || str_cmp( klucz, "Haslo" ) )
	BLAD_SKLADNI_BLEDOMATU
    bledomat.haslo = fread_string( fp, &status );
    if ( status )
	BLAD_SKLADNI_BLEDOMATU

    klucz = fread_word( fp, &status, TRUE );
    if ( status || str_cmp( klucz, "Baza" ) )
	BLAD_SKLADNI_BLEDOMATU
    bledomat.baza = fread_string( fp, &status );
    if ( status )
	BLAD_SKLADNI_BLEDOMATU

    klucz = fread_word( fp, &status, TRUE );
    if ( status || str_cmp( klucz, "Przedrostek" ) )
	BLAD_SKLADNI_BLEDOMATU
    bledomat.przedrostek = fread_string( fp, &status );
    if ( status )
	BLAD_SKLADNI_BLEDOMATU

    klucz = fread_word( fp, &status, TRUE );
    if ( status || str_cmp( klucz, "Bledy" ) )
	BLAD_SKLADNI_BLEDOMATU
    bledomat.bledy = fread_number( fp, &status );
    if ( status )
	BLAD_SKLADNI_BLEDOMATU

    klucz = fread_word( fp, &status, TRUE );
    if ( status || str_cmp( klucz, "Literowki" ) )
	BLAD_SKLADNI_BLEDOMATU
    bledomat.literowki = fread_number( fp, &status );
    if ( status )
	BLAD_SKLADNI_BLEDOMATU

    klucz = fread_word( fp, &status, TRUE );
    if ( status || str_cmp( klucz, "Pomysly" ) )
	BLAD_SKLADNI_BLEDOMATU
    bledomat.pomysly = fread_number( fp, &status );
    if ( status )
	BLAD_SKLADNI_BLEDOMATU

    bledomat.wlaczony = TRUE;
    fclose( fp );

    return;
}


/* Lam 22.1.2005 */
bool zglos_do_bazy( CHAR_DATA *ch, int kategoria, char *argument )
{
    MYSQL *baza = mysql_init( NULL );
    char buf[ MSL ];
    char bufarg[ MSL ];
    static CHAR_DATA pustak;

    if ( !mysql_real_connect( baza, bledomat.adres, bledomat.uzytkownik, bledomat.haslo, bledomat.baza, 0, NULL, 0 ) )
    {
	sprintf( bug_buf, "Blad laczenia z baza: %s", mysql_error( baza ) );
	bug( bug_buf, 0 );
	mysql_close( baza );
	return FALSE;
    }

    strcpy( buf, argument );
    pustak.polskie = 2;
    pustak.kolor = 0;
    colourconv( bufarg, lac2html( buf, TRUE ), &pustak, sizeof( bufarg ) );
    sprintf( buf, "INSERT INTO bledomat_bledy "
	"( id, katid, tresc, stan, alarm, ip, autor, przypisany, data, datamod, vnum ) "
	"VALUES ( 0, %d, '%s', 1, 0, '(Lac) %s', '%s', '', %d, %d, %d )",
	kategoria, bufarg,
	ch->desc ? ch->desc->ipaddr : "(brak)", ch->name,
	(int) current_time, (int) current_time, ch->in_room->vnum );

    if ( mysql_query( baza, buf ) )
    {
	sprintf( bug_buf, "Blad wstawienia bledu do bazy: %s", mysql_error( baza ) );
	bug( bug_buf, 0 );
	mysql_close( baza );
	return FALSE;
    }

    sprintf( buf, "%s%d", bledomat.przedrostek, (int) mysql_insert_id( baza ) );
    ch_printf( ch, "Je`sli chcesz skomentowa`c b`l`ad lub sprawdzi`c post`epy, odwied`x:\n\r%s\n\r",
	buf );
    wiznet( buf, ch, NULL, NULL, WIZ_IDEA, 0, 0 );

    mysql_close( baza );

    return TRUE;
}
#endif /* defined( BLEDOMAT ) */


void zglos_blad( CHAR_DATA *ch, char *opis, char *dopeln, char *plik,
		int kategoria, char *argument )
{
    char buf[ MSL ];
    static char poprzedni[ MSL ];
#if defined( BLEDOMAT )
    static time_t kiedy;

    /* nie jest to zbyt rozsadne, ale nie zdarzylo sie jeszcze, aby dwoch
       graczy zglaszalo bledy tak szybko :) */
    if ( kiedy + 30 > current_time )
    {
	STC( "Pami`etaj, dopiski do b`l`ed`ow musisz zg`lasza`c przez b`l`edomat,\n\r"
		"kt`ory znajduje si`e pod adresem http://bledomat.lac.pl/\n\r"
		"Je`sli chcesz zg`losi`c inny b`l`ad, po prostu odczekaj jeszcze chwilk`e.\n\r", ch );
	return;
    }
#endif

    if ( strlen( argument ) < 20 )
    {
	ch_printf( ch, "Opis %s jest zbyt kr`otki. Je`sli twoja pr`oba zg`loszenia %s by`la\n\r"
		"pomy`lk`a, zignoruj ten komunikat. Je`sli jednak faktycznie chcesz zg`losi`c\n\r"
		"%s, napisz wi`ecej informacji, aby`smy dok`ladnie wiedzieli, o co chodzi.\n\r",
		dopeln, dopeln, opis );
	return;
    }

    if ( !str_cmp( poprzedni, argument ) )
    {
	STC( "Ju`z o tym wiemy, ale dzi`eki jeszcze raz!\n\r", ch );
	return;
    }

#if defined( BLEDOMAT )
    kiedy = current_time;
#endif
    strcpy( poprzedni, argument );

    sprintf( buf, "{M$N{R zg`lasza %s:{x $t", opis );
    wiznet( buf, ch, (OBJ_DATA *) argument, NULL, WIZ_IDEA, 0, 0 );
    sprintf( buf, "{RZg`laszasz %s{x:\n\r%s\n\r", opis, argument );
    send_to_char( buf, ch );

#if defined( BLEDOMAT )
    if ( !bledomat.wlaczony || !zglos_do_bazy( ch, kategoria, argument ) )
#endif
	append_file( ch, plik, argument );

    return;
}


KOMENDA( do_bug )
{
    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Widzisz jakie`s b`l`edy?\n\r", ch );
	return;
    }

    zglos_blad( ch, "b`l`ad", "b`l`edu", PCBUG_FILE, bledomat.bledy, argument );

    return;
}


KOMENDA( do_idea )
{
    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Masz jaki`s pomys`l?\n\r", ch );
	return;
    }

    zglos_blad( ch, "pomys`l", "pomys`lu", IDEA_FILE, bledomat.pomysly, argument );

    return;
}


KOMENDA( do_typo )
{
    if ( !strlen_pl_kol( argument ) )
    {
	ASTC( "Dostrzeg`l$o`s liter`owk`e?", ch );
	return;
    }

    zglos_blad( ch, "liter`owk`e", "liter`owki", TYPO_FILE, bledomat.literowki, argument );

    return;
}


KOMENDA( do_rent )
{
    send_to_char( "W tym mudzie nie ma wynajmowania pomieszcze`n.\n\rJest za to przechowalnia.\n\r", ch );

    return;
}


KOMENDA( do_qui )
{
    send_to_char( "Je`sli chcesz wyj`s`c, napisz WYJD`X.\n\r", ch );

    return;
}


/*
 * Lam 31.3.98: uzywane przez "wyjdz" i "usun": sprawdza, czy gracz moze wyjsc
 * z gry
 */
bool can_quit_now( CHAR_DATA *ch, bool fCicho )
{
    if ( ch->position == POS_FIGHTING )
    {
	if ( !fCicho ) send_to_char( "Daj spok`oj, teraz, w trakcie walki?\n\r", ch );
	return FALSE;
    }

    if ( ch->position <= POS_STUNNED )
    {
	if ( !fCicho ) ASTC( "Czekaj, jeszcze nie umar`l$o`s...", ch );
	return FALSE;
    }

    if ( ( auction->item ) && ( ( ch == auction->buyer ) || ( ch == auction->seller ) ) )
    {
	if ( !fCicho ) send_to_char( "{BZaczekaj na zako`nczenie aukcji.{x\n\r", ch );
	return FALSE;
    }

    if ( ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_DENY ) )
      && ch->timer < 40 && ch->pnote && PP( ch ) )
    {
	if ( ch->sex == 2 )
	{
	    if ( !fCicho ) STC( "Rozpocz`e`la`s pisanie noty. Je`sli chcesz wyj`s`c nie wysy`laj`ac jej,\n\r"
		"wyczy`s`c not`e pisz`ac \"nota czy`s`c\", je`sli chcesz j`a wys`la`c, zr`ob to\n\r"
		"przed wyj`sciem z gry.\n\r", ch );
	}
	else
	{
	    if ( !fCicho ) STC( "Rozpocz`a`le`s pisanie noty. Je`sli chcesz wyj`s`c nie wysy`laj`ac jej,\n\r"
		"wyczy`s`c not`e pisz`ac \"nota czy`s`c\", je`sli chcesz j`a wys`la`c, zr`ob to\n\r"
		"przed wyj`sciem z gry.\n\r", ch );
	}
	return FALSE;
    }

    if ( !IS_NPC( ch )
      && !IS_SET( ch->act, PLR_DENY )
      && !IS_IMMORTAL( ch )
      && ch->pcdata->odwleczenie )
    {
	if ( !fCicho )
	{
	    int sekundy;
	    int l;

	    sekundy = ( ch->pcdata->odwleczenie + PULSE_PER_SECOND - 1 )
		      / PULSE_PER_SECOND;
	    l = liczba( sekundy );
	    ch_printf( ch, "Zaczekaj na wyj`scie jeszcze %d sekund%s.\n\r",
		sekundy, l == 1 ? "`e" : l == 2 ? "y" : "" );
	}
	return FALSE;
    }

    return TRUE;
}


KOMENDA( do_wciel )
{
    CHAR_DATA *rch;

    if ( IS_NPC( ch ) )
    {
	STC( "No co ty? Wciel si`e w swoj`a rol`e moba.\n\r", ch );
	return;
    }

    if ( WP( ch ) )
    {
	STC( "Bardziej wcieli`c si`e nie mo`zesz.\n\r", ch );
	return;
    }

    STC( "Wcielasz si`e w sw`a posta`c.\n\r", ch );
    if ( ch->pnote )
	STC( "Pami`etaj, `ze piszesz not`e, mo`ze sko`ncz j`a przed rozpocz`eciem gry?\n\r", ch );
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch->deleted || rch == ch )
	    continue;

	if ( !IS_NPC( rch )
	    && CZY_OPCJA( rch, OPC_WCHODZENIE )
	    && ( !IS_SET( ch->act, PLR_WIZINVIS )
		|| get_trust( rch ) >= ch->pcdata->wizinvis ) )
	    act_pp( ZM_ZAWSZE, "$n wciela si`e w posta`c.", ch, NULL, rch, TO_VICT );
    }

    if ( !ch->pcdata->ic_position )
    {
	sprintf( log_buf, "do_wciel: ic_position == POS_DEAD, ch: %s", ch->name );
	bug( log_buf, 0 );
    }
    else
	ch->position = ch->pcdata->ic_position;

    char_from_room( ch );
    if ( !ch->pcdata->ic_in_room )
    {
	sprintf( log_buf, "do_wciel: ic_in_room == NULL, ch: %s", ch->name );
	bug( log_buf, 0 );
	char_to_room( ch, get_room_index( ch->pcdata->strefa->swiatynia ) );
    }
    else if ( ch->pcdata->ic_in_room == ch->in_room )
    {
	sprintf( log_buf, "do_wciel: ic_in_room == in_room, ch: %s", ch->name );
	bug( log_buf, 0 );
	char_to_room( ch, get_room_index( ch->pcdata->strefa->swiatynia ) );
    }
    else
	char_to_room( ch, ch->pcdata->ic_in_room );

    if ( IS_SET( ch->act, PLR_WIZINVIS ) )
	act( ZM_WZROK | ZM_WID_CHAR, "{B### (BN) $n wchodzi do gry.{x", ch, NULL, NULL, TO_ROOM );
    else
	act( ZM_WZROK | ZM_WID_CHAR, "{B### $n wchodzi do gry.{x", ch, NULL, NULL, TO_ROOM );

    if ( !IS_AWAKE( ch ) || !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_BLIND ) )
	do_look( ch, "" );

    ch->pcdata->odwleczenie = ODWLECZENIE;

    return;
}


KOMENDA( do_quit )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA       *rch;
#if defined( IMUD )
    char imbuf[ 200 ];
#endif

    if ( IS_NPC( ch ) )
    {
	STC( "Mo`ze \"wr`o`c\"?\n\r", ch );
	return;
    }

    if ( !can_quit_now( ch, FALSE ) )
	return;

    if ( WP( ch ) && str_cmp( argument, "zupe`lnie" ) )
    {
	if ( ch->in_room->vnum != ROOM_VNUM_LIMBO )
	    act( ZM_WZROK | ZM_WID_CHAR, "{G### $n opu`sci`l$9 ten `swiat.{x", ch, NULL, NULL, TO_ROOM );
	ch->pcdata->ic_in_room = ch->in_room;
	ch->pcdata->ic_position = ch->position;

	zabij_wszystkie_slugi( ch );
	affect_strip( ch, gsn_kontrola_umyslu );
	die_follower( ch );
	PRZERWIJ_KOPANIE( ch, FALSE );
	PRZERWIJ_KARMPOJ( ch, FALSE );
	/* Lam 1.12.2004: znikanie kluczy podczas wyjscia z postaci */
	wywal_smieci( ch );

	char_from_room( ch );
	char_to_room( ch, get_room_index( ROOM_VNUM_POZA_SWIATEM ) );
	for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	{
	    if ( rch->deleted || rch == ch )
		continue;

	    if ( !IS_NPC( rch )
		&& CZY_OPCJA( rch, OPC_WCHODZENIE )
		&& ( !IS_SET( ch->act, PLR_WIZINVIS )
		    || get_trust( rch ) >= ch->pcdata->wizinvis ) )
		act_pp( ZM_ZAWSZE, "$n powraca z gry sw`a postaci`a.", ch, NULL, rch, TO_VICT );
	}
	ch->position = POS_STANDING;
/*	REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_BLIND ); */
	do_look( ch, "" );
	return;
    }

    if ( CZY_OPCJA( ch, OPC_MIW ) ) /* przywrocenie przewijania */
	send_to_char( ESCS "7" ESCS "[0;25r" ESCS "8", ch );

    if ( !merc_down ) /* przeciwko powodziom przy jebucie */
    {
	if ( ( ch->in_room->vnum != ROOM_VNUM_LIMBO )
	  && WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR, "{G### $n powr`oci`l$9 do \"realnego\" `swiata.{x", ch, NULL, NULL, TO_ROOM );
	else if ( PP( ch ) )
	    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	    {
		if ( rch->deleted || rch == ch )
		    continue;

		if ( !IS_NPC( rch )
		    && CZY_OPCJA( rch, OPC_WCHODZENIE )
		    && ( get_trust( ch ) < LEVEL_IMMORTAL
		      || get_trust( rch ) >= get_trust( ch ) ) )
		    act_pp( ZM_ZAWSZE, "$n opuszcza gr`e.", ch, NULL, rch, TO_VICT );
	    }

	wiznet( "{M$N{x po`l`aczy`l$7 si`e z rzeczywisto`sci`a.",
		ch, NULL, NULL, WIZ_LOGIN, 0,
		( get_trust( ch ) > 108 ) ? 108 : get_trust( ch ) );
			/* 108/109/110 to rowne poziomy */
#if defined( IMUD )
	if ( !imud_down )
	{
	    sprintf( imbuf, "player_off '%s'\n", ch->short_descr );
	    write_to_imud_buffer( imbuf );
	}
#endif
    }

    if ( ch->desc )
    {
	sprintf( log_buf, "%s opuszcza gre (pol. %d).", ch->name, ch->desc->descriptor );
	log_string( log_buf );
    }
    else
    {
	sprintf( log_buf, "%s opuszcza gre (brak pol.)", ch->name );
	log_string( log_buf );

	/* Lam 26.10.2008: nauka prawidlowego wychodzenia.
	   Musi byc tu, przed save_char_obj( ). */
	if ( WP( ch ) )
	    SET_BIT( ch->act, PLR_UZYJ_WYJDZ );
    }

    zabij_wszystkie_slugi( ch );
    die_follower( ch );
    save_char_obj( ch );
    if ( ch->pcdata->clan )
    {
	ch->pcdata->clan_member->seen = current_time;
	ch->pcdata->clan_member->ch = NULL;
    }

    /* Lam 26.3.2000: losowe komunikaty */
    /* Lam 17.6.2003: nie pokazuja sie wyrzuconym postaciom */
    if ( !IS_SET( ch->act, PLR_DENY ) )
    {
	STC( quit_table[ number_range( 0, quit_count ) ], ch );
	STC( "\n\r", ch ); /* odstep przed Connection closed */
    }

    /* Lam 6.11.2000: liczniki
       niestety czas jest liczony od wejscia do gry, a liczniki bajtow/linii
       sa liczone od polaczenia. Po utracie polaczenia i ponownym wejsciu
       liczniki bajtow licza od zera, a czas od wejscia postaci. FIXME */
    if ( ch->desc )
    {
	int licz_zn, licz_ln;
	char czasomierz[ MIL ];
	int sek = current_time - ch->logon;
	int godz, min;

	strcpy( czasomierz, "Opuszczasz Laca po " );
	godz = sek / 3600;
	sek %= 3600;
	min = sek / 60;
	sek %= 60;
	if ( godz )
	    sprintf( czasomierz + strlen( czasomierz ), "%d godzin%s",
		godz, liczba( godz ) == 1 ? "ie" : "ach" );
	if ( godz && min && !sek )
	    strcat( czasomierz, " i " );
	if ( godz && min && sek )
	    strcat( czasomierz, ", " );
	if ( min )
	    sprintf( czasomierz + strlen( czasomierz ), "%d minu%s",
		min, liczba( min ) == 1 ? "cie" : "tach" );
	if ( sek )
	{
	    if ( min || godz )
		strcat( czasomierz, " i " );
	    sprintf( czasomierz + strlen( czasomierz ), "%d sekund%s",
		sek, liczba( sek ) == 1 ? "zie" : "ach" );
	}
	strcat( czasomierz, " gry.\n\r" );
	STC( czasomierz, ch );

	licz_zn = liczba( ch->desc->wej_znaki );
	licz_ln = liczba( ch->desc->wej_linie );
	ch_printf( ch, "Podczas tej sesji Lac odebra`l od ciebie %d znak%s (%d lini%s).\n\r",
		ch->desc->wej_znaki,
		licz_zn == 1 ? "" : licz_zn == 2 ? "i" : "`ow",
		ch->desc->wej_linie,
		licz_ln == 1 ? "`e" : licz_ln == 2 ? "e" : "i" );
	licz_zn = liczba( ch->desc->wyj_znaki );
	licz_ln = liczba( ch->desc->wyj_linie );
	ch_printf( ch, "W tym samym czasie wys`la`l ci %d znak%s (%d lini%s).\n\r\n\r",
		ch->desc->wyj_znaki,
		licz_zn == 1 ? "" : licz_zn == 2 ? "i" : "`ow",
		ch->desc->wyj_linie,
		licz_ln == 1 ? "`e" : licz_ln == 2 ? "e" : "i" );
    }

    /*
     * After extract_char the ch is no longer valid!
     */
    d = ch->desc;
    extract_char( ch, TRUE );

    if ( d )
	close_socket( d, FALSE );

    return;
}


KOMENDA( do_save )
{
    if ( IS_NPC( ch ) )
	return;

/*
    if ( ch->level < 2 && !ch->trust )
    {
	send_to_char( "Musisz mie`c drugi poziom, aby zapisywa`c.\n\r", ch );
	return;
    }
*/

    save_char_obj( ch );
    ASTC( "Twoja posta`c zosta`la zapisana, a ty ukaran$y ma`lym op`o`xnieniem.", ch );
    WAIT_STATE( ch, 16 );

    return;
}


KOMENDA( do_follow )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Za kim chcesz si`e w`l`oczy`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie ma go tu.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master )
    {
	act( ZM_ZAWSZE, "Chyba jednak nale`zy chodzi`c za $$!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( !ch->master )
	{
	    ASTC( "Ju`z przecie`z chodzisz sam$a za sob`a.", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }

    if ( victim == ch->master )
    {
	act( ZM_ZAWSZE, "Przecie`z ju`z chodzisz za $$!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ( ch->level - victim->level < -10
	|| ch->level - victim->level > 10 )
      && !IS_HERO( ch ) )
    {
	send_to_char( "Bez przesady, to nie dla ciebie partia.\n\r", ch );
	return;
    }

    if ( ch->master )
	stop_follower( ch );

    add_follower( ch, victim );

    return;
}


void wywal_monitory( CHAR_DATA *ch )
{
    ZWOD_DATA *zwod;

    if ( ch->monitor )
    {
	wywal_zwod( &ch->monitor->zwod_monitor, ch );
	ch->monitor = NULL;
    }

    if ( ch->zwod_monitor )
	for ( zwod = ch->zwod_monitor; zwod; zwod = zwod->next )
	{
	    if ( zwod->ch->monitor != ch )
		bug( "zwod->ch->monitor != ch", 0 );
	    zwod->ch->monitor = NULL;
	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_monitor;
		ch->zwod_monitor = NULL;
		break;
	    }
	}

    return;
}


void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master = master;
    dod_zwod( &master->zwod_master, ch );

    if ( ch->leader )
    {
	wywal_zwod( &ch->leader->zwod_leader, ch );
	ch->leader = NULL;
    }

    if ( ch->monitor && ch->monitor->leader != ch )
    {
	wywal_zwod( &ch->monitor->zwod_monitor, ch );
	ch->monitor = NULL;
    }

    act( ZM_WZROK | ZM_WID_CHAR, "$n b`edzie odt`ad za tob`a chodzi`c.", ch, NULL, master, TO_VICT );
    act( ZM_ZAWSZE, "B`edziesz odt`ad chodzi`c za $$.",  ch, NULL, master, TO_CHAR );

    return;
}


/*
 * Uwaga: ta funkcja odpala sie rekurencyjnie.
 */
void stop_follower( CHAR_DATA *ch )
{
    if ( !ch->master )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	affect_strip( ch, gsn_charm_person );
	affect_strip( ch, gsn_domination );
	affect_strip( ch, gsn_hipnoza );
	affect_strip( ch, gsn_kontrola_umyslu );
	if ( IS_AFFECTED( ch, AFF_SLUGA ) )
	{
	    /* zwolnij_sluge wywoluje teraz affect_remove z affect_strip */
	    affect_strip( ch, gsn_sprowadzenie_szkieletu );
	    affect_strip( ch, gsn_wskrzeszenie );
	    affect_strip( ch, gsn_stworzenie_mumii );
	    /* oznaczam w ten przedziwny sposob dla plynne_odradzanie, zeby
	       tutaj go nie zabijac */
	    SET_BIT_V( ch->affected_by, AFF_SLUGA );
	}
	if ( IS_AFFECTED( ch, AFF_CHARM ) )
	{
	    if ( !IS_NPC( ch ) || !IS_SET( ch->act, ACT_PET ) )
		bug( "nie zdjeto wplywu z AFF_CHARM", 0 );
	    REMOVE_BIT_V( ch->affected_by, AFF_CHARM );
	}
    }

    /* w przypadku zdjecia czarow juz sie uruchomilismy rekurencyjnie */
    if ( !ch->master )
	return;

    act( ZM_WZROK | ZM_WID_CHAR, "$n przestaje za tob`a chodzi`c.",
	ch, NULL, ch->master, TO_VICT );
    act( ZM_ZAWSZE, "Przestajesz chodzi`c za $$.",
	ch, NULL, ch->master, TO_CHAR );

    wywal_zwod( &ch->master->zwod_master, ch );
    ch->master = NULL;
    if ( ch->leader )
    {
	wywal_zwod( &ch->leader->zwod_leader, ch );
	ch->leader = NULL;
	wywal_monitory( ch );
    }
    if ( ch->monitor )
    {
	wywal_zwod( &ch->monitor->zwod_monitor, ch );
	ch->monitor = NULL;
    }

    return;
}


void die_follower( CHAR_DATA *ch )
{
    ZWOD_DATA *zwod, *zwod_next;

    if ( ch->master )
	stop_follower( ch );

    wywal_monitory( ch );

    if ( ch->leader )
    {
	wywal_zwod( &ch->leader->zwod_leader, ch );
	ch->leader = NULL;
    }

    if ( ch->zwod_leader )
    {
	for ( zwod = ch->zwod_leader; zwod; zwod = zwod->next )
	{
	    if ( zwod->ch->leader != ch )
		bug( "zwod->ch->leader != ch", 0 );
	    zwod->ch->leader = NULL;
	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_leader;
		ch->zwod_leader = NULL;
		break;
	    }
	}
    }

    if ( ch->zwod_master )
    {
	for ( zwod = ch->zwod_master; zwod; zwod = zwod_next )
	{
	    zwod_next = zwod->next; /* stop_follower wyczysci liste */
	    if ( zwod->ch->master != ch )
		bug( "zwod->ch->master != ch", 0 );
	    stop_follower( zwod->ch );
	}
	if ( ch->zwod_master )
	    bug( "stop_follower nie dal rady wyczyscic listy zwod", 0 );
    }

    return;
}


KOMENDA( do_order )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    char       arg[ MAX_INPUT_LENGTH ];
    bool       found;
    bool       zajeci;
    bool       fAll;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
    {
	send_to_char( "Komu co rozkaza`c?\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	if ( ch->sex == 2 )
	    STC( "Raczej powinna`s wykonywa`c, a nie dawa`c rozkazy.\n\r", ch );
	else
	    STC( "Raczej powiniene`s wykonywa`c, a nie dawa`c rozkazy.\n\r", ch );
	return;
    }

    if ( czy_wszyscy( arg ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Ju`z si`e robi!\n\r", ch );
	    return;
	}

	if ( !IS_AFFECTED( victim, AFF_CHARM ) || victim->master != ch )
	{
	    ASTC( "Sam$a to zr`ob!", ch );
	    return;
	}
    }

    found = FALSE;
    zajeci = FALSE;
    for ( och = ch->in_room->people; och; och = och_next )
    {
	och_next = och->next_in_room;

	if ( och->in_room != room )
	    break;

	if ( och->deleted )
	    continue;

	if ( IS_AFFECTED( och, AFF_CHARM )
	    && och->master == ch
	    && ( fAll || och == victim ) )
	{
	    if ( och->wait )
	    {
		zajeci = TRUE;
		continue;
	    }
	    if ( !found )
		found = TRUE;
	    act( ZM_ZAWSZE, "$n nakazuje ci zrobi`c \"$t\".", ch, argument, och, TO_VICT );
	    interpret( och, argument, INTERP_ORDER );
	}

	if ( ch->deleted || ch->in_room != room )
	    return;
    }

    if ( !found )
    {
	if ( zajeci )
	    send_to_char( "Nikt tu nie ma czasu s`lucha`c polece`n.\n\r", ch );
	else
	    send_to_char( "Nikt si`e tu ciebie nie s`lucha.\n\r", ch );
    }
    else
    {
	if ( zajeci )
	    send_to_char( "Rozkaz zosta`l wykonany, ale nie wszyscy mieli czas go wys`lucha`c.\n\r", ch );
	else
	    send_to_char( "Rozkaz zosta`l wykonany.\n\r", ch );
    }

    return;
}


/*
 * Lam 23.3.2005: "grupa" bardziej jak "kto", nie patrzy na niewidki (nawet
 * boskie; nie chodz w grupie jesli sobie tego nie zyczysz). Zreszta i tak
 * wiadomo, kto byl w grupie, zanim wlaczyl BN.
 */
KOMENDA( do_group )
{
    CHAR_DATA *victim;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];
    const char *pozycje[ 8 ] =
    {
	"`smier`c", "dogorywanie", "nieprzytomno`s`c", "og`luszenie",
	"sen", "odpoczynek", "walka", "stoj`aca"
    };

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = ( ch->leader ) ? ch->leader : ch;
	sprintf( buf, "{mGrupa %s:{x\n\r", leader->dopelniacz ); /* PERSD( leader, ch ) */
	send_to_char( buf, ch );

	for ( gch = char_list; gch; gch = gch->next )
	{
	    if ( gch->deleted )
		continue;
	    if ( is_same_group( gch, ch ) )
	    {
		ch_printf( ch,
"{B[{y%2d {r%s{B] {R%-16s{x {G%4d{x/{g%4d {b`z {G%4d{x/{g%4d {bm {G%4d{x/{g%4d {br{x\n\r",
			gch->level,
			IS_NPC( gch ) ? "{G---"
				      : (char *)class_table[ gch->klasa ].pl_name,
			gch->short_descr, /* PERS( gch, ch ) */
			gch->hit,   gch->max_hit,
			gch->mana,  gch->max_mana,
			gch->move,  gch->max_move );
		ch_printf( ch, "          {BDo`sw.: {M%-6d{B   Pozycja: {Y%s{x\n\r",
			gch->exp, pozycje[ URANGE( 0, gch->position, 7 ) ] );
	    }
	}
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz tu nikogo takiego.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	if ( ch->leader && ch->leader != ch )
	{
	    act( ZM_ZAWSZE, "Wyst`epujesz z grupy $!.", ch, NULL, ch->leader, TO_CHAR );
	    act( ZM_ZAWSZE, "$n wyst`epuje z twojej grupy.", ch, NULL, ch->leader, TO_VICT );
	    /* glownie dylemat ponizej, bo inni z grupy mogliby widziec,
	       nawet oslepieni/nieprzytomni, skoro "grupa" i "gmow" dziala,
	       to by moglo byc miedzypomieszczeniowe */
	    if ( ch->in_room == ch->leader->in_room )
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n wyst`epuje z grupy $!.", ch, NULL, ch->leader, TO_NOTVICT );
	    if ( ch->leader )
	    {
		wywal_zwod( &ch->leader->zwod_leader, ch );
		ch->leader = NULL;
	    }
	    wywal_monitory( ch );
	}
	else
	    STC( "Zawsze jeste`s ze sob`a \"w grupie\".\n\r", ch );

	return;
    }

    if ( ch->master || ( ch->leader && ch->leader != ch ) )
    {
	send_to_char( "Przecie`z chodzisz za kim`s innym!\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act( ZM_ZAWSZE, "$N nie chodzi za tob`a.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	if ( victim->leader )
	{
	    wywal_zwod( &victim->leader->zwod_leader, victim );
	    victim->leader = NULL;
	}
	wywal_monitory( victim );
	act( ZM_ZAWSZE, "Usuwasz $# ze swojej grupy.", ch, NULL, victim, TO_CHAR );
	act( ZM_ZAWSZE, "$n usuwa ci`e ze swojej grupy.", ch, NULL, victim, TO_VICT );
	/* tu jak w gmow i komentarz jak powyzej - grupa powinna widziec? */
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n usuwa $# ze swojej grupy.", ch, NULL, victim, TO_NOTVICT );
	return;
    }

    if ( ch->level - victim->level < -8
      || ch->level - victim->level >  8
      || ( ch->level <  100 && victim->level >= 100 ) /* Lam 7.4.99 */
      || ( ch->level >= 100 && victim->level <  100 ) )
    {
	act( ZM_ZAWSZE, "$N nie mo`ze do`l`aczy`c do twojej grupy.", ch, NULL, victim, TO_CHAR );
	act( ZM_ZAWSZE, "Nie mo`zesz do`l`aczy`c do grupy $1.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$N nie mo`ze do`l`aczy`c do grupy $1.", ch, NULL, victim, TO_NOTVICT );
	return;
    }

    if ( victim->leader ) /* nie wiem, czy to mozliwe */
	wywal_zwod( &victim->leader->zwod_leader, victim );
    victim->leader = ch;
    dod_zwod( &ch->zwod_leader, victim );
    act( ZM_ZAWSZE, "$N do`l`acza do twojej grupy.", ch, NULL, victim, TO_CHAR );
    act( ZM_ZAWSZE, "Do`l`aczasz do grupy $1.", ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$N do`l`acza do grupy $1.", ch, NULL, victim, TO_NOTVICT );

    /* Lam 6.7.98, 7.11.2005 */
    if ( !ch->monitor )
	do_monitor( ch, arg );
    if ( !victim->monitor )
    {
	one_argument( ch->name, arg );
	do_monitor( victim, arg );
    }

    return;
}


/*
 * Lam: monitoruj
 */
KOMENDA( do_monitor )
{
    CHAR_DATA *victim;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	CHAR_DATA *monitor = ch->monitor;

	if ( !monitor )
	{
	    STC( "Nikogo nie monitorujesz.\n\r", ch );
	    return;
	}

	if ( monitor->deleted )
	    return;

	act( ZM_ZAWSZE, "Monitorujesz $#.", ch, NULL, monitor, TO_CHAR );

	sprintf( buf, "Ma on%s %d/%d `zycia, %d/%d many i %d do`swiadczenia.\n\r",
			monitor->sex == 2 ? "a" : monitor->sex == 0 ? "o" : "",
			monitor->hit,
			monitor->max_hit,
			monitor->mana,
			monitor->max_mana,
			monitor->exp);
	send_to_char( buf, ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
	return;
    }

    if ( !is_same_group( victim, ch ) )
    {
	act( ZM_ZAWSZE, "$N nie jest w twojej grupie!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->monitor == victim )
    {
	act( ZM_ZAWSZE, "Ju`z $V monitorujesz.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->monitor )
	wywal_zwod( &ch->monitor->zwod_monitor, ch );

    if ( victim == ch )
    {
	if ( ch->monitor )
	{
	    act( ZM_ZAWSZE, "Przestajesz monitorowa`c $#.", ch, NULL, ch->monitor, TO_CHAR );
	    ch->monitor = NULL;
	}
	else
	    STC( "Nikogo nie monitorujesz.\n\r", ch );
    }
    else
    {
	ch->monitor = victim;
	dod_zwod( &victim->zwod_monitor, ch );
	act( ZM_ZAWSZE, "Monitorujesz $#.", ch, NULL, victim, TO_CHAR );
    }

    return;
}


/*
 * 'Split' originally by Gnort, God of Chaos.
 */
KOMENDA( do_split )
{
    CHAR_DATA *gch;
    char       buf[ MAX_STRING_LENGTH ];
    char       arg[ MAX_INPUT_LENGTH  ];
    int        members;
    int        amount;
    int        share;
    int        extra;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Ile z`lota rozdzieli`c?\n\r", ch );
	return;
    }

    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "Twojej grupie by si`e to nie spodoba`lo.\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "Rozdzielasz zero monet, ale nikt tego nie zauwa`za. Niewdzi`eczni!\n\r", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "Nie masz takiej ilo`sci z`lota przy sobie.\n\r", ch );
	return;
    }

    members = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( gch->deleted )
	    continue;
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members < 2 )
    {
	send_to_char( "Zatrzymaj wszystko.\n\r", ch );
	return;
    }

    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "Nie masz co dzieli`c. Dusigrosz!\n\r", ch );
	return;
    }

    ch->gold -= amount;
    ch->gold += share + extra;

    sprintf( buf,
	"Rozdzielasz %s. Twoja dzia`lka to %s.\n\r",
	ZLOTO_B( amount ), ZLOTO_M( share + extra ) );
    send_to_char( buf, ch );

    sprintf( buf, "$n rozdziela %s. Twoja dzia`lka to %s.",
	ZLOTO_B( amount ), ZLOTO_M( share ) );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( gch->deleted )
	    continue;
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    /* jesli nie widzi, nie umie policzyc */
	    act( ZM_WZROK, buf, ch, NULL, gch, TO_VICT );
	    act( ZM_PRZYT | ZM_W_WZROK, "Dostajesz od kogo`s troch`e z`lota.", ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }

    return;
}


/*
 * Lam 29.4.2000: ta funkcja powinna uzywac act( ) (zeby nie bylo widac
 * niewidzialnych), pozwalac slyszec spiacym oraz graczom nie mogacym mowic,
 * a takze miec konfigurowalny kolor.
 *
 * Vigud 17.9.2010: z "niewidzialnych" Lam sie wycofal, wiec nie trzeba act( ),
 * cala reszta jest od dawna. Dzis dodalem kolorowanie imion i osobny komunikat
 * dla autora wiadomosci.
 */
KOMENDA( do_gtell )
{
    CHAR_DATA *gch;
    char       buf [ MAX_STRING_LENGTH ];

    if ( !strlen_pl_kol( argument ) )
    {
	send_to_char( "Co powiedzie`c grupie?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->act, PLR_NO_TELL ) )
    {
	send_to_char( "Twoja informacja nie dotar`la!\n\r", ch );
	return;
    }

    if ( miodek( ch, argument ) )
	return;

    argument = makedrunk( argument, ch );

    for ( gch = char_list; gch; gch = gch->next )
	if ( is_same_group( gch, ch ) )
	{
	    if ( gch == ch )
	    {
		sprintf( buf, "{;M`owisz grupie: \"%s{;\"{x\n\r", argument );
		STC( buf, gch );
		continue;
	    }

	    buf[ 0 ] = '{';
	    buf[ 1 ] = ( gch->podswietl == 0 || !ch->desc ) ? ';' : ch->desc->kolor;

	    sprintf( buf + 2, "%s%s m`owi grupie: \"%s{%c\"{x\n\r",
		ch->short_descr,
		( gch->podswietl == 1 ) ? "{;" : "",
		argument,
		( gch->podswietl == 2 ) ? buf[ 1 ] : ';' );

	    STC( buf, gch );
	}

    return;
}


KOMENDA( do_beep )
{
    CHAR_DATA *victim;
    char arg[ MIL ];
    char buf[ MSL ];

    if ( IS_NPC( ch ) )
	return;

    one_argument( argument, arg );

    if ( !arg[ 0 ] )
    {
	send_to_char( "\aKogo bipn`a`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world_pc_tele( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	STC( "\a\aBipasz si`e. Czy tw`oj g`lo`sniczek dzia`la?\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Tylko graczy mo`zna bipa`c.\n\r", ch );
	return;
    }
    else if ( CZY_OPCJA( victim, OPC_CISZA ) && get_trust( ch ) < 108 )
    {	/* bogowie >= 108 moga bipac pomimo opcji */
	ch_printf( ch, "%s nie pozwala si`e bipa`c.\n\r", PERS( victim, ch, TRUE ) );
	return;
    }
    else if ( is_name2( ch->name, victim->pcdata->ignore ) )
    {
	act_pp( ZM_ZAWSZE, "$N nie `zyczy sobie, aby`s $U si`e narzuca`l$a.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( WP( ch ) && PP( victim ) && skill_get_trust( ch ) < 106 )
    {
	STC( "Nie wolno ci bipa`c graczy poza postaci`a.\n\r", ch );
	return;
    }
    if ( PP( ch ) && WP( victim ) && skill_get_trust( ch ) < 106 )
    {
	STC( "Nie wolno ci bipa`c graczy w postaci.\n\r", ch );
	return;
    }

    ch_printf( ch, "Bipasz %s.\n\r", PERSB( victim, ch, TRUE ) );
    sprintf( buf, "\a\a%s ci`e bipa.\n\r", PERS( ch, victim, TRUE ) );
    buf[ 2 ] = UPPER( buf[ 2 ] );
    STC( buf, victim );

    return;
}


/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->deleted || bch->deleted )
	return FALSE;

    if ( ach->leader ) ach = ach->leader;
    if ( bch->leader ) bch = bch->leader;

    return ach == bch;
}


/*
 * Separatory napisane przez Albiego z pomoca Lama
 */
KOMENDA( do_separator )
{
    char       buf [ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
	 return;

    if ( argument[ 0 ] == '\0' )
    {
	sprintf( buf, "Tw`oj separator to \"{R%c`{x\".\n\r", ch->separator );
	send_to_char( buf, ch );
	return;
    }

    if ( ( argument[ 0 ] == 's' ) || ( argument[ 0 ] == 'e' )
      || ( argument[ 0 ] == 'p' ) || ( argument[ 0 ] == '~' )
      || ( argument[ 0 ] == ' ' ) )
    {
	send_to_char( "Takiego separatora nie mo`zesz mie`c!\n\r", ch );
	return;
    }

    ch->separator = argument[ 0 ];
    /* Lam: ponizej ` po to, zeby taki separator tez byl pokazywany */
    /* separator { wciaz nie bedzie pokazywany... trudno */
    sprintf( buf, "Separator ustawiony na \"{R%c`{x\".\n\r", ch->separator );
    send_to_char( buf, ch );

    return;
}


KOMENDA( do_delete_disabled )
{
    STC( "Kasowanie postaci nie jest obs`lugiwane. W zamian mo`zesz si`e zrestartowa`c.\n\r", ch );

    return;
}


#if defined( MAM_SKASUJ )
KOMENDA( do_delet )
{
    if ( IS_NPC( ch ) )
	return;

    if ( *argument ) /* maly trik */
	REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_DELETE );
    send_to_char( "Musisz napisa`c skasuj w ca`lo`sci je`sli NAPRAWD`E chcesz skasowa`c sw`a posta`c.\n\r", ch );

    return;
}


KOMENDA( do_delete )
{
#if defined( MSDOS )
    send_to_char( "Naci`snij F8 w Dos Navigatorze, aby si`e usun`a`c. :)\n\r", ch );
    return;
#endif

    if ( IS_NPC( ch ) || !ch->desc )
	return;

    if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_DELETE ) )
    {
	if ( *argument )
	{
	    REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_DELETE );
	    send_to_char( "Jednak nie chcesz si`e skasowa`c...\n\r", ch );
	    return;
	}
	else
	{
	    /* czy mozemy wyjsc z gry? */
	    if ( !can_quit_now( ch, FALSE ) )
		return;

	    save_char_obj( ch );

	    send_to_char( "Podaj has`lo, `zebym wiedzia`l, czy na pewno chcesz si`e skasowa`c.\n\r"
			  "Has`lo: ", ch );
	    send_to_char( (char *) &echo_off_str[ 0 ], ch );
	    ch->pcdata->was_in_room = ch->in_room;
	    char_from_room( ch );
	    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
	    ch->desc->connected = CON_GET_OLD_PWD_DELETE;
	    return;
	}
    }

    if ( *argument )
	send_to_char( "Napisz skasuj bez argumentu, aby si`e usun`a`c.\n\r", ch );
    else
    {
	ASTC( "Je`sli naprawd`e jeste`s zdecydowan$y si`e usun`a`c, napisz skasuj dwa razy.\n\r"
		"Pami`etaj: usuni`ecie jest NIEODWRACALNE. Mo`ze jednak zmienisz zdanie?\n\r"
		"Je`sli wpiszesz po raz drugi \"skasuj\", posta`c zostanie skasowana.\n\r"
		"Je`sli napiszesz \"skasuj\" z argumentem lub wyjdziesz, flaga pragnienia\n\r"
		"skasowania zostanie z ciebie zdj`eta.", ch );
	SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_DELETE );
    }

    return;
}
#endif /* MAM_SKASUJ */


/*
 * Lam 11.5.98: polecenie "zadanie", na wniosek Aldara i Quada
 * Winchester: Zadanie start/stop
 * Lam 18.10.2005: przebudowane w hybryde - obsluguje liste wykonywanych zadan
 * z krain jak rowniez aktualnie trwajace zadanie animowane przez
 * niesmiertelnych.
 * Lam 23.5.2006: nikt nie pisze opisow zadan, wylaczam, bo przez to tylko
 * gracze zglaszaja bledy, ktorych nikt nie zamierza poprawiac.
 */
KOMENDA( do_quest )
{
#if 0
    MPQUEST_DATA *mpquest;
#endif
    static char quest[ MAX_STRING_LENGTH * 2 ];
    static bool start;
    char tmp[ MAX_INPUT_LENGTH ];
    int pos;
    int last;
#if 0
    int i;
#endif

    /* Lam 12.2.2004: automatyczne powiadomienie o zadaniu przy wejsciu do gry */
    if ( !str_cmp( argument, "autowej" ) )
    {
	if ( start )
	    STC( "\n\r{gTrwa zadanie! Napisz {Gzadanie{g, aby dowiedzie`c si`e, o co chodzi tym razem.{x\n\r", ch );

	return;
    }

    one_argument( argument, tmp );
    if ( skill_get_trust( ch ) >= 103 )
    {
	if ( !str_cmp( tmp, "pomoc" ) || !str_cmp( tmp, "?" ) )
	{
	    send_to_char( "{GSk`ladnia: {Wzadanie [ pomoc/start/stop/+/-/czy`s`c ] [ tekst ].{x\n\r", ch );
	    return;
	}
	if ( !str_cmp( tmp, "start" ) )
	{
	    if ( !quest[ 0 ] )
	    {
		send_to_char( "Napisz najpierw tre`s`c.\n\r", ch );
		return;
	    }
	    if ( start )
	    {
		send_to_char( "Zadanie ju`z trwa.\n\r", ch );
		return;
	    }
	    send_to_all_char( "{Y                -=-=-=-=- !!!  RUSZY`LO ZADANIE  !!! -=-=-=-=-{x\n\r" );
	    send_to_all_char( "{Y  -=-=-=-=- {gNapisz {Gzadanie{g, a dowiesz si`e, o co chodzi tym razem  {Y-=-=-=-=-{x\n\r" );
	    start = TRUE;
	    return;
	}
	if ( !str_cmp( tmp, "stop" ) )
	{
	    if ( !start )
	    {
		send_to_char( "Nie masz czego zatrzymywa`c.\n\r", ch );
		return;
	    }
	    send_to_all_char( "{Y               -=-=-=-=- !!!  KONIEC ZADANIA  !!! -=-=-=-=-{x\n\r" );
	    start = FALSE;
	    quest[ 0 ] = '\0';
	    return;
	}
	if ( !str_cmp( tmp, "czy`s`c" ) )
	{
	    if ( start )
	    {
		send_to_char( "Czy`sci`c dzia`laj`ace zadanie?\n\r", ch );
		return;
	    }
	    quest[ 0 ] = '\0';
	    send_to_char( "Czy`scisz zadanie.\n\r", ch );
	    return;
	}
	/* mozna modyfikowac zadanie w locie */
	if ( *argument == '+' )
	{
	    if ( strlen( quest ) > MAX_STRING_LENGTH * 2 - MAX_INPUT_LENGTH )
	    {
		send_to_char( "Za d`lugi tekst.\n\r", ch );
		return;
	    }
	    if ( argument[ 1 ] == ' ' )
	    {
		argument = one_argument( argument, tmp );
		strcat( quest, argument );
	    }
	    else
		strcat( quest, argument + 1 );
	    strcat( quest, "\n\r" );
	    send_to_char( "Dodajesz lini`e do tre`sci zadania.\n\r", ch );
	    return;
	}
	if ( *argument == '-' )
	{
	    if ( !quest[ 0 ] )
	    {
		send_to_char( "Nie piszesz `zadnego zadania.\n\r", ch );
		return;
	    }

	    last = 0;
	    for ( pos = 0; quest[ pos ]; pos++ )
		if ( quest[ pos ] == '\r' && quest[ pos + 1 ] != '\0' )
		    last = pos;

	    quest[ last + 1 ] = '\0';
	    if ( !last )
		quest[ 0 ] = '\0';
	    send_to_char( "Usuwasz lini`e z tre`sci zadania.\n\r", ch );
	    return;
	}
    }

#if 0 /* wersja z lista zadan - niech ktos pisze te opisy, to sie wlaczy */
    if ( is_number( tmp ) )
    {
	i = 0;
	pos = atoi( tmp );

	for ( mpquest = ch->mpquest; mpquest; mpquest = mpquest->next )
	    if ( mpquest->title && *mpquest->title )
	    {
		i++;
		if ( i == pos )
		{
		    if ( mpquest->descr && *mpquest->descr )
			STC( mpquest->descr, ch );
		    else
			STC( "Nie wiesz niczego o tym zadaniu.\n\r", ch );
		    return;
		}
	    }

	STC( "Nie zlecono ci takiego zadania.\n\r", ch );
	return;
    }

    if ( ( *tmp && !str_prefix( tmp, "lista" ) ) || !quest[ 0 ]
      || ( !start && skill_get_trust( ch ) < 103 ) )
    {
	char buf[ MSL ];
	char bufm[ MSL ];

	buf[ 0 ] = '\0';
	i = 0;
	for ( mpquest = ch->mpquest; mpquest; mpquest = mpquest->next )
	    if ( mpquest->title && *mpquest->title )
	    {
		if ( !i )
		    strcpy( buf, "Zlecono ci nast`epuj`ace zadania:\n\r" );
		i++;
		sprintf( bufm, "%3d. %s\n\r", i, mpquest->title );
		strcat( buf, bufm );
	    }

	if ( i )
	    strcat( buf, "Napisz \"zadanie <numer>\", aby dowiedzie`c si`e czego`s wi`ecej.\n\r" );
	else
	    strcpy( buf, "Nie zlecono ci `zadnych zada`n.\n\r" );
	send_to_char( buf, ch );
	return;
    }
#else
    if ( !start )
    {
	STC( "Nie trwa `zadne og`olnomudowe zadanie.\n\r", ch );
	return;
    }
#endif

    send_to_char( quest, ch );

    return;
}


/* czy stos jest pusty? */
bool pusty_stos( STOS *s )
{
    return s ? FALSE : TRUE;
}


/* odloz operacje na stos */
void na_stos( STOS **s, char c )
{
    STOS *t;

    t = calloc( 1, sizeof( STOS ) );
    t->op = c;
    t->nast = *s;
    *s = t;

    return;
}


/* odloz wartosc na stos */
void na_stos_w( STOS **s, float w )
{
    STOS *t;

    t = calloc( 1, sizeof( STOS ) );
    t->wart = w;
    t->nast = *s;
    *s = t;

    return;
}


/* jaka operacja jest na szczycie stosu? */
char glowa( STOS *s )
{
    return s->op;
}


/* zwroc operacje ze szczytu stosu, zdejmujac ja z niego */
char ze_stosu( STOS **s )
{
    char c = 0;
    STOS *t;

    t = *s;
    if ( *s )
    {
	c = ( *s )->op;
	*s = ( *s )->nast;
	free( t );
    }

    return c;
}


/* zdejmij i zwroc wartosc ze szczytu stosu */
float ze_stosu_w( STOS **s )
{
    float w = 0;
    STOS *t;

    t = *s;
    if ( *s )
    {
	w = (*s)->wart;
	*s = (*s)->nast;
	free( t );
    }

    return w;
}


/* zwraca nastepny skladnik wyrazenia (liczbe, dzialanie). */
#define POMIN while ( *c && strchr( sep2, *c ) ) c++;
char *licz_nastepny_skladnik( char *wyrazenie, char *skladnik )
{
    const char *sep1 = "+-*/^k"; /* dzialania */
    const char *sep2 = " \t"; /* separatory */
    const char *sep3 = "()"; /* nawiasy */
    bool czy_dzialanie; /* czego szukamy? */
    char *c = wyrazenie;
    char buf1[ 1000 ];
    char *b = buf1;

    /* puste wyrazenie/koniec wyrazenia */
    if ( !c || !*c )
    {
	*skladnik = '\0';
	return c;
    }

    /* pomin spacje/tabulatory */
    POMIN

    if ( strchr( sep3, *c ) ) /* kazdy nawias jest oddzielny */
    {
	*skladnik = *c;
	    skladnik++, c++;
	*skladnik = '\0';
	POMIN
	return c;
    }

    czy_dzialanie = FALSE;
    if ( strchr( sep1, *c ) )
	czy_dzialanie = TRUE;

    /* dzialania trzymaja sie grupami, nawet jesli sa oddzielone odstepem,
       np. 2+ *2 to 2 +* 2, dzieki temu latwo znajdujemy bledy */
    if ( czy_dzialanie )
    {
	while ( ( *b = *c ) )
	if ( strchr( sep2, *c ) )
	    POMIN
	else if ( !strchr( sep1, *c ) )
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
    /* liczby daja sie oddzielac odstepami, ale takze dzialaniami
       i nawiasami */
    {
	while ( ( *b = *c )
		&& !strchr( sep1, *c )
		&& !strchr( sep2, *c )
		&& !strchr( sep3, *c ) )
	    b++, c++;

	if ( *b )
	{
	    POMIN
	    *b = '\0';
	}
	strcpy( skladnik, buf1 );
	return c;
    }

    *skladnik = '\0';

    return c;
}


/* sprawdza, czy skladnik jest "zdatny", czyli czy go obslugujemy */
void sprawdz_zdatnosc( CHAR_DATA *ch, char *skl, bool *blad )
{
    if ( strcmp( skl, "+" )
      && strcmp( skl, "-" )
      && strcmp( skl, "*" )
      && strcmp( skl, "/" )
	/*  && strcmp( skl, "^" ) */
      && strcmp( skl, "(" )
      && strcmp( skl, ")" )
      && strcmp( skl, "k" ) )
    {
	ch_printf( ch, "B`l`ad: nie obs`lugiwane: \"%s\".\n\r", skl );
	*blad = 2;
    }

    return;
}


/* czy skladnik jest liczba? liczba moze zawierac najwyzej jedna kropke,
   poza tym musi sie skladac z cyfr. Przecinek zamieniamy na kropke */
bool licz_czy_liczba( char *w )
{
    bool kropka = FALSE;

    while ( w && *w )
    {
	if ( *w == ',' )
	    *w = '.';
	if ( !isdigit( (unsigned char) *w ) && ( *w != '.' || kropka ) )
	    return FALSE;
	if ( *w == '.' )
	    kropka = TRUE;
	w++;
    }

    return TRUE;
}


/* jaki jest priorytet dzialania? */
int priorytet( CHAR_DATA *ch, char c, bool *blad )
{
    switch ( c )
    {
	case '+': case '-':	return 1;
	case '*': case '/':	return 2;
	/*case '^':*/ case 'k':	return 3;
	case '(':		return 0;
	default:
	{
	    ch_printf( ch, "Nie obs`lugiwane dzia`lanie \"%c\".\n\r", c );
	    *blad = 4;
	    return -1;
	}
    }

    return -1;
}


/* Lam 14.12.2001: ograniczenie poziomu zaglebiania rzucania koscmi */
int dice_level;

/* rekurencyjna funkcja obliczajaca wynik zapisanego na stosie przez funkcje
   onp wyrazenia */
float wynik( CHAR_DATA *ch, STOS **s, bool *blad )
{
    float pzl, toc;
    char op;
    float wart;

    /* Lam 13.1.2003 */
    if ( !*s )
    {
	STC( "Brak liczb wok`o`l znaku dzia`lania.\n\r", ch );
	*blad = 11;
	return 0;
    }

    op = (*s)->op;
    wart = (*s)->wart;

    ze_stosu( s );

    if ( op )
    {
	/* + i * sa przemienne, pozostale dzialania trzeba odwrocic */
	switch ( op )
	{
	    case '+': pzl = wynik( ch, s, blad );
		if ( *blad ) return 0;
		return wynik( ch, s, blad ) + pzl;
	    case '*': pzl = wynik( ch, s, blad );
		if ( *blad ) return 0;
		return wynik( ch, s, blad ) * pzl;
	    case '-': pzl = wynik( ch, s, blad );
		if ( *blad ) return 0;
		return wynik( ch, s, blad ) - pzl;
	    case '/': pzl = wynik( ch, s, blad );
		if ( *blad ) return 0;
		return wynik( ch, s, blad ) / pzl;
/*	    case '^': pzl = wynik( ch, s, blad );
		if ( *blad ) return 0;
		return pow( wynik( ch, s, blad ), pzl ); */
	    case 'k': if ( dice_level )
		{
		    STC( "Nie wolno zag`l`ebia`c rzut`ow ko`s`cmi.\n\r", ch );
		    *blad = 8;
		    return 0;
		}
		dice_level++;
		pzl = wynik( ch, s, blad );
		if ( *blad ) return 0;
		if ( pzl > 100 )
		{
		    STC( "Ko`s`c mo`ze by`c co najwy`zej stu`scienna.\n\r", ch );
		    *blad = 9;
		    return 0;
		}
		toc = wynik( ch, s, blad );
		if ( toc > 100 )
		{
		    STC( "Nie wolno wykona`c wi`ecej ni`z 100 rzut`ow ko`sci`a.\n\r", ch );
		    *blad = 10;
		    return 0;
		}
		pzl = dice( toc, pzl );
		dice_level--;
		return pzl;
	    case '(': STC( "Niedomkni`ety nawias!\n\r", ch );
		*blad = 6;
		return 0;
	    default:  STC( "Dziwny b`l`ad.\n\r", ch );
		*blad = 7;
		return 0;
	}
    }

    return wart;
}


KOMENDA( do_licz )
{
    /* dwa stosy */
    STOS *s1 = NULL; /* stos pomocny przy funkcji onp */
    STOS *s2 = NULL; /* stos na ktory odklada funkcja onp, a z ktorego czyta funkcja wynik */
    char *tym;
    char wyr[ 1024 ];
    float wart;
    int p;
    bool blad = FALSE;

    tym = argument;
    dice_level = 0;

    if ( !*tym )
    {
	STC( "Sk`ladnia: licz <wyra`zenie>\n\r", ch );
	return;
    }

    do
    {
	tym = licz_nastepny_skladnik( tym, wyr );
	if ( licz_czy_liczba( wyr ) )
	{
	    wart = atof( wyr );
	    na_stos_w( &s2, wart );
	}
	else
	{
	    sprawdz_zdatnosc( ch, wyr, &blad );
	    if ( blad )
	    {
		while ( !pusty_stos( s1 ) )
		    ze_stosu( &s1 );
		while ( !pusty_stos( s2 ) )
		    ze_stosu( &s2 );
		return;
	    }
	    if ( *wyr == '(' )
		na_stos( &s1, *wyr );
	    else if ( *wyr == ')' )
	    {
		while ( !pusty_stos( s1 )
		     && glowa( s1 ) != '(' )
		{
		    na_stos( &s2, ze_stosu( &s1 ) );
		}
		if ( pusty_stos( s1 ) )
		{
		    STC( "Nawias zamykaj`acy bez otwieraj`acego!\n\r", ch );
		    while ( !pusty_stos( s2 ) )
			ze_stosu( &s2 );
		    return;
		}
		ze_stosu( &s1 );
	    }
	    else
	    {
		p = priorytet( ch, *wyr, &blad );
		if ( blad )
		{
		    while ( !pusty_stos( s1 ) )
			ze_stosu( &s1 );
		    while ( !pusty_stos( s2 ) )
			ze_stosu( &s2 );
		    return;
		}
		while ( !pusty_stos( s1 )
		     && priorytet( ch, glowa( s1 ), &blad ) >= p )
		{
		    na_stos( &s2, ze_stosu( &s1 ) );
		}
		if ( blad )
		{
		    while ( !pusty_stos( s1 ) )
			ze_stosu( &s1 );
		    while ( !pusty_stos( s2 ) )
			ze_stosu( &s2 );
		    return;
		}
		na_stos( &s1, *wyr );
	    }
	}
    }
    while ( tym && *tym );

    while ( !pusty_stos( s1 ) )
	na_stos( &s2, ze_stosu( &s1 ) );

    wart = wynik( ch, &s2, &blad );
    /* funkcje ustawiajac blad wypisuja komunikat */
    if ( blad )
    {
	while ( !pusty_stos( s2 ) )
	    ze_stosu( &s2 );
	return;
    }
    if ( !pusty_stos( s2 ) )
    {
	STC( "B`l`ad sk`ladniowy wyra`zenia.\n\r", ch );
	while ( !pusty_stos( s2 ) )
	    ze_stosu( &s2 );
    }
    else
    {
	sprintf( wyr, "Wynik: %f\n\r", wart );
	for ( tym = wyr; *tym; tym++ )
	    if ( *tym == '.' )
		*tym = ',';
	STC( wyr, ch );
    }

    return;
}
