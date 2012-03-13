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
 *  Odpowiednik merc.h dla GLACY                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "../merc.h"
#include <gtk/gtk.h>
#include <string.h>
#if !defined( BEZ_GTKSPELL )
# include <gtkspell/gtkspell.h>
#endif

/* _( ) dla gettexta uzywam do tlumaczenia na unikod, jesli trzeba */
# define _( a ) pols_z_lac( a )

/* glaca.c */
/* takiej postaci musza byc funkcje do laczenia z nimi sygnalow */
#define FUNKCJA_DLA_GTK( f )	G_MODULE_EXPORT void f( GtkWidget *widget, gpointer data )
#define FUNKCJA_DLA_GTK_2( f )	G_MODULE_EXPORT gint f( GtkWidget *widget, GdkEvent *event, gpointer data )

#define USTAW_CHECKBOXA( w, g, f ) \
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( w ), IS_SET( g, f ) )
#define USTAW_FLAGE( w, g, f ) \
	if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( w ) ) ) \
		SET_BIT( g, f ); \
	else	REMOVE_BIT( g, f );

#define USTAW_CHECKBOXA_V( w, g, f ) \
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( w ), IS_SET_V( g, f ) )
#define USTAW_FLAGE_V( w, g, f ) \
	if ( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( w ) ) ) \
		SET_BIT_V( g, f ); \
	else	REMOVE_BIT_V( g, f );

/* Teraz robie to sobie tak: */
#define ZROB_LANCUCH( entry, ssm_string ) \
    gtk_object_set_data( GTK_OBJECT( entry ), "ssm_string", &ssm_string ); \
    gtk_entry_set_text( GTK_ENTRY( entry ), _( ssm_string ) );
#define WEZ_LANCUCH( entry ) \
    zmien_lancuch( entry, entry );
#define PUSC_LANCUCH( entry ) \
    gtk_object_remove_data( GTK_OBJECT( entry ), "ssm_string" ); \
    gtk_entry_set_text( GTK_ENTRY( entry ), "" );

#define ZROB_LANCUCH_TEXT( buffer, ssm_string ) \
	g_object_set_data( G_OBJECT( buffer ), "ssm_string", &ssm_string ); \
	gtk_text_buffer_set_text( buffer, _( ssm_string ), -1 );
#define WEZ_LANCUCH_TEXT( buffer ) \
	zmien_lancuch_textbuffer( buffer );
#define PUSC_LANCUCH_TEXT( buffer ) \
	g_object_steal_data( G_OBJECT( buffer ), "ssm_string" ); \
	gtk_text_buffer_set_text( buffer, "", -1 );

/* glade ma ten feler, ze moze sie zmienic wszystko */
#define ZNAJDZ_KONTROLKE( zmienna, nazwa )	\
do { if ( !( zmienna = GTK_WIDGET( gtk_builder_get_object( glacaxml, nazwa ) ) ) ) \
    {	printf( "Blad glaca.glade? Kontrolka: %s\n\r", nazwa );	\
	exit( 1 );    } } while ( 0 )

#define ZNAJDZ_KONTROLKE2( zmienna, nazwa, typ )	\
do { if ( !( zmienna = typ( gtk_builder_get_object( glacaxml, nazwa ) ) ) ) \
    {	printf( "Blad glaca.glade? Kontrolka: %s\n\r", nazwa );	\
	exit( 1 );    } } while ( 0 )

#define SPIN_GET_VALUE( spin )	\
	gtk_spin_button_get_value_as_int( GTK_SPIN_BUTTON( spin ) )

#define ENTRY_GET_TEXT( entry )	\
	gtk_entry_get_text( GTK_ENTRY( entry ) )

#define COMBO_GET_ACTIVE( combo )	\
	gtk_combo_box_get_active( GTK_COMBO_BOX( combo ) )

#define COMBO_SET_ACTIVE( combo, index )	\
	gtk_combo_box_set_active( GTK_COMBO_BOX( combo ), index )

