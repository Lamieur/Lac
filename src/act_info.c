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

int	format_list_to_char	args( ( char *buf1, int wielkosc_buf1,
					OBJ_DATA *list,	CHAR_DATA *ch,
					bool fShort, bool fShowNothing,
					bool krocej, bool inwentarz,
					bool fDontCombine, int ilosc,
					char *argument ) );
void	show_char_to_char_scan	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	przypadek_do_przyjecia	args( ( CHAR_DATA *ch, char *arg ) );
void	pokaz_wyjscia	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, bool fAuto ) );
void	patrz_w		args( ( CHAR_DATA *ch, OBJ_DATA *obj, char *argument ) );
char	*zmien_nawiasy	args( ( char *arg ) );
void	przytnij_linie	args( ( char *linia, int szer ) );
void	zapisz_odmiane	args( ( CHAR_DATA *ch ) );

/*
 * Lam 14.11.1999
 * Na wniosek RenTora, ktory chcial powrotu pomocy 'news'.
 * Niezalezne od help.are.
 */
KOMENDA( do_nowosci )
{
    STC( lac_nowosci, ch );

    return;
}


/*
 * Lam 5.2.2006: kolejnosc pokazywania ekwipunku w "ekwipunek".
 * Nowy system umozliwia pokazywanie nie wszystkich miejsc, niektorych miejsc
 * kilka razy, ale to efekty uboczne - chodzilo tylko o dowolne zmiany
 * kolejnosci.
 */
const int where_order[ ] =
{
	WEAR_HEAD,
	WEAR_FACE,
	WEAR_NECK_1,
	WEAR_NECK_2,
	WEAR_ARMS,
	WEAR_WRIST_L,
	WEAR_WRIST_R,
	WEAR_HANDS,
	WEAR_FINGER_L,
	WEAR_FINGER_R,
	WEAR_ABOUT,
	WEAR_BODY,
	WEAR_WAIST,
	WEAR_LEGS,
	WEAR_FEET,
	WEAR_SHIELD,
	WEAR_WIELD,
	WEAR_HOLD,
	WEAR_WIELD_2,
	WEAR_LIGHT,
	WEAR_NONE /* koniecznie zakonczyc WEAR_NONE */
};


/*
 * Nazwy poszczegolnych miejsc zakladania rzeczy, kolejnosc wedlug wartosci
 * WEAR_*
 */
const char *  const   where_name      [ ] =
{
    "{y<u`zywane jako `swiat`lo>{x   ",
    "{m<za`lo`zone na palec>{x      ",
    "{m<za`lo`zone na palec>{x      ",
    "{m<za`lo`zone na szyj`e>{x      ",
    "{m<za`lo`zone na szyj`e>{x      ",
    "{m<zarzucone na cia`lo>{x     ",
    "{m<za`lo`zone na g`low`e>{x      ",
    "{m<za`lo`zone na nogi>{x       ",
    "{m<za`lo`zone na stopy>{x      ",
    "{m<za`lo`zone na r`ece>{x       ",
    "{m<za`lo`zone na ramiona>{x    ",
    "{m<u`zywane jako tarcza>{x    ",
    "{m<owini`ete wok`o`l cia`la>{x   ",
    "{m<za`lo`zone na pas>{x        ",
    "{m<na`lo`zone na nadgarstek>{x ",
    "{m<na`lo`zone na nadgarstek>{x ",
    "{R<pierwsza bro`n>{x          ",
    "{G<trzymane w r`ekach>{x      ",
    "{R<druga bro`n>{x             ",
    "{m<za`lo`zone na twarz>{x      "
};


/*
 * Lam 2.5.98
 */
const char *urojone_przedmioty[ 32 ] =
{
	"(b`lyszczy) co`s fajnego i kolorowego",
	"co`s `zywego",
	"jaki`s wyj`atkowy przedmiot",
	"co`s zupe`lnie zadziwiaj`acego",
	"co`s nieprawdopodobnego",
	"co`s, co si`e rusza",
	"co`s, co na pewno chce ci`e zje`s`c",
	"(magia) co`s totalnie cudownego",
	"co`s bardzo `ladnego",
	"co`s przyjemnie pachn`acego",
	"rzecz, kt`ora ci ucieka",
	"(trucizna) co`s paskudnie `smierdz`acego",
	"co`s niespotykanie niespotykanego",
	"co`s wygl`adaj`acego smakowicie",
	"co`s, czego nie potrafisz okre`sli`c",
	"(brz`eczy) co`s czadowego",
	"(magia) (`swieci) co`s `swiec`acego na zielono",
	"co`s idealnie pi`eknego",
	"(trucizna) co`s wyj`atkowo obrzydliwego",
	"(niewidz.) co`s, czego nie wida`c",
	"co`s, czego musisz si`e natychmiast pozby`c",
	"co`s, czego sobie nie przypominasz",
	"co`s nie pasuj`acego do reszty",
	"co`s niegustownego",
	"co`s bardzo odpowiedniego",
	"(niezwyk`lo`s`c) co`s niezwykle niezwyk`lego",
	"co`s, co zaraz zniknie",
	"co`s, czego nie masz",
	"(brz`eczy) co`s co za chwil`e wybuchnie",
	"co`s, co uzdrowi ludzko`s`c",
	"co`s gwarantuj`acego wielk`a w`ladz`e",
	"co`s, czego nie ma"
};

/*
 * Lam 3.5.98
 */
const char *urojone_na_ziemi[ 32 ] =
{
	"Le`zy tu co`s fajnego i kolorowego.",
	"Jaki`s przedmiot usi`luje ci uciec.",
	"Na pod`lo`zu spoczywa jaki`s wyj`atkowy przedmiot.",
	"Dostrzegasz tu co`s zupe`lnie zadziwiaj`acego.",
	"Widzisz co`s nieprawdopodobnego!",
	"Zauwa`zasz tu co`s, co si`e rusza w obawie przed tob`a.",
	"Ten przedmiot na pewno chce ci`e zje`s`c!",
	"Na ziemi le`zy co`s totalnie cudownego.",
	"Kto`s pozostawi`l tu co`s bardzo `ladnego.",
	"Le`zy tu co`s przyjemnie pachn`acego.",
	"Spoczywa tu rzecz, kt`ora ci ucieka!",
	"Le`zy tu co`s paskudnie `smierdz`acego!",
	"Widzisz tu co`s niespotykanie niespotykanego.",
	"Dostrzegasz tu co`s wygl`adaj`acego smakowicie.",
	"Widzisz co`s, czego nie potrafisz okre`sli`c.",
	"Widzisz co`s czadowego.",
	"Widzisz co`s `swiec`acego na niebiesko.",
	"Widzisz tu idea`l pi`eknego przedmiotu.",
	"Uuu! Zauwa`zasz co`s wyj`atkowo obrzydliwego.",
	"Widzisz co`s, czego nie wida`c.",
	"Kto`s pozostawi`l tu co`s, co musisz natychmiast mie`c!",
	"Le`zy tu co`s niespodziewanie niespodziewanego.",
	"Na ziemi spoczywa co`s nie pasuj`acego do reszty.",
	"Dostrzegasz tu co`s niegustownego.",
	"Zauwa`zasz co`s bardzo odpowiedniego.",
	"Le`zy tu co`s niezwykle niezwyk`lego.",
	"Widzisz co`s, co zaraz zniknie.",
	"Widzisz biegaj`ac`a ma`l`a bia`l`a myszk`e.",
	"Le`zy tu co`s, co za chwil`e wybuchnie! Uciekaj!",
	"Widzisz co`s, co uzdrowi ludzko`s`c.",
	"Kto`s nieroztropnie pozostawi`l tu co`s gwarantuj`acego wielk`a w`ladz`e.",
	"Widzisz co`s, czego nie ma."
};

/*
 * Lam 3.5.98
 */
const char *urojone_moby[ 16 ] =
{
	"Widzisz strasznego potwora! Uciekaj!\n\r",
	"Stoi tutaj twoja mama.\n\r",
	"Stoi przed tob`a najpi`ekniejsza na `swiecie dziewczyna.\n\r",
	"Widzisz wielkiego smoka z trzema g`lowami.\n\r",
	"Jest tu wypatruj`aca kolejnej ofiary rozw`scieczona lamia.\n\r",
	"Widzisz samego siebie!\n\r",
	"Wielka bestia z z`ebami do ziemi usi`luje odgry`x`c ci g`low`e.\n\r",
	"Uzbrojony po z`eby rycerz w ci`e`zkiej zbroi atakuje ci`e!\n\r",
	"Jaki`s demon stoi tu i patrzy na ciebie podejrzliwie.\n\r",
	"Widzisz stra`znika miejskiego usi`luj`acego ci`e zabi`c!\n\r",
	"Widzisz stwora z innej p`laszczyzny astralnej.\n\r",
	"(bia`la aura) Stoi tu pot`e`zny b`og i ma zamiar ci`e unicestwi`c!\n\r",
	"(MORDERCA) Stoi przed tob`a przero`sni`eta ro`slina-morderca!\n\r",
	"Stoi tu jaki`s zupe`lnie zwyczajny cz`lowiek.\n\r",
	"Stoi tutaj dziwaczna bestia z czterema nogami i sze`scioma r`ekami.\n\r",
	"(oczarowanie) Stoi przed tob`a tw`oj wyznawca prosz`acy o pokut`e.\n\r"
};

/*
 * act_obj.c
 */
bool pijane_przedmioty( CHAR_DATA *ch );

/*
 * Local functions.
 */
char *  format_obj_to_char      args( ( OBJ_DATA *obj, CHAR_DATA *ch,
					bool fShort, bool krocej,
					bool inwentarz ) );
void    show_char_to_char_0     args( ( CHAR_DATA *victim, CHAR_DATA *ch,
					bool skanowanie, bool skrot ) );
void    show_char_to_char_1     args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char       args( ( CHAR_DATA *list, CHAR_DATA *ch,
					bool skrot ) );



char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch,
				bool fShort, bool krocej, bool inwentarz )
{
    static char buf [ MAX_STRING_LENGTH ];
    static char buf2[ MAX_STRING_LENGTH ];

    buf[ 0 ] = '\0';
    if ( IS_OBJ_STAT( obj, ITEM_QUEST ) )	strcat( buf, "(zadanie) " );
    if ( IS_OBJ_STAT( obj, ITEM_INVIS ) )	strcat( buf, "(niewidz.) " );
    if ( IS_OBJ_STAT( obj, ITEM_EVIL ) && HAS_DETECT_EVIL( ch ) )
						strcat( buf, "(czerwona aura) " );
    if ( IS_AFFECTED( ch, AFF_DETECT_MAGIC )
	&& IS_OBJ_STAT( obj, ITEM_MAGIC ) )	strcat( buf, "(magia) " );
    if ( IS_OBJ_STAT( obj, ITEM_GLOW ) )	strcat( buf, "(b`lyszczy) " );
    if ( IS_OBJ_STAT( obj, ITEM_HUM )
      && CAN_HEAR( ch ) )			strcat( buf, "(brz`eczy) " );
    if ( IS_OBJ_STAT( obj, ITEM_FLAME ) )	strcat( buf, "(gor`ace) " );
    if ( IS_OBJ_STAT( obj, ITEM_CHAOS ) )	strcat( buf, "(chaos) " );
    if ( IS_OBJ_STAT( obj, ITEM_ICY ) )		strcat( buf, "(zimne) " );
    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )	strcat( buf, "(trucizna) " );
    if ( obj->item_type == ITEM_LIGHT
      && obj->value[ 2 ].v != 0
      && ( obj->wear_loc == WEAR_LIGHT
	|| obj->in_room
	|| obj->value[ 1 ].v == 0 ) )
	strcat( buf, "(`swieci) " );

    if ( fShort )
    {
	if ( obj->short_descr )
	{
	    strcat( buf, obj->short_descr );
	    if ( CZY_OPCJA( ch, OPC_SHOWLEVEL ) && !krocej )
	    {
		sprintf( buf2, "  {y[%3d]{x", obj->level );
		strcat( buf, buf2 );
	    }
	    if ( ( IS_OBJ_STAT( obj, ITEM_INVENTORY )
		|| IS_OBJ_STAT( obj, ITEM_PLAYER_SHOP ) )
	      && ( inwentarz
		|| ch->level >= L_SEN ) ) /* tak tak, nie get_trust */
		strcat( buf, " {M(SKLEP){x" );
	}
    }
    else
    {
	if ( obj->in_room
	  && obj->in_room->sector_type == SECT_WATER_DEEP
	  && obj->item_type == ITEM_BOAT )
	{
	    sprintf( buf2, "Na wodzie unosi si`e %s.", obj->short_descr );
	    strcat( buf, buf2 );
	}
	else
	{
	    if ( obj->description && *obj->description )
		strcat( buf, obj->description );
	    else
	    {
		if ( IS_SET( obj->pIndexData->progtypes, O_LONG_DESCR_PROG ) )
		{
		    buf2[ 0 ] = '\0';
		    oprog_long_descr_trigger( obj, ch, buf2 );
		    while ( buf2[ 0 ] && isspace( (int) buf2[ strlen( buf2 ) - 1 ] ) )
			buf2[ strlen( buf2 ) - 1 ] = '\0';
		    if ( buf2[ 0 ] )
		    {
			strcat( buf, buf2 );
			strcat( buf, "{x" );
		    }
		}
		else
		{
		    sprintf( buf2, "Le`zy tu %s.", obj->short_descr );
		    strcat( buf, buf2 );
		}
	    }
	}
    }

    return buf;
}


/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
int format_list_to_char( char *buf1, int wielkosc_buf1, OBJ_DATA *list,
			CHAR_DATA *ch, bool fShort, bool fShowNothing,
			bool krocej, bool inwentarz, bool fDontCombine,
			int ilosc, char *argument )
{
    OBJ_DATA *obj;
    char buf[ MSL ];
    char buf2[ 200 ];
    char **prgpstrShow = NULL;
    char *pstrShow;
    int *prgnShow = NULL;
    int nShow;
    int iShow;
    int count;
    bool fCombine;
    bool przepelnienie = FALSE;

    if ( !ch->desc )
	return 0;

    if ( wielkosc_buf1 < MSL )
    {
	bug( "wielkosc_buf1 = %d < MSL", wielkosc_buf1 );
	return 0;
    }

    if ( krocej )
	strcat( buf1, "{oPrzedmioty: " );
    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	count++;
    }

    if ( count )
    {
	prgpstrShow = alloc_mem( count * sizeof( char * ) );
	prgnShow    = alloc_mem( count * sizeof( int ) );
    }
    nShow       = 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj; obj = obj->next_content )
    {
	if ( IS_OBJ_STAT( obj, ITEM_INVENTORY )
	  && ch->level < L_SEN )
	    continue;

	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) && !IS_OBJ_STAT( obj, ITEM_POM ) )
	{
	    /* Lam 27.1.2005: tylko pasujace */
	    if ( *argument && !is_name( argument, obj->name ) )
		continue;

	    pstrShow = format_obj_to_char( obj, ch, fShort, krocej, inwentarz );
	    fCombine = FALSE;

	    if ( !fDontCombine )
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		    if ( !strcmp( prgpstrShow[ iShow ], pstrShow ) )
		    {
			prgnShow[ iShow ]++;
			fCombine = TRUE;
			break;
		    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow[ nShow ] = str_dup( pstrShow );
		prgnShow[ nShow ] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if ( !przepelnienie )
	{
	    if ( prgnShow[ iShow ] != 1 )
	    {
		if ( !krocej )
		    sprintf( buf, "{R(%2d){x ", prgnShow[ iShow ] );
		else
		    sprintf( buf, "(%d) ", prgnShow[ iShow ] );
		strcat( buf1, buf );
	    }
	    else if ( !krocej )
		strcat( buf1, "     " );

	    if ( !fShort && !krocej )
		strcat( buf1, "{o" );

	    if ( IS_NPC( ch )
	      || ( ch->pcdata->condition[ COND_DRUNK ] < number_percent( ) + 10
		&& ( !IS_AFFECTED( ch, AFF_DEZORIENTACJA )
		  || number_percent( ) > 20 ) ) )
	    {
		strcat( buf1, prgpstrShow[ iShow ] );
	    }
	    else if ( fShort )
	    {
		strcat( buf1, urojone_przedmioty[ number_bits( 5 ) ] );
		if ( CZY_OPCJA( ch, OPC_SHOWLEVEL ) )
		{
		    sprintf( buf2, "  {y[%3d]", ch->level + number_bits( 6 ) + ch->pcdata->condition[ COND_DRUNK ] );
		    strcat( buf1, buf2 );
		}
	    }
	    else
		strcat( buf1, urojone_na_ziemi[ number_bits( 5 ) ] );

	    if ( !krocej )
		strcat( buf1, "{x\n\r" );
	    else if ( iShow != nShow - 1 )
		strcat( buf1, ", " );

	    /* Lam 19.11.2003 */
	    /* Vigud: dodaje (unsigned int), zeby uciszyc kompilator. Powyzej
	       jest sprawdzany warunek na wielkosc_buf1 < MSL, a skoro
	       MSL > MIL, to na pewno wielkosc_buf1 - MIL > 0. */
	    if ( strlen( buf1 ) > (unsigned int) ( wielkosc_buf1 - MIL ) )
	    {
		/* powinno sie jeszcze zmiescic chociaz to */
		strcat( buf1, "(wi`ecej nie da si`e wy`swietli`c)\n\r" );
		przepelnienie = TRUE;
	    }

	    /* Lam 27.1.2006: opcjonalne ograniczenie ilosci. Nie robie tego
	       powyzej, bo i tak trzeba zliczanie duplikatow przeprowadzic. */
	    if ( ilosc )
	    {
		ilosc--;
		if ( !ilosc )
		    przepelnienie = TRUE; /* nadaje sie idealnie */
	    }
	}
	free_string( prgpstrShow[ iShow ] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	strcat( buf1, "     " );
	if ( !IS_NPC( ch )
	  && ( ch->pcdata->condition[ COND_DRUNK ] > 10
	    || IS_AFFECTED( ch, AFF_DEZORIENTACJA ) ) )
	{
	    if ( fShort )
	    {
		strcat( buf1, urojone_przedmioty[ number_bits( 5 ) ] );
		if ( CZY_OPCJA( ch, OPC_SHOWLEVEL ) )
		{
		    sprintf( buf2, "  {y[%3d]{x", ch->level + number_bits( 6 ) + ch->pcdata->condition[ COND_DRUNK ] );
		    strcat( buf1, buf2 );
		}
		strcat( buf1, "\n\r" );
	    }
	    else
	    {
		/* Lam: to nie jest uzywane, ale jesli kiedys bedzie? */
		strcat( buf1, urojone_na_ziemi[ number_bits( 5 ) ] );
		strcat( buf1, "\n\r" );
	    }
	}
	else
	    strcat( buf1, "Nic.\n\r" );
    }

    /*
     * Clean up.
     */
    if ( count )
    {
	free_mem( prgpstrShow, count * sizeof( char * ) );
	free_mem( prgnShow,    count * sizeof( int )    );
    }

    if ( krocej )
	strcat( buf1, "{x\n\r" );

    return nShow;
}


/* przyklad uzycia format_list_to_char, dzieki temu mozna domalowywac taka
   liste do juz istniejacego i wstepnie wypelnionego czyms bufora */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort,
			bool fShowNothing, bool krocej, bool inwentarz,
			bool fDontCombine, int ilosc, char *argument )
{
    char buf[ MSL * 4 ];
    int nShow;

    buf[ 0 ] = '\0';
    nShow = format_list_to_char( buf, MSL * 4, list, ch, fShort, fShowNothing,
		krocej, inwentarz, fDontCombine, ilosc, argument );

    if ( nShow || fShowNothing )
	send_to_char( buf, ch );

    return;
}


void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch, bool skanowanie,
			  bool skrot )
{
    char buf[ MSL ];
    char samoimie[ MIL ]; /* imie moze miec 12 liter, tytul 60 - wejdzie */
    bool zamaskowany;

    strcpy( buf, skanowanie ? "  " : "{E" );

    if ( is_affected( victim, gsn_disguise )
      && ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
      && number_percent( ) < victim->pcdata->learned[ gsn_disguise ]
			     + 2 * ( victim->level - ch->level ) )
	zamaskowany = TRUE;
    else
	zamaskowany = FALSE;

    /* Lam: nowa boska niewidzialnosc */
    if ( !zamaskowany && !IS_NPC( victim ) && IS_SET( victim->act, PLR_WIZINVIS )
      && IS_HERO( ch )
      && victim->pcdata->wizinvis <= get_trust( ch ) )
	strcat( buf, "(BN) " );

    if ( skanowanie && !zamaskowany && !IS_NPC( victim ) )
	strcat ( buf, "(GRACZ) ");

    if ( IS_AWAKE( victim )
      && !IS_AFFECTED( victim, AFF_CHARM )
      && ( ( IS_NPC( victim ) && victim->pIndexData->pShop )
	|| ( IS_NPC( victim ) && victim->pIndexData->pHealer )
	|| ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_SHOPKEEPER ) ) ) )
	strcat( buf, "(sklep) " );

    if ( IS_AWAKE( victim )
      && !IS_AFFECTED( victim, AFF_CHARM )
      && !IS_MUTE( victim )
      && !IS_SET( victim->in_room->room_flags, ROOM_CONE_OF_SILENCE )
      && IS_NPC( victim )
      && victim->pIndexData->game_fun )
	strcat( buf, "(krupier) " );

    if ( IS_AFFECTED( victim, AFF_BRZECZY )
      && CAN_HEAR( ch ) )
	strcat( buf, "(brz`eczy) " );

    if ( IS_AFFECTED( victim, AFF_INVISIBLE ) )
	strcat( buf, "(niewidka) " );

    if ( IS_AFFECTED( victim, AFF_HIDE ) )
	strcat( buf, "(ukrycie) " );

    if ( IS_AFFECTED( victim, AFF_CHARM ) )
	strcat( buf, "(oczarowanie) " );

    if ( IS_AFFECTED( victim, AFF_SPOKOJ ) )
	strcat( buf, "(spok`oj) " );

    if ( IS_AFFECTED( victim, AFF_PASS_DOOR )
      || ( IS_SET( race_table[ victim->race ].race_abilities, RACE_PASSDOOR )
	&& ( victim->race == zr_martwiak
	  || victim->race == zr_nietoperz ) ) )
	strcat( buf, "(elastyczno`s`c) " );

    if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
	strcat( buf, "(r`o`zowa aura) " );

    if ( IS_EVIL( victim ) && HAS_DETECT_EVIL( ch ) )
	strcat( buf, "(czerwona aura) " );

    if ( HAS_SANCTUARY( victim ) )
	strcat( buf, "(bia`la aura) " );

    if ( IS_AFFECTED( victim, AFF_FLAMING ) )
	strcat( buf, "(p`lonie) " );

    if ( IS_AFFECTED( victim, AFF_LODOWA_TARCZA ) )
	strcat( buf, "(ch`l`od) " );

    if ( IS_AFFECTED( victim, AFF_BLACK_AURA ) )
	strcat( buf, "(czarna aura) " );

    if ( IS_AFFECTED( victim, AFF_GROZA ) )
	strcat( buf, "(groza) " );

    if ( IS_AFFECTED( victim, AFF_MROK ) )
	strcat( buf, "(mrok) " );

    if ( IS_AFFECTED( victim, AFF_TARCZA_UMARLYCH ) )
	strcat( buf, "(tarcza umar`lych) " );

    if ( IS_AFFECTED( victim, AFF_ANIOL_STROZ ) )
	strcat( buf, "(anio`l str`o`z) " );

    if ( IS_AFFECTED( victim, AFF_SWIETA_AURA ) )
	strcat( buf, "(`swi`eta aura) " );

    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	strcat( buf, "(MORDERCA) " );

    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_THIEF ) )
	strcat( buf, "(Z`LODZIEJ) " );

    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_ZABOJCA ) )
	strcat( buf, "(ZAB`OJCA) " );

    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_AFK ) )
	strcat( buf, "(OOK) " );

    if ( IS_SET( victim->info, INF_CLAN_GUARD ) )
    {
	if ( victim->sex == 2 )
	    strcat( buf, "({WStra`zniczka Porz`adku Klanowego{E) ");
	else
	    strcat( buf, "({WStra`znik Porz`adku Klanowego{E) ");
    }

    if ( skanowanie )
    {
	if ( !zamaskowany )
	    sprintf( samoimie, "%s", PERS( victim, ch, FALSE ) );
	else
	    sprintf( samoimie, "%s", victim->false_name );
	samoimie[ 0 ] = UPPER( samoimie[ 0 ] );
	if ( samoimie[ 0 ] == '`' ) samoimie[ 1 ] = UPPER( samoimie[ 1 ] );
	strcat( buf, samoimie );
	if ( !zamaskowany && !IS_NPC( victim ) && victim->pcdata->title )
	    strcat( buf, victim->pcdata->title );
	strcat( buf, "\n\r{x" );
	send_to_char( buf, ch );
	return;
    }

    /* Qwert: spiacego zamaskowanego nie mozna dojrzec, bo sie nie rusza */
    if ( ( victim->position == POS_STANDING || zamaskowany )
      && victim->long_descr[ 0 ] != '\0'
      /* Lam: oczarowane moby nie pelnia swoich funkcji */
      && !IS_AFFECTED( victim, AFF_CHARM )
      /* Lam: stacjonarne moby maja swoje opisy tylko w swoim pomieszczeniu */
      && ( !IS_NPC( victim )
	|| !IS_SET( victim->act, ACT_SENTINEL )
	|| victim->reset_room == victim->in_room ) )
    {
	strcat( buf, victim->long_descr );
	strcat( buf, "{x\n\r" );
	send_to_char( buf, ch );
	return;
    }

    if ( IS_NPC( victim )
      && victim->position == POS_STANDING
      && IS_SET( victim->pIndexData->progtypes, M_LONG_DESCR_PROG ) )
    {
	char buf2[ MSL ];

	buf2[ 0 ] = '\0';
	mprog_long_descr_trigger( victim, ch, buf2 );

	if ( buf2[ 0 ] )
	{
	    int z = strlen( buf2 ) - 1;
	    while ( isspace( (int) buf2[ z ] ) )
		buf2[ z-- ] = '\0';

	    strcat( buf, buf2 );
	    strcat( buf, "{x\n\r" );
	    send_to_char( buf, ch );
	    return;
	}
    }

    strcpy( samoimie, PERS( victim, ch, FALSE ) );
    if ( !IS_NPC( victim ) && !skrot )
	strcat( samoimie, victim->pcdata->title );

    switch ( victim->position )
    {
	case POS_DEAD:
	    strcat( buf, "Le`zy tu bez oznak `zycia " );
	    strcat( buf, samoimie );
	    strcat( buf, "{E.{x\n\r" );
	    break;
	case POS_MORTAL:
	    if ( victim->sex == 2 )
		strcat( buf, "Le`zy tu `smiertelnie ranna ");
	    else
		strcat( buf, "Le`zy tu `smiertelnie ranny ");
	    strcat( buf, samoimie );
	    strcat( buf, "{E.{x\n\r" );
	    break;
	case POS_INCAP:
	    strcat( buf, "Le`zy tu w niemocy " );
	    strcat( buf, samoimie );
	    strcat( buf, "{E.{x\n\r" );
	    break;
	case POS_STUNNED:
	    if ( victim->sex == 2 )
		strcat( buf, "Le`zy tu poharatana ");
	    else
		strcat( buf, "Le`zy tu poharatany ");
	    strcat( buf, samoimie );
	    strcat( buf, "{E.{x\n\r" );
	    break;
	case POS_SLEEPING:
	    strcat( buf, "`Spi tu sobie smacznie " );
	    strcat( buf, samoimie );
	    strcat( buf, "{E.{x\n\r" );
	    break;
	case POS_RESTING:
	    strcat( buf, "Wyleguje si`e tu " );
	    strcat( buf, samoimie );
	    strcat( buf, "{E.{x\n\r" );
	    break;
	case POS_STANDING:
	    strcat( buf, "Stoi tu " );
	    strcat( buf, samoimie );
	    strcat( buf, "{E.{x\n\r" );
	    break;
	case POS_FIGHTING:
	    samoimie[ 0 ] = UPPER( samoimie[ 0 ] );
	    if ( samoimie[ 0 ] == '`' ) samoimie[ 1 ] = UPPER( samoimie[ 1 ] );
	    strcat( buf, samoimie );
	    strcat( buf, "{E walczy tu " );
	    if ( !victim->fighting )
		strcat( buf, "z powietrzem?!{x\n\r" );
	    else if ( FIGHTING( victim ) == ch )
		strcat( buf, "z TOB`A!{x\n\r" );
	    else if ( victim->in_room == FIGHTING( victim )->in_room )
	    {
		if ( czy_ze( PERSN( FIGHTING( victim ), ch, FALSE ) ) )
		    strcat( buf, "ze " );
		else
		    strcat( buf, "z " );
		strcat( buf, PERSN( FIGHTING( victim ), ch, FALSE ) );
		strcat( buf, ".{x\n\r" );
	    }
	    else
		strcat( buf, "z jak`a`s wyimaginowan`a postaci`a?!{x\n\r" );
	    break;
    }

    send_to_char( buf, ch );

    return;
}


