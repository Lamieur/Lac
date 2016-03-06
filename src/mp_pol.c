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
 *  Obsluga wbudowanych polecen progow.                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "merc.h"
#include "mp_wyzw.h"


CHAR_DATA	*get_char_prog		args( ( CHAR_DATA *ch, char *argument,
						CHAR_DATA *actor, OBJ_DATA *obj,
						CHAR_DATA *rndm, CHAR_DATA *vict,
						OBJ_DATA *v_obj ) );
CHAR_DATA	*get_char_room_prog 	args( ( CHAR_DATA *ch, char *argument,
						CHAR_DATA *actor, OBJ_DATA *obj,
						CHAR_DATA *rndm, CHAR_DATA *vict,
						OBJ_DATA *v_obj ) );
OBJ_DATA	*get_obj_prog		args( ( CHAR_DATA *ch, char *argument,
						CHAR_DATA *actor, OBJ_DATA *obj,
						CHAR_DATA *rndm, CHAR_DATA *vict,
						OBJ_DATA *v_obj ) );
OBJ_DATA	*get_obj_here_prog 	args( ( CHAR_DATA *ch, char *argument,
						CHAR_DATA *actor, OBJ_DATA *obj,
						CHAR_DATA *rndm, CHAR_DATA *vict,
						OBJ_DATA *v_obj ) );
ROOM_INDEX_DATA	*find_location_prog	args( ( CHAR_DATA *ch, char *argument,
						CHAR_DATA *actor, OBJ_DATA *obj,
						CHAR_DATA *rndm, CHAR_DATA *vict,
						OBJ_DATA *v_obj ) );
char		*mp_tr_pl		args( ( CHAR_DATA *mob, char znak,
						bool widzi ) );
char		*mp_tr_pl_obj		args( ( OBJ_DATA *obj, char znak,
						bool widzi ) );
bool		mprog_translate_one	args( ( char *ch, char *t, CHAR_DATA *obser,
						CHAR_DATA *mob,	CHAR_DATA *actor,
						OBJ_DATA *obj, CHAR_DATA *rndm,
						CHAR_DATA *vict, OBJ_DATA *v_obj ) );
void		mprog_translate		args( ( char *from, char *to,
						CHAR_DATA *obser, CHAR_DATA *mob,
						CHAR_DATA *actor, OBJ_DATA *obj,
						CHAR_DATA *rndm, CHAR_DATA *vict,
						OBJ_DATA *v_obj ) );
char		*return_field		args( ( CHAR_DATA *ch, char *arg ) );
int		mprog_licz		args( ( char *str, char *t, CHAR_DATA *mob,
						CHAR_DATA *actor, OBJ_DATA *obj,
						CHAR_DATA *rndm, CHAR_DATA *vict,
						OBJ_DATA *v_obj ) );
void		mprog_przelicz		args( ( char *buf, CHAR_DATA *mob,
						CHAR_DATA *actor, OBJ_DATA *obj,
						CHAR_DATA *rndm, CHAR_DATA *vict,
						OBJ_DATA *v_obj ) );
int		interpretuj_zmysl	args( ( CHAR_DATA *ch,
						const char *argument ) );
bool		czy_wyswietlic		args( ( CHAR_DATA *komu, int zmysly,
						CHAR_DATA *ch, CHAR_DATA *actor,
						OBJ_DATA *obj, CHAR_DATA *rndm,
						CHAR_DATA *vict, OBJ_DATA *v_obj ) );

MTRIG_DATA *mtrig_list;
RRAND_DATA *rrand_list;

DECLARE_PROG_FUN( do_mpasound		);
DECLARE_PROG_FUN( do_mpat		);
DECLARE_PROG_FUN( do_mpbeep		);		/* Lam */
DECLARE_PROG_FUN( do_mpbug		);		/* Lam */
DECLARE_PROG_FUN( do_mpcast		);		/* Lam */
DECLARE_PROG_FUN( do_mpchalign		);		/* Lam */
DECLARE_PROG_FUN( do_mpcomeback		);		/* Lam */
DECLARE_PROG_FUN( do_mpcrithit		);		/* Lam */
DECLARE_PROG_FUN( do_mpdamage		);		/* Qwert */
DECLARE_PROG_FUN( do_mpdelexit		);		/* Lam */
DECLARE_PROG_FUN( do_mpdontcomeback	);		/* Lam */
DECLARE_PROG_FUN( do_mpdrop		);		/* Qwert */
DECLARE_PROG_FUN( do_mpecho		);
DECLARE_PROG_FUN( do_mpechoaround	);
DECLARE_PROG_FUN( do_mpechoaround_n	);		/* Lam */
DECLARE_PROG_FUN( do_mpechoat		);
DECLARE_PROG_FUN( do_mpechoat_n		);		/* Lam */
DECLARE_PROG_FUN( do_mpendquest		);		/* Lam */
DECLARE_PROG_FUN( do_mpforce		);
DECLARE_PROG_FUN( do_mpgive		);		/* Tissarius */
DECLARE_PROG_FUN( do_mpgoto		);
DECLARE_PROG_FUN( do_mpjunk		);
DECLARE_PROG_FUN( do_mpkill		);
DECLARE_PROG_FUN( do_mplag		);		/* Ulryk */
DECLARE_PROG_FUN( do_mpmakeexit		);		/* Lam */
DECLARE_PROG_FUN( do_mpmaketrap		);		/* Lam */
DECLARE_PROG_FUN( do_mpmasscrithit	);		/* Lam */
DECLARE_PROG_FUN( do_mpmassslay		);		/* Lam */
DECLARE_PROG_FUN( do_mpmload		);
DECLARE_PROG_FUN( do_mpoload		);
DECLARE_PROG_FUN( do_mppeace		);		/* Lam */
DECLARE_PROG_FUN( do_mppolymorph	);		/* Qwert */
DECLARE_PROG_FUN( do_mppurge		);
DECLARE_PROG_FUN( do_mpqp		);		/* Lam */
DECLARE_PROG_FUN( do_mpquest		);		/* Lam */
DECLARE_PROG_FUN( do_mpquestdata	);		/* Lam */
DECLARE_PROG_FUN( do_mprestore		);		/* Lam */
DECLARE_PROG_FUN( do_mpsetdoor		);		/* Lam */
DECLARE_PROG_FUN( do_mpsetowner		);		/* Ulryk */
DECLARE_PROG_FUN( do_mpslay		);		/* Lam i Grzechu */
DECLARE_PROG_FUN( do_mpstepto		);		/* Lam dla Ulryka */
DECLARE_PROG_FUN( do_mptransfer		);
DECLARE_PROG_FUN( do_mptransfer_n	);		/* Lam */
DECLARE_PROG_FUN( do_mptrigger		);		/* Lam */
DECLARE_PROG_FUN( do_mpuntrap		);		/* Lam */
DECLARE_PROG_FUN( do_mpquesttitle	);		/* Lam */
DECLARE_PROG_FUN( do_mpquestdescr	);		/* Lam */

const struct prog_cmd_type prog_cmd_table[ ] =
{
    { "mpasound",       do_mpasound		},
    { "mpat",           do_mpat			},
    { "mpbeep",		do_mpbeep		},
    { "mpbug",		do_mpbug		},
    { "mpcast",		do_mpcast		},
    { "mpchalign",	do_mpchalign		},
    { "mpcomeback",	do_mpcomeback		},
    { "mpcrithit",	do_mpcrithit		},
    { "mpdamage",	do_mpdamage		},
    { "mpdelexit",	do_mpdelexit		},
    { "mpdontcomeback",	do_mpdontcomeback	},
    { "mpdrop",		do_mpdrop		},
    { "mpecho",         do_mpecho		},
    { "mpechoat",       do_mpechoat		},
    { "mpechoat_n",     do_mpechoat_n		},
    { "mpechoaround",   do_mpechoaround		},
    { "mpechoaround_n", do_mpechoaround_n	},
    { "mpendquest",	do_mpendquest		},
    { "mpforce",        do_mpforce		},
    { "mpgive",         do_mpgive		},
    { "mpgoto",         do_mpgoto		},
    { "mpjunk",         do_mpjunk		},
    { "mpkill",         do_mpkill		},
    { "mplag",          do_mplag		},
    { "mpmakeexit",	do_mpmakeexit		},
    { "mpmaketrap",	do_mpmaketrap		},
    { "mpmasscrithit",	do_mpmasscrithit	},
    { "mpmassslay",	do_mpmassslay		},
    { "mpmload",        do_mpmload		},
    { "mpoload",        do_mpoload		},
    { "mppeace",        do_mppeace		},
    { "mppurge",        do_mppurge		},
    { "mppolymorph",    do_mppolymorph		},
    { "mpqp",		do_mpqp			},
    { "mpquest",	do_mpquest		},
    { "mpquestdata",	do_mpquestdata		},
    { "mpquesttitle",	do_mpquesttitle		},
    { "mpquestdescr",	do_mpquestdescr		},
    { "mprestore",	do_mprestore		},
/*    { "mpsetdoor",	do_mpsetdoor		}, napisze to kiedys */
    { "mpsetowner",	do_mpsetowner		},
    { "mpslay",		do_mpslay		},
    { "mpstepto",	do_mpstepto		},
    { "mptransfer",	do_mptransfer		},
    { "mptransfer_n",	do_mptransfer_n		},
    { "mptrigger",	do_mptrigger		},
    { "mpuntrap",	do_mpuntrap		},
    { "",		NULL			}
};


/*
 * Lam 31.12.2003
 * w przypadku blednego $* przyjmuje $i, co moze powodowac zupelnie
 * niespodziewane dla autora dzialanie proga (oczywiscie tylko niepoprawnego)
 */
CHAR_DATA *get_char_prog( CHAR_DATA *ch, char *argument,
		CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    TYP_ZMIENNEJ *zm;

    if ( !*argument )
	return NULL;

    if ( *argument == '$' )
    {
	if ( strlen( argument ) > 2 )
	{
	    progbug( ch, "get_char_prog: bledna zmienna wbudowana '%s'", argument );
	    return ch;
	}
	if ( argument[ 1 ] == 'n' && actor )
	    return actor;
	if ( argument[ 1 ] == 't' && vict )
	    return vict;
	if ( argument[ 1 ] == 'r' && rndm )
	    return rndm;
	if ( argument[ 1 ] != 'i' )
	    progbug( ch, "get_char_prog: bledna zmienna wbudowana '%s'", argument );
	return ch;
    }

    zm = znajdz_zmienna_moba( ch, argument );

    if ( !zm )
	return NULL;
    else if ( zm->typ == TZ_POST )
	return (CHAR_DATA *) zm->wartosc.p;
    else
	progbug( ch, "get_char_prog: zmienna '%s' nie typu postaciowego", argument );

    return NULL;
}


/*
 * Lam 31.12.2003
 */
CHAR_DATA *get_char_room_prog( CHAR_DATA *ch, char *argument,
		CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    CHAR_DATA *rch;
    char       arg[ MAX_INPUT_LENGTH ];
    int        number;
    int        count;

    rch = get_char_prog( ch, argument, actor, obj, rndm, vict, v_obj );
    if ( rch )
	return rch;

    number = number_argument( argument, arg );
    count  = 0;
    if ( !str_cmp( arg, "si`e" ) || !str_cmp( arg, "siebie" ) )
	return ch;
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( !can_see( ch, rch ) || !is_name( arg, rch->name ) )
	    continue;
	if ( ++count == number )
	    return rch;
    }

    return NULL;
}


/*
 * Tu sobie nie moglem pozwolic na zwracanie $i, z wiadomych powodow, wiec
 * blad bedzie sygnalizowany dwukrotnie (tu, ze bledny $* i potem
 * w mppoleceniu, ze nie znalazl przedmiotu)
 */
OBJ_DATA *get_obj_prog( CHAR_DATA *ch, char *argument,
		CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    TYP_ZMIENNEJ *zm;

    if ( !*argument )
	return NULL;

    if ( *argument == '$' )
    {
	if ( strlen( argument ) > 2 )
	{
	    progbug( ch, "get_obj_prog: bledna zmienna wbudowana '%s'", argument );
	    return NULL;
	}
	if ( argument[ 1 ] == 'o' )
	    return obj;
	if ( argument[ 1 ] == 'p' )
	    return v_obj;
	progbug( ch, "get_obj_prog: bledna zmienna wbudowana '%s'", argument );
    }

    zm = znajdz_zmienna_moba( ch, argument );

    if ( !zm )
	return NULL;
    else if ( zm->typ == TZ_PRZE )
	return (OBJ_DATA *) zm->wartosc.p;
    else
	progbug( ch, "get_char_prog: zmienna '%s' nie typu przedmiotowego", argument );

    return NULL;
}


