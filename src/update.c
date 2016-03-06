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
#include "update.h"
#include "mp_pol.h"
#include <math.h>

/*
 * Externals
 */
extern int obr_z_upadku[ ];

/*
 * Globals
 */
bool	delete_obj;
bool	delete_char;
bool	murder_char;

/*
 * Local functions.
 */
int     hit_gain		args( ( CHAR_DATA *ch ) );
int     mana_gain		args( ( CHAR_DATA *ch ) );
int     move_gain		args( ( CHAR_DATA *ch ) );
void    mobile_update		args( ( void ) );
void    weather_update		args( ( AREA_DATA *area ) );
void    char_update		args( ( void ) );
void    obj_update		args( ( void ) );
void	room_update		args( ( void ) );
void    aggr_update		args( ( void ) );
void    murders_update		args( ( void ) );
void	lv_hlp			args( ( CHAR_DATA *ch ) );
void	pijanstwo		args( ( CHAR_DATA *ch ) );
void	wzbudzenie_strachu	args( ( CHAR_DATA *ch ) );
void	plynne_odradzanie	args( ( void ) );
void	rrand_update		args( ( void ) );
void	mtrig_update		args( ( void ) );
void	licz_graczy2		args( ( int *play, int *desc ) );
void	stats_update		args( ( void ) );
void	time_update		args( ( void ) );
void	list_update		args( ( void ) );
void	podpowiadaj		args( ( void ) );
void	airobj_update		args( ( void ) );
void	airchar_update		args( ( void ) );
void	sentinel_update		args( ( void ) );

/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch )
{
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;
    int add_gold;

    add_hp	= con_app[ get_curr_con( ch ) ].hitp + number_range(
		    class_table[ ch->klasa ].hp_min,
		    class_table[ ch->klasa ].hp_max );
    add_mana	= class_table[ ch->klasa ].fMana
	? number_range( int_app[ get_curr_int( ch ) ].mana,
		( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 7 )
	: 0;
    add_move	=
	number_range( 4, ( get_curr_con( ch ) + get_curr_dex( ch ) + 4 ) / 4 );
    add_prac	= wis_app[ get_curr_wis( ch ) ].practice;
    add_gold	= 2 * WYDATEK( ch->level, 20 ); /* 2 cwiczenia */

    add_gold		-= add_gold % 100; /* zeby wygladalo bardziej okraglo */

    add_hp		 = UMAX(    1, add_hp	);
    add_mana		 = UMAX(    0, add_mana );
    add_move		 = UMAX(    4, add_move );
    add_gold		 = UMAX( 1000, add_gold );

    if ( ch->hit == ch->max_hit )
	ch->hit		+= add_hp;
    ch->max_hit		+= add_hp;
    if ( ch->mana == ch->max_mana )
	ch->mana	+= add_mana;
    ch->max_mana	+= add_mana;
    if ( ch->move == ch->max_move )
	ch->move	+= add_move;
    ch->max_move	+= add_move;
    ch->practice	+= add_prac;
    ch->gold		+= add_gold;

    if ( !IS_NPC( ch ) )
	REMOVE_BIT( ch->act, PLR_BOUGHT_PET );

    ch_printf( ch, "Zyskujesz: %d/%d `z, %d/%d m, %d/%d r, %d/%d `cw.\n\r",
		add_hp,		ch->max_hit,
		add_mana,	ch->max_mana,
		add_move,	ch->max_move,
		add_prac,	ch->practice );
    ch_printf( ch, "Z tej okazji bogowie zsy`laj`a ci w prezencie %s.\n\r",
		ZLOTO_B( add_gold ) );

    return;
}


/*
 * Demote stuff.
 */
void demote_level( CHAR_DATA *ch )
{
    char buf[ MAX_STRING_LENGTH ];
    int  add_hp;
    int  add_mana;
    int  add_move;
    int  add_prac;

    if ( ch->level == 1 )
	return;

    /* Lam 26.10.2004: zmiana get_curr_*( ch ) na 26 i zmaksymalizowanie */
    add_hp      = con_app[ 26 ].hitp + class_table[ ch->klasa ].hp_max;
    add_mana    = class_table[ ch->klasa ].fMana ? 11 : 0;
    add_move    = 14;
    add_prac    = wis_app[ 26 ].practice;

    ch->max_hit         -= add_hp;
    ch->max_mana        -= add_mana;
    ch->max_move        -= add_move;
    ch->practice        -= add_prac;

    if ( !IS_NPC( ch ) )
	REMOVE_BIT( ch->act, PLR_BOUGHT_PET );

    ch->level -= 1;
    sprintf( buf,
	    "Tracisz: %d/%d `z, %d/%d m, %d/%d r, %d/%d `cw.\n\r",
	    add_hp,     ch->max_hit,
	    add_mana,   ch->max_mana,
	    add_move,   ch->max_move,
	    add_prac,   ch->practice );
    send_to_char( buf, ch );

    return;
}


/*
 * Pomoc po zrobieniu poziomu: Pomysl Albi 22.4.98, wykonanie Lam 22.4.98
 * pierwsze pomoce poziomowo-klasowe autorstwa Albiego (22.4.98)
 * Lam 5.7.2006 (na wniosek Miri): dla wyzszego zaufania nie pokazuja sie.
 * Dla zaufania rownego (np. restart na 5 poziomie i zrobienie 5 ponownie)
 * przyjmuje, ze gracz nie mial dosc okazji zrozumiec tresci tej pomocy ;)
 */
void lv_hlp( CHAR_DATA *ch )
{
    HELP_DATA *hlp;
    char       buf[ MAX_INPUT_LENGTH ];

    /* Lam 5.7.2006 */
    if ( get_trust( ch ) > ch->level )
	return;

/* Najpierw ogolna pomoc dla poziomu (niezaleznie od klasy). */
    sprintf( buf, "#POZIOM%03d", ch->level ); /* #POZIOM002 - #POZIOM110 */

    for ( hlp = help_first; hlp; hlp = hlp->next )
    {
	if ( hlp->level > get_trust( ch )
	  || str_cmp( hlp->keyword, buf ) )
	    continue;

	if ( *hlp->text == '.' )
	    send_to_char( hlp->text + 1, ch );
	else
	    send_to_char( hlp->text, ch );
	break; /* TYLKO JEDNA POMOC DLA 1 POZIOMU */
    }

/* Teraz pomoc dla poziomu danej klasy. */
    sprintf( buf, "#POZIOM%s%03d", class_table[ ch->klasa ].who_name, ch->level );
    /* #POZIOMz`lo002, #POZIOMszp101 itp. */

    for ( hlp = help_first; hlp; hlp = hlp->next )
    {
	if ( hlp->level > get_trust( ch )
	  || str_cmp( hlp->keyword, buf ) )
	    continue;

	if ( *hlp->text == '.' )
	    send_to_char( hlp->text + 1, ch );
	else
	    send_to_char( hlp->text, ch );
	break; /* jw. */
    }

    return;
}


void gain_exp( CHAR_DATA *ch, int gain )
{
    char buf[ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch )
      || ( ch->level >= LEVEL_HERO && gain > -1000 )
      || ch->level < 0 )
	return;

    ch->exp = UMAX( EXP_PER_LEVEL, ch->exp + gain );
    while ( ch->level < LEVEL_HERO
	 && ch->exp >= EXP_PER_LEVEL * ( ch->level + 1 ) )
    {
	sprintf( buf, "{M$@{x uda`lo si`e osi`agn`a`c poziom {Y%d{x!", ch->level + 1 );
	wiznet( buf, ch, NULL, NULL, WIZ_LEVEL, 0, 101 );
	sprintf( buf, "%s osiaga poziom %d", ch->name, ch->level + 1 );
	log_string( buf );
	send_to_char( "{GGratuluj`e! UDA`LO CI SI`E ZDOBY`C POZIOM!{x\n\r", ch );
	ch->level++;
	advance_level( ch );
	lv_hlp( ch );

	/* Lam 26.10.2008: #2445: opc +czas dopiero od 3 */
	if ( ch->level == 3 && !IS_SET( ch->pcdata->opcje, OPC_SHOWTICK ) )
	{
	    SET_BIT( ch->pcdata->opcje, OPC_SHOWTICK );
	    STC( "{YAutomatycznie w`l`aczy`lo ci si`e powiadamianie o up`lywie czasu. Je`sli ci\n\r"
		"przeszkadza, mo`zesz je wy`l`aczy`c pisz`ac: {Gopcje -czas{x\n\r\n\r", ch );
	}

	/* Automatyczny awans w zaleznosci od ilosci ukonczonych profesji. */
	if ( ch->level == LEVEL_HERO
	  && !IS_SET( ch->act, PLR_KILLER ) )
	{
	    int poziomy[ MAX_CLASS ];
	    int i, ile;

	    wypelnij_poziomy( ch, poziomy );
	    ile = 0;
	    for ( i = 0; i < MAX_CLASS; i++ )
		if ( poziomy[ i ] >= 100 )
		    ile++;

	    while ( ch->level < LEVEL_HERO + 2
		 && ch->level < LEVEL_HERO + ile - 1 )
	    {
		sprintf( buf, "{M$@{x uda`lo si`e osi`agn`a`c poziom {Y%d{x!", ch->level + 1 );
		wiznet( buf, ch, NULL, NULL, WIZ_LEVEL, 0, 101 );
		sprintf( buf, "%s osiaga poziom %d", ch->name, ch->level + 1 );
		log_string( buf );
		ch->level++;
		ch->exp = ch->level * EXP_PER_LEVEL;
		advance_level( ch );
		lv_hlp( ch );
	    }
	    if ( ile > 3 )
		ch->trust = UMAX( ch->level, ch->trust );
	}
    }

    return;
}


void uzyj( CHAR_DATA *ch, int sn, const char *co )
{
    char  buf[ MAX_STRING_LENGTH ];
    int   prog;

    if ( IS_NPC( ch ) )
	return;

    if ( ch->pcdata->learned[ sn ] >= 100 )
	return;

    ++ch->pcdata->used[ sn ];

    /* Ulryk: ponizszy wzor interpretowac nalezy jako:
       x% * to_use * 20 / ( get_curr_int( ch ) - 2 );
       Koncowka tego wzoru jest stara jak swiat, ja dodalem jedynie x%, ktore
       zalezy od learned[ sn ] i waha sie od 15% (przy wyuczeniu 1%) do ~230%
       (przy 100%). Zaleznosc x od wyuczenia jest kwadratowa, wiec to nie
       balansuje sredniej liczby prob potrzebnych do wycwiczenia umiejetnosci.
       Prawdopodobnie gdzies w okolicach 20-40% cwiczy sie najszybciej. Mnie to
       specjalnie nie martwi. Na pewno od 66% zaczyna sie robic trudniej niz
       obecnie. Tym bardziej warto cwiczyc od razu na 75%. */
    prog = ( ( ( ( ch->pcdata->learned[ sn ] * ch->pcdata->learned[ sn ] ) / 50 + 15 ) * skill_table[ sn ].to_use / 5 ) / ( get_curr_int( ch ) - 2 ) );

    if ( ch->pcdata->used[ sn ] >= prog )
    {
	++ch->pcdata->learned[ sn ];
	ch->pcdata->used[ sn ] = 0;
	if ( ch->pcdata->learned[ sn ] < 100 )
	    sprintf( buf, "{MStajesz si`e lepsz$0 $T $t!{x" );
	else
	    sprintf( buf, "{RJeste`s najlepsz$0 $T $t!{x" );
	act( ZM_ZAWSZE, buf, ch, co, czy_we( co ) ? "we" : "w", TO_CHAR );
	gain_exp( ch, 10 );
    }

    return;
}


/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain = 0;
    int racehpgainextra = race_table[ ch->race ].hp_gain;

    if ( IS_SET( ch->in_room->room_flags, ROOM_NOTE_READ ) )
	return 0; /* Lam 23.2.98 */

    if ( IS_NPC( ch ) )
    {
	gain += ch->level / 2 + 1;
	switch ( ch->position )
	{
	    case POS_SLEEPING:
		gain += get_curr_con( ch ) * 2;
		break;
	    case POS_RESTING:
		gain += get_curr_con( ch );
		break;
	}
    }
    else
    {
	int szybsze = ch->pcdata->learned[ gsn_fast_healing ];
	int regen;

	gain += 5;

	if ( IS_AFFECTED( ch, AFF_REGENERACJA ) )
	    regen = 100;
	else
	{
	    regen = szybsze;
	    if ( szybsze )
		uzyj( ch, gsn_fast_healing, "szybkim zdrowieniu" );
	}

	switch ( ch->position )
	{
	    default:
		if ( regen )
		    gain += regen / 4;
		break;

	    case POS_SLEEPING:
		gain += get_curr_con( ch ) * 2;

		if ( regen )
		    gain += ch->level * regen / 64;
		break;

	    case POS_RESTING:
		gain += get_curr_con( ch );

		if ( regen )
		    gain += ch->level * regen / 128;
		break;
	}

	if ( ch->pcdata->condition[ COND_FULL ] == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[ COND_THIRST ] == 0 )
	    gain /= 2;
    }

    gain += racehpgainextra;

    if ( IS_SET( ch->in_room->room_flags, ROOM_CLAN_HP ) )
	gain *= 2;

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

    if ( IS_AFFECTED( ch, AFF_DEGENERACJA ) )
	gain /= 2;

    if ( IS_AFFECTED( ch, AFF_WRZACA_KREW ) )
	gain /= 2;

    return gain;
}


int mana_gain( CHAR_DATA *ch )
{
    int gain = 0;
    int racemanaextra = race_table[ ch->race ].mana_gain;

    if ( IS_SET( ch->in_room->room_flags, ROOM_NOTE_READ ) )
	return 0; /* Lam 23.2.98 */

    /* Lam: moby moga zuzywac mane, jesli maja czarna aure */
    if ( IS_NPC( ch ) )
    {
	gain = ch->level + 1;
	switch ( ch->position )
	{
	    case POS_SLEEPING:
		gain += get_curr_int( ch ) * 2;
		break;
	    case POS_RESTING:
		gain += get_curr_int( ch );
		break;
	}
    }
    else
    {
	int szybsze = ch->pcdata->learned[ gsn_meditation ];
	int regen;

	gain += 5;

	if ( IS_AFFECTED( ch, AFF_REGENERACJA ) )
	    regen = 100;
	else
	{
	    regen = szybsze;
	    if ( szybsze )
		uzyj( ch, gsn_meditation, "medytowaniu" );
	}

	switch ( ch->position )
	{
	    default:
		if ( regen )
		    gain += regen / 4;
		break;

	    case POS_SLEEPING:
		gain += get_curr_int( ch ) * 2;

		if ( regen )
		    gain += ch->level * regen / 128;
		break;

	    case POS_RESTING:
		gain += get_curr_int( ch );

		if ( regen )
		    gain += ch->level * regen / 256;
		break;
	}

	if ( ch->pcdata->condition[ COND_FULL ] == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[ COND_THIRST ] == 0 )
	    gain /= 2;
    }

    gain += racemanaextra;

    if ( IS_SET( ch->in_room->room_flags, ROOM_CLAN_MANA ) )
	gain *= 2;

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

    if ( IS_AFFECTED( ch, AFF_DEGENERACJA ) )
	gain /= 2;

    if ( IS_AFFECTED( ch, AFF_WRZACA_KREW ) )
	gain /= 2;

    return gain;
}


