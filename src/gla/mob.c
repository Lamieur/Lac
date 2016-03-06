/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   ___           _______     ________      |                             *
 *  |###|         |#######|   |########| (R) |            GLACA            *
 *  |###|        |###|^|###| |###|^^|###|    |                             *
 *  |###|        |###| |###| |###|   ^^^     |                             *
 *  |###|        |###|_|###| |###|           |            Vigud            *
 *  |###|        |#########| |###|           |             Lam             *
 *  |###|        |###|^|###| |###|           |                             *
 *  |###|        |###| |###| |###|   ___     |                             *
 *  |###|______  |###| |###| |###|__|###|    |                             *
 *  |##########| |###| |###|  |########|     |                             *
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

#include "glaca.h"
#include "../special.h"
#include "../handler.h"
#include "../gry.h"
#include "../const.h"


struct dmob_data dmob;

FUNKCJA_DLA_GTK( zatwierdzenie_zmian_moba );
FUNKCJA_DLA_GTK( zmiana_flagi_wplywu );
FUNKCJA_DLA_GTK( zmiana_flagi_zachowania );


void zrob_okno_Moba( void )
{
    GtkWidget *vbox, *tree, *text;
    int i, j;
    char nazwa[ MIL ];

    ZNAJDZ_KONTROLKE( dmob.Mob, "oknomob" );
    ZNAJDZ_KONTROLKE( dmob.entry_name, "mob_entry_name" );
    ZNAJDZ_KONTROLKE( dmob.entry_short, "mob_entry_short" );
    ZNAJDZ_KONTROLKE( dmob.entry_dopelniacz, "mob_entry_dopelniacz" );
    ZNAJDZ_KONTROLKE( dmob.entry_celownik, "mob_entry_celownik" );
    ZNAJDZ_KONTROLKE( dmob.entry_biernik, "mob_entry_biernik" );
    ZNAJDZ_KONTROLKE( dmob.entry_narzednik, "mob_entry_narzednik" );
    ZNAJDZ_KONTROLKE( dmob.entry_miejscownik, "mob_entry_miejscownik" );
    ZNAJDZ_KONTROLKE( dmob.entry_wolacz, "mob_entry_wolacz" );
    ZNAJDZ_KONTROLKE( dmob.entry_long, "mob_entry_long" );
    ZNAJDZ_KONTROLKE( text, "mob_text_opis" );

    /* tego nie potrafi GLADE, kaka */
    gtk_widget_modify_font( text, font_desc );

#if !defined( BEZ_GTKSPELL )
    gtkspell_new_attach( GTK_TEXT_VIEW( text ), NULL, NULL );
#endif

    ZNAJDZ_KONTROLKE2( dmob.buffer, "mob_buffer_opis", GTK_TEXT_BUFFER );

    ZNAJDZ_KONTROLKE( dmob.combo_plec, "mob_combo_plec" );
    ZNAJDZ_KONTROLKE( dmob.label_rasa, "mob_label_rasa" );

    ZNAJDZ_KONTROLKE( dmob.label_zachowania, "mob_label_zachowania" );
    ZNAJDZ_KONTROLKE( dmob.label_wplywy, "mob_label_wplywy" );

    ZNAJDZ_KONTROLKE( vbox, "mob_vbox_wplywy" );
    for ( j = i = 0; affect_bit_values[ i ].name; i++ )
	if ( affect_bit_values[ i ].available )
	{
	    dmob.checkaff[ j ] = gtk_check_button_new_with_label( _( affect_bit_values[ i ].name ) );
	    gtk_widget_set_tooltip_text( dmob.checkaff[ j ], _( affect_bit_values[ i ].tooltip ) );
	    gtk_box_pack_start( GTK_BOX( vbox ), dmob.checkaff[ j ], FALSE, FALSE, 0 );
	    g_signal_connect( G_OBJECT( dmob.checkaff[ j ] ), "toggled",
		G_CALLBACK( zmiana_flagi_wplywu ), NULL );
	    dmob.check_na_aff[ j ] = affect_bit_values[ i ].bitn;
	    j++;
	}
    gtk_widget_show_all( vbox );

    ZNAJDZ_KONTROLKE( vbox, "mob_vbox_zachowania" );
    /* pierwsza flaga to mob, nieklikalne i nieustawialne */
    dmob.checkact[ 0 ] = gtk_check_button_new( );
    dmob.check_na_act[ 0 ] = mob_act_flags[ 0 ].bitv;
    for ( i = j = 1; mob_act_flags[ i ].name; i++ )
	if ( mob_act_flags[ i ].available )
	{
	    dmob.checkact[ j ] = gtk_check_button_new_with_label( _( mob_act_flags[ i ].name ) );
	    gtk_widget_set_tooltip_text( dmob.checkact[ j ], _( mob_act_flags[ i ].tooltip ) );
	    gtk_box_pack_start( GTK_BOX( vbox ), dmob.checkact[ j ], FALSE, FALSE, 0 );
	    g_signal_connect( G_OBJECT( dmob.checkact[ j ] ), "toggled",
		G_CALLBACK( zmiana_flagi_zachowania ), NULL );
	    dmob.check_na_act[ j ] = mob_act_flags[ i ].bitv;
	    j++;
	}
    gtk_widget_show_all( vbox );

    ZNAJDZ_KONTROLKE( dmob.spinbutton_poziom, "mob_spin_poziom" );
    ZNAJDZ_KONTROLKE( dmob.spinbutton_moralnosc, "mob_spin_moralnosc" );
    ZNAJDZ_KONTROLKE( dmob.spinbutton_zamoznosc, "mob_spin_zamoznosc" );

    ZNAJDZ_KONTROLKE2( dmob.store_resety, "mob_store_resety", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE( tree, "mob_treeview_resety" );
    dmob.select_reset = gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );

    ZNAJDZ_KONTROLKE( dmob.check_sklep, "mob_check_sklep" );
    ZNAJDZ_KONTROLKE( dmob.check_czary, "mob_check_czary" );
    ZNAJDZ_KONTROLKE( dmob.button_progi, "mob_button_progi" );
    ZNAJDZ_KONTROLKE( dmob.button_poprz, "mob_button_poprz" );
    ZNAJDZ_KONTROLKE( dmob.button_nast, "mob_button_nast" );

    for ( i = 0; i < 5; i++ )
    {
	sprintf( nazwa, "mob_combo_sklepS%d", i );
	ZNAJDZ_KONTROLKE( dmob.combo_sklepS[ i ], nazwa );
	gtk_combo_box_set_model( GTK_COMBO_BOX( dmob.combo_sklepS[ i ] ),
				 GTK_TREE_MODEL( store_typyprzedmiotow ) );

	sprintf( nazwa, "mob_label_sklepH%d", i );
	ZNAJDZ_KONTROLKE2( dmob.label_sklepH[ i ], nazwa, GTK_LABEL );

	sprintf( nazwa, "mob_spin_sklepH%d", i );
	ZNAJDZ_KONTROLKE( dmob.spin_sklepH[ i ], nazwa );

	sprintf( nazwa, "mob_toggle_sklepH%d", i );
	ZNAJDZ_KONTROLKE( dmob.toggle_sklepH[ i ], nazwa );
    }

    ZNAJDZ_KONTROLKE( dmob.spin_czynneOd, "mob_spin_czynneOd" );
    ZNAJDZ_KONTROLKE( dmob.spin_czynneDo, "mob_spin_czynneDo" );
    ZNAJDZ_KONTROLKE( dmob.spin_marzaK, "mob_spin_marzaK" );
    ZNAJDZ_KONTROLKE( dmob.spin_marzaS, "mob_spin_marzaS" );

    ZNAJDZ_KONTROLKE( dmob.table_sklep, "mob_table_sklep" );
    ZNAJDZ_KONTROLKE( dmob.table_czary, "mob_table_czary" );

    ZNAJDZ_KONTROLKE( dmob.combo_spec, "mob_combo_spec" );
    for ( i = 0; spec_table[ i ].fun; i++ )
	gtk_combo_box_append_text( GTK_COMBO_BOX( dmob.combo_spec ), spec_table[ i ].name );

    ZNAJDZ_KONTROLKE( dmob.spin_wplywA, "mob_spin_wplywA" );
    ZNAJDZ_KONTROLKE( dmob.spin_grupa, "mob_spin_grupa" );

    ZNAJDZ_KONTROLKE( dmob.combo_krupier, "mob_combo_krupier" );
    for ( i = 0; game_table[ i ].fun; i++ )
	gtk_combo_box_append_text( GTK_COMBO_BOX( dmob.combo_krupier ), _( game_table[ i ].name ) );

    for ( i = 0; i < 5; i++ )
	dmob.pHealer_pusty.spells[ i ] = -1;

    return;
}


