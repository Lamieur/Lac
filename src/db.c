/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   ___           _______     ________      |                             *
 *  |###|         |#######|   |########| (R) |             Lac             *
 *  |###|        |###|^|###| |###|^^|###|    |                             *
 *  |###|        |###| |###| |###|   ^^^     |    ekipa programistyczna:   *
 *  |###|        |###|_|###| |###|           |   Lam, Qwert, Ulryk, Vigud  *
 *  |###|        |#########| |###|           |      Tissarius, Cadaver     *
 *  |###|        |###|^|###| |###|           |        Alandar, Gimza       *
 *  |###|        |###| |###| |###|   ___     |    trzy grosze dolozyli:    *
 *  |###|______  |###| |###| |###|__|###|    |      Varda, Nop, Fuyara     *
 *  |##########| |###| |###|  |########|     |     Albi, Malven, Muzgus    *
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


#include "merc.h"

#if !defined( ultrix ) && !defined( apollo ) && !defined( __minix ) && !defined( PLAN9 )
# include <memory.h>
#endif

/* Lam 20.1.2004 */
#if defined( BACKTRACE )
# include <execinfo.h>
#endif

#if defined( PRINTSTACK )
# include <ucontext.h>
#endif

#if !defined( NO_STRERROR_R )
# include <errno.h>
#endif

/*
 * Lam 21.11.2000: Wersje krain
 */
struct
{
    char *nazwa;
    bool plusy;		/* plusy w mianowniku oznaczajace obsluge deklinacji */
    bool stare_flagi;	/* stare flagi i nieuzywane atrybuty w mobach */
    bool oswietlenie;	/* oswietlenie pomieszczenia */
    bool wielkosc;	/* wielkosc przedmiotow */
    bool wolacz;	/* wolacze mobow */
    bool pommiej;	/* miejscownik pomieszczenia do "gdzie" */
    bool dodkp;		/* dodatnia klasa pancerza */
    bool zbres;		/* bez zbednych liczb w #RESETS */
    bool pozwo;		/* poziom w resecie 'O' */
    bool entery;	/* zbedne entery w jednoliniowych opisach */
    bool opisprz;	/* opis przedmiotu zamiast E, w miejscu action */
    int  wartosci;	/* ilosc wartosci przedmiotow */
}
wersje_krain[ ] =
{
/*    nazwa              +  st osw wlk wol nrz dkp res pzo ent opi, wart */
    { "Lac-1.4.1",	 0,  0,  1,  1,  1,  1,  1,  1,  1,  0,  1,  6 },
    { "Lac-1.4.0-pre4",	 0,  0,  1,  1,  1,  1,  1,  1,  1,  0,  1,  6 },
    { "Lac-1.4.0-pre3",	 0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  6 },
    { "Lac-1.4.0-pre2",	 0,  0,  1,  1,  1,  1,  1,  1,  0,  1,  0,  6 },
    { "Lac-1.4.0-pre1",	 0,  0,  1,  1,  1,  1,  1,  0,  0,  1,  0,  6 },
    { "Lac-1.4.-1",	 0,  0,  1,  1,  1,  0,  1,  0,  0,  1,  0,  6 },
    { "Lac-1.4.-2",	 0,  0,  1,  1,  1,  0,  0,  0,  0,  1,  0,  6 },
    { "Lac-1.4.-3",	 0,  0,  1,  1,  0,  0,  0,  0,  0,  1,  0,  6 },
    { "Lac-1.4.-4",	 0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  6 },
    { "Lac-1.3",	 1,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0,  4 },
    { "Envy-2.0",	 1,  1,  0,  0,  0,  0,  0,  0,  0,  1,  0,  4 }
};
#define DOMYSLNA_WERSJA 0 /* Lac-1.4.1 */

/*
 * Lam 5.12.2004: polecenie "przeladuj" i funkcje z nim zwiazane
 */
struct zapamietana_kraina
{
    AREA_DATA *kraina;
    MOB_INDEX_DATA *moby;
    OBJ_INDEX_DATA *przedmioty;
    ROOM_INDEX_DATA *pomieszczenia;
    RESET_DATA *resety;
};

/*
 * Globals.
 */
AUCTION_DATA		*auction;

HELP_DATA		*help_first;
HELP_DATA		*help_last;

CHAR_DATA		*first_free_char;
CHAR_DATA		*last_free_char;
EXTRA_DESCR_DATA	*extra_descr_free;
MPROG_DATA		*mprog_free;
AFFECT_DATA		*affect_free;
ALIAS_DATA		*alias_free;		/* Lam */
REFUSE_DATA		*refuse_free;		/* Gimza */
TELL_DATA		*tell_free;		/* Lam */
MPQUEST_DATA		*mpquest_free;		/* Lam */
NOTE_DATA		*note_free;
CLAN_DATA		*clan_free;		/* Lam */
CLAN_MEMBER_DATA	*clan_member_free;	/* Malven */
CLAN_REL_DATA		*clan_rel_free;		/* Malven */
OBJ_DATA		*obj_free;
PC_DATA			*pcdata_free;
FIGHT_DATA		*fight_free;		/* Ulryk */
EXIT_DATA		*exit_free;		/* Lam */
RESET_DATA		*reset_free;		/* Vigud */
ZWOD_DATA		*zwod_free;		/* Lam */
IMIONA_DATA		*imiona_wolne;		/* Lam */

CHAR_DATA		*char_list;
char			*help_greeting;
char			*help_namehelp; /* pomoc przy wybieraniu imion */
char			bug_buf			[ MAX_INPUT_LENGTH * 2 ];
char			log_buf			[ MAX_INPUT_LENGTH * 2 ];
char			posdead_buf		[ MAX_STRING_LENGTH ];
KILL_DATA               kill_table              [ MAX_LEVEL ];
NOTE_DATA		*note_list;
SCHEMAT_DATA		*schemat_list;
CLAN_DATA		*clan_list;
OBJ_DATA		*object_list;
SORTED_SKILL_DATA	*sorted_skill_list;
/* Lam 10.3.2003: mud_age, wiek muda, pamieta liczbe godzin od 1 dnia miesiaca
		zimy 1 roku (czas 0, mudowy poczatek epoki) */
long			mud_age;
TIME_INFO_DATA          time_info;
WEATHER_DATA            weather_info;
time_t                  down_time;
time_t                  warning1;
time_t                  warning2;
bool                    Reboot;
char *			hint_table[ MAX_HINT ];	/* Lam */
char *			quit_table[ MAX_QUIT ]; /* Lam */
char *			offensive_table[ MAX_OFFENSIVE ]; /* Lam */
struct miodek_data	miodek_table[ MAX_MIODEK ]; /* Lam */
/*char *		maskowanie_table[ MAX_MASKOWANIE ];
int			maskowanie_count;*/
int			quit_count;
/* struct	koles_type	koles_table[ MAX_TRACK ]; zastapione */
KOLES_DATA *		lista_kolesi;		/* Lam */
struct pose_data	pose_table[ MAX_CLASS ][ MAX_POSE ];
int			pose_count[ MAX_CLASS ];
int			skill_count[ MAX_CLASS ];
int			multi_count[ MAX_CLASS ];
int			ilosc_czesci_ciala[ MAX_RACE ];
struct	powody_data	powody[ MAX_POWODY ];
IMIONA_DATA *		imiona[ MAX_DLUG_IMIENIA + 1 ][ 64 ];
IMIONA_DATA *		imiona_ost[ MAX_DLUG_IMIENIA + 1 ][ 64 ];
FILE *			httpdlog;

int                     gsn_backstab;
int                     gsn_berserk;            /* by Thelonius */
int			gsn_blokowanie_tarcza;	/* Lam */
int                     gsn_breathe_water;      /* by Thelonius */
int                     gsn_burning_hands;
int			gsn_cause_light;
int                     gsn_change;             /* Lam */
int                     gsn_circle;             /* by Thelonius */
int                     gsn_cone_of_silence;
int                     gsn_disarm;
int                     gsn_dodge;
int			gsn_enhanced_whois;	/* Lam */
int                     gsn_fast_flee;		/* Lam */
int                     gsn_fast_healing;	/* Lam */
int			gsn_fetch;		/* Lam */
int                     gsn_flamestrike;
int                     gsn_fourth_attack;	/* Lam */
int			gsn_haggle;		/* Lam */
int                     gsn_hide;
int			gsn_hunt;		/* z Wurma */
int			gsn_krwawe_lzy;		/* Lam */
int			gsn_kontrola_nad_woda;	/* Lam */
int			gsn_kumulacja_mocy;	/* Qwert */
int			gsn_longscan;		/* Lam */
int			gsn_meditation;		/* Lam */
int                     gsn_no_trip;		/* Lam */
int			gsn_overhear;		/* Lam */
int			gsn_pajeczyna;
int                     gsn_peek;
int                     gsn_pick_lock;
int			gsn_plywanie;		/* Lam */
int                     gsn_poison_weapon;      /* by Thelonius */
int			gsn_profesjonalizm;
int                     gsn_scrolls;            /* by Binky / Thelonius */
int                     gsn_snare;              /* by Binky / Thelonius */
int                     gsn_sneak;
int                     gsn_staves;             /* by Binky / Thelonius */
int                     gsn_steal;
int                     gsn_strong_wield;	/* Lam */
int			gsn_taranowanie;	/* Lam */
int			gsn_telepathy;		/* Lam */
int                     gsn_untangle;           /* by Thelonius */
int                     gsn_wands;              /* by Binky / Thelonius */

int                     gsn_bash;
int                     gsn_dual;               /* by Thelonius */
int                     gsn_enhanced_damage;
int                     gsn_kick;
int                     gsn_parry;
int                     gsn_rescue;
int                     gsn_second_attack;
int                     gsn_third_attack;
int                     gsn_fury;               /* Qwert */

int                     gsn_blindness;
int			gsn_dazzle;		/* Lam */
int                     gsn_charm_person;
int                     gsn_curse;
int                     gsn_invis;
int                     gsn_mass_invis;
int                     gsn_mute;               /* by Thelonius */
int			gsn_podglad;		/* Lam */
int                     gsn_poison;
int                     gsn_sleep;
int                     gsn_polymorph_other;
int                     gsn_turn_undead;
int                     gsn_rozpalanie_ognisk;  /* Ulryk */
int			gsn_blokada_mentalna;	/* Ulryk */


/*
 * Psionicist gsn's (by Thelonius).
 */
int                     gsn_chameleon;
int                     gsn_domination;
int                     gsn_heighten;
int                     gsn_shadow;

/*
 * Fuyara: skrytobojca
 */
int			gsn_anatomy;
int			gsn_appraise;
int			gsn_assasinate;
int			gsn_assemble;
int			gsn_awareness;
int			gsn_concentrate;
int			gsn_critical_hit;
int			gsn_cumulate_strength;
int			gsn_death_tech;
int			gsn_disguise;
int			gsn_dragon_tech;
int			gsn_enhanced_kick;
int			gsn_flash;
int			gsn_high_efficiency;
int			gsn_initiation;
int			gsn_jump;
int			gsn_mighty_hands;
int			gsn_power_tech;
int			gsn_pull;
int			gsn_reinforce_blade;
int			gsn_runes;
int			gsn_rune_flare;
int			gsn_rune_fire;
int			gsn_rune_ice;
int			gsn_rune_poison;
int			gsn_search;
int			gsn_speed_tech;
int			gsn_strike;
int			gsn_stun;
int			gsn_whirl;
int			gsn_wraith_morph;
int			gsn_walka_na_oslep;

int                     gsn_stake;

/*
 * Race gsn's (by Kahn).
 */
int                     gsn_vampiric_bite;
int			gsn_postac_nietoperza;

/*
 * Spadanie
 */
int	gsn_uderzenie_w;
int	gsn_uderzenie_o;
int	gsn_upadek;

/*
 * Qwert
 */
int	gsn_aura_pokoju;
int     gsn_duchowe_wsparcie;
int     gsn_bio_przyspieszenie;
int     gsn_przeniesienie_sily;
int     gsn_ostrze_ognia;
int     gsn_ostrze_chaosu;
int     gsn_ostrze_lodu;
int     gsn_przeniesienie_umyslu;
int     gsn_lodowa_tarcza;
int     gsn_lodowy_kolec;
int     gsn_dezorientacja;
int     gsn_zniszczenie;
int     gsn_przeklenstwo_natury;
int     gsn_przyspieszenie;
int     gsn_spowolnienie;
int     gsn_zakazenie;
int     gsn_kukla;
int     gsn_regeneracja;
int     gsn_degeneracja;
int     gsn_hipnoza;
int     gsn_rewers;
int     gsn_uwolnienie_umyslow;
int     gsn_znamie_smierci;

/*
 * Ulryk: nekromanta
 */

int     gsn_porazajacy_dotyk;
int     gsn_balsamowanie;
int     gsn_sprowadzenie_szkieletu;
int     gsn_mroczny_plomien;
int     gsn_kontrola_umyslu;
int     gsn_wskrzeszenie;
int     gsn_pocalunek_smierci;
int     gsn_eksplozja;
int     gsn_stworzenie_mumii;

/* Lam 5.11.2004: zapobiegam uzywaniu skill_lookup */
int	gsn_acid_breath;
int	gsn_armor;
int	gsn_bless;
int	gsn_combat_mind;
int	gsn_cure_light;
int	gsn_fire_breath;
int	gsn_frost_breath;
int	gsn_gas_breath;
int	gsn_lightning_breath;
int	gsn_mana_gift;
int	gsn_refresh;
int	gsn_shield;
int	gsn_teleport;


/*
 * Lam 29.1.2004: zapamietane rasy
 */
int	zr_bog;
int	zr_byk;
int	zr_harpia;
int	zr_jednorozec;
int	zr_jez;
int	zr_martwiak;
int	zr_minotaur;
int	zr_niedzwiedz;
int	zr_nietoperz;
int	zr_pajeczak;
int	zr_papuga;
int	zr_przedmiot;
int	zr_shambler;
int	zr_smok;
int	zr_troll;
int	zr_wampir;
int	zr_waz;

/*
 * Paladyn od Gimzy
 */
int     gsn_aniol_stroz;
int     gsn_rozblysk;
int     gsn_zaglada;
int     gsn_swiety_msciciel;


/*
 * Lam 23.3.99: zmienne czytane z pliku vnums.txt zamiast wielu #define w merc.h
 */
int             MOB_VNUM_MUMIA;         /* Ulryk */
int             MOB_VNUM_ZOMBIE;        /* Ulryk */
int		MOB_VNUM_SZKIELET;      /* Ulryk */
int		MOB_VNUM_KUKLA;		/* Qwert */
int		MOB_VNUM_CITYGUARD;
int		MOB_VNUM_VAMPIRE;
int		MOB_VNUM_ULT;
int		MOB_VNUM_SECRETARY;
int		MOB_VNUM_MIDGAARD_MAYOR;
int		MOB_VNUM_BANKIER;
/* int		MOB_VNUM_KOLES; */
int		MOB_VNUM_PRZECHOWALNIA;
int		MOB_VNUM_WARSZTAT;
int		MOB_VNUM_SUPERMOB;
int		OBJ_VNUM_MONEY_ONE;
int		OBJ_VNUM_MONEY_SOME;
int		OBJ_VNUM_CORPSE_NPC;
int		OBJ_VNUM_CORPSE_PC;
int		OBJ_VNUM_FLAKI;
int		OBJ_VNUM_GLOWA;
int		OBJ_VNUM_REKA;
int		OBJ_VNUM_NOGA;
int		OBJ_VNUM_SERCE;
int		OBJ_VNUM_OGON;
int		OBJ_VNUM_SKRZYDLO;
int		OBJ_VNUM_JEZYK;
int		OBJ_VNUM_UCHO;
int		OBJ_VNUM_LAPA;
int		OBJ_VNUM_LEB;
int		OBJ_VNUM_ZAB;
int		OBJ_VNUM_KIEL;
int		OBJ_VNUM_ROG;
int		OBJ_VNUM_SZPON;
int		OBJ_VNUM_PLETWA;
int		OBJ_VNUM_MUSHROOM;
int		OBJ_VNUM_LIGHT_BALL;
int		OBJ_VNUM_SPRING;
int		OBJ_VNUM_EGG;
int		OBJ_VNUM_PORTAL;
int		OBJ_VNUM_BRAMA;
int		OBJ_VNUM_DROGOWSKAZ;
int             OBJ_VNUM_CHRUST;
int		OBJ_VNUM_SCHOOL_MACE;
int		OBJ_VNUM_SCHOOL_DAGGER;
int		OBJ_VNUM_SCHOOL_SWORD;
int		OBJ_VNUM_SCHOOL_VEST;
int		OBJ_VNUM_SCHOOL_SHIELD;
/* int		OBJ_VNUM_SCHOOL_BANNER; */
int		OBJ_VNUM_SCHOOL_CLUB;
int		OBJ_VNUM_BLACK_POWDER;
int		OBJ_VNUM_STAKE;
int		OBJ_VNUM_KREW;
int		OBJ_VNUM_KALUZA_KRWI;
int 		OBJ_VNUM_PALAD_MLOT;
int		OBJ_VNUM_PALAD_MIECZ;
int		OBJ_VNUM_SILVER_POWDER;
int		OBJ_VNUM_JEDZ_MIN;
int		OBJ_VNUM_JEDZ_MAX;
int		ROOM_VNUM_LIMBO;
int		ROOM_VNUM_CHAT;
int		ROOM_VNUM_TEMPLE;
int		ROOM_VNUM_ALTAR;
int		ROOM_VNUM_SCHOOL;
int		ROOM_VNUM_GRAVEYARD_A;
int		ROOM_VNUM_PURGATORY_A;
int		ROOM_VNUM_POZA_SWIATEM;
/* int		ROOM_VNUM_PUNKT; */



/*
 * mp_wczyt.c
 */
int 		mprog_name_to_type	args ( ( char* name, int gdzie ) );
MPROG_DATA	*mprog_file_read 	args ( ( char* f, MPROG_DATA* mprg,
						void *param, int gdzie ) );
bool		load_mobprogs           args ( ( FILE* fp ) );
bool   		mprog_read_programs     args ( ( FILE* fp, void *param,
						int gdzie ) );

/*
 * Locals.
 */
MOB_INDEX_DATA		*mob_index_hash          [ MAX_KEY_HASH       ];
OBJ_INDEX_DATA		*obj_index_hash          [ MAX_KEY_HASH       ];
ROOM_INDEX_DATA		*room_index_hash         [ MAX_KEY_HASH       ];

AREA_DATA		*area_first;
AREA_DATA		*area_last;
AREA_DATA		*area_dla_helpow;
AREA_DATA		*area_czysciec; /* Lam */

ZONE_DATA		*zone_first; /* Lam */

extern char             str_empty                [ 1                  ];

int			top_alias; /* Lam */
int                     top_affect;
int                     top_area;
int			top_clan;
int			top_clan_member;
int			top_clan_rel;
int                     top_ed;
int                     top_exit;
int			top_fight; /* Lam dla struktur Ulryka */
int			top_healer; /* Lam */
int                     top_help;
int			top_imiona; /* Lam */
int			top_mob; /* Lam */
int                     top_mob_index;
int			top_mpquest; /* Lam */
int			top_mprog; /* Lam */
int                     top_obj_index;
int			top_obj; /* Lam */
int			top_pcdata; /* Lam */
int			top_refuse;
int                     top_reset;
int                     top_room;
int                     top_shop;
int			top_stosmp; /* Lam */
int			top_tell; /* Lam */
int			top_trap; /* Lam */
int			top_zmienna; /* Lam */
int			top_zwod; /* Lam */

#define                 MAX_PERM_BLOCK  131072
#define                 MAX_MEM_LIST    16

void *                  rgFreeList              [ MAX_MEM_LIST       ];
const int               rgSizeList              [ MAX_MEM_LIST       ] =
{
    4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768,
    65536, 131072-64
};

extern int              nAllocString;
extern int              sAllocString;
extern int              nOverFlowString;
extern int              sOverFlowString;
extern bool             Full;
int                     nAllocPerm;
int                     sAllocPerm;



/*
 * Semi-locals.
 */
bool			fBootDb;
bool			ODPLUSKWIACZ;
bool			PRZERABIACZ;
char			HTTPD_log		[ MAX_INPUT_LENGTH ];
FILE *			fpArea;
char			strArea			[ MAX_INPUT_LENGTH ];

char *			daPrompt;

char			lsc_init_str		[ MAX_INPUT_LENGTH ];
char			lsc_delimiter;
char			lsc_clear_str		[ MAX_INPUT_LENGTH ];
unsigned int		lsc_init_str_len;

/*
 * Local booting procedures.
 */
bool    load_area		args( ( FILE *fp ) );
void    load_class		args( ( FILE *fp, int cla ) );	/* Lam */
bool    load_helps		args( ( FILE *fp ) );
bool    load_mobiles		args( ( FILE *fp ) );
bool    load_objects		args( ( FILE *fp ) );
bool    load_resets		args( ( FILE *fp ) );
bool    load_rooms		args( ( FILE *fp ) );
bool    load_shops		args( ( FILE *fp ) );
bool    load_healers		args( ( FILE *fp ) );
bool    load_specials		args( ( FILE *fp ) );
void    load_schematy		args( ( void ) );
void    load_notes		args( ( void ) );
void	load_clans		args( ( void ) );
void    load_ban        	args( ( void ) );
void    load_down_time  	args( ( void ) );
void    fix_exits       	args( ( void ) );
void	czytaj_ciala		args( ( void ) );	/* Lam - handler.c */
void	load_hints		args( ( void ) );	/* Lam */
void	load_offensive		args( ( void ) );	/* Lam */
void	load_miodek		args( ( void ) );	/* Lam */
void	load_powody		args( ( void ) );	/* Lam */
void	load_imiona		args( ( void ) );	/* Lam */
/* Qwert: to sie jeszcze przyda */
/*void	load_maskowanie		args( ( void ) );*/     /* Ulryk */
void	load_quits		args( ( void ) );	/* Lam */
void	load_vnums		args( ( void ) );	/* Lam */
void	load_tracks		args( ( void ) );	/* Lam */
void	load_zones		args( ( void ) );	/* Lam */
void	load_lsc		args( ( void ) );	/* Lam */
void	sprawdz_strefy		args( ( void ) );	/* Lam */
void	sprawdz_ociemniale_moby	args( ( void ) );
void    reset_area      	args( ( AREA_DATA *pArea ) );
int	szukaj_wersji_krainy	args( ( char *arg ) );
bool	fread_time		args( ( FILE *fp, int *godz, int *min ) );
char	*nazwa_krainy		args( ( char *nazwa ) );
void	znajdz_kraine		args( ( char *arg,
					struct zapamietana_kraina *zk ) );
void	zapamietaj_kraine	args( ( struct zapamietana_kraina *zk ) );
void	przywroc_kraine		args( ( struct zapamietana_kraina *zk ) );


/*
 * Big mama top level function.
 */
void boot_db( bool edytor )
{
    FILE *fpList;

    /*
     * Init some data space stuff.
     */
    init_string_space( );
    fBootDb = TRUE;

    /*
     * Init random number generator.
     */
    init_mm( );

    if ( !( fpList = fopen( AREA_LIST, "r" ) ) )
    {
	chdir( "../area" );
	/* niewazne, czy/ktory chdir sie uda, blad w perror bedzie prawdziwy */
	if ( !( fpList = fopen( AREA_LIST, "r" ) ) )
	{
	    chdir( "area" );
	    if ( !( fpList = fopen( AREA_LIST, "r" ) ) )
	    {
		lac_perror( AREA_LIST );
		cbug( "Nie udalo sie otworzyc listy krain! Byc moze uruchamiasz Laca w zlym katalogu?", 0 );
		exit( 1 );
	    }
	}
    }

    /*
     * Lam 10.1.2003: teraz zapisywany jest wiek muda, wiec przenosze tutaj
     */
    czytaj_ustawienia( );	/* Lam 11.3.98, 25.12.1999 */

    build_spec_list( );		/* Lam 4.2.2004 */

    /*
     * Set time and weather.
     */
    {
	long lday, lmonth;

/*      mud_age         = ( current_time - 858218400 */ /* 650336715 */ /* )
			   / ( PULSE_TICK / PULSE_PER_SECOND ); */
	time_info.hour  = mud_age % 24;
	if ( ODPLUSKWIACZ || TylkoKonwersja )
	{
	    mud_age -= time_info.hour;
	    time_info.hour = 0; /* Lam 3.8.2005, zeby spec_cast_ghost sie odnawialy zawsze po uruchomieniu lac -o */
	}
	lday            = mud_age / 24;
	time_info.day   = lday    % 35;
	lmonth          = lday    / 35;
	time_info.month = lmonth  % 17;
	time_info.year  = lmonth  / 17 + 1; /* + 1 bo zaczynalismy od roku 1, nie 0 --Lam */

	     if ( time_info.hour <  5 ) weather_info.sunlight = SUN_DARK;
	else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
	else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
	else if ( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
	else                            weather_info.sunlight = SUN_DARK;

	weather_info.change     = 0;
	weather_info.mmhg       = 960;
	if ( time_info.month >= 7 && time_info.month <=12 )
	    weather_info.mmhg += number_range( 1, 50 );
	else
	    weather_info.mmhg += number_range( 1, 80 );

	     if ( weather_info.mmhg <=  980 ) weather_info.sky = SKY_LIGHTNING;
	else if ( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
	else if ( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
	else                                  weather_info.sky = SKY_CLOUDLESS;

    }

    /* Przygotowanie aukcji */
    if ( !( auction = (AUCTION_DATA *) malloc( sizeof( AUCTION_DATA ) ) ) )
    {
	cbug( "boot_db: NULL malloc auction!", 0 );
	exit( 1 );
    }
    auction->item = NULL; /* niczego nie aukcjonujemy */

    /*
     * Jesli umiejetnosc ma gsn, to jej go odpowiednio ustawic.
     * To tylko numer umiejetnosci, nie musi byc po load_class( ).
     * Wyzerowac wszystkie umiejetnosci na odpowiednie poziomy.
     * Specjalny przypadek: reserved
     */
    {
	int sn;
	int cla;
	SORTED_SKILL_DATA *ss, *tss;

	for ( cla = 0; cla < MAX_CLASS; cla++ )
	{
	    /* 0 to zawsze reserved, jesli to zmienisz, popelnisz blad */
	    skill_table[ 0 ].skill_level[ cla ] = 199;
	    skill_table[ 0 ].multi_level[ cla ] = 199;
	}

	ss = calloc( sizeof( SORTED_SKILL_DATA ), 1 );
	ss->sn = 0;
	sorted_skill_list = ss;

	/* Lam 20.4.98: sn = 1, wczesniej powyzsze nie dzialalo */
	for ( sn = 1; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[ sn ].pgsn )
		*skill_table[ sn ].pgsn = sn;

	    for ( cla = 0; cla < MAX_CLASS; cla++ )
	    {
		/* Lam 5.2.2004: umiejetnosci nie dajace sie cwiczyc na 101 */
		if ( IS_SET( skill_table[ sn ].flags, SKILL_NPC ) )
		    skill_table[ sn ].skill_level[ cla ] = L_DIR;
		else
		    skill_table[ sn ].skill_level[ cla ] = L_JUN; /* 9.4.2007 z L_APP */
		skill_table[ sn ].multi_level[ cla ] = 199;
	    }

	    ss = calloc( sizeof( SORTED_SKILL_DATA ), 1 );
	    ss->sn = sn;
	    if ( alf_wczes( skill_table[ sn ].pl_name, skill_table[ sorted_skill_list->sn ].pl_name ) )
	    {
		ss->next = sorted_skill_list;
		sorted_skill_list = ss;
	    }
	    else
		for ( tss = sorted_skill_list; tss; tss = tss->next )
		{
		    if ( !tss->next )
		    {
			tss->next = ss;
			break;
		    }
		    else if ( alf_wczes( skill_table[ sn ].pl_name, skill_table[ tss->next->sn ].pl_name ) )
		    {
			ss->next = tss->next;
			tss->next = ss;
			break;
		    }
		}
	}
    }

    /*
     * Lam 29.1.2004: zapamietane rasy
     */
    {
	int index;

	for ( index = 0; index < MAX_RACE; index++ )
	{
	    if ( race_table[ index ].zr )
		*race_table[ index ].zr = index;

	    ilosc_czesci_ciala[ index ] = zlicz_bity( race_table[ index ].parts );
	}
    }

    /* Lam 23.3.99: inne funkcje wymagaja, aby load_vnums bylo na poczatku */
    load_vnums( );

    /*
     * Klasy przed krainami, zeby bledy klas nie marnowaly czasu potrzebnego na
     * zaladowanie krain.
     */
    {
	FILE *fpClass;
	int cla;
	char buf [ MAX_INPUT_LENGTH ];

	for ( cla = 0; cla < MAX_CLASS; cla++ )
	{
	    sprintf( buf, "%s%s", CLASS_DIR, class_table[ cla ].who_name );
	    if ( !( fpClass = fopen( buf, "r" ) ) )
	    {
		lac_perror( buf ); /* (buf): no such file or directory -Lam */
		cbug( "Nie udalo sie otworzyc pliku z danymi profesji!", 0 );
		exit( 1 );
	    }
	    load_class( fpClass, cla );
	    fclose( fpClass );
	}
    }

    /*
     * Lam 27.11.2000: strefy wczytywane sa przed krainami, a po wczytaniu
     * krain nastepuje dodatkowe sprawdzenie vnumow.
     */
    load_zones( );

    /*
     * Read in all the area files.
     */
    for ( ; ; )
    {
	int stat;

	strcpy( strArea, fread_word( fpList, &stat, TRUE ) );
	if ( strArea[ 0 ] == '$' )
	    break;

	if ( strArea[ 0 ] == '-' )
	{
	    fpArea = stdin;
	}
	else
	{
	    if ( !( fpArea = fopen( strArea, "r" ) ) )
	    {
		lac_perror( strArea );
		cbug( "Nie udalo sie otworzyc pliku krainy! Sprawdz, czy nazwa wpisana w " AREA_LIST " jest prawidlowa.", 0 );
		exit( 1 );
	    }
	}

	area_dla_helpow = NULL;

	if ( load_area_file( fpArea, NULL ) )
	    exit( 1 );

	if ( fpArea != stdin )
	    fclose( fpArea );
	fpArea = NULL;
    }

    fclose( fpList );

    /*
     * Fix up exits.
     * Declare db booting over.
     * Reset all areas once.
     * Load up the notes file.
     */
    fix_exits( );
    fBootDb  = FALSE;
    daPrompt = str_dup( "<%A%h`z %mm %vr %xd`s><%n`z %cm> " );

    if ( !edytor ) /* edytor nie potrzebuje niczego */
    {
	int i;

	for ( i = 0; czesci_ciala[ i ].wsk; i++ )
	    czesci_ciala[ i ].vnum = *czesci_ciala[ i ].wsk;

	load_notes( );
	load_ban( );
	load_down_time( );
	load_clans( );		/* Lam 12.97 */
	czytaj_ciala( );	/* Lam 20.3.98 */
	load_hints( );		/* Lam 9.2.99 */
	load_schematy( );	/* Cadaver 16.12.2001 */
	load_tracks( );		/* Lam 22.3.99, 15.5.2000 */
	load_offensive( );	/* Lam 13.12.2002 */
	load_miodek( );		/* Lam 14.12.2002 */
	load_powody( );		/* Lam 12.11.2006 */
	load_imiona( );		/* Lam 26.12.2006 */
	load_lsc( );		/* Lam 7.1.2012 */
/*	load_maskowanie( );	   Ulryk 23.9.2003 */
#if defined( BLEDOMAT )
	wczytaj_bledomat( );	/* Lam 24.1.2005 */
#endif /* defined( BLEDOMAT ) */
	/* boot_done niszczy tymczasowe drzewo przeszukiwania SSM-a, wczesniej
	   bylo to razem z fBootDb = FALSE, ale przeciez po tym sa ladowane
	   lancuchy z innych plikow (np. duzo powtorzonych imion postaci
	   blokujacych imiona w load_imiona) */
	boot_done( );

	if ( ODPLUSKWIACZ ) do_vnumy( NULL, "" );	/* Lam 26.6.99 */
	load_quits( );		/* Lam 26.3.2000 */
	do_slookup( NULL, "" );	/* Lam 6.11.2000 */
	sprawdz_strefy( );	/* Lam 27.11.2000 */
	do_racestat( NULL, "" );/* Lam 4.12.2000 */
	/* Uruchamiam update_handler zamiast area_update, to ustawia wiele
	   potrzebnych rzeczy, uruchamia tez area_update, chociaz tylko raz
	   (wczesniej uruchamialo sie raz z tego miejsca i drugi w chwile
	   po wejsciu w main_loop).
	   Konwerter pozwala wykryc pewne bledy (dzieki zmianom w reset_area). */
	if ( TylkoKonwersja && ODPLUSKWIACZ )
	{
	    area_update( );
	    sprawdz_ociemniale_moby( );
	}
	else
	{
	    update_handler( );
	}
    }
    else /* edytor */
	boot_done( );

    return;
}


/*
 * Lam 27.11.2000: Czytanie listy stref.
 */
void load_zones( void )
{
    FILE *fp;
    char literka;
    char *slowo;
    int status;
    ZONE_DATA *strefa;
    ZONE_DATA *ostatnia;

    if ( !( fp = fopen( ZONE_FILE, "r" ) ) )
    {
	lac_perror( ZONE_FILE );
	exit( 1 ); /* mud musi miec strefy, chocby jedna. */
    }

    for ( ; ; )
    {
	literka = fread_letter( fp );
	if ( literka != '#' )
	{
	    cbug( "Uszkodzony plik stref? Brak '#'", 0 ); /* moze byc EOF, nie wnikam */
	    exit( 1 );
	}

	slowo = fread_word( fp, &status, TRUE );
	if ( !str_cmp( slowo, "KONIEC" ) )
	    break;
	else if ( !str_cmp( slowo, "STREFA" ) )
	{
	    strefa		= calloc( sizeof( ZONE_DATA ), 1 );
	    strefa->nazwa	= str_dup( "" );
	    strefa->dopelniacz	= str_dup( "" );
	    strefa->celownik	= str_dup( "" );
	    strefa->biernik	= str_dup( "" );
	    strefa->narzednik	= str_dup( "" );
	    strefa->miejscownik	= str_dup( "" );
	    strefa->stolica	= str_dup( "" );
	    strefa->stol_dop	= str_dup( "" );
	    strefa->stol_cel	= str_dup( "" );
	    strefa->stol_bie	= str_dup( "" );
	    strefa->stol_narz	= str_dup( "" );
	    strefa->stol_mie	= str_dup( "" );
	    strefa->wladca	= str_dup( "" );
	    strefa->namiestnik	= str_dup( "" );
	    strefa->rasy	= str_dup( "" );
	    for ( ; ; )
	    {
		slowo = fread_word( fp, &status, TRUE );
		if ( !str_cmp( slowo, "Nazwa" ) )
		    strefa->nazwa = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Dopelniacz" ) )
		    strefa->dopelniacz = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Celownik" ) )
		    strefa->celownik = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Biernik" ) )
		    strefa->biernik = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Narzednik" ) )
		    strefa->narzednik = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Miejscownik" ) )
		    strefa->miejscownik = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Stolica" ) )
		    strefa->stolica = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "StolDop" ) )
		    strefa->stol_dop = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "StolCel" ) )
		    strefa->stol_cel = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "StolBie" ) )
		    strefa->stol_bie = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "StolNarz" ) )
		    strefa->stol_narz = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "StolMie" ) )
		    strefa->stol_mie = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Wladca" ) )
		    strefa->wladca = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Namiestnik" ) )
		    strefa->namiestnik = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Rasy" ) )
		    strefa->rasy = fread_string( fp, &status );
		else if ( !str_cmp( slowo, "Przywolanie" ) )
		    strefa->przywolanie = fread_number( fp, &status );
		else if ( !str_cmp( slowo, "Swiatynia" ) )
		    strefa->swiatynia = fread_number( fp, &status );
		else if ( !str_cmp( slowo, "Szkola" ) )
		    strefa->szkola = fread_number( fp, &status );
		else if ( !str_cmp( slowo, "Koniec" ) )
		    break;
		else
		{
		    cbug( "Nieznane pole w strefie w pliku " ZONE_FILE, 0 );
		    exit( 1 );
		}
	    }
	    if ( !zone_first )
		zone_first = strefa;
	    else
	    {
		for ( ostatnia = zone_first; ostatnia->next; ostatnia = ostatnia->next )
		    ;
		ostatnia->next = strefa;
	    }
	}
	else
	{
	    cbug( "Nieznana sekcja w pliku " ZONE_FILE, 0 );
	    exit( 1 );
	}
    }

    fclose( fp );
    if ( !zone_first )
    {
	cbug( "Nie zdefiniowano stref!", 0 );
	exit( 1 );
    }

    return;
}


ZONE_DATA *znajdz_strefe( char *nazwa )
{
    ZONE_DATA *strefa;

    for ( strefa = zone_first; strefa; strefa = strefa->next )
	if ( !str_cmp( nazwa, strefa->nazwa ) )
	    break;

    return strefa;
}


ZONE_DATA *znajdz_strefe_skrot( char *nazwa )
{
    ZONE_DATA *strefa;

    for ( strefa = zone_first; strefa; strefa = strefa->next )
	if ( !str_prefix( nazwa, strefa->nazwa ) )
	    break;

    return strefa;
}


ZONE_DATA *znajdz_strefe_po_stolicy( char *nazwa )
{
    ZONE_DATA *strefa;

    for ( strefa = zone_first; strefa; strefa = strefa->next )
	if ( !str_cmp( nazwa, strefa->stolica ) )
	    break;

    return strefa;
}


ZONE_DATA *znajdz_strefe_po_vnumie( int vnum )
{
    ZONE_DATA *strefa;

    for ( strefa = zone_first; strefa; strefa = strefa->next )
	if ( vnum == strefa->przywolanie || vnum == strefa->szkola )
	    break;

    return strefa;
}


