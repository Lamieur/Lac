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
 *  Funkcje obslugi struktur i ich list.                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "merc.h"
#include <errno.h> /* errno jest uzywany przez czytaj_ciala( ) */


/*
 * Globalna zmienna eksportowa.
 */
bool	wzmocnienie_affect_join;

/*
 * Local functions.
 */
void	affect_modify		args( ( CHAR_DATA *ch, AFFECT_DATA *paf,
				      bool fAdd, bool check_wield ) );
FUNKCJA_CZASOWA( przyzwyczajenie_oczu );
void	komunikat_parzenia	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
CHAR_DATA *real_get_char_room	args( ( CHAR_DATA *ch, char *argument, int *count ) );
CHAR_DATA *real_get_char_room2	args( ( CHAR_DATA *ch, char *argument, int *count ) );
OBJ_DATA *real_get_obj_here	args( ( CHAR_DATA *ch, char *argument, int *count ) );
char	*choice_field_to_name	args( ( struct choice_field_data *values, int value,
				bool trusted ) );
bool	choice_field_avail	args( ( struct choice_field_data *values, int value ) );
char	*bit_field_to_name	args( ( struct bit_field_data *values, int value,
				bool trusted ) );
bool	bit_field_avail		args( ( struct bit_field_data *values, int value ) );
char	*bitvector_field_to_name args( ( struct bitvector_field_data *values,
				int *value, bool trusted, bool dla_ludzi ) );
bool	bitvector_field_avail	args( ( struct bitvector_field_data *values,
				int *value, int size ) );
void	zapisz_ciala		args( ( void ) );
void	czytaj_ciala		args( ( void ) );
void	wywal_smieci_z_listy	args( ( OBJ_DATA *lista, int *klucze,
					int *drogowskazy ) );

/*
 * Lam 24.8.98: funkcje czasowe
 */
TIMER_DATA	*timer_list;
TIMER_DATA	*timer_free;
int		top_timer;


TIMER_DATA *new_timer( void )
{
    TIMER_DATA *timer;

    if ( !timer_free )
    {
	++top_timer;
	return calloc( 1, sizeof( TIMER_DATA ) );
    }

    timer	= timer_free;
    timer_free	= timer->next;
    timer->ch	= NULL; /* na wszelki wypadek */
    timer->paused = FALSE;

    return timer;
}


void del_timer( TIMER_DATA *timer )
{
    TIMER_DATA *temp;

    if ( !timer )
    {
	bug( "Del_timer: NULL timer!", 0 );
	return;
    }

    if ( timer_list == timer )
    {
	timer_list  = timer->next;
	timer->next = timer_free;
	timer_free  = timer;
	return;
    }

    for ( temp = timer_list; temp; temp = temp->next )
	if ( temp->next == timer )
	{
	    temp->next = timer->next;
	    timer->next = timer_free;
	    timer_free = timer;
	    return;
	}

    bug( "del_timer - timer not found", 0 );

    return;
}


TIMER_DATA *find_timer( CHAR_DATA *ch, TIMER_FUN *fun )
{
    TIMER_DATA *temp;

    for ( temp = timer_list; temp; temp = temp->next )
	if ( temp->ch == ch && temp->fun == fun )
	    break;

    return temp;
}


void run_timer( TIMER_DATA *timer )
{
    if ( timer->time < 1 )
    {
	bug( "run_timer: timer->time == %d", timer->time );
	return;
    }
    timer->next = timer_list;
    timer_list = timer;

    return;
}


void timer_update( void )
{
    TIMER_DATA *timer, *timer_next;

    for ( timer = timer_list; timer; timer = timer_next )
    {
	timer_next = timer->next;
	if ( !timer->paused && --timer->time <= 0 )
	{
	    /* Lam 26.5.99: najpierw jest usuwany z listy, nastepnie wywolywana
	       jest funkcja. Dzieki temu funkcja moze sprawdzic, czy jest na
	       liscie jeszcze jedna jej instancja. */
	    del_timer( timer );
	    timer->fun( timer );
	}
    }

    return;
}


/*
 * Lam 28.3.99: teraz przedmioty zawieszone w powietrzu beda spadac
 */
AIROBJ_DATA	*airobj_list;
AIROBJ_DATA	*airobj_free;


AIROBJ_DATA *new_airobj( OBJ_DATA *obj )
{
    AIROBJ_DATA *airobj;

    /* chyba nawet nie ma prawa zaistniec taka sytuacja */
    if ( obj->deleted )
	return NULL;

    if ( !airobj_free )
	airobj = calloc( 1, sizeof( AIROBJ_DATA ) );
    else
    {
	airobj		= airobj_free;
	airobj_free	= airobj->next;
    }

    airobj->obj = obj;
/*    airobj->wysokosc = 0; */

    airobj->next = airobj_list;
    airobj_list = airobj;

    return airobj;
}


AIROBJ_DATA *find_airobj( OBJ_DATA *obj )
{
    AIROBJ_DATA *airobj;

    for ( airobj = airobj_list; airobj; airobj = airobj->next )
	if ( airobj->obj == obj )
	    return airobj;

    return NULL;
}


void del_airobj( AIROBJ_DATA *airobj )
{
    AIROBJ_DATA *temp;

    if ( !airobj )
    {
	bug( "del_airobj: NULL airobj!", 0 );
	return;
    }

    if ( airobj_list == airobj )
    {
	airobj_list  = airobj->next;
	airobj->next = airobj_free;
	airobj_free  = airobj;
	return;
    }

    for ( temp = airobj_list; temp; temp = temp->next )
	if ( temp->next == airobj )
	{
	    temp->next = airobj->next;
	    airobj->next = airobj_free;
	    airobj_free = airobj;
	    return;
	}

    bug( "del_airobj - airobj not found in airobj_list", 0 );

    return;
}


/*
 * Alandar 11.11.2003: skopiowane z air_obj
 * Alandar 15.11.2003: modyfikacje
 * Alandar: postacie w powietrzu po spadnieciu latania beda spadac w dol
 * Lam 27.9.2005: rozszerzenie na opadanie pod wode az na dno
 */
AIRCHAR_DATA	*airchar_list;
AIRCHAR_DATA	*airchar_free;

AIRCHAR_DATA *new_airchar( CHAR_DATA *chr )
{
    AIRCHAR_DATA *airchar;

    if ( !airchar_free )
	airchar = calloc( 1, sizeof( AIRCHAR_DATA ) );
    else
    {
	airchar		= airchar_free;
	airchar_free	= airchar->next;
    }

    airchar->chr = chr;
    airchar->wysokosc = 0;
    airchar->slaby_plywak = FALSE;

    airchar->next = airchar_list;
    airchar_list = airchar;

    return airchar;
}


AIRCHAR_DATA *find_airchar( CHAR_DATA *chr )
{
    AIRCHAR_DATA *airchar;

    for ( airchar = airchar_list; airchar; airchar = airchar->next )
	if ( airchar->chr == chr )
	    return airchar;

    return NULL;
}


/* calkowicie przepisane przez Lama */
void check_airchar( CHAR_DATA *chr )
{
    bool powinien = FALSE;

    if ( !chr->deleted
      && chr->in_room->exit[ DIR_DOWN ]
      && chr->in_room->exit[ DIR_DOWN ]->to_room
      && !IS_SET( chr->in_room->exit[ DIR_DOWN ]->exit_info, EX_CLOSED ) )
    {
	if ( CZY_POWIETRZE( chr->in_room )
	  && !IS_FLYING( chr ) )
	{
	    powinien = TRUE;
	}

	if ( !powinien
	  && chr->in_room->sector_type == SECT_WATER_DEEP
	  && !IS_FLYING( chr )
	  && !IS_SET( race_table[ chr->race ].poruszanie, POR_POD_WODA )
	  && !IS_SET( race_table[ chr->race ].poruszanie, POR_PO_WODZIE )
	  && !IS_AFFECTED( chr, AFF_KONTROLA_NAD_WODA )
	  && ( IS_NPC( chr )
	    || !chr->pcdata->learned[ gsn_plywanie ]
	    || !IS_AWAKE( chr ) )
	  && !jest_w_lodzi( chr ) )
	{
	    powinien = TRUE;
	}

	if ( !powinien
	  && CZY_POD_WODA( chr->in_room )
	  && !IS_SET( race_table[ chr->race ].poruszanie, POR_POD_WODA )
	  && !IS_AFFECTED( chr, AFF_KONTROLA_NAD_WODA )
	  && ( IS_NPC( chr )
	    || !chr->pcdata->learned[ gsn_plywanie ]
	    || !IS_AWAKE( chr ) ) )
	{
	    powinien = TRUE;
	}
    }

    if ( powinien )
    {
	AIRCHAR_DATA *airchar = find_airchar( chr );
	if ( !airchar )
	    new_airchar( chr );
    }

    return;
}


void del_airchar( AIRCHAR_DATA *airchar )
{
    AIRCHAR_DATA *temp;

    if ( !airchar )
    {
	bug( "del_airchar: NULL airchar!", 0 );
	return;
    }

    if ( airchar_list == airchar )
    {
	airchar_list  = airchar->next;
	airchar->next = airchar_free;
	airchar_free  = airchar;
	return;
    }

    for ( temp = airchar_list; temp; temp = temp->next )
	if ( temp->next == airchar )
	{
	    temp->next = airchar->next;
	    airchar->next = airchar_free;
	    airchar_free = airchar;
	    return;
	}

    bug( "del_airchar - airchar not found in airchar_list", 0 );

    return;
}


/*
 * Lam: funkcja sprawdzajaca, czy gracz wybral umiejetnosc przy zakladaniu
 * postaci i czy moze ja cwiczyc/widziec w "umiejetnosciach" itd.
 * sn - skill number
 */
bool ma_w_multi( CHAR_DATA *ch, int sn )
{
    int i;

    if ( IS_NPC( ch ) )
    {
	bug( "ma_w_multi( ) - NPC!", 0 );
	return FALSE;
    }

    for ( i = 0; i < 5; i++ )
	if ( ch->pcdata->multi[ i ] == sn )
	    return TRUE;

    return FALSE;
}


/*
 * Lam 9.4.2007: Lac zmienil podejscie na odwrotne - zamiast wybierac cos
 * z innych profesji, trzeba konczyc gre wieloma profesjami, a w nagrode mozna
 * cwiczyc ich umiejetnosci na poziomach 101-102.
 *
 * Funkcja wypelnia tablice poziomow, ktore postac osiagnela wczesniej w swojej
 * historii. Na tej podstawie "poziomy" raportuje, a "cwicz" decyduje, co mozna
 * cwiczyc.
 */
void wypelnij_poziomy( CHAR_DATA *ch, int *poziomy )
{
    char buf[ MIL ];
    RESTART_DATA *restart;
    char *wpis;
    int i, poz;

    for ( i = 0; i < MAX_CLASS; i++ )
	poziomy[ i ] = 0;

    if ( IS_NPC( ch ) )
    {
	cbug( "mob", 0 );
	return;
    }

    poziomy[ ch->klasa ] = ch->level;

    for ( restart = ch->pcdata->restart; restart; restart = restart->nast )
    {
	wpis = restart->wpis;
	wpis++; /* < albo ( */
	wpis = one_argument( wpis, buf ); /* poziom */
	poz = atoi( buf );
	one_argument( wpis, buf ); /* profesja */

	for ( i = 0; i < MAX_CLASS; i++ )
	    if ( !str_cmp( buf, class_table[ i ].pl_name ) )
		break;

	if ( i < MAX_CLASS )
	{
	    if ( poz > poziomy[ i ] )
		poziomy[ i ] = poz;
	}
	else
	{
	    sprintf( bug_buf, "Dziwna profesja '%s', poziom %d, postac %s.",
		buf, poz, ch->name );
	    cbug( bug_buf, 0 );
	}
    }

    return;
}


/*
 * Funkcja rowniez zwiazana z wieloprofesjowoscia; sprawdza, czy postac moze
 * uzywac danej umiejetnosci, a by moc uzywac, musi ja moc cwiczyc w sposob
 * normalny lub wybierajac w multi.
 * Lam 31.3.2005: lub uustaw
 */
bool moze_uzyc( CHAR_DATA *ch, int sn )
{
    int poziomy[ MAX_CLASS ];
    int i;

    if ( IS_NPC( ch ) )
	return !IS_AFFECTED( ch, AFF_CHARM );

    if ( ch->pcdata->learned[ sn ] )
	return TRUE;

    wypelnij_poziomy( ch, poziomy );

    if ( ch->level >= L_APP && ch->level < L_JUN ) /* restartowcy */
	for ( i = 0; i < MAX_CLASS; i++ )
	    if ( poziomy[ i ] >= 100
	      && skill_table[ sn ].skill_level[ i ] < L_APP )
	    {
		return TRUE;
	    }

    if ( ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
      && ch->level < skill_table[ sn ].multi_level[ ch->klasa ] )
    {
	return FALSE;
    }

    if ( ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
      && !ma_w_multi( ch, sn ) )
    {
	return FALSE;
    }

    return TRUE;
}


/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust( CHAR_DATA *ch )
{
    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

    if ( ch->trust > ch->level )
	return ch->trust;

    if ( IS_NPC( ch ) && ch->level >= LEVEL_HERO )
	return LEVEL_HERO - 1;

    return ch->level;
}


int skill_get_trust( CHAR_DATA *ch )
{
    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

    if ( IS_NPC( ch ) && ch->level >= LEVEL_HERO )
	return LEVEL_HERO - 1;

    return ch->level < LEVEL_HERO ? ch->level : UMAX( ch->level, ch->trust );
}


int get_clanstat( CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) )
	return -1;
    else
	return ch->pcdata->clan ? ch->pcdata->clan_member->level : CLAN_NONE;
}


/*
 * Retrieve a character's age.
 */
int get_age( CHAR_DATA *ch )
{
    return 17 + ( ch->played + (int) ( current_time - ch->logon ) ) / 428400;

    /* 428400 assumes 30 secs/mud hour * 24 hours/day * 35 days/month *
       17 months/year - Kahn */
}


/*
 * Retrieve character's current strength.
 */
int get_curr_str( CHAR_DATA *ch )
{
    int max;
    int mod = race_table[ ch->race ].str_mod;
    int sila_wlasciwa;

    if ( IS_NPC( ch ) )
    {
	sila_wlasciwa = 13 + mod;
	max = UMIN( 23 + mod, 26 );
    }
    else
    {
	sila_wlasciwa = UMIN( ch->pcdata->perm_str, 18 + mod );

	if ( class_table[ ch->klasa ].attr_prime == APPLY_STR )
	    max = UMIN( 30, 25 + mod );
	else
	    max = UMIN( 23 + mod, 26 );
    }

    return URANGE( 3, sila_wlasciwa + ch->mod_str, max );
}


/*
 * Retrieve character's current intelligence.
 */
int get_curr_int( CHAR_DATA *ch )
{
    int max;
    int mod = race_table[ ch->race ].int_mod;
    int inteligencja_wlasciwa;

    if ( IS_NPC( ch ) )
    {
	inteligencja_wlasciwa = 13 + mod;
	max = UMIN( 23 + mod, 26 );
    }
    else
    {
	inteligencja_wlasciwa = UMIN( ch->pcdata->perm_int, 18 + mod );

	if ( class_table[ ch->klasa ].attr_prime == APPLY_INT )
	    max = UMIN( 30, 25 + mod );
	else
	    max = UMIN( 23 + mod, 26 );
    }

    return URANGE( 3, inteligencja_wlasciwa + ch->mod_int, max );
}


/*
 * Retrieve character's current wisdom.
 */
int get_curr_wis( CHAR_DATA *ch )
{
    int max;
    int mod = race_table[ ch->race ].wis_mod;
    int madrosc_wlasciwa;

    if ( IS_NPC( ch ) )
    {
	madrosc_wlasciwa = 13 + mod;
	max = UMIN( 23 + mod, 26 );
    }
    else
    {
	madrosc_wlasciwa = UMIN( ch->pcdata->perm_wis, 18 + mod );

	if ( class_table[ ch->klasa ].attr_prime == APPLY_WIS )
	    max = UMIN( 30, 25 + mod );
	else
	    max = UMIN( 23 + mod, 26 );
    }

    return URANGE( 3, madrosc_wlasciwa + ch->mod_wis, max );
}


/*
 * Retrieve character's current dexterity.
 */
int get_curr_dex( CHAR_DATA *ch )
{
    int max;
    int mod = race_table[ ch->race ].dex_mod;
    int zrecznosc_wlasciwa;

    if ( IS_NPC( ch ) )
    {
	zrecznosc_wlasciwa = 13 + mod;
	max = UMIN( 23 + mod, 26 );
    }
    else
    {
	zrecznosc_wlasciwa = UMIN( ch->pcdata->perm_dex, 18 + mod );

	if ( class_table[ ch->klasa ].attr_prime == APPLY_DEX )
	    max = UMIN( 30, 25 + mod );
	else
	    max = UMIN( 23 + mod, 26 );
    }

    return URANGE( 3, zrecznosc_wlasciwa + ch->mod_dex, max );
}


/*
 * Retrieve character's current constitution.
 */
int get_curr_con( CHAR_DATA *ch )
{
    int max;
    int mod = race_table[ ch->race ].con_mod;
    int budowa_wlasciwa;

    if ( IS_NPC( ch ) )
    {
	budowa_wlasciwa = 13 + mod;
	max = UMIN( 23 + mod, 26 );
    }
    else
    {
	budowa_wlasciwa = UMIN( ch->pcdata->perm_con, 18 + mod );

	if ( class_table[ ch->klasa ].attr_prime == APPLY_CON )
	    max = UMIN( 30, 25 + mod );
	else
	    max = UMIN( 23 + mod, 26 );
    }

    return URANGE( 3, budowa_wlasciwa + ch->mod_con, max );
}


/*
 * Retrieve character's current hitroll for given weapon location
 */
int get_hitroll( CHAR_DATA *ch, int wpn )
{
    OBJ_DATA    *other_wield;
    AFFECT_DATA *paf;
    int          other_wpn;
    int          hitroll;

    if ( wpn == WEAR_WIELD)
	other_wpn = WEAR_WIELD_2;
    else if ( wpn == WEAR_WIELD_2 )
	other_wpn = WEAR_WIELD;
    else
    {
	char buf[ MAX_STRING_LENGTH ];
	sprintf( buf, "get_hitroll: Invalid weapon location %d on %s.",
		wpn, ch->name );
	bug( buf, 0 );
	return 0;
    }

    hitroll = ch->hitroll;
    if ( !( other_wield = get_eq_char( ch, other_wpn ) ) )
	return hitroll;

    for ( paf = other_wield->pIndexData->affected; paf; paf = paf->next )
	if ( paf->location == APPLY_HITROLL )
	    hitroll -= paf->modifier;

    for ( paf = other_wield->affected; paf; paf = paf->next )
	if ( paf->location == APPLY_HITROLL )
	    hitroll -= paf->modifier;

    return hitroll;
}


/*
 * Retrieve character's current damroll for given weapon location
 */
int get_damroll( CHAR_DATA *ch, int wpn )
{
    OBJ_DATA    *other_wield;
    AFFECT_DATA *paf;
    int          other_wpn;
    int          damroll;

    if ( wpn == WEAR_WIELD)
	other_wpn = WEAR_WIELD_2;
    else if ( wpn == WEAR_WIELD_2 )
	other_wpn = WEAR_WIELD;
    else
    {
	char buf[ MAX_STRING_LENGTH ];
	sprintf( buf, "get_damroll: Invalid weapon location %d on %s.",
		wpn, ch->name );
	bug( buf, 0 );
	return 0;
    }

    damroll = ch->damroll;
    if ( !( other_wield = get_eq_char( ch, other_wpn ) ) )
	return damroll;

    for ( paf = other_wield->pIndexData->affected; paf; paf = paf->next )
	if ( paf->location == APPLY_DAMROLL )
	    damroll -= paf->modifier;

    for ( paf = other_wield->affected; paf; paf = paf->next )
	if ( paf->location == APPLY_DAMROLL )
	    damroll -= paf->modifier;

    return damroll;
}


/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n( CHAR_DATA *ch )
{
    if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
	/* Lam: bylo 1000, zmienilem na 200, ale i tak sa sakwy, wiec wraca 1000 */
	return 1000;

    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	return 0;

    /* Lam: + ch->level / 10 */
    return MAX_WEAR + get_curr_dex( ch ) + ch->level / 10;
}


/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w( CHAR_DATA *ch )
{
    if ( !IS_NPC( ch ) && ch->level >= L_SEN )
	return 40000;

    if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
	return 4000;

    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	return 0;

    return str_app[ get_curr_str( ch ) ].carry;
}


/*
 * Apply or remove an affect to a character.
 */
