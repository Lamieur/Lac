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
 *  Plik jest wytworem wyobrazni Qwerta. Jest obecnie glownie zbiorem      *
 *  czarow mnicha.                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "merc.h"
#include "magic.h"


CZAR( spell_aura_pokoju )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_SPOKOJ )
      && !is_affected( victim, sn ) )
    {
	send_to_char( "Ju`z odczuwasz spok`oj.\n\r", victim );
	return FALSE;
    }

    if ( IS_AFFECTED( victim, AFF_CHARM ) )
    {
	send_to_char( "Teraz nie mo`zesz si`e uspokoi`c.\n\r", victim );
	return FALSE;
    }

    /* zabojca ma prawo to wrzucic, przed graczem i tak nie uchroni :) */
    if ( IS_SET( victim->act, PLR_KILLER )
      || IS_SET( victim->act, PLR_THIEF ) )
    {
	send_to_char( "Twoja z`lo`s`c nie pozwala ci si`e uspokoi`c.\n\r", victim );
	return FALSE;
    }

    af.type       = sn;
    af.duration   = 1;
    af.location   = APPLY_NONE;
    af.modifier   = 0;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_SPOKOJ );
    af.level = level;
    af.caster = ch;

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    send_to_char( "Czujesz nag`ly przyp`lyw spokoju, ogarniaj`acy cia`lo.\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n wygl`ada bardzo przyja`xnie.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_duchowe_wsparcie )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    bool polepszony = FALSE;
    bool wzmocniony = FALSE;
    int zycie;

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_DAMROLL;
    af.modifier = level / 6;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    zycie = victim->hit;
    af.location = APPLY_HIT;
    af.modifier = level / 2;
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;
    victim->hit = zycie;

    af.location = APPLY_SAVING_SPELL;
    af.modifier = level / 10; /* mniejsze niz blogoslawienstwo */
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    if ( !polepszony )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocniony );

    if ( ch != victim )
	 send_to_char( "Rzucasz b`logos`lawie`nstwo duch`ow.\n\r", ch );
    send_to_char( "Czujesz na sobie b`logos`lawie`nstwo duch`ow.\n\r", victim );

    return TRUE;
}


CZAR( spell_bio_przyspieszenie )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    bool polepszony = FALSE;
    bool wzmocniony = FALSE;

    af.type     = sn;
    af.duration = level / 2;
    af.location = APPLY_CON;
    af.modifier = 1 + (level >= 20) + (level >= 40);
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    af.location = APPLY_MOVE;
    af.modifier = level;
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    if ( !polepszony )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocniony );

    send_to_char( "Poprawiasz funkcjonowanie swojego organizmu.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "Cia`lo $1 zaczyna si`e rozrasta`c.", ch, NULL, NULL, TO_ROOM);

    return TRUE;
}


CZAR( spell_przeniesienie_sily )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int zabierz;

    if ( is_affected( victim, sn ) )
    {
	send_to_char( "Twoja si`la ju`z jest przeniesiona.\n\r", victim );
	return FALSE;
    }

    zabierz = number_fuzzy( level * 3 );

    if ( zabierz > victim->hit )
	zabierz = victim->hit - 1; /* teraz moze miec najmniej 1 zycia */

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_MANA;
    af.modifier = zabierz;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( victim, &af );

    af.location = APPLY_HIT;
    af.modifier = -af.modifier;
    affect_to_char( victim, &af );

    if ( victim->max_hit < victim->hit )
	victim->hit = victim->max_hit;

    send_to_char( "Przenosisz swoj`a si`l`e fizyczn`a w zapasy energii.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n os`lab`l$a fizycznie.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}

/*
 * Qwert: wczesniej bylo za dobre i zawsze sie udawalo
 */
CZAR( spell_ostrze_ognia )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( IS_NPC( ch ) )
    {
	STC( "Moby nie rzucaj`a tego czaru.\n\r", ch );
	return FALSE;
    }

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC ) )
    {
	send_to_char( "Ten przedmiot nie mo`ze by`c wzmocniony.\n\r", ch );
	return FALSE;
    }

    if ( obj->wear_loc != WEAR_NONE )
	remove_obj( ch, obj->wear_loc, TRUE );

    if ( ch->deleted )
	return FALSE;

    if ( obj->level > ch->level )
    {
	send_to_char( "Nie mo`zesz wzmacnia`c broni o poziomie wy`zszym od twojego.\n\r",
		     ch );
	return FALSE;
    }

    if ( ( !IS_NPC( ch ) && !IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	&& is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) )
      || ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	&& !is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) ) )
    {
	send_to_char( "Nie potrafisz zaczarowa`c takiej broni.\n\r", ch );
	return FALSE;
    }

    SET_BIT_V( obj->extra_flags, ITEM_MAGIC );
    SET_BIT_V( obj->extra_flags, ITEM_FLAME );
    if ( number_percent( ) <=
	ch->pcdata->learned[ sn ] / 2 + 3 * ( ch->level - obj->level ) )
    {
	send_to_char( "Tworzysz p`lomienne ostrze.\n\r", ch );
	obj->level = UMAX( number_fuzzy( ch->level - 3 ), obj->level );
	if ( !IS_OBJ_STAT( obj, ITEM_PRIVATE )
	  || !str_cmp( ch->name, obj->owner ) )
	{
	    free_string( obj->owner );
	    obj->owner = str_dup( ch->name );
	    SET_BIT_V( obj->extra_flags, ITEM_PRIVATE );
	}
    }
    else
    {
	send_to_char( "Tworzysz p`lomienne ostrze o zbyt wysokiej temperaturze...\n\r", ch );
	SET_BIT_V( obj->extra_flags, ITEM_NODROP );
	obj->wear_flags = 1;
	obj->cost = 0;
    }

    return TRUE;
}