ZONE_LIST *dodaj_strefe( ZONE_LIST *strefy, ZONE_DATA *strefa )
{
    ZONE_LIST *pierwsza = strefy;
    ZONE_LIST *nowa;

    if ( !strefa )
    {
	cbug( "dodaj_strefe: proba dodania NULLowej strefy", 0 );
	exit( 1 );
    }

    nowa       = calloc( sizeof( ZONE_LIST ), 1 );
    nowa->zone = strefa;
    nowa->next = NULL;

    if ( !strefy )
	return nowa;

    while ( strefy->next )
	strefy = strefy->next;

    strefy->next = nowa;

    return pierwsza;
}


bool strefa_nalezy( ZONE_DATA *strefa, ZONE_LIST *lista )
{
    ZONE_LIST *p;

    for ( p = lista; p; p = p->next )
	if ( strefa == p->zone )
	    return TRUE;

    return FALSE;
}


void sprawdz_strefy( void )
{
    ZONE_DATA *strefa;
    ROOM_INDEX_DATA *pom;
    FILE *fp;

    if ( !( fp = fopen( ZONES_FILE, "w" ) ) )
    {
	lac_perror( ZONES_FILE );
	cbug( "Blad otwarcia pliku " ZONES_FILE, 0 );
	exit( 1 );
    }

    for ( strefa = zone_first; strefa; strefa = strefa->next )
    {
	if ( !( pom = get_room_index( strefa->przywolanie ) ) )
	{
	    fprintf( fp, "Strefa '%s' bez przywolania (%d)\n",
			strefa->nazwa, strefa->przywolanie );
	    cbug( "Strefa bez przywolania", 0 );
	    exit( 1 );
	}

	if ( !strefa_nalezy( strefa, pom->area->strefy ) )
	{
	    fprintf( fp, "Strefa '%s' ma przywolanie (%d) w innej strefie!\n",
			strefa->nazwa, strefa->przywolanie );
	    cbug( "Niewlasciwe przywolanie strefy", 0 );
	    exit( 1 );
	}

	if ( !( pom = get_room_index( strefa->szkola ) ) )
	{
	    fprintf( fp, "Strefa '%s' bez szkoly (%d)\n",
			strefa->nazwa, strefa->szkola );
	    cbug( "Strefa bez szkoly", 0 );
	    exit( 1 );
	}

	if ( !strefa_nalezy( strefa, pom->area->strefy ) )
	{
	    fprintf( fp, "Strefa '%s' ma szkole (%d) w innej strefie!\n",
			strefa->nazwa, strefa->szkola );
	    cbug( "Niewlasciwa szkola strefy", 0 );
	    exit( 1 );
	}

	fprintf( fp, "Strefa '%s': %d krain, zawierajacych %d pomieszczen, %d mobow i %d przedmiotow\n",
		strefa->nazwa, strefa->ile_krain, strefa->ile_pomieszczen,
		strefa->ile_mobow, strefa->ile_przedmiotow );
    }

    fclose( fp );

    return;
}


/*
 * Lam 19.12.1997: wczytanie pliku zawierajacego umiejetnosci klasy.
 * Lam 6.11.2000: czytanie poz z pliku
 */
void load_class( FILE *fp, int cla )
{
    char *klucz;
    int   stat;
    int   sn;
    int   level = 1; /* tu mogloby byc L_APP dla bezpieczenstwa */
    char  buf [ MAX_STRING_LENGTH ];

    for ( ; !feof( fp ); )
    {
	klucz = fread_word( fp, &stat, TRUE );
	if ( !klucz )
	{
	    cbug( "load_class(): Blad przy czytaniu klucza.", 0 );
	    break;
	}

	if ( !str_cmp( klucz, "Poziom" ) )
	    level = fread_number( fp, &stat );

	else if ( !str_cmp( klucz, "Um" ) )
	{
	    if ( ( sn = skill_lookup_ang( fread_word( fp, &stat, TRUE ) ) ) < 1 )
	    {
		char bufor[ MIL ];
		sprintf( bufor, "load_class(): Um: zla umiejetnosc na poziom %d, klasa %d [%s]", level, cla, class_table[ cla ].who_name );
		cbug( bufor, 0 );
	    }
	    else
	    {
		skill_table[ sn ].skill_level[ cla ] = level;
		skill_count[ cla ]++;
	    }
	}
	else if ( !str_cmp( klucz, "+Dod" ) )
	{
	    if ( ( sn = skill_lookup_ang( fread_word( fp, &stat, TRUE ) ) ) < 1 )
	    {
		cbug( "load_class(): +Dod: zla umiejetnosc", 0 );
	    }
	    else
	    {
		skill_table[ sn ].multi_level[ cla ] = level;
		multi_count[ cla ]++;
	    }
	}
	else if ( !str_cmp( klucz, "#POZY" ) )
	{
	    for ( ; ; )
	    {
		int kupka;

		do
		{
		    kupka = getc( fp );
		}
		while ( isspace( kupka ) );
		if ( feof( fp ) )
		{
		    cbug( "load_class (class %d): EOF przy czytaniu poz", cla );
		    break;
		}
		ungetc( kupka, fp );
		if ( kupka == '#' )
		    break;

		pose_table[ cla ][ pose_count[ cla ] ].to_char = fread_string( fp, &stat );
		pose_table[ cla ][ pose_count[ cla ] ].to_room = fread_string( fp, &stat );
		pose_count[ cla ]++;
	    }
	}
	/* ignorujemy te sekcje */
	else if ( !str_cmp( klucz, "#PROFESJA" ) )
	    continue;
	else if ( !str_cmp( klucz, "#KONIEC" ) )
	    break;
	else
	{
	    sprintf( buf, "load_class(): zly klucz '%s', klasa %d [%s].", klucz, cla, class_table[ cla ].who_name );
	    cbug( buf, 0 );
	    fread_to_eol( fp );
	}
    }

    return;
}


int szukaj_wersji_krainy( char *arg )
{
    int pzl;
    int wiel = sizeof( wersje_krain ) / sizeof( wersje_krain[ 0 ] );

    for ( pzl = 0; pzl < wiel; pzl++ )
	if ( !str_cmp( wersje_krain[ pzl ].nazwa, arg ) )
	    break;

    if ( pzl >= wiel )
    {
	sprintf( bug_buf, "Nierozpoznana wersja krainy: %s", arg );
	cbug( bug_buf, 0 );
/*	pzl = 0; */
	exit( 1 );
    }

    return pzl;
}


bool load_area_file( FILE *fp, CHAR_DATA *ch )
{
    int stat;
    bool wyjscie;
    char literka;

    for ( ; ; )
    {
	char *word;

	literka = fread_letter( fp );
	if ( literka == '*' ) /* Lam 3.1.2006: komentarze przed/miedzy sekcjami */
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( literka != '#' )
	{
	    cbug( "Load_area_file: # not found.", 0 );
	    return TRUE;
	}

	word = fread_word( fp, &stat, TRUE );

	if ( stat )
	    return TRUE;

	if ( word[ 0 ] == '$' )
	    break;
	else if ( !str_cmp( word, "AREA" ) )
	    wyjscie = load_area( fp );
	else if ( !str_cmp( word, "HELPS" ) )
	    wyjscie = load_helps( fp );
	else if ( !str_cmp( word, "MOBILES" ) )
	    wyjscie = load_mobiles( fp );
	else if ( !str_cmp( word, "OBJECTS" ) )
	    wyjscie = load_objects( fp );
	else if ( !str_cmp( word, "RESETS" ) )
	    wyjscie = load_resets( fp );
	else if ( !str_cmp( word, "ROOMS" ) )
	    wyjscie = load_rooms( fp );
	/* ponizsze zachowane jedynie po to, by moc czytac krainy z Envy
	   i starego Laca */
	else if ( !str_cmp( word, "SHOPS" ) )
	    wyjscie = load_shops( fp );
	else if ( !str_cmp( word, "HEALERS" ) )
	    wyjscie = load_healers( fp );
	else if ( !str_cmp( word, "SPECIALS" ) )
	    wyjscie = load_specials( fp );
	else if ( !str_cmp( word, "MOBPROGS" ) )
	    wyjscie = load_mobprogs( fp );
	else
	{
	    cbug( "Load_area_file: bad section name.", 0 );
	    return TRUE;
	}

	if ( wyjscie )
	    return TRUE;
    }

    if ( ODPLUSKWIACZ
      && area_last /* wczytywanie plikow pomocy bez #AREA */
      && area_last->lowgroupvnum
      && ( area_last->lowgroupvnum < area_last->lowmobvnum
	|| area_last->highgroupvnum > area_last->highmobvnum ) )
	cbug( "Kraina ma grupy mobow rozrzucone bardziej niz moby. Numeruj grupy tak jak moby i nie uzywaj grup z innych krain.", 0 );

    return FALSE;
}


/*
 * Snarf an 'area' header line.
 */
bool load_area( FILE *fp )
{
    AREA_DATA *pArea;
    ZONE_DATA *strefa;
    ZONE_LIST *lista;
    int        stat;
    char      *key;

    pArea               = alloc_perm( sizeof( *pArea ) );
    pArea->name		= str_dup( "" );
    pArea->full_name	= str_dup( "" );
    pArea->reset_msg	= str_dup( "" );
    pArea->opis		= str_dup( "" );
    pArea->revision	= str_dup( "" );
    pArea->reset_first  = NULL;
    pArea->reset_last   = NULL;
    pArea->file_name	= str_dup( strArea );
    pArea->recall       = 0; /* 31.3.2002: zmiana z ROOM_VNUM_TEMPLE */
    /* Lam: pogoda i czas krainy */
    pArea->time_info	= time_info;
    pArea->weather_info = weather_info;
    pArea->version	= DOMYSLNA_WERSJA;
    pArea->flags	= 0;
    pArea->odnowiona	= FALSE;
#if !defined( WYMAGANE_STREFY )
    pArea->strefy	= NULL;
#endif

    for ( ; ; ) /* mozna zrobic lepiej, jak przy postaci */
    {
	key = fread_word( fp, &stat, TRUE );

	if ( !str_cmp( key, "Name" ) )
	    pArea->name = fread_string( fp, &stat );

	else if ( !str_cmp( key, "Long" ) )
	    pArea->full_name = fread_string( fp, &stat );

	else if ( !str_cmp( key, "Reset" ) )
	    pArea->reset_msg = fread_string( fp, &stat );

	else if ( !str_cmp( key, "Revision" ) )
	    pArea->revision = fread_string( fp, &stat );

	else if ( !str_cmp( key, "Recall" ) )
	    pArea->recall = fread_number( fp, &stat );

	else if ( !str_cmp( key, "Timezone" ) )
	    pArea->time_info.hour += fread_number( fp, &stat );

	else if ( !str_cmp( key, "Flags" ) )
	    pArea->flags = fread_number( fp, &stat );

	else if ( !str_cmp( key, "Version" ) )
	{
	    key = fread_string( fp, &stat );
	    pArea->version = szukaj_wersji_krainy( key );
	    free_string( key );
	}

	else if ( !str_cmp( key, "Zone" ) )
	{
	    key = fread_string( fp, &stat );
	    strefa = znajdz_strefe( key );
	    if ( !strefa )
	    {
		strefa = znajdz_strefe_po_stolicy( key );
		if ( !strefa )
		{
		    int vnum = atoi( key );
		    strefa = znajdz_strefe_po_vnumie( vnum );
		    if ( !vnum || !strefa )
		    {
			cbug( "Nieistniejaca strefa!", 0 );
			return TRUE;
		    }
		}
	    }
	    pArea->strefy = dodaj_strefe( pArea->strefy, strefa );
	    free_string( key );
	}

	else if ( !str_cmp( key, "Descr" ) )
	    pArea->opis = fread_string( fp, &stat );

	else if ( !str_cmp( key, "End" ) )
	{
	    if ( !pArea->strefy )
	    {
		cbug( "Kraina bez okreslonych stref!", 0 );
		return TRUE;
	    }
	    break;
	}
	else
	{
	    cbug( "load_area: zle slowo kluczowe!", 0 );
	}
    }

    popraw_czas_krainy( pArea );

    pArea->age          = 15; /* zresetuje sie od razu */
    pArea->nplayer      = 0;
    pArea->numofrooms	= 0;
    pArea->numofmobs	= 0;
    pArea->numofobjs	= 0;
    pArea->lowroomvnum	= (-1) & ~b31; /* najwieksza l. dodatnia? */
    pArea->highroomvnum	= 0;
    pArea->lowmobvnum	= (-1) & ~b31;
    pArea->highmobvnum	= 0;
    pArea->lowobjvnum	= (-1) & ~b31;
    pArea->highobjvnum	= 0;
    pArea->lowmoblevel	= (-1) & ~b31;
    pArea->highmoblevel	= 0;

    if ( !area_first )
	area_first = pArea;
    if ( area_last )
	area_last->next = pArea;
    area_last   = pArea;
    pArea->next = NULL;

    area_dla_helpow	= pArea;
    for ( lista = pArea->strefy; lista; lista = lista->next )
	lista->zone->ile_krain++;
    top_area++;

    return FALSE;
}


/*
 * Snarf a help section.
 */
bool load_helps( FILE *fp )
{
    HELP_DATA *pHelp;
    char      *keyword;
    int        level;
    int        stat;

    for ( ; ; )
    {
	level           = fread_number( fp, &stat );
	keyword         = fread_string( fp, &stat );
	if ( !keyword ) return TRUE; /* to w przypadku bledow w pliku krainy */
	if ( keyword[ 0 ] == '$' )
	{
	    free_string( keyword );
	    break;
	}
	pHelp           = alloc_perm( sizeof( *pHelp ) );
	pHelp->area	= area_dla_helpow; /* wazne! */
	pHelp->level    = level;
	pHelp->keyword  = keyword;
	pHelp->text     = fread_string( fp, &stat );

#if defined( MSDOS )
	if ( !str_cmp( pHelp->keyword, "#powitanie_DOS" ) )
	    help_greeting = pHelp->text;
#else
	if ( !str_cmp( pHelp->keyword, "powitanie" ) )
	    help_greeting = pHelp->text;
#endif
	if ( !str_cmp( pHelp->keyword, "#imi`e" ) )
	    help_namehelp = pHelp->text;

	if ( !help_first )
	    help_first = pHelp;
	if ( help_last )
	    help_last->next = pHelp;

	help_last       = pHelp;
	pHelp->next     = NULL;
	top_help++;
    }

    return FALSE;
}


/*
 * Snarf a mob section.
 */
bool load_mobiles( FILE *fp )
{
    MOB_INDEX_DATA *pMobIndex;

    if ( !area_last )
    {
	cbug( "Load_mobiles: no #AREA seen yet.", 0 );
	return TRUE;
    }

    for ( ; ; )
    {
	char *race;
	char  letter;
	int   vnum;
	int   stat;

	letter = fread_letter( fp );
	if ( letter != '#' )
	{
	    cbug( "Load_mobiles: # not found.", 0 );
	    return TRUE;
	}

	vnum = fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	if ( vnum < 0 || vnum >= MAX_VNUM )
	{
	    cbug( "Load_mobiles: vnum %d poza zakresem.", vnum );
	    return TRUE;
	}

	pMobIndex                       = new_mob_index( vnum, area_last );
	if ( !pMobIndex )
	    return TRUE;

	pMobIndex->player_name          = fread_string( fp, &stat );
	pMobIndex->short_descr          = fread_string( fp, &stat );
	if ( wersje_krain[ area_last->version ].plusy )
	{
	    if ( *pMobIndex->short_descr == '+' )
	    {
		free_string( pMobIndex->short_descr );
		pMobIndex->short_descr          = fread_string( fp, &stat );
		pMobIndex->dopelniacz           = fread_string( fp, &stat );
		pMobIndex->celownik             = fread_string( fp, &stat );
		pMobIndex->biernik              = fread_string( fp, &stat );
		pMobIndex->narzednik            = fread_string( fp, &stat );
		pMobIndex->miejscownik          = fread_string( fp, &stat );
		if ( wersje_krain[ area_last->version ].wolacz )
		    pMobIndex->wolacz		= fread_string( fp, &stat );
		else
		    pMobIndex->wolacz           = str_dup( pMobIndex->short_descr );
	    }
	    else
	    {
		pMobIndex->dopelniacz           = str_dup( pMobIndex->short_descr );
		pMobIndex->celownik             = str_dup( pMobIndex->short_descr );
		pMobIndex->biernik              = str_dup( pMobIndex->short_descr );
		pMobIndex->narzednik            = str_dup( pMobIndex->short_descr );
		pMobIndex->miejscownik          = str_dup( pMobIndex->short_descr );
		pMobIndex->wolacz		= str_dup( pMobIndex->short_descr );
	    }
	}
	else
	{
	    pMobIndex->dopelniacz		= fread_string( fp, &stat );
	    pMobIndex->celownik			= fread_string( fp, &stat );
	    pMobIndex->biernik			= fread_string( fp, &stat );
	    pMobIndex->narzednik		= fread_string( fp, &stat );
	    pMobIndex->miejscownik		= fread_string( fp, &stat );
	    if ( wersje_krain[ area_last->version ].wolacz )
		pMobIndex->wolacz		= fread_string( fp, &stat );
	    else
		pMobIndex->wolacz               = str_dup( pMobIndex->short_descr );
	}
	pMobIndex->long_descr           = fread_string( fp, &stat );
	if ( wersje_krain[ area_last->version ].entery )
	    zjedz_entery_z_konca( &pMobIndex->long_descr );
	pMobIndex->description          = fread_string( fp, &stat );

	pMobIndex->long_descr[ 0 ]        = UPPER( pMobIndex->long_descr[ 0 ] );
	pMobIndex->description[ 0 ]       = UPPER( pMobIndex->description[ 0 ] );

	if ( ODPLUSKWIACZ )
	{
	    if ( !*pMobIndex->player_name )
		cbug( "Mob %d: brak slow kluczowych!", pMobIndex->vnum );

	    if ( !*pMobIndex->short_descr )
		cbug( "Mob %d: brak mianownika!", pMobIndex->vnum );

	    if ( !*pMobIndex->dopelniacz )
		cbug( "Mob %d: brak dopelniacza!", pMobIndex->vnum );

	    if ( !*pMobIndex->celownik )
		cbug( "Mob %d: brak celownika!", pMobIndex->vnum );

	    if ( !*pMobIndex->biernik )
		cbug( "Mob %d: brak biernika!", pMobIndex->vnum );

	    if ( !*pMobIndex->narzednik )
		cbug( "Mob %d: brak narzednika!", pMobIndex->vnum );

	    if ( !*pMobIndex->miejscownik )
		cbug( "Mob %d: brak miejscownika!", pMobIndex->vnum );

	    if ( !*pMobIndex->wolacz )
		cbug( "Mob %d: brak wolacza!", pMobIndex->vnum );
	}

	if ( wersje_krain[ area_last->version ].stare_flagi )
	{
	    pMobIndex->act                  = fread_number( fp, &stat ) | ACT_IS_NPC;
	    CLEAR_BITS( pMobIndex->affected_by );
	    pMobIndex->affected_by[ 0 ]     = fread_number( fp, &stat );
	    pMobIndex->pShop                = NULL;
	    pMobIndex->alignment            = fread_number( fp, &stat );
	    letter                          = fread_letter( fp );
	    pMobIndex->level                = fread_number( fp, &stat );

	    /*
	     * The unused stuff is for imps who want to use the old-style
	     * stats-in-files method.
	     * Lam: Lac tego nie uzywa, szkoda miejsca w plikach
	     */
	    if ( letter != 'L' )
	    {
		/* pMobIndex->hitroll     */ fread_number( fp, &stat );
		/* pMobIndex->ac          */ fread_number( fp, &stat );
		/* pMobIndex->hitnodice   */ fread_number( fp, &stat );
		/* 'd'                    */ fread_letter( fp );
		/* pMobIndex->hitsizedice */ fread_number( fp, &stat );
		/* '+'                    */ fread_letter( fp );
		/* pMobIndex->hitplus     */ fread_number( fp, &stat );
		/* pMobIndex->damnodice   */ fread_number( fp, &stat );
		/* 'd'                    */ fread_letter( fp );
		/* pMobIndex->damsizedice */ fread_number( fp, &stat );
		/* '+'                    */ fread_letter( fp );
		/* pMobIndex->damplus     */ fread_number( fp, &stat );
		/* pMobIndex->gold        */ fread_number( fp, &stat );
		/* xp                     */ fread_number( fp, &stat );
		/* position               */ fread_number( fp, &stat );
	    }
	    race                            = fread_string( fp, &stat );
	    pMobIndex->sex                  = fread_number( fp, &stat );
	}
	else
	{
	    pMobIndex->sex                  = fread_number( fp, &stat );

	    /* Vigud 5.3.2011; ->sex jest unsigned, nigdy nie bedzie < 0 */
	    if ( /* pMobIndex->sex < 0 || */ pMobIndex->sex > 2 )
		cbug( "Mob %d: bledna plec.", vnum );

	    pMobIndex->act                  = fread_number( fp, &stat ) | ACT_IS_NPC;
	    fread_vector( pMobIndex->affected_by, fp, &stat );
	    /* moze ma to malo wspolnego, ale akurat w tym samym momencie
	       pojawily sie miejscowniki, a zniklo to */
	    if ( !wersje_krain[ area_last->version ].pommiej )
		SET_BIT( pMobIndex->affected_by[ 1 ], fread_number( fp, &stat ) );

	    /* w poziomie bylo number_fuzzy( ), ale psulo sie przy asave */
	    pMobIndex->level                = fread_number( fp, &stat );

	    pMobIndex->alignment            = fread_number( fp, &stat );
	    pMobIndex->zamoznosc            = fread_number( fp, &stat );
	    race                            = fread_string( fp, &stat );
	}

	if ( ODPLUSKWIACZ )
	{
	    if ( !act_flag_avail( pMobIndex->act ) )
		cbug( "Mob %d: nie obslugiwane/niedozwolone flagi zachowania.", vnum );

	    if ( !affect_bit_avail( pMobIndex->affected_by ) )
		cbug( "Mob %d: nie obslugiwane/niedozwolone wplywy.", vnum );

	    if ( pMobIndex->alignment < -1000 || pMobIndex->alignment > 1000 )
		cbug( "Mob %d: moralnosc poza zakresem.", vnum );
	}

	pMobIndex->index_area->lowmoblevel	=
		UMIN( pMobIndex->index_area->lowmoblevel, pMobIndex->level );
	pMobIndex->index_area->highmoblevel	=
		UMAX( pMobIndex->index_area->highmoblevel, pMobIndex->level );

	pMobIndex->race = race_lookup( race );
	free_string( race );
	if ( pMobIndex->race < 0 )
	{
	    cbug( "Load_mobiles: vnum %d bad race.", vnum );
	    pMobIndex->race = 0;
	}

	pMobIndex->pShop		= NULL;
	pMobIndex->pHealer		= NULL;

	/* wczytywanie mobprogow (>) i innych pol opcjonalnych */
	/* To pomysl Sektora. Pozbywam sie niepotrzebnych sekcji. */
	for ( ; ; )
	{
	    letter = fread_letter( fp );
	    if ( letter == '*' ) /* komentarz -Lam */
		fread_to_eol( fp );
	    else if ( letter == '>' ) /* MOBProg */
	    {
		if ( pMobIndex->mobprogs )
		    cbug( "Mob %d: wiecej niz jedna \"sekcja\" progow (byc moze to tylko zbedna \"|\").", pMobIndex->vnum );
		ungetc( letter, fp );
		if ( mprog_read_programs( fp, pMobIndex, 0 ) )
		    return TRUE;
	    }
	    else if ( letter == 'F' ) /* spec_fun */
	    {
		pMobIndex->spec_fun = spec_lookup( fread_word( fp, &stat, TRUE ) );
		if ( !pMobIndex->spec_fun )
		{
		    cbug( "Load_mobiles: mob: %d: nieznana specjalnosc", pMobIndex->vnum );
		    return TRUE;
		    /* nalezy to poprawic w krainie zanim wczyta to konwerter */
		}
	    }
	    else if ( letter == 'H' ) /* sprzedawca czarow */
	    {
		HEALER_DATA *pHealer;
		int spnum;
		char *slowo;

		pHealer                 = new_healer( );
		pHealer->keeper         = pMobIndex->vnum;
		for ( spnum = 0; spnum <= 4; spnum++ )
		{
		    slowo = fread_word( fp, &stat, TRUE );
		    if ( !*slowo || !str_cmp( slowo, "xxx" ) )
			pHealer->spells[ spnum ] = -1;
		    else
		    {
			pHealer->spells[ spnum ] = skill_lookup_ang( slowo );
			if ( pHealer->spells[ spnum ] <= 0 )
			{
			    sprintf( bug_buf, "Sprzedawca czarow %d: niepoprawny czar %d: %s", pMobIndex->vnum, spnum, slowo );
			    cbug( bug_buf, 0 );
			}
		    }
		    pHealer->cost[ spnum ] = fread_number( fp, &stat );
		}

		pMobIndex->pHealer	= pHealer;
	    }
	    else if ( letter == 'S' ) /* sklep */
	    {
		SHOP_DATA *pShop;
		int        iTrade;

		if ( IS_SET( pMobIndex->act, ACT_SCAVENGER ) )
		    cbug( "Mob %d prowadzi sklep i jest kolekcjonerem", pMobIndex->vnum );

		pShop                   = new_shop( );
		pShop->keeper           = pMobIndex->vnum;
		for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
		{
		    pShop->buy_type[ iTrade ] = fread_number( fp, &stat );
		    if ( ODPLUSKWIACZ
		      && ( pShop->buy_type[ iTrade ] < 0
			|| pShop->buy_type[ iTrade ] >= ITEM_MAX
			|| ( pShop->buy_type[ iTrade ] != 0
			  && !*typy_przedmiotow[ pShop->buy_type[ iTrade ] ].name ) ) )
		    {
			sprintf( bug_buf, "Mob %d skupuje przedmiot blednego typu %d (ustaw 0, jesli nie ma skupowac).",
				pMobIndex->vnum, pShop->buy_type[ iTrade ] );
			cbug( bug_buf, 0 );
		    }
		}
		pShop->profit_buy       = fread_number( fp, &stat );
		pShop->profit_sell      = fread_number( fp, &stat );
		pShop->open_hour        = fread_number( fp, &stat );
		pShop->close_hour       = fread_number( fp, &stat );
		if ( ODPLUSKWIACZ && pShop->open_hour >= pShop->close_hour )
		    cbug( "Sklep moba %d nigdy nie jest otwarty.", pMobIndex->vnum );
		pShop->quest_shop	= FALSE;

		if ( pShop->profit_buy < 130 || pShop->profit_sell > 70 )
		{
		    sprintf( bug_buf, "Sklep u moba #%d, zakup %d, sprzedaz %d!",
			pMobIndex->vnum, pShop->profit_buy, pShop->profit_sell );
		    cbug( bug_buf, 0 );
		}

		pMobIndex->pShop        = pShop;
	    }
	    else if ( letter == 'G' ) /* grupa mobow */
	    {
		pMobIndex->grupa = fread_number( fp, &stat );
		if ( area_last->lowgroupvnum )
		    area_last->lowgroupvnum = UMIN( area_last->lowgroupvnum, pMobIndex->grupa );
		else
		    area_last->lowgroupvnum = pMobIndex->grupa;
		area_last->highgroupvnum = UMAX( area_last->highgroupvnum, pMobIndex->grupa );
	    }
	    else if ( letter == 'A' ) /* wplyw */
	    {
		if ( fread_number( fp, &stat ) != APPLY_SIZE )
		    cbug( "Mob %d: wplyw nie na wielkosc? Konwertuje na wielkosc.", pMobIndex->vnum );
		pMobIndex->mod_size = fread_number( fp, &stat );
		if ( pMobIndex->mod_size < -1 || pMobIndex->mod_size > 1 )
		{
		    cbug( "Mob %d: za duza modyfikacja wielkosci.", pMobIndex->vnum );
		    pMobIndex->mod_size = URANGE( -1, pMobIndex->mod_size, 1 );
		}
		if ( race_table[ pMobIndex->race ].size + pMobIndex->mod_size < 0 )
		{
		    cbug( "Mob %d: modyfikacja wielkosci niemozliwa, usuwam wplyw.", pMobIndex->vnum );
		    pMobIndex->mod_size = 0;
		}
	    }
	    else if ( letter == 'K' )
	    {
		pMobIndex->game_fun = game_lookup( fread_word( fp, &stat, TRUE ) );
		if ( !pMobIndex->game_fun )
		{
		    cbug( "Load_mobiles: mob: %d: nieznana gra", pMobIndex->vnum );
		    return TRUE;
		}
	    }
	    else
	    {
		ungetc( letter, fp );
		break;
	    }
	}

	if ( ODPLUSKWIACZ )
	{
	    /* nie bedzie na tyle inteligentny, zeby sprawdzic, czy ten
	       descr_prog cokolwiek pokazuje, ale lepsze to niz ostrzezenia, ze
	       nie ma opisu, kiedy ma descr_proga */
	    if ( !*pMobIndex->description
	      && !IS_SET( pMobIndex->progtypes, M_DESCR_PROG ) )
		cbug( "Mob %d: brak opisu!", pMobIndex->vnum );

	    if ( !*pMobIndex->long_descr && pMobIndex->vnum != MOB_VNUM_SUPERMOB
	      && !IS_SET( pMobIndex->progtypes, M_LONG_DESCR_PROG ) )
		cbug( "Mob %d: brak dlugiego opisu!", pMobIndex->vnum );

	    if ( *pMobIndex->long_descr
	      && IS_SET( pMobIndex->progtypes, M_LONG_DESCR_PROG ) )
		cbug( "Mob %d: dlugi opis oprocz long_descr_proga.", pMobIndex->vnum );

	    if ( pMobIndex->pShop && pMobIndex->pHealer )
		cbug( "Mob %d: sklep z przedmiotami i sprzedawca czarow (wybierz jedno!)",
		    pMobIndex->vnum );

	    if ( IS_SET( pMobIndex->act, ACT_NIE_POMAGA )
	      && !pMobIndex->grupa )
		cbug( "Mob %d: zachowanie nie_pomaga, ale wcale nie jest w grupie.",
		    pMobIndex->vnum );
	}
    }

    return FALSE;
}


/*
 * Snarf an obj section.
 */
