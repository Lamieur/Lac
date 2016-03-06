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
 *  Zasada dzialania jak rowniez wiekszosc funkcji zostalo zaczerpnietych  *
 *  z dystrybucji Eye Of the Storm ][, ktora uzywa ILAB OLC 1.1b, ktora    *
 *  z kolei czerpie obficie z The Isles 1.1. Poklon autorom.               *
 *  Funkcje importowal, przystosowywal, a nawet pisal od nowa Lam.         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "merc.h"
#include "kzapisz.h"
#include "const.h"
#include "comm.h"


char	*rozpisz_flagi	args( ( int flagi, bool czy_bez_b00 ) );
void	save_mobprogs	args( ( FILE *fp, void *vo, int gdzie ) );
void	save_mobiles	args( ( FILE *fp, AREA_DATA *pArea ) );
void	save_objects	args( ( FILE *fp, AREA_DATA *pArea ) );
void	save_helps	args( ( FILE *fp, AREA_DATA *pArea ) );
int	drzwi		args( ( int locks ) );
void	save_rooms	args( ( FILE *fp, AREA_DATA *pArea ) );
void	save_resets	args( ( FILE *fp, AREA_DATA *pArea ) );


char *	const	wearloc_str	[ ]	=
{
    "`swiat`lo", "lewy palec", "prawy palec", "g`orna szyja", "dolna szyja",
    "cia`lo", "g`lowa", "nogi", "stopy", "d`lonie", "ramiona", "tarcza",
    "wok`o`l cia`la", "pas", "lewy nadgarstek", "prawy nadgarstek",
    "bro`n", "trzymane", "druga bro`n", "twarz", "MAX_WEAR!"
};

char nazwa[ MIL ];


/*
 * Lam: przystosowuje lancuch do zapisania w pliku krainy
 *
 * Lam 28.4.99: jesli enter == TRUE, to na koncu dodawany jest znak konca linii
 * (np. opisy pomieszczen)
 *
 * Lam 11.6.2005: formatuj - docelowo zawijacz wierszy i przerzucacz samotnych
 * literek, chwilowo wykrywacz przy lac -k -o
 */
char *fix_string( const char *str, bool enter, bool formatuj )
{
    static char strfix[ MAX_STRING_LENGTH * 2 ];
    int i;
    int o;
    int e;

    if ( str == NULL )
	return "\0";

    for ( i = o = e = 0; str[ o ] != '\0'; i++, o++ )
    {
	if ( str[ o ] == '\r' || str[ o ] == '~' )
	    o++;

	if ( ODPLUSKWIACZ && formatuj
	  && ( str[ o ] == '\n'
	    || str[ o ] == '\0' ) )
	{
	    strfix[ i ] = '\0';
	    if ( i > 1
	      && isspace( (unsigned char) strfix[ i - 2 ] )
	      && ( ( strfix[ i - 1 ] >= 'a'
		  && strfix[ i - 1 ] <= 'z' )
		|| ( strfix[ i - 1 ] >= 'A' /* wielka litera zaczynajaca zdanie */
		  && strfix[ i - 1 ] <= 'Z'
		  && i > 2
		  && strfix[ i - 3 ] == '.' ) ) )
	    {
		sprintf( log_buf, "[%s] Pojedyncza literka: %s", nazwa, &strfix[ e ] );
		log_string( log_buf );
	    }
	    if ( i - e > 79 )
	    {
		int dl = strlen_pl_kol_miel( &strfix[ e ] );
		if ( dl > 79 )
		{
		    sprintf( log_buf, "[%s] Za dluga linia (%d): %s", nazwa, dl, &strfix[ e ] );
		    log_string( log_buf );
		}
	    }
	    e = i + 1;
	}

	if ( str[ o ] == '\0' )
	    break;

	strfix[ i ] = str[ o ];
    }

    if ( enter && i && strfix[ i - 1 ] != '\n' ) /* Lam */
	strfix[ i++ ] = '\n';

    while ( !enter && i
	 && ( strfix[ i - 1 ] == '\n'
	   || strfix[ i - 1 ] == '\r' ) )
    {
	i--;
    }

    strfix[ i ] = '\0';

    return strfix;
}


char *rozpisz_flagi( int flagi, bool czy_bez_b00 )
{
    /* zdecydowalem sie na te 2 tablice ze wzgledu na duza szybkosc takiego
       rozwiazania. Wielokrotne przesuwanie bitow i sprintf( )owanie byloby duzo
       wolniejsze (aczkolwiek to by bylo mniej pisania) */
    static unsigned int bity[ 32 ] =  { b00, b01, b02, b03, b04, b05,
			b06, b07, b08, b09, b10, b11, b12,
			b13, b14, b15, b16, b17, b18, b19,
			b20, b21, b22, b23, b24, b25, b26,
			b27, b28, b29, b30, b31 };
    static char *nazwy[ 32 ] = { "b00", "b01", "b02", "b03", "b04", "b05",
			"b06", "b07", "b08", "b09", "b10", "b11", "b12",
			"b13", "b14", "b15", "b16", "b17", "b18", "b19",
			"b20", "b21", "b22", "b23", "b24", "b25", "b26",
			"b27", "b28", "b29", "b30", "b31" };
    static char bitstr[ MSL ];
    int i;
    bool bylo = FALSE;

    bitstr[ 0 ] = '\0';
    for ( i = czy_bez_b00 ? 1 : 0; i < 32; i++ )
    {
	if ( IS_SET( flagi, bity[ i ] ) )
	{
	    if ( bylo )
		strcat( bitstr, "|" );
	    else
		bylo = TRUE;

	    strcat( bitstr, nazwy[ i ] );
	}
    }

    return bitstr[ 0 ] ? bitstr : "0";
}


char *rozpisz_flagi_v_fun( int *flagi, int ilebitow, bool czy_bez_b00 )
{
    static char bitstr[ MSL ];
    char jedenbit[ MIL ];
    int pzl;
    bool bylo = FALSE;

    bitstr[ 0 ] = '\0';
    for ( pzl = czy_bez_b00 ? 1 : 0; pzl < ilebitow; pzl++ )
    {
	if ( IS_SET_V( flagi, pzl ) )
	{
	    if ( bylo ) strcat( bitstr, "|" ); else bylo = TRUE;
	    sprintf( jedenbit, "b%02d", pzl );
	    strcat( bitstr, jedenbit );
	}
    }

    return bitstr[ 0 ] ? bitstr : "0";
}


void save_mobprogs( FILE *fp, void *vo, int gdzie )
{
    MOB_INDEX_DATA *mob = (MOB_INDEX_DATA *) vo;
    ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA *) vo;
    EXIT_DATA *exit = (EXIT_DATA *) vo;
    OBJ_INDEX_DATA *obj = (OBJ_INDEX_DATA *) vo;
    MPROG_DATA *mprog;

    if ( ( gdzie == 0 && !mob->mobprogs )
      || ( gdzie == 1 && !room->mobprogs )
      || ( gdzie == 2 && !exit->mobprogs )
      || ( gdzie == 3 && !obj->mobprogs ) )
    {
	return;
    }

    mprog = ( gdzie == 0 ) ? mob->mobprogs
	  : ( gdzie == 1 ) ? room->mobprogs
	  : ( gdzie == 2 ) ? exit->mobprogs
	  : obj->mobprogs;
    for ( ; mprog != NULL; mprog = mprog->next )
    {
	/* Lam 10.4.2002, na razie zapisuje in_file_prog, a pliku nie rusza */
	if ( mprog->parent
	  || !str_cmp( mprog_type_to_name( mprog->type, gdzie ), "ERROR_PROG" ) )
	{
	    continue;
	}

	fprintf( fp, ">%s %s~\n", mprog_type_to_name( mprog->type, gdzie ),
	    fix_string( mprog->arglist, FALSE, FALSE ) );
	if ( str_cmp( mprog_type_to_name( mprog->type, gdzie ), "in_file_prog" ) )
	    fprintf( fp, "%s~\n", fix_string( mprog->comlist, TRUE, FALSE ) );
    }
    fprintf( fp, "|\n" );

    return;
}


