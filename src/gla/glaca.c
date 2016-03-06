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
 *  Edytor krain oparty na GTK+                                            *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* #define KOMPILUJE_GLACA_C - wlaczyc, kiedy sie bedzie dalo */
#include "glaca.h"
#include "../mp_wczyt.h"
#include "../const.h"
#include <sys/stat.h> /* stat( ) */
#include <libgen.h> /* dirname( ) */
#include <sys/time.h> /* gettimeofday( ) */


/* szczerze mowiac nie mam pojecia, gdzie jest dostepny iconv.h.
   Ale z drugiej strony to i tak tylko na potrzeby nowego GTK+, wiec
   nie musze sie tym przejmowac. Lam 5.11.2001
   FIXME: napisac to bez iconva, uzywajac uni_lac[ ] */
#include <iconv.h>

iconv_t iso_uni;
iconv_t uni_iso;

void zrob_Listy( void );

/* Globalne. Lubie globalne. Zawsze lubilem. BASIC rzadzi */
AREA_DATA *area;
GtkBuilder *glacaxml;
GtkTreeIter NullIter;
int typ_na_numer[ ITEM_MAX ];
int numer_na_typ[ ITEM_MAX ];
GtkListStore *store_typyprzedmiotow;

GtkListStore *store_pomieszczenia;
GtkTreeModel *filter_pom;

GtkListStore *store_moby;
GtkTreeModel *filter_mob;

GtkListStore *store_przedmioty;
GtkTreeModel *filter_prz;

GtkListStore *store_czary;
GtkTreeModel *filter_czary;

GtkListStore *store_materialy;
GtkTreeModel *filter_materialy;

GtkListStore *store_plyny;
GtkTreeModel *filter_plyny;

GtkListStore *store_rasy;
GtkTreeModel *filter_rasy;

PangoFontDescription *font_desc;

struct dlis_data dlis;

/* tu wchodzimy do programu, czytamy krainy i wyswietlamy liste krain.
   gdy uzytkownik wybierze kraine, wywolujemy kolejne funkcje (nizej w pliku) */
