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
 *  Losowe gry pieniezne, czyli hazard.                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "merc.h"


struct game_type game_table[ ] =
{
    { gra_czarna_czerwona,	"gra_czarna_czerwona",	0, 0	},
    { gra_kolo_fortuny,		"gra_ko`lo_fortuny",	0, 0	},
    { gra_marynarz,		"gra_marynarz",		0, 0	},
    { gra_nwt,			"gra_nwt",		0, 0	},
    { gra_pewna_reka,		"gra_pewna_r`eka",	0, 0	},
    { gra_pkn,			"gra_pkn",		0, 0	},
    { gra_siodemka,		"gra_si`odemka",	0, 0	},
    { gra_szczurek,		"gra_szczurek",		0, 0	},
    { gra_trzy_kubki,		"gra_trzy_kubki",	0, 0	},
    { gra_wojna_kart,		"gra_wojna_kart",	0, 0	},
    { gra_wojna_kosci,		"gra_wojna_ko`sci",	0, 0	},
    { gra_zgadnij_karte,	"gra_zgadnij_kart`e",	0, 0	},
    { NULL,			NULL,			0, 0	}
};


GAME_FUN *game_lookup( const char *name )
{
    int i;

    for ( i = 0; game_table[ i ].fun; i++ )
	if ( !str_cmp( name, game_table[ i ].name ) )
	    break;

    return game_table[ i ].fun;
}


char *game_name( GAME_FUN *fun )
{
    int i;

    if ( fun == NULL )
	return "NULL";

    for ( i = 0; game_table[ i ].fun; i++ )
	if ( fun == game_table[ i ].fun )
	    return game_table[ i ].name;

    return "nieznana!";
}


/*
 * Przepisane przez Lama.
 */
KOMENDA( do_postaw )
{
    CHAR_DATA *krupier;
    static int bilans;
    static int grano;
    int przed;
    int i;

    if ( !IS_NPC( ch ) && ch->level >= L_SEN && !str_cmp( argument, "bilans" ) )
    {
	STC( "                 gra:       bilans gier\n\r", ch );
	for ( i = 0; game_table[ i ].fun; i++ )
	    ch_printf( ch, "%s: %12d %d\n\r", wyrownaj( game_table[ i ].name, 20 ),
			game_table[ i ].bilans, game_table[ i ].grano );
	ch_printf( ch, "Ca`lkowity bilans: %d, gier: %d.\n\r", bilans, grano );
	return;
    }

    if ( IS_MUTE( ch )
      || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "Usi`lujesz co`s z siebie wydusi`c, nie wydobywa si`e jednak `zaden d`xwi`ek.\n\r", ch );
	return;
    }

    for ( krupier = ch->in_room->people;
	krupier;
	krupier = krupier->next_in_room )
    {
	if ( can_see( ch, krupier )
	  && IS_NPC( krupier )
	  && IS_AWAKE( krupier )
	  && !IS_AFFECTED( krupier, AFF_CHARM )
	  && krupier->pIndexData->game_fun
	  && !IS_MUTE( krupier )
	  && !IS_SET( krupier->in_room->room_flags, ROOM_CONE_OF_SILENCE )
	  && krupier != ch )
	    break;
    }

    if ( !krupier )
    {
	send_to_char( "Tu nie mo`zesz uprawia`c hazardu.\n\r", ch );
	return;
    }

    if ( !can_see( krupier, ch ) )
    {
	act( ZM_ZAWSZE, "$n nie mo`ze ci`e zobaczy`c.", krupier, NULL, ch, TO_VICT );
	return;
    }

    przed = ch->gold;

    ( *krupier->pIndexData->game_fun )( ch, krupier, argument );

    bilans += przed - ch->gold;
    grano++;
    for ( i = 0; game_table[ i ].fun; i++ )
	if ( game_table[ i ].fun == krupier->pIndexData->game_fun )
	    break;
    if ( game_table[ i ].fun )
    {
	game_table[ i ].bilans += przed - ch->gold;
	game_table[ i ].grano++;
    }
    else
	bug( "nie znalazlem gry", 0 );

    return;
}


