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
 *  Caly nekromanta, wlaczajac w to niniejszy plik, jest dzielem Ulryka.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"
#include "magic.h"
#include "handler.h"
#include "const.h"
#include "db.h"


/*
 * definicje lokalne
 */

OBJ_DATA  *znajdz_zwloki( CHAR_DATA *ch, char *argument, int sn );
void       zmien_poziom_moba( CHAR_DATA *ch, int poziom );
int        ile_moze_slug( CHAR_DATA *ch );
CHAR_DATA *znajdz_sluge( CHAR_DATA *ch, char *argument );

/*
 * funkcje globalne
 */

void zwolnij_sluge( CHAR_DATA *ch )
{
    AFFECT_DATA *paf, *paf_next;

    for ( paf = ch->affected; paf; paf = paf_next )
    {
	paf_next = paf->next;

	if ( paf->deleted )
	    continue;

	if ( paf->type == gsn_kontrola_umyslu )
	{
	    paf->modifier -= 1;

	    if ( paf->modifier == 0 )
		affect_remove( ch, paf, TRUE );
	}
    }

    return;
}


void zabij_wszystkie_slugi( CHAR_DATA *ch )
{
    ZWOD_DATA *zwod, *zwod_next;

    /* nikt nie podaza -> brak slug */
    if ( !ch->zwod_master )
	return;

    for ( zwod = ch->zwod_master; zwod; zwod = zwod_next )
    {
	zwod_next = zwod->next;
	if ( zwod->ch->master != ch )
	    bug( "zwod->ch->master != ch", 0 );
	if ( IS_AFFECTED( zwod->ch, AFF_SLUGA ) )
	{
	    /* przydaloby sie zrobic, zeby najpierw bylo "przestaje za toba
	       chodzic", a potem dopiero, ze sie rozpada, ale to by wymagalo
	       nieco glebszych zmian, a ze te rzeczy nastepuja jednoczesnie,
	       nie jest wcale zle */
	    act( ZM_WZROK | ZM_WID_CHAR, "$n rozpada si`e.", zwod->ch, NULL, NULL, TO_ROOM );
	    extract_char( zwod->ch, TRUE );
	}
    }
    return;
}


/*
 * funkcje lokalne
 */

OBJ_DATA *znajdz_zwloki( CHAR_DATA *ch, char *argument, int sn )
{
    OBJ_DATA  *obj;
    char       arg[ MAX_INPUT_LENGTH ];
    char      *tekst;

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	bool graczowe = FALSE;

	/* Lam 28.5.2006: obsluga cial graczy, zeby nie mowil, ze nie ma cial,
	   ale zadalem sobie trud zapewnienia, ze znajdzie cialo moba, chocby
	   lezalo duzo cial graczy */
	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
	    if ( obj->item_type == ITEM_CORPSE_NPC
	      && can_see_obj( ch, obj ) )
		break;
	    if ( obj->item_type == ITEM_CORPSE_PC
	      && can_see_obj( ch, obj ) )
		graczowe = TRUE;
	}

	if ( !obj )
	{
	    if ( graczowe )
		tekst = "Bogowie chroni`a to cia`lo przed twymi praktykami.\n\r";
	    else if ( sn == gsn_balsamowanie )
		tekst = "Nie widzisz tu cia`la do balsamowania.\n\r";
	    else if ( sn == gsn_pocalunek_smierci )
		tekst = "Nie widzisz tu `zadnego cia`la.\n\r";
	    else if ( sn == gsn_eksplozja )
		tekst = "Nie ma tu zw`lok, kt`ore mo`zesz wysadzi`c.\n\r";
	    else if ( sn == gsn_wskrzeszenie )
		tekst = "Nie widzisz nikogo do wskrzeszenia.\n\r";
	    else if ( sn == gsn_stworzenie_mumii )
		tekst = "Nie widzisz tu zabalsamowanych zw`lok.\n\r";
	    else
	    {
		bug( "znajdz_zwloki: zly sn", 0 );
		return NULL;
	    }

	    send_to_char( tekst, ch );
	    return NULL;
	 }
    }
    else
    {
	obj = get_obj_list( ch, arg, ch->in_room->contents );
	if ( !obj )
	{
	    send_to_char( "Nie widzisz tu tego.\n\r", ch );
	    return NULL;
	}
	if ( obj->item_type != ITEM_CORPSE_NPC )
	{
	    if ( obj->item_type == ITEM_CORPSE_PC )
		send_to_char( "Bogowie chroni`a to cia`lo przed twymi praktykami.\n\r", ch );
	    else
	    {
		if ( sn == gsn_balsamowanie )
		    tekst = "Balsamowa`c mo`zesz jedynie cia`la!\n\r";
		else if ( sn == gsn_pocalunek_smierci )
		    tekst = "Poca`lunek `smierci musisz sk`lada`c na zw`lokach.\n\r";
		else if ( sn == gsn_eksplozja )
		    tekst = "Do wywo`lania eksplozji potrzebujesz cia`la.\n\r";
		else if ( sn == gsn_wskrzeszenie )
		    tekst = "Aby wskrzesi`c kogo`s, potrzebujesz jego cia`la.\n\r";
		else if ( sn == gsn_stworzenie_mumii )
		    tekst = "Do tego potrzebujesz zabalsamowanego cia`la.\n\r";
		else
		{
		    bug( "znajdz_zwloki: zly sn", 0 );
		    return NULL;
		}

		send_to_char( tekst, ch );
	    }
	    return NULL;
	}
    }

    return obj;
}


