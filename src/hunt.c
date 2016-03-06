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
 *  Szukanie drogi przeszlo wielka rewolucje, jest prostsze i wydajne      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"
#include "const.h"
#if !defined( NO_WMEMSET )
# include <wchar.h>
#endif


void	mam_cie		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/*
    Improved track/find_path snippet by Palrich (Michael Barton, red@icuii.com)

    By Palrich
    Head Coder @ Divine Blood

    (nie obeszlo sie bez bardzo duzych zmian Lama)
*/

/* wersja dla tablicy calych komorek. Okazalo sie podczas testow, ze tablica
   z elementami bool (czyli char) dzialala wolniej niz te przesuniecia bitow,
   dopiero tablica intow byla 30% szybsza, ale za cene 32 razy wiekszego
   zuzycia pamieci, wiec przegrala
#define PATH_IS_FLAG( flag, bit ) ( flag[ bit ] )
#define PATH_SET_FLAG( flag, bit ) ( flag[ bit ] = 1 ) */
#define PATH_IS_FLAG( flag, bit )	IS_SET_V( flag, bit )
#define PATH_SET_FLAG( flag, bit )	SET_BIT_V( flag, bit )

int find_path( ROOM_INDEX_DATA *from, ROOM_INDEX_DATA *to, int max_depth,
			bool same_area, bool through_doors )
{
    static unsigned int bitvector[ MAX_VNUM / BITS_PER_INT ];
    ROOM_INDEX_DATA *rlist;
    ROOM_INDEX_DATA *track_room_list;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    int i, depth;
    /* rzutowanie najpierw w char *, a potem w wchar_t * pozwala pozbyc sie
       ostrzezen gcc z -fstrict-aliasing (czyli -Os i wyzsze) */
    char *bvwsk = (char *) &bitvector;

    /* bzero( bitvector, sizeof( bitvector ) ); */
#if defined( NO_WMEMSET )
    memset( bvwsk, 0, MAX_VNUM / BITS_PER_INT );
#else
    wmemset( (wchar_t *) bvwsk, 0, MAX_VNUM / BITS_PER_INT * SIZEOF_INT / SIZEOF_WCHAR_T );
#endif
    /* Vigud 5.3.2011: FIXME: vnumy powinny z definicji byc unsigned */
    PATH_SET_FLAG( bitvector, (unsigned int) from->vnum );
    track_room_list = from;
    track_room_list->next_track = NULL;
    for ( depth = 0; depth < max_depth; depth++ )
    {
	rlist = track_room_list;
	track_room_list = NULL;
	for ( ; rlist; rlist = rlist->next_track )
	    for ( i = 0; i < MAX_DIR; i++ )
	    {
		if ( !( pexit = rlist->exit[ i ] )
		  || ( !through_doors && IS_SET( pexit->exit_info, EX_CLOSED ) )
		  || !( to_room = pexit->to_room )
		  || ( same_area && from->area != to_room->area )
		  || PATH_IS_FLAG( bitvector, to_room->vnum ) )
		{
		    continue;
		}

		PATH_SET_FLAG( bitvector, (unsigned int) to_room->vnum );
		to_room->track_came_from = rlist;

		if ( to_room == to )
		{
		    /* tu widac cala droge i mozna ja sobie zapamietac */
		    if ( rlist == from )
			return i;
		    while ( rlist->track_came_from != from )
			rlist = rlist->track_came_from;
		    for ( i = 0; i < MAX_DIR; i++ )
			if ( from->exit[ i ] && from->exit[ i ]->to_room == rlist )
			    return i;
		    return -1;
		}
		else
		{
		    to_room->next_track = track_room_list;
		    track_room_list = to_room;
		}
	    }
    }

    return -1;
}


