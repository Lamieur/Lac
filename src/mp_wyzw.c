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
 *  Typy wyzwalaczy i obsluga uruchamiania roznych rodzajow progow.        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"
#include "mp_wyzw.h"
#include "comm.h"
#include "db.h"
#include <stdarg.h> /* va_list */


/* to konieczne niestety, zabezpieczenie podpatrzone w Smaugu */
#define MAX_PROG_NEST 10
int prog_nest;

/*
 * descr_prog (rprog) nie interpretuje polecen, w zamian dorzuca to, co by
 * normalnie zinterpretowal do statycznego bufora, ktory potem jest wysylany
 * graczowi jako opis pomieszczenia.
 * Od 7.1.2004 takze moby maja descr_proga.
 */
bool descr_prog_in_progress;
char descr_prog_buffer[ 2 * MSL ];

/*
 * Local function prototypes
 */
bool	mprog_do_ifchck		args( ( char* ifchck, CHAR_DATA* mob,
				       CHAR_DATA* actor, OBJ_DATA* obj,
				       CHAR_DATA* rndm, CHAR_DATA* vict,
				       OBJ_DATA* v_obj ) );
char	*mprog_process_if	args( ( char* ifchck, char* com_list,
				       CHAR_DATA* mob, CHAR_DATA* actor,
				       OBJ_DATA* obj, CHAR_DATA* rndm,
				       CHAR_DATA* vict, OBJ_DATA* v_obj ) );
char	*mprog_process_while	args( ( char* ifchck, char* com_list,
				       CHAR_DATA* mob, CHAR_DATA* actor,
				       OBJ_DATA* obj, CHAR_DATA* rndm,
				       CHAR_DATA* vict, OBJ_DATA* v_obj ) );
bool	mprog_translate		args( ( char *ch, char* t, CHAR_DATA* mob,
				       CHAR_DATA* actor, OBJ_DATA* obj,
				       CHAR_DATA* rndm, CHAR_DATA* vict,
				       OBJ_DATA* v_obj ) );
void	mprog_driver		args( ( char* com_list, CHAR_DATA* mob,
				       CHAR_DATA* actor, OBJ_DATA* obj,
				       CHAR_DATA* vict, OBJ_DATA* v_obj,
				       MPROG_DATA *mprogram, int gdzie ) );
CHAR_DATA *room_supermob	args( ( ROOM_INDEX_DATA *room ) );
CHAR_DATA *exit_supermob	args( ( ROOM_INDEX_DATA *room, int door ) );
CHAR_DATA *obj_supermob		args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *room ) );
void	dismiss_supermob	args( ( CHAR_DATA *supermob ) );
void	oprog_sacr_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	oprog_donate_trigger	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );

/***************************************************************************
 * Local function code and brief comments.
 */

/*
 * Used to get sequential lines of a multi line string (separated by "\n\r")
 * Thus its like one_argument(), but a trifle different. It is destructive
 * to the multi line string argument, and thus clist must not be shared.
 *
 * Lam 5.12.2003: while nie moze dzialac, jesli to niszczy clist; przepisuje
 */
char *mprog_next_command( char *clist, char *cmnd )
{
    char *p1 = clist;
    char *p2 = cmnd;

    while ( isspace( (unsigned char) *p1 ) )
	p1++; /* Lam: puste linie nie przerywaja proga */

    while ( *p1 != '\n' && *p1 != '\r' && *p1 != '\0' )
    {
	*p2 = *p1;
	p1++;
	p2++;
    }

    *p2 = '\0';

    while ( isspace( (unsigned char) *p1 ) )
	p1++; /* Lam 16.4.2000: zeby linie nie zaczynaly sie od enterow */

    return p1;
}


/*
 * Teraz instrukcja warunkowa jest obslugiwana wylacznie przez
 * oblicz_wyrazenie( ) i dziedziczy cala skladnie wyrazen MobC.
 */
bool mprog_do_ifchck( char *ifchck, CHAR_DATA *mob, CHAR_DATA *actor,
		     OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
		     OBJ_DATA *v_obj )
{
    int zwrot, wynik, typw;
    void *wynwsk;
    char txt[ MIL ];

    zwrot = oblicz_wyrazenie( mob, ifchck, actor, obj, rndm, vict, v_obj, &typw, &wynik, txt, &wynwsk );
    if ( zwrot )
    {
	if ( typw )
	    progbug( mob, "mprog_do_ifchk: nie obsluguje wynikow wyrazen innych niz calkowite" );
	else /* Lam 5.12.2003: wynik != 0 jest traktowany jako prawda */
	    zwrot = wynik ? TRUE : FALSE;
    }
    progdebug( mob, "Warunek: %s: %s\n\r", zwrot ? "{GPRAWDA{x" : "{BFA`LSZ{x", ifchck );

    /* potrzebne return -1 jesli blad */
    return zwrot;
}


/* Quite a long and arduous function, this guy handles the control
 * flow part of MOBprograms.  Basicially once the driver sees an
 * 'if' attention shifts to here.  While many syntax errors are
 * caught, some will still get through due to the handling of break
 * and errors in the same fashion.  The desire to break out of the
 * recursion without catastrophe in the event of a mis-parse was
 * believed to be high. Thus, if an error is found, it is bugged and
 * the parser acts as though a break were issued and just bails out
 * at that point. I havent tested all the possibilites, so I'm speaking
 * in theory, but it is 'guaranteed' to work on syntactically correct
 * MOBprograms, so if the mud crashes here, check the mob carefully!
 */