void zmien_poziom_moba( CHAR_DATA *ch, int poziom )
{
    ch->level   = URANGE( 1, poziom, LEVEL_HERO );
    ch->armor   = interpolate( ch->level, 100, -100 );
    ch->max_hit = ch->level * 8 + number_range(
				 ch->level * ch->level * 3 / 16,
				 ch->level * ch->level / 4 );
    ch->hit     = ch->max_hit;

    return;
}


int ile_moze_slug( CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) )
	return 0;

    return 1 + ( ch->level +
	ch->pcdata->learned[ gsn_sprowadzenie_szkieletu ] +
	ch->pcdata->learned[ gsn_wskrzeszenie ] +
	ch->pcdata->learned[ gsn_stworzenie_mumii ] ) / 300;
}


CHAR_DATA *znajdz_sluge( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;
    int        count = 0;

    number = number_argument( argument, arg );

    for ( wch = char_list; wch ; wch = wch->next )
    {
	if ( !can_see( ch, wch )
	  || !is_name( arg, wch->name )
	  || !IS_NPC( wch )
	  || !IS_AFFECTED( wch, AFF_SLUGA )
	  || wch->master != ch )
	{
	    continue;
	}

	if ( ++count == number )
	    return wch;
    }

    return NULL;
}


/*
 * czary i umiejetnosci
 */

CZAR( spell_mroczny_znak )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = URANGE( 10, level / 2, 25 );
    af.location = APPLY_SAVING_SPELL;
    af.modifier = 15 + level / 10;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    STC( "Naznaczasz si`e mrocznym znakiem.\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n naznaczy`l$a si`e mrocznym znakiem.", victim, NULL, NULL,
	 TO_ROOM );

    return TRUE;
}


KOMENDA( do_balsamuj )
{
    OBJ_DATA *obj;
    int old_timer;
    int szansa;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_balsamowanie )
      || !ch->pcdata->learned[ gsn_balsamowanie ] )
    {
	send_to_char( "Nie potrafisz balsamowa`c zw`lok.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	ASTC( "Jeste`s na to zbyt zaj`et$y walk`a.", ch );
	return;
    }

    if ( !( obj = znajdz_zwloki( ch, argument, gsn_balsamowanie ) ) )
	return;

    /* czy cialo nie bylo juz wczesniej balsamowane */
    if ( obj->value[ 0 ].v != 0 )
    {
	send_to_char( "To cia`lo ju`z by`lo balsamowane.\n\r", ch );
	return;
    }

    /* tak, by cialo nie rozpadlo sie w trakcie balsamowania */
    old_timer = obj->timer;
    obj->timer = 0;
    WAIT_STATE( ch, skill_table[ gsn_balsamowanie ].beats );
    obj->timer = old_timer;

    /* na wynik balsamowania ma wplyw takze madrosc i inteligencja */
    szansa = 0
	+ ch->pcdata->learned[ gsn_balsamowanie ]
	+ ( get_curr_wis( ch ) + get_curr_int( ch ) - 40 ) * 5;

    if ( number_percent( ) > URANGE( 0, szansa, 95 ) )
    {
	send_to_char( "Nie uda`lo ci si`e zabalsamowa`c zw`lok.\n\r", ch );
	obj->value[ 0 ].v = 2;
	if ( !number_bits( 2 ) )
	{
	    send_to_char( "Cia`lo rozpada si`e na skutek twoich eksperyment`ow.\n\r", ch );
	    extract_obj( obj );
	}
	return;
    }

    obj->timer *= ( 3 + ( ch->level / 10 +
			  ch->pcdata->learned[ gsn_balsamowanie ] / 10 ) / 4 );
			  /* maksymalnie *8 */
    obj->value[ 0 ].v = 1;
    act( ZM_ZAWSZE, "Balsamujesz $j.", ch, obj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_OBJ1, "$n balsamuje $j.", ch, obj, NULL, TO_ROOM );

    uzyj( ch, gsn_balsamowanie, "balsamowaniu zw`lok" );

    return;
}


