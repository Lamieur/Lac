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
#include "../mp_wczyt.h"


struct dprog_data dprog;

#define KOL_PANGO 2
#define KOL_USUN 3
#define KOL_GRUBOSC 4


void zrob_okno_Progow( void )
{
    PangoFontDescription *pango_font_desc;
    GtkWidget *tree;

    ZNAJDZ_KONTROLKE( dprog.Prog, "oknoprogi" );
    ZNAJDZ_KONTROLKE( dprog.combobox, "prg_combobox" );
    ZNAJDZ_KONTROLKE( dprog.entry, "prg_entry" );
    ZNAJDZ_KONTROLKE( dprog.textview, "prg_textview" );
    ZNAJDZ_KONTROLKE( dprog.button_dodaj, "prg_button_dodaj" );
    ZNAJDZ_KONTROLKE( dprog.button_usun, "prg_button_usun" );
    ZNAJDZ_KONTROLKE( dprog.button_przywroc, "prg_button_przywroc" );
    ZNAJDZ_KONTROLKE( dprog.button_gotowe, "prg_button_gotowe" );
    ZNAJDZ_KONTROLKE2( dprog.treestore, "prg_treestore_tree", GTK_TREE_STORE );
    ZNAJDZ_KONTROLKE2( dprog.buffer, "prg_buffer", GTK_TEXT_BUFFER );
    ZNAJDZ_KONTROLKE( dprog.label_kur, "prg_label_kur" );
    ZNAJDZ_KONTROLKE( tree, "prg_treeview" );
    dprog.select = gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );

    /* wydaje mi sie, ze jak nie podam wielkosci, to on powinien to sobie
       wziac z globalnych ustawien tematu */
    pango_font_desc = pango_font_description_from_string( "Monospace" );
    gtk_widget_modify_font( dprog.textview, pango_font_desc );
    pango_font_description_free( pango_font_desc );

    return;
}


void wypelnij_opisy( void )
{
    GtkTreeIter iter;
    EXTRA_DESCR_DATA *ed, *kopia;

    gtk_widget_hide( dprog.combobox );

    for ( ed = dprog.opisy; ed; ed = ed->next )
    {
	kopia = new_extra_descr( );
	kopia->next = ed;
	kopia->keyword = str_dup( ed->keyword );
	kopia->description = str_dup( ed->description );
	kopia->deleted = FALSE;

	gtk_tree_store_append( dprog.treestore, &iter, NULL );
	gtk_tree_store_set( dprog.treestore, &iter,
	    KOL_WSKAZNIK, kopia,
	    KOL_NAZWA, _( kopia->keyword ),
	    KOL_PANGO, PANGO_STYLE_NORMAL,
	    KOL_USUN, FALSE,
	    KOL_GRUBOSC, PANGO_WEIGHT_NORMAL, -1 );
    }

    return;
}


