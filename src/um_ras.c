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
 *  Umiejetnosci ras (automatyczne i reczne) i polecenia tylko z rasami    *
 *  sie rozprawiajace.                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"

void	pc_breathe		args( ( CHAR_DATA *ch ) );
void	pc_screech		args( ( CHAR_DATA *ch ) );
void	pc_spit			args( ( CHAR_DATA *ch ) );
void	pc_lightning		args( ( CHAR_DATA *ch ) );
void	pc_blindness		args( ( CHAR_DATA *ch ) );
bool	check_race_special	args( ( CHAR_DATA *ch ) );

/* This code is for PC's who polymorph into dragons.
 * Yeah I know this is specialized code, but this is fun.  :)
 * Breathe on friend and enemy alike.
 * -Kahn
 */
void pc_breathe( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        sn;

    send_to_char( "Musisz sobie bekn`a`c!\n\r", ch );
    act( ZM_SLUCH, "$n beka!", ch, NULL, NULL, TO_ROOM );

    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;

	if ( victim->in_room != room )
	    break;

	if ( victim->deleted
	  || victim == ch
	  || is_safe( ch, victim, FALSE ) )
	    continue;

	if ( !IS_NPC( victim )
	  && !is_fighting( ch, victim )
	  && czy_morderstwo( ch, victim ) )
	    continue;

	sn = gsn_fire_breath;
	( *skill_table[ sn ].spell_fun )( sn, ch->level, ch, victim, USAGE_BREATH );

	if ( ch->deleted || ch->in_room != room )
	    break;
    }

    return;
}


/* This code is for PC's who polymorph into harpies.
 * Yeah I know this is specialized code, but this is fun.  :)
 * Scream into the ears of enemy and friend alike.
 * -Kahn
 */
void pc_screech( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        sn;

    send_to_char( "Musisz si`e wykrzycze`c!\n\r", ch );
    act( ZM_SLUCH, "$n g`lo`sno krzyczy!", ch, NULL, NULL, TO_ROOM );

    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;

	if ( victim->in_room != room )
	    break;

	if ( victim->deleted
	  || victim == ch
	  || is_safe( ch, victim, FALSE ) )
	    continue;

	if ( !IS_NPC( victim )
	  && !is_fighting( ch, victim )
	  && czy_morderstwo( ch, victim ) )
	    continue;

	act( ZM_SLUCH, "Uszy p`ekaj`a ci od krzyku $1. Au`c!", ch, NULL, victim,
	    TO_VICT );
	sn = skill_lookup_ang( "agitation" );
	( *skill_table[ sn ].spell_fun )( sn, ch->level, ch, victim, USAGE_BREATH );

	if ( ch->deleted || ch->in_room != room )
	    break;
    }

    return;
}


void pc_spit( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        sn;

    send_to_char( "Robisz u`zytek ze swojego jadu!\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n u`zywa swojego jadu!", ch, NULL, NULL, TO_ROOM );
    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;

	if ( victim->in_room != room )
	    break;

	if ( victim->deleted
	  || victim == ch
	  || is_safe( ch, victim, FALSE ) )
	    continue;

	if ( !IS_NPC( victim )
	  && !is_fighting( ch, victim )
	  && czy_morderstwo( ch, victim ) )
	    continue;

	act( ZM_PRZYT, "Dosi`ega ci`e jad $1.", ch, NULL, victim, TO_VICT );
	sn = gsn_poison;
	( *skill_table[ sn ].spell_fun )( sn, ch->level, ch, victim, USAGE_BREATH );
	damage( ch, victim, number_range( 1, ch->level ),
		gsn_poison_weapon, WEAR_NONE, FALSE );

	if ( ch->deleted || ch->in_room != room )
	    break;
    }

    return;
}


