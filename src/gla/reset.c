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

struct dres_data dres;

FUNKCJA_DLA_GTK( combo_res_ekw );
FUNKCJA_DLA_GTK( spin_res_ekw );
FUNKCJA_DLA_GTK( spin_res_mob );
FUNKCJA_DLA_GTK( spin_res_prz );
FUNKCJA_DLA_GTK( zatwierdzenie_zmian_resetu );
FUNKCJA_DLA_GTK_2( smierc_dialogu_resetu );

void zrob_okno_Resetu( void )
{
    GtkWidget *tree;
    int i;
    char bufor[ MIL ];

    ZNAJDZ_KONTROLKE( dres.Reset, "oknoreset" );
    ZNAJDZ_KONTROLKE( dres.spin_mob, "res_spin_mob" );
    ZNAJDZ_KONTROLKE( dres.spin_pom, "res_spin_pom" );
    ZNAJDZ_KONTROLKE( dres.spin_prz, "res_spin_prz" );
    ZNAJDZ_KONTROLKE2( dres.label_mob, "res_label_mob", GTK_LABEL );
    ZNAJDZ_KONTROLKE2( dres.label_pom, "res_label_pom", GTK_LABEL );
    ZNAJDZ_KONTROLKE2( dres.label_prz, "res_label_prz", GTK_LABEL );
    ZNAJDZ_KONTROLKE( dres.spin_int, "res_spin_int" );
    ZNAJDZ_KONTROLKE2( dres.label_int, "res_label_int", GTK_LABEL );
    ZNAJDZ_KONTROLKE( dres.button_poprz, "res_button_poprz" );
    ZNAJDZ_KONTROLKE( dres.button_nast, "res_button_nast" );
    ZNAJDZ_KONTROLKE( dres.hbox_mob, "res_hbox_mob" );
    ZNAJDZ_KONTROLKE( dres.hbox_prz, "res_hbox_prz" );
    ZNAJDZ_KONTROLKE( dres.frame_ekw, "res_frame_ekw" );
    ZNAJDZ_KONTROLKE( dres.button_poprz, "res_button_poprz" );
    ZNAJDZ_KONTROLKE( dres.button_nast, "res_button_nast" );

    ZNAJDZ_KONTROLKE( dres.frame_inw, "res_frame_inw" );
    ZNAJDZ_KONTROLKE2( dres.label_inw, "res_label_inw", GTK_LABEL );
    ZNAJDZ_KONTROLKE( dres.button_inw_usun, "res_button_inw_usun" );
    ZNAJDZ_KONTROLKE2( dres.store_inw, "res_treestore_inw", GTK_TREE_STORE );
    ZNAJDZ_KONTROLKE2( dres.treeview_inw, "res_treeview_inw", GTK_TREE_VIEW );
    ZNAJDZ_KONTROLKE( tree, "res_treeview_inw" );
    dres.select_inw = gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );
    ZNAJDZ_KONTROLKE( dres.spin_inw_dodaj, "res_spin_inw_dodaj" );
    ZNAJDZ_KONTROLKE( dres.toggle_inw_dodaj, "res_toggle_inw_dodaj" );
    ZNAJDZ_KONTROLKE2( dres.label_inw_dodaj, "res_label_inw_dodaj", GTK_LABEL );
    ZNAJDZ_KONTROLKE( dres.button_inw_dodaj, "res_button_inw_dodaj" );
    gtk_adjustment_configure(
	gtk_spin_button_get_adjustment( GTK_SPIN_BUTTON( dres.spin_inw_dodaj ) ),
	1, 1, MAX_VNUM, 1, 10, 0 );

    dres.wear_na_item[ WEAR_LIGHT    ] = ITEM_TAKE | 0;
    dres.wear_na_item[ WEAR_FINGER_L ] = ITEM_TAKE | ITEM_WEAR_FINGER;
    dres.wear_na_item[ WEAR_FINGER_R ] = ITEM_TAKE | ITEM_WEAR_FINGER;
    dres.wear_na_item[ WEAR_NECK_1   ] = ITEM_TAKE | ITEM_WEAR_NECK;
    dres.wear_na_item[ WEAR_NECK_2   ] = ITEM_TAKE | ITEM_WEAR_NECK;
    dres.wear_na_item[ WEAR_BODY     ] = ITEM_TAKE | ITEM_WEAR_BODY;
    dres.wear_na_item[ WEAR_HEAD     ] = ITEM_TAKE | ITEM_WEAR_HEAD;
    dres.wear_na_item[ WEAR_LEGS     ] = ITEM_TAKE | ITEM_WEAR_LEGS;
    dres.wear_na_item[ WEAR_FEET     ] = ITEM_TAKE | ITEM_WEAR_FEET;
    dres.wear_na_item[ WEAR_HANDS    ] = ITEM_TAKE | ITEM_WEAR_HANDS;
    dres.wear_na_item[ WEAR_ARMS     ] = ITEM_TAKE | ITEM_WEAR_ARMS;
    dres.wear_na_item[ WEAR_SHIELD   ] = ITEM_TAKE | ITEM_WEAR_SHIELD;
    dres.wear_na_item[ WEAR_ABOUT    ] = ITEM_TAKE | ITEM_WEAR_ABOUT;
    dres.wear_na_item[ WEAR_WAIST    ] = ITEM_TAKE | ITEM_WEAR_WAIST;
    dres.wear_na_item[ WEAR_WRIST_L  ] = ITEM_TAKE | ITEM_WEAR_WRIST;
    dres.wear_na_item[ WEAR_WRIST_R  ] = ITEM_TAKE | ITEM_WEAR_WRIST;
    dres.wear_na_item[ WEAR_WIELD    ] = ITEM_TAKE | ITEM_WIELD;
    dres.wear_na_item[ WEAR_HOLD     ] = ITEM_TAKE | ITEM_HOLD;
    dres.wear_na_item[ WEAR_WIELD_2  ] = ITEM_TAKE | ITEM_WIELD;
    dres.wear_na_item[ WEAR_FACE     ] = ITEM_TAKE | ITEM_WEAR_FACE;

    for ( i = 0; i < MAX_WEAR; i++ )
    {
	GtkLabel *label;

	sprintf( bufor, "res_label_ekw%d", i );
	ZNAJDZ_KONTROLKE2( dres.label_ekw[ i ], bufor, GTK_LABEL );

	sprintf( bufor, "res_spin_ekw%d", i );
	ZNAJDZ_KONTROLKE( dres.spin_ekw[ i ], bufor );

	gtk_adjustment_configure(
	    gtk_spin_button_get_adjustment( GTK_SPIN_BUTTON( dres.spin_ekw[ i ] ) ),
	    -1, -1, MAX_VNUM, 1, 10, 0 );
	/* powyzsze 3 koniecznie w takiej kolejnosci */

	sprintf( bufor, "res_label_miejsce%d", i );
	ZNAJDZ_KONTROLKE2( label, bufor, GTK_LABEL );
	gtk_label_set_text( label, _( wearloc_str[ i ] ) );

	sprintf( bufor, "res_toggle_ekw%d", i );
	ZNAJDZ_KONTROLKE( dres.toggle_ekw[ i ], bufor );
	g_signal_connect( G_OBJECT( dres.toggle_ekw[ i ] ), "toggled",
	    G_CALLBACK( combo_res_ekw ), NULL );
    }

    return;
}