bool load_objects( FILE *fp )
{
    OBJ_INDEX_DATA *pObjIndex;

    if ( !area_last )
    {
	cbug( "Load_objects: no #AREA seen yet.", 0 );
	return TRUE;
    }

    for ( ; ; )
    {
	char *value[ 6 ];
	int   avalue = 0;
	char  letter;
	int   vnum;
	int   stat;

	letter = fread_letter( fp );
	if ( letter != '#' )
	{
	    cbug( "Load_objects: # not found.", 0 );
	    return TRUE;
	}

	vnum = fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	if ( vnum < 0 || vnum >= MAX_VNUM )
	{
	    cbug( "Load_objects: vnum %d poza zakresem.", vnum );
	    return TRUE;
	}

	pObjIndex                       = new_obj_index( vnum, area_last );
	if ( !pObjIndex )
	    return TRUE;

	pObjIndex->name                 = fread_string( fp, &stat );
	pObjIndex->short_descr          = fread_string( fp, &stat );
	if ( wersje_krain[ area_last->version ].plusy )
	{
	    if ( *pObjIndex->short_descr == '+' )
	    {
		free_string( pObjIndex->short_descr );
		pObjIndex->short_descr  = fread_string( fp, &stat );
		pObjIndex->dopelniacz   = fread_string( fp, &stat );
		pObjIndex->celownik     = fread_string( fp, &stat );
		pObjIndex->biernik      = fread_string( fp, &stat );
		pObjIndex->narzednik    = fread_string( fp, &stat );
		pObjIndex->miejscownik  = fread_string( fp, &stat );
	    }
	    else
	    {
		pObjIndex->dopelniacz   = str_dup( pObjIndex->short_descr );
		pObjIndex->celownik     = str_dup( pObjIndex->short_descr );
		pObjIndex->biernik      = str_dup( pObjIndex->short_descr );
		pObjIndex->narzednik    = str_dup( pObjIndex->short_descr );
		pObjIndex->miejscownik  = str_dup( pObjIndex->short_descr );
	    }
	}
	else
	{
	    pObjIndex->dopelniacz       = fread_string( fp, &stat );
	    pObjIndex->celownik         = fread_string( fp, &stat );
	    pObjIndex->biernik          = fread_string( fp, &stat );
	    pObjIndex->narzednik        = fread_string( fp, &stat );
	    pObjIndex->miejscownik      = fread_string( fp, &stat );
	}

	pObjIndex->description          = fread_string( fp, &stat );
/* Lam: action description przedmiotow, w Envy nieuzywane */
	if ( wersje_krain[ area_last->version ].opisprz )
	    pObjIndex->look_descr       = fread_string( fp, &stat );
	else
	    pObjIndex->action           = fread_string( fp, &stat );

/*      pObjIndex->short_descr[ 0 ]       = LOWER( pObjIndex->short_descr[ 0 ] );*/
	pObjIndex->description[ 0 ]       = UPPER( pObjIndex->description[ 0 ] );

	if ( !wersje_krain[ area_last->version ].stare_flagi )
	{
	    pObjIndex->rodzaj		= fread_number( fp, &stat );
	    if ( wersje_krain[ area_last->version ].wielkosc )
		pObjIndex->wielkosc	= fread_number( fp, &stat );
	}
	pObjIndex->item_type            = fread_number( fp, &stat );
	if ( pObjIndex->item_type >= ITEM_MAX
	  || pObjIndex->item_type < 0
	  || !typy_przedmiotow[ pObjIndex->item_type ].name
	  || !*typy_przedmiotow[ pObjIndex->item_type ].name )
	    cbug( "Przedmiot %d: zly typ przedmiotu!", vnum );
	if ( wersje_krain[ area_last->version ].stare_flagi )
	{
	    CLEAR_BITS( pObjIndex->extra_flags );
	    pObjIndex->extra_flags[ 0 ] = fread_number( fp, &stat );
	}
	else
	    fread_vector( pObjIndex->extra_flags, fp, &stat );
	pObjIndex->wear_flags           = fread_number( fp, &stat );
	if ( pObjIndex->wear_flags >= b16 )
	    cbug( "Przedmiot %d: nie obslugiwana flaga ubrania!", vnum );
	if ( ODPLUSKWIACZ
	  && !pObjIndex->wielkosc
	  /* prowizorka; dziala, dopoki ktos nie pozmienia ITEM_WEAR_* */
	  && ( ( pObjIndex->wear_flags > ITEM_TAKE
	      && pObjIndex->wear_flags < ITEM_WIELD )
	    || IS_SET( pObjIndex->wear_flags, ITEM_WEAR_FACE ) ) )
	    cbug( "Przedmiot %d: zakladalny, ale bez wielkosci!", vnum );
	if ( ODPLUSKWIACZ
	  && pObjIndex->wielkosc
	  /* komentarz jw. */
	  && ( pObjIndex->wear_flags <= ITEM_TAKE
	    || IS_SET( pObjIndex->wear_flags, ITEM_WIELD )
	    || IS_SET( pObjIndex->wear_flags, ITEM_HOLD ) ) )
	    cbug( "Przedmiot %d: nie zakladalny, a ma ustawiana wielkosc!", vnum );

	/* Lam 27.10.2004 */
	if ( ODPLUSKWIACZ )
	{
	    int biciki = pObjIndex->item_type == ITEM_LIGHT ? 1 : 0;
	    int ubranka = pObjIndex->wear_flags;
	    REMOVE_BIT( ubranka, ITEM_TAKE );

	    /* swiatla moga swiecic na ziemi, ale prawdziwe flagi nie maja
	       sensu bez ITEM_TAKE */
	    if ( ubranka && ubranka == pObjIndex->wear_flags )
		cbug( "Przedmiot %d: mozna zakladac, ale nie da sie wziac w rece", vnum );

	    while ( ubranka )
	    {
		if ( ubranka & 1 )
		    biciki++;
		ubranka >>= 1;
	    }
	    if ( biciki > 1 )
		cbug( "Przedmiot %d: Lac nie obsluguje przedmiotow ubieranych w kilku miejscach.", vnum );

	    if ( pObjIndex->item_type != ITEM_WEAPON
	      && IS_SET( pObjIndex->wear_flags, ITEM_WIELD ) )
		cbug( "Przedmiot %d dobywany jako bron, ale nie typu bron", vnum );
	}

	for ( avalue = 0; avalue < wersje_krain[ area_last->version ].wartosci; avalue++ )
	    value[ avalue ] = fread_string( fp, &stat );
	/*
	 * Brottor, 29.12.2007: po powyzszym, avalue wynosi tyle, ile wartosci
	 * przewidziano w wersji krainy, w jakiej napisano area_last. Jesli to
	 * jest mniej niz 6, reszte wartosci trzeba wyczyscic.
	 */
	for ( ; avalue < 6; avalue++ )
	    value[ avalue ] = "";

	if ( wersje_krain[ area_last->version ].stare_flagi )
	{
	    pObjIndex->weight               = fread_number( fp, &stat );
	    /* cost */			      fread_number( fp, &stat );
	    pObjIndex->rodzaj               = fread_number( fp, &stat );
	    pObjIndex->anti_race            = str_dup( "" );
	    pObjIndex->anti_class           = str_dup( "" );
	}
	else
	{
	    pObjIndex->weight               = fread_number( fp, &stat );
	    pObjIndex->anti_race            = fread_string( fp, &stat );
	    pObjIndex->anti_class           = fread_string( fp, &stat );
	}

	if ( pObjIndex->rodzaj < 0 || pObjIndex->rodzaj > 3 )
	{
	    cbug( "Przedmiot %d zawiera nieistniejacy rodzaj gramatyczny.", vnum );
	    pObjIndex->rodzaj = 0;
	}

	if ( ODPLUSKWIACZ && !extra_bit_avail( pObjIndex->extra_flags ) )
	    cbug( "Przedmiot %d zawiera nie obslugiwane flagi.", vnum );

	/*
	 * Check here for the redundancy of invisible light sources - Kahn
	 */
	if ( ODPLUSKWIACZ
	  && pObjIndex->item_type == ITEM_LIGHT
	  && IS_SET_V( pObjIndex->extra_flags, ITEM_INVIS ) )
	{
	    cbug( "Przedmiot %d jest niewidzialnym zrodlem swiatla, zdejmuje niewidke.", vnum );
	    REMOVE_BIT_V( pObjIndex->extra_flags, ITEM_INVIS );
	}
	/*
	 * Lam: to samo z zakopanymi
	 */
	if ( ODPLUSKWIACZ
	  && pObjIndex->item_type == ITEM_LIGHT
	  && IS_SET_V( pObjIndex->extra_flags, ITEM_BURIED ) )
	{
	    cbug( "Przedmiot %d jest zakopanym zrodlem swiatla, odkopuje go.", vnum );
	    REMOVE_BIT_V( pObjIndex->extra_flags, ITEM_BURIED );
	}

	pObjIndex->cena = 0;
	pObjIndex->poziom = 0;
	pObjIndex->qp = 0;
	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( fp );

	    if ( letter == '*' ) /* komentarze - Lam jak wszedzie */
		fread_to_eol( fp );

	    else if ( letter == '>' )
	    {
		if ( pObjIndex->mobprogs )
		    cbug( "Przedmiot %d: wiecej niz jedna \"sekcja\" progow (byc moze to tylko zbedna \"|\").", vnum );
		ungetc( letter, fp );
		if ( mprog_read_programs( fp, pObjIndex, 3 ) )
		    return TRUE;
	    }

	    else if ( letter == 'A' )
	    {
		AFFECT_DATA *paf, *tpaf;

		if ( ODPLUSKWIACZ
		  && IS_SET_V( pObjIndex->extra_flags, ITEM_QUEST ) )
		{
		    sprintf( bug_buf, "Przedmiot zadaniowy %d ma wplywy.", vnum );
		    cbug( bug_buf, 0 );
		}

		paf                     = alloc_perm( sizeof( *paf ) );
		paf->type               = 0;
		paf->duration           = -1;
		paf->location           = fread_number( fp, &stat );
		if ( ODPLUSKWIACZ && !affect_loc_avail( paf->location ) )
		    cbug( "Przedmiot %d: niedopuszczalny/nie obslugiwany wplyw 'A'.",
			vnum );
		if ( wersje_krain[ area_last->version ].dodkp
		  || ( paf->location != APPLY_AC
		    && paf->location != APPLY_SAVING_PARA
		    && paf->location != APPLY_SAVING_ROD
		    && paf->location != APPLY_SAVING_PETRI
		    && paf->location != APPLY_SAVING_BREATH
		    && paf->location != APPLY_SAVING_SPELL ) )
		    paf->modifier           = fread_number( fp, &stat );
		else
		    paf->modifier           = 0 - fread_number( fp, &stat );
		CLEAR_BITS( paf->bitvector );
		paf->level		= 0;
		paf->caster		= NULL;
		paf->next		= NULL;
		if ( pObjIndex->affected )
		{
		    if ( ODPLUSKWIACZ && pObjIndex->affected->location == paf->location )
		    {
			sprintf( bug_buf, "Przedmiot %d ma dwa wplywy na %s.",
			    vnum, affect_loc_name_pl_b( paf->location ) );
			cbug( bug_buf, 0 );
		    }

		    for ( tpaf = pObjIndex->affected; tpaf->next; tpaf = tpaf->next )
			if ( ODPLUSKWIACZ && tpaf->next->location == paf->location )
			{
			    sprintf( bug_buf, "Przedmiot %d ma dwa wplywy na %s.",
				vnum, affect_loc_name_pl_b( paf->location ) );
			    cbug( bug_buf, 0 );
			}
		    tpaf->next		= paf;
		}
		else
		    pObjIndex->affected	= paf;
		top_affect++;
	    }

	    /* dodatkowe opisy znikaja. "E" tylko dla starych krain */
	    else if ( letter == 'E' )
	    {
		char *opis, *slowa;

		if ( wersje_krain[ area_last->version ].opisprz )
		    cbug( "Przedmiot %d: dodatkowe opisy 'E' nie sa juz obslugiwane. Dodatkowy opis zastapi prawdziwy opis albo zostanie usuniety.", vnum );

		slowa = fread_string( fp, &stat );
		opis = fread_string( fp, &stat );
		if ( strcmp( slowa, pObjIndex->name ) )
		{
		    sprintf( bug_buf, "Przedmiot %d: slowa kluczowe dodatkowego opisu (%s) niezgodne z przedmiotem (%s). Usuwam opis.",
			vnum, slowa, pObjIndex->name );
		    cbug( bug_buf, 0 );
		    free_string( opis );
		}
		else
		{
		    if ( pObjIndex->look_descr && *pObjIndex->look_descr )
			cbug( "Przedmiot %d: wiele identycznych opisow 'E' lub opis 'E' po podaniu prawdziwego opisu, jako opis przedmiotu pozostawiam tylko ostatni opis 'E'.", vnum );
		    pObjIndex->look_descr = opis;
		}
		free_string( slowa );
	    }

	    else if ( letter == 'U' )
	    {
		pObjIndex->action       = fread_string( fp, &stat );
	    }

	    else if ( letter == 'X' )
	    {
		pObjIndex->cena		= fread_number( fp, &stat );
		pObjIndex->poziom	= fread_number( fp, &stat );
	    }

	    /* Lam 9.4.2000 */
	    else if ( letter == 'Q' )
	    {
		pObjIndex->qp		= fread_number( fp, &stat );
	    }

	    else
	    {
		ungetc( letter, fp );
		break;
	    }
	}

	/* Lam 19.1.2004 na prosbe Jemkita */
	if ( ODPLUSKWIACZ && is_name( "r`o`zd`zk", pObjIndex->name )
	  && pObjIndex->item_type != ITEM_WAND )
	{
	    sprintf( bug_buf, "Przedmiot %d - rozdzka '%s' (%s) nie typu ITEM_WAND",
		vnum, pObjIndex->short_descr, pObjIndex->name );
	    cbug( bug_buf, 0 );
	}

	/* Lam 6.7.2005 */
	if ( ODPLUSKWIACZ )
	{
	    if ( IS_OBJ_STAT( pObjIndex, ITEM_INVERT_RACES )
	      && !*pObjIndex->anti_race )
		cbug( "Przedmiot %d nie pasuje na zadne rasy.", vnum );

	    if ( IS_OBJ_STAT( pObjIndex, ITEM_INVERT_CLASSES )
	      && !*pObjIndex->anti_class )
		cbug( "Przedmiot %d nie pasuje do zadnych profesji.", vnum );

	    if ( *pObjIndex->anti_race )
	    {
		int rasa;
		char *lista;
		char slowo[ MIL ];

		lista = pObjIndex->anti_race;
		for ( lista = one_argument( lista, slowo ); *slowo; lista = one_argument( lista, slowo ) )
		{
		    for ( rasa = 0; rasa < MAX_RACE; rasa++ )
			if ( !str_cmp( race_table[ rasa ].ang, slowo ) )
			    break;

		    if ( rasa == MAX_RACE )
		    {
			sprintf( bug_buf, "Przedmiot %d (nie) pasuje na nieznana rase \"%s\".",
				vnum, slowo );
			cbug( bug_buf, 0 );
		    }
		}
	    }

	    if ( *pObjIndex->anti_class )
	    {
		int prof;
		char *lista;
		char slowo[ MIL ];

		lista = pObjIndex->anti_class;
		for ( lista = one_argument( lista, slowo ); *slowo; lista = one_argument( lista, slowo ) )
		{
		    for ( prof = 0; prof < MAX_CLASS; prof++ )
			if ( !str_cmp( class_table[ prof ].who_name, slowo ) )
			    break;

		    if ( prof == MAX_CLASS )
		    {
			sprintf( bug_buf, "Przedmiot %d (nie) pasuje do nieznanej profesji \"%s\".",
				vnum, slowo );
			cbug( bug_buf, 0 );
		    }
		}
	    }

	    if ( pObjIndex->affected
	      && ( !IS_SET( pObjIndex->wear_flags, ITEM_TAKE )
		|| ( pObjIndex->item_type != ITEM_LIGHT
		  && ( pObjIndex->wear_flags & ~ITEM_TAKE ) == 0 ) ) )
	    {
		cbug( "Przedmiot %d ma wplywy, ale nie pozwala sie zakladac.",
			vnum );
	    }

	    if ( !*pObjIndex->description
	      && !IS_SET( pObjIndex->progtypes, O_LONG_DESCR_PROG ) )
		cbug( "Przedmiot %d: brak opisu w pomieszczeniu!", pObjIndex->vnum );

	    /* zbyt wiele przedmiotow nie ma opisu, wiec jest na to za wczesnie
	    if ( !*pObjIndex->look_descr
	      && !IS_SET( pObjIndex->progtypes, O_DESCR_PROG ) )
		cbug( "Przedmiot %d: brak opisu!", pObjIndex->vnum ); */
	}

	/*
	 * Translate character strings *value[] into integers:  sn's for
	 * items with spells, or straight conversion for other items.
	 * - Thelonius
	 */
	for ( avalue = 0; avalue < wersje_krain[ area_last->version ].wartosci; avalue++ )
	    pObjIndex->value[ avalue ].v = atoi( value[ avalue ] );

	switch ( pObjIndex->item_type )
	{
	default:
	    break;

	case ITEM_FOUNTAIN:
	    if ( ODPLUSKWIACZ
	      && ( pObjIndex->value[ 5 ].v < 0
		|| pObjIndex->value[ 5 ].v > LIQ_MAX ) )
		cbug( "Fontanna o vnumie %d zawiera nieistniejacy plyn.", vnum );
	    break;

	case ITEM_FOOD:
	    /* Alandar 04.10.2004 - obsluga wartosci przedmiotow bedacych opisami */
	    if ( *value[ 4 ] && str_cmp( value[ 4 ], "0" ) )
		pObjIndex->value[ 4 ].p = ( void * ) str_dup( value[ 4 ] );
	    else
		pObjIndex->value[ 4 ].p = NULL;
	    break;

	case ITEM_DRINK_CON:
	    /* Alandar 04.10.2004 - obsluga wartosci przedmiotow bedacych opisami */
	    if ( *value[ 4 ] && str_cmp( value[ 4 ], "0" ) )
		pObjIndex->value[ 4 ].p = ( void * ) str_dup( value[ 4 ] );
	    else
		pObjIndex->value[ 4 ].p = NULL;
	    if ( ODPLUSKWIACZ )
	    {
		if ( pObjIndex->value[ 2 ].v < 0 || pObjIndex->value[ 2 ].v > LIQ_MAX )
		    cbug( "Pojemnik na plyn o vnumie %d zawiera nieistniejacy plyn.", vnum );
		if ( pObjIndex->value[ 0 ].v < 1 )
		    cbug( "Pojemnik na plyn o vnumie %d nie ma pojemnosci.", vnum );
	    }
	    break;

	case ITEM_TREASURE:
	    pObjIndex->value[ 5 ].v = material_lookup( value[ 5 ] );
	    if ( ODPLUSKWIACZ )
	    {
		if ( !pObjIndex->value[ 5 ].v )
		    cbug( "Skarb %d bez materialu.", vnum );
		else if ( pObjIndex->value[ 5 ].v < 0 )
		{
		    sprintf( bug_buf, "Skarb %d z nieznanym materialem %s, ustawiam \"nic\".",
			vnum, value[ 5 ] );
		    cbug( bug_buf, 0 );
		    pObjIndex->value[ 5 ].v = 0; /* nic */
		}
		if ( pObjIndex->value[ 1 ].v )
		    cbug( "Skarb %d: skarby musza miec 0 w wartosci 1, zeruje.", vnum );
	    }
	    else if ( pObjIndex->value[ 5 ].v < 0 )
		pObjIndex->value[ 5 ].v = 0; /* nic */
	    pObjIndex->value[ 1 ].v = 0;
	    break;

	case ITEM_ARMOR:
	    pObjIndex->value[ 5 ].v = material_lookup( value[ 5 ] );
	    if ( ODPLUSKWIACZ )
	    {
		if ( !pObjIndex->value[ 5 ].v )
		    cbug( "Zbroja %d bez materialu.", vnum );
		else if ( pObjIndex->value[ 5 ].v < 0 )
		{
		    sprintf( bug_buf, "Zbroja %d z nieznanym materialem %s, ustawiam \"nic\".",
			vnum, value[ 5 ] );
		    cbug( bug_buf, 0 );
		    pObjIndex->value[ 5 ].v = 0; /* nic */
		}
	    }
	    else if ( pObjIndex->value[ 5 ].v < 0 )
		pObjIndex->value[ 5 ].v = 0; /* nic */
	    break;

	case ITEM_WEAPON:
	    pObjIndex->value[ 5 ].v = material_lookup( value[ 5 ] );
	    if ( !pObjIndex->value[ 3 ].v )
		cbug( "Bron %d bez typu obrazen.", vnum );
	    if ( ODPLUSKWIACZ )
	    {
		if ( !pObjIndex->value[ 5 ].v )
		    cbug( "Bron %d bez materialu.", vnum );
		else if ( pObjIndex->value[ 5 ].v < 0 )
		{
		    sprintf( bug_buf, "Bron %d z nieznanym materialem %s, ustawiam \"nic\".",
			vnum, value[ 5 ] );
		    cbug( bug_buf, 0 );
		    pObjIndex->value[ 5 ].v = 0; /* nic */
		}
	    }
	    else if ( pObjIndex->value[ 5 ].v < 0 )
		pObjIndex->value[ 5 ].v = 0; /* nic */
	    break;

	case ITEM_SPELLBOOK:
	    pObjIndex->value[ 0 ].v = skill_lookup_ang( value[ 0 ] );
	    break;

	case ITEM_PILL:
	case ITEM_POTION:
	    if ( *value[ 4 ] && str_cmp( value[ 4 ], "0" ) )
		pObjIndex->value[ 4 ].p = ( char * ) str_dup( value[ 4 ] );
	    else
		pObjIndex->value[ 4 ].p = NULL;
	/* brak break celowy */
	case ITEM_SCROLL:
	    pObjIndex->value[ 1 ].v = skill_lookup_ang( value[ 1 ] );
	    pObjIndex->value[ 2 ].v = skill_lookup_ang( value[ 2 ] );
	    pObjIndex->value[ 3 ].v = skill_lookup_ang( value[ 3 ] );
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
	    pObjIndex->value[ 1 ].v = atoi( value[ 1 ] );
	    pObjIndex->value[ 2 ].v = atoi( value[ 2 ] );
	    if ( pObjIndex->value[ 2 ].v > pObjIndex->value[ 1 ].v )
	    {
		sprintf( log_buf, "Przedmiot %d - rozdzka/bulawa '%s' z %d ladunkami na %d.",
		    vnum, pObjIndex->short_descr,
		    pObjIndex->value[ 2 ].v, pObjIndex->value[ 1 ].v );
		cbug( log_buf, 0 );
	    }
	    pObjIndex->value[ 3 ].v = skill_lookup_ang( value[ 3 ] );
	    if ( pObjIndex->value[ 3 ].v < 0
	      || pObjIndex->value[ 3 ].v > MAX_SKILL )
	    {
		sprintf( log_buf, "Przedmiot %d - rozdzka/bulawa '%s' z nieznanym czarem '%s'.",
		    vnum, pObjIndex->short_descr, value[ 3 ] );
		cbug( log_buf, 0 );
	    }
	    else if ( skill_table[ pObjIndex->value[ 3 ].v ].spell_fun == NULL
		   || skill_table[ pObjIndex->value[ 3 ].v ].spell_fun == spell_null )
	    {
		sprintf( log_buf, "Przedmiot %d - rozdzka/bulawa '%s' z umiejetnoscia nie bedaca czarem '%s'.",
		    vnum, pObjIndex->short_descr, value[ 3 ] );
		cbug( log_buf, 0 );
	    }
	    else if ( pObjIndex->item_type == ITEM_WAND )
	    {
		if ( skill_table[ pObjIndex->value[ 3 ].v ].target == TAR_IGNORE )
		{
		    sprintf( log_buf, "Przedmiot %d - rozdzka '%s' z czarem TAR_IGNORE '%s'",
			vnum, pObjIndex->short_descr,
			skill_table[ pObjIndex->value[ 3 ].v ].pl_name );
		    cbug( log_buf, 0 );
		}
		if ( skill_table[ pObjIndex->value[ 3 ].v ].target == TAR_CHAR_SELF )
		{
		    sprintf( log_buf, "Przedmiot %d - rozdzka '%s' z czarem TAR_CHAR_SELF '%s'",
			vnum, pObjIndex->short_descr,
			skill_table[ pObjIndex->value[ 3 ].v ].pl_name );
		    cbug( log_buf, 0 );
		}
	    }
	    else if ( pObjIndex->item_type == ITEM_STAFF
	      && skill_table[ pObjIndex->value[ 3 ].v ].target == TAR_OBJ_INV )
	    {
		sprintf( log_buf, "Przedmiot %d - bulawa '%s' z czarem TAR_OBJ_INV '%s'",
		    vnum, pObjIndex->short_descr,
		    skill_table[ pObjIndex->value[ 3 ].v ].pl_name );
		cbug( log_buf, 0 );
	    }
	    break;

	case ITEM_BANDYTA:
	    if ( pObjIndex->value[ 1 ].v < 3 || pObjIndex->value[ 1 ].v > 5 )
		cbug( "Jednoreki bandyta (vnum %d) musi miec od 3 do 5 okienek.", vnum );
	    break;

	case ITEM_LIGHT:
	    if ( ODPLUSKWIACZ )
	    {
		if ( pObjIndex->value[ 1 ].v && pObjIndex->value[ 2 ].v < 0 )
		    cbug( "Zrodlo swiatla (vnum %d) nie magiczne, ale nieskonczone.", vnum );
		if ( pObjIndex->value[ 2 ].v == 0 )
		    cbug( "Zrodlo swiatla (vnum %d) jest niesprawne.", vnum );
	    }
	    break;
	}

	/* tego w Envy nie bylo */
	for ( avalue = 0; avalue < wersje_krain[ area_last->version ].wartosci; avalue++ )
	    free_string( value[ avalue ] );
    }

    return FALSE;
}


/*
 * Snarf a reset section.
 */
bool load_resets( FILE *fp )
{
    RESET_DATA *pReset;
    OBJ_INDEX_DATA *oid = NULL;
    MOB_INDEX_DATA *mid = NULL;
    int poziom = -1;

    if ( !area_last )
    {
	cbug( "Load_resets: no #AREA seen yet.", 0 );
	return TRUE;
    }

    for ( ; ; )
    {
	EXIT_DATA       *pexit;
	ROOM_INDEX_DATA *pRoomIndex;
	char             letter;
	int              stat;
	char		*rstring = "";

	if ( ( letter = fread_letter( fp ) ) == 'S' )
	    break;

	if ( letter == '*' )
	{
	    rstring = fread_string_eol( fp, &stat );
	    /* Lam 5.1.2003: zeby nie powtarzal tego tekstu */
	    if ( !str_prefix( " Krain`e zapisa`l", rstring ) )
	    {
		free_string( rstring );
		continue;
	    }
	}

	pReset = new_reset( );
	pReset->command = letter;
	if ( letter == '*' )
	    pReset->string = rstring;
	else
	{
	    if ( wersje_krain[ area_last->version ].zbres )
	    {
		pReset->arg1 = fread_number( fp, &stat );

		if ( letter == 'D' )
		    pReset->arg2 = drzwi_d( fread_word( fp, &stat, TRUE ) ); /* Lam */
		else if ( letter == 'M' || letter == 'R' )
		    pReset->arg2 = fread_number( fp, &stat );
		else if ( wersje_krain[ area_last->version ].pozwo && letter == 'O' )
		    pReset->arg2 = fread_number( fp, &stat );
		else
		    pReset->arg2 = 0;

		if ( letter == 'M' || letter == 'O'
		  || letter == 'D' || letter == 'E' )
		    pReset->arg3 = fread_number( fp, &stat );
		else
		    pReset->arg3 = 0;

		fread_to_eol( fp );
	    }
	    else
	    {
		/* if_flag */  fread_number( fp, &stat );
		pReset->arg1 = fread_number( fp, &stat );
		if ( letter == 'D' )
		    pReset->arg2 = drzwi_d( fread_word( fp, &stat, TRUE ) ); /* Lam */
		else
		    pReset->arg2 = fread_number( fp, &stat );
		pReset->arg3 = ( letter == 'G' || letter == 'R' )
				    ? 0 : fread_number( fp, &stat );
		fread_to_eol( fp );
	    }
	}

	/*
	 * Validate parameters.
	 * We're calling the index functions for the side effect.
	 */
	switch ( letter )
	{
	default:
	    cbug( "Load_resets: bad command '%c'.", letter );
	    return TRUE;
	    break;

	case '*':
	    break;

	case 'M':
	    oid = NULL;
	    mid = get_mob_index( pReset->arg1 );
	    poziom = mid->level;
	    mid->resetcount++;
	    get_room_index( pReset->arg3 );
	    break;

	case 'O':
	    mid = NULL;
	    oid = get_obj_index( pReset->arg1 );
	    get_room_index( pReset->arg3 );

	    if ( !wersje_krain[ area_last->version ].pozwo )
		pReset->arg2 = URANGE( 0, poziom - 2, LEVEL_HERO );

	    /* zabezpieczenie przed uzywaniem mobow-kolekcjonerow do zbierania
	       pieniedzy z ziemi */
	    if ( oid->item_type == ITEM_MONEY
	      && !IS_SET_V( oid->extra_flags, ITEM_BURIED ) )
		cbug( "Load_resets: 'O': skarb %d lezy na ziemi.", pReset->arg1 );
	    break;
	case 'P':
	    get_obj_index( pReset->arg1 );
	    if ( !oid )
	    {
		cbug( "Load_resets: 'P' nie moze wystepowac na poczatku sekcji ani bezposrednio po 'M'", 0 );
		break;
	    }
	    /* w starych (z Envy) krainach bywaly 'P' na przedmiocie innym
	       niz poprzedni, w naszym formacie najpierw wprowadzilismy
	       wymog zgodnosci (co wykrylo bledne krainy), aktualnie w ogole
	       nie trzeba podawac vnuma "celu" */
	    if ( !wersje_krain[ area_last->version ].zbres
	      && oid != get_obj_index( pReset->arg3 ) )
		cbug( "Load_resets: 'P': vnum docelowego przedmiotu niezgodny z poprzednim przedmiotem", 0 );
	    if ( ODPLUSKWIACZ
	      && oid->item_type != ITEM_CONTAINER
	      && oid->item_type != ITEM_CORPSE_NPC
	      && oid->item_type != ITEM_CORPSE_PC ) /* na wszelki wypadek */
	    {
		sprintf( bug_buf,
			"Load_resets: 'P': wkladanie przedmiotu %d w przedmiot %d, ktory nie jest pojemnikiem.",
			pReset->arg1, oid->vnum );
		cbug( bug_buf, 0 );
	    }
	    break;

	case 'G':
	case 'E':
	    oid = get_obj_index( pReset->arg1 );
	    if ( !mid )
		cbug( "Load_resets: 'G'/'E' musza wystepowac po 'M' (a nie na poczatku sekcji lub po 'O')", 0 );
	    if ( !CAN_WEAR( oid, ITEM_TAKE ) )
		cbug( "Load_resets: 'G'/'E': przedmiotu %d nie mozna podnosic.", pReset->arg1 );
	    if ( letter == 'E' )
	    {
		if ( pReset->arg3 >= MAX_WEAR )
		{
		    sprintf( bug_buf, "Load_resets: 'E': przedmiot %d zakladany w niedopuszczalnym miejscu %d!",
			pReset->arg1, pReset->arg3 );
		    cbug( bug_buf, 0 );
		}
		else if ( !can_equip_index_at( oid, pReset->arg3 ) )
		{
		    sprintf( bug_buf, "Load_resets: 'E': przedmiotu %d nie wolno zakladac w miejscu %d.",
			pReset->arg1, pReset->arg3 );
		    cbug( bug_buf, 0 );
		}
	    }
	    break;

	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0
	      || pReset->arg2 > 9 /* bylo 5 */
	      || !( pexit = pRoomIndex->exit[ pReset->arg2 ] )
	      || !IS_SET( pexit->exit_info, EX_ISDOOR ) )
	    {
		cbug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
		return TRUE;
	    }

	    if ( pReset->arg3 < 0 || pReset->arg3 > 4 )
	    {
		cbug( "Load_resets: 'D': bad 'locks': %d.", pReset->arg3 );
		return TRUE;
	    }

	    if ( pReset->arg3 == 3
	      || pReset->arg3 == 4 )
	    {
		if ( pReset->arg2 != DIR_DOWN )
		{
		    sprintf( log_buf, "Load_resets: 'D': %d:%d - zakopywanie wyjscia nie w dol", pReset->arg1, pReset->arg2 );
		    cbug( log_buf, 0 );
		}

		if ( !IS_SET( pexit->exit_info, EX_DIG ) )
		{
		    sprintf( log_buf, "Load_resets: 'D': %d:%d - zakopywanie wyjscia bez EX_DIG", pReset->arg1, pReset->arg2 );
		    cbug( log_buf, 0 );
		}
	    }

	    break;

	case 'R':
	    get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0 || pReset->arg2 > 9 ) /* bylo 6 */
	    {
		cbug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
		return TRUE;
	    }

	    break;
	}

	if ( !area_last->reset_first )
	    area_last->reset_first = pReset;
	if ( area_last->reset_last )
	    area_last->reset_last->next = pReset;

	area_last->reset_last = pReset;
	pReset->next = NULL;
    }

    return FALSE;
}


/* Vigud 10.3.2010 */
OBJ_INDEX_DATA *new_obj_index( int vnum, AREA_DATA *area )
{
    OBJ_INDEX_DATA *pObjIndex;
    ZONE_LIST *lista;
    int iHash;

    pObjIndex				= alloc_perm( sizeof( *pObjIndex ) );
    pObjIndex->vnum			= vnum;
    pObjIndex->area			= area;
    area->lowobjvnum			= UMIN( area->lowobjvnum, vnum );
    area->highobjvnum			= UMAX( area->highobjvnum, vnum );
    if ( !area->last_obj )
	area->first_obj = area->last_obj = pObjIndex;
    else if ( pObjIndex->vnum > area->last_obj->vnum )
    {
	/* krainy sa sortowane vnumami - powyzszy i ten przypadek wyczerpia
	   100% wystapien w krainach Laca */
	area->last_obj->next_in_area = pObjIndex;
	area->last_obj = pObjIndex;
    }
    else if ( pObjIndex->vnum < area->first_obj->vnum )
    {
	pObjIndex->next_in_area = area->first_obj;
	area->first_obj = pObjIndex;
    }
    else
    {
	OBJ_INDEX_DATA *poprz;

	for ( poprz = area->first_obj; poprz->next_in_area; poprz = poprz->next_in_area )
	    if ( poprz->next_in_area->vnum > pObjIndex->vnum )
		break;

	if ( poprz->vnum == pObjIndex->vnum )
	{
	    cbug( "new_obj_index: powtorzony vnum przedmiotu %d.", vnum );
	    return NULL;
	}
	else if ( poprz->vnum > pObjIndex->vnum || !poprz->next_in_area )
	{
	    cbug( "new_obj_index: niemozliwy blad!", 0 );
	    return NULL;
	}
	else
	{
	    pObjIndex->next_in_area = poprz->next_in_area;
	    poprz->next_in_area = pObjIndex;
	}
    }

    pObjIndex->name			= str_dup( "" );
    pObjIndex->short_descr		= str_dup( "" );
    pObjIndex->dopelniacz		= str_dup( "" );
    pObjIndex->celownik			= str_dup( "" );
    pObjIndex->biernik			= str_dup( "" );
    pObjIndex->narzednik		= str_dup( "" );
    pObjIndex->miejscownik		= str_dup( "" );
    pObjIndex->description		= str_dup( "" );
    pObjIndex->look_descr		= str_dup( "" );
    pObjIndex->action			= str_dup( "" );
    pObjIndex->anti_class		= str_dup( "" );
    pObjIndex->anti_race		= str_dup( "" );
    pObjIndex->min_reset_level		= 999;

    iHash			= vnum % MAX_KEY_HASH;
    pObjIndex->next		= obj_index_hash[ iHash ];
    obj_index_hash[ iHash ]	= pObjIndex;
    top_obj_index++;
    area->numofobjs++;
    for ( lista = area->strefy; lista; lista = lista->next )
	lista->zone->ile_przedmiotow++;

    return pObjIndex;
}


/* Vigud 10.3.2010 */
MOB_INDEX_DATA *new_mob_index( int vnum, AREA_DATA *area )
{
    MOB_INDEX_DATA *pMobIndex;
    ZONE_LIST *lista;
    int iHash;

    pMobIndex				= alloc_perm( sizeof( *pMobIndex ) );
    pMobIndex->vnum			= vnum;
    pMobIndex->index_area		= area;
    area->lowmobvnum			= UMIN( area->lowmobvnum, vnum );
    area->highmobvnum			= UMAX( area->highmobvnum, vnum );
    if ( !area->last_mob )
	area->first_mob = area->last_mob = pMobIndex;
    else if ( pMobIndex->vnum > area->last_mob->vnum )
    {
	/* krainy sa sortowane vnumami - powyzszy i ten przypadek wyczerpia
	   100% wystapien w krainach Laca */
	area->last_mob->next_in_area = pMobIndex;
	area->last_mob = pMobIndex;
    }
    else if ( pMobIndex->vnum < area->first_mob->vnum )
    {
	pMobIndex->next_in_area = area->first_mob;
	area->first_mob = pMobIndex;
    }
    else
    {
	MOB_INDEX_DATA *poprz;

	for ( poprz = area->first_mob; poprz->next_in_area; poprz = poprz->next_in_area )
	    if ( poprz->next_in_area->vnum > pMobIndex->vnum )
		break;

	if ( poprz->vnum == pMobIndex->vnum )
	{
	    cbug( "new_mob_index: powtorzony vnum moba %d", vnum );
	    return NULL;
	}
	else if ( poprz->vnum > pMobIndex->vnum || !poprz->next_in_area )
	{
	    cbug( "new_mob_index: niemozliwy blad!", 0 );
	    return NULL;
	}
	else
	{
	    pMobIndex->next_in_area = poprz->next_in_area;
	    poprz->next_in_area = pMobIndex;
	}
    }

    pMobIndex->player_name		= str_dup( "" );
    pMobIndex->short_descr		= str_dup( "" );
    pMobIndex->long_descr		= str_dup( "" );
    pMobIndex->dopelniacz		= str_dup( "" );
    pMobIndex->celownik			= str_dup( "" );
    pMobIndex->biernik			= str_dup( "" );
    pMobIndex->narzednik		= str_dup( "" );
    pMobIndex->miejscownik		= str_dup( "" );
    pMobIndex->wolacz			= str_dup( "" );
    pMobIndex->description		= str_dup( "" );
    pMobIndex->act			= ACT_IS_NPC;

    iHash			= vnum % MAX_KEY_HASH;
    pMobIndex->next		= mob_index_hash[ iHash ];
    mob_index_hash[ iHash ]	= pMobIndex;
    top_mob_index++;
    area->numofmobs++;
    for ( lista = area->strefy; lista; lista = lista->next )
	lista->zone->ile_mobow++;
    kill_table[ URANGE( 0, pMobIndex->level, MAX_LEVEL - 1 ) ].number++;

    return pMobIndex;
}


/*
 * Lam 21.5.2000
 * Pomieszczenie wytworzone przez te funkcje musi byc w pelni stabilne, nadajace
 * sie do natychmiastowego dolaczenia do krainy i ewentualnego dorobienia nazwy,
 * opisow, wyjsc itp. Uzywane przez load_rooms, kod klanow tworzacy
 * pomieszczenia, a takze przez edytor krain.
 * Lancuchy musza byc puste (""), zeby nie trzeba bylo ich zwalniac.
 */
ROOM_INDEX_DATA *new_room( int vnum, AREA_DATA *area )
{
    ROOM_INDEX_DATA *pRoomIndex;
    ZONE_LIST       *lista;
    int iHash;

    pRoomIndex				= alloc_perm( sizeof( *pRoomIndex ) );
    pRoomIndex->area			= area;
    pRoomIndex->vnum			= vnum;
    area->lowroomvnum			= UMIN( area->lowroomvnum, vnum );
    area->highroomvnum			= UMAX( area->highroomvnum, vnum );
    if ( !area->last_room )
	area->first_room = area->last_room = pRoomIndex;
    else if ( pRoomIndex->vnum > area->last_room->vnum )
    {
	/* krainy sa sortowane vnumami - powyzszy i ten przypadek wyczerpia
	   100% wystapien w krainach Laca */
	area->last_room->next_in_area = pRoomIndex;
	area->last_room = pRoomIndex;
    }
    else if ( pRoomIndex->vnum < area->first_room->vnum )
    {
	pRoomIndex->next_in_area = area->first_room;
	area->first_room = pRoomIndex;
    }
    else
    {
	ROOM_INDEX_DATA *poprz;

	for ( poprz = area->first_room; poprz->next_in_area; poprz = poprz->next_in_area )
	    if ( poprz->next_in_area->vnum > pRoomIndex->vnum )
		break;

	if ( poprz->vnum == pRoomIndex->vnum )
	{
	    cbug( "New_room: powtorzony vnum pomieszczenia %d.", vnum );
	    return NULL;
	}
	else if ( poprz->vnum > pRoomIndex->vnum || !poprz->next_in_area )
	{
	    cbug( "New_room: niemozliwy blad!", 0 );
	    return NULL;
	}
	else
	{
	    pRoomIndex->next_in_area = poprz->next_in_area;
	    poprz->next_in_area = pRoomIndex;
	}
    }

    pRoomIndex->name			= str_dup( "" );
    pRoomIndex->miejscownik		= str_dup( "" );
    pRoomIndex->description		= str_dup( "" );
    pRoomIndex->anti_race		= str_dup( "" );
    pRoomIndex->anti_class		= str_dup( "" );

    iHash			= vnum % MAX_KEY_HASH;
    pRoomIndex->next		= room_index_hash[ iHash ];
    room_index_hash[ iHash ]	= pRoomIndex;
    top_room++;
    pRoomIndex->area->numofrooms++; /* Lam */
    for ( lista = pRoomIndex->area->strefy; lista; lista = lista->next )
	lista->zone->ile_pomieszczen++;

    return pRoomIndex;
}


/*
 * Nie zwalnia niczego, nie wrzuca na liste odzyskowa. To wyciek pamieci.
 * Strata to kilkadziesiat bajtow tylko podczas uruchamiania edytora krain.
 */
void del_room( ROOM_INDEX_DATA *pRoomIndex )
{
    ZONE_LIST       *lista;
    ROOM_INDEX_DATA *pp;
/*    int door; */
    int iHash;

/* FIXME: zwalnianie wyjsc
    for ( door = 0; door <= 9; door++ )
	pRoomIndex->exit[ door ]		= NULL;
*/

    top_room--;
    pRoomIndex->area->numofrooms--;
    for ( lista = pRoomIndex->area->strefy; lista; lista = lista->next )
	lista->zone->ile_pomieszczen--;

    iHash			= pRoomIndex->vnum % MAX_KEY_HASH;
    if ( room_index_hash[ iHash ] == pRoomIndex )
    {
	room_index_hash[ iHash ] = room_index_hash[ iHash ]->next;
    }
    else
	for ( pp = room_index_hash[ iHash ]; pp && pp->next; pp = pp->next )
	    if ( pp->next == pRoomIndex )
	    {
		pp->next = pRoomIndex->next;
		break;
	    }

    return;
}


EXIT_DATA *new_exit( )
{
    EXIT_DATA *ex;

    if ( !exit_free )
    {
	top_exit++;
	ex = alloc_perm( sizeof( EXIT_DATA ) );
	ex->vnum = 0;
	ex->to_room = NULL;
	ex->exit_info = 0;
	ex->key = -1;
	ex->rodzaj = 3;
	ex->keyword = str_dup( "" );
	ex->short_descr = str_dup( "" );
	ex->biernik = str_dup( "" );
	ex->description = str_dup( "" );
	ex->mobprogs = NULL;
	ex->progtypes = 0;
	return ex;
    }

    ex = exit_free;
    exit_free = exit_free->next;
    ex->next = NULL;

    return ex;
}


void del_exit( EXIT_DATA *ex )
{
    ex->vnum = 0;
    ex->to_room = NULL;
    ex->exit_info = 0;
    ex->key = -1;
    ex->rodzaj = 3;
    free_string( ex->keyword );
    ex->keyword = str_dup( "" );
    free_string( ex->short_descr );
    ex->short_descr = str_dup( "" );
    free_string( ex->biernik );
    ex->biernik = str_dup( "" );
    free_string( ex->description );
    ex->description = str_dup( "" );
    ex->mobprogs = NULL;
    ex->progtypes = 0;
    ex->next = exit_free;
    exit_free = ex;

    return;
}


RESET_DATA *new_reset( )
{
    RESET_DATA *res;

    if ( !reset_free )
    {
	top_reset++;
	res = alloc_perm( sizeof( RESET_DATA ) );
	res->string = str_dup( "" );
	return res;
    }

    res = reset_free;
    reset_free = reset_free->next;
    res->next = NULL;

    return res;
}


void del_reset( RESET_DATA *reset )
{
    reset->command = 0;
    reset->arg1 = 0;
    reset->arg2 = 0;
    reset->arg3 = 0;
    free_string( reset->string );
    reset->string = str_dup( "" );
    reset->next = reset_free;
    reset_free = reset;

    return;
}


/*
 * Lam 21.5.2000: teraz uzywa new_room( ) do robienia nowych pomieszczen
 */
