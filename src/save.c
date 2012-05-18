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
#include <sys/stat.h>
#include <errno.h>

#if defined( sun )
#include <memory.h>
#endif

#if defined( sun )
int	system			args( ( const char *string ) );
#endif


void	rozbierz		args( ( CHAR_DATA *ch ) );
void	ubierz			args( ( CHAR_DATA *ch ) );
void	tnij_monety		args( ( OBJ_DATA *lista ) );
void	wytnij_naziemne		args( ( OBJ_DATA *lista ) );
void	zabierz_um_bwwu		args( ( CHAR_DATA *ch ) );
void	recover			args( ( FILE *fp, long fpos ) );

/*
 * Wersja formatu plikow - zmieniac przy kazdej zmianie powodujacej niemoznosc
 * wczytania plikow ze starej wersji i pisac oddzielne procedury dla roznych
 * wersji.
 * Wprowadzone przez Lama 25.2.98 (wersja 0, przed zapisywaniem rozebranych)
 */
#define	FILE_VERSION	9
/* Wersja 1: 27.2.98 - rozbieranie postaci przed zapisem */
/* Wersja 2: 2.7.99 - polskie ImmSkll */
/* Wersja 3: 2.3.2000 - ch->pcdata->opcje */
/* Wersja 4: 5.1.2003 - pytanie o wolacz */
/* Wersja 5: 15.9.2004 - denominacja i sciecie zlota */
/* Wersja 6: 9.4.2005 - odebranie dodatkowych umiejetnosci */
/* Wersja 7: 27.4.2005 - rozdzielenie KP ze zbroi i magicznego, dodatnia KP */
/* Wersja 8: 5.5.2007 - kolejne odebranie umiejetnosci, tym razem BWWU */
/* Wersja 9: 21.2.2008 - ponowna konwersja kilku dodatkowych ImmSkll */

/* Lam: globalna, dzieki niej informacja przenosi sie z fread_char do
   load_char_obj */
int save_ver = 0;

/* zapisywanie ubioru postaci */
OBJ_DATA *save_eq[ MAX_WEAR ];

/*
 * Array of containers read for proper re-nesting of objects.
 *
 * Lam: tu moga byc problemy z przechowalnia, chociaz nie powinny, dopoki ktos
 * w pliku nie umiesci obiektu z przechowalni w obiekcie z inwentarza...
 * Moga byc problemy w stylu: przedmiot z gracza nagle pojawia sie
 * w urnie klanu... Ale tylko jesli ktos namiesza w pliku, jesli plik bedzie
 * wyprodukowany przez mud, gwarantuje, ze nic sie nie stanie.
 */
#define MAX_NEST        100
static  OBJ_DATA *      rgObjNest       [ MAX_NEST ];



/*
 * Local functions.
 */
void    fwrite_char     args( ( CHAR_DATA *ch,  FILE *fp ) );
int     fread_char      args( ( CHAR_DATA *ch,  FILE *fp ) );
		/* use = 0 (char), 1 (przechowalnia), 2 (obj), 3 (room) */
void	konwersja_opcji args( ( CHAR_DATA *ch ) );


/*
 * Lam: rozebranie gracza przed zapisem postaci
 * Pomysl jest sciagniety ze Smauga, wykonanie tez wzorowane
 */
void rozbierz( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    int gdzie;

    for ( gdzie = 0; gdzie < MAX_WEAR; gdzie++ )
	save_eq[ gdzie ] = NULL;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	if ( ( obj->wear_loc > -1 ) && ( obj->wear_loc < MAX_WEAR ) )
	{
	    if ( save_eq[ obj->wear_loc ] )
		bug( "rozbierz( ): 2 rzeczy na jednym miejscu (%d)", obj->wear_loc );
	    else
		save_eq[ obj->wear_loc ] = obj;
	    unequip_char_save( ch, obj );
	}
    }

    return;
}


void ubierz( CHAR_DATA *ch )
{
    int gdzie;

    /* Lam 16.5.2005: zmiana sposobu uzywania swiatla */
    if ( save_eq[ WEAR_LIGHT ]
      && ( save_eq[ WEAR_HOLD ]
	|| save_eq[ WEAR_WIELD_2 ] ) )
	save_eq[ WEAR_LIGHT ] = NULL;

    for ( gdzie = 0; gdzie < MAX_WEAR; gdzie++ )
	if ( save_eq[ gdzie ] )
	{
	    /* Lam 20.4.2004: jesli przedmiot zmienil sie w krainie, nie nosmy
		pasa na ramionach nawet, jesli wczesniej bylo to mozliwe.
		W ten sposob po prostu wskoczy do inwentarza. */
	    if ( can_equip_at( save_eq[ gdzie ], gdzie ) )
		save_equip_char( ch, save_eq[ gdzie ], gdzie );
	    save_eq[ gdzie ] = NULL;
	}

    return;
}


/* Courtesy of Yaz of 4th Realm */
char *initial( const char *str )
{
    static char strint[ 4 ];

    strint[ 0 ] = LOWER( str[ 0 ] );

    if ( str[ 0 ] == '`' )
	strint[ 0 ] = LOWER( str[ 1 ] );

    if ( str[ 0 ] == '`' && strint[ 0 ] == 'x' )
	strint[ 0 ] = 'z';

    return strint;
}


/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
    FILE *fp;
    char  buf     [ MSL ];
    char  strsave [ MSL ];

    if ( IS_NPC( ch ) /* || ( ch->level < 2 && ch->level >= 0 ) */ )
	return;

    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

    /* rozbierzemy ch, zeby nie bylo zamieszania z rzeczami */
    rozbierz( ch );

    ch->save_time = current_time;
    zamknij_reserve( fpReserve );

    /* player files parsed directories by Yaz 4th Realm */
#if !defined( macintosh ) && !defined( MSDOS )
    sprintf( strsave, PLAYER_DIR "%s" DIR_SEPARATOR "%s",
	initial( ch->name ), capitalize( ch->name ) );
    /* Kopia w backup/ dziala w nastepujacy sposob:
	- jesli gracz wszedl do gry i zapisuje, to znaczy, ze plik mial
	  dobry, wiec:
	- stary plik jest przenoszony do katalogu backup przy probie
	  zapisania postaci, ale tylko przy pierwszej probie w danej sesji
	- jesli postac zle sie zapisze, zawsze jest pod reka kopia
	  sprzed ostatniej sesji gry gracza
    */
    {
	char strbkp[ MIL ];
	sprintf( strbkp, BACKUP_DIR "%s" DIR_SEPARATOR "%s",
	    initial( ch->name ), capitalize( ch->name ) );
	if ( !IS_SET( ch->pcdata->temp_flags, TEMPFLAG_BACKUP ) )
	{
	    rename( strsave, strbkp );
	    /* ignoruje zwracana przez rename( ) wartosc, poniewaz nawet jesli
	       bedzie blad, to nas to niezbyt obchodzi :) */
	    SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_BACKUP );
	}
    }
#else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
#endif
    if ( !( fp = fopen( strsave, "w" ) ) )
    {
	sprintf( buf, "Save_char_obj: fopen %s: ", ch->name );
	bug( buf, 0 );
	lac_perror( strsave );
	STC( "Nie mo`zna zapisa`c twojej postaci!\n\r", ch );
    }
    else
    {
	fwrite_char( ch, fp );
	if ( ch->carrying )
	    fwrite_obj( ch, ch->carrying, fp, 0, FALSE );
	if ( ch->przechowalnia )
	    fwrite_obj( ch, ch->przechowalnia, fp, 0, TRUE );
	fprintf( fp, "#END\n" );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    /* mozemy ch z powrotem ubrac :) */
    ubierz( ch );

    return;
}



/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    AFFECT_DATA  *paf;
    ALIAS_DATA   *alias;
    int           sn;
    MPQUEST_DATA *mpquest;
    REFUSE_DATA  *refuse;
    RESTART_DATA *restart;
    int		  tp;
    int           wplywy[ AFF_FLAGS_SIZE ];

    fprintf( fp, "#%s\n", IS_NPC( ch ) ? "MOB" : "PLAYER"       );

    fprintf( fp, "FileVersion %d\n",	FILE_VERSION		);
    fprintf( fp, "Nm          %s~\n",   ch->name                );
    if ( !IS_NPC( ch ) ) /* Lam: to chce miec na poczatku pliku */
    {
	fprintf( fp, "Login       %s~\n",   ch->pcdata->login	);
	fprintf( fp, "Email       %s~\n",   ch->pcdata->email	);
	if ( ch->pcdata->strefa )
	    fprintf( fp, "Zn          %s~\n",	ch->pcdata->strefa->nazwa );
    }
    fprintf( fp, "ShtDsc      %s~\n",   ch->short_descr         );
    fprintf( fp, "Dopelniacz  %s~\n",   ch->dopelniacz          );
    fprintf( fp, "Celownik    %s~\n",   ch->celownik            );
    fprintf( fp, "Biernik     %s~\n",   ch->biernik             );
    fprintf( fp, "Narzednik   %s~\n",   ch->narzednik           );
    fprintf( fp, "Miejscownik %s~\n",   ch->miejscownik         );
    fprintf( fp, "Wolacz      %s~\n",	ch->wolacz		);
    fprintf( fp, "LngDsc      %s~\n",   ch->long_descr          );
    fprintf( fp, "LngDscOrg   %s~\n",   ch->long_descr_orig     );
    fprintf( fp, "FalseName   %s~\n",   ch->false_name          );
    fprintf( fp, "Dscr        %s~\n",   ch->description         );
    fprintf( fp, "Sx          %d\n",    ch->sex                 );
    fprintf( fp, "Cla         %d\n",    ch->klasa               );

    fprintf( fp, "Race        %s~\n",   race_table[ ch->race ].ang );

    fprintf( fp, "Lvl         %d\n",    ch->level               );
    fprintf( fp, "Trst        %d\n",    ch->trust               );
    fprintf( fp, "Playd       %d\n",
	ch->played + (int) ( current_time - ch->logon )         );
    fprintf( fp, "Note        %ld\n",   (unsigned long) ch->last_note );
    fprintf( fp, "Room        %d\n",
      ( ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
       && ch->pcdata && ch->pcdata->was_in_room )
      ? ( ch->pcdata->was_in_room == get_room_index( ROOM_VNUM_POZA_SWIATEM )
	 && ch->pcdata->ic_in_room )
	? ch->pcdata->ic_in_room->vnum
	: ch->pcdata->was_in_room->vnum
      : ( PP( ch ) && ch->pcdata && ch->pcdata->ic_in_room )
	? ch->pcdata->ic_in_room->vnum
	: ch->in_room->vnum );

    fprintf( fp, "HpMnMv      %d %d %d %d %d %d\n",
	ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
    fprintf( fp, "Gold        %d\n",    ch->gold                );
    fprintf( fp, "Bank        %d\n",    ch->pcdata->bank        );
    fprintf( fp, "Exp         %d\n",    ch->exp                 );
    fprintf( fp, "QuestP      %d\n",    ch->qp                  );
    if ( !IS_SET( ch->act, b20 ) ) /* PLR_DENY */
	fprintf( fp, "Act         %d\n",    ch->act                 );
    else
    { /* zeby bylo latwiej odblokowac gracza - wystarczy usunac b20 w pliku */
	REMOVE_BIT( ch->act, b20 );
	fprintf( fp, "Act         %d|b20\n",ch->act                 );
	SET_BIT( ch->act, b20 );
    }
    fprintf( fp, "Info        %d\n",    ch->info                );
    fprintf( fp, "Pol         %d\n",    ch->polskie             );
    fprintf( fp, "Kolor       %d\n",	ch->kolor		);
    fprintf( fp, "Podswietl   %d\n",	ch->podswietl		);

    COPY_BITS( wplywy, ch->affected_by );
    REMOVE_BIT_V( wplywy, AFF_CHARM );
    REMOVE_BIT_V( wplywy, AFF_BRZECZY );
    fprintf( fp, "AffectV     %s\n", rozpisz_flagi_v( wplywy, FALSE ) );

    tp = PP( ch ) && ch->pcdata && ch->pcdata->ic_position
		? ch->pcdata->ic_position : ch->position;
    /* Bug fix from Alander */
    fprintf( fp, "Pos         %d\n",
	    tp == POS_FIGHTING ? POS_STANDING : tp );

    fprintf( fp, "Prac        %d\n",    ch->practice            );
    fprintf( fp, "SavThr      %d\n",    ch->saving_throw        );
    fprintf( fp, "Align       %d\n",    ch->alignment           );
    fprintf( fp, "Hit         %d\n",    ch->hitroll             );
    fprintf( fp, "Dam         %d\n",    ch->damroll             );
/*    fprintf( fp, "Armr        %d\n",    ch->armor               ); */
    fprintf( fp, "MArmr       %d\n",    ch->marmor              );
    fprintf( fp, "Wimp        %d\n",    ch->wimpy               );
    fprintf( fp, "Deaf        %d\n",    ch->deaf                );

    if ( IS_NPC( ch ) )
    {
	fprintf( fp, "Vnum        %d\n",        ch->pIndexData->vnum    );
    }
    else
    {
	if ( ch->pcdata->hp_angel )
	    fprintf( fp, "Angel       %d\n",	ch->pcdata->hp_angel    );
	fprintf( fp, "Opcje       %d\n",	ch->pcdata->opcje       );
	fprintf( fp, "Prmpt       %s~\n",	ch->pcdata->prompt      );
	if ( ch->pcdata->fprompt && *ch->pcdata->fprompt )
	    fprintf( fp, "Fprmpt      %s~\n",   ch->pcdata->fprompt     );
	fprintf( fp, "Separator   %c~\n",	ch->separator		);
	fprintf( fp, "Barwy       %s~\n",	ch->pcdata->kolor	);
	if ( ch->pcdata->stopien && *ch->pcdata->stopien )
	    fprintf( fp, "Stpn        %s~\n",   ch->pcdata->stopien     );
	if ( ch->pcdata->clan )
	    fprintf( fp, "Clan        %s~\n",   ch->pcdata->clan->name  );
	fprintf( fp, "Paswd       %s~\n",	ch->pcdata->pwd		);
	if ( *ch->pcdata->bamfin )
	    fprintf( fp, "Bmfin       %s~\n",	ch->pcdata->bamfin	);
	if ( *ch->pcdata->bamfout )
	    fprintf( fp, "Bmfout      %s~\n",	ch->pcdata->bamfout	);
	if ( *ch->pcdata->immskll )
	    fprintf( fp, "Immskll     %s~\n", 	ch->pcdata->immskll	);
	if ( ch->pcdata->wiznet )
	    fprintf( fp, "Wiznet      %d\n",	ch->pcdata->wiznet	);
	if ( *ch->pcdata->szpic_wybr )
	    fprintf( fp, "SzpWbr      %s~\n",	ch->pcdata->szpic_wybr	);
	if ( *ch->pcdata->szpic_wykl )
	    fprintf( fp, "SzpWkl      %s~\n",	ch->pcdata->szpic_wykl	);
	fprintf( fp, "Ttle        %s~\n",       ch->pcdata->title       );
	if ( ch->pcdata->wizinvis )
	    fprintf( fp, "Wizinvis    %d\n",	ch->pcdata->wizinvis	);
	if ( ch->pcdata->niesmpoz != 103 )
	    fprintf( fp, "Niesmpoz    %d\n",	ch->pcdata->niesmpoz	);
	if ( ch->pcdata->hint )
	    fprintf( fp, "Hint        %d\n",	ch->pcdata->hint	);

	/* nalozone na gracza kary w ch->act, a sprawcy ponizej */
	if ( ch->pcdata->love.od      && *ch->pcdata->love.od )
	    fprintf( fp, "LovedBy2    %d %s~\n",
		(int) ch->pcdata->love.do_kiedy, ch->pcdata->love.od	     );
	if ( ch->pcdata->silence.od   && *ch->pcdata->silence.od )
	    fprintf( fp, "SilencedBy2 %d %s~\n",
		(int) ch->pcdata->silence.do_kiedy, ch->pcdata->silence.od   );
	if ( ch->pcdata->noemote.od   && *ch->pcdata->noemote.od )
	    fprintf( fp, "NoemotedBy2 %d %s~\n",
		(int) ch->pcdata->noemote.do_kiedy, ch->pcdata->noemote.od   );
	if ( ch->pcdata->nocolour.od  && *ch->pcdata->nocolour.od )
	    fprintf( fp, "NocolouredBy2 %d %s~\n",
		(int) ch->pcdata->nocolour.do_kiedy, ch->pcdata->nocolour.od );
	if ( ch->pcdata->notell.od    && *ch->pcdata->notell.od )
	    fprintf( fp, "NotelledBy2 %d %s~\n",
		(int) ch->pcdata->notell.do_kiedy, ch->pcdata->notell.od     );
	if ( ch->pcdata->log.od       && *ch->pcdata->log.od )
	    fprintf( fp, "LoggedBy2   %d %s~\n",
		(int) ch->pcdata->log.do_kiedy, ch->pcdata->log.od	     );
	if ( ch->pcdata->freeze.od    && *ch->pcdata->freeze.od )
	    fprintf( fp, "FreezedBy2  %d %s~\n",
		(int) ch->pcdata->freeze.do_kiedy, ch->pcdata->freeze.od     );
	if ( ch->pcdata->deny.od      && *ch->pcdata->deny.od )
	    fprintf( fp, "DenyBy2     %d %s~\n",
		(int) ch->pcdata->deny.do_kiedy, ch->pcdata->deny.od	     );
	if ( ch->pcdata->thief_from && *ch->pcdata->thief_from )
	    fprintf( fp, "ThiefFrom   %s~\n",	ch->pcdata->thief_from	);
	if ( ch->pcdata->killer_from && *ch->pcdata->killer_from )
	    fprintf( fp, "KillerFrom  %s~\n",	ch->pcdata->killer_from	);
	if ( ch->pcdata->zabojca_from && *ch->pcdata->zabojca_from )
	    fprintf( fp, "ZabojcaFrom %s~\n",	ch->pcdata->zabojca_from);
	if ( ch->pcdata->wampiryzm.pochodzenie && *ch->pcdata->wampiryzm.pochodzenie )
	    fprintf( fp, "VampireFrom %s~\n",   ch->pcdata->wampiryzm.pochodzenie );
	if ( ch->pcdata->wampiryzm.rasa != -1 )
	    fprintf( fp, "OrigRace    %d\n",    ch->pcdata->wampiryzm.rasa );
	if ( ch->pcdata->wampiryzm.strefa && *ch->pcdata->wampiryzm.strefa )
	    fprintf( fp, "OrigZone    %s~\n",   ch->pcdata->wampiryzm.strefa );
	if ( ch->pcdata->wampiryzm.ile != 0 )
	    fprintf( fp, "VIle        %d\n",    ch->pcdata->wampiryzm.ile );
	if ( ch->pcdata->wampiryzm.moc != -1 )
	    fprintf( fp, "VMoc        %d\n",    ch->pcdata->wampiryzm.moc );
	fprintf( fp, "Ignore      %s~\n",	ch->pcdata->ignore	);
	fprintf( fp, "Pojedynek   %s~\n",	ch->pcdata->pojedynek	);
	fprintf( fp, "Nienawisc   %s~\n",	ch->pcdata->nienawisc	);


	fprintf( fp, "AtrPrm      %d %d %d %d %d\n",
		ch->pcdata->perm_str,
		ch->pcdata->perm_int,
		ch->pcdata->perm_wis,
		ch->pcdata->perm_dex,
		ch->pcdata->perm_con );

	fprintf( fp, "AtrMd       %d %d %d %d %d\n",
		ch->mod_str,
		ch->mod_int,
		ch->mod_wis,
		ch->mod_dex,
		ch->mod_con );

	fprintf( fp, "Cond        %d %d %d\n",
		ch->pcdata->condition[ 0 ],
		ch->pcdata->condition[ 1 ],
		ch->pcdata->condition[ 2 ] );

	fprintf( fp, "Kill        %d %d\n",
		ch->pcdata->killed,
		ch->pcdata->dead );
	fprintf( fp, "PKll        %d %d\n",
		ch->pcdata->pkilled,
		ch->pcdata->pdead );
	fprintf( fp, "Scde        %d %d\n",
		ch->pcdata->wykrwawienia,
		ch->pcdata->samobojstwa );

	fprintf( fp, "Pglen       %d\n",   ch->pcdata->pagelen     );

	for ( mpquest = ch->mpquest; mpquest; mpquest = mpquest->next )
	{
	    fprintf( fp, "MPQD2       %d %d %d %d %d %d %d %s~ %s~\n",
		mpquest->vnum, mpquest->stage, mpquest->data[ 0 ],
		mpquest->data[ 1 ], mpquest->data[ 2 ], mpquest->data[ 3 ],
		mpquest->data[ 4 ], mpquest->title, mpquest->descr );
	}

	for ( restart = ch->pcdata->restart; restart; restart = restart->nast )
	    fprintf( fp, "RHstr       %s~\n", restart->wpis );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[ sn ].old_name && ch->pcdata->learned[ sn ] > 0 )
	    {
		fprintf( fp, "Skll2       %d %d %d '%s'\n",
		    ch->pcdata->learned_level[ sn ], ch->pcdata->learned[ sn ],
		    ch->pcdata->used[ sn ], skill_table[ sn ].pl_name );
	    }
	}
	/* uzywam zmiennej sn w innych celach - pseudo-wieloprofesjowosc */
	for ( sn = 0; sn < 5; sn++ )
	    if ( ch->pcdata->multi[ sn ] > 0 )
	    {
		fprintf( fp, "MltSkll     '%s'\n",
		    skill_table[ ch->pcdata->multi[ sn ] ].old_name );
	    }
    } /* !IS_NPC( ch ) */

    for ( paf = ch->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;
	/* Ulryk: nie zapisuje kontroli umyslow, bo:
	1) dla gracza oznacza, ze ma aktywnego sluge. (A kiedy gracz wyjdzie
	   z gry, to straci sluge, bo poki co sie nie zapisuja)
	2) dla moba oznacza oczarowanie, ale co nas obchodza moby? */
	if ( paf->type == gsn_kontrola_umyslu )
	    continue;

	/* Lam 13.1.2004: Afft3, zapisywany poziom, ale rzucajacy czar nie */
	/* Lam 17.5.2006: Afft4, z wektorem */
	fprintf( fp, "Afft4      %18s~ %3d %3d %3d %3d %s\n",
		skill_table[ paf->type ].old_name,
		paf->duration,
		paf->modifier,
		paf->location,
		paf->level,
		rozpisz_flagi_v( paf->bitvector, FALSE ) );
    }

    /*) slynna czesc zapisujaca aliasy (*/
    for ( alias = ch->alias; alias; alias = alias->next )
	fprintf( fp, "Alias       %s %s~\n",
		alias->name,
		alias->todo );

    /* komenda odmow */
    for ( refuse = ch->pcdata->refuse; refuse; refuse = refuse->next )
	fprintf( fp, "Refuse      '%s' '%s' %s %s~\n",
		refuse->refused_cmd,
		refuse->who,
		refuse->date,
		refuse->why );

    fprintf( fp, "End\n\n" );

    return;
}