FUNKCJA_DLA_GTK( poprznast_reset )
{
    GtkTreePath *path;
    GtkTreeModel *model;

    if ( !gtk_tree_selection_get_selected( dkra.selection_res, &model, &dres.iter ) )
	return;

    path = gtk_tree_model_get_path( model, &dres.iter );

    zatwierdzenie_zmian_resetu( NULL, GINT_TO_POINTER( 1 ) );

    if ( widget == dres.button_poprz )
	gtk_tree_path_prev( path );
    else if ( widget == dres.button_nast )
	gtk_tree_path_next( path );

    gtk_tree_selection_select_path( dkra.selection_res, path );
    gtk_tree_path_free( path );
    reset_z_listy( NULL, NULL );

    return;
}


static gboolean reset_filter( GtkTreeModel *model, GtkTreePath *path,
				 GtkTreeIter *iter, gpointer data )
{
    char *s;
    gboolean b;
    OBJ_INDEX_DATA *o;

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &s, KOL_VIS, &b, KOL_ELEMENT, &o, -1 );

    if ( dres.wear_na_item[ dres.miejsce ] != o->wear_flags
      || ( dres.miejsce == WEAR_LIGHT && o->item_type != ITEM_LIGHT ) )
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


FUNKCJA_DLA_GTK( combo_res_ekw )
{
    for ( dres.miejsce = 0; ; dres.miejsce++ )
	if ( dres.miejsce == MAX_WEAR )
	    return;
	else if ( widget == dres.toggle_ekw[ dres.miejsce ] )
	    break;

    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filter_prz, widget, reset_filter );
    else if ( dcom.wybrany )
    {
	OBJ_INDEX_DATA *obj;

	gtk_tree_model_get( filter_prz, &dcom.iter, KOL_ELEMENT, &obj, -1 );
	SPIN_SET_VALUE( dres.spin_ekw[ dres.miejsce ], obj->vnum );
    }

    return;
}