int move_gain( CHAR_DATA *ch )
{
    int gain, racemoveextra;

    if ( IS_SET( ch->in_room->room_flags, ROOM_NOTE_READ ) )
	return 0; /* Lam 23.2.98 */

    gain = 0;
    racemoveextra = race_table[ ch->race ].move_gain;

    switch ( ch->position )
    {
	default:           gain += get_curr_dex( ch ); break;
	case POS_RESTING:  gain += get_curr_dex( ch ) * 2; break;
	case POS_SLEEPING: gain += get_curr_dex( ch ) * 3; break;
    }

    if ( IS_NPC( ch ) )
	gain += UMIN( ch->level, 20 );
    else
    {
	if ( ch->pcdata->condition[ COND_FULL ] == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[ COND_THIRST ] == 0 )
	    gain /= 2;
    }

    if ( IS_AFFECTED( ch, AFF_REGENERACJA ) )
	gain *= 2;

    gain += racemoveextra;

    if ( IS_SET( ch->in_room->room_flags, ROOM_REFRESH ) )
	gain *= 2;

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

    if ( IS_AFFECTED( ch, AFF_DEGENERACJA ) )
	gain /= 2;

    if ( IS_AFFECTED( ch, AFF_WRZACA_KREW ) )
	gain /= 2;

    return gain;
}


/* Przeznaczenie: uzywane przy odzyskiwaniu przytomnosci (przebudzeniu). */
void komunikaty_ustroju( CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) )
	return;

    if ( ch->pcdata->condition[ COND_FULL ] == 0 )
	act( ZM_PRZYT, "Umierasz z g`lodu!", ch, NULL, NULL, TO_CHAR );
    else if ( ch->pcdata->condition[ COND_FULL ] == 1
	   || ch->pcdata->condition[ COND_FULL ] == 2 )
	act( ZM_PRZYT, "Jeste`s g`lodn$y.", ch, NULL, NULL, TO_CHAR );

    if ( ch->pcdata->condition[ COND_THIRST ] == 0 )
	act( ZM_PRZYT, "Umierasz z pragnienia!", ch, NULL, NULL, TO_CHAR );
    else if ( ch->pcdata->condition[ COND_THIRST ] == 1
	   || ch->pcdata->condition[ COND_THIRST ] == 2 )
	act( ZM_PRZYT, "Jeste`s spragnion$y.", ch, NULL, NULL, TO_CHAR );

    return;
}


void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if ( value == 0
      || IS_NPC( ch )
      || ( value < 0
	&& iCond != COND_DRUNK
	&& ( ch->level >= LEVEL_HERO || ch->level < 0 ) ) )
	return;

    condition                           = ch->pcdata->condition[ iCond ];

    /* Vigud 28.9.2010: nie-wampiry ponizej 5 poziomu nie glodnieja na smierc */
    if ( iCond != COND_DRUNK && ch->level < 5 && !CZY_WAMPIR( ch ) )
	ch->pcdata->condition[ iCond ]      = URANGE( 1, condition + value, 48 );
    else
	ch->pcdata->condition[ iCond ]      = URANGE( 0, condition + value, 48 );

    if ( iCond == COND_DRUNK )
    {
	if ( ch->pcdata->condition[ iCond ] == 0
	  && condition > 0 )
	    act( ZM_PRZYT, "Trze`xwiejesz zupe`lnie.", ch, NULL, NULL, TO_CHAR );
	else if ( ch->pcdata->condition[ iCond ] == 3
	       && condition > 3 )
	    act( ZM_PRZYT, "Zaczynasz trze`xwie`c.", ch, NULL, NULL, TO_CHAR );
    }

    if ( iCond == COND_FULL )
    {
	if ( ch->pcdata->condition[ iCond ] == 0 )
	    act( ZM_PRZYT, "Umierasz z g`lodu!", ch, NULL, NULL, TO_CHAR );
	else if ( ch->pcdata->condition[ iCond ] == 1
	       || ch->pcdata->condition[ iCond ] == 2 )
	    act( ZM_PRZYT, "Jeste`s g`lodn$y.", ch, NULL, NULL, TO_CHAR );
	else if ( ch->pcdata->condition[ iCond ] == 3
	       && condition > 3 )
	    act( ZM_PRZYT, "Robisz si`e g`lodn$y.", ch, NULL, NULL, TO_CHAR );
    }

    if ( iCond == COND_THIRST )
    {
	if ( ch->pcdata->condition[ iCond ] == 0 )
	    act( ZM_PRZYT, "Umierasz z pragnienia!", ch, NULL, NULL, TO_CHAR );
	else if ( ch->pcdata->condition[ iCond ] == 1
	       || ch->pcdata->condition[ iCond ] == 2 )
	    act( ZM_PRZYT, "Jeste`s spragnion$y.", ch, NULL, NULL, TO_CHAR );
	else if ( ch->pcdata->condition[ iCond ] == 3
	       && condition > 3 )
	    act( ZM_PRZYT, "Robisz si`e spragnion$y.", ch, NULL, NULL, TO_CHAR );
    }

    return;
}


/*
 * Lam 2.5.98: moja wizja bycia pijanym :)
 */
void pijanstwo( CHAR_DATA *ch )
{
    char *to_char = NULL;
    char *to_room = NULL;

    if ( IS_NPC( ch )
      || PP( ch )
      || ( ch->pcdata->condition[ COND_DRUNK ] < 16
	&& !IS_AFFECTED( ch, AFF_DEZORIENTACJA ) )
      || number_bits( 3 ) )
	return;

    switch ( IS_AFFECTED( ch, AFF_DEZORIENTACJA )
     ? number_range( 1, 14 )
     : number_bits( 3 ) + ch->pcdata->condition[ COND_DRUNK ] / 10 )
    {
	/* zawor bezpieczenstwa? Sprobuj "mustaw sie nietrzezwosc 100" */
	default:to_char = "Ale czadowo! Wszystko takie kolorowe...";
		break;
	case 1: to_char = "Czujesz si`e ca`lkiem fajnie.";
		break;
	case 2: to_char = "Kr`eci ci si`e w g`lowie.";
		break;
	case 3: to_char = "Jest ci bardzo weso`lo.";
		to_room = "$n g`lupio si`e u`smiecha.";
		break;
	case 4: to_char = "Nie mo`zesz powstrzyma`c si`e od `smiechu.";
		to_room = "$n nagle wybucha `smiechem!";
		break;
	case 5: to_char = "Kto`s co`s do ciebie m`owi, ale nie wiesz co.";
		to_room = "$n rozgl`ada si`e jakby czego`s poszukuj`ac.";
		break;
	case 6: to_char = "Musisz sobie bekn`a`c.";
		to_room = "$n beka g`lo`sno.";
		break;
	case 7: to_char = "S`lysza`l$o`s? Kto`s ci`e chyba wo`la`l z oddali...";
		break;
	case 8: to_char = "Niedobrze ci.";
		to_room = "$n robi si`e zielon$0.";
		break;
	case 9: to_char = "Wymiotujesz.";
		to_room = "$n wymiotuje prosto na ciebie!";
		break;
	case 10:to_char = "Nagle ze `sciany wype`lza bia`la mysz!";
		break;
	case 11:to_char = "Unosisz si`e w powietrze i robisz kilka salt.";
		to_room = "$n przewraca si`e niezdarnie na ziemi`e.";
		break;
	case 12:to_char = "Poznajesz przysz`lo`s`c wszech`swiata!";
		break;
	/* ponizsze teoretycznie nieosiagalne bez mustaw */
	case 13:to_char = "Jeste`s wsz`edzie, jeste`s wszystkim, jeste`s wszechobecn$y!";
		break;
	case 14:to_char = "Odkrywasz, `ze jeste`s najwy`zszym w`ladc`a rzeczywisto`sci!";
		break;
    }

    if ( !IS_AWAKE( ch ) )
    {
	if ( number_bits( 2 ) )
	    return; /* mniejsze (wprost minimalne) szanse podczas snu */

	to_char = "`Sni`a ci si`e koszmary.";
	to_room = "$n mamrocze co`s przez sen.";
    }

    if ( to_char )
	ASTC( to_char, ch );
    if ( to_room )
	act( ZM_SLUCH, to_room, ch, NULL, NULL, TO_ROOM );

    return;
}


/*
 * Ulryk 20.10.2002: wzbudzenie strachu
 */
void wzbudzenie_strachu( CHAR_DATA *ch )
{
    if ( IS_AFFECTED( ch, AFF_GROZA )
      && ( !IS_SET( ch->in_room->room_flags, ROOM_SAFE ) ) )
    {
	CHAR_DATA  *mch;
	CHAR_DATA  *mch_nast;
	EXIT_DATA  *pexit;
	int         direction;
	int         door = -1;

	for ( mch = ch->in_room->people; mch; mch = mch_nast )
	{
	    /* Lam 13.1.2003: to bylo ponizej sprawdzenia, czy mch->deleted,
		czyli gdyby pierwsza postac w ch->in_room byla skasowana,
		mud by sie wykrzaczyl. To jednak nigdy sie nie zdarzylo, ale
		na wszelki wypadek... */
	    mch_nast = mch->next_in_room;

	    if ( mch->deleted
	      || !IS_NPC( mch )
	      || !can_see( mch, ch )
	      || !IS_AWAKE( mch )
	      || IS_AFFECTED( mch, AFF_CHARM )
	      || IS_AFFECTED( mch, AFF_GROZA )
	      || ( !IS_SET( mch->act, ACT_WIMPY ) && ( ch->level + 5 < mch->level ) )
	      || number_bits( 1 ) )
		continue;

	    if ( !IS_SET( mch->act, ACT_SENTINEL ) )
		for ( direction = 0; direction <= 9; direction++ )
		{
		    if ( ( pexit = mch->in_room->exit[ direction ] )
		      && ( number_range( 0, direction ) == 0 )
		      && pexit->to_room
		      && !IS_SET( pexit->exit_info, EX_CLOSED )
		      && !IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
		      && ( !IS_SET( mch->act, ACT_STAY_AREA )
			|| pexit->to_room->area == ch->in_room->area )
		      && can_enter_room( mch, pexit->to_room, FALSE )
		      && can_move( mch, mch->in_room, pexit->to_room, door, FALSE ) )
			door = direction;
		}

	    if ( door == -1 )
	    {
		char   buf[ MAX_STRING_LENGTH ];

		buf[ 0 ] = '\0';

		switch ( number_bits( 2 ) )
		{
		default:
		    act( ZM_WZROK | ZM_WID_CHAR, "$n patrzy na ciebie z przera`zeniem.", mch, NULL, ch, TO_VICT );
		    act( ZM_WZROK | ZM_WID_VICT, "$N patrzy na $# z przera`zeniem.", ch, NULL, mch, TO_NOTVICT );
		    break;
		case 0:
		    sprintf( buf, "Zostaw mnie w spokoju, %s.",
			    ch->wolacz );
		    break;
		case 1:
		    sprintf( buf, "Odejd`x st`ad %s i daj mi spok`oj.",
			    ch->wolacz );
		    break;
		case 2:
		    sprintf( buf, "Nie chc`e mie`c z tob`a nic do czynienia, %s.",
			    ch->wolacz );
		    break;
		}
		if ( buf[ 0 ] != '\0' )
		   do_say( mch, buf );
	    }
	    else
	    {
		act( ZM_WZROK | ZM_WID_CHAR, "$n ucieka w pop`lochu.", mch, NULL, NULL, TO_ROOM );
		move_char( mch, door, FALSE );
	    }
	}
    }

    return;
}


/*
 * Mob autonomous action.
 */