void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd, bool check_wield )
{
    OBJ_DATA *wield;
    OBJ_DATA *wield2;
    char      buf[ MAX_STRING_LENGTH ];
    int       mod;

    mod = paf->modifier;

    if ( fAdd )
	OR_BITS( ch->affected_by, paf->bitvector );
    else
    {
	AND_NOT_BITS( ch->affected_by, paf->bitvector );
	mod = 0 - mod;
    }

    switch ( paf->location )
    {
    default:
	sprintf( buf, "Affect_modify: unknown location %d on %s.",
		paf->location, ch->name );
	bug( buf, 0 );
	return;

    case APPLY_NONE:                                            break;
    /* Lam 13.1.2004: moby tez moga miec wieksze atrybuty */
    case APPLY_STR:
	ch->mod_str += mod;
	break;
    case APPLY_DEX:
	ch->mod_dex += mod;
	break;
    case APPLY_INT:
	ch->mod_int += mod;
	break;
    case APPLY_WIS:
	ch->mod_wis += mod;
	break;
    case APPLY_CON:
	ch->mod_con += mod;
	break;
    /* PLEC? Przeciez to tablica, a nie ma zadnego sprawdzania zakresow! */
    case APPLY_SEX:         ch->sex                 += mod;	break;
    /* RASE? jw. */
    case APPLY_RACE:        ch->race                += mod;	break;
    case APPLY_SIZE:						break;
    case APPLY_CLASS:                                           break;
    case APPLY_LEVEL:                                           break;
    case APPLY_AGE:                                             break;
    case APPLY_HEIGHT:                                          break;
    case APPLY_WEIGHT:                                          break;
    case APPLY_MANA:
	ch->max_mana += mod;
	if ( check_wield ) ch->mana = UMIN( ch->mana, UMAX( ch->max_mana, ch->mana + mod ) );
	break;
    case APPLY_HIT:
	ch->max_hit += mod;
	if ( check_wield ) ch->hit = UMIN( ch->hit, UMAX( ch->max_hit, ch->hit + mod ) );
	break;
    case APPLY_MOVE:
	ch->max_move += mod;
	if ( check_wield ) ch->move = UMIN( ch->move, UMAX( ch->max_move, ch->move + mod ) );
	break;
    case APPLY_GOLD:                                            break;
    case APPLY_EXP:                                             break;
    case APPLY_AC:            ch->marmor                += mod; break;
    case APPLY_HITROLL:       ch->hitroll               += mod; break;
    case APPLY_DAMROLL:       ch->damroll               += mod; break;
    case APPLY_SAVING_PARA:   ch->saving_throw          += mod; break;
    case APPLY_SAVING_ROD:    ch->saving_throw          += mod; break;
    case APPLY_SAVING_PETRI:  ch->saving_throw          += mod; break;
    case APPLY_SAVING_BREATH: ch->saving_throw          += mod; break;
    case APPLY_SAVING_SPELL:  ch->saving_throw          += mod; break;
    }

    if ( IS_NPC( ch ) || !check_wield )
	return;

    /*
     * Check for PC weapon wielding.
     * Guard against recursion (for weapons with affects).
     * If more than one weapon, drop weapon 2 first, then recheck.
     * And yes, it does work.  :)  --- Thelonius (Monk)
     */
    if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) )
    {
	if ( ( wield2 = get_eq_char( ch, WEAR_WIELD_2 ) ) )
	{
	    if ( ( ( get_obj_weight( wield ) + get_obj_weight( wield2 ) )
		  > str_app[ get_curr_str( ch ) ].wield )
		|| !IS_SET( race_table[ ch->race ].race_abilities,
			   RACE_WEAPON_WIELD ) )
	    {
		static int depth;

		if ( depth == 0 )
		{
		    depth++;

		    if ( IS_OBJ_STAT( wield2, ITEM_LOYAL )
		      || IS_OBJ_STAT( wield2, ITEM_NODROP ) )
		    {
			act( ZM_PRZYT, "Wypuszczasz $j.", ch, wield2, NULL, TO_CHAR );
			act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wypuszcza $j.", ch, wield2, NULL, TO_ROOM );
			obj_from_char( wield2 );
			obj_to_char( wield2, ch );
		    }
		    else
		    {
			act( ZM_PRZYT, "Upuszczasz $j.", ch, wield2, NULL, TO_CHAR );
			act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n upuszcza $j.", ch, wield2, NULL, TO_ROOM );
			obj_from_char( wield2 );
			obj_to_room( wield2, ch->in_room );
			/* po upuszczeniu, bo dopiero wtedy przedmiot staje sie
			   widzialny */
			if ( wield2->rodzaj == 3 )
			    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawiaj`a si`e tu $p.", ch, wield2, NULL, TO_ROOM );
			else
			    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, wield2, NULL, TO_ROOM );
		    }

		    depth--;
		}
	    }
	}
	else if ( ( get_obj_weight( wield ) > str_app[ get_curr_str( ch ) ].wield )
	       || !IS_SET( race_table[ ch->race ].race_abilities, RACE_WEAPON_WIELD ) )
	{
	    static int depth;

	    if ( depth == 0 )
	    {
		depth++;

		if ( IS_OBJ_STAT( wield, ITEM_LOYAL )
		  || IS_OBJ_STAT( wield, ITEM_NODROP ) )
		{
		    act( ZM_PRZYT, "Wypuszczasz $j.", ch, wield, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wypuszcza $j.", ch, wield, NULL, TO_ROOM );
		    obj_from_char( wield );
		    obj_to_char( wield, ch );
		}
		else
		{
		    act( ZM_PRZYT, "Upuszczasz $j.", ch, wield, NULL, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n upuszcza $j.", ch, wield, NULL, TO_ROOM );
		    obj_from_char( wield );
		    obj_to_room( wield, ch->in_room );
		    if ( wield->rodzaj == 3 )
			act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawiaj`a si`e tu $p.", ch, wield, NULL, TO_ROOM );
		    else
			act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, wield, NULL, TO_ROOM );
		}

		depth--;
	    }
	}
    }
    else if ( ( wield2 = get_eq_char( ch, WEAR_WIELD_2 ) )
	   && ( get_obj_weight( wield2 ) > str_app[ get_curr_str( ch ) ].wield
	     || !IS_SET( race_table[ ch->race ].race_abilities, RACE_WEAPON_WIELD ) ) )
    {
	static int depth;

	if ( depth == 0 )
	{
	    depth++;

	    if ( IS_OBJ_STAT( wield2, ITEM_LOYAL )
	      || IS_OBJ_STAT( wield2, ITEM_NODROP ) )
	    {
		act( ZM_PRZYT, "Wypuszczasz $j.", ch, wield2, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n wypuszcza $j.", ch, wield2, NULL, TO_ROOM );
		obj_from_char( wield2 );
		obj_to_char( wield2, ch );
	    }
	    else
	    {
		act( ZM_PRZYT, "Upuszczasz $j.", ch, wield2, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n upuszcza $j.", ch, wield2, NULL, TO_ROOM );
		obj_from_char( wield2 );
		obj_to_room( wield2, ch->in_room );
		if ( wield2->rodzaj == 3 )
		    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawiaj`a si`e tu $p.", ch, wield2, NULL, TO_ROOM );
		else
		    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, wield2, NULL, TO_ROOM );
	    }

	    depth--;
	}
    }

    return;
}


/*
 * Give an affect to a char.
 */
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new;

    /* Lista dopuszczonych czarow wyglada zle i sugeruje uzycie flagi
       w skill_list, ale to tylko odpluskwiacz.
       W tym kodzie chodzi o namierzenie czarow, ktore ustawiaja czas trwania
       za pomoca wzoru (np. duration = level - 10). To, ze czar jest dostepny
       od poziomu 20, nie znaczy, ze tak bedzie zawsze, lub ze nikt nie zrobi
       z nim przedmiotu z poziomem 1. */
    if ( paf->duration < 0
      && paf->type != gsn_berserk
      && paf->type != gsn_kontrola_umyslu )
    {
	bug( "paf->duration == %d", paf->duration );
	paf->duration = 0;
    }
    paf_new             = new_affect( );

    *paf_new            = *paf;
    paf_new->deleted    = FALSE;
    paf_new->next       = ch->affected;
    ch->affected        = paf_new;
    if ( paf_new->caster )
	dod_zwod( &paf_new->caster->zwod_affected, ch );

    affect_modify( ch, paf_new, TRUE, TRUE );

    return;
}


/*
 * Remove an affect from a char.
 */
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf, bool checks )
{
    ZWOD_DATA *zwod;

    if ( !ch->affected )
    {
	bug( "Affect_remove: no affect.", 0 );
	return;
    }

    /* Lam 11.1.2008: checks, czasem nie musimy sprawdzac */
    affect_modify( ch, paf, FALSE, checks );

    paf->deleted = TRUE;
    /* nie czyszcze zwod u paf->castera, Lac zapewnia to, ze ch bedzie
       w char_list przynajmniej tak dlugo, jak paf na ch i zdejmie go dopiero
       list_update, wiec ani ch->affected nie zgubi wskaznika na paf->castera,
       ani w paf->caster->zwod_affected nie zostanie wskaznik na ch po smierci */
    /* 19.10.2008: z powodu drugiego padu w ciagu ostatnich dwoch lat, jednak to
       sprawdzam. Nie powinno nigdy wyskoczyc, ale nie wiadomo, skad sie bierze
       ten blad, wiec szukam, gdzie sie da.
       FIXME: usunac kiedy wykryjemy blad. */
    if ( paf->caster )
    {
	if ( !( zwod = paf->caster->zwod_affected ) )
	{
	    sprintf( bug_buf, "ch %s; caster %s; paf type %d, level %d, mod %d: brak zwod_affected",
		ch->short_descr, paf->caster->short_descr, paf->type,
		paf->level, paf->modifier );
	    cbug( bug_buf, 0 );
	}
	else
	{
	    for ( ; zwod; zwod = zwod->next )
		if ( zwod->ch == ch )
		    break;

	    if ( !zwod )
	    {
		sprintf( bug_buf, "ch %s; caster %s; paf type %d, level %d, mod %d: brak zwod_affected",
		    ch->short_descr, paf->caster->short_descr, paf->type,
		    paf->level, paf->modifier );
		cbug( bug_buf, 0 );
	    }
	}
    }

    /* Jesli nie sprawdzamy ubrania, to i bitow nie musimy. Przynajmniej
       dopoki jedynym uzytkownikiem checks == FALSE jest affect_join z ENH_MAX
       (mamy tam pewnosc, ze nie zmienia sie bity, a reakcje na ich zmiane
       tylko by cos popsuly). */
    if ( !checks )
	return;

    /* Lam 11.8.2005: nadal swiatla znosza dzialanie strefy cienia */
    if ( IS_SET_V( paf->bitvector, AFF_MROK )
      && ch->in_room && !ch->in_room->light )
    {
	sprawdz_przyzwyczajanie( ch->in_room );
    }

    /* Lam 13.9.2005 */
    if ( IS_SET_V( paf->bitvector, AFF_CHARM ) )
    {
	if ( !ch->master )
	{
	    sprintf( bug_buf, "brak ch->master, ch \"%s\", paf->type \"%s\" paf->caster \"%s\"",
		ch->short_descr,
		skill_table[ paf->type ].pl_name,
		paf->caster ? paf->caster->short_descr : "(brak)" );
	    bug( bug_buf, 0 );
	}
	else
	{
	    /* Lam 16.7.2008, #2391 */
	    if ( IS_SET_V( paf->bitvector, AFF_SLUGA ) )
		zwolnij_sluge( ch->master );
	    stop_follower( ch );
	}
    }

    /* sprawdza czy przypadkiem postac nie przestala latac w powietrzu */
    if ( !ch->deleted )
	check_airchar( ch );

    return;
}


/*
 * Strip all affects of a given sn.
 */
void affect_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf; paf = paf->next )
	if ( !paf->deleted
	  && paf->type == sn )
	{
	    affect_remove( ch, paf, TRUE );
	}

    return;
}


/*
 * Return true if a char is affected by a spell.
 */
bool is_affected( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf; paf = paf->next )
	if ( !paf->deleted
	  && paf->type == sn )
	{
	    return TRUE;
	}

    return FALSE;
}


/*
 * Add or enhance an affect.
 * Alandar 17.01.2004: Modyfikacje
 *
 * Lam 12.6.2006: ENH_MAX. Dziala to tak, ze wyjsciowy wplyw dostaje maksimum
 * z wplywu i czasu dzialania obu (starego i nowego), ale bity ZAWSZE ze
 * starego. Dzieki temu czary, ktore dodaja bity w zaleznosci od tego, czy
 * jakies juz sa "na postaci", nie musza byc modyfikowane pod katem ENH_MAX,
 * bo nie popsuja bitow postaci, ale takze nie moga dodac bitow, ktorych np.
 * nie udalo sie ustawic rzucajac czar poprzednio (choc zaden taki czar mi nie
 * przychodzi mi do glowy, zakazenie jest ofensywne)
 *
 * Zmieniam tez void na bool - da znac, czy cos udalo sie polepszyc, aby czary,
 * ktore chca sie wzmacniac, mogly uzyc NIEUDANE_BC w przeciwnym przypadku.
 */
bool affect_join( CHAR_DATA *ch, AFFECT_DATA *paf, int how )
{
    AFFECT_DATA *paf_old;
    bool polepszono = FALSE;

    /* Globalny wskaznik dla ENH_MAX, wygodniejsza zmiana istniejacych
       dziesiatek czarow niz przez wartosc zwracana. Por. WZMOCNIONY( ) */
    wzmocnienie_affect_join = FALSE;

    /* Sprawdza czy znany sposob kumulowania zostal uzyty */
    if ( how < ADD_AFFECT || how > ENH_MAX )
    {
	bug( "affect_join: Nieznany sposob kumulowania wplywow, przyjmuje dodawanie", 0 );
	how = ADD_AFFECT;
    }

    for ( paf_old = ch->affected; paf_old; paf_old = paf_old->next )
    {
	if ( paf_old->deleted )
	    continue;

	/* Lam 12.6.2006: wczesniej sprawdzano tylko ->type, ale to nie
	   obslugiwalo czarow zostawiajacych kilka wplywow, jak blogoslawienstwo */
	if ( paf_old->type == paf->type
	  && paf_old->location == paf->location )
	{
	    /* pozniej bedzie potrzebne */
	    int paf_modifier = paf->modifier;

	    /* zapamietuje wiekszy z dwoch poziomow */
	    paf->level = UMAX( paf->level, paf_old->level );

	    /* najpierw wyznaczamy czas trwania */
	    if ( how == ENH_MAX )
	    {
		COPY_BITS( paf->bitvector, paf_old->bitvector );

		if ( paf->duration > paf_old->duration )
		    polepszono = TRUE;
		else
		    paf->duration = paf_old->duration;
	    }
	    else if ( how == ADD_AFFECT || how == ENH_MOD || paf_old->duration == 0 )
		paf->duration += paf_old->duration;
	    else
		paf->duration = URANGE( 1, paf_old->duration +
		  paf->duration * paf->duration / paf_old->duration,
		  paf_old->duration + paf->duration );

	    /* potem modyfikator */
	    if ( how == ENH_MAX )
	    {
		/* Lam: jesli sa czary, ktore daja wplyw raz dodatni, raz
		   ujemny, to ja ich nie znam, ale niech nie uzywaja ENH_MAX */
		if ( paf->modifier >= 0 )
		{
		    if ( paf->modifier > paf_old->modifier )
			polepszono = TRUE;
		    else
			paf->modifier = paf_old->modifier;
		}
		else
		{
		    if ( paf->modifier < paf_old->modifier )
			polepszono = TRUE;
		    else
			paf->modifier = paf_old->modifier;
		}
	    }
	    else if ( how == ADD_AFFECT || how == ENH_TIME || paf_old->modifier == 0 )
		paf->modifier += paf_old->modifier;
	    else
	    {
		int absmod = ABSOLUTE( paf->modifier );
		/* wiem, ze moznaby to napisac prosciej, ale wtedy cos nie
		   dziala z ulamkami */
		paf->modifier = paf_old->modifier
		    + URANGE( 0 - absmod,
			      paf->modifier * absmod / ABSOLUTE( paf_old->modifier ),
			      absmod );
	    }

	    /* jesli modyfikator jest juz maksymalny, to losowo moze sie jeszcze
	       zwiekszyc o 1 */
	    if ( how != ENH_MAX
	      && paf_old->modifier != 0
	      && paf->modifier == paf_old->modifier
	      && number_range( 0, ABSOLUTE( paf_old->modifier + paf_modifier ) ) >
					ABSOLUTE( paf_old->modifier ) )
	    {
		paf->modifier > 0 ? paf->modifier++ : paf->modifier--;
	    }

	    /* jesli mialem polepszac, nie bede usuwal poprzedniego wplywu
	       i zmienial kolejnosci we "wplywach" */
	    if ( how == ENH_MAX )
	    {
		if ( !polepszono )
		    return FALSE;
		else
		    wzmocnienie_affect_join = TRUE;
	    }

	    affect_remove( ch, paf_old, how == ENH_MAX ? FALSE : TRUE );
	    break;
	}
    }

    affect_to_char( ch, paf );
    /* Jesli to ENH_MAX, dojdzie do tego miejsca w dwoch przypadkach:
       1. Nie bylo takiego wplywu wczesniej.
       2. Byl taki wplyw, ale polepszylismy go.
       Dlatego w tym miejscu zawsze TRUE. Dla innych ENH_ nie ma i tak innej
       mozliwosci niz udane affect_join, a chocby byla, w tym miejscu na
       pewno bedzie udane (skoro poszlo affect_to_char). */
    return TRUE;
}


/*
 * Ulryk 28.12.2003: wplywy dla pomieszczen
 */
void affect_to_room( ROOM_INDEX_DATA *room, AFFECT_DATA *paf )
{
    AFFECT_DATA *new;
    char buf[ MAX_STRING_LENGTH ];

    if ( !paf )
	return;

    if ( paf->bitvector && IS_SET( room->room_flags, paf->bitvector[ 0 ] ) )
    {
	sprintf( buf, "affect_to_room: pomieszczenie juz ma flage %s",
		room_flag_name_pl( paf->bitvector[ 0 ] ) );
	bug( buf, 0 );
	return;
    }

    SET_BIT( room->room_flags, paf->bitvector[ 0 ] );

    new            = new_affect( );
    *new           = *paf;
    new->next      = room->affected;
    room->affected = new;

    return;
}


bool room_is_affected( ROOM_INDEX_DATA *room, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = room->affected; paf; paf = paf->next )
	if ( paf->type == sn )
	    return TRUE;

    return FALSE;
}


void affect_from_room( ROOM_INDEX_DATA *room, int sn )
{
    AFFECT_DATA *paf = NULL;
    AFFECT_DATA *prev = NULL;
    AFFECT_DATA *prev_next;

    if ( !room_is_affected( room, sn ) )
    {
	bug( "affect_from_room: brak wplywu.", 0 );
	return;
    }

    if ( room->affected->type == sn )
    {
	paf = room->affected;
	room->affected = paf->next;
    }
    else
    {
	for ( prev = room->affected; prev->next; prev = prev_next )
	{
	    prev_next = prev->next;

	    if ( prev->next->type == sn )
	    {
		paf = prev->next;
		prev->next = paf->next;
		break;
	    }
	}
	if ( !paf )
	    return;
    }

    REMOVE_BIT( room->room_flags, paf->bitvector[ 0 ] );

    paf->next = affect_free;
    affect_free = paf;

    return;
}


/*
 * Move a char out of a room.
 */
void char_from_room( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    CHAR_DATA *prev;
    ROOM_INDEX_DATA *tmproom;

    if ( !ch->in_room )
    {
	bug( "Char_from_room: NULL.", 0 );
	return;
    }

    if ( !IS_NPC( ch ) )
	--ch->in_room->area->nplayer;

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
	&& obj->item_type == ITEM_LIGHT
	&& obj->value[ 2 ].v != 0
	&& ch->in_room->light > 0 )
    {
	--ch->in_room->light;
    }

    /* Lam 29.1.2004 zamiast zajmujacej procesor petli w move_char( ): */
    for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
    {
	if ( is_fighting( ch, prev ) )
	{
	    sprintf( log_buf, "ch %s walczy wci`a`z z %s (w pomieszczeniu)", ch->short_descr, prev->narzednik );
	    bug( log_buf, 0 );
	    stop_fighting_char( ch, prev );
	}
	if ( is_fighting( prev, ch ) )
	{
	    sprintf( log_buf, "%s (w pomieszczeniu) walczy wci`a`z z %s (ch)", prev->short_descr, ch->narzednik );
	    bug( log_buf, 0 );
	    stop_fighting_char( prev, ch );
	}
    }

    if ( ch == ch->in_room->people )
	ch->in_room->people = ch->next_in_room;
    else
    {
	for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
	    if ( prev->next_in_room == ch )
	    {
		prev->next_in_room = ch->next_in_room;
		break;
	    }

	if ( !prev )
	{
	    sprintf( log_buf, "Char_from_room: ch %s not found in room %d.",
		ch->name, ch->in_room->vnum );
	    bug( log_buf, 0 );
	}
    }

    tmproom = ch->in_room;
    ch->in_room = NULL;
    ch->next_in_room = NULL;
    /* moglismy zabrac ostatnie swiatlo, ale mogla tez wyjsc postac z flaga
       (MROK) */
    sprawdz_przyzwyczajanie( tmproom );

    return;
}


FUNKCJA_CZASOWA( przyzwyczajenie_oczu )
{
    bool czy_ciemnia;

    /* jesli nie musi sie przyzwyczaic (np. nadwzrok albo "wyjdz" zdjelo flage) */
    if ( !IS_SET( timer->ch->pcdata->temp_flags, TEMPFLAG_BLIND ) )
	return;

    REMOVE_BIT( timer->ch->pcdata->temp_flags, TEMPFLAG_BLIND );

    if ( PP( timer->ch ) || timer->ch->in_room->vnum == ROOM_VNUM_LIMBO )
	return;

    czy_ciemnia = room_is_dark( timer->ch->in_room );

    if ( IS_AWAKE( timer->ch ) )
    {
	if ( czy_ciemnia )
	{
	    if ( IS_SET( timer->ch->pcdata->temp_flags, TEMPFLAG_DARK ) )
		/* bylo ciemno, ktos cos zapalil, ale znowu zgaslo */
		STC( "Odzyskujesz wzrok.\n\r", timer->ch );
	    else
		/* bylo jasno, zgaslo, przyzwyczail sie */
		STC( "Twe oczy przyzwyczai`ly si`e do mroku.\n\r", timer->ch );

	    SET_BIT( timer->ch->pcdata->temp_flags, TEMPFLAG_DARK );
	}
	else
	{
	    if ( IS_SET( timer->ch->pcdata->temp_flags, TEMPFLAG_DARK ) )
		/* bylo ciemno, ktos cos zapalil, ten sie przyzwyczail */
		STC( "Twe oczy przyzwyczai`ly si`e do `swiat`la.\n\r", timer->ch );
	    else
		/* bylo jasno, zgaslo, ale znowu ktos zapalil */
		STC( "Odzyskujesz wzrok.\n\r", timer->ch );

	    REMOVE_BIT( timer->ch->pcdata->temp_flags, TEMPFLAG_DARK );
	}

	do_look( timer->ch, "auto" );
    }
    else
    {
	if ( czy_ciemnia )
	    SET_BIT( timer->ch->pcdata->temp_flags, TEMPFLAG_DARK );
	else
	    REMOVE_BIT( timer->ch->pcdata->temp_flags, TEMPFLAG_DARK );
    }

    return;
}


/*
 * Przyzwyczajanie oczu do swiatla
 * Lam 26.8.98: pomysl i rozpoczecie pracy
 * Lam 24.3.99: timery
 * Lam 2.12.2004: calkowita naprawa
 */
void sprawdz_przyzwyczajanie_ch( CHAR_DATA *dch, bool czy_ciemnia )
{
    TIMER_DATA *timer, *atimer;

    if ( IS_NPC( dch ) )
	return;

    if ( PP( dch ) || dch->in_room->vnum == ROOM_VNUM_LIMBO )
    {
	/* nie oslepi go nic, ale czy_ciemnia pamieta z miejsca, z ktorego
	   wyszedl PP/zapadl w nicosc */
	REMOVE_BIT( dch->pcdata->temp_flags, TEMPFLAG_BLIND );
	return;
    }

    /* interesuja nas tylko widomi gracze z oczami */
    if ( !IS_AFFECTED( dch, AFF_BLIND )
      && !IS_SET( race_table[ dch->race ].race_abilities, RACE_NO_EYES ) )
    {
	atimer = find_timer( dch, przyzwyczajenie_oczu );

	/* czy powinien czekac na przyzwyczajenie? */
	if ( IS_AWAKE( dch )
	  && !IS_SET( dch->act, PLR_HOLYLIGHT )
	  && ( ( IS_SET( dch->pcdata->temp_flags, TEMPFLAG_DARK )
	      && !czy_ciemnia )
	    || ( !IS_SET( dch->pcdata->temp_flags, TEMPFLAG_DARK )
	      && czy_ciemnia ) ) )
	{
	    /* powinien czekac na przyzwyczajenie, a czy czeka? */
	    if ( atimer && IS_SET( dch->pcdata->temp_flags, TEMPFLAG_BLIND )
	      && atimer->time > 0 )
		/* tak, czeka na przyzwyczajenie, nastepny */
		return;

	    /* nie, nie czeka na przyzwyczajenie */
	    if ( atimer )
	    {
		/* mial timera, ktory mial mu powiedziec, ze nie musi sie
		   przyzwyczajac (atimer->time == 0), a jednak musi */
		atimer->time = 5;
		SET_BIT( dch->pcdata->temp_flags, TEMPFLAG_BLIND );
		return;
	    }

	    if ( IS_AWAKE( dch ) )
	    {
		if ( czy_ciemnia )
		    STC( "Tracisz wzrok z powodu braku `swiat`la.\n\r", dch );
		else
		    STC( "O`slepia ci`e jasne `swiat`lo.\n\r", dch );
	    }
	    SET_BIT( dch->pcdata->temp_flags, TEMPFLAG_BLIND );
	    timer = new_timer( );
	    timer->ch = dch;
	    timer->time = 5;
/*	    timer->arg[ 0 ].p = dch->in_room; */
	    timer->fun = przyzwyczajenie_oczu;
	    run_timer( timer );
	    return;
	}

	/* nie powinien czekac na przyzwyczajenie, a czy czeka? */
	if ( atimer && IS_SET( dch->pcdata->temp_flags, TEMPFLAG_BLIND ) )
	{
	    atimer->time = 0;
	    return;
	}

	/* nie powinien czekac i nie czeka, zapamietajmy stan. Uwaga: i tak
	   musimy to robic, nie mozemy zalozyc, ze zaciemnienie pomieszczenia
	   nie uleglo zmianie (podpowiedz: nadwzrok) */
	if ( czy_ciemnia )
	    SET_BIT( dch->pcdata->temp_flags, TEMPFLAG_DARK );
	else
	    REMOVE_BIT( dch->pcdata->temp_flags, TEMPFLAG_DARK );
    }

    return;
}


void sprawdz_przyzwyczajanie( ROOM_INDEX_DATA *room )
{
    CHAR_DATA *dch;
    bool czy_ciemnia = room_is_dark( room );

    for ( dch = room->people; dch; dch = dch->next_in_room )
	if ( !dch->deleted )
	    sprawdz_przyzwyczajanie_ch( dch, czy_ciemnia );

    return;
}