void pc_lightning( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        sn;

    send_to_char( "Unosisz r`ece ponad g`low`e...\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n unosi r`ece ponad g`low`e...", ch, NULL, NULL, TO_ROOM );
    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;

	if ( victim->in_room != room )
	    break;

	/* 50% szans na trafienie pradem kazdego w pomieszczeniu */
	if ( number_bits( 1 )
	  || victim->deleted
	  || victim == ch
	  || is_safe( ch, victim, FALSE ) )
	    continue;

	if ( !IS_NPC( victim )
	  && !is_fighting( ch, victim )
	  && czy_morderstwo( ch, victim ) )
	    continue;

	act( ZM_WZROK | ZM_WID_CHAR, "Z d`loni $1 strzela piorun!", ch, NULL, victim,
	    TO_VICT );
	sn = skill_lookup_ang( "lightning bolt" );
	( *skill_table[ sn ].spell_fun )( sn, ch->level, ch, victim, USAGE_BREATH );
	/* po trzepnieciu pioruna raczej powinnismy lezec na glebie :) */
	WAIT_STATE( victim, 2 * PULSE_PER_SECOND );

	if ( ch->deleted || ch->in_room != room )
	    break;
    }

    return;
}


void pc_blindness( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        sn;

    send_to_char( "Tw`oj r`og rozb`lyska jasnym `swiat`lem!\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "R`og $1 rozb`lyska jasnym `swiat`lem!", ch, NULL, NULL, TO_ROOM );
    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;

	/* Ulryk: siebie i oslepionych nie oslepia */
	if ( IS_AFFECTED( victim, AFF_BLIND )
	  || victim->deleted
	  || victim == ch
	  || is_safe( ch, victim, FALSE ) )
	    continue;

	/* Ulryk: trafienie NPC na 50%, PC na 25% */
	if ( ( IS_NPC( victim ) && number_bits( 1 ) )
	    || ( !IS_NPC ( victim ) && number_bits( 2 ) ) )
	{
	    act( ZM_WZROK, "Zd`a`zy`l$o`s os`loni`c wzrok przed o`slepiaj`acym `swiat`lem rogu!", victim, NULL, NULL, TO_CHAR );
	    continue;
	}

	if ( !IS_NPC( victim )
	  && !is_fighting( ch, victim )
	  && czy_morderstwo( ch, victim ) )
	    continue;

	sn = gsn_blindness;
	( *skill_table[ sn ].spell_fun )( sn, ch->level, ch, victim, USAGE_BREATH );
    }

    return;
}


bool check_race_special( CHAR_DATA *ch )
{
    /* Lam 7.9.2005: po m z sie mozna przypadkiem strzelic w kogos
       w pomieszczeniu, chociaz nie chcialo sie nikogo skrzywdzic - blokuje,
       zeby ch nie zostal z samobojcy zabojca.
       Przed zostaniem morderca atakujac kogos innego z "morduj" sa zablokowane
       poszczegolne funkcje rasowe. */
    if ( FIGHTING( ch ) == ch )
	return FALSE;

    if ( ch->race == zr_jednorozec
      && number_percent( ) < ch->level / 2 )
    {
	pc_blindness( ch );
	return TRUE;
    }

    if ( ch->race == zr_shambler
      && number_percent( ) < ch->level / 2 )
    {
	pc_lightning( ch );
	return TRUE;
    }

    if ( ch->race == zr_smok
      && number_percent( ) < ch->level / 2 )
    {
	pc_breathe( ch );
	return TRUE;
    }

    if ( ch->race == zr_harpia
      && number_percent( ) < ch->level / 2 )
    {
	pc_screech( ch );
	return TRUE;
    }

    if ( ( ch->race == zr_pajeczak || ch->race == zr_waz )
      && number_percent( ) < ch->level / 2 )
    {
	pc_spit( ch );
	return TRUE;
    }

    return FALSE;
}


KOMENDA( do_fee )
{
    send_to_char(
	"Je`sli chcesz si`e po`zywi`c cudzym kosztem, napisz PO`ZYW.\n\r", ch );
    return;
}