void mobile_update( void )
{
    CHAR_DATA *ch;
    EXIT_DATA *pexit;
    int        door;

    /* Examine all mobs. */
    for ( ch = char_list; ch; ch = ch->next )
    {
	int rnum;

	if ( ch->deleted )
	    continue;

	/* Lam: troche zamieniona kolejnosc, zeby ropijaczyc muda :) */
	if ( !ch->in_room )
	    continue;

	if ( IS_AFFECTED( ch, AFF_GROZA ) )
	    wzbudzenie_strachu( ch );

	if ( !IS_NPC( ch ) )
	{
	    pijanstwo( ch );
	    /* Lam: dwa warunki sie powtarzaja, ale nie wyciagnalem, zeby bylo
	       widac, ze jest kopiowane z check_airchar i latwo sie kopiowalo
	       w przyszlosci, a te akurat to zadna strata mocy procesora, bo
	       typ podloza predzej bedzie je eliminowal */
	    if ( ( ch->in_room->sector_type == SECT_WATER_DEEP
		&& !IS_FLYING( ch )
		&& !IS_SET( race_table[ ch->race ].poruszanie, POR_POD_WODA )
		&& !IS_SET( race_table[ ch->race ].poruszanie, POR_PO_WODZIE )
		&& !IS_AFFECTED( ch, AFF_KONTROLA_NAD_WODA )
		&& !jest_w_lodzi( ch ) )
	      || ( CZY_POD_WODA( ch->in_room )
		&& !IS_SET( race_table[ ch->race ].poruszanie, POR_POD_WODA )
		&& !IS_AFFECTED( ch, AFF_KONTROLA_NAD_WODA ) ) )
	    {
		/* jesli w ogole nie umie plywac, podlega pod check_airchar,
		   a nie tutaj
		   Lam 23.5.2006: koniec ruchu tez powoduje slabego plywaka */
		if ( ch->move < 10
		  || ( ch->pcdata->learned[ gsn_plywanie ]
		    && ch->pcdata->learned[ gsn_plywanie ] < 100
		    && number_percent( ) + 10 > ch->pcdata->learned[ gsn_plywanie ] ) )
		{
		    AIRCHAR_DATA *airchar = find_airchar( ch );
		    if ( !airchar )
			airchar = new_airchar( ch );
		    airchar->slaby_plywak = TRUE;
		}
	    }
	    continue;
	}

	if ( IS_AFFECTED( ch, AFF_CHARM ) )
	    continue;

	/* Lam 29.7.2005 */
	if ( ch->position < POS_FIGHTING
	  && !number_bits( 4 )
	  && !IS_SET( ch->pIndexData->progtypes, M_ASLEEP_TIME_PROG ) )
	    /* moze sie nie udac - tam sa warunki, tu nie ma sensu powtarzac */
	    do_stand( ch, "" );

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0
	  && !is_affected( ch, gsn_zaglada ) )
	{
	    if ( ( *ch->spec_fun )( ch ) )
		continue;
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position < POS_STANDING )
	    continue;

	/* polowanie po specu, zeby burmistrz nie zboczyl z trasy, po
	   sprawdzeniu pozycji (nie musze patrzec na ch->fighting); sprawdzam
	   czy sentinel na wypadek opetujacych niesmiertelnych, bo sentinelowi
	   i tak sie samo nie ustawi */
	if ( !IS_SET( ch->act, ACT_SENTINEL ) && ch->hunting )
	{
	    hunt_victim( ch );
	    continue;
	}

	/* Lam 1.1.2003: jednak rand_prog uruchamia sie zawsze
	if ( ch->in_room->area->nplayer > 0 )
	{ */
	    mprog_random_trigger( ch );
	    if ( ch->position < POS_STANDING || ch->deleted )
		continue;
/*	} */

	if ( !number_bits( 4 ) )
	    check_wield( ch );

	/* Scavenge */
	if ( IS_SET( ch->act, ACT_SCAVENGER )
	    && ch->in_room->contents
	    && number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int       max;

	    max         = 1;
	    obj_best    = 0;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
		if ( CAN_WEAR( obj, ITEM_TAKE )
		    && obj->cost > max
		    && can_see_obj( ch, obj ) )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}

	    if ( obj_best
	      && ch->carry_number + get_obj_number( obj_best ) < can_carry_n( ch )
	      && ch->carry_weight + get_obj_weight( obj_best ) < can_carry_w( ch ) )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n podnosi $j.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/* Ulryk 10.01.2003: odzywianie sie mobow */
	if ( IS_NPC( ch ) && ( !ch->pIndexData->pShop ) && ( !ch->fighting )
	  && ( !number_bits( 5 ) ) )
	{
	    OBJ_DATA *jedzonko;
	    OBJ_DATA *jedzonko_nast;
	    bool      pojadlem = FALSE;
	    bool      popilem = FALSE;

	    for ( jedzonko = ch->carrying;
		  jedzonko;
		  jedzonko = jedzonko_nast )
	    {
		jedzonko_nast = jedzonko->next_content;

		if ( jedzonko->deleted )
		    continue;

		if ( ( jedzonko->item_type == ITEM_FOOD )
		  && ( jedzonko->value[ 3 ].v == 0 )
		  && ( !pojadlem ) )
		{
		    do_eat( ch, jedzonko->name );
		    pojadlem = TRUE;
		    /* Ulryk: jesli czesc ciala, to zjada w calosci */
		    if ( jedzonko->value[ 1 ].v != 0 )
			extract_obj( jedzonko );
		    continue;
		}
		if ( ( jedzonko->item_type == ITEM_DRINK_CON  )
		  && ( jedzonko->value[ 3 ].v == 0 )
		  && ( !popilem ) )
		{
		    do_drink( ch, jedzonko->name );
		    popilem = TRUE;
		    continue;
		}
	    }
	}

	/* Wander or Flee  - modifications by Jason Dinkel */
	/* Lam: uciekanie w poplochu */
	if ( ch->hit < ch->max_hit / 2
	  || ( ch->fearing && ch->fearing->in_room
	    && ch->fearing->in_room == ch->in_room ) )
	    rnum = 3;
	else
	    rnum = 5;

	/* Okropny, ogromny warunek. */
	if ( !IS_SET( ch->act, ACT_SENTINEL )
	  && ( door = number_bits( rnum ) ) <= 9
	  && ( pexit = ch->in_room->exit[ door ] )
	  && pexit->to_room
	  && ( !IS_SET( pexit->exit_info, EX_CLOSED )
	    || ( IS_PASSING( ch )
	      && !IS_SET( pexit->exit_info, EX_PASSPROOF ) )
	    || ( !IS_SET( pexit->exit_info, EX_LOCKED )
	      && IS_SET( race_table[ ch->race ].race_abilities, RACE_OPENS_DOORS ) ) )
	  /* Tu juz wiemy, ze ma dokad isc. Ale moze nie chce? Moby, ktore ktos
	     przyprowadzil do miejsc bez-moba, beda po nich lazily, az znajda
	     miejsce stosowne dla mobow. */
	  && ( !IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
	    || IS_SET( ch->in_room->room_flags, ROOM_NO_MOB ) )
	  /* Jednokrainowcy nie ruszaja sie ze swojej krainy sami, ale jesli
	     ktos ich wyprowadzi, nie przelaza miedzy krainami, nawet do swojej
	     (szansa mizerna, ale mozna kiedys zmienic). */
	  && ( !IS_SET( ch->act, ACT_STAY_AREA )
	    || pexit->to_room->area == ch->in_room->area )
	  /* Mozliwosc ruchu, glownie umiejetnosc poruszania sie. */
	  && can_move( ch, ch->in_room, pexit->to_room, door, FALSE )
	  /* Sam nie wyjdzie/nie wejdzie pod wode bez oddychania, a jesli sie
	     dusi, bedzie lazil gdzie tylko moze szukajac wyjscia. */
	  && ( !CZY_BRAK_TLENU_W_POM( ch, pexit->to_room )
	    || CZY_BRAK_TLENU_W_POM( ch, ch->in_room ) )
	  /* Tu glupi przypadek - jesli mob nie ma oddychania pod woda, nie
	     wejdzie na gleboka wode, jesli mialby w nia wpasc. Wpadania
	     z wysokiej gory do glebokiej wody nie mam zamiaru obslugiwac :) */
	  && !( pexit->to_room->sector_type == SECT_WATER_DEEP
	     && !IS_AFFECTED( ch, AFF_GILLS )
	     && !IS_SET( race_table[ ch->race ].oddychanie, ODD_WODA )
	     /* To kopia tego, co bylo wyzej, kopii check_airchar :) */
	     && !IS_FLYING( ch )
	     && !IS_SET( race_table[ ch->race ].poruszanie, POR_POD_WODA )
	     && !IS_SET( race_table[ ch->race ].poruszanie, POR_PO_WODZIE )
	     && !IS_AFFECTED( ch, AFF_KONTROLA_NAD_WODA )
	     && !jest_w_lodzi( ch ) )
	  /* Wampirom nie chce sie wychodzic na slonce. */
	  && ( !CZY_PALACE_SLONCE_W_POM( ch, pexit->to_room )
	    || CZY_PALACE_SLONCE_W_POM( ch, ch->in_room ) ) )
	{
	    /* Give message if hurt */
	    if ( rnum == 3 )
		act( ZM_WZROK | ZM_WID_CHAR, "$n ucieka w pop`lochu!", ch, NULL, NULL, TO_ROOM );

	    move_char( ch, door, FALSE );
	    /* Lam: zabezpieczenie przed MOBProgami */
	    if ( ch->position < POS_STANDING
	      || ch->deleted
	      || ch->in_room != pexit->to_room )
		continue;

	    /* If people are in the room, then flee. */
	    if ( rnum == 3 )
	    {
		CHAR_DATA *rch;

		for ( rch = pexit->to_room->people;
		      rch;
		      rch = rch->next_in_room )
		{
		    if ( rch->deleted )
			continue;

		    if ( !IS_NPC( rch ) && can_see( ch, rch ) )
		    {
			int direction;

			door = -1;
			act( ZM_WZROK | ZM_WID_CHAR, "$n ucieka w pop`lochu!", ch, NULL, NULL, TO_ROOM );

			/* Find an exit giving each one an equal chance */
			for ( direction = 0; direction <= 9; direction++ )
			{
			    /* Lam 20.12.1999 na wniosek Grzecha:
					ROOM_NO_MOB, ACT_STAY_AREA */
			    /* tu panika, wiec mniej inteligencji - moze
			       probowac sie utopic */
			    if ( ( pexit = ch->in_room->exit[ direction ] )
			      && ( number_range( 0, direction ) == 0 )
			      && pexit->to_room
			      && ( !IS_SET( pexit->exit_info, EX_CLOSED )
				|| ( IS_PASSING( ch )
				  && !IS_SET( pexit->exit_info, EX_PASSPROOF ) ) )
			      && !IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
			      && ( !IS_SET( ch->act, ACT_STAY_AREA )
				|| pexit->to_room->area == ch->in_room->area )
			      && can_move( ch, ch->in_room, pexit->to_room, direction, FALSE ) )
				door = direction;
			}

			/* If no exit, attack.  Else flee! */
			if ( door == -1 )
			{
			    act( ZM_WZROK | ZM_WID_CHAR, "$n w panice nie znajduje wyj`scia i rzuca si`e do ataku!", ch, NULL, NULL, TO_ROOM );
			    multi_hit( ch, rch, TYPE_UNDEFINED );
			}
			else
			{
			    ROOM_INDEX_DATA *in_room;

			    in_room = ch->in_room;
			    move_char( ch, door, FALSE );
			    if ( ch->in_room
			      && ch->in_room == in_room
			      && ch->in_room == rch->in_room
			      && IS_AWAKE( ch ) )
			    {
				act( ZM_WZROK | ZM_WID_CHAR, "$n, w panice i nie mog`ac uciec, rzuca si`e do ataku!", ch, NULL, NULL, TO_ROOM );
				multi_hit( ch, rch, TYPE_UNDEFINED );
			    }
			}
			break; /* break wystarczy jako zabezpieczenie przed
				  MOBProgiem (chyba) - zobaczymy... */
		    }
		}
	    }
	}
    }

    return;
}


/*
 * Update the weather.
 */
void weather_update( AREA_DATA *area )
{
    DESCRIPTOR_DATA *d;
    char             buf[ MAX_STRING_LENGTH ];
    int              diff, pomi;

    buf[ 0 ] = '\0';

    switch ( ++area->time_info.hour )
    {
    case  6:
	area->weather_info.sunlight = SUN_RISE;
	strcat( buf, "{YS`lo`nce wstaje na wschodzie.{x\n\r" );
	break;

    case  7:
	area->weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "{YRozpocz`a`l si`e dzie`n.{x\n\r" );
	break;

    case 19:
	area->weather_info.sunlight = SUN_SET;
	strcat( buf, "{yS`lo`nce powoli znika za horyzontem.{x\n\r" );
	break;

    case 20:
	area->weather_info.sunlight = SUN_DARK;
	strcat( buf, "Rozpoczyna si`e noc.\n\r" );
	break;

    case 24:
	area->time_info.hour = 0;
	area->time_info.day++;
	break;
    }

    if ( area->time_info.day   >= 35 )
    {
	area->time_info.day = 0;
	area->time_info.month++;
    }

    if ( area->time_info.month >= 17 )
    {
	area->time_info.month = 0;
	area->time_info.year++;
    }

    /* Lam 10.1.2003: o polnocy informuje o nadejsciu swieta */
    if ( !area->time_info.hour )
    {
	for ( pomi = 0; holiday_table[ pomi ].name; pomi++ )
	    if ( holiday_table[ pomi ].month == area->time_info.month + 1
	      && holiday_table[ pomi ].day == area->time_info.day + 1 )
		break;

	if ( holiday_table[ pomi ].name )
	    for ( d = descriptor_list; d; d = d->next )
		if ( d->connected == CON_PLAYING
		  && d->character->in_room->area == area
		  && holiday_table[ pomi ].name )
		    send_to_char( holiday_table[ pomi ].message, d->character );
    }

    /*
     * Weather change.
     */
    if ( area->time_info.month >= 9 && area->time_info.month <= 16 )
	diff = area->weather_info.mmhg > 985 ? -2 : 2;
    else
	diff = area->weather_info.mmhg > 1015 ? -2 : 2;

    area->weather_info.change   += diff * dice( 1, 4 ) + dice( 2, 6 ) - dice( 2, 6 );
    area->weather_info.change    = UMAX( area->weather_info.change, -12 );
    area->weather_info.change    = UMIN( area->weather_info.change, 12 );

    area->weather_info.mmhg     += area->weather_info.change;
    area->weather_info.mmhg      = UMAX( area->weather_info.mmhg, 960 );
    area->weather_info.mmhg      = UMIN( area->weather_info.mmhg, 1040 );

    switch ( area->weather_info.sky )
    {
    default:
	bug( "Weather_update: bad sky %d.", area->weather_info.sky );
	area->weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( area->weather_info.mmhg < 990
	  || ( area->weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "Chmurzy si`e.\n\r" );
	    area->weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if ( area->weather_info.mmhg < 970
	  || ( area->weather_info.mmhg < 990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "Zaczyna pada`c.\n\r" );
	    area->weather_info.sky = SKY_RAINING;
	}

	if ( area->weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "Chmury znikaj`a.\n\r" );
	    area->weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if ( area->weather_info.mmhg < 970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "{WNa niebie widzisz rozb`lyski b`lyskawic.{x\n\r" );
	    area->weather_info.sky = SKY_LIGHTNING;
	}

	if ( area->weather_info.mmhg > 1030
	  || ( area->weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "Deszcz przesta`l pada`c.\n\r" );
	    area->weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if ( area->weather_info.mmhg > 1010
	  || ( area->weather_info.mmhg > 990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "Przesta`lo b`lyska`c.\n\r" );
	    area->weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }

    if ( buf[ 0 ] != '\0' )
    {
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	      && d->character->in_room
	      && d->character->in_room->area == area )
	    {
		if ( IS_OUTSIDE( d->character )
		  && IS_AWAKE( d->character )
		  && d->character->in_room->sector_type != SECT_UNDERWATER
		  && can_see_room( d->character, d->character->in_room ) )
		    send_to_char( buf, d->character );

		/* FIXME: to nie obsluguje rozlaczonych postaci, bo leci
		   po deskryptorach. Jesli 1. rozlaczy sie, 2. nie ma swiatla,
		   3. jest na dworze, ale nie w miescie, 4. akurat minela
		   godzina 19, 5. polaczy sie ("czesto" sie zdarza), pojawi mu
		   sie komunikat o przyzwyczajaniu dopiero, jak sie ruszy */
		if ( area->weather_info.sunlight == SUN_SET
		  || area->weather_info.sunlight == SUN_RISE )
		    sprawdz_przyzwyczajanie_ch( d->character, room_is_dark( d->character->in_room ) );
	    }
	}
    }

    return;
}


/*
 * Lam 27.1.2004: plynne odradzanie sie postaci
 * Funkcja operuje na tysiecznych czesciach punktu zycia/many/ruchu, przez
 * calosc obliczen liczy wedlug wzorow "na godzine", potem dzielac to przez
 * ilosc pulsow w aktualnej godzinie, dodajac pz/m/r lub zadajac obrazenia,
 * a nastepnie zapamietujac reszty w strukturze postaci do kolejnego odradzania.
 *
 * Lam 29.1.2004: optymalizuje pod katem szybkosci i poprawnosci obliczen.
 * Lam 4.2.2006: obsluga ch->wait u mobow zamiast w game_loop dla deskryptorow
 *
 * Z calego Laca, wlasnie ta funkcja zajmuje okolo 2/3 czasu procesora.
 */
