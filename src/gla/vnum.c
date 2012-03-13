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
 *  Okienko pobierajace vnumy i kod tworzacy nowe struktury muda           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "glaca.h"

struct dvnum_data dvnum;

#define UTWORZ_VNUMA_POMIESZCZENIA	0
#define UTWORZ_VNUMA_MOBA		1
#define UTWORZ_VNUMA_PRZEDMIOTU		2


void zrob_okno_Vnuma( void )
{
    ZNAJDZ_KONTROLKE( dvnum.Vnum, "oknovnum" );
    ZNAJDZ_KONTROLKE( dvnum.spin, "vnm_spin" );
    ZNAJDZ_KONTROLKE( dvnum.label, "vnm_label" );
    ZNAJDZ_KONTROLKE( dvnum.button_tak, "vnm_button_tak" );

    return;
}


int wolny_vnum_moba( AREA_DATA *kr )
{
    MOB_INDEX_DATA *mob = kr->first_mob;
    int vnum;

    if ( !mob )
	return 1; /* Moze wziac skads zakres/zajrzec do pomieszczen? */
    else
	vnum = mob->vnum;

    /* Jesli jest mob o vnumie np. 1001, a nie ma o vnumie 1000.
       Specjalna obsluga limbo.are (wychodzilby vnum 0). */
    if ( vnum % 100 && vnum / 100 )
	return vnum / 100 * 100;

    for ( ; mob->next_in_area; mob = mob->next_in_area )
	if ( mob->next_in_area->vnum != mob->vnum + 1 )
	    break;

    /* Ma prawo zmienic zakres na kolejne 100. W skrajnym przypadku to zwroci
       vnuma zajetego przez moba z innej krainy. To nie problem - zatwierdzenie
       vnuma powinno wywalic ostrzezenie, zmuszajac uzytkownika do podania
       wlasciwego vnuma z kolejnego przydzielonego mu przedzialu. Kolejny
       przebieg zacznie juz sugerowac vnumy w tym nowym przedziale. */
    return mob->vnum + 1;
}


/* Ponizej dwie dokladne kopie powyzszego, identyczne zastrzezenia. */
int wolny_vnum_przedmiotu( AREA_DATA *kr )
{
    OBJ_INDEX_DATA *prz = kr->first_obj;
    int vnum;

    if ( !prz )
	return 1;
    else
	vnum = prz->vnum;

    if ( vnum % 100 && vnum / 100 )
	return vnum / 100 * 100;

    for ( ; prz->next_in_area; prz = prz->next_in_area )
	if ( prz->next_in_area->vnum != prz->vnum + 1 )
	    break;

    return prz->vnum + 1;
}


int wolny_vnum_pomieszczenia( AREA_DATA *kr )
{
    ROOM_INDEX_DATA *pom = kr->first_room;
    int vnum;

    if ( !pom )
	return 1;
    else
	vnum = pom->vnum;

    if ( vnum % 100 && vnum / 100 )
	return vnum / 100 * 100;

    for ( ; pom->next_in_area; pom = pom->next_in_area )
	if ( pom->next_in_area->vnum != pom->vnum + 1 )
	    break;

    return pom->vnum + 1;
}


FUNKCJA_DLA_GTK( utworz_vnuma )
{
    int vnum = 1;

    if ( widget == dkra.button_umob )
    {
	dvnum.typ = UTWORZ_VNUMA_MOBA;
	vnum = wolny_vnum_moba( area );
    }
    else if ( widget == dkra.button_uprz )
    {
	dvnum.typ = UTWORZ_VNUMA_PRZEDMIOTU;
	vnum = wolny_vnum_przedmiotu( area );
    }
    else if ( widget == dkra.button_upom )
    {
	dvnum.typ = UTWORZ_VNUMA_POMIESZCZENIA;
	vnum = wolny_vnum_pomieszczenia( area );
    }
    else
	return;

    SPIN_SET_VALUE( dvnum.spin, vnum );

    gtk_grab_add( dvnum.Vnum );
    gtk_widget_show( dvnum.Vnum );

    return;
}


