extern DESCRIPTOR_DATA     *descriptor_list;

extern time_t              current_time;
extern FILE                *fpReserve;
extern FILE                *fpBugReserve;

extern char                str_boot_time[ ];

extern struct stats_type   max_play;
extern int                 max_play_all_time;
extern char                max_play_all_time_time[ 200 ];
extern struct stats_type   max_desc;
extern int                 max_desc_all_time;
extern char                max_desc_all_time_time[ 200 ];

extern bool                merc_down;
extern int                 numlock;
extern bool                TylkoKonwersja;
extern bool                wizlock;
extern unsigned const char echo_on_str[ ];
extern unsigned const char echo_off_str[ ];
extern unsigned const char keepalive_str[ ];

extern bool                imud_down; /* Lam */