void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    char      buf[ MAX_STRING_LENGTH ];
    char      buf2[ MAX_INPUT_LENGTH ];
    int       iWhere, iWear;
    int       percent;
    bool      found;

    /* 17.2.2005: Lam, #106 */
    if ( victim == ch )
	STC( "Ogl`adasz si`e.\n\r", ch );
    else if ( can_see( victim, ch ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "$n patrzy na ciebie.", ch, NULL, victim, TO_VICT );
	/* ZM_WID_VICT zbedne, $n moze patrzec na "kogos" i to widac */
	act( ZM_WZROK | ZM_WID_CHAR, "$n patrzy na $#.",  ch, NULL, victim, TO_NOTVICT );
    }

    if ( !IS_NPC( victim )
      && victim->race == zr_wampir
      && victim->pcdata->wampiryzm.rasa != -1 )
/*	sprintf( buf2, "$N by`l$A %s, lecz teraz k`ly zdradzaj`a $C dusz`e wampira.\n\r",
		 race_table[ victim->pcdata->wampiryzm.rasa ].narzednik ); */
	sprintf( buf2, "$N, pierwotnie rasy %s, teraz ma charakterystyczne wampirze k`ly.\n\r",
		 race_table[ victim->pcdata->wampiryzm.rasa ].who_name );
    else
	sprintf( buf2, "$N jest rasy %s.\n\r",
		 race_table[ victim->race ].who_name );

    if ( IS_SET( victim->info, INF_CLAN_GUARD ) )
    {
	if ( victim->sex == 2 )
	    sprintf( buf, "{WStra`zniczka Porz`adku Klanowego{x. %s\n\r", buf2 );
	else
	    sprintf( buf, "{WStra`znik Porz`adku Klanowego{x. %s\n\r", buf2 );
    }
    else
	strcpy( buf, buf2 );
    act( ZM_ZAWSZE, buf, ch, NULL, victim, TO_CHAR );

    if ( victim->description[ 0 ] != '\0' )
    {
	send_to_char( przemiel_opis( ch, victim->description ), ch );
	mprog_descr_trigger( victim, ch );
    }
    else if ( !mprog_descr_trigger( victim, ch ) )
	act( ZM_ZAWSZE, "Nie mo`zesz powiedzie`c niczego na $C temat.", ch, NULL, victim, TO_CHAR );

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, "{x" );
    strcat( buf, PERS( victim, ch, FALSE ) );

	 if ( percent >= 100 ) strcat( buf, " cieszy si`e pe`lni`a zdrowia.\n\r" );
    else if ( percent >=  90 ) strcat( buf, " ma kilka dra`sni`e`c.\n\r" );
    else if ( percent >=  80 ) strcat( buf, " ma par`e szram.\n\r" );
    else if ( percent >=  70 ) strcat( buf, " ma kilka naci`e`c.\n\r" );
    else if ( percent >=  60 ) strcat( buf, " ma kilka ran.\n\r" );
    else if ( percent >=  50 ) strcat( buf, " ma wiele ran.\n\r" );
    else if ( percent >=  40 ) strcat( buf, " ma kilka paskudnych ran.\n\r" );
    else if ( percent >=  30 ) strcat( buf, " ma wiele paskudnych ran.\n\r" );
    else if ( percent >=  20 ) strcat( buf, " ledwo `zyje.\n\r" );
    else if ( percent >=  10 ) strcat( buf, " dogorywa.\n\r" );
    else		       strcat( buf, " UMIERA.\n\r" );

    buf[ 2 ] = UPPER( buf[ 2 ] );
    if ( buf[ 2 ] == '`' ) buf[ 3 ] = UPPER( buf[ 3 ] );
    send_to_char( buf, ch );

    found = FALSE;
    for ( iWhere = 0; where_order[ iWhere ] != WEAR_NONE; iWhere++ )
    {
	iWear = where_order[ iWhere ];
	if ( ( obj = get_eq_char( victim, iWear ) )
	    && can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( ZM_ZAWSZE, "$Z u`zywa:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char( where_name[ iWear ], ch );
	    if ( IS_AFFECTED( victim, AFF_ILUZJA )
	      && ( !IS_SET( ch->act, PLR_HOLYLIGHT ) )
	      && ( !class_table[ ch->klasa ].fMana
		|| !saves_spell( victim->level - get_curr_wis( ch ) + 22, ch ) ) )
	    {
		send_to_char( urojone_przedmioty[ number_bits( 5 ) ], ch );
		if ( CZY_OPCJA( ch, OPC_SHOWLEVEL ) )
		    ch_printf( ch, "  {y[%3d]{x", number_bits( 6 ) + obj->level );
	    }
	    else
		send_to_char( format_obj_to_char( obj, ch, TRUE, FALSE, FALSE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    if ( victim != ch
      && !IS_NPC( ch )
      && ( number_percent( ) <= ch->pcdata->learned[ gsn_peek ]
	|| ( IS_AFFECTED( victim, AFF_CHARM )
	  && victim->master == ch ) )
      && ( !IS_AFFECTED( victim, AFF_ILUZJA )
	|| ( IS_SET( ch->act, PLR_HOLYLIGHT ) )
	|| ( class_table[ ch->klasa ].fMana
	  && saves_spell( victim->level - get_curr_wis( ch ) + 22, ch ) ) ) )
    {
	send_to_char( "\n\rZagl`adaj`ac w inwentarz widzisz:\n\r", ch );
	show_list_to_char( victim->carrying, ch, TRUE, TRUE, FALSE, FALSE, FALSE, 0, "" );

	if ( !IS_AFFECTED( victim, AFF_CHARM )
	  || victim->master != ch )
	    uzyj( ch, gsn_peek, "zagl`adaniu w kieszenie" );
    }

    return;
}


void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch, bool skrot )
{
    CHAR_DATA *rch;
    bool widzi = FALSE;

    if ( !list || PP( ch ) )
	return;

    for ( rch = list; rch; rch = rch->next_in_room )
    {
	if ( rch->deleted || rch == ch )
	    continue;

	if ( !IS_NPC( rch )
	  && IS_SET( rch->act, PLR_WIZINVIS )
	  && get_trust( ch ) < rch->pcdata->wizinvis )
	    continue;
	if ( IS_NPC( rch ) && rch->pIndexData->vnum == MOB_VNUM_SUPERMOB )
	    continue; /* supermoba nie widac */

	/* okno i podgladanie pozwalaja zagladac do innych pomieszczen, ale
	   podgladanie ma wlasne zabezpieczenia przed oslepieniem i ciemnica */
	if ( ch->in_room == rch->in_room )
	    widzi = can_see( ch, rch );
	else
	    widzi = can_see_dla_okna( ch, rch );

	if ( widzi )
	{
	    if ( IS_NPC( ch )
	      || ( ch->pcdata->condition[ COND_DRUNK ] < number_percent( ) + 20
	       && ( !IS_AFFECTED( ch, AFF_DEZORIENTACJA )
		 || number_percent( ) > 20 ) ) )
		show_char_to_char_0( rch, ch, FALSE, skrot );
	    else
		ch_printf( ch, "{E%s{x", urojone_moby[ number_bits( 4 ) ] );
	}
	else if ( room_is_dark( ch->in_room )
	  /* Lam 4.12.2004, Muzgus zauwazyl, ze czerwone oczy patrza na niego
	     przez sen: */
	  && IS_AWAKE( rch )
	  && HAS_INFRAVISION( rch )
	  && ( IS_NPC( rch ) || !IS_SET( rch->act, PLR_WIZINVIS ) ) )
	{
	    send_to_char( "Widzisz wlepion`a w ciebie par`e czerwonych oczu!\n\r", ch );
	}
    }

    return;
}


void show_char_to_char_scan( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    if ( !list )
	return;

    for ( rch = list; rch; rch = rch->next_in_room )
    {
	if ( rch->deleted || rch == ch )
	    continue;

	if ( !IS_NPC( rch )
	  && IS_SET( rch->act, PLR_WIZINVIS )
	  && get_trust( ch ) < rch->pcdata->wizinvis )
	    continue;

	if ( can_see( ch, rch ) )
	{
	    if ( !IS_NPC( ch )
	      && ( number_percent( ) < ch->pcdata->condition[ COND_DRUNK ] * 3
		|| ( IS_AFFECTED( ch, AFF_DEZORIENTACJA )
		  && number_percent( ) < 30 ) ) )
	    /* bardzo male szanse dla pijakow na widzenie na odleglosc */
		send_to_char( "  Jaka`s zamglona posta`c.\n\r", ch );
	    else
		show_char_to_char_0( rch, ch, TRUE, FALSE );
	}
    }

    return;
}


bool check_blind( CHAR_DATA *ch )
{
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( IS_AFFECTED( ch, AFF_BLIND )
      || ( !IS_NPC( ch ) && IS_SET( ch->pcdata->temp_flags, TEMPFLAG_BLIND ) ) )
    {
	send_to_char( "Niczego nie widzisz!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}


void pokaz_wyjscia( CHAR_DATA *ch, ROOM_INDEX_DATA *room, bool fAuto )
{
    EXIT_DATA *pexit;
    char buf[ MAX_STRING_LENGTH ];
    int door, d;
    bool found;

    buf[ 0 ] = '\0';

    if ( WP( ch ) && !check_blind( ch ) )
	return;

    strcpy( buf, fAuto ? "{e[Wyj`scia:" : "{GWidoczne wyj`scia:{x\n\r" );

    found = FALSE;
    for ( d = 0; d < MAX_DIR; d++ )
    {
	door = dir_order[ d ];
	if ( ( pexit = room->exit[ door ] )
	    && pexit->to_room
	    && !IS_SET( pexit->exit_info, EX_BURIED ) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
		    strcat( buf, " {g(" );
		    strcat( buf, kierunki[ door ].krotki );
		    strcat( buf, "){e" );
		}
		else
		{
		    strcat( buf, " " );
		    strcat( buf, kierunki[ door ].krotki );
		}
	    }
	    else
	    {
		sprintf( buf + strlen( buf ), "{RNa %-5s -{x %s.\n\r",
		    kierunki[ door ].biernik,
		    IS_SET( pexit->exit_info, EX_CLOSED )
		    ? "zamkni`ete"
		    : !can_see_room( ch, pexit->to_room )
		      ? "jest tam zbyt ciemno"
		      : pexit->to_room->name );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " `zadne" : "`Zadne.\n\r" );

    if ( fAuto )
	strcat( buf, "]{x\n\r" );

    send_to_char( buf, ch );

    return;
}


void pokaz_pomieszczenie( CHAR_DATA *ch, ROOM_INDEX_DATA *pom, bool skrot )
{
    char bufor[ MSL * 2 ];
    char bufek[ MSL * 2 + MIL ];

    strcpy( bufek, "{Q" );

    /* Polmrok jest w oswietlonym pomieszczeniu, do ktorego bez latarni
       nie dociera swiatlo. Strefa cienia robi mrok, jesli nie ma latarni,
       ale nie powoduje polmroku przy latarni, bo "wychodzi" z gracza
       oficjalnie (nie chcialo mi sie petli pisac).
       Polmrok to tez cien, ROOM_DARK dla pomieszczen bez ROOM_INDOORS. */
    if ( !room_is_dark( pom )
      && ( ( pom->sector_type != SECT_INSIDE
	  && pom->sector_type != SECT_CITY
	  && !IS_SET( pom->room_flags, ROOM_INDOORS )
	  && ( pom->area->weather_info.sunlight == SUN_DARK
	    || pom->area->weather_info.sunlight == SUN_SET ) )
	|| IS_SET( pom->room_flags, ROOM_DARK ) ) )
	strcat( bufek, "(p`o`lmrok) " );
    if ( room_is_dark( pom ) )
	strcat( bufek, "(mrok) " );
    if ( IS_SET( pom->room_flags, ROOM_CONE_OF_SILENCE ) )
	strcat( bufek, "(cisza) " );
    if ( room_is_private( pom ) )
	strcat( bufek, "(t`lok) " );
    if ( room_is_affected( pom, gsn_rozpalanie_ognisk ) )
	strcat( bufek, "(ciep`lo) " );
    else if ( IS_SET( pom->room_flags, ROOM_CLAN_HP ) )
	strcat( bufek, "(przytulno`s`c) " );
    if ( IS_SET( pom->room_flags, ROOM_CLAN_MANA )
      && ( IS_NPC( ch )
	|| class_table[ ch->klasa ].fMana
	|| IS_AFFECTED( ch, AFF_DETECT_MAGIC ) ) )
	strcat( bufek, "(splot many) " );

    /* 'patrz' / 'patrz auto' */
    sprintf( bufor, "%s{x\n\r", pom->name );
    bufor[ 0 ] = UPPER( bufor[ 0 ] );

    if ( bufor[ 0 ] == '`' )
	bufor[ 1 ] = UPPER( bufor[ 1 ] );

    strcat( bufek, bufor );
    send_to_char( bufek, ch );

    if ( CZY_OPCJA( ch, OPC_AUTOEXIT ) )
	pokaz_wyjscia( ch, pom, TRUE );

    if ( !skrot )
    {
	STC( przemiel_opis( ch, pom->description ), ch );
	rprog_descr_trigger( ch, pom, NULL );
	STC( "{x", ch );
    }

    /* skoro jest (cisza), moze to usunac? */
    if ( IS_SET( pom->room_flags, ROOM_CONE_OF_SILENCE ) )
	send_to_char( "Jest tu strasznie cicho.\n\r", ch );

    if ( room_is_affected( pom, gsn_rozpalanie_ognisk ) )
	send_to_char( "{ROgnisko rozgrzewa ci`e swym `zarem.{x\n\r", ch );

    if ( skrot )
    {
	show_list_to_char( pom->contents, ch, TRUE, FALSE, TRUE, FALSE, FALSE, 0, "" );
	show_char_to_char( pom->people, ch, TRUE );
    }
    else
    {
	show_list_to_char( pom->contents, ch, FALSE, FALSE, FALSE, FALSE, FALSE, 0, "" );
	show_char_to_char( pom->people, ch, FALSE );
    }

    return;
}


void patrz_w( CHAR_DATA *ch, OBJ_DATA *obj, char *argument )
{
    char buf[ MAX_STRING_LENGTH * 2 ];
    int ilosc;
    char nazwa[ MIL ];
    bool pojedynczo = FALSE;
    int masa;

    switch ( obj->item_type )
    {
	default:
	    send_to_char( "To nie pojemnik.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[ 1 ].v <= 0 )
	    {
		send_to_char( "Pusto!\n\r", ch );
		break;
	    }

	    sprintf( buf, "%s %s wype`lnion%s %s p`lynem.\n\r",
		obj->rodzaj == 3 ? "S`a" : "Jest",
		obj->value[ 1 ].v <     obj->value[ 0 ].v / 4
		    ? "mniej ni`z do po`lowy" :
		obj->value[ 1 ].v < 3 * obj->value[ 0 ].v / 4
		    ? "do po`lowy" :
		obj->value[ 1 ].v == obj->value[ 0 ].v
		    ? "ca`lkowicie" : "wi`ecej ni`z do po`lowy",
		obj->rodzaj == 2 ? "a" : obj->rodzaj == 1 ? "y" : "e",
		liq_table_pl[ obj->value[ 2 ].v ].liq_color );

	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	    if ( IS_SET( obj->value[ 1 ].v, CONT_CLOSED ) )
	    {
		send_to_char( "To jest zamkni`ete.\n\r", ch );
		break;
	    }

	    /* CH w przypadku wypelnienia pojemnika przedmiotami, ktorych nie
	       widzi, powinien otrzymac komunikat, ze pojemnik jest pusty */
	    masa = get_obj_weight_vis_only( ch, obj ) - obj->weight;
	    if ( masa <= 0 )
	    {
		send_to_char( "Ten pojemnik jest pusty.\n\r", ch );
		break;
	    }
	    else
	    {
		argument = one_argument( argument, nazwa );
		if ( *nazwa && !str_prefix( nazwa, "pojedynczo" ) )
		{
		    pojedynczo = TRUE;
		    argument = one_argument( argument, nazwa );
		}
		if ( is_number( nazwa ) )
		{
		    ilosc = UMAX( 0, atoi( nazwa ) );
		    one_argument( argument, nazwa );
		}
		else
		    ilosc = 0;

		act( ZM_ZAWSZE, "$p zawiera$T:", ch, obj, obj->rodzaj == 3 ? "j`a" : "", TO_CHAR );
		show_list_to_char( obj->contains, ch, TRUE, TRUE, FALSE, FALSE, pojedynczo, ilosc, nazwa );
		sprintf( buf, "Ten pojemnik jest wype`lniony %s.\n\r",
		    masa < 2 * obj->value[ 0 ].v / 5
			? "mniej ni`z do po`lowy" :
		    masa <= 3 * obj->value[ 0 ].v / 5
			? "do po`lowy" :
		    masa == obj->value[ 0 ].v
			? "w ca`lo`sci" : "wi`ecej ni`z do po`lowy" );
		send_to_char( buf, ch );
	    }
	    break;

	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET( obj->value[ 1 ].v, CONT_CLOSED ) )
	    {
		send_to_char( "To jest zamkni`ete.\n\r", ch );
		break;
	    }

	    argument = one_argument( argument, nazwa );
	    if ( *nazwa && !str_prefix( nazwa, "pojedynczo" ) )
	    {
		pojedynczo = TRUE;
		argument = one_argument( argument, nazwa );
	    }
	    if ( is_number( nazwa ) )
	    {
		ilosc = UMAX( 0, atoi( nazwa ) );
		one_argument( argument, nazwa );
	    }
	    else
		ilosc = 0;

	    act( ZM_ZAWSZE, "$p zawiera$T:", ch, obj, obj->rodzaj == 3 ? "j`a" : "", TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE, FALSE, FALSE, pojedynczo, ilosc, nazwa );
	    break;
    }

    return;
}


KOMENDA( do_look )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    char      *pdesc;
    int        door = -1;
    char       buf[ MAX_STRING_LENGTH ];
    static char     const  y_a_e   [ ] = { 'e', 'y', 'a', 'e' };

    if ( !IS_NPC( ch ) && !ch->desc )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "Nie widzisz niczego pr`ocz gwiazd!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "Niczego nie widzisz, przecie`z `spisz! Chrrr.\n\r", ch );
	return;
    }

    if ( WP( ch ) && !check_blind( ch ) )
	return;

    if ( ( IS_NPC( ch )
	|| !IS_SET( ch->act, PLR_HOLYLIGHT ) )
      && !HAS_INFRAVISION( ch )
      && room_is_dark( ch->in_room ) )
    {
	send_to_char( "Ale`z tu ciemno...\n\r", ch );
	show_char_to_char( ch->in_room->people, ch, FALSE );
	return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	pokaz_pomieszczenie( ch, ch->in_room,
		arg1[ 0 ] != '\0' && CZY_OPCJA( ch, OPC_BRIEF ) );
	rprog_look_trigger( ch );
	return;
    }


    if ( PP( ch ) )
    {
	STC( "Nie ma tu niczego materialnego, na co mo`zesz patrze`c.\n\r", ch );
	return;
    }

	 if ( !str_cmp( arg1, "n"  ) ) door = 0;
    else if ( !str_cmp( arg1, "e"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s"  ) ) door = 2;
    else if ( !str_cmp( arg1, "w"  )
	   && arg2[ 0 ] == '\0'    )   door = 3;
    else if ( !str_cmp( arg1, "g"  ) ) door = 4;
    else if ( !str_cmp( arg1, "d"  ) ) door = 5;
    else if ( !str_cmp( arg1, "ne" ) ) door = 6;
    else if ( !str_cmp( arg1, "se" ) ) door = 7;
    else if ( !str_cmp( arg1, "nw" ) ) door = 8;
    else if ( !str_cmp( arg1, "sw" ) ) door = 9;


    /* 'patrz <kierunek>' */
    /* Lam 13.4.98 - zakopanych wyjsc nie widac */
    /* Lam 23.3.2005: zmiana zachowania - jesli nawet nie ma wyjscia, i tak
       "probuje" spojrzec w tym kierunku. Tym sposobem zachowanie jest
       konsekwentne - p d zawsze patrzy w dol, a nie raz w dol, raz na przedmiot
       o nazwie zaczynajacej sie na litere "d". */
    if ( door >= 0 )
    {
	if ( !( pexit = ch->in_room->exit[ door ] )
	  || IS_SET( pexit->exit_info, EX_BURIED ) )
	{
	    send_to_char( "Niczego tam nie ma.\n\r", ch );
	    return;
	}

	if ( pexit->to_room && !can_see_room( ch, pexit->to_room ) )
	    send_to_char( "Nie widzisz tam niczego.\n\r", ch );
	else if ( pexit->description && pexit->description[ 0 ] != '\0' )
	{
	    send_to_char( pexit->description, ch );
	    send_to_char( "\n\r", ch );
	}
	else
	    send_to_char( "Nic specjalnego.\n\r", ch );

	if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
	{
	    char *stan;
	    /* otwarte lub wywazone - musi je oswietlac jedno z pomieszczen */
	    if ( !can_see_room( ch, ch->in_room )
	      && ( !pexit->to_room
		|| !can_see_room( ch, pexit->to_room ) ) )
		return;

	    if ( IS_SET( pexit->exit_info, EX_BASHED ) )
		stan = "Wywa`zon";
	    else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
	    {
		/* zamkniete drzwi - nie oswietla ich sasiednie pomieszczenie
		   tylko to, w ktorym stoi postac */
		if ( !can_see_room( ch, ch->in_room ) )
		    return;
		stan = "Zamkni`et";
	    }
	    else
		stan = "Otwart";

	    sprintf( buf, "%s%c $D.", stan, y_a_e[ pexit->rodzaj ] );

	    act( ZM_ZAWSZE | ZM_DYLEMAT, buf, ch, NULL, pexit->short_descr, TO_CHAR );
	}

	return;
    }

    if ( !str_cmp( arg1, "w" ) )
    {
	/* Ulryk: tutaj jest arg2[ 0 ] != '\0', bo sprawdzalismy to juz wyzej */
	if ( door >= 0 )
	{
	    send_to_char( "W co spojrze`c?\n\r", ch );
	    return;
	}

	if ( pijane_przedmioty( ch ) )
	    return;

	if ( !( obj = get_obj_here( ch, arg2 ) ) )
	{
	    send_to_char( "Nie widzisz tu tego.\n\r", ch );
	    return;
	}

	patrz_w( ch, obj, "" );

	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    if ( pijane_przedmioty( ch ) )
	return;

    if ( ( obj = get_obj_here( ch, arg1 ) ) )
    {
	char buf[ MAX_INPUT_LENGTH ];

	number_argument( arg1, buf );

	if ( *obj->look_descr )
	{
	    send_to_char( przemiel_opis( ch, obj->look_descr ), ch );
	    oprog_descr_trigger( obj, ch );
	    return;
	}
	else if ( IS_SET( obj->pIndexData->progtypes, O_DESCR_PROG ) )
	{
	    if ( oprog_descr_trigger( obj, ch ) )
		return;
	}

	/* Lam 13.12.2001: nie pokazywac obj->description */
	if ( obj->carried_by == ch )
	{
	    act( ZM_ZAWSZE, "Nie widzisz w $l niczego szczeg`olnego.", ch, obj, NULL, TO_CHAR );
	    return;
	}

	if ( obj->in_room == ch->in_room )
	{
	    if ( obj->description && *obj->description )
	    {
		send_to_char( obj->description, ch );
		send_to_char( "\n\r", ch );
	    }
	    else if ( IS_SET( obj->pIndexData->progtypes, O_LONG_DESCR_PROG ) )
	    {
		buf[ 0 ] = '\0';
		oprog_long_descr_trigger( obj, ch, buf );
		if ( buf[ 0 ] )
		{
		    int z = strlen( buf ) - 1;

		    while ( isspace( (int) buf[ z ] ) )
			buf[ z-- ] = '\0';

		    strcat( buf, "{x\n\r" );
		    send_to_char( buf, ch );
		}
		else
		    act( ZM_ZAWSZE, "Nie widzisz w $l niczego szczeg`olnego.", ch, obj, NULL, TO_CHAR );
	    }
	    else
		act( ZM_ZAWSZE, "Nie widzisz w $l niczego szczeg`olnego.", ch, obj, NULL, TO_CHAR );

	    return;
	}

	bug( "do_look(): Patrzenie na przedmiot, ktorego nie ma.", 0 );
	return;
    }

    pdesc = get_extra_descr( arg1, ch->in_room->extra_descr );
    if ( pdesc )
    {
	send_to_char( przemiel_opis( ch, pdesc ), ch );
	return;
    }

    /* Alandar 06.12.2004: descr_prog do dodatkowych opisow */
    if ( rprog_descr_trigger( ch, ch->in_room, arg1 ) )
	return;

    send_to_char( "Nie widzisz tu tego.\n\r", ch );

    return;
}


KOMENDA( do_examine )
{
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH  ], arg2[ MAX_INPUT_LENGTH ];
    int isname, count = 1, number;

    number = number_argument( argument, arg2 );
    argument = one_argument( arg2, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Co obejrze`c?\n\r", ch );
	return;
    }

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "Nie widzisz niczego pr`ocz gwiazd!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "Niczego nie widzisz, przecie`z `spisz! Chrrr.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( pijane_przedmioty( ch ) )
	return;

    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj ) )
	{
	    if ( ( isname = is_name( arg, obj->name ) ) && count == number )
	    {
		if ( *obj->look_descr )
		    send_to_char( obj->look_descr, ch );
		else
		{
		    send_to_char( obj->description, ch );
		    send_to_char( "\n\r", ch );
		}
		patrz_w( ch, obj, argument );
		return;
	    }

	    if ( isname )
		count++;
	}

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( can_see_obj( ch, obj ) )
	{
	    if ( ( isname = is_name( arg, obj->name ) ) && count == number )
	    {
		if ( *obj->look_descr )
		    send_to_char( obj->look_descr, ch );
		patrz_w( ch, obj, argument );
		return;
	    }

	    if ( isname )
		count++;
	}

    if ( ( victim = get_char_room( ch, arg ) ) )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    STC( "Nie widzisz tu niczego takiego.\n\r", ch );

    return;
}


/*
 * Thanks to Zrin for auto-exit part.
 */
KOMENDA( do_exits )
{
    pokaz_wyjscia( ch, ch->in_room, !str_cmp( argument, "auto" ) );

    return;
}


KOMENDA( do_scan )
{
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    int door, d;
    bool dlugi_skan;
    bool zamkniete;
    bool sa_drzwi = FALSE;
    char bufor[ MIL ];
    static char     const  y_a_e   [ ] = { 'e', 'y', 'a', 'e' };

    if ( IS_NPC( ch ) && !ch->desc )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "Widzisz tylko gwiazdy!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	ASTC( "Niczego nie widzisz, przeciez `spisz, zapomnia`l$o`s?", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    dlugi_skan = ( IS_NPC( ch ) || number_percent( ) <= ch->pcdata->learned[ gsn_longscan ] );

    for ( d = 0; d < MAX_DIR; d++ )
    {
	door = dir_order[ d ];
	/* 'patrz <kierunek>', jesli wyjscie zakopane, udaj, ze go nie ma */
	if ( ( pexit = ch->in_room->exit[ door ] )
	  && !IS_SET( pexit->exit_info, EX_BURIED ) )
	{
	    sa_drzwi = TRUE;
	    switch ( door )
	    {
		case 0: send_to_char( "{MNa p`o`lnoc:{x ", ch ); break;
		case 1: send_to_char( "{MNa wsch`od:{x ", ch ); break;
		case 2: send_to_char( "{MNa po`ludnie:{x ", ch ); break;
		case 3: send_to_char( "{MNa zach`od:{x ", ch ); break;
		case 4: send_to_char( "{MW g`or`e:{x ", ch ); break;
		case 5: send_to_char( "{MW d`o`l:{x ", ch ); break;
		case 6: send_to_char( "{MNa pn.-wsch`od:{x ", ch ); break;
		case 7: send_to_char( "{MNa pd.-wsch`od:{x ", ch ); break;
		case 8: send_to_char( "{MNa pn.-zach`od:{x ", ch ); break;
		case 9: send_to_char( "{MNa pd.-zach`od:{x ", ch ); break;
	    }

	    if ( pexit->to_room
	      && room_is_dark( pexit->to_room )
	      && ( IS_NPC( ch )
		|| !IS_SET( ch->act, PLR_HOLYLIGHT ) )
	      && ( !room_is_dark( ch->in_room )
		|| !HAS_INFRAVISION( ch ) ) )
	    {
		strcpy( bufor, "Niczego nie mo`zesz dojrze`c." );
	    }
	    else if ( pexit->description && pexit->description[ 0 ] != '\0' )
		strcpy( bufor, pexit->description );
	    else if ( pexit->to_room )
		strcpy( bufor, pexit->to_room->name );
	    else
		strcpy( bufor, "Nic ciekawego." );

	    strcat( bufor, "\n\r" );
	    STC( bufor, ch );
	    bufor[ 0 ] = '\0';

	    zamkniete = ( IS_SET( pexit->exit_info, EX_CLOSED ) );

	    if ( pexit->keyword
	      && pexit->keyword[ 0 ] != '\0'
	      && pexit->keyword[ 0 ] != ' ' )
	    {
		if ( IS_SET( pexit->exit_info, EX_BASHED ) )
		    sprintf( bufor, "  {r(Wywa`zon%c z framug $D.){x", y_a_e[ pexit->rodzaj ] );
		else if ( zamkniete )
		    sprintf( bufor, "  {rZamkni`et%c $D.{x", y_a_e[ pexit->rodzaj ] );
		else if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
		    sprintf( bufor, "  {r(Otwart%c $D.){x", y_a_e[ pexit->rodzaj ] );
	    }
	    else
	    {
		if ( IS_SET( pexit->exit_info, EX_BASHED ) )
		    strcpy( bufor, "  {r(Drzwi wywa`zone z framug.){x" );
		else if ( zamkniete )
		    strcpy( bufor, "  {rPrzej`scie jest zamkni`ete.{x" );
		else if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
		    strcpy( bufor, "  {r(Otwarte.){x" );
	    }

	    if ( bufor[ 0 ] != '\0' )
		act( ZM_ZAWSZE | ZM_DYLEMAT, bufor, ch, NULL, pexit->short_descr, TO_CHAR );

	    if ( !zamkniete
	      && pexit->to_room
	      && !IS_SET( pexit->to_room->room_flags, ROOM_FOG ) )
	    {
		show_char_to_char_scan( pexit->to_room->people, ch );
	    }

	    to_room = pexit->to_room;

	    /* Lam: sokoli wzrok */
	    if ( dlugi_skan
	      && !zamkniete
	      && to_room
	      && !IS_SET( to_room->room_flags, ROOM_FOG )
	      && ( pexit = to_room->exit[ door ] )
	      && !IS_SET( pexit->exit_info, EX_BURIED ) )
	    {
		switch ( door )
		{
		    case 0: send_to_char( "{mDalej na p`o`lnoc:{x ", ch ); break;
		    case 1: send_to_char( "{mDalej na wsch`od:{x ", ch ); break;
		    case 2: send_to_char( "{mDalej na po`ludnie:{x ", ch ); break;
		    case 3: send_to_char( "{mDalej na zach`od:{x ", ch ); break;
		    case 4: send_to_char( "{mDalej w g`or`e:{x ", ch ); break;
		    case 5: send_to_char( "{mDalej w d`o`l:{x ", ch ); break;
		    case 6: send_to_char( "{mDalej na pn.-wsch`od:{x ", ch ); break;
		    case 7: send_to_char( "{mDalej na pd.-wsch`od:{x ", ch ); break;
		    case 8: send_to_char( "{mDalej na pn.-zach`od:{x ", ch ); break;
		    case 9: send_to_char( "{mDalej na pd.-zach`od:{x ", ch ); break;
		}

		if ( pexit->to_room
		  && room_is_dark( pexit->to_room )
		  && ( IS_NPC( ch )
		    || !IS_SET( ch->act, PLR_HOLYLIGHT ) )
		  && ( !room_is_dark( ch->in_room )
		    || !HAS_INFRAVISION( ch ) ) )
		{
		    strcpy( bufor, "Niczego nie mo`zesz dojrze`c." );
		}
		else if ( pexit->description && pexit->description[ 0 ] != '\0' )
		    strcpy( bufor, pexit->description );
		else if ( pexit->to_room )
		    strcpy( bufor, pexit->to_room->name );
		else
		    strcpy( bufor, "Nic ciekawego." );

		strcat( bufor, "\n\r" );
		STC( bufor, ch );
		bufor[ 0 ] = '\0';

		/* widocznosc drzwi z pomieszczenia odleglego o 1 do pomieszczenia
		   odleglego od 2 zalezy od widocznosci pomieszczenia odl. o 1 */
		if ( !room_is_dark( to_room )
		  || ( !IS_NPC( ch )
		    && IS_SET( ch->act, PLR_HOLYLIGHT ) )
		  || ( room_is_dark( ch->in_room )
		    && HAS_INFRAVISION( ch ) ) )
		{
		    if ( pexit->keyword
		      && pexit->keyword[ 0 ] != '\0'
		      && pexit->keyword[ 0 ] != ' ' )
		    {
			if ( IS_SET( pexit->exit_info, EX_BASHED ) )
			    sprintf( bufor, "  {r(Wywa`zon%c z framug $D.){x", y_a_e[ pexit->rodzaj ] );
			else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
			    sprintf( bufor, "  {rZamkni`et%c $D.{x", y_a_e[ pexit->rodzaj ] );
			else if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
			    sprintf( bufor, "  {r(Otwart%c $D.){x", y_a_e[ pexit->rodzaj ] );
		    }
		    else
		    {
			if ( IS_SET( pexit->exit_info, EX_BASHED ) )
			    strcpy( bufor, "  {r(Drzwi wywa`zone z framug.){x" );
			else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
			    strcpy( bufor, "  {rPrzej`scie jest zamkni`ete.{x" );
			else if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
			    strcpy( bufor, "  {r(Otwarte.){x" );
		    }

		    if ( bufor[ 0 ] != '\0' )
			act( ZM_ZAWSZE | ZM_DYLEMAT, bufor, ch, NULL, pexit->short_descr, TO_CHAR );
		}

		if ( !IS_SET( pexit->exit_info, EX_CLOSED ) && pexit->to_room )
		    show_char_to_char_scan( pexit->to_room->people, ch );
	    }
	}
    }

    if ( dlugi_skan )
	uzyj( ch, gsn_longscan, "dalekowzroczno`sci" );

    /* Qwert: rozgladanie na BN niewidoczne */
    if ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) )
	act( ZM_WZROK | ZM_WID_CHAR, "$n rozgl`ada si`e we wszystkich kierunkach.", ch, NULL, NULL, TO_ROOM );

    if ( !sa_drzwi )
	send_to_char( "To pomieszczenie nie posiada wyj`s`c!\n\r", ch );

    return;
}


KOMENDA( do_punktacja )
{
    char         buf [ MSL ];
    char         buf1[ MSL ];
    char	 temp[ MSL ];
    int		 pzl;

    strcpy( buf1, "{C" );
    if ( STREFA( ch ) )
    {
	sprintf( buf, "Mieszkan%s %s, ",
		ch->sex == 2 ? "ka" : "iec",
		POCHODZENIE( ch ) );
	strcat( buf1, buf );
    }
    sprintf( buf,
	    "%s%s%s{x\n\r-------------------------------------------------------------------------------\n\r",
	    ch->short_descr,
	    IS_NPC( ch ) ? "" : ch->pcdata->title,
	    IS_SET( ch->info, INF_KARTOGRAF ) ? ch->sex == 2 ? "{C, kartografka" : "{C, kartograf" : "" );
    strcat( buf1, buf );

    pzl = liczba( ( ch->played + (int) (current_time - ch->logon ) ) / 3600 );
    sprintf( buf, "Wiek: {Y%d{G lat%s ({Y%d{G godzin%s)",
	    get_age( ch ),
	    liczba( get_age( ch ) ) == 2 ? "a" : "", /* 1 niemozliwe */
	    ( ( ch->played + (int) (current_time - ch->logon ) ) / 3600 ),
	    pzl == 1 ? "a" : pzl == 2 ? "y" : "" );
    sprintf( temp, "{G%-45s  Moralno`s`c: ", buf );
    strcat( buf1, temp );
    if ( ch->level >= 10 )
    {
	sprintf( buf, "{Y%d{G (", ch->alignment );
	strcat( buf1, buf );
    }
	 if ( ch->alignment >   900 ) strcat( buf1, "anielska" );
    else if ( ch->alignment >   700 ) strcat( buf1, "`swi`eta" );
    else if ( ch->alignment >   350 ) strcat( buf1, "{gdobra" );
    else if ( ch->alignment >   100 ) strcat( buf1, "{gmi`la" );
    else if ( ch->alignment >= -100 ) strcat( buf1, "{xneutralna" );
    else if ( ch->alignment >= -350 ) strcat( buf1, "{ygro`xna" );
    else if ( ch->alignment >= -700 ) strcat( buf1, "{rz`la" );
    else if ( ch->alignment >= -900 ) strcat( buf1, "{Rdemoniczna" );
    else                             strcat( buf1, "{Rszata`nska" );
    if ( ch->level >= 10 )
	strcat( buf1, "{G){x\n\r" );
    else
	strcat( buf1, "{x\n\r" );

    sprintf( buf,
	    " {GSi`l: {M%2d{G/{m%-2d{G      Poziom: {Y%-3d {G",
	    get_curr_str( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_STR
	     ? UMIN( 30, 25 + race_table[ ch->race ].str_mod )
	     : UMIN( 23 + race_table[ ch->race ].str_mod, 26 ),
	    ch->level );
    strcat( buf1, buf );
    if ( get_trust( ch ) != ch->level )
	sprintf( buf, "({Y%3d{G)", get_trust( ch ) );
    else
	strcpy( buf, "     " );
    strcat( buf1, buf );
    sprintf( buf, "          Rasa: {R%s",
	( !IS_NPC( ch ) && ch->race == zr_wampir
       && ch->pcdata->wampiryzm.rasa != -1 )
	 ? race_table[ ch->pcdata->wampiryzm.rasa ].who_name
	 : race_table[ ch->race ].who_name );
    strcat( buf1, buf );
    if ( CZY_WAMPIR( ch ) && !( IS_NPC( ch )
	|| ( ch->race == zr_wampir
	  && ch->pcdata->wampiryzm.rasa == -1 ) ) )
	strcat( buf1, "{G ({Rwampir{G){x\n\r" );
    else
	strcat( buf1, "{x\n\r" );

    sprintf( buf,
	    " {GInt: {M%2d{G/{m%-2d{G   Pkt Do`sw.: {Y%-6d{G         Profesja: {R%s{x\n\r",
	    get_curr_int( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_INT
	     ? UMIN( 30, 25 + race_table[ ch->race ].int_mod )
	     : UMIN( 23 + race_table[ ch->race ].int_mod, 26 ),
	    ch->exp, IS_NPC( ch ) ? "Mob" : class_table[ ch->klasa ].long_name );
    strcat( buf1, buf );

    sprintf( temp, "%d{G/{y%d", ch->hit, ch->max_hit );
    sprintf( buf,
	    " {GM`ad: {M%2d{G/{m%-2d{G       `Zycie: {Y%-15s{G     KP, MKP: ",
	    get_curr_wis( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_WIS
	     ? UMIN( 30, 25 + race_table[ ch->race ].wis_mod )
	     : UMIN( 23 + race_table[ ch->race ].wis_mod, 26 ),
	    temp );
    strcat( buf1, buf );
    if ( ch->level >= 25 )
    {
	sprintf( buf, "{Y%d{G, {Y%d{G (", GET_AC( ch ), GET_MAC( ch ) );
	strcat( buf1, buf );
    }

    pzl = LICZ_WYPAROWANIE_KP( ch, 100 );
	 if ( pzl >= 100 ) strcat( buf1, "totalne dno" );
    else if ( pzl >=  90 ) strcat( buf1, "beznadziejnie" );
    else if ( pzl >=  85 ) strcat( buf1, "`xle" );
    else if ( pzl >=  80 ) strcat( buf1, "nieciekawie" );
    else if ( pzl >=  75 ) strcat( buf1, "cienko" );
    else if ( pzl >=  70 ) strcat( buf1, "przeci`etnie" );
    else if ( pzl >=  65 ) strcat( buf1, "tak sobie" );
    else if ( pzl >=  60 ) strcat( buf1, "w miar`e" );
    else if ( pzl >=  55 ) strcat( buf1, "przyzwoicie" );
    else if ( pzl >=  50 ) strcat( buf1, "nie`xle" );
    else if ( pzl >=  45 ) strcat( buf1, "`swietnie" );
    else if ( pzl >=  40 ) strcat( buf1, "bajecznie" );
    else if ( pzl >=  35 ) strcat( buf1, "cudownie" );
    else		   strcat( buf1, "idea`l" );

    if ( ch->level >= 25 )
	strcat( buf1, "){x\n\r" );
    else
	strcat( buf1, "{x\n\r" );

    sprintf( temp, "%d{G/{y%d", ch->mana, ch->max_mana );
    sprintf( buf,
	    " {GZr`e: {M%2d{G/{m%-2d{G        Mana: {Y%-15s{G         OPC: ",
	    get_curr_dex( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_DEX
	     ? UMIN( 30, 25 + race_table[ ch->race ].dex_mod )
	     : UMIN( 23 + race_table[ ch->race ].dex_mod, 26 ),
	    temp );
    strcat( buf1, buf );
    if ( ch->level >= 25 )
    {
	sprintf( buf, "{Y%d{G (", GET_ST( ch ) );
	strcat( buf1, buf );
    }
	 if ( GET_ST( ch ) <    0 ) strcat( buf1, "totalne dno" );
    else if ( GET_ST( ch ) <   10 ) strcat( buf1, "beznadziejnie" );
    else if ( GET_ST( ch ) <   20 ) strcat( buf1, "`xle" );
    else if ( GET_ST( ch ) <   30 ) strcat( buf1, "przeci`etnie" );
    else if ( GET_ST( ch ) <   40 ) strcat( buf1, "tak sobie" );
    else if ( GET_ST( ch ) <   50 ) strcat( buf1, "w miar`e" );
    else if ( GET_ST( ch ) <   60 ) strcat( buf1, "przyzwoicie" );
    else if ( GET_ST( ch ) <   70 ) strcat( buf1, "nie`xle" );
    else if ( GET_ST( ch ) <   80 ) strcat( buf1, "`swietnie" );
    else if ( GET_ST( ch ) <   90 ) strcat( buf1, "bajecznie" );
    else if ( GET_ST( ch ) <  100 ) strcat( buf1, "cudownie" );
    else                            strcat( buf1, "idea`l" );
    if ( ch->level >= 25 )
	strcat( buf1, "){x\n\r" );
    else
	strcat( buf1, "{x\n\r" );

    sprintf( temp, "%d{G/{y%d", ch->move, ch->max_move );
    sprintf( buf,
	    " {GBud: {M%2d{G/{m%-2d{G        Ruch: {Y%-15s{G",
	    get_curr_con( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_CON
	     ? UMIN( 30, 25 + race_table[ ch->race ].con_mod )
	     : UMIN( 23 + race_table[ ch->race ].con_mod, 26 ), temp );
    strcat( buf1, buf );
    if ( !can_see_room( ch, ch->in_room ) )
	strcpy( temp, "?" );
    else
	sprintf( temp, "%d", ch->carry_number );
    sprintf( buf, "   Inwentarz: {Y%s{G/{y%d{G, {Y%d{G/{y%d{G kg{x\n\r",
	    temp, can_carry_n( ch ),
	    ch->carry_weight / 4, can_carry_w( ch ) / 4 );
    strcat( buf1, buf );

    if ( !IS_NPC( ch ) )
    {
	sprintf( buf, "  {G`Cw: {Y%-5d{G       Z`loto: {Y%-14s{x{GZab`ojstwa: {Y%d{G ({Y%d{G gracz%s){x\n\r",
		ch->practice,
		formatuj_liczbe( ch->gold ),
		ch->pcdata->killed, ch->pcdata->pkilled,
		ch->pcdata->pkilled == 1 ? "a" : "y" );
	strcat( buf1, buf );

	sprintf( buf, "{GStrona: {Y%-3d{G        Bank: {Y%-14s{G    Zgony: {Y%d{G ({Y%d{G z r`ak gracz%s,{x\n\r",
		ch->pcdata->pagelen,
		formatuj_liczbe( ch->pcdata->bank ),
		ch->pcdata->dead, ch->pcdata->pdead,
		ch->pcdata->pdead == 1 ? "a" : "y" );
	strcat( buf1, buf );

	sprintf( buf, "{GTch`orz: {Y%-5d{G Pkt Zada`n: {Y%-6d{G                   {Y%d{G wykrw., {Y%d{G samob.){x\n\r",
		ch->wimpy, ch->qp,
		ch->pcdata->wykrwawienia, ch->pcdata->samobojstwa );
	strcat( buf1, buf );

	sprintf( buf, "{GG`l`od   [%s]  Pragnienie [%s]  Nietrze`xwo`s`c [%s]  Ukrycie       [%s]  Niewidka [%s]{x\n\r",
		ch->pcdata->condition[ COND_FULL ] < 3 ? "{RX{G" : " ",
		ch->pcdata->condition[ COND_THIRST ] < 3 ? "{RX{G" : " ",
		ch->pcdata->condition[ COND_DRUNK ] > 3 ? "{RX{G" : " ",
		IS_AFFECTED( ch, AFF_HIDE ) ? "{RX{G" : " ",
		IS_AFFECTED( ch, AFF_INVISIBLE ) ? "{RX{G" : " " );
	strcat( buf1, buf );

	sprintf( buf, "{GKl`atwa [%s]  Otrucie    [%s]  Brak tlenu   [%s]  Pal`ace s`lo`nce [%s]  Lot      [%s]{x\n\r",
		IS_AFFECTED( ch, AFF_CURSE ) ? "{RX{G" : " ",
		IS_AFFECTED( ch, AFF_POISON ) ? "{RX{G" : " ",
		CZY_BRAK_TLENU( ch ) ? "{RX{G" : " ",
		CZY_PALACE_SLONCE( ch ) ? "{RX{G" : " ",
		IS_FLYING( ch ) ? "{RX{G" : " " );
	strcat( buf1, buf );
    }

    strcat( buf1, "-------------------------------------------------------------------------------\n\r" );
    if ( !IS_NPC( ch ) )
    {
	bool linia = FALSE;
	if ( *ch->false_name )
	{
	    sprintf( buf, "{G Fa`lszywe imi`e: {Y%s{x\n\r", ch->false_name );
	    strcat( buf1, buf );
	    linia = TRUE;
	}

	if ( *ch->long_descr && is_affected( ch, gsn_disguise ) )
	{
	    sprintf( buf, "{GOpis maskuj`acy: {Y%s{x\n\r", ch->long_descr );
	    strcat( buf1, buf );
	    linia = TRUE;
	}
	else if ( *ch->long_descr_orig && !is_affected( ch, gsn_disguise ) )
	{
	    sprintf( buf, "{GOpis maskuj`acy: {Y%s{x\n\r", ch->long_descr_orig );
	    strcat( buf1, buf );
	    linia = TRUE;
	}
	if ( linia )
	    strcat( buf1, "-------------------------------------------------------------------------------\n\r" );
    }
    if ( !IS_NPC( ch ) && get_trust( ch ) >= 101 )
    {
	bool czy_cokolwiek = FALSE;

	if ( is_authorized( ch, "bniewidka" ) )
	{
	    sprintf( buf, "{GBoska niewidzialno`s`c [%s], poziom: {Y%d{G.{x\n\r",
			IS_SET( ch->act, PLR_WIZINVIS ) ? "{RX{G" : " ",
			ch->pcdata->wizinvis );
	    strcat( buf1, buf );
	    czy_cokolwiek = TRUE;
	}

	if ( is_authorized( ch, "bkuk`la" ) )
	{
	    sprintf( buf, "{GBoska kuk`la [%s].{x\n\r",
			IS_SET( ch->act, PLR_BOSKA_KUKLA ) ? "{RX{G" : " " );
	    strcat( buf1, buf );
	    czy_cokolwiek = TRUE;
	}

	/* Lam: istotne jest, czy moze latac, a nie, czy moze zmieniac te
		teksty */
	if ( is_authorized( ch, "le`cdo" ) )
	{
	    sprintf( buf, "{GWskok:  %s {R%s.{x\n\r", ch->short_descr,
			ch->pcdata->bamfin[ 0 ] ? ch->pcdata->bamfin
			: "pojawia si`e w`sr`od rozb`lysk`ow `swiat`la" );
	    strcat( buf1, buf );
	    sprintf( buf, "{GWyskok: %s {R%s.{x\n\r", ch->short_descr,
			ch->pcdata->bamfout[ 0 ] ? ch->pcdata->bamfout
			: "znika w`sr`od rozb`lysk`ow `swiat`la" );
	    strcat( buf1, buf );
	    czy_cokolwiek = TRUE;
	}

	if ( get_trust( ch ) > 103 && is_authorized( ch, ">" ) )
	{
	    sprintf( buf, "{GKana`l nie`smiertelnych: {Y%d{G poziom.{x\n\r",
			ch->pcdata->niesmpoz );
	    strcat( buf1, buf );
	    czy_cokolwiek = TRUE;
	}

	if ( czy_cokolwiek )
	    strcat( buf1, "-------------------------------------------------------------------------------\n\r" );
    }

    pzl = ( PP( ch ) && !IS_NPC( ch ) ) ? ch->pcdata->ic_position : ch->position;
    switch ( pzl )
    {
    case POS_DEAD:
	if ( ch->sex == 2 )
	    strcat( buf1, "{RJeste`s MARTWA!{x\n\r" );
	else
	    strcat( buf1, "{RJeste`s MARTWY!{x\n\r" );
	break;
    case POS_MORTAL:
	if ( ch->sex == 2 )
	    strcat( buf1, "{RJeste`s `smiertelnie ranna!{x\n\r" );
	else
	    strcat( buf1, "{RJeste`s `smiertelnie ranny!{x\n\r" );
	break;
    case POS_INCAP:
	if ( ch->sex == 2 )
	    strcat( buf1, "{RJeste`s nieprzytomna.{x\n\r" );
	else
	    strcat( buf1, "{RJeste`s nieprzytomny.{x\n\r" );
	break;
    case POS_STUNNED:
	if ( ch->sex == 2 )
	    strcat( buf1, "{RJeste`s poharatana.{x\n\r" );
	else
	    strcat( buf1, "{RJeste`s poharatany.{x\n\r" );
	break;
    case POS_SLEEPING:
	strcat( buf1, "`Spisz.\n\r" ); break;
    case POS_RESTING:
	strcat( buf1, "Odpoczywasz.\n\r" ); break;
    case POS_STANDING:
	strcat( buf1, "Stoisz.\n\r" ); break;
    case POS_FIGHTING:
	strcat( buf1, "{rWalczysz.{x\n\r" ); break;
    }

    if ( ch->level >= 15 )
    {
	sprintf( buf, "{mpremia celno`sci: {R%-3d{m  obra`ze`n: {R%d{m.{x\n\r",
	    get_hitroll( ch, WEAR_WIELD ), get_damroll( ch, WEAR_WIELD) );
	if ( IS_NPC( ch )
	    || moze_uzyc( ch, gsn_dual ) )
	    strcat ( buf1, "Pierwsza bro`n: " );
	else
	    buf[ 2 ] = 'P'; /* "Premia" */
	strcat( buf1, buf );
	if ( get_eq_char( ch, WEAR_WIELD_2 )
	  || ( ( IS_NPC( ch ) || ch->pcdata->learned[ gsn_dual ] )
	    && !( get_eq_char( ch, WEAR_SHIELD )
	      || get_eq_char( ch, WEAR_HOLD )
	      || get_eq_char( ch, WEAR_LIGHT ) ) ) )
	{
	    sprintf( buf, "Druga bro`n   : {mpremia celno`sci: {R%-3d{m  obra`ze`n: {R%d{m.{x\n\r",
		get_hitroll( ch, WEAR_WIELD_2 ),
		get_damroll( ch, WEAR_WIELD_2 ) );
	    strcat( buf1, buf );
	}
    }

    strcat( buf1, "{gAby zobaczy`c, jakie czary na ciebie dzia`laj`a, napisz \"wp`lywy\".{x\n\r" );

    send_to_char( buf1, ch );

    return;
}


/* Lam 15.6.2005: sortowanie czasem trwania od 20 poziomu, kiedy to widac */
#define MAX_CZAS_TRWANIA 250

KOMENDA( do_affects )
{
    char         buf1[ MAX_STRING_LENGTH * 4 ];
    char         buf[ MAX_STRING_LENGTH ];
    char	 tczt[ MAX_CZAS_TRWANIA ];
    AFFECT_DATA *paf;
    int		 pzl;
    int		 czt;
    int          i;
    bool         sortuj;

    buf1[ 0 ] = '\0';

    sortuj = ( *argument && !str_prefix( argument, "sortuj" ) && ch->level >= 20 );

    if ( ch->affected )
    {
	bool printed = FALSE;

	if ( sortuj )
	{
	    for ( i = 0; i < MAX_CZAS_TRWANIA; i++ )
		tczt[ i ] = 0;

	    for ( paf = ch->affected; paf; paf = paf->next )
	    {
		if ( paf->deleted
		  || IS_SET( skill_table[ paf->type ].flags, SKILL_INVISIBLE ) )
		    continue;

		czt = paf->duration + 2;
		tczt[ URANGE( 0, czt, MAX_CZAS_TRWANIA - 1 ) ]++;
	    }
	}

	for ( i = 0; i < MAX_CZAS_TRWANIA; i++ )
	{
	    if ( sortuj && !tczt[ i ] )
		continue;

	    czt = i - 2;

	    for ( paf = ch->affected; paf; paf = paf->next )
	    {
		if ( paf->deleted
		  || ( sortuj
		    && UMIN( paf->duration, MAX_CZAS_TRWANIA - 3 ) != czt ) )
		    continue;

		if ( !IS_SET( skill_table[ paf->type ].flags, SKILL_INVISIBLE ) )
		{
		    if ( !printed )
		    {
			strcat( buf1, "{gDzia`laj`a na ciebie czary:{x\n\r" );
			printed = TRUE;
		    }

		    sprintf( buf, "%s \"{m%s{x\"",
			     skill_table[ paf->type ].spell_fun
			  && skill_table[ paf->type ].spell_fun != spell_null ?
				 "Czar: " : "Wp`lyw:",
			     skill_table[ paf->type ].pl_name );
		    strcat( buf1, buf );

		    if ( ch->level >= 20 )
		    {
			pzl = liczba( paf->duration );
			if ( paf->location != APPLY_NONE && paf->duration != -1 )
			sprintf( buf,
				 " zmienia {r%s{x o {B%d{x na {y%d{x godzin%s",
				 affect_loc_name_pl_b( paf->location ),
				 paf->modifier,
				 paf->duration,
				 pzl == 1 ? "`e" : pzl == 2 ? "y" : "" );
			else if ( paf->duration != -1 )
			sprintf( buf,
				 " dzia`la jeszcze przez {y%d{x godzin%s",
				 paf->duration,
				 pzl == 1 ? "`e" : pzl == 2 ? "y" : "" );
			else if ( paf->location != APPLY_NONE )
			sprintf( buf,
				" zmienia {r%s{x o {B%d{x",
				affect_loc_name_pl_b( paf->location ),
				paf->modifier );
			else strcpy( buf, "" );
			strcat( buf1, buf );
		    }
		    strcat( buf1, ".\n\r" );
		}
	    }

	    if ( !sortuj )
		break;
	}

	if ( !printed )
	    sprintf( buf1, "Nie dzia`laj`a na ciebie absolutnie `zadne czary.\n\r" );
    }
    else
       sprintf( buf1, "Nie dzia`laj`a na ciebie absolutnie `zadne czary.\n\r" );

    send_to_char( buf1, ch );

    return;
}


char *asctime_pl( const struct tm *timeptr )
{
    static const char wday_name[ 7 ][ 16 ] = {
	"niedziela", "poniedzia`lek", "wtorek", "`sroda", "czwartek",
	"pi`atek", "sobota"
    };
    static const char mon_name[ 12 ][ 16 ] = {
	"stycznia", "lutego", "marca", "kwietnia", "maja", "czerwca",
	"lipca", "sierpnia", "wrze`snia", "pa`xdziernika", "listopada", "grudnia"
    };
    static char result[ 128 ]; /* powinno wystarczyc */

    (void) sprintf( result, "%02d:%02d:%02d, %s, %d %s %dr.\n",
	timeptr->tm_hour,
	timeptr->tm_min, timeptr->tm_sec,
	wday_name[ timeptr->tm_wday ],
	timeptr->tm_mday, mon_name[ timeptr->tm_mon ],
	1900 + timeptr->tm_year );

    return result;
}


char *asctime_pl_nonl( const struct tm *timeptr )
{
    char *tym = asctime_pl( timeptr );
    if ( *tym )
	tym[ strlen( tym ) - 1 ] = 0;

    return tym;
}


char *asctime_pl_krotko( const struct tm *timeptr )
{
    static char result[ 128 ]; /* powinno wystarczyc */

    (void) sprintf( result, "%02d:%02d:%02d, %d.%d.%d\n",
	timeptr->tm_hour,
	timeptr->tm_min, timeptr->tm_sec,
	timeptr->tm_mday, timeptr->tm_mon + 1,
	1900 + timeptr->tm_year );

    return result;
}


char *asctime_pl_krotko_nonl( const struct tm *timeptr )
{
    char *tym = asctime_pl_krotko( timeptr );
    if ( *tym )
	tym[ strlen( tym ) - 1 ] = 0;

    return tym;
}


extern char str_boot_time[ ];
KOMENDA( do_time )
{
    char buf[ MAX_STRING_LENGTH ];
    int day, min, i;
    ROOM_INDEX_DATA *room;

    if ( !( room = ch->in_room ) )
	return;

    if ( PP( ch ) && !IS_NPC( ch ) && ch->pcdata->ic_in_room )
	room = ch->pcdata->ic_in_room;

    day = room->area->time_info.day + 1;
    min = ( pulse_point_max - pulse_point ) * 60 / pulse_point_max;
    i = liczba( min );

    if ( min )
	sprintf( buf,
	    "%s %d minut%s po godzinie %d, dzie`n %s, %d dzie`n miesi`aca %s %d roku.\n\r",
		i == 2 ? "S`a" : "Jest",
		min,
		i == 1 ? "a" : i == 2 ? "y" : "",
		room->area->time_info.hour,
		day_name[ day % 7 ],
		day,
		month_name[ room->area->time_info.month ],
		room->area->time_info.year );
    else
	sprintf( buf,
	    "Jest godzina %d, dzie`n %s, %d dzie`n miesi`aca %s %d roku.\n\r",
		room->area->time_info.hour,
		day_name[ day % 7 ],
		day,
		month_name[ room->area->time_info.month ],
		room->area->time_info.year );

    send_to_char( buf, ch );

    for ( i = 0; holiday_table[ i ].name; i++ )
	if ( holiday_table[ i ].month == ch->in_room->area->time_info.month + 1
	  && holiday_table[ i ].day == day )
	break;
    if ( holiday_table[ i ].name )
    {
	sprintf( buf, "Dzisiejszy dzie`n og`loszono %s. Jest to dzie`n `swi`ateczny.\n\r",
	    holiday_table[ i ].name );
	STC( buf, ch );
    }

    if ( is_authorized( get_char( ch ), "jebustaw" ) )
    {
	sprintf( buf,
	    "Laca odpalono  : %s\rCzas systemowy : %s\r",
	    str_boot_time,
	    (char *) asctime_pl( localtime( &current_time ) ) );
	send_to_char( buf, ch );
	if ( down_time > 0 )
	{
	    sprintf( buf, "%-15s: %s\r", Reboot ? "Restart" : "Koniec pracy",
		(char *) asctime_pl( localtime( &down_time ) ) );
	    send_to_char( buf, ch );
	}
    }

    return;
}


/*
 * Lam 10.1.2003
 */
KOMENDA( do_kalendarz )
{
    char buf[ MSL ]; /* sprawdzilem, kalendarz zajmuje ok. 3 KiB */
    char buf1[ MIL ];
    int mies, mmies, month, day, tyg, dzi, pom, i;

    sprintf( buf, "                                %d\n\r",
	ch->in_room->area->time_info.year );
    month = ch->in_room->area->time_info.month;
    day = ch->in_room->area->time_info.day + 1;
    for ( mies = 0; mies < 17; mies += 3 )
    {
	for ( mmies = mies; mmies < mies + 3 && mmies < 17; mmies++ )
	{
	    if ( mmies != mies )
		strcat( buf, "   " );
	    sprintf( buf1, "{y%2d{Y%s{x", mmies + 1,
			wyrownaj( month_name[ mmies ], 18 ) );
	    strcat( buf, buf1 );
	}
	strcat( buf, "\n\r" );
	for ( mmies = mies; mmies < mies + 3 && mmies < 17; mmies++ )
	{
	    if ( mmies != mies )
		strcat( buf, "   " );
	    strcat( buf, "by k`l gr wo wi s`l ks" );
	}
	strcat( buf, "\n\r" );
	for ( tyg = 0; tyg < 5; tyg++ )
	{
	    for ( mmies = mies; mmies < mies + 3 && mmies < 17; mmies++ )
	    {
		if ( mmies != mies )
		    strcat( buf, "   " );
		for ( dzi = 1; dzi < 8; dzi++ )
		{
		    if ( dzi != 1 )
			strcat( buf, " " );
		    pom = tyg * 7 + dzi;
		    if ( pom == day && mmies == month )
			sprintf( buf1, "{G%2d{x", pom );
		    else
		    {
			for ( i = 0; holiday_table[ i ].name; i++ )
			    if ( holiday_table[ i ].month == mmies + 1
			      && holiday_table[ i ].day == pom )
				break;
			if ( holiday_table[ i ].name )
			    sprintf( buf1, "{R%2d{x", pom );
			else
			    sprintf( buf1, "%2d", pom );
		    }
		    strcat( buf, buf1 );
		}
	    }
	    strcat( buf, "\n\r" );
	}
	if ( mies < 15 )
	    strcat( buf, "\n\r" );
    }
    STC( buf, ch );

    return;
}


KOMENDA( do_weather )
{
	   char         buf     [ MAX_STRING_LENGTH ];
    static char * const sky_look[ 4 ] =
    {
	"bezchmurne",
	"pochmurne",
	"deszczowe",
	"przecinane b`lyskawicami"
    };

    if ( !ch->in_room )
	return;

    if ( ch->in_room->sector_type == SECT_UNDERWATER )
    {
	send_to_char( "Nie wiesz, jaka pogoda jest na powierzchni.\n\r", ch );
	return;
    }

    if ( !IS_OUTSIDE( ch ) )
    {
	send_to_char( "Nie wiesz, jaka pogoda jest na zewn`atrz.\n\r", ch );
	return;
    }

    sprintf( buf, "Niebo jest %s i %s.\n\r",
	    sky_look[ ch->in_room->area->weather_info.sky ],
	    ch->in_room->area->weather_info.change >= 0
	    ? "wieje ciep`ly wiaterek z po`ludnia"
	    : "wieje zimny wiatr z p`o`lnocy" );
    send_to_char( buf, ch );

    return;
}


void real_help( CHAR_DATA *ch, char *arg, char *skroty, WHO_DESCRIPTOR_DATA *d )
{
    HELP_DATA *help;
    char      *keylist;
    char       big_buf[ MAX_STRING_LENGTH * 25 ];
    char       key[ MAX_INPUT_LENGTH ];
    bool       jest = FALSE;

    big_buf[ 0 ] = '\0';

    if ( arg[ 0 ] == '\0' )
    {
	bug( "real_help: brak argumentu!", 0 );
	return;
    }

    for ( help = help_first; help; help = help->next )
    {
	if ( ( !ch && help->level > 1 )
	  || ( ch && help->level > get_trust( ch ) ) )
	    continue;

	for ( keylist = help->keyword; ; )
	{
	    keylist = one_argument( keylist, key );
	    if ( key[ 0 ] == '\0' || !str_cmp( arg, key ) )
		break;
	}

	if ( key[ 0 ] == '\0' )
	    continue;

	if ( jest )
	    strcat( big_buf, "\n\r-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r\n\r" );

	if ( help->level >= 0 && str_cmp( arg, "nog`loszenie" ) )
	{
	    strcat( big_buf, help->keyword );
	    strcat( big_buf, "\n\r" );
	}

	if ( strlen( help->text ) + strlen( big_buf )
	      > MAX_STRING_LENGTH * 24 )
	{
	    bug( "real_help: Za du`zo tekstu!", 0 );
	    if ( ch )
		send_to_char( "Za du`zo tekstu!\n\r", ch );

	    return;
	}

	if ( help->text[ 0 ] == '.' )
	    strcat( big_buf, przemiel_opis( ch, help->text ) + 1 );
	else
	    strcat( big_buf, przemiel_opis( ch, help->text ) );
	jest = TRUE;
    }

    if ( skroty[ 0 ] != '\0' )
    {
	strcat( big_buf, "\n\r{gSkr`ot pasuje r`ownie`z do pomocy:{x\n\r" );
	strcat( big_buf, skroty );
    }

    if ( ch )
	send_to_char( big_buf, ch );
    else if ( d )
    {
	char wyjbuf[ MSL * 30 ];
	if ( big_buf[ 0 ] == '\0' )
	    sprintf( big_buf, "{RNie ma takiego tematu!{x" );

	colourconv( wyjbuf, lac2html( big_buf, FALSE ), NULL, sizeof( wyjbuf ) );
	STH( d, wyjbuf );
    }

    return;
}


void sprawdz_hasla( char *arg, char *haslo, char *skroty, int poziom )
{
    HELP_DATA *help;
    char      *keylist;
    char       key[ MAX_INPUT_LENGTH ];
    bool       jedno = FALSE;
    int        i;

    for ( help = help_first; help; help = help->next )
    {
	if ( help->level > poziom )
	    continue;

	for ( keylist = help->keyword; ; )
	{
	    keylist = one_argument( keylist, key );

	    if ( key[ 0 ] == '\0' || !str_cmp( arg, key ) )
		break;
	}

	/* Jesli pasuje dokladnie, to zapisuje i sprawdza dalej,
	   w przeciwnym wypadku szuka slow zaczynajacych sie od argumentu

	   Ulryk 28.0.2004: wazna zmiana: nie wyszukujemy skrotow, gdy argument
	   zaczyna sie od znaku # */
	if ( *key )
	{
	    strcpy( haslo, key );
	    continue;
	}
	else if ( arg[ 0 ] != '#' )
	    for ( keylist = help->keyword; ; )
	    {
		keylist = one_argument( keylist, key );

		if ( key[ 0 ] == '\0' || !str_prefix( arg, key ) )
		    break;
	     }

	if ( key[ 0 ] == '\0' )
	    continue;

	strcat( key, "{x\n\r" );
	if ( str_infix( key, skroty ) )
	{
	    if ( !( *skroty ) )
		jedno = TRUE;
	    if ( *skroty )
		jedno = FALSE;

	    strcat( skroty, "{?" );
	    strcat( skroty, key );
	}
    }

    if ( jedno && haslo[ 0 ] == '\0' )
    {
	for ( i = 0; skroty[ i + 2 ] != '{'; i++ )
	   haslo[ i ] = skroty[ i + 2 ];
	haslo[ i ] = '\0';
	skroty[ 0 ] = '\0';
    }

    return;
}


KOMENDA( do_help )
{
    char      *arg = argument;
    char       haslo[ MAX_INPUT_LENGTH ];
    char       skroty[ MAX_STRING_LENGTH ];

    skroty[ 0 ] = haslo[ 0 ] = '\0';

    if ( *arg == '\0' )
	arg = "podsumowaniexxx";

    sprawdz_hasla( arg, haslo, skroty, get_trust( ch ) );

    if ( haslo[ 0 ] == '\0' && skroty[ 0 ] == '\0' )
    {
	send_to_char( "Nie pomog`e ci z tym.\n\r", ch );
	return;
    }

    if ( haslo[ 0 ] != '\0' )
    {
	real_help( ch, haslo, skroty, 0 );
	return;
    }

    if ( *skroty )
    {
	send_to_char( "{gSkr`ot pasuje do pomocy:{x\n\r", ch );
	send_to_char( skroty, ch );
    }

    return;
}


/*
 * New 'who' command originally by Alander of Rivers of Mud.
 * Lam: sortowanie i drobne nieistotne poprawki (tlumaczenie, poziomy, kolor,
 * klany (sortowanie kiedys napisze lepsze, na razie musi byc tak))
 */
KOMENDA( do_who )
{
    real_who( ch, argument, NULL );

    return;
}


char *zmien_nawiasy( char *arg )
{
    char *p = arg;
    char *b;
    char  buf[ MSL ];

    if ( !arg )
	return arg;

    buf[ 0 ] = '\0';
    b = buf;

    while ( *p != '\0' )
    {
	if ( *p == '<' )
	{
	   strcat( buf, "&lt;" );
	   while ( *b != '\0' ) b++;
	}
	else if ( *p == '>' )
	{
	   strcat( buf, "&gt;" );
	   while ( *b != '\0' ) b++;
	}
	else if ( *p == '&' )
	{
	   strcat( buf, "&amp;" );
	   while ( *b != '\0' ) b++;
	}
	else
	{
	    *b = *p;
	    *++b = '\0';
	}

	p++;
    }

    strcpy( arg, buf );

    return arg;
}


void przytnij_linie( char *linia, int szer )
{
    while ( *linia && szer )
	if ( *linia == '{' )
	{
	    linia++;
	    if ( *linia )
	    {
		if ( *linia == '{' ) /* {{ liczy sie jako znak { */
		    szer--;
		linia++;
	    }
	    else
	    {
		linia--; /* zeby zignorowac { */
		break;
	    }
	}
	else if ( *linia == '`' )
	{
	    linia++;
	    if ( *linia == '`' ) /* `` liczy sie jako znak ` */
	    {
		linia++;
		szer--;
	    }
	    else if ( !*linia )
	    {
		linia--; /* zeby zignorowac ` */
		break;
	    }
	}
	else
	{
	    linia++;
	    szer--;
	}

    *linia = '\0';

    return;
}


void real_who( CHAR_DATA *ch, char *argument, WHO_DESCRIPTOR_DATA *who_d )
{
    DESCRIPTOR_DATA *d;
    ZONE_DATA       *zone;
    DESCRIPTOR_DATA *poziomy[ MAX_LEVEL + 1 ];
    DESCRIPTOR_DATA *ostatni[ MAX_LEVEL + 1 ];
    /* powinno wystarczyc na ok. 1000 graczy */
    char             buf[ MAX_STRING_LENGTH * 30 ];
    char             buf2[ MAX_STRING_LENGTH ];
#if defined( KLANY )
    char             clanbuf[ MAX_INPUT_LENGTH ];
#endif
    char             invisbuf[ MAX_INPUT_LENGTH ];
    int              iClass;
    int              iLevelLower;
    int              iLevelUpper;
    int              nNumber;
    int              nMatch;
    bool             rgfClass[ MAX_CLASS ];
    bool             fClassRestrict;
    bool	     fClanRestrict;  /* Lam */
    bool             fShort = FALSE; /* Lam */
    bool             fStrefRestrict; /* Ulryk */
    char             clan[ MAX_INPUT_LENGTH ];
    char             strefa[ MAX_INPUT_LENGTH ];
    int              poziom;
    int              szerokosc;

    /*
     * Set default arguments.
     */
    iLevelLower    = 1;
    iLevelUpper    = MAX_LEVEL;
    fClassRestrict = FALSE;
    fClanRestrict  = FALSE;
    fStrefRestrict = FALSE;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	rgfClass[ iClass ] = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ; ch; ) /* dziala jak if ( ch ) for ( ; ; ) { } */
    {
	char arg[ MAX_STRING_LENGTH ];

	argument = one_argument( argument, arg );
	if ( arg[ 0 ] == '\0' )
	    break;

	if ( is_number( arg ) )
	{
	    switch ( ++nNumber )
	    {
	    case 1: iLevelLower = UMAX( 1, atoi( arg ) ); break;
	    case 2: iLevelUpper = UMIN( MAX_LEVEL, atoi( arg ) ); break;
	    default:
		send_to_char( "Mo`zesz poda`c tylko dwie liczby.\n\r", ch );
		return;
	    }
	}
	else
	{
	    int iClass;

	    /*
	     * Look for classes to turn on.
	     */
	    if ( !str_prefix( arg, "bogowie" ) )
		iLevelLower = L_SEN;
	    else if ( !str_prefix( arg, "nie`smiertelni" ) )
		iLevelLower = LEVEL_IMMORTAL;
	    else if ( !str_prefix( arg, "bohaterowie" ) )
		iLevelLower = LEVEL_HERO;
	    else if ( !str_cmp( "skr`ot", arg ) )
		fShort = TRUE;
	    else
	    {
		bool arg_to_strefa = FALSE;
		bool arg_to_prof = FALSE;

		for ( zone = zone_first; zone; zone = zone->next )
		{
		    if ( !str_prefix( arg, zone->nazwa ) )
		    {
			strcpy( strefa, zone->nazwa );
			fStrefRestrict = TRUE;
			arg_to_strefa = TRUE;
			break;
		    }
		}
		/* Ulryk: jesli to strefa, to juz nie profesja */
		if ( arg_to_strefa )
		    continue;

		/* arg[ 3 ]    = '\0'; -- Envy */
		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
		    if ( !str_cmp( arg, class_table[ iClass ].pl_name )
		      || !str_cmp( arg, class_table[ iClass ].long_name ) )
		    {
			fClassRestrict = TRUE;
			rgfClass[ iClass ] = TRUE;
			arg_to_prof = TRUE;
			break;
		    }
		}
		if ( arg_to_prof )
		    continue;

		if ( clan_lookup2( arg ) )
		{
		    strcpy( clan, arg );
		    fClanRestrict = TRUE;
		    continue;
		}

		if ( iClass == MAX_CLASS )
		{
		    send_to_char( "Kogo mam wy`swietli`c?\n\r", ch );
		    return;
		}
	    }
	}
    }

    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[ 0 ] = '\0';

    for ( poziom = iLevelLower; poziom <= iLevelUpper; poziom++ )
    {
	poziomy[ poziom ] = NULL;
	ostatni[ poziom ] = NULL;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	CHAR_DATA *wch = ( d->original ) ? d->original : d->character;

	if ( d->connected != CON_PLAYING
	  || ( ch && !can_see_who( ch, wch ) )
	  || ( !ch && IS_SET( wch->act, PLR_WIZINVIS ) && !IS_SET( wch->act, PLR_BOSKA_KUKLA ) ) )
	    continue;

	if ( wch->level >= iLevelLower
	  && wch->level <= iLevelUpper )
	{
	    if ( poziomy[ wch->level ] )
		ostatni[ wch->level ]->nast_na_poz = d;
	    else
		poziomy[ wch->level ] = d;
	    ostatni[ wch->level ] = d;
	    d->nast_na_poz = NULL;
	}
    }

    /* kto skrot, dostepne tylko graczom */
    if ( fShort )
    {
	for ( poziom = iLevelLower; poziom <= iLevelUpper; poziom++ )
	    for ( d = poziomy[ poziom ]; d; d = d->nast_na_poz )
	    {
		char poz[ 10 ];
		char kol[ 3 ];
		CHAR_DATA *wch = ( d->original ) ? d->original : d->character;

		if ( ( fClassRestrict && !rgfClass[ wch->klasa ] )
		  || ( fClanRestrict && !is_clan_member( wch, clan_lookup2( clan ) ) )
		  || ( fStrefRestrict && str_cmp( strefa, POCHODZENIE_M( wch ) ) ) )
		    continue;

		nMatch++;

		sprintf( poz, "%3d", wch->level );

		kol[ 0 ] = '\0';
		if ( wch->level >= LEVEL_STOPIEN )
		{
		    strcpy( poz, "---" );
		    strcpy( kol, "{W" );
		}

		if ( wch->level >= L_DIR )
		{
		    strcpy( poz, "===" );
		    strcpy( kol, "{C" );
		}

		sprintf( buf2, "%s[%s %s]%s%s",
			kol, poz, wyrownaj( wch->short_descr, -12 ),
			kol[ 0 ] ? "{x" : "",
			nMatch % 4 ? " " : "\n\r" );
		strcat( buf, buf2 );
	    }

	if ( nMatch % 4 )
		strcat( buf, "\n\r" );

	sprintf( buf2, "{y%d{r gracz%s.{x\n\r",
		nMatch, nMatch == 1 ? "" : "y" );
	strcat( buf, buf2 );
	send_to_char( buf, ch );
	return;
    }

    szerokosc = ch && ch->desc && ch->desc->szer ? ch->desc->szer : 80;
    szerokosc = UMAX( szerokosc, 45 ) - 1;

    for ( poziom = iLevelLower; poziom <= iLevelUpper; poziom++ )
	for ( d = poziomy[ poziom ]; d; d = d->nast_na_poz )
	{
	    CHAR_DATA *wch;
	    char const *class;

	    wch = ( d->original ) ? d->original : d->character;

	    /*
	     * Check for match against restrictions.
	     */
	    if ( ( fClassRestrict && !rgfClass[ wch->klasa ] )
	      || ( fClanRestrict && !is_clan_member( wch, clan_lookup2( clan ) ) )
	      || ( fStrefRestrict && str_cmp( strefa, POCHODZENIE_M( wch ) ) ) )
		continue;

	    nMatch++;

	    /*
	     * Figure out what to print for class.
	     */
	    class = class_table[ wch->klasa ].pl_name;
	    if ( wch->level >= LEVEL_STOPIEN || IS_SET( wch->info, INF_STOPIEN_SHOW ) )
	    {
		switch ( wch->level )
		{
		    default: break;
		    case 110: class = "       {B*{RS{MZ{BE{CF{x{B*{G"; break;
		    case 109: class = "DYREKTOR"; break;
		    case 108: class = "KIEROWNIK"; break;
		    case 107: class = "SENIOR"; break;
		    case 106: class = "SENIOR"; break;
		    case 105: class = "JUNIOR"; break;
		    case 104: class = "JUNIOR"; break;
		    case 103: class = "JUNIOR"; break;
		    case 102: class = "DUSZEK"; break;
		    case 101: class = "DUSZEK"; break;
		}
		if ( wch->pcdata->stopien && *wch->pcdata->stopien )
		    class = wch->pcdata->stopien;
		if ( IS_SET( wch->info, INF_BUDOWNICZY ) )
		    class = "BUDOWNICZY";
		else if ( IS_SET( wch->info, INF_QUESTMASTER ) )
		    class = "ZADA`NDAJEC";
	    }

#if defined( KLANY )
	    /* Klan */
	    if ( is_any_clan_member( wch ) && can_see_clan( ch, wch->pcdata->clan )
	      && *wch->pcdata->clan->skrot )
		sprintf( clanbuf, "{Y%c%s ",
		    wch->pcdata->clan_member->level >= CLAN_MASTER ? '*' :
		    wch->pcdata->clan_member->level >= CLAN_HERO ? '@' : ' ',
		    wch->pcdata->clan->skrot );
	    else
		strcpy( clanbuf, "       " );
#endif
	    /*
	     * Lam: Boska Niewidzialnosc
	     * Pomimo, ze smiertelni moga widziec bogow niewidzialnych dla poziomow
	     * nizszych, to nie beda wiedziec o tym, ze widza niewidzialnego.
	     */
	    if ( ch
	      && IS_SET( wch->act, PLR_WIZINVIS )
	      && get_trust( ch ) >= LEVEL_HERO )
	    {
		/* Lam/Vigud:
		 * (BN) widac jak masz BN bez BK
		 * (BK) widac jak masz BN z BK i twoje zaufanie przewyzsza
		   poziom BN (czyli zobaczysz osobnika WP, a nie tylko kukle)
		 * w innych przypadkach nie widac nic, tak samo jak ludzie
		   ponizej 100 nie widza */
		if ( !IS_SET( wch->act, PLR_BOSKA_KUKLA ) )
		    sprintf( invisbuf, "(BN: %d) ", wch->pcdata->wizinvis );
		else if ( get_trust( ch ) >= wch->pcdata->wizinvis )
		    sprintf( invisbuf, "(BK: %d) ", wch->pcdata->wizinvis );
		else
		    invisbuf[ 0 ] = '\0';
	    }
	    else
		invisbuf[ 0 ] = '\0';

	    /*
	     * Format it up.
	     */
	    if ( wch->level < LEVEL_STOPIEN && !IS_SET( wch->info, INF_STOPIEN_SHOW ))
		sprintf( buf2, "%s{y%3d {g%s {r%-11s%s %s{x%s%s%s%s%s%s%s%s%s%s%s",
			wch->trust > wch->level ? "{G<" :
			wch->trust == wch->level ? "{g<" : "{B(",
			wch->level,
			class,
			wyrownaj( IS_SET( wch->info, INF_BUDOWNICZY ) ? "BUDOWNICZY" :
			    wch->level >= LEVEL_HERO ? "BOHATER" :
			    race_table[ wch->race ].who_name, -11 ),
			wch->trust > wch->level ? "{G>" :
			wch->trust == wch->level ? "{g>" : "{B)",
#if defined( KLANY )
			IS_SET( wch->info, INF_CLAN_GUARD ) ? " {C({cSPK{C) " : wyrownaj( clanbuf, 7 ),
#else
			"",
#endif
			PP( wch ) ? "{W(PP){x " : "",
			invisbuf,
			( wch->desc && wch->desc->idle >= 2400 && ( get_trust( wch ) < LEVEL_IMMORTAL || ( ch && get_trust( wch ) <= get_trust( ch ) ) ) ) ? "(B) " : "",
			IS_SET( wch->act, PLR_AFK      ) ? "(OOK) " : "",
			IS_SET( wch->info, INF_MORS    ) ? "({BMORS{x) " : "",
			IS_SET( wch->info, INF_LAMER   ) ? "(LAIK) " : "",
			IS_SET( wch->act, PLR_KILLER   ) ? "(MORDERCA) " : "",
			IS_SET( wch->act, PLR_THIEF    ) ? "(Z`LODZIEJ) " : "",
			IS_SET( wch->act, PLR_ZABOJCA  ) ? "(ZAB`OJCA) " : "",
			wch->short_descr,
			wch->pcdata->title );
	    else
/* wersja z poziomem
		sprintf( buf2, "{M<{R={Y- %3d {G%s{Y-{R={M> {Y%s%s%s%s{G%s%s%s%s%s%s%s%s",
			wch->level, */
		sprintf( buf2, "{M<{R={Y- {G%s{Y -{R={M> %s{G%s%s%s%s%s%s%s%s%s%s%s",
			wyrownaj( class, 13 ),
#if defined( KLANY )
			IS_SET( wch->info, INF_CLAN_GUARD ) ? " {C({cSPK{C) " : wyrownaj( clanbuf, 7 ),
#else
			"",
#endif
			PP( wch ) ? "(PP) " : "",
			invisbuf,
			( wch->desc && wch->desc->idle >= 2400 && ( get_trust( wch ) < LEVEL_IMMORTAL || ( ch && get_trust( wch ) <= get_trust( ch ) ) ) ) ? "(B) " : "",
			IS_SET( wch->act, PLR_AFK      ) ? "(OOK) " : "",
			IS_SET( wch->info, INF_MORS    ) ? "({BMORS{G) " : "",
			IS_SET( wch->info, INF_LAMER   ) ? "(LAIK) " : "",
			IS_SET( wch->act, PLR_KILLER   ) ? "(MORDERCA) " : "",
			IS_SET( wch->act, PLR_THIEF    ) ? "(Z`LODZIEJ) " : "",
			IS_SET( wch->act, PLR_ZABOJCA  ) ? "(ZAB`OJCA) " : "",
			wch->short_descr,
			wch->pcdata->title );
	    przytnij_linie( buf2, szerokosc );
	    if ( !ch )
		zmien_nawiasy( buf2 );
	    /* 200 wystarczy na enter, "za duzo..." i "widzisz grajacych..." */
	    if ( strlen( buf ) + strlen( buf2 ) > sizeof( buf ) - 300 )
	    {
		strcat( buf, "Za du`zo graczy. U`zyj \"kto skr`ot\"." );
		strcat( buf, ch ? "\n\r" : "<br>" );
		poziom = MAX_LEVEL + 1; /* wiec wyjdzie z glownej petli */
		break;
	    }
	    strcat( buf, buf2 );
	    strcat( buf, ch ? "{x\n\r" : "{x<br>" );
	}

    sprintf( buf2, ch ? "{rWidzisz {y%d{r posta%s w grze.{x\n\r"
	: "{rWidzisz {y%d{r posta%s w grze.<br></span></pre>\n",
	    nMatch,
	    nMatch == 1 ? "`c" : liczba( nMatch ) == 2 ? "cie" : "ci" );
    strcat( buf, buf2 );

    if ( ch )
	send_to_char( buf, ch );
    else if ( who_d )
    {
	char wyjbuf[ MSL * 40 ];
	colourconv( wyjbuf, buf, NULL, sizeof( wyjbuf ) );
	STH( who_d, wyjbuf );
    }

    return;
}


/* Lam: nowe "kimjest" oraz "wiecej informacji" */
KOMENDA( do_whois )
{
    CHAR_DATA *wch;
    CHAR_DATA *nast;
    char buf[ MAX_STRING_LENGTH ];
    char name[ MAX_INPUT_LENGTH ];
    int poziomy[ MAX_CLASS ];
    int i, ile;
    bool found = FALSE;
    bool w_pomieszczeniu = TRUE;
    bool kopia_w_pom;

    one_argument( argument, name );

    if ( name[ 0 ] == '\0' )
    {
	send_to_char( "Sk`ladnia: kimjest <imi`e>\n\r", ch );
	return;
    }

    name[ 0 ] = UPPER( name[ 0 ] );

    buf[ 0 ] = '\0';
    for ( wch = ch->in_room->people; wch; wch = nast )
    {
	kopia_w_pom = w_pomieszczeniu;
	if ( w_pomieszczeniu )
	{
	    if ( wch->next_in_room )
		nast = wch->next_in_room;
	    else
	    {
		nast = char_list;
		w_pomieszczeniu = FALSE;
	    }
	}
	else
	    nast = wch->next;

	if ( !wch->in_room
	  || IS_NPC( wch )
	  || !can_see_who( ch, wch )
	  || str_prefix( name, wch->name )
	  || ( !kopia_w_pom && ch->in_room == wch->in_room ) )
	    continue;

	if ( found )
	    strcat( buf, "\n\r" );
	found = TRUE;

	if ( !wch->desc )
	    strcat( buf, "(BRAK PO`L`ACZENIA) " );
	else if ( wch->desc->idle >= 2400
	       && ( get_trust( wch ) < LEVEL_IMMORTAL
		 || get_trust( wch ) <= get_trust( ch ) ) )
	    strcat( buf, wch->sex == 2 ? "(BEZCZYNNA) " : "(BEZCZYNNY) " );

	/* Lam 19.12.2002: Wladca Leanderu, Lam */
	if ( STREFA( wch ) )
	{
	    if ( !str_cmp( wch->pcdata->strefa->wladca, wch->name ) )
		sprintf( buf + strlen( buf ), "W`ladc%s %s, ",
			wch->sex == 2 ? "zyni" : "a", POCHODZENIE( wch ) );
	    else if ( !str_cmp( wch->pcdata->strefa->namiestnik, wch->name ) )
		sprintf( buf + strlen( buf ), "Namiestni%s %s, ",
			wch->sex == 2 ? "czka" : "k", POCHODZENIE( wch ) );
	    else
		sprintf( buf + strlen( buf ), "Mieszkan%s %s, ",
			wch->sex == 2 ? "ka" : "iec", POCHODZENIE( wch ) );
	}

	sprintf( buf + strlen( buf ), "%s%s{x%s\n\r",
		wch->short_descr, wch->pcdata->title, IS_SET( wch->info, INF_KARTOGRAF ) ?
		wch->sex == 2 ? ", kartografka" : ", kartograf" : "" );

	sprintf( buf + strlen( buf ), "Poziom: %d  profesja: %s  rasa: %s%s%s%s  p`le`c: %s  wiek: %d\n\r",
			wch->level,
			class_table[ wch->klasa ].long_name,
			wch->race == zr_wampir
		     && wch->pcdata->wampiryzm.rasa != -1
		      ? race_table[ wch->pcdata->wampiryzm.rasa ].who_name
		      : "",
			wch->race == zr_wampir
		     && wch->pcdata->wampiryzm.rasa != -1
		      ? "-"
		      : "",
			race_table[ wch->race ].who_name,
			IS_AFFECTED( wch, AFF_POLYMORPH )
			 ? ( ( wch->sex == 1 )
			     ? " (przemieniony)"
			     : " (przemieniona)" )
			 : "",
			( wch->sex == 1 ) ? "m`eska"
		      : ( wch->sex == 2 ) ? "`ze`nska" : "nijaka",
			get_age( wch ) );

	if ( wch->pcdata->clan )
	    sprintf( buf + strlen( buf ), "%s jest %s %s.\n\r",
		wch->short_descr,
		wch->sex == SEX_FEMALE ?
		    wch->pcdata->clan->lev_name_zn[ wch->pcdata->clan_member->level ]
		  : wch->pcdata->clan->lev_name_mn[ wch->pcdata->clan_member->level ],
		wch->pcdata->clan->dop );

	wypelnij_poziomy( wch, poziomy );
	ile = 0;
	for ( i = 0; i < MAX_CLASS; i++ )
	    if ( poziomy[ i ] >= 100 )
		ile++;
	if ( ile > 0 )
	    sprintf( buf + strlen( buf ), "%s uko`nczy`l%s ju`z gr`e %d profesj%s.\n\r",
		wch->short_descr, wch->sex == 2 ? "a" : "", ile,
		ile == 1 ? "`a" : "ami" );

	if ( !IS_NPC( ch )
	  && ch->pcdata->learned[ gsn_enhanced_whois ] >= number_percent( )
	  && ( get_trust( ch ) >= 106 || get_trust( wch ) < 106 ) )
	{
	    sprintf( buf + strlen( buf ), "%s ma %d%% `zycia i %d%% many.\n\r",
		wch->short_descr,
		wch->max_hit  ? ( wch->hit  * 100 / wch->max_hit )  : 0,
		wch->max_mana ? ( wch->mana * 100 / wch->max_mana ) : 0 );
	    strcat( buf, "Kana`ly:" );
	    strcat( buf, !IS_SET( wch->deaf, CHANNEL_AUCTION )
		     ? " +AUKCJA    "
		     : " -aukcja    " );

	    strcat( buf, !IS_SET( wch->deaf, CHANNEL_CHAT )
		     ? " +GAW`EDA    "
		     : " -gaw`eda    " );

	    strcat( buf, !IS_SET( wch->deaf, CHANNEL_MUSIC )
		     ? " +`SPIEW     "
		     : " -`spiew     " );

	    strcat( buf, !IS_SET( wch->deaf, CHANNEL_SHOUT )
		     ? " +WRZASK    "
		     : " -wrzask    " );

	    strcat( buf, !IS_SET( wch->deaf, CHANNEL_YELL )
		     ? " +KRZYK\n\r"
		     : " -krzyk\n\r" );

	    strcat( buf, !IS_SET( wch->deaf, CHANNEL_GRATS )
		     ? "        +GRATULACJE"
		     : "        -gratulacje" );

	    strcat( buf, !IS_SET( wch->deaf, CHANNEL_FLAME )
		     ? " +KL`ATWA    "
		     : " -kl`atwa    " );

#if defined( IMUD )
	    strcat( buf, !IS_SET( wch->deaf, CHANNEL_IM )
		     ? " +IM        "
		     : " -im        " );
#endif

	    if ( IS_HERO( ch ) && IS_HERO( wch )
		&& is_authorized( get_char( ch ), ":" )
		&& is_authorized( get_char( wch ), ":" ) )
	    {
		strcat( buf, !IS_SET( wch->deaf, CHANNEL_HERO )
		     ? " +BOHATER   "
		     : " -bohater   " );
	    }

	    if ( get_trust( ch ) >= 103 && get_trust( wch ) >= 103
		&& is_authorized( get_char( ch ), ">" )
		&& is_authorized( get_char( wch ), ">" ) )

	    {
		strcat( buf, !IS_SET( wch->deaf, CHANNEL_IMMORTAL )
		     ? " +NIE`SMIERT "
		     : " -nie`smiert " );
	    }

	    if ( is_any_clan_member( ch ) )
	    {
		strcat( buf, !IS_SET( wch->deaf, CHANNEL_CLAN )
		     ? " +KLAN\n\r"
		     : " -klan\n\r" );
	    }
	    else
		strcat( buf, "\n\r" );

	    uzyj( ch, gsn_enhanced_whois, "szpiegowaniu" );
	}

	/* Lam 18.10.2000 */
	if ( wch->desc
	  && get_trust( ch ) >= 103
	  && is_authorized( get_char( ch ), "po`l`aczenia" ) )
	{
	    if ( wch->trust > wch->level ) /* Qwert */
		sprintf( buf + strlen( buf ), "Zaufanie: %d\n\r", wch->trust );
	    sprintf( buf + strlen( buf ), "Po`l`aczenie: {W%s@%s{x",
		wch->desc->user, wch->desc->host );
	    if ( *wch->desc->lsc_host )
		sprintf( buf + strlen( buf ), "  [%s@%s]",
			wch->desc->lsc_user, wch->desc->lsc_host );
	    sprintf( buf + strlen( buf ), "\n\rBezczynno`s`c: %d s (%s)\n\r",
		wch->desc->idle / 4, krotko_bezczynnosc( wch->desc->idle ) );
	}
    }

    if ( buf[ 0 ] == '\0' )
	send_to_char( "Nikt taki nie gra obecnie.\n\r", ch );
    else
	send_to_char( buf, ch );

    return;
}


KOMENDA( do_inventory )
{
    send_to_char( "Taszczysz ze sob`a:\n\r", ch );

    if ( *argument && !str_prefix( argument, "pojedynczo" ) )
	show_list_to_char( ch->carrying, ch, TRUE, TRUE, FALSE, TRUE, TRUE, 0, "" );
    else
	show_list_to_char( ch->carrying, ch, TRUE, TRUE, FALSE, TRUE, FALSE, 0, "" );

    return;
}


/*
 * Lam: "---" oraz pijanstwo
 */
KOMENDA( do_equipment )
{
    OBJ_DATA *obj;
    int       iWhere, iWear;
    char      buf2[ 20 ]; /* wystarczy do poziomu */

    send_to_char( "Aktualnie u`zywasz:\n\r", ch );
    for ( iWhere = 0; where_order[ iWhere ] != WEAR_NONE; iWhere++ )
    {
	iWear = where_order[ iWhere ];

	/* To zeby nie pokazywalo zarowno rak jak i dwoch broni.
	   "Lewa" reka moze byc albo z 2. bronia, albo cos trzymac, albo
	   trzymac swiatlo.
	   Pusta lewa reka jest 2. bronia, jesli postac umie dwie bronie,
	   a na dodatek nie ma na niej tarczy.
	   Lewa reka trzymajaca swiatlo to "uzywane jako swiatlo".
	   Kazdy inny przypadek (pusta czy nie) pokazuje "trzymane" */
	if ( iWear == WEAR_HOLD
	  && ( get_eq_char( ch, WEAR_WIELD_2 )
	    || get_eq_char( ch, WEAR_LIGHT )
	    || ( ( IS_NPC( ch ) || ch->pcdata->learned[ gsn_dual ] )
	      && !( get_eq_char( ch, WEAR_SHIELD )
		 || get_eq_char( ch, WEAR_HOLD )
		 || get_eq_char( ch, WEAR_LIGHT ) ) ) ) )
	    continue;
	if ( iWear == WEAR_WIELD_2
	  && ( ( !IS_NPC( ch ) && !ch->pcdata->learned[ gsn_dual ] )
	    || get_eq_char( ch, WEAR_SHIELD )
	    || get_eq_char( ch, WEAR_HOLD )
	    || get_eq_char( ch, WEAR_LIGHT ) ) )
	    continue;
	if ( iWear == WEAR_LIGHT
	  && !get_eq_char( ch, WEAR_LIGHT ) )
	    continue;

	send_to_char( where_name[ iWear ], ch );

	if ( !IS_NPC( ch )
	  && ( number_percent( ) < ch->pcdata->condition[ COND_DRUNK ] / 2
	   || ( IS_AFFECTED( ch, AFF_DEZORIENTACJA )
	     && number_percent( ) <= 20 ) ) )
	{
	    send_to_char( urojone_przedmioty[ number_bits( 5 ) ], ch );
	    if ( CZY_OPCJA( ch, OPC_SHOWLEVEL ) )
	    {
		sprintf( buf2, "  {y[%3d]{x", number_bits( 6 ) +
		    ch->level + ch->pcdata->condition[ COND_DRUNK ] );
		send_to_char( buf2, ch );
	    }
	    send_to_char( "\n\r", ch );
	    continue;
	}

	if ( !( obj = get_eq_char( ch, iWear ) ) )
	{
	    send_to_char( " ---\n\r", ch );
	    continue;
	}

	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE, FALSE, FALSE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	    send_to_char( "co`s\n\r", ch );
    }

    return;
}


KOMENDA( do_compare )
{
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    char     *msg;
    char      arg1[ MAX_INPUT_LENGTH ];
    char      arg2[ MAX_INPUT_LENGTH ];
    int       value1;
    int       value2;

    if ( ch->position == POS_FIGHTING )
    {
	STC( "No co ty, przecie`z ci`agle walczysz!\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' )
    {
	send_to_char( "Por`owna`c co z czym?\n\r", ch );
	return;
    }

    if ( !( obj1 = get_obj_carry( ch, arg1 ) ) )
    {
	send_to_char( "Nie masz tego ze sob`a.\n\r", ch );
	return;
    }

    if ( arg2[ 0 ] == '\0' )
    {
	for ( obj2 = ch->carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
	      && can_see_obj( ch, obj2 )
	      && obj1->item_type == obj2->item_type
	      && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE ) != 0 )
		break;
	}

	if ( !obj2 )
	{
	    send_to_char( "Nie nosisz na sobie niczego por`ownywalnego.\n\r", ch );
	    return;
	}
    }
    else if ( !( obj2 = get_obj_carry( ch, arg2 ) ) )
    {
	char new_arg2[ MIL ];
	char new_argt[ MIL ];
	int  number;

	/*  Strip off number argument, subtract one, paste it together */
	number = number_argument( arg2, new_argt );
	if ( number > 1 )
	    number--;
	sprintf( new_arg2, "%d.%s", number, new_argt );

	if ( !( obj2 = get_obj_wear( ch, new_arg2 ) ) )
	{
	    send_to_char( "Nie masz tego ze sob`a.\n\r", ch );
	    return;
	}
    }

    msg         = NULL;
    value1      = 0;
    value2      = 0;

    if ( obj1 == obj2 )
    {
	act( ZM_ZAWSZE, "Por`ownujesz $j z $k. Wygl`adaj`a dziwnie podobnie...", ch, obj1, obj2, TO_CHAR );
	return;
    }
    else if ( obj1->item_type != obj2->item_type
	   || ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE ) == 0 )
    {
	act( ZM_ZAWSZE, "Nie mo`zesz por`owna`c $g z $K.", ch, obj1, obj2, TO_CHAR );
	return;
    }
    else
	switch ( obj1->item_type )
	{
	    default:
		act( ZM_ZAWSZE, "Nie mo`zesz por`owna`c $g z $K.", ch, obj1, obj2, TO_CHAR );
		return;

	    case ITEM_ARMOR:
		value1 = obj1->value[ 0 ].v;
		value2 = obj2->value[ 0 ].v;
		break;

	    case ITEM_WEAPON:
		value1 = obj1->value[ 1 ].v + obj1->value[ 2 ].v;
		value2 = obj2->value[ 1 ].v + obj2->value[ 2 ].v;
		break;
	}

    if ( obj1->rodzaj == 3 )
    {
	if ( obj2->wear_loc != WEAR_NONE )
	{
	    if ( value1 == value2 )
		msg = "$p i $P (na tobie) wygl`adaj`a podobnie.";
	    else if ( value1  > value2 )
		msg = "$p wygl`adaj`a lepiej ni`z $P (na tobie).";
	    else
		msg = "$p wygl`adaj`a gorzej ni`z $P (na tobie).";
	}
	else
	{
	    if ( value1 == value2 )
		msg = "$p i $P wygl`adaj`a podobnie.";
	    else if ( value1  > value2 )
		msg = "$p wygl`adaj`a lepiej ni`z $P.";
	    else
		msg = "$p wygl`adaj`a gorzej ni`z $P.";
	}
    }
    else
    {
	if ( obj2->wear_loc != WEAR_NONE )
	{
	    if ( value1 == value2 )
		msg = "$p i $P (na tobie) wygl`adaj`a podobnie.";
	    else if ( value1  > value2 )
		msg = "$p wygl`ada lepiej ni`z $P (na tobie).";
	    else
		msg = "$p wygl`ada gorzej ni`z $P (na tobie).";
	}
	else
	{
	    if ( value1 == value2 )
		msg = "$p i $P wygl`adaj`a podobnie.";
	    else if ( value1  > value2 )
		msg = "$p wygl`ada lepiej ni`z $P.";
	    else
		msg = "$p wygl`ada gorzej ni`z $P.";
	}
    }

    act( ZM_ZAWSZE, msg, ch, obj1, obj2, TO_CHAR );

    /* Lam 24.2.2006 */
    if ( obj1->level <= ch->level
      && czy_pasuje( obj1, ch ) == NOTHING_WRONG )
    {
	if ( IS_AFFECTED( ch, AFF_DETECT_MAGIC )
	  && IS_OBJ_STAT( obj1, ITEM_MAGIC ) )
	    act( ZM_ZAWSZE, "Wyczuwasz magi`e zakl`et`a w $l.", ch, obj1, NULL, TO_CHAR );
	if ( obj1->pIndexData->affected
	  || obj1->affected )
	    act( ZM_ZAWSZE, "Za`lo`zenie $g wp`lynie dodatkowo na twe atrybuty.", ch, obj1, NULL, TO_CHAR );
    }
    else
	act( ZM_ZAWSZE, "Nie mo`zesz u`zywa`c $g, wi`ec nie dowiesz si`e wszystkiego.", ch, obj1, NULL, TO_CHAR );

    if ( obj2->level <= ch->level
      && czy_pasuje( obj2, ch ) == NOTHING_WRONG )
    {
	if ( IS_AFFECTED( ch, AFF_DETECT_MAGIC )
	  && IS_OBJ_STAT( obj2, ITEM_MAGIC ) )
	    act( ZM_ZAWSZE, "Wyczuwasz magi`e zakl`et`a w $l.", ch, obj2, NULL, TO_CHAR );
	if ( obj2->pIndexData->affected
	  || obj2->affected )
	    act( ZM_ZAWSZE, "Za`lo`zenie $g wp`lynie dodatkowo na twe atrybuty.", ch, obj2, NULL, TO_CHAR );
    }
    else
	act( ZM_ZAWSZE, "Nie mo`zesz u`zywa`c $g, wi`ec nie dowiesz si`e wszystkiego.", ch, obj2, NULL, TO_CHAR );

    return;
}


KOMENDA( do_credits )
{
    do_help( ch, "diku" );

    return;
}


/* Qwert 31.03.04: zamaskowany gracz jest "Gdzies w poblizu" */
KOMENDA( do_where )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             buf[ MAX_STRING_LENGTH ];
    char             arg[ MAX_INPUT_LENGTH  ];
    bool             found;

    if ( !check_blind( ch ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Gracze w pobli`zu:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
		&& ( victim = d->character )
		&& !IS_NPC( victim )
		&& victim->in_room
		&& victim->in_room->area == ch->in_room->area
		&& can_see( ch, victim ) )
	    {
		if ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_HOLYLIGHT )
		  && ch != victim
		  && is_affected( victim, gsn_disguise )
		  && number_percent( ) < ( victim->pcdata->learned[ gsn_disguise ] / 5 * 3 )
				+ 2 * ( victim->level - ch->level ) )
		{ /* false name nie wyswietlam, bo to gracz, a nie mob */
		    sprintf( buf, "%s gdzie`s w pobli`zu\n\r",
			    wyrownaj( victim->short_descr, -28 ) );
		}
		else
		{
		    sprintf( buf, "%s %s\n\r",
			    wyrownaj( victim->short_descr, -28 ),
			    victim->in_room->miejscownik );
		}
		found = TRUE;
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "Nikt.\n\r", ch );
    }
    else
    {
	if ( ( victim = get_char_area( ch, arg ) ) )
	{
	    if ( is_affected( victim, gsn_disguise )
	      && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_HOLYLIGHT )
	      && ch != victim
	      && number_percent( ) < ( victim->pcdata->learned[ gsn_disguise ] / 5 * 3 )
		     + 2 * ( victim->level - ch->level ) )
	    {
		sprintf( buf, "%s gdzie`s w pobli`zu\n\r",
			wyrownaj( PERS( victim, ch, FALSE ), -28 ) );
	    }
	    else
	    {
		sprintf( buf, "%s %s\n\r",
			wyrownaj( PERS( victim, ch, FALSE ), -28 ),
			victim->in_room->miejscownik );
	    }

	    send_to_char( buf, ch );
	}
	else
	    act( ZM_ZAWSZE, "Nie widzisz nikogo o imieniu \"$T\".", ch, NULL, arg, TO_CHAR );
    }

    return;
}


KOMENDA( do_consider )
{
    CHAR_DATA *victim;
    char      *buf = "";
    char      *msg;
    char       arg[ MAX_INPUT_LENGTH ];
    int        diff;
    int        hpmul;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Nad walk`a z kim chcesz si`e zastanowi`c?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	STC( "Nie b`edziesz mie`c problem`ow z zabiciem si`e nawet, je`sli sobie oddasz!\n\r", ch );
	return;
    }

    diff = victim->level - ch->level;

	 if ( diff <= -40 ) msg = "Zabijesz $# jednym palcem.";
    else if ( diff <= -20 ) msg = "Mo`zesz zabi`c $# bez `zadnego ekwipunku.";
    else if ( diff <= -10 ) msg = "Pokonasz $# bez wi`ekszego wysi`lku.";
    else if ( diff <=  -5 ) msg = "$N nie jest dla ciebie przeciwnikiem.";
    else if ( diff <=  -2 ) msg = "$N wygl`ada na `latwy cel.";
    else if ( diff <=   1 ) msg = "`Swietny wyb`or!";
    else if ( diff <=   4 ) msg = "$N potrafi si`e przed tob`a broni`c, ale mo`ze $U to nie wystarczy`c.";
/*  else if ( diff <=   9 ) msg = "Masz chyba wi`ecej szcz`e`scia ni`z z`eb`ow."; */
    else if ( diff <=   9 ) msg = "Pokonanie $! sprawi ci wiele trudno`sci.";
    else if ( diff <=  14 ) msg = "Je`sli nawet pokonasz $#, to ogromnym wysi`lkiem. Zastan`ow si`e, czy warto.";
    else if ( diff <=  19 ) msg = "$N bez problemu si`e z tob`a rozprawi.";
    else if ( diff <=  39 ) msg = "`Smier`c podzi`ekuje ci za tw`oj dar.";
    else                    msg = "Zginiesz zanim zd`a`zysz mrugn`a`c.";

    act( ZM_ZAWSZE, msg, ch, NULL, victim, TO_CHAR );

    /* Lam 7.12.2000: zmienione z roznicy na stosunek */
    if ( !victim->hit )
	return;

    hpmul = ch->hit * 100 / victim->hit;

    if ( ( ( diff > 1 ) && ( hpmul <= 100 ) )
      || ( ( diff < -1 ) && ( hpmul >= 100 ) ) )
	send_to_char( "Ponadto,", ch );
    else if ( diff >= -1 && diff <= 1 )
	send_to_char( "Pr`ocz tego,", ch );
    else
	send_to_char( "Jednak`ze,", ch );

    if ( hpmul >= 201 )
	buf = " jeste`s od $B du`zo silniejsz$0.";
    if ( hpmul <= 200 )
	buf = " jeste`s od $B silniejsz$0.";
    if ( hpmul <= 150 )
	buf = " jeste`s od $B nieco silniejsz$0.";
    if ( hpmul <= 120 )
	buf = " jeste`s od $B ciutk`e silniejsz$0.";
    if ( hpmul <= 100 )
	buf = " jeste`s od $B ciutk`e s`labsz$0.";
    if ( hpmul <= 80 )
	buf = " jeste`s od $B nieco s`labsz$0.";
    if ( hpmul <= 60 )
	buf = " jeste`s od $B s`labsz$0.";
    if ( hpmul <= 50 )
	buf = " jeste`s od $B du`zo s`labsz$0.";

    act( ZM_ZAWSZE, buf, ch, NULL, victim, TO_CHAR );

    if ( ch->race == victim->race )
	STC( "Jeste`scie tej samej rasy, powinni`scie sobie pomaga`c, a nie walczy`c.\n\r", ch );
    else if ( czy_grupa_wrogow( victim, ch ) )
	act( ZM_ZAWSZE, "$N jest twoim naturalnym wrogiem!", ch, NULL, victim, TO_CHAR );

    return;
}


void set_title( CHAR_DATA *ch, char *title )
{
    char buf[ MIL ];
    char buf2[ MIL ];

    if ( IS_NPC( ch ) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( ile_kolorow( title ) > 4 )
    {
	STC( "Tw`oj tytu`l zawiera`l wi`ecej ni`z cztery zmiany koloru. Odkolorowuj`e go.\n\r", ch );
	odkoloruj( title, buf2 );
	title = buf2;
    }

    buf[ 0 ] = '\0';

    if ( isalpha( (int) title[ 0 ] ) || isdigit( (int) title[ 0 ] )
      || ( title[ 0 ] == '{' ) || ( title[ 0 ] == '`' ) )
    {
	buf[ 0 ] = ' ';
	strcpy( buf + 1, title );
    }
    else
	strcpy( buf, title );

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );

    return;
}


KOMENDA( do_title )
{
    if ( IS_NPC( ch ) )
	return;

    smash_tilde( argument );

    if ( argument[ 0 ] == '\0' )
    {
	send_to_char( "Zmieni`c tytu`l na jaki?\n\r", ch );
	return;
    }

    if ( miodek_opisowy( ch, argument ) )
	return;

    /* niezbyt to madre, mozna wstrzyknac { na koniec */
    if ( strlen_pl( argument ) > 60 )
	argument[ 60 ] = '\0';

    set_title( ch, argument );
    send_to_char( "Zmieniasz sw`oj tytu`l.\n\r", ch );

    return;
}


KOMENDA( do_description )
{
    char buf[ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
	return;

    if ( argument[ 0 ] != '\0' )
    {
	buf[ 0 ] = '\0';
	smash_tilde( argument );
	if ( *argument == '-' )
	{
	    int last;
	    int pos;

	    if ( ch->description )
		strcpy( buf, ch->description );

	    if ( !ch->description || !buf[ 0 ] )
	    {
		send_to_char( "Nie masz sk`ad usuwa`c.\n\r", ch );
		return;
	    }

	    last = 0;
	    for ( pos = 0; buf[ pos ]; pos++ )
		if ( buf[ pos ] == '\r' && buf[ pos + 1 ] != '\0' )
		    last = pos;

	    buf[ last + 1 ] = '\0';
	    if ( !last )
		buf[ 0 ] = '\0';
	    free_string( ch->description );
	    ch->description = str_dup( buf );
	    if ( ch->sex == 2 )
		STC( "Usun`e`la`s linijk`e opisu.\n\r", ch );
	    else
		STC( "Usun`a`le`s linijk`e opisu.\n\r", ch );
	    return;
	}
	if ( argument[ 0 ] == '+' )
	{
	    if ( ch->description )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace( (int) *argument ) )
		argument++;
	}

	if ( strlen( buf ) + strlen( argument ) >=  MAX_STRING_LENGTH - 3 )
	{
	    send_to_char( "Za d`lugi opis.\n\r", ch );
	    return;
	}

	if ( miodek_opisowy( ch, argument ) )
	    return;

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    send_to_char( "Tw`oj opis:\n\r", ch );
    send_to_char( ( ch->description && *ch->description )
			? ch->description : "(Brak).\n\r", ch );
    send_to_char( "{xNapisz \"pomoc opis\", aby dowiedzie`c si`e, jak zmienia`c sw`oj opis.\n\r", ch );

    return;
}


KOMENDA( do_report )
{
    char buf[ MSL ];
    char wyj[ MSL ];
    char arg[ MIL ];

    argument = one_argument( argument, arg );
    if ( !arg[ 0 ]
      || !str_prefix( arg, "standard" ) )
	sprintf( buf, "%d/%d `z %d/%d m %d/%d r %d do`sw.",
	    ch->hit,  ch->max_hit,
	    ch->mana, ch->max_mana,
	    ch->move, ch->max_move,
	    ch->exp );
    else if ( !str_prefix( arg, "atrybuty" ) )
	sprintf( buf, "si`l: %d/%d  int: %d/%d  m`ad: %d/%d  zr`e: %d/%d  bud: %d/%d",
	    get_curr_str( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_STR
	     ? UMIN( 30, 25 + race_table[ ch->race ].str_mod )
	     : UMIN( 23 + race_table[ ch->race ].str_mod, 26 ),
	    get_curr_int( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_INT
	     ? UMIN( 30, 25 + race_table[ ch->race ].int_mod )
	     : UMIN( 23 + race_table[ ch->race ].int_mod, 26 ),
	    get_curr_wis( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_WIS
	     ? UMIN( 30, 25 + race_table[ ch->race ].wis_mod )
	     : UMIN( 23 + race_table[ ch->race ].wis_mod, 26 ),
	    get_curr_dex( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_DEX
	     ? UMIN( 30, 25 + race_table[ ch->race ].dex_mod )
	     : UMIN( 23 + race_table[ ch->race ].dex_mod, 26 ),
	    get_curr_con( ch ),
	    !IS_NPC( ch ) && class_table[ ch->klasa ].attr_prime == APPLY_CON
	     ? UMIN( 30, 25 + race_table[ ch->race ].con_mod )
	     : UMIN( 23 + race_table[ ch->race ].con_mod, 26 ) );
    else if ( !str_prefix( arg, "z`loto" ) )
    {
	sprintf( buf, "%s z`lota",
		formatuj_liczbe( ch->gold ) );
	if ( !IS_NPC( ch ) )
	    sprintf( buf + strlen( buf ), ", %s w banku",
		formatuj_liczbe( ch->pcdata->bank ) );
    }
    else if ( !IS_NPC( ch ) && !str_prefix( arg, "zab`ojstwa" ) )
    {
	int zab = liczba( ch->pcdata->killed );
	int zgon = liczba( ch->pcdata->dead );
	int wykr = liczba( ch->pcdata->wykrwawienia );
	int samob = liczba( ch->pcdata->samobojstwa );

	sprintf( buf, "%d zab`ojstw%s (%d gracz%s), %d zgon%s (%d z r`ak gracz%s, %d wykrwawie%s, %d samob`ojstw%s)",
		ch->pcdata->killed, zab == 1 ? "o" : zab == 2 ? "a" : "",
		ch->pcdata->pkilled, ch->pcdata->pkilled == 1 ? "a" : "y",
		ch->pcdata->dead, zgon == 1 ? "" : zgon == 2 ? "y" : "`ow",
		ch->pcdata->pdead, ch->pcdata->pdead == 1 ? "a" : "y",
		ch->pcdata->wykrwawienia,
		wykr == 1 ? "nie" : wykr == 2 ? "nia" : "`n",
		ch->pcdata->samobojstwa,
		samob == 1 ? "o" : samob == 2 ? "a" : "" );
    }
    else if ( !str_prefix( arg, "premie" ) )
    {
	if ( ch->level < 15 )
	{
	    send_to_char( "Jeszcze nie znasz swoich premii!\n\r", ch );
	    return;
	}
	if ( get_eq_char( ch, WEAR_WIELD_2 )
	  || ( ( IS_NPC( ch ) || ch->pcdata->learned[ gsn_dual ] )
	    && !( get_eq_char( ch, WEAR_SHIELD )
	      || get_eq_char( ch, WEAR_HOLD )
	      || get_eq_char( ch, WEAR_LIGHT ) ) ) )
	    sprintf( buf, "bro`n 1: PC %d, PO %d; bro`n 2: PC %d, PO %d",
		get_hitroll( ch, WEAR_WIELD ), get_damroll( ch, WEAR_WIELD ),
		get_hitroll( ch, WEAR_WIELD_2 ), get_damroll( ch, WEAR_WIELD_2 ) );
	else
	    sprintf( buf, "premia celno`sci: %d, premia obra`ze`n: %d",
		get_hitroll( ch, WEAR_WIELD ), get_damroll( ch, WEAR_WIELD ) );
    }
    else if ( !str_prefix( arg, "wiek" ) )
    {
	int pzl = liczba( ( ch->played + (int) ( current_time - ch->logon ) ) / 3600 );

	sprintf( buf, "wiek: %d lat%s (%d godzin%s)",
	    get_age( ch ),
	    liczba( get_age( ch ) ) == 2 ? "a" : "", /* 1 niemozliwe */
	    ( ( ch->played + (int) (current_time - ch->logon ) ) /3600 ),
	    pzl == 1 ? "a" : pzl == 2 ? "y" : "" );
    }
    else if ( !str_prefix( arg, "ochrona" ) )
    {
	if ( ch->level < 25 )
	{
	    send_to_char( "Jeszcze nie znasz swojej klasy pancerza ani ochrony przed czarami!\n\r", ch );
	    return;
	}
	sprintf( buf, "KP: %d, MKP: %d, OPC: %d",
	    GET_AC( ch ), GET_MAC( ch ), GET_ST( ch ) );
    }
    else
    {
	send_to_char( "Pierwszy parametr musi by`c jednym z nast`epuj`acych:\n\r", ch );
	send_to_char( "  standard atrybuty z`loto zab`ojstwa premie wiek ochrona\n\r", ch );
	return;
    }

    if ( !*argument )
    {
	if ( PP( ch ) )
	{
	    STC( "Poza postaci`a musisz poda`c dodatkowy argument.\n\r", ch );
	    return;
	}

	if ( IS_MUTE( ch )
	  || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE )
	  || ( !IS_NPC( ch ) && ( IS_SET( ch->act, PLR_SILENCE )
			       || IS_SET( ch->act, PLR_NO_TELL ) ) ) )
	{
	    send_to_char( "Ruszasz wargami, ale nie s`lycha`c d`xwi`eku.\n\r", ch );
	    return;
	}

	sprintf( wyj, "{qPodajesz: %s{x\n\r", buf );
	send_to_char( wyj, ch );
	act( ZM_SLUCH, "{q$n podaje: $t{x", ch, buf, NULL, TO_ROOM );
    }
    else
    {
	arg[ 0 ] = ch->separator;
	arg[ 1 ] = '\0';
	if ( strstr( argument, arg ) )
	{
	    send_to_char( "Albo cwaniakujesz, albo co`s `xle robisz.\n\r", ch );
	    return;
	}

	if ( strlen( argument ) > 30 )
	{
	    send_to_char( "Nie przesadzaj z d`lugo`sci`a polecenia.\n\r", ch );
	    return;
	}

	buf[ MIL - 31 ] = '\0'; /* PZU */

	if ( ch->desc )
	{
	    ch_printf( ch, "{GWykonuj`e:{x %s %s\n\r", argument, buf );
	    sprintf( wyj, "&%s %s", argument, buf );
	    interpret( ch, wyj, INTERP_NORMAL );
	}
    }

    return;
}


KOMENDA( do_practice )
{
    char buf[ MAX_STRING_LENGTH ];
    char buf1[ MAX_SKILL * 52 ]; /* wyszlo mi, ze powinno zawsze wystarczyc */
    int poziomy[ MAX_CLASS ];
    int sn;
    int money = WYDATEK( ch->level, 20 );
    int licz;
    int adept;
    int i;
    bool nie_ma;

    /* nizej jest troszke obslugi cwiczenia przez moby, ale gdyby ktos chcial
       to odblokowac, jest wiele miejsc w tej funkcji, ktore tego nie
       uwzgledniaja */
    if ( IS_NPC( ch ) )
	return;

    buf1[ 0 ] = '\0';

    if ( ch->level < 3 )
    {
	send_to_char(
	    "`Zeby `cwiczy`c, musisz mie`c trzeci poziom. Na razie id`x trenowa`c!\n\r",
	    ch );
	return;
    }

    wypelnij_poziomy( ch, poziomy );

    if ( ch->level <= 5 )
	money = ch->level * 20;

    if ( argument[ 0 ] == '\0' )
    {
	CHAR_DATA *mob;
	int col;
	bool bylo;
	SORTED_SKILL_DATA *tss;

	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
	    if ( mob->deleted )
		continue;
	    if ( IS_NPC( mob ) && IS_AWAKE( mob )
	      && IS_SET( mob->act, ACT_PRACTICE ) )
		break;
	}

	col = 0;
	bylo = FALSE;
/*      for ( sn = 0; sn < MAX_SKILL; sn++ ) */
	for ( tss = sorted_skill_list; tss; tss = tss->next )
	{
	    sn = tss->sn;

	    if ( ch->pcdata->learned[ sn ] )
	    {
		if ( !bylo )
		{
		    strcat( buf1, "{gZnasz umiej`etno`sci:{x\n\r" );
		    bylo = TRUE;
		}

		nie_ma = TRUE;
		if ( ch->level >= L_APP && ch->level < L_JUN ) /* restartowcy */
		{
		    for ( i = 0; i < MAX_CLASS; i++ )
			if ( poziomy[ i ] >= 100
			  && skill_table[ sn ].skill_level[ i ] <= ch->level )
			    break;

		    /* ukonczyl gre profesja, ktora ma te umiejetnosc */
		    if ( i != MAX_CLASS )
			nie_ma = FALSE;
		}

		if ( nie_ma
		  && ch->level < skill_table[ sn ].skill_level[ ch->klasa ] )
		    sprintf( buf, "{W%s* %3d%%{x",
			wyrownaj( skill_table[ sn ].pl_name, 19 ),
			ch->pcdata->learned[ sn ] );
		else
		    sprintf( buf, "%s %3d%%",
			wyrownaj( skill_table[ sn ].pl_name, 20 ),
			ch->pcdata->learned[ sn ] );
		strcat( buf1, buf );
		if ( ++col % 3 == 0 )
		    strcat( buf1, "\n\r" );
		else
		    strcat( buf1, " " );
	    }
	}

	if ( col % 3 != 0 )
	    strcat( buf1, "\n\r" );

	if ( mob )
	{
	    col = 0;
	    bylo = FALSE;
	    for ( tss = sorted_skill_list; tss; tss = tss->next )
	    {
		sn = tss->sn;

		adept = IS_NPC( ch ) ? 100 : class_table[ ch->klasa ].skill_adept;

		if ( skill_table[ sn ].to_practice )
		    adept = skill_table[ sn ].to_practice;

		if ( ch->pcdata->learned[ sn ] >= adept )
		    continue;

		nie_ma = TRUE;

		if ( ch->level >= L_APP && ch->level < L_JUN ) /* restartowcy */
		{
		    for ( i = 0; i < MAX_CLASS; i++ )
			if ( poziomy[ i ] >= 100
			  && skill_table[ sn ].skill_level[ i ] <= ch->level )
			    break;

		    /* ukonczyl gre profesja, ktora ma te umiejetnosc */
		    if ( i != MAX_CLASS )
			nie_ma = FALSE;
		}

		if ( nie_ma )
		{
		    if ( ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
		      && ch->level < skill_table[ sn ].multi_level[ ch->klasa ] )
			continue;

		    if ( ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
		      && !ma_w_multi( ch, sn ) )
			continue;
		}

		if ( !bylo )
		{
		    strcat( buf1, "{gMo`zesz si`e tu uczy`c umiej`etno`sci:{x\n\r" );
		    bylo = TRUE;
		}

		if ( nie_ma
		 && ch->level < skill_table[ sn ].skill_level[ ch->klasa ] )
		    sprintf( buf, "{W%s* %3d%%{x",
			wyrownaj( skill_table[ sn ].pl_name, 19 ),
			ch->pcdata->learned[ sn ] );
		else
		    sprintf( buf, "%s %3d%%",
			wyrownaj( skill_table[ sn ].pl_name, 20 ),
			ch->pcdata->learned[ sn ] );
		strcat( buf1, buf );
		if ( ++col % 3 == 0 )
		    strcat( buf1, "\n\r" );
		else
		    strcat( buf1, " " );
	    }

	    if ( col % 3 != 0 )
		strcat( buf1, "\n\r" );
	}

	licz = liczba( ch->practice );
	sprintf( buf, "{yMasz %d sesj%s `cwicze`n.{x\n\r",
		ch->practice,
		licz == 1 ? "`e"
		 : licz == 2 ? "e" : "i" );
	strcat( buf1, buf );
	sprintf( buf, "Za `cwiczenie p`lacisz %s.\n\r", ZLOTO_B( money ) );
	strcat( buf1, buf );
	/* Lam 7.7.98: ponizsze na wniosek Nopa */
	licz = liczba( ch->gold );
	sprintf( buf, "Two%s %d monet%s wystarcz%s",
		licz == 1 ? "ja" : licz == 2 ? "je" : "ich",
		ch->gold,
		licz == 1 ? "a" : licz == 2 ? "y" : "",
		licz == 2 ? "`a" : "y" );
	strcat( buf1, buf );
	licz = liczba( ch->gold / money );
	sprintf( buf, " na %d `cwicze%s.\n\r",
		ch->gold / money,
		licz == 1 ? "nie" : licz == 2 ? "nia" : "`n" );
	strcat( buf1, buf );
	send_to_char( buf1, ch );
    }
    else
    {
	CHAR_DATA *mob;

	if ( !IS_AWAKE( ch ) )
	{
	    send_to_char( "W czasie snu?\n\r", ch );
	    return;
	}

	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
	    if ( mob->deleted )
		continue;
	    if ( IS_NPC( mob ) && IS_AWAKE( mob )
	      && IS_SET( mob->act, ACT_PRACTICE ) )
		break;
	}

	if ( !mob )
	{
	    send_to_char( "Tu nie mo`zesz tego robi`c.\n\r", ch );
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    send_to_char( "Nie masz ju`z sesji `cwicze`n.\n\r", ch );
	    return;
	}
	else if ( money > ch->gold )
	{
	    send_to_char( "Brakuje ci pieni`edzy na `cwiczenie.\n\r", ch );
	    return;
	}

	if ( ( sn = skill_lookup_pl( argument ) ) < 0 )
	{
	    STC( "Takiej umiej`etno`sci nie znam.\n\r", ch );
	    return;
	}

	nie_ma = TRUE;
	if ( ch->level >= L_APP && ch->level < L_JUN ) /* restartowcy */
	{
	    for ( i = 0; i < MAX_CLASS; i++ )
		if ( poziomy[ i ] >= 100
		  && skill_table[ sn ].skill_level[ i ] <= ch->level )
		    break;

	    /* ukonczyl gre profesja, ktora ma te umiejetnosc */
	    if ( i != MAX_CLASS )
		nie_ma = FALSE;
	}

	if ( !IS_NPC( ch )
	  && nie_ma
	  && ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
	  && ( ch->level < skill_table[ sn ].multi_level[ ch->klasa ]
	    || !ma_w_multi( ch, sn ) ) )
	{
	    sprintf( buf, "Nie mo`zesz `cwiczy`c umiej`etno`sci \"%s\".\n\r",
		skill_table[ sn ].pl_name );
	    send_to_char( buf, ch );
	    return;
	}

	adept = IS_NPC( ch ) ? 100 : class_table[ ch->klasa ].skill_adept;

	/* niektore umiejki mozna cwiczyc nie do 75%, tylko do: */
	if ( skill_table[ sn ].to_practice ) /* jesli 0, to 75 */
	    adept = skill_table[ sn ].to_practice;

	if ( skill_table[ sn ].needed && skill_table[ sn ].needed[ 0 ] != '\0' )
	{
	    int needsn;
	    if ( ( needsn = skill_lookup_pl( skill_table[ sn ].needed ) ) < 0 )
		bug( "Skill %d wymaga nieistniejacej umiejetnosci", sn );
	    else if ( ch->pcdata->learned[ needsn ] < adept )
	    {
		sprintf( buf, "Najpierw musisz zosta`c adept%s umiej`etno`sci \"%s\".\n\r",
		    ch->sex == 2 ? "k`a" : "em",
		    skill_table[ needsn ].pl_name );
		send_to_char( buf, ch );
		return;
	    }
	}

	if ( ch->pcdata->learned[ sn ] >= adept )
	{
	    sprintf( buf, "Ju`z jeste`s adept%s umiej`etno`sci \"%s\".\n\r",
		ch->sex == 2 ? "k`a" : "em",
		skill_table[ sn ].pl_name );
	    send_to_char( buf, ch );
	}
	else
	{
	    ch->practice--;
	    ch->gold -= money;
	    if ( !ch->pcdata->learned[ sn ] )
		ch->pcdata->learned_level[ sn ] = UMIN( ch->level, L_APP );
	    if ( nie_ma && ch->level < skill_table[ sn ].skill_level[ ch->klasa ] )
		ch->pcdata->learned[ sn ] += int_app[ get_curr_int( ch ) ].learn / 3;
	    else
		ch->pcdata->learned[ sn ] += int_app[ get_curr_int( ch ) ].learn;
	    if ( ch->pcdata->learned[ sn ] < adept )
	    {
		act( ZM_ZAWSZE, "`Cwiczysz umiej`etno`s`c \"$T\".",
		    ch, NULL, skill_table[ sn ].pl_name, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_DYLEMAT,
		    "$n `cwiczy umiej`etno`s`c \"$T\".",
		    ch, NULL, skill_table[ sn ].pl_name, TO_ROOM );
	    }
	    else
	    {
		ch->pcdata->learned[ sn ] = adept;
		if ( ch->sex == 2 )
		{
		    act( ZM_ZAWSZE, "Sta`la`s si`e adeptk`a umiej`etno`sci \"$T\".",
			ch, NULL, skill_table[ sn ].pl_name, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_DYLEMAT,
			"$n sta`la si`e adeptk`a umiej`etno`sci \"$T\".",
			ch, NULL, skill_table[ sn ].pl_name, TO_ROOM );
		}
		else
		{
		    act( ZM_ZAWSZE, "Sta`le`s si`e adeptem umiej`etno`sci \"$T\".",
			ch, NULL, skill_table[ sn ].pl_name, TO_CHAR );
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_DYLEMAT,
			"$n sta`l si`e adeptem umiej`etno`sci \"$T\".",
			ch, NULL, skill_table[ sn ].pl_name, TO_ROOM );
		}
	    }
	}
    }

    return;
}


/*
 * 'Wimpy' originally by Dionysos.
 */
KOMENDA( do_wimpy )
{
    char buf[ MAX_STRING_LENGTH ];
    char arg[ MAX_INPUT_LENGTH  ];
    int wimpy;
    int l;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Twoja odwaga przewy`zsza rozwag`e!\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit )
    {
	ASTC( "Sta`l$o`s si`e potwornym tch`orzem!", ch );
	return;
    }

    if ( wimpy == ch->wimpy )
    {
	l = liczba( wimpy );
	sprintf( buf, "Ju`z masz tch`orza ustawionego na %d punkt%s `zycia.\n\r",
		wimpy, l == 2 ? "y" : l == 1 ? "" : "`ow" );
	send_to_char( buf, ch );
	return;
    }

    l = liczba( wimpy );
    sprintf( buf, "Tch`orz przestawiony z %d na %d punkt%s `zycia.\n\r",
	ch->wimpy, wimpy,
	l == 2 ? "y" : l == 1 ? "" : "`ow" );
    send_to_char( buf, ch );
    ch->wimpy = wimpy;

    return;
}


KOMENDA( do_hasl )
{
    send_to_char( "W celu zmiany has`la napisz HAS`LO w ca`lo`sci.\n\r", ch );
 
    return;
}


/*
 * Lam 18.6.98: nowa funkcja do zmiany hasla
 */
KOMENDA( do_haslo )
{
    if ( IS_NPC( ch ) || !ch->desc )
	return;

    if ( !can_quit_now( ch, FALSE ) ) return;

/*    if ( ch->level > 1 ) */
    save_char_obj( ch );

    ch->desc->connected = CON_GET_OLD_PWD_PWD;
    send_to_char( "Podaj swoje stare has`lo: ", ch );
    send_to_char( (char *) echo_off_str, ch );

    ch->pcdata->was_in_room = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );

    return;
}


/*
 * Contributed by Alander.
 * Lam 10.1.2003: z argumentem szuka polecen
 */
KOMENDA( do_commands )
{
    char buf [ MAX_STRING_LENGTH ];
    char buf1[ MAX_STRING_LENGTH ];
    int  cmd;
    int  col;

    buf1[ 0 ] = '\0';
    col = 0;
    if ( !*argument )
    {
	for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	{
	    if ( cmd_table[ cmd ].level <  LEVEL_HERO
	      && cmd_table[ cmd ].level <= get_trust( ch )
	      && !IS_SET( cmd_table[ cmd ].restr, CMD_NOTLISTED ) )
	    {
		sprintf( buf, "%s", wyrownaj( cmd_table[ cmd ].name, -16 ) );
		strcat( buf1, buf );
		if ( ++col % 5 == 0 )
		    strcat( buf1, "\n\r" );
	    }
	}

	if ( col % 5 != 0 )
	    strcat( buf1, "\n\r" );

	send_to_char( buf1, ch );
    }
    else
    {
	for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	{
	    if ( !str_prefix( argument, cmd_table[ cmd ].name )
	      && cmd_table[ cmd ].level <  LEVEL_HERO
	      && cmd_table[ cmd ].level <= get_trust( ch )
	      && !IS_SET( cmd_table[ cmd ].restr, CMD_NOTLISTED ) )
	    {
		sprintf( buf, "%s", wyrownaj( cmd_table[ cmd ].name, -16 ) );
		strcat( buf1, buf );
		if ( ++col % 5 == 0 )
		    strcat( buf1, "\n\r" );
	    }
	}

	if ( !*buf1 )
	    strcat( buf1, "Skr`ot nie pasuje do `zadnych polece`n.\n\r" );

	if ( col % 5 != 0 )
	    strcat( buf1, "\n\r" );

	send_to_char( buf1, ch );
    }

    return;
}


KOMENDA( do_channels )
{
    char arg[ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
	{
	    send_to_char( "Nie wolno ci u`zywa`c kana`l`ow.\n\r", ch );
	    return;
	}

	send_to_char( "Kana`ly:", ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_AUCTION )
		     ? " +AUKCJA    "
		     : " -aukcja    ",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_CHAT )
		     ? " +GAW`EDA    "
		     : " -gaw`eda    ",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_MUSIC )
		     ? " +`SPIEW     "
		     : " -`spiew     ",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_SHOUT )
		     ? " +WRZASK    "
		     : " -wrzask    ",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_YELL )
		     ? "+KRZYK\n\r"
		     : "-krzyk\n\r",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_GRATS )
		     ? "        +GRATULACJE"
		     : "        -gratulacje",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_FLAME )
		     ? " +KL`ATWA    "
		     : " -kl`atwa    ",
		     ch );