/* Vampiric bite.  Feeding */
KOMENDA( do_feed )
{
    CHAR_DATA   *victim;
    OBJ_DATA    *obj;
    AFFECT_DATA  af;
    char         arg [ MAX_INPUT_LENGTH ];
    char         kto [ MAX_INPUT_LENGTH ];
    int          dam;
    int          heal;
    int          baza;

    argument = one_argument( argument, arg );
    victim = NULL;

    if ( *arg )
    {
	pp_get( ch, arg ); /* Qwert: i tu mozna uzyc */
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	    return;
	}
    }

    if ( !victim
      && ( !ch->fighting
	|| !( victim = FIGHTING( ch ) ) ) )
    {
	send_to_char( "Kim chcesz si`e po`zywi`c?\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Nie si`egniesz sobie do szyi.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;
    check_killer( ch, victim );

    if ( IS_SET( race_table[ victim->race ].parts, P_GLOWA ) )
    {
	act( ZM_ZAWSZE, "`Z`adn$y krwi rzucasz si`e na szyj`e $!!", ch, NULL, victim, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n rzuca si`e na szyj`e $!!", ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rzuca si`e na tw`a szyj`e!", ch, NULL, victim, TO_VICT );
    }
    else
    {
	act( ZM_ZAWSZE, "`Z`adn$y krwi rzucasz si`e na $!!", ch, NULL, victim, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n rzuca si`e na $!!", ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rzuca si`e na ciebie!", ch, NULL, victim, TO_VICT );
    }

    WAIT_STATE( ch, 12 );

    if ( !is_fighting( ch, victim ) )
	set_fighting( ch, victim );
    if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	set_fighting( victim, ch );

    if ( !CZY_WAMPIR( ch ) )
    {
	ASTC( "Nie trafi`l$o`s!", ch );
	return;
    }

    baza = 30 / ( 1 + ( IS_NPC( ch ) ? 0 : ch->pcdata->wampiryzm.moc ) );

    if ( ch->fighting )
    {
	if ( number_percent( ) > UMAX( baza, ch->level - 5 )
	  && IS_AWAKE( victim ) )
	{
	    ASTC( "Nie trafi`l$o`s!", ch );
	    return;
	}
    }
    else
    {
	if ( number_percent( ) > UMAX( baza, ch->level * 2 )
	  && IS_AWAKE( victim ) )
	{
	    ASTC( "Nie trafi`l$o`s!", ch );
	    return;
	}
    }

    dam = dice( 3, ch->level );

    /* Lam 15.11.2002: najpierw zarazanie, potem atak. Wczesniej zdarzalo sie,
       ze wampir zabijal pozywiajac sie, a klatwa byla rzucana na ofiare juz
       w czysccu, albo podczas ugryzienia ofiara uciekala i zarazala sie w innym
       pomieszczeniu */

    /* Lam 19.12.2002: miesiac temu usilowalem przekonstruowac wszystkie te
       warunki w taki sposob, by nigdzie nie uzywac returna ani goto, ale
       w niektorych sytuacjach nie sprawdzalo sie, a mi sie juz nie chcialo
       "babrac" w 20 poziomach warunkow, wiec wrocilem do starej wersji
       i domalowalem goto :) Zmienie to kiedys! */

    /* Lam 15.2.2006: nie bylo sprawdzane! */
    if ( IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_BLOOD ) )
	goto ATAK;

    /* Lets see if we can get some food from this attack */
    if ( !IS_NPC( ch )
      && number_percent( ) < ( 34 - victim->level + ch->level ) )
    {
	/* If not hungry, thirsty or damaged, then continue */
	if ( ch->pcdata->condition[ COND_FULL ] > 40
	  && ch->pcdata->condition[ COND_THIRST ] > 40
	  && ch->max_hit
	  && ( ch->hit * 100 / ch->max_hit > 75
	    || number_percent( ) < ch->hit * 100 / ch->max_hit - 10 ) )
	{
	    goto ATAK;
	}
	else
	{
	    if ( race_table[ victim->race ].parts == P_HS )
		send_to_char( "`Lapczywie pijesz `swie`z`a krew prosto z rany.\n\r", ch );
	    else if ( number_bits( 1 ) )
		send_to_char( "Gryziesz z wigorem, aby po chwili delektowa`c si`e smakiem juchy.\n\r", ch );
	    else
		send_to_char( "Kilkakrotnie wbijasz k`ly w ofiar`e, s`acz`ac po`zywn`a posok`e.\n\r", ch );
	    heal = number_range( 1, dam );
	    ch->hit = UMIN( ch->hit + heal, UMAX( ch->max_hit, ch->hit ) );
	    /* Get full as per drinking blood */
	    gain_condition( ch, COND_FULL, heal * 2 );
	    /* Quench thirst as per drinking blood absolute value */
	    gain_condition( ch, COND_THIRST, heal );
	    dam *= 2;
	}
    }

    /* Lam 26.2.2007: nie wiem, dlaczego zrobilismy to po tylu latach */
    if ( !*argument || str_prefix( argument, "zara`zaj`ac" ) )
	goto ATAK;

    /* Ok now we see if we can infect the victim */
    if ( IS_NPC( victim ) )
	goto ATAK;

    if ( CZY_WAMPIR( victim ) || IS_AFFECTED( victim, AFF_POLYMORPH ) )
	goto ATAK;

    /* Lam: ponizej bylo age < 21, ale w koncu doszedlem do wniosku ze to
	    bez sensu, albowiem nikt nikogo nie zarazal. Teraz zadnego
	    ograniczenia wiekiem nie ma, moze sobie ktos poszaleje... */
    if ( victim->level < 11 /* || get_age( victim ) < 18 */ )
	goto ATAK;

    if ( IS_AFFECTED( victim, AFF_DOLADOWANIE ) && !IS_EVIL( victim ) )
	goto ATAK;

    if ( saves_spell( ch->level, victim ) )
	goto ATAK;

    /* Lam: w Envy tu bylo ITEM_HOLD i to w ogole nie dzialalo! */
    if ( ( obj = get_eq_char( victim, WEAR_HOLD ) ) )
    {
	if ( IS_OBJ_STAT( obj, ITEM_VAMPIRE_BANE )
	  && ch->level < 111 ) /* Lam: bylo 21; Ulryk: bylo 41 */
	{
	    goto ATAK;
	}
	if ( IS_OBJ_STAT( obj, ITEM_HOLY )
	  && ( ch->level < 32 || victim->level > ch->level ) )
	{
	    goto ATAK;
	}
    }

    af.type      = gsn_vampiric_bite;
    af.duration  = UMAX( 5, 30 - ch->level ); /* to malo, ale po kilku gryzach... */
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_VAMP_BITE );
    af.level = ch->level;
    af.caster = ch;
    affect_join( victim, &af, ENH_AFFECT );
    if ( !IS_NPC( victim ) )
    {
	free_string( victim->pcdata->wampiryzm.pochodzenie );
	sprintf( kto, "%s %%s", ch->name );
	victim->pcdata->wampiryzm.pochodzenie = str_dup( kto );
	victim->pcdata->wampiryzm.moc =
		( !IS_NPC( ch ) ? ch->pcdata->wampiryzm.moc : 0 ) + 1;
    }
    send_to_char( "Och! Posmakuj tej energii!\n\r", ch );
    send_to_char( "Twoja krew zaczyna ci`e parzy`c!\n\r", victim );

ATAK:
    damage( ch, victim, dam, gsn_vampiric_bite, WEAR_NONE, FALSE );

    return;
}


