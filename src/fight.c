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

extern bool murder_char; /* Lam, update.c */

/*
 * Local functions.
 */
bool    check_dodge		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_parry		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
/* Gimza: check_doom dotyczy gsn_zaglada */
bool    check_doom		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	sprawdz_blok_tarcza	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    dam_message		args( ( CHAR_DATA *ch, CHAR_DATA *victim,
					int dam, int dt, int wpn ) );
bool    is_wielding_specweapon	args( ( CHAR_DATA *ch, int wpn, int flag ) );
void    make_corpse		args( ( CHAR_DATA *ch ) );
void    set_fighting_main	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    disarm			args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    trip			args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

bool    check_race_special	args( ( CHAR_DATA *ch ) );
void    use_magical_item 	args( ( CHAR_DATA *ch ) );
void	split2			args( ( CHAR_DATA *ch, int argument ) );
void	zdejmij_niewidki_przy_ataku args( ( CHAR_DATA *ch ) );
void	stop_fighting_all	args( ( CHAR_DATA *ch ) );
void	make_no_corpse		args( ( CHAR_DATA *ch ) );
bool	registered		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	ocen_czar		args( ( int v ) );
bool	czy_w_grze		args( ( CHAR_DATA *ch, const char *imie ) );


/*
 * Do zapisywania przebiegu zabojstw/morderstw w kronice, zapamietuje ostatnio
 * uzyte polecenie "morduj". Taki jeden globalny bufor wystarczy w zupelnosci,
 * poniewaz morderstw i tak nie da sie zagniezdzac. --Lam
 */
char ostatnie_morduj[ 2 * MIL ];


/*
 * Control the fights going on.
 * Called periodically by update_handler.
 * Slightly less efficient than Merc 2.2.  Takes 10% of
 *  total CPU time.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    ROOM_INDEX_DATA *wpomi;
    bool mobfighting;

    for ( ch = char_list; ch; ch = ch->next )
    {
	if ( !( wpomi = ch->in_room ) || ch->deleted )
	    continue;

	if ( ( victim = FIGHTING( ch ) ) )
	{
	    if ( IS_AWAKE( ch ) && victim->in_room == wpomi
	      && !victim->deleted )
	    {
		/* Ok here we test for switch if victim is charmed */
		/* Lam 9.1.2003: nie przenosi sie ze slugi na pana, jesli pan
		   jest ogluszony (prawdopodobnie przez ch) */
		if ( IS_AFFECTED( victim, AFF_CHARM )
		  && victim->master
		  && victim->master->in_room == wpomi
		  && can_see( ch, victim->master )
		  && !is_safe( ch, victim->master, FALSE )
		  && IS_AWAKE( victim->master )
		  && number_percent( ) < 20 ) /* bylo > 40 czyli 60% */
		{
		    set_fighting_main( ch, victim->master );
		    if ( !is_fighting( victim->master, ch ) )
			set_fighting( victim->master, ch );
		    multi_hit( ch, victim->master, TYPE_UNDEFINED );
		}
		else
		{
		    multi_hit( ch, victim, TYPE_UNDEFINED );
		}

		if ( ch->deleted
		  || ch->in_room != wpomi
		  || victim->in_room != wpomi )
		    continue;
		mprog_hitprcnt_trigger( ch, victim );

		if ( ch->deleted
		  || ch->in_room != wpomi
		  || victim->in_room != wpomi )
		    continue;
		mprog_fight_trigger( ch, victim );
	    }
	    else
	    {
		sprintf( bug_buf, "ch '%s' walczy wciaz z kims, z kim nie powinien (%s/%s/%s)",
		    ch->name,
		    IS_AWAKE( ch ) ? "..." : "ch nieprzytomny",
		    victim->in_room == wpomi ? "..." : "ofiara nie w pom",
		    victim->deleted ? "ofiara skasowana" : "..." );
		bug( bug_buf, 0 );
		stop_fighting_char( ch, victim );
	    }
	    continue;
	}

	/* Lam 26.12.2003, dla ochrony trolla i starszego wampira */
	if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PEACEFUL ) )
	    continue;

	/* Lam 9.1.2003: !IS_AWAKE( ch ), wczesniej postacie z -10 punktow
	   zycia mogly budzic sie chcac pomagac innym w walce */
	if ( !IS_AWAKE( ch )
	  || IS_AFFECTED( ch, AFF_BLIND )
	  || ( IS_NPC( ch ) && ch->pIndexData->pShop )
	  || ( IS_NPC( ch ) && ch->pIndexData->pHealer ) )
	    continue;

	/*
	 * Tu wiadomo, ze ch nie walczy. Sprawdzimy, czy nie powinien wlaczyc
	 * sie do jakiejs innej walki. Gracze wlaczaja sie do walk grup graczy
	 * (pomoc grupa), a moby do grup graczy, grup mobow i ewentualnie do
	 * walk mobow podobnych poziomem i moralnoscia.
	 */
	mobfighting = FALSE;

	for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	{
	    if ( rch->deleted
	      || !( victim = FIGHTING( rch ) )
	      || !can_see( ch, victim ) )
		continue;

	    /*
	     * Przypadek: "grupa graczy", zrobiona poleceniem "grupa", moga
	     * w niej byc gracze i oczarowane moby.
	     */
	    if ( is_same_group( ch, rch ) )
		break;

	    /*
	     * Przypadek: wzajemna pomoc mobow w walce z graczem.
	     */
	    if ( IS_NPC( ch )
	      && !IS_AFFECTED( ch, AFF_CHARM ) /* nie bylo */
	      && IS_NPC( rch )
	      && !IS_NPC( victim )
		/* Lam: blad w Envy - kupowane zwierzatka atakowaly pana */
	      && !is_same_group( ch, victim ) )
	    {
		mobfighting = TRUE;
		break;
	    }
	}

	if ( !rch || !victim )
	    continue;

	/*
	 * Tu juz wiemy, ze chcemy wlaczyc sie do walki.
	 *
	 * ch - nie walczaca postac (mob lub gracz)
	 * rch - walczaca postac, ktorej ch moze zechciec pomoc
	 * victim - osoba, z ktora walczy rch
	 *
	 * Jesli ch i rch sa mobami, victim graczem, a w pomieszczeniu jest
	 * wiecej postaci w grupie victim, ch moze sie zdecydowac zaatakowac
	 * inna postac z owej grupy, zeby utrudnic jej natarcie.
	 * Ustawiona zmienna mobfighting znaczy dokladnie taka sytuacje.
	 */
	if ( mobfighting )
	{
	    CHAR_DATA *vch;
	    int number;

	    number = 0;
	    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	    {
		if ( can_see( ch, vch )
		  && is_same_group( vch, victim )
		  && number_range( 0, number ) == 0 )
		{
		    victim = vch;
		    number++;
		}
	    }

	    /*
	     * Moby bezwarunkowo pomagaja innym mobom z danej grupy mobow.
	     * W innych przypadkach musza uznac to za stosowne.
	     */
	    if ( ( !rch->pIndexData->grupa
		|| rch->pIndexData->grupa != ch->pIndexData->grupa
		|| IS_SET( ch->act, ACT_NIE_POMAGA ) )
	      && ( ( rch->pIndexData != ch->pIndexData && number_bits( 3 ) != 0 )
		|| ( IS_GOOD( ch ) && IS_GOOD( victim ) )
		|| abs( victim->level - ch->level ) > 5 ) )
		continue;
	}

	if ( !is_safe( ch, victim, FALSE ) )
	    multi_hit( ch, victim, TYPE_UNDEFINED );
    }

    return;
}


/*
 * Wyprowadza serie atakow.
 *
 * Uwaga: w odroznieniu od Envy, w Lacu nie wolno uzyc multi_hit do "odwetu
 * za atak" w kierunku ofiara->agresor bez uprzedniego set_fighting
 * w odpowiedniej kolejnosci. Kod jest pelen przykladow, z ktorych mozna
 * skorzystac.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    ROOM_INDEX_DATA *fight_room;
    FIGHT_DATA *fight, *fight_next;
    CHAR_DATA *victim2;
    int chance;
    bool spec_ras = FALSE; /* Lam 11.5.98 */

    if ( ch->deleted )
    {
	bug( "multi_hit: ch deleted!", 0 );
	return;
    }

    if ( victim->deleted )
    {
	bug( "multi_hit: victim deleted!", 0 );
	return;
    }

    if ( victim->position == POS_DEAD )
    {
	bug( "multi_hit: dead victim?", 0 );
	return;
    }

    if ( !( fight_room = ch->in_room ) )
    {
	bug( "multi_hit: NULL ch->in_room!", 0 );
	return;
    }

    if ( fight_room != victim->in_room )
    {
	bug( "multi_hit: victim not in room with ch!", 0 );
	return;
    }

    if ( !ZDOLNY_DO_WALKI( ch ) )
    {
	bug( "multi_hit: ch niezdolny do walki", 0 );
	return;
    }

    /* Gimza: check_doom sprawdza czy ch juz walczy, wiec musi to byc tutaj
       przed set_fighting (Lam: teraz set_fighting dopiero w damage) */
    if ( !IS_NPC( ch )
      && ch->klasa == CLASS_PALADYN
      && moze_uzyc( ch, gsn_zaglada )
      && !is_affected( ch, gsn_snare ) )
	check_doom( ch, victim );

    /* Qwert: cios z techniki smoka sprawia, ze ch jest zdezorientowany */
    if ( IS_SET_V( ch->affected_by, AFF_DISORIENTATED ) )
    {
	REMOVE_BIT_V( ch->affected_by, AFF_DISORIENTATED );
	return;
    }

    if ( !IS_NPC( ch ) && ch->fighting )
    {
	spec_ras = check_race_special( ch );

	/* Lam 13.12.2001: moze ktos zginac */
	if ( ch->deleted
	  || ch->in_room != fight_room )
	    return;
    }

    if ( !victim->deleted
      && victim->in_room == fight_room )
    {
	one_hit( ch, victim, dt, WEAR_WIELD );

	if ( ch->deleted
	  || ch->in_room != fight_room )
	    return;
    }

    if ( dt == gsn_backstab
      || dt == gsn_circle )
	return;

    if ( !victim->deleted
      && victim->in_room == fight_room
      && FIGHTING( ch ) == victim ) /* ogluszony ch przestaje walczyc */
    {
	chance = IS_NPC( ch ) ? ch->level
	    : ch->pcdata->learned[ gsn_second_attack ]
	     / ( ( IS_NPC( victim ) || IS_NPC( ch ) ) ? 2 : 4 );
	if ( number_percent( ) < chance )
	{
	    uzyj( ch, gsn_second_attack, "dwuatakowo`sci" );
	    one_hit( ch, victim, dt, WEAR_WIELD );
	    if ( ch->deleted
	      || ch->in_room != fight_room )
		return;
	    /* uzywam spec_ras takze jako zmarla_ofiara */
	    if ( victim->deleted
	      || victim->in_room != fight_room
	      || FIGHTING( ch ) != victim )
		spec_ras = TRUE;
	}

	if ( !spec_ras )
	{
	    chance = IS_NPC( ch ) ? ch->level / 2
				  : ch->pcdata->learned[ gsn_third_attack ] / 4;
	    if ( number_percent( ) < chance )
	    {
		uzyj( ch, gsn_third_attack, "tr`ojatakowo`sci" );
		one_hit( ch, victim, dt, WEAR_WIELD );
		if ( ch->deleted
		  || ch->in_room != fight_room )
		    return;
		if ( victim->deleted
		  || victim->in_room != fight_room
		  || FIGHTING( ch ) != victim )
		    spec_ras = TRUE;
	    }
	}

	if ( !spec_ras )
	{
	    chance = IS_NPC( ch ) ? ch->level / 2
				  : ch->pcdata->learned[ gsn_fourth_attack ] / 4;
	    if ( number_percent( ) < chance )
	    {
		uzyj( ch, gsn_fourth_attack, "czw`oratakowo`sci" );
		one_hit( ch, victim, dt, WEAR_WIELD );
		if ( ch->deleted
		  || ch->in_room != fight_room )
		    return;
		if ( victim->deleted
		  || victim->in_room != fight_room
		  || FIGHTING( ch ) != victim )
		    spec_ras = TRUE;
	    }
	}

	if ( !spec_ras )
	{
	    /* niech moby maja wiecej atakow */
	    chance = IS_NPC( ch ) ? ch->level / 4 : 0;
	    if ( number_percent( ) < chance )
	    {
		one_hit( ch, victim, dt, WEAR_WIELD );
		if ( ch->deleted
		  || ch->in_room != fight_room )
		    return;
		if ( victim->deleted
		  || victim->in_room != fight_room
		  || FIGHTING( ch ) != victim )
		    spec_ras = TRUE;
	    }
	}

	if ( !spec_ras )
	{
	    if ( get_eq_char( ch, WEAR_WIELD_2 )
	      || !( IS_NPC( ch )
		|| get_eq_char( ch, WEAR_SHIELD )
		|| get_eq_char( ch, WEAR_HOLD )
		|| get_eq_char( ch, WEAR_LIGHT ) ) )
	    {
		chance = IS_NPC( ch ) ? ch->level : ch->pcdata->learned[ gsn_dual ] / 2;
		if ( number_percent( ) < chance )
		{
		    uzyj( ch, gsn_dual, "u`zywaniu drugiej broni" );
		    one_hit( ch, victim, dt, WEAR_WIELD_2 );
		    if ( ch->deleted
		      || ch->in_room != fight_room )
			return;
		    /* tu juz victim nie ma znaczenia; dopisac w razie dodawania
		       jeszcze jakichs atakow! */
		}
	    }
	}
    }

    /* Lam 26.2.2006 */
    if ( is_affected( ch, gsn_berserk ) )
    {
	for ( fight = ch->fighting; fight; fight = fight_next )
	{
	    fight_next = fight->next;

	    victim2 = fight->vch;
	    /* sprawdzmy, czy fight_next jest nadal tego gracza */
	    if ( !victim2 /* zrobil przeskok na liste fight_free, moze wystapic */
	      || !is_fighting( ch, victim2 ) /* zrobil przeskok i wrocil na liste kogos innego */ )
		break;
	    if ( victim2 == victim ) /* ten juz raz dostal */
		continue;

	    /* 75% szans na probe ataku pierwsza bronia */
	    if ( number_bits( 2 ) )
	    {
		one_hit( ch, victim2, dt, WEAR_WIELD );

		if ( ch->deleted
		  || ch->in_room != fight_room )
		    return;
		if ( victim2->deleted
		  || victim2->in_room != fight_room )
		    continue;
	    }

	    /* maksymalnie 33% szans na probe ataku druga bronia */
	    chance = IS_NPC( ch ) ? ch->level : ch->pcdata->learned[ gsn_dual ] / 3;
	    if ( chance
	      && ( get_eq_char( ch, WEAR_WIELD_2 )
		|| !( IS_NPC( ch )
		  || get_eq_char( ch, WEAR_SHIELD )
		  || get_eq_char( ch, WEAR_HOLD )
		  || get_eq_char( ch, WEAR_LIGHT ) ) )
	      && number_percent( ) < chance )
	    {
		one_hit( ch, victim2, dt, WEAR_WIELD_2 );

		if ( ch->deleted
		  || ch->in_room != fight_room )
		    return;
	    }
	}
    }

    return;
}


/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int wpn )
{
    OBJ_DATA *wield;
    char      buf [ MAX_STRING_LENGTH ];
    int       tt; /* trudnosc trafienia */
    int       ca; /* celnosc ataku */
    int       thac0_00;
    int       thac0_47;
    int       dam;
    int       diceroll;
    int       test;
    int       premia_rasowa;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( ch->deleted )
    {
	sprintf( buf, "one_hit: ch %s, victim %s, ch deleted!",
		ch->name, victim->name );
	bug( buf, 0 );
	return;
    }

    if ( victim->deleted )
    {
	sprintf( buf, "one_hit: ch %s, victim %s, victim deleted!",
		ch->name, victim->name );
	bug( buf, 0 );
	return;
    }

    if ( victim->position == POS_DEAD )
    {
	sprintf( buf, "one_hit: ch %s, victim %s, victim POS_DEAD",
		ch->name, victim->name );
	bug( buf, 0 );
	return;
    }

    if ( !ch->in_room )
    {
	sprintf( buf, "one_hit: ch %s, victim %s, ch in NULL room!",
		ch->name, victim->name );
	bug( buf, 0 );
	return;
    }

    if ( !victim->in_room )
    {
	sprintf( buf, "one_hit: ch %s, victim %s, victim in NULL room!",
		ch->name, victim->name );
	bug( buf, 0 );
	return;
    }

    if ( ch->in_room != victim->in_room )
    {
	sprintf( buf, "one_hit: ch %s (room %d) not with victim %s (room %d)",
		ch->name, ch->in_room->vnum, victim->name, victim->in_room->vnum );
	bug( buf, 0 );
	return;
    }

    if ( !ZDOLNY_DO_WALKI( ch ) )
    {
	sprintf( buf, "one_hit: ch %s, victim %s, ch niezdolny do walki!",
		ch->name, victim->name );
	bug( buf, 0 );
	return;
    }

    if ( is_affected( ch, gsn_zaglada ) )
    {
	send_to_char( "Strach parali`zuje twoje dzia`lania.\n\r", ch );
	return;
    }

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, wpn );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[ 3 ].v;
    }

    /*
     * Porazajacy dotyk nekromanty
     */
     if ( ( !wield ) && dt == TYPE_HIT
	 && ch->klasa == CLASS_NEKROMANTA
	 && moze_uzyc( ch, gsn_porazajacy_dotyk )
	 && number_percent( ) <
	    URANGE( 0, ch->pcdata->learned[ gsn_porazajacy_dotyk ], 95 ) )
	dt = gsn_porazajacy_dotyk;

    /*
     * Figure out the racial hit bonuses.
     */
    premia_rasowa = ( ROZMIAR( ch ) - ROZMIAR( victim ) ) * 2;

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC( ch ) )
    {
	thac0_00 =  15;
	thac0_47 =  50;
    }
    else
    {
	thac0_00 = class_table[ ch->klasa ].thac0_00;
	thac0_47 = class_table[ ch->klasa ].thac0_47;
    }

    /* Ulryk: celnosc ataku */
    ca = interpolate( ch->level, thac0_00, thac0_47 )
       + dex_app[ get_curr_dex( ch ) ].tohit
       + LICZ_BU( ch, wpn )
       + UMAX( 0, 15 - ch->level ); /* Lam: przyspieszenie walk na poczatku */

    /* Lam: ulatwienie gry nowicjuszom */
    if ( !IS_NPC( ch ) )
    {
	ca += UMAX( 0, 10 - ch->level );
	/* wojownicy dostaja drugie ataki i uniki, magowie czary kosztujace
	   50 many, a zabierajace 1 zycia. Na razie dostana taka pomoc: */
	if ( class_table[ ch->klasa ].fMana )
	    ca += UMAX( 0, 10 - ch->level );
    }

    if ( !can_see( ch, victim ) )
    {
	if ( !IS_NPC( ch )
	  && number_percent( ) < ch->pcdata->learned[ gsn_walka_na_oslep ] / 2 )
	{
	    ca -= 15;
	    uzyj( ch, gsn_walka_na_oslep, "walce na o`slep" );
	}
	else
	    ca -= 20;
    }

    if ( ch == victim )
	ca += 20;

    ca  = UMAX( 0, ca );

    /* Ulryk: trudnosc trafienia */
    tt  = dex_app[ get_curr_dex( victim ) ].defensive
      + premia_rasowa - LICZ_MINUS_TT( victim );

    if ( IS_AFFECTED( victim, AFF_HOLD ) )
	tt /= 3;

    if ( IS_AFFECTED( victim, AFF_SZAL_BOJOWY )
      || !IS_AWAKE( victim )
      || ch == victim ) /* Lam, #1313 */
	tt = 0;

    /* to czar miedzywymiarowy, wiec stan i uwaga postaci na to nie wplywa */
    if ( IS_AFFECTED( victim, AFF_PRZEMIESZCZENIE ) )
	tt += 20;

    tt = UMAX( 0, tt );

    /* Ulryk: rzut za trafienie */
    diceroll = number_percent( );
    test = URANGE( 6, ca - tt,  90 );
    if ( diceroll >= test )
    {
	/* pudlo */
	damage( ch, victim, 0, dt, wpn, FALSE );
	tail_chain( );
	return;
    }

    /*
    sprintf( log_buf, "%s vs %s: dr: %d, ca: %d, tt: %d", ch->short_descr, victim->short_descr, diceroll, ca, tt );
    log_string( log_buf );
    */

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC( ch ) )
    {
	dam = number_range( 3 * ch->level / 2, ch->level * 3 );
	if ( wield )
	    dam += dam / 2;
    }
    else /* Qwert: ingeruje w podstawy, zeby mniej lub wiecej na koncu bylo */
    {
	if ( wield )
	{
	    dam = number_range( wield->value[ 1 ].v, wield->value[ 2 ].v );
	    if ( ch->klasa == CLASS_SKRYTOBOJCA )
		dam -= dam / 10; /* patrz: komentarz ponizej */
	}
	else /* na niskich poziomach lepiej od broni */
	{
	    dam = number_range( 1, 3 ) * ch->level / 20;

	     /* skrytobojcy lepiej walcza rekami niz bronia */
	    if ( ( number_percent( )
	       < ch->pcdata->learned[ gsn_mighty_hands ] / 3 )
	      && ch->klasa == CLASS_SKRYTOBOJCA )
	    {
		dam += dam * ch->pcdata->learned[ gsn_mighty_hands ] / ( 150 - ch->level );
		uzyj( ch, gsn_mighty_hands, "zadawaniu zwi`ekszonych obra`ze`n r`ekoma" );
	    }
	}

	if ( wield && dam > 200 )
	{
	    sprintf( buf, "One_hit dam range > 200 from %d to %d",
		wield->value[ 1 ].v, wield->value[ 2 ].v );
	    cbug( buf, 0 );

	    dam = 0; /* :) */
	    /* Wersja dla oszustow:
	    send_to_char( "{GLam {RNIE TOLERUJE{G takich broni! W zwi`azku z tym nie zadajesz obra`ze`n.{x\n\r",
		 ch );
	    wiznet( "Lam nakazuje: {M$N{x ma {RNATYCHMIAST{x zosta`c pozbawion$y broni!",
		ch, NULL, NULL, 0, 0, 103 );
	    return; */
	}
    }

    /*
     * Bonuses.
     */

    dam += ( str_app[ get_curr_str( ch ) ].todam
	   * ROZMIAR( ch ) / ( ( wield ) ? 6 : 12 ) );
    dam += LICZ_BO( ch, wpn );

    /* Lam 5.2.2006: zatrute dzialaja mocniej tylko na rasy reagujace na
       trucizny */
    if ( wield
      && IS_OBJ_STAT( wield, ITEM_POISONED )
      && !IS_SET( race_table[ victim->race ].race_abilities, RACE_POISONIMM ) )
	dam += dam / 4;
    /* Lam 5.2.2006: w zamian wampiry beda cierpiec jeszcze mocniej od broni
       zrobionych ze srebra (lub innego materialu o takim dzialaniu) */
    if ( wield
      && wield->item_type == ITEM_WEAPON
      && material_table[ wield->value[ 5 ].v ].vampburn
      && CZY_WAMPIR( victim ) )
	dam += dam / 3;

    if ( !IS_NPC( ch )
      && ch->klasa == CLASS_PALADYN
      && ch->pcdata->learned[ gsn_swiety_msciciel ] > 0
      && ( ( victim->race == zr_martwiak
	  || CZY_WAMPIR( victim )
	  || IS_EVIL( victim ) )
	|| ( !IS_NPC( victim )
	  && ( IS_SET( victim->act, PLR_KILLER )
	    || IS_SET( victim->act, PLR_THIEF ) ) ) ) )
    {
	dam += dam * ch->pcdata->learned[ gsn_swiety_msciciel ] / 120;
	uzyj( ch, gsn_swiety_msciciel, "t`epieniu z`la" );
    }
    else if ( !IS_NPC( ch ) && ch->pcdata->learned[ gsn_enhanced_damage ] > 0
      && dt != gsn_backstab && dt != gsn_strike
      && number_percent( ) < ch->pcdata->learned[ gsn_enhanced_damage ] )
    {
	dam += dam * ch->pcdata->learned[ gsn_enhanced_damage ] / 150;
	uzyj( ch, gsn_enhanced_damage, "zadawaniu zwi`ekszonych obra`ze`n" );
    }
    if ( !IS_AWAKE( victim ) )
	dam *= 2;
    if ( dt == gsn_backstab || dt == gsn_strike )
    {
	dam *= 2 + UMIN( ( ch->level / 8 ), 6 ); /* Lam: w Envy 4 */
	if ( dt == gsn_strike && number_percent ( ) <= ch->pcdata->learned[ gsn_assasinate ] * 2 / 5 )
	{
	    /* Qwert: wydaje mi sie, ze uzylem dosc rozsadnych liczb */
	    /* do 120% strike na poz. 100 */
	    dam += ( 5 * dam / 50 ) * ( 5 * ch->level / 250 );
	    uzyj( ch, gsn_assasinate, "zab`ojczo celnym ciosie" );
	}
    }
    else if ( dt == gsn_circle ) /* 150% to 200% at lev. 50 */
	dam += dam / 2 + ( dam * ch->level ) / 100;
    else if ( dt == gsn_porazajacy_dotyk )
    {
	uzyj( ch, gsn_porazajacy_dotyk, "u`zywaniu pora`zaj`acego dotyku" );
	dam += ( UMIN( ch->pcdata->learned[ dt ], 2 * ch->level ) + ch->level ) / 3;
    }
    if ( CZY_WAMPIR( ch ) && !wield )
	dam += dam / ( 2 + ( IS_NPC( ch ) ? 0 : ch->pcdata->wampiryzm.moc ) );

    if ( dam <= 0 )
	dam = 1;

    damage( ch, victim, dam, dt, wpn, FALSE );

    tail_chain( );

    return;
}


