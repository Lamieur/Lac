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
#include "magic.h"
#include "handler.h"


void	stworz_jedzenie		args( ( CHAR_DATA *ch, int level, int vnum ) );
void	wlasciwa_teleportacja	args( ( CHAR_DATA *caster, CHAR_DATA *victim,
					ROOM_INDEX_DATA *pRoomIndex ) );



/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
    CHAR_DATA *rch;
    char      *pName;
    char       buf[ MAX_STRING_LENGTH ];
    char       buf2[ MAX_STRING_LENGTH ];
    int        iSyl;
    int        length;
    bool       czy;

    struct syl_type
    {
	char *old;
	char *new;
    };

    static const struct syl_type   syl_table[ ] =
    {
	{ " ",		" "		},
	{ "ar",		"abra"		},
	{ "au",		"kada"		},
	{ "blo",	"fido"		},
	{ "bur",	"mosa"		},
	{ "cu",		"judi"		},
	{ "cz",		"pum"		},
	{ "de",		"oculo"		},
	{ "dz",		"`z"		},
	{ "en",		"unso"		},
	{ "hi",		"lo"		},
	{ "kry",	"nofra"		},
	{ "kr",		"c`ow"		},
	{ "ku",		"ra"		},
	{ "lo",		"hi"		},
	{ "mor",	"zak"		},
	{ "nie",	"illa"		},
	{ "no`s`c",	"lacri"		},
	{ "os",		"nose"		},
	{ "per",	"duda"		},
	{ "pro",	"`lak"		},
	{ "ra",		"gru"		},
	{ "re",		"candus"	},
	{ "ru",		"sido"		},
	{ "rz",		"nofo"		},
	{ "slo",	"buh"		},
	{ "son",	"sabru"		},
	{ "swia",	"dies"		},
	{ "sz",		"ton"		},
	{ "tr",		"cula"		},
	{ "a", "a" },	{ "`a", "my" },	{ "b", "b" },	{ "c", "n" },
	{ "`c", "ni" },	{ "d", "e" },	{ "e", "z" },	{ "`e", "om" },
	{ "f", "rt" },	{ "g", "o" },	{ "h", "p" },	{ "i", "u" },
	{ "j", "d" },	{ "k", "t" },	{ "l", "r" },	{ "`l", "bu" },
	{ "m", "w" },	{ "n", "i" },	{ "`n", "s" },	{ "o", "a" },
	{ "`o", "w" },	{ "p", "s" },	{ "q", "d" },	{ "r", "f" },
	{ "s", "g" },	{ "`s", "j" },	{ "t", "h" },	{ "u", "j" },
	{ "v", "z" },	{ "w", "h" },	{ "x", "n" },	{ "y", "l" },
	{ "z", "k" },	{ "`x", "t" },	{ "`z", "dz" },
	{ "", "" }
    };

    /* Lam 17.5.2004: psionicy nie musza wypowiadac slow */
    if ( ch->klasa == CLASS_PSIONIK )
    {
	/* Lam: jesli maja glowe (nie leb), pewnie maja i czolo */
	if ( IS_SET( race_table[ ch->race ].parts, P_GLOWA ) )
	    act( ZM_WZROK | ZM_WID_CHAR, "$n marszczy czo`lo.", ch, NULL, NULL, TO_ROOM );

	return;
    }

    buf[ 0 ] = '\0';
    for ( pName = skill_table[ sn ].pl_name; *pName != '\0'; pName += length )
    {
	for ( iSyl = 0;
	      ( length = strlen( syl_table[ iSyl ].old ) ) != 0;
	      iSyl++ )
	{
	    /* Lam: str_prefix obsluguje polskie litery we wszystkie strony,
	       zmieniam na strncmp */
	    if ( !strncmp( syl_table[ iSyl ].old, pName, length ) )
	    {
		strcat( buf, syl_table[ iSyl ].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    if ( ch->klasa == CLASS_MNICH )
    {
	sprintf( buf2, "$n `spiewa: \"%s.\"", buf );
	sprintf( buf,  "$n `spiewa: \"%s.\"", skill_table[ sn ].pl_name );
    }
    else if ( ch->klasa == CLASS_SZAMAN )
    {
	sprintf( buf2, "$n szepcze: \"%s.\"", buf );
	sprintf( buf,  "$n szepcze: \"%s.\"", skill_table[ sn ].pl_name );
    }
    else if ( strchr( buf, ' ' ) )
    {
	sprintf( buf2, "$n wypowiada s`lowa: \"%s.\"", buf );
	sprintf( buf,  "$n wypowiada s`lowa: \"%s.\"", skill_table[ sn ].pl_name );
    }
    else
    {
	sprintf( buf2, "$n wypowiada s`lowo: \"%s.\"", buf );
	sprintf( buf,  "$n wypowiada s`lowo: \"%s.\"", skill_table[ sn ].pl_name );
    }

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	if ( rch != ch )
	{
	    czy = !IS_NPC( ch )
	       && ch->klasa == rch->klasa
	       && class_table[ ch->klasa ].fMana;
	    act( ZM_SLUCH, czy ? buf : buf2, ch, NULL, rch, TO_VICT );
	    act( ZM_W_SLUCH | ZM_WZROK | ZM_WID_CHAR, "$n porusza ustami wykonuj`ac dziwne gesty.", ch, NULL, rch, TO_VICT );
	}

    return;
}


/*
 * Wydzielone z do_cast dla celow czarow z flaga SKILL_NO_COST.
 *
 * Uwaga! Czary SKILL_NO_COST maja uzywac makra RZUCENIE_CZARU( ), ktore
 * sprawdza, czy usage == USAGE_CAST, a takze wypowiada czar i odbiera mane.
 * Funkcja opoznienie_czaru( ) nie jest przeznaczona do uzytku poza USAGE_CAST
 * lub bez rownoczesnego kosztu many i uzycia say_spell( ).
 *
 * Zobacz tez komentarz przy RZUCENIE_CZARU( ) w merc.h.
 */
void opoznienie_czaru( CHAR_DATA *ch, int sn )
{
    /* zaburzenie energii i przyspieszenie wylaczaja przyspieszajacy wplyw
       profesjonalizmu */
    if ( !IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_PRZYSPIESZENIE )
      && !IS_AFFECTED( ch, AFF_ZABURZENIE_ENERGII )
      && number_percent( ) < 2 * ch->pcdata->learned[ gsn_profesjonalizm ] / 3 )
    {
	WAIT_STATE( ch, number_range( skill_table[ sn ].beats / 2,
				      skill_table[ sn ].beats * 3 / 4 ) );
	uzyj( ch, gsn_profesjonalizm, "zaawansowanym rzucaniu czar`ow" );
    }
    else
    /* zaburzenie energii nie kumuluje sie ze spowolnieniem, ale jesli jest
       bez spowolnienia, to anuluje efekt przyspieszenia */
    if ( IS_AFFECTED( ch, AFF_PRZYSPIESZENIE )
      && !IS_AFFECTED( ch, AFF_SPOWOLNIENIE )
      && IS_AFFECTED( ch, AFF_ZABURZENIE_ENERGII ) )
    {
	WAIT_STATE( ch, 2 * skill_table[ sn ].beats );
    }
    else
	WAIT_STATE( ch, skill_table[ sn ].beats );

    return;
}


/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name = "";


/*
 * Jedna z najczesciej uzywanych w trakcie gry funkcji - do_cast.
 * Lam 1.3.98: teraz moby tez moga czarowac
 */
KOMENDA( do_cast )
{
    void      *vo;
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *room = ch->in_room;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    int        mana;
    int        sn;

    if ( !IS_NPC( ch ) && !class_table[ ch->klasa ].fMana && ch->level < L_APP )
    {
	send_to_char( "Nie znasz si`e na czarowaniu.\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	return; /* order nie, ale mobprog zadziala */

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' )
    {
	send_to_char( "Jaki czar rzuci`c na kogo lub co?\n\r", ch );
	return;
    }

	 /* Lam: zabezpieczenie przed reserved: */
    if ( ( sn = skill_lookup_pl( arg1 ) ) < 1
	|| !skill_table[ sn ].spell_fun /* to umiejetnosc, a nie czar */
	|| skill_table[ sn ].spell_fun == spell_null
	|| ( !IS_NPC( ch )
	  && ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
	  && ch->level < skill_table[ sn ].multi_level[ ch->klasa ]
	  && !ch->pcdata->learned[ sn ] ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch )
      && ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
      && !ma_w_multi( ch, sn )
      && !ch->pcdata->learned[ sn ] )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return;
    }

    if ( ch->position < skill_table[ sn ].minimum_position )
    {
	switch ( ch->position )
	{
	    default:
		send_to_char( "Nie mo`zesz skoncentrowa`c si`e wystarczaj`aco.\n\r", ch );
		break;
	    case POS_RESTING:
		ASTC( "Czujesz si`e zbyt odpr`e`zon$y, `zeby czarowa`c.", ch );
		break;
	    case POS_SLEEPING:
		send_to_char( "`Snisz o pot`e`znych zakl`eciach...\n\r", ch );
		break;
	    case POS_FIGHTING:
		ASTC( "Jeste`s na to zbyt zaj`et$y walk`a.", ch );
		break;
	    case POS_DEAD:
		send_to_char( "`Swiate`lko, kt`ore widzisz na ko`ncu tunelu, jest fajniejsze ni`z czary.\n\r", ch );
		break;
	    case POS_STUNNED:
		ASTC( "Chcia`l$aby`s czarowa`c, ale energia ucieka ci ranami wraz z krwi`a.", ch );
		break;
	    case POS_INCAP:
		send_to_char( "Zamiast pr`obowa`c rzuca`c zakl`ecia, przygotuj si`e na spotkanie z przeznaczeniem.\n\r", ch );
		break;
	}
	return;
    }

    if ( ch->klasa != CLASS_PSIONIK )
    {
	if ( IS_MUTE( ch ) )
	{
	    send_to_char( "Poruszasz ustami, ale nie mo`zesz niczego powiedzie`c.\n\r", ch );
	    return;
	}

	if ( IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	{
	    send_to_char( "Tutaj nie wolno niczego m`owi`c!\n\r", ch );
	    return;
	}
    }
    else if ( IS_AFFECTED( ch, AFF_BLOKADA_MENTALNA ) )
    {
	send_to_char( "Nie mo`zesz si`e wystarczaj`aco skupi`c.\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_zaglada ) )
    {
	act( ZM_ZAWSZE, "Jeste`s tak przera`zon$y, `ze o rzucaniu czar`ow nie ma mowy.", ch, NULL, NULL, TO_CHAR );
	return;
    }

    mana = MANA_COST( ch, sn );

    /*
     * Locate targets.
     */
    victim = NULL;
    vo     = NULL;

    switch ( skill_table[ sn ].target )
    {
    default:
	bug( "Do_cast: bad target for sn %d.", sn );
	return;

    case TAR_IGNORE:
	break;

    case TAR_CHAR_OFFENSIVE:
	if ( arg2[ 0 ] == '\0' )
	{
	    if ( !( victim = FIGHTING( ch ) ) )
	    {
		send_to_char( "Na kogo rzuci`c ten czar?\n\r", ch );
		return;
	    }
	}
	else
	{
	    pp_get( ch, arg2 );
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
		return;
	    }
	}

	if ( is_safe( ch, victim, TRUE ) )
	    return;

	/* Lam: w Envy moby nie moga czarowac, ale to bylo. Hmm... */
	if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
	{
	    send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	    return;
	}

	check_killer( ch, victim );

	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( arg2[ 0 ] == '\0' )
	{
	    if ( IS_SET( skill_table[ sn ].flags, SKILL_DEF_VICT )
	      && ( !IS_SET( skill_table[ sn ].flags, SKILL_PSI_SELF )
		|| ch->klasa != CLASS_PSIONIK ) )
		victim = FIGHTING( ch );
	    if ( !victim )
		victim = ch;
	}
	else
	{
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		if ( IS_SET( skill_table[ sn ].flags, SKILL_PSI_SELF )
		  && ch->klasa == CLASS_PSIONIK )
		    send_to_char( "Nie mo`zesz rzuca`c tego czaru na innych.\n\r", ch );
		else
		    send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
		return;
	    }

	    if ( IS_SET( skill_table[ sn ].flags, SKILL_PSI_SELF )
	      && victim != ch
	      && ch->klasa == CLASS_PSIONIK )
	    {
		send_to_char( "Nie mo`zesz rzuca`c tego czaru na innych.\n\r", ch );
		return;
	    }
	}

	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	if ( arg2[ 0 ] != '\0' && !is_name( arg2, ch->name ) )
	{
	    send_to_char( "Nie mo`zesz rzuca`c tego czaru na innych.\n\r", ch );
	    return;
	}

	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( arg2[ 0 ] == '\0' )
	{
	    send_to_char( "Na co ma by`c rzucony ten czar?\n\r", ch );
	    return;
	}

	if ( !( obj = get_obj_carry( ch, arg2 ) ) )
	{
	    send_to_char( "Nie masz tego przedmiotu.\n\r", ch );
	    return;
	}

	vo = (void *) obj;
	break;
    }

    if ( ch->mana < mana )
    {
	send_to_char( "Nie wystarczy ci mocy magicznych.\n\r", ch );
	return;
    }

    target_name = argument;

    if ( !IS_NPC( ch )
      && ( number_percent( ) > ch->pcdata->learned[ sn ]
	|| ( IS_AFFECTED( ch, AFF_DEZORIENTACJA )
	  && number_percent( ) > 80 ) ) )
    {
	ASTC( "Straci`l$o`s koncentracj`e.", ch );
	ch->mana -= mana / 2;
	opoznienie_czaru( ch, sn );
	if ( str_cmp( skill_table[ sn ].old_name, "ventriloquate" )
	  && str_cmp( skill_table[ sn ].old_name, "create sound" ) )
	    say_spell( ch, sn );
    }
    else
    {
	if ( !IS_SET( skill_table[ sn ].flags, SKILL_NO_COST ) )
	{
	    ch->mana -= mana;
	    opoznienie_czaru( ch, sn );
	    if ( str_cmp( skill_table[ sn ].old_name, "ventriloquate" )
	      && str_cmp( skill_table[ sn ].old_name, "create sound" ) )
		say_spell( ch, sn );
	}

	if ( ( *skill_table[ sn ].spell_fun )( sn,
				SPELL_LEVEL( ch, ch->level ), ch, vo,
				USAGE_CAST ) )
	{
	    char bufor[ MAX_STRING_LENGTH ];

	    sprintf( bufor, "rzucaniu czaru \"%s\"", skill_table[ sn ].pl_name );
	    uzyj( ch, sn, bufor ); /* tylko jesli czar sie powiodl */
	    if ( victim && victim->master != ch && victim != ch
	      && IS_AWAKE( victim )
	      && ch->in_room == room && victim->in_room == room
	      && !ch->deleted
	      && !victim->deleted )
		mprog_spell_victim_trigger( skill_table[ sn ].pl_name, victim, ch );
	}
    }

    target_name = "";

    if ( skill_table[ sn ].target == TAR_CHAR_OFFENSIVE
      && victim->master != ch && victim != ch && IS_AWAKE( victim )
      && ch->in_room == room
      && victim->in_room == room
      && !ch->deleted
      && !victim->deleted )
    {
	if ( !is_fighting( ch, victim ) )
	    set_fighting( ch, victim );

	if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	{
	    set_fighting( victim, ch );
	    /* Lam 20.1.2004: Ulryk wypatrzyl, ze multi_hit wyskakiwal takze
	       podczas walki, dajac dodatkowe ataki skierowane nie zawsze
	       w osobe, z ktora bezposrednio walczy victim, wiec przenosze to
	       tutaj */
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	}
    }

    return;
}


/*
 * Cast spells at targets using a magical object.
 */
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim,
		    OBJ_DATA *obj, char *arg, int usage )
{
    void *vo;
    ROOM_INDEX_DATA *room = ch->in_room;

    if ( !room
      || ch->deleted
      || ( victim
	&& ( victim->deleted
	  || room != victim->in_room ) ) )
    {
	bug( "obj_cast_spell: odmawiam dzialania. Napraw muda.", 0 );
	return;
    }

    if ( sn <= 0 )
	return;

    if ( sn >= MAX_SKILL || skill_table[ sn ].spell_fun == 0 )
    {
	bug( "Obj_cast_spell: bad sn %d.", sn );
	return;
    }
    target_name = "";

    switch ( skill_table[ sn ].target )
    {
    default:
	bug( "Obj_cast_spell: zly cel dla sn %d.", sn );
	return;

    case TAR_IGNORE:
	vo = NULL;
	target_name = arg;
	break;

    case TAR_CHAR_OFFENSIVE:
	if ( !victim )
	    victim = FIGHTING( ch );
	if ( !victim || obj )
	{
	    send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	    return;
	}

	if ( ch != victim )
	{
	    if ( is_safe( ch, victim, TRUE ) )
		return;
	    check_killer( ch, victim );
	}

	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( obj )
	{
	    send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	    return;
	}
	if ( !victim )
	    victim = ch;
	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	if ( obj || ( victim && victim != ch ) )
	{
	    send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	    return;
	}
	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( !obj )
	{
	    send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	    return;
	}
	vo = (void *) obj;
	break;
    }

    ( *skill_table[ sn ].spell_fun )( sn, level, ch, vo, usage );
    if ( victim && victim->master != ch && victim != ch
      && IS_AWAKE( victim )
      && ch->in_room == room && victim->in_room == room
      && !ch->deleted
      && !victim->deleted )
    {
       mprog_spell_victim_trigger( skill_table[ sn ].pl_name, victim, ch );
    }
    target_name = "";

    /* Lam 1.1.2003: po tym czarze ofiara moze oddac i nas zabic */
    if ( ch->deleted
      || ch->in_room != room
      || ( victim
        && ( victim->deleted || victim->in_room != room ) )
      || ( obj && obj->deleted ) )
    {
	return;
    }

    if ( skill_table[ sn ].target == TAR_CHAR_OFFENSIVE
	&& victim->master != ch && ch != victim && IS_AWAKE( victim ) )
    {
	/* Lam 25.4.2005: kopiuje z do_cast zabezpieczenie przed multi_hit
	   w trakcie walki (tu bylo nawet gorzej, bo ze zwoju mozna bylo
	   puscic 3 czary i otrzymac wiazanke 15 ciosow miedzy rundami walki */
	if ( !is_fighting( ch, victim ) )
	    set_fighting( ch, victim );

	if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	{
	    set_fighting( victim, ch );
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	}
    }

    return;
}


/*
 * Spell functions.
 */
CZAR( spell_acid_blast )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = dice( level, 8 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_armor )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = 24;
    af.location = APPLY_AC;
    af.modifier = 20;
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
	send_to_char( "Rzucasz ochron`e.\n\r", ch );

    act( ZM_PRZYT, "Czujesz, `ze kto`s ci`e chroni.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_black_aura )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_BLACK_AURA )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( victim->klasa == CLASS_MNICH
	&& skill_get_trust( victim ) < LEVEL_IMMORTAL )
    {
	STC( "Mnich nie mo`ze si`e chroni`c czarn`a aur`a.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = number_fuzzy( level / 4 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_BLACK_AURA );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_WZROK, "Otacza ci`e czarna aura.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "Czarna aura otacza $3.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_bless )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    bool polepszony = FALSE;
    bool wzmocniony = FALSE;

    if ( victim->position == POS_FIGHTING )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 6 + level;
    af.location = APPLY_HITROLL;
    af.modifier = IS_NPC( ch ) ? level / 10 : level / 5;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    af.location = APPLY_SAVING_SPELL;
    af.modifier = IS_NPC( ch ) ? level / 10 : level / 6;
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    if ( !polepszony )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocniony );

    if ( ch != victim )
	send_to_char( "Rzucasz b`logos`lawie`nstwo.\n\r", ch );

    act( ZM_PRZYT, "Czujesz si`e b`logos`lawion$y.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_blindness )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_BLIND )
      || IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_EYES ) )
    {
	if ( usage != USAGE_RECUR )
	    NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	if ( usage != USAGE_RECUR )
	    NIEUDANE( ch );
	return FALSE;
    }

    /* przed wplywem, zeby sprawdzic, czy ofiara widziala bez oslepienia */
    act( ZM_WZROK, "Niczego nie widzisz!", victim, NULL, NULL, TO_CHAR );

    af.type     = sn;
    af.duration = 2 + level / 8;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_BLIND );
    affect_to_char( victim, &af );

    /* tym sposobem jesli kogos oslepiam samemu bedac oslepionym, nie mam
       pojecia co sie stalo, dopiero kolejne oslepienie powie mi, ze juz
       oslepilem, o ile mam wykrycie magii */
    if ( ch != victim )
	act( ZM_WZROK | ZM_WID_VICT, "$N o`slep`l$A!", ch, NULL, victim, TO_CHAR );

    /* po czym mozna to poznac? Zalozylem, ze widac, jak przeciera oczy
       albo zaczyna chodzic po omacku */
    act( ZM_WZROK | ZM_WID_CHAR | ZM_DYLEMAT, "$N jest o`slepion$Y!", ch, NULL, victim, TO_NOTVICT );

    return TRUE;
}


SPADANIE( wear_off_blindness )
{
    if ( IS_AWAKE( ch ) && can_see_room( ch, ch->in_room ) )
    {
	act( ZM_WZROK, "Znowu mo`zesz widzie`c.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rozgl`ada si`e mrugaj`ac oczami.", ch, NULL, NULL, TO_ROOM );
    }

    return;
}


CZAR( spell_breathe_water )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_GILLS )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 24;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_GILLS );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_PRZYT, "Mo`zesz teraz oddycha`c pod wod`a.", victim, NULL, NULL, TO_CHAR );

    if ( ch != victim )
	act( ZM_ZAWSZE, "Umo`zliwiasz $2 oddychanie pod wod`a.", victim, NULL, ch, TO_VICT );

    return TRUE;
}