#if defined( IMUD )
	send_to_char( !IS_SET( ch->deaf, CHANNEL_IM )
		     ? " +IM        "
		     : " -im        ",
		     ch );
#endif

	if ( IS_HERO( ch ) && is_authorized( get_char( ch ), ":" ) )
	{
	    send_to_char( !IS_SET( ch->deaf, CHANNEL_HERO )
		     ? " +BOHATER   "
		     : " -bohater   ",
		     ch );
	}

	if ( get_trust( ch ) >= 103 && is_authorized( get_char( ch ), ">" ) )
	{
	    send_to_char( !IS_SET( ch->deaf, CHANNEL_IMMORTAL )
		     ? " +NIE`SMIERT "
		     : " -nie`smiert ",
		     ch );
	}

	if ( is_any_clan_member( ch ) )
	{
	    send_to_char( !IS_SET( ch->deaf, CHANNEL_CLAN )
		     ? " +KLAN      "
		     : " -klan      ",
		     ch );
	}

	send_to_char( "\n\r", ch );
    }
    else
    {
	int  bit;
	bool fClear;

	     if ( arg[ 0 ] == '+' ) fClear = TRUE;
	else if ( arg[ 0 ] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Napisz czy chcesz w`l`aczy`c(+) czy wy`l`aczy`c(-) kana`l?\n\r", ch );
	    return;
	}

	     if ( !str_prefix( arg + 1, "aukcja"	) ) bit = CHANNEL_AUCTION;
	else if ( !str_prefix( arg + 1, "gaw`eda"	) ) bit = CHANNEL_CHAT;
	else if ( !str_prefix( arg + 1, "im"		) ) bit = CHANNEL_IM;
	else if ( !str_prefix( arg + 1, "bohater"	) ) bit = CHANNEL_HERO;
	else if ( !str_prefix( arg + 1, "nie`smiert"	) ) bit = CHANNEL_IMMORTAL;
	else if ( !str_prefix( arg + 1, "`spiew"	) ) bit = CHANNEL_MUSIC;
	else if ( !str_prefix( arg + 1, "krzyk"		) ) bit = CHANNEL_YELL;
	else if ( !str_prefix( arg + 1, "wrzask"	) ) bit = CHANNEL_SHOUT;
	else if ( !str_prefix( arg + 1, "gratulacje"	) ) bit = CHANNEL_GRATS;
	else if ( !str_prefix( arg + 1, "kl`atwa"	) ) bit = CHANNEL_FLAME;
	else if ( !str_prefix( arg + 1, "klan"		) ) bit = CHANNEL_CLAN;
	else if ( czy_wszystko( arg + 1 ) )
	{
	    if ( fClear )
	    {
		STC( "W`l`aczasz wszystkie kana`ly.\n\r", ch );
		ch->deaf = 0;
	    }
	    else
	    {
		STC( "Wy`l`aczasz wszystkie kana`ly.\n\r", ch );
		/* zeby ujemne nie wskakiwaly w plik postaci: */
		ch->deaf = (unsigned) b31 - 1;
	    }
	    return;
	}
	else
	{
	    send_to_char( "Kt`ory kana`l w`l`aczy`c lub wy`l`aczy`c?\n\r", ch );
	    return;
	}

	if ( fClear )
	    REMOVE_BIT ( ch->deaf, bit );
	else
	    SET_BIT    ( ch->deaf, bit );

	send_to_char( "Dobrze.\n\r", ch );
    }

    return;
}