#define SPIN_SET_VALUE( spin, value )	\
	gtk_spin_button_set_value( GTK_SPIN_BUTTON( spin ), value )

#define CHECK_GET_ACTIVE( button )	\
	gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( button ) )

#define CHECK_SET_ACTIVE( button, value )	\
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( button ), value )

#define KOL_WSKAZNIK 0
#define KOL_NAZWA 1
#define KOL_VNUM 1
#define KOL_VIS 2
#define KOL_OPIS 2
#define KOL_OPIS2 3
#define KOL_ELEMENT 3

/*
 * Tu konfiguruje sie standard polskich liter (domyslnie LIN)
 */
#define DOS		1
#define LIN		2
#define WIN		3
#define STANDARD	LIN

/* glaca.c */
struct dlis_data
{
    GtkWidget *okno_lista_krain;
    GtkWidget *lista_krain;
    GtkListStore *store_krain;
    GtkTreeSelection *selection;
    GtkWidget *button_mob;
    GtkWidget *button_pom;
    GtkWidget *button_prz;
    GtkEntry *entry_vnum;
}; /* lista krain */

extern struct dlis_data dlis;

struct dkra_data
{
    GtkWidget *okno_Krainy;
    GtkWidget *entry_name;
    GtkWidget *entry_fullname;
    GtkWidget *entry_resetmsg;
    GtkWidget *spin_recall;
    GtkWidget *button_zapisz;
    GtkWidget *button_przywroc;
    GtkWidget *entry_wersja;
    GtkWidget *spin_strefa;
    GtkWidget *check_nielista;
    GtkWidget *check_nieteleport;
    GtkToggleButton *check_strefa[ 32 ];
    GtkTextBuffer *buffer_opis;
    GtkTreeView *tree_pom;
    GtkTreeView *tree_mob;
    GtkTreeView *tree_prz;
    GtkTreeView *tree_res;
    GtkListStore *store_pom;
    GtkListStore *store_mob;
    GtkListStore *store_prz;
    GtkTreeStore *store_res;
    GtkTreeSelection *selection_pom;
    GtkTreeSelection *selection_mob;
    GtkTreeSelection *selection_prz;
    GtkTreeSelection *selection_res;
    GtkWidget *button_upom;
    GtkWidget *button_umob;
    GtkWidget *button_uprz;
}; /* dialog krainy */

extern struct dkra_data dkra;

void zrob_okno_Krainy( void );

extern AREA_DATA *area;
extern GtkBuilder *glacaxml;
extern GtkTreeIter NullIter;

extern int typ_na_numer[ ITEM_MAX ];
extern int numer_na_typ[ ITEM_MAX ];

extern GtkListStore *store_typyprzedmiotow;

extern GtkListStore *store_pomieszczenia;
extern GtkTreeModel *filter_pom;

extern GtkListStore *store_moby;
extern GtkTreeModel *filter_mob;

extern GtkListStore *store_przedmioty;
extern GtkTreeModel *filter_prz;

extern GtkListStore *store_czary;
extern GtkTreeModel *filter_czary;

extern GtkListStore *store_materialy;
extern GtkTreeModel *filter_materialy;

extern GtkListStore *store_plyny;
extern GtkTreeModel *filter_plyny;

extern GtkListStore *store_rasy;
extern GtkTreeModel *filter_rasy;

extern PangoFontDescription *font_desc;

char *pols_z_lac( const char *wej );
char *pols_z_lac_len( const char *wej, int len );
char *pols_z_lac_gtk_text( const char *wej );
char *lac_z_pols( const char *wej );
bool str_infix_uni( const char *astr, const char *bstr );