CZAR( spell_burning_hands )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const int        dam_each[ ] =
    {
	 0,
	 0,  0,  0,  0, 14,     17, 20, 23, 26, 29,
	29, 29, 30, 30, 31,     31, 32, 32, 33, 33,
	34, 34, 35, 35, 36,     36, 37, 37, 38, 38,
	39, 39, 40, 40, 41,     41, 42, 42, 43, 43,
	44, 44, 45, 45, 46,     46, 47, 47, 48, 48,
	49, 49, 50, 50, 51,	51, 52, 52, 53, 53,
	54, 54, 55, 55, 56,	56, 57, 57, 58, 58,
	59, 59, 60, 60, 61,	61, 62, 62, 63, 63,
	64, 64, 65, 65, 66,	66, 67, 67, 68, 68,
	69, 69, 70, 70, 71,	71, 72, 72, 73, 73
    };
		 int        dam;

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_call_lightning )
{
    CHAR_DATA *vch;
    int        dam;

    if ( !IS_OUTSIDE( ch ) )
    {
	send_to_char( "Nie w zamkni`etym pomieszczeniu.\n\r", ch );
	return FALSE;
    }

    if ( ch->in_room->area->weather_info.sky < SKY_RAINING )
    {
	send_to_char( "Potrzebujesz z`lej pogody.\n\r", ch );
	return FALSE;
    }

    dam = dice( level / 2, 8 );

    send_to_char( "B`lyskawica spada z nieba na twych wrog`ow!\n\r", ch );
    act( ZM_WZROK, "$n sprowadza na swych wrog`ow b`lyskawic`e!",
	ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
	if ( vch->deleted
	  || !vch->in_room
	  || is_safe( ch, vch, FALSE ) )
	{
	    continue;
	}


	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch
	      && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
	    {
		damage( ch, vch, saves_spell( level, vch ) ? dam / 2 : dam,
		       sn, WEAR_NONE, FALSE );
	    }

	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area
	  && IS_OUTSIDE( vch )
	  && IS_AWAKE( vch )
	  && can_see_room( vch, vch->in_room ) )
	{
	    send_to_char( "B`lyskawica przecina niebo!\n\r", vch );
	}
    }

    return TRUE;
}


CZAR( spell_cause_light )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    damage( ch, victim, dice( 1, 8 ) + level / 3, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_cause_critical )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    damage( ch, victim, dice( 3, 8 ) + level - 6, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_cause_serious )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    damage( ch, victim, dice( 2, 8 ) + level / 2, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_change_sex )
{
    /* Lam 20.11.2000: blokada tego czaru. Na zawsze. */
    STC( "Nic si`e nie wydarzy`lo.\n\r", ch );

    return FALSE;
    /* Lam 21.3.2006: poprzednia tresc funkcji byla zbedna */
}


CZAR( spell_charm_person )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo, *rch;
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
    {
	STC( "Mobom nie wolno oczarowywa`c graczy.\n\r", ch );
	return FALSE;
    }

    if ( victim == ch )
    {
	ASTC( "Jeste`s sob`a coraz bardziej oczarowan$y!", ch );
	return FALSE;
    }

/*  if ( ch->level >= LEVEL_HERO && get_trust( ch ) < L_SEN )
    {
	send_to_char( "Nie potrzebujesz ju`z s`lug.\n\r", ch );
	sprintf( log_buf, "%s charming", ch->name );
	log_string( log_buf );
	return FALSE;
    } */

    /* wydaje sie niemozliwe przy obecnych is_safe */
    if ( IS_AFFECTED( victim, AFF_CHARM ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM )
      || level < victim->level
      || saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    if ( !IS_NPC( victim )
      || IS_SET( victim->act, ACT_NO_CHARM ) )
    {
	act( ZM_ZAWSZE, "$N nie pozwala si`e opanowa`c.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	if ( is_fighting( rch, victim ) )
	{
	    act( ZM_ZAWSZE, "$N jest zbyt zaj`et$Y walk`a, `zeby zwraca`c na ciebie uwag`e.", ch, NULL, victim, TO_CHAR );
	    return FALSE;
	}

    if ( victim->master )
	stop_follower( victim );

    if ( is_affected( victim, gsn_kontrola_umyslu ) ) /* wiemy, ze nie CHARM */
    {
	act( ZM_PRZYT, "Tracisz mo`zliwo`s`c kontrolowania s`lug.", victim, NULL, NULL, TO_CHAR );
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

    send_to_char( "Rzucasz oczarowanie.\n\r", ch );
    act( ZM_PRZYT, "Czy`z $n nie jest mi`l$y?", ch, NULL, victim, TO_VICT );

    return TRUE;
}


CZAR( spell_chill_touch )
{
    CHAR_DATA   *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    static const int         dam_each[ ] =
    {
	 0,
	 0,  0,  6,  7,  8,      9, 12, 13, 13, 13,
	14, 14, 14, 15, 15,     15, 16, 16, 16, 17,
	17, 17, 18, 18, 18,     19, 19, 19, 20, 20,
	20, 21, 21, 21, 22,     22, 22, 23, 23, 23,
	24, 24, 24, 25, 25,     25, 26, 26, 26, 27,
	27, 27, 28, 28, 29,	29, 30, 30, 30, 31,
	31, 32, 32, 32, 33,	33, 33, 34, 34, 34,
	35, 35, 36, 36, 36,	37, 37, 38, 38, 39,
	39, 39, 40, 40, 41,	41, 42, 42, 43, 43,
	44, 44, 45, 45, 46, 	46, 47, 48, 49, 50
    };
		 int         dam;

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam   = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );
    if ( !saves_spell( level, victim ) )
    {
	af.type      = sn;
	af.duration  = 6;
	af.location  = APPLY_STR;
	af.modifier  = -2;
	CLEAR_BITS( af.bitvector );
	af.level = level;
	af.caster = ch;
	affect_join( victim, &af, ENH_TIME );
    }
    else
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_colour_spray )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each[ ] =
    {
	 0,
	 0,  0,  0,  0,  0,      0,  0,  0,  0,  0,
	30, 35, 40, 45, 50,     55, 55, 55, 56, 57,
	58, 58, 59, 60, 61,     61, 62, 63, 64, 64,
	65, 66, 67, 67, 68,     69, 70, 70, 71, 72,
	73, 73, 74, 75, 76,     76, 77, 78, 79, 79,

	80, 80, 81, 81, 82,	82, 83, 83, 84, 85,
	86, 86, 87, 87, 88,	89, 90, 90, 91, 91,
	92, 92, 93, 93, 94,	95, 95, 96, 96, 97,
	97, 98, 98, 99, 99, 	100, 101, 102, 102, 103,
	103, 104, 105, 105, 106,	106, 107, 107, 108, 109
    };
		 int        dam;

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_cone_of_silence )
{
    CHAR_DATA *rch;
    ROOM_INDEX_DATA *pRoomIndex;
    AFFECT_DATA af;

    if ( !( pRoomIndex = ch->in_room ) )
	return FALSE;

    if ( IS_SET( pRoomIndex->room_flags, ROOM_SAFE ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c w tym pomieszczeniu.\n\r", ch );
	return FALSE;
    }

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( IS_NPC( rch )
	  && rch->level > ch->level + 10
	  && can_see( rch, ch )
	  && !IS_AFFECTED( rch, AFF_CHARM )
	  && !is_safe( ch, rch, FALSE ) )
	{
	    char text[ 2 * MIL ];

	    sprintf( text, "%s usi`luje wyciszy`c pomieszczenie! Pom`o`zcie mi %s unieszkodliwi`c!",
		ch->short_descr, ch->sex == 2 ? "j`a" : "go" );
	    do_yell( rch, text );
	    multi_hit( rch, ch, TYPE_UNDEFINED );
	    return FALSE;
	}
    }

    if ( !IS_SET( pRoomIndex->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "Tworzysz stref`e ciszy!\n\r", ch );
	act( ZM_SLUCH, "$n tworzy stref`e ciszy!", ch, NULL, NULL, TO_ROOM );

	af.type = sn;
	af.duration = UMAX( 3, level / 10 );
	af.bitvector[ 0 ] = ROOM_CONE_OF_SILENCE;
	/* na pomieszczenia nieuzywane af.level/caster */
	affect_to_room( pRoomIndex, &af );

	return TRUE;
    }

    STC( "W tym pomieszczeniu jest ju`z wystarczaj`aco cicho.\n\r", ch );

    return FALSE;
}


CZAR( spell_continual_light )
{
    OBJ_DATA *light;

    light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), 0 );

    act( ZM_ZAWSZE, "Pstrykasz palcami tworz`ac $j.", ch, light, NULL, TO_CHAR );
    /* ch nie zostanie oslepiony, ale kolejne postacie powinny */
    obj_to_room( light, ch->in_room );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n pstryka palcami tworz`ac $j.", ch, light, NULL, TO_ROOM );
    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, light, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_control_weather )
{
    if ( *target_name && !str_prefix( target_name, "lepsza" ) )
	ch->in_room->area->weather_info.change += dice( UMIN( level * 3, 100 ) / 3, 4 );
    else if ( *target_name && !str_prefix( target_name, "gorsza" ) )
	ch->in_room->area->weather_info.change -= dice( UMIN( level * 3, 100 ) / 3, 4 );
    else
    {
	send_to_char( "Jak chcesz zmieni`c pogod`e? (lepsza/gorsza)\n\r", ch );
	return FALSE;
    }

    send_to_char( "Zmieniasz pogod`e.\n\r", ch );

    return TRUE;
}


/*
 * Lam 15.9.2005: tworzenie jedzenia z jadlospisem.
 * Wiekszosc pracy nad samym jadlospisem wykonal Nifir.
 */
void stworz_jedzenie( CHAR_DATA *ch, int level, int vnum )
{
    OBJ_DATA *mushroom;

    mushroom = create_object( get_obj_index( vnum ), 0 );
    mushroom->value[ 0 ].v = 5 + level;
    obj_to_room( mushroom, ch->in_room );

    if ( mushroom->rodzaj == 3 )
    {
	act( ZM_WZROK | ZM_WID_OBJ1, "Nagle pojawiaj`a si`e tu $p.", ch, mushroom, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_OBJ1, "Nagle pojawiaj`a si`e tu $p.", ch, mushroom, NULL, TO_ROOM );
    }
    else
    {
	act( ZM_WZROK | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, mushroom, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, mushroom, NULL, TO_ROOM );
    }

    return;
}


/*
 * VNUM_MUSHROOM chowal grzyba, ale od poczatku istnienia Laca byl nim magiczny
 * gofr, ktory zostaje awaryjnie na wypadek braku innego jedzenia, aby nie
 * znikal graczom z plikow postaci, no i byl kiedys prog uzywajacy vnumu 20.
 */
CZAR( spell_create_food )
{
    OBJ_INDEX_DATA *oi;
    char mbuf[ MIL ];
    char buf[ MSL ];
    int vn, i, num;

    if ( *target_name
      && ( !str_prefix( target_name, "lista" )
	|| !str_prefix( target_name, "jad`lospis" ) ) )
    {
	i = 0;
	buf[ 0 ] = '\0';
	for ( vn = OBJ_VNUM_JEDZ_MIN; vn <= OBJ_VNUM_JEDZ_MAX; vn++ )
	    if ( ( oi = get_obj_index( vn ) ) )
	    {
		sprintf( mbuf, "%3d. %s\n\r", ++i, oi->short_descr );
		strcat( buf, mbuf );
	    }

	if ( !i )
	{
	    RZUCENIE_CZARU( ch, sn, usage );
	    stworz_jedzenie( ch, level, OBJ_VNUM_MUSHROOM );
	    return TRUE;
	}
	else
	    STC( buf, ch );

	return FALSE;
    }

    RZUCENIE_CZARU( ch, sn, usage );

    if ( !*target_name )
    {
	stworz_jedzenie( ch, level, OBJ_VNUM_MUSHROOM );
	return TRUE;
    }

    if ( is_number( target_name ) )
	num = atoi( target_name );
    else
	num = 0;

    i = 0;
    for ( vn = OBJ_VNUM_JEDZ_MIN; vn <= OBJ_VNUM_JEDZ_MAX; vn++ )
	if ( ( oi = get_obj_index( vn ) ) )
	{
	    if ( ( !num && is_name( target_name, oi->name ) )
	      || ++i == num )
	    {
		stworz_jedzenie( ch, level, vn );
		return TRUE;
	    }
	}

    stworz_jedzenie( ch, level, OBJ_VNUM_MUSHROOM );

    return TRUE;
}


CZAR( spell_create_spring )
{
    OBJ_DATA *spring;

    if ( ch->in_room )
    {
	if ( CZY_WODA( ch->in_room ) )
	{
	    act( ZM_PRZYT, "W wodzie powstaje niewielki wir gin`ac po chwili.", ch, NULL, NULL, TO_CHAR );
	    act( ZM_PRZYT, "W wodzie powstaje niewielki wir gin`ac po chwili.", ch, NULL, NULL, TO_ROOM );
	    return FALSE;
	}
	else if ( CZY_POWIETRZE( ch->in_room ) )
	{
	    act( ZM_WZROK, "Powietrze elektryzuje si`e, kilka kropel wody pojawia si`e i spada w d`o`l.",
	      ch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK, "Powietrze elektryzuje si`e, kilka kropel wody pojawia si`e i spada w d`o`l.",
	      ch, NULL, NULL, TO_ROOM );
	    return FALSE;
	}
    }

    spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0 );
    spring->timer = level;
    obj_to_room( spring, ch->in_room );

    act( ZM_WZROK | ZM_WID_OBJ1, "$p wytryska z ziemi.", ch, spring, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_OBJ1, "$p wytryska z ziemi.", ch, spring, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_create_water )
{
    OBJ_DATA *obj   = (OBJ_DATA *) vo;
    int       water;

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "W tym nie da si`e trzyma`c p`lyn`ow.\n\r", ch );
	return FALSE;
    }

    if ( obj->value[ 2 ].v != LIQ_WATER && obj->value[ 1 ].v != 0 )
    {
	send_to_char( "To ju`z zawiera jak`a`s ciecz.\n\r", ch );
	return FALSE;
    }

    water = UMIN( level * ( ch->in_room->area->weather_info.sky >= SKY_RAINING ? 4 : 2 ),
		 obj->value[ 0 ].v - obj->value[ 1 ].v );

    if ( water > 0 )
    {
	obj->value[ 2 ].v = LIQ_WATER;
	obj->value[ 1 ].v += water;
	if ( !is_name( "woda", obj->name ) )
	{
	    char buf[ MAX_STRING_LENGTH ];

	    sprintf( buf, "%s woda", obj->name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}
	act( ZM_ZAWSZE, "Nape`lniasz $p.", ch, obj, NULL, TO_CHAR );
    }

    return TRUE;
}


CZAR( spell_cure_blindness )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA *paf;
    bool leczono = FALSE;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( is_affected( victim, gsn_blindness ) )
    {
	affect_strip( victim, gsn_blindness );
	leczono = TRUE;
    }

    if ( is_affected( victim, gsn_dazzle ) )
    {
	affect_strip( victim, gsn_dazzle );
	leczono = TRUE;
    }

    if ( is_affected( victim, gsn_krwawe_lzy ) )
    {
	affect_strip( victim, gsn_krwawe_lzy );
	leczono = TRUE;
    }

    if ( is_affected( victim, gsn_rune_flare ) )
    {
	affect_strip( victim, gsn_rune_flare );
	leczono = TRUE;
    }

    for ( paf = victim->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;

	if ( IS_SET_V( paf->bitvector, AFF_BLIND ) )
	{
	    affect_remove( victim, paf, TRUE );
	    leczono = TRUE;
	}
    }

    if ( !leczono )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( ch != victim )
	send_to_char( "Leczysz `slepot`e.\n\r", ch );

    act( ZM_WZROK, "Znowu mo`zesz widzie`c!", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_cure_critical )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( victim->hit >= victim->max_hit )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    heal = dice( 3, 8 ) + level - 6;
    victim->hit = UMIN( victim->hit + heal,
		  UMAX( victim->max_hit, victim->hit ) );
    update_pos( victim );

    if ( ch != victim )
	send_to_char( "Rzucasz leczenie.\n\r", ch );

    if ( victim->hit == victim->max_hit )
	ASTC( "Odzyska`l$o`s pe`lni`e zdrowia.", victim );
    else
	act( ZM_PRZYT, "Czujesz si`e du`zo lepiej!", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_cure_light )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( victim->hit >= victim->max_hit )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    heal = dice( 1, 8 ) + level / 3;
    victim->hit = UMIN( victim->hit + heal, UMAX( victim->max_hit, victim->hit ) );
    update_pos( victim );

    if ( ch != victim )
	send_to_char( "Rzucasz leczenie.\n\r", ch );

    if ( victim->hit == victim->max_hit )
	ASTC( "Odzyska`l$o`s pe`lni`e zdrowia.", victim );
    else
	act( ZM_PRZYT, "Czujesz si`e lepiej.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_cure_poison )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( !is_affected( victim, gsn_poison ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    affect_strip( victim, gsn_poison );

    if ( ch != victim )
	send_to_char( "Leczysz trucizn`e.\n\r", ch );

    act( ZM_PRZYT, "Ciep`ly pr`ad przenika twoje cia`lo.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "$N wygl`ada zdrowiej.", ch, NULL, victim, TO_NOTVICT );

    return TRUE;
}


CZAR( spell_cure_serious )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( victim->hit >= victim->max_hit )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    heal = dice( 2, 8 ) + level / 2 ;
    victim->hit = UMIN( victim->hit + heal,
		  UMAX( victim->max_hit, victim->hit ) );
    update_pos( victim );

    if ( victim->hit == victim->max_hit )
	ASTC( "Odzyska`l$o`s pe`lni`e zdrowia.", victim );
    else
	act( ZM_PRZYT, "Czujesz si`e lepiej!", victim, NULL, NULL, TO_CHAR );

    if ( ch != victim )
	send_to_char( "Rzucasz leczenie.\n\r", ch );

    return TRUE;
}


/*
 * Lam: podarunek many
 * 23.2.99: poprawka - jesli sie mialo wiecej many niz 100% (po zdjeciu
 * przedmiotu), uzdrawiacz ja zabieral
 */
CZAR( spell_mana_gift )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( victim->mana >= victim->max_mana )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    victim->mana = UMIN( victim->mana + 25,
			 UMAX( victim->mana, victim->max_mana ) );

    if ( victim->mana == victim->max_mana )
	ASTC( "Odzyska`l$o`s pe`lni`e magicznych si`l.", victim );
    else
	act( ZM_PRZYT, "Odnawiaj`a si`e twe magiczne moce.", victim, NULL, NULL, TO_CHAR );

    if ( ch != victim )
	send_to_char( "Udzielasz many.\n\r", ch );

    return TRUE;
}


CZAR( spell_curse )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_CURSE ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 2 * level;
    af.location = APPLY_HITROLL;
    /* Alandar: przeklenstwo natury mnicha ma - ch->level * 2 / 3, wiec tu daje / 2 */
    af.modifier = - level / 2;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_CURSE );
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    /* Alandar: + 10 OPC na 80 poziomie to ani duzo, ani malo */
    af.modifier  = - level / 8;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Rzucasz kl`atw`e.\n\r", ch );

    act( ZM_PRZYT, "Czujesz na sobie kl`atw`e.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


/*
 * Qwert 11.11.2000: dodane sprawdzenie czy nie jest lojalny
 */
CZAR( spell_destroy_cursed )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        yesno  = 0;

    for ( obj = victim->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	/* Qwert: niech czar nie niszczy przedmiotow lojalnych
	   Qwert 27.02.04: poprawka */
	if ( ( IS_OBJ_STAT( obj, ITEM_LOYAL )
	    || IS_OBJ_STAT( obj, ITEM_QUEST )
	    || IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	  && ch != victim
	  && ch->level < LEVEL_IMMORTAL
	  && obj->wear_flags != ITEM_TAKE )
	{
	    continue;
	}

	if ( IS_OBJ_STAT( obj, ITEM_NODROP )
	  && obj->wear_loc == WEAR_NONE )
	{
	    if ( obj->rodzaj == 3 )
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, "$p spadaj`a z ciebie i ulegaj`a zniszczeniu.",
		    victim, obj, NULL, TO_CHAR );
		act( ZM_PRZYT | ZM_W_WID_OBJ1, "Robi ci si`e l`zej.",
		    victim, obj, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$p spadaj`a z $1 i ulegaj`a zniszczeniu.",
		    victim, obj, NULL, TO_ROOM );
	    }
	    else
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, "$p spada z ciebie i ulega zniszczeniu.",
		    victim, obj, NULL, TO_CHAR );
		act( ZM_PRZYT | ZM_W_WID_OBJ1, "Robi ci si`e l`zej.",
		    victim, obj, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$p spada z $1 i ulega zniszczeniu.",
		    victim, obj, NULL, TO_ROOM );
	    }
	    extract_obj( obj );
	    yesno = 1;
	}
    }

    if ( ch != victim && yesno )
	send_to_char( "Niszczenie przekl`etych uda`lo si`e.\n\r", ch );

    return TRUE;
}