KOMENDA( do_polskie )
{
    static char *polskie[ 5 ] =
    {
	"brak",
	"dos-latin-2",
	"iso-8859-2",
	"win-1250",
	"utf-8"
    };

    char arg[ MAX_INPUT_LENGTH ];
    int standrd;
    int i;

    one_argument( argument, arg );

    ch->polskie = URANGE( 0, ch->polskie, 4 );

    if ( arg[ 0 ] == '\0' )
    {
	if ( ch->polskie == 0 )
	    ch_printf( ch, "Nie widzisz polskich liter.\n\r" );
	else
	    ch_printf( ch, "Polskie litery widzisz w standardzie: %s\n\r",
		polskie[ ch->polskie ] );
	return;
    }

    standrd = atoi ( arg );

    for ( i = 0; i < 5; i++ )
	if ( !str_prefix( arg, polskie[ i ] ) )
	    standrd = i;

    if ( standrd < 1 || standrd > 4 )
    {
	ch->polskie = 0;
	if ( ch->desc )
	    ch->desc->polskie = 0;
	STC( "Nie widzisz polskich liter.\n\r", ch );
	return;
    }

    ch->polskie = standrd;
    if ( ch->desc )
	ch->desc->polskie = standrd;
    ch_printf( ch, "P`o`xn`a noc`a jasno`s`c ksi`e`zyca ol`sni`la gra`n.\n\r"
		"P`O`XN`A NOC`A JASNO`S`C KSI`E`ZYCA OL`SNI`LA GRA`N. (standard: %s)\n\r",
		polskie[ ch->polskie ] );
    return;
}