/*
 * Move a char into a room.
 */
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA *obj;
    CHAR_DATA *dch;

    if ( !ch )
    {
	bug( "Char_to_room: NULL ch", 0 );
	return;
    }

    if ( !pRoomIndex )
    {
	bug( "Char_to_room: NULL pRoomIndex.", 0 );
	return;
    }

    /* Lam: na wszelki wypadek */
    /* Lam 14.5.2000: wiecej informacji, bo to sie zdarza */
    if ( ch->in_room )
    {
	char buf[ MSL ];
	sprintf( buf, "Char_to_room: postac '%s' jest w pom. %d, a ktos chce ja wsadzic do %d",
		ch->name, ch->in_room->vnum, pRoomIndex->vnum );
	bug( buf, 0 );
	return;
    }

    /* Lam 8.11.2000 */
    for ( dch = pRoomIndex->people; dch; dch = dch->next_in_room )
	if ( dch == ch )
	{
	    char buf[ MSL ];
	    sprintf( buf, "Char_to_room: postac '%s' jest juz w pom. %d, ale nie ma ch->in_room!",
		ch->name, pRoomIndex->vnum );
	    bug( buf, 0 );
	    return;
	}

    ch->in_room        = pRoomIndex;
    ch->next_in_room   = pRoomIndex->people;
    pRoomIndex->people = ch;

    if ( !IS_NPC( ch ) )
	++ch->in_room->area->nplayer;

    /* Muzgus: gaszenie swiatla palacego sie niemagicznym swiatlem przy wejsciu
       do pomieszczenia podwodnego */
    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
      && obj->item_type == ITEM_LIGHT
      && obj->value[ 1 ].v == 1
      && ch->in_room->sector_type == SECT_UNDERWATER )
    {
	if ( obj->rodzaj == 3 )
	{
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p gasn`a.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p gasn`a.", ch, obj, NULL, TO_ROOM );
	}
	else
	{
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p ga`snie.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p ga`snie.", ch, obj, NULL, TO_ROOM );
	}
	unequip_char( ch, obj );
    }

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
      && obj->item_type == ITEM_LIGHT
      && obj->value[ 2 ].v != 0 )
    {
	++ch->in_room->light;
    }

    /* moglismy oswietlic pomieszczenie, ale moglismy je tez zaciemnic mrokiem
       ze "strefy cienia", wiec nie wnikam, czy zmienialismy ch->in_room->light */
    sprawdz_przyzwyczajanie( ch->in_room );

    check_airchar( ch );

    return;
}


/*
 * Give an obj to a char.
 */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    if ( obj->deleted )
	bug( "obj_to_char: object deleted", 0 );

    if ( obj->carried_by )
	bug( "obj_to_char: object carried_by", 0 );

    if ( obj->owned_by )
	bug( "obj_to_char: object owned_by", 0 );

    if ( obj->in_obj )
	bug( "obj_to_char: object in_obj", 0 );

    if ( obj->in_room )
	bug( "obj_to_char: object in_room", 0 );

    obj->next_content = ch->carrying;
    ch->carrying      = obj;
    obj->carried_by   = ch;
    obj->owned_by     = NULL;
    obj->in_room      = NULL;
    obj->in_obj       = NULL;
    ch->carry_number += get_obj_number( obj );
    ch->carry_weight += get_obj_weight( obj );

    return;
}


/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( obj->deleted )
	bug( "obj_to_char: object deleted", 0 );

    if ( obj->owned_by )
	bug( "obj_from_char: object owned_by", 0 );

    if ( obj->in_obj )
	bug( "obj_from_char: object in_obj", 0 );

    if ( obj->in_room )
	bug( "obj_from_char: object in_room", 0 );

    if ( !( ch = obj->carried_by ) )
    {
	bug( "Obj_from_char: null ch.", 0 );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
	unequip_char( ch, obj );

    if ( IS_OBJ_STAT( obj, ITEM_PLAYER_SHOP ) )
    {
	REMOVE_BIT_V( obj->extra_flags, ITEM_PLAYER_SHOP );
	obj->plr_shop_cost = 0;
    }

    /* zabezpieczcenie przed "zmus piekarz upusc wsz", bo potem mozna to wziac
       i dac np. chleb kowalowi */
    if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	REMOVE_BIT_V( obj->extra_flags, ITEM_INVENTORY );

    if ( ch->carrying == obj )
	ch->carrying = obj->next_content;
    else
    {
	OBJ_DATA *prev;

	for ( prev = ch->carrying; prev; prev = prev->next_content )
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }

	if ( !prev )
	    bug( "Obj_from_char: obj not in list.", 0 );
    }

    obj->carried_by   = NULL;
    obj->owned_by     = NULL;
    obj->next_content = NULL;
    ch->carry_number -= get_obj_number( obj );
    ch->carry_weight -= get_obj_weight( obj );
    /* FIXME: stanie sie zbedne przy plywaniu lodkami */
    if ( obj->item_type == ITEM_BOAT )
	check_airchar( ch );

    return;
}


/*
 * Lam: przechowalnia
 */
void obj_to_przechowalnia( OBJ_DATA *obj, CHAR_DATA *ch )
{
    if ( obj->deleted )
	bug( "obj_to_char: object deleted", 0 );

    if ( obj->carried_by )
	bug( "obj_to_przechowalnia: object carried_by", 0 );

    if ( obj->owned_by )
	bug( "obj_to_przechowalnia: object owned_by", 0 );

    if ( obj->in_obj )
	bug( "obj_to_przechowalnia: object in_obj", 0 );

    if ( obj->in_room )
	bug( "obj_to_przechowalnia: object in_room", 0 );

    obj->next_content = ch->przechowalnia;
    ch->przechowalnia = obj;
    obj->owned_by     = ch;
    obj->carried_by   = NULL;
    obj->in_room      = NULL;
    obj->in_obj       = NULL;

    return;
}


void obj_from_przechowalnia( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( obj->deleted )
	bug( "obj_to_char: object deleted", 0 );

    if ( obj->carried_by )
	bug( "obj_from_przechowalnia: object carried_by", 0 );

    if ( obj->in_obj )
	bug( "obj_from_przechowalnia: object in_obj", 0 );

    if ( obj->in_room )
	bug( "obj_from_przechowalnia: object in_room", 0 );

    if ( !( ch = obj->owned_by ) )
    {
	bug( "Obj_from_przechowalnia: null ch.", 0 );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	bug( "Obj_from_przechowalnia: wear_loc NOT none.", 0 );
	return;
    }

    if ( ch->przechowalnia == obj )
	ch->przechowalnia = obj->next_content;
    else
    {
	OBJ_DATA *prev;

	for ( prev = ch->przechowalnia; prev; prev = prev->next_content )
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }

	if ( !prev )
	    bug( "Obj_from_przechowalnia: obj not in list.", 0 );
    }

    obj->carried_by   = NULL;
    obj->owned_by     = NULL;
    obj->next_content = NULL;

    return;
}


/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac( OBJ_DATA *obj, int iWear )
{
    if ( obj->item_type != ITEM_ARMOR )
	return 0;

    switch ( iWear )
    {
	case WEAR_BODY:		return 3 * obj->value[ 0 ].v;
	case WEAR_HEAD:		return 2 * obj->value[ 0 ].v;
	case WEAR_LEGS:		return 2 * obj->value[ 0 ].v;
	case WEAR_FEET:		return     obj->value[ 0 ].v;
	case WEAR_HANDS:	return     obj->value[ 0 ].v;
	case WEAR_ARMS:		return     obj->value[ 0 ].v;
	case WEAR_SHIELD:	return 2 * obj->value[ 0 ].v;
	case WEAR_FINGER_L:	return     obj->value[ 0 ].v / 4;
	case WEAR_FINGER_R:	return     obj->value[ 0 ].v / 4;
	case WEAR_NECK_1:	return     obj->value[ 0 ].v;
	case WEAR_NECK_2:	return     obj->value[ 0 ].v;
	case WEAR_ABOUT:	return 2 * obj->value[ 0 ].v;
	case WEAR_WAIST:	return     obj->value[ 0 ].v;
	case WEAR_WRIST_L:	return     obj->value[ 0 ].v;
	case WEAR_WRIST_R:	return     obj->value[ 0 ].v;
	case WEAR_FACE:		return     obj->value[ 0 ].v;
    }

    return 0;
}


/*
 * Lam 20.4.2004: sprawdzenie, czy przedmiot mozna zakladac w tym miejscu. Kod
 * zapozyczony z wear_obj, tu sprawdzane dla ubierania przy wczytywaniu postaci
 * i ewentualnie dla resetow 'E'. Nie ma problemow z przedmiotami dajacymi sie
 * zakladac w wielu miejscach.
 */
bool can_equip_at( OBJ_DATA *obj, int at )
{
    if ( obj->item_type == ITEM_LIGHT
      && at == WEAR_LIGHT )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER )
      && ( at == WEAR_FINGER_L
	|| at == WEAR_FINGER_R ) )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK )
      && ( at == WEAR_NECK_1
	|| at == WEAR_NECK_2 ) )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY )
      && at == WEAR_BODY )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD )
      && at == WEAR_HEAD )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS )
      && at == WEAR_LEGS )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET )
      && at == WEAR_FEET )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS )
      && at == WEAR_HANDS )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS )
      && at == WEAR_ARMS )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT )
      && at == WEAR_ABOUT )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST )
      && at == WEAR_WAIST )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST )
      && ( at == WEAR_WRIST_L
	|| at == WEAR_WRIST_R ) )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD )
      && at == WEAR_SHIELD )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WIELD )
      && ( at == WEAR_WIELD
	|| at == WEAR_WIELD_2 ) )
	return TRUE;

    if ( /* CAN_WEAR( obj, ITEM_HOLD )
      && */ at == WEAR_HOLD )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_FACE )
      && at == WEAR_FACE )
	return TRUE;

    return FALSE;
}


/*
 * wersja can_equip_at dla indeksow, dla sprawdzania resetow 'E'. To dokladna
 * kopia powyzszej funkcji ze zmieniona tylko deklaracja
 */
bool can_equip_index_at( OBJ_INDEX_DATA *obj, int at )
{
    if ( obj->item_type == ITEM_LIGHT
      && at == WEAR_LIGHT )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER )
      && ( at == WEAR_FINGER_L
	|| at == WEAR_FINGER_R ) )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK )
      && ( at == WEAR_NECK_1
	|| at == WEAR_NECK_2 ) )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY )
      && at == WEAR_BODY )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD )
      && at == WEAR_HEAD )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS )
      && at == WEAR_LEGS )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET )
      && at == WEAR_FEET )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS )
      && at == WEAR_HANDS )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS )
      && at == WEAR_ARMS )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT )
      && at == WEAR_ABOUT )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST )
      && at == WEAR_WAIST )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST )
      && ( at == WEAR_WRIST_L
	|| at == WEAR_WRIST_R ) )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD )
      && at == WEAR_SHIELD )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WIELD )
      && ( at == WEAR_WIELD
	|| at == WEAR_WIELD_2 ) )
	return TRUE;

    if ( /* CAN_WEAR( obj, ITEM_HOLD )
      && */ at == WEAR_HOLD )
	return TRUE;

    if ( CAN_WEAR( obj, ITEM_WEAR_FACE )
      && at == WEAR_FACE )
	return TRUE;

    return FALSE;
}


#define DLA_POSTACI( t ) \
    act( ZM_WZROK | ZM_WID_OBJ1, t, ch, obj, NULL, TO_CHAR );

#define DLA_POMIESZCZENIA( t ) \
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, t, ch, obj, NULL, TO_ROOM );

void komunikat_parzenia( CHAR_DATA *ch, OBJ_DATA *obj )
{
    act( ZM_PRZYT | ZM_W_WID_OBJ1, "Co`s ci`e parzy, ale udaje ci si`e tego pozby`c.", ch, obj, NULL, TO_CHAR );
    if ( obj->wear_loc == WEAR_HOLD
      || obj->wear_loc == WEAR_WIELD
      || obj->wear_loc == WEAR_WIELD_2 )
    {
	if ( obj->rodzaj == 1 )
	{
	    DLA_POSTACI( "$p parzy ci`e, wi`ec wypuszczasz go z r`ak." );
	    DLA_POMIESZCZENIA( "$p parzy $3, a $z wypuszcza go z r`ak." );
	}
	else if ( obj->rodzaj == 2 )
	{
	    DLA_POSTACI( "$p parzy ci`e, wi`ec wypuszczasz j`a z r`ak." );
	    DLA_POMIESZCZENIA( "$p parzy $3, a $z wypuszcza j`a z r`ak." );
	}
	else if ( obj->rodzaj == 3 )
	{
	    DLA_POSTACI( "$p parz`a ci`e, wi`ec wypuszczasz je z r`ak." );
	    DLA_POMIESZCZENIA( "$p parz`a $3, a $z wypuszcza je z r`ak." );
	}
	else
	{
	    DLA_POSTACI( "$p parzy ci`e, wi`ec wypuszczasz je z r`ak." );
	    DLA_POMIESZCZENIA( "$p parzy $3, a $z wypuszcza je z r`ak." );
	}
    }
    else
    {
	if ( obj->rodzaj == 1 )
	{
	    DLA_POSTACI( "$p parzy ci`e, wi`ec zrywasz go z siebie." );
	    DLA_POMIESZCZENIA( "$p parzy $3, a $z zrywa go z siebie." );
	}
	else if ( obj->rodzaj == 2 )
	{
	    DLA_POSTACI( "$p parzy ci`e, wi`ec zrywasz j`a z siebie." );
	    DLA_POMIESZCZENIA( "$p parzy $3, a $z zrywa j`a z siebie." );
	}
	else if ( obj->rodzaj == 3 )
	{
	    DLA_POSTACI( "$p parz`a ci`e, wi`ec zrywasz je z siebie." );
	    DLA_POMIESZCZENIA( "$p parz`a $3, a $z zrywa je z siebie." );
	}
	else
	{
	    DLA_POSTACI( "$p parzy ci`e, wi`ec zrywasz je z siebie." );
	    DLA_POMIESZCZENIA( "$p parzy $3, a $z zrywa je z siebie." );
	}
    }

    return;
}
#undef DLA_POSTACI
#undef DLA_POMIESZCZENIA


/*
 * Sprawdzanie, czy jakies przedmioty nie powinny spasc z gracza. Dalo sie na
 * przyklad rzucic przemiane, zalozyc przedmioty antyrasowe i po uplywie
 * dzialania czaru przedmioty zostawaly na postaci.
 *
 * Ulryk 14.1.2004: Wazne: jesli zmieniamy cos tutaj, to trzeba tez zmienic
 * w equip_char( ) i vice versa.
 */
void sprawdz_ubranie( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->deleted || ( obj->wear_loc == WEAR_NONE ) )
	    continue;

	if ( ( !IS_OBJ_STAT( obj, ITEM_INVERT_RACES )
	    && is_name2( race_table[ ch->race ].ang, obj->anti_race ) )
	  || ( IS_OBJ_STAT( obj, ITEM_INVERT_RACES )
	    && !is_name2( race_table[ ch->race ].ang, obj->anti_race ) )
	  || ( !IS_NPC( ch ) && !IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	    && is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) )
	  || ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	    && !is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) )
	  || ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_PRIVATE )
	    && str_cmp( ch->name, obj->owner ) ) )
	{
	    if ( obj->rodzaj == 3 )
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, "$p nie pasuj`a na ciebie.", ch, obj, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$p nie pasuj`a na $3.",  ch, obj, NULL, TO_ROOM );
	    }
	    else
	    {
		act( ZM_WZROK | ZM_WID_OBJ1, "$p nie pasuje na ciebie.", ch, obj, NULL, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$p nie pasuje na $3.",  ch, obj, NULL, TO_ROOM );
	    }
	    unequip_char( ch, obj );
	}

	if ( ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL   ) && IS_EVIL   ( ch ) )
	  || ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD   ) && IS_GOOD   ( ch ) )
	  || ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL( ch ) )
	  || ( IS_OBJ_STAT( obj, ITEM_ANTI_MALE   ) && ( ch->sex == 1 ) )
	  || ( IS_OBJ_STAT( obj, ITEM_ANTI_FEMALE ) && ( ch->sex == 2 ) )
	  || ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTER ) && ( ch->sex == 0 ) )
	  || ( IS_OBJ_STAT( obj, ITEM_HOLY        ) && IS_EVIL   ( ch ) )
	  || ( IS_OBJ_STAT( obj, ITEM_VAMPIRE_BANE) && CZY_WAMPIR( ch ) ) )
	{
	    komunikat_parzenia( ch, obj );
	    unequip_char( ch, obj );
	}
    }

    check_race_wearlocs( ch );

    return;
}


/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj;

    if ( iWear >= MAX_WEAR )
	bug( "iWear %d >= MAX_WEAR", iWear );

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( !obj->deleted
	  && obj->wear_loc == iWear )
	{
	    return obj;
	}

    return NULL;
}


/* Ulryk: przerobione na zwracanie sumy bitow */
int czy_pasuje( OBJ_DATA *obj, CHAR_DATA *ch )
{
    int	wynik = NOTHING_WRONG;
    /* Lam, Lac 1.4.-3 alfa, antyrasowe i antyprofesjowe przedmioty
	    Lac 1.4.-3 beta, odwracanie znaczenia tych lancuchow
	    Lac 1.4.-3-pre16, moby nie maja profesji!
	    Lac 1.4.-2-pre3, poprawka poprzedniej poprawki
	    Lac 1.4.0-pre46, profesje BWWU */
    if ( ( !IS_OBJ_STAT( obj, ITEM_INVERT_RACES )
	&& is_name2( race_table[ ch->race ].ang, obj->anti_race ) )
      || ( IS_OBJ_STAT( obj, ITEM_INVERT_RACES )
	&& !is_name2( race_table[ ch->race ].ang, obj->anti_race ) ) )
    {
	SET_BIT( wynik, WRONG_RACE );
    }

    if ( !IS_NPC( ch ) && !IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
      && is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) )
    {
	SET_BIT( wynik, WRONG_CLASS );
    }

    if ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
      && !is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) )
    {
	int i, poziomy[ MAX_CLASS ];

	if ( ch->level >= L_APP )
	{
	    wypelnij_poziomy( ch, poziomy );

	    for ( i = 0; i < MAX_CLASS; i++ )
		if ( poziomy[ i ] >= 100
		  && is_name2( class_table[ i ].who_name, obj->anti_class ) )
		{
		    break;
		}

	    /* nie ukonczyl gry zadna profesja, do ktorej pasuje */
	    if ( i == MAX_CLASS )
		SET_BIT( wynik, WRONG_CLASS );
	}
	else
	    SET_BIT( wynik, WRONG_CLASS );
    }

    if ( !IS_NPC( ch )
      && IS_OBJ_STAT( obj, ITEM_PRIVATE )
      && str_cmp( ch->name, obj->owner ) )
    {
	SET_BIT( wynik, WRONG_PERSON );
    }

    return wynik;
}


/*
 * Equip a char with an obj.
 */
bool equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    AFFECT_DATA *paf;
    char         buf[ MAX_STRING_LENGTH ];

    if ( get_eq_char( ch, iWear ) )
    {
	sprintf( buf, "Equip_char: %s already equipped at %d.",
		ch->name, iWear );
	bug( buf, 0 );
	return FALSE;
    }

    if ( czy_pasuje( obj, ch ) )
    {
	act( ZM_PRZYT | ZM_W_WID_OBJ1, "Robi ci si`e l`zej.", ch, obj, NULL, TO_CHAR );

	if ( obj->rodzaj == 3 )
	{
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p nie pasuj`a i zsuwaj`a si`e z ciebie.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$p zsuwaj`a si`e z $1.",  ch, obj, NULL, TO_ROOM );
	}
	else
	{
	    act( ZM_WZROK | ZM_WID_OBJ1, "$p nie pasuje i zsuwa si`e z ciebie.", ch, obj, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$p zsuwa si`e z $1.", ch, obj, NULL, TO_ROOM );
	}
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	if ( obj->rodzaj == 3 )
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawiaj`a si`e tu $p.", ch, obj, NULL, TO_ROOM );
	else
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, obj, NULL, TO_ROOM );
	return FALSE;
    }

    /* Lam: antyplciowe przedmioty */
    if ( ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL   ) && IS_EVIL   ( ch ) )
      || ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD   ) && IS_GOOD   ( ch ) )
      || ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL( ch ) )
      || ( IS_OBJ_STAT( obj, ITEM_ANTI_MALE   ) && ( ch->sex == 1 ) )
      || ( IS_OBJ_STAT( obj, ITEM_ANTI_FEMALE ) && ( ch->sex == 2 ) )
      || ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTER ) && ( ch->sex == 0 ) )
      || ( IS_OBJ_STAT( obj, ITEM_HOLY        ) && IS_EVIL   ( ch ) )
      || ( IS_OBJ_STAT( obj, ITEM_VAMPIRE_BANE)
	&& CZY_WAMPIR( ch ) ) )
    {
	komunikat_parzenia( ch, obj );
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	if ( obj->rodzaj == 3 )
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawiaj`a si`e tu $p.", ch, obj, NULL, TO_ROOM );
	else
	    act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_OBJ1, "Nagle pojawia si`e tu $p.", ch, obj, NULL, TO_ROOM );
	return FALSE;
    }

    ch->armor           += apply_ac( obj, iWear );
    obj->wear_loc        = iWear;

    if ( IS_OBJ_STAT( obj, ITEM_HUM ) )
	zmien_brzeczenie( ch, TRUE, FALSE );

    /* Lam 27.10.2004: tylko "trzymane" daja wplywy kiedy sie je trzyma */
    if ( iWear != WEAR_HOLD || CAN_WEAR( obj, ITEM_HOLD ) )
    {
	for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
	    affect_modify( ch, paf, TRUE, TRUE );
	for ( paf = obj->affected; paf; paf = paf->next )
	    affect_modify( ch, paf, TRUE, TRUE );

	if ( obj->item_type == ITEM_LIGHT
	  && iWear == WEAR_LIGHT
	  && obj->value[ 2 ].v != 0
	  && ch->in_room )
	{
	    ++ch->in_room->light;
	    if ( ch->in_room->light == 1 ) /* zmienilismy z 0 */
		sprawdz_przyzwyczajanie( ch->in_room );
	}
    }

    if ( obj->action && *obj->action )
    {
	sprintf( buf, "%s\n\r", przemiel_opis( ch, obj->action ) );
	send_to_char( buf, ch );
    };

    return TRUE;
}


/* Lam 27.2.98: wersja equip_char( ) do zapisywania postaci (rozbierz/ubierz).
   Brak opisu zakladania, nie sprawdza moralnosci */
void save_equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    AFFECT_DATA *paf;
    char         buf[ MAX_STRING_LENGTH ];

    if ( get_eq_char( ch, iWear ) )
    {
	sprintf( buf, "Equip_char: %s already equipped at %d.",
		ch->name, iWear );
	bug( buf, 0 );
	return;
    }

    ch->armor    += apply_ac( obj, iWear );
    obj->wear_loc = iWear;

    if ( IS_OBJ_STAT( obj, ITEM_HUM ) )
	zmien_brzeczenie( ch, TRUE, TRUE );

    if ( iWear != WEAR_HOLD || CAN_WEAR( obj, ITEM_HOLD ) )
    {
	for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
	    affect_modify( ch, paf, TRUE, FALSE );
	for ( paf = obj->affected; paf; paf = paf->next )
	    affect_modify( ch, paf, TRUE, FALSE );

	if ( obj->item_type == ITEM_LIGHT
	  && iWear == WEAR_LIGHT
	  && obj->value[ 2 ].v != 0
	  && ch->in_room )
	{
	    ++ch->in_room->light;
	}
    }

    return;
}