GRA( gra_czarna_czerwona )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    int  limit = 100000;
    int  amount;
    int  die;
    int  i;
    static char *ktora[ 3 ] = { "lewa", "`srodkowa", "prawa" };

    argument = one_argument( argument, buf );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    if ( !*argument )
    {
	STC( "Musisz zgadn`a`c, czy czerwona jest karta: lewa, `srodkowa czy prawa (1/2/3).\n\r", ch );
	return;
    }

    for ( i = 0; i < 3; i++ )
	if ( !str_prefix( argument, ktora[ i ] )
	  || ( is_number( argument ) && atoi( argument ) == i + 1 ) )
	{
	    break;
	}

    if ( i == 3 )
    {
	STC( "Musisz zgadn`a`c, czy czerwona jest karta: lewa, `srodkowa czy prawa (1/2/3).\n\r", ch );
	return;
    }

    sprintf( buf, "$N przestaje rusza`c kartami. K`ladziesz na stole %s i m`owisz \"$t\".", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, ktora[ i ], croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$N przestaje rusza`c kartami, a $n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, ktora[ i ], croupier, TO_NOTVICT );
    sprintf( buf, "$n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR | ZM_W_WID_VICT, buf, ch, ktora[ i ], croupier, TO_NOTVICT );
    sprintf( buf, "$N przestaje rusza`c kartami, a $n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR | ZM_W_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die = number_range( 0, 2 );
    act( ZM_ZAWSZE, "$N odwraca karty, czerwona jest $t.", ch, ktora[ die ], croupier, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "$N odwraca karty, czerwona jest $t.", ch, ktora[ die ], croupier, TO_NOTVICT );

    if ( die == i )
    {
	amount = 18 * amount / 10;
	ch_printf( ch, "Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


GRA( gra_kolo_fortuny )
{
    OBJ_INDEX_DATA *obj_ind;
    OBJ_DATA *obj;
    char buf[ MAX_STRING_LENGTH ];
    int  limit = 100000;
    int  amount;

    one_argument( argument, buf );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    sprintf( buf, "K`ladziesz na stole %s i kr`ecisz ko`lem.", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, NULL, croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$n k`ladzie na stole %s i kr`eci ko`lem.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
    act( ZM_WZROK | ZM_W_WID_CHAR, "Ko`lo fortuny samo zaczyna si`e kr`eci`c.",
	ch, NULL, croupier, TO_NOTVICT );
    ch->gold -= amount;

    /* sezonowe nagrody, nie bawie sie we vnumy z vnums.txt */
    if ( !number_bits( 5 ) && ( obj_ind = get_obj_index( 4400 ) ) )
    {
	obj = create_object( obj_ind, 0 );
	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch )
	  || ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
	    send_to_char( "Na kole wypada nagroda rzeczowa, ale odmawiasz jej odebrania.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "Na kole wypada nagroda rzeczowa, ale $n odmawia jej odebrania.", ch, NULL, croupier, TO_NOTVICT );
	    extract_obj( obj );
	    return;
	}

	STC( "Na kole wypada nagroda rzeczowa. Wygrywasz bilet do cyrku!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "Na kole wypada nagroda rzeczowa, $n wygrywa bilet do cyrku!", ch, NULL, croupier, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Na kole wypada nagroda rzeczowa.", ch, NULL, croupier, TO_NOTVICT );
	obj_to_char( obj, ch );
	zmien_poziom( obj, ch );
	return;
    }

    if ( !number_bits( 5 ) && ( obj_ind = get_obj_index( 1013 ) ) )
    {
	obj = create_object( obj_ind, 0 );
	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch )
	  || ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
	    send_to_char( "Na kole wypada nagroda rzeczowa, ale odmawiasz jej odebrania.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "Na kole wypada nagroda rzeczowa, ale $n odmawia jej odebrania.", ch, NULL, croupier, TO_NOTVICT );
	    extract_obj( obj );
	    return;
	}

	STC( "Na kole wypada nagroda rzeczowa. Wygrywasz du`zy kawa`lek suchej ofcolskiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "Na kole wypada nagroda rzeczowa, $n wygrywa such`a ofcolsk`a!", ch, NULL, croupier, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Na kole wypada nagroda rzeczowa.", ch, NULL, croupier, TO_NOTVICT );
	obj_to_char( obj, ch );
	zmien_poziom( obj, ch );
	return;
    }

    /* pierscionki z roznego koloru oczkami 3225-3228 */
    if ( !number_bits( 5 ) && ( obj_ind = get_obj_index( number_range( 3225, 3228 ) ) ) )
    {
	obj = create_object( obj_ind, 0 );
	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch )
	  || ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
	    send_to_char( "Na kole wypada nagroda rzeczowa, ale odmawiasz jej odebrania.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "Na kole wypada nagroda rzeczowa, ale $n odmawia jej odebrania.", ch, NULL, croupier, TO_NOTVICT );
	    extract_obj( obj );
	    return;
	}

	STC( "Na kole wypada nagroda rzeczowa. Wygrywasz pier`scionek!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "Na kole wypada nagroda rzeczowa, $n wygrywa pier`scionek!", ch, NULL, croupier, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Na kole wypada nagroda rzeczowa.", ch, NULL, croupier, TO_NOTVICT );
	obj_to_char( obj, ch );
	zmien_poziom( obj, ch );
	return;
    }

    if ( !number_bits( 5 ) && ( obj_ind = get_obj_index( 3040 ) ) )
    {
	obj = create_object( obj_ind, 0 );
	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch )
	  || ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
	    send_to_char( "Na kole wypada nagroda rzeczowa, ale odmawiasz jej odebrania.\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "Na kole wypada nagroda rzeczowa, ale $n odmawia jej odebrania.", ch, NULL, croupier, TO_NOTVICT );
	    extract_obj( obj );
	    return;
	}

	STC( "Na kole wypada nagroda rzeczowa. Wygrywasz zw`oj identyfikacji!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "Na kole wypada nagroda rzeczowa, $n wygrywa zw`oj identyfikacji!", ch, NULL, croupier, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Na kole wypada nagroda rzeczowa.", ch, NULL, croupier, TO_NOTVICT );
	obj_to_char( obj, ch );
	zmien_poziom( obj, ch );
	return;
    }

    if ( !number_bits( 8 ) )
    {
	STC( "Ko`lo pokaza`lo wycieczk`e!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "Ko`lo zatrzymuje si`e, a $n wygrywa wycieczk`e!", ch, NULL, croupier, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Ko`lo pokazuje wycieczk`e.", ch, NULL, croupier, TO_NOTVICT );
	( *skill_table[ gsn_teleport ].spell_fun )( gsn_teleport,
		SPELL_LEVEL( croupier, croupier->level ), croupier, ch, USAGE_GAME );
	return;
    }

    if ( !number_bits( 5 ) )
    {
	amount = 5 * amount;
	ch_printf( ch, "Ko`lo pokaza`lo \"x 5\"! Wygrywasz %s.\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( buf, "Ko`lo pokaza`lo \"x 5\", $n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Ko`lo pokaza`lo \"x 5\"!", ch, NULL, croupier, TO_NOTVICT );
	return;
    }

    if ( !number_bits( 6 ) )
    {
	amount = 10 * amount;
	ch_printf( ch, "Ko`lo pokaza`lo \"x 10\"! Wygrywasz %s.\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( buf, "Ko`lo pokaza`lo \"x 10\", $n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Ko`lo pokaza`lo \"x 10\"!", ch, NULL, croupier, TO_NOTVICT );
	return;
    }

    if ( !number_bits( 7 ) )
    {
	amount = 25 * amount;
	ch_printf( ch, "Ko`lo pokaza`lo \"x 25\"! Wygrywasz %s.\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( buf, "Ko`lo pokaza`lo \"x 25\", $n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Ko`lo pokaza`lo \"x 25\"!", ch, NULL, croupier, TO_NOTVICT );
	return;
    }

    if ( !number_bits( 8 ) )
    {
	amount = 100 * amount;
	ch_printf( ch, "Ko`lo pokaza`lo \"x 100\"! Wygrywasz %s.\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( buf, "Ko`lo pokaza`lo \"x 100\", $n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
	act( ZM_WZROK | ZM_W_WID_CHAR, "Ko`lo pokaza`lo \"x 100\"!", ch, NULL, croupier, TO_NOTVICT );
	return;
    }

    send_to_char( "Tym razem szcz`e`scie ci nie dopisa`lo - ko`lo pokaza`lo brak nagrody.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "Ko`lo pokaza`lo brak nagrody, $n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    act( ZM_WZROK | ZM_W_WID_CHAR, "Ko`lo pokaza`lo brak nagrody.", ch, NULL, croupier, TO_NOTVICT );

    return;
}


GRA( gra_marynarz )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    char choice[ MAX_INPUT_LENGTH ];
    int  limit = 100000;
    int  amount;
    int  gracza;
    int  krupiera;
    int  suma;

    argument = one_argument( argument, buf );
    one_argument( argument, choice );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    gracza = atoi( choice );
    if ( !*choice || !is_number( choice )
      || gracza < 1 || gracza > 10 )
    {
	STC( "Podaj, ile palc`ow poka`zesz (od 1 do 10).\n\r", ch );
	return;
    }

    sprintf( buf, "K`ladziesz na stole %s.", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, NULL, croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
    ch->gold -= amount;

    /* No dobra, tu lekko oszukamy ;) Gracz moze nigdy nie pokazywac 10, wiec
       krupier moze nie pokazywac 1 ;) */
    if ( gracza != 1 )
	krupiera = number_range( 1, 10 );
    else
	krupiera = number_range( 2, 10 );
    suma = gracza + krupiera;

    /* FIXME: widzenie tylko jednego albo drugiego */
    if ( krupiera == gracza )
    {
	sprintf( buf, "$N pokazuje %d, tak samo, jak ty. To razem %d.", krupiera, suma );
	act( ZM_ZAWSZE, buf, ch, NULL, croupier, TO_CHAR );
	sprintf( buf, "$N pokazuje %d, tak samo, jak $n. To razem %d.", krupiera, suma );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	sprintf( buf, "$N pokazuje %d, a ty %d. To razem %d.", krupiera, gracza, suma );
	act( ZM_ZAWSZE, buf, ch, NULL, croupier, TO_CHAR );
	sprintf( buf, "$N pokazuje %d, a $n %d. To razem %d.", krupiera, gracza, suma );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    }

    if ( suma == 2 )
	act( ZM_ZAWSZE, "$N odlicza: sufit - ty. Wygrywasz!", ch, NULL, croupier, TO_CHAR );
    else if ( suma == 3 )
	act( ZM_ZAWSZE, "$N odlicza: sufit - ty - $Z. Przegrywasz!", ch, NULL, croupier, TO_CHAR );
    else if ( suma % 2 == 0 )
	act( ZM_ZAWSZE, "$N odlicza, zaczynaj`ac od sufitu. Wyliczanka ko`nczy si`e na tobie. Wygrywasz!", ch, NULL, croupier, TO_CHAR );
    else
	act( ZM_ZAWSZE, "$N odlicza, zaczynaj`ac od sufitu. Wyliczanka ko`nczy si`e na $%. Przegrywasz!", ch, NULL, croupier, TO_CHAR );

    if ( suma % 2 == 0 )
    {
	amount = 18 * amount / 10;
	ch_printf( ch, "Wygrywasz %s.\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


/*
 * Upper-Lower-Triple
 * Game idea by Partan
 * Coded by Thelonius
 *
 * Mocno przerobione, zeby sie do czegokolwiek nadawalo. Sam pomysl gry
 * bez zmian.
 */
GRA( gra_nwt )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    char choice[ MAX_INPUT_LENGTH ];
    int  limit = 100000;
    int  ichoice;
    char *schoice;
    int  amount;
    int  die1;
    int  die2;
    int  die3;
    int  total;

    argument = one_argument( argument, buf );
    one_argument( argument, choice );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }
/*
 *  At the moment, the winnings (and losses) do not actually go through
 *  the croupier.  They could do so, if each croupier is loaded with a
 *  certain bankroll.  Unfortunately, they would probably be popular
 *  (and rich) targets.
 */

    if ( !*choice )
    {
	STC( "Wybierz, na co stawiasz. Mo`zliwo`sci: nisko, wysoko, tr`ojka.\n\r", ch );
	return;
    }

    if ( !str_prefix( choice, "nisko" ) )
    {
	ichoice = 1;
	schoice = "nisko";
    }
    else if ( !str_prefix( choice, "wysoko" ) )
    {
	ichoice = 2;
	schoice = "wysoko";
    }
    else if ( !str_prefix( choice, "tr`ojka" ) )
    {
	ichoice = 3;
	schoice = "tr`ojka";
    }
    else
    {
	STC( "Wybierz, na co stawiasz. Mo`zliwo`sci: nisko, wysoko, tr`ojka.\n\r", ch );
	return;
    }
/*
 *  Now we have a wagering amount, and a choice.
 *  Let's place the bets and roll the dice, shall we?
 */
    sprintf( buf, "K`ladziesz na stole %s i m`owisz \"$t\".", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, schoice, croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR, buf, ch, schoice, croupier, TO_NOTVICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR, buf, ch, schoice, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die1 = number_range( 1, 6 );
    die2 = number_range( 1, 6 );
    die3 = number_range( 1, 6 );
    total = die1 + die2 + die3;

    sprintf( msg, "$n rzuca ko`s`cmi: wypada %d, %d i %d", die1, die2, die3 );

    if ( die1 == die2 && die2 == die3 )
    {
	strcat( msg, ". To tr`ojka!" );
	act( ZM_ZAWSZE, msg, croupier, NULL, ch, TO_VICT );
	act( ZM_WZROK | ZM_WID_CHAR, msg, croupier, NULL, ch, TO_NOTVICT );

	if ( ichoice == 3 )
	{
	    amount *= 30;
	    ch_printf( ch, "To tr`ojka! Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	    ch->gold += amount;
	    sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	    act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
	}
	else
	{
	    send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
	}

	return;
    }

    sprintf( buf, ", daj`ac razem %d.", total );
    strcat( msg, buf );
    act( ZM_ZAWSZE, msg, croupier, NULL, ch, TO_VICT );
    act( ZM_WZROK | ZM_WID_CHAR, msg, croupier, NULL, ch, TO_NOTVICT );

    if ( ( ( total <= 10 ) && ( ichoice == 1 ) )
      || ( ( total >= 11 ) && ( ichoice == 2 ) ) )
    {
	amount = 18 * amount / 10;
	ch_printf( ch, "Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


GRA( gra_pewna_reka )
{
    char buf[ MAX_STRING_LENGTH ];
    char choice[ MAX_INPUT_LENGTH ];
    int  limit = 100000;
    int  ichoice;
    int  amount;
    int  die1;
    int  die2;
    int	 total;

    argument = one_argument( argument, buf );
    one_argument( argument, choice );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    ichoice = atoi( choice );
    if ( !*choice || !is_number( choice )
      || ichoice < 2 || ichoice > 12 )
    {
	STC( "Musisz okre`sli`c, ile oczek zamierzasz wyrzuci`c (od 2 do 12).\n\r", ch );
	return;
    }

    sprintf( buf, "K`ladziesz na stole %s i m`owisz \"$t\".", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, choice, croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR, buf, ch, choice, croupier, TO_NOTVICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR, buf, ch, choice, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die1 = number_range( 1, 6 );
    die2 = number_range( 1, 6 );
    total = die1 + die2;

    ch_printf( ch, "Rzucasz ko`s`cmi: wypada %d i %d, razem %d.\n\r", die1, die2, total );
    sprintf( buf, "$n rzuca ko`s`cmi: wypada %d i %d, razem %d.", die1, die2, total );
    act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );

    if ( total == ichoice )
    {
	/* Szansa, ze wyjdzie 7 jest jak 1:6, inne wyniki sa mniej
	   prawdopodobne, ale to problem graczy, jesli nie rozumieja ;) */
	amount *= 5;
	ch_printf( ch, "Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( buf, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


GRA( gra_pkn )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    char choice[ MAX_INPUT_LENGTH ];
    int  limit = 100000;
    int  amount;
    int  die1;
    int  die2;
    static char *nazwy[ 3 ] = { "papier", "kamie`n", "no`zyce" };
    static int zabija[ 3 ] = { 1, 2, 0 };
    static char *komzab[ 3 ] = { "Papier owija kamie`n.",
	"Kamie`n t`epi no`zyce.", "No`zyce tn`a papier." };

    argument = one_argument( argument, buf );
    one_argument( argument, choice );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    if ( !*choice )
    {
	STC( "Wybierz, co poka`zesz r`ek`a. Mo`zliwo`sci: papier, kamie`n, no`zyce.\n\r", ch );
	return;
    }

    for ( die1 = 0; die1 < 3; die1++ )
	if ( !str_prefix( choice, nazwy[ die1 ] ) )
	    break;

    if ( die1 > 2 )
    {
	STC( "Wybierz, co poka`zesz r`ek`a. Mo`zliwo`sci: papier, kamie`n, no`zyce.\n\r", ch );
	return;
    }

    sprintf( buf, "K`ladziesz na stole %s.", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, NULL, croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die2 = number_range( 0, 2 );

    /* FIXME: tu tez nie obslugiwana sytuacja, kiedy widze tylko jednego */
    if ( die1 == die2 )
    {
	sprintf( buf, "$N pokazuje %s, tak samo, jak ty.", nazwy[ die2 ] );
	act( ZM_ZAWSZE, buf, ch, NULL, croupier, TO_CHAR );
	sprintf( buf, "$N pokazuje %s, tak samo, jak $n.", nazwy[ die2 ] );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	sprintf( buf, "$N pokazuje %s, a ty %s.", nazwy[ die2 ], nazwy[ die1 ] );
	act( ZM_ZAWSZE, buf, ch, NULL, croupier, TO_CHAR );
	sprintf( buf, "$N pokazuje %s, a $n %s.", nazwy[ die2 ], nazwy[ die1 ] );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    }

    if ( zabija[ die1 ] == die2 )
    {
	amount = 5 * amount / 2;
	ch_printf( ch, "%s Wygrywasz %s!\n\r", komzab[ die1 ], ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "%s $n wygra`l$a %s!", komzab[ die1 ], ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


/*
 * Siodemka, prostsza wersja NWT, pomysl ze strony z latkami do Envy.
 */
GRA( gra_siodemka )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    char choice[ MAX_INPUT_LENGTH ];
    int  limit = 100000;
    int  ichoice;
    char *schoice;
    int  amount;
    int  die1;
    int  die2;
    int  total;

    argument = one_argument( argument, buf );
    one_argument( argument, choice );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    if ( !*choice )
    {
	STC( "Wybierz, na co stawiasz. Mo`zliwo`sci: mniej, wi`ecej, siedem.\n\r", ch );
	return;
    }

    if ( !str_prefix( choice, "mniej" ) )
    {
	ichoice = 1;
	schoice = "mniej";
    }
    else if ( !str_prefix( choice, "wi`ecej" ) )
    {
	ichoice = 2;
	schoice = "wi`ecej";
    }
    else if ( !str_prefix( choice, "siedem" )
	   || !str_prefix( choice, "si`odemka" )
	   || !str_cmp( choice, "7" ) )
    {
	ichoice = 3;
	schoice = "si`odemka";
    }
    else
    {
	STC( "Wybierz, na co stawiasz. Mo`zliwo`sci: mniej, wi`ecej, siedem.\n\r", ch );
	return;
    }

    sprintf( buf, "K`ladziesz na stole %s i m`owisz \"$t\".", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, schoice, croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR, buf, ch, schoice, croupier, TO_NOTVICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR, buf, ch, schoice, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die1 = number_range( 1, 6 );
    die2 = number_range( 1, 6 );
    total = die1 + die2;

    sprintf( msg, "$n rzuca ko`s`cmi: wypada %d i %d", die1, die2 );

    if ( total == 7 )
    {
	strcat( msg, ". To si`odemka!" );
	act( ZM_ZAWSZE, msg, croupier, NULL, ch, TO_VICT );
	act( ZM_WZROK | ZM_WID_CHAR, msg, croupier, NULL, ch, TO_NOTVICT );

	if ( ichoice == 3 )
	{
	    amount *= 5;
	    ch_printf( ch, "To si`odemka! Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	    ch->gold += amount;
	    sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	    act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
	}
	else
	{
	    send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
	}

	return;
    }

    sprintf( buf, ", daj`ac razem %d.", total );
    strcat( msg, buf );
    act( ZM_ZAWSZE, msg, croupier, NULL, ch, TO_VICT );
    act( ZM_WZROK | ZM_WID_CHAR, msg, croupier, NULL, ch, TO_NOTVICT );

    if ( ( ( total <= 6 ) && ( ichoice == 1 ) )
      || ( ( total >= 8 ) && ( ichoice == 2 ) ) )
    {
	amount = 18 * amount / 10;
	ch_printf( ch, "Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


GRA( gra_szczurek )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    int  limit = 100000;
    int  amount;
    int  die;
    int  i;
    static char *ktora[ 4 ] = { "ser", "marchewka", "kukurydza", "sa`lata" };
    static char *ktorad[ 4 ] = { "sera", "marchewki", "kukurydzy", "sa`laty" };

    argument = one_argument( argument, buf );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    if ( !*argument )
    {
	STC( "Wytypuj, co wybierze szczur: ser/marchewka/kukurydza/sa`lata.\n\r", ch );
	return;
    }

    for ( i = 0; i < 4; i++ )
	if ( !str_prefix( argument, ktora[ i ] ) )
	    break;

    if ( i == 4 )
    {
	STC( "Wytypuj, co wybierze szczur: ser/marchewka/kukurydza/sa`lata.\n\r", ch );
	return;
    }

    sprintf( buf, "$N przynosi szczura zza kotary. K`ladziesz na stole %s i m`owisz \"$t\".", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, ktora[ i ], croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$N przynosi szczura zza kotary, a $n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, ktora[ i ], croupier, TO_NOTVICT );
    sprintf( buf, "$n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR | ZM_W_WID_VICT, buf, ch, ktora[ i ], croupier, TO_NOTVICT );
    sprintf( buf, "$N przynosi szczura zza kotary, a $n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR | ZM_W_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die = number_range( 0, 3 );
    act( ZM_ZAWSZE, "Szczur biegnie do $t.", ch, ktorad[ die ], croupier, TO_CHAR );
    act( ZM_WZROK, "Szczur biegnie do $t.", ch, ktorad[ die ], croupier, TO_NOTVICT );

    if ( die == i )
    {
	amount = 34 * amount / 10;
	ch_printf( ch, "Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


/*
 * Kopia czarnej-czerwonej :)
 */
GRA( gra_trzy_kubki )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    int  limit = 100000;
    int  amount;
    int  die;
    int  i;
    static char *ktora[ 3 ] = { "lewy", "`srodkowy", "prawy" };

    argument = one_argument( argument, buf );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    if ( !*argument )
    {
	STC( "Musisz zgadn`a`c, pod kt`orym kubkiem jest kulka: lewy/`srodkowy/prawy (1/2/3).\n\r", ch );
	return;
    }

    for ( i = 0; i < 3; i++ )
	if ( !str_prefix( argument, ktora[ i ] )
	  || ( is_number( argument ) && atoi( argument ) == i + 1 ) )
	{
	    break;
	}

    if ( i == 3 )
    {
	STC( "Musisz zgadn`a`c, pod kt`orym kubkiem jest kulka: lewy/`srodkowy/prawy (1/2/3).\n\r", ch );
	return;
    }

    sprintf( buf, "$N przestaje kr`eci`c kubkami. K`ladziesz na stole %s i m`owisz \"$t\".", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, ktora[ i ], croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$N przestaje kr`eci`c kubkami, a $n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, ktora[ i ], croupier, TO_NOTVICT );
    sprintf( buf, "$n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR | ZM_W_WID_VICT, buf, ch, ktora[ i ], croupier, TO_NOTVICT );
    sprintf( buf, "$N przestaje kr`eci`c kubkami, a $n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR | ZM_W_WID_VICT, buf, ch, NULL, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die = number_range( 0, 2 );
    act( ZM_ZAWSZE, "$N podnosi kubki, kulk`e skrywa`l $t.", ch, ktora[ die ], croupier, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "$N podnosi kubki, kulk`e skrywa`l $t.", ch, ktora[ die ], croupier, TO_NOTVICT );

    if ( die == i )
    {
	amount = 18 * amount / 10;
	ch_printf( ch, "Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


GRA( gra_wojna_kart )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    int  limit = 100000;
    int  amount;
    int  die1;
    int  die2;
    static char *karty[ 6 ] = { "dziewi`atk`e", "dziesi`atk`e", "waleta", "dam`e", "kr`ola", "asa" };

    one_argument( argument, buf );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    sprintf( buf, "K`ladziesz na stole %s.", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, NULL, croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die1 = die2 = 0;
    while ( die1 == die2 )
    {
	die1 = number_range( 0, 5 );
	/* Za drugim razem powinna byc mniejsza szansa na to samo, bo w talii
	   powinno takich zostac 3, a innych po 4, ale niewazne, bedzie wieksza
	   szansa na wojne! ;) */
	die2 = number_range( 0, 5 );
	sprintf( msg, "$n tasuje tali`e i wyci`aga: dla siebie %s, dla ciebie %s.",
		karty[ die1 ], karty[ die2 ] );
	if ( die1 == die2 )
	    strcat( msg, " To wojna!" );
	act( ZM_ZAWSZE, msg, croupier, NULL, ch, TO_VICT );
	sprintf( msg, "$n tasuje tali`e i wyci`aga: dla siebie %s, dla $! %s.",
		karty[ die1 ], karty[ die2 ] );
	if ( die1 == die2 )
	    strcat( msg, " To wojna!" );
	act( ZM_WZROK | ZM_WID_CHAR, msg, croupier, NULL, ch, TO_NOTVICT );
    }

    if ( die1 < die2 )
    {
	amount = 18 * amount / 10;
	ch_printf( ch, "Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


GRA( gra_wojna_kosci )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    int  limit = 100000;
    int  amount;
    int  die11, die12;
    int  die21, die22;

    one_argument( argument, buf );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    sprintf( buf, "K`ladziesz na stole %s.", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, NULL, croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    sprintf( buf, "$n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die11 = die12 = die21 = die22 = 0;
    while ( die11 + die12 == die21 + die22 )
    {
	die11 = number_range( 1, 6 );
	die12 = number_range( 1, 6 );
	sprintf( msg, "Rzucasz ko`s`cmi, wypada %d i %d, razem %d.",
		die11, die12, die11 + die12 );
	act( ZM_ZAWSZE, msg, croupier, NULL, ch, TO_VICT );
	sprintf( msg, "$N rzuca ko`s`cmi, wypada %d i %d, razem %d.",
		die11, die12, die11 + die12 );
	act( ZM_WZROK | ZM_WID_VICT, msg, croupier, NULL, ch, TO_NOTVICT );
	die21 = number_range( 1, 6 );
	die22 = number_range( 1, 6 );
	sprintf( msg, "$n rzuca ko`s`cmi, wypada %d i %d, razem %d.",
		die21, die22, die21 + die22 );
	if ( die11 + die12 == die21 + die22 )
	    strcat( msg, " To wojna!" );
	act( ZM_ZAWSZE, msg, croupier, NULL, ch, TO_VICT );
	sprintf( msg, "$n rzuca ko`s`cmi, wypada %d i %d, razem %d.",
		die21, die22, die21 + die22 );
	if ( die11 + die12 == die21 + die22 )
	    strcat( msg, " To wojna!" );
	act( ZM_WZROK | ZM_WID_CHAR, msg, croupier, NULL, ch, TO_NOTVICT );
    }

    if ( die11 + die12 > die21 + die22 )
    {
	amount = 18 * amount / 10;
	ch_printf( ch, "Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


GRA( gra_zgadnij_karte )
{
    char msg[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    int  limit = 100000;
    int  amount;
    int  die;
    int  i;
    static char *kartym[ 6 ] = { "dziewi`atka", "dziesi`atka", "walet", "dama", "kr`ol", "as" };
    static char *kartyn[ 6 ] = { "9", "10", "", "", "", "" };

    argument = one_argument( argument, buf );

    if ( buf[ 0 ] == '\0' || !is_number( buf ) )
    {
	send_to_char( "Ile z`lota chcesz postawi`c?\n\r", ch );
	return;
    }

    amount = atoi( buf );

    if ( amount > ch->gold )
    {
	send_to_char( "Nie wystarczy ci z`lota.\n\r", ch );
	return;
    }

    if ( amount < 100 )
    {
	send_to_char( "Musisz postawi`c przynajmniej sto monet.\n\r", ch );
	return;
    }

    if ( amount > limit )
    {
	sprintf( buf, "Maksymalna stawka to %s.", ZLOTO_M( limit ) );
	act( ZM_ZAWSZE, buf, croupier, NULL, ch, TO_VICT );
	return;
    }

    if ( !*argument )
    {
	STC( "Musisz wytypowa`c kart`e od dziewi`atki do asa.\n\r", ch );
	return;
    }

    for ( i = 0; i < 6; i++ )
	if ( !str_prefix( argument, kartym[ i ] )
	  || !str_cmp( argument, kartyn[ i ] ) )
	{
	    break;
	}

    if ( i == 6 )
    {
	STC( "Musisz wytypowa`c jedn`a kart`e od dziewi`atki do asa.\n\r", ch );
	return;
    }

    sprintf( buf, "$N k`ladzie kart`e koszulk`a do g`ory. K`ladziesz na stole %s i m`owisz \"$t\".", ZLOTO_B( amount ) );
    act( ZM_ZAWSZE, buf, ch, kartym[ i ], croupier, TO_CHAR );
    act( ZM_ZAWSZE, "$n zak`lada si`e z tob`a.", ch, NULL, croupier, TO_VICT );
    /* karta widzialna nawet, jesli krupiera nie widac */
    sprintf( buf, "$N k`ladzie kart`e koszulk`a do g`ory, a $n k`ladzie na stole %s i m`owi \"$t\".", ZLOTO_B( amount ) );
    act( ZM_SLUCH | ZM_WID_CHAR, buf, ch, kartym[ i ], croupier, TO_NOTVICT );
    sprintf( buf, "$N k`ladzie kart`e koszulk`a do g`ory, a $n k`ladzie na stole %s.", ZLOTO_B( amount ) );
    act( ZM_WZROK | ZM_W_SLUCH | ZM_WID_CHAR, buf, ch, NULL, croupier, TO_NOTVICT );
    ch->gold -= amount;

    die = number_range( 0, 5 );
    act( ZM_ZAWSZE, "$N odwraca kart`e - to $t.", ch, kartym[ die ], croupier, TO_CHAR );
    act( ZM_WZROK | ZM_WID_VICT, "$N odwraca kart`e - to $t.", ch, kartym[ die ], croupier, TO_NOTVICT );

    if ( die == i )
    {
	amount = 5 * amount;
	ch_printf( ch, "Wygrywasz %s!\n\r", ZLOTO_B( amount ) );
	ch->gold += amount;
	sprintf( msg, "$n wygra`l$a %s!", ZLOTO_B( amount ) );
	act( ZM_WZROK | ZM_WID_CHAR, msg, ch, NULL, croupier, TO_NOTVICT );
    }
    else
    {
	send_to_char( "C`o`z, nast`epnym razem na pewno p`ojdzie ci lepiej!\n\r", ch );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przegra`l$a.", ch, NULL, croupier, TO_NOTVICT );
    }

    return;
}


/*
 * Szansa na trafienie    3       4        5
 * 3 okienka, 12      1:144
 * 4 okienka, 12       1:39  1:1728
 * 5 okienek, 12      (1:17)  1:377  1:20736
 * 5 okienek, 6        (1:5)   1:51   1:1296
 * (gdzie szansa na trojke nie zawiera czworek ani piatek itd.)
 * W nawiasach nie nagradzane.
 * Niech mnie ktos poprawi, bo sam juz nie wiedzialem, co licze ;)
 */
KOMENDA( do_graj )
{
    static int bilans;
    static int grano;
    static int ilewzorkow[ 2 ] = { 12, 6 };
    static char *wzorki[ 2 ][ 12 ] =
    {
	{
	    "{Yz`lota moneta{x",
	    "{Wkula `swiat`la{x",
	    "{wgnomi pier`og{x",
	    "{csztylet{x",
	    "{ghe`lm{x",
	    "{rr`o`zd`zka{x",
	    "{Cpergamin{x",
	    "{ybuk`lak{x",
	    "{Gzielony smok{x",
	    "{Rczerwony smok{x",
	    "{Btarcza{x",
	    "{Mchleb{x"
	},
	{
	    "{Gsa`lata{x",
	    "{Rpomidor{x",
	    "{gog`orek{x",
	    "{ycebula{x",
	    "{rpapryka{x",
	    "{Ykukurydza{x"
	}
    };
    int iletraf[ 12 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int najwtraf = 0;
    OBJ_DATA *bandyta;
    char buf[ MSL ];
    char buf2[ MSL ];
    int i;
    int stop[ 5 ];

    if ( !IS_NPC( ch ) && ch->level >= L_SEN && !str_cmp( argument, "bilans" ) )
    {
	ch_printf( ch, "Bilans automat`ow: %d, gier: %d.\n\r", bilans, grano );
	return;
    }

    for ( bandyta = ch->in_room->contents; bandyta; bandyta = bandyta->next_content )
	if ( !bandyta->deleted
	  && bandyta->item_type == ITEM_BANDYTA
	  && can_see_obj( ch, bandyta ) )
	{
	    break;
	}

    if ( !bandyta )
    {
	STC( "Nie widzisz tu `zadnego automatu do gry.\n\r", ch );
	return;
    }

    if ( ch->gold < 100 )
    {
	STC( "Gra na automacie kosztuje 100 z`lotych monet.\n\r", ch );
	return;
    }

    if ( bandyta->value[ 1 ].v < 3 || bandyta->value[ 1 ].v > 5 )
    {
	STC( "Ten automat jest uszkodzony.\n\r", ch );
	return;
    }

    if ( bandyta->value[ 2 ].v < 0 || bandyta->value[ 2 ].v > 1 )
    {
	STC( "Ten automat jest uszkodzony.\n\r", ch );
	return;
    }

    grano++;
    ch->gold -= 100;
    bandyta->value[ 0 ].v += 100;
    bilans += 100;

    act( ZM_WZROK | ZM_WID_CHAR, "$n gra na automacie.", ch, NULL, NULL, TO_ROOM );
    STC( "Wrzucasz 100 monet i uruchamiasz automat.\n\rPrzez chwil`e co`s w nim furkocze, nagle zatrzymuje si`e, a okienka pokazuj`a:\n\r", ch );
    buf[ 0 ] = '\0';
    for ( i = 0; i < bandyta->value[ 1 ].v; i++ )
    {
	stop[ i ] = number_range( 0, ilewzorkow[ bandyta->value[ 2 ].v ] - 1 );
	iletraf[ stop[ i ] ]++;
	if ( iletraf[ najwtraf ] < iletraf[ stop[ i ] ] )
	    najwtraf = stop[ i ];
	sprintf( buf2, "%s", wyrownaj( wzorki[ bandyta->value[ 2 ].v ][ stop[ i ] ], -14 ) );
	strcat( buf, buf2 );
    }
    strcat( buf, "\n\r" );

    STC( buf, ch );

    if ( iletraf[ najwtraf ] == bandyta->value[ 1 ].v )
    {
	ASTC( "Rozbi`l$o`s bank!", ch );
	act( ZM_WZROK | ZM_SLUCH, "$n rozbi`l$a bank!", ch, NULL, NULL, TO_ROOM );
	/* troche oszukuje ;) */
	i = UMAX( 2000, bandyta->value[ 0 ].v - 20000 );
	sprintf( buf, "Automat wyrzuca %s.", ZLOTO_B( i ) );
	ASTC( buf, ch );
	act( ZM_WZROK, buf, ch, NULL, NULL, TO_ROOM );
	bilans -= i;
	ch->gold += i;
	bandyta->value[ 0 ].v = UMAX( 2000, bandyta->value[ 0 ].v - i );
    }
    else if ( bandyta->value[ 1 ].v > 3
	   && iletraf[ najwtraf ] == bandyta->value[ 1 ].v - 1 )
    {
	ASTC( "Prawie rozbi`l$o`s bank!", ch );
	act( ZM_WZROK | ZM_SLUCH, "$n prawie rozbi`l$a bank!", ch, NULL, NULL, TO_ROOM );
	/* 2% to bedzie malo po rozbiciu banku, ale przed bedzie calkiem
	   pokazna sumka */
	i = UMAX( 10000, bandyta->value[ 0 ].v ) / 50;
	sprintf( buf, "Automat wyrzuca %s.", ZLOTO_B( i ) );
	ASTC( buf, ch );
	act( ZM_WZROK, buf, ch, NULL, NULL, TO_ROOM );
	bilans -= i;
	ch->gold += i;
	bandyta->value[ 0 ].v = UMAX( 2000, bandyta->value[ 0 ].v - i );
    }
    else
	STC( "Tym razem ci si`e nie uda`lo.\n\r", ch );

    return;
}