CZAR( spell_detect_evil )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( HAS_DETECT_EVIL( victim )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_DETECT_EVIL );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    if ( ch != victim )
	send_to_char( "Rzucasz wykrycie z`la.\n\r", ch );

    act( ZM_WZROK, "Oczy ci b`lyszcz`a.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


SPADANIE( wear_off_detect_evil )
{
    act( ZM_WZROK, "Czerwie`n w twoim widzeniu zanika.", ch, NULL, NULL, TO_CHAR );

    return;
}


CZAR( spell_detect_hidden )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( HAS_DETECT_HIDDEN( victim )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_DETECT_HIDDEN );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    if ( ch != victim )
	send_to_char( "Rzucasz wykrycie ukrytych.\n\r", ch );

    act( ZM_WZROK, "Wyostrza ci si`e wzrok.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


SPADANIE( wear_off_detect_hidden )
{
    act( ZM_WZROK, "Przestajesz widzie`c rzeczy ukryte.", ch, NULL, NULL, TO_CHAR );

    return;
}


CZAR( spell_detect_invis )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( HAS_DETECT_INVIS( victim )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_DETECT_INVIS );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    if ( ch != victim )
	send_to_char( "Rzucasz wykrycie niewidki.\n\r", ch );

    act( ZM_WZROK, "Rozszerzaj`a ci si`e `xrenice.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


SPADANIE( wear_off_detect_invis )
{
    act( ZM_WZROK, "Nie widzisz ju`z niewidzialnego.", ch, NULL, NULL, TO_CHAR );

    return;
}


CZAR( spell_detect_magic )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->klasa == CLASS_SKRYTOBOJCA && ch->level < L_APP )
    {
	STC( "Nic si`e nie wydarzy`lo.\n\r", ch );
	return FALSE;
    }

    if ( IS_AFFECTED( victim, AFF_DETECT_MAGIC )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_DETECT_MAGIC );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    if ( ch != victim )
	send_to_char( "Rzucasz wykrycie magii.\n\r", ch );

    /* #2457 - wykrycie magii ma sens takze bez wzroku, ZM_WZROK -> ZM_PRZYT */
    act( ZM_PRZYT, "Zaczynasz spostrzega`c magi`e.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_detect_poison )
{
    char bufor[ MSL ];
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    /* od teraz plyny sa obslugiwane osobno, nie razem z jedzeniem */
    if ( /*obj->item_type == ITEM_DRINK_CON ||*/ obj->item_type == ITEM_FOOD )
    {
	if ( obj->value[ 3 ].v != 0 )
	{
	    sprintf( bufor, "$p pachn%s jakim`s truj`acym jadem.",
	      obj->rodzaj == 3 ? "`a" : "ie");
	    act( ZM_ZAWSZE, bufor, ch, obj, NULL, TO_CHAR );
	}
	else
	{
	    sprintf( bufor, "$p wygl`ada%s bardzo apetycznie.",
	      obj->rodzaj == 3 ? "j`a" : "");
	    act( ZM_ZAWSZE, bufor, ch, obj, NULL, TO_CHAR );
	}
    }
    else if ( obj->item_type == ITEM_DRINK_CON && obj->value[ 3 ].v != 0 )
	act( ZM_ZAWSZE, "P`lyn w $k jest zdatny do picia.", ch, obj, NULL, TO_CHAR );
    else
    {
	sprintf( bufor, "$p nie wydaj%s si`e by`c zatrut%s.",
	  obj->rodzaj == 3 ? "`a" : "e",
	  obj->rodzaj == 1 ? "y" : obj->rodzaj == 2 ? "a" : "e" );
	act( ZM_ZAWSZE, bufor, ch, obj, NULL, TO_CHAR );
    }

    return TRUE;
}


CZAR( spell_dispel_evil )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    if ( !IS_NPC( ch ) && IS_EVIL( ch ) )
    {
	ASTC( "Jeste`s za bardzo z`l$y, `zeby to zrobi`c.", ch );
	return FALSE;
    }

    if ( IS_GOOD( victim ) || IS_NEUTRAL( victim ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "Bogowie chroni`a $3.", victim, NULL, ch, TO_ROOM );
	act( ZM_WZROK | ZM_WID_CHAR, "Bogowie chroni`a ci`e przed wp`lywem czaru $1.", ch, NULL, victim, TO_VICT );
	return FALSE;
    }

    dam = dice( level, 4 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


/* Ulryk: calkiem nowe rozproszenie */
CZAR( spell_dispel_magic )
{
    char         arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA   *victim = NULL;
    AFFECT_DATA *paf;
    bool         fOffensive = FALSE;
    bool         fSelfAll   = FALSE;
    bool         udane      = FALSE;
    bool         bialazw    = FALSE; /* Lam: biala z wplywu */

    if ( IS_NPC( ch ) && ch->pIndexData->pHealer )
	victim = ( CHAR_DATA* ) vo;

    if ( !victim )
    {
	target_name = one_argument( target_name, arg );
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "Kogo chcesz rozproszy`c?\n\r", ch );
	    return FALSE;
	}

	if ( arg[ 0 ] == '\0' )
	    victim = ch;

	if ( arg[ 0 ] == '\0'
	  && ch->fighting
	  && FIGHTING( ch )->in_room == ch->in_room
	  && ( IS_NPC( ch )
	    || ch->klasa != CLASS_PSIONIK
	    || ch->level >= L_APP ) ) /* psionik-BWWU moze rozpraszac oslepiony */
	{
	    victim = FIGHTING( ch );
	}

	target_name = one_argument( target_name, arg );
	if ( ( !str_prefix( arg, "wszystkie" ) && arg[ 0 ] != '\0' )
	  || ( arg[ 0 ] == '\0' && is_fighting( ch, victim ) ) )
	{
	    fOffensive = TRUE;
	}

	if ( victim == ch && fOffensive )
	{
	    fSelfAll = TRUE;
	    fOffensive = FALSE;
	}
    }

    /* Lam 13.11.2005: nie dla psionikow */
    if ( !IS_NPC( ch )
      && victim != ch
      && usage == USAGE_CAST
      && ch->klasa == CLASS_PSIONIK
      && ch->level < L_APP )
    {
	STC( "Nie mo`zesz rzuca`c tego czaru na innych.\n\r", ch );
	return FALSE;
    }

    RZUCENIE_CZARU( ch, sn, usage );

    if ( fOffensive )
    {
	if ( is_safe( ch, victim, TRUE ) )
	{
	    NIEUDANE( ch );
	    return FALSE;
	}
	check_killer( ch, victim );
    }

    if ( IS_NPC( victim )
      && IS_AFFECTED( victim, AFF_SLUGA )
      && victim->master )
    {
	act( ZM_ZAWSZE, "$N rozpada si`e na skutek twego czaru.",
	     ch, NULL, victim, TO_CHAR );
	send_to_char( "Rozpadasz si`e na skutek rozproszenia magii.\n\r",
		      victim );
	act( ZM_WZROK | ZM_WID_VICT, "$N rozpada si`e na skutek rozproszenia magii.",
	     ch, NULL, victim, TO_NOTVICT );

	extract_char( victim, TRUE );
	return TRUE;
    }

    /* od teraz zasadnicze zmiany */
    for ( paf = victim->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;

	if ( IS_SET_V( paf->bitvector, AFF_SANCTUARY ) )
	    bialazw = TRUE;

	if ( IS_SET( skill_table[ paf->type ].flags, SKILL_NO_DISPEL )
	  && skill_get_trust( ch ) < LEVEL_IMMORTAL )
	{
	    if ( skill_table[ paf->type ].spell_fun
	      && skill_table[ paf->type ].spell_fun != spell_null
	      && paf->type != gsn_poison )
	    {
		ch_printf( ch, "Nie mo`zesz rozproszy`c czaru \"%s\".\n\r",
			   skill_table[ paf->type ].pl_name );
	    }
	    continue;
	}

	if ( paf->type == gsn_kontrola_umyslu
	  && skill_get_trust( ch ) < LEVEL_IMMORTAL
	  && ( !IS_NPC( victim ) ) )
	{
	    send_to_char( "Nie mo`zesz rozproszy`c czaru \"kontrola umys`lu\".\n\r", ch );
	    continue;
	}

	if ( !skill_table[ paf->type ].spell_fun
	  || skill_table[ paf->type ].spell_fun == spell_null )
	{
	    continue;
	}

	if ( fSelfAll
	  || ( skill_table[ paf->type ].target == TAR_CHAR_OFFENSIVE )
	  || IS_SET( skill_table[ paf->type ].flags, SKILL_FRIENDLY_DISPEL ) )
	{
	    affect_remove( victim, paf, TRUE );
	    udane = TRUE;
	    continue;
	}

	if ( !fOffensive )
	    continue;

	if ( !saves_spell( level, victim ) )
	{
	    if ( skill_table[ paf->type ].msg_off && *skill_table[ paf->type ].msg_off )
		act( ZM_PRZYT, skill_table[ paf->type ].msg_off, victim, NULL, NULL, TO_CHAR );
	    /* skad wiem, jakie czary ktos z kogos zdejmuje? Nawet z wykryciem
	       magii nie znam "wplywow" innych postaci! */
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_DYLEMAT, "$n przesta`l$a by`c pod wp`lywem dzia`lania czaru \"$t\".",
		    victim, skill_table[ paf->type ].pl_name, NULL, TO_ROOM );
	    affect_strip( victim, paf->type );
	    udane = TRUE;
	}
    }

    if ( fOffensive
      && !bialazw
      && IS_AFFECTED( victim, AFF_SANCTUARY )
      && !saves_spell( level, victim ) )
    {
	REMOVE_BIT_V( victim->affected_by, AFF_SANCTUARY );
	if ( !HAS_SANCTUARY( victim ) )
	{
	    act( ZM_WZROK, "Bia`la aura znika.", victim, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "Bia`la aura znika z cia`la $1.", victim, NULL, NULL, TO_ROOM );
	    udane = TRUE;
	}
    }

    if ( !udane )
	STC( "Nie uda`lo ci si`e rozproszy`c `zadnej magii.\n\r", ch );
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Uda`lo ci si`e rzuci`c rozproszenie magii.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rzuci`l$a na siebie rozproszenie magii.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
	act( ZM_ZAWSZE, "Uda`lo ci si`e rzuci`c na $# rozproszenie magii.", ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "$n rzuci`l$a na ciebie rozproszenie magii.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "$n rzuci`l$a na $# rozproszenie magii.", ch, NULL, victim, TO_NOTVICT );
    }

    sprawdz_ubranie( victim );

    if ( fOffensive )
    {
	if ( !is_fighting( ch, victim ) )
	    set_fighting( ch, victim );

	if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	{
	    set_fighting( victim, ch );
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	}
    }

    return udane;
}


CZAR( spell_earthquake )
{
    CHAR_DATA *vch;
    ROOM_INDEX_DATA *room = ch->in_room;

    if ( !CZY_ZIEMIA( ch->in_room ) )
    {
	send_to_char( "Tutaj to nie zadzia`la.\n\r", ch );
	return FALSE;
    }

    send_to_char( "Ziemia trz`esie ci si`e pod stopami!\n\r", ch );
    act( ZM_PRZYT, "Ziemia trz`esie si`e i dr`zy.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
	if ( vch->deleted || !vch->in_room || vch == ch )
	    continue;

	/* obrazenia w pomieszczeniu dopoki ch nie ucieknie/nie zginie */
	if ( vch->in_room == room
	  && ch->in_room == room )
	{
	    if ( vch->position == POS_SLEEPING )
	    {
		STC( "Budzi ci`e jaki`s ha`las.\n\r", vch );
		vch->position = POS_RESTING;
	    }

	    if ( is_safe( ch, vch, FALSE ) )
		continue;

	    if ( vch->position > POS_RESTING )
		damage( ch, vch, level + dice( 2, 8 ), sn, WEAR_NONE, FALSE );

	    if ( !IS_FLYING( vch ) && vch->in_room == room )
		WAIT_STATE( vch, 10 );

	    continue;
	}

	/* a tu nietypowo jak na obszarowke, nawet po smierci ch, funkcja nie
	   konczy sie, a leci dalej po liscie, zeby wszyscy zobaczyli ten efekt */
	if ( vch->in_room != room
	  && vch->in_room->area == room->area
	  && IS_AWAKE( vch )
	  && CZY_ZIEMIA( vch->in_room ) )
	{
	    send_to_char( "Ziemia trz`esie si`e i dr`zy.\n\r", vch );
	}
    }

    return ( ch->in_room == room ) ? TRUE : FALSE;
}


CZAR( spell_enchant_weapon )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    char        buf[ MAX_STRING_LENGTH ];

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| obj->affected )
    {
	send_to_char( "Ten przedmiot nie mo`ze by`c wzmocniony.\n\r", ch );
	return FALSE;
    }

    if ( obj->wear_loc != WEAR_NONE )
	remove_obj( ch, obj->wear_loc, TRUE );

    if ( ch->deleted ) /* na wypadek remove_proga */
	return FALSE;

    /* Thanks to Magoo for this little insight */
    if ( obj->level > ch->level ) /* Lam 4.6.2000: trust -> level */
    {
	send_to_char( "Nie mo`zesz wzmacnia`c broni o poziomie wy`zszym od twojego.\n\r",
		     ch );
	return FALSE;
    }

    paf                 = new_affect( );

    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_HITROLL;
    paf->modifier       = level / 5;
    CLEAR_BITS( paf->bitvector );
    paf->next           = obj->affected;
    obj->affected       = paf;

    paf                 = new_affect( );

    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_DAMROLL;
    paf->modifier       = level / 10;
    CLEAR_BITS( paf->bitvector );
    paf->next           = obj->affected;
    obj->affected       = paf;
    obj->level          = UMAX( number_fuzzy( ch->level - 5 ), obj->level );

    SET_BIT_V( obj->extra_flags, ITEM_MAGIC );

    sprintf( buf, "$p `swiec%s si`e na ", ( obj->rodzaj == 3 ) ? "`a" : "i" );

    if ( IS_GOOD( ch ) )
    {
	strcat( buf, "niebiesko." );
	SET_BIT_V( obj->extra_flags, ITEM_ANTI_EVIL );
    }
    else if ( IS_EVIL( ch ) )
    {
	strcat( buf, "czerwono." );
	SET_BIT_V( obj->extra_flags, ITEM_ANTI_GOOD );
    }
    else
    {
	strcat( buf, "`z`o`lto." );
	SET_BIT_V( obj->extra_flags, ITEM_ANTI_EVIL );
	SET_BIT_V( obj->extra_flags, ITEM_ANTI_GOOD );
    }

    act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );

    if ( !IS_OBJ_STAT( obj, ITEM_PRIVATE )
      || !str_cmp( ch->name, obj->owner ) )
    {
	free_string( obj->owner );
	obj->owner = str_dup( ch->name );
	SET_BIT_V( obj->extra_flags, ITEM_PRIVATE );
    }

    send_to_char( "Czar si`e uda`l.\n\r", ch );

    return TRUE;
}


/*
 * Drain XP, MANA, HP.
 * Caster gains HP.
 *
 * Qwert 12.11.2000 nie dziala na mnicha, kazdy czar i tak mu zabiera
 *
 * Lam: ale tylko mane, ktora tylko mnich moze ssac do woli i traci najwyzej
 * 10/atak, w porownaniu do ssania energii, ktorego efekt mozna szybko zmarnowac
 * dezintegracja. Dlatego mi sie to nie podoba.
 *
 * Vigud 2.4.2010: kompromis: niech mnichowi oszczedza tylko mane
 */
CZAR( spell_energy_drain )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    if ( saves_spell( level, victim )
      /*|| victim->klasa == CLASS_MNICH*/ )
    {
	send_to_char( "Nic si`e nie wydarzy`lo.\n\r", ch );
	return FALSE;
    }

    ch->alignment = UMAX( -1000, ch->alignment - 200 );
    if ( victim->level <= 2 )
	dam              = ch->hit + 1;
    else
    {
	gain_exp( victim, 0 - number_range( level / 3, level ) );

	/* Vigud: warunek */
	if ( victim->klasa != CLASS_MNICH )
	    victim->mana = UMAX( victim->mana / 2, victim->mana - number_range( level / 3, level ) );

	victim->move	 = UMAX( victim->move / 2, victim->move - number_range( level / 3, level ) );
	/* kto by uwierzyl, ze tyle czasu mogl zyc taki wzor?
	dam		 = dice( 1, level ); */
	dam		 = UMAX( ( ( interpolate( level, 200, 4500 )
				     + 2 * ch->max_hit )
				   - ch->hit ) / 128 + level / 16, 2 );
	dam		 = number_range( dam / 2, dam * 3 / 2 );
	ch->hit		+= dam;
    }

    damage( ch, victim, dam * 2, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    /* Obnizenie moralnosci powyzej moglo spowodowac, ze niektore przedmioty
       beda parzyly ch. Sprawdzam tylko ch->deleted - w innych sytuacjach nie ma
       znaczenia, co sie stalo z ch, sprawdzenie albo bedzie na miejscu, albo
       niczego nie zepsuje. */
    if ( !ch->deleted )
	sprawdz_ubranie( ch );

    return TRUE;
}


/*
 * Lam: czar klerycki - skopiowany zywcem z kuli ognia, ale na sporo pozniejszym
 * poziomie, wiec wszystko w porzadku.
 */
CZAR( spell_finger_of_death )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
		 int        dam;
    static const int        dam_each[ ] =
    {
	  0,
	  0,   0,   0,   0,   0,          0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,         35,  40,  45,  50,  55,
	 60,  65,  70,  75,  80,         82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,        102, 104, 106, 108, 110,
	112, 114, 116, 118, 120,        122, 124, 126, 128, 130,
	132, 134, 136, 138, 140,	142, 144, 146, 148, 150,
	152, 154, 156, 158, 160,	162, 164, 166, 168, 170,
	172, 174, 176, 178, 180,	182, 184, 186, 188, 190,
	192, 194, 196, 198, 200,	202, 204, 206, 208, 210,
	212, 214, 216, 218, 220,	222, 224, 226, 228, 230
    };

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_fireball )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
		 int        dam;
    static const int        dam_each[ ] =
    {
	  0,
	  0,   0,   0,   0,   0,          0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,         35,  40,  45,  50,  55,
	 60,  65,  70,  75,  80,         82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,        102, 104, 106, 108, 110,
	112, 114, 116, 118, 120,        122, 124, 126, 128, 130,
	132, 134, 136, 138, 140,	142, 144, 146, 148, 150,
	152, 154, 156, 158, 160,	162, 164, 166, 168, 170,
	172, 174, 176, 178, 180,	182, 184, 186, 188, 190,
	192, 194, 196, 198, 200,	202, 204, 206, 208, 210,
	212, 214, 216, 218, 220,	222, 224, 226, 228, 230
    };

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_flamestrike )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = dice( 6, level );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_faerie_fire )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int         effect;

    if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    effect       = - UMIN( 2 * level, 150 );
    if ( saves_spell( level, victim ) )
	effect  /= 2;

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = effect;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_FAERIE_FIRE );

    affect_to_char( victim, &af );

    act( ZM_WZROK, "Otacza ci`e r`o`zowa otoczka.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n jest otoczon$y r`o`zow`a otoczk`a.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_faerie_fog )
{
    CHAR_DATA *ich;

    ASTC( "Wywo`la`l$o`s chmur`e purpurowego dymu.", ch );
    act( ZM_WZROK, "$n wywo`la`l$a chmur`e purpurowego dymu.", ch, NULL, NULL, TO_ROOM );

    for ( ich = ch->in_room->people; ich; ich = ich->next_in_room )
    {
	if ( ( !IS_NPC( ich ) && IS_SET( ich->act, PLR_WIZINVIS ) )
	  || ich == ch
	  || saves_spell( level, ich ) )
	{
	    continue;
	}

	if ( is_affected( ich, gsn_disguise ) )
	{
	    char zapas[ MIL ];

	    strcpy( zapas, ich->long_descr );
	    free_string( ich->long_descr );
	    ich->long_descr = str_dup( ich->long_descr_orig );
	    free_string( ich->long_descr_orig );
	    ich->long_descr_orig = str_dup( zapas );
	    affect_strip( ich, gsn_disguise );
	}
	affect_strip( ich, gsn_invis );
	affect_strip( ich, gsn_mass_invis );
	affect_strip( ich, gsn_sneak );
	affect_strip( ich, gsn_shadow );
	REMOVE_BIT_V( ich->affected_by, AFF_HIDE );
	REMOVE_BIT_V( ich->affected_by, AFF_INVISIBLE );
	REMOVE_BIT_V( ich->affected_by, AFF_SNEAK );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zosta`l$a zdemaskowan$y!", ich, NULL, NULL, TO_ROOM );
	act( ZM_PRZYT, "Zosta`l$o`s zdemaskowan$y!", ich, NULL, NULL, TO_CHAR );
    }

    return TRUE;
}


CZAR( spell_fly )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_FLYING( victim )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level + 3;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_FLYING );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    if ( IS_SET( race_table[ victim->race ].parts, P_NOGI )
      && victim->in_room && CZY_ZIEMIA( victim->in_room ) )
    {
	act( ZM_PRZYT, "Twoje stopy odrywaj`a si`e od ziemi.", victim, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Stopy $1 odrywaj`a si`e od ziemi.", victim, NULL, NULL, TO_ROOM );
    }
    else if ( IS_SET( race_table[ victim->race ].parts, P_LAPA )
      && victim->in_room && CZY_ZIEMIA( victim->in_room ) )
    {
	act( ZM_PRZYT, "Twoje `lapy odrywaj`a si`e od ziemi.", victim, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "`Lapy $1 odrywaj`a si`e od ziemi.", victim, NULL, NULL, TO_ROOM );
    }
    else if ( victim->in_room && CZY_POD_WODA( victim->in_room ) )
    {
	act( ZM_PRZYT, "Czujesz si`e l`zejsz$y.", victim, NULL, NULL, TO_CHAR );
    }
    else if ( victim->in_room && CZY_POWIETRZE( victim->in_room ) )
    {
	act( ZM_PRZYT, "Zaczynasz unosi`c si`e w powietrzu.", victim, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zaczyna unosi`c si`e w powietrzu.", victim, NULL, NULL, TO_ROOM );
    }
    else
    {
	act( ZM_PRZYT, "Unosisz si`e w powietrze.", victim, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n unosi si`e w powietrze.", victim, NULL, NULL, TO_ROOM );
    }

    return TRUE;
}


CZAR( spell_gate )
{
    MOB_INDEX_DATA *ind;
    CHAR_DATA *gch;
    CHAR_DATA *vampire;
    int        npccount  = 0;
    int        pccount   = 0;

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_SUMMON ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
	    npccount++;

	if ( !IS_NPC( gch )
	  || ( IS_NPC( gch ) && IS_AFFECTED( gch, AFF_CHARM ) ) )
	{
	    pccount++;
	}
    }

    if ( npccount > pccount + level / 30 )
    {
	do_say( ch, "Jest tu nas za du`zo! Jeden musi umrze`c!" );
	return FALSE;
    }

    do_say( ch, "Chod`xcie za mn`a! Po`l`aczcie si`e ze mn`a w krwawej `la`xni!" );

    ind = get_mob_index( MOB_VNUM_VAMPIRE );
    /* Lam: w dosowym Lacu uzycie czaru wywalalo z powodu braku indeksu */
    if ( !ind )
    {
	NIEUDANE( ch );
	return FALSE;
    }
    /* Ulryk: czar zawsze sprowadzal 50 poziomowego wampira, w zwiazku z czym,
       calosc miala dosc ograniczone zastosowanie */
    vampire = create_mobile( ind );
    vampire->level   = URANGE( 1, ch->level - 5, LEVEL_HERO );
    vampire->max_hit = vampire->level * 8 + number_range(
				 vampire->level * vampire->level * 3 / 16,
				 vampire->level * vampire->level / 4 );
    vampire->hit     = vampire->max_hit;

    add_follower( vampire, ch );
    char_to_room( vampire, ch->in_room );
    if ( FIGHTING( ch ) )
    {
	if ( !is_fighting( vampire, FIGHTING( ch ) ) )
	    set_fighting( vampire, FIGHTING( ch ) );

	if ( !is_fighting( FIGHTING( ch ), vampire )
	  && ZDOLNY_DO_WALKI( FIGHTING( ch ) ) )
	{
	    set_fighting( FIGHTING( ch ), vampire );
	}
    }

    return TRUE;
}