bool load_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *pRoomIndex;

    if ( !area_last )
    {
	cbug( "Load_rooms: no #AREA seen yet.", 0 );
	return TRUE;
    }

    for ( ; ; )
    {
	char letter;
	int  vnum;
	int  door;
	int  stat;
	EXIT_DATA *last_exit = NULL; /* Lam do eprogow */

	letter = fread_letter( fp );
	if ( letter != '#' )
	{
	    cbug( "Load_rooms: # not found.", 0 );
	    return TRUE;
	}

	vnum = fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	if ( vnum < 0 || vnum >= MAX_VNUM )
	{
	    cbug( "Load_rooms: vnum %d poza zakresem.", vnum );
	    return TRUE;
	}

	pRoomIndex = new_room( vnum, area_last );
	if ( !pRoomIndex )
	    return TRUE;

	/* Lam 3.2.2006 */
	if ( vnum == ROOM_VNUM_PURGATORY_A )
	    area_czysciec = area_last;

	pRoomIndex->name		= fread_string( fp, &stat );
	if ( wersje_krain[ area_last->version ].pommiej )
	{
	    pRoomIndex->miejscownik	= fread_string( fp, &stat );
	    if ( ODPLUSKWIACZ && !*pRoomIndex->miejscownik )
		cbug( "Pomieszczenie %d bez miejscownika.", vnum );
	}
	pRoomIndex->description		= fread_string( fp, &stat );
	if ( wersje_krain[ area_last->version ].stare_flagi )
	    /* Area number */		  fread_number( fp, &stat );
	else
	{
	    pRoomIndex->pojemnosc	= fread_number( fp, &stat );
	    pRoomIndex->strata_ruchu	= fread_number( fp, &stat );
	    if ( wersje_krain[ area_last->version ].oswietlenie )
		/* pRoomIndex->light */	  fread_number( fp, &stat );
	}
	pRoomIndex->original_room_flags = fread_number( fp, &stat );
	pRoomIndex->room_flags		= pRoomIndex->original_room_flags;
	pRoomIndex->sector_type		= fread_number( fp, &stat );
	if ( pRoomIndex->sector_type < 0
	  || pRoomIndex->sector_type >= SECT_MAX )
	{
	    sprintf( bug_buf, "Load_rooms: room %d bad sector type %d.", vnum, pRoomIndex->sector_type );
	    cbug( bug_buf, 0 );
	    pRoomIndex->sector_type = SECT_UNDERWATER;
	    /* SECT_UNDERWATER, aby spowodowac wyswietlenie bledow po
	       ewentualnym przekonwertowaniu przez lac -k, co byc moze wymusi na
	       autorze poprawienie krainy recznie zamiast zostawic np.
	       SECT_FOREST w srodku zamku */
	}
	if ( ODPLUSKWIACZ
	  && pRoomIndex->sector_type == SECT_INSIDE
	  && !IS_SET( pRoomIndex->room_flags, ROOM_INDOORS ) )
	{
	    cbug( "Pomieszczenie %d: pomieszczenie, ale nie wnetrze.", vnum );
	    SET_BIT( pRoomIndex->original_room_flags, ROOM_INDOORS );
	}
	/* teraz to poprawna sytuacja i oznacza cien, ale mozna uzyc jako
	   wykrywacz takich pomieszczen, byc moze blednie oznaczonych przez
	   autora krainy
	if ( ODPLUSKWIACZ
	  && pRoomIndex->sector_type != SECT_UNDERWATER
	  && !IS_SET( pRoomIndex->room_flags, ROOM_INDOORS )
	  && IS_SET( pRoomIndex->room_flags, ROOM_DARK ) )
	    cbug( "Pomieszczenie %d: widac slonce, ale ktos probuje je nieudolnie zaciemniac za pomoca ROOM_DARK (b00)", vnum );
	*/
	if ( wersje_krain[ area_last->version ].stare_flagi )
	{
	    pRoomIndex->pojemnosc	=
		  IS_SET( pRoomIndex->room_flags, b11 /* ROOM_SOLITARY */ ) ? 1
		: IS_SET( pRoomIndex->room_flags, b09 /* ROOM_PRIVATE */ ) ? 2
		: 0;
	    REMOVE_BIT( pRoomIndex->original_room_flags, b09 | b11 );
	    REMOVE_BIT( pRoomIndex->room_flags, b09 | b11 );
	    pRoomIndex->anti_race	= str_dup( "" );
	    pRoomIndex->anti_class	= str_dup( "" );
	}
	else
	{
	    pRoomIndex->anti_race	= fread_string( fp, &stat );
	    pRoomIndex->anti_class	= fread_string( fp, &stat );
	}

	/* Lam 28.9.2005, przedmioty mialy, a pomieszczenia nie */
	if ( ODPLUSKWIACZ )
	{
	    if ( IS_SET( pRoomIndex->room_flags, ROOM_INVERT_RACES )
	      && !*pRoomIndex->anti_race )
		cbug( "Do pomieszczenia %d nie wejdzie zadna rasa.", pRoomIndex->vnum );

	    if ( IS_SET( pRoomIndex->room_flags, ROOM_INVERT_CLASSES )
	      && !*pRoomIndex->anti_class )
		cbug( "Do pomieszczenia %d nie wejdzie zadna profesja.", pRoomIndex->vnum );

	    if ( *pRoomIndex->anti_race )
	    {
		int rasa;
		char *lista;
		char slowo[ MIL ];

		lista = pRoomIndex->anti_race;
		for ( lista = one_argument( lista, slowo ); *slowo; lista = one_argument( lista, slowo ) )
		{
		    for ( rasa = 0; rasa < MAX_RACE; rasa++ )
			if ( !str_cmp( race_table[ rasa ].ang, slowo ) )
			    break;

		    if ( rasa == MAX_RACE )
		    {
			sprintf( bug_buf, "Pomieszczenie %d (nie) wpuszcza nieznanej rasy \"%s\".",
				pRoomIndex->vnum, slowo );
			cbug( bug_buf, 0 );
		    }
		}
	    }

	    if ( *pRoomIndex->anti_class )
	    {
		int prof;
		char *lista;
		char slowo[ MIL ];

		lista = pRoomIndex->anti_class;
		for ( lista = one_argument( lista, slowo ); *slowo; lista = one_argument( lista, slowo ) )
		{
		    for ( prof = 0; prof < MAX_CLASS; prof++ )
			if ( !str_cmp( class_table[ prof ].who_name, slowo ) )
			    break;

		    if ( prof == MAX_CLASS )
		    {
			sprintf( bug_buf, "Pomieszczenie %d (nie) wpuszcza nieznanej profesji \"%s\".",
				pRoomIndex->vnum, slowo );
			cbug( bug_buf, 0 );
		    }
		}
	    }
	}

	for ( ; ; )
	{
	    letter = fread_letter( fp );

	    if ( letter == 'S' || letter == 's' )
	    {
		if ( letter == 's' )
		    cbug( "Load_rooms: vnum %d has lowercase 's'", vnum );
		break;
	    }

	    if ( letter == '*' )
		fread_to_eol( fp );
		/* komentarze w pomieszczeniach */
	    else if ( letter == 'D' )
	    {
		EXIT_DATA *pexit;
		int        locks;
		char       bufor[ MIL ];

		door = drzwi_d( fread_word( fp, &stat, TRUE ) ); /* Lam */
		if ( door < 0 )
		    cbug( "Fread_rooms: vnum %d: invalid door number.", vnum );

		pexit			= new_exit( );
		pexit->description	= fread_string( fp, &stat );

		if ( wersje_krain[ area_last->version ].entery )
		    zjedz_entery_z_konca( &pexit->description );

		pexit->keyword		= fread_string( fp, &stat );

		if ( wersje_krain[ area_last->version ].plusy )
		{
		    one_argument( pexit->keyword, bufor );
		    pexit->short_descr	= str_dup( bufor );
		    pexit->biernik	= str_dup( bufor );
		    pexit->rodzaj	= 3;
		    if ( *pexit->keyword == '+' )
		    {
			free_string( pexit->keyword );
			free_string( pexit->short_descr );
			free_string( pexit->biernik );
			pexit->keyword		= fread_string( fp, &stat );
			pexit->short_descr	= fread_string( fp, &stat );
			pexit->biernik		= fread_string( fp, &stat );
			pexit->rodzaj		= fread_number( fp, &stat );
		    }
		}
		else
		{
		    pexit->short_descr	= fread_string( fp, &stat );
		    pexit->biernik	= fread_string( fp, &stat );
		    pexit->rodzaj	= fread_number( fp, &stat );
		}
		pexit->exit_info	= 0;
		locks			= fread_number( fp, &stat );
		pexit->key		= fread_number( fp, &stat );
		pexit->vnum		= fread_number( fp, &stat );
		if ( pexit->vnum == ROOM_VNUM_POZA_SWIATEM )
		{
		    cbug( "Wyjscie z %d do poza swiatem!", pRoomIndex->vnum );
		}

		/*
		Lam 7.4.98: dla porownania, wersja oryginalna:
		switch ( locks )
		{
		case 1: pexit->exit_info = EX_ISDOOR;                    break;
		case 2: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF;     break;
		case 3: pexit->exit_info = EX_ISDOOR | EX_BASHPROOF;     break;
		case 4: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
					 | EX_BASHPROOF;                 break;
		case 5: pexit->exit_info = EX_ISDOOR | EX_PASSPROOF;     break;
		case 6: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
					 | EX_PASSPROOF;                 break;
		case 7: pexit->exit_info = EX_ISDOOR | EX_BASHPROOF
					 | EX_PASSPROOF;                 break;
		case 8: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
					 | EX_BASHPROOF | EX_PASSPROOF;  break;
		}
		Moja wersja dziala identycznie (bez zmian krain), jest
		ladniejsza i latwiejsza w rozbudowie, choc wolniejsza.
		*/

		if ( locks )
		{
		    pexit->exit_info = EX_ISDOOR;
		    locks--; /* od teraz mamy normalne bity */
		    if ( IS_SET( locks, 1 ) )
			SET_BIT( pexit->exit_info, EX_PICKPROOF );
		    if ( IS_SET( locks, 2 ) )
			SET_BIT( pexit->exit_info, EX_BASHPROOF );
		    if ( IS_SET( locks, 4 ) )
			SET_BIT( pexit->exit_info, EX_PASSPROOF );
		    if ( IS_SET( locks, 8 ) )
		    {
			SET_BIT( pexit->exit_info, EX_DIG );
			if ( ODPLUSKWIACZ )
			{
			    if ( door != DIR_DOWN )
				cbug( "Pomieszczenie %d ma odkopywalne wyjscie, ale nie w dol.", pRoomIndex->vnum );
			    if ( !czy_mozna_kopac( pRoomIndex->sector_type, NULL ) )
				cbug( "Pomieszczenie %d ma odkopywalne wyjscie, ale zle podloze do kopania.", pRoomIndex->vnum );
			}
		    }
		    if ( IS_SET( locks, 16 ) )
			SET_BIT( pexit->exit_info, EX_DONTRESET );
		    if ( IS_SET( locks, 32 ) )
			SET_BIT( pexit->exit_info, EX_BEZKLAMKI );
		    if ( IS_SET( locks, 64 ) )
			SET_BIT( pexit->exit_info, EX_BEZZAMKA );
		}

		if ( pRoomIndex->exit[ door ] )
		    cbug( "Pomieszczenie %d: dwa wyjscia w jednym kierunku", pRoomIndex->vnum );
		pRoomIndex->exit[ door ] = pexit;
		last_exit = pexit;
	    }
	    /* Lam 21.5.99: dopisywanie na koniec listy */
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;
		EXTRA_DESCR_DATA *ed2;

		ed                      = new_extra_descr( );
		ed->keyword             = fread_string( fp, &stat );
		ed->description         = fread_string( fp, &stat );
		ed->next                = NULL;
		if ( !pRoomIndex->extra_descr )
		    pRoomIndex->extra_descr = ed;
		else
		{
		    for ( ed2 = pRoomIndex->extra_descr; ed2->next; ed2 = ed2->next )
			; /* tylko szukanie */
		    ed2->next = ed;
		}
	    }
	    else if ( letter == 'T' )
	    {
		DEATHTRAP_DATA *trap;

		trap			= alloc_mem( sizeof( *trap ) );
		trap->trap_type		= fread_number( fp, &stat );
		trap->trap_string	= fread_string( fp, &stat );
		top_trap++;
		if ( pRoomIndex->trap )
		    cbug( "load_rooms (#%d): pomieszczenie z dwoma pulapkami!", pRoomIndex->vnum );
		pRoomIndex->trap = trap;
	    }
	    else if ( letter == '>' ) /* MOBProg */
	    {
		/* Uwaga: jesli prog znajduje sie w krainie przed wyjsciami,
		   jest rprogiem. Jesli jest za jakims wyjsciem, jest eprogiem
		   tego wyjscia. */
		ungetc( letter, fp );
		if ( last_exit )
		{
		    if ( last_exit->mobprogs )
			cbug( "Wyjscie z pomieszczenia %d: wiecej niz jedna \"sekcja\" progow (byc moze to tylko zbedna \"|\").", pRoomIndex->vnum );
		    if ( mprog_read_programs( fp, last_exit, 2 ) )
			return TRUE;
		}
		else
		{
		    if ( pRoomIndex->mobprogs )
			cbug( "Pomieszczenie %d: wiecej niz jedna \"sekcja\" progow (byc moze to tylko zbedna \"|\").", pRoomIndex->vnum );
		    if ( mprog_read_programs( fp, pRoomIndex, 1 ) )
			return TRUE;
		}
	    }
	    else
	    {
		cbug( "Load_rooms: vnum %d has flag not 'DEST>*'.", vnum );
		return TRUE;
	    }
	}
	if ( ODPLUSKWIACZ && !*pRoomIndex->description
	  && !IS_SET( pRoomIndex->progtypes, R_DESCR_PROG ) )
	    cbug( "Pomieszczenie %d: brak opisu lub descr_proga!", pRoomIndex->vnum );
    }

    return FALSE;
}


bool load_healers( FILE *fp )
{
    HEALER_DATA *pHealer;
    int        keeper = 0;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int stat;
	int spnum;
	char *slowo;

	keeper                  = fread_number( fp, &stat );
	if ( keeper == 0 )
	    break;

	pHealer                 = new_healer( );
	pHealer->keeper         = keeper;
	for ( spnum = 0; spnum <= 4; spnum++ )
	{
	    slowo = fread_word( fp, &stat,TRUE );
	    if ( !*slowo || !str_cmp( slowo, "xxx" ) )
		pHealer->spells[ spnum ] = -1;
	    else
	    {
		pHealer->spells[ spnum ] = skill_lookup_ang( slowo );
		if ( pHealer->spells[ spnum ] <= 0 )
		{
		    sprintf( bug_buf, "Sprzedawca czarow %d: niepoprawny czar %d: %s", pHealer->keeper, spnum, slowo );
		    cbug( bug_buf, 0 );
		}
	    }
	    pHealer->cost[ spnum ] = fread_number( fp, &stat );
	}

	fread_to_eol( fp );

	pMobIndex               = get_mob_index( pHealer->keeper );
	pMobIndex->pHealer	= pHealer;
    }

    return FALSE;
}


/*
 * Snarf a shop section.
 */
bool load_shops( FILE *fp )
{
    SHOP_DATA *pShop;
    int        keeper = 0;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int             iTrade;
	int             stat;

	keeper                  = fread_number( fp, &stat );
	if ( keeper == 0 )
	    break;
	pShop                   = new_shop( );
	pShop->keeper           = keeper;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    pShop->buy_type[ iTrade ] = fread_number( fp, &stat );
	pShop->profit_buy       = fread_number( fp, &stat );
	pShop->profit_sell      = fread_number( fp, &stat );
	pShop->open_hour        = fread_number( fp, &stat );
	pShop->close_hour       = fread_number( fp, &stat );
				  fread_to_eol( fp );
	pShop->quest_shop	= FALSE;
	if ( pShop->profit_buy < 130 || pShop->profit_sell > 70 )
	{
	    char bugbuf[ MSL ];
	    sprintf( bugbuf, "Sklep u moba #%d, zakup %d, sprzedaz %d!",
		keeper, pShop->profit_buy, pShop->profit_sell );
	    cbug( bugbuf, 0 );
	}
	pMobIndex               = get_mob_index( pShop->keeper );
	pMobIndex->pShop        = pShop;
    }

    return FALSE;
}


/*
 * Snarf spec proc declarations.
 */
bool load_specials( FILE *fp )
{
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	char            letter;
	int             stat;

	switch ( letter = fread_letter( fp ) )
	{
	default:
	    cbug( "Load_specials: letter '%c' not *MS.", letter );
	    return TRUE;

	case 'S':
	    return FALSE;

	case '*':
	    break;

	case 'M':
	    pMobIndex = get_mob_index( fread_number( fp, &stat ) );
	    pMobIndex->spec_fun = spec_lookup( fread_word( fp, &stat, TRUE ) );
	    if ( pMobIndex->spec_fun == 0 )
	    {
		cbug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
		return TRUE;
	    }
	    break;
	}

	fread_to_eol( fp );
    }

    cbug( "load_specials() wyskoczylo z for().", 0 );

    return TRUE;
}


/*
 * Lam 8.3.98: czytanie notek (usuwanie notek napisalem w styczniu)
 * Lam 3.6.99: zamiast usuwac noty calkowicie, przenosze je do innego pliku,
 * gdzie zawsze sa pod reka (wczesniej wszelkie pomysly roznych graczy znikaly
 * po dwoch tygodniach, a nikt nie robil ich kopii)
 */
void load_notes( void )
{
    FILE	*fp;
    NOTE_DATA	*pnotelast;
    int		 count = 0; /* ilosc notek do usuniecia */
    char	*key;

    if ( !( fp = fopen( NOTE_FILE, "r" ) ) )
	return;

    pnotelast = NULL;
    for ( ; ; )
    {
	NOTE_DATA *pnote;
	char       letter;
	int        stat = 0;
	bool       blond = 0;

	do
	{
	    letter = getc( fp );
	    /* chyba czytelne, mimo ze koniec funkcji jest na poczatku */
	    if ( feof( fp ) )
	    {
		fclose( fp );
		/* automatyczne usuwanie dwutygodniowych notek -Lam */
		if ( count > 0 )
		{
		    while ( note_list
		       && ( note_list->date_stamp < current_time
			  - ( DLUGOSC_ZYCIA_NOTY * 24 * 60 * 60 ) ) )
		    {
			pnote = note_list;
			/* stara notka zostaje przeniesiona do pliku notes.old */
			if ( !( fp = fopen( OLD_NOTE_FILE, "a" ) ) )
			{
			    lac_perror( OLD_NOTE_FILE );
			}
			else
			{
			    fprintf( fp, "Sender  %s~\n", pnote->sender     );
			    fprintf( fp, "Date    %s~\n", pnote->date       );
			    fprintf( fp, "Stamp   %ld\n", (unsigned long)pnote->date_stamp );
			    fprintf( fp, "To      %s~\n", pnote->to_list    );
			    fprintf( fp, "Subject %s~\n", pnote->subject    );
			    if ( pnote->vote != VOTE_NO )
			    {
				fprintf( fp, "Vote    %d\n",  pnote->vote );
				fprintf( fp, "Votes   %d %d %d\n", pnote->yesnum, pnote->nonum, pnote->abstnum );
				fprintf( fp, "Yes     %s~\n", pnote->yes );
				fprintf( fp, "No      %s~\n", pnote->no );
				fprintf( fp, "Abstain %s~\n", pnote->abst );
			    }
			    fprintf( fp, "Text\n%s~\n\n", pnote->text       );
			    fclose( fp );
			}
			/* pamiec po nocie jeszcze sie przyda */
			note_list = pnote->next;
			free_string( pnote->text );
			free_string( pnote->subject );
			free_string( pnote->to_list );
			free_string( pnote->date );
			free_string( pnote->sender );
			pnote->next = note_free;
			note_free = pnote;
		    }
		    /* nagranie notek, bez usunietych */
		    rewrite_all_notes( );
		}
		return;
	    }
	}
	while ( isspace( (int) letter ) );
	ungetc( letter, fp );

	pnote		= alloc_perm( sizeof( *pnote ) );
	/* te rzeczy musza byc ustawiane, a reszta musi byc czytana */
	pnote->yes	= str_dup( "" );
	pnote->no	= str_dup( "" );
	pnote->abst	= str_dup( "" );
	pnote->yesnum = pnote->nonum = pnote->abstnum = 0;
	pnote->vote	= VOTE_NO;

    /* ta petla w lepszy sposob czyta notke niz Envy, lecz nie sprawdza
       poprawnosci tak jak tamta wersja */
    for ( ; ; )
    {
	key = fread_word( fp, &stat, TRUE );
	if ( !key || !*key || stat )
	{
	    blond = 1;
	    break;
	}

	if ( !str_cmp( key, "sender" ) )
	    pnote->sender	= fread_string( fp, &stat );
	if ( !str_cmp( key, "date" ) )
	    pnote->date		= fread_string( fp, &stat );
	if ( !str_cmp( key, "stamp" ) )
	{
	    pnote->date_stamp	= fread_number( fp, &stat );
	    if ( pnote->date_stamp < current_time
				- ( DLUGOSC_ZYCIA_NOTY * 24 * 60 * 60 ) )
		count++;
	}
	if ( !str_cmp( key, "to" ) )
	    pnote->to_list	= fread_string( fp, &stat );
	if ( !str_cmp( key, "subject" ) )
	    pnote->subject	= fread_string( fp, &stat );
	if ( !str_cmp( key, "text" ) )
	{
	    pnote->text		= fread_string( fp, &stat );
	    break; /* "text" to koniec noty */
	}
	if ( !str_cmp( key, "yes" ) )
	    pnote->yes		= fread_string( fp, &stat );
	if ( !str_cmp( key, "no" ) )
	    pnote->no		= fread_string( fp, &stat );
	if ( !str_cmp( key, "abstain" ) )
	    pnote->abst		= fread_string( fp, &stat );
	if ( !str_cmp( key, "vote" ) )
	    pnote->vote		= fread_number( fp, &stat );
	if ( !str_cmp( key, "votes" ) )
	{
	    pnote->yesnum	= fread_number( fp, &stat );
	    pnote->nonum	= fread_number( fp, &stat );
	    pnote->abstnum	= fread_number( fp, &stat );
	}
    } /* koniec notki */

	if ( blond )
	    break;

	if ( !note_list )
	    note_list           = pnote;
	else
	    pnotelast->next     = pnote;

	pnotelast               = pnote;
    }

    strcpy( strArea, NOTE_FILE );
    fpArea = fp;
    cbug( "Load_notes: bad key word.", 0 );
    exit( 1 );

    return;
}


/*
 * Lam 9.2.99
 */
void load_hints( void )
{
    FILE *plik;
    int hint = 0;
    int zwrot = 0;
    int znak;

    if ( !( plik = fopen( HINT_FILE, "r" ) ) )
    {
	lac_perror( HINT_FILE );
	return;
    }

    if ( feof( plik ) )
    {
	cbug( "HINT_FILE pusty!", 0 );
	fclose( plik );
	return;
    }

    for ( ; !feof( plik ); )
    {
	hint_table[ hint ] = fread_string( plik, &zwrot );
	if ( zwrot )
	{
	    cbug( "blad przy czytaniu z HINT_FILE", 0 );
	    fclose( plik );
	    return;
	}

	if ( ++hint >= MAX_HINT )
	{
	    cbug( "Za duzo podpowiedzi. Zwieksz MAX_HINT.", 0 );
	    fclose( plik );
	    return;
	}

	if ( !feof( plik ) )
	{
	    do
		znak = getc( plik );
	    while ( isspace( znak ) && znak != EOF );
	    if ( znak != EOF )
		ungetc( znak, plik );
	}
    }

    fclose( plik );

    return;
}


/*
 * Lam 26.3.2000
 */
void load_quits( void )
{
    FILE *plik;
    int quit = 0;
    int zwrot = 0;
    int znak;

    if ( !( plik = fopen( QUIT_FILE, "r" ) ) )
    {
	lac_perror( QUIT_FILE );
	return;
    }

    if ( feof( plik ) )
    {
	cbug( "QUIT_FILE pusty!", 0 );
	fclose( plik );
	return;
    }

    for ( ; !feof( plik ); )
    {
	quit_table[ quit ] = fread_string( plik, &zwrot );
	quit_count = quit;
	if ( zwrot )
	{
	    cbug( "blad przy czytaniu z QUIT_FILE", 0 );
	    fclose( plik );
	    return;
	}

	if ( ++quit >= MAX_QUIT )
	{
	    cbug( "Za duzo podpowiedzi. Zwieksz MAX_QUIT.", 0 );
	    fclose( plik );
	    return;
	}

	if ( !feof( plik ) )
	{
	    do
		znak = getc( plik );
	    while ( isspace( znak ) && znak != EOF );
	    if ( znak != EOF )
		ungetc( znak, plik );
	}
    }

    fclose( plik );

    return;
}


/*
 * Lam 13.12.2002
 */
void load_offensive( void )
{
    FILE *plik;
    int offensive = 0;
    int zwrot = 0;
    int znak;

    if ( !( plik = fopen( OFFENSIVE_FILE, "r" ) ) )
    {
	lac_perror( OFFENSIVE_FILE );
	return;
    }

    if ( feof( plik ) )
    {
	cbug( "OFFENSIVE_FILE pusty!", 0 );
	fclose( plik );
	return;
    }

    for ( ; !feof( plik ); )
    {
	offensive_table[ offensive ] = str_dup( fread_word( plik, &zwrot, FALSE ) );
	if ( zwrot )
	{
	    cbug( "blad przy czytaniu z OFFENSIVE_FILE", 0 );
	    fclose( plik );
	    return;
	}

	if ( ++offensive >= MAX_OFFENSIVE )
	{
	    cbug( "Za duzo wulgaryzmow. Zwieksz MAX_OFFENSIVE.", 0 );
	    fclose( plik );
	    return;
	}

	if ( !feof( plik ) )
	{
	    do
		znak = getc( plik );
	    while ( isspace( znak ) && znak != EOF );
	    if ( znak != EOF )
		ungetc( znak, plik );
	}
    }

    fclose( plik );

    return;
}


/*
 * Lam 14.12.2002
 */
void load_miodek( void )
{
    FILE *plik;
    int miodek = 0;
    int zwrot = 0;
    int znak;

    if ( !( plik = fopen( MIODEK_FILE, "r" ) ) )
    {
	lac_perror( MIODEK_FILE );
	return;
    }

    if ( feof( plik ) )
    {
	cbug( "MIODEK_FILE pusty!", 0 );
	fclose( plik );
	return;
    }

    for ( ; !feof( plik ); )
    {
	miodek_table[ miodek ].slowo = str_dup( fread_word( plik, &zwrot, FALSE ) );
	if ( zwrot )
	{
	    cbug( "blad przy czytaniu z MIODEK_FILE", 0 );
	    fclose( plik );
	    return;
	}

	miodek_table[ miodek ].zastepstwo = fread_string( plik, &zwrot );
	if ( zwrot )
	{
	    cbug( "blad przy czytaniu z MIODEK_FILE", 0 );
	    fclose( plik );
	    return;
	}

	if ( ++miodek >= ( MAX_MIODEK - 1 ) ) /* musi zostac jedno puste miejsce */
	{
	    cbug( "Za duzo slow Miodka. Zwieksz MAX_MIODEK.", 0 );
	    fclose( plik );
	    return;
	}

	if ( !feof( plik ) )
	{
	    do
		znak = getc( plik );
	    while ( isspace( znak ) && znak != EOF );
	    if ( znak != EOF )
		ungetc( znak, plik );
	}
    }

    fclose( plik );

    return;
}


/* Lam */
void load_powody( void )
{
    FILE *plik;
    char znak;
    bool cokolwiek = FALSE;
    int nr, stat;

    if ( !( plik = fopen( POWODY_FILE, "r" ) ) )
    {
	lac_perror( POWODY_FILE );
	cbug( "Potrzebuje pliku z powodami blokad imion.", 0 );
	exit( 1 );
    }

    for ( ; !feof( plik ); )
    {
	znak = fread_letter( plik );

	if ( znak == '*' )
	{
	    fread_to_eol( plik );
	    continue;
	}

	if ( znak != '#' )
	{
	    cbug( "Brak '#' w pliku " POWODY_FILE ".", 0 );
	    exit( 1 );
	}

	nr = fread_number( plik, &stat );
	if ( nr < 0 )
	    break;

	if ( nr >= MAX_POWODY )
	{
	    cbug( "Za wysoki numer powodu w " POWODY_FILE ".", nr );
	    exit( 1 );
	}

	if ( powody[ nr ].skrot )
	{
	    cbug( "Dwa razy podany powod #%d w " POWODY_FILE ".", nr );
	    exit( 1 );
	}

	cokolwiek = TRUE;
	powody[ nr ].skrot = fread_string( plik, &stat );
	powody[ nr ].nazwa = fread_string( plik, &stat );
	powody[ nr ].tytul = fread_string( plik, &stat );
	powody[ nr ].objasnienie = fread_string( plik, &stat );
	/* EOF w ktorymkolwiek fread_string zostanie wykryty tez w ostatnim */
	if ( stat )
	{
	    cbug( "Blad przy wczytywaniu powodu #%d z " POWODY_FILE ".", nr );
	    exit( 1 );
	}
    }

    fclose( plik );

    if ( !cokolwiek )
    {
	cbug( "Nie wczytalem niczego z pliku " POWODY_FILE ".", 0 );
	exit( 1 );
    }

    if ( !powody[ 0 ].skrot )
    {
	cbug( "Potrzebuje powodu nr 0 w " POWODY_FILE ".", 0 );
	exit( 1 );
    }

    return;
}


IMIONA_DATA *nowe_imie( void )
{
    IMIONA_DATA *im;

    if ( !imiona_wolne )
    {
	top_imiona++;
	im = calloc( 1, sizeof( IMIONA_DATA ) );
    }
    else
    {
	im = imiona_wolne;
	imiona_wolne = im->nast;
    }

    return im;
}


void zwolnij_imie( IMIONA_DATA *im )
{
    IMIONA_DATA **lista;
    IMIONA_DATA **ost;
    IMIONA_DATA *pop;

    lista = LISTA_IMION( im->imie );

    if ( *lista == im )
	*lista = im->nast;
    else
    {
	for ( pop = *lista; pop; pop = pop->nast )
	    if ( pop->nast == im )
		break;

	if ( !pop )
	{
	    bug( "spsuta lista", 0 );
	    return;
	}

	pop->nast = im->nast;
    }

    /* To robie juz po zmianie listy. */
    ost = OST_IMIE( im->imie );

    if ( *ost == im )
    {
	if ( *lista )
	{
	    for ( pop = *lista; pop; pop = pop->nast )
		if ( pop->nast == NULL )
		    break;

	    if ( !pop )
	    {
		bug( "spsuta lista", 0 );
		*ost = NULL;
		return;
	    }

	    *ost = pop;
	}
	else
	    *ost = NULL;
    }

    im->nast = imiona_wolne;
    imiona_wolne = im;
    free_string( im->imie );
    free_string( im->blokujacy );

    return;
}


void dodaj_imie( IMIONA_DATA *im )
{
    IMIONA_DATA **lista;
    IMIONA_DATA **ost;

    if ( !*( ost = OST_IMIE( im->imie ) ) )
    {
	lista = LISTA_IMION( im->imie );
	im->nast = *lista; /* powinno byc NULL */
	*lista = im;
	*ost = im;
    }
    else
    {
	( *ost )->nast = im;
	*ost = im;
	im->nast = NULL;
    }

    return;
}


void load_imiona( void )
{
    IMIONA_DATA *im;
    FILE *plik;
    int stat;
    int znak;

    if ( !( plik = fopen( IMIONA_FILE, "r" ) ) )
	return;

    for ( ; !feof( plik ); )
    {
	znak = fread_letter( plik );

	if ( znak == '*' )
	{
	    fread_to_eol( plik );
	    continue;
	}
	else
	    ungetc( znak, plik );

	im = nowe_imie( );
	im->imie = fread_string( plik, &stat );
	im->powod = fread_number( plik, &stat );
	im->blokujacy = fread_string( plik, &stat );
	im->kiedy = fread_number( plik, &stat );

	if ( strlen_pl( im->imie ) >= 3
	  && strlen_pl( im->imie ) <= MAX_DLUG_IMIENIA )
	{
	    if ( im->powod < 0
	      || im->powod > MAX_POWODY
	      || !*powody[ im->powod ].skrot )
	    {
		cbug( "Bledny numer powodu!", 0 );
		im->powod = 0;
	    }

	    dodaj_imie( im );
	}
	else
	{
	    sprintf( bug_buf, "Imie %s nie moze byc imieniem, ignoruje w " IMIONA_FILE ".",
		im->imie );
	    cbug( bug_buf, 0 );
	}

	if ( !feof( plik ) )
	{
	    do
		znak = getc( plik );
	    while ( isspace( znak ) && znak != EOF );
	    if ( znak != EOF )
		ungetc( znak, plik );
	}
    }

    return;
}