FUNKCJA_DLA_GTK( spin_res_ekw )
{
    OBJ_INDEX_DATA *obj = NULL;
    int vnum;

    for ( dres.miejsce = 0; ; dres.miejsce++ )
	if ( dres.miejsce == MAX_WEAR )
	    return;
	else if ( widget == dres.spin_ekw[ dres.miejsce ] )
	    break;

    if ( ( vnum = SPIN_GET_VALUE( widget ) ) == -1 )
	gtk_label_set( dres.label_ekw[ dres.miejsce ], "(brak)" );
    else if ( !( obj = get_obj_index( vnum ) ) )
	gtk_label_set( dres.label_ekw[ dres.miejsce ], "przedmiot nie istnieje" );
    else
	gtk_label_set( dres.label_ekw[ dres.miejsce ], _( obj->short_descr ) );


    /* Odnosnie: dres.iter_ekw[ dres.miejsce ].stamp
       Dokumentacja wyjatkowo pozwala na odczyt/zapis bezposrednio ze struktury
       GtkTreeIter, w przeciwienstwie do wiekszosci struktur, w tym pokrewnych
       dla niej GtkTreePath i GtkTreeRowReference. Na dodatek pisze:
       "some models guarantee that an iterator is valid for as long as the node
       it refers to is valid (most notably the GtkTreeStore and GtkListStore)" */

    /*
     * Aktualizacja zakladanych pojemnikow w inwentarzu. Dzialanie:
     *
     * nowy nie jest pojemnikiem:
     * * stary jest pojemnikiem: usun go z listy
     * * stary nie jest pojemnikiem: niczego nie zmieniaj na liscie
     *
     * nowy jest pojemnikiem:
     * * stary jest pojemnikiem: znajdz stary na liscie i zmien nazwe na nowa
     * * stary nie jest pojemnikiem: dodaj nowy do listy
     */

    if ( !obj || obj->item_type != ITEM_CONTAINER )
    {
	if ( dres.iter_ekw[ dres.miejsce ].stamp != 0 )
	{
	    gtk_tree_store_remove( dres.store_inw, &dres.iter_ekw[ dres.miejsce ] );
	    dres.iter_ekw[ dres.miejsce ].stamp = 0;
	}
	return;
    }
    else
    {
	char bufor[ MIL ];

	sprintf( bufor, "w %s (#%d)", obj->miejscownik, obj->vnum );

	if ( dres.iter_ekw[ dres.miejsce ].stamp == 0 )
	    gtk_tree_store_append( dres.store_inw, &dres.iter_ekw[ dres.miejsce ], NULL );

	gtk_tree_store_set( dres.store_inw, &dres.iter_ekw[ dres.miejsce ],
	    KOL_WSKAZNIK, obj,
	    KOL_NAZWA, _( bufor ),
	    KOL_VIS, PANGO_WEIGHT_BOLD, -1 );
    }

    return;
}


FUNKCJA_DLA_GTK( reset_z_listy )
{
    GtkTreeModel *model;
    GtkTreeIter parent, iter;

    if ( !gtk_tree_selection_get_selected( dkra.selection_res, &model, &dres.iter ) )
	return;

    iter = dres.iter;

    while ( gtk_tree_model_iter_parent( model, &parent, &iter ) )
	iter = parent;

    gtk_tree_model_get( model, &iter, KOL_WSKAZNIK, &dres.reset, -1 );

    dialog_resetu( NULL, NULL );

    return;
}