void plynne_odradzanie( void )
{
    CHAR_DATA *ch, *ch_nast;
    ROOM_INDEX_DATA *in_room;
    int putym, manatym, ruchtym, puprzed;

    for ( ch = char_list; ch; ch = ch_nast )
    {
	ch_nast = ch->next;

	if ( ch->deleted )
	    continue;

	if ( ch->wait )
	{
	    if ( ch->desc )
		ch->desc->wait = TRUE;
	    ch->wait--;
	}

	if ( IS_NPC( ch ) )
	{
	    /* Slugi mozna opuszczac bez rozpadania, rzucajac sobie aure spokoju
	       i rozkazujac im kogos atakowac. Takie moby dostaja
	       w stop_follower specjalna kombinacje flag - AFF_SLUGA bez wplywu.
	       Funkcje wywolujace stop/die_follower nie spodziewaja sie smierci
	       postaci, wiec latam to w ten sposob z lenistwa. */
	    if ( IS_AFFECTED( ch, AFF_SLUGA )
	      && !IS_AFFECTED( ch, AFF_CHARM ) )
	    {
		send_to_char( "Rozpadasz si`e z braku pana.\n\r", ch );
		act( ZM_WZROK | ZM_WID_CHAR, "$n si`e rozpada.",
		     ch, NULL, NULL, TO_ROOM );

		stop_fighting( ch, TRUE );
		extract_char( ch, TRUE );
		continue;
	    }
	}
	else if ( ch->pcdata->odwleczenie )
	    ch->pcdata->odwleczenie--;

	in_room = ch->in_room;

	if ( IS_SET( in_room->room_flags, ROOM_NOTE_READ ) )
	    continue;

	putym = manatym = ruchtym = 0;

	/* w czysccu ma miejsce odradzanie, ale nie cierpienie */
	if ( !IS_SET( in_room->room_flags, ROOM_PURGATORY ) )
	{
	    if ( CZY_PALACE_SLONCE( ch ) )
	    {
		int dmg;

		if ( ch->in_room->sector_type == SECT_INSIDE )
		    dmg = 10;
		else if ( ch->in_room->sector_type == SECT_FOREST )
		    dmg = 25;
		else
		    dmg = 50;

		if ( ch->in_room->area->weather_info.sky == SKY_CLOUDY )
		    dmg /= 2;
		if ( ch->in_room->area->weather_info.sky == SKY_RAINING )
		{
		    dmg *= 3;
		    dmg /= 4;
		}
		dmg = (int) dmg * ( ( ch->max_hit << 10 ) / ( 200 + ch->level ) );

		putym -= dmg;
	    }

	    if ( CZY_BRAK_TLENU( ch ) )
		putym -= ( 5120 + ( ch->level << 11 ) );

	    if ( IS_AFFECTED( ch, AFF_POISON ) )
	    {
		AFFECT_DATA *paf;

		for ( paf = ch->affected; paf; paf = paf->next )
		    if ( !paf->deleted && IS_SET_V( paf->bitvector, AFF_POISON ) )
			break;

		if ( !paf )
		    bug( "plynne_odradzanie: IS_AFFECTED zwraca prawde, a we wplywach nie znaleziono!", 0 );
		else
		    putym -= ( paf->level / 7 + 1 ) << 10;
	    }

	    if ( IS_AFFECTED( ch, AFF_WRZACA_KREW ) )
	    {
		AFFECT_DATA *paf;

		for ( paf = ch->affected; paf; paf = paf->next )
		    if ( !paf->deleted && IS_SET_V( paf->bitvector, AFF_WRZACA_KREW ) )
			break;

		if ( paf )
		{
		    /* Gimza: tak skalkulowane, ze czar z 30 poz. sciaga zlym
		       okolo 30 pz; neutralnym polowe tego */
		    if ( ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
		      || ( !IS_NPC( ch )
			&& ( IS_SET( ch->act, PLR_KILLER )
			  || IS_SET( ch->act, PLR_THIEF ) ) ) )
		    /* Gimza 29.01.05: wydluzone i jeszcze mocniej dziala na
		       zlych */
			putym -= ( paf->level / 4 + 1 ) << 13;
		    else
			putym -= ( paf->level / 4 + 1 ) << 12;
		}
	    }

	    if ( !IS_NPC( ch ) )
	    {
		if ( ch->pcdata->condition[ COND_FULL ] <= 0 )
		    putym -= ( UMAX( ch->max_hit, 40 ) << 10 ) / 40;

		if ( ch->pcdata->condition[ COND_THIRST ] <= 0 )
		    putym -= ( UMAX( ch->max_hit, 25 ) << 10 ) / 25;
	    }
	}

	if ( ch->position >= POS_STUNNED )
	{
	    if ( ch->hit < ch->max_hit )
		putym += hit_gain( ch ) << 10;
	    else if ( putym < 0 )
	    {
		/* jesli jest glodny, ale szybciej sie odradza, zeby nie
		   zalewalo napisami, ze zycie osiaga maksimum */
		putym += hit_gain( ch ) << 10;
		if ( putym > 0 )
		    putym = 0;
	    }

	    /* many i ruchu na razie nic nie zabiera */
	    if ( ch->mana < ch->max_mana )
		manatym += mana_gain( ch ) << 10;

	    if ( ch->move < ch->max_move )
		ruchtym += move_gain( ch ) << 10;
	}

	if ( ch->position == POS_STUNNED )
	    putym -= 512;
	else if ( ch->position == POS_INCAP )
	    putym -= 2048;
	else if ( ch->position == POS_MORTAL )
	    putym -= 3072;
	else if ( ch->position == POS_DEAD )
	{
	    sprintf( log_buf, "plynne_odradzanie: ch %s POS_DEAD - dobijam",
		ch->short_descr );
	    bug( log_buf, 0 );
	    if ( !IS_NPC( ch ) )
		fprintf( stderr, "%s", posdead_buf );
	    ch->position = POS_MORTAL;
	    damage( ch, ch, 100, TYPE_UNDEFINED, WEAR_NONE, TRUE );
	}

	if ( ch->hit <= 0
	  && ( ch->fighting
	    || ch->position > POS_STUNNED ) )
	{
	    sprintf( bug_buf, "ktos znowu budzi nieprzytomnych: ch %s, %swalczy, pos %d, hit %d",
		ch->name, ch->fighting ? "" : "nie ", ch->position, ch->hit );
	    cbug( bug_buf, 0 );
	    /* Takie cos nie powinno niczego popsuc: */
	    update_pos( ch );
	    stop_fighting( ch, FALSE );
	}

	if ( manatym )
	{
	    manatym /= pulse_point_max;
	    manatym += ch->mana_remainder;
	    ch->mana_remainder = manatym % 1024;
	    manatym /= 1024;
	    if ( manatym > 0 )
	    {
		ch->mana += URANGE( 0, ch->max_mana - ch->mana, manatym );
		if ( ch->mana == ch->max_mana )
		    ASTC( "Odzyska`l$o`s pe`lni`e magicznych si`l.", ch );
	    }
	    else if ( manatym < 0 )
		ch->mana = UMAX( 0, ch->mana - manatym );
	}

	if ( ruchtym )
	{
	    ruchtym /= pulse_point_max;
	    ruchtym += ch->move_remainder;
	    ch->move_remainder = ruchtym % 1024;
	    ruchtym /= 1024;
	    if ( ruchtym > 0 )
	    {
		ch->move += URANGE( 0, ch->max_move - ch->move, ruchtym );
		if ( ch->move == ch->max_move
		  && ( IS_NPC( ch )
		    || !CZY_OPCJA( ch, OPC_RUCH ) ) )
		    ASTC( "Odzyska`l$o`s wszystkie punkty ruchu.", ch );
	    }
	    else if ( ruchtym < 0 )
		ch->move = UMAX( 0, ch->move - ruchtym );
	}

	if ( putym )
	{
	    putym /= pulse_point_max;
	    putym += ch->hit_remainder;
	    ch->hit_remainder = putym % 1024;
	    putym /= 1024;
	    if ( putym > 0 )
	    {
		puprzed = ch->hit;
		ch->hit += URANGE( 0, ch->max_hit - ch->hit, putym );
		if ( puprzed <= 0
		  /* Lam: ten warunek, zeby nie ustawialo POS_DEAD przy
		     znamieniu smierci */
		  && ch->hit > puprzed )
		{
		    update_pos( ch );
		    if ( ch->hit > 0 )
			STC( "Odzyskujesz przytomno`s`c.\n\r", ch );
		}
		else if ( ch->hit == ch->max_hit )
		    ASTC( "Odzyska`l$o`s pe`lni`e zdrowia.", ch );
	    }
	    else if ( putym < 0 )
	    {
		puprzed = ch->hit;
		damage( ch, ch, 0 - putym, TYPE_UNDEFINED, WEAR_NONE, TRUE );

		if ( ch->deleted || in_room != ch->in_room )
		    continue;

		putym = ch->max_hit / 4;
		if ( ch->hit < putym )
		{
		    if ( puprzed >= putym )
			STC( "Tw`oj b`ol narasta!\n\r", ch );
		    else
		    {
			putym = ch->max_hit / 8;
			if ( puprzed >= putym && ch->hit < putym )
			    STC( "Tw`oj b`ol staje si`e nie do zniesienia!\n\r", ch );
		    }
		}
	    }
	}

	if ( !ch->deleted
	  && !IS_NPC( ch )
	  && ch->level == 1
	  && ch->hit < 0 )
	{
	    STC( "Bogowie lecz`a twoje rany!\n\r", ch );
	    ch->hit = ch->max_hit;
	    update_pos( ch );
	}
    }

    return;
}


/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_save;
    CHAR_DATA *ch_quit;
    CHAR_DATA *ch_nast;
    time_t     save_time;

    ch_save     = NULL;
    ch_quit     = NULL;
    save_time   = current_time;

    for ( ch = char_list; ch; ch = ch_nast )
    {
	AFFECT_DATA *paf;
	bool         zabic_sluge = FALSE;  /* Ulryk: do slug Nekromanty */

	ch_nast = ch->next;

	if ( ch->deleted )
	    continue;

	/*
	 * Find dude with oldest save time.
	 */
	if ( !IS_NPC( ch )
	    && ( !ch->desc || ch->desc->connected == CON_PLAYING )
	    &&   ch->level > 0 /* Lam 20.7.2005: teraz wszyscy sie zapisuja */
	    &&   ch->save_time < save_time )
	{
	    ch_save     = ch;
	    save_time   = ch->save_time;
	}

	if ( !IS_NPC( ch )
	  && ( !ch->desc || ch->desc->connected == CON_PLAYING )
	  && ch->level > 0 )
	    sprawdz_restrykcje( ch );

	if ( !IS_NPC( ch )
	  && ( !ch->desc || ch->desc->connected == CON_PLAYING )
	  && ch->level > 0
	  && ( ch->level <= LEVEL_HERO
	    || ( !ch->desc && !IS_SWITCHED( ch ) ) ) )
	{
	    OBJ_DATA *obj;

	    if ( !IS_SET( ch->in_room->room_flags, ROOM_NOTE_READ )
	      && !IS_SET( ch->in_room->room_flags, ROOM_PURGATORY )
	      && ( obj = get_eq_char( ch, WEAR_LIGHT ) )
	      && obj->item_type == ITEM_LIGHT
	      && obj->value[ 2 ].v > 0 )
	    {
		if ( --obj->value[ 2 ].v == 0 && ch->in_room )
		{
		    --ch->in_room->light;
		    act( ZM_WZROK | ZM_WID_OBJ1, "$p ga`snie.", ch, obj, NULL, TO_ROOM );
		    if ( IS_AWAKE( ch ) )
			act( ZM_WZROK | ZM_WID_OBJ1, "$p ga`snie.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		    if ( !ch->in_room->light ) /* zmienilismy z 1 na 0 */
			sprawdz_przyzwyczajanie( ch->in_room );
		}
	    }

	    if ( ++ch->timer >= 10
	      || ( !ch->desc && !IS_SWITCHED( ch ) ) ) /* Lam: po utracie polaczenia przyspieszone */
	    {
		if ( WP( ch )
		  && !ch->pcdata->was_in_room
		  && !ch->pcdata->odwleczenie /* niedoczekanie ;) */
		  && ch->in_room
		  && !ch->fighting )
		{
		    CHAR_DATA *przec;
		    /* Zdarzylo sie, ze ogluszony zapadl w nicosc, kiedy cos
		       z nim nadal walczylo, co to musiala byc za walka! ;) */
		    for ( przec = ch->in_room->people; przec; przec = przec->next_in_room )
			if ( is_fighting( przec, ch ) )
			    break;
		    if ( !przec )
		    {
			ch->pcdata->was_in_room = ch->in_room;
			send_to_char( "Zapadasz w nico`s`c.\n\r", ch );
			act( ZM_WZROK | ZM_WID_CHAR, "$n zapada w nico`s`c.",
			    ch, NULL, NULL, TO_ROOM );
			save_char_obj( ch );
			char_from_room( ch );
			char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		    }
		}
	    }

	    /* Lam 26.4.2008: w ogole wylaczam wywalanie polaczonych graczy
	       poza zabojcami. */
	    if ( ( !ch->desc || IS_SET( ch->act, PLR_ZABOJCA ) )
	      && ch->timer > 20 && !IS_SWITCHED( ch ) )
		ch_quit = ch;

	    /* Lam 15.12.2004: jesli 10 minut (20 godzin od 20) probowalo
	       go wywalic i nie dalo rady, prawdopodobnie lezy bez zycia i nie
	       moze zdechnac ani sie wyleczyc. Obecnie pozostawia to kruczek,
	       ze jesli ktos walczy 20 minut nie dotykajac klawiatury, bedzie
	       leczony jesli uda mu sie zostac pokiereszowanym. Jakie sa
	       szanse? :) */
	    if ( ch->hit < 1 && ch->timer > 40 && WP( ch ) && !FIGHTING( ch ) )
	    {
		ch->hit = UMIN( ch->hit + 1, UMAX( ch->hit, ch->max_hit ) );
		update_pos( ch );
	    }

	    /* trzezwiejemy wszedzie WP */
	    if ( WP( ch ) )
		gain_condition( ch, COND_DRUNK, -1 );
	    /* Lam 23.2.98: nie glodniejemy w takich miejscach */
	    if ( !IS_SET( ch->in_room->room_flags, ROOM_NOTE_READ )
	      && !IS_SET( ch->in_room->room_flags, ROOM_PURGATORY ) )
	    {
		gain_condition( ch, COND_FULL,
			( -1 - race_table[ ch->race ].hunger_mod ) );

		/* Lam, #1682 */
		if ( CZY_WODA( ch->in_room )
		  && IS_SET( race_table[ ch->race ].oddychanie, ODD_WODA ) )
		    gain_condition( ch, COND_THIRST, 1 );
		else
		    gain_condition( ch, COND_THIRST,
			( -1 - race_table[ ch->race ].thirst_mod ) );
	    }
	}
	else if ( !IS_NPC( ch )
	       && ( ch->level > LEVEL_HERO || ch->level < 0 )
	       && ch->desc
	       && WP( ch ) )
	{
	    /* Lam 2.5.98: Trzezwienie niesmiertelnych */
	    gain_condition( ch, COND_DRUNK, -1 );
	}

	if ( !IS_SET( ch->in_room->room_flags, ROOM_NOTE_READ ) )
	for ( paf = ch->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
		continue;
	    if ( paf->duration > 0 )
		paf->duration--;
	    else if ( paf->duration < 0 )
		;
	    else
	    {
		bool komunikat = FALSE;
		int sn = 0;

		if ( !paf->next
		  || paf->next->type != paf->type
		  || paf->next->duration > 0 )
		{
		    komunikat = TRUE;
		    sn = paf->type;
		}

		if ( paf->type == gsn_vampiric_bite )
		{
		    /* Lam 30.4.2001: zdjecie przemiany! */
		    affect_strip( ch, gsn_polymorph_other );
		    affect_strip( ch, gsn_postac_nietoperza );

		    /* Tissarius 14.12.2001: dobre postacie nie zostaja wampirami */
		    if ( ch->level < LEVEL_IMMORTAL
		      && ( IS_GOOD( ch )
			|| race_table[ ch->race ].parts != P_HS ) )
		    {
			STC( "Ugryzienia na szyi zaczynaj`a ci`e strasznie bole`c.\n\rCzujesz si`e bardzo `xle.\n\r", ch );
			if ( ch->position > POS_SLEEPING )
			{
			    if ( CZY_ZIEMIA( ch->in_room ) )
				act( ZM_WZROK | ZM_WID_CHAR, "$n osuwa si`e na ziemi`e i umiera od ugryzienia wampira.", ch, NULL, NULL, TO_ROOM );
			    else
				act( ZM_WZROK | ZM_WID_CHAR, "$n umiera od ugryzienia wampira.", ch, NULL, NULL, TO_ROOM );
			}
			ch->hit = -15;
			/* ch->position = POS_MORTAL; */
			damage( ch, ch, 100, TYPE_UNDEF_ZZ, WEAR_NONE, TRUE );
			break;
		    }
		    else
		    {
			if ( !IS_NPC( ch ) )
			{
			    char kto[ MAX_INPUT_LENGTH ];

			    ch->pcdata->wampiryzm.rasa = ch->race;
			    ch->pcdata->wampiryzm.ile++;
			    free_string( ch->pcdata->wampiryzm.strefa );
			    ch->pcdata->wampiryzm.strefa =
				str_dup( ch->pcdata->strefa->nazwa );
			    sprintf( kto, ch->pcdata->wampiryzm.pochodzenie,
				     ascdata( localtime( &current_time ) ) );
			    free_string( ch->pcdata->wampiryzm.pochodzenie );
			    ch->pcdata->wampiryzm.pochodzenie = str_dup( kto );
			}
			ch->race = zr_wampir;
			ch->alignment = UMIN( ch->alignment, 0 );
			STC( "Czujesz, `ze musisz napi`c si`e krwi!\n\r", ch );
		    }
		}

		if ( ( paf->type == gsn_znamie_smierci )
		   && ch->level < LEVEL_IMMORTAL )
		{
		    int obrazenia;

		    if ( ch->position > POS_SLEEPING )
		    {
			if ( CZY_ZIEMIA( ch->in_room ) )
			{
			    STC( "Nagle bezw`ladnie osuwasz si`e na ziemi`e!\n\r", ch );
			    act( ZM_WZROK | ZM_WID_CHAR, "Nagle $n osuwa si`e bezw`ladnie na ziemi`e!", ch, NULL, NULL, TO_ROOM );
			}
			else
			{
			    STC( "Nagle bezw`ladnie osuwasz si`e!\n\r", ch );
			    act( ZM_WZROK | ZM_WID_CHAR, "Nagle $n osuwa si`e bezw`ladnie!", ch, NULL, NULL, TO_ROOM );
			}
		    }
		    obrazenia = 3 * ch->hit;
		    if ( HAS_SANCTUARY( ch ) )
			obrazenia *= 2;
		    obrazenia = UMAX( obrazenia, 100 );
		    ch->hit = -9;
		    /* ch->position = POS_MORTAL; */
		    damage( ch, ch, obrazenia, TYPE_UNDEF_ZZ, WEAR_NONE, TRUE );
		    break;
		}

		if ( IS_NPC( ch ) && IS_SET_V( paf->bitvector, AFF_SLUGA )
		    && ch->master )
		    zabic_sluge = TRUE;

		if ( paf->type == gsn_disguise )
		{
		    char zapas[ MIL ];
		    strcpy( zapas, ch->long_descr );
		    free_string( ch->long_descr );
		    ch->long_descr = str_dup( ch->long_descr_orig );
		    free_string( ch->long_descr_orig );
		    ch->long_descr_orig = str_dup( zapas ); /* mozna dac str_dup("") */
		}                       /* dla jedn. opisow dac usuwanie false_name! */

		affect_remove( ch, paf, TRUE );
		/* check_race_wearlocs( ch ); */
		sprawdz_ubranie( ch );
		if ( paf->type == gsn_polymorph_other && CZY_WAMPIR( ch ) )
		    ch->alignment = UMIN( ch->alignment, 0 );

		/* Lam 20.11.2000: komunikat po zdjeciu wplywu (np. niewidki) */
		if ( komunikat && sn > 0 )
		{
		    if ( skill_table[ sn ].msg_off
		      && *skill_table[ sn ].msg_off )
		    {
			/* Lam 7.5.2000: STC -> ASTC */
			/* Lam 25.3.2006: ASTC -> act */
			act( ZM_PRZYT, skill_table[ sn ].msg_off, ch, NULL, NULL, TO_CHAR );
		    }
		    /* Lam 20.11.2000: spadanie wplywow widoczne w pomieszczeniu */
		    /* Lam 10.4.2004: ...ale nie jesli mam BN */
		    if ( skill_table[ sn ].msg_off_others
		      && *skill_table[ sn ].msg_off_others
		      && ( IS_NPC( ch )
			|| !IS_SET( ch->act, PLR_WIZINVIS ) ) )
		    {
			act( ZM_WZROK | ZM_WID_CHAR, skill_table[ sn ].msg_off_others, ch, NULL, NULL, TO_ROOM );
		    }
		    /* Niby moznaby to odblokowac dla wszystkich i kazac
		       w funkcjach sprawdzac BN, ale to za duzo sprawdzania.
		       Jesli ktos naprawde bedzie tego potrzebowal, dorobi
		       sobie jakies SKILL_COSTAM i tyle. */
		    if ( skill_table[ sn ].wear_off_fun
		      && ( IS_NPC( ch )
			|| !IS_SET( ch->act, PLR_WIZINVIS ) ) )
		    {
			( *skill_table[ sn ].wear_off_fun )( ch );
		    }
		}
	    }
	}
	if ( ch->deleted ) /* znamie smierci */
	    continue;

	/* powyzej 60 poziomu zajmie sie tym move_char */
	if ( is_affected( ch, gsn_postac_nietoperza )
	  && ch->in_room->area->time_info.hour > 6
	  && ch->in_room->area->time_info.hour < 20
	  && ch->level < 60 )
	{
	    STC( "Powracasz do swojej postaci.\n\r", ch );
	    affect_strip( ch, gsn_postac_nietoperza );
	}

	if ( !ch->deleted && IS_NPC( ch ) && ch->position >= POS_RESTING )
	    mprog_time_trigger( ch );
	/* Alandar */
	if ( !ch->deleted && IS_NPC( ch ) )
	    mprog_asleep_time_trigger( ch );

	if ( !ch->deleted && zabic_sluge ) extract_char( ch, TRUE );
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     * Lam: sprawdzanie, czy ch->desc->connected == CON_PLAYING jest dokonywane
     * powyzej, wyjatkowo nie skwasili ;)
     */
    if ( ch_save || ch_quit )
	for ( ch = char_list; ch; ch = ch->next )
	{
	    if ( ch->deleted )
		continue;
	    if ( ch == ch_save )
		save_char_obj( ch );
	    if ( ch == ch_quit )
		do_quit( ch, "zupe`lnie" );
	}

    return;
}