char *mprog_process_if( char *ifchck, char *com_list, CHAR_DATA *mob,
		       CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		       CHAR_DATA *vict, OBJ_DATA *v_obj ) /* bool nif? */
{
    static char null[ 1 ];
    char buf[ MAX_INPUT_LENGTH ];
    char *morebuf = NULL;
    char cmnd_buf[ MAX_INPUT_LENGTH ];
    char *cmnd = cmnd_buf;
    bool loopdone = FALSE;
    bool flag = FALSE;
    int  legal;
    int  ifnest = 0; /* licznik zagniezdzania */
    int  whilenest = 0;
    bool not = FALSE; /* Lam */
    char notbuf[ MIL ];

    one_argument( ifchck, notbuf );
    if ( !str_cmp( notbuf, "not" ) )
    {
	not = TRUE;
	ifchck = one_argument( ifchck, notbuf );
    }

    /* check for trueness of the ifcheck */
    if ( ( legal = mprog_do_ifchck( ifchck, mob, actor, obj, rndm, vict, v_obj ) ) )
    {
	if ( legal == 1 )
	    flag = TRUE;
	else
	    return null;
    }

    if ( not )
	flag = !flag;

    while ( !loopdone ) /* scan over any existing or statements */
    {
	cmnd     = cmnd_buf;
	com_list = mprog_next_command( com_list, cmnd );
	while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
	    cmnd++;
	if ( *cmnd == '\0' )
	{
	    progbug( mob, "no commands after IF/OR" );
	    return null;
	}
	morebuf = one_argument( cmnd, buf );
	if ( !str_cmp( buf, "or" ) )
	{
	    if ( flag )
		continue; /* Lam 15.4.2000, poprawka zlej metody z 22.10.1999 */

	    one_argument( morebuf, notbuf );
	    if ( !str_cmp( notbuf, "not" ) )
	    {
		not = TRUE;
		morebuf = one_argument( morebuf, notbuf );
	    }
	    else
		not = FALSE;

	    if ( ( legal = mprog_do_ifchck( morebuf, mob, actor, obj, rndm, vict, v_obj ) ) )
	    {
		if ( legal == 1 )	/* ifcheck wyszedl TRUE */
		{
		    if ( !not )
			flag = TRUE;
		}
		else			/* blad */
		    return null;
	    }
	    else			/* ifcheck wyszedl FALSE */
	    {
		if ( not )
		    flag = TRUE;
	    }
	}
	else
	    loopdone = TRUE;
    }

    if ( flag )
    {
	for ( ; ; ) /*ifcheck was true, do commands but ignore else to endif*/
	{
	    if ( !str_cmp( buf, "if" ) )
	    {
		com_list = mprog_process_if( morebuf, com_list, mob, actor, obj, rndm, vict, v_obj );
		while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		    cmnd++;
		if ( *com_list == '\0' )
		    return null;
		cmnd     = cmnd_buf;
		com_list = mprog_next_command( com_list, cmnd );
		morebuf  = one_argument( cmnd,buf );
		continue;
	    }
	    if ( !str_cmp( buf, "while" ) )
	    {
		com_list = mprog_process_while( morebuf, com_list, mob, actor, obj, rndm, vict, v_obj );
		while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		    cmnd++;
		if ( *com_list == '\0' )
		    return null;
		cmnd     = cmnd_buf;
		com_list = mprog_next_command( com_list, cmnd );
		morebuf  = one_argument( cmnd,buf );
		continue;
	    }
	    if ( !str_cmp( buf, "break" ) )
		return null;
	    if ( !str_cmp( buf, "endif" ) )
		return com_list;
	    if ( !str_cmp( buf, "endwhile" ) )
		progbug( mob, "unexpected endwhile inside of if body, trying to continue..." );
	    if ( !str_cmp( buf, "else" ) )
	    {
		ifnest = 0;
		whilenest = 0;
		while ( str_cmp( buf, "endif" ) || ifnest != 0 )
		{
		    if ( !str_cmp( buf, "if" ) )       ifnest++;
		    if ( !str_cmp( buf, "endif" ) )    ifnest--;
		    if ( !str_cmp( buf, "while" ) )    whilenest++;
		    if ( !str_cmp( buf, "endwhile" ) ) whilenest--;
		    cmnd     = cmnd_buf;
		    com_list = mprog_next_command( com_list, cmnd );
		    while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
			cmnd++;
		    if ( *cmnd == '\0' )
		    {
			progbug( mob, "missing endif after else" );
			return null;
		    }
		    one_argument( cmnd,buf );
		    if ( !str_cmp( buf, "else" ) && !ifnest )
			progbug( mob, "double else?" );
		}
		if ( whilenest )
		    progbug( mob, "unmatched/unexpected while/endwhile inside of if body" );
		return com_list;
	    }
	    mprog_process_cmnd( cmnd, mob, actor, obj, rndm, vict, v_obj );
	    if ( mob->deleted )
		return null; /* Lam 31.5.98: na wszelki wypadek */
	    cmnd     = cmnd_buf;
	    com_list = mprog_next_command( com_list, cmnd );
	    while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		cmnd++;
	    if ( *cmnd == '\0' )
	    {
		progbug( mob, "missing else or endif" );
		return null;
	    }
	    morebuf = one_argument( cmnd, buf );
	}
    }
    else /* false ifcheck, find else and do existing commands or quit at endif */
    {
	ifnest = 0;
	whilenest = 0;
	/* Lam: zagniezdzanie */
	while ( ( str_cmp( buf, "else" ) || ifnest != 0 )
	     && ( str_cmp( buf, "endif" ) || ifnest != 0 ) )
	{
	    if ( !str_cmp( buf, "if" ) )       ifnest++;
	    if ( !str_cmp( buf, "endif" ) )    ifnest--;
	    if ( !str_cmp( buf, "while" ) )    whilenest++;
	    if ( !str_cmp( buf, "endwhile" ) ) whilenest--;
	    cmnd     = cmnd_buf;
	    com_list = mprog_next_command( com_list, cmnd );
	    while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		cmnd++;
	    if ( *cmnd == '\0' )
	    {
		progbug( mob, "missing an else or endif" );
		return null;
	    }
	    one_argument( cmnd, buf );
	}

	if ( whilenest )
	    progbug( mob, "unmatched/unexpected while/endwhile inside of if body" );
	/* found either an else or an endif.. act accordingly */
	if ( !str_cmp( buf, "endif" ) )
	    return com_list;
	cmnd     = cmnd_buf;
	com_list = mprog_next_command( com_list, cmnd );
	while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
	    cmnd++;
	if ( *cmnd == '\0' )
	{
	    progbug( mob, "missing endif" );
	    return null;
	}
	morebuf = one_argument( cmnd, buf );

	for ( ; ; ) /* process the post-else commands until an endif is found */
	{
	    if ( !str_cmp( buf, "if" ) )
	    {
		com_list = mprog_process_if( morebuf, com_list, mob, actor,
					     obj, rndm, vict, v_obj );
		while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		    cmnd++;
		if ( *com_list == '\0' )
		    return null;
		cmnd     = cmnd_buf;
		com_list = mprog_next_command( com_list, cmnd );
		morebuf  = one_argument( cmnd,buf );
		continue;
	    }
	    if ( !str_cmp( buf, "while" ) )
	    {
		com_list = mprog_process_while( morebuf, com_list, mob, actor,
					    obj, rndm, vict, v_obj );
		while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		    cmnd++;
		if ( *com_list == '\0' )
		    return null;
		cmnd     = cmnd_buf;
		com_list = mprog_next_command( com_list, cmnd );
		morebuf  = one_argument( cmnd,buf );
		continue;
	    }
	    if ( !str_cmp( buf, "else" ) )
	    {
		progbug( mob, "found else in an else section" );
		return null;
	    }
	    if ( !str_cmp( buf, "endwhile" ) )
		progbug( mob, "found endwhile in an else section" );
	    if ( !str_cmp( buf, "break" ) )
		return null;
	    if ( !str_cmp( buf, "endif" ) )
		return com_list;
	    mprog_process_cmnd( cmnd, mob, actor, obj, rndm, vict, v_obj );
	    if ( mob->deleted )
		return null; /* Lam 31.5.98: na wszelki wypadek */
	    cmnd     = cmnd_buf;
	    com_list = mprog_next_command( com_list, cmnd );
	    while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		cmnd++;
	    if ( *cmnd == '\0' )
	    {
		progbug( mob, "missing endif in else section" );
		return null;
	    }
	    morebuf = one_argument( cmnd, buf );
	}
    }

    bug( "if nie powinno tu wejsc!", 0 );

    return null;
}


/*
 * Lam 5.12.2003
 */
char *mprog_process_while( char *ifchck, char *com_list, CHAR_DATA *mob,
		       CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		       CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    static char null[ 1 ];
    char buf[ MIL ];
    char *morebuf = NULL;
    char cmnd_buf[ MIL ];
    char *cmnd = NULL;
    char *origifchck = ifchck;
    char *origcom_list = com_list;
    bool loopdone;
    bool flag;
    bool koniecpetli = FALSE;
    int legal;
    int ifnest, whilenest;
    bool not;
    char notbuf[ MIL ];
    int iter = 0;

    while ( !koniecpetli )
    {
	if ( ++iter > 100 )
	{
	    progbug( mob, "Zapetlona petla while! Przerywam po 100 iteracjach!" );
	    return null;
	}

	/* kazdy przebieg startujemy od nowa */
	ifchck = origifchck;
	com_list = origcom_list;
	loopdone = FALSE;
	flag = FALSE;
	not = FALSE;

	one_argument( ifchck, notbuf );
	if ( !str_cmp( notbuf, "not" ) )
	{
	    not = TRUE;
	    ifchck = one_argument( ifchck, notbuf );
	}

	if ( ( legal = mprog_do_ifchck( ifchck, mob, actor, obj, rndm, vict, v_obj ) ) )
	{
	    if ( legal == 1 )
		flag = TRUE;
	    else
		return null;
	}

	if ( not )
	    flag = !flag;

	while ( !loopdone ) /* scan over any existing or statements */
	{
	    cmnd     = cmnd_buf;
	    com_list = mprog_next_command( com_list, cmnd );
	    while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		cmnd++;
	    if ( *cmnd == '\0' )
	    {
		progbug( mob, "no commands after WHILE/OR" );
		return null;
	    }
	    morebuf = one_argument( cmnd, buf );
	    if ( !str_cmp( buf, "or" ) )
	    {
		if ( flag )
		     continue; /* Lam 15.4.2000, poprawka zlej metody z 22.10.1999 */

		one_argument( morebuf, notbuf );
		if ( !str_cmp( notbuf, "not" ) )
		{
		    not = TRUE;
		    morebuf = one_argument( morebuf, notbuf );
		}
		else
		    not = FALSE;

		if ( ( legal = mprog_do_ifchck( morebuf, mob, actor, obj, rndm, vict, v_obj ) ) )
		{
		    if ( legal == 1 )	/* ifcheck wyszedl TRUE */
		    {
			if ( !not )
			    flag = TRUE;
		    }
		    else		/* blad */
			return null;
		}
		else			/* ifcheck wyszedl FALSE */
		{
		    if ( not )
			flag = TRUE;
		}
	    }
	    else
		loopdone = TRUE;
	}

	/* w tym momencie przeszlismy wszystkie while/or i znamy wynik warunkow */
	if ( flag )
	{
	    for ( ; ; )
	    {
		if ( !str_cmp( buf, "if" ) )
		{
		    com_list = mprog_process_if( morebuf, com_list, mob, actor, obj, rndm, vict, v_obj );
		    while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
			cmnd++;
		    if ( *com_list == '\0' )
			return null;
		    cmnd     = cmnd_buf;
		    com_list = mprog_next_command( com_list, cmnd );
		    morebuf  = one_argument( cmnd,buf );
		    continue;
		}
		if ( !str_cmp( buf, "while" ) )
		{
		    com_list = mprog_process_while( morebuf, com_list, mob, actor, obj, rndm, vict, v_obj );
		    while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
			cmnd++;
		    if ( *com_list == '\0' )
			return null;
		    cmnd     = cmnd_buf;
		    com_list = mprog_next_command( com_list, cmnd );
		    morebuf  = one_argument( cmnd,buf );
		    continue;
		}
		if ( !str_cmp( buf, "break" ) )
		    return null;
		if ( !str_cmp( buf, "endif" )
		  || !str_cmp( buf, "else" ) )
		    progbug( mob, "unexpected else/endif inside of while loop, trying to continue..." );
		if ( !str_cmp( buf, "endwhile" ) )
		    break;
		mprog_process_cmnd( cmnd, mob, actor, obj, rndm, vict, v_obj );
		if ( mob->deleted ) return null; /* Lam 31.5.98 na wszelki wypadek */
		cmnd     = cmnd_buf;
		com_list = mprog_next_command( com_list, cmnd );
		while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		    cmnd++;
		if ( *cmnd == '\0' )
		{
		    progbug( mob, "missing endwhile" );
		    return null;
		}
		morebuf = one_argument( cmnd, buf );
	    }
	}
	else /* warunek falszywy, w while nie ma else, lecimy do konca petli */
	{
	    ifnest = 0;
	    whilenest = 0;
	    /* Lam: moge w ogole nie sprawdzac zagniezdzania ifow, ale moze
	       ktorys z krainkowcow bedzie mieszal te instrukcje, wiec trzeba to
	       sprawdzac */
	    while ( *buf && ( str_cmp( buf, "endwhile" ) || whilenest != 0 ) )
	    {
		if ( !str_cmp( buf, "if" ) )        ifnest++;
		if ( !str_cmp( buf, "endif" ) )     ifnest--;
		if ( !str_cmp( buf, "while" ) )     whilenest++;
		if ( !str_cmp( buf, "endwhile" ) )  whilenest--;
		cmnd     = cmnd_buf;
		com_list = mprog_next_command( com_list, cmnd );
		while ( ( *cmnd == ' ' || *cmnd == '\t' ) )
		    cmnd++;
		if ( *cmnd == '\0' )
		{
		    progbug( mob, "missing endwhile" );
		    return null;
		}
		one_argument( cmnd, buf );
	    }

	    if ( ifnest )
		progbug( mob, "unmatched/unexpected if/endif inside of while loop" );
	    /* koniec petli while */
	    return com_list;
	}
    } /* while ( !koniecpetli ) */

    bug( "while nie powinno tu wejsc!", 0 );

    return null;
}