/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, bool own )
{
    AFFECT_DATA      *paf;
    int gdzie, wear_loc; /* zapisywanie pomimo rozebrania */

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->next_content )
	fwrite_obj( ch, obj->next_content, fp, iNest, own );

    /*
     * Castrate storage characters.
     *
     * Poprawka Lama: 3 poziomy roznicy sa dopuszczalne...
     * Przechowalnia ma ograniczenie poziomu w do_przechowaj( )
     * Lam 25.12.1999: ITEM_VANISH znika jak klucze, ale nie w przechowalni
     */
    if ( !IS_OBJ_STAT( obj, ITEM_QUEST )
      && ( ( !own && ch && ch->level >= 0 && ch->level < LEVEL_IMMORTAL
	  && ( ch->level + 3 ) < obj->level )
	|| obj->item_type == ITEM_KEY
	|| obj->item_type == ITEM_BEACON
	|| ( !own && IS_OBJ_STAT( obj, ITEM_VANISH ) )
	|| obj->deleted ) )
	return;

    if ( !own )
	fprintf( fp, "#OBJECT\n" );
      else
	fprintf( fp, "#PRZECHOWALNIA\n" );
/* Lam 8.5.98 - Nest 0 tez wywalic z pliku! */
    if ( iNest )
	fprintf( fp, "Nest         %d\n",   iNest                    );
/* Lam 7.5.98: zeby pliki mniej zajmowaly. Strcmp jest szybka i odroznia
   wielkie litery od malych, czyli to jest to, co byc powinno. */
    if ( strcmp( obj->name, obj->pIndexData->name ) )
	fprintf( fp, "Name         %s~\n",  obj->name                );
    if ( strcmp( obj->short_descr, obj->pIndexData->short_descr ) )
	fprintf( fp, "ShortDescr   %s~\n",  obj->short_descr         );
    if ( strcmp( obj->dopelniacz, obj->pIndexData->dopelniacz ) )
	fprintf( fp, "Dopelniacz   %s~\n",  obj->dopelniacz          );
    if ( strcmp( obj->celownik, obj->pIndexData->celownik ) )
	fprintf( fp, "Celownik     %s~\n",  obj->celownik            );
    if ( strcmp( obj->biernik, obj->pIndexData->biernik ) )
	fprintf( fp, "Biernik      %s~\n",  obj->biernik             );
    if ( strcmp( obj->narzednik, obj->pIndexData->narzednik ) )
	fprintf( fp, "Narzednik    %s~\n",  obj->narzednik           );
    if ( strcmp( obj->miejscownik, obj->pIndexData->miejscownik ) )
	fprintf( fp, "Miejscownik  %s~\n",  obj->miejscownik         );
    if ( strcmp( obj->description, obj->pIndexData->description ) )
	fprintf( fp, "Description  %s~\n",  obj->description         );
    if ( strcmp( obj->look_descr, obj->pIndexData->look_descr ) )
	fprintf( fp, "LookDescr    %s~\n",  obj->look_descr          );
    if ( strcmp( obj->action, obj->pIndexData->action ) )
	fprintf( fp, "Action       %s~\n",  obj->action              );
    if ( strcmp( obj->anti_class, obj->pIndexData->anti_class ) )
	fprintf( fp, "AClass       %s~\n",  obj->anti_class          );
    if ( strcmp( obj->anti_race, obj->pIndexData->anti_race ) )
	fprintf( fp, "ARace        %s~\n",  obj->anti_race           );
    if ( *obj->owner )
	fprintf( fp, "Owner        %s~\n",  obj->owner               );
    fprintf( fp, "Vnum         %d\n",   obj->pIndexData->vnum        );
    if ( obj->rodzaj != obj->pIndexData->rodzaj )
	fprintf( fp, "Rodzaj       %d\n",   obj->rodzaj              );
    if ( !SAME_BITS( obj->extra_flags, obj->pIndexData->extra_flags ) )
	fprintf( fp, "ExtraFlags2  %s\n",   rozpisz_flagi_v( obj->extra_flags, FALSE ) );
    if ( obj->wear_flags != obj->pIndexData->wear_flags )
	fprintf( fp, "WearFlags    %d\n",   obj->wear_flags          );

    wear_loc = -1; /* WEAR_NONE */
    for ( gdzie = 0; gdzie < MAX_WEAR; gdzie++ )
	if ( obj == save_eq[ gdzie ] )
	{
	    wear_loc = gdzie;
	    break;
	}
/* jesli nie byl zalozony, to wear_loc == -1; */
    /* ponizej bylo obj->wear_loc */
    if ( wear_loc != -1 )
	fprintf( fp, "WearLoc      %d\n",   wear_loc                 );
    if ( obj->item_type != obj->pIndexData->item_type )
	fprintf( fp, "ItemType     %d\n",   obj->item_type           );
    if ( obj->weight != obj->pIndexData->weight )
	fprintf( fp, "Weight       %d\n",   obj->weight              );
    /* w pliku najwiecej jest zawsze jedzenia z piekarni na 0 poziomie... */
    if ( obj->level )
	fprintf( fp, "Level        %d\n",   obj->level               );
    if ( obj->timer != -1 )
	fprintf( fp, "Timer        %d\n",   obj->timer               );
    fprintf( fp, "Cost         %d\n",   obj->cost                    );
    if ( obj->plr_shop_cost )
	fprintf( fp, "PlrCost      %d\n",   obj->plr_shop_cost       );
    /* Lam: zeby nie wpisywalo w4 jako -151608008 - mniejszy plik ;) */
    if ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON
      || obj->item_type == ITEM_POTION || obj->item_type == ITEM_PILL )
	fprintf( fp, "Val          %d %d %d %d 0 %d\n",
	    obj->value[ 0 ].v, obj->value[ 1 ].v, obj->value[ 2 ].v, obj->value[ 3 ].v,
	    obj->value[ 5 ].v );
    else fprintf( fp, "Val          %d %d %d %d %d %d\n",
	    obj->value[ 0 ].v, obj->value[ 1 ].v, obj->value[ 2 ].v, obj->value[ 3 ].v,
	    obj->value[ 4 ].v, obj->value[ 5 ].v );

    switch ( obj->item_type )
    {
    case ITEM_FOOD:
    case ITEM_DRINK_CON:
	/* Alandar 04.10.2004: obsluga wartosci przedmiotow bedacych opisami */
	if ( str_cmp( obj->value[ 4 ].p ? ( char * ) obj->value[ 4 ].p : "0",
	  obj->pIndexData->value[ 4 ].p ? ( char * ) obj->pIndexData->value[ 4 ].p : "0" ) )
	    fprintf( fp, "String 4     %s~\n",
	      obj->value[ 4 ].p ? ( char * ) obj->value[ 4 ].p : "0" );
	break;
    case ITEM_POTION:
    case ITEM_PILL:
	if ( str_cmp( obj->value[ 4 ].p ? ( char * ) obj->value[ 4 ].p : "0",
	  obj->pIndexData->value[ 4 ].p ? ( char * ) obj->pIndexData->value[ 4 ].p : "0" ) )
	    fprintf( fp, "String 4     %s~\n",
	      obj->value[ 4 ].p ? ( char * ) obj->value[ 4 ].p : "0" );
    case ITEM_SCROLL:
	if ( obj->value[ 1 ].v > 0 )
	    fprintf( fp, "Spell 1      '%s'\n",
		skill_table[ obj->value[ 1 ].v ].old_name );

	if ( obj->value[ 2 ].v > 0 )
	    fprintf( fp, "Spell 2      '%s'\n",
		skill_table[ obj->value[ 2 ].v ].old_name );

	if ( obj->value[ 3 ].v > 0 )
	    fprintf( fp, "Spell 3      '%s'\n",
		skill_table[ obj->value[ 3 ].v ].old_name );
	break;

    case ITEM_STAFF:
    case ITEM_WAND:
	if ( obj->value[ 3 ].v > 0 )
	    fprintf( fp, "Spell 3      '%s'\n",
		skill_table[ obj->value[ 3 ].v ].old_name );
	break;

    case ITEM_SPELLBOOK:
	if ( obj->value[ 0 ].v > 0 )
	    fprintf( fp, "Spell 0      '%s'\n",
		skill_table[ obj->value[ 0 ].v ].old_name );
	break;
    case ITEM_ARMOR:
    case ITEM_WEAPON:
    case ITEM_TREASURE:
	if ( obj->value[ 5 ].v > 0 && obj->value[ 5 ].v < MAX_MATERIAL )
	    fprintf( fp, "Material 5   '%s'\n",
		material_table[ obj->value[ 5 ].v ].name );
	else
	    fprintf( fp, "Material 5   'nic'\n");
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	fprintf( fp, "Affect3      '%s' %d %d %d %s\n",
		skill_table[ paf->type ].old_name,
		paf->duration,
		paf->modifier,
		paf->location,
		rozpisz_flagi_v( paf->bitvector, FALSE ) );
    }

    fprintf( fp, "End\n\n" );

    if ( obj->contains )
	fwrite_obj( ch, obj->contains, fp, iNest + 1, own );

    tail_chain( );

    return;
}


