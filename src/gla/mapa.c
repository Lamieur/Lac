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
 *  Poruszanie sie po mapie i tworzenie wyjsc.                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "glaca.h"

struct dmapa_data dmapa;

struct
{
    int x;
    int y;
} prostokaty[ 11 ] =
{
    { 120, 120 },
    { 220, 220 },
    { 120, 320 },
    {  20, 220 },
    { 120,  20 },
    { 120, 420 },
    { 220, 120 },
    { 220, 320 },
    {  20, 120 },
    {  20, 320 },
    { 120, 220 } /* to aktualne pomieszczenie */
};


void maluj_mape( GtkWidget *widget )
{
    EXIT_DATA *e;
    char buf[ 16 ];
    int d;

    if ( !GDK_IS_DRAWABLE( dmapa.pixmap ) )
	return;

    gdk_draw_rectangle( dmapa.pixmap, widget->style->dark_gc[ 0 ], TRUE,
		0, 0, widget->allocation.width, widget->allocation.height );

    gdk_draw_rectangle( dmapa.pixmap, widget->style->mid_gc[ 0 ], TRUE,
		prostokaty[ 10 ].x, prostokaty[ 10 ].y, 80, 80 );
    gdk_draw_rectangle( dmapa.pixmap, widget->style->black_gc, FALSE,
		prostokaty[ 10 ].x, prostokaty[ 10 ].y, 80, 80 );

    sprintf( buf, "#%d", dmapa.room->vnum );
    pango_layout_set_text( dmapa.layout, buf, -1 );
    gdk_draw_layout( dmapa.pixmap, widget->style->black_gc,
		prostokaty[ 10 ].x + 3, prostokaty[ 10 ].y + 1, dmapa.layout );
    pango_layout_set_text( dmapa.layout, pols_z_lac_len( dmapa.room->name, 10 ), -1 );
    gdk_draw_layout( dmapa.pixmap, widget->style->black_gc,
		prostokaty[ 10 ].x + 3, prostokaty[ 10 ].y + 14, dmapa.layout );

    for ( d = 0; d < 10; d++ )
    {
	gdk_draw_rectangle( dmapa.pixmap, widget->style->white_gc, TRUE,
		prostokaty[ d ].x, prostokaty[ d ].y, 80, 80 );
	pango_layout_set_text( dmapa.layout, kierunki[ d ].skrot, -1 );
	gdk_draw_layout( dmapa.pixmap, widget->style->black_gc,
		prostokaty[ d ].x, prostokaty[ d ].y - 14, dmapa.layout );
	if ( ( e = dmapa.room->exit[ d ] ) )
	{
	    gdk_draw_rectangle( dmapa.pixmap, widget->style->white_gc, TRUE,
		prostokaty[ d ].x, prostokaty[ d ].y, 80, 80 );
	    if ( e->to_room )
	    {
		sprintf( buf, "#%d", e->to_room->vnum );
		pango_layout_set_text( dmapa.layout, buf, -1 );
		gdk_draw_layout( dmapa.pixmap, widget->style->black_gc,
			prostokaty[ d ].x + 3, prostokaty[ d ].y + 1, dmapa.layout );
		pango_layout_set_text( dmapa.layout, pols_z_lac_len( e->to_room->name, 10 ), -1 );
		gdk_draw_layout( dmapa.pixmap, widget->style->black_gc,
			prostokaty[ d ].x + 3, prostokaty[ d ].y + 14, dmapa.layout );
	    }
	    else
	    {
		pango_layout_set_text( dmapa.layout, "(brak)", -1 );
		gdk_draw_layout( dmapa.pixmap, widget->style->black_gc,
			prostokaty[ d ].x + 3, prostokaty[ d ].y + 1, dmapa.layout );
	    }
	}
	else
	    gdk_draw_rectangle( dmapa.pixmap, widget->style->dark_gc[ 0 ], TRUE,
		prostokaty[ d ].x, prostokaty[ d ].y, 80, 80 );
	gdk_draw_rectangle( dmapa.pixmap, widget->style->black_gc, FALSE,
		prostokaty[ d ].x, prostokaty[ d ].y, 80, 80 );
    }

    /* wymuszenie odswiezenia calego GtkDrawable */
    gtk_widget_queue_draw_area( widget, 0, 0, widget->allocation.width,
		widget->allocation.height );

    return;
}