void zapisz_imiona( void )
{
    FILE *plik;
    int i, j, wyn;
    IMIONA_DATA *im;

    zamknij_reserve( fpReserve );
    if ( !( plik = fopen( IMNOWE_FILE, "w" ) ) )
	lac_perror( IMNOWE_FILE );
    else
    {
	fprintf( plik, "* Plik jest zapisywany przez Laca - reczne zmiany nie maja sensu.\n\n" );
	wyn = 0;
	for ( i = 0; i <= MAX_DLUG_IMIENIA && wyn >= 0; i++ )
	    for ( j = 0; j < 64 && wyn >= 0; j++ )
		for ( im = imiona[ i ][ j ]; im && wyn >= 0; im = im->nast )
		    wyn = fprintf( plik, "%s~ %d %s~ %d\n",
			im->imie, im->powod, im->blokujacy, (int) im->kiedy );
	fclose( plik );

	if ( wyn >= 0 )
	    if ( rename( IMNOWE_FILE, IMIONA_FILE ) < 0 )
		lac_perror( "rename " IMNOWE_FILE );
    }

    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


/*
 * Ulryk 23.9.2003
 * Qwert: wykomentowalem to, ale mysle, ze jeszcze sie do czegos przyda
void load_maskowanie( void )
{
    FILE *plik;
    int mask = 0;
    int zwrot = 0;
    int znak;

    if ( !( plik = fopen( MASKOWANIE_FILE, "r" ) ) )
    {
	lac_perror( MASKOWANIE_FILE );
	return;
    }

    if ( feof( plik ) )
    {
	cbug( "MASKOWANIE_FILE pusty!", 0 );
	fclose( plik );
	return;
    }

    for ( ; !feof( plik ); )
    {
	maskowanie_table[ mask ] = fread_string( plik, &zwrot );
	if ( zwrot )
	{
	    cbug( "blad przy czytaniu z MASKOWANIE_FILE", 0 );
	    fclose( plik );
	    return;
	}

	if ( ++mask >= MAX_MASKOWANIE )
	{
	    cbug( "Za duzo opisow maskowania. Zwieksz MAX_MASKOWANIE.", 0 );
	    fclose( plik );
	    return;
	}

	if ( !feof( plik ) )
	{
	    do
		znak = getc( plik );
	    while ( isspace( znak ) && znak != EOF );
	    if ( znak != EOF )
		ungetc( znak, plik );
	}
    }

    fclose( plik );
    maskowanie_count = mask;

    return;
}
*/


/* Lam 15.5.2000 do load_tracks, i wkrotce do_buy, do_list */
KOLES_DATA *znajdz_kolesia( int vnum )
{
    KOLES_DATA *koles;

    for ( koles = lista_kolesi; koles; koles = koles->nast )
	if ( koles->vnum == vnum )
	    break;

    return ( koles && koles->vnum == vnum ) ? koles : NULL;
}

/*
 * Lam 22.3.99
 * Lam 15.5.2000: wielu kolesi z roznymi drogami
 */
void load_tracks( void )
{
    FILE *plik;
    KOLES_DATA *koles = NULL;
    int vnum;
    int zwrot = 0;
    int znak;

    if ( !( plik = fopen( TRACK_FILE, "r" ) ) )
    {
	lac_perror( TRACK_FILE );
	return;
    }

    if ( feof( plik ) )
    {
	cbug( "TRACK_FILE pusty!", 0 );
	fclose( plik );
	return;
    }

    for ( ; !feof( plik ); )
    {
	do
	    znak = getc( plik );
	while ( isspace( znak ) );
	if ( znak == '#' )
	{
	    vnum = fread_number( plik, &zwrot );
	    if ( znajdz_kolesia( vnum ) )
	    {
		cbug( "Load_tracks: powtorzony vnum kolesia: %d", vnum );
		exit( 1 );
	    }
	    koles = calloc( sizeof( KOLES_DATA ), 1 );
	    koles->vnum = vnum;
	    koles->nast = lista_kolesi;
	    lista_kolesi = koles;
	    continue;
	}
	ungetc( znak, plik );

	if ( !koles )
	{
	    cbug( "Load_tracks: drogi bez przypisanego kolesia", 0 );
	    exit( 1 );
	}

	koles->drogi[ koles->droga ].nazwa = fread_string( plik, &zwrot );
	koles->drogi[ koles->droga ].droga = fread_string( plik, &zwrot );
	if ( zwrot )
	{
	    cbug( "blad przy czytaniu z TRACK_FILE", 0 );
	    fclose( plik );
	    return;
	}

	if ( ++koles->droga >= MAX_TRACK )
	{
	    cbug( "Za duzo drog. Zwieksz MAX_TRACK.", 0 );
	    fclose( plik );
	    return;
	}

	if ( !feof( plik ) )
	{
	    do
		znak = getc( plik );
	    while ( isspace( znak ) && znak != EOF );
	    if ( znak != EOF )
		ungetc( znak, plik );
	}
    }

    fclose( plik );

    return;
}


void load_vnums( void )
{
    struct
    {
	int  *dokad;
	char *klucz;
	bool  bylo;
    } tablica [ ] =
    {
	{ &MOB_VNUM_MUMIA,              "MOB_MUMIA",            FALSE },
	{ &MOB_VNUM_ZOMBIE,             "MOB_ZOMBIE",           FALSE },
	{ &MOB_VNUM_SZKIELET,           "MOB_SZKIELET",         FALSE },
	{ &MOB_VNUM_KUKLA,		"MOB_KUKLA",		FALSE },
	{ &MOB_VNUM_CITYGUARD,		"MOB_CITYGUARD",	FALSE },
	{ &MOB_VNUM_VAMPIRE,		"MOB_VAMPIRE",		FALSE },
	{ &MOB_VNUM_ULT,		"MOB_ULT",		FALSE },
	{ &MOB_VNUM_SECRETARY,		"MOB_SECRETARY",	FALSE },
	{ &MOB_VNUM_MIDGAARD_MAYOR,	"MOB_MIDGAARD_MAYOR",	FALSE },
	{ &MOB_VNUM_BANKIER,		"MOB_BANKIER",		FALSE },
/*	{ &MOB_VNUM_KOLES,		"MOB_KOLES",		FALSE }, */
	{ &MOB_VNUM_PRZECHOWALNIA,	"MOB_PRZECHOWALNIA",	FALSE },
	{ &MOB_VNUM_WARSZTAT,		"MOB_WARSZTAT",		FALSE },
	{ &MOB_VNUM_SUPERMOB,		"MOB_SUPERMOB",		FALSE },
	{ &OBJ_VNUM_MONEY_ONE,		"OBJ_MONEY_ONE",	FALSE },
	{ &OBJ_VNUM_MONEY_SOME,		"OBJ_MONEY_SOME",	FALSE },
	{ &OBJ_VNUM_CORPSE_NPC,		"OBJ_CORPSE_NPC",	FALSE },
	{ &OBJ_VNUM_CORPSE_PC,		"OBJ_CORPSE_PC",	FALSE },
	{ &OBJ_VNUM_FLAKI,		"OBJ_FLAKI",		FALSE },
	{ &OBJ_VNUM_GLOWA,		"OBJ_GLOWA",		FALSE },
	{ &OBJ_VNUM_REKA,		"OBJ_REKA",		FALSE },
	{ &OBJ_VNUM_NOGA,		"OBJ_NOGA",		FALSE },
	{ &OBJ_VNUM_SERCE,		"OBJ_SERCE",		FALSE },
	{ &OBJ_VNUM_OGON,		"OBJ_OGON",		FALSE },
	{ &OBJ_VNUM_SKRZYDLO,		"OBJ_SKRZYDLO",		FALSE },
	{ &OBJ_VNUM_JEZYK,		"OBJ_JEZYK",		FALSE },
	{ &OBJ_VNUM_UCHO,		"OBJ_UCHO",		FALSE },
	{ &OBJ_VNUM_LAPA,		"OBJ_LAPA",		FALSE },
	{ &OBJ_VNUM_LEB,		"OBJ_LEB",		FALSE },
	{ &OBJ_VNUM_ZAB,		"OBJ_ZAB",		FALSE },
	{ &OBJ_VNUM_KIEL,		"OBJ_KIEL",		FALSE },
	{ &OBJ_VNUM_ROG,		"OBJ_ROG",		FALSE },
	{ &OBJ_VNUM_SZPON,		"OBJ_SZPON",		FALSE },
	{ &OBJ_VNUM_PLETWA,		"OBJ_PLETWA",		FALSE },
	{ &OBJ_VNUM_MUSHROOM,		"OBJ_MUSHROOM",		FALSE },
	{ &OBJ_VNUM_LIGHT_BALL,		"OBJ_LIGHT_BALL",	FALSE },
	{ &OBJ_VNUM_SPRING,		"OBJ_SPRING",		FALSE },
	{ &OBJ_VNUM_EGG,		"OBJ_EGG",		FALSE },
	{ &OBJ_VNUM_PORTAL,		"OBJ_PORTAL",		FALSE },
	{ &OBJ_VNUM_BRAMA,		"OBJ_BRAMA",		FALSE },
	{ &OBJ_VNUM_DROGOWSKAZ,		"OBJ_DROGOWSKAZ",	FALSE },
	{ &OBJ_VNUM_CHRUST,             "OBJ_CHRUST",           FALSE },
	{ &OBJ_VNUM_SCHOOL_MACE,	"OBJ_SCHOOL_MACE",	FALSE },
	{ &OBJ_VNUM_SCHOOL_DAGGER,	"OBJ_SCHOOL_DAGGER",	FALSE },
	{ &OBJ_VNUM_SCHOOL_SWORD,	"OBJ_SCHOOL_SWORD",	FALSE },
	{ &OBJ_VNUM_SCHOOL_VEST,	"OBJ_SCHOOL_VEST",	FALSE },
	{ &OBJ_VNUM_SCHOOL_SHIELD,	"OBJ_SCHOOL_SHIELD",	FALSE },
/*	{ &OBJ_VNUM_SCHOOL_BANNER,	"OBJ_SCHOOL_BANNER",	FALSE }, */
	{ &OBJ_VNUM_SCHOOL_CLUB,	"OBJ_SCHOOL_CLUB",	FALSE },
	{ &OBJ_VNUM_BLACK_POWDER,	"OBJ_BLACK_POWDER",	FALSE },
	{ &OBJ_VNUM_STAKE,		"OBJ_STAKE",		FALSE },
	{ &OBJ_VNUM_KREW,		"OBJ_KREW",		FALSE },
	{ &OBJ_VNUM_KALUZA_KRWI,	"OBJ_KALUZA_KRWI",	FALSE },
	{ &OBJ_VNUM_PALAD_MLOT, 	"OBJ_PALAD_MLOT",       FALSE },
	{ &OBJ_VNUM_PALAD_MIECZ,	"OBJ_PALAD_MIECZ",	FALSE },
	{ &OBJ_VNUM_SILVER_POWDER,	"OBJ_SILVER_POWDER",	FALSE },
	{ &OBJ_VNUM_JEDZ_MIN,		"OBJ_JEDZ_MIN",		FALSE },
	{ &OBJ_VNUM_JEDZ_MAX,		"OBJ_JEDZ_MAX",		FALSE },
	{ &ROOM_VNUM_LIMBO,		"ROOM_LIMBO",		FALSE },
	{ &ROOM_VNUM_CHAT,		"ROOM_CHAT",		FALSE },
	{ &ROOM_VNUM_TEMPLE,		"ROOM_TEMPLE",		FALSE },
	{ &ROOM_VNUM_ALTAR,		"ROOM_ALTAR",		FALSE },
	{ &ROOM_VNUM_SCHOOL,		"ROOM_SCHOOL",		FALSE },
	{ &ROOM_VNUM_GRAVEYARD_A,	"ROOM_GRAVEYARD_A",	FALSE },
	{ &ROOM_VNUM_PURGATORY_A,	"ROOM_PURGATORY_A",	FALSE },
	{ &ROOM_VNUM_POZA_SWIATEM,	"ROOM_POZA_SWIATEM",	FALSE },
/*	{ &ROOM_VNUM_PUNKT,		"ROOM_PUNKT",		FALSE }, */
	{ NULL,				NULL,			FALSE }
    };

    FILE *fp;
    char *klucz;
    int   i;
    int   stat = 0;
    char  c;

    if ( !( fp = fopen( VNUM_FILE, "r" ) ) )
    {
	lac_perror( VNUM_FILE );
	if ( system( "touch SHUTDOWN.TXT" ) )
	    log_string( "Nie udalo sie dotknac SHUTDOWN.TXT" );
	exit( 1 );
    }

    c = fread_letter( fp );

    for ( ; !feof( fp ); )
    {
	ungetc( c, fp );
	klucz = fread_word( fp, &stat, TRUE );
	if ( !klucz || stat )
	    break;

	for ( i = 0; tablica[ i ].dokad; i++ )
	    if ( !str_cmp( klucz, tablica[ i ].klucz ) )
		break;

	if ( !tablica[ i ].dokad )
	{
	    sprintf( bug_buf, "load_vnums: nieznany klucz '%s'", klucz );
	    cbug( bug_buf, 0 );
	    fread_number( fp, &stat );
	    c = fread_letter( fp );
	    continue;
	}

	*tablica[ i ].dokad = fread_number( fp, &stat );
	tablica[ i ].bylo = TRUE;
	c = fread_letter( fp );
    }

    for ( i = 0; tablica[ i ].dokad; i++ )
	if ( !tablica[ i ].bylo )
	{
	    sprintf( bug_buf, "load_vnums: nie bylo vnuma: %s", tablica[ i ].klucz );
	    cbug( bug_buf, 0 );
	    if ( system( "touch SHUTDOWN.TXT" ) )
		log_string( "Nie udalo sie dotknac SHUTDOWN.TXT" );
	    exit( 1 );
	}

    fclose( fp );

    return;
}


/*
 * Lam 12.98-3.98: wykonywane przy starcie muda wczytywanie klanow. Liste klanow
 * serwer tworzy sam pod wplywem dzialan klanotworczych graczy. Od razu sa
 * tworzone pomieszczenia klanow, jesli klany takowe posiadaja.
 * Lam 13.3.98: zmiana formatu pliku klanow
 * Lam 16.3.98: urny klanowe zapisywane w pliku przy restarcie
 * ----
 * Malven, 11-12.2003: gruntowna przebudowa funkcji, nie zmienily sie tylko jej
 * nazwa i przeznaczenie
 */
void load_clans( void )
{
    FILE      *fp;
    CLAN_DATA *clan;
    CLAN_DATA *clannext;
    char       buf[ MAX_STRING_LENGTH ];
    int        stat;
    int        letter;

    /*
     * Odczytanie pliku z polozeniami plikow klanowych i zarezerwowanie pamieci
     * na wczytywane ich.
     */
    if ( !( fp = fopen( CLAN_KLANY_FILE, "r" ) ) )
    {
	lac_perror( CLAN_KLANY_FILE );

	/* zalozenie pustego pliku */
	if ( ( fp = fopen( CLAN_KLANY_FILE, "w" ) ) )
	    fclose( fp );
	else
	{
	    sprintf( buf, "load_clans: nie mozna utworzyc pliku %s! STOP!", CLAN_KLANY_FILE );
	    cbug( buf, 0 );
	    exit( -1 );
	}

	return;
    }

    for ( ; ; )
    {
	char *dir;

	while ( isspace( letter = getc( fp ) ) );
	if ( feof( fp ) )
	    break;
	ungetc( letter, fp );

	dir = fread_string( fp, &stat );

	if ( !dir || !strlen( dir ) || ( strlen( dir ) > MAX_CLAN_DIR ) )
	{
	    cbug( "load_clans: nieprawidlowy wpis w pliku z lista klanow", 0 );
	    free_string( dir );
	    continue;
	}

	/* sprawdzenie czy ten sam klan nie bedzie przypadkiem probowal
	   zaladowac sie 2 razy */
	for ( clan = clan_list; clan; clan = clan->next )
	    if ( !strcmp( clan->dir, dir ) )
		break;

	if ( clan )
	{
	    sprintf( buf, "load_clans: w pliku z lista klanow zdublowany wpis klanu %s", dir );
	    cbug( buf, 0 );
	    free_string( dir );
	    continue;
	}

	clan = clan_new( );
	clan->next = clan_list;
	clan_list = clan;
	free_string( clan->dir );
	clan->dir = str_dup( dir );

	free_string( dir );
    }

    fclose( fp );

    /*
     * Wczytanie wszystkich danych klanow
     */
    for ( clan = clan_list; clan; clan = clannext )
    {
	char  path[ MAX_INPUT_LENGTH ];
	char *key;
	char *name;

	clannext = clan->next;

	/*
	 * Podstawowe dane klanu
	 */
	sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
	    CLAN_DANE_FILE );

	if ( !( fp = fopen( path, "r" ) ) )
	{
	    lac_perror( path );
	    clan_remove( clan );
	    continue;
	}

	/* sprawdzenie klucza #CLAN */
	while ( isspace( letter = getc( fp ) ) );
	if ( feof( fp ) )
	{
	    sprintf( buf, "load_clans: brak klucza #CLAN w pliku %s", path );
	    cbug( buf, 0 );
	    clan_remove( clan );
	    fclose( fp );
	    continue;
	}
	ungetc( letter, fp );

	if ( !( key = fread_word( fp, &stat, TRUE ) )
	  || strcmp( key, "#CLAN" ) )
	{
	    sprintf( buf, "load_clans: brak klucza #CLAN w pliku %s", path );
	    cbug( buf, 0 );
	    clan_remove( clan );
	    fclose( fp );
	    continue;
	}

	if ( !( name = fread_string( fp, &stat ) ) )
	{
	    sprintf( buf, "load_clans: blad odczytu wartosci klucza #CLAN w pliku %s", path );
	    cbug( buf, 0 );
	    clan_remove( clan );
	    fclose( fp );
	    continue;
	}

	if ( strlen( name ) < 3 )
	{
	    sprintf( buf, "load_clans: nazwa klanu za krotka w pliku %s", path );
	    cbug( buf, 0 );
	    clan_remove( clan );
	    free_string( name );
	    continue;
	}

	if ( strlen( name ) > MAX_CLAN_NAME )
	{
	    sprintf( buf, "load_clans: nazwa klanu za dluga w pliku %s", path );
	    cbug( buf, 0 );
	    clan_remove( clan );
	    free_string( name );
	    continue;
	}

	free_string( clan->name );
	clan->name = str_dup( name );
	free_string( clan->dop );
	clan->dop = str_dup( name );
	free_string( name );

	/* wczytywanie danych */
	for ( ; ; )
	{
	    char *klucz;
	    char *pomoc;
	    int   i;

	    while ( isspace( letter = getc( fp ) ) );
	    if ( feof( fp ) )
		break;
	    ungetc( letter, fp );

	    klucz = fread_word( fp, &stat, TRUE );
	    if ( !klucz )
	    {
		sprintf( buf, "load_clans: blad odczytu klucza w pliku %s", path );
		cbug( buf, 0 );
		continue;
	    }
	    else if ( !str_cmp( klucz, "Dop" ) )
	    {
		if ( !( pomoc = fread_string( fp, &stat ) ) )
		{
		    sprintf( buf, "load_clans: blad odczytu dopelniacza w pliku %s", path );
		    cbug( buf, 0 );
		    continue;
		}

		if ( strlen( pomoc ) > MAX_CLAN_DOPELNIACZ )
		{
		    sprintf( buf, "load_clans: dopelniacz klanu za dlugi w pliku %s", path );
		    cbug( buf, 0 );
		}
		else
		{
		    free_string( clan->dop );
		    clan->dop = str_dup( pomoc );
		}

		free_string( pomoc );
	    }
	    else if ( !str_cmp( klucz, "Status" ) )
	    {
		i = fread_number( fp, &stat );
		if ( ( i < CLAN_STAT_USUNIETY )
		  || ( i > CLAN_STAT_AKTYWNY ) )
		{
		    sprintf( buf, "load_clans: status klanu spoza zakresu w pliku %s", path );
		    cbug( buf, i );
		    clan->status = CLAN_STAT_USZKODZONY;
		}
		else
		    clan->status = i;
	    }
/*	    TODO
	    else if ( !str_cmp( klucz, "Flags" ) )
		clan->flags	= fread_number( fp, &stat );
	    else if ( !str_cmp( klucz, "Room" ) )
		clan->room	= fread_number( fp, &stat ); */
	    else if ( !str_cmp( klucz, "Info" ) )
	    {
		if ( !( pomoc = fread_string( fp, &stat ) ) )
		{
		    cbug( "Brak informacji o klanie.", 0 );
		    continue;
		}

		free_string( clan->info );
		clan->info = str_dup( pomoc );
		free_string( pomoc );
	    }
	    else if ( !str_cmp( klucz, "Skrot" ) )
	    {
		if ( !( pomoc = fread_string( fp, &stat ) ) )
		{
		    cbug( "Brak skrotu nazwy klanowej.", 0 );
		    continue;
		}

		if ( strlen_pl_kol( pomoc ) >= 6 )
		{
		    cbug( "Skrot nazwy klanowej ma zla dlugosc.", 0 );
		    free_string( pomoc );
		    continue;
		}

		free_string( clan->skrot );
		clan->skrot = str_dup( pomoc );
		free_string( pomoc );
	    }
	    else if ( !str_cmp( klucz, "DataZal" ) )
	    {
		if ( !( pomoc = fread_string( fp, &stat ) ) )
		{
		    cbug( "Brak daty zalozenia klanu.", 0 );
		    continue;
		}

		free_string( clan->data_zal );
		clan->data_zal = str_dup( pomoc );
		free_string( pomoc );
	    }
	    else if ( !str_cmp( klucz, "Gold" ) )
		clan->gold	= fread_number( fp, &stat );
	    else if ( !str_cmp( klucz, "Punkty" ) )
		clan->punkty	= fread_number( fp, &stat );
	    else if ( !str_cmp( klucz, "LvNamMM" ) )
	    {
		i = fread_number( fp, &stat );
		if ( !stat && ( pomoc = fread_string( fp, &stat ) ) )
		{
		    if ( ( i >= 0 ) && ( i <= 9 ) && strlen( pomoc ) )
		    {
			free_string( clan->lev_name_mm[ i ] );
			clan->lev_name_mm[ i ] = str_dup( pomoc );
		    }
		    free_string( pomoc );
		}
	    }
	    else if ( !str_cmp( klucz, "LvNamZM" ) )
	    {
		i = fread_number( fp, &stat );
		if ( !stat && ( pomoc = fread_string( fp, &stat ) ) )
		{
		    if ( ( i >= 0 ) && ( i <= 9 ) && strlen( pomoc ) )
		    {
			free_string( clan->lev_name_zm[ i ] );
			clan->lev_name_zm[ i ] = str_dup( pomoc );
		    }
		    free_string( pomoc );
		}
	    }
	    else if ( !str_cmp( klucz, "LvNamMD" ) )
	    {
		i = fread_number( fp, &stat );
		if ( !stat && ( pomoc = fread_string( fp, &stat ) ) )
		{
		    if ( ( i >= 0 ) && ( i <= 9 ) && strlen( pomoc ) )
		    {
			free_string( clan->lev_name_md[ i ] );
			clan->lev_name_md[ i ] = str_dup( pomoc );
		    }
		    free_string( pomoc );
		}
	    }
	    else if ( !str_cmp( klucz, "LvNamZD" ) )
	    {
		i = fread_number( fp, &stat );
		if ( !stat && ( pomoc = fread_string( fp, &stat ) ) )
		{
		    if ( ( i >= 0 ) && ( i <= 9 ) && strlen( pomoc ) )
		    {
			free_string( clan->lev_name_zd[ i ] );
			clan->lev_name_zd[ i ] = str_dup( pomoc );
		    }
		    free_string( pomoc );
		}
	    }
	    else if ( !str_cmp( klucz, "LvNamMN" ) )
	    {
		i = fread_number( fp, &stat );
		if ( !stat && ( pomoc = fread_string( fp, &stat ) ) )
		{
		    if ( ( i >= 0 ) && ( i <= 9 ) && strlen( pomoc ) )
		    {
			free_string( clan->lev_name_mn[ i ] );
			clan->lev_name_mn[ i ] = str_dup( pomoc );
		    }
		    free_string( pomoc );
		}
	    }
	    else if ( !str_cmp( klucz, "LvNamZN" ) )
	    {
		i = fread_number( fp, &stat );
		if ( !stat && ( pomoc = fread_string( fp, &stat ) ) )
		{
		    if ( ( i >= 0 ) && ( i <= 9 ) && strlen( pomoc ) )
		    {
			free_string( clan->lev_name_zn[ i ] );
			clan->lev_name_zn[ i ] = str_dup( pomoc );
		    }
		    free_string( pomoc );
		}
	    }
	    else
	    {
		sprintf( buf, "load_clans: nierozpoznany klucz w pliku %s", path );
		cbug( buf, 0 );
	    }
	}

	fclose( fp );

/*
 * Informacje klanowe
 */
	load_clan_statut( clan );

/*
 * Lista klanowiczow
 */
	{
	    CLAN_MEMBER_DATA *member;
	    CLAN_MEMBER_DATA *member_list[ 10 ];
	    CLAN_MEMBER_DATA *member_last[ 10 ];
	    FILE             *fp2;
	    int               i;
	    int               prev;

	    sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
		CLAN_LISTA_FILE );

	    if ( !( fp = fopen( path, "r" ) ) )
	    {
		/* brak pliku z lista klanowiczow */
		lac_perror( path );
		clan->status = CLAN_STAT_USZKODZONY;
		continue;
	    }

	    /* sprawdzenie klucza #CLAN */
	    while ( isspace( letter = getc( fp ) ) );
	    if ( feof( fp ) )
	    {
		sprintf( buf, "load_clans: brak klucza #CLAN w pliku %s", path );
		cbug( buf, 0 );
		clan->status = CLAN_STAT_USZKODZONY;
		fclose( fp );
		continue;
	    }
	    ungetc( letter, fp );

	    if ( !( key = fread_word( fp, &stat, TRUE ) )
	      || strcmp( key, "#CLAN" ) )
	    {
		sprintf( buf, "load_clans: brak klucza #CLAN w pliku %s", path );
		cbug( buf, 0 );
		clan->status = CLAN_STAT_USZKODZONY;
		fclose( fp );
		continue;
	    }

	    if ( !( name = fread_string( fp, &stat ) ) )
	    {
		sprintf( buf, "load_clans: blad odczytu wartosci klucza #CLAN w pliku %s", path );
		cbug( buf, 0 );
		clan->status = CLAN_STAT_USZKODZONY;
		fclose( fp );
		continue;
	    }

	    if ( strcmp( name, clan->name ) )
	    {
		sprintf( buf, "load_clans: niezgodna z nazwa klanu wartosc klucza #CLAN w pliku %s", path );
		cbug( buf, 0 );
		clan->status = CLAN_STAT_USZKODZONY;
		fclose( fp );
		free_string( name );
		continue;
	    }

	    free_string( name );
	    for ( i = 0; i < 10; i++ )
		member_list[ i ] = member_last[ i ] = NULL;

	    /* Czyta plik i tworzy posortowana wg poziomu liste klanowiczow.
	       Robi to w taki sposob, ze tworzy liste dla kazdego stopnia
	       klanowego i laduje na nia wszystkich o tym stopniu. Na koncu
	       listy sa sklejane od 1 do 9 i dzieki temu wynikowa lista jest
	       uporzadkowana jak nalezy. */
	    for ( ; ; )
	    {
		int   level;
		char *name;
		int   zloto;
		int   punkty;
		long  seen;
		bool  found;

		while ( isspace( letter = getc( fp ) ) );
		if ( feof( fp ) )
		    break;
		ungetc( letter, fp );

		level = fread_number( fp, &stat );
		if ( stat || level < 0 || level > 9 )
		{
		    sprintf( buf, "load_clans: nieprawidlowy wpis dla poziomu w pliku %s", path );
		    cbug( buf, stat );
		    continue;
		}

		if ( !( name = fread_string( fp, &stat ) ) )
		{
		    sprintf( buf, "load_clans: nieprawidlowy wpis dla imienia w pliku %s", path );
		    cbug( buf, stat );
		    continue;
		}

		zloto = fread_number( fp, &stat );
		if ( stat || level < 0 )
		{
		    sprintf( buf, "load_clans: nieprawidlowy wpis dla zlota w pliku %s", path );
		    cbug( buf, stat );
		    continue;
		}

		punkty = fread_number( fp, &stat );
		if ( stat || punkty < 0 )
		{
		    sprintf( buf, "load_clans: nieprawidlowy wpis dla punktow w pliku %s", path );
		    cbug( buf, stat );
		    continue;
		}

		seen = fread_number( fp, &stat );
		if ( stat || seen < 0 )
		{
		    sprintf( buf, "load_clans: nieprawidlowy wpis dla czasu ostatniej wizyty w pliku %s", path );
		    cbug( buf, stat );
		    continue;
		}

		if ( !strlen( name )
		  || ( strlen( name ) > MAX_PLAYER_NAME ) )
		{
		    sprintf( buf, "load_clans: nieprawidlowa dlugosc wpisu dla imienia w pliku %s", path );
		    cbug( buf, stat );
		    free_string( name );
		    continue;
		}

		/* Sprawdzenie czy gracz istnieje na wypadek recznego skasowania
		   jego pliku postaci itp. Jesli nie, jest on usuwany z listy
		   czlonkow klanu. */
		found = FALSE;

#if !defined( macintosh ) && !defined( MSDOS )
		sprintf( buf, PLAYER_DIR "%s" DIR_SEPARATOR "%s",
		    initial( name ), capitalize( name ) );

		if ( ( fp2 = fopen( buf, "r" ) ) )
		{
		    found = TRUE;
		    fclose( fp2 );
		}
		else
		{
		    /* moze byc skompresowany gzipem */
		    strcat( buf, ".gz" );
		    if ( ( fp2 = fopen( buf, "r" ) ) )
		    {
			found = TRUE;
			fclose( fp2 );
		    }
		}
#else
		sprintf( buf, PLAYER_DIR "%s", capitalize( name ) );
		if ( ( fp2 = fopen( buf, "r" ) ) )
		{
		    found = TRUE;
		    fclose( fp2 );
		}
#endif
		if ( found )
		{
		    /* umieszczenie na liscie klanowiczow o danym stopniu */
		    member = clan_member_new( );

		    member->next = NULL;
		    member->level = ( short ) level;
		    member->zloto = zloto;
		    member->punkty = punkty;
		    member->seen = seen;
		    free_string( member->name );
		    member->name = str_dup( name );

		    if ( !member_list[ level ] )
			member_list[ level ] = member;
		    else
			member_last[ level ]->next = member;

		    member_last[ level ] = member;
		}

		free_string( name );
	    }

	    fclose( fp );

	    /* polaczenie list pomocniczych w posortowana calosc */
	    clan->member_list = member_list[ 1 ];

	    prev = 0;
	    for ( i = 1; i < 10; i++ )
	    {
		if ( !member_list[ i ] )
		    continue;

		if ( prev )
		    member_last[ prev ]->next = member_list[ i ];
		else
		    clan->member_list = member_list[ i ];

		prev = i;
	    }
	}
    }

    /*
     * Stosunki miedzyklanowe
     */
    for ( clan = clan_list; clan; clan = clannext )
    {
	char  path[ MAX_INPUT_LENGTH ];
	char *key;
	char *name;

	clannext = clan->next;

	{
	    sprintf( path, "%s%s%s%s", CLAN_DIR, clan->dir, DIR_SEPARATOR,
		CLAN_STOSUNKI_FILE );

	    if ( !( fp = fopen( path, "r" ) ) )
	    {
		/* brak pliku z lista stosunkow */
		lac_perror( path );
		clan->status = CLAN_STAT_USZKODZONY;
		continue;
	    }

	    /* sprawdzenie klucza #CLAN */
	    while ( isspace( letter = getc( fp ) ) );
	    if ( feof( fp ) )
	    {
		sprintf( buf, "load_clans: brak klucza #CLAN w pliku %s", path );
		cbug( buf, 0 );
		clan->status = CLAN_STAT_USZKODZONY;
		fclose( fp );
		continue;
	    }
	    ungetc( letter, fp );

	    if ( !( key = fread_word( fp, &stat, TRUE ) )
	      || strcmp( key, "#CLAN" ) )
	    {
		sprintf( buf, "load_clans: brak klucza #CLAN w pliku %s", path );
		cbug( buf, 0 );
		clan->status = CLAN_STAT_USZKODZONY;
		fclose( fp );
		continue;
	    }

	    if ( !( name = fread_string( fp, &stat ) ) )
	    {
		sprintf( buf, "load_clans: blad odczytu wartosci klucza #CLAN w pliku %s", path );
		cbug( buf, 0 );
		clan->status = CLAN_STAT_USZKODZONY;
		fclose( fp );
		continue;
	    }

	    if ( strcmp( name, clan->name ) )
	    {
		sprintf( buf, "load_clans: niezgodna z nazwa klanu wartosc klucza #CLAN w pliku %s", path );
		cbug( buf, 0 );
		clan->status = CLAN_STAT_USZKODZONY;
		fclose( fp );
		free_string( name );
		continue;
	    }

	    free_string( name );

	    /* wczytanie listy stosunkow */
	    for ( ; ; )
	    {
		CLAN_DATA     *klan;
		bool           war;
		int            pkill;
		int            pdead;
		CLAN_REL_DATA *rel;

		while ( isspace( letter = getc( fp ) ) );
		if ( feof( fp ) )
		    break;
		ungetc( letter, fp );

		if ( !( name = fread_string( fp, &stat ) ) )
		{
		    sprintf( buf, "load_clans: nieprawidlowy wpis dla nazwy klanu w pliku %s", path );
		    cbug( buf, 0 );
		    continue;
		}
		if ( !( klan = clan_lookup( name ) ) )
		{
		    sprintf( buf, "load_clans: znaleziono relacje z nieistniejacym klanem \"%s\"", name );
		    cbug( buf, 0 );
		    free_string( name );
		    continue;
		}

		free_string( name );

		war = fread_number( fp, &stat );
		if ( stat || ( war != TRUE && war != FALSE ) )
		{
		    sprintf( buf, "load_clans: nieprawidlowy wpis dla wojny w pliku %s", path );
		    cbug( buf, stat );
		    continue;
		}

		pkill = fread_number( fp, &stat );
		if ( stat || ( pkill < 0 ) )
		{
		    sprintf( buf, "load_clans: nieprawidlowy wpis dla pkill w pliku %s", path );
		    cbug( buf, stat );
		    continue;
		}

		pdead = fread_number( fp, &stat );
		if ( stat || ( pdead < 0 ) )
		{
		    sprintf( buf, "load_clans: nieprawidlowy wpis dla pdead w pliku %s", path );
		    cbug( buf, stat );
		    continue;
		}

		rel = clan_rel_new( );
		rel->next = clan->rel_list;
		clan->rel_list = rel;

		rel->clan  = klan;
		rel->war   = war;
		rel->pkill = pkill;
		rel->pdead = pdead;
	    }

	    fclose( fp );
	}

    /*
     * TODO: Skrzynia klanowa
     */

    }

    check_clan_rel( );

    return;
}


/*
 * Lam 23.3.99: czyta z pliku czas w postaci 00:00 (godziny:minuty). Zwraca
 * FALSE w przypadku bledu. A bledow w zasadzie nie widzi, o to niech sie
 * troszcza ludzie piszacy jebut.txt
 */
bool fread_time( FILE *fp, int *godz, int *min )
{
    int c;
    int tymczas;

    do
	c = getc( fp );
    while ( isspace( c ) );

    tymczas = 0;

    if ( !isdigit( c ) )
    {
	cbug( "Fread_time: brak cyfry (godzina)", 0 );
	return FALSE;
    }

    while ( isdigit( c ) )
    {
	tymczas = tymczas * 10 + c - '0';
	c = getc( fp );
    }

    if ( c != ':' )
    {
	cbug( "Fread_time: brak dwukropka", 0 );
	return FALSE;
    }

    *godz = tymczas;

    do
	c = getc( fp );
    while ( isspace( c ) );

    tymczas = 0;

    if ( !isdigit( c ) )
    {
	cbug( "Fread_time: brak cyfry (minuta)", 0 );
	return FALSE;
    }

    while ( isdigit( c ) )
    {
	tymczas = tymczas * 10 + c - '0';
	c = getc( fp );
    }

    *min = tymczas;

    if ( !isspace( c ) )
	ungetc( c, fp );

    return TRUE;
}


void load_down_time( void )
{
    FILE *fp;
    int   godz, min;
    struct tm *czas;
    time_t tymczas;


    down_time = 0;
    warning1  = 0;
    warning2  = 0;
    Reboot    = FALSE;

    if ( !( fp = fopen( DOWN_TIME_FILE, "r" ) ) )
	return;

    for ( ; ; )
    {
	char *word;
	int   letter;
	int   stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		if ( down_time > 0 && down_time < 301 )
		{
		    down_time  = current_time + 300;
		    warning2   = down_time - 151; /* -1 - Lam */
		    warning1   = warning2  - 75;
		}
		else
		    if ( down_time > 0 )
		    {
			down_time += current_time;
			warning2   = down_time - 151; /* -1 - Lam */
			warning1   = warning2  - 150;
		    }
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );

	word = fread_word( fp, &stat, TRUE );

	if ( !str_cmp( word, "co" ) )
	{
	    fread_time( fp, &godz, &min );
	    tymczas = (time_t) ( ( godz * 3600 ) + ( min * 60 ) );
	    if ( !down_time || tymczas < down_time )
		down_time = tymczas;
	}
	else if ( !str_cmp( word, "o" ) )
	{
	    fread_time( fp, &godz, &min );

	    czas = localtime( &current_time );
	    /* Lam 4.6.99: poprawka (nie uwzglednial sekund) */
	    if ( czas->tm_hour > godz
	      || ( czas->tm_hour == godz
		&& ( czas->tm_min > min
		  || ( czas->tm_min == min
		    && czas->tm_sec > 0 ) ) ) )
	    {
		tymczas = current_time + ( 24 * 60 * 60 );
		czas = localtime( &tymczas );
	    }

	    czas->tm_sec = 0;
	    czas->tm_min = min;
	    czas->tm_hour = godz;

	    tymczas = mktime( czas ) - (time_t) current_time;
	    if ( !down_time || tymczas < down_time )
		down_time = tymczas;
	}
	else if ( !str_cmp( word, "jebut" ) )
	    Reboot = TRUE;
	else if ( *word == '*' )
	    fread_to_eol( fp );
    }

    return;
}


/*
 * Load up the ban file
 */
void load_ban( void )
{
    BAN_DATA  *pban;
    FILE      *fp;

    if ( !( fp = fopen( BAN_FILE, "r" ) ) )
	return;

    for ( ; ; )
    {
	int letter;
	int stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );

	if ( ban_free == NULL )
	{
	    pban     = alloc_perm( sizeof( *pban ) );
	}
	else
	{
	    pban     = ban_free;
	    ban_free = ban_free->next;
	}

	pban->host   = fread_string( fp, &stat );
	pban->user   = fread_string( fp, &stat );
	pban->type   = fread_number( fp, &stat );
	pban->banned = fread_string( fp, &stat );

	pban->next   = ban_list;
	ban_list     = pban;
    }

    return;
}


/*
 * Cadaver 16.12.2001: ladowanie schematow barw
 */
void load_schematy( void )
{
    SCHEMAT_DATA *pschematlast;
    FILE *fp;

    pschematlast = alloc_perm( sizeof( *pschematlast ) );
    pschematlast->nazwa = str_dup( "domy`slny" );
    pschematlast->schemat = str_dup( "----------------------------------------" );
    pschematlast->next = NULL;
    schemat_list = pschematlast;

    if ( !( fp = fopen( SCHEMATY_FILE, "r" ) ) )
    {
	lac_perror( SCHEMATY_FILE );
	return;
    }

    for ( ; ; )
    {
	SCHEMAT_DATA *pschemat;
	int letter;
	int stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );

	pschemat = alloc_perm( sizeof( *pschemat ) );

	pschemat->nazwa = fread_string( fp, &stat );
	pschemat->schemat = fread_string( fp, &stat );

	pschematlast->next = pschemat;

	pschematlast = pschemat;
    }
/*    fclose( fp ); - will never be executed */
    return;
}


void load_lsc( void )
{
    FILE *fp;

    /* Brak pliku to nie problem. lsc_init_str_len pozostanie 0 i LSC bedzie
       wylaczony w obu miejscach gdzie jest uzywany. */
    if ( !( fp = fopen( LSC_FILE, "r" ) ) )
	return;

    for ( ; ; )
    {
	char *word;
	int   letter;
	int   stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		if ( *lsc_init_str && *lsc_clear_str && lsc_delimiter )
		    lsc_init_str_len = strlen( lsc_init_str );
		else
		    cbug( "LSC: konfiguracja niepelna, LSC nieaktywny.", 0 );
		fclose( fp );
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );

	word = fread_word( fp, &stat, TRUE );

	if ( !str_cmp( word, "LSCInit" ) )
	{
	    word = fread_word( fp, &stat, TRUE );
	    if ( stat || !*word )
		return;
	    strcpy( lsc_init_str, word );
	}
	else if ( !str_cmp( word, "LSCDelim" ) )
	{
	    word = fread_word( fp, &stat, TRUE );
	    if ( stat || !*word )
		return;
	    if ( strlen( word ) > 1 )
	    {
		cbug( "LSC: LSCDelim musi byc jednym znakiem.", 0 );
		return;
	    }
	    lsc_delimiter = word[ 0 ];
	}
	else if ( !str_cmp( word, "LSCClear" ) )
	{
	    word = fread_word( fp, &stat, TRUE );
	    if ( stat || !*word )
		return;
	    strcpy( lsc_clear_str, word );
	}
	else if ( *word == '*' )
	    fread_to_eol( fp );
	else
	    cbug( "Nieznane slowo kluczowe w konfiguracji LSC!", 0 );
    }


    return;
}



/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad or suspicious reverse exits.
 */