void wypelnij_progi( void )
{
    char buf[ 2 * MIL ];
    MPROG_DATA *prog, *kopia, *parent = NULL;
    GtkTreeIter iter, witer;

    gtk_widget_show( dprog.combobox );

    for ( prog = dprog.progi; prog; prog = prog->next )
    {
	sprintf( buf, "%s %s", mprog_type_to_name( prog->type, dprog.gdzie ),
		 _( prog->arglist ) );

	/* ponizsze jest takie samo dla wszystkich kopii */
	kopia = new_mprog( );
	kopia->next = prog; /* to do odzysku, lista robiona jest gdzie indziej */
	kopia->type = prog->type;

	if ( prog->parent )
	{
	    kopia->parent = parent; /* ostatnia kopia jest rodzicem */
	    kopia->arglist = prog->arglist; /* nie ma sensu kopiowac, */
	    kopia->comlist = prog->comlist; /* wystarczy nie zwalniac */

	    /* niewazne, ze zagniezdzanie in_file_progow bedzie sie pokazywalo
	       jako dzieci jednego glownego in_file_proga, i tak nie wolno ich
	       edytowac, wiec to pryszcz, zreszta w krainach tego nikt
	       nie uzywa */
	    gtk_tree_store_append( dprog.treestore, &witer, &iter );
	    gtk_tree_store_set( dprog.treestore, &witer,
		KOL_NAZWA, buf,
		KOL_WSKAZNIK, kopia,
		KOL_PANGO, PANGO_STYLE_NORMAL,
		KOL_USUN, FALSE,
		KOL_GRUBOSC, PANGO_WEIGHT_NORMAL, -1 );
	}
	else
	{
	    parent = kopia; /* ta kopia byc moze jest rodzicem */
	    kopia->parent = NULL;
	    kopia->arglist = str_dup( prog->arglist );
	    kopia->comlist = str_dup( prog->comlist );

	    gtk_tree_store_append( dprog.treestore, &iter, NULL );
	    gtk_tree_store_set( dprog.treestore, &iter,
		KOL_NAZWA, buf,
		KOL_WSKAZNIK, kopia,
		KOL_PANGO, PANGO_STYLE_NORMAL,
		KOL_USUN, FALSE,
		KOL_GRUBOSC, PANGO_WEIGHT_NORMAL, -1 );
	}
    }

    gtk_combo_box_set_model( GTK_COMBO_BOX( dprog.combobox ),
			     GTK_TREE_MODEL( dprog.store_progi[ dprog.gdzie ] ) );
    COMBO_SET_ACTIVE( dprog.combobox, -1 );

    return;
}


FUNKCJA_DLA_GTK( dialog_progow )
{
    char buf[ 2 * MIL ];

    gtk_grab_add( dprog.Prog );

    gtk_widget_set_sensitive( dprog.combobox, FALSE );
    gtk_entry_set_editable( GTK_ENTRY( dprog.entry ), FALSE );
    gtk_text_view_set_editable( GTK_TEXT_VIEW( dprog.textview ), FALSE );

    gtk_tree_store_clear( dprog.treestore );
    PUSC_LANCUCH( dprog.entry );
    PUSC_LANCUCH_TEXT( dprog.buffer );

    if ( widget == dmob.button_progi )
    {
	sprintf( buf, "Progi moba #%d (%s)", dmob.mob->vnum,
		dmob.mob->short_descr );
	dprog.progi = dmob.mob->mobprogs;
	dprog.gdzie = 0;

	wypelnij_progi( );
    }
    else if ( widget == dpom.button_progi )
    {
	sprintf( buf, "Progi pomieszczenia #%d (%s)", dpom.room->vnum,
		dpom.room->name );
	dprog.progi = dpom.room->mobprogs;
	dprog.gdzie = 1;

	wypelnij_progi( );
    }
    else if ( widget == dwyj.button_progi )
    {
	if ( dwyj.exit->to_room )
	    sprintf( buf, "Progi wyj`scia #%d (%s)", dwyj.exit->to_room->vnum,
		dwyj.exit->to_room->name );
	else
	    sprintf( buf, "Progi nowego wyj`scia" );
	dprog.progi = dwyj.exit->mobprogs;
	dprog.gdzie = 2;

	wypelnij_progi( );
    }
    else if ( widget == dprz.button_progi )
    {
	sprintf( buf, "Progi przedmiotu #%d (%s)", dprz.obj->vnum,
		dprz.obj->short_descr );
	dprog.progi = dprz.obj->mobprogs;
	dprog.gdzie = 3;

	wypelnij_progi( );
    }
    else if ( widget == dpom.button_opisy )
    {
	sprintf( buf, "Dodatkowe opisy pomieszczenia #%d (%s)", dpom.room->vnum,
		dpom.room->name );
	dprog.opisy = dpom.room->extra_descr;
	dprog.gdzie = 4;

	wypelnij_opisy( );
    }

    gtk_window_set_title( GTK_WINDOW( dprog.Prog ), _( buf ) );

    gtk_widget_show( dprog.Prog );

    return;
}


static gboolean zwolnij_kopie_opisow( GtkTreeModel *model, GtkTreePath *path,
				      GtkTreeIter *iter, gpointer user_data )
{
    EXTRA_DESCR_DATA *ed;

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &ed, -1 );
    del_extra_descr( ed );

    return FALSE;
}


