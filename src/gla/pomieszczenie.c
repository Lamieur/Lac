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

struct dpom_data dpom;

gboolean zmien_wyjscie( GtkWidget *widget, GdkEventCrossing *event, gpointer data );
FUNKCJA_DLA_GTK( zatwierdzenie_zmian_pomieszczenia );
FUNKCJA_DLA_GTK( edytuj_wyjscie );
FUNKCJA_DLA_GTK( zmiana_flagi_pomieszczenia );


void zrob_okno_Pomieszczenia( void )
{
    int i, j;
    GtkWidget *tree, *text;
    GtkTable *table;
    char nazwa[ MIL ];

    ZNAJDZ_KONTROLKE( dpom.Pomieszczenie, "oknopomieszczenie" );
    ZNAJDZ_KONTROLKE( dpom.entry_nazwa, "pom_entry_nazwa" );
    ZNAJDZ_KONTROLKE( dpom.entry_miejscownik, "pom_entry_miejscownik" );
    ZNAJDZ_KONTROLKE( text, "pom_text_opis" );

    /* tego nie potrafi GLADE, kaka */
    gtk_widget_modify_font( text, font_desc );

#if !defined( BEZ_GTKSPELL )
    gtkspell_new_attach( GTK_TEXT_VIEW( text ), NULL, NULL );
#endif

    ZNAJDZ_KONTROLKE2( dpom.buffer, "pom_buffer_opis", GTK_TEXT_BUFFER );

    ZNAJDZ_KONTROLKE( dpom.combobox_podloze, "pom_combo_podloze" );
    ZNAJDZ_KONTROLKE( dpom.spin_pojemnosc, "pom_spin_pojemnosc" );
    ZNAJDZ_KONTROLKE( dpom.spin_ruch, "pom_spin_ruch" );
    ZNAJDZ_KONTROLKE( dpom.entry_antirace, "pom_entry_antirace" );
    ZNAJDZ_KONTROLKE( dpom.entry_anticlass, "pom_entry_anticlass" );

    for ( i = 0; i < 10; i++ )
    {
	sprintf( nazwa, "pom_label_%d", i );
	ZNAJDZ_KONTROLKE( dpom.label[ i ], nazwa );

	sprintf( nazwa, "pom_button_%d", i );
	ZNAJDZ_KONTROLKE( dpom.button[ i ], nazwa );
	gtk_signal_connect( GTK_OBJECT( dpom.button[ i ] ), "enter-notify-event",
			    GTK_SIGNAL_FUNC( zmien_wyjscie ), GINT_TO_POINTER( i ) );
	gtk_signal_connect( GTK_OBJECT( dpom.button[ i ] ), "clicked",
			    GTK_SIGNAL_FUNC( edytuj_wyjscie ), GINT_TO_POINTER( i ) );
    }

    ZNAJDZ_KONTROLKE2( dpom.store_resety, "pom_store_resety", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE( tree, "pom_treeview_resety" );
    dpom.select_reset = gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );

    ZNAJDZ_KONTROLKE( dpom.button_opisy, "pom_button_opisy" );
    ZNAJDZ_KONTROLKE( dpom.label_opisy, "pom_label_opisy" );
    ZNAJDZ_KONTROLKE( dpom.label_wyjscie, "pom_label_wyjscie" );
    ZNAJDZ_KONTROLKE( dpom.button_progi, "pom_button_progi" );
    ZNAJDZ_KONTROLKE( dpom.button_poprz, "pom_button_poprz" );
    ZNAJDZ_KONTROLKE( dpom.button_nast, "pom_button_nast" );
    ZNAJDZ_KONTROLKE( dpom.label_flagi, "pom_label_flagi" );
    ZNAJDZ_KONTROLKE( dpom.vbox_pulapka, "pom_vbox_pulapka" );
    ZNAJDZ_KONTROLKE2( dpom.radio_pulapka1, "pom_radio_pulapka1", GTK_RADIO_BUTTON );
    ZNAJDZ_KONTROLKE2( dpom.radio_pulapka2, "pom_radio_pulapka2", GTK_RADIO_BUTTON );
    ZNAJDZ_KONTROLKE2( dpom.buffer_pulapka, "pom_buffer_pulapka", GTK_TEXT_BUFFER );
    ZNAJDZ_KONTROLKE2( dpom.check_pulapka, "pom_check_pulapka", GTK_CHECK_BUTTON );

    ZNAJDZ_KONTROLKE2( table, "pom_table_zachowania", GTK_TABLE );
    for ( i = j = 0; room_flags_table[ i ].name; i++ )
	if ( room_flags_table[ i ].available )
	{
	    dpom.check[ j ] = gtk_check_button_new_with_label( _( room_flags_table[ i ].name ) );
	    gtk_widget_set_tooltip_text( dpom.check[ j ], _( room_flags_table[ i ].tooltip ) );
	    gtk_table_attach( table, dpom.check[ j ],
		j % 2, j % 2 + 1, j / 2, j / 2 + 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0 );
	    g_signal_connect( G_OBJECT( dpom.check[ j ] ), "toggled",
		G_CALLBACK( zmiana_flagi_pomieszczenia ), NULL );
	    dpom.check_na_flage[ j ] = room_flags_table[ i ].bitv;
	    j++;
	}
    gtk_widget_show_all( GTK_WIDGET( table ) );

    return;
}