FUNKCJA_DLA_GTK( dialog_krainy );
FUNKCJA_DLA_GTK( zmien_lancuch );
void zmien_lancuch_textbuffer( GtkTextBuffer *buffer );
void ile_progow_button( GtkWidget *button, MPROG_DATA *mprg );
void ile_opisow_label( GtkWidget *label, EXTRA_DESCR_DATA *ed );
void glaca_widget_set_visible( GtkWidget *widget, gboolean show );
GdkWindow *glaca_widget_get_window( GtkWidget *widget );
char *wiersz( AREA_DATA *kraina, char *bufor );


/* pomieszczenie.c */
struct dpom_data
{
    ROOM_INDEX_DATA *room;
    unsigned int check_na_flage[ 32 ];

    GtkTreeIter iter; /* ktory wpis na liscie pomieszczen uaktualnic */
    GtkWidget *Pomieszczenie;
    GtkWidget *entry_nazwa;
    GtkWidget *entry_miejscownik;
    GtkWidget *combobox_podloze;
    GtkWidget *spin_pojemnosc;
    GtkWidget *spin_ruch;
    GtkWidget *entry_antirace;
    GtkWidget *entry_anticlass;
    GtkWidget *button[ 10 ];
    GtkWidget *label[ 10 ];
    GtkWidget *label_wyjscie;
    GtkWidget *label_flagi;
    GtkWidget *button_progi;
    GtkWidget *button_opisy;
    GtkWidget *label_opisy;
    GtkWidget *button_poprz;
    GtkWidget *button_nast;
    GtkWidget *check[ 32 ];
    GtkTextBuffer *buffer;
    GtkTreeSelection *select_reset;
    GtkListStore *store_resety;
    GtkWidget *vbox_pulapka;
    GtkRadioButton *radio_pulapka1;
    GtkRadioButton *radio_pulapka2;
    GtkTextBuffer *buffer_pulapka;
    GtkCheckButton *check_pulapka;
}; /* dialog pomieszczenia */

extern struct dpom_data dpom;

FUNKCJA_DLA_GTK( dialog_pomieszczenia );
void zrob_okno_Pomieszczenia( void );
void aktualizuj_etykietki_wyjsc( void );
FUNKCJA_DLA_GTK( pomieszczenie_z_listy );

/* mob.c */
struct dmob_data
{
    MOB_INDEX_DATA *mob;
    unsigned int check_na_aff[ MAX_AFF_FLAG + 1 ];
    unsigned int check_na_act[ 32 ];
    int czary[ 5 ];
    int rasa;

    HEALER_DATA pHealer_pusty;
    SHOP_DATA pShop_pusty;

    GtkWidget *Mob;
    GtkWidget *entry_name;
    GtkWidget *entry_short;
    GtkWidget *entry_dopelniacz;
    GtkWidget *entry_celownik;
    GtkWidget *entry_biernik;
    GtkWidget *entry_narzednik;
    GtkWidget *entry_miejscownik;
    GtkWidget *entry_wolacz;
    GtkWidget *entry_long;
    GtkWidget *combo_plec;
    GtkWidget *label_rasa;
    GtkWidget *spinbutton_poziom;
    GtkWidget *spinbutton_moralnosc;
    GtkWidget *spinbutton_zamoznosc;
    GtkWidget *combo_spec;
    GtkWidget *check_sklep;
    GtkWidget *check_czary;
    GtkWidget *button_progi;
    GtkWidget *label_zachowania;
    GtkWidget *label_wplywy;
    GtkWidget *checkaff[ MAX_AFF_FLAG + 1 ];
    GtkWidget *checkact[ 32 ];
    GtkWidget *combo_sklepS[ 5 ];
    GtkLabel *label_sklepH[ 5 ];
    GtkWidget *spin_sklepH[ 5 ];
    GtkWidget *toggle_sklepH[ 5 ];
    GtkWidget *spin_marzaK;
    GtkWidget *spin_marzaS;
    GtkWidget *spin_czynneOd;
    GtkWidget *spin_czynneDo;
    GtkWidget *spin_wplywA;
    GtkWidget *spin_grupa;
    GtkWidget *combo_krupier;
    GtkWidget *table_sklep;
    GtkWidget *table_czary;
    GtkWidget *button_poprz;
    GtkWidget *button_nast;
    GtkTextBuffer *buffer;
    GtkTreeIter iter;
    GtkTreeSelection *select_reset;
    GtkListStore *store_resety;
}; /* dialog moba */