/*
 * Contributed by Grodyn.
 * Zmiany Lama.
 * konfigi[ ] - 21.5.98
 */
KOMENDA( do_config )
{
static struct
{
    char *opcja;
    int bit;
} konfigi[ ] =
{
    { "wyj`scia",	OPC_AUTOEXIT },
    { "rabowanie",	OPC_AUTOLOOT },
    { "z`loto",		OPC_AUTOGOLD },
    { "`swi`ecenie",	OPC_AUTOSAC },
    { "linia",		OPC_BLANK },
    { "skr`ot",		OPC_BRIEF },
    { "zach`eta",	OPC_PROMPT },
/*  { "telnetga",	OPC_TELNET_GA }, */
    { "podzial",	OPC_AUTOSPLIT },
    { "czas",		OPC_SHOWTICK },
    { "poziom",		OPC_SHOWLEVEL },
    { "zawijanie",	OPC_WRAP },
    { "aliassep",	OPC_NO_SEPARATOR },
    { "cutcp",		OPC_CUTCP },
    { "podpowied`x",	OPC_HINT },
    { "uniki",		OPC_UNIKI },
    { "chybianie",	OPC_CHYBIANIE },
    { "cisza",		OPC_CISZA },
    { "wchodzenie",	OPC_WCHODZENIE },
    { "angielskie",	OPC_ANGIELSKIE },
#if defined( MAM_AUTOKLATWE )
    { "autokl`atwa",	OPC_AUTOKLATWA },
#endif
    { "pojemniki",	OPC_POJEMNIKI },
    { "gwiazdki",	OPC_GWIAZDKI },
    { "autowciel",	OPC_AUTOWCIEL },
    { "ruch",		OPC_RUCH },
    { "czaspp",		OPC_CZASPP },
    { "", 0 }
};
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    bool zaawansowane;

    if ( IS_NPC( ch ) )
    {
	STC( "Moby nie s`a konfigurowalne.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( !*arg || !str_prefix( arg, "zaawansowane" ) )
    {
	if ( *arg )
	    zaawansowane = TRUE;
	else
	    zaawansowane = FALSE;

	strcpy( buf, "{Y[ Opcja     ] {GZnaczenie{x\n\r" );

	if ( zaawansowane )
	{
	    strcat( buf, CZY_OPCJA( ch, OPC_NO_SEPARATOR )
		? "{y[+ALIASSEP  ]{x Separator dzia`la ci tylko w aliasach.\n\r"
		: "{y[-aliassep  ]{x Separator dzia`la ci normalnie.\n\r" );

	    strcat( buf, CZY_OPCJA( ch, OPC_ANGIELSKIE )
		? "{y[+ANGIELSKIE]{x Widzisz podpowiedzi o polskich odpowiednikach angielskich polece`n.\n\r"
		: "{y[-angielskie]{x Nie chcesz s`lucha`c o polskich zamiennikach angielskich polece`n.\n\r" );
	}

#if defined( MAM_AUTOKLATWE )
	strcat( buf, CZY_OPCJA( ch, OPC_AUTOKLATWA )
	    ? "{y[+AUTOKL`ATWA]{x Chcesz, aby niecenzuralne wypowiedzi zmienia`ly kana`l na kl`atw`e.\n\r"
	    : "{y[-autokl`atwa]{x Chcesz, aby twoje niecenzuralne gaw`edzenie by`lo blokowane.\n\r" );
#endif

	if ( zaawansowane )
	    strcat( buf, CZY_OPCJA( ch, OPC_AUTOWCIEL )
		? "{y[+AUTOWCIEL ]{x Twoja posta`c wciela si`e automatycznie przy wej`sciu do gry.\n\r"
		: "{y[-autowciel ]{x Zaczynasz gr`e poza postaci`a.\n\r" );

	strcat( buf, !IS_SET( ch->act, PLR_NO_EMOTE )
	    ? ""
	    : "{R[+BEZEMOCJI ]{x Nie mo`zesz u`zywac polecenia emocja.\n\r" );

	strcat( buf, !IS_SET( ch->act, PLR_NO_COLOUR )
	    ? ""
	    : "{R[+BEZKOLORU ]{x Nie mo`zesz u`zywa`c kolor`ow.\n\r" );

	strcat( buf, !IS_SET( ch->act, PLR_NO_TELL )
	    ? ""
	    : "{R[+BEZMOWY   ]{x Nie mo`zesz si`e porozumiewac.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_CHYBIANIE )
	    ? "{y[+CHYBIANIE ]{x Nie widzisz chybionych cios`ow.\n\r"
	    : "{y[-chybianie ]{x Widzisz chybione ciosy.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_CISZA )
	    ? "{y[+CISZA     ]{x Nie pozwalasz si`e bipa`c.\n\r"
	    : "{y[-cisza     ]{x Nie przeszkadza ci bycie bipanym.\n\r" );

	if ( zaawansowane )
	    strcat( buf, CZY_OPCJA( ch, OPC_CUTCP )
		? "{y[+CUTCP     ]{x Jeste`s nieszcz`e`sliwym u`zytkownikiem CUTCP.\n\r"
		: "{y[-cutcp     ]{x Na szcz`e`scie nie u`zywasz CUTCP.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_SHOWTICK )
	    ? "{y[+CZAS      ]{x Widzisz up`lyw czasu.\n\r"
	    : "{y[-czas      ]{x Nie widzisz up`lywu czasu.\n\r" );

	if ( zaawansowane )
	    strcat( buf, CZY_OPCJA( ch, OPC_CZASPP )
		? "{y[+CZASPP    ]{x Widzisz czas w zach`ecie PP.\n\r"
		: "{y[-czaspp    ]{x Nie widzisz czasu w zach`ecie PP.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_GWIAZDKI )
	    ? "{y[+GWIAZDKI  ]{x Obra`zenia s`a ci prezentowane gwiazdkami.\n\r"
	    : "{y[-gwiazdki  ]{x Nie widzisz gwiazdek ilustruj`acych obra`zenia.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_BLANK )
	    ? "{y[+LINIA     ]{x Masz pust`a lini`e przed zach`et`a.\n\r"
	    : "{y[-linia     ]{x Nie masz pustej linii przed zach`et`a.\n\r" );

	if ( zaawansowane )
	    strcat( buf, CZY_OPCJA( ch, OPC_MIW )
		? "{y[+ONW       ]{x U`zywasz okna na wiadomo`sci.\n\r"
		: "{y[-onw       ]{x Nie u`zywasz okna na wiadomo`sci.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_HINT )
	    ? "{y[+PODPOWIED`X]{x Widzisz podpowiedzi.\n\r"
	    : "{y[-podpowied`x]{x Nie chcesz, `zeby ci podpowiadano.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_AUTOSPLIT )
	    ? "{y[+PODZIA`L   ]{x Automatycznie rozdzielasz z`loto na cz`lonk`ow grupy.\n\r"
	    : "{y[-podzia`l   ]{x Nie rozdzielasz z`lota automatycznie.\n\r" );

	if ( zaawansowane )
	    strcat( buf, CZY_OPCJA( ch, OPC_POJEMNIKI )
		? "{y[+POJEMNIKI ]{x Pozostawiasz sobie pojemniki po wypitych napojach.\n\r"
		: "{y[-pojemniki ]{x Po`swi`ecasz pojemniki po wypitych napojach.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_SHOWLEVEL )
	    ? "{y[+POZIOM    ]{x Widzisz poziom przedmiot`ow.\n\r"
	    : "{y[-poziom    ]{x Nie widzisz poziomu przedmiot`ow.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_AUTOLOOT )
	    ? "{y[+RABOWANIE ]{x Automatycznie wyci`agasz rzeczy z cia`l.\n\r"
	    : "{y[-rabowanie ]{x Nie przetrz`asasz cia`l automatycznie.\n\r" );

	if ( zaawansowane )
	    strcat( buf, CZY_OPCJA( ch, OPC_RUCH )
		? "{y[+RUCH      ]{x Nie otrzymujesz komunikat`ow odnawiania punkt`ow ruchu.\n\r"
		: "{y[-ruch      ]{x Widzisz, kiedy osi`agasz maksimum punkt`ow ruchu.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_BRIEF )
	    ? "{y[+SKR`OT     ]{x Nie widzisz opisu przy wchodzeniu do pomieszczenia.\n\r"
	    : "{y[-skr`ot     ]{x Widzisz opis pomieszczenia gdy do niego wchodzisz.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_AUTOSAC )
	    ? "{y[+`SWI`ECENIE ]{x Automatycznie po`swi`ecasz cia`la ofiar bez ekwipunku.\n\r"
	    : "{y[-`swi`ecenie ]{x Nie po`swi`ecasz cia`l automatycznie.\n\r" );

/*	if ( zaawansowane )
	    strcat( buf, CZY_OPCJA( ch, OPC_TELNET_GA )
		? "{y[+TELNETGA  ]{x Tw`oj telnet dostaje sekwencj`e Go Ahead.\n\r"
		: "{y[-telnetga  ]{x Tw`oj telnet nie dostaje sekwencji Go Ahead.\n\r" ); */

	strcat( buf, IS_SET( ch->act, PLR_SILENCE )
	    ? "{R[+UCISZENIE ]{x Nie wolno ci si`e odzywa`c.\n\r"
	    : "" );

	strcat( buf, CZY_OPCJA( ch, OPC_UNIKI )
	    ? "{y[+UNIKI     ]{x Nie widzisz unikni`etych/sparowanych atak`ow.\n\r"
	    : "{y[-uniki     ]{x Widzisz unikni`ete/sparowane ataki.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_WCHODZENIE )
	    ? "{y[+WCHODZENIE]{x Obserwujesz ruch poza `swiatem.\n\r"
	    : "{y[-wchodzenie]{x Nie obserwujesz wchodz`acych/wychodz`acych postaci.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_AUTOEXIT )
	    ? "{y[+WYJ`SCIA   ]{x Widzisz wyj`scia przy patrzeniu w pomieszczeniu.\n\r"
	    : "{y[-wyj`scia   ]{x Nie widzisz wyj`s`c, gdy patrzysz na pomieszczenie.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_PROMPT )
	    ? "{y[+ZACH`ETA   ]{x Masz zach`et`e.\n\r"
	    : "{y[-zach`eta   ]{x Nie masz zach`ety.\n\r" );

	if ( zaawansowane )
	    strcat( buf, CZY_OPCJA( ch, OPC_WRAP )
		? "{y[+ZAWIJANIE ]{x Lac zawija tekst wysy`lany do ciebie.\n\r"
		: "{y[-zawijanie ]{x Tw`oj terminal sam zawija tekst.\n\r" );

	strcat( buf, CZY_OPCJA( ch, OPC_AUTOGOLD )
	    ? "{y[+Z`LOTO     ]{x Automatycznie wyci`agasz z`loto z cia`l.\n\r"
	    : "{y[-z`loto     ]{x Nie wyci`agasz z`lota z cia`l.\n\r" );

	STC( buf, ch );
    }
    else
    {
	char buf[ MAX_STRING_LENGTH ];
	int  bit = 0;
	int tmp;
	bool fSet;

	     if ( arg[ 0 ] == '+' ) fSet = TRUE;
	else if ( arg[ 0 ] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Konfiguracja -opcja czy +opcja?\n\r", ch );
	    return;
	}

	if ( arg[ 1 ] == '\0' )
	{
	    send_to_char( "Kt`or`a opcj`e zmieni`c?\n\r", ch );
	    return;
	}

	if ( !str_prefix( arg + 1, "onw" ) )
	{
	    send_to_char( "Opcj`e \"onw\" prze`l`acza si`e poleceniem \"onw\".\n\r", ch );
	    return;
	}

	for ( tmp = 0; konfigi[ tmp ].bit; tmp++ )
	{
	    if ( !str_prefix( arg + 1, konfigi[ tmp ].opcja ) )
	    {
		bit = konfigi[ tmp ].bit;
		break;
	    }
	}

	if ( !konfigi[ tmp ].bit )
	{
	    send_to_char( "Kt`or`a opcj`e zmieni`c?\n\r", ch );
	    return;
	}

	if ( fSet )
	{
	    SET_BIT( ch->pcdata->opcje, bit );
	    sprintf( buf, "Opcj`e \"%s\" masz W`L`ACZON`A.\n\r", konfigi[ tmp ].opcja );
	    send_to_char( buf, ch );
	}
	else
	{
	    REMOVE_BIT( ch->pcdata->opcje, bit );
	    sprintf( buf, "Opcj`e \"%s\" masz WY`L`ACZON`A.\n\r", konfigi[ tmp ].opcja );
	    send_to_char( buf, ch );
	}
    }

    return;
}


KOMENDA( do_spells )
{
    char buf1[ MSL * 4 ];
    char buf[ MIL ];
    char mcbuf[ MIL ];
    int  sn;
    int  col;
    SORTED_SKILL_DATA *tss;

    if ( IS_NPC( ch )
	|| ( !IS_NPC( ch )
	  && !class_table[ ch->klasa ].fMana
	  && ch->level < L_APP ) )
    {
       send_to_char ( "Nie potrzebujesz czar`ow.\n\r", ch );
       return;
    }

    buf1[ 0 ] = '\0';

    col = 0;
    for ( tss = sorted_skill_list; tss; tss = tss->next )
    {
	sn = tss->sn;

	if ( !skill_table[ sn ].pl_name )
	    break;
	if ( !skill_table[ sn ].min_mana )
	    continue;
	/* Lam 7.9.99: jesli sie nauczyl z ksiegi, niech widzi */
	if ( !ch->pcdata->learned[ sn ] )
	{
	    if ( ( ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
		&& ch->level < skill_table[ sn ].multi_level[ ch->klasa ] )
		|| ( skill_table[ sn ].skill_level[ ch->klasa ] > LEVEL_HERO
		  && skill_table[ sn ].multi_level[ ch->klasa ] > LEVEL_HERO ) )
		continue;
	    if ( ( ch->level < skill_table[ sn ].skill_level[ ch->klasa ]
		&& !ma_w_multi( ch, sn ) ) )
/*              && skill_table[ sn ].skill_level[ ch->klasa ] > LEVEL_HERO ) */
		/* ^^^ czy to potrzebne? -Lam */
		continue;
	    strcpy( mcbuf, " nd. " );
	}
	else
	    sprintf( mcbuf, "%3dm ", MANA_COST( ch, sn ) );

	sprintf( buf, "%s %s", wyrownaj( skill_table[ sn ].pl_name, 20 ), mcbuf );
	strcat( buf1, buf );
	if ( ++col % 3 == 0 )
	   strcat( buf1, "\n\r" );
    }

    if ( col % 3 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );

    return;
}


KOMENDA( do_slist )
{
    real_slist( ch, argument, 0 );

    return;
}

/*
 * Lam: wszyscy moga patrzec na slisty innych profesji
 */
void real_slist( CHAR_DATA *ch, char *argument, WHO_DESCRIPTOR_DATA *d )
{
    char buf[ MAX_STRING_LENGTH * 4 ];
    char buf1[ MAX_STRING_LENGTH * 2 ];
    int  sn;
    int  col;
    int  level;
    bool pSpell;
    int  class = 0;
    int  pom;
    bool immos; /* pierwotnie inne profesje widzieli tylko niesmiertelni */

    if ( ch && IS_NPC( ch ) )
    {
	send_to_char( "Moby nie maj`a umiej`etno`sci, nie pami`etasz?\n\r", ch );
	return;
    }

    if ( ch )
	class = ch->klasa;
    immos = FALSE;
    for ( pom = 0; pom < MAX_CLASS; pom++ )
	if ( !str_cmp( argument, class_table[ pom ].who_name ) )
	{
	    class = pom;
	    immos = TRUE;
	    break;
	}

    if ( !ch && !immos )
    {
	strcpy( buf, "Nie ma takiej profesji.\n" );
	STH( d, buf );
	return;
    }

    buf1[ 0 ] = '\0';

    if ( immos )
	strcat( buf1, "Umiej`etno`sci dost`epne przedstawicielom profesji:\n\r\n\r" );
    else
	strcat( buf1, "Dost`epne ci umiej`etno`sci:\n\r\n\r" );
    strcat( buf1, "Poziom   Umiej`etno`sci\n\r\n\r" );

    for ( level = 1; level <= LEVEL_HERO; level++ )
    {
	col = 0;
	pSpell = TRUE;

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( !skill_table[ sn ].pl_name )
		break;
	    if ( skill_table[ sn ].skill_level[ class ] != level
	      && skill_table[ sn ].multi_level[ class ] != level )
		continue;
	    if ( skill_table[ sn ].multi_level[ class ] == level
	      && !immos && !ma_w_multi( ch, sn ) )
		continue;

	    if ( pSpell )
	    {
		sprintf( buf, "%2d:", level );
		strcat( buf1, buf );
	    }

	    /* Lam: duze poprawki formatu */
	    col++;
	    if ( ( col % 3 == 1 ) && !pSpell )
		strcat ( buf1, "   " );

	    if ( ch )
	    {
		if ( skill_table[ sn ].skill_level[ class ] == level )
		    sprintf( buf, "%s", wyrownaj( skill_table[ sn ].pl_name, 23 ) );
		else
		    sprintf( buf, "{W%s*{x", wyrownaj( skill_table[ sn ].pl_name, 22 ) );
	    }
	    else
	    {
		if ( skill_table[ sn ].skill_level[ class ] == level )
		    sprintf( buf, "{?%s{x ", wyrownaj( skill_table[ sn ].pl_name, 22 ) );
		else
		    sprintf( buf, "{?%s{x*", wyrownaj( skill_table[ sn ].pl_name, 22 ) );
	    }
	    strcat( buf1, buf );

	    if ( col % 3 == 0 )
		strcat ( buf1, "\n\r" );

	    pSpell = FALSE;
	}

	if ( col % 3 != 0 )
	  strcat ( buf1, "\n\r" );
    }

    if ( ch )
	send_to_char( buf1, ch );
    else
    {
	colourconv( buf, lac2html( buf1, FALSE ), NULL, sizeof( buf ) );
	STH( d, buf );
    }

    return;
}

KOMENDA( do_pagelen )
{
    char buf[ MAX_STRING_LENGTH ];
    char arg[ MAX_INPUT_LENGTH  ];
    int  lines;

    if ( IS_NPC( ch ) )
	return;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	lines = liczba( ch->pcdata->pagelen );
	/* przypadek "lini`e" niemozliwy */
	sprintf( buf, "Wielko`s`c strony ustawiona na %d lini%s tekstu.\n\r",
		ch->pcdata->pagelen, lines == 2 ? "e" : "i" );
	send_to_char( buf, ch );
	return;
    }
    else
	lines = atoi( arg );

    if ( lines < 12 )
    {
	send_to_char( "Nie mo`zesz mie`c mniej ni`z 12 linii na stronie.\n\r", ch );
	return;
    }

    if ( lines > 80 )
    {
	send_to_char( "Wi`ecej ni`z 80 to za du`zo, ustawiam 80.\n\r", ch );
	lines = 80;
    }

    ch->pcdata->pagelen = lines;
    lines = liczba( ch->pcdata->pagelen );
    sprintf( buf, "Wielko`s`c strony ustawiasz na %d lini%s tekstu.\n\r",
		ch->pcdata->pagelen, lines == 2 ? "e" : "i" );
    send_to_char( buf, ch );

    return;
}