/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    AFFECT_DATA *paf;
    char buf[ MAX_STRING_LENGTH ];
    int wear_loc;

    if ( obj->wear_loc == WEAR_NONE )
    {
	sprintf( buf, "Unequip_char: %s already unequipped with %d.",
		ch->name, obj->pIndexData->vnum );
	bug( buf, 0 );
	return;
    }

    ch->armor -= apply_ac( obj, obj->wear_loc );

    wear_loc = obj->wear_loc;
    obj->wear_loc = -1;

    if ( IS_OBJ_STAT( obj, ITEM_HUM ) )
	zmien_brzeczenie( ch, FALSE, FALSE );

    if ( wear_loc == WEAR_HOLD )
    {
	TIMER_DATA *timer;

	for ( timer = timer_list; timer; timer = timer->next )
	    if ( ( timer->fun == przestan_karmic || timer->fun == przestan_poic )
	      && ( OBJ_DATA * ) timer->arg[ 1 ].p == obj )
	    {
		del_timer( timer );
		break;
	    }
    }

    if ( wear_loc != WEAR_HOLD || CAN_WEAR( obj, ITEM_HOLD ) )
    {
	for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
	    affect_modify( ch, paf, FALSE, TRUE );
	for ( paf = obj->affected; paf; paf = paf->next )
	    affect_modify( ch, paf, FALSE, TRUE );

	if ( obj->item_type == ITEM_LIGHT
	  && wear_loc == WEAR_LIGHT
	  && obj->value[ 2 ].v != 0
	  && ch->in_room )
	{
	    --ch->in_room->light;
	    if ( ch->in_room->light == 0 ) /* zmienilismy z 1 */
		sprawdz_przyzwyczajanie( ch->in_room );
	}
    }

    return;
}


/* Lam 11.10.2003: wersja z affect_modify nie sprawdzajacym broni, zeby nie
   tracic broni podczas zapisywania postaci bez przedmiotow na sobie */
void unequip_char_save( CHAR_DATA *ch, OBJ_DATA *obj )
{
    AFFECT_DATA *paf;
    char buf[ MAX_STRING_LENGTH ];
    int wear_loc;

    if ( obj->wear_loc == WEAR_NONE )
    {
	sprintf( buf, "Unequip_char: %s already unequipped with %d.",
		ch->name, obj->pIndexData->vnum );
	bug( buf, 0 );
	return;
    }

    ch->armor -= apply_ac( obj, obj->wear_loc );

    wear_loc = obj->wear_loc;
    obj->wear_loc = -1;

    if ( IS_OBJ_STAT( obj, ITEM_HUM ) )
	zmien_brzeczenie( ch, FALSE, TRUE );

    if ( wear_loc != WEAR_HOLD || CAN_WEAR( obj, ITEM_HOLD ) )
    {
	for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
	    affect_modify( ch, paf, FALSE, FALSE );
	for ( paf = obj->affected; paf; paf = paf->next )
	    affect_modify( ch, paf, FALSE, FALSE );

	if ( obj->item_type == ITEM_LIGHT
	  && wear_loc == WEAR_LIGHT
	  && obj->value[ 2 ].v != 0
	  && ch->in_room )
	{
	    --ch->in_room->light;
	}
    }

    return;
}


/*
 * Lam 7.6.99: funkcja sprawdza, czy po zmianie rasy przedmioty, ktore ma na
 * sobie postac, nadal moga byc przez nia uzywane.
 * Lam 20.11.2004: zalatana prawdopodobna mozliwosc zepsucia list
 */
void check_race_wearlocs( CHAR_DATA *ch )
{
    OBJ_DATA *obj, *obj_next;

    for ( obj = ch->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted
	  || ( obj->wear_loc == WEAR_NONE ) )
	{
	    continue;
	}

	switch ( obj->wear_loc )
	{
	    default: bug( "check_race_wearlocs: dziwny obj->wear_loc: %d", obj->wear_loc );
			break;
	    case WEAR_LIGHT:
	    case WEAR_HOLD:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_TRZYMANE ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_FINGER_L:
	    case WEAR_FINGER_R:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_PALEC ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_NECK_1:
	    case WEAR_NECK_2:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_SZYJA ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_BODY:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_CIALO ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_HEAD:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_GLOWA ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_LEGS:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_NOGI ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_FEET:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_STOPY ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_HANDS:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_DLONIE ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_ARMS:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_RAMIONA ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_SHIELD:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_TARCZA ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_ABOUT:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_WOKOL ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_WAIST:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_PAS ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_WRIST_L:
	    case WEAR_WRIST_R:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_NADGARSTEK ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_WIELD:
	    case WEAR_WIELD_2:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_BRON )
			  || !IS_SET( race_table[ ch->race ].race_abilities, RACE_WEAPON_WIELD ) )
			    unequip_char( ch, obj );
			break;
	    case WEAR_FACE:
			if ( IS_SET( race_table[ ch->race ].wearlocs, WRL_TWARZ ) )
			    unequip_char( ch, obj );
			break;
	}

	/* albo obj wypadl z inwentarza (niemozliwe), albo obj_next (mozliwe) */
	if ( obj_next != obj->next_content )
	{
	    bug( "(to nie blad) (informacyjnie) restartuje szukanie (jeden)", 0 );
	    obj_next = ch->carrying;
	}
	/* oba wypadly w odwrotnej kolejnosci (rowniez niemozliwe) */
	else if ( obj_next && obj_next->carried_by != ch )
	{
	    bug( "(to nie blad) (informacyjnie) restartuje szukanie (oba)", 0 );
	    obj_next = ch->carrying;
	}
	/* mozliwe sa maksymalnie dwa takie restarty */
    }

    return;
}


/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    int       nMatch;

    nMatch = 0;
    for ( obj = list; obj; obj = obj->next_content )
	if ( !obj->deleted
	  && obj->pIndexData == pObjIndex )
	{
	    nMatch++;
	}

    return nMatch;
}


/*
 * Move an obj out of a room.
 */
void obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *in_room;

    if ( obj->deleted )
	bug( "obj_from_room: object deleted", 0 );

    if ( obj->carried_by )
	bug( "obj_from_room: object carried_by", 0 );

    if ( obj->owned_by )
	bug( "obj_from_room: object owned_by", 0 );

    if ( obj->in_obj )
	bug( "obj_from_room: object in_obj", 0 );

    if ( !( in_room = obj->in_room ) )
    {
	bug( "obj_from_room: NULL obj->in_room.", 0 );
	return;
    }

    if ( obj == in_room->contents )
	in_room->contents = obj->next_content;
    else
    {
	OBJ_DATA *prev;

	for ( prev = in_room->contents; prev; prev = prev->next_content )
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }

	if ( !prev )
	{
	    bug( "Obj_from_room: obj not found.", 0 );
	    return;
	}
    }

    obj->in_room      = NULL;
    obj->next_content = NULL;
    if ( obj->item_type == ITEM_LIGHT && obj->value[ 2 ].v != 0 )
    {
	--in_room->light;
	if ( !in_room->light ) /* zmienilismy z 1 na 0 */
	    sprawdz_przyzwyczajanie( in_room );
    }

    return;
}


/*
 * Move an obj into a room.
 */
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
    if ( !obj )
    {
	bug( "obj_to_room: NULL obj", 0 );
	return;
    }

    if ( obj->deleted )
	bug( "obj_to_room: object deleted", 0 );

    if ( obj->carried_by )
	bug( "obj_to_room: object carried_by", 0 );

    if ( obj->owned_by )
	bug( "obj_to_room: object owned_by", 0 );

    if ( obj->in_obj )
	bug( "obj_to_room: object in_obj", 0 );

    if ( obj->in_room )
	bug( "obj_to_room: object in_room", 0 );

    /* dzieki bug( ) zrzucajacemu stos odwolan mozna namierzyc wszystko */
    if ( pRoomIndex->vnum == ROOM_VNUM_POZA_SWIATEM )
	bug( "obj_to_room: przedmiot spadajacy poza swiat!", 0 );

    obj->next_content    = pRoomIndex->contents;
    pRoomIndex->contents = obj;
    obj->in_room         = pRoomIndex;
    obj->carried_by      = NULL;
    obj->owned_by        = NULL;
    obj->in_obj          = NULL;

    if ( obj->item_type == ITEM_LIGHT && obj->value[ 2 ].v != 0 )
    {
	++pRoomIndex->light;
	if ( pRoomIndex->light == 1 ) /* zmienilismy z 0 na 1 */
	    sprawdz_przyzwyczajanie( pRoomIndex );
    }

    /* Lam 28.3.99 */
    if ( IS_SET( obj->wear_flags, ITEM_TAKE )
      && pRoomIndex->exit[ DIR_DOWN ]
      && pRoomIndex->exit[ DIR_DOWN ]->to_room
      && !IS_SET( pRoomIndex->exit[ DIR_DOWN ]->exit_info, EX_CLOSED )
      && ( CZY_POWIETRZE( pRoomIndex )
	|| pRoomIndex->sector_type == SECT_WATER_DEEP
	|| CZY_POD_WODA( pRoomIndex ) )
      && !find_airobj( obj ) )
    {
	new_airobj( obj );
    }

    return;
}


/*
 * Move an object into an object.
 */
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
    if ( obj_to->deleted )
    {
	bug( "Obj_to_obj:  Obj_to already deleted", 0 );
	return;
    }

    if ( obj->deleted )
	bug( "obj_to_obj: object deleted", 0 );

    if ( obj->carried_by )
	bug( "obj_to_obj: object carried_by", 0 );

    if ( obj->owned_by )
	bug( "obj_to_obj: object owned_by", 0 );

    if ( obj->in_obj )
	bug( "obj_to_obj: object in_obj", 0 );

    if ( obj->in_room )
	bug( "obj_to_obj: object in_room", 0 );

    obj->next_content = obj_to->contains;
    obj_to->contains  = obj;
    obj->in_obj       = obj_to;
    obj->in_room      = NULL;
    obj->owned_by     = NULL;
    obj->carried_by   = NULL;

    for ( ; obj_to; obj_to = obj_to->in_obj )
	if ( !obj_to->deleted
	  && obj_to->carried_by )
	{
/* pojemniki: obj_to->carried_by->carry_number += get_obj_number( obj ); */
	    obj_to->carried_by->carry_weight += get_obj_weight( obj );
	}

    return;
}


/*
 * Move an object out of an object.
 */
void obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;

    if ( obj->deleted )
	bug( "obj_from_obj: object deleted", 0 );

    if ( obj->carried_by )
	bug( "obj_from_obj: object carried_by", 0 );

    if ( obj->owned_by )
	bug( "obj_from_obj: object owned_by", 0 );

    if ( obj->in_room )
	bug( "obj_from_obj: object in_room", 0 );

    if ( !( obj_from = obj->in_obj ) )
    {
	bug( "Obj_from_obj: null obj_from.", 0 );
	return;
    }

    if ( obj == obj_from->contains )
	obj_from->contains = obj->next_content;
    else
    {
	OBJ_DATA *prev;

	for ( prev = obj_from->contains; prev; prev = prev->next_content )
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }

	if ( !prev )
	{
	    bug( "Obj_from_obj: obj not found.", 0 );
	    return;
	}
    }

    obj->next_content = NULL;
    obj->in_obj       = NULL;

    for ( ; obj_from; obj_from = obj_from->in_obj )
	if ( !obj_from->deleted
	  && obj_from->carried_by )
	{
/*            obj_from->carried_by->carry_number -= get_obj_number( obj ); */
	    obj_from->carried_by->carry_weight -= get_obj_weight( obj );
	}

    return;
}


/*
 * Extract an obj from the world.
 */
extern bool delete_obj;
void extract_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_content;
    OBJ_DATA *obj_next;
    CHAR_DATA *wch;
    AREA_DATA *kraina;
    TYP_ZMIENNEJ *zm, *zm_nast;
    int z_pom, z_inw, z_prz, z_poj;

    if ( obj->deleted )
    {
	bug( "Extract_obj:  Obj already deleted", 0 );
	return;
    }

    z_pom = z_inw = z_prz = z_poj = 0;

    if ( obj->in_room )
    {
	obj_from_room( obj );
	z_pom = 1;
    }
    if ( obj->carried_by )
    {
	obj_from_char( obj );
	z_inw = 1;
    }
    if ( obj->owned_by )
    {
	obj_from_przechowalnia( obj );
	z_prz = 1;
    }
    if ( obj->in_obj )
    {
	obj_from_obj( obj );
	z_poj = 1;
    }

    if ( ( z_pom + z_inw + z_prz + z_poj ) > 1 )
    {
	sprintf( log_buf, "extract_obj: Przedmiot %d w wielu miejscach: pom %d inw %d prz %d poj %d.",
		obj->pIndexData ? obj->pIndexData->vnum : 0, z_pom, z_inw,
		z_prz, z_poj );
	bug( log_buf, 0 );
    }

/* To zbyt czesto stosowane w kodzie
    if ( ( z_pom + z_inw + z_prz + z_poj ) < 1 )
	bug( "extract_obj: Przedmiot %d zwalniany znikad.",
		obj->pIndexData ? obj->pIndexData->vnum : 0 );
*/

    for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
    {
	obj_next = obj_content->next_content;
	/* to w Envy nie powinno byc mozliwe, a jednak dali taki warunek */
	if ( obj_content->deleted )
	{
	    sprintf( log_buf, "extract_obj: Przedmiot %d zawiera skasowany przedmiot %d",
		obj->pIndexData ? obj->pIndexData->vnum : 0,
		obj_content->pIndexData ? obj_content->pIndexData->vnum : 0 );
	    bug( log_buf, 0 );
	    continue;
	}
	extract_obj( obj_content );
    }

    /* tu taka sama uwaga, co w extract_char - potrzebna pomocnicza lista! */
    for ( wch = char_list; wch; wch = wch->next )
	for ( zm = wch->zmienne; zm; zm = zm_nast )
	{
	    zm_nast = zm->nast;
	    if ( zm->typ == TZ_PRZE
	      && zm->wartosc.p == obj )
		usun_z_listy( &wch->zmienne, zm, wch );
	}
    for ( kraina = area_first; kraina; kraina = kraina->next )
	for ( zm = kraina->zmienne; zm; zm = zm_nast )
	{
	    zm_nast = zm->nast;
	    if ( zm->typ == TZ_PRZE
	      && zm->wartosc.p == obj )
		usun_z_listy( &kraina->zmienne, zm, NULL );
	}

    obj->deleted = TRUE;
    delete_obj   = TRUE;

    return;
}


/*
 * Extract a char from the world.
 *
 * Uwaga, ta funkcja sluzy do wyciagniecia postaci ze swiata, czyli usunieciu
 * wszelkich wskaznikow, ktore moga na nia pokazywac. Nie sluzy do zwalniania
 * pamieci struktur postaci. Postac pozostaje w char_list, a jej dane sa
 * zwalniane pozniej przez free_char( ).
 */
extern bool delete_char;
void extract_char( CHAR_DATA *ch, bool fPull )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    MTRIG_DATA *trig;
    TYP_ZMIENNEJ *zm, *zm_nast;
    AFFECT_DATA *paf;
    TIMER_DATA *timer, *timer_next;
    AREA_DATA *kraina;
    ZWOD_DATA *zwod;

    if ( !ch->in_room )
    {
	bug( "Extract_char: NULL ch->in_room.", 0 );
	return;
    }

    /* 11.12.2001 zapetlil sie w tej funkcji, dodatkowe sprawdzenie, moze
       sie powtorzy? */
#if defined( DEBUG_LOOPS_CHAR_LIST )
    SPRAWDZ_PETLE( CHAR_DATA, char_list, next );
#endif

    if ( ch->fighting )
	stop_fighting( ch, TRUE );

    if ( fPull )
    {
	zabij_wszystkie_slugi( ch );
	die_follower( ch );

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
		continue;
	    if ( obj->wear_loc != WEAR_NONE )
		unequip_char_save( ch, obj );
	    extract_obj( obj );
	}

	for ( obj = ch->przechowalnia; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( !obj->deleted )
		extract_obj( obj );
	}
    }

    char_from_room( ch );

    if ( !fPull )
    {
	ROOM_INDEX_DATA *location;
	int apos;

	if ( !IS_NPC( ch ) && get_trust( ch ) < 5 )
	    location = get_room_index( ROOM_VNUM_GRAVEYARD_A );
	else
	    location = get_room_index( ROOM_VNUM_PURGATORY_A );

	if ( !location )
	{
	    bug( "Purgatory A does not exist!", 0 );
	    char_to_room( ch, get_room_index( ROOM_VNUM_ALTAR ) );
	}
	else
	    char_to_room( ch, location );

	send_to_char( "\n\r", ch );
	/* Lam 18.11.2003: do_look bylo, ale wypoczywanie ustawia sie pozniej,
		pozwole mu sie wiec rozejrzec, i tak bedzie to mogl zrobic
		natychmiast po smierci */
	apos = ch->position;
	ch->position = POS_STANDING;
	do_look( ch, "" ); /* zauwaz: nie "auto" */
	ch->position = apos;
	return;
    }

    if ( IS_NPC( ch ) )
	--ch->pIndexData->count;

    if ( ch->desc && ch->desc->original )
	do_return( ch, "" );

    /*
     * Lam 29.11.2005: zamiast petli po char_list, duzo malych pomocniczych
     * list zwrotnych odnosnikow we wszystkie strony. Bardzo szybkie, choc
     * kodu niemalo i pamieci tez troche zajmuje.
     */
    if ( ch->retell )
	wywal_zwod( &ch->retell->zwod_retell, ch );

    if ( ch->zwod_retell )
	for ( zwod = ch->zwod_retell; zwod; zwod = zwod->next )
	{
	    if ( zwod->ch->retell != ch )
		bug( "zwod->ch->retell != ch", 0 );
	    zwod->ch->retell = NULL;
	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_retell;
		ch->zwod_retell = NULL;
		break;
	    }
	}

    if ( ch->reply )
	wywal_zwod( &ch->reply->zwod_reply, ch );

    if ( ch->zwod_reply )
	for ( zwod = ch->zwod_reply; zwod; zwod = zwod->next )
	{
	    if ( zwod->ch->reply != ch )
		bug( "zwod->ch->reply != ch", 0 );
	    zwod->ch->reply = NULL;
	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_reply;
		ch->zwod_reply = NULL;
		break;
	    }
	}

    if ( ch->hunting )
	wywal_zwod( &ch->hunting->zwod_hunting, ch );

    if ( ch->zwod_hunting )
	for ( zwod = ch->zwod_hunting; zwod; zwod = zwod->next )
	{
	    if ( zwod->ch->hunting != ch )
		bug( "zwod->ch->hunting != ch", 0 );
	    zwod->ch->hunting = NULL;
	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_hunting;
		ch->zwod_hunting = NULL;
		break;
	    }
	}

    if ( ch->hating )
	wywal_zwod( &ch->hating->zwod_hating, ch );

    if ( ch->zwod_hating )
	for ( zwod = ch->zwod_hating; zwod; zwod = zwod->next )
	{
	    if ( zwod->ch->hating != ch )
		bug( "zwod->ch->hating != ch", 0 );
	    zwod->ch->hating = NULL;
	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_hating;
		ch->zwod_hating = NULL;
		break;
	    }
	}

    if ( ch->fearing )
	wywal_zwod( &ch->fearing->zwod_fearing, ch );

    if ( ch->zwod_fearing )
	for ( zwod = ch->zwod_fearing; zwod; zwod = zwod->next )
	{
	    if ( zwod->ch->fearing != ch )
		bug( "zwod->ch->fearing != ch", 0 );
	    zwod->ch->fearing = NULL;
	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_fearing;
		ch->zwod_fearing = NULL;
		break;
	    }
	}

    if ( ch->debugger )
	wywal_zwod( &ch->debugger->zwod_debugger, ch );

    if ( ch->zwod_debugger )
	for ( zwod = ch->zwod_debugger; zwod; zwod = zwod->next )
	{
	    if ( zwod->ch->debugger != ch )
		bug( "zwod->ch->debugger != ch", 0 );
	    zwod->ch->debugger = NULL;
	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_debugger;
		ch->zwod_debugger = NULL;
		break;
	    }
	}

    if ( ch->zwod_zmienne )
	for ( zwod = ch->zwod_zmienne; zwod; zwod = zwod->next )
	{
	    bool bylo_cos = FALSE;
	    for ( zm = zwod->ch->zmienne; zm; zm = zm_nast )
	    {
		zm_nast = zm->nast;
		if ( zm->typ == TZ_POST
		  && zm->wartosc.p == ch )
		{
		    /* tu ostatni parametr NULL, tutaj sami sobie usuwamy zwod */
		    usun_z_listy( &zwod->ch->zmienne, zm, NULL );
		    bylo_cos = TRUE;
		}
	    }
	    if ( !bylo_cos )
		bug( "Nie znalazlem zadnej zmiennej za zwod.", 0 );

	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_zmienne;
		ch->zwod_zmienne = NULL;
		break;
	    }
	}

    if ( ch->zwod_affected )
	for ( zwod = ch->zwod_affected; zwod; zwod = zwod->next )
	{
	    bool bylo_cos = FALSE;
	    for ( paf = zwod->ch->affected; paf; paf = paf->next )
		if ( paf->caster == ch ) /* usuniete (deleted) tez musimy sprawdzac */
		{
		    paf->caster = NULL;
		    bylo_cos = TRUE;
		}

	    if ( !bylo_cos )
		bug( "Nie znalazlem wplywu na postaci za zwod.", 0 );

	    if ( !zwod->next )
	    {
		zwod->next = zwod_free;
		zwod_free = ch->zwod_affected;
		ch->zwod_affected = NULL;
		break;
	    }
	}

    for ( trig = mtrig_list; trig; trig = trig->next )
    {
	if ( trig->ch == ch )
	    trig->ch = NULL;

	/* trig_update( ) go usunie */
	if ( trig->actor == ch )
	    trig->actor = NULL;

	if ( trig->vict == ch )
	    trig->vict = NULL;
    }

    for ( timer = timer_list; timer; timer = timer_next )
    {
	timer_next = timer->next;
	if ( timer->ch == ch )
	    del_timer( timer );
	else if ( ( timer->fun == przestan_karmic || timer->fun == przestan_poic )
	       && ( CHAR_DATA * ) timer->arg[ 0 ].p == ch )
	{
	    del_timer( timer );
	}
    }

    /* zmienne krain nie uzywaja zwrotnych odnosnikow - maly zysk */
    for ( kraina = area_first; kraina; kraina = kraina->next )
	for ( zm = kraina->zmienne; zm; zm = zm_nast )
	{
	    zm_nast = zm->nast;
	    if ( zm->typ == TZ_POST
	      && zm->wartosc.p == ch )
	    {
		usun_z_listy( &kraina->zmienne, zm, NULL );
	    }
	}

    ch->deleted = TRUE;

    if ( ch->desc )
	ch->desc->character = NULL;

    delete_char = TRUE;

    return;
}


CHAR_DATA *real_get_char_room( CHAR_DATA *ch, char *argument, int *count )
{
    CHAR_DATA *rch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;

    number = number_argument( argument, arg );
    if ( !str_cmp( arg, "si`e" ) || !str_cmp( arg, "siebie" ) )
	return ch;
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( !can_see( ch, rch ) || !is_name( arg, rch->name ) )
	    continue;
	if ( ++*count == number )
	    return rch;
    }

    return NULL;
}


CHAR_DATA *real_get_char_room2( CHAR_DATA *ch, char *argument, int *count )
{
    CHAR_DATA *rch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;

    number = number_argument( argument, arg );
    if ( !str_cmp( arg, "si`e" ) || !str_cmp( arg, "siebie" ) )
	return ch;
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( !can_see( ch, rch ) || !is_name2( arg, rch->name ) )
	    continue;
	if ( ++*count == number )
	    return rch;
    }

    return NULL;
}


/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room( CHAR_DATA *ch, char *argument )
{
    int        count = 0;

    return real_get_char_room( ch, argument, &count );
}