/*
 * Lam: to wersja do_split, ktora jako parametr dostaje inta, a nie wskaznik na
 * CHAR_DATA. Oprocz tego rozni sie nieznacznie
 */
void split2( CHAR_DATA *ch, int argument )
{
    CHAR_DATA *gch;
    char       buf [ MAX_STRING_LENGTH ];
    int        members;
    int        amount;
    int        share;
    int        extra;

    amount = argument;

    if ( amount < 0 )
    {
	send_to_char( "Twojej grupie by si`e to nie spodoba`lo.\n\r", ch );
	return;
    }

    if ( amount == 0 )
	return;

    if ( ch->gold < amount )
    {
	send_to_char( "Nie masz tyle z`lota.\n\r", ch );
	return;
    }

    members = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	/* Lam 14.1.2004: nie dzieli sie z mobami */
	if ( gch->deleted
	  || IS_NPC( gch ) )
	    continue;
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members < 2 )
    {
/*	send_to_char( "Nie masz z kim si`e dzieli`c.\n\r", ch ); */
	return;
    }

    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
/*	send_to_char( "Nawet si`e nie fatyguj dzieleniem tego.\n\r", ch ); */
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
	/* Lam 14.1.2004 */
	if ( gch->deleted
	  || IS_NPC( gch ) )
	    continue;
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( ZM_PRZYT, buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }

    return;
}


/*
 * Wydzielone z damage( ), zeby powtorzyc po ewentualnym use_magical_item
 */
void zdejmij_niewidki_przy_ataku( CHAR_DATA *ch )
{
    /*
     * Inviso attacks ... not.
     */
    if ( IS_AFFECTED( ch, AFF_INVISIBLE ) )
    {
	affect_strip( ch, gsn_invis      );
	affect_strip( ch, gsn_mass_invis );
	REMOVE_BIT_V( ch->affected_by, AFF_INVISIBLE );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nabiera kszta`lt`ow i staje si`e widzialn$y.", ch, NULL, NULL, TO_ROOM );
    }

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
    {
	affect_strip( ch, gsn_hide      );
	affect_strip( ch, gsn_chameleon );
	REMOVE_BIT_V( ch->affected_by, AFF_HIDE );
    }

    if ( is_affected( ch, gsn_disguise ) )
    {
	char zapas[ MIL ];

	strcpy( zapas, ch->long_descr );
	free_string( ch->long_descr );
	ch->long_descr = str_dup( ch->long_descr_orig );
	free_string( ch->long_descr_orig );
	/* mozna dac str_dup( "" ) dla jednorazowego opisu */
	ch->long_descr_orig = str_dup( zapas );
	affect_strip( ch, gsn_disguise );

	STC( "Przestajesz si`e maskowa`c.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nagle ukazuje swoje prawdziwe oblicze.", ch, NULL, NULL, TO_ROOM );
    }

    return;
}



/*
 * Inflict damage from a hit.
 *
 * Lam 13.1.2004: bool czy_bez_walki, wtedy obrazenia traktowane sa jakby
 * bylo damage( victim, victim, ... ), ale komunikat nie.
 *
 * Lam 9.1.2009: czy_bez_walki trzeba tez ustawic wykrwawianiu i innym
 * autoobrazeniom, ktore nie maja rozpoczynac walki i nie maja sprawdzac
 * is_safe( ch, ch ), w przeciwnym wypadku gracz musialby wpisac "morduj", zeby
 * dostac obrazenia od nieudanego wywazenia, upadku itd.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int wpn,
		bool czy_bez_walki )
{
    ROOM_INDEX_DATA *in_room = ch->in_room;
    bool pudlo = FALSE;
    int pozycja = victim->position;
    bool czy_dodac_zgon = FALSE;
    bool czy_zwykly_zgon = FALSE;

    if ( dt == TYPE_UNDEF_ZZ )
    {
	dt = TYPE_UNDEFINED;
	czy_zwykly_zgon = TRUE;
    }

    if ( victim->position == POS_DEAD )
    {
	bug( "victim in POS_DEAD", 0 );
	return;
    }

    if ( victim->deleted )
    {
	bug( "victim deleted", 0 );
	return;
    }

    if ( ch->deleted )
    {
	bug( "ch deleted", 0 );
	return;
    }

    if ( !victim->in_room )
    {
	bug( "victim in NULL room", 0 );
	return;
    }

    if ( !ch->in_room )
    {
	bug( "ch in NULL room", 0 );
	return;
    }

    if ( !czy_bez_walki && ch->in_room != victim->in_room )
    {
	bug( "ch not with victim", 0 );
	return;
    }

    if ( dam < 0 )
    {
	bug( "dam == %d < 0", dam );
	dam = 0;
    }

    if ( !dam )
	pudlo = TRUE;

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 4000 )
	dam = 4000;

    if ( !czy_bez_walki && ch != victim )
    {
	TIMER_DATA *timer;

	PRZERWIJ_KOPANIE( ch, TRUE );

	if ( ( timer = find_timer( ch, przestan_karmic ) )
	  || ( timer = find_timer( ch, przestan_poic ) ) )
	{
	    del_timer( timer );
	    timer->fun( timer );
	}
    }

    if ( !czy_bez_walki && victim != ch )
    {
	int tmpdam = dam;

	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim, TRUE ) )
	    return;
	check_killer( ch, victim );

	/* ch tez moze nie byc zdolny do walki w sytuacji, kiedy dogorywajac
	   "odbija" ciosy tarczami - wtedy damage( ) wywoluje jeden z czarow,
	   ktory znowu wywoluje damage( ) z ogluszonym ch */
	if ( ZDOLNY_DO_WALKI( ch )
	  && !is_fighting( ch, victim ) )
	    set_fighting( ch, victim );

	if ( ZDOLNY_DO_WALKI( victim )
	  && !is_fighting( victim, ch ) )
	    set_fighting( victim, ch );

	/*
	 * If NPC victim is following, ch might attack victim's master.
	 * No charm check here because charm would be dispelled from
	 * tanking mobile when combat ensues thus ensuring PC charmer is
	 * not harmed.
	 *
	 * Check for is_same_group wont work as following mobile is not
	 * always grouped with PC charmer - Kahn
	 *
	 * Lam 9.1.2003: IS_AWAKE( )
	 * Lam 23.4.2006: can_see( ), is_safe( )
	 */
	if ( IS_NPC( ch )
	  && IS_NPC( victim )
	  && victim->master
	  && can_see( ch, victim->master )
	  && !is_safe( ch, victim->master, FALSE )
	  && IS_AWAKE( victim->master )
	  && victim->master->in_room == ch->in_room
	  && number_bits( 6 ) == 0 )
	{
	    set_fighting_main( ch, victim->master );
	    if ( ZDOLNY_DO_WALKI( victim )
	      && !is_fighting( victim->master, ch ) )
		set_fighting( victim->master, ch );
	    return;
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

	zdejmij_niewidki_przy_ataku( ch );

	/*
	 * Lam 16.5.98: moja wizja wykonania polowania/nienawisci/strachu
	 * zupelnie (acz nie skrajnie) rozna od smaugowej
	 */
	if ( dam && IS_NPC( victim ) && ch != victim
	  && !IS_SET( victim->act, ACT_NO_TRACK ) )
	{
	    if ( !IS_SET( victim->act, ACT_SENTINEL ) )
	    {
		if ( victim->hunting )
		    wywal_zwod( &victim->hunting->zwod_hunting, victim );
		victim->hunting = ch; /* bedzie scigal w razie ucieczki */
		dod_zwod( &ch->zwod_hunting, victim );
	    }
	    if ( victim->hating )
		wywal_zwod( &victim->hating->zwod_hating, victim );
	    victim->hating = ch; /* zaatakuje w razie powrotu */
	    dod_zwod( &ch->zwod_hating, victim );
	}

	/*
	 * Damage modifiers.
	 */
	dam /= 2;

	dam -= (int) ( dam * con_app[ get_curr_con( victim ) ].wyparowanie );
	if ( dt != gsn_porazajacy_dotyk
	  && ( dt >= TYPE_HIT || !skill_table[ dt ].spell_fun ) )
	    dam = LICZ_WYPAROWANIE_KP( victim, dam );
	else
	    dam = LICZ_WYPAROWANIE_OPC( victim, dam );

	if ( HAS_SANCTUARY( victim ) )
	    dam /= 2;

	/* Lam: dobrzy bogowie chronia przed zlem nawet zlych, zli bogowie
	   chronia przed dobrem tylko zlych */
	if ( HAS_PROTECTION( victim )
	  && IS_EVIL( ch ) )
	    dam -= dam / 4;

	if ( HAS_PROTECT_GOOD( victim )
	  && IS_EVIL( victim )
	  && IS_GOOD( ch ) )
	    dam -= dam / 4;

	if ( !IS_NPC( ch ) || IS_NPC( victim ) )
	    if ( 100 * ch->hit / ch->max_hit
	       - 100 * victim->hit / victim->max_hit > 40 )
		dam = UMIN( tmpdam / 3, dam );

	if ( !dam && ( tmpdam > 0 ) )
	    dam = 1;

/*      if ( dam > 0 )
	{
	    sprintf( log_buf, "%s vs %s: dam: %d, tmpdam: %d (dt: %d)", ch->short_descr, victim->short_descr, dam, tmpdam, dt );
	    log_string( log_buf );
	} Ulryk: przydatne przy testach, wiec na razie jedynie wykomentowalem */

	if ( dam <= 0 )
	{
	    dam = 0;
	    pudlo = TRUE;
	}

	/*
	 * Check for disarm, trip, parry, and dodge.
	 *
	 * FIXME: odkad dziala ->wait dla mobow, da sie to przepisac, zeby
	 * dzialalo tak samo, jak u graczy. Nalezy to zrobic.
	 */
	if ( dt >= TYPE_HIT || dt == gsn_kick || dt == gsn_porazajacy_dotyk )
	{
	    int leveldiff = ch->level - victim->level;

	    /* Lam: tu byla wieksza szansa, ze rozbroi mnie lub podetnie mob
	       10 poziomow nizej niz 10 poziomow wyzej ode mnie */
	    if ( IS_NPC( ch ) && ch != victim && ( ch->level > 5 )
	      && number_percent( )
		 < ( leveldiff < -5
		     ? UMIN( 15, UMAX( 1, ch->level / 2 + leveldiff / 2 ) )
		     : UMAX( 5, ch->level / 10 + leveldiff ) )
	      && dam == 0
	      && ( get_eq_char( ch, WEAR_WIELD )
		|| get_eq_char( ch, WEAR_WIELD_2 ) ) )
		disarm( ch, victim );
	    /* Lam: tu bylo nawet gorzej niz z rozbrajaniem. Wiekszosc graczy
	       na wysokich poziomach radzilo sobie lataniem, ale nie zmienia
	       to faktu, ze tu byl blad */
	    if ( IS_NPC( ch ) && ch != victim
	      && number_percent( )
		 < ( leveldiff < -5
		     ? UMIN( 15, UMAX( 3, ch->level / 2 + leveldiff ) )
		     : UMAX( 10, leveldiff ) )
	      && dam == 0 )
		trip( ch, victim );
	    if ( IS_NPC( ch )
	      && IS_SET( race_table[ ch->race ].race_abilities,
			 RACE_WEAPON_WIELD )
	    /* Lam: tu bylo wszystko sporo wieksze, wiec moby zbyt czesto
	       uzywaly rzeczy */
	      && number_percent( ) < UMIN( 15, UMAX( 1, ch->level / 4 ) )
	      && !IS_NPC( victim ) )
		use_magical_item( ch );
	    /* Lam 16.10.2002: use_magical_item moze czasem zabic victim,
	       a takze ch, i to w zadnym wypadku nie jest blad */
	    if ( ch->deleted || victim->deleted
	      || ch->in_room != victim->in_room
	      || ch->in_room != in_room )
		return;
	    /* use_magical_item moze byc recytacja zwoju niewidzialnosci
	       Lam 4.6.2007 */
	    zdejmij_niewidki_przy_ataku( ch );
	    if ( dam > 0 )
	    {
		if ( IS_NPC( victim ) )
		{
		    if ( !get_eq_char( victim, WEAR_WIELD )
		      && !get_eq_char( victim, WEAR_WIELD_2 ) )
		    {
			if ( check_dodge( ch, victim ) )
			    return;
		    }
		    else if ( dt != gsn_porazajacy_dotyk
			   && check_parry( ch, victim ) )
			return;
		}
		else
		{
		    if ( dt != gsn_porazajacy_dotyk
		      && check_parry( ch, victim ) )
			return;
		    if ( dt != gsn_porazajacy_dotyk
		      && sprawdz_blok_tarcza( ch, victim ) )
			return;
		    if ( check_dodge( ch, victim ) )
			return;
		}
	    }
	}

	if ( !IS_NPC( victim )
	  && dam > 0
	  && IS_AFFECTED( victim, AFF_ANIOL_STROZ )
	  && victim->pcdata->hp_angel > 0
	  && victim->hit > 0 )
	{
	    if ( IS_EVIL( victim ) )
	    {
		affect_strip( victim, gsn_aniol_stroz );
		send_to_char( "Anio`l odwraca si`e od ciebie.\n\r", victim );
		act( ZM_WZROK | ZM_WID_CHAR, "Anio`l chroni`acy $3 znika.", victim, NULL, NULL, TO_ROOM );
	    }
	    else
	    {
		if ( dt >= 0 && dt < MAX_SKILL )
		{
		    act( ZM_ZAWSZE, "Anio`l os`lania ci`e przed czarem $1.", ch, NULL, victim, TO_VICT );
		    act( ZM_WZROK | ZM_WID_CHAR, "Anio`l os`lania $3 przed czarem.", victim, NULL, NULL, TO_ROOM );
		    act( ZM_PRZYT | ZM_W_WID_CHAR, "Co`s poch`lania moc zakl`ecia.", victim, NULL, ch, TO_VICT );
		}
		else
		{
		    send_to_char( "Anio`l przyjmuje wymierzone w ciebie ciosy.\n\r", victim );
		    act( ZM_WZROK | ZM_WID_CHAR, "Anio`l os`lania $3 przed atakiem.", victim, NULL, NULL, TO_ROOM );
		    act( ZM_PRZYT | ZM_W_WID_CHAR, "Co`s poch`lania moc ciosu.", victim, NULL, ch, TO_VICT );
		}

		victim->pcdata->hp_angel = UMAX( 0, victim->pcdata->hp_angel - dam );

		if ( victim->pcdata->hp_angel <= 0 )
		{
		    affect_strip( victim, gsn_aniol_stroz );
		    send_to_char( "Anio`l przestaje ci`e ochrania`c.\n\r", victim );
		    act( ZM_WZROK | ZM_WID_CHAR, "Anio`l chroni`acy $3 znika.", victim, NULL, NULL, TO_ROOM );
		}

		return;
	    }
	}


	/* Lam: jestem wiecej niz pewien, ze to powinno byc wlasnie tu */
	if ( dam > 1
	  && IS_AFFECTED( victim, AFF_BLACK_AURA ) && victim->mana > 0 )
	{
	    if ( dam / 2 < victim->mana )
	    {
		victim->mana -= dam % 2;
		dam /= 2;
		victim->mana -= dam;
	    }
	    else
	    {
		dam -= victim->mana;
		victim->mana = 0;
	    }
	}
    }
    else if ( !czy_bez_walki && victim == ch && dt != TYPE_UNDEFINED )
    {
	if ( is_safe( ch, victim, TRUE ) )
	    return;

	if ( ZDOLNY_DO_WALKI( ch ) /* raczej bedzie, skoro tu jestesmy */
	  && !is_fighting( ch, ch ) )
	    set_fighting( ch, ch );

	zdejmij_niewidki_przy_ataku( ch );
    }

    if ( !dam && !pudlo )
	dam = 1;

    /*
     * We moved dam_message out of the victim != ch if above
     * so self damage would show.  Other valid type_undefined
     * damage is ok to avoid like mortally wounded damage - Kahn
     */
    if ( dt != TYPE_UNDEFINED )
    {
	dam_message( ch, victim, dam, dt, wpn );
#if defined( MSDOS ) && !defined( NO_SOUND )
	if ( dam && descriptor_list && descriptor_list->character
	  && ( ch->in_room == descriptor_list->character->in_room ) )
	    damage_sound( );
#endif
    }

    /*
     * Qwert 01.04.2001: moja wizja, teraz atak magiczny z pierwszej linii jest
     * o 20% lepszy, a zza plecow o 20% gorszy - mag musi uwazac by nie poranic
     * kompana. 20 to niewielka zmiana, ale mozna dac wiecej.
     */
    if ( !IS_NPC( ch )
      && skill_table[ dt ].min_mana
      && dam > 1 )
    {
     /* ch walczy bezposrednio z victimem, nie jest zajety jeszcze inna walka */
	if ( FIGHTING( victim ) == ch && FIGHTING( ch ) == victim )
	    dam += (int) ( dam * 0.2 );
     /* ch chowa sie za plecami */
	else if ( FIGHTING( victim ) != ch && FIGHTING( ch ) == victim )
	    dam -= (int) ( dam * 0.2 );
     /* reszta przypadkow nie zmienia obrazen */
    }

    /*
     * MOMENT ZADANIA OBRAZEN
     *
     * Qwert 11.11.2000: mala zmiana obrazen fizycznych (ataki magiczne
     * zabieraja mnichowi do 10 pkt DODATKOWO - mnich moze sie przed tym bronic
     * wyciszeniem)
     */
    victim->hit -= dam;
    if ( !IS_NPC( victim )
      && victim->klasa == CLASS_MNICH
      && victim->level < LEVEL_IMMORTAL
      && dt < TYPE_HIT
      && dt > 0
      && dam > 3
      && skill_table[ dt ].min_mana
      && dt != gsn_lodowy_kolec )
    {
	if ( victim->mana > 10 )
	    victim->mana -= number_range( 1, 10 );
	else
	    victim->hit -= number_range( 1, 10 );
    }

    if ( !IS_NPC( victim )
      && victim->level >= LEVEL_IMMORTAL
      && victim->hit < 1 )
	victim->hit = 1;

    /* Lam: wczesniej tu byly tarcze, ale ich efekty powodowaly mnostwo
       problemow */

    if ( dam > 0 && dt > TYPE_HIT
      && is_wielding_specweapon( ch, wpn, ITEM_POISONED )
      && !saves_spell( ch->level, victim )
      && !CZY_WAMPIR( victim )
      && !IS_SET( race_table[ victim->race ].race_abilities, RACE_POISONIMM ) )
    {
	AFFECT_DATA af;

	af.type      = gsn_poison;
	af.duration  = 1; /* 2 godziny maks. */
	af.location  = APPLY_STR;
	/* z czaru jest / 40 to z broni co najmniej / 60 */
	af.modifier  = - ( get_eq_char( ch, wpn ) )->level / 60 - 1;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_POISON );
	af.level = ( get_eq_char( ch, wpn ) )->level;
	af.caster = ch;
	affect_join( victim, &af, ENH_AFFECT );
    }

    update_pos( victim );

    /* Lam 25.10.2000: to bylo przed update_pos (zle) */
    if ( is_affected( victim, gsn_berserk )
      && victim->position <= POS_STUNNED )
	affect_strip( victim, gsn_berserk );

    switch ( victim->position )
    {
	case POS_MORTAL:
	    act( ZM_ZAWSZE, "Jeste`s `smiertelnie rann$y i bez pomocy zaraz umrzesz.",
		victim, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n jest `smiertelnie rann$y i bez pomocy zaraz umrze.",
		victim, NULL, NULL, TO_ROOM );
	    break;

	case POS_INCAP:
	    act( ZM_ZAWSZE, "Jeste`s poharatan$y i umrzesz bez pomocy.",
		victim, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n jest poharatan$y i bez pomocy umrze.",
		victim, NULL, NULL, TO_ROOM );
	    break;

	case POS_STUNNED:
	    act( ZM_ZAWSZE, "Jeste`s og`luszon$y, ale jest jeszcze dla ciebie szansa.",
		victim, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n zosta`l$a og`luszon$y, ale jeszcze ma szans`e na prze`zycie.",
		victim, NULL, NULL, TO_ROOM );
	    break;

	case POS_DEAD:
	    break;

	default:
	    /* Lam: niesmiertelnych nie boli */
	    if ( victim != ch )
	    {
		if ( ( dam > victim->max_hit / 4 ) && !IS_IMMORTAL( victim ) )
		    send_to_char( "{RAu! To naprawd`e ZABOLA`LO!{x\n\r", victim );
		if ( ( victim->hit < victim->max_hit / 4 )
		  && ( !( /* CZY_OPCJA( victim, OPC_CHYBIANIE ) && */ pudlo ) ) )
		    send_to_char( "KRWAWISZ i to nie`xle!\n\r", victim );
	    }
	    break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE( victim ) && victim->fighting )
    {
	FIGHT_DATA *fight;
	CHAR_DATA *fch;

	/* powoduje, ze mob nie bedzie scigal tego gracza */
	for ( fight = victim->fighting; fight; fight = fight->next )
	{
	    if ( fight->vch->hunting == victim )
	    {
		wywal_zwod( &victim->zwod_hunting, fight->vch );
		fight->vch->hunting = NULL;
	    }
	    if ( fight->vch->hating == victim )
	    {
		wywal_zwod( &victim->zwod_hating, fight->vch );
		fight->vch->hating = NULL;
	    }
	}

	stop_fighting( victim, FALSE );
	/* Powoduje, ze nieagresywne moby nie dobijaja pokiereszowanych
	   graczy. Tu zmiana, zeby niezaleznie od tego, kto gracza atakowal,
	   przerwaly walke moby, ale nie walczacy z graczem gracze i ich slugi.
	   Tchorzliwe agresywne moby tez bija nieprzytomnych, wiec nie
	   sprawdzam dodatkowo tchorzliwosci. */
	for ( fch = victim->in_room->people; fch; fch = fch->next_in_room )
	    if ( fch != victim
	      && is_fighting( fch, victim )
	      && IS_NPC( fch )
	      && !IS_AFFECTED( fch, AFF_CHARM )
	      && !IS_SET( fch->act, ACT_AGGRESSIVE ) )
		stop_fighting_char( fch, victim );
    }

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	ROOM_INDEX_DATA *room = victim->in_room;
	int typ_zgonu = ZGON_ZWYKLY;
	bool czy_plakac = TRUE;

	/* tu bylo stop_fighting */

	if ( !ch->deleted && ch->in_room == room
	  && ch != victim && !czy_bez_walki )
	    if ( mprog_death_trigger( victim, ch ) )
		czy_plakac = FALSE;

	if ( victim->deleted
	  || victim->in_room != room )
	    return;

	/* teraz stop_fighting jest dopiero tu, #2502 */
	stop_fighting( victim, TRUE );

	if ( czy_plakac )
	    death_cry( victim, dt, pozycja );

	act( ZM_ZAWSZE, "{RJeste`s MARTW$)!{x", victim, NULL, NULL, TO_CHAR );
	if ( dt == TYPE_UNDEFINED ) /* wylacznie wykrwawianie */
	    act( ZM_WZROK | ZM_WID_CHAR, "{y$n UMAR`L$(!{x", victim, NULL, NULL, TO_ROOM );
	else
	    act( ZM_WZROK | ZM_WID_CHAR, "{y$n zosta`l$9 ZABIT$)!{x", victim, NULL, NULL, TO_ROOM );

	if ( find_timer( victim, finish_doom ) )
	    del_timer( find_timer( victim, finish_doom ) );

	if ( !czy_bez_walki && !ch->deleted )
	    czy_dodac_zgon = group_gain( ch, victim );

	if ( !IS_NPC( victim ) )
	{
	    int  gold = 0;
	    char buf[ MAX_STRING_LENGTH ];
	    char buf2[ MAX_STRING_LENGTH ];

	    if ( dt == gsn_uderzenie_w || dt == gsn_uderzenie_o )
	    {
		strcpy( buf2, "{M$N{x zmar`l$7 na skutek upadku z wysoko`sci." );
		sprintf( log_buf, "%s killed by %s%s at %d (uderzenie o podloze)",
			victim->short_descr,
			ch->deleted ? "(dead) " : "", ch->short_descr,
			victim->in_room->vnum );
	    }
	    else if ( dt == gsn_upadek )
	    {
		char *koncowki[ 3 ] = { "e", "ego", "`a" };
		sprintf( buf2, "{M$N{x zosta`l$7 zabit$8 przez spadaj`ac%s %s.",
		    koncowki[ ch->sex ], ch->biernik );
		sprintf( log_buf, "%s killed by %s%s at %d (upadek)",
			victim->short_descr,
			ch->deleted ? "(dead) " : "", ch->short_descr,
			victim->in_room->vnum );
	    }
	    else if ( victim == ch && pozycja != POS_FIGHTING && pozycja != POS_STANDING )
	    {
		strcpy( buf2, "{M$N{x zmar`l$7 na skutek odniesionych ran." );
		sprintf( log_buf, "%s killed by %s%s at %d (wykrw%s%s%s%s%s%s)",
			victim->short_descr,
			ch->deleted ? "(dead) " : "", ch->short_descr,
			victim->in_room->vnum,
			CZY_PALACE_SLONCE( victim ) ? " slon" : "",
			CZY_BRAK_TLENU( victim ) ? " tlen" : "",
			IS_AFFECTED( victim, AFF_POISON ) ? " truc" : "",
			IS_AFFECTED( victim, AFF_WRZACA_KREW ) ? " wrza" : "",
			( victim->pcdata->condition[ COND_FULL ] <= 0 ) ? " glod" : "",
			( victim->pcdata->condition[ COND_THIRST ] <= 0 ) ? " prag" : "" );
	    }
	    else
	    {
		sprintf( buf2, "{M$N{x zosta`l$7 zabit$8 przez %s.", ch->biernik );
		sprintf( log_buf, "%s killed by %s%s at %d (atak)",
			victim->short_descr,
			ch->deleted ? "(dead) " : "", ch->short_descr,
			victim->in_room->vnum );
	    }
	    wiznet( buf2, victim, NULL, NULL, WIZ_DEATH, 0, 0 );
	    log_string( log_buf );

	    /* Qwert: zabicie gracza przez gracza usuwa flage zabojcy */
	    if ( !ch->deleted
	      && !( IS_NPC( ch ) || IS_NPC( victim ) )
	      && IS_SET( victim->act, PLR_ZABOJCA )
	      && ch != victim
	      /* Lam 10.4.2005 */
	      && !IS_SET( victim->info, INF_LACSAKRA ) )
	    {
		/* to jest tutaj, zeby komunikat pokazal sie w odpowiednim miejscu */
		send_to_char( "{RTaka `lamaga jak ty nie mo`ze dalej nosi`c miana zab`ojcy!{x\n\r", victim );
		REMOVE_BIT( victim->act, PLR_ZABOJCA );
	    }

	    /*
	     * Dying penalty:
	     * 1/2 way back to previous 2 levels.
	     * Lam: to by bylo zbyt brutalne
	     */
	    if ( czy_bez_walki || ch->deleted || IS_NPC( ch ) || ch == victim )
		/* samobojstwo/smierc glodowa/poturbowany w walce z mobem */
	    {
		strata_dosw_za_smierc( victim );
	    }
	    else
	    {
		if ( ch != victim )
		{
/*
		    if ( IS_SET( victim->act, PLR_REGISTER ) )
*/
		    {
			int exp;
			int li;

			exp = number_range( 20, 40 ) * 10; /* Lam: bylo 250-750 */
			li = liczba( exp );
			gain_exp( victim, 0 - exp );
			sprintf( buf, "{RTracisz %d punkt%s do`swiadczenia.{x\n\r",
				exp, li == 1 ? "" : li == 2 ? "y" : "`ow" );
			send_to_char( buf, victim );
			if ( victim->exp < 1001 )
			{
			    send_to_char( "Nie masz ju`z punkt`ow do`swiadczenia do oddania.\n\r", victim );
			    /* Qwert: stc->act */
			    act( ZM_ZAWSZE, "Teraz jeste`s naprawd`e MARTW$).", victim, NULL, NULL, TO_CHAR );
			    SET_BIT( victim->act, PLR_DENY );
			    free_string( victim->pcdata->deny.od );
			    victim->pcdata->deny.od = str_dup( "automatyczna smierc: ponizej 1001 doswiadczenia" );
			}
		    }

		    gold = victim->gold * number_range( 10, 20 ) / 100;
		    ch->gold += gold;
		    victim->gold -= gold;
		    sprintf( buf, "Tracisz %s.\n\r", ZLOTO_B( gold ) );
		    send_to_char( buf, victim );
		    sprintf( buf, "Dostajesz %s.\n\r", ZLOTO_B( gold ) );
		    send_to_char( buf, ch );
		}
	    }

	    if ( ch == victim )
	    {
		if ( czy_zwykly_zgon )
		    typ_zgonu = ZGON_ZWYKLY;
		else if ( dt == TYPE_UNDEFINED && wpn == WEAR_NONE )
		    typ_zgonu = ZGON_WYKRWAWIENIE;
		else
		    typ_zgonu = ZGON_SAMOBOJSTWO;
	    }
	    else
	    {
		if ( !IS_NPC( ch ) )
		    typ_zgonu = ZGON_GRACZ;
		else
		    typ_zgonu = ZGON_ZWYKLY;
	    }
	}

	raw_kill( ch, victim, czy_bez_walki, czy_dodac_zgon, typ_zgonu );

	/* Ok, now we want to remove the deleted flag from the
	 * PC victim.
	 */
	if ( !IS_NPC( victim ) )
	    victim->deleted = FALSE;

	if ( !ch->deleted && ch->in_room == room
	  && !IS_NPC( ch )
	  && IS_NPC( victim )
	  && !IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_CORPSE ) )
	{
	    int zloto;
	    OBJ_DATA *obj = get_obj_list( ch, "cia`lo", ch->in_room->contents );

	    if ( obj )
	    {
		OBJ_DATA *zawart = get_obj_list( ch, "z`lotoxxx", obj->contains );
		/* Autogold by Morpheus */
		if ( zawart && CZY_OPCJA( ch, OPC_AUTOGOLD ) )
		{
		    zloto = ch->gold;
		    do_get( ch, "z`lotoxxx cia`lo" );
		    zloto = ch->gold - zloto;
		/* Lam: autopodzial
		  - split2 zamiast sprintf( buf, "%d", zloto );
				   do_split( ch, buf ); */
		    if ( CZY_OPCJA( ch, OPC_AUTOSPLIT ) )
			split2( ch, zloto );
		}

		/* Lam 15.5.2000: moze sie zdarzyc, ze w ciele cos sie znajdzie,
		   ale bedzie niewidzialne dla gracza, i jednak zobaczy
		   "Nie widze tu "cialo"" albo "nie widze tego w ciele czyimstam" */
		if ( obj->contains )
		{
		    if ( CZY_OPCJA( ch, OPC_AUTOLOOT ) )
			do_get( ch, "wsz cia`lo" );
		    else
			do_look( ch, "w cia`lo" );
		}

		if ( CZY_OPCJA( ch, OPC_AUTOSAC  ) && !obj->contains )
		    do_sacrifice( ch, "cia`lo" );
	    }
	}

	/*
	 * Remove victims of pk who no longer have exps left
	 */
	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_DENY ) )
	{
	    do_note( victim, "czy`s`c" );
	    do_quit( victim, "zupe`lnie" );
	}

	return;
    }

    if ( czy_bez_walki || victim == ch || ch->deleted )
	return;

    /* Lam: tarcze przeniesione tu. Dzieki temu mamy pewnosc, ze victim
       niezaleznie od wszystkiego nie zawisnie gdzies z POS_DEAD.
       Tarcze sprawdzaly !czy_bez_walki && victim != ch, stalo sie to zbedne
       po przenosinach. */
    if ( ( dam > 1 )
      && IS_AFFECTED( victim, AFF_FLAMING )
      && !IS_AFFECTED( ch, AFF_LODOWA_TARCZA ) )
    {
	/* powinno byc tak jak jest z ruchem - flaga, czy postac juz dostala od
	   jakiejs tarczy, czy nie, wtedy najwyzej raz kazda tarcza zadziala */
	if ( dt != gsn_flamestrike && dt != gsn_mroczny_plomien )
	    ( *skill_table[ gsn_flamestrike ].spell_fun )( gsn_flamestrike,
			( victim->level / 3 ), victim, ch, USAGE_SHIELD );
	/* Lam: ten czar moze spowodowac jego odbicie, albo wywolanie proga,
	   ktory zabije victim */
	if ( ch->deleted || victim->deleted
	  || ch->in_room != victim->in_room
	  || ch->in_room != in_room )
	    return;
    }

    if ( ( dam > 1 )
      && IS_AFFECTED( victim, AFF_LODOWA_TARCZA )
      && !IS_AFFECTED( ch, AFF_FLAMING ) )
    {
	if ( dt != gsn_lodowy_kolec && dt != gsn_mroczny_plomien )
	    ( *skill_table[ gsn_lodowy_kolec ].spell_fun )( gsn_lodowy_kolec,
			( victim->level / 3 ), victim, ch, USAGE_SHIELD );
	if ( ch->deleted || victim->deleted
	  || ch->in_room != victim->in_room
	  || ch->in_room != in_room )
	    return;
    }

    if ( ( dam > 1 )
      && IS_AFFECTED( victim, AFF_TARCZA_UMARLYCH ) )
    {
	if ( dt != gsn_mroczny_plomien
	  && dt != gsn_lodowy_kolec
	  && dt != gsn_flamestrike )
	    ( *skill_table[ gsn_mroczny_plomien ].spell_fun )( gsn_mroczny_plomien,
			( victim->level / 3 ), victim, ch, USAGE_SHIELD );
	if ( ch->deleted || victim->deleted
	  || ch->in_room != victim->in_room
	  || ch->in_room != in_room )
	    return;
    }

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC( victim ) && !victim->desc && IS_AWAKE( victim ) )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return;
	}
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC( victim ) && dam > 0 )
    {
	if ( ( IS_SET( victim->act, ACT_WIMPY ) && number_bits( 1 ) == 0
	    && victim->hit < victim->max_hit / 2 )
	  || ( IS_AFFECTED( victim, AFF_CHARM ) && victim->master
	    && victim->master->in_room != victim->in_room ) )
	{
	    if ( victim->hunting )
	    {
		wywal_zwod( &victim->hunting->zwod_hunting, victim );
		victim->hunting = NULL;
	    }
	    if ( victim->hating )
	    {
		wywal_zwod( &victim->hating->zwod_hating, victim );
		victim->hating = NULL;
	    }
	    if ( victim->fearing )
		wywal_zwod( &victim->fearing->zwod_fearing, victim );
	    victim->fearing = ch;
	    dod_zwod( &ch->zwod_fearing, victim );
	    do_flee( victim, "" );
	    if ( ch->deleted || victim->deleted
	      || ( !is_fighting( ch, victim ) ) )
		return;
	}

	if ( IS_AFFECTED( ch, AFF_GROZA )
	  && !IS_SET( victim->act, ACT_PEACEFUL )
	  && number_bits( 2 ) == 0
	  && victim->level - ch->level < 6 )
	{
	    do_flee( victim, "" );
	    if ( ch->deleted || victim->deleted
	      || ( !is_fighting( ch, victim ) ) )
		return;
	}
    }

    if ( !IS_NPC( victim )
      && victim->hit > 0
      && victim->hit <= victim->wimpy
	 /* Lam: istotna zmiana - z wycwiczona szybka ucieczka nie czeka na
	    wstanie z ziemi. Gracze czesto gina po podcieciu, bo nie moga wstac
	    i uciec, a tak robia jakis wymyk i uciekaja.
	    Mimo wszystko sprawiedliwiej byloby tak jak w Envy, ze jesli
	    ch->wait nie jest zerem, to nie powinno dac sie uciec. Z drugiej
	    strony wtedy moge uciec piszac "uciekaj", "tchorz" jest od tego zeby
	    uciec zawsze. */
      && ( victim->wait == 0
	|| number_percent( ) < victim->pcdata->learned[ gsn_fast_flee ] / 3 ) )
	do_flee( victim, "" );

	/* Lam: tu przenosze magiczne bronie Qwerta. Wczesniej byly w miejscu,
	   gdzie ofiara mogla byc juz zabita, nie czekajac na komunikat
	   o smierci, w wyniku czego dalo sie postac zabic dwa razy jednym
	   ciosem */

    if ( ch->deleted || victim->deleted
      || ( !is_fighting( ch, victim ) ) )
	return; /* na wypadek proga albo czegos innego */

    /* Qwert */
    /* Lam: dodalem !number_bits( 1 ) zeby wychodzilo rzadziej */
    /* Qwert: zdjalem number_bits - mag/mnich maja po 1 ataku. Jest save_spell
       i nikt tego nie uzywa! Moze dac ch->class == CLASS_MAG? */
    if ( dam > 1 && dt > TYPE_HIT
      && is_wielding_specweapon( ch, wpn, ITEM_FLAME )
      && !saves_spell( ch->level, victim )
	/* && !number_bits( 1 ) */ )
    {
	( *skill_table[ gsn_flamestrike ].spell_fun )( gsn_flamestrike,
			( ch->level / 5 ), ch, victim, USAGE_WEAPON );
	if ( victim->deleted || ch->deleted
	  || ( !is_fighting( ch, victim ) ) )
	    return;
    }

    if ( dam > 1 && dt > TYPE_HIT
      && is_wielding_specweapon( ch, wpn, ITEM_ICY )
      && !saves_spell( ch->level, victim )
	/* && !number_bits( 1 ) */ )
    {
	( *skill_table[ gsn_lodowy_kolec ].spell_fun )( gsn_lodowy_kolec,
			( ch->level / 5 ), ch, victim, USAGE_WEAPON );
	if ( victim->deleted || ch->deleted
	  || ( !is_fighting( ch, victim ) ) )
	    return;
    }

    /* Qwert: tu (chaos), number_bits musi zostac */
    if ( dam > 1 && dt > TYPE_HIT
      && is_wielding_specweapon( ch, wpn, ITEM_CHAOS )
      && !saves_spell( ch->level, victim )
      && !CZY_WAMPIR( victim )
      && !IS_SET( race_table[ victim->race ].race_abilities, RACE_POISONIMM )
      && !number_bits( 1 ) )
    {
	AFFECT_DATA af;

	af.type      = gsn_ostrze_chaosu;
	af.duration  = 1;
	af.location  = APPLY_AC;
	af.modifier  = -4; /* Qwert */
	CLEAR_BITS( af.bitvector );
	af.level = 0;
	af.caster = ch;
	affect_join( victim, &af, ENH_AFFECT );

	( *skill_table[ gsn_cause_light ].spell_fun )( gsn_cause_light,
			( ch->level / 3 ), ch, victim, USAGE_WEAPON );

	if ( victim->deleted || ch->deleted
	  || ( !is_fighting( ch, victim ) ) )
	    return;
    }

    if ( dam > 1 && dt > TYPE_HIT
      && is_wielding_specweapon( ch, wpn, ITEM_MANA_LEECH )
      && !IS_NPC( victim )
      && !IS_GOOD( victim )
      && ( victim->mana > 0 ) )
    {
	OBJ_DATA *wield;
	int mana_amount, modifier;

	wield = get_eq_char( ch, WEAR_WIELD );

	if ( IS_EVIL( victim )
	  || CZY_WAMPIR( victim )
	  || IS_SET( victim->act, PLR_KILLER )
	  || IS_SET( victim->act, PLR_THIEF )
	  || victim->race == zr_martwiak )
	    modifier = 2;
	else
	    modifier = 1;

	mana_amount = number_fuzzy( victim->max_mana / 200 ) * modifier;

	if ( mana_amount <= 0 )
	    mana_amount = 1;

	mana_amount += wield->level / 4;
	mana_amount = LICZ_WYPAROWANIE_OPC( victim, mana_amount );

	victim->mana = UMAX( 0, victim->mana - mana_amount );

	send_to_char( "Jaka`s dziwna si`la dr`a`zy tw`oj umys`l.\n\r", victim );

	if ( victim->deleted || ch->deleted
	  || !is_fighting( ch, victim ) )
	    return;
    }

    tail_chain( );

    return;
}


