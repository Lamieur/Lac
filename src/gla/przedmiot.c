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
#include "../handler.h"


struct dprz_data dprz;

FUNKCJA_DLA_GTK( zatwierdzenie_zmian_przedmiotu );
FUNKCJA_DLA_GTK( zmiana_flagi_przedmiotu );
FUNKCJA_DLA_GTK( zmiana_typu_przedmiotu );
void uaktualnij_liste_wplywow( void );


/* Celem tej funkcji jest blokowanie wiersza "(brak)" w typie przedmiotu tak,
   abym tej samej listy przedmiotow mogl uzyc w sklepie moba, gdzie ten wiersz
   chce miec dostepny. Nie mam pojecia jak to dziala, ale funkcja pochodzi
   z programu gtk-demo, ktory polecam wszystkim programujacym z uzyciem GTK+! */
static void set_sensitive( GtkCellLayout *cell_layout, GtkCellRenderer *cell,
			   GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data )
{
    GtkTreePath *path;
    gint *indices;
    gboolean sensitive;

    path = gtk_tree_model_get_path( tree_model, iter );
    indices = gtk_tree_path_get_indices( path );
    /* sensitive = indices[ a ] != b; - indeksem blokowanego wiersza jest b */
    sensitive = indices[ 0 ] != 0;
    gtk_tree_path_free( path );

    g_object_set( cell, "visible", sensitive, NULL );
    g_object_set( cell, "sensitive", sensitive, NULL );

    return;
}


