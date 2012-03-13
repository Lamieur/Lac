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
 *  Zamiennik dla GtkComboBox stworzony z mysla o dlugich listach          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "glaca.h"
#include <gdk/gdkkeysyms.h>

struct dcom_data dcom;

void combo_wskocz( bool wybrany );

void zrob_dialog_Combo( void )
{
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    ZNAJDZ_KONTROLKE( dcom.Dialog, "oknocombo" );
    ZNAJDZ_KONTROLKE2( dcom.entry, "com_entry", GTK_ENTRY );
    ZNAJDZ_KONTROLKE( dcom.eventbox, "com_eventbox" );
    ZNAJDZ_KONTROLKE2( dcom.treeview, "com_treeview", GTK_TREE_VIEW );
    dcom.select = gtk_tree_view_get_selection( dcom.treeview );

    /* Glade nie pozwala zapisac pustych kolumn */
    renderer = gtk_cell_renderer_text_new( );
    column = gtk_tree_view_column_new( );
    gtk_tree_view_column_set_sizing( column, GTK_TREE_VIEW_COLUMN_FIXED );
    gtk_tree_view_column_pack_start( column, renderer, FALSE );
    gtk_tree_view_column_set_attributes( column, renderer,
	"text", KOL_NAZWA, NULL );
    gtk_tree_view_append_column( dcom.treeview, column );
    gtk_tree_view_set_tooltip_column( dcom.treeview, KOL_NAZWA );

    /* to dla tematow GTK, byc moze ktorys cos zmienia w takich oknach */
    gtk_widget_set_name( GTK_WIDGET( dcom.Dialog ), "gtk-combobox-popup-window" );
}


FUNKCJA_DLA_GTK( combo_dialog_show )
{
    GdkWindow *window = glaca_widget_get_window( GTK_WIDGET( dcom.Dialog ) );
    int mask = GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK;

    /* z opisu gtk_window_move( ): "most window managers ignore requests for
       initial window positions (instead using a user-defined placement
       algorithm) and honor requests after the window has already been shown."
       Z tego powodu drugi raz wywoluje dwie ponizsze funkcje: */
    gtk_window_move( GTK_WINDOW( dcom.Dialog ), dcom.wymiary.x, dcom.wymiary.y );
    gtk_widget_set_size_request( GTK_WIDGET( dcom.Dialog),
	dcom.wymiary.width, dcom.wymiary.height );

    gtk_widget_grab_focus( GTK_WIDGET( dcom.Dialog ) );

    if ( !GTK_WIDGET_HAS_FOCUS( dcom.entry ) )
	gtk_widget_grab_focus( GTK_WIDGET( dcom.entry ) );

    gtk_grab_add( GTK_WIDGET( dcom.Dialog ) );

    if ( gdk_pointer_grab( window, TRUE, mask, NULL, NULL, GDK_CURRENT_TIME )
	 == GDK_GRAB_SUCCESS )
    {
	if ( gdk_keyboard_grab( window, TRUE, GDK_CURRENT_TIME ) != GDK_GRAB_SUCCESS )
	{
	    gdk_display_pointer_ungrab( gdk_drawable_get_display( window ),
					GDK_CURRENT_TIME );
	    combo_wskocz( FALSE );
	}
    }
    else
	combo_wskocz( FALSE );
}


static gboolean standard_filter( GtkTreeModel *model, GtkTreePath *path,
				 GtkTreeIter *iter, gpointer data )
{
    char *s;
    gboolean b;

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &s, KOL_VIS, &b, -1 );

    if ( !b && *dcom.text == '\0' )
    {
	gtk_list_store_set( dcom.store, iter, KOL_VIS, TRUE, -1 );
	return FALSE;
    }

    /* TRUE == TRUE: tekst z pola nie zawiera sie w lancuchu, ale element
           jeszcze jest pokazywany - nalezy zmienic,
       FALSE == FALSE: tekst z pola zawiera sie w lancuchu, powinien zostac
           wyswietlony, ale jeszcze nie jest - nalezy zmienic
       wszystkie inne przypadki: jest dobrze, nie zmieniac widocznosci */
    if ( str_infix( dcom.text, s ) == b )
	gtk_list_store_set( dcom.store, iter, KOL_VIS, !b, -1 );

    return FALSE; /* szukaj dalej */
}


