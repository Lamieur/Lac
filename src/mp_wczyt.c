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
 *  Wczytywanie progow z plikow krain i zewnetrznych plikow.               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"
#include "mp_wczyt.h"


/*
 * Uzywane wylacznie lokalnie.
 */
void	interpretuj_proga	args( ( MPROG_DATA *prog ) );
void	arg_do_proga		args( ( MPROG_DATA *prog ) );
int	mprog_name_to_type	args( ( char *name, int gdzie ) );
MPROG_DATA *mprog_file_read	args( ( char *f, MPROG_DATA *mprg,
					void *param, int gdzie ) );
bool	load_mobprogs		args( ( FILE *fp ) );
bool	mprog_read_programs	args( ( FILE *fp, void *param, int gdzie ) );


struct progi_type progi[ 4 ][ 26 ] =
{
    { /* MPROGI */
	{ "in_file_prog",	IN_FILE_PROG		},
/*	{ "act_prog", -1 }, */
	{ "speech_prog",	M_SPEECH_PROG		},
	{ "rand_prog",		M_RAND_PROG		},
	{ "fight_prog",		M_FIGHT_PROG		},
	{ "hitprcnt_prog",	M_HITPRCNT_PROG		},
	{ "death_prog",		M_DEATH_PROG		},
	{ "entry_prog",		M_ENTRY_PROG		},
	{ "greet_prog",		M_GREET_PROG		},
	{ "all_greet_prog",	M_ALL_GREET_PROG	},
	{ "give_prog",		M_GIVE_PROG		},
	{ "bribe_prog",		M_BRIBE_PROG		},
	{ "trig_prog",		M_TRIG_PROG		},
	{ "soc_prog",		M_SOC_PROG		},
	{ "time_prog",		M_TIME_PROG		},
	{ "tell_prog",		M_TELL_PROG		},
	{ "say_prog",		M_SAY_PROG		},
	{ "steal_prog",		M_STEAL_PROG		},
	{ "steal_fail_prog",	M_STEAL_FAIL_PROG	},
	{ "asleep_time_prog",	M_ASLEEP_TIME_PROG	}, /* Alandar 21.11.2003 */
	{ "descr_prog",		M_DESCR_PROG		}, /* Lam 7.1.2004 */
	{ "feed_prog",		M_FEED_PROG		}, /* Alandar 22.12.2004 */
	{ "water_prog",		M_WATER_PROG		}, /* jw. */
	{ "spell_victim_prog",	M_SPELL_VICTIM_PROG	}, /* Lam 23.6.2005 */
	{ "long_descr_prog",	M_LONG_DESCR_PROG	}, /* Lam 31.5.2006 */
	{ "aggr_attack_prog",	M_AGGR_ATTACK_PROG	}, /* Lam 4.1.2009 */
	{ "",			ERROR_PROG		}
    },
    { /* RPROGI */
	{ "in_file_prog",	IN_FILE_PROG		},
	{ "speech_prog",	R_SPEECH_PROG		},
	{ "greet_prog",		R_GREET_PROG		},
	{ "look_prog",		R_LOOK_PROG		},
	{ "do_prog",		R_DO_PROG		},
	{ "descr_prog",		R_DESCR_PROG		},
	{ "drop_prog",		R_DROP_PROG		}, /* Lam 9.4.2000 */
	{ "time_prog",		R_TIME_PROG		}, /* Lam 31.3.2002 */
	{ "rand_prog",		R_RAND_PROG		},
	{ "undug_prog",		R_UNDUG_PROG		}, /* Lam 17.4.2005 */
	{ "",			ERROR_PROG		}
    },
    { /* EPROGI */
	{ "in_file_prog",	IN_FILE_PROG		},
	{ "pass_prog",		E_PASS_PROG		},
	{ "open_prog",		E_OPEN_PROG		},
	{ "close_prog",		E_CLOSE_PROG		},
	{ "lock_prog",		E_LOCK_PROG		},
	{ "unlock_prog",	E_UNLOCK_PROG		},
	{ "pick_prog",		E_PICK_PROG		},
	{ "bash_prog",		E_BASH_PROG		},
	{ "dig_prog",		E_DIG_PROG		},
	{ "bounce_prog",	E_BOUNCE_PROG		},
	{ "",			ERROR_PROG		}
    },
    { /* OPROGI */
	{ "in_file_prog",	IN_FILE_PROG		},
	{ "take_prog",		O_TAKE_PROG		},
	{ "drop_prog",		O_DROP_PROG		},
	{ "wear_prog",		O_WEAR_PROG		},
	{ "remove_prog", 	O_REMOVE_PROG		},
	{ "buy_prog",		O_BUY_PROG		},
	{ "sell_prog",		O_SELL_PROG		},
	{ "use_prog",		O_USE_PROG		},
	{ "give_prog",		O_GIVE_PROG		},
	{ "put_prog",		O_PUT_PROG		},
	{ "dig_prog",		O_DIG_PROG		},
	{ "putin_prog",		O_PUTIN_PROG		},
	{ "sacr_prog",		O_SACR_PROG		},
	{ "donate_prog",	O_DONATE_PROG		},
	{ "steal_prog",		O_STEAL_PROG		},
	{ "steal_fail_prog",	O_STEAL_FAIL_PROG	},
	{ "putin_gold_prog",	O_PUTIN_GOLD_PROG	}, /* Tissarius 22.10.2001 */
	{ "open_prog",		O_OPEN_PROG		}, /* Alandar 21.12.2004 */
	{ "close_prog",		O_CLOSE_PROG		}, /* jw. */
	{ "descr_prog",		O_DESCR_PROG		}, /* Lam 10.1.2009 */
	{ "long_descr_prog",	O_LONG_DESCR_PROG	}, /* jw. */
	{ "",			ERROR_PROG		}
    }
};