/*
 * Funkcja dostosowana do potrzeb Laca
 * Lam 8.7.98 - pierwsze proby zapisywania krain
 *    22.8.98 - zrobiony konwerter do eksportu do formatu Laca 1.4
 */
void save_mobiles( FILE *fp, AREA_DATA *pArea )
{
    MOB_INDEX_DATA *pMobIndex;

    /* supermobowi zmieniala sie kraina w room/obj/exit_supermob( ) */
/*    get_mob_index( MOB_VNUM_SUPERMOB )->area = get_room_index( ROOM_VNUM_LIMBO )->area; */

    fprintf( fp, "#MOBILES\n" );
    for ( pMobIndex = pArea->first_mob; pMobIndex; pMobIndex = pMobIndex->next_in_area )
    {
	if ( pMobIndex->index_area != pArea )
	{
	    cbug( "Mob poza kraina", 0 );
	    continue;
	}

	fprintf( fp, "#%d\n",	pMobIndex->vnum );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->player_name, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->short_descr, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->dopelniacz, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->celownik, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->biernik, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->narzednik, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->miejscownik, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->wolacz, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->long_descr, FALSE, TRUE ) );
	fprintf( fp, "%s~\n",	fix_string( pMobIndex->description, TRUE, TRUE ) );
	fprintf( fp, "%d ",	pMobIndex->sex );
	fprintf( fp, "%s ",	rozpisz_flagi( pMobIndex->act, TRUE ) );
	fprintf( fp, "%s\n",	rozpisz_flagi_v( pMobIndex->affected_by, FALSE ) );
	fprintf( fp, "%d ",	pMobIndex->level );
	fprintf( fp, "%d ",	pMobIndex->alignment );
	fprintf( fp, "%d ",	pMobIndex->zamoznosc );
	fprintf( fp, "%s~\n",	race_table[ pMobIndex->race ].who_name );

	if ( pMobIndex->spec_fun )
	    fprintf( fp, "F %s\n", spec_name( pMobIndex->spec_fun ) );
	if ( pMobIndex->game_fun )
	    fprintf( fp, "K %s\n", game_name( pMobIndex->game_fun ) );
	if ( pMobIndex->pHealer )
	{
	    int snum;

	    fprintf( fp, "H" );
	    for ( snum = 0; snum < 5; snum ++ )
	    {
		if ( pMobIndex->pHealer->spells[ snum ] > 0 )
		    fprintf( fp, " \"%s\" %d",
			skill_table[ pMobIndex->pHealer->spells[ snum ] ].pl_name,
			pMobIndex->pHealer->cost[ snum ] );
		else
		    fprintf( fp, " \"xxx\" 0" );
	    }
	    fprintf( fp, "\n" );
	}
	if ( pMobIndex->pShop )
	{
	    int pom;

	    fprintf( fp, "S" );
	    for ( pom = 0; pom < MAX_TRADE; pom++ )
		fprintf( fp, " %d", pMobIndex->pShop->buy_type[ pom ] );
	    fprintf( fp, "\n  %d %d %d %d\n",
			pMobIndex->pShop->profit_buy,
			pMobIndex->pShop->profit_sell,
			pMobIndex->pShop->open_hour,
			pMobIndex->pShop->close_hour );
	}
	if ( pMobIndex->grupa )
	    fprintf( fp, "G %d\n", pMobIndex->grupa );
	if ( pMobIndex->mod_size )
	    fprintf( fp, "A %d %d\n", APPLY_SIZE, pMobIndex->mod_size );
	save_mobprogs( fp, pMobIndex, 0 );
	fprintf( fp, "\n" );
    }
    fprintf( fp, "#0\n\n\n" );

    return;
}


