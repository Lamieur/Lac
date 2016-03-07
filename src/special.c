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
#include "special.h"
#include "const.h"
#include "db.h"


bool	dragon		args( ( CHAR_DATA *ch, int sn ) );
char	*last_word	args( ( char *argument ) );
char	*przeciagaj	args( ( char *argument ) );

/*
 * Lam 4.2.2004: tablica zamiast stosu ifow w funkcjach ponizej
 */
struct spec_type spec_table[ ] =
{
    { spec_breath_any,		"spec_breath_any" },
    { spec_breath_acid,		"spec_breath_acid" },
    { spec_breath_fire,		"spec_breath_fire" },
    { spec_breath_frost,	"spec_breath_frost" },
    { spec_breath_gas,		"spec_breath_gas" },
    { spec_breath_lightning,	"spec_breath_lightning" },
    { spec_cast_adept,		"spec_cast_adept" },
    { spec_cast_cleric,		"spec_cast_cleric" },
    { spec_cast_ghost,		"spec_cast_ghost" },
    { spec_cast_mage,		"spec_cast_mage" },
    { spec_cast_psionicist,	"spec_cast_psionicist" },
    { spec_cast_undead,		"spec_cast_undead" },
    { spec_executioner,		"spec_executioner" },
    { spec_fido,		"spec_fido" },
    { spec_guard,		"spec_guard" },
    { spec_janitor,		"spec_janitor" },
    { spec_mayor,		"spec_mayor" },
    { spec_poison,		"spec_poison" },
    { spec_repairman,		"spec_repairman" },
    { spec_thief,		"spec_thief" },
    { spec_healer,		"spec_healer" },
    { spec_waker,		"spec_waker" },
    { spec_unicorn,		"spec_unicorn" },
    { spec_parrot,		"spec_parrot" },
    { spec_vampire_elder,	"spec_vampire_elder" },
    { NULL,			NULL }
};


SPEC_FUN *spec_lookup( const char *name )
{
    int i;

    for ( i = 0; spec_table[ i ].fun; i++ )
	if ( !str_cmp( name, spec_table[ i ].name ) )
	    break;

    return spec_table[ i ].fun;
}


char *spec_name( SPEC_FUN *fun )
{
    int i;

    if ( fun == NULL )
	return "NULL";

    for ( i = 0; spec_table[ i ].fun; i++ )
	if ( fun == spec_table[ i ].fun )
	    break;

    if ( !spec_table[ i ].fun )
	return "nieznana!";
    else
	return spec_table[ i ].name;
}


char spec_list[ MSL ];

void build_spec_list( void )
{
    int i;

    /* coz, zalozylem, ze bedzie jedna specjalnosc :) */
    strcpy( spec_list, spec_table[ 0 ].name );

    for ( i = 1; spec_table[ i ].fun; i++ )
    {
	strcat( spec_list, " " );
	strcat( spec_list, spec_table[ i ].name );
    }

    return;
}


/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, int sn )
{
    CHAR_DATA *victim;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->deleted )
	    continue;
	if ( is_safe( ch, victim, FALSE ) )
	    continue;
	if ( is_fighting( victim, ch ) && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    ( *skill_table[ sn ].spell_fun )( sn, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );

    return TRUE;
}


/*
 * Special procedures for mobiles.
 */
SPECJALNOSC( spec_breath_any )
{
    if ( ch->position != POS_FIGHTING )
	return FALSE;

    switch ( number_bits( 3 ) )
    {
	case 0: return spec_breath_fire		( ch );
	case 1:
	case 2: return spec_breath_lightning	( ch );
	case 3: return spec_breath_gas		( ch );
	case 4: return spec_breath_acid		( ch );
	case 5:
	case 6:
	case 7: return spec_breath_frost	( ch );
    }

    return FALSE;
}


SPECJALNOSC( spec_breath_acid )
{
    return dragon( ch, gsn_acid_breath );
}


SPECJALNOSC( spec_breath_fire )
{
    return dragon( ch, gsn_fire_breath );
}


SPECJALNOSC( spec_breath_frost )
{
    return dragon( ch, gsn_frost_breath );
}


SPECJALNOSC( spec_breath_gas )
{
    if ( ch->position != POS_FIGHTING )
	return FALSE;

    ( *skill_table[ gsn_gas_breath ].spell_fun )( gsn_gas_breath, SPELL_LEVEL( ch, ch->level ), ch, NULL, USAGE_SPECIAL );

    return TRUE;
}


SPECJALNOSC( spec_breath_lightning )
{
    return dragon( ch, gsn_lightning_breath );
}


