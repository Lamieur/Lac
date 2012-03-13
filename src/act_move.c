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

const   int     movement_loss   [ SECT_MAX ]    =
{
    1, 2, 2, 3, 4, 6, 4, 1, 5, 10, 6
};

const   int     obr_z_upadku    [ SECT_MAX ]    =
{
    4, 5, 4, 3, 4, 5, 1, 2, 3, 0, 4
};

/*
 * Local functions.
 */
void	zrob_krew	args( ( CHAR_DATA *ch, int door ) );
bool    has_key         args( ( CHAR_DATA *ch, int key ) );
KOMENDA( follow_enter );


void wchodzenie_do_pomieszczenia( CHAR_DATA *ch, ROOM_INDEX_DATA *in_room,
	ROOM_INDEX_DATA *to_room, int door, bool pokaz_kierunek )
{
    /* Tissarius: sprawdzanie czy nie wyciaga */
    if ( !IS_AFFECTED( ch, AFF_SNEAK )
      && ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) )
    {
	/* chodzenie po dnie */
	if ( CZY_DNO_POD_WODA( in_room )
	  && !IS_SET( race_table[ ch->race ].poruszanie, POR_POD_WODA )
	  && IS_SET( race_table[ ch->race ].poruszanie, POR_LAD )
	  && door != DIR_UP )
	{
	    /* Obsluga zmiennych komunikatow wejscia/wyjscia. Poki co mamy
	       tylko pelzajace weze, docelowo jednak powinna byc tez obsluga
	       ras, ktore kom. wej/wyj maja dla poruszania woda/powietrzem */
	    if ( !pokaz_kierunek )
		act( ZM_WZROK | ZM_WID_CHAR, "$T $n.", ch, NULL,
			*race_table[ ch->race ].kom_wejscia
			? race_table[ ch->race ].kom_wejscia :
				IS_SET( race_table[ ch->race ].parts, P_NOGI | P_LAPA )
				? "nadchodzi" : "dociera tu",
			TO_ROOM );
	    else
		act( ZM_WZROK | ZM_WID_CHAR, "$n $T $t.", ch,
			kierunki[ kierunki[ door ].odwrotny ].z_dopelniacz,
			*race_table[ ch->race ].kom_wejscia
			? race_table[ ch->race ].kom_wejscia :
				IS_SET( race_table[ ch->race ].parts, P_NOGI | P_LAPA )
				? "nadchodzi" : "dociera tu",
			TO_ROOM );
	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz narastaj`ace brz`eczenie.", ch, NULL, NULL, TO_ROOM );
	}
	else
	/* - oprocz chodzenia po dnie pod woda mozna tylko plywac
	   - jesli siedzi sie w lodzi, to sie plynie, a nie lata
	   - bez lodzi jesli sie nie lata nad gleboka woda, to sie plynie
	   - w plytkiej wodzie brodza wszyscy, ktorzy potrafia (ryby plywaja) */
	if ( CZY_POD_WODA( in_room )
	  || ( in_room->sector_type == SECT_WATER_DEEP
	    && ( jest_w_lodzi( ch )
	      || !IS_FLYING( ch )
	      || door == DIR_DOWN ) )
	  || ( in_room->sector_type == SECT_WATER_SHALLOW
	    && !IS_FLYING( ch )
	    && !IS_SET( race_table[ ch->race ].poruszanie, POR_LAD ) ) )
	{
	    if ( !pokaz_kierunek )
		act( ZM_WZROK | ZM_WID_CHAR, "Nadp`lywa $n.", ch, NULL, NULL, TO_ROOM );
	    else
		act( ZM_WZROK | ZM_WID_CHAR, "$n nadp`lywa $t.", ch, kierunki[ kierunki[ door ].odwrotny ].z_dopelniacz, NULL, TO_ROOM );
	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz narastaj`ace brz`eczenie.", ch, NULL, NULL, TO_ROOM );
	}
	else
	/* nad woda mozna leciec, w powietrzu w ogole nie ma wyboru */
	if ( CZY_POWIETRZE( in_room )
	  || ( door == DIR_UP && CZY_POWIETRZE( to_room ) )
	  || ( IS_FLYING( ch )
	    && CZY_WODA( in_room ) ) )
	{
	    if ( !pokaz_kierunek )
		act( ZM_WZROK | ZM_WID_CHAR, "Nadfruwa $n.", ch, NULL, NULL, TO_ROOM );
	    else
		act( ZM_WZROK | ZM_WID_CHAR, "$n nadfruwa $t.", ch, kierunki[ kierunki[ door ].odwrotny ].z_dopelniacz, NULL, TO_ROOM );
	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz narastaj`ace brz`eczenie.", ch, NULL, NULL, TO_ROOM );
	}
	else
	{
	    if ( !pokaz_kierunek )
		act( ZM_WZROK | ZM_WID_CHAR, "$T $n.", ch, NULL,
			*race_table[ ch->race ].kom_wejscia
			? race_table[ ch->race ].kom_wejscia :
				IS_SET( race_table[ ch->race ].parts, P_NOGI | P_LAPA )
				? "nadchodzi" : "dociera tu",
			TO_ROOM );
	    else
	    {
		if ( ch->race == zr_jez
		  && ch->in_room->area->weather_info.sunlight == SUN_DARK )
		    act( ZM_WZROK | ZM_WID_CHAR, "$n tupta $t.", ch, kierunki[ kierunki[ door ].odwrotny ].z_dopelniacz, NULL, TO_ROOM );
		else
		    act( ZM_WZROK | ZM_WID_CHAR, "$n $T $t.", ch,
			kierunki[ kierunki[ door ].odwrotny ].z_dopelniacz,
			*race_table[ ch->race ].kom_wejscia
			? race_table[ ch->race ].kom_wejscia :
				IS_SET( race_table[ ch->race ].parts, P_NOGI | P_LAPA )
				? "nadchodzi" : "dociera tu",
			TO_ROOM );
	    }
	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz narastaj`ace brz`eczenie.", ch, NULL, NULL, TO_ROOM );
	}
    }
    else if ( IS_AFFECTED( ch, AFF_SNEAK )
	   && ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) )
    {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted || vch == ch )
		continue;

	    if ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_HOLYLIGHT ) )
	    {
		act( ZM_WZROK, "$n wkrada si`e do pomieszczenia.", ch, NULL, vch, TO_VICT );
		continue;
	    }

	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH, "S`lyszysz narastaj`ace brz`eczenie.", ch, NULL, vch, TO_VICT );
	}
    }
    else if ( !IS_NPC( ch )
	   && IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	/* Lam 5.7.2004: wykrywanie BN; do tego tez jest potrzebny nadwzrok */
	CHAR_DATA *vch;
	char bnbuf[ 80 ];

	sprintf( bnbuf, "{W(BN: %d){x $n wkrada si`e do pomieszczenia.", ch->pcdata->wizinvis );
	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted
	      || vch == ch
	      || get_trust( vch ) < ch->pcdata->wizinvis )
		continue;

	    if ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_HOLYLIGHT ) )
	    {
		act( ZM_WZROK, bnbuf, ch, NULL, vch, TO_VICT );
		continue;
	    }

	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH, "S`lyszysz narastaj`ace brz`eczenie.", ch, NULL, vch, TO_VICT );
	}
    }

    return;
}


/*
 * Funkcja chodzaca. Spolszczanie i zmiany - Lam
 * 13.4.98 - zakopane wyjscia
 * 10.5.98 - pijanstwo
 *  6.7.98 - autootwieranie zamknietych drzwi
 * 22.5.99 - pulapki
 *  6.7.99 - slady krwi
 *  1.1.03 - otwieranie drzwi nieprzenikalnych przez przenikajacych
 * 17.2.03 - jesli skrytobojca wyciaga kogos to wyciaganie == TRUE - Tissarius
 * 20.11.03- nietoperze < 90 zmieniaja sie w wampiry na sloncu
 *  6.4.05 - przenikanie przez sidla/rozrywanie ich przez silnych graczy
 */