CZAR( spell_ostrze_chaosu )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( IS_NPC( ch ) )
    {
	STC( "Moby nie rzucaj`a tego czaru.\n\r", ch );
	return FALSE;
    }

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC ) )
    {
	send_to_char( "Ten przedmiot nie mo`ze by`c wzmocniony.\n\r", ch );
	return FALSE;
    }

    if ( obj->wear_loc != WEAR_NONE )
	remove_obj( ch, obj->wear_loc, TRUE );

    if ( ch->deleted )
	return FALSE;

    if ( obj->level > ch->level )
    {
	send_to_char( "Nie mo`zesz wzmacnia`c broni o poziomie wy`zszym od twojego.\n\r",
		     ch );
	return FALSE;
    }

    if ( ( !IS_NPC( ch ) && !IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	&& is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) )
      || ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	&& !is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) ) )
    {
	send_to_char( "Nie potrafisz zaczarowa`c takiej broni.\n\r", ch );
	return FALSE;
    }

    SET_BIT_V( obj->extra_flags, ITEM_MAGIC );
    SET_BIT_V( obj->extra_flags, ITEM_CHAOS );
    if ( number_percent( ) <=
	ch->pcdata->learned[ sn ] / 2 + 3 * ( ch->level - obj->level ) )
    {
	send_to_char( "Tworzysz ostrze chaosu.\n\r", ch );
	obj->level = UMAX( number_fuzzy( ch->level - 3 ), obj->level );
	if ( !IS_OBJ_STAT( obj, ITEM_PRIVATE )
	  || !str_cmp( ch->name, obj->owner ) )
	{
	    free_string( obj->owner );
	    obj->owner = str_dup( ch->name );
	    SET_BIT_V( obj->extra_flags, ITEM_PRIVATE );
	}
    }
    else
    {
	send_to_char( "Tworzysz ostrze chaosu, jednak nieco zbyt chaotyczne...\n\r", ch );
	SET_BIT_V( obj->extra_flags, ITEM_NODROP );
	obj->wear_flags = 1;
	obj->cost = 0;
    }

    return TRUE;
}


CZAR( spell_ostrze_lodu )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( IS_NPC( ch ) )
    {
	STC( "Moby nie rzucaj`a tego czaru.\n\r", ch );
	return FALSE;
    }

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC ) )
    {
	send_to_char( "Ten przedmiot nie mo`ze by`c wzmocniony.\n\r", ch );
	return FALSE;
    }

    if ( obj->wear_loc != WEAR_NONE )
	remove_obj( ch, obj->wear_loc, TRUE );

    if ( ch->deleted )
	return FALSE;

    if ( obj->level > ch->level )
    {
	send_to_char( "Nie mo`zesz wzmacnia`c broni o poziomie wy`zszym od twojego.\n\r",
		     ch );
	return FALSE;
    }

    if ( ( !IS_NPC( ch ) && !IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	&& is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) )
      || ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	&& !is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) ) )
    {
	send_to_char( "Nie potrafisz zaczarowa`c takiej broni.\n\r", ch );
	return FALSE;
    }

    SET_BIT_V( obj->extra_flags, ITEM_MAGIC );
    SET_BIT_V( obj->extra_flags, ITEM_ICY );
    if ( number_percent( ) <=
	ch->pcdata->learned[ sn ] / 2 + 3 * ( ch->level - obj->level ) )
    {
	send_to_char( "Tworzysz lodowe ostrze.\n\r", ch );
	obj->level = UMAX( number_fuzzy( ch->level - 3 ), obj->level );
	if ( !IS_OBJ_STAT( obj, ITEM_PRIVATE )
	  || !str_cmp( ch->name, obj->owner ) )
	{
	    free_string( obj->owner );
	    obj->owner = str_dup( ch->name );
	    SET_BIT_V( obj->extra_flags, ITEM_PRIVATE );
	}
    }
    else
    {
	send_to_char( "Tworzysz lodowe ostrze o zbyt niskiej temperaturze...\n\r", ch );
	SET_BIT_V( obj->extra_flags, ITEM_NODROP );
	obj->wear_flags = 1;
	obj->cost = 0;
    }

    return TRUE;
}


CZAR( spell_przeniesienie_umyslu )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = level / 6;
    af.location = APPLY_INT;
    af.modifier = 1 + ( level >= 18 ) + ( level >= 25 );
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    if ( ch != victim )
	send_to_char( "Rzucasz przeniesienie umys`lu.\n\r", ch );
    send_to_char( "Czujesz nag`ly przyp`lyw inteligencji.\n\r", victim );

    return TRUE;
}


CZAR( spell_lodowa_tarcza )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_LODOWA_TARCZA )
      && !is_affected( victim, sn ) )
    {
	send_to_char( "Lodowa tarcza ju`z ci`e chroni.\n\r", victim );
	return FALSE;
    }

    if ( IS_AFFECTED( victim, AFF_FLAMING ) )
    {
	send_to_char( "Jest ci za gor`aco, `zeby tworzy`c l`od.\n\r", victim );
	return FALSE;
    }

    af.type     = sn;
    af.duration = number_fuzzy( level / 4 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_LODOWA_TARCZA );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    send_to_char( "Otacza ci`e lodowa tarcza.\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n jest otoczon$y lodow`a tarcz`a.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_sztorm_lodowy )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = dice( level, 10 );

    if ( IS_AFFECTED( victim, AFF_LODOWA_TARCZA ) )
	dam /= 2;
    if ( IS_AFFECTED( victim, AFF_FLAMING ) )
	dam += dam/4;
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}

/*
 * Sila uderzenia dokladnie na poziomie uderzenia plomieni.
 * Uzyte do aff_lodowa_tarcza.
 */