/*
 * Lam 10.6.98: powyzsza tablica oraz dwie ponizsze funkcje. Wszystko napisane
 * od zera, bo stare sie do niczego nie nadawaly, bedac zestawami ifow i nie
 * potrafiac sprawdzic czy dany prog nadaje sie dla moba/pomieszczenia. Teraz
 * jest to robione.
 * Lam 10-14.7.98: jeszcze inna tablica, eprogi...
 * Lam 16.7.98: oprogi
 */
int mprog_name_to_type( char *name, int gdzie )
{
    int pzl;

    for ( pzl = 0; *progi[ gdzie ][ pzl ].name; pzl++ )
	if ( !str_cmp( name, progi[ gdzie ][ pzl ].name ) )
	    break;

    return progi[ gdzie ][ pzl ].trig; /* moze dojsc do ERROR_PROGA */
}


char *mprog_type_to_name( int type, int gdzie )
{
    int pzl;

    for ( pzl = 0; *progi[ gdzie ][ pzl ].name; pzl++ )
	if ( progi[ gdzie ][ pzl ].trig == type )
	    break;

    return (*progi[ gdzie ][ pzl ].name) ? progi[ gdzie ][ pzl ].name : "ERROR_PROG";
}


/* This routine reads in scripts of MOBprograms from a file */
MPROG_DATA *mprog_file_read( char *f, MPROG_DATA *mprg,
			    void *param, int gdzie )
{
    MOB_INDEX_DATA	*pMobIndex	= (MOB_INDEX_DATA *) param;
    ROOM_INDEX_DATA	*pRoomIndex	= (ROOM_INDEX_DATA *) param;
    OBJ_INDEX_DATA	*pObjIndex	= (OBJ_INDEX_DATA *) param;
    EXIT_DATA		*pExit		= (EXIT_DATA *) param;
    char MOBProgfile[ MAX_INPUT_LENGTH ];
    char bugbuf[ MAX_STRING_LENGTH ];
    MPROG_DATA *mprg2;
    FILE *progfile;
    char letter;
    bool done;
    bool searchdone;
    int stat; /* Lam: dla przyzwoitosci, ale nie sprawdzam tego potem :) */

    sprintf( MOBProgfile, "%s%s", MOB_DIR, f );

    progfile = fopen( MOBProgfile, "r" );
    if ( !progfile )
    {
	sprintf( bugbuf, "Couldn't open prog file '%s'.", MOBProgfile );
	cbug( bugbuf, 0 );
	return NULL;
    }

    searchdone = FALSE;
    while ( !searchdone )
    {
	letter = fread_letter( progfile );
	switch ( letter )
	{
	    case '>':
		searchdone = TRUE;
		break;
	    case '*':
		fread_to_eol( progfile );
		break;
	    case '|':
		sprintf( bugbuf, "Empty mobprog file '%s'.", MOBProgfile );
		cbug( bugbuf, 0 );
		return NULL;
	    default:
		sprintf( bugbuf, "In mobprog file syntax error (file '%s').", MOBProgfile );
		cbug( bugbuf, 0 );
		return NULL;
	}
    }

    mprg2 = mprg;
    done = FALSE;
    while ( !done )
    {
	/* Lam 10.4.2002: nowe progi nie usuwaja starego, dodatkowo pamietaja go */
	mprg2->next = new_mprog( );
	mprg2 = mprg2->next;
	mprg2->parent = mprg;
	mprg2->next = NULL;

	mprg2->type = mprog_name_to_type( fread_word( progfile, &stat, TRUE ), gdzie );
	switch ( mprg2->type )
	{
	    case ERROR_PROG:
		sprintf( bugbuf, "mobprog file (%s): type error", MOBProgfile );
		cbug( bugbuf, 0 );
		return NULL;
		break;
	    case IN_FILE_PROG:
/*		sprintf( bugbuf, "mprog file '%s' contains a call to file.", MOBProgfile );
		cbug( bugbuf, 0 );
		return NULL;
		break; */
		mprg2->arglist = fread_string( progfile, &stat );
		mprg2->comlist = str_dup( "" );
		mprg2 = mprog_file_read( mprg2->arglist, mprg2, param, gdzie );
		if ( !mprg2 )
		    return NULL;
		fread_to_eol( progfile );
		searchdone = FALSE;
		while ( !searchdone )
		{
		    letter = fread_letter( progfile );
		    switch ( letter )
		    {
			case '>':
			    searchdone = TRUE;
			    break;
			case '*':
			    fread_to_eol( progfile );
			    break;
			case '|':
			    mprg2->next = NULL;
			    fread_to_eol( progfile );
			    done = TRUE;
			    searchdone = TRUE;
			    break;
			default:
			    cbug( "Mprog_read_programs: vnum %d bad MOBPROG in file.",
				gdzie == 0 ? pMobIndex->vnum : gdzie == 1 ? pRoomIndex->vnum
				: gdzie == 3 ? pObjIndex->vnum : 0 );
			    return NULL;
		    }
		}
		break;
	    /* Lam 20.3.2005: tu zapomnial dopisac Alandar */
	    case R_RAND_PROG:
		if ( gdzie == 1 )
		{
		    RRAND_DATA *rand;
		    rand = ( RRAND_DATA * ) calloc( sizeof( RRAND_DATA ), 1 );
		    rand->next = rrand_list;
		    rand->room = pRoomIndex;
		    rrand_list = rand;
		}
	    default:
		if ( !gdzie )
		    pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
		else if ( gdzie == 1 )
		    pRoomIndex->progtypes = pRoomIndex->progtypes | mprg2->type;
		else if ( gdzie == 2 )
		    pExit->progtypes = pExit->progtypes | mprg2->type;
		else
		    pObjIndex->progtypes = pObjIndex->progtypes | mprg2->type;
		mprg2->arglist       = fread_string( progfile, &stat );
		mprg2->comlist       = fread_string( progfile, &stat );
		if ( ODPLUSKWIACZ )
		{
		    if ( gdzie == 1 && mprg2->type == R_DO_PROG )
			arg_do_proga( mprg2 );
		    interpretuj_proga( mprg2 );
		}
		searchdone = FALSE;
		while ( !searchdone )
		{
		    letter = fread_letter( progfile );
		    switch ( letter )
		    {
			case '>':
			    searchdone = TRUE;
			    break;
			case '*':
			    fread_to_eol( progfile );
			    break;
			case '|':
			    mprg2->next = NULL;
			    done = TRUE;
			    searchdone = TRUE;
			    break;
			default:
			    sprintf( bugbuf, "Mprog_read_programs: vnum %d bad MOBPROG in file '%s'.",
				gdzie == 0 ? pMobIndex->vnum : gdzie == 1 ? pRoomIndex->vnum
				: gdzie == 3 ? pObjIndex->vnum : 0, MOBProgfile );
			    cbug( bugbuf, 0 );
			    return NULL;
		    }
		}
		break;
	}
    }
    fclose( progfile );

    return mprg2;
}