int main( int argc, char **argv )
{
    struct timeval now_time;
    AREA_DATA *a;
    GtkTreeIter iter;
    struct stat fst;
    char katalog[ MIL ], sciezka[ MIL ], *p;
    GtkWidget *powitalne;
    GtkCellRendererText *renderer;

    gtk_set_locale( );
    gtk_init( &argc, &argv );

    glacaxml = gtk_builder_new( );
    if ( !stat( "glaca.glade", &fst ) )
	gtk_builder_add_from_file( glacaxml, "glaca.glade", NULL /* &error */ );
    else
    {
	/* dirname( ) z glibc niszczy argumenty, wiec nie podam mu argv[ 0 ] */
	strcpy( katalog, argv[ 0 ] );
	strcpy( katalog, dirname( katalog ) );
	sprintf( sciezka, "%s/glaca.glade", katalog );
	if ( !stat( sciezka, &fst ) )
	    gtk_builder_add_from_file( glacaxml, sciezka, NULL /* &error */ );
	else
	{
	    sprintf( sciezka, "%s/gla/glaca.glade", katalog );
	    if ( !stat( sciezka, &fst ) )
		gtk_builder_add_from_file( glacaxml, sciezka, NULL /* &error */ );
	    else
	    {
		sprintf( sciezka, "%s/../glaca/glaca.glade", katalog );
		if ( !stat( sciezka, &fst ) )
		    gtk_builder_add_from_file( glacaxml, sciezka, NULL /* &error */ );
		else
		{
		    cbug( "Nie znalazlem pliku glaca.glade. Sprawdz swoja instalacje.", 0 );
		    return 1;
		}
	    }
	}
    }

    /* okienko powitalne */
    ZNAJDZ_KONTROLKE( powitalne, "oknopowitalne" );
    gtk_widget_show( powitalne );

    /* Doprowadzmy do natychmiastowego wyswietlenia tego okienka. Nie bedzie
       aktualizowane az zniknie, ale to juz minimalny problem, bo jednak
       wczytanie krain nie jest az tak dlugie :) */
    while ( gtk_events_pending( ) )
	gtk_main_iteration( );

    /* Wczytanie krain */
    TylkoKonwersja = TRUE;
    comm_main( TRUE, argc, argv );

    if ( ( iso_uni = iconv_open( "UTF-8", "ISO_8859-2" ) ) == (iconv_t) -1 )
	lac_perror( "iso_uni" );
    if ( ( uni_iso = iconv_open( "ISO_8859-2", "UTF-8" ) ) == (iconv_t) -1 )
	lac_perror( "uni_iso" );

    /* Wielkim problemem gtk_builder_connect_signals jest brak mozliwosci
       sprawdzenia, czy wszystko sie udalo (poza przeczytaniem stderr, gdzie
       to wyrzuca bledy, co za glupota).
       
       Moznaby przekierowac stderr do pliku, nastepnie go odczytac i wyswietlic
       w okienku GTK+, zeby pod Windows uzytkownik mial co skopiowac, zeby potem
       wkleic opiekunowi programu. */
    gtk_builder_connect_signals( glacaxml, NULL );

    ZNAJDZ_KONTROLKE( dlis.lista_krain, "treeviewlistakrain" );
    ZNAJDZ_KONTROLKE( dlis.okno_lista_krain, "oknolistakrain" );
    ZNAJDZ_KONTROLKE2( dlis.store_krain, "lis_store", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE2( renderer, "lis_renderer_nazwa", GTK_CELL_RENDERER_TEXT );
    g_object_set( renderer, "font", "Monospace", NULL );
    ZNAJDZ_KONTROLKE2( renderer, "lis_renderer_zakres", GTK_CELL_RENDERER_TEXT );
    g_object_set( renderer, "font", "Monospace", NULL );
    dlis.selection = gtk_tree_view_get_selection( GTK_TREE_VIEW( dlis.lista_krain ) );
    gtk_tree_selection_set_mode( dlis.selection, GTK_SELECTION_MULTIPLE );

    ZNAJDZ_KONTROLKE( dlis.button_mob, "lis_button_mob" );
    ZNAJDZ_KONTROLKE( dlis.button_pom, "lis_button_pom" );
    ZNAJDZ_KONTROLKE( dlis.button_prz, "lis_button_prz" );
    ZNAJDZ_KONTROLKE2( dlis.entry_vnum, "lis_entry_vnum", GTK_ENTRY );

    for ( a = area_first; a; a = a->next )
    {
	p = wiersz( a, sciezka );
	gtk_list_store_append( dlis.store_krain, &iter );
	gtk_list_store_set( dlis.store_krain, &iter,
		KOL_WSKAZNIK, a,
		KOL_NAZWA, a->file_name,
		KOL_OPIS, sciezka,
		KOL_OPIS2, p, -1 );
    }

    font_desc = pango_font_description_from_string( "Monospace" );
    pango_font_description_set_absolute_size( font_desc, 12 * PANGO_SCALE );

    /* dialogi */
    zrob_Listy( );
    zrob_dialog_Combo( );
    zrob_okno_Krainy( );
    zrob_okno_Pomieszczenia( );
    zrob_okno_Moba( );
    zrob_okno_Vnuma( );
    zrob_okno_Mapy( );
    zrob_okno_Wyjscia( );
    zrob_okno_Progow( );
    zrob_okno_Przedmiotu( );
    zrob_okno_Resetu( );

    pango_font_description_free( font_desc );

    gtk_widget_show( dlis.okno_lista_krain );
    gtk_widget_hide( powitalne );

    /* zadne ZNAJDZ_KONTROLKE nie moze byc potem wywolywane - wszystko musi
       byc wczesniej znalezione w funkcjach zrob_okno_blabla */
    g_object_unref( G_OBJECT( glacaxml ) );
    gtk_main( );

    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    log_string( "Edytor krain zakonczyl dzialanie." );

    return 0;
}


/* Tworzenie uniwersalnych list GtkListStore dla wielu kontrolek. Warto zwrocic
   uwage, ze nie oplaca sie przypinac list do kontrolek juz w Glade, bo
   dodawanie elementow do list, ktore sa juz przypiete, trwa dluzej. Dopiero po
   uzupelnieniu list do podpiecia nalezy uzywac gtk_combo_box_set_model( ). */
void zrob_Listy( void )
{
    SORTED_SKILL_DATA *ss;
    ROOM_INDEX_DATA *room;
    MOB_INDEX_DATA *mob;
    OBJ_INDEX_DATA *obj;
    GtkTreeIter iter;
    int i, p;
    char bufor[ MIL ];

    ZNAJDZ_KONTROLKE2( store_pomieszczenia, "store_pomieszczenia", GTK_LIST_STORE );
    g_object_ref( store_pomieszczenia );
    for ( i = 0; i < MAX_KEY_HASH; i++ )
	for ( room = room_index_hash[ i ]; room; room = room->next )
	{
	    sprintf( bufor, "#%d: %s", room->vnum, room->name );
	    gtk_list_store_append( store_pomieszczenia, &iter );
	    gtk_list_store_set( store_pomieszczenia, &iter,
		KOL_WSKAZNIK, room->name,
		KOL_NAZWA, _( bufor ),
		KOL_ELEMENT, room,
		KOL_VIS, TRUE, -1 );
	}
    filter_pom = gtk_tree_model_filter_new( GTK_TREE_MODEL( store_pomieszczenia ), NULL );
    gtk_tree_model_filter_set_visible_column( GTK_TREE_MODEL_FILTER( filter_pom ), KOL_VIS );


    ZNAJDZ_KONTROLKE2( store_moby, "store_moby", GTK_LIST_STORE );
    g_object_ref( store_moby );
    for ( i = 0; i < MAX_KEY_HASH; i++ )
	for ( mob = mob_index_hash[ i ]; mob; mob = mob->next )
	{
	    sprintf( bufor, "#%d: %s", mob->vnum, mob->short_descr );
	    gtk_list_store_append( store_moby, &iter );
	    gtk_list_store_set( store_moby, &iter,
		KOL_WSKAZNIK, mob->short_descr,
		KOL_NAZWA, _( bufor ),
		KOL_ELEMENT, mob,
		KOL_VIS, TRUE, -1 );
	}
    filter_mob = gtk_tree_model_filter_new( GTK_TREE_MODEL( store_moby ), NULL );
    gtk_tree_model_filter_set_visible_column( GTK_TREE_MODEL_FILTER( filter_mob ), KOL_VIS );


    ZNAJDZ_KONTROLKE2( store_przedmioty, "store_przedmioty", GTK_LIST_STORE );
    g_object_ref( store_przedmioty );
    for ( i = 0; i < MAX_KEY_HASH; i++ )
	for ( obj = obj_index_hash[ i ]; obj; obj = obj->next )
	{
	    sprintf( bufor, "#%d: %s", obj->vnum, obj->short_descr );
	    gtk_list_store_append( store_przedmioty, &iter );
	    gtk_list_store_set( store_przedmioty, &iter,
		KOL_WSKAZNIK, obj->short_descr,
		KOL_NAZWA, _( bufor ),
		KOL_ELEMENT, obj,
		KOL_VIS, TRUE, -1 );
	}
    filter_prz = gtk_tree_model_filter_new( GTK_TREE_MODEL( store_przedmioty ), NULL );
    gtk_tree_model_filter_set_visible_column( GTK_TREE_MODEL_FILTER( filter_prz ), KOL_VIS );


    ZNAJDZ_KONTROLKE2( store_czary, "store_czary", GTK_LIST_STORE );
    g_object_ref( store_czary );

    gtk_list_store_append( store_czary, &iter );
    gtk_list_store_set( store_czary, &iter,
	KOL_WSKAZNIK, "(brak)",
	KOL_NAZWA, "(brak)",
	KOL_ELEMENT, -1,
	KOL_VIS, TRUE, -1 );

    for ( ss = sorted_skill_list; ss; ss = ss->next )
	if ( ss->sn ) /* wyfiltrowac "zarezerwowane" */
	{
	    gtk_list_store_append( store_czary, &iter );
	    gtk_list_store_set( store_czary, &iter,
		KOL_WSKAZNIK, skill_table[ ss->sn ].pl_name,
		KOL_NAZWA, _( skill_table[ ss->sn ].pl_name ),
		KOL_ELEMENT, ss->sn,
		KOL_VIS, TRUE, -1 );
	}
    filter_czary = gtk_tree_model_filter_new( GTK_TREE_MODEL( store_czary ), NULL );
    gtk_tree_model_filter_set_visible_column( GTK_TREE_MODEL_FILTER( filter_czary ), KOL_VIS );


    ZNAJDZ_KONTROLKE2( store_materialy, "store_materialy", GTK_LIST_STORE );
    g_object_ref( store_materialy );
    for ( i = 0; i < MAX_MATERIAL; i++ )
    {
	gtk_list_store_append( store_materialy, &iter );
	gtk_list_store_set( store_materialy, &iter,
	    KOL_WSKAZNIK, material_table[ i ].name,
	    KOL_NAZWA, _( material_table[ i ].name ),
	    KOL_ELEMENT, i,
	    KOL_VIS, TRUE, -1 );
    }
    filter_materialy = gtk_tree_model_filter_new( GTK_TREE_MODEL( store_materialy ), NULL );
    gtk_tree_model_filter_set_visible_column( GTK_TREE_MODEL_FILTER( filter_materialy ), KOL_VIS );


    ZNAJDZ_KONTROLKE2( store_plyny, "store_plyny", GTK_LIST_STORE );
    g_object_ref( store_plyny );
    for ( i = 0; i < LIQ_MAX; i++ )
    {
	gtk_list_store_append( store_plyny, &iter );
	gtk_list_store_set( store_plyny, &iter,
	    KOL_WSKAZNIK, liq_table_pl[ i ].liq_name,
	    KOL_NAZWA, _( liq_table_pl[ i ].liq_name ),
	    KOL_ELEMENT, i,
	    KOL_VIS, TRUE, -1 );
    }
    filter_plyny = gtk_tree_model_filter_new( GTK_TREE_MODEL( store_plyny ), NULL );
    gtk_tree_model_filter_set_visible_column( GTK_TREE_MODEL_FILTER( filter_plyny ), KOL_VIS );


    ZNAJDZ_KONTROLKE2( store_rasy, "store_rasy", GTK_LIST_STORE );
    g_object_ref( store_rasy );
    for ( i = 0; i < MAX_RACE; i++ )
    {
	gtk_list_store_append( store_rasy, &iter );
	gtk_list_store_set( store_rasy, &iter,
	    KOL_WSKAZNIK, race_table[ i ].who_name,
	    KOL_NAZWA, _( race_table[ i ].who_name ),
	    KOL_ELEMENT, i,
	    KOL_VIS, TRUE, -1 );
    }
    filter_rasy = gtk_tree_model_filter_new( GTK_TREE_MODEL( store_rasy ), NULL );
    gtk_tree_model_filter_set_visible_column( GTK_TREE_MODEL_FILTER( filter_rasy ), KOL_VIS );

    ZNAJDZ_KONTROLKE2( dprog.store_progi[ 0 ], "prg_store_mprogi", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE2( dprog.store_progi[ 1 ], "prg_store_rprogi", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE2( dprog.store_progi[ 2 ], "prg_store_eprogi", GTK_LIST_STORE );
    ZNAJDZ_KONTROLKE2( dprog.store_progi[ 3 ], "prg_store_oprogi", GTK_LIST_STORE );
    for ( p = 0; p <= 3; p++ )
    {
	g_object_ref( dprog.store_progi[ p ] );
	for ( i = 0; *progi[ p ][ i ].name; i++ )
	{
	    gtk_list_store_append( dprog.store_progi[ p ], &iter );
	    gtk_list_store_set( dprog.store_progi[ p ], &iter, 0, _( progi[ p ][ i ].name ), -1 );
	}
    }


    ZNAJDZ_KONTROLKE2( store_typyprzedmiotow, "store_typyprzedmiotow", GTK_LIST_STORE );
    g_object_ref( store_typyprzedmiotow );
    /* tak na wszelki wypadek, bo trudno takie cos znalezc */
    numer_na_typ[ 0 ] = 0;
    typ_na_numer[ 0 ] = 0;
    for ( p = i = 1; i < ITEM_MAX; i++ )
	if ( *typy_przedmiotow[ i ].name )
	{
	    gtk_list_store_append( store_typyprzedmiotow, &iter );
	    gtk_list_store_set( store_typyprzedmiotow, &iter,
				0, _( typy_przedmiotow[ i ].name_pl ), -1 );

	    numer_na_typ[ p ] = i;
	    typ_na_numer[ i ] = p;
	    p++;
	}
	else
	    numer_na_typ[ i ] = ITEM_TRASH;

    return;
}

static gboolean nie_dotykane( GtkTreeModel *model, GtkTreePath *path,
			     GtkTreeIter *iter, gpointer user_data )
{
    AREA_DATA *kraina;

    gtk_tree_model_get( model, iter, KOL_WSKAZNIK, &kraina, -1 );
    gtk_list_store_set( dlis.store_krain, iter, KOL_NAZWA, kraina->file_name, -1 );

    return FALSE;
}


FUNKCJA_DLA_GTK( zapisz_krainy )
{
    AREA_DATA *kraina;
    struct timeval now_time;

    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    log_string( "Lac zapisuje wczytane krainy..." );
    /* Wygodnie mi testowac w ten sposob zmiany wprowadzone przez Glace.
       Mozna to zmienic dla wszystkich systemow, nie tylko Windows, ale rownie
       dobrze mozna sobie ln -s .  save jesli komus sie nie podoba :) */
    for ( kraina = area_first; kraina; kraina = kraina->next )
    {
	kraina->odnowiona = FALSE;
#if defined( WIN32 )
	save_area( kraina, FALSE );
#else
	save_area( kraina, TRUE );
#endif
    }
    gtk_tree_model_foreach( GTK_TREE_MODEL( dlis.store_krain ), nie_dotykane, NULL );

    return;
}


FUNKCJA_DLA_GTK( skocz_do )
{
    int vnum = atoi( gtk_entry_get_text( dlis.entry_vnum ) );

    if ( vnum < 1 )
	return;

    if ( widget == dlis.button_mob )
    {
	dmob.mob = get_mob_index( vnum );
	dmob.iter = NullIter;
	dialog_moba( NULL, NULL );
    }
    else if ( widget == dlis.button_pom )
    {
	dpom.room = get_room_index( vnum );
	dpom.iter = NullIter;
	dialog_pomieszczenia( NULL, NULL );
    }
    else if ( widget == dlis.button_prz )
    {
	dprz.obj = get_obj_index( vnum );
	dprz.iter = NullIter;
	dialog_przedmiotu( NULL, NULL );
    }

    return;
}


char *wiersz( AREA_DATA *kraina, char *bufor )
{
    char *p;

    bufor[ 9 ] = ' ';
    bufor[ 10 ] = '\0';
    strcpy( bufor, _( kraina->name ) );
    for ( p = bufor + 9; *p != ' '; p++ );
	;
    *p = '\0';
    for ( p++; *p == ' '; p++ );
	;

    return p;
}


FUNKCJA_DLA_GTK( zmien_lancuch )
{
    GtkWidget *entry = GTK_WIDGET( data );
    char **lancuch = gtk_object_get_data( GTK_OBJECT( entry ), "ssm_string" );

    if ( lancuch && *lancuch )
    {
	free_string( *lancuch );
	*lancuch = str_dup( lac_z_pols( ENTRY_GET_TEXT( entry ) ) );
    }

    return;
}


void zmien_lancuch_textbuffer( GtkTextBuffer *buffer )
{
    GtkTextIter start, end;
    char *text;
    char **lancuch;

    gtk_text_buffer_get_bounds( buffer, &start, &end );
    text = gtk_text_buffer_get_text( buffer, &start, &end, FALSE );
    lancuch = g_object_get_data( G_OBJECT( buffer ), "ssm_string" );
    if ( lancuch && *lancuch )
    {
	free_string( *lancuch );
	*lancuch = str_dup( lac_z_pols( text ) );
    }
    g_free( text );

    return;
}


void ile_progow_button( GtkWidget *button, MPROG_DATA *mprg )
{
    if ( mprg )
    {
	int ile = 0;
	char buf[ MIL ];

	for ( ; mprg; mprg = mprg->next )
	    if ( !mprg->parent )
		ile++;

	sprintf( buf, "Progi: %d", ile );
	gtk_button_set_label( GTK_BUTTON( button ), _( buf ) );
    }
    else
	gtk_button_set_label( GTK_BUTTON( button ), _( "(Progi)" ) );

    return;
}


void ile_opisow_label( GtkWidget *label, EXTRA_DESCR_DATA *ed )
{
    if ( ed )
    {
	char buf[ MIL ], *bufor;

	strcpy( buf, "Dodatkowe opisy: " );
	bufor = buf + strlen( "Dodatkowe opisy: " );
	for ( ; ed; ed = ed->next )
	    bufor = bufor + sprintf( bufor, "\"%s\" ", ed->keyword );
	gtk_label_set_text( GTK_LABEL( label ), _( buf ) );
    }
    else
	gtk_label_set_text( GTK_LABEL( label ), _( "(Dodatkowe opisy)" ) );

    return;
}


void glaca_widget_set_visible( GtkWidget *widget, gboolean show )
{
    if ( show )
	gtk_widget_show( widget );
    else
	gtk_widget_hide( widget );

    return;
}


GdkWindow *glaca_widget_get_window( GtkWidget *widget )
{
    if ( gtk_check_version( 2, 14, 0 ) )
	return gtk_widget_get_window( widget );
    else
	return widget->window;
}


void on_size_request( GtkWidget *widget, GtkRequisition *requisition, gpointer data )
{
    GtkWidget *parent = gtk_widget_get_parent( widget );
    GtkPolicyType hpolicy;

    if ( !parent || !GTK_IS_SCROLLED_WINDOW( parent ) )
	return;

    hpolicy = ( requisition->height > 0 ) ? GTK_POLICY_ALWAYS : GTK_POLICY_NEVER;

    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( parent ),
	    GTK_POLICY_AUTOMATIC, hpolicy );

    return;
}