void do_hunt( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];
    char arg[ MAX_STRING_LENGTH ];
    CHAR_DATA *victim;
    int direction;
    bool fArea;
    bool uzycie;

    one_argument( argument, arg );

    if ( ch->hunting )
    {
	wywal_zwod( &ch->hunting->zwod_hunting, ch );
	ch->hunting = NULL;
    }
    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Na kogo chcesz zapolowa`c?\n\r", ch );
	return;
    }

    if ( ch->in_room->area == area_czysciec )
    {
	send_to_char( "No co`s ty, w niecielesnym `swiecie?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_HUNT ) )
    {
	send_to_char( "Nie mo`zesz tu znale`x`c niczyich `slad`ow.\n\r", ch );
	return;
    }

    /* only imps can hunt to different areas */
    fArea = ( get_trust( ch ) < L_SEN );

    if ( fArea )
	victim = get_char_area( ch, arg );
    else
	victim = get_char_world( ch, arg );

    if ( !victim )
    {
	send_to_char( "Nikogo takiego nie ma w pobli`zu.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Znajdujesz siebie!\n\r", ch );
	return;
    }

    if ( ch->in_room == victim->in_room )
    {
	act( ZM_ZAWSZE, "$N jest tutaj!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_NO_HUNT ) )
    {
	act( ZM_ZAWSZE, "Nie mo`zesz znale`x`c drogi do $!.", ch, NULL, victim, TO_CHAR );
	return;
    }

    /*
     * Deduct some movement.
     */
    if ( ch->move > 2 )
	ch->move -= 3;
    else
    {
	ASTC( "Jeste`s zbyt wyczerpan$y, aby szuka`c `slad`ow!", ch );
	return;
    }

    act( ZM_WZROK | ZM_WID_CHAR, "$n szuka `slad`ow na ziemi.", ch, NULL, NULL, TO_ROOM );
    WAIT_STATE( ch, skill_table[ gsn_hunt ].beats );
    direction = find_path( ch->in_room, victim->in_room,
			40000, fArea, TRUE );

    if ( direction == -1 )
    {
	/* Vigud 26.7.2010: jesli nie moglem widziec celu, $! bylo zamieniane na
	   "kogos", a to wygladalo dosc glupio, skoro wiem, na kogo poluje */
	/* act( ZM_ZAWSZE, "Nie mo`zesz st`ad znale`x`c drogi do $!.", ch, NULL, victim, TO_CHAR ); */
	sprintf( buf, "Nie mo`zesz st`ad znale`x`c drogi do %s.\n\r", victim->dopelniacz );
	STC( buf, ch );
	return;
    }

    if ( direction < 0 || direction > 9 )
    {
	send_to_char( "Wiesz, co`s tu jest nie tak.\n\r", ch );
	return;
    }

    /*
     * Give a random direction if the player misses the die roll.
     */
    uzycie = TRUE;
    if ( ( IS_NPC( ch ) && number_percent( ) < 25 )        /* NPC @ 25% */
      || ( !IS_NPC( ch ) && number_percent( ) >          /* PC @ norm */
		ch->pcdata->learned[ gsn_hunt ] ) )
    {
	do
	{
	    direction = number_door( );
	}
	while ( ch->in_room->exit[ direction ] == NULL
	     || ch->in_room->exit[ direction ]->to_room == NULL );
	uzycie = FALSE;
    }

    /*
     * Display the results of the search.
     */
    sprintf( buf, "Droga do $! prowadzi na %s.", kierunki[ direction ].biernik );
    act( ZM_ZAWSZE, buf, ch, NULL, victim, TO_CHAR );

    /* bez wywal_zwod, jest to robione wczesniej */
    ch->hunting = victim;
    dod_zwod( &victim->zwod_hunting, ch );
    if ( uzycie )
	uzyj( ch, gsn_hunt, "poszukiwaniu dr`og" );

    return;
}


/* Wersja wykonywana przy chodzeniu */
void auto_hunt( CHAR_DATA *ch )
{
    char buf[ MAX_STRING_LENGTH ];
    CHAR_DATA *victim;
    int direction;
    bool fArea;

    if ( IS_NPC( ch ) ) /* moby maja wlasna metode polowania */
      return;

    victim = ch->hunting;

    if ( !victim )
	return;

    if ( !can_see( ch, victim ) )
    {
	/* Vigud 26.7.2010 */
	/* act( ZM_ZAWSZE, "Nie mo`zesz st`ad znale`x`c drogi do $!. Przestajesz polowa`c na $#.",
	    ch, NULL, victim, TO_CHAR ); */
	sprintf( buf, "Nie mo`zesz st`ad znale`x`c drogi do %s. Przestajesz polowa`c na %s.\n\r",
	    victim->dopelniacz, victim->biernik );
	STC( buf, ch );
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
	return;
    }

    if ( ch->in_room == victim->in_room )
    {
	act( ZM_ZAWSZE, "{Y$N jest tutaj!{x", ch, NULL, victim, TO_CHAR );
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_HUNT ) )
    {
	send_to_char( "Nie mo`zesz tu znale`x`c niczyich `slad`ow.\n\r", ch );
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
	return;
    }

    /* only imps can hunt to different areas */
    fArea = ( skill_get_trust( ch ) < 106 );

    /*
     * Deduct some movement.
     */
    if ( ch->move > 2 )
	ch->move -= 3;
    else
    {
	ASTC( "Jeste`s zbyt wyczerpan$y, aby szuka`c `slad`ow!", ch );
	return;
    }

    act( ZM_WZROK | ZM_WID_CHAR, "$n szuka `slad`ow na ziemi.", ch, NULL, NULL, TO_ROOM );
    WAIT_STATE( ch, 3 );
    direction = find_path( ch->in_room, victim->in_room, 40000, fArea, TRUE );

    if ( direction == -1 )
    {
	/* Vigud 26.7.2010 */
	/* act( ZM_ZAWSZE, "Nie mo`zesz st`ad znale`x`c drogi do $!. Przestajesz polowa`c na $#.",
	    ch, NULL, victim, TO_CHAR ); */
	sprintf( buf, "Nie mo`zesz st`ad znale`x`c drogi do %s. Przestajesz polowa`c na %s.\n\r",
	    victim->dopelniacz, victim->biernik );
	STC( buf, ch );
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
	return;
    }

    if ( direction < 0 || direction > 9 )
    {
	send_to_char( "Co`s tu jest nie tak.\n\r", ch );
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
	return;
    }

    /*
     * Give a random direction if the player misses the die roll.
     */
    if ( ( IS_NPC( ch ) && number_percent( ) < 25 )        /* NPC @ 25% */
      || ( !IS_NPC( ch ) && number_percent( ) >          /* PC @ norm */
		  ch->pcdata->learned[ gsn_hunt ] ) )
    {
	do
	{
	    direction = number_door( );
	}
	while ( !ch->in_room->exit[ direction ]
	     || !ch->in_room->exit[ direction ]->to_room );
    }

    /*
     * Display the results of the search.
     */
    sprintf( buf, "{YDroga do $! prowadzi na %s.{x", kierunki[ direction ].biernik );
    act( ZM_ZAWSZE, buf, ch, NULL, victim, TO_CHAR );

    return;
}