/*
 * The main focus of the MOBprograms.  This routine is called
 * whenever a trigger is successful.  It is responsible for parsing
 * the command list and figuring out what to do. However, like all
 * complex procedures, everything is farmed out to the other guys.
 */
void mprog_driver( char *com_list, CHAR_DATA *mob, CHAR_DATA *actor,
		   OBJ_DATA *obj, CHAR_DATA *vict, OBJ_DATA *v_obj,
		   MPROG_DATA *mprogram, int gdzie )
{
    char tmpcmndlst[ 16 * MSL ];
    char buf[ MIL ];
    char *morebuf;
    char *command_list;
    char cmnd_buf[ MIL ];
    char *cmnd;
    CHAR_DATA *rndm = NULL;
    CHAR_DATA *vch = NULL;
    int count = 0;

    if ( IS_AFFECTED( mob, AFF_CHARM ) )
	return;

    if ( ++prog_nest > MAX_PROG_NEST )
    {
	progbug( mob, "usiluje przekroczyc MAX_PROG_NEST!" );
	prog_nest--;
	return;
    }

    /* get a random visible mortal player who is in the room with the mob */
    for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
	if ( !IS_NPC( vch )
	  && vch->level < LEVEL_IMMORTAL
	  && can_see( mob, vch ) )
	{
	    if ( number_range( 0, count ) == 0 )
		rndm = vch;
	    count++;
	}

    progdebug( mob, "{GPocz`atek proga.{x Mob #%d: %s %s\n\r", mob->pIndexData->vnum,
	       mprog_type_to_name( mprogram->type, gdzie ), mprogram->arglist );

    strcpy( tmpcmndlst, com_list );
    command_list = tmpcmndlst;
    cmnd         = cmnd_buf;
    command_list = mprog_next_command( command_list, cmnd );
    while ( *cmnd != '\0' )
    {
	morebuf = one_argument( cmnd, buf );
	if ( !str_cmp( buf, "if" ) )
	    command_list = mprog_process_if( morebuf, command_list, mob,
					actor, obj, rndm, vict, v_obj );
	else if ( !str_cmp( buf, "while" ) )
	    command_list = mprog_process_while( morebuf, command_list, mob,
					actor, obj, rndm, vict, v_obj );
	else if ( !str_cmp( buf, "endif" ) )
	    progbug( mob, "unexpected endif" );
	else if ( !str_cmp( buf, "else" ) )
	    progbug( mob, "unexpected else" );
	else if ( !str_cmp( buf, "endwhile" ) )
	    progbug( mob, "unexpected endwhile" );
	else if ( !str_cmp( buf, "break" ) )
	{
	    progdebug( mob, "{RBREAK na najwy`zszym poziomie.{x Mob #%d: %s %s\n\r", mob->pIndexData->vnum,
		   mprog_type_to_name( mprogram->type, gdzie ), mprogram->arglist );
	    break;
	}
	else
	{
	    mprog_process_cmnd( cmnd, mob, actor, obj, rndm, vict, v_obj );
	    if ( mob->deleted )
	    {
		prog_nest--;
		progdebug( mob, "{RKoniec proga - mob zgin`a`l.{x Mob #%d: %s %s\n\r", mob->pIndexData->vnum,
		     mprog_type_to_name( mprogram->type, gdzie ), mprogram->arglist );
		return; /* Lam 31.5.98 na wszelki wypadek */
	    }
	}
	cmnd         = cmnd_buf;
	command_list = mprog_next_command( command_list, cmnd );
    }

    progdebug( mob, "{RKoniec proga.{x Mob #%d: %s %s\n\r", mob->pIndexData->vnum,
	       mprog_type_to_name( mprogram->type, gdzie ), mprogram->arglist );

    prog_nest--;

    return;
}


/***************************************************************************
 * Global function code and brief comments.
 */


/* The next two routines are the basic trigger types. Either trigger
 *  on a certain percent, or trigger on a keyword or word phrase.
 *  To see how this works, look at the various trigger routines..
 */
/*
 * Lam 12.6.98: return zamiast break - 2 pasujace slowa nie uruchomia proga
 *		dwukrotnie.
 * Lam 11.7.98: napisana inaczej obsluga r/eprogow.
 * Lam 22.3.99: teraz bedzie obslugiwac polskie litery. Najwazniejsza czesc
 *		funkcji napisana od nowa. Zmiana dzialania 'p'.
 * Lam 15.1.2003: czesc porownujaca lancuchy przepisana, obsluguje polskie
 *		litery oraz kolory, powinna tez dzialac duzo szybciej.
 */