/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = object_list; obj; obj = obj_next )
    {
	CHAR_DATA *rch;
	char      *message;
	OBJ_DATA  *in_obj;

	obj_next = obj->next;
	if ( obj->deleted )
	    continue;

	if ( obj->timer < -1 )
	    obj->timer = -1;

	/* Lam 31.12.2007: ciche pozbywanie sie czesci cial mobow, ktore nigdy
	   sie nie rozkladaja (one sa typu skarb i maja vnum w w1), oczywiscie
	   pod warunkiem, ze nikt tego nie widzi. */
	if ( obj->item_type == ITEM_TREASURE
	  && obj->value[ 1 ].v > 0
	  && obj->in_room
	  && !obj->in_room->area->nplayer
	  && !number_bits( 3 ) )
	    obj->timer = 1;

	if ( obj->timer < 0 )
	    continue;

	/* Lam 23.12.2003: przedmioty nie rozkladaja sie PP */
	/* aby sprawdzic, czy przedmiot znajduje sie razem z graczem PP, musze
	   znalezc najbardziej zewnetrzny pojemnik */
	in_obj = obj;
	while ( in_obj && in_obj->in_obj )
	    in_obj = in_obj->in_obj;

	if ( ( in_obj->carried_by
	    && in_obj->carried_by->in_room
	    && IS_SET( in_obj->carried_by->in_room->room_flags, ROOM_NOTE_READ ) ) )
/*	  || ( in_obj->in_room
	    && IS_SET( in_obj->in_room->room_flags, ROOM_NOTE_READ ) )
	powyzsze wykomentowane - przeciez nie chcemy przedmiotow na ziemi PP,
	a jesli nawet jakis sie tam znajdzie, mozna mu pustawic czas
	zamiast byc zmuszonym do natychmiastowego jebuta */
	    continue;

	/*
	 *  Bug fix:  used to shift to obj_free if an object whose
	 *  timer ran out contained obj->next.  Bug was reported
	 *  only when the object(s) inside also had timer run out
	 *  during the same tick.     --Thelonius (Monk)
	 */
	if ( --obj->timer == 0 )
	{
	    switch ( obj->item_type )
	    {
		default:              message = (obj->rodzaj == 3) ? "$p znikaj`a." : "$p znika."; break;
		case ITEM_WEAPON:     message = "$j do ko`nca prze`zera rdza."; break;
		case ITEM_FOUNTAIN:   message = (obj->rodzaj == 3) ? "$p wysychaj`a." : "$p wysycha."; break;
		case ITEM_CORPSE_NPC: message = (obj->rodzaj == 3) ? "$p rozpadaj`a si`e w proch." : "$p rozpada si`e w proch."; break;
		case ITEM_CORPSE_PC:  message = (obj->rodzaj == 3) ? "$p rozpadaj`a si`e w proch!" : "$p rozpada si`e w proch!"; break;
		case ITEM_FOOD:       message = (obj->rodzaj == 3) ? "$p rozk`ladaj`a si`e." : "$p rozk`lada si`e."; break;
	    }

	    if ( obj->carried_by )
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, message, obj->carried_by, obj, NULL, TO_CHAR );
	    }
	    else if ( obj->in_room
		   && ( rch = obj->in_room->people ) )
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, message, rch, obj, NULL, TO_ROOM );
		/* Lam 25.9.2002 */
		act( ZM_WZROK | ZM_WID_OBJ1, message, rch, obj, NULL, TO_CHAR );
	    }

	    if ( obj == object_list )
	    {
		extract_obj( obj );
		obj_next = object_list;
	    }
	    else /* (obj != object_list) */
	    {
		OBJ_DATA *previous;

		for ( previous = object_list; previous; previous = previous->next )
		    if ( previous->next == obj )
			break;

		if ( !previous )  /* Can't see how, but... */
		    bug( "Obj_update: obj %d no longer in object_list",
			obj->pIndexData->vnum );

		extract_obj( obj );

		obj_next = previous->next;
	    }
	}
    }

    return;
}


/*
 * Lam 31.3.2002, tylko dla time_proga, docelowo chcialbym tu umiescic spadanie
 * czasowych wplywow pomieszczen i moze cos jeszcze?
 *
 * Spadanie wplywow dopisal Ulryk.
 */
void room_update( void )
{
    ROOM_INDEX_DATA *r;
    AFFECT_DATA     *paf;
    AFFECT_DATA     *paf_next;
    int              i;

    for ( i = 0; i < MAX_KEY_HASH; i++ )
	for ( r = room_index_hash[ i ]; r; r = r->next )
	{
	    rprog_time_trigger( r );
	    for ( paf = r->affected; paf; paf = paf_next )
	    {
		paf_next = paf->next;

		if ( paf->duration < 0 )
		    continue;

		if ( --paf->duration < 0 )
		{
		    CHAR_DATA *rch;

		    /* FIXME: brak obslugi msg_off_fun, potrzebuje tego splot
		       many (widac spadanie bez wykrycia magii) i strefa ciszy
		       (tu ZM_SLUCH) */
		    if ( skill_table[ paf->type ].msg_off
		      && *skill_table[ paf->type ].msg_off
		      && ( rch = r->people ) )
		    {
			act( ZM_WZROK, skill_table[ paf->type ].msg_off, rch, NULL, NULL, TO_CHAR );
			act( ZM_WZROK, skill_table[ paf->type ].msg_off, rch, NULL, NULL, TO_ROOM );
		    }

		    affect_from_room( r, paf->type );
		    sprawdz_przyzwyczajanie( r ); /* zgaslo ognisko */
		}
	    }
	}

    return;
}


void rrand_update( void )
{
    RRAND_DATA *prrand;

    for ( prrand = rrand_list; prrand; prrand = prrand->next )
	rprog_random_trigger( prrand->room );

    return;
}

/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes .2% of total CPU time.
 *
 * -Kahn
 */
void aggr_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *mch;
    CHAR_DATA *mch_next;
    CHAR_DATA *vch;
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    ROOM_INDEX_DATA *room;

    /*
     * Let's not worry about link dead characters. -Kahn
     */
    for ( d = descriptor_list; d; d = d->next )
    {
	ch = d->character;

	if ( d->connected != CON_PLAYING
	  || ch->level >= LEVEL_IMMORTAL
	  || !ch->in_room
	/* Qwert: w czysscu nikt nikogo nie atakuje? teraz moze :) */
	/*  || ch->in_room->area == area_czysciec */ )
	    continue;

	room = ch->in_room;

	/* mch wont get hurt -- jak to nie? Na koncu wywoluje multi_hit
	   i rzeczywiscie szanse, ze mob jest agresywny wobec kogos, kto jest go
	   w stanie zabic tarcza, sa nikle, ale jednak...
	   Dodatkowo dodalem tu proga, wiec juz musowo zabezpieczyc. */
	for ( mch = room->people; mch; mch = mch_next )
	{
	    int count;
	    bool hate = FALSE;

	    /* Wystarczy dac mppurge w aggr_attack_progu jednego z kilku mobow
	       w pomieszczeniu...
	       Break pominie to pomieszczenie zupelnie, chocby w nim zostaly
	       jakies moby - to zaden problem - zaatakuja w kolejnym pulsie,
	       a jak sie uda graczowi wymknac, to pogratulujemy. */
	    if ( mch->in_room != room )
		break;

	    mch_next = mch->next_in_room;

	    if ( !IS_NPC( mch )
	      || mch->deleted
	      || mch->fighting
	      || IS_AFFECTED( mch, AFF_CHARM )
	      || !IS_AWAKE( mch )
	      || ( IS_SET( mch->act, ACT_WIMPY ) && IS_AWAKE( ch ) )
	      || IS_SET( mch->act, ACT_PEACEFUL )
	      || !can_see( mch, ch )
	      || is_safe( mch, ch, FALSE ) ) /* Lam 9.4.99 */
		continue; /* rozbilem na 2 ify dla potrzeb nienawisci :) */

	    if ( mch->hating == ch )
	    {
		/* Lam 18.5.98 */
		do_say( mch, "Znowu si`e spotykamy! Teraz gi`n!" );
		multi_hit( mch, ch, TYPE_UNDEFINED );

		/* zaatakuje jeden w pulsie, ale nie bedzie problemu, ze kogos
		   zabil i potem multi_hit zglasza bledy */
		break;
	    }

	    /* Ulryk: prawa de'Morgana rzadza :) Teraz powinno juz dzialac bez
	       zarzutu */
	    if ( ( !IS_SET( mch->act, ACT_AGGRESSIVE )
		|| ( ch->level - mch->level > 10 ) )
	      && ( !is_name2( race_table[ ch->race ].ang,
				race_table[ mch->race ].hate )
		|| abs( mch->level - ch->level ) > 4
		|| IS_SET( mch->act, ACT_NOT_RACIST ) ) )
		continue;

/*            if ( is_name2( race_table[ ch->race ].ang,
			    race_table[ mch->race ].hate ) )
	    Lam 26.1.2004: hate = TRUE oznacza nie, ze po prostu kogos
	      nienawidzimy i tego tez szukajmy, ale, ze bedziemy szukac
	      tego, kogo nienawidzimy. Przy powyzszym warunku agresywny mob
	      nie byl agresywny wobec postaci nizszej poziomem o ponad 4
	      bedacej jednoczesnie rasy nienawidzonej przez moba.
*/
	    if ( !IS_SET( mch->act, ACT_AGGRESSIVE )
	      || ( ch->level - mch->level > 10 ) )
		hate = TRUE;

	    /*
	     * Ok we have a 'ch' player character and a 'mch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     *
	     * Lam: gracze bez polaczen i tak musza byc w limbo, zeby nie dostac.
	     */
	    count  = 0;
	    victim = NULL;
	    for ( vch = mch->in_room->people; vch; vch = vch->next_in_room )
	    {
		if ( IS_NPC( vch )
		  || vch->deleted
		  || vch->level >= LEVEL_IMMORTAL )
		    continue;

		if ( ( !IS_SET( mch->act, ACT_WIMPY ) || !IS_AWAKE( vch ) )
		    && can_see( mch, vch )
		    && !is_safe( mch, vch, FALSE) )
		{
		    if ( !hate
		      || ( hate
			&& is_name2( race_table[ vch->race ].ang,
					race_table[ mch->race ].hate )
			&& abs( mch->level - vch->level ) <= 4 ) )
		    {
			if ( number_range( 0, count ) == 0 )
			    victim = vch;
			count++;
		    }
		}
	    }

	    if ( !victim )
		continue;

	    /* Qwert */
	    if ( is_affected( victim, gsn_disguise )
	      && number_percent( ) < ch->pcdata->learned[ gsn_disguise ] * 3 / 5 )
		continue; /* uff! dal sie nabrac :) */

	    mprog_aggr_attack_trigger( mch, victim );

	    if ( mch->deleted || victim->deleted
	      || mch->in_room != room
	      || victim->in_room != room
	      || mch->fighting
	      || IS_AFFECTED( mch, AFF_CHARM )
	      || !IS_AWAKE( mch )
	      || !can_see( mch, ch )
	      || is_safe( mch, ch, FALSE ) )
		continue;

	    multi_hit( mch, victim, TYPE_UNDEFINED );
	    break; /* uwaga jak przy nienawisci */
	} /* mch loop */
    } /* descriptor loop */

    return;
}