CZAR( spell_giant_strength )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_STR;
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
	send_to_char( "Rzucasz si`l`e olbrzyma.\n\r", ch );

    act( ZM_PRZYT, "Czujesz si`e mocniejsz$y.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_harm )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = UMAX( 20, victim->hit - dice( 1,4 ) );

    if ( saves_spell( level, victim ) )
	dam = UMIN( 50, dam / 4 );

    dam = UMIN( 100, dam );

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_heal )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( victim->hit >= victim->max_hit )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    heal = 50 + 2 * UMAX( 0, level - 42 ) + dice( 4, 8 );
    victim->hit = UMIN( victim->hit + heal,
		  UMAX( victim->max_hit, victim->hit ) );
    update_pos( victim );

    if ( ch != victim )
	send_to_char( "Rzucasz uzdrowienie.\n\r", ch );

    if ( victim->hit == victim->max_hit )
	ASTC( "Odzyska`l$o`s pe`lni`e zdrowia.", victim );
    else
	act( ZM_PRZYT, "Ciep`lo ogarnia twoje cia`lo.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_identify )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    char         buf[ MAX_STRING_LENGTH ];
    int          czypas;

    sprintf( buf,
	    "%s %s typu %s, dodatkowe w`la`sciwo`sci: %s.\n\r",
	    obj->short_descr,
	    obj->rodzaj == 3 ? "s`a" : "jest",
	    item_type_name_pl( obj ),
	    extra_bit_name_pl_dla_ludzi( obj->extra_flags, ch ) );
    buf[ 0 ] = UPPER( buf[ 0 ] );
    send_to_char( buf, ch );
    sprintf( buf,
	    "Waga: %s, warto`s`c: %d, poziom: %d.\n\r",
	    slowny_ciezar( obj->weight ),
	    obj->cost,
	    obj->level );
    send_to_char( buf, ch );

    switch ( obj->item_type )
    {
    case ITEM_SPELLBOOK:
	if ( obj->value[ 0 ].v > 0 && obj->value[ 0 ].v < MAX_SKILL
	  && obj->value[ 1 ].v > 0 && obj->value[ 1 ].v <= 100
	  && obj->value[ 2 ].v <= 110 )
	{
	    ch_printf( ch, "Ta ksi`ega uczy umiej`etno`sci \"%s\".\n\r",
		skill_table[ obj->value[ 0 ].v ].pl_name );
	}
	break;
    case ITEM_PILL:
    case ITEM_SCROLL:
    case ITEM_POTION:
	if ( ( obj->value[ 1 ].v <= 0 || obj->value[ 1 ].v >= MAX_SKILL )
	  && ( obj->value[ 2 ].v <= 0 || obj->value[ 2 ].v >= MAX_SKILL )
	  && ( obj->value[ 3 ].v <= 0 || obj->value[ 3 ].v >= MAX_SKILL ) )
	{
	    STC( "Nie zawiera `zadnych czar`ow.\n\r", ch );
	    break;
	}
	sprintf( buf, "Czary poziomu %d:", obj->value[ 0 ].v );
	send_to_char( buf, ch );

	if ( obj->value[ 1 ].v > 0 && obj->value[ 1 ].v < MAX_SKILL )
	{
	    send_to_char( " \"", ch );
	    send_to_char( skill_table[ obj->value[ 1 ].v ].pl_name, ch );
	    send_to_char( "\"", ch );
	}

	if ( obj->value[ 2 ].v > 0 && obj->value[ 2 ].v < MAX_SKILL )
	{
	    send_to_char( " \"", ch );
	    send_to_char( skill_table[ obj->value[ 2 ].v ].pl_name, ch );
	    send_to_char( "\"", ch );
	}

	if ( obj->value[ 3 ].v > 0 && obj->value[ 3 ].v < MAX_SKILL )
	{
	    send_to_char( " \"", ch );
	    send_to_char( skill_table[ obj->value[ 3 ].v ].pl_name, ch );
	    send_to_char( "\"", ch );
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	if ( obj->value[ 3 ].v <= 0 || obj->value[ 3 ].v >= MAX_SKILL )
	{
	    STC( "Nie zawiera `zadnych czar`ow.\n\r", ch );
	    break;
	}

	sprintf( buf, "Ma %d z %d `ladunk`ow %d poziomu czaru",
		obj->value[ 2 ].v, obj->value[ 1 ].v, obj->value[ 0 ].v );
	send_to_char( buf, ch );

	if ( obj->value[ 3 ].v > 0 && obj->value[ 3 ].v < MAX_SKILL )
	{
	    send_to_char( " \"", ch );
	    send_to_char( skill_table[ obj->value[ 3 ].v ].pl_name, ch );
	    send_to_char( "\"", ch );
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_WEAPON:
	sprintf( buf, "%s wykonan%s %s.\n\r",
		obj->rodzaj == 3 ? "S`a" : "Jest",
		obj->rodzaj == 1 ? "y" : obj->rodzaj == 2 ? "a" : "e",
		material_table[ obj->value[ 5 ].v ].dopelniacz );
	send_to_char( buf, ch );
	sprintf( buf, "Obra`zenia: od %d do %d (`srednio %d).\n\r",
		obj->value[ 1 ].v, obj->value[ 2 ].v,
		( obj->value[ 1 ].v + obj->value[ 2 ].v ) / 2 );
	send_to_char( buf, ch );
	break;

    case ITEM_ARMOR:
	sprintf( buf, "%s wykonan%s %s.\n\r",
		obj->rodzaj == 3 ? "S`a" : "Jest",
		obj->rodzaj == 1 ? "y" : obj->rodzaj == 2 ? "a" : "e",
		material_table[ obj->value[ 5 ].v ].dopelniacz );
	send_to_char( buf, ch );
	sprintf( buf, "Klasa pancerza: %d.\n\r", obj->value[ 0 ].v );
	send_to_char( buf, ch );
	break;
    case ITEM_TREASURE:
	sprintf( buf, "%s wykonan%s %s.\n\r",
		obj->rodzaj == 3 ? "S`a" : "Jest",
		obj->rodzaj == 1 ? "y" : obj->rodzaj == 2 ? "a" : "e",
		material_table[ obj->value[ 5 ].v ].dopelniacz );
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

    czypas = czy_pasuje( obj, ch );

    if ( czypas == WRONG_PERSON ) /* tylko osobisty, reszta sie zgadza */
	STC( "Ten przedmiot do ciebie nie pasuje, ale mo`zesz spr`obowa`c go dostosowa`c.\n\r", ch );
    else if ( czypas != NOTHING_WRONG )
	STC( "Ten przedmiot do ciebie wyra`xnie nie pasuje.\n\r", ch );

    return TRUE;
}


CZAR( spell_infravision )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( HAS_INFRAVISION( victim )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 2 * level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_INFRARED );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_WZROK, "Zaczynasz dostrzega`c nowe barwy.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "Oczy $1 staj`a si`e czerwone.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


SPADANIE( wear_off_infravision )
{
    act( ZM_WZROK, "Przestajesz widzie`c w ciemno`sci.", ch, NULL, NULL, TO_CHAR );

    return;
}


CZAR( spell_invis )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_INVISIBLE )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( !IS_AFFECTED( victim, AFF_INVISIBLE ) )
    {
	act( ZM_PRZYT, "Znikasz w nico`sci.", victim, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n znika w nico`sci.", victim, NULL, NULL, TO_ROOM );
    }

    af.type     = sn;
    af.duration = 24;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_INVISIBLE );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    return TRUE;
}


CZAR( spell_know_alignment )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char      *msg;
    int        ap;

    ap = victim->alignment;

	 if ( ap >  700 ) msg = "$N jest wr`ecz okropnie dobr$8.";
    else if ( ap >  350 ) msg = "$N ma wspania`ly charakter.";
    else if ( ap >  100 ) msg = "$N jest cz`esto mi`l$8 dla innych.";
    else if ( ap > -100 ) msg = "$N nie jest zbyt mi`l$8.";
    else if ( ap > -350 ) msg = "$N lubi k`lama`c, kra`s`c i w og`ole to nic mi`lego.";
    else if ( ap > -700 ) msg = "$N jest okropn$8!";
    else msg = "Nie chcia`lbym niczego m`owi`c o $%!";

    act( ZM_ZAWSZE, msg, ch, NULL, victim, TO_CHAR );

    return TRUE;
}


CZAR( spell_laser_eyes )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    /* lepiej od wybuchu plazmowego, gorzej od strzaly ksiezyca */
    dam = dice( level, 12 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_lightning_bolt )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
		 int        dam;
    static const int        dam_each[ ] =
    {
	 0,
	 0,  0,  0,  0,  0,      0,  0,  0, 25, 28,
	31, 34, 37, 40, 40,     41, 42, 42, 43, 44,
	44, 45, 46, 46, 47,     48, 48, 49, 50, 50,
	51, 52, 52, 53, 54,     54, 55, 56, 56, 57,
	58, 58, 59, 60, 60,     61, 62, 62, 63, 64,
	64, 65, 65, 66, 66,	67, 67, 68, 68, 69,
	70, 70, 71, 71, 72,	72, 73, 73, 74, 75,
	76, 76, 77, 77, 78,	78, 78, 79, 79, 80,
	81, 81, 82, 82, 83,	83, 83, 84, 84, 85,
	86, 86, 87, 87, 88,	88, 89, 90, 91, 92
    };

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_locate_object )
{
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    char      buf[ MAX_INPUT_LENGTH ];
    char      long_buf[ MAX_INPUT_LENGTH * 101 ] = "\0";
    int       licznik = 0;

    if ( !*target_name )
    {
	STC( "Musisz poda`c nazw`e szukanego przedmiotu.\n\r", ch );
	return FALSE;
    }

    for ( obj = object_list; obj; obj = obj->next )
    {
	if ( obj->deleted || !can_see_obj( ch, obj )
	  || !is_name( target_name, obj->name )
	  || IS_OBJ_STAT( obj, ITEM_NO_LOCATE ) )
	{
	    continue;
	}


	for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by )
	{
	    if ( !can_see( ch, in_obj->carried_by ) )
		continue;

	    sprintf( buf, "%s noszon%c przez %s.\n\r",
		    obj->short_descr,
		    obj->rodzaj == 1 ? 'y' : obj->rodzaj == 2 ? 'a' : 'e',
		    PERSB( in_obj->carried_by, ch, FALSE ) );
	}
	else
	{
	    sprintf( buf, "%s %s.\n\r",
		    obj->short_descr, !in_obj->in_room
		    ? "w przechowalni" : in_obj->in_room->miejscownik );
	}

	buf[ 0 ] = UPPER( buf[ 0 ] );
	if ( buf[ 0 ] == '`' )
	    buf[ 1 ] = UPPER( buf[ 1 ] );
	licznik++;
	strcat( long_buf, buf );
	if ( licznik == 100 ) break;
    }
    if ( licznik == 0 )
	send_to_char( "Nie ma niczego takiego!\n\r", ch );
    else
    {
	if ( licznik == 100 )
	    strcat( long_buf, "{ROgraniczam do pierwszych 100 wyst`apie`n.{x\n\r" );

	send_to_char( long_buf, ch );
    }

    return TRUE;
}


CZAR( spell_magic_missile )
{
		 CHAR_DATA *victim      = (CHAR_DATA *) vo;
		 int       dam;
    static const int       dam_each[ ] =
    {
	 0,
	 3,  3,  4,  4,  5,      6,  6,  6,  6,  6,
	 7,  7,  7,  7,  7,      8,  8,  8,  8,  8,
	 9,  9,  9,  9,  9,     10, 10, 10, 10, 10,
	11, 11, 11, 11, 11,     12, 12, 12, 12, 12,
	13, 13, 13, 13, 13,     14, 14, 14, 14, 14,
	15, 15, 15, 15, 15,	16, 16, 16, 16, 16,
	17, 17, 17, 17, 17,	18, 18, 18, 18, 18,
	19, 19, 19, 20, 20,	20, 21, 21, 21, 21,
	22, 22, 23, 23, 23,	24, 24, 24, 25, 25,
	26, 26, 26, 27, 27,	28, 28, 29, 29, 30
    };

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_mass_protection )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;
    bool czy = FALSE;

    send_to_char( "Rzucasz ochron`e na swoj`a grup`e.\n\r", ch );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch )
	  || ( HAS_PROTECTION( gch ) && !is_affected( gch, sn ) ) )
	{
	    continue;
	}

	af.type     = sn;
	af.duration = 10 + level / 2;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_PROTECT );

	if ( !affect_join( gch, &af, ENH_MAX ) )
	    continue;

	czy = TRUE;
	if ( !wzmocnienie_affect_join )
	{
	    act( ZM_PRZYT, "Odczuwasz otaczaj`ac`a ci`e magiczn`a barier`e.", gch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n zostaje obj`et$y magiczn`a ochron`a.", gch, NULL, NULL, TO_ROOM );
	}

    }

    return czy;
}


CZAR( spell_mass_shield )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;
    bool czy = FALSE;

    send_to_char( "Rzucasz magiczn`a tarcz`e na swoj`a grup`e.\n\r", ch );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) )
	    continue;

	af.type     = sn;
	af.duration = 8 + level;
	af.location = APPLY_AC;
	af.modifier = 30;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	if ( !affect_join( gch, &af, ENH_MAX ) )
	    continue;

	czy = TRUE;
	if ( !wzmocnienie_affect_join )
	{
	    act( ZM_PRZYT, "Otacza ci`e magiczna tarcza.", gch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n jest otoczon$y magiczn`a tarcz`a.", gch, NULL, NULL, TO_ROOM );
	}
    }

    return czy;
}


CZAR( spell_mass_heal )
{
    CHAR_DATA *gch;
    int        heal;

    send_to_char( "Rzucasz leczenie na swoj`a grup`e.\n\r", ch );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch )
	  || IS_AFFECTED( gch, AFF_DEGENERACJA )
	  || gch->hit >= gch->max_hit )
	{
	    continue;
	}

	heal = dice( 17, 4 ) + level;
	gch->hit = UMIN( gch->hit + heal, UMAX( gch->hit, gch->max_hit ) );
	update_pos( gch );

	if ( gch->hit == gch->max_hit )
	    ASTC( "Odzyska`l$o`s pe`lni`e zdrowia.", gch );
	else
	    act( ZM_PRZYT, "Czujesz si`e lepiej!", gch, NULL, NULL, TO_CHAR );
    }

    return TRUE;
}