FUNKCJA_DLA_GTK( poprznast_pom )
{
    GtkTreePath *path;
    GtkTreeModel *model;

    if ( !gtk_tree_selection_get_selected( dkra.selection_pom, &model, &dpom.iter ) )
	return;

    path = gtk_tree_model_get_path( model, &dpom.iter );

    zatwierdzenie_zmian_pomieszczenia( NULL, GINT_TO_POINTER( 1 ) );

    if ( widget == dpom.button_poprz )
	gtk_tree_path_prev( path );
    else if ( widget == dpom.button_nast )
	gtk_tree_path_next( path );

    gtk_tree_selection_select_path( dkra.selection_pom, path );
    gtk_tree_path_free( path );
    pomieszczenie_z_listy( NULL, NULL );

    return;
}


gboolean zmien_wyjscie( GtkWidget *widget, GdkEventCrossing *event, gpointer data )
{
    char buf[ 1000 ];
    char *dupa;
    int door;
    ROOM_INDEX_DATA *room = dpom.room;

    door = GPOINTER_TO_INT( data );

    dupa = buf + sprintf( buf, "%s: ", kierunki[ door ].na_krew );
    if ( room->exit[ door ] )
    {
	if ( room->exit[ door ]->to_room )
	    sprintf( dupa, "#%d: \n%s\n%s",
		room->exit[ door ]->to_room->vnum,
		room->exit[ door ]->description,
		room->exit[ door ]->to_room->name );
	else
	    sprintf( dupa, "\n%s", "wyj`scie bez docelowego pomieszczenia." );
    }
    else
	sprintf( dupa, "\n%s", "brak wyj`scia" );

    gtk_label_set( GTK_LABEL( dpom.label_wyjscie ), _( buf ) );

    return FALSE;
}



FUNKCJA_DLA_GTK( pomieszczenie_z_listy )
{
    GtkTreeModel *model;

    if ( !gtk_tree_selection_get_selected( dkra.selection_pom, &model, &dpom.iter ) )
	return;
    gtk_tree_model_get( model, &dpom.iter, KOL_WSKAZNIK, &dpom.room, -1 );
    dialog_pomieszczenia( NULL, NULL );
}