CHAR_DATA *nowa_postac( DESCRIPTOR_DATA *d, char *name )
{
    CHAR_DATA *ch;
    extern char *daPrompt;
    int i;

    ch                                  = new_character( TRUE );

    d->character                        = ch;
    ch->desc                            = d;
    ch->name                            = str_dup( name );
    ch->short_descr                     = str_dup( name );
    ch->dopelniacz                      = str_dup( name );
    ch->celownik                        = str_dup( name );
    ch->biernik                         = str_dup( name );
    ch->narzednik                       = str_dup( name );
    ch->miejscownik                     = str_dup( name );
    ch->wolacz				= str_dup( name );
    ch->long_descr_orig			= str_dup( "" );
    ch->false_name			= str_dup( "" );
    ch->pcdata->fLog                    = NULL;
    ch->pcdata->login			= str_dup( d->login ? d->login : "(brak danych)" );
    ch->pcdata->email                   = str_dup( "" );
    ch->pcdata->prompt                  = str_dup( daPrompt );
    ch->pcdata->fprompt			= str_dup( "" );
    ch->pcdata->stopien			= str_dup( "" );
    ch->last_note                       = 0;
    ch->act				= 0; /* teraz wszystko jest w opcjach */
    ch->pcdata->opcje			= OPC_BLANK
					| OPC_AUTOSPLIT
					| OPC_AUTOGOLD
					/* | OPC_SHOWTICK - automat od 3 poz. */
					| OPC_AUTOLOOT
					| OPC_AUTOEXIT
					| OPC_PROMPT
					/* | OPC_HINT */
					| OPC_NO_SEPARATOR
					| OPC_SHOWLEVEL;
    ch->deaf				= CHANNEL_FLAME;
    if ( ch->desc->term_type && !str_cmp( ch->desc->term_type, "vt102" ) )
	SET_BIT( ch->pcdata->opcje, OPC_CUTCP );
    ch->polskie				= d->polskie;
    ch->kolor				= 2;
    ch->podswietl			= 0;
    ch->info                            = 0;
    ch->pcdata->clan                    = NULL;
    ch->pcdata->clan_member             = NULL;
    ch->pcdata->pwd                     = str_dup( "" );
    ch->pcdata->bamfin                  = str_dup( "" );
    ch->pcdata->bamfout                 = str_dup( "" );
    ch->pcdata->immskll                 = str_dup( "" );
    ch->pcdata->title                   = str_dup( "" );
    ch->pcdata->ignore			= str_dup( "" );
    ch->pcdata->pojedynek		= str_dup( "" );
    ch->pcdata->nienawisc		= str_dup( "" );
    ch->pcdata->love.od			= str_dup( "" );
    ch->pcdata->silence.od		= str_dup( "" );
    ch->pcdata->noemote.od		= str_dup( "" );
    ch->pcdata->nocolour.od		= str_dup( "" );
    ch->pcdata->notell.od		= str_dup( "" );
    ch->pcdata->log.od			= str_dup( "" );
    ch->pcdata->freeze.od		= str_dup( "" );
    ch->pcdata->deny.od			= str_dup( "" );
    ch->pcdata->thief_from		= str_dup( "" );
    ch->pcdata->killer_from		= str_dup( "" );
    ch->pcdata->zabojca_from		= str_dup( "" );
    ch->pcdata->wampiryzm.pochodzenie	= str_dup( "" );
    ch->pcdata->wampiryzm.strefa	= str_dup( "" );
    ch->pcdata->wampiryzm.rasa		= -1;
    ch->pcdata->wampiryzm.ile		= 0;
    ch->pcdata->wampiryzm.moc		= -1;
    ch->pcdata->szpic_wybr		= str_dup( "" );
    ch->pcdata->szpic_wykl		= str_dup( "" );
    ch->pcdata->last_login		= str_dup( "" );
    ch->pcdata->perm_str                = 13;
    ch->pcdata->perm_int                = 13;
    ch->pcdata->perm_wis                = 13;
    ch->pcdata->perm_dex                = 13;
    ch->pcdata->perm_con                = 13;
    ch->pcdata->condition[ COND_THIRST ] = 48;
    ch->pcdata->condition[ COND_FULL ]  = 48;
    ch->pcdata->killed                  = 0;
    ch->pcdata->dead                    = 0;
    ch->pcdata->pkilled			= 0;
    ch->pcdata->pdead			= 0;
    ch->pcdata->pagelen                 = 20;
    for ( i = 0; i < 5; i++ )
	ch->pcdata->multi[ i ]		= 0;
    for ( i = 0; i < MAX_OSTATNIE; i++ )
	ch->pcdata->ostatnie[ i ]	= NULL;

    ch->pcdata->switched		= FALSE;
    ch->pcdata->temp_flags		= 0;
    ch->pcdata->hint			= 0;
    ch->pcdata->wizinvis		= 0;
    ch->pcdata->niesmpoz                = 103;

    /* Lam 5.4.2000: 40 konfigurowalnych kolorow na pewno wystarczy */
    strcpy( ch->pcdata->kolor, "----------------------------------------" );

    ch->position = POS_STANDING;

    return ch;
}


void tnij_monety( OBJ_DATA *lista )
{
    OBJ_DATA *obj, *obj_next;

    for ( obj = lista; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted )
	    continue;

	if ( obj->contains )
	    tnij_monety( obj->contains );

	if ( obj->item_type == ITEM_MONEY )
	    extract_obj( obj );
    }

    return;
}


/*
 * Wyciecie przedmiotow, ktore w krainie zostaly zastapione takimi, ktorych nie
 * da sie podniesc z ziemi.
 *
 * Jedynie przedmioty z ustawionym "pustaw" ubraniem moga po zmianie w krainie
 * nie zniknac z inwentarza, ale raz, ze takich praktycznie nie ma, a dwa, ze
 * zapewne maja wtedy zmienione takze inne wlasciwosci.
 */
void wytnij_naziemne( OBJ_DATA *lista )
{
    OBJ_DATA *obj, *obj_next;

    for ( obj = lista; obj; obj = obj_next )
    {
	obj_next = obj->next_content;

	if ( obj->deleted )
	    continue;

	if ( obj->contains )
	    wytnij_naziemne( obj->contains );

	if ( !CAN_WEAR( obj, ITEM_TAKE ) )
	    extract_obj( obj );
    }

    return;
}


/*
 * Odbiera BWWU z pliku w wersji < 8 nadmiarowe umiejetnosci.
 *
 * To odbiera umiejetnosci, ktore postac bedzie mogla przeczytac z ksiag. Jest
 * to pewne niedociagniecie, ale dlugo im nie zajmie wycwiczenie od nowa. Nieco
 * sie przed tym zabezpieczam, pozostawiajac umiejetnosci wycwiczone na
 * poziomach < 101. Skoro dali rade wycwiczyc wczesniej, to pewnie zasluguja.
 * Tym sposobem zostawiam umiejki dostepne danej profesji, ale odbieram te
 * z ksiag z innych gildii. Jakos to zniosa.
 */
void zabierz_um_bwwu( CHAR_DATA *ch )
{
    int poziomy[ MAX_CLASS ];
    int i, sn;

    wypelnij_poziomy( ch, poziomy );

    for ( sn = 0; sn < MAX_SKILL; sn++ )
	if ( ch->pcdata->learned[ sn ]
	  && ch->pcdata->learned_level[ sn ] >= L_APP )
	{
	    for ( i = 0; i < MAX_CLASS; i++ )
		if ( poziomy[ i ] >= 100
		  && skill_table[ sn ].skill_level[ i ] <= ch->level )
		    break;

	    if ( i == MAX_CLASS )
	    {
/*		ch_printf( ch, "Odbieram umiej`etno`s`c \"%s\"\n\r", skill_table[ sn ].pl_name ); */
		ch->pcdata->learned[ sn ] = 0;
		ch->practice++;
	    }
	}

    return;
}


/*
 * Load a char and inventory into a new ch structure.
 *
 *** WAZNA ZMIANA: load_char_obj nie wywoluje nowa_postac( ), trzeba recznie!
 */
int load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
	   FILE       *fp;
	   CHAR_DATA  *ch = d->character;
	   char  strsave [ MSL ]; /* bylo MIL, ale to powodowalo pady przy
			wpisaniu imienia na 160 znakow, gdyz strsave to
			../player/a/ i dopiero ta nazwa. Jesli nazwa miala
			MIL, nie starczalo miejsca na katalog. 6.11.1999 */
	   int         found;
	   bool        zepsuty_przedmiot = FALSE;
	   int         gdzie;
#if !defined( macintosh ) && !defined( MSDOS )
	   struct stat fst;
#endif
	   char        sorry_player [] =
	     "**********************************************************\n\r"
	     "** Od czasu twojej ostatniej gry jedno ze znaczacych    **\n\r"
	     "** pol pliku twej postaci uleglo uszkodzeniu. Prosze    **\n\r"
	     "** skontaktowac sie z kompetentnym bogiem, aby pomogl   **\n\r"
	     "** ci odzyskac twa postac.                              **\n\r"
	     "**********************************************************\n\r";
	   char        sorry_object [] =
	     "**********************************************************\n\r"
	     "** Jeden z posiadanych przez ciebie przedmiotow zostal  **\n\r"
	     "** uszkodzony w pliku postaci. Ten przedmiot zniknie    **\n\r"
	     "** z twojego inwentarza. Jesli przedmiot mial jakas     **\n\r"
	     "** wartosc, mozesz w tej sprawie skontaktowac sie       **\n\r"
	     "** z bogami, ktorzy byc moze beda w stanie pomoc.       **\n\r"
	     "**********************************************************\n\r";


    for ( gdzie = 0; gdzie < MAX_WEAR; gdzie++ )
	save_eq[ gdzie ] = NULL; /* to niestety konieczne */

    found = 0;
    zamknij_reserve( fpReserve );

    /* parsed player file directories by Yaz of 4th Realm */
    /* decompress if .gz file exists - Thx Alander */
#if !defined( macintosh ) && !defined( MSDOS )
    sprintf( strsave, PLAYER_DIR "%s" DIR_SEPARATOR "%s%s",
	initial( ch->name ), capitalize( name ), ".gz" );
    if ( ( fp = fopen( strsave, "r" ) ) )
    {
	char buf[ MAX_STRING_LENGTH ];
	int zwrot;

	fclose( fp );
	sprintf( buf, "gzip -dfq %s", strsave );
	zwrot = system( buf );
	if ( zwrot != 0 )
	    bug( "system( \"gzip -dfq\" ) zawiodl: %d", zwrot );
    }
    else if ( errno != ENOENT )
    {
	lac_perror( "load_char_obj: fopen" );
	fpReserve = fopen( NULL_FILE, "r" );
	return -1;
    }
    sprintf( strsave, PLAYER_DIR "%s" DIR_SEPARATOR "%s",
	initial( ch->name ), capitalize( name ) );
    if ( stat( strsave, &fst ) != -1
      && fst.st_size == 0 )
    {
	sprintf( strsave, BACKUP_DIR "%s" DIR_SEPARATOR "%s",
	    initial( ch->name ), capitalize( name ) );
	if ( d->descriptor )
	    write_to_buffer( d, "*** Plik twojej postaci ulegl uszkodzeniu! Probuje uzyc kopii zapasowej...\n\r", 0 );
	bug( "Uszkodzona postac! (0 pliku)", 0 );
	SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_BACKUP );
    }
#else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
#endif
    if ( ( fp = fopen( strsave, "r" ) ) )
    {
	char buf[ MAX_STRING_LENGTH ];
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[ iNest ] = NULL;

	found = 1;
	for ( ; ; )
	{
	    char *word;
	    char  letter;
	    int   status;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp, &status, TRUE );

	    if ( !str_cmp( word, "PLAYER" ) )
	    {
		if ( fread_char( ch, fp ) )
		{
		    sprintf( buf,
			    "Load_char_obj: %s section PLAYER corrupt.",
			    name );
		    bug( buf, 0 );
		    if ( d->descriptor )
			write_to_buffer( d, sorry_player, 0 );

		    /*
		     * In case you are curious,
		     * it is ok to leave ch alone for close_socket
		     * to free.
		     * We want to now kick the bad character out as
		     * what we are missing are MANDATORY fields.  -Kahn
		     */
		    SET_BIT( ch->act, PLR_DENY );
		    fclose( fp );
		    fpReserve = fopen( NULL_FILE, "r" );
		    return 1;
		}
	    }
	    else if ( !str_cmp( word, "OBJECT" ) )
	    {
		if ( fread_obj( ch, fp, 0 ) == FREAD_OBJ_BLAD )
		{
		    sprintf( buf,
			    "Load_char_obj: %s section OBJECT corrupt.",
			    name );
		    log_string( buf );
		    zepsuty_przedmiot = TRUE;
/*		    write_to_buffer( d, sorry_object, 0 );
		    return TRUE; */
/* oryginal:        return FALSE; :) fajny blad Envy - to by byl nowy gracz */
		}
	    }
	    else if ( !str_cmp( word, "PRZECHOWALNIA" ) )
	    {
		if ( fread_obj( ch, fp, 1 ) == FREAD_OBJ_BLAD ) /* 1 == przechowalnia */
		{
		    sprintf( buf,
			    "Load_char_obj: %s section PRZECHOWALNIA corrupt.",
			    name );
		    log_string( buf );
		    zepsuty_przedmiot = TRUE;
/*		    write_to_buffer( d, sorry_object, 0 );
		    return TRUE; */
		}
	    }
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	} /* for */

	if ( save_ver >= 1 ) /* wersja pliku 1 obsluguje zdejmowanie przedmiotow */
	    ubierz( ch );

	if ( save_ver < 5 ) /* denominacja */
	    tnij_monety( ch->carrying );

	wytnij_naziemne( ch->carrying );
	wytnij_naziemne( ch->przechowalnia );

	/*
	 * Lam 22.3.98: zepsute przedmioty nie robia returna, tylko czytaja plik
	 * dalej, dzieki czemu nie traci sie przedmiotow zapisanych w pliku po
	 * usunietych ze swiata (np. licencje albo rzeczy z usunietych krain).
	 * Informacja wyswietla sie na koncu.
	 */
	if ( zepsuty_przedmiot && d->descriptor )
	    write_to_buffer( d, sorry_object, 0 );
	fclose( fp );
    }
    else if ( errno != ENOENT )
    {
	lac_perror( "load_char_obj: fopen" );
	fpReserve = fopen( NULL_FILE, "r" );
	return -1;
    }

    fpReserve = fopen( NULL_FILE, "r" );

    return found;
}


/*
 * Read in a char.
 */