void save_objects( FILE *fp, AREA_DATA *pArea )
{
    OBJ_INDEX_DATA *pObjIndex;
    AFFECT_DATA *pAf;

    fprintf( fp, "#OBJECTS\n" );
    for ( pObjIndex = pArea->first_obj; pObjIndex; pObjIndex = pObjIndex->next_in_area )
    {
	if ( pObjIndex->area != pArea )
	{
	    cbug( "Przedmiot poza kraina", 0 );
	    continue;
	}
	fprintf( fp, "#%d\n",	pObjIndex->vnum );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->name, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->short_descr, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->dopelniacz, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->celownik, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->biernik, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->narzednik, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->miejscownik, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->description, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->look_descr, TRUE, TRUE ) );
	fprintf( fp, "%d ",	pObjIndex->rodzaj );
	fprintf( fp, "%d ",	pObjIndex->wielkosc );
	fprintf( fp, "%d ",	pObjIndex->item_type );
	fprintf( fp, "%s ",	rozpisz_flagi_v( pObjIndex->extra_flags, FALSE ) );
	fprintf( fp, "%s\n",	rozpisz_flagi( pObjIndex->wear_flags, FALSE ) );

	switch ( pObjIndex->item_type )
	{
	default:
	fprintf( fp, "%d~ %d~ %d~ %d~ %d~ %d~\n",
		pObjIndex->value[ 0 ].v,
		pObjIndex->value[ 1 ].v,
		pObjIndex->value[ 2 ].v,
		pObjIndex->value[ 3 ].v,
		pObjIndex->value[ 4 ].v,
		pObjIndex->value[ 5 ].v );
	    break;

	/* Alandar 04.10.2004: obsluga wartosci bedacych opisami */
	case ITEM_FOOD:
	case ITEM_DRINK_CON:
	fprintf( fp, "%d~ %d~ %d~ %d~ %s~ %d~\n",
		pObjIndex->value[ 0 ].v,
		pObjIndex->value[ 1 ].v,
		pObjIndex->value[ 2 ].v,
		pObjIndex->value[ 3 ].v,
		pObjIndex->value[ 4 ].p ?
		( char * ) pObjIndex->value[ 4 ].p : "",
		pObjIndex->value[ 5 ].v );
	    break;

	case ITEM_SPELLBOOK:
	fprintf( fp, "%s~ %d~ %d~ %d~ %d~ %d~\n",
		pObjIndex->value[ 0 ].v > 0 ?
		      skill_table[ pObjIndex->value[ 0 ].v ].pl_name
		    : "0",
		pObjIndex->value[ 1 ].v,
		pObjIndex->value[ 2 ].v,
		pObjIndex->value[ 3 ].v,
		pObjIndex->value[ 4 ].v,
		pObjIndex->value[ 5 ].v );
	    break;

	case ITEM_WEAPON:
	case ITEM_ARMOR:
	case ITEM_TREASURE:
	fprintf( fp, "%d~ %d~ %d~ %d~ %d~ %s~\n", /* material */
		pObjIndex->value[ 0 ].v,
		pObjIndex->value[ 1 ].v,
		pObjIndex->value[ 2 ].v,
		pObjIndex->value[ 3 ].v,
		pObjIndex->value[ 4 ].v,
		   pObjIndex->value[ 5 ].v >= 0
		&& pObjIndex->value[ 5 ].v < MAX_MATERIAL
		? material_table[ pObjIndex->value[ 5 ].v ].name
		: "nic" );
	    break;

	case ITEM_PILL:
	case ITEM_POTION:
	fprintf( fp, "%d~ %s~ %s~ %s~ %s~ %d~\n",
		pObjIndex->value[ 0 ].v,
		pObjIndex->value[ 1 ].v > 0 ?
		      skill_table[ pObjIndex->value[ 1 ].v ].pl_name
		    : "0",
		pObjIndex->value[ 2 ].v > 0 ?
		      skill_table[ pObjIndex->value[ 2 ].v ].pl_name
		    : "0",
		pObjIndex->value[ 3 ].v > 0 ?
		      skill_table[ pObjIndex->value[ 3 ].v ].pl_name
		    : "0",
		pObjIndex->value[ 4 ].p != 0 ?
		( char * ) pObjIndex->value[ 4 ].p : "",
		pObjIndex->value[ 5 ].v );
	    break;

	case ITEM_SCROLL:
	fprintf( fp, "%d~ %s~ %s~ %s~ %d~ %d~\n",
		pObjIndex->value[ 0 ].v,
		pObjIndex->value[ 1 ].v > 0 ?
		      skill_table[ pObjIndex->value[ 1 ].v ].pl_name
		    : "0",
		pObjIndex->value[ 2 ].v > 0 ?
		      skill_table[ pObjIndex->value[ 2 ].v ].pl_name
		    : "0",
		pObjIndex->value[ 3 ].v > 0 ?
		      skill_table[ pObjIndex->value[ 3 ].v ].pl_name
		    : "0",
		pObjIndex->value[ 4 ].v,
		pObjIndex->value[ 5 ].v );
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
	fprintf( fp, "%d~ %d~ %d~ %s~ %d~ %d~\n",
		pObjIndex->value[ 0 ].v,
		pObjIndex->value[ 1 ].v,
		pObjIndex->value[ 2 ].v,
		pObjIndex->value[ 3 ].v > 0 ?
		      skill_table[ pObjIndex->value[ 3 ].v ].pl_name
		    : "0",
		pObjIndex->value[ 4 ].v,
		pObjIndex->value[ 5 ].v );
	    break;
	}
	fprintf( fp, "%d ", pObjIndex->weight );
	fprintf( fp, "%s~ ",	fix_string( pObjIndex->anti_race, FALSE, FALSE ) );
	fprintf( fp, "%s~\n",	fix_string( pObjIndex->anti_class, FALSE, FALSE ) );

	if ( pObjIndex->action && *pObjIndex->action )
	    fprintf( fp, "U %s~\n", fix_string( pObjIndex->action, FALSE, FALSE ) );
	if ( pObjIndex->cena || pObjIndex->poziom )
	    fprintf( fp, "X %d %d\n", pObjIndex->cena, pObjIndex->poziom );
	if ( pObjIndex->qp )
	    fprintf( fp, "Q %d\n", pObjIndex->qp );
	for ( pAf = pObjIndex->affected; pAf; pAf = pAf->next )
	{
	    fprintf( fp, "A %d %d\n",  pAf->location, pAf->modifier );
	}
	save_mobprogs( fp, pObjIndex, 3 );
	fprintf( fp, "\n" );
    }
    fprintf( fp, "#0\n\n\n" );

    return;
}