SPECJALNOSC( spec_cast_adept )
{
    CHAR_DATA *victim;

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	/* Lam 12.1.2003: uzdrowiciele nie popieraja mordercow */
	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	    continue;
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
	    break;
    }

    if ( !victim
      || IS_NPC( victim )
      || victim->level > 100 )
	return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0:
	say_spell( ch, gsn_armor );
	spell_armor( gsn_armor, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;

    case 1:
	say_spell( ch, gsn_bless );
	spell_bless( gsn_bless, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;

    case 2:
	say_spell( ch, gsn_mana_gift );
	spell_mana_gift( gsn_mana_gift,
			 SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;

    case 3:
	say_spell( ch, gsn_cure_light );
	spell_cure_light( gsn_cure_light,
			 SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;

/*    case 4:
	act( "$n utters the words 'rozar'.",  ch, NULL, NULL, TO_ROOM );
	spell_cure_poison( gsn_cure_poison,
			  SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;
*/
    case 5:
	say_spell( ch, gsn_refresh );
	spell_refresh( gsn_refresh, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;

    case 6:
	say_spell( ch, gsn_combat_mind );
	spell_combat_mind( gsn_combat_mind, SPELL_LEVEL( ch, ch->level ), ch,
			  victim, USAGE_SPECIAL );
	return TRUE;

    case 7:
	say_spell( ch, gsn_shield );
	spell_shield( gsn_shield, SPELL_LEVEL( ch, ch->level ), ch,
			  victim, USAGE_SPECIAL );
	return TRUE;
    }

    return FALSE;
}


SPECJALNOSC( spec_cast_cleric )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( is_fighting( victim, ch )
	  && ( can_see( ch, victim ) || FIGHTING( ch ) == victim )
	  && number_bits( 2 ) == 0 )
	{
	    break;
	}
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "cause serious";  break;
	case  2: min_level =  7; spell = "earthquake";     break;
	case  3: min_level =  9; spell = "cause critical"; break;
	case  4: min_level = 10; spell = "dispel evil";    break;
	case  5: min_level = 12; spell = "curse";          break;
	case  6: /* min_level = 12; spell = "change sex";     break; */
	case  7: min_level = 13; spell = "flamestrike";    break;
	case  8:
	case  9: min_level = 14; spell = "mute";           break;
	case 10: min_level = 15; spell = "harm";           break;
	default: min_level = 16; spell = "dispel magic";   break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup_ang( spell ) ) < 0 )
	return FALSE;
    ( *skill_table[ sn ].spell_fun )( sn, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );

    return TRUE;
}


SPECJALNOSC( spec_cast_mage )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( is_fighting( victim, ch )
	  && ( can_see( ch, victim ) || FIGHTING( ch ) == victim )
	  && number_bits( 2 ) == 0 )
	{
	    break;
	}
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	    case  0: min_level =  0; spell = "blindness";      break;
	    case  1: min_level =  3; spell = "chill touch";    break;
	    case  2: min_level =  7; spell = "weaken";         break;
	    case  3: min_level =  8; spell = "teleport";       break;
	    case  4: min_level = 11; spell = "colour spray";   break;
	    case  5: /* min_level = 12; spell = "change sex";     break; */
	    case  6: min_level = 13; spell = "energy drain";   break;
	    case  7:
	    case  8:
	    case  9: min_level = 15; spell = "fireball";       break;
	    case 12: min_level = 16; spell = "polymorph other";break;
	    case 13: min_level = 16; spell = "polymorph other";break;
	    default: min_level = 20; spell = "acid blast";     break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup_ang( spell ) ) < 0 )
	return FALSE;
    ( *skill_table[ sn ].spell_fun )( sn, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );

    return TRUE;
}


SPECJALNOSC( spec_cast_undead )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( is_fighting( victim, ch )
	  && ( can_see( ch, victim ) || FIGHTING( ch ) == victim )
	  && number_bits( 2 ) == 0 )
	{
	    break;
	}
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	    case  0: min_level =  0; spell = "curse";          break;
	    case  1: min_level =  3; spell = "weaken";         break;
	    case  2: min_level =  6; spell = "chill touch";    break;
	    case  3: min_level =  9; spell = "blindness";      break;
	    case  4: min_level = 12; spell = "poison";         break;
	    case  5: min_level = 15; spell = "energy drain";   break;
	    case  6: min_level = 18; spell = "harm";           break;
	    case  7: min_level = 21; spell = "teleport";       break;
/*	    case  8:
	    case  9:
	    case 10: if ( ch->race == zr_wampir )
		 {
		     min_level = 24;
		     spell = "vampiric bite";  break;
		 }
*/
	default: min_level = 24; spell = "gate";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup_ang( spell ) ) < 0 )
	return FALSE;
    ( *skill_table[ sn ].spell_fun )( sn, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );

    return TRUE;
}