/*
 * Lam 31.12.2003
 */
OBJ_DATA *get_obj_here_prog( CHAR_DATA *ch, char *argument,
		CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    OBJ_DATA *tobj;

    if ( ( tobj = get_obj_prog( ch, argument, actor, obj, rndm, vict, v_obj ) ) )
	return tobj;

    if ( ( tobj = get_obj_list( ch, argument, ch->in_room->contents ) ) )
	return tobj;

    if ( ( tobj = get_obj_carry( ch, argument ) ) )
	return tobj;

    if ( ( tobj = get_obj_wear( ch, argument ) ) )
	return tobj;

    return NULL;
}


/*
 * Lam 1.1.2003
 */
ROOM_INDEX_DATA *find_location_prog( CHAR_DATA *ch, char *argument,
		CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    CHAR_DATA *victim;
    TYP_ZMIENNEJ *zm;

    /* Lam 8.11.2005: zmienna vnum typu calkowitego zamiast mpgoto %(vnum) */
    zm = znajdz_zmienna_moba( ch, argument );
    if ( zm && zm->typ == TZ_CALK )
	return get_room_index( zm->wartosc.i );

    if ( ( victim = get_char_prog( ch, argument, actor, obj, rndm, vict, v_obj ) ) )
	return victim->in_room;

    return find_location( ch, argument );
}


/*
 * Lam 13.6.98: jest to polski dodatek do mprog_translate
 */
char *mp_tr_pl( CHAR_DATA *mob, char znak, bool widzi )
{
    switch ( znak )
    {
	case 'D': return widzi ? mob->dopelniacz  : "kogo`s";
	case 'C': return widzi ? mob->celownik    : "komu`s";
	case 'B': return widzi ? mob->biernik     : "kogo`s";
	case 'N': return widzi ? mob->narzednik   : "kim`s";
	case 'M': return widzi ? mob->miejscownik : "kim`s";
	case 'W': return widzi ? mob->wolacz      : "ktosiu";
	default:  return NULL;
    }

    return NULL;
}


/*
 * wersja dla przedmiotow
 */
char *mp_tr_pl_obj( OBJ_DATA *obj, char znak, bool widzi )
{
    switch ( znak )
    {
	case 'D': return widzi ? obj->dopelniacz  : "czego`s";
	case 'C': return widzi ? obj->celownik    : "czemu`s";
	case 'B': return widzi ? obj->biernik     : "co`s";
	case 'N': return widzi ? obj->narzednik   : "czym`s";
	case 'M': return widzi ? obj->miejscownik : "czym`s";
	default:  return NULL;
    }

    return NULL;
}


/* Lam 13.6.98: makro pomocne ponizej */
#define MPTRPL( kto, widzi )					\
{								\
    polpom = mp_tr_pl( kto, *( ch + 1 ), widzi );		\
    if ( polpom )						\
    {								\
	strcpy( t, polpom );					\
	return TRUE;						\
    }								\
    else							\
    {								\
	strcpy( t, widzi ? kto->short_descr : "kto`s" );	\
	return FALSE;						\
    }								\
}

/* rowniez wersja dla przedmiotow */
#define MPTRPLO( przedmiot )					\
if ( przedmiot )						\
{								\
    widzi = can_see_obj( obser, przedmiot );			\
    polpom = mp_tr_pl_obj( przedmiot, *( ch + 1 ), widzi );	\
    if ( polpom )						\
    {								\
	strcpy( t, polpom );					\
	return TRUE;						\
    }								\
    else							\
    {								\
	strcpy( t, widzi ? przedmiot->short_descr : "co`s" );	\
	return FALSE;						\
    }								\
}


/* This routine handles the variables for command expansion.
 * If you want to add any go right ahead, it should be fairly
 * clear how it is done and they are quite easy to do, so you
 * can be as creative as you want. The only catch is to check
 * that your variables exist before you use them. At the moment,
 * using $t when the secondary target refers to an object
 * i.e. >prog_act drops~<nl>if ispc($t)<nl>sigh<nl>endif<nl>~<nl>
 * probably makes the mud crash (vice versa as well) The cure
 * would be to change act() so that vo becomes vict & v_obj.
 * but this would require a lot of small changes all over the code.
 *
 * To, co jest napisane powyzej, to bzdura :)  Ale i tak nie mam act_proga.
 *
 * Lam 30.5.98 - zabezpieczenia przed NULL
 *     31.5.98 - wywalenie polowy rzeczy bo np. byly angielskie
 *     13.6.98 - przypadki, bool zamiast void, char* zamiast char itp.
 *
 * zwraca TRUE jesli pobrala dodatkowa litere (np. $ND)
 */
bool mprog_translate_one( char *ch, char *t, CHAR_DATA *obser, CHAR_DATA *mob,
		CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    char	*polpom = NULL;
    bool	widzi = TRUE;

    *t = '\0';
    switch ( *ch ) {
	case 'i':
	    if ( can_see( obser, mob ) )
		strcpy( t, name_expand( obser, mob ) );
	    else
		strcpy( t, "kto`s" );
	    break;

	case 'I':
	    widzi = can_see( obser, mob );
	    MPTRPL( mob, widzi );
	    break;

	case 'n':
	    if ( actor )
	    {
		if ( can_see( obser, actor ) )
		    strcpy( t, name_expand( obser, actor ) );
		    /* wczesniej: one_argument( actor->name, t ); */
		else
		    strcpy( t, "kto`s" );
	    }
	    break;

	case 'N': /* kiedys to bylo imie + tytul  */
	    if ( actor )
	    {
		widzi = can_see( obser, actor );
		MPTRPL( actor, widzi );
	    }
	    break;

	case 't':
	    if ( vict )
	    {
		if ( can_see( obser, vict ) )
		    one_argument( vict->name, t );
		else
		    strcpy( t, "kto`s" );
	    }
	    break;

	case 'T':
	    if ( vict )
	    {
		widzi = can_see( obser, vict );
		MPTRPL( vict, widzi );
	    }
	    break;

	case 'r':
	    if ( rndm )
	    {
		if ( can_see( obser, rndm ) )
		    one_argument( rndm->name, t );
		else
		    strcpy( t, "kto`s" );
	    }
	    break;

	case 'R':
	    if ( rndm )
	    {
	       widzi = can_see( obser, rndm );
	       MPTRPL( rndm, widzi );
	    }
	    break;

	case 'o':
	    if ( obj )
		can_see_obj( obser, obj ) ? one_argument( obj->name, t )
				      : strcpy( t, "co`s" );
	    break;

	case 'O':
	    MPTRPLO( obj );
	    break;

	case 'p':
	    if ( v_obj )
		can_see_obj( obser, v_obj ) ? one_argument( v_obj->name, t )
					: strcpy( t, "co`s" );
	    break;

	case 'P':
	    MPTRPLO( v_obj );
	    break;

	case '$':
	    strcpy( t, "$" );
	    break;

	default:
	    progbug( mob, "zla $zmienna: $%c", *ch );
	    break;
    }

    return FALSE;
}


void mprog_translate( char *from, char *to, CHAR_DATA *obser, CHAR_DATA *mob,
		CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    char tmp[ MIL ];
    char *i;

    while ( *from )
    {
	if ( *from == '$' )
	{
	    from++;
	    if ( mprog_translate_one( from, tmp, obser, mob, actor, obj, rndm, vict, v_obj ) )
		from++;
	    i = tmp;
	    from++;
	    while ( ( *to = *i ) )
		++to, ++i;
	    continue;
	}
	*to++ = *from++;
    }
    *to = '\0';

    return;
}


/*
 * Lam 17.7.99
 * Lam 27.10.99: dodana obsluga zmiennych
 */
char *return_field( CHAR_DATA *ch, char *arg )
{
    struct
    {
	char *slowo;
	int *adres;
    } pola[ ] =
    {
	{ "level",		&ch->level		},
	{ "trust",		&ch->trust		},
	{ "hit",		&ch->hit		},
	{ "max_hit",		&ch->max_hit		},
	{ "mana",		&ch->mana		},
	{ "max_mana",		&ch->max_mana		},
	{ "move",		&ch->move		},
	{ "max_move",		&ch->max_move		},
	{ "gold",		&ch->gold		},
	{ "exp",		&ch->exp		},
	{ "saving_throw",	&ch->saving_throw	},
	{ "hitroll",		&ch->hitroll		},
	{ "damroll",		&ch->damroll		},
	{ "armor",		&ch->armor		},
	{ "wimpy",		&ch->wimpy		},
	{ "practice",		&ch->practice		},
	{ "alignment",		&ch->alignment		},
	{ "sex",		(signed int *) &ch->sex	},
	{ "position",		&ch->position		},
	{ "room.vnum",		&ch->in_room->vnum	},
	{ "room.pojemnosc",	&ch->in_room->pojemnosc	},
	{ "room.light",		&ch->in_room->light	},
	{ "room.sector_type",	&ch->in_room->sector_type },
	{ "room.area.nplayer",	&ch->in_room->area->nplayer },
	{ "room.area.age",	&ch->in_room->area->age	},
	{ NULL,			NULL,			}
    };
    static char zwrot[ MIL * 2 ];
    int i;
    TYP_ZMIENNEJ *zm = znajdz_zmienna_moba( ch, arg );

    if ( !zm )
    {
	for ( i = 0; pola[ i ].slowo; i++ )
	    if ( !str_cmp( pola[ i ].slowo, arg ) )
	    {
		sprintf( zwrot, "%d", *pola[ i ].adres );
		return zwrot;
	    }
    }
    else if ( zm->typ == TZ_CALK )
    {
	sprintf( zwrot, "%d", zm->wartosc.i );
	return zwrot;
    }
    else if ( zm->typ == TZ_ZNAK )
    {
	strcpy( zwrot, ( char * ) zm->wartosc.p );
	return zwrot;
    }
    else if ( zm->typ == TZ_POST )
    {
	strcpy( zwrot, name_expand( ch, ( CHAR_DATA * ) zm->wartosc.p ) );
	return zwrot;
    }
    else if ( zm->typ == TZ_PRZE )
    {
	strcpy( zwrot,  name_expand_obj( ch, ( OBJ_DATA * ) zm->wartosc.p ) );
	return zwrot;
    }
    else
	progbug( ch, "return_field: nie obslugiwana sytuacja" );

    return "(nieznane)";
}


/*
 * Lam 24.7.98 - rozpoczecie prac, %p
 * Lam    6.99 - %l, %{}, %<>, %[]
 * Lam 17.7.99 - %()
 */