static gboolean znajdz_iter( GtkTreeModel *model, GtkTreePath *path,
			     GtkTreeIter *iter, gpointer user_data )
{
    ROOM_INDEX_DATA *room = NULL;

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &room, -1 );

    if ( room == dmapa.room )
    {
	gtk_tree_selection_select_iter( dkra.selection_pom, iter );
	return TRUE; /* przestan szukac */
    }

    return FALSE; /* szukaj dalej */
}


G_MODULE_EXPORT gint button_press_event( GtkWidget *widget, GdkEventButton *event )
{
    int d;

    if ( !dmapa.pixmap
      || event->type != GDK_BUTTON_PRESS
      || ( event->button != 1
	&& event->button != 3 ) )
	return TRUE;

    if ( event->button == 1
      && event->x > prostokaty[ 10 ].x
      && event->x < prostokaty[ 10 ].x + 80
      && event->y > prostokaty[ 10 ].y
      && event->y < prostokaty[ 10 ].y + 80 )
    {
	dpom.room = dmapa.room;
	dialog_pomieszczenia( NULL, NULL );
	return TRUE;
    }

    for ( d = 0; d < 10; d++ )
    {
	if ( event->x > prostokaty[ d ].x
	  && event->x < prostokaty[ d ].x + 80
	  && event->y > prostokaty[ d ].y
	  && event->y < prostokaty[ d ].y + 80 )
	{
	    if ( event->button == 1
	      && dmapa.room->exit[ d ]
	      && dmapa.room->exit[ d ]->to_room )
	    {
		dmapa.room = dmapa.room->exit[ d ]->to_room;
		/* sluzylo do zaznaczania pomieszczenia na liscie pomieszczen,
		   jednak ja wole osobno "chodzic" po tej liscie, a osobno po
		   mapie. Tym bardziej, ze na mapie mozna przejsc do
		   pomieszczenia spoza listy. Kandydat na opcje: */
		if ( 0 )
		    gtk_tree_model_foreach( GTK_TREE_MODEL( dkra.store_pom ), znajdz_iter, NULL );
		maluj_mape( widget );
	    }
	    break;
	}
    }

    return TRUE;
}


G_MODULE_EXPORT gint configure_event( GtkWidget *widget, GdkEventConfigure *event )
{
    if ( dmapa.pixmap )
	g_object_unref( dmapa.pixmap );

    dmapa.pixmap = gdk_pixmap_new( widget->window,
				widget->allocation.width,
				widget->allocation.height, -1 );

    maluj_mape( dmapa.drawingarea );
    return TRUE;
}


G_MODULE_EXPORT gint expose_event( GtkWidget *widget, GdkEventExpose *event )
{
    gdk_draw_drawable( widget->window,
			widget->style->fg_gc[ GTK_WIDGET_STATE( widget ) ],
			dmapa.pixmap,
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height );

    return FALSE;
}


void zrob_okno_Mapy( void )
{
    ZNAJDZ_KONTROLKE( dmapa.Mapa, "oknomapa" );
    ZNAJDZ_KONTROLKE( dmapa.drawingarea, "map_drawingarea" );

    dmapa.layout = gtk_widget_create_pango_layout( dmapa.drawingarea, "" );
    pango_layout_set_font_description( dmapa.layout, font_desc );

    return;
}


FUNKCJA_DLA_GTK( dialog_mapy )
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    if ( !gtk_tree_selection_get_selected( dkra.selection_pom, &model, &iter ) )
	return;
    gtk_tree_model_get( model, &iter, KOL_WSKAZNIK, &dmapa.room, -1 );

    if ( !dmapa.room )
	return;

    gtk_grab_add( dmapa.Mapa );

    /* najpierw pokazuje, potem maluje, zeby sie configure_event odpalil */
    gtk_widget_show( dmapa.Mapa );
    maluj_mape( dmapa.drawingarea );

    return;
}


FUNKCJA_DLA_GTK_2( smierc_dialogu_mapy )
{
    gtk_widget_hide( dmapa.Mapa );
    gtk_grab_remove( dmapa.Mapa );

    return TRUE;
}