FUNKCJA_DLA_GTK( dialog_resetu )
{
    char bufor[ MIL ];
    OBJ_INDEX_DATA *obj;
    RESET_DATA *r;
    GtkTreeIter iter, przy_sobie;

    if ( !dres.reset )
	return;

    gtk_tree_store_clear( dres.store_inw );
    gtk_widget_set_sensitive( dres.button_inw_dodaj, FALSE );
    gtk_widget_set_sensitive( dres.button_inw_usun, FALSE );

    if ( dres.reset->command == 'O' )
    {
	gtk_label_set_text( dres.label_int, "poziom" );
	gtk_label_set_text( dres.label_inw, _( "zawarto`s`c" ) );

	/* jesli wartosc sie nie zmieni, nie zostanie wyemitowany sygnal, a on
	   jest potrzebny do ustawienia kontrolek dla przedmiotu */
	if ( SPIN_GET_VALUE( dres.spin_prz ) == dres.reset->arg1 )
	    spin_res_prz( NULL, NULL );
	else
	    SPIN_SET_VALUE( dres.spin_prz, dres.reset->arg1 );

	gtk_tree_store_insert_with_values( dres.store_inw, &przy_sobie, NULL, 0,
	    KOL_WSKAZNIK, NULL,
	    KOL_NAZWA, "W pojemniku",
	    KOL_VIS, PANGO_WEIGHT_BOLD, -1 );

	for ( r = dres.reset->next; r; r = r->next )
	    if ( r->command == 'P' )
	    {
		obj = get_obj_index( r->arg1 );
		sprintf( bufor, "%s (#%d)", obj->short_descr, obj->vnum );

		gtk_tree_store_append( dres.store_inw, &iter, &przy_sobie );
		gtk_tree_store_set( dres.store_inw, &iter,
		    KOL_WSKAZNIK, obj,
		    KOL_NAZWA, _( bufor ),
		    KOL_VIS, PANGO_WEIGHT_NORMAL, -1 );
	    }
	    else if ( r->command != '*' )
		break;
    }
    else if ( dres.reset->command == 'M' )
    {
	int i;
	GtkTreeIter last;

	gtk_label_set_text( dres.label_int, "limit" );
	gtk_label_set_text( dres.label_inw, "inwentarz" );

	if ( SPIN_GET_VALUE( dres.spin_mob ) == dres.reset->arg1 )
	    spin_res_mob( NULL, NULL );
	else
	    SPIN_SET_VALUE( dres.spin_mob, dres.reset->arg1 );

	gtk_tree_store_insert_with_values( dres.store_inw, &przy_sobie, NULL, 0,
	    KOL_WSKAZNIK, NULL,
	    KOL_NAZWA, "Przy sobie",
	    KOL_VIS, PANGO_WEIGHT_BOLD, -1 );

	for ( i = 0; i < MAX_WEAR; i++ )
	{
	    /* w tej kolejnosci, bo funkcja podczepiona pod zmiane wartosci
	       w spinie sprobuje usunac elementy wyczyszczone wczesniej przez
	       gtk_tree_store_clear() */
	    dres.iter_ekw[ i ].stamp = 0;
	    SPIN_SET_VALUE( dres.spin_ekw[ i ], -1 );
	}

	for ( r = dres.reset->next; r; r = r->next )
	    if ( r->command == 'P' )
	    {
		obj = get_obj_index( r->arg1 );
		sprintf( bufor, "%s (#%d)", obj->short_descr, obj->vnum );

		gtk_tree_store_append( dres.store_inw, &iter, &last );
		gtk_tree_store_set( dres.store_inw, &iter,
		    KOL_WSKAZNIK, obj,
		    KOL_NAZWA, _( bufor ),
		    KOL_VIS, PANGO_WEIGHT_NORMAL, -1 );
	    }
	    else if ( r->command == 'G' )
	    {
		gint weight;
		GtkTreePath *path;

		obj = get_obj_index( r->arg1 );
		sprintf( bufor, "%s (#%d)", obj->short_descr, obj->vnum );

		path = gtk_tree_model_get_path( GTK_TREE_MODEL( dres.store_inw ), &przy_sobie );

		if ( obj->item_type != ITEM_CONTAINER
		  || dres.reset->command != 'M'
		  || gtk_tree_path_get_depth( path ) > 1
		  || gtk_tree_path_get_indices( path )[ 0 ] != 0 )
		{
		    weight = PANGO_WEIGHT_NORMAL;
		}
		else
		    weight = PANGO_WEIGHT_BOLD;

		gtk_tree_path_free( path );

		gtk_tree_store_append( dres.store_inw, &last, &przy_sobie );
		gtk_tree_store_set( dres.store_inw, &last,
		    KOL_WSKAZNIK, obj,
		    KOL_NAZWA, _( bufor ),
		    KOL_VIS, weight, -1 );
	    }
	    else if ( r->command == 'E' )
	    {
		SPIN_SET_VALUE( dres.spin_ekw[ r->arg3 ], r->arg1 );
		last = dres.iter_ekw[ r->arg3 ];
	    }
	    else if ( r->command != '*' )
		break;

	gtk_widget_show( dres.frame_inw );
    }
    else if ( dres.reset->command == 'D' )
    {
	return;
    }
    else
    {
	printf( "edycja resetu innego niz M/O/D\n" );
	return;
    }

    glaca_widget_set_visible( dres.frame_ekw, ( dres.reset->command == 'M' ) );
    glaca_widget_set_visible( dres.hbox_mob, ( dres.reset->command == 'M' ) );
    glaca_widget_set_visible( dres.hbox_prz, ( dres.reset->command == 'O' ) );
    SPIN_SET_VALUE( dres.spin_int, dres.reset->arg2 );
    SPIN_SET_VALUE( dres.spin_pom, dres.reset->arg3 );

    gtk_tree_view_expand_all( dres.treeview_inw );

    gtk_grab_add( dres.Reset );
    gtk_widget_show( dres.Reset );

    return;
}