/* Do_prompt from Morgenes from Aldara Mud */
KOMENDA( do_prompt )
{
    char buf[ MAX_STRING_LENGTH ];

    buf[ 0 ] = '\0';

    /* Unswitched NPC's get kicked out */
    if ( !ch->desc )
	return;

    /* Will always have a pc ch after this */
    ch = ( ch->desc->original ? ch->desc->original : ch->desc->character );

    if ( argument[ 0 ] == '\0' )
    {
	send_to_char( CZY_OPCJA( ch, OPC_PROMPT ) ?
		"Aby wy`l`aczy`c zach`et`e napisz \"opcje -zach`eta\", zobacz te`z \"pomoc zach`eta\".\n\r"
		: "Aby w`l`aczy`c zach`et`e napisz \"opcje +zach`eta\", zobacz te`z \"pomoc zach`eta\".\n\r", ch );
	sprintf( buf, "Twoja obecna zach`eta: %s\n\r", ch->pcdata->prompt );
	send_to_char_bw( buf, ch );
	return;
    }

    if ( !strcmp( argument, "1" ) )
	strcat( buf, "<%A%h`z %mm %vr %xd`s> ");
    else if ( !strcmp( argument, "2" ) )
	strcat( buf, "<%A%h`z %mm %vr %xd`s><%n`z %cm> ");
    else if ( !strcmp( argument, "3" ) )
	strcat( buf, "<%A%h`z(%p%%) %mm(%P%%) %vr %xd`s> ");
    else
    {
	if ( strlen( argument ) > 80 )
	    argument[ 80 ] = '\0';
	smash_tilde( argument );
	strcat( buf, argument );
    }

    free_string( ch->pcdata->prompt );
    ch->pcdata->prompt = str_dup( buf );
    send_to_char( "Zmieniasz sobie zach`et`e.\n\r", ch );

    return;
}