/* Snarf a MOBprogram section from the area file.
 */
bool load_mobprogs( FILE *fp )
{
    MOB_INDEX_DATA *iMob;
    MPROG_DATA     *original;
    MPROG_DATA     *working;
    char            letter;
    int             value;
    int		  stat;
    bool            koniec = FALSE;

    while ( !koniec )
    {
	letter = fread_letter( fp );
	switch ( letter )
	{
	    default:
		cbug( "Load_mobprogs: bad command '%c'.", letter );
		return TRUE;
	    case 'S':
	    case 's':
		fread_to_eol( fp );
		koniec = TRUE;
		break;
	    case '*':
		fread_to_eol( fp );
		break;
	    case 'M':
	    case 'm':
		value = fread_number( fp, &stat );
		if ( ( iMob = get_mob_index( value ) ) == NULL )
		{
		    cbug( "Load_mobprogs: vnum %d doesnt exist", value );
		    return TRUE;
		}

		/* Go to the end of the prog command list if other commands
		   exist */
		if ( ( original = iMob->mobprogs ) )
		    for ( ; original->next != NULL; original = original->next )
			;

		working = new_mprog( );
		if ( original )
		    original->next = working;
		else
		    iMob->mobprogs = working;
		working = mprog_file_read( fread_word( fp, &stat, TRUE ), working, iMob, 0 );
		if ( !working )
		    return TRUE;
		working->next = NULL;
		fread_to_eol( fp );
		break;
	}
    }

    return FALSE;
}