int zapisz_reset_D( ROOM_INDEX_DATA *room )
{

    return 0;
}


int zapisz_reset_O( ROOM_INDEX_DATA *room )
{
    OBJ_INDEX_DATA *obj;
    int j, n, v;
    RESET_DATA *r, *temp;
    GtkTreeModel *model;
    GtkTreeIter iter, child;
    char buf[ MIL ], bufnum[ MIL ];

    if ( ( v = SPIN_GET_VALUE( dres.spin_prz ) ) < 0
      || !( obj = get_obj_index( v ) ) )
	return 1;

    /* zmien reset O */
    dres.reset->arg1 = SPIN_GET_VALUE( dres.spin_prz );
    dres.reset->arg2 = SPIN_GET_VALUE( dres.spin_int );
    dres.reset->arg3 = SPIN_GET_VALUE( dres.spin_pom );

    /* zwolnij wszystkie komentarze i resety P podczepione do resetu O */
    for ( r = dres.reset->next; r;  )
	if ( r->command == 'P' || r->command == '*' )
	{
	    temp = r;
	    r = r->next;
	    del_reset( temp );
	}
	else
	    break;

    temp = r; /* tutaj temp pamieta pierwszy reset spoza O */
    r = dres.reset;

    model = GTK_TREE_MODEL( dres.store_inw );

    /* teraz zapis przedmiotow w pojemniku jako resety P */
    if ( obj->item_type == ITEM_CONTAINER )
    {
	gtk_tree_model_iter_nth_child( model, &iter, NULL, 0 );
	j = gtk_tree_model_iter_n_children( model, &iter );

	for ( n = 0; n < j; n++ )
	    if ( gtk_tree_model_iter_nth_child( model, &child, &iter, n ) )
	    {
		OBJ_INDEX_DATA *o;

		gtk_tree_model_get( model, &child, KOL_WSKAZNIK, &o, -1 );

		r->next = new_reset( );
		r = r->next;
		r->command = 'P';
		r->arg1 = o->vnum;
	    }
    }
    r->next = temp;

    /* odzwierciedlenie powyzszych zmian na liscie resetow */
    strcpy( bufnum, &dres.reset->command );
    sprintf( buf, "%s -> %s", obj->short_descr, room->name );
    gtk_tree_store_set( dkra.store_res, &dres.iter,
	KOL_VNUM, bufnum,
	KOL_OPIS, _( buf ), -1 );

    model = GTK_TREE_MODEL( dkra.store_res );
    j = gtk_tree_model_iter_n_children( model, &dres.iter );
    for ( n = 0; n < j; n++ )
	if ( gtk_tree_model_iter_nth_child( model, &child, &dres.iter, 0 ) )
	    gtk_tree_store_remove( dkra.store_res, &child );

    for ( r = dres.reset->next; r && r->command == 'P'; r = r->next )
    {
	OBJ_INDEX_DATA *o = get_obj_index( r->arg1 );

	sprintf( bufnum, "%c", r->command );
	strcpy( buf, o->short_descr );

	gtk_tree_store_append( dkra.store_res, &iter, &dres.iter );
	gtk_tree_store_set( dkra.store_res, &iter,
	    KOL_WSKAZNIK, r,
	    KOL_VNUM, bufnum,
	    KOL_OPIS, _( buf ), -1 );
    }

    return 0;
}