/* Lam 7.9.2005: funkcja poprawi czytelnosc is_safe/check_killer, a powstala
   przy okazji zabraniania ataku 101->100 (w praktyce roznica 100 poziomow) */
bool czy_morderstwo( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->level >= L_APP && victim->level < L_APP )
	return TRUE;

    if ( CZY_WAMPIR( victim ) )
	return FALSE;

    /* Lam 26.7.2006: swietna taktyka wampirow byl alias uciekajacy, polykajacy
       szybko miksture przemiany, po czym gracz udawal, ze slabo ucieka,
       podczas gdy goniacy go lowca sadzil, ze nadal ma do czynienia z wampirem
       i zostawal ukarany flaga */
    if ( IS_AFFECTED( victim, AFF_POLYMORPH ) )
    {
	AFFECT_DATA *paf;

	for ( paf = victim->affected; paf; paf = paf->next )
	    if ( !paf->deleted && IS_SET_V( paf->bitvector, AFF_POLYMORPH ) )
		break;

	if ( !paf )
	    bug( "brak przemiany!", 0 );
	else if ( paf->location != APPLY_RACE )
	    bug( "przemiana nie wplywa na rase!", 0 );
	else if ( victim->race - paf->modifier == zr_wampir )
	    return FALSE;
    }

    if ( ch->level - victim->level > 5 )
	return TRUE;

    return FALSE;
}


/*
 * Qwert 27.02.04: zabojca i dosc znaczne zmiany tutaj
 */
bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim, bool czy )
{
    bool check = TRUE;

    if ( !is_fighting( ch, victim ) && !is_fighting( victim, ch ) )
    {
	if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) && ch->master
	  && is_safe( ch->master, victim, FALSE ) )
	    return TRUE;

	if ( IS_NPC( victim ) && IS_AFFECTED( victim, AFF_CHARM )
	  && victim->master && is_safe( ch, victim->master, FALSE ) )
	{
	    if ( czy ) act( ZM_ZAWSZE, "Nie wolno ci zaatakowa`c $!.",
			   ch, NULL, victim, TO_CHAR );
	    return TRUE;
	}
    }

    /* Lam 13.9.2005: zamiast stosu kombinacji ponizej */
    if ( is_fighting( victim, ch )
      || is_fighting( ch, victim ) )
	return FALSE;

    /* Lam 25.4.2004: zaden mob nigdy nie zaatakuje niewidki na poziomie
       niesmiertelny */
    if ( IS_NPC( ch )
      && !IS_NPC( victim )
      && IS_SET( victim->act, PLR_WIZINVIS )
      && victim->pcdata->wizinvis >= LEVEL_IMMORTAL )
	return TRUE; /* komunikat w tej sytuacji wydaje sie zbedny */

    if ( ch->in_room->area == area_czysciec )
    {
	if ( czy ) STC( "No co`s ty, w niecielesnym `swiecie?\n\r", ch );
	return TRUE;
    }

   /*
    * Qwert: mozna atakowac wszedzie, jesli ma flage (ZABOJCA). Nie usuwam
    * AFF_SPOKOJ z victima, bo moby nie maja prawa bic nikogo z taka flaga
    */

    if ( !( IS_NPC( ch ) || IS_NPC( victim ) )
      && ch != victim
      /* jesli nie ma, tylko normalne bicie: */
      && IS_SET( victim->act, PLR_ZABOJCA ) )
	check = FALSE;

    if ( !IS_NPC( ch ) && IS_AFFECTED( ch, AFF_GHOUL ) )
    {
	if ( czy ) STC( "W tej formie nie potrafisz walczy`c.\n\r", ch );
	return TRUE;
    }

    /* Lam */
    if ( !IS_NPC( ch )
      && ch == victim
      && !IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA ) )
    {
	if ( czy ) STC( "Je`sli na pewno chcesz si`e zabi`c, u`zyj polecenia \"morduj\".\n\r", ch );
	return TRUE;
    }

    /* Qwert: gracz chce zabic kogos, kto wymaga do tego "morduj" */
    if ( !( IS_NPC( ch ) || IS_NPC( victim ) )
      && ch != victim
      && ( !is_fighting( ch, victim ) )
      && ( !is_fighting( victim, ch ) )
      && !IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA )
      && !( is_name2( ch->name, victim->pcdata->pojedynek )
	 || is_name2( victim->name, ch->pcdata->nienawisc )
	 || IS_SET( victim->act, PLR_KILLER )
	 || IS_SET( victim->act, PLR_THIEF )
	 || ( IS_SET( victim->act, PLR_ZABOJCA )
	   && !czy_morderstwo( ch, victim ) ) ) )
    {
	if ( czy ) act( ZM_ZAWSZE, "Aby zaatakowa`c $# u`zyj \"morduj\".", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

   /*
    * Qwert: gracz chce zamordowac kogos, kogo mozna wylacznie zabic. KONIECZNA
    * zmiana w przypadku uzycia atakow obszarowych. Potrzebny tez byl komunikat.
    */
    if ( !( IS_NPC( ch ) || IS_NPC( victim ) )
      /* Lam 17.4.2005: "m z sie" u mordercy/zabojcy ma dzialac: */
      && ch != victim
      && IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA )
      && ( !is_fighting( ch, victim ) )
      && ( !is_fighting( victim, ch ) )
      && ( IS_SET( victim->act, PLR_KILLER )
	|| IS_SET( victim->act, PLR_THIEF )
	|| is_name2( ch->name, victim->pcdata->pojedynek )
	/* atak nie tylko na zabojce: */
	|| is_name2( victim->name, ch->pcdata->nienawisc )
	/* lowca uzyl "morduj": */
	|| ( IS_SET( victim->act, PLR_ZABOJCA )
	  && !czy_morderstwo( ch, victim ) ) ) )
    {
	if ( czy ) act( ZM_ZAWSZE, "Do ataku na $# nie mo`zesz u`zy`c \"morduj\".", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

    if ( !IS_NPC( victim ) && IS_AFFECTED( victim, AFF_GHOUL ) )
    {
	if ( czy )
	{
	    act( ZM_ZAWSZE, "Tw`oj cios przechodzi przez $# na wylot.", ch, NULL, victim, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "Cios $1 przechodzi na wylot przez $#.", ch, NULL, victim, TO_NOTVICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT, "$n robi zamach i niezdarnie si`e potyka.", ch, NULL, victim, TO_NOTVICT );
	    act( ZM_WZROK | ZM_WID_CHAR, "Cios $1 przechodzi przez ciebie na wylot.", ch, NULL, victim, TO_VICT );
	}
	return TRUE;
    }

    /* Lam 5.6.98 na wniosek RenTora */
    if ( check /* && ( !is_fighting( victim, ch ) ) */
      && IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
	if ( czy )
	{
	    if ( ch == victim )
		act( ZM_ZAWSZE, "Jeste`s w bezpiecznym pomieszczeniu.", ch, NULL, NULL, TO_CHAR );
	    else
		act( ZM_ZAWSZE, "$N jest w bezpiecznym pomieszczeniu.", ch, NULL, victim, TO_CHAR );
	}
	return TRUE;
    }

    /* Lam 28.4.2000 na wniosek RenTora */
    if ( IS_NPC( victim )
      && ( !victim->fighting )
      && IS_SET( victim->act, ACT_PEACEFUL ) )
    {
	if ( czy ) act( ZM_ZAWSZE, "Nie wolno ci walczy`c z $$.", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

    /*
     * Qwert 11.11.2000: mala zmiana sprawdzania spokoju. Nie sprawdzam
     * mordercow/zlodziei, bo teraz tego nie wrzuca na siebie
     */
    if ( check && ch != victim && IS_AFFECTED( victim, AFF_SPOKOJ ) )
    {
	if ( czy ) act( ZM_ZAWSZE, "Z $! emanuje taki spok`oj, `ze ani w g`lowie ci walka.", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

    /* Qwert: sprawdzam czy nikt z grupy nie zmusza nas do walki */
    if ( check && IS_AFFECTED( ch, AFF_SPOKOJ ) )
    {
	if ( czy ) STC( "Czujesz taki spok`oj, `ze nie masz ochoty na walk`e.\n\r", ch );

	/* ch mogl byc dowodca grupy */
	if ( !ch->master )
	    die_follower( ch );

	/* ch chodzil za kims, mogl byc czlonkiem grupy */
	if ( ch->master )
	    stop_follower( ch );

	stop_fighting( ch, TRUE );

	return TRUE;
    }

    if ( !IS_NPC( ch ) && ( IS_NPC( victim )
	 && IS_AFFECTED( victim, AFF_SLUGA ) && victim->master == ch ) )
    {
	if ( czy ) STC( "Nie atakuj w`lasnych s`lug.\n\r", ch );
	return TRUE;
    }

    if ( IS_NPC( ch ) || IS_NPC( victim ) )
	return FALSE;

    if ( is_name2( ch->name, victim->pcdata->pojedynek ) )
	return FALSE;

    if ( ch == victim )
	return FALSE;

    /* Qwert 07.03.2001 */
    if ( IS_SET( ch->info, INF_LAMER ) )
    {
	STC( "Twoja niezdarno`s`c nie pozwala ci zaatakowac kogokolwiek.\n\r", ch );
	return TRUE;
    }

    if ( IS_SET( victim->info, INF_LAMER ) )
    {
	STC( "Szkoda czasu na walk`e z tak`a niezdar`a.\n\r", ch );
	return TRUE;
    }

    if ( IS_SET( victim->act, PLR_KILLER )
      || IS_SET( victim->act, PLR_THIEF ) )
	return FALSE;

    /* Lam 27.11.1999: LEVEL_HERO -> LEVEL_IMMORTAL */
    if ( victim->level >= LEVEL_IMMORTAL )
    {
	if ( czy ) act( ZM_ZAWSZE, "$N jest nie`smierteln$Y, wi`ec jaki sens ma walka?",
		ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

    /* Lam 27.11.1999: LEVEL_HERO -> LEVEL_IMMORTAL */
    /* Lam 17.2.2005: !is_fighting, niesmiertelny moze oddac w razie pojedynku */
    /* Lam 13.9.2005: is_fighting wyrzuca FALSE na samym poczatku funkcji */
    if ( ch->level >= LEVEL_IMMORTAL /* && !is_fighting( victim, ch ) */ )
    {
	if ( czy ) send_to_char( "Nie`smiertelni nie morduj`a ani nie okradaj`a graczy.\n\r", ch );
	return TRUE;
    }

    if ( victim->level < 10 /* && !is_fighting( ch, victim ) */
      && !IS_SET( victim->act, PLR_ZABOJCA ) ) /* wazne! */
    {
	if ( czy ) send_to_char( "Mordowanie graczy poni`zej poziomu 10 zosta`lo zakazane.\n\r", ch );
	return TRUE;
    }

    /* to tu wczesniej odbywalo sie sprawdzanie czy ROOM_SAFE */
    return FALSE;
}


/*
 * Ocenia legalnosc ataku i ustawia flagi ZABOJCY i MORDERCY za nieprzepisowe
 * walki oraz wywoluje dodaj_zabojce (daje prawo zemsty).
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char kto[ MIL ];

    /* legalne: ataki na moby, ataki na siebie oraz ataki w walkach, ktore juz
       trwaja */
    if ( IS_NPC( victim )
      || ch == victim
      || is_fighting( ch, victim )
      || is_fighting( victim, ch ) )
	return;

    /* jest to atak na gracza. Moby flag nie dostaja, ale jesli atak byl
       rozkazem gracza, ponosi za niego odpowiedzialnosc rozkazujacy */
    if ( IS_NPC( ch ) )
    {
	if ( IS_AFFECTED( ch, AFF_CHARM )
	  && ch->master
	  && ch->master->in_room == ch->in_room
	  && !IS_NPC( ch->master )
	  && IS_SET( ch->master->act, PLR_CHCE_BYC_MORDERCA ) )
	{
	    check_killer( ch->master, victim );
	    /* typowy rozkaz do ataku nie ustawia walki - mozna sie do niej
	       wlaczyc recznie lub automatycznie dzieki grupie, jednak atak
	       na gracza z uzyciem "morduj" moze wymagac straty kolejnego
	       poziomu, aby wlaczyc sie do walki, za ktora juz dostalo sie
	       morderce (pierwszego za m rozk, drugiego za m z) */
	    if ( ZDOLNY_DO_WALKI( victim )
	      && !is_fighting( victim, ch->master ) )
		set_fighting( victim, ch->master );
	    /* gracz nie moze uzyc "morduj" bedac ogluszonym, nie sprawdzam */
	    if ( !is_fighting( ch->master, victim ) )
		set_fighting( ch->master, victim );
	}
	return;
    }

    /* morderce i zlodzieja wolno zabic absolutnie kazdemu */
    if ( IS_SET( victim->act, PLR_KILLER )
      || IS_SET( victim->act, PLR_THIEF ) )
	return;

    /* Lam: pojedynek oczywiscie nie da mordercy */
    if ( is_name2( ch->name, victim->pcdata->pojedynek ) )
	return;

    /* Qwert: odwet, legalny atak */
    if ( is_name2( victim->name, ch->pcdata->nienawisc ) )
	return;

    /* Qwert: lowcy zabojcow */
    if ( IS_SET( victim->act, PLR_ZABOJCA )
      && !czy_morderstwo( ch, victim ) )
	return;

    /* Qwert: mozna miec dwie flagi naraz. Jak widac ponizej, kto spelnil
       wszystkie warunki, ten dostaje prawo odwetu. Wczesniej bylo to
       w odwrotnej kolejnosci i po mordercy moglo wyjsc z funkcji. Wampiry
       przenioslem tylko do mordercy - zabojca wampirow to tez zabojca. */

    act( ZM_ZAWSZE, "$n ci`e zaatakowa`l$a! Masz teraz prawo zabi`c $3!", ch, NULL, victim, TO_VICT );

    /* Lam 7.9.2005: za atak na bohatera od 101 nie liczy sie zabojca, ale daje
       prawo zemsty */
    if ( victim->level >= L_APP )
    {
	if ( ch->level < L_APP )
	    dodaj_zabojce( victim, ch );
	return;
    }

    if ( !IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA ) )
	bug( "Gracz bez PLR_CHCE_BYC_MORDERCA zostal zab`ojc`a", 0 );

    if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_ZABOJCA )
      && !IS_SET( ch->act, PLR_ZABOJCA ) ) /* tylko jesli nie ma flagi */
	send_to_char( "Poprzez tw`oj zbrodniczy akt uznano ci`e za ZAB`OJC`E!\n\r", ch );

    /* czy dostal juz flage ZABOJCA w tym tiku? */
    if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_ZABOJCA ) )
	sprintf( kto, "%s %s", victim->name, ch->pcdata->zabojca_from );
    else
    {
	sprintf( kto, "%s %s", victim->name, ascdata( localtime( &current_time ) ) );
	SET_BIT( ch->act, PLR_ZABOJCA );
	SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_ZABOJCA );
	murder_char = TRUE;
    }
    free_string( ch->pcdata->zabojca_from );
    ch->pcdata->zabojca_from = str_dup( kto );
    dodaj_zabojce( victim, ch );

    if ( czy_morderstwo( ch, victim ) )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	char buf[ MAX_STRING_LENGTH ];
	char killer_kto[ MIL ];

	if ( !IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA ) )
	    bug( "Gracz bez PLR_CHCE_BYC_MORDERCA zostal morderca", 0 );

	if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_MORDERCA ) )
	{
	    if ( !IS_SET( ch->act, PLR_KILLER )
	      && ch->level >= L_APP )
		send_to_char( "{RJeste`s MORDERC`A! Powt`orz taki atak, a stracisz poziom!{x\n\r", ch );
	    else
		send_to_char( "{RJeste`s MORDERC`A! Tracisz 1000 punkt`ow do`swiadczenia.{x\n\r", ch );
	}
	sprintf( buf, "Ratunku! %s mnie atakuje!", ch->name );
	do_yell( victim, buf );

	/* czy dostal juz flage MORDERCA w tym tiku? */
	if ( IS_SET( ch->pcdata->temp_flags, TEMPFLAG_MORDERCA ) )
	{
	    sprintf( killer_kto, "%s %s", victim->name, ch->pcdata->killer_from );
	    free_string( ch->pcdata->killer_from );
	    ch->pcdata->killer_from = str_dup( killer_kto );
	    return;
	}
	sprintf( killer_kto, "%s %s", victim->name, ascdata( localtime( &current_time ) ) );
	murder_char = TRUE;
	free_string( ch->pcdata->killer_from );
	ch->pcdata->killer_from = str_dup( killer_kto );
	/* Lam 7.9.2005 - na 101 pierwszy morderca jest bez straty poziomu */
	if ( IS_SET( ch->act, PLR_KILLER )
	  || ch->level < L_APP )
	{
	    gain_exp( ch, -1000 );
	    demote_level( ch );
	}
	SET_BIT( ch->act, PLR_KILLER );
	SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_MORDERCA );

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->deleted )
		continue;

	    /*
	     * Remove item inventories
	     * Includes licenses to kill
	     */
	    if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
		extract_obj( obj );
	    else if ( obj->wear_loc != WEAR_NONE )
		unequip_char_save( ch, obj );
	}

	save_char_obj( ch );
    }

    return;
}


/*
 * Qwert: zmienilem na sprawdzanie flagi w ogole
 */
bool is_wielding_specweapon( CHAR_DATA *ch, int wpn, int flag )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, wpn ) )
      && IS_OBJ_STAT( obj, flag ) )
	return TRUE;

    return FALSE;
}


/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance = 0;

    if ( !moze_uzyc( victim, gsn_parry ) )
	return FALSE;

    if ( !IS_AWAKE( victim ) )
	return FALSE;

    if ( is_affected( victim, gsn_zaglada ) )
	return FALSE;

    if ( IS_NPC( victim ) )
    {
	if ( get_eq_char( victim, WEAR_WIELD ) )
	{
	    if ( ch->level < 10 )
		chance = ch->level * 2;
	    else
		chance = UMIN( 50, ch->level + 10 );
	}

	if ( get_eq_char( victim, WEAR_WIELD_2 ) )
	    chance += UMIN( 20, victim->level / 4 );
    }
    else
    {
	if ( get_eq_char( victim, WEAR_WIELD ) )
	    chance = victim->pcdata->learned[ gsn_parry ] / 2;

	if ( get_eq_char( victim, WEAR_WIELD_2 ) )
	    chance += victim->pcdata->learned[ gsn_parry ] / 5;
    }

    /* zeby (dzieki roznicy poziomow) nie uzywali wojownicy, ktorzy nie
       wycwiczyli */
    if ( !chance )
	return FALSE;

    if ( number_percent( )
	 >= URANGE( 5, chance + 2 * ( victim->level - ch->level ), 95 ) )
	return FALSE;

    super_act( OPC_UNIKI, ZM_ZAWSZE, "{O$n sparowa`l$a tw`oj atak.{x",
	victim, NULL, ch, TO_VICT, FALSE );
    super_act( OPC_UNIKI, ZM_ZAWSZE, "{OParujesz atak $1.{x",
	ch, NULL, victim, TO_VICT, FALSE );
    super_act( OPC_UNIKI, ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT,
	"{O$N sparowa`l$A atak $1.{x", ch, NULL, victim, TO_NOTVICT, FALSE );
    /* ZM_DYLEMAT - wiecej odglosow, a na pewno uzaleznienie odglosow od
       typu broni - miecze moga brzeczec, ale jakies kije stukac
    super_act( OPC_UNIKI, ZM_SLUCH | ZM_W_WID_CHAR | ZM_W_WID_VICT,
	"{OS`lyszysz g`lo`sny brz`ek.{x", ch, NULL, victim, TO_NOTVICT, FALSE ); */
    uzyj( victim, gsn_parry, "parowaniu atak`ow" );

    return TRUE;
}


/*
 * Lam 6.5.2007
 */
bool sprawdz_blok_tarcza( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance = 0;

    if ( !moze_uzyc( victim, gsn_blokowanie_tarcza ) )
	return FALSE;

    if ( !IS_AWAKE( victim ) )
	return FALSE;

    if ( is_affected( victim, gsn_zaglada ) )
	return FALSE;

    if ( !get_eq_char( victim, WEAR_SHIELD ) )
	return FALSE;

    if ( IS_NPC( victim ) )
	chance = UMIN( 30, ch->level / 2 );
    else
	chance = victim->pcdata->learned[ gsn_blokowanie_tarcza ] / 4;

    if ( !chance )
	return FALSE;

    if ( number_percent( )
	 >= URANGE( 5, chance + 2 * ( victim->level - ch->level ), 95 ) )
	return FALSE;

    super_act( OPC_UNIKI, ZM_ZAWSZE, "{O$n zablokowa`l$a tw`oj atak tarcz`a.{x",
	victim, NULL, ch, TO_VICT, FALSE );
    super_act( OPC_UNIKI, ZM_ZAWSZE, "{OBlokujesz atak $1 tarcz`a.{x",
	ch, NULL, victim, TO_VICT, FALSE );
    super_act( OPC_UNIKI, ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT,
	"{O$N zablokowa`l$A atak $1 tarcz`a.{x", ch, NULL, victim, TO_NOTVICT, FALSE );
    uzyj( victim, gsn_blokowanie_tarcza, "blokowaniu atak`ow tarcz`a" );

    return TRUE;
}


/*
 * Check for dodge.
 * Wysoka efektywnosc do 20% zmniejsza/zwieksza szanse na unikniecie ciosu
 * (wymagane wycwiczenie unikow)
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;
    int effective = 0;

    if ( !moze_uzyc( victim, gsn_dodge ) )
	return FALSE;

    if ( !IS_AWAKE( victim ) )
	return FALSE;

    if ( is_affected( victim, gsn_zaglada ) )
	return FALSE;

    effective -= IS_NPC( ch )     ? 0 :
	number_percent( ) <= ch->pcdata->learned[ gsn_high_efficiency ] / 2 ?
		ch->pcdata->learned[ gsn_high_efficiency ] / 5 : 0;
    effective += IS_NPC( victim ) ? 0 :
	number_percent( ) <= victim->pcdata->learned[ gsn_high_efficiency ] / 2 ?
		victim->pcdata->learned[ gsn_high_efficiency ] / 5 : 0;

    if ( IS_NPC( victim ) )
    {
	if ( ch->level < 10 )
	    chance = victim->level * 2;
	else
	    chance = UMIN( 50, victim->level + 10 );
    }
    else
	chance = victim->pcdata->learned[ gsn_dodge ] / 2;

    if ( !chance )
	return FALSE;

    if ( number_percent( )
	 >= URANGE( 5, chance + effective + 2 * ( victim->level - ch->level ), 95 ) )
    {
	if ( effective < 0 )
	    uzyj( ch, gsn_high_efficiency, "wysokiej efektywno`sci" );
	return FALSE;
    }

    super_act( OPC_UNIKI, ZM_ZAWSZE, "{O$N uchyla si`e przed twoim atakiem.{x",
	ch, NULL, victim, TO_CHAR, FALSE );
    super_act( OPC_UNIKI, ZM_ZAWSZE, "{OUchylasz si`e przed atakiem $1.{x",
	ch, NULL, victim, TO_VICT, FALSE );
    super_act( OPC_UNIKI, ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT,
	"{O$N uchyla si`e przed atakiem $1.{x",
	ch, NULL, victim, TO_NOTVICT, FALSE );
    super_act( OPC_UNIKI, ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT,
	"{O$N si`e gimnastykuje.{x",
	ch, NULL, victim, TO_NOTVICT, FALSE );

    uzyj( victim, gsn_dodge, "unikach" );

    if ( effective > 0 )
	uzyj( victim, gsn_high_efficiency, "wysokiej efektywno`sci" );

    return TRUE;
}


/*
 * Gimza: sprawdzanie czy przeciwnik wpada w chwilowe odretwienie
 */
bool check_doom( CHAR_DATA *ch, CHAR_DATA *victim )
{
    TIMER_DATA *timer;
    AFFECT_DATA af;
    int chance;
    double modifier;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( ch == victim )
	return FALSE;

    if ( IS_GOOD( victim ) || IS_EVIL( ch ) )
	return FALSE;

    if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_DOOM_PROOF ) )
	return FALSE;

    if ( !IS_NPC( victim ) && victim->klasa == CLASS_PSIONIK )
	return FALSE;

    if ( is_fighting( victim, ch ) )
	return FALSE;

    /* finish_doom odpala sie teraz po zagladzie zabezpieczajac przed nia
       jakis czas po jej przeminieciu --Lam */
    if ( is_affected( victim, gsn_zaglada )
      || find_timer( victim, finish_doom ) )
	return FALSE;

    if ( !get_eq_char( ch, WEAR_WIELD ) )
	return FALSE;

    /* Lam 4.11.2004: zaglada wychodzila nawet bez cwiczenia */
    if ( !ch->pcdata->learned[ gsn_zaglada ] )
	return FALSE;

    chance = ch->level - victim->level;
    if ( IS_GOOD( ch ) )
	chance += ch->pcdata->learned[ gsn_zaglada ] / 3;
    modifier = 1;

    if ( ( victim->race == zr_martwiak
	|| CZY_WAMPIR( victim )
	|| IS_EVIL( victim ) )
	  || ( !IS_NPC( victim )
	     && ( IS_SET( victim->act, PLR_KILLER )
	       || IS_SET( victim->act, PLR_THIEF ) ) ) )
    {
	chance += ch->pcdata->learned[ gsn_zaglada ] / 2;
	/* Gimza 29.01.05: martwiaki, wampiry, zle postacie, zabojcy i zlodzieje
	   boja sie tego dluzej niz inni */
	modifier = 1.75;
	/* Lam 5.1.2009: modifier byl typu int i zawsze wynosil 1 mimo
	   powyzszego przypisania (nastepowala konwersja do 1 w locie) */
    }

    if ( chance < 0 )
	chance = 1;

    if ( !( number_percent( ) < chance ) )
    {
	/* nieudana zaglada zabezpiecza na chwile przed kolejna proba */
	timer = new_timer( );

	timer->ch = victim;
	timer->time = 120; /* 30 sekund odpornosci, jak w finish_doom */
	timer->fun = finish_doom;
	run_timer( timer );

	return FALSE;
    }

    send_to_char( "Tw`oj atak `smiertelnie przerazi`l przeciwnika.\n\r", ch );
    /* po czym to poznaje? */
    act( ZM_WZROK | ZM_WID_CHAR | ZM_DYLEMAT, "$n jest `smiertelnie przera`zon$y.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "Paniczny strach skr`eca w tobie trzewia.\n\r", victim );

    uzyj( ch, gsn_zaglada, "wzbudzaniu strachu" );

    af.type      = gsn_zaglada;
    af.duration  = 1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    CLEAR_BITS( af.bitvector );
    af.level = ch->level;
    af.caster = ch;

    affect_to_char( victim, &af );

    timer = new_timer( );

    timer->ch = victim;
    /* Gimza: w najlepszym wypadku 9 * PULSE_VIOLENCE */
    timer->time = (int) ( ( ch->pcdata->learned[ gsn_zaglada ] / 2 ) * modifier );
    if ( !IS_GOOD( ch ) )
	timer->time = timer->time / 2 + 1;
    timer->fun = finish_doom;
    run_timer( timer );

/*  WAIT_STATE( victim, timer->time ); ale zeby przyspieszenie nie cielo: */
    victim->wait = UMAX( victim->wait, timer->time );
    /* Lam: mob po pierwszej ucieczce paladyna po udanej zagladzie nie bedzie
       sie juz bal paladynow */
    if ( IS_NPC( victim ) )
	SET_BIT( victim->act, ACT_DOOM_PROOF );

    return TRUE;
}


FUNKCJA_CZASOWA( finish_doom )
{
    CHAR_DATA *ch = timer->ch;

    if ( is_affected( ch, gsn_zaglada ) )
    {
	TIMER_DATA *ntim;

	affect_strip( ch, gsn_zaglada );
	send_to_char( "Przera`zenie mija.\n\r", ch );
	send_to_char( "Ju`z mo`zesz normalnie walczy`c.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n mo`ze ju`z normalnie walczy`c.", ch, NULL, NULL, TO_ROOM );

	ntim = new_timer( );
	/* w tym miejscu prawdopodobnie ntim == timer, nie uzywaj timera */
	ntim->ch = ch;
	ntim->time = 120; /* 30 sekund */
	ntim->fun = finish_doom;
	run_timer( ntim );
    }

    return;
}


#if defined( BACKTRACE )
# include <execinfo.h>
#endif

/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    CHAR_DATA *foe;

    if ( victim->hit > 0 )
    {
	if ( victim->position <= POS_STUNNED )
	{
	    victim->position = POS_STANDING;
	    /* Lam 13.9.2005: przeciwnicy mogli walczyc z victim, kiedy victim
			byl ogluszony */
	    for ( foe = victim->in_room->people; foe; foe = foe->next_in_room )
		if ( is_fighting( foe, victim )
		  && !is_fighting( victim, foe ) )
		    set_fighting( victim, foe );
	}
	if ( victim->fighting )
	    victim->position = POS_FIGHTING;
	return;
    }

    if ( IS_NPC( victim ) || victim->hit <= -11 )
    {
#if defined( BACKTRACE )
	/* skopiowane z real_bug */
	void *slady[ 20 ]; /* wskazniki na funkcje */
	size_t ilslad; /* ilosc sladow */
	char **symslad; /* symbole sladow, nazwy funkcji */
	unsigned int i;

	/* zdarza sie glownie graczom, wiec upewnie sie, ze moby nie beda
	   lecialy do posdead_buf */
	if ( IS_NPC( victim ) )
	{
	    victim->position = POS_DEAD;
	    return;
	}

	ilslad = backtrace( slady, 20 );
	symslad = backtrace_symbols( slady, ilslad );

	sprintf( posdead_buf, "[-----] Stos wywolan update_pos ustawiajac POS_DEAD, postac %s.\n",
		victim->short_descr );

	for ( i = 0; i < ilslad; i++ )
	    sprintf( posdead_buf + strlen( posdead_buf ),
		"[-----] [#%u] %s\n", i, symslad[ i ] );

	free( symslad );
#endif
	victim->position = POS_DEAD;
	return;
    }

	 if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    check_airchar( victim );

    return;
}


/*
 * Rozpoczynanie walk. Wymagane uzycie set_fighting dla obu walczacych stron
 * (ch->victim i victim->ch)! Uwaga - kolejnosc nie jest dowolna, najpierw
 * walke musi ustawic agresor (z powodu komunikatow wymuszenia zmiany pozycji
 * oraz z powodu ustawiania odwleczenia wyjscia tylko dla atakujacego).
 *
 * Poza sprawdzaniem kolejnosci, sprawdzaj tez ZDOLNY_DO_WALKI( ).
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[ MAX_STRING_LENGTH ];
    FIGHT_DATA *fight;
    FIGHT_DATA *last_fight = NULL;

    if ( !ch || !victim || !ch->in_room || !victim->in_room )
    {
	bug( "set_fighting: postacie niezdatne do walki", 0 );
	return;
    }

    if ( !ZDOLNY_DO_WALKI( ch ) )
    {
	sprintf( buf, "set_fighting: niezdolny do walki ch (%s) chce walczyc z victim (%s) w %d",
		ch->short_descr, victim->short_descr,
		victim->in_room->vnum );
	bug( buf, 0 );
	return;
    }

    for ( fight = ch->fighting; fight; fight = fight->next )
    {
	if ( fight && fight->vch == victim )
	{
	    sprintf( buf, "set_fighting: ch (%s) juz walczy z victim (%s) w %d",
			ch->short_descr, victim->short_descr,
			victim->in_room->vnum );
	    bug( buf, 0 );
	    return;
	}

	if ( fight && fight->next == NULL )
	    last_fight = fight;
    }

    if ( !fight_free )
    {
	fight = alloc_perm( sizeof( *fight ) );
	top_fight++;
    }
    else
    {
	fight = fight_free;
	fight_free = fight_free->next;
    }

    if ( victim->position == POS_SLEEPING )
    {
	act( ZM_ZAWSZE, "Budzisz $# swoim atakiem.", ch, NULL, victim, TO_CHAR );
	act( ZM_ZAWSZE, "Budzi ci`e atak $1.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "Atak $1 budzi $#.", ch, NULL, victim, TO_NOTVICT );
	/* na chwile zmieniam pozycje, zeby pokazac komunikaty, potem wracam do
	   snu, zeby zdejmowanie uspienia mialo zawsze jasna sytuacje (w tej
	   chwili niepotrzebne, ale nie zaszkodzi) */
	victim->position = POS_STANDING;
	komunikaty_ustroju( victim );
	victim->position = POS_SLEEPING;
    }

    if ( victim->position == POS_RESTING )
    {
	act( ZM_ZAWSZE, "Przerywasz wypoczynek $! swoim atakiem.", ch, NULL, victim, TO_CHAR );
	act( ZM_ZAWSZE, "Atak $1 przerywa tw`oj wypoczynek.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "Atak $1 przerywa wypoczynek $!.", ch, NULL, victim, TO_NOTVICT );
    }

    if ( IS_AFFECTED( victim, AFF_SLEEP ) )
	affect_strip( victim, gsn_sleep );

    /* Dodanie tego tutaj ma dwa podstawowe efekty:
	- dobry dla nas, bo nie trzeba sie przebijac przez zrodla
	  w poszukiwaniu rozpoczynania walk,
	- zly dla graczy, bo automatyczne ataki niektorych ras w czasie walki
	  beda dla nich czesciej zabojcze */
    if ( !is_fighting( victim, ch ) && ch != victim )
    {
	if ( !IS_NPC( ch ) )
	    ch->pcdata->odwleczenie = ODWLECZENIE;

	/* zabezpieczenie przed "rozk wsz z ofiara|wyjdz|wciel" */
	if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM )
	  && ch->master && !IS_NPC( ch->master ) )
	    ch->master->pcdata->odwleczenie = ODWLECZENIE;

	/* #2268 - Lam, 29.12.2007 */
	if ( victim->fighting )
	    act( ZM_WZROK | ZM_WID_CHAR, "{R$n w`l`acza si`e do walki z tob`a.{x", ch, NULL, victim, TO_VICT );
	else /* bez ZM_WID_CHAR, a wiec zauwazy pierwszy atak */
	    act( ZM_WZROK, "{R$n rozpoczyna z tob`a walk`e.{x", ch, NULL, victim, TO_VICT );
    }

    fight->next = NULL;
    fight->vch = victim;
    if ( !last_fight )
	ch->fighting = fight;
    else
	last_fight->next = fight;

    ch->position = POS_FIGHTING;

    return;
}