static gboolean nowa_lista_z_kopii_opisow( GtkTreeModel *model, GtkTreePath *path,
					   GtkTreeIter *iter, gpointer user_data )
{
    EXTRA_DESCR_DATA *ed;
    static EXTRA_DESCR_DATA *last;
    gboolean usun;

    gtk_tree_model_get( model, iter,
	KOL_WSKAZNIK, &ed,
	KOL_USUN, &usun, -1 );

    if ( usun )
    {
	del_extra_descr( ed );
	return FALSE;
    }

    if ( !dprog.opisy )
    {
	dprog.opisy = last = ed;
	last->next = NULL;
	return FALSE;
    }

    last->next = ed;
    last = ed;
    last->next = NULL;

    return FALSE;
}


static gboolean zwolnij_kopie_progow( GtkTreeModel *model, GtkTreePath *path,
				      GtkTreeIter *iter, gpointer user_data )
{
    MPROG_DATA *prog;

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &prog, -1 );

    /* te progi to kopie, ale ich arglist i comlist wskazuja na oryginaly,
       dlatego uchronmy je przed del_mprog( ) */
    if ( prog->parent )
	prog->comlist = prog->arglist = NULL;

    del_mprog( prog );

    return FALSE;
}


static gboolean nowa_lista_z_kopii_progow( GtkTreeModel *model, GtkTreePath *path,
					   GtkTreeIter *iter, gpointer user_data )
{
    MPROG_DATA *prog;
    static MPROG_DATA *last;
    gboolean usun;

    gtk_tree_model_get( model, iter,
	KOL_WSKAZNIK, &prog,
	KOL_USUN, &usun, -1 );

    if ( usun )
    {
	if ( gtk_tree_model_iter_has_child( model, iter ) )
	{
	    if ( dprog.del_ancestor )
		gtk_tree_path_free( dprog.del_ancestor );
	    dprog.del_ancestor = gtk_tree_path_copy( path );
	}

	del_mprog( prog );
	return FALSE;
    }

    if ( dprog.del_ancestor && gtk_tree_path_is_descendant( path, dprog.del_ancestor ) )
    {
	del_mprog( prog );
	return FALSE;
    }

    if ( !dprog.progi )
    {
	dprog.progi = last = prog;
	last->next = NULL;
	return FALSE;
    }

    last->next = prog;
    last = prog;
    last->next = NULL;

    return FALSE;
}


FUNKCJA_DLA_GTK( zatwierdzenie_zmian_progow )
{
    gtk_widget_hide( dprog.Prog );

    WEZ_LANCUCH( dprog.entry );
    WEZ_LANCUCH_TEXT( dprog.buffer );

    if ( dprog.gdzie < 4 )
    {
	MPROG_DATA *prog, *del;

	for ( prog = dprog.progi; prog;  )
	{
	    del = prog;
	    prog = prog->next;

	    if ( del->parent )
		del->comlist = del->arglist = NULL;

	    del_mprog( del );
	}
	dprog.progi = NULL;
	gtk_tree_model_foreach( GTK_TREE_MODEL( dprog.treestore ), nowa_lista_z_kopii_progow, NULL );
	if ( dprog.del_ancestor )
	{
	    gtk_tree_path_free( dprog.del_ancestor );
	    dprog.del_ancestor = NULL;
	}
    }
    else if ( dprog.gdzie == 4 )
    {
	EXTRA_DESCR_DATA *ed, *del;

	for ( ed = dprog.opisy; ed;  )
	{
	    del = ed;
	    ed = ed->next;
	    del_extra_descr( del );
	}
	dprog.opisy = NULL;
	gtk_tree_model_foreach( GTK_TREE_MODEL( dprog.treestore ), nowa_lista_z_kopii_opisow, NULL );
    }

    switch ( dprog.gdzie )
    {
    case 0:
	dmob.mob->mobprogs = dprog.progi;
	dmob.mob->index_area->odnowiona = TRUE;
	ile_progow_button( dmob.button_progi, dmob.mob->mobprogs );
	break;
    case 1:
	dpom.room->mobprogs = dprog.progi;
	dpom.room->area->odnowiona = TRUE;
	ile_progow_button( dpom.button_progi, dpom.room->mobprogs );
	break;
    case 2:
	dwyj.exit->mobprogs = dprog.progi;
	dwyj.room->area->odnowiona = TRUE;
	ile_progow_button( dwyj.button_progi, dwyj.exit->mobprogs );
	break;
    case 3:
	dprz.obj->mobprogs = dprog.progi;
	dprz.obj->area->odnowiona = TRUE;
	ile_progow_button( dprz.button_progi, dprz.obj->mobprogs );
	break;
    case 4:
	dpom.room->extra_descr = dprog.opisy;
	dpom.room->area->odnowiona = TRUE;
	ile_opisow_label( dpom.label_opisy, dpom.room->extra_descr );
	break;
    }

    gtk_grab_remove( dprog.Prog );

    return;
}