void zrob_okno_Przedmiotu( void )
{
    int i, j;
    char nazwa[ MIL ];
    GtkCellRenderer *renderer;
    GtkWidget *tree, *text;
    GtkTable *table;

    ZNAJDZ_KONTROLKE( dprz.Przedmiot, "oknoprzedmiot" );
    ZNAJDZ_KONTROLKE( dprz.entry_name, "prz_entry_name" );
    ZNAJDZ_KONTROLKE( dprz.entry_short, "prz_entry_short" );
    ZNAJDZ_KONTROLKE( dprz.entry_dopelniacz, "prz_entry_dopelniacz" );
    ZNAJDZ_KONTROLKE( dprz.entry_celownik, "prz_entry_celownik" );
    ZNAJDZ_KONTROLKE( dprz.entry_biernik, "prz_entry_biernik" );
    ZNAJDZ_KONTROLKE( dprz.entry_narzednik, "prz_entry_narzednik" );
    ZNAJDZ_KONTROLKE( dprz.entry_miejscownik, "prz_entry_miejscownik" );
    ZNAJDZ_KONTROLKE( dprz.entry_descr, "prz_entry_descr" );
    ZNAJDZ_KONTROLKE( text, "prz_text_opis" );

    /* tego nie potrafi GLADE, kaka */
    gtk_widget_modify_font( text, font_desc );

#if !defined( BEZ_GTKSPELL )
    gtkspell_new_attach( GTK_TEXT_VIEW( text ), NULL, NULL );
#endif

    ZNAJDZ_KONTROLKE2( dprz.buffer, "prz_buffer_opis", GTK_TEXT_BUFFER );

    ZNAJDZ_KONTROLKE( dprz.combo_rodzaj, "prz_combo_rodzaj" );
    ZNAJDZ_KONTROLKE( dprz.combo_typ, "prz_combo_typ" );
    ZNAJDZ_KONTROLKE2( renderer, "prz_renderer_typ", GTK_CELL_RENDERER );
    gtk_combo_box_set_model( GTK_COMBO_BOX( dprz.combo_typ ),
			     GTK_TREE_MODEL( store_typyprzedmiotow ) );
    gtk_cell_layout_set_cell_data_func( GTK_CELL_LAYOUT( dprz.combo_typ ), renderer,
					set_sensitive, NULL, NULL );

    ZNAJDZ_KONTROLKE( dprz.spin_wielk, "prz_spin_wielk" );
    ZNAJDZ_KONTROLKE( dprz.spin_waga, "prz_spin_waga" );
    ZNAJDZ_KONTROLKE( dprz.combo_ubr, "prz_combo_ubr" );

    ZNAJDZ_KONTROLKE2( dprz.store_app, "prz_store_app", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE( dprz.spin_app, "prz_spin_app" );
    ZNAJDZ_KONTROLKE( dprz.combo_app, "prz_combo_app" );
    ZNAJDZ_KONTROLKE( dprz.button_app_usun, "prz_button_appusun" );
    ZNAJDZ_KONTROLKE( tree, "prz_treeview_app" );
    dprz.select_app = gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );

    ZNAJDZ_KONTROLKE2( dprz.store_resety, "prz_store_resety", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE( tree, "prz_treeview_resety" );
    dprz.select_reset = gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );

    ZNAJDZ_KONTROLKE( dprz.entry_aras, "prz_entry_aras" );
    ZNAJDZ_KONTROLKE( dprz.entry_aprof, "prz_entry_aprof" );
    ZNAJDZ_KONTROLKE( dprz.entry_action, "prz_entry_action" );
    ZNAJDZ_KONTROLKE( dprz.spin_cena, "prz_spin_cena" );
    ZNAJDZ_KONTROLKE( dprz.spin_poz, "prz_spin_poziom" );
    ZNAJDZ_KONTROLKE( dprz.spin_qp, "prz_spin_qp" );

    for ( i = 0; i < 6; i++ )
    {
	sprintf( nazwa, "prz_label_w%d", i );
	ZNAJDZ_KONTROLKE2( dprz.label_w[ i ], nazwa, GTK_LABEL );

	sprintf( nazwa, "prz_spin_w%d", i );
	ZNAJDZ_KONTROLKE( dprz.spin_w[ i ], nazwa );

	sprintf( nazwa, "prz_entry_w%d", i );
	ZNAJDZ_KONTROLKE( dprz.entry_w[ i ], nazwa );

	sprintf( nazwa, "prz_label_toggle%d", i );
	ZNAJDZ_KONTROLKE2( dprz.label_toggle[ i ], nazwa, GTK_LABEL );

	sprintf( nazwa, "prz_toggle_w%d", i );
	ZNAJDZ_KONTROLKE( dprz.toggle_w[ i ], nazwa );
    }
    ZNAJDZ_KONTROLKE( dprz.label_flagi, "prz_label_flagi" );
    ZNAJDZ_KONTROLKE( dprz.button_progi, "prz_button_progi" );
    ZNAJDZ_KONTROLKE( dprz.button_poprz, "prz_button_poprz" );
    ZNAJDZ_KONTROLKE( dprz.button_nast, "prz_button_nast" );

    /* typy ubioru w combo, Lac wyklucza zakladanie przedmiotu w wielu
       miejscach */
    gtk_combo_box_append_text( GTK_COMBO_BOX( dprz.combo_ubr ), "(brak/tylko ziemia)" );
    for ( i = 0; i < 16; i++ ) /* 16 za db.c i merc.h */
    {
	j = 0;
	SET_BIT( j, 1 << i );
	gtk_combo_box_append_text( GTK_COMBO_BOX( dprz.combo_ubr ),
		_( wear_flag_name_pl( j ) ) );
    }

    /* nawet lista flag z tablicy */
    ZNAJDZ_KONTROLKE2( table, "prz_table_flagi", GTK_TABLE );
    for ( i = j = 0; extra_bit_values[ i ].name; i++ )
	/* edytowac da sie tylko flagi dostepne w krainach */
	if ( extra_bit_values[ i ].available )
	{
	    dprz.checkextra[ j ] = gtk_check_button_new_with_label( _( extra_bit_values[ i ].name ) );
	    gtk_widget_set_tooltip_text( dprz.checkextra[ j ], _( extra_bit_values[ i ].tooltip ) );
	    gtk_table_attach( table, dprz.checkextra[ j ],
		j % 2, j % 2 + 1, j / 2, j / 2 + 1,
		GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0 );
	    g_signal_connect( G_OBJECT( dprz.checkextra[ j ] ), "toggled",
		G_CALLBACK( zmiana_flagi_przedmiotu ), NULL );
	    dprz.check_na_flage[ j ] = extra_bit_values[ i ].bitn;
	    j++;
	}

    gtk_widget_show_all( GTK_WIDGET( table ) );

    j = 0;
    for ( i = 0; apply_values[ i ].name; i++ )
	if ( apply_values[ i ].available )
	{
	    gtk_combo_box_append_text( GTK_COMBO_BOX( dprz.combo_app ),
		_( apply_values[ i ].name ) );
	    dprz.nr_na_wplyw[ j ] = i;
	    j++;
	}

    return;
}


