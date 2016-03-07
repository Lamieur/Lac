extern char               *daPrompt;
extern bool               fBootDb;
extern int                top_mob_index;
extern int                top_obj_index;
extern int                top_trap;


extern MOB_INDEX_DATA     *mob_index_hash[ MAX_KEY_HASH ];
extern OBJ_INDEX_DATA     *obj_index_hash[ MAX_KEY_HASH ];
extern ROOM_INDEX_DATA    *room_index_hash[ MAX_KEY_HASH ];

extern ZONE_DATA          *zone_first;
extern AREA_DATA          *area_first;
extern AREA_DATA          *area_czysciec; /* Lam */
extern HELP_DATA          *help_first;

extern SCHEMAT_DATA       *schemat_list;
extern CHAR_DATA          *char_list;
extern NOTE_DATA          *note_list;
extern CLAN_DATA          *clan_list;
extern OBJ_DATA           *object_list;
extern SORTED_SKILL_DATA  *sorted_skill_list;
extern KOLES_DATA         *lista_kolesi;

extern AFFECT_DATA        *affect_free;
extern NOTE_DATA          *note_free;
extern CLAN_DATA          *clan_free;
extern CLAN_MEMBER_DATA   *clan_member_free;
extern CLAN_REL_DATA      *clan_rel_free;
extern OBJ_DATA           *obj_free;
extern FIGHT_DATA         *fight_free;
extern ZWOD_DATA          *zwod_free;

extern int                top_fight;
extern int                top_clan;
extern int                top_clan_member;
extern int                top_clan_rel;
extern int                top_stosmp;
extern int                top_zmienna;

extern KILL_DATA          kill_table[ ];
extern char               bug_buf[ ];
extern char               log_buf[ ];
extern char               posdead_buf[ ];
extern long               mud_age;
/*ponizsze trzymaja czas Midgaardu */
extern TIME_INFO_DATA     time_info;
extern WEATHER_DATA       weather_info;
extern time_t             down_time;
extern time_t             warning1;
extern time_t             warning2;
extern bool               Reboot;

extern AUCTION_DATA       *auction;
extern char               *help_greeting;
extern char               *help_namehelp;

extern char               lsc_init_str[ MAX_INPUT_LENGTH ];
extern char               lsc_delimiter;
extern char               lsc_clear_str[ MAX_INPUT_LENGTH ];
extern unsigned int       lsc_init_str_len;

extern char               *hint_table[ MAX_HINT ];
extern char               *quit_table[ MAX_QUIT ];
extern char               *offensive_table[ MAX_OFFENSIVE ];
extern struct miodek_data miodek_table[ MAX_OFFENSIVE ];
/*
extern char               *maskowanie_table[ MAX_MASKOWANIE ];
extern int                maskowanie_count;
*/
extern int                quit_count;
extern struct pose_data   pose_table[ MAX_CLASS ][ MAX_POSE ];
extern int                pose_count[ MAX_CLASS ];
extern int                skill_count[ MAX_CLASS ];
extern int                multi_count[ MAX_CLASS ];
extern int                ilosc_czesci_ciala[ MAX_RACE ];
extern struct powody_data powody[ MAX_POWODY ];
extern IMIONA_DATA        *imiona[ MAX_DLUG_IMIENIA + 1 ][ 64 ];
extern bool               ODPLUSKWIACZ;
extern bool               PRZERABIACZ;
extern char               HTTPD_log[ MIL ];
extern FILE               *httpdlog;