FUNKCJA_DLA_GTK( dialog_pomieszczenia )
{
    int i;
    AREA_DATA *a;
    RESET_DATA *reset;
    char buf[ MIL ];
    ROOM_INDEX_DATA *room = dpom.room;

    if ( !dpom.room )
	return;

    gtk_grab_add( dpom.Pomieszczenie );

    sprintf( buf, "#%d: %s (%s)",
	room->vnum,
	room->name,
	room->area->full_name );
    gtk_window_set_title( GTK_WINDOW( dpom.Pomieszczenie ), _( buf ) );

    ZROB_LANCUCH( dpom.entry_nazwa, room->name );
    ZROB_LANCUCH( dpom.entry_miejscownik, room->miejscownik );
    ZROB_LANCUCH_TEXT( dpom.buffer, room->description );
    COMBO_SET_ACTIVE( dpom.combobox_podloze, room->sector_type );
    SPIN_SET_VALUE( dpom.spin_pojemnosc, room->pojemnosc );
    SPIN_SET_VALUE( dpom.spin_ruch, room->strata_ruchu );
    ZROB_LANCUCH( dpom.entry_antirace, room->anti_race );
    ZROB_LANCUCH( dpom.entry_anticlass, room->anti_class );

    ile_opisow_label( dpom.label_opisy, room->extra_descr );

    ile_progow_button( dpom.button_progi, room->mobprogs );

    for ( i = 0; dpom.check[ i ]; i++ )
	USTAW_CHECKBOXA( dpom.check[ i ], dpom.room->original_room_flags, dpom.check_na_flage[ i ] );
    zmiana_flagi_pomieszczenia( NULL, NULL );

    aktualizuj_etykietki_wyjsc( );
    gtk_label_set( GTK_LABEL( dpom.label_wyjscie ), "" );

    if ( dpom.room->trap )
    {
	CHECK_SET_ACTIVE( dpom.check_pulapka, TRUE );
	if ( dpom.room->trap->trap_type == 0 )
	    CHECK_SET_ACTIVE( dpom.radio_pulapka1, TRUE );
	else
	    CHECK_SET_ACTIVE( dpom.radio_pulapka2, TRUE );
	ZROB_LANCUCH_TEXT( dpom.buffer_pulapka, dpom.room->trap->trap_string );
    }
    else
    {
	gtk_text_buffer_set_text( dpom.buffer_pulapka, "", 0 );
	CHECK_SET_ACTIVE( dpom.check_pulapka, FALSE );
    }
    gtk_text_buffer_set_modified( dpom.buffer_pulapka, FALSE );

    gtk_list_store_clear( dpom.store_resety );
    for ( a = area_first; a; a = a->next )
	for ( reset = a->reset_first; reset; reset = reset->next )
	    if ( reset->arg3 == dpom.room->vnum )
	    {
		if ( reset->command == 'M' )
		{
		    GtkTreeIter iter;
		    MOB_INDEX_DATA *mob = get_mob_index( reset->arg1 );

		    sprintf( buf, "#%d: %s (%d)", reset->arg1, mob->short_descr, reset->arg2 );
		    gtk_list_store_append( dpom.store_resety, &iter );
		    gtk_list_store_set( dpom.store_resety, &iter,
			KOL_NAZWA, _( buf ),
			KOL_WSKAZNIK, reset, -1 );
		}
		else if ( reset->command == 'O' )
		{
		    GtkTreeIter iter;
		    OBJ_INDEX_DATA *obj = get_obj_index( reset->arg1 );

		    sprintf( buf, "#%d: %s [%d]", reset->arg1, obj->short_descr, reset->arg2 );
		    gtk_list_store_append( dpom.store_resety, &iter );
		    gtk_list_store_set( dpom.store_resety, &iter,
			KOL_NAZWA, _( buf ),
			KOL_WSKAZNIK, reset, -1 );
		}
	    }


    gtk_widget_show( dpom.Pomieszczenie );

    return;
}


/* dialog pomieszczenia nie umiera, on sie tylko chowa */
FUNKCJA_DLA_GTK_2( smierc_dialogu_pomieszczenia )
{
    gtk_widget_hide( dpom.Pomieszczenie );
    gtk_grab_remove( dpom.Pomieszczenie );

    return TRUE; /* dialog nie umrze */
}