/* Lam 8.4.99: bojowa zacheta */
KOMENDA( do_fprompt )
{
    char buf[ MAX_STRING_LENGTH ];

    /* Unswitched NPC's get kicked out */
    if ( !ch->desc )
	return;

    /* Will always have a pc ch after this */
    ch = ( ch->desc->original ? ch->desc->original : ch->desc->character );

    if ( argument[ 0 ] == '\0' )
    {
	if ( ch->pcdata->fprompt && *ch->pcdata->fprompt )
	{
	    sprintf( buf, "Twoja zach`eta bitewna: %s\n\r", ch->pcdata->fprompt );
	    send_to_char_bw( buf, ch );
	    send_to_char( "Aby wy`l`aczy`c zach`et`e bitewn`a, napisz \"bzach`eta brak\".\n\r", ch );
	}
	else
	    send_to_char( "Sk`ladnia: bzach`eta <nowa zach`eta bitewna>. Zobacz \"pomoc zach`eta\".\n\r", ch );
	return;
    }

    if ( !strcmp( argument, "1" ) )
	strcpy( buf, "<%A%h`z %mm %vr %xd`s> ");
    else if ( !strcmp( argument, "2" ) )
	strcpy( buf, "<%A%h`z %mm %vr %xd`s><%n`z %cm> ");
    else if ( !strcmp( argument, "3" ) )
	strcpy( buf, "<%A%h`z(%p%%) %mm(%P%%) %vr %xd`s> ");
    else if ( !str_cmp( argument, "brak" ) || !str_cmp( argument, "null" )
	   || !str_cmp( argument, "auto" ) || !strcmp( argument, "0" ) )
	strcpy( buf, "" );
    else
    {
	if ( strlen( argument ) > 80 )
	    argument[ 80 ] = '\0';
	smash_tilde( argument );
	strcpy( buf, argument );
    }

    free_string( ch->pcdata->fprompt );
    ch->pcdata->fprompt = str_dup( buf );
    send_to_char( "Zmieniasz sobie zach`et`e bitewn`a.\n\r", ch );

    return;
}


