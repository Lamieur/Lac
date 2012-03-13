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
 *  Glowne okienko edycji pojedynczej krainy                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "glaca.h"

struct dkra_data dkra;

FUNKCJA_DLA_GTK( zmiana_strefy );
FUNKCJA_DLA_GTK( wczytaj_pola_AREA );
FUNKCJA_DLA_GTK( zmiana_strefy );

void zrob_okno_Krainy( void )
{
    ZONE_DATA *strefa;
    GtkTable *table;
    int i;

    ZNAJDZ_KONTROLKE( dkra.okno_Krainy, "oknoglowne" );
    ZNAJDZ_KONTROLKE( dkra.entry_name, "kra_entry_name" );
    ZNAJDZ_KONTROLKE( dkra.entry_fullname, "kra_entry_fullname" );
    ZNAJDZ_KONTROLKE( dkra.entry_resetmsg, "kra_entry_resetmsg" );
    ZNAJDZ_KONTROLKE( dkra.spin_recall, "kra_spin_recall" );
    ZNAJDZ_KONTROLKE( dkra.entry_wersja, "kra_entry_wersja" );
    ZNAJDZ_KONTROLKE( dkra.spin_strefa, "kra_spin_strefa" );
    ZNAJDZ_KONTROLKE( dkra.check_nielista, "kra_check_nielista" );
    ZNAJDZ_KONTROLKE( dkra.check_nieteleport, "kra_check_nieteleport" );
    ZNAJDZ_KONTROLKE2( dkra.buffer_opis, "kra_buffer_opis", GTK_TEXT_BUFFER );
    ZNAJDZ_KONTROLKE( dkra.button_przywroc, "kra_button_przywroc" );
    ZNAJDZ_KONTROLKE2( dkra.tree_pom, "kra_tree_pom", GTK_TREE_VIEW );
    ZNAJDZ_KONTROLKE2( dkra.tree_mob, "kra_tree_mob", GTK_TREE_VIEW );
    ZNAJDZ_KONTROLKE2( dkra.tree_prz, "kra_tree_prz", GTK_TREE_VIEW );
    ZNAJDZ_KONTROLKE2( dkra.tree_res, "kra_tree_res", GTK_TREE_VIEW );
    ZNAJDZ_KONTROLKE2( dkra.store_pom, "kra_store_pom", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE2( dkra.store_mob, "kra_store_mob", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE2( dkra.store_prz, "kra_store_prz", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE2( dkra.store_res, "kra_treestore_res", GTK_TREE_STORE );
    dkra.selection_pom = gtk_tree_view_get_selection( GTK_TREE_VIEW( dkra.tree_pom ) );
    dkra.selection_mob = gtk_tree_view_get_selection( GTK_TREE_VIEW( dkra.tree_mob ) );
    dkra.selection_prz = gtk_tree_view_get_selection( GTK_TREE_VIEW( dkra.tree_prz ) );
    dkra.selection_res = gtk_tree_view_get_selection( GTK_TREE_VIEW( dkra.tree_res ) );
    ZNAJDZ_KONTROLKE( dkra.button_upom, "kra_button_pom_dodaj" );
    ZNAJDZ_KONTROLKE( dkra.button_umob, "kra_button_mob_dodaj" );
    ZNAJDZ_KONTROLKE( dkra.button_uprz, "kra_button_prz_dodaj" );

    ZNAJDZ_KONTROLKE2( table, "kra_table_strefy", GTK_TABLE );
    i = 0;
    for ( strefa = zone_first; strefa; strefa = strefa->next )
    {
	GtkWidget *b = gtk_check_button_new_with_label( _( strefa->nazwa ) );
	dkra.check_strefa[ i ] = GTK_TOGGLE_BUTTON( b );
	gtk_table_attach( table, b,
	    i % 2, i % 2 + 1, i / 2, i / 2 + 1,
	    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0 );
	g_signal_connect( G_OBJECT( dkra.check_strefa[ i ] ), "toggled",
	    G_CALLBACK( zmiana_strefy ), NULL );
	i++;
    }
    gtk_widget_show_all( GTK_WIDGET( table ) );

    return;
}


static gboolean ogwiazdkuj_zmienione(
		GtkTreeModel *model, GtkTreePath *path,
		GtkTreeIter *iter, gpointer data )
{
    AREA_DATA *a;
    static char nazwa[ MIL ], autor[ MIL ];
    char *zakres;

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &a, -1 );

    if ( a->odnowiona )
    {
	sprintf( nazwa, "* %s", a->file_name );
	zakres = wiersz( a, autor );
	gtk_list_store_set( dlis.store_krain, iter,
	    KOL_NAZWA, nazwa,
	    /* autor i zakres mogly sie zmienic */
	    KOL_OPIS, autor,
	    KOL_OPIS2, zakres, -1 );
    }

    return FALSE; /* szukaj dalej */
}


FUNKCJA_DLA_GTK( smierc_dialogu_krainy )
{
    gtk_list_store_clear( dkra.store_pom );
    gtk_list_store_clear( dkra.store_mob );
    gtk_list_store_clear( dkra.store_prz );
    gtk_tree_store_clear( dkra.store_res );

    gtk_tree_model_foreach( GTK_TREE_MODEL( dlis.store_krain ), ogwiazdkuj_zmienione, NULL );

    gtk_widget_hide( dkra.okno_Krainy );
    gtk_widget_show( dlis.okno_lista_krain );

    return;
}

static void wczytaj_zaznaczone_krainy(
		GtkTreeModel *model, GtkTreePath *path,
		GtkTreeIter *iter, gpointer userdata )
{
    GtkTreeIter additer, mobiter, objiter;
    static char bufnum[ MIL ], buf[ MIL ];
    AREA_DATA *a;
    ROOM_INDEX_DATA *room;
    MOB_INDEX_DATA *mob;
    OBJ_INDEX_DATA *obj;
    RESET_DATA *reset;
    int vnum;
    static char *	const	stany_drzwi	[ ]	=
    {
	"otw`orz, odklucz, odkop",
	"zamknij, ale odklucz i odkop",
	"zamknij i zaklucz, ale odkop",
	"zamknij i zakop, ale odklucz",
	"zamknij, zaklucz i zakop"
    };

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &a, -1 );

    if ( !area )
	area = a;

    buf[ 0 ] = '\0';

    for ( vnum = a->lowroomvnum; vnum <= a->highroomvnum; vnum++ )
	if ( ( room = get_room_index( vnum ) ) && room->area == a )
	{
	    sprintf( bufnum, "#%d", room->vnum );
	    strcpy( buf, _( room->name ) );

	    gtk_list_store_append( dkra.store_pom, &additer );
	    gtk_list_store_set( dkra.store_pom, &additer,
		KOL_WSKAZNIK, room,
		KOL_VNUM, bufnum,
		KOL_OPIS, buf, -1 );
	}

    for ( vnum = a->lowmobvnum; vnum <= a->highmobvnum; vnum++ )
	if ( ( mob = get_mob_index( vnum ) ) && mob->index_area == a )
	{
	    sprintf( bufnum, "#%d", mob->vnum );
	    strcpy( buf, _( mob->short_descr ) );

	    gtk_list_store_append( dkra.store_mob, &additer );
	    gtk_list_store_set( dkra.store_mob, &additer,
		KOL_WSKAZNIK, mob,
		KOL_VNUM, bufnum,
		KOL_OPIS, buf, -1 );
	}

    for ( vnum = a->lowobjvnum; vnum <= a->highobjvnum; vnum++ )
	if ( ( obj = get_obj_index( vnum ) ) && obj->area == a )
	{
	    sprintf( bufnum, "#%d", obj->vnum );
	    strcpy( buf, _( obj->short_descr ) );

	    gtk_list_store_append( dkra.store_prz, &additer );
	    gtk_list_store_set( dkra.store_prz, &additer,
		KOL_WSKAZNIK, obj,
		KOL_VNUM, bufnum,
		KOL_OPIS, buf, -1 );
	}

    for ( reset = a->reset_first; reset; reset = reset->next )
    {
	sprintf( bufnum, "%c", reset->command );
	if ( reset->command == 'M' )
	{
	    mob = get_mob_index( reset->arg1 );
	    room = get_room_index( reset->arg3 );
	    sprintf( buf, "%s -> %s", mob->short_descr, room->name );
	    gtk_tree_store_append( dkra.store_res, &mobiter, NULL );
	    additer = mobiter;
	}
	else if ( reset->command == 'O' )
	{
	    obj = get_obj_index( reset->arg1 );
	    room = get_room_index( reset->arg3 );
	    sprintf( buf, "%s -> %s", obj->short_descr, room->name );
	    gtk_tree_store_append( dkra.store_res, &objiter, NULL );
	    additer = objiter;
	}
	else if ( reset->command == 'E' || reset->command == 'G' )
	{
	    obj = get_obj_index( reset->arg1 );
	    if ( reset->command == 'G' )
	        strcpy( buf, obj->short_descr );
	    else
	        sprintf( buf, "<%s> %s", wearloc_str[ reset->arg3 ], obj->short_descr );
	    gtk_tree_store_append( dkra.store_res, &objiter, &mobiter );
	    additer = objiter;
	}
	else if ( reset->command == 'P' )
	{
	    obj = get_obj_index( reset->arg1 );
	    strcpy( buf, obj->short_descr );
	    gtk_tree_store_append( dkra.store_res, &additer, &objiter );
	}
	else if ( reset->command == 'D' )
	{
	    room = get_room_index( reset->arg1 );
	    sprintf( buf, "%s -> %s: %s",
	        room->exit[ reset->arg2 ]->short_descr,
	        room->exit[ reset->arg2 ]->to_room->name,
	        stany_drzwi[ reset->arg3 ] );
	    gtk_tree_store_append( dkra.store_res, &additer, NULL );
	}
	else
	    continue;
	gtk_tree_store_set( dkra.store_res, &additer,
	    KOL_WSKAZNIK, reset,
	    KOL_VNUM, bufnum,
	    KOL_OPIS, _( buf ), -1 );
    }
}


FUNKCJA_DLA_GTK( dialog_krainy )
{
    char buf[ MSL ];

    area = NULL;

    /* Optymalizacja, chociaz troche na wyrost */
    g_object_ref( dkra.store_pom );
    g_object_ref( dkra.store_mob );
    g_object_ref( dkra.store_prz );
    g_object_ref( dkra.store_res );
    gtk_tree_view_set_model( GTK_TREE_VIEW( dkra.tree_pom ), NULL );
    gtk_tree_view_set_model( GTK_TREE_VIEW( dkra.tree_mob ), NULL );
    gtk_tree_view_set_model( GTK_TREE_VIEW( dkra.tree_prz ), NULL );
    gtk_tree_view_set_model( GTK_TREE_VIEW( dkra.tree_res ), NULL );

    gtk_tree_selection_selected_foreach( dlis.selection, wczytaj_zaznaczone_krainy, NULL );

    gtk_tree_view_set_model( dkra.tree_pom, GTK_TREE_MODEL( dkra.store_pom ) );
    gtk_tree_view_set_model( dkra.tree_mob, GTK_TREE_MODEL( dkra.store_mob ) );
    gtk_tree_view_set_model( dkra.tree_prz, GTK_TREE_MODEL( dkra.store_prz ) );
    gtk_tree_view_set_model( dkra.tree_res, GTK_TREE_MODEL( dkra.store_res ) );
    gtk_tree_view_set_search_column( dkra.tree_pom, KOL_OPIS );
    gtk_tree_view_set_search_column( dkra.tree_mob, KOL_OPIS );
    gtk_tree_view_set_search_column( dkra.tree_prz, KOL_OPIS );
    gtk_tree_view_set_search_column( dkra.tree_res, KOL_OPIS );

    if ( !area )
	return;

    wczytaj_pola_AREA( NULL, NULL );

    sprintf( buf, "glaca - %s", area->file_name );
    gtk_window_set_title( GTK_WINDOW( dkra.okno_Krainy ), buf );

    gtk_widget_show( dkra.okno_Krainy );
    gtk_widget_hide( dlis.okno_lista_krain );

    return;
}


FUNKCJA_DLA_GTK( wczytaj_pola_AREA )
{
    ZONE_DATA *swiat;
    ZONE_LIST *kraina;
    int i = 0;

    ZROB_LANCUCH( dkra.entry_resetmsg, area->reset_msg );
    ZROB_LANCUCH( dkra.entry_fullname, area->full_name );
    ZROB_LANCUCH( dkra.entry_name, area->name );
    SPIN_SET_VALUE( dkra.spin_recall, area->recall );
    ZROB_LANCUCH( dkra.entry_wersja, area->revision );
    SPIN_SET_VALUE( dkra.spin_strefa, wiek_krainy( area ) - mud_age );
    USTAW_CHECKBOXA( dkra.check_nielista, area->flags, AREA_NOT_LISTED );
    USTAW_CHECKBOXA( dkra.check_nieteleport, area->flags, AREA_NO_TELEPORT );
    ZROB_LANCUCH_TEXT( dkra.buffer_opis, area->opis );

    for ( swiat = zone_first; swiat; swiat = swiat->next )
    {
	for ( kraina = area->strefy; kraina; kraina = kraina->next )
	    if ( kraina->zone == swiat )
		break;

	gtk_toggle_button_set_active( dkra.check_strefa[ i ],
	    kraina ? TRUE : FALSE );
	i++;
    }

    return;
}


FUNKCJA_DLA_GTK( zapisz_pola_AREA )
{
    ZONE_LIST *kraina, *del;
    ZONE_DATA *swiat;
    int i;

    WEZ_LANCUCH( dkra.entry_name );
    WEZ_LANCUCH( dkra.entry_fullname );
    WEZ_LANCUCH( dkra.entry_resetmsg );
    area->recall = SPIN_GET_VALUE( dkra.spin_recall );
    WEZ_LANCUCH( dkra.entry_wersja );
    area->time_info = time_info;
    area->time_info.hour += SPIN_GET_VALUE( dkra.spin_strefa );
    popraw_czas_krainy( area );
    USTAW_FLAGE( dkra.check_nielista, area->flags, AREA_NOT_LISTED );
    USTAW_FLAGE( dkra.check_nieteleport, area->flags, AREA_NO_TELEPORT );
    WEZ_LANCUCH_TEXT( dkra.buffer_opis );

    for ( kraina = area->strefy; kraina; )
    {
	del = kraina;
	kraina = kraina->next;
	free( del );
    }
    area->strefy = NULL;

    i = 0;
    for ( swiat = zone_first; swiat; swiat = swiat->next )
    {
	if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( dkra.check_strefa[ i ] ) ) )
	    area->strefy = dodaj_strefe( area->strefy, swiat );
	i++;
    }

    area->odnowiona = TRUE;

    return;
}


FUNKCJA_DLA_GTK( zmiana_strefy )
{
    int i;

    for ( i = 0; dkra.check_strefa[ i ]; i++ )
	if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( dkra.check_strefa[ i ] ) ) )
	    return;

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widget ), TRUE );

    return;
}