FUNKCJA_DLA_GTK_2( smierc_dialogu_progow )
{
    gtk_widget_hide( dprog.Prog );

    if ( dprog.gdzie < 4 )
	gtk_tree_model_foreach( GTK_TREE_MODEL( dprog.treestore ), zwolnij_kopie_progow, NULL );
    else if ( dprog.gdzie == 4 )
	gtk_tree_model_foreach( GTK_TREE_MODEL( dprog.treestore ), zwolnij_kopie_opisow, NULL );

    gtk_grab_remove( dprog.Prog );

    return TRUE;
}


FUNKCJA_DLA_GTK( przywrocenie_proga )
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gpointer *ptr;
    gboolean usun;
    PangoStyle style;
    gint weight;

    if ( !gtk_tree_selection_get_selected( dprog.select, &model, &iter ) )
	return;

    gtk_tree_model_get( model, &iter,
	KOL_WSKAZNIK, &ptr,
	KOL_USUN, &usun,
	KOL_PANGO, &style,
	KOL_GRUBOSC, &weight, -1 );

    if ( !usun && style == PANGO_STYLE_NORMAL )
	return;

    if ( style != PANGO_STYLE_NORMAL && weight != PANGO_WEIGHT_BOLD )
    {
	if ( dprog.gdzie < 4 )
	{
	    MPROG_DATA *prog = ( MPROG_DATA *) ptr;
	    int i;

	    if ( prog->parent )
		return;

	    free_string( prog->arglist );
	    free_string( prog->comlist );
	    prog->arglist = str_dup( prog->next->arglist );
	    prog->comlist = str_dup( prog->next->comlist );
	    prog->type = prog->next->type;

	    ZROB_LANCUCH( dprog.entry, prog->arglist );
	    ZROB_LANCUCH_TEXT( dprog.buffer, prog->comlist );

	    for ( i = 0; *progi[ dprog.gdzie ][ i ].name; i++ )
		if ( progi[ dprog.gdzie ][ i ].trig == prog->type )
		    COMBO_SET_ACTIVE( dprog.combobox, i );
	}
	else if ( dprog.gdzie == 4 )
	{
	    EXTRA_DESCR_DATA *ed = ( EXTRA_DESCR_DATA *) ptr;

	    free_string( ed->keyword );
	    free_string( ed->description );
	    ed->keyword = str_dup( ed->next->keyword );
	    ed->description = str_dup( ed->next->description );
	    ZROB_LANCUCH( dprog.entry, ed->keyword );
	    ZROB_LANCUCH_TEXT( dprog.buffer, ed->description );
	}
	else
	    return;
    }

    if ( weight != PANGO_WEIGHT_BOLD )
	style = PANGO_STYLE_NORMAL;
    else
	style = PANGO_STYLE_ITALIC;

    gtk_tree_store_set( dprog.treestore, &iter,
	KOL_USUN, FALSE,
	KOL_PANGO, style, -1 );

    return;
}


FUNKCJA_DLA_GTK( usuniecie_proga )
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    WEZ_LANCUCH( dprog.entry );
    WEZ_LANCUCH_TEXT( dprog.buffer );

    if ( !gtk_tree_selection_get_selected( dprog.select, &model, &iter ) )
	return;

    gtk_tree_store_set( dprog.treestore, &iter, KOL_USUN, TRUE, -1 );

    return;
}