/* Lam 20.11.2003 */
KOMENDA( do_nietoperz )
{
    AFFECT_DATA af;
    OBJ_DATA *obj;

    if ( ch->race == zr_nietoperz )
    {
	if ( is_affected( ch, gsn_postac_nietoperza ) )
	{
	    STC( "Powracasz do swojej postaci.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n powraca do postaci wampira.", ch, NULL, NULL, TO_ROOM );
	    affect_strip( ch, gsn_postac_nietoperza );
	    return;
	}
	STC( "Jeste`s przecie`z nietoperzem.\n\r", ch );
	return;
    }

    if ( ch->race != zr_wampir )
    {
	STC( "Nie jeste`s wampirem.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_POLYMORPH ) )
    {
	STC( "Dzia`la ju`z na ciebie pot`e`zniejsza przemiana.\n\r", ch );
	return;
    }

    if ( ch->in_room->area->weather_info.sunlight != SUN_SET
      && ch->in_room->area->weather_info.sunlight != SUN_DARK )
    {
	int wymog = 30 + ( IS_NPC( ch ) ? 0 : ch->pcdata->wampiryzm.moc ) * 10;
	wymog = UMIN( wymog, 60 );
	if ( ch->level < wymog )
	{
	    STC( "Mo`zesz to robi`c tylko noc`a.\n\r", ch );
	    return;
	}

	if ( !IS_SET( ch->in_room->room_flags, ROOM_UNDERGROUND )
	  && ch->in_room->sector_type != SECT_UNDERWATER
	  && ch->level < 90 )
	{
	    STC( "Jest tu do tego zbyt jasno.\n\r", ch );
	    return;
	}
    }

    STC( "Zmieniasz si`e w nietoperza.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zmienia si`e w nietoperza.", ch, NULL, NULL, TO_ROOM );

    /* przemiana zabierze mu sile i moglby stracic bron, wiec zdejmijmy, reszta
       sprzetu spadnie mu w check_race_wearlocs( ). Uzywam wersji _save, bo
       2. bron moze tez wplywac na sile */
    if ( ( obj = get_eq_char( ch, WEAR_WIELD_2 ) ) )
	unequip_char_save( ch, obj );

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) )
	unequip_char_save( ch, obj );

    /* wspolne wlasciwosci wplywu */
    af.type = gsn_postac_nietoperza;
    af.duration = ch->level / 10;
    af.level = ch->level;
    af.caster = ch;
    CLEAR_BITS( af.bitvector );

    /* wplyw pierwszy, rasa nietoperza */
    af.location = APPLY_RACE;
    af.modifier = zr_nietoperz - ch->race;
    SET_BIT_V( af.bitvector, AFF_POSTAC_NIETOPERZA );
    affect_to_char( ch, &af );

    /* przed nastepnymi wplywami, zeby zycie nie spadlo mu na minus po
       spadnieciu rzeczy */
    /* check_race_wearlocs( ch ); - zastapilem silniejsza wersja */
    sprawdz_ubranie( ch );
    REMOVE_BIT_V( af.bitvector, AFF_POSTAC_NIETOPERZA );

    /* wplyw drugi, wysoka zrecznosc */
    af.location = APPLY_DEX;
    af.modifier = ch->level / 6 + 1;
    affect_to_char( ch, &af );

    if ( ch->max_hit <= 20 )
	return;

    /* wplyw trzeci, sciecie zycia */
    af.location = APPLY_HIT;
    af.modifier = 0 - ch->max_hit * 4 / 5;
    affect_to_char( ch, &af );

    ch->hit = UMIN( ch->hit, ch->max_hit );

    return;
}