CZAR( spell_mass_invis )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;
    bool czy = FALSE;
    bool komunikat;

    send_to_char( "Rzucasz niewidzialno`s`c na swoj`a grup`e.\n\r", ch );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch )
	  || ( IS_AFFECTED( gch, AFF_INVISIBLE ) && !is_affected( gch, sn ) ) )
	{
	    continue;
	}

	af.type     = sn;
	af.duration = 24;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_INVISIBLE );
	komunikat = !IS_AFFECTED( gch, AFF_INVISIBLE );

	if ( !affect_join( gch, &af, ENH_MAX ) )
	    continue;

	czy = TRUE;
	if ( komunikat )
	{
	    act( ZM_WZROK, "Znikasz w nico`sci.", gch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n znika w nico`sci.", gch, NULL, NULL, TO_ROOM );
	}
    }

    return czy;
}


CZAR( spell_mass_refresh )
{
    CHAR_DATA *gch;
    bool czy = FALSE;

    send_to_char( "Pr`obujesz nieco odnowi`c swoj`a grup`e.\n\r", ch );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch )
	  || IS_AFFECTED( gch, AFF_DEGENERACJA )
	  || gch->move >= gch->max_move )
	    continue;

	gch->move = UMIN( gch->move + level, UMAX( gch->move, gch->max_move ) );

	act( ZM_PRZYT, "Czujesz si`e mniej zm`eczon$y.", gch, NULL, NULL, TO_CHAR );
	czy = TRUE;
    }

    return czy;
}


CZAR( spell_moon_arrow )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    if ( !IS_OUTSIDE( ch )
      || ( ch->in_room->area->weather_info.sunlight != SUN_SET
	&& ch->in_room->area->weather_info.sunlight != SUN_DARK )
      || ch->in_room->area->weather_info.sky == SKY_LIGHTNING )
    {
	send_to_char( "Nie widzisz ksi`e`zyca.\n\r", ch );
	return FALSE;
    }

    dam = dice( level, 13 ); /* naprawde mocny czar */
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_sunbeam )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    if ( !IS_OUTSIDE( ch )
       || ( ch->in_room->area->weather_info.sunlight != SUN_RISE
	 && ch->in_room->area->weather_info.sunlight != SUN_LIGHT )
       || ch->in_room->area->weather_info.sky == SKY_LIGHTNING )
    {
	send_to_char( "Do tego potrzebne ci s`lo`nce.\n\r", ch );
	return FALSE;
    }

    dam = dice( level, 12 ) + level; /* naprawde mocny czar */
    if ( CZY_WAMPIR( victim ) )
	dam += 3 * level; /* wampiry bardzo, bardzo boli slonce */
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_mute )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_MUTE( victim ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level;
    af.location = 0;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_MUTE );
    affect_to_char( victim, &af );

    if ( ch == victim )
    {
	act( ZM_ZAWSZE, "Wyciszy`l$o`s si`e!", ch, NULL, NULL, TO_CHAR );
	act( ZM_SLUCH | ZM_WID_CHAR, "$n si`e wyciszy`l$a!", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
	act( ZM_ZAWSZE, "Wyciszy`l$o`s $#!", ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "$n ci`e wyciszy`l$a!", ch, NULL, victim, TO_VICT );
	act( ZM_SLUCH | ZM_WID_VICT, "$n wyciszy`l$a $#!", ch, NULL, victim, TO_NOTVICT );
    }

    return TRUE;
}


SPADANIE( wear_off_mute )
{
    if ( !IS_MUTE( ch ) )
	act( ZM_PRZYT, "Znowu mo`zesz m`owi`c.", ch, NULL, NULL, TO_CHAR );

    return;
}


CZAR( spell_null )
{
    send_to_char( "To nie jest czar!\n\r", ch );
    bug( "Wywolanie spell_null!", 0 );

    return FALSE;
}


CZAR( spell_pass_door )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_PASSING( victim )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = number_fuzzy( level / 4 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_PASS_DOOR );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_PRZYT, "Stajesz si`e bardzo elastyczn$y.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n staje si`e bardzo elastyczn$y.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_plasma_blast )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = dice( level, 11 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_poison )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( saves_spell( level, victim )
      || CZY_WAMPIR( victim )
      || IS_SET( race_table[ victim->race ].race_abilities, RACE_POISONIMM ) )
    {
	STC( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_STR;
    af.modifier = - level / 40 - 1;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_POISON );
    affect_join( victim, &af, ENH_AFFECT );

    if ( ch != victim )
	send_to_char( "Rzucasz otrucie.\n\r", ch );

    act( ZM_PRZYT, "Czujesz si`e bardzo `xle.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_polymorph_other )
{
    CHAR_DATA   *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[ MAX_STRING_LENGTH ];
    int i;

    if ( ch != victim
      && saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    if ( IS_AFFECTED( victim, AFF_POLYMORPH ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    /* Qwert: koniec z przemianami rzucanymi na sklepikarzy */
    if ( IS_NPC( victim ) && victim->pIndexData->pShop )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    affect_strip( victim, gsn_postac_nietoperza );

    af.type     = sn;
    af.duration = dice( 5, level ); /* Lam: bylo 10 */
    af.location = APPLY_RACE;
    af.level    = level;
    af.caster   = ch;

    i = 500;
    do
    {
	af.modifier = number_range( 0, MAX_RACE - 1 ) - victim->race;
	if ( --i == 0 )
	{
	    NIEUDANE( ch );
	    return FALSE;
	}
    }
    while ( af.modifier == 0 );

    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_POLYMORPH );
    affect_to_char( victim, &af );
    if ( ch != victim )
	send_to_char( "Rzucasz przemian`e.\n\r", ch );
    act( ZM_PRZYT, "Czujesz si`e inaczej.", victim, NULL, NULL, TO_CHAR );
    if ( CZY_WAMPIR( victim ) )
	victim->alignment = UMIN( victim->alignment, 0 );
    /* check_race_wearlocs( victim ); *//* Lam 7.6.99 */
    sprawdz_ubranie( victim );
    check_airchar( victim );

    if ( !IS_NPC( ch ) )
    {
	sprintf( buf, "Log %s: PRZEMIENIL %s (%s).",
		 ch->name,
		 victim->biernik,
		 race_table[ victim->race ].name );
	log_string_ch( ch, buf );
    }

    return TRUE;
}


CZAR( spell_protection )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( HAS_PROTECTION( victim )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 24;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_PROTECT );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    if ( ch != victim )
	send_to_char( "Rzucasz ochron`e.\n\r", ch );

    act( ZM_PRZYT, "Czujesz si`e chronion$y.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_portal )
{
    OBJ_DATA *portl;
    OBJ_DATA *portl2;
    char *name;
    int vnum = PRZYWOLANIE_P( ch ); /* Lam 21.9.2004: dodane _P */
    char buf[ MAX_STRING_LENGTH ];
    int glevel;

    if ( !ch->in_room )
	return FALSE;

    if ( ch->in_room->vnum == vnum )
    {
	send_to_char( "To bez sensu.\n\r", ch );
	return FALSE;
    }

    if ( ch->in_room->area == get_room_index( vnum )->area )
    {
	send_to_char( "W obr`ebie miasta nie musisz tworzy`c portali do miasta, nieprawda`z?\n\r", ch );
	return FALSE;
    }

    if ( !strefa_nalezy( STREFA( ch ), ch->in_room->area->strefy ) )
    {
	send_to_char( "Jeste`s zbyt daleko od domu.\n\r", ch );
	return FALSE;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
      || IS_SET( ch->in_room->room_flags, ROOM_NO_SUMMON ) )
    {
	send_to_char( "Powietrze zaczyna iskrzy`c od twej magii, ale portal si`e nie pojawia.\n\r", ch );
	return FALSE;
    }

    name = ch->dopelniacz;

    portl = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    glevel = ch->level - 5;
    if ( ch->level >= LEVEL_HERO )
    {
	glevel = UMAX( glevel, LEVEL_HERO );
	if ( ch->level >= LEVEL_IMMORTAL )
	    glevel = UMAX( glevel, LEVEL_IMMORTAL );
    }
    portl->value[ 0 ].v = glevel;
    portl->value[ 2 ].v = IS_NPC( ch ) || usage != USAGE_CAST
	? UMAX( 0, 20 - level / 4 )
	: UMAX( 0, 20 - level * ch->pcdata->learned[ sn ] / 250 );
    portl->timer = 24;

    /* Alandar */
    sprintf( buf, "%s %s", portl->name, ch->name );
    free_string( portl->name );
    portl->name = str_dup( buf );

    sprintf( buf, portl->short_descr, name );
    free_string( portl->short_descr );
    portl->short_descr = str_dup( buf );

    sprintf( buf, portl->dopelniacz, name );
    free_string( portl->dopelniacz );
    portl->dopelniacz = str_dup( buf );

    sprintf( buf, portl->celownik, name );
    free_string( portl->celownik );
    portl->celownik = str_dup( buf );

    sprintf( buf, portl->biernik, name );
    free_string( portl->biernik );
    portl->biernik = str_dup( buf );

    sprintf( buf, portl->narzednik, name );
    free_string( portl->narzednik );
    portl->narzednik = str_dup( buf );

    sprintf( buf, portl->miejscownik, name );
    free_string( portl->miejscownik );
    portl->miejscownik = str_dup( buf );

    sprintf( buf, portl->description, ch->biernik );
    free_string( portl->description );
    portl->description = str_dup( buf );
    portl->value[ 1 ].v = vnum;

    obj_to_room( portl, ch->in_room );

    portl2 = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    portl2->value[ 0 ].v = glevel;
    portl2->value[ 2 ].v = IS_NPC( ch ) || usage != USAGE_CAST
	? UMAX( 0, 20 - level / 4 )
	: UMAX( 0, 20 - level * ch->pcdata->learned[ sn ] / 250 );
    portl2->timer = 24;

    /* Alandar */
    sprintf( buf, "%s %s", portl2->name, ch->name );
    free_string( portl2->name );
    portl2->name = str_dup( buf );

    sprintf( buf, portl2->short_descr, name );
    free_string( portl2->short_descr );
    portl2->short_descr = str_dup( buf );

    sprintf( buf, portl2->dopelniacz, name );
    free_string( portl2->dopelniacz );
    portl2->dopelniacz = str_dup( buf );

    sprintf( buf, portl2->celownik, name );
    free_string( portl2->celownik );
    portl2->celownik = str_dup( buf );

    sprintf( buf, portl2->biernik, name );
    free_string( portl2->biernik );
    portl2->biernik = str_dup( buf );

    sprintf( buf, portl2->narzednik, name );
    free_string( portl2->narzednik );
    portl2->narzednik = str_dup( buf );

    sprintf( buf, portl2->miejscownik, name );
    free_string( portl2->miejscownik );
    portl2->miejscownik = str_dup( buf );

    sprintf( buf, portl2->description, ch->biernik );
    free_string( portl2->description );
    portl2->description = str_dup( buf );

    portl2->value[ 1 ].v = ch->in_room->vnum;

    obj_to_room( portl2, get_room_index( vnum ) );

    send_to_char( "Tworzysz nowy portal.\n\r", ch );
    act( ZM_WZROK, "$n tworzy nowy portal.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}


/* Lam 22.3.98 */
CZAR( spell_ray_of_truth )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    ROOM_INDEX_DATA *room = ch->in_room;

    if ( IS_EVIL( ch ) )
    {
	victim = ch;
	send_to_char( "Pozytywna energia obraca si`e przeciw tobie!\n\r", ch );
    }

    if ( victim != ch )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "$n unosi r`ek`e, z kt`orej strzela o`slepiaj`acy promie`n `swiat`la.",
		ch, NULL, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Widzisz o`slepiaj`acy b`lysk.",
		ch, NULL, NULL, TO_ROOM );
	act( ZM_WZROK, "Unosisz r`ek`e, z kt`orej strzela o`slepiaj`acy promie`n `swiat`la.",
		ch, NULL, NULL, TO_CHAR );
	act( ZM_ZAWSZE | ZM_W_WZROK, "Unosisz r`ek`e.", ch, NULL, NULL, TO_CHAR );
    }

    if ( IS_GOOD( victim ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "`Swiat`lo nie wyrz`adza $2 szkody.", victim, NULL, NULL, TO_ROOM );
	act( ZM_WZROK, "`Swiat`lo nie wyrz`adza ci szkody.", victim, NULL, NULL, TO_CHAR );
	return FALSE;
    }

    dam = dice( level, 10 ); /* lepiej niz wybuch kwasu, prawie wybuch plazmowy */

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    if ( !victim->deleted    /* Ulryk: to nie bylo sprawdzane! */
      && room == ch->in_room /* Lam: ani to */
      && room == victim->in_room
      && !number_bits( 2 ) ) /* 25% szans na oslepniecie od swiatla */
    {
	spell_blindness( gsn_blindness, level / 2, ch, victim, USAGE_RECUR );
    }

    return TRUE;
}


CZAR( spell_recharge_item )
{
    int learned = 80;

    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( usage == USAGE_CAST && IS_NPC( ch ) )
    {
	send_to_char( "Chwilowo niedost`epne mobom.\n\r", ch );
	return FALSE;
    }

    if ( obj->item_type != ITEM_WAND
      && obj->item_type != ITEM_STAFF )
    {
	send_to_char( "Ten przedmiot nie mo`ze by`c na`ladowany.\n\r", ch );
	return FALSE;
    }

    if ( usage == USAGE_CAST )
	learned = ch->pcdata->learned[ sn ];

    if ( number_percent( ) <
		learned / 2 + 3 * ( ch->level - obj->value[ 0 ].v )
	&& obj->value[ 2 ].v < obj->value[ 1 ].v )
    {
	obj->value[ 2 ].v = obj->value[ 1 ].v;
	obj->cost = 1;
	if ( obj->rodzaj == 3 )
	{
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozb`lyskaj`a.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozb`lyskaj`a.", ch, obj, NULL, TO_ROOM );
	}
	else
	{
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozb`lyska.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozb`lyska.", ch, obj, NULL, TO_ROOM );
	}
    }
    else
    {
	if ( obj->rodzaj == 3 )
	{
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozb`lyskaj`a i nagle rozpadaj`a si`e na kawa`lki!",
		ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozb`lyskaj`a i nagle rozpadaj`a si`e na kawa`lki!",
		ch, obj, NULL, TO_ROOM );
	}
	else
	{
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozb`lyska i nagle rozpada si`e na kawa`lki!",
		ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p rozb`lyska i nagle rozpada si`e na kawa`lki!",
		ch, obj, NULL, TO_ROOM );
	}
	extract_obj( obj );

	damage( ch, ch, ch->max_hit / 16, sn, WEAR_NONE, TRUE );
    }

    return TRUE;
}


CZAR( spell_refresh )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( victim->move >= victim->max_move )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    victim->move = UMIN( victim->move + level,
		   UMAX( victim->max_move, victim->move ) );

    if ( ch != victim )
	send_to_char( "Zmniejszasz zm`eczenie.\n\r", ch );

    act( ZM_PRZYT, "Czujesz si`e mniej zm`eczon$y.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


/* Remove Alignment by Thelonius for EnvyMud */
CZAR( spell_remove_alignment )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    char *str;

    if ( usage == USAGE_CAST && IS_NPC( ch ) )
    {
	send_to_char( "Chwilowo niedost`epne mobom.\n\r", ch );
	return FALSE;
    }

    if ( !IS_OBJ_STAT( obj, ITEM_EVIL )
      && !IS_OBJ_STAT( obj, ITEM_ANTI_GOOD )
      && !IS_OBJ_STAT( obj, ITEM_ANTI_EVIL )
      && !IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ) )
    {
	send_to_char( "Nic si`e nie zdarzy`lo.\n\r", ch );
	return FALSE;
    }

    if ( ( usage != USAGE_CAST ) || ( number_percent( ) <= ch->pcdata->learned[ sn ] ) )
    {
	REMOVE_BIT_V( obj->extra_flags, ITEM_EVIL );
	REMOVE_BIT_V( obj->extra_flags, ITEM_ANTI_GOOD );
	REMOVE_BIT_V( obj->extra_flags, ITEM_ANTI_EVIL );
	REMOVE_BIT_V( obj->extra_flags, ITEM_ANTI_NEUTRAL );
	if ( obj->rodzaj == 3 )
	    str = "$p chwilk`e brz`ecz`a, po czym cichn`a.";
	else
	    str = "$p chwilk`e brz`eczy, po czym cichnie.";
	act( ZM_SLUCH | ZM_WID_OBJ1, str, ch, obj, NULL, TO_CHAR );
	act( ZM_SLUCH | ZM_WID_OBJ1, str, ch, obj, NULL, TO_ROOM );
	act( ZM_SLUCH | ZM_W_WID_OBJ1, "Przez chwil`e s`lyszysz ciche brz`eczenie.", ch, obj, NULL, TO_CHAR );
	act( ZM_SLUCH | ZM_W_WID_OBJ1, "Przez chwil`e s`lyszysz ciche brz`eczenie.", ch, obj, NULL, TO_ROOM );
	return TRUE;
    }

    SET_BIT_V( obj->extra_flags, ITEM_NODROP );
    obj->wear_flags = ITEM_TAKE;                /* Useless   */
    obj->cost       = 0;                        /* Worthless */
    switch ( obj->rodzaj )
    {
	case 1: str= "$p rozb`lyskuje jasno, po czym staje si`e szary.";
		break;
	case 2:	str = "$p rozb`lyskuje jasno, po czym staje si`e szara.";
		break;
	case 3: str = "$p rozb`lyskuj`a jasno, po czym staj`a si`e szare.";
		break;
	default:str = "$p rozb`lyskuje jasno, po czym staje si`e szare.";
		break;
    }
    act( ZM_WZROK | ZM_WID_OBJ1, str, ch, obj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_OBJ1, str, ch, obj, NULL, TO_ROOM );

    return FALSE; /* Lam 15.4.2002 TRUE -> FALSE */
}


/* Expulsion of ITEM_NOREMOVE addition by Katrina */
CZAR( spell_remove_curse )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        iWear;
    bool       yesno  = FALSE;

    for ( iWear = 0; iWear < MAX_WEAR; iWear ++ )
    {
	if ( !( obj = get_eq_char( victim, iWear ) ) )
	    continue;

	if ( IS_OBJ_STAT( obj, ITEM_NOREMOVE )
	  && !IS_OBJ_STAT( obj, ITEM_LOYAL )
	  && !IS_OBJ_STAT( obj, ITEM_QUEST ) )
	{
	    /* jesli musi upuscic jedzenie, to musi przestac karmic */
	    if ( iWear == WEAR_HOLD )
		PRZERWIJ_KARMPOJ( victim, FALSE );
	    unequip_char( victim, obj );
	    obj_from_char( obj );
	    obj_to_room( obj, victim->in_room );
	    act( ZM_PRZYT, "Rzucasz $j na ziemi`e.",  victim, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n rzuca $j na ziemi`e.", victim, obj, NULL, TO_ROOM );
	    if ( obj->rodzaj == 3 )
		act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawiaj`a si`e tu $p.", victim, obj, NULL, TO_ROOM );
	    else
		act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", victim, obj, NULL, TO_ROOM );
	    yesno = TRUE;
	}
	if ( IS_OBJ_STAT( obj, ITEM_NOREMOVE )
	  && ( IS_OBJ_STAT( obj, ITEM_LOYAL )
	    || IS_OBJ_STAT( obj, ITEM_QUEST ) ) )
	{
	    /* jesli musi upuscic jedzenie, to musi przestac karmic */
	    if ( iWear == WEAR_HOLD )
		PRZERWIJ_KARMPOJ( victim, FALSE );
	    unequip_char( victim, obj );
	    act( ZM_PRZYT, "Zrzucasz $j z siebie.",  victim, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zrzuca $j z siebie.", victim, obj, NULL, TO_ROOM );
	    yesno = TRUE;
	}
    }

    if ( is_affected( victim, gsn_curse ) )
    {
	affect_strip( victim, gsn_curse );
	act( ZM_PRZYT, "Czujesz si`e pewniej.", victim, NULL, NULL, TO_CHAR );
	yesno = TRUE;
    }

    if ( is_affected( victim, gsn_pajeczyna ) )
    {
	affect_strip( victim, gsn_pajeczyna );
	ASTC( "Znowu jeste`s woln$y.", victim );
	yesno = TRUE;
    }

    if ( ch != victim && yesno )
	send_to_char( "Zdejmujesz kl`atw`e.\n\r", ch );
    else if ( !yesno )
	send_to_char( "Nie by`lo kl`atw do zdj`ecia.\n\r", ch );

    return yesno;
}


/*
 * Lam
 */
CZAR( spell_remove_invis )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Chwilowo niedost`epne mobom.\n\r", ch );
	return FALSE;
    }

    if ( *target_name == '\0' )
    {
	send_to_char( "Na co chcesz rzuci`c ten czar?\n\r", ch );
	return FALSE;
    }

    obj = get_obj_carry( ch, target_name );

    if ( obj )
    {
	if ( !IS_OBJ_STAT( obj, ITEM_INVIS ) )
	{
	    NIEUDANE_BC( ch );
	    return FALSE;
	}

	REMOVE_BIT_V( obj->extra_flags, ITEM_INVIS );
	if ( obj->rodzaj == 3 )
	    act( ZM_ZAWSZE, "$p nabieraj`a kszta`lt`ow.", ch, obj, NULL, TO_CHAR );
	else
	    act( ZM_ZAWSZE, "$p nabiera kszta`lt`ow.", ch, obj, NULL, TO_CHAR );
	return TRUE;
    }

    victim = get_char_room( ch, target_name );

    if ( !victim || !can_see( ch, victim ) )
    {
	send_to_char( "Nie znajdujesz tu niczego/nikogo takiego.\n\r", ch );
	return FALSE;
    }

    if ( !IS_AFFECTED( victim, AFF_INVISIBLE ) )
    {
	act( ZM_ZAWSZE, "$N nie jest niewidzialn$Y.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    if ( !IS_NPC( victim )
      && !IS_NPC( ch )
      && usage == USAGE_CAST
      && ( number_percent( ) > ch->pcdata->learned[ sn ]
	|| victim->level > ch->level + 8 ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    affect_strip( victim, gsn_invis );
    affect_strip( victim, gsn_mass_invis );
    affect_strip( victim, gsn_sneak );
    affect_strip( victim, gsn_shadow );
    REMOVE_BIT_V( victim->affected_by, AFF_INVISIBLE );
    REMOVE_BIT_V( victim->affected_by, AFF_HIDE );
    REMOVE_BIT_V( victim->affected_by, AFF_SNEAK );

    act( ZM_PRZYT, "Czujesz, `ze ka`zdy mo`ze ci`e zobaczy`c.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zosta`l$a zdemaskowan$y!", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_remove_silence )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    /*
     *  Note: because this spell is declared as TAR_CHAR_DEFENSIVE,
     *  do_cast will set vo = ch with no argument.  Since it is
     *  impossible for a silenced character to cast anyway, if
     *  victim == ch, that means there was no argument supplied
     *  (or the spellcaster tried to specify themself), i.e., trying
     *  to remove a "Cone of Silence" from ch->in_room.
     *  This might seem pointless, but it will work within obj_cast_spell.
     *  Thelonius (Monk) 5/94
     */
    if ( victim == ch )
    {
	ROOM_INDEX_DATA *pRoomIndex;
	bool             DidSomething;

	pRoomIndex   = ch->in_room;
	DidSomething = FALSE;

	if ( room_is_affected( pRoomIndex, gsn_cone_of_silence ) )
	{
	    affect_from_room( pRoomIndex, gsn_cone_of_silence );
	    send_to_char( "Znosisz stref`e ciszy!\n\r", ch );
	    act( ZM_SLUCH, "$n znosi stref`e ciszy!", ch, NULL, NULL, TO_ROOM );
	    DidSomething = TRUE;
	}

	if ( IS_SET( pRoomIndex->room_flags, ROOM_CONE_OF_SILENCE ) )
	{
	    REMOVE_BIT( pRoomIndex->room_flags, ROOM_CONE_OF_SILENCE );
	    send_to_char( "Znosisz stref`e ciszy!\n\r", ch );
	    act( ZM_SLUCH, "$n znosi stref`e ciszy!", ch, NULL, NULL, TO_ROOM );
	    DidSomething = TRUE;
	}

	if ( IS_AFFECTED( victim, AFF_MUTE ) )
	{
	    affect_strip( victim, gsn_mute );
	    act( ZM_SLUCH, "Mo`zesz ju`z m`owi`c.", ch, NULL, NULL, TO_CHAR );
	    act( ZM_SLUCH | ZM_WID_CHAR, "$n mo`ze ju`z m`owi`c.", ch, NULL, NULL, TO_ROOM );
	    DidSomething = TRUE;
	}

	if ( !DidSomething )
	    send_to_char( "Fzzzzzzzzz...\n\r", ch );

	return DidSomething;
    }

    if ( IS_AFFECTED( victim, AFF_MUTE ) )
    {
	affect_strip( victim, gsn_mute );
	act( ZM_ZAWSZE, "$N mo`ze ju`z m`owi`c.",
		ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "Dzi`eki $2 mo`zesz ju`z m`owi`c.",
		ch, NULL, victim, TO_VICT );
	act( ZM_SLUCH | ZM_WID_VICT, "Dzi`eki $2 $N mo`ze ju`z m`owi`c.",
		ch, NULL, victim, TO_NOTVICT );
	return TRUE;
    }
    else
	NIEUDANE_BC( ch );

    return FALSE;
}


CZAR( spell_sanctuary )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( HAS_SANCTUARY( victim )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = number_fuzzy( level / 2 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_SANCTUARY );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_WZROK, "Otacza ci`e bia`la aura.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n jest otoczon$y bia`l`a aur`a.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_shield )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = 8 + level;
    af.location = APPLY_AC;
    af.modifier = 20;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_PRZYT, "Otacza ci`e magiczna tarcza.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_DYLEMAT, "$n jest otoczon$y magiczn`a tarcz`a.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_shocking_grasp )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const int dam_each[ ] =
    {
	 0,
	 0,  0,  0,  0,  0,      0, 20, 25, 29, 33,
	36, 39, 39, 39, 40,     40, 41, 41, 42, 42,
	43, 43, 44, 44, 45,     45, 46, 46, 47, 47,
	48, 48, 49, 49, 50,     50, 51, 51, 52, 52,
	53, 53, 54, 54, 55,     55, 56, 56, 57, 57,
	58, 58, 59, 59, 60,	60, 61, 61, 62, 62,
	63, 63, 64, 64, 65,	65, 66, 66, 67, 67,
	68, 68, 69, 69, 70,	70, 71, 71, 72, 72,
	73, 73, 74, 74, 75,	75, 76, 76, 77, 77,
	78, 78, 79, 79, 80,	80, 81, 81, 82, 82
    };
    int dam;

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_sleep )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_SLEEP ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( level < victim->level
      || saves_spell( level, victim )
      || CZY_WAMPIR( victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 4 + level / 2;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_SLEEP );
    affect_join( victim, &af, ADD_AFFECT );

    if ( ch != victim )
	send_to_char( "Rzucasz u`spienie.\n\r", ch );

    if ( IS_AWAKE( victim ) )
    {
	send_to_char( "Chce ci si`e spa`c... Chrrr...\n\r", victim );
	if ( victim->position == POS_FIGHTING || ch->fighting )
	   stop_fighting( victim, TRUE );
	do_sleep( victim, "" );
    }

    return TRUE;
}


CZAR( spell_stone_skin )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = level / 2; /* nie bylo podzielone */
    af.location = APPLY_AC;
    af.modifier = 40;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_PRZYT, "Twoja sk`ora zamienia si`e w kamie`n.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "Sk`ora $1 zamienia si`e w kamie`n.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


/*
 * Lam 19.5.98: summon caly czas kwasil, bo bylo get_char_world, a nie
 * get_char_area, naprawilem.
 *
 * Qwert 27.2.02: latwiejsze sciaganie zabojcow
 */
CZAR( spell_summon )
{
    CHAR_DATA *victim;
    bool newif = TRUE;

    if ( !*target_name )
    {
	send_to_char( "Musisz poda`c imi`e postaci, kt`or`a chcesz przywo`la`c.\n\r", ch );
	return FALSE;
    }

    if ( !( victim = get_char_area( ch, target_name ) ) )
    {
	send_to_char( "Nie ma takiej postaci w okolicy.\n\r", ch );
	return FALSE;
    }

    if ( victim == ch )
    {
	send_to_char( "Nie mo`zesz by`c jeszcze bli`zej siebie.\n\r", ch );
	return FALSE;
    }

    if ( victim->in_room == ch->in_room )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( !victim->fighting
      && IS_SET( victim->act, PLR_ZABOJCA )
      && ( victim->level <= level + 5
	|| CZY_WAMPIR( victim )
	|| is_name2( victim->name, ch->pcdata->nienawisc ) ) )
    {
	newif = FALSE; /* duzo latwiej sciagnac */
    }

    if ( !victim->in_room
      || PP( victim )
      || ch->in_room->trap
      || !can_enter_room( victim, ch->in_room, FALSE )
      || IS_SET( ch->in_room->room_flags, ROOM_NO_SUMMON )
      || ( newif
	&& ( IS_SET( victim->in_room->room_flags, ROOM_SAFE )
	  || ( IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL )
	    != IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL ) )
	  || IS_SET( victim->in_room->room_flags, ROOM_NO_SUMMON )
	  || IS_AFFECTED( victim, AFF_CURSE )
	  || victim->level >= level + 3
	  || victim->fighting
	  || ( IS_NPC( victim ) && saves_spell( level, victim ) ) ) ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    PRZERWIJ_KOPANIE( victim, FALSE );
    PRZERWIJ_KARMPOJ( victim, FALSE );

    act( ZM_WZROK | ZM_WID_CHAR, "$n znika nagle przywo`lan$y magi`a.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( ZM_PRZYT, "$n przywo`la`l$a ci`e do siebie!", ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_CHAR, "$n pojawia si`e nagle przywo`lan$y magi`a.", victim, NULL, NULL, TO_ROOM );

    if ( IS_AWAKE( victim ) )
	do_look( victim, "auto" );
    else
	send_to_char( "Wiercisz si`e przez sen.\n\r", victim );

    return TRUE;
}


void wlasciwa_teleportacja( CHAR_DATA *caster, CHAR_DATA *victim, ROOM_INDEX_DATA *pRoomIndex )
{
    bool slonce_przed, slonce_po; /* Lam 27.1.2006, #1166 */
    bool tlen_przed, tlen_po; /* Lam 27.1.2006, #1166 */

    slonce_przed = CZY_PALACE_SLONCE( victim );
    tlen_przed = CZY_BRAK_TLENU( victim );

    if ( victim->fighting )
	stop_fighting( victim, TRUE );
    act( ZM_WZROK | ZM_WID_CHAR, "$n powoli zapada si`e w niebyt.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act( ZM_WZROK | ZM_WID_CHAR, "$n powraca z niebytu.", victim, NULL, NULL, TO_ROOM );
    if ( victim == caster )
	send_to_char( "{RTeleportujesz si`e!{x\n\r", victim );
    else
	send_to_char( "{RPadasz ofiar`a magicznej teleportacji!{x\n\r", victim );
    do_look( victim, "auto" );

    slonce_po = CZY_PALACE_SLONCE( victim );
    if ( slonce_przed != slonce_po )
    {
	if ( slonce_po )
	    STC( "{Y`Swiat`lo pali twoj`a sk`or`e!{x\n\r", victim );
	else
	    STC( "`Swiat`lo przestaje ci`e parzy`c.\n\r", victim );
    }

    tlen_po = CZY_BRAK_TLENU( victim );
    if ( tlen_przed != tlen_po )
    {
	if ( tlen_po )
	    STC( "{RNie mo`zesz oddycha`c!{x\n\r", victim );
	else
	    STC( "`Lapiesz oddech.\n\r", victim );
    }

    return;
}


CZAR( spell_teleport )
{
    CHAR_DATA       *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *pRoomIndex;

    if ( !victim->in_room
      || ( IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL )
	&& victim->level < LEVEL_IMMORTAL )
      || ( !IS_NPC( ch ) && victim->fighting )
      || ( victim != ch
	&& usage != USAGE_GAME /* wygrana w kole fortuny, zawsze wychodzi */
	&& ( saves_spell( level, victim )
	  || saves_spell( level, victim ) ) ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    for ( ; ; )
    {
	/* teoretycznie moze sie okazac, ze nigdy nie wyjdzie z tej petli... */
	pRoomIndex = get_room_index( number_range( 0, 65535 ) );
	if ( pRoomIndex )
	    if ( !room_is_private( pRoomIndex )
	      && !pRoomIndex->trap /* zabezpieczenie przed pulapkami */
	      && !IS_SET( pRoomIndex->room_flags, ROOM_NO_TELEPORT )
	      && !IS_SET( pRoomIndex->area->flags, AREA_NO_TELEPORT ) )
	{
	    break;
	}
    }
    /* Qwert: durny komunikat jest po teleporcie */
    PRZERWIJ_KOPANIE( victim, FALSE );
    PRZERWIJ_KARMPOJ( victim, FALSE );

    if ( ch->hating == victim )
    {
	wywal_zwod( &victim->zwod_hating, ch );
	ch->hating = NULL;
    }

    if ( ch->hunting == victim )
    {
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
    }

    wlasciwa_teleportacja( ch, victim, pRoomIndex );

    return TRUE;
}


/*
 * Lam: grupowa teleportacja
 */
CZAR( spell_teleport_group )
{
    CHAR_DATA       *rch, *rch_next;
    ROOM_INDEX_DATA *pRoomIndex;
    bool udane = FALSE;

    if ( !ch->in_room
      || IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL ) )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    for ( ; ; )
    {
	pRoomIndex = get_room_index( number_range( 0, 65535 ) );
	if ( pRoomIndex
	  && !room_is_private( pRoomIndex ) /* FIXME: za slabe dla grupy! */
	  && !pRoomIndex->trap
	  && !IS_SET( pRoomIndex->room_flags, ROOM_NO_TELEPORT )
	  && !IS_SET( pRoomIndex->area->flags, AREA_NO_TELEPORT ) )
	{
	    break;
	}
    }

    for ( rch = ch->in_room->people; rch; rch = rch_next )
    {
	rch_next = rch->next_in_room;

	if ( rch->deleted
	  || rch == ch
	  || !is_same_group( ch, rch )
	  || !can_see( ch, rch ) )
	{
	    continue;
	}

	/* zwykla teleportacja nie patrzy na klatwy, wiec ta tez nie */
	if ( FIGHTING( rch ) )
	{
	    act( ZM_ZAWSZE, "Nie mo`zesz zabra`c $!.", ch, NULL, rch, TO_CHAR );
	    continue;
	}

	PRZERWIJ_KOPANIE( rch, FALSE );
	PRZERWIJ_KARMPOJ( rch, FALSE );

	wlasciwa_teleportacja( ch, rch, pRoomIndex );
	udane = TRUE;
    }

    if ( !FIGHTING( ch ) ) /* jest juz POS_STANDING, ale to nie zaszkodzi */
    {
	PRZERWIJ_KOPANIE( ch, FALSE );
	PRZERWIJ_KARMPOJ( ch, FALSE );

	wlasciwa_teleportacja( ch, ch, pRoomIndex );
	udane = TRUE;
    }

    return udane;
}


CZAR( spell_ventriloquate )
{
    CHAR_DATA *vch;
    char       buf1[ MAX_STRING_LENGTH ];
    char       buf2[ MAX_STRING_LENGTH ];
    char       buf3[ MAX_STRING_LENGTH ];
    char       speaker[ MAX_INPUT_LENGTH  ];

    /* Lam 4.11.2004: uciszeni nie moga tego uzywac, ale wyciszeni lub
       przemienieni owszem */
    if ( IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE )
      || ( !IS_NPC( ch )
	&& ( IS_SET( ch->act, PLR_SILENCE )
	  || IS_SET( ch->act, PLR_NO_TELL ) ) ) )
    {
	STC( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    target_name = one_argument2( target_name, speaker );

    if ( !*target_name )
    {
	STC( "Musisz wpisa`c czyj`a mow`e udajesz oraz co dok`ladnie ma \"powiedzie`c\".\n\r", ch );
	return FALSE;
    }

    sprintf( buf1, "{q%s m`owi: \"%s{q\"{x\n\r", speaker, target_name );
    sprintf( buf2, "{qKto`s powoduje, `ze %s m`owi: \"%s{q\"{x\n\r", speaker, target_name );
    sprintf( buf3, "{q%s powoduje, `ze %s m`owi: \"%s{q\"{x\n\r", ch->short_descr, speaker, target_name );
    buf1[ 2 ] = UPPER( buf1[ 2 ] );
    buf3[ 2 ] = UPPER( buf3[ 2 ] );

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_HOLYLIGHT )
	  && can_see( vch, ch ) )
	    send_to_char( buf3, vch );
	else if ( !is_name2( speaker, vch->name ) && IS_AWAKE( vch ) )
	    send_to_char( saves_spell( level, vch ) ? buf2 : buf1, vch );
    }

    return TRUE;
}


CZAR( spell_weaken )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level / 2;
    af.location = APPLY_STR;
    af.modifier = -2;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Rzucasz os`labienie.\n\r", ch );

    act( ZM_PRZYT, "Czujesz si`e s`labsz$y.", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


/*
 * This is for muds that want scrolls of recall.
 */
CZAR( spell_word_of_recall )
{
    /* do_recall sprawdza poziom, a czar nie. Obie funkcje wywoluja funkcje
       real_recall, ktora jest wlasciwa funkcja przywolujaca. */
    real_recall( (CHAR_DATA *) vo, "" );

    return TRUE; /* i tak gracz tego nigdy nie bedzie mial */
}


/*
 * NPC spells.
 */
CZAR( spell_acid_breath )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    OBJ_DATA  *obj;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
    {
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    int iWear;

	    obj_next = obj_lose->next_content;

	    if ( obj_lose->deleted
	      || number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    case ITEM_ARMOR:
		if ( obj_lose->value[ 0 ].v > 0 )
		{
		    /* Correction on math by IvoryTiger */
		    if ( obj_lose->rodzaj == 3 )
			act( ZM_WZROK | ZM_WID_OBJ1, "$p zostaj`a prze`zarte przez kwas!",
			    victim, obj_lose, NULL, TO_CHAR );
		    else if ( obj_lose->rodzaj == 2 )
			act( ZM_WZROK | ZM_WID_OBJ1, "$p zostaje prze`zarta przez kwas!",
			    victim, obj_lose, NULL, TO_CHAR );
		    else if ( obj_lose->rodzaj == 1 )
			act( ZM_WZROK | ZM_WID_OBJ1, "$p zostaje prze`zarty przez kwas!",
			    victim, obj_lose, NULL, TO_CHAR );
		    else
			act( ZM_WZROK | ZM_WID_OBJ1, "$p zostaje prze`zarte przez kwas!",
			    victim, obj_lose, NULL, TO_CHAR );
		    if ( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
			victim->armor -= apply_ac( obj_lose, iWear );
		    obj_lose->value[ 0 ].v -= 1;
		    obj_lose->cost = 0;
		    if ( iWear != WEAR_NONE )
			victim->armor += apply_ac( obj_lose, iWear );
		}
		break;

	    case ITEM_CONTAINER:
		act( ZM_WZROK | ZM_WID_OBJ1, "Kwas prze`zera $j!",
		    victim, obj_lose, NULL, TO_CHAR );
		for ( obj = obj_lose->contains; obj; obj = obj_next )
		{
		    obj_next = obj->next_content;
		    if ( obj->deleted )
			continue;
		    obj_from_obj( obj );

		    if ( IS_OBJ_STAT( obj, ITEM_LOYAL )
		      || IS_OBJ_STAT( obj, ITEM_NODROP )
		      || IS_OBJ_STAT( obj, ITEM_QUEST ) )
		    {
			obj_to_char( obj, victim );
			if ( obj->rodzaj == 3 )
			{
			    act( ZM_WZROK | ZM_WID_OBJ2 | ZM_WID_OBJ1, "$P wypadaj`a z $g!",
				victim, obj_lose, obj, TO_CHAR );
			    act( ZM_WZROK | ZM_WID_OBJ2 | ZM_W_WID_OBJ1, "$P pojawiaj`a si`e nagle w twoim inwentarzu.",
				victim, obj_lose, obj, TO_CHAR );
			}
			else
			{
			    act( ZM_WZROK | ZM_WID_OBJ2 | ZM_WID_OBJ1, "$P wypada z $g!",
				victim, obj_lose, obj, TO_CHAR );
			    act( ZM_WZROK | ZM_WID_OBJ2 | ZM_W_WID_OBJ1, "$P pojawia si`e nagle w twoim inwentarzu.",
				victim, obj_lose, obj, TO_CHAR );
			}
		    }
		    else
		    {
			obj_to_room( obj, ch->in_room );

			act( ZM_WZROK | ZM_WID_CHAR, "$n upuszcza $J.",
			    victim, obj_lose, obj, TO_ROOM );
			act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ2,
			    obj->rodzaj == 3 ? "Pojawiaj`a si`e tu $P." : "Pojawia si`e tu $P.",
			    victim, obj_lose, obj, TO_ROOM );

			if ( obj->rodzaj == 3 )
			{
			    act( ZM_WZROK | ZM_WID_OBJ2 | ZM_WID_OBJ1, "$P wypadaj`a z $g!",
				victim, obj_lose, obj, TO_CHAR );
			    act( ZM_WZROK | ZM_WID_OBJ2 | ZM_W_WID_OBJ1, "Pojawiaj`a si`e tu $P.",
				victim, obj_lose, obj, TO_CHAR );
			}
			else
			{
			    act( ZM_WZROK | ZM_WID_OBJ2 | ZM_WID_OBJ1, "$P wypada z $g!",
				victim, obj_lose, obj, TO_CHAR );
			    act( ZM_WZROK | ZM_WID_OBJ2 | ZM_W_WID_OBJ1, "Pojawia si`e tu $P.",
				victim, obj_lose, obj, TO_CHAR );
			}

			{
			    ROOM_INDEX_DATA *in_room = victim->in_room;

			    oprog_drop_trigger( victim, obj );
			    if ( victim->in_room != in_room
			      || ch->in_room != in_room )
			    {
				if ( !obj_lose->deleted )
				    extract_obj( obj_lose );
				return FALSE;
			    }
			}
		    }
		}
		extract_obj( obj_lose );
		break;

	    case ITEM_FURNITURE:
		act( ZM_WZROK | ZM_WID_OBJ1, "Kwas prze`zera $j!",
		    victim, obj_lose, NULL, TO_CHAR );
		extract_obj( obj_lose );
		break;
	    }
	}
    }

    hpch = ch->max_hit
	 + zetnij( interpolate( ch->level, 750, 3000 ) - ch->max_hit,
	   interpolate( ch->level, 5000, 1000 ), ch->hit - ch->max_hit );
    /* URANGE( 10, ch->hit, ch->max_hit ); */
    dam  = number_range( hpch / 8 + 1, hpch / 4 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_fire_breath )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    char *msg;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted
	      || number_bits( 2 ) != 0 )
		continue;

	    /* na uproszczenie z can_see moge sobie pozwolic tylko dlatego,
	       ze nizej jest act TO_CHAR */
	    if ( can_see_obj( victim, obj_lose ) && obj_lose->rodzaj == 3 )
	    {
		switch ( obj_lose->item_type )
		{
		    default:             continue;
		    case ITEM_CONTAINER: msg = "$p spalaj`a si`e!"; break;
		    case ITEM_POTION:    msg = "$p wyparowuj`a!"; break;
		    case ITEM_SCROLL:    msg = "$p spalaj`a si`e!"; break;
		    case ITEM_STAFF:     msg = "$p spalaj`a si`e z dymem!"; break;
		    case ITEM_WAND:      msg = "$p iskrz`a si`e i p`ekaj`a z trzaskiem!"; break;
		    case ITEM_FOOD:
		    case ITEM_PILL:      msg = "$p krusz`a si`e i zw`eglaj`a!"; break;
		    case ITEM_FURNITURE: msg = "$p spalaj`a si`e z trzaskiem!"; break;
		}
	    }
	    else
	    {
		switch ( obj_lose->item_type )
		{
		    default:             continue;
		    case ITEM_CONTAINER: msg = "$p spala si`e!"; break;
		    case ITEM_POTION:    msg = "$p wyparowuje!"; break;
		    case ITEM_SCROLL:    msg = "$p spala si`e!"; break;
		    case ITEM_STAFF:     msg = "$p spala si`e z dymem!"; break;
		    case ITEM_WAND:      msg = "$p iskrzy si`e i p`eka z trzaskiem!"; break;
		    case ITEM_FOOD:
		    case ITEM_PILL:      msg = "$p kruszy si`e i zw`egla!"; break;
		    case ITEM_FURNITURE: msg = "$p spala si`e z trzaskiem!"; break;
		}
	    }

	    act( ZM_WZROK | ZM_WID_OBJ1, msg, victim, obj_lose, NULL, TO_CHAR );
	    extract_obj( obj_lose );
	}

    hpch = ch->max_hit
	 + zetnij( interpolate( ch->level, 750, 3000 ) - ch->max_hit,
	   interpolate( ch->level, 5000, 1000 ), ch->hit - ch->max_hit );
    /* URANGE( 10, ch->hit, ch->max_hit ); */
    dam  = number_range( hpch / 8 + 1, hpch / 4 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_frost_breath )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    char *msg;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted
	      || number_bits( 2 ) != 0 )
	    {
		continue;
	    }

	    if ( obj_lose->item_type != ITEM_CONTAINER
	      && obj_lose->item_type != ITEM_DRINK_CON
	      && obj_lose->item_type != ITEM_POTION )
	    {
		continue;
	    }

	    if ( obj_lose->rodzaj == 3 )
		msg = "$p zamarzaj`a i p`ekaj`a z trzaskiem!";
	    else
		msg = "$p zamarza i p`eka z trzaskiem!";
	    /* FIXME: rozbic na wersje ze sluchem i bez */
	    act( ZM_WZROK | ZM_WID_OBJ1, msg, victim, obj_lose, NULL, TO_CHAR );
	    extract_obj( obj_lose );
	}

    hpch = ch->max_hit
	 + zetnij( interpolate( ch->level, 750, 3000 ) - ch->max_hit,
	   interpolate( ch->level, 5000, 1000 ), ch->hit - ch->max_hit );
    /* URANGE( 10, ch->hit, ch->max_hit ); */
    dam  = number_range( hpch / 8 + 1, hpch / 4 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_gas_breath )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;
    int        hpch;

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( vch->in_room != room )
	    break;

	if ( vch->deleted
	  || is_safe( ch, vch, FALSE ) )
	    continue;

	if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
	{
	    hpch = ch->max_hit
		 + zetnij( interpolate( ch->level, 750, 3000 ) - ch->max_hit,
		   interpolate( ch->level, 5000, 1000 ), ch->hit - ch->max_hit );
	    dam  = number_range( hpch / 8 + 1, hpch / 4 );
	    if ( saves_spell( level, vch ) )
		dam /= 2;
	    spell_poison( gsn_poison, level, ch, vch, usage );
	    damage( ch, vch, dam, sn, WEAR_NONE, FALSE );
	}

	if ( ch->deleted || ch->in_room != room )
	    return FALSE;
    }

    return TRUE;
}