/*
 * Lam 30.6.98 (16 komunikatow chyba wystarczy)
 * Rasy bez mowy niczego nie powiedza.
 */
void mam_cie( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !can_see( ch, victim ) )
    {
	if ( !number_bits( 3 ) )
	{
	    switch ( number_bits( 2 ) )
	    {
		default: do_say( ch, "Nie widz`e ci`e, ale wiem, `ze tu jeste`s! Poka`z si`e!" );
			break;
		case 1:  do_say( ch, "Wyczuwam twoj`a obecno`s`c! Poka`z si`e!" );
			break;
		case 2:  do_say( ch, "Gdzie si`e ukrywasz? Wy`la`x!" );
			break;
		case 3:  do_say( ch, "Tch`orzysz? Ukrywasz si`e? Walcz!" );
			break;
	    }
	}
	return;
    }

    if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
	if ( number_bits( 3 ) ) /* 1/8 szans ze cokolwiek powie */
	    return;

	switch ( number_bits( 2 ) )
	{
	    default: do_say( ch, "Wyjd`x st`ad, to porozmawiamy!" );
			break;
	    case 1:  do_say( ch, "Nie chowaj si`e tu! Tch`orzysz?" );
			break;
	    case 2:  do_say( ch, "Chcesz zgin`a`c? Za`latwmy to na zewn`atrz, co?" );
			break;
	    case 3:  do_say( ch, "Chod`x st`ad, a poka`z`e ci twoje miejsce na cmentarzu!" );
			break;
	}
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
    {
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
	return;
    }

    switch ( number_bits( 4 ) )
    {
	default: do_say( ch, "My`slisz, `ze przede mn`a uciekniesz, co?" );
		break;
	case 1:  do_say( ch, "Tym razem mi nie uciekniesz!" );
		break;
	case 2:  do_say( ch, "Jednak nie uda`lo ci si`e umkn`a`c! Gi`n!" );
		break;
	case 3:  do_say( ch, "Mam ci`e wreszcie! Krwi!" );
		break;
	case 4:  do_say( ch, "Nadesz`la chwila zemsty!" );
		break;
	case 5:  do_say( ch, "Teraz si`e policzymy!" );
		break;
	case 6:  do_say( ch, "Powiem grabarzowi, `ze ma nowego klienta, ciebie!" );
		break;
	case 7:  do_say( ch, "Czy masz ju`z miejsce na cmentarzu? B`edzie ci potrzebne!" );
		break;
	case 8:  do_say( ch, "Znowu si`e spotykamy! Haha!" );
		break;
	case 9:  do_say( ch, "Tym razem ci`e u`smierc`e!" );
		break;
	case 10: do_say( ch, "Przede mn`a nie ma ucieczki!" );
		break;
	case 11: do_say( ch, "Co mam ci wyry`c na nagrobku?" );
		break;
	case 12: do_say( ch, "Wypruj`e ci wn`etrzno`sci i nakarmi`e nimi pieska!" );
		break;
	case 13: do_say( ch, "Dobrze, `ze si`e spotykamy, s`epy czekaj`a na twoje zw`loki!" );
		break;
	case 14: do_say( ch, "Tw`oj m`ozg b`edzie dobrym nawozem!" );
		break;
	case 15: do_say( ch, "Wyd`lubi`e ci oczy i zrobi`e z nich amulet!" );
		break;
    }
    multi_hit( ch, victim, TYPE_UNDEFINED );

    return;
}