void fix_exits( void )
{
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;
    ROOM_INDEX_DATA *to_room = NULL;
    ROOM_INDEX_DATA *pRoomIndex;
    char buf[ MAX_STRING_LENGTH ];
    int iHash;
    int door;
    bool bootdb;

    bootdb = fBootDb;
    fBootDb = FALSE;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex = room_index_hash[ iHash ];
	      pRoomIndex;
	      pRoomIndex = pRoomIndex->next )
	{
	    bool fexit;

	    fexit = FALSE;
	    for ( door = 0; door <= 9; door++ )
	    {
		if ( ( pexit = pRoomIndex->exit[ door ] ) )
		{
		    fexit = TRUE;
		    if ( pexit->vnum <= 0 )
			pexit->to_room = NULL;
		    else
		    {
			pexit->to_room = get_room_index( pexit->vnum );
			if ( !pexit->to_room )
			{
			    sprintf( buf, "Fix_exits [%s]: pomieszczenie %d ma w kierunku %d (%s) wyjscie do pomieszczenia o nieznanym vnumie (%d)",
				pRoomIndex->area->file_name, pRoomIndex->vnum,
				door, kierunki[ door ].skrot, pexit->vnum );
			    cbug( buf, 0 );
			    exit( 1 );
			}
		    }

		    /* Lam 16.4.2005 */
		    if ( ODPLUSKWIACZ
		      && pRoomIndex->sector_type == SECT_UNDERWATER
		      && door != DIR_UP
		      && pexit->to_room
		      && pexit->to_room->sector_type != SECT_UNDERWATER )
		    {
			sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest pod woda, ale ma wyjscie do pomieszczen nie pod woda w kierunku %d (%s)",
			    pRoomIndex->area->file_name, pRoomIndex->vnum,
			    door, kierunki[ door ].skrot );
			cbug( buf, 0 );
		    }

		    /* Lam 16.4.2005 */
		    if ( ODPLUSKWIACZ
		      && pexit
		      && !pexit->to_room
		      && IS_SET( pexit->exit_info, EX_ISDOOR ) )
		    {
			sprintf( buf, "Fix_exits [%s]: pom. %d ma wyjscie w kierunku %d (%s) bez pom. docelowego, ale bedace drzwiami",
			    pRoomIndex->area->file_name, pRoomIndex->vnum,
			    door, kierunki[ door ].skrot );
			cbug( buf, 0 );
		    }

		    /* Lam 16.4.2005 na prosbe Skifra (#278) */
		    if ( ODPLUSKWIACZ
		      && pexit
		      && pexit->to_room
		      && IS_SET( pexit->exit_info, EX_ISDOOR ) )
		    {
			RESET_DATA *pTmpReset;
			int ileich = 0;

			for ( pTmpReset = pRoomIndex->area->reset_first; pTmpReset; pTmpReset = pTmpReset->next )
			    if ( pTmpReset->command == 'D'
			      && pTmpReset->arg1 == pRoomIndex->vnum
			      && pTmpReset->arg2 == door )
				ileich++;

			if ( !ileich
			  && !IS_SET( pexit->exit_info, EX_DONTRESET ) )
			{
			    sprintf( buf, "Fix_exits [%s]: Brak resetu dla wyjscia z %d w kierunku %d (%s)",
				pRoomIndex->area->file_name, pRoomIndex->vnum,
				door, kierunki[ door ].skrot );
			    cbug( buf, 0 );
			}

			if ( ileich
			  && IS_SET( pexit->exit_info, EX_DONTRESET ) )
			{
			    sprintf( buf, "Fix_exits [%s]: Istnieje reset dla wyjscia z %d w kierunku %d (%s), choc wyjscie oznaczono jako nie wymagajace ustawiania resetem",
				pRoomIndex->area->file_name, pRoomIndex->vnum,
				door, kierunki[ door ].skrot );
			    cbug( buf, 0 );
			}

			if ( ileich > 1 )
			{
			    sprintf( buf, "Fix_exits [%s]: Wiecej niz jeden reset dla wyjscia z %d w kierunku %d (%s)",
				pRoomIndex->area->file_name, pRoomIndex->vnum,
				door, kierunki[ door ].skrot );
			    cbug( buf, 0 );
			}
		    }
		}
	    }

	    if ( !fexit )
		SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );

	    if ( ODPLUSKWIACZ
	      && pRoomIndex->sector_type == SECT_AIR
	      && ( !( pexit = pRoomIndex->exit[ DIR_DOWN ] )
		|| !pexit->to_room ) )
	    {
		sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest w powietrzu, ale nie ma wyjscia w dol",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		cbug( buf, 0 );
	    }

	    /* Lam 16.4.2005 */
	    if ( ODPLUSKWIACZ
	      && pRoomIndex->sector_type == SECT_AIR
	      && ( pexit = pRoomIndex->exit[ DIR_UP ] )
	      && pexit->to_room
	      && pexit->to_room->sector_type != SECT_AIR )
	    {
		sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest w powietrzu, ma wyjscie w gore, ale nad nim nie ma powietrza",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		cbug( buf, 0 );
	    }

	    /* Lam 16.4.2005 wszystkie typy wody ponizej */
	    if ( ODPLUSKWIACZ
	      && pRoomIndex->sector_type == SECT_WATER_DEEP )
	    {
		if ( !( pexit = pRoomIndex->exit[ DIR_DOWN ] )
		  || !pexit->to_room )
		{
		    sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest gleboka woda, ale nie ma wyjscia w dol",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		    cbug( buf, 0 );
		}

		if ( pexit
		  && IS_SET( pexit->exit_info, EX_ISDOOR ) )
		{
		    sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest gleboka woda, ma wyjscie w dol, ale to drzwi",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		    cbug( buf, 0 );
		}

		if ( pexit
		  && pexit->to_room
		  && pexit->to_room->sector_type != SECT_UNDERWATER )
		{
		    sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest gleboka woda, ma wyjscie w dol, ale tam jest pomieszczenie nie pod woda",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		    cbug( buf, 0 );
		}
	    }

	    if ( ODPLUSKWIACZ
	      && pRoomIndex->sector_type == SECT_UNDERWATER )
	    {
		if ( !IS_SET( pRoomIndex->room_flags, ROOM_INDOORS )
		  && ( !( pexit = pRoomIndex->exit[ DIR_UP ] )
		    || !pexit->to_room ) )
		{
		    sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest pod woda, nie jest pomieszczeniem, ale nie ma wyjscia w gore",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		    cbug( buf, 0 );
		}

		if ( pexit
		  && IS_SET( pexit->exit_info, EX_ISDOOR ) )
		{
		    sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest pod woda, ma wyjscie w dol, ale to drzwi",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		    cbug( buf, 0 );
		}

		if ( pexit
		  && pexit->to_room
		  && pexit->to_room->sector_type != SECT_UNDERWATER
		  && pexit->to_room->sector_type != SECT_WATER_DEEP )
		{
		    sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest pod woda, ma wyjscie w gore, ale tam nie ma glebokiej wody/pod woda",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		    cbug( buf, 0 );
		}
	    }

	    if ( ODPLUSKWIACZ
	      && pRoomIndex->sector_type == SECT_WATER_SHALLOW
	      && ( pexit = pRoomIndex->exit[ DIR_DOWN ] )
	      && pexit->to_room )
	    {
		sprintf( buf, "Fix_exits [%s]: pomieszczenie %d jest plytka woda, ale ma wyjscie w dol",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		cbug( buf, 0 );
	    }

	    /* Lam 2.6.2006 */
	    if ( ODPLUSKWIACZ
	      && pRoomIndex->trap
	      && pRoomIndex->trap->trap_type == 1
	      && ( !( pexit = pRoomIndex->exit[ DIR_UP ] )
		|| !pexit->to_room
		|| pexit->to_room->sector_type != SECT_AIR ) )
	    {
		sprintf( buf, "Fix_exits [%s]: pomieszczenie %d ma pulapke typu 1, ale nad nim nie ma powietrza",
			pRoomIndex->area->file_name, pRoomIndex->vnum );
		cbug( buf, 0 );
	    }

	    /* Lam 19.10.2008, #641, nie ma nic wspolnego z fix_exits, ale
	       idealnie tu pasuje */
	    if ( ODPLUSKWIACZ
	      && IS_SET( pRoomIndex->room_flags, ROOM_PET_SHOP ) )
	    {
		RESET_DATA *pTmpReset;
		int sprzedawcy = 0;
		bool ruchomy = FALSE;
		bool butdb;

		butdb = fBootDb;
		fBootDb = FALSE;
		if ( !get_room_index( pRoomIndex->vnum + 1 ) )
		    cbug( "Pomieszczenie %d: sklep ze zwierzetami bez zaplecza", pRoomIndex->vnum );
		fBootDb = butdb;

		for ( pTmpReset = pRoomIndex->area->reset_first; pTmpReset; pTmpReset = pTmpReset->next )
		    if ( pTmpReset->command == 'M'
		      && pTmpReset->arg3 == pRoomIndex->vnum )
		    {
			sprzedawcy += pTmpReset->arg2;
			if ( !IS_SET( get_mob_index( pTmpReset->arg1 )->act, ACT_SENTINEL ) )
			    ruchomy = TRUE;
		    }

		if ( sprzedawcy != 1 )
		{
		    sprintf( buf, "Pomieszczenie %d: sklep ze zwierzetami ma %d sprzedawcow (musi miec 1)",
			pRoomIndex->vnum, sprzedawcy );
		    cbug( buf, 0 );
		}
		else if ( ruchomy )
		    cbug( "Pomieszczenie %d: sklep ze zwierzetami ma ruchomego sprzedawce", pRoomIndex->vnum );
	    }
	}
    }

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex = room_index_hash[ iHash ];
	      pRoomIndex;
	      pRoomIndex = pRoomIndex->next )
	{
	    for ( door = 0; door <= 9; door++ )
	    {
		pexit = pRoomIndex->exit[ door ];
		pexit_rev = NULL;
		if ( ODPLUSKWIACZ
		  && pexit
		  && ( to_room = pexit->to_room ) )
		{
		    if ( ( pexit_rev = to_room->exit[ kierunki[ door ].odwrotny ] )
		      && pexit_rev->to_room != pRoomIndex
		      /* dopuszczam "blad" powrotu pod warunkiem, ze stad jest
			pass_prog, a z powrotem jest tylko opis wyjscia */
		      && ( pexit_rev->to_room
			|| !IS_SET( pexit->progtypes, E_PASS_PROG ) ) )
		    {
			sprintf( buf, "Fix_exits [%s]: blad drogi powrotnej: %d:%s -> %d:%s -> %d.",
			    pRoomIndex->area->file_name, pRoomIndex->vnum,
			    kierunki[ door ].skrot, to_room->vnum,
			    kierunki[ kierunki[ door ].odwrotny ].skrot,
			    ( !pexit_rev->to_room ) ? 0 : pexit_rev->to_room->vnum );
			cbug( buf, 0 );
		    }
		    else
		    {
			if ( !IS_SET( pexit->progtypes, E_PASS_PROG )
			  && ( !( pexit_rev = to_room->exit[ kierunki[ door ].odwrotny ] )
			    || !pexit_rev->to_room ) )
			{
			    sprintf( buf, "Fix_exits [%s]: brak drogi powrotnej lub pass_proga: %d:%s -> %d:%s -> (brak)",
				pRoomIndex->area->file_name, pRoomIndex->vnum,
				kierunki[ door ].skrot, to_room->vnum,
				kierunki[ kierunki[ door ].odwrotny ].skrot );
			    cbug( buf, 0 );
			}
		    }
		}

		/* Lam 17.11.2003 */
		if ( pexit && !pexit->key )
		{
		    sprintf( buf, "Fix_exits [%s]: drzwi %d:%s, klucz 0 - ustawiam -1.",
			pRoomIndex->area->file_name, pRoomIndex->vnum,
			kierunki[ door ].skrot );
		    cbug( buf, 0 );
		    pexit->key = -1;
		}

		if ( ODPLUSKWIACZ
		  && pexit && to_room && pexit_rev
		  && pexit_rev->to_room == pRoomIndex )
		{
		    /* Lam 10.11.2003 */
		    if ( pexit->key != pexit_rev->key )
		    {
			sprintf( buf, "Fix_exits [%s]: rozne klucze z dwoch stron: %d (%d:%s) != %d (%d:%s)!",
				pRoomIndex->area->file_name, pexit->key,
				pRoomIndex->vnum, kierunki[ door ].skrot,
				pexit_rev->key,	to_room->vnum,
				kierunki[ kierunki[ door ].odwrotny ].skrot );
			cbug( buf, 0 );
		    }
		    /* Lam 25.2.2006, swiadomie strcmp */
		    if ( strcmp( pexit->keyword, pexit_rev->keyword ) )
		    {
			sprintf( buf, "Fix_exits [%s]: %d:%s i %d:%s: rozne slowa kluczowe z dwoch stron (\"%s\" i \"%s\").",
				pRoomIndex->area->file_name, pRoomIndex->vnum,
				kierunki[ door ].skrot, to_room->vnum,
				kierunki[ kierunki[ door ].odwrotny ].skrot,
				pexit->keyword, pexit_rev->keyword );
			cbug( buf, 0 );
		    }
		    if ( strcmp( pexit->short_descr, pexit_rev->short_descr ) )
		    {
			sprintf( buf, "Fix_exits [%s]: %d:%s i %d:%s: rozne mianowniki z dwoch stron (\"%s\" i \"%s\").",
				pRoomIndex->area->file_name, pRoomIndex->vnum,
				kierunki[ door ].skrot, to_room->vnum,
				kierunki[ kierunki[ door ].odwrotny ].skrot,
				pexit->short_descr, pexit_rev->short_descr );
			cbug( buf, 0 );
		    }
		    if ( strcmp( pexit->biernik, pexit_rev->biernik ) )
		    {
			sprintf( buf, "Fix_exits [%s]: %d:%s i %d:%s: rozne bierniki z dwoch stron (\"%s\" i \"%s\").",
				pRoomIndex->area->file_name, pRoomIndex->vnum,
				kierunki[ door ].skrot, to_room->vnum,
				kierunki[ kierunki[ door ].odwrotny ].skrot,
				pexit->biernik, pexit_rev->biernik );
			cbug( buf, 0 );
		    }
		}

		/* Lam 10.11.2003 */
		if ( ODPLUSKWIACZ
		  && pexit && to_room && pexit_rev
		  && pexit_rev->to_room == pRoomIndex
		  && ( pexit->exit_info & ~( EX_DIG | EX_BURIED | EX_DONTRESET | EX_BEZKLAMKI | EX_BEZZAMKA) )
			 != ( pexit_rev->exit_info & ~( EX_DIG | EX_BURIED | EX_DONTRESET | EX_BEZKLAMKI | EX_BEZZAMKA ) ) )
		{
		    sprintf( buf, "Fix_exits [%s]: rozne flagi przejsc: %d:%s != %d:%s!",
			pRoomIndex->area->file_name, pRoomIndex->vnum,
			kierunki[ door ].skrot, to_room->vnum,
			kierunki[ kierunki[ door ].odwrotny ].skrot );
		    cbug( buf, 0 );
		}
	    }
	}
    }

    fBootDb = bootdb;

    return;
}


/*
 * Repopulate areas periodically.
 */
void area_update( void )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( ++pArea->age < 3 )
	    continue;

	/*
	 * Check for PC's.
	 * Lam 25.6.98: w Szkole i w Limbo nie widac resetow (np. przy
	 * zmienianiu hasla)
	 * Lam 24.5.99: komunikaty resetowania
	 * Lam 20.5.2002: to_szkola
	 */
	/* Wylaczone 15.5.2006 - same problemy, kiedy jest jeden komunikat
	   dla calej krainy, w ktorej jest kilka typow podloza.

	ZONE_DATA *strefa;
	bool to_szkola = FALSE;

	for ( strefa = zone_first; strefa; strefa = strefa->next )
	    if ( pArea == get_room_index( strefa->szkola )->area )
		to_szkola = TRUE;

	if ( pArea->nplayer > 0 && pArea->age == 15 - 1
		&& !to_szkola
		&& pArea != get_room_index( ROOM_VNUM_LIMBO )->area )
	{
	    char buf [ MSL ];
	    CHAR_DATA *pch;

	    if ( *pArea->reset_msg )
		sprintf( buf, "%s\n\r", pArea->reset_msg );
	    else
		strcpy( buf, "S`lyszysz kroczki malutkich st`op.\n\r" );

	    for ( pch = char_list; pch; pch = pch->next )
	    {
		if ( !IS_NPC( pch )
		    && IS_AWAKE( pch )
		    && pch->in_room
		    && pch->in_room->area == pArea )
		{
		    send_to_char( buf, pch );
		}
	    }
	}
	*/

	/*
	 * Check age and reset.
	 * Note: Mud School resets every 3 minutes (not 15).
	 *
	 * Uwaga! Zmienilem to na 2 minuty. Zastanawiam sie, czy nie napisac
	 * do szkoly progow, ktore by mpmloadowaly nastepnego potwora gdy tylko
	 * jednego ktos zabije. Przy kilku rozpoczynajacych gre rownoczesnie
	 * graczach, to by im pomoglo.
	 * -Lam
	 *
	 * Lam 20.5.2002: wszystkie trzy szkoly tak dzialaja.
	 */
	if ( pArea->nplayer == 0 || pArea->age >= 15 )
	{
	    ZONE_DATA *strefa;

	    reset_area( pArea );
	    pArea->odnowiona = TRUE;
	    pArea->age = number_range( 0, 3 );
	    for ( strefa = zone_first; strefa; strefa = strefa->next )
		if ( pArea == get_room_index( strefa->szkola )->area )
		    pArea->age = 15 - 2;
	}
    }

    return;
}


/*
 * Zamiast wykrywacza w reset_area.
 * Dzieki wywolaniu juz po odnowieniu calego muda, zapewniamy, ze nie jest
 * blednie zglaszana sytuacja, kiedy to mob nie ma widzenia w ciemnosci, ale
 * jednak stoi w towarzystwie stacjonarnego moba ze swiatlem.
 */
void sprawdz_ociemniale_moby( void )
{
    CHAR_DATA *mob;

    for ( mob = char_list; mob; mob = mob->next )
    {
	if ( mob->in_room
	  && room_is_dark( mob->in_room )
	  && !HAS_INFRAVISION( mob )
	  && mob->level > 10 ) /* na niskich poziomach slabe moby i zwierzeta */
	{
	    sprintf( bug_buf, "Mob %d (%s [%d], rasa %s) odnawiany po ciemku w %d (%s), a bez widzenia w ciemnosci.",
		mob->pIndexData->vnum, mob->short_descr, mob->level,
		race_table[ mob->race ].who_name, mob->in_room->vnum,
		mob->in_room->area->file_name );
	    cbug( bug_buf, 0 );
	}
    }

    return;
}


/*
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
    CHAR_DATA *mob = NULL;
    OBJ_DATA *obj = NULL;
    RESET_DATA *pReset;
    OBJ_DATA *obj_to;
    EXIT_DATA *pexit, *pexit_rev;
    MOB_INDEX_DATA *pMobIndex;
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *pRoomIndex;
    int level = -1;
    bool last = FALSE;
    int	kierunek, i, ile;

    for ( pReset = pArea->reset_first; pReset; pReset = pReset->next )
    {

	switch ( pReset->command )
	{
	default:
	    cbug( "Reset_area: bad command %c.", pReset->command );
	    break;

	case '*':
	    continue;

	case 'M':
	    if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
	    {
		cbug( "Reset_area: 'M': bad mob vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
		cbug( "Reset_area: 'M': bad room vnum %d.", pReset->arg3 );
		continue;
	    }

	    level = URANGE( 0, pMobIndex->level - 2, LEVEL_HERO );

	    /* wczesniej to bylo przed ustawieniem "level", co moglo wplynac
	       na poziom przedmiotow odnawianych resetem "O" po danym "M" */
	    if ( ( pMobIndex->spec_fun == spec_cast_ghost
		  && ( pRoomIndex->area->weather_info.sunlight != SUN_DARK ) ) )
	    {
		last = FALSE;
		mob = NULL;
		break;
	    }

	    /*
	     * Decyzja, czy odnowic moba. Jest dosc skomplikowana, wiec omowie.
	     * 1. Moby stacjonarne, ktore odnawia tylko 1 reset w calym swiecie
	     *    sa uznawane za imienne i wyjatkowe i nie pozwalaja sie mnozyc
	     *    poprzez wyciaganie. Tutaj zalicza sie np. burmistrz
	     *    Midgaardu, ktory wczesniej byl obslugiwany oddzielnie.
	     * 2. Moby stacjonarne, ktore maja wiecej niz 1 reset i albo
	     *    niczego nie sprzedaja, albo nie wracaja do domu, dzialaja
	     *    po staremu - typowo nie mnoza sie, ale jesli jednego ktos
	     *    oczaruje i wyprowadzi, w jego miejsce pojawi sie kolejny,
	     *    chocby tamten mial wrocic.
	     * 3. Stacjonarni sklepikarze zawsze potrafia wrocic do domu,
	     *    chocby nie wiadomo gdzie ich nie wyciagnieto (pomagaja im
	     *    w tym bogowie), a ze jest ich mniej niz mobow z punktu 2,
	     *    zwlaszcza, ze wiekszosc obejmuje punkt 1, pozwalam sobie na
	     *    szukanie zgubionego sklepikarza w calym mudzie, jesli nie
	     *    ma go w pomieszczeniu. Czyli stacjonarni sprzedawcy zawsze
	     *    w mudzie istnieja w liczbie okreslonej resetem, nigdy nie
	     *    pozwalajac sie klonowac.
	     * 4. Moby ruchome maja resety wymagajace liczby w calym swiecie
	     *    i tu akurat rozwiazanie jest oczywiste.
	     */
	    if ( IS_SET( pMobIndex->act, ACT_SENTINEL )
	      && pMobIndex->resetcount == 1 )
	    {
		if ( pMobIndex->count >= pReset->arg2 )
		{
		    last = FALSE;
		    mob = NULL;
		    break;
		}
	    }
	    else if ( IS_SET( pMobIndex->act, ACT_SENTINEL )
	      && ( ( !pMobIndex->pShop
		  && !pMobIndex->pHealer )
		|| IS_SET( pMobIndex->act, ACT_DONT_COME_BACK ) ) )
	    {
		CHAR_DATA *ch;
		int        count = 0;

		for ( ch = pRoomIndex->people; ch; ch = ch->next_in_room )
		    if ( IS_NPC( ch ) && ch->pIndexData == pMobIndex )
			count++;
		if ( count >= pReset->arg2 )
		{
		    last = FALSE;
		    mob = NULL;
		    break;
		}
	    }
	    /* powyzej byl pkt 2, czyli stacjonarni nie-sprzedawcy.
	       Tutaj stacjonarni to juz tylko sprzedawcy: */
	    else if ( IS_SET( pMobIndex->act, ACT_SENTINEL ) )
	    {
		CHAR_DATA *ch;
		int        count = 0;

		/* najpierw test w pomieszczeniu, jesli sklepikarzy jest tylu,
		   co potrzeba, nie ma potrzeby dalszych dzialan */
		for ( ch = pRoomIndex->people; ch; ch = ch->next_in_room )
		    if ( IS_NPC( ch ) && ch->pIndexData == pMobIndex )
			count++;
		if ( count >= pReset->arg2 )
		{
		    last = FALSE;
		    mob = NULL;
		    break;
		}

		/* szukamy ilosci postaci w swiecie, ktore byly resetowane w tym
		   pomieszczeniu, w ktorym dany sklepikarz powinien byc
		   resetowany, gdyby go nie bylo */
		count = 0;
		for ( ch = char_list; ch; ch = ch->next )
		    if ( IS_NPC( ch )
		      && ch->pIndexData == pMobIndex
		      && ch->reset_room == pRoomIndex )
			count++;
		if ( count >= pReset->arg2 )
		{
		    last = FALSE;
		    mob = NULL;
		    break;
		}
	    }
	    else
		if ( pMobIndex->count >= pReset->arg2 )
		{
		    last = FALSE;
		    mob = NULL;
		    break;
		}

	    mob = create_mobile( pMobIndex );

	    /* Lam 1.4.2002: Sprawdzanie mieszkancow stolic stref */
	    if ( ODPLUSKWIACZ )
	    {
		ZONE_DATA *strefa;
		char param[ MSL ];
		char plum[ MSL ];
		char pidum[ MIL ];
		char *r;

		for ( strefa = zone_first; strefa; strefa = strefa->next )
		{
		    if ( pRoomIndex->area != get_room_index( strefa->przywolanie )->area )
			continue;

		    strcpy( plum, strefa->rasy );
		    for ( r = one_argument( plum, pidum ); *pidum; r = one_argument( r, pidum ) )
		    {
			if ( is_name2( pidum, race_table[ mob->race ].hate ) )
			{
			    sprintf( param, "Reset_area: mob #%d rasy %s, resetowany w #%d, nienawidzi rasy %s ze strefy %s.",
				mob->pIndexData->vnum, race_table[ mob->race ].ang,
				pRoomIndex->vnum, pidum, strefa->nazwa );
			    cbug( param, 0 );
			}
		    }
		}
	    }

	    /*
	     * Check for pet shop.
	     */
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;

		pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
		if ( pRoomIndexPrev
		    && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
		    SET_BIT( mob->act, ACT_PET );
	    }

	    char_to_room( mob, pRoomIndex );
	    /* Lam 9.1.2003: zapamietywanie miejsca resetu */
	    mob->reset_room = pRoomIndex;

	    /* Lam 23.5.2007: symulacja nadchodzenia mobow. Petla losuje jedno
	       z mozliwych wejsc, ktorym mogl wejsc mob. Oczywiscie moze
	       zdarzyc sie, ze wylosuje zamkniete pomieszczenie, w ktorym przed
	       chwila moba nie bylo. Najlepiej byloby wiec oznaczyc w krainie
	       miejsce, z ktorego rozchodza sie do "domow" moby i puszczac
	       "polowanie" (jednak modyfikujac, aby uwzglednialo can_move). */
	    if ( pArea->nplayer )
	    {
		ile = 0;
		kierunek = 10;
		for ( i = 0; i < 10; i++ )
		{
		    if ( ( pexit = pRoomIndex->exit[ i ] )
		      && pexit->to_room
		      && ( pexit_rev = pexit->to_room->exit[ kierunki[ i ].odwrotny ] )
		      && pexit_rev->to_room == pRoomIndex
		      && ( !IS_SET( pexit_rev->exit_info, EX_CLOSED )
			|| ( IS_PASSING( mob )
			  && !IS_SET( pexit_rev->exit_info, EX_PASSPROOF ) ) )
		      && can_move( mob, pexit->to_room, pRoomIndex, kierunki[ i ].odwrotny, FALSE ) )
		    {
			if ( number_range( 0, ile ) == 0 )
			    kierunek = i;
			ile++;
		    }
		}

		if ( kierunek < 10 )
		    wchodzenie_do_pomieszczenia( mob,
			pRoomIndex->exit[ kierunek ]->to_room, pRoomIndex,
			kierunki[ kierunek ].odwrotny, TRUE );
		else
		    act( ZM_WZROK | ZM_WID_CHAR, "Znienacka pojawia si`e tu $n.",
			mob, NULL, NULL, TO_ROOM );
	    }

	    /* Lam 25.2.2006 */
	    if ( ODPLUSKWIACZ )
	    {
		if ( airchar_list
		  && airchar_list->chr == mob )
		{
		    sprintf( bug_buf, "Mob %d odnawiany w pomieszczeniu %d natychmiast spada.",
			pMobIndex->vnum, pRoomIndex->vnum );
		    cbug( bug_buf, 0 );
		}
		if ( CZY_BRAK_TLENU( mob ) )
		{
		    sprintf( bug_buf, "Mob %d odnawiany w pomieszczeniu %d od razu sie dusi.",
			pMobIndex->vnum, pRoomIndex->vnum );
		    cbug( bug_buf, 0 );
		}
		/* wersja CZY_PALACE_SLONCE bez testu pozycji slonca */
		if ( CZY_WAMPIR( mob )
		  && !IS_SET( mob->in_room->room_flags, ROOM_UNDERGROUND )
		  && mob->in_room->sector_type != SECT_UNDERWATER )
		{
		    sprintf( bug_buf, "Mob %d odnawiany w pomieszczeniu %d jest za dnia palony przez slonce.",
			pMobIndex->vnum, pRoomIndex->vnum );
		    cbug( bug_buf, 0 );
		}
		/* na zyczenie Skifra */
		if ( IS_SET( pRoomIndex->room_flags, ROOM_NO_MOB )
		  && !IS_SET( mob->act, ACT_SENTINEL ) )
		{
		    sprintf( bug_buf, "Ruchomy mob %d odnawiany w pomieszczeniu bez_moba %d.",
			pMobIndex->vnum, pRoomIndex->vnum );
		    cbug( bug_buf, 0 );
		}
		/* Lam 22.5.2006 */
		if ( ( !IS_SET( mob->act, ACT_SENTINEL )
		    || IS_SET( mob->act, ACT_WIMPY ) )
		  && !can_move( mob, pRoomIndex, pRoomIndex, DIR_NORTH, FALSE ) )
		{
		    sprintf( bug_buf, "Ruchomy/tchorzliwy mob %d odnawiany w pomieszczeniu %d nie moze sie ruszac.",
			pMobIndex->vnum, pRoomIndex->vnum );
		    cbug( bug_buf, 0 );
		}
	    }

	    level = URANGE( 0, mob->level - 2, LEVEL_HERO );
	    last = TRUE;
	    obj = NULL;
	    break;

	case 'O':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
		cbug( "Reset_area: 'O': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( pReset->arg2 < 0 )
	    {
		sprintf( bug_buf, "Reset_area: %s: 'O' (%d w %d) bez 'M'.",
			pArea->file_name, pReset->arg1, pReset->arg3 );
		cbug( bug_buf, 0 );
		continue;
	    }

	    if ( ODPLUSKWIACZ
	      && IS_SET_V( pObjIndex->extra_flags, ITEM_QUEST )
	      && !IS_SET_V( pObjIndex->extra_flags, ITEM_BURIED ) )
	    {
		sprintf( bug_buf, "Reset_area: 'O': Przedmiot zadaniowy %d jest restartowany w pomieszczeniu %d.",
			pObjIndex->vnum, pReset->arg3 );
		cbug( bug_buf, 0 );
	    }

	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
		cbug( "Reset_area: 'O': bad vnum %d.", pReset->arg3 );
		continue;
	    }

	    if ( pArea->nplayer > 0 )
	    {
		last = FALSE;
		obj = NULL;
		break;
	    }

	    /* Lam 25.12.2003: przedmioty, ktorych nie mozna zabrac z ziemi
		udaja, ze sie wlasnie zresetowaly (dla nastepujacych resetow
		'P') */
	    if ( count_obj_list( pObjIndex, pRoomIndex->contents ) > 0 )
	    {
		if ( !IS_SET( pObjIndex->wear_flags, ITEM_TAKE ) )
		{
		    OBJ_DATA *tmp_obj;

		    last = FALSE;
		    obj = NULL;
		    for ( tmp_obj = pRoomIndex->contents; tmp_obj; tmp_obj = tmp_obj->next_content )
		    {
			if ( tmp_obj->deleted )
			    continue;
			if ( tmp_obj->pIndexData == pObjIndex )
			{
			    last = TRUE;
			    obj = tmp_obj;
			    mob = NULL;
			    break;
			}
		    }
		    break;
		}
		else
		{
		    last = FALSE;
		    obj = NULL;
		    break;
		}
	    }

	    if ( TylkoKonwersja ) /* minimalny poziom dla lac -k -s */
		obj = create_object( pObjIndex, UMAX( 1, pReset->arg2 - 1 ) );
	    else
		obj = create_object( pObjIndex, number_fuzzy( pReset->arg2 ) );

	    obj_to_room( obj, pRoomIndex );
	    last = TRUE;
	    mob = NULL;
	    break;

	case 'P':
	    /* Lam 3.6.99: calkowicie zmienilem, rezygnujac z kompatybinosci
	       z istniejacymi krainami (np. haon.are) */
	    if ( !last )
		break;

	    if ( !obj )
	    {
		char buf [ MIL ];

		sprintf( buf, "Reset_area (%s): 'P': brak przedmiotu, w ktory mozna wlozyc przedmiot %d.",
				pArea->file_name, pReset->arg1 );
		cbug( buf, 0 );
		/* last = FALSE; */
		break;
	    }
	    else
	    {
		obj_to = obj;

		if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
		{
		    cbug( "Reset_area: 'P': bad vnum %d.", pReset->arg1 );
		    continue;
		}

		if ( ODPLUSKWIACZ
		  && IS_SET_V( pObjIndex->extra_flags, ITEM_QUEST ) )
		{
		    sprintf( bug_buf, "Reset_area: 'P': Przedmiot zadaniowy %d jest restartowany w przedmiocie.", pObjIndex->vnum );
		    cbug( bug_buf, 0 );
		}

		if ( count_obj_list( pObjIndex, obj_to->contains ) > 0 )
		{
		    /* zmiany last by byly, gdyby bylo wkladanie jednego
		       przedmiotu w drugi
		    last = FALSE; */
		    break;
		}

		if ( TylkoKonwersja ) /* minimalny poziom dla lac -k -s */
		    obj = create_object( pObjIndex, UMAX( 1, obj_to->level - 1 ) );
		else
		    obj = create_object( pObjIndex, number_fuzzy( obj_to->level ) );
		obj_to_obj( obj, obj_to );
		obj = obj_to; /* mozna wlozyc dwa przedmioty w jeden, ale
				 nie mozna wlozyc przedmiotu w przedmiot,
				 ktory jest juz w przedmiocie */

		/* Sprawdzam tylko dla pierwszego przebiegu, bo potem gracz
		   moze zapchac pojemnik. Poki co zapchanie pojemnika przez
		   gracza nie zapobiega odnawianiu sie w nim przedmiotow */
		if ( ODPLUSKWIACZ
		  && !pArea->odnowiona
		  && get_obj_weight( obj ) - obj->weight > obj->value[ 0 ].v )
		{
		    sprintf( bug_buf, "Reset_area: 'P': Wlozenie przedmiotu %d przepelnilo pojemnik %d.", pReset->arg1, obj->pIndexData->vnum );
		    cbug( bug_buf, 0 );
		}

		/* jw.
		last = TRUE; */
	    }
	    break;

	case 'G':
	case 'E':
	    if ( !last )
		break;

	    if ( !mob )
	    {
		cbug( "Reset_area: 'E' or 'G': null mob for vnum %d.",
		    pReset->arg1 );
		last = FALSE;
		obj = NULL;
		break;
	    }

	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
		cbug( "Reset_area: 'E' or 'G': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ODPLUSKWIACZ
	      && IS_SET_V( pObjIndex->extra_flags, ITEM_QUEST ) )
	    {
		sprintf( bug_buf, "Reset_area: '%c': Przedmiot zadaniowy %d jest restartowany u moba.", pReset->command, pObjIndex->vnum );
		cbug( bug_buf, 0 );
	    }

	    /* Lam 29.8.98: w sklepie tylko inwentarz ma niskie poziomy */
	    if ( mob->pIndexData->pShop && pReset->command == 'G' )
	    {
		int olevel;

		pObjIndex->w_sklepie = TRUE;

		if ( TylkoKonwersja ) /* minimalny poziom dla lac -k -s */
		{
		    switch ( pObjIndex->item_type )
		    {
			default:		olevel =  0; break;
			case ITEM_PILL:		olevel =  0; break;
			case ITEM_POTION:	olevel =  0; break;
			case ITEM_SCROLL:	olevel =  5; break;
			case ITEM_WAND:		olevel = 10; break;
			case ITEM_SPELLBOOK:	olevel = 20; break;
			case ITEM_STAFF:	olevel = 15; break;
			case ITEM_ARMOR:	olevel =  5; break;
			case ITEM_FURNITURE:	olevel =  5; break;
			case ITEM_WEAPON:	olevel =  5; break;
		    }
		}
		else
		{
		    switch ( pObjIndex->item_type )
		    {
			default:		olevel = 0;                      break;
			case ITEM_PILL:		olevel = number_range(  0, 10 ); break;
			case ITEM_POTION:	olevel = number_range(  0, 10 ); break;
			case ITEM_SCROLL:	olevel = number_range(  5, 15 ); break;
			case ITEM_WAND:		olevel = number_range( 10, 20 ); break;
			case ITEM_SPELLBOOK:	olevel = number_range( 20, 30 ); break;
			case ITEM_STAFF:	olevel = number_range( 15, 25 ); break;
			case ITEM_ARMOR:	olevel = number_range(  5, 15 ); break;
			case ITEM_FURNITURE:	olevel = number_range(  5, 15 ); break;
			case ITEM_WEAPON:	olevel = number_range(  5, 15 ); break;
		    }
		}
		if ( pObjIndex->poziom )
		    olevel = pObjIndex->poziom;
		obj = create_object( pObjIndex, olevel );
		SET_BIT_V( obj->extra_flags, ITEM_INVENTORY );
		if ( pObjIndex->cena )
		    obj->cost = pObjIndex->cena;
		if ( pObjIndex->qp )
		    mob->pIndexData->pShop->quest_shop = TRUE;
	    }
	    else
	    {
		if ( TylkoKonwersja ) /* minimalny poziom dla lac -k -s */
		    obj = create_object( pObjIndex, UMAX( 1, level - 1 ) );
		else
		    obj = create_object( pObjIndex, number_fuzzy( level ) );
	    }

	    if ( IS_SET_V( obj->extra_flags, ITEM_BURIED ) )
	    {
		cbug( "reset_area: 'E' albo 'G': musze usunac flage ITEM_BURIED z przedmiotu", 0 );
		REMOVE_BIT_V( obj->extra_flags, ITEM_BURIED );
	    }

	    obj_to_char( obj, mob );

	    /* Lam: sztucznie zwiekszam sile, zeby uniosl wlasny inwentarz, ale
	       nie ponad maksimum rasy */
	    while ( mob->carry_weight > can_carry_w( mob )
		 && get_curr_str( mob ) < UMIN( 23 + race_table[ mob->race ].str_mod, 26 ) )
		mob->mod_str++;
	    if ( ODPLUSKWIACZ && mob->carry_weight > can_carry_w( mob ) )
		cbug( "reset_area: 'E': mob %d nie uniesie wlasnego inwentarza!", mob->pIndexData->vnum );

	    if ( TylkoKonwersja && ODPLUSKWIACZ
	      && pReset->command == 'E'
	      && pReset->arg3 == WEAR_LIGHT
	      && !IS_SET( mob->act, ACT_SENTINEL )
	      && obj->value[ 2 ].v != 0 )
	    {
		/* Tylko konwerter, bo i tak nie popsuje swiatla.
		   Tylko ruchomy, bo stacjonarny jest stalym zrodlem swiatla.
		   Swiatlo zostaje mu w inwentarzu, a on sam dostaje widzenie
		   w ciemnosci, zeby nie zglosilo go jako niewidomego w nocy. */
		SET_BIT_V( mob->affected_by, AFF_INFRARED );
	    }
	    else if ( pReset->command == 'E' )
	    {
		equip_char( mob, obj, pReset->arg3 );
		if ( ODPLUSKWIACZ )
		    check_race_wearlocs( mob );
		if ( !obj->carried_by )
		    cbug( "reset_area: 'E': przedmiot %d spadl z moba!", pReset->arg1 );
		else if ( get_eq_char( mob, pReset->arg3 ) != obj )
		{
		    sprintf( bug_buf, "reset_area: 'E': [%s] przedmiot %d nie dal sie zalozyc na moba %d w miejscu %d",
			pArea->file_name, pReset->arg1,
			mob->pIndexData->vnum, pReset->arg3 );
		    cbug( bug_buf, 0 );
		}

		/* Lam 1.1.2008: Sediius zauwazyl, ze moby noszace sprzet
		   wplywajacy na zycie odnawiaja sie z np. 65% zycia */
		mob->hit = UMAX( mob->max_hit, mob->hit );
		mob->mana = UMAX( mob->max_mana, mob->mana );
		mob->move = UMAX( mob->max_move, mob->move );

		if ( pReset->arg3 == WEAR_WIELD
		  || pReset->arg3 == WEAR_WIELD_2 )
		{
		    OBJ_DATA *bron;
		    int weight = 0;
		    if ( ( bron = get_eq_char( mob, WEAR_WIELD ) ) )
			weight += get_obj_weight( bron );
		    if ( ( bron = get_eq_char( mob, WEAR_WIELD_2 ) ) )
			weight += get_obj_weight( bron );
		    /* Lam: sztucznie zwiekszam sile, zeby uniosl wlasna bron,
		       ale nie ponad maksimum rasy */
		    while ( weight > str_app[ get_curr_str( mob ) ].wield
			 && get_curr_str( mob ) < UMIN( 23 + race_table[ mob->race ].str_mod, 26 ) )
			mob->mod_str++;
		    if ( ODPLUSKWIACZ
		      && weight > str_app[ get_curr_str( mob ) ].wield )
			cbug( "reset_area: 'E': mob %d nie uniesie broni, ktora mu wlozono w rece!", mob->pIndexData->vnum );
		}

		if ( ODPLUSKWIACZ )
		{
		    int ile_rak = 0;
		    if ( get_eq_char( mob, WEAR_LIGHT ) )
			ile_rak++;
		    if ( get_eq_char( mob, WEAR_HOLD ) )
			ile_rak++;
		    if ( get_eq_char( mob, WEAR_WIELD_2 ) )
		    {
			ile_rak++;
			if ( get_eq_char( mob, WEAR_SHIELD ) )
			    cbug( "reset_area: 'E': uwaga cyrkowiec! Mob %d uzywa tarczy wraz z druga bronia!", mob->pIndexData->vnum );
		    }
		    if ( ile_rak > 1 )
			cbug( "reset_area: 'E': uwaga mutant! Mob %d ma zbyt wiele rak (swiatlo/druga bron/trzymane)!", mob->pIndexData->vnum );
		    /* zakladane, a nie brane w dlonie */
		    if ( pReset->arg3 != WEAR_LIGHT
		      && pReset->arg3 != WEAR_HOLD
		      && pReset->arg3 != WEAR_WIELD
		      && pReset->arg3 != WEAR_WIELD_2 )
		    {
			/* Niestety, sa rasy z wielkoscia 0. Tego fragmentu
			   uzywalem do konwersji, mozna to odblokowac do
			   jakiegos konwertowania krain albo kierowac sie
			   ostrzezeniami ponizszego testu --Lam */
			if ( pObjIndex->wielkosc == 0 )
			{
			    pObjIndex->wielkosc = race_table[ mob->race ].size;
			    sprintf( bug_buf, "reset_area: 'E': przedmiot %d bez wielkosci, ustawiam %d (zeby pasowalo na moba %d)",
				pObjIndex->vnum, pObjIndex->wielkosc,
				mob->pIndexData->vnum );
			    cbug( bug_buf, 0 );
			}
			if ( pObjIndex->wielkosc < race_table[ mob->race ].size - 1
			  || pObjIndex->wielkosc > race_table[ mob->race ].size + 1 )
			{
			    sprintf( bug_buf, "reset_area: 'E': przedmiot %d jest zlej wielkosci (%d) dla moba %d (wielkosci %d)",
				pObjIndex->vnum, pObjIndex->wielkosc,
				mob->pIndexData->vnum, race_table[ mob->race ].size );
			    cbug( bug_buf, 0 );
			}
		    }
		}
	    }
	    last = TRUE;
	    break;

	case 'D':
	    if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
	    {
		cbug( "Reset_area: 'D': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( pReset->arg2 < 0
	      || pReset->arg2 > 9
	      || !( pexit = pRoomIndex->exit[ pReset->arg2 ] ) )
	    {
		sprintf( log_buf, "Reset_area: 'D': %d:%d - brak drzwi w tym kierunku.", pReset->arg1, pReset->arg2 );
		cbug( log_buf, 0 );
		break;
	    }

	    /* Bashed doors persist across resets */
	    if ( !IS_SET( pexit->exit_info, EX_BASHED ) )
	    {
		if ( ODPLUSKWIACZ && !IS_SET( pexit->exit_info, EX_ISDOOR ) )
		{
		    sprintf( log_buf, "Reset_area: 'D': %d:%d - zamykanie wyjscia nie bedacego nawet drzwiami!", pReset->arg1, pReset->arg2 );
		    cbug( log_buf, 0 );
		}
		switch ( pReset->arg3 )
		{
		case 0:
		    REMOVE_BIT( pexit->exit_info, EX_CLOSED );
		    REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		    REMOVE_BIT( pexit->exit_info, EX_BURIED );
		    break;

		case 1:
		    SET_BIT( pexit->exit_info, EX_CLOSED );
		    REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		    REMOVE_BIT( pexit->exit_info, EX_BURIED );
		    break;

		case 2:
		    SET_BIT( pexit->exit_info, EX_CLOSED );
		    SET_BIT( pexit->exit_info, EX_LOCKED );
		    REMOVE_BIT( pexit->exit_info, EX_BURIED );
		    if ( ODPLUSKWIACZ
		      && ( pexit->key <= 0 || !get_obj_index( pexit->key ) ) )
		    {
			sprintf( log_buf, "Reset_area: 'D': %d:%d - zakluczanie wyjscia z nieistniejacym kluczem %d", pReset->arg1, pReset->arg2, pexit->key );
			cbug( log_buf, 0 );
		    }
		    break;
		case 3:
		    SET_BIT( pexit->exit_info, EX_CLOSED );
		    REMOVE_BIT(	pexit->exit_info, EX_LOCKED );
		    if ( pReset->arg2 == DIR_DOWN )
			SET_BIT( pexit->exit_info, EX_BURIED );
		    break;
		case 4:
		    SET_BIT( pexit->exit_info, EX_CLOSED );
		    SET_BIT( pexit->exit_info, EX_LOCKED );
		    if ( ODPLUSKWIACZ
		      && ( pexit->key <= 0 || !get_obj_index( pexit->key ) ) )
		    {
			sprintf( log_buf, "Reset_area: 'D': %d:%d - zakluczanie wyjscia z nieistniejacym kluczem %d", pReset->arg1, pReset->arg2, pexit->key );
			cbug( log_buf, 0 );
		    }
		    if ( pReset->arg2 == DIR_DOWN )
			SET_BIT( pexit->exit_info, EX_BURIED );
		    break;
		}

		if ( ODPLUSKWIACZ && pexit->to_room )
		{
		    RESET_DATA *pTmpReset;
		    bool czy = FALSE;
		    int stan1, stan2;

		    /* zmiany stanow po to, by wyjscia zakopane z jednej strony
		       nie byly zglaszane jako bledy */
		    stan1 = pReset->arg3 > 2 ? pReset->arg3 - 2 : pReset->arg3;
		    for ( pTmpReset = pArea->reset_first; pTmpReset; pTmpReset = pTmpReset->next )
		    {
			stan2 = pTmpReset->arg3 > 2 ? pTmpReset->arg3 - 2 : pTmpReset->arg3;
			if ( pTmpReset->command == 'D'
			  && pTmpReset->arg1 == pexit->to_room->vnum
			  && pTmpReset->arg2 == kierunki[ pReset->arg2 ].odwrotny )
			{
			    czy = TRUE;
			    if ( stan1 != stan2 )
			    {
				sprintf( log_buf, "Reset_area: 'D': %d:%d stan %d != (odwrotny reset 'D') %d:%d stan %d",
					pReset->arg1, pReset->arg2, pReset->arg3,
					pTmpReset->arg1, pTmpReset->arg2, pTmpReset->arg3 );
				cbug( log_buf, 0 );
			    }
			    break;
			}
		    }
		    if ( !czy )
		    {
			sprintf( log_buf, "Reset_area: 'D': %d:%d - nie moge znalezc resetu drzwi po drugiej stronie!", pReset->arg1, pReset->arg2 );
			cbug( log_buf, 0 );
		    }
		}
	    }

	    break;

	case 'R':
	    if ( TylkoKonwersja )
		continue;

	    if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
	    {
		cbug( "Reset_area: 'R': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    {
		int d0;
		int d1;

		for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
		{
		    d1 = number_range( d0, pReset->arg2-1 );
		    pexit = pRoomIndex->exit[ d0 ];
		    pRoomIndex->exit[ d0 ] = pRoomIndex->exit[ d1 ];
		    pRoomIndex->exit[ d1 ] = pexit;
		}
	    }
	    break;
	}

	if ( ODPLUSKWIACZ
	  && !pArea->odnowiona
	  && !last
	  && pReset->command != 'R' && pReset->command != 'D' )
	{
	    sprintf( bug_buf, "Nieudane polecenie odnowienia! plik %s, %c 0 %-5d %-3d %-5d",
		pArea->file_name, pReset->command,
		pReset->arg1, pReset->arg2, pReset->arg3 );
	    cbug( bug_buf, 0 );
	}
    }

    return;
}