CZAR( spell_lightning_breath )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    hpch = ch->max_hit
	 + zetnij( interpolate( ch->level, 750, 3000 ) - ch->max_hit,
	   interpolate( ch->level, 5000, 1000 ), ch->hit - ch->max_hit );
    dam = number_range( hpch / 8 + 1, hpch / 4 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


/*
 * Code for Psionicist spells/skills by Thelonius
 */
CZAR( spell_adrenaline_control )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    bool polepszony = FALSE;
    bool wzmocniony = FALSE;

    af.type     = sn;
    af.duration = UMAX( 0, level - 5 );
    af.location = APPLY_DEX;
    af.level    = level;
    af.caster   = ch;
    if ( ch->klasa == CLASS_PSIONIK )
	af.modifier = UMAX( 2, level / 30 + 1 );
    else
	af.modifier = 2;
    CLEAR_BITS( af.bitvector );
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    af.location = APPLY_CON;
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    if ( !polepszony )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocniony );

    ASTC( "Da`l$o`s sobie pot`e`zny zastrzyk adrenaliny!", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n da`l$a sobie pot`e`zny zastrzyk adrenaliny!", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_agitation )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
		 int        dam;
    static const int        dam_each[ ] =
    {
	0,
	 0,  0,  0,  0,  0,     12, 15, 18, 21, 24,
	24, 24, 25, 25, 26,     26, 26, 27, 27, 27,
	28, 28, 28, 29, 29,     29, 30, 30, 30, 31,
	31, 31, 32, 32, 32,     33, 33, 33, 34, 34,
	34, 35, 35, 35, 36,     36, 36, 37, 37, 37,
	38, 38, 38, 39, 39,	39, 40, 40, 40, 41,
	41, 41, 42, 42, 42,	43, 43, 43, 44, 44,
	45, 45, 45, 46, 46,	46, 47, 47, 47, 48,
	48, 48, 49, 49, 50,	50, 51, 51, 52, 52,
	53, 53, 54, 54, 55,	55, 56, 57, 58, 59
    };

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_awe )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_fighting( victim, ch ) )
    {
	STC( "Wybierz w`lasnego przeciwnika.\n\r", ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    stop_fighting( victim, TRUE );

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

    act( ZM_ZAWSZE, "Uspokajasz $#!", ch, NULL, victim, TO_CHAR );
    act( ZM_PRZYT, "$n ci`e uspokaja!",ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_VICT, "$n uspokaja $#!", ch, NULL, victim, TO_NOTVICT );

    return TRUE;
}


CZAR( spell_ballistic_attack )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
		 int        dam;
    static const int        dam_each[ ] =
    {
	 0,
	 3,  4,  4,  5,  6,       6,  6,  7,  7,  7,
	 7,  7,  8,  8,  8,       9,  9,  9, 10, 10,
	10, 11, 11, 11, 12,      12, 12, 13, 13, 13,
	14, 14, 14, 15, 15,      15, 16, 16, 16, 17,
	17, 17, 18, 18, 18,      19, 19, 19, 20, 20
    };

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
      dam /= 2;

    act( ZM_ZAWSZE, "Chichoczesz, gdy grad kamieni uderza w $#.",
	ch, NULL, victim, TO_CHAR );

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_cell_adjustment )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    bool wyszlo = FALSE;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( is_affected( victim, gsn_poison ) )
    {
	affect_strip( victim, gsn_poison );
	act( ZM_PRZYT, "Ciep`le pr`ady przenikaj`a twoje cia`lo.", victim, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_VICT, "$N wygl`ada zdrowiej.", ch, NULL, victim, TO_NOTVICT );
	wyszlo = TRUE;
    }

    if ( is_affected( victim, gsn_curse  ) )
    {
	affect_strip( victim, gsn_curse  );
	act( ZM_PRZYT, "Czujesz si`e pewniej.", victim, NULL, NULL, TO_CHAR );
	wyszlo = TRUE;
    }

    if ( wyszlo )
	send_to_char( "Czar uda`l si`e.\n\r", ch );
    else
	NIEUDANE_BC( ch );

    return wyszlo;
}


CZAR( spell_combat_mind )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    bool polepszony = FALSE;
    bool wzmocniony = FALSE;

    af.type     = sn;
    af.duration = level + 3;
    af.location = APPLY_HITROLL;
    af.modifier = IS_NPC( ch ) ? level / 9 : level / 5;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    af.location = APPLY_AC;
    af.modifier = IS_NPC( ch ) ? level / 3 + 5 : level / 2 + 9;
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    if ( !polepszony )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocniony );

    if ( victim != ch )
	send_to_char( "Udzielasz natchnienia do walki.\n\r", ch );

    act( ZM_PRZYT, "Posiad`l$o`s znajomo`s`c magicznej taktyki wojennej.",
		 victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_complete_healing )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( victim->max_hit <= victim->hit )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    victim->hit = UMAX( victim->hit, victim->max_hit );
    update_pos( victim );

    if ( ch != victim )
	send_to_char( "Rzucasz ca`lkowite uzdrowienie.\n\r", ch );

    act( ZM_PRZYT, "Wspaniale, jeste`s ca`lkowicie uzdrowion$y!", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_control_flames )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
		 int        dam;
    static const int        dam_each[ ] =
    {
	 0,
	 0,  0,  0,  0,  0,       0,  0, 16, 20, 24,
	28, 32, 35, 38, 40,      42, 44, 45, 45, 45,
	46, 46, 46, 47, 47,      47, 48, 48, 48, 49,
	49, 49, 50, 50, 50,      51, 51, 51, 52, 52,
	52, 53, 53, 53, 54,      54, 54, 55, 55, 55
    };

    if ( !get_eq_char( ch, WEAR_LIGHT ) )
    {
	send_to_char( "Musisz mie`c przy sobie jakie`s `xr`od`lo `swiat`la.\n\r", ch );
	return FALSE;
    }

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_death_field )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;
    int        hpch;

    if ( !IS_EVIL( ch ) )
    {
	ASTC( "Nie jeste`s wystarczaj`aco z`l$y, aby to zrobi`c!", ch );
	return FALSE;
    }

    RZUCENIE_CZARU( ch, sn, usage );

    act( ZM_WZROK, "Emanuje z ciebie czarna po`swiata!", ch, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "Czarna po`swiata emanuje z $1!", ch, NULL, NULL, TO_ROOM );

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( vch->in_room != room )
	    break;

	if ( vch->deleted
	  || is_safe( ch, vch, FALSE ) )
	{
	    continue;
	}

	if ( IS_NPC( vch ) && vch != ch )
	{
	    hpch = URANGE( 10, ch->hit, 1999 );
	    if ( !saves_spell( level, vch )
	      && !saves_spell( level, vch ) /* mniejsze prawdopodobienstwo */
	      && level >= vch->level - 5 )
	    {
		dam = 3 * vch->hit;
		if ( HAS_SANCTUARY( vch ) )
		    dam *= 2;
		dam = UMAX( dam, 100 );
		vch->hit = 1;
		update_pos( vch );
		send_to_char( "`Dosi`ega ci`e kosa `smierci!\n\r", vch );
		/* Lam 18.1.2003: TO_NOTVICT */
		act( ZM_WZROK | ZM_WID_VICT, "Kosa `smierci dosi`ega $#!", ch, NULL, vch, TO_NOTVICT );
	    }
	    else
		dam = number_range( hpch / 16 + 1, hpch / 8 );

	    damage( ch, vch, dam, sn, WEAR_NONE, FALSE );

	    if ( ch->deleted
	      || ch->in_room != room )
	    {
		return FALSE;
	    }
	}
    }

    return TRUE;
}