FUNKCJA_DLA_GTK( poprznast_prz )
{
    GtkTreePath *path;
    GtkTreeModel *model;

    if ( !gtk_tree_selection_get_selected( dkra.selection_prz, &model, &dprz.iter ) )
	return;

    path = gtk_tree_model_get_path( model, &dprz.iter );

    zatwierdzenie_zmian_przedmiotu( NULL, GINT_TO_POINTER( 1 ) );

    if ( widget == dprz.button_poprz )
	gtk_tree_path_prev( path );
    else if ( widget == dprz.button_nast )
	gtk_tree_path_next( path );

    gtk_tree_selection_select_path( dkra.selection_prz, path );
    gtk_tree_path_free( path );
    przedmiot_z_listy( NULL, NULL );

    return;
}


FUNKCJA_DLA_GTK( przedmiot_z_listy )
{
    GtkTreeModel *model;

    if ( !gtk_tree_selection_get_selected( dkra.selection_prz, &model, &dprz.iter ) )
	return;
    gtk_tree_model_get( model, &dprz.iter, KOL_WSKAZNIK, &dprz.obj, -1 );
    dialog_przedmiotu( NULL, NULL );
}


FUNKCJA_DLA_GTK( dialog_przedmiotu )
{
    int i, j;
    AREA_DATA *a;
    RESET_DATA *reset;
    char buf[ MIL ];

    if ( !dprz.obj )
	return;

    gtk_grab_add( dprz.Przedmiot );

    sprintf( buf, "#%d: %s (%s)",
	dprz.obj->vnum,
	dprz.obj->short_descr,
	dprz.obj->area->full_name );
    gtk_window_set_title( GTK_WINDOW( dprz.Przedmiot ), _( buf ) );

    ZROB_LANCUCH( dprz.entry_name, dprz.obj->name );
    ZROB_LANCUCH( dprz.entry_short, dprz.obj->short_descr );
    ZROB_LANCUCH( dprz.entry_dopelniacz, dprz.obj->dopelniacz );
    ZROB_LANCUCH( dprz.entry_celownik, dprz.obj->celownik );
    ZROB_LANCUCH( dprz.entry_biernik, dprz.obj->biernik );
    ZROB_LANCUCH( dprz.entry_narzednik, dprz.obj->narzednik );
    ZROB_LANCUCH( dprz.entry_miejscownik, dprz.obj->miejscownik );
    ZROB_LANCUCH( dprz.entry_descr, dprz.obj->description );

    ZROB_LANCUCH_TEXT( dprz.buffer, dprz.obj->look_descr );

    SPIN_SET_VALUE( dprz.spin_waga, dprz.obj->weight );
    SPIN_SET_VALUE( dprz.spin_wielk, dprz.obj->wielkosc );

    COMBO_SET_ACTIVE( dprz.combo_rodzaj, dprz.obj->rodzaj );

    /* Ubranie ma ustawione najwyzej dwa bity: 0 (ITEM_TAKE) i jakis wyzszy
       oznaczajacy, gdzie mozna przedmiot zalozyc. Szukam najwyzszego bitu
       i tyle. */
    i = 0;
    for ( j = dprz.obj->wear_flags; j; j >>= 1 )
	i++;
    COMBO_SET_ACTIVE( dprz.combo_ubr, i );

    for ( i = 0; i < 6; i++ )
    {
	int val;

	if ( typy_przedmiotow[ dprz.obj->item_type ].spell[ i ] == VAL_STRING )
	{
	    gtk_entry_set_text( GTK_ENTRY( dprz.entry_w[ i ] ),
		dprz.obj->value[ i ].p ? _( (char *) dprz.obj->value[ i ].p ) : "" );
	    val = 0;
	}
	else
	{
	    gtk_entry_set_text( GTK_ENTRY( dprz.entry_w[ i ] ), "" );
	    val = dprz.obj->value[ i ].v;
	}

	/* Wszystkie combo ustawiam na podstawie pierwotnych wartosci
	   z przedmiotu podczas odpalania okienka. Jesli przedmiot mial
	   ustawiony zly typ, zmiana typu ustawi poprawna interpretacje
	   wartosci. Jesli ktos za duzo kreci galka typu, nie straci zmian
	   dokonywanych w miedzyczasie (bo kazdy combo pamieta osobno), az do
	   zamkniecia okienka. */
	dprz.wartosci[ 0 ][ i ] = ( val < MAX_SKILL ) ? val : 0;
	dprz.wartosci[ 1 ][ i ] = ( val > 0 && val < MAX_MATERIAL ) ? val : 0;
	dprz.wartosci[ 2 ][ i ] = ( val > 0 && val < LIQ_MAX ) ? val : 0;

	SPIN_SET_VALUE( dprz.spin_w[ i ], val );
    }

    /* ustawienie combo_typ generuje sygnal pokazujacy odpowiednie kontrolki
       edycji... */
    COMBO_SET_ACTIVE( dprz.combo_typ, typ_na_numer[ dprz.obj->item_type ] );
    /* ... o ile typ sie zmienia, bo jesli nie, to sygnal nie jest generowany,
       co nie stanowilo problemu dopoki nie wprowadzilem naszego combo. Dlatego
       zmiana_typu_przedmiotu( ) musi zostac wywolana tak czy owak */
    zmiana_typu_przedmiotu( NULL, NULL );

    for ( i = 0; dprz.checkextra[ i ]; i++ )
	USTAW_CHECKBOXA_V( dprz.checkextra[ i ], dprz.obj->extra_flags, dprz.check_na_flage[ i ] );
    /* jesli wystapila jakas zmiana, powinien zostac wygenerowany sygnal
       "toggled", ale jesli nie wystapila, i tak chce wypelnic etykiete
       odpowiednia informacja */
    zmiana_flagi_przedmiotu( NULL, NULL );

    ZROB_LANCUCH( dprz.entry_aprof, dprz.obj->anti_class );
    ZROB_LANCUCH( dprz.entry_aras, dprz.obj->anti_race );
    ZROB_LANCUCH( dprz.entry_action, dprz.obj->action );
    SPIN_SET_VALUE( dprz.spin_cena, dprz.obj->cena );
    SPIN_SET_VALUE( dprz.spin_poz, dprz.obj->poziom );
    SPIN_SET_VALUE( dprz.spin_qp, dprz.obj->qp );

    uaktualnij_liste_wplywow( );
    SPIN_SET_VALUE( dprz.spin_app, 0 );
    COMBO_SET_ACTIVE( dprz.combo_app, -1 );

    ile_progow_button( dprz.button_progi, dprz.obj->mobprogs );

    gtk_list_store_clear( dprz.store_resety );
    for ( a = area_first; a; a = a->next )
    {
	RESET_DATA *last_mob = NULL;
	RESET_DATA *last_obj = NULL;

	for ( reset = a->reset_first; reset; reset = reset->next )
	{

	    if ( reset->command == 'O'
	      || reset->command == 'G'
	      || reset->command == 'E' )
	    {
		last_obj = reset;
	    }
	    else if ( reset->command == 'M' )
		last_mob = reset;

	    if ( reset->arg1 == dprz.obj->vnum )
	    {
		if ( reset->command == 'G' || reset->command == 'E' )
		{
		    GtkTreeIter iter;
		    MOB_INDEX_DATA *mob = get_mob_index( last_mob->arg1 );

		    sprintf( buf, "%c #%d: %s",
			reset->command, mob->vnum, mob->short_descr );

		    gtk_list_store_append( dprz.store_resety, &iter );
		    gtk_list_store_set( dprz.store_resety, &iter,
			KOL_NAZWA, _( buf ),
			KOL_WSKAZNIK, reset, -1 );
		}
		else if ( reset->command == 'P' )
		{
		    GtkTreeIter iter;
		    OBJ_INDEX_DATA *obj = get_obj_index( last_obj->arg1 );

		    sprintf( buf, "P #%d: %s (%c)",
			obj->vnum, obj->short_descr, last_obj->command );

		    gtk_list_store_append( dprz.store_resety, &iter );
		    gtk_list_store_set( dprz.store_resety, &iter,
			KOL_NAZWA, _( buf ),
			KOL_WSKAZNIK, reset, -1 );
		}
		else if ( reset->command == 'O' )
		{
		    GtkTreeIter iter;
		    ROOM_INDEX_DATA *room = get_room_index( reset->arg3 );

		    sprintf( buf, "O #%d: %s [%d]",
			room->vnum, room->name, reset->arg2 );

		    gtk_list_store_append( dprz.store_resety, &iter );
		    gtk_list_store_set( dprz.store_resety, &iter,
			KOL_NAZWA, _( buf ),
			KOL_WSKAZNIK, reset, -1 );
		}
	    }
	}
    }

    gtk_widget_show( dprz.Przedmiot );

    return;
}