/* This procedure is responsible for reading any in_file MOBprograms.
 */
/*
 * Obsluga pomieszczen: 29.5.98
 * Obsluga wyjsc:	11.7.98
 * Obsluga przedmiotow: 16.7.98
 */
bool mprog_read_programs( FILE *fp, void *param, int gdzie )
{
    MOB_INDEX_DATA	*pMobIndex	= (MOB_INDEX_DATA *) param;
    ROOM_INDEX_DATA	*pRoomIndex	= (ROOM_INDEX_DATA *) param;
    EXIT_DATA		*pExit		= (EXIT_DATA *) param;
    OBJ_INDEX_DATA	*pObjIndex	= (OBJ_INDEX_DATA *) param;
    MPROG_DATA *mprg;
    char letter;
    bool done = FALSE;
    int	stat;

    letter = fread_letter( fp );
    if ( letter != '>' )
    {
	cbug( "Mprog_read_programs: vnum %d MOBPROG char",
		( gdzie == 0 ) ? pMobIndex->vnum :
		( gdzie == 1 ) ? pRoomIndex->vnum :
		( gdzie == 2 ) ? 0 : pObjIndex->vnum );
	return TRUE;
    }
    if ( !gdzie )
    {
	pMobIndex->mobprogs = new_mprog( );
	mprg = pMobIndex->mobprogs;
    }
    else if ( gdzie == 1 )
    {
	pRoomIndex->mobprogs = new_mprog( );
	mprg = pRoomIndex->mobprogs;
    }
    else if ( gdzie == 2 )
    {
	pExit->mobprogs = new_mprog( );
	mprg = pExit->mobprogs;
    }
    else
    {
	pObjIndex->mobprogs = new_mprog( );
	mprg = pObjIndex->mobprogs;
    }

    while ( !done )
    {
	mprg->type = mprog_name_to_type( fread_word( fp, &stat, TRUE ), gdzie );
	switch ( mprg->type )
	{
	    case ERROR_PROG:
		cbug( "Mprog_read_programs: vnum %d [m/r/e/o]PROG type.",
		    gdzie == 0 ? pMobIndex->vnum : gdzie == 1 ? pRoomIndex->vnum
		    : gdzie == 3 ? pObjIndex->vnum : 0 );
		return TRUE;
	    case IN_FILE_PROG:
		mprg->arglist = fread_string( fp, &stat );
		mprg->comlist = str_dup( "" );
		mprg = mprog_file_read( mprg->arglist, mprg, param, gdzie );
		if ( !mprg )
		    return TRUE;
		fread_to_eol( fp );
		letter = fread_letter( fp );
		switch ( letter )
		{
		    case '>':
			mprg->next = new_mprog( );
			mprg       = mprg->next;
			mprg->next = NULL;
			break;
		    case '|':
			mprg->next = NULL;
			fread_to_eol( fp );
			done = TRUE;
			break;
		    default:
			cbug( "Mprog_read_programs: vnum %d bad MOBPROG in file.",
			  gdzie == 0 ? pMobIndex->vnum : gdzie == 1 ? pRoomIndex->vnum
			  : gdzie == 3 ? pObjIndex->vnum : 0 );
			return TRUE;
			break;
		}
		break;
	    /* Alandar 03.02.2004: rand_prog dla pomieszczen */
	    case R_RAND_PROG:
		if ( gdzie == 1 )
		{
		    RRAND_DATA *rand;
		    rand = ( RRAND_DATA * ) calloc( sizeof( RRAND_DATA ), 1 );
		    rand->next = rrand_list;
		    rand->room = pRoomIndex;
		    rrand_list = rand;
		}
	    default:
		if ( !gdzie )
		    pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
		else if ( gdzie == 1 )
		    pRoomIndex->progtypes = pRoomIndex->progtypes | mprg->type;
		else if ( gdzie == 2 )
		    pExit->progtypes = pExit->progtypes | mprg->type;
		else
		    pObjIndex->progtypes = pObjIndex->progtypes | mprg->type;
		mprg->arglist        = fread_string( fp, &stat );
		fread_to_eol( fp );
		mprg->comlist        = fread_string( fp, &stat );
		if ( ODPLUSKWIACZ )
		{
		    if ( gdzie == 1 && mprg->type == R_DO_PROG )
			arg_do_proga( mprg );
		    interpretuj_proga( mprg );
		}
		fread_to_eol( fp );
		letter = fread_letter( fp );
		switch ( letter )
		{
		    case '>':
			mprg->next = new_mprog( );
			mprg       = mprg->next;
			mprg->next = NULL;
			break;
		    case '|':
			mprg->next = NULL;
			fread_to_eol( fp );
			done = TRUE;
			break;
		    default:
			cbug( "Mprog_read_programs: vnum %d bad MOBPROG (syntax error).",
			  gdzie == 0 ? pMobIndex->vnum : gdzie == 1 ? pRoomIndex->vnum
			  : gdzie == 3 ? pObjIndex->vnum : 0 );
			return TRUE;
		}
		break;
	}
    }

    return FALSE;
}