int zapisz_reset_M( ROOM_INDEX_DATA *room )
{
    MOB_INDEX_DATA *mob;
    OBJ_INDEX_DATA *obj;
    int i, j, n, v;
    RESET_DATA *r, *temp;
    GtkTreeModel *model;
    GtkTreeIter iter, child;
    char buf[ MIL ], bufnum[ MIL ];

    /* sprawdz poprawnosc danych */
    if ( ( v = SPIN_GET_VALUE( dres.spin_mob ) ) < 0
      || !( mob = get_mob_index( v ) ) )
	return 1;

    for ( i = 0; i < MAX_WEAR; i++ )
    {
	if ( ( v = SPIN_GET_VALUE( dres.spin_ekw[ i ] ) ) < 0 )
	    continue;

	if ( !( obj = get_obj_index( v ) ) )
	    return 2;

	if( dres.wear_na_item[ i ] != obj->wear_flags
	  || ( i == WEAR_LIGHT && obj->item_type != ITEM_LIGHT ) )
	    /* przydalby sie dialog z komunikatem, dlaczego odmawia zapisu */
	    return 3;
    }

    /* zmien reset M */
    dres.reset->arg1 = SPIN_GET_VALUE( dres.spin_mob );
    dres.reset->arg2 = SPIN_GET_VALUE( dres.spin_int );
    dres.reset->arg3 = SPIN_GET_VALUE( dres.spin_pom );

    /* zwolnij wszystkie resety podczepione do resetu M */
    for ( r = dres.reset->next; r;  )
    {
	if ( r->command != 'P' && r->command != 'G'
	  && r->command != 'E' && r->command != '*' )
	    break;

	temp = r;
	r = r->next;
	del_reset( temp );
    }

    temp = r; /* tutaj temp pamieta pierwszy reset spoza M */
    r = dres.reset;

    model = GTK_TREE_MODEL( dres.store_inw );

    /* z kazdego wybranego przedmiotu zrob reset E */
    for ( i = 0; i < MAX_WEAR; i++ )
    {
	if ( ( v = SPIN_GET_VALUE( dres.spin_ekw[ i ] ) ) == -1 )
	    continue;

	r->next = new_reset( );
	r = r->next;
	r->command = 'E';
	r->arg1 = v;
	r->arg3 = i;

	iter = dres.iter_ekw[ i ];

	/* jesli przedmiot ma zawartosc, zrob z niej resety P */
	if ( iter.stamp != 0 )
	{
	    j = gtk_tree_model_iter_n_children( model, &iter );

	    for ( n = 0; n < j; n++ )
		if ( gtk_tree_model_iter_nth_child( model, &child, &iter, n ) )
		{
		    gtk_tree_model_get( model, &child, KOL_WSKAZNIK, &obj, -1 );

		    r->next = new_reset( );
		    r = r->next;
		    r->command = 'P';
		    r->arg1 = obj->vnum;
		}
	}
    }

    /* teraz zapis przedmiotow "przy sobie" jako resety G */
    gtk_tree_model_get_iter_first( model, &iter );
    j = gtk_tree_model_iter_n_children( model, &iter );

    for ( n = 0; n < j; n++ )
	if ( gtk_tree_model_iter_nth_child( model, &child, &iter, n ) )
	{
	    gtk_tree_model_get( model, &child, KOL_WSKAZNIK, &obj, -1 );

	    r->next = new_reset( );
	    r = r->next;
	    r->command = 'G';
	    r->arg1 = obj->vnum;

	    /* resety G moga byc pojemnikami, wiec jeszcze raz resety P: */
	    {
		int k, l;
		GtkTreeIter grandchild;

		l = gtk_tree_model_iter_n_children( model, &child );

		for ( k = 0; k < l; k++ )
		    if ( gtk_tree_model_iter_nth_child( model, &grandchild, &child, k ) )
		    {
			gtk_tree_model_get( model, &grandchild, KOL_WSKAZNIK, &obj, -1 );

			r->next = new_reset( );
			r = r->next;
			r->command = 'P';
			r->arg1 = obj->vnum;
		    }
	    }
	}

    r->next = temp;

    /* odzwierciedlenie powyzszych zmian na liscie resetow */
    strcpy( bufnum, &dres.reset->command );
    sprintf( buf, "%s -> %s", mob->short_descr, room->name );
    gtk_tree_store_set( dkra.store_res, &dres.iter,
	KOL_VNUM, bufnum,
	KOL_OPIS, _( buf ), -1 );

    model = GTK_TREE_MODEL( dkra.store_res );
    j = gtk_tree_model_iter_n_children( model, &dres.iter );
    for ( n = 0; n < j; n++ )
	if ( gtk_tree_model_iter_nth_child( model, &child, &dres.iter, 0 ) )
	    gtk_tree_store_remove( dkra.store_res, &child );

    for ( r = dres.reset->next; r; r = r->next )
    {
	switch ( r->command )
	{
	    case 'G':
	    case 'E':
		gtk_tree_store_append( dkra.store_res, &iter, &dres.iter );
		child = iter;
		break;
	    case 'P':
	        gtk_tree_store_append( dkra.store_res, &iter, &child );
	        break;
	    case '*':
		continue;
	    default:
		return 0;
	}

	sprintf( bufnum, "%c", r->command );
	strcpy( buf, get_obj_index( r->arg1 )->short_descr );

	gtk_tree_store_set( dkra.store_res, &iter,
	    KOL_WSKAZNIK, r,
	    KOL_VNUM, bufnum,
	    KOL_OPIS, _( buf ), -1 );
    }

    return 0;
}