FUNKCJA_DLA_GTK_2( smierc_dialogu_przedmiotu )
{
    gtk_widget_hide( dprz.Przedmiot );
    gtk_grab_remove( dprz.Przedmiot );

    return TRUE; /* dialog nie umrze */
}


FUNKCJA_DLA_GTK( zatwierdzenie_zmian_przedmiotu )
{
    char buf[ MSL ];
    int i;
    int typ;

    WEZ_LANCUCH( dprz.entry_name );
    WEZ_LANCUCH( dprz.entry_short );
    WEZ_LANCUCH( dprz.entry_dopelniacz );
    WEZ_LANCUCH( dprz.entry_celownik );
    WEZ_LANCUCH( dprz.entry_biernik );
    WEZ_LANCUCH( dprz.entry_narzednik );
    WEZ_LANCUCH( dprz.entry_miejscownik );
    WEZ_LANCUCH( dprz.entry_descr );

    WEZ_LANCUCH_TEXT( dprz.buffer );
    PUSC_LANCUCH_TEXT( dprz.buffer );

    WEZ_LANCUCH( dprz.entry_aprof );
    WEZ_LANCUCH( dprz.entry_aras );
    WEZ_LANCUCH( dprz.entry_action );

    dprz.obj->rodzaj = COMBO_GET_ACTIVE( dprz.combo_rodzaj );
    typ = numer_na_typ[ COMBO_GET_ACTIVE( dprz.combo_typ ) ];
    dprz.obj->item_type = typ;
    i = COMBO_GET_ACTIVE( dprz.combo_ubr );
    /* tylko odrobinke zakrecone: */
    if ( i > 1 )
    {
	i--;
	i = 1 << i;
	SET_BIT( i, ITEM_TAKE );
    }
    dprz.obj->wear_flags = i;

    for ( i = 0; i < 6; i++ )
    {
	if ( typy_przedmiotow[ typ ].spell[ i ] == VAL_STRING )
	{
	    if ( dprz.obj->value[ i ].p )
		free_string( dprz.obj->value[ i ].p );

	    strcpy( buf, lac_z_pols( ENTRY_GET_TEXT( dprz.entry_w[ i ] ) ) );
	    if ( *buf )
		dprz.obj->value[ i ].p = str_dup( buf );
	}
	else if ( typy_przedmiotow[ typ ].spell[ i ] == VAL_SPELL )
	    dprz.obj->value[ i ].v = dprz.wartosci[ 0 ][ i ];
	else if ( typy_przedmiotow[ typ ].spell[ i ] == VAL_MATERIAL )
	    dprz.obj->value[ i ].v = dprz.wartosci[ 1 ][ i ];
	else if ( typy_przedmiotow[ typ ].spell[ i ] == VAL_LIQUID )
	    dprz.obj->value[ i ].v = dprz.wartosci[ 2 ][ i ];
	else
	    dprz.obj->value[ i ].v = SPIN_GET_VALUE( dprz.spin_w[ i ] );
    }

    for ( i = 0; dprz.checkextra[ i ]; i++ )
	USTAW_FLAGE_V( dprz.checkextra[ i ], dprz.obj->extra_flags, dprz.check_na_flage[ i ] );

    dprz.obj->weight = SPIN_GET_VALUE( dprz.spin_waga );
    dprz.obj->wielkosc = SPIN_GET_VALUE( dprz.spin_wielk );
    dprz.obj->cena = SPIN_GET_VALUE( dprz.spin_cena );
    dprz.obj->poziom = SPIN_GET_VALUE( dprz.spin_poz );
    dprz.obj->qp = SPIN_GET_VALUE( dprz.spin_qp );

    if ( memcmp( &dprz.iter, &NullIter, sizeof( NullIter ) ) )
    {
	strcpy( buf, _( dprz.obj->short_descr ) );
	gtk_list_store_set( dkra.store_prz, &dprz.iter, KOL_OPIS, buf, -1 );
    }

    if ( !data )
	smierc_dialogu_przedmiotu( NULL, NULL, NULL );

    dprz.obj->area->odnowiona = TRUE;

    return;
}