SPECJALNOSC( spec_executioner )
{
    CHAR_DATA *victim;
    bool       fVampire = FALSE;
    char      *crime;
    char       buf [ MAX_STRING_LENGTH ];

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    crime = "";
    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->deleted
	  || !can_see( ch, victim )
	  || victim->level > LEVEL_IMMORTAL
	  || is_safe( ch, victim, FALSE ) )
	    continue;

	if ( !IS_NPC( victim ) && CZY_WAMPIR( victim )
	 && ( STREFA( victim ) == STREFA( ch ) )
	 && ( STREFA( victim ) != znajdz_strefe( "`swiat mroku" ) )
	 && ( !is_affected( victim, gsn_polymorph_other ) ) )
	    fVampire = TRUE;

	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	{
	    crime = "MORDERC`A";
	    break;
	}

	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_THIEF  ) )
	{
	    crime = "Z`LODZIEJEM";
	    break;
	}

	if ( fVampire )
	    break;
    }

    if ( !victim )
	return FALSE;

    if ( fVampire )
    {
	ZONE_DATA *strefa;
	sprintf( buf, "%s jest krwiopijc`a! PRECZ Z NASZEGO MIASTA!",
		PERS( victim, ch, FALSE ) );
	do_yell( ch, buf );
	if ( ( strefa = znajdz_strefe( "`swiat mroku" ) ) )
	    victim->pcdata->strefa = strefa;
    }

    if ( crime[ 0 ] != '\0' )
    {
	sprintf( buf, "%s jest %s! CHRONI`C NIEWINNYCH! KRWI!",
		PERS( victim, ch, FALSE ), crime );
	do_yell( ch, buf );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
		     ch->in_room );
	char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
		     ch->in_room );
    }

    return TRUE;
}


SPECJALNOSC( spec_fido )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *corpse;
    OBJ_DATA *corpse_next;

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    for ( corpse = ch->in_room->contents; corpse; corpse = corpse_next )
    {
	corpse_next = corpse->next_content;

	if ( corpse->deleted
	  || corpse->item_type != ITEM_CORPSE_NPC )
	    continue;

	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n dziko rozszarpuje zw`loki.", ch, corpse, NULL, TO_ROOM );
	for ( obj = corpse->contains; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
		continue;
	    obj_from_obj( obj );
	    obj_to_room( obj, ch->in_room );
	}
	extract_obj( corpse );
	return TRUE;
    }

    return FALSE;
}


SPECJALNOSC( spec_guard )
{
    CHAR_DATA *victim;
    CHAR_DATA *ech;
    CHAR_DATA *pijak;
    char      *crime;
    char       buf [ MAX_STRING_LENGTH ];
    int        max_evil;

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    max_evil = 300;
    ech      = NULL;
    pijak    = NULL;
    crime    = "";

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->deleted
	  || !can_see( ch, victim )
	  || victim->level > LEVEL_HERO
	  || is_safe( ch, victim, FALSE ) )
	    continue;

	if ( victim->spec_fun
	  && ( victim->spec_fun == spec_guard
	    || victim->spec_fun == spec_executioner
	    || victim->spec_fun == spec_mayor ) )
	    continue;

	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	{
	     crime = "MORDERC`A";
	     break;
	}

	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_THIEF  ) )
	{
	    crime = "Z`LODZIEJEM";
	    break;
	}

	if ( !IS_NPC( victim )
	   && victim->pcdata->condition[ COND_DRUNK ] > 3
	   && get_age( victim ) == 17
	   /* Lam 1.10.2002: Ulryk zmienil szanse z 15/16 na jeszcze wieksze,
	      teraz jest 1/16, moze wystarczy? */
	   && !number_bits( 4 ) )
	{
	    int pcond = pijak ? pijak->pcdata->condition[ COND_DRUNK ] : 0;
	    int vcond = victim->pcdata->condition[ COND_DRUNK ];

	    if ( !pijak || pcond < vcond )
	    {
		pijak = victim;
	    }
	}

	if ( victim->fighting
	  /* Lam 28.6.2009: #2526 */
	  && !IS_SET( race_table[ victim->fighting->vch->race ].race_abilities, RACE_MUTE )
	  && !is_fighting( victim, ch )
	  && victim->alignment < max_evil )
	{
	    max_evil = victim->alignment;
	    ech      = victim;
	}
    }

    if ( victim )
    {
	sprintf( buf, "%s jest %s! CHRONI`C NIEWINNYCH! HUZIA!",
		victim->name, crime );
	do_yell( ch, buf );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return TRUE;
    }

    if ( ech )
    {
	if ( !IS_MUTE( ch )
	  && !IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	    act( ZM_SLUCH, "$n drze si`e: \"CHRONI`C NIEWINNYCH! HUZIA!\"",
		ch, NULL, NULL, TO_ROOM );
	multi_hit( ch, ech, TYPE_UNDEFINED );
	return TRUE;
    }

    if ( pijak )
    {
	sprintf( buf, "%s ty %s! Co na to twoi rodzice?!",
		pijak->name,
		pijak->sex == 2 ? "pijaczko" : "pijaku" );
	do_yell( ch, buf );
	return TRUE;
    }

    return FALSE;
}