/*
 * Uzywane tylko do sprawdzania poprawnosci progow z wlaczonym odpluskwiaczem.
 *
 * Mozliwe jest zrobienie z tej funkcji rozbijacza na kolejne polecenia wraz
 * z interpretowaniem ich jednokrotnie i zapamietywaniem w strukturach, co
 * trzeba uruchomic z jakimi parametrami. Jednak operacje na lancuchach
 * (mprog_next_command, interpretuj_zmysly, oblicz_wyrazenie) sa naprawde
 * szybkie. To, ze mozna je przyspieszyc tysiac razy nie zmienia faktu, ze sa
 * rzeczy bardziej warte optymalizacji, przynajmniej na razie :)
 */
void interpretuj_proga( MPROG_DATA *prog )
{
    char linia[ MSL ]; /* na wypadek przepelnienia */
    char pol[ MIL ];
    char *p, *q;
    int zagniezdzenie;
    bool towhile[ 100 ]; /* to if czy while? towhile[ 0 ] nigdy nieuzywane */

/* TODO: sprawdzanie prog->arglist */

    p = prog->comlist;
    zagniezdzenie = 0;

    if ( !*p )
	cbug( "pusta tresc proga", 0 );

    while ( *p )
    {
	p = mprog_next_command( p, linia );
	if ( strlen( linia ) >= MIL )
	{
	    cbug( "za dluga linia proga (maksimum %d znakow)", MIL - 1 );
	    return;
	}
	q = one_argument( linia, pol );
	if ( !str_cmp( pol, "if" ) )
	{
	    zagniezdzenie++;
	    if ( zagniezdzenie >= 100 )
	    {
		bug( "interpretuj_proga nie obsluguje tak glebokich zagniezdzen", 0 );
		return;
	    }
	    towhile[ zagniezdzenie ] = FALSE;
	}
	else if ( !str_cmp( pol, "else" ) )
	{
	    if ( !zagniezdzenie )
	    {
		cbug( "else na najwyzszym poziomie proga (brak if)", 0 );
		return;
	    }
	    else if ( towhile[ zagniezdzenie ] )
	    {
		cbug( "else wewnatrz while", 0 );
		return;
	    }
	}
	else if ( !str_cmp( pol, "endif" ) )
	{
	    if ( !zagniezdzenie )
	    {
		cbug( "endif na najwyzszym poziomie proga (brak if)", 0 );
		return;
	    }
	    else if ( towhile[ zagniezdzenie ] )
	    {
		cbug( "endif wewnatrz while", 0 );
		return;
	    }
	    else
		zagniezdzenie--;
	}
	else if ( !str_cmp( pol, "while" ) )
	{
	    zagniezdzenie++;
	    if ( zagniezdzenie >= 100 )
	    {
		bug( "interpretuj_proga nie obsluguje tak glebokich zagniezdzen", 0 );
		return;
	    }
	    towhile[ zagniezdzenie ] = TRUE;
	}
	else if ( !str_cmp( pol, "endwhile" ) )
	{
	    if ( !zagniezdzenie )
	    {
		cbug( "endwhile na najwyzszym poziomie proga (brak while)", 0 );
		return;
	    }
	    else if ( !towhile[ zagniezdzenie ] )
	    {
		cbug( "endwhile wewnatrz if", 0 );
		return;
	    }
	    else
		zagniezdzenie--;
	}
	else if ( !str_cmp( pol, "break" ) )
	{
	    if ( !zagniezdzenie )
	    {
		cbug( "break na najwyzszym poziomie proga (poza if/while)", 0 );
		return;
	    }
	}
	else if ( !str_cmp( pol, "mpasound" ) )
	{
	    one_argument( q, pol );
	    if ( !interpretuj_zmysly( NULL, pol ) )
	    {
		cbug( "brak jakichkolwiek zmyslow dla mpasound. Nie sprawdzam dalszej czesci proga.", 0 );
		return;
	    }
	}
	else if ( !str_cmp( pol, "mpechoaround" ) )
	{
	    q = one_argument( q, pol );
	    one_argument( q, pol );
	    if ( !interpretuj_zmysly( NULL, pol ) )
	    {
		cbug( "brak jakichkolwiek zmyslow dla mpechoaround. Nie sprawdzam dalszej czesci proga.", 0 );
		return;
	    }
	}
	else if ( !str_cmp( pol, "mpechoat" ) )
	{
	    q = one_argument( q, pol );
	    one_argument( q, pol );
	    if ( !interpretuj_zmysly( NULL, pol ) )
	    {
		cbug( "brak jakichkolwiek zmyslow dla mpechoat. Nie sprawdzam dalszej czesci proga.", 0 );
		return;
	    }
	}
	else if ( !str_cmp( pol, "mpecho" ) )
	{
	    one_argument( q, pol );
	    if ( !interpretuj_zmysly( NULL, pol ) )
	    {
		cbug( "brak jakichkolwiek zmyslow dla mpecho. Nie sprawdzam dalszej czesci proga.", 0 );
		return;
	    }
	}
    }

    /* podaje wszystkie brakujace zamkniecia if/while, w kolejnosci, w ktorej
       powinny wystapic w tresci proga; przewaznie zamiast tego zadziala
       wykrywacz endifa w petli lub endwhile w warunku, ale to tez dziala */
    if ( zagniezdzenie )
    {
	strcpy( linia, "brak " );
	for ( ; zagniezdzenie; zagniezdzenie-- )
	    if ( towhile[ zagniezdzenie ] )
		strcat( linia, "endwhile, " );
	    else
		strcat( linia, "endif, " );
	linia[ strlen( linia ) - 2 ] = '\0';
	cbug( linia, 0 );
    }

    return;
}


void arg_do_proga( MPROG_DATA *prog )
{
    char typ[ MIL ];
    char nazwa[ MIL ];
    char argument[ MIL ];
    char *tmp;

    tmp = prog->arglist;
    tmp = one_argument( tmp, typ );
    tmp = one_argument( tmp, nazwa );
    tmp = one_argument( tmp, argument );

    if ( strlen( typ ) != 1 )
	cbug( "do_prog blednego typu (ma miec jedna litere)", 0 );

    if ( !nazwa[ 0 ] )
    {
	cbug( "do_prog musi miec nazwe polecenia!", 0 );
	return;
    }

    switch ( typ[ 0 ] )
    {
	default:
	    cbug( "nie obslugiwany typ do_proga", 0 );
	    break;
	case '-':
	case 'n':
	case 'o':
	case 'i':
	case 'c':
	    break;
	case 's':
	case 'p':
	    if ( !argument[ 0 ] )
		cbug( "brak argumentu dla do_proga s/p", 0 );
	    else if ( !*tmp )
		cbug( "brak komunikatu nie trafienia w argument dla do_proga s/p", 0 );
	    break;
    }

    return;
}