/*
 * Lam 21.9.2002, pierwotny pomysl Erwina Andreasena
 * Erwin uzywal tego do swojego "for" - my do progow.
 *
 * Sens jest taki, zeby znalezc taka nazwe, ktora identyfikowalaby te postac
 * jesli podamy ja funkcji get_char_room. Przyklad: stoja w pomieszczeniu lama
 * i Lam. "p lam" pokaze lame, Lama pokaze "p 2.lam".
 *
 * Wzgledem oryginalu powazne zmiany w zalozeniach. Po pierwsze, Lac obsluguje
 * skroty nazw, czyli podlegaja temu tak moby, jak i postacie graczy
 * (w oryginale gracze mieli unikalne imiona). Po drugie, Erwin w milczeniu
 * zakladal, ze polecenie "for" bedzie uzywane przez boga z nadwzrokiem, a ta
 * funkcja uzywana tylko przez to polecenie. Stad funkcja nie brala parametru
 * "viewer" i nie sprawdzala, czy cel jest widoczny. W przypadku, kiedy
 * w pomieszczeniu znajduje sie niewidzialna lama i Lam, Lam moze byc "2.lam"
 * albo "lam", w zaleznosci od patrzacego.
 */
const char *name_expand( CHAR_DATA *viewer, CHAR_DATA *ch )
{
    int count = 1;
    CHAR_DATA *rch;
    char name[ MAX_INPUT_LENGTH ];

    static char outbuf[ MAX_INPUT_LENGTH ];

    /* szukamy pierwszego slowa kluczowego */
    one_argument( ch->name, name );

    /* niezbyt prawdopodobne */
    if ( !*name )
    {
	strcpy( outbuf, "" );
	return outbuf;
    }

    for ( rch = ch->in_room->people; rch && ( rch != ch ); rch = rch->next_in_room )
	if ( can_see( viewer, rch ) && is_name( name, rch->name ) )
	    count++;

    /* podjelismy decyzje, zeby pokazywalo "1.lam", bedzie bardziej uwidaczniac
       ewentualne bledy progow */
    sprintf( outbuf, "%d.%s", count, name );

    return outbuf;
}


/* Lam 3.5.2003, na potrzeby use_magical_obj i progow */
const char *name_expand_obj( CHAR_DATA *viewer, OBJ_DATA *obj )
{
    int count = 1;
    OBJ_DATA *robj;
    char name[ MAX_INPUT_LENGTH ];

    static char outbuf[ MAX_INPUT_LENGTH ];

    /* szukamy pierwszego slowa kluczowego */
    one_argument( obj->name, name );

    /* niezbyt prawdopodobne */
    if ( !*name )
    {
	strcpy( outbuf, "co`s?" );
	return outbuf;
    }

    /* uznalem, ze nalezy szukac w inwentarzu, malo jest polecen, ktore
       zagladaja gdzie indziej (np. na ziemie), a raczej mob bedzie chcial
       uzywac przedmiotu z inwentarza, niz patrzec na cos na podlodze */
    for ( robj = viewer->carrying; robj && ( robj != obj ); robj = robj->next_content )
	if ( can_see_obj( viewer, robj ) && is_name( name, robj->name ) )
	    count++;

    /* podjelismy decyzje, zeby pokazywalo 1.lam, bedzie bardziej uwidaczniac
       ewentualne bledy progow */
    sprintf( outbuf, "%d.%s", count, name );

    return outbuf;
}


/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;
    int        count = 0;

    if ( ( wch = real_get_char_room( ch, argument, &count ) ) )
	return wch;

    number = number_argument( argument, arg );
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( !can_see( ch, wch )
	  || !is_name( arg, wch->name )
	  || wch->in_room == ch->in_room )
	{
	    continue;
	}

	if ( ++count == number )
	    return wch;
    }

    return NULL;
}


/*
 * Szuka GRACZA w swiecie, a jesli takiego gracza nie ma, to mob tez sie nada.
 * Chodzi o to, zeby moc stosowac skroty, a i tak trafiac na graczy.
 */
CHAR_DATA *get_char_world_pc( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;
    int        count = 0;

    if ( ( wch = real_get_char_room( ch, argument, &count ) ) )
	return wch;

    number = number_argument( argument, arg );
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( IS_NPC( wch )
	  || !can_see( ch, wch )
	  || !is_name( arg, wch->name )
	  || wch->in_room == ch->in_room )
	{
	    continue;
	}

	if ( ++count == number )
	    return wch;
    }

    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( !IS_NPC( wch )
	  || !can_see( ch, wch )
	  || !is_name( arg, wch->name )
	  || wch->in_room == ch->in_room )
	{
	    continue;
	}

	if ( ++count == number )
	    return wch;
    }

    return wch;
}


/*
 * Kolejna odmiana, ta z kolei do uzytku przy telepatii - wystarczy, ze wiem
 * o obecnosci kogos w swiecie (?kto), a moge sie z nim telepatycznie
 * porozumiec, chocby byl niewidzialny i siedzial gdzies po ciemku (to na
 * telepatie wplywu miec nie moze).
 */
CHAR_DATA *get_char_world_pc_tele( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;
    int        count = 0;

    if ( ( wch = real_get_char_room( ch, argument, &count ) ) )
	return wch;

    number = number_argument( argument, arg );
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( IS_NPC( wch )
	  || !can_see_who( ch, wch )
	  || !is_name( arg, wch->name )
	  || ( wch->in_room == ch->in_room && can_see( ch, wch ) ) )
	{
	    continue;
	}

	if ( ++count == number )
	    return wch;
    }

    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( !IS_NPC( wch )
	  || !can_see_who( ch, wch )
	  || !is_name( arg, wch->name )
	  || ( wch->in_room == ch->in_room && can_see( ch, wch ) ) )
	{
	    continue;
	}

	if ( ++count == number )
	    return wch;
    }

    return wch;
}


/*
 * Lam: Szuka CHAR_DATA w krainie, zmodyfikowalem get_char_world
 */
CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;
    int        count = 0;

    if ( ( wch = real_get_char_room( ch, argument, &count ) ) )
	return wch;

    number = number_argument( argument, arg );
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( !can_see( ch, wch )
	  || !is_name( arg, wch->name )
	  || wch->in_room == ch->in_room
	  /* to jedyna zmiana: */
	  || ch->in_room->area != wch->in_room->area )
	{
	    continue;
	}

	if ( ++count == number )
	    return wch;
    }

    return NULL;
}


/* Wszystkie wersje z numerem 2 szukaja po calej nazwie zamiast skrotami */
CHAR_DATA *get_char_area2( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;
    int        count = 0;

    if ( ( wch = real_get_char_room2( ch, argument, &count ) ) )
	return wch;

    number = number_argument( argument, arg );
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( !can_see( ch, wch )
	  || !is_name2( arg, wch->name )
	  || wch->in_room == ch->in_room
	  /* to jedyna zmiana: */
	  || ch->in_room->area != wch->in_room->area )
	{
	    continue;
	}

	if ( ++count == number )
	    return wch;
    }

    return NULL;
}


/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room2( CHAR_DATA *ch, char *argument )
{
    int        count = 0;

    return real_get_char_room2( ch, argument, &count );
}


/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world2( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;
    int        count = 0;

    if ( ( wch = real_get_char_room2( ch, argument, &count ) ) )
	return wch;

    number = number_argument( argument, arg );
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( !can_see( ch, wch )
	  || !is_name2( arg, wch->name )
	  || wch->in_room == ch->in_room )
	{
	    continue;
	}

	if ( ++count == number )
	    return wch;
    }

    return NULL;
}


/*
 * Lam 9.7.2006
 */
CHAR_DATA *get_mob_world_vnum( CHAR_DATA *ch, char *argument )
{
    char arg[ MIL ];
    CHAR_DATA *mob;
    int vnum;
    int number;
    int count = 0;

    number = number_argument( argument, arg );
    if ( !( vnum = atoi( arg ) ) )
    {
	/* bug( "bledny format vnuma", 0 ); -- nie sprawdzam w mstat, mozna
	   recznie podac vnum 0, za to na 100% bedzie to liczba, wiec nie
	   bledny format */
	return NULL;
    }

    for ( mob = char_list; mob; mob = mob->next )
    {
	if ( !mob->pIndexData
	  || mob->pIndexData->vnum != vnum
	  || !can_see( ch, mob ) )
	{
	    continue;
	}

	if ( ++count == number )
	    return mob;
    }

    return NULL;
}


/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
    OBJ_DATA *obj;

    for ( obj = object_list; obj; obj = obj->next )
	if ( !obj->deleted
	  && obj->pIndexData == pObjIndex )
	{
	    return obj;
	}

    return NULL;
}


/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    number = number_argument( argument, arg );
    count  = 0;

    for ( obj = list; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj )
	  && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}

    return NULL;
}


/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( obj->wear_loc == WEAR_NONE
	    && can_see_obj( ch, obj )
	    && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}

    return NULL;
}


/*
 * Znajduje obiekt w przechowalni
 */
OBJ_DATA *get_obj_przechowalnia( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->przechowalnia; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj )
	    && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}

    return NULL;
}


/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( obj->wear_loc != WEAR_NONE
	    && can_see_obj( ch, obj )
	    && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}

    return NULL;
}


OBJ_DATA *real_get_obj_here( CHAR_DATA *ch, char *argument, int *count )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       number;

    number = number_argument( argument, arg );

    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
	    if ( ++*count == number )
		return obj;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj ) && obj->wear_loc == WEAR_NONE
	  && is_name( arg, obj->name ) )
	{
	    if ( ++*count == number )
		return obj;
	}

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj ) && obj->wear_loc != WEAR_NONE
	  && is_name( arg, obj->name ) )
	{
	    if ( ++*count == number )
		return obj;
	}

    return NULL;
}


/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
    int      count = 0;

    return real_get_obj_here( ch, argument, &count );
}


/*
 * Tissarius 14.10.2002: szukanie obiektu po vnumie
 * Lam 9.7.2006: dodana skladnia 12.1234 (number_argument)
 */
OBJ_DATA *get_obj_world_vnum( CHAR_DATA *ch, char *argument )
{
    char arg[ MIL ];
    OBJ_DATA *obj;
    int vnum;
    int number;
    int count = 0;

    number = number_argument( argument, arg );
    if ( !( vnum = atoi( arg ) ) )
    {
	/* bug( "bledny format vnuma", 0 ); - komentarz jak w get_mob_itd. */
	return NULL;
    }

    for ( obj = object_list; obj; obj = obj->next )
	if ( !obj->deleted
	  && ( !ch || can_see_obj( ch, obj ) )
	  && obj->pIndexData && obj->pIndexData->vnum == vnum )
	{
	    if ( ++count == number )
		return obj;
	}

    return NULL;
}


/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg[ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    number = number_argument( argument, arg );
    count  = 0;

    if ( ( obj = real_get_obj_here( ch, argument, &count ) ) )
	return obj;

    for ( obj = object_list; obj; obj = obj->next )
	if ( obj->in_room != ch->in_room
	  && obj->carried_by != ch
	  && !obj->deleted
	  && can_see_obj( ch, obj )
	  && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}

    return NULL;
}


/*
 * Create a 'money' obj.
 */
OBJ_DATA *create_money( int amount )
{
    OBJ_DATA *obj;

    if ( amount <= 0 )
    {
	bug( "Create_money: zero or negative money %d.", amount );
	amount = 1;
    }

    if ( amount == 1 )
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE  ), 0 );
    else
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0 );

    obj->value[ 0 ].v = amount;

    return obj;
}


/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number( OBJ_DATA *obj )
{
    return 1;
/* - problematyczny kod, usuniety 17.6.98 z inicjatywy Winchestera
    int number;

    number = 0;
    if ( obj->item_type == ITEM_CONTAINER )
	for ( obj = obj->contains; obj; obj = obj->next_content )
	{
	    if ( obj->deleted )
		continue;
	    number += get_obj_number( obj );
	}
    else
	number = 1;

    return number;
*/
}


/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight( OBJ_DATA *obj )
{
    int weight = obj->weight;

    for ( obj = obj->contains; obj; obj = obj->next_content )
	if ( !obj->deleted )
	    weight += get_obj_weight( obj );

    return weight;
}


/*
 * Brottor 22.5.2007: to co wyzej, ale pomija mase (wage) przedmiotow
 * niewidzialnych dla CH. Na potrzeby patrz_w w act_info.c.
 */
int get_obj_weight_vis_only( CHAR_DATA *ch, OBJ_DATA *obj )
{
    int weight = obj->weight;

    for ( obj = obj->contains; obj; obj = obj->next_content )
	if ( !obj->deleted && can_see_obj( ch, obj ) )
	    weight += get_obj_weight_vis_only( ch, obj );

    return weight;
}


/*
 * True if room is dark.
 */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *ch;

    if ( pRoomIndex->light > 0 )
	return FALSE;

    /* Lam 3.12.2004: obj_to_room/obj_from_room zmieniaja teraz ->light
    for ( obj = pRoomIndex->contents; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	if ( obj->item_type == ITEM_LIGHT && obj->value[ 2 ].v != 0 )
	    return FALSE;
    } */

    if ( room_is_affected( pRoomIndex, gsn_rozpalanie_ognisk ) )
	return FALSE;

    for ( ch = pRoomIndex->people; ch; ch = ch->next_in_room )
	if ( !ch->deleted && IS_AFFECTED( ch, AFF_MROK ) )
	    return TRUE;

    if ( pRoomIndex->sector_type == SECT_INSIDE
      || pRoomIndex->sector_type == SECT_CITY
      || IS_SET( pRoomIndex->room_flags, ROOM_INDOORS ) )
    {
	if ( IS_SET( pRoomIndex->room_flags, ROOM_DARK ) )
	    return TRUE;

	return FALSE;
    }

    if ( pRoomIndex->area->weather_info.sunlight == SUN_SET
      || pRoomIndex->area->weather_info.sunlight == SUN_DARK )
    {
	return TRUE;
    }

    return FALSE;
}


/*
 * True if room is private.
 */
bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *rch;
    int        count;

    /* pojemnosc wprowadzona przez Lama. pojemnosc == 0 - nieskonczone */
    if ( !pRoomIndex->pojemnosc )
	return FALSE;

    /* uwaga: ujemna pojemnosc spowoduje, ze nie bedzie sie tam dalo wejsc */
    count = 0;
    for ( rch = pRoomIndex->people; rch; rch = rch->next_in_room )
    {
	if ( rch->deleted )
	    continue;

	/* Lam 10.4.2004: 103 to poziom BN, na ktorym raczej nie chce sie
	   byc widzialnym/wykrywalnym przez smiertelnikow */
	if ( !IS_NPC( rch )
	  && IS_SET( rch->act, PLR_WIZINVIS )
	  && rch->pcdata->wizinvis >= 103 )
	{
	    continue;
	}

	count++;
    }

    /* Lam 15.12.2000: Qwert zauwazyl, ze bylo > zamiast >= */
    if ( count >= pRoomIndex->pojemnosc )
	return TRUE;

    return FALSE;
}


bool can_enter_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room, bool pokaz )
{
    int i, poziomy[ MAX_CLASS ];

    if ( !IS_NPC( ch ) && ch->level >= L_SEN )
	return TRUE;

    if ( room_is_private( room ) )
    {
	if ( pokaz )
	    send_to_char( "To pomieszczenie jest w tej chwili przepe`lnione.\n\r", ch );
	return FALSE;
    }

    if ( !IS_NPC( ch ) )
    {
	/* Lam: anty-profesje */
	if ( !IS_SET( room->room_flags, ROOM_INVERT_CLASSES )
	  && is_name2( class_table[ ch->klasa ].who_name, room->anti_class ) )
	{
	    if ( pokaz )
		send_to_char( "Nie wolno ci tam wej`s`c.\n\r", ch );
	    return FALSE;
	}

	/* wybrane profesje, tu odrebna obsluga BWWU */
	if ( IS_SET( room->room_flags, ROOM_INVERT_CLASSES )
	  && !is_name2( class_table[ ch->klasa ].who_name, room->anti_class ) )
	{
	    if ( ch->level >= L_APP )
	    {
		wypelnij_poziomy( ch, poziomy );

		for ( i = 0; i < MAX_CLASS; i++ )
		    if ( poziomy[ i ] >= 100
		      && is_name2( class_table[ i ].who_name, room->anti_class ) )
		    {
			break;
		    }

		/* nie ukonczyl gry zadna profesja, ktora moze tu wchodzic */
		if ( i == MAX_CLASS )
		{
		    if ( pokaz )
			send_to_char( "Nie wolno ci tam wej`s`c.\n\r", ch );
		    return FALSE;
		}
	    }
	    else
	    {
		if ( pokaz )
		    send_to_char( "Nie wolno ci tam wej`s`c.\n\r", ch );
		return FALSE;
	    }
	}

	/* Lam: anty-rasy */
	if ( ( !IS_SET( room->room_flags, ROOM_INVERT_RACES )
	    && is_name2( race_table[ ch->race ].ang, room->anti_race ) )
	  || ( IS_SET( room->room_flags, ROOM_INVERT_RACES )
	    && !is_name2( race_table[ ch->race ].ang, room->anti_race ) ) )
	{
	    if ( pokaz )
		send_to_char( "Pod t`a postaci`a nie mo`zesz tam wej`s`c.\n\r", ch );
	    return FALSE;
	}

	if ( IS_AFFECTED( ch, AFF_POSTAC_NIETOPERZA ) /* CZY_WAMPIR( ch ) */
	  && ( ( !IS_SET( room->room_flags, ROOM_INVERT_RACES )
	      && is_name2( "vampire", room->anti_race ) )
	    || ( IS_SET( room->room_flags, ROOM_INVERT_RACES )
	      && !is_name2( "vampire", room->anti_race ) ) ) )
	{
	    if ( pokaz )
		send_to_char( "Pod t`a postaci`a nie mo`zesz tam wej`s`c.\n\r", ch );
	    return FALSE;
	}
    }

    return TRUE;
}


bool jest_w_lodzi( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    bool jest = FALSE;
    bool slepy = FALSE;

    if ( ch->in_room->sector_type != SECT_WATER_DEEP
      && ch->in_room->sector_type != SECT_WATER_SHALLOW )
    {
	return FALSE;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->pcdata->temp_flags, TEMPFLAG_BLIND ) )
    {
	slepy = TRUE;
	REMOVE_BIT( ch->pcdata->temp_flags, TEMPFLAG_BLIND );
    }

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( obj->item_type == ITEM_BOAT && can_see_obj( ch, obj ) )
	{
	    jest = TRUE;
	    break;
	}

    if ( slepy )
	SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_BLIND );

    return jest;
}


/*
 * Lam: mozliwosc (umiejetnosc) poruszania sie
 * Ten kod ma prawo zakladac, ze ktos, kto nie umie latac lub plywac, spadl
 * wczesniej na ziemie lub opadl na dno. Argument "czy" powstal jako
 * rozroznienie pokazywania komunikatow, ale poniewaz oznacza, ze to sam ch
 * probuje przejsc, to jesli mu sie to uda, w tej funkcji bedzie sie uczyl
 * plywania - jesli uzyjesz can_move tak, ze mimo podania czy == TRUE, postac
 * nie bedzie wykonywala ruchu, dodaj nowy argument, a na razie wystarczy ten.
 * Jest tez uzywana dla in_room == to_room do sprawdzania, czy ktos moze sie
 * ruszyc z danego miejsca (chocby teoretycznie)
 */
int can_move( CHAR_DATA *ch, ROOM_INDEX_DATA *in_room,
	ROOM_INDEX_DATA *to_room, int door, bool czy )
{
    int ret = 2;

    /* dla czy == TRUE bedzie zapewne probowal rozrywac sidla, ale dla
       czy == FALSE jest pchany/ciagany lub symuluje nadejscie do pomieszczenia
       podczas odnawiania krainy */
    if ( !czy
      && IS_AFFECTED( ch, AFF_HOLD ) )
    {
	return 0;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM )
      && ch->master
      && in_room == ch->master->in_room )
    {
	if ( czy )
	    send_to_char( "Co? Jeszcze nie pad`l rozkaz, by opu`sci`c to pomieszczenie!\n\r", ch );
	return 0;
    }

    /* latanie w powietrzu - w gore wymaga latania, w bok nie wymaga - najwyzej
       spowoduje spadanie */
    if ( CZY_POWIETRZE( to_room )
      && door == DIR_UP
      && !IS_FLYING( ch ) )
    {
	if ( czy )
	    send_to_char( "Nie umiesz lata`c.\n\r", ch );
	return 0;
    }

    /* rybom wystarczy nawet plytka woda, ale na ladzie bez latania nie da
       rady (FIXME: wlasciwie powinno sie je mimo to dac pchac) */
    if ( ( !CZY_WODA( to_room )
	|| !CZY_WODA( in_room ) )
      && !IS_SET( race_table[ ch->race ].poruszanie, POR_LAD )
      && !IS_FLYING( ch ) )
    {
	if ( czy )
	    send_to_char( "Chocia`z miotasz si`e, nie mo`zesz si`e poruszy`c!\n\r", ch );
	return 0;
    }

    /* powietrze i lad za nami, kolej na wode: */
    /* w plytkiej wodzie moga sie ruszac wszyscy - zakladamy naiwnie, ze rekin
       ma dosc wody, zeby przeplynac, wszystko zas inne brnie po dnie (plytka
       woda to taka, gdzie dno jest w tym samym pomieszczeniu) */

    /* pozostaje wiec gleboka woda:
       - na powierzchni glebokiej wody mozna plynac lodzia
       - mozna sobie leciec nad jej powierzchnia
       - mozna sobie plynac w wodzie
       - z lodzi/latania zawsze mozna wejsc do wody (kierunek "w dol") */
    if ( door != DIR_DOWN /* FIXME: wolno wpychac w wode ofiary bez oddychania */
      && ( in_room->sector_type == SECT_WATER_DEEP
	|| ( to_room->sector_type == SECT_WATER_DEEP
	  && door != DIR_UP ) ) /* uwaga ponizej! */
      && !IS_FLYING( ch )
      && !IS_SET( race_table[ ch->race ].poruszanie, POR_POD_WODA )
      && !IS_SET( race_table[ ch->race ].poruszanie, POR_PO_WODZIE )
      && !IS_AFFECTED( ch, AFF_KONTROLA_NAD_WODA )
      && !jest_w_lodzi( ch ) )
    {
	if ( !IS_NPC( ch ) && ch->pcdata->learned[ gsn_plywanie ] )
	{
	    /* if ( czy ) uzyj( ch, gsn_plywanie, "p`lywaniu" ); */
	    ret = 1;
	}
	else
	{
	    /* mozna wskoczyc z ladu do wody (tak jak zeskoczyc w przepasc),
	       poruszanie ladem sprawdzone bylo wyzej */
	    if ( CZY_WODA( in_room ) )
	    {
		if ( czy )
		    send_to_char( "Nie potrafisz p`lywa`c.\n\r", ch );
		return 0;
	    }
	}
    }

    /* obiecana uwaga: jesli pomieszczenie "nad" to "gleboka woda", a idziemy
       w gore, ten kod zaklada, ze pod "gleboka woda" zawsze jest "pod woda"!
       wykrywacze w db.c to sprawdzaja i inaczej byc nie moze, dlatego powyzej
       nie bylo sprawdzania kierunku "w gore", zalapie sie ono ponizej: */

    /* pod woda dozwolone sa tylko przejscia pod wode lub w gore do glebokiej
       wody; z glebokiej wody mozna opasc na dno w kazdej chwili wedle woli
       (czyli sprawdzam wszystkie kierunki, ale "w dol" nie) */
    if ( door != DIR_DOWN
      && ( CZY_POD_WODA( in_room )
	|| CZY_POD_WODA( to_room ) )
      && !IS_SET( race_table[ ch->race ].poruszanie, POR_POD_WODA )
      && !IS_AFFECTED( ch, AFF_KONTROLA_NAD_WODA ) )
    {
	if ( !IS_NPC( ch ) && ch->pcdata->learned[ gsn_plywanie ] )
	{
	    /* if ( czy ) uzyj( ch, gsn_plywanie, "p`lywaniu" ); */
	    ret = 1;
	}
	else
	{
	    /* nie umie plywac, ale jesli ma pod stopami dno, moze moze przejsc
	       plasko? */
	    if ( door == DIR_UP
	      || !CZY_DNO_POD_WODA( in_room )
	      || !IS_SET( race_table[ ch->race ].poruszanie, POR_LAD ) )
	    {
		if ( czy )
		    send_to_char( "`Zeby si`e tam uda`c musisz umie`c p`lywa`c pod wod`a.\n\r", ch );
		return 0;
	    }
	}
    }

    /* sprawdzona mozliwosc przemieszczenia sie */
    /* Vigud: zwraca 0, gdy ch nie moze przejsc, 1, gdy moze przejsc i dodatkowo
       uczy sie plywac, 2, gdy moze przejsc, ale nie uczy sie plywac */
    return ret;
}