/*
 * Lam 16.5.98
 */
void hunt_victim( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *to_room;
    int direction;

    victim = ch->hunting;

    if ( victim == NULL )
    {
	bug( "hunt_victim: NULL victim", 0 );
	return;
    }

    if ( !ch->in_room || !victim->in_room )
    {
	bug( "hunt_victim: NULL ch->in_room or victim->in_room", 0 );
	return;
    }

    if ( ch->in_room == victim->in_room )
    {
	mam_cie( ch, victim );
	return;
    }

    if ( ch->in_room->area != victim->in_room->area )
    {
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
	return;
    }

    if ( ch->move > 1 )
	ch->move -= 2;
    else
	return;

    WAIT_STATE( ch, 3 );
    direction = find_path( ch->in_room, victim->in_room, 40000, TRUE, TRUE );

    if ( direction == -1 )
    {
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
	return;
    }

    if ( direction < 0 || direction > 9 )
    {
	wywal_zwod( &victim->zwod_hunting, ch );
	ch->hunting = NULL;
	return;
    }

    if ( number_percent( ) < 10 ) /* 10% mylnych tropow */
	do
	{
	    direction = number_door( );
	}
	while ( !ch->in_room->exit[ direction ]
	     || !ch->in_room->exit[ direction ]->to_room );

    to_room = ch->in_room->exit[ direction ]->to_room;

    /* Lam 30.6.98 - to sie rzadko zdarza, ale jednak */
    if ( ch->in_room->area != to_room->area
      && IS_SET( ch->act, ACT_STAY_AREA ) )
    {
	return;
    }

    if ( IS_SET( to_room->room_flags, ROOM_NO_MOB ) )
	return; /* Tissarius 5.10.2001 */

    move_char( ch, direction, FALSE );

    /* mogl wystapic entry_prog moba ch lub greet_prog moba/pomieszczenia */
    if ( !ch->deleted
      && ch->in_room == to_room
      && ch->in_room == victim->in_room
      && ch->position == POS_STANDING )
    {
	mam_cie( ch, victim );
    }

    return;
}