/*
 * Dzielo Ulryka
 * Lam dodal zmienna murder_char, bo bardzo zajmowalo procesor
 */
void murders_update( void )
{
    CHAR_DATA *ch;

    if ( !murder_char )
	return;

    for ( ch = char_list; ch; ch = ch->next )
	if ( !IS_NPC( ch ) )
	{
	    REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_MORDERCA );
	    REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_ZABOJCA  );
	}

    murder_char = FALSE;

    return;
}


/*
 * Lam 15.6.98: przelatuje po mtrig_list i odpala wyzwalacze, jesli akurat
 * uzna, ze powinna
 */
void mtrig_update( void )
{
    MTRIG_DATA *trig, *trig_next, *trig_prev;

    for ( trig = mtrig_list; trig; trig = trig_next )
    {
	trig_next = trig->next;

	if ( --trig->time >= 0 ) continue;

/*	if ( !trig->actor ) trig->actor = trig->ch; */
	if ( trig->ch ) /* nie usuwac tej linii */
	    mprog_wordlist_check( trig->arg, trig->ch, trig->actor, NULL,
		trig->vict, NULL, M_TRIG_PROG, trig->ch->pIndexData->mobprogs, 0 );

	/* usuniecie wyzwalacza */
	if ( trig == mtrig_list )
	    mtrig_list = trig->next;
	else
	{
	    for ( trig_prev = mtrig_list; trig_prev && trig_prev->next != trig;
			trig_prev = trig_prev->next )
		;
	    trig_prev->next = trig_next;
	}

	free( trig ); /* tylko tyle */
    }

    return;
}


void licz_graczy2( int *play, int *desc )
{
    DESCRIPTOR_DATA *d;
    int ldesc = 0;
    int lplay = 0;

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected != CON_GET_NAME )
	    ldesc++;
	if ( d->connected == CON_PLAYING )
	    lplay++;
    }
    *play = lplay;
    *desc = ldesc;

    return;
}


void stats_update( void )
{
    static int bufor;
    int i;

    if ( bufor > 0 )
    {
	bufor--;
	return;
    }

    if ( current_time % 86400 == 0 )
    {
	bufor = 20;

	for ( i = MAX_DAYS - 1; i > 0; i-- )
	{
	    max_play.day[ i ] = max_play.day[ i - 1 ];
	    max_desc.day[ i ] = max_desc.day[ i - 1 ];
	}

	licz_graczy2( &max_play.day[ 0 ], &max_desc.day[ 0 ] );
    }

    return;
}


/* Update the check on time for autoshutdown */
void time_update( void )
{
    FILE            *fp;
    char             buf [ MAX_STRING_LENGTH ];
    int		min_tmp;
    int		sek_tmp;
    int		min_pom;
    int		sek_pom;

    if ( down_time <= 0 )
	return;

    min_tmp = ( down_time - current_time ) / 60;
    sek_tmp = ( down_time - current_time ) % 60;
    min_pom = liczba( min_tmp );
    sek_pom = liczba( sek_tmp );

    if ( current_time > warning1 && warning1 > 0 )
    {
	sprintf( buf, "{yPierwsze ostrze`zenie!{x\n\r{r%s{x za %d minut%s i %d sekund%s.\n\r",
		Reboot ? "Restart" : "Koniec gry",
		min_tmp,
		min_pom == 1 ? "`e" : min_pom == 2 ? "y" : "",
		sek_tmp,
		sek_pom == 1 ? "`e" : sek_pom == 2 ? "y" : "" );
	send_to_all_char( buf );
	warning1 = 0;
    }
    if ( current_time > warning2 && warning2 > 0 )
    {
	sprintf( buf, "{YDrugie ostrze`zenie!{x\n\r{R%s{x za %d minut%s %d sekund%s.\n\r",
		Reboot ? "Restart" : "Koniec gry",
		min_tmp,
		min_pom == 1 ? "`e" : min_pom == 2 ? "y" : "",
		sek_tmp,
		sek_pom == 1 ? "`e" : sek_pom == 2 ? "y" : "" );
	send_to_all_char( buf );
	warning2 = 0;
    }
    if ( current_time + 10 > down_time && warning2 == 0 )
    {
	sprintf( buf, "{YOstatnie ostrze`zenie!{x\n\r{R%s{x za 10 sekund!\n\r",
		Reboot ? "Restart" : "Koniec gry" );
	send_to_all_char( buf );
	warning2--;
    }
    if ( current_time > down_time )
    {
	sprintf( buf, "Mud %s si`e automatycznie.\n\r",
	    Reboot ? "restartuje" : "wy`l`acza" );
	send_to_all_char( buf );
	log_string( buf );

	if ( auction->item )
	{
	    sprintf( buf, "Aukcja %s zosta`la przerwana przez mud.",
			    auction->item->dopelniacz );
	    talk_auction( buf );
	    obj_to_char( auction->item, auction->seller );
	    auction->item = NULL;
	    auction->seller->gold += auction->zastaw;
	    send_to_char( "Pieni`adze z zastawu powracaj`a do ciebie.\n\r", auction->seller );
	    if ( auction->buyer != NULL && auction->buyer != auction->seller )
	    {
		auction->buyer->gold += auction->bet;
		send_to_char( "Pieni`adze powracaj`a do ciebie.\n\r",
		    auction->buyer );
	    }
	}

	merc_down = TRUE;
	end_of_game( );

	if ( !Reboot )
	{
	    zamknij_reserve( fpReserve );
	    if ( !( fp = fopen( SHUTDOWN_FILE, "a" ) ) )
	    {
		lac_perror( SHUTDOWN_FILE );
		bug( "Could not open the Shutdown file!", 0 );
	    }
	    else
	    {
		fprintf( fp, "Shutdown by System\n" );
		fclose( fp );
	    }
	    fpReserve = fopen( NULL_FILE, "r" );
	}
/* Przenioslem stad zeby wiznet nie kwasil
	merc_down = TRUE;
*/
    }

    return;
}


/*
 * Remove deleted EXTRA_DESCR_DATA from objects.
 * Remove deleted AFFECT_DATA from chars and objects.
 * Remove deleted CHAR_DATA and OBJ_DATA from char_list and object_list.
 */
void list_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    OBJ_DATA  *obj;
    OBJ_DATA  *obj_next;

    if ( delete_char )
	for ( ch = char_list; ch; ch = ch_next )
	{
	    AFFECT_DATA *paf;
	    AFFECT_DATA *paf_next;

	    for ( paf = ch->affected; paf; paf = paf_next )
	    {
		paf_next = paf->next;

		if ( paf->deleted || ch->deleted )
		{
		    AFFECT_DATA *prev;

		    if ( ch->affected == paf )
			ch->affected = paf->next;
		    else
		    {
			for ( prev = ch->affected; prev; prev = prev->next )
			    if ( prev->next == paf )
			    {
				prev->next = paf->next;
				break;
			    }

			if ( !prev )
			{
			    bug( "List_update: cannot find paf on ch.", 0 );
			    continue;
			}
		    }

		    paf->next   = affect_free;
		    affect_free = paf;
		    if ( paf->caster )
		    {
			for ( prev = ch->affected; prev; prev = prev->next )
			    if ( prev->caster == paf->caster )
				break;
			/* jesli to ostatni wplyw tej postaci */
			if ( !prev )
			    wywal_zwod( &paf->caster->zwod_affected, ch );
		    }
		}
	    }

	    ch_next = ch->next;

	    if ( ch->deleted )
	    {
		if ( ch == char_list )
		    char_list = ch->next;
		else
		{
		    CHAR_DATA *prev;

		    for ( prev = char_list; prev; prev = prev->next )
			if ( prev->next == ch )
			{
			    prev->next = ch->next;
			    break;
			}

		    if ( !prev )
		    {
			char buf [ MAX_STRING_LENGTH ];

			sprintf( buf, "List_update: char %s not found.",
				ch->name );
			bug( buf, 0 );
			continue;
		    }
		}

		free_char( ch, FALSE );
	    }
	}

    if ( delete_obj )
      for ( obj = object_list; obj; obj = obj_next )
      {
	  AFFECT_DATA      *paf;
	  AFFECT_DATA      *paf_next;

	  for ( paf = obj->affected; paf; paf = paf_next )
	  {
	      paf_next = paf->next;

	      if ( obj->deleted )
	      {
		  if ( obj->affected == paf )
		      obj->affected = paf->next;
		  else
		  {
		      AFFECT_DATA *prev;

		      for ( prev = obj->affected; prev; prev = prev->next )
			  if ( prev->next == paf )
			  {
			      prev->next = paf->next;
			      break;
			  }

		      if ( !prev )
		      {
			  bug( "List_update: cannot find paf on obj.", 0 );
			  continue;
		      }
		  }

		  paf->next   = affect_free;
		  affect_free = paf;
	      }
	  }

	  obj_next = obj->next;

	  if ( obj->deleted )
	  {
	      if ( obj == object_list )
		  object_list = obj->next;
	      else
	      {
		  OBJ_DATA *prev;

		  for ( prev = object_list; prev; prev = prev->next )
		      if ( prev->next == obj )
		      {
			  prev->next = obj->next;
			  break;
		      }

		  if ( !prev )
		  {
		      bug( "List_update: obj %d not found.",
			  obj->pIndexData->vnum );
		      continue;
		  }
	      }

	      free_string( obj->name );
	      free_string( obj->description );
	      free_string( obj->look_descr );
	      free_string( obj->short_descr );
	      free_string( obj->dopelniacz );
	      free_string( obj->celownik );
	      free_string( obj->biernik );
	      free_string( obj->narzednik );
	      free_string( obj->miejscownik );
	      free_string( obj->action );
	      free_string( obj->anti_class );
	      free_string( obj->anti_race );
	      free_string( obj->owner );
	      /* Alandar 06.10.2004: obsluga wartosci przedmiotow bedacych opisami */
	      if ( ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON
		  || obj->item_type == ITEM_POTION || obj->item_type == ITEM_PILL )
		&& obj->value[ 4 ].p )
	      {
		  char *lanc = (char *) obj->value[ 4 ].p;
		  free_string( lanc );
	      }

	      --obj->pIndexData->count;

	      obj->next = obj_free;
	      obj_free  = obj;
	  }
      }

    delete_obj  = FALSE;
    delete_char = FALSE;

    return;
}


/*
 * Update the ban file upon call.
 * Written by Tre of EnvyMud and modified by Kahn
 *
 * Przepisane przez Lama
 */