/*
 * True if char can see victim.
 */
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->deleted )
	return FALSE;

    if ( ch == victim )
	return TRUE;

    /* moby nie widza boskiej niewidzialnosci, niezaleznie od poziomu... */
    if ( IS_NPC( ch )
	&& !IS_NPC( victim ) && IS_SET( victim->act, PLR_WIZINVIS ) )
	return FALSE;

    /* ... w przeciwienstwie do graczy */
    if ( !IS_NPC( victim )
	&& IS_SET( victim->act, PLR_WIZINVIS )
	&& get_trust( ch ) < victim->pcdata->wizinvis )
	return FALSE;

/*  if ( IS_NPC( victim ) && victim->pIndexData->vnum == MOB_VNUM_SUPERMOB )
	return FALSE; */ /* zeby nie bylo widac w rprogu po mpforce $n patrz */

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( PP( ch ) && PP( victim ) )
	return TRUE;

    if ( IS_AFFECTED( ch, AFF_BLIND )
      || ( !IS_NPC( ch )
	&& IS_SET( ch->pcdata->temp_flags, TEMPFLAG_BLIND ) ) )
	return FALSE;

    /* Tu byl blad: bylo ch->in_room - trudne do wykrycia, gdyz dopoki nie sa
       w roznych pokojach, to sie nie ujawnia - odkryte przy skanowaniu.
       Lam 25.8.98: z jasnego pomieszczenia nie zajrzysz do ciemnego */
    if ( victim->in_room && room_is_dark( victim->in_room )
	&& ( !ch->in_room
	  || !room_is_dark( ch->in_room )
	  || !HAS_INFRAVISION( ch ) ) )
	return FALSE;

    if ( victim->position == POS_DEAD )
	return TRUE;

    if ( IS_AFFECTED( victim, AFF_INVISIBLE )
	&& !HAS_DETECT_INVIS( ch ) )
	return FALSE;

    if ( IS_AFFECTED( victim, AFF_HIDE )
	&& !HAS_DETECT_HIDDEN( ch )
	&& !victim->fighting )
	return FALSE;

    return TRUE;
}


/*
 * Lam: podgladajac inne pomieszczenia magicznie (czarem "okno") nie potrzebuje
 * wlasnego wzroku, moge tez zagladac w ciemne miejsca.
 * Kopia can_see bez niektorych fragmentow.
 */
bool can_see_dla_okna( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->deleted )
	return FALSE;

    if ( ch == victim )
	return TRUE;

    /* moby nie widza boskiej niewidzialnosci, niezaleznie od poziomu... */
    if ( IS_NPC( ch )
	&& !IS_NPC( victim ) && IS_SET( victim->act, PLR_WIZINVIS ) )
	return FALSE;

    /* ... w przeciwienstwie do graczy */
    if ( !IS_NPC( victim )
	&& IS_SET( victim->act, PLR_WIZINVIS )
	&& get_trust( ch ) < victim->pcdata->wizinvis )
	return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( PP( ch ) && PP( victim ) )
	return TRUE;

    /* tu pominiete fragmenty w porownaniu do can_see */
    if ( victim->in_room && room_is_dark( victim->in_room )
	&& ( !ch->in_room
	  || !HAS_INFRAVISION( ch ) ) )
	return FALSE;

    if ( victim->position == POS_DEAD )
	return TRUE;

    if ( IS_AFFECTED( victim, AFF_INVISIBLE )
	&& !HAS_DETECT_INVIS( ch ) )
	return FALSE;

    if ( IS_AFFECTED( victim, AFF_HIDE )
	&& !HAS_DETECT_HIDDEN( ch )
	&& !victim->fighting )
	return FALSE;

    return TRUE;
}


/*
 * Lam 7.6.98: wersja can_see dla do_who
 */
bool can_see_who( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->deleted )
	return FALSE;

    if ( ch == victim )
	return TRUE;

    if ( IS_NPC( ch )
	&& !IS_NPC( victim ) && IS_SET( victim->act, PLR_WIZINVIS ) )
	return FALSE;

    if ( !IS_NPC( victim )
	&& IS_SET( victim->act, PLR_WIZINVIS )
	&& get_trust( ch ) < victim->pcdata->wizinvis
	&& !IS_SET( victim->act, PLR_BOSKA_KUKLA ) ) /* Vigud */
	return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( PP( ch ) /* && PP( victim ) */ )
	return TRUE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) )
	return FALSE;

    return TRUE;
}


/*
 * True if char can see obj.
 */
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;
    CHAR_DATA *carried_by = NULL;

#if 0 /* jednak "wyslij" tez zglasza duzo bledow */
    if ( obj->deleted )
    {
	/* Lam 24.6.2004: to nie powinno sie zdarzac, poniewaz extract_obj
	   usuwa przedmiot z wszelkich list poza obj_list (co najwyzej
	   pgdzie zglosi bledy, wtedy nalezy usunac to tymczasowe rozwiazanie,
	   poniewaz do tej pory bedzie wiadomo, czy sie przydalo) */
	bug( "can_see_obj: obj deleted", 0 );
	return FALSE;
    }
#endif

    /* Lam 20.3.2004: nie widac przedmiotow postaci, ktorej sie nie widzi */
    if ( obj->carried_by )
	carried_by = obj->carried_by;

    if ( obj->owned_by )
	carried_by = obj->owned_by;

    for ( obj_from = obj->in_obj; obj_from; obj_from = obj_from->in_obj )
    {
	if ( obj_from->carried_by )
	    carried_by = obj_from->carried_by;
	if ( obj_from->owned_by )
	    carried_by = obj_from->owned_by;
    }

    if ( carried_by && !can_see( ch, carried_by ) )
	return FALSE;

    /* Lam 16.9.2004: nie widac sklepu oczarowanego sklepikarza, nie widzi go
       nawet sam sklepikarz, zeby nie moc oddac towaru */
    if ( carried_by
      && IS_NPC( carried_by )
      && carried_by->pIndexData->pShop
      && IS_AFFECTED( carried_by, AFF_CHARM )
      && IS_OBJ_STAT( obj, ITEM_INVENTORY )
      && ( IS_NPC( ch )
	|| ch->level < L_SEN ) )
	return FALSE;

    if ( !IS_NPC( ch )
      && IS_SET( ch->act, PLR_HOLYLIGHT )
      && !IS_OBJ_STAT( obj, ITEM_BURIED ) )
	return TRUE;

    if ( IS_AFFECTED( ch, AFF_BLIND )
      || ( !IS_NPC( ch )
	&& IS_SET( ch->pcdata->temp_flags, TEMPFLAG_BLIND ) ) )
	return FALSE;

    /* Swiatlo widac po ciemku, jesli jest zapalone. Latarnia w inwentarzu
       nie jest zapalona, ale kula swiatla jest. */
    if ( obj->item_type == ITEM_LIGHT
      && obj->value[ 2 ].v != 0
      && ( obj->value[ 1 ].v == 0 || obj->wear_loc == WEAR_LIGHT ) )
	return TRUE;

    if ( !HAS_INFRAVISION( ch )
      && room_is_dark( ch->in_room ) )
	return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_INVIS )
      && !IS_SET( race_table[ ch->race ].race_abilities, RACE_DETECT_INVIS )
      && !IS_AFFECTED( ch, AFF_DETECT_INVIS ) )
	return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_BURIED ) )
	return FALSE; /* Lam 15.11.1999 */

    return TRUE;
}


/*
 * Lam 23.3.2005
 */
bool can_see_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room )
{
    if ( !ch )
    {
	bug( "can_see_room: null ch", 0 );
	return FALSE;
    }

    if ( !room )
    {
	bug( "can_see_room: null room", 0 );
	return FALSE;
    }

    /* pewnie wiekszosc funkcji, ktore beda wywolywac can_see_room, juz zawiera
       te testy (np. poprzez check_blind( )), ale przezornie sprawdzmy */
    if ( ( IS_NPC( ch )
	|| !IS_SET( ch->act, PLR_HOLYLIGHT ) )
      && ( IS_AFFECTED( ch, AFF_BLIND )
	|| ( !IS_NPC( ch )
	  && IS_SET( ch->pcdata->temp_flags, TEMPFLAG_BLIND ) ) ) )
	return FALSE;

    /* jesli nie widzi w ciemnosci w ogole, nie zobaczy ciemnego, zobaczy jasne */
    if ( ( IS_NPC( ch )
	|| !IS_SET( ch->act, PLR_HOLYLIGHT ) )
      && !HAS_INFRAVISION( ch ) )
	return !room_is_dark( room );

    /* jesli widzi w ciemnosci, zobaczy ciemne z ciemnego, ale nie ciemne
       z jasnego */
    if ( room != ch->in_room
      && !room_is_dark( ch->in_room ) )
	return !room_is_dark( room );

    /* widzi w ciemnosci i patrzy z ciemnego na cokolwiek albo na pomieszczenie,
       w ktorym stoi */
    return TRUE;
}


/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_OBJ_STAT( obj, ITEM_NODROP )
      || ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL ) )
    {
	return TRUE;
    }

    return FALSE;
}


/*
 * Tabele z dopuszczalnymi wartosciami pola, tabele z dopuszczalnymi flagami
 * w polu. Latwiejsza od stosu ifow edycja, szybsza obsluga wielu zwiazanych
 * z kazda wartoscia/flaga cech. Aktualnie mozliwa zamiana wartosci na lancuch
 * (do *statow) oraz automatyczne sprawdzanie poprawnosci krain (zglaszanie
 * umieszczania w nich nieobslugiwanych wartosci lub flag).
 *
 * Na poczatek ogolne funkcje dla wszystkich rodzajow funkcji.
 */
char *choice_field_to_name( struct choice_field_data *values, int value,
				bool trusted )
{
    int i;

    for ( i = 0; values[ i ].name; i++ )
	if ( values[ i ].value == value
	  && ( trusted || !values[ i ].auth_required ) )
	{
	    return values[ i ].name;
	}

    return "(b`l`ad)";
}


bool choice_field_avail( struct choice_field_data *values, int value )
{
    int i;

    for ( i = 0; values[ i ].name; i++ )
	if ( values[ i ].value == value )
	    return values[ i ].available;

    return FALSE;
}


char *bit_field_to_name( struct bit_field_data *values, int value,
				bool trusted )
{
    static char buf[ 1024 ];
    int i;

    buf[ 0 ] = '\0';

    for ( i = 0; values[ i ].name; i++ )
	if ( IS_SET( value, values[ i ].bitv )
	  && ( trusted || !values[ i ].auth_required ) )
	{
	    strcat( buf, " " );
	    strcat( buf, values[ i ].name );
	}

    return buf[ 0 ] ? buf + 1 : "(brak)";
}


bool bit_field_avail( struct bit_field_data *values, int value )
{
    int i;

    /* value to lokalna kopia, moge jej czyscic dozwolone bity */
    for ( i = 0; values[ i ].name; i++ )
	if ( values[ i ].available )
	    REMOVE_BIT( value, values[ i ].bitv );

    /* jesli cos zostalo, jest niedozwolone */
    return !value;
}


char *bitvector_field_to_name( struct bitvector_field_data *values,
				int *value, bool trusted, bool dla_ludzi )
{
    static char buf[ 1024 ];
    int i;

    buf[ 0 ] = '\0';

    if ( dla_ludzi )
    {
	for ( i = 0; values[ i ].name; i++ )
	    if ( IS_SET_V( value, values[ i ].bitn )
	      && ( trusted || !values[ i ].auth_required ) )
	    {
		if ( buf[ 0 ] == ' ' )
		    strcat( buf, ", " );
		else
		    strcat( buf, " " );
		strcat( buf, values[ i ].name );
	    }

	/* "ssace_mane" i podobne */
	for ( i = 1; i < 1024 && buf[ i ] != '\0'; i++ )
	    if ( buf[ i ] == '_' )
		buf[ i ] = ' ';
    }
    else
	for ( i = 0; values[ i ].name; i++ )
	    if ( IS_SET_V( value, values[ i ].bitn )
	      && ( trusted || !values[ i ].auth_required ) )
	    {
		strcat( buf, " " );
		strcat( buf, values[ i ].name );
	    }

    return buf[ 0 ] ? buf + 1 : "(brak)";
}


bool bitvector_field_avail( struct bitvector_field_data *values,
				int *value, int size )
{
    int lokalna[ 16 ]; /* zeby nie uzywac malloc */
    int i;

    /* COPY_BITS */
    for ( i = 0; i < size; i++ )
	lokalna[ i ] = value[ i ];

    for ( i = 0; values[ i ].name; i++ )
	if ( values[ i ].available )
	    REMOVE_BIT_V( lokalna, values[ i ].bitn );

    /* jesli cos zostalo, jest niedozwolone */
    for ( i = 0; i < size; i++ )
	if ( lokalna[ i ] )
	    return FALSE;

    return TRUE;
}


struct choice_field_data apply_values[ ] =
{
    { APPLY_NONE,		"nic",				FALSE,	FALSE },
    { APPLY_STR,		"si`l`e",			TRUE,	FALSE },
    { APPLY_DEX,		"zr`eczno`s`c",			TRUE,	FALSE },
    { APPLY_INT,		"inteligencj`e",		TRUE,	FALSE },
    { APPLY_WIS,		"m`adro`s`c",			TRUE,	FALSE },
    { APPLY_CON,		"budow`e fizyczn`a",		TRUE,	FALSE },
    { APPLY_SEX,		"p`le`c",			FALSE,	FALSE },
    { APPLY_CLASS,		"klas`e",			FALSE,	FALSE },
    { APPLY_LEVEL,		"poziom",			FALSE,	FALSE },
    { APPLY_AGE,		"wiek",				FALSE,	FALSE },
    { APPLY_HEIGHT,		"wysoko`s`c",			FALSE,	FALSE },
    { APPLY_WEIGHT,		"wag`e",			FALSE,	FALSE },
    { APPLY_MANA,		"man`e",			TRUE,	FALSE },
    { APPLY_HIT,		"`zycie",			TRUE,	FALSE },
    { APPLY_MOVE,		"ruch",				TRUE,	FALSE },
    { APPLY_GOLD,		"z`loto",			FALSE,	FALSE },
    { APPLY_EXP,		"do`swiadczenie",		FALSE,	FALSE },
    { APPLY_AC,			"klas`e pancerza",		TRUE,	FALSE },
    { APPLY_HITROLL,		"premi`e celno`sci",		TRUE,	FALSE },
    { APPLY_DAMROLL,		"premi`e obra`ze`n",		TRUE,	FALSE },
    { APPLY_SAVING_PARA,	"ochron`e przed parali`zem",	TRUE,	FALSE },
    { APPLY_SAVING_ROD,		"jak`a`s ochron`e",		TRUE,	FALSE },
    { APPLY_SAVING_PETRI,	"ochron`e przed skamienieniem",	TRUE,	FALSE },
    { APPLY_SAVING_BREATH,	"ochron`e przed zioni`eciem",	TRUE,	FALSE },
    { APPLY_SAVING_SPELL,	"ochron`e przed czarami",	TRUE,	FALSE },
    { APPLY_RACE,		"ras`e",			FALSE,	FALSE },
    { APPLY_SIZE,		"wielko`s`c",			FALSE,	FALSE },
    { 0,			NULL,				FALSE,	FALSE }
};


char *affect_loc_name_pl_b( int location )
{
    return choice_field_to_name( apply_values, location, FALSE );
}


bool affect_loc_avail( int location )
{
    return choice_field_avail( apply_values, location );
}


struct bitvector_field_data affect_bit_values[ ] =
{
    { AFF_BLIND,		"o`slepienie",		TRUE,	FALSE,
      "`slepy" },
    { AFF_INVISIBLE,		"niewidzialno`s`c",	TRUE,	FALSE,
      "niewidzialny" },
    { AFF_DETECT_EVIL,		"wykrycie_z`la",	TRUE,	FALSE,
      "" },
    { AFF_DETECT_INVIS,		"wykrycie_niewidki",	TRUE,	FALSE,
      "" },
    { AFF_DETECT_MAGIC,		"wykrycie_magii",	TRUE,	FALSE,
      "" },
    { AFF_DETECT_HIDDEN,	"wykrycie_ukrycia",	TRUE,	FALSE,
      "" },
    { AFF_HOLD,			"pochwycenie",		TRUE,	FALSE,
      "nie mo`ze ucieka`c, `latwiej go trafi`c" },
    { AFF_SANCTUARY,		"u`swi`ecenie",		TRUE,	FALSE,
      "bia`la aura = -1/2 obra`ze`n (twardziel)" },
    { AFF_FAERIE_FIRE,		"pogorszenie_ochrony",	TRUE,	FALSE,
      "r`o`zowa aura" },
    { AFF_INFRARED,		"podczerwie`n",		TRUE,	FALSE,
      "widzi w ciemno`sci" },
    { AFF_CURSE,		"kl`atwa",		TRUE,	FALSE,
      "niemo`zliwo`s`c przywo`lania" },
    { AFF_CHANGE_SEX,		"zmiana_p`lci",		FALSE,	FALSE,
      "" },
    { AFF_POISON,		"otrucie",		FALSE,	FALSE,
      "" },
    { AFF_PROTECT,		"ochrona",		TRUE,	FALSE,
      "-1/4 obra`ze`n od z`lych postaci" },
    { AFF_POLYMORPH,		"przemiana",		FALSE,	FALSE,
      "" },
    { AFF_SNEAK,		"skradanie",		TRUE,	FALSE,
      "nie wida`c wchodzenia/wychodzenia" },
    { AFF_HIDE,			"ukrycie",		TRUE,	FALSE,
      "ukryty" },
    { AFF_SLEEP,		"u`spienie",		TRUE,	FALSE,
      "obudzi`c mo`zna tylko atakiem" },
    { AFF_CHARM,		"oczarowanie",		FALSE,	FALSE,
      "" },
    { AFF_FLYING,		"latanie",		TRUE,	FALSE,
      "lata w powietrzu, zu`zywa ma`lo ruchu" },
    { AFF_PASS_DOOR,		"przenikanie",		TRUE,	FALSE,
      "przechodzi przez szpary zamkni`etych drzwi" },

    /* nieuzywane */
    { AFF_WATERWALK,		"chodzenie_po_wodzie",	FALSE,	FALSE,
      "" },
    { AFF_SUMMONED,		"przywo`lany",		FALSE,	FALSE,
      "" },

    { AFF_MUTE,			"wyciszenie",		TRUE,	FALSE,
      "nie mo`ze m`owi`c na g`los" },
    { AFF_GILLS,		"skrzela",		TRUE,	FALSE,
      "oddychanie pod wod`a" },
    { AFF_VAMP_BITE,		"kl`atwa_wampira",	FALSE,	FALSE,
      "" },
    { AFF_GHOUL,		"kl`atwa_gula",		FALSE,	FALSE,
      "" },
    { AFF_FLAMING,		"p`lon`aca_tarcza",	TRUE,	FALSE,
      "p`lon`aca tarcza, znosi lodow`a" },
    { AFF_BLACK_AURA,		"czarna_aura",		TRUE,	FALSE,
      "czarna aura, obr. rozk`ladane na `zycie i man`e" },
    { AFF_DISORIENTATED,	"oszo`lomienie",	TRUE,	FALSE,
      "taranowanie/technika smoka" },
    { AFF_SPOKOJ,		"spok`oj",		TRUE,	FALSE,
      "aura pokoju, dost`epna BWWU" },
    { AFF_LODOWA_TARCZA,	"lodowa_tarcza",	TRUE,	FALSE,
      "lodowa tarcza, znosi p`lon`ac`a" },
    { AFF_PRZYSPIESZENIE,	"przyspieszenie",	TRUE,	FALSE,
      "skraca czas czekania po poleceniach" },
    { AFF_SPOWOLNIENIE,		"spowolnienie",		TRUE,	FALSE,
      "wyd`lu`za czas czekania po poleceniach" },
    { AFF_REGENERACJA,		"regeneracja",		TRUE,	FALSE,
      "szybka regeneracja atrybut`ow" },
    { AFF_DEGENERACJA,		"degeneracja",		TRUE,	FALSE,
      "wolna regeneracja, brak mo`zliwo`sci leczenia" },
    { AFF_DEZORIENTACJA,	"dezorientacja",	TRUE,	FALSE,
      "czar mnicha, podobny do pija`nstwa" },
    { AFF_SLUGA,		"s`luga",		FALSE,	FALSE,
      "" },
    { AFF_GROZA,		"groza",		TRUE,	FALSE,
      "aura strachu" },
    { AFF_MROK,			"mrok",			TRUE,	FALSE,
      "strefa cienia" },
    { AFF_TARCZA_UMARLYCH,	"tarcza_umar`lych",	TRUE,	FALSE,
      "odpowiada na ciosy jak p`lon`aca/lodowa" },
    { AFF_POSTAC_NIETOPERZA,	"posta`c_nietoperza",	FALSE,	FALSE,
      "" },
    { AFF_ANIOL_STROZ,		"anio`l_str`o`z",	FALSE,	FALSE,
      "" },
    { AFF_SWIETA_AURA,		"`swi`eta_aura",	TRUE,	FALSE,
      "`swi`eta aura paladyna (sam bit daje tylko napis)" },
    { AFF_WRZACA_KREW,		"wrz`aca_krew",		FALSE,	FALSE,
      "" },
    { AFF_DOLADOWANIE,		"do`ladowanie",		TRUE,	FALSE,
      "zabezpiecza przed zara`zeniem wampiryzmem" },
    { AFF_ILUZJA,		"iluzja",		TRUE,	FALSE,
      "ukrywa ekwipunek" },
    { AFF_BRZECZY,		"brz`eczy",		FALSE,	FALSE,
      "" },
    { AFF_SZAL_BOJOWY,		"sza`l",		TRUE,	FALSE,
      "sza`l bojowy" },
    { AFF_ZABURZENIE_ENERGII,	"zaburzenie_energii",	TRUE,	FALSE,
      "drogie i cz`esto wolniejsze rzucanie czar`ow" },
    { AFF_BLOKADA_MENTALNA,	"blokada_mentalna",	TRUE,	FALSE,
      "blokada czarowania psionika" },
    { AFF_KONTROLA_NAD_WODA,	"kontrola_nad_wod`a",	TRUE,	FALSE,
      "pe`lna kontrola nad wod`a" },
    { AFF_PRZEMIESZCZENIE,	"przemieszczenie",	TRUE,	FALSE,
      "znacznie obni`za szanse trafienia" },
    { AFF_TARCZA_ZLA,		"tarcza_z`la",		TRUE,	FALSE,
      "negacja ochrony, -1/4 obra`ze`n od dobrych" },
    { 0,			NULL,			FALSE,	FALSE, "" }
};


/*
 * Return ascii name of an affect bit vector.
 */
char *affect_bit_name_pl( int *vector )
{
    return bitvector_field_to_name( affect_bit_values, vector, TRUE, FALSE );
}


bool affect_bit_avail( int *vector )
{
    return bitvector_field_avail( affect_bit_values, vector, AFF_FLAGS_SIZE );
}