CZAR( spell_lodowy_kolec )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = dice( 6, level );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_przekonanie )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Niedost`epne dla mob`ow.\n\r", ch );
	return FALSE;
    }

    if ( !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
    {
	send_to_char( "Przedmiot nie odpowiada...\n\r", ch );
	return FALSE;
    }

    if ( obj->wear_loc != WEAR_NONE )
	remove_obj( ch, obj->wear_loc, TRUE );

    if ( ch->deleted )
	return FALSE;

    if ( obj->level > ch->level || get_curr_int( ch ) < 18 )
    {
	send_to_char( "Duch przedmiotu jest dla ciebie zbyt inteligentny...\n\r", ch );
	return FALSE;
    }

    if (  IS_OBJ_STAT(  obj, ITEM_LOYAL        )
      &&  !IS_OBJ_STAT( obj, ITEM_EVIL         )
      &&  !IS_OBJ_STAT( obj, ITEM_ANTI_GOOD    )
      &&  !IS_OBJ_STAT( obj, ITEM_ANTI_EVIL    )
      &&  !IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ) )
    {
	send_to_char( "{TDuch przedmiotu m`owi ci: \"Czego ty jeszcze ode mnie chcesz?\"{x\n\r", ch );
	return FALSE;
    }

    if ( number_percent( ) <
     ch->pcdata->learned[ sn ] / 2 + 3 * ( ch->level - obj->level ) )
    {
	REMOVE_BIT_V( obj->extra_flags, ITEM_EVIL         );
	REMOVE_BIT_V( obj->extra_flags, ITEM_ANTI_GOOD    );
	REMOVE_BIT_V( obj->extra_flags, ITEM_ANTI_EVIL    );
	REMOVE_BIT_V( obj->extra_flags, ITEM_ANTI_NEUTRAL );
	SET_BIT_V( obj->extra_flags, ITEM_LOYAL );

	send_to_char( "Duch przedmiotu wydaje si`e by`c do ciebie przekonany.\n\r", ch );
	act( ZM_SLUCH | ZM_WID_CHAR, "$n rozmawia ze swoim inwentarzem! Oszala`l$a?!", ch, NULL, NULL, TO_ROOM );
	return TRUE;
    }

    send_to_char( "Duch przedmiotu znika razem z przedmiotem!\n\r", ch );
    extract_obj( obj );

    return FALSE;
}


CZAR( spell_dezorientacja )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DEZORIENTACJA ) )
    {
	if ( ch == victim )
	    ASTC( "Ju`z jeste`s zdezorientowan$y.", ch );
	else
	    send_to_char( "Twoja ofiara ju`z jest zdezorientowana.\n\r", ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    af.type      = sn;
    af.duration  = number_fuzzy( level / 4 );
    af.location  = APPLY_DEX;
    /* Ulryk: bylo APPLY_AC = - level, ale zmieniam dzialanie: */
    af.modifier  = -3;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_DEZORIENTACJA );
    af.level = level;
    af.caster = ch;
    affect_to_char( victim, &af );

    send_to_char( "Czujesz dezorientacj`e!\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n wydaje si`e by`c zdezorientowan$y.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_oczyszczenie )
{
    CHAR_DATA   *victim = (CHAR_DATA *) vo;
    AFFECT_DATA *paf;

    /* Lam 6.2.2006 */
    if ( is_affected( victim, gsn_poison ) )
    {
	affect_strip( victim, gsn_poison );
	send_to_char( "Ciep`le pr`ady przenikaj`a twoje cia`lo.\n\r", victim );
	act( ZM_WZROK | ZM_WID_CHAR, "$N wygl`ada zdrowiej.", ch, NULL, victim, TO_NOTVICT );
    }

    for ( paf = victim->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;

	if ( IS_SET( skill_table[ paf->type ].flags, SKILL_NO_DISPEL )
	    && skill_get_trust( ch ) < LEVEL_IMMORTAL )
	{
	    if ( skill_table[ paf->type ].spell_fun
	      && skill_table[ paf->type ].spell_fun != spell_null
	      && paf->type != gsn_poison )
	    {
		ch_printf( victim, "Nie mo`zesz oczy`sci`c si`e z czaru \"%s\".\n\r",
		       skill_table[ paf->type ].pl_name );
	    }
	    continue;
	}

	if ( paf->type == gsn_kontrola_umyslu
	    && skill_get_trust( ch ) < LEVEL_IMMORTAL
	    && ( !IS_NPC( victim ) ) )
	{
	    send_to_char( "Nie mo`zesz oczy`sci`c si`e z czaru \"kontrola umys`lu\".\n\r",
			 ch );
	    continue;
	}

	if ( !skill_table[ paf->type ].spell_fun
	  || skill_table[ paf->type ].spell_fun == spell_null )
	    continue;

	if ( IS_SET_V( paf->bitvector, AFF_SLUGA ) )
	{
	    send_to_char( "Nie mo`zesz oczy`sci`c si`e z tego czaru.\n\r",
			 ch );
	    continue;
	}
	affect_remove( victim, paf, TRUE );
    }

    act( ZM_ZAWSZE, "Poddajesz si`e rytua`lowi oczyszczaj`acemu.",
			ch, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR,  "$n podda`l$a si`e tajemniczemu rytua`lowi oczyszczaj`acemu.",
			ch, NULL, NULL, TO_ROOM );

    sprawdz_ubranie( ch );

    /* jak przeczyszczac to przeczyszczac :) */
    if ( !IS_NPC( victim ) && skill_get_trust( victim ) < LEVEL_IMMORTAL )
    {
	ch->pcdata->condition[ COND_FULL ] = 0;
	if ( !CZY_WAMPIR( ch ) )
	    ch->pcdata->condition[ COND_THIRST ] = 0;
    }

    return TRUE;
}


CZAR( spell_przeklenstwo_natury )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	send_to_char( "Twoja ofiara ju`z jest przekl`eta.\n\r", ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 6 + level;
    af.location = APPLY_HITROLL;
    af.modifier = 0 - level * 2 / 3;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( victim, &af );

    af.location  = APPLY_DAMROLL;
    affect_to_char( victim, &af );

    af.location  = APPLY_STR;
    af.modifier  = 0 - level / 30;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ofiara zosta`la przekl`eta.\n\r", ch );
    send_to_char( "Dosi`ega ci`e przekle`nstwo natury!.\n\r", victim );

    return TRUE;
}


CZAR( spell_przyspieszenie )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_PRZYSPIESZENIE )
      && !is_affected( victim, sn ) )
    {
	send_to_char( "Nie mo`zesz ju`z rusza`c si`e szybciej!\n\r", ch );
	return FALSE;
    }

    victim->move /= 2; /* musi cos kosztowac... */

    af.type     = sn;
    af.duration = level/4;
    af.location = APPLY_DEX;
    af.modifier = 1 + ( level >= 20 ) + ( level >= 30 );
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_PRZYSPIESZENIE );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    send_to_char( "Czujesz, `ze mo`zesz si`e szybciej porusza`c.\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n porusza si`e jakby szybciej.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_spowolnienie )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_SPOWOLNIENIE ) || saves_spell( level, victim ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }
    af.type     = sn;
    af.duration = level / 4;
    af.location = APPLY_DEX;
    af.modifier = -1 - ( level >= 20 ) - ( level >= 30 );
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_SPOWOLNIENIE );
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Twoja ofiara porusza si`e wolniej.\n\r", ch );
    send_to_char( "Nagle `swiat przyspieszy`l, co si`e dzieje?!\n\r", victim );

    return TRUE;
}