/*
 * Take a character data from the free list and clean it out.
 */
CHAR_DATA *new_character( bool player )
{
    static PC_DATA    pcdata_zero;
	   CHAR_DATA *ch;

    if ( !first_free_char )
    {
	ch		= alloc_perm( sizeof( CHAR_DATA ) );
	top_mob++;
    }
    else
    {
	ch		= first_free_char;
	first_free_char = first_free_char->next;
	if ( !first_free_char )
	    last_free_char = NULL;
    }

    clear_char( ch );

    if ( player )
    {
	if ( !pcdata_free )
	{
	    ch->pcdata = alloc_perm( sizeof( *ch->pcdata ) );
	    top_pcdata++;
	}
	else
	{
	    ch->pcdata = pcdata_free;
	    pcdata_free = pcdata_free->next;
	}

	*ch->pcdata = pcdata_zero;
    }

    ch->deleted = FALSE;

    return ch;
}


SHOP_DATA *new_shop( void )
{
    ++top_shop;

    return alloc_mem( sizeof( SHOP_DATA ) );
}


HEALER_DATA *new_healer( void )
{
    ++top_healer;

    return alloc_mem( sizeof( HEALER_DATA ) );
}


ALIAS_DATA *new_alias( )
{
    ALIAS_DATA *alias;

    if ( !alias_free )
    {
	++top_alias;
	return alloc_perm( sizeof( ALIAS_DATA ) );
    }

    alias = alias_free;
    alias_free = alias_free->next;

    return alias;
}


void del_alias( CHAR_DATA *ch, ALIAS_DATA *alias )
{
    ALIAS_DATA *temp;

    if ( !alias )
    {
	bug ( "Del_alias: NULL alias! return...", 0 );
	return;
    }

    free_string( alias->name );
    free_string( alias->todo );

    if ( ch->alias == alias )
    {
	ch->alias = alias->next;
	alias->next = alias_free;
	alias_free = alias;
	return;
    }

    for ( temp = ch->alias; temp; temp = temp->next )
    {
	if ( temp->next == alias )
	{
	    temp->next=alias->next;
	    alias->next = alias_free;
	    alias_free = alias;
	    return;
	}
    }

    bug( "del_alias :( - widocznie nie znalazl aliasu u postaci... db.c", 0 );

    return;
}


/* Gimza: dwie funkcje obslugujace komende odmow - skopiowane z aliasow */
REFUSE_DATA *new_refuse( )
{
    REFUSE_DATA *refuse;

    if ( !refuse_free )
    {
	++top_refuse;
	return alloc_perm( sizeof( REFUSE_DATA ) );
    }

    refuse = refuse_free;
    refuse_free = refuse_free->next;

    return refuse;
}


void del_refuse( CHAR_DATA *ch, REFUSE_DATA *refuse )
{
    REFUSE_DATA *temp;

    if ( !refuse )
    {
	bug ( "Del_refuse: NULL refuse! return...", 0 );
	return;
    }

    refuse->refused_cmd[ 0 ] = '\0';
    refuse->who[ 0 ] = '\0';
    refuse->why[ 0 ] = '\0';
    refuse->date[ 0 ] = '\0';

    if ( ch->pcdata->refuse == refuse )
    {
	ch->pcdata->refuse = refuse->next;
	refuse->next = refuse_free;
	refuse_free = refuse;
	return;
    }

    for ( temp = ch->pcdata->refuse; temp; temp = temp->next )
	if ( temp->next == refuse )
	{
	    temp->next = refuse->next;
	    refuse->next = refuse_free;
	    refuse_free = refuse;
	    return;
	}

    bug( "del_refuse :( - widocznie nie znalazl refuse u postaci... db.c", 0 );

    return;
}


TELL_DATA *new_tell( void )
{
    TELL_DATA *tell;

    if ( !tell_free )
    {
	++top_tell;
	return alloc_perm( sizeof( TELL_DATA ) );
    }

    tell = tell_free;
    tell_free = tell_free->next;

    return tell;
}


void del_tell( CHAR_DATA *ch, TELL_DATA *tell )
{
    TELL_DATA * temp;

    if ( !tell )
    {
	bug ( "Del_tell: NULL tell!", 0 );
	return;
    }

    free_string( tell->tell );

    if ( ch->tell == tell )
    {
	ch->tell = tell->next;
	tell->next = tell_free;
	tell_free = tell;
	return;
    }

    for ( temp = ch->tell; temp; temp = temp->next )
    {
	if ( temp->next == tell )
	{
	    temp->next = tell->next;
	    tell->next = tell_free;
	    tell_free = tell;
	    return;
	}
    }

    bug( "del_tell - tell not found", 0 );

    return;
}


MPQUEST_DATA *new_mpquest( void )
{
    MPQUEST_DATA *mpquest;
    int i;

    if ( !mpquest_free )
    {
	++top_mpquest;
	mpquest = alloc_perm( sizeof( MPQUEST_DATA ) );
    }
    else
    {
	mpquest = mpquest_free;
	mpquest_free = mpquest_free->next;
    }

    mpquest->vnum = 0;
    mpquest->stage = 0;
    for ( i = 0; i < 5; i++ )
	mpquest->data[ i ] = 0;
    mpquest->title = str_dup( "" );
    mpquest->descr = str_dup( "" );

    return mpquest;
}


void del_mpquest( CHAR_DATA *ch, MPQUEST_DATA *mpquest )
{
    MPQUEST_DATA *temp;

    if ( !mpquest )
    {
	bug ( "Del_mpquest: NULL mpquest!", 0 );
	return;
    }

    if ( mpquest->title )
	free_string( mpquest->title );
    if ( mpquest->descr )
	free_string( mpquest->descr );

    if ( ch->mpquest == mpquest )
    {
	ch->mpquest = mpquest->next;
	mpquest->next = mpquest_free;
	mpquest_free = mpquest;
	return;
    }

    for ( temp = ch->mpquest; temp; temp = temp->next )
    {
	if ( temp->next == mpquest )
	{
	    temp->next = mpquest->next;
	    mpquest->next = mpquest_free;
	    mpquest_free = mpquest;
	    return;
	}
    }

    bug( "del_mpquest - mpquest not found", 0 );

    return;
}


MPQUEST_DATA *find_mpquest( CHAR_DATA *ch, int vnum )
{
    MPQUEST_DATA *temp;

    if ( !ch->mpquest )
	return NULL;

    for ( temp = ch->mpquest; temp; temp = temp->next )
	if ( temp->vnum == vnum )
	    return temp;

    return NULL;
}


AFFECT_DATA *new_affect( )
{
    AFFECT_DATA *paf;

    if ( !affect_free )
    {
	paf = alloc_perm( sizeof( AFFECT_DATA ) );
    }
    else
    {
	paf = affect_free;
	affect_free = affect_free->next;
    }

    CLEAR_BITS( paf->bitvector );
    paf->deleted = FALSE;
    paf->modifier = 0;
    paf->level = 0;
    paf->caster = NULL;
    paf->location = APPLY_NONE;

    return paf;
}


OBJ_DATA *new_object( )
{
    OBJ_DATA *obj;

    if ( !obj_free )
    {
	obj = alloc_perm( sizeof( *obj ) );
	top_obj++;
    }
    else
    {
	obj = obj_free;
	obj_free = obj_free->next;
	if ( obj->in_room )
	{
	    bug( "new_object: object in_room", 0 );
	    obj_from_room( obj );
	}
	if ( obj->carried_by )
	{
	    bug( "new_object: object carried_by", 0 );
	    obj_from_char( obj );
	}
	if ( obj->owned_by )
	{
	    bug( "new_object: object owned_by", 0 );
	    obj_from_przechowalnia( obj );
	}
	if ( obj->in_obj )
	{
	    bug( "new_object: object in_obj", 0 );
	    obj_from_obj( obj );
	}
    }

    memset( obj, 0, sizeof( OBJ_DATA ) );

    return obj;
}


EXTRA_DESCR_DATA *new_extra_descr( )
{
    EXTRA_DESCR_DATA *ed;

    if ( !extra_descr_free )
    {
	top_ed++;
	return alloc_perm( sizeof( EXTRA_DESCR_DATA ) );
    }

    ed = extra_descr_free;
    extra_descr_free = extra_descr_free->next;

    return ed;
}


void del_extra_descr( EXTRA_DESCR_DATA *ed )
{
    if ( !ed )
    {
	bug( "del_extra_descr: NULL ed!", 0 );
	return;
    }

    if ( ed->keyword )
	free_string( ed->keyword );
    if ( ed->description )
	free_string( ed->description );
    ed->deleted = TRUE;

    ed->next = extra_descr_free;
    extra_descr_free = ed;

    return;
}


MPROG_DATA *new_mprog( )
{
    MPROG_DATA *prog;

    if ( !mprog_free )
    {
	top_mprog++;
	return alloc_perm( sizeof( MPROG_DATA ) );
    }

    prog = mprog_free;
    mprog_free = mprog_free->next;

    return prog;
}


void del_mprog( MPROG_DATA *prog )
{
    if ( !prog )
    {
	bug( "del_mprog: NULL prog!", 0 );
	return;
    }

    if ( prog->arglist )
	free_string( prog->arglist );
    if ( prog->comlist )
	free_string( prog->comlist );

    prog->next = mprog_free;
    mprog_free = prog;

    return;
}


/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *mob;
    int poziomica;

    if ( !pMobIndex )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }

    mob                 = new_character( FALSE );

    mob->pIndexData     = pMobIndex;
    mob->area		= pMobIndex->index_area;

    mob->name           = str_dup( pMobIndex->player_name );
    mob->short_descr    = str_dup( pMobIndex->short_descr );
    mob->long_descr     = str_dup( pMobIndex->long_descr  );
    mob->long_descr_orig = str_dup( "" );
    mob->false_name     = str_dup( "" );
    mob->description    = str_dup( pMobIndex->description );
    mob->dopelniacz     = str_dup( pMobIndex->dopelniacz  );
    mob->celownik       = str_dup( pMobIndex->celownik    );
    mob->biernik        = str_dup( pMobIndex->biernik     );
    mob->narzednik      = str_dup( pMobIndex->narzednik   );
    mob->miejscownik    = str_dup( pMobIndex->miejscownik );
    mob->wolacz		= str_dup( pMobIndex->wolacz      );
/* 30.11.2000: dzieki DEBUG_SSM odkrylem powtorke: */
/*    mob->description    = str_dup( pMobIndex->description ); */
    mob->spec_fun       = pMobIndex->spec_fun;

    mob->separator      = '|';
    if ( TylkoKonwersja ) /* ustawi minimalny poziom przedmiotow przy lac -k -s */
	mob->level	= UMAX( 1, pMobIndex->level - 1 );
    else
	mob->level	= number_fuzzy( pMobIndex->level );
    mob->act            = pMobIndex->act;
    COPY_BITS( mob->affected_by, pMobIndex->affected_by );
    mob->alignment      = pMobIndex->alignment;
    mob->sex            = pMobIndex->sex;
    mob->race           = pMobIndex->race;
    poziomica = number_fuzzy( pMobIndex->level );
    mob->gold           = ZYSK( poziomica, number_fuzzy( 10 ) )
			* pMobIndex->zamoznosc / 2; /* pierwotnie / 5 */

    mob->armor          = 0;
    mob->marmor         = number_fuzzy( interpolate( mob->level, 1, 1000 ) );
/*    mob->hitroll	= number_fuzzy( 3 * mob->level / 4 );
    mob->damroll	= number_fuzzy( 3 * mob->level / 4 );*/
    if ( mob->spec_fun
      && !str_prefix( "spec_cast", spec_name( mob->spec_fun ) ) )
	mob->saving_throw += number_fuzzy( mob->level );

    mob->max_hit        = mob->level * 8 + number_range(
				mob->level * mob->level * 3 / 16,
				mob->level * mob->level / 4 );
    mob->hit            = mob->max_hit;

    /* Lam 13.1.2004 */
    if ( pMobIndex->level > 12 )
    {
	mob->mod_str = URANGE( 0, number_fuzzy( pMobIndex->level / 12 ), 10 );
	mob->mod_int = URANGE( 0, number_fuzzy( pMobIndex->level / 12 ), 10 );
	mob->mod_wis = URANGE( 0, number_fuzzy( pMobIndex->level / 12 ), 10 );
	mob->mod_dex = URANGE( 0, number_fuzzy( pMobIndex->level / 12 ), 10 );
	mob->mod_con = URANGE( 0, number_fuzzy( pMobIndex->level / 12 ), 10 );
    }

    /*
     * Insert in list.
     */
    mob->next           = char_list;
    char_list           = mob;
    pMobIndex->count++;

    return mob;
}


/* Lam 29.11.2005: zwrotne odnosniki miedzy postaciami dla extract_char, zeby
   nie lecial po char_list. Zajmuja 250 KiB pamieci, ale dzieki temu Lac jest
   duzo szybszy */
void dod_zwod( ZWOD_DATA **lista, CHAR_DATA *cel )
{
    ZWOD_DATA *zwod;

    /* jeden zwrotny odnosnik dla jednej postaci - w przypadku list takich
       jak lista zmiennych, wplywow lub walk, extract_char musi sprawdzic cala
       dla danej postaci, a nie szukac jednego wystapienia dla jednego zwod */
    for ( zwod = *lista; zwod; zwod = zwod->next )
	if ( zwod->ch == cel )
	    return;

    /* jesli nie mial, dodajemy */
    if ( !zwod_free )
    {
	top_zwod++;
	zwod = alloc_perm( sizeof( ZWOD_DATA ) );
    }
    else
    {
	zwod = zwod_free;
	zwod_free = zwod_free->next;
    }

    zwod->ch = cel;
    zwod->next = *lista;
    *lista = zwod;

    return;
}


void wywal_zwod( ZWOD_DATA **lista, CHAR_DATA *cel )
{
    ZWOD_DATA *zwod, *zwod2;

    if ( !*lista )
    {
	dbug( "wywal_zwod: pusta lista, zrzucam stos!", 0 );
	return;
    }

    if ( ( *lista )->ch == cel )
    {
	zwod = *lista;
	*lista = zwod->next;
	zwod->next = zwod_free;
	zwod_free = zwod;
	return;
    }

    for ( zwod = *lista; zwod->next; zwod = zwod->next )
	if ( zwod->next->ch == cel )
	{
	    zwod2 = zwod->next;
	    zwod->next = zwod2->next;
	    zwod2->next = zwod_free;
	    zwod_free = zwod2;
	    return;
	}

    bug( "Nie znaleziono zwod do postaci.", 0 );

    return;
}


/*
 * Create an instance of an object.
 * Lam 18.5.2000: zapamietywanie najnizszego poziomu na ktorym przedmiot
 * sie pojawia (wykrywanie przekretow)
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
    OBJ_DATA *obj;
    int poziomica, i;

    if ( !pObjIndex )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	exit( 1 );
    }

    if ( level < 0 )
    {
	bug( "Create_object: level %d", level );
	level = 0;
    }

    obj                 = new_object( );

    obj->pIndexData     = pObjIndex;
    obj->in_room        = NULL;
    obj->level          = level;
    obj->wear_loc       = -1;
    pObjIndex->min_reset_level = UMIN( level, pObjIndex->min_reset_level );

    obj->name           = str_dup( pObjIndex->name        );
    obj->short_descr    = str_dup( pObjIndex->short_descr );
    obj->dopelniacz     = str_dup( pObjIndex->dopelniacz  );
    obj->celownik       = str_dup( pObjIndex->celownik    );
    obj->biernik        = str_dup( pObjIndex->biernik     );
    obj->narzednik      = str_dup( pObjIndex->narzednik   );
    obj->miejscownik    = str_dup( pObjIndex->miejscownik );
    obj->description    = str_dup( pObjIndex->description );
    obj->look_descr	= str_dup( pObjIndex->look_descr  );
    obj->action         = str_dup( pObjIndex->action      );
    obj->anti_class	= str_dup( pObjIndex->anti_class  );
    obj->anti_race	= str_dup( pObjIndex->anti_race   );
    obj->owner          = str_dup( "" );
    obj->item_type      = pObjIndex->item_type;
    COPY_BITS( obj->extra_flags, pObjIndex->extra_flags );
    obj->wear_flags     = pObjIndex->wear_flags;
    /* Alandar 06.10.2004 - obsluga wartosci przedmiotow bedacych opisami */
    for ( i = 0; i < 6; i++ )
	if ( typy_przedmiotow[ obj->item_type ].spell[ i ] == VAL_STRING )
	{
	    if ( pObjIndex->value[ i ].p )
		obj->value[ i ].p = ( void * ) str_dup( ( char * ) pObjIndex->value[ i ].p );
	    else
		obj->value[ i ].p = NULL;
	}
	else
	    obj->value[ i ].v = pObjIndex->value[ i ].v;
    obj->weight         = pObjIndex->weight;
    obj->rodzaj         = pObjIndex->rodzaj; /* Wazne */
    /* czesciej ten koszt bedzie zyskiem niz wydatkiem, ale niech tak bedzie --Lam */
    poziomica = number_fuzzy( level );
    obj->cost           = WYDATEK( poziomica, number_fuzzy( 4 ) );
    obj->plr_shop_cost	= 0;
    obj->timer		= -1;	/* Alandar - 20.11.2003 - do spadajacych przedmiotow */
    obj->deleted        = FALSE;

    /*
     * Lam 10.5.98: losowanie wartosci przedmiotow. Nowa flaga b18 zapobiega
     * temu losowaniu
     */
    if ( !IS_OBJ_STAT( obj, ITEM_NORANDVALUES ) )
    {
	switch ( obj->item_type )
	{
	    default:
		cbug( "Create_object: vnum %d bad type. Setting TRASH", pObjIndex->vnum );
		obj->item_type = ITEM_TRASH;
		break;

	    case ITEM_LIGHT:
	    case ITEM_TREASURE:
	    case ITEM_FURNITURE:
	    case ITEM_TRASH:
	    case ITEM_CONTAINER:
	    case ITEM_DRINK_CON:
	    case ITEM_KEY:
	    case ITEM_FOOD:
	    case ITEM_BOAT:
	    case ITEM_CORPSE_NPC:
	    case ITEM_CORPSE_PC:
	    case ITEM_FOUNTAIN:
	    case ITEM_PORTAL:
	    case ITEM_SPELLBOOK:
	    case ITEM_BEACON:
	    case ITEM_SHOVEL:
	    case ITEM_SWISTEK:
	    case ITEM_DEVICE:
	    case ITEM_BANDYTA:
		break;

	    case ITEM_SCROLL:
		obj->value[ 0 ].v   = number_fuzzy( obj->value[ 0 ].v );
		break;

	    case ITEM_WAND:
	    case ITEM_STAFF:
		obj->value[ 0 ].v   = number_fuzzy( obj->value[ 0 ].v );
		if ( obj->value[ 1 ].v > 1 )
		    obj->value[ 1 ].v = number_fuzzy( obj->value[ 1 ].v );
		obj->value[ 2 ].v   = UMIN( obj->value[ 1 ].v, obj->value[ 2 ].v );
		break;

	    case ITEM_WEAPON:
		obj->value[ 1 ].v   = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
		obj->value[ 2 ].v   = number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
		break;

	    case ITEM_ARMOR:
		obj->value[ 0 ].v   = number_fuzzy( level / 4 + 2 );
		break;

	    case ITEM_POTION:
	    case ITEM_PILL:
		obj->value[ 0 ].v   = number_fuzzy( number_fuzzy( obj->value[ 0 ].v ) );
		break;

	    case ITEM_MONEY: /* Lam: mam watpliwosci, ale jest NORANDVALUES */
		obj->value[ 0 ].v   = obj->cost;
		break;
	}
    }

    obj->next           = object_list;
    object_list         = obj;
    pObjIndex->count++;

    return obj;
}


/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;

    *ch                         = ch_zero;
    /* Lam 29.11.2005: tu bylo ustawianie wielu pol na 0, zbedny wysilek po
       wykonaniu powyzszego */
    ch->name                    = &str_empty[ 0 ];
    ch->short_descr             = &str_empty[ 0 ];
    ch->dopelniacz              = &str_empty[ 0 ];
    ch->celownik                = &str_empty[ 0 ];
    ch->biernik                 = &str_empty[ 0 ];
    ch->narzednik               = &str_empty[ 0 ];
    ch->miejscownik             = &str_empty[ 0 ];
    ch->wolacz			= &str_empty[ 0 ];
    ch->long_descr              = &str_empty[ 0 ];
    ch->long_descr_orig         = &str_empty[ 0 ];
    ch->false_name		= &str_empty[ 0 ];
    ch->description             = &str_empty[ 0 ];
    ch->logon                   = current_time;
    ch->position                = POS_STANDING;
    ch->separator               = '|';
    ch->practice                = 21;
    ch->hit                     = 20;
    ch->max_hit                 = 20;
    ch->mana                    = 100;
    ch->max_mana                = 100;
    ch->move                    = 100;
    ch->max_move                = 100;

    return;
}


/*
 * Free a character.
 */
void free_char( CHAR_DATA *ch, bool sprawdz )
{
    OBJ_DATA    *obj;
    OBJ_DATA    *obj_next;
    AFFECT_DATA *paf;
    CHAR_DATA   *dch;

    if ( ch->in_room ) /* Lam 8.11.2000: na wszelki wypadek */
	for ( dch = ch->in_room->people; dch; dch = dch->next_in_room )
	    if ( dch == ch )
	    {
		bug( "free_char: postac byla w pomieszczeniu %d", ch->in_room->vnum );
		char_from_room( ch );
		break;
	    }

    /* Lam 19.10.2008: szukanie dziury w calym. FIXME: usunac jak sie znajdzie */
    if ( ch->zwod_affected )
    {
	sprintf( bug_buf, "free_char: ch %s ma zwod_affected", ch->short_descr );
	bug( bug_buf, 0 );
    }

    /* Lam 11.4.2002: to sie zdarza przy restarcie i utracie polaczenia, na
	razie tam nie poprawiam. Ponizsze rozwiazanie dorazne, ale bedzie na
	przyszlosc */
    if ( sprawdz )
	for ( dch = char_list; dch; dch = dch->next )
	    if ( dch == ch )
	    {
		bug( "free_char: postac byla w char_list!", 0 );
		if ( ch == char_list )
		    char_list = ch->next;
		else
		{
		    for ( dch = char_list; dch; dch = dch->next )
			if ( dch->next == ch )
			{
			    dch->next = ch->next;
			    break;
			}
		    if ( !dch )
			bug( "A co tu sie stalo?!", 0 );
		}
		break;
	    }

    /*
     * This places the character in a place we know exists and allows for
     * more controled removal of mud effects and such.  Suggested by Erwin.
     */
    ch->in_room = get_room_index( ROOM_VNUM_LIMBO );

    for ( obj = ch->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->deleted )
	    continue;
	if ( obj->wear_loc != WEAR_NONE )
	    unequip_char_save( ch, obj );
	extract_obj( obj );
    }

    for ( obj = ch->przechowalnia; obj; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->deleted )
	    continue;
	extract_obj( obj );
    }

    for ( paf = ch->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;
	affect_remove( ch, paf, FALSE );
    }

    while ( ch->alias )
	del_alias( ch, ch->alias );

    while ( ch->tell )
	del_tell( ch, ch->tell );

    while ( ch->mpquest )
	del_mpquest( ch, ch->mpquest );

    while ( ch->zmienne )
	usun_z_listy( &ch->zmienne, ch->zmienne, ch );

    if ( !IS_NPC( ch ) && ch->pcdata->restart )
    {
	RESTART_DATA *restart;

	while ( ch->pcdata->restart )
	{
	    restart = ch->pcdata->restart;
	    ch->pcdata->restart = restart->nast;
	    free_string( restart->wpis );
	    free( restart );
	}
    }

/*
 * Qwert: blad, zalewa wiznet, gdy postac jeszcze nie jest zalozona i probuje
 * wejsc do gry jako juz zalozona, nie majac przypadkow. Dziwacznie rozwiazane,
 * ale jak nie ma ch->name, to reszty pewnie tez.
 */
    if ( ch->name )
    {
	free_string( ch->name               );
	free_string( ch->short_descr        );
	free_string( ch->dopelniacz         );
	free_string( ch->celownik           );
	free_string( ch->biernik            );
	free_string( ch->narzednik          );
	free_string( ch->miejscownik        );
	free_string( ch->wolacz             );
	free_string( ch->long_descr         );
	free_string( ch->long_descr_orig    );
	free_string( ch->false_name	    );
	free_string( ch->description        );
    }

    if ( ch->pcdata )
    {
	int i;

	if ( ch->pcdata->fLog ) fclose( ch->pcdata->fLog );
	ch->pcdata->fLog = NULL;
	free_string( ch->pcdata->pwd            );
	free_string( ch->pcdata->bamfin         );
	free_string( ch->pcdata->bamfout        );
	free_string( ch->pcdata->immskll        );
	free_string( ch->pcdata->title          );
	free_string( ch->pcdata->prompt         );
	free_string( ch->pcdata->fprompt	);
	free_string( ch->pcdata->stopien	);
	free_string( ch->pcdata->email          );
	free_string( ch->pcdata->login          );
	free_string( ch->pcdata->last_login     );
	free_string( ch->pcdata->love.od	); /* Qwert */
	free_string( ch->pcdata->silence.od	);
	free_string( ch->pcdata->notell.od	);
	free_string( ch->pcdata->noemote.od	);
	free_string( ch->pcdata->nocolour.od	);
	/* Lam 13.5.2000: powyzej bylo 2 razy silenced_by, co powodowalo bardzo
	   dziwne rezultaty w przypadku uciszonych przeze mnie postaci. Wykryte
	   dzieki nowej, ulepszonej funkcji free_string( ) */
	free_string( ch->pcdata->log.od		);
	free_string( ch->pcdata->freeze.od	);
	free_string( ch->pcdata->deny.od	);
	free_string( ch->pcdata->thief_from	);
	free_string( ch->pcdata->killer_from	);
	free_string( ch->pcdata->zabojca_from	);
	free_string( ch->pcdata->ignore		);
	/* Qwert */
	free_string( ch->pcdata->pojedynek	);
	free_string( ch->pcdata->nienawisc	);
	free_string( ch->pcdata->szpic_wybr	);
	free_string( ch->pcdata->szpic_wykl	);
	/* Tissarius */
	for ( i = 0; i < MAX_OSTATNIE; i++ )
	    if ( ch->pcdata->ostatnie[ i ] )
		free_string( ch->pcdata->ostatnie[ i ] );
	/* intow nie trzeba czyscic, tym sie zajmuje nowa_postac (save.c) */
	ch->pcdata->next = pcdata_free;
	pcdata_free = ch->pcdata;
	ch->pcdata = NULL;

	free_string( pcdata_free->wampiryzm.pochodzenie );
	free_string( pcdata_free->wampiryzm.strefa );
    }

    ch->in_room = NULL;

    /* Lam 28.8.98: teraz lista zwolnionych postaci jest FILO zamiast FIFO,
     * dzieki temu nawet jesli przez kilka sekund gdzies pozostanie wskaznik na
     * CHAR_DATA po zwolnieniu, nie zostanie on tak szybko ponownie zuzyty i byc
     * moze zapobiegnie to jakiemus przekretowi. Napisalem to, aby chociaz
     * troche zabezpieczyc sie przed bledami mogacymi wyniknac w trakcie
     * eksploatacji timerow (male szanse, ale zawsze cos). */
    ch->next = NULL;

    if ( last_free_char )
	last_free_char->next = ch;
    else
	first_free_char = ch;

    last_free_char = ch;

    return;
}


/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    for ( ; ed; ed = ed->next )
	if ( is_name( name, ed->keyword ) )
	    return ed->description;

    return NULL;
}


/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
    MOB_INDEX_DATA *pMobIndex;

    /*
     * There is a possibility of vnum passed is negative.
     * Trapping suggested by Erwin Andreasen  -Kahn
     */

    if ( vnum < 0 )
	return NULL;

    for ( pMobIndex = mob_index_hash[ vnum % MAX_KEY_HASH ];
	  pMobIndex;
	  pMobIndex = pMobIndex->next )
    {
	if ( pMobIndex->vnum == vnum )
	    return pMobIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_mob_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}


/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;

    /*
     * There is a possibility of vnum passed is negative.
     * Trapping suggested by Erwin Andreasen  -Kahn
     */

    if ( vnum < 0 )
	return NULL;

    for ( pObjIndex = obj_index_hash[ vnum % MAX_KEY_HASH ];
	  pObjIndex;
	  pObjIndex = pObjIndex->next )
    {
	if ( pObjIndex->vnum == vnum )
	    return pObjIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_obj_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}


/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
    ROOM_INDEX_DATA *pRoomIndex;

    /*
     * There is a possibility of vnum passed is negative.
     * Trapping suggested by Erwin Andreasen  -Kahn
     */
    if ( vnum < 0 )
	return NULL;

    for ( pRoomIndex = room_index_hash[ vnum % MAX_KEY_HASH ];
	  pRoomIndex;
	  pRoomIndex = pRoomIndex->next )
    {
	if ( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}


/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace( (int) c ) );

    return c;
}


