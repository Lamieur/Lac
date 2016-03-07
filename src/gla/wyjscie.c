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
 * ----------------------------------------------------------------------- *
 *  Kod odpowiedzialny za dialog wyjscia (tworzenie i edycja)              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "glaca.h"
#include "../handler.h"
#include "../const.h"


struct dwyj_data dwyj;

FUNKCJA_DLA_GTK( zmiana_flagi_wyjscia );


void zrob_okno_Wyjscia( void )
{
    int i, j;
    GtkWidget *vbox;

    ZNAJDZ_KONTROLKE( dwyj.Wyjscie, "oknowyjscie" );
    ZNAJDZ_KONTROLKE( dwyj.entry_opis, "wyj_entry_opis" );
    ZNAJDZ_KONTROLKE( dwyj.entry_keywords, "wyj_entry_keywords" );
    ZNAJDZ_KONTROLKE( dwyj.entry_mianownik, "wyj_entry_mianownik" );
    ZNAJDZ_KONTROLKE( dwyj.entry_biernik, "wyj_entry_biernik" );
    ZNAJDZ_KONTROLKE( dwyj.spin_klucz, "wyj_spin_klucz" );
    ZNAJDZ_KONTROLKE( dwyj.spin_to_room, "wyj_spin_to_room" );
    ZNAJDZ_KONTROLKE( dwyj.combo_rodzaj, "wyj_combo_rodzaj" );
    ZNAJDZ_KONTROLKE( dwyj.button_progi, "wyj_button_progi" );
    ZNAJDZ_KONTROLKE( dwyj.label_flagi, "wyj_label_flagi" );
    ZNAJDZ_KONTROLKE( dwyj.combo_akcja, "wyj_combo_akcja" );
    ZNAJDZ_KONTROLKE( dwyj.hbox_opis, "wyj_hbox_opis" );
    ZNAJDZ_KONTROLKE( dwyj.hbox_pola, "wyj_hbox_pola" );
    ZNAJDZ_KONTROLKE( dwyj.label_wyjscie, "wyj_label_wyjscie" );
    ZNAJDZ_KONTROLKE( dwyj.label_klucz, "wyj_label_klucz" );

    ZNAJDZ_KONTROLKE2( dwyj.store_akcjanowe, "wyj_store_akcjanowe", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE2( dwyj.store_akcja, "wyj_store_akcja", GTK_LIST_STORE );
    g_object_ref( dwyj.store_akcjanowe );
    g_object_ref( dwyj.store_akcja );

    ZNAJDZ_KONTROLKE( vbox, "wyj_vbox_flagi" );
    for ( i = j = 0; exit_info_flags[ i ].name; i++ )
	if ( exit_info_flags[ i ].available )
	{
	    dwyj.check[ j ] = gtk_check_button_new_with_label( _( exit_info_flags[ i ].name ) );
	    gtk_widget_set_tooltip_text( dwyj.check[ j ], _( exit_info_flags[ i ].tooltip ) );
	    gtk_box_pack_start( GTK_BOX( vbox ), dwyj.check[ j ], FALSE, FALSE, 0 );
	    g_signal_connect( G_OBJECT( dwyj.check[ j ] ), "toggled",
		G_CALLBACK( zmiana_flagi_wyjscia ), NULL );
	    gtk_widget_show( dwyj.check[ j ] );
	    dwyj.check_na_flage[ j ] = exit_info_flags[ i ].bitv;
	    j++;
	}

    return;
}


void dialog_wyjscia( ROOM_INDEX_DATA *room, int door )
{
    char buf[ 2 * MIL ];
    int i;

    if ( !room )
	return;

    dwyj.room = room;
    dwyj.door = door;
    dwyj.exit = room->exit[ door ];

    if ( !dwyj.exit )
    {
	dwyj.exit = dwyj.nowe = new_exit( );

	gtk_combo_box_set_model( GTK_COMBO_BOX( dwyj.combo_akcja ),
				 GTK_TREE_MODEL( dwyj.store_akcjanowe ) );
	COMBO_SET_ACTIVE( dwyj.combo_akcja, 0 );

	strcpy( buf, "dwyj.nowe wyj`scie" );
    }
    else
    {
	gtk_combo_box_set_model( GTK_COMBO_BOX( dwyj.combo_akcja ),
				 GTK_TREE_MODEL( dwyj.store_akcja ) );

	if ( dwyj.exit->to_room )
	{
	    COMBO_SET_ACTIVE( dwyj.combo_akcja, 0 );
	    sprintf( buf, "Wyj`scie do: #%d (%s)", dwyj.exit->to_room->vnum, dwyj.exit->to_room->name );
	}
	else
	{
	    COMBO_SET_ACTIVE( dwyj.combo_akcja, 1 );
	    strcpy( buf, "Wyj`scie bez pomieszczenia docelowego" );
	}
    }

    gtk_window_set_title( GTK_WINDOW( dwyj.Wyjscie ), _( buf ) );

    ZROB_LANCUCH( dwyj.entry_opis, dwyj.exit->description );
    ZROB_LANCUCH( dwyj.entry_keywords, dwyj.exit->keyword );
    ZROB_LANCUCH( dwyj.entry_mianownik, dwyj.exit->short_descr );
    ZROB_LANCUCH( dwyj.entry_biernik, dwyj.exit->biernik );

    SPIN_SET_VALUE( dwyj.spin_klucz, dwyj.exit->key );
    SPIN_SET_VALUE( dwyj.spin_to_room, dwyj.exit->vnum );

    COMBO_SET_ACTIVE( dwyj.combo_rodzaj, dwyj.exit->rodzaj );

    for ( i = 0; dwyj.check[ i ]; i++ )
	USTAW_CHECKBOXA( dwyj.check[ i ], dwyj.exit->exit_info, dwyj.check_na_flage[ i ] );
    zmiana_flagi_wyjscia( NULL, NULL );

    ile_progow_button( dwyj.button_progi, dwyj.exit->mobprogs );

    gtk_grab_add( dwyj.Wyjscie );
    gtk_widget_show( dwyj.Wyjscie );

    return;
}


FUNKCJA_DLA_GTK_2( smierc_dialogu_wyjscia )
{
    if ( dwyj.nowe )
    {
	del_exit( dwyj.nowe );
	dwyj.nowe = NULL;
    }

    gtk_widget_hide( dwyj.Wyjscie );
    gtk_grab_remove( dwyj.Wyjscie );

    return TRUE;
}


FUNKCJA_DLA_GTK( zatwierdzenie_zmian_wyjscia )
{
    int i;
    EXIT_DATA **drugie = NULL;

    if ( dwyj.nowe )
    {
	dwyj.room->exit[ dwyj.door ] = dwyj.nowe;
	dwyj.nowe = NULL;
    }

    switch ( COMBO_GET_ACTIVE( dwyj.combo_akcja ) )
    {
    case 0: /* zwykle przejscie */
	WEZ_LANCUCH( dwyj.entry_opis );
	WEZ_LANCUCH( dwyj.entry_keywords );
	WEZ_LANCUCH( dwyj.entry_mianownik );
	WEZ_LANCUCH( dwyj.entry_biernik );

	dwyj.exit->key = SPIN_GET_VALUE( dwyj.spin_klucz );
	dwyj.exit->vnum = SPIN_GET_VALUE( dwyj.spin_to_room );
	dwyj.exit->rodzaj = COMBO_GET_ACTIVE( dwyj.combo_rodzaj );

	for ( i = 0; dwyj.check[ i ]; i++ )
	    USTAW_FLAGE( dwyj.check[ i ], dwyj.exit->exit_info, dwyj.check_na_flage[ i ] );

	/* jesli nie znajdzie, to uzna, ze to sciana */
	dwyj.exit->to_room = get_room_index( dwyj.exit->vnum );
	break;

    case 1: /* bez pomieszczenia docelowego */
	del_exit( dwyj.exit );
	dwyj.exit = new_exit( );
	dwyj.exit->description = str_dup( lac_z_pols( ENTRY_GET_TEXT( dwyj.entry_opis ) ) );
	break;

    case 3: /* usun w obu kierunkach */
	if ( dwyj.exit->to_room )
	    drugie = &dwyj.exit->to_room->exit[ kierunki[ dwyj.door ].odwrotny ];

	if ( drugie && *drugie )
	{
	    del_exit( *drugie );
	    *drugie = NULL;
	}
	/* celowy brak break; */

    case 2: /* usun w jednym kierunku */
	del_exit( dwyj.exit );
	dwyj.room->exit[ dwyj.door ] = NULL;

    default:
	break;
    }

    aktualizuj_etykietki_wyjsc( );
    maluj_mape( dmapa.drawingarea );
    smierc_dialogu_wyjscia( NULL, NULL, NULL );
    dwyj.room->area->odnowiona = TRUE;
    if ( dwyj.exit->to_room )
	dwyj.exit->to_room->area->odnowiona = TRUE;

    return;
}


FUNKCJA_DLA_GTK( zmiana_flagi_wyjscia )
{
    char buf[ MSL ];
    int tymczas = 0;
    int i;

    for ( i = 0; dwyj.check[ i ]; i++ )
	USTAW_FLAGE( dwyj.check[ i ], tymczas, dwyj.check_na_flage[ i ] );

    strcpy( buf, "flagi: " );
    strcat( buf, exit_info_name_pl( tymczas ) );
    gtk_label_set_text( GTK_LABEL( dwyj.label_flagi ), _( buf ) );

    return;
}


FUNKCJA_DLA_GTK( zmiana_akcji_wyjscia )
{
    int i = COMBO_GET_ACTIVE( dwyj.combo_akcja );

    glaca_widget_set_visible( dwyj.hbox_opis, ( i == 0 || i == 1 ) );
    glaca_widget_set_visible( dwyj.hbox_pola, ( i == 0 ) );
    glaca_widget_set_visible( dwyj.label_flagi, ( i == 0 ) );

    return;
}


FUNKCJA_DLA_GTK( zmiana_celu_wyjscia )
{
    ROOM_INDEX_DATA *room = get_room_index( SPIN_GET_VALUE( dwyj.spin_to_room ) );

    if ( !room )
	gtk_label_set( GTK_LABEL( dwyj.label_wyjscie ), _( "pomieszczenie nie istnieje" ) );
    else
	gtk_label_set( GTK_LABEL( dwyj.label_wyjscie ), _( room->name ) );

    return;
}


FUNKCJA_DLA_GTK( zmiana_klucza_wyjscia )
{
    OBJ_INDEX_DATA *obj;
    int vnum = SPIN_GET_VALUE( dwyj.spin_klucz );

    if ( vnum == -1 )
	gtk_label_set( GTK_LABEL( dwyj.label_klucz ), _( "drzwi bez klucza" ) );
    else if ( !( obj = get_obj_index( vnum ) ) )
	gtk_label_set( GTK_LABEL( dwyj.label_klucz ), _( "przedmiot nie istnieje" ) );
    else
	gtk_label_set( GTK_LABEL( dwyj.label_klucz ), _( obj->short_descr ) );

    return;
}


FUNKCJA_DLA_GTK( combo_pom_docel )
{
    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filter_pom, widget, NULL );
    else if ( dcom.wybrany )
    {
	ROOM_INDEX_DATA *room;

	gtk_tree_model_get( filter_pom, &dcom.iter, KOL_ELEMENT, &room, -1 );
	SPIN_SET_VALUE( dwyj.spin_to_room, room->vnum );
    }

    return;
}


static gboolean klucze_filter( GtkTreeModel *model, GtkTreePath *path,
				 GtkTreeIter *iter, gpointer data )
{
    char *s;
    gboolean b;
    OBJ_INDEX_DATA *o;

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &s, KOL_VIS, &b, KOL_ELEMENT, &o, -1 );

    if ( o->item_type != ITEM_KEY )
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


FUNKCJA_DLA_GTK( combo_klucze )
{
    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filter_prz, widget, klucze_filter );
    else if ( dcom.wybrany )
    {
	OBJ_INDEX_DATA *obj;

	gtk_tree_model_get( filter_prz, &dcom.iter, KOL_ELEMENT, &obj, -1 );
	SPIN_SET_VALUE( dwyj.spin_klucz, obj->vnum );
    }

    return;
}

