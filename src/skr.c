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

/******************************************************************************
 *                        Skrytobojca - dzielo Fuyary                         *
 ******************************************************************************/

/*
 * Lokalne definicje
 */

#include "merc.h"
#include "handler.h"
#include "const.h"


#define BAD_DOOR		  -1

bool	critical_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	rune_flare		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	rune_fire		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	rune_ice		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	rune_poison		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	calculate_damage	args( ( CHAR_DATA *ch, CHAR_DATA *victim,
					int gsn ) );

/* act_obj.c */
bool	pijane_przedmioty	args( ( CHAR_DATA *ch ) );

/* POCZATEK: zmodyfikowany fragment z one_hit dotyczacy modyfikacji obrazen */
int calculate_damage( CHAR_DATA *ch, CHAR_DATA *victim, int gsn )
{
    int dam  = 0;
    /* zamienilem obrazenia tak, zeby nadac wiekszy sens technikom:
       -death moze zabic ofiare
       -dragon to cios techniczny, wybijajacy z rytmu przeciwnika
       -power to po prostu najmocniejszy cios skrytobojcy
       -speed to najszybszy cios wykonywany bez opoznienia (na koncu jest)
       -obuchem mozna ogluszyc i przerwac walke */
	 if ( gsn == gsn_death_tech )
	    dam = number_range( ch->level / 3, ch->level );
    else if ( gsn == gsn_dragon_tech )
	    dam = number_range( ch->level / 8, ch->level / 4 );
    else if ( gsn == gsn_power_tech )
	    dam = number_range( ch->level / 3, 4 * ch->level / 3 );
    else if ( gsn == gsn_speed_tech )
    {
	    dam = UMIN( 20, ch->level );

	    dam = number_range( dam / 8, dam / 4 );
    }
    else if ( gsn == gsn_stun )
	    dam = number_range( ch->level / 4, ch->level / 2 );
    else
	    dam = number_range( ch->level / 8, ch->level / 4 );

    if ( ch->pcdata->learned[ gsn_enhanced_damage ] > 0 )
    {
	dam += dam * ch->pcdata->learned[ gsn_enhanced_damage ] / 150;
	uzyj( ch, gsn_enhanced_damage, "zadawaniu zwi`ekszonych obra`ze`n" );
    }

    if ( !IS_AWAKE( victim ) )
	dam *= 2;

    /* skrytobojcy lepiej walcza rekami niz bronia */
    if ( ch->pcdata->learned[ gsn_mighty_hands ] > 0 )
    {
	dam += dam * ch->pcdata->learned[ gsn_mighty_hands ] / ( 150 - ch->level );
	uzyj( ch, gsn_mighty_hands, "zadawaniu zwi`ekszonych obra`ze`n r`ekoma" );
    }

    if ( ch->pcdata->learned[ gsn_anatomy ] > 0 )
    {
	dam += dam * ch->pcdata->learned[ gsn_anatomy ] / 150;
	uzyj( ch, gsn_anatomy, "znajomo`sci anatomii" );
    }

    /* Qwert: dodalem tu, bo za wolno sie uczy, a wtajemniczenie i tak jest
       wymagane, zeby uzyc ktorejkolwiek z tych technik (180 uzyc na 1%
       wtajemniczenia :)) */
    if ( ch->pcdata->learned[ gsn_initiation ] > 74 )
	uzyj( ch, gsn_initiation, "wy`zszym stopniu wtajemniczenia" );

    if ( dam <= 0 )
	dam = 1;

    return dam;
}
/* KONIEC: zmodyfikowany fragment z one_hit dotyczacy modyfikacji obrazen */


/*
 * Umiejetnosci
 */

/*
 * Ocen - okreslenie wartosci i przydatnosci przedmiotow
 *
 * Qwert: pragne tylko wspomniec, ze skrytobojcy nie znaja sie na magii. Ale
 * skoro Ulryk sie napracowal, to nie bede psul dobrej roboty :) Niech bedzie,
 * ze 'intuicyjnie' cos tam wyczuwaja.
 *
 * Muzgus: nie wyczuwaja magii, to nie wyczuwaja i tyle. W tej chwili rozpoznaja
 * tylko to, czy przedmiot jest ofensywny, ochronny czy bez sensu.
 */