int mprog_licz( char *str, char *t, CHAR_DATA *mob, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
			OBJ_DATA *v_obj )
{
    char pomoc[ 10 ][ MIL ];
    int ile;
    int lnik = 1;

    *t = '\0';
    switch ( *str )
    {
	case '\0':
		strcpy( t, "%" );
		return 0;
	case '%':
		strcpy( t, "%" );
		return 1;
	case 'p':
		sprintf( t, "%d", number_percent( ) );
		return 1;
	case 'l':
		str++;
		lnik++;
		if ( *str == 'i' )
		    sprintf( t, "%d", mob->level );
		else if ( *str == 'n' )
		    sprintf( t, "%d", actor ? actor->level : -1 );
		else if ( *str == 't' )
		    sprintf( t, "%d", vict ? vict->level : -1 );
		return lnik;
	case '{':
		str++;
		lnik++;
		for ( ile = 0; ile < 10; ile++ )
		    pomoc[ ile ][ 0 ] = '\0';
		ile = 0;

		while ( *str != '}' )
		{
		    if ( !*str )
		    {
			progbug( mob, "mprog_licz: %%{}: brak nawiasu zamykajacego!" );
			return 0; /* to moze brzydko wygladac! */
		    }
		    if ( *str == ';' && ile < 9 )
			ile++;
		    else
		    {
			pomoc[ ile ][ strlen( pomoc[ ile ] ) + 1 ] = '\0';
			pomoc[ ile ][ strlen( pomoc[ ile ] ) ] = *str;
		    }
		    str++;
		    lnik++;
		}
		strcpy( t, pomoc[ number_range( 0, ile ) ] );
		return lnik;
	case '<':
		str++;
		lnik++;
		for ( ile = 0; ile < 10; ile++ )
		    pomoc[ ile ][ 0 ] = '\0';
		ile = 0;

		while ( *str != '>' )
		{
		    if ( !*str )
		    {
			progbug( mob, "mprog_licz: %%<>: brak nawiasu zamykajacego!" );
			return 0; /* to moze brzydko wygladac! */
		    }
		    if ( *str == ';' && ile < 9 )
			ile++;
		    else
		    {
			pomoc[ ile ][ strlen( pomoc[ ile ] ) + 1 ] = '\0';
			pomoc[ ile ][ strlen( pomoc[ ile ] ) ] = *str;
		    }
		    str++;
		    lnik++;
		}
		if ( ile != 1 )
		    progbug( mob, "mprog_licz: %%<>: zla ilosc parametrow" );
		sprintf( t, "%d", number_range( atoi( pomoc[ 0 ] ),
			atoi( pomoc[ 1 ] ) ) );
		return lnik;
	case '[':
	    {
		/* Lam 13.12.2001 */
		progbug( mob, "mprog_licz: %%[]: %%[] nie jest juz obslugiwane, uzywaj int/aint!" );
		return 0; /* to bedzie psulo progi, ktore tego uzywaja */
	    }
	case '(':
	    {
		/* docelowy format: n.class */
		char bufor[ MSL ];
		CHAR_DATA *sprawdzany;
		char *bwsk = bufor;

		str++;
		lnik++;
		while ( *str != ')' )
		{
		    if ( !*str )
		    {
			progbug( mob, "mprog_licz: %%(): brak nawiasu zamykajacego!" );
			return lnik;
		    }
		    *bwsk++ = *str;
		    str++;
		    lnik++;
		}
		*bwsk = '\0';

		if ( !bufor[ 0 ] )
		{
		    progbug( mob, "mprog_licz: %%(): pusta zawartosc nawiasow!" );
		    return lnik;
		}

		if ( bufor[ 1 ] == '.' )
		{
		    switch ( bufor[ 0 ] )
		    {
			case 'i': sprawdzany = mob;
			    break;
			case 'n': sprawdzany = actor;
			    break;
			default: progbug( mob, "mprog_licz: %%(): zly argument [nie n/i]" );
			    return lnik;
		    }
		    strcpy( t, return_field( sprawdzany, &bufor[ 2 ] ) );
		}
		else
		    strcpy( t, return_field( mob, bufor ) );

		return lnik;
	    }
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
		str++;
		lnik++;
		if ( *str != ',' )
		{
		    progbug( mob, "mprog_licz: %%a,bn: brak przecinka" );
		    return lnik;
		}
		progbug( mob, "mprog_licz: %%a,bn: Lam to len i nie skonczyl tego" );
		return lnik;
    }
    progbug( mob, "mprog_licz: nieznany parametr: %%%c", *str );

    return 0;
}


/*
 * Wydzielone z mprog_process_cmnd 5.1.2003
 */
void mprog_przelicz( char *buf, CHAR_DATA *mob, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
			OBJ_DATA *v_obj )
{
    char tmp[ MSL ];
    char temp[ MSL ];
    char *str;
    int pzl;
    int op;

    /* Tu mialem problem. % mozna zagniezdzac, wiec obsluguje je od konca do
       poczatku. Jednak to uniemozliwialo wprowadzenia %% zamiast pojedynczego
       % w tekscie proga. Dlatego taka sztuczka: najpierw zamieniam %% na \222,
       nastepnie obrabiam wszystkie %, a potem przerabiam \222 na pojedyncze %. */
    str = buf;
    while ( *str )
    {
	if ( str[ 0 ] == '%' && str[ 1 ] == '%' )
	{
	    *str = '\222';
	    strcpy( temp, &str[ 2 ] );
	    strcpy( &str[ 1 ], temp );
	}
	str++;
    }

    for ( pzl = strlen( buf ) - 1; pzl >= 0; pzl-- )
	if ( buf[ pzl ] == '%' )
	{
	    op = mprog_licz( &buf[ pzl + 1 ], tmp, mob, actor, obj, rndm, vict, v_obj );
	    /* Lam 22.12.2005: pokaze takze zmienna wartosci "" */
	    if ( op || tmp[ 0 ] )
	    {
		buf[ pzl ] = '\0';
		strcpy( temp, &buf[ pzl + 1 + op ] );
		strcat( buf, tmp );
		strcat( buf, temp );
	    }
	}
	else if ( buf[ pzl ] == '\222' )
	    buf[ pzl ] = '%';

    return;
}


/* This procedure simply copies the cmnd to a buffer while expanding
 * any variables by calling the translate procedure.  The observant
 * code scrutinizer will notice that this is taken from act()
 *
 * Lam 29.5.98    - zabezpieczenia
 *     15.6.98    - mprog_actor do trig_proga (juz tego nie ma)
 *     27.10.1999 - MobC, zmienne, blokady na $*
 *     15.11.1999 - progdebug
 *     16.4.2000  - komentarze
 *     30.12.2003 - wbudowane polecenia mp* bez mprog_translate
 */
void mprog_process_cmnd( char *cmnd, CHAR_DATA *mob, CHAR_DATA *actor,
			OBJ_DATA *obj, CHAR_DATA *rndm, CHAR_DATA *vict,
			OBJ_DATA *v_obj )
{
    PROG_FUN *fun = NULL;
    char buf[ MSL ]; /* tu i ponizej bylo max_input_length */
    char tmp[ MSL ];
    char temp[ MSL ];
    char *i;
    int cmd;

    /* komentarze */
    if ( *cmnd == '/'
      || *cmnd == '*' )
	return; /* krotko */

    one_argument( cmnd, temp );
    for ( cmd = 0; prog_cmd_table[ cmd ].fun; cmd++ )
	if ( !str_prefix( temp, prog_cmd_table[ cmd ].name ) )
	{
	    fun = prog_cmd_table[ cmd ].fun;
	    break;
	}

    if ( !fun
      && str_cmp( temp, "int" )
      && str_cmp( temp, "aint" )
      && str_cmp( temp, "chr" )
      && str_cmp( temp, "achr" )
      && str_cmp( temp, "obj" )
      && str_cmp( temp, "aobj" )
      && str_cmp( temp, "str" )
      && str_cmp( temp, "astr" ) )
    {
	mprog_translate( cmnd, buf, mob, mob, actor, obj, rndm, vict, v_obj );
    }
    else
	strcpy( buf, cmnd );

    mprog_przelicz( buf, mob, actor, obj, rndm, vict, v_obj );

    if ( strlen( buf ) > MAX_INPUT_LENGTH - 1 )
    {
	progbug( mob, "Za dluga linia w mobprogu! Obcinam..." );
	buf[ MAX_INPUT_LENGTH - 1 ] = '\0';
    }

    /* Lam 20.12.1999: zmieniona kolejnosc w taki sposob, aby int i aint
			dzialaly takze w descr_progu */
    i = one_argument( buf, tmp );

#define if_( t ) if ( !str_cmp( tmp, t ) )

         if_( "aint" ) mv_aint( mob, i, actor, obj, rndm, vict, v_obj );
    else if_( "int"  ) mv_int(  mob, i, actor, obj, rndm, vict, v_obj );
    else if_( "astr" ) mv_astr( mob, i, actor, obj, rndm, vict, v_obj );
    else if_( "str"  ) mv_str(  mob, i, actor, obj, rndm, vict, v_obj );
    else if_( "achr" ) mv_achr( mob, i, actor, obj, rndm, vict, v_obj );
    else if_( "chr"  ) mv_chr(  mob, i, actor, obj, rndm, vict, v_obj );
    else if_( "aobj" ) mv_aobj( mob, i, actor, obj, rndm, vict, v_obj );
    else if_( "obj"  ) mv_obj(  mob, i, actor, obj, rndm, vict, v_obj );

#undef if_

    /* Lam: descr_prog */
    else if ( descr_prog_in_progress )
    {
	if ( strlen( descr_prog_buffer ) > 2 * MSL - MIL - 2 )
		progbug( mob, "Za dlugi opis w descr_progu!" );
	else if ( buf[ 0 ]
		   && buf[ strlen( buf ) - 1 ] == '\\' )
	{
	    buf[ strlen( buf ) - 1 ] = '\0';
	    strcat( descr_prog_buffer, buf );
	}
	else
	{
	    strcat( descr_prog_buffer, buf );
	    strcat( descr_prog_buffer, "\n\r" );
	}
    }
    else if ( fun )
    {
	progdebug( mob, "Wbudowane polecenie: %s\n\r", buf );
	( *fun )( i, mob, actor, obj, rndm, vict, v_obj );
	return;
    }
    else
    {
	progdebug( mob, "Zwyk`le polecenie: %s\n\r", buf );
	if ( !interpret( mob, buf, INTERP_MPROG ) )
	    progbug( mob, "Mob nie m`og`l wykona`c polecenia: %s", buf );
    }

    return;
}


/* ZMYSLY W MPECHO */
#define MPZM_WZROK	b00
#define MPZM_SLUCH	b01
#define MPZM_WECH	b02
#define MPZM_DOTYK	b03
#define MPZM_ZAWSZE	b04
#define MPZM_PRZYT	( MPZM_WZROK | MPZM_SLUCH | MPZM_WECH | MPZM_DOTYK )
#define MPZM_WID_I	b05
#define MPZM_WID_N	b06
#define MPZM_WID_T	b07
#define MPZM_WID_R	b08
#define MPZM_WID_O	b09
#define MPZM_WID_P	b10
#define MPZM_W_WZROK	b11
#define MPZM_W_SLUCH	b12
#define MPZM_W_WECH	b13
#define MPZM_W_DOTYK	b14
#define MPZM_W_PRZYT	( MPZM_W_WZROK | MPZM_W_SLUCH | MPZM_W_WECH | MPZM_W_DOTYK )
#define MPZM_W_WID_I	b15
#define MPZM_W_WID_N	b16
#define MPZM_W_WID_T	b17
#define MPZM_W_WID_R	b18
#define MPZM_W_WID_O	b19
#define MPZM_W_WID_P	b20


struct
{
    char *nazwa;
    int bit;
} nazwy_mp_zmyslow[ ] =
{
    { "WZROK",		MPZM_WZROK },
    { "S`LUCH",		MPZM_SLUCH },
    { "W`ECH",		MPZM_WECH },
    { "DOTYK",		MPZM_DOTYK },
    { "ZAWSZE",		MPZM_ZAWSZE },
    { "PRZYT",		MPZM_PRZYT },
    { "WID_I",		MPZM_WID_I },
    { "WID_N",		MPZM_WID_N },
    { "WID_T",		MPZM_WID_T },
    { "WID_R",		MPZM_WID_R },
    { "WID_O",		MPZM_WID_O },
    { "WID_P",		MPZM_WID_P },
    { "W_WZROK",	MPZM_W_WZROK },
    { "W_S`LUCH",	MPZM_W_SLUCH },
    { "W_W`ECH",	MPZM_W_WECH },
    { "W_DOTYK",	MPZM_W_DOTYK },
    { "W_PRZYT",	MPZM_W_PRZYT },
    { "W_WID_I",	MPZM_W_WID_I },
    { "W_WID_N",	MPZM_W_WID_N },
    { "W_WID_T",	MPZM_W_WID_T },
    { "W_WID_R",	MPZM_W_WID_R },
    { "W_WID_O",	MPZM_W_WID_O },
    { "W_WID_P",	MPZM_W_WID_P },
    { "DYLEMAT",	0 }, /* a co, niech tez maja :) */
    { NULL,		0 },
};


int interpretuj_zmysl( CHAR_DATA *ch, const char *argument )
{
    int i;

    for ( i = 0; nazwy_mp_zmyslow[ i ].nazwa; i++ )
	if ( !str_cmp( argument, nazwy_mp_zmyslow[ i ].nazwa ) )
	    return nazwy_mp_zmyslow[ i ].bit;

    if ( ch )
	progbug( ch, "Nieznany zmysl \"%s\".", argument );
    else
    {
	/* wywolane przez interpretuj_proga przy starcie muda */
	sprintf( bug_buf, "nieznany zmysl \"%s\"", argument );
	cbug( bug_buf, 0 );
    }

    return 0;
}


int interpretuj_zmysly( CHAR_DATA *ch, const char *argument )
{
    char jeden[ MIL ];
    char *p;
    int wynik = 0;

    p = jeden;
    while ( TRUE )
    {
	if ( !isalpha( (unsigned char) *argument ) && *argument != '_' && *argument != '`' )
	{
	    *p = '\0';
	    wynik |= interpretuj_zmysl( ch, jeden );
	    p = jeden;
	    if ( *argument == '\0' )
		break;
	    argument++;
	}
	else
	    *p++ = *argument++;
    }

    /* w mpecho*
    if ( !wynik )
	progbug( ch, "Brak zmyslow." ); */

    if ( !IS_SET( wynik, MPZM_PRZYT | MPZM_ZAWSZE ) )
    {
	if ( ch )
	    progbug( ch, "brak wybranych zmyslow (musi wystapic min. jeden z: wzrok s`luch w`ech dotyk przyt zawsze)" );
	else
	    cbug( "brak wybranych zmyslow (musi wystapic min. jeden z: wzrok sluch wech dotyk przyt zawsze)", 0 );
	return wynik;
    }

    return wynik;
}


