/*
 *Malven: klanowe zmienne globalne w const.c
 */
extern const char                 *clan_lev_name_mm[ 10 ];
extern const char                 *clan_lev_name_zm[ 10 ];
extern const char                 *clan_lev_name_md[ 10 ];
extern const char                 *clan_lev_name_zd[ 10 ];
extern const char                 *clan_lev_name_mn[ 10 ];
extern const char                 *clan_lev_name_zn[ 10 ];
extern const char                 *clan_stat_name[ 5 ];
extern const int                  max_clan_level_members[ 10 ];

extern const char                 *tablica_kolorow[ 4 ][ MAX_COLOUR ];
extern const char                 *polska_tablica[ 5 ][ 18 ];

extern const char                 *day_name[ ];
extern const char                 *month_name[ ];
extern const struct holiday_type  holiday_table[ ];

extern const struct str_app_type  str_app[ 31 ];
extern const struct int_app_type  int_app[ 31 ];
extern const struct wis_app_type  wis_app[ 31 ];
extern const struct dex_app_type  dex_app[ 31 ];
extern const struct con_app_type  con_app[ 31 ];

extern const struct class_type    class_table[ MAX_CLASS ];
extern const int                  dir_order[ MAX_DIR ];
extern const struct dir_type      kierunki[ MAX_DIR ];
extern const struct object_type   typy_przedmiotow[ ITEM_MAX ];
extern const struct liq_type      liq_table_pl[ LIQ_MAX ];
extern const char                 *typy_podloza[ ];
extern struct skill_type          skill_table[ MAX_SKILL ];
extern char * const               title_table[ MAX_CLASS ][ 2 ];
extern const struct material_type material_table[ MAX_MATERIAL ];
/* extern struct koles_type          koles_table[ MAX_TRACK ]; */
extern const struct race_type     race_table[ MAX_RACE ];
extern struct czesc_ciala         czesci_ciala[ ];
extern const struct struckdrunk   drunk[ ];
extern const int                  przelicznik_zysku[ 112 ];
extern const int                  przelicznik_wydatkow[ 112 ];

extern char * const               lac_nowosci;