void mprog_wordlist_check( char *arg, CHAR_DATA *mob, CHAR_DATA *actor,
			  OBJ_DATA *obj, CHAR_DATA *vict, OBJ_DATA *v_obj,
			  int type, MPROG_DATA *program, int gdzie )
{
    char        temp1[ MAX_STRING_LENGTH ];
    char        temp2[ MAX_INPUT_LENGTH ];
    char        word[ MAX_INPUT_LENGTH ];
    char        arg_po_konw[ MIL ];
    char        lst_po_konw[ MSL ];
    MPROG_DATA *mprg;
    TYP_ZMIENNEJ *zm;
    char       *list;
    char       *dupl;
    char       *lst_po_wsk;
    char       *arg_po_wsk;
    char       *start;
    char       *astr;
    char       *bstr;
    unsigned int i;
    bool        spacja;
    bool	skrot = FALSE;

    /* Lam: to byla bardzo niestabilna funkcja */
    if ( type != M_TRIG_PROG && IS_NPC( actor ) ) /* zbedne mob == actor */
	return;
    /* trig_prog jest wyjatkiem, ale i tak jest ograniczenie max_prog_nest */

    for ( mprg = program; mprg; mprg = mprg->next )
    {
	if ( mprg->type & type )
	{
	    strcpy( temp1, mprg->arglist );
	    list = temp1;
	    for ( i = 0; i < strlen( list ); i++ )
		list[ i ] = LOWER( list[ i ] );

	    strcpy( temp2, arg );
	    dupl = temp2;
	    for ( i = 0; i < strlen( dupl ); i++ )
		dupl[ i ] = LOWER( dupl[ i ] );

	    /* Lam 1.2.2003: tu jest lepiej, bo speech_progi p tez wymagaja
	       odkolorowania */
	    lst_po_wsk = lst_po_konw;
	    arg_po_wsk = arg_po_konw;

	    odkoloruj( list, lst_po_konw );
	    odkoloruj( dupl, arg_po_konw );

	    /* Lam 7.2.2006: * wylapuje wszystko */
	    if ( ( lst_po_konw[ 0 ] == '*' ) && ( lst_po_konw[ 1 ] == '\0' ) )
	    {
		zmien_zmienna( &mob->zmienne, "_argument", TZ_ZNAK, 0, arg_po_wsk, mob );
		zmien_zmienna( &mob->zmienne, "_trafione", TZ_ZNAK, 0, "*", mob );
		mprog_driver( mprg->comlist, mob, actor, obj, vict, v_obj, mprg, gdzie );
		if ( ( zm = znajdz_zmienna( mob->zmienne, "_argument" ) ) )
		    usun_z_listy( &mob->zmienne, zm, NULL );
		if ( ( zm = znajdz_zmienna( mob->zmienne, "_trafione" ) ) )
		    usun_z_listy( &mob->zmienne, zm, NULL );
	    }

	    /* 'p' powoduje, ze caly tekst musi byc identycznie wpisany. Inaczej
	       to nie mialo sensu, bo mozna bylo zrobic speech_prog p a b~ albo
	       speech_prog 'a b'~... */
	    if ( ( lst_po_konw[ 0 ] == 'p' ) && ( lst_po_konw[ 1 ] == ' ' ) )
	    {
		lst_po_wsk += 2;
		if ( !str_cmp( lst_po_wsk, arg_po_wsk ) )
		{
		    zmien_zmienna( &mob->zmienne, "_argument", TZ_ZNAK, 0, arg_po_wsk, mob );
		    zmien_zmienna( &mob->zmienne, "_trafione", TZ_ZNAK, 0, lst_po_wsk, mob );
		    mprog_driver( mprg->comlist, mob, actor, obj, vict, v_obj, mprg, gdzie );
		    if ( ( zm = znajdz_zmienna( mob->zmienne, "_argument" ) ) )
			usun_z_listy( &mob->zmienne, zm, NULL );
		    if ( ( zm = znajdz_zmienna( mob->zmienne, "_trafione" ) ) )
			usun_z_listy( &mob->zmienne, zm, NULL );
		    return;
		}
	    }
	    else
	    {
		/* Alandar 06.12.2004: 's' powoduje, ze dzialaja rowniez skroty */
		if ( ( lst_po_konw[ 0 ] == 's' ) && ( lst_po_konw[ 1 ] == ' ' ) )
		{
		    lst_po_wsk += 2;
		    skrot = TRUE;
		}
		/* Lam 16.1.2003: przepisane zupelnie, pomijajac str_str, spora
		   czesc kopiowana z str_prefix (polskie) */
		lst_po_wsk = one_argument( lst_po_wsk, word );
		for ( ; *word; lst_po_wsk = one_argument( lst_po_wsk, word ) )
		{
		    spacja = TRUE; /* poczatek slowa */
		    for ( start = arg_po_wsk; *start; start++ )
		    {
			/* bedzie poczatek slowa */
			if ( isspace( (unsigned char) *start ) )
			{
			    spacja = TRUE;
			    continue;
			}

			/* nie spacja, kolejna litera w nie pasujacym wyrazie */
			if ( !spacja )
			    continue;

			/* spacja, pierwsza litera wyrazu, porownajmy */
			astr = start;
			bstr = word;

			for ( ; ; astr++, bstr++ )
			{
			    /* koniec szukanego prez proga zwrotu, znalazlem */
			    if ( !*bstr )
			    {
				/* nie sprawdzam prefiksow, tylko cale zwroty
				   ograniczone spacjami */
				if ( *astr && !isspace( (unsigned char) *astr ) )
				    spacja = FALSE;
				break;
			    }

			    /* jesli skroty maja dzialac (skrot == TRUE) i astr
			       sie skonczyl, to znaczy, ze ciag pasuje
			       i przechodzimy dalej */
			    if ( skrot && ( !*astr || isspace( (unsigned char) *astr ) ) )
				break;
			    if ( *astr != *bstr )
			    {
				if ( *astr == '`' )
				{
				    astr++;
				    if ( *astr == *bstr )
					continue;
				    if ( *astr == 'x' && *bstr == 'z' )
					continue;
				    spacja = FALSE;
				    break;
				}
				if ( *bstr == '`' )
				{
				    bstr++;
				    if ( *astr == *bstr )
					continue;
				    if ( *astr == 'z' && *bstr == 'x' )
					continue;
				    spacja = FALSE;
				    break;
				}
				spacja = FALSE;
				break;
			    }
			}

			if ( spacja )
			{
			    zmien_zmienna( &mob->zmienne, "_argument", TZ_ZNAK, 0, arg_po_wsk, mob );
			    zmien_zmienna( &mob->zmienne, "_trafione", TZ_ZNAK, 0, word, mob );
			    mprog_driver( mprg->comlist, mob, actor, obj, vict, v_obj, mprg, gdzie );
			    if ( ( zm = znajdz_zmienna( mob->zmienne, "_argument" ) ) )
				usun_z_listy( &mob->zmienne, zm, NULL );
			    if ( ( zm = znajdz_zmienna( mob->zmienne, "_trafione" ) ) )
				usun_z_listy( &mob->zmienne, zm, NULL );
			    return; /* Lam */
			}
		    }
		}
	    }
	}
    }

    return;
}


/*
 * Lam: 11.7.98: lepsze przystosowanie do r/eprogow
 */
void mprog_percent_check( CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj,
			 CHAR_DATA *vict, OBJ_DATA *v_obj, int type, MPROG_DATA *program, int gdzie )
{
    MPROG_DATA * mprg;
    int prob;

    for ( mprg = program; mprg != NULL; mprg = mprg->next )
	if ( mprg->type & type )
	{
	    prob = atoi( mprg->arglist );
	    if ( prob == 100 /* wiekszosc progow i tak ma 100 */
	      || number_percent( ) <= prob )
	    {
		mprog_driver( mprg->comlist, mob, actor, obj, vict, v_obj, mprg, gdzie );
		if ( type != M_GREET_PROG && type != M_ALL_GREET_PROG
		  && type != R_GREET_PROG )
		    break;
	    }
	}

  return;
}


/* The triggers.. These are really basic, and since most appear only
 * once in the code (hmm. i think they all do) it would be more efficient
 * to substitute the code in and make the mprog_xxx_check routines global.
 * However, they are all here in one nice place at the moment to make it
 * easier to see what they look like. If you do substitute them back in,
 * make sure you remember to modify the variable names to the ones in the
 * trigger calls.
 */
/* act_prog nieuzywany w Lacu, aby zapobiec padom
void mprog_act_trigger( char *buf, CHAR_DATA *mob, CHAR_DATA *ch,
		       OBJ_DATA *obj, void *vo )
{
    MPROG_ACT_LIST * tmp_act;

    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & ACT_PROG ) )
    {
	tmp_act = alloc_mem( sizeof( MPROG_ACT_LIST ) );
	if ( mob->mpactnum > 0 )
	    tmp_act->next = mob->mpact->next;
	else
	    tmp_act->next = NULL;

	mob->mpact      = tmp_act;
	mob->mpact->buf = str_dup( buf );
	mob->mpact->ch  = ch;
	mob->mpact->obj = obj;
	mob->mpact->vo  = vo;
	mob->mpactnum++;
    }

    return;
}
*/


void mprog_bribe_trigger( CHAR_DATA *mob, CHAR_DATA *ch, int amount )
{
    MPROG_DATA *mprg;
   OBJ_DATA   *obj;

    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & M_BRIBE_PROG ) )
    {
	obj = create_money( amount );
	obj_to_char( obj, mob );
	mob->gold -= amount;

	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	    if ( ( mprg->type & M_BRIBE_PROG )
	      && ( amount >= atoi( mprg->arglist ) ) )
	    {
		mprog_driver( mprg->comlist, mob, ch, obj, NULL, NULL, mprg, 0 );
		break;
	    }

	/* jesli niczego nie zrobil ze zlotem */
	if ( !obj->deleted && obj->carried_by == mob )
	{
	    extract_obj( obj );
	    mob->gold += amount;
	}
    }

    return;
}


bool mprog_death_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    int position;
    bool zwrot = FALSE;
    TYP_ZMIENNEJ *zm;

    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & M_DEATH_PROG ) )
    {
	/* Lam: dodalem zmiane pozycji, nie robil tego sam */
	position = mob->position;
	mob->position = POS_STANDING;
	/* Lam 15.6.98: dodalem ch ($n), bo o dziwo nie bylo */
	mprog_percent_check( mob, ch, NULL, NULL, NULL, M_DEATH_PROG,
		   mob->pIndexData->mobprogs, 0 );
	mob->position = position; /* zazwyczaj POS_DEAD */
	if ( ( zm = znajdz_zmienna( mob->zmienne, "zwrot" ) ) )
	{
	    if ( zm->typ == TZ_CALK
	      && zm->wartosc.i == 1 )
		zwrot = TRUE;
	    usun_z_listy( &mob->zmienne, zm, NULL );
	}
    }

    return zwrot;
}


void mprog_entry_trigger( CHAR_DATA *mob )
{
    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & M_ENTRY_PROG ) )
	mprog_percent_check( mob, NULL, NULL, NULL, NULL, M_ENTRY_PROG,
		mob->pIndexData->mobprogs, 0 );

    return;
}


void mprog_fight_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & M_FIGHT_PROG ) )
	mprog_percent_check( mob, ch, NULL, NULL, NULL, M_FIGHT_PROG,
		mob->pIndexData->mobprogs, 0 );

    return;
}