/*
 * Pierwotnie skopiowane z super_act( ).
 */
bool czy_wyswietlic( CHAR_DATA *komu, int zmysly, CHAR_DATA *ch,
		CHAR_DATA *actor, OBJ_DATA *obj, CHAR_DATA *rndm,
		CHAR_DATA *vict, OBJ_DATA *v_obj )
{
    /* sprawdzenie, czy sa ustawione ktorekolwiek ze zmyslow na "plus"
       przeniesione do interpretuj_zmysly, zeby zglaszac raz, a nie dla
       kazdej postaci w pomieszczeniu
    if ( !IS_SET( zmysly, MPZM_PRZYT | MPZM_ZAWSZE ) )
    {
	progbug( ch, "brak wybranych zmyslow" );
	return FALSE;
    } */

    /* 1. MPZM_ZAWSZE widoczne zawsze; dowolny inny zmysl wystarczy do
          odebrania wiadomosci */
    if ( !IS_SET( zmysly, MPZM_ZAWSZE ) )
    {
	bool widzi = FALSE;

	if ( IS_SET( zmysly, MPZM_WZROK )
	  && IS_AWAKE( komu )
	  && can_see_room( komu, komu->in_room ) )
	{
	    widzi = TRUE;
	}

	if ( !widzi
	  && IS_SET( zmysly, MPZM_SLUCH )
	  && CAN_HEAR( komu ) )
	{
	    widzi = TRUE;
	}

	if ( !widzi
	  && IS_SET( zmysly, MPZM_WECH )
	  && IS_AWAKE( komu )
	  && !CZY_BRAK_TLENU( komu ) )
	{
	    widzi = TRUE;
	}

	if ( !widzi
	  && IS_SET( zmysly, MPZM_DOTYK )
	  && IS_AWAKE( komu ) )
	{
	    widzi = TRUE;
	}

	if ( !widzi )
	    return FALSE;
    }

    /* 2. MPZM_WID_* musza byc widoczne wszystkie, zeby odebrac wiadomosc */
    if ( IS_SET( zmysly, MPZM_WID_I )
      && !can_see( komu, ch ) )
    {
	return FALSE;
    }

    if ( IS_SET( zmysly, MPZM_WID_N )
      && ( !actor
	|| !can_see( komu, actor ) ) )
    {
	if ( !actor )
	    progbug( ch, "wid_n bez $n!" );
	return FALSE;
    }

    if ( IS_SET( zmysly, MPZM_WID_T )
      && ( !vict
	|| !can_see( komu, vict ) ) )
    {
	if ( !vict )
	    progbug( ch, "wid_t bez $t!" );
	return FALSE;
    }

    if ( IS_SET( zmysly, MPZM_WID_R )
      && ( !rndm
	|| !can_see( komu, rndm ) ) )
    {
	if ( !rndm )
	    progbug( ch, "wid_r bez $r!" );
	return FALSE;
    }

    if ( IS_SET( zmysly, MPZM_WID_O )
      && ( !obj
	|| !can_see_obj( komu, obj ) ) )
    {
	if ( !obj )
	    progbug( ch, "wid_o bez $o!" );
	return FALSE;
    }

    if ( IS_SET( zmysly, MPZM_WID_P )
      && ( !v_obj
	|| !can_see_obj( komu, v_obj ) ) )
    {
	if ( !v_obj )
	    progbug( ch, "wid_p bez $p!" );
	return FALSE;
    }

    /* 3. Odbieranie dowolnego ze zmyslow MPZM_W_* blokuje wiadomosc */
    if ( IS_SET( zmysly, MPZM_W_WZROK )
      && IS_AWAKE( komu )
      && can_see_room( komu, komu->in_room ) )
    {
	return FALSE;
    }

    if ( IS_SET( zmysly, MPZM_W_SLUCH )
      && CAN_HEAR( komu ) )
    {
	return FALSE;
    }

    if ( IS_SET( zmysly, MPZM_W_WECH )
      && IS_AWAKE( komu )
      && !CZY_BRAK_TLENU( komu ) )
    {
	return FALSE;
    }

    if ( IS_SET( zmysly, MPZM_W_DOTYK )
      && IS_AWAKE( komu ) )
    {
	return FALSE;
    }

    /* 4. Bez MPZM_W_WID_* widac wszystkich. Z jakimis MPZM_W_WID_* wystarczy
          zalapanie sie na dowolny, zeby zobaczyc komunikat */
    if ( IS_SET( zmysly, MPZM_W_WID_I )
      || IS_SET( zmysly, MPZM_W_WID_N )
      || IS_SET( zmysly, MPZM_W_WID_T )
      || IS_SET( zmysly, MPZM_W_WID_R )
      || IS_SET( zmysly, MPZM_W_WID_O )
      || IS_SET( zmysly, MPZM_W_WID_P ) )
    {
	bool widzi = FALSE;

	if ( IS_SET( zmysly, MPZM_W_WID_I )
	  && !can_see( komu, ch ) )
	{
	    widzi = TRUE;
	}

	if ( !widzi
	  && IS_SET( zmysly, MPZM_W_WID_N )
	  && ( !actor
	    || !can_see( komu, actor ) ) )
	{
	    if ( !actor )
		progbug( ch, "w_wid_n bez $n!" );
	    widzi = TRUE;
	}

	if ( !widzi
	  && IS_SET( zmysly, MPZM_W_WID_T )
	  && ( !vict
	    || !can_see( komu, vict ) ) )
	{
	    if ( !vict )
		progbug( ch, "w_wid_t bez $t!" );
	    widzi = TRUE;
	}

	if ( !widzi
	  && IS_SET( zmysly, MPZM_W_WID_R )
	  && ( !rndm
	    || !can_see( komu, rndm ) ) )
	{
	    if ( !rndm )
		progbug( ch, "w_wid_r bez $r!" );
	    widzi = TRUE;
	}

	if ( !widzi
	  && IS_SET( zmysly, MPZM_W_WID_O )
	  && ( !obj
	    || !can_see_obj( komu, obj ) ) )
	{
	    if ( !obj )
		progbug( ch, "w_wid_o bez $o!" );
	    widzi = TRUE;
	}

	if ( !widzi
	  && IS_SET( zmysly, MPZM_W_WID_P )
	  && ( !v_obj
	    || !can_see_obj( komu, v_obj ) ) )
	{
	    if ( !obj )
		progbug( ch, "w_wid_p bez $p!" );
	    widzi = TRUE;
	}

	if ( !widzi )
	    return FALSE;
    }

    return TRUE;
}


/* WLASCIWE POLECENIA PROGOWE */

/* prints the argument to all the rooms aroud the mobile */
KOMENDA_PROGOWA( do_mpasound )
{
    ROOM_INDEX_DATA *was_in_room;
    CHAR_DATA *obser;
    int door;
    char buf[ MSL ];
    char arg[ MIL ];
    int zmysly;

    if ( argument[ 0 ] == '\0' )
    {
	progbug( ch, "mpasound - brak argumentu" );
	return;
    }

    argument = one_argument( argument, arg );
    zmysly = interpretuj_zmysly( ch, arg );
    if ( !zmysly )
    {
	progbug( ch, "mpasound - kompletny brak zmyslow" );
	return;
    }

    /* Lam 15.6.98 */
    if ( argument[ 0 ] == '.' )
	argument++;

    was_in_room = ch->in_room;
    for ( door = 0; door <= 9; door++ )
    {
	EXIT_DATA       *pexit;

	if ( ( pexit = was_in_room->exit[ door ] ) != NULL
	  && pexit->to_room != NULL
	  && pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    for ( obser = ch->in_room->people; obser; obser = obser->next_in_room )
	    {
		if ( obser->deleted
		  || obser == ch
		  || !czy_wyswietlic( obser, zmysly, ch, actor, obj, rndm, vict, v_obj ) )
		{
		    continue;
		}

		mprog_translate( argument, buf, obser, ch, actor, obj, rndm, vict, v_obj );
		strcat( buf, "\n\r" );
		buf[ 0 ] = UPPER( buf[ 0 ] );

		if ( buf[ 0 ] == '{' )
		    buf[ 2 ] = UPPER( buf[ 2 ] );

		if ( buf[ 0 ] == '`' )
		    buf[ 1 ] = UPPER( buf[ 1 ] );

		else if ( ( buf[ 0 ] == '{' ) && ( buf[ 2 ] == '`' ) )
		    buf[ 3 ] = UPPER( buf[ 3 ] );

		STC( buf, obser );
	    }
	}
    }

    ch->in_room = was_in_room;

    return;
}


/* lets the mobile kill any player or mobile without murder */
KOMENDA_PROGOWA( do_mpkill )
{
    char      arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	progbug( ch, "mpkill - brak argumentu" );
	return;
    }

    if ( !IS_AWAKE( ch ) )
    {
	progbug( ch, "mpkill - mob jest nieprzytomny" );
	return;
    }

    if ( ( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) == NULL )
    {
	progbug( ch, "mpkill - nie znaleziono ofiary \"%s\"", arg );
	return;
    }

    if ( victim == ch )
    {
	progbug( ch, "mpkill - proba ataku samego siebie" );
	return;
    }

    if ( ch->in_room != victim->in_room )
    {
	progbug( ch, "mpkill - ofiara jest w innym pomieszczeniu" );
	return;
    }

    if ( ch->position == POS_FIGHTING || ch->fighting )
    {
	progbug( ch, "mpkill - mob juz walczy" );
	return;
    }

    if ( !can_see( ch, victim ) )
    {
	progbug( ch, "mpkill - mob nie widzi ofiary" );
	return;
    }

    if ( is_safe( ch, victim, FALSE ) )
    {
	progbug( ch, "mpkill - ofiara jest bezpieczna" );
	return;
    }

    multi_hit( ch, victim, TYPE_UNDEFINED );

    return;
}


/*
 * Ustawia opoznienie postaci
 * Wielkosc opoznienia wyrazona jest w pulsach, nie w sekundach.
 */
KOMENDA_PROGOWA( do_mplag )
{
    char      arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	progbug( ch, "mplag - brak argumentu" );
	return;
    }

    if ( ( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) == NULL )
    {
	progbug( ch, "mplag - nie ma ofiary!" );
	return;
    }

    if ( !is_number( argument ) )
    {
	progbug( ch, "mplag - bledny drugi argument" );
	return;
    }

    if ( atoi( argument ) > 240 )
    {
	progbug( ch, "mplag - lag wiekszy niz minuta" );
	return;
    }

    victim->wait = UMAX( atoi( argument ), victim->wait );

    return;
}


/* lets the mobile destroy an object in its inventory
   it can also destroy a worn object and it can destroy
   items using all.xxxxx or just plain all of them */