void zmien_etykietke( int i, int val )
{
    const char *nazwa;

    if ( val == VAL_SPELL )
	nazwa = dprz.wartosci[ 0 ][ i ] > -1
	    ? skill_table[ dprz.wartosci[ 0 ][ i ] ].pl_name : "(brak)";

    else if ( val == VAL_MATERIAL )
	nazwa = material_table[ dprz.wartosci[ 1 ][ i ] ].name;

    else if ( val == VAL_LIQUID )
	nazwa = liq_table_pl[ dprz.wartosci[ 2 ][ i ] ].liq_name;

    else
	nazwa = "";

    gtk_label_set_text( dprz.label_toggle[ i ], _( nazwa ) );

    return;
}


/* polaczone z sygnalem "changed" dprz.combo_typ, wywoluje sie samo
   z dialog_przedmiotu( ) przy ustawianiu wartosci */
FUNKCJA_DLA_GTK( zmiana_typu_przedmiotu )
{
    int i, val;
    int typ = numer_na_typ[ COMBO_GET_ACTIVE( dprz.combo_typ ) ];

    for ( i = 0; i < 6; i++ )
    {
	val = typy_przedmiotow[ typ ].spell[ i ];

	glaca_widget_set_visible( dprz.spin_w[ i ], val == VAL_NORMAL );
	glaca_widget_set_visible( dprz.entry_w[ i ], val == VAL_STRING );

	if ( val == VAL_SPELL || val == VAL_MATERIAL || val == VAL_LIQUID )
	    gtk_widget_show( dprz.toggle_w[ i ] );
	else
	    gtk_widget_hide( dprz.toggle_w[ i ] );

	gtk_label_set_text( dprz.label_w[ i ], _( typy_przedmiotow[ typ ].value[ i ] ) );
	zmien_etykietke( i, val );
    }

    return;
}