int fread_char( CHAR_DATA *ch, FILE *fp )
{
    char        *word;
    char        buf [ MAX_STRING_LENGTH ];
    AFFECT_DATA *paf;
    ALIAS_DATA  *alias;
    ALIAS_DATA  *alias_tmp;
    REFUSE_DATA *refuse;
    REFUSE_DATA *refuse_tmp;
    RESTART_DATA *restart;
    int         sn;
    int         i;
    int         j;
    int         error_count = 0;
    int		zabrane_dodatkowe = 0;
    int         status;
    int         status1;
    char        *p;
    int         tmpi;
    int         num_keys;
    int         last_key = 0;
#if defined( DODATKOWE_W_NIANI )
    int         multicount = 0;
#endif

    static	char	def_prompt [] = "<%A%h`z %mm %vr %xd`s><%n`z %cm> ";
    static	char	def_sdesc  [] = "(opis uszkodzony!)";
    static	char	def_dopel  [] = "(dope`lniacz uszkodzony!)";
    static	char	def_celow  [] = "(celownik uszkodzony!)";
    static	char	def_biern  [] = "(biernik uszkodzony!)";
    static	char	def_narze  [] = "(narz`ednik uszkodzony!)";
    static	char	def_miejs  [] = "(miejscownik uszkodzony!)";
/*    static	char	def_wolacz [] = "(wo`lacz uszkodzony!)"; */
    static	char	def_ldesc  [] = "Uszkodzony d`lugi opis!";
    static	char	def_desc   [] = "Uszkodzony w pliku postaci opis!";
    static	char	def_title  [] = " (uszkodzony w pliku postaci tytu`l!)";
    static	char	def_login  [] = "(nieznane)";
    static	char	def_email  [] = "";

    struct key_data key_tab [] = {
      { "FileVersion",FALSE, 0, 0,		{ &save_ver,          NULL } }, /* uwaga - save_ver jest globalna */
      { "ShtDsc", TRUE,      0, &def_sdesc,     { &ch->short_descr,   NULL } },
      { "Dopelniacz", TRUE,  0, &def_dopel,     { &ch->dopelniacz,    NULL } },
      { "Celownik",   TRUE,  0, &def_celow,     { &ch->celownik,      NULL } },
      { "Biernik",    TRUE,  0, &def_biern,     { &ch->biernik,       NULL } },
      { "Narzednik",  TRUE,  0, &def_narze,     { &ch->narzednik,     NULL } },
      { "Miejscownik",TRUE,  0, &def_miejs,     { &ch->miejscownik,   NULL } },
      { "Wolacz",     TRUE,  DEFLT, 0,		{ &ch->wolacz,        NULL } },
      { "LngDsc",     TRUE,  0, &def_ldesc,     { &ch->long_descr,    NULL } },
      { "LngDscOrg",  TRUE,  DEFLT, 0,          { &ch->long_descr_orig, NULL } },
      { "FalseName",  TRUE,  DEFLT, 0,          { &ch->false_name,    NULL } },
      { "Dscr",   TRUE,  0, &def_desc,       { &ch->description,   NULL } },
      { "Prmpt",  TRUE,  0, &def_prompt,     { &ch->pcdata->prompt,NULL } },
      { "Fprmpt", TRUE,  DEFLT,	0,		{ &ch->pcdata->fprompt,NULL } },
      { "Stpn",   TRUE,  DEFLT,	0,		{ &ch->pcdata->stopien,NULL } },
      { "Login",  TRUE,  0, &def_login,      { &ch->pcdata->last_login, NULL } },
      { "Email",  TRUE,  0, &def_email,      { &ch->pcdata->email, NULL } },
      { "LovedBy",    TRUE, DEFLT, 0,		{ &ch->pcdata->love.od,    NULL } },
      { "SilencedBy", TRUE, DEFLT, 0,		{ &ch->pcdata->silence.od, NULL } },
      { "NoemotedBy", TRUE, DEFLT, 0,		{ &ch->pcdata->noemote.od, NULL } },
      { "NocolouredBy",TRUE,DEFLT, 0,		{ &ch->pcdata->nocolour.od, NULL } },
      { "NotelledBy", TRUE, DEFLT, 0,		{ &ch->pcdata->notell.od, NULL } },
      { "LoggedBy",   TRUE, DEFLT, 0,		{ &ch->pcdata->log.od,   NULL } },
      { "FreezedBy",  TRUE, DEFLT, 0,		{ &ch->pcdata->freeze.od,  NULL } },
      { "ThiefFrom",  TRUE, DEFLT, 0,		{ &ch->pcdata->thief_from,  NULL } },
      { "KillerFrom", TRUE, DEFLT, 0,		{ &ch->pcdata->killer_from, NULL } },
      { "ZabojcaFrom",TRUE, DEFLT, 0,		{ &ch->pcdata->zabojca_from,NULL } },
      { "VampireFrom",TRUE, DEFLT, 0,           { &ch->pcdata->wampiryzm.pochodzenie, NULL } },
      { "OrigRace",  FALSE, -1, 0,              { &ch->pcdata->wampiryzm.rasa,  NULL } },
      { "OrigZone",   TRUE, DEFLT, 0,           { &ch->pcdata->wampiryzm.strefa,   NULL } },
      { "VIle",      FALSE, DEFLT, 0,           { &ch->pcdata->wampiryzm.ile, NULL } },
      { "VMoc",      FALSE, -1, 0,              { &ch->pcdata->wampiryzm.moc, NULL } },
      { "Ignore",     TRUE, DEFLT, 0,		{ &ch->pcdata->ignore,      NULL } },
      { "Pojedynek",  TRUE, DEFLT, 0,		{ &ch->pcdata->pojedynek,   NULL } },
      { "Nienawisc",  TRUE, DEFLT, 0,		{ &ch->pcdata->nienawisc,   NULL } },
      { "Sx",     FALSE, SEX_MALE, 0,           { &ch->sex,           NULL } },
      { "Cla",    FALSE, MAND, 0,               { &ch->klasa,         NULL } },
      { "Lvl",    FALSE, MAND, 0,               { &ch->level,         NULL } },
      { "Trst",   FALSE, 0, 0,                  { &ch->trust,         NULL } },
      { "Playd",  FALSE, 0, 0,                  { &ch->played,        NULL } },
      { "Note",   FALSE, 0, 0,                  { &ch->last_note,     NULL } },
      { "Hint",   FALSE, 0, 0,                  { &ch->pcdata->hint,  NULL } },
      { "Wizinvis",   FALSE, 0, 0,              { &ch->pcdata->wizinvis,  NULL } },
      { "Niesmpoz",   FALSE, 103, 0,            { &ch->pcdata->niesmpoz,  NULL } },
      { "HpMnMv", FALSE, MAND, 0,               { &ch->hit,
						  &ch->max_hit,
						  &ch->mana,
						  &ch->max_mana,
						  &ch->move,
						  &ch->max_move,      NULL } },
      { "Kill",   FALSE, DEFLT, 0,              { &ch->pcdata->killed,
						  &ch->pcdata->dead,  NULL } },
      { "PKll",  FALSE, DEFLT, 0,               { &ch->pcdata->pkilled,
						  &ch->pcdata->pdead, NULL } },
      { "Scde",  FALSE, DEFLT, 0,               { &ch->pcdata->wykrwawienia,
						  &ch->pcdata->samobojstwa,
								      NULL } },
      { "Gold",   FALSE, 0,     0,              { &ch->gold,          NULL } },
      { "Bank",   FALSE, 0,     0,              { &ch->pcdata->bank,  NULL } },
      { "Exp",    FALSE, MAND,  0,              { &ch->exp,           NULL } },
      { "QuestP", FALSE, 0,     0,              { &ch->qp,            NULL } },
      { "Act",    FALSE, DEFLT, 0,              { &ch->act,           NULL } },
      { "Info",   FALSE, 0,     0,              { &ch->info,          NULL } },
      { "Pol",    FALSE, 0,     0,              { &ch->polskie,       NULL } },
      { "Kolor",  FALSE, 2,     0,              { &ch->kolor,         NULL } },
      { "Podswietl",FALSE, 0,   0,              { &ch->podswietl,     NULL } },
      { "AffdBy", FALSE, DEFLT,	0,		{ &ch->affected_by[ 0 ], NULL } },
      { "AffdBy2",FALSE, DEFLT,	0,		{ &ch->affected_by[ 1 ], NULL } },
      { "Pos",    FALSE, POS_STANDING, 0,       { &ch->pcdata->ic_position,
								      NULL } },
      { "Prac",   FALSE, MAND,  0,              { &ch->practice,      NULL } },
      { "SavThr", FALSE, MAND,  0,              { &ch->saving_throw,  NULL } },
      { "Align",  FALSE, 0,     0,              { &ch->alignment,     NULL } },
      { "Hit",    FALSE, MAND,  0,              { &ch->hitroll,       NULL } },
      { "Dam",    FALSE, MAND,  0,              { &ch->damroll,       NULL } },
      { "Armr",   FALSE, 0,     0,              { &ch->armor,         NULL } },
      { "MArmr",  FALSE, MAND,  0,              { &ch->marmor,        NULL } },
      { "Wimp",   FALSE, 10,    0,              { &ch->wimpy,         NULL } },
      { "Deaf",   FALSE, 0,     0,              { &ch->deaf,          NULL } },
      { "Angel",  FALSE, 0,     0,              { &ch->pcdata->hp_angel, NULL } },
      { "Opcje",  FALSE, DEFLT, 0,		{ &ch->pcdata->opcje, NULL } },
      { "Paswd",  TRUE,  MAND,  0,              { &ch->pcdata->pwd,   NULL } },
      { "Bmfin",  TRUE,  DEFLT, 0,              { &ch->pcdata->bamfin,
								      NULL } },
      { "Bmfout", TRUE,  DEFLT, 0,              { &ch->pcdata->bamfout,
								      NULL } },
      { "Immskll",TRUE,  DEFLT, 0,              { &ch->pcdata->immskll,
								      NULL } },
      { "Wiznet", FALSE, 0,     0,              { &ch->pcdata->wiznet,
								      NULL } },
      { "SzpWbr", TRUE,  DEFLT, 0,              { &ch->pcdata->szpic_wybr,
								      NULL } },
      { "SzpWkl", TRUE,  DEFLT, 0,              { &ch->pcdata->szpic_wykl,
								      NULL } },
      { "Ttle",   TRUE,  0,    &def_title,      { &ch->pcdata->title, NULL } },
      { "AtrPrm", FALSE, MAND,  0,              { &ch->pcdata->perm_str,
						  &ch->pcdata->perm_int,
						  &ch->pcdata->perm_wis,
						  &ch->pcdata->perm_dex,
						  &ch->pcdata->perm_con,
								      NULL } },
      { "AtrMd",  FALSE, MAND,  0,              { &ch->mod_str,
						  &ch->mod_int,
						  &ch->mod_wis,
						  &ch->mod_dex,
						  &ch->mod_con,
								      NULL } },
      { "Cond",   FALSE, DEFLT, 0,              { &ch->pcdata->condition [ 0 ],
						  &ch->pcdata->condition [ 1 ],
						  &ch->pcdata->condition [ 2 ],
								      NULL } },
      { "Pglen",  FALSE, 20,    0,              { &ch->pcdata->pagelen,
								      NULL } },
      { "\0",     FALSE, 0,     0,              { NULL                     } }
    };

    for ( num_keys = 0; *key_tab[ num_keys ].key; )
	num_keys++;

    for ( ; !feof (fp) ; )
    {

	word = fread_word( fp, &status, TRUE );

	if ( !word )
	{
	    bug( "fread_char:  Error reading key.  EOF?", 0 );
	    fread_to_eol( fp );
	    break;
	}

		/* This little diddy searches for the keyword
		   from the last keyword found */

	for ( i = last_key;
	      i < last_key + num_keys &&
		str_cmp( key_tab[ i % num_keys ].key, word ); )
	    i++;

	i = i % num_keys;

	if ( !str_cmp( key_tab[ i ].key, word ) )
	    last_key = i;
	else
	    i = num_keys;

	if ( *key_tab[ i ].key )         /* Key entry found in key_tab */
	{
	    if ( key_tab[ i ].string == SPECIFIED )
	    {
		sprintf( bug_buf, "Key '%s' already specified.", word );
		cbug( bug_buf, 0 );
	    }
	    else if ( key_tab[ i ].string ) /* Entry is a string */
	    {
		if ( ( p = fread_string( fp, &status ) ) && !status )
		{
		    free_string ( *(char **)key_tab[ i ].ptrs[ 0 ] );
		    *(char **)key_tab[ i ].ptrs[ 0 ] = p;
		}
	    }
	    else /* Entry is an integer */
	    {
		for ( j = 0; key_tab[ i ].ptrs[ j ]; j++ )
		{
		    tmpi = fread_number ( fp, &status );
		    if ( !status )
			*(int *)key_tab[ i ].ptrs[ j ] = tmpi;
		}
	    }

	    if ( status )
	    {
		fread_to_eol( fp );
		continue;
	    }
	    else
		key_tab[ i ].string = SPECIFIED;
	}

	else if ( *word == '*' || !str_cmp( word, "Nm" ) )
	    fread_to_eol( fp );

	else if ( !str_cmp( word, "End" ) )
	    break;

	else if ( !str_cmp( word, "Separator" ) )
	{
	    char *pomocniczy;
	    pomocniczy = fread_string( fp, &status );
	    ch->separator = *pomocniczy;
	    free_string( pomocniczy ); /* Lam 5.4.2000: wyciek pamieci */
	}

	else if ( !str_cmp( word, "Barwy" ) )
	{
	    char *pomoc = fread_string( fp, &status );
	    strcpy( ch->pcdata->kolor, pomoc );
	    free_string( pomoc );
	}

	else if ( !str_cmp( word, "Clanstat" ) )
	{
	    /* Ulryk: Clanstat jest wczytywane z danych klanu */
	    /* Lam: przywracam zgodnosc z formatem */
	    fread_number( fp, &status );
	}

	else if ( !str_cmp( word, "Clan" ) )
	{
	    char             *pomoc;
	    CLAN_MEMBER_DATA *klanowicz;

	    pomoc = fread_string( fp, &status );
	    if ( *pomoc )
	    {
		if ( strlen( pomoc ) < 3 )
		    bug( "fread_char: Nazwa klanu krotsza niz 3 litery!", 0 );
		else
		{
		    if ( strlen( pomoc ) > MAX_CLAN_NAME )
			bug( "fread_char: Nazwa klanu dluzsza niz maksymalna!", strlen( pomoc ) );
		    ch->pcdata->clan = clan_lookup( pomoc );
		    if ( ch->pcdata->clan )
		    {
			for ( klanowicz = ch->pcdata->clan->member_list;
			      klanowicz;
			      klanowicz = klanowicz->next )
			    if ( !str_cmp( ch->name, klanowicz->name ) )
				break;

			if ( klanowicz )
			    ch->pcdata->clan_member = klanowicz;
			else
			{
			    sprintf( bug_buf, "Brak klanowicza na liscie klanowej, klan '%s'.", pomoc );
			    bug( bug_buf, 0 );
			    ASTC( "Klan, do kt`orego nale`za`l$o`s, zgubi`l twoje dokumenty.", ch );
			    ch->pcdata->clan = NULL;
			}
		    }
		    else
		    {
			/* Lam: to ma byc niby blad? W tej chwili, po wszystkich
			   czystkach i 4 latach bez klanow, nadal jest 17
			   postaci ze starymi klanami.
			bug( "fread_char: Klan, do ktorego nalezal gracz, nie istnieje.", 0 ); */
			ASTC( "Klan, do kt`orego nale`za`l$o`s, nie istnieje.", ch );
		    }
		}
	    }
	    free_string( pomoc ); /* Lam 5.4.2000: wyciek pamieci */
	}

	else if ( !str_cmp( word, "PKill" ) ) /* poprzednia wersja zliczania zabojstw */
	{
	    fread_number( fp, &status );
	    fread_number( fp, &status );
	}

	else if ( !str_cmp( word, "Room" ) )
	{
	    /* Lam 20.8.2003: teraz postac po wejsciu do gry pojawia sie PP */
	    ch->in_room = get_room_index( ROOM_VNUM_POZA_SWIATEM );
	    ch->pcdata->ic_in_room = get_room_index( fread_number( fp, &status ) );
	    /* Lam 20.8.2003: to glupie, zeby ich wywalalo do Limbo */
	    if ( !ch->pcdata->ic_in_room )
	    {
		if ( ch->pcdata->strefa )
		    ch->pcdata->ic_in_room = get_room_index( ch->pcdata->strefa->swiatynia );
		else /* ewentualnie dotrze jakos z Midgaardu */
		    ch->pcdata->ic_in_room = get_room_index( ROOM_VNUM_TEMPLE );
	    }
	}

	else if ( !str_cmp( word, "Race" ) )
	{
	    char *pomras = fread_string( fp, &status );

	    i  = race_lookup( pomras );

	    if ( status )
		bug( "Fread_char: Unknown Race.", 0 );
	    else
		ch->race = i;

	    free_string( pomras ); /* Lam 5.4.2000: wyciek pamieci */
	}

	else if ( !str_cmp( word, "Skll" ) )
	{
	    char *bufeczek;

	    i  = fread_number( fp, &status );
	    bufeczek = fread_word( fp, &status1, TRUE );
	    sn = skill_lookup_ang( bufeczek );

	    if ( status || status1 )
	    {
		bug( "Fread_char: Error reading skill. (Skll)", 0 );
		fread_to_eol( fp );
		continue;
	    }

	    if ( sn < 0 )
	    {
		sprintf( log_buf, "Fread_char: unknown skill '%s'.", bufeczek );
		bug( log_buf, 0 );
	    }
	    else if ( IS_SET( skill_table[ sn ].flags, SKILL_NPC )
		   && skill_table[ sn ].skill_level[ ch->klasa ] == L_DIR
		   && skill_table[ sn ].multi_level[ ch->klasa ] == 199
		   && ch->level < L_DIR )
	    {
		ch->pcdata->learned[ sn ] = 0;
		ch->pcdata->used[ sn ] = 0;
		ch->pcdata->learned_level[ sn ] = 0;
	    }
	    else if ( save_ver < 6
		   && ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
		   && ch->level < skill_table[ sn ].multi_level[ ch->klasa ] )
	    {
		ch->pcdata->learned[ sn ] = 0;
		ch->pcdata->used[ sn ] = 0;
		ch->pcdata->learned_level[ sn ] = 0;
		ch->practice += UMIN( i, 75 ) / 10; /* "multi" cwiczone przy int 15 */
		zabrane_dodatkowe++;
	    }
	    else
	    {
		ch->pcdata->learned[ sn ] = i;
		ch->pcdata->used[ sn ] = 0;
		if ( skill_table[ sn ].multi_level[ ch->klasa ] <
		      skill_table[ sn ].skill_level[ ch->klasa ] )
		{
		    /* ksiega czytana zanim ma do wyboru z dodatkowych */
		    if ( skill_table[ sn ].multi_level[ ch->klasa ] > ch->level )
			ch->pcdata->learned_level[ sn ] = UMAX( 3, ch->level - 3 );
		    else
			ch->pcdata->learned_level[ sn ] = skill_table[ sn ].multi_level[ ch->klasa ];
		}
		else
		{
		    if ( skill_table[ sn ].skill_level[ ch->klasa ] >= L_APP )
		    {
			if ( ch->level >= L_APP )
			    ch->pcdata->learned_level[ sn ] = L_APP;
			else /* zapewne nauczyl sie z ksiegi (ale kiedy?) */
			    ch->pcdata->learned_level[ sn ] = UMAX( 3, ch->level - 3 );
		    }
		    else
		    {
			/* ksiega przeczytana zanim moze sie uczyc */
			if ( ch->level < skill_table[ sn ].skill_level[ ch->klasa ] )
			    ch->pcdata->learned_level[ sn ] = UMAX( 3, ch->level - 3 );
			else
			    ch->pcdata->learned_level[ sn ] = skill_table[ sn ].skill_level[ ch->klasa ];
		    }
		}
	    }
	}

	else if ( !str_cmp( word, "Skill" ) )
	{
	    int pzl;
	    char *bufeczek;

	    i  = fread_number( fp, &status );
	    pzl= fread_number( fp, &status );
	    bufeczek = fread_word( fp, &status1, TRUE );
	    sn = skill_lookup_ang( bufeczek );

	    if ( status )
	    {
		bug( "Fread_char: Error reading skill. (status)", 0 );
		fread_to_eol( fp );
		continue;
	    }
	    if ( status1 )
	    {
		bug( "Fread_char: Error reading skill. (status1)", 0 );
		fread_to_eol( fp );
		continue;
	    }

	    if ( sn < 0 )
	    {
		sprintf( log_buf, "Fread_char: unknown skill '%s'.", bufeczek );
		bug( log_buf, 0 );
	    }
	    else if ( IS_SET( skill_table[ sn ].flags, SKILL_NPC )
		   && skill_table[ sn ].skill_level[ ch->klasa ] == L_DIR
		   && skill_table[ sn ].multi_level[ ch->klasa ] == 199
		   && ch->level < L_DIR )
	    {
		ch->pcdata->learned[ sn ] = 0;
		ch->pcdata->used[ sn ] = 0;
		ch->pcdata->learned_level[ sn ] = 0;
	    }
	    else if ( save_ver < 6
		   && ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
		   && ch->level < skill_table[ sn ].multi_level[ ch->klasa ] )
	    {
		ch->pcdata->learned[ sn ] = 0;
		ch->pcdata->used[ sn ] = 0;
		ch->pcdata->learned_level[ sn ] = 0;
		ch->practice += UMIN( i, 75 ) / 10; /* "multi" cwiczone przy int 15 */
		zabrane_dodatkowe++;
	    }
	    else
	    {
		ch->pcdata->learned[ sn ] = i;
		ch->pcdata->used[ sn ] = pzl;
		if ( skill_table[ sn ].multi_level[ ch->klasa ] <
		      skill_table[ sn ].skill_level[ ch->klasa ] )
		{
		    /* ksiega czytana zanim ma do wyboru z dodatkowych */
		    if ( skill_table[ sn ].multi_level[ ch->klasa ] > ch->level )
			ch->pcdata->learned_level[ sn ] = UMAX( 3, ch->level - 3 );
		    else
			ch->pcdata->learned_level[ sn ] = skill_table[ sn ].multi_level[ ch->klasa ];
		}
		else
		{
		    if ( skill_table[ sn ].skill_level[ ch->klasa ] >= L_APP )
		    {
			if ( ch->level >= L_APP )
			    ch->pcdata->learned_level[ sn ] = L_APP;
			else /* zapewne nauczyl sie z ksiegi (ale kiedy?) */
			    ch->pcdata->learned_level[ sn ] = UMAX( 3, ch->level - 3 );
		    }
		    else
		    {
			/* ksiega przeczytana zanim moze sie uczyc */
			if ( ch->level < skill_table[ sn ].skill_level[ ch->klasa ] )
			    ch->pcdata->learned_level[ sn ] = UMAX( 3, ch->level - 3 );
			else
			    ch->pcdata->learned_level[ sn ] = skill_table[ sn ].skill_level[ ch->klasa ];
		    }
		}
	    }
	}

	else if ( !str_cmp( word, "Skll2" ) )
	{
	    int pzl;
	    int pzm;
	    char *bufeczek;

	    pzm = fread_number( fp, &status );
	    i = fread_number( fp, &status );
	    pzl = fread_number( fp, &status );
	    bufeczek = fread_word( fp, &status1, TRUE );
	    sn = skill_lookup_ang( bufeczek );

	    if ( status )
	    {
		bug( "Fread_char: Error reading skill. (status)", 0 );
		fread_to_eol( fp );
		continue;
	    }
	    if ( status1 )
	    {
		bug( "Fread_char: Error reading skill. (status1)", 0 );
		fread_to_eol( fp );
		continue;
	    }

	    if ( sn < 0 )
	    {
		sprintf( log_buf, "Fread_char: unknown skill '%s'.", bufeczek );
		bug( log_buf, 0 );
	    }
	    else if ( ( IS_SET( skill_table[ sn ].flags, SKILL_NPC )
		     && skill_table[ sn ].skill_level[ ch->klasa ] == L_DIR
		     && skill_table[ sn ].multi_level[ ch->klasa ] == 199
		     && ch->level < L_DIR )
		   || ( sn == gsn_aura_pokoju /* pierwszy tak wyciety czar */
		     && ch->level < L_APP ) ) /* docelowo jakies SKILL_COS */
	    {
		ch->pcdata->learned[ sn ] = 0;
		ch->pcdata->used[ sn ] = 0;
		ch->pcdata->learned_level[ sn ] = 0;
		if ( sn == gsn_aura_pokoju ) /* bede litosciwy ;) */
		{
		    sn = gsn_invis; /* taka zmiana nastapila tez w class/mni */
		    ch->pcdata->learned[ sn ] = i;
		    ch->pcdata->used[ sn ] = pzl;
		    ch->pcdata->learned_level[ sn ] = pzm;
		}
	    }
	    /* else if ( save_ver < 6 - w Skll2 niemozliwe */
	    else
	    {
		ch->pcdata->learned[ sn ] = i;
		ch->pcdata->used[ sn ] = pzl;
		ch->pcdata->learned_level[ sn ] = pzm;
	    }
	}

	else if ( !str_cmp( word, "MPQuest" ) )
	{
	    int pzl;
	    MPQUEST_DATA *mpquest;

	    i = fread_number( fp, &status );
	    pzl = fread_number( fp, &status );

	    if ( status )
	    {
		bug( "Fread_char: Error reading mpquest. (status)", 0 );
		fread_to_eol( fp );
		continue;
	    }

	    mpquest = find_mpquest( ch, i );
	    if ( mpquest )
		bug( "Fread_char: duplicate mpquest %d", i );
	    else
	    {
		mpquest = new_mpquest( );
		mpquest->vnum = i;
		mpquest->stage  = pzl;
		for ( i = 0; i < 5; i++ )
		      mpquest->data[ i ] = 0;
		mpquest->next = ch->mpquest;
		ch->mpquest = mpquest;
	    }
	}

	else if ( !str_cmp( word, "MPQD" ) )
	{
	    int pzl;
	    int vnumek;
	    int mpqdata[ 5 ];
	    MPQUEST_DATA *mpquest;

	    vnumek = fread_number( fp, &status );
	    pzl = fread_number( fp, &status );
	    for ( i = 0; i < 5; i++ )
		mpqdata[ i ] = fread_number( fp, &status );

	    if ( status )
	    {
		bug( "Fread_char: Error reading mpquest. (status)", 0 );
		fread_to_eol( fp );
		continue;
	    }

	    mpquest = find_mpquest( ch, vnumek );
	    if ( mpquest )
		bug( "Fread_char: duplicate mpquest %d", i );
	    else
	    {
		mpquest = new_mpquest( );
		mpquest->vnum = vnumek;
		mpquest->stage  = pzl;
		for ( i = 0; i < 5; i++ )
		    mpquest->data[ i ] = mpqdata[ i ];
		mpquest->next = ch->mpquest;
		ch->mpquest = mpquest;
	    }
	}

	else if ( !str_cmp( word, "MPQD2" ) )
	{
	    int pzl;
	    int vnumek;
	    int mpqdata[ 5 ];
	    MPQUEST_DATA *mpquest;
	    char *title;
	    char *descr;

	    vnumek = fread_number( fp, &status );
	    pzl = fread_number( fp, &status );
	    for ( i = 0; i < 5; i++ )
		mpqdata[ i ] = fread_number( fp, &status );
	    title = fread_string( fp, &status );
	    descr = fread_string( fp, &status );

	    if ( status )
	    {
		bug( "Fread_char: Error reading mpquest. (status)", 0 );
		fread_to_eol( fp );
		continue;
	    }

	    mpquest = find_mpquest( ch, vnumek );
	    if ( mpquest )
		bug( "Fread_char: duplicate mpquest %d", i );
	    else
	    {
		mpquest = new_mpquest( );
		mpquest->vnum = vnumek;
		mpquest->stage  = pzl;
		for ( i = 0; i < 5; i++ )
		    mpquest->data[ i ] = mpqdata[ i ];
		mpquest->title = title;
		mpquest->descr = descr;

		/* i one niech sie czytaja w kolejnosci */
		mpquest->next = NULL;
		if ( !ch->mpquest )
		    ch->mpquest = mpquest;
		else
		{
		    MPQUEST_DATA *pmpq;

		    pmpq = ch->mpquest;
		    while ( pmpq->next )
			pmpq = pmpq->next;
		    pmpq->next = mpquest;
		}
	    }
	}

	else if ( !str_cmp( word, "MltSkll" ) )
	{
#if defined( DODATKOWE_W_NIANI )
	    sn = skill_lookup_ang( fread_word( fp, &status, TRUE ) );
#else
	    fread_word( fp, &status, TRUE );
#endif
	    if ( status )
	    {
		bug( "Fread_char: Error reading skill. (MltSkll)", 0 );
		fread_to_eol( fp );
		continue;
	    }

#if defined( DODATKOWE_W_NIANI )
	    if ( sn < 0 )
		bug( "Fread_char: unknown skill.", 0 );
	    else
	    {
		ch->pcdata->multi[ multicount ] = sn;
		if ( multicount < 4 ) multicount++;
	    }
#endif
	}

	else if ( !str_cmp( word, "AffectV" ) )
	{
	    /* to zastepuje stare AffdBy i AffdBy2 obslugiwane nadal przez
	       tablice powyzej */
	    fread_vector( ch->affected_by, fp, &status );
	}

	else if ( !str_cmp( word, "Afft" ) )
	{
	    char *kupa = fread_string( fp, &status );

	    paf                 = new_affect( );

	    paf->type           = affect_lookup( kupa );
	    if ( paf->type < 0 )
		paf->type = 0;
	    paf->duration       = fread_number( fp, &status );
	    paf->modifier       = fread_number( fp, &status );
	    paf->location       = fread_number( fp, &status );
	    CLEAR_BITS( paf->bitvector );
	    paf->bitvector[ 0 ] = fread_number( fp, &status );
	    paf->level	  = 0;
	    paf->caster	  = NULL;
	    paf->deleted        = FALSE;
	    paf->next           = ch->affected;
	    ch->affected        = paf;
	    free_string( kupa ); /* Lam 5.4.2000: wyciek pamieci */
	}

	else if ( !str_cmp( word, "Afft2" ) )
	{
	    char *kupa = fread_string( fp, &status );

	    paf                 = new_affect( );

	    paf->type           = affect_lookup( kupa );
	    if ( paf->type < 0 )
		paf->type = 0;
	    paf->duration       = fread_number( fp, &status );
	    paf->modifier       = fread_number( fp, &status );
	    paf->location       = fread_number( fp, &status );
	    CLEAR_BITS( paf->bitvector );
	    paf->bitvector[ 0 ] = fread_number( fp, &status );
	    paf->bitvector[ 1 ] = fread_number( fp, &status );
	    paf->level	  = 0;
	    paf->caster	  = NULL;
	    paf->deleted        = FALSE;
	    paf->next           = ch->affected;
	    ch->affected        = paf;
	    free_string( kupa );
	}

	else if ( !str_cmp( word, "Afft3" ) )
	{
	    char *kupa = fread_string( fp, &status );

	    paf                 = new_affect( );

	    paf->type           = affect_lookup( kupa );
	    if ( paf->type < 0 )
		paf->type = 0;
	    paf->duration       = fread_number( fp, &status );
	    paf->modifier       = fread_number( fp, &status );
	    paf->location       = fread_number( fp, &status );
	    CLEAR_BITS( paf->bitvector );
	    paf->bitvector[ 0 ] = fread_number( fp, &status );
	    paf->bitvector[ 1 ] = fread_number( fp, &status );
	    paf->level	  = fread_number( fp, &status );
	    paf->caster	  = NULL;
	    paf->deleted        = FALSE;
	    /* Lam 27.10.2004: niech sie czytaja w kolejnosci */
	    paf->next = NULL;
	    if ( !ch->affected )
		ch->affected = paf;
	    else
	    {
		AFFECT_DATA *tpaf;
		tpaf = ch->affected;
		while ( tpaf->next )
		    tpaf = tpaf->next;
		tpaf->next = paf;
	    }
	    free_string( kupa );
	}

	else if ( !str_cmp( word, "Afft4" ) )
	{
	    char *kupa = fread_string( fp, &status );

	    paf                 = new_affect( );

	    paf->type           = affect_lookup( kupa );
	    if ( paf->type < 0 )
		paf->type = 0;
	    paf->duration       = fread_number( fp, &status );
	    paf->modifier       = fread_number( fp, &status );
	    paf->location       = fread_number( fp, &status );
	    paf->level	  = fread_number( fp, &status );
	    fread_vector( paf->bitvector, fp, &status );
	    paf->caster	  = NULL;
	    paf->deleted        = FALSE;
	    /* Lam 27.10.2004: niech sie czytaja w kolejnosci */
	    paf->next = NULL;
	    if ( !ch->affected )
		ch->affected = paf;
	    else
	    {
		AFFECT_DATA *tpaf;
		tpaf = ch->affected;
		while ( tpaf->next )
		    tpaf = tpaf->next;
		tpaf->next = paf;
	    }
	    free_string( kupa );
	}

	else if ( !str_cmp( word, "Alias" ) )
	{
	    alias               = new_alias( );

	    alias->name = str_dup( fread_word( fp, &status, FALSE ) );
	    alias->todo = fread_string( fp, &status );
	    if ( ch->alias )
	    {
		alias_tmp = ch->alias;
		while ( alias_tmp->next )
		    alias_tmp = alias_tmp->next;

		alias_tmp->next = alias;
		alias->next = NULL;

	    }
	    else
	    {
		alias->next         = ch->alias;
		ch->alias           = alias;
	    }
	}

	else if ( !str_cmp( word, "Refuse" ) )
	{
	    char *temp;

	    refuse               = new_refuse( );

	    temp = fread_word( fp, &status, TRUE );
	    sprintf( refuse->refused_cmd, "%s", temp );
	    temp = fread_word( fp, &status, TRUE );
	    sprintf( refuse->who, "%s", temp );
	    temp = fread_word( fp, &status, FALSE );
	    sprintf( refuse->date, "%s", temp );
	    temp = fread_string( fp, &status );
	    sprintf( refuse->why, "%s", temp );
	    free_string ( temp );

	    if ( ch->pcdata->refuse )
	    {
		refuse_tmp = ch->pcdata->refuse;
		while ( refuse_tmp->next )
		    refuse_tmp = refuse_tmp->next;

		refuse_tmp->next = refuse;
		refuse->next = NULL;
	    }
	    else
	    {
		refuse->next = ch->pcdata->refuse;
		ch->pcdata->refuse = refuse;
	    }
	}

	else if ( !str_cmp( word, "RHstr" ) )
	{
	    if ( !ch->pcdata->restart )
	    {
		restart = calloc( sizeof( RESTART_DATA ), 1 );
		ch->pcdata->restart = restart;
	    }
	    else
	    {
		for ( restart = ch->pcdata->restart; restart->nast; restart = restart->nast )
		    ;

		restart->nast = calloc( sizeof( RESTART_DATA ), 1 );
		restart = restart->nast;
	    }

	    restart->wpis = fread_string( fp, &status );
	}

	/* Lam 30.3.2002 */
	else if ( !str_cmp( word, "Zn" ) )
	{
	    char *kupa;

	    kupa = fread_string( fp, &status );
	    ch->pcdata->strefa = znajdz_strefe( kupa );
	    free_string( kupa );
	}

	else if ( !str_cmp( word, "LovedBy2" ) )
	{
	    ch->pcdata->love.do_kiedy = fread_number( fp, &status );
	    free_string( ch->pcdata->love.od );
	    ch->pcdata->love.od = fread_string( fp, &status );
	}
	else if ( !str_cmp( word, "SilencedBy2" ) )
	{
	    ch->pcdata->silence.do_kiedy = fread_number( fp, &status );
	    free_string( ch->pcdata->silence.od );
	    ch->pcdata->silence.od = fread_string( fp, &status );
	}
	else if ( !str_cmp( word, "NoemotedBy2" ) )
	{
	    ch->pcdata->noemote.do_kiedy = fread_number( fp, &status );
	    free_string( ch->pcdata->noemote.od );
	    ch->pcdata->noemote.od = fread_string( fp, &status );
	}
	else if ( !str_cmp( word, "NocolouredBy2" ) )
	{
	    ch->pcdata->nocolour.do_kiedy = fread_number( fp, &status );
	    free_string( ch->pcdata->nocolour.od );
	    ch->pcdata->nocolour.od = fread_string( fp, &status );
	}
	else if ( !str_cmp( word, "NotelledBy2" ) )
	{
	    ch->pcdata->notell.do_kiedy = fread_number( fp, &status );
	    free_string( ch->pcdata->notell.od );
	    ch->pcdata->notell.od = fread_string( fp, &status );
	}
	else if ( !str_cmp( word, "LoggedBy2" ) )
	{
	    ch->pcdata->log.do_kiedy = fread_number( fp, &status );
	    free_string( ch->pcdata->log.od );
	    ch->pcdata->log.od = fread_string( fp, &status );
	}
	else if ( !str_cmp( word, "FreezedBy2" ) )
	{
	    ch->pcdata->freeze.do_kiedy = fread_number( fp, &status );
	    free_string( ch->pcdata->freeze.od );
	    ch->pcdata->freeze.od = fread_string( fp, &status );
	}
	else if ( !str_cmp( word, "DenyBy2" ) )
	{
	    ch->pcdata->deny.do_kiedy = fread_number( fp, &status );
	    free_string( ch->pcdata->deny.od );
	    ch->pcdata->deny.od = fread_string( fp, &status );
	}

	else
	{
	    sprintf( buf, "fread_char: Unknown key '%s' in pfile.", word );
	    bug( buf, 0 );
	    fread_to_eol( fp );
	}

    }

		/* Require all manditory fields, set defaults */

    for ( i = 0; *key_tab[ i ].key; i++ )
    {

	if ( key_tab[ i ].string == SPECIFIED ||
	     key_tab[ i ].deflint == DEFLT )
	    continue;

	if ( !str_cmp( key_tab[ i ].key, "MArmr" ) && save_ver < 7 )
	    continue;

	if ( key_tab[ i ].deflint == MAND )
	{
	    sprintf( buf, "Manditory field '%s' missing from pfile.",
			  key_tab[ i ].key );
	    bug( buf, 0 );
	    error_count++;
	    continue;
	}

	       /* This if/else sets default strings and numbers */

	if ( key_tab[ i ].string && key_tab[ i ].deflptr )
	{
	    free_string( *(char **)key_tab[ i ].ptrs[ 0 ] );
	    *(char **)key_tab[ i ].ptrs[ 0 ] =
	      str_dup( (char *)key_tab[ i ].deflptr );
	}
	else
	    for ( j = 0; key_tab[ i ].ptrs[ j ]; j++ )
		*(int *)key_tab[ i ].ptrs[ j ] = key_tab[ i ].deflint;
    }

		/* Fixups */

    set_title( ch, ch->pcdata->title );

    if ( !str_cmp( ch->pcdata->prompt, "<%A%hpu %mm %vr %xd`s> " ) )
    {
	free_string( ch->pcdata->prompt );
	ch->pcdata->prompt = str_dup( "<%A%h`z %mm %vr %xd`s> " );
    }

    if ( !str_cmp( ch->pcdata->prompt, "<%A%hpu %mm %vr %xd`s><%npu %cm> " ) )
    {
	free_string( ch->pcdata->prompt );
	ch->pcdata->prompt = str_dup( "<%A%h`z %mm %vr %xd`s><%n`z %cm> " );
    }

    if ( !str_cmp( ch->pcdata->prompt, "<%A%hpu(%p%%) %mm(%P%%) %vr %xd`s> " ) )
    {
	free_string( ch->pcdata->prompt );
	ch->pcdata->prompt = str_dup( "<%A%h`z(%p%%) %mm(%P%%) %vr %xd`s> " );
    }

    if ( ch->polskie > 4 )
	ch->polskie = 0; /* wczesniej do_polskie tego wcale nie sprawdzalo */

    if ( ch->desc )
	ch->desc->polskie = ch->polskie;

    if ( !hint_table[ ch->pcdata->hint ] ) /* blad w hints.txt? */
    {
	REMOVE_BIT( ch->pcdata->opcje, OPC_HINT );
	ch->pcdata->hint = 0;
    }

    if ( IS_IMMORTAL( ch )
      && ( ch->pcdata->wizinvis < 1
	|| ch->pcdata->wizinvis > get_trust( ch ) ) )
	ch->pcdata->wizinvis = get_trust( ch );

    /* wersja 2 skonwertowala wiekszosc, ale 4 (a przynajmniej 4 pamietam)
       polecenia sie "zaplataly" */
    if ( save_ver < 9 )
	konwersja_bumi( ch );

    if ( save_ver < 3 ) /* 3 - wprowadzenie opcji */
	konwersja_opcji( ch );

    if ( save_ver < 4 || !*ch->wolacz ) /* 4 - dodany wolacz */
	SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_WOLACZ );

    sprawdz_wampira( ch );

    if ( save_ver < 5 ) /* 5 - denominacja */
    {
	int moby, przeliczone, hamulec;

	moby = ch->gold / ( ch->level * ch->level * 10 );
	przeliczone = moby * ZYSK( ch->level, 10 );
	hamulec = WYDATEK( ch->level, 160 ); /* 2 treningi atrybutow */
/*	if ( przeliczone > hamulec )
	    STC( "Tw`oj portfel zosta`l okradziony!\n\r", ch ); */
	przeliczone = UMIN( przeliczone, hamulec );
	ch->gold = przeliczone;

	moby = ch->pcdata->bank / ( ch->level * ch->level * 10 );
	przeliczone = moby * ZYSK( ch->level, 10 );
	hamulec = WYDATEK( ch->level, 320 ); /* plus dla oszczedzajacych */
	przeliczone = UMIN( przeliczone, hamulec );
	ch->pcdata->bank = przeliczone;
    }

    if ( IS_SET( ch->act, PLR_LOG ) )
    {
	char  logfile[ MSL ];

	sprintf( logfile, "../log/player/%s.log",
		 capitalize( ch->name ) );

	if ( !( ch->pcdata->fLog = fopen( logfile, "a" ) ) )
	{
	    sprintf( buf, "fread_char: fopen %s", logfile );
	    /* bug( buf, 0 ); */
	    lac_perror( logfile );
	}
	else
	    setvbuf( ch->pcdata->fLog, (char *) NULL, _IOLBF, 0 );
    }

    /* Lam 11.1.2008 */
    if ( CZY_OPCJA( ch, OPC_AUTOWCIEL ) && ch->pcdata->ic_in_room )
	ch->in_room = ch->pcdata->ic_in_room;

    /* Lam 7.9.2005: moze sie zdarzyc po morderstwie, kiedy to postac jest
       zapisywana po zdobyciu flagi mordercy, a nastepnie ktos zabije muda (lub
       sie wywali, ale znalazlem za pomoca ^C :) Ta flaga powinna isc do
       temp_flags, ale z lenistwa bedzie tak: */
    if ( IS_SET( ch->act, PLR_CHCE_BYC_MORDERCA ) )
	REMOVE_BIT( ch->act, PLR_CHCE_BYC_MORDERCA );

    if ( zabrane_dodatkowe )
	ch_printf( ch, "\n\r                                 {R---===*===---{x\n\r"
		"{YUwaga, twoja posta`c zosta`la pozbawiona dodatkowych umiej`etno`sci w liczbie %d.{x\n\r"
		"Twoim zadaniem jako gracza jest zapomnienie o tym, `ze posta`c kiedykolwiek\n\r"
		"posiada`la wi`ecej umiej`etno`sci, ni`z ma dost`epnych aktualnie.\n\r"
		"Aby umili`c ci t`e zmian`e, bogowie oddaj`a ci sesje `cwicze`n z du`z`a nadwy`zk`a.\n\r"
		"Niekt`ore dodatkowe umiej`etno`sci b`ed`a te`z dost`epne w ksi`egach w gildii.\n\r"
		"Pami`etaj - twoja posta`c nigdy nie posiada`la wi`ecej umiej`etno`sci ni`z teraz!\n\r"
		"                                 {R---===*===---{x\n\r\n\r", zabrane_dodatkowe );

    /* Ulryk: rozdzielenie KP na wplywy magiczne i ze zbroi */
    if ( save_ver < 7 )
    {
	ch->armor = 0;
	ch->marmor = 0;
	ch->saving_throw = 0;

	for ( paf = ch->affected; paf; paf = paf->next )
	{
	    if ( paf->location == APPLY_AC )
	    {
		paf->modifier = - paf->modifier;
		ch->marmor += paf->modifier;
	    }

	    if ( paf->location == APPLY_SAVING_PARA
	      || paf->location == APPLY_SAVING_ROD
	      || paf->location == APPLY_SAVING_PETRI
	      || paf->location == APPLY_SAVING_BREATH
	      || paf->location == APPLY_SAVING_SPELL )
	    {
		paf->modifier = - paf->modifier;
		ch->saving_throw += paf->modifier;
	    }
	}

	/* Lam */
	STC( "\n\r                                 {R---===*===---{x\n\r"
	     "Drogi graczu. Uleg`l zmianie system rozgrywania walk. Zanim zaczniesz w`edr`owki,\n\r"
	     "zapoznaj si`e z pomocami {?WALKA{x, {?KP{x i {?OPC{x oraz pomocami do atrybut`ow.\n\r"
	     "                                 {R---===*===---{x\n\r\n\r", ch );
    }

    if ( save_ver < 8 && ch->level >= L_APP && ch->level < LEVEL_IMMORTAL )
    {
	zabierz_um_bwwu( ch );
	SET_BIT( ch->pcdata->temp_flags, TEMPFLAG_KMNKT_BWWU );
    }

    zjedz_entery_z_konca( &ch->long_descr );
    zjedz_entery_z_konca( &ch->long_descr_orig );

    return error_count;
}