CZAR( spell_sprowadzenie_szkieletu )
{
    CHAR_DATA   *szkielet;
    AFFECT_DATA *paf;
    AFFECT_DATA  af;
    char         buf[ MAX_STRING_LENGTH ];
    int          poziom;

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "Tw`oj umys`l jest zbyt zniewolony, by tego dokona`c.\n\r", ch );
	return FALSE;
    }

    for ( paf = ch->affected; paf; paf = paf->next )
	if ( !paf->deleted && paf->type == gsn_kontrola_umyslu )
	    break;

    if ( paf && paf->modifier >= ile_moze_slug( ch ) )
    {
	send_to_char( "Nie jeste`s w stanie kontrolowa`c kolejnego s`lugi.\n\r", ch );
	return FALSE;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_SUMMON ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    poziom = ch->level - 5;
    /* Lam: bohater na 100 i wyzej zmienia poziomy przedmiotow, jego slugi tez.
       Daje szkieletom poziom bohatera, bo inaczej nie daloby sie ich ubrac. */
    if ( ch->level >= LEVEL_HERO )
	poziom = ch->level;
    szkielet = create_mobile( get_mob_index( MOB_VNUM_SZKIELET ) );
    zmien_poziom_moba( szkielet, poziom );

    sprintf( buf, szkielet->description, ch->dopelniacz );
    free_string( szkielet->description );
    szkielet->description = str_dup( buf );

    if ( !IS_NPC( ch ) )
	szkielet->area = get_room_index( ch->pcdata->strefa->przywolanie )->area;

    char_to_room( szkielet, ch->in_room );
    act( ZM_WZROK | ZM_WID_VICT, "$N przybywa na wezwanie $1.", ch, NULL, szkielet, TO_NOTVICT );

    add_follower( szkielet, ch );
    af.type     = sn;
    af.duration = 5 + level / 20;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_CHARM );
    SET_BIT_V( af.bitvector, AFF_SLUGA );
    affect_to_char( szkielet, &af );

    if ( paf && !paf->deleted )
	paf->modifier += 1;
    else
    {
	af.type     = gsn_kontrola_umyslu;
	af.duration = -1;
	af.location = APPLY_NONE;
	af.modifier = 1;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	affect_to_char( ch, &af );
    }

    return TRUE;
}


CZAR( spell_odeslanie )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( ch == victim )
    {
	send_to_char( "Siebie nie ode`slesz.\n\r", ch );
	return FALSE;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "To nie jest tw`oj s`luga.\n\r", ch );
	return FALSE;
    }

    if ( !IS_AFFECTED( victim, AFF_SLUGA )
      || victim->master != ch )
    {
	send_to_char( "To nie jest tw`oj s`luga.\n\r", ch );
	return FALSE;
    }

    send_to_char( "Odwo`lujesz swego s`lug`e.\n\r", ch );
    act( ZM_ZAWSZE, "$n odsy`la ci`e z powrotem w niebyt.", ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_VICT, "$n odsy`la $#.", ch, NULL, victim, TO_NOTVICT );

    extract_char( victim, TRUE );

    return TRUE;
}


CZAR( spell_wejrzenie )
{
    CHAR_DATA *sluga;

    if ( target_name[ 0 ] == '\0' )
    {
	send_to_char( "Gdzie chcesz wejrze`c?\n\r", ch );
	return FALSE;
    }

    sluga = znajdz_sluge( ch, target_name );

    if ( !sluga || sluga->deleted )
    {
	send_to_char( "Nie mo`zesz odnale`x`c takiego s`lugi.\n\r", ch );
	return FALSE;
    }

    /* Ulryk: to juz teoretycznie niepotrzebne, bo sprawdzane w znajdz_sluge( ).
       Zostawie to jeszcze na jakis czas, zobaczymy co dalej */
    if ( !IS_NPC( sluga )
      || !IS_AFFECTED( sluga, AFF_SLUGA )
      || sluga->master != ch )
    {
	send_to_char( "To nie tw`oj s`luga.\n\r", ch );
	bug( "spell_wejrzenie: sluga nagle przestal byc sluga.", 0 );
	return FALSE;
    }

    if ( !sluga->in_room )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c.\n\r", ch );
	bug( "spell_wejrzenie: sluga znajduje sie nigdzie.", 0 );
	return FALSE;
    }

/*
    if ( sluga->in_room == ch->in_room )
    {
	send_to_char( "Czy to ma sens? Jeste`scie w tym samym pomieszczeniu.\n\r", ch );
	return FALSE;
    }
    Lam: ma to sens, jesli sluga ma wykrycia, a ja nie, wiec sobie go opetuje
*/

    if ( !ch->desc )
	return FALSE;

    if ( sluga->desc )
    {
	send_to_char( "Kto`s inny kontroluje umys`l twego s`lugi.\n\r", ch );
	return FALSE;
    }

    act( ZM_ZAWSZE, "Na chwil`e op`etujesz cia`lo $!.", ch, NULL, sluga, TO_CHAR );
    sluga->desc = ch->desc;
    sluga->polskie = ch->polskie;
    sluga->kolor = ch->kolor;

    do_look( sluga, "" );

    sluga->desc = NULL;

    return TRUE;
}