void save_helps( FILE *fp, AREA_DATA *pArea )
{
    HELP_DATA *pHelp;
    bool found = FALSE;

    for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
	if ( pHelp->area == pArea )
	{
	    if ( !found )
	    {
		fprintf( fp, "#HELPS\n\n" );
		found = TRUE;
	    }
	    fprintf( fp, "%d %s~\n",
		pHelp->level,
		fix_string( pHelp->keyword, FALSE, FALSE ) );
/*	    fprintf( fp, "~ ~\n" );  przyklady, "patrz takze" */
	    fprintf( fp, "%s~\n",
		fix_string( pHelp->text, TRUE, TRUE ) );
	}

    if ( found )
	fprintf( fp, "\n0 $~\n\n\n" );


    return;
}


/* sciagnalem wlasna funkcje z db.c i odwrocilem dzialanie */
int drzwi( int locks )
{
	int info = 0;
	if ( locks )
	{
	    if ( IS_SET( locks, EX_PICKPROOF ) )
		SET_BIT( info, 1 );
	    if ( IS_SET( locks, EX_BASHPROOF ) )
		SET_BIT( info, 2 );
	    if ( IS_SET( locks, EX_PASSPROOF ) )
		SET_BIT( info, 4 );
	    if ( IS_SET( locks, EX_DIG ) )
		SET_BIT( info, 8 );
	    if ( IS_SET( locks, EX_DONTRESET ) )
		SET_BIT( info, 16 );
	    if ( IS_SET( locks, EX_BEZKLAMKI ) )
		SET_BIT( info, 32 );
	    if ( IS_SET( locks, EX_BEZZAMKA ) )
		SET_BIT( info, 64 );
	}
	if ( IS_SET( locks, EX_ISDOOR ) )
	    info++;

	return info;
}