struct bitvector_field_data extra_bit_values[ ] =
{
    { ITEM_GLOW,		"b`lyszczy",		TRUE,	FALSE,
      "dodaje napis \"(b`lyszczy)\", bez znaczenia" },
    { ITEM_HUM,			"brz`eczy",		TRUE,	FALSE,
      "dodaje napis \"(brz`eczy)\" - uniemo`zliwia skradanie" },
    { ITEM_POM,			"pomieszczeniowe",	TRUE,	TRUE,
      "przedmiot jest elementem pomieszczenia\n"
      "(nie wida`c d`lugiego opisu, ale mo`zna wykonywa`c na nim polecenia)" },
    { ITEM_PRIVATE,		"osobiste",		TRUE,	TRUE,
      "przedmiot osobisty, wymaga dostosowania" },
    { ITEM_EVIL,		"z`le",			TRUE,	FALSE,
      "dodaje napis \"(czerwona aura)\", bez znaczenia,\n"
      "najlepiej `l`aczy`c z flag`a \"antydobre\"" },
    { ITEM_INVIS,		"niewidka",		TRUE,	FALSE,
      "przedmiot jest niewidzialny" },
    { ITEM_MAGIC,		"magia",		TRUE,	FALSE,
      "dodaje napis \"(magia)\" - ma wp`lyw na niekt`ore czary i trucie broni" },
    { ITEM_NODROP,		"niepozbywalne",	TRUE,	FALSE,
      "nie da si`e wyrzuci`c z inwentarza" },
    { ITEM_QUEST,		"zadanie",		TRUE,	FALSE,
      "(zadanie) - nie mo`ze mie`c wp`lyw`ow, zawsze zapisywany" },
    { ITEM_ANTI_GOOD,		"antydobre",		TRUE,	FALSE,
      "dobrzy nie mog`a u`zywa`c (parzy ci`e i zrzucasz...)" },
    { ITEM_ANTI_EVIL,		"antyz`le",		TRUE,	FALSE,
      "`xli nie mog`a u`zywa`c (parzy ci`e i zrzucasz...)" },
    { ITEM_ANTI_NEUTRAL,	"antyneutralne",	TRUE,	FALSE,
      "neutralni nie mog`a u`zywa`c (parzy ci`e i zrzucasz...)" },
    { ITEM_NOREMOVE,		"niezdejmowalne",	TRUE,	FALSE,
      "nie da si`e zdj`a`c" },
    { ITEM_INVENTORY,		"inwentarz",		FALSE,	TRUE,
      "" },
    { ITEM_POISONED,		"zatrute",		TRUE,	FALSE,
      "tylko bronie, `zarcie ma w warto`sci 3!" },
    { ITEM_VAMPIRE_BANE,	"antywampirze",		TRUE,	FALSE,
      "przy \"po`zyw\" zabezpiecza ofiar`e przed zara`zeniem!\n"
      "(musi by`c trzymane w d`loniach!)" },
    { ITEM_HOLY,		"`swi`ete",		TRUE,	FALSE,
      "zadaje obra`zenia wampirowi przy zak`ladaniu" },
    { ITEM_LOYAL,		"lojalne",		TRUE,	FALSE,
      "nie da si`e kra`s`c, rozbrajanie do inwentarza" },
    { ITEM_NORANDVALUES,	"nie_losowane",		TRUE,	TRUE,
      "nie losuj`a si`e warto`sci" },
    { ITEM_NO_LOCATE,		"nie_lokalizow",	TRUE,	TRUE,
      "nie mo`zna przedmiotu lokalizowa`c czarami" },
    { ITEM_BURIED,		"zakopane",		TRUE,	FALSE,
      "zakopane! ustawia`c TYLKO przedmiotom le`z`acym na ziemi!" },
    { ITEM_ANTI_MALE,		"antym`eskie",		TRUE,	FALSE,
      "nie mo`ze by`c u`zywane przez m`e`zczyzn" },
    { ITEM_ANTI_FEMALE,		"anty`ze`nskie",	TRUE,	FALSE,
      "nie mo`ze by`c u`zywane przez kobiety" },
    { ITEM_ANTI_NEUTER,		"antynijakie",		TRUE,	FALSE,
      "nie mo`ze by`c u`zywane przez postacie p`lci nijakiej" },
    { ITEM_INVERT_RACES,	"odwr_rasy",		TRUE,	TRUE,
      "odwraca znaczenie `la`ncucha antyrasowego" },
    { ITEM_INVERT_CLASSES,	"odwr_prof",		TRUE,	TRUE,
      "odwraca znaczenie `la`ncucha antyklasowego" },
    { ITEM_VANISH,		"znikaj`ace",		TRUE,	FALSE,
      "przedmiot zawsze znika przy wychodzeniu z gry" },
    { ITEM_REINFORCED,		"wzmocnione",		TRUE,	FALSE,
      "broni nie daje si`e wzmacnia`c (skrytob`ojcy)" },
    { ITEM_FLAME,		"gor`ace",		TRUE,	FALSE,
      "ostrze ognia" },
    { ITEM_CHAOS,		"chaotyczne",		TRUE,	FALSE,
      "ostrze chaosu" },
    { ITEM_ICY,			"zimne",		TRUE,	FALSE,
      "ostrze lodu" },
    { ITEM_MANA_LEECH,		"ss`ace_man`e",		TRUE,	FALSE,
      "bro`n ss`aca man`e z przeciwnika (grabie`zca paladyna)" },
    { ITEM_PLAYER_SHOP,		"sklep_gracza",		FALSE,	TRUE,
      "" },
    { 0,			NULL,			FALSE,	FALSE, "" }
};


char *extra_bit_name_pl( int *extra_flags, CHAR_DATA *ch )
{
    char *wyj = bitvector_field_to_name( extra_bit_values, extra_flags,
			( !ch || is_authorized( ch, "pstat" )
			      || is_authorized( ch, "pustaw" )
			      || is_authorized( ch, "pszukaj" ) ),
			FALSE );

    return !str_cmp( wyj, "(brak)" ) ? "`zadne" : wyj;
}


char *extra_bit_name_pl_dla_ludzi( int *extra_flags, CHAR_DATA *ch )
{
    char *wyj = bitvector_field_to_name( extra_bit_values, extra_flags,
			( !ch || is_authorized( ch, "pstat" )
			      || is_authorized( ch, "pustaw" )
			      || is_authorized( ch, "pszukaj" ) ),
			TRUE );

    return !str_cmp( wyj, "(brak)" ) ? "`zadne" : wyj;
}


bool extra_bit_avail( int *extra_flags )
{
    return bitvector_field_avail( extra_bit_values, extra_flags,
	ITEM_EXTRA_FLAGS_SIZE );
}


char *wear_flag_name_pl( int wear )
{
    static char buf[ 512 ];

    buf[ 0 ] = '\0';
    if ( wear & ITEM_TAKE	   ) strcat( buf, " inwentarz" );
    if ( wear & ITEM_WEAR_BODY     ) strcat( buf, " cia`lo" );
    if ( wear & ITEM_WEAR_FINGER   ) strcat( buf, " palce" );
    if ( wear & ITEM_WEAR_NECK     ) strcat( buf, " szyja" );
    if ( wear & ITEM_WEAR_HEAD     ) strcat( buf, " g`lowa" );
    if ( wear & ITEM_WEAR_LEGS     ) strcat( buf, " nogi" );
    if ( wear & ITEM_WEAR_FEET     ) strcat( buf, " stopy" );
    if ( wear & ITEM_WEAR_HANDS    ) strcat( buf, " d`lonie" );
    if ( wear & ITEM_WEAR_ARMS     ) strcat( buf, " ramiona" );
    if ( wear & ITEM_WEAR_SHIELD   ) strcat( buf, " tarcza" );
    if ( wear & ITEM_WEAR_ABOUT    ) strcat( buf, " owini`ete" );
    if ( wear & ITEM_WEAR_WAIST    ) strcat( buf, " pas" );
    if ( wear & ITEM_WEAR_WRIST    ) strcat( buf, " nadgarstki" );
    if ( wear & ITEM_WIELD         ) strcat( buf, " bro`n" );
    if ( wear & ITEM_HOLD          ) strcat( buf, " trzymane" );
    if ( wear & ITEM_WEAR_FACE     ) strcat( buf, " twarz" );

    return ( buf[ 0 ] != '\0' ) ? buf + 1 : "`zadne";
}


struct bit_field_data mob_act_flags[ ] =
{
    { ACT_IS_NPC,		"mob",			TRUE,	FALSE,
      "" },
    { ACT_SENTINEL,		"stacjonarny",		TRUE,	FALSE,
      "nie rusza si`e z miejsca" },
    { ACT_SCAVENGER,		"kolekcjoner",		TRUE,	FALSE,
      "podnosi przedmioty z ziemi" },
    { ACT_NO_HUNT,		"nie_polowalny",	TRUE,	FALSE,
      "nie da si`e na niego polowa`c" },
    { ACT_NO_TRACK,		"nie_poluje",		TRUE,	FALSE,
      "nie `sciga graczy, kt`orzy mu uciekaj`a (ma sens wy`l`acznie dla mob`ow ruchomych)" },
    { ACT_AGGRESSIVE,		"agresywny",		TRUE,	FALSE,
      "agresywny, atakuje inne postacie" },
    { ACT_STAY_AREA,		"jedna_kraina",		TRUE,	FALSE,
      "nie wyjdzie poza swoj`a krain`e" },
    { ACT_WIMPY,		"tch`orz",		TRUE,	FALSE,
      "ucieka, gdy ma ma`lo `zycia" },
    { ACT_PET,			"zwierz`atko",		FALSE,	FALSE,
      "" },
    { ACT_TRAIN,		"trenuje",		TRUE,	FALSE,
      "nie ustawia`c poza szko`lami/gildi`a - trener" },
    { ACT_PRACTICE,		"naucza",		TRUE,	FALSE,
      "nie ustawia`c poza szko`lami/gildi`a - nauczyciel" },
    { ACT_GAMBLE,		"hazardzista",		FALSE,	FALSE,
      "" },
    { ACT_PEACEFUL,		"pokojowy",		TRUE,	FALSE,
      "moba nie da si`e atakowa`c" },
    { ACT_NO_CHARM,		"nie_oczarowalny",	TRUE,	FALSE,
      "moba nie da si`e oczarowa`c" },
    { ACT_NO_GIFTS,		"nieprzekupny",		TRUE,	FALSE,
      "mob nie przyjmuje przedmiot`ow" },
    { ACT_DONT_COME_BACK,	"nie_wraca",		TRUE,	FALSE,
      "stacjonarny, kt`ory po opuszczeniu pomieszczenia nie pr`obuje do niego wraca`c" },
    { ACT_DOOM_PROOF,		"zag`ladoodporny",	TRUE,	FALSE,
      "odporny na dzia`lanie umiej`etno`sci zag`lada" },
    { ACT_MOVED,		"RUSZONY",		FALSE,	FALSE,
      "" },
    { ACT_NOT_RACIST,		"nie_rasista",		TRUE,	FALSE,
      "nie atakuje ras, kt`orych nie lubi" },
    { ACT_NIE_POMAGA,		"nie_pomaga",		TRUE,	FALSE,
      "tylko dla mob`ow w grupie - nie pomaga innym\n"
      "(to jemu inne moby z grupy pomagaj`a, chroni`ac go przed graczami)" },
    { 0,			NULL,			FALSE,	FALSE, "" },
};


char *act_flag_name_pl( int act, CHAR_DATA *ch )
{
    static char buf[ 512 ];

    buf[ 0 ] = '\0';

    if ( IS_SET( act, ACT_IS_NPC ) )
	return bit_field_to_name( mob_act_flags, act, FALSE );
    else
    {
	if ( IS_SET( act, PLR_BOUGHT_PET ) )	strcat( buf, " kupi`l_zwierz`e" );
	if ( IS_SET( act, PLR_SHOPKEEPER ) )	strcat( buf, " sklepikarz_gracz" );
	if ( IS_SET( act, PLR_SHOP_BREAK ) )	strcat( buf, " przerwa_sklepu" );
	if ( IS_SET( act, PLR_HOLYLIGHT ) )	strcat( buf, " nadwzrok" );
	if ( IS_SET( act, PLR_WIZINVIS ) )	strcat( buf, " boska_niew" );
	/* Lam 6.1.2004: zabezpieczenie przed uzyciem "mstat" w celu zdobycia
	   tych informacji */
	if ( is_authorized( ch, "fstat" ) )
	{
	    if ( IS_SET( act, PLR_SILENCE ) )	strcat( buf, " uciszony" );
	    if ( IS_SET( act, PLR_NO_EMOTE ) )	strcat( buf, " bez_emocji" );
	    if ( IS_SET( act, PLR_NO_TELL ) )	strcat( buf, " bez_mowy" );
	    if ( IS_SET( act, PLR_NO_COLOUR ) )	strcat( buf, " bez_koloru" );
	    if ( IS_SET( act, PLR_LOG ) )	strcat( buf, " log" );
	    if ( IS_SET( act, PLR_DENY ) )	strcat( buf, " wyrzucony?" );
	    if ( IS_SET( act, PLR_FREEZE ) )	strcat( buf, " mro`zonka" );
	    if ( IS_SET( act, PLR_LOVE ) )	strcat( buf, " mi`lo`s`c" );
	}
	if ( IS_SET( act, PLR_THIEF ) )		strcat( buf, " z`lodziej" );
	if ( IS_SET( act, PLR_KILLER ) )	strcat( buf, " morderca" );
	if ( IS_SET( act, PLR_ZABOJCA ) )	strcat( buf, " zab`ojca" );
	if ( IS_SET( act, PLR_WAMPIR_OD_URODZENIA ) )
						strcat( buf, " wampir_od_urodzenia" );
	if ( IS_SET( act, PLR_AFK ) )		strcat( buf, " ook" );
    }

    return ( buf[ 0 ] != '\0' ) ? buf + 1 : "(brak?)";
}


bool act_flag_avail( int act )
{
    return bit_field_avail( mob_act_flags, act );
}


struct bit_field_data room_flags_table[ ] =
{
    { ROOM_DARK,		"ciemny",		TRUE,	FALSE,
      "trzeba widzie`c w ciemno`sci albo mie`c latarni`e" },
    { ROOM_NO_HUNT,		"bez_polowania",	TRUE,	FALSE,
      "z tego pomieszczenia nie da si`e tropi`c" },
    { ROOM_NO_MOB,		"bez_moba",		TRUE,	FALSE,
      "do tego pomieszczenia nie wejdzie `zaden mob" },
    { ROOM_INDOORS,		"wn`etrze",		TRUE,	FALSE,
      "nie wida`c pogody, s`lo`nce nie wp`lywa na o`swietlenie" },
    { ROOM_UNDERGROUND,		"podziemie",		TRUE,	FALSE,
      "wampiry nie b`ed`a tu cierpie`c w dzie`n" },
    { ROOM_FOG,			"mg`la",		TRUE,	FALSE,
      "nie da si`e skanowa`c do i przez to miejsce" },
    { ROOM_INVERT_RACES,	"odwr_ras",		TRUE,	FALSE,
      "odwr`ocenie znaczenia `la`ncucha antyrasowego" },
    { ROOM_INVERT_CLASSES,	"odwr_prof",		TRUE,	FALSE,
      "odwr`ocenie znaczenia `la`ncucha antyklasowego" },
    { ROOM_SAFE,		"bezpieczny",		TRUE,	FALSE,
      "nie mo`zna si`e tu bi`c, nie mo`zna st`ad przywo`lywa`c" },
    { ROOM_PET_SHOP,		"sklep_zwierz",		TRUE,	FALSE,
      "sklep ze zwierz`etami" },
    { ROOM_NO_RECALL,		"bez_przywo`lania",	TRUE,	FALSE,
      "nie da si`e przywo`la`c do domu (/)" },
    { ROOM_CONE_OF_SILENCE,	"strefa_ciszy",		TRUE,	FALSE,
      "miejsce pod dzia`laniem czaru \"strefa ciszy\"" },
    { ROOM_CLAN_HP,		"klan_`zycie",		TRUE,	FALSE,
      "" },
    { ROOM_CLAN_MANA,		"klan_mana",		TRUE,	FALSE,
      "" },
    { ROOM_NOTE_READ,		"czytanie_not",		TRUE,	FALSE,
      "" },
    { ROOM_PURGATORY,		"czy`sciec",		TRUE,	FALSE,
      "" },
    { ROOM_NO_TELEPORT,		"nie_dla_teleportu",	TRUE,	FALSE,
      "nie da si`e w to miejsce wteleportowa`c" },
    { ROOM_REFRESH,		"od`swie`zenie",	TRUE,	FALSE,
      "punkty ruchu b`ed`a si`e tu szybciej odnawia`c" },
    { ROOM_NO_SUMMON,		"bez_przyzywania",	TRUE,	FALSE,
      "nie da si`e przywo`lywa`c/przechodzi`c" },
    { 0,			NULL,			FALSE,	FALSE,
      "" }
};


char *room_flag_name_pl( int room )
{
    return bit_field_to_name( room_flags_table, room, FALSE );
}


struct bit_field_data exit_info_flags[ ] =
{
    { EX_ISDOOR,		"drzwi",		TRUE,	FALSE,
      "" },
    { EX_CLOSED,		"zamkni`ete",		TRUE,	FALSE,
      "" },
    { EX_LOCKED,		"zakluczone",		TRUE,	FALSE,
      "" },
    { EX_BASHED,		"wywa`zone",		TRUE,	FALSE,
      "" },
    { EX_BASHPROOF,		"niewywa`zalne",	TRUE,	FALSE,
      "odporne na \"wywa`z\"" },
    { EX_PICKPROOF,		"nie_do_z`lamania",	TRUE,	FALSE,
      "odporne na \"w`lam\"" },
    { EX_PASSPROOF,		"nieprzenikalne",	TRUE,	FALSE,
      "odporne na przenikanie" },
    { EX_DIG,			"odkopywalne",		TRUE,	FALSE,
      "" },
    { EX_BURIED,		"zakopane",		TRUE,	FALSE,
      "mo`zna zakopa`c resetem D" },
    { EX_DONTRESET,		"nie_odnawiane",	TRUE,	FALSE,
      "nie wymaga resetu D, je`sli jest w innej krainie" },
    { EX_BEZKLAMKI,		"bez_klamki",		TRUE,	FALSE,
      "z tej strony nie mo`zna otworzy`c" },
    { EX_BEZZAMKA,		"bez_zamka",		TRUE,	FALSE,
      "z tej strony nie mo`zna odkluczy`c" },
    { 0,			NULL,			FALSE,	FALSE, "" },
};

char *exit_info_name_pl( int flag )
{
    return bit_field_to_name( exit_info_flags, flag, FALSE );
}


/* Lam 4.1.2003: dopoki to bylo uzywane przez polecenia dla niesmiertelnych,
   nie bylo az takiego problemu, ale odkad pojawilo sie w poleceniu "czas",
   moglo wywalic muda, jesli ktos rozkazal to oczarowanemu mobowi */
CHAR_DATA *get_char( CHAR_DATA *ch )
{
    if ( !ch->pcdata && ch->desc )
	return ch->desc->original;

    return ch;
}


bool longstring( CHAR_DATA *ch, char *argument )
{
    if ( strlen_pl( argument ) > 60 )
    {
	send_to_char( "Nie wi`ecej ni`z 60 znak`ow jest dopuszczalne.\n\r", ch );
	return TRUE;
    }
    else
	return FALSE;
}


bool authorized( CHAR_DATA *ch, char *skllnm )
{
    char buf[ MAX_STRING_LENGTH ];

    if ( !is_authorized( ch, skllnm ) )
    {
	sprintf( buf, "Nie jeste`s uprawnion%c do u`zywania polecenia \"%s\".\n\r",
		ch->sex == 1 ? 'y' : ch->sex == 2 ? 'a' : 'e', skllnm );
	send_to_char( buf, ch );
	return FALSE;
    }

    return TRUE;
}


bool is_authorized( CHAR_DATA *ch, char *skllnm )
{
    if ( ( !IS_NPC( ch ) && !is_name2( skllnm, ch->pcdata->immskll ) )
      || IS_NPC( ch ) )
    {
	return FALSE;
    }

    return TRUE;
}


/*
 * Lam 19-20.3.98: funkcja zapisuje zawartosc czyscca, wczesniej usuwajac
 * z niego wszystko, co nie jest cialem postaci gracza.
 */
void zapisz_ciala( void )
{
    ROOM_INDEX_DATA *room = get_room_index( ROOM_VNUM_GRAVEYARD_A );
    OBJ_DATA *obj, *next_obj;
    FILE *fp;

    if ( !room )
    {
	bug( "zapisz_ciala: nie ma czyscca!", 0 );
	return;
    }
    /* usuwam z pomieszczenia wszystkie przedmioty nie bedace cialami */
    for ( obj = room->contents; obj; obj = next_obj )
    {
	next_obj = obj->next_content;
	if ( !obj->deleted && obj->item_type != ITEM_CORPSE_PC )
	    extract_obj( obj );
    }
    if ( !room->contents ) /* jesli nie ma cial */
	return;
    zamknij_reserve( fpReserve );
    if ( !( fp = fopen( CORPSE_FILE, "w" ) ) )
	lac_perror( CORPSE_FILE );
    else
    {
	fwrite_obj( NULL, room->contents, fp, 0, FALSE );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


void czytaj_ciala( void )
{
    ROOM_INDEX_DATA *room = get_room_index( ROOM_VNUM_GRAVEYARD_A );
    FILE *fp;

    if ( !room )
    {
	bug( "czytaj_ciala: nie ma czyscca!", 0 );
	return;
    }
    zamknij_reserve( fpReserve );
    if ( !( fp = fopen( CORPSE_FILE, "r" ) ) )
    {
	if ( errno != ENOENT ) /* brak pliku moze sie zdarzyc */
	    lac_perror( CORPSE_FILE );
    }
    else
    {
	int status;
	char *klucz;
	char litera;

	for ( ; !feof( fp ); )
	{
	    litera = fread_letter( fp );
	    if ( litera != '#' )
	    {
		if ( !feof( fp ) )
		    bug( "czytaj_ciala: brak #", 0 );
		break;
	    }
	    klucz = fread_word( fp, &status, TRUE );
	    if ( str_cmp( klucz, "OBJECT" ) )
	    {
		bug( "czytaj_ciala: brak #OBJECT", 0 );
		break;
	    }
	    fread_obj( room, fp, 3 );
	}
	fclose( fp );
	if ( remove( CORPSE_FILE ) )
	    lac_perror( CORPSE_FILE );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


/*
 * Lam 16.3.98: wybralem te funkcje do zapisywania urn klanowych, bo jest
 * wywolywana przez restarty manualne i automatyczne.
 * Lam 19.3.98: przy kazdym restarcie zapisywane sa wszystkie ciala z czyscca
 * Lam 26.12.1999: zapisz_ustawienia( )
 */
void end_of_game( void )
{
    CHAR_DATA       *ch, *ch_next;
    DESCRIPTOR_DATA *d;
    DESCRIPTOR_DATA *d_next;

    /* Ulryk: taka kolejnosc jest konieczna. Jesli trafimy na postac, ktora nie
       ma prawa sie zapisac (restart, kasowanie, zmiana hasla), to
       close_socket( ) usunie ja z char_list i kolejna petla juz na nia nie
       trafi. */

    for ( d = descriptor_list; d; d = d_next )
    {
	d_next = d->next;
	if ( d->connected == CON_PLAYING )
	{
	    if ( !can_quit_now( d->character, TRUE ) )
	    {
		do_save( d->character, "" );
		STC( "Zapraszamy po przerwie.\n\r", d->character );
		close_socket( d, TRUE );
	    }
	    else
		do_quit( d->character, "zupe`lnie" );
	}
	else
	{
	    write_to_buffer( d, "\n\rPrzykro mi, ale wlasnie restartujemy gre, sprobuj polaczyc sie za chwile.\n\r", 0 );
	    close_socket( d, TRUE );
	}
    }

    for ( ch = char_list; ch; ch = ch_next )
    {
	ch_next = ch->next;

	if ( ch->deleted
	  || IS_NPC( ch ) )
	{
	    continue;
	}

	if ( !can_quit_now( ch, TRUE ) )
	{
	    do_save( ch, "" );
	    STC( "Zapraszamy po przerwie.\n\r", ch );
	}
	else
	    do_quit( ch, "zupe`lnie" );
    }

    write_clans( );
    zapisz_urny( );
    zapisz_ciala( );
    rewrite_all_notes( );
    zapisz_ustawienia( );
    /* zapisz_imiona( ); */
    bug( "***czyszczenie powtorek***", 0 );

    return;
}


int material_lookup( const char *material )
{
    int index;

    for ( index = 0; index < MAX_MATERIAL; index++ )
	if ( !str_cmp( material, material_table[ index ].name ) )
	    return index;

    return -1; /* do 10.2.2006 zwracalo 0 (nic) */
}


int material_lookup_short( const char *material )
{
    int index;

    for ( index = 0; index < MAX_MATERIAL; index++ )
	if ( !str_prefix( material, material_table[ index ].name ) )
	    return index;

    return -1;
}


int liquid_lookup( const char *liquid )
{
    int index;

    for ( index = 0; index < LIQ_MAX; index++ )
	if ( !str_cmp( liquid, liq_table_pl[ index ].liq_name ) )
	    return index;

    return -1;
}


/*
 * Nowa wersja race_lookup uwzglednia polska i angielska nazwe.
 * Lam 22.6.2004: najpierw szuka pelnych nazw, potem skrotow. Dziala wolniej
 * tylko wywolywane z niani lub przy bledzie w krainie.
 */
int race_lookup( const char *race )
{
    int index;

    for ( index = 0; index < MAX_RACE; index++ )
	if ( !str_cmp( race, race_table[ index ].who_name )
	  || !str_cmp( race, race_table[ index ].ang ) )
	{
	    return index;
	}

    for ( index = 0; index < MAX_RACE; index++ )
	if ( !str_prefix( race, race_table[ index ].who_name )
	  || !str_prefix( race, race_table[ index ].ang ) )
	{
	    return index;
	}

    return -1;
}


/*
 * Lookup a skill by name.
 */
int skill_lookup_ang( const char *name )
{
    int sn;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( !skill_table[ sn ].old_name )
	    break;
	if ( !str_cmp( name, skill_table[ sn ].old_name )
	  || !str_cmp( name, skill_table[ sn ].pl_name )
	  || !str_cmp( name, skill_table[ sn ].short_name ) )
	{
	    return sn;
	}
    }

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( !skill_table[ sn ].old_name )
	    break;
	if ( !str_prefix( name, skill_table[ sn ].old_name )
	  || !str_prefix( name, skill_table[ sn ].pl_name )
	  || !str_prefix( name, skill_table[ sn ].short_name ) )
	{
	    return sn;
	}
    }

    return -1;
}


int skill_lookup_pl( const char *name )
{
    int sn;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( !skill_table[ sn ].pl_name )
	    break;
	if ( !str_cmp( name, skill_table[ sn ].pl_name )
	  || !str_cmp( name, skill_table[ sn ].short_name ) )
	    return sn;
    }

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( !skill_table[ sn ].pl_name )
	    break;
	if ( !str_prefix( name, skill_table[ sn ].pl_name )
	  || !str_prefix( name, skill_table[ sn ].short_name ) )
	{
	    return sn;
	}
    }

    return -1;
}