CZAR( spell_wzmocnienie_umyslu )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    bool polepszony = FALSE;
    bool wzmocniony = FALSE;

    af.type     = sn;
    af.duration = ( level >= 50 ? 25 : 15 );
    af.location = APPLY_INT;
    af.modifier = 1 + ( level >= 25 ) + ( level >= 50 );
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    af.location = APPLY_WIS;
    polepszony |= affect_join( victim, &af, ENH_MAX );
    wzmocniony |= wzmocnienie_affect_join;

    if ( !polepszony )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocniony );

    /* to jest lepsze niz "wzmacniasz moc czaru": */
    send_to_char( "Wzmacniasz moce swojego umys`lu.\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n wzmacnia sw`oj umys`l.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_nekromantyczna_zbroja )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = UMAX( 24, level );
    af.location = APPLY_AC;
    af.level    = level;
    af.caster   = ch;
    if ( ch->klasa == CLASS_NEKROMANTA )
	af.modifier = 10 + 5 * ( level / 20 );
    else
	af.modifier = 10;
    CLEAR_BITS( af.bitvector );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    STC( "Otacza ci`e nekromantyczna zbroja.\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n jest otoczon$y nekromantyczn`a zbroj`a.", victim, NULL, NULL,
	 TO_ROOM );

    return TRUE;
}


/*
 * odpowiednik uderzenia plomieni i lodowego kolca, uzyte do tarczy umarlych
 */
CZAR( spell_mroczny_plomien )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = dice( 6, level );

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_kontrola_umyslu )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo, *rch;
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
    {
	STC( "Mobom nie wolno kontrolowa`c umys`l`ow graczy.\n\r", ch );
	return FALSE;
    }

    if ( victim == ch )
    {
	STC( "Ju`z kontrolujesz sw`oj umys`l.\n\r", ch );
	return FALSE;
    }

/*
    if ( ch->level >= LEVEL_HERO && get_trust( ch ) < L_SEN )
    {
	send_to_char( "Nie potrzebujesz ju`z s`lug.\n\r", ch );
	sprintf( log_buf, "%s probuje kontrolowac umysl %s.",
		ch->name, victim->dopelniacz );
	log_string( log_buf );
	return FALSE;
    }
*/

    if ( IS_AFFECTED( victim, AFF_CHARM )
      || IS_AFFECTED( ch, AFF_CHARM )
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

    send_to_char( "Rzucasz kontrol`e umys`lu.\n\r", ch );
    act( ZM_ZAWSZE, "Czy`z \"rady\" $1 nie s`a wspania`le?", ch, NULL, victim, TO_VICT );

    return TRUE;
}


CZAR( spell_wyssanie_duszy )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dam;

    if ( ch == victim )
    {
	send_to_char( "Twoja dusza nie chce zosta`c wyssana.\n\r", ch );
	return FALSE;
    }

    dam = number_range( -10, 10 ) + UMIN( 50 - ( ch->alignment / 100 ), level );
    dam = UMAX( 4, dam );

    if ( saves_spell( level, victim ) )
	dam /= 2;
    else
    {
	ch->hit = UMIN( ch->hit + dam, UMAX( ch->max_hit, ch->hit ) );

	if ( !is_affected( victim, sn ) && number_bits( 4 ) )
	{
	    af.type     = sn;
	    af.duration = number_fuzzy( 5 );
	    af.location = APPLY_DEX;
	    af.modifier = -1;
	    af.level    = level;
	    af.caster   = ch;
	    CLEAR_BITS( af.bitvector );
	    affect_to_char( victim, &af );

	    ASTC( "Czujesz si`e pozbawion$y cz`astki siebie.\n\r", victim );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n poblad`l$a.", victim, NULL, NULL, TO_ROOM );
	}
    }

    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


CZAR( spell_wskrzeszenie )
{
    MOB_INDEX_DATA *pierwowzor;
    OBJ_DATA    *cialo;
    OBJ_DATA    *obj, *obj_next;
    CHAR_DATA   *zombie;
    AFFECT_DATA *paf;
    AFFECT_DATA  af;
    char         buf[ MAX_STRING_LENGTH ];
    char         buf1[ MAX_STRING_LENGTH ];
    int          poziom;
    int          timer = 0;

    if ( !( cialo = znajdz_zwloki( ch, target_name, gsn_wskrzeszenie ) ) )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "Tw`oj umys`l jest zbyt zniewolony, by tego dokona`c.\n\r", ch );
	return FALSE;
    }

    for ( paf = ch->affected; paf; paf = paf->next )
	if ( !paf->deleted && paf->type == gsn_kontrola_umyslu )
	    break;

    if ( paf && paf->modifier >= ile_moze_slug( ch ) )
    {
	send_to_char( "Nie jeste`s w stanie kontrolowa`c kolejnego s`lugi.\n\r", ch );
	return FALSE;
    }

    /* poziom = ch->level - 2; */
    /* Lam: powyzsze na ponizsze: */
    pierwowzor = get_mob_index( cialo->value[ 2 ].v );
    poziom = pierwowzor ? URANGE( 0, pierwowzor->level - 2, ch->level - 2 ) : 0;
    zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE ) );
    zmien_poziom_moba( zombie, poziom );