/*
 * Funkcja szukajaca w pliku nastepnego przedmiotu/klanu/konca pliku.
 * Lam: dodalem obsluge przechowalni i klanow. Nie jestem pewien, czy to w ogole
 * dziala, na szczescie nigdy nie jest uzywane ;)
 */
void recover( FILE *fp, long fpos )
{
    char buf[ MAX_STRING_LENGTH ];

    fseek( fp, fpos, 0 );

    while ( !feof( fp ) )
    {
	fpos = ftell( fp );

	if ( !fgets( buf, MAX_STRING_LENGTH, fp ) )
	    return;

	if ( !strncmp( buf, "#OBJECT", 7 ) ||
	     !strncmp( buf, "#PRZECHOWALNIA", 14 ) ||
	     !strncmp( buf, "#CLAN", 5 ) || /* w pliku postaci i tak bez znaczenia */
	     !strncmp( buf, "#END", 4 ) )
	{
	    fseek( fp, fpos, 0 );
	    return;
	}
    }

    return;
}


/*
 *  0 - do inwentarza ch
 *  1 - do przechowalni ch
 *  2 - do przedmiotu (uzywane przy urnach)
 *  3 - do pomieszczenia (zapisywanie lezacych na ziemi zwlok)
 */
int fread_obj( void *parametr, FILE *fp, int use )
{
    CHAR_DATA        *ch = (CHAR_DATA *) parametr;
    OBJ_DATA         *par_obj = (OBJ_DATA *) parametr;
    ROOM_INDEX_DATA  *par_room = (ROOM_INDEX_DATA *) parametr;
    OBJ_DATA         obj;
    OBJ_DATA         *new_obj;
    AFFECT_DATA      *paf;
    char              buf[ MAX_STRING_LENGTH ];
    char             *spell_name = NULL;
    char             *p          = NULL;
    char             *word;
    char             *tmp_ptr;
    bool              fNest;
    bool              fVnum;
    bool              fString[ 6 ];
    bool              byly_extra_flags = FALSE;
    long              fpos;
    int               iNest;
    int               iValue;
    int               status;
    int               sn;
    int               vnum;
    int               num_keys;
    int               last_key   = 0;
    int               i, j, tmpi;

/* To bylo zle, teraz DEFLT brane jest z obj.pIndexData
    char              corobj [] = "(Uszkodzony przedmiot!)"; */

/*
 * 7.5.98 - wielkie zmiany w tablicy (defaults) i branie z indeksu. Nawet
 * wear_loc nie jest MAND - to dlatego, ze i tak przedmioty zapisuja sie zdjete,
 * wiec nawet uszkodzenia plikow nie przeszkadzaja. Wiekszosc -1 oznacza, ze
 * bedzie brane z index_data (wear_loc i timer oczywiscie nie) w przypadku braku
 * w pliku. Pliki zmniejszylem kilkakrotnie (zalezy od ilosci przedmiotow)! :)
 */
    struct key_data key_tab[ ] =
    {
	{ "Name",        TRUE,  DEFLT, 0,         { &obj.name,        NULL } },
	{ "ShortDescr",  TRUE,  DEFLT, 0,         { &obj.short_descr, NULL } },
	{ "Dopelniacz",  TRUE,  DEFLT, 0,         { &obj.dopelniacz,  NULL } },
	{ "Celownik",    TRUE,  DEFLT, 0,         { &obj.celownik,    NULL } },
	{ "Biernik",     TRUE,  DEFLT, 0,         { &obj.biernik,     NULL } },
	{ "Narzednik",   TRUE,  DEFLT, 0,         { &obj.narzednik,   NULL } },
	{ "Miejscownik", TRUE,  DEFLT, 0,         { &obj.miejscownik, NULL } },
	{ "Description", TRUE,  DEFLT, 0,         { &obj.description, NULL } },
	{ "LookDescr",	 TRUE,  DEFLT, 0,	  { &obj.look_descr,  NULL } },
	{ "Action",      TRUE,  DEFLT, 0,         { &obj.action,      NULL } },
	{ "AClass",      TRUE,  DEFLT, 0,         { &obj.anti_class,  NULL } },
	{ "ARace",       TRUE,  DEFLT, 0,         { &obj.anti_race,   NULL } },
	{ "Owner",       TRUE,  DEFLT, 0,         { &obj.owner,       NULL } },
	{ "Rodzaj",      FALSE, -1,    0,         { &obj.rodzaj,      NULL } },
	{ "WearFlags",   FALSE, -1,    0,         { &obj.wear_flags,  NULL } },
	{ "WearLoc",     FALSE, -1,    0,         { &obj.wear_loc,    NULL } },
	{ "ItemType",    FALSE, -1,    0,         { &obj.item_type,   NULL } },
	{ "Weight",      FALSE, -1,    0,         { &obj.weight,      NULL } },
	{ "Level",       FALSE, 0,     0,         { &obj.level,       NULL } },
	{ "Timer",       FALSE, -1,    0,         { &obj.timer,       NULL } },
	{ "Cost",        FALSE, 300,   0,         { &obj.cost,        NULL } },
	{ "PlrCost",     FALSE, 0,     0,         { &obj.plr_shop_cost, NULL } },
	{ "Values",      FALSE, DEFLT, 0,         { &obj.value [ 0 ].v,
						    &obj.value [ 1 ].v,
						    &obj.value [ 2 ].v,
						    &obj.value [ 3 ].v, NULL } },
	{ "Val",	FALSE, DEFLT,  0,	  { &obj.value [ 0 ].v,
						    &obj.value [ 1 ].v,
						    &obj.value [ 2 ].v,
						    &obj.value [ 3 ].v,
						    &obj.value [ 4 ].v,
						    &obj.value [ 5 ].v, NULL } },
	{ "\0",          FALSE, 0,     0,         { NULL }                 }
    };

/* UWAGA! Tutaj niebezpieczna rzecz: Values nie sa MAND tylko DEFLT w zwiazku ze
   zmiana formatu z 'Values' na 'Val' (6 values w miejsce 4). W przyszlosci
   sugeruje sie zabic Values, a Val dac MAND. Ale to po autokonwersji postaci
   i skasowaniu nieuzywanych. */

    memset( &obj, 0, sizeof( OBJ_DATA ) );

    obj.name        = str_dup( "" );
    obj.short_descr = str_dup( "" );
    obj.dopelniacz  = str_dup( "" );
    obj.celownik    = str_dup( "" );
    obj.biernik     = str_dup( "" );
    obj.narzednik   = str_dup( "" );
    obj.miejscownik = str_dup( "" );
    obj.description = str_dup( "" );
    obj.look_descr  = str_dup( "" );
    obj.action      = str_dup( "" );
    obj.anti_class  = str_dup( "" );
    obj.anti_race   = str_dup( "" );
    obj.owner       = str_dup( "" );
    obj.deleted     = FALSE;

    fNest           = TRUE; /* Lam 8.5.98: bylo FALSE, teraz jest lepiej */
/*
    fVnum           = TRUE;
    tak bylo - to straszne, ale w oryginalnej Envy 2.2 tak bylo! :(
*/
    fVnum           = FALSE;
    fString[ 0 ] = fString[ 1 ] = fString[ 2 ] = fString[ 3 ] = fString[ 4 ]
	= fString[ 5 ] = FALSE;
    iNest           = 0;

    new_obj = new_object( );

    for ( num_keys = 0; *key_tab[ num_keys ].key; )
	num_keys++;

    for ( fpos = ftell( fp ) ; !feof( fp ) ; )
    {

	word = fread_word( fp, &status, TRUE );

	if ( !word )
	{
	    char bufek [ MAX_STRING_LENGTH ];

	    bug( "fread_obj: funkcja fread_word zwrocila NULL! ...", 0 );
	    if ( use == 0 || use == 1 ) /* to oznacza postac */
	    {
		sprintf( bufek, "Zla postac: %s, plik zostal skopiowany do ../player/bug/", ch->name );
		bug( bufek, 0 );
		wiznet( bufek, ch, NULL, NULL, WIZ_DEBUG, 0, 0 );
		sprintf( bufek, "cp %s%s/%s %sbug/%s",
		    PLAYER_DIR, initial( ch->name ), ch->name, PLAYER_DIR, ch->name );
		if ( system( bufek ) )
		    bug( "Nie da sie skopiowac!", 0 );
	    }
	    recover    ( fp, fpos        );
	    free_string( obj.name        );
	    free_string( obj.short_descr );
	    free_string( obj.dopelniacz  );
	    free_string( obj.celownik    );
	    free_string( obj.biernik     );
	    free_string( obj.narzednik   );
	    free_string( obj.miejscownik );
	    free_string( obj.action      );
	    free_string( obj.anti_race   );
	    free_string( obj.anti_class  );
	    free_string( obj.owner       );
	    free_string( obj.description );
	    free_string( obj.look_descr  );
	    extract_obj( new_obj         );
	    return FREAD_OBJ_BLAD;
	}

	/* szuka poczawszy od poprzednio znalezionego klucza */
	for ( i = last_key;
	      i < last_key + num_keys &&
		str_cmp( key_tab[ i % num_keys ].key, word ); )
	    i++;

	i = i % num_keys;

	if ( !str_cmp( key_tab[ i ].key, word ) )
	    last_key = i + 1;
	else
	    i = num_keys;

	if ( *key_tab[ i ].key )         /* Key entry found in key_tab */
	{
	    if ( key_tab[ i ].string == SPECIFIED )
		bug( "Fread_obj: key already specified.", 0 );

			/* Entry is a string */

	    else if ( key_tab[ i ].string )
	    {
		if ( ( p = fread_string( fp, &status ) ) && !status )
		{
		   free_string ( * (char **) key_tab[ i ].ptrs[ 0 ] );
		   * (char **) key_tab[ i ].ptrs[ 0 ] = p;
		}
	    }

			/* Entry is an integer */
	    else
		for ( j = 0; key_tab[ i ].ptrs[ j ]; j++ )
		{
		    tmpi = fread_number( fp, &status );
		    if ( !status )
			* (int *) key_tab[ i ].ptrs[ j ] = tmpi;
		}

	    if ( status )
	    {
		fread_to_eol( fp );
		continue;
	    }
	    else
		key_tab[ i ].string = SPECIFIED;
	}

	else if ( *word == '*' )
	    fread_to_eol( fp );

	else if ( !str_cmp( word, "End" ) )
	{
	    if ( !fNest || !fVnum )
	    {
		bug( "Fread_obj: incomplete object.", 0 );

		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.dopelniacz  );
		free_string( obj.celownik    );
		free_string( obj.biernik     );
		free_string( obj.narzednik   );
		free_string( obj.miejscownik );
		free_string( obj.action      );
		free_string( obj.anti_class  );
		free_string( obj.anti_race   );
		free_string( obj.owner       );
		free_string( obj.description );
		free_string( obj.look_descr  );
		extract_obj( new_obj         );

		return FREAD_OBJ_BLAD;
	    }
	    break;
	}

	else if ( !str_cmp( word, "Nest" ) )
	{
	    iNest = fread_number( fp, &status );

	    if ( status )       /* Losing track of nest level is bad */
	    {
		/* teraz jednak zamiast wywalac muda, sprobujemy ocalec.
		   Lam 14.6.2002 */
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.dopelniacz  );
		free_string( obj.celownik    );
		free_string( obj.biernik     );
		free_string( obj.narzednik   );
		free_string( obj.miejscownik );
		free_string( obj.action      );
		free_string( obj.anti_class  );
		free_string( obj.anti_race   );
		free_string( obj.owner       );
		free_string( obj.description );
		free_string( obj.look_descr  );
		extract_obj( new_obj         );
		return FREAD_OBJ_BLAD;
	    }

	    else if ( iNest < 0 || iNest >= MAX_NEST )
	    {
		bug( "Fread_obj: bad nest %d.", iNest );
		iNest = 0;
	    }
	    /* Lam: przeniesione na koniec, zeby w pliku nie trzeba bylo pisac
	       "Nest 0". Wiec teraz Nest nie jest wymagany i przyjmowany
	       domyslnie 0, a w tablice zostanie wpisane po wczytaniu calego
	       obiektu. */
/*
	    else
	    {
		rgObjNest[ iNest ] = new_obj;
		fNest = TRUE;
	    }
*/
	}

	else if ( !str_cmp( word, "String" ) )
	{
	    iValue = fread_number( fp, &status );

	    /* spell_name uzyte z lenistwa, oczywiscie chodzi o lancuch */
	    if ( !status )
		spell_name = fread_string( fp, &status );

	    if ( status )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( iValue < 0 || iValue > 5 )
		bug( "Fread_obj: bad iValue %d.", iValue );
	    else
	    {
		obj.value[ iValue ].p = (void *) spell_name;
		fString[ iValue ] = TRUE;
	    }
	}

	else if ( !str_cmp( word, "Spell" ) )
	{
	    iValue = fread_number( fp, &status );

	    if ( !status )
		spell_name = fread_word( fp, &status, TRUE );

	    if ( status )       /* Recover is to skip spell */
	    {
		fread_to_eol( fp );
		continue;
	    }

	    sn = skill_lookup_ang( spell_name );

	    if ( iValue < 0 || iValue > 5 )
		bug( "Fread_obj: bad iValue %d.", iValue );

	    else if ( sn < 0 )
		bug( "Fread_obj: unknown skill.", 0 );

	    else
		obj.value[ iValue ].v = sn;
	}

	else if ( !str_cmp( word, "Material" ) )
	{
	    iValue = fread_number( fp, &status );

	    if ( !status )
		spell_name = fread_word( fp, &status, TRUE );

	    if ( status )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    /* tak ktos kiedys napisal :( */
	    if ( !str_cmp( spell_name, "bronz" ) )
		spell_name = "br`az";
	    /* a tego sie pozbylismy, ogromne kontrowersje :) */
	    else if ( !str_cmp( spell_name, "adamantium" ) )
		spell_name = "stal";

	    sn = material_lookup( spell_name );

	    if ( iValue < 0 || iValue > 5 )
		bug( "Fread_obj: bad iValue %d.", iValue );
	    else if ( sn < 0 )
	    {
		sprintf( bug_buf, "Fread_obj: unknown material '%s'.", spell_name );
		bug( bug_buf, 0 );
		obj.value[ iValue ].v = 0; /* na sztywno "nic" */
	    }
	    else
		obj.value[ iValue ].v = sn;
	}

	else if ( !str_cmp( word, "Vnum" ) )
	{

	    vnum = fread_number( fp, &status );

	    if ( status )               /* Can't live without vnum */
	    {
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.dopelniacz  );
		free_string( obj.celownik    );
		free_string( obj.biernik     );
		free_string( obj.narzednik   );
		free_string( obj.miejscownik );
		free_string( obj.action      );
		free_string( obj.anti_class  );
		free_string( obj.anti_race   );
		free_string( obj.owner       );
		free_string( obj.description );
		free_string( obj.look_descr  );
		extract_obj( new_obj         );
		return FREAD_OBJ_BLAD;
	    }

	    if ( !( obj.pIndexData = get_obj_index( vnum ) ) )
	    /* ALE NUMER! Kolesie z Envy nie wpadli na to, ze tu trzeba zrobic
	       return FALSE i sie wszystko ostro psulo... No ale jesli ktos ma
	       muda doskonalego i nie musi testowac kodu odpowiedzialnego za
	       naprawianie niedoskonalosci :) Tak naprawde to nie przewidzieli
	       nawet, ze ktos moze chciec usunac jedna z ich "slusznych" krain */
	    {
		/* Lam 22.12.2005: lepsze pokazywanie, komu konkretnie
		   zaszkodzilismy usuwajac krainy/przedmioty :) */
		if ( use == 0 )
		    sprintf( log_buf, "Fread_obj (postac '%s', inwen.): brak vnuma %d.",
			ch->short_descr, vnum );
		else if ( use == 1 )
		    sprintf( log_buf, "Fread_obj (postac '%s', przech.): brak vnuma %d.",
			ch->short_descr, vnum );
		else if ( use == 2 )
		    sprintf( log_buf, "Fread_obj (przedmiot '%s'/%d, urna): brak vnuma %d.",
			par_obj->short_descr, par_obj->pIndexData->vnum, vnum );
		else if ( use == 3 )
		    sprintf( log_buf, "Fread_obj (pom. %d, cialo): brak vnuma %d.",
			par_room->vnum, vnum );
		else
		    sprintf( log_buf, "Fread_obj (blad 'use'!): brak vnuma %d.",
			vnum );
		log_string( log_buf );

		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.dopelniacz  );
		free_string( obj.celownik    );
		free_string( obj.biernik     );
		free_string( obj.narzednik   );
		free_string( obj.miejscownik );
		free_string( obj.action      );
		free_string( obj.anti_class  );
		free_string( obj.anti_race   );
		free_string( obj.owner       );
		free_string( obj.description );
		free_string( obj.look_descr  );
		extract_obj( new_obj         );
		return FREAD_OBJ_VNUM;
	    }
	    else
		fVnum = TRUE;
	}

		/* The following keys require extra processing */

	else if ( !str_cmp( word, "Affect" ) )
	{
	    paf = new_affect( );

	    paf->type       = fread_number( fp, &status );
	    if ( paf->type < 0 || paf->type >= MAX_SKILL )
		paf->type = 0;
	    paf->duration   = fread_number( fp, &status );
	    paf->modifier   = fread_number( fp, &status );
	    paf->location   = fread_number( fp, &status );
	    CLEAR_BITS( paf->bitvector );
	    paf->bitvector[ 0 ]  = fread_number( fp, &status );
	    if ( save_ver < 7
	      && ( paf->location == APPLY_AC
		|| paf->location == APPLY_SAVING_PARA
		|| paf->location == APPLY_SAVING_ROD
		|| paf->location == APPLY_SAVING_PETRI
		|| paf->location == APPLY_SAVING_BREATH
		|| paf->location == APPLY_SAVING_SPELL ) )
		paf->modifier = 0 - paf->modifier;

	    paf->next = obj.affected;
	    obj.affected = paf;
	}

	else if ( !str_cmp( word, "Affect2" ) )
	{
	    paf = new_affect( );

	    paf->type       = fread_number( fp, &status );
	    if ( paf->type < 0 || paf->type >= MAX_SKILL )
		paf->type = 0;
	    paf->duration   = fread_number( fp, &status );
	    paf->modifier   = fread_number( fp, &status );
	    paf->location   = fread_number( fp, &status );
	    CLEAR_BITS( paf->bitvector );
	    paf->bitvector[ 0 ]  = fread_number( fp, &status );
	    paf->bitvector[ 1 ] = fread_number( fp, &status );
	    if ( save_ver < 7
	      && ( paf->location == APPLY_AC
		|| paf->location == APPLY_SAVING_PARA
		|| paf->location == APPLY_SAVING_ROD
		|| paf->location == APPLY_SAVING_PETRI
		|| paf->location == APPLY_SAVING_BREATH
		|| paf->location == APPLY_SAVING_SPELL ) )
		paf->modifier = 0 - paf->modifier;

	    paf->next = obj.affected;
	    obj.affected = paf;
	}

	else if ( !str_cmp( word, "Affect3" ) )
	{
	    paf = new_affect( );

	    spell_name = fread_word( fp, &status, TRUE );

	    if ( status )       /* Recover is to skip spell */
	    {
		bug( "status in Affect3", 0 );
		fread_to_eol( fp );
		continue;
	    }

	    sn = affect_lookup( spell_name );

	    if ( sn < 0 )
	    {
		sprintf( bug_buf, "Fread_obj: unknown skill '%s' for Affect3.", spell_name );
		bug( bug_buf, 0 );
		paf->type   = 0;
	    }
	    else
		paf->type   = sn;
	    paf->duration   = fread_number( fp, &status );
	    paf->modifier   = fread_number( fp, &status );
	    paf->location   = fread_number( fp, &status );
	    fread_vector( paf->bitvector, fp, &status );
	    /* if ( save_ver < 7 --niemozliwe z Affect3 */

	    paf->next = NULL;
	    if ( !obj.affected )
		obj.affected = paf;
	    else
	    {
		AFFECT_DATA *tpaf;
		tpaf = obj.affected;
		while ( tpaf->next )
		    tpaf = tpaf->next;
		tpaf->next = paf;
	    }
	}

	else if ( !str_cmp( word, "ExtraDescr" ) )
	{
	    tmp_ptr = fread_string( fp, &status );

	    if ( !status )
		p = fread_string( fp, &status );

	    if ( status )
	    {
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.dopelniacz  );
		free_string( obj.celownik    );
		free_string( obj.biernik     );
		free_string( obj.narzednik   );
		free_string( obj.miejscownik );
		free_string( obj.action      );
		free_string( obj.anti_class  );
		free_string( obj.anti_race   );
		free_string( obj.owner       );
		free_string( obj.description );
		free_string( obj.look_descr  );
		extract_obj( new_obj         );
		return FREAD_OBJ_BLAD;
	    }

	    /* POMIJANE, nie wystepowalo w plikach graczy, szkoda zachodu */
	    free( tmp_ptr );
	    free( p );
	}
	else if ( !str_cmp( word, "ExtraFlags" ) )
	{
	    CLEAR_BITS( obj.extra_flags );
	    obj.extra_flags[ 0 ] = fread_number( fp, &status );
	    byly_extra_flags = TRUE;

	    if ( status )
	    {
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.dopelniacz  );
		free_string( obj.celownik    );
		free_string( obj.biernik     );
		free_string( obj.narzednik   );
		free_string( obj.miejscownik );
		free_string( obj.action      );
		free_string( obj.anti_class  );
		free_string( obj.anti_race   );
		free_string( obj.owner       );
		free_string( obj.description );
		free_string( obj.look_descr  );
		extract_obj( new_obj         );
		return FREAD_OBJ_BLAD;
	    }
	}
	else if ( !str_cmp( word, "ExtraFlags2" ) )
	{
	    fread_vector( obj.extra_flags, fp, &status );
	    byly_extra_flags = TRUE;

	    if ( status )
	    {
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.dopelniacz  );
		free_string( obj.celownik    );
		free_string( obj.biernik     );
		free_string( obj.narzednik   );
		free_string( obj.miejscownik );
		free_string( obj.action      );
		free_string( obj.anti_class  );
		free_string( obj.anti_race   );
		free_string( obj.owner       );
		free_string( obj.description );
		free_string( obj.look_descr  );
		extract_obj( new_obj         );
		return FREAD_OBJ_BLAD;
	    }
	}
	else
	{
	    sprintf( log_buf, "Nieznany klucz '%s' w sekcji #OBJECT postaci '%s'.",
		word, use < 2 ? ch->name : "(brak)" );
	    bug( log_buf, 0 );
	}
    }

    /* skopiowane z "End", ktory wcale nie jest wymagany (wystarczy EOF, zeby tu
       dojsc */
    if ( !fNest || !fVnum )
    {
	bug( "Fread_obj: incomplete object (without 'End').", 0 );

	recover    ( fp, fpos        );
	free_string( obj.name        );
	free_string( obj.short_descr );
	free_string( obj.dopelniacz  );
	free_string( obj.celownik    );
	free_string( obj.biernik     );
	free_string( obj.narzednik   );
	free_string( obj.miejscownik );
	free_string( obj.action      );
	free_string( obj.anti_class  );
	free_string( obj.anti_race   );
	free_string( obj.owner       );
	free_string( obj.description );
	free_string( obj.look_descr  );
	extract_obj( new_obj         );

	return FREAD_OBJ_BLAD;
    }

    /* wymaga pol z MAND; DEFLT i SPECIFIED ignoruje, ustawia domyslne wartosci */
    for ( i = 0; *key_tab[ i ].key; i++ )
    {

	if ( key_tab[ i ].string == SPECIFIED ||
	     key_tab[ i ].deflint == DEFLT )
	    continue;

	if ( key_tab[ i ].deflint == MAND )
	{
	    sprintf( buf, "Manditory OBJECT field '%s' missing from pfile.",
		    key_tab[ i ].key );
	    bug( buf, 0 );

	    recover    ( fp, fpos        );
	    free_string( obj.name        );
	    free_string( obj.short_descr );
	    free_string( obj.dopelniacz  );
	    free_string( obj.celownik    );
	    free_string( obj.biernik     );
	    free_string( obj.narzednik   );
	    free_string( obj.miejscownik );
	    free_string( obj.action      );
	    free_string( obj.anti_class  );
	    free_string( obj.anti_race   );
	    free_string( obj.owner       );
	    free_string( obj.description );
	    free_string( obj.look_descr  );
	    extract_obj( new_obj         );

	    return FREAD_OBJ_BLAD;
	}

		/* This if/else sets default strings and numbers */

	if ( key_tab[ i ].string && key_tab[ i ].deflptr )
	{
	    free_string ( * (char **) key_tab[ i ].ptrs[ 0 ] );
	    * (char **) key_tab[ i ].ptrs[ 0 ] =
			str_dup( (char *) key_tab[ i ].deflptr );
	}
	else
	    for ( j = 0; key_tab[ i ].ptrs[ j ]; j++ )
		* (int *) key_tab[ i ].ptrs[ j ] = key_tab[ i ].deflint;
    }

    /* Lam 7.5.98: ponizsze ustawia rzeczy z OBJ_INDEX_DATA, a nie glupoty
       tak jak Envy */
    /* name zostawilem w pliku zeby widziec jakie mam przedmioty */
    /* jednak nie zostawilem */
    if ( !*obj.name )
    {   free_string( obj.name );
	obj.name = str_dup( obj.pIndexData->name ); }
    /* ponizsze nigdy nie wystepuja w pliku, jesli moga byc wziete z indeksu */
    if ( !*obj.short_descr )
    {   free_string( obj.short_descr );
	obj.short_descr = str_dup( obj.pIndexData->short_descr ); }
    if ( !*obj.dopelniacz )
    {   free_string( obj.dopelniacz );
	obj.dopelniacz = str_dup( obj.pIndexData->dopelniacz ); }
    if ( !*obj.celownik )
    {   free_string( obj.celownik );
	obj.celownik = str_dup( obj.pIndexData->celownik ); }
    if ( !*obj.biernik )
    {   free_string( obj.biernik );
	obj.biernik = str_dup( obj.pIndexData->biernik ); }
    if ( !*obj.narzednik )
    {   free_string( obj.narzednik );
	obj.narzednik = str_dup( obj.pIndexData->narzednik ); }
    if ( !*obj.miejscownik )
    {   free_string( obj.miejscownik );
	obj.miejscownik = str_dup( obj.pIndexData->miejscownik ); }
    if ( !*obj.description )
    {   free_string( obj.description );
	obj.description = str_dup( obj.pIndexData->description ); }
    if ( !*obj.look_descr )
    {   free_string( obj.look_descr );
	obj.look_descr = str_dup( obj.pIndexData->look_descr ); }
    if ( !*obj.action )
    {   free_string( obj.action );
	obj.action = str_dup( obj.pIndexData->action ); }
    if ( !*obj.anti_class )
    {   free_string( obj.anti_class );
	obj.anti_class = str_dup( obj.pIndexData->anti_class ); }
    if ( !*obj.anti_race )
    {   free_string( obj.anti_race );
	obj.anti_race = str_dup( obj.pIndexData->anti_race ); }

    if ( obj.rodzaj == -1 )
	obj.rodzaj = obj.pIndexData->rodzaj;
    if ( !byly_extra_flags )
	COPY_BITS( obj.extra_flags, obj.pIndexData->extra_flags );
    if ( obj.wear_flags == -1 )
	obj.wear_flags = obj.pIndexData->wear_flags;
    if ( obj.item_type == -1 )
	obj.item_type = obj.pIndexData->item_type;
    if ( obj.weight == -1 )
	obj.weight = obj.pIndexData->weight;

    if ( ( obj.item_type == ITEM_FOOD || obj.item_type == ITEM_DRINK_CON
	|| obj.item_type == ITEM_POTION || obj.item_type == ITEM_PILL )
      && !fString[ 4 ] )
    {
	/* Lam: na wypadek, gdyby w Val bylo cos innego niz 0, bez String */
	obj.value[ 4 ].p = NULL;
	/* a o tym tez nie pomyslal Alandar */
	if ( obj.pIndexData->value[ 4 ].p )
	    obj.value[ 4 ].p = (void *) str_dup( (char *) obj.pIndexData->value[ 4 ].p );
    }

    /* Lam 3.12.2004: po ostatnich zmianach w zrodlach swiatla mielibysmy zbyt
       wiele "unikatow", nieskonczonych zrodel swiatla z wplywami lub nie
       gasnacych pod woda latarni */
    if ( obj.item_type == ITEM_LIGHT )
    {
	obj.value[ 1 ].v = obj.pIndexData->value[ 1 ].v;
	if ( obj.value[ 2 ].v > obj.pIndexData->value[ 2 ].v )
	    obj.value[ 2 ].v = obj.pIndexData->value[ 2 ].v;
    }

    /* Lam 8.2.2006: mozliwe zwlaszcza, jesli typ przedmiotu zmieni sie
       w indeksie, a w pliku postaci nie bedzie pola "Material" */
    if ( obj.item_type == ITEM_WEAPON
      || obj.item_type == ITEM_ARMOR
      || obj.item_type == ITEM_TREASURE )
    {
	if ( obj.value[ 5 ].v < 0 || obj.value[ 5 ].v >= MAX_MATERIAL )
	    obj.value[ 5 ].v = 0;
    }

    /* Lam 27.6.2006: po zmianie wozu z w0 == 100 w zbroje na cialo na nasty
       poziom - podobne rzeczy przydalyby sie takze dla innych typow
       przedmiotow na przyszlosc */
    if ( obj.item_type == ITEM_ARMOR
      && !IS_OBJ_STAT( obj.pIndexData, ITEM_NORANDVALUES )
      && obj.value[ 0 ].v > obj.level / 4 + 3 )
	obj.value[ 0 ].v = obj.level / 4 + 3;

    memcpy( new_obj, &obj, sizeof( OBJ_DATA ) );

    new_obj->next = object_list;
    object_list   = new_obj;

    new_obj->pIndexData->count++;

    rgObjNest[ iNest ] = new_obj;
    /* Lam 27.6.2006: jesli przedmiot w krainie zmienil sie z pojemnika w cos
       innego, albo ktos ma przedmiot z zawartoscia wymuszona przez resety
       (teraz wykrywa to wykrywacz i od jakiegos czasu w krainach tego nie ma,
       ale byly dwa takie przedmioty), bedzie mu sie "wysypywala" zawartosc,
       niezaleznie od zagniezdzenia. Nie chce mi sie sprawdzac, czy moze
       mozna wsadzic w rgObjNest[ iNest - 2 ], nie spotkalem sie w praktyce :)
       Aha, tym sposobem w przechowalni kilku osobom zrobi sie wiecej
       przedmiotow niz moga przechowywac, ale to nic */
    if ( iNest == 0
      || ( rgObjNest[ iNest - 1 ]->item_type != ITEM_CONTAINER
	&& rgObjNest[ iNest - 1 ]->item_type != ITEM_CORPSE_NPC
	&& rgObjNest[ iNest - 1 ]->item_type != ITEM_CORPSE_PC ) )
    {
	if ( iNest )
	{
	    if ( use == 0 )
		sprintf( log_buf, "Fread_obj (postac '%s', inwen.): przedmiot %d z zawartoscia.",
		    ch->short_descr, rgObjNest[ iNest - 1 ]->pIndexData->vnum );
	    else if ( use == 1 )
		sprintf( log_buf, "Fread_obj (postac '%s', przech.): przedmiot %d z zawartoscia.",
		    ch->short_descr, rgObjNest[ iNest - 1 ]->pIndexData->vnum );
	    else if ( use == 2 )
		sprintf( log_buf, "Fread_obj (przedmiot '%s'/%d, urna): przedmiot %d z zawartoscia.",
		    par_obj->short_descr, par_obj->pIndexData->vnum,
		    rgObjNest[ iNest - 1 ]->pIndexData->vnum );
	    else if ( use == 3 )
		sprintf( log_buf, "Fread_obj (pom. %d, cialo): przedmiot %d z zawartoscia.",
		    par_room->vnum, rgObjNest[ iNest - 1 ]->pIndexData->vnum );
	    else
		sprintf( log_buf, "Fread_obj (blad 'use'!): przedmiot %d z zawartoscia.",
		    rgObjNest[ iNest - 1 ]->pIndexData->vnum );
	    log_string( log_buf );
	}

	if ( use == 0 )
	    obj_to_char( new_obj, ch );
	else if ( use == 1 )
	    obj_to_przechowalnia( new_obj, ch );
	else if ( use == 2 )
	    obj_to_obj( new_obj, par_obj );
	else
	    obj_to_room( new_obj, par_room );
    }
    else
	obj_to_obj( new_obj, rgObjNest[ iNest - 1 ] );

    if ( save_ver >= 1
      && new_obj->wear_loc > -1
      && new_obj->wear_loc < MAX_WEAR )
    {
	if ( save_eq[ new_obj->wear_loc ] )
	    bug( "fread_obj: save_eq[%d] juz istnieje!", new_obj->wear_loc );
	save_eq[ new_obj->wear_loc ] = new_obj;
	new_obj->wear_loc = -1;
    }

    return FREAD_OBJ_DB;
}