KOMENDA( do_stake )
{
    CHAR_DATA   *victim;
    OBJ_DATA    *obj;
    AFFECT_DATA  af;
    char         arg [ MAX_INPUT_LENGTH ];
    bool         found = FALSE;
    int          chance;

    if ( !check_blind( ch ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Komu wbi`c ko`lek w serce?\n\r", ch );
	return;
    }

    pp_get( ch, arg );
    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Nie mo`zesz tego robi`c na mobach.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;
    check_killer( ch, victim );

    /* Check here to see if they have a stake in hand */

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( obj->pIndexData->vnum == OBJ_VNUM_STAKE
	    && obj->wear_loc == WEAR_WIELD )
	{
	    found = TRUE;
	    break;
	}

    if ( !found )
    {
	send_to_char( "`Zeby to robi`c potrzebujesz odpowiedniego narz`edzia.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rzuca si`e na $#, nagle zdaj`ac sobie spraw`e, `ze ma puste r`ece.",
	    ch, NULL, victim, TO_ROOM );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return;
    }

    act( ZM_WZROK | ZM_WID_VICT, "$n rzuca si`e na $# z $k.", ch, obj, victim, TO_NOTVICT );
    act( ZM_ZAWSZE, "Rzucasz si`e na $# z $k.", ch, obj, victim, TO_CHAR );
    act( ZM_WZROK, "$n rzuca si`e na ciebie z $k.", ch, obj, victim, TO_VICT );

    /* Calculate chances, give vampire the ghoul effect,
       autokill the vampire. */
    chance = ( IS_NPC( ch ) ? ch->level : ch->pcdata->learned[ gsn_stake ] );
    chance = chance - victim->level + ch->level;

    if ( number_percent( ) < chance
	&& !IS_AWAKE( victim )
	&& CZY_WAMPIR( victim ) )
    {
	act( ZM_WZROK | ZM_WID_VICT, "$n przebija serce $#", ch, obj, victim, TO_NOTVICT );
	act( ZM_ZAWSZE, "Przebijasz serce $#.", ch, obj, victim, TO_CHAR );
	act( ZM_ZAWSZE, "$n przebija twe serce.", ch, obj, victim, TO_VICT );

	victim->hit = -10;
	damage( ch, victim, 40, gsn_stake, WEAR_NONE, FALSE );

	af.type      = gsn_stake;
	af.duration  = 10;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_GHOUL );
	af.level = ch->level;
	af.caster = ch;
	affect_join( victim, &af, ADD_AFFECT );

	uzyj( ch, gsn_stake, "ko`lkowaniu" );
    }
    else
	multi_hit( ch, victim, TYPE_UNDEFINED );

    return;
}