/*
 *  czesc kodu robiaca moba z ciala
 */
    timer = cialo->timer;

    for ( obj = cialo->contains; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted )
	    continue;

	obj_from_obj( obj );
	obj_to_char( obj, zombie );
    }

    if ( cialo->pIndexData->vnum == OBJ_VNUM_CORPSE_NPC
     /* ponizsze, aby zapobiec istnieniu: Zombi zombi zombi stoi sobie tutaj.*/
	&& pierwowzor->race != race_lookup( "undead" ) )
    {
	sprintf( buf, "zombi %s", pierwowzor->dopelniacz );

	free_string( zombie->short_descr );
	zombie->short_descr = str_dup( buf );

	free_string( zombie->dopelniacz );
	zombie->dopelniacz = str_dup( buf );

	free_string( zombie->celownik );
	zombie->celownik = str_dup( buf );

	free_string( zombie->biernik );
	zombie->biernik = str_dup( buf );

	free_string( zombie->narzednik );
	zombie->narzednik = str_dup( buf );

	free_string( zombie->miejscownik );
	zombie->miejscownik = str_dup( buf );

	free_string( zombie->wolacz );
	zombie->wolacz = str_dup( buf );
    }
    else
	sprintf( buf, "zombi" );

    sprintf( buf1, zombie->long_descr, buf );
    free_string( zombie->long_descr );
    zombie->long_descr = str_dup( buf1 );

    sprintf( buf, zombie->description,
	    ch->sex == 2 ? "a" : "",
	    ch->short_descr );
    free_string( zombie->description );
    zombie->description = str_dup( buf );

    if ( !IS_NPC( ch ) )
	zombie->area = get_room_index( ch->pcdata->strefa->przywolanie )->area;

    extract_obj( cialo );
    char_to_room( zombie, ch->in_room );
    act( ZM_WZROK | ZM_WID_VICT, "$n powo`luje do istnienia $#.", ch, NULL, zombie, TO_NOTVICT );

    add_follower( zombie, ch );
    af.type     = sn;
    af.duration = 5 + timer + level / 20;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_CHARM );
    SET_BIT_V( af.bitvector, AFF_SLUGA );
    affect_to_char( zombie, &af );

    if ( paf && !paf->deleted )
	paf->modifier += 1;
    else
    {
	af.type     = gsn_kontrola_umyslu;
	af.duration = -1;
	af.location = APPLY_NONE;
	af.modifier = 1;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	affect_to_char( ch, &af );
    }

    return TRUE;
}


CZAR( spell_pocalunek_smierci )
{
    OBJ_DATA *obj;
    int bonus;

    if ( !( obj = znajdz_zwloki( ch, target_name, gsn_pocalunek_smierci ) ) )
	return FALSE;

    bonus = dice( 10, 5 ) + level / 2;
    ch->hit = UMIN( ch->hit + bonus,
		UMAX( ch->max_hit, ch->hit ) );
    /* wzor jak w wysysaniu energii */
    bonus = UMAX( ( ( interpolate( level, 200, 4500 )
		      + 2 * ch->max_mana )
		    - ch->mana ) / 128 + level / 16, 2 );
    bonus = bonus / 2 + dice( 2, 6 );
    ch->mana += bonus;

    send_to_char( "Sk`ladasz poca`lunek `smierci.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n sk`lada poca`lunek `smierci.", ch, obj, NULL, TO_ROOM );
    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1,
	obj->rodzaj == 3 ? "$p znikaj`a." : "$p znika.", ch, obj, NULL, TO_ROOM );
    extract_obj( obj );

    return TRUE;
}


CZAR( spell_eksplozja )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        dam;

    if ( !( obj = znajdz_zwloki( ch, target_name, gsn_eksplozja ) ) )
	return FALSE;

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	send_to_char( "Nie mo`zesz tego zrobi`c tutaj.\n\r", ch );
	return FALSE;
    }

    act( ZM_ZAWSZE, "Powodujesz eksplozj`e $g.", ch, obj, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_OBJ1, "$n powoduje eksplozj`e $g.", ch, obj, NULL, TO_ROOM );

    /* dam = dice( level, 15 ) + level; */
    /* Lam 13.5.2007: uzaleznienie obrazen od masy ciala. Obrazenia duzo wieksze
       dla ciezkich cial, ale mniej takich cial mozna uniesc. Skrajne obrazenia
       (4 razy wieksze niz poprzedni wzor) mozna uzyskac z cial smokow, ktore
       nalezy zabic w tym samym pomieszczeniu, bo nawet przepchnac sie tego
       przeciez nie da */
    dam = number_range( zetnij( 37 * level, 300, obj->weight ),
			zetnij( 43 * level, 300, obj->weight ) );

    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;

	if ( victim->in_room != room )
	    break;

	if ( victim->deleted
	  || victim == ch
	  || is_same_group( ch, victim )
	  || is_safe( ch, victim, FALSE ) )
	{
	    continue;
	}

	damage( ch, victim, saves_spell( level, victim ) ? dam / 2 : dam,
		sn, WEAR_NONE, FALSE );

	if ( ch->deleted || ch->in_room != room )
	    return FALSE;
    }
    extract_obj( obj );

    return TRUE;
}