void set_fighting_main( CHAR_DATA *ch, CHAR_DATA *victim )
{
    FIGHT_DATA *fight;

    if ( is_fighting( ch, victim ) )
	stop_fighting_char( ch, victim );

    fight = ch->fighting;
    ch->fighting = NULL;
    set_fighting( ch, victim );
    ch->fighting->next = fight;

    return;
}


bool is_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    FIGHT_DATA *fight;

    for ( fight = ch->fighting; fight; fight = fight->next )
	if ( fight->vch == victim )
	    return TRUE;

    return FALSE;
}


/*
 * Stop fights.
 */
void stop_fighting_char( CHAR_DATA *ch, CHAR_DATA *victim )
{
    FIGHT_DATA *fight;
    FIGHT_DATA *rem_fight = NULL;

    if ( !ch->fighting )
	return;

    if ( ch->fighting->vch == victim )
    {
	rem_fight = ch->fighting;
	ch->fighting = rem_fight->next;
    }

    if ( !rem_fight )
	for ( fight = ch->fighting; fight; fight = fight->next )
	     if ( fight->next && fight->next->vch == victim )
	     {
		 rem_fight = fight->next;
		 fight->next = rem_fight->next;
		 break;
	     }

    if ( rem_fight )
    {
	rem_fight->vch = NULL;
	rem_fight->next = fight_free;
	fight_free = rem_fight;
    }

    if ( !ch->fighting )
    {
	TIMER_DATA *timer;

	ch->position = POS_STANDING;
	if ( is_affected( ch, gsn_berserk ) )
	    affect_strip( ch, gsn_berserk );

	/* to symuluje przeminiecie zaglady, zabezpieczajac przed kolejna */
	if ( is_affected( ch, gsn_zaglada )
	  && ( timer = find_timer( victim, finish_doom ) ) )
	{
	    del_timer( timer );
	    timer->fun( timer );
	    /* w trakcie zaglady czas oczekiwania postaci jest wysoki,
	       zmniejszam na sekunde, poniewaz postac mogla byc np. podcieta
	       i powinna jeszcze chwile poczekac */
	    ch->wait = UMIN( ch->wait, 4 );
	}
	update_pos( ch );
    }

    return;
}


void stop_fighting_all( CHAR_DATA *ch )
{
    FIGHT_DATA *fight;
    FIGHT_DATA *fight_next;

    for ( fight = ch->fighting; fight; fight = fight_next )
    {
	fight_next = fight->next;
	stop_fighting_char( ch, fight->vch );
    }

    return;
}


void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    stop_fighting_all( ch );

    if ( !fBoth )
	return;

/*    for ( fch = char_list; fch; fch = fch->next ) */
    for ( fch = ch->in_room->people; fch; fch = fch->next_in_room )
	if ( fch != ch && is_fighting( fch, ch ) )
	    stop_fighting_char( fch, ch );

    return;
}


void strata_dosw_za_smierc( CHAR_DATA *victim )
{
    if ( victim->exp > EXP_PER_LEVEL * victim->level + 200 )
	gain_exp( victim, ( EXP_PER_LEVEL * victim->level - victim->exp ) / 2 );
    else
	gain_exp( victim, -100 );

    return;
}


/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    OBJ_DATA        *corpse;
    OBJ_DATA        *obj;
    OBJ_DATA        *obj_next;
    ROOM_INDEX_DATA *location;
    char            *name;
    char             buf [ MAX_STRING_LENGTH ];
    int              waga;

    name = ch->dopelniacz;

    if ( IS_NPC( ch ) )
    {
	/*
	 * This longwinded corpse creation routine comes about because
	 * we dont want anything created AFTER a corpse to be placed
	 * INSIDE a corpse.  This had caused crashes from obj_update()
	 * in extract_obj() when the updating list got shifted from
	 * object_list to obj_free.          --- Thelonius (Monk)
	 */

	if ( ch->gold > 0 )
	{
	    OBJ_DATA * coins;

	    coins = create_money( ch->gold );
	    corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_NPC ), 0 );
	    obj_to_obj( coins, corpse );
	    ch->gold = 0;
	}
	else
	{
	    corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_NPC ), 0 );
	}

	corpse->timer = number_range( 2, 4 );
	corpse->value[ 0 ].v = corpse->value[ 1 ].v = corpse->value[ 2 ].v = 0;
	if ( ch->pIndexData )
	    corpse->value[ 2 ].v = ch->pIndexData->vnum;
    }
    else
    {
	corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ), 0 );
	corpse->timer = number_range( 25, 40 );
    }

    waga = race_table[ ch->race ].weight;
    corpse->weight = number_range( waga - waga / 20, waga + waga / 20 );

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->dopelniacz, name );
    free_string( corpse->dopelniacz );
    corpse->dopelniacz = str_dup( buf );

    sprintf( buf, corpse->celownik, name );
    free_string( corpse->celownik );
    corpse->celownik = str_dup( buf );

    sprintf( buf, corpse->biernik, name );
    free_string( corpse->biernik );
    corpse->biernik = str_dup( buf );

    sprintf( buf, corpse->narzednik, name );
    free_string( corpse->narzednik );
    corpse->narzednik = str_dup( buf );

    sprintf( buf, corpse->miejscownik, name );
    free_string( corpse->miejscownik );
    corpse->miejscownik = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted )
	    continue;

	/* Lam 20.2.2005: zabezpieczenie przed zapetleniem przy zrzucaniu
	   przedmiotow dajacych sile do noszenia broni */
	if ( obj->wear_loc != WEAR_NONE )
	    unequip_char_save( ch, obj );

	/*
	 * Remove item inventories from all corpses.
	 * Includes licenses to kill
	 */
	if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	{
	    extract_obj( obj );
	}
	else
	{
	    obj_from_char( obj );

	/* remove this line and else{} below if you want player to not have
	   their eq upon death - Kahn */
/*          if ( IS_NPC( ch ) ) */
	    {
		obj_to_obj( obj, corpse );
	    }
/*          else
	    {
		obj_to_char( obj, ch );
	    }
*/
	}
    }

    if ( IS_NPC( ch ) )
    {
	obj_to_room( corpse, ch->in_room );
    }
    else
    {
	if ( !( location = get_room_index( ROOM_VNUM_GRAVEYARD_A ) ) )
	{
	    bug( "GRAVEYARD A does not exist!", 0 );
	    obj_to_room( corpse, ch->in_room );
	}
	else
	  obj_to_room( corpse, location );
    }

    return;
}


/*
 * Funkcja podobna do poprzedniej, ale nie robi ciala. Dziala dla duchow i
 * innych eterycznych widziadel. --Ulryk
 */
void make_no_corpse( CHAR_DATA *ch )
{
    OBJ_DATA        *obj;
    OBJ_DATA        *obj_next;
    bool	     found = FALSE;

    /* dla gracza zawsze robi cialo, nawet jesli byl duchem */
    if ( !IS_NPC( ch ) )
    {
	make_corpse( ch );
	return;
    }

    if ( ch->gold > 0 )
    {
	OBJ_DATA * coins;
	coins = create_money( ch->gold );
	obj_to_room( coins, ch->in_room );
	found = TRUE;
	ch->gold = 0;
    }

    /* skopiowane praktycznie w calosci z make_corpse( ) */
    for ( obj = ch->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted )
	    continue;

	if ( obj->wear_loc != WEAR_NONE )
	     unequip_char_save( ch, obj );

	if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	    extract_obj( obj );
	else
	{
	    obj_from_char( obj );
	    obj_to_room( obj, ch->in_room );
	    found = TRUE;
	}
    }

    /* dylemat, bo nie wiadomo, czy cokolwiek z tego ekwipunku bedzie widoczne
       dla dowolnego obserwatora --Lam */
    if ( found )
	act( ZM_WZROK | ZM_DYLEMAT, "Ekwipunek $1 rozsypuje si`e woko`lo.", ch, NULL, NULL, TO_ROOM );

    return;
}


/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch, int dt, int pozycja )
{
    ROOM_INDEX_DATA *was_in_room;
    char            *msg;
    int              ktora = -1;
    int              door;
    bool             slij = TRUE;

#if defined( MSDOS ) && !defined( NO_SOUND )
    if ( ch->in_room == descriptor_list->character->in_room )
	death_sound( );
#endif

    /* Zabijanie przez pulapki/zarznij/progi ustawia dt == 0.
       Losujemy, czy chcemy, zeby cos odpadlo: */
    if ( number_bits( 1 )
      && ( dt >= TYPE_HIT || dt == gsn_kick || dt == gsn_porazajacy_dotyk )
      && ilosc_czesci_ciala[ ch->race ]
      /* im mniej ma roznych czesci, tym rzadziej beda odpadac */
      && number_bits( ilosc_czesci_ciala[ ch->race ] ) )
    {
	/* chcemy cos mu oderwac/wybic, wylosujmy cos */
	int tym;

	/* losujemy, ktory bit sposrod wszystkich zapalonych */
	tym = number_bits( 16 ) % ilosc_czesci_ciala[ ch->race ] + 1;
	/* dalej szukamy zapalonego bitu o tym numerze */
	while ( tym )
	{
	    ktora++;
	    if ( IS_SET( race_table[ ch->race ].parts, 1 << ktora ) )
		tym--;
	}

	if ( czesci_ciala[ ktora ].komunikat )
	{
	    slij = FALSE;
	    act( ZM_WZROK, czesci_ciala[ ktora ].komunikat, ch, NULL, NULL, TO_ROOM );
	    if ( !IS_MUTE( ch ) )
		act( ZM_SLUCH | ZM_W_WZROK, "S`lyszysz czyj`s `smiertelny p`lacz.", ch, NULL, NULL, TO_ROOM );
	}
    }
    else
    {
	/* nie chcemy odrywac czesci, losujemy jakies komunikaty */
	switch ( number_bits( 2 ) )
	{
	    default:
		break;
	    case 0:
		if ( CZY_ZIEMIA( ch->in_room ) && pozycja >= POS_FIGHTING )
		    act( ZM_WZROK | ZM_WID_CHAR, "$n pada na ziemi`e MARTW$).", ch, NULL, NULL, TO_ROOM );
		else
		    act( ZM_WZROK | ZM_WID_CHAR, "$n zastyga w bezruchu.", ch, NULL, NULL, TO_ROOM );
		if ( !IS_MUTE( ch ) )
		    act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz czyj`s `smiertelny p`lacz.", ch, NULL, NULL, TO_ROOM );
		slij = FALSE;
		break;
	    case 1:
		/* if ( race_table[ ch->race ].parts != P_NONE ) */
		if ( !IS_SET( race_table[ ch->race ].race_abilities, RACE_NO_BLOOD ) )
		{
		    if ( CZY_WODA( ch->in_room ) )
			act( ZM_WZROK | ZM_WID_CHAR, "Krew $1 rozp`lywa si`e dooko`la.", ch, NULL, NULL, TO_ROOM );
		    else
			act( ZM_WZROK | ZM_WID_CHAR, "$n obryzguje ci`e krwi`a.", ch, NULL, NULL, TO_ROOM );
		    if ( !IS_MUTE( ch ) )
			act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz czyj`s `smiertelny p`lacz.", ch, NULL, NULL, TO_ROOM );
		    slij = FALSE;
		}
		break;
	}
    }

    if ( slij )
    {
	if ( !IS_MUTE( ch ) )
	{
	    act( ZM_SLUCH | ZM_WID_CHAR, "S`lyszysz `smiertelny p`lacz $1.", ch, NULL, NULL, TO_ROOM );
	    act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz czyj`s `smiertelny p`lacz.", ch, NULL, NULL, TO_ROOM );
	    act( ZM_W_SLUCH | ZM_WZROK | ZM_WID_CHAR, "$n przybiera poz`e pe`ln`a b`olu.", ch, NULL, NULL, TO_ROOM );
	}
	else
	    act( ZM_WZROK | ZM_WID_CHAR, "$n przybiera poz`e pe`ln`a b`olu.", ch, NULL, NULL, TO_ROOM );
    }

    if ( ktora >= 0 )
    {
	OBJ_DATA *obj;
	char     *name;
	char      buf[ MAX_STRING_LENGTH ];
	int       vnum;

	vnum		= czesci_ciala[ ktora ].vnum;

	if ( czesci_ciala[ ktora ].rasa_w_opisie
	  && ( IS_SET( race_table[ ch->race ].race_abilities, RACE_MUTE )
	    || *czesci_ciala[ ktora ].wsk != OBJ_VNUM_GLOWA ) )
	{
	    name	= race_table[ ch->race ].dopelniacz;
	}
	else
	    name	= ch->dopelniacz;

	obj		= create_object( get_obj_index( vnum ), 0 );
	/* zeby, rogi itd. nie rozkladaja sie po chwili */
	if ( obj->item_type == ITEM_FOOD
	  || obj->item_type == ITEM_DRINK_CON
	  || obj->item_type == ITEM_TRASH )
	{
	    obj->timer = number_range( 4, 7 );
	}

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->dopelniacz, name );
	free_string( obj->dopelniacz );
	obj->dopelniacz = str_dup( buf );

	sprintf( buf, obj->celownik, name );
	free_string( obj->celownik );
	obj->celownik = str_dup( buf );

	sprintf( buf, obj->biernik, name );
	free_string( obj->biernik );
	obj->biernik = str_dup( buf );

	sprintf( buf, obj->narzednik, name );
	free_string( obj->narzednik );
	obj->narzednik = str_dup( buf );

	sprintf( buf, obj->miejscownik, name );
	free_string( obj->miejscownik );
	obj->miejscownik = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	/* zatruwanie jadalnych/pitnych czesci ciala zatrutych postaci */
	if ( IS_AFFECTED( ch, AFF_POISON )
	  && ( obj->item_type == ITEM_FOOD
	    || obj->item_type == ITEM_DRINK_CON ) )
	{
	    obj->value[ 3 ].v = 1;
	}

	/* zapamietanie vnuma, np. do progow zadaniowych */
	if ( obj->item_type == ITEM_FOOD
	  || obj->item_type == ITEM_TREASURE )
	{
	    obj->value[ 1 ].v = IS_NPC( ch ) ? ch->pIndexData->vnum : -1;
	}

	/* teraz ciezar zalezy od rasy, ktorej cos odpada */
	obj->weight = UMAX( 1, race_table[ ch->race ].weight
				* czesci_ciala[ ktora ].masa / 1024 );

	obj_to_room( obj, ch->in_room );
    }

    if ( IS_NPC( ch ) || IS_MUTE( ch ) )
	msg = "S`lyszysz odg`losy czyjej`s `smierci.";
    else
	msg = "Twych uszu dobiega czyj`s `smiertelny p`lacz.";

    /* bardzo brzydka sztuczka z ch->in_room, ale dziala doskonale (poki co) */
    was_in_room = ch->in_room;
    for ( door = 0; door <= 9; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[ door ] )
	    && pexit->to_room
	    && pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( ZM_SLUCH, msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

    return;
}


/* Lam 3.12.2004: obsluga ch->deleted (to sie zdarzylo!) */
void raw_kill( CHAR_DATA *ch, CHAR_DATA *victim, bool czy_bez_walki,
		bool czy_dodac_zgon, int typ_zgonu )
{
    AFFECT_DATA   *paf;
    CLAN_DATA     *clanc, *clanv;
    CLAN_REL_DATA *relc, *relv;
    bool	   zwampirz = FALSE;
    bool           czy_maska = FALSE;

    /* Lam 19.3.2001: czesc zliczajaca zabojstwa byla po
       if ( victim->deleted ) return; i mozna bylo nabijac sobie doswiadczenie
       na Satharielu: majac x99 zabojstw zawsze dostawalo sie za niego 2 razy
       wiecej doswiadczenia nigdy nie przeskakujac z 99 na 100. */
    if ( !ch->deleted && !IS_NPC( ch ) && ch != victim )
	++ch->pcdata->killed;
    if ( !IS_NPC( victim ) )
	++victim->pcdata->dead;

    /* Qwert 27.02.04: jesli ch mial na liscie victima - usuwam */
    if ( !ch->deleted
      && !( IS_NPC( ch ) || IS_NPC( victim ) )
      && ch != victim
      && is_name2( victim->name, ch->pcdata->nienawisc ) )
    {
	char nienawisc [ MAX_STRING_LENGTH ];
	char newhate   [ MAX_STRING_LENGTH ];
	char tmp       [ MAX_INPUT_LENGTH ];
	char *list, *imie;

	if ( !str_cmp( victim->name, victim->short_descr ) )
	    imie = victim->short_descr;
	else
	    imie = victim->name;

	strcpy( nienawisc, ch->pcdata->nienawisc );
	strcpy( newhate, "" );
	list = &nienawisc[ 0 ];
	for ( ; ; )
	{
	    list = one_argument2( list, tmp );
	    if ( !tmp[ 0 ] )
		break;
	    if ( str_cmp( tmp, imie ) ) /* nie imie, wiec mozna przepisac */
	    {
		strcat( newhate, tmp );
		strcat( newhate, " " );
	    }
	}
	free_string( ch->pcdata->nienawisc );
	ch->pcdata->nienawisc = str_dup( newhate );
    }

    /* Malven, 2003.12.17: jesli obaj sa graczami i naleza do klanow bedacych
       w stanie wojny, zwiekszam liczniki zabojstw dla klanu */
    if ( !ch->deleted && !IS_NPC( ch ) && !IS_NPC( victim ) )
    {
	if ( typ_zgonu == ZGON_GRACZ )
	{
	    ++ch->pcdata->pkilled;
	    ++victim->pcdata->pdead;
	}
	else if ( typ_zgonu == ZGON_SAMOBOJSTWO )
	    ++victim->pcdata->samobojstwa;
	else if ( typ_zgonu == ZGON_WYKRWAWIENIE )
	    ++victim->pcdata->wykrwawienia;

	if ( ( clanc = ch->pcdata->clan )
	  && ( clanv = victim->pcdata->clan )
	  && ( clanc != clanv )
	  && ( clanc->status != CLAN_STAT_NOWY )
	  && ( clanc->status != CLAN_STAT_USZKODZONY )
	  && ( clanc->status != CLAN_STAT_ZAWIESZONY )
	  && ( clanv->status != CLAN_STAT_NOWY )
	  && ( clanv->status != CLAN_STAT_USZKODZONY )
	  && ( clanv->status != CLAN_STAT_ZAWIESZONY )
	  && ( relc = clan_rel_lookup( clanc, clanv ) )
	  && ( relc->war == TRUE )
	  && ( relv = clan_rel_lookup( clanv, clanc ) ) )
	{
	    char buf[ MIL ];

	    sprintf( buf, "+ %s [%s] killed by %s at %d",
		victim->name, clanv->name, ch->name, victim->in_room->vnum );
	    clan_log( clanc, buf );

	    sprintf( buf, "- %s killed by %s [%s] at %d",
		victim->name, ch->name, clanc->name, victim->in_room->vnum );
	    clan_log( clanv, buf );

	    ++relc->pkill;
	    ++relv->pdead;
	    write_clan( clanc, CLAN_WRITE_STOSUNKI );
	    write_clan( clanv, CLAN_WRITE_STOSUNKI );
	}
    }

    stop_fighting( victim, TRUE );

    PRZERWIJ_KOPANIE( victim, FALSE );
    PRZERWIJ_KARMPOJ( victim, FALSE );

/*    death_cry( victim ); */
    if ( IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_CORPSE ) )
	make_no_corpse( victim );
    else
	make_corpse( victim );

    /* po ugryzieniu przez wampira smierc powoduje zmiane rasy. */
    /* Lam 4.3.2001: zmiana rasy byla przed spadnieciem wplywow, m.in.
       "przemiany", mozna bylo modyfikowac sobie rase na stale. Teraz jest to
       zapamietywane i ewentualnie wykonywane pozniej. */
    if ( !IS_NPC( victim ) && IS_AFFECTED( victim, AFF_VAMP_BITE )
      && ( !IS_GOOD( victim ) ) )
	zwampirz = TRUE;

    for ( paf = victim->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;

	/* dlaczego to jest w petli? Chyba powinno byc && paf->type == ... */
	/* Keep the ghoul affect */
	if ( !IS_NPC( victim ) && IS_AFFECTED( victim, AFF_GHOUL ) )
	    continue;

	if ( paf->type == gsn_disguise )
	{
	    czy_maska = TRUE;
	    free_string( victim->false_name );
	    victim->false_name = str_dup( "" );
	    free_string( victim->long_descr );
	    victim->long_descr = str_dup( victim->long_descr_orig );
	    free_string( victim->long_descr_orig );
	    /* smierc powoduje strate opisu maskujacego: */
	    victim->long_descr_orig = str_dup( "" );
	}

	if ( !IS_NPC( victim )
	  && paf->type == gsn_kontrola_umyslu )
	    zabij_wszystkie_slugi( victim );

	affect_remove( victim, paf, TRUE );
    }

    if ( IS_NPC( victim ) )
    {
	if ( czy_dodac_zgon )
	{
	    victim->pIndexData->killed++;
	    kill_table[ URANGE( 0, victim->pIndexData->level, MAX_LEVEL - 1 ) ].killed++;
	}
	extract_char( victim, TRUE );
	return;
    }

    /* nie mial wplywu, ale mial opis maskujacy */
    if ( !czy_maska && *victim->false_name && !IS_NPC( victim ) )
    {
	free_string( victim->false_name );
	victim->false_name = str_dup( "" );
	free_string( victim->long_descr_orig );
	/* smierc powoduje strate opisu maskujacego: */
	victim->long_descr_orig = str_dup( "" );
    }

    if ( zwampirz && race_table[ victim->race ].parts == P_HS )
    {
	if ( !IS_NPC( victim ) )
	{
	    char kto[ MAX_INPUT_LENGTH ];

	    victim->pcdata->wampiryzm.rasa = victim->race;
	    victim->pcdata->wampiryzm.ile++;
	    free_string( victim->pcdata->wampiryzm.strefa );
	    victim->pcdata->wampiryzm.strefa =
		    str_dup( victim->pcdata->strefa->nazwa );
	    sprintf( kto, victim->pcdata->wampiryzm.pochodzenie,
		     ascdata( localtime( &current_time ) ) );
	    free_string( victim->pcdata->wampiryzm.pochodzenie );
	    victim->pcdata->wampiryzm.pochodzenie = str_dup( kto );
	}
	victim->race = zr_wampir;
	victim->alignment = UMIN( victim->alignment, 0 );
    }

    extract_char( victim, FALSE );
    victim->armor        = 0;
    victim->marmor       = 0;
    victim->position     = POS_RESTING;
    victim->hit          = 1;
    victim->mana         = victim->max_mana;
    victim->move         = victim->max_move;
    victim->hitroll      = 0;
    victim->damroll      = 0;
    victim->saving_throw = 0;
    victim->mod_str = 0; /* to jest dobra rzecz -Lam */
    victim->mod_int = 0; /* chociaz i tak od czasu kiedy przedmioty zapisuja */
    victim->mod_wis = 0; /* sie zdjete z ciala, te linijki sa praktycznie */
    victim->mod_dex = 0; /* nieprzydatne, tylko czary ewentualnie */
    victim->mod_con = 0;
    victim->pcdata->temp_flags = 0; /* zeby nie byl slepy po smierci? */
    victim->wait = 0; /* Lam 13.4.2005 */
    victim->brzeczenie = 0;
    if ( CZY_WAMPIR( victim ) ) /* Lam 3.12.2004: bylo ch */
    {
	victim->pcdata->condition[ COND_FULL   ] = 4; /* 3 to juz glod */
	victim->pcdata->condition[ COND_THIRST ] = 4;
    }
    else
    {
	victim->pcdata->condition[ COND_FULL   ] = 10;
	victim->pcdata->condition[ COND_THIRST ] = 10;
    }
    victim->pcdata->condition[ COND_DRUNK      ] = 0;

/*    save_char_obj( victim ); -Lam: wyciete 21.5.98 */
    return;
}


bool czy_grupa_wrogow( CHAR_DATA *ch, CHAR_DATA *gch )
{
    if ( !str_infix( race_table[ ch->race ].ang, race_table[ gch->race ].hate ) )
	return TRUE;

    if ( !IS_NPC( gch )
      && gch->klasa == CLASS_PALADYN
      && ( ch->race == zr_martwiak
	|| CZY_WAMPIR( ch )
	|| IS_EVIL( ch )
	|| ( !IS_NPC( ch )
	  && ( IS_SET( ch->act, PLR_KILLER )
	    || IS_SET( ch->act, PLR_THIEF ) ) ) ) )
	return TRUE;

    if ( !IS_NPC( ch )
      && ch->klasa == CLASS_PALADYN
      && ( gch->race == zr_martwiak
	|| CZY_WAMPIR( gch )
	|| IS_EVIL( gch )
	|| ( !IS_NPC( gch )
	  && ( IS_SET( gch->act, PLR_KILLER )
	    || IS_SET( gch->act, PLR_THIEF ) ) ) ) )
	return TRUE;

    return FALSE;
}


/* Lam 8.5.2005: zaczyna zwracac prawde, jesli ktos cos dostal */
bool group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    char       buf[ MAX_STRING_LENGTH ];
    int        members;
    int        xp;
    int        li;
    bool       czy;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( /* IS_NPC( ch ) || */ victim == ch )
	return FALSE;

    members = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
    }

    lch = ( ch->leader ) ? ch->leader : ch;

    czy = FALSE;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) )
	    continue;

	if ( IS_NPC( gch ) )
	    continue;

	if ( gch->level - lch->level > 8 )
	{
	    send_to_char( "To nie jest odpowiednia grupa dla ciebie.\n\r", gch );
	    continue;
	}

	if ( gch->level - lch->level < -8 )
	{
	    send_to_char( "Ta grupa jest dla ciebie za dobra.\n\r",  gch );
	    continue;
	}

	xp = xp_compute( gch, victim ) / members;