extern struct dmob_data dmob;

void zrob_okno_Moba( void );
FUNKCJA_DLA_GTK( dialog_moba );
FUNKCJA_DLA_GTK( mob_z_listy );

/* vnum.c */
struct dvnum_data
{
    GtkWidget *Vnum;
    GtkWidget *spin;
    GtkWidget *label;
    GtkWidget *button_tak;
    int typ;
};

extern struct dvnum_data dvnum;

void zrob_okno_Vnuma( void );

FUNKCJA_DLA_GTK( utworz_vnuma );

/* mapa.c */
struct dmapa_data
{
    ROOM_INDEX_DATA *room;

    PangoLayout *layout;
    GtkWidget *Mapa;
    GtkWidget *drawingarea;
    GdkPixmap *pixmap;
};

extern struct dmapa_data dmapa;

void zrob_okno_Mapy( void );
void maluj_mape( GtkWidget *widget );

/* wyjscie.c */
struct dwyj_data
{
    EXIT_DATA *exit, *nowe;
    ROOM_INDEX_DATA *room;
    int door;
    unsigned int check_na_flage[ 32 ];

    GtkWidget *Wyjscie;
    GtkWidget *etykietka;
    GtkWidget *entry_opis;
    GtkWidget *entry_keywords;
    GtkWidget *entry_mianownik;
    GtkWidget *entry_biernik;
    GtkWidget *spin_klucz;
    GtkWidget *spin_to_room;
    GtkWidget *combo_rodzaj;
    GtkWidget *check[ 32 ];
    GtkWidget *label_flagi;
    GtkWidget *button_progi;
    GtkWidget *combo_akcja;
    GtkWidget *hbox_opis;
    GtkWidget *hbox_pola;
    GtkWidget *label_wyjscie;
    GtkWidget *label_klucz;
    GtkListStore *store_akcjanowe;
    GtkListStore *store_akcja;
};

extern struct dwyj_data dwyj;

void zrob_okno_Wyjscia( void );
void dialog_wyjscia( ROOM_INDEX_DATA *room, int door );

/* progi.c */
struct dprog_data
{
    int gdzie; /* 0 mob, 1 pom, 2 wyj, 3 prz, 4 dod. opis */
    MPROG_DATA *progi;
    EXTRA_DESCR_DATA *opisy;
    bool nie_zmieniaj;
    GtkTreePath *del_ancestor;

    GtkWidget *Prog;
    GtkWidget *combobox;
    GtkWidget *entry;
    GtkWidget *button_dodaj;
    GtkWidget *button_usun;
    GtkWidget *button_przywroc;
    GtkWidget *button_gotowe;
    GtkWidget *textview;
    GtkTreeStore *treestore;
    GtkListStore *store_progi[ 4 ];
    GtkTreeSelection *select;
    GtkTextBuffer *buffer;
    GtkWidget *label_kur;
};

extern struct dprog_data dprog;

void zrob_okno_Progow( void );

/* reset.c */
struct dres_data
{
    RESET_DATA *reset;
    int wear_na_item[ MAX_WEAR ];
    int miejsce;
    GtkTreeIter iter;

    GtkWidget *Reset;
    GtkWidget *spin_mob;
    GtkWidget *spin_pom;
    GtkWidget *spin_prz;
    GtkLabel *label_mob;
    GtkLabel *label_pom;
    GtkLabel *label_prz;
    GtkWidget *spin_int;
    GtkLabel *label_int;
    GtkWidget *button_poprz;
    GtkWidget *button_nast;
    GtkWidget *hbox_mob;
    GtkWidget *hbox_prz;