void move_char( CHAR_DATA *ch, int door, bool wyciaganie )
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    const int moved = b17;		/* 131072 - Matches ACT & PLR bits */
    bool isdrunk = FALSE;		/* Lam */
    bool pokaz_kierunek = FALSE;	/* Lam */
    bool brief = FALSE;
    bool przenikanie = FALSE;		/* Lam */
    int move = 0;
    bool slonce_przed, slonce_po;	/* Lam */
    bool tlen_przed, tlen_po;		/* Lam */
    bool oslepiony_przed;		/* Lam */
    int mod_opoznienia;			/* Gimza */
    int cmsr;				/* Vigud: czy moze sie ruszyc */
    bool przeszukuje;			/* Vigud */

    if ( door < 0 || door > 9 )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    /*
     * Prevents infinite move loop in
     * maze zone when group has 2 leaders - Kahn
     */
    if ( IS_SET( ch->act, moved ) )
	return;

    /* Alandar TODO: zamiast tego, funkcje umieszczone w timerze oraz czulosc
       timerow (na przemieszczenie, na zmiany w inwentarzu/ekwipunku) i jedna
       funkcja sprawdzajaca czulosc i wywolujaca funkcje przerywajaca */
    PRZERWIJ_KOPANIE( ch, TRUE );
    PRZERWIJ_KARMPOJ( ch, TRUE );

    in_room = ch->in_room;
    mod_opoznienia = 1;
    slonce_przed = CZY_PALACE_SLONCE( ch );
    tlen_przed = CZY_BRAK_TLENU( ch );
    if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->temp_flags, TEMPFLAG_BLIND ) )
	oslepiony_przed = TRUE;
    else
	oslepiony_przed = FALSE;

    if ( !IS_NPC( ch ) && ( ch->pcdata->condition[ COND_DRUNK ] > 10
			 || IS_AFFECTED( ch, AFF_DEZORIENTACJA ) ) )
    {
	isdrunk = TRUE;
	if ( number_percent( ) < ch->pcdata->condition[ COND_DRUNK ] - 10
	  || ( IS_AFFECTED( ch, AFF_DEZORIENTACJA )
	    && number_percent( ) < 20 ) )
	    door = number_door( ); /* Lam: losowy kierunek */
    }

    /* Lam: nie ma wyjscia jesli go nie ma lub jesli jest zakopane */
    if ( !( pexit = in_room->exit[ door ] ) || !( to_room = pexit->to_room )
	|| IS_SET( pexit->exit_info, EX_BURIED ) )
    {
	if ( !eprog_bounce_trigger( ch, door ) )
	{
	    if ( isdrunk )
	    {
		if ( in_room->sector_type == SECT_INSIDE
		  || in_room->sector_type == SECT_CITY )
		    ASTC( "Jeste`s tak pijan$y, `ze trafiasz w `scian`e!", ch );
		else
		    ASTC( "Jeste`s tak pijan$y, `ze pomyli`l$o`s kierunki i kr`ecisz si`e w k`o`lko!", ch );
	    }
	    else
		send_to_char( "Nie mo`zesz i`s`c w tym kierunku.\n\r", ch );
	}
	return;
    }

    if ( IS_AFFECTED( ch, AFF_HOLD ) )
    {
	if ( !is_affected( ch, gsn_snare )
	  || ( number_percent( )
	     > str_app[ get_curr_str( ch ) ].uwalnianie
	    && !IS_PASSING( ch ) ) )
	{
	    STC( "Schwytano ci`e w sid`la! Nie mo`zesz si`e porusza`c!\n\r", ch );
	    WAIT_STATE( ch, 4 );
	    return;
	}
	else if ( IS_PASSING( ch ) )
	{
	    STC( "Przenikasz przez sid`la!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n przenika przez sid`la!", ch, NULL, NULL, TO_ROOM );
	    affect_strip( ch, gsn_snare );
	}
	else
	{
	    STC( "Rozrywasz sid`la!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n rozrywa sid`la!", ch, NULL, NULL, TO_ROOM );
	    act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz jaki`s trzask.", ch, NULL, NULL, TO_ROOM );
	    affect_strip( ch, gsn_snare );
	}
    }

    /* Lam 6.7.98: przechodzenie przez zamkniete drzwi */
    /* Lam 1.1.2003: postacie z przenikaniem nie otwieraly sobie
       nieprzenikalnych, ale nie zakluczonych drzwi */
    if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
    {
	if ( !IS_SET( pexit->exit_info, EX_LOCKED )
	  && IS_SET( race_table[ ch->race ].race_abilities, RACE_OPENS_DOORS )
	  && ( !IS_PASSING( ch )
	    || IS_SET( pexit->exit_info, EX_PASSPROOF ) ) )
	{
	    ROOM_INDEX_DATA *from;

	    from = ch->in_room;
	    do_open( ch, kierunki[ door ].skrot );
	    if ( ch->in_room != from ) /* eprog? */
		return;
	    if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	    {
		move_char( ch, door, FALSE );		/* rekurencja */
		if ( ch->deleted ) return;		/* greet_prog */
		if ( from != ch->in_room		/* jesli sie ruszyl */
		  && ch->in_room->exit[ kierunki[ door ].odwrotny ] /* jesli jest wyjscie */
		  && ch->in_room->exit[ kierunki[ door ].odwrotny ]->to_room == from
			/* jesli prowadzi to poprzedniego pomieszczenia */
		  && IS_SET( ch->in_room->exit[ kierunki[ door ].odwrotny ]->exit_info, EX_ISDOOR ) )
			/* jesli da sie zamknac */
		    do_close( ch, kierunki[ kierunki[ door ].odwrotny ].skrot );
		return;
	    }
	}

	if ( !IS_PASSING( ch ) )
	{
	    if ( !eprog_bounce_trigger( ch, door ) )
	    {
		if ( !can_see_room( ch, ch->in_room ) )
		{
		    /* jesli nie wie, co to, nie wie, czy zamkniete */
		    STC( "Przej`scie jest zablokowane.\n\r", ch );
		    return;
		}

		switch ( pexit->rodzaj )
		{
		    default:
			act( ZM_ZAWSZE | ZM_DYLEMAT, "$D s`a zamkni`ete.",
				ch, NULL, pexit->short_descr, TO_CHAR );
			break;
		    case 0:
			act( ZM_ZAWSZE | ZM_DYLEMAT, "$D jest zamkni`ete.",
				ch, NULL, pexit->short_descr, TO_CHAR );
			break;
		    case 1:
			act( ZM_ZAWSZE | ZM_DYLEMAT, "$D jest zamkni`ety.",
				ch, NULL, pexit->short_descr, TO_CHAR );
			break;
		    case 2:
			act( ZM_ZAWSZE | ZM_DYLEMAT, "$D jest zamkni`eta.",
				ch, NULL, pexit->short_descr, TO_CHAR );
			break;
		}
	    }
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_PASSPROOF ) )
	{
	    if ( !eprog_bounce_trigger( ch, door ) )
	    {
		if ( !can_see_room( ch, ch->in_room ) )
		{
		    /* jesli nie wie, co to, nie wie, czy zamkniete */
		    STC( "Przej`scie jest zablokowane.\n\r", ch );
		    return;
		}

		/* brak widoku obsluzylem wyzej, czyli ZM_ZAWSZE pasuje */
		act( ZM_ZAWSZE, "Nie potrafisz przenikn`a`c przez $D. Au!",
			ch, NULL, pexit->biernik, TO_CHAR );
	    }
	    return;
	}

	przenikanie = TRUE;
    }

    if ( !can_enter_room( ch, to_room, TRUE ) )
	return;

    if ( !( cmsr = can_move( ch, in_room, to_room, door, TRUE ) ) )
	return;

    if ( !IS_NPC( ch ) )
    {
	move = movement_loss[ UMIN( SECT_MAX - 1, in_room->sector_type ) ]
	     + in_room->strata_ruchu
	     + movement_loss[ UMIN( SECT_MAX - 1, to_room->sector_type ) ]
	     + to_room->strata_ruchu;

	if ( ( przeszukuje = is_affected( ch, gsn_search ) ) )
	{
	    int 	 mod;

	    mod   = ch->pcdata->learned[ gsn_search ] / 15;
	    move += 15 - mod;
	}

	/* latanie zmniejsza zmeczenie, ale w powietrzu, nie w lodzi/pod woda;
	   pod woda/na wodzie zmeczenie zmniejsza kontrola, tez nie w lodzi */
	if ( !jest_w_lodzi( ch )
	  && ( ( IS_FLYING( ch )
	      && !CZY_POD_WODA( ch->in_room ) )
	    || ( IS_AFFECTED( ch, AFF_KONTROLA_NAD_WODA )
	      && CZY_WODA( ch->in_room ) ) ) )
	{
	    if ( is_affected( ch, gsn_search ) )
		move = UMAX( 2, move / 4 ) + 6;
	    else
		move = UMAX( 2, move / 4 );
	}

	/* sprawdzamy czy postac przeciazona */
	if ( ch->carry_weight > can_carry_w( ch )
	  && !IS_FLYING( ch ) )
	{
	    /* obciazenie w granicach 100-200% */
	    if ( ch->carry_weight <= can_carry_w( ch ) * 2 )
	    {
		move *= 2;
		mod_opoznienia = 3;
	    }
	    else /* ponad 200% normy */
	    {
		ASTC( "Jeste`s zbyt przeci`a`zon$y, `zeby si`e poruszy`c.", ch );
		return;
	    }
	}

	/* Tissarius: sprawdzanie czy nie wyciaga */
	if ( ch->move < move && !wyciaganie )
	{
	    if ( is_affected( ch, gsn_search ) )
		affect_strip( ch, gsn_search );
	    ASTC( "Jeste`s zbyt wycie`nczon$y.", ch );
	    return;
	}

	if ( przeszukuje )
	    uzyj( ch, gsn_search, "przeszukiwaniu" );
	if ( cmsr == 1 )
	    uzyj( ch, gsn_plywanie, "p`lywaniu" );
    } /* if ( !IS_NPC( ch ) ) */

    eprog_pass_trigger( ch, door ); /* Lam 13.7.98 */
    if ( ch->deleted || in_room != ch->in_room )
	return;

    /* Tissarius: sprawdzanie czy nie wyciaga */
    if ( !IS_NPC( ch ) && !wyciaganie )
    {
	WAIT_STATE( ch, mod_opoznienia );
	ch->move -= move;
    }

    /* Lam, pomysl sladow krwi sciagniety z EOS
       Tissarius: sprawdzanie czy nie wyciaga;
       Alandar: poprzez MAX_DIR daje znac, ze slady w powietrze */
    if ( !wyciaganie )
	zrob_krew( ch, to_room->sector_type != SECT_AIR ? door : MAX_DIR );

    /* Tissarius: sprawdzanie czy nie wyciaga */
    if ( !IS_AFFECTED( ch, AFF_SNEAK )
      && ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) )
      && !wyciaganie )
    {
	/* chodzenie po dnie */
	if ( CZY_DNO_POD_WODA( in_room )
	  && !IS_SET( race_table[ ch->race ].poruszanie, POR_POD_WODA )
	  && IS_SET( race_table[ ch->race ].poruszanie, POR_LAD )
	  && door != DIR_UP )
	{
	    act( ZM_WZROK | ZM_WID_CHAR, "$n $t na $T.", ch,
		*race_table[ ch->race ].kom_wyjscia
		? race_table[ ch->race ].kom_wyjscia :
			IS_SET( race_table[ ch->race ].parts, P_NOGI | P_LAPA )
			? "odchodzi" : "przemieszcza si`e",
		kierunki[ door ].biernik, TO_ROOM );
	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH | ZM_W_WID_CHAR, "Brz`eczenie cichnie.", ch, NULL, NULL, TO_ROOM );
	}
	else
	/* - oprocz chodzenia po dnie pod woda mozna tylko plywac
	   - jesli siedzi sie w lodzi, to sie plynie, a nie lata
	   - bez lodzi jesli sie nie lata nad gleboka woda, to sie plynie
	   - w plytkiej wodzie brodza wszyscy, ktorzy potrafia (ryby plywaja) */
	if ( CZY_POD_WODA( in_room )
	  || ( in_room->sector_type == SECT_WATER_DEEP
	    && ( jest_w_lodzi( ch )
	      || !IS_FLYING( ch )
	      || door == DIR_DOWN ) )
	  || ( in_room->sector_type == SECT_WATER_SHALLOW
	    && !IS_FLYING( ch )
	    && !IS_SET( race_table[ ch->race ].poruszanie, POR_LAD ) ) )
	{
	    act( ZM_WZROK | ZM_WID_CHAR, "$n odp`lywa na $T.", ch, NULL, kierunki[ door ].biernik, TO_ROOM );
	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH | ZM_W_WID_CHAR, "Brz`eczenie cichnie.", ch, NULL, NULL, TO_ROOM );
	}
	else
	/* nad woda mozna leciec, w powietrzu w ogole nie ma wyboru */
	if ( CZY_POWIETRZE( in_room )
	  || ( door == DIR_UP && CZY_POWIETRZE( to_room ) )
	  || ( IS_FLYING( ch )
	    && CZY_WODA( in_room ) ) )
	{
	    act( ZM_WZROK | ZM_WID_CHAR, "$n odfruwa na $T.", ch, NULL, kierunki[ door ].biernik, TO_ROOM );
	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH | ZM_W_WID_CHAR, "Brz`eczenie cichnie.", ch, NULL, NULL, TO_ROOM );
	}
	else
	{
	    if ( ch->race == zr_jez
	      && ch->in_room->area->weather_info.sunlight == SUN_DARK )
		act( ZM_WZROK | ZM_WID_CHAR, "$n tupta na $T.", ch, NULL, kierunki[ door ].biernik, TO_ROOM );
	    else
		act( ZM_WZROK | ZM_WID_CHAR, "$n $t na $T.", ch,
			*race_table[ ch->race ].kom_wyjscia
			? race_table[ ch->race ].kom_wyjscia :
				IS_SET( race_table[ ch->race ].parts, P_NOGI | P_LAPA )
				? "odchodzi" : "przemieszcza si`e",
			kierunki[ door ].biernik, TO_ROOM );
	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH | ZM_W_WID_CHAR, "Brz`eczenie cichnie.", ch, NULL, NULL, TO_ROOM );
	}
    }
    else if ( !IS_NPC( ch )
	   && IS_SET( ch->act, PLR_WIZINVIS )
	   && !wyciaganie )
    {
	/* Lam 5.7.2004: wykrywanie BN; do tego tez jest potrzebny nadwzrok */
	CHAR_DATA *vch;
	char bnbuf[ 80 ];

	sprintf( bnbuf, "{W(BN: %d){x $n wykrada si`e z pomieszczenia na $t.", ch->pcdata->wizinvis );
	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted
	      || vch == ch
	      || get_trust( vch ) < ch->pcdata->wizinvis )
		continue;

	    if ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_HOLYLIGHT ) )
	    {
		act( ZM_WZROK, bnbuf, ch, kierunki[ door ].biernik, vch, TO_VICT );
		continue;
	    }

	    /* przy BN na niskie poziomy, efekt skradania - smiertelnicy
	       bez nadwzroku slysza brzeczenie */
	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH, "Brz`eczenie cichnie.", ch, NULL, vch, TO_VICT );
	}
    }
    else if ( IS_AFFECTED( ch, AFF_SNEAK )
	   && !wyciaganie )
    {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted || vch == ch )
		continue;

	    if ( ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_HOLYLIGHT ) )
	      || is_fighting( ch, vch ) || is_fighting( vch, ch ) )
	    {
		act( ZM_WZROK, "$n wykrada si`e z pomieszczenia na $t.", ch, kierunki[ door ].biernik, vch, TO_VICT );
		/* dla walczacych bez nadwzroku */
		if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		    act( ZM_SLUCH | ZM_W_WZROK, "Brz`eczenie cichnie.", ch, NULL, vch, TO_VICT );
		continue;
	    }

	    if ( IS_AFFECTED( ch, AFF_BRZECZY ) )
		act( ZM_SLUCH, "Brz`eczenie cichnie.", ch, NULL, vch, TO_VICT );
	}
    }

    if ( ch->in_room != to_room
      && to_room->exit[ kierunki[ door ].odwrotny ]
      && to_room->exit[ kierunki[ door ].odwrotny ]->to_room == ch->in_room )
	pokaz_kierunek = TRUE;

    if ( przenikanie )
	act( ZM_ZAWSZE | ZM_DYLEMAT, "Przenikasz przez $D.", ch, NULL, pexit->biernik, TO_CHAR );

    if ( ch->fighting )
	act( ZM_ZAWSZE, "Uciekasz na $T.", ch, NULL, kierunki[ door ].biernik, TO_CHAR );

    /* Lam 11.6.2006: Zeby zamknac #333 dziele na dwa etapy. I tu klopot.
       W tym miejscu ch przezyl juz pass_prog wyjscia, ale moze nie przezyc
       greet_proga pomieszczenia docelowego lub moba tam siedzacego. Jesli ch
       jest mobem, za ktorym chodzi gracz (jak w #333), zobaczy:
       Mob odchodzi na wschod.
       Podazasz za mobem.
       Przestajesz chodzic za mobem.
       Co bedzie wygladac okropnie. Poki co nie znam miejsca, gdzie greet_prog
       zabija ruchome moby, wiec zostawiam wlasnie tak. */
    for ( fch = in_room->people; fch; fch = fch_next )
    {
	fch_next = fch->next_in_room;

	if ( fch->deleted )
	    continue;

	if ( fch->master == ch && fch->position == POS_STANDING )
	    act( ZM_PRZYT, "Pod`a`zasz za $$.", fch, NULL, ch, TO_CHAR );
    }

    /* Lam 29.1.2004, zamiast petli ponizej i zamiast tego samego w do_flee */
    stop_fighting( ch, TRUE );

    char_from_room( ch );
    char_to_room( ch, to_room );

    if ( !wyciaganie )
	wchodzenie_do_pomieszczenia( ch, in_room, to_room, door, pokaz_kierunek );

    if ( ch->level < 90
      && is_affected( ch, gsn_postac_nietoperza )
      && !IS_SET( ch->in_room->room_flags, ROOM_NOTE_READ )
      && !IS_SET( ch->in_room->room_flags, ROOM_PURGATORY )
      && ch->in_room->area->time_info.hour > 6
      && ch->in_room->area->time_info.hour < 20
      && !IS_SET( ch->in_room->room_flags, ROOM_UNDERGROUND )
      && ch->in_room->sector_type != SECT_UNDERWATER )
    {
	STC( "Powracasz do swojej postaci.\n\r", ch );
	affect_strip( ch, gsn_postac_nietoperza );
    }

    SET_BIT( ch->act, moved );

    /* Lam 11.6.2006: ten if aktualnie nie ma sensu. Wczesniej byla mozliwosc
       smierci przed tym etapem, teraz dopiero w po_wejsciu. Warunek powinien
       byc zawsze spelniony. */
    if ( !ch->deleted && ch->in_room == to_room )
    {
	/* sciagniete ze Smauga - niech bedzie widac opis pomieszczenia-pulapki */
	if ( !IS_NPC( ch )
	  && ch->in_room->trap
	  && !IS_IMMORTAL( ch )
	  && CZY_OPCJA( ch, OPC_BRIEF ) )
	{
	    brief = TRUE;
	    REMOVE_BIT( ch->pcdata->opcje, OPC_BRIEF );
	}

	/*
	 * Suggested by D'Sai from A Moment in Tyme Mud.  Why have mobiles
	 * see the room?  -Kahn
	 */
	/* Tissarius: sprawdzanie czy nie wyciaga */
	if ( ch->desc && !wyciaganie )
	{
	    if ( IS_NPC( ch )
	      || oslepiony_przed
	      || !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_BLIND ) )
		do_look( ch, "auto" );
	    auto_hunt( ch );  /* Lam */
	}

	if ( brief )
	    SET_BIT( ch->pcdata->opcje, OPC_BRIEF );

	slonce_po = CZY_PALACE_SLONCE( ch );
	if ( slonce_przed != slonce_po )
	{
	    if ( slonce_po )
		STC( "{Y`Swiat`lo pali twoj`a sk`or`e!{x\n\r", ch );
	    else
		STC( "`Swiat`lo przestaje ci`e parzy`c.\n\r", ch );
	}

	tlen_po = CZY_BRAK_TLENU( ch );
	if ( tlen_przed != tlen_po )
	{
	    if ( tlen_po )
		STC( "{RNie mo`zesz oddycha`c!{x\n\r", ch );
	    else
		STC( "`Lapiesz oddech.\n\r", ch );
	}

	po_wejsciu( ch, TRUE );
    }

    /* Jesli ch to gracz, smierc w po_wejsciu nie rozwiaze grupy. W kwestii
       mobow patrz wyzej */
    for ( fch = in_room->people; fch; fch = fch_next )
    {
	fch_next = fch->next_in_room;

	if ( fch->deleted )
	    continue;

	if ( fch->master == ch && fch->position == POS_STANDING )
	    move_char( fch, door, FALSE );
    }

    REMOVE_BIT( ch->act, moved );

    return;
}


/*
 * ostroznosc niemozliwa podczas spadania
 * UWAGA! aktualnie spadanie = !ostroznosc i z tego powodu pulapki dla
 * spadajacych wlasnie to sprawdzaja, jesli to bedzie uzyte gdzies poza
 * spadaniem i poruszaniem sie, dodac odrebny argument!
 */
void po_wejsciu( CHAR_DATA *ch, bool ostroznosc )
{
    ROOM_INDEX_DATA *to_room = ch->in_room;

    /* Lam 22.5.99: pulapki
       Skrytobojcy moga omijac pulapki dzieki Fuyarze.
       Lam 15.1.2004: omijanie pulapek nie sprawdzalo, czy w pomieszczeniu jest
       pulapka */
    if ( !IS_NPC( ch ) && ch->in_room->trap && ostroznosc
      && is_affected( ch, gsn_search ) )
    {
	STC( "{GWchodz`ac ostro`znie do pomieszczenia odkrywasz, `ze znajduje si`e w nim{x\n\r", ch );
	STC( "{Gpu`lapka. Dzi`eki swym wyostrzonym zmys`lom jeste`s w stanie j`a omin`a`c.{x\n\r", ch );
    }
    else
    if ( !IS_NPC( ch )
      && ch->in_room->trap
      && !IS_IMMORTAL( ch )
      /* spadajacy wpada w kazda pulapke, nie spadajacy nie wpada w typ 1 */
      && ( !ostroznosc || ch->in_room->trap->trap_type != 1 ) )
    {
	char buf [ MSL ];

	act( ZM_WZROK | ZM_DYLEMAT, "$n ginie na miejscu!", ch, NULL, NULL, TO_ROOM );
	send_to_char( przemiel_opis( ch, ch->in_room->trap->trap_string ), ch );
	sprintf( buf, "%s hit a DEATH TRAP in room %d!",
	    ch->name, ch->in_room->vnum );
	log_string( buf );
	sprintf( buf, "{R$N{x wpada w `smierteln`a pu`lapk`e w pomieszczeniu {Y#%d{x.", ch->in_room->vnum );
	wiznet( buf, ch, NULL, NULL, WIZ_DEATH, 0, 0 );
	/* mprog_death_trigger zbedny, to nie mob */
	strata_dosw_za_smierc( ch );
	death_cry( ch, 0, ch->position );
	raw_kill( ch, ch, FALSE, FALSE, ZGON_ZWYKLY ); /* zdecydowalem sie na raw_kill... nie wiem czy to dobrze */
    }

    /* Lam 15.1.2004: wykonywane przed podazaniem */
    if ( !ch->deleted && ch->in_room == to_room )
    {
	mprog_entry_trigger( ch );
	if ( !ch->deleted && ch->in_room == to_room )
	{
	    rprog_greet_trigger( ch );
	    if ( !ch->deleted && ch->in_room == to_room )
		 mprog_greet_trigger( ch );
	}
    }

    return;
}