void mprog_give_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
    char        buf[ MAX_INPUT_LENGTH ];
    MPROG_DATA *mprg;

    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & M_GIVE_PROG ) )
    {
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	{
	    one_argument( mprg->arglist, buf );
	    if ( ( mprg->type & M_GIVE_PROG )
	      && ( !str_cmp( obj->name, mprg->arglist )
		|| !str_cmp( "all", buf )
		|| ( mprg->arglist[ 0 ] == '#'
		   && atoi( mprg->arglist + 1 ) == obj->pIndexData->vnum ) ) )
	    {
		mprog_driver( mprg->comlist, mob, ch, obj, NULL, NULL, mprg, 0 );
		break;
	    }
	}
    }

    return;
}


void mprog_greet_trigger( CHAR_DATA *ch )
{
    CHAR_DATA *vmob, *vmob_next;

    for ( vmob = ch->in_room->people; vmob; vmob = vmob_next )
    {
	vmob_next = vmob->next_in_room;

	if ( !IS_NPC( vmob )
	  || vmob->fighting
	  || !IS_AWAKE( vmob )
	  || ( IS_NPC( ch ) && ch->pIndexData == vmob->pIndexData ) )
	{
	    continue;
	}

	if ( can_see( vmob, ch ) && vmob->pIndexData->progtypes & M_GREET_PROG )
	    mprog_percent_check( vmob, ch, NULL, NULL, NULL, M_GREET_PROG,
		     vmob->pIndexData->mobprogs, 0 );
	else if ( vmob->pIndexData->progtypes & M_ALL_GREET_PROG )
	    mprog_percent_check( vmob, ch, NULL, NULL, NULL, M_ALL_GREET_PROG,
		     vmob->pIndexData->mobprogs, 0 );
    }

    return;
}


/*
 * Lam 22.7.98: UMAX( 1, mob->max_hit ), prowizorycznie, ale skutecznie.
 *		(Na wszelki wypadek zabezpieczenie przed dzieleniem przez zero.)
 */
void mprog_hitprcnt_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    MPROG_DATA *mprg;

    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & M_HITPRCNT_PROG ) )
    {
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	    if ( ( mprg->type & M_HITPRCNT_PROG )
	      && ( ( 100 * mob->hit / UMAX( 1, mob->max_hit ) ) < atoi( mprg->arglist ) ) )
	    {
		mprog_driver( mprg->comlist, mob, ch, NULL, NULL, NULL, mprg, 0 );
		break;
	    }
    }

    return;
}


void mprog_random_trigger( CHAR_DATA *mob )
{
    if ( mob->pIndexData->progtypes & M_RAND_PROG )
	mprog_percent_check( mob, NULL, NULL, NULL, NULL, M_RAND_PROG,
		mob->pIndexData->mobprogs, 0 );

    return;
}


/*
 * Lam 9.6.99: Teraz ta funkcja obsluguje speech_proga, tell_proga i say_proga.
 * Dodatkowy parametr mowi funkcji, czy zostala wywolana przez do_say, czy moze
 * do_tell/retell/reply/whisper. say_prog odpala sie przy mow ('); tell_prog
 * przy powiedz, odpowiedz, ponow, szepcz, mowdo; speech_prog w obu przypadkach.
 * Uwaga: gadanie w grupach nie jest obslugiwane, oczarowane moby nie moga
 * wykonywac progow.
 */
void mprog_speech_trigger( CHAR_DATA *ch, char *txt, CHAR_DATA *vict )
{
    CHAR_DATA *vmob;
    CHAR_DATA *vmob_next;
    int flagi = M_SPEECH_PROG | ( vict ? M_TELL_PROG : M_SAY_PROG );

    /* sprytna petla. Jesli jest vict, wykona sie tylko raz, tylko dla niego */
    for ( vmob = vict ? vict : ch->in_room->people;
	  vmob;
	  vmob = vmob_next )
    {
	vmob_next = vict ? NULL : vmob->next_in_room;

	if ( IS_NPC( vmob )
	  && IS_AWAKE( vmob )
	  && ( vmob->pIndexData->progtypes & flagi ) )
	{
	    mprog_wordlist_check( txt, vmob, ch, NULL, NULL, NULL, flagi,
		vmob->pIndexData->mobprogs, 0 );
	}
    }

    return;
}


void mprog_time_trigger( CHAR_DATA *mob )
{
    if ( mob->pIndexData->progtypes & M_TIME_PROG )
	mprog_percent_check( mob, NULL, NULL, NULL, NULL, M_TIME_PROG,
		mob->pIndexData->mobprogs, 0 );

    return;
}


void mprog_asleep_time_trigger( CHAR_DATA *mob )
{
    if ( mob->pIndexData->progtypes & M_ASLEEP_TIME_PROG )
	mprog_percent_check( mob, NULL, NULL, NULL, NULL, M_ASLEEP_TIME_PROG,
		mob->pIndexData->mobprogs, 0 );

    return;
}


void mprog_soc_trigger( char *txt, CHAR_DATA *mob, CHAR_DATA *actor )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & M_SOC_PROG ) )
	mprog_wordlist_check( txt, mob, actor, NULL, NULL, NULL, M_SOC_PROG,
	    mob->pIndexData->mobprogs, 0 );

    return;
}


/*
 * 7.1.2004
 */
bool mprog_descr_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    MPROG_DATA *mprg;

    if ( IS_NPC( mob ) && IS_SET( mob->pIndexData->progtypes, M_DESCR_PROG ) )
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	    if ( mprg->type & M_DESCR_PROG )
	    {
		descr_prog_in_progress = TRUE;
		descr_prog_buffer[ 0 ] = '\0';
		mprog_driver( mprg->comlist, mob, ch, NULL, NULL, NULL, mprg, 0 );
		descr_prog_in_progress = FALSE;
		STC( descr_prog_buffer, ch );
		descr_prog_buffer[ 0 ] = '\0';
		return TRUE;
	    }

    return FALSE;
}


/*
 * 31.5.2006
 */
bool mprog_long_descr_trigger( CHAR_DATA *mob, CHAR_DATA *ch, char *buf )
{
    MPROG_DATA *mprg;

    if ( IS_NPC( mob ) && IS_SET( mob->pIndexData->progtypes, M_LONG_DESCR_PROG ) )
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	    if ( mprg->type & M_LONG_DESCR_PROG )
	    {
		descr_prog_in_progress = TRUE;
		descr_prog_buffer[ 0 ] = '\0';
		mprog_driver( mprg->comlist, mob, ch, NULL, NULL, NULL, mprg, 0 );
		descr_prog_in_progress = FALSE;
		strcat( buf, descr_prog_buffer );
		descr_prog_buffer[ 0 ] = '\0';
		return TRUE;
	    }

    return FALSE;
}


/*
 * Alandar 22.12.2004: prog reagujacy na karmienie konkretnym przedmiotem lub
 * czymkolwiek
 */
void mprog_feed_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
    char	 buf[ MIL ];
    MPROG_DATA	*mprg;

    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & M_FEED_PROG ) )
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	{
	    one_argument( mprg->arglist, buf );

	    if ( ( mprg->type & M_FEED_PROG )
	      && ( !str_cmp( obj->name, mprg->arglist )
		|| !str_cmp( "all", buf )
		|| ( mprg->arglist[ 0 ] == '#'
		  && atoi( mprg->arglist + 1 ) == obj->pIndexData->vnum ) ) )
	    {
		mprog_driver( mprg->comlist, mob, ch, obj, NULL, NULL, mprg, 0 );
		break;
	    }
	}

    return;
}


/*
 * Alandar 22.12.2004: prog reagujacy na pojenie z konkretnego przedmiotu lub
 * czegokolwiek
 */
void mprog_water_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{
    char	 buf[ MIL ];
    MPROG_DATA	*mprg;

    if ( IS_NPC( mob )
      && ( mob->pIndexData->progtypes & M_WATER_PROG ) )
	for ( mprg = mob->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	{
	    one_argument( mprg->arglist, buf );

	    if ( ( mprg->type & M_WATER_PROG )
	      && ( !str_cmp( obj->name, mprg->arglist )
		|| !str_cmp( "all", buf )
		|| ( mprg->arglist[ 0 ] == '#'
		  && atoi( mprg->arglist + 1 ) == obj->pIndexData->vnum ) ) )
	    {
		mprog_driver( mprg->comlist, mob, ch, obj, NULL, NULL, mprg, 0 );
		break;
	    }
	}

    return;
}


void mprog_spell_victim_trigger( char *txt, CHAR_DATA *mob, CHAR_DATA *actor )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & M_SPELL_VICTIM_PROG ) )
	mprog_wordlist_check( txt, mob, actor, NULL, NULL, NULL, M_SPELL_VICTIM_PROG,
	    mob->pIndexData->mobprogs, 0 );
/* pomysl:
   - jesli prog ustawi zmienna _zwrot na 1, nie wykonac funkcji czaru,
   - jesli prog ustawi zmienna _zwrot na 2, jw. oraz nie dac opoznienia i nie
     ustawiac walki.
   Oczywiscie to by wymagalo uruchamiania wyzwalacza przed rzuceniem czaru,
   a nie po, jak jest teraz (zawsze mozna dac osobny wyzwalacz) */
    return;
}