/*
 * Funkcja uzywana TYLKO przy czytaniu wplywow z pliku postaci (save.c), stad
 * angielskie nazwy. Powinna zawsze zwracac ten sam wynik, co skill_lookup_ang.
 */
int affect_lookup( const char *affectname )
{
    int index;

    for ( index = 0; index < MAX_SKILL; index++ )
	if ( !str_cmp( affectname, skill_table[ index ].old_name ) )
	    return index;

    return -1;
}


/*
 * Rzut na ochrone przed czarem.
 * Lam: poprawki obliczen
 */
bool saves_spell( int level, CHAR_DATA *victim )
{
    int save;

    save = ( victim->level - level ) * 4 + LICZ_OPC( victim );
    save = URANGE( 5, save, 95 );

    return number_percent( ) < save;
}


/*
 * Lam 16.12.2004 do mstat
 */
int zloto_w_pojemnikach( OBJ_DATA *lista )
{
    OBJ_DATA *obj, *obj_next;
    int zloto = 0;

    for ( obj = lista; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted )
	    continue;

	if ( obj->contains )
	    zloto += zloto_w_pojemnikach( obj->contains );

	if ( obj->item_type == ITEM_MONEY )
	    zloto += obj->value[ 0 ].v;
    }

    return zloto;
}


/*
 * Lam 1.12.2004: znikanie kluczy podczas wyjscia z postaci.
 * Lam 28.4.2006: szukanie takze w pojemnikach (zauwazyl Civril), informacja
 * dla graczy, ktorzy nie czytali ?wyjdz/?zapisz.
 */
void wywal_smieci_z_listy( OBJ_DATA *lista, int *klucze, int *drogowskazy )
{
    OBJ_DATA *obj, *obj_next;

    for ( obj = lista; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted )
	    continue;

	if ( obj->contains )
	    wywal_smieci_z_listy( obj->contains, klucze, drogowskazy );

	if ( !IS_OBJ_STAT( obj, ITEM_QUEST )
	  && ( obj->item_type == ITEM_KEY
	    || obj->item_type == ITEM_BEACON
	    || IS_OBJ_STAT( obj, ITEM_VANISH ) ) )
	{
	    if ( obj->item_type == ITEM_KEY )
		( *klucze )++;
	    if ( obj->item_type == ITEM_BEACON )
		( *drogowskazy )++;
	    /* znikajace przedmioty znikaja same, nie pozbywa sie ich postac */
	    extract_obj( obj );
	}
    }

    return;
}


void wywal_smieci( CHAR_DATA *ch )
{
    int klucze, drogowskazy;
    char buf[ MIL ];

    klucze = drogowskazy = 0;
    wywal_smieci_z_listy( ch->carrying, &klucze, &drogowskazy );

    if ( klucze == 0 && drogowskazy == 0 )
	return;

    strcpy( buf, "Pozbywasz si`e " );
    if ( klucze )
    {
	if ( klucze == 1 )
	    strcat( buf, "zb`ednego klucza" );
	else
	    strcat( buf, "zb`ednych kluczy" );

	if ( drogowskazy == 1 )
	    strcat( buf, " i drogowskazu" );
	else if ( drogowskazy > 1 )
	    strcat( buf, " i drogowskaz`ow" );

	strcat( buf, ".\n\r" );
    }
    else
    {
	if ( drogowskazy == 1 )
	    strcat( buf, "zb`ednego drogowskazu.\n\r" );
	else
	    strcat( buf, "zb`ednych drogowskaz`ow.\n\r" );
    }

    STC( buf, ch );

    return;
}


/*
 * Lam 11.3.98
 * Lam 10.4.2004: ukrywanie niesmiertelnych z BN > 102 (bo rekord mozna miec na 102)
 */
int licz_graczy( void )
{
    DESCRIPTOR_DATA *desc;
    CHAR_DATA *ch;
    int ile = 0;

    for ( desc = descriptor_list; desc; desc = desc->next )
	if ( desc->connected == CON_PLAYING )
	{
	    ch = ( desc->original ) ? desc->original : desc->character;

	    if ( !IS_NPC( ch )
	      && ( !IS_SET( ch->act, PLR_WIZINVIS )
		|| ch->pcdata->wizinvis < 103 ) )
	    {
		ile++;
	    }
	}

    return ile;
}


/*
 * Lam 11.3.98
 */
int licz_polaczenia( void )
{
    DESCRIPTOR_DATA *desc;
    CHAR_DATA *ch;
    int ile = 0;

    for ( desc = descriptor_list; desc; desc = desc->next )
	if ( desc->connected != CON_GET_NAME )
	{
	    if ( desc->original )
		ch = desc->original;
	    else
		ch = desc->character;

	    if ( !ch /* nie powinno miec miejsca */
	      || IS_NPC( ch )
	      || ( !IS_SET( ch->act, PLR_WIZINVIS )
		|| ch->pcdata->wizinvis < 103 ) )
	    {
		ile++;
	    }
	}

    return ile;
}


void zapisz_ustawienia( void )
{
    FILE *fp;
    int wyn;

    zamknij_reserve( fpReserve );
    if ( !( fp = fopen ( NSYSTEM_FILE, "w" ) ) )
	lac_perror( NSYSTEM_FILE );
    else
    {
	fprintf( fp, "Gracze      %d  %s~\n", max_play_all_time, max_play_all_time_time );
	fprintf( fp, "Polaczenia  %d  %s~\n", max_desc_all_time, max_desc_all_time_time );
	fprintf( fp, "Blokada     %d\n", numlock );
	/* ostatni fprintf poda blad, wczesniejsze sa niewazne */
	wyn = fprintf( fp, "Wiek        %ld\n", mud_age );
	fclose( fp );

	/* Jesli zapisal bez bledu, przewalimy dane, dzieki temu nie
	   stracimy wieku muda w przypadku np. zapchanego dysku.
	   FIXME? MinGW32 chyba wymaga unlink( SYSTEM_FILE ) */
	if ( wyn >= 0
	  && rename( NSYSTEM_FILE, SYSTEM_FILE ) < 0 )
	{
	    lac_perror( "rename " NSYSTEM_FILE );
	}
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


void czytaj_ustawienia( void )
{
    FILE *fp;
    int stat;
    char *plum;

    zamknij_reserve( fpReserve );
    if ( !( fp = fopen( SYSTEM_FILE, "r" ) ) )
	lac_perror( SYSTEM_FILE );
    else
    {
	if ( str_cmp( fread_word( fp, &stat, TRUE ), "Gracze" ) )
	{
	    bug( "Nie znalazlem graczy w pliku system.txt", 0 );
	    fclose( fp );
	    fpReserve = fopen( NULL_FILE, "r" );
	    return;
	}
	max_play_all_time = fread_number( fp, &stat );
	plum = fread_string( fp, &stat );
	strcpy( max_play_all_time_time, plum );
	free_string( plum );
	if ( str_cmp( fread_word( fp, &stat, TRUE ), "Polaczenia" ) )
	{
	    bug( "Nie znalazlem polaczen w pliku system.txt", 0 );
	    fclose( fp );
	    fpReserve = fopen( NULL_FILE, "r" );
	    return;
	}
	max_desc_all_time = fread_number( fp, &stat );
	plum = fread_string( fp, &stat );
	strcpy( max_desc_all_time_time, plum );
	free_string( plum );
	if ( str_cmp( fread_word( fp, &stat, TRUE ), "Blokada" ) )
	{
	    bug( "Nie znalazlem polaczen w pliku system.txt", 0 );
	    fclose( fp );
	    fpReserve = fopen( NULL_FILE, "r" );
	    return;
	}
	numlock = fread_number( fp, &stat );
	if ( str_cmp( fread_word( fp, &stat, TRUE ), "Wiek" ) )
	{
	    bug( "Nie znalazlem wieku muda w pliku system.txt", 0 );
	    fclose( fp );
	    fpReserve = fopen( NULL_FILE, "r" );
	    return;
	}
	mud_age = fread_number( fp, &stat );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


void czy_rekord_graczy( void )
{
    int ile;
    char buf[ MAX_STRING_LENGTH ];

    ile = licz_graczy( );

    if ( ile > max_play.day[ 0 ] )
	max_play.day[ 0 ] = ile;

    if ( ile > max_play.total )
	max_play.total = ile;

    if ( ile > max_play_all_time )
    {
	sprintf( buf, "[!!!] Nowy rekord ilosci graczy: %d", ile );
	log_string( buf );
	wiznet( buf, NULL, NULL, NULL, WIZ_DEBUG, 0, 106 );
	max_play_all_time = ile;
	strcpy( max_play_all_time_time, asctime_pl( localtime( &current_time ) ) );
	zapisz_ustawienia( );
    }

    return;
}


void czy_rekord_polaczen( void )
{
    int ile;
    char buf[ MAX_STRING_LENGTH ];

    ile = licz_polaczenia( );

    if ( ile > max_desc.day[ 0 ] )
	max_desc.day[ 0 ] = ile;

    if ( ile > max_desc.total )
	max_desc.total = ile;

    if ( ile > max_desc_all_time )
    {
	sprintf( buf, "[!!!] Nowy rekord ilosci polaczen: %d", ile );
	log_string( buf );
	wiznet( buf, NULL, NULL, NULL, WIZ_DEBUG, 0, 106 );
	max_desc_all_time = ile;
	strcpy( max_desc_all_time_time, asctime_pl( localtime( &current_time ) ) );
	zapisz_ustawienia( );
    }

    return;
}


void sprawdz_wampira( CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) )
	return;

    /* tym sposobem nawet wampir czystej krwi przemieniony 3 lata temu
       i wczytany jako nie-wampir bez zapamietanej mocy nie wywali muda
       po zakonczeniu przemiany */
    if ( ch->pcdata->wampiryzm.moc == -1 )
    {
	if ( ch->pcdata->wampiryzm.pochodzenie
	  && *ch->pcdata->wampiryzm.pochodzenie )
	{
	    ch->pcdata->wampiryzm.moc = 1;
	}
	else
	    ch->pcdata->wampiryzm.moc = 0;
    }

    if ( CZY_WAMPIR( ch ) )
    {
	ch->alignment = UMIN( ch->alignment, 0 );
	if ( ch->pcdata->wampiryzm.ile == 0 )
	    ch->pcdata->wampiryzm.ile = 1;
    }

    return;
}


void zmien_wlasciciela( OBJ_DATA *obj, char *stare_imie, char *imie )
{
    OBJ_DATA *p;

    if ( IS_OBJ_STAT( obj, ITEM_PRIVATE ) && !str_cmp( stare_imie, obj->owner ) )
    {
	free_string( obj->owner );
	obj->owner = str_dup( imie );
    }

    if ( obj->item_type == ITEM_CONTAINER
      || obj->item_type == ITEM_CORPSE_NPC )
    {
	for ( p = obj->contains; p; p = p->next_content )
	    zmien_wlasciciela( p, stare_imie, imie );
    }

    return;
}


/*
 * Wydzielone przez Lama, zmiany wlascicieli i klanow napisal Ulryk.
 */
void zmien_wlasciciela_wszedzie( CHAR_DATA *victim, char *new_name )
{
    OBJ_DATA *obj;
    CLAN_MEMBER_DATA *member;

    for ( obj = victim->carrying; obj; obj = obj->next_content )
	zmien_wlasciciela( obj, victim->name, zwielkoliteruj( new_name ) );

    for ( obj = victim->przechowalnia; obj; obj = obj->next_content )
	zmien_wlasciciela( obj, victim->name, zwielkoliteruj( new_name ) );

    if ( victim->pcdata->clan )
	for ( member = victim->pcdata->clan->member_list;
	      member;
	      member = member->next )
	{
	    if ( !str_cmp( member->name, victim->name ) )
	    {
		free_string( member->name );
		member->name = str_dup( capitalize( new_name ) );
		break;
	    }
	}

    return;
}


void zmien_imie_postaci( CHAR_DATA *victim, char *new_name )
{
    free_string( victim->name );
    victim->name = str_dup( capitalize( new_name ) );

    free_string( victim->short_descr );
    victim->short_descr = str_dup( zwielkoliteruj( new_name ) );

    free_string( victim->dopelniacz );
    victim->dopelniacz = str_dup( zwielkoliteruj( new_name ) );

    free_string( victim->celownik );
    victim->celownik = str_dup( zwielkoliteruj( new_name ) );

    free_string( victim->biernik );
    victim->biernik = str_dup( zwielkoliteruj( new_name ) );

    free_string( victim->narzednik );
    victim->narzednik = str_dup( zwielkoliteruj( new_name ) );

    free_string( victim->miejscownik );
    victim->miejscownik = str_dup( zwielkoliteruj( new_name ) );

    free_string( victim->wolacz );
    victim->wolacz = str_dup( zwielkoliteruj( new_name ) );

    /* powinienem jeszcze usuwac long_descr, bo tez moze zawierac stare imie */

    return;
}


bool czy_zajete_imie( char *imie )
{
    CHAR_DATA       *ch;
    DESCRIPTOR_DATA *d;
    FILE            *fp;
    char             buf[ MAX_STRING_LENGTH ];

    /* czy jest taki plik postaci? */
#if !defined( macintosh ) && !defined( MSDOS )
    sprintf( buf, PLAYER_DIR "%s" DIR_SEPARATOR "%s",
	initial( imie ), capitalize( imie ) );
#else
    sprintf( buf, "%s%s", PLAYER_DIR, capitalize( imie ) );
#endif
    zamknij_reserve( fpReserve );
    if ( ( fp = fopen( buf, "r" ) ) )
    {
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
	return TRUE;
    }

    strcat( buf, ".gz" );
    if ( ( fp = fopen( buf, "r" ) ) )
    {
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
	return TRUE;
    }

    fpReserve = fopen( NULL_FILE, "r" );

    /* czy istnieje niezapisana postac o takim imieniu? */
    for ( ch = char_list; ch; ch = ch->next )
	if ( !IS_NPC( ch ) && ( !str_cmp( ch->name, imie ) ) )
	    return TRUE;

    for ( d = descriptor_list; d; d = d->next )
	if ( d->character && ( !str_cmp( d->character->name, imie ) ) )
	    return TRUE;

    return FALSE;
}


int wiek_krainy( AREA_DATA *pArea )
{
    int wiek;

    wiek = pArea->time_info.year - 1;
    wiek *= 17;
    wiek += pArea->time_info.month;
    wiek *= 35;
    wiek += pArea->time_info.day;
    wiek *= 24;
    wiek += pArea->time_info.hour;

    return wiek;
}


void popraw_czas_krainy( AREA_DATA *pArea )
{
    if ( pArea->time_info.hour < 0 )
    {
	pArea->time_info.day--;
	pArea->time_info.hour += 24;
    }
    if ( pArea->time_info.day < 0 )
    {
	pArea->time_info.month--;
	pArea->time_info.day += 35;
    }
    if ( pArea->time_info.month < 0 )
    {
	pArea->time_info.year--;
	pArea->time_info.month += 17;
    }
    if ( pArea->time_info.hour >= 24 )
    {
	pArea->time_info.day++;
	pArea->time_info.hour -= 24;
    }
    if ( pArea->time_info.day >= 35 )
    {
	pArea->time_info.month++;
	pArea->time_info.day -= 35;
    }
    if ( pArea->time_info.month >= 17 )
    {
	pArea->time_info.year++;
	pArea->time_info.month -= 17;
    }

    return;
}


void sprawdz_restrykcje( CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act, PLR_LOVE ) && ch->pcdata->love.do_kiedy
      && ch->pcdata->love.do_kiedy < current_time )
    {
	REMOVE_BIT( ch->act, PLR_LOVE );
	ch->pcdata->love.do_kiedy = 0;
	free_string( ch->pcdata->love.od );
	ch->pcdata->love.od = str_dup( "" );
    }

    if ( IS_SET( ch->act, PLR_SILENCE ) && ch->pcdata->silence.do_kiedy
      && ch->pcdata->silence.do_kiedy < current_time )
    {
	REMOVE_BIT( ch->act, PLR_SILENCE );
	ch->pcdata->silence.do_kiedy = 0;
	free_string( ch->pcdata->silence.od );
	ch->pcdata->silence.od = str_dup( "" );
	send_to_char( "Znowu mo`zesz u`zywa`c kana`l`ow.\n\r", ch );
    }

    if ( IS_SET( ch->act, PLR_NO_EMOTE ) && ch->pcdata->noemote.do_kiedy
      && ch->pcdata->noemote.do_kiedy < current_time )
    {
	REMOVE_BIT( ch->act, PLR_NO_EMOTE );
	ch->pcdata->noemote.do_kiedy = 0;
	free_string( ch->pcdata->noemote.od );
	ch->pcdata->noemote.od = str_dup( "" );
	send_to_char( "Znowu mo`zesz okazywa`c swoje emocje.\n\r", ch );
    }

    if ( IS_SET( ch->act, PLR_NO_COLOUR ) && ch->pcdata->nocolour.do_kiedy
      && ch->pcdata->nocolour.do_kiedy < current_time )
    {
	REMOVE_BIT( ch->act, PLR_NO_COLOUR );
	ch->pcdata->nocolour.do_kiedy = 0;
	free_string( ch->pcdata->nocolour.od );
	ch->pcdata->nocolour.od = str_dup( "" );
	send_to_char( "Znowu mo`zesz u`zywa`c kolor`ow.\n\r", ch );
    }

    if ( IS_SET( ch->act, PLR_NO_TELL ) && ch->pcdata->notell.do_kiedy
      && ch->pcdata->notell.do_kiedy < current_time )
    {
	REMOVE_BIT( ch->act, PLR_NO_TELL );
	ch->pcdata->notell.do_kiedy = 0;
	free_string( ch->pcdata->notell.od );
	ch->pcdata->notell.od = str_dup( "" );
	send_to_char( "Zn`ow mo`zesz m`owi`c.\n\r", ch );
    }

    if ( IS_SET( ch->act, PLR_LOG ) && ch->pcdata->log.do_kiedy
      && ch->pcdata->log.do_kiedy < current_time )
    {
	REMOVE_BIT( ch->act, PLR_LOG );
	ch->pcdata->log.do_kiedy = 0;
	free_string( ch->pcdata->log.od );
	ch->pcdata->log.od = str_dup( "" );
	if ( ch->pcdata->fLog ) fclose( ch->pcdata->fLog );
	ch->pcdata->fLog = NULL;
    }

    if ( IS_SET( ch->act, PLR_FREEZE ) && ch->pcdata->freeze.do_kiedy
      && ch->pcdata->freeze.do_kiedy < current_time )
    {
	REMOVE_BIT( ch->act, PLR_FREEZE );
	ch->pcdata->freeze.do_kiedy = 0;
	free_string( ch->pcdata->freeze.od );
	ch->pcdata->freeze.od = str_dup( "" );
	send_to_char( "Znowu mo`zesz gra`c.\n\r", ch );
    }

    return;
}


/*
 * Ulryk 19.03.2007: specjalna wersja powyzszego, dla bitu DENY zwraca TRUE,
 * jesli zdjeto bit PLR_DENY.
 */
bool sprawdz_deny( CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) )
	return FALSE;

    if ( IS_SET( ch->act, PLR_DENY ) && ch->pcdata->deny.do_kiedy
      && ch->pcdata->deny.do_kiedy < current_time )
    {
	REMOVE_BIT( ch->act, PLR_DENY );
	ch->pcdata->deny.do_kiedy = 0;
	free_string( ch->pcdata->deny.od );
	ch->pcdata->deny.od = str_dup( "" );
	return TRUE;
    }

    return FALSE;
}


/*
 * Ulryk 18.03.2007: od teraz policz_czas( ) zamienia napisy postaci 7d8g10m na
 * znacznik czasu 7 dni, 8 godzin i 10 minut
 */
time_t policz_czas( char *arg )
{
    char *p;
    char  buf[ MIL ];
    int   i = 0;
    int   doliczane = 0;

    for ( p = arg; p && *p != '\0'; p++ )
	if ( *p >= '0' && *p <= '9' )
	    buf[ i++ ] = *p;
	else
	{
	    buf[ i ] = '\0';
	    switch ( *p )
	    {
		case 'd':
		    doliczane += 86400 * atoi( buf );
		    break;
		case 'g':
		case 'h':
		    doliczane += 3600 * atoi( buf );
		    break;
		case 'm':
		    doliczane += 60 * atoi( buf );
		    break;
		default:
		    return 0;
	    }

	    i = 0;
	}

    /* jesli i != 0, to znaczy, ze po liczbie nie bylo litery */
    if ( i )
	return 0;

    return current_time + doliczane;
}


bool czy_mozna_kopac( int sector_type, CHAR_DATA *ch )
{
    switch ( sector_type )
    {
	case SECT_CITY:
	case SECT_INSIDE:
	    if ( ch )
		send_to_char( "Pod`lo`ze jest zbyt twarde, nie da si`e tu kopa`c.\n\r", ch );
	    return FALSE;
	case SECT_WATER_SHALLOW:
	case SECT_WATER_DEEP:
	case SECT_UNDERWATER:
	    if ( ch )
		send_to_char( "Nie wiesz, jak przekopa`c wod`e.\n\r", ch );
	    return FALSE;
	case SECT_AIR:
	    if ( ch )
		send_to_char( "Chcesz przekopa`c powietrze?\n\r", ch );
	    return FALSE;
    }

    return TRUE;
}