CZAR( spell_kradziez_cienia )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
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
    af.duration = level / 3;
    af.location = APPLY_SAVING_SPELL;
    af.modifier = - level / 10;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( victim, &af );

    af.location = APPLY_HITROLL;
    af.modifier = -10;
    affect_to_char( victim, &af );

    if ( ch != victim )
    {
	act( ZM_ZAWSZE, "Kradniesz $@ cie`n.", ch, NULL, victim, TO_CHAR );
	act( ZM_WZROK, "$n pozbawi`l$a ci`e twego cienia.", ch, NULL, victim, TO_VICT );
    }
    else
	send_to_char( "Kradniesz sobie cie`n.\n\r", ch );

    return TRUE;
}


CZAR( spell_aura_strachu )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type     = sn;
    af.duration = number_fuzzy( 2 + level / 25 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_GROZA );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    STC( "Zaczynasz wzbudza`c groz`e.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n nabiera gro`xnego wygl`adu.", ch, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_strefa_cienia )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	ASTC( "Jeste`s ju`z otoczon$y stref`a cienia.", ch );
	return FALSE;
    }

    af.type     = sn;
    af.duration = victim->level / 5;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_MROK );
    if ( !HAS_INFRAVISION( victim ) )
	SET_BIT_V( af.bitvector, AFF_INFRARED );
    affect_to_char( victim, &af );

    STC( "Mrok ogarnia twoje otoczenie.\n\r", ch );
    act( ZM_WZROK, "Za spraw`a $1 robi si`e bardzo ciemno.", ch, NULL, NULL, TO_ROOM );

    /* Lam 3.12.2004: poki co dowolne swiatlo powoduje, ze mrok niczego nie robi
       (wiec komunikat powyzej jest bledny) */
    if ( !ch->in_room->light )
	sprawdz_przyzwyczajanie( ch->in_room );

    return TRUE;
}


SPADANIE( wear_off_strefa_cienia )
{
    act( ZM_WZROK, "Mrok wok`o`l ciebie rozst`epuje si`e.", ch, NULL, NULL, TO_CHAR );

    return;
}


CZAR( spell_stworzenie_mumii )
{
    MOB_INDEX_DATA *pierwowzor;
    OBJ_DATA    *cialo;
    OBJ_DATA    *obj, *obj_next;
    CHAR_DATA   *mumia;
    AFFECT_DATA *paf;
    AFFECT_DATA  af;
    char         buf[ MAX_STRING_LENGTH ];
    char         buf1[ MAX_STRING_LENGTH ];
    int          poziom;
    int          timer = 0;

    if ( !( cialo = znajdz_zwloki( ch, target_name, gsn_stworzenie_mumii ) ) )
	return FALSE;

    if ( cialo->value[ 0 ].v != 1 )
    {
	send_to_char( "Mo`zesz stworzy`c mumi`e tylko z zabalsamowanych zw`lok.\n\r", ch );
	return FALSE;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "Tw`oj umys`l jest zbyt zniewolony, by tego dokona`c.\n\r", ch );
	return FALSE;
    }

    for ( paf = ch->affected; paf; paf = paf->next )
	if ( !paf->deleted && paf->type == gsn_kontrola_umyslu )
	    break;

    if ( paf && paf->modifier == ile_moze_slug( ch ) )
    {
	send_to_char( "Nie jeste`s w stanie kontrolowa`c kolejnego s`lugi.\n\r", ch );
	return FALSE;
    }

    pierwowzor = get_mob_index( cialo->value[ 2 ].v );
    poziom = pierwowzor ? URANGE( 0, pierwowzor->level, ch->level ) : 0;
    mumia = create_mobile( get_mob_index( MOB_VNUM_MUMIA ) );
    zmien_poziom_moba( mumia, poziom );

    timer = cialo->timer;

    for ( obj = cialo->contains; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted )
	    continue;

	obj_from_obj( obj );
	obj_to_char( obj, mumia );
    }

    /* teraz bedzie sie robic opis */
    if ( cialo->pIndexData->vnum == OBJ_VNUM_CORPSE_NPC
      && pierwowzor->race != race_lookup( "undead" ) )
    {
	sprintf( buf, "mumii %s", pierwowzor->dopelniacz );
	free_string( mumia->dopelniacz );
	mumia->dopelniacz = str_dup( buf );

	free_string( mumia->celownik );
	mumia->celownik = str_dup( buf );

	free_string( mumia->miejscownik );
	mumia->miejscownik = str_dup( buf );

	sprintf( buf, "mumi`e %s", pierwowzor->dopelniacz );
	free_string( mumia->biernik );
	mumia->biernik = str_dup( buf );

	sprintf( buf, "mumi`a %s", pierwowzor->dopelniacz );
	free_string( mumia->narzednik );
	mumia->narzednik = str_dup( buf );

	sprintf( buf, "mumio %s", pierwowzor->dopelniacz );
	free_string( mumia->wolacz );
	mumia->wolacz = str_dup( buf );

	sprintf( buf, "mumia %s", pierwowzor->dopelniacz );
	free_string( mumia->short_descr );
	mumia->short_descr = str_dup( buf );
    }
    else
	sprintf( buf, "mumia" );

    sprintf( buf1, mumia->long_descr, buf );
    free_string( mumia->long_descr );
    mumia->long_descr = str_dup( buf1 );

    sprintf( buf, mumia->description, ch->dopelniacz );
    free_string( mumia->description );
    mumia->description = str_dup( buf );

    if ( !IS_NPC( ch ) )
	mumia->area = get_room_index( ch->pcdata->strefa->przywolanie )->area;

    extract_obj( cialo );
    char_to_room( mumia, ch->in_room );
    act( ZM_WZROK | ZM_WID_VICT, "$n powo`luje do istnienia $#.", ch, NULL, mumia, TO_NOTVICT );

    add_follower( mumia, ch );
    af.type     = sn;
    af.duration = 5 + timer + level / 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_CHARM );
    SET_BIT_V( af.bitvector, AFF_SLUGA );
    affect_to_char( mumia, &af );

    if ( paf && !paf->deleted )
	paf->modifier += 1;
    else
    {
	af.type     = gsn_kontrola_umyslu;
	af.duration = -1;
	af.location = APPLY_NONE;
	af.modifier = 1;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	affect_to_char( ch, &af );
    }

    return TRUE;
}