KOMENDA_PROGOWA( do_mpjunk )
{
    char      arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *tobj;
    OBJ_DATA *tobj_next;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0')
    {
	progbug( ch, "mpjunk - brak argumentu" );
	return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	if ( ( tobj = get_obj_prog( ch, argument, actor, obj, rndm, vict, v_obj ) ) )
	{
	    if ( tobj->deleted )
		progbug( ch, "mpjunk: przedmiot '%s' (ze zmiennej) juz skasowany, uzywaj deleted( )", argument );
	    else
		extract_obj( tobj );
	    return;
	}
	if ( ( tobj = get_obj_wear( ch, arg ) ) != NULL )
	{
	    /* to usunietego przedmiotu nie znajdzie */
	    unequip_char( ch, tobj );
	    extract_obj( tobj );
	    return;
	}
	if ( ( tobj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    progbug( ch, "mpjunk: nie widze przedmiotu '%s'", arg );
	    return;
	}
	extract_obj( tobj );
    }
    else
    {
	bool found = FALSE;

	for ( tobj = ch->carrying; tobj != NULL; tobj = tobj_next )
	{
	    tobj_next = tobj->next_content;

	    if ( tobj->deleted )
		continue;

	    if ( arg[ 3 ] == '\0' || is_name( &arg[ 4 ], tobj->name ) )
	    {
		if ( tobj->wear_loc != WEAR_NONE)
		    unequip_char( ch, tobj );
		extract_obj( tobj );
		found = TRUE;
	    }
	}

	if ( !found )
	{
	    if ( !str_cmp( arg,"all" ) )
		progbug( ch, "mpjunk all: Mob niczego przy sobie nie ma" );
	    else
		progbug( ch, "mpjunk all.obj: Mob nie ma przy sobie ani jednego \"%s\"", &arg[ 4 ] );
	}
    }

    return;
}


/* prints the message to everyone in the room other than the mob and victim */
KOMENDA_PROGOWA( do_mpechoaround )
{
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MSL ];
    CHAR_DATA *victim, *obser;
    int zmysly;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	progbug( ch, "mpechoaround - brak argumentu" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpechoaround - victim '%s' does not exist", arg );
	return;
    }

    argument = one_argument( argument, arg );
    zmysly = interpretuj_zmysly( ch, arg );
    if ( !zmysly )
    {
	progbug( ch, "mpechoaround - kompletny brak zmyslow" );
	return;
    }

    /* Lam 15.6.98 */
    if ( argument[ 0 ] == '.' )
	argument++;

    for ( obser = victim->in_room->people; obser; obser = obser->next_in_room )
    {
	if ( obser->deleted || obser == ch || obser == victim
	  || !czy_wyswietlic( obser, zmysly, ch, actor, obj, rndm, vict, v_obj ) )
	    continue;

	mprog_translate( argument, buf, obser, ch, actor, obj, rndm, vict, v_obj );
	strcat( buf, "\n\r" );
	buf[ 0 ] = UPPER( buf[ 0 ] );

	if ( buf[ 0 ] == '{' )
	    buf[ 2 ] = UPPER( buf[ 2 ] );

	if ( buf[ 0 ] == '`' )
	    buf[ 1 ] = UPPER( buf[ 1 ] );

	else if ( ( buf[ 0 ] == '{' ) && ( buf[ 2 ] == '`' ) )
	    buf[ 3 ] = UPPER( buf[ 3 ] );

	STC( buf, obser );
    }

    return;
}


/* prints the message to only the victim */
KOMENDA_PROGOWA( do_mpechoat )
{
    char arg[ MIL ];
    char buf[ MSL ];
    CHAR_DATA *victim;
    int zmysly;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
    {
	progbug( ch, "mpechoat - brak argumentu" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpechoat - victim '%s' does not exist", arg );
	return;
    }

    argument = one_argument( argument, arg );
    zmysly = interpretuj_zmysly( ch, arg );
    if ( !zmysly )
    {
	progbug( ch, "mpechoat - kompletny brak zmyslow" );
	return;
    }

    /* Lam 15.6.98 */
    if ( argument[ 0 ] == '.' )
	argument++;

    if ( !czy_wyswietlic( victim, zmysly, ch, actor, obj, rndm, vict, v_obj ) )
	return;

    mprog_translate( argument, buf, victim, ch, actor, obj, rndm, vict, v_obj );
    strcat( buf, "\n\r" );
    buf[ 0 ]          = UPPER( buf[ 0 ] );

    if ( buf[ 0 ] == '{' )
	buf[ 2 ] = UPPER( buf[ 2 ] );

    if ( buf[ 0 ] == '`' )
	buf[ 1 ] = UPPER( buf[ 1 ] );

    else if ( ( buf[ 0 ] == '{' ) && ( buf[ 2 ] == '`' ) )
	buf[ 3 ] = UPPER( buf[ 3 ] );

    STC( buf, victim );

    return;
}


/* prints the message to the room at large */
KOMENDA_PROGOWA( do_mpecho )
{
    CHAR_DATA *obser;
    char buf[ MSL ];
    char arg[ MIL ];
    int zmysly;

    if ( argument[ 0 ] == '\0' )
    {
	progbug( ch, "mpecho - called w/o argument" );
	return;
    }

    argument = one_argument( argument, arg );
    zmysly = interpretuj_zmysly( ch, arg );
    if ( !zmysly )
    {
	progbug( ch, "mpecho - kompletny brak zmyslow" );
	return;
    }

    /* Lam 15.6.98 */
    if ( argument[ 0 ] == '.' )
	argument++;

    for ( obser = ch->in_room->people; obser; obser = obser->next_in_room )
    {
	if ( obser->deleted
	  || obser == ch
	  || !czy_wyswietlic( obser, zmysly, ch, actor, obj, rndm, vict, v_obj ) )
	{
	    continue;
	}

	mprog_translate( argument, buf, obser, ch, actor, obj, rndm, vict, v_obj );
	strcat( buf, "\n\r" );
	buf[ 0 ]          = UPPER( buf[ 0 ] );

	if ( buf[ 0 ] == '{' )
	    buf[ 2 ] = UPPER( buf[ 2 ] );

	if ( buf[ 0 ] == '`' )
	    buf[ 1 ] = UPPER( buf[ 1 ] );

	else if ( ( buf[ 0 ] == '{' ) && ( buf[ 2 ] == '`' ) )
	    buf[ 3 ] = UPPER( buf[ 3 ] );

	STC( buf, obser );
    }

    return;
}


/* lets the mobile load an item or mobile.  All items
are loaded into inventory.  you can specify a level with
the load object portion as well. */

KOMENDA_PROGOWA( do_mpmload )
{
    char            arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA      *victim;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || !is_number( arg ) )
    {
	progbug( ch, "mpmload - Bad vnum as arg: '%s'", arg );
	return;
    }

    if ( atoi( arg ) == 3 )
    {
	progbug( ch, "mpmload - proboje mpmloadowac supermoba." );
	return;
    }

    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	progbug( ch, "mpmload - Bad mob vnum: '%s'", arg );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );

    return;
}


KOMENDA_PROGOWA( do_mpoload )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MIL ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA       *tobj;
    int             level;
    int		    timer = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[ 0 ] == '\0' || !is_number( arg1 ) )
    {
	progbug( ch, "mpoload - Bad syntax" );
	return;
    }

    if ( arg2[ 0 ] == '\0' )
	level = get_trust( ch );
    else
    {
	/*
	 * New feature from Alander.
	 */
	if ( !is_number( arg2 ) )
	{
	    progbug( ch, "mpoload - Bad level '%s'", arg2 );
	    return;
	}
	level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
	{
	    progbug( ch, "mpoload - Bad level %d", level );
	    return;
	}

	if ( arg3[ 0 ] )
	{
	    if ( !is_number( arg3 ) )
	    {
		progbug( ch, "mpoload - Bad timer '%s'", arg3 );
		return;
	    }
	    else timer = atoi( arg3 );
	}
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	progbug( ch, "mpoload - Bad vnum arg: '%s'", arg1 );
	return;
    }

    tobj = create_object( pObjIndex, level );
    if ( timer )
	tobj->timer = timer;

    if ( CAN_WEAR( tobj, ITEM_TAKE )
      && !IS_OBJ_STAT( pObjIndex, ITEM_BURIED ) )
	obj_to_char( tobj, ch );
    else
	obj_to_room( tobj, ch->in_room );

    return;
}


/* lets the mobile purge all objects and other npcs in the room,
   or purge a specified object or mob in the room.  It can purge
   itself, but this had best be the last command in the MOBprogram
   otherwise ugly stuff will happen */
KOMENDA_PROGOWA( do_mppurge )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA  *tobj;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	/* 'purge' */
	CHAR_DATA *vnext;
	OBJ_DATA  *tobj_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC( victim ) && victim != ch && victim->pIndexData->vnum != 3 )
	    {
		stop_fighting( victim, TRUE );
		extract_char( victim, TRUE );
	    }
	}

	for ( tobj = ch->in_room->contents; tobj != NULL; tobj = tobj_next )
	{
	    tobj_next = tobj->next_content;
	    extract_obj( tobj );
	}

	return;
    }

    /* Lam 16.9.2004: aby dzialalo mppurge $o bez bledu w get_char_prog */
    if ( ( arg[ 0 ] == '$'
      && ( arg[ 1 ] == 'o'
        || arg[ 1 ] == 'p' ) )
      || !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	if ( ( tobj = get_obj_here_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
	    extract_obj( tobj );
	else
	    progbug( ch, "mppurge - Victim '%s' not found", arg );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	progbug( ch, "mppurge - Purging a PC (mppurge %s -> %s)", arg, victim->name );
	return;
    }

    if ( victim->pIndexData->vnum == 3 )
    {
	progbug( ch, "mppurge: usiluje wymiesc supermoba." );
	return;
    }

    stop_fighting( victim, TRUE );
    extract_char( victim, TRUE );

    return;
}


/* lets the mobile goto any location it wishes that is not private */
KOMENDA_PROGOWA( do_mpgoto )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	progbug( ch, "mpgoto - brak argumentu" );
	return;
    }

    if ( !( location = find_location_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpgoto - nie ma miejsca \"%s\"", arg );
	return;
    }

    if ( ch->fighting )
	stop_fighting( ch, TRUE );

    char_from_room( ch );
    char_to_room( ch, location );

    return;
}


/* lets the mobile do a command at another location. Very useful */
KOMENDA_PROGOWA( do_mpat )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    CHAR_DATA       *wch;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
    {
	progbug( ch, "mpat - Bad argument" );
	return;
    }

    if ( !( location = find_location_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpat - No such location '%s'", arg );
	return;
    }

    if ( ch->fighting )
    {
	progbug( ch, "mpat - usiluje sie przemiescic w trakcie walki" );
	return;
    }

    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    mprog_process_cmnd( argument, ch, actor, obj, rndm, vict, v_obj );
/*    interpret( ch, argument, INTERP_MPAT ); */

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch->deleted )
	    continue;

	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    break;
	}
    }

    return;
}


/* lets the mobile transfer people.  the all argument transfers
   everyone in the current room to the specified location */
KOMENDA_PROGOWA( do_mptransfer )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char             arg3[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    CHAR_DATA       *victim, *v_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[ 0 ] == '\0' )
    {
	progbug( ch, "mptransfer - Bad syntax" );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	for ( victim = ch->in_room->people; victim; victim = v_next )
	{
	    v_next = victim->next_in_room;

	    if ( victim->deleted
	      || victim == ch
	      || victim->in_room != ch->in_room )
	    {
		continue;
	    }

	    if ( can_see( ch, victim ) )
	    {
		char buf[ MAX_STRING_LENGTH ];

		sprintf( buf, "%s %s %s", name_expand( ch, victim ), arg2, arg3 );
		do_mptransfer( buf, ch, actor, obj, rndm, vict, v_obj );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[ 0 ] == '\0' )
	location = ch->in_room;
    else
    {
	if ( !( location = find_location_prog( ch, arg2, actor, obj, rndm, vict, v_obj ) ) )
	{
	    progbug( ch, "mptransfer - No such location '%s'", arg2 );
	    return;
	}

	if ( room_is_private( location ) )
	{
	    progbug( ch, "mptransfer - Private room (arg '%s', vnum %d)",
				arg2, location->vnum );
	    return;
	}

	if ( location->vnum == ROOM_VNUM_POZA_SWIATEM )
	{
	    progbug( ch, "mptransfer - proba przeniesienia postaci poza postac (arg '%s', vnum %d)",
				arg2, location->vnum );
	    return;
	}

	if ( location->trap )
	{
	    progbug( ch, "mptransfer - TRAP room (arg '%s', vnum %d)",
				arg2, location->vnum );
	    return;
	}
    }

    if ( !( victim = get_char_room_prog( ch, arg1, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mptransfer - No such person '%s'", arg1 );
	return;
    }

    if ( !victim->in_room )
    {
	progbug( ch, "mptransfer - Victim in NULL room?!" );
	return;
    }

    if ( victim->fighting )
	stop_fighting( victim, TRUE );

    PRZERWIJ_KOPANIE( victim, FALSE );
    PRZERWIJ_KARMPOJ( victim, FALSE );

    char_from_room( victim );
    char_to_room( victim, location );

    if ( arg3[ 0 ] == 'p' )
	do_look( victim, "auto" );

    return;
}


KOMENDA_PROGOWA( do_mptransfer_n )
{
    progbug( ch, "mptransfer_n - polecenie usuniete!" );

    return;
}


/* lets the mobile force someone to do something.  must be mortal level
   and the all argument only affects those in the room with the mobile */
KOMENDA_PROGOWA( do_mpforce )
{
    char arg[ MIL ];
    char buf[ MSL ];

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
    {
	progbug( ch, "mpforce - Bad syntax" );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

/* Lamowi opadla szczeka i rece.
	for ( vch = char_list; vch != NULL; vch = vch_next )
*/
	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next = vch->next_in_room;

	    if ( vch != ch /* tego tez nie bylo... */
		&& get_trust( vch ) < get_trust( ch )
		&& can_see( ch, vch ) )
	    {
		mprog_translate( argument, buf, vch, ch, actor, obj, rndm, vict, v_obj );
		interpret( vch, buf, INTERP_MPFORCE );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) == NULL )
	{
	    progbug( ch, "mpforce - No such victim '%s'", arg );
	    return;
	}

	if ( victim == ch )
	{
	    progbug( ch, "mpforce - Forcing oneself" );
	    return;
	}

	mprog_translate( argument, buf, victim, ch, actor, obj, rndm, vict, v_obj );
	interpret( victim, buf, INTERP_MPFORCE );
    }

    return;
}


KOMENDA_PROGOWA( do_mppeace )
{
    CHAR_DATA *rch;

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch->fighting )
	    stop_fighting( rch, TRUE );
	if ( rch->hunting )
	{
	    wywal_zwod( &rch->hunting->zwod_hunting, rch );
	    rch->hunting = NULL;
	}
	if ( rch->hating )
	{
	    wywal_zwod( &rch->hating->zwod_hating, rch );
	    rch->hating = NULL;
	}
	if ( rch->fearing )
	{
	    wywal_zwod( &rch->fearing->zwod_fearing, rch );
	    rch->fearing = NULL;
	}
    }

    send_to_char( "Zrobione.\n\r", ch );

    return;
}