FUNKCJA_DLA_GTK( combo_rasa_mob )
{
    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filter_rasy, widget, NULL );
    else if ( dcom.wybrany )
    {
	gtk_tree_model_get( filter_rasy, &dcom.iter, KOL_ELEMENT, &dmob.rasa, -1 );
	gtk_label_set_text( GTK_LABEL( dmob.label_rasa ), _( race_table[ dmob.rasa ].who_name ) );
    }

    return;
}


FUNKCJA_DLA_GTK( zmiana_check_sklep )
{
    if ( widget == dmob.check_sklep )
	glaca_widget_set_visible( dmob.table_sklep, CHECK_GET_ACTIVE( dmob.check_sklep ) );
    else if ( widget == dmob.check_czary )
	glaca_widget_set_visible( dmob.table_czary, CHECK_GET_ACTIVE( dmob.check_czary ) );

    return;
}


FUNKCJA_DLA_GTK( poprznast_mob )
{
    GtkTreePath *path;
    GtkTreeModel *model;

    if ( !gtk_tree_selection_get_selected( dkra.selection_mob, &model, &dmob.iter ) )
	return;

    path = gtk_tree_model_get_path( model, &dmob.iter );

    zatwierdzenie_zmian_moba( NULL, GINT_TO_POINTER( 1 ) );

    if ( widget == dmob.button_poprz )
	gtk_tree_path_prev( path );
    else if ( widget == dmob.button_nast )
	gtk_tree_path_next( path );

    gtk_tree_selection_select_path( dkra.selection_mob, path );
    gtk_tree_path_free( path );
    mob_z_listy( NULL, NULL );

    return;
}