void combo_wyskocz( GtkTreeModel *filter, GtkWidget *button, GtkTreeModelForeachFunc func )
{
    gint x = 0, y = 0, monitor_num;
    GdkRectangle monitor;
    /* zmienne wylacznie dla wygody: */
    GtkWindow *top = GTK_WINDOW( gtk_widget_get_toplevel( button ) );
    GtkWindow *combo_wnd = GTK_WINDOW( dcom.Dialog );
    GdkScreen *screen = gtk_widget_get_screen( button );

    dcom.button = GTK_TOGGLE_BUTTON( button );
    dcom.wybrany = FALSE;

    /* - pozniej ustawiany jest stan wcisniecia GtkToggleButton, wiec funkcji
         nie moze wywolywac widget innego rodzaju,
       - jesli przycisk nie jest wcisniety, to dzieja sie dziwne rzeczy
       - ten warunek byl sprawdzany dla gtk_window_group_add_window()
         i gtk_window_set_transient_for(), przenosze go tutaj */
    if ( !GTK_IS_TOGGLE_BUTTON( button )
      || !gtk_toggle_button_get_active( dcom.button )
      || !GTK_IS_WINDOW( top ) )
	return;

    dcom.filter_function = ( func ) ? func : standard_filter;
    dcom.filter = filter;
    dcom.store = GTK_LIST_STORE( gtk_tree_model_filter_get_model( GTK_TREE_MODEL_FILTER( filter ) ) );

    /* niech wyskoczy na tym samym ekranie i niech podaza za buttonem w razie
       przeniesienia calego okna na inny ekran lub pulpit */
    gtk_window_group_add_window( gtk_window_get_group( top ), combo_wnd );
    gtk_window_set_transient_for( combo_wnd, top );
    gtk_window_set_screen( combo_wnd, screen );

    monitor_num = gdk_screen_get_monitor_at_window( screen, glaca_widget_get_window( button ) );
    gdk_screen_get_monitor_geometry( screen, monitor_num, &monitor );
    gdk_window_get_root_coords( glaca_widget_get_window( button ), x, y, &x, &y );
    gtk_widget_get_allocation( button, &dcom.wymiary );
    /* dcom.wymiary.width = bez zmian */
    dcom.wymiary.x += x;
    dcom.wymiary.y += y + dcom.wymiary.height;

    if ( dcom.wymiary.x < 0 )
	dcom.wymiary.x = 0;
    else if ( dcom.wymiary.x > monitor.width - dcom.wymiary.width )
	dcom.wymiary.x = monitor.width - dcom.wymiary.width;

    if ( dcom.wymiary.y + 300 > monitor.height )
	dcom.wymiary.y -= ( 300 + dcom.wymiary.height );

    dcom.wymiary.height = 300;

    if ( *gtk_entry_get_text( dcom.entry ) == '\0' )
    {
	dcom.text = str_dup( "" );
	gtk_tree_model_foreach( GTK_TREE_MODEL( dcom.store ), dcom.filter_function, NULL );
    }
    else
	gtk_entry_set_text( dcom.entry, "" );

    gtk_tree_view_set_model( dcom.treeview, filter );

    gtk_window_move( GTK_WINDOW( dcom.Dialog ), dcom.wymiary.x, dcom.wymiary.y );
    gtk_widget_set_size_request( dcom.Dialog, dcom.wymiary.width, dcom.wymiary.height );

    gtk_widget_show( dcom.Dialog );

    return;
}


void combo_wskocz( bool wybrany )
{
    GtkTreeModel *model;

    dcom.wybrany = wybrany && gtk_tree_selection_get_selected(
					dcom.select, &model, &dcom.iter );

    gtk_grab_remove( dcom.Dialog );
    gtk_widget_hide( dcom.Dialog );
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dcom.button ), FALSE );
    gtk_tree_view_set_model( dcom.treeview, NULL );

    return;
}


G_MODULE_EXPORT gboolean combo_key_press(
		GtkWidget *widget, GdkEventKey *event, gpointer data )
{
    /* pierwsze dwa przypadki to anulowanie/zatwierdzenie wyboru i w obu
       przypadkach nalezy schowac okno. Inne przyciski ignorujemy */

    if ( event->keyval == GDK_Return
      || event->keyval == GDK_ISO_Enter
      || event->keyval == GDK_KP_Enter )
    {
	combo_wskocz( TRUE );
    }
    else if ( event->keyval == GDK_Escape )
	combo_wskocz( FALSE );

    return FALSE;
}

G_MODULE_EXPORT gboolean combo_button_release(
		GtkWidget *widget, GdkEventButton *event, gpointer data )
{
    if ( !gtk_widget_is_ancestor( gtk_get_event_widget( ( GdkEvent *) event ), dcom.Dialog ) )
	combo_wskocz( FALSE );

    return TRUE;
}


G_MODULE_EXPORT gboolean treeview_button_release(
		GtkWidget *widget, GdkEventButton *event, gpointer data )
{
    GtkTreePath *path;

    if ( !gtk_tree_view_get_path_at_pos( dcom.treeview, event->x, event->y,
					 &path, NULL, NULL, NULL ) )
	return TRUE;

    gtk_tree_path_free( path );
    combo_wskocz( TRUE );

    return FALSE;
}


FUNKCJA_DLA_GTK( zmiana_tekstu_filtra )
{
    dcom.text = g_strdup( lac_z_pols( gtk_entry_get_text( dcom.entry ) ) );
    gtk_tree_view_set_model( dcom.treeview, NULL );
    gtk_tree_model_foreach( GTK_TREE_MODEL( dcom.store ), dcom.filter_function, NULL );
    gtk_tree_view_set_model( dcom.treeview, dcom.filter );
    g_free( dcom.text );

    return;
}


G_MODULE_EXPORT gboolean combo_tooltip( GtkWidget *widget, gint x, gint y,
			gboolean keyboard_mode, GtkTooltip *tooltip,
			gpointer user_data )
{
    if ( !GTK_IS_LABEL( widget ) )
	return FALSE;

    gtk_tooltip_set_text( tooltip, gtk_label_get_text( GTK_LABEL( widget ) ) );
    return TRUE;
}