SPECJALNOSC( spec_janitor )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    for ( trash = ch->in_room->contents; trash; trash = trash_next )
    {
	trash_next = trash->next_content;

	if ( trash->deleted
	  || !IS_SET( trash->wear_flags, ITEM_TAKE ) )
	    continue;

	if ( trash->item_type == ITEM_DRINK_CON
	  || trash->item_type == ITEM_TRASH
	  || trash->cost < 10 )
	{
	    if ( ch->carry_number + get_obj_number( trash ) > can_carry_n( ch )
	      || ch->carry_weight + get_obj_weight( trash ) > can_carry_w( ch ) )
		return FALSE;

	    act( ZM_WZROK | ZM_WID_CHAR, "$n zmiata jakie`s `smieci.", ch, NULL, NULL, TO_ROOM );
	    obj_from_room( trash );
	    obj_to_char( trash, ch );
	    return TRUE;
	}
    }

    return FALSE;
}


SPECJALNOSC( spec_mayor )
{
    ROOM_INDEX_DATA   *prev_room = ch->in_room;
    CHAR_DATA         *vch;
    OBJ_DATA          *obj;
    static       int   pos;
    static       bool  move;
    static       bool  open;
		 int   kier;
		 bool  sekretarka = FALSE;
		 bool  mieszkaniec = FALSE;
		 int   przedmioty = 0;

#define ROOM_OFFICE    ch->reset_room
#define ROOM_GATE1     3041
#define ROOM_GATE2     3040

    if ( ch->fighting )
	return spec_cast_cleric( ch );
    if ( ch->position < POS_SLEEPING )
	return FALSE;

    if ( !move )
    {
	if ( ch->in_room->area->time_info.hour ==  6 )
	{
	    open = TRUE;
	    move = TRUE;
	    pos  = 0;
	    ch->position = POS_STANDING;
	    act( ZM_WZROK | ZM_WID_CHAR, "$n budzi si`e i g`lo`sno ziewa.", ch, NULL, NULL, TO_ROOM );
	    act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz g`lo`sne ziewni`ecie.", ch, NULL, NULL, TO_ROOM );
	}

	if ( ch->in_room->area->time_info.hour == 20 )
	{
	    open = FALSE;
	    move = TRUE;
	    pos  = 0;
	    ch->position = POS_STANDING;
	    act( ZM_WZROK | ZM_WID_CHAR, "$n budzi si`e i g`lo`sno ziewa.", ch, NULL, NULL, TO_ROOM );
	    act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz g`lo`sne ziewni`ecie.", ch, NULL, NULL, TO_ROOM );
	}
    }

    if ( !move )
	return FALSE;

    switch ( pos )
    {
    case 0:
	kier = find_path( ch->in_room, get_room_index( ROOM_GATE1 ),
			40000, FALSE, TRUE );
	break;
    case 1:
	kier = find_path( ch->in_room, get_room_index( ROOM_GATE2 ),
			40000, FALSE, TRUE );
	break;
    default:
	pos = 2;
	kier = find_path( ch->in_room, ROOM_OFFICE,
			40000, FALSE, TRUE );
	break;
    }

    if ( kier >= 0 && kier <= 9 )
    {
	move_char( ch, kier, FALSE );
	if ( ch->deleted )
	    return FALSE;
    }

    if ( prev_room == ch->in_room )
    {
	if ( ch->in_room == ROOM_OFFICE )
	    return FALSE;

	wiznet( "[BZDET] $N nie mo`ze doj`s`c do celu, tworzy portal.", ch, NULL, NULL, WIZ_DEBUG, 0, 106 );
	STC( "Modlisz si`e o powr`ot do swego biura...\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wznosi oczy ku niebu.", ch, NULL, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR, "Nagle powietrze zaczyna wirowa`c wok`o`l $1, kt`or$y zdaje si`e w nim rozp`lywa`c...", ch, NULL, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR, "$n znika zupe`lnie.", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, ch->reset_room );
	act( ZM_WZROK | ZM_WID_CHAR, "Nagle powietrze zaczyna wirowa`c, powoli ukazuj`ac posta`c $1!", ch, NULL, NULL, TO_ROOM );
    }

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( vch->deleted
	  || ( IS_NPC( vch ) && vch->pIndexData->vnum == MOB_VNUM_MIDGAARD_MAYOR )
	  || !can_see( ch, vch ) )
	    continue;

	if ( IS_NPC( vch ) && vch->pIndexData->vnum == MOB_VNUM_SECRETARY )
	    sekretarka = TRUE;
	else if ( POCHODZENIE_M( vch ) == POCHODZENIE_M( ch ) && !number_bits( 2 ) )
	    mieszkaniec = TRUE;
    }

    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	 przedmioty++;

    if ( sekretarka )
	do_say( ch, "Witaj, kochanie!" );
    else if ( przedmioty > 10 )
	do_say( ch, "Co za widok! Musz`e co`s zrobi`c z tym `smietnikiem!" );
    else if ( mieszkaniec )
    {
	if ( ch->in_room->area->time_info.hour >= 6
	  && ch->in_room->area->time_info.hour < 20 )
	    do_say( ch, "Dzie`n dobry, mieszka`ncy!" );
	else
	    do_say( ch, "Dobry wiecz`or, mieszka`ncy!" );
    }

    if ( ( pos == 0 && ch->in_room->vnum == ROOM_GATE1 )
      || ( pos == 1 && ch->in_room->vnum == ROOM_GATE2 ) )
    {
	if ( open )
	{
	    do_unlock( ch, "brama" );
	    do_open  ( ch, "brama" );
	    do_say( ch, "Niniejszym og`laszam miasto Midgaard otwartym!" );
	}
	else
	{
	    do_close ( ch, "brama" );
	    do_lock  ( ch, "brama" );
	    do_say( ch, "Niniejszym og`laszam miasto Midgaard zamkni`etym!" );
	}
	pos++;
	return FALSE;
    }

    if ( pos == 2 && ch->in_room == ROOM_OFFICE )
    {
	ch->position = POS_SLEEPING;
	act( ZM_WZROK | ZM_WID_CHAR, "$n k`ladzie si`e i zapada w sen.", ch, NULL, NULL, TO_ROOM );
	move = FALSE;
	return FALSE;
    }

    return FALSE;
}