KOMENDA( do_afk )
{
    if ( IS_NPC( ch ) )
	return;

    SET_BIT( ch->act, PLR_AFK );
    send_to_char( "Odchodzisz od klawiatury.\n\r", ch );
    if WP( ch )
	act( ZM_WZROK | ZM_WID_CHAR | ZM_DYLEMAT, "$n odchodzi od klawiatury.",
	     ch, NULL, ch, TO_ROOM );

    return;
}


/*
 * Lam 30.5.99: Wycwiczony szpieg moze sobie zajrzec do sasiedniego
 *		pomieszczenia.
 * Lam 30.1.2004: dalo sie uciec z walki bez straty doswiadczenia podgladajac
 *		sasiednie pomieszczenie
 */
KOMENDA( do_podgladaj )
{
    char arg[ MIL ];
    int door;
    EXIT_DATA *pexit;

    if ( IS_NPC( ch ) || ( !IS_NPC( ch ) && !moze_uzyc( ch, gsn_podglad ) ) )
    {
	send_to_char( "Podgl`adanie zostaw tym, kt`orzy si`e na tym znaj`a.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	ASTC( "Jeste`s zbyt zaj`et$y walk`a.", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    one_argument( argument, arg );
    if ( !arg[ 0 ] )
    {
	send_to_char( "W kt`orym kierunku podgl`ada`c?\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg, FALSE ) ) < 0 )
    {
	if ( door == -1 )
	    send_to_char( "Nie ma tu takich drzwi.\n\r", ch );
	return;
    }

    pexit = ch->in_room->exit[ door ];
    if ( !pexit || !pexit->to_room )
    {
	send_to_char( "Nie ma tam `zadnego przej`scia.\n\r", ch );
	return;
    }

    /* Lam: mozna podgladac przez zamkniete drzwi */
    if ( IS_SET( pexit->exit_info, EX_CLOSED )
      && IS_SET( pexit->exit_info, EX_PICKPROOF )
      && IS_SET( pexit->exit_info, EX_PASSPROOF ) )
    {
	send_to_char( "Nie znajdujesz nawet najmniejszej szparki.\n\r", ch );
	return;
    }

    if ( room_is_dark( pexit->to_room ) )
    {
	send_to_char( "Jest tam tak ciemno, `ze niczego nie dostrze`zesz.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_podglad ].beats );

    if ( ch->pcdata->learned[ gsn_podglad ] < number_percent( ) )
    {
	ASTC( "Co`s ci wpad`lo do oka i niczego nie zauwa`zy`l$o`s.", ch );
	return;
    }

    send_to_char( "{YZagl`adasz i podgl`adasz:{x\n\r", ch );
    pokaz_pomieszczenie( ch, pexit->to_room, FALSE );
    uzyj( ch, gsn_podglad, "podgl`adaniu" );

    return;
}


KOMENDA( do_tobank )
{
    CHAR_DATA * mob;
    char arg[ MAX_INPUT_LENGTH ];
    int amount;
    char buf[ MAX_STRING_LENGTH ];
    int ll;

    if ( IS_NPC( ch ) )
	return;

    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	if ( IS_NPC ( mob ) && ( mob->pIndexData->vnum == MOB_VNUM_BANKIER )
	  && IS_AWAKE( mob ) )
	    break;

    if ( !mob )
    {
	send_to_char( "Nie ma tu bankiera.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Ile z`lota wp`laci`c do banku?\n\r", ch );
	return;
    }

    amount = atoi ( arg );

    if ( amount == 0 )
    {
	send_to_char( "Chcesz co`s wp`laci`c czy nie?\n\r", ch );
	return;
    }

    if ( amount < 0 )
    {
	send_to_char( "Do wyp`lacania s`lu`zy polecenie \"wyp`la`c\".\n\r", ch );
	return;
    }

    if ( amount > ch->gold )
    {
	send_to_char( "Nie masz takiej ilo`sci z`lota przy sobie.\n\r", ch );
	return;
    }

    if ( !can_see( mob, ch ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze ci`e zobaczy`c.", mob, NULL, ch, TO_VICT );
	return;
    }

    ch->gold -= amount;
    amount -= amount / 20;
    ch->pcdata->bank += amount;
    ll = liczba( amount );
    sprintf( buf, "Po odpisaniu podatku do banku w`edruj%s %s.\n\r",
	ll == 2 ? "`a" : "e", ZLOTO_M( amount ) );
    send_to_char( buf, ch );
    sprintf( buf, "Aktualny stan twojego konta: %s.\n\r", ZLOTO_M( ch->pcdata->bank ) );
    send_to_char( buf, ch );

    return;
}


KOMENDA( do_frombank )
{
    CHAR_DATA * mob;
    char arg[ MAX_INPUT_LENGTH ];
    int amount;
    char buf[ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
	return;

    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	if ( IS_NPC ( mob ) && ( mob->pIndexData->vnum == MOB_VNUM_BANKIER )
	  && IS_AWAKE( mob ) )
	    break;

    if ( !mob )
    {
	send_to_char( "Nie ma tu bankiera.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Ile z`lota wyp`laci`c z banku?\n\r", ch );
	return;
    }

    amount = atoi ( arg );

    if ( amount == 0 )
    {
	send_to_char( "Chcesz co`s wyp`laci`c czy nie?\n\r", ch );
	return;
    }

    if ( amount < 0 )
    {
	send_to_char( "Do wp`lacania s`lu`zy polecenie \"wp`la`c\".\n\r", ch );
	return;
    }

    if ( amount > ch->pcdata->bank )
    {
	send_to_char( "Nie masz takiej ilo`sci z`lota na koncie.\n\r", ch );
	return;
    }

    if ( !can_see( mob, ch ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze ci`e zobaczy`c.", mob, NULL, ch, TO_VICT );
	return;
    }

    ch->gold += amount;
    ch->pcdata->bank -= amount;
    sprintf( buf, "Wyp`lacasz z banku %s.\n\r", ZLOTO_B( amount ) );
    send_to_char( buf, ch );
    sprintf( buf, "Aktualny stan twojego konta: %s.\n\r", ZLOTO_M( ch->pcdata->bank ) );
    send_to_char( buf, ch );

    return;
}


KOMENDA( do_odmiana )
{
    char buf [ MAX_STRING_LENGTH ];
    char buf1[ MAX_STRING_LENGTH ];

    if ( argument[ 0 ] == '\0' )
    {
	strcpy( buf, "Oto podana przez ciebie odmiana imienia twojej postaci przez przypadki:\n\r" );
	sprintf( buf1, "Mianownik    [kto? co?]:        %s\n\r", ch->short_descr );
	strcat( buf, buf1 );
	sprintf( buf1, "Dope`lniacz   [kogo? czego?]:    %s\n\r", ch->dopelniacz );
	strcat( buf, buf1 );
	sprintf( buf1, "Celownik     [komu? czemu?]:    %s\n\r", ch->celownik );
	strcat( buf, buf1 );
	sprintf( buf1, "Biernik      [kogo? co?]:       %s\n\r", ch->biernik );
	strcat( buf, buf1 );
	sprintf( buf1, "Narz`ednik    [z kim? z czym?]:  %s\n\r", ch->narzednik );
	strcat( buf, buf1 );
	sprintf( buf1, "Miejscownik  [o kim? o czym?]:  %s\n\r", ch->miejscownik );
	strcat( buf, buf1 );
	sprintf( buf1, "Wo`lacz       [o!]:              %s\n\r", ch->wolacz );
	strcat( buf, buf1 );
	strcat( buf, "ODMIANA z parametrem \"przyk`lad\" poka`ze ci przyk`lad zastosowania przypadk`ow.\n\r" );
	strcat( buf, "ODMIANA z parametrem \"zmie`n\" pozwoli ci zmieni`c przypadki twojego imienia.\n\r" );
	strcat( buf, "ODMIANA z imieniem jako parametr poka`ze ci odmian`e imienia wskazanej postaci.\n\r" );
	send_to_char( buf, ch );
	return;
    }

    if ( !str_prefix( argument, "przyk`lad" ) )
    {
	strcpy( buf, "Przyk`lad zastosowania przypadk`ow:\n\r" );
	sprintf( buf1, "Mianownik    : Stoi tu {Y%s{x.\n\r", ch->short_descr );
	strcat( buf, buf1 );
	sprintf( buf1, "Dope`lniacz   : Nie widz`e tu {Y%s{x.\n\r", ch->dopelniacz );
	strcat( buf, buf1 );
	sprintf( buf1, "Celownik     : Dajesz sztylet {Y%s{x.\n\r", ch->celownik );
	strcat( buf, buf1 );
	sprintf( buf1, "Biernik      : Twoje ci`ecie kaleczy {Y%s{x.\n\r", ch->biernik );
	strcat( buf, buf1 );
	sprintf( buf1, "Narz`ednik    : Potw`or walczy %s {Y%s{x.\n\r",
	    czy_ze( ch->narzednik ) ? "ze" : "z", ch->narzednik );
	strcat( buf, buf1 );
	sprintf( buf1, "Miejscownik  : Rozmawiamy tu o {Y%s{x.\n\r", ch->miejscownik );
	strcat( buf, buf1 );
	sprintf( buf1, "Wo`lacz       : Czekali`smy na ciebie, {Y%s{x.\n\r", ch->wolacz );
	strcat( buf, buf1 );
	send_to_char( buf, ch );
	return;
    }

    if ( !str_prefix( argument, "zmie`n" ) )
    {
/*	char buf [ MAX_STRING_LENGTH ]; */

	if ( ch->fighting || ch->position < POS_SLEEPING || IS_NPC( ch ) )
	{
	    send_to_char( "Nie mo`zesz teraz tego zrobi`c.\n\r", ch );
	    return;
	}

/*
	if ( ch->level < 3 )
	{
	    send_to_char( "Odmieni`c mo`zesz si`e po osi`agni`eciu 3 poziomu. Jest to spowodowane faktem,\n\r"
		"`ze zmiany przypadk`ow s`a przegl`adane przez ludzi, a nie chcemy przekopywa`c\n\r"
		"si`e przez stosy odmieniaj`acych si`e setki razy nowych postaci.\n\r", ch );
	    return;
	}
*/

	send_to_char( "Podaj teraz swoje przypadki, je`sli kt`orego`s nie podasz, natychmiast\n\r", ch );
	send_to_char( "operacja zostanie przerwana. Je`sli jako przypadki nie podasz przypadk`ow\n\r", ch );
	send_to_char( "twojego imienia, twoja posta`c mo`ze zosta`c usuni`eta bez ostrze`zenia.\n\r\n\r", ch );
/*	sprintf( buf, "Dope`lniacz [kogo? czego?] (%s): ", ch->dopelniacz );
	send_to_char( buf, ch ); */
	ch->odmiana = 1;

	return;
    }
    else
    {
        /* Brottor 14.5.2007: ponizsze na podstawie do_tell */
	CHAR_DATA *victim;

	one_argument( argument, buf );

	if ( !( victim = get_char_world_pc_tele( ch, buf ) )
	  || IS_NPC( victim ) )
	{
	    send_to_char( "Nikt taki nie gra obecnie.\n\r", ch );
	    return;
	}

	strcpy( buf, "Imi`e podanej postaci odmienia si`e nast`epuj`aco:\n\r" );
	sprintf( buf1, "Mianownik    [kto? co?]:        %s\n\r", victim->short_descr );
	strcat( buf, buf1 );
	sprintf( buf1, "Dope`lniacz   [kogo? czego?]:    %s\n\r", victim->dopelniacz );
	strcat( buf, buf1 );
	sprintf( buf1, "Celownik     [komu? czemu?]:    %s\n\r", victim->celownik );
	strcat( buf, buf1 );
	sprintf( buf1, "Biernik      [kogo? co?]:       %s\n\r", victim->biernik );
	strcat( buf, buf1 );
	sprintf( buf1, "Narz`ednik    [z kim? z czym?]:  %s\n\r", victim->narzednik );
	strcat( buf, buf1 );
	sprintf( buf1, "Miejscownik  [o kim? o czym?]:  %s\n\r", victim->miejscownik );
	strcat( buf, buf1 );
	sprintf( buf1, "Wo`lacz       [o!]:              %s\n\r", victim->wolacz );
	strcat( buf, buf1 );
	send_to_char( buf, ch );
    }

    return;
}


void zapisz_odmiane( CHAR_DATA *ch )
{
    FILE *fp;

    zamknij_reserve( fpReserve );

    if ( ( fp = fopen( PRZYP_FILE, "a" ) ) )
    {
	fprintf( fp, "Name: %s  Short: %s\n", ch->name, ch->short_descr );
	fprintf( fp, "Dopelniacz  [kogo? czego?]   : %s\n", ch->dopelniacz  );
	fprintf( fp, "Celownik    [komu? czemu?]   : %s\n", ch->celownik    );
	fprintf( fp, "Biernik     [kogo? co?]      : %s\n", ch->biernik     );
	fprintf( fp, "Narzednik   [z kim? z czym?] : %s\n", ch->narzednik   );
	fprintf( fp, "Miejscownik [o kim? o czym?] : %s\n", ch->miejscownik );
	fprintf( fp, "Wolacz      [o!]             : %s\n", ch->wolacz      );
	fprintf( fp, "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n" );
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );

    if ( ch->desc && !ch->desc->connected )
	wiznet( "{G$N{x odmienia si`e przez przypadki... Sprawd`x, czy poprawnie.",
		ch, NULL, NULL, WIZ_ODM, 0, 103 );

    return;
}


bool przypadek_do_przyjecia( CHAR_DATA *ch, char *arg )
{
    if ( strlen( arg ) > 20 )
    {
	STC( "Tak d`lugiego przypadku na pewno nie masz.\n\r", ch );
	return FALSE;
    }

    for ( ; *arg; arg++ )
	if ( !isalpha( (int) *arg ) && *arg != '`' && *arg != '\'' )
	{
	    STC( "Imiona nie zawieraj`a znak`ow innych ni`z litery, popraw si`e.\n\r", ch );
	    return FALSE;
	}

    return TRUE;
}


void odmiana_postaci( CHAR_DATA *ch, char *arg )
{
/*    char buf [ MAX_STRING_LENGTH ]; */

    if ( ch->odmiana == 0 )
    {
	bug( "odmiana_postaci: ch->odmiana == 0 ", 0 );
	return;
    }

    while ( isspace( (int) *arg ) )
	arg++;

    if ( !ch->desc->connected
      && ( arg[ 0 ] == '\0' || arg[ 0 ] == '\n' || arg[ 0 ] == '\r' ) )
    {
	ch->odmiana = 0;
	zapisz_odmiane( ch );
	do_odmiana( ch, "przyk`lad" );
	return;
    }

/* przy zakladaniu postaci */
    if ( arg[ 0 ] == '\0' || arg[ 0 ] == '\n' || arg[ 0 ] == '\r' )
    {
	send_to_char( "Musisz co`s wpisa`c!\n\r", ch );
	return;
    }

    arg[ 0 ] = UPPER( arg[ 0 ] );
    if ( arg[ 0 ] == '`' ) arg[ 1 ] = UPPER( arg[ 1 ] );

    switch ( ch->odmiana )
    {
	case 1:
	    if ( !przypadek_do_przyjecia( ch, arg ) )
		break;
	    free_string( ch->dopelniacz );
	    ch->dopelniacz = str_dup( zwielkoliteruj( arg ) );
	    ch->odmiana = 2;
	    break;
	case 2:
	    if ( !przypadek_do_przyjecia( ch, arg ) )
		break;
	    free_string( ch->celownik );
	    ch->celownik = str_dup( zwielkoliteruj( arg ) );
	    ch->odmiana = 3;
	    break;
	case 3:
	    if ( !przypadek_do_przyjecia( ch, arg ) )
		break;
	    free_string( ch->biernik );
	    ch->biernik = str_dup( zwielkoliteruj( arg ) );
	    ch->odmiana = 4;
	    break;
	case 4:
	    if ( !przypadek_do_przyjecia( ch, arg ) )
		break;
	    free_string( ch->narzednik );
	    ch->narzednik = str_dup( zwielkoliteruj( arg ) );
	    ch->odmiana = 5;
	    break;
	case 5:
	    if ( !przypadek_do_przyjecia( ch, arg ) )
		break;
	    free_string( ch->miejscownik );
	    ch->miejscownik = str_dup( zwielkoliteruj( arg ) );
	    ch->odmiana = 6;
	    break;
	case 6:
	    if ( !przypadek_do_przyjecia( ch, arg ) )
		break;
	    free_string( ch->wolacz );
	    ch->wolacz = str_dup( zwielkoliteruj( arg ) );
	    ch->odmiana = 0;
	    zapisz_odmiane( ch );
	    send_to_char( "\n\r", ch );
	    do_odmiana( ch, "przyk`lad" );
	    break;
	default:
	    bug( "odmiana_postaci: ch->odmiana spoza zakresu", 0 );
	    ch->odmiana = 0;
	    break;
    }

    return;
}


void odmiana_prompt( CHAR_DATA *ch )
{
    char buf[ MAX_STRING_LENGTH ];

    switch ( ch->odmiana )
    {
	case 1:
	    sprintf( buf, "Dope`lniacz [kogo? czego?] (%s): ", ch->dopelniacz );
	    send_to_char( buf, ch );
	    break;
	case 2:
	    sprintf( buf, "Celownik [komu? czemu?] (%s): ", ch->celownik );
	    send_to_char( buf, ch );
	    break;
	case 3:
	    sprintf( buf, "Biernik [kogo? co?] (%s): ", ch->biernik );
	    send_to_char( buf, ch );
	    break;
	case 4:
	    sprintf( buf, "Narz`ednik [z kim? z czym?] (%s): ", ch->narzednik );
	    send_to_char( buf, ch );
	    break;
	case 5:
	    sprintf( buf, "Miejscownik [o kim? o czym?] (%s): ", ch->miejscownik );
	    send_to_char( buf, ch );
	    break;
	case 6:
	    sprintf( buf, "Wo`lacz [o!] (%s): ", ch->wolacz );
	    send_to_char( buf, ch );
	    break;
	default:
	    bug( "odmiana_prompt: ch->odmiana spoza zakresu", 0 );
	    ch->odmiana = 0;
	    break;
    }

    return;
}


KOMENDA( do_version )
{
    send_to_char( LAC_VERSION "\n\r", ch );

    return;
}


KOMENDA( do_ostatnie )
{
    bool  czy_cos = FALSE;

    if ( !IS_NPC( ch ) )
    {
	int i;

	send_to_char( "Twoje ostatnie wiadomo`sci:\n\r", ch );
	for ( i = 0; i < MAX_OSTATNIE; i++ )
	    if ( ch->pcdata->ostatnie[ ( ch->pcdata->ost_end + i ) % MAX_OSTATNIE ] )
	    {
		czy_cos = TRUE;
		send_to_char( ch->pcdata->ostatnie[ ( ch->pcdata->ost_end + i ) % MAX_OSTATNIE ], ch );
	    }
	if ( !czy_cos )
	    send_to_char( "Nikt do ciebie niczego nie m`owi`l.\n\r", ch );
    }
    else
    {
	send_to_char( "Chyba `zartujesz?\n\r", ch );
    }

    return;
}


KOMENDA( do_poziomy )
{
    int poziomy[ MAX_CLASS ];
    int i;

    if ( IS_NPC( ch ) )
    {
	STC( "Moby nie zdobywaj`a poziom`ow.\n\r", ch );
	return;
    }

    wypelnij_poziomy( ch, poziomy );

    for ( i = 0; i < MAX_CLASS; i++ )
	if ( !*class_table[ i ].races ) /* szaman */
	    continue;
	else if ( i == ch->klasa )
	    ch_printf( ch, " {Y%s{x", class_table[ i ].pl_name );
	else if ( poziomy[ i ] >= 100 )
	    ch_printf( ch, " {G%s{x", class_table[ i ].pl_name );
	else
	    ch_printf( ch, " %s", class_table[ i ].pl_name );

    STC( "\n\r", ch );

    for ( i = 0; i < MAX_CLASS; i++ )
	if ( !*class_table[ i ].races )
	    continue;
	else if ( i == ch->klasa )
	    ch_printf( ch, " {Y%3d{x", UMIN( poziomy[ i ], 100 ) );
	else if ( poziomy[ i ] == 0 )
	    STC( "   {b-{x", ch );
	else if ( poziomy[ i ] >= 100 )
	    STC( " {G100{x", ch );
	else
	    ch_printf( ch, " %3d", poziomy[ i ] );

    STC( "\n\r", ch );

    return;
}