void mprog_aggr_attack_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & M_AGGR_ATTACK_PROG ) )
	mprog_percent_check( mob, ch, NULL, NULL, NULL, M_AGGR_ATTACK_PROG,
		mob->pIndexData->mobprogs, 0 );

    return;
}


/*
 * Lam: room_supermob
 * Wzorowane oczywiscie na Smaugu. To dobry wzor. Naturalnie ja to robie lepiej :)
 */
CHAR_DATA *room_supermob( ROOM_INDEX_DATA *room )
{
    CHAR_DATA  *supermob;
    char	buf[ MIL ];

    supermob = create_mobile( get_mob_index( MOB_VNUM_SUPERMOB ) );

    free_string( supermob->short_descr );
    supermob->short_descr = str_dup( room->name );
    free_string( supermob->dopelniacz );
    supermob->dopelniacz = str_dup( room->name );
    free_string( supermob->celownik );
    supermob->celownik = str_dup( room->name );
    free_string( supermob->biernik );
    supermob->biernik = str_dup( room->name );
    free_string( supermob->narzednik );
    supermob->narzednik = str_dup( room->name );
    free_string( supermob->miejscownik );
    supermob->miejscownik = str_dup( room->name );
    free_string( supermob->wolacz );
    supermob->wolacz = str_dup( room->name );

    sprintf( buf, "Pom. %5d", room->vnum );
    free_string( supermob->description );
    supermob->description = str_dup( buf );

    supermob->area = room->area;

    char_to_room( supermob, room );

    return supermob;
}


/*
 * Lam: exit_supermob. Przydaloby sie wiecej sprawdzania bledow
 */
CHAR_DATA *exit_supermob( ROOM_INDEX_DATA *room, int door )
{
    CHAR_DATA  *supermob;
    EXIT_DATA  *exit = room->exit[door];
    char	buf[ MIL ];

    supermob = create_mobile( get_mob_index( MOB_VNUM_SUPERMOB ) );

    if ( exit->short_descr && *exit->short_descr )
	strcpy( buf, exit->short_descr );
    else
	strcpy( buf, "drzwi" );

    free_string( supermob->short_descr );
    supermob->short_descr = str_dup( buf );
    free_string( supermob->dopelniacz );
    supermob->dopelniacz = str_dup( buf );
    free_string( supermob->celownik );
    supermob->celownik = str_dup( buf );
    free_string( supermob->biernik );
    supermob->biernik = str_dup( buf );
    free_string( supermob->narzednik );
    supermob->narzednik = str_dup( buf );
    free_string( supermob->miejscownik );
    supermob->miejscownik = str_dup( buf );
    free_string( supermob->wolacz );
    supermob->wolacz = str_dup( buf );

    sprintf( buf, "Wyj. %5d:%d", room->vnum, door );
    free_string( supermob->description );
    supermob->description = str_dup( buf );

    supermob->area = room->area;

    char_to_room( supermob, room );

    return supermob;
}


/*
 * Lam: obj_supermob
 */
CHAR_DATA *obj_supermob( OBJ_DATA *obj, ROOM_INDEX_DATA *room )
{
    CHAR_DATA  *supermob;
    char	buf[ MIL ];

    supermob = create_mobile( get_mob_index( MOB_VNUM_SUPERMOB ) );

    free_string( supermob->short_descr );
    supermob->short_descr = str_dup( obj->short_descr );
    free_string( supermob->dopelniacz );
    supermob->dopelniacz = str_dup( obj->dopelniacz );
    free_string( supermob->celownik );
    supermob->celownik = str_dup( obj->celownik );
    free_string( supermob->biernik );
    supermob->biernik = str_dup( obj->biernik );
    free_string( supermob->narzednik );
    supermob->narzednik = str_dup( obj->narzednik );
    free_string( supermob->miejscownik );
    supermob->miejscownik = str_dup( obj->miejscownik );
    free_string( supermob->wolacz );
    supermob->wolacz = str_dup( obj->short_descr );

    sprintf( buf, "Prz. %5d", obj->pIndexData->vnum );
    free_string( supermob->description );
    supermob->description = str_dup( buf );

    supermob->area = obj->pIndexData->area;

    char_to_room( supermob, room );

    return supermob;
}


void dismiss_supermob( CHAR_DATA *supermob )
{
    if ( !supermob )
    {
	bug( "dismiss_supermob: NULL supermob", 0 );
	return;
    }

    if ( supermob->deleted )
	return; /* to nie blad - supermob moze czasem zginac... */

    if ( !supermob->in_room )
	bug( "dismiss_supermob: supermob in NULL room", 0 );

    extract_char( supermob, TRUE ); /* i tak zostaje w char_list do update */

    return;
}


/* --------========[ RPROGI ]========-------- */


/*
 * greet_prog dla pomieszczen
 */
void rprog_greet_trigger( CHAR_DATA *ch )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->progtypes, R_GREET_PROG ) )
    {
	supermob = room_supermob( ch->in_room );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, R_GREET_PROG,
			ch->in_room->mobprogs, 1 );
	dismiss_supermob( supermob );
    }

    return;
}


void rprog_speech_trigger( char *txt, CHAR_DATA *ch )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->progtypes, R_SPEECH_PROG ) )
    {
	supermob = room_supermob( ch->in_room );
	mprog_wordlist_check( txt, supermob, ch, NULL, NULL, NULL, R_SPEECH_PROG,
			ch->in_room->mobprogs, 1 );
	dismiss_supermob( supermob );
    }

    return;
}


void rprog_look_trigger( CHAR_DATA *ch )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->progtypes, R_LOOK_PROG ) )
    {
	supermob = room_supermob( ch->in_room );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, R_LOOK_PROG,
			ch->in_room->mobprogs, 1 );
	dismiss_supermob( supermob );
    }

    return;
}


/*
 * Lam 9.4.2000
 * Wywolywane kiedy gracz upuszcza cos na ziemie pomieszczenia. Napisane, aby
 * uzdrowiciel mogl zbierac przedmioty spod oltarza.
 */
void rprog_drop_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->progtypes, R_DROP_PROG ) )
    {
	supermob = room_supermob( ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, R_DROP_PROG,
			ch->in_room->mobprogs, 1 );
	dismiss_supermob( supermob );
    }

    return;
}


void rprog_time_trigger( ROOM_INDEX_DATA *room )
{
    CHAR_DATA *supermob;

    if ( IS_SET( room->progtypes, R_TIME_PROG ) )
    {
	supermob = room_supermob( room );
	mprog_percent_check( supermob, NULL, NULL, NULL, NULL, R_TIME_PROG,
			room->mobprogs, 1 );
	dismiss_supermob( supermob );
    }

    return;
}


/*
 * Lam 27.06.99: widzieliscie kiedys Diku z pomieszczeniami zaleznymi od pory
 * dnia, pogody, ilosci mobow w pomieszczeniu, tego, czy mieszkaniec jest
 * wewnatrz czy nie (przyklad krola goblinow - ismobhere( ))? Nie? To teraz
 * macie Laca.
 *
 * Wszystkie ifchecki dzialaja, dziala liczenie, podstawianie roznych tekstow,
 * roznica miedzy zwyczajnym progiem, a tym, polega na tym, ze normalnie
 * wszystko sie wykonuje, a tutaj leci do gracza (bez zadnego mpecho).
 * Alandar 06.12.2004: dodana obsluga descr_progowych dodatkowych opisow
 * Lam 14.7.2006: umozliwiane zagladanie do innych pomieszczen
 */
bool rprog_descr_trigger( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *txt )
{
    CHAR_DATA *supermob;
    MPROG_DATA *mprg;

    if ( IS_SET( room->progtypes, R_DESCR_PROG ) )
    {
	if ( txt )
	{
	    descr_prog_in_progress = TRUE;
	    descr_prog_buffer[ 0 ] = '\0';
	    supermob = room_supermob( room );
	    mprog_wordlist_check( txt, supermob, ch, NULL, NULL, NULL,
				R_DESCR_PROG, room->mobprogs, 1 );
	    dismiss_supermob( supermob );
	    descr_prog_in_progress = FALSE;
	    if ( descr_prog_buffer[ 0 ] )
	    {
		STC( descr_prog_buffer, ch );
		descr_prog_buffer[ 0 ] = '\0';
		return TRUE;
	    }
	    else
		return FALSE;
	}
	for ( mprg = room->mobprogs; mprg != NULL; mprg = mprg->next )
	    if ( ( mprg->type & R_DESCR_PROG ) && !str_cmp( mprg->arglist, "" ) )
	    {
		descr_prog_in_progress = TRUE;
		descr_prog_buffer[ 0 ] = '\0';
		supermob = room_supermob( room );
		mprog_driver( mprg->comlist, supermob, ch, NULL, NULL, NULL, mprg, 1 );
		dismiss_supermob( supermob );
		descr_prog_in_progress = FALSE;
		STC( descr_prog_buffer, ch );
		if ( descr_prog_buffer[ 0 ] )
		{
		    descr_prog_buffer[ 0 ] = '\0';
		    return TRUE;
		}
		else
		    return FALSE;
	    }
    }

    return FALSE;
}