/*		/ ( ( members > 2 ) ? ( members - 1 ) : 1 ); */
	/* Zaokraglanie do 250 PO podzieleniu przez ilosc czlonkow, a nie przed */
	xp = UMIN( 250, xp );

	if ( ( lch != gch && czy_grupa_wrogow( lch, gch ) )
	  || ( ch != gch && czy_grupa_wrogow( ch, gch ) ) )
	{
	    send_to_char( "Tracisz trzeci`a cz`e`s`c swoich punkt`ow za grup`e z naturalnymi wrogami.\n\r", gch );
	    xp -= xp / 3;
	}

	if ( ( gch == ch ) && ( ( gch->pcdata->killed + 1 ) % 100 == 0 ) )
	{
	    sprintf( buf, "{BTo tw`oj %d przeciwnik. Dostajesz za niego 2 * wi`ecej do`swiadczenia!{x\n\r", ch->pcdata->killed + 1 );
	    send_to_char( buf, gch );
	    xp *= 2;
	}

	li = liczba( xp );
	sprintf( buf, "{GDostajesz %d punkt%s do`swiadczenia.{x\n\r",
		xp, li == 1 ? "" : li == 2 ? "y" : "`ow" );
	send_to_char( buf, gch );
	gain_exp( gch, xp );

	sprawdz_ubranie( gch );

	if ( xp )
	    czy = TRUE;
    }

    return czy;
}


/* Lam 19.11.2005 */
const int przelicznik_doswiadczenia[ 101 ] =
{
	300,
	300, 250, 200, 150, 140, 130, 125, 120, 115, 110,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100,  99,  98,  96,  94,  92,  90,  87,  85,  80,
	 75,  70,  65,  60,  55,  55,  55,  55,  55,  50
};


/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 *
 * Lam: zmniejszylem doswiadczenie postaciom z uswieceniem, plonaca tarcza itp.
 * oraz postaciom, ktore walcza bronia.
 *
 * Qwert: dostosowuje to do mnicha. Wczesniej zbyt latwo sie nim gralo.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    float     bonus;
    int       xp;
    int       align;
    int       extra;
    int       level;
    int       number;

    /* Lam 9.12.1999: w zwiazku z dozwoleniem walk z bohaterami, smiertelni nie
       moga na tym zyskiwac. Jesli wiec jakis bohater sie podlozy, nie bedzie
       wart ani jednego punktu. */
    if ( !IS_NPC( victim ) && victim->level >= LEVEL_HERO )
	return 0;

    /* Lam 7.4.99: premia za profesje */
    bonus = IS_NPC( gch ) ? 1.0 : class_table[ gch->klasa ].bonus;

    /* Lam 27.11.1999: zmiana na wniosek RenTora, bylo tu bezsensowne
       ograniczenie poziomu moba, niszczace przyjemnosc z gry w grupie */
    xp    = 105 - URANGE( -10, gch->level - victim->level, 10 ) * 25;
    align = gch->alignment - victim->alignment;

    /* Lam 20.5.2000: 500 na 1000
	   13.9.2000: tylko przy liczeniu premii */
    if ( align > 500 )
    {
	gch->alignment  = UMIN( gch->alignment + ( align - 500 ) / 4,  1000 );
	if ( align > 1000 ) bonus *= (float) 5/4;
	if ( CZY_WAMPIR( gch ) ) gch->alignment = UMIN( gch->alignment, 0 );
    }
    else if ( align < -500 )
    {
	gch->alignment  = UMAX( gch->alignment + ( align + 500 ) / 4, -1000 );
	if ( align < -1000 ) bonus *= (float) 5/4;
    }
    else
    {
	gch->alignment -= gch->alignment / 4;
	bonus *= (float) 3/4;
    }

    if ( HAS_SANCTUARY( victim ) )
	bonus += (float) 1/3; /* Lam 20.5.2000: 1/2 -> 1/3 */

    if ( IS_AFFECTED( victim, AFF_FLAMING )
      && !IS_AFFECTED ( gch, AFF_LODOWA_TARCZA ) )
	bonus += (float) 4/10;

    if ( !IS_AFFECTED( victim, AFF_FLAMING )
      && IS_AFFECTED ( gch, AFF_LODOWA_TARCZA ) )
	bonus += (float) 4/10;

    if ( HAS_SANCTUARY( gch ) )
	bonus -= (float) 1/4;

    /* przyspieszenie jest bardzo dobrym czarem */
    if ( IS_AFFECTED( gch, AFF_PRZYSPIESZENIE ) )
	bonus -= (float) 1/5;

    if ( IS_AFFECTED( victim, AFF_PRZYSPIESZENIE ) )
	bonus += (float) 1/6;

    if ( IS_AFFECTED( gch, AFF_SPOWOLNIENIE ) )
	bonus += (float) 1/6;

    if ( IS_AFFECTED( victim, AFF_SPOWOLNIENIE ) )
	bonus -= (float) 1/5;

    /* Qwert: usuwam potracanie premii, w koncu inne profesje tez maja
       odnawiania, ktore nic ich nie kosztuja, a nie dosc ze regen/degen
       zabiera mane, to i tak strasznie krotko trwa

    if ( IS_AFFECTED( victim, AFF_REGENERACJA ) )
	bonus += (float) 1/7;

    if ( IS_AFFECTED( gch, AFF_REGENERACJA ) )
	bonus -= (float) 1/6;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
	bonus -= (float) 1/6;

    if ( IS_AFFECTED( gch, AFF_DEGENERACJA ) )
	bonus += (float) 1/7;
    */
    if ( IS_AFFECTED( gch, AFF_FLAMING ) )
	bonus -= (float) 1/6;

    if ( IS_AFFECTED( gch, AFF_LODOWA_TARCZA ) )
	bonus -= (float) 1/6;

    if ( get_eq_char( victim, WEAR_WIELD ) )
	bonus += (float) 1/4;

    if ( get_eq_char( victim, WEAR_WIELD_2 ) )
	bonus += (float) 1/5;

    if ( get_eq_char( gch, WEAR_WIELD ) )
	bonus -= (float) 1/8;

    if ( get_eq_char( gch, WEAR_WIELD_2 ) )
	bonus -= (float) 1/12;

    if ( !str_infix( race_table[ victim->race ].ang,
		    race_table[ gch->race ].hate ) )
	bonus += (float) 1/8;

    if ( victim->race == gch->race )
	bonus -= (float) 1/8;

    if ( IS_NPC( victim ) )
    {
	if ( IS_SET( victim->act, ACT_AGGRESSIVE ) )
	    bonus += (float) 1/20;

	if ( victim->pIndexData->pShop != 0 )
	    bonus -= (float) 1/4;

	if ( victim->spec_fun != 0 )
	{
	    if (   victim->spec_fun == spec_lookup( "spec_breath_any"       )
		|| victim->spec_fun == spec_lookup( "spec_cast_psionicist"  )
		|| victim->spec_fun == spec_lookup( "spec_cast_undead"      )
		|| victim->spec_fun == spec_lookup( "spec_breath_gas"       )
		|| victim->spec_fun == spec_lookup( "spec_cast_mage"        ) )
		bonus += (float) 1/3;

	    if (   victim->spec_fun == spec_lookup( "spec_breath_fire"      )
		|| victim->spec_fun == spec_lookup( "spec_breath_cold"      )
		|| victim->spec_fun == spec_lookup( "spec_breath_acid"      )
		|| victim->spec_fun == spec_lookup( "spec_breath_lightning" )
		|| victim->spec_fun == spec_lookup( "spec_cast_cleric"      )
		|| victim->spec_fun == spec_lookup( "spec_cast_ghost"       ) )
		bonus += (float) 1/5;

	    if (   victim->spec_fun == spec_lookup( "spec_poison"           )
		|| victim->spec_fun == spec_lookup( "spec_thief"            ) )
		bonus += (float) 1/20;

	    if ( victim->spec_fun == spec_lookup( "spec_cast_adept"         ) )
		bonus -= (float) 1/2;
	}
    }
    else
    {
/*
	if ( !IS_SET( victim->act, PLR_REGISTER ) )
	    bonus = 0.0;
	else
*/
	    bonus *= (float) 2;
    }
    xp = (int) ( xp * bonus );

    /*
     * Popularnosc ofiary liczona na tle innych indeksow mobow na danym
     * poziomie (czyli zabijanie zwierzat na arenie na 1 poz. nie obniza
     * popularnosci kapitana na 95).
     * Na podstawie innych zabojstw mobow na danym poziomie jest liczona
     * srednia ilosc zgonow dowolnego moba (jako wszystkie zgony podzielone
     * przez ilosc indeksow mobow na danym poziomie).
     * W tym miejscu mamy juz obliczone nalezne doswiadczenie i podlega ono
     * modyfikacji:
     *   -1/8 za kazde zab. powyzej sredniej (do -50%)
     *   +1/8 za kazde zab. ponizej sredniej (do +25%)
     */
    if ( IS_NPC( victim ) )
    {
	level  = URANGE( 0, victim->pIndexData->level, MAX_LEVEL - 1 );
	number = UMAX( 1, kill_table[ level ].number );
	extra  = victim->pIndexData->killed - kill_table[ level ].killed
	  / number;
	xp    -= xp * URANGE( -2, extra, 4 ) / 8;
    }

    /* Lam 19.11.2005 */
    xp = xp * przelicznik_doswiadczenia[ URANGE( 1, gch->level, 100 ) ] / 100;

    xp = number_range( xp * 7 / 8, xp * 9 / 8 );
    xp = UMAX( 0, xp );

    /* ograniczenie do 250 w group_gain( ) */
    return xp;
}


void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt,
		 int wpn )
{
    /* oryginal:
    static char * const  attack_table   [ ] =
    {
	"hit",
	"slice",  "stab",    "slash", "whip", "claw",
	"blast",  "pound",   "crush", "grep", "bite",
	"pierce", "suction", "chop"
    }; */
    static char * const attack_table[ ] =
    {
	"uderzenie",
	"ciachni`ecie", "d`xgni`ecie", "dziabni`ecie", "smagni`ecie",
	"drapni`ecie", "trzepni`ecie", "`lomotni`ecie", "walni`ecie",
	"rozszarpywanie", "ugryzienie", "ci`ecie", "ssanie", "r`abni`ecie",
	"uk`lucie"
    };
    static char * const attack_table_dop[ ] =
    {
	"uderzenia",
	"ciachni`ecia", "d`xgni`ecia", "dziabni`ecia", "smagni`ecia",
	"drapni`ecia", "trzepni`ecia", "`lomotni`ecia", "walni`ecia",
	"rozszarpywania", "ugryzienia", "ci`ecia", "ssania", "r`abni`ecia",
	"uk`lucia"
    };

    const  char         *vp;
    const  char         *vp2;
    const  char         *attack;
	   char          buf            [ MAX_STRING_LENGTH ];
	   char          buf1           [ 2 * MIL ];
	   char          buf2           [ 2 * MIL ];
	   char          buf3           [ 2 * MIL ];
	   char          buf4           [ 2 * MIL ];
	   char          buf5           [ 2 * MIL ];
	   char		 buf6		[ 2 * MIL ];
	   char		 gwbuf		[ 20 ];
    const  char	*	 koncowka;
    const  char	*	 srodek;
	   int		 odam;
	   char          punct;

    if ( dam == 0 )
    {
	if ( ch != victim )
	{
	    /* Lam 9.12.2000: male f to walka, wielkie F to chybianie */
	    super_act( OPC_CHYBIANIE, ZM_ZAWSZE, "{FTw`oj atak chybia $#.{x", ch, NULL, victim, TO_CHAR, FALSE );
	    super_act( OPC_CHYBIANIE, ZM_WZROK, "{FAtak $1 chybia ci`e.{x", ch, NULL, victim, TO_VICT, FALSE );
	    super_act( OPC_CHYBIANIE, ZM_WZROK | ZM_WID_CHAR, "{FAtak $1 chybia $#.{x", ch, NULL, victim, TO_NOTVICT, FALSE );
	    /* widze, kto chybia, a ze "kogos", to niewazne */
	}
	else
	{
	    super_act( OPC_CHYBIANIE, ZM_ZAWSZE, "{FPr`obujesz si`e uderzy`c, ale chybiasz.{x", ch, NULL, victim, TO_CHAR, FALSE );
	    super_act( OPC_CHYBIANIE, ZM_WZROK | ZM_WID_CHAR, "{F$n pr`obuje si`e uderzy`c, ale chybia.{x", ch, NULL, victim, TO_ROOM, FALSE );
	}

	return;
    }

    koncowka = "e";
    srodek = "o";
    odam = dam;
    dam *= 100;
    if ( victim->hit > 0 )
	dam /= victim->hit;

	 if ( dam <=   1 ) { vp = "`laskocze";
			    vp2 = "`laskocz`a"; }
    else if ( dam <=   2 ) { vp = "drapie";
			    vp2 = "drapi`a"; }
    else if ( dam <=   4 ) { vp = "trafia";
			    vp2 = "trafiaj`a"; }
    else if ( dam <=   6 ) { vp = "kaleczy";
			    vp2 = "kalecz`a"; }
    else if ( dam <=   8 ) { vp = "rani";
			    vp2 = "rani`a"; }
    else if ( dam <=  10 ) { vp = "t`lucze";
			    vp2 = "t`luk`a"; }
    else if ( dam <=  20 ) { vp = "`cwiartuje";
			    vp2 = "`cwiartuj`a"; }
    else if ( dam <=  30 ) { vp = "dewastuje";
			    vp2 = "dewastuj`a"; }
    else if ( dam <=  40 ) { vp = "mia`zd`zy";
			    vp2 = "mia`zd`z`a"; }
    else if ( dam <=  50 ) { vp = "KALECZY";
			    vp2 = "KALECZ`A"; }
    else if ( dam <=  60 ) { vp = "* DEZINTEGRUJE *";
			    vp2 = "* DEZINTEGRUJ`A *"; }
    else if ( dam <=  70 ) { vp = "** ROZNOSI **";
			    vp2 = "** ROZNOSZ`A **"; }
    else if ( dam <=  80 ) { vp = "** MASAKRUJE **";
			    vp2 = "** MASAKRUJ`A **"; }
    else if ( dam <=  90 ) { vp = "*** DEMOLUJE ***";
			    vp2 = "*** DEMOLUJ`A ***"; }
    else if ( dam <  100 ) { vp = "{y*** UNICESTWIA ***{f";
			    vp2 = "{y*** UNICESTWIAJ`A ***{f"; }
    else                   { vp = "{Y*** UNICESTWIA ***{f";
			    vp2 = "{Y*** UNICESTWIAJ`A ***{f"; }

    punct   = ( dam <= 50 ) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
	if ( ch->race > MAX_RACE )
	{
	    bug( "Dam_message: %d invalid race", ch->race );
	    ch->race = 0;
	}

	attack = race_table[ ch->race ].dmg_message;
	koncowka = ( race_table[ ch->race ].rodzaj == 1 ) ?
		     "" : ( race_table[ ch->race ].rodzaj == 2 ) ? "a" : "e";
	srodek = ( race_table[ ch->race ].rodzaj == 1 ) ? "`o" : "o";
	if ( race_table[ ch->race ].rodzaj == 3 )
	    vp = vp2;

	sprintf( buf1, "{fTw%sj%s %s %s $#%c{x",   srodek, koncowka, attack, vp, punct );
	sprintf( buf2, "{f%s $1 %s ci`e%c{x",      attack, vp, punct );
	sprintf( buf3, "{f%s $1 %s $#%c{x",        attack, vp, punct );
	sprintf( buf4, "{fTw%sj%s %s %s ci`e%c{x", srodek, koncowka, attack, vp, punct );
	sprintf( buf5, "{f%s $1 %s $v%c{x",        attack, vp, punct );
	strcpy( buf6, "{fS`lyszysz g`luchy odg`los uderzenia.{x" );
    }
    else
    {
	buf6[ 0 ] = '\0';
	if ( dt >= 0 && dt < MAX_SKILL )
	{
	    /* atak czarami */
	    attack      = skill_table[ dt ].noun_damage;
	    koncowka	= ( skill_table[ dt ].rodzaj == 1 ) ? ""
			: ( skill_table[ dt ].rodzaj == 2 ) ? "a" : "e";
	    srodek	= ( skill_table[ dt ].rodzaj == 1 ) ? "`o" : "o";
	    if ( skill_table[ dt ].rodzaj == 3 )
		vp = vp2;
	}
	else if ( dt >= TYPE_HIT
	       && dt - TYPE_HIT <
	          (int) ( sizeof( attack_table ) / sizeof( attack_table[ 0 ] ) ) )
	{
	    /* atak bronia */
	    attack = attack_table[ dt - TYPE_HIT ];
	    sprintf( buf6, "{fS`lyszysz odg`los %s.{x",
		attack_table_dop[ dt - TYPE_HIT ] );
	}
	else
	{
	    /* bron o niezidentyfikowanych obrazeniach */
	    sprintf( buf, "Dam_message: bad dt %d caused by %s.", dt,
		    ch->name );
	    bug( buf, 0 );
	    dt      = TYPE_HIT;
	    attack  = attack_table[ 0 ];
	}

	if ( dt > TYPE_HIT && is_wielding_specweapon( ch, wpn, ITEM_POISONED ) )
	{
	    /* nie ma sprawdzania tw`oj/twoja/twoje, bo to tylko bron -Lam */
	    sprintf( buf1, "{fTwoje truj`ace %s %s $#%c{x", attack, vp, punct );
	    sprintf( buf2, "{fTruj`ace %s $1 %s ci`e%c{x", attack, vp, punct );
	    sprintf( buf3, "{fTruj`ace %s $1 %s $#%c{x", attack, vp, punct );
	    sprintf( buf4, "{fTwoje truj`ace %s %s ci`e%c{x", attack, vp, punct );
	    sprintf( buf5, "{fTruj`ace %s $1 %s $v%c{x", attack, vp, punct );
	}
	else
	{
	    /* a tu juz musi byc */
	    sprintf( buf1, "{fTw%sj%s %s %s $#%c{x", srodek, koncowka, attack, vp, punct );
	    sprintf( buf2, "{f%s $1 %s ci`e%c{x", attack, vp, punct );
	    sprintf( buf3, "{f%s $1 %s $#%c{x", attack, vp, punct );
	    sprintf( buf4, "{fTw%sj%s %s %s ci`e%c{x", srodek, koncowka, attack, vp, punct );
	    sprintf( buf5, "{f%s $1 %s $v%c{x", attack, vp, punct );
	}
    }

    strcpy( gwbuf, " [" );
    if ( odam > 100 )
    {
	int gwiazdki = UMIN( odam / 100, 10 );
	int i;

	strcat( gwbuf, "{R" );
	for ( i = 0; i < 10; i++ )
	{
	    if ( i == gwiazdki )
		strcat( gwbuf, "{x*" );
	    else
		strcat( gwbuf, "*" );
	}
	if ( gwiazdki == 10 )
	    strcat( gwbuf, "{x" );
    }
    else
    {
	int gwiazdki = UMIN( odam / 10 + 1, 10 );
	int i;

	for ( i = 0; i < 10; i++ )
	{
	    if ( i < gwiazdki )
		strcat( gwbuf, "*" );
	    else
		strcat( gwbuf, " " );
	}
    }
    strcat( gwbuf, "]" );

    if ( victim != ch )
    {
	super_act( OPC_GWIAZDKI, ZM_ZAWSZE,
		buf1, ch, NULL, victim, TO_CHAR, FALSE );
	super_act( OPC_GWIAZDKI, ZM_PRZYT,
		buf2, ch, NULL, victim, TO_VICT, FALSE );
	super_act( OPC_GWIAZDKI, ZM_WZROK | ZM_WID_CHAR,
		buf3, ch, NULL, victim, TO_NOTVICT, FALSE );
	super_act( OPC_GWIAZDKI, ZM_WZROK | ZM_WID_VICT | ZM_W_WID_CHAR,
		buf3, ch, NULL, victim, TO_NOTVICT, FALSE );

	strcat( buf1, gwbuf );
	strcat( buf2, gwbuf );
	strcat( buf3, gwbuf );
	super_act( OPC_GWIAZDKI | OPC_ODWROC, ZM_ZAWSZE,
		buf1, ch, NULL, victim, TO_CHAR, FALSE );
	super_act( OPC_GWIAZDKI | OPC_ODWROC, ZM_PRZYT,
		buf2, ch, NULL, victim, TO_VICT, FALSE );
	super_act( OPC_GWIAZDKI | OPC_ODWROC, ZM_WZROK | ZM_WID_CHAR,
		buf3, ch, NULL, victim, TO_NOTVICT, FALSE );
	super_act( OPC_GWIAZDKI | OPC_ODWROC, ZM_WZROK | ZM_WID_VICT | ZM_W_WID_CHAR,
		buf3, ch, NULL, victim, TO_NOTVICT, FALSE );

	/* odglosy bez gwiazdek */
	if ( *buf6 )
	    act( ZM_SLUCH | ZM_W_WID_VICT | ZM_W_WID_CHAR,
		buf6, ch, NULL, victim, TO_NOTVICT );
    }
    else
    {
	super_act( OPC_GWIAZDKI, ZM_ZAWSZE,
		buf4, ch, NULL, victim, TO_CHAR, FALSE );
	super_act( OPC_GWIAZDKI, ZM_WZROK | ZM_WID_CHAR,
		buf5, ch, NULL, victim, TO_ROOM, FALSE );

	strcat( buf4, gwbuf );
	strcat( buf5, gwbuf );
	super_act( OPC_GWIAZDKI | OPC_ODWROC, ZM_ZAWSZE,
		buf4, ch, NULL, victim, TO_CHAR, FALSE );
	super_act( OPC_GWIAZDKI | OPC_ODWROC, ZM_WZROK | ZM_WID_CHAR,
		buf5, ch, NULL, victim, TO_ROOM, FALSE );
    }

    return;
}


/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
bool disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    /* Lam 27.9.2000: rasy bez rak nie rozbrajaja nigdy */
    if ( ( ROZMIAR( ch ) - ROZMIAR( victim ) ) < -2
      || !IS_SET( race_table[ ch->race ].parts, P_RECE ) )
	return FALSE;

    if ( is_affected( ch, gsn_zaglada ) )
	return FALSE;

    obj = get_eq_char( victim, WEAR_WIELD );

    if ( ( !obj || IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
      && !( obj = get_eq_char( victim, WEAR_WIELD_2 ) ) )
	return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
	return FALSE;

    if ( !get_eq_char( ch, WEAR_WIELD   )
      && !get_eq_char( ch, WEAR_WIELD_2 )
      && number_bits( 2 ) == 0 )
	return FALSE;

    if ( !IS_NPC( victim )
     && number_percent( ) < ( victim->pcdata->learned[ gsn_strong_wield ] / 2 ) )
    {
	act( ZM_ZAWSZE, "{g$N za mocno trzyma sw`a bro`n.{x",
		ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "{g$n nie potrafi wytr`aci`c ci broni z r`eki.{x",
		ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "{g$2 nie udaje si`e rozbroi`c $!, gdy`z strasznie silnie dzier`zy on$A bro`n.{x",
		ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_VICT | ZM_WID_CHAR, "$n wywija broni`a.",
		ch, NULL, victim, TO_NOTVICT );
	act( ZM_SLUCH | ZM_W_WID_CHAR | ZM_W_WID_VICT, "S`lyszysz chrz`est broni.",
		ch, NULL, victim, TO_NOTVICT );
	uzyj( victim, gsn_strong_wield, "mocnym trzymaniu swej broni" );
	return FALSE;
    }

    act( ZM_ZAWSZE, "{GRozbrajasz $#!{x", ch, NULL, victim, TO_CHAR );
    act( ZM_PRZYT, "{R$n ci`e ROZBRAJA!{x", ch, NULL, victim, TO_VICT );
    /* mozna dodac obj, zrobic cos z WID_OBJ1 */
    act( ZM_WZROK | ZM_DYLEMAT, "{y$n ROZBRAJA $#!{x", ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_NPC( victim ) || IS_OBJ_STAT( obj, ITEM_LOYAL )  /* Lam */
      || IS_OBJ_STAT( obj, ITEM_QUEST ) || IS_OBJ_STAT( obj, ITEM_NODROP ) )
	obj_to_char( obj, victim );
    else
	obj_to_room( obj, victim->in_room );

    return TRUE;
}