FUNKCJA_DLA_GTK( potwierdz_vnuma )
{
    int vnum;
    char bufnum[ MSL ];
    GtkTreeIter iter;
    GtkTreePath *path;
    GtkTreeModel *model;
    GtkTreeView *tree_view;
    ROOM_INDEX_DATA *room;
    MOB_INDEX_DATA *mob;
    OBJ_INDEX_DATA *obj;

    vnum = SPIN_GET_VALUE( dvnum.spin );
    sprintf( bufnum, "#%d", vnum );

    switch ( dvnum.typ )
    {
    case UTWORZ_VNUMA_PRZEDMIOTU:
	if ( get_obj_index( vnum ) )
	    return;

	obj = new_obj_index( vnum, area );

	gtk_list_store_append( dkra.store_prz, &iter );
	gtk_list_store_set( dkra.store_prz, &iter,
	    KOL_WSKAZNIK, obj,
	    KOL_VNUM, bufnum, -1 );

	gtk_tree_selection_select_iter( dkra.selection_prz, &iter );
	model = GTK_TREE_MODEL( dkra.store_prz );
	tree_view = GTK_TREE_VIEW( dkra.tree_prz );

	przedmiot_z_listy( NULL, NULL );

	break;

    case UTWORZ_VNUMA_POMIESZCZENIA:
	if ( get_room_index( vnum ) )
	    return;

	room = new_room( vnum, area );

	gtk_list_store_append( dkra.store_pom, &iter );
	gtk_list_store_set( dkra.store_pom, &iter,
	    KOL_WSKAZNIK, room,
	    KOL_VNUM, bufnum, -1 );

	gtk_tree_selection_select_iter( dkra.selection_pom, &iter );
	model = GTK_TREE_MODEL( dkra.store_pom );
	tree_view = GTK_TREE_VIEW( dkra.tree_pom );

	pomieszczenie_z_listy( NULL, NULL );

	break;

    case UTWORZ_VNUMA_MOBA:
	if ( get_mob_index( vnum ) )
	    return;

	mob = new_mob_index( vnum, area );

	gtk_list_store_append( dkra.store_mob, &iter );
	gtk_list_store_set( dkra.store_mob, &iter,
	    KOL_WSKAZNIK, mob,
	    KOL_VNUM, bufnum, -1 );

	gtk_tree_selection_select_iter( dkra.selection_mob, &iter );
	model = GTK_TREE_MODEL( dkra.store_mob );
	tree_view = GTK_TREE_VIEW( dkra.tree_mob );

	mob_z_listy( NULL, NULL );

	break;

    default:
	return;
    }

    path = gtk_tree_model_get_path( model, &iter );
    gtk_tree_view_scroll_to_cell( tree_view, path, NULL, FALSE, 0, 0 );
    gtk_tree_path_free( path );

    gtk_widget_hide( dvnum.Vnum );
    gtk_grab_remove( dvnum.Vnum );

    area->odnowiona = TRUE;

    return;
}


FUNKCJA_DLA_GTK( anuluj_vnuma )
{
    gtk_widget_hide( dvnum.Vnum );
    gtk_grab_remove( dvnum.Vnum );

    return;
}


FUNKCJA_DLA_GTK( zmiana_vnuma )
{
    int vnum;

    vnum = SPIN_GET_VALUE( dvnum.spin );

    if ( ( dvnum.typ == UTWORZ_VNUMA_PRZEDMIOTU && get_obj_index( vnum ) )
      || ( dvnum.typ == UTWORZ_VNUMA_POMIESZCZENIA && get_room_index( vnum ) )
      || ( dvnum.typ == UTWORZ_VNUMA_MOBA && get_mob_index( vnum ) ) )
    {
	gtk_label_set( GTK_LABEL( dvnum.label ), _( "vnum zaj`ety" ) );
	gtk_widget_set_sensitive( dvnum.button_tak, FALSE );
    }
    else
    {
	gtk_label_set( GTK_LABEL( dvnum.label ), "" );
	gtk_widget_set_sensitive( dvnum.button_tak, TRUE );
    }

    return;
}