FUNKCJA_DLA_GTK( dodanie_proga )
{
    gpointer *nowy;
    GtkTreeIter iter;
    char buf[ 2 * MIL ];

    if ( dprog.gdzie < 4 )
    {
	MPROG_DATA *prog;

	prog = new_mprog( );
	prog->next = NULL; /* nie ma skad przywrocic */
	prog->type = progi[ dprog.gdzie ][ 1 ].trig;
	prog->parent = NULL;
	prog->arglist = str_dup( "" );
	prog->comlist = str_dup( "" );
	sprintf( buf, "%s %s", mprog_type_to_name( prog->type, dprog.gdzie ),
	    _( prog->arglist ) );
	nowy = ( gpointer *) prog;
    }
    else if ( dprog.gdzie == 4 )
    {
	EXTRA_DESCR_DATA *ed;

	ed = new_extra_descr( );
	ed->next = NULL;
	ed->keyword = str_dup( "nowy opis" );
	ed->description = str_dup( "" );
	ed->deleted = FALSE;
	strcpy( buf, _( ed->keyword ) );
	nowy = ( gpointer *) ed;
    }
    else
	return;

    gtk_tree_store_append( dprog.treestore, &iter, NULL );
    gtk_tree_store_set( dprog.treestore, &iter,
	KOL_NAZWA, buf,
	KOL_WSKAZNIK, nowy,
	KOL_PANGO, PANGO_STYLE_ITALIC,
	KOL_USUN, FALSE,
	KOL_GRUBOSC, PANGO_WEIGHT_BOLD, -1 );

    return;
}


FUNKCJA_DLA_GTK( zmiana_zaznaczonego_proga )
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gboolean sensitive = TRUE, in_file = FALSE;
    gpointer ptr;
    int i;

    WEZ_LANCUCH( dprog.entry );
    WEZ_LANCUCH_TEXT( dprog.buffer );

    gtk_label_set_text( GTK_LABEL( dprog.label_kur ), "" );

    if ( gtk_tree_selection_get_selected( dprog.select, &model, &iter ) )
    {
	gtk_tree_model_get( model, &iter, KOL_WSKAZNIK, &ptr, -1 );

	dprog.nie_zmieniaj = TRUE;
	if ( dprog.gdzie < 4 ) /* m/r/e/o-progi */
	{
	    MPROG_DATA *prog = ( MPROG_DATA *) ptr;

	    ZROB_LANCUCH( dprog.entry, prog->arglist );
	    ZROB_LANCUCH_TEXT( dprog.buffer, prog->comlist );

	    for ( i = 0; *progi[ dprog.gdzie ][ i ].name; i++ )
		if ( progi[ dprog.gdzie ][ i ].trig == prog->type )
		    COMBO_SET_ACTIVE( dprog.combobox, i );

	    sensitive = ( prog->parent == NULL );
	    in_file = ( prog->type == IN_FILE_PROG );
	}
	else if ( dprog.gdzie == 4 ) /* dodatkowe opisy pomieszczenia */
	{
	    EXTRA_DESCR_DATA *ed = ( EXTRA_DESCR_DATA *) ptr;

	    ZROB_LANCUCH( dprog.entry, ed->keyword );
	    ZROB_LANCUCH_TEXT( dprog.buffer, ed->description );
	}
	dprog.nie_zmieniaj = FALSE;
    }
    else
    {
	sensitive = FALSE;
	PUSC_LANCUCH( dprog.entry );
	PUSC_LANCUCH_TEXT( dprog.buffer );
    }

    gtk_entry_set_editable( GTK_ENTRY( dprog.entry ), sensitive );
    gtk_widget_set_sensitive( dprog.combobox, sensitive );
    gtk_widget_set_sensitive( dprog.button_usun, sensitive );
    gtk_widget_set_sensitive( dprog.button_przywroc, sensitive );
    gtk_text_view_set_editable( GTK_TEXT_VIEW( dprog.textview ), sensitive && !in_file );

    return;
}