SPECJALNOSC( spec_poison )
{
    CHAR_DATA *victim;

    if ( ch->position != POS_FIGHTING
      || !( victim = FIGHTING( ch ) )
      || number_percent( ) > 2 * ch->level )
	return FALSE;

    /* vampiric_bite jest tu tylko dlatego, ze ma atak typu ugryzienie */
    damage( ch, victim, 10 + ch->level / 10, gsn_vampiric_bite, TYPE_UNDEFINED, FALSE );
    spell_poison( gsn_poison, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );

    return TRUE;
}


SPECJALNOSC( spec_thief )
{
    CHAR_DATA *victim;
    int        gold;

    if ( ch->position != POS_STANDING )
	return FALSE;

    for ( victim = ch->in_room->people; victim;
	 victim = victim->next_in_room )
    {
	if ( IS_NPC( victim )
	    || victim->level >= LEVEL_IMMORTAL
	    || number_bits( 3 ) != 0
	    || !can_see( ch, victim ) )	/* Thx Glop */
	    continue;

	/* Lam 14.5.2000: ponizszy if */
	if ( victim->gold < 1
	  || is_safe( ch, victim, FALSE ) )
	    continue;

	/* Thanks to Zeke from MudX for pointing the percent bug */
	if ( IS_AWAKE( victim ) && victim->level > 5
	    && number_percent( ) + victim->level - ch->level >= 33 )
	{
	    /* Lam 26.8.98: sprawdzenie czy ma rece. Powinien byc jakis
	       komunikat zastepczy, zeby gracze wiedzieli, ze sa okradani,
	       niezaleznie od rasy zlodzieja. Chwilowo nie ma niczego, ale to
	       jest lepsze od rak kamieni i wirkow w moim portfelu */
	    if ( IS_SET( race_table[ ch->race ].parts, P_RECE ) )
	    {
		act( ZM_WZROK | ZM_WID_CHAR, "Odkrywasz r`ece $1 w swoim portfelu!",
			ch, NULL, victim, TO_VICT );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$N odkrywa r`ece $1 w swoim portfelu!",
			ch, NULL, victim, TO_NOTVICT );
	    }
	    return TRUE;
	}
	else
	{
	    gold = victim->gold * number_range( 1, 20 ) / 100;
	    ch->gold     += gold;
	    victim->gold -= gold;
	    return TRUE;
	}
    }

    return FALSE;
}


/*
 * Psionicist spec_fun by Thelonius for EnvyMud.
 */
SPECJALNOSC( spec_cast_psionicist )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    if ( ch->position != POS_FIGHTING
      || IS_AFFECTED( ch, AFF_BLOKADA_MENTALNA ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( is_fighting( victim, ch )
	  && ( can_see( ch, victim ) || FIGHTING( ch ) == victim )
	  && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	    case  0: min_level =  0; spell = "mind thrust";          break;
	    case  1: min_level =  4; spell = "psychic drain";        break;
	    case  2: min_level =  6; spell = "agitation";            break;
	    case  3: min_level =  8; spell = "psychic crush";        break;
	    case  4: min_level =  9; spell = "project force";        break;
	    case  5: min_level = 13; spell = "ego whip";             break;
	    case  6: min_level = 14; spell = "energy drain";         break;
	    case  7:
	    case  8: min_level = 17; spell = "psionic blast";        break;
	    case  9: min_level = 20; spell = "detonate";             break;
	    case 10: min_level = 27; spell = "disintegrate";         break;
	    default: min_level = 25; spell = "ultrablast";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup_ang( spell ) ) < 0 )
	return FALSE;
    ( *skill_table[ sn ].spell_fun )( sn, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );

    return TRUE;
}


SPECJALNOSC( spec_cast_ghost )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    /* teraz duchy nie gina w podziemiach, zauwazone przez Ulryka */
    if ( ch->in_room->area->weather_info.sunlight != SUN_DARK
      && !IS_SET( ch->in_room->room_flags, ROOM_UNDERGROUND ) )
    {
	if ( !ch->in_room )
	{
	    bug( "Spec_cast_ghost: NULL in_room.", 0 );
	    return FALSE;
	}

	if ( ch->fighting )
	    stop_fighting( ch, TRUE );

	act( ZM_WZROK | ZM_WID_CHAR, "Promie`n s`loneczny pada na $1, niszcz`ac $v.",
	    ch, NULL, NULL, TO_ROOM);

	extract_char( ch, TRUE );
	return TRUE;
    }

    if ( ch->position != POS_FIGHTING
      || IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
	if ( !victim->deleted
	  && is_fighting( victim, ch )
	  && ( can_see( ch, victim ) || FIGHTING( ch ) == victim )
	  && number_bits( 2 ) == 0 )
	{
	    break;
	}

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	    case  0: min_level =  0; spell = "curse";          break;
	    case  1: min_level =  3; spell = "weaken";         break;
	    case  2: min_level =  6; spell = "chill touch";    break;
	    case  3: min_level =  9; spell = "blindness";      break;
	    case  4: min_level = 12; spell = "poison";         break;
	    case  5: min_level = 15; spell = "energy drain";   break;
	    case  6: min_level = 18; spell = "harm";           break;
	    case  7: min_level = 21; spell = "teleport";       break;
	    default: min_level = 24; spell = "gate";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup_ang( spell ) ) < 0 )
	return FALSE;
    ( *skill_table[ sn ].spell_fun )( sn, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );

    return TRUE;
}