/*
 * Read a number from a file.
 */
int fread_number( FILE *fp, int *status )
{
    int  c;
    bool sign;
    int  number;
    bool bitv; /* Lam */

    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    number = 0;

    sign = FALSE;
    if ( c == '+' )
    {
	c = getc( fp );
    }
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }

    if ( !isdigit( c ) && ( c != 'b' ) )
    {
	*status = 1;
	cbug( "Fread_number: bad format.", 0 );
/*	cbug( "   If bad object, check for missing '~' in value[] fields.", 0 ); */
	return 0;
    }

    bitv = FALSE;
    if ( c == 'b' )
    {
	bitv = TRUE;
	c = getc( fp );
    }

    while ( isdigit( c ) )
    {
	number = number * 10 + c - '0';
	c = getc( fp );
    }

    /* Lam 15.11.2001: probowalem to zamienic na tablice poteg, ale okazalo
       sie, ze mud uruchamia sie nawet o 5% szybciej z przesunieciami bitow niz
       z dodawaniem, bo tu wszystko dzieje sie w rejestrach, tam - w pamieci */
    if ( bitv )
    {
	if ( number > BITS_PER_INT - 1 )
	{
	    cbug( "Fread_number: zbyt wysoki numer bitu: %d", number );
	    *status = 1;
	    return 0;
	}
	number = 1 << number; /* 2 do potegi number */
    }

    if ( sign )
	number = 0 - number;

    if ( c == '|' )
	number += fread_number( fp, status );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}


/*
 * Czytanie wektorow bitow. Uzycie tylko przez makro fread_vector, ktore
 * najpierw zeruje wektor (mozna to zrobic tutaj i zapamietac, czy wywolujemy
 * rekurencyjnie po '|', znamy rozmiar)
 */
void real_fread_vector( int *flag, unsigned int size, FILE *fp, int *status )
{
    int  c;
    unsigned int  number = 0;
    bool bitv;

    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    if ( !isdigit( c ) && ( c != 'b' ) )
    {
	*status = 1;
	cbug( "Fread_vector: bad format.", 0 );
	return;
    }

    bitv = FALSE;
    if ( c == 'b' )
    {
	bitv = TRUE;
	c = getc( fp );
    }

    while ( isdigit( c ) )
    {
	number = number * 10 + c - '0';
	c = getc( fp );
    }

    if ( bitv )
    {
	if ( ( number >> MOVE_BITS ) >= size )
	{
	    *status = 1;
	    cbug( "Fread_vector: za wysoki numer bitu (%d) dla danych flag", number );
	    return;
	}
	SET_BIT_V( flag, number );
    }
    else
    if ( number != 0 )
    {
	*status = 1;
	cbug( "Fread_vector: dopuszczalne wylacznie bity b<numer>!", 0 );
	return;
    }

    if ( c == '|' )
	real_fread_vector( flag, size, fp, status );
    else if ( c != ' ' )
	ungetc( c, fp );

    return;
}


/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
    int c;

    do
    {
	c = getc( fp );
    }
    while ( c != '\n' && c != '\r' && c != EOF );

    do
    {
	c = getc( fp );
    }
    while ( c == '\n' || c == '\r' );

    ungetc( c, fp );

    return;
}


/*
 * Read one word (into static buffer).
 * Lam 13.6.2000: czy_wielo: czy dopuszczalne slowa wielowyrazowe, np.
 * 'wbijanie kolka'. Przeorganizowane (zoptymalizowane) przez Brottora.
 */
char *fread_word( FILE *fp, int *status, bool czy_wielo )
{
    static char  word[ MAX_INPUT_LENGTH ];
	   char  buf[ MAX_STRING_LENGTH ];
	   char *pword;
	   int   cEnd;
	   int   c;

    *status = 0;

    do
    {
	cEnd = getc( fp );
    }
    while ( isspace( cEnd ) );

    if ( feof( fp ) ) /* Lam 23.2.98 */
    {
	bug( "fread_word: EOF", 0 );
	*status = 1;
	return NULL;
    }

    if ( czy_wielo && ( cEnd == '\'' || cEnd == '\"' ) )
    {
	pword = word;
	for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
	{
	    if ( feof( fp ) )
	    {
		*pword = '\0';
		return word;
	    }
	    c = getc( fp );
	    *pword = c;
	    if ( c == cEnd || c == EOF )
	    {
		*pword = '\0';
		if ( c == EOF )
		{
		    bug( "fread_word: EOF", 0 );
		    *status = 1;
		}
		return word;
	    }
	}
    }
    else
    {
	word[ 0 ] = cEnd;
	pword = word + 1;
	/* Brottor: na pierwszy rzut oka tego nie widac, ale cEnd jest pozniej
	   uzywane */
	cEnd = ' ';
	for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
	{
	    if ( feof( fp ) )
	    {
		*pword = '\0';
		return word;
	    }
	    c = getc( fp );
	    *pword = c;
	    /* pomija rowniez znaki konca linii */
	    if ( isspace( c ) )
	    {
		ungetc( c, fp );
		*pword = '\0';
		return word;
	    }
	}
    }

    /* Lam 10.2.98: pomoc w odpluskwianiu
       Lam 23.2.98: pomoglo w przypadku EOF */
    word[ MAX_INPUT_LENGTH - 1 ] = '\0';
    sprintf( buf, "Fread_word: (cEnd: '%c') word too long: %s", cEnd, word );
    bug( buf, 0 );
    *status = 1;

    return NULL;
}


/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
#if defined( ALLOC_VALGRIND )
    return calloc( 1, sMem );
#else
    void *pMem;
    int iList;

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[ iList ] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
	bug( "Alloc_mem: size %d too large.", sMem );
	exit( 1 );
    }

    if ( !rgFreeList[ iList ] )
    {
	pMem = alloc_perm( rgSizeList[ iList ] );
    }
    else
    {
	pMem = rgFreeList[ iList ];
	rgFreeList[ iList ] = *( (void **) rgFreeList[ iList ] );
    }

    return pMem;
#endif /* ALLOC_VALGRIND */
}


/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
#if defined( ALLOC_VALGRIND )
    free( pMem );
#else
    int iList;

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[ iList ] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
	bug( "Free_mem: size %d too large.", sMem );
	exit( 1 );
    }

    *( (void **) pMem ) = rgFreeList[ iList ];
    rgFreeList[ iList ] = pMem;

    return;
#endif /* ALLOC_VALGRIND */
}


/*
 * Allocate some permanent memory.
 * Permanent memory is never freed, pointers into it may be copied safely.
 *
 * Pamiec jest przydzielana przez system w blokach po (domyslnie) 128 KiB,
 * nastepnie wewnatrz kazdego bloku rozdzielana w miare potrzeb kolejnym
 * funkcjom, ktore o pamiec prosza. O kolejny blok poprosimy system dopiero
 * kiedy ten blok sie wyczerpie. Realnie na rzecz patrzac, to nam niczego nie
 * daje w naszych czasach (podobna logika wbudowana jest w glibc).
 */
void *alloc_perm( int sMem )
{
#if defined( ALLOC_VALGRIND )
    return calloc( 1, sMem );
#else
	   void *pMem;
    static char *pMemPerm;
    static int   iMemPerm;

    while ( sMem % sizeof( long ) != 0 )
	sMem++;
    if ( sMem > MAX_PERM_BLOCK )
    {
	bug( "Alloc_perm: %d too large.", sMem );
	exit( 1 );
    }

    if ( !pMemPerm || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if ( !( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) )
	{
	    lac_perror( "Alloc_perm" );
	    exit( 1 );
	}
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;

    return pMem;
#endif /* ALLOC_VALGRIND */
}


/* Lam 23.8.1998, przepisane 6.1.2008 na wyrownaj( ), zeby pozbyc sie
   colourconv( ) z do_areas( ) */
char *nazwa_krainy( char *nazwa )
{
    static char nowa[ 128 ];
    char poziomy[ 12 ];
    char imiona[ 40 ];
    bool po_poz = FALSE;
    char *ptr;

    ptr = poziomy;
    while ( !po_poz )
    {
	if ( *nazwa == ']' )
	    po_poz = TRUE;
	*ptr++ = *nazwa++;
    }

    *ptr = '\0';

    while ( isspace( (int) *nazwa ) )
	nazwa++;

    ptr = imiona;

    if ( *nazwa != '+' )
	*ptr++ = ' ';

    do
    {
	*ptr++ = *nazwa++;
    } while ( !isspace( (int) *nazwa ) );

    *ptr = '\0';

    while ( isspace( (int) *nazwa ) )
	nazwa++;

    sprintf( nowa, "%s%s%s", poziomy, wyrownaj( imiona, -11 ), nazwa );

    return nowa;
}


/*
 * Lam 17.2.2003: lekko uporzadkowany kod wyswietlajacy to, obsluga krain
 * niewidocznych na liscie i lepsza obsluga stref
 */
KOMENDA( do_areas )
{
    real_areas( ch, argument, 0 );

    return;
}

void real_areas( CHAR_DATA *ch, char *argument, WHO_DESCRIPTOR_DATA *d )
{
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    ZONE_DATA *pZone;
    char       buf  [ MAX_STRING_LENGTH ];
    char       buf1 [ MAX_STRING_LENGTH * 4 ];
    char       bufa1[ MAX_STRING_LENGTH * 4 ];
    char       bufa2[ MAX_STRING_LENGTH * 4 ];
    int        iArea;
    int        iAreaHalf;
    int        iAreaTop; /* Lam 17.2.2003 */
    char       arg[ MAX_STRING_LENGTH ];

    buf1[ 0 ] = '\0';
    pZone = NULL;

    one_argument( argument, arg );

    if ( *arg )
    {
	pZone = znajdz_strefe_skrot( arg );
	if ( !pZone && ch )
	{
	    STC( "Nie ma takiej strefy.\n\r", ch );
	    return;
	}
    }

    /* najpierw liczymy strefy do pokazania */
    iAreaTop = 0;
    for ( pArea1 = area_first; pArea1; pArea1 = pArea1->next )
	if ( IS_SET( pArea1->flags, AREA_NOT_LISTED )
	  || ( pZone
	    && ( !strefa_nalezy( pZone, pArea1->strefy ) ) ) )
	    continue;
	else
	    iAreaTop++;

    if ( !iAreaTop )
    {
	if ( ch )
	    send_to_char( "Brak krain do pokazania.\n\r", ch );
	else if ( d )
	    STH( d, "Brak krain do pokazania.<br>" );
	return;
    }

    /* potem pokazujemy, co mamy pokazac, w kolumnach */
    /* FIXME: Uwaga! Obecny kod nie obsluguje mniej niz 3 widocznych krain
       w strefie, ani wiecej niz polowe niewidocznych krain w strefie */
    iAreaHalf = ( iAreaTop + 1 ) / 2;
    pArea1 = area_first;
    pArea2 = area_first;
    iArea = 0;
    if ( IS_SET( pArea2->flags, AREA_NOT_LISTED )
      || ( pZone
	&& ( !strefa_nalezy( pZone, pArea2->strefy ) ) ) )
	iArea--;
    for ( ; iArea < iAreaHalf; )
    {
	pArea2 = pArea2->next;
	if ( !IS_SET( pArea2->flags, AREA_NOT_LISTED )
	  && ( !pZone
	    || strefa_nalezy( pZone, pArea2->strefy ) ) )
	    iArea++;
    }

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
	/* w kazdej kolumnie oddzielnie szukamy nastepnej do pokazania */
	while ( IS_SET( pArea1->flags, AREA_NOT_LISTED )
	     || ( pZone
	       && ( !strefa_nalezy( pZone, pArea1->strefy ) ) ) )
	       pArea1 = pArea1->next;
	while ( pArea2
	     && ( IS_SET( pArea2->flags, AREA_NOT_LISTED )
	       || ( pZone
		 && ( !strefa_nalezy( pZone, pArea2->strefy ) ) ) ) )
	    pArea2 = pArea2->next;
	/* warunki spelnione */
	strcpy( bufa1, nazwa_krainy( pArea1->name ) );
	if ( pArea2 )
	    strcpy( bufa2, nazwa_krainy( pArea2->name ) );
	sprintf( buf, "%s%s\n\r", wyrownaj( bufa1, -39 ),
		wyrownaj( ( pArea2 ) ? bufa2 : "", -39 ) );
	strcat( buf1, buf );
	pArea1 = pArea1->next;
	if ( pArea2 )
	    pArea2 = pArea2->next;
    }

    if ( ch && IS_IMMORTAL( ch ) )
    {
	sprintf( buf, "\n\r{yAktualnie znajdujesz si`e w krainie: {Y%s{x\n\r",
	    ch->in_room->area->full_name );
	strcat( buf1, buf );
    }

    if ( ch )
	send_to_char( buf1, ch );
    else if ( d )
    {
	char wyjbuf[ MSL * 4 ];
	colourconv( wyjbuf, lac2html( buf1, FALSE ), NULL, sizeof( wyjbuf ) );
	STH( d, wyjbuf );
    }

    return;
}


void znajdz_kraine( char *arg, struct zapamietana_kraina *zk )
{
    AREA_DATA *kr;

    zk->kraina = NULL;

    for ( kr = area_first; kr; kr = kr->next )
	if ( !strcmp( kr->file_name, arg ) ) /* limbo.are != LIMBO.ARE */
	{
	    zk->kraina = kr;
	    return;
	}

    return;
}


void zapamietaj_kraine( struct zapamietana_kraina *zk )
{
    MOB_INDEX_DATA *mob, *tmob;
    OBJ_INDEX_DATA *prz, *tprz;
    ROOM_INDEX_DATA *pom, *tpom;
    RESET_DATA *tres;
    int i;

    for ( i = 0; i < MAX_KEY_HASH; i++ )
    {
	while ( mob_index_hash[ i ]
	     && mob_index_hash[ i ]->index_area == zk->kraina )
	{
	    tmob = mob_index_hash[ i ];
	    mob_index_hash[ i ] = tmob->next;
	    tmob->next = zk->moby;
	    zk->moby = tmob;
	}

	for ( mob = mob_index_hash[ i ]; mob && mob->next; mob = mob->next )
	{
	    if ( mob->next->index_area == zk->kraina )
	    {
		tmob = mob->next;
		mob->next = tmob->next;
		tmob->next = zk->moby;
		zk->moby = tmob;
	    }
	}

	while ( obj_index_hash[ i ]
	     && obj_index_hash[ i ]->area == zk->kraina )
	{
	    tprz = obj_index_hash[ i ];
	    obj_index_hash[ i ] = tprz->next;
	    tprz->next = zk->przedmioty;
	    zk->przedmioty = tprz;
	}

	for ( prz = obj_index_hash[ i ]; prz && prz->next; prz = prz->next )
	{
	    if ( prz->next->area == zk->kraina )
	    {
		tprz = prz->next;
		prz->next = tprz->next;
		tprz->next = zk->przedmioty;
		zk->przedmioty = tprz;
	    }
	}

	while ( room_index_hash[ i ]
	     && room_index_hash[ i ]->area == zk->kraina )
	{
	    tpom = room_index_hash[ i ];
	    room_index_hash[ i ] = tpom->next;
	    tpom->next = zk->pomieszczenia;
	    zk->pomieszczenia = tpom;
	}

	for ( pom = room_index_hash[ i ]; pom && pom->next; pom = pom->next )
	{
	    if ( pom->next->area == zk->kraina )
	    {
		tpom = pom->next;
		pom->next = tpom->next;
		tpom->next = zk->pomieszczenia;
		zk->pomieszczenia = tpom;
	    }
	}

	while ( zk->kraina->reset_first )
	{
	    tres = zk->kraina->reset_first;
	    zk->kraina->reset_first = tres->next;
	    tres->next = zk->resety;
	    zk->resety = tres;
	}
    }

    return;
}


void przywroc_kraine( struct zapamietana_kraina *zk )
{
    return;
}


KOMENDA( do_przeladuj )
{
#if 0
    struct zapamietana_kraina zk;
#endif

    if ( !authorized( get_char( ch ), "prze`laduj" ) )
	return;

    STC( "Nie u`zywaj tego!\n\r", ch );
    return;

#if 0
    if ( !*argument )
    {
	STC( "Podaj nazw`e pliku krainy.\n\r", ch );
	return;
    }

    if ( !( fpArea = fopen( argument, "r" ) ) )
    {
	lac_perror( argument );
	ch_printf( ch, "Nie uda`lo si`e otworzy`c pliku \"%s\", komunikat b`l`edu w logu.\n\r", argument );
	return;
    }

    znajdz_kraine( argument, &zk );
    if ( zk.kraina )
	zapamietaj_kraine( &zk );

    strcpy( strArea, argument );
    if ( load_area_file( fpArea, ch ) )
    {
	/* FIXME: odzyskanie krainy */
	STC( "B`l`ad `ladowania krainy! Szczeg`o`l`ow szukaj w dzienniku.\n\r", ch );
	fpArea = NULL;
	return;
    }
/*
    fix_exits( );
*/

    fpArea = NULL;
    STC( "Kraina za`ladowana.\n\r", ch );

    return;
#endif
}


/* Lam: pozmienialem na ch_printf */
extern int top_timer; /* handler.c */
extern int HEADER_SIZE; /* ssm.c */
KOMENDA( do_memory )
{
    int ssmn;

    if ( !authorized( get_char( ch ), "pami`e`c" ) )
	return;

    ch_printf( ch, "Affects  %5d * %d\n\r", top_affect, (int) sizeof( AFFECT_DATA ) );
    ch_printf( ch, "Aliases  %5d * %d\n\r", top_alias, (int) sizeof( ALIAS_DATA ) );
    ch_printf( ch, "Areas    %5d * %d\n\r", top_area, (int) sizeof( AREA_DATA ) );
    ch_printf( ch, "Clans    %5d * %-5d    ", top_clan, (int) sizeof( CLAN_DATA ) );
    ch_printf( ch, "ClMembs  %5d * %-5d    ", top_clan_member, (int) sizeof( CLAN_MEMBER_DATA ) );
    ch_printf( ch, "ClRels   %5d * %d\n\r", top_clan_rel, (int) sizeof( CLAN_REL_DATA ) );
    ch_printf( ch, "ExDes    %5d * %d\n\r", top_ed, (int) sizeof( EXTRA_DESCR_DATA ) );
    ch_printf( ch, "Fights   %5d * %d\n\r", top_fight, (int) sizeof( FIGHT_DATA ) );
    ch_printf( ch, "Helps    %5d * %-5d    ", top_help, (int) sizeof( HELP_DATA ) );
    ch_printf( ch, "Imiona   %5d * %d\n\r", top_imiona, (int) sizeof( IMIONA_DATA ) );
    ch_printf( ch, "MobInd   %5d * %-5d    ", top_mob_index, (int) sizeof( MOB_INDEX_DATA ) );
    ch_printf( ch, "Mobs     %5d * %-5d    ", top_mob, (int) sizeof( CHAR_DATA ) );
    ch_printf( ch, "Shops    %5d * %d\n\r", top_shop, (int) sizeof( SHOP_DATA ) );
    ch_printf( ch, "Healers  %5d * %-5d    ", top_healer, (int) sizeof( HEALER_DATA ) );
    ch_printf( ch, "MPQuests %5d * %-5d    ", top_mpquest, (int) sizeof( MPQUEST_DATA ) );
    ch_printf( ch, "PCData   %5d * %d\n\r", top_pcdata, (int) sizeof( PC_DATA ) );
    ch_printf( ch, "ObjInd   %5d * %-5d    ", top_obj_index, (int) sizeof( OBJ_INDEX_DATA ) );
    ch_printf( ch, "Objs     %5d * %d\n\r", top_obj, (int) sizeof( OBJ_DATA ) );
    ch_printf( ch, "Progs    %5d * %-5d    ", top_mprog, (int) sizeof( MPROG_DATA ) );
    ch_printf( ch, "Zmienne  %5d * %-5d    ", top_zmienna, (int) sizeof( TYP_ZMIENNEJ ) );
    ch_printf( ch, "StosMP   %5d * %d\n\r", top_stosmp, (int) sizeof( STOSMP ) );
    ch_printf( ch, "Resets   %5d * %d\n\r", top_reset, (int) sizeof( RESET_DATA ) );
    ch_printf( ch, "Rooms    %5d * %-5d    ", top_room, (int) sizeof( ROOM_INDEX_DATA ) );
    ch_printf( ch, "Exits    %5d * %-5d    ", top_exit, (int) sizeof( EXIT_DATA ) );
    ch_printf( ch, "Traps    %5d * %d\n\r", top_trap, (int) sizeof( DEATHTRAP_DATA ) );
    ch_printf( ch, "Tells    %5d * %d\n\r", top_tell, (int) sizeof( TELL_DATA ) );
    ch_printf( ch, "Timers   %5d * %-5d    ", top_timer, (int) sizeof( TIMER_DATA ) );
    ch_printf( ch, "ZwOd     %5d * %d\n\r", top_zwod, (int) sizeof( ZWOD_DATA ) );

    ch_printf( ch, "Total perms    %6d blocks of %7d bytes.\n\r\n\r",
	    nAllocPerm, sAllocPerm );

    ch_printf( ch, "Shared Strings   %5d strings of %7d bytes (max %d).\n\r",
	    nAllocString, sAllocString, MAX_STRING );
    ssmn = ssm_num_free_entries( );
    ch_printf( ch, "Free Entries     %5d, largest   %7d bytes, avg %7d bytes.\n\r",
	    ssmn, ssm_max_free_entry( ),
	    ssmn ? ( MAX_STRING - sAllocString - ssmn * HEADER_SIZE ) / ssmn : 0 );
    ch_printf( ch, "Overflow Strings %5d strings of %7d bytes.\n\r",
	    nOverFlowString, sOverFlowString );

    if ( Full )
	STC( "Shared String Heap is full, increase MAX_CHUNKS.\n\r", ch );

    return;
}


/*
 * Append a string to a file.
 * Lam 5.3.98: fpReserve na fpBugReserve
 */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;

    if ( IS_NPC( ch ) || str[ 0 ] == '\0' )
	return;

    zamknij_reserve( fpBugReserve );
    if ( !( fp = fopen( file, "a" ) ) )
    {
	lac_perror( file );
	send_to_char( "Nie moge otworzyc pliku!\n\r", ch );
    }
    else
    {
	fprintf( fp, "[%6d] %s: %s\n",
		ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
	fclose( fp );
    }

    fpBugReserve = fopen( NULL_FILE, "r" );

    return;
}


/*
 * Reports a bug.
 * Lam 17.6.98: robie na wzor demonow logujacych Linuksa (last message repeated
 * x times) dla powtarzajacych sie bledow. Powtorki sa odnotowywane w pliku
 * z bledami, ale w dzienniku pisza sie caly czas, zeby bylo widac zaleznosc
 * czasowa miedzy bledami a innymi zdarzeniami.
 * Lam 18.4.2000: do odpluskwiania plik, linia i nazwa funkcji
 * Lam 20.1.2004: obsluga backtrace na systemach z GNU libc
 * Lam 17.6.2007: dbug( ), potrzebuje znalezc jeden przypadek
 */
void real_bug( const char *str, int param, const char *plik, int linia,
			const char *funkcja, bool zrzut, bool zrob_core )
{
	FILE *fp;
	char  buf [ MAX_STRING_LENGTH ];
static	char  last_bug[ MSL ]; /* Lam */
static	int   powtorki;
#if defined( BACKTRACE )
	void *slady[ 20 ]; /* wskazniki na funkcje */
	unsigned int ilslad; /* ilosc sladow */
	char **symslad; /* symbole sladow, nazwy funkcji */
	unsigned int i;
#endif

#if defined( unix )
    if ( zrob_core )
	if ( !fork( ) )
	    abort( );
#endif

    if ( fpArea )
    {
	int iLine;
	int iChar;

	if ( fpArea == stdin )
	{
	    iLine = 0;
	}
	else
	{
	    iChar = ftell( fpArea );
	    fseek( fpArea, 0, 0 );
	    /* Lam 3.12.2000: feof( ) */
	    for ( iLine = 0; ftell( fpArea ) < iChar && !feof( fpArea ); iLine++ )
	    {
		while ( getc( fpArea ) != '\n' && !feof( fpArea ) )
		    ;
	    }
	    fseek( fpArea, iChar, 0 );
	}

	sprintf( buf, "[*****] PLIK: %s LINIA: %d", strArea, iLine );
	log_string( buf );

	if ( ( fp = fopen( "SHUTDOWN.TXT", "a" ) ) )
	{
	    fprintf( fp, "[*****] %s\n", buf );
	    fclose( fp );
	}
    }
#if defined( BACKTRACE )
    else if ( zrzut && strcmp( str, "***czyszczenie powtorek***" ) )
    {
	ilslad = backtrace( slady, 20 );
	symslad = backtrace_symbols( slady, ilslad );

	sprintf( buf, "[-----] TANDETA: Zrzucam stos wywolan, %u adresow", ilslad );
	log_string( buf );

	for ( i = 0; i < ilslad; i++ )
	    fprintf( stderr, "[-----] [#%u] %s\n", i, symslad[ i ] );

	free( symslad );
    }
#endif
#if defined( PRINTSTACK )
    else if ( zrzut && strcmp( str, "***czyszczenie powtorek***" ) )
    {
	sprintf( buf, "[-----] TANDETA: Zrzucam stos wywolan" );
	log_string( buf );

	printstack( fileno( stderr ) );
    }
#endif

#if defined( DOPALANY_BUG )
    /* gwiazdki niepotrzebne odkad do_bug ma oddzielny plik, ale rzuca sie
       w oczy w dziennikach */
    sprintf( buf, "[*****] [%s:%d %s()] TANDETA: ", plik, linia, funkcja );
#else
    strcpy( buf, "[*****] TANDETA: " );
#endif
    /* Lam 23.3.2005: sprintf moglo byc niebezpieczne, jesli chcialem zglosic
       blad zawierajacy lancuch, ktory mogl zalezec od uzytkownika (ktory by
       tam nawstawial %s, %n itp.). Teraz przy param == 0 bedzie mogl wyskoczyc
       jakis blad ze x == %d (zamiast 0), ale my to raczej zrozumiemy */
    if ( param )
	sprintf( buf + strlen( buf ), str, param );
    else
	strcat( buf, str );
    if ( strcmp( str, "***czyszczenie powtorek***" ) ) /* tego nie logowac */
    {
	char buf1[ MSL ];
	char *b, *b1;

	log_string( buf );
	b = buf1;
	b1 = buf;
	while ( TRUE )
	{
	    *b = *b1;
	    if ( !*b )
		break;
	    if ( *b == '$' )
	    {
		b++;
		*b = '\\';
	    }
	    b++;
	    b1++;
	}

	wiznet( buf1, NULL, NULL, NULL, WIZ_BUG, 0, 106 );
	if ( zrzut )
	    wiznet( "[*****] {YDo dziennika zosta`l zapisany zrzut stosu!{x", NULL, NULL, NULL, WIZ_BUG, 0, 106 );
    }

    if ( !strcmp( buf, last_bug ) )
    {
	powtorki++;
	return;
    }
    else if ( powtorki )
    {
	sprintf( last_bug, "[-----] Ostatni blad powtorzony jeszcze %d raz%s",
		powtorki, powtorki == 1 ? "" : "y" );
	zamknij_reserve( fpBugReserve );
	if ( ( fp = fopen( BUG_FILE, "a" ) ) )
	{
	    fprintf( fp, "%s\n", last_bug );
	    fclose( fp );
	}
	fpBugReserve = fopen( NULL_FILE, "r" );
    }

    strcpy( last_bug, buf );
    powtorki = 0;

    /*
     * W ten sposob rozwiazalem problem pokazywania resztek bledow przed
     * restartem, bo normalnie nie pokazaloby sie, ze blad sie powtorzyl.
     */
    if ( !strcmp( str, "***czyszczenie powtorek***" ) )
	return;

    zamknij_reserve( fpBugReserve );
    if ( ( fp = fopen( BUG_FILE, "a" ) ) )
    {
	fprintf( fp, "%s\n", buf );
	fclose( fp );
    }
    fpBugReserve = fopen( NULL_FILE, "r" );

    return;
}


/* Lam 6.11.2004 */
#if defined( NO_STRERROR_R )
void lac_perror( const char *str )
{
    perror( str );

    return;
}
#elif defined( GNU_STRERROR_R ) && ( defined( _GNU_SOURCE ) || !( _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600 ) )
/* niestandardowe rozszerzenie GNU */
void lac_perror( const char *str )
{
    char strtime[ MIL ];
    char strerrb[ MIL * 2 ];
    char strerrt[ MIL * 2 ];
    char *zwrse;

    strerrb[ 0 ] = '\0';

    /* Wyglada na to, ze rozszerzenie GNU jest dobre, bo nigdy nie kopiuje
       lancuchow, w zamian zwracajac adres stalego lancucha gdzies w swojej
       pamieci. Zamiast strerrt zwyklem podawac NULL, ale glibc 2.3 wprowadza
       __nonnull ((2)), wiec podaje ten bufor, choc zdaje sie, ze jest
       wykorzystywany wylacznie do wpisywania tam "Unknown error xxx"
       w podbramkowych sytuacjach */
    if ( !( zwrse = strerror_r( errno, strerrt, MIL * 2 ) ) )
	strcpy( strerrb, "BLAD STRERROR_R" );
    else
	strcpy( strerrb, zwrse );

    strcpy( strtime, ctime( &current_time ) );
    strtime[ strlen( strtime ) - 1 ] = '\0';
    fprintf( stderr, "%s :[PERROR]: %s: %s\n", strtime, str, strerrb );

    return;
}
#else /* normalna wersja (standard SUSv3), zawsze kopiuje lancuchy */
void lac_perror( const char *str )
{
    char strtime[ MIL ];
    char strerrb[ MIL * 2 ];

    strerrb[ 0 ] = '\0';

# if !defined( MSDOS )
    if ( strerror_r( errno, strerrb, MIL * 2 ) )
	strcpy( strerrb, "BLAD STRERROR_R" );
# else /* DJGPP nie ma strerror_r, bo i watkow nie ma */
    strcpy( strerrb, strerror( errno ) );
# endif
    strcpy( strtime, ctime( &current_time ) );
    strtime[ strlen( strtime ) - 1 ] = '\0';
    fprintf( stderr, "%s :[PERROR]: %s: %s\n", strtime, str, strerrb );

    return;
}
#endif


/*
 * Writes a string to the log.
 */
void log_string( const char *str )
{
    char *strtime;

    strtime = ctime( &current_time );
    strtime[ strlen( strtime ) - 1 ] = '\0';
    fprintf( stderr, "%s :: %s\n", strtime, str );

    return;
}


void log_string_ch( CHAR_DATA *ch, const char *str )
{
    CHAR_DATA *org;
    char *strtime;

    if ( ch->desc && ch->desc->original )
	org = ch->desc->original;
    else
	org = ch;

    strtime = ctime( &current_time );
    strtime[ strlen( strtime ) - 1 ] = '\0';

    if ( !IS_NPC( org ) && org->pcdata->fLog )
	fprintf( org->pcdata->fLog, "%s :: %s\n", strtime, str );
    else
	fprintf( stderr, "%s :: %s\n", strtime, str );

    return;
}


/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 *
 * GCC w pewnym momencie (glownie z powodu niemozliwosci implementacji na
 * architekturach takich jak ARM) przestal optymalizowac "tail calls" dla
 * wlasciwie wszystkich funkcji (m. in. zadna funkcja, ktora miala wlasne
 * zmienne nie byla kandydatem do optymalizacji). Sytuacja ulegla drastycznej
 * poprawie wraz z GCC 3.4 (gdzie wprowadzono inne instrukcje w wewnetrznym
 * jezyku RTL z mozliwoscia optymalizacji dla konkretnych architektur
 * procesora).
 *
 * Aby jednak nie popsuc nam odpluskwiaczy, na co skarzyl sie Furey w 1993,
 * gcc domyslnie wykonuje zwykle wywolania funkcji i duzo wczesniej niz GCC 3.4
 * (bo w 3.0) pojawila sie opcja -foptimize-sibling-calls (wlaczona przy -O2
 * i -Os, ale wylaczona domyslnie). Oznacza to, ze GCC nie bedzie sprawiac
 * klopotow dopoki ktos nie wlaczy optymalizacji (co powoduje sto innych
 * problemow oprocz tego). Mowiac prosciej - tail_chain( ) do niczego nie
 * sluzy w naszych czasach, chyba, ze ktos ma zamiar kompilowac Laca za pomoca
 * GCC 2.x lub czegos jeszcze innego.
 *
 * -foptimize-sibling-calls moze byc przydatne takze bez innych optymalizacji,
 * poniewaz zabezpiecza program przed bledami naruszania ochrony w przypadku
 * bardzo glebokiej rekurencji (np. poprzednia wersja find_path w swiecie
 * milion razy wiekszym od naszego), chociaz w Lacu nie ma funkcji, ktore
 * by sie rekurencyjnie wywolywaly miliony razy, a jesli beda, to chyba tylko
 * nieskonczone (wtedy lepiej, zeby sie wywalil niz zapetlil). Czyli znowu
 * wniosek, ze nam to niczego nie da.
 *
 * Z wymienionych wyzej powodow ani nie uzywam tail_chain( ) nigdzie, ani nie
 * usuwam zupelnie z kodu.
 *
 * --Lam
 */
void tail_chain( void )
{
    return;
}


/* Lam */
KOMENDA( do_astat )
{
    CHAR_DATA *rch;
    AREA_DATA *Area;
    ZONE_LIST *lista;
    char       buf[ MAX_STRING_LENGTH ];
    char       buf1[ MAX_STRING_LENGTH * 4 ];

    rch = get_char( ch );
    if ( !authorized( rch, "kstat" ) )
	return;

    buf1[ 0 ] = '\0';

    if ( !*argument )
    {
	for ( Area = area_first; Area; Area = Area->next )
	{
	    /* Lam 22.5.98: poprawka wygladu */
	    sprintf( buf, "%s", wyrownaj( Area->name, -39 ) );
	    sprintf( buf + znajdz_adres_pl_kol( buf, 39 ), "|%s",
			wyrownaj( Area->full_name, -25 ) );
	    sprintf( buf + znajdz_adres_pl_kol( buf, 64 ), "|%s\n\r",
			Area->file_name );
	    strcat( buf1, buf );
	}
    }
    else
    {
	strcpy( buf1, "Podaj nazwe pliku krainy.\n\r" );
	for ( Area = area_first; Area; Area = Area->next )
	if ( !str_prefix( argument, Area->file_name ) )
	{
	    /* Lam 30.5.98 */
	    sprintf( buf1, "Nazwa:           %s\n\r", Area->name );
	    sprintf( buf, "Pe`lna nazwa:     %s\n\r", Area->full_name );
	    strcat( buf1, buf );
	    sprintf( buf, "Nazwa pliku:     %s\n\r", Area->file_name );
	    strcat( buf1, buf );
	    if ( Area->revision )
		sprintf( buf, "Wersja:          %s\n\r", Area->revision );
	    else
		sprintf( buf, "Wersja:          (brak informacji)\n\r" );
	    strcat( buf1, buf );
	    sprintf( buf, "Strefy:          %s\n\r", Area->strefy->zone->nazwa );
	    strcat( buf1, buf );
	    lista = Area->strefy->next;
	    while ( lista )
	    {
		sprintf( buf, "                 %s\n\r", lista->zone->nazwa );
		lista = lista->next;
		strcat( buf1, buf );
	    }
	    if ( Area->recall )
		sprintf( buf, "Przywo`lanie:     %d\n\r", Area->recall );
	    else
		sprintf( buf, "Przywo`lanie:     (brak, miejsce urodzenia postaci)\n\r" );
	    strcat( buf1, buf );
	    sprintf( buf, "Strefa czasowa:  %d     Godzina:  %d\n\r",
		    Area->time_info.hour - time_info.hour, Area->time_info.hour );
	    strcat( buf1, buf );
	    if ( Area->reset_msg && *Area->reset_msg )
	    {
		sprintf( buf,  "Przed odnow`a:    %s\n\r", Area->reset_msg );
		strcat( buf1, buf );
	    }
	    /* liczniki room/mob/obj - Lam 7.7.98 */
	    sprintf( buf, "Pomieszczenia:   %d\n\r", Area->numofrooms );
	    strcat( buf1, buf );
	    sprintf( buf, "Indeksy mob`ow:   %d\n\r", Area->numofmobs );
	    strcat( buf1, buf );
	    sprintf( buf, "Indeksy przedm:  %d\n\r", Area->numofobjs );
	    strcat( buf1, buf );
	    /* poziomy mobow - Lam 9.7.98 */
	    if ( Area->numofmobs )
	    {
		sprintf( buf, "Poziomy mob`ow:   %d - %d\n\r",
				    Area->lowmoblevel, Area->highmoblevel );
		strcat( buf1, buf );
	    }
	    /* zakresy vnumowe - Lam 8.7.98 */
	    if ( Area->numofrooms )
	    {
		sprintf( buf, "Vnumy pom.:      #%d - #%d\n\r",
				    Area->lowroomvnum, Area->highroomvnum );
		strcat( buf1, buf );
	    }
	    if ( Area->numofmobs )
	    {
		sprintf( buf, "Vnumy mob`ow:     #%d - #%d\n\r",
				    Area->lowmobvnum, Area->highmobvnum );
		strcat( buf1, buf );
	    }
	    if ( Area->numofobjs )
	    {
		sprintf( buf, "Vnumy przedm.:   #%d - #%d\n\r",
				    Area->lowobjvnum, Area->highobjvnum );
		strcat( buf1, buf );
	    }
	    if ( get_trust( ch ) >= L_DIR )
	    {
		sprintf( buf, "\n\rGracze:          %d\n\r", Area->nplayer );
		strcat( buf1, buf );
	    }
	    sprintf( buf, "Wiek:            %d\n\r", Area->age );
	    strcat( buf1, buf );
	    break;
	}
    }

    send_to_char( buf1, ch );

    return;
}
