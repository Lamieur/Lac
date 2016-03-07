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
#include "mp_wyzw.h"
#include "magic.h"
#include "handler.h"
#include "const.h"
#include "act_info.h"
#include "act_move.h"
#include "comm.h"
#include "db.h"


CHAR_DATA	*find_przechowalnia	args( ( CHAR_DATA *ch, bool komunikat ) );
void		zmien_poziom_przedmiotu	args( ( OBJ_DATA *przedmiot, int poziom ) );
bool		pijane_przedmioty	args( ( CHAR_DATA *ch ) );
void		get_obj			args( ( CHAR_DATA *ch, OBJ_DATA *obj,
						OBJ_DATA *container, bool give ) );
OBJ_DATA	*znajdz_cialo		args( ( CHAR_DATA *ch, char *argument ) );
void		wear_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj,
						bool fReplace, bool hold ) );
void		wear_hold		args( ( CHAR_DATA *ch, char *argument, bool hold ) );
CHAR_DATA	*find_keeper		args( ( CHAR_DATA *ch, char *argument ) );
CHAR_DATA	*find_healer		args( ( CHAR_DATA *ch, char *argument ) );
CHAR_DATA	*find_koles		args( ( CHAR_DATA *ch, char *argument ) );
int		get_cost		args( ( CHAR_DATA *keeper,
						OBJ_DATA *obj, bool fBuy ) );
OBJ_DATA	*get_obj_shop		args( ( CHAR_DATA *ch, char *argument ) );
void		potencjalnie_zamknij_sklep	args( ( CHAR_DATA *ch ) );
void		list_przechowalnia	args( ( CHAR_DATA *ch ) );
int		parsebet		args( ( const int currentbet, char *s ) );
void		list_osobiste		args( ( CHAR_DATA *keeper, CHAR_DATA *ch ) );
CHAR_DATA	*find_warsztat		args( ( CHAR_DATA *ch ) );

/*
 * Ulryk 25.06.2002: zmienia poziom przedmiotu (a takze jego zawartosci
 * w przypadku pojemnikow) na podany
 */
void zmien_poziom_przedmiotu( OBJ_DATA *przedmiot, int poziom )
{
    OBJ_DATA *obj;

    przedmiot->level = UMAX( przedmiot->level, poziom );
    if ( przedmiot->item_type == ITEM_CONTAINER
      || przedmiot->item_type == ITEM_CORPSE_NPC )
	for ( obj = przedmiot->contains; obj; obj = obj->next_content )
	    zmien_poziom_przedmiotu( obj, poziom );

    return;
}


/*
 * Lam: owijam powyzsze w to, zeby nie powtarzac wszedzie warunkow
 */
void zmien_poziom( OBJ_DATA *przedmiot, CHAR_DATA *ch )
{
    CHAR_DATA *herszt;

    /* Lam 9.10.2000: podczas trwania proga nie zmienia sie poziom */
    if ( !prog_nest )
    {
	if ( ch->level >= LEVEL_HERO )
	    zmien_poziom_przedmiotu( przedmiot, ch->level );

	herszt = ch;
	while ( IS_NPC( herszt ) /* oczarowany gracz nie zmieni poziomu */
	     && IS_AFFECTED( herszt, AFF_CHARM )
	     && herszt->master )
	    herszt = herszt->master;

	if ( IS_NPC( ch )
	  && herszt != ch
	  && herszt->level >= LEVEL_HERO
	  && herszt->level > ch->level )
	    zmien_poziom_przedmiotu( przedmiot, herszt->level );
    }

    return;
}


/*
 * Lam 3.5.98: szukanie przedmiotu przez pijane postacie. Teksty inspirowane
 * tekstami ze Smauga.
 */
bool pijane_przedmioty( CHAR_DATA *ch )
{
    char *t = NULL;

    if ( IS_NPC( ch )
      || ( ch->pcdata->condition[ COND_DRUNK ] <= number_percent( ) / 2
	&& ( !IS_AFFECTED( ch, AFF_DEZORIENTACJA )
	  || number_percent( ) > 20 ) ) )
	return FALSE;

    switch ( IS_AFFECTED( ch, AFF_DEZORIENTACJA )
	    ? number_range( 1, 17 )
	    : ch->pcdata->condition[ COND_DRUNK ] / 5 + number_bits( 3 ) )
    {
	default: t = "Nie mo`zesz tego dosi`egn`a`c."; break;
	case 1:  t = "Zaraz... zapomnia`l$o`s czego szukasz."; break;
	case 2:  t = "Co`s powstrzymuje ci`e przed wzi`eciem tego w r`ece."; break;
	case 3:  t = "Jak to? To co`s ci ucieka!"; break;
	case 4:  t = "Zrobi`l$aby`s to, gdyby przedmiot si`e tak nie rusza`l."; break;
	case 5:  t = "Zaraz... Najpierw trzeba si`e podrapa`c..."; break;
	case 6:  t = "Nie uda`lo ci si`e tego znale`x`c..."; break;
	case 7:  t = "Nie mo`zesz tego dosi`egn`a`c!"; break;
	case 8:  t = "Wolisz pod`luba`c sobie w nosie."; break;
	case 9:  t = "Ale`z to jest zakopane pod stert`a innych rzeczy!"; break;
	case 10: t = "`Ze co? Nie o`smielisz si`e!"; break;
	case 11: t = "Prawie mia`l$o`s to w gar`sci, ale ci uciek`lo."; break;
	case 12: t = "Kt`ore? Widz`e dwie sztuki.. trzy..."; break;
	case 13: t = "Nie, za bardzo si`e boisz."; break;
	case 14: t = "Aaa! To jest ca`le we krwi!"; break;
	case 15: t = "Jeste`s wszechmocn$y, po co mia`l$aby`s to robi`c?"; break;
	case 16: t = "Nagle ujrza`l$o`s boga zabraniaj`acego ci tego."; break;
	case 17: t = "Daj spok`oj, nie masz na to si`ly."; break;
    }
    ASTC( t, ch );

    return TRUE;
}


void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container, bool give )
{
    if ( !CAN_WEAR( obj, ITEM_TAKE ) )
    {
	send_to_char( "Nie mo`zesz tego wzi`a`c.\n\r", ch );
	return;
    }

    if ( !( obj->item_type == ITEM_MONEY ) )
    {
	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
	{
	    act( ZM_ZAWSZE, "Masz przy sobie zbyt wiele przedmiot`ow, nie uniesiesz $g.",
		ch, obj, NULL, TO_CHAR );
	    return;
	}

	/* Lam 17.4.2005: przy pelnym obciazeniu i tak mozna wyjac z pojemnika
	   we wlasnym inwentarzu */
	if ( ( !container || container->carried_by != ch )
	  && ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
	    act( ZM_ZAWSZE, "D`xwigasz zbyt wielki ci`e`zar, nie uniesiesz $g.",
		ch, obj, NULL, TO_CHAR );
	    return;
	}
    }

    if ( !give )
    {
	if ( container )
	{
	    if ( container->item_type == ITEM_CORPSE_NPC
	      || container->item_type == ITEM_CORPSE_PC )
	    {
		if ( czy_ze( container->dopelniacz ) )
		{
		    act( ZM_ZAWSZE, "Zabierasz $j ze $G.", ch, obj, container, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ2, "$n zabiera $j ze $G.", ch, obj, container, TO_ROOM );
		    /* ze zwlok, ale z czegos */
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ2, "$n zabiera $j z $G.", ch, obj, container, TO_ROOM );
		}
		else
		{
		    act( ZM_ZAWSZE, "Zabierasz $j z $G.", ch, obj, container, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR, "$n zabiera $j z $G.", ch, obj, container, TO_ROOM );
		}
	    }
	    else
	    {
		if ( czy_ze( container->dopelniacz ) )
		{
		    act( ZM_ZAWSZE, "Wyci`agasz $j ze $G.", ch, obj, container, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ2, "$n wyci`aga $j ze $G.", ch, obj, container, TO_ROOM );
		    /* ze skrzyni, ale z czegos */
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ2, "$n wyci`aga $j z $G.", ch, obj, container, TO_ROOM );
		}
		else
		{
		    act( ZM_ZAWSZE, "Wyci`agasz $j z $G.", ch, obj, container, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR, "$n wyci`aga $j z $G.", ch, obj, container, TO_ROOM );
		}
	    }
	    obj_from_obj( obj );
	}
	else
	{
	    act( ZM_ZAWSZE, "Podnosisz $j.", ch, obj, container, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n podnosi $j.", ch, obj, container, TO_ROOM );
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1,
		obj->rodzaj == 3 ? "$p znikaj`a." : "$p znika.",
		ch, obj, container, TO_ROOM );
	    obj_from_room( obj );
	}
    }

    if ( ( ( IS_OBJ_STAT( obj, ITEM_HOLY ) && IS_EVIL( ch ) )
      || ( IS_OBJ_STAT( obj, ITEM_VAMPIRE_BANE )
	&& CZY_WAMPIR( ch ) ) )
      && ( ch->in_room->area != area_czysciec )
      && ( !IS_SET( ch->in_room->room_flags, ROOM_SAFE ) ) )
    {
	act( ZM_ZAWSZE, "Pali ci`e `SWI`ETY OGIE`N $g. Au`c!", ch, obj, NULL,
	    TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "`SWI`ETY OGIE`N $g pali $3!", ch, obj, NULL, TO_ROOM );
	damage( ch, ch, ch->level, gsn_burning_hands, WEAR_NONE, TRUE );
    }

    REMOVE_BIT_V( obj->extra_flags, ITEM_POM );
    obj_to_char( obj, ch );
    zmien_poziom( obj, ch );

    if ( !give )
	oprog_take_trigger( ch, obj );

    if ( !obj->deleted && !ch->deleted
      && obj->carried_by == ch
      && obj->item_type == ITEM_MONEY )
    {
	char buf [ MAX_STRING_LENGTH ];
	int  amount = obj->value[ 0 ].v;

	ch->gold += amount;

	if ( amount > 1 )
	{
	    sprintf( buf, "Naliczy`l$o`s %s.", ZLOTO_B( amount ) );
	    ASTC( buf, ch );
	}

	extract_obj( obj );
    }

    return;
}


/*
 * Lam 4.3.98: szuka w pomieszczeniu z ch ciala, ktore jest jego.
 */
OBJ_DATA *znajdz_cialo( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    char      *pd;
    char       name[ 200 ];
    char       arg[ MAX_INPUT_LENGTH ];

    /* jezeli np. 2.cialo, to nie szukaj, bo wie, czego chce */
    if ( number_argument( argument, arg ) > 1 )
	return NULL;

    /* mob wiadomo - zadnych nie moze oprozniac */
    if ( IS_NPC( ch ) )
	return NULL;

    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
    {
	if ( obj->item_type != ITEM_CORPSE_PC )
	    continue;

	pd = obj->short_descr;
	pd = one_argument( pd, name );
	pd = one_argument( pd, name );
	one_argument( pd, name );

	if ( !can_see_obj( ch, obj ) /* musi widziec */
	  || !is_name( arg, obj->name ) /* musi spelniac kryteria */
	  || str_cmp( name, ch->dopelniacz ) ) /* musi byc jego */
	    continue;

	return obj; /* warunki spelnione */
    }

    return NULL;
}