/*
 * Podciecie postaci.
 * Lam 28.8.98: stworzenia bez nog nie podcinaja ani nie moga zostac podciete
 */
void trip( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->wait || victim->wait )
	return;

    if ( !IS_SET( race_table[ ch->race ].parts, P_NOGI )
      || !IS_SET( race_table[ victim->race ].parts, P_NOGI ) )
	return;

    if ( is_affected( ch, gsn_zaglada ) )
	return;

    if ( ch->race == zr_smok )
    {
	act( ZM_ZAWSZE, "{WPrzewracasz $# podmuchem ze swych skrzyde`l!{x", ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "{W$n przewraca ci`e podmuchem ze swych skrzyde`l!{x", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n przewraca $# podmuchem ze swych skrzyde`l.", ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT, "$N si`e przewraca.", ch, NULL, victim, TO_NOTVICT );
    }
    else
    {
	if ( IS_FLYING( victim ) )
	    return;

	/* FIXME: dodac porownanie wielkosci ras */
	if ( !IS_NPC( victim )
	  && number_percent( ) < ( victim->pcdata->learned[ gsn_no_trip ] / 2 ) )
	{
	    act( ZM_ZAWSZE, "{g$N za mocno trzyma si`e na nogach.{x", ch, NULL, victim, TO_CHAR );
	    act( ZM_PRZYT, "{g$n nie potrafi ci`e podci`a`c.{x", ch, NULL, victim, TO_VICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "{g$2 nie udaje si`e podci`a`c $!.{x", ch, NULL, victim, TO_NOTVICT );
	    WAIT_STATE( ch, PULSE_VIOLENCE ); /* proba podciecia troche trwa */
	    uzyj( victim, gsn_no_trip, "mocnym trzymaniu si`e na nogach" );
	    return;
	}

	if ( !IS_NPC( victim ) /* duzo latwiej podskoczyc niz stac prosto dalej */
	  && number_percent( ) < victim->pcdata->learned[ gsn_jump ] * 7 / 10 )
	{
	    act( ZM_ZAWSZE, "{g$N podskoczy`l$A przy pr`obie podci`ecia.{x", ch, NULL, victim, TO_CHAR );
	    act( ZM_PRZYT, "{gPodskakujesz, gdy $n pr`obuje ci`e podci`a`c.{x", ch, NULL, victim, TO_VICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "{g$N podskoczy`l$A, gdy $n pr`obowa`l$a $V podci`a`c.{x", ch, NULL, victim, TO_NOTVICT );
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT, "{g$N podskoczy`l$A.{x", ch, NULL, victim, TO_NOTVICT );
	    WAIT_STATE( ch, PULSE_VIOLENCE ); /* proba podciecia troche trwa */
	    uzyj( victim, gsn_jump, "podskakiwaniu" );
	    return;
	}

	act( ZM_ZAWSZE, "{WPodcinasz $#!{x", ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "{W$n ci`e podcina i przewracasz si`e!{x", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n podcina $# i $N przewraca si`e.", ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT, "$N si`e przewraca.", ch, NULL, victim, TO_NOTVICT );
    }

    WAIT_STATE( ch, PULSE_VIOLENCE );
    WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
    /* victim->position = POS_RESTING; */

    return;
}


KOMENDA( do_kill )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Z kim walk`e rozpocz`a`c?\n\r", ch );
	return;
    }

    if ( !ch->fighting )
	pp_get( ch, arg ); /* Qwert: tego moze uzyc kazdy */

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	if ( is_fighting( ch, victim ) )
	    act( ZM_ZAWSZE, "Ju`z z $W walczysz.", ch, NULL, victim, TO_CHAR );
	else
	    send_to_char( "Ju`z walczysz, zajmij si`e t`a walk`a.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    if ( victim == ch )
    {
	if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA ) )
	{
	    send_to_char( "Usi`lujesz si`e zabi`c. B`edzie bola`lo!\n\r", ch );
	    multi_hit( ch, ch, TYPE_UNDEFINED );
	    return;
	}
	else
	{
	    bug( "is_safe szwankuje", 0 );
	    STC( "Je`sli na pewno chcesz si`e zabi`c, u`zyj polecenia \"morduj\".\n\r", ch );
	    return;
	}
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );

    return;
}


KOMENDA( do_murder )
{
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];

    buf[ 0 ] = '\0';
    one_argument( argument, arg );

    /* przypadek "morduj & cosdalej" - wycinamy & */
    if ( !str_cmp( arg, "&" ) )
    {
	argument = one_argument( argument, arg );
	one_argument( argument, arg );
    }

    if ( !str_prefix( arg[ 0 ] == '&' ? arg + 1 : arg, "morduj" ) )
    {
	STC( "Sk`ladnia: morduj <polecenie, kt`orym chcesz zaatakowa`c>\n\r", ch );
	STC( "Przyk`lad: morduj zabij Aramis\n\r", ch );
	STC( "          morduj c wybuch a\n\r", ch );
	return;
    }

    if ( arg[ 0 ] != '&' )
	strcat( buf, "&" );

    strcat( buf, argument );

    if ( !IS_NPC( ch ) )
	SET_BIT( ch->act, PLR_CHCE_BYC_MORDERCA );

    ch_printf( ch, "{GWywo`luj`e{x: %s.\n\r", argument );
    /* Lam 16.7.2006: */
    sprintf( ostatnie_morduj, "%s: morduj %s", ch->name, argument );
    interpret( ch, buf, INTERP_NORMAL );

    if ( !IS_NPC( ch ) )
	REMOVE_BIT( ch->act, PLR_CHCE_BYC_MORDERCA );

    return;
}


/*
 * I'm only allowing backstabbing with the primary weapon...immortals
 * who wield two weapons, with the first not being a dagger, will be
 * unable to backstab or circle.  Tough cookie.  --- Thelonius
 */