/* -=- DO_PROG -=- */
bool rprog_do_trigger( CHAR_DATA *ch, char *cmd, char *arg, bool skrot )
{
    MPROG_DATA *mprg;
    CHAR_DATA *supermob;
    OBJ_DATA *obj = NULL;
    CHAR_DATA *vict = NULL;

    if ( !IS_SET( ch->in_room->progtypes, R_DO_PROG ) )
	return FALSE;

    for ( mprg = ch->in_room->mobprogs; mprg; mprg = mprg->next )
	if ( ( mprg->type & R_DO_PROG ) )
	{
	    char typ[ MIL ];		/* typ do_proga */
	    char nazwa[ MIL ];		/* polecenie */
	    char *tmp;

	    tmp = mprg->arglist;
	    tmp = one_argument( tmp, typ );
	    tmp = one_argument( tmp, nazwa );

	    /* Lam 22.12.2005: do_prog - *~ umozliwa wylapanie wszystkiego,
	       co nie pasowalo do innych do_progow */
	    if ( str_cmp( nazwa, "*" )
	      && ( ( skrot && str_prefix( cmd, nazwa ) )
		|| ( !skrot && str_cmp( cmd, nazwa ) ) ) )
		continue;

	    /* tu dochodzi jesli argument proga to *
	       lub nazwa polecenia jest zgodna z argumentem */
	    switch ( typ[ 0 ] )
	    {
		default:
			progbug( NULL,
				"Do_prog (#%d, %s): nie obsluguje typu '%s'",
				ch->in_room->vnum, nazwa, typ );
			return FALSE;

		case '-': /* po prostu wykonanie bez patrzenia na argumenty */
		case 'n':
			break;
		case 's':
			{
			    char argument[ MIL ];
			    tmp = one_argument( tmp, argument );
			    if ( !arg
			      || !arg[ 0 ]
			      || str_cmp( arg, argument ) )
			    {
				ch_printf( ch, "%s\n\r", tmp );
				return TRUE;
			    }
			}
			break;
		case 'p':
			{
			    char argument[ MIL ];
			    tmp = one_argument( tmp, argument );
			    if ( !arg
			      || !arg[ 0 ]
			      || str_prefix( arg, argument ) )
			    {
				ch_printf( ch, "%s\n\r", tmp );
				return TRUE;
			    }
			}
			break;
		case 'o':
		case 'i':
			if ( !arg || !arg[ 0 ] )
			{
			    send_to_char( "Musisz poda`c nazw`e przedmiotu.\n\r", ch );
			    return TRUE;
			}
			obj = typ[ 0 ] == 'o' ? get_obj_here( ch, arg )
					      : get_obj_carry( ch, arg );
			if ( !obj )
			{
			    send_to_char( "Nie widz`e tego.\n\r", ch );
			    return TRUE;
			}
			break;
		case 'c':
			if ( !arg || !arg[ 0 ] )
			{
			    send_to_char( "Musisz poda`c imi`e postaci.\n\r", ch );
			    return TRUE;
			}
			vict = get_char_room( ch, arg );
			if ( !vict )
			{
			    send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
			    return TRUE;
			}
			break;
	    }

	    supermob = room_supermob( ch->in_room );
	    zmien_zmienna( &supermob->zmienne, "_polecenie", TZ_ZNAK, 0, cmd, supermob );
	    zmien_zmienna( &supermob->zmienne, "_argument", TZ_ZNAK, 0, arg, supermob );
	    mprog_driver( mprg->comlist, supermob, ch, obj, vict, NULL, mprg, 1 );
	    dismiss_supermob( supermob );

	    /* Ulryk 01.12.2004: gdy damy argument "n", prog ignoruje argumenty
	       oraz _nie_ powstrzymuje interpretera przed wywolaniem oryginalnej
	       funkcji */
	    if ( typ[ 0 ] != 'n' )
		return TRUE;
	    else
		return FALSE;
	}

    return FALSE;
}


void rprog_random_trigger( ROOM_INDEX_DATA *room )
{
    CHAR_DATA *supermob;
    if ( IS_SET( room->progtypes, R_RAND_PROG ) )
    {
	supermob = room_supermob( room );
	mprog_percent_check( supermob, NULL, NULL, NULL, NULL, R_RAND_PROG,
	  room->mobprogs, 1 );
	dismiss_supermob( supermob );
    }

    return;
}


void rprog_undug_trigger( CHAR_DATA *ch )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->progtypes, R_UNDUG_PROG ) )
    {
	supermob = room_supermob( ch->in_room );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, R_UNDUG_PROG,
			ch->in_room->mobprogs, 1 );
	dismiss_supermob( supermob );
    }

    return;
}


/* --------========[ EPROGI ]========-------- */


void eprog_open_trigger( CHAR_DATA *ch, int door )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->exit[door]->progtypes, E_OPEN_PROG ) )
    {
	supermob = exit_supermob( ch->in_room, door );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, E_OPEN_PROG,
			ch->in_room->exit[door]->mobprogs, 2 );
	dismiss_supermob( supermob );
    }

    return;
}


void eprog_close_trigger( CHAR_DATA *ch, int door )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->exit[door]->progtypes, E_CLOSE_PROG ) )
    {
	supermob = exit_supermob( ch->in_room, door );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, E_CLOSE_PROG,
			ch->in_room->exit[door]->mobprogs, 2 );
	dismiss_supermob( supermob );
    }

    return;
}


void eprog_pass_trigger( CHAR_DATA *ch, int door )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->exit[door]->progtypes, E_PASS_PROG ) )
    {
	supermob = exit_supermob( ch->in_room, door );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, E_PASS_PROG,
			ch->in_room->exit[door]->mobprogs, 2 );
	dismiss_supermob( supermob );
    }

    return;
}


void eprog_lock_trigger( CHAR_DATA *ch, int door )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->exit[door]->progtypes, E_LOCK_PROG ) )
    {
	supermob = exit_supermob( ch->in_room, door );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, E_LOCK_PROG,
			ch->in_room->exit[door]->mobprogs, 2 );
	dismiss_supermob( supermob );
    }

    return;
}


void eprog_unlock_trigger( CHAR_DATA *ch, int door )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->exit[door]->progtypes, E_UNLOCK_PROG ) )
    {
	supermob = exit_supermob( ch->in_room, door );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, E_UNLOCK_PROG,
			ch->in_room->exit[door]->mobprogs, 2 );
	dismiss_supermob( supermob );
    }

    return;
}


void eprog_pick_trigger( CHAR_DATA *ch, int door )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->exit[door]->progtypes, E_PICK_PROG ) )
    {
	supermob = exit_supermob( ch->in_room, door );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, E_PICK_PROG,
			ch->in_room->exit[door]->mobprogs, 2 );
	dismiss_supermob( supermob );
    }

    return;
}


void eprog_bash_trigger( CHAR_DATA *ch, int door )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->exit[door]->progtypes, E_BASH_PROG ) )
    {
	supermob = exit_supermob( ch->in_room, door );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, E_BASH_PROG,
			ch->in_room->exit[door]->mobprogs, 2 );
	dismiss_supermob( supermob );
    }

    return;
}


void eprog_dig_trigger( CHAR_DATA *ch, int door )
{
    CHAR_DATA *supermob;

    if ( IS_SET( ch->in_room->exit[door]->progtypes, E_DIG_PROG ) )
    {
	supermob = exit_supermob( ch->in_room, door );
	mprog_percent_check( supermob, ch, NULL, NULL, NULL, E_DIG_PROG,
			ch->in_room->exit[door]->mobprogs, 2 );
	dismiss_supermob( supermob );
    }

    return;
}


bool eprog_bounce_trigger( CHAR_DATA *ch, int door )
{
    CHAR_DATA *supermob;
    MPROG_DATA *mprg;
    char arg[ MIL ];
    bool wylacz_komunikat = FALSE;

    if ( !ch->in_room
      || !ch->in_room->exit[ door ] )
	return FALSE;

    for ( mprg = ch->in_room->exit[ door ]->mobprogs; mprg; mprg = mprg->next )
	if ( ( mprg->type & E_BOUNCE_PROG ) )
	{
	    one_argument( mprg->arglist, arg );
	    if ( atoi( arg ) == 1 )
		wylacz_komunikat = TRUE;

	    supermob = exit_supermob( ch->in_room, door );
	    mprog_driver( mprg->comlist, supermob, ch, NULL, NULL, NULL, mprg, 2 );
	    dismiss_supermob( supermob );
	    break;
	}

    return wylacz_komunikat;
}


/* --------========[ OPROGI ]========-------- */

/* Alandar 21.12.2004 */
void oprog_open_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_OPEN_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_OPEN_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