/*
 * spec_fun to repair bashed doors by Thelonius for EnvyMud.
 */
SPECJALNOSC( spec_repairman )
{
    EXIT_DATA       *pexit;
    EXIT_DATA       *pexit_rev;
    ROOM_INDEX_DATA *to_room;
    int              door;

    if ( !IS_AWAKE( ch ) )
	return FALSE;

    door = number_door( ) /* range( 0, 5 ) */;
    /*
     *  Could search through all doors randomly, but deathtraps would
     *  freeze the game!  And I'd prefer not to go through from 1 to 6...
     *  too boring.  Instead, just check one direction at a time.  There's
     *  a 51% chance they'll find the door within 4 tries anyway.
     *  -- Thelonius (Monk)
     */
    if ( !( pexit = ch->in_room->exit[ door ] ) )
	return FALSE;

    if ( IS_SET( pexit->exit_info, EX_BASHED ) )
    {
	REMOVE_BIT( pexit->exit_info, EX_BASHED );
	act( ZM_ZAWSZE, "Naprawiasz $D.", ch, NULL, pexit->biernik, TO_CHAR );
	act( ZM_WZROK, "$n naprawia $D.", ch, NULL, pexit->biernik, TO_ROOM );

	/* Don't forget the other side! */
	if ( ( to_room = pexit->to_room )
	    && ( pexit_rev = to_room->exit[ kierunki[ door ].odwrotny ] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_BASHED );

	    for ( rch = to_room->people; rch; rch = rch->next_in_room )
		switch ( pexit->rodzaj )
		{
		    default: act( ZM_WZROK, "$D zostaj`a wstawione na zawiasy.",
				rch, NULL, pexit_rev->short_descr, TO_CHAR );
			     break;

		    case 0:  act( ZM_WZROK, "$D zostaje wstawione na zawiasy.",
				rch, NULL, pexit_rev->short_descr, TO_CHAR );
			     break;

		    case 1:  act( ZM_WZROK, "$D zostaje wstawiony na zawiasy.",
				rch, NULL, pexit_rev->short_descr, TO_CHAR );
			     break;

		    case 2:  act( ZM_WZROK, "$D zostaje wstawiona na zawiasy.",
				rch, NULL, pexit_rev->short_descr, TO_CHAR );
			     break;
		}
	}

	return TRUE;
    }

    return FALSE;
}


/*
 * To dla uzdrowiciela na oltarzu, zamiast spec_cast_adept
 * Funkcja nie losuje ofiar, lecz szuka potrzebujacych, dzieki czemu wystarczy
 * jeden uzdrawiacz na wielu ludzi, bo blogoslawienstwa i taktyki bitewne daje
 * potrzebujacym, wiec nie trzeba czekac.
 */