CZAR( spell_detonate )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
		 int        dam;
		 OBJ_DATA  *obj_lose;
		 OBJ_DATA  *obj_next;
    static const int        dam_each[ ] =
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,  75,
	 80,  85,  90,  95, 100,	102, 104, 106, 108, 110,
	112, 114, 116, 118, 120,	122, 124, 126, 128, 130,
	132, 134, 136, 138, 140,	142, 144, 146, 148, 150,
	152, 154, 156, 158, 160,	162, 164, 166, 168, 170,
	172, 174, 176, 178, 180,	182, 184, 186, 188, 190,
	192, 194, 196, 198, 200,	202, 204, 206, 208, 210,
	212, 214, 216, 218, 220,	222, 224, 226, 228, 230,
	232, 235, 238, 242, 248,	250, 252, 254, 257, 260
    };

    /* Lam 6.9.2005: skopiowane z dezintegracji */
    if ( number_percent( ) < level && !saves_spell( level, victim ) )
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
		continue;

	    if ( IS_OBJ_STAT( obj_lose, ITEM_LOYAL )
	      || IS_OBJ_STAT( obj_lose, ITEM_QUEST )
	      || IS_OBJ_STAT( obj_lose, ITEM_INVENTORY )
	      /* tu 1/32 na uszkodzenie 1 przedmiotu: */
	      || number_bits( 5 ) != 0 )
	    {
		continue;
	    }

	    if ( obj_lose->rodzaj == 3 )
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpadaj`a si`e!", victim, obj_lose, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpadaj`a si`e!", victim, obj_lose, NULL, TO_ROOM );
	    }
	    else
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpada si`e!", victim, obj_lose, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpada si`e!", victim, obj_lose, NULL, TO_ROOM );
	    }

	    extract_obj( obj_lose );
	    /* uszkodzimy najwyzej 1 przedmiot, dezintegracja lepsza */
	    break;
	}

    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


/* Qwert 27.02.04: male poprawki */
/* Lam 6.9.2005: wieksze poprawki */
CZAR( spell_disintegrate )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    bool udana = FALSE;

    if ( victim->level > ch->level + 5
      || IS_IMMORTAL( victim ) )
    {
	STC( "Nie uda ci si`e.\n\r", ch );
	return FALSE;
    }

    if ( !IS_NPC( ch ) && !IS_NPC( victim )
      && IS_SET( ch->act, PLR_ZABOJCA ) && !IS_SET( victim->act, PLR_ZABOJCA ) )
    {
	STC( "Nie uda ci si`e.\n\r", ch );
	return FALSE;
    }

    if ( !is_fighting( victim, ch ) )
    {
	ASTC( "Sta`l$o`s zbyt daleko.", ch );
	return FALSE;
    }

    if ( number_percent( ) < ( level * 3 / 2 ) && !saves_spell( level, victim ) )
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted
	      /* Qwert: lojalny sprzet nie powinien znikac: */
	      || IS_OBJ_STAT( obj_lose, ITEM_LOYAL )
	      || IS_OBJ_STAT( obj_lose, ITEM_QUEST )
	      || IS_OBJ_STAT( obj_lose, ITEM_INVENTORY )
	      || number_bits( 2 ) != 0  )
	    {
		continue;
	    }

	    if ( obj_lose->rodzaj == 3 )
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpadaj`a si`e!", victim, obj_lose, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpadaj`a si`e!", victim, obj_lose, NULL, TO_ROOM );
	    }
	    else
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpada si`e!", victim, obj_lose, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_OBJ1, "$p rozpada si`e!", victim, obj_lose, NULL, TO_ROOM );
	    }

	    extract_obj( obj_lose );
	    udana = TRUE;
	}

    if ( !saves_spell( level, victim ) )
    {
	int procent = victim->hit * 100 / victim->max_hit;

	/* w tym miejscu nigdy nie zejsc ponizej 8! (ze wzorem ponizej) */
	if ( procent < 13 )
	{
	    act( ZM_ZAWSZE, "Spowodowa`l$o`s, `ze $N rozpad`l$A si`e!", ch, NULL, victim, TO_CHAR );
	    act( ZM_ZAWSZE, "Zosta`l$O`s zdezintegrowan$Y przez $3!", ch, NULL, victim, TO_VICT );
	    act( ZM_WZROK | ZM_WID_VICT, "$n dezintegruje $#!", ch, NULL, victim, TO_NOTVICT );

	    if ( IS_NPC( victim ) )
	    {
		group_gain( ch, victim );

		if ( !ch->deleted && !IS_NPC( ch ) )
		    ++ch->pcdata->killed;

		extract_char( victim, TRUE );
	    }
	    else
	    {
		victim->hit = 1;
		victim->position = POS_MORTAL;
		damage( ch, victim, 100, TYPE_UNDEFINED, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );
	    }
	}
	else
	{
	    act( ZM_ZAWSZE, "Spowodowa`l$o`s ogromne zniszczenia w ciele $!!", ch, NULL, victim, TO_CHAR );
	    act( ZM_PRZYT, "$n spowodowa`l$a ogromne zniszczenia w twoim ciele!", ch, NULL, victim, TO_VICT );
	    act( ZM_WZROK | ZM_WID_VICT, "$n spowodowa`l$a ogromne zniszczenia w ciele $!!", ch, NULL, victim, TO_NOTVICT );
	    /*
	     * przy 13% bedzie zabierac 5-7% (musi przezyc)
	     * przy 100% bedzie zabierac 14-16%
	     * przy 1000% bedzie zabierac 554-556%
	     */
	    victim->hit -= victim->max_hit * number_fuzzy( 5 + UMAX( 0, ( procent - 100 ) / 2 ) + procent / 10 ) / 100;
	}
	udana = TRUE;
    }

    if ( !udana )
	act( ZM_ZAWSZE, "$N broni si`e przed dezintegracj`a.", ch, NULL, victim, TO_CHAR );

    return udana;
}


CZAR( spell_displacement )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_PRZEMIESZCZENIE )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level - 4;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_PRZEMIESZCZENIE );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_PRZYT, "Cz`e`sciowo przenosisz si`e w inne miejsce.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n cz`e`sciowo przenosi si`e w inne miejsce.",
	victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_domination )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo, *rch;
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
    {
	STC( "Mobom nie wolno dominowa`c nad graczami.\n\r", ch );
	return FALSE;
    }

    if ( victim == ch )
    {
	send_to_char( "W pe`lni nad sob`a dominujesz!\n\r", ch );
	return FALSE;
    }

/*  if ( ch->level >= LEVEL_HERO && get_trust( ch ) < L_SEN )
    {
	send_to_char( "Z tak`a bosko`sci`a nie potrzebujesz s`lug.\n\r", ch );
	sprintf( log_buf, "%s charming", ch->name );
	log_string( log_buf );
	return FALSE;
    } */

    if ( IS_AFFECTED( victim, AFF_CHARM ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM )
      || level < victim->level
      || saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    if ( !IS_NPC( victim )
      || IS_SET( victim->act, ACT_NO_CHARM ) )
    {
	act( ZM_ZAWSZE, "$N nie pozwala si`e zdominowa`c.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	if ( is_fighting( rch, victim ) )
	{
	    act( ZM_ZAWSZE, "$N jest zbyt zaj`et$Y walk`a, `zeby zwraca`c na ciebie uwag`e.", ch, NULL, victim, TO_CHAR );
	    return FALSE;
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

    act( ZM_ZAWSZE, "B`edziesz dominowa`l$a nad $$!", ch, NULL, victim, TO_CHAR );
    act( ZM_ZAWSZE, "B`edziesz zdominowan$Y przez $3!", ch, NULL, victim, TO_VICT );

    return TRUE;
}


CZAR( spell_ectoplasmic_form )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_PASSING( victim )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = number_fuzzy( level / 4 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_PASS_DOOR );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_PRZYT, "Stajesz si`e bardzo elastyczn$y.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n staje si`e bardzo elastyczn$y.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_ego_whip )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->klasa == CLASS_PSIONIK
      || is_affected( victim, sn ) || saves_spell( level, victim ) )
    {
	act( ZM_ZAWSZE, "$N wytrzymuje pr`ob`e przywo`lania z`lych wspomnie`n.", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_HITROLL;
    af.level    = level;
    af.caster   = ch;
    if ( ch->klasa == CLASS_PSIONIK )
	af.modifier  = -2 - level / 3;
    else
	af.modifier  = -2 - level / 7;
    CLEAR_BITS( af.bitvector );
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    if ( ch->klasa == CLASS_PSIONIK )
	af.modifier  = - 10 - level / 5;
    else
	af.modifier  = - 5 - level / 10;
    affect_to_char( victim, &af );

    af.location  = APPLY_DEX;
    af.modifier  = - 3;
    affect_to_char( victim, &af );

    act( ZM_ZAWSZE, "Przywo`lujesz najgorsze wspomnienia z `zycia $!.", ch, NULL, victim, TO_CHAR );
    send_to_char( "Zbiera ci si`e na p`lacz, kiedy przypominaj`a ci si`e twe najgorsze do`swiadczenia.\n\r", victim );
    /* act( "Umys`l $! jest niszczony przez $3!", ch, NULL, victim, TO_NOTVICT ); */

    return TRUE;
}


CZAR( spell_energy_containment )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = level / 2 + 7;
    af.location = APPLY_SAVING_SPELL;
    af.level    = level;
    af.caster   = ch;
    if ( ch->klasa == CLASS_PSIONIK )
	af.modifier  = level / 4 + 5;
    else
	af.modifier  = UMIN( level / 4, 15 );
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    send_to_char( "Mo`zesz przejmowa`c pewne formy energii.\n\r", ch );

    return TRUE;
}


CZAR( spell_enhance_armor )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    int         szansa;
    char        buf[ MAX_STRING_LENGTH ];

    if ( usage == USAGE_CAST && IS_NPC( ch ) )
    {
	send_to_char( "Chwilowo niedost`epne mobom.\n\r", ch );
	return FALSE;
    }

    if ( obj->item_type != ITEM_ARMOR
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| obj->affected )
    {
	send_to_char( "Ten przedmiot nie mo`ze by`c wzmocniony.\n\r", ch );
	return FALSE;
    }

    paf                 = new_affect( );

    paf->type           = sn;
    paf->duration       = -1;
    paf->location       = APPLY_AC;
    CLEAR_BITS( paf->bitvector );
    paf->next           = obj->affected;
    obj->affected       = paf;

    szansa = ch->pcdata->learned[ sn ] + 3 * ( ch->level - obj->level );

    if ( usage == USAGE_CAST
      && number_percent( ) >= szansa )
    /* Bad Enhancement ... opps! :) */
    {
	paf->modifier   = - level / 8;
	obj->cost       = 0;

	SET_BIT_V( obj->extra_flags, ITEM_NODROP );
	act( ZM_WZROK | ZM_WID_OBJ1, obj->rodzaj == 3 ? "$p lekko czerniej`a." : "$p lekko czernieje.",
		ch, obj, NULL, TO_CHAR );
	return TRUE;
    }

    /* Good enhancement */
    paf->modifier = level / 8;
    obj->level = UMAX( number_fuzzy( ch->level - 3 ), obj->level );

    sprintf( buf, "$p `swiec%s si`e na ", ( obj->rodzaj == 3 ) ? "`a" : "i" );

    if ( IS_GOOD( ch ) )
    {
	strcat( buf, "niebiesko." );
	SET_BIT_V( obj->extra_flags, ITEM_ANTI_EVIL );
    }
    else if ( IS_EVIL( ch ) )
    {
	strcat( buf, "czerwono." );
	SET_BIT_V( obj->extra_flags, ITEM_ANTI_GOOD );
    }
    else
    {
	strcat( buf, "`z`o`lto." );
	SET_BIT_V( obj->extra_flags, ITEM_ANTI_EVIL );
	SET_BIT_V( obj->extra_flags, ITEM_ANTI_GOOD );
    }

    act( ZM_WZROK | ZM_WID_OBJ1, buf, ch, obj, NULL, TO_CHAR );

    SET_BIT_V( obj->extra_flags, ITEM_MAGIC );
    if ( !IS_OBJ_STAT( obj, ITEM_PRIVATE )
	  || !str_cmp( ch->name, obj->owner ) )
    {
	free_string( obj->owner );
	obj->owner = str_dup( ch->name );
	SET_BIT_V( obj->extra_flags, ITEM_PRIVATE );
    }

    send_to_char( "Ulepszasz zbroj`e.\n\r", ch );

    return TRUE;
}


CZAR( spell_enhanced_strength )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_STR;
    af.modifier = 1 + ( level >= 15 ) + ( level >= 25 );
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    ASTC( "Stajesz si`e silniejsz$y!", victim );

    return TRUE;
}


CZAR( spell_flesh_armor )
{
    CHAR_DATA   *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_AC;
    af.level    = level;
    af.caster   = ch;
    if ( ch->klasa == CLASS_PSIONIK )
	af.modifier = UMAX( 40, level / 2 + 10 );
    else
	af.modifier = 20;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_PRZYT, "Twoja sk`ora staje si`e twarda niczym stalowy pancerz.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "Sk`ora $1 staje si`e twarda niczym stal.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_inertial_barrier )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;
    bool wyszlo = FALSE;
    bool wzmocn = FALSE;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch )
	  || ( HAS_PROTECTION( gch ) && !is_affected( gch, sn ) ) )
	{
	    continue;
	}

	af.type     = sn;
	af.duration = 24;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_PROTECT );

	if ( affect_join( gch, &af, ENH_MAX ) )
	{
	    wyszlo = TRUE;
	    if ( !wzmocnienie_affect_join )
	    {
		act( ZM_WZROK | ZM_WID_CHAR, "Wok`o`l $1 tworzy si`e bariera my`slowa.", gch, NULL, NULL, TO_ROOM );
		act( ZM_PRZYT, "Wok`o`l ciebie tworzy si`e bariera my`slowa.", gch, NULL, NULL, TO_CHAR );
	    }
	    else
		wzmocn = TRUE;
	}
    }

    WZMOCNIONY( ch, sn, wzmocn );

    if ( !wyszlo )
	NIEUDANE_BC( ch );

    return wyszlo;
}


CZAR( spell_bariera_zla )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;
    bool wyszlo = FALSE;
    bool wzmocn = FALSE;

    if ( !IS_EVIL( ch ) )
    {
	ASTC( "Jeste`s zbyt dobr$y, aby `xli bogowie wsparli tw`a grup`e.", ch );
	return FALSE;
    }

    RZUCENIE_CZARU( ch, sn, usage );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch )
	  || ( HAS_PROTECT_GOOD( gch ) && !is_affected( gch, sn ) )
	  || !IS_EVIL( gch ) )
	{
	    continue;
	}

	af.type     = sn;
	af.duration = 24;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_TARCZA_ZLA );

	if ( affect_join( gch, &af, ENH_MAX ) )
	{
	    wyszlo = TRUE;
	    if ( !wzmocnienie_affect_join )
	    {
		act( ZM_WZROK | ZM_WID_CHAR, "Wok`o`l $1 tworzy si`e bariera z`la.", gch, NULL, NULL, TO_ROOM );
		act( ZM_PRZYT, "Wok`o`l ciebie tworzy si`e bariera z`la.", gch, NULL, NULL, TO_CHAR );
	    }
	    else
		wzmocn = TRUE;
	}
    }

    WZMOCNIONY( ch, sn, wzmocn );

    if ( !wyszlo )
	NIEUDANE_BC( ch );

    return wyszlo;
}


CZAR( spell_inflict_pain )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    damage( ch, victim, dice( 2, 10 ) + level / 2, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_intellect_fortress )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;
    bool wyszlo = FALSE;
    bool wzmocn = FALSE;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) )
	    continue;

	af.type     = sn;
	af.duration = 24;
	af.location = APPLY_AC;
	af.level    = level;
	af.caster   = ch;
	af.modifier = ( gch == ch && ch->klasa == CLASS_PSIONIK )
			? UMAX( 40, level / 2 + 10 ) : 30;
	CLEAR_BITS( af.bitvector );
	if ( affect_join( gch, &af, ENH_MAX ) )
	{
	    wyszlo = TRUE;
	    if ( !wzmocnienie_affect_join )
	    {
		if ( gch != ch )
		    act( ZM_PRZYT, "$N si`l`a swojego umys`lu tworzy wok`o`l ciebie wirtualn`a fortec`e.", gch, NULL, ch, TO_CHAR );
		else
		    send_to_char( "Tworzysz wok`o`l siebie wirtualn`a fortec`e.\n\r", gch );
		act( ZM_WZROK | ZM_WID_CHAR, "Wirtualna forteca tworzy si`e wok`o`l $1.", gch, NULL, gch, TO_ROOM );
	    }
	    else
		wzmocn = TRUE;
	}
    }

    WZMOCNIONY( ch, sn, wzmocn );

    if ( !wyszlo )
	NIEUDANE_BC( ch );

    return wyszlo;
}


CZAR( spell_lend_health )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        hpch;

    if ( ch == victim )
    {
	send_to_char( "Po`zycza`c zdrowie samemu sobie?\n\r", ch );
	return FALSE;
    }

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    hpch = UMIN( 50, victim->max_hit - victim->hit );
    if ( hpch <= 0 ) /* Lam 21.5.2000: tu bylo == ! mozna bylo ZABIERAC zycie */
    {
	act( ZM_ZAWSZE, "Jeste`s bardzo mi`l$y, ale $N nie potrzebuje uzdrowienia.", ch, NULL,
	    victim, TO_CHAR );
	return FALSE;
    }
    if ( ch->hit - hpch < 50 )
    {
	ASTC( "Sam$a potrzebujesz uzdrowienia!", ch );
	return FALSE;
    }
    victim->hit += hpch;
    ch->hit     -= hpch;
    update_pos( victim );
    update_pos( ch );

    act( ZM_ZAWSZE, "Cz`e`s`c twojej energii sp`lyn`e`la na $#.", ch, NULL, victim, TO_CHAR );
    if ( victim->hit == victim->max_hit )
	ASTC( "Odzyska`l$o`s pe`lni`e zdrowia.", victim );
    else
	act( ZM_PRZYT, "$n oddaje ci cz`e`s`c swojej energii.", ch, NULL, victim, TO_VICT );
/*  act( "$n oddaje $@ cz`e`s`c swojej energii.", ch, NULL, victim, TO_NOTVICT ); */

    return TRUE;
}


CZAR( spell_mental_barrier )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = 24;
    af.location = APPLY_AC;
    af.level    = level;
    af.caster   = ch;
    if ( ch->klasa == CLASS_PSIONIK )
	af.modifier = UMAX( 20, level / 4 + 15 );
    else
	af.modifier  = 20;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    ASTC( "Stworzy`l$o`s wok`o`l siebie barier`e mentaln`a.", victim );

    return TRUE;
}


CZAR( spell_mind_thrust )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    damage( ch, victim, dice( 1, 10 ) + level / 2, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_project_force )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    damage( ch, victim, dice( 4, 6 ) + level, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_psionic_blast )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
		 int        dam;
    static const int        dam_each[ ] =
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,	  0,  45,  50,  55,  60,
	 64,  68,  72,  76,  80,	 82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,	102, 104, 106, 108, 100,
	112, 114, 116, 118, 120,	122, 124, 126, 128, 130,
	132, 134, 136, 138, 140,	142, 144, 146, 148, 150,
	152, 154, 156, 158, 160,	162, 164, 166, 168, 170,
	172, 174, 176, 178, 180,	182, 184, 186, 188, 190,
	192, 194, 196, 198, 200,	202, 204, 206, 208, 210,
	212, 214, 216, 218, 220,	222, 224, 226, 228, 230
    };


    level    = UMAX( 0, level );
    level    = UMIN( (unsigned int) level, sizeof( dam_each ) / sizeof( dam_each[ 0 ] ) - 1 );
    dam      = number_range( dam_each[ level ] / 2, dam_each[ level ] * 2 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_psychic_crush )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    damage( ch, victim, dice( 3, 5 ) + level, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_psychic_drain )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level / 2;
    af.location = APPLY_STR;
    af.level    = level;
    af.caster   = ch;
    if ( ch->klasa == CLASS_PSIONIK )
	af.modifier  = -1 - ( level >= 10 ) - ( level >= 20 ) - ( level >= 30 );
    else
	af.modifier  = -1 - ( level >= 30 ) - ( level >= 60 );
    CLEAR_BITS( af.bitvector );
    affect_to_char( victim, &af );

    act( ZM_PRZYT, "Czujesz si`e wyczerpan$y.", victim, NULL, NULL, TO_CHAR );
    act( ZM_ZAWSZE, "Odsysasz si`l`e $!.", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "$n odsysa si`l`e $!.", ch, NULL, victim, TO_NOTVICT );

    return TRUE;
}