FUNKCJA_DLA_GTK( zatwierdzenie_zmian_resetu )
{
    ROOM_INDEX_DATA *room;
    RESET_DATA *r;
    AREA_DATA *kraina;
    int blad, v;

    if ( ( v = SPIN_GET_VALUE( dres.spin_pom ) ) < 0
      || !( room = get_room_index( v ) ) )
	return;

    switch ( dres.reset->command )
    {
	case 'M': blad = zapisz_reset_M( room ); break;
	case 'O': blad = zapisz_reset_O( room ); break;
	case 'D': blad = zapisz_reset_D( room ); break;
	default: return;
    }

    if ( blad != 0 )
	return;

    for ( kraina = area_first; kraina; kraina = kraina->next )
	for ( r = kraina->reset_first; r && r != kraina->reset_last; r = r->next )
	    if ( r == dres.reset )
	    {
		kraina->odnowiona = TRUE;
		break;
	    }

    if ( !data )
	smierc_dialogu_resetu( NULL, NULL, NULL );

    return;
}


FUNKCJA_DLA_GTK_2( smierc_dialogu_resetu )
{
    gtk_widget_hide( dres.Reset );
    gtk_grab_remove( dres.Reset );

    return TRUE;
}


FUNKCJA_DLA_GTK( combo_res_mob )
{
    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filter_mob, widget, NULL );
    else if ( dcom.wybrany )
    {
	MOB_INDEX_DATA *mob;

	gtk_tree_model_get( filter_mob, &dcom.iter, KOL_ELEMENT, &mob, -1 );
	SPIN_SET_VALUE( dres.spin_mob, mob->vnum );
    }

    return;
}


FUNKCJA_DLA_GTK( combo_res_pom )
{
    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filter_pom, widget, NULL );
    else if ( dcom.wybrany )
    {
	ROOM_INDEX_DATA *room;

	gtk_tree_model_get( filter_pom, &dcom.iter, KOL_ELEMENT, &room, -1 );
	SPIN_SET_VALUE( dres.spin_pom, room->vnum );
    }

    return;
}


FUNKCJA_DLA_GTK( combo_res_prz )
{
    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filter_prz, widget, NULL );
    else if ( dcom.wybrany )
    {
	OBJ_INDEX_DATA *obj;

	gtk_tree_model_get( filter_prz, &dcom.iter, KOL_ELEMENT, &obj, -1 );
	SPIN_SET_VALUE( dres.spin_prz, obj->vnum );
    }

    return;
}


FUNKCJA_DLA_GTK( spin_res_mob )
{
    MOB_INDEX_DATA *mob = get_mob_index( SPIN_GET_VALUE( dres.spin_mob ) );
    int i;
    int wl = ~0;

    if ( !mob )
    {
	gtk_window_set_title( GTK_WINDOW( dres.Reset ), "Reset moba" );
	gtk_label_set_text( dres.label_mob, "mob nie istnieje" );
    }
    else
    {
	char bufor[ MIL ];

	sprintf( bufor, "Reset %s (#%d, %s)",
	    mob->dopelniacz,
	    mob->vnum,
	    mob->index_area->full_name );
	gtk_window_set_title( GTK_WINDOW( dres.Reset ), _( bufor ) );

	gtk_label_set_text( dres.label_mob, _( mob->short_descr ) );

	wl = race_table[ mob->race ].wearlocs;
    }

    for ( i = 0; i < MAX_WEAR; i++ )
    {
	bool s; 

	if ( !mob )
	    s = TRUE;
	else if ( i == WEAR_LIGHT )
	    s = IS_SET( wl, ITEM_HOLD );
	else
	    s = IS_SET( wl, dres.wear_na_item[ i ] & ~ITEM_TAKE );

	gtk_widget_set_sensitive( dres.spin_ekw[ i ], !s );
	gtk_widget_set_sensitive( dres.toggle_ekw[ i ], !s );
    }

    return;
}


FUNKCJA_DLA_GTK( spin_res_pom )
{
    ROOM_INDEX_DATA *room = get_room_index( SPIN_GET_VALUE( dres.spin_pom ) );

    if ( !room )
	gtk_label_set_text( dres.label_pom, "pomieszczenie nie istnieje" );
    else
	gtk_label_set_text( dres.label_pom, _( room->name ) );

    return;
}