    GtkWidget *frame_inw;
    GtkLabel *label_inw;
    GtkWidget *button_inw_usun;
    GtkTreeStore *store_inw;
    GtkTreeView* treeview_inw;
    GtkTreeSelection *select_inw;
    GtkWidget *spin_inw_dodaj;
    GtkWidget *toggle_inw_dodaj;
    GtkLabel *label_inw_dodaj;
    GtkWidget *button_inw_dodaj;

    GtkWidget *frame_ekw;
    GtkWidget *spin_ekw[ MAX_WEAR ];
    GtkWidget *toggle_ekw[ MAX_WEAR ];
    GtkLabel *label_ekw[ MAX_WEAR ];
    GtkTreeIter iter_ekw[ MAX_WEAR ];
};

extern struct dres_data dres;

void zrob_okno_Resetu( void );
FUNKCJA_DLA_GTK( dialog_resetu );
FUNKCJA_DLA_GTK( reset_z_listy );

/* przedmiot.c */
struct dprz_data
{
    OBJ_INDEX_DATA *obj;

    unsigned int check_na_flage[ ITEM_MAX_EXTRA_FLAG + 1 ];
    int nr_na_wplyw[ 32 ]; /* nadlozylem duzo */
    int wartosci[ 3 ][ 6 ];

    GtkWidget *Przedmiot;
    GtkWidget *entry_name;
    GtkWidget *entry_short;
    GtkWidget *entry_dopelniacz;
    GtkWidget *entry_celownik;
    GtkWidget *entry_biernik;
    GtkWidget *entry_narzednik;
    GtkWidget *entry_miejscownik;
    GtkWidget *entry_descr;
    GtkTextBuffer *buffer;

    GtkWidget *combo_rodzaj;
    GtkWidget *combo_typ;
    GtkWidget *spin_wielk;
    GtkWidget *spin_waga;
    GtkWidget *combo_ubr;

    GtkLabel *label_w[ 6 ];
    GtkWidget *spin_w[ 6 ];
    GtkWidget *entry_w[ 6 ];
    GtkLabel *label_toggle[ 6 ];
    GtkWidget *toggle_w[ 6 ];

    GtkWidget *checkextra[ ITEM_MAX_EXTRA_FLAG + 1 ];
    GtkWidget *label_flagi;

    GtkTreeSelection *select_reset;
    GtkListStore *store_resety;

    GtkTreeSelection *select_app;
    GtkListStore *store_app;
    GtkWidget *spin_app;
    GtkWidget *combo_app;
    GtkWidget *button_app_usun;

    GtkWidget *entry_aras;
    GtkWidget *entry_aprof;
    GtkWidget *entry_action;
    GtkWidget *spin_cena;
    GtkWidget *spin_poz;
    GtkWidget *spin_qp;

    GtkWidget *button_poprz;
    GtkWidget *button_nast;

    GtkWidget *button_progi;
    GtkTreeIter iter;
};

extern struct dprz_data dprz;

void zrob_okno_Przedmiotu( void );
FUNKCJA_DLA_GTK( dialog_przedmiotu );
FUNKCJA_DLA_GTK( przedmiot_z_listy );

/* combo.c */
struct dcom_data
{
    bool wybrany; /* czy zostal wybrany jakis element */
    GtkToggleButton *button; /* przycisk, ktory wywolal okienko */
    GtkAllocation wymiary;
    GtkTreeIter iter;
    GtkTreeModelForeachFunc filter_function;
    GtkListStore *store;
    GtkTreeModel *filter;
    gchar *text;

    GtkWidget *Dialog;
    GtkEntry *entry;
    GtkTreeView *treeview;
    GtkTreeSelection *select;
    GtkWidget *eventbox;
}; /* okno z lista dla naszych combo */

extern struct dcom_data dcom;

void zrob_dialog_Combo( void );
void combo_wyskocz( GtkTreeModel *filter, GtkWidget *button, GtkTreeModelForeachFunc func );