KOMENDA_PROGOWA( do_mpsetowner )
{
    OBJ_DATA  *tobj;
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	progbug( ch, "mpsetowner - brak argumentu" );
	return;
    }

    if ( !*argument )
    {
	progbug( ch, "mpsetowner - brak wlasciciela" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, argument, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpsetowner - nie moge znalezc ofiary" );
	return;
    }

    if ( !( tobj = get_obj_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
	if ( !( tobj = get_obj_carry( ch, arg ) ) )
	{
	    progbug( ch, "mpsetowner - brak przedmiotu" );
	    return;
	}

    if ( !IS_OBJ_STAT( tobj, ITEM_PRIVATE ) )
	SET_BIT_V( tobj->extra_flags, ITEM_PRIVATE );

    free_string( tobj->owner );
    tobj->owner = str_dup( victim->name );

    return;
}


KOMENDA_PROGOWA( do_mpslay )
{
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *room;
    bool placz = TRUE;

    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
	progbug( ch, "mpslay - brak argumentu" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpslay - nie ma ofiary!" );
	return;
    }

    if ( ch == victim )
    {
	progbug( ch, "mpslay - samobojstwo jest zakazane" );
	return;
    }

    if ( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL )
    {
	progbug( ch, "mpslay - usiluje zabic niesmiertelnego" );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	sprintf( log_buf, "%s killed by mpslay at %d", victim->short_descr, victim->in_room->vnum );
	log_string( log_buf );
	wiznet( "{M$N{x zosta`l$7 zabit$8 poprzez mpslay.", victim, NULL, NULL, WIZ_DEATH, 0, 0 );
	strata_dosw_za_smierc( victim );
    }

    room = victim->in_room; /* wcale nie musi byc rowny ch->in_room */

    if ( mprog_death_trigger( victim, ch ) )
	placz = FALSE;

    if ( victim->deleted
      || victim->in_room != room )
    {
	return;
    }

    if ( placz )
	death_cry( victim, 0, victim->position );

    raw_kill( ch, victim, FALSE, FALSE, ZGON_ZWYKLY );

    return;
}


/*
 * Lam 20.5.99: mass slay - narzedzie zaglady masowej, zabija wszystko oprocz
 * moba i niesmiertelnych
 */
KOMENDA_PROGOWA( do_mpmassslay )
{
    CHAR_DATA *victim, *victim_next;
    bool placz = TRUE;
    ROOM_INDEX_DATA *room = ch->in_room;

    if ( !ch->in_room )
    {
	progbug( ch, "mpmassslay - ch->in_room == NULL!!!" );
	return;
    }

    if ( !( victim = ch->in_room->people ) )
    {
	progbug( ch, "mpmassslay - nie ma ofiar!" );
	return;
    }

    for ( ; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;

	if ( victim->in_room != room ) /* death_progi mogly zepsuc liste */
	    return;

	if ( victim->deleted
	  || victim == ch
	  || ( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL ) )
	{
	    continue;
	}

	if ( !IS_NPC( victim ) )
	{
	    sprintf( log_buf, "%s killed by mpmassslay at %d", victim->short_descr, victim->in_room->vnum );
	    log_string( log_buf );
	    wiznet( "{M$N{x zosta`l$7 zabit$8 poprzez mpmassslay.", victim, NULL, NULL, WIZ_DEATH, 0, 0 );
	    strata_dosw_za_smierc( victim );
	}

	if ( mprog_death_trigger( victim, ch ) )
	    placz = FALSE;

	if ( victim->deleted
	  || victim->in_room != room )
	{
	    continue;
	}

	if ( placz )
	    death_cry( victim, 0, victim->position );

	raw_kill( ch, victim, FALSE, FALSE, ZGON_ZWYKLY );

	if ( ch->deleted ) /* to nam bardzo psuje szyki, ale raw_kill pojdzie */
	    return;

	/* dziala bez komunikatu. Przed wykonaniem doradzam mpecho */
    }

    return;
}


KOMENDA_PROGOWA( do_mpcrithit )
{
    progbug( ch, "mpcrithit - polecenie usuniete, uzywaj mpslay" );

    return;
}


KOMENDA_PROGOWA( do_mpmasscrithit )
{
    progbug( ch, "mpmasscrithit - polecenie usuniete, uzywaj mpmassslay" );

    return;
}


/*
 * Lam 24.11.98
 */
KOMENDA_PROGOWA( do_mpbeep )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	progbug( ch, "mpbeep - brak argumentu" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpbeep - nie znaleziono ofiary \"%s\"", arg );
	return;
    }

    send_to_char( "\a", victim );

    return;
}


/*
 * Lam 15.6.98: mptrigger
 */
KOMENDA_PROGOWA( do_mptrigger )
{
    MTRIG_DATA *trig;
    CHAR_DATA *victim;
    char arg[ MIL ];
    int time;

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mptrigger: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mptrigger: nie znalazlem ofiary '%s'", arg );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	progbug( ch, "mptrigger: ofiara '%s' nie jest mobem!", arg );
	return;
    }

    if ( !IS_SET( victim->pIndexData->progtypes, M_TRIG_PROG ) )
    {
	progbug( ch, "mptrigger: ofiara (vnum %d) nie obsluguje trig_proga", victim->pIndexData->vnum );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mptrigger: za malo argumentow" );
	return;
    }
    if ( !is_number( arg ) )
    {
	progbug( ch, "mptrigger: drugi argument musi byc liczba (jest '%s')", arg );
	return;
    }
    time = atoi( arg );

    if ( time > 240 )
    {
	progbug( ch, "mptrigger: czas wiekszy niz 240 (bo %d), obcinam.", time );
	time = 240;
    }

    trig = calloc( sizeof( MTRIG_DATA ), 1 );
    strcpy( trig->arg, argument );
    trig->ch	= victim;
    if ( ch != victim )
    {
	trig->actor	= actor;
	trig->vict	= ch;
    }
    else
    {
/*	trig->vict	= ch->mprog_victim;  - nalezy wprowadzic */
	trig->actor	= actor;
/*	if ( !trig->actor )
	    trig->actor = ch; - wykomentowane, Lam 16.12.2003 */
    }
    trig->next	= mtrig_list;
    trig->time	= time;
    mtrig_list	= trig;

    return;
}


/*
 * Lam 17.8.98: mpquest
 */
KOMENDA_PROGOWA( do_mpquest )
{
    MPQUEST_DATA *mpquest;
    CHAR_DATA *victim;
    char arg[ MIL ];
    int vnum;
    int stage;

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpquest: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpquest: nie znalazlem ofiary '%s'", arg );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	progbug( ch, "mpquest: ofiara '%s' jest mobem!", arg );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpquest: za malo argumentow" );
	return;
    }

    if ( !is_number( arg ) )
    {
	progbug( ch, "mpquest: drugi argument musi byc liczba (jest '%s')", arg );
	return;
    }
    vnum = atoi( arg );

    if ( !is_number( argument ) )
    {
	progbug( ch, "mpquest: trzeci argument musi byc liczba (jest '%s')", argument );
	return;
    }
    stage = atoi( argument );

    mpquest = find_mpquest( victim, vnum );

    /* jesli takiego zadania jeszcze nie robil */
    if ( !mpquest )
    {
	mpquest = new_mpquest( );
	mpquest->vnum = vnum;
	mpquest->next = victim->mpquest;
	victim->mpquest = mpquest;
    }

    mpquest->stage = stage;

    return;
}


/*
 * Lam 18.8.98: mpendquest
 */
KOMENDA_PROGOWA( do_mpendquest )
{
    MPQUEST_DATA *mpquest;
    CHAR_DATA *victim;
    char arg[ MIL ];
    int vnum;

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpendquest: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpendquest: nie znalazlem ofiary '%s'", arg );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	progbug( ch, "mpendquest: ofiara '%s' jest mobem!", arg );
	return;
    }

    if ( !is_number( argument ) )
    {
	progbug( ch, "mpendquest: drugi argument musi byc liczba (jest '%s')", argument );
	return;
    }
    vnum = atoi( argument );

    mpquest = find_mpquest( victim, vnum );

    if ( !mpquest )
    {
	progbug( ch, "mpendquest: ofiara '%s' nie robila questa '%d'", arg, vnum );
	return;
    }

    del_mpquest( victim, mpquest );

    return;
}


/*
 * Lam 13.6.99
 */
KOMENDA_PROGOWA( do_mpquestdata )
{
    MPQUEST_DATA *mpquest;
    CHAR_DATA *victim;
    char arg[ MIL ];
    int vnum;
    int idx;
    int val;

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpquestdata: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpquestdata: nie znalazlem ofiary '%s'", arg );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	progbug( ch, "mpquestdata: ofiara '%s' jest mobem (%s)!", arg, victim->name );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpquestdata: za malo argumentow" );
	return;
    }

    if ( !is_number( arg ) )
    {
	progbug( ch, "mpquestdata: drugi argument musi byc liczba (jest '%s')", arg );
	return;
    }
    vnum = atoi( arg );

    argument = one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
	progbug( ch, "mpquestdata: trzeci argument musi byc liczba (jest '%s')", argument );
	return;
    }
    idx = atoi( arg );
    if ( idx < 0 || idx > 4 )
    {
	progbug( ch, "mpquestdata: trzeci argument musi byc z zakresu <0;4> (jest %d)", idx );
	return;
    }

    if ( !is_number( argument ) )
    {
	progbug( ch, "mpquestdata: czwarty argument musi byc liczba (jest '%s')", argument );
	return;
    }
    val = atoi( argument );

    mpquest = find_mpquest( victim, vnum );

    if ( !mpquest )
    {
	progbug( ch, "mpquestdata: nie znalazlem mpquesta %d na postaci '%s'!",
		vnum, victim->name );
	return;
    }

    mpquest->data[ idx ] = val;

    return;
}


/*
 * Lam 30.9.2005
 */
KOMENDA_PROGOWA( do_mpquesttitle )
{
    MPQUEST_DATA *mpquest;
    CHAR_DATA *victim;
    char arg[ MIL ];
    int vnum;

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpquesttitle: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpquesttitle: nie znalazlem ofiary '%s'", arg );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	progbug( ch, "mpquesttitle: ofiara '%s' jest mobem!", arg );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpquesttitle: za malo argumentow" );
	return;
    }

    if ( !is_number( arg ) )
    {
	progbug( ch, "mpquesttitle: drugi argument musi byc liczba (jest '%s')", arg );
	return;
    }
    vnum = atoi( arg );

    mpquest = find_mpquest( victim, vnum );

    if ( !mpquest )
    {
	progbug( ch, "mpquesttitle: gracz '%s' nie wykonywal zadania %d", victim->short_descr, vnum );
	return;
    }

    free_string( mpquest->title );
    if ( !str_cmp( argument, "-" ) || !str_cmp( argument, "clear" ) )
    {
	mpquest->title = str_dup( "" );
	free_string( mpquest->descr );
	mpquest->descr = str_dup( "" );
    }
    else
	mpquest->title = str_dup( argument );

    return;
}