FUNKCJA_DLA_GTK( mob_z_listy )
{
    GtkTreeModel *model;

    if ( !gtk_tree_selection_get_selected( dkra.selection_mob, &model, &dmob.iter ) )
	return;
    gtk_tree_model_get( model, &dmob.iter, KOL_WSKAZNIK, &dmob.mob, -1 );
    dialog_moba( NULL, NULL );
}


FUNKCJA_DLA_GTK( dialog_moba )
{
    int i, iTrade, spnum;
    HEALER_DATA *pHealer_temp;
    SHOP_DATA *pShop_temp;
    AREA_DATA *a;
    RESET_DATA *reset;
    char buf[ MIL ];

    if ( !dmob.mob )
	return;

    gtk_grab_add( dmob.Mob );

    sprintf( buf, "#%d: %s (%s)",
	dmob.mob->vnum,
	dmob.mob->short_descr,
	dmob.mob->index_area->full_name );
    gtk_window_set_title( GTK_WINDOW( dmob.Mob ), _( buf ) );

    ZROB_LANCUCH( dmob.entry_name, dmob.mob->player_name );
    ZROB_LANCUCH( dmob.entry_short, dmob.mob->short_descr );
    ZROB_LANCUCH( dmob.entry_dopelniacz, dmob.mob->dopelniacz );
    ZROB_LANCUCH( dmob.entry_celownik, dmob.mob->celownik );
    ZROB_LANCUCH( dmob.entry_biernik, dmob.mob->biernik );
    ZROB_LANCUCH( dmob.entry_narzednik, dmob.mob->narzednik );
    ZROB_LANCUCH( dmob.entry_miejscownik, dmob.mob->miejscownik );
    ZROB_LANCUCH( dmob.entry_wolacz, dmob.mob->wolacz );
    ZROB_LANCUCH( dmob.entry_long, dmob.mob->long_descr );

    ZROB_LANCUCH_TEXT( dmob.buffer, dmob.mob->description );

    SPIN_SET_VALUE( dmob.spinbutton_poziom, dmob.mob->level );
    SPIN_SET_VALUE( dmob.spinbutton_moralnosc, dmob.mob->alignment );
    SPIN_SET_VALUE( dmob.spinbutton_zamoznosc, dmob.mob->zamoznosc );

    COMBO_SET_ACTIVE( dmob.combo_plec, dmob.mob->sex );
    dmob.rasa = dmob.mob->race;
    gtk_label_set_text( GTK_LABEL( dmob.label_rasa ), _( race_table[ dmob.rasa ].who_name ) );

    COMBO_SET_ACTIVE( dmob.combo_spec, 0 ); /* (brak) */
    if ( dmob.mob->spec_fun )
	for ( i = 0; spec_table[ i ].fun; i++ )
	    if ( dmob.mob->spec_fun == spec_table[ i ].fun )
	    {
		COMBO_SET_ACTIVE( dmob.combo_spec, i + 1 );
		break;
	    }

    COMBO_SET_ACTIVE( dmob.combo_krupier, 0 ); /* (brak) */
    if ( dmob.mob->game_fun )
	for ( i = 0; game_table[ i ].fun; i++ )
	    if ( dmob.mob->game_fun == game_table[ i ].fun )
	    {
		COMBO_SET_ACTIVE( dmob.combo_krupier, i + 1 );
		break;
	    }

    SPIN_SET_VALUE( dmob.spin_grupa, dmob.mob->grupa );
    SPIN_SET_VALUE( dmob.spin_wplywA, dmob.mob->mod_size );

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dmob.check_sklep ), dmob.mob->pShop != NULL );
    pShop_temp = dmob.mob->pShop ? dmob.mob->pShop : &dmob.pShop_pusty;

    for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	COMBO_SET_ACTIVE( dmob.combo_sklepS[ iTrade ],
	    typ_na_numer[ pShop_temp->buy_type[ iTrade ] ] );

    SPIN_SET_VALUE( dmob.spin_czynneOd, pShop_temp->open_hour );
    SPIN_SET_VALUE( dmob.spin_czynneDo, pShop_temp->close_hour );
    SPIN_SET_VALUE( dmob.spin_marzaK, pShop_temp->profit_buy );
    SPIN_SET_VALUE( dmob.spin_marzaS, pShop_temp->profit_sell );

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dmob.check_czary ), dmob.mob->pHealer != NULL );
    pHealer_temp = dmob.mob->pHealer ? dmob.mob->pHealer : &dmob.pHealer_pusty;

    for ( spnum = 0; spnum <= 4; spnum++ )
    {
	int index = pHealer_temp->spells[ spnum ];

	dmob.czary[ spnum ] = index;

	if ( index == -1 )
	    gtk_label_set_text( dmob.label_sklepH[ spnum ], "(brak)" );
	else
	    gtk_label_set_text( dmob.label_sklepH[ spnum ], _( skill_table[ index ].pl_name ) );

	SPIN_SET_VALUE( dmob.spin_sklepH[ spnum ], pHealer_temp->cost[ spnum ] );
    }

    ile_progow_button( dmob.button_progi, dmob.mob->mobprogs );

    for ( i = 0; dmob.checkaff[ i ]; i++ )
	USTAW_CHECKBOXA_V( dmob.checkaff[ i ], dmob.mob->affected_by, dmob.check_na_aff[ i ] );
    /* jesli wystapila jakas zmiana, powinien zostac wygenerowany sygnal
       "toggled", ale jesli nie wystapila, i tak chce wypelnic etykiete
       odpowiednia informacja */
    zmiana_flagi_wplywu( NULL, NULL );

    for ( i = 0; dmob.checkact[ i ]; i++ )
	USTAW_CHECKBOXA( dmob.checkact[ i ], dmob.mob->act, dmob.check_na_act[ i ] );
    zmiana_flagi_zachowania( NULL, NULL );

    gtk_list_store_clear( dmob.store_resety );
    for ( a = area_first; a; a = a->next )
	for ( reset = a->reset_first; reset; reset = reset->next )
	    if ( reset->arg1 == dmob.mob->vnum && reset->command == 'M' )
	    {
		GtkTreeIter iter;
		ROOM_INDEX_DATA *room = get_room_index( reset->arg3 );

		sprintf( buf, "#%d: %s (%d)", reset->arg3, room->name, reset->arg2 );
		gtk_list_store_append( dmob.store_resety, &iter );
		gtk_list_store_set( dmob.store_resety, &iter,
		    KOL_NAZWA, _( buf ),
		    KOL_WSKAZNIK, reset, -1 );
	    }

    gtk_widget_show( dmob.Mob );

    return;
}