FUNKCJA_DLA_GTK( zmiana_flagi_przedmiotu )
{
    char buf[ MSL ];
    int tymczas[ ITEM_EXTRA_FLAGS_SIZE ];
    int i;

    memset( tymczas, 0, ITEM_EXTRA_FLAGS_SIZE * SIZEOF_INT );

    for ( i = 0; dprz.checkextra[ i ]; i++ )
	USTAW_FLAGE_V( dprz.checkextra[ i ], tymczas, dprz.check_na_flage[ i ] );

    strcpy( buf, "flagi: " );
    strcat( buf, extra_bit_name_pl( tymczas, NULL ) );
    gtk_label_set_text( GTK_LABEL( dprz.label_flagi ), _( buf ) );

    return;
}


void uaktualnij_liste_wplywow( void )
{
    char buf[ MIL ];
    GtkTreeIter iter;
    AFFECT_DATA *paf;

    gtk_list_store_clear( dprz.store_app );
    if ( dprz.obj->affected )
    {
	for ( paf = dprz.obj->affected; paf; paf = paf->next )
	{
	    sprintf( buf, "%s%d na %s",
		    paf->modifier >= 0 ? "+" : "", paf->modifier,
		    _( affect_loc_name_pl_b( paf->location ) ) );
	    gtk_list_store_append( dprz.store_app, &iter );
	    gtk_list_store_set( dprz.store_app, &iter,
		KOL_NAZWA, buf,
		KOL_WSKAZNIK, paf, -1 );
	}
	gtk_widget_set_sensitive( dprz.button_app_usun, TRUE );
    }
    else
	gtk_widget_set_sensitive( dprz.button_app_usun, FALSE );

    return;
}