CZAR( spell_psychic_healing )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    if ( IS_AFFECTED( victim, AFF_DEGENERACJA ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	return FALSE;
    }

    if ( victim->hit >= victim->max_hit )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    heal = dice( 3, 6 ) + 2 * level / 3 ;
    victim->hit = UMIN( victim->hit + heal,
		  UMAX( victim->max_hit, victim->hit ) );
    update_pos( victim );

    if ( victim->hit == victim->max_hit )
	ASTC( "Odzyska`l$o`s pe`lni`e zdrowia.", victim );
    else
	act( ZM_PRZYT, "Czujesz si`e lepiej!", victim, NULL, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_share_strength )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim == ch )
    {
	if ( ch->sex == 2 )
	    send_to_char( "Nie mo`zesz dzieli`c si`ly z sam`a sob`a!\n\r", ch );
	else
	    send_to_char( "Nie mo`zesz dzieli`c si`ly z samym sob`a!\n\r", ch );
	return FALSE;
    }
    if ( is_affected( victim, sn ) )
    {
	act( ZM_ZAWSZE, "$N ju`z dzieli z kim`s swoj`a si`l`e.", ch, NULL, victim,
	    TO_CHAR );
	return FALSE;
    }
    if ( get_curr_str( ch ) <= 5 )
    {
	send_to_char( "Nie masz czym si`e dzieli`c.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_STR;
    af.modifier =  1 + ( level >= 20 ) + ( level >= 30 );
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( victim, &af );

    af.modifier  = -1 - ( level >= 20 ) - ( level >= 30 );
    affect_to_char( ch,     &af );

    act( ZM_PRZYT, "$n dzieli si`e z tob`a swoj`a si`l`a.", ch, NULL, victim, TO_VICT );
    act( ZM_ZAWSZE, "Dzielisz si`e si`l`a z$E $$.", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "$n dzieli si`e si`l`a z$E $$.",  ch, NULL, victim, TO_NOTVICT );

    return TRUE;
}


CZAR( spell_thought_shield )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = level;
    af.location = APPLY_SAVING_SPELL;
    af.level    = level;
    af.caster   = ch;
    if ( ch->klasa == CLASS_PSIONIK )
	af.modifier = UMAX( 10, level / 3 + 2 );
    else
	af.modifier = 10;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    ASTC( "Utworzy`l$o`s tarcz`e wok`o`l siebie.", ch );

    return TRUE;
}


CZAR( spell_ultrablast )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;
    int        hpch;

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( vch->in_room != room )
	    break;

	if ( vch->deleted
	  || is_safe( ch, vch, FALSE ) )
	{
	    continue;
	}

	if ( vch != ch && !is_same_group( vch, ch ) )
	{
	    hpch = ch->max_hit
		 + zetnij( interpolate( ch->level, 750, 3000 ) - ch->max_hit,
		   interpolate( ch->level, 5000, 1000 ), ch->hit - ch->max_hit );
	    dam  = number_range( hpch / 4, hpch / 2 );
	    if ( saves_spell( level, vch ) )
		dam /= 2;
	    damage( ch, vch, dam, sn, WEAR_NONE, FALSE );
	}

	if ( ch->deleted
	  || ch->in_room != room )
	{
	    return FALSE;
	}
    }

    return TRUE;
}


CZAR( spell_turn_undead )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;

    if ( victim->level >= level
      || saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    if ( CZY_WAMPIR( victim )
      || victim->race == zr_martwiak )
    {
	do_flee( victim, "" );
    }

    return TRUE;
}


CZAR( spell_exorcise )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, gsn_vampiric_bite ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( IS_EVIL( victim ) && skill_get_trust( ch ) < L_SEN )
    {
	act( ZM_ZAWSZE, "$N jest zbyt z`l$Y, aby ulec egzorcyzmowi!", ch, NULL, victim, TO_CHAR );
	return FALSE;
    }
    /*
     * We actually give the vampiric curse a chance to save
     * at the victim's level
     *
     * Lam 6.9.2001: bylo !saves_spell
     */
    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    affect_strip( victim, gsn_vampiric_bite );

    send_to_char( "Dokonujesz egzorcyzmu.\n\r", ch );
    act( ZM_PRZYT, "Ciep`le pr`ady przep`lywaj`a przez twoje cia`lo.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "$N wygl`ada zdrowiej.", ch, NULL, victim, TO_NOTVICT );

    return TRUE;
}


/* Flame shield spell from Malice of EnvyMud */
CZAR( spell_flaming )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_FLAMING )
      && !is_affected( victim, sn ) )
    {
	STC( "Ju`z otacza ci`e p`lon`aca tarcza.\n\r", ch );
	return FALSE;
    }

    if ( IS_AFFECTED( victim, AFF_LODOWA_TARCZA ) )
    {
	STC( "Jest ci za zimno, aby tworzy`c ogie`n.\n\r", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = number_fuzzy( level / 4 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_FLAMING );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_PRZYT, "Otacza ci`e p`lon`aca tarcza.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n jest otoczon$y p`lon`ac`a tarcz`a.",
	victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_alchemy )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int	     zloto;
    char     bufor[ MAX_STRING_LENGTH ];

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "Nie mo`zesz pozby`c si`e tego przedmiotu.\n\r", ch );
	return FALSE;
    }
    zloto = URANGE( 1, obj->level / 2, 20 );
    zloto += obj->weight / 3;
    zloto *= level / 10;
    if ( obj->item_type == ITEM_WEAPON )
	zloto *= 2;
    if ( obj->item_type == ITEM_ARMOR )
    {
	zloto *= 4;
	zloto /= 3;
    }
    if ( obj->item_type == ITEM_TRASH )
	zloto /= 4;
    if ( obj->item_type == ITEM_FOOD )
	zloto /= 3;
    if ( obj->item_type == ITEM_DRINK_CON )
	zloto /= 10;
    sprintf( bufor, "Zmieniasz $j w %s!", ZLOTO_B( zloto ) );
    act( ZM_ZAWSZE, bufor, ch, obj, NULL, TO_CHAR );
    ch->gold += zloto;
    extract_obj( obj );

    return TRUE;
}


CZAR( spell_dream )
{
    CHAR_DATA *vict;
    char arg[ MAX_INPUT_LENGTH ];

    target_name = one_argument( target_name, arg );
    if ( !(vict = get_char_world_pc( ch, arg ) )
      || ( ch->in_room->area != vict->in_room->area ) )
    {
	send_to_char( "Nikogo takiego nie ma w okolicy...\n\r", ch );
	return FALSE;
    }

    if ( vict->position != POS_SLEEPING )
    {
	send_to_char( "Twoja ofiara nie `spi.\n\r", ch );
	return FALSE;
    }

    if ( !target_name )
    {
	send_to_char( "O czym ma `sni`c twoja ofiara?\n\r", ch );
	return FALSE;
    }

    act( ZM_ZAWSZE, "{Y`Sni ci si`e $n m`owi`ac$0: \"$t{Y\"{x", ch, target_name, vict, TO_VICT );
    send_to_char( "Wywo`lujesz sen.\n\r", ch );

    return TRUE;
}


CZAR( spell_krwawe_lzy )
{
    CHAR_DATA   *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_BLIND )
      || IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_EYES ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    act( ZM_WZROK | ZM_WID_VICT, "$N o`slep`l$A!", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK, "Twoje oczy nagle zachodz`a krwi`a! Niczego nie widzisz!",
			victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "Oczy $! zachodz`a krwi`a, a $N jest o`slepion$Y!", ch, NULL, victim, TO_NOTVICT );

    af.type     = sn;
    af.duration = 2 + level / 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_BLIND );
    affect_to_char( victim, &af );

    return TRUE;
}


/* Freak: 25.10.2004 */
CZAR( spell_pajeczyna )
{
    CHAR_DATA   *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    AFFECT_DATA *paf;

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    if ( is_affected( victim, sn ) )
    /* jezeli juz dziala, to oplatamy jeszcze raz */
    {
	for ( paf = victim->affected; paf; paf = paf->next )
	    if ( paf->type == sn )
		break;

	if ( !paf )
	{
	    NIEUDANE_BC( ch );
	    return FALSE;
	}

	paf->duration = UMIN( paf->duration +
			    ch->level / ( 8 + paf->duration ),
			    3 + ( ch->level ) / 5 );

	act( ZM_ZAWSZE, "Oplatasz $V kolejn`a warstw`a magicznej paj`eczyny.", ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "$n oplata ci`e kolejn`a warstw`a magicznej paj`eczyny!", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "$n oplata $# kolejn`a warstw`a magicznej paj`eczyny.", ch, NULL, victim, TO_NOTVICT );
    }
    else if ( IS_AFFECTED( victim, AFF_HOLD ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }
    else
    {
	if ( !ch->fighting
	  || victim != FIGHTING( ch ) )
	{
	    af.duration = 1 + ( ( ch->level ) / 8 );
	}
	else
	    af.duration = 3 + ( ( ch->level ) / 8 );

	af.type     = sn;
	af.location = APPLY_HITROLL;
	af.modifier = -ch->level;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	SET_BIT_V( af.bitvector, AFF_HOLD );

	affect_to_char( victim, &af );

	act( ZM_ZAWSZE, "Pochwyci`l$o`s $V w magiczn`a paj`eczyn`e.", ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "$n pochwyci`l$a ci`e w magiczn`a paj`eczyn`e!", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "$n pochwyci`l$a $# w magiczn`a paj`eczyn`e.", ch, NULL, victim, TO_NOTVICT );
    }

    return TRUE;
}


/* ACK! */
CZAR( spell_okno )
{
    OBJ_DATA *beacon;
    CHAR_DATA *rch;

    if ( target_name[ 0 ] == '\0' )
    {
	send_to_char( "Dok`ad utworzy`c okno?\n\r", ch );
	return FALSE;
    }

    beacon = get_obj_world( ch, target_name );

    if ( beacon == NULL
      || beacon->item_type != ITEM_BEACON )
    {
	send_to_char( "Nie mog`e znale`x`c celu.\n\r", ch );
	return FALSE;
    }

    if ( beacon->in_room == NULL )
    {
	send_to_char( "Kto`s trzyma drogowskaz lub go w co`s w`lo`zy`l.\n\r", ch );
	return FALSE;
    }

    if ( beacon->in_room == ch->in_room )
    {
	STC( "Ale`z ju`z jeste`s w tym pomieszczeniu. Po prostu si`e rozejrzyj.\n\r", ch );
	return FALSE;
    }

/*   if ( str_cmp( beacon->owner, ch->name ) )
    {
	send_to_char( "To nie tw`oj drogowskaz!\n\r", ch );
	return FALSE;
    }
*/
    act( ZM_WZROK | ZM_WID_CHAR, "$n machni`eciem r`eki tworzy magiczne okno.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Machni`eciem r`eki tworzysz magiczne okno.\n\r", ch );

    pokaz_pomieszczenie( ch, beacon->in_room, FALSE );
    if ( ( rch = beacon->in_room->people ) )
    {
	act( ZM_WZROK, "Nagle w powietrzu pojawiaj`a si`e dziwne iskrz`ace zawirowania.", rch, NULL, NULL, TO_ROOM );
	act( ZM_WZROK, "Nagle w powietrzu pojawiaj`a si`e dziwne iskrz`ace zawirowania.", rch, NULL, NULL, TO_CHAR );
    }

    return TRUE;
}


CZAR( spell_brama )
{
    /* Create a magic portal to another room */
    OBJ_DATA *beacon;
    OBJ_DATA *ob;
    CHAR_DATA *rch;
    char buf[ MIL ];
    int glevel;

    if ( target_name[ 0 ] == '\0' )
	sprintf( buf, "drogowskaz_%s", ch->name );
    else
	strcpy( buf, target_name );

    beacon = get_obj_world( ch, buf );

    if ( beacon == NULL
      || beacon->item_type != ITEM_BEACON )
    {
	send_to_char( "Nie mog`e znale`x`c celu.\n\r", ch );
	return FALSE;
    }

    if ( !beacon->in_room )
    {
	send_to_char( "Zdaje si`e, `ze tw`oj drogowskaz nie le`zy na ziemi.\n\r", ch );
	return FALSE;
    }

    if ( beacon->in_room == ch->in_room )
    {
	STC( "Brama z wej`sciem i wyj`sciem w jednym pomieszczeniu? To bez sensu.\n\r", ch );
	return FALSE;
    }

    /* jesli drogowskaz przeniosl bohater/niesmiertelny na wyzszym poziomie */
    if ( beacon->level > ch->level )
    {
	send_to_char( "Kto`s uszkodzi`l drogowskaz! Tworzysz k`l`eby dymu, ale brama si`e nie pojawia.\n\r", ch );
	if ( ( rch = beacon->in_room->people ) )
	{
	    /* bez odmiany, bo to zawsze "drogowskaz" */
	    act( ZM_WZROK | ZM_WID_OBJ1, "Nagle $p znika!", rch, beacon, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, "Nagle $p znika!", rch, beacon, NULL, TO_ROOM );
	}
	extract_obj( beacon );
	return FALSE;
    }

    if ( beacon->owner != '\0' && str_cmp( beacon->owner, ch->name ) )
    {
	send_to_char( "To nie tw`oj drogowskaz!\n\r", ch );
	return FALSE;
    }

    /* Brama jest obustronna, wiec sprawdzamy flagi z obu stron. W spell_summon
       NO_RECALL musi sie zgadzac, ale tutaj nie musi byc miedzy krainami, wiec
       przy NO_RECALL z obu stron tez nie da bramy */
    if ( IS_SET( beacon->in_room->room_flags, ROOM_SAFE )
      || IS_SET( ch->in_room->room_flags, ROOM_SAFE )
      || IS_SET( beacon->in_room->room_flags, ROOM_NO_RECALL )
      || IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
      || ch->in_room->trap
      || beacon->in_room->trap )
    {
	send_to_char( "Nie uda`lo ci si`e.\n\r", ch );
	return FALSE;
    }

    ob = create_object( get_obj_index( OBJ_VNUM_BRAMA ), level );
    obj_to_room( ob, ch->in_room );
    ob->timer = 1 + ( level / 30 );
    glevel = ch->level - 5;
    if ( ch->level >= LEVEL_HERO )
    {
	glevel = UMAX( glevel, LEVEL_HERO );
	if ( ch->level >= LEVEL_IMMORTAL )
	    glevel = UMAX( glevel, LEVEL_IMMORTAL );
    }
    ob->value[ 0 ].v = glevel;
    ob->value[ 1 ].v = ( beacon->carried_by == NULL
		     ? beacon->in_room->vnum : beacon->carried_by->in_room->vnum );
    ob->value[ 2 ].v = IS_NPC( ch ) || usage != USAGE_CAST
	? UMAX( 0, 20 - level / 4 )
	: UMAX( 0, 20 - level * ch->pcdata->learned[ sn ] / 250 );

    act( ZM_ZAWSZE, "Tworzysz $j jednym ruchem r`eki.", ch, ob, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n tworzy $j jednym ruchem r`eki.", ch, ob, NULL, TO_ROOM );
    /* tu tez bez odmiany, zawsze "brama" */
    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, ob, NULL, TO_ROOM );

    ob = create_object( get_obj_index( OBJ_VNUM_BRAMA ), level );
    obj_to_room( ob, beacon->in_room );
    ob->timer = 1 + ( level / 30 );
    ob->value[ 0 ].v = glevel;
    ob->value[ 1 ].v = ch->in_room->vnum;
    ob->value[ 2 ].v = IS_NPC( ch ) || usage != USAGE_CAST
	? UMAX( 0, 20 - level / 4 )
	: UMAX( 0, 20 - level * ch->pcdata->learned[ sn ] / 250 );
    if ( ( rch = beacon->in_room->people ) )
    {
	act( ZM_WZROK | ZM_WID_OBJ1, "Nagle $p znika!", rch, beacon, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_OBJ1, "Nagle $p znika!", rch, beacon, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_WID_OBJ1, "Przed tob`a pojawia si`e $p!", rch, ob, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_OBJ1, "Przed tob`a pojawia si`e $p!", rch, ob, NULL, TO_ROOM );
    }
    extract_obj( beacon );

    return TRUE;
}


CZAR( spell_drogowskaz )
{
   /* Create a beacon, ready for a portal to 'goto'.
    * I owe someone thanks for this, but forgot the name... D'oh.
    * How this works: (+ for portals, etc )
    * a) Caster makes beacon: ownership set, obj->name = target_name
    * b) Someone goes off with beacon, drops it
    * c) Owner can then cast portal spell to it, beacon extracted
    * -S- */

    char buf[ MAX_STRING_LENGTH ];
    char arg[ MAX_STRING_LENGTH ];
    OBJ_DATA *ob;

    one_argument( target_name, arg );
    if ( !*arg )
	sprintf( arg, "drogowskaz_%s", ch->name );

    ob = get_obj_world( ch, arg );
    if ( ob != NULL )
    {
	send_to_char( "Ju`z istnieje przedmiot o tej nazwie.\n\r", ch );
	return FALSE;
    }

    ob = create_object( get_obj_index( OBJ_VNUM_DROGOWSKAZ ), level );
    sprintf( buf, "drogowskaz %s", arg );
    free_string( ob->name );
    ob->name = str_dup( buf );
    sprintf( buf, "%s", ch->name );
    free_string( ob->owner );
    ob->owner = str_dup( buf );
    ob->timer = number_range( 30, 45 );
    obj_to_room( ob, ch->in_room );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n magicznie tworzy $j.", ch, ob, NULL, TO_ROOM );
    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, ob, NULL, TO_ROOM );
    act( ZM_ZAWSZE, "Sw`a magi`a tworzysz $j.", ch, ob, NULL, TO_CHAR );

    return TRUE;
}


CZAR( spell_kocie_ruchy )
{
    AFFECT_DATA af;
    CHAR_DATA  *victim = (CHAR_DATA *) vo;

    if ( IS_AFFECTED( victim, AFF_PRZYSPIESZENIE ) )
    {
	send_to_char( "Jeszcze szybciej porusza`c si`e nie b`edziesz.", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = level / 2;
    af.location = APPLY_DEX;
    af.modifier = 1;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_ZAWSZE, "Czujesz si`e zr`eczniejsz$y.", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "Ruchy $1 staj`a si`e szybsze.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_iluzja )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_ILUZJA )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = ( ch == victim ) ? level : level / 5;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_ILUZJA );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    if ( ch == victim )
    {
	send_to_char( "Oblekasz si`e w iluzj`e.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n obleka si`e w iluzj`e.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
	act( ZM_ZAWSZE, "Rzucasz iluzj`e na $#.", ch, NULL, victim, TO_CHAR );
	act( ZM_WZROK, "$n rzuca na ciebie iluzj`e.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "$n rzuca iluzj`e na $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return TRUE;
}


CZAR( spell_zaburzenie_energii )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_ZABURZENIE_ENERGII ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 2 + level / 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_ZABURZENIE_ENERGII );
    affect_to_char( victim, &af );

    if ( ch != victim )
    {
	act( ZM_ZAWSZE, "Zaburzasz energi`e $!.", ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "$n zaburza twoj`a energi`e.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "$n zaburza energi`e $!.", ch, NULL, victim, TO_NOTVICT );
    }
    else
    {
	act( ZM_ZAWSZE, "Zaburzasz swoj`a energi`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zaburza swoj`a energi`e.", ch, NULL, NULL, TO_ROOM );
    }

    return TRUE;
}


CZAR( spell_blokada_mentalna )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_BLOKADA_MENTALNA ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 3 + level / 15;
    af.location = APPLY_WIS;
    af.modifier = -1 - ( level >= 50 ) - ( level >=  75 );
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_BLOKADA_MENTALNA );
    affect_to_char( victim, &af );

    if ( ch != victim )
    {
	act( ZM_ZAWSZE, "Blokujesz psychik`e $!.", ch, NULL, victim, TO_CHAR );
	act( ZM_ZAWSZE | ZM_DYLEMAT, "$n blokuje twoj`a psychik`e.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "$n blokuje psychik`e $!.", ch, NULL, victim, TO_NOTVICT );
    }
    else
    {
	act( ZM_ZAWSZE, "Blokujesz swoj`a psychik`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n blokuje swoj`a psychik`e.", ch, NULL, NULL, TO_ROOM );
    }

    return TRUE;
}


CZAR( spell_odblokowanie_mentalne )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;

    if ( is_affected( victim, gsn_blokada_mentalna ) )
    {
	affect_strip( victim, gsn_blokada_mentalna );
	if ( ch != victim )
	    send_to_char( "Odblokowujesz psychik`e.\n\r", ch );
	send_to_char( "Blokada twojej psychiki zosta`la zniesiona.\n\r", victim );
	return TRUE;
    }
    else
	NIEUDANE_BC( ch );

    return FALSE;
}


CZAR( spell_kontrola_nad_woda )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_KONTROLA_NAD_WODA )
      && !is_affected( victim, sn ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = 14 + level / 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_KONTROLA_NAD_WODA );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    act( ZM_PRZYT, "Mo`zesz teraz kontrolowa`c wod`e wok`o`l siebie.", victim, NULL, NULL, TO_CHAR );
    /* CZY_WODA bez sensu - jesli nie opadl, nie potrzebuje kontroli aktualnie,
       a moze fruwac/stac w lodzi */
    if ( CZY_POD_WODA( victim->in_room ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "Woda wok`o`l $1 zaczyna wirowa`c.", victim, NULL, NULL, TO_ROOM );
	act( ZM_WZROK | ZM_W_WID_CHAR, "W wodzie tworzy si`e ma`ly wir.", victim, NULL, NULL, TO_ROOM );
    }
    else if ( ch != victim )
	act( ZM_ZAWSZE, "Umo`zliwiasz $@ kontrol`e nad wod`a.", ch, NULL, victim, TO_CHAR );

    return TRUE;
}


/* Lam 7.2.2006, nazwe wymyslil Reif */
CZAR( spell_rytual_zycia )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( is_affected( victim, gsn_znamie_smierci ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "$n odprawia skomplikowany rytua`l `zycia.", ch, NULL, NULL, TO_ROOM );
	STC( "Przep`lywa przez ciebie strumie`n energii `zyciowej.\n\r", victim );
	affect_strip( victim, gsn_znamie_smierci );
	return TRUE;
    }

    /* Kilka osob zapewne kupi to nie majac na sobie znamienia - dam im szanse
       zorientowania sie, ze za pieniadze zostal kupiony czar, ale nie wyszedl
       z oczywistego powodu. */
    act( ZM_WZROK | ZM_WID_CHAR, "$n rozpoczyna odprawianie rytua`lu `zycia, ale przerywa orientuj`ac si`e, `ze to bezcelowe.", ch, NULL, NULL, TO_ROOM );
    NIEUDANE_BC( ch );

    return FALSE;
}


/* Lam 7.4.2006 */
CZAR( spell_splot_many )
{
    AFFECT_DATA af;

    if ( IS_SET( ch->in_room->room_flags, ROOM_CLAN_MANA ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    af.type = sn;
    af.duration = UMIN( 13, level / 5 );
    af.bitvector[ 0 ] = ROOM_CLAN_MANA;
    affect_to_room( ch->in_room, &af );
    STC( "Tworzysz w tym miejscu splot kana`l`ow many.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n tworzy w tym miejscu splot kana`l`ow many.", ch, NULL, NULL, TO_ROOM );
    act( ZM_PRZYT | ZM_W_WID_CHAR, "Tworzy si`e tu splot kana`l`ow many.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}