FUNKCJA_DLA_GTK_2( smierc_dialogu_moba )
{
    gtk_widget_hide( dmob.Mob );
    gtk_grab_remove( dmob.Mob );

    return TRUE; /* dialog nie umrze */
}


FUNKCJA_DLA_GTK( zatwierdzenie_zmian_moba )
{
    char buf[ MSL ];
    int i;

    WEZ_LANCUCH( dmob.entry_name );
    WEZ_LANCUCH( dmob.entry_short );
    WEZ_LANCUCH( dmob.entry_dopelniacz );
    WEZ_LANCUCH( dmob.entry_celownik );
    WEZ_LANCUCH( dmob.entry_biernik );
    WEZ_LANCUCH( dmob.entry_narzednik );
    WEZ_LANCUCH( dmob.entry_miejscownik );
    WEZ_LANCUCH( dmob.entry_wolacz );
    WEZ_LANCUCH( dmob.entry_long );

    WEZ_LANCUCH_TEXT( dmob.buffer );
    PUSC_LANCUCH_TEXT( dmob.buffer );

    dmob.mob->level = SPIN_GET_VALUE( dmob.spinbutton_poziom );
    dmob.mob->alignment = SPIN_GET_VALUE( dmob.spinbutton_moralnosc );
    dmob.mob->zamoznosc = SPIN_GET_VALUE( dmob.spinbutton_zamoznosc );

    dmob.mob->sex = COMBO_GET_ACTIVE( dmob.combo_plec );
    dmob.mob->race = dmob.rasa;

    /* -1 [(brak)] albo -2 [brak wyboru?] */
    i = COMBO_GET_ACTIVE( dmob.combo_spec ) - 1;
    dmob.mob->spec_fun = ( i < 0 ) ? NULL : spec_table[ i ].fun;

    i = COMBO_GET_ACTIVE( dmob.combo_krupier ) - 1;
    dmob.mob->game_fun = ( i < 0 ) ? NULL : game_table[ i ].fun;

    dmob.mob->grupa = SPIN_GET_VALUE( dmob.spin_grupa );
    dmob.mob->mod_size = SPIN_GET_VALUE( dmob.spin_wplywA );

    for ( i = 0; dmob.checkaff[ i ]; i++ )
	USTAW_FLAGE_V( dmob.checkaff[ i ], dmob.mob->affected_by, dmob.check_na_aff[ i ] );

    for ( i = 0; dmob.checkact[ i ]; i++ )
	USTAW_FLAGE( dmob.checkact[ i ], dmob.mob->act, dmob.check_na_act[ i ] );

    /* jesli w GUI zaznaczono sklep, to go zapiszmy */
    if ( CHECK_GET_ACTIVE( dmob.check_sklep ) )
    {
	int iTrade;

	/* stworzenie struktury sklepu, jesli wczesniej nie istniala */
	if ( !dmob.mob->pShop )
	    dmob.mob->pShop = new_shop( );

	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    dmob.mob->pShop->buy_type[ iTrade ] =
		numer_na_typ[ COMBO_GET_ACTIVE( dmob.combo_sklepS[ iTrade ] ) ];

	dmob.mob->pShop->open_hour = SPIN_GET_VALUE( dmob.spin_czynneOd );
	dmob.mob->pShop->close_hour = SPIN_GET_VALUE( dmob.spin_czynneDo );
	dmob.mob->pShop->profit_buy = SPIN_GET_VALUE( dmob.spin_marzaK );
	dmob.mob->pShop->profit_sell = SPIN_GET_VALUE( dmob.spin_marzaS );
    }
    /* W GUI sklep wylaczony, ale mob ma jakis sklep - usuwamy go */
    else if ( dmob.mob->pShop )
    {
	free_mem( dmob.mob->pShop, sizeof( SHOP_DATA ) );
	dmob.mob->pShop = NULL;
    }

    if ( CHECK_GET_ACTIVE( dmob.check_czary ) )
    {
	int spnum;

	if ( !dmob.mob->pHealer )
	    dmob.mob->pHealer = new_healer( );

	for ( spnum = 0; spnum < 5; spnum++ )
	{
	    dmob.mob->pHealer->spells[ spnum ] = dmob.czary[ spnum ];
	    dmob.mob->pHealer->cost[ spnum ] = SPIN_GET_VALUE( dmob.spin_sklepH[ spnum ] );
	}
    }
    else if ( dmob.mob->pHealer )
    {
	free_mem( dmob.mob->pHealer, sizeof( HEALER_DATA ) );
	dmob.mob->pHealer = NULL;
    }

    if ( memcmp( &dmob.iter, &NullIter, sizeof( NullIter ) ) )
    {
	strcpy( buf, _( dmob.mob->short_descr ) );
	gtk_list_store_set( dkra.store_mob, &dmob.iter, KOL_OPIS, buf, -1 );
    }

    if ( !data )
	smierc_dialogu_moba( NULL, NULL, NULL );

    dmob.mob->index_area->odnowiona = TRUE;

    return;
}