KOMENDA( do_appraise )
{
    OBJ_DATA *obj;
    char buf[ MAX_STRING_LENGTH ];
    int av;
    int gold;
    int ac;
    int hold;
    int foo;
    int time;
    int czypas;
    int ocena;

    if ( !moze_uzyc( ch, gsn_appraise ) )
    {
	send_to_char( "Nie potrafisz tego wykona`c.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->learned[ gsn_appraise ] == 0 )
    {
	send_to_char( "Nie umiesz ocenia`c przedmiot`ow!\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Lepiej si`e zajmij walk`a.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( argument[ 0 ] == '\0' )
    {
	send_to_char( "Co chcesz oceni`c?\n\r", ch );
	return;
    }

    if ( !( obj = get_obj_here( ch, argument ) ) )
    {
	send_to_char( "Przyda`loby si`e, aby to by`l przedmiot!\n\r", ch );
	return;
    }

    /* Ulryk: zmieniam, poniewaz 1% dawal tyle samo co 100% */
    if ( !IS_NPC( ch )
      && obj->level > ch->level * 2 / 3 + ch->pcdata->learned[ gsn_appraise ] )
    {
	send_to_char( "Nie jeste`s w stanie oceni`c tego przedmiotu.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_appraise ].beats );

    switch ( obj->item_type )
    {
    case ITEM_WEAPON:
	/* nieco ciekawsze komunikaty */
	av = ( obj->value[ 1 ].v + obj->value[ 2 ].v ) / 2;
	     if ( av <  5 ) sprintf( buf, "Nie utniesz nawet trawy %s $k.",
			    obj->rodzaj == 2 ? "t`a" : obj->rodzaj == 3 ? "tymi" : "tym" );
	else if ( av < 10 ) sprintf( buf, "%s $p %s zrani`c ma`le zwierz`e.",
			    obj->rodzaj == 1 ? "Ten" : obj->rodzaj == 2 ? "Ta" : obj->rodzaj == 3 ? "Te" : "To",
			    obj->rodzaj == 3 ? "mog`a" : "mo`ze" );
	else if ( av < 15 ) sprintf( buf, "%s $p %s w porz`adku, ale nie warto %s kra`s`c!",
			    obj->rodzaj == 1 ? "Ten" : obj->rodzaj == 2 ? "Ta" : obj->rodzaj == 3 ? "Te" : "To",
			    obj->rodzaj == 3 ? "s`a" : "jest",
			    obj->rodzaj == 2 ? "jej" : obj->rodzaj == 3 ? "ich" : "go" );
	else if ( av < 20 ) sprintf( buf, "%s $p %s nie`xle, mo`ze czas %s zdoby`c?",
			    obj->rodzaj == 1 ? "Ten" : obj->rodzaj == 2 ? "Ta" : obj->rodzaj == 3 ? "Te" : "To",
			    obj->rodzaj == 3 ? "wygl`adaj`a" : "wygl`ada",
			    obj->rodzaj == 1 ? "taki" : obj->rodzaj == 2 ? "tak`a" : "takie" );
	else if ( av < 30 ) sprintf( buf, "%s $p %s na bardzo pot`e`zn%s!",
			    obj->rodzaj == 1 ? "Ten" : obj->rodzaj == 2 ? "Ta" : obj->rodzaj == 3 ? "Te" : "To",
			    obj->rodzaj == 3 ? "wygl`adaj`a" : "wygl`ada",
			    obj->rodzaj == 1 ? "y" : obj->rodzaj == 2 ? "`a" : "e" );
	else if ( av < 50 ) sprintf( buf, "%s $k powalisz drzewo za jednym zamachem!",
			    obj->rodzaj == 2 ? "T`a" : obj->rodzaj == 3 ? "Tymi" : "Tym" );
	else                sprintf( buf, "Z %s $k mo`zesz i`s`c na samych bog`ow!",
			    obj->rodzaj == 2 ? "t`a" : obj->rodzaj == 3 ? "tymi" : "tym" );

	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
	    send_to_char( "Wyczuwasz w tym trucizn`e!\n\r", ch );
	if ( obj->timer > 0 )
	    send_to_char( "Ten przedmiot nied`lugo si`e rozpadnie.\n\r", ch );
	break;

    case ITEM_ARMOR:
	ac = obj->value[ 0 ].v;
	     if ( ac <  0 ) sprintf( buf, "$p chroni%s gorzej od papierowej torebki!",
			    obj->rodzaj == 3 ? "`a" : "" );
	else if ( ac <  3 ) sprintf( buf, "$p zdaj%s si`e by`c niezbyt mocn%s.",
			    obj->rodzaj == 3 ? "`a" : "e",
			    obj->rodzaj == 1 ? "y" : obj->rodzaj == 2 ? "a" : "e" );
	else if ( ac <  5 ) sprintf( buf, "$p %sby ca`lkiem nie`xle ci`e chroni`c.",
			    obj->rodzaj == 1 ? "m`og`l" : obj->rodzaj == 2 ? "mog`la" : obj->rodzaj == 3 ? "mog`ly" : "mog`lo" );
	else if ( ac < 10 ) sprintf( buf, "$p zapewni%s ci dobr`a ochron`e przed atakami.",
			    obj->rodzaj == 3 ? "`a" : "" );
	else if ( ac < 50 ) sprintf( buf, "$p mo%s chroni`c nawet bog`ow!",
			    obj->rodzaj == 3 ? "g`a" : "`ze" );
	else                sprintf( buf, "$p %s boskiej konstrukcji!",
			    obj->rodzaj == 3 ? "s`a" : "jest" );

	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_SCROLL:
    case ITEM_STAFF:
    case ITEM_WAND:
	ocena = ocen_przedmiot( obj->value[ 1 ].v, obj->value[ 2 ].v, obj->value[ 3 ].v );

	     if ( ocena == 2 ) sprintf( buf, "$p lekko dr`z%s.",
			       obj->rodzaj == 3 ? "`a" : "y" );
	else if ( ocena == 1 ) sprintf( buf, "$p lekko iskrz%s.",
			       obj->rodzaj == 3 ? "`a" : "y" );
	else                   sprintf( buf, "$p nie ma%s w sobie nic specjalnego.",
			       obj->rodzaj == 3 ? "j`a" : "" );

	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_POTION:
    case ITEM_PILL:
	ocena = ocen_przedmiot( obj->value[ 1 ].v, obj->value[ 2 ].v, obj->value[ 3 ].v );

	     if ( ocena == 2 ) sprintf( buf, "$p pachn%s perfumami.",
			       obj->rodzaj == 3 ? "`a" : "ie" );
	else if ( ocena == 1 ) sprintf( buf, "$p pachn%s siark`a.",
			       obj->rodzaj == 3 ? "`a" : "ie" );
	else                   sprintf( buf, "$p nie ma%s `zadnego zapachu.",
			       obj->rodzaj == 3 ? "j`a" : "" );

	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_CONTAINER:
	hold = obj->value[ 0 ].v;

	     if ( hold <  40 ) sprintf( buf, "$p pomie%s kilka kilogram`ow.",
			       obj->rodzaj == 3 ? "szcz`a" : "`sci" );
	else if ( hold <  80 ) sprintf( buf, "$p pomie%s kilkana`scie kilogram`ow.",
			       obj->rodzaj == 3 ? "szcz`a" : "`sci" );
	else if ( hold < 400 ) sprintf( buf, "$p pomie%s kilkadziesi`at kilogram`ow.",
			       obj->rodzaj == 3 ? "szcz`a" : "`sci" );
	else                   sprintf( buf, "$p pomie%s kilkaset kilogram`ow.",
			       obj->rodzaj == 3 ? "szcz`a" : "`sci" );

	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_FURNITURE:
	sprintf( buf, "$p wygl`ada%s jak meb%s.",
	      obj->rodzaj == 3 ? "j`a" : "",
	      obj->rodzaj == 3 ? "le" : "el" );
	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_TRASH:
	sprintf( buf, "$p zdaj%s si`e do niczego nie s`lu`zy`c, naprawd`e.",
	      obj->rodzaj == 3 ? "`a" : "e" );
	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_DRINK_CON:
	sprintf( buf, "w %s", obj->name );
	do_look( ch, buf );

    if ( obj->value[ 3 ].v != 0 )
	send_to_char( "Wyczuwasz trucizn`e!\n\r", ch );

	break;

    case ITEM_KEY:
	sprintf( buf, "$p wygl`ada%s jak klucz.",
	      obj->rodzaj == 3 ? "j`a" : "" );
	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_FOOD:
	foo = obj->value[ 0 ].v / UMAX( 1 + race_table[ch->race].hunger_mod, 1 );

       /* Ulryk: mysle, ze juz to naprawilem */
	     if ( foo <  1 ) sprintf( buf, "$p nadaj%s si`e jedynie na przek`ask`e!",
			     obj->rodzaj == 3 ? "`a" : "e" );
	else if ( foo <  5 ) sprintf( buf, "$p zapewni%s ci godziwy posi`lek.",
			     obj->rodzaj == 3 ? "`a" : "" );
	else if ( foo < 10 ) sprintf( buf, "$p mo%s by`c ca`lkiem sytym posi`lkiem.",
			     obj->rodzaj == 3 ? "g`a" : "`ze" );
	else if ( foo < 15 ) sprintf( buf, "$p %s bardzo sytym posi`lkiem.",
			     obj->rodzaj == 3 ? "s`a" : "jest" );
	else if ( foo < 24 ) sprintf( buf, "$p wystarcz%s ci na ca`ly dzie`n jedzenia!",
			     obj->rodzaj == 3 ? "`a" : "y" );
	else                 sprintf( buf, "$p %s wi`ecej ni`z jednodniow`a racj`a!",
			     obj->rodzaj == 3 ? "s`a" : "jest" );

	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );

	if ( obj->value[ 3 ].v != 0 )
	    send_to_char( "Wyczuwasz trucizn`e!\n\r", ch );

	break;

    case ITEM_MONEY:
	act( ZM_ZAWSZE, "To przecie`z z`loto!", ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_BOAT:
	sprintf( buf, "$p wydaj%s si`e by`c solidnym sposobem na podr`o`zowanie po wodzie.",
	     obj->rodzaj == 3 ? "`a" : "e" );
	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_CORPSE_PC:
    case ITEM_CORPSE_NPC:
	send_to_char( "S`a to zwyk`le zw`loki.\n\r", ch  );
	break;

    case ITEM_FOUNTAIN:
	switch ( obj->value[ 5 ].v )
	{
	case LIQ_WATER:
	    act( ZM_ZAWSZE, "$p wydaje si`e by`c `xr`od`lem wody, kt`ore ugasi ka`zde pragnienie.",
	      ch, obj, NULL, TO_CHAR );
	    break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 7:
	case 8:
	    act( ZM_ZAWSZE, "$p pachnie alkoholem!", ch, obj, NULL, TO_CHAR );
	    break;
	case 6:
	case 15:
	    act( ZM_ZAWSZE, "P`lyn w $k ma s`lodki zapach.", ch, obj, NULL, TO_CHAR );
	    break;
	case 9:
	    act( ZM_ZAWSZE, "$p `smierdzi zgnilizn`a.", ch, obj, NULL, TO_CHAR );
	    break;
	case 10:
	    act( ZM_ZAWSZE, "Zapach z $g przynosi ci na my`sl j`edrne piersi.", ch, obj, NULL, TO_CHAR );
	    break;
	case 11:
	case 12:
	    act( ZM_ZAWSZE, "$p wydaje si`e by`c zach`et`a, aby usi`a`s`c wygodnie i si`e zrelaksowa`c.",
	      ch, obj, NULL, TO_CHAR );
	    break;
	case LIQ_BLOOD:
	    sprintf( buf, "$p %s pe`ln%c krwi!",
	      obj->rodzaj == 3 ? "s`a" : "jest",
	      obj->rodzaj == 1 ? 'y' : obj->rodzaj == 2 ? 'a' : 'e' );
	    act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	    break;
	case 14:
	    act( ZM_ZAWSZE, "Zapach z $g z jakiego`s powodu przynosi ci na my`sl morskie podr`o`ze.",
	      ch, obj, NULL, TO_CHAR );
	    break;
	}
	if ( obj->value[ 4 ].v != 0 )
	    send_to_char( "Wyczuwasz trucizn`e!\n\r", ch );
	break;

    case ITEM_LIGHT:
	time = obj->value[ 2 ].v;
	     if ( time <  0 ) sprintf( buf, "$p nigdy nie ga%s.",
			      obj->rodzaj == 3 ? "sn`a" : "`snie" );
	else if ( time <  1 ) sprintf( buf, "$p zaraz zga%s!",
			      obj->rodzaj == 3 ? "sn`a" : "`snie" );
	else if ( time <  6 ) sprintf( buf, "$p pos`lu`z%s ci mo`ze jeszcze z p`o`l nocy.",
			      obj->rodzaj == 3 ? "`a" : "y" );
	else if ( time < 12 ) sprintf( buf, "Na jutro musisz za`latwi`c sobie inne `xr`od`lo `swiat`la." );
	else if ( time < 60 ) sprintf( buf, "T%s $p wystarcz%s ci jeszcze spokojnie na kilka dni.",
			      obj->rodzaj == 1 ? "en" :
			      obj->rodzaj == 2 ? "a" :
			      obj->rodzaj == 3 ? "e" : "o",
			      obj->rodzaj == 3 ? "`a" : "y" );
	else if ( time < 168 ) sprintf( buf, "$p b`ed%s p`lon`a`c do tygodnia czasu.",
			      obj->rodzaj == 3 ? "`a" : "zie" );
	else                  sprintf( buf, "$p nadaj%s si`e idealnie na d`lugie wyprawy.",
			      obj->rodzaj == 3 ? "`a" : "e" );

	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_SHOVEL:
	sprintf( buf, "$p z pewno`sci`a si`e przyda%s by co`s odkopa`c!",
	    obj->rodzaj == 3 ? "dz`a" : "" );
	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_TREASURE:
	sprintf( buf, "$p stanowi%s skarb! Tylko czyj?",
	    obj->rodzaj == 3 ? "`a" : "" );
	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	break;

    case ITEM_SPELLBOOK:
	sprintf( buf, "$p z pewno`sci`a ci`e czego`s naucz%s.",
	    obj->rodzaj == 3 ? "`a" : "y" );
	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
	if ( ch->level >= obj->value[ 2 ].v )
	    send_to_char( "...szybko to przeczytaj, a by`c mo`ze nauczysz si`e czego`s nowego!\n\r", ch );
	break;

    case ITEM_PORTAL:
	act( ZM_ZAWSZE, "$p to portal, kt`ory dok`ad`s prowadzi.", ch, obj, NULL, TO_CHAR );
	if ( ch->level >= obj->value[ 0 ].v )
	    send_to_char( "...czujesz jak nieznana si`la zaprasza ci`e do `srodka!\n\r", ch );
	break;

    case ITEM_BEACON:
	act( ZM_ZAWSZE, "$p to magiczny drogowskaz. Kto`s bardzo chce tu wr`oci`c.", ch, obj, NULL, TO_CHAR );
	break;

    default:
	act( ZM_ZAWSZE, "Nie widzisz niczego specjalnego w $l.", ch, obj, NULL, TO_CHAR );
	break;
    }

    /* Teraz dajmy ogolne wyobrazenie o cenie przedmiotu */
    /* Lam: i tak wszystkie przedmioty maja bardzo niska wartosc */
    if ( obj->affected != NULL )
	 act( ZM_ZAWSZE, "Wyczuwasz dziwn`a moc w $l.", ch, obj, NULL, TO_CHAR );

    gold = ( obj->cost );
	 if ( gold <   100 ) sprintf( buf, "$p nie %s niczego wart%s.",
			     obj->rodzaj == 3 ? "s`a" : "jest",
			     obj->rodzaj == 1 ? "" : obj->rodzaj == 2 ? "a" : "e" );
    else if ( gold <  1000 ) sprintf( buf, "$p zdaj%s si`e mie`c jak`a`s warto`s`c.",
			     obj->rodzaj == 3 ? "`a" : "e" );
    else if ( gold <  5000 ) sprintf( buf, "$p ma%s przyzwoit`a warto`s`c.",
			     obj->rodzaj == 3 ? "j`a" : "" );
    else if ( gold < 10000 ) sprintf( buf, "$p osi`agn%s wysok`a cen`e na aukcji.",
			     obj->rodzaj == 3 ? "`a" : "ie" );
    else if ( gold < 50000 ) sprintf( buf, "$p wydaj%s si`e by`c BARDZO warto`sciow%s.",
			     obj->rodzaj == 3 ? "`a" : "e",
			     obj->rodzaj == 1 ? "y" : obj->rodzaj == 2 ? "a" : "e" );
    else                     sprintf( buf, "$p wygl`ada%s na rzecz bezcenn`a!",
			     obj->rodzaj == 3 ? "j`a" : "" );

    czypas = czy_pasuje( obj, ch );
    if ( czypas == WRONG_PERSON ) /* tylko osobisty, reszta sie zgadza */
	STC( "Ten przedmiot do ciebie nie pasuje, ale mo`zesz spr`obowa`c go dostosowa`c.\n\r", ch );
    else if ( czypas != NOTHING_WRONG )
	STC( "Ten przedmiot do ciebie wyra`xnie nie pasuje.\n\r", ch );

    if ( IS_SET( obj->wear_flags, ITEM_TAKE ) )
	act( ZM_ZAWSZE, buf, ch, obj, NULL, TO_CHAR );
    uzyj( ch, gsn_appraise, "ocenianiu przedmiot`ow" );

    return;
}


KOMENDA( do_assemble )
{
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_assemble ) || ch->pcdata->learned[ gsn_assemble ] == 0 )
    {
	send_to_char( "Nie potrafisz tego wykona`c.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Nie mo`zesz si`e skupi`c walcz`ac.\n\r", ch );
	return;
    }

    if ( ch->pcdata->learned[ gsn_initiation ] == 0 )
    {
	send_to_char( "Masz za niski stopie`n wtajemniczenia.\n\r", ch );
	return;
    }

    if ( ch->move * 10 < ch->max_move )
    {
	send_to_char( "Nie masz na tyle si`l by si`e skupi`c.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_assemble ].beats );

    if ( number_percent( ) > ch->pcdata->learned[ gsn_assemble ] * 4 / 5
	+ ( ch->pcdata->learned[ gsn_initiation ] / 5 ) )
    {
	send_to_char( "Zaczynasz medytowa`c, lecz zbyt ci`e`zko jest ci si`e skupi`c.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje medytowa`c.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    if ( is_affected( ch, gsn_assemble ) )
    {
	send_to_char( "Ju`z kontrolujesz przep`lyw energii przez swe cia`lo.\n\r", ch );
	return;
    }

    af.type      = gsn_assemble;
    af.duration  = number_fuzzy( 5 );
    af.location  = APPLY_AC;
    af.modifier  = ch->level;
    af.level     = ch->level;
    af.caster    = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( ch, &af );

    ch->move = UMAX( 1, ch->move - number_range( 20, ch->max_move / 5 ) );

    send_to_char( "Zaczynasz odczuwa`c coraz wi`eksze skupienie energii w swoim ciele.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zaczyna medytacj`e. Po chwili otacza $v tajemnicza po`swiata.", ch, NULL, NULL, TO_ROOM );

    uzyj( ch, gsn_assemble, "kontroli przep`lywu energii" );
    uzyj( ch, gsn_initiation, "wy`zszym stopniu wtajemniczenia" );

    return;
}


KOMENDA( do_awareness )
{
    AFFECT_DATA af;
    bool 	used = FALSE;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_awareness )
      || !ch->pcdata->learned[ gsn_awareness ] )
    {
	send_to_char( "Nie potrafisz tego wykona`c.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Nie mo`zesz si`e skupi`c walcz`ac.\n\r", ch );
	return;
    }

    if ( ch->move * 10 < ch->max_move )
    {
	send_to_char( "Nie masz na tyle si`l by si`e skupi`c.\n\r", ch );
	return;
    }

    if ( ch->pcdata->learned[ gsn_initiation ] == 0 )
    {
	send_to_char( "Masz za niski stopie`n wtajemniczenia.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) ) /* Qwert: slepy nie jest swiadomy :) */
	return;

    WAIT_STATE( ch, skill_table[ gsn_awareness ].beats );

    if ( number_percent( ) > ch->pcdata->learned[ gsn_awareness ] * 4 / 5 )
    {
	send_to_char( "Bezskutecznie starasz si`e zwi`ekszy`c sw`a `swiadomo`s`c otoczenia.\n\r", ch );
	return;
    }

    af.type      = gsn_awareness;
    af.duration  = ch->level / 3;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    CLEAR_BITS( af.bitvector );
    af.level = ch->level;
    af.caster = ch;

    /* nie sciagac wplywow innych umiejetnosci */
    if ( !HAS_DETECT_HIDDEN( ch ) )
    {
	SET_BIT_V( af.bitvector, AFF_DETECT_HIDDEN );
	affect_to_char( ch, &af );
	REMOVE_BIT_V( af.bitvector, AFF_DETECT_HIDDEN );
	used = TRUE;
    }

    /* Qwert: jak mu dam wykrycie magii, to az sie zdziwi! :) */
    if ( !HAS_INFRAVISION( ch ) )
    {
	SET_BIT_V( af.bitvector, AFF_INFRARED );
	affect_to_char( ch, &af );
	REMOVE_BIT_V( af.bitvector, AFF_INFRARED );
	used = TRUE;
    }

    if ( !HAS_DETECT_INVIS( ch )
      && number_percent( ) < ch->pcdata->learned[ gsn_initiation ] * 3 / 4 )
    {
	SET_BIT_V( af.bitvector, AFF_DETECT_INVIS );
	affect_to_char( ch, &af );
	used = TRUE;
	uzyj( ch, gsn_initiation, "wy`zszym stopniu wtajemniczenia" );
    }

    if ( !used )
	act( ZM_ZAWSZE, "Bezskutecznie starasz si`e zwi`ekszy`c sw`a `swiadomo`s`c otoczenia.", ch, NULL, NULL, TO_CHAR );
    else
    {
	act( ZM_ZAWSZE, "Stajesz si`e bardziej `swiadom$y otoczenia.", ch, NULL, NULL, TO_CHAR );
	ch->move = UMAX( 1, ch->move - number_range( 20, ch->max_move / 5 ) );
	uzyj( ch, gsn_awareness, "`swiadomo`sci otoczenia" );
    }

    return;
}


KOMENDA( do_concentrate )
{
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_concentrate ) || ch->pcdata->learned[ gsn_concentrate ] == 0 )
    {
	send_to_char( "Nie potrafisz tego wykona`c.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Nie mo`zesz si`e skupi`c walcz`ac.\n\r", ch );
	return;
    }

    if ( ch->pcdata->learned[ gsn_initiation ] == 0 )
    {
	send_to_char( "Masz za niski stopie`n wtajemniczenia.\n\r", ch );
	return;
    }

    if ( ch->move * 10 < ch->max_move )
    {
	send_to_char( "Nie masz na tyle si`l by si`e skupi`c.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_concentrate ].beats );

    if ( number_percent( ) > ch->pcdata->learned[ gsn_concentrate ] * 4 / 5
	+ ( ch->pcdata->learned[ gsn_initiation ] / 5 ) )
    {
	send_to_char( "Zaczynasz medytowa`c, lecz zbyt ci`e`zko jest ci si`e skupi`c.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje medytowa`c.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    if ( is_affected( ch, gsn_concentrate ) )
    {
	send_to_char( "Twa koncentracja nie mo`ze by`c g`l`ebsza.\n\r", ch );
	return;
    }

    af.type      = gsn_concentrate;
    af.duration  = number_fuzzy( 5 );
    af.location  = APPLY_HITROLL;
    af.modifier  = ch->level;
    af.level     = ch->level;
    af.caster    = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( ch, &af );

    send_to_char( "Skupiasz si`e bardziej na wszystkim co robisz.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zaczyna medytowa`c.", ch, NULL, NULL, TO_ROOM );
    ch->move = UMAX( 1, ch->move - number_range( 20, ch->max_move / 5 ) );

    uzyj( ch, gsn_concentrate, "koncentracji" );
    uzyj( ch, gsn_initiation, "wy`zszym stopniu wtajemniczenia" );

    return;
}


bool critical_hit( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;
    int l;

    if ( ch->level >= 105 && IS_NPC( victim )
      && is_authorized( ch, "zar`znij" ) )
    {
	return TRUE;
    }

    /* Vigud: tam dalej czyta ch->pcdata->learned */
    if ( IS_NPC( ch ) )
	return FALSE;

    /* Lam: krytyczne ciosy w trakcie walki tylko ponizej 20% zycia */
    if ( victim->fighting && ( victim->hit * 5 > victim->max_hit ) )
	return FALSE;

    /* Qwert: oraz gdy ch ma dosc sil by atakowac skutecznie (powyzej 20%) */
    if ( victim->fighting && ( ch->hit * 5 < ch->max_hit ) )
	return FALSE;

    /* zeby nie wyskakiwaly komunikaty, ze ktos zadal smiertelny cios niesmiertelnemu */
    if ( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL )
	return FALSE;

    /* 75% max - mysle, ze tyle wystarczy */
    if ( number_percent( ) >= ch->pcdata->learned[ gsn_critical_hit ] * 3 / 4 )
	return FALSE;

    if ( ch->klasa != CLASS_SKRYTOBOJCA )
	return FALSE;

    /* wplyw roznicy poziomow atakujacego i ofiary, kazdy poziom roznicy to 1% */
    chance = ch->level - victim->level;

    /* wplyw stopnia wycwiczenia "zabojczej celnosci", -4 do 4% */
    l = ch->pcdata->learned[ gsn_assasinate ];
    chance += l > 85 ?
	number_range( 1, l / 25 ) : 0 - ( number_range( 1, 4 ) );

    /* wplyw stopnia wycwiczenia "anatomii", -4 do 4% */
    l = ch->pcdata->learned[ gsn_anatomy ];
    chance += l > 80 ?
	number_range( 1, l / 25 ) : 0 - ( number_range( 1, 4 ) );

    chance += ( 100 - ( victim->hit * 100 / victim->max_hit ) ) / 40; /* max 2,5% */

    /* maksymalna szansa to 10,5% + roznica poziomow skrytobojcy i ofiary */
    /* ale dopiero po wycwiczeniu "zabojczej celnosci" i "anatomii" */
    if ( number_percent( ) < chance )
    {
	uzyj( ch, gsn_critical_hit, "krytycznym trafieniu" );
	if ( ch->pcdata->learned[ gsn_assasinate ] > 0 )
	    uzyj( ch, gsn_assasinate, "zab`ojczo celnym ciosie" );
	return TRUE;
    }

    return FALSE;
}


KOMENDA( do_cumulate_strength )
{
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_cumulate_strength )
      || ch->pcdata->learned[ gsn_cumulate_strength ] == 0 )
    {
	send_to_char( "Nie potrafisz tego wykona`c.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Nie mo`zesz si`e skupi`c walcz`ac.\n\r", ch );
	return;
    }

    if ( ch->pcdata->learned[ gsn_initiation ] == 0 )
    {
	send_to_char( "Masz za niski stopie`n wtajemniczenia.\n\r", ch );
	return;
    }

    if ( ch->move * 10 < ch->max_move )
    {
	send_to_char( "Nie masz na tyle si`l by si`e skupi`c.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_cumulate_strength ].beats );

    if ( number_percent( ) > ch->pcdata->learned[ gsn_cumulate_strength ] * 4 / 5
	+ ( ch->pcdata->learned[ gsn_initiation ] / 5 ) )
    {
	send_to_char( "Zaczynasz medytowa`c, lecz zbyt ci`e`zko jest ci si`e skupi`c.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje medytowa`c.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    if ( is_affected( ch, gsn_cumulate_strength ) )
    {
	send_to_char( "Nie jeste`s w stanie skumulowa`c w sobie wi`ecej si`ly.\n\r", ch );
	return;
    }

    af.type      = gsn_cumulate_strength;
    af.duration  = number_fuzzy( 5 );
    af.level     = ch->level;
    af.caster    = ch;
    CLEAR_BITS( af.bitvector );

    af.location  = APPLY_DAMROLL;
    af.modifier  = ch->level / 3;
    affect_to_char( ch, &af );

    af.location  = APPLY_STR;
    af.modifier  = 1 + ( ch->level >= 35 ) + ( ch->level >= 45 ) + ( ch->level >= 60 );
    affect_to_char( ch, &af );

    send_to_char( "Skupiasz ca`l`a energi`e cia`la by zwi`ekszy`c swoj`a si`l`e.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zaczyna medytowa`c.", ch, NULL, NULL, TO_ROOM );
    ch->move = UMAX( 1, ch->move - number_range( 20, ch->max_move / 5 ) );

    uzyj( ch, gsn_cumulate_strength, "kumulacji si`ly" );
    uzyj( ch, gsn_initiation, "wy`zszym stopniu wtajemniczenia" );

    return;
}


KOMENDA( do_death_tech )
{
    CHAR_DATA *victim = NULL;
    char       arg[ MAX_INPUT_LENGTH ];
    bool brak_nauki;

    const char *co;
    static const char *const miejsca[ 10 ] =
    {
	"splot s`loneczny",
	"skro`n",
	"t`etnic`e szyjn`a",
	"oczy",
	"podbrzusze",
	"kr`egos`lup",
	"t`etnic`e l`ed`xwiow`a",
	"grdyk`e",
	"klatk`e piersiow`a",
	"punkt krytyczny"
    };

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_death_tech ) )
    {
	send_to_char( "Nie wiesz jak si`e wykonuje technik`e `smierci.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) || get_eq_char( ch, WEAR_WIELD_2 ) )
    {
	send_to_char( "Nie mo`zesz wykona`c techniki `smierci u`zywaj`ac broni.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	if ( !ch->fighting )
	{
	    send_to_char( "Kogo chcesz uderzy`c?\n\r", ch );
	    return;
	}
	else
	    victim = FIGHTING( ch );
    }
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Chcesz si`e u`smierci`c?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE )
      || !check_blind( ch ) )
	return;

    check_killer( ch, victim );
    if ( !is_fighting( ch, victim ) )
	set_fighting( ch, victim );
    if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	set_fighting( victim, ch );

    WAIT_STATE( ch, skill_table[ gsn_death_tech ].beats );

    if ( number_percent( ) + 20 > ch->pcdata->learned[ gsn_death_tech ] )
    {
	damage( ch, victim, 0, gsn_death_tech, WEAR_NONE, FALSE );
	return;
    }

    if ( !critical_hit( ch, victim ) )
    {
	send_to_char( "Nie udaje ci si`e trafi`c w `zaden punkt krytyczny.\n\r", ch );
	uzyj( ch, gsn_death_tech, "technice `smierci" );
	damage( ch, victim, calculate_damage( ch, victim, gsn_death_tech ), gsn_death_tech, WEAR_NONE, FALSE );
	return;
    }

    if ( race_table[ victim->race ].parts == P_HS )
	co = miejsca[ number_range( 0, 8 ) ];
    else
	co = miejsca[ 9 ];

    /* teraz nie mozna tego uzywac bedac oslepionym, wiec komunikaty moga
       zakladac, ze wiemy w co celujemy i trafiamy */
    act( ZM_ZAWSZE, "Tw`oj pot`e`zny cios trafia $# w $t.", ch, co, victim, TO_CHAR );
    act( ZM_ZAWSZE, "$N osuwa si`e bezw`ladnie na ziemi`e.", ch, NULL, victim, TO_CHAR );

    act( ZM_WZROK | ZM_WID_VICT, "Pot`e`zny cios $1 trafia $# w $t.", ch, co, victim, TO_NOTVICT );
    act( ZM_WZROK | ZM_WID_VICT, "$N osuwa si`e bezw`ladnie na ziemi`e.", ch, NULL, victim, TO_NOTVICT );

    act( ZM_PRZYT, "Pot`e`zny cios $1 trafia ci`e w $t.", ch, co, victim, TO_VICT );
    send_to_char( "Osuwasz si`e bezw`ladnie na ziemi`e.\n\r", victim );

    /* jak dobrze wyjdzie, to sie szybciej uczy */
    uzyj( ch, gsn_death_tech, "technice `smierci" );
    uzyj( ch, gsn_death_tech, "technice `smierci" );

    /* Lam: to powinno go na dobre usmiercic */
    victim->hit = -10;

    /* Qwert: ch ma maksymalnie 5% szansy, ze zabicie victima czegos go nauczy.
       Przenioslem anatomie tutaj, bo wyzej nie mialo to wiekszego sensu */
    if ( IS_NPC( victim )
      && number_percent( ) <= ch->pcdata->learned[ gsn_anatomy ] / 20 )
    {
	brak_nauki = FALSE;
    }
    else
	brak_nauki = TRUE;
    damage( ch, victim, 10, TYPE_UNDEFINED, WEAR_NONE, brak_nauki );

    return;
}


/*
 * Qwert: dosc zakrecona funkcja :)
 */
KOMENDA( do_disguise )
{
    AFFECT_DATA  af;
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];
    char     zapas[ MAX_INPUT_LENGTH ];

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_disguise ) || ch->klasa != CLASS_SKRYTOBOJCA )
    {
	send_to_char( "Nie masz poj`ecia o maskowaniu.\n\r", ch );
	return;
    }

    if ( ch->pcdata->learned[ gsn_disguise ] == 0 )
    {
	send_to_char( "Nie bardzo wiesz jeszcze jak si`e maskowa`c.\n\r", ch );
	return;
    }

    /* POLE NAPRAWCZE: w razie gdyby ktos cos popsul, tu sie naprawi :) */
    if ( ( !is_affected( ch, gsn_disguise ) && ( ( *ch->false_name && !*ch->long_descr_orig )
	|| ( !*ch->false_name && *ch->long_descr_orig ) ) )
      || ( is_affected( ch, gsn_disguise ) && ( !*ch->false_name || !*ch->long_descr ) ) )
    {
	char buf[ MIL ];
	/* kombinuje, zeby nie popsuc ch long_descr, gdyby mial */
	if ( is_affected( ch, gsn_disguise ) )
	{
	    free_string( ch->long_descr );
	    if ( *ch->long_descr_orig )
		ch->long_descr = str_dup( ch->long_descr_orig );
	    else
		ch->long_descr = str_dup( "" );
	}
	else
	{
	    free_string( ch->long_descr_orig );
	    ch->long_descr_orig = str_dup( "" );
	}

	if ( *ch->false_name )
	{
	    free_string( ch->false_name );
	    ch->false_name = str_dup( "" );
	}

	if ( is_affected( ch, gsn_disguise ) )
	    affect_strip( ch, gsn_disguise );

	wiznet( "{G$N{x ma zepsute maskowanie! Naprawiam...",
		ch, NULL, NULL, WIZ_DEBUG, 0, 103 );
	sprintf( buf, "do_disguise: %s ma zepsute maskowanie!",
		ch->name );
	bug( buf, 0 );
    } /* brak return, wiec nawet nikt nie zauwazy, ze cos jest nie tak */

    if ( !check_blind( ch ) )
	return;

    if ( ch->move * 10 < ch->max_move && !is_affected( ch, gsn_disguise ) )
    {
	send_to_char( "Nie masz tyle si`ly by si`e skutecznie zamaskowa`c.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( !is_affected( ch, gsn_disguise ) && !*ch->false_name && arg[ 0 ] == '\0' )
    {
	send_to_char( "Jak chcesz si`e zamaskowa`c?\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Lepiej zajmij si`e walk`a!\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_disguise ) && arg[ 0 ] != '\0' )
    {
	send_to_char( "Najpierw przesta`n si`e maskowa`c!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_disguise ].beats );

    /* mamy opis i chcemy go zdjac (maskuj bez parametru, ale z opisem) */
    if ( is_affected( ch, gsn_disguise ) && *ch->false_name && arg[ 0 ] == '\0' )
    {
	strcpy( zapas, ch->long_descr );
	free_string( ch->long_descr );
	ch->long_descr = str_dup( ch->long_descr_orig );
	free_string( ch->long_descr_orig );
	/* mozna dac str_dup( "" ) dla jednorazowego opisu */
	ch->long_descr_orig = str_dup( zapas );
	affect_strip( ch, gsn_disguise );

	STC( "Przestajesz si`e maskowa`c.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nagle wy`lania si`e nie wiadomo sk`ad.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    /* pusty arg, ale mamy jakis opis i chcemy go "zalozyc" na siebie */
    if ( !is_affected( ch, gsn_disguise ) && *ch->false_name && arg[ 0 ] == '\0' )
    {
	strcpy( zapas, ch->long_descr );
	free_string( ch->long_descr );
	ch->long_descr = str_dup( ch->long_descr_orig );
	free_string( ch->long_descr_orig );
	ch->long_descr_orig = str_dup( zapas );

	af.type      = gsn_disguise;
	af.duration  = ( ch->level + ch->pcdata->learned[ gsn_disguise ] ) / 10;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.level     = ch->level;
	af.caster    = ch;
	CLEAR_BITS( af.bitvector );
	affect_to_char( ch, &af );

	send_to_char( "Maskujesz si`e.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nagle zlewa si`e z otoczeniem.", ch, NULL, NULL, TO_ROOM );
	ch->move = UMAX( 1, ch->move - number_range( ch->move / 15, ch->move / 5 ) );
	uzyj( ch, gsn_disguise, "maskowaniu si`e" );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    /* zmiana tylko w moby czlekoksztaltne. Mozna jeszcze ograniczyc, dajac np.
       RACE_PC_AVAIL */
    if ( !IS_NPC( victim ) || race_table[ victim->pIndexData->race ].parts != P_HS
      || !*victim->pIndexData->long_descr || !*victim->pIndexData->short_descr
      || victim->pIndexData->vnum == MOB_VNUM_KUKLA
      || victim->pIndexData->vnum < 200 ) /* limbo.are i dom.are zabezpieczam */
    {
	send_to_char( "Nie mo`zesz udawa`c kogo`s takiego!\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    act( ZM_ZAWSZE, "Przygl`adasz si`e uwa`znie wygl`adowi $!.", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n uwa`znie przygl`ada si`e $@.", ch, NULL, victim, TO_NOTVICT );
    act( ZM_ZAWSZE, "$n uwa`znie ci si`e przygl`ada.", ch, NULL, victim, TO_VICT );

    /* Qwert: te procenty ponizej ucza szacunku dla pracy krainkowcow :) */
    if ( IS_AWAKE( victim )
      && can_see( victim, ch ) /* Lam 23.3.2006 */
      && ( number_percent( ) > ch->pcdata->learned[ gsn_disguise ] / 3
				+ ( ch->level - victim->level ) / 2 ) )
    {
	act( ZM_ZAWSZE, "$N si`e zdenerwowa`l$A twoim bezczelnym zachowaniem!", ch, NULL, victim, TO_CHAR );
	act( ZM_WZROK | ZM_WID_VICT, "$N si`e zdenerwowa`l$A bezczelnym zachowaniem $1!", ch, NULL, victim, TO_NOTVICT );
	act( ZM_ZAWSZE, "Natr`etny wzrok $1 wyprowadzi`l ci`e z r`ownowagi!", ch, NULL, victim, TO_VICT );
	/* arg nie jest pusty, wiec nie chce starego opisu, czyli usuwam */
	if ( *ch->false_name )
	{
	    free_string( ch->long_descr_orig );
	    ch->long_descr_orig = str_dup( "" );
	    free_string( ch->false_name );
	    ch->false_name = str_dup( "" );
	}
	check_killer( ch, victim ); /* niepotrzebne (bo moby), ale niech bedzie */
	if ( !is_fighting( ch, victim ) )
	    set_fighting( ch, victim );
	if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	{
	    set_fighting( victim, ch );
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	return;
    }

    act( ZM_ZAWSZE, "No! Teraz ju`z wiesz co zrobi`c, by wygl`ada`c jak $N!", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n nagle z niewiadomego powodu u`smiecha si`e szeroko.", ch, NULL, NULL, TO_ROOM );
    free_string( ch->long_descr_orig );
    ch->long_descr_orig = str_dup( victim->pIndexData->long_descr );
    free_string( ch->false_name );
    strcpy( zapas, victim->pIndexData->short_descr );
    zapas[ 0 ] = UPPER( zapas[ 0 ] );
    if ( zapas[ 0 ] == '`' ) zapas[ 1 ] = UPPER( zapas[ 1 ] );
    ch->false_name = str_dup( zapas );
    uzyj( ch, gsn_disguise, "maskowaniu si`e" );

    return;
}


KOMENDA( do_dragon_tech )
{
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_dragon_tech )
      || !ch->pcdata->learned[ gsn_dragon_tech ] )
    {
	send_to_char( "Nie wiesz jak si`e wykonuje technik`e smoka.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD   )
      || get_eq_char( ch, WEAR_WIELD_2 ) )
    {
	send_to_char( "Nie mo`zesz wykona`c techniki smoka u`zywaj`ac broni.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( !ch->fighting )
    {
	if ( arg[ 0 ] == '\0' )
	{
	    send_to_char( "Pr`obujesz uderzy`c niewidzialnego przeciwnika.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n dziwnie wymachuje r`ekami.", ch, NULL, NULL, TO_ROOM );
	}
	else
	    send_to_char( "Musisz walczy`c, aby wykona`c technik`e smoka.\n\r", ch );

	return;
    }
    else if ( arg[ 0 ] == '\0' )
	victim = FIGHTING( ch );
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "To raczej bezcelowe...\n\r", ch );
	return;
    }

    if ( !is_fighting( victim, ch ) )
    {
	act( ZM_ZAWSZE, "$N nie walczy z tob`a!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    WAIT_STATE( ch, skill_table[ gsn_dragon_tech ].beats );

    if ( !check_blind( ch ) )
    {
	if ( IS_NPC( ch )
	  || number_percent( ) > 2 * ch->pcdata->learned[ gsn_walka_na_oslep ] / 3 )
	    return;
	else
	    uzyj( ch, gsn_walka_na_oslep, "walce na o`slep" );
    }

    send_to_char( "Posy`lasz pot`e`zny cios w przeciwnika.\n\r", ch );
    act( ZM_PRZYT, "$n posy`la w ciebie pot`e`zny cios.", ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_CHAR, "$n posy`la pot`e`zny cios w $#.", ch, NULL, victim, TO_NOTVICT );

    if ( number_percent( ) <= ch->pcdata->learned[ gsn_dragon_tech ] * 3 / 4
			 + 2 * ( ch->level - victim->level ) )
    {
	SET_BIT_V( victim->affected_by, AFF_DISORIENTATED );
	WAIT_STATE( victim, skill_table[ gsn_dragon_tech ].beats + 4 );
	act( ZM_PRZYT, "Si`la uderzenia sprawia, `ze tracisz na moment orientacj`e.", victim, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Si`la uderzenia sprawia, `ze $n traci na moment orientacj`e.", victim, NULL, NULL, TO_ROOM );
	uzyj( ch, gsn_dragon_tech, "technice smoka" );
	damage( ch, victim, calculate_damage( ch, victim, gsn_dragon_tech ), gsn_dragon_tech, WEAR_NONE, FALSE );
    }
    else
	damage( ch, victim, 0, gsn_dragon_tech, WEAR_NONE, FALSE );

    return;
}


KOMENDA( do_flash )
{
    OBJ_DATA  	    *obj;
    EXIT_DATA       *pexit;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    char       	     arg[ MAX_INPUT_LENGTH ];
    int        	     door;

    if ( IS_NPC( ch ) )
	return;

    if ( !ch->pcdata->learned[ gsn_flash ] || !moze_uzyc( ch, gsn_flash ) )
    {
	send_to_char( "Raczej nie wiesz jak to si`e robi.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char( "Przecie`z nie walczysz...\n\r", ch );
	return;
    }

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( obj->pIndexData->vnum == OBJ_VNUM_SILVER_POWDER )
	    break;

    if ( !obj )
    {
	send_to_char( "Nie masz srebrnego proszku.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_HOLD ) )
    {
	if ( is_affected( ch, gsn_snare )
	  && number_percent( ) > str_app[ get_curr_str( ch ) ].uwalnianie
	  && !IS_PASSING( ch ) )
	{
	    STC( "Z`lapano ci`e w sid`la! Nie mo`zesz uciec!\n\r", ch );
	    if ( !IS_NPC( ch ) )
	    {
		STC( "Tracisz 10 punkt`ow do`swiadczenia.\n\r", ch );
		gain_exp( ch, -10 );
	    }
	    WAIT_STATE( ch, skill_table[ gsn_flash ].beats );
	    return;
	}
	else if ( !is_affected( ch, gsn_snare ) )
	{
	    STC( "Z tych side`l si`e nie uwolnisz!\n\r", ch );
	    return;
	}
	else if ( IS_PASSING( ch ) )
	{
	    STC( "Przenikasz przez sid`la!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n przenika przez sid`la!", ch, NULL, NULL, TO_ROOM );
	    affect_strip( ch, gsn_snare );
	}
	else
	{
	    STC( "Rozrywasz sid`la!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n rozrywa sid`la!", ch, NULL, NULL, TO_ROOM );
	    act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz jaki`s trzask.", ch, NULL, NULL, TO_ROOM );
	    affect_strip( ch, gsn_snare );
	}
    }

    one_argument( argument, arg );

	 if ( !str_cmp( arg, "n" ) )	door = DIR_NORTH;
    else if ( !str_cmp( arg, "s" ) )	door = DIR_SOUTH;
    else if ( !str_cmp( arg, "w" ) )	door = DIR_WEST;
    else if ( !str_cmp( arg, "e" ) )	door = DIR_EAST;
    else if ( !str_cmp( arg, "g" ) )	door = DIR_UP;
    else if ( !str_cmp( arg, "d" ) )	door = DIR_DOWN;
    else if ( !str_cmp( arg, "nw" ) )	door = DIR_NORTHWEST;
    else if ( !str_cmp( arg, "sw" ) )	door = DIR_SOUTHWEST;
    else if ( !str_cmp( arg, "se" ) )	door = DIR_SOUTHEAST;
    else if ( !str_cmp( arg, "ne" ) )	door = DIR_NORTHEAST;
    /* ucieknie w losowym kierunku: */
    else 				door = BAD_DOOR;

    in_room = ch->in_room;

    if ( !check_blind( ch ) )
	door = BAD_DOOR;    /* slepy ucieka w dowolnym kierunku */

    /* podal zly kierunek, albo go nie podal... ucieka przez pierwsze lepsze
       drzwi */
    if ( door == BAD_DOOR )
	for ( door = 0; door < 10; door++ )
	    if ( ( pexit = in_room->exit[ door ] )
	      && ( to_room = pexit->to_room )
	      && !IS_SET( pexit->exit_info, EX_BURIED )
	      && !IS_SET( pexit->exit_info, EX_CLOSED )
	      && !room_is_private( to_room ) )
	    {
		break;
	    }

    if ( door == BAD_DOOR )
    {
	STC( "Nie mo`zesz st`ad uciec!\n\r", ch );
	return;
    }

   /* sprawdzenia: czy jest wyjscie, czy nie jest zamkniete i czy lokacja nie
      jest prywatna */
    if ( !( pexit = in_room->exit[ door ] )
      || !( to_room = pexit->to_room )
      || IS_SET( pexit->exit_info, EX_BURIED ) )
    {
	send_to_char( "Przecie`z nie ma wyj`scia w tym kierunku.\n\r", ch );
	return;
    }

    if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
    {
	send_to_char( "Przecie`z to wyj`scie jest zamkni`ete.\n\r", ch );
	return;
    }

    if ( room_is_private( to_room ) )
    {
	send_to_char( "To pomieszczenie jest w tej chwili przepe`lnione.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_flash ].beats );

			   /* 60% z blysku i 10% z efektywnosci */
    if ( number_percent( ) > ch->pcdata->learned[ gsn_flash ] * 3 / 5
			 + ( ch->pcdata->learned[ gsn_high_efficiency ] / 10 )
			 -   !check_blind( ch ) * 20 ) /* slepy ma mniejsza szanse */
    {
	act( ZM_ZAWSZE,"Przez nieuwag`e rozsypujesz $j podczas walki.", ch, obj, NULL, TO_CHAR );
	if ( !IS_NPC( ch ) )
	    gain_exp( ch, -5 ); /* -5, bo traci dodatkowo proszek */
	extract_obj( obj );
	return;
    }

    extract_obj( obj ); /* Qwert: bylo obj_from_char */

    send_to_char( "Rzucasz w powietrze gar`s`c srebrnego proszku i ulatniasz si`e.\n\r", ch );
    act( ZM_WZROK, "$n rzuca gar`s`c srebrnego proszku, sprawiaj`ac, `ze niczego nie widzisz.", ch, NULL, NULL, TO_ROOM );

    /* skrytobojca przerywa walke, ale przeciwnik nie wie co sie dzieje, wiec
       przez chwile walczy dalej :) */
    stop_fighting( ch, TRUE );

    char_from_room( ch );
    char_to_room( ch, to_room );

    act( ZM_WZROK | ZM_WID_CHAR, "$n pojawia si`e tu znik`ad.", ch, NULL, NULL, TO_ROOM );
    if ( !IS_NPC( ch ) )
    {
	act( ZM_ZAWSZE, "Uciek`l$o`s z pola walki! Tracisz 25 punkt`ow do`swiadczenia.",
		ch, NULL, NULL, TO_CHAR );
	gain_exp( ch, -25 );
    }
    do_look( ch, "auto" );
    uzyj( ch, gsn_flash, "u`zywaniu srebrnego proszku" );

    return;
}


KOMENDA( do_power_tech )
{
    CHAR_DATA *victim;
    char       arg[ MAX_INPUT_LENGTH ];

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_power_tech )
      || ch->pcdata->learned[ gsn_power_tech ] == 0 )
    {
	send_to_char( "Nie wiesz jak si`e wykonuje technik`e mocy.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) || get_eq_char( ch, WEAR_WIELD_2 ) )
    {
	send_to_char( "Nie mo`zesz wykona`c techniki mocy u`zywaj`ac broni.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( !ch->fighting )
    {
	if ( arg[ 0 ] == '\0' )
	{
	    send_to_char( "Pr`obujesz uderzy`c niewidzialnego przeciwnika.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n dziwnie wymachuje r`ekami.", ch, NULL, NULL, TO_ROOM );
	}
	else
	    send_to_char( "Musisz walczy`c, aby wykona`c technik`e mocy.\n\r", ch );

	return;
    }
    else if ( arg[ 0 ] == '\0' )
	victim = FIGHTING( ch );
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "To raczej bezcelowe...\n\r", ch );
	return;
    }

    if ( !is_fighting( victim, ch ) )
    {
	act( ZM_ZAWSZE, "$N nie walczy z tob`a!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    WAIT_STATE( ch, skill_table[ gsn_power_tech ].beats );

    if ( !check_blind( ch ) )
    {
	if ( IS_NPC( ch )
	  || number_percent( ) > 2 * ch->pcdata->learned[ gsn_walka_na_oslep ] / 3 )
	{
	    return;
	}
	else
	    uzyj( ch, gsn_walka_na_oslep, "walce na o`slep" );
    }

    if ( number_percent( ) <= ch->pcdata->learned[ gsn_power_tech ] * 3 / 4
			 + 2 * ( ch->level - victim->level ) )
    {
	send_to_char( "Skupiasz sw`a moc w d`loni i posy`lasz pot`e`zny cios w przeciwnika.\n\r", ch );
	uzyj( ch, gsn_power_tech, "technice mocy" );
	damage( ch, victim, calculate_damage( ch, victim, gsn_power_tech ), gsn_power_tech, WEAR_NONE, FALSE );
    }
    else
	damage( ch, victim, 0, gsn_power_tech, WEAR_NONE, FALSE );

    return;
}


KOMENDA( do_pull )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *from_room;
/*    ROOM_INDEX_DATA *to_room; */
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    int door, rev_door, i;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_pull ) || ch->pcdata->learned[ gsn_pull ] == 0 )
    {
	send_to_char( "Nie umiesz wyci`aga`c przeciwnik`ow.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
    {
	send_to_char( "Kogo i sk`ad chcesz tu wci`agn`a`c?\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Lepiej zajmij si`e walk`a!\n\r", ch );
	return;
    }

	 if ( !str_cmp( arg2, "n"  ) ) door = 0;
    else if ( !str_cmp( arg2, "e"  ) ) door = 1;
    else if ( !str_cmp( arg2, "s"  ) ) door = 2;
    else if ( !str_cmp( arg2, "w"  ) ) door = 3;
    else if ( !str_cmp( arg2, "g"  ) ) door = 4;
    else if ( !str_cmp( arg2, "d"  ) ) door = 5;
    else if ( !str_cmp( arg2, "ne" ) ) door = 6;
    else if ( !str_cmp( arg2, "se" ) ) door = 7;
    else if ( !str_cmp( arg2, "nw" ) ) door = 8;
    else if ( !str_cmp( arg2, "sw" ) ) door = 9;
    else
    {
	send_to_char( "Z jakiego kierunku chcesz tu kogo`s wci`agn`a`c?\n\r", ch );
	return;
    }

    in_room = ch->in_room;

    if ( !( pexit = in_room->exit[ door ] )
      || !( from_room = pexit->to_room ) )
    {
	STC( "Nie mo`zesz si`e tam dosta`c.\n\r", ch );
	return;
    }

    STC( "Pr`obujesz podej`s`c do swojej ofiary...\n\r", ch );

    /* Tissarius: probujemy podejsc do ofiary */
    move_char( ch, door, TRUE );

    if ( ch->deleted ) /* aktualnie zbedne - moby nie wyciagaja */
	return;

    if ( ch->in_room == in_room )
    {
	act( ZM_ZAWSZE, "Nie mo`zesz wyci`agn`a`c jej nie ruszaj`ac si`e z miejsca!", ch, NULL, NULL, TO_CHAR );
	return;
    }

    if ( ch->in_room != from_room ) /* czysciec */
	return;

    /* Tissarius: udalo nam sie ruszyc z miejsca */
    victim = get_char_room( ch, arg1 );

    WAIT_STATE( ch, skill_table[ gsn_pull ].beats );

    /* Tissarius: w ponizszych przypadkach nie wraca. Albo ktos zdazyl sobie
       pojsc, albo trafilismy nie tam, gdzie chcielismy, bo bylismy pijani, albo
       tam nikogo takiego nie bylo */
    if ( !victim )
    {
	STC( "Ze zdziwieniem zauwa`zasz, `ze nie ma tu twojej ofiary!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nagle pojawia si`e tu znik`ad i patrzy zdziwionym wzrokiem.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
    {
	act( ZM_WZROK | ZM_WID_CHAR, "$n nagle pojawia si`e tu znik`ad i patrzy zdziwionym wzrokiem.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    if ( victim->fighting )
    {
	act( ZM_ZAWSZE, "$N w`la`snie walczy. Chc`ac nie chc`ac zostajesz wci`agni`et$y do walki.", ch, NULL, victim, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n w`l`acza si`e do walki z tob`a.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_CHAR, "$n w`l`acza si`e do walki z$E $$.", ch, NULL, victim, TO_NOTVICT );

	check_killer( ch, victim );
	if ( !is_fighting( ch, victim ) )
	    set_fighting( ch, victim );
	if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	    set_fighting( victim, ch );
	return;
    }

    if ( ch == victim )
    {
	act( ZM_ZAWSZE, "Przecie`z to bezcelowe.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nagle pojawia si`e tu znik`ad i patrzy zdziwionym wzrokiem.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    act( ZM_ZAWSZE, "`Lapiesz $#...", ch, NULL, victim, TO_CHAR );
    act( ZM_PRZYT, "$n `lapie ci`e...", ch, NULL, victim, TO_VICT );
    /* robi to z zaskoczenia, wiec inni nie widza... */
    /* Qwert: juz widza */

    /* Tissarius: szukamy drzwi, ktore nas zaprowadza z powrotem */
    /* Vigud 02.02.2011: .odwrotny zamiast petli */
    i = kierunki[ door ].odwrotny;
    if ( from_room->exit[ i ] && from_room->exit[ i ]->to_room == in_room )
	rev_door = i;
    else
	rev_door = -1;

    /* a jesli mu nie wyjdzie? */
    /* Lam: mala, moze chwilowa, zmiana: tu blokuje wyjscie -1 */
    if ( rev_door == -1
      || !can_push_pull( ch, victim, from_room, in_room, rev_door )
      || ( number_percent( ) > ch->pcdata->learned[ gsn_pull ]
	   + ( 5 * ( ch->level - victim->level ) ) + ( ch->pcdata->learned[ gsn_sneak ] / 10 ) ) )
    {
	act( ZM_ZAWSZE, "...ale jako`s nie doceni`l$o`s $C si`ly.", ch, NULL, victim, TO_CHAR );
	act( ZM_PRZYT, "...ale jako`s nie doceni`l$a twojej si`ly.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "$N odwraca si`e, by stwierdzi`c, `ze $n $V szarpie.", ch, NULL, victim, TO_NOTVICT );

	check_killer( ch, victim );
	if ( !is_fighting( ch, victim ) )
	    set_fighting( ch, victim );
	if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	{
	    set_fighting( victim, ch );
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	return;
    }

    /* probujemy wrocic... */
    if ( rev_door != -1 )
	move_char( ch, rev_door, TRUE );

    if ( ch->deleted )
	return;

    /* Tissarius: nie udalo sie */
    if ( ch->in_room == from_room )
    {
	send_to_char( "Nie jeste`s w stanie wr`oci`c do poprzedniego pomieszczenia.\n\r", ch );

	act( ZM_PRZYT, "$N rzuca si`e na ciebie ze zlo`sci`a...", ch, NULL, victim, TO_VICT );
	act( ZM_PRZYT, "...co za bezczelno`s`c, trzeba $u dokopa`c!", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_VICT, "$N nagle rzuca si`e na $3!", ch, NULL, victim, TO_NOTVICT );

	check_killer( ch, victim );
	if ( !is_fighting( ch, victim ) )
	    set_fighting( ch, victim );
	if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	{
	    set_fighting( victim, ch );
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	return;
    }
    /* ...ale zauwaza, ze victim nagle znika do innej lokacji... */
    /* Tissarius: Wracamy...*/

    /* Qwert: dalem przed move_char victima, ale po move_char ch, bo wyswietlalo
       sie w zlym miejscu (ch tego nie zobaczy) :) */
    if ( victim->in_room == from_room && ch->in_room != from_room )
	act( ZM_WZROK | ZM_WID_CHAR, "$n nagle st`ad znika!", victim, NULL, NULL, TO_ROOM );

    /* Lam: tu nie moze byc rev_door -1 */
    move_char( victim, rev_door, TRUE );

    if ( victim->deleted )
	return;

    /* Tissarius: ale czy poszedl do lokacji pierwotnej? */
    if ( victim->in_room == in_room )
    {
	send_to_char( "...i przyci`agasz do siebie.\n\r", ch );
	send_to_char( "...i ku swemu zdziwieniu l`adujesz w innym miejscu!\n\r", victim );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n nagle tu wpada trzymaj`ac $#!", ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR | ZM_WID_VICT, "Nagle przybiega tu $N.", ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT, "Nagle wpada tu $n.", ch, NULL, victim, TO_NOTVICT );
	/* niech moby siedza i sie nie ruszaja (o ile nie jest to sprzedawca
	   przedmiotow albo czarow) */
	if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_SENTINEL )
	  && !IS_SET( ch->act, ACT_DONT_COME_BACK )
	  && !( victim->pIndexData->pShop || victim->pIndexData->pHealer ) )
	    SET_BIT( victim->act, ACT_DONT_COME_BACK );
	uzyj( ch, gsn_pull, "wyci`aganiu" );
    }
    else /* smierc victim? */
	act( ZM_ZAWSZE, "Nie uda`lo ci si`e wyci`agn`ac $!!", ch, NULL, victim, TO_CHAR );

    return;
}


KOMENDA( do_reinforce_blade )
{
    OBJ_DATA	*obj;
    AFFECT_DATA	*af;
    char	arg[ MAX_INPUT_LENGTH ];
    int		szansa;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_reinforce_blade )
      || ch->pcdata->learned[ gsn_reinforce_blade ] == 0 )
    {
	send_to_char( "Nie umiesz wzmacnia`c broni.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	send_to_char( "Jak`a bro`n chcesz wzmocni`c?\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Nie masz na to czasu podczas walki.\n\r", ch );
	return;
    }

    if ( !check_blind( ch )
      || pijane_przedmioty( ch ) )
    {
	return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	send_to_char( "Nie masz tej broni przy sobie.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_WEAPON )
    {
	send_to_char( "To nie jest bro`n.\n\r", ch );
	return;
    }

    if ( obj->level > ch->level )
    {
	send_to_char( "Nie mo`zesz wzmacnia`c broni o poziomie wy`zszym od twojego.\n\r",
		     ch );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_REINFORCED ) )
    {
	send_to_char( "Ta bro`n ju`z jest wzmocniona.\n\r", ch );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_CHAOS ) )
    {
	/* niech wreszcie zaczna korzystac z ostrza lodu i ognia :) */
	send_to_char( "Ta bro`n jest zbyt chaotyczna by j`a wzmocni`c.\n\r", ch );
	return;
    }

    if ( ( !IS_NPC( ch ) && !IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	&& is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) )
      || ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_INVERT_CLASSES )
	&& !is_name2( class_table[ ch->klasa ].who_name, obj->anti_class ) ) )
    {
	send_to_char( "Nie potrafisz wzmocni`c takiej broni.\n\r", ch );
	return;
    }

    /* bron jest, ale czy skrytobojca ma wystarczajaco oleju w glowie? */
    if ( get_curr_wis( ch ) < 19 )
    {
	send_to_char( "Nie potrafisz sobie przypomnie`c, jak to si`e robi`lo...\n\r", ch );
	return;
    }

    /* zeby sie nie pocharatal... */
    /* Vigud 13.4.2010: zamiast prewencyjnie zabezpieczac postac przed
       wzmacnianiem broni, kiedy nie powinna tego robic, wole zeby jej stan mial
       wplyw na efekt - decyzje zostawiam graczowi. Jeszcze wspomne o tym, ze
       przy tej wersji, gracz mogl sobie czekac i czekac, ale od czekania
       zrecznosc sie nie zmienia...

    if ( get_curr_dex( ch ) < 19 || ch->pcdata->condition[ COND_DRUNK ] > 0 )
    {
	send_to_char( "R`ece ci si`e za bardzo trz`es`a. Lepiej poczeka`c ni`z zrobi`c sobie krzywd`e.\n\r", ch );
	return;
    } */

    act( ZM_ZAWSZE, "Zaczynasz obr`obk`e $g w celu $T wzmocnienia...", ch, obj,
	obj->rodzaj == 3 ? "ich" : obj->rodzaj == 2 ? "jej" : "jego", TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n zaczyna co`s robi`c przy $l.", ch, obj, NULL, TO_ROOM );

    WAIT_STATE( ch, skill_table[ gsn_reinforce_blade ].beats );

    /* sprawdzamy czy bron juz zmienia obrazenia */
    for ( af = obj->affected; af; af = af->next )
	if ( af->location == APPLY_DAMROLL )
	    break;

    if ( !af ) /* nie ma takiego wplywu, wiec go tworzymy */
    {
	af = new_affect( );
	af->next = obj->affected;
	obj->affected = af;
	af->deleted = FALSE;
	af->type = gsn_reinforce_blade;
	af->duration = -1; /* Qwert: tak ladniej wyglada */
	CLEAR_BITS( af->bitvector );
	af->location = APPLY_DAMROLL;
	af->modifier = 0;
    }

    SET_BIT_V( obj->extra_flags, ITEM_REINFORCED );

    szansa = 0
	+ ch->pcdata->learned[ gsn_reinforce_blade ] / 2
	+ 2 * ( ch->level - obj->level )
	+ 2 * ( get_curr_dex( ch ) - 20 ) /* kara/nagroda za zrecznosc */
	- 2 * ch->pcdata->condition[ COND_DRUNK ]; /* kara za wplyw alkoholu */

    /* teraz czy mu wyjdzie? */
    if ( number_percent( ) > szansa )
    {
	send_to_char( "Co`s ci chyba nie wysz`lo... bro`n wygl`ada gorzej ni`z przedtem!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n jest niezadowolon$y z wynik`ow w`lasnej pracy.", ch, obj, NULL, TO_ROOM );

	/* psujemy stosownie obrazenia: im wyzszy poziom, tym mniej popsuje, bo
	   w koncu cos tam umie */
	af->modifier -= ( 110 - ch->level ) / 4;

	return;
    }

    act( ZM_ZAWSZE, "Po d`lugich i `zmudnych zabiegach, $p nabiera$T tajemniczego blasku!",
	ch, obj, obj->rodzaj == 3 ? "j`a" : "", TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$n ko`nczy prac`e nad sw`a broni`a.", ch, obj, NULL, TO_ROOM );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_OBJ1, "$c $p nabiera$T tajemniczego blasku!",
	ch, obj, obj->rodzaj == 3 ? "j`a" : "", TO_ROOM );

    /* dzieki temu chyba nikt poza nami nie bedzie mogl uzyc tej broni */
    SET_BIT_V( obj->extra_flags, ITEM_LOYAL );

    /* Qwert: mozna dopisac nawet plec i antyrase :) */
    obj->level = UMAX( number_fuzzy( ch->level - 3 ), obj->level );
    if ( !IS_OBJ_STAT( obj, ITEM_PRIVATE )
      || !str_cmp( ch->name, obj->owner ) )
    {
	free_string( obj->owner );
	obj->owner = str_dup( ch->name );
	SET_BIT_V( obj->extra_flags, ITEM_PRIVATE );
    }

    af->modifier += ch->level / 2; /* dodajemy stosowne obrazenia */

    uzyj( ch, gsn_reinforce_blade, "ulepszaniu broni" );

    return;
}


KOMENDA( do_runes )
{
    CHAR_DATA	*victim = NULL;
    char	 arg1[ MAX_INPUT_LENGTH ];
    char	 arg2[ MAX_INPUT_LENGTH ];
    char	 buf [ MAX_STRING_LENGTH ];
    const char	*co;
    const char	*czego;
    int		 i;
    int		 k = -1;

    static const char *const runy[ 4 ][ 2 ] =
    {
	{ "o`slepiaj`acy promie`n", "`swiat`la" },
	{ "kul`e", "ognia" },
	{ "sopel", "lodu" },
	{ "strza`l`e", "trucizny" }
    };

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_runes ) )
    {
	send_to_char( "Nie masz poj`ecia jak nakre`sla`c te tajemne znaki.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) || get_eq_char( ch, WEAR_WIELD_2 ) )
    {
	send_to_char( "Nie mo`zesz nakre`sla`c tajemnych znak`ow runicznych u`zywaj`ac broni.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[ 0 ] == '\0' )
    {
	send_to_char( "Jakiej runy chcesz u`zy`c?\n\r", ch );
	return;
    }

    for ( i = 0; i < 4; i++ )
	if ( !str_prefix( arg1, runy[ i ][ 1 ] ) )
	{
	    co = runy[ i ][ 0 ];
	    czego = runy[ i ][ 1 ];
	    k = i;
	    break;
	}

    if ( k == -1 )
    {
	act( ZM_ZAWSZE, "Nie istniej`a runy $t.", ch, arg1, NULL, TO_CHAR );
	return;
    }

    if ( arg2[ 0 ] == '\0' )
    {
	if ( !ch->fighting )
	{
	    sprintf( buf, "W kogo chcesz pos`la`c %s %s?\n\r", co, czego );
	    send_to_char( buf, ch );
	    return;
	}
	else
	    victim = FIGHTING( ch );
    }
    else if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( ch->pcdata->learned[ gsn_runes ] == 0 )
    {
	send_to_char( "Nie posiadasz tajemnej wiedzy runicznej.\n\r", ch );
	return;
    }

    if ( ( k == 0 && ch->pcdata->learned[ gsn_rune_flare  ] == 0 )
      || ( k == 1 && ch->pcdata->learned[ gsn_rune_fire   ] == 0 )
      || ( k == 2 && ch->pcdata->learned[ gsn_rune_ice    ] == 0 )
      || ( k == 3 && ch->pcdata->learned[ gsn_rune_poison ] == 0 ) )
    {
	act( ZM_ZAWSZE, "Nie posiadasz tajemnej wiedzy o runach $t.", ch, czego, NULL, TO_CHAR );
	return;
    }

    if ( number_percent( ) > ch->pcdata->learned[ gsn_runes ] )
    {
	send_to_char( "Nie mo`zesz sobie przypomnie`c jak to si`e robi`lo.\n\r", ch );
	WAIT_STATE( ch, 4 );
	return;
    }

    if ( ch->move * 20 < ch->max_move )
    {
	send_to_char( "Nie masz si`ly u`zywa`c znak`ow runicznych.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Nie mo`zna na sobie u`zywa`c znak`ow runicznych.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    check_killer( ch, victim );

    uzyj( ch, gsn_runes, "runach" );
    if ( !is_fighting( ch, victim ) )
	set_fighting( ch, victim );
    if ( !is_fighting( victim, ch ) && ZDOLNY_DO_WALKI( victim ) )
	set_fighting( victim, ch );

    act( ZM_WZROK | ZM_WID_CHAR, "$n kre`sli w powietrzu tajemny znak.", ch, NULL, NULL, TO_ROOM );
    /* kazda proba niech kosztuje 5% ruchu */
    ch->move = UMAX( ch->move - ch->max_move / 20, 1 );

    switch ( k )
    {
	case 0: rune_flare( ch, victim );	break;
	case 1: rune_fire( ch, victim );	break;
	case 2: rune_ice( ch, victim );		break;
	case 3: rune_poison( ch, victim );	break;
	default: break;
    }

    return;
}


void rune_flare( CHAR_DATA *ch, CHAR_DATA *victim )
{
    AFFECT_DATA af;
    int szansa;

    WAIT_STATE( ch, skill_table[ gsn_rune_flare ].beats );

    if ( !check_blind( ch ) )
    {
	if ( IS_NPC( ch )
	  || number_percent( ) > 2 * ch->pcdata->learned[ gsn_walka_na_oslep ] / 3 )
	{
	    return;
	}
	else
	    uzyj( ch, gsn_walka_na_oslep, "walce na o`slep" );
    }

    szansa = 0
	+ ch->pcdata->learned[ gsn_rune_flare ] / 5 * 4
	+ ch->pcdata->learned[ gsn_runes ] / 5;

    if ( IS_AFFECTED( victim, AFF_BLIND )
      || IS_SET( race_table[ victim->race ].race_abilities, RACE_NO_EYES )
      || number_percent( ) > szansa
      || saves_spell( ch->level, victim ) )
    {
	send_to_char( "Kontur runy `swiat`la zaczyna b`lyszcze`c, lecz po chwili ga`snie.\n\r", ch );
	act( ZM_WZROK, "Co`s b`lyska, ale po chwili zanika.", ch, NULL, NULL, TO_ROOM );

	return;
    }

    send_to_char( "Kontur runy `swiat`la zaczyna b`lyszcze`c.\n\r", ch );
    act( ZM_ZAWSZE, "Nagle z jej `srodka strzela jasny promie`n `swiat`la o`slepiaj`ac $#.", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK, "Nagle strzela w ciebie jasny promie`n `swiat`la... niczego nie widzisz!", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "Nagle jasny promie`n `swiat`la strzela w $# o`slepiaj`ac $V.", ch, NULL, victim, TO_NOTVICT );

    af.type      = gsn_rune_flare;
    af.duration  = number_fuzzy( ch->level / 10 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.level     = ch->level;
    af.caster    = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_BLIND );
    affect_to_char( victim, &af );

    /* udana proba + 6.66% :) */
    ch->move = UMAX( ch->move - ch->max_move / 15, 1 );
    uzyj( ch, gsn_rune_flare, "runie `swiat`la" );

    return;
}


void rune_fire( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int szansa;

    WAIT_STATE( ch, skill_table[ gsn_rune_fire ].beats );

    if ( !check_blind( ch ) )
    {
	if ( IS_NPC( ch )
	  || number_percent( ) > 2 * ch->pcdata->learned[ gsn_walka_na_oslep ] / 3 )
	{
	    return;
	}
	else
	    uzyj( ch, gsn_walka_na_oslep, "walce na o`slep" );
    }

    szansa = 0
	+ ch->pcdata->learned[ gsn_rune_fire ] / 5 * 4
	+ ch->pcdata->learned[ gsn_runes ] / 5;

    if ( number_percent( ) > szansa
      || saves_spell( ch->level, victim ) )
    {
	send_to_char( "Kontur runy ognia zaczyna b`lyszcze`c, lecz po chwili ga`snie.\n\r", ch );
	act( ZM_WZROK, "Co`s b`lyska, ale po chwili zanika.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    send_to_char( "Kontur runy ognia zaczyna b`lyszcze`c.\n\r", ch );
    act( ZM_ZAWSZE, "Nagle z jej `srodka strzela ognista kula w kierunku $!.", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK, "Nagle strzela w ciebie ognista kula!", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK, "Nagle ognista kula strzela w $#.", ch, NULL, victim, TO_NOTVICT );

    /* Lam: przy poprzednim wzorze na obrazenia, kat padal w 4 kolejkach.
       Zmienilem, ale nie wiem czy jest odpowiednio. */
    /* Qwert: trudno dobrac tu odpowiednie obrazenia (sztorm lodowy + 2 ;)) */
    /* udana proba + 10% :) */
    ch->move = UMAX( ch->move - ch->max_move / 10, 1 );
    uzyj( ch, gsn_rune_fire, "runie ognia" );
    damage( ch, victim, dice( ch->level, 9 ), gsn_rune_fire, WEAR_NONE, FALSE );

    return;
}


void rune_ice( CHAR_DATA *ch, CHAR_DATA *victim )
{
    AFFECT_DATA af;
    int szansa;

    WAIT_STATE( ch, skill_table[ gsn_rune_ice ].beats );

    if ( !check_blind( ch ) )
    {
	if ( IS_NPC( ch )
	  || number_percent( ) > 2 * ch->pcdata->learned[ gsn_walka_na_oslep ] / 3 )
	{
	    return;
	}
	else
	    uzyj( ch, gsn_walka_na_oslep, "walce na o`slep" );
    }

    szansa = 0
	+ ch->pcdata->learned[ gsn_rune_ice ] / 5 * 4
	+ ch->pcdata->learned[ gsn_runes ] / 5;

    /* losuje raz zrecznosc, raz sile */
    if ( number_percent( ) > szansa
      || saves_spell( ch->level, victim ) )
    {
	send_to_char( "Kontur runy lodu zaczyna b`lyszcze`c, lecz po chwili ga`snie.\n\r", ch );
	act( ZM_WZROK, "Co`s b`lyska, ale po chwili zanika.", ch, NULL, NULL, TO_NOTVICT );
	return;
    }

    send_to_char( "Kontur runy lodu zaczyna b`lyszcze`c.\n\r", ch );
    act( ZM_ZAWSZE, "Nagle z jej `srodka strzela w kierunku $! sopel lodu.", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK, "Nagle strzela w ciebie sopel lodu!", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK, "Nagle sopel lodu strzela w $!.", ch, NULL, victim, TO_NOTVICT );

    /* udana proba + 8.33% :) */
    ch->move = UMAX( ch->move - ch->max_move / 12, 1 );
    uzyj( ch, gsn_rune_ice, "runie lodu" );
    damage( ch, victim, number_range( ch->level / 2, ch->level * 3 / 2 ), gsn_rune_ice, WEAR_NONE, FALSE );

    if ( ch->deleted
      || victim->deleted
      || number_bits( 1 ) )
    {
	return; /* Qwert */
    }

    /* na wniosek Mole'a - zobaczymy co dalej :) */
    act( ZM_ZAWSZE, "Kawa`lek sopla wbija si`e w $# os`labiaj`ac $V.", ch, NULL, victim, TO_CHAR );
    act( ZM_PRZYT, "Kawalek sopla wbija si`e w twoje cia`lo os`labiaj`ac ci`e!", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "Kawa`lek sopla wbija si`e w $# os`labiaj`ac $V.", ch, NULL, victim, TO_NOTVICT );

    af.type      = gsn_rune_ice;
    af.duration  = number_fuzzy( ch->level / 10 + 1 );
    af.location  = number_bits( 1 ) ? APPLY_DEX : APPLY_STR;
    af.modifier  = 0 - number_fuzzy( 1 );
    af.level     = ch->level;
    af.caster    = ch;
    CLEAR_BITS( af.bitvector );
    affect_join( victim, &af, ENH_AFFECT );

    return;
}


void rune_poison( CHAR_DATA *ch, CHAR_DATA *victim )
{
    AFFECT_DATA af;
    int szansa;

    WAIT_STATE( ch, skill_table[ gsn_rune_poison ].beats );

    if ( !check_blind( ch ) )
    {
	if ( IS_NPC( ch )
	  || number_percent( ) > 2 * ch->pcdata->learned[ gsn_walka_na_oslep ] / 3 )
	    return;
	else
	    uzyj( ch, gsn_walka_na_oslep, "walce na o`slep" );
    }

    szansa = 0
	+ ch->pcdata->learned[ gsn_rune_poison ] / 5 * 4
	+ ch->pcdata->learned[ gsn_runes ] / 5;

    if ( IS_SET( race_table[ victim->race ].race_abilities, RACE_POISONIMM )
      || number_percent( ) > szansa
      || saves_spell( ch->level, victim ) )
    {
	send_to_char( "Kontur runy trucizny zaczyna b`lyszcze`c, lecz po chwili ga`snie.\n\r", ch );
	act( ZM_WZROK, "Co`s b`lyska, ale po chwili zanika.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    send_to_char( "Kontur runy trucizny zaczyna b`lyszcze`c.\n\r", ch );
    act( ZM_ZAWSZE, "Nagle z jej `srodka strzela ciemna strza`la trafiaj`ac $#.", ch, NULL, victim, TO_CHAR );
    act( ZM_PRZYT, "Nagle strzela w ciebie ciemna strza`la... czujesz si`e `xle!", victim, NULL, NULL, TO_CHAR );
    act( ZM_WZROK, "Nagle ciemna strza`la strzela w $#.", ch, NULL, victim, TO_NOTVICT );

    af.type      = gsn_rune_poison;
    af.duration  = number_fuzzy( ch->level / 10 );
    af.location  = APPLY_STR;
    af.modifier  = 0 - number_fuzzy( 2 );
    af.level     = ch->level;
    af.caster    = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_POISON );
    affect_join( victim, &af, ENH_AFFECT );

    /* udana proba + 7.14% :) */
    ch->move = UMAX( ch->move - ch->max_move / 14, 1 );
    uzyj( ch, gsn_rune_poison, "runie trucizny" );

    return;
}


KOMENDA( do_search )
{
    AFFECT_DATA af;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_search ) )
    {
	send_to_char( "Nie umiesz przeszukiwac otoczenia!\n\r", ch );
	return;
    }

    if ( ch->pcdata->learned[ gsn_search    ] == 0
      || ch->pcdata->learned[ gsn_awareness ] == 0 )
    {
	send_to_char( "Wyszukiwanie nie le`zy w granicach twych mo`zliwo`sci!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( ch->fighting )
    {
	send_to_char( "Lepiej zajmij si`e walk`a!\n\r", ch );
	return;
    }

    if ( ch->move * 10 < ch->max_move && !is_affected( ch, gsn_search ) )
    {
	send_to_char( "Nie masz si`ly zwraca`c wi`eksz`a uwag`e na otoczenie.\n\r", ch );
	return;
    }

    if ( ch->fighting && !is_affected( ch, gsn_search ) )
    {
	send_to_char( "Nie mo`zesz si`e skupi`c walcz`ac.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_search ].beats );

    if ( !is_affected( ch, gsn_search ) &&
	number_percent( ) > ch->pcdata->learned[ gsn_search ] / 2
			+ ( ch->pcdata->learned[ gsn_awareness ] / 3 ) )
    {
	send_to_char( "Nie potrafisz si`e skupi`c na otoczeniu.\n\r", ch );
	ch->move = UMAX( ch->move - ch->max_move / 20, 1 ); /* 5% wystarczy */
	return;
    }

    if ( is_affected( ch, gsn_search ) )
    {
	send_to_char( "Przestajesz zwraca`c wi`eksz`a uwag`e na otoczenie.\n\r", ch );
	affect_strip( ch, gsn_search );
	return;
    }

    af.type      = gsn_search;
    af.duration  = ch->level / 2;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.level     = ch->level;
    af.caster    = ch;
    CLEAR_BITS( af.bitvector );
    affect_to_char( ch, &af );

    send_to_char( "Zaczynasz zwraca`c wi`eksz`a uwag`e na otoczenie.\n\r", ch );

    return;
}


KOMENDA( do_speed_tech )
{
    CHAR_DATA *victim;
    AFFECT_DATA *paf;
    AFFECT_DATA af;
    char arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *in_room;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_speed_tech )
      || ch->pcdata->learned[ gsn_speed_tech ] == 0 )
    {
	send_to_char( "Nie wiesz jak si`e wykonuje technik`e szybko`sci.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD   )
      || get_eq_char( ch, WEAR_WIELD_2 ) )
    {
	send_to_char( "Nie mo`zesz wykona`c techniki szybko`sci u`zywaj`ac broni.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
    {
	if ( IS_NPC( ch )
	  || number_percent( ) > 2 * ch->pcdata->learned[ gsn_walka_na_oslep ] / 3 )
	{
	    return;
	}
	else
	    uzyj( ch, gsn_walka_na_oslep, "walce na o`slep" );
    }

    one_argument( argument, arg );

    if ( !ch->fighting )
    {
	if ( arg[ 0 ] == '\0' )
	{
	    send_to_char( "Pr`obujesz uderzy`c niewidzialnego przeciwnika.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n dziwnie wymachuje r`ekami.", ch, NULL, NULL, TO_ROOM );
	}
	else
	    send_to_char( "Musisz walczy`c, aby wykona`c technik`e szybko`sci.\n\r", ch );

	return;
    }
    else if ( arg[ 0 ] == '\0' )
	victim = FIGHTING( ch );
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( !is_fighting( victim, ch ) )
    {
	act( ZM_ZAWSZE, "$N nie walczy z tob`a!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "To raczej bezcelowe...\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    /* Qwert: podoba mi sie ta technika :) */
    if ( number_percent( ) <= ch->pcdata->learned[ gsn_speed_tech ] * 3 / 4
			 + 2 * ( ch->level - victim->level ) )
    {
	send_to_char( "Koncentrujesz si`e i posy`lasz seri`e cios`ow w kierunku przeciwnika.\n\r", ch );
	act( ZM_PRZYT, "$n posy`la w ciebie seri`e cios`ow.", ch, NULL, victim, TO_VICT );
	act( ZM_WZROK | ZM_WID_CHAR, "$n posy`la seri`e cios`ow w $#.", ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_WID_VICT, "$n posy`la seri`e cios`ow w $#.", ch, NULL, victim, TO_NOTVICT );
	/* tu powinno byc uzyj, bo dalej moze nam victim umrzec, a skoro sie udalo, to uzywamy :) */
	uzyj( ch, gsn_speed_tech, "technice szybko`sci" );

	in_room = ch->in_room;
	damage( ch, victim, calculate_damage( ch, victim, gsn_speed_tech ), gsn_speed_tech, WEAR_NONE, FALSE );
	if ( !ch->deleted && !victim->deleted
	  && in_room == ch->in_room && in_room == victim->in_room )
	{
	    damage( ch, victim, calculate_damage( ch, victim, gsn_speed_tech ), gsn_speed_tech, WEAR_NONE, FALSE );
	    if ( !ch->deleted && !victim->deleted
	      && in_room == ch->in_room && in_room == victim->in_room )
		damage( ch, victim, calculate_damage( ch, victim, gsn_speed_tech ), gsn_speed_tech, WEAR_NONE, FALSE );
	}
    }
    else
	damage( ch, victim, 0, gsn_speed_tech, WEAR_NONE, FALSE );

    /* opoznienie przerzucone na koniec */
    for ( paf = ch->affected; paf; paf = paf->next )
	if ( !paf->deleted && paf->type == gsn_speed_tech )
	    break;

    WAIT_STATE( ch, ( paf && ( !paf->deleted ) ? paf->duration * 2 : 0 )
		   + skill_table[ gsn_speed_tech ].beats );

    if ( paf && ( !paf->deleted ) )
	paf->duration = UMIN( paf->duration + 1, 10 );
    else
    {
	af.type       = gsn_speed_tech;
	af.duration   = 1;
	af.location   = APPLY_NONE;
	af.modifier   = 0;
	af.level      = ch->level;
	af.caster     = ch;
	CLEAR_BITS( af.bitvector );
	affect_to_char( ch, &af );
    }

    return;
}


KOMENDA( do_strike )
{
    OBJ_DATA        *obj;
    CHAR_DATA       *victim;
    EXIT_DATA       *pexit;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    char       arg1[ MAX_INPUT_LENGTH ];
    char       arg2[ MAX_INPUT_LENGTH ];
    int        door;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_strike ) )
    {
	send_to_char( "Zostaw to do`swiadczonym skrytob`ojcom.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !is_affected( ch, gsn_sneak ) )
    {
	send_to_char( "Musisz si`e skrada`c, aby ci to wysz`lo.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Lepiej si`e zajmij walk`a!\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_HOLD ) )
    {
	if ( !is_affected( ch, gsn_snare )
	  || ( number_percent( )
	     > str_app[ get_curr_str( ch ) ].uwalnianie
	    && !IS_PASSING( ch ) ) )
	{
	    STC( "Schwytano ci`e w sid`la! Nie mo`zesz si`e porusza`c!\n\r", ch );
	    WAIT_STATE( ch, skill_table[ gsn_strike ].beats );
	    return;
	}
	else if ( IS_PASSING( ch ) )
	{
	    STC( "Przenikasz przez sid`la!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n przenika przez sid`la!", ch, NULL, NULL, TO_ROOM );
	    affect_strip( ch, gsn_snare );
	}
	else
	{
	    STC( "Rozrywasz sid`la!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n rozrywa sid`la!", ch, NULL, NULL, TO_ROOM );
	    act( ZM_SLUCH | ZM_W_WID_CHAR, "S`lyszysz jaki`s trzask.", ch, NULL, NULL, TO_ROOM );
	    affect_strip( ch, gsn_snare );
	}
    }

    in_room = ch->in_room;

    if ( IS_AFFECTED( ch, AFF_CHARM )
	&& ch->master
	&& in_room == ch->master->in_room )
    {
	send_to_char( "Co? Jeszcze nie pad`l rozkaz by opu`sci`c to pomieszczenie!\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    /* zamiana kolejnosci argumentow - <cel> <kierunek>, jak w wyciaganiu */
    if ( arg2[ 0 ] == '\0' )
    {
	send_to_char( "W jakim kierunku chcesz zaatakowa`c?\n\r", ch );
	return;
    }

    if ( arg1[ 0 ] == '\0' )
    {
	send_to_char( "Kogo chcesz zaskoczy`c swoim atakiem?\n\r", ch );
	return;
    }

	 if ( !str_cmp( arg2, "n" ) ) door = 0;
    else if ( !str_cmp( arg2, "e" ) ) door = 1;
    else if ( !str_cmp( arg2, "s" ) ) door = 2;
    else if ( !str_cmp( arg2, "w" ) ) door = 3;
    else if ( !str_cmp( arg2, "g" ) ) door = 4;
    else if ( !str_cmp( arg2, "d" ) ) door = 5;
    else if ( !str_cmp( arg2, "ne" ) ) door = 6;
    else if ( !str_cmp( arg2, "se" ) ) door = 7;
    else if ( !str_cmp( arg2, "nw" ) ) door = 8;
    else if ( !str_cmp( arg2, "sw" ) ) door = 9;
    else
    {
      send_to_char( "W kt`or`a stron`e chcesz zaatakowa`c?\n\r", ch );
      return;
    }

    /* sprawdzenia: czy jest wyjscie, czy nie jest zamkniete i czy lokacja nie
     * jest prywatna */
    if ( !( pexit = in_room->exit[ door ] ) || !( to_room = pexit->to_room )
      || IS_SET( pexit->exit_info, EX_BURIED ) )
    {
	send_to_char( "Przecie`z nie ma wyj`scia w tym kierunku.\n\r", ch );
	return;
    }

    if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
    {
	send_to_char( "Przecie`z to wyj`scie jest zamkni`ete.\n\r", ch );
	return;
    }

    if ( !can_enter_room( ch, to_room, TRUE ) )
	return;

    if ( !( obj = get_eq_char( ch, WEAR_WIELD ) )
      || ( obj->value[ 3 ].v != 11 && obj->value[ 3 ].v != 2 ) )
    {
	send_to_char( "Musisz dzier`zy`c odpowiedni`a bro`n.\n\r", ch );
	return;
    }

    PRZERWIJ_KOPANIE( ch, TRUE );
    PRZERWIJ_KARMPOJ( ch, TRUE );

    char_from_room( ch );
    char_to_room( ch, to_room );

    /* Qwert: jak nie moze, to ma wracac! */
    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	char_from_room( ch );
	char_to_room( ch, in_room );
	send_to_char( "Kogo chcesz zaskoczy`c swoim atakiem?\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	char_from_room( ch );
	char_to_room( ch, in_room );
	send_to_char( "Chcesz podrapa`c si`e po plecach?\n\r", ch );
	return;
    }

    if ( victim->fighting )
    {
	char_from_room( ch );
	char_to_room( ch, in_room );
	act( ZM_ZAWSZE, "$N w`la`snie walczy wi`ec nie ma co si`e w to w`l`acza`c.", ch, NULL, victim, TO_CHAR );
	send_to_char( "Cichutko wracasz...\n\r", ch );
	return;
    }

    if ( IS_AWAKE( victim ) && victim->hit < victim->max_hit )
    {
	char_from_room( ch );
	char_to_room( ch, in_room );
	act( ZM_ZAWSZE, "$N jest rann$Y i podejrzliw$Y... nie mo`zesz $M zaskoczy`c.", ch, NULL, victim, TO_CHAR );
	send_to_char( "Cichutko wracasz...\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
    {
	char_from_room( ch );
	char_to_room( ch, in_room );
	send_to_char( "Cichutko wracasz...\n\r", ch );
	return;
    }

    do_look( ch, "" );

    WAIT_STATE( ch, skill_table[ gsn_strike ].beats );
    if ( !IS_AWAKE( victim )
	|| number_percent( ) <= ch->pcdata->learned[ gsn_strike ] )
    {
/*SbF*/	send_to_char( "Zachodzisz bezszelestnie sw`a ofiar`e od ty`lu i zadajesz cios...\n\r", ch );
/*SbF*/	send_to_char( "Nagle czujesz ch`l`od ostrza w swoich plecach...\n\r", victim );
	uzyj( ch, gsn_strike, "podst`epnym ataku" );
	/* one_hit, wiec check_killer zbedny: */
	one_hit( ch, victim, gsn_strike, WEAR_WIELD );
    }
    else
	damage( ch, victim, 0, gsn_strike, WEAR_WIELD, FALSE );

    return;
}


KOMENDA( do_stun )
{
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *room = ch->in_room;
    int szansa;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_stun ) || ch->pcdata->learned[ gsn_stun ] == 0 )
    {
	send_to_char( "Nie potrafisz og`lusza`c przeciwnik`ow.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD )
      || get_eq_char( ch, WEAR_WIELD_2 ) )
    {
	send_to_char( "Nie mo`zesz wykona`c techniki og`luszania u`zywaj`ac broni.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( !ch->fighting )
    {
	if ( arg[ 0 ] == '\0' )
	{
	    send_to_char( "Pr`obujesz uderzy`c niewidzialnego przeciwnika.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n dziwnie wymachuje r`ekami.", ch, NULL, NULL, TO_ROOM );
	}
	else
	    send_to_char( "Musisz walczy`c, aby og`luszy`c przeciwnika.\n\r", ch );

	return;
    }
    else if ( arg[ 0 ] == '\0' )
	victim = FIGHTING( ch );
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "Nie widzisz takiej postaci.\n\r", ch );
	return;
    }

    if ( !is_fighting( victim, ch ) )
    {
	act( ZM_ZAWSZE, "$N nie walczy z tob`a!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "To raczej bezcelowe...\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim, TRUE ) )
	return;

    WAIT_STATE( ch, skill_table[ gsn_stun ].beats );

    if ( !check_blind( ch ) )
    {
	if ( IS_NPC( ch )
	  || number_percent( ) > 2 * ch->pcdata->learned[ gsn_walka_na_oslep ] / 3 )
	{
	    return;
	}
	else
	    uzyj( ch, gsn_walka_na_oslep, "walce na o`slep" );
    }

    szansa = 0
	+ ch->pcdata->learned[ gsn_stun ] * 3 / 4
	+ 2 * ( ch->level - victim->level );

    if ( number_percent( ) > szansa )
    {
	damage( ch, victim, 0, gsn_stun, WEAR_NONE, FALSE );
	return;
    }

    send_to_char( "Skupiasz sw`a moc w d`loni i posy`lasz pot`e`zny cios w g`low`e przeciwnika.\n\r", ch );

    if ( victim->hit > victim->max_hit / 8 /* tyci dodalem, inaczej bez sensu */
      || ( number_percent( ) > ch->pcdata->learned[ gsn_stun ] ) )
    {
	uzyj( ch, gsn_stun, "og`luszaniu" );
	damage( ch, victim, calculate_damage( ch, victim, gsn_stun ), gsn_stun, WEAR_NONE, FALSE );
	return;
    }

    damage( ch, victim, calculate_damage( ch, victim, gsn_stun ), gsn_stun, WEAR_NONE, FALSE );
    if ( ch->deleted
      || victim->deleted
      || ch->in_room != room
      || victim->in_room != room )
    {
	return;
    }

    stop_fighting( victim, TRUE );
    /* stop_fighting( ch, FALSE ); Lam 28.3.2007 */
    victim->position = POS_STUNNED;

    act( ZM_WZROK | ZM_WID_VICT, "$n wyprowadza pot`e`zny cios w g`low`e $!.", ch, NULL, victim, TO_NOTVICT );
    act( ZM_ZAWSZE, "$N nagle traci r`ownowag`e i pada nieprzytomn$Y na ziemi`e.", ch, NULL, victim, TO_CHAR );
    STC( "Tracisz przytomno`s`c!\n\r", victim );
    uzyj( ch, gsn_stun, "og`luszaniu" );

    return;
}


KOMENDA( do_whirl )
{
    ROOM_INDEX_DATA *room = ch->in_room;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    bool       jest = FALSE;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_whirl ) || ch->pcdata->learned[ gsn_whirl ] == 0 )
    {
	send_to_char( "Nie wiesz jak zrobi`c m`lynek.\n\r", ch );
	return;
    }

    if ( !get_eq_char( ch, WEAR_WIELD )
      || !get_eq_char( ch, WEAR_WIELD_2 ) )
    {
	send_to_char( "`Zeby zrobi`c m`lynek musisz u`zy`c dw`och broni.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_whirl ].beats );

    send_to_char( "Chwytasz mocno za sw`a bro`n i zaczynasz wirowa`c...\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n chwyta mocno za sw`a bro`n i zaczyna wirowa`c...", ch, NULL, NULL, TO_ROOM );

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( vch->in_room != room )
	    break;

	/* Qwert: + is_same_group */
	if ( vch == ch
	  || vch->deleted
	  || is_same_group( vch, ch )
	  || is_safe( ch, vch, FALSE ) )
	    continue;

	if ( number_percent( ) <= ch->pcdata->learned[ gsn_whirl ] * 3 / 4
					    + 2 * ( ch->level - vch->level ) )
	{
	    if ( !vch->fighting && critical_hit( ch, vch ) )
	    {
		act( ZM_ZAWSZE, "Tw`oj silny cios broni`a u`smierci`l $#!", ch, NULL, vch, TO_CHAR );
		act( ZM_WZROK | ZM_WID_VICT, "Silny cios broni`a $1 spowodowa`l nag`l`a `smier`c $!!", ch, NULL, vch, TO_NOTVICT );
		act( ZM_ZAWSZE, "Silny cios broni`a $1 powoduje twoj`a nag`l`a `smier`c!", ch, NULL, vch, TO_VICT );

		/* Lam: to powinno go na dobre usmiercic */
		vch->hit = -10;
		damage( ch, vch, 10, TYPE_UNDEFINED, WEAR_NONE, TRUE ); /* zawsze TRUE */
	    }
	    else
		one_hit( ch, vch, gsn_whirl, WEAR_WIELD );
	}
	else
	    damage( ch, vch, 0, gsn_whirl, WEAR_WIELD, FALSE );

	jest = TRUE;

	if ( ch->in_room != room )
	    break;
    }

    if ( ch->in_room != room )
	return;

    if ( jest )
    {
	uzyj( ch, gsn_whirl, "m`lynku" );
	return;
    }

    send_to_char( "...a`z zaczyna kr`eci`c ci si`e w g`lowie, wi`ec przestajesz.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "...po chwili przestaje z lekko zachwian`a r`ownowag`a.", ch, NULL, NULL, TO_ROOM );

    return;
}


KOMENDA( do_wraith_morph )
{
    AFFECT_DATA af;
    int szansa;

    if ( IS_NPC( ch ) )
	return;

    if ( !moze_uzyc( ch, gsn_wraith_morph )
      || !ch->pcdata->learned[ gsn_wraith_morph ] )
    {
	send_to_char( "Nie potrafisz tego wykona`c.\n\r", ch );
	return;
    }

    if ( ch->pcdata->learned[ gsn_initiation ] == 0 )
    {
	send_to_char( "Masz za niski stopie`n wtajemniczenia.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "Lepiej si`e zajmij walk`a.\n\r", ch );
	return;
    }

    if ( ch->hit * 5 < ch->max_hit )
    {
	send_to_char( "Masz zbyt ma`lo si`l by si`e przeobrazi`c w widmo.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[ gsn_wraith_morph ].beats );

    szansa = 0
	+ ch->pcdata->learned[ gsn_wraith_morph ] * 4 / 5
	+ ch->pcdata->learned[ gsn_initiation ] / 5 ;

    if ( number_percent( ) > szansa )
    {
	send_to_char( "Starasz si`e przeobrazi`c w widmo, lecz nie udaje ci si`e.\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zaczyna chwilowo migota`c, po czym wraca do normy.", ch, NULL, NULL, TO_ROOM );
	ch->hit -= ch->hit / 6;
	return;
    }

    if ( IS_PASSING( ch ) )
    {
	send_to_char( "Przecie`z ju`z mo`zesz przenika`c przez drzwi!\n\r", ch );
	return;
    }

    af.type      = gsn_wraith_morph;
    af.duration  = number_fuzzy( 3 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.level     = ch->level;
    af.caster    = ch;
    CLEAR_BITS( af.bitvector );
    SET_BIT_V( af.bitvector, AFF_PASS_DOOR );
    affect_join( ch, &af, ADD_AFFECT );

    act( ZM_ZAWSZE, "Skupiasz si`e, po czym stajesz si`e ca`lkowicie przezroczyst$y.", ch, NULL, NULL, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "$n zaczyna migota`c, po czym staje si`e ca`lkowicie przezroczyst$y.", ch, NULL, NULL, TO_ROOM );

    ch->hit -= number_range( ch->hit / 6, ch->hit / 2 );

    uzyj( ch, gsn_wraith_morph, "przeobra`zaniu si`e w widmo" );
    uzyj( ch, gsn_initiation, "wy`zszym stopniu wtajemniczenia" );

    return;
}