KOMENDA( do_get )
{
    OBJ_DATA *obj;
    OBJ_DATA *container;
    char      arg1[ MAX_INPUT_LENGTH ];
    char      arg2[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    /* Get type. */
    if ( arg1[ 0 ] == '\0' )
    {
	send_to_char( "Co wzi`a`c?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( arg2[ 0 ] == '\0' )
    {
	if ( !czy_wszystko( arg1 ) && str_prefix( "wsz.", arg1 ) )
	{
	    /* "wez przedmiot" */
	    if ( !( obj = get_obj_list( ch, arg1, ch->in_room->contents ) ) )
	    {
		act( ZM_ZAWSZE, "Nie widz`e tu \"$T\".", ch, NULL, arg1, TO_CHAR );
		return;
	    }

	    get_obj( ch, obj, NULL, FALSE );
	}
	else
	{
	    /* "wez wsz" albo 'wez wsz.przedmiot' */
	    OBJ_DATA *obj_next;
	    int found = FALSE;

	    for ( obj = ch->in_room->contents; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;

		if ( !IS_OBJ_STAT( obj, ITEM_POM )
		  && ( czy_wszystko( arg1 )
		    || ( arg1[ 3 ] == '.' && is_name( &arg1[ 4 ], obj->name ) ) )
		    && can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, NULL, FALSE );
		}
	    }

	    if ( !found )
	    {
		if ( czy_wszystko( arg1 ) )
		    send_to_char( "Niczego tu nie widz`e.\n\r", ch );
		else
		    act( ZM_ZAWSZE, "Nie widz`e tu $T.", ch, NULL, &arg1[ 4 ], TO_CHAR );
	    }
	}
    }
    else
    {
	/* "wez ... pojemnik" */
	if ( czy_wszystko( arg2 ) || !str_prefix( "wsz.", arg2 ) )
	{
	    send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	    return;
	}

	if ( !( container = get_obj_here( ch, arg2 ) ) )
	{
	    act( ZM_ZAWSZE, "Nie widz`e tu \"$T\".", ch, NULL, arg2, TO_CHAR );
	    return;
	}

	switch ( container->item_type )
	{
	    default:
		send_to_char( "To nie pojemnik.\n\r", ch );
		return;

	    case ITEM_CONTAINER:
	    case ITEM_CORPSE_NPC:
		break;

	    case ITEM_CORPSE_PC:
	    {
		CHAR_DATA *gch;
		char      *pd;
		char       name[ MAX_INPUT_LENGTH ];
		char       dop[ MAX_INPUT_LENGTH ];

		if ( IS_NPC( ch ) )
		{
		    send_to_char( "Nie mo`zesz tego zrobi`c, jeste`s mobem!\n\r", ch );
		    return;
		}

		pd = container->short_descr;
		pd = one_argument( pd, name );
		pd = one_argument( pd, name );
		one_argument( pd, name );
		one_argument( ch->dopelniacz, dop );

		if ( str_cmp( name, ch->dopelniacz ) && ( get_trust( ch ) < 106 ) )
		{
		    OBJ_DATA *tmp_obj;

		    /* Lam: znalezienie ciala, jesli zachodzi taka potrzeba (to,
		       czy zachodzi jest sprawdzane wewnatrz funkcji) */
		    if ( ( tmp_obj = znajdz_cialo( ch, argument ) ) )
		    {
			container = tmp_obj;
			break; /* break dotyczy switch */
		    }

		    for ( gch = char_list; gch; gch = gch->next )
			if ( !IS_NPC( gch )
			    && is_same_group( ch, gch )
			    && !str_cmp( name, dop ) )
			{
			    break;
			}

		    if ( !gch )
		    {
			STC( "`Ladnie to tak okrada`c cia`la? Nie ma mowy - niczego nie bierzesz.\n\r", ch );
			return;
		    }
		}
	    }
	}

	if ( IS_SET( container->value[ 1 ].v, CONT_CLOSED ) )
	{
	    if ( container->rodzaj == 1 )
		act( ZM_ZAWSZE, "$p jest zamkni`ety.", ch, container, NULL, TO_CHAR );
	    else if ( container->rodzaj == 2 )
		act( ZM_ZAWSZE, "$p jest zamkni`eta.", ch, container, NULL, TO_CHAR );
	    else if ( container->rodzaj == 3 )
		act( ZM_ZAWSZE, "$p s`a zamkni`ete.", ch, container, NULL, TO_CHAR );
	    else
		act( ZM_ZAWSZE, "$p jest zamkni`ete.", ch, container, NULL, TO_CHAR );
	    return;
	}

	if ( !czy_wszystko( arg1 ) && str_prefix( "wsz.", arg1 ) )
	{
	    /* "wez przedmiot pojemnik" */
	    obj = get_obj_list( ch, arg1, container->contains );
	    if ( !obj )
	    {
		act( ZM_ZAWSZE, "Nie widz`e tego w $L.", ch, NULL, container, TO_CHAR );
		return;
	    }
	    get_obj( ch, obj, container, FALSE );
	}
	else
	{
	    /* "wez wsz pojemnik" albo 'wez wsz.przedmiot pojemnik' */
	    OBJ_DATA *obj_next;
	    int found = FALSE;

	    for ( obj = container->contains; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;

		if ( ( czy_wszystko( arg1 )
		    || ( arg1[ 3 ] == '.' && is_name( &arg1[ 4 ], obj->name ) ) )
		  && can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, container, FALSE );
		}
	    }

	    if ( !found )
	    {
		if ( czy_wszystko( arg1 ) )
		    act( ZM_ZAWSZE, "Nie widz`e niczego w $L.", ch, NULL, container, TO_CHAR );
		else
		    act( ZM_ZAWSZE, "Nie widz`e tego w $L.", ch, NULL, container, TO_CHAR );
	    }
	}
    }

    return;
}


KOMENDA( do_put )
{
    OBJ_DATA *container;
    OBJ_DATA *obj;
    char      arg1[ MAX_INPUT_LENGTH ];
    char      arg2[ MAX_INPUT_LENGTH ];
    char      arg3[ MAX_INPUT_LENGTH ];
    bool      urna = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
    {
	send_to_char( "W`lo`zy`c co w co?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( czy_wszystko( arg2 ) || !str_prefix( "wsz.", arg2 ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return;
    }

    if ( !( container = get_obj_here( ch, czy_monety ( arg2 ) ? arg3 : arg2 ) ) )
    {
	act( ZM_ZAWSZE, "Nie widzisz pojemnika o nazwie \"$T\".", ch, NULL, czy_monety ( arg2 ) ? arg3 : arg2, TO_CHAR );
	return;
    }

    if ( container->item_type != ITEM_CONTAINER )
    {
	send_to_char( "To nie pojemnik.\n\r", ch );
	return;
    }

    if ( IS_SET( container->value[ 1 ].v, CONT_CLOSED ) )
    {
	if ( container->rodzaj == 1 )
	    act( ZM_ZAWSZE, "$p jest zamkni`ety.", ch, container, NULL, TO_CHAR );
	else if ( container->rodzaj == 2 )
	    act( ZM_ZAWSZE, "$p jest zamkni`eta.", ch, container, NULL, TO_CHAR );
	else if ( container->rodzaj == 3 )
	    act( ZM_ZAWSZE, "$p s`a zamkni`ete.", ch, container, NULL, TO_CHAR );
	else
	    act( ZM_ZAWSZE, "$p jest zamkni`ete.", ch, container, NULL, TO_CHAR );
	return;
    }

    /* Tissarius 21.10.2001: wkladanie monet do pojemnika (skopiowane z do_drop
       i nieco przerobione) */
    if ( czy_monety( arg2 ) )
    {
	/* "wloz <ilosc> monet pojemnik" */
	int amount;

	if ( arg3[ 0 ] == '\0' )
	{
	    send_to_char( "W co w`lo`zy`c z`loto?\n\r", ch );
	    return;
	}

	amount = advatoi( arg1 );

	if ( amount <= 0 )
	{
	    send_to_char( "Jak masz zamiar to zrobi`c?\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "Nie nosisz ze sob`a takiej kasy.\n\r", ch );
	    return;
	}

	act( ZM_ZAWSZE, "Wk`ladasz troch`e z`lota w $J.", ch, NULL, container, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wk`lada troch`e z`lota w $J.", ch, NULL, container, TO_ROOM );
	oprog_putin_gold_trigger( ch, amount, container ); /* Tissarius 23.10.2001 */
	return;
    }

    if ( is_name( "#urna_na_dary_", container->name ) )
	urna = TRUE;

    if ( !czy_wszystko( arg1 ) && str_prefix( "wsz.", arg1 ) )
    {
	/* "wloz przedmiot pojemnik" */
	if ( !( obj = get_obj_carry( ch, arg1 ) ) )
	{
	    send_to_char( "Nie masz tego przy sobie.\n\r", ch );
	    return;
	}

	if ( obj == container )
	{
	    send_to_char( "Czy chcesz zastosowa`c wk`ladanie rekurencyjne?\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "Nie mo`zesz si`e tego pozby`c.\n\r", ch );
	    return;
	}

	if ( urna
	  && ( obj->item_type == ITEM_TRASH
	    || obj->item_type == ITEM_FOOD
	    || obj->item_type == ITEM_KEY
	    || obj->level >= LEVEL_HERO ) )
	{
	    STC( "Nie potrzeba takich dar`ow w urnach.\n\r", ch );
	    return;
	}

	if ( get_obj_weight( obj ) + get_obj_weight( container )
	    - container->weight > container->value[ 0 ].v )
	{
	    send_to_char( "Nie wejdzie!\n\r", ch );
	    return;
	}

	act( ZM_ZAWSZE, "Wk`ladasz $j w $J.", ch, obj, container, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wk`lada $j w $J.", ch, obj, container, TO_ROOM );
	oprog_put_and_putin_trigger( ch, obj, container );
    }
    else
    {
	/* "wloz wsz pojemnik" albo "wloz wsz.przedmiot pojemnik" */
	OBJ_DATA *obj_next;
	ROOM_INDEX_DATA *room = ch->in_room;
	bool wsadzone = FALSE;
	bool znalezione = FALSE;

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg1[ 3 ] != '.'
		|| is_name( &arg1[ 4 ], obj->name ) )
	      && can_see_obj( ch, obj )
	      && obj->wear_loc == WEAR_NONE
	      && obj != container )
	    {
		znalezione = TRUE;

		if ( can_drop_obj( ch, obj )
		  && get_obj_weight( obj ) + get_obj_weight( container )
		      - container->weight <= container->value[ 0 ].v
		  && ( !urna
		    || ( obj->item_type != ITEM_TRASH
		      && obj->item_type != ITEM_FOOD
		      && obj->item_type != ITEM_KEY
		      && obj->level < LEVEL_HERO ) ) )
		{
		    act( ZM_ZAWSZE, "Wk`ladasz $j w $J.", ch, obj, container, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR, "$n wk`lada $j w $J.", ch, obj, container, TO_ROOM );
		    oprog_put_and_putin_trigger( ch, obj, container );
		    wsadzone = TRUE;
		}

		if ( ch->deleted
		  || ch->in_room != room
		  || container->deleted )
		    return;
	    }
	}

	if ( !znalezione )
	    STC( "Nie masz `zadnego pasuj`acego przedmiotu nadaj`acego si`e do w`lo`zenia.\n\r", ch );
	else if ( !wsadzone )
	    STC( "Nic nie chce si`e zmie`sci`c.\n\r", ch );
    }

    return;
}


KOMENDA( do_drop )
{
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *in_room;
    char arg[ MAX_INPUT_LENGTH ], arg2[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co upu`sci`c?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( czy_monety( arg2 ) )
    {
	/* "upusc <ilosc> monet" */
	OBJ_DATA *obj_next;
	int       amount;

	amount = advatoi( arg );

	if ( amount <= 0 )
	{
	    send_to_char( "Jak masz zamiar to zrobi`c?\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "Nie nosisz ze sob`a takiej kasy.\n\r", ch );
	    return;
	}

	/* miejsca w arg2 wystarczy */
	sprintf( arg2, "Upuszczasz %s.\n\r", ZLOTO_B( amount ) );
	ch->gold -= amount;

	/* pieniadze dolaczaja do lezacych na ziemi, przepisane przez Lama */
	for ( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->deleted )
		continue;

	    if ( obj->pIndexData->vnum == OBJ_VNUM_MONEY_ONE
	      || obj->pIndexData->vnum == OBJ_VNUM_MONEY_SOME )
	    {
		amount += obj->pIndexData->vnum == OBJ_VNUM_MONEY_ONE ? 1
							: obj->value[ 0 ].v;
		extract_obj( obj );
	    }
	}

	obj_to_room( create_money( amount ), ch->in_room );
	STC( arg2, ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n upuszcza troch`e z`lota.", ch, NULL, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Pojawia si`e tu troch`e z`lota.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    in_room = ch->in_room;

    if ( !czy_wszystko( arg ) && str_prefix( "wsz.", arg ) )
    {
	/* "upusc przedmiot" */
	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
	    send_to_char( "Nie masz tego przy sobie.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "Nie potrafisz si`e tego pozby`c.\n\r", ch );
	    return;
	}

	obj_from_char( obj );
	/* Lam 16.1.2004: dorzucanie do ognisk */
	if ( obj->pIndexData->vnum == OBJ_VNUM_CHRUST
	  && room_is_affected( ch->in_room, gsn_rozpalanie_ognisk ) )
	{
	    AFFECT_DATA *af;

	    STC( "Dorzucasz do ognia.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n dorzuca do ognia.", ch, NULL, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_W_WID_CHAR, "Ognisko przez chwil`e bucha wi`ekszym ogniem.", ch, NULL, NULL, TO_ROOM );
	    /* dorzucajac do juz palacego sie ogniska mozna je rozpalic
	       nieco bardziej niz rozpalajac je na poczatku */
	    for ( af = ch->in_room->affected; af; af = af->next )
		if ( af->type == gsn_rozpalanie_ognisk )
		    af->duration = UMIN( af->duration + obj->value[ 0 ].v, 13 );

	    extract_obj( obj );
	}
	else
	{
	    /* upuszczenie swiatla moze oslepic ch i kogos, kto widzi ch */
	    act( ZM_ZAWSZE, "Upuszczasz $j.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n upuszcza $j.", ch, obj, NULL, TO_ROOM );
	    obj_to_room( obj, ch->in_room );
	    /* jesli ktos nie widzi ch, moze zobaczyc obj dopiero po
	       upuszczeniu */
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1,
		obj->rodzaj == 3 ? "Pojawiaj`a si`e tu $p." : "Pojawia si`e tu $p.",
		ch, obj, NULL, TO_ROOM );
	    oprog_drop_trigger( ch, obj );
	    if ( !ch->deleted && !obj->deleted && ch->in_room == in_room )
		rprog_drop_trigger( ch, obj );
	}
    }
    else
    {
	/* "upusc wsz" albo "upusc wsz.przedmiot" */
	OBJ_DATA *obj_next;
	bool found, niewsz;

	found = FALSE;
	niewsz = FALSE;
	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( czy_wszystko( arg )
	       || ( arg[ 3 ] == '.' && is_name( &arg[ 4 ], obj->name ) ) )
		&& can_see_obj( ch, obj )
		&& obj->wear_loc == WEAR_NONE )
	    {
		if ( !can_drop_obj( ch, obj ) )
		{
		    niewsz = TRUE;
		    continue;
		}
		found = TRUE;
		obj_from_char( obj );
		/* Lam 16.1.2004: dorzucanie do ognisk */
		if ( obj->pIndexData->vnum == OBJ_VNUM_CHRUST
		  && room_is_affected( ch->in_room, gsn_rozpalanie_ognisk ) )
		{
		    AFFECT_DATA *af;

		    STC( "Dorzucasz do ognia.\n\r", ch );
		    /* dorzucajac do juz palacego sie ogniska mozna je rozpalic
		       ciutke bardziej niz rozpalajac je na poczatku */
		    for ( af = ch->in_room->affected; af; af = af->next )
			if ( af->type == gsn_rozpalanie_ognisk )
			    af->duration = UMIN( af->duration + obj->value[ 0 ].v, 13 );
		    extract_obj( obj );
		}
		else
		{
		    act( ZM_ZAWSZE, "Upuszczasz $j.", ch, obj, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR, "$n upuszcza $j.", ch, obj, NULL, TO_ROOM );
		    obj_to_room( obj, ch->in_room );
		    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1,
			obj->rodzaj == 3 ? "Pojawiaj`a si`e tu $p." : "Pojawia si`e tu $p.",
			ch, obj, NULL, TO_ROOM );
		    oprog_drop_trigger( ch, obj );
		    if ( !ch->deleted && !obj->deleted && ch->in_room == in_room )
			rprog_drop_trigger( ch, obj );
		}

		if ( ch->deleted || ch->in_room != in_room )
		    return;
	    }
	}

	if ( !found )
	{
	    if ( niewsz )
		STC( "`Zadnego z wybranych przedmiot`ow nie mo`zesz si`e pozby`c.\n\r", ch );
	    else
	    {
		if ( czy_wszystko( arg ) )
		    send_to_char( "Niczego przy sobie nie masz.\n\r", ch );
		else
		    act( ZM_ZAWSZE, "Nie nosisz przy sobie niczego o nazwie \"$T\".",
			ch, NULL, &arg[ 4 ], TO_CHAR );
	    }
	}
	else if ( niewsz )
	    STC( "Niekt`orych z wybranych przedmiot`ow nie mo`zesz si`e pozby`c.\n\r", ch );
    }

    return;
}


KOMENDA( do_give )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
    {
	send_to_char( "Da`c co komu?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( czy_monety( arg2 ) )
    {
	/* "daj <ilosc> monet ktos" */
	char buf[ MAX_STRING_LENGTH ];
	int  amount, lilia;

	amount   = advatoi( arg1 );
	if ( amount <= 0 )
	{
	    send_to_char( "Jak to sobie wyobra`zasz?\n\r", ch );
	    return;
	}

	one_argument( argument, arg2 );

	if ( arg2[ 0 ] == '\0' )
	{
	    send_to_char( "Da`c co komu?\n\r", ch );
	    return;
	}

	if ( !( victim = get_char_room( ch, arg2 ) ) )
	{
	    send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
	    return;
	}

	if ( ch == victim )
	{
	    send_to_char( "Nie przek`ladaj bez sensu z`lota z r`eki do r`eki, kiedy wok`o`l tylu z`lodziei.\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "Nie nosisz przy sobie takiej kasy.\n\r", ch );
	    return;
	}

	ch->gold     -= amount;
	victim->gold += amount;
	lilia = liczba( amount );
	sprintf( buf, "Dajesz $@ %d monet%s.", amount, lilia == 1 ? "`e" :
		lilia == 2 ? "y" : "" );
	act( ZM_ZAWSZE, buf, ch, NULL, victim, TO_CHAR );

	sprintf( buf, "$n daje ci %d monet%s.", amount, lilia == 1 ? "`e" :
		lilia == 2 ? "y" : "" );
	act( ZM_WZROK, buf, ch, NULL, victim, TO_VICT );
	act( ZM_PRZYT | ZM_W_WZROK, "Dostajesz od kogo`s troch`e z`lota.", ch, NULL, victim, TO_VICT );

	act( ZM_WZROK | ZM_WID_CHAR, "$n daje $@ troch`e z`lota.",  ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT, "$n daje $@ troch`e z`lota.",  ch, NULL, victim, TO_NOTVICT );
	mprog_bribe_trigger( victim, ch, amount );
	return;
    }

    if ( !( obj = get_obj_carry( ch, arg1 ) ) )
    {
	send_to_char( "Nie masz tego przy sobie.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "Najpierw musisz to zdj`a`c.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Nie potrzebujesz sobie dawa`c rzeczy, kt`ore ju`z masz.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "Nie mo`zesz si`e tego pozby`c.\n\r", ch );
	return;
    }

    if ( !IS_AWAKE( victim ) )
    {
	act( ZM_ZAWSZE, "$N nie zauwa`za twoich wysi`lk`ow, poczekaj, a`z otworzy oczy.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ( IS_OBJ_STAT( obj, ITEM_HOLY ) && IS_EVIL( victim ) )
      || ( IS_OBJ_STAT( obj, ITEM_VAMPIRE_BANE )
	&& CZY_WAMPIR( victim ) )
      || ( IS_NPC( victim ) && ( ( victim->pIndexData->pShop )
	|| IS_SET( victim->act, ACT_NO_GIFTS ) ) ) )
    {
	act( ZM_ZAWSZE, "$N odmawia przyj`ecia $g.", ch, obj, victim, TO_CHAR );
	act( ZM_WZROK | ZM_WID_VICT, "$n pr`obuje da`c $@ $j, ale $Z odmawia.",
	    ch, obj, victim, TO_NOTVICT );
	/* kolejny blad w Envy - powyzej zamiast to_notvict bylo to_room */
	act( ZM_PRZYT, "Odmawiasz przyj`ecia $g od $1.",
	    ch, obj, victim, TO_VICT );
	return;
    }

    if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
    {
	act( ZM_ZAWSZE, "$N ma pe`lny inwentarz.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
	act( ZM_ZAWSZE, "$N nie uniesie takiego ci`e`zaru.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( ZM_ZAWSZE, "$N nie mo`ze tego zobaczy`c.", ch, NULL, victim, TO_CHAR );
	return;
    }

    act( ZM_ZAWSZE, "Dajesz $j $@.",  ch, obj, victim, TO_CHAR    );
    act( ZM_PRZYT, "$n daje ci $j.", ch, obj, victim, TO_VICT    );
    act( ZM_WZROK | ZM_WID_CHAR, "$n daje $j $@.", ch, obj, victim, TO_NOTVICT );
    obj_from_char( obj );
    get_obj( victim, obj, NULL, TRUE );
    /* Lam 20.8.98: powyzsze zamiast obj_to_char( obj, victim ); */
    /* po wzieciu - moze obj jest teraz widoczny? */
    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT, "$n daje $j $@.", ch, obj, victim, TO_NOTVICT );

    /* najpierw oprog, przedmiot moze sie rozleciec w jego trakcie, wiec nie ma
       sensu, zeby mob wczesniej probowal z nim cos robic (np. "zadaniowe" moby
       maja sklonnosc do oddawania/upuszczania i gadania glupot) */
    oprog_give_trigger( ch, victim, obj );

    if ( obj->deleted
      || ch->in_room != room
      || victim->in_room != room )
	return;

    mprog_give_trigger( victim, ch, obj );

    return;
}


KOMENDA( do_fill )
{
    OBJ_DATA *obj;
    OBJ_DATA *z_obj;
    char      arg[ MAX_INPUT_LENGTH ];
    char      z_arg[ MAX_INPUT_LENGTH ];
    int       w;

    if ( !check_blind( ch ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	STC( "Co nape`lni`c?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	STC( "Nie masz tego przy sobie.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	STC( "Nie nape`lnisz tego.\n\r", ch );
	return;
    }

    if ( obj->value[ 1 ].v >= obj->value[ 0 ].v )
    {
	STC( "Tw`oj pojemnik jest pe`lny.\n\r", ch );
	return;
    }

    one_argument( argument, z_arg );

    if ( z_arg[ 0 ] == '\0' )
    {
	for ( z_obj = ch->in_room->contents; z_obj; z_obj = z_obj->next_content )
	    if ( z_obj->item_type == ITEM_FOUNTAIN )
		break;

	if ( !z_obj )
	{
	    STC( "Nie ma tu studni ani fontanny.\n\r", ch );
	    return;
	}

	w = 5;
    }
    else
    {
	if ( !( z_obj = get_obj_here( ch, z_arg ) ) )
	{
	    STC( "Nie mo`zesz tego znale`x`c.\n\r", ch );
	    return;
	}

	if ( z_obj->item_type == ITEM_DRINK_CON )
	    w = 2;
	else if ( z_obj->item_type == ITEM_FOUNTAIN )
	    w = 5;
	else
	{
	    STC( "W tym nie ma `zadnego p`lynu.\n\r", ch );
	    return;
	}
    }

    if ( obj == z_obj )
    {
	/* fontanna sie nie zdarzy, obj jest sprawdzany pod tym katem */
	STC( "To jest ten sam pojemnik na p`lyn.\n\r", ch );
	return;
    }

    if ( z_obj->value[ 1 ].v == 0 )
    {
	char bufor[ MSL ];

	sprintf( bufor, "$p %s zupe`lnie pust%s.",
	    z_obj->rodzaj == 3 ? "s`a" : "jest",
	    z_obj->rodzaj == 1 ? "y" : z_obj->rodzaj == 2 ? "a" : "e" );

	act( ZM_ZAWSZE, bufor, ch, z_obj, NULL, TO_CHAR );
	return;
    }

    if ( obj->value[ 1 ].v != 0
      && obj->value[ 2 ].v != z_obj->value[ w ].v )
    {
	STC( "Tam ju`z jest inny p`lyn.\n\r", ch );
	return;
    }

    /* w ten sposob otrzymuje: Alandar nape`lnia buk`lak wod`a z fontanny.
       Przy !can_see bedzie: Kto`s nape`lnia co`s z czego`s. */
    sprintf( arg, "%s %s", obj->biernik,
	liq_table_pl[ z_obj->value[ w ].v ].liq_narzednik );

    if ( can_see_obj( ch, z_obj ) && czy_ze( z_obj->dopelniacz ) )
    {
	act( ZM_ZAWSZE, "Nape`lniasz $r ze $G.", ch, arg, z_obj, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nape`lnia $r ze $G.", ch, arg, z_obj, TO_ROOM );
    }
    else
    {
	act( ZM_ZAWSZE, "Nape`lniasz $r z $G.", ch, arg, z_obj, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nape`lnia $r z $G.", ch, arg, z_obj, TO_ROOM );
    }

    /* kopiowanie typu plynu; pojemniki maja to w wart. 2, fontanny w 5, stad
       zmienna w */
    obj->value[ 2 ].v   = z_obj->value[ w ].v;

    /* Nalewam z B do A. Jesli to wiecej niz A moze zmiescic, ta czesc trafia do
       B z powrotem, a w A jest tyle ile moze sie zmiescic.

       W przypadku fontann, nie ma znaczenia ich pojemnosc, w nich ilosc plynu
       sie nie zmienia, a pojemnik docelowy wypelniany jest do oporu. */
    if ( z_obj->item_type == ITEM_DRINK_CON )
    {
	int A = obj->value[ 1 ].v;
	int maxA = obj->value[ 0 ].v;
	int B = z_obj->value[ 1 ].v;

	A += B;
	if ( A > maxA )
	{
	    B = A - maxA;
	    A = maxA;
	}
	else
	    B = 0;

	obj->value[ 1 ].v   = URANGE( 0, A, maxA );
	z_obj->value[ 1 ].v = URANGE( 0, B, z_obj->value[ 0 ].v );
    }
    else
	obj->value[ 1 ].v = obj->value[ 0 ].v;

    oprog_use_trigger( ch, z_obj, NULL );

    return;
}


/*
 * Lam 17.6.2006
 */
KOMENDA( do_wylej )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];

    if ( !check_blind( ch ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co opr`o`zni`c?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	send_to_char( "Nie masz tego przy sobie.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "Nie mo`zesz z tego niczego wyla`c.\n\r", ch );
	return;
    }

    if ( obj->value[ 1 ].v == 0 )
    {
	send_to_char( "Tam ju`z jest pusto.\n\r", ch );
	return;
    }

    if ( obj->value[ 1 ].v < 0 )
    {
	bug( "nadpusty pojemnik", 0 );
	obj->value[ 1 ].v = 1;
    }

    if ( czy_ze( obj->dopelniacz ) )
    {
	act( ZM_ZAWSZE, "Wylewasz $r ze $G.", ch, liq_table_pl[ obj->value[ 2 ].v ].liq_biernik, obj, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wylewa $r ze $G.", ch, liq_table_pl[ obj->value[ 2 ].v ].liq_biernik, obj, TO_ROOM );
    }
    else
    {
	act( ZM_ZAWSZE, "Wylewasz $r z $G.", ch, liq_table_pl[ obj->value[ 2 ].v ].liq_biernik, obj, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wylewa $r z $G.", ch, liq_table_pl[ obj->value[ 2 ].v ].liq_biernik, obj, TO_ROOM );
    }

    obj->value[ 1 ].v = 0;

    return;
}


/*
 * Alandar 03.11.2004 - pojenie innych postaci trzymanym w reku pojemnikiem
 * oczywiscie dziala tez na slepe i pijane postacie
 * Muzgus  28.05.2006 - dodanie "napoj sie"
 */
KOMENDA( do_napoj )
{
    char arg[ MIL ];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    TIMER_DATA *timer;

    /* standardowe sprawdzenia */
    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	STC( "Kogo chcesz napoi`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	STC( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( !( obj = get_eq_char( ch, WEAR_HOLD ) )
      || ( obj->item_type != ITEM_DRINK_CON
        && obj->item_type != ITEM_POTION ) )
    {
	STC( "Nie trzymasz `zadnego pojemnika z p`lynem w r`eku.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	if ( ( timer = find_timer( ch, przestan_poic ) ) )
	{
	    del_timer( timer );
	    timer->fun( timer );
	}
	do_drink( ch, "z_r`eki" );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( !check_blind( ch ) )
	return;

    /* sprawdza czy ktos inny juz nie karmi postaci */
    for ( timer = timer_list; timer; timer = timer->next )
	if ( timer->fun == przestan_poic && ( CHAR_DATA * ) timer->arg[ 0 ].p == ch )
	{
	    STC( "Kto`s ju`z pr`obuje napoi`c t`e posta`c.\n\r", ch );
	    return;
	}

    /* jesli chce karmic nowa postac najpierw musi przestac karmic stara */
    if ( ( timer = find_timer( ch, przestan_poic ) ) )
    {
	del_timer( timer );
	timer->fun( timer );
    }

    act( ZM_ZAWSZE, "Podsuwasz $@ $j pod nos.", ch, obj, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n podsuwa $@ $j pod nos.", ch, obj, victim, TO_NOTVICT );
    act( ZM_PRZYT, "$n podsuwa ci $j pod nos.", ch, obj, victim, TO_VICT );

    /* nowy timer */
    timer = new_timer( );
    timer->ch = ch;
    timer->time = 40;
    timer->fun = przestan_poic;
    timer->arg[ 0 ].p = ( void * ) victim;
    timer->arg[ 1 ].p = ( void * ) obj;
    run_timer( timer );

    mprog_water_trigger( victim, ch, obj ); /* Alandar 22.12.2004 */
    /* zakladam, ze przewidzialem wszystkie okolicznosci przerwania karmienia,
       wiec tu tego juz nie sprawdzam */

    return;
}


/*
 * Funkcja timera wywolywana gdy postac straci cierpliwosc (10 sek)
 * i przestanie karmic a takze przy innych okazjach przerywajacych pojenie
 */
FUNKCJA_CZASOWA( przestan_poic )
{
    act( ZM_ZAWSZE, "Zabierasz $@ $j spod nosa.", timer->ch,
      ( OBJ_DATA * ) timer->arg[ 1 ].p, ( CHAR_DATA * ) timer->arg[ 0 ].p, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zabiera $@ $j spod nosa.", timer->ch,
      ( OBJ_DATA * ) timer->arg[ 1 ].p, ( CHAR_DATA * ) timer->arg[ 0 ].p, TO_NOTVICT );
    act( ZM_PRZYT, "$n zabiera ci $j spod nosa.", timer->ch,
      ( OBJ_DATA * ) timer->arg[ 1 ].p, ( CHAR_DATA * ) timer->arg[ 0 ].p, TO_VICT );

    return;
}


/*
 * Lam kwiecen-maj 98: "pij"
 * Alandar 12.10.2004 - Przebudowa funkcji i poprawki
 * Alandar 03.11.2004 - dodanie pojenia
 * Muzgus  28.05.2006 - dodanie "napoj sie"
 */
KOMENDA( do_drink )
{
    CHAR_DATA *vch = NULL;
    OBJ_DATA *obj = NULL;
    ROOM_INDEX_DATA *in_room = ch->in_room;
    char arg[ MAX_INPUT_LENGTH ];
    int amount;
    int liquid;
    bool poison = FALSE;
    bool czy_pojony = FALSE, czy_karmiony = FALSE;

    if ( IS_SET( race_table[ ch->race ].race_abilities, RACE_NO_MOUTH ) )
    {
	STC( "Nie masz czym pi`c.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( !IS_NPC( ch )
      && ch->level < LEVEL_HERO
      && ( ( !CZY_WAMPIR( ch ) && ch->pcdata->condition[ COND_THIRST ] > 40 )
	|| ( CZY_WAMPIR( ch ) && ch->pcdata->condition[ COND_FULL ] > 40 ) ) )
    {
	send_to_char( "Nie chce ci si`e pi`c.\n\r", ch );
	return;
    }

    if ( arg[ 0 ] == '\0' )
    {
	TIMER_DATA *timer;

	/* sprawdza czy ktos nie probuje napoic tej postaci */
	for ( timer = timer_list; timer; timer = timer->next )
	{
	    if ( timer->fun == przestan_poic && ( CHAR_DATA * ) timer->arg[ 0 ].p == ch )
	    {
		obj = ( OBJ_DATA * ) timer->arg[ 1 ].p;
		vch = timer->ch;

		del_timer( timer );
		czy_pojony = TRUE;
		break;
	    }
	    else if ( timer->fun == przestan_karmic && ( CHAR_DATA * ) timer->arg[ 0 ].p == ch )
	    {
		obj = ( OBJ_DATA * ) timer->arg[ 1 ].p;
		vch = timer->ch;

		czy_karmiony = TRUE;
	    }
	}

	/* zabezpieczenie przed padami */
	if ( czy_pojony || czy_karmiony )
	{
	    if ( vch->deleted || vch->in_room != ch->in_room )
	    {
		bug( "do_drink: znaleziono timer, ale postacie sa w roznych pomieszczeniach", 1 );
		STC( "Tw`oj dobroczy`nca gdzie`s znikn`a`l!\n\r", ch );
		return;
	    }
	    if ( obj->deleted || obj != get_eq_char( vch, WEAR_HOLD ) )
	    {
		bug( "do_drink: znaleziono timer, ale przedmiot gdzies sie zapodzial", 1 );
		STC( "Twoje picie gdzie`s si`e zapodzia`lo!\n\r", ch );
		return;
	    }
	}

	if ( !czy_pojony && czy_karmiony )
	{
	    act( ZM_ZAWSZE, "Nie mo`zesz pi`c z $g od $!.", ch, obj, vch, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT,
		"$n bezskutecznie pr`obuje si`e napi`c z $g od $!.",
		ch, obj, vch, TO_NOTVICT );
	    act( ZM_PRZYT, "$n bezskutecznie pr`obuje si`e napi`c z $g od ciebie.",
		ch, obj, vch, TO_VICT );
	    return;
	}

	if ( !czy_pojony )
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
		if ( obj->item_type == ITEM_FOUNTAIN )
		    break;

	if ( !obj )
	{
	    send_to_char( "Co pi`c?\n\r", ch );
	    return;
	}
    }
    else if ( !strcmp( arg, "z_r`eki" ) )
    {
	if ( !( obj = get_eq_char( ch, WEAR_HOLD ) ) )
	{
	    STC( "Nie trzymasz `zadnego pojemnika z p`lynem w r`eku.\n\r", ch );
	    return;
	}
    }

    /* jesli postac jest pojona to pomijamy te sprawdzenia */
    if ( !czy_pojony
      && strcmp( arg, "z_r`eki" ) )
    {
	if ( !check_blind( ch ) )
	    return;

	if ( arg[ 0 ] != '\0' && !( obj = get_obj_here( ch, arg ) ) )
	{
	    send_to_char( "Nie masz tego.\n\r", ch );
	    return;
	}

	if ( pijane_przedmioty( ch ) )
	    return;

	if ( !IS_NPC( ch ) && ch->pcdata->condition[ COND_DRUNK ] > 16
	  && number_bits( 2 ) ) /* Lam: 1/4 szans na trafienie do ust */
	{
	    /* Alandar: czy mozna nie trafic do ust z fontanny...? */
	    send_to_char( "Nie trafiasz do ust! *Ik!*\n\r", ch );
	    return;
	}
    }

    /* Vigud 29.9.2010: wydzielone z do_quaff */
    if ( obj->item_type == ITEM_POTION )
    {
	/* Alandar 04.10.2004: wreszcie obsluga tekstu do polykania */
	if ( obj->value[ 4 ].p )
	{
	    char bufor[ MSL ];

	    sprintf( bufor, "%s\n\r", (char *) obj->value[ 4 ].p );
	    send_to_char( bufor, ch );
	}
	else
	    act( ZM_ZAWSZE, "Po`lykasz $j.", ch, obj, NULL, TO_CHAR );

	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n po`lyka $j.", ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n odchyla na moment g`low`e.", ch, obj, NULL, TO_ROOM );

	if ( obj->level > ch->level )
	    act( ZM_ZAWSZE, "$p ma jak dla ciebie za wysoki poziom.", ch, obj, NULL, TO_CHAR );
	else
	{
	    int i;

	    for ( i = 1; i <= 3; i++ )
	    {
		obj_cast_spell( obj->value[ i ].v, obj->value[ 0 ].v, ch, ch, NULL, "si`e", USAGE_POTION );
		if ( !ch->in_room
		  || ch->deleted
		  || in_room != ch->in_room
		  || obj->deleted
		  || obj->carried_by != ch )
		{
		    if ( !obj->deleted
		      && ( !IS_NPC( ch )
			|| IS_AFFECTED( ch, AFF_CHARM ) ) )
		    {
			extract_obj( obj );
		    }
		    return;
		}
	    }
	    oprog_use_trigger( ch, obj, NULL );
	}

	if ( !obj->deleted
	  && ( !IS_NPC( ch )
	    || IS_AFFECTED( ch, AFF_CHARM ) ) )
	{
	    extract_obj( obj );
	}
	return;
    }

    /* najpierw sprawdza jaki plyn, czy jest zatruty i ile go jest */
    switch ( obj->item_type )
    {
	default:
	    send_to_char( "Nie mo`zesz z tego pi`c.\n\r", ch );
	    return;

	case ITEM_FOUNTAIN:
	    if ( obj->value[ 4 ].v != 0
	      && !IS_SET( race_table[ ch->race ].race_abilities, RACE_POISONIMM )
	      && !CZY_WAMPIR( ch ) )
		poison = TRUE;
	    liquid = obj->value[ 5 ].v;
	    amount = 8;
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[ 3 ].v != 0
	      && !IS_SET( race_table[ ch->race ].race_abilities, RACE_POISONIMM )
	      && !CZY_WAMPIR( ch ) )
		poison = TRUE;
	    liquid = obj->value[ 2 ].v;
	    amount = number_range( 5, 8 ); /* Lam: w Envy bylo 3-10 */
	    amount = UMIN( amount, obj->value[ 1 ].v );
	    obj->value[ 1 ].v -= amount;
	    break;
    }

    /* wampiry nie powinny pic wody i innych plynow z wody sie skladajacych */
    /* zmiana koncepcji
    if ( CZY_WAMPIR( ch ) && plyn != 2 && plyn != 3 && plyn != 4 && plyn != 5
      && plyn != 7 && plyn != 8 && plyn != LIQ_BLOOD )
	poison = TRUE;*/

    /* ograniczenie na plyn, zeby nie wywalac Laca dajac plyn spoza tablicy */
    if ( liquid < 0 || liquid >= LIQ_MAX )
    {
	bug( "do_drink: Niepoprawny numer plynu: %d.", liquid );
	liquid = LIQ_WATER;
    }

    /* z pustego nie da sie pic */
    if ( amount == 0 )
    {
	char buffer[ MSL ];

	if ( can_see_obj( ch, obj ) )
	    sprintf( buffer, "$p %s zupe`lnie pust%s.",
		obj->rodzaj == 3 ? "s`a" : "jest",
		obj->rodzaj == 1 ? "y" : obj->rodzaj == 2 ? "a" : "e" );
	else
	    strcpy( buffer, "Pojemnik jest zupe`lnie pusty." );
	act( ZM_ZAWSZE, buffer, ch, obj, NULL, TO_CHAR );
	return;
    }

    /* tekst do picia */
    if ( czy_pojony )
    {
	if ( can_see_obj( ch, obj ) && czy_ze( obj->dopelniacz ) )
	{
	    act( ZM_ZAWSZE, "Pijesz ze $g od $!.", ch, obj, vch, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n pije ze $g od $!.", ch, obj, vch, TO_NOTVICT );
	    act( ZM_PRZYT, "$n pije ze $g od ciebie.", ch, obj, vch, TO_VICT );
	}
	else
	{
	    act( ZM_ZAWSZE, "Pijesz z $g od $!.", ch, obj, vch, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n pije z $g od $!.", ch, obj, vch, TO_NOTVICT );
	    act( ZM_PRZYT, "$n pije z $g od ciebie.", ch, obj, vch, TO_VICT );
	}
    }
    else if ( can_see_obj( ch, obj ) && czy_ze( obj->dopelniacz ) )
    {
	act( ZM_ZAWSZE, "Pijesz $R ze $g.",
	  ch, obj, liq_table_pl[ liquid ].liq_biernik, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pije $R ze $g.",
	  ch, obj, liq_table_pl[ liquid ].liq_biernik, TO_ROOM );
    }
    else
    {
	act( ZM_ZAWSZE, "Pijesz $R z $g.",
	    ch, obj, liq_table_pl[ liquid ].liq_biernik, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pije $R z $g.",
	    ch, obj, liq_table_pl[ liquid ].liq_biernik, TO_ROOM );
    }

    if ( !IS_NPC( ch ) )
    {
	int cond_full, cond_thirst, cond_drunk;

	cond_full = ch->pcdata->condition[ COND_FULL ];
	cond_thirst = ch->pcdata->condition[ COND_THIRST ];
	cond_drunk = ch->pcdata->condition[ COND_DRUNK ];

	gain_condition( ch, COND_DRUNK,
	  amount * liq_table_pl[ liquid ].liq_affect[ COND_DRUNK ]
	  / UMAX( 2, ROZMIAR( ch ) ) ); /* ta linijka: Lam 11.5.98 */

	if ( !CZY_WAMPIR( ch ) )
	{
	    gain_condition( ch, COND_FULL,
	      amount * liq_table_pl[ liquid ].liq_affect[ COND_FULL ] );
	    gain_condition( ch, COND_THIRST,
	      amount * liq_table_pl[ liquid ].liq_affect[ COND_THIRST ] );
	}
	/* If blood */
	else if ( liquid == LIQ_BLOOD )
	{
	    gain_condition( ch, COND_FULL, amount * 2 );
	    gain_condition( ch, COND_THIRST, amount );
	}

	/* Vigud: to raczej powinno byc w gain_condition() */
	if ( ch->pcdata->condition[ COND_FULL ] > 40
	  && ch->pcdata->condition[ COND_FULL ] != cond_full  )
	    ASTC( "Czujesz si`e najedzon$y.", ch );
	else if ( cond_full < 3 && ch->pcdata->condition[ COND_FULL ] > 3 )
	    ASTC( "Nie jeste`s ju`z g`lodn$y.", ch );

	if ( ch->pcdata->condition[ COND_THIRST ] > 40
	  && ch->pcdata->condition[ COND_THIRST ] != cond_thirst  )
	    ASTC( "W pe`lni zaspokoi`l$o`s swoje pragnienie.", ch );
	else if ( cond_thirst < 3 && ch->pcdata->condition[ COND_THIRST ] > 3 )
	    ASTC( "Nie jeste`s ju`z spragnion$y.", ch );

	if ( ch->pcdata->condition[ COND_DRUNK ] > 10
	  && ch->pcdata->condition[ COND_DRUNK ] != cond_drunk )
	    ASTC( "Jeste`s pijan$y.", ch );
    }

    if ( poison == TRUE )
    {
	/* The shit was poisoned! */
	AFFECT_DATA af;

	send_to_char( "Krztusisz si`e i d`lawisz.\n\r", ch );
	act( ZM_WZROK | ZM_SLUCH, "$n krztusi si`e i d`lawi.", ch, NULL, NULL, TO_ROOM );
	af.type      = gsn_poison;
	af.duration  = 3 * amount;
	af.location  = APPLY_STR;
	af.modifier  = -2;
	af.level     = 0;
	af.caster    = NULL;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_POISON );
	affect_join( ch, &af, ENH_AFFECT );
    }

    oprog_use_trigger( ch, obj, NULL );

    if ( ch->in_room != in_room
      || ch->deleted
      || obj->deleted )
	return;

    /* wymagam, aby ten if byl ostatni przed returnem, bo zawiera wlasny wyskok
       z funkcji, a takze psuje zmienna ch */
    if ( obj->item_type == ITEM_DRINK_CON && obj->value[ 1 ].v <= 0 )
    {
	if ( czy_pojony )
	    ch = vch;

	/* Lam 17.6.2006: opc +pojemniki */
	if ( CZY_OPCJA( ch, OPC_POJEMNIKI ) )
	    return;

	/* Alandar 04.10.2004: obsluga roznych tekstow znikania pojemnika */
	if ( obj->value[ 4 ].p )
	{
	    char bufor[ MSL ];

	    sprintf( bufor, "%s\n\r", (char *) obj->value[ 4 ].p );
	    send_to_char( bufor, ch );
	}
	else
	{
	    send_to_char( "Po`swi`ecasz pusty pojemnik bogom.\n\r", ch );
	    ch->gold += UMAX( obj->level, 10 );
	}

	extract_obj( obj );
    }

    return;
}


/*
 * Alandar 03.11.2004 - karmienie postaci pozywieniem trzymanym w reku.
 * Dziala tez na slepe i pijane postacie.
 * Muzgus  28.05.2006 - dodanie "nakarm sie"
 */
KOMENDA( do_nakarm )
{
    char arg[ MIL ];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    TIMER_DATA *timer;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	STC( "Kogo chcesz nakarmi`c?\n\r", ch );
	return;
    }
    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	STC( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( !( obj = get_eq_char( ch, WEAR_HOLD ) )
      || ( obj->item_type != ITEM_FOOD
        && obj->item_type != ITEM_PILL ) )
    {
	STC( "Nie trzymasz `zadnego jedzenia w r`eku.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	if ( ( timer = find_timer( ch, przestan_karmic ) ) )
	{
	    del_timer( timer );
	    timer->fun( timer );
	}
	do_eat( ch, "z_r`eki" );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( !check_blind( ch ) )
	return;

    /* sprawdza czy ktos inny juz nie karmi postaci */
    for ( timer = timer_list; timer; timer = timer->next )
	if ( timer->fun == przestan_karmic && ( CHAR_DATA * ) timer->arg[ 0 ].p == ch )
	{
	    STC( "Kto`s ju`z pr`obuje nakarmi`c t`e posta`c.\n\r", ch );
	    return;
	}

    /* jesli postac chce nakarmic kogos innego zanim nakarmi poprzedniego
       przestaje karmic tego poprzedniego */
    if ( ( timer = find_timer( ch, przestan_karmic ) ) )
    {
	del_timer( timer );
	timer->fun( timer );
    }

    act( ZM_ZAWSZE, "Podsuwasz $@ $j pod nos.", ch, obj, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n podsuwa $@ $j pod nos.", ch, obj, victim, TO_NOTVICT );
    act( ZM_PRZYT, "$n podsuwa ci $j pod nos.", ch, obj, victim, TO_VICT );

    /* nowy timer */
    timer = new_timer( );
    timer->ch = ch;
    timer->time = 40;
    timer->fun = przestan_karmic;
    timer->arg[ 0 ].p = ( void * ) victim;
    timer->arg[ 1 ].p = ( void * ) obj;
    run_timer( timer );

    mprog_feed_trigger( victim, ch, obj ); /* Alandar 22.12.2004 */

    return;
}


/*
 * Funkcja wywolywana gdy postac przestaje karmic po 10 sekundach lub recznie
 * przy innych okazjach
 */
FUNKCJA_CZASOWA( przestan_karmic )
{
    act( ZM_ZAWSZE, "Zabierasz $@ $j spod nosa.", timer->ch,
      ( OBJ_DATA * ) timer->arg[ 1 ].p, ( CHAR_DATA * ) timer->arg[ 0 ].p, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zabiera $@ $j spod nosa.", timer->ch,
      ( OBJ_DATA * ) timer->arg[ 1 ].p, ( CHAR_DATA * ) timer->arg[ 0 ].p, TO_NOTVICT );
    act( ZM_PRZYT, "$n zabiera ci $j spod nosa.", timer->ch,
      ( OBJ_DATA * ) timer->arg[ 1 ].p, ( CHAR_DATA * ) timer->arg[ 0 ].p, TO_VICT );

    return;
}


/*
 * Alandar 03.11.2004: dodanie karmienia
 * Muzgus  28.05.2006: dodanie "nakarm sie"
 */
KOMENDA( do_eat )
{
    CHAR_DATA	*vch = NULL;
    OBJ_DATA	*obj = NULL;
    char	 arg[ MAX_INPUT_LENGTH ];
    bool	 czy_karmiony = FALSE, czy_pojony = FALSE;
    ROOM_INDEX_DATA *room = ch->in_room;

    one_argument( argument, arg );

    if ( IS_SET( race_table[ ch->race ].race_abilities, RACE_NO_MOUTH ) )
    {
	STC( "Nie masz czym je`s`c.\n\r", ch );
	return;
    }

    /* najpierw czy jedzenie ma gdzie sie pomiescic */
    if ( ch->level < LEVEL_HERO
      && !IS_NPC( ch )
      && ch->pcdata->condition[ COND_FULL ] > 40 )
    {
	ASTC( "Wi`ecej w siebie nie wepchniesz. Jeste`s pe`ln$0.", ch );
	return;
    }

    if ( arg[ 0 ] == '\0' )
    {
	TIMER_DATA *timer;

	/* sprawdza czy ktos tej postaci nie probuje nakarmic */
	for ( timer = timer_list; timer; timer = timer->next )
	{
	    if ( timer->fun == przestan_karmic && ( CHAR_DATA * ) timer->arg[ 0 ].p == ch )
	    {
		vch = timer->ch;
		obj = ( OBJ_DATA * ) timer->arg[ 1 ].p;

		del_timer( timer );
		czy_karmiony = TRUE;
		break;
	    }
	    else if ( timer->fun == przestan_poic && ( CHAR_DATA * ) timer->arg[ 0 ].p == ch )
	    {
		vch = timer->ch;
		obj = ( OBJ_DATA * ) timer->arg[ 1 ].p;

		czy_pojony = TRUE;
	    }
	}
	/* zabezpieczenie przed padami */
	if ( czy_pojony || czy_karmiony )
	{
	    if ( vch->deleted || vch->in_room != ch->in_room )
	    {
		bug( "do_eat: znaleziono timer, ale postacie sa w roznych pomieszczeniach", 1 );
		STC( "Tw`oj dobroczy`nca gdzie`s znikn`a`l!\n\r", ch );
		return;
	    }
	    if ( obj->deleted || obj != get_eq_char( vch, WEAR_HOLD ) )
	    {
		bug( "do_eat: znaleziono timer, ale przedmiot gdzies sie zapodzial", 1 );
		STC( "Twoje jedzenie gdzie`s si`e zapodzia`lo!\n\r", ch );
		return;
	    }
	}

	if ( czy_karmiony )
	{
	    act( ZM_ZAWSZE, "Zjadasz $j od $!.", ch, obj, vch, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n zjada $j od $!.", ch, obj, vch, TO_NOTVICT );
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT, "$n zjada $j od $!.", ch, obj, vch, TO_NOTVICT );
	    act( ZM_PRZYT, "$n zjada $j od ciebie.", ch, obj, vch, TO_VICT );
	}
	else if ( czy_pojony )
	{
	    act( ZM_ZAWSZE, "Nie mo`zesz zje`s`c $g od $!.", ch, obj, vch, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n bezskutecznie pr`obuje ugry`x`c $j od $!.",
		ch, obj, vch, TO_NOTVICT );
	    act( ZM_PRZYT, "$n bezskutecznie pr`obuje ugry`x`c $j od ciebie.",
		ch, obj, vch, TO_VICT );
	    return;
	}
    }
    else if ( !strcmp( arg, "z_r`eki" ) )
    {
	if ( !( obj = get_eq_char( ch, WEAR_HOLD ) ) )
	{
	    STC( "Nie trzymasz `zadnego jedzenia w r`eku.\n\r", ch );
	    return;
	}
    }

    /* pomijamy inne sprawdzenia jesli postac jest karmiona */
    if ( !czy_karmiony
       && strcmp( arg, "z_r`eki" ) )
    {
	if ( !check_blind( ch ) )
	    return;

	if ( arg[ 0 ] == '\0' )
	{
	    send_to_char( "Co skonsumowa`c?\n\r", ch );
	    return;
	}

	if ( pijane_przedmioty( ch ) )
	    return;

	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
	    send_to_char( "Nie masz tego przy sobie.\n\r", ch );
	    return;
	}

	if ( ch->level < 106 && obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
	{
	    send_to_char( "To nie jest jadalne.\n\r", ch );
	    return;
	}
    }
    if ( !czy_karmiony )
    {
	/* Alandar 04.10.2004: obsluga roznych opisow konsumowania dla gracza */
	if ( ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_PILL )
	  && obj->value[ 4 ].p )
	{
	    char bufor[ MSL ];

	    sprintf( bufor, "%s\n\r", (char *) obj->value[ 4 ].p );
	    send_to_char( bufor, ch );
	}
	else
	    act( ZM_ZAWSZE, "Konsumujesz $j.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n konsumuje $j.", ch, obj, NULL, TO_ROOM );
    }

    switch ( obj->item_type )
    {
	case ITEM_FOOD:
	    if ( !IS_NPC( ch ) )
	    {
		int cond_full, cond_thirst, cond_drunk;

		cond_full = ch->pcdata->condition[ COND_FULL ];
		cond_thirst = ch->pcdata->condition[ COND_THIRST ];
		cond_drunk = ch->pcdata->condition[ COND_DRUNK ];

		if ( !CZY_WAMPIR( ch ) )
		{
		    /* chwilowo takie dziwne wzorki, potem nie bedzie juz w0
		       tylko ciezar */
		    /* na razie tylko komunikaty */
		    gain_condition( ch, COND_FULL, obj->value[ 0 ].v );
/*		    gain_condition( ch, COND_FULL,
		      obj->value[ 0 ].v * food_table[ obj->value[ 2 ].v ].food_affect[ COND_FULL ] / 5 );
		    gain_condition( ch, COND_THIRST,
		      obj->value[ 0 ].v * food_table[ obj->value[ 2 ].v ].food_affect[ COND_THIRST ] / 5 );
		    gain_condition( ch, COND_DRUNK,
		      obj->value[ 0 ].v * food_table[ obj->value[ 2 ].v ].food_affect[ COND_DRUNK ] / 5 );*/
		}

		/* Vigud: to raczej powinno byc w gain_condition() */
		if ( ch->pcdata->condition[ COND_FULL ] > 40
		  && ch->pcdata->condition[ COND_FULL ] != cond_full  )
		    ASTC( "Czujesz si`e najedzon$y.", ch );
		else if ( cond_full < 3 && ch->pcdata->condition[ COND_FULL ] > 3 )
		    ASTC( "Nie jeste`s ju`z g`lodn$y.", ch );

		if ( ch->pcdata->condition[ COND_THIRST ] > 40
		  && ch->pcdata->condition[ COND_THIRST ] != cond_thirst  )
		    ASTC( "W pe`lni zaspokoi`l$o`s swoje pragnienie.", ch );
		else if ( cond_thirst < 3 && ch->pcdata->condition[ COND_THIRST ] > 3 )
		    ASTC( "Nie jeste`s ju`z spragnion$y.", ch );

		if ( ch->pcdata->condition[ COND_DRUNK ] > 10
		  && ch->pcdata->condition[ COND_DRUNK ] != cond_drunk )
		    ASTC( "Jeste`s pijan$y.", ch );
	    }

	    if ( obj->value[ 3 ].v != 0
	      && !CZY_WAMPIR( ch )
	      && !IS_SET( race_table[ ch->race ].race_abilities, RACE_POISONIMM ) )
	    {
		/* The shit was poisoned! */
		AFFECT_DATA af;

		act( ZM_WZROK | ZM_SLUCH, "$n krztusi si`e i d`lawi.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "Krztusisz si`e i d`lawisz.\n\r", ch );

		af.type     = gsn_poison;
		af.duration = 2 * obj->value[ 0 ].v;
		af.location = APPLY_STR;
		af.modifier = -2;
		af.level    = 0;
		af.caster   = NULL;
		CLEAR_BITS( af.bitvector );
		SET_BIT_V( af.bitvector, AFF_POISON );
		affect_join( ch, &af, ENH_AFFECT );
	    }
	    break;

	case ITEM_PILL:
	    if ( obj->level > ch->level )
		if ( obj->rodzaj == 3 )
		    act( ZM_ZAWSZE, "$p maj`a dla ciebie zbyt zaawansowan`a struktur`e chemiczn`a.", ch, obj, NULL, TO_CHAR );
		else
		    act( ZM_ZAWSZE, "$p ma dla ciebie zbyt zaawansowan`a struktur`e chemiczn`a.", ch, obj, NULL, TO_CHAR );
	    else
	    {
		obj_cast_spell( obj->value[ 1 ].v, obj->value[ 0 ].v, ch, ch, NULL, "si`e", USAGE_PILL );
		if ( obj->deleted )
		    return;
		if ( !ch->in_room || ch->deleted || room != ch->in_room )
		{
		    extract_obj( obj );
		    return;
		}

		obj_cast_spell( obj->value[ 2 ].v, obj->value[ 0 ].v, ch, ch, NULL, "si`e", USAGE_PILL );
		if ( obj->deleted )
		    return;
		if ( !ch->in_room || ch->deleted || room != ch->in_room )
		{
		    extract_obj( obj );
		    return;
		}

		obj_cast_spell( obj->value[ 3 ].v, obj->value[ 0 ].v, ch, ch, NULL, "si`e", USAGE_PILL );
		if ( obj->deleted )
		    return;
		if ( !ch->in_room || ch->deleted || room != ch->in_room )
		{
		    extract_obj( obj );
		    return;
		}
	    }
	    break;
    }

    oprog_use_trigger( ch, obj, NULL );

    /* Lam 1.7.2005: w tym miejscu ch mogl zginac, a przedmiot pojsc do ciala,
       wtedy tez ma z niego zniknac */
    if ( !obj->deleted
      && ( !IS_NPC( ch )
        || IS_AFFECTED( ch, AFF_CHARM )
        || czy_karmiony ) )
    {
	extract_obj( obj );
    }

    return;
}


/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( !( obj = get_eq_char( ch, iWear ) ) )
	return TRUE;

    if ( !fReplace )
	return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
    {
	act( ZM_ZAWSZE, "Nie mo`zesz zdj`a`c $g.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }

    /* jesli zdejmuje trzymane jedzenie, przestaje automatycznie karmic */
    if ( iWear == WEAR_HOLD )
    {
	TIMER_DATA *timer;

	for ( timer = timer_list; timer; timer = timer->next )
	    if ( ( timer->fun == przestan_karmic || timer->fun == przestan_poic )
	      && ( OBJ_DATA * ) timer->arg[ 1 ].p == obj )
	    {
		del_timer( timer );
		timer->fun( timer );
		break;
	    }
    }

    unequip_char( ch, obj );
    /* Alandar: po wprowadzeniu trzymania ten komunikat troche nie pasuje */
    act( ZM_WZROK | ZM_WID_CHAR, "$n zaprzestaje u`zywania $g.", ch, obj, NULL, TO_ROOM );
    act( ZM_ZAWSZE, "Zaprzestajesz u`zywania $g.", ch, obj, NULL, TO_CHAR );
    oprog_remove_trigger( ch, obj );

    return TRUE;
}


/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 *
 * Lam 22.7.98: wear_prog i remove_prog. Sprawdzane jest, czy remove_prog nie
 * zabil przypadkiem gracza.
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, bool hold )
{
    char buf [ MAX_STRING_LENGTH ];
    ROOM_INDEX_DATA *room = ch->in_room;

    if ( ch->level < obj->level )
    {
	sprintf( buf, "Do u`zywania %s potrzebujesz poziomu %d.\n\r",
	    can_see_obj( ch, obj ) ? obj->dopelniacz : "tego",
	    obj->level );
	send_to_char( buf, ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje u`zy`c $g, ale jest zbyt niedo`swiadczon$y.",
	    ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( hold || CAN_WEAR( obj, ITEM_HOLD ) )
    {
	if ( !remove_obj( ch, WEAR_WIELD_2, fReplace )
	  || ch->deleted || room != ch->in_room

	  || !remove_obj( ch, WEAR_HOLD, fReplace )
	  || ch->deleted || room != ch->in_room

	  || !remove_obj( ch, WEAR_LIGHT, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_TRZYMANE ) )
	{
	    send_to_char( "Nie mo`zesz niczego trzyma`c.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Bierzesz $j w swe r`ece.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n bierze $j w swoje r`ece.", ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ekami.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_HOLD ) && CAN_WEAR( obj, ITEM_HOLD ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( obj->item_type == ITEM_LIGHT )
    {
	if ( !remove_obj( ch, WEAR_WIELD_2, fReplace )
	  || ch->deleted || room != ch->in_room

	  || !remove_obj( ch, WEAR_HOLD, fReplace )
	  || ch->deleted || room != ch->in_room

	  || !remove_obj( ch, WEAR_LIGHT, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_TRZYMANE ) )
	{
	    send_to_char( "Nie mo`zesz u`zywa`c `swiat`la.\n\r", ch );
	    return;
	}

	/* Muzgus: uniemozliwienie zapalenia swiatla palacego sie zwyklym ogniem
	   pod woda */
	if ( obj->value[ 1 ].v == 1
	   && ch->in_room->sector_type == SECT_UNDERWATER )
	{
	    send_to_char( "Pr`obujesz skrzesa`c ognie`n pod wod`a, jednak bez powodzenia.\n\r", ch );
	    return;
	}

	/* Lam 8.2.2005: obsluga wypalonych zrodel swiatla */
	if ( obj->value[ 2 ].v == 0 )
	{
	    act( ZM_ZAWSZE, "Bierzesz $j w r`ece, ale nie potrafisz wydoby`c `swiat`la.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n bierze $j w r`ece, ale nie potrafi wydoby`c `swiat`la.", ch, obj, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ekami.", ch, obj, NULL, TO_ROOM );
	}
	else
	{
	    /* Lam 3.12.2004: zroznicowanie komunikatow */
	    if ( obj->value[ 1 ].v == 1 )
	    {
		act( ZM_ZAWSZE, "Zapalasz $j i unosisz nad g`low`e.",  ch, obj, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zapala $j i unosi nad g`low`e.", ch, obj, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "R`eka $1 zaczyna `swieci`c.", ch, obj, NULL, TO_ROOM );
	    }
	    else
	    {
		act( ZM_ZAWSZE, "Przy`swiecasz sobie $k.",  ch, obj, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n przy`swieca sobie $k.", ch, obj, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "R`eka $1 zaczyna `swieci`c.", ch, obj, NULL, TO_ROOM );
	    }
	}
	if ( equip_char( ch, obj, WEAR_LIGHT ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( get_eq_char( ch, WEAR_FINGER_L )
	  && get_eq_char( ch, WEAR_FINGER_R )
	  && !remove_obj( ch, WEAR_FINGER_L, fReplace )
	  && !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
	    return;

	if ( ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_PALEC ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na palce.\n\r", ch );
	    return;
	}

	if ( !get_eq_char( ch, WEAR_FINGER_L ) )
	{
	    act( ZM_ZAWSZE, "Zak`ladasz $j na palec serdeczny swojej lewej r`eki.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wk`lada $j na serdeczny palec swojej lewej r`eki.", ch, obj, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n zaciera r`ece.", ch, obj, NULL, TO_ROOM );
	    if ( equip_char( ch, obj, WEAR_FINGER_L ) )
		oprog_wear_trigger( ch, obj );
	    return;
	}

	if ( !get_eq_char( ch, WEAR_FINGER_R ) )
	{
	    act( ZM_ZAWSZE, "Zak`ladasz $j na palec serdeczny swojej prawej r`eki.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wk`lada $j na serdeczny palec swojej prawej r`eki.",   ch, obj, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n zaciera r`ece.", ch, obj, NULL, TO_ROOM );
	    if ( equip_char( ch, obj, WEAR_FINGER_R ) )
		oprog_wear_trigger( ch, obj );
	    return;
	}

	bug( "Wear_obj: no free finger.", 0 );
	send_to_char( "Ju`z nosisz dwa pier`scienie.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( get_eq_char( ch, WEAR_NECK_1 )
	  && get_eq_char( ch, WEAR_NECK_2 )
	  && !remove_obj( ch, WEAR_NECK_1, fReplace )
	  && !remove_obj( ch, WEAR_NECK_2, fReplace ) )
	    return;

	if ( ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_SZYJA ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na szyj`e.\n\r", ch );
	    return;
	}

	if ( !get_eq_char( ch, WEAR_NECK_1 ) )
	{
	    act( ZM_ZAWSZE, "Zak`ladasz $j na sw`a szyj`e.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wk`lada $j na swoj`a szyj`e.",   ch, obj, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n kiwa g`low`a.", ch, obj, NULL, TO_ROOM );
	    if ( equip_char( ch, obj, WEAR_NECK_1 ) )
		oprog_wear_trigger( ch, obj );
	    return;
	}

	if ( !get_eq_char( ch, WEAR_NECK_2 ) )
	{
	    act( ZM_ZAWSZE, "Zak`ladasz $j na sw`a szyj`e.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wk`lada $j na swoj`a szyj`e.",   ch, obj, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n kiwa g`low`a.", ch, obj, NULL, TO_ROOM );
	    if ( equip_char( ch, obj, WEAR_NECK_2 ) )
		oprog_wear_trigger( ch, obj );
	    return;
	}

	bug( "Wear_obj: no free neck.", 0 );
	send_to_char( "Ju`z masz szyj`e obci`a`zon`a.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( !remove_obj( ch, WEAR_BODY, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_CIALO ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na swe cia`lo.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Zak`ladasz $j na swe cia`lo.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zak`lada $j na swoje cia`lo.",   ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n si`e wierci.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_BODY ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( !remove_obj( ch, WEAR_HEAD, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_GLOWA ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na g`low`e.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Zak`ladasz $j na swoj`a g`low`e.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zak`lada $j na swoj`a g`low`e.",   ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n kr`eci g`low`a.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_HEAD ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FACE ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( !remove_obj( ch, WEAR_FACE, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_TWARZ ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na twarz.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Zak`ladasz $j na swoj`a twarz.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zak`lada $j na swoj`a twarz.",   ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n `lapie si`e za twarz.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_FACE ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( !remove_obj( ch, WEAR_LEGS, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_NOGI ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na nogi.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Zak`ladasz $j na swoje nogi.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zak`lada $j na swoje nogi.",   ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n robi przysiady i masuje swoje nogi.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_LEGS ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( !remove_obj( ch, WEAR_FEET, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_STOPY ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na stopy.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Wzuwasz $j.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wzuwa $j.", ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n masuje swoje stopy.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_FEET ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( !remove_obj( ch, WEAR_HANDS, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_DLONIE ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na d`lonie.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Wk`ladasz $j na swe d`lonie.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wk`lada $j na swoje d`lonie.",   ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n zaciera r`ece.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_HANDS ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( !remove_obj( ch, WEAR_ARMS, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_RAMIONA ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na ramiona.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Zak`ladasz $j na swe ramiona.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zak`lada $j na swoje ramiona.",   ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n g`laszcze si`e po ramionach.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_ARMS ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( !remove_obj( ch, WEAR_ABOUT, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_WOKOL ) )
	{
	    send_to_char( "Nie mo`zesz si`e w nic owija`c.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Owijasz $j wok`o`l swego cia`la.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n owija $j wok`o`l swojego cia`la.", ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n drapie si`e po ramionach.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_ABOUT ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( !remove_obj( ch, WEAR_WAIST, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_PAS ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na biodra.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Zak`ladasz $j wok`o`l swych bioder.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zak`lada $j wok`o`l swoich bioder.",   ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n drapie si`e po brzuchu.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_WAIST ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	if ( get_eq_char( ch, WEAR_WRIST_L )
	  && get_eq_char( ch, WEAR_WRIST_R )
	  && !remove_obj( ch, WEAR_WRIST_L, fReplace )
	  && !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
	    return;

	if ( ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_NADGARSTEK ) )
	{
	    send_to_char( "Nie mo`zesz zak`lada`c niczego na nadgarstki.\n\r", ch );
	    return;
	}

	if ( !get_eq_char( ch, WEAR_WRIST_L ) )
	{
	    act( ZM_ZAWSZE, "Wk`ladasz $j na sw`oj lewy nadgarstek.",
		ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wk`lada $j na sw`oj lewy nadgarstek.",
		ch, obj, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n rozmasowuje sobie lewy nadgarstek.", ch, obj, NULL, TO_ROOM );
	    if ( equip_char( ch, obj, WEAR_WRIST_L ) )
		oprog_wear_trigger( ch, obj );
	    return;
	}

	if ( !get_eq_char( ch, WEAR_WRIST_R ) )
	{
	    act( ZM_ZAWSZE, "Wk`ladasz $j na sw`oj prawy nadgarstek.",
		ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wk`lada $j na sw`oj prawy nadgarstek.",
		ch, obj, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n rozmasowuje sobie prawy nadgarstek.", ch, obj, NULL, TO_ROOM );
	    if ( equip_char( ch, obj, WEAR_WRIST_R ) )
		oprog_wear_trigger( ch, obj );
	    return;
	}

	bug( "Wear_obj: no free wrist.", 0 );
	send_to_char( "Ju`z masz nadgarstki zaj`ete.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
	if ( !remove_obj( ch, WEAR_WIELD_2, fReplace )
	  || ch->deleted || room != ch->in_room

	  || !remove_obj( ch, WEAR_SHIELD, fReplace )
	  || ch->deleted || room != ch->in_room )
	    return;

	if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_TARCZA ) )
	{
	    send_to_char( "Nie mo`zesz u`zywa`c tarcz.\n\r", ch );
	    return;
	}
	act( ZM_ZAWSZE, "Zak`ladasz $j na przedrami`e.", ch, obj, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zak`lada $j na przedrami`e.", ch, obj, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ekami.", ch, obj, NULL, TO_ROOM );
	if ( equip_char( ch, obj, WEAR_SHIELD ) )
	    oprog_wear_trigger( ch, obj );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD ) )
    {
	/* Lam 25.10.2000: nie probowac zalozyc jesli gracz ma umiejetnosc
	   'dwie bronie' na 0% */
	if ( IS_NPC( ch )
	  || ( moze_uzyc( ch, gsn_dual )
	    && ch->pcdata->learned[ gsn_dual ] ) )
	{
	    /*
	     * If you think this looks ugly now, just imagine how
	     * I felt trying to write it!  --- Thelonius (Monk)
	     * Now, it will attempt to wear in 1 if open, then 2 if
	     * open, else replace 2, else replace 1.
	     */
	    if ( get_eq_char( ch, WEAR_WIELD  )
		&& ( ( get_eq_char( ch, WEAR_WIELD_2 )
		      && !remove_obj( ch, WEAR_WIELD_2, fReplace ) )
		    || !remove_obj( ch, WEAR_HOLD,    fReplace )
		    || !remove_obj( ch, WEAR_SHIELD,  fReplace )
		    || !remove_obj( ch, WEAR_LIGHT,   fReplace ) )
		&&   !remove_obj( ch, WEAR_WIELD, fReplace ) )
		return;

	    if ( ch->deleted || room != ch->in_room )
		return;

	    if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_BRON ) )
	    {
		send_to_char( "Nie mo`zesz u`zywa`c broni.\n\r", ch );
		return;
	    }

	    if ( !get_eq_char( ch, WEAR_WIELD ) )
	    {
		OBJ_DATA *other_weapon;
		int       weight = 0;

		if ( ( other_weapon = get_eq_char( ch, WEAR_WIELD_2 ) ) )
		    weight = get_obj_weight( other_weapon );

		if ( weight + get_obj_weight( obj )
		    > str_app[ get_curr_str( ch ) ].wield )
		{
		    STC( "To dla ciebie za ci`e`zkie.\n\r", ch );
		    return;
		}

		act( ZM_ZAWSZE, "Wyci`agasz jako bro`n $j.", ch, obj, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wyci`aga $j jako bro`n.", ch, obj, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ek`a.", ch, obj, NULL, TO_ROOM );
		if ( equip_char( ch, obj, WEAR_WIELD ) )
		    oprog_wear_trigger( ch, obj );
		return;
	    }

	    if ( !get_eq_char( ch, WEAR_WIELD_2 ) )
	    {
		OBJ_DATA *primary_weapon;
		int       weight;

		primary_weapon = get_eq_char( ch, WEAR_WIELD );
		weight = get_obj_weight( primary_weapon );

		if ( weight + get_obj_weight( obj )
		    > str_app[ get_curr_str( ch ) ].wield )
		{
		    STC( "To dla ciebie za ci`e`zkie.\n\r", ch );
		    return;
		}

		act( ZM_ZAWSZE, "Wyci`agasz jako bro`n $j.", ch, obj, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wyci`aga $j jako bro`n.", ch, obj, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ek`a.", ch, obj, NULL, TO_ROOM );
		if ( equip_char( ch, obj, WEAR_WIELD_2 ) )
		    oprog_wear_trigger( ch, obj );
		return;
	    }
	    bug( "Wear_obj: no free weapon slot.", 0 );
	    send_to_char( "Ju`z nosisz dwie bronie.\n\r", ch );
	    return;
	}
	else /* can only wield one weapon */
	{
	    if ( !remove_obj( ch, WEAR_WIELD, fReplace )
	      || ch->deleted || room != ch->in_room )
		return;

	    if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_BRON ) )
	    {
		send_to_char( "Nie mo`zesz u`zywa`c broni.\n\r", ch );
		return;
	    }

	    if ( get_obj_weight( obj ) > str_app[ get_curr_str( ch ) ].wield )
	    {
		send_to_char( "To dla ciebie za ci`e`zkie.\n\r", ch );
		return;
	    }

	    act( ZM_ZAWSZE, "Wyci`agasz jako bro`n $j.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wyci`aga $j jako bro`n.", ch, obj, NULL, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ek`a.", ch, obj, NULL, TO_ROOM );
	    if ( equip_char( ch, obj, WEAR_WIELD ) )
		oprog_wear_trigger( ch, obj );
	    return;
	}
    }

    if ( fReplace )
	send_to_char( "Nie mo`zesz tego za`lo`zy`c lub u`zywa`c jako broni/`swiat`la.\n\r", ch );

    return;
}


/*
 * Lam: powinna byc mozliwosc zrobienia ubierz wsz.pierscien, a nie tylko wsz
 */
void wear_hold( CHAR_DATA *ch, char *argument, bool hold )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *room = ch->in_room;

    if ( !IS_NPC( ch ) && IS_AFFECTED( ch, AFF_GHOUL ) )
    {
	STC( "Twoja niecielesno`s`c uniemo`zliwia ci zak`ladanie rzeczy na siebie.\n\r", ch );
	return;
    }

    if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_TRZYMANE ) )
    {
	STC( "Nie masz jak tego chwyci`c.\n\r", ch );
	return;
    }

    one_argument( argument, arg );
 
    if ( arg[ 0 ] == '\0' )
    {
	sprintf( arg, "Co chcesz %s?\n\r", hold ? "trzyma`c" : "za`lo`zy`c" );
	STC( arg, ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( czy_wszystko( arg ) )
    {
	OBJ_DATA *obj_next;

	if ( hold )
	{
	    STC( "Nie mo`zesz trzyma`c wszystkiego naraz.\n\r", ch );
	    return;
	}

	/* Ulryk: w trakcie walki zmiana ekwipunku jest ograniczona do tego, co
	   sie trzyma w rekach, a i tak wymaga szczegolnego skupienia, wiec
	   hurtem sie nie da */
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->wear_loc != WEAR_NONE || !can_see_obj( ch, obj ) )
		continue;

	    if ( CAN_WEAR( obj, ITEM_WIELD )
		&& !IS_SET( race_table[ ch->race ].race_abilities,
			   RACE_WEAPON_WIELD ) )
		continue;

	    if ( obj->level > ch->level ) /* Lam */
		continue;

	    wear_obj( ch, obj, FALSE, hold );
	    if ( ch->deleted || room != ch->in_room ) /* remove_prog albo wear_prog */
		return;
	}
	return;
    }
    else
    {
	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
	    send_to_char( "Nie masz tego przy sobie.\n\r", ch );
	    return;
	}

	if ( CAN_WEAR( obj, ITEM_WIELD )
	    && !IS_SET( race_table[ ch->race ].race_abilities,
		       RACE_WEAPON_WIELD ) )
	{
	    send_to_char( "Nie potrafisz utrzyma`c broni.\n\r", ch );
	    return;
	}

	wear_obj( ch, obj, TRUE, hold );
    }

    return;
}


KOMENDA( do_wear )
{
    wear_hold( ch, argument, FALSE );

    return;
}


KOMENDA( do_hold )
{
    wear_hold( ch, argument, TRUE );

    return;
}


/*
 * Lam 2.3.98: "zdejmij wsz"
 */
KOMENDA( do_remove )
{
    OBJ_DATA *obj, *obj_next;
    char      arg[ MIL ];
    char      arg2[ MIL ];
    ROOM_INDEX_DATA *room = ch->in_room;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co zdj`a`c?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( czy_wszystko( arg ) )
    {
	if ( ch->fighting )
	{
	    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	    return;
	}

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->wear_loc != WEAR_NONE )
		remove_obj( ch, obj->wear_loc, TRUE );

	    if ( ch->deleted || room != ch->in_room ) /* remove_prog */
		return;
	}
    }
    else
    {
	int count = 0, number, i;
	bool cos;

	number = number_argument( arg, arg2 );
	obj = NULL;
	cos = !str_prefix( arg2, "co`s" );

	for ( i = 0; !obj && where_order[ i ] != WEAR_NONE; i++ )
	    for ( obj = ch->carrying; obj; obj = obj->next_content )
		if ( obj->wear_loc == where_order[ i ] )
		{
		    if ( !can_see_obj( ch, obj ) )
		    {
			if ( !cos )
			    continue;
		    }
		    else if ( !is_name( arg2, obj->name ) )
			continue;

		    if ( ++count == number )
			break;
		}

	if ( !obj )
	{
	    send_to_char( "Nie masz tego przy sobie.\n\r", ch );
	    return;
	}

	switch ( obj->wear_loc )
	{
	    case WEAR_LIGHT:
	    case WEAR_SHIELD:
	    case WEAR_WIELD:
	    case WEAR_HOLD:
	    case WEAR_WIELD_2:
		remove_obj( ch, obj->wear_loc, TRUE );
		break;
	    default:
		if ( ch->fighting )
		    ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
		else
		    remove_obj( ch, obj->wear_loc, TRUE );
		break;
	}
    }

    return;
}


KOMENDA( do_sacrifice )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || !str_cmp( arg, ch->name ) )
    {
	if ( IS_IMMORTAL( ch ) )
	{
	    ASTC( "Komu chcesz si`e po`swi`eci`c? Przecie`z sam$a jeste`s nie`smierteln$y.", ch );
	    act( ZM_WZROK | ZM_WID_CHAR,
		"$n spokojnie po`swi`eca si`e bogom, gdy nagle zauwa`za, `ze jest nie`smierteln$y.",
		ch, NULL, NULL, TO_ROOM );
	}
	else
	{
	    send_to_char(
		"Bogowie przyjmuj`a twoj`a ofert`e. Mog`a z niej skorzysta`c p`o`xniej.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR,
		"$n po`swi`eca si`e bogom, kt`orzy wielkodusznie odmawiaj`a.",
		ch, NULL, NULL, TO_ROOM );
	}
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( !obj )
    {
	send_to_char( "Nie mo`zesz tego znale`x`c.\n\r", ch );
	return;
    }

    if ( !CAN_WEAR( obj, ITEM_TAKE ) || IS_OBJ_STAT( obj, ITEM_POM ) )
    {
	if ( obj->rodzaj != 3 )
	   act( ZM_ZAWSZE, "$p nie nadaje si`e na ofiar`e.", ch, obj, NULL, TO_CHAR );
	else
	   act( ZM_ZAWSZE, "$p nie nadaj`a si`e na ofiar`e.", ch, obj, NULL, TO_CHAR );
	return;
    }

    send_to_char( "Bogowie daj`a ci troch`e z`lotych monet za t`e ofiar`e.\n\r", ch );
    ch->gold += ( obj->item_type == ITEM_MONEY ) ? 1 : UMAX( obj->level, 10 );

    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n po`swi`eca $j bogom.", ch, obj, NULL, TO_ROOM );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n wznosi r`ece ku g`orze.", ch, obj, NULL, TO_ROOM );
    extract_obj( obj );

    return;
}


/*
 * Lam 28.9.98
 */
KOMENDA( do_czytaj )
{
    OBJ_DATA  *ksiega;
    char       arg1[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *room = ch->in_room;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Niczego ci to nie da.\n\r", ch );
	return;
    }

    one_argument( argument, arg1 );

    if ( !arg1[ 0 ] )
    {
	send_to_char( "Sk`ladnia: czytaj <ksi`ega>\n\r", ch );
	return;
    }

    if ( !( ksiega = get_obj_carry( ch, arg1 ) ) )
    {
	send_to_char( "Nie masz takiej ksi`egi.\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( ksiega->item_type == ITEM_SCROLL )
    {
	send_to_char( "Patrzysz na pergamin i dochodzisz do wniosku, `ze jednak nale`zy go wyrecytowa`c.\n\r", ch );
	return;
    }

    if ( ksiega->item_type != ITEM_SPELLBOOK )
    {
	send_to_char( "Musisz przeczyta`c magiczn`a ksi`eg`e, nic innego.\n\r", ch );
	return;
    }

    if ( ksiega->value[ 0 ].v <= 0 || ksiega->value[ 0 ].v >= MAX_SKILL
      || ksiega->value[ 1 ].v <= 0 || ksiega->value[ 1 ].v > 100
      || ksiega->value[ 2 ].v > 110 )
    {
	send_to_char( "Z tej ksi`egi nie da si`e niczego wyczyta`c.\n\r", ch );
	return;
    }

    if ( ch->pcdata->learned[ ksiega->value[ 0 ].v ] >= ksiega->value[ 1 ].v )
    {
	send_to_char( "Ta ksi`ega niczego wi`ecej ci`e ju`z nie nauczy.\n\r", ch );
	return;
    }

    /* Muzgus, 19.03.2005: lancuchy antyrasowe i antyklasowe przy czytaniu */
    /* Lam 18.4.2007: rozbicie na dwa warunki, obsluga BWWU */
    if ( ( !IS_OBJ_STAT( ksiega, ITEM_INVERT_RACES )
	&& is_name2( race_table[ ch->race ].ang, ksiega->anti_race ) )
      || ( IS_OBJ_STAT( ksiega, ITEM_INVERT_RACES )
	&& !is_name2( race_table[ ch->race ].ang, ksiega->anti_race ) )
      || ( !IS_NPC( ch ) && !IS_OBJ_STAT( ksiega, ITEM_INVERT_CLASSES )
	&& is_name2( class_table[ ch->klasa ].who_name, ksiega->anti_class ) )
      || ( !IS_NPC( ch ) && IS_OBJ_STAT( ksiega, ITEM_PRIVATE )
	&& str_cmp( ch->name, ksiega->owner ) ) )
    {
	send_to_char( "Ta ksi`ega nie jest przeznaczona dla ciebie.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && IS_OBJ_STAT( ksiega, ITEM_INVERT_CLASSES )
      && !is_name2( class_table[ ch->klasa ].who_name, ksiega->anti_class ) )
    {
	int i, poziomy[ MAX_CLASS ];

	if ( ch->level >= L_APP )
	{
	    wypelnij_poziomy( ch, poziomy );

	    for ( i = 0; i < MAX_CLASS; i++ )
		if ( poziomy[ i ] >= 100
		  && is_name2( class_table[ i ].who_name, ksiega->anti_class ) )
		    break;

	    /* nie ukonczyl gry zadna profesja, ktora moze tego uzywac */
	    if ( i == MAX_CLASS )
	    {
		send_to_char( "Ta ksi`ega nie jest przeznaczona dla ciebie.\n\r", ch );
		return;
	    }
	}
	else
	{
	    send_to_char( "Ta ksi`ega nie jest przeznaczona dla ciebie.\n\r", ch );
	    return;
	}
    }

    if ( ch->level < ksiega->value[ 2 ].v )
    {
	send_to_char( "Ta ksi`ega jest dla ciebie zupe`lnie niezrozumia`la.\n\r", ch );
	return;
    }

    if ( !ch->pcdata->learned[ ksiega->value[ 0 ].v ] )
	ch->pcdata->learned_level[ ksiega->value[ 0 ].v ] = UMIN( ch->level, L_APP );
    ch->pcdata->learned[ ksiega->value[ 0 ].v ] = ksiega->value[ 1 ].v;
    ch->pcdata->used[ ksiega->value[ 0 ].v ] = 0;
    act( ZM_ZAWSZE, "Dzi`eki $h posiad`l$o`s znajomo`s`c umiej`etno`sci \"$T\".",
	ch, ksiega, skill_table[ ksiega->value[ 0 ].v ].pl_name, TO_CHAR );

    oprog_use_trigger( ch, ksiega, NULL );

    if ( ch->in_room && !ch->deleted && room == ch->in_room )
    {
	act( ZM_ZAWSZE, "Niszczysz $j, aby nikt inny nie posiad`l tej wiedzy.",
		ch, ksiega, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n czyta co`s, a nast`epnie to niszczy.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !ksiega->deleted )
	extract_obj( ksiega );

    return;
}


KOMENDA( do_recite )
{
    OBJ_DATA  *scroll;
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *room = ch->in_room;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( !arg1[ 0 ] )
    {
	send_to_char( "Sk`ladnia: recytuj <pergamin>\n\r", ch );
	return;
    }

    if ( !( scroll = get_obj_carry( ch, arg1 ) ) )
    {
	send_to_char( "Nie masz tego pergaminu.\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( scroll->item_type != ITEM_SCROLL )
    {
	send_to_char( "Mo`zesz recytowa`c wy`l`acznie pergaminy z zakl`eciami.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg2[ 0 ] == '\0' )
    {
	victim = ch;
	if ( ch->fighting )
	    victim = FIGHTING( ch );
    }
    else
    {
	if ( !( victim = get_char_room( ch, arg2 ) )
	    && !( obj  = get_obj_here( ch, arg2 ) ) )
	{
	    send_to_char( "Nie widzisz tego.\n\r", ch );
	    return;
	}
    }

    if ( IS_MUTE( ch )
	|| IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "Poruszasz wargami, ale nie s`lycha`c d`xwi`eku.\n\r", ch );
	return;
    }

    if ( ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	|| IS_AFFECTED( ch, AFF_CHARM ) )
    {
	act( ZM_ZAWSZE, "Usi`lujesz recytowa`c $j, ale masz zbyt s`lab`a wol`e.",
	    ch, scroll, NULL, TO_CHAR );
	/* po czym niby moze to poznac obserwator?
	act( "$n usi`luje recytowa`c $j, ale ma zbyt s`lab`a wol`e.",
	    ch, scroll, NULL, TO_ROOM ); */
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    act( ZM_ZAWSZE, "Recytujesz $j.", ch, scroll, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n recytuje $j.", ch, scroll, NULL, TO_ROOM );
    /* jesli widzi ch, nie widzi zwoju, ale niczego nie slyszy, to nie wie */
    act( ZM_SLUCH | ZM_W_WID_OBJ1, "$n co`s mamrocze.", ch, scroll, NULL, TO_ROOM );

    /* Scrolls skill by Binky for EnvyMud, modified by Thelonius */
    if ( !IS_NPC( ch )
	&& !( number_percent( ) <=
	    ch->pcdata->learned[ gsn_scrolls ] +
	    ( ch->pcdata->learned[ gsn_scrolls ] ?
	      UMIN( 10, ( 100 - ch->pcdata->learned[ gsn_scrolls ] ) / 2 )
	      : 0 ) ) )
    {
	switch ( number_bits( 3 ) )
	{
	case 0:
	case 1:
	case 2:
	case 3:
	    act( ZM_ZAWSZE, "Nie rozumiesz $g.", ch, scroll, NULL, TO_CHAR );
	    /* act( "$n nie potrafi zrozumie`c $g.", ch, scroll, NULL, TO_ROOM ); */
	    return;
	case 4:
	case 5:
	case 6:
	    send_to_char( "Co`s ci si`e chyba pomyli`lo.\n\r", ch );
	    /* act( "$2 chyba co`s si`e pomyli`lo.", ch, NULL, NULL, TO_ROOM ); */
	    act( ZM_ZAWSZE,
		scroll->rodzaj == 3 ? "$p spalaj`a si`e jasnym p`lomieniem."
				  : "$p spala si`e jasnym p`lomieniem.",
		ch, scroll, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1,
		scroll->rodzaj == 3 ? "$p spalaj`a si`e jasnym p`lomieniem."
				  : "$p spala si`e jasnym p`lomieniem.",
		ch, scroll, NULL, TO_ROOM );
	    extract_obj( scroll );
	    return;
	case 7:
	    act( ZM_ZAWSZE, scroll->rodzaj == 3 ?
		"Spapra`l$o`s recytacj`e, a $p spalaj`a si`e w`sr`od dymu!"
		: "Spapra`l$o`s recytacj`e, a $p spala si`e w`sr`od dymu!",
		ch, scroll, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, scroll->rodzaj == 3 ?
		"$p p`lon`a i znikaj`a w`sr`od dymu!"
		: "$p p`lonie i znika w`sr`od dymu!",
		ch, scroll, NULL, TO_ROOM );
	    /*
	     * damage( ) call after extract_obj in case the damage would
	     * have extracted ch.  This is okay because we merely mark
	     * obj->deleted; it still retains all values until list_update.
	     * Sloppy?  Okay, create another integer variable. ---Thelonius
	     */
	    extract_obj( scroll );
	    damage( ch, ch, scroll->level, gsn_scrolls, WEAR_NONE, TRUE );
	    return;
	}
    }

    if ( scroll->level > ch->level )
	act( ZM_ZAWSZE, scroll->rodzaj == 3 ? "$p maj`a jak dla ciebie za wysoki poziom."
		: "$p ma jak dla ciebie za wysoki poziom.", ch, scroll, NULL, TO_CHAR );
    else
    {
	int level = class_table[ ch->klasa ].fMana ? scroll->value[ 0 ].v
			: scroll->value[ 0 ].v / 2;
	int i;

	uzyj( ch, gsn_scrolls, "u`zywaniu pergamin`ow" );
	for ( i = 1; i <= 3; i++ )
	{
	    obj_cast_spell( scroll->value[ i ].v, level, ch, victim, obj, argument, USAGE_SCROLL );
	    if ( !ch->in_room || ch->deleted || room != ch->in_room
	      || scroll->deleted || scroll->carried_by != ch
	      || ( victim && ( victim->in_room != room || victim->deleted ) )
	      || ( obj && obj->deleted ) )
	    {
		if ( !scroll->deleted
		  && ( !IS_NPC( ch )
		    || IS_AFFECTED( ch, AFF_CHARM ) ) )
		    extract_obj( scroll );
		return;
	    }
	}
	oprog_use_trigger( ch, scroll, NULL );
    }

    if ( !scroll->deleted
      && ( !IS_NPC( ch )
	|| IS_AFFECTED( ch, AFF_CHARM ) ) )
	extract_obj( scroll );

    return;
}


KOMENDA( do_brandish )
{
    OBJ_DATA  *staff;
    CHAR_DATA *vch;
    int        sn;
    ROOM_INDEX_DATA *room = ch->in_room;

    if ( !( staff = get_eq_char( ch, WEAR_HOLD ) ) )
    {
	send_to_char( "Niczego nie masz w r`ece.\n\r", ch );
	return;
    }

    if ( staff->item_type != ITEM_STAFF )
    {
	send_to_char( "Masz w r`ece z`ly przedmiot.\n\r", ch );
	return;
    }

    if ( ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	|| IS_AFFECTED( ch, AFF_CHARM ) )
    {
	act( ZM_ZAWSZE, "Usi`lujesz wyzwoli`c moce $g, ale nie masz silnej woli.",
	    ch, staff, NULL, TO_CHAR );
	/* act( "$n chce wyzwoli`c moce $g, ale nie ma silnej woli.",
	    ch, staff, NULL, TO_ROOM ); */
	return;
    }

    if ( ( sn = staff->value[ 3 ].v ) < 0
	|| sn >= MAX_SKILL
	|| skill_table[ sn ].spell_fun == 0 )
    {
	bug( "Do_brandish: bad sn %d.", sn );
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( staff->value[ 2 ].v > 0 )
    {
	CHAR_DATA *vch_next;

	act( ZM_ZAWSZE, "Wyzwalasz moce $g.",  ch, staff, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wyzwala moce $g.", ch, staff, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n unosi r`ek`e.", ch, staff, NULL, TO_ROOM );

	/* Staves skill by Binky for EnvyMud, modified by Thelonius */
	if ( !IS_NPC( ch )
	  && !( number_percent( ) <=
	    ch->pcdata->learned[ gsn_staves ] +
	    ( ch->pcdata->learned[ gsn_staves ] ?
	      UMIN( 10, ( 100 - ch->pcdata->learned[ gsn_staves ] ) / 2 )
	      : 0 ) ) )
	{
	    switch ( number_bits( 3 ) )
	    {
	    case 0:
	    case 1:
	    case 2:
	    case 3:
		act( ZM_ZAWSZE, "Nie mo`zesz wyzwoli`c mocy $g.",
		    ch, staff, NULL, TO_CHAR );
		/* act( "$n nie mo`ze wyzwoli`c mocy $g.",
		    ch, staff, NULL, TO_ROOM ); */
		return;
	    case 4:
	    case 5:
	    case 6:
		act( ZM_ZAWSZE, "Przywo`lujesz moce $g, ale one si`e ulatniaj`a.",
		    ch, staff, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n przywo`luje moce $g, ale one si`e ulatniaj`a.",
		    ch, staff, NULL, TO_ROOM );
		if ( --staff->value[ 2 ].v <= 0 )
		{
		    if ( staff->rodzaj == 3 )
		    {
			act( ZM_WZROK | ZM_WID_OBJ1, "$p spalaj`a si`e i znikaj`a.",
				ch, staff, NULL, TO_CHAR );
			act( ZM_WZROK | ZM_WID_OBJ1, "$p spalaj`a si`e i znikaj`a.",
				ch, staff, NULL, TO_ROOM );
		    }
		    else
		    {
			act( ZM_WZROK | ZM_WID_OBJ1, "$p spala si`e i znika.",
				ch, staff, NULL, TO_CHAR );
			act( ZM_WZROK | ZM_WID_OBJ1, "$p spala si`e i znika.",
				ch, staff, NULL, TO_ROOM );
		    }
		    extract_obj( staff );
		}
		return;
	    case 7:
		act( ZM_ZAWSZE, "Nie panujesz nad moc`a $g, a to rozpada si`e na kawa`lki!",
		    ch, staff, NULL, TO_CHAR );
		if ( staff->rodzaj == 3 )
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpadaj`a si`e na kawa`lki!",
			ch, staff, NULL, TO_ROOM );
		else
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpada si`e na kawa`lki!",
			ch, staff, NULL, TO_ROOM );
		/*
		 * damage( ) call after extract_obj in case the damage would
		 * have extracted ch.  This is okay because we merely mark
		 * obj->deleted; it still retains all values until list_update.
		 * Sloppy?  Okay, create another integer variable. ---Thelonius
		 */
		extract_obj( staff );
		damage( ch, ch, staff->level, gsn_staves, WEAR_NONE, TRUE );
		return;
	    }
	}

	uzyj( ch, gsn_staves, "u`zywaniu bu`law" );
	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next = vch->next_in_room;

	    if ( vch->in_room != room )
		break;

	    if ( vch->deleted
	      || ( !IS_NPC( vch )
		&& IS_SET( vch->act, PLR_WIZINVIS )
		&& vch->pcdata->wizinvis <= get_trust( ch ) ) )
		continue;

	    switch ( skill_table[ sn ].target )
	    {
		default:
		    bug( "Do_brandish: bad target for sn %d.", sn );
		    return;

		case TAR_IGNORE:
		    if ( vch != ch )
			continue;
		    break;

		case TAR_CHAR_OFFENSIVE:
		    if ( IS_NPC( ch ) ? IS_NPC( vch ) : !IS_NPC( vch ) )
			continue;
		    break;

		case TAR_CHAR_DEFENSIVE:
		    if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
			continue;
		    break;

		case TAR_CHAR_SELF:
		    if ( vch != ch )
			continue;
		    break;
	    }

	    obj_cast_spell( staff->value[ 3 ].v,
		class_table[ ch->klasa ].fMana ? staff->value[ 0 ].v : staff->value[ 0 ].v / 2,
		ch, vch, NULL, "", USAGE_STAFF );
	    if ( !ch->in_room || ch->deleted || room != ch->in_room || staff->deleted )
		break;
	}

	if ( !ch->deleted && room == ch->in_room && !staff->deleted )
	    oprog_use_trigger( ch, staff, NULL );
    }

    if ( !staff->deleted
      && ( !IS_NPC( ch )
	|| IS_AFFECTED( ch, AFF_CHARM ) ) )
	if ( --staff->value[ 2 ].v <= 0 )
	{
	    if ( ch->in_room && !ch->deleted && room == ch->in_room )
	    {
		if ( staff->rodzaj == 3 )
		{
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p spalaj`a si`e i znikaj`a.",
			ch, staff, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p spalaj`a si`e i znikaj`a.",
			ch, staff, NULL, TO_ROOM );
		}
		else
		{
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p spala si`e i znika.",
			ch, staff, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p spala si`e i znika.",
			ch, staff, NULL, TO_ROOM );
		}
	    }
	    extract_obj( staff );
	}

    return;
}


KOMENDA( do_zap )
{
    OBJ_DATA  *wand;
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];
    int        sn;
    ROOM_INDEX_DATA *room = ch->in_room;

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' && !ch->fighting )
    {
	send_to_char( "Co lub kogo zaczarowa`c?\n\r", ch );
	return;
    }

    if ( !( wand = get_eq_char( ch, WEAR_HOLD ) ) )
    {
	send_to_char( "Niczego nie masz w d`loni.\n\r", ch );
	return;
    }

    if ( wand->item_type != ITEM_WAND )
    {
	send_to_char( "Mo`zesz to robi`c tylko r`o`zd`zkami.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg[ 0 ] == '\0' )
    {
	if ( ch->fighting )
	    victim = FIGHTING( ch );
	else
	{
	    send_to_char( "Co lub kogo zaczarowa`c?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( !( victim = get_char_room ( ch, arg ) )
	    && !( obj  = get_obj_here  ( ch, arg ) ) )
	{
	    send_to_char( "Nie mo`zesz tego znale`x`c.\n\r", ch );
	    return;
	}
    }

    if ( ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	|| IS_AFFECTED( ch, AFF_CHARM ) )
    {
	act( ZM_ZAWSZE, "Usi`lujesz czarowa`c $k, ale nie masz silnej woli.",
	    ch, wand, NULL, TO_CHAR );
	/* act( "$n pr`obuje czarowa`c $k, ale nie ma silnej woli.",
	    ch, wand, NULL, TO_ROOM ); */
	return;
    }

    if ( ( sn = wand->value[ 3 ].v ) < 0
	|| sn >= MAX_SKILL
	|| skill_table[ sn ].spell_fun == 0 )
    {
	bug( "Do_zap: bad sn %d.", sn );
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( wand->value[ 2 ].v > 0 )
    {
	if ( victim )
	{
	    if ( victim == ch )
	    {
		act( ZM_ZAWSZE, "Zaczarowujesz si`e za pomoc`a $g.", ch, wand, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zaczarowuje si`e za pomoc`a $g.", ch, wand, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ek`a.", ch, wand, NULL, TO_ROOM );
	    }
	    else
	    {
		act( ZM_ZAWSZE, "Zaczarowujesz $# za pomoc`a $g.", ch, wand, victim, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zaczarowuje ci`e za pomoc`a $g.", ch, wand, victim, TO_VICT );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT | ZM_WID_OBJ1, "$n zaczarowuje $# za pomoc`a $g.", ch, wand, victim, TO_NOTVICT );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT | ZM_WID_OBJ1, "$n wymachuje $k.", ch, wand, victim, TO_NOTVICT );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ek`a.", ch, wand, victim, TO_ROOM );
	    }
	}
	else
	{
	    act( ZM_ZAWSZE, "Zaczarowujesz $J za pomoc`a $g.", ch, wand, obj, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1 | ZM_WID_OBJ2, "$n zaczarowuje $J za pomoc`a $g.", ch, wand, obj, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1 | ZM_W_WID_OBJ2, "$n macha $k.", ch, wand, obj, TO_ROOM );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ek`a.", ch, wand, obj, TO_ROOM );
	}

	/* Wands skill by Binky for EnvyMud, modified by Thelonius */
	if ( !IS_NPC( ch )
	  && !( number_percent( ) <=
	    ch->pcdata->learned[ gsn_wands ] +
	    ( ch->pcdata->learned[ gsn_wands ] ?
	      UMIN( 10, ( 100 - ch->pcdata->learned[ gsn_wands ] ) / 2 )
	      : 0 ) ) )
	{
	    switch ( number_bits( 3 ) )
	    {
	    case 0:
	    case 1:
	    case 2:
	    case 3:
		act( ZM_ZAWSZE, "Nie potrafisz wyzwoli`c mocy $g.",
		    ch, wand, NULL, TO_CHAR );
		/* act( "$n nie potrafi wyzwoli`c mocy $g.",
		    ch, wand, NULL, TO_ROOM ); */
		return;
	    case 4:
	    case 5:
	    case 6:
		act( ZM_ZAWSZE, "Przywo`lujesz moce $g, ale one si`e ulatniaj`a.",
		    ch, wand, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n przywo`luje moce $g, ale one si`e ulatniaj`a.",
		    ch, wand, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_OBJ1, "$n macha r`ek`a.",
		    ch, wand, NULL, TO_ROOM );
		if ( --wand->value[ 2 ].v <= 0 )
		{
		    if ( wand->rodzaj == 3 )
		    {
			act( ZM_ZAWSZE, "$p spalaj`a si`e jasnym p`lomieniem.",
				ch, wand, NULL, TO_CHAR );
			act( ZM_WZROK | ZM_WID_OBJ1, "$p spalaj`a si`e jasnym p`lomieniem.",
				ch, wand, NULL, TO_ROOM );
		    }
		    else
		    {
			act( ZM_ZAWSZE, "$p spala si`e jasnym p`lomieniem.",
				ch, wand, NULL, TO_CHAR );
			act( ZM_WZROK | ZM_WID_OBJ1, "$p spala si`e jasnym p`lomieniem.",
				ch, wand, NULL, TO_ROOM );
		    }
		    extract_obj( wand );
		}
		return;
	    case 7:
		act( ZM_ZAWSZE, "Nie potrafisz zapanowa`c nad moc`a $g.",
		    ch, wand, NULL, TO_CHAR );
		if ( wand->rodzaj == 3 )
		{
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpadaj`a si`e na kawa`lki!",
			ch, wand, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpadaj`a si`e na kawa`lki!",
			ch, wand, NULL, TO_ROOM );
		}
		else
		{
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpada si`e na kawa`lki!",
			ch, wand, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpada si`e na kawa`lki!",
			ch, wand, NULL, TO_ROOM );
		}
		/*
		 * damage( ) call after extract_obj in case the damage would
		 * have extracted ch.  This is okay because we merely mark
		 * obj->deleted; it still retains all values until list_update.
		 * Sloppy?  Okay, create another integer variable. ---Thelonius
		 */
		extract_obj( wand );
		damage( ch, ch, wand->level, gsn_wands, WEAR_NONE, TRUE );
		return;
	    }
	}

	uzyj( ch, gsn_wands, "u`zywaniu r`o`zd`zek" );
	/* wand->value[ 0 ] is not used for wands */
	obj_cast_spell( wand->value[ 3 ].v,
	    class_table[ ch->klasa ].fMana ? wand->value[ 0 ].v : wand->value[ 0 ].v / 2,
	    ch, victim, obj, argument, USAGE_WAND );
	if ( !ch->deleted && room == ch->in_room && !wand->deleted )
	   oprog_use_trigger( ch, wand, NULL );
    }

    if ( !wand->deleted
      && ( !IS_NPC( ch )
	|| IS_AFFECTED( ch, AFF_CHARM ) ) )
	if ( --wand->value[ 2 ].v <= 0 )
	{
	    if ( ch->in_room && !ch->deleted && room == ch->in_room )
	    {
		if ( wand->rodzaj == 3 )
		{
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpadaj`a si`e na kawa`lki!",
			ch, wand, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpadaj`a si`e na kawa`lki!",
			ch, wand, NULL, TO_ROOM );
		}
		else
		{
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpada si`e na kawa`lki!",
			ch, wand, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpada si`e na kawa`lki!",
			ch, wand, NULL, TO_ROOM );
		}
	    }
	    extract_obj( wand );
	}

    return;
}


KOMENDA( do_uzyj )
{
    OBJ_DATA *obj;

    if ( !( obj = get_eq_char( ch, WEAR_HOLD ) ) )
    {
	send_to_char( "Nie masz niczego w r`ekach.\n\r", ch );
	return;
    }

    switch ( obj->item_type )
    {
	default:
	    send_to_char( "Nie wiesz jak masz tego u`zy`c.\n\r", ch );
	    break;
	case ITEM_WAND:
	    do_zap( ch, argument );
	    break;
	case ITEM_STAFF:
	    do_brandish( ch, argument );
	    break;
	case ITEM_SCROLL:
	    send_to_char( "Pergaminy wypada raczej recytowa`c.\n\r", ch );
	    break;
	case ITEM_DEVICE:
	    if ( IS_SET( obj->pIndexData->progtypes, O_USE_PROG ) )
		oprog_use_trigger( ch, obj, argument );
	    else
		send_to_char( "To urz`adzenie jest niesprawne.\n\r", ch );
	    break;
    }

    return;
}


KOMENDA( do_steal )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       buf[ MAX_STRING_LENGTH ];
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;
    int        count;
    int        percent;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Moby nie kradn`a bez spec`ow.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
    {
	send_to_char( "Ukra`s`c co komu?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	ASTC( "To bez sensu... Sam$a siebie nie okradniesz.", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    obj = NULL;

    WAIT_STATE( ch, skill_table[ gsn_steal ].beats );

    /* Modified chances for stealing by Morpheus */
    percent = ch->level; /* Base value */

    percent += number_range( -10, 10 ); /* Luck */

    percent -= victim->level; /* Character level vs victim's */

    if ( !IS_AWAKE( victim ) )
	percent += 25; /* Sleeping characters are easier */

    percent += ch->pcdata->learned[ gsn_steal ]; /* Character ability */

    if ( IS_AFFECTED( ch, AFF_SNEAK ) )
	percent += 10; /* Quiet characters steal better */

    if ( !can_see( victim, ch ) ) /* Lam: tu bylo ch, victim :) */
	percent += 10; /* Unseen characters steal better */

    if ( !str_prefix( arg1, "monety" ) || !str_cmp( arg1, "z`loto" ) )
	percent = (int) ( percent * 1.2 );
    else
    {
	number = number_argument( arg1, arg );
	count  = 0;

	for ( obj = victim->carrying; obj; obj = obj->next_content )
	    if ( obj->wear_loc == WEAR_NONE
	      && can_see_obj( ch, obj )
	      && is_name( arg, obj->name ) )
	    {
		if ( ++count == number )
		    break;
	    }

	if ( !obj )
	{
	    send_to_char( "Nie mo`zesz tego znale`x`c.\n\r", ch );
	    return;
	}

	if ( obj->wear_loc == WEAR_NONE )
	    percent = (int) ( percent * .8 );
    }

    if ( ( !IS_NPC( victim )
	&& ( ( !IS_SET( victim->act, PLR_KILLER )
	    && !IS_SET( victim->act, PLR_THIEF )
	    && !CZY_WAMPIR( victim ) )
          /* wyzszych poziomem GRACZY nie powinno sie dac okradac */
	  && ch->level - victim->level < 0 ) )
      || percent < number_percent( ) )
    {
	/*
	 * Failure.
	 *
	 * Qwert: tylko nieudane proby dopisuja do listy
	 *
	 * Lam: jesli byl pojedynek i nie uzywal "morduj", niech ofiara
	 * nie dostaje bezkarnego usmiercenia
	 *
	 * Lam 10.1.2008: jesli ofiara spi, ma szanse sie obudzic, ale jesli
	 * sie nie obudzi albo jest nieprzytomna, nie ma jak sie zorientowac
	 */
	if ( victim->position == POS_SLEEPING
	  && number_bits( 1 ) )
	{
	    victim->position = POS_STANDING;
	    STC( "Co`s przerywa tw`oj sen.\n\r", victim );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n nagle si`e budzi.", victim, NULL, NULL, TO_ROOM );
	    komunikaty_ustroju( victim );
	}

	if ( IS_AWAKE( victim ) )
	{
	    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA ) )
		dodaj_zabojce( victim, ch ); /* nienawisc */
	    send_to_char( "No nie!\n\r", ch );
	    act( ZM_ZAWSZE, "$n usi`lowa`l$a ci`e okra`s`c!", ch, NULL, victim, TO_VICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n usi`lowa`l$a okra`s`c $#.", ch, NULL, victim, TO_NOTVICT );
	    if ( can_see( victim, ch ) && ch->sex == 2 )
		sprintf( buf, "%s jest z`lodziejk`a!", PERS( ch, victim, FALSE ) );
	    else
		sprintf( buf, "%s jest z`lodziejem!", PERS( ch, victim, FALSE ) );
	    do_yell( victim, buf );

	    /* tu analogicznie - pojedynek nie moze dac flagi */
	    if ( IS_NPC( victim )
	      || IS_NPC( ch ) /* wiem, ze to zbedne, ale moze kiedy progi? */
	      || !IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA ) )
	    {
		if ( !is_fighting( ch, victim ) ) /* teoretycznie zbedne */
		    set_fighting( ch, victim );
		/* ZDOLNY_DO_WALKI zbedne, IS_AWAKE jest ponizej */
		if ( !is_fighting( victim, ch ) )
		    set_fighting( victim, ch );
		multi_hit( victim, ch, TYPE_UNDEFINED );
	    }
	    else
	    {
/*
		if ( !licensed( ch ) )
		{
		    send_to_char(
			 "Nie masz licencji! Tracisz 400 punkt`ow do`swiadczenia.\n\r", ch );
		    gain_exp( ch, -400 );
		    if ( registered( ch, victim )
			&& ch->level - victim->level < 6 )
		    {
			return;
		    }
		}
*/

		if ( ch->level - victim->level > 5
	       /* || !registered( ch, victim ) */ )
		{
		    log_string( buf );
		    if ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_THIEF ) )
		    {
			char kto[ MIL ];

			sprintf( kto, "%s %s", victim->name, ascdata( localtime( &current_time ) ) );
			SET_BIT( ch->act, PLR_THIEF );
			free_string( ch->pcdata->thief_from );
			ch->pcdata->thief_from = str_dup( kto );
			send_to_char( "{R*** Jeste`s Z`LODZIEJEM! ***{x\n\r", ch );
			save_char_obj( ch );
		    }
		}
	    }
	}
	else
	    STC( "Nie uda`lo ci si`e niczego ukra`s`c, ale przynajmniej nikt ci`e nie nakry`l.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg1, "monety" ) || !str_cmp( arg1, "z`loto" ) )
    {
	int amount;
	int gold = UMIN( victim->gold, ZYSK( ch->level, 10 ) * 5 );

	amount = gold * number_range( 5, 15 ) / 100;
	if ( ( percent - 50 ) > number_percent( ) )
	    amount += gold * number_range( 5, 10 ) / 100;
	if ( amount <= 0 )
	{
	    ASTC( "Nie dobra`l$o`s si`e do z`lota.", ch );
	    return;
	}

	ch->gold     += amount;
	victim->gold -= amount;
	sprintf( buf, "Bosko! Masz %s.\n\r", ZLOTO_B( amount ) );
	send_to_char( buf, ch );
	if ( ch->pcdata->learned[ gsn_steal ] )
	    uzyj( ch, gsn_steal, "okradaniu postaci" );
	return;
    }

    if ( !can_drop_obj( victim, obj )
      || IS_OBJ_STAT( obj, ITEM_INVENTORY )
      || IS_OBJ_STAT( obj, ITEM_LOYAL ) /* Lam */
      || IS_OBJ_STAT( obj, ITEM_QUEST ) /* Ulryk */
      || obj->level > ch->level )
    {
	act( ZM_ZAWSZE, "Nie mo`zesz $U tego wyrwa`c.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	send_to_char( "Masz pe`lne r`ece.\n\r", ch );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	send_to_char( "Nie uniesiesz tego.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	ASTC( "Troch`e si`e pom`eczy`l$o`s, no i jest!", ch );
	unequip_char( victim, obj );
    }
    /* Qwert: teraz zmieni poziom przy kradziezy (anty heros) */
    obj_from_char( obj );
    zmien_poziom( obj, ch );
    obj_to_char( obj, ch );
    send_to_char( "Dokonujesz kradzie`zy!\n\r", ch );

    if ( ch->pcdata->learned[ gsn_steal ] )
	uzyj( ch, gsn_steal, "okradaniu postaci" );

    return;
}


/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *keeper, *vch;
    SHOP_DATA *pShop;
    char buf[ MSL ];
    char arg[ MIL ];
    int number, count;

    pShop = NULL;
    keeper = NULL;
    count = 0;
    number = number_argument( argument, arg );
    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( ch != vch
	  && can_see( ch, vch )
	  && ( ( IS_NPC( vch ) && vch->pIndexData->pShop )
	    || ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_SHOPKEEPER ) ) )
	  && IS_AWAKE( vch )
	  && !IS_AFFECTED( vch, AFF_CHARM )
	  && ( arg[ 0 ] == '\0' || is_name( arg, vch->name ) ) )
	{
	    if ( ++count == number
	      || ( arg[ 0 ] == '\0'
		&& vch->in_room == vch->reset_room ) )
	    {
		keeper = vch;
		if ( IS_NPC( vch ) )
		    pShop = vch->pIndexData->pShop;
		else
		    pShop = NULL;
		if ( arg[ 0 ] != '\0' || vch->in_room == vch->reset_room )
		    break;
	    }
	}
    }

    if ( !keeper )
    {
	if ( *argument )
	    STC( "Nie ma tu sprzedawcy pasuj`acego do opisu.\n\r", ch );
	else
	    STC( "Nie ma tu nikogo ch`etnego do prowadzenia z tob`a handlu.\n\r", ch );
	return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze ci`e zobaczy`c.", keeper, NULL, ch, TO_VICT );
	return NULL;
    }

    /*
     * Undesirables.
     */
    if ( IS_NPC( keeper ) && !IS_NPC( ch ) && IS_SET( ch->act, PLR_KILLER ) )
    {
	do_say( keeper, "Nie lubimy tu morderc`ow!" );
	sprintf( buf, "%s, MORDERCA, jest u mnie w sklepie!", ch->name );
	do_yell( keeper, buf );
	return NULL;
    }

    if ( IS_NPC( keeper ) && !IS_NPC( ch ) && IS_SET( ch->act, PLR_THIEF ) )
    {
	do_say( keeper, "Nie lubimy tu z`lodziei!" );
	sprintf( buf, "%s, Z`LODZIEJ, jest u mnie w sklepie!", ch->name );
	do_yell( keeper, buf );
	return NULL;
    }

    /*
     * Shop hours.
     */
    if ( pShop && ch->in_room->area->time_info.hour < pShop->open_hour )
    {
	act( ZM_ZAWSZE, "$n jeszcze nie handluje, przyjd`x p`o`xniej.", keeper, NULL, ch, TO_VICT );
	return NULL;
    }

    if ( pShop && ch->in_room->area->time_info.hour > pShop->close_hour )
    {
	act( ZM_ZAWSZE, "$n ju`z nie handluje, przyjd`x jutro.", keeper, NULL, ch, TO_VICT );
	return NULL;
    }

    if ( !IS_NPC( keeper ) && IS_SET( keeper->act, PLR_SHOP_BREAK ) )
    {
	act( ZM_ZAWSZE, "$n ma teraz przerw`e, spr`obuj p`o`xniej.", keeper, NULL, ch, TO_VICT );
	return NULL;
    }

    return keeper;
}


CHAR_DATA *find_healer( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *healer;
    HEALER_DATA *pHealer;
    char       buf[ MAX_STRING_LENGTH ];

    pHealer = NULL;
    for ( healer = ch->in_room->people; healer; healer = healer->next_in_room )
    {
	if ( IS_NPC( healer ) && ( pHealer = healer->pIndexData->pHealer )
	  && IS_AWAKE( healer )
	  && !IS_AFFECTED( healer, AFF_CHARM )
	  && ( argument[ 0 ] == '\0'
	    || ( is_name( argument, healer->name )
	      && can_see( ch, healer ) ) ) )
	    break;
    }

    if ( !healer || !pHealer )
	/* bez mowienia, ze nie mozna, to zrobi find_keeper */
	return NULL;

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( healer, ch ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze ci`e zobaczy`c.", healer, NULL, ch, TO_VICT );
	return NULL;
    }

    /*
     * Undesirables.
     */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_KILLER ) )
    {
	do_say( healer, "Nie lubimy tu morderc`ow!" );
	sprintf( buf, "%s MORDERCA jest tu i chce czar`ow!", ch->name );
	do_yell( healer, buf );
	return NULL;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_THIEF ) )
    {
	do_say( healer, "Nie lubimy tu z`lodziei!" );
	sprintf( buf, "%s Z`LODZIEJ jest tu i chce czar`ow!", ch->name );
	do_yell( healer, buf );
	return NULL;
    }

    return healer;
}


/* Koles (sprzedawca drog do krain) */
CHAR_DATA *find_koles( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *keeper;
    char       buf[ MAX_STRING_LENGTH ];

    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
    {
	if ( can_see( ch, keeper )
	  && IS_NPC( keeper ) && znajdz_kolesia( keeper->pIndexData->vnum )
	  && IS_AWAKE( keeper )
	  && !IS_AFFECTED( keeper, AFF_CHARM )
	  && ( argument[ 0 ] == '\0' || is_name( argument, keeper->name ) ) )
	    break;
    }

    if ( !keeper || !znajdz_kolesia( keeper->pIndexData->vnum ) )
    {
/*      send_to_char( "Nie ma tu nikogo kto zna`lby drog`e do jakiejkolwiek krainy.\n\r", ch ); */
	return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze ci`e zobaczy`c.", keeper, NULL, ch, TO_VICT );
	return NULL;
    }

    /*
     * Undesirables.
     */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_KILLER ) )
    {
	do_say( keeper, "Nie lubimy tu morderc`ow!" );
	sprintf( buf, "%s MORDERCA jest tu i chce drogi do innych krain!", ch->name );
	do_yell( keeper, buf );
	return NULL;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_THIEF ) )
    {
	do_say( keeper, "Nie lubimy tu z`lodziei!" );
	sprintf( buf, "%s Z`LODZIEJ jest tu i chce drogi do innych krain!", ch->name );
	do_yell( keeper, buf );
	return NULL;
    }

    return keeper;
}


int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int        cost;
    bool       first;

    if ( !obj )
	return 0;

    if ( !IS_NPC( keeper ) )
	return obj->plr_shop_cost;

    if ( !( pShop = keeper->pIndexData->pShop ) )
	return 0;

    if ( !obj->cost )
	return 0;

    if ( fBuy )
	cost = obj->cost * pShop->profit_buy / 100;
    else
    {
	OBJ_DATA *obj2;
	int       itype;

	cost = 0;
	for ( itype = 0; itype < MAX_TRADE; itype++ )
	    if ( obj->item_type == pShop->buy_type[ itype ] )
	    {
		cost = obj->cost * pShop->profit_sell / 100;
		break;
	    }

	/* jesli nie skupuje takich przedmiotow */
	if ( itype == MAX_TRADE )
	    return 0;

	first = TRUE;
	for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
	    if ( obj->pIndexData == obj2->pIndexData )
	    {
		cost /= 2;
		first = FALSE;
	    }

	/* Lam 12.11.2005: niech placa chociaz 100 monet za poszerzenie
	   asortymentu, niech bedzie warto sprzedawac im niskopoziomowe bzdety,
	   bo najczesciej chca takie kupowac gracze z najnizszych poziomow */
	if ( first )
	    cost = UMAX( cost, 100 );
    }

    if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
	cost = obj->value[ 1 ].v ? ( cost * obj->value[ 2 ].v / obj->value[ 1 ].v ) : 0;

    return cost;
}


/* Lam 18.6.2005, modyfikacja get_obj_carry */
OBJ_DATA *get_obj_shop( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    number = number_argument( argument, arg );
    count  = 0;

    /* najpierw stala ekspozycja lub sklep gracza */
    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( obj->wear_loc == WEAR_NONE
	  && ( ( IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	    || ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_PLAYER_SHOP ) ) )
	  && can_see_obj( ch, obj )
	  && is_name( arg, obj->name )
	  && get_cost( ch, obj, TRUE ) > 0 )
	{
	    if ( ++count == number )
		return obj;
	}

    /* dla gracza to koniec poszukiwan */
    if ( !IS_NPC( ch ) )
	return NULL;

    /* potem nabytki */
    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( obj->wear_loc == WEAR_NONE
	  && !IS_OBJ_STAT( obj, ITEM_INVENTORY )
	  && can_see_obj( ch, obj )
	  && is_name( arg, obj->name )
	  && get_cost( ch, obj, TRUE ) > 0 )
	{
	    if ( ++count == number )
		return obj;
	}

    return NULL;
}


void potencjalnie_zamknij_sklep( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( IS_OBJ_STAT( obj, ITEM_PLAYER_SHOP )
	  && obj->wear_loc == WEAR_NONE )
	    break;

    if ( !obj )
	do_sklep( ch, "zamknij" );

    return;
}


/*
 * Multiple object buy modifications by Erwin Andreasen
 * Obtained from Canth's snippets page at:
 * http://www.xs4all.nl/~phule/snippets/snippets.html
 * Lam: zakup czarow i drog do krain
 * Lam: targowanie
 */
KOMENDA( do_buy )
{
    char arg[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co kupi`c?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
	CHAR_DATA       *pet;
	ROOM_INDEX_DATA *pRoomIndexNext;
	ROOM_INDEX_DATA *in_room;
	char             buf [ MAX_STRING_LENGTH ];

	if ( IS_NPC( ch ) )
	{
	    STC( "Nie mo`zesz sobie kupi`c zwierz`ecia.\n\r", ch );
	    return;
	}

	if ( !( pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 ) ) )
	{
	    bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "Nie mo`zna tu tego kupi`c.\n\r", ch );
	    return;
	}

	in_room     = ch->in_room;
	ch->in_room = pRoomIndexNext;
	pet         = get_char_room( ch, arg );
	ch->in_room = in_room;

	if ( !pet || !IS_SET( pet->act, ACT_PET ) )
	{
	    send_to_char( "Tu nie mo`zna tego kupi`c.\n\r", ch );
	    return;
	}

	if ( IS_SET( ch->act, PLR_BOUGHT_PET ) )
	{
	    ASTC( "Ju`z kupi`l$o`s zwierz`atko na tym poziomie.", ch );
	    return;
	}

	if ( ch->gold < WYDATEK( pet->level, 10 ) )
	{
	    send_to_char( "Nie sta`c ci`e na to.\n\r", ch );
	    return;
	}

	if ( ch->level < pet->level )
	{
	    ASTC( "Jeszcze nie doros`l$o`s do posiadania tego zwierz`atka.", ch );
	    return;
	}

	ch->gold -= WYDATEK( pet->level, 10 );
	pet       = create_mobile( pet->pIndexData );
	SET_BIT( ch->act, PLR_BOUGHT_PET );
	SET_BIT( pet->act, ACT_PET );
	SET_BIT_V( pet->affected_by, AFF_CHARM );

	/* Tissarius 07.10.2001: usuwamy, drugi argument to imie zwierzaczka */
	/* one_argument( argument, arg ); */

	if ( arg2[ 0 ] != '\0' )
	{
	    sprintf( buf, "%s %s", pet->name, arg2 );
	    free_string( pet->name );
	    pet->name = str_dup( buf );
	}

	sprintf( buf, "%sKarteczka przy szyi g`losi: \"Nale`z`e do %s\".\n\r",
		pet->description, ch->dopelniacz );
	free_string( pet->description );
	pet->description = str_dup( buf );

	char_to_room( pet, ch->in_room );
	add_follower( pet, ch );
	send_to_char( "Ciesz si`e swoim nowym zwierzaczkiem.\n\r", ch );
	/* moze "ktos kupic sobie szczeniaczka", widac zakup i tak */
	act( ZM_WZROK | ZM_WID_VICT, "$n kupuje sobie $#.", ch, NULL, pet, TO_ROOM );
	return;
    }
    else
    {
	CHAR_DATA *koles;
	KOLES_DATA *kol;
	char       buf[ MAX_STRING_LENGTH ];
	int        ktora;
	OBJ_DATA  *obj;
	CHAR_DATA *keeper;
	int        cost;
	int        targ;
	int        haggle;
	float      pom;
	int        item_count = 1; /* buy only one by default */

	if ( ( koles = find_koles( ch, arg2 ) ) )
	{
	    kol = znajdz_kolesia( koles->pIndexData->vnum );
	    if ( !kol )
	    {
		bug( "nie znalazlem kolesia?", 0 );
		return;
	    }
	    cost = WYDATEK( ch->level, 15 );
	    if ( ch->gold < cost )
	    {
		send_to_char( "Nie wystarczy ci na to pieni`edzy.\n\r", ch );
		return;
	    }
	    if ( !is_number( arg ) )
	    {
		send_to_char( "Musisz poda`c numer krainy.\n\r", ch );
		return;
	    }
	    ktora = atoi( arg );
	    if ( ktora >= kol->droga || ktora < 0 )
	    {
		send_to_char( "Numer spoza zakresu.\n\r", ch );
		return;
	    }
	    sprintf( buf, "%s zastanawia si`e chwil`e po czym m`owi:\n\r", koles->short_descr );
	    buf[ 0 ] = UPPER( buf[ 0 ] );
	    send_to_char( buf, ch );

	    sprintf( buf, "%s, tak?\n\r%s\n\r",
		kol->drogi[ ktora ].nazwa, kol->drogi[ ktora ].droga );
	    send_to_char( buf, ch );
	    sprintf( buf, "Nale`z%s si`e %s.\n\r",
			    liczba( cost ) == 2 ? "`a" : "y",
			    ZLOTO_M( cost ) );
	    ch->gold -= cost;
	    send_to_char( buf, ch );
	    act( ZM_SLUCH, "$n pyta o co`s $#.", ch, NULL, koles, TO_ROOM );

	    return;
	}

/* targowanie sie - moby 0% */
	if ( IS_NPC( ch ) )
	    haggle = 0;
	else
	    haggle = ch->pcdata->learned[ gsn_haggle ] + number_bits( 3 );

	/* / 400, ale napisalem tak, zeby bylo widac jak to dziala */
	pom = (float) ( 1 - (float) haggle / 100 * 0.25 );
	if ( pom < (float) 0.5 )
	    pom = (float) 0.5;

	/* kupowanie czarow */
	if ( ( keeper = find_healer( ch, arg2 ) ) )
	/* tak, jest tu sprzedawca czarow */
	{
	    int pzl;
	    int sn;

	    /* sprzedawca moze sprzedawac 5 roznych rodzajow czarow */
	    for ( pzl = 0; pzl < 5; pzl++ )
		/* czy ma tam jakikolwiek czar do sprzedania? */
		if ( keeper->pIndexData->pHealer->spells[ pzl ] > 0
		/* czy ch podal jako parametr nazwe czaru? */
		  && !str_prefix( arg, skill_table[ keeper->pIndexData->pHealer->spells[ pzl ] ].pl_name ) )
		{
		    /* czy stac nas na czar? */
		    sn = keeper->pIndexData->pHealer->spells[ pzl ];
		    cost = keeper->pIndexData->pHealer->cost[ pzl ];
		    targ = (int) ( pom * cost );
		    if ( ch->gold < targ )
		    /* jednak nas nie stac */
		    {
			sprintf( buf, "Nie sta`c ci`e na czar \"%s\".\n\r",
				skill_table[ sn ].pl_name );
			send_to_char( buf, ch );
			return;
		    }

		    /* kupujemy czar */
		    sprintf( buf, "Kupujesz czar \"%s\" za %d monet, targuj`ac si`e do %d monet.\n\r",
			skill_table[ sn ].pl_name,
			cost, targ );
		    send_to_char( buf, ch );
		    /* inni tez niech wiedza, ale nie za duzo */
		    act( ZM_WZROK | ZM_WID_CHAR, "$n kupuje jaki`s czar.", ch, NULL, NULL, TO_ROOM );
		    /* zaplata (tylko zabieranie, keeper niczego nie dostaje) */
		    ch->gold -= targ;

		    /* Ulryk: musi byc defensywny albo TAR_IGNORE z flaga
		       SKILL_KUPOWALNE

		       Lam: SKILL_KUPOWALNE sami ustawiamy, TAR_CHAR_SELF jest
		       w porzadku, a reszte kontrolujemy sami, usuwam test na
		       TAR_IGNORE */
		    if ( skill_table[ sn ].target != TAR_CHAR_DEFENSIVE
		      && ( skill_table[ sn ].spell_fun == spell_null
			|| skill_table[ sn ].spell_fun == NULL
			|| !IS_SET( skill_table[ sn ].flags, SKILL_KUPOWALNE ) ) )
			return;

		    /* keeper rzuca czar na ch */
		    strcpy( buf, name_expand( keeper, ch ) );
		    target_name = buf;
		    ( *skill_table[ sn ].spell_fun )
			( sn, URANGE( 1, keeper->level, LEVEL_HERO ),
			keeper, (void *) ch, USAGE_HEALER );
		    target_name = "";
		    if ( haggle > 58 ) uzyj( ch, gsn_haggle, "targowaniu si`e" );
			return;
		}
	    /* jezeli nie ma takiego czaru u sprzedawcy */
	    send_to_char( "Tu tego nie mo`zesz kupi`c.\n\r", ch );
	    return;
	}
	/* koniec czesci odpowiedzialnej za sprzedawcow czarow */

	if ( is_number( arg ) )
	{
	    item_count = atoi( arg );
	    strcpy( arg, arg2 );
	    strcpy( arg2, arg3 );
	}

	if ( !( keeper = find_keeper( ch, arg2 ) ) )
	    return;

	obj  = get_obj_shop( keeper, arg );
	cost = get_cost( keeper, obj, TRUE );
	if ( IS_NPC( keeper ) )
	    targ = (int) ( pom * cost );
	else
	{
	    targ = cost; /* gracze sie nie targuja */
	    haggle = 0;
	}
	/* mozna zrobic, zeby ch uzywal gsn_haggle, a keeper gsn_fetch i na tej
	   podstawie sie targowali (z warunkiem, ze pom <= 1.0, czyli wciskanie
	   tylko do obrony przed targowaniem), ale to niesprawiedliwe dla
	   nie-zlodziei, poza tym uznajemy, ze gracze sie po prostu nie targuja
	   (kupujacy akceptuje cene, sprzedajacy nie chce sie targowac) */

	if ( cost <= 0 || !can_see_obj( ch, obj ) )
	{
	    act( ZM_ZAWSZE, "$n nie sprzedaje takiego towaru - spr`obuj napisa`c \"lista\".",
		keeper, NULL, ch, TO_VICT );
	    return;
	}

	if ( item_count < 1 )
	{
	    send_to_char( "Ile chcesz kupi`c? Ilo`s`c musi by`c wi`eksza ni`z 0.\n\r", ch );
	    return;
	}

	if ( obj->level > ch->level )
	{
	    act( ZM_ZAWSZE, "Jeszcze nie mo`zesz u`zywa`c $g.", keeper, obj, ch, TO_VICT );
	    return;
	}

	if ( IS_SET( czy_pasuje( obj, ch ), ~WRONG_PERSON ) )
	{
	    act( ZM_ZAWSZE, "I tak nie mo`zesz u`zywa`c $g.", keeper, obj, ch, TO_VICT );
	    return;
	}

	if ( ch->gold < ( targ * item_count ) )
	{
	    if ( item_count == 1 )
	    {
		act( ZM_ZAWSZE, "Nie sta`c ci`e na zakup $g.", keeper, obj, ch, TO_VICT );
		return;
	    }
	    else
	    {
		if ( ( ch->gold / targ ) > 0 )
		    sprintf( buf, "Sta`c ci`e najwy`zej na %d!",
			    ( ch->gold / targ ) );
		else
		    sprintf( buf, "Nie sta`c ci`e nawet na jedn`a sztuk`e." );

		act( ZM_ZAWSZE, buf, keeper, obj, ch, TO_VICT );
		return;
	    }
	}

	/* jesli ktos sprzeda przedmiot dostepny za PZ, nie sprzeda go za PZ,
	   wiec drugi sprzedawca tez PZ nie powinien zadac */
	if ( IS_NPC( keeper )
	  && keeper->pIndexData->pShop->quest_shop
	  && IS_OBJ_STAT( obj, ITEM_INVENTORY )
	  && ch->qp < item_count * obj->pIndexData->qp )
	{
	    act( ZM_ZAWSZE, "Masz za ma`lo punkt`ow zada`n.", keeper, NULL, ch, TO_VICT );
	    return;
	}

	if ( ch->carry_number + ( item_count * get_obj_number( obj ) ) >
	    can_carry_n( ch ) )
	{
	    if ( item_count == 1 )
	    {
		act( ZM_ZAWSZE, "Nie uniesiesz $g.", keeper, obj, ch, TO_VICT );
		return;
	    }
	    else
	    {
		if ( ( ( can_carry_n( ch ) - ch->carry_number ) / get_obj_number( obj ) ) > 0 )
		    sprintf( buf, "Tw`oj inwentarz pomie`sci najwy`zej %d!",
			    ( can_carry_n( ch ) - ch->carry_number ) / get_obj_number( obj ) );
		else
		    sprintf( buf, "Nie masz nawet odrobiny miejsca w inwentarzu." );

		act( ZM_ZAWSZE, buf, keeper, obj, ch, TO_VICT );
		return;
	    }
	}

	if ( ch->carry_weight + ( item_count * get_obj_weight( obj ) ) >
	    can_carry_w( ch ) )
	{
	    if ( item_count == 1 )
	    {
		act( ZM_ZAWSZE, "Nie ud`xwigniesz $g.", keeper, obj, ch, TO_VICT );
		return;
	    }
	    else
	    {
		if ( ( ( can_carry_w( ch ) - ch->carry_weight ) / get_obj_weight( obj ) ) > 0 )
		    sprintf( buf, "Ud`xwigniesz najwy`zej %d!",
			    ( can_carry_w( ch ) - ch->carry_weight ) / get_obj_weight( obj ) );
		else
		    sprintf( buf, "Nie ud`xwigniesz nawet jednej sztuki." );

		act( ZM_ZAWSZE, buf, keeper, obj, ch, TO_VICT );
		return;
	    }
	}

	if ( ( item_count > 1 )
	  && ( !IS_NPC( keeper ) || !IS_OBJ_STAT( obj, ITEM_INVENTORY ) ) )
	{
	    act( ZM_ZAWSZE, "$n ma tylko 1 $j.", keeper, obj, ch, TO_VICT );
	    return;
	}

	if ( item_count == 1 )
	{
	    if ( IS_NPC( keeper ) )
		sprintf( buf, "Kupujesz $j, targuj`ac si`e do %d z`lot%s monet%s.",
			targ, targ == 1 ? "ej" : "ych", targ == 1 ? "y" : "" );
	    else
		sprintf( buf, "Kupujesz od $! $j za %s.",
			zlota_moneta( targ, FALSE, TRUE ) );
	    act( ZM_ZAWSZE, buf, ch, obj, keeper, TO_CHAR );
	    if ( IS_NPC( keeper ) )
		sprintf( buf, "$n kupuje $j, targuj`ac si`e do %d z`lot%s monet%s.",
			targ, targ == 1 ? "ej" : "ych", targ == 1 ? "y" : "" );
	    else
		sprintf( buf, "$n kupuje od ciebie $j za %s.",
			zlota_moneta( targ, FALSE, TRUE ) );
	    act( ZM_ZAWSZE, buf, ch, obj, keeper, TO_VICT );
	    /* nie chcialo mi sie poprawiac stylu na "$n cos kupuje" */
	    act( ZM_WZROK | ZM_WID_CHAR, "$n kupuje $j.", ch, obj, keeper, TO_NOTVICT );
	}
	else
	{
	    int licz = targ * item_count;

	    if ( !IS_NPC( keeper ) )
		bug( "Tego nie obsluguja komunikaty.", 0 );
	    sprintf( buf, "$n kupuje %d * $j.", item_count );
	    act( ZM_WZROK | ZM_WID_CHAR, buf, ch, obj, NULL, TO_ROOM );
	    sprintf( buf, "Kupujesz %d * $j, targuj`ac si`e do %d z`lot%s monet%s.",
		item_count, licz,
		licz == 1 ? "ej" : "ych",
		licz == 1 ? "y" : "" );
	    act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	}

	if ( IS_NPC( keeper )
	  && keeper->pIndexData->pShop->quest_shop
	  && obj->pIndexData->qp > 0
	  && IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	{
	    int licz = liczba( item_count * obj->pIndexData->qp );
	    ch->qp -= item_count * obj->pIndexData->qp;
	    sprintf( buf, "Wydajesz na to %d punkt%s zada`n.\n\r",
			item_count * obj->pIndexData->qp,
			licz == 1 ? ""
			: licz == 2 ? "y"
			: "`ow" );
	    STC( buf, ch );
	}
	ch->gold -= targ * item_count;
	if ( !IS_NPC( keeper ) )
	    keeper->gold += targ * item_count;
	if ( haggle > 58 )
	    uzyj( ch, gsn_haggle, "targowaniu si`e" );

	/* Qwert: teraz zmiana poziomu przy kupowaniu */
	if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	    for ( ; item_count > 0; item_count-- )
	    {
		obj = create_object( obj->pIndexData, obj->level );
		zmien_poziom( obj, ch );
		obj_to_char( obj, ch );
	    }
	else
	{
	    zmien_poziom( obj, ch );
	    obj_from_char( obj );
	    potencjalnie_zamknij_sklep( keeper );
	    obj_to_char( obj, ch );
	}

	return;
    }

    return;
}



KOMENDA( do_list )
{
    char buf[ MAX_STRING_LENGTH ];
    char buf1[ MAX_STRING_LENGTH * 4 ];

    buf1[ 0 ] = '\0';

    if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
	CHAR_DATA       *pet;
	ROOM_INDEX_DATA *pRoomIndexNext;
	bool             found;

	pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
	if ( !pRoomIndexNext )
	{
	    bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "Tu nie mo`zesz tego robi`c.\n\r", ch );
	    return;
	}

	found = FALSE;
	for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
	{
	    if ( IS_SET( pet->act, ACT_PET ) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    strcat( buf1, "Zwierz`eta na sprzeda`z:\n\r" );
		}
		sprintf( buf, "[%2d] %8d - %s\n\r",
			pet->level,
			WYDATEK( pet->level, 10 ),
			pet->short_descr );
		strcat( buf1, buf );
	    }
	}
	if ( !found )
	    send_to_char( "W`la`snie skonczy`ly nam si`e zwierz`eta.\n\r", ch );

	send_to_char( buf1, ch );
	return;
    }
    else
    {
	CHAR_DATA *koles;
	KOLES_DATA *kol;
	int	   numer;
	OBJ_DATA  *obj;
	CHAR_DATA *keeper;
	char       arg[ MAX_INPUT_LENGTH ];
	char       arg2[ MAX_INPUT_LENGTH ];
	char       nazwaprz[ MAX_INPUT_LENGTH ];
	int        cost;
	bool       found;
	bool       stala;
	int        licz;

	argument = one_argument( argument, arg );
	one_argument( argument, arg2 );

	/* Lam 15.5.2000: sprzedawcow drog tez da sie wybierac po nazwie, ale
	   pierwszy argument jest ignorowany, tak jak przy czarach */
	if ( ( koles = find_koles( ch, arg2 ) ) )
	{
	    if ( !( kol = znajdz_kolesia( koles->pIndexData->vnum ) ) )
	    {
		bug( "nie znalazlem kolesia!", 0 );
		return;
	    }
	    sprintf( buf, "{g%s mo`ze powiedzie`c ci drog`e do krain:{x\n\r", koles->short_descr);
	    buf[ 2 ] = UPPER( buf[ 2 ] );
	    strcpy( buf1, buf );
	    for ( numer = 0; kol->drogi[ numer ].nazwa; numer++ )
	    {
		sprintf( buf, "  %2d. %s\n\r", numer, kol->drogi[ numer ].nazwa );
		strcat( buf1, buf );
	    }
	    cost = WYDATEK( ch->level, 15 );
	    sprintf( buf, "{yZa dowiedzenie si`e dowolnej z dr`og p`lacisz {Y%d{y %s.\n\r",
		cost, zlota_moneta( cost, FALSE, FALSE ) );
	    strcat( buf1, buf );
	    licz = liczba( ch->gold );
	    sprintf( buf, "Twoje {Y%d{y monet%s wystarcz%s na poznanie {Y%d{y %s.{x\n\r",
		ch->gold,
		licz == 1 ? "a" : licz == 2 ? "y" : "",
		licz == 2 ? "`a" : "y",
		ch->level ? ch->gold / cost : 0,
		ch->level ? ( ch->gold / cost == 1 ? "drogi" : "dr`og" ) : "dr`og" );
	    strcat( buf1, buf );
	    send_to_char( buf1, ch );
	    return;
	}

	/* tu czesc odpowiedzialna za uzdrawiaczy sprzedajacych czary */
	if ( ( keeper = find_healer( ch, arg2 ) ) )
	{
	    int numer_pom;

	    sprintf( buf, "{g%s ma do zaoferowania czary:{x\n\r", keeper->short_descr);
	    buf[ 2 ] = UPPER( buf[ 2 ] );
	    for ( numer_pom = 0; numer_pom < 5; numer_pom++ )
		if ( keeper->pIndexData->pHealer->spells[numer_pom] > 0 )
		{
		    sprintf( buf1, "  {G%s{x za {Y%d{x %s\n\r",
			skill_table[ keeper->pIndexData->pHealer->spells[ numer_pom ] ].pl_name,
			keeper->pIndexData->pHealer->cost[ numer_pom ],
			zlota_moneta( keeper->pIndexData->pHealer->cost[ numer_pom ], FALSE, FALSE ) );
		    strcat( buf, buf1 );
		}

	    STC( buf, ch );
	    return;
	}

	if ( !( keeper = find_keeper( ch, arg2 ) ) )
	    return;

	found = FALSE;
	for ( obj = keeper->carrying; obj; obj = obj->next_content )
	{
	    if ( !IS_NPC( keeper ) )
		break; /* gracze nie maja nieskonczonej stalej ekspozycji */

	    if ( IS_NPC( keeper ) && !IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
		continue;

	    if ( obj->wear_loc != WEAR_NONE
	      || ( cost = get_cost( keeper, obj, TRUE ) ) <= 0 )
		continue;

	    if ( can_see_obj( ch, obj )
	      && ( arg[ 0 ] == '\0'
		|| czy_wszystko( arg )
		|| is_name( arg, obj->name ) ) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    strcat( buf1, "{gW sta`lej ekspozycji sklepu znajduj`a si`e:{x\n\r" );
		    if ( keeper->pIndexData->pShop->quest_shop )
			strcat( buf1, "{Y[{MPzm{R    Cena{C   PZ{Y]{y Rzecz {g(na ile ci`e sta`c){x\n\r" );
		    else
			strcat( buf1, "{Y[{MPzm{R    Cena{Y]{y Rzecz {g(na ile ci`e sta`c){x\n\r" );
		}

		/* Lam 17.6.2006 */
		if ( obj->item_type == ITEM_DRINK_CON
		  && obj->value[ 1 ].v != 0 )
		{
		    int liquid = obj->value[ 2 ].v;

		    if ( liquid < 0 || liquid >= LIQ_MAX )
			liquid = LIQ_WATER;

		    if ( obj->rodzaj == 3 )
			sprintf( nazwaprz, "%s (zawieraj`a %s)",
				obj->short_descr,
				liq_table_pl[ liquid ].liq_biernik );
		    else
			sprintf( nazwaprz, "%s (zawiera %s)",
				obj->short_descr,
				liq_table_pl[ liquid ].liq_biernik );
		}
		else
		    strcpy( nazwaprz, obj->short_descr );

		if ( keeper->pIndexData->pShop->quest_shop )
		    sprintf( buf, "{Y[{m%3d{r %7d{c %4d{Y]{x %s   (%d)\n\r",
			obj->level, cost, obj->pIndexData->qp, nazwaprz,
			UMIN( 999, UMIN( cost > 0 ? ch->gold / cost : 999,
			obj->pIndexData->qp > 0 ? ch->qp / obj->pIndexData->qp : 999 ) ) );
		else
		    sprintf( buf, "{Y[{m%3d {r%7d{Y]{x %s   (%d)\n\r",
			obj->level, cost, nazwaprz,
			UMIN( 999, cost > 0 ? ch->gold / cost : 999 ) );
		strcat( buf1, buf );
		if ( strlen( buf1 ) > sizeof( buf1 ) - MIL )
		{
		    strcat( buf1, "(Za du`zo przedmiot`ow, by wy`swietli`c wszystkie!)\n\r" );
		    break;
		}
	    }
	}

	licz = 0;
	stala = found;
	for ( obj = keeper->carrying; obj; obj = obj->next_content )
	{
	    if ( ( IS_NPC( keeper ) && IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	      || ( !IS_NPC( keeper ) && !IS_OBJ_STAT( obj, ITEM_PLAYER_SHOP ) ) )
		continue;

	    if ( obj->wear_loc != WEAR_NONE
	      || ( cost = get_cost( keeper, obj, TRUE ) ) <= 0 )
		continue;

	    if ( can_see_obj( ch, obj )
	      && ( arg[ 0 ] == '\0'
		|| czy_wszystko( arg )
		|| is_name( arg, obj->name ) ) )
	    {
		licz++;
		/* jesli podal "wszystko" albo nazwe przedmiotu, nie ma
		   ograniczenia, a jesli nie podal argumentu, ucinamy */
		if ( !*arg && licz > 10 )
		{
		    strcat( buf1, "{g(I nie tylko! U`zyj argumentu \"wszystko\", aby zobaczy`c pe`len asortyment.){x\n\r" );
		    break;
		}

		if ( !found || stala )
		{
		    if ( found )
			strcat( buf1, "\n\r{gDodatkowo mo`zna tu dzi`s dosta`c:\n\r" );
		    else
			strcat( buf1, "{gMo`zna tu dzi`s dosta`c:\n\r" );
		    found = TRUE;
		    stala = FALSE;
		    strcat( buf1, "{Y[{MPzm{R    Cena{Y]{y Rzecz {g(na ile ci`e sta`c){x\n\r" );
		}

		sprintf( buf, "{Y[{m%3d {r%7d{Y]{x %s   (%d)\n\r",
			obj->level, cost, obj->short_descr,
			UMIN( 999, cost > 0 ? ch->gold / cost : 999 ) );
		strcat( buf1, buf );
		if ( strlen( buf1 ) > sizeof( buf1 ) - MIL )
		{
		    strcat( buf1, "(Za du`zo przedmiot`ow, by wy`swietli`c wszystkie!)\n\r" );
		    break;
		}
	    }
	}

	if ( !found )
	{
	    if ( arg[ 0 ] == '\0' || czy_wszystko( arg ) )
		send_to_char( "Niczego tu nie mo`zesz kupi`c.\n\r", ch );
	    else
		send_to_char( "Tu tego nie dostaniesz.\n\r", ch );
	    return;
	}
	else
	{
	    sprintf( buf, "{gPosiadasz {G%d{g %s.{x\n\r",
			ch->gold, zlota_moneta( ch->gold, FALSE, FALSE ) );
	    strcat( buf1, buf );
	}

	send_to_char( buf1, ch );
	return;
    }

    return;
}


/*
 * Lam: dodalem "wciskanie"
 */
KOMENDA( do_sell )
{
    OBJ_DATA  *obj;
    CHAR_DATA *keeper;
    char       buf[ MAX_STRING_LENGTH ];
    char       arg[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    int        cost;
    int        targ;
    int        fetch;
    float      pom;

    /* targowanie sie - moby 0% */
    if ( IS_NPC( ch ) )
	fetch = 0;
      else
	fetch = ch->pcdata->learned[ gsn_fetch ];

    pom = (float) ( 1 + (float) fetch / 100 * 0.2 );

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co sprzeda`c?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( !( keeper = find_keeper( ch, arg2 ) ) )
	return;

    if ( !IS_NPC( keeper ) )
    {
	act( ZM_ZAWSZE, "$n nie skupuje przedmiot`ow automatycznie.",
		keeper, NULL, ch, TO_VICT );
	return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	STC( "Nie masz takiego przedmiotu.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "Nie mo`zesz si`e tego pozby`c.\n\r", ch );
	return;
    }

    if ( !can_see_obj( keeper, obj ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze zobaczy`c $g.", keeper, obj, ch, TO_VICT );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0
      || obj->level >= LEVEL_HERO )
    {
	act( ZM_ZAWSZE, "$n nie jest zainteresowan$y $k.", keeper, obj, ch, TO_VICT );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
    {
	act( ZM_ZAWSZE, "$n nie skupuje rzeczy zatrutych.", keeper, NULL, ch, TO_VICT );
	return;
    }

    targ = cost;
    if ( fetch > 50 )
    {
	if ( cost < 10 )
	    targ = (int) ( cost * 3 * pom );
	else if ( cost < 100 )
	    targ = (int) ( cost * 2 * pom );
	else if ( cost < 1000 )
	    targ = (int) ( cost * 1.5 * pom );
	else
	    targ = (int) ( cost * pom );

	uzyj( ch, gsn_fetch, "zachwalaniu towar`ow" );
    }

    sprintf( buf, "Sprzedajesz $j o warto`sci %d za %s.",
	    cost, ZLOTO_B( targ ) );
    act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n sprzedaje $j.", ch, obj, NULL, TO_ROOM );
    ch->gold     += targ;

    if ( obj->item_type == ITEM_TRASH )
	extract_obj( obj );
    else
    {
	obj_from_char( obj );
	obj_to_char( obj, keeper );
    }

    return;
}


KOMENDA( do_value )
{
    OBJ_DATA  *obj;
    CHAR_DATA *keeper;
    char       buf[ MAX_STRING_LENGTH ];
    char       arg[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    int        cost;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co wyceni`c?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( ( keeper = find_przechowalnia( ch, FALSE ) ) )
    {
	/* kopia do_przechowaj */
	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
	    act( ZM_ZAWSZE, "Nie masz tego.", keeper, NULL, ch, TO_VICT );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "Nie mo`zesz si`e tego pozby`c.\n\r", ch );
	    return;
	}

	if ( !can_see_obj( keeper, obj ) )
	{
	    act( ZM_ZAWSZE, "$n nie mo`ze tego dostrzec.", keeper, NULL, ch, TO_VICT );
	    return;
	}

	/* Ulryk: skoro przedmiot zadaniowy, to nie widze sensu w oddawaniu go
	   do przechowalni */
	if ( IS_OBJ_STAT( obj, ITEM_QUEST )
	  || obj->timer >= 0
	  || obj->level > ch->level
	  || obj->item_type == ITEM_KEY
	  || obj->item_type == ITEM_TRASH )
	{
	    send_to_char( "Nie mo`zesz tego odda`c na przechowanie.\n\r", ch );
	    return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
	{
	    act( ZM_ZAWSZE, "Przechowalnia nie przechowuje rzeczy zatrutych.",
		keeper, NULL, ch, TO_VICT );
	    return;
	}

	cost = WYDATEK( ( ch->level + obj->level ) / 2 + 1, 20 );

	sprintf( buf, "Mo`zesz przechowa`c $j za %s.", ZLOTO_B( cost ) );
	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );

	return;
    }

    if ( !( keeper = find_keeper( ch, arg2 ) ) )
	return;

    if ( !IS_NPC( keeper ) )
    {
	act( ZM_ZAWSZE, "$n nie skupuje przedmiot`ow automatycznie.",
		keeper, NULL, ch, TO_VICT );
	return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	STC( "Nie masz takiego przedmiotu.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "Nie mo`zesz si`e tego pozby`c.\n\r", ch );
	return;
    }

    if ( !can_see_obj( keeper, obj ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze zobaczy`c $g.", keeper, obj, ch, TO_VICT );
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0
      || obj->level >= LEVEL_HERO )
    {
	act( ZM_ZAWSZE, "$n nie jest zainteresowan$y $k.", keeper, obj, ch, TO_VICT );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
    {
	act( ZM_ZAWSZE, "$n nie skupuje rzeczy zatrutych.", keeper, NULL, ch, TO_VICT );
	return;
    }

    sprintf( buf, "$n da ci za $j %s.",  ZLOTO_B( cost ) );
    act( ZM_ZAWSZE, buf, keeper, obj, ch, TO_VICT );

    return;
}


/* Poison weapon by Thelonius for EnvyMud */
KOMENDA( do_poison_weapon )
{
    OBJ_DATA *obj;
    OBJ_DATA *pobj;
    OBJ_DATA *wobj;
    char      arg[ MAX_INPUT_LENGTH ];

    /* Don't allow mobs or unskilled pcs to do this */
    if ( IS_NPC( ch )
      || !moze_uzyc( ch, gsn_poison_weapon ) )
    {
	send_to_char( "Wydaje ci si`e, `ze jeste`s z`lodziejem?\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co usi`lujesz zatru`c?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( ch->fighting )
    {
	send_to_char( "Podczas walki?\n\r", ch );
	return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	send_to_char( "Nie masz tej broni przy sobie.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_WEAPON )
    {
	send_to_char( "To nie jest bro`n.\n\r", ch );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
    {
	send_to_char( "Ta bro`n ju`z jest zatruta.\n\r", ch );
	return;
    }

    /* Now we have a valid weapon...check to see if we have the powder. */
    for ( pobj = ch->carrying; pobj; pobj = pobj->next_content )
	if ( pobj->pIndexData->vnum == OBJ_VNUM_BLACK_POWDER )
	    break;

    if ( !pobj )
    {
	send_to_char( "Nie masz sproszkowanej trucizny.\n\r", ch );
	return;
    }

    /* Okay, we have the powder...do we have water? */
    for ( wobj = ch->carrying; wobj; wobj = wobj->next_content )
	if ( wobj->item_type == ITEM_DRINK_CON
	    && wobj->value[ 1 ].v  >  0
	    && wobj->value[ 2 ].v  == 0 )
	    break;

    if ( !wobj )
    {
	send_to_char( "Nie masz wody, aby zmiesza`c j`a z proszkiem.\n\r", ch );
	return;
    }

    /* Great, we have the ingredients...but is the thief smart enough? */
    if ( !IS_NPC( ch ) && get_curr_wis( ch ) < 19 )
    {
	send_to_char( "Nie potrafisz sobie przypomnie`c, jak to si`e robi`lo...\n\r", ch );
	return;
    }

    /* And does the thief have steady enough hands? */
    if ( !IS_NPC( ch )
      && ( get_curr_dex( ch ) < 20
	|| ch->pcdata->condition[ COND_DRUNK ] > 0
	|| IS_AFFECTED( ch, AFF_DEZORIENTACJA ) ) )
    {
	send_to_char( "Twe r`ece nie s`a wystarczaj`aco zr`eczne, aby wymiesza`c trucizn`e.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_poison_weapon ].beats );

    /* Check the skill percentage */
    if ( !IS_NPC( ch )
      && number_percent( ) > ch->pcdata->learned[ gsn_poison_weapon ] )
    {
	ASTC( "Wyla`l$o`s trucizn`e na siebie. Au`c!", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wylewa na siebie trucizn`e.", ch, NULL, NULL, TO_ROOM );
	extract_obj( pobj );
	extract_obj( wobj );
	damage( ch, ch, ch->level, gsn_poison_weapon, WEAR_NONE, TRUE );
	return;
    }

    /* Lam 12.10.1999 $g -> $j */
    act( ZM_ZAWSZE, "Rozpuszczasz $j w $L, tworz`ac `smierteln`a trucizn`e!",
	ch, pobj, wobj, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1 | ZM_WID_OBJ2,
	"$n miesza $j w $L, tworz`ac `smierteln`a trucizn`e!",
	ch, pobj, wobj, TO_ROOM );
    act( ZM_ZAWSZE, "Rozprowadzasz trucizn`e na $j, nadaj`ac dzikiego blasku!",
	ch, obj, NULL, TO_CHAR  );
    act( ZM_WZROK | ZM_WID_CHAR,
	"$n rozprowadza trucizn`e na $j, nadaj`ac dzikiego blasku!",
	ch, obj, NULL, TO_ROOM  );
    SET_BIT_V( obj->extra_flags, ITEM_POISONED );
    obj->cost *= ch->level;

    /* Set an object timer.  Dont want proliferation of poisoned weapons */
    /* obj->timer = 24 + ch->level; */
    obj->timer = 120 + 5 * ch->level;

    if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
	obj->timer *= 2;

    /* WHAT?  All of that, just for that one bit?  How lame. ;) */
    act( ZM_ZAWSZE, "Resztki trucizny prze`zeraj`a $j.", /* Lam 10.12.1999 $g -> $j */
	ch, wobj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_OBJ1, "Resztki trucizny prze`zeraj`a $j.",
	ch, wobj, NULL, TO_ROOM );
    extract_obj( pobj );
    extract_obj( wobj );

    uzyj( ch, gsn_poison_weapon, "zatruwaniu broni" );

    return;
}


/* Contributed by BoneCrusher of EnvyMud. */
KOMENDA( do_donate )
{
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CHAR_DATA *rch;
    char      arg1[ MAX_INPUT_LENGTH ];
    char      urna[ MIL ];

    one_argument( argument, arg1 );

    if ( arg1[ 0 ] == '\0' )
    {
	send_to_char( "Co wys`la`c do urny na dary?\n\r", ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    sprintf( urna, "#urna_na_dary_%s", POCHODZENIE_M( ch ) );
    container = get_obj_world( ch, urna );

    if ( !czy_wszystko( arg1 ) && str_prefix( "wsz.", arg1 ) )
    {
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "Nie masz tego przy sobie.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "Nie mo`zesz si`e tego pozby`c.\n\r", ch );
	    return;
	}

	if ( !container )
	{
	    send_to_char( "Twoja kraina zosta`la pozbawiona urny na dary!\n\r", ch );
	    return;
	}

	if ( get_obj_weight( obj ) + get_obj_weight( container )
	  > container->value[ 0 ].v )
	{
	    send_to_char( "Ten przedmiot nie zmie`sci si`e w urnie.\n\r", ch );
	    return;
	}

	/* Alandar: pigulki sa dosc wartosciowe, usuwam z listy */
	/* Lam: rzeczy na poziomie 100 i wyzszych sa bezwartosciowe */
	if ( obj->item_type == ITEM_TRASH
	  || obj->item_type == ITEM_FOOD
	  || obj->item_type == ITEM_KEY
	  || obj->level >= LEVEL_HERO )
	{
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wysy`la $j do $G.", ch, obj, container, TO_ROOM );
	    act( ZM_ZAWSZE, "Wysy`lasz $j do $G.", ch, obj, container, TO_CHAR );
	    extract_obj( obj );
	    return;
	}

	obj_from_char( obj );
	obj_to_obj( obj, container );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wysy`la $j do $G.", ch, obj, container, TO_ROOM );
	act( ZM_ZAWSZE, "Wysy`lasz $j do $G.", ch, obj, container, TO_CHAR );
	if ( ( rch = container->in_room->people ) )
	{
	    act( ZM_SLUCH | ZM_WID_OBJ1, "Z $g dobiega g`lo`sny brz`ek!", rch, container, NULL, TO_ROOM );
	    act( ZM_SLUCH | ZM_WID_OBJ1, "Z $g dobiega g`lo`sny brz`ek!", rch, container, NULL, TO_CHAR );
	    act( ZM_SLUCH | ZM_W_WID_OBJ1, "Twych uszu dobiega g`lo`sny brz`ek!", rch, container, NULL, TO_ROOM );
	    act( ZM_SLUCH | ZM_W_WID_OBJ1, "Twych uszu dobiega g`lo`sny brz`ek!", rch, container, NULL, TO_CHAR );
	}
    }
    else
    {
	bool czy = FALSE;

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( !( czy_wszystko( arg1 )
		|| ( arg1[ 3 ] == '.' && is_name( &arg1[ 4 ], obj->name ) ) )
	      || !can_see_obj( ch, obj )
	      || obj->wear_loc != WEAR_NONE
	      || !can_drop_obj( ch, obj ) )
		continue;

	    if ( !container )
	    {
		send_to_char( "Twoja kraina zosta`la pozbawiona urny na dary!\n\r", ch );
		return;
	    }

	    if ( obj != container
	      && get_obj_weight( obj ) + get_obj_weight( container )
			<= container->value[ 0 ].v )
	    {
		czy = TRUE;

		/* Alandar: pigulki jw., Lam: poziom jw. */
		if ( obj->item_type == ITEM_TRASH
		  || obj->item_type == ITEM_FOOD
		  || obj->item_type == ITEM_KEY
		  || obj->level >= LEVEL_HERO )
		{
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wysy`la $j do $G.", ch, obj, container, TO_ROOM );
		    act( ZM_ZAWSZE, "Wysy`lasz $j do $G.", ch, obj, container, TO_CHAR );
		    extract_obj( obj );
		    continue;
		}

		obj_from_char( obj );
		obj_to_obj( obj, container );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wysy`la $j do $G.", ch, obj, container, TO_ROOM );
		act( ZM_ZAWSZE, "Wysy`lasz $j do $G.", ch, obj, container, TO_CHAR );
		if ( ( rch = container->in_room->people ) )
		{
		    act( ZM_SLUCH | ZM_WID_OBJ1, "Z $g dobiega g`lo`sny brz`ek!", rch, container, NULL, TO_ROOM );
		    act( ZM_SLUCH | ZM_WID_OBJ1, "Z $g dobiega g`lo`sny brz`ek!", rch, container, NULL, TO_CHAR );
		    act( ZM_SLUCH | ZM_W_WID_OBJ1, "Twych uszu dobiega g`lo`sny brz`ek!", rch, container, NULL, TO_ROOM );
		    act( ZM_SLUCH | ZM_W_WID_OBJ1, "Twych uszu dobiega g`lo`sny brz`ek!", rch, container, NULL, TO_CHAR );
		}
	    }
	}

	if ( !czy )
	    STC( "Nie uda`lo si`e niczego wys`la`c.\n\r", ch );
    }

    return;
}


KOMENDA( do_register )
{
    send_to_char( "Rejestracja wcale nie jest potrzebna do zabijania.\n\r", ch );

    return;
}


/*
 * sciagniete z find_koles( ), a wczesniej z find_keeper( )
 */
CHAR_DATA *find_przechowalnia( CHAR_DATA *ch, bool komunikat )
{
    CHAR_DATA *keeper;
    char       buf[ MAX_STRING_LENGTH ];

    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
	if ( IS_NPC( keeper ) && ( keeper->pIndexData->vnum == MOB_VNUM_PRZECHOWALNIA ) )
	    break;

    if ( !keeper || ( keeper->pIndexData->vnum != MOB_VNUM_PRZECHOWALNIA )
		 || ( keeper && IS_AFFECTED( keeper, AFF_CHARM ) ) )
    {
	if ( komunikat )
	    send_to_char( "Nie ma tu obs`lugi przechowalni.\n\r", ch );
	return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
	if ( komunikat )
	    act( ZM_ZAWSZE, "$n nie mo`ze ci`e zobaczy`c.", keeper, NULL, ch, TO_VICT );
	return NULL;
    }

    /*
     * Undesirables.
     */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_KILLER ) )
    {
	do_say( keeper, "Nie lubimy tu morderc`ow!" );
	sprintf( buf, "%s MORDERCA jest tu i chce co`s przechowa`c!", ch->name );
	do_yell( keeper, buf );
	return NULL;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_THIEF ) )
    {
	do_say( keeper, "Nie lubimy tu z`lodziei!" );
	sprintf( buf, "%s Z`LODZIEJ jest tu i chce co`s przechowa`c!", ch->name );
	do_yell( keeper, buf );
	return NULL;
    }

    return keeper;
}


void list_przechowalnia( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    char buf[ MAX_STRING_LENGTH * 4 ];
    char buf1[ MAX_STRING_LENGTH ];

    if ( !ch->przechowalnia )
    {
	send_to_char( "Niczego nie przechowujesz.\n\r", ch );
	return;
    }

    buf[ 0 ] = 0; /* rownie dobrze byloby '\0', to tylko zmiana typu */
    strcat( buf, "Aktualnie trzymasz w przechowalni:\n\r" );

    for ( obj = ch->przechowalnia; obj; obj = obj->next_content )
    {
	sprintf( buf1, "  %s\n\r", obj->short_descr );
	strcat( buf, buf1 );
    }

    send_to_char( buf, ch );

    return;
}


KOMENDA( do_przechowaj )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *tobj;
    CHAR_DATA *keeper;
    char       buf[ MAX_STRING_LENGTH ];
    char       arg[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    int        cost;
    int        count;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[ 0 ] == '\0' )
    {
	list_przechowalnia( ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( !( keeper = find_przechowalnia( ch, TRUE ) ) )
	return;

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	act( ZM_ZAWSZE, "Nie masz tego.", keeper, NULL, ch, TO_VICT );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "Nie mo`zesz si`e tego pozby`c.\n\r", ch );
	return;
    }

    if ( !can_see_obj( keeper, obj ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze tego dostrzec.", keeper, NULL, ch, TO_VICT );
	return;
    }

    /* Ulryk: skoro przedmiot zadaniowy, to nie widze sensu w oddawaniu go do
       przechowalni */
    if ( IS_OBJ_STAT( obj, ITEM_QUEST )
      || obj->timer >= 0
      || obj->level > ch->level
      || obj->item_type == ITEM_KEY
      || obj->item_type == ITEM_TRASH )
    {
	send_to_char( "Nie mo`zesz tego odda`c na przechowanie.\n\r", ch );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
    {
	act( ZM_ZAWSZE, "Przechowalnia nie przechowuje rzeczy zatrutych.",
	    keeper, NULL, ch, TO_VICT );
	return;
    }

    count = 0;
    for ( tobj = ch->przechowalnia; tobj; tobj = tobj->next_content )
	count++;

    if ( count >= UMAX( ( ch->level / 5 ) + 1, 5 ) )
    {
	act( ZM_ZAWSZE, "Nie mo`zesz przechowywa`c wi`ekszej ilo`sci przedmiot`ow.",
	    keeper, NULL, ch, TO_VICT );
	return;
    }

    cost = WYDATEK( ( ch->level + obj->level ) / 2 + 1, 20 );

    if ( cost > ch->gold )
    {
	STC( "Nie posiadasz wystarczaj`acej ilo`sci z`lota.\n\r", ch );
	return;
    }

    sprintf( buf, "Oddajesz $j na przechowanie za %s.", ZLOTO_B( cost ) );
    act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_OBJ1, "$n oddaje $j na przechowanie.", ch, obj, NULL, TO_ROOM );
    ch->gold -= cost;

    obj_from_char( obj );
    obj_to_przechowalnia( obj, ch );

    return;
}


KOMENDA( do_odbierz )
{
    OBJ_DATA  *obj;
    CHAR_DATA *keeper;
    char       arg[ MAX_INPUT_LENGTH  ];
    char       arg2[ MAX_INPUT_LENGTH  ];

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[ 0 ] == '\0' )
    {
	list_przechowalnia( ch );
	return;
    }

    if ( !( keeper = find_przechowalnia( ch, TRUE ) ) )
	return;

    if ( !( obj = get_obj_przechowalnia( ch, arg ) ) )
    {
	act( ZM_ZAWSZE, "Nie przechowujesz niczego takiego.",
	    keeper, NULL, ch, TO_VICT );
	return;
    }

    /* To nie powinno miec miejsca, o ile get_obj_przechowalnia nie zepsuje */
    if ( !can_see_obj( ch, obj ) )
    {
	act( ZM_ZAWSZE, "Nie mo`zesz tego zobaczy`c.",
	    keeper, NULL, ch, TO_VICT );
	return;
    }

    act( ZM_ZAWSZE, "Odbierasz $j z przechowalni.", ch, obj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n odbiera $j z przechowalni.", ch, obj, NULL, TO_ROOM );

    obj_from_przechowalnia( obj );
    obj_to_char( obj, ch );

    return;
}


/* AUKCJA */
/* zawartosc pliku bet.h: */

/*
 * The following code was written by Erwin Andreasen for the automated
 * auction command.
 *
 * Completely cleaned up by Thoric
 * Lam tez dolozyl swoje.
 */


/*
  util function, converts an 'advanced' ASCII-number-string into a number.
  Used by parsebet() but could also be used by do_give or do_wimpy.

  Advanced strings can contain 'k' (or 'K') and 'm' ('M') in them, not just
  numbers. The letters multiply whatever is left of them by 1,000 and
  1,000,000 respectively. Example:

  14k = 14 * 1,000 = 14,000
  23m = 23 * 1,000,0000 = 23,000,000

  If any digits follow the 'k' or 'm', the are also added, but the number
  which they are multiplied is divided by ten, each time we get one left. This
  is best illustrated in an example :)

  14k42 = 14 * 1000 + 14 * 100 + 2 * 10 = 14420

  Of course, it only pays off to use that notation when you can skip many 0's.
  There is not much point in writing 66k666 instead of 66666, except maybe
  when you want to make sure that you get 66,666.

  More than 3 (in case of 'k') or 6 ('m') digits after 'k'/'m' are automatically
  disregarded. Example:

  14k1234 = 14,123

  If the number contains any other characters than digits, 'k' or 'm', the
  function returns 0. It also returns 0 if 'k' or 'm' appear more than
  once.
*/

int advatoi( char *s )
{
    int number = 0;	/* number to be returned */
    int multiplier = 0;	/* multiplier used to get the extra digits right */

    /*
     * as long as the current character is a digit add to current number
     */
    while ( isdigit( (unsigned char) s[ 0 ] ) )
	number = ( number * 10 ) + ( *s++ - '0' );

    switch ( UPPER( s[ 0 ] ) )
    {
	case 'K'  : number *= ( multiplier = 1000 );	++s; break;
	case 'M'  : number *= ( multiplier = 1000000 );	++s; break;
	case '\0' : break;
	default   : return 0; /* not k nor m nor NULL - return 0! */
    }

    /* if any digits follow k/m, add those too */
    while ( isdigit( (unsigned char) s[ 0 ] ) && ( multiplier > 1 ) )
    {
	/* the further we get to right, the less the digit 'worth' */
	multiplier /= 10;
	number = number + ( ( *s++ - '0' ) * multiplier );
    }

    /* return 0 if non-digit character was found, other than NULL */
    if ( s[ 0 ] != '\0' && !isdigit( (unsigned char) s[ 0 ] ) )
	return 0;

    /* anything left is likely extra digits (ie: 14k4443  -> 3 is extra) */

    return number;
}


/*
  This function allows the following kinds of bets to be made:

  Absolute bet
  ============

  bet 14k, bet 50m66, bet 100k

  Relative bet
  ============

  These bets are calculated relative to the current bet. The '+' symbol adds
  a certain number of percent to the current bet. The default is 25, so
  with a current bet of 1000, bet + gives 1250, bet +50 gives 1500 etc.
  Please note that the number must follow exactly after the +, without any
  spaces!

  The '*' or 'x' bet multiplies the current bet by the number specified,
  defaulting to 2. If the current bet is 1000, bet x  gives 2000, bet x10
  gives 10,000 etc.

  Lam 1.7.2000: zmiana dzialania + (stary + to %, obecny + dodaje do stawki)
*/
int parsebet( const int currentbet, char *s )
{
    /* check to make sure it's not blank */
    if ( s[ 0 ] != '\0' )
    {
	/* if first char is a digit, use advatoi */
	if ( isdigit( (unsigned char) s[ 0 ] ) )
	    return ( advatoi( s ) );

	/* add number (default 1000) */
	if ( s[ 0 ] == '+' )
	{
	    if ( s[ 1 ] == '\0' )
		return ( currentbet + 1000 );
	    return ( currentbet + advatoi( s + 1 ) );
	}

	/* multiply (default is by 2) */
	if ( s[ 0 ] == '*' || s[ 0 ] == 'x' )
	{
	    if (s[ 1 ] == '\0')
		return( currentbet * 2 );
	    return ( currentbet * atoi( s + 1 ) );
	}

	/* add percent (default 25%) */
	if ( s[ 0 ] == '%' )
	{
	    if ( s[ 1 ] == '\0' )
		return ( currentbet * 125 ) / 100;
	    return ( currentbet * ( 100 + atoi( s + 1 ) ) ) / 100;
	}
    }

    return 0;
}


KOMENDA( do_auction )
{
    OBJ_DATA *obj;
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];

    /* argument = */
    one_argument( argument, arg1 );

    /* NPC can be extracted at any time and thus can't auction! */
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->deaf, CHANNEL_AUCTION ) )
    {
	send_to_char( "Kana`l aukcji masz wy`l`aczony. W`l`acz go, aby m`oc bra`c udzia`l w licytacji.\n\r", ch );
	return;
    }

    if ( ( time_info.hour > 20 || time_info.hour < 8 ) && auction->item == NULL )
    {
	send_to_char ( "Aukcjoner poszed`l spa`c.\n\r", ch );
	return;
    }

    if ( arg1[ 0 ] == '\0' )
    {
	if ( auction->item != NULL )
	{
	    AFFECT_DATA *paf;
	    obj = auction->item;

	    /* show item data here */
	    if ( auction->bet > 0 )
		sprintf( buf, "Aktualna stawka to %s.\n\r", ZLOTO_M( auction->bet ) );
	    else
		sprintf( buf, "Nie otrzymano `zadnych zg`losze`n.\n\r" );
	    send_to_char( buf, ch );

	    sprintf( buf,
		"%s %s typu %s, dodatkowe w`la`sciwo`sci: %s.\n\rWa`zy %s, ma warto`s`c %d i poziom %d.\n\r",
		obj->short_descr,
		obj->rodzaj == 3 ? "s`a" : "jest",
		item_type_name_pl( obj ),
		extra_bit_name_pl_dla_ludzi( obj->extra_flags, ch ),
		slowny_ciezar( obj->weight ),
		obj->cost,
		obj->level );
	    buf[ 0 ] = UPPER( buf[ 0 ] );
	    send_to_char( buf, ch );

	    sprintf( buf, "U`zycie: %s\n\r",
		     wear_flag_name_pl( obj->wear_flags ) );
	    send_to_char( buf, ch );

	    switch ( obj->item_type )
	    {
		case ITEM_PILL:
		case ITEM_SCROLL:
		case ITEM_POTION:
		  sprintf( buf, "Czary %d poziomu:", obj->value[ 0 ].v );
		  send_to_char( buf, ch );

		  if ( obj->value[ 1 ].v >= 0 && obj->value[ 1 ].v < MAX_SKILL )
		  {
		     send_to_char( " \"", ch );
		     send_to_char( skill_table[ obj->value[ 1 ].v ].pl_name, ch );
		     send_to_char( "\"", ch );
		  }

		  if ( obj->value[ 2 ].v >= 0 && obj->value[ 2 ].v < MAX_SKILL )
		  {
		     send_to_char( " \"", ch );
		     send_to_char( skill_table[ obj->value[ 2 ].v ].pl_name, ch );
		     send_to_char( "\"", ch );
		  }

		  if ( obj->value[ 3 ].v >= 0 && obj->value[ 3 ].v < MAX_SKILL )
		  {
		     send_to_char( " \"", ch );
		     send_to_char( skill_table[ obj->value[ 3 ].v ].pl_name, ch );
		     send_to_char( "\"", ch );
		  }

		  send_to_char( ".\n\r", ch );
		  break;

		case ITEM_WAND:
		case ITEM_STAFF:
		  sprintf( buf, "Ma %d(%d) `ladunk`ow czaru %d poziomu:",
			obj->value[ 1 ].v, obj->value[ 2 ].v, obj->value[ 0 ].v );
		  send_to_char( buf, ch );

		  if ( obj->value[ 3 ].v >= 0 && obj->value[ 3 ].v < MAX_SKILL )
		  {
		     send_to_char( " \"", ch );
		     send_to_char( skill_table[ obj->value[ 3 ].v ].pl_name, ch );
		     send_to_char( "\"", ch );
		  }

		  send_to_char( ".\n\r", ch );
		  break;

		case ITEM_WEAPON:
		  sprintf( buf, "Zadaje obra`zenia od %d do %d (`srednio %d).\n\r",
			obj->value[ 1 ].v, obj->value[ 2 ].v,
			( obj->value[ 1 ].v + obj->value[ 2 ].v ) / 2 );
		  send_to_char( buf, ch );
		  break;

		case ITEM_ARMOR:
		  sprintf( buf, "Klasa pancerza: %d.\n\r", obj->value[ 0 ].v );
		  send_to_char( buf, ch );
		  break;
	    }

	    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
		if ( paf->location != APPLY_NONE && paf->modifier != 0 )
		{
		    sprintf( buf, "Wp`lywa na %s o %d.\n\r",
			    affect_loc_name_pl_b( paf->location ), paf->modifier );
		    send_to_char( buf, ch );
		}

	    for ( paf = obj->affected; paf; paf = paf->next )
		if ( paf->location != APPLY_NONE && paf->modifier != 0 )
		{
		    sprintf( buf, "Wp`lywa na %s o %d.\n\r",
			    affect_loc_name_pl_b( paf->location ), paf->modifier );
		    send_to_char( buf, ch );
		}

	    if ( ( obj->item_type == ITEM_CONTAINER ) && ( obj->contains ) )
	    {
		send_to_char( "Zawiera:\n\r", ch );
		show_list_to_char( obj->contains, ch, TRUE, TRUE, FALSE, FALSE, FALSE, 0, "" );
	    }

	    if ( IS_IMMORTAL( ch ) )
	    {
		sprintf( buf, "Sprzedawca: %s.  Kupuj`acy: %s.  Kolejka: %d.\n\r",
			auction->seller->name, auction->buyer->name,
			( auction->going + 1 ) );
		send_to_char( buf, ch );
		sprintf( buf, "Czas do ko`nca kolejki: %d.\n\r", auction->pulse );
		send_to_char( buf, ch );
	    }
	    return;
	}
	else
	{
	    send_to_char( "Nie toczy si`e `zadna aukcja.\n\r", ch );
	    return;
	}
    }

    if ( IS_IMMORTAL( ch ) && !str_cmp( arg1, "stop" ) )
    {
	if ( auction->item == NULL )
	{
	    send_to_char( "Nie ma aukcji do zatrzymania.\n\r", ch );
	    return;
	}
	else /* stop the auction */
	{
	    sprintf( buf,"Aukcja %s zosta`la przerwana przez nie`smiertelnego.",
			    auction->item->dopelniacz );
	    talk_auction( buf );
	    obj_to_char( auction->item, auction->seller );
	    auction->item = NULL;
	    auction->seller->gold += auction->zastaw;
	    send_to_char( "Pieni`adze z zastawu powracaj`a do ciebie.\n\r", auction->seller );
	    if ( auction->buyer != NULL && auction->buyer != auction->seller ) /* return money to the buyer */
	    {
		auction->buyer->gold += auction->bet;
		send_to_char( "Pieni`adze powracaj`a do ciebie.\n\r", auction->buyer );
	    }
	    return;
	}
    }

/*
    if ( !str_cmp( arg1, "bet" ) )
*/
	if ( auction->item )
	{
	    int newbet;

	    if ( ch == auction->seller )
	    {
		send_to_char( "Nie mo`zesz kupi`c w`lasnego przedmiotu.\n\r", ch );
		return;
	    }

	    /* make - perhaps - a bet now */
	    if ( argument[ 0 ] == '\0' )
	    {
		send_to_char( "Jak du`zo chcesz zap`laci`c?\n\r", ch );
		return;
	    }

	    newbet = parsebet( auction->bet, argument );

	    if ( newbet < auction->starting )
	    {
		send_to_char( "Musisz przebi`c warto`s`c pocz`atkow`a.\n\r", ch );
		return;
	    }

	    if ( newbet < ( auction->bet + 100 ) )
	    {
		send_to_char( "Musisz do`lo`zy`c minimum 100 monet.\n\r", ch );
		return;
	    }

/*            if ( newbet > ch->gold )
		teraz potencjalny nabywca moze postawic tyle kasy ile ma, a tak
		nie mogl, bo w rzeczywistosci nie mial kasy, a miala ja aukcja */
	    if ( newbet > ( ch->gold + ( ( auction->buyer == ch ) ? auction->bet : 0 ) ) )
	    {
		send_to_char( "Nie masz takiej ilo`sci z`lota!\n\r", ch );
		return;
	    }

	    if ( newbet > 1000000 )
	    {
		send_to_char( "Nie mo`zesz zap`laci`c wi`ecej ni`z milion.\n\r", ch );
		return;
	    }

	    /* the actual bet is OK! */

	    /* return the gold to the last buyer, if one exists */
	    if ( auction->buyer != NULL && auction->buyer != auction->seller )
		auction->buyer->gold += auction->bet;

	    ch->gold -= newbet; /* substract the gold - important :) */
	    auction->buyer = ch;
	    auction->bet   = newbet;
	    auction->going = 0;
	    auction->pulse = PULSE_AUCTION; /* start the auction over again */

	    sprintf( buf, "Zg`loszono %d monet za %s.", newbet, auction->item->biernik );
	    talk_auction( buf );
	    return;
	}
/*
	else
	{
	    send_to_char( "Nie toczy si`e `zadna aukcja.\n\r", ch );
	    return;
	}
*/
/* finally... */

    if ( pijane_przedmioty( ch ) )
	return;

    argument = one_argument( argument, arg1 );

    obj = get_obj_carry( ch, arg1 ); /* does char have the item ? */

    if ( !obj )
    {
	send_to_char ( "Nie masz tego przy sobie.\n\r", ch );
	return;
    }

    if ( obj->timer > 0 )
    {
	send_to_char( "Ten przedmiot wkr`otce zniknie i nie mo`zesz go aukcjonowa`c.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "Nie mo`zesz si`e tego pozby`c.\n\r", ch );
	return;
    }

    one_argument( argument, arg2 );

    if ( arg2[ 0 ] == '\0' )
	strcpy( arg2, "1" );

    if ( advatoi( arg2 ) == 0 )
    {
	send_to_char( "Musisz poda`c liczb`e okre`slaj`ac`a minimaln`a stawk`e.\n\r", ch );
	return;
    }

    if ( advatoi( arg2 ) < 0 )
    {
	ASTC( "Widzia`l$o`s kiedy`s sprzeda`z czego`s za ujemn`a ilo`s`c z`lota?", ch );
	return;
    }

    if ( ( ( advatoi( arg2 ) + obj->cost ) / 10 ) + ( 20 * ch->level )
	> ch->gold )
    {
       send_to_char( "Nie b`edzie ci`e sta`c na pokrycie koszt`ow aukcji.\n\r", ch );
       return;
    }

    if ( auction->item == NULL )
    switch ( obj->item_type )
    {
    default:
	send_to_char( "Takiego typu przedmiot`ow nie mo`zna aukcjonowa`c.\n\r", ch );
	return;

    case ITEM_LIGHT:
    case ITEM_TREASURE:
    case ITEM_POTION:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_FOOD:
    case ITEM_BOAT:
    case ITEM_PILL:
    case ITEM_WEAPON:
    case ITEM_ARMOR:
    case ITEM_STAFF:
    case ITEM_WAND:
    case ITEM_SCROLL:
    case ITEM_SPELLBOOK:
    case ITEM_SHOVEL:
	obj_from_char( obj );
	auction->item = obj;
	auction->bet = 0;
	auction->buyer = ch;
	auction->seller = ch;
	auction->pulse = PULSE_AUCTION;
	auction->going = 0;
	auction->starting = advatoi( arg2 );
	auction->zastaw = ( auction->starting + auction->item->cost ) / 10
			+ 20 * auction->seller->level;
	auction->seller->gold -= auction->zastaw;

	if ( auction->starting > 0 )
	  auction->bet = auction->starting;

	sprintf( buf, "Aukcjoner pobiera zastaw w wysoko`sci %d.\n\r", auction->zastaw );
	send_to_char( buf, auction->seller);
	sprintf( buf, "Nowa aukcja: %s, cena wywo`lawcza: %s.", obj->short_descr, ZLOTO_M( auction->starting ) );
	talk_auction( buf );

	return;
    } /* switch */
    else
    {
	act( ZM_ZAWSZE, "Spr`obuj p`o`xniej - na razie toczy si`e aukcja $g!", ch, auction->item, NULL, TO_CHAR );
	WAIT_STATE( ch, PULSE_VIOLENCE * 150 / 100 );
	return;
    }

    return;
}


/*
 * Lam 29.5.98 na wzor ROM-a (zazadane przez RenTora)
 */
#define SPRAWDZ_INWENTARZ						\
	if ( ch->carry_number >= can_carry_n( ch )			\
	  && ch->carry_weight >= can_carry_w( ch ) )			\
	{								\
	    STC( "Wi`ecej przedmiot`ow nie uniesiesz.\n\r", ch );	\
	    return;							\
	}

KOMENDA( do_outfit )
{
    OBJ_DATA *obj;
    bool dostal = FALSE;

    if ( IS_NPC( ch ) || ch->level > 5 )
    {
	ASTC( "Sam$a znajd`x sobie ubranie, jeste`s wystarczaj`aco do`swiadczon$y.", ch );
	return;
    }

    /*
    SPRAWDZ_INWENTARZ

    if ( !( IS_SET( race_table[ ch->race ].wearlocs, WRL_TRZYMANE )
	 || get_eq_char( ch, WEAR_LIGHT ) ) )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_BANNER ), 0 );
	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_LIGHT );
	dostal = TRUE;
    }
    */

    SPRAWDZ_INWENTARZ

    if ( !( IS_SET( race_table[ ch->race ].wearlocs, WRL_CIALO )
	 || get_eq_char( ch, WEAR_BODY ) ) )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_VEST ), 0 );
	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_BODY );
	dostal = TRUE;
    }

    SPRAWDZ_INWENTARZ

    if ( !( IS_SET( race_table[ ch->race ].wearlocs, WRL_TARCZA )
	 || get_eq_char( ch, WEAR_SHIELD ) ) )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_SCHOOL_SHIELD ), 0 );
	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_SHIELD );
	dostal = TRUE;
    }

    SPRAWDZ_INWENTARZ

    if ( !( IS_SET( race_table[ ch->race ].wearlocs, WRL_BRON )
	 || get_eq_char( ch, WEAR_WIELD ) ) )
    {
	obj = create_object( get_obj_index( *class_table[ ch->klasa ].weapon ), 0 );
	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_WIELD );
	dostal = TRUE;
    }

    if ( dostal )
	send_to_char( "Bogowie wyposa`zyli ci`e w podstawowy ekwipunek.\n\r", ch );
    else
	send_to_char( "Bogowie nie reaguj`a na twoje pro`sby o wi`eksz`a ilo`s`c ekwipunku.\n\r", ch );

    return;
}


#define WARUNKI( ch ) ( ( ch->in_room && IS_OUTSIDE( ch ) \
		       && ch->in_room->sector_type != SECT_UNDERWATER ) \
		      ? ch->in_room->area->weather_info.sky : 0 )
KOMENDA( do_ognisko )
{
    AFFECT_DATA  af;
    OBJ_DATA    *obj;
    OBJ_DATA    *obj_next;
    int          ilosc = 0;

    if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_TRZYMANE ) )
    {
	STC( "Nie mo`zesz rozpala`c ognisk.\n\r", ch );
	return;
    }

    if ( !moze_uzyc( ch, gsn_rozpalanie_ognisk ) )
    {
	send_to_char( "Nie potrafisz rozpali`c ogniska.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( room_is_affected( ch->in_room, gsn_rozpalanie_ognisk ) )
    {
	send_to_char( "Nie ma co, ognisko ju`z p`lonie.\n\r", ch );
	return;
    }

    for ( obj = ch->in_room->contents; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted )
	    continue;

	if ( obj->pIndexData->vnum == OBJ_VNUM_CHRUST )
	{
	    ilosc += obj->value[ 0 ].v;
	    extract_obj( obj );
	}
    }

    if ( !ilosc )
    {
	send_to_char( "Nie masz chrustu!\n\r", ch );
	return;
    }

    if ( CZY_WODA( ch->in_room ) )
    {
	send_to_char( "Robisz co mo`zesz, ale woda ca`ly czas rozmywa twoj`a konstrukcj`e.\n\r", ch );
	return;
    }

    if ( CZY_POWIETRZE( ch->in_room ) )
    {
	send_to_char( "Robisz co mo`zesz, ale nie mo`zesz u`lo`zy`c chrustu w miejscu.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_rozpalanie_ognisk ].beats );

    if ( URANGE( 1, ( 1 - WARUNKI( ch ) ) * 20 + ilosc
		      + ( IS_NPC( ch ) ? 50 : ch->pcdata->learned[ gsn_rozpalanie_ognisk ] ),
		 95 ) <= number_percent( ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	act( ZM_WZROK, "$n pr`obuje rozpali`c ognisko, ale marnuje ca`ly chrust.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    send_to_char( "Rozpalasz ognisko.\n\r", ch );
    act( ZM_WZROK, "$n rozpala ognisko.", ch, NULL, NULL, TO_ROOM );
    af.type      = gsn_rozpalanie_ognisk;
    af.duration  = UMIN( ilosc + ( ch->level > 80 ? 1 : 0 ), 11 );
    CLEAR_BITS( af.bitvector );
    if ( !IS_NPC( ch ) && ch->pcdata->learned[ gsn_rozpalanie_ognisk ] )
	uzyj( ch, gsn_rozpalanie_ognisk, "rozpalaniu ognisk" );
    if ( !IS_SET( ch->in_room->room_flags, ROOM_CLAN_HP ) )
	SET_BIT( af.bitvector[ 0 ], ROOM_CLAN_HP );
    if ( !IS_SET( ch->in_room->room_flags, ROOM_REFRESH ) )
	SET_BIT( af.bitvector[ 0 ], ROOM_REFRESH );
    /* af.level i af.caster nieuzywane w pomieszczeniach */

    affect_to_room( ch->in_room, &af );

    /* moglismy zapalic swiatlo w pomieszczeniu */
    sprawdz_przyzwyczajanie( ch->in_room );

    return;
}


KOMENDA( do_chrust )
{
    OBJ_DATA *obj;
    int       szansa = 0;
    int       ruch;

    ruch = movement_loss[ UMIN( SECT_MAX-1, ch->in_room->sector_type ) ]
	 + ch->in_room->strata_ruchu;

    if ( ch->move < ruch )
    {
	if ( is_affected( ch, gsn_search ) )
	    affect_strip( ch, gsn_search );

	ASTC( "Jeste`s zbyt wycie`nczon$y.", ch );
	    return;
    }
    else
	ch->move -= ruch;

    PRZERWIJ_KOPANIE( ch, TRUE );

    if ( pijane_przedmioty( ch ) )
	return;

    /* Lam 28.1.2006, #816 */
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	if ( obj->pIndexData->vnum == OBJ_VNUM_CHRUST )
	{
	    get_obj( ch, obj, NULL, FALSE );
	    return;
	}

    switch ( ch->in_room->sector_type )
    {
	default:
	    szansa = -10;
	    break;

	case SECT_FIELD:
	case SECT_HILLS:
	    szansa += 30;
	    break;

	case SECT_FOREST:
	case SECT_MOUNTAIN:
	    szansa += 60;
	    break;
    }

    switch ( ch->in_room->area->weather_info.sky )
    {
	case SKY_CLOUDLESS:
	    szansa += 10;
	    break;

	case SKY_LIGHTNING:
	    szansa -= 10;
	    break;

	default:
	    break;
    }

    if ( !can_see_room( ch, ch->in_room ) )
	szansa = szansa * szansa / 200;

    if ( URANGE( 0, szansa, 95 ) > number_percent( ) )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_CHRUST ), 0 );
	obj_to_room( obj, ch->in_room );
	get_obj( ch, obj, NULL, FALSE );
    }
    else
	send_to_char( "Niczego nie znajdujesz.\n\r", ch );

    return;
}


void list_osobiste( CHAR_DATA *keeper, CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    char      buf1[ MAX_INPUT_LENGTH ];
    char      buf2[ MAX_STRING_LENGTH ];
    bool      jest = FALSE;

    buf2[ 0 ] = '\0';

    strcat( buf2, "{Y[{MPoz{x    {RCena{x    {CPZ{Y]{x {yPrzedmiot{x\n\r" );
    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( IS_OBJ_STAT( obj, ITEM_PRIVATE )
	  && str_cmp( ch->name, obj->owner )
	  && can_see_obj( ch, obj ) )
	{
	    if ( can_see_obj( keeper, obj ) )
		sprintf( buf1, "{Y[{m%3d{r%8d{x    {c%2d{Y]{x %s\n\r",
			 obj->level,
			 WYDATEK( ( ch->level + obj->level ) / 2 + 1, 10 ),
			 obj->level / 10 + 1,
			 obj->short_descr );
	    else
		sprintf( buf1, "{Y[{x --------------- {Y]{x %s (%s nie widzi tego)\n\r", obj->short_descr, PERS( keeper, ch, FALSE ) );
	    strcat( buf2, buf1 );
	    jest = TRUE;
	}

    if ( !jest )
	send_to_char( "Nie masz niczego do dostosowania.\n\r", ch );
    else
	send_to_char( buf2, ch );

    return;
}


CHAR_DATA *find_warsztat( CHAR_DATA *ch )
{
    CHAR_DATA *keeper;
    char       buf[ MAX_STRING_LENGTH ];

    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
	if ( ch != keeper
	  && IS_NPC( keeper )
	  && ( keeper->pIndexData->vnum == MOB_VNUM_WARSZTAT )
	  && IS_AWAKE( keeper )
	  && !IS_AFFECTED( keeper, AFF_CHARM ) )
	    break;

    if ( !keeper )
    {
	send_to_char( "Nie ma tu obs`lugi warsztatu.\n\r", ch );
	return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze ci`e zobaczy`c.", keeper, NULL, ch, TO_VICT );
	return NULL;
    }

    /*
     * Undesirables.
     */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_KILLER ) )
    {
	do_say( keeper, "Nie lubimy tu morderc`ow!" );
	sprintf( buf, "%s MORDERCA jest tu, pomocy!", ch->name );
	do_yell( keeper, buf );
	return NULL;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_THIEF ) )
    {
	do_say( keeper, "Nie lubimy tu z`lodziei!" );
	sprintf( buf, "%s Z`LODZIEJ jest tu, pomocy!", ch->name );
	do_yell( keeper, buf );
	return NULL;
    }

    return keeper;
}


KOMENDA( do_dostosuj )
{
    OBJ_DATA  *obj;
    CHAR_DATA *keeper;
    char       buf[ MAX_STRING_LENGTH ];
    char       arg[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    int        cost;
    int        pz;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( !( keeper = find_warsztat( ch ) ) )
	return;

    if ( arg[ 0 ] == '\0' )
    {
	list_osobiste( keeper, ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	act( ZM_ZAWSZE, "Nie masz tego.", keeper, NULL, ch, TO_VICT );
	return;
    }

    if ( !can_see_obj( keeper, obj ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze tego dostrzec.", keeper, NULL, ch, TO_VICT );
	return;
    }

    if ( ( !IS_OBJ_STAT( obj, ITEM_INVERT_RACES )
	    && is_name2( race_table[ ch->race ].ang, obj->anti_race ) )
	  || ( IS_OBJ_STAT( obj, ITEM_INVERT_RACES )
	    && !is_name2( race_table[ ch->race ].ang, obj->anti_race ) )
	  || ( !IS_NPC( ch ) && !IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	    && is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) )
	  || ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	    && !is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) ) )
    {
	act( ZM_ZAWSZE, "Tego przedmiotu nie da si`e do ciebie dostosowa`c.",
	    keeper, NULL, ch, TO_VICT );
	return;
    }

    if ( !IS_OBJ_STAT( obj, ITEM_PRIVATE ) || !str_cmp( ch->name, obj->owner ) )
    {
	act( ZM_ZAWSZE, "Nie potrzebujesz tego dostosowywa`c.", keeper, NULL, ch, TO_VICT );
	return;
    }

    if ( obj->timer >= 0
	|| obj->level > ch->level
	|| obj->item_type == ITEM_KEY
	|| obj->item_type == ITEM_TRASH )
    {
	send_to_char( "Nie op`laca ci si`e to.\n\r", ch );
	return;
    }

    cost = WYDATEK( ( ch->level + obj->level ) / 2 + 1, 10 );
    pz = obj->level / 10 + 1;

    if ( cost > ch->gold )
    {
	STC( "Nie posiadasz wystarczaj`acej ilo`sci z`lota.\n\r", ch );
	return;
    }

    if ( pz > ch->qp )
    {
	STC( "Nie posiadasz wystarczaj`acej liczby punkt`ow zada`n.\n\r", ch );
	return;
    }

    sprintf( buf, "Dostosowujesz $j do siebie za %s i %d punkt%s zada`n.",
	    ZLOTO_B( cost ),
	    pz, pz == 1 ? "" : liczba( pz ) == 2 ? "y" : "`ow" );
    act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_OBJ1, "$n dostosowuje $j do siebie.", ch, obj, NULL, TO_ROOM );
    ch->gold     -= cost;
    ch->qp       -= pz;

    free_string( obj->owner );
    obj->owner = str_dup( ch->name );
    SET_BIT_V( obj->extra_flags, ITEM_PRIVATE );

    return;
}


KOMENDA( do_sklep )
{
    char arg[ MIL ];
    char buf[ MSL ];
    char buf1[ MSL * 4 ];
    OBJ_DATA *obj;
    int cena;
    bool bylo;

    if ( IS_NPC( ch ) )
    {
	STC( "Moby prowadz`a sklepy w inny spos`ob ni`z gracze.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !*arg )
    {
	if ( !IS_SET( ch->act, PLR_SHOPKEEPER ) )
	{
	    STC( "Nie prowadzisz sklepu. Przeczytaj \"pomoc sklep\", je`sli chcesz go otworzy`c.\n\r", ch );
	    return;
	}
	if ( IS_SET( ch->act, PLR_SHOP_BREAK ) )
	    strcpy( buf1, "Prowadzisz sklep, kt`ory aktualnie ma przerw`e.\n\r" );
	else
	    strcpy( buf1, "Prowadzisz sklep, kt`ory aktualnie jest czynny.\n\r" );

	bylo = FALSE;
	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
	    if ( !IS_OBJ_STAT( obj, ITEM_PLAYER_SHOP )
	      || obj->wear_loc != WEAR_NONE
	      || !can_see_obj( ch, obj ) )
		continue;

	    if ( !bylo )
	    {
		bylo = TRUE;
		strcat( buf1, "Asortyment sklepu:\n\r" );
		strcat( buf1, "{Y[{MPzm{R    Cena{Y]{y Rzecz{x\n\r" );
	    }

	    sprintf( buf, "{Y[{m%3d {r%7d{Y]{x %s\n\r",
			obj->level, obj->plr_shop_cost, obj->short_descr );
	    if ( strlen( buf1 ) + strlen( buf ) > sizeof( buf1 ) - MIL )
	    {
		strcat( buf1, "(Za du`zo przedmiot`ow, by wy`swietli`c wszystkie!)\n\r" );
		break;
	    }
	    strcat( buf1, buf );
	}

	if ( !bylo )
	    strcat( buf1, "W twoim sklepie nie ma aktualnie `zadnego towaru.\n\r" );

	STC( buf1, ch );
	return;
    }

    if ( !str_prefix( arg, "otw`orz" ) )
    {
	if ( IS_SET( ch->act, PLR_SHOPKEEPER )
	  && !IS_SET( ch->act, PLR_SHOP_BREAK ) )
	{
	    STC( "Ju`z prowadzisz sklep.\n\r", ch );
	    return;
	}
	SET_BIT( ch->act, PLR_SHOPKEEPER );
	REMOVE_BIT( ch->act, PLR_SHOP_BREAK );
	STC( "Otwierasz sklep.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "zamknij" ) )
    {
	if ( !IS_SET( ch->act, PLR_SHOPKEEPER ) )
	{
	    STC( "Aby zamkn`a`c sklep, musisz go najpierw otworzy`c.\n\r", ch );
	    return;
	}
	REMOVE_BIT( ch->act, PLR_SHOPKEEPER );
	REMOVE_BIT( ch->act, PLR_SHOP_BREAK );
	STC( "Zamykasz sklep.\n\r", ch );

	for ( obj = ch->carrying; obj; obj = obj->next_content )
	    if ( IS_OBJ_STAT( obj, ITEM_PLAYER_SHOP ) )
	    {
		REMOVE_BIT_V( obj->extra_flags, ITEM_PLAYER_SHOP );
		obj->plr_shop_cost = 0;
	    }

	return;
    }

    if ( !str_prefix( arg, "przerwa" ) )
    {
	if ( !IS_SET( ch->act, PLR_SHOPKEEPER ) )
	{
	    STC( "Nie prowadzisz sklepu.\n\r", ch );
	    return;
	}
	if ( IS_SET( ch->act, PLR_SHOP_BREAK ) )
	{
	    STC( "Przerwa ju`z trwa. Aby j`a zako`nczy`c, otw`orz sklep.\n\r", ch );
	    return;
	}
	SET_BIT( ch->act, PLR_SHOP_BREAK );
	STC( "Robisz sobie przerw`e od prowadzenia sklepu.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "dodaj" ) )
    {
	argument = one_argument( argument, arg );
	if ( !*arg || !*argument || !is_number( argument ) )
	{
	    STC( "Sk`ladnia: sklep dodaj <przedmiot> <cena>\n\r", ch );
	    return;
	}

	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
	    STC( "Nie masz tego przy sobie.\n\r", ch );
	    return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_PLAYER_SHOP ) )
	{
	    STC( "Ten przedmiot jest ju`z w asortymencie sklepu.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    STC( "Nie mo`zesz si`e tego pozby`c.\n\r", ch );
	    return;
	}

	cena = atoi( argument );

	if ( cena < 1 || cena > 1000000 )
	{
	    STC( "Musisz poda`c rozs`adn`a cen`e.\n\r", ch );
	    return;
	}

	SET_BIT_V( obj->extra_flags, ITEM_PLAYER_SHOP );
	obj->plr_shop_cost = cena;
	act_pp( ZM_ZAWSZE, "Dodajesz $j do asortymentu sklepu.", ch, obj, NULL, TO_CHAR );
	return;
    }

    if ( !str_prefix( arg, "usu`n" ) )
    {
	int number, count;

	number = number_argument( argument, arg );
	count = 0;
	for ( obj = ch->carrying; obj; obj = obj->next_content )
	    if ( !obj->deleted
	      && IS_OBJ_STAT( obj, ITEM_PLAYER_SHOP )
	      && obj->wear_loc == WEAR_NONE
	      && can_see_obj( ch, obj )
	      && is_name( arg, obj->name ) )
	    {
		if ( ++count == number )
		    break;
	    }

	if ( !obj )
	{
	    STC( "Nie masz takiego przedmiotu w asortymencie sklepu.\n\r", ch );
	    return;
	}

	REMOVE_BIT_V( obj->extra_flags, ITEM_PLAYER_SHOP );
	obj->plr_shop_cost = 0;
	act_pp( ZM_ZAWSZE, "Usuwasz $j z asortymentu sklepu.", ch, obj, NULL, TO_CHAR );
	return;
    }

    STC( "B`l`ad sk`ladni. Napisz \"pomoc sklep\", aby si`e z ni`a zapozna`c.\n\r", ch );

    return;
}


void check_wield( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( !IS_NPC( ch ) )
	return;

    if ( !IS_SET( race_table[ ch->race ].race_abilities, RACE_WEAPON_WIELD ) )
	return;

    if ( !get_eq_char( ch, WEAR_WIELD )
      || !( get_eq_char( ch, WEAR_WIELD_2 )
	 || get_eq_char( ch, WEAR_SHIELD )
	 || get_eq_char( ch, WEAR_HOLD ) ) )
    {
	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
	    if ( obj->deleted
	      || obj->wear_loc != WEAR_NONE
	      || IS_OBJ_STAT( obj, ITEM_INVENTORY )
	      || !can_see_obj( ch, obj ) )
		continue;

	    if ( obj->item_type == ITEM_WEAPON && obj->level <= ch->level )
		break;
	}

	if ( obj )
	    wear_obj( ch, obj, FALSE, FALSE );
    }

    return;
}


void zmien_brzeczenie( CHAR_DATA *ch, bool fOn, bool fCiche )
{
    if ( !ch->brzeczenie && !fOn )
    {
	bug( "zmien_brzeczenie: proba usuniecia brzeczenia z niebrzeczacej postaci.", 0 );
	return;
    }

    ch->brzeczenie += ( fOn ) ? 1 : -1;

    if ( fOn )
    {
	if ( ch->brzeczenie == 1 )
	{
	    SET_BIT_V( ch->affected_by, AFF_BRZECZY );

	    if ( !fCiche
	      && ch->in_room
	      && IS_AWAKE( ch )
	      && !IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE )
	      && !PP( ch ) )
	    {
		STC( "Zaczynasz pobrz`ekiwa`c.\n\r", ch );
		act( ZM_SLUCH, "$n zaczyna pobrz`ekiwa`c.", ch, NULL, NULL, TO_ROOM );
	    }
	}
    }
    else
    {
	if ( ch->brzeczenie == 0 )
	{
	    if ( !fCiche
	      && ch->in_room
	      && IS_AWAKE( ch )
	      && !IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE )
	      && !PP( ch ) )
	    {
		STC( "Przestajesz pobrz`ekiwa`c.\n\r", ch );
		act( ZM_SLUCH, "$n przestaje pobrz`ekiwa`c.", ch, NULL, NULL, TO_ROOM );
	    }
	    REMOVE_BIT_V( ch->affected_by, AFF_BRZECZY );
	}
    }

    return;
}