FUNKCJA_DLA_GTK( zmiana_flagi_wplywu )
{
    char buf[ MSL ];
    int tymczas[ MAX_AFF_FLAG ];
    int i;

    memset( tymczas, 0, MAX_AFF_FLAG );

    for ( i = 0; dmob.checkaff[ i ]; i++ )
	USTAW_FLAGE_V( dmob.checkaff[ i ], tymczas, dmob.check_na_aff[ i ] );

    strcpy( buf, "wp`lywy: " );
    strcat( buf, affect_bit_name_pl( tymczas ) );
    gtk_label_set_text( GTK_LABEL( dmob.label_wplywy ), _( buf ) );

    return;
}


FUNKCJA_DLA_GTK( zmiana_flagi_zachowania )
{
    char buf[ MSL ];
    int tymczas = 0;
    int i;

    for ( i = 0; dmob.checkact[ i ]; i++ )
	USTAW_FLAGE( dmob.checkact[ i ], tymczas, dmob.check_na_act[ i ] );

    strcpy( buf, "zachowanie: " );
    strcat( buf, act_flag_name_pl( tymczas, NULL ) );
    gtk_label_set_text( GTK_LABEL( dmob.label_zachowania ), _( buf ) );

    return;
}


static gboolean czary_moba_filter( GtkTreeModel *model, GtkTreePath *path,
				 GtkTreeIter *iter, gpointer data )
{
    char *s;
    gboolean b;
    int sn;

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &s, KOL_VIS, &b, KOL_ELEMENT, &sn, -1 );

    if ( sn == -1 ) /* zawsze pokazuj "(brak)" */
    {
	if ( !b )
	    gtk_list_store_set( dcom.store, iter, KOL_VIS, TRUE, -1 );
	return FALSE;
    }

    if ( skill_table[ sn ].target != TAR_CHAR_DEFENSIVE
      && !IS_SET( skill_table[ sn ].flags, SKILL_KUPOWALNE ) )
    {
	if ( b )
	    gtk_list_store_set( dcom.store, iter, KOL_VIS, FALSE, -1 );
	return FALSE;
    }

    if ( !b && *dcom.text == '\0' )
    {
	gtk_list_store_set( dcom.store, iter, KOL_VIS, TRUE, -1 );
	return FALSE;
    }

    if ( str_infix( dcom.text, s ) == b )
	gtk_list_store_set( dcom.store, iter, KOL_VIS, !b, -1 );

    return FALSE; /* szukaj dalej */
}


FUNKCJA_DLA_GTK( combo_mob_sklepH )
{
    int i;

    for ( i = 0; ; i++ )
	if ( i == 5 )
	    return;
	else if ( widget == dmob.toggle_sklepH[ i ] )
	    break;

    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filter_czary, widget, czary_moba_filter );
    else if ( dcom.wybrany )
    {
	int sn;

	gtk_tree_model_get( filter_czary, &dcom.iter, KOL_ELEMENT, &sn, -1 );
	if ( sn == -1 )
	    gtk_label_set_text( dmob.label_sklepH[ i ], "(brak)" );
	else
	    gtk_label_set_text( dmob.label_sklepH[ i ], _( skill_table[ sn ].pl_name ) );
	dmob.czary[ i ] = sn;
    }

    return;
}