KOMENDA_PROGOWA( do_mpquestdescr )
{
    MPQUEST_DATA *mpquest;
    CHAR_DATA *victim;
    char arg[ MIL ];
    char buf[ MSL ];
    int vnum;

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpquestdescr: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpquestdescr: nie znalazlem ofiary '%s'", arg );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	progbug( ch, "mpquestdescr: ofiara '%s' jest mobem!", arg );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpquestdescr: za malo argumentow" );
	return;
    }

    if ( !is_number( arg ) )
    {
	progbug( ch, "mpquestdescr: drugi argument musi byc liczba (jest '%s')", arg );
	return;
    }
    vnum = atoi( arg );

    mpquest = find_mpquest( victim, vnum );

    if ( !mpquest )
    {
	progbug( ch, "mpquestdescr: gracz '%s' nie wykonywal zadania %d", victim->short_descr, vnum );
	return;
    }

    buf[ 0 ] = '\0';

    if ( *argument == '+' )
    {
	if ( mpquest->descr )
	    strcpy( buf, mpquest->descr );
	argument++;
	while ( isspace( (unsigned char) *argument ) )
	    argument++;
    }

    if ( strlen( buf ) + strlen( argument ) >= MAX_STRING_LENGTH - 3 )
    {
	progbug( ch, "mpquestdescr: za dlugi opis!" );
	return;
    }

    strcat( buf, argument );
    strcat( buf, "\n\r" );
    free_string( mpquest->descr );
    mpquest->descr = str_dup( buf );

    return;
}


/*
 * Lam 23.5.99: mozna robic pulapki z poziomu mobprogow.
 */
extern int top_trap; /* db.c */
KOMENDA_PROGOWA( do_mpmaketrap )
{
    DEATHTRAP_DATA 	*trap;
    char		arg1[ MIL ];
    ROOM_INDEX_DATA	*location;

    argument = one_argument( argument, arg1 );

    if ( !arg1[ 0 ] || !*argument )
    {
	progbug( ch, "mpmaketrap - za malo argumentow!" );
	return;
    }

    if ( !( location = find_location_prog( ch, arg1, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpmaketrap - nie ma pomieszczenia '%s'", arg1 );
	return;
    }

    if ( location->trap )
    {
	progbug( ch, "mpmaketrap (#%s): pomieszczenie mialo pulapke (kasuje)", arg1 );
	free_string( location->trap->trap_string );
	free_mem( location->trap, sizeof( location->trap ) );
	top_trap--;
    }

    trap		= alloc_mem( sizeof( *trap ) );
    trap->trap_type	= 0;
    trap->trap_string	= str_dup( argument );
    top_trap++;
    location->trap	= trap;

    return;
}


KOMENDA_PROGOWA( do_mpuntrap )
{
    char		arg1[ MIL ];
    ROOM_INDEX_DATA	*location;

    one_argument( argument, arg1 );

    if ( !arg1[ 0 ] )
    {
	progbug( ch, "mpuntrap - za malo argumentow!" );
	return;
    }

    if ( !( location = find_location_prog( ch, arg1, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpuntrap - nie ma pomieszczenia '%s'", arg1 );
	return;
    }

    if ( location->trap )
    {
	free_string( location->trap->trap_string );
	free_mem( location->trap, sizeof( location->trap ) );
	location->trap = NULL;
	top_trap--;
    }
    else
	progbug( ch, "mpuntrap (#%s): w pomieszczeniu nie bylo pulapki", arg1 );

    return;
}


/*
 * Lam 8.12.1999
 */
KOMENDA_PROGOWA( do_mpqp )
{
    CHAR_DATA *victim;
    char arg[ MIL ];
    int qp;

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpqp: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpqp: nie znalazlem ofiary" );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	progbug( ch, "mpqp: co za debilizm dawac qp mobom? (%s -> %s)",
			arg, victim->name );
	return;
    }

    one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
	progbug( ch, "mpqp: drugi argument musi byc liczba (jest '%s')", arg );
	return;
    }
    qp = atoi( arg );

    if ( qp < 1 )
    {
	progbug( ch, "mpqp: trzeba dac przynajmniej 1 qp (a nie %d)", qp );
	return;
    }

    if ( qp > 100 )
    {
	progbug( ch, "mpqp: mozna dac najwyzej 100 qp (wiec nie %d)", qp );
	return;
    }

    victim->qp += qp;

    return;
}


/*
 * Qwert
 * Lam: (11.5.2000)
 */
KOMENDA_PROGOWA( do_mpdrop )
{
    OBJ_DATA *tobj;
    char      arg[ MAX_INPUT_LENGTH ], arg2[ MAX_INPUT_LENGTH ];
    bool      found;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[ 0 ] == '\0' )
    {
	progbug( ch, "mpdrop: Brak argumentow" );
	return;
    }

    /* Tissarius 27.10.2001
    if ( is_number( arg ) ) */
    if ( czy_monety( arg2 ) )
    {
	/* "mpdrop NNNN monet" */
	OBJ_DATA *tobj_next;
	int       amount;

	amount = atoi( arg );

	if ( amount <= 0 )
	{
	    progbug( ch, "mpdrop: niedodatnia liczba" );
	    return;
	}

	if ( ch->gold < amount )
	{
	    progbug( ch, "mpdrop: Mob probuje upuscic %d zlota majac %d",
		amount, ch->gold );
	    return;
	}

	ch->gold -= amount;

	/* pieniadze dolaczaja do lezacych na ziemi. Przepisane przez Lama. */
	for ( tobj = ch->in_room->contents; tobj; tobj = tobj_next )
	{
	    tobj_next = tobj->next_content;

	    if ( tobj->deleted )
		continue;

	    if ( tobj->pIndexData->vnum == OBJ_VNUM_MONEY_ONE
	      || tobj->pIndexData->vnum == OBJ_VNUM_MONEY_SOME )
	    {
		amount += tobj->pIndexData->vnum == OBJ_VNUM_MONEY_ONE ? 1
							: tobj->value[ 0 ].v;
		extract_obj( tobj );
	    }
	}

	obj_to_room( create_money( amount ), ch->in_room );
	return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	if ( ( tobj = get_obj_prog( ch, arg, actor, obj, rndm, vict, v_obj ) )
	  && tobj->carried_by != ch )
	{
	    progbug( ch, "mpdrop: przedmiot z $* nie w inwentarzu moba" );
	    return;
	}

	/* "mpdrop tobj" */
	if ( !tobj && !( tobj = get_obj_carry( ch, arg ) ) )
	{
	    progbug( ch, "mpdrop: Brak przedmiotu do upuszczenia" );
	    return;
	}

	if ( !can_drop_obj( ch, tobj ) )
	{
	    progbug( ch, "mpdrop: Proba upuszczenia przedmiotu niepozbywalnego" );
	    return;
	}

	obj_from_char( tobj );
	obj_to_room( tobj, ch->in_room );
	oprog_drop_trigger( ch, tobj );
	if ( !ch->deleted && !tobj->deleted )
	    rprog_drop_trigger( ch, tobj );
    }
    else
    {
	/* "mpdrop all" albo "mpdrop all.tobj" */
	OBJ_DATA *tobj_next;

	found = FALSE;
	for ( tobj = ch->carrying; tobj; tobj = tobj_next )
	{
	    tobj_next = tobj->next_content;

	    if ( ( !str_cmp( arg, "all" )
	        || ( arg[ 3 ] == '.' && is_name( &arg[ 4 ], tobj->name ) ) )
	      && can_see_obj( ch, tobj )
	      && tobj->wear_loc == WEAR_NONE
	      && can_drop_obj( ch, tobj ) )
	    {
		found = TRUE;
		obj_from_char( tobj );
		obj_to_room( tobj, ch->in_room );
		oprog_drop_trigger( ch, tobj );
		if ( !ch->deleted && !tobj->deleted )
		    rprog_drop_trigger( ch, tobj );
		if ( ch->deleted )
		    return;
	    }
	}

	if ( !found )
	{
	    if ( !str_cmp( arg, "all" ) )
		progbug( ch, "mpdrop all: Mob niczego przy sobie nie ma" );
	    else
		progbug( ch, "mpdrop all.obj: Mob nie ma przy sobie ani jednego \"%s\"", &arg[ 4 ] );
	}
    }

    return;
}


/*
 * Tissarius 27.10.2001
 */
KOMENDA_PROGOWA( do_mpgive )
{
    CHAR_DATA *victim;
    OBJ_DATA *tobj;
    char      arg[ MAX_INPUT_LENGTH ], arg2[ MAX_INPUT_LENGTH ], arg3[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
    {
	progbug( ch, "mpgive: Brak argumentow" );
	return;
    }

    if ( czy_monety ( arg2 ) && arg3[ 0 ] == '\0' )
    {
	progbug( ch, "mpgive: Malo argumentow (komu dac pieniadze?)" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, czy_monety( arg2 ) ? arg3 : arg2, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpgive: Mob chce dac cos komus, kogo nie ma" );
	return;
    }

    if ( czy_monety( arg2 ) )
    {
	/* "mpgive NNNN monet postac" */
	int       amount;

	amount = atoi( arg );

	if ( amount <= 0 )
	{
	    progbug( ch, "mpgive: niedodatnia liczba" );
	    return;
	}

	if ( ch->gold < amount )
	{
	    progbug( ch, "mpgive: Mob probuje upuscic %d zlota majac %d",
		amount, ch->gold );
	    return;
	}

	ch->gold -= amount;
	victim->gold +=amount;

	return;
    }

    if ( ( tobj = get_obj_prog( ch, arg, actor, obj, rndm, vict, v_obj ) )
      && tobj->carried_by != ch )
    {
	progbug( ch, "mpgive: przedmiot z $* nie w inwentarzu moba" );
	return;
    }

    if ( !tobj && !( tobj = get_obj_carry( ch, arg ) ) )
    {
	progbug( ch, "mpgive: Brak przedmiotu do oddania" );
	return;
    }

/* Na zyczenie Ulryka 30.4.2002
    if ( !can_drop_obj( ch, tobj ) )
    {
	progbug( ch, "mpgive: Proba upuszczenia przedmiotu niepozbywalnego" );
	return;
    }
*/

    if ( !CAN_WEAR( tobj, ITEM_TAKE ) )
    {
	progbug( ch, "mpgive: Proba przekazania przedmiotu, ktorego nie mozna wziac" );
	return;
    }

    if ( victim->carry_number + get_obj_number( tobj ) > can_carry_n( victim )
	|| victim->carry_weight + get_obj_weight( tobj ) > can_carry_w( victim ) )
    {
	progbug( ch, "mpgive: Prog nie sprawdza, czy ofiara uniesie przedmiot" );
	return;
    }

    if ( ( IS_OBJ_STAT( tobj, ITEM_HOLY ) && IS_EVIL( victim ) )
      || ( IS_OBJ_STAT( tobj, ITEM_VAMPIRE_BANE )
	&& CZY_WAMPIR( victim ) ) )
    {
	progbug( ch, "mpgive: Pr`oba przekazania przedmiotu, kt`ory mo`ze zabi`c ofiar`e" );
	return;
    }

/* Ulryk: przyjmuje, ze nie mozna wcisnac komus przedmiotu, ktory moze go zabic
    {
	act( "Pali ci`e `SWI`ETY OGIE`N $g. Au`c!", victim, tobj, NULL, TO_CHAR );
	act( "`SWI`ETY OGIE`N $g pali $3!", victim, tobj, NULL, TO_ROOM );
	damage( victim, victim, 50, gsn_burning_hands, WEAR_NONE, TRUE );
	if ( ch->deleted )
	    return;
    }
*/

    /* Ulryk: 26.04.2002 */
    if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_NO_GIFTS ) )
    {
	progbug( ch, "mpgive: ofiara nie chce przyj`a`c przedmiotu" );
	return;
    }

    obj_from_char( tobj );
    obj_to_char( tobj, victim );
    mprog_give_trigger( victim, ch, tobj );
    oprog_give_trigger( ch, victim, tobj );

    return;
}


KOMENDA_PROGOWA( do_mpechoaround_n )
{
    progbug( ch, "mpechoaround_n - polecenie usuniete!" );

    return;
}


KOMENDA_PROGOWA( do_mpechoat_n )
{
    progbug( ch, "mpechoat_n - polecenie usuniete!" );

    return;
}


/*
 * Lam 29.12.2003
 */