SPECJALNOSC( spec_healer )
{
    CHAR_DATA *victim;

    if ( !IS_AWAKE( ch )
      || ch->fighting
      || IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( IS_NPC( victim )
	  /* Lam 12.1.2003: mordercy nie beda mieli lekko: */
	  || ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	  || STREFA( ch ) != STREFA( victim ) )
	    continue;

	if ( victim != ch && can_see( ch, victim )
	  && !is_affected( victim, gsn_bless )
	  && victim->level < 101 )
	    break;
    }

    if ( victim )
    {
	say_spell( ch, gsn_bless );
	spell_bless( gsn_bless, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;
    }

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( IS_NPC( victim )
	  /* Lam 12.1.2003: mordercy nie beda mieli lekko: */
	  || ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	  || STREFA( ch ) != STREFA( victim ) )
	    continue;

	if ( victim != ch && can_see( ch, victim )
	  && !is_affected( victim, gsn_combat_mind )
	  && victim->level < 101 )
	    break;
    }

    if ( victim )
    {
	say_spell( ch, gsn_combat_mind );
	spell_combat_mind( gsn_combat_mind, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;
    }

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( IS_NPC( victim )
	  /* Lam 12.1.2003: mordercy nie beda mieli lekko: */
	  || ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	  || STREFA( ch ) != STREFA( victim ) )
	    continue;

	if ( victim != ch && can_see( ch, victim )
	  && !is_affected( victim, gsn_armor )
	  && victim->level < 101 )
	    break;
    }

    if ( victim )
    {
	say_spell( ch, gsn_armor );
	spell_armor( gsn_armor, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;
    }

/*** standardowe losowe leczenie ***/
    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( IS_NPC( victim )
	  /* Lam 12.1.2003: mordercy nie beda mieli lekko: */
	  || ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	  || STREFA( ch ) != STREFA( victim ) )
	    continue;

	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
	  && victim->level < 101 )
	    break;
    }

    if ( !victim )
	return FALSE;

    switch ( number_bits( 2 ) )
    {
    case 0:
    case 3:
	if ( victim->hit < victim->max_hit )
	{
	    say_spell( ch, gsn_cure_light );
	    spell_cure_light( gsn_cure_light,
			SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	}
	return TRUE;

    case 1:
	if ( victim->mana < victim->max_mana )
	{
	    say_spell( ch, gsn_mana_gift );
	    spell_mana_gift( gsn_mana_gift,
			SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	}
	return TRUE;

    case 2:
	if ( victim->move < victim->max_move )
	{
	    say_spell( ch, gsn_refresh );
	    spell_refresh( gsn_refresh, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	}
	return TRUE;
    }

    return FALSE;
}


/*
 * Qwert 12.11.2000
 */
SPECJALNOSC( spec_waker )
{
    CHAR_DATA   *victim;
    char	*tekst = NULL;
    bool 	czyspi = FALSE;

    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	return FALSE;

    if ( !IS_AWAKE( ch) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( IS_AWAKE( victim )
	  || IS_AFFECTED( victim, AFF_SLEEP )
	  || number_bits( 1 ) )
	    continue;

	czyspi = TRUE;
	do_wake( victim, "" );
    }

    if ( !czyspi )
	return FALSE;

    switch ( number_range( 1, 8 ) )
    {
	default: tekst = "Wstawa`c!";					break;
	case  1: tekst = "Wstawa`c! Nadchodzi koniec `swiata!";		break;
	case  2: tekst = "Wstawa`c! Rozdaj`a darmowe piwo!";		break;
	case  3: tekst = "Wstawa`c! To nie sypialnia do cholery!";	break;
	case  4: tekst = "Wstawa`c! Pali si`e!";			break;
	case  5: tekst = "Wstawa`c! Atakuje nas stado dzikich kr`ow!";	break;
	case  6: tekst = "Pobudka!";					break;
	case  7: tekst = "Otwiera`c oczy `spiochy!";			break;
	case  8: tekst = "Pobudka! Do boju! Przeciwnik nie `spi!";	break;
    }

    /* Lam: do_say, do jednego pomieszczenia, poza tym nie musimy sie bawic
	    w sprawdzanie czy postac moze mowic (rasa, uciszenie...) */
    do_say( ch, tekst );

    return TRUE;
}


/*
 * Ulryk 03.03.2002
 */
SPECJALNOSC( spec_unicorn )
{
    CHAR_DATA *victim;
    int        sn;

    victim = FIGHTING( ch );

    if ( !ch->fighting
      || victim->deleted
      || ch->race != zr_jednorozec )
	return FALSE;

    /* jesli ma ponizej 10% ¿ycia, to losuje czy bedzie oslepial, czy
       teleportowal */
    if ( ( ch->max_hit > ch->hit * 10 ) && number_bits( 1 ) )
	sn = gsn_teleport;
    else
	sn = gsn_blindness;

    if ( ( ch == victim ) || ( number_percent( ) > ch->level / 2 )
	|| ( sn == gsn_blindness && IS_AFFECTED( victim, AFF_BLIND ) ) )
    {
	return FALSE;
    }

    act( ZM_WZROK | ZM_WID_CHAR, "R`og $1 rozb`lyska jasnym `swiat`lem!", ch, NULL, NULL, TO_ROOM );
    ( *skill_table[ sn ].spell_fun )( sn, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );

    return TRUE;
}


/*
 * Ulryk 26.02.2003
 */
char *last_word( char *argument )
{
    char buf[ MAX_STRING_LENGTH ];
    char *arg = NULL;

    buf[ 0 ] = '\0';

    if ( !argument )
	return NULL;

    while ( *argument != '\0' )
    {
	arg = argument;
	argument = one_argument( argument, buf );
    }

    return arg;
}


char *przeciagaj( char *argument )
{
    char buf[ MAX_STRING_LENGTH ];
    int  pos = 0;

    if ( !argument )
	return NULL;

    do {
	if ( UPPER( *argument ) == 'R' && UPPER( *( argument + 1 ) ) != 'Z' )
	{
	    strcpy( &buf[ pos ], "rrr" );
	    pos += 3;
	}
	else
	    buf[ pos++ ] = *argument;
    }
    while ( *argument++ );
    buf[ pos ] = '\0';
    strcpy( argument, buf );

    return argument;
}


bool papuguj( CHAR_DATA *ch, char *argument )
{
    char buf[ 2 * MAX_INPUT_LENGTH ];

    if ( !IS_AWAKE( ch ) )
	return FALSE;

    buf[ 0 ] = '\0';

    if ( argument )
    {
	char *arg = last_word( argument );
	if ( !arg || ( UPPER( *arg ) < 'A' || UPPER( *arg ) > 'Z' ) )
	    return FALSE;
	arg = przeciagaj( arg );

	sprintf( buf, "%s! %s!", arg, arg );
    }
    else
    {
	switch ( number_bits( 3 ) )
	{
	    case 0:
	    case 1:
	    case 2: strcat( buf, "Rrrrwa ma`c!" );                    break;
	    case 3:
	    case 4: strcat( buf, "Daaaj orzeszka! Daaaj orzeszka!" ); break;
	    case 5: strcat( buf, "Brrrawo! Brrawo!" );                break;
	    case 6: strcat( buf, "Rrrratunku! Ucieka`c!" );           break;
	    default:strcat( buf, "Grrrrratuluj`e!" );
	}
    }

    do_say( ch, buf );

    return TRUE;
}


SPECJALNOSC( spec_parrot )
{
    if ( !number_bits( 2 ) )
	return papuguj( ch, NULL );

    return FALSE;
}


/* Ulryk 20.11.2003 na bazie spec_healer */
SPECJALNOSC( spec_vampire_elder )
{
    CHAR_DATA *victim;

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( !IS_NPC( victim ) && !CZY_WAMPIR( victim ) )
	    continue;
	if ( victim != ch && can_see( ch, victim )
	  && !is_affected( victim, gsn_combat_mind )
	  && victim->level < 101 )
	    break;
    }

    if ( victim && !IS_NPC( victim ) && victim->level < 101 )
    {
	say_spell( ch, gsn_combat_mind );
	spell_combat_mind( gsn_combat_mind, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;
    }

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	/* Lam 12.1.2003 */
	if ( !IS_NPC( victim ) && !CZY_WAMPIR( victim ) )
	    continue;
	if ( victim != ch && can_see( ch, victim )
	  && !is_affected( victim, gsn_armor )
	  && victim->level < 101 )
	    break;
    }

    if ( victim && !IS_NPC( victim ) && victim->level < 101 )
    {
	say_spell( ch, gsn_armor );
	spell_armor( gsn_armor, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;
    }

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( !IS_NPC( victim ) && !CZY_WAMPIR( victim ) )
	    continue;
	if ( victim != ch && can_see( ch, victim )
	  && !is_affected( victim, gsn_shield )
	  && victim->level < 101 )
	    break;
    }

    if ( victim && !IS_NPC( victim ) && victim->level < 101 )
    {
	say_spell( ch, gsn_shield );
	spell_shield( gsn_shield, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	return TRUE;
    }

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	/* Lam 12.1.2003 */
	if ( !IS_NPC( victim ) && !CZY_WAMPIR( victim ) )
	    continue;
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
	  && victim->level < 101 )
	    break;
    }

    if ( !victim || victim->level > 100 || IS_NPC( victim ) )
	return FALSE;

    switch ( number_bits( 2 ) )
    {
    case 0:
    case 3:
	if ( victim->hit < victim->max_hit )
	{
	    say_spell( ch, gsn_cure_light );
	    spell_cure_light( gsn_cure_light,
			SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	}
	return TRUE;

    case 1:
	if ( victim->mana < victim->max_mana )
	{
	    say_spell( ch, gsn_mana_gift );
	    spell_mana_gift( gsn_mana_gift,
			SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	}
	return TRUE;

    case 2:
	if ( victim->move < victim->max_move )
	{
	    say_spell( ch, gsn_refresh );
	    spell_refresh( gsn_refresh, SPELL_LEVEL( ch, ch->level ), ch, victim, USAGE_SPECIAL );
	}
	return TRUE;
    }

    return FALSE;
}