FUNKCJA_DLA_GTK( dodaj_wplyw_przedmiotu )
{
    AFFECT_DATA *af;
    int co, ile;

    co = dprz.nr_na_wplyw[ COMBO_GET_ACTIVE( dprz.combo_app ) ];
    ile = SPIN_GET_VALUE( dprz.spin_app );

    /* nie bede sie bawil w komunikaty */
    if ( !ile || !co )
	return;

    /* jesli mamy wplyw na to samo, zmienie istniejacy wplyw */
    for ( af = dprz.obj->affected; af; af = af->next )
	if ( af->location == co )
	    break;

    if ( !af )
    {
	af = new_affect( );
	af->type = 0;
	af->duration = -1;
	af->location = co;
	af->next = dprz.obj->affected;
	dprz.obj->affected = af;
    }
    af->modifier = ile;

    uaktualnij_liste_wplywow( );

    return;
}


FUNKCJA_DLA_GTK( usun_wplyw_przedmiotu )
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    AFFECT_DATA *zaf, *prev;

    if ( !gtk_tree_selection_get_selected( dprz.select_app, &model, &iter ) )
	return;

    gtk_tree_model_get( model, &iter, KOL_WSKAZNIK, &zaf, -1 );
    if ( !zaf )
	printf( "Kaka?\n" );

    if ( dprz.obj->affected == zaf )
	dprz.obj->affected = zaf->next;
    else
    {
	for ( prev = dprz.obj->affected; prev; prev = prev->next )
	    if ( prev->next == zaf )
		break;
	if ( !prev )
	    printf( "Jeszcze wieksza kaka?\n" );
	prev->next = zaf->next;
    }
    zaf->next = affect_free;
    affect_free = zaf;

    uaktualnij_liste_wplywow( );

    return;
}


FUNKCJA_DLA_GTK( combo_prz_w )
{
    int i, val, v;
    GtkTreeModel *filtr;

    for ( i = 0; ; i++ )
	if ( i == 6 )
	    return;
	else if ( widget == dprz.toggle_w[ i ] )
	    break;

    val = typy_przedmiotow[ numer_na_typ[ COMBO_GET_ACTIVE( dprz.combo_typ ) ] ].spell[ i ];

    switch ( val )
    {
	case VAL_SPELL:    v = 0; filtr = filter_czary;     break;
	case VAL_MATERIAL: v = 1; filtr = filter_materialy; break;
	case VAL_LIQUID:   v = 2; filtr = filter_plyny;     break;
	default:
	    return;
    }

    if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) )
	combo_wyskocz( filtr, widget, NULL );
    else if ( dcom.wybrany )
    {
	int n;

	gtk_tree_model_get( filtr, &dcom.iter, KOL_ELEMENT, &n, -1 );
	dprz.wartosci[ v ][ i ] = n;
	zmien_etykietke( i, val );
    }

    return;
}
