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
 *  Paladyn, dzielo Gimzy                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"
#include "handler.h"


KOMENDA( do_rozblysk )
{
    CHAR_DATA *victim = NULL;
    char      arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA  *obj;

    if ( IS_NPC( ch ) || !ch->pcdata->learned[ gsn_rozblysk ] )
    {
	send_to_char( "Nie potrafisz tego zrobi`c.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' && !ch->fighting )
    {
	send_to_char( "Kogo lub co chcesz o`slepi`c?\n\r", ch );
	return;
    }

    if ( arg[ 0 ] == '\0' && ch->fighting )
	victim = FIGHTING( ch );

    if ( arg[ 0 ] != '\0' )
    {
	 if ( !( victim = get_char_room( ch, arg ) ) )
	 {
	    send_to_char( "Nie wida`c takiego przeciwnika.\n\r", ch );
	    return;
	 }
    }

    if ( ch == victim )
    {
	send_to_char( "Nie chcesz chyba zrobi`c sobie krzywdy?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    if ( !get_eq_char( ch, WEAR_WIELD ) )
    {
	send_to_char( "Aby o`slepi`c, musisz dzier`zyc palady`nski m`lot.\n\r", ch );
	return;
    }

    obj = get_eq_char( ch, WEAR_WIELD );

    if ( obj->pIndexData->vnum != OBJ_VNUM_PALAD_MLOT )
    {
	send_to_char( "Do wytworzenia wi`azki `swiat`la potrzebujesz palady`nskiego m`lota.\n\r", ch );
	return;
    }
    else
    {
	act( ZM_WZROK | ZM_WID_CHAR, "$n unosi m`lot, z kt`orego strzela jasna wi`azka `swiat`la.", ch, NULL, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Nagle wszystko o`swietla jasna wi`azka `swiat`la.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Uwalniasz z m`lota bia`l`a wi`azk`e `swiat`la.\n\r", ch );

	WAIT_STATE( ch, 1 * PULSE_VIOLENCE );

	if ( spell_blindness( gsn_blindness, ch->level, ch, victim, 0 ) )
	    uzyj( ch, gsn_rozblysk, "o`slepianiu `swiat`lem" );

	if ( !is_fighting( ch, victim ) )
	{
	    check_killer( ch, victim );
	    if ( !is_fighting( ch, victim ) )
		set_fighting( ch, victim );

	    if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	    {
		set_fighting( victim, ch );
		multi_hit( victim, ch, TYPE_UNDEFINED );
	    }
	}
    }

    return;
}


CZAR( spell_pozytywne_doladowanie )
{
    ZONE_DATA *strefa;
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
	return FALSE;

    for ( strefa = zone_first; strefa; strefa = strefa->next )
	if ( ch->in_room->vnum == strefa->swiatynia )
	    break;

    if ( !strefa )
    {
	send_to_char( "Pozytywn`a energi`e mo`zesz czerpa`c tylko w `swi`atyniach.\n\r", ch );
	return FALSE;
    }

    if ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
    {
	send_to_char( "Pozytywna energia nie jest przeznaczona dla ciebie.\n\r", ch );
	return FALSE;
    }

    if ( IS_AFFECTED( ch, AFF_DOLADOWANIE )
      && !is_affected( ch, sn ) )
    {
	send_to_char( "Twoje cia`lo jest ju`z przepe`lnione pozytywn`a energi`a.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 20 + level / 2;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_DOLADOWANIE );

    if ( !affect_join( ch, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    send_to_char( "Pozytywna energia przep`lywa przez twoje cia`lo.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n do`ladowuje swoje cia`lo pozytywn`a energi`a.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_cud )
{
    ZONE_DATA *strefa;
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( ch->klasa != CLASS_PALADYN && ch->klasa != CLASS_KLERYK )
    {
	send_to_char( "Tylko wybra`ncy mog`a b`laga`c o cud.\n\r", ch );
	return FALSE;
    }

    for ( strefa = zone_first; strefa; strefa = strefa->next )
	if ( ch->in_room->vnum == strefa->swiatynia )
	    break;

    if ( !strefa )
    {
	send_to_char( "Znajd`x bardziej odpowiednie miejsce na modlitw`e o cud.\n\r", ch );
	return FALSE;
    }

    if ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
    {
	send_to_char( "Bogowie nie wys`luchaj`a modlitwy osoby z tak pod`lym charakterem.\n\r", ch );
	return FALSE;
    }

    if ( is_affected( ch, sn ) )
    {
	send_to_char( "Cuda nie zdarzaj`a si`e zbyt cz`esto.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 24 + ( ( 100 - ch->pcdata->learned[ sn ] ) / 3 )
		     - ( ch->level / 8 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( ch, &af );

    ch->hit = UMAX( ch->hit, ch->max_hit );
    ch->mana = UMAX( ch->mana, ch->max_mana );
    ch->move = UMAX( ch->move, ch->max_move );

    send_to_char( "Czujesz moc bog`ow p`lyn`ac`a przez twoje cia`lo.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "Moc bog`ow sp`lywa na $3.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_nalozenie_rak )
{
    CHAR_DATA 	*victim  =  (CHAR_DATA *)  vo;
    AFFECT_DATA *paf;
    bool blinded = FALSE;
    bool poisoned = FALSE;

    for ( paf = victim->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;

	if ( paf->type == gsn_blindness || paf->type == gsn_dazzle )
	{
	    affect_remove( victim, paf, TRUE );
	    blinded = TRUE;
	}

	if ( paf->type == gsn_poison )
	{
	    affect_remove( victim, paf, TRUE );
	    poisoned = TRUE;
	}
    }

    if ( blinded )
    {
	send_to_char( "Leczysz `slepot`e.\n\r", ch );
	send_to_char( "Mo`zesz ju`z widzie`c!\n\r", victim );
    }

    if ( poisoned )
    {
	send_to_char( "Neutralizujesz trucizn`e.\n\r", ch );
	send_to_char( "Trucizna w twoim ciele zosta`la zneutralizowana.\n\r", victim  );
    }

    return TRUE;
}


CZAR( spell_tarcza_archaniolow )
{
    AFFECT_DATA af;

    if ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
    {
	act( ZM_ZAWSZE, "Jeste`s z`l$y. `Zaden anio`l ci nie pomo`ze.", ch, NULL, NULL, TO_CHAR );
	return FALSE;
    }

    af.type     = sn;
    af.duration = ( 10 + level / 2 );
    af.location = APPLY_AC;
    af.modifier = 30;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( ch, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    send_to_char( "Archanio`lowie otaczaj`a ci`e magiczn`a tarcz`a.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "Archanio`lowie otaczaj`a $3 tarcz`a.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_sila_jednosci )
{
    CHAR_DATA   *gch;
    AFFECT_DATA *afp;
    AFFECT_DATA  af;
    bool strengthened;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
    {
	act( ZM_ZAWSZE, "Jeste`s z`l$y. Bogowie ci`e nie wspomog`a.", ch, NULL, NULL, TO_CHAR );
	return FALSE;
    }

    send_to_char( "Prosisz bog`ow o si`l`e dla siebie i swoich sprzymierze`nc`ow.\n\r", ch );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	strengthened = FALSE;

	if ( gch->deleted
	  || is_affected( gch, sn )
	  || !is_same_group( gch, ch ) )
	{
	    continue;
	}

	for ( afp = gch->affected; afp; afp = afp->next )
	{
	    if ( afp->deleted )
		continue;

	    if ( afp->location == APPLY_STR && afp->modifier > 0 )
		strengthened = TRUE;
	}

	if ( strengthened )
	    continue;

	af.type     = sn;
	af.duration = 10 + ( level / 2 );
	af.location = APPLY_STR;
	af.modifier = 1 + ( level >= 15 );
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	affect_to_char( gch, &af );

	if ( gch == ch )
	    send_to_char( "Sp`lywa na ciebie si`la bog`ow.\n\r", ch );
	else if ( gch->sex == 2 )
	    act( ZM_PRZYT, "Modlitwa $1 czyni ci`e silniejsz`a.", ch, NULL, gch, TO_VICT );
	else
	    act( ZM_PRZYT, "Modlitwa $1 czyni ci`e silniejszym.", ch, NULL, gch, TO_VICT );
    }

    return TRUE;
}


CZAR( spell_boski_pyl )
{
    CHAR_DATA *vch;
    AFFECT_DATA *paf;
    bool invisible;
    bool hidden;

    send_to_char( "Rozsypujesz boski py`l.\n\r", ch );
    act( ZM_WZROK, "$n rozsypuje boski py`l.", ch, NULL, NULL, TO_ROOM );

    if ( IS_OUTSIDE( ch ) )
    {
	if ( ch->in_room->area->weather_info.sky == SKY_RAINING
	  || ch->in_room->area->weather_info.sky == SKY_LIGHTNING )
	{
	    send_to_char( "Mocny wiatr rozwiewa py`l.\n\r", ch );
	    return FALSE;
	}
    }

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	invisible = FALSE;
	hidden = FALSE;

	if ( vch->deleted
	  || ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_WIZINVIS ) ) )
	    continue;

	for ( paf = vch->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
		continue;

	    if ( paf->type == gsn_invis || paf->type == gsn_mass_invis )
	    {
		affect_remove( vch, paf, TRUE );
		invisible = TRUE;
	    }

	    if ( paf->type == gsn_hide || paf->type == gsn_shadow || paf->type == gsn_sneak )
	    {
		affect_remove( vch, paf, TRUE );
		hidden = TRUE;
	    }
	}

	if ( invisible )
	{
	    act( ZM_WZROK, "Nie jeste`s ju`z niewidzialn$y.", vch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n nie jest ju`z niewidzialn$y.", vch, NULL, NULL, TO_ROOM );
	}

	if ( hidden )
	{
	    act( ZM_WZROK, "Nie jeste`s ju`z ukryt$y.", vch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n zosta`l$a zdemaskowan$y. ", vch, NULL, NULL, TO_ROOM );
	}
    }

    return TRUE;
}


CZAR( spell_spadajace_gwiazdy )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_nast;
    ROOM_INDEX_DATA *in_room = ch->in_room;

    if ( ch->in_room->sector_type == SECT_UNDERWATER )
    {
	send_to_char( "Nie sprowadzisz gwiazd pod wod`e.\n\r", ch );
	return FALSE;
    }

    if ( !IS_OUTSIDE( ch ) && ch->in_room->sector_type == SECT_INSIDE )
    {
	send_to_char( "Nie uda ci si`e tego zrobi`c w zamkni`etym pomieszczeniu.\n\r", ch );
	return FALSE;
    }

    if ( !IS_OUTSIDE( ch ) )
    {
	send_to_char( "Gwiazdy nie mog`a tu dotrze`c.\n\r", ch );
	return FALSE;
    }

    send_to_char( "Przywo`lujesz deszcz kamiennych meteoryt`ow rani`acy twoich wrog`ow.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n sprowadza kamienny deszcz na swoich wrog`ow!", ch, NULL, NULL, TO_ROOM );
    act( ZM_WZROK | ZM_W_WID_CHAR, "Na niebie pojawia si`e r`oj spadaj`acych meteoryt`ow.", ch, NULL, NULL, TO_ROOM );

    for ( vch = ch->in_room->people; vch; vch = vch_nast )
    {
	vch_nast = vch->next_in_room;

	if ( vch->in_room != in_room )
	    break;

	if ( vch->deleted
	  || vch == ch
	  || is_same_group( vch, ch )
	  || is_safe( ch, vch, FALSE ) )
	{
	    continue;
	}

	damage( ch, vch, level + dice( 3, 8 ), sn, WEAR_NONE, FALSE );

	if ( ch->deleted || ch->in_room != in_room )
	    return FALSE;
    }

    return TRUE;
}


CZAR( spell_aniol_stroz )
{
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( ch->klasa != CLASS_PALADYN && ch->klasa != CLASS_KLERYK )
    {
	send_to_char( "Twoje pro`sby s`a daremne.\n\r", ch );
	return FALSE;
    }

    if ( is_affected( ch, sn ) )
    {
	send_to_char( "Tylko jeden anio`l mo`ze czuwa`c nad jedn`a postaci`a.\n\r", ch );
	return FALSE;
    }

    if ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
    {
	send_to_char( "Ci`e`zar twoich grzech`ow jest zbyt wielki.\n\r", ch );
	send_to_char( "Bogowie nie wys`luchaj`a twoich pr`o`sb.\n\r", ch );
	return FALSE;
    }

    if ( ch->hit < 20 )
    {
	act( ZM_ZAWSZE, "Jeste`s zbyt poharatan$y, `zeby przywo`la`c anio`la.", ch, NULL, NULL, TO_CHAR );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 8 + ( level / 2 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_ANIOL_STROZ );
    affect_to_char( ch, &af );

    ch->pcdata->hp_angel = ch->hit * 15 / 100;

    send_to_char( "Anio`l zst`epuje z niebios, aby ci`e chroni`c.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "Anio`l zst`epuje z niebios, aby chroni`c $3.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_stworzenie_mlota )
{
    OBJ_DATA *mlot;
    AFFECT_DATA *paf;
    static const int hit_table[ ] = { 10, 15, 18, 20, 25, 40 };
    static const int dam_table[ ] = { 15, 25, 30, 35, 45, 65 };
    int dam_bonus;
    int hit_bonus;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( ch->klasa != CLASS_PALADYN )
    {
	send_to_char( "Tylko paladyni potrafi`a stworzy`c magiczny m`lot.\n\r", ch );
	return FALSE;
    }

    if ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
    {
	if ( ch->sex == 2 )
	    send_to_char( "Jeste`s upad`l`a paladynk`a.\n\r", ch );
	else
	    send_to_char( "Jeste`s upad`lym paladynem.\n\r", ch );

	send_to_char( "Bogowie nie wys`luchali twoich pr`o`sb.\n\r", ch );
	return FALSE;
    }

    hit_bonus = hit_table[ URANGE( 0, level / 20, 5 ) ];
    dam_bonus = dam_table[ URANGE( 0, level / 20, 5 ) ];

    mlot = create_object( get_obj_index( OBJ_VNUM_PALAD_MLOT ), level );

    paf                 = new_affect( );

    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_DAMROLL;
    paf->modifier       = number_fuzzy( dam_bonus );
    CLEAR_BITS( paf->bitvector );
    paf->next           = mlot->affected;
    mlot->affected      = paf;

    paf                 = new_affect( );

    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_HITROLL;
    paf->modifier       = number_fuzzy( hit_bonus );
    CLEAR_BITS( paf->bitvector );
    paf->next           = mlot->affected;
    mlot->affected      = paf;

    mlot->cost = 1;
    if ( !IS_OBJ_STAT( mlot, ITEM_PRIVATE )
      || !str_cmp( ch->name, mlot->owner ) )
    {
	free_string( mlot->owner );
	mlot->owner = str_dup( ch->name );
	SET_BIT_V( mlot->extra_flags, ITEM_PRIVATE );
    }

    obj_to_room( mlot, ch->in_room );
    act( ZM_WZROK | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch,  mlot, NULL, TO_CHAR );

    ch->mana = 0;
    ch->move /= 2;

    send_to_char( "Zmaterializowanie m`lota poch`lan`ia ca`le twoje magiczne si`ly.\n\r", ch );
    send_to_char( "Czujesz ogromne zm`eczenie.\n\r", ch );

    return TRUE;
}


CZAR( spell_niezlomna_wiara )
{
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( ch->klasa != CLASS_PALADYN )
    {
	send_to_char( "Tylko paladyni posiadaj`a tak siln`a wiar`e.\n\r", ch );
	return FALSE;
    }

    if ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
    {
	send_to_char( "Dobrzy bogowie nie pomagaj`a takim jak ty.\n\r", ch );
	return FALSE;
    }

    if ( is_affected( ch, sn ) )
    {
	send_to_char( "Nie nadu`zywaj cierpliwo`sci bog`ow.\n\r", ch );
	return FALSE;
    }
    /* Gimza: dopiero od 1/5 maks. zycia paladyn ma prawo prosic o boska pomoc */
    if ( ch->hit > ( UMAX( ch->hit, ch->max_hit ) / 5 ) )
    {
	send_to_char( "Nie szukaj pomocy u bog`ow nadaremnie.\n\r", ch );
	return FALSE;
    }

    ch->hit += ( 3 * URANGE( 30, level, 100 ) );
    update_pos( ch );

    send_to_char( "Czujesz si`e lepiej.\n\r", ch );

    af.type     = sn;
    af.duration = 15 + ( 100 - UMIN( level, 100 ) / 6 ) ;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( ch, &af );

    return TRUE;
}


CZAR( spell_swieta_aura )
{
    AFFECT_DATA af;
    bool polepszony = FALSE;
    bool wzmocniony = FALSE;

    if ( ch->klasa != CLASS_PALADYN
      && ch->klasa != CLASS_KLERYK
      && ch->level < L_SEN )
    {
	send_to_char( "Daj sobie spok`oj. `Swi`eta aura jest tylko dla wybranych.\n\r", ch );
	return FALSE;
    }

    if ( IS_AFFECTED( ch, AFF_SWIETA_AURA )
      && !is_affected( ch, sn ) )
    {
	act( ZM_ZAWSZE, "Ju`z jeste`s otoczon$y aur`a `swi`eto`sci.", ch, NULL, NULL, TO_CHAR );
	return FALSE;
    }

    if ( ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
      || ( !IS_NPC( ch )
	&& ( IS_SET( ch->act, PLR_KILLER )
	  || IS_SET( ch->act, PLR_THIEF ) ) ) )
    {
	send_to_char( "Taka szumowina jak ty nie jest godna aury `swi`eto`sci.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 10 + level / 4;
    af.location = APPLY_HITROLL;
    af.modifier = 15 + ( level / 4 );
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_SWIETA_AURA );
    polepszony |= affect_join( ch, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    af.location = APPLY_DAMROLL;
    af.modifier = 15 + ( level / 4 );
    CLEAR_BITS( af.bitvector );
    polepszony |= affect_join( ch, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    if ( !polepszony )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocniony );

    send_to_char( "Otacza ci`e aura `swi`eto`sci.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n otacza si`e aur`a `swi`eto`sci.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_wrzaca_krew )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    bool modifier;

    if ( IS_AFFECTED( victim, AFF_WRZACA_KREW ) )
    {
	send_to_char( "Krew twojego przeciwnika jest ju`z wystarczaj`aco gor`aca.\n\r", ch );
	return FALSE;
    }

    if ( IS_GOOD( victim ) )
    {
	send_to_char( "Nie znajdujesz z`la w twoim przeciwniku.\n\r", ch );
	return FALSE;
    }

    /* Lam 7.3.2006, #1266 */
    if ( IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_BLOOD ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    modifier = ( IS_EVIL( victim ) || CZY_WAMPIR( victim ) )
		 || ( !IS_NPC( victim )
		   && ( IS_SET( victim->act, PLR_KILLER )
		     || IS_SET( victim->act, PLR_THIEF ) ) );

    af.type      = sn;
    af.duration  = 2 + ( modifier ? 1 : 0 );
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_WRZACA_KREW );
    af.level = level;
    af.caster = ch;
    /* Gimza: wiadomo, ze jak krew sie gotuje, to w zbroi troche niewygodnie */
    if ( get_eq_char( victim, WEAR_BODY ) )
    {
	af.location = APPLY_AC;
	af.modifier = 0 - number_fuzzy( level );
    }
    else
    {
	af.location = APPLY_NONE;
	af.modifier = 0;
    }

    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Wprawiasz w ruch cz`astki z`la w twoim przeciwniku.\n\r", ch );
    act( ZM_SLUCH, "S`lyszysz bulgotanie w`lasnej krwi. Ale parzy!", victim, NULL, NULL, TO_CHAR );
    /* dalem sluch, ze niby bulgotanie */
    act( ZM_WZROK | ZM_SLUCH | ZM_WID_CHAR, "Krew $1 zaczyna si`e gotowa`c.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_strazniczy_glif )
{
    OBJ_DATA *object = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( object->level > ch->level )
    {
	send_to_char( "Nie mo`zesz ulepszy`c zbroi o poziomie wi`ekszym od twojego.\n\r", ch );
	return FALSE;
    }

    if ( !CAN_WEAR( object, ITEM_WEAR_BODY ) )
    {
	send_to_char( "Magiczne naci`ecia ochronne mog`a by`c wykonane tylko na zbroi.\n\r", ch );
	return FALSE;
    }

    if ( IS_SET( czy_pasuje( object, ch ), ~WRONG_PERSON ) )
    {
	send_to_char( "Ta zbroja nie b`edzie do ciebie pasowa`c.\n\r", ch );
	return FALSE;
    }

    for ( paf = object->affected; paf; paf = paf->next )
	if ( paf->location == APPLY_SAVING_PARA
	  || paf->location == APPLY_SAVING_ROD
	  || paf->location == APPLY_SAVING_PETRI
	  || paf->location == APPLY_SAVING_BREATH
	  || paf->location == APPLY_SAVING_SPELL )
	{
	    send_to_char( "Tej zbroi ju`z nie ulepszysz.\n\r", ch );
	    return FALSE;
	}

    paf                 = new_affect( );

    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_SAVING_SPELL;
    paf->modifier       = level / 2;
    CLEAR_BITS( paf->bitvector );
    paf->next           = object->affected;
    object->affected    = paf;
    if ( !IS_OBJ_STAT( object, ITEM_PRIVATE )
      || !str_cmp( ch->name, object->owner ) )
    {
	free_string( object->owner );
	object->owner = str_dup( ch->name );
	SET_BIT_V( object->extra_flags, ITEM_PRIVATE );
    }

    send_to_char( "Nacinasz magiczne znaki ochronne na swojej zbroi.\n\r", ch );

    return TRUE;
}


CZAR( spell_jednosc_obronna )
{
    CHAR_DATA *gch;
    CHAR_DATA *gch_next;
    AFFECT_DATA af;
    int modifier;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( ch->klasa != CLASS_PALADYN )
    {
	send_to_char( "Tylko paladyni potrafi`a si`e tak broni`c.\n\r", ch );
	return FALSE;
    }

    modifier = 1;
    /* Gimza: najpierw sprawdzam ilu paladynow jest w grupie paladyna */
    for ( gch = ch->in_room->people; gch; gch = gch_next )
    {
	gch_next = gch->next_in_room;

	if ( gch->deleted
	  || IS_NPC( gch )
	  || gch == ch
	  || gch->klasa != CLASS_PALADYN
	  || !is_same_group( gch, ch )
	  || is_affected( gch, sn ) )
	{
	    continue;
	}

	modifier++;
    }
    /* Gimza: teraz nakladam wplyw wykorzystujac powyzszy modyfikator */
    for ( gch = ch->in_room->people; gch; gch = gch_next )
    {
	gch_next = gch->next_in_room;

	if ( gch->deleted
	  || IS_NPC( gch )
	  || gch->klasa != CLASS_PALADYN
	  || !is_same_group( gch, ch )
	  || is_affected( gch, sn ) )
	{
	    continue;
	}

	af.type     = sn;
	af.duration = ch->level / 4;
	af.location = APPLY_AC;
	af.modifier = URANGE( level / 3, ( level / 3 ) * modifier, 150 );
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	affect_to_char( gch, &af );

	if ( modifier > 1 )
	    send_to_char( "Otaczaj`acy ci`e paladyni sprawiaj`a, `ze czujesz si`e bezpieczniej.\n\r", gch );
	else
	    send_to_char( "Czujesz si`e bezpieczniej.\n\r", gch );
    }

    return TRUE;
}


CZAR( spell_kradziez_zycia )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;	/* dawca zycia */
    CHAR_DATA *vch;		        	/* odbiorca zycia */
    CHAR_DATA *vch_next;
    CHAR_DATA *healed_ch;
    int hp_points;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( ch == victim )
    {
	send_to_char( "Chcesz odebra`c sobie `zycie?\n\r", ch );
	return FALSE;
    }

    if ( victim->race == zr_martwiak || CZY_WAMPIR( victim ) )
    {
	send_to_char( "Tw`oj przeciwnik nie nale`zy do `swiata `zywych istot.\n\r", ch );
	return FALSE;
    }

    healed_ch = ch; /* Lam 8.3.2005 dla Nifira */

    /* szukam odbiorcy zycia w swojej grupie - wlacznie z ch */
    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( vch->deleted
	  || IS_NPC( vch )
	  || CZY_WAMPIR( vch )
	  || !is_same_group( vch, ch ) )
	{
	    continue;
	}


	if ( !healed_ch )
	    healed_ch = vch;
	else if ( vch->hit * 100 / vch->max_hit <
		  healed_ch->hit * 100 / healed_ch->max_hit )
	{
	    healed_ch = vch;
	}
    }

    if ( ch->deleted || victim->deleted )
	return FALSE;

    if ( saves_spell( level, victim ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    hp_points = number_range( 15, level );

    send_to_char( "Czujesz, jak uchodzi z ciebie `zycie.\n\r", victim );

    if ( victim->hit - hp_points < 1 )
    {
	hp_points = hp_points - victim->hit;
	victim->hit = 1;
    }
    else
	victim->hit -= hp_points;

    healed_ch->hit = UMAX( healed_ch->hit,
		UMIN( healed_ch->hit + hp_points, healed_ch->max_hit ) );

    send_to_char( "Czujesz, jak wst`epuj`a w ciebie nowe si`ly.\n\r", healed_ch );
    update_pos( healed_ch );

    return TRUE;
}


CZAR( spell_stworzenie_grabiezcy )
{
    OBJ_DATA *miecz;
    AFFECT_DATA *paf;
    static const int bonus_table[ ] = { 15, 25, 30, 35, 40, 50 };
    int bonus;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( ch->klasa != CLASS_PALADYN )
    {
	send_to_char( "Tylko paladyni potrafi`a stworzy`c `Swi`etego Grabie`zc`e.\n\r", ch );
	return FALSE;
    }

    if ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
    {
	if ( ch->sex == 2 )
	    send_to_char( "Jeste`s upad`l`a paladynk`a.\n\r", ch );
	else
	    send_to_char( "Jeste`s upad`lym paladynem.\n\r", ch );

	send_to_char( "Bogowie nie wys`luchaj`a twoich pr`o`sb.\n\r", ch );
	return FALSE;
    }

    bonus = bonus_table[ URANGE( 0, level / 20, 5 ) ];
    miecz = create_object( get_obj_index( OBJ_VNUM_PALAD_MIECZ ), level );

    paf                 = new_affect( );

    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_DAMROLL;
    paf->modifier       = number_fuzzy( bonus );
    CLEAR_BITS( paf->bitvector );
    paf->next           = miecz->affected;
    miecz->affected     = paf;

    paf                 = new_affect( );

    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_HITROLL;
    paf->modifier       = number_fuzzy( bonus );
    CLEAR_BITS( paf->bitvector );
    paf->next           = miecz->affected;
    miecz->affected     = paf;

    SET_BIT_V( miecz->extra_flags, ITEM_MANA_LEECH );
    miecz->cost = 1;
    if ( !IS_OBJ_STAT( miecz, ITEM_PRIVATE )
      || !str_cmp( ch->name, miecz->owner ) )
    {
	free_string( miecz->owner );
	miecz->owner = str_dup( ch->name );
	SET_BIT_V( miecz->extra_flags, ITEM_PRIVATE );
    }

    obj_to_room( miecz, ch->in_room );
    act( ZM_WZROK | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch,  miecz, NULL, TO_CHAR );

    ch->mana = 0;
    ch->move /= 2;

    send_to_char( "Zmaterializowanie `Swi`etego Grabie`zcy poch`lania ca`le twoje magiczne si`ly.\n\r", ch );
    send_to_char( "Czujesz ogromne zm`eczenie.\n\r", ch );

    return TRUE;
}


CZAR( spell_jednosc_bojowa )
{
    CHAR_DATA *gch;
    CHAR_DATA *gch_next;
    AFFECT_DATA af;
    int modifier;

    if ( IS_NPC( ch ) )
	return FALSE;

    if ( ch->klasa != CLASS_PALADYN )
    {
	send_to_char( "Tylko paladyni potrafi`a tak walczy`c.\n\r", ch );
	return FALSE;
    }

    modifier = 1;

    for ( gch = ch->in_room->people; gch; gch = gch_next )
    {
	gch_next = gch->next_in_room;

	if ( gch->deleted
	  || IS_NPC( gch )
	  || gch == ch
	  || gch->klasa != CLASS_PALADYN
	  || !is_same_group( gch, ch )
	  || is_affected( gch, sn ) )
	{
	    continue;
	}

	modifier++;
    }

    for ( gch = ch->in_room->people; gch; gch = gch_next )
    {
	gch_next = gch->next_in_room;

	if ( gch->deleted
	  || IS_NPC( gch )
	  || gch->klasa != CLASS_PALADYN
	  || !is_same_group( gch, ch )
	  || is_affected( gch, sn ) )
	{
	    continue;
	}

	af.type     = sn;
	af.duration = ch->level / 4;
	af.location = APPLY_HITROLL;
	af.modifier = URANGE( ch->level / 3, ( ch->level / 3 ) * modifier, 50 );
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	affect_to_char( gch, &af );

	af.location  = APPLY_DAMROLL;
	affect_to_char( gch, &af );

	if ( modifier > 1 )
	{
	    send_to_char( "Zaczynasz rozumie`c si`e z innymi paladynami bez s`l`ow.\n\r", gch );
	    act( ZM_ZAWSZE, "Na pewno b`edziesz teraz lepiej walczy`l$a.", gch, NULL, NULL, TO_CHAR );
	}
	else
	    act( ZM_ZAWSZE, "Czujesz, `ze b`edziesz teraz lepiej walczy`l$a.", gch, NULL, NULL, TO_CHAR );
    }

    return TRUE;
}


CZAR( spell_duchowa_zbroja )
{
    AFFECT_DATA af;

    if ( ch->klasa != CLASS_PALADYN
      && ch->klasa != CLASS_KLERYK
      && ch->level < L_SEN )
    {
	send_to_char( "Twoja wiara jest zbyt s`laba.\n\r", ch );
	return FALSE;
    }

    if ( ( IS_EVIL( ch ) || CZY_WAMPIR( ch ) )
      || ( !IS_NPC( ch )
	&& ( IS_SET( ch->act, PLR_KILLER )
	  || IS_SET( ch->act, PLR_THIEF ) ) ) )
    {
	send_to_char( "Twoja wiara ju`z dawno umar`la.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level / 4;
    af.location = APPLY_AC;
    af.modifier = level / 2;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( ch, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    send_to_char( "Czujesz na sobie niewidzialn`a zbroj`e.\n\r", ch );

    return TRUE;
}