void save_rooms( FILE *fp, AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoomIndex;
    EXTRA_DESCR_DATA *pEd;
    EXIT_DATA *pExit;
    int door;

    fprintf( fp, "#ROOMS\n" );
    for ( pRoomIndex = pArea->first_room; pRoomIndex; pRoomIndex = pRoomIndex->next_in_area )
    {
	if ( pRoomIndex->area != pArea )
	{
	    cbug( "Pomieszczenie poza kraina", 0 );
	    continue;
	}
	fprintf( fp, "#%d\n", pRoomIndex->vnum );
	fprintf( fp, "%s~\n", fix_string( pRoomIndex->name, FALSE, FALSE ) );
	fprintf( fp, "%s~\n", fix_string( pRoomIndex->miejscownik, FALSE, FALSE ) );
	fprintf( fp, "%s~\n", fix_string( pRoomIndex->description, TRUE, TRUE ) );
	fprintf( fp, "%d ",   pRoomIndex->pojemnosc );
	fprintf( fp, "%d ",   pRoomIndex->strata_ruchu );
	fprintf( fp, "0 " ); /* FIXME: oswietlenie */
	fprintf( fp, "%s\n",  rozpisz_flagi( pRoomIndex->original_room_flags, FALSE ) );
	fprintf( fp, "%d ",   pRoomIndex->sector_type );
	fprintf( fp, "%s~ ",  fix_string( pRoomIndex->anti_race, FALSE, FALSE ) );
	fprintf( fp, "%s~\n", fix_string( pRoomIndex->anti_class, FALSE, FALSE ) );

	/* UWAGA! Zapisuje takze pulapki bedace skutkami dzialania mobprogow
	   (mpmaketrap)!
	   FIXME: bool trap->czy_prog */
	if ( pRoomIndex->trap )
	{
	    fprintf( fp, "T %d\n%s~\n", pRoomIndex->trap->trap_type,
			fix_string( pRoomIndex->trap->trap_string, TRUE, TRUE ) );
	}

	for ( pEd = pRoomIndex->extra_descr; pEd; pEd = pEd->next )
	{
	    fprintf( fp, "E %s~\n",
		fix_string( pEd->keyword, FALSE, FALSE ) );
	    fprintf( fp, "%s~\n",
		fix_string( pEd->description, TRUE, TRUE ) );
	}

	save_mobprogs( fp, pRoomIndex, 1 );

	for ( door = 0; door < 10; door++ )
	{
	    if ( ( pExit = pRoomIndex->exit[ door ] ) )
	    {
		fprintf( fp, "D %s\n", kierunki[ door ].skrotd );
		fprintf( fp, "%s~\n",
		    fix_string( pExit->description, FALSE, TRUE ) );
		fprintf( fp, "%s~ ", fix_string( pExit->keyword, FALSE, FALSE ) );
		fprintf( fp, "%s~ ", fix_string( pExit->short_descr, FALSE, FALSE ) );
		fprintf( fp, "%s~\n", fix_string( pExit->biernik, FALSE, FALSE ) );
		fprintf( fp, "%d ", pExit->rodzaj );
		fprintf( fp, "%d %d %d\n",
		    drzwi( pExit->exit_info ),
		    pExit->key,
		    pExit->to_room ? pExit->to_room->vnum : 0 );
		save_mobprogs( fp, pExit, 2 );
	    }
	}
	fprintf( fp, "S\n\n" );
    }
    fprintf( fp, "#0\n\n\n\n" );

    return;
}