/*
 * Zwolnione wraz z wprowadzeniem ch->pcdata->opcje.
 */
#define XPLR_NO_SEPARATOR	b02	/* Lam (kiedys tu bylo PK) */
#define XPLR_AUTOEXIT		b03	/* czy ktos ma to wylaczone? */
#define XPLR_AUTOLOOT		b04
#define XPLR_AUTOSAC		b05
#define XPLR_BLANK		b06
#define XPLR_BRIEF		b07
#define XPLR_MIW		b08	/* Lam */
#define XPLR_HINT		b09	/* Lam (kiedys tu bylo COMBINE) */
#define XPLR_PROMPT		b10
#define XPLR_TELNET_GA		b11
#define XPLR_WRAP		b14	/* WRAP added/WIZBIT usuniete przez Lama */
#define XPLR_AUTOGOLD		b24
#define XPLR_AUTOSPLIT		b26
#define XPLR_SHOWTICK		b27
#define XPLR_SHOWLEVEL		b28
#define XPLR_COLOUR		b29	/* usuniete przez Lama 26.5.98 */
#define XPLR_CUTCP		b31	/* 32 bit! */

void konwersja_opcji( CHAR_DATA *ch )
{
	if ( IS_NPC( ch ) )
	{
		bug( "Konwersja opcji NPC!", 0 );
		return;
	}

	ch->pcdata->opcje =
		( IS_SET( ch->act, XPLR_AUTOEXIT ) ? OPC_AUTOEXIT : 0 ) |
		( IS_SET( ch->act, XPLR_AUTOGOLD ) ? OPC_AUTOGOLD : 0 ) |
		( IS_SET( ch->act, XPLR_AUTOLOOT ) ? OPC_AUTOLOOT : 0 ) |
		( IS_SET( ch->act, XPLR_AUTOSAC ) ? OPC_AUTOSAC : 0 ) |
		( IS_SET( ch->act, XPLR_AUTOSPLIT ) ? OPC_AUTOSPLIT : 0 ) |
		( IS_SET( ch->act, XPLR_BLANK ) ? OPC_BLANK : 0 ) |
		( IS_SET( ch->act, XPLR_BRIEF ) ? OPC_BRIEF : 0 ) |
		( IS_SET( ch->act, XPLR_CUTCP ) ? OPC_CUTCP : 0 ) |
		( IS_SET( ch->act, XPLR_HINT ) ? OPC_HINT : 0 ) |
		( IS_SET( ch->act, XPLR_MIW ) ? OPC_MIW : 0 ) |
		( IS_SET( ch->act, XPLR_NO_SEPARATOR ) ? OPC_NO_SEPARATOR : 0 ) |
		( IS_SET( ch->act, XPLR_PROMPT ) ? OPC_PROMPT : 0 ) |
		( IS_SET( ch->act, XPLR_SHOWLEVEL ) ? OPC_SHOWLEVEL : 0 ) |
		( IS_SET( ch->act, XPLR_SHOWTICK ) ? OPC_SHOWTICK : 0 ) |
		/* ( IS_SET( ch->act, XPLR_TELNET_GA ) ? OPC_TELNET_GA : 0 ) | */
		( IS_SET( ch->act, XPLR_WRAP ) ? OPC_WRAP : 0 );

	REMOVE_BIT( ch->act,	  XPLR_NO_SEPARATOR
				| XPLR_AUTOEXIT
				| XPLR_AUTOLOOT
				| XPLR_AUTOSAC
				| XPLR_BLANK
				| XPLR_BRIEF
				| XPLR_MIW
				| XPLR_HINT
				| XPLR_PROMPT
				| XPLR_TELNET_GA
				| XPLR_WRAP
				| XPLR_AUTOGOLD
				| XPLR_AUTOSPLIT
				| XPLR_SHOWTICK
				| XPLR_SHOWLEVEL
				| XPLR_COLOUR
				| XPLR_CUTCP );

	return;
}