KOMENDA_PROGOWA( do_mpmakeexit )
{
    EXIT_DATA *ex;
    ROOM_INDEX_DATA *ri;
    char arg[ MIL ];
    int d, vnum;

    if ( !*argument )
    {
	progbug( ch, "mpmakeexit - brak argumentow!" );
	return;
    }

    argument = one_argument( argument, arg );

    for ( d = 0; d < 10; d++ )
	if ( !str_cmp( arg, kierunki[ d ].skrot ) )
	    break;

    if ( d == 10 )
    {
	progbug( ch, "mpmakeexit - nieznany kierunek '%s'", arg );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] )
    {
	progbug( ch, "mpmakeexit - brak drugiego argumentu!" );
	return;
    }
    if ( !is_number( arg ) )
    {
	progbug( ch, "mpmakeexit - vnum pomieszczenia ('%s') musi byc liczba!", arg );
	return;
    }
    /* mozna zrobic wyjscie do vnuma 0 z samym opisem */
    vnum = atoi( arg );
    ri = NULL;
    if ( vnum && !( ri = get_room_index( vnum ) ) )
    {
	progbug( ch, "mpmakeexit - brak pomieszczenia o vnumie %d", vnum );
	return;
    }
    if ( ch->in_room->exit[ d ] )
    {
	progbug( ch, "mpmakeexit - pomieszczenie %d ma juz wyjscie w kierunku %d", ch->in_room->vnum, d );
	return;
    }
    if ( !*argument )
    {
	progbug( ch, "mpmakeexit - brak opisu wyjscia!" );
	return;
    }

    ex = new_exit( );
    ex->description = str_dup( argument );
    ex->to_room = ri;
    ex->vnum = vnum;
    ch->in_room->exit[ d ] = ex;

    if ( d == DIR_DOWN )
    {
	CHAR_DATA *rch;
	OBJ_DATA *robj;

	for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	    check_airchar( rch );
	for ( robj = ch->in_room->contents; robj; robj = robj->next_content )
	    new_airobj( robj );
    }

    return;
}


/*
 * Lam 29.12.2003
 */
KOMENDA_PROGOWA( do_mpdelexit )
{
    char arg[ MIL ];
    int d;

    if ( !*argument )
    {
	progbug( ch, "mpmakeexit - brak argumentow!" );
	return;
    }

    one_argument( argument, arg );

    for ( d = 0; d < 10; d++ )
	if ( !str_cmp( arg, kierunki[ d ].skrot ) )
	    break;

    if ( d == 10 )
    {
	progbug( ch, "mpdeleexit - nieznany kierunek '%s'", arg );
	return;
    }

    if ( !ch->in_room->exit[ d ] )
    {
	progbug( ch, "mpdelexit - pomieszczenie %d nie ma wyjscia w kierunku %d", ch->in_room->vnum, d );
	return;
    }

    del_exit( ch->in_room->exit[ d ] );
    ch->in_room->exit[ d ] = NULL;

    return;
}


/*
 * Lam 22.1.2004
 */
KOMENDA_PROGOWA( do_mpbug )
{
    char buf[ MSL ];

    if ( argument[ 0 ] == '\0' )
    {
	progbug( ch, "mpbug bez argumentu" );
	return;
    }

    mprog_translate( argument, buf, ch, ch, actor, obj, rndm, vict, v_obj );
    progbug( ch, "%s", buf );

    return;
}


/*
 * Qwert 28.2.2004: Uzdrowiciel teraz bedzie rzadzic! :)
 */
KOMENDA_PROGOWA( do_mppolymorph )
{
    char        arg1[ MAX_INPUT_LENGTH ];
    char        arg2[ MAX_INPUT_LENGTH ];
    char        arg3[ MAX_INPUT_LENGTH ];
    AFFECT_DATA af;
    CHAR_DATA   *victim;
    int         rasa, czas;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
    {
	progbug( ch, "mppolymorph - Zbyt mala ilosc argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg1, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mppolymorph - Brak osoby '%s'", arg1 );
	return;
    }

    rasa = race_lookup( arg2 );

    if ( rasa < 0 )
    {
	progbug( ch, "mppolymorph - Rasa '%s' nie istnieje", arg2 );
	return;
    }

    if ( !is_number( arg3 ) )
    {
	progbug( ch, "mppolymorph - Zla skladnia" );
	return;
    }

    czas = atoi( arg3 );
    /* czy -1 to przegiecie? Fajnie miec rase az do smierci */
    if ( czas < -1 || czas > 997 )
    {
	progbug( ch, "mppolymorph - Argument 'czas' jest poza dostepnym zakresem" );
	return;
    }

    if ( IS_AFFECTED( victim, AFF_POLYMORPH ) )
    {
	affect_strip( victim, gsn_polymorph_other );
/*      sprawdz_ubranie( victim );  Qwert: to ma tu byc? */
    }

    if ( is_affected( victim, gsn_postac_nietoperza ) )
	affect_strip( victim, gsn_postac_nietoperza );

    af.type     = gsn_polymorph_other;
    af.duration = czas;
    af.location = APPLY_RACE;
    af.modifier = rasa - victim->race;
    af.level    = ch->level;
    af.caster   = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_POLYMORPH );
    affect_to_char( victim, &af );

    send_to_char( "Czujesz si`e inaczej.\n\r", victim );
    /* check_race_wearlocs( victim ); *//* Lam 7.6.99 */
    sprawdz_ubranie( victim );

    return;
}


/*
 * Qwert 05.03.2004
 */
KOMENDA_PROGOWA( do_mpdamage )
{
    CHAR_DATA *victim;
    char arg[ MIL ];
    int kopniak;
    bool placz = TRUE;

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ] || !*argument )
    {
	progbug( ch, "mpdamage: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpdamage: nie znalazlem ofiary" );
	return;
    }

    if ( victim == ch )
    {
	progbug( ch, "mpdamage: atak samobojczy" );
	return;
    }

    one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
	progbug( ch, "mpdamage: drugi argument musi byc liczba (jest '%s')", arg );
	return;
    }
    kopniak = atoi( arg );

    if ( kopniak < 1 )
    {
	progbug( ch, "mpdamage: obrazenia musza byc dodatnie (a nie %d)", kopniak );
	return;
    }

    if ( kopniak > 2000 )
    {
	progbug( ch, "mpdamage: mozna dac najwyzej 2000 obrazen (wiec nie %d)", kopniak );
	return;
    }

    if ( victim->deleted )
    {
	progbug( ch, "mpdamage: victim deleted!" );
	return;
    }

    /* czyste obrazenia, zadnych komunikatow - o reszte trzeba zadbac w progu */
    if ( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL )
	victim->hit = UMAX( victim->hit - kopniak, 1 ); /* wersja bez zabijania */
    else
    {
	victim->hit = victim->hit - kopniak;
	update_pos( victim );
	if ( victim->hit <= -10 )
	{
	    ROOM_INDEX_DATA *room;

	    if ( !IS_NPC( victim ) )
	    {
		sprintf( log_buf, "%s killed by mpdamage at %d", victim->short_descr, victim->in_room->vnum );
		log_string( log_buf );
		wiznet( "{M$N{x zosta`l$7 zabit$8 poprzez mpdamage.",
					victim, NULL, NULL, WIZ_DEATH, 0, 0 );
		strata_dosw_za_smierc( victim );
	    }

	    room = victim->in_room; /* wcale nie musi byc rowny ch->in_room */

	    if ( mprog_death_trigger( victim, ch ) )
		placz = FALSE;

	    if ( victim->deleted
	      || victim->in_room != room )
	    {
		return;
	    }

	    if ( placz )
		death_cry( victim, 0, victim->position );

	    raw_kill( ch, victim, FALSE, FALSE, ZGON_ZWYKLY );
	}
    }

    return;
}


/*
 * Lam 8.11.2005 dla Orenolda na zyczenie Ulryka
 */
KOMENDA_PROGOWA( do_mpstepto )
{
    char arg[ MIL ];
    ROOM_INDEX_DATA *location;
    int kier;

    one_argument( argument, arg );
    if ( !*arg )
    {
	progbug( ch, "mpstepto: brak argumentu" );
	return;
    }

    if ( !( location = find_location_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpstepto: nie znam miejsca '%s'", arg );
	return;
    }

    if ( ch->fighting )
    {
	progbug( ch, "mpstepto: trwa walka" );
	return;
    }

    if ( ch->position < POS_STANDING )
    {
	progbug( ch, "mpstepto: mob musi stac (a jest w pozycji %d)", ch->position );
	return;
    }

    if ( location->area != ch->in_room->area )
    {
	progbug( ch, "mpstepto: niedozwolone szukanie drog do innych krain" );
	return;
    }

/* chyba powinno sie to dac sprawdzic jakims ifem w progu, tak samo, jak
   zakluczenie drzwi, ale na razie zostawiam, niech sprawdzaja, czy sie ruszyl
   po find_path i move_char
    if ( !can_enter_room( ch, location, FALSE ) )
    {
	progbug( ch, "mpstepto: nie wolno wejsc do pomieszczenia" );
	return;
    }
*/

    kier = find_path( ch->in_room, location, 40000, TRUE, TRUE );

    if ( kier >= 0 && kier <= 9 )
	move_char( ch, kier, FALSE );

    return;
}


KOMENDA_PROGOWA( do_mpcomeback )
{
    if ( !IS_SET( ch->act, ACT_SENTINEL ) )
    {
	progbug( ch, "mpcomeback: mob nie jest stacjonarny" );
	return;
    }

    REMOVE_BIT( ch->act, ACT_DONT_COME_BACK );

    return;
}


KOMENDA_PROGOWA( do_mpdontcomeback )
{
    if ( !IS_SET( ch->act, ACT_SENTINEL ) )
    {
	progbug( ch, "mpcontcomeback: mob nie jest stacjonarny" );
	return;
    }

    SET_BIT( ch->act, ACT_DONT_COME_BACK );

    return;
}


KOMENDA_PROGOWA( do_mpcast )
{
    CHAR_DATA *caster, *victim;
    char arg[ MIL ];
    int sn, level;

    argument = one_argument( argument, arg );
    if ( !*arg )
    {
	progbug( ch, "mpcast: za malo argumentow" );
	return;
    }

    if ( !( caster = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpcast: nie znalazlem rzucajacego" );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !*arg )
    {
	progbug( ch, "mpcast: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpcast: nie znalazlem ofiary" );
	return;
    }

    argument = one_argument( argument, arg );
    if ( !*arg )
    {
	progbug( ch, "mpcast: za malo argumentow" );
	return;
    }

    if ( ( sn = skill_lookup_pl( arg ) ) < 1 )
    {
	progbug( ch, "mpcast: nie ma umiejetnosci \"%s\"", arg );
	return;
    }

    if ( !skill_table[ sn ].spell_fun
      || skill_table[ sn ].spell_fun == spell_null )
    {
	progbug( ch, "mpcast: umiejetnosc \"%s\" to nie czar", arg );
	return;
    }

    /* UWAGA! Brak sprawdzania pozycji, teoretyczna mozliwosc przegiec,
       trzeba pilnowac progow. */

    if ( skill_table[ sn ].target == TAR_OBJ_INV )
    {
	progbug( ch, "mpcast: nie obsluguje czarow rzucanych na przedmioty" );
	return;
    }

    level = atoi( argument );
    if ( level < 1 )
	level = caster->level;

    /* raczej nie ma sensu ustawianie name_expand( caster, victim ) */
    target_name = "";

    /* USAGE_CAST moze czasem uznawac, ze w przypadku, gdy caster to gracz,
       ma wycwiczone i probowac sprawdzic, jak dobrze - do dyskusji */
    if ( (*skill_table[ sn ].spell_fun) ( sn,
		SPELL_LEVEL( caster, level ), caster, victim, USAGE_CAST ) )
    {
	/* mprog_spell_victim_trigger?
	   ustawienie zmiennej, czy czar sie udal? */
	return;
    }

    return;
}


/*
 * Lam 6.1.2008
 */
KOMENDA_PROGOWA( do_mprestore )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	progbug( ch, "mprestore - brak argumentu" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mprestore - nie znaleziono ofiary \"%s\"", arg );
	return;
    }

    victim->hit  = UMAX( victim->max_hit, victim->hit );
    victim->mana = UMAX( victim->max_mana, victim->mana );
    victim->move = UMAX( victim->max_move, victim->move );
    update_pos( victim );

    return;
}


/*
 * Lam 7.1.2008
 */
KOMENDA_PROGOWA( do_mpchalign )
{
    CHAR_DATA *victim;
    char arg[ MIL ];
    int wart;

    argument = one_argument( argument, arg );
    if ( !*arg || !*argument )
    {
	progbug( ch, "mpchalign: za malo argumentow" );
	return;
    }

    if ( !( victim = get_char_room_prog( ch, arg, actor, obj, rndm, vict, v_obj ) ) )
    {
	progbug( ch, "mpchalign: nie znalazlem ofiary" );
	return;
    }

/*  if ( IS_NPC( victim ) ) - dopuszczamy? */

    if ( !is_number( argument ) )
    {
	progbug( ch, "mpchalign: drugi argument musi byc liczba (jest '%s')", argument );
	return;
    }
    wart = atoi( argument );

    victim->alignment = URANGE( -1000, wart, 1000 );

    return;
}