/*
 * Lam  9.7.98: napisane poprawnie
 * Lam 23.7.98: komentarze
 * Lam 15.1.00: komentarze po polsku, bo jest lpl
 */
void save_resets( FILE *fp, AREA_DATA *pArea )
{
    char *	const	door_reset_str	[ ]	=
    {
	"otw`orz", "zamknij", "zamknij, zaklucz", "zamknij, zakop",
	"zamknij, zaklucz, zakop"
    };

    RESET_DATA *pReset;
    MOB_INDEX_DATA *pLastMob = NULL;
    OBJ_INDEX_DATA *pLastObj = NULL;
    char buf[ MAX_STRING_LENGTH ];

    fprintf( fp, "#RESETS\n" );

    for ( pReset = pArea->reset_first; pReset; pReset = pReset->next )
    {
	switch ( pReset->command )
	{
	default:
	    bug( "Save_resets: bad command '%c'.", pReset->command );
	    break;

	case '*': /* Lam 10.4.2002 */
	    fprintf( fp, "*%s\n", pReset->string );
	    break;

	case 'M':
	    pLastMob = get_mob_index( pReset->arg1 );
	    fprintf( fp, "M %-5d %-3d %-5d\t; %s -> %s\n",
		pReset->arg1,
		pReset->arg2,
		pReset->arg3,
		pLastMob->short_descr,
		get_room_index( pReset->arg3 )->name );
	    break;

	case 'O':
	    pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "O %-5d %-3d %-5d\t; %s -> %s\n",
		pReset->arg1,
		pReset->arg2,
		pReset->arg3,
		pLastObj->short_descr,
		get_room_index( pReset->arg3 )->name );
	    break;

	case 'P':
	    if ( !pLastObj )
	    {
		sprintf( buf,
		    "Save_resets: 'P': !NO_OBJ! in [%s]", pArea->file_name );
		bug( buf, 0 );
		continue;
	    }
	    fprintf( fp, "P %-5d\t\t\t; %s -> %s\n",
		pReset->arg1,
		get_obj_index( pReset->arg1 )->short_descr,
		pLastObj->short_descr );
	    /* pLastObj sie nie zmienia - mozna wlozyc dwa przedmioty w jeden,
	       ale nie przedmiot w przedmiot wlozony w trzeci przedmiot */
	    break;

	case 'G':
	    if ( !pLastMob )
	    {
		sprintf( buf,
		    "Save_resets: 'G': !NO_MOB! in [%s]", pArea->file_name );
		bug( buf, 0 );
		continue;
	    }
	    fprintf( fp, "G %-5d\t\t\t; %s -> %s%s\n",
		pReset->arg1,
		get_obj_index( pReset->arg1 )->short_descr,
		pLastMob->short_descr,
		pLastMob->pShop ? " (sklep)" : "" );
	    pLastObj = get_obj_index( pReset->arg1 );
	    break;

	case 'E':
	    if ( !pLastMob )
	    {
		sprintf( buf,
		    "Save_resets: 'E': !NO_MOB! in [%s]", pArea->file_name );
		bug( buf, 0 );
		continue;
	    }
	    fprintf( fp, "E %-5d %-3d\t\t; %s -> %s (ubierz: %s)\n",
		pReset->arg1,
		pReset->arg3,
		get_obj_index( pReset->arg1 )->short_descr,
		pLastMob->short_descr,
		pReset->arg3 < 0 ? "nigdzie!" :
		  ( pReset->arg3 >= MAX_WEAR ? "nieznane!" :
		    wearloc_str[ pReset->arg3 ] ) );
	    pLastObj = get_obj_index( pReset->arg1 );
	    break;

	case 'D':
	    fprintf( fp, "D %-5d %-3s %-5d\t; %s/%s: %s\n",
		pReset->arg1,
		kierunki[ pReset->arg2 ].skrotd,
		pReset->arg3,
		get_room_index( pReset->arg1 )->name,
		kierunki[ pReset->arg2 ].krotki,
		door_reset_str[ pReset->arg3 ] );
	    break;

	case 'R':
	    fprintf( fp, "R %-5d %-3d\t\t; losuj wyj`scia: %s\n",
		pReset->arg1,
		pReset->arg2,
		get_room_index( pReset->arg1 )->name );
	    break;
	}
    }
    /* if ( GLACA )
	fprintf( fp, "\n* Krain`e zapisa`la glaca (" LAC_VERSION ")\nS\n\n\n" );
    else */
    fprintf( fp, "\n* Krain`e zapisa`l Lac (" LAC_VERSION ")\nS\n\n\n" );

    return;
}