/* Alandar 21.12.2004 */
void oprog_close_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_CLOSE_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_CLOSE_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


void oprog_take_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_TAKE_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_TAKE_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


void oprog_drop_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_DROP_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_DROP_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


void oprog_give_trigger( CHAR_DATA *ch, CHAR_DATA *ch2, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_GIVE_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, ch2, NULL, O_GIVE_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


void oprog_wear_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_WEAR_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_WEAR_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


void oprog_remove_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_REMOVE_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_REMOVE_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


void oprog_use_trigger( CHAR_DATA *ch, OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_USE_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	if ( argument )
	    zmien_zmienna( &supermob->zmienne, "_argument", TZ_ZNAK, 0, argument, supermob );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_USE_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


void oprog_dig_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_DIG_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_DIG_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


/*
 * Tissarius 27.10.2001
 */
void oprog_put_and_putin_trigger( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    CHAR_DATA *supermob;
    static bool in_put_prog;

    obj_from_char( obj );

    if ( IS_SET( obj->pIndexData->progtypes, O_PUT_PROG ) && !in_put_prog )
    {
	supermob = obj_supermob( obj, ch->in_room );
	in_put_prog = TRUE;
	obj_to_char( obj, supermob );
	mprog_percent_check( supermob, ch, obj, NULL, container, O_PUT_PROG,
			obj->pIndexData->mobprogs, 3 );

	/* jesli niczego nie zrobil z przedmiotem */
	if ( !obj->deleted && obj->carried_by == supermob )
	{
	    obj_from_char( obj );
	    dismiss_supermob( supermob );
	    in_put_prog = FALSE;
	}
	else if ( container->in_room && container->in_room != ch->in_room )
	{
	    obj_from_char( obj );
	    obj_to_char( obj, ch );
	    dismiss_supermob( supermob );
	    in_put_prog = FALSE;
	    return;
	}
	else
	{
	    dismiss_supermob( supermob );
	    in_put_prog = FALSE;
	    return;
	}
    }

    if ( IS_SET( container->pIndexData->progtypes, O_PUTIN_PROG )
	&& !container->deleted && !ch->deleted && !obj->deleted )
    {
	supermob = obj_supermob( container, ch->in_room );
	obj_to_char( obj, supermob );
	mprog_percent_check( supermob, ch, container, NULL, obj, O_PUTIN_PROG,
			container->pIndexData->mobprogs, 3 );

	/* jesli niczego nie zrobil z przedmiotem */
	if ( !obj->deleted && obj->carried_by == supermob )
	{
	    obj_from_char( obj );
	    dismiss_supermob( supermob );
	}
	else
	{
	    dismiss_supermob( supermob );
	    return;
	}
    }

    if ( !container->deleted )
	obj_to_obj( obj, container );
    else
	extract_obj( obj );

    return;
}


/*
 * Tissarius 22.10.2001: prog dla przedmiotow, do ktorych wkladane sa pieniadze
 */
void oprog_putin_gold_trigger( CHAR_DATA *ch, int amount, OBJ_DATA *container )
{
    CHAR_DATA *supermob;
    OBJ_DATA *obj2;

    ch->gold -= amount;

    if ( IS_SET( container->pIndexData->progtypes, O_PUTIN_GOLD_PROG ) )
    {
	MPROG_DATA *mprg;

	supermob = obj_supermob( container, ch->in_room );
	supermob->gold = amount;

	for ( mprg = container->pIndexData->mobprogs; mprg; mprg = mprg->next )
	    if ( ( mprg->type & O_PUTIN_GOLD_PROG )
	      && ( supermob->gold >= atoi( mprg->arglist ) ) )
	    {
		mprog_driver( mprg->comlist, supermob, ch, container, NULL, NULL, mprg, 3 );
		break;
	    }

	amount = supermob->gold;
	dismiss_supermob( supermob );
    }

    for ( obj2 = container->contains; obj2; obj2 = obj2->next_content )
    {
	if ( obj2->deleted )
	    continue;

	if ( obj2->pIndexData->vnum == OBJ_VNUM_MONEY_ONE
	  || obj2->pIndexData->vnum == OBJ_VNUM_MONEY_SOME )
	{
	    amount += obj2->pIndexData->vnum == OBJ_VNUM_MONEY_ONE ? 1 : obj2->value[ 0 ].v;
	    extract_obj( obj2 );
	}
    }
    obj_to_obj( create_money( amount ), container );

    return;
}


void oprog_sacr_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_SACR_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_SACR_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


void oprog_donate_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
{
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_DONATE_PROG ) )
    {
	supermob = obj_supermob( obj, ch->in_room );
	mprog_percent_check( supermob, ch, obj, NULL, NULL, O_DONATE_PROG,
			obj->pIndexData->mobprogs, 3 );
	dismiss_supermob( supermob );
    }

    return;
}


bool oprog_descr_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
    MPROG_DATA *mprg;
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_DESCR_PROG ) )
    {
	for ( mprg = obj->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	    if ( mprg->type & O_DESCR_PROG )
	    {
		supermob = obj_supermob( obj, ch->in_room );

		descr_prog_in_progress = TRUE;
		descr_prog_buffer[ 0 ] = '\0';
		mprog_driver( mprg->comlist, supermob, ch, obj, NULL, NULL, mprg, 3 );
		descr_prog_in_progress = FALSE;
		send_to_char( descr_prog_buffer, ch );
		descr_prog_buffer[ 0 ] = '\0';

		dismiss_supermob( supermob );
		return TRUE;
	    }
    }

    return FALSE;
}


bool oprog_long_descr_trigger( OBJ_DATA *obj, CHAR_DATA *ch, char *buf )
{
    MPROG_DATA *mprg;
    CHAR_DATA *supermob;

    if ( IS_SET( obj->pIndexData->progtypes, O_LONG_DESCR_PROG ) )
    {
	for ( mprg = obj->pIndexData->mobprogs; mprg != NULL; mprg = mprg->next )
	    if ( ( mprg->type & O_LONG_DESCR_PROG ) )
	    {
		supermob = obj_supermob( obj, ch->in_room );

		descr_prog_in_progress = TRUE;
		descr_prog_buffer[ 0 ] = '\0';
		mprog_driver( mprg->comlist, supermob, ch, obj, NULL, NULL, mprg, 3 );
		descr_prog_in_progress = FALSE;
		strcat( buf, descr_prog_buffer );
		descr_prog_buffer[ 0 ] = '\0';

		dismiss_supermob( supermob );
		return TRUE;
	    }
    }

    return FALSE;
}


/* ========================================================================= */

char *asctime_progbug( const struct tm *timeptr )
{
    static char result[ 128 ];

    sprintf( result, "%02d.%02d.%04d %2d:%02d:%02d | ",
	     timeptr->tm_mday, timeptr->tm_mon + 1, /* + 1: Lam 14.11.1999 */
	     1900 + timeptr->tm_year,
	     timeptr->tm_hour,
	     timeptr->tm_min, timeptr->tm_sec );

    return result;
}


/*
 * Lam 16.6.98: uzywane do zglaszania bledow mobprogowych.
 */
void progbug( CHAR_DATA *mob, const char *str, ... )
{
    FILE   *fp;
    char    buf[ MSL ];
    char    buf1[ MSL ];
    char *b, *b1;
    va_list args;

    if ( mob && IS_NPC( mob ) )
    {
	if ( mob->pIndexData->vnum == 3 )
	    sprintf( buf, "%s: ", mob->description ); /* Room 00 */
	else
	    sprintf( buf, "Mob  %5d: ", mob->pIndexData->vnum );
    }
    else if ( mob )
	strcpy( buf, "(gracz?)  : " );
    else
	strcpy( buf, "(brak)    : " );

    va_start( args, str );
    vsprintf( buf + strlen( buf ), str, args );
    va_end( args );

    strcpy( buf1, "[*] PROGBUG: " );
    strcat( buf1, buf );
    log_string( buf1 );

    strcpy( buf1, asctime_progbug( localtime( &current_time ) ) );
    strcat( buf1, buf );

    zamknij_reserve( fpBugReserve );
    if ( ( fp = fopen( PROGBUG_FILE, "a" ) ) )
    {
	fprintf( fp, "%s\n", buf1 );
	fclose( fp );
    }
    fpBugReserve = fopen( NULL_FILE, "r" );

    b = buf;
    b1 = buf1;
    while ( TRUE )
    {
	*b = *b1;
	if ( !*b )
	    break;
	if ( *b == '$' )
	{
	    b++;
	    *b = '\\';
	}
	b++;
	b1++;
    }

    wiznet( buf, mob, NULL, NULL, WIZ_PROGBUG, 0, 106 );

    return;
}


void progdebug( CHAR_DATA *mob, const char *str, ... )
{
    char    buf[ MSL ];
    va_list args;

    if ( !mob->debugger )
	return;

    va_start( args, str );
    vsprintf( buf, str, args );
    va_end( args );

    STC( " {G<----{x ", mob->debugger );
    STC( buf, mob->debugger );

    return;
}