/*
 * Lam 20.11.2000: zmiana: teraz AFF_BLIND i AFF_CURSE staraja sie doczepic do
 *  pierwszego wplywu jaki zostanie rzucony, pod warunkiem, ze nie zostaly
 *  rzucone przez inne czary (wczesniej mozna bylo zdjac oslepienie i klatwe
 *  poprzez rzucenie na siebie tych czarow i zdjecie ich)
 *
 * Lam 8.3.2006: obnizam poziom, do 40 wlacznie oslepia z szansa 50%,
 *  powyzej 100%; znacznie skracam czas trwania
 */
CZAR( spell_zakazenie )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( ch == victim )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c na sobie.\n\r", ch );
	return FALSE;
    }

    if ( is_affected( victim, sn ) )
    {
	act( ZM_ZAWSZE, "$N ju`z jest zaka`zon$Y.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	act( ZM_ZAWSZE, "$N wytrzymuje pr`ob`e zaka`zenia.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    af.type	= sn;
/* to przeboj, przeciez skracalo sie do 10 + level / 3 :)
    af.duration	= ( 2 * level / 3 + 20 ) / 2; */
    af.duration	= 2 + level / 7;
    af.location	= APPLY_HITROLL;
    af.modifier	= -level / 2;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    if ( level > 40 && !IS_AFFECTED( victim, AFF_BLIND )
      && !IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_EYES ) )
	SET_BIT_V( af.bitvector, AFF_BLIND );
    affect_to_char( victim, &af );

    if ( !number_bits( 1 ) )
    {
	af.location = APPLY_DAMROLL;
	af.modifier = -level / 3;
	CLEAR_BITS( af.bitvector );
	if ( !IS_AFFECTED( victim, AFF_BLIND )
	  && !IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_EYES ) )
	    SET_BIT_V( af.bitvector, AFF_BLIND );
	else if ( !IS_AFFECTED( victim, AFF_CURSE ) )
	    SET_BIT_V( af.bitvector, AFF_CURSE );
	affect_to_char( victim, &af );
    }

    if ( level > 70 )
    {
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -level / 8;
	CLEAR_BITS( af.bitvector );
	if ( !IS_AFFECTED( victim, AFF_BLIND )
	  && !IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_EYES ) )
	    SET_BIT_V( af.bitvector, AFF_BLIND );
	else if ( !IS_AFFECTED( victim, AFF_CURSE ) )
	    SET_BIT_V( af.bitvector, AFF_CURSE );
	affect_to_char( victim, &af );
    }

    if ( !number_bits( 1 ) )
    {
	af.location	= APPLY_STR;
	af.modifier	= -1 - ( level >= 45 ) - ( level >= 70 ) - ( level >= 90 );
	CLEAR_BITS( af.bitvector );
	affect_to_char( victim, &af );
    }

    send_to_char( "Ofiara podda`la si`e zaka`zeniu.\n\r", ch );
    send_to_char( "Twoje cia`lo ogarnia zaka`zenie.\n\r", victim );

    return TRUE;
}


CZAR( spell_rytual_smierci )
{
    OBJ_DATA *obj;
    int	premia;

    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	if ( !obj->deleted
	  && can_see_obj( ch, obj )
	  && obj->item_type == ITEM_CORPSE_NPC )
	{
	    break;
	}

    if ( !obj )
    {
	send_to_char( "Nie widzisz cia`la do po`swi`ecenia w rytuale `smierci.\n\r", ch );
	return FALSE;
    }

    /* wzor sciagniety z pocalunku, a tamten oparty na wysysaniu */
    premia = ( ( interpolate( level, 200, 4500 ) + 2 * ch->max_mana ) - ch->mana ) / 128 + level / 16;
    ch->mana += UMAX( premia, 2 ) / 2 + dice( 2, 6 );
    send_to_char( "Poch`laniasz ca`l`a energi`e, kt`ora pozosta`la w ciele.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n podnosi $j i odprawia dziwny rytua`l.", ch, obj, NULL, TO_ROOM );
    extract_obj( obj );

    return TRUE;
}


CZAR( spell_sciana_ognia )
{
    CHAR_DATA *vch, *vch_nast;
    ROOM_INDEX_DATA *room = ch->in_room;
    int dam;

    if ( IS_SET( room->room_flags, ROOM_SAFE ) )
    {
	send_to_char( "Tu nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    send_to_char( "Eksploduje z ciebie `sciana ognia!\n\r", ch );
    act( ZM_WZROK, "$n tworzy `scian`e ognia!", ch, NULL, NULL, TO_ROOM );

    for ( vch = room->people; vch; vch = vch_nast )
    {
	vch_nast = vch->next_in_room;

	if ( vch->in_room != room )
	    break;

	if ( vch->deleted
	  || is_safe( ch, vch, FALSE ) )
	    continue;

	if ( vch != ch && !is_same_group( vch, ch ) )
	{
	    /* Lam 9.4.2005: dajemy to magom, zacznie reagowac na saves_spell */
	    dam = level + dice( level, 14 );
	    if ( saves_spell( level, vch ) )
		dam /= 2;
	    damage( ch, vch, dam, sn, WEAR_NONE, FALSE );
	}
	if ( ch->deleted || ch->in_room != room )
	    return FALSE;
    }

    return TRUE;
}


/*
 * Lam: w razie przypadkowego uzycia grozilo padem, blokuje komentujac, co jest
 * na pierwszy rzut oka nie tak
 */
CZAR( spell_kukla )
{
    NIEUDANE( ch );
    return FALSE;

#if 0
    CHAR_DATA *decoy;
    int num, count;
    AFFECT_DATA af;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char opis[ MAX_INPUT_LENGTH ];

    if ( is_affected( ch,sn ) || IS_NPC( victim ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }
    /* opisy graczy wcale tak nie wygladaja */
    if ( victim->long_descr[ 0 ] == '\0' )
	sprintf( opis,"%s%s stoi sobie tutaj.\n\r", victim->name, victim->pcdata->title );
    else
	strcpy( opis, victim->long_descr );

    num = number_range( 1, 3 );
    for ( count = 0; count < num; count++ )
    {
	decoy = create_mobile( get_mob_index( MOB_VNUM_KUKLA ) );

	free_string( decoy->long_descr );
	decoy->long_descr = str_dup( opis );

	free_string( decoy->name );
	decoy->name = str_dup( victim->name );

	free_string( decoy->short_descr );
	decoy->short_descr = str_dup( victim->short_descr );

	/* Lam 25.9.2002: wszystkie przypadki */
	free_string( decoy->dopelniacz );
	decoy->dopelniacz = str_dup( victim->dopelniacz );

	free_string( decoy->celownik );
	decoy->celownik = str_dup( victim->celownik );

	free_string( decoy->biernik );
	decoy->biernik = str_dup( victim->biernik );

	free_string( decoy->narzednik );
	decoy->narzednik = str_dup( victim->narzednik );

	free_string( decoy->miejscownik );
	decoy->miejscownik = str_dup( victim->miejscownik );

	free_string( decoy->wolacz );
	decoy->wolacz = str_dup( victim->wolacz );

	decoy->max_hit = 1;
	decoy->hit = 1;
	decoy->sex = victim->sex; /* Lam 25.9.2002 na ch, Lam 6.1.2004 na victim */
	/* nie wolno ustawiac mastera bezposrednio bez dodania zwod_master,
	   lepiej add_follower */
	/* decoy->master = ch; */ /* Tu bylo victim (blad) :( */
	char_to_room( decoy, victim->in_room );

    /* Qwert: jak wyciales mnichowi to juz z tym nie kombinuje */
    }
    send_to_char( "Jeste`s wsz`edzie!\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n jest wsz`edzie!", victim, NULL, NULL, TO_ROOM );
    af.type = sn;
    af.modifier = 0;
    af.duration = 50;
    CLEAR_BITS( af.bitvector );
    af.location = 0;
    af.level = level;
    af.caster = ch;
    affect_to_char( ch, &af );

    return TRUE;
#endif
}


CZAR( spell_przekazanie_przedmiotu )
{
    ROOM_INDEX_DATA *chroom, *victroom;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char buf[ MSL ];

    target_name = one_argument( target_name, arg1 );
    one_argument( target_name, arg2 );

    if ( arg1[ 0 ] == '\0'
      || arg2[ 0 ] == '\0')
    {
	send_to_char( "Co do kogo wys`la`c?\n\r", ch );
	return FALSE;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nie masz tego przy sobie.\n\r", ch );
	return FALSE;
    }
    /* Qwert: teraz Lac sie nie wysypie :) */
    if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
    {
	if ( !( victim = get_char_room( ch, arg2 ) ) )
	{
	    send_to_char( "Nie pozb`edziesz si`e tego w ten spos`ob.\n\r", ch );
	    return FALSE;
	}
    }
    else
	victim = get_char_world_pc( ch, arg2 );

    if ( victim == NULL || PP( victim ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return FALSE;
    }

    if ( victim == ch )
    {
	send_to_char( "Mo`ze po prostu prze`lo`zysz sobie to do drugiej r`eki?\n\r", ch );
	return FALSE;
    }

    if ( !IS_AWAKE( victim ) )
    {
	act( ZM_ZAWSZE, "Duch $! jest chyba zbyt nieprzytomny, aby przyj`a`c przedmiot.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    /* Lam 22.1.2006: analogicznie do telepatii, glownie po to, zeby zadaniowe
       give_progi nie musialy sie zastanawiac, czy inroom( $i ) == inroom( $n ) */
    if ( IS_NPC( victim ) && victim->in_room != ch->in_room )
    {
	act( ZM_ZAWSZE, "Duch $! jest za s`laby na przeniesienie przedmiotu na tak`a odleg`lo`s`c.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    if ( victim->level >= LEVEL_HERO && !IS_HERO( ch ) )
    {
	/* Lam 17.1.2003: brak polskiej literki */
	send_to_char( "Nie mo`zna wysy`la`c niczego do bohater`ow.\n\r", ch );
	return FALSE;  /* ale inny niesmiertelny moze... */
    }

    if ( victim->carry_weight >= can_carry_w( victim ) )
    {
	if ( obj->rodzaj == 3 )
	    act( ZM_ZAWSZE, "$p wa`z`a zbyt du`zo.", ch, obj, NULL, TO_CHAR );
	else
	    act( ZM_ZAWSZE, "$p wa`zy zbyt du`zo.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }

    if ( victim->carry_number >= can_carry_n( victim ) )
    {
	if ( obj->rodzaj == 3 )
	    act( ZM_ZAWSZE, "$p zajmuj`a zbyt du`zo inwentarza.", ch, obj, NULL, TO_CHAR );
	else
	    act( ZM_ZAWSZE, "$p zajmuje zbyt du`zo inwentarza.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }

    if ( IS_NPC( victim )
      && ( victim->pIndexData->pShop
	|| IS_SET( victim->act, ACT_NO_GIFTS ) ) )
    {
	act( ZM_ZAWSZE, "$N nie przyjmuje `zadnych przedmiot`ow.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    if ( obj->rodzaj != 3 )
	strcpy( buf, "$p rozb`lyskuje jasno, po czym znika." );
    else
	strcpy( buf, "$p rozb`lyskuj`a jasno, po czym znikaj`a." );
    act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_OBJ1, buf, ch, obj, NULL, TO_ROOM );

    zmien_poziom( obj, ch );
    obj_from_char( obj );

    zmien_poziom( obj, victim );
    obj_to_char( obj, victim );

    if ( obj->rodzaj != 3 )
	strcpy( buf, "$p pojawia si`e nagle w twoim inwentarzu." );
    else
	strcpy( buf, "$p pojawiaj`a si`e nagle w twoim inwentarzu." );
    act( ZM_WZROK | ZM_WID_OBJ1, buf, victim, obj, NULL, TO_CHAR );
    chroom = ch->in_room;
    victroom = victim->in_room;
    oprog_give_trigger( ch, victim, obj );
    if ( obj->deleted || ch->in_room != chroom || victim->in_room != victroom )
	return FALSE;
    mprog_give_trigger( victim, ch, obj );
    if ( obj->deleted || ch->in_room != chroom || victim->in_room != victroom )
	return FALSE;

    return TRUE;
}


CZAR( spell_regeneracja )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_REGENERACJA )
      && !is_affected( victim, sn ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    if ( is_affected( victim, gsn_degeneracja ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "$n wygl`ada zdrowiej.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Ciep`lo ogarnia twoje zniszczone cia`lo.\n\r", victim );
	affect_strip( victim, gsn_degeneracja );
    }

    af.type     = sn;
    af.duration = level / 5;
    af.location = APPLY_NONE;
    af.modifier = level / 11;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_REGENERACJA );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    send_to_char( "Twoje cia`lo posiad`lo zdolno`s`c szybszej regeneracji.\n\r", victim );

    return TRUE;
}


CZAR( spell_degeneracja )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "To cia`lo ju`z straci`lo zdolno`s`c do regeneracji.\n\r", ch );
	return FALSE;
    }

    if ( saves_spell( level, victim )
	|| victim->race == zr_troll
	|| CZY_WAMPIR( victim )
	|| victim->race == zr_martwiak )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    act( ZM_PRZYT, "Twoje cia`lo straci`lo zdolno`s`c regeneracji.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n nagle zblad`l$a.", victim, NULL, NULL, TO_ROOM );

    af.type     = sn;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.duration = level / 6;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_DEGENERACJA );
    affect_to_char( victim, &af );

    return TRUE;
}


CZAR( spell_porazenie )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    int loss;

    if ( ch == victim )
    {
	send_to_char( "Jak to sobie wyobra`zasz?\n\r", ch );
	return FALSE;
    }

    loss = number_range( 1, level );

    dam = number_range( 4, 8 ) * ( level / 8 );
    if ( ch->move < loss )
    {
       send_to_char( "Nie masz ju`z si`l by to zrobi`c!\n\r", ch );
       return FALSE;
    }

    ch->move = UMAX( 1, ch->move - loss );

    if ( saves_spell( level, victim ) )
	dam /= 2;
    else
	/* mnich z przyspieszeniem potrzebuje 6 pulsow czekania po porazeniu
	   i moze "za darmo" wrzucic jeszcze cos za 4 pulsy zanim ofiara bedzie
	   mogla uciec (o ile nie obroni kolejnego czaru); jesli mnich
	   zdecyduje sie na czar za 6, ofiara ma szanse nawiac */
	victim->wait = UMAX( victim->wait, 11 );
    send_to_char( "Twoje cia`lo zaczyna si`e iskrzy`c!\n\r", ch );

    act( ZM_WZROK | ZM_WID_CHAR, "Cia`lo $1 zaczyna si`e iskrzy`c!",
	 ch, NULL, NULL, TO_ROOM );
    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_wywolanie_strachu )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if ( IS_NPC( ch ) )
    {
	STC( "Moby nie mog`a tego robi`c.\n\r", ch );
	return FALSE;
    }

    if ( ( victim->level ) > ( level + 5 )
      || victim == ch
      || saves_spell( level, victim )  )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    dam = dice( 2, level / 8 ); /* slabiutko */
    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );
    if ( ch->deleted )
	return FALSE;
    if ( victim->deleted )
	return TRUE;
    /* te algorytmy liczace prawdopodobienstwo sa do niczego, wiec dam inaczej */
    if ( number_percent( ) <= ch->pcdata->learned[ sn ]
    &&   !number_bits( 1 ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "$N zaczyna si`e trz`a`s`c ze strachu!", ch, NULL, victim, TO_ROOM );
	send_to_char( "Nagle ogarnia ci`e wielki strach, musisz si`e schowa`c!\n\r", victim );
	do_flee( victim, "" );
    }

    return TRUE;
}


CZAR( spell_hipnoza )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo, *rch;
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
    {
	STC( "Mobom nie wolno hipnotyzowa`c graczy.\n\r", ch );
	return FALSE;
    }

    if ( victim == ch )
    {
	send_to_char( "Nie potrafisz si`e zahipnotyzowa`c.\n\r", ch );
	return FALSE;
    }

/*  if ( ch->level >= LEVEL_HERO && get_trust( ch ) < L_SEN )
    {
	send_to_char( "Z tak`a bosko`sci`a nie potrzebujesz s`lug.\n\r",
		     ch );
	sprintf( log_buf, "%s hipnotyzuje", ch->name );
	log_string( log_buf );
	return FALSE;
    } */

    if ( IS_AFFECTED( victim, AFF_CHARM )
      || IS_AFFECTED( ch,     AFF_CHARM )
      || level < victim->level
      || saves_spell( level, victim ) )
    {
	return FALSE;
    }

    if ( !IS_NPC( victim )
      || IS_SET( victim->act, ACT_NO_CHARM ) )
    {
	act( ZM_ZAWSZE, "$N nie pozwala si`e opanowa`c.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( is_fighting( rch, victim ) )
	{
	    act( ZM_ZAWSZE, "$N jest zbyt zaj`et$Y walk`a, `zeby zwraca`c na ciebie uwag`e.", ch, NULL, victim, TO_CHAR );
	    return FALSE;
	}
    }

    if ( victim->master )
	stop_follower( victim );

    if ( is_affected( victim, gsn_kontrola_umyslu ) ) /* wiemy, ze nie CHARM */
    {
	send_to_char( "Tracisz mo`zliwo`s`c kontrolowania s`lug.\n\r", victim );
	zabij_wszystkie_slugi( victim );
	affect_strip( victim, gsn_kontrola_umyslu );
    }

    add_follower( victim, ch );
    af.type     = sn;
    af.duration = number_fuzzy( level / 4 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_CHARM );
    affect_to_char( victim, &af );

    send_to_char( "Przejmujesz kontrol`e nad swoj`a ofiar`a.\n\r", ch );
    act( ZM_ZAWSZE, "$n przejmuje nad tob`a kontrol`e!", ch, NULL, victim, TO_VICT );

    return TRUE;
}


CZAR( spell_rewers )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int tymczas;

    /* Qwert: moze sie nie spodobac, ale ograniczam */
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Moby nie mog`a tego robi`c\n\r", ch );
	return FALSE;
    }

    if ( ch->klasa != CLASS_MNICH
    &&   skill_get_trust( ch ) < L_SEN )
    {
	send_to_char( "Zostaw to mnichom.\n\r", ch );
	return FALSE;
    }

    if ( is_affected( ch, sn ) )
    {
	send_to_char( "Nie jeste`s w stanie tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.modifier = 0; /* Qwert: praktyka pokazala, ze tego nie uzywaja :( */
    af.duration = level / 2;
    af.location = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( ch, &af );
    tymczas 	 = victim->hit;
    victim->hit  = victim->mana;
    victim->mana = tymczas;

    send_to_char( "Przenosisz swoje cia`lo astralne do `swiata fizycznego.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "Obraz $1 zaczyna migota`c.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_przekazanie_bolu )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if ( !ch->fighting || ( !is_fighting( victim, ch ) ) )
    {
	send_to_char( "Mo`zesz to zrobi`c tylko podczas walki z ofiar`a.\n\r", ch );
	return FALSE;
    }

    if ( ch->max_hit <= ch->hit )
    {
	send_to_char( "Nic ci`e nie boli!\n\r", ch );
	return FALSE;
    }

    /* Qwert: powyzej 70% jeszcze nie ma takiego bolu... */
    if ( ch->hit >= ( ( ch->max_hit ) * 0.7 ) )
    {
	send_to_char( "Tw`oj b`ol jest zbyt s`laby, by go komu`s przekaza`c.\n\r", ch );
	return FALSE;
    }

    if ( ch == victim )
    {
	 send_to_char( "Ju`z czujesz b`ol!\n\r", ch );
	 return FALSE;
    }

    dam = UMIN( dice( level, ch->max_hit/ch->hit ), dice( level, 15 ) + level );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_uwolnienie_umyslow )
{
    CHAR_DATA *vch = (CHAR_DATA *) vo;
    int czy = FALSE;

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	if ( !vch->deleted
	  && can_see( ch, vch ) /* Lam 23.3.2006 */
	  && !saves_spell( level, vch )
	  && number_bits( 2 )
	  && ( !IS_AFFECTED( vch, AFF_SLUGA ) )
	  && ( is_affected( vch, gsn_charm_person )
	    || is_affected( vch, gsn_domination )
	    || is_affected( vch, gsn_hipnoza )
	    || is_affected( vch, gsn_kontrola_umyslu ) ) )
	{
	    czy = TRUE;

	    affect_strip( vch, gsn_charm_person );
	    affect_strip( vch, gsn_domination );
	    affect_strip( vch, gsn_hipnoza );

	    if ( is_affected( vch, gsn_kontrola_umyslu )
	      && ( !IS_AFFECTED( vch, AFF_CHARM ) ) )
	    {
		STC( "Tracisz kontrol`e nad swoimi s`lugami!\n\r", vch );
		if ( ch != vch )
		    act( ZM_ZAWSZE, "Uwalniasz s`lugi $# z niewoli umys`lu!", ch, NULL, vch, TO_CHAR );
		zabij_wszystkie_slugi( vch );
		affect_strip( vch, gsn_kontrola_umyslu );
		continue;
	    }
	    affect_strip( vch, gsn_kontrola_umyslu );

	    send_to_char( "Zn`ow masz woln`a wol`e!\n\r", vch );
	    if ( ch != vch )
		act( ZM_ZAWSZE, "Uwalniasz $# z niewoli umys`lu!", ch, NULL, vch, TO_CHAR );
	}

    if ( !czy )
	send_to_char( "Nie uda`lo ci si`e nikogo uwolni`c.\n\r", ch );

    return czy;
}


CZAR( spell_znamie_smierci )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, gsn_znamie_smierci ) )
    {
	send_to_char( "Ta osoba zaraz umrze.\n\r", ch );
	return FALSE;
    }

    /* Lam: dodatkowe zabezpieczenie roznicy poziomow postaci, zeby nikt nie
       zrobil z tym rozdzek

       Qwert: wiec zmieniam tez dezintegracje psionika */
    if ( saves_spell( level, victim )
      || victim->level > ch->level + 4
      || victim->level >= LEVEL_IMMORTAL )
    {
	send_to_char( "Twoja ofiara unika pi`etna `smierci.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = number_range( 4, 6 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( victim, &af );

    send_to_char( "Szykuj si`e na spotkanie z przeznaczeniem!\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zostaje naznaczon$y znamieniem `smierci.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


/*
 * Qwert 27.02.04: latwiej teraz zapolowac na zabojcow
 */
CZAR( spell_przejscie )
{
    CHAR_DATA *victim;
    bool newif = TRUE;

    if ( IS_NPC( ch ) )
    {
	STC( "Moby nie rzucaj`a tego czaru.\n\r", ch );
	return FALSE;
    }

    if ( !( victim = get_char_area( ch, target_name ) ) )
    {
	send_to_char( "Nie ma takiej postaci w okolicy.\n\r", ch );
	return FALSE;
    }

    if ( victim == ch )
    {
	STC( "Ju`z jeste`s blisko siebie, nawet bardzo.\n\r", ch );
	return FALSE;
    }

    /* nie dalem sprawdzania pulapek - tu ma nie byc! */
    if ( !ch->fighting
      && !PP( victim )
      && IS_SET( victim->act, PLR_ZABOJCA )
      && ( victim->level <= level + 5
	|| CZY_WAMPIR( victim )
	|| is_name2( victim->name, ch->pcdata->nienawisc ) ) )
    {
	newif = FALSE; /* duzo latwiej przejsc */
    }

    if ( !victim->in_room
      || !can_enter_room( ch, victim->in_room, FALSE )
      || IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
      || IS_SET( ch->in_room->room_flags, ROOM_NO_SUMMON )
      || IS_SET( victim->in_room->room_flags, ROOM_NO_SUMMON )
      || victim->in_room->trap
      || PP( victim )
      || ( newif
	&& ( victim == ch
	  || IS_AFFECTED( ch, AFF_CURSE )
	  || victim->level >= level + 3
	  || ( IS_NPC( victim ) && saves_spell( level, victim ) )
	  || number_percent( ) > ch->pcdata->learned[ sn ] / 2 + 3 * ( ch->level - victim->level ) ) ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    PRZERWIJ_KOPANIE( ch, TRUE );
    PRZERWIJ_KARMPOJ( ch, TRUE );

    act( ZM_WZROK | ZM_WID_CHAR, "$n rozp`lywa si`e w powietrzu.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    act( ZM_ZAWSZE, "Przemieszczasz si`e do $!!", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "Nagle pojawia si`e tu $n.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    return TRUE;
}


CZAR( spell_kumulacja_mocy )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = 2; /* Lam: bylo 10 */
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_join( victim, &af, ADD_AFFECT );

    if ( ch != victim )
    {
	STC( "Kumulujesz moc.\n\r", ch );
	STC( "Czujesz nag`ly przyp`lyw mocy.\n\r", victim );
    }
    else
	STC( "Kumulujesz w sobie moc.\n\r", ch );

    return TRUE;
}


CZAR( spell_wyzwolenie_mocy )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA *poweraf;
    int potega = 0;
    int sciecie;

    if ( ch == victim )
    {
	send_to_char( "Nie mo`zesz tego sobie zrobi`c.\n\r", ch );
	return FALSE;
    }

    for ( poweraf = ch->affected; poweraf; poweraf = poweraf->next )
	if ( !poweraf->deleted
	  && poweraf->type == gsn_kumulacja_mocy )
	{
	    potega = 30 * poweraf->duration;
	    break;
	}

    if ( !potega )
    {
	send_to_char( "Nie masz `zadnej skumulowanej mocy.\n\r", ch );
	return FALSE;
    }

    /* Lam: uzalezniam od poziomu, mag na 53 poziomie mogl zabijac jednym
	    strzalem moby do mniej wiecej 75 poziomu (bez uswiecenia) */
    potega = UMIN( potega, level * 40 );

    /* Lam: zeby nie dalo sie tym dobijac. Sztyletowanie wymaga rozpoczynania
	    walki ze zdrowym przeciwnikiem, ale nie wymaga przygotowania, tu
	    bedzie nieco lepiej */
    /* zalozenie jest takie, zeby zabrac prawie, ale nie wszystko... */
    if ( HAS_SANCTUARY( victim ) )
	sciecie = UMIN( potega, victim->hit * 4 );
    else
	sciecie = UMIN( potega, victim->hit * 2 );
    /* ale nie mniej, niz by zabral nawet wybroniony wybuch plazmowy... */
    sciecie = UMAX( sciecie, dice( level, 11 ) / 2 );
    /* ale jednak nie wiecej, niz bylo skumulowane */
    potega = UMIN( potega, sciecie );

    STC( "Wznosisz r`ece ku g`orze, czuj`ac przep`lyw pot`e`znej mocy!\n\r",ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n wznosi r`ece ku g`orze.", ch, NULL, NULL, TO_ROOM );
    act( ZM_ZAWSZE, "Z twoich palc`ow strzela w kierunku $! o`slepiaj`acy promie`n!", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK, "Z palc`ow $1 strzela w twoim kierunku o`slepiaj`acy promie`n!", ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_VICT, "Z palc`ow $1 strzela w kierunku $! o`slepiaj`acy promie`n!", ch, NULL, victim, TO_NOTVICT );

    affect_strip( ch, gsn_kumulacja_mocy );
    damage( ch, victim, potega, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    /* Lam: przepisane, zeby naprawde odczuli uderzenie, mimo przyspieszenia */
    ch->wait = UMAX( ch->wait, 12 );
    /* Lam: dam szanse na ucieczke postaci, ktora juz mocno uszkodzono, zeby nie
       bylo mozliwosci dobicia kombinacja wyzwolenie + plazmowy, kiedy ofiara
       jest ogluszona */
    if ( victim->hit * 3 < victim->max_hit )
	victim->wait = UMAX( victim->wait, 10 );
    else
	victim->wait = UMAX( victim->wait, 15 );

    return TRUE;
}