void zrob_krew( CHAR_DATA *ch, int door )
{
    OBJ_DATA *krew, *krew_next;
    char buf[ 200 ];
    int ile_krwi;

    /* czy w ogole krwawic? */
    if ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_WIZINVIS ) )
      || !race_table[ ch->race ].parts
      || ch->hit >= ch->max_hit / 4
      /* Alandar 15.01.04: nie powinnismy zostawiac sladow krwi w powietrzu */
      /* Ulryk 03.11.05: ani w wodzie */
      || !CZY_ZIEMIA( ch->in_room )
      /* Lam 15.2.2006: czy rasa ma krew? */
      || IS_SET( race_table[ ch->race ].race_abilities, RACE_NO_BLOOD ) )
	return;

    /* brak krwi takze "w niecielesnym swiecie", Lam 16.11.2005 */
    if ( ch->in_room->area == area_czysciec )
	return;

    /* Lam 15.1.2004: kaluze krwi */
    ile_krwi = 0;
    for ( krew = ch->in_room->contents; krew; krew = krew->next_content )
    {
	if ( krew->deleted )
	    continue;

	if ( krew->pIndexData->vnum == OBJ_VNUM_KREW )
	    ile_krwi++;
	else if ( krew->pIndexData->vnum == OBJ_VNUM_KALUZA_KRWI )
	{
	    /* jesli jest juz kaluza, powiekszamy ja */
	    if ( krew->timer >= 0 )
		krew->timer = UMIN( 10, krew->timer + 1 );

	    return;
	}
    }

    if ( ile_krwi >= 2 )
    {
	for ( krew = ch->in_room->contents; krew; krew = krew_next )
	{
	    krew_next = krew->next_content;

	    if ( !krew->deleted && krew->pIndexData->vnum == OBJ_VNUM_KREW )
		extract_obj( krew );
	}

	krew = create_object( get_obj_index( OBJ_VNUM_KALUZA_KRWI ), 0 );
	krew->timer = 2 + number_bits( 1 ) + number_bits( 1 );
	obj_to_room( krew, ch->in_room );
	return;
    }

    krew = create_object( get_obj_index( OBJ_VNUM_KREW ), 0 );
    krew->timer = 1;
    /* na/w wodzie powinny pozostawac krotko */
    if ( !ch->in_room->sector_type == SECT_WATER_SHALLOW
      && !ch->in_room->sector_type == SECT_WATER_DEEP
      && !ch->in_room->sector_type == SECT_UNDERWATER )
	krew->timer += number_bits( 1 ) + number_bits( 1 );

    /* jesli door == MAX_DIR to znaczy, ze slady powinny prowadzic w powietrze,
       a ze nie potrafie sobie tego wyobrazic, po prostu sie tam znajduja bez
       prowadzenia */
    if ( door == MAX_DIR )
	sprintf( buf, "Widzisz tu `slady czyjej`s krwi." );
    else
	sprintf( buf, krew->description, kierunki[ door ].na_krew );
    free_string( krew->description );
    krew->description = str_dup( buf );
    obj_to_room( krew, ch->in_room );

    return;
}


KOMENDA( do_north )
{
    move_char( ch, DIR_NORTH, FALSE );

    return;
}


KOMENDA( do_east )
{
    move_char( ch, DIR_EAST, FALSE );

    return;
}


KOMENDA( do_south )
{
    move_char( ch, DIR_SOUTH, FALSE );

    return;
}


KOMENDA( do_west )
{
    move_char( ch, DIR_WEST, FALSE );
    
    return;
}


KOMENDA( do_up )
{
    move_char( ch, DIR_UP, FALSE );

    return;
}


KOMENDA( do_down )
{
    move_char( ch, DIR_DOWN, FALSE );

    return;
}


KOMENDA( do_northeast )
{
    move_char( ch, DIR_NORTHEAST, FALSE );

    return;
}


KOMENDA( do_southeast )
{
    move_char( ch, DIR_SOUTHEAST, FALSE );

    return;
}


KOMENDA( do_northwest )
{
    move_char( ch, DIR_NORTHWEST, FALSE );

    return;
}


KOMENDA( do_southwest )
{
    move_char( ch, DIR_SOUTHWEST, FALSE );

    return;
}


/*
 * Fukcja zmieniona wzgledem Envy, uwaga na funkcje uzywajace jej.
 * Teraz nie pisze komunikatu, ze nie ma wyjscia, zwraca wtedy -1.
 * -1 zwraca dla wyjsc nie 'newsgd', dla nich jest -2 i pisze komunikat.
 * To po to, zeby najpierw szukac wyjsc, a potem przedmiotow w "otworz" itp.
 * Lam 14.4.98: modyfikacje dla zakopanych wyjsc
 * Lam 25.9.98: spolszczenie
 */
int find_door( CHAR_DATA *ch, char *arg, bool drzwi )
{
    EXIT_DATA *pexit;
    int        door;

	 if ( !str_cmp( arg, "n" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) ) door = 3;
    else if ( !str_cmp( arg, "pn" ) ) door = 0;
    else if ( !str_cmp( arg, "pd" ) ) door = 2;
    else if ( !str_prefix( arg, "p`o`lnoc" ) ) door = 0;
    else if ( !str_prefix( arg, "wsch`od" ) ) door = 1;
    else if ( !str_prefix( arg, "po`ludnie" ) ) door = 2;
    else if ( !str_prefix( arg, "zach`od" ) ) door = 3;
    else if ( !str_prefix( arg, "g`ora" ) ) door = 4;
    else if ( !str_prefix( arg, "d`o`l" ) ) door = 5;
    else if ( !str_cmp( arg, "ne" ) ) door = 6;
    else if ( !str_cmp( arg, "se" ) ) door = 7;
    else if ( !str_cmp( arg, "nw" ) ) door = 8;
    else if ( !str_cmp( arg, "sw" ) ) door = 9;
    else if ( !str_prefix( arg, "pn-wsch" ) ) door = 6;
    else if ( !str_prefix( arg, "pd-wsch" ) ) door = 7;
    else if ( !str_prefix( arg, "pn-zach" ) ) door = 8;
    else if ( !str_prefix( arg, "pd-zach" ) ) door = 9;
    else
    {
	if ( !can_see_room( ch, ch->in_room ) )
	    return -1;

	for ( door = 0; door <= 9; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[ door ] )
	      && ( !drzwi || IS_SET( pexit->exit_info, EX_ISDOOR ) )
	      && !IS_SET( pexit->exit_info, EX_BURIED )
	      && pexit->keyword
	      && is_name( arg, pexit->keyword ) )
		return door;
	}
/*        act( "Nie widz`e tu $T.", ch, NULL, arg, TO_CHAR );
*/
	return -1;
    }

    if ( !( pexit = ch->in_room->exit[ door ] )
      || IS_SET( pexit->exit_info, EX_BURIED ) )
    {
	act( ZM_ZAWSZE, "Nie widz`e tu drzwi na $T.", ch, NULL,
	/*arg*/ kierunki[ door ].biernik, TO_CHAR );
	return -2;
    }

    if ( drzwi && !IS_SET( pexit->exit_info, EX_ISDOOR ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return -2;
    }

    return door;
}


KOMENDA( do_open )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       door;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co otworzy`c?\n\r", ch );
	return;
    }

    door = find_door( ch, arg, TRUE );
    if ( door == -2 )
	return;

    if ( !IS_SET( race_table[ ch->race ].race_abilities, RACE_OPENS_DOORS ) )
    {
	send_to_char( "Nie umiesz niczego otwiera`c.\n\r", ch );
	return;
    }

    if ( door >= 0 )
    {
	/* "otworz drzwi" */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[ door ];

	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "To ju`z jest otwarte.\n\r", ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
	{
	    if ( !can_see_room( ch, ch->in_room ) )
	    {
		STC( "Nie mo`zesz tego otworzy`c.\n\r", ch );
		return;
	    }

	    send_to_char( "To jest zakluczone.\n\r", ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_BEZKLAMKI ) )
	{
	    if ( !can_see_room( ch, ch->in_room ) )
	    {
		STC( "Nie mo`zesz tego otworzy`c.\n\r", ch );
		return;
	    }

	    send_to_char( "Od tej strony nie da si`e tego otworzy`c.\n\r", ch );
	    return;
	}

	REMOVE_BIT( pexit->exit_info, EX_CLOSED );
	if ( IS_NPC( ch ) && ch->pIndexData->vnum == MOB_VNUM_SUPERMOB ) /* Lam */
	{
	    if ( pexit->rodzaj > 2 )
		act( ZM_WZROK, "$D si`e otwieraj`a.", ch, NULL, pexit->short_descr, TO_ROOM );
	    else
		act( ZM_WZROK, "$D si`e otwiera.", ch, NULL, pexit->short_descr, TO_ROOM );
	}
	else if ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) /* Qwert */
	    act( ZM_WZROK, "$n otwiera $D.", ch, NULL, pexit->biernik, TO_ROOM );
	/* powyzej ewentualnie oddzielne ZM_WZROK | ZM_W_WID_CHAR, bez "ktos" */

	if ( !can_see_room( ch, ch->in_room ) )
	    send_to_char( "Otwierasz przej`scie.\n\r", ch );
	else
	    act( ZM_ZAWSZE, "Otwierasz $D.", ch, NULL, pexit->biernik, TO_CHAR );

	/* Alandar 23.10.2004: sprawdza czy postac nie otworzyla sobie badz
	   komus drzwi spod nog */
	if ( door == DIR_DOWN )
	{
	    CHAR_DATA *rch;
	    OBJ_DATA *robj;

	    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
		check_airchar( rch );
	    for ( robj = ch->in_room->contents; robj; robj = robj->next_content )
		new_airobj( robj );
	}

	/* open the other side */
	if ( ( to_room   = pexit->to_room )
	  && ( pexit_rev = to_room->exit[ kierunki[ door ].odwrotny ] )
	  && pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;
	    OBJ_DATA *robj;

	    /* moze zamiast zakladac, ze wyjscie sie odkopie gdy je otworzymy,
	       powinnismy wprowadzic bit okreslajacy, czy to przypadkiem nie
	       klapa otwierana do gory? (Wtedy nie daloby sie jej otworzyc bez
	       odkopania z zewnatrz). Dobrze by bylo, gdyby przy klapie
	       otwieranej w dol mozna bylo zostac zasypanym przez ziemie
	       wpadajaca z gory, i np. od razu zginac. To drugie bez problemu
	       do zrobienia open_progiem! */
	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    REMOVE_BIT( pexit_rev->exit_info, EX_BURIED );
	    for ( rch = to_room->people; rch; rch = rch->next_in_room )
	    {
		if ( rch->deleted )
		    continue;

		if ( IS_AWAKE( rch ) )
		{
		    if ( pexit_rev->rodzaj > 2 )
			act( ZM_WZROK, "$D si`e otwieraj`a.", rch, NULL, pexit_rev->short_descr, TO_CHAR );
		    else
			act( ZM_WZROK, "$D si`e otwiera.", rch, NULL, pexit_rev->short_descr, TO_CHAR );
		}
		/* Alandar 23.10.2004: sprawdza czy postac nie stracila gruntu
		   spod nog */
		if ( door == DIR_UP )
		    check_airchar( rch );
	    }
	    if ( door == DIR_UP )
		for ( robj = to_room->contents; robj; robj = robj->next_content )
		    new_airobj( robj );
	}
	eprog_open_trigger( ch, door ); /* Lam 13.7.98 */
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* "otworz przedmiot" */
	if ( obj->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "To nie pojemnik.\n\r", ch );
	    return;
	}

	if ( !IS_SET( obj->value[ 1 ].v, CONT_CLOSED ) )
	{
	    send_to_char( "To ju`z jest otwarte.\n\r", ch );
	    return;
	}

	if ( !IS_SET( obj->value[ 1 ].v, CONT_CLOSEABLE ) )
	{
	    send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	    return;
	}

	if ( IS_SET( obj->value[ 1 ].v, CONT_LOCKED ) )
	{
	    send_to_char( "To jest zakluczone.\n\r", ch );
	    return;
	}

	REMOVE_BIT( obj->value[ 1 ].v, CONT_CLOSED );
	act( ZM_ZAWSZE, "Otwierasz $j.", ch, obj, NULL, TO_CHAR );
	if ( ( IS_NPC( ch ) && ch->pIndexData->vnum != MOB_VNUM_SUPERMOB ) /* Lam */
	  || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) ) ) /* Qwert */
	    act( ZM_WZROK, "$n otwiera $j.", ch, obj, NULL, TO_ROOM );

	oprog_open_trigger( ch, obj );	/* Alandar 21.12.2004 */
	return;
    }

    act( ZM_ZAWSZE, "Nie widz`e tu niczego o nazwie \"$T\".", ch, NULL, arg, TO_CHAR );

    return;
}


KOMENDA( do_close )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       door;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co zamkn`a`c?\n\r", ch );
	return;
    }

    door = find_door( ch, arg, TRUE );
    if ( door == -2 ) return;

    if ( !IS_SET( race_table[ ch->race ].race_abilities, RACE_OPENS_DOORS ) )
    {
	send_to_char( "Nie umiesz niczego zamyka`c.\n\r", ch );
	return;
    }

    if ( door >= 0 )
    {
	/* "zamknij drzwi" */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit   = ch->in_room->exit[ door ];
	if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "To ju`z jest zamkni`ete.\n\r", ch );
	    return;
	}

       if ( IS_SET( pexit->exit_info, EX_BASHED ) )
       {
	    if ( !can_see_room( ch, ch->in_room ) )
	    {
		STC( "Nie mo`zesz tego zamkn`a`c.\n\r", ch );
		return;
	    }

	   switch ( pexit->rodzaj )
	   {
	       default:
	       act( ZM_ZAWSZE, "$D zosta`ly wywa`zone i nie mog`a zosta`c zamkni`ete.",
		   ch, NULL, pexit->short_descr, TO_CHAR );
	       return;

	       case 0:
	       act( ZM_ZAWSZE, "$D zosta`lo wywa`zone i nie mo`ze zosta`c zamkni`ete.",
		   ch, NULL, pexit->short_descr, TO_CHAR );
	       return;

	       case 1:
	       act( ZM_ZAWSZE, "$D zosta`l wywa`zony i nie mo`ze zosta`c zamkni`ety.",
		   ch, NULL, pexit->short_descr, TO_CHAR );
	       return;

	       case 2:
	       act( ZM_ZAWSZE, "$D zosta`la wywa`zona i nie mo`ze zosta`c zamkni`eta.",
		   ch, NULL, pexit->short_descr, TO_CHAR );
	       return;
	   }
       }

	if ( IS_SET( pexit->exit_info, EX_BEZKLAMKI ) )
	{
	    send_to_char( "Od tej strony nie da si`e tego dobrze zamkn`a`c.\n\r", ch );
	    return;
	}

	SET_BIT( pexit->exit_info, EX_CLOSED );
	if ( IS_NPC( ch ) && ch->pIndexData->vnum == MOB_VNUM_SUPERMOB ) /* Lam */
	{
	    if ( pexit->rodzaj > 2 )
		act( ZM_WZROK, "$D si`e zamykaj`a.", ch, NULL, pexit->short_descr, TO_ROOM );
	    else
		act( ZM_WZROK, "$D si`e zamyka.", ch, NULL, pexit->short_descr, TO_ROOM );
	}
	else if ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) /* Qwert */
	    act( ZM_WZROK, "$n zamyka $D.", ch, NULL, pexit->biernik, TO_ROOM );

	if ( !can_see_room( ch, ch->in_room ) )
	    send_to_char( "Zamykasz przej`scie.\n\r", ch );
	else
	    act( ZM_ZAWSZE, "Zamykasz $D.", ch, NULL, pexit->biernik, TO_CHAR );

	/* close the other side */
	if ( ( to_room   = pexit->to_room )
	  && ( pexit_rev = to_room->exit[ kierunki[ door ].odwrotny ] )
	  && pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch; rch = rch->next_in_room )
	    {
		if ( rch->deleted || !IS_AWAKE( rch ) )
		    continue;
		if ( pexit_rev->rodzaj > 2 )
		    act( ZM_WZROK, "$D si`e zamykaj`a.", rch, NULL, pexit_rev->short_descr, TO_CHAR );
		else
		    act( ZM_WZROK, "$D si`e zamyka.", rch, NULL, pexit_rev->short_descr, TO_CHAR );
	    }
	}
	eprog_close_trigger( ch, door ); /* Lam 13.7.98 */
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* "zamknij przedmiot" */
	if ( obj->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "To nie pojemnik.\n\r", ch );
	    return;
	}

	if ( IS_SET( obj->value[ 1 ].v, CONT_CLOSED ) )
	{
	    send_to_char( "To ju`z jest zamkni`ete.\n\r", ch );
	    return;
	}

	if ( !IS_SET( obj->value[ 1 ].v, CONT_CLOSEABLE ) )
	{
	    send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	    return;
	}

	SET_BIT( obj->value[ 1 ].v, CONT_CLOSED );
	act( ZM_ZAWSZE, "Zamykasz $j.", ch, obj, NULL, TO_CHAR );
	if ( ( IS_NPC( ch ) && ch->pIndexData->vnum != MOB_VNUM_SUPERMOB ) /* Lam */
	  || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) ) ) /* Qwert */
	    act( ZM_WZROK, "$n zamyka $j.", ch, obj, NULL, TO_ROOM );

	oprog_close_trigger( ch, obj );	/* Alandar 21.12.2004 */
	return;
    }

    act( ZM_ZAWSZE, "Nie widz`e tu niczego o nazwie \"$T\".", ch, NULL, arg, TO_CHAR );

    return;
}


bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( obj->pIndexData->vnum == key && can_see_obj( ch, obj ) )
	    return TRUE;

    return FALSE;
}


KOMENDA( do_lock )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       door;

    if ( ch->position == POS_FIGHTING )
    {
	STC( "No co ty, przecie`z ci`agle walczysz!\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co zakluczy`c?\n\r", ch );
	return;
    }

    door = find_door( ch, arg, TRUE );
    if ( door == -2 ) return;

    if ( !IS_SET( race_table[ ch->race ].race_abilities, RACE_OPENS_DOORS ) )
    {
	send_to_char( "Nie umiesz zaklucza`c zamk`ow.\n\r", ch );
	return;
    }

    if ( door >= 0 )
    {
	/* "zaklucz drzwi" */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit   = ch->in_room->exit[ door ];

	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "To nie jest zamkni`ete.\n\r", ch );
	    return;
	}

	if ( !can_see_room( ch, ch->in_room ) )
	{
	    send_to_char( "Nie widzisz zamka.\n\r", ch );
	    return;
	}

	if ( pexit->key < 0 )
	{
	    send_to_char( "To nie mo`ze zosta`c zakluczone.\n\r", ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_BEZZAMKA ) )
	{
	    send_to_char( "Z tej strony nie wida`c zamka.\n\r", ch );
	    return;
	}

	if ( !has_key( ch, pexit->key ) )
	{
	    send_to_char( "Brak ci klucza.\n\r", ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
	{
	    send_to_char( "To ju`z jest zakluczone.\n\r", ch );
	    return;
	}

	SET_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char( "*Klik*\n\r", ch );
	/* powiedzmy, ze to widac nawet jesli nie widac postaci przy zamku */
	if ( ( IS_NPC( ch ) && ch->pIndexData->vnum != MOB_VNUM_SUPERMOB ) /* Lam */
	  || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) ) ) /* Qwert */
	    act( ZM_WZROK, "$n zaklucza $D.", ch, NULL, pexit->biernik, TO_ROOM );

	/* lock the other side */
	if ( ( to_room   = pexit->to_room )
	  && ( pexit_rev = to_room->exit[ kierunki[ door ].odwrotny ] )
	  && pexit_rev->to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
	eprog_lock_trigger( ch, door ); /* Lam 13.7.98 */
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* "zaklucz przedmiot" */
	if ( obj->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "To nie pojemnik.\n\r", ch );
	    return;
	}

	if ( !IS_SET( obj->value[ 1 ].v, CONT_CLOSED ) )
	{
	    send_to_char( "To nie jest zamkni`ete.\n\r", ch );
	    return;
	}

	if ( obj->value[ 2 ].v < 0 )
	{
	    send_to_char( "To nie mo`ze zosta`c zakluczone.\n\r", ch );
	    return;
	}

	if ( !has_key( ch, obj->value[ 2 ].v ) )
	{
	    send_to_char( "Brak ci klucza.\n\r", ch );
	    return;
	}

	if ( IS_SET( obj->value[ 1 ].v, CONT_LOCKED ) )
	{
	    send_to_char( "To ju`z jest zakluczone.\n\r", ch );
	    return;
	}

	SET_BIT( obj->value[ 1 ].v, CONT_LOCKED );
	send_to_char( "*Klik*\n\r", ch );
	if ( ( IS_NPC( ch ) && ch->pIndexData->vnum != MOB_VNUM_SUPERMOB ) /* Lam */
	  || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) ) ) /* Qwert */
	    act( ZM_WZROK, "$n zaklucza $j.", ch, obj, NULL, TO_ROOM );
	return;
    }

    act( ZM_ZAWSZE, "Nie widz`e tu niczego o nazwie \"$T\".", ch, NULL, arg, TO_CHAR );

    return;
}


KOMENDA( do_unlock )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       door;

    if ( ch->position == POS_FIGHTING )
    {
	STC( "No co ty, przecie`z ci`agle walczysz!\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co odkluczy`c?\n\r", ch );
	return;
    }

    door = find_door( ch, arg, TRUE );
    if ( door == -2 ) return;

    if ( !IS_SET( race_table[ ch->race ].race_abilities, RACE_OPENS_DOORS ) )
    {
	send_to_char( "Nie umiesz odklucza`c zamk`ow.\n\r", ch );
	return;
    }

    if ( door >= 0 )
    {
	/* "odklucz drzwi" */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[ door ];

	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "To nie jest zamkni`ete.\n\r", ch );
	    return;
	}

	if ( !can_see_room( ch, ch->in_room ) )
	{
	    send_to_char( "Nie widzisz zamka.\n\r", ch );
	    return;
	}

	if ( pexit->key < 0 )
	{
	    send_to_char( "To nie mo`ze zosta`c odkluczone.\n\r", ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_BEZZAMKA ) )
	{
	    send_to_char( "Z tej strony nie wida`c zamka.\n\r", ch );
	    return;
	}

	if ( !has_key( ch, pexit->key ) )
	{
	    send_to_char( "Brak ci klucza.\n\r", ch );
	    return;
	}

	if ( !IS_SET( pexit->exit_info, EX_LOCKED ) )
	{
	    send_to_char( "To ju`z jest odkluczone.\n\r", ch );
	    return;
	}

	REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char( "*Klik*\n\r", ch );
	if ( ( IS_NPC( ch ) && ch->pIndexData->vnum != MOB_VNUM_SUPERMOB ) /* Lam */
	  || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) ) ) /* Qwert */
	    act( ZM_WZROK, "$n odklucza $D.", ch, NULL, pexit->biernik, TO_ROOM );

	/* unlock the other side */
	if ( ( to_room   = pexit->to_room )
	  && ( pexit_rev = to_room->exit[ kierunki[ door ].odwrotny ] )
	  && pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
	eprog_unlock_trigger( ch, door ); /* Lam 13.7.98 */
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* "odklucz przedmiot" */
	if ( obj->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "To nie pojemnik.\n\r", ch );
	    return;
	}

	if ( !IS_SET( obj->value[ 1 ].v, CONT_CLOSED ) )
	{
	    send_to_char( "To nie jest zamkni`ete.\n\r", ch );
	    return;
	}

	if ( obj->value[ 2 ].v < 0 )
	{
	    send_to_char( "To nie mo`ze zosta`c odkluczone.\n\r", ch );
	    return;
	}

	if ( !has_key( ch, obj->value[ 2 ].v ) )
	{
	    send_to_char( "Brak ci klucza.\n\r", ch );
	    return;
	}

	if ( !IS_SET( obj->value[ 1 ].v, CONT_LOCKED ) )
	{
	    send_to_char( "To nie jest zakluczone.\n\r", ch );
	    return;
	}

	REMOVE_BIT( obj->value[ 1 ].v, CONT_LOCKED );
	send_to_char( "*Klik*\n\r", ch );
	if ( ( IS_NPC( ch ) && ch->pIndexData->vnum != MOB_VNUM_SUPERMOB ) /* Lam */
	  || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) ) ) /* Qwert */
	    act( ZM_WZROK, "$n odklucza $j.", ch, obj, NULL, TO_ROOM );
	return;
    }

    act( ZM_ZAWSZE, "Nie widz`e tu niczego o nazwie \"$T\".", ch, NULL, arg, TO_CHAR );

    return;
}


KOMENDA( do_pick )
{
    OBJ_DATA  *obj;
    CHAR_DATA *gch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        door;

    if ( ch->position == POS_FIGHTING )
    {
	STC( "No co ty, przecie`z ci`agle walczysz!\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Gdzie chcesz si`e w`lama`c?\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_pick_lock ].beats );

    door = find_door( ch, arg, TRUE );
    if ( door == -2 ) return;

    /* Check skill roll for player-char, make sure mob isn't charmed */
    if ( ( !IS_NPC( ch )
	&& number_percent( ) >= ch->pcdata->learned[ gsn_pick_lock ] )
      || ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) ) )
    {
	send_to_char( "Nie wysz`lo ci.\n\r", ch);
	return;
    }

    if ( !IS_SET( race_table[ ch->race ].race_abilities, RACE_OPENS_DOORS ) )
    {
	send_to_char( "Nie umiesz odklucza`c zamk`ow.\n\r", ch );
	return;
    }

    if ( door >= 0 )
    {
	/* "wlam drzwi" */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[ door ];

	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "To nie jest zamkni`ete.\n\r", ch );
	    return;
	}

	if ( !can_see_room( ch, ch->in_room ) )
	{
	    send_to_char( "Nie widzisz zamka.\n\r", ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_BEZZAMKA ) )
	{
	    send_to_char( "Z tej strony nie wida`c zamka.\n\r", ch );
	    return;
	}

	if ( pexit->key < 0 )
	{
	    send_to_char( "Tu nie mo`zesz si`e w`lama`c.\n\r", ch );
	    return;
	}

	if ( !IS_SET( pexit->exit_info, EX_LOCKED ) )
	{
	    send_to_char( "To ju`z jest odkluczone.\n\r", ch );
	    return;
	}

	/* look for guards */
	for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	{
	    if ( gch->deleted )
		continue;
	    if ( IS_NPC( gch ) && IS_AWAKE( gch )
	      && ch->level + 5 < gch->level )
	    {
		act( ZM_ZAWSZE, "$N stoi zbyt blisko zamka.", ch, NULL, gch, TO_CHAR );
		return;
	    }
	}

	if ( IS_SET( pexit->exit_info, EX_PICKPROOF ) )
	{
	    send_to_char( "Nie wysz`lo ci.\n\r", ch );
	    return;
	}

	REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char( "*Klik*\n\r", ch );
	if ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) /* Qwert */
	{
	    act( ZM_WZROK | ZM_WID_CHAR, "$n odklucza $D bez klucza!", ch, NULL, pexit->biernik, TO_ROOM );
	    act( ZM_WZROK | ZM_W_WID_CHAR, "$n odklucza $D.", ch, NULL, pexit->biernik, TO_ROOM );
	}
	uzyj( ch, gsn_pick_lock, "otwieraniu zamk`ow bez u`zycia klucza" );

	/* pick the other side */
	if ( ( to_room   = pexit->to_room )
	  && ( pexit_rev = to_room->exit[ kierunki[ door ].odwrotny ] )
	  && pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
	eprog_pick_trigger( ch, door ); /* Lam 13.7.98 */
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* "wlam przedmiot" */
	if ( obj->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "To nie pojemnik.\n\r", ch );
	    return;
	}
	if ( !IS_SET( obj->value[ 1 ].v, CONT_CLOSED ) )
	{
	    send_to_char( "To nie jest zamkni`ete.\n\r", ch );
	    return;
	}
	if ( obj->value[ 2 ].v < 0 )
	{
	    send_to_char( "Tego nie da si`e odkluczy`c.\n\r", ch );
	    return;
	}
	if ( !IS_SET( obj->value[ 1 ].v, CONT_LOCKED ) )
	{
	    send_to_char( "To ju`z jest odkluczone.\n\r", ch );
	    return;
	}

	/* look for guards */
	for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	{
	    if ( gch->deleted )
		continue;
	    if ( IS_NPC( gch ) && IS_AWAKE( gch )
	      && ch->level + 5 < gch->level )
	    {
		act( ZM_ZAWSZE, "$N stoi zbyt blisko zamka.", ch, NULL, gch, TO_CHAR );
		return;
	    }
	}

	if ( IS_SET( obj->value[ 1 ].v, CONT_PICKPROOF ) )
	{
	    send_to_char( "Nie wysz`lo ci.\n\r", ch );
	    return;
	}

	REMOVE_BIT( obj->value[ 1 ].v, CONT_LOCKED );
	send_to_char( "*Klik*\n\r", ch );
	if ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) /* Qwert */
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n w`lamuje si`e do $g.", ch, obj, NULL, TO_ROOM );
	uzyj( ch, gsn_pick_lock, "otwieraniu zamk`ow bez u`zycia klucza" );
	return;
    }

    act( ZM_ZAWSZE, "Nie widz`e tu niczego o nazwie \"$T\".", ch, NULL, arg, TO_CHAR );

    return;
}


KOMENDA( do_stand )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED( ch, AFF_SLEEP ) )
	{
	    send_to_char( "Nie mo`zesz si`e obudzi`c!\n\r", ch );
	    return;
	}

	send_to_char( "Budzisz si`e i wstajesz.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n budzi si`e i wstaje.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	/* Lam 5.6.98: "patrz" na prosbe gracza, niestety nie pamietam kogo... */
	do_look( ch, "auto" );
	komunikaty_ustroju( ch );
	break;

    case POS_RESTING:
	if ( CZY_WODA( ch->in_room )
	  || CZY_POWIETRZE( ch->in_room ) )
	    send_to_char( "Obracasz si`e do pionu.\n\r", ch );
	else
	    send_to_char( "Podnosisz si`e z ziemi.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wstaje.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_FIGHTING:
	send_to_char( "Ju`z walczysz, wi`ec nie wstaniesz bardziej!\n\r", ch );
	break;

    case POS_STANDING:
	send_to_char( "Ju`z stoisz.\n\r", ch );
	break;
    }

    return;
}


KOMENDA( do_rest )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "Budzisz si`e, ale nie wstajesz.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n budzi si`e, ale nie wstaje.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	komunikaty_ustroju( ch );
	break;

    case POS_RESTING:
	send_to_char( "Ju`z odpoczywasz.\n\r", ch );
	break;

    case POS_FIGHTING:
	send_to_char( "Nie w czasie walki!\n\r", ch );
	break;

    case POS_STANDING:
	PRZERWIJ_KOPANIE( ch, TRUE );
	if ( CZY_WODA( ch->in_room )
	  || CZY_POWIETRZE( ch->in_room ) )
	    send_to_char( "Obracasz si`e do poziomu, pozwalaj`ac sobie na chwil`e odpoczynku.\n\r", ch );
	else
	    send_to_char( "Walisz si`e na ziemi`e i odpoczywasz.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n odpoczywa.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	break;
    }

    return;
}