CZAR( spell_koszmar )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if ( victim->position != POS_SLEEPING )
    {
	send_to_char( "Ten czar dzia`la tylko na `spi`ace osoby.\n\r", ch );
	return FALSE;
    }

    if ( saves_spell( level - 25 + ( get_curr_wis( ch ) - get_curr_wis( victim ) ) * 3, victim ) )
    {
	/* FIXME: brak komunikatu i dziwna reakcja ("zabij" zamiast "czaruj") */
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return FALSE;
    }

    dam = 3 * victim->hit;
    if ( HAS_SANCTUARY( victim ) )
	dam *= 2;
    dam = UMAX( 100, dam );
    victim->hit = 1;
    ASTC( "Twe sny przybieraj`a drastyczny obr`ot, czujesz si`e osaczon$y ze wszystkich\n\rstron. Spadasz w d`o`l do piek`la. Czujesz ogie`n pal`acy twoje trzewia.\n\rDooko`la demony zanosz`a si`e morderczym `smiechem.\n\rB`ol rozsadza twoj`a g`low`e.", victim );

    /* FIXME: zabojstwo nie zostanie zaliczone ch, tylko doliczone do
       "samobojstw" victim. */
    damage( victim, victim, dam, sn, WEAR_NONE, TRUE );

    return TRUE;
}


CZAR( spell_przeklety_deszcz )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *vch;
    CHAR_DATA *vch_nast;
    bool udane;

    if ( !IS_OUTSIDE( ch ) )
    {
	send_to_char( "Tego czaru nie mo`zesz rzuca`c w zamkni`etym pomieszczeniu.\n\r", ch );
	return FALSE;
    }

    if ( ch->in_room->area->weather_info.sky < SKY_RAINING )
    {
	send_to_char( "Potrzebujesz z`lej pogody.\n\r", ch );
	return FALSE;
    }

    send_to_char( "Przyzywasz moce chaosu, sprowadzaj`ac przekl`ety deszcz.\n\r", ch );
    act( ZM_SLUCH, "$n przyzywa moce chaosu, sprowadzaj`ac przekl`ety deszcz!", ch, NULL, NULL, TO_ROOM );

    udane = FALSE;
    for ( vch = ch->in_room->people; vch; vch = vch_nast )
    {
	vch_nast = vch->next_in_room;

	if ( vch->in_room != room )
	    break;

	if ( vch->deleted )
	    continue;

	if ( vch == ch || vch->race == zr_martwiak )
	{
	    int heal = dice( level / 4, 4 );

	    if ( vch->hit < vch->max_hit )
	    {
		send_to_char( "Przekl`ety deszcz nape`lnia ci`e o`zywcz`a energi`a.\n\r", vch );
		vch->hit = UMIN( vch->hit + heal,
				 UMAX( vch->max_hit, vch->hit ) );
		udane = TRUE;
	    }
	    else
		send_to_char( "Orze`xwia ci`e przyjemny ch`l`od przekl`etego deszczu.\n\r", vch );
	}
	else if ( IS_GOOD( vch ) && !is_safe( ch, vch, FALSE ) )
	{
	    int dam = dice( level / 2, 6 );

	    if ( saves_spell( level, vch ) )
		dam /= 2;

	    send_to_char( "Przekl`ety deszcz przenika ci`e swym ch`lodem.\n\r", vch );

	    damage( ch, vch, dam, sn, WEAR_NONE, FALSE );
	    udane = TRUE;

	    if ( ch->deleted || ch->in_room != room )
		return FALSE;
	}
	else
	    send_to_char( "Cienisty opad nie ma na ciebie wp`lywu.\n\r", vch );
    }

    return udane;
}