void ban_update( void )
{
    FILE      *fp;
    BAN_DATA  *pban;

    zamknij_reserve( fpReserve );

    if ( !( fp = fopen( BAN_FILE, "w" ) ) )
    {
	bug( "Ban_update:  fopen of BAN_FILE failed", 0 );
	return;
    }

    for ( pban = ban_list; pban; pban = pban->next )
	fprintf( fp, "%s~ %s~ %d %s~\n",
		pban->host, pban->user, pban->type, pban->banned );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


/* aukcja */
void auction_update( void )
{
    int tax, pay;
    char buf[ MAX_STRING_LENGTH ];

    switch ( ++auction->going ) /* increase the going state */
    {
	case 1 : /* going once */
	case 2 : /* going twice */
	    if ( auction->bet > auction->starting )
		sprintf( buf, "%s: %d po raz %s.", auction->item->short_descr,
			auction->bet, ( ( auction->going == 1 ) ? "pierwszy" : "drugi" ) );
	    else
		sprintf( buf, "%s po raz %s (nie otrzymano zg`losze`n).", auction->item->short_descr,
			( ( auction->going == 1 ) ? "pierwszy" : "drugi" ) );

	    talk_auction( buf );
	    break;

	case 3 : /* SOLD! */
	    if ( !auction->buyer && auction->bet )
	    {
		bug( "Auction code reached SOLD, with NULL buyer, but %d gold bid", auction->bet );
		auction->bet = 0;
	    }
	    if ( auction->bet > 0 && auction->buyer != auction->seller )
	    {
		switch ( auction->item->rodzaj )
		{
		    default:
			sprintf( buf, "%s sprzedane %s za %s.",
			    auction->item->short_descr,
			    auction->buyer->celownik,
			    ZLOTO_B( auction->bet ) );
			break;
		    case 1:
			sprintf( buf, "%s sprzedany %s za %s.",
			    auction->item->short_descr,
			    auction->buyer->celownik,
			    ZLOTO_B( auction->bet ) );
			break;
		    case 2:
			sprintf( buf, "%s sprzedana %s za %s.",
			    auction->item->short_descr,
			    auction->buyer->celownik,
			    ZLOTO_B( auction->bet ) );
			break;
		}
		talk_auction( buf );

		if ( auction->buyer->position == POS_SLEEPING )
		{
		    auction->buyer->position = POS_RESTING;
		    act( ZM_SLUCH, "Budzi ci`e krzyk aukcjonera.", auction->buyer,
			NULL, NULL, TO_CHAR );
		}
		act( ZM_WZROK | ZM_WID_OBJ1, "Aukcjoner pojawia si`e przed tob`a i wr`ecza ci $j.",
			auction->buyer, auction->item, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_OBJ1, "Aukcjoner pojawia si`e przed $4, i podaje $u $j.",
			auction->buyer, auction->item, NULL, TO_ROOM );

		if ( ( auction->buyer->carry_weight
		       + get_obj_weight( auction->item ) )
		     > can_carry_w( auction->buyer ) )
		{
		    act( ZM_ZAWSZE, "Nie uniesiesz $g.", auction->buyer, auction->item, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_OBJ1, "$n nie jest w stanie unie`s`c $g.", auction->buyer, auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->buyer->in_room );
		}
		else
		{
		    zmien_poziom( auction->item,  auction->buyer );
		    obj_to_char( auction->item, auction->buyer );
		}

		/* pay = (int)auction->bet * 0.9; */
		tax = auction->bet / 10;
		pay = (int) auction->bet - tax;
		auction->seller->gold += auction->zastaw;
		auction->seller->gold += pay; /* give him the money, tax 10 % */
		sprintf( buf, "Aukcjoner wyp`laca ci %s oraz zastaw, potr`acaj`ac %s.\n\r", ZLOTO_B( pay ), ZLOTO_B( tax ) );
		send_to_char( buf, auction->seller );
		auction->item = NULL; /* reset item */
	    }
	    else /* not sold */
	    {
		sprintf( buf, "Nie otrzymano zg`loszenia na %s - aukcja przedmiotu zosta`la zako`nczona.", auction->item->biernik );
		talk_auction( buf );
		/* nieprzytomnych nie bede wybudzal, zobacza aukcjonera,
		   ale nie chce mi sie i tego obslugiwac jakos specjalnie */
		if ( auction->seller->position == POS_SLEEPING )
		{
		    auction->seller->position = POS_RESTING;
		    act( ZM_SLUCH, "Budzi ci`e krzyk aukcjonera.", auction->seller,
			NULL, NULL, TO_CHAR );
		}
		act( ZM_WZROK | ZM_WID_OBJ1, "Aukcjoner pojawia si`e przed tob`a i zwraca ci $j.",
		      auction->seller, auction->item, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_OBJ1, "Aukcjoner pojawia si`e przed $4 i zwraca $u $j.",
		      auction->seller, auction->item, NULL, TO_ROOM );
		if ( ( auction->seller->carry_weight
		     + get_obj_weight( auction->item ) )
		   > can_carry_w( auction->seller ) )
		{
		    act( ZM_WZROK | ZM_WID_OBJ1, "Nie uniesiesz $g.", auction->seller,
			auction->item, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_OBJ1, "$n upuszcza $j, gdy`z jest za s`lab$0.",
			auction->seller, auction->item, NULL, TO_ROOM );
		    obj_to_room( auction->item, auction->seller->in_room );
		}
		else
		    obj_to_char( auction->item,auction->seller );
	    } /* else */
	    auction->item = NULL; /* clear auction */
    } /* switch */

    return;
} /* func */


/*
 * Lam 11.2.99
 */
void podpowiadaj( void )
{
    DESCRIPTOR_DATA *desc;

    for ( desc = descriptor_list; desc; desc = desc->next )
    {
	CHAR_DATA *ch = desc->character;

	if ( desc->connected
	  || !ch || IS_NPC( ch )
	  || !CZY_OPCJA( ch, OPC_HINT ) )
	    continue;

	if ( ch->pcdata->hint > MAX_HINT || !hint_table[ ch->pcdata->hint ] )
	{
	    ch->pcdata->hint = 0;
	    REMOVE_BIT( ch->pcdata->opcje, OPC_HINT );
	    bug( "Blad! To sie nie powinno zdarzyc! update.c, linia %d", __LINE__ );
	    continue;
	}

	send_to_char( "{GCzy wiesz, `ze...:{Y\n\r", ch );
	send_to_char( hint_table[ ch->pcdata->hint ], ch );
	send_to_char( "{x", ch );

	ch->pcdata->hint++;
	if ( !hint_table[ ch->pcdata->hint ] )
	{
	    ch->pcdata->hint = 0;
	    REMOVE_BIT( ch->pcdata->opcje, OPC_HINT );
	    send_to_char( "{GTo by`la ostatnia podpowied`x. Podpowiedzi zosta`ly wy`l`aczone.\n\rJe`sli chcesz znowu zobaczy`c podpowiedzi, napisz \"opcje +podpowied`x\".{x\n\r", ch );
	}
    }

    return;
}


/*
 * Lam 28.3.99
 * Alandar 20.11.2003: Niewielkie modyfikacje
 * Lam 27.9.2005: opadanie pod wode
 *
 * FIXME? nie ma wypornosci przedmiotow (pusta zamknieta skrzynia powinna sie
 * unosic na wodzie, a nawet wyplywac na powierzchnie (!), tonac po wlozeniu
 * kilku kilogramow i nie wyplywajac, jesli nawet pusta, ale otwarta)
 */
void airobj_update( void )
{
    AIROBJ_DATA *airobj = airobj_list;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *dest_room;
    CHAR_DATA *rch;
    char buf[ MIL * 2 ];

    while ( airobj )
    {
	obj = airobj->obj;

	if ( obj
	  && !obj->deleted
	  && obj->in_room
	  && obj->in_room->exit[ DIR_DOWN ]
	  && ( dest_room = obj->in_room->exit[ DIR_DOWN ]->to_room )
	  && IS_SET( obj->wear_flags, ITEM_TAKE )
	  && !IS_SET( obj->in_room->exit[ DIR_DOWN ]->exit_info, EX_CLOSED )
	  && ( CZY_POWIETRZE( obj->in_room )
	    || ( obj->in_room->sector_type == SECT_WATER_DEEP
	      && obj->item_type != ITEM_BOAT ) /* Lam 17.2.2006 */
	    || CZY_POD_WODA( obj->in_room ) ) )
	{
	    if ( ( rch = obj->in_room->people ) )
	    {
		sprintf( buf, "$p %spada%s w d`o`l.",
			CZY_POWIETRZE( obj->in_room ) ? "s" : "o",
			obj->rodzaj == 3 ? "j`a" : "" );
		act( ZM_WZROK | ZM_WID_OBJ1, buf, rch, obj, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_OBJ1, buf, rch, obj, NULL, TO_CHAR );
	    }

	    obj_from_room( obj );
	    obj_to_room( obj, dest_room );

/*	    airobj->wysokosc += dest_room->pojemnosc == 0 ? 3 : dest_room->pojemnosc; */

	    if ( ( rch = obj->in_room->people ) )
	    {
		sprintf( buf, "$p spada%s ci na g`low`e z g`ory.",
			obj->rodzaj == 3 ? "j`a" : "" );
		act( ZM_WZROK | ZM_WID_OBJ1, buf, rch, obj, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_OBJ1, buf, rch, obj, NULL, TO_CHAR );
	    }
	    airobj = airobj->next;
	}
	else
	{
	    AIROBJ_DATA *temp = airobj;
	    airobj = airobj->next;
	    del_airobj( temp );
	}
    }

    return;
}


/*
 * Alandar 12.11.2003: Lamowe airobj_update po gruntownych przerobkach
 * Lam 27.9.2005: Alandar naobiecywal, ale w koncu ja robie opadanie pod wode
 */
void airchar_update( void )
{
    AIRCHAR_DATA *airchar = airchar_list;
    CHAR_DATA *chr, *tchr, *dchr = NULL;
    ROOM_INDEX_DATA *from_room, *dest_room;
    OBJ_DATA *wield;
    int temp, dam;
    bool powinien;

    while ( airchar )
    {
	/* to aby w razie nie wylosowania nowego dchr nie zostawal stary */
	dchr = NULL;
	chr = airchar->chr;
	dam = 0;
	powinien = FALSE;
	from_room = dest_room = FALSE;

	/* jesli chr jest, nie lata, jest w powietrzu i jest wyjscie na dol */
	if ( chr
	  && !chr->deleted
	  && ( from_room = chr->in_room )
	  && from_room->exit[ DIR_DOWN ]
	  && ( dest_room = from_room->exit[ DIR_DOWN ]->to_room )
	  && !IS_SET( from_room->exit[ DIR_DOWN ]->exit_info, EX_CLOSED ) )
	{
	    /* tutaj warunki sa kopia check_airchar */
	    if ( CZY_POWIETRZE( from_room )
	      && !IS_FLYING( chr ) )
	    {
		powinien = TRUE;
		act( ZM_WZROK | ZM_WID_CHAR, "$n spada w d`o`l!", chr, NULL, NULL, TO_ROOM );
		act( ZM_PRZYT, "Spadasz w d`o`l!", chr, NULL, NULL, TO_CHAR );
	    }

	    if ( !powinien
	      && from_room->sector_type == SECT_WATER_DEEP
	      && !IS_FLYING( chr )
	      && !IS_SET( race_table[ chr->race ].poruszanie, POR_POD_WODA )
	      && !IS_SET( race_table[ chr->race ].poruszanie, POR_PO_WODZIE )
	      && !IS_AFFECTED( chr, AFF_KONTROLA_NAD_WODA )
	      && ( IS_NPC( chr )
		|| !chr->pcdata->learned[ gsn_plywanie ]
		|| !IS_AWAKE( chr )
		|| airchar->slaby_plywak )
	      && !jest_w_lodzi( chr ) )
	    {
		powinien = TRUE;
		act( ZM_WZROK | ZM_WID_CHAR, "$n opada pod wod`e!", chr, NULL, NULL, TO_ROOM );
		act( ZM_PRZYT, "Opadasz pod wod`e!", chr, NULL, NULL, TO_CHAR );
	    }

	    if ( !powinien
	      && CZY_POD_WODA( from_room )
	      && !IS_SET( race_table[ chr->race ].poruszanie, POR_POD_WODA )
	      && !IS_AFFECTED( chr, AFF_KONTROLA_NAD_WODA )
	      && ( IS_NPC( chr )
		|| !chr->pcdata->learned[ gsn_plywanie ]
		|| !IS_AWAKE( chr )
		|| airchar->slaby_plywak ) )
	    {
		powinien = TRUE;
		act( ZM_WZROK | ZM_WID_CHAR, "$n idzie na dno!", chr, NULL, NULL, TO_ROOM );
		act( ZM_PRZYT, "Idziesz na dno!", chr, NULL, NULL, TO_CHAR );
	    }
	}

	if ( powinien )
	{
	    bool slonce_przed, slonce_po;
	    bool tlen_przed, tlen_po;
	    bool oslepiony_przed;

	    /* z move_char */
	    slonce_przed = CZY_PALACE_SLONCE( chr );
	    tlen_przed = CZY_BRAK_TLENU( chr );
	    if ( !IS_NPC( chr ) && IS_SET( chr->pcdata->temp_flags, TEMPFLAG_BLIND ) )
		oslepiony_przed = TRUE;
	    else
		oslepiony_przed = FALSE;

	    /* if ( chr->fighting ) - zdarzylo sie, ze walczaca postac stracila
	       przytomnosc pod woda i opadla na dno, teraz bezwarunkowo
	       stop_fighting */
	    stop_fighting( chr, TRUE );
	    PRZERWIJ_KARMPOJ( chr, FALSE );
	    char_from_room( chr );
	    char_to_room( chr, dest_room );
	    if ( chr->desc && chr->position > POS_SLEEPING
	      && ( IS_NPC( chr )
		|| oslepiony_przed
		|| !IS_SET( chr->pcdata->temp_flags, TEMPFLAG_BLIND ) ) )
		do_look( chr, "auto" );
	    /* niech go nieco zdezorientuje, zeby nie mogl rzucic sobie latania
	       w powietrzu (wyzwalaczem), albo zmniejszyc wysokosc idac recznie
	       w dol :) */
	    chr->wait = UMAX( chr->wait, 2 );

	    /* tez z move_char */
	    slonce_po = CZY_PALACE_SLONCE( chr );
	    if ( slonce_przed != slonce_po )
	    {
		if ( slonce_po )
		    STC( "{Y`Swiat`lo pali twoj`a sk`or`e!{x\n\r", chr );
		else
		    STC( "`Swiat`lo przestaje ci`e parzy`c.\n\r", chr );
	    }

	    tlen_po = CZY_BRAK_TLENU( chr );
	    if ( tlen_przed != tlen_po )
	    {
		if ( tlen_po )
		    STC( "{RNie mo`zesz oddycha`c!{x\n\r", chr );
		else
		    STC( "`Lapiesz oddech.\n\r", chr );
	    }

	    airchar->slaby_plywak = FALSE;

	    if ( CZY_POWIETRZE( from_room ) )
	    {
		/* zlicza wysokosc na podstawie pojemnosci - domyslnie 3 */
		airchar->wysokosc += dest_room->pojemnosc == 0 ? 3 : dest_room->pojemnosc;

		/* tu jeszcze raz, ale oblicza pojemnosc pomieszczenia na uzytek wpadania
		   na inne postacie - jak juz beda wszystkie pojemnosci beda rozne od 0
		   to mozna to zamienic na temp = dest_room->pojemnosc; */
		temp = ( dest_room->pojemnosc == 0 ? 3 : dest_room->pojemnosc ) * 3;

		/* losuje postac na ktora wpadnie spadajaca postac */
		if ( !chr->deleted )
		    for ( tchr = chr->in_room->people; tchr; tchr = tchr->next_in_room )
		    {
			if ( number_range( 0, temp ) == 0
			  && chr != tchr
			  && !tchr->deleted
			  /* to zeby nie dalo sie krzywdzic pokojowych mobow */
			  && ( !IS_NPC( chr )
			    || !IS_SET( chr->act, ACT_PEACEFUL ) ) )
			{
			    /* im zrecznosc wieksza od 20 tym wieksza szansa na unikniecie */
			    if ( number_range( 15, get_curr_dex( tchr ) ) < 21 )
				dchr = tchr;
			    break;
			}
			temp--;
		    }

		/* przypadkowe wpadanie na inne postacie podczas spadania */
		if ( dchr )
		{
		    /* zapisuje dane z dchr na wypadek niefortunnego zejscia :) */
		    OBJ_DATA *dchr_wield1 = get_eq_char( dchr, WEAR_WIELD ),
			     *dchr_wield2 = get_eq_char( dchr, WEAR_WIELD_2 );
		    int dchr_con = get_curr_con( dchr );
		    bool ups = 0;

		    /* komunikaty dla postaci w pomieszczeniu */
		    act( ZM_WZROK | ZM_WID_CHAR, "$N spada z g`ory prosto na g`low`e $1!", dchr, NULL, chr, TO_NOTVICT );
		    if ( dchr->position != POS_SLEEPING )
			act( ZM_PRZYT, "$N spada z g`ory prosto na twoj`a g`low`e!", dchr, NULL, chr, TO_CHAR );
		    else
			act( ZM_ZAWSZE, "Czujesz mocne uderzenie w g`low`e!.", dchr, NULL, chr, TO_CHAR );
		    if ( chr->position != POS_SLEEPING )
			act( ZM_PRZYT, "Spadasz prosto na g`low`e $!.", chr, NULL, dchr, TO_CHAR );
		    else
			act( ZM_ZAWSZE, "Uderzasz o co`s.", chr, NULL, dchr, TO_CHAR );

		    dam = airchar->wysokosc;
		    wield = get_eq_char( chr, WEAR_WIELD );
		    /* 30% szans na zahaczenie o pierwsza bron */
		    if ( wield && number_percent( ) < 30  )
		    {
			dam += number_range( wield->value[ 1 ].v, wield->value[ 2 ].v ) * 50
				       / dchr->max_hit;
			ups = 1;
		    }
		    wield = get_eq_char( chr, WEAR_WIELD_2 );
		    /* i 30% szans na zahaczenie o druga bron */
		    if ( wield && number_percent( ) < 30 )
		    {
			dam += number_range( wield->value[ 1 ].v, wield->value[ 2 ].v ) * 50
				       / dchr->max_hit;
			ups = 1;
		    }
		    /* jesli spadajacy jest wiekszy, wieksze obrazenia dla postaci na ktora wpada */
		    dam += UMAX ( 0, get_curr_con( chr ) - get_curr_con( dchr ) );
		    /* no i zamiana na procenty */
		    dam *= dchr->max_hit;
		    dam /= 100;
		    dam = URANGE( 1, dam, 2000 );

		    if ( ups && dchr->position != POS_SLEEPING )
			act( ZM_WZROK, "$N zahacza ci`e broni`a.", dchr, NULL, chr, TO_CHAR );
		    ups = 0;

		    damage( chr, dchr, dam, gsn_upadek, WEAR_NONE, TRUE );
		    if ( dchr->position == POS_SLEEPING )
		    {
			dchr->position = POS_STANDING;
			do_look( dchr, "auto" );
		    }

		    /* jw. dla postaci spadajacej */
		    dam = airchar->wysokosc * 2;
		    if ( dchr_wield1 && number_percent( ) < 30  )
		    {
			dam += number_range( dchr_wield1->value[ 1 ].v, dchr_wield1->value[ 2 ].v ) * 50
				    / chr->max_hit;
			ups = 1;
		    }
		    if ( dchr_wield2 && number_percent( ) < 30 )
		    {
			dam += number_range( dchr_wield2->value[ 1 ].v, dchr_wield2->value[ 2 ].v ) * 50
				    / chr->max_hit;
			ups = 1;
		    }
		    dam += UMAX ( 0, dchr_con - get_curr_con( chr ) );
		    dam *= chr->max_hit;
		    dam /= 100;

		    if ( ups && chr->position != POS_SLEEPING )
			act( ZM_WZROK, "Zahaczasz o bro`n $!.", chr, NULL, dchr, TO_CHAR );
		}
		else
		    act( ZM_WZROK | ZM_WID_CHAR, "$n spada z g`ory.", chr, NULL, NULL, TO_ROOM );
	    }
	    else
		act( ZM_WZROK | ZM_WID_CHAR, "$n spada z g`ory.", chr, NULL, NULL, TO_ROOM );

	    if ( !chr->deleted && chr->in_room == dest_room )
		po_wejsciu( chr, FALSE );

	    /* bolesny koniec podrozy :) */
	    /* w wodzie opada sie lagodnie */
	    if ( !chr->deleted && chr->in_room == dest_room
	      && CZY_POWIETRZE( from_room )
	      && ( dest_room->sector_type != SECT_AIR
		|| !dest_room->exit[ DIR_DOWN ]
		|| !dest_room->exit[ DIR_DOWN ]->to_room
		|| IS_SET( dest_room->exit[ DIR_DOWN ]->exit_info, EX_CLOSED ) ) )
	    {
		int dambkp = dam;

		/* Nie chcemy, zeby zadne Vertimy zabijaly burmistrza
		   przywolywaniem nad fontanne */
		if ( IS_NPC( chr ) && IS_SET( chr->act, ACT_PEACEFUL ) )
		{
		    act( ZM_WZROK | ZM_WID_CHAR, "$n zatrzymuje si`e nagle i `lagodnie l`aduje.", chr, NULL, NULL, TO_ROOM );
		    act( ZM_PRZYT, "Jaka`s magiczna si`la ratuje ci`e od upadku.", chr, NULL, NULL, TO_CHAR );
		    continue;
		}
		/* podstawowa sprawa - wysokosc i podloze, na ktorym sie laduje */
		dam = airchar->wysokosc;

		/* Lam: nieloty ze skrzydlami moga neutralizowac sile upadku */
		if ( !IS_FLYING( chr )
		  && IS_AWAKE( chr )
		  && IS_SET( race_table[ chr->race ].parts, P_SKRZYDLA ) )
		{
		    dam--;
		    if ( !dam )
		    {
			act( ZM_WZROK | ZM_WID_CHAR, "$n l`aduje mi`ekko z ca`lych si`l trzepocz`ac skrzyd`lami.", chr, NULL, NULL, TO_ROOM );
			act( ZM_PRZYT, "L`adujesz mi`ekko z ca`lych si`l trzepocz`ac skrzyd`lami.", chr, NULL, NULL, TO_CHAR );
			continue;
		    }
		}

		dam *= obr_z_upadku[ UMIN( SECT_MAX, dest_room->sector_type ) ];

		dam += ( get_curr_con( chr ) - get_curr_dex( chr ) ) / 2;

		/* ladowanie na kims lagodzi upadek */
		if ( dchr )
		    dam /= 2;

		/* zamienam procenty na punkty zycia */
		dam *= chr->max_hit;
		dam /= 100;
		/* dodaje obrazenia od zahaczenia o bron, patrz wyzej */
		dam += dambkp;
		/* ograniczenie */
		dam = URANGE( 5, dam, 2000 );

		if ( dest_room->sector_type == SECT_WATER_SHALLOW
		  || dest_room->sector_type == SECT_WATER_DEEP
		  || dest_room->sector_type == SECT_UNDERWATER )
		    /* wprowadzamy nieco losowosci */
		    damage( chr, chr, number_range( UMAX( 1, dam - dam / 10 ), UMIN( dam + dam / 10, 2000 ) ),
			gsn_uderzenie_w, WEAR_NONE, TRUE );
		else if ( dest_room->sector_type == SECT_AIR )
		    damage( chr, chr, number_range( UMAX( 1, dam - dam / 10 ), UMIN( dam + dam / 10, 2000 ) ),
			gsn_upadek, WEAR_NONE, TRUE );
		else
		    damage( chr, chr, number_range( UMAX( 1, dam - dam / 10 ), UMIN( dam + dam / 10, 2000 ) ),
			gsn_uderzenie_o, WEAR_NONE, TRUE );
		if ( chr->position == POS_SLEEPING )
		{
		    chr->position = POS_STANDING;
		    do_look( chr, "auto" );
		}
	    }

	    airchar = airchar->next;
	}
	else
	{
	    AIRCHAR_DATA *to_delete = airchar;
	    airchar = airchar->next;
	    del_airchar( to_delete );
	}
    }

    return;
}


/*
 * Lam 9.1.2003, 16.1.2003
 */
void sentinel_update( void )
{
    CHAR_DATA *ch, *ch_next;
    ROOM_INDEX_DATA *prev_room;
    int kier;
    char bufcio[ MIL ];

    for ( ch = char_list; ch; ch = ch_next )
    {
	ch_next = ch->next;

	if ( ch->deleted
	  || ch->fighting
	  || !IS_NPC( ch )
	  || !IS_SET( ch->act, ACT_SENTINEL )
	  || ch->position <= POS_RESTING
	  || IS_SET( ch->act, ACT_DONT_COME_BACK )
	  || IS_AFFECTED( ch, AFF_HOLD )
	  || IS_AFFECTED( ch, AFF_CHARM )
	  || ch->desc /* Lam 18.5.2004 */ )
	    continue;

	if ( !ch->reset_room
	  && !IS_SET( ch->act, ACT_PET ) ) /* (mp)mload */
	    continue;

	if ( ch->reset_room
	  && ( ch->in_room == ch->reset_room ) )
	    continue;

	/* Biedak, wyszedl z domu. */
	/* Lam 12.10.2004: zeby ograniczyc pozeranie procesora przez
		find_path oraz czasem ograniczyc wracanie dwoch sklepikarzy
		do jednego sklepu, mob nie bedzie wracal, jesli nikt tego
		nie widzi, po prostu sobie zaginie w boju :) */
	if ( !ch->in_room->area->nplayer && !number_bits( 2 ) )
	{
	    wiznet( "[BZDET] $N jest w krainie bez graczy, znika ukradkiem ze `swiata.", ch, NULL, NULL, WIZ_DEBUG, 0, 106 );
	    extract_char( ch, TRUE );
	    continue;
	}

	if ( !ch->reset_room ) /* czyli kupione ACT_PET, nie maja dokad wracac */
	    continue;

	/* Sprobujmy wrocic. Wrocimy nawet z innej strefy, nawet przez
	   pomieszczenia NO_MOB */
	kier = find_path( ch->in_room, ch->reset_room,
			40000, FALSE, TRUE );

	if ( kier < 0 || kier > 9 )
	{
	    if ( ( ch->pIndexData->pShop
		|| ch->pIndexData->pHealer )
	      && !IS_SET( ch->act, ACT_DONT_COME_BACK ) )
	    {
		wiznet( "[BZDET] $N nie mo`ze wr`oci`c, tworzy portal.", ch, NULL, NULL, WIZ_DEBUG, 0, 106 );
		STC( "Modlisz si`e o powr`ot do miejsca pracy...\n\r", ch );
		act( ZM_WZROK | ZM_WID_CHAR, "$n wznosi oczy ku niebu.", ch, NULL, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR, "Nagle powietrze zaczyna wirowa`c wok`o`l $1, kt`or$y zdaje si`e w nim rozp`lywa`c...", ch, NULL, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR, "$n znika zupe`lnie.", ch, NULL, NULL, TO_ROOM );
		char_from_room( ch );
		char_to_room( ch, ch->reset_room );
		act( ZM_WZROK | ZM_WID_CHAR, "Nagle powietrze zaczyna wirowa`c, powoli ukazuj`ac posta`c $1!", ch, NULL, NULL, TO_ROOM );
		continue;
	    }
	    wiznet( "[BZDET] $N nie mo`ze wr`oci`c, w`l`acza ACT_DONT_COME_BACK.", ch, NULL, NULL, WIZ_DEBUG, 0, 106 );
	    SET_BIT( ch->act, ACT_DONT_COME_BACK );
	    continue;
	}

	prev_room = ch->in_room;
	move_char( ch, kier, FALSE );

	if ( ch->deleted )
	    continue;

	if ( prev_room == ch->in_room )
	{
	    sprintf( bufcio, "[BZDET] $N nie ruszy`l$A si`e z miejsca #%d, cho`c chce.", prev_room->vnum );
	    wiznet( bufcio, ch, NULL, NULL, WIZ_DEBUG, 0, 106 );
	    if ( ( ch->pIndexData->pShop
		|| ch->pIndexData->pHealer )
	      && !IS_SET( ch->act, ACT_DONT_COME_BACK ) )
	    {
		wiznet( "[BZDET] $N nie ruszy`l$A si`e z miejsca, tworzy portal.", ch, NULL, NULL, WIZ_DEBUG, 0, 106 );
		STC( "Modlisz si`e o powr`ot do miejsca pracy...\n\r", ch );
		act( ZM_WZROK | ZM_WID_CHAR, "$n wznosi oczy ku niebu.", ch, NULL, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR, "Nagle powietrze zaczyna wirowa`c wok`o`l $1, kt`or$y zdaje si`e w nim rozp`lywa`c...", ch, NULL, NULL, TO_ROOM );
		act( ZM_WZROK | ZM_WID_CHAR, "$n znika zupe`lnie.", ch, NULL, NULL, TO_ROOM );
		char_from_room( ch );
		char_to_room( ch, ch->reset_room );
		act( ZM_WZROK | ZM_WID_CHAR, "Nagle powietrze zaczyna wirowa`c, powoli ukazuj`ac posta`c $1!", ch, NULL, NULL, TO_ROOM );
		continue;
	    }
	}
	else
	{
	    if ( ch->in_room == ch->reset_room )
		wiznet( "[BZDET] $N ruszy`l$A si`e z miejsca, jest w domu.", ch, NULL, NULL, WIZ_DEBUG, 0, 106 );
	    else
		wiznet( "[BZDET] $N ruszy`l$A si`e z miejsca.", ch, NULL, NULL, WIZ_DEBUG, 0, 106 );
	}
    }

    return;
}


/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 *
 * Lam: teraz jest mniej losowe
 */
int pulse_point; /* Lam 26.1.2004: wywalone poza funkcje */
int pulse_point_max; /* Lam: do obliczania minut czasu gry */

void update_handler_before( void )
{
    plynne_odradzanie( );

    return;
}


void update_handler( void )
{
    static int pulse_area;
    static int pulse_mobile;
    static int pulse_violence;
    static int pulse_pits_save;
    static int pulse_hint;
    static const char numer_godz[ 24 ][ 30 ] =
		{ "zerowa", "pierwsza", "druga", "trzecia", "czwarta",
		  "pi`ata", "sz`osta", "si`odma", "`osma", "dziewi`ata",
		  "dziesi`ata", "jedenasta", "dwunasta", "trzynasta",
		  "czternasta", "pi`etnasta", "szesnasta", "siedemnasta",
		  "osiemnasta", "dziewi`etnasta", "dwudziesta",
		  "dwudziesta pierwsza", "dwudziesta druga",
		  "dwudziesta trzecia" };

    if ( --pulse_area <= 0 )
    {
	pulse_area = number_range( 3 * PULSE_AREA / 4, 5 * PULSE_AREA / 4 );
	area_update( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence = PULSE_VIOLENCE;
	violence_update( );
	sentinel_update( );
    }

    if ( --pulse_mobile <= 0 )
    {
	pulse_mobile = number_range( PULSE_MOBILE - PULSE_PER_SECOND,
				PULSE_MOBILE + PULSE_PER_SECOND );
	mobile_update( );
	rrand_update( );
    }

    if ( --pulse_point <= 0 )
    {
	DESCRIPTOR_DATA *d;
	AREA_DATA *kr;
	char buf[ MAX_STRING_LENGTH ];

	pulse_point	= number_range( 3 * PULSE_TICK / 4, 5 * PULSE_TICK / 4 );
	pulse_point_max	= pulse_point;

	/* Lam */
	for ( kr = area_first; kr; kr = kr->next )
	{
	    if ( kr->time_info.hour == 23 )
		strcpy( buf, "Min`e`la p`o`lnoc.\n\r" );
	    else if ( kr->time_info.hour == 11 )
		strcpy( buf, "Min`e`lo po`ludnie.\n\r" );
	    else
		sprintf( buf, "Min`e`la godzina %s.\n\r", numer_godz[ kr->time_info.hour + 1 ] );

	    for ( d = descriptor_list; d; d = d->next )
	    {
		/* Lam 10.3.2006: "keepalive" co pol minuty dla tanich domowych
		   ruterkow, ktore zapominaja o polaczeniach juz po kilku
		   minutach od przeslania ostatnich danych (zwlaszcza jesli sa
		   inne komputery w sieci, ktore generuja inne polaczenia do
		   tablicy NAT-u).
		   To powoduje, ze ida pakiety Z DANYMI w obie strony nawet,
		   jesli ktos jest PP i nie widzi godzin, opc -wchodzenie
		   (skarzyla sie na to Miri, skarzyl sie Nyd, pewnie nie tylko
		   oni mieli takie problemy). */
		if ( d->connected == CON_PLAYING
		  && d->character->in_room->area == kr
		  && d->idle > 29 )
		{
		    write_to_descriptor( d->descriptor, (char *) keepalive_str, 0 );

		    if ( CZY_OPCJA( d->character, OPC_SHOWTICK )
		      && WP( d->character ) )
		    {
			send_to_char( buf, d->character );
		    }
		}
	    }
	    weather_update( kr );
	}

	/* czas Midgaardu */
	time_info = get_room_index( ROOM_VNUM_TEMPLE )->area->time_info;
	weather_info = get_room_index( ROOM_VNUM_TEMPLE )->area->weather_info;

	mud_age++;

	char_update( );
	obj_update( );
	room_update( );
	list_update( );
    }

    if ( auction->item && --auction->pulse <= 0 )
    {
	auction->pulse = PULSE_AUCTION;
	auction_update( );
    }

    if ( --pulse_pits_save <= 0 )
    {
	write_clans( );
	zapisz_urny( );

	/* Lam 10.4.98: taka czestotliwosc swobodnie wystarczy przy obecnej
	   ilosci graczy, moze ze 2 glosy w glosowaniu zniklyby w przypadku pada.
	   I tak nie ma padow */
	rewrite_all_notes( );
	zapisz_ustawienia( ); /* Lam 10.1.2003: do zapisywania czasu */
	pulse_pits_save = PULSE_PER_SECOND * 900; /* kwadrans */
    }

    if ( --pulse_hint <= 0 )
    {
	podpowiadaj( );
	pulse_hint = PULSE_PER_SECOND * 60; /* 1 podpowiedz na minute */
    }

    stats_update( );
    time_update( );
    mtrig_update( ); /* Lam */
    timer_update( ); /* Lam */
    aggr_update( );
    /* tu bylo plynne_odradzanie */
    murders_update( ); /* Ulryk */
    airobj_update( ); /* Lam */
    airchar_update( ); /* Alandar */
#if defined( DEBUG_SSM )
    oznacz_wszystko( );
    zrzut_kwasow_lancuchow( );
#endif
    tail_chain( );

    return;
}

/* Ciekawostka: czasem miesza weather_update, dlatego godziny sa w tym samym
   miejscu, natomiast time_update sluzy do autorebootow */