void save_area( AREA_DATA *pArea, bool save_dir )
{
    ZONE_LIST *lista;
    FILE *fp;
    long int wiek;

    zamknij_reserve( fpReserve );
    sprintf( nazwa, "%s%s", save_dir ? SAVE_DIR : "", pArea->file_name );
    if ( !( fp = fopen( nazwa, "w" ) ) )
    {
	bug( "Save_area: fopen", 0 );
	lac_perror( nazwa );
	return;
    }

    wiek = wiek_krainy( pArea );

    fprintf( fp, "#AREA\n" );
    fprintf( fp, "Version  Lac-1.4.1~\n" );
    fprintf( fp, "Name     %s~\n", fix_string( pArea->name, FALSE, FALSE ) );
    fprintf( fp, "Long     %s~\n", fix_string( pArea->full_name, FALSE, FALSE ) );
    if ( pArea->flags )
	fprintf( fp, "Flags    %s\n", rozpisz_flagi( pArea->flags, FALSE ) );
    if ( pArea->revision )
	fprintf( fp, "Revision %s~\n", fix_string( pArea->revision, FALSE, FALSE ) );
    else
	fprintf( fp, "Revision $%sId: $~\n", "" );
    if ( pArea->recall && pArea->recall != pArea->strefy->zone->przywolanie )
	fprintf( fp, "Recall   %d\n", pArea->recall );
    if ( *pArea->reset_msg )
	fprintf( fp, "Reset    %s~\n", fix_string( pArea->reset_msg, FALSE, FALSE ) );
    if ( wiek - mud_age )
	fprintf( fp, "Timezone %ld\n", wiek - mud_age );
    for ( lista = pArea->strefy; lista; lista = lista->next )
	fprintf( fp, "Zone     %s~\n", fix_string( lista->zone->nazwa, FALSE, FALSE ) );
    if ( pArea->opis && *pArea->opis )
	fprintf( fp, "Descr\n%s~\n", fix_string( pArea->opis, TRUE, TRUE ) );
    fprintf( fp, "End\n\n\n" );

    save_helps( fp, pArea );
    save_mobiles( fp, pArea );
    save_objects( fp, pArea );
    save_rooms( fp, pArea );
    save_resets( fp, pArea );

    fprintf( fp, "#$\n" );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}