/*
 * Lam: skopiowane z techniki smoka
 * Lam 26.1.2006: #1290
 */
KOMENDA( do_taranuj )
{
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    if ( ch->race != zr_minotaur && ch->race != zr_byk )
    {
	send_to_char( "Nie umiesz taranowa`c nikogo g`low`a.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    victim = ( *arg ) ? get_char_room( ch, arg ) : FIGHTING( ch );

    if ( !victim )
    {
	send_to_char( "Kogo staranowa`c g`low`a?\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Bezskutecznie pr`obujesz trafi`c czo`lem we w`lasny brzuch.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    WAIT_STATE( ch, skill_table[ gsn_taranowanie ].beats );

    act( ZM_ZAWSZE, "Bierzesz kr`otki rozbieg i uderzasz w $# czo`lem.", ch, NULL, victim, TO_CHAR );
    act( ZM_PRZYT, "$n bierze kr`otki rozbieg i uderza ci`e czo`lem.", ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_CHAR, "$n bierze kr`otki rozbieg i uderza w $# czo`lem.", ch, NULL, victim, TO_NOTVICT );

    if ( number_percent( ) <= URANGE( 20, ch->level, 70 )
				+ 2 * ( ch->level - victim->level ) )
    {
	SET_BIT_V( victim->affected_by, AFF_DISORIENTATED );
	WAIT_STATE( victim, skill_table[ gsn_taranowanie ].beats + 4 );
	act( ZM_PRZYT, "Si`la uderzenia sprawia, `ze tracisz na moment orientacj`e.", victim, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Si`la uderzenia sprawia, `ze $n traci na moment orientacj`e.", victim, NULL, NULL, TO_ROOM );
	damage( ch, victim,
		number_range( ch->level, ch->level * 3 / 2 ),
		gsn_taranowanie, WEAR_NONE, FALSE );
    }
    else
	damage( ch, victim, number_range( ch->level / 2, ch->level ),
		gsn_taranowanie, WEAR_NONE, FALSE );

    return;
}