KOMENDA( do_backstab )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    if ( !moze_uzyc( ch, gsn_backstab )
      || ( !IS_NPC( ch ) && !ch->pcdata->learned[ gsn_backstab ] ) )
    {
	send_to_char( "Zostaw to fachowcom.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo zasztyletowa`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Jak chcesz si`e na siebie zaczai`c?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
      return;

    /* Lam 25.5.98: zawsze mnie denerwowalo na innych mudach, ze majac bron
       dzgajaca (stab) nie moglem nia sztyletowac (backstab) */
    if ( !( obj = get_eq_char( ch, WEAR_WIELD ) )
      || ( obj->value[ 3 ].v != 11 && obj->value[ 3 ].v != 2 ) )
    {
	send_to_char( "Musisz dzier`zy`c odpowiedni`a bro`n.\n\r", ch );
	return;
    }

    if ( victim->fighting )
    {
	send_to_char( "Nie mo`zesz zaj`s`c od ty`lu walcz`acej osoby.\n\r", ch );
	return;
    }

    if ( IS_AWAKE( victim ) && victim->hit < victim->max_hit )
    {
	act( ZM_ZAWSZE, "$N jest rann$Y i podejrzliw$Y... nie potrafisz si`e zaczai`c.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    /* Qwert: wycinam, bo i tak jest to sprawdzane w damage( ), a tutaj
       doprowadzalo w pewnych przypadkach do podwojnego mordercy
    check_killer( ch, victim ); */

    WAIT_STATE( ch, skill_table[ gsn_backstab ].beats );
    if ( !IS_AWAKE( victim )
      || IS_NPC( ch )
      || number_percent( ) <= ch->pcdata->learned[ gsn_backstab ] )
    {
	uzyj( ch, gsn_backstab, "sztyletowaniu" );
	check_killer( ch, victim );
	multi_hit( ch, victim, gsn_backstab );
    }
    else
	damage( ch, victim, 0, gsn_backstab, WEAR_WIELD, FALSE );

    return;
}


KOMENDA( do_circle_skill )
{
    OBJ_DATA  *obj;
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    if ( !moze_uzyc( ch, gsn_circle )
      || ( !IS_NPC( ch ) && !ch->pcdata->learned[ gsn_circle ] ) )
    {
	send_to_char( "Zostaw to fachowcom.\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	return;

    if ( !ch->fighting )
    {
	send_to_char( "`Zeby to robi`c, musisz walczy`c.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
	victim = FIGHTING( ch );
    else
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	    return;
	}

    if ( victim == ch )
    {
	send_to_char( "Kr`ecisz si`e wko`lo. Juhu!\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
      return;

    if ( victim != FIGHTING( ch ) )
    {
	send_to_char( "Jedna walka naraz.\n\r", ch );
	return;
    }

    if ( !victim->fighting )
    {
	act( ZM_ZAWSZE, "Czemu? $Z niczego nikomu nie robi.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !is_same_group( ch, FIGHTING( victim ) ) )
    {
	send_to_char( "Mo`ze zajmiesz si`e sob`a?\n\r", ch );
	return;
    }

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	if ( FIGHTING( rch ) == ch )
	    break;

    if ( rch )
    {
	ASTC( "Jeste`s zbyt zaj`et$y obrywaniem.", ch );
	return;
    }

    /* Lam: rowniez bron dzgajaca (2, stab) */
    if ( !( obj = get_eq_char( ch, WEAR_WIELD ) )
	|| ( obj->value[ 3 ].v != 11 && obj->value[ 3 ].v != 2 ) )
    {
	send_to_char( "Musisz dzier`zy`c odpowiedni`a bro`n.\n\r", ch );
	return;
    }

    act( ZM_ZAWSZE, "Zachodzisz $# od ty`lu...", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zachodzi $# od ty`lu...", ch, NULL, victim, TO_NOTVICT );

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[ gsn_circle ].beats );

    if ( IS_NPC( ch )
	|| number_percent( ) <= ch->pcdata->learned[ gsn_circle ] * 2 / 3 )
    {
	uzyj( ch, gsn_circle, "zadawaniu cios`ow w plecy" );
	set_fighting_main( victim, ch );
	multi_hit( ch, victim, gsn_circle );
    }
    else
	act( ZM_ZAWSZE, "Nie uda`lo ci si`e zaj`s`c $M od ty`lu.", ch, NULL, victim, TO_CHAR );

    return;
}


KOMENDA( do_change )
{
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    if ( !moze_uzyc( ch, gsn_change ) )
    {
	send_to_char( "Przecie`z nie potrafisz tego zrobi`c.\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	return;

    if ( !ch->fighting )
    {
	send_to_char( "Musisz walczy`c, by to zrobi`c.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Z kim chcesz walczy`c?\n\r", ch );
	return;
    }
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu twej ofiary.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	ASTC( "No co ty? Walczy`c sam$a ze sob`a? W takiej chwili?", ch );
	return;
    }

    if ( victim == FIGHTING( ch ) )
    {
	send_to_char( "Nie dokonujesz `zadnej zmiany.\n\r", ch );
	return;
    }

    if ( !is_fighting( victim, ch ) )
    {
	send_to_char( "Musisz wybra`c kogo`s, kto walczy z tob`a.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
      return;

    if ( IS_NPC( ch )
      || number_percent( ) <= ch->pcdata->learned[ gsn_change ] * 3 / 4 )
    {
	set_fighting_main( ch, victim );
	act( ZM_ZAWSZE, "Teraz walczysz z$E $$.", ch, NULL, victim, TO_CHAR );
	uzyj( ch, gsn_change, "dobieraniu sobie przeciwnik`ow" );
    }
    else
	act( ZM_ZAWSZE, "Nie wysz`lo ci.", ch, NULL, victim, TO_CHAR );

    return;
}


KOMENDA( do_flee )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    int attempt;
    int chance;
    EXIT_DATA *pexit;
    int door;
    int przed;

    if ( !( victim = FIGHTING( ch ) ) )
    {
	if ( ch->position == POS_FIGHTING )
	    ch->position = POS_STANDING;
	send_to_char( "Z nikim nie walczysz.\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_berserk ) )
    {
	act( ZM_ZAWSZE, "Nie mo`zesz ucieka`c, wpad`l$o`s w sza`l bojowy!",
		ch, NULL, NULL, TO_CHAR );
	return;
    }

    if ( ch == victim )
    {
	STC( "Przestajesz si`e molestowa`c.\n\r", ch );
	stop_fighting_char( ch, ch );
	return;
    }

    if ( is_affected( ch, gsn_zaglada ) )
	return;

    if ( IS_AFFECTED( ch, AFF_HOLD ) )
    {
	if ( is_affected( ch, gsn_snare )
	  && number_percent( ) > str_app[ get_curr_str( ch ) ].uwalnianie
	  && !IS_PASSING( ch ) )
	{
	    STC( "Nie uda`lo ci si`e rozerwa`c side`l, spr`obuj jeszcze raz!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje ucieka`c, ale zosta`l$a schwytan$y w sid`la!",
		ch, NULL, NULL, TO_ROOM );
	    if ( !IS_NPC( ch ) )
	    {
		STC( "Tracisz 10 punkt`ow do`swiadczenia.\n\r", ch );
		gain_exp( ch, -10 );
	    }
	    return;
	}
	else if ( !is_affected( ch, gsn_snare ) )
	{
	    STC( "Z tych side`l si`e nie uwolnisz!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje ucieka`c, ale zosta`l$a schwytan$y w sid`la!",
		ch, NULL, NULL, TO_ROOM );
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

    was_in = ch->in_room;

    for ( door = 0; door < 10; door++ )
    {
	if ( ( pexit = was_in->exit[ door ] )
	    && pexit->to_room
	    && ( !IS_SET( pexit->exit_info, EX_CLOSED )
	      || ( IS_PASSING( ch )
		&& !IS_SET( pexit->exit_info, EX_PASSPROOF ) ) )
	    && ( !IS_NPC( ch )
	      || ( !IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
		&& ( !IS_SET( ch->act, ACT_STAY_AREA )
		  || pexit->to_room->area == ch->in_room->area ) ) ) )
	    break;
    }

    if ( door == 10 )
    {
	STC( "Nie masz dok`ad ucieka`c!\n\r", ch );
	return;
    }

    chance = 6;
    if ( !IS_NPC( ch ) )
	if ( number_percent( ) <= ch->pcdata->learned[ gsn_fast_flee ] )
	{
	    chance = 20;
	    uzyj( ch, gsn_fast_flee, "szybkim wianiu" );
	}

    przed = ch->exp;

    for ( attempt = 0; attempt < chance; attempt++ )
    {
	door = number_door( );
	if ( ( pexit = was_in->exit[ door ] ) == 0
	    || !pexit->to_room
	    || ( IS_SET( pexit->exit_info, EX_CLOSED )
	      && ( !IS_PASSING( ch )
		|| IS_SET( pexit->exit_info, EX_PASSPROOF ) ) )
	    || ( IS_NPC( ch )
	      && ( IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
		|| ( IS_SET( ch->act, ACT_STAY_AREA )
		  && pexit->to_room->area != ch->in_room->area ) ) ) )
	    continue;

	move_char( ch, door, FALSE );

	if ( ch->deleted )
	    return;

	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( ZM_WZROK | ZM_WID_CHAR, "$n uciek`l$a!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;

	/* Jesli liczba doswiadczenia ch zmniejszyla sie, albo uciekl
	   rekurencyjnie, albo zginal. Obie sytuacje moga spowodowac
	   greet_progi i pass_progi. */
	if ( !IS_NPC( ch ) && ch->exp >= przed )
	{
	    act( ZM_ZAWSZE, "Uciek`l$o`s z pola walki! Tracisz 25 punkt`ow do`swiadczenia.",
			ch, NULL, NULL, TO_CHAR );
	    gain_exp( ch, -25 );
	}
	/* stop_fighting( ch, TRUE ); niepotrzebne po zmianach */
	return;
    }

    send_to_char( "Nie uda`lo ci si`e! Tracisz 10 punkt`ow do`swiadczenia.\n\r", ch );
    gain_exp( ch, -10 );

    return;
}


KOMENDA( do_berserk )
{
    AFFECT_DATA af;

    /* Don't allow charmed mobs to do this, check player's level */
    /* teraz to sprawdzanie zalatwia funkcja moze_uzyc( ) */
    if ( !moze_uzyc( ch, gsn_berserk ) )
    {
	send_to_char( "Nie jeste`s wystarczaj`aco do`swiadczonym wojownikiem, aby wpa`s`c w bojowy sza`l.\n\r",
		     ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char( "Z nikim nie walczysz.\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_berserk ) )
    {
	STC( "W wi`ekszy sza`l ju`z nie wpadniesz.\n\r", ch );
	return;
    }

    send_to_char( "Krew twego wroga rozpryskuje si`e wok`o`l!\n\r", ch );
    send_to_char( "Smak krwi doprowadza ci`e do szale`nstwa!\n\r", ch );

    if ( IS_NPC( ch )
	|| number_percent( ) <= ch->pcdata->learned[ gsn_berserk ] )
    {
	af.type       = gsn_berserk;
	af.duration   = -1;
	af.location   = APPLY_HITROLL;
	af.modifier   = ch->level / 2;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_SZAL_BOJOWY );
	af.level      = ch->level;
	af.caster     = ch;
	affect_to_char( ch, &af );

	af.location   = APPLY_DAMROLL;
	af.modifier   = ch->level / 2;
	CLEAR_BITS( af.bitvector );
	affect_to_char( ch, &af );

	ASTC( "Wpad`l$o`s w bojowy sza`l!", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wpad`l$a w sza`l bojowy!", ch, NULL, NULL, TO_ROOM );
	uzyj( ch, gsn_berserk, "bojowym op`etaniu" );

	return;
    }
    send_to_char( "Op`edzasz si`e od szale`nstwa.\n\r", ch );

    return;
}


KOMENDA( do_rescue )
{
    CHAR_DATA *victim;
    CHAR_DATA *fch;
    CHAR_DATA *rch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        count;

    /* Don't allow charmed mobs to do this, check player's level */
    if ( !moze_uzyc( ch, gsn_rescue ) )
    {
	send_to_char( "Zostaw to innym.\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_berserk ) )
    {
	ASTC( "Nikogo nie uratujesz, wpad`l$o`s w bojowy sza`l!", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo odratowa`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Mo`ze lepiej uciec?\n\r", ch );
	return;
    }

    /* Lam 10.6.2004: i tak pozniej jest test jednej grupy, a w praktyce
       szybciej przeciwnik przerzuci sie na gracza, niz ten gracz sprobuje
       uratowac moba z grupy (czyli efekt ten sam)
    if ( !IS_NPC( ch ) && IS_NPC( victim ) )
    {
	send_to_char( "Nie potrzebuj`e twojej pomocy!\n\r", ch );
	return;
    } */

    if ( is_fighting( ch, victim ) )
    {
	send_to_char( "Teraz ju`z za p`o`xno. Nie trzeba by`lo wszczyna`c b`ojki.\n\r", ch );
	return;
    }

    if ( !victim->fighting )
    {
	send_to_char( "Ta osoba z nikim nie walczy.\n\r", ch );
	return;
    }

    if ( !is_same_group( ch, victim ) )
    {
	send_to_char( "Czemu chcesz pomaga`c komu`s spoza swej grupy?\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    WAIT_STATE( ch, skill_table[ gsn_rescue ].beats );

    count = 0;
    rch = NULL;
    for ( fch = victim->in_room->people; fch; fch = fch->next_in_room )
    {
	if ( !IS_NPC( fch )
	  || fch->deleted )
	    continue;

	if ( FIGHTING( fch ) == victim )
	{
	    if ( number_range( 0, count ) == 0 )
		rch = fch;
	    count++;
	}
    }
    fch = rch; /* dorobilem losowanie, przepisze wartosc, reszty funkcji nie musze zmieniac */

    if ( !fch
      || ( !IS_NPC( ch )
	&& number_percent( ) > ch->pcdata->learned[ gsn_rescue ] ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    act( ZM_ZAWSZE, "Odratowujesz $#!", ch, NULL, victim, TO_CHAR );
    act( ZM_PRZYT, "$n odratowuje ci`e!", ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_VICT, "$n odratowuje $#!", ch, NULL, victim, TO_NOTVICT );

    uzyj( ch, gsn_rescue, "ratowaniu innych postaci" );

    set_fighting_main( fch, ch );
    if ( !is_fighting( ch, fch ) )
	set_fighting( ch, fch );

    return;
}


KOMENDA( do_kick )
{
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    /* Don't allow charmed mobs to do this, check player's level */
    if ( !moze_uzyc( ch, gsn_kick ) )
    {
	send_to_char( "Nie potrafisz tego zrobi`c.\n\r", ch );
	return;
    }

    /* Lam 10.8.2002 */
    if ( !IS_SET( race_table[ ch->race ].parts, P_NOGI ) )
    {
	STC( "Do kopania potrzebne ci nogi.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char( "Z nikim nie walczysz.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    victim = FIGHTING( ch );

    if ( arg[ 0 ] != '\0' )
    {
	if ( is_affected( ch, gsn_berserk )
	  && get_char_room( ch, arg ) != victim )
	{
	    ASTC( "Nie mo`zesz! Zawalczy`l$o`s si`e na `smier`c!", ch );
	    return;
	}

	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	    return;
	}
    }

    WAIT_STATE( ch, skill_table[ gsn_kick ].beats );
    if ( IS_NPC( ch ) || number_percent( ) <= ch->pcdata->learned[ gsn_kick ] )
    {
	int kopniak;
	kopniak = number_range( ch->level / 2, ch->level );
	if ( !IS_NPC( ch ) && number_percent( ) <= ch->pcdata->learned[ gsn_enhanced_kick ] / 5 * 4 )
	{
	    kopniak += number_range( ch->level / 4, ch->level );
	    uzyj( ch, gsn_enhanced_kick, "wymierzaniu pot`e`znych kopniak`ow" );
	}
	uzyj( ch, gsn_kick, "wymierzaniu kopniak`ow" );
	damage( ch, victim, kopniak, gsn_kick,
	       WEAR_NONE, FALSE );
    }
    else
	damage( ch, victim, 0, gsn_kick, WEAR_NONE, FALSE );

    return;
}


KOMENDA( do_disarm )
{
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];
    int        percent;

    /* Don't allow charmed mobiles to do this, check player's level */
    if ( !moze_uzyc( ch, gsn_disarm ) )
    {
	send_to_char( "Nie umiesz rozbraja`c przeciwnik`ow.\n\r", ch );
	return;
    }

    if ( !get_eq_char( ch, WEAR_WIELD )
      && !get_eq_char( ch, WEAR_WIELD_2 ) )
    {
	send_to_char( "`Zeby rozbraja`c, musisz u`zy`c broni.\n\r", ch );
	return;
    }


    if ( !ch->fighting )
    {
	send_to_char( "Z nikim nie walczysz.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    victim = FIGHTING( ch );

    if ( arg[ 0 ] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	    return;
	}

    if ( ch == victim )
    {
	STC( "Je`sli chcesz si`e rozbroi`c, po prostu zdejmij bro`n.\n\r", ch );
	return;
    }

    if ( !is_fighting( victim, ch ) && !is_fighting( ch, victim ) )
    {
	act( ZM_ZAWSZE, "$Z nie walczy z tob`a!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !get_eq_char( victim, WEAR_WIELD )
      && !get_eq_char( victim, WEAR_WIELD_2 ) )
    {
	send_to_char( "Tw`oj przeciwnik nie u`zywa broni.\n\r", ch );
	return;
    }

    if ( ( ROZMIAR( ch ) - ROZMIAR( victim ) ) < -2 )
    {
	act( ZM_ZAWSZE, "On$A jest dla ciebie za du`z$Y. Nie masz szans wytr`aci`c $U broni.", ch, NULL, victim, TO_CHAR );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_disarm ].beats );
    percent = number_percent( ) + victim->level - ch->level;

    if ( !get_eq_char( ch, WEAR_WIELD ) )
	percent *= 2;           /* 1/2 as likely with only 2nd weapon */

    if ( IS_NPC( ch ) || percent < ch->pcdata->learned[ gsn_disarm ] * 2 / 3 )
    {
	if ( disarm( ch, victim ) )
	    uzyj( ch, gsn_disarm, "rozbrajaniu" );
	else
	    send_to_char( "Nie wysz`lo ci.\n\r", ch );
    }
    else
	send_to_char( "Nie wysz`lo ci.\n\r", ch );

    return;
}


KOMENDA( do_sla )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "zar`znij" ) )
	return;

    send_to_char( "Je`sli chcesz kogo`s zar`zn`a`c, wpisz ZAR`ZNIJ w ca`lo`sci.\n\r", ch );

    return;
}


KOMENDA( do_slay )
{
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    char arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *room = ch->in_room;
    bool placz = TRUE;

    rch = get_char( ch );

    if ( !authorized( rch, "zar`znij" ) )
	return;

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Kogo zar`zn`a`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widz`e tu takiej postaci.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Samob`ojstwo to grzech `smiertelnik`ow.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && victim->level >= ch->level )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return;
    }

    act( ZM_ZAWSZE, "Zarzynasz $V z zimn`a krwi`a!", ch, NULL, victim, TO_CHAR );
    act( ZM_PRZYT, "$n zarzyna ci`e z zimn`a krwi`a!", ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_VICT, "$n zarzyna $# z zimn`a krwi`a!", ch, NULL, victim, TO_NOTVICT );
    if ( !IS_NPC( victim ) )
	wiznet( "{M$N{x zosta`l$A zar`zni`et$Y przez nie`smiertelnego.", victim, NULL, NULL, WIZ_DEATH, 0, 0 );

    if ( mprog_death_trigger( victim, ch ) )
	placz = FALSE;

    if ( ch->deleted || victim->deleted
      || ch->in_room != room || victim->in_room != room )
	return;

    if ( placz )
	death_cry( victim, 0, victim->position );

    raw_kill( ch, victim, FALSE, FALSE, IS_NPC( ch ) ? ZGON_ZWYKLY : ZGON_GRACZ );

    return;
}


/*
 * Qwert
 */
KOMENDA( do_fury )
{
    AFFECT_DATA af;

    if ( !moze_uzyc( ch, gsn_fury ) )
    {
	send_to_char( "Nie jeste`s w stanie si`e w`sciec.\n\r",
		     ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char( "Z nikim nie walczysz.\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_fury ) )
    {
	send_to_char( "Ju`z wi`ecej si`e w`sciec nie mo`zesz.\n\r", ch );
	return;
    }

    send_to_char( "{RKrwi!{x\n\r", ch );
    send_to_char( "Widok twojego przeciwnika doprowadza ci`e do w`sciek`lo`sci!\n\r", ch );

    if ( IS_NPC( ch )
	|| number_percent( ) <= ch->pcdata->learned[ gsn_fury ] )
    {
	af.type      = gsn_fury;
	af.duration  = 4;
	af.location  = APPLY_HITROLL;
	af.modifier  = 10 + ch->level / 3;
	CLEAR_BITS( af.bitvector );
	af.level = ch->level;
	af.caster = ch;
	affect_to_char( ch, &af );

	af.location  = APPLY_DAMROLL;
	af.modifier  = 15 + ch->level / 3;
	affect_to_char( ch, &af );

	af.location  = APPLY_DEX;
	af.modifier  = -1 - ( ch->level < 60 );
	affect_to_char( ch, &af );

	act( ZM_ZAWSZE, "Jeste`s w`sciek`l$y!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n si`e w`sciek`l$a!", ch, NULL, NULL, TO_ROOM );
	uzyj( ch, gsn_fury, "w`sciek`lo`sci" );

	return;
    }
    send_to_char( "Po chwili, powoli si`e uspokajasz.\n\r", ch );

    return;
}


bool licensed( CHAR_DATA *ch )
{
    /* teraz bicie graczy jest dozwolone */
    return TRUE;
/*  oryginalna wersja:
    OBJ_DATA *obj;

    if ( ch->race == zr_wampir )
	return TRUE;

    if ( !IS_SET( ch->act, PLR_REGISTER ) )
	return FALSE;
*/
    /*
     * If already fighting then we just jump out.
     */
/*
    if ( ch->fighting )
	return TRUE;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == OBJ_VNUM_LICENSE )
	    return TRUE;
    }

    return FALSE;
*/
}


bool registered( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /* teraz mamy mordowanie */
    return TRUE;
/*  oryginalna wersja:
    if ( IS_SET( victim->act, PLR_REGISTER ) )
	return TRUE;

    if ( ch->race == zr_wampir )
	return TRUE;

    return FALSE;
*/
}


#define PRZEDMIOT_OFENSYWNY 1
#define PRZEDMIOT_OCHRONNY 2
#define PRZEDMIOT_BEZ_SENSU 3
/* Lam 3.5.2003 */
int ocen_czar( int v )
{
    if ( skill_table[ v ].target == TAR_CHAR_OFFENSIVE )
	return PRZEDMIOT_OFENSYWNY;

    if ( ( skill_table[ v ].target == TAR_CHAR_DEFENSIVE
	     && skill_table[ v ].spell_fun != spell_polymorph_other )
	   || ( skill_table[ v ].target == TAR_CHAR_SELF
	     && skill_table[ v ].spell_fun != spell_teleport
	     && skill_table[ v ].spell_fun != spell_word_of_recall ) )
	return PRZEDMIOT_OCHRONNY;

    return PRZEDMIOT_BEZ_SENSU;
}


int ocen_przedmiot( int v1, int v2, int v3 )
{
    int ocena = 0, ocenat = 0;

    if ( !v1 )
	return PRZEDMIOT_BEZ_SENSU;

    ocena = ocen_czar( v1 );

    if ( v2 )
    {
	ocenat = ocen_czar( v2 );
	if ( ocena != ocenat )
	    return PRZEDMIOT_BEZ_SENSU;
    }

    if ( v3 )
    {
	ocenat = ocen_czar( v3 );
	if ( ocena != ocenat )
	    return PRZEDMIOT_BEZ_SENSU;
    }

    return ocena;
}


/* Mobs using magical items by Spitfire from merc mailing list
 *
 * Modified to give every magical item an equal chance of being used plus
 * eating pills -Kahn
 *
 * Lam 3.5.2003: zmienione, by ocenialo przydatnosc przedmiotu i uzywalo go
 * zgodnie z przeznaczeniem, lub nie uzywalo. Przy okazji dopisalem dokladne
 * wybieranie przedmiotow (recytuj 2.zwoj) */
void use_magical_item( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *cobj     = NULL;
    int       number   = 0;
    char      buf[ MIL ];
    char      oname[ MIL ];
    int       ocena;

    if ( ch->deleted )
    {
	bug( "ch deleted", 0 );
	return;
    }

    if ( ch->position < POS_FIGHTING )
	return;

    if ( is_affected( ch, gsn_zaglada ) )
	return;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( ( obj->item_type == ITEM_SCROLL
	    || obj->item_type == ITEM_WAND
	    || obj->item_type == ITEM_STAFF
	    || obj->item_type == ITEM_PILL
	    /* Alandar 04.10.2004 - brakowalo mikstur: */
	    || obj->item_type == ITEM_POTION )
	  && number_range( 0, number ) == 0 )
	{
	    cobj = obj;
	    number++;
	}
    }

    if ( !cobj )
	return;

    strcpy( oname, name_expand_obj( ch, cobj ) );

    switch ( cobj->item_type )
    {
	case ITEM_SCROLL:
	    ocena = ocen_przedmiot( cobj->value[ 1 ].v, cobj->value[ 2 ].v,
			cobj->value[ 3 ].v );
	    /* niniejsza funkcja jest wywolywana w czasie walki, wiec na pewno
	       jest ustawione ch->fighting i nie podanie parametru spowoduje
	       wziecie przeciwnika na cel */
	    if ( ocena == PRZEDMIOT_OCHRONNY )
	    {
		sprintf( buf, "%s si`e", oname );
		do_recite( ch, buf );
	    }
	    if ( ocena == PRZEDMIOT_OFENSYWNY )
	    {
		sprintf( buf, "%s", oname );
		do_recite( ch, buf );
	    }
	    break;

	case ITEM_WAND:
	    if ( cobj->wear_loc == WEAR_HOLD )
	    {
		ocena = ocen_przedmiot( cobj->value[ 3 ].v, 0, 0 );
		if ( ocena == PRZEDMIOT_OCHRONNY )
		    do_zap( ch, "si`e" );
		if ( ocena == PRZEDMIOT_OFENSYWNY )
		    do_zap( ch, "" );
	    }
	    break;

	case ITEM_STAFF:
	    ocena = ocen_przedmiot( cobj->value[ 3 ].v, 0, 0 );
	    if ( cobj->wear_loc == WEAR_HOLD && ocena == PRZEDMIOT_OFENSYWNY )
		do_brandish( ch, "" );
	    break;

	case ITEM_POTION:
	    ocena = ocen_przedmiot( cobj->value[ 1 ].v, cobj->value[ 2 ].v,
			cobj->value[ 3 ].v );
	    sprintf( buf, "%s", oname ); /* Lam 13.4.2000, bylo "potion" */
	    if ( ocena == PRZEDMIOT_OCHRONNY )
		do_drink( ch, buf );
	    break;

	case ITEM_PILL:
	    ocena = ocen_przedmiot( cobj->value[ 1 ].v, cobj->value[ 2 ].v,
			cobj->value[ 3 ].v );
	    sprintf( buf, "%s", oname );
	    if ( ocena == PRZEDMIOT_OCHRONNY )
		do_eat( ch, buf );
	    break;
    }

    return;
}


bool czy_w_grze( CHAR_DATA *ch, const char *imie )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *gracz;

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	  && ( gracz = d->character )
	  && !gracz->deleted
	  && !IS_NPC( gracz )
	  && can_see_who( ch, gracz )
	  && is_name2( imie, gracz->name ) )
	{
	    return TRUE;
	}
    }

    return FALSE;
}


/*
 * Lam 25.12.2003
 * Qwert 27.02.04 zabojca - zmiana z 3 powodow:
   1. pojedynek dotyczy walk miedzy graczami
   2. nie musze robic nowego polecenia, a gracz MUSI miec mozliwosc usuniecia
      niechcianego imienia
   3. jesli ch nie chce sie mscic, to niech usuwa - lista imion bedzie krotsza
*/
KOMENDA( do_pojedynek )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *gracz;
    char *imie, *list;
    char imie1[ MAX_INPUT_LENGTH ];
    char tmp[ MAX_INPUT_LENGTH ];
    char pojedynek[ MAX_STRING_LENGTH ];
    char nienawisc[ MAX_STRING_LENGTH ];
    char newpoj[ MAX_STRING_LENGTH * 2 ];
    bool minus = FALSE, jest = FALSE;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Moby nie wypowiadaj`a pojedynk`ow.\n\r", ch );
	return;
    }

    if ( !*argument )
    {
	if ( *ch->pcdata->pojedynek )
	{
	    strcpy( newpoj, "Zezwalasz na atakowanie si`e nast`epuj`acym postaciom:\n\r" );
	    strcpy( pojedynek, ch->pcdata->pojedynek );
	    list = pojedynek;
	    for ( ; ; )
	    {
		list = one_argument2( list, tmp );
		if ( !*tmp )
		    break;
		strcat( newpoj, tmp );
		if ( czy_w_grze( ch, tmp ) )
		    strcat( newpoj, " (w grze)\n\r" );
		else
		    strcat( newpoj, "\n\r" );
	    }
	}
	else
	    strcpy( newpoj, "Nikomu nie zezwalasz si`e atakowa`c bezkarnie.\n\r" );

	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	      && ( gracz = d->character )
	      && !gracz->deleted
	      && !IS_NPC( gracz )
	      && can_see_who( ch, gracz )
	      && is_name2( ch->name, gracz->pcdata->pojedynek ) )
	    {
		if ( !jest )
		{
		    strcat( newpoj, "Nast`epuj`ace postacie w grze pozwalaj`a ci si`e atakowa`c:\n\r" );
		    jest = TRUE;
		}
		strcat( newpoj, gracz->short_descr );
		strcat( newpoj, "\n\r" );
	    }
	}

	if ( !jest )
	    strcat( newpoj, "`Zadna z graj`acych obecnie postaci nie pozwala ci si`e atakowa`c.\n\r" );

	if ( *ch->pcdata->nienawisc )
	{
	    strcat( newpoj, "Nast`epuj`acych graczy mo`zesz raz bezkarnie zabi`c:\n\r" );
	    strcpy( nienawisc, ch->pcdata->nienawisc );
	    list = nienawisc;
	    for ( ; ; )
	    {
		list = one_argument2( list, tmp );
		if ( !*tmp )
		    break;
		strcat( newpoj, tmp );
		if ( czy_w_grze( ch, tmp ) )
		    strcat( newpoj, " (w grze)\n\r" );
		else
		    strcat( newpoj, "\n\r" );
	    }
	 /* send_to_char( newpoj, ch ); */
	}
	else
	    strcat( newpoj, "Nikogo nie masz prawa bezkarnie zabi`c.\n\r" );

	send_to_char( newpoj, ch );
	return;
    }

    one_argument( argument, imie1 );
    imie = &imie1[ 0 ];
    if ( *imie == '-' )
    {
	imie++;
	minus = TRUE;
    }

    if ( !*imie )
    {
	if ( minus )
	    STC( "Podaj imi`e gracza, z kt`orym nie chcesz ju`z walczy`c.\n\r", ch );
	else
	    STC( "Podaj imi`e gracza, z kt`orym chcesz si`e pojedynkowa`c.\n\r", ch );
	return;
    }

    if ( minus )
    {
	bool usun = FALSE;

	if ( WP( ch ) )
	{
	    /* brak jednego slowa powoduje, ze zabojca tez pasuje */
	    STC( "Rezygnowa`c mo`zna tylko poza postaci`a.\n\r", ch );
	    return;
	}

	/* Jesli ma kogos w pojedynku, to nie moze go miec w nienawisci, bo mu
	   tego nie doda. Jesli walczyl z kims, a ma go w nienawisci, to chyba
	   nie bedzie wypowiadac pojedynku komus, kogo i tak moze bezkarnie
	   zabic. Jesli jednak doda, to smierc tego kogos spowoduje automatyczne
	   usuniecie go z listy. */

	if ( !is_name2( imie, ch->pcdata->pojedynek ) )
	{
	    usun = TRUE;
	    if ( !is_name2( imie, ch->pcdata->nienawisc ) )
	    {
		send_to_char( "Takiego gracza nie by`lo na li`scie.\n\r", ch );
		return;
	    }
	}

	if ( !usun && ( gracz = get_char_world2( ch, imie ) ) )
	    act_pp( ZM_ZAWSZE, "{R$n nie chce ju`z by`c przez ciebie atakowan$y.{x", ch, NULL, gracz, TO_VICT );

	if ( !usun )
	{
	    strcpy( pojedynek, ch->pcdata->pojedynek );
	    strcpy( newpoj, "" );
	    list = &pojedynek[ 0 ];
	    for ( ; ; )
	    {
		list = one_argument2( list, tmp );
		if ( !tmp[ 0 ] )
		    break;
		if ( str_cmp( tmp, imie ) ) /* nie imie, wiec mozna przepisac */
		{
		    strcat( newpoj, tmp );
		    strcat( newpoj, " " );
		}
	    }
	    free_string( ch->pcdata->pojedynek );
	    ch->pcdata->pojedynek = str_dup( newpoj );
	}
	else /* Qwert: skopiowane */
	{
	    strcpy( nienawisc, ch->pcdata->nienawisc );
	    strcpy( newpoj, "" );
	    list = &nienawisc[ 0 ];
	    for ( ; ; )
	    {
		list = one_argument2( list, tmp );
		if ( !tmp[ 0 ] )
		    break;
		if ( str_cmp( tmp, imie ) ) /* nie imie, wiec mozna przepisac */
		{
		    strcat( newpoj, tmp );
		    strcat( newpoj, " " );
		}
	    }
	    free_string( ch->pcdata->nienawisc );
	    ch->pcdata->nienawisc = str_dup( newpoj );
	}

    }
    else
    {
	if ( check_parse_name( imie, FALSE ) ) /* ograniczenie spacji, tyldy itd. */
	{
	    send_to_char( "Nie mo`zesz pojedynkowa`c si`e z kim`s takim.\n\r", ch );
	    return;
	}

	if ( is_name2( imie, ch->pcdata->pojedynek ) )
	{
	    send_to_char( "Ta posta`c figuruje ju`z na li`scie.\n\r", ch );
	    return;
	}

	if ( strlen( ch->pcdata->pojedynek ) + strlen( imie ) > 1000 )
	{
	    send_to_char( "Pojedynkujesz si`e ze zbyt du`z`a ilo`sci`a postaci.\n\r", ch );
	    return;
	}

	for ( d = descriptor_list; d; d = d->next )
	    if ( d->connected == CON_PLAYING
	      && ( gracz = d->character )
	      && !gracz->deleted
	      && !IS_NPC( gracz )
	      && can_see_who( ch, gracz )
	      && is_name2( imie, gracz->name ) )
		break;

	if ( !d )
	{
	    STC( "Posta`c musi by`c graczem i znajdowa`c si`e w `swiecie.\n\r"
		 "Pami`etaj, `ze wypowiadaj`ac pojedynek musisz poda`c pe`lne imi`e postaci.\n\r", ch );
	    return;
	}

	if ( ch == gracz )
	{
	    STC( "Sobie nie musisz wypowiada`c pojedynk`ow.\n\r", ch );
	    return;
	}

	/* tu chodzi o "zwielkoliterowanie", o ile sie da */
	if ( !str_cmp( gracz->name, gracz->short_descr ) )
	{
	    strcpy( imie1, gracz->short_descr );
	    imie = imie1;
	}

	act_pp( ZM_ZAWSZE, "$n wypowiada ci pojedynek i pozwala si`e atakowa`c.", ch, NULL, gracz, TO_VICT );
	strcpy( pojedynek, ch->pcdata->pojedynek );
	strcat( pojedynek, imie );
	strcat( pojedynek, " " );
	free_string( ch->pcdata->pojedynek );
	ch->pcdata->pojedynek = str_dup( pojedynek );
    }
    do_pojedynek( ch, "" ); /* pokaze liste */

    return;
}


/*
 * Qwert 27.02.04: funkcja uzywana w dwoch miejscach
 */
void dodaj_zabojce( CHAR_DATA *ofiara, CHAR_DATA *zabojca )
{
    char tablica_nienawisci[ MAX_STRING_LENGTH ];
    char imie1[ MAX_INPUT_LENGTH ];
    char *imie;

    /* na wszelki wypadek, gdyby cos sie zmienilo w do_steal */
    if ( IS_NPC( zabojca ) || IS_NPC( ofiara ) )
       return;

    sprintf( log_buf, "nieregulaminowy atak: %s -> %s (daje prawo zemsty, atak: %s)",
	zabojca->short_descr, ofiara->short_descr, ostatnie_morduj );
    log_string( log_buf );
    sprintf( log_buf, "nieregulaminowy atak: {M%s{x -> {m%s{x.",
	zabojca->short_descr, ofiara->short_descr );
    wiznet( log_buf, zabojca, NULL, NULL, WIZ_MURDER, 0, 0 );

    if ( is_name2( zabojca->name, ofiara->pcdata->nienawisc ) )
       return;

    /* ponizsze, dzieki "pojedynkowi", malo prawdopodobne, musi jednak byc */
    if ( strlen( ofiara->pcdata->nienawisc ) + strlen( zabojca->name ) > 1000 )
    {
	char lista[ MAX_STRING_LENGTH ];
	char tablica_imion[ MAX_INPUT_LENGTH ];

	imie = ofiara->pcdata->nienawisc;

	while ( strlen( imie ) + strlen( zabojca->name ) > 980 )
	    imie = one_argument2( imie, tablica_imion );

	strcpy( lista, imie );
	free_string( ofiara->pcdata->nienawisc );
	ofiara->pcdata->nienawisc = str_dup( lista );
    }

    strcpy( imie1, zabojca->name );
    imie = &imie1[ 0 ];

    if ( !str_cmp( zabojca->name, zabojca->short_descr ) )
    {
	strcpy( imie1, zabojca->short_descr );
	imie = imie1;
    }

    strcpy( tablica_nienawisci, ofiara->pcdata->nienawisc );
    strcat( tablica_nienawisci, imie );
    strcat( tablica_nienawisci, " " );
    free_string( ofiara->pcdata->nienawisc );
    ofiara->pcdata->nienawisc = str_dup( tablica_nienawisci );

    return;
}


/*
 * Qwert 27.02.04: wciaganie zabojcow spoza postaci. Uzywane przed
 * get_char_room w kazdym poleceniu, ktore gwarantuje nam zaczecie walki po
 * uzyciu tej funkcji. Nalezy dopracowac tresc komunikatow.
 */
void pp_get( CHAR_DATA *ch, char *victim_name )
{
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    int licz;

    if ( IS_NPC( ch ) )
	return;

    if ( ch->fighting || ch->position == POS_FIGHTING )
	return;

    if ( victim_name[ 0 ] == '\0' )
	return;

    /* Polecenia do ataku i sciagniecia nie moga byc takie same, jak polecenia
       normalne, inaczej jakis zabojca nazwie sie np. POTW (potwor) czy BES
       (bestia), jakis gracz moze wejsc postacia na niskim poziomie, wpisac
       "zabij potw" i sciagnac w ten sposob sobie zabojce na glowe. Poza tym,
       wykrzyknik po imieniu dodaje nieco pasji ;) */

    /* jesli ostatnie znaki imienia to "!"... */
    if ( victim_name[ strlen( victim_name ) - 1 ] != '!' )
	return;

    /* ... wycinam je, zeby argument byl prawidlowy */
    for ( licz = 0 ; victim_name[ licz ] != '!' ; licz++ );
	victim_name[ licz ] = '\0';

    if ( !( victim = get_char_world_pc( ch, victim_name ) ) )
	return;

    if ( IS_NPC( victim ) )
	return;

    if ( WP( victim ) || !IS_SET( victim->act, PLR_ZABOJCA ) )
	return;

    if ( !( is_name2( victim->name, ch->pcdata->nienawisc )
	 || is_name2( ch->name, victim->pcdata->pojedynek )
	 || IS_SET( victim->act, PLR_KILLER )
	 || IS_SET( victim->act, PLR_THIEF )
	 || !czy_morderstwo( ch, victim ) ) )
	return; /* nie mordercy */

    act( ZM_ZAWSZE, "B`lagasz bog`ow, aby pomogli ci zabi`c $#!", ch, NULL, victim, TO_CHAR );

    if ( !can_enter_room( victim, ch->in_room, FALSE )
      || ch->in_room->area == area_czysciec )
    {
	send_to_char( "Bogowie nie reaguj`a na twe b`lagania.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_KILLER ) || IS_SET( victim->act, PLR_THIEF ) )
    {
	CHAR_DATA *pomocnik;
	/* zabezpieczenie przed pomoca ze strony kata */
	for ( pomocnik = ch->in_room->people; pomocnik; pomocnik = pomocnik->next_in_room )
	{
	    if ( ch != pomocnik && IS_NPC( pomocnik )
	      && ( pomocnik->spec_fun == spec_lookup( "spec_executioner" )
		|| pomocnik->spec_fun == spec_lookup( "spec_guard" ) ) )
	    {
		send_to_char( "Bogowie nie reaguj`a na twe b`lagania.\n\r", ch );
		return;
	    }
	}
    }

    if ( ch->in_room->trap
         /* inaczej is_safe zatrzyma walke: */
      || IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA ) )
    {
	send_to_char( "Bogowie nie reaguj`a na twe b`lagania.\n\r", ch );
	return;
    }

    /* duzo komunikatow, ale wszystko musi wiedziec */
    act_pp( ZM_ZAWSZE, "{RTw`oj niespokojny duch nie pozwala ci tak tkwi`c bezczynnie!{x", victim, NULL, NULL, TO_CHAR );
    /* dalbym do_wciel, ale tam jest do_look, a chce tego uniknac */
    act_pp( ZM_ZAWSZE, "Wcielasz si`e.", victim, NULL, NULL, TO_CHAR );
    act_pp( ZM_ZAWSZE, "Czujesz, jak nieznana si`la unosi ci`e w kierunku $3!", ch, NULL, victim, TO_VICT );
    for ( rch = victim->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch->deleted || rch == victim )
	    continue;

	/* sprawdzanie boskiej niewidki zbedne ze ZM_WID_CHAR */
	if ( !IS_NPC( rch )
	  && CZY_OPCJA( rch, OPC_WCHODZENIE ) )
	    act_pp( ZM_WZROK | ZM_WID_CHAR, "$n zostaje zmuszon$y do wcielenia.", victim, NULL, rch, TO_VICT );
    }

    sprintf( log_buf, "pp_get: sciagajacy %s do %d, sciagany %s z %d",
	ch->short_descr, ch->in_room->vnum,
	victim->short_descr, victim->pcdata->ic_in_room->vnum );
    log_string( log_buf );
    sprintf( log_buf, "`Lowca {M%s{x `sci`aga {m%s{x z {y%d{x do {Y%d{x.",
	ch->short_descr, victim->biernik,
	victim->pcdata->ic_in_room->vnum, ch->in_room->vnum );
    wiznet( log_buf, ch, NULL, NULL, WIZ_MURDER, 0, 0 );

    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( ZM_WZROK | ZM_WID_CHAR, "Bogowie sprowadzaj`a tu $3, aby sprawiedliwo`sci sta`lo si`e zado`s`c.", victim, NULL, ch, TO_NOTVICT );
    act( ZM_ZAWSZE, "Bogowie wys`luchali twych b`laga`n i sprowadzili tu $#. Pora wymierzy`c sprawiedliwo`s`c!", ch, NULL, victim, TO_CHAR );
    do_look( victim, "" ); /* dopiero teraz moze spojrzec */

    victim->pcdata->odwleczenie = ODWLECZENIE;

    return; /* jesli po returnie MOZE nie byc walki - NIE UZYWAC tej funkcji! */
}


/* Vigud: funkcja zastepcza dla makra FIGHTING, wiecej informacji w merc.h przy
   definicji makra. */
CHAR_DATA *fighting_func( CHAR_DATA *ch )
{
    return ch->fighting ? ch->fighting->vch : NULL;
}