FUNKCJA_DLA_GTK( zatwierdzenie_zmian_pomieszczenia )
{
    char buf[ MSL ];
    int i;

    WEZ_LANCUCH( dpom.entry_nazwa );
    WEZ_LANCUCH( dpom.entry_miejscownik );

    WEZ_LANCUCH_TEXT( dpom.buffer );
    PUSC_LANCUCH_TEXT( dpom.buffer );

    dpom.room->pojemnosc = SPIN_GET_VALUE( dpom.spin_pojemnosc );
    dpom.room->strata_ruchu = SPIN_GET_VALUE( dpom.spin_ruch );
    dpom.room->sector_type = COMBO_GET_ACTIVE( dpom.combobox_podloze );

    WEZ_LANCUCH( dpom.entry_antirace );
    WEZ_LANCUCH( dpom.entry_anticlass );

    for ( i = 0; dpom.check[ i ]; i++ )
	USTAW_FLAGE( dpom.check[ i ], dpom.room->original_room_flags, dpom.check_na_flage[ i ] );

    /* jesli w GUI zaznaczono pulapke, to ja zapiszmy */
    if ( CHECK_GET_ACTIVE( dpom.check_pulapka ) )
    {
	/* stworzenie struktury, jesli wczesniej nie istniala */
	if ( !dpom.room->trap )
	{
	    GtkTextIter start, end;
	    char *text;

	    dpom.room->trap = alloc_mem( sizeof( DEATHTRAP_DATA ) );

	    gtk_text_buffer_get_bounds( dpom.buffer_pulapka, &start, &end );
	    text = gtk_text_buffer_get_text( dpom.buffer_pulapka, &start, &end, FALSE );
	    strcpy( buf, lac_z_pols( text ) );
	    g_free( text );
	    dpom.room->trap->trap_string = str_dup( buf );
	}
	else if ( gtk_text_buffer_get_modified( dpom.buffer_pulapka ) )
	    WEZ_LANCUCH_TEXT( dpom.buffer_pulapka );

	dpom.room->trap->trap_type = CHECK_GET_ACTIVE( dpom.radio_pulapka1 ) ? 0 : 1;
    }
    /* W GUI pulapka wylaczona, ale w pomieszczeniu nadal jest - usuwamy ja */
    else if ( dpom.room->trap )
    {
	if ( dpom.room->trap->trap_string )
	    free_string( dpom.room->trap->trap_string );
	free_mem( dpom.room->trap, sizeof( DEATHTRAP_DATA ) );
	dpom.room->trap = NULL;
    }

    if ( memcmp( &dpom.iter, &NullIter, sizeof( NullIter ) ) )
    {
	strcpy( buf, _( dpom.room->name ) );
	gtk_list_store_set( dkra.store_pom, &dpom.iter, KOL_OPIS, buf, -1 );
    }

    if ( !data )
	smierc_dialogu_pomieszczenia( NULL, NULL, NULL );

    dpom.room->area->odnowiona = TRUE;

    return;
}


FUNKCJA_DLA_GTK( edytuj_wyjscie )
{
    int door = GPOINTER_TO_INT( data );

    dialog_wyjscia( dpom.room, door );

    return;
}


FUNKCJA_DLA_GTK( zmiana_flagi_pomieszczenia )
{
    char buf[ MSL ];
    int tymczas = 0;
    int i;

    for ( i = 0; dpom.check[ i ]; i++ )
	USTAW_FLAGE( dpom.check[ i ], tymczas, dpom.check_na_flage[ i ] );

    strcpy( buf, "flagi: " );
    strcat( buf, room_flag_name_pl( tymczas ) );
    gtk_label_set_text( GTK_LABEL( dpom.label_flagi ), _( buf ) );

    return;
}


void aktualizuj_etykietki_wyjsc( void )
{
    char buf[ 1000 ];
    ROOM_INDEX_DATA *room = dpom.room;
    int i;

    if ( !room )
	return;

    for ( i = 0; i < 10; i++ )
    {
	if ( room->exit[ i ] )
	{
	    if ( room->exit[ i ]->to_room )
		sprintf( buf, "#%d", room->exit[ i ]->to_room->vnum );
	    else
		strcpy( buf, _( "(`sciana)" ) );
	}
	else
	    strcpy( buf, "-" );
	gtk_label_set( GTK_LABEL( dpom.label[ i ] ), buf );
    }

    return;
}


FUNKCJA_DLA_GTK( zmiana_check_pulapka )
{
    glaca_widget_set_visible( dpom.vbox_pulapka, CHECK_GET_ACTIVE( dpom.check_pulapka ) );

    return;
}