FUNKCJA_DLA_GTK( zmiana_prog_entry )
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    char buf[ 2 * MIL ];

    if ( dprog.nie_zmieniaj
      || !gtk_tree_selection_get_selected( dprog.select, &model, &iter ) )
	return;

    if ( dprog.gdzie < 4 )
    {
	MPROG_DATA *prog;

	gtk_tree_model_get( model, &iter, KOL_WSKAZNIK, &prog, -1 );
	sprintf( buf, "%s %s", mprog_type_to_name( prog->type, dprog.gdzie ),
			ENTRY_GET_TEXT( dprog.entry ) );
    }
    else if ( dprog.gdzie == 4 )
	strcpy( buf, ENTRY_GET_TEXT( dprog.entry ) );
    else
	return;
    gtk_tree_store_set( dprog.treestore, &iter,
	KOL_NAZWA, buf,
	KOL_PANGO, PANGO_STYLE_ITALIC,
	KOL_USUN, FALSE, -1 );

    return;
}


FUNKCJA_DLA_GTK( zmiana_prog_combo )
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    MPROG_DATA *prog;
    char buf[ 2 * MIL ];
    int typ;

    if ( dprog.gdzie > 3
      || dprog.nie_zmieniaj
      || !gtk_tree_selection_get_selected( dprog.select, &model, &iter ) )
	return;

    gtk_tree_model_get( model, &iter, KOL_WSKAZNIK, &prog, -1 );
    typ = prog->type;
    prog->type = progi[ dprog.gdzie ][ COMBO_GET_ACTIVE( dprog.combobox ) ].trig;

    if ( typ == prog->type )
	return;

    sprintf( buf, "%s %s", mprog_type_to_name( prog->type, dprog.gdzie ),
	ENTRY_GET_TEXT( dprog.entry ) );
    gtk_tree_store_set( dprog.treestore, &iter,
	KOL_NAZWA, buf,
	KOL_PANGO, PANGO_STYLE_ITALIC,
	KOL_USUN, FALSE, -1 );

    /* na wypadek, jakbym zmienil na IN_FILE_PROG, blokuje wtedy wpisywanie,
       ale tekst w okienku zostaje - w koncu ktos mogl sie pomylic */
    gtk_text_view_set_editable( GTK_TEXT_VIEW( dprog.textview ),
	prog->parent || prog->type == IN_FILE_PROG ? FALSE : TRUE );

    return;
}


G_MODULE_EXPORT void progi_ruszanie_kursora( GtkTextBuffer *textbuffer,
			GtkTextIter *arg1, GtkTextMark *arg2,
			gpointer user_data )
{
    const char *nazwa;
    char buf[ MIL ];

    nazwa = gtk_text_mark_get_name( arg2 );
    if ( !nazwa || strcmp( nazwa, "insert" ) )
	return;

    sprintf( buf, "Linia %d z %d, znak %d",
	gtk_text_iter_get_line( arg1 ) + 1,
	gtk_text_buffer_get_line_count( textbuffer ),
	gtk_text_iter_get_line_offset( arg1 ) + 1 );

    gtk_label_set_text( GTK_LABEL( dprog.label_kur ), _( buf ) );

    return;
}


FUNKCJA_DLA_GTK( pisanie_po_progu )
{
    GtkTextMark *mark;
    GtkTextIter iter;
    char buf[ MIL ];

    mark = gtk_text_buffer_get_insert( dprog.buffer );
    gtk_text_buffer_get_iter_at_mark( dprog.buffer, &iter, mark );

    sprintf( buf, "Linia %d z %d, znak %d",
	gtk_text_iter_get_line( &iter ) + 1,
	gtk_text_buffer_get_line_count( dprog.buffer ),
	gtk_text_iter_get_line_offset( &iter ) + 1 );

    gtk_label_set_text( GTK_LABEL( dprog.label_kur ), _( buf ) );

    return;
}


G_MODULE_EXPORT gboolean progi_zmiana_skupienia_z( GtkWidget *widget,
			GdkEventFocus *arg1, gpointer user_data )
{
    gtk_label_set_text( GTK_LABEL( dprog.label_kur ), "" );

    return FALSE;
}