CZAR( spell_tarcza_umarlych )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_TARCZA_UMARLYCH )
      && !is_affected( victim, sn ) )
    {
	send_to_char( "Tarcza umar`lych ju`z ci`e chroni.\n\r", victim );
	return FALSE;
    }

    if ( IS_NPC( ch )
      || ( ch->klasa != CLASS_NEKROMANTA
	&& skill_get_trust( ch ) < LEVEL_IMMORTAL ) )
    {
	send_to_char( "Nie mo`zesz rzuci`c tego czaru.\n\r", victim );
	return FALSE;
    }

    af.type     = sn;
    af.duration = number_fuzzy( level / 4 );
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.level    = level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_TARCZA_UMARLYCH );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    send_to_char( "Otacza ci`e tarcza umar`lych.\n\r", victim );
    act( ZM_WZROK | ZM_WID_CHAR, "$n jest otoczon$y tarcz`a umar`lych.", victim, NULL, NULL, TO_ROOM );

    return TRUE;
}


CZAR( spell_wycienczenie )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( saves_spell( level, victim ) )
    {
	NIEUDANE( ch );
	return FALSE;
    }

    ASTC( "Czujesz, `ze odp`lywa z ciebie energia.", victim );
    if ( ch != victim )
	ASTC( "Powodujesz wycie`nczenie.", ch );

    if ( number_percent( ) < 33 )
    {
	ASTC( "Brak ci si`l na wykonanie jakiegokolwiek ruchu.", victim );

	/* Vigud 11.4.2010: skoro zmiana pozycji zostala wylaczona, ponizsze
	   komunikaty nie maja prawa bytu. Przy okazji znika problem
	   z postaciami, ktorych nie da sie przewrocic (np. latajace) - dopoki
	   ktos nie przywroci zmiany pozycji... */
/*
 	ASTC( "{WPrzewracasz si`e!{x", victim );
 	act( ZM_WZROK | ZM_WID_CHAR, "$n przewraca si`e.", victim, NULL, NULL, TO_ROOM );
*/

	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
/*	victim->position = POS_RESTING; */
    }
    else
    {
	OBJ_DATA *obj;

	if ( !( obj = get_eq_char( victim, WEAR_WIELD ) ) )
	    obj = get_eq_char( victim, WEAR_WIELD_2 );

	if ( number_percent( ) < 50
	  && obj
	  && !IS_OBJ_STAT( obj, ITEM_NOREMOVE )
	  && !IS_OBJ_STAT( obj, ITEM_NODROP ) )
	{

	    if ( !IS_NPC( victim )
	      && number_percent( ) < victim->pcdata->learned[ gsn_strong_wield ] / 2 )
	    {
		if ( ch != victim )
		    act( ZM_ZAWSZE, "{g$N mocno trzyma sw`a bro`n.{x", ch, NULL, victim, TO_CHAR );
		act( ZM_PRZYT, "{gUdaje ci si`e utrzyma`c bro`n w r`ece.{x", ch, NULL, victim, TO_VICT );

		uzyj( victim, gsn_strong_wield, "mocnym trzymaniu swej broni" );
	    }
	    else
	    {
		/* powinien byc oddzielny komunikat dla upuszczania/wypuszczania
		   wlaczajac oddzielny dla pojawiania sie broni */
		act( ZM_WZROK | ZM_WID_CHAR, "$n nie zdo`la`l$9 utrzyma`c broni.", victim, NULL, NULL, TO_ROOM );
		ASTC( "{RUpuszczasz bro`n!{x", victim );

		obj_from_char( obj );
		if ( IS_NPC( victim )
		  || IS_OBJ_STAT( obj, ITEM_LOYAL )
		  || IS_OBJ_STAT( obj, ITEM_QUEST )
		  || IS_OBJ_STAT( obj, ITEM_NODROP ) )
		{
		    obj_to_char( obj, victim );
		}
		else
		    obj_to_room( obj, victim->in_room );
	    }
	}
    }

    if ( !is_affected( victim, sn ) )
    {
	af.type     = sn;
	af.duration = level / 33;
	af.location = APPLY_STR;
	af.modifier = -3;
	af.level    = level;
	af.caster   = ch;
	CLEAR_BITS( af.bitvector );
	affect_join( victim, &af, ADD_AFFECT );
    }

    return TRUE;
}


CZAR( spell_cienisty_pocisk )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = dice( level, 10 );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, WEAR_NONE, DAMAGEBEZWALKI( ch, victim, usage ) );

    return TRUE;
}


/*
 * Lam 19.11.2005
 */
CZAR( spell_tarcza_zla )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( !IS_EVIL( ch ) )
    {
	ASTC( "Jeste`s zbyt dobr$y.", ch );
	return FALSE;
    }

    RZUCENIE_CZARU( ch, sn, usage );

    if ( HAS_PROTECT_GOOD( victim )
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
    SET_BIT_V( af.bitvector, AFF_TARCZA_ZLA );

    if ( !affect_join( victim, &af, ENH_MAX ) )
    {
	NIEUDANE_BC( ch );
	return FALSE;
    }

    WZMOCNIONY( ch, sn, wzmocnienie_affect_join );

    if ( ch != victim )
	send_to_char( "Rzucasz tarcz`e z`la.\n\r", ch );
    ASTC( "Czujesz si`e chronion$y przed dobrem.", victim );

    return TRUE;
}