KOMENDA( do_sleep )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "Ju`z `spisz.\n\r", ch );
	break;

    case POS_RESTING:
    case POS_STANDING:
	PRZERWIJ_KOPANIE( ch, TRUE );
	PRZERWIJ_KARMPOJ( ch, TRUE );
	send_to_char( "Zasypiasz.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zasypia.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SLEEPING;
	/* sprawdza czy przypadkiem zasypiajaca postac nie byla w powietrzu
	   i nie latala dzieki skrzydlom */
	check_airchar( ch );
	break;

    case POS_FIGHTING:
	send_to_char( "Nie w czasie walki!\n\r", ch );
	break;
    }

    return;
}


KOMENDA( do_wake )
{
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	do_stand( ch, argument );
	return;
    }

    if ( !IS_AWAKE( ch ) )
    {
	ASTC( "Sam$a przecie`z `spisz!", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    /* ogromna dziura: IS_AWAKE( victim ), dawalo sie budzic prawie martwych */
    if ( victim->position != POS_SLEEPING )
    {
	act( ZM_ZAWSZE, "$N nie `spi.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( IS_AFFECTED( victim, AFF_SLEEP ) )
    {
	act( ZM_ZAWSZE, "Robisz co mo`zesz, ale $N si`e nie budzi!", ch, NULL, victim, TO_CHAR );
	return;
    }

    victim->position = POS_STANDING;
    act( ZM_ZAWSZE, "Budzisz $#.", ch, NULL, victim, TO_CHAR );
    act( ZM_PRZYT, "$n ci`e budzi.", ch, NULL, victim, TO_VICT );
    /* Lam budzi kogos, ktos budzi Lama, ale nigdy: ktos budzi kogos */
    act( ZM_WZROK | ZM_WID_CHAR, "$n budzi $#.", ch, NULL, victim, TO_NOTVICT );
    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT, "$n budzi $#.", ch, NULL, victim, TO_NOTVICT );
    komunikaty_ustroju( victim );

    return;
}


KOMENDA( do_sneak )
{
    AFFECT_DATA af;

    /* Don't allow charmed mobs to do this, check player's skill */
    if ( !moze_uzyc( ch, gsn_sneak ) )
    {
	send_to_char( "Nie umiesz si`e skrada`c.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 6 );
    send_to_char( "Pr`obujesz porusza`c si`e cicho.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if ( IS_NPC( ch ) || number_percent( ) <= ch->pcdata->learned[ gsn_sneak ] )
    {
	STC( "Skradasz si`e.\n\r", ch );
	uzyj( ch, gsn_sneak, "skradaniu si`e" );
	af.type      = gsn_sneak;
	af.duration  = ch->level / 3 + 1; /* Lam 2.6.2004 z ch->level */
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_SNEAK );
	af.level     = ch->level;
	af.caster    = ch;
	affect_to_char( ch, &af );
    }
    else
       STC( "Nie uda`lo ci si`e.\n\r", ch );

    return;
}


KOMENDA( do_hide )
{
    /* Dont allow charmed mobiles to do this, check player's skill */
    if ( !moze_uzyc( ch, gsn_hide ) )
    {
	send_to_char( "Nie umiesz si`e ukrywa`c.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	STC( "Nie w czasie walki.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_hide ].beats );
    send_to_char( "Pr`obujesz si`e ukry`c.\n\r", ch );

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
	REMOVE_BIT_V( ch->affected_by, AFF_HIDE );

    if ( IS_NPC( ch ) || number_percent( ) <= ch->pcdata->learned[ gsn_hide ] )
    {
	STC( "Ukrywasz si`e.\n\r", ch );
	SET_BIT_V( ch->affected_by, AFF_HIDE );
	uzyj( ch, gsn_hide, "ukrywaniu si`e" );
    }
    else
	STC( "Nie uda`lo ci si`e.\n\r", ch );

    return;
}


/*
 * Contributed by Alander.
 */
KOMENDA( do_visible )
{
    bool niewidzialny = FALSE;

    if ( IS_AFFECTED( ch, AFF_INVISIBLE )
      || IS_AFFECTED( ch, AFF_SNEAK ) )
	niewidzialny = TRUE;

    affect_strip( ch, gsn_invis );
    affect_strip( ch, gsn_mass_invis );
    affect_strip( ch, gsn_sneak );
    affect_strip( ch, gsn_shadow );
    REMOVE_BIT_V( ch->affected_by, AFF_INVISIBLE );
    REMOVE_BIT_V( ch->affected_by, AFF_SNEAK );

    if ( niewidzialny )
	send_to_char( "Teraz wszyscy ci`e widz`a.\n\r", ch );
    else
	ASTC( "Bardziej widzialn$y by`c nie mo`zesz.", ch );

    return;
}


KOMENDA( do_recall )
{
    if ( ch->level >= ( ( STREFA( ch ) == znajdz_strefe( "`Swiat mroku" ) ) ?
	25 : 50 ) && ch->level < 100 )
    {
	ASTC( "Sam$a znajd`x drog`e, jeste`s wystarczaj`aco do`swiadczon$y.", ch );
	return;
    }
    real_recall( ch, argument );

    return;
}


KOMENDA( real_recall )
{
    ROOM_INDEX_DATA *location;
    char             buf[ MAX_STRING_LENGTH ];
    int              place;

    PRZERWIJ_KOPANIE( ch, TRUE );
    PRZERWIJ_KARMPOJ( ch, TRUE );

    act( ZM_WZROK | ZM_SLUCH, "$n modli si`e o przeniesienie.", ch, NULL, NULL, TO_ROOM );

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
	|| IS_AFFECTED( ch, AFF_CURSE ) )
    {
	send_to_char( "Bogowie chyba ci`e opu`scili.\n\r", ch );
	return;
    }

    place = PRZYWOLANIE( ch );
    if ( !( location = get_room_index( place ) ) )
    {
	ASTC( "Kompletnie si`e zagubi`l$o`s.", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( ch->fighting )
    {
	int lose;

	if ( number_bits( 1 ) == 0 )
	{
	    WAIT_STATE( ch, 4 );
	    lose = ( ch->desc ) ? 25 : 50;
	    gain_exp( ch, 0 - lose );
	    sprintf( buf, "Nie uda`lo ci si`e! Tracisz %d punkt`ow do`swiadczenia.\n\r", lose );
	    send_to_char( buf, ch );
	    return;
	}

	lose = ( ch->desc ) ? 50 : 100;
	gain_exp( ch, 0 - lose );
	sprintf( buf, "Przeniesienie z walki kosztuje ci`e %d punkt`ow do`swiadczenia.\n\r", lose );
	send_to_char( buf, ch );
    }

    /* poza powyzszym warunkiem na wypadek chwilowej walki jednostronnej */
    stop_fighting( ch, TRUE );

    ch->move /= 2;
    act( ZM_WZROK | ZM_WID_CHAR, "$n znika.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( ZM_WZROK | ZM_WID_CHAR, "$n pojawia si`e.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    return;
}


KOMENDA( do_train )
{
    CHAR_DATA *mob;
    char      *pOutput;
    char      *pOut1;
    char      *pOut2;
    char      *pOut3;
    char       buf[ MAX_STRING_LENGTH ];
    bool       ok = FALSE;
    int       *pAbility;
    int        cost;
    int        money;
    int        bone_flag = 0; /* Added for training of hp ma mv */
    int        wybor = 0;

    if ( IS_NPC( ch ) )
	return;

    /*
     * Check for trainer.
     */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	if ( IS_NPC( mob ) && IS_AWAKE( mob ) && IS_SET( mob->act, ACT_TRAIN ) )
	    break;

    if ( !mob )
    {
	send_to_char( "Tu nie mo`zesz trenowa`c.\n\r", ch );
	return;
    }

    if ( argument[ 0 ] == '\0' )
    {
	int licz = liczba( ch->practice );
	sprintf( buf, "Masz %d sesj%s `cwicze`n.\n\r", ch->practice,
		 licz == 1 ? "`e" : licz == 2 ? "e" : "i" );
	send_to_char( buf, ch );
	argument = "foo";
    }

    cost  = 5;
    money = WYDATEK( ch->level, 80 );
    if ( ch->level <= 5 )
	money = ch->level * 80;

    if ( !str_prefix( argument, "si`la" ) )
    {
	if ( class_table[ ch->klasa ].attr_prime == APPLY_STR )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_str;
	pOutput     = "strength";
	pOut1       = "Twoja si`la ro`snie.\n\r";
	pOut2       = "Twoja si`la osi`agn`e`la ju`z maksimum.\n\r";
	pOut3       = "Si`la $1 ro`snie.";
	wybor       = 1;
    }

    else if ( !str_prefix( argument, "inteligencja" ) )
    {
	if ( class_table[ ch->klasa ].attr_prime == APPLY_INT )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_int;
	pOutput     = "intelligence";
	pOut1       = "Twoja inteligencja ro`snie.\n\r";
	pOut2       = "Twoja inteligencja osi`agn`e`la ju`z maksimum.\n\r";
	pOut3       = "Inteligencja $1 ro`snie.";
	wybor       = 2;
    }

    else if ( !str_prefix( argument, "m`adro`s`c" ) )
    {
	if ( class_table[ ch->klasa ].attr_prime == APPLY_WIS )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_wis;
	pOutput     = "wisdom";
	pOut1       = "Twoja m`adro`s`c ro`snie.\n\r";
	pOut2       = "Twoja m`adro`s`c osi`agn`e`la ju`z maksimum.\n\r";
	pOut3       = "M`adro`s`c $1 ro`snie.";
	wybor       = 3;
    }

    else if ( !str_prefix( argument, "zr`eczno`s`c" ) )
    {
	if ( class_table[ ch->klasa ].attr_prime == APPLY_DEX )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_dex;
	pOutput     = "dexterity";
	pOut1       = "Twoja zr`eczno`s`c ro`snie.\n\r";
	pOut2       = "Twoja zr`eczno`s`c osi`agn`e`la ju`z maksimum.\n\r";
	pOut3       = "Zr`eczno`s`c $1 ro`snie.";
	wybor       = 4;
    }

    else if ( !str_prefix( argument, "budowa" ) )
    {
	if ( class_table[ ch->klasa ].attr_prime == APPLY_CON )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_con;
	pOutput     = "constitution";
	pOut1       = "Twoje cia`lo si`e rozrasta.\n\r";
	pOut2       = "Twoje cia`lo ma ju`z maksymalne gabaryty.\n\r";
	pOut3       = "Cia`lo $1 si`e rozrasta.";
	wybor       = 5;
    }

    /* ---------------- By Bonecrusher ------------------- */

    else if ( !str_prefix( argument, "`zycie" ) )
    {
	    cost    = 1;
	    money   = WYDATEK( ch->level, 18 );
	bone_flag   = 1;
	pAbility    = &ch->max_hit;
	pOutput     = "hit points";
	pOut1       = "Twoje punkty `zycia rosn`a.\n\r";
	pOut2       = "Twoje punkty `zycia co`s skwasi`ly!\n\r";
	pOut3       = "Punkty `zycia $1 rosn`a.";
    }

    else if ( !str_prefix( argument, "mana" ) )
    {
	    cost    = 1;
	    money   = WYDATEK( ch->level, 18 );
	bone_flag   = 1;
	pAbility    = &ch->max_mana;
	pOutput     = "mana points";
	pOut1       = "Rosn`a twoje punkty magii.\n\r";
	pOut2       = "Mana ci co`s kwasi.\n\r";
	pOut3       = "Punkty magii $1 rosn`a.";
    }

    else if ( !str_prefix( argument, "ruch" ) )
    {
	    cost    = 1;
	    money   = WYDATEK( ch->level, 18 );
	bone_flag   = 2;
	pAbility    = &ch->max_move;
	pOutput     = "move points";
	pOut1       = "Rosn`a twoje punkty ruchu.\n\r";
	pOut2       = "Twoje ruchy ostro kwasz`a!.\n\r";
	pOut3       = "Punkty ruchu $1 rosn`a.";
    }

    /* --------------------------------------------*/

    else
    {
	strcpy( buf, "Mo`zesz trenowa`c:" );
	if ( ch->pcdata->perm_str < 18 + race_table[ ch->race ].str_mod )
	    strcat( buf, " si`l`e" );
	if ( ch->pcdata->perm_int < 18 + race_table[ ch->race ].int_mod )
	    strcat( buf, " inteligencj`e" );
	if ( ch->pcdata->perm_wis < 18 + race_table[ ch->race ].wis_mod )
	    strcat( buf, " m`adro`s`c" );
	if ( ch->pcdata->perm_dex < 18 + race_table[ ch->race ].dex_mod )
	    strcat( buf, " zr`eczno`s`c" );
	if ( ch->pcdata->perm_con < 18 + race_table[ ch->race ].con_mod )
	    strcat( buf, " budow`e" );

	strcat( buf, " `zycie man`e ruch" );

	if ( buf[ strlen( buf ) - 1 ] != ':' )
	{
	    strcat( buf, ".\n\r" );
	    send_to_char( buf, ch );
	    sprintf( buf, "P`lacisz %s za atrybuty.\n\r", ZLOTO_B( money ) );
	    send_to_char( buf, ch );
	    money   = WYDATEK( ch->level, 18 );
	    sprintf( buf, "P`lacisz %s za `zycie/man`e/ruchy.\n\r",
		    ZLOTO_B( money ) );
	    send_to_char( buf, ch );
	}

	return;
    }

    if ( bone_flag == 0 )
    {
	     if ( wybor == 1 && *pAbility < 18 + race_table[ ch->race ].str_mod )
		ok = TRUE;
	else if ( wybor == 2 && *pAbility < 18 + race_table[ ch->race ].int_mod )
		ok = TRUE;
	else if ( wybor == 3 && *pAbility < 18 + race_table[ ch->race ].wis_mod )
		ok = TRUE;
	else if ( wybor == 4 && *pAbility < 18 + race_table[ ch->race ].dex_mod )
		ok = TRUE;
	else if ( wybor == 5 && *pAbility < 18 + race_table[ ch->race ].con_mod )
		ok = TRUE;

	if ( !ok )
	{
	    send_to_char( pOut2, ch );
	    return;
	}
    }

    if ( cost > ch->practice )
    {
	send_to_char( "Nie wystarczy ci sesji `cwicze`n.\n\r", ch );
	return;
    }
    else if ( money > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    ch->practice                -= cost;
    ch->gold                    -= money;

    if ( bone_flag == 0 )
	*pAbility               += 1;
    else if ( bone_flag == 1 )
	*pAbility               += 2; /* Lam: bylo dice( 1, 2 ); */
    else
	*pAbility               += dice( 1, 4 ); /* bylo 1k5 */

    send_to_char( pOut1, ch );
    /* czy to widac? */
    act( ZM_WZROK | ZM_WID_CHAR | ZM_DYLEMAT, pOut3, ch, NULL, pOutput, TO_ROOM );

    return;
}


KOMENDA( do_chameleon )
{
    if ( !IS_NPC( ch )
	&& !moze_uzyc( ch, gsn_chameleon ) )
    {
	send_to_char( "Nie potrafisz zlewa`c si`e z otoczeniem.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	STC( "Nie w czasie walki.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_chameleon ].beats );
    send_to_char( "Usi`lujesz zla`c si`e z otoczeniem.\n\r", ch );

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
	REMOVE_BIT_V( ch->affected_by, AFF_HIDE );

    if ( IS_NPC( ch ) || number_percent( ) <= ch->pcdata->learned[ gsn_chameleon ] )
    {
	STC( "Zlewasz si`e z otoczeniem.\n\r", ch );
	SET_BIT_V( ch->affected_by, AFF_HIDE );
	uzyj( ch, gsn_chameleon, "ukrywaniu si`e" );
    }
    else
	STC( "Nie wysz`lo ci.\n\r", ch );

    return;
}


KOMENDA( do_heighten )
{
    AFFECT_DATA af;
    int used = 0; /* Lam 21.9.2002, to nie bylo inicjalizowane i zawsze "wychodzilo" */

    if ( !IS_NPC( ch )
	&& !moze_uzyc( ch, gsn_heighten ) )
    {
	send_to_char( "Nie wiesz jak wyostrzy`c swe zmys`ly.\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_heighten ) )
    {
	STC( "Masz ju`z wyostrzone zmys`ly.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_heighten ].beats );
    if ( IS_NPC( ch ) || number_percent( ) <= ch->pcdata->learned[ gsn_heighten ] )
    {
	if ( !HAS_DETECT_INVIS( ch ) )
	{
	    af.type      = gsn_heighten;
	    af.duration  = 24;
	    af.modifier  = 0;
	    af.location  = APPLY_NONE;
	    CLEAR_BITS( af.bitvector );
	    SET_BIT_V( af.bitvector, AFF_DETECT_INVIS );
	    af.level = ch->level;
	    af.caster = ch;
	    affect_to_char( ch, &af );

	    used += 50;
	}

	if ( !HAS_DETECT_HIDDEN( ch ) )
	{
	    af.type      = gsn_heighten;
	    af.duration  = 24;
	    af.modifier  = 0;
	    af.location  = APPLY_NONE;
	    CLEAR_BITS( af.bitvector );
	    SET_BIT_V( af.bitvector, AFF_DETECT_HIDDEN );
	    af.level = ch->level;
	    af.caster = ch;
	    affect_to_char( ch, &af );

	    used += 50;
	}

	if ( !HAS_INFRAVISION( ch ) )
	{
	    af.type      = gsn_heighten;
	    af.duration  = 24;
	    af.modifier  = 0;
	    af.location  = APPLY_NONE;
	    CLEAR_BITS( af.bitvector );
	    SET_BIT_V( af.bitvector, AFF_INFRARED );
	    af.level = ch->level;
	    af.caster = ch;
	    affect_to_char( ch, &af );

	    used += 50;
	}

	if ( used > 0 )
	{
	    send_to_char( "Wyostrzaj`a si`e twoje zmys`ly.\n\r", ch );
	    uzyj( ch, gsn_heighten, "wyostrzaniu swych zmys`l`ow" );
	}
	else
	    STC( "Nie mo`zesz bardziej wyostrzy`c swoich zmys`l`ow.\n\r", ch );
    }
    else
	STC( "Nie wysz`lo ci.\n\r", ch );

    return;
}


KOMENDA( do_shadow )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& !moze_uzyc( ch, gsn_shadow ) )
    {
	send_to_char( "Nie umiesz porusza`c si`e jak cie`n.\n\r", ch );
	return;
    }

    send_to_char( "Pr`obujesz porusza`c si`e jak cie`n.\n\r", ch );
    affect_strip( ch, gsn_shadow );

    WAIT_STATE( ch, skill_table[ gsn_shadow ].beats );
    if ( IS_NPC( ch ) || number_percent( ) <= ch->pcdata->learned[ gsn_shadow ] )
    {
	STC( "Poruszasz si`e jak cie`n.\n\r", ch );
	uzyj( ch, gsn_shadow, "poruszaniu si`e pod postaci`a cienia" );
	af.type      = gsn_shadow;
	af.duration  = ch->level;
	af.modifier  = APPLY_NONE;
	af.location  = 0;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_SNEAK );
	af.level = ch->level;
	af.caster = ch;
	affect_to_char( ch, &af );
    }
    else
	STC( "Nie wysz`lo ci.\n\r", ch );

    return;
}


/*
 * Bash code by Thelonius for EnvyMud (originally bash_door)
 * Damage modified using Morpheus's code
 * Message for bashproof doors by that wacky guy Kahn
 */
KOMENDA( do_bash )
{
    CHAR_DATA *gch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        door;
    ROOM_INDEX_DATA *in_room = ch->in_room;

    if ( IS_NPC( ch ) || ( !IS_NPC( ch )
			&& !moze_uzyc( ch, gsn_bash ) ) )
    {
	if ( ch->sex == 2 )
	    STC( "Za s`laba z ciebie wojowniczka, `zeby`s mog`la wywa`za`c drzwi!\n\r", ch );
	else
	    STC( "Za s`laby z ciebie wojownik, `zeby`s m`og`l wywa`za`c drzwi!\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co wywa`zy`c?\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Najpierw sko`ncz walk`e.\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg, TRUE ) ) >= 0 )
    {
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	int              chance;

	pexit = ch->in_room->exit[ door ];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "Uspok`oj si`e. To ju`z jest otwarte.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, skill_table[ gsn_bash ].beats );

	if ( !IS_NPC( ch ) )
	  chance = ch->pcdata->learned[ gsn_bash ] / 2;
	else
	  chance = 0;

	if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
	    chance /= 2;

	if ( IS_SET( pexit->exit_info, EX_BASHPROOF ) )
	{
	    if ( pexit->rodzaj == 0 )
	    {
		act( ZM_ZAWSZE | ZM_DYLEMAT, "`LUBUDU! Usi`lujesz wywa`zy`c $D, ale to nawet nie drgnie.",
		    ch, NULL, pexit->biernik, TO_CHAR );
		act( ZM_WZROK, "`LUBUDU! $n usi`luje wywa`zy`c $D, ale to nawet nie drgnie.",
		    ch, NULL, pexit->biernik, TO_ROOM );
	    }
	    else if ( pexit->rodzaj == 1 )
	    {
		act( ZM_ZAWSZE | ZM_DYLEMAT, "`LUBUDU! Usi`lujesz wywa`zy`c $D, ale on nawet nie drgnie.",
		    ch, NULL, pexit->biernik, TO_CHAR );
		act( ZM_WZROK, "`LUBUDU! $n usi`luje wywa`zy`c $D, ale on nawet nie drgnie.",
		    ch, NULL, pexit->biernik, TO_ROOM );
	    }
	    else if ( pexit->rodzaj == 2 )
	    {
		act( ZM_ZAWSZE | ZM_DYLEMAT, "`LUBUDU! Usi`lujesz wywa`zy`c $D, ale ona nawet nie drgnie.",
		    ch, NULL, pexit->biernik, TO_CHAR );
		act( ZM_WZROK, "`LUBUDU! $n usi`luje wywa`zy`c $D, ale ona nawet nie drgnie.",
		    ch, NULL, pexit->biernik, TO_ROOM );
	    }
	    else
	    {
		act( ZM_ZAWSZE | ZM_DYLEMAT, "`LUBUDU! Usi`lujesz wywa`zy`c $D, ale one nawet nie drgn`a.",
		    ch, NULL, pexit->biernik, TO_CHAR );
		act( ZM_WZROK, "`LUBUDU! $n usi`luje wywa`zy`c $D, ale one nawet nie drgn`a.",
		    ch, NULL, pexit->biernik, TO_ROOM );
	    }
	    damage( ch, ch, ( ch->max_hit /  5 ), gsn_bash, WEAR_NONE, TRUE );
	    return;
	}

	if ( chance
	  && get_curr_str( ch ) >= 20
	  && number_percent( ) <
		( chance + 4 * ( get_curr_str( ch ) - 20 ) ) )
	{
	    /* Success */

	    REMOVE_BIT( pexit->exit_info, EX_CLOSED );
	    if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
		REMOVE_BIT( pexit->exit_info, EX_LOCKED );

	    SET_BIT( pexit->exit_info, EX_BASHED );

	    act( ZM_ZAWSZE | ZM_DYLEMAT, "Trach! Wywa`zasz $D!",
		ch, NULL, pexit->biernik, TO_CHAR );
	    act( ZM_WZROK, "$n wywa`za $D!",
		ch, NULL, pexit->biernik, TO_ROOM );
	    uzyj( ch, gsn_bash, "wywa`zaniu drzwi" );

	    damage( ch, ch, ( ch->max_hit / 20 ), gsn_bash, WEAR_NONE, TRUE );

	    /* Alandar 23.10.2004: sprawdza czy postac nie wywazyla sobie badz
	       komus drzwi spod nog */
	    if ( door == DIR_DOWN )
	    {
		CHAR_DATA *rch;
		OBJ_DATA *robj;

		for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
		    check_airchar( rch );
		for ( robj = ch->in_room->contents; robj; robj = robj->next_content )
		    new_airobj( robj );
	    }

	    /* Bash through the other side */
	    if ( ( to_room   = pexit->to_room )
	      && ( pexit_rev = to_room->exit[ kierunki[ door ].odwrotny ] )
	      && pexit_rev->to_room == ch->in_room )
	    {
		CHAR_DATA *rch;
		OBJ_DATA *robj;

		REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
		if ( IS_SET( pexit_rev->exit_info, EX_LOCKED ) )
		    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );

		SET_BIT( pexit_rev->exit_info, EX_BASHED );

		for ( rch = to_room->people; rch; rch = rch->next_in_room )
		{
		    if ( rch->deleted )
			continue;
		    if ( pexit_rev->rodzaj == 0 )
			act( ZM_WZROK, "$D zostaje wywa`zone!",
			    rch, NULL, pexit_rev->short_descr, TO_CHAR );
		    else if ( pexit_rev->rodzaj == 1 )
			act( ZM_WZROK, "$D zostaje wywa`zony!",
			    rch, NULL, pexit_rev->short_descr, TO_CHAR );
		    else if ( pexit_rev->rodzaj == 2 )
			act( ZM_WZROK, "$D zostaje wywa`zona!",
			    rch, NULL, pexit_rev->short_descr, TO_CHAR );
		    else
			act( ZM_WZROK, "$D zostaj`a wywa`zone!",
			    rch, NULL, pexit_rev->short_descr, TO_CHAR );
		    /* Alandar 23.10.2004: sprawdza czy postac nie stracila
		       gruntu pod nogami */
		    check_airchar( rch );
		}
		if ( door == DIR_UP )
		    for ( robj = to_room->contents; robj; robj = robj->next_content )
			new_airobj( robj );

		eprog_bash_trigger( ch, door ); /* Lam 21.7.98 */
		if ( ch->deleted )
		    return;
	    }
	}
	else
	{
	    /* Failure */

	    if ( pexit->rodzaj == 0 )
	    {
		act( ZM_ZAWSZE | ZM_DYLEMAT, "AU`C! Walisz barkiem w $D, ale ono nawet nie drgnie.",
		    ch, NULL, pexit->biernik, TO_CHAR );
		act( ZM_WZROK, "$n wyje`zd`za z bara w $D, ale ono si`e trzyma.",
		    ch, NULL, pexit->biernik, TO_ROOM );
	    }
	    else if ( pexit->rodzaj == 1 )
	    {
		act( ZM_ZAWSZE | ZM_DYLEMAT, "AU`C! Walisz barkiem w $D, ale on nawet nie drgnie.",
		    ch, NULL, pexit->biernik, TO_CHAR );
		act( ZM_WZROK, "$n wyje`zd`za z bara w $D, ale on si`e trzyma.",
		    ch, NULL, pexit->biernik, TO_ROOM );
	    }
	    else if ( pexit->rodzaj == 2 )
	    {
		act( ZM_ZAWSZE | ZM_DYLEMAT, "AU`C! Walisz barkiem w $D, ale ona nawet nie drgnie.",
		    ch, NULL, pexit->biernik, TO_CHAR );
		act( ZM_WZROK, "$n wyje`zd`za z bara w $D, ale ona si`e trzyma.",
		    ch, NULL, pexit->biernik, TO_ROOM );
	    }
	    else
	    {
		act( ZM_ZAWSZE | ZM_DYLEMAT, "AU`C! Walisz barkiem w $D, ale one nawet nie drgn`a.",
		    ch, NULL, pexit->biernik, TO_CHAR );
		act( ZM_WZROK, "$n wyje`zd`za z bara w $D, ale one si`e trzymaj`a.",
		    ch, NULL, pexit->biernik, TO_ROOM );
	    }
	    damage( ch, ch, ( ch->max_hit / 10 ), gsn_bash, WEAR_NONE, TRUE );
	}
    }
    else
    {
	if ( door == -1 )
	    send_to_char( "Musisz wywa`zy`c drzwi.\n\r", ch );
	return;
    }

    /*
     * Check for "guards"... anyone bashing a door is considered as
     * a potential aggressor, and there's a 25% chance that mobs
     * will do unto before being done unto.
     * But first...let's make sure ch ain't dead?  That'd be a pain.
     */

    if ( ch->deleted || !ZDOLNY_DO_WALKI( ch ) || ch->in_room != in_room )
	return;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !gch->deleted
	  && gch != ch
	  && IS_AWAKE( gch )
	  && can_see( gch, ch )
	  && !gch->fighting
	  && ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
	  && ( ch->level - gch->level <= 4 )
	  && number_bits( 2 ) == 0 )
	    multi_hit( gch, ch, TYPE_UNDEFINED );
    }

    return;
}


/*
 * Ulryk 29.11.2004: pochwycenie przepisane od nowa.
 */
KOMENDA( do_snare )
{
    CHAR_DATA   *victim;
    AFFECT_DATA  af;
    char         arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( !moze_uzyc( ch, gsn_snare )
      || ( !IS_NPC( ch ) && !ch->pcdata->learned[ gsn_snare ] ) )
    {
	send_to_char( "Nie umiesz chwyta`c w sid`la.\n\r", ch );
	return;
    }

    if ( arg[ 0 ] == '\0' )
    {
	if ( !( victim = FIGHTING( ch ) ) )
	{
	    send_to_char( "Kogo pochwyci`c w sid`la?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	    return;
	}

	if ( ch->fighting && ( !is_fighting( ch, victim ) ) )
	{
	    send_to_char( "Zajmij si`e lepiej swoim przeciwnikiem!\n\r", ch );
	    return;
	}
    }

    /* Lam 7.3.2006, na sztywno <= 1 to moze slaby pomysl, ale moze juz nie
       bedziemy psuc tablicy ras (zreszta szybko gracze zglosza) */
    if ( ROZMIAR( victim ) <= 1 )
    {
	act( ZM_ZAWSZE, "$N jest tak ma`l$Y i zwrotn$Y, `ze rezygnujesz z pr`ob pochwycenia $M.",
		ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ( !is_affected( victim, gsn_snare )
	&& IS_AFFECTED( victim, AFF_HOLD ) )
      || IS_PASSING( victim ) )
    {
	send_to_char( "To niczego nie da.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    check_killer( ch, victim );

    WAIT_STATE( ch, skill_table[ gsn_snare ].beats );

    if ( ( IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
	|| ( ch == victim )
	|| ( !IS_NPC( ch )
	  && number_percent( ) <= ch->pcdata->learned[ gsn_snare ] )
	|| ( !IS_NPC( ch ) && !IS_NPC( victim ) && number_percent( )
	  <= ch->pcdata->learned[ gsn_snare ] / 2
	   - 2 * ( get_curr_dex( victim ) - get_curr_dex( ch ) ) ) )
    {
	affect_strip( victim, gsn_snare );

	af.type      = gsn_snare;
	af.duration  = 1 + ( ( ch->level ) / 8 );
	af.location  = APPLY_HITROLL;
	af.modifier  = -ch->level;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_HOLD );
	af.level = ch->level;
	af.caster = ch;

	affect_to_char( victim, &af );

	if ( ch != victim )
	{
	    act( ZM_ZAWSZE | ZM_WID_VICT, "Pochwyci`l$o`s $V w sid`la!", ch, NULL, victim, TO_CHAR );
	    act( ZM_ZAWSZE | ZM_W_WID_VICT, "Pochwyci`l$o`s przeciwnika w sid`la!", ch, NULL, victim, TO_CHAR );
	    act( ZM_PRZYT, "$n pochwyci`l$a ci`e w sid`la!", ch, NULL, victim, TO_VICT );
	    act( ZM_WZROK | ZM_WID_VICT, "$n pochwyci`l$a $# w sid`la.", ch, NULL, victim, TO_NOTVICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT, "$n chwyta w sid`la powietrze!", ch, NULL, victim, TO_NOTVICT );
	    uzyj( ch, gsn_snare, "chwytaniu przeciwnik`ow" );
	}
	else
	{
	    act( ZM_ZAWSZE, "Pochwyci`l$o`s si`e w sid`la!", ch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n zapl`ata`l$a si`e w sid`la.", ch, NULL, NULL, TO_ROOM );
	}
    }
    else
    {
	act( ZM_ZAWSZE, "Nie uda`lo ci si`e schwyta`c $M w sid`la.",
	    ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "$n pr`obowa`l$a schwyta`c ci`e w sid`la! `Lap $v!",
	    ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obowa`l$a usidli`c $#, jednak bezskutecznie!",
	    ch, NULL, victim, TO_NOTVICT );
    }

    if ( !is_fighting( ch, victim ) )
    {
	set_fighting( ch, victim );
	if ( ZDOLNY_DO_WALKI( victim )
	  && !is_fighting( victim, ch ) )
	{
	    set_fighting( victim, ch );
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	}
    }

    return;
}


/* Untangle by Thelonius for EnvyMud */
KOMENDA( do_untangle )
{
    CHAR_DATA   *victim;
    char         arg[ MAX_INPUT_LENGTH ];

    if ( !moze_uzyc( ch, gsn_untangle ) )
    {
	send_to_char( "Nie masz poj`ecia jak to zrobi`c.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
	victim = ch;
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    if ( !IS_AFFECTED( victim, AFF_HOLD ) )
    {
	if ( victim == ch )
	    ASTC( "Nie jeste`s pochwycon$y.", ch );
	else
	    act( ZM_ZAWSZE, "$N nie jest pochwycon$Y.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !is_affected( victim, gsn_snare ) )
    {
	STC( "Nie wiesz, jak sobie poradzi`c z takimi sid`lami.\n\r", ch );
	return;
    }

    if ( IS_NPC( ch )
      || number_percent( ) <= ch->pcdata->learned[ gsn_untangle ] )
    {
	affect_strip( victim, gsn_snare );

	if ( victim != ch )
	{
	    act( ZM_ZAWSZE, "Wyzwalasz $# z side`l.", ch, NULL, victim, TO_CHAR );
	    act( ZM_PRZYT, "$n wyzwala ci`e z side`l.", ch, NULL, victim, TO_VICT );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n wyzwala $# z side`l.", ch, NULL, victim, TO_NOTVICT );
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT, "$n wyzwala $# z side`l.", ch, NULL, victim, TO_NOTVICT );
	}
	else
	{
	    send_to_char( "Uwalniasz si`e z side`l.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n uwalnia si`e z side`l.", ch, NULL, NULL, TO_ROOM );
	}
	uzyj( ch, gsn_untangle, "wyzwalaniu si`e i innych" );
	WAIT_STATE( ch, skill_table[ gsn_untangle ].beats );
    }
    else
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	WAIT_STATE( ch, skill_table[ gsn_untangle ].beats / 2 );
    }

    return;
}


/*
 * Lam: Funkcja follow_enter to wersja funkcji do_enter bez podazania. Wyobrazmy
 * sobie sytuacje: A chodzi za B, B za C, a C za A. Wtedy A przechodzi przez
 * portal, za nim C, za nim B, a za nim A...
 * To rozwiazanie powoduje, ze tylko chodzacy za postacia, ktora akurat
 * przechodzi przez portal, ida za nia. Reszta musi to robic manualnie...
 * Wlasciwa funkcja do_enter znajduje sie ponizej.
 */
KOMENDA( follow_enter )
{
    ROOM_INDEX_DATA     *to_room, *old_room;
    OBJ_DATA            *obj;
    char                arg[ MAX_INPUT_LENGTH ];
    bool		brief = FALSE;

    if ( *argument == '\0' )
    {
	send_to_char( "W jaki portal chcesz wej`s`c?\n\r", ch );
	return;
    };

    one_argument( argument, arg );
    if ( !( obj = get_obj_list( ch, arg, ch->in_room->contents ) ) )
    {
	send_to_char( "Nie ma tu niczego takiego.\n\r", ch );
	return;
    };
    if ( obj->item_type != ITEM_PORTAL )
    {
	send_to_char( "To nie portal.\n\r", ch );
	return;
    };

    /* oczarowane moby bylyby uciazliwe, gdyby nie mogly przechodzic */
    if ( !IS_NPC( ch ) && obj->value[ 0 ].v > ch->level )
    {
	send_to_char( "Masz za niski poziom do wej`scia w ten portal.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje przej`s`c przez portal, ale ma za niski poziom.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    old_room = ch->in_room;
    if ( number_percent( ) < obj->value[ 2 ].v )
    {
	int i;
	ROOM_INDEX_DATA *t = NULL;

	/* Vigud 22.4.2010: 1000 prob znalezienia losowego pomieszczenia */
	for ( i = 1000; i; i-- )
	    if ( ( to_room = get_room_index( number_range( 0, 65535 ) ) ) )
		if ( !room_is_private( to_room )
		  && !IS_SET( to_room->room_flags, ROOM_NO_TELEPORT )
		  && !IS_SET( to_room->area->flags, AREA_NO_TELEPORT ) )
		{
		    t = to_room;
		    break;
		}

	to_room = t ? t : old_room;
    }
    else
	to_room = get_room_index( obj->value[ 1 ].v );

    if ( !to_room )
    {
	send_to_char( "Z tym portalem co`s jest nie w porz`adku... Boisz si`e wej`s`c.\n\r", ch );
	return;
    }
    if ( to_room == old_room )
    {
	send_to_char( "Ten portal donik`ad nie prowadzi.\n\r", ch );
	return;
    }

    if ( room_is_private( to_room ) || to_room->vnum == ROOM_VNUM_POZA_SWIATEM )
    {
	send_to_char( "Usi`lujesz wskoczy`c w portal, ale ten wyrzuca ci`e z powrotem!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje wskoczy`c w portal, ale ten wyrzuca $v z powrotem.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    act( ZM_WZROK | ZM_WID_CHAR, "$n przechodzi przez $j.", ch, obj, NULL, TO_ROOM );
    act( ZM_ZAWSZE, "Przechodzisz przez $j.", ch, obj, NULL, TO_CHAR );
    char_from_room( ch );
    char_to_room( ch, to_room );
    act( ZM_WZROK | ZM_WID_CHAR, "$n wyskakuje z $g.", ch, obj, NULL, TO_ROOM );
    if ( !IS_NPC( ch ) && ch->in_room->trap && !IS_IMMORTAL( ch ) )
    {
	if ( CZY_OPCJA( ch, OPC_BRIEF ) )
	{
	    brief = TRUE;
	    REMOVE_BIT( ch->pcdata->opcje, OPC_BRIEF );
	}
    }

    do_look( ch, "auto" );
    auto_hunt( ch ); /* Lam */

    if ( brief )
	SET_BIT( ch->pcdata->opcje, OPC_BRIEF );

    /* Lam 22.5.99 */
    if ( !IS_NPC( ch ) && ch->in_room->trap && is_affected( ch, gsn_search ) )
    {
	STC( "{GWchodz`ac ostro`znie do pomieszczenia odkrywasz, `ze znajduje si`e w nim{x\n\r", ch );
	STC( "{Gpu`lapka. Dzi`eki swym wyostrzonym zmys`lom jeste`s w stanie j`a omin`a`c.{x\n\r", ch );
    }
    else if ( !IS_NPC( ch ) && ch->in_room->trap && !IS_IMMORTAL( ch ) )
    {
	char buf [ MSL ];

	act( ZM_WZROK | ZM_DYLEMAT, "$n ginie na miejscu!", ch, NULL, NULL, TO_ROOM );
	send_to_char( przemiel_opis( ch, ch->in_room->trap->trap_string ), ch );
	sprintf( buf, "%s hit a DEATH TRAP in room %d!",
		     ch->name, ch->in_room->vnum );
	log_string( buf );
	sprintf( buf, "{R$N{x wpada w `smierteln`a pu`lapk`e w pomieszczeniu {Y#%d{x.", ch->in_room->vnum );
	wiznet( buf, ch, NULL, NULL, WIZ_DEATH, 0, 0 );
	/* mprog_death_trigger zbedny, to nie mob */
	death_cry( ch, 0, ch->position );
	/* zdecydowalem sie na raw_kill... nie wiem czy to dobrze */
	raw_kill( ch, ch, FALSE, FALSE, ZGON_ZWYKLY );
    }

    return;
}


/*
 * To wersja wykonywana przez interpret( )
 */
KOMENDA( do_enter )
{
    ROOM_INDEX_DATA     *to_room, *old_room;
    OBJ_DATA            *obj;
    char                arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA		*fch, *fch_next;
    bool		brief = FALSE;

    PRZERWIJ_KOPANIE( ch, TRUE );
    PRZERWIJ_KARMPOJ( ch, TRUE );

    if ( !*argument )
    {
	send_to_char( "W jaki portal chcesz wej`s`c?\n\r", ch );
	return;
    };

    one_argument( argument, arg );
    if ( !( obj = get_obj_list( ch, arg, ch->in_room->contents ) ) )
    {
	send_to_char( "Nie ma tu niczego takiego.\n\r", ch );
	return;
    };

    if ( obj->item_type != ITEM_PORTAL )
    {
	send_to_char( "To nie portal.\n\r", ch );
	return;
    };

    /* oczarowane moby bylyby uciazliwe, gdyby nie mogly przechodzic */
    if ( !IS_NPC( ch ) && obj->value[ 0 ].v > ch->level )
    {
	send_to_char( "Masz za niski poziom do wej`scia w ten portal.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje przej`s`c przez portal, ale ma za niski poziom.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    old_room = ch->in_room;
    if ( number_percent( ) < obj->value[ 2 ].v )
    {
	int i;
	ROOM_INDEX_DATA *t = NULL;

	/* Vigud 22.4.2010: 1000 prob znalezienia losowego pomieszczenia */
	for ( i = 1000; i; i-- )
	    if ( ( to_room = get_room_index( number_range( 0, 65535 ) ) ) )
		if ( !room_is_private( to_room )
		  && !IS_SET( to_room->room_flags, ROOM_NO_TELEPORT )
		  && !IS_SET( to_room->area->flags, AREA_NO_TELEPORT ) )
		{
		    t = to_room;
		    break;
		}

	to_room = t ? t : old_room;
    }
    else
	to_room = get_room_index( obj->value[ 1 ].v );

    if ( !to_room )
    {
	send_to_char( "Z tym portalem co`s jest nie w porz`adku... Boisz si`e wej`s`c.\n\r", ch );
	return;
    }

    if ( to_room == old_room )
    {
	send_to_char( "Ten portal donik`ad nie prowadzi.\n\r", ch );
	return;
    }

    if ( room_is_private( to_room ) || to_room->vnum == ROOM_VNUM_POZA_SWIATEM )
    {
	send_to_char( "Usi`lujesz wskoczy`c w portal, ale ten wyrzuca ci`e z powrotem!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje wskoczy`c w portal, ale ten wyrzuca $v z powrotem.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    act( ZM_WZROK | ZM_WID_CHAR, "$n przechodzi przez $j.", ch, obj, NULL, TO_ROOM );
    act( ZM_ZAWSZE, "Przechodzisz przez $j.", ch, obj, NULL, TO_CHAR );
    char_from_room( ch );
    char_to_room( ch, to_room );
    act( ZM_WZROK | ZM_WID_CHAR, "$n wyskakuje z $g.", ch, obj, NULL, TO_ROOM );

    if ( !IS_NPC( ch ) && ch->in_room->trap && !IS_IMMORTAL( ch ) )
    {
	if ( CZY_OPCJA( ch, OPC_BRIEF ) )
	{
	    brief = TRUE;
	    REMOVE_BIT( ch->pcdata->opcje, OPC_BRIEF );
	}
    }

    do_look( ch, "auto" );
    auto_hunt( ch ); /* Lam */

    if ( brief )
	SET_BIT( ch->pcdata->opcje, OPC_BRIEF );
    /*
     * Lam 22.5.99
     */
    if ( !IS_NPC( ch ) && ch->in_room->trap && is_affected( ch, gsn_search ) )
    {
	STC( "{GWchodz`ac ostro`znie do pomieszczenia odkrywasz, `ze znajduje si`e w nim{x\n\r", ch );
	STC( "{Gpu`lapka. Dzi`eki swym wyostrzonym zmys`lom jeste`s w stanie j`a omin`a`c.{x\n\r", ch );
    }
    else if ( !IS_NPC( ch ) && ch->in_room->trap && !IS_IMMORTAL( ch ) )
    {
	char buf [ MSL ];

	act( ZM_WZROK | ZM_DYLEMAT, "$n ginie na miejscu!", ch, NULL, NULL, TO_ROOM );
	send_to_char( przemiel_opis( ch, ch->in_room->trap->trap_string ), ch );
	sprintf( buf, "%s hit a DEATH TRAP in room %d!",
		     ch->name, ch->in_room->vnum );
	log_string( buf );
	sprintf( buf, "{R$N{x wpada w `smierteln`a pu`lapk`e w pomieszczeniu {Y#%d{x.", ch->in_room->vnum );
	wiznet( buf, ch, NULL, NULL, WIZ_DEATH, 0, 0 );
	/* mprog_death_trigger zbedny, to nie mob */
	death_cry( ch, 0, ch->position );
	/* zdecydowalem sie na raw_kill... nie wiem czy to dobrze */
	raw_kill( ch, ch, FALSE, FALSE, ZGON_ZWYKLY );
    }

    /* byc moze cala reszta rowniez powinna wpadac w pulapke */
    if ( ch->deleted )
	return;

    /* przechodzenie przez portal grupowo */
    for ( fch = old_room->people; fch; fch = fch_next )
    {
	fch_next = fch->next_in_room;

/*	if ( fch->master == ch && IS_AFFECTED( fch, AFF_CHARM )
	  && fch->position < POS_FIGHTING )
	    do_stand( fch, "" ); */

	if ( fch->master == ch && fch->position == POS_STANDING
	  && can_see_obj( fch, obj ) )
	{
	    if ( IS_NPC( fch ) && IS_SET( fch->act, ACT_AGGRESSIVE ) )
	    {
		act( ZM_ZAWSZE, "Nie mo`zesz zabra`c $! ze sob`a.", ch, NULL, fch, TO_CHAR );
		act( ZM_PRZYT, "$n nie mo`ze zabra`c ci`e ze sob`a.", ch, NULL, fch, TO_VICT );
		continue;
	    }
	    PRZERWIJ_KOPANIE( fch, TRUE );
	    PRZERWIJ_KARMPOJ( fch, TRUE );
	    act( ZM_PRZYT, "Pod`a`zasz za $4.", ch, NULL, fch, TO_VICT );
	    follow_enter( fch, argument );
	}
    }

    return;
}


/*
 * Lam 18.5.2005: wyciagniete z do_push, rozszerzone i uzyte w do_pull
 */
bool can_push_pull( CHAR_DATA *ch, CHAR_DATA *victim,
		ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to, int door )
{
    if ( victim->in_room != from )
	bug( "victim->in_room != from", 0 );

    if ( !from->exit[ door ]
      || from->exit[ door ]->to_room != to )
	bug( "from->exit[ door ]->to_room != to", 0 );

    if ( ( IS_NPC( victim )
	&& ( IS_SET( victim->act, ACT_TRAIN )
	  || IS_SET( victim->act, ACT_PRACTICE )
	  || IS_SET( victim->act, ACT_PEACEFUL ) /* Cadaver */
	  || victim->pIndexData->pShop
	  || victim->pIndexData->pHealer
	     /* ponizsze to uzdrowiciel, kat itp. */
	  || ( victim->spec_fun && IS_SET( victim->act, ACT_SENTINEL ) ) ) )
      || !victim->in_room
      || IS_SET( from->room_flags, ROOM_SAFE )
      || IS_SET( from->room_flags, ROOM_NO_RECALL )
      || IS_SET( to->room_flags, ROOM_NO_RECALL )
      || victim->position != POS_STANDING
/*    || is_safe( ch, victim ) */
      || victim->level >= ch->level + 5
/*    || ( !IS_NPC( victim ) && victim->level >= LEVEL_HERO ) */
      || ( to->trap && !IS_IMMORTAL( victim ) )
      || !can_move( victim, from, to, door, FALSE )
      || ( CZY_POWIETRZE( to ) && !IS_FLYING( victim ) ) )
	return FALSE;
    else
	return TRUE;
}


/*
 * Sciagniete
 */
KOMENDA( do_push )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    OBJ_DATA *obj;
    int door;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );
    victim = get_char_room( ch, arg1 );
    obj = get_obj_list( ch, arg1, ch->in_room->contents );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
    {
	send_to_char( "Kogo lub co wypchn`a`c w jakim kierunku?\n\r", ch );
	return;
    }

    if ( ( !victim || !can_see( ch, victim ) )
      && ( !obj || !can_see_obj( ch, obj ) ) )
    {
	send_to_char( "Kogo lub co wypchn`a`c w jakim kierunku?\n\r", ch );
	return;
    }

	 if ( !str_cmp( arg2, "n" ) ) door = 0;
    else if ( !str_cmp( arg2, "e" ) ) door = 1;
    else if ( !str_cmp( arg2, "s" ) ) door = 2;
    else if ( !str_cmp( arg2, "w" ) ) door = 3;
    else if ( !str_prefix( arg2, "p`o`lnoc" ) ) door = 0;
    else if ( !str_prefix( arg2, "wsch`od" ) ) door = 1;
    else if ( !str_prefix( arg2, "po`ludnie" ) ) door = 2;
    else if ( !str_prefix( arg2, "zach`od" ) ) door = 3;
    else if ( !str_prefix( arg2, "g`ora" ) ) door = 4;
    else if ( !str_prefix( arg2, "d`o`l" ) ) door = 5;
    else if ( !str_cmp( arg2, "ne" ) ) door = 6;
    else if ( !str_cmp( arg2, "se" ) ) door = 7;
    else if ( !str_cmp( arg2, "nw" ) ) door = 8;
    else if ( !str_cmp( arg2, "sw" ) ) door = 9;
    else
    {
	send_to_char( "W jakim kierunku chcesz pcha`c?\n\r", ch );
	return;
    }

    if ( obj )
    {
	in_room = obj->in_room;
	if ( ( pexit = in_room->exit[ door ] ) == NULL
	  || ( to_room = pexit->to_room ) == NULL
	  || IS_SET( pexit->exit_info, EX_BURIED ) )
	{
	    send_to_char( "Nie mo`zesz pcha`c w tym kierunku.\n\r", ch );
	    return;
	}

	if ( !IS_SET( obj->wear_flags, ITEM_TAKE ) && !IS_OBJ_STAT( obj, ITEM_POM ) )
	{
	    send_to_char( "Tego nie da si`e pcha`c.\n\r", ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    act( ZM_ZAWSZE | ZM_DYLEMAT, "Nie mo`zesz przepchn`a`c tego przez $D.", ch, NULL, pexit->biernik, TO_CHAR );
	    return;
	}

	if ( door == DIR_UP
	  && to_room->exit[ DIR_DOWN ]
	  && to_room->exit[ DIR_DOWN ]->to_room
	  && !IS_SET( to_room->exit[ DIR_DOWN ]->exit_info, EX_CLOSED )
	  && CZY_POWIETRZE( to_room ) )
	{
	    STC( "To i tak zaraz spadnie.\n\r", ch );
	    return;
	}

	act( ZM_ZAWSZE, "Usi`lujesz wypchn`a`c $j z pomieszczenia.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje wypchn`a`c $j z pomieszczenia.", ch, obj, NULL, TO_ROOM );

	if ( obj->weight > ( 2 * can_carry_w( ch ) ) )
	{
	    if ( obj->rodzaj == 1 )
		act( ZM_ZAWSZE, "$p jest dla ciebie za ci`e`zki.", ch, obj, NULL, TO_CHAR );
	    else if ( obj->rodzaj == 2 )
		act( ZM_ZAWSZE, "$p jest dla ciebie za ci`e`zka.", ch, obj, NULL, TO_CHAR );
	    else if ( obj->rodzaj == 3 )
		act( ZM_ZAWSZE, "$p s`a dla ciebie za ci`e`zkie.", ch, obj, NULL, TO_CHAR );
	    else
		act( ZM_ZAWSZE, "$p jest dla ciebie za ci`e`zkie.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n chce pcha`c $j, ale nie daje rady.", ch, obj, NULL, TO_ROOM );
	    return;
	}

	if ( !IS_IMMORTAL( ch )
	  && ( IS_SET( ch->in_room->room_flags, ROOM_SAFE )
	    || IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
	    || ( number_percent( ) > 75 ) ) )
	{
	    send_to_char( "To si`e nawet nie ruszy.\n\r", ch );
	    return;
	}

	if ( ch->move > 10 )
	{
	    ch->move -= 10;
	    send_to_char( "Udaje ci si`e!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n osi`aga sw`oj cel!", ch, NULL, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "$p wyje`zd`za z pomieszczenia.", ch, obj, NULL, TO_ROOM );
	    obj_from_room( obj );
	    obj_to_room( obj, to_room );
	}
	else
	{
	    ASTC( "Jeste`s zbyt zm`eczon$y, `zeby cokolwiek pcha`c.", ch );
	}
    }
    else
    {
	if ( ch == victim )
	{
	    act( ZM_ZAWSZE, "Usi`lujesz pcha`c sam$a siebie, wygl`adaj`ac `smiesznie.", ch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n postanawia si`e o`smieszy`c i sam$a siebie popycha.", ch, NULL, NULL, TO_ROOM );
	    return;
	}

	in_room = victim->in_room;
	if ( ( pexit   = in_room->exit[ door ] ) == NULL
	  || ( to_room = pexit->to_room ) == NULL
	  || IS_SET( pexit->exit_info, EX_BURIED ) )
	{
	    send_to_char( "Nie mo`zesz pcha`c w tym kierunku.\n\r", ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_CLOSED )
	  && ( !IS_PASSING( victim )
	    || IS_SET( pexit->exit_info, EX_PASSPROOF ) ) )
	{
	    act( ZM_ZAWSZE, "$N nie daje si`e przepchn`a`c przez zamkni`ete przej`scie.", ch, NULL, victim, TO_CHAR );
	    act( ZM_PRZYT, "$n pcha ci`e woko`lo, ale nie mo`ze nigdzie przepchn`a`c.", ch, NULL, victim, TO_VICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT | ZM_DYLEMAT, "$n pcha $#, nie mog`ac nigdzie wypchn`a`c!", ch, NULL, victim, TO_NOTVICT );
	    return;
	}

	act( ZM_ZAWSZE, "Usi`lujesz wypchn`a`c $# z pomieszczenia.", ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "$n usi`luje wypchn`a`c ci`e z pomieszczenia!", ch, NULL, victim, TO_VICT );
	/* to $n jest pchajacym, (nie)widocznosc $N nie zmienia niczego w zobaczeniu tego */
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje wypchn`a`c $# z pomieszczenia.", ch, NULL, victim, TO_NOTVICT );

	if ( !can_push_pull( ch, victim, in_room, to_room, door )
	  || ( number_percent( ) > 75 ) )
	{
	    send_to_char( "Nawet nie drgnie.\n\r", ch );
	    return;
	}

	if ( ch->move > 10 )
	{
	    ch->move -= 10;
	    send_to_char( "Uda`lo ci si`e!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n dopina swego.", ch, NULL, NULL, TO_ROOM );
	    move_char( victim, door, FALSE );
	}
	else
	{
	    ASTC( "Jeste`s zbyt zm`eczon$y, `zeby kogokolwiek pcha`c.", ch );
	}
    }

    return;
}


/*
 * Lam na prosbe graczy (Bartka, a potem innych)
 */
KOMENDA( do_vomit )
{
    if ( ch->in_room->area == area_czysciec )
    {
	STC( "No co`s ty, w niecielesnym `swiecie?\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->condition[ COND_FULL ] < 10 )
    {
	ASTC( "I tak nie jeste`s najedzon$y, po co bardziej si`e g`lodzi`c?", ch );
	return;
    }

    if ( IS_SET( race_table[ ch->race ].race_abilities, RACE_NO_MOUTH ) )
    {
	STC( "Nie masz czym zwymiotowa`c.\n\r", ch );
	return;
    }

    if ( race_table[ ch->race ].hunger_mod < 0
      && race_table[ ch->race ].thirst_mod < 0 )
    {
	STC( "Nie masz czego zwymiotowa`c.\n\r", ch );
	return;
    }

    act( ZM_WZROK | ZM_WID_CHAR, "{R$n wymiotuje.{x", ch, NULL, NULL, TO_ROOM );
    if ( IS_SET( race_table[ ch->race ].parts, P_RECE ) )
	STC( "Wk`ladasz sobie palec w gard`lo i wymiotujesz.\n\r", ch );
    else
	STC( "Wymiotujesz.\n\r", ch );
    if ( race_table[ ch->race ].hunger_mod > -1 )
	ASTC( "Czujesz si`e podle i na dodatek stajesz si`e g`lodn$y.", ch );
    else
	ASTC( "Czujesz si`e podle i na dodatek stajesz si`e spragnion$y.", ch );

    if ( !IS_NPC( ch ) )
    {
	if ( race_table[ ch->race ].hunger_mod > -1 )
	    ch->pcdata->condition[ COND_FULL ] = 1;
	if ( race_table[ ch->race ].thirst_mod > -1 )
	    ch->pcdata->condition[ COND_THIRST ] = 2;
    }

    WAIT_STATE( ch, 24 );

    return;
}


/*
 * Lam 23.3.99
 * Pomysly: zrobic to umiejetnoscia, losowe lamanie sie lopat :)
 */
FUNKCJA_CZASOWA( finish_digging )
{
    bool lopata = FALSE;
    EXIT_DATA *pexit;
    CHAR_DATA *ch = timer->ch;
    ROOM_INDEX_DATA *room = ch->in_room;
    OBJ_DATA *obj;
    bool anything = FALSE;

    if ( !check_blind( ch ) )
    {
	STC( "Niczego nie odkopujesz po omacku.\n\r", ch );
	return;
    }

    ch->move = UMAX( ch->move - 5, 0 );

    if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) && obj->item_type == ITEM_SHOVEL )
	lopata = TRUE;

    if ( ( pexit = ch->in_room->exit[ DIR_DOWN ] )
      && IS_SET( pexit->exit_info, EX_BURIED )
      && ch->move /* jesli zuzyl reszte ruchu, nie wykopuje */
	/* z lopata masz 100% szans, bez - 25% */
      && ( number_percent( ) * ( lopata ? 1 : 4 ) ) < 100 )
    {
	REMOVE_BIT( pexit->exit_info, EX_BURIED );
	/* jesli nie jest zakluczone, od razu otworzymy */
	if ( !IS_SET( pexit->exit_info, EX_LOCKED ) )
	{
	    ROOM_INDEX_DATA *to_room;
	    EXIT_DATA *pexit_rev;

	    REMOVE_BIT( pexit->exit_info, EX_CLOSED );

	    /* z drugiej strony... */
	    if ( ( to_room = pexit->to_room )
	      && ( pexit_rev = to_room->exit[ kierunki[ DIR_DOWN ].odwrotny ] )
	      && pexit_rev->to_room == ch->in_room )
	    {
		CHAR_DATA *rch;

		REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
		for ( rch = to_room->people; rch; rch = rch->next_in_room )
		{
		    if ( rch->deleted )
			continue;
		    if ( pexit_rev->rodzaj > 2 )
			act( ZM_WZROK, "$D otwieraj`a si`e.", rch, NULL, pexit_rev->short_descr, TO_CHAR );
		    else
			act( ZM_WZROK, "$D otwiera si`e.", rch, NULL, pexit_rev->short_descr, TO_CHAR );
		}
	    }
	}

	send_to_char( "Odkopujesz przej`scie!\n\r", ch );
	/* ktos odkopuje - dla mnie w porzadku */
	act( ZM_WZROK, "$n odkopuje przej`scie!", ch, NULL, NULL, TO_ROOM );
	anything = TRUE;
	eprog_dig_trigger( ch, DIR_DOWN );
	/* tutaj postac moze byc juz zabita/skasowana/przemieszczona, ale
	   to nieistotne */
	{
	    CHAR_DATA *rch;
	    OBJ_DATA *robj;

	    for ( rch = room->people; rch; rch = rch->next_in_room )
		check_airchar( rch );
	    for ( robj = room->contents; robj; robj = robj->next_content )
		new_airobj( robj );
	}

	/* dopiero teraz mozemy zareagowac na smierc/przeniesienie progiem */
	if ( ch->deleted || ch->in_room != room )
	    return;
    }
/*
    else
	send_to_char( "Nie uda`lo ci si`e odkopa`c `zadnego przej`scia...\n\r", ch );
*/

    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	/* bez lopaty 50% */
	if ( IS_OBJ_STAT( obj, ITEM_BURIED )
	  && ( number_percent( ) * ( lopata ? 1 : 2 ) ) < 100 )
	    break;

    /* tu jw., koniec ruchu - brak nagrody */
    if ( !ch->move || !obj )
    {
	if ( !anything )
	{
	    send_to_char( "Nie odkopujesz niczego ciekawego.\n\r", ch );
	    rprog_undug_trigger( ch );
	}
	return;
    }

    REMOVE_BIT_V( obj->extra_flags, ITEM_BURIED );
    act( ZM_ZAWSZE, "Odkopujesz $j!", ch, obj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n odkopuje $j!", ch, obj, NULL, TO_ROOM );
    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "$n odkopuje $j!", ch, obj, NULL, TO_ROOM );
    oprog_dig_trigger( ch, obj );

    return;
}


KOMENDA( do_dig )
{
    TIMER_DATA *timer;

    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "Nie wiesz jak si`e do tego zabra`c.\n\r", ch );
	return;
    }

    if ( find_timer( ch, finish_digging ) )
    {
	send_to_char( "Przecie`z wci`a`z kopiesz!\n\r", ch );
	return;
    }

    if ( !czy_mozna_kopac( ch->in_room->sector_type, ch ) )
	return;

    if ( ch->move <= 10 )
    {
	STC( "Nie wystarczy ci si`l.\n\r", ch );
	return;
    }

    ch->move -= 5; /* drugie 5 na koniec kopania */

    timer = new_timer( );
    timer->ch = ch;
    timer->time = IS_AFFECTED( ch, AFF_PRZYSPIESZENIE ) ? 8 : 12;
    timer->fun = finish_digging;
    run_timer( timer );

    send_to_char( "Rozpoczynasz kopanie...\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zaczyna kopa`c w ziemi.", ch, NULL, NULL, TO_ROOM );

    return;
}


KOMENDA( do_wsiadz )
{
    STC( "Jeszcze nie da si`e w nic wsiada`c.\n\r", ch );

    return;
}


KOMENDA( do_wysiadz )
{
    STC( "Nie masz z czego wysiada`c.\n\r", ch );

    return;
}