FUNKCJA_DLA_GTK( spin_res_prz )
{
    OBJ_INDEX_DATA *obj = get_obj_index( SPIN_GET_VALUE( dres.spin_prz ) );
    char bufor[ MIL ];

    gtk_window_resize( GTK_WINDOW( dres.Reset ), 1, 1 );

    if ( !obj )
    {
	gtk_label_set_text( dres.label_prz, "przedmiot nie istnieje" );
	gtk_window_set_title( GTK_WINDOW( dres.Reset ), "Reset przedmiotu" );
	gtk_widget_hide( dres.frame_inw );
	return;
    }

    gtk_label_set_text( dres.label_prz, _( obj->short_descr ) );

    sprintf( bufor, "Reset %s (#%d, %s)",
	obj->dopelniacz,
	obj->vnum,
	obj->area->full_name );
    gtk_window_set_title( GTK_WINDOW( dres.Reset ), _( bufor ) );

    glaca_widget_set_visible( dres.frame_inw, ( obj->item_type == ITEM_CONTAINER ) );

    return;
}


G_MODULE_EXPORT void zmiana_pojemnika( GtkTreeView *tree, gpointer data )
{
    GtkTreeModel *model;
    GtkTreeIter iter, parent;
    bool has_parent, is_container;
    gint weight;

    if ( !gtk_tree_selection_get_selected( dres.select_inw, &model, &iter ) )
	return;

    gtk_tree_model_get( model, &iter, KOL_VIS, &weight, -1 );

    is_container = ( weight == PANGO_WEIGHT_BOLD );
    has_parent = gtk_tree_model_iter_parent( model, &parent, &iter );

    gtk_widget_set_sensitive( dres.button_inw_dodaj, is_container );
    gtk_widget_set_sensitive( dres.button_inw_usun, has_parent );

    return;
}


FUNKCJA_DLA_GTK( usun_z_inwentarza )
{
    GtkTreeModel *model;
    GtkTreeIter iter, parent;

    if ( !gtk_tree_selection_get_selected( dres.select_inw, &model, &iter )
      || !gtk_tree_model_iter_parent( model, &parent, &iter ) )
	return;

    gtk_widget_set_sensitive( dres.button_inw_usun, FALSE );
    gtk_tree_store_remove( dres.store_inw, &iter );

    return;
}


FUNKCJA_DLA_GTK( dodaj_do_inwentarza )
{
    GtkTreeModel *model;
    GtkTreeIter iter, nowy;
    OBJ_INDEX_DATA *obj;
    char bufor[ MIL ];
    gint weight;
    GtkTreePath *path;

    if ( !gtk_tree_selection_get_selected( dres.select_inw, &model, &iter ) )
	return;

    gtk_tree_model_get( model, &iter, KOL_VIS, &weight, -1 );

    if ( weight != PANGO_WEIGHT_BOLD
      || !( obj = get_obj_index( SPIN_GET_VALUE( dres.spin_inw_dodaj ) ) )
      || !IS_SET( obj->wear_flags, ITEM_TAKE ) )
	return;

    path = gtk_tree_model_get_path( model, &iter );

    if ( obj->item_type != ITEM_CONTAINER
      || dres.reset->command != 'M'
      || gtk_tree_path_get_depth( path ) > 1
      || gtk_tree_path_get_indices( path )[ 0 ] != 0 )
    {
	weight = PANGO_WEIGHT_NORMAL;
    }
    else
	weight = PANGO_WEIGHT_BOLD;

    gtk_tree_path_free( path );

    sprintf( bufor, "%s (#%d)", obj->short_descr, obj->vnum );

    gtk_tree_store_append( dres.store_inw, &nowy, &iter );
    gtk_tree_store_set( dres.store_inw, &nowy,
	KOL_WSKAZNIK, obj,
	KOL_NAZWA, _( bufor ),
	KOL_VIS, weight,
	-1 );

    return;
}


FUNKCJA_DLA_GTK( spin_res_dodaj )
{
    OBJ_INDEX_DATA *obj = get_obj_index( SPIN_GET_VALUE( dres.spin_inw_dodaj ) );

    if ( !obj )
	gtk_label_set_text( dres.label_inw_dodaj, "przedmiot nie istnieje" );
    else
	gtk_label_set_text( dres.label_inw_dodaj, _( obj->short_descr ) );

    return;
}


FUNKCJA_DLA_GTK( combo_res_dodaj )
{
    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filter_prz, widget, NULL );
    else if ( dcom.wybrany )
    {
	OBJ_INDEX_DATA *obj;

	gtk_tree_model_get( filter_prz, &dcom.iter, KOL_ELEMENT, &obj, -1 );
	SPIN_SET_VALUE( dres.spin_inw_dodaj, obj->vnum );
    }

    return;
}
