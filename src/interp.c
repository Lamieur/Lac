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


void	do_many			args( ( CHAR_DATA *ch, char *argument ) );
void	zastepstwo		args( ( CHAR_DATA *ch, int numer ) );
char	*ity_argument		args( ( const char *argument, int ktory ) );
char	*po_itym_arg		args( ( const char *argument, int ktory ) );
void	sprawdz_parametry	args( ( const char *alias, int *parametry ) );
void	nowe_instrukcje		args( ( CHAR_DATA *ch ) );



/* To w wiekszosci dzielo Albiego, jeden z najstarszych fragmentow kodu */
void do_many( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char buffer[ MAX_INPUT_LENGTH ];
    int  count;
    int  count_buf;

    rch = get_char( ch );
    if ( !authorized( rch, "%" ) )
	return;

    count_buf = 0;
    for ( count = 0; argument[ count ] != '\0'; count++ )
	if ( argument[ count ] != ch->separator )
	{
	    buffer[ count_buf ] = argument[ count ];
	    count_buf++;
	}
	else
	{
	    buffer[ count_buf ] = '\0';
	    interpret( ch, buffer, INTERP_MANY );
	    count_buf = 0;
	}

    buffer[ count_buf ] = '\0';
    if ( buffer[ 0 ] != '\0' )
	interpret( ch, buffer, INTERP_MANY );

    ch->wait = 0;

    return;
}


/*
 * Command logging types.
 */
#define NORMAL		0
#define ALWAYS		1
#define NEVER		2

/*
 * Restrykcje wykonywania komend.
 * Teraz nie ma mozliwosci stworzenia proga, ktory po zobaczeniu mnie na
 * poziomie 110 zmuszalby mnie (mpforce) do awansu tworcy krainy. Nie ma tez
 * mozliwosci uzycia "zmus" do zmuszenia moba do zmuszenia mnie do awansu. Przy
 * okazji nie bedzie sie dalo wyreczyc sie kims np. zmus albi jebut tylko trzeba
 * bedzie robic to samemu.
 */
#define NONE		  0
#define FORCE		b00	/* nie mozna do tego zmusic (force, mpforce) */
#define PROG		b01	/* w progu nie mozna tego umiescic,
				   rowniez mpforce */
#define ORDER		b02	/* mozna zmusic, ale nie mozna rozkazac */
#define NOIC		b03	/* nie mozna wykonywac WP */
#define OOC		b04	/* mozna wykonywac PP */
#define NOTLISTED	CMD_NOTLISTED	/* nie widac w "polecenia" */
#define ALLFORCE b00 | b01 | b02 /* nie mozna w zaden sposob zmusic */

/*
 * God Levels - Check them out in merc.h
 */
#define L_HER                   LEVEL_HERO

/*
 * Log-all switch.
 */
bool                            fLogAll             = FALSE;



/*
 * Command table.
 */
const   struct  cmd_type        cmd_table       [ ] =
{
    { "n",		"",		do_north,       POS_STANDING,    0,  NORMAL, NONE },
    { "e",		"",		do_east,        POS_STANDING,    0,  NORMAL, NONE },
    { "s",		"",		do_south,       POS_STANDING,    0,  NORMAL, NONE },
    { "w",		"",		do_west,        POS_STANDING,    0,  NORMAL, NONE },
    { "g`ora",		"up",		do_up,          POS_STANDING,    0,  NORMAL, NONE },
    { "d`o`l",		"down",		do_down,        POS_STANDING,    0,  NORMAL, NONE },
    { "ne",		"",		do_northeast,	POS_STANDING,	 0,  NORMAL, NONE },
    { "nw",		"",		do_northwest,	POS_STANDING,	 0,  NORMAL, NONE },
    { "se",		"",		do_southeast,	POS_STANDING,	 0,  NORMAL, NONE },
    { "sw",		"",		do_southwest,	POS_STANDING,	 0,  NORMAL, NONE },

    /*
     * Lam 10.9.98: tablica posortowana alfabetycznie.
     * Poustawialem tak, zeby polecenia skracalo sie jak najwygodniej.
     */
    { "'",		"",		do_say,         POS_RESTING,     0,  NORMAL, NONE },
    { ",",              "",		do_emote,       POS_RESTING,     0,  NORMAL, NONE },
    { ".",              "",		do_chat,        POS_SLEEPING,    0,  NORMAL, NOIC | OOC },
    { "/",		"",		do_recall,      POS_FIGHTING,    0,  NORMAL, NONE },
    { ";",              "",		do_gtell,       POS_DEAD,        0,  NORMAL, NONE },
    { "<",		"",		do_flame,       POS_SLEEPING,    0,  NORMAL, NOIC | OOC },
    { "=",              "",		do_consider,    POS_RESTING,     0,  NORMAL, NONE },
    { "?",		"",		do_help,	POS_DEAD,	 0,  NORMAL, OOC },
    { "\"",             "",		do_clan_chat,   POS_SLEEPING,    0,  NORMAL, NOIC | OOC },
    { "aukcja",		"auction",	do_auction,     POS_SLEEPING,    0,  NORMAL, NONE },
    { "aliasuj",	"alias",	do_alias,       POS_DEAD,        0,  NORMAL, OOC },
    { "atak",           "",             do_strike,      POS_STANDING,    0,  NORMAL, NONE },
    { "bipnij",		"beep",		do_beep,        POS_SLEEPING,    0,  NORMAL, OOC },
    { "balsamuj",       "",             do_balsamuj,    POS_STANDING,    0,  NORMAL, NONE },
    { "barwy",		"",		do_barwy,	POS_DEAD,	 0,  NORMAL, OOC },
    { "b`lysk",         "",             do_flash,       POS_FIGHTING,    0,  NORMAL, NONE },
    { "b`l`ad",         "bug",		do_bug,         POS_DEAD,        0,  NORMAL, OOC },
    { "bzach`eta",	"fprompt",	do_fprompt,     POS_DEAD,        0,  NORMAL, OOC },
    { "czaruj",		"cast",		do_cast,	POS_DEAD,	 0,  NORMAL, NONE },
    { "chrust",         "",             do_chrust,      POS_STANDING,    0,  NORMAL, NONE },
    { "chi",            "",             do_assemble,    POS_STANDING,    0,  NORMAL, NONE },
    { "cie`n",		"shadow form",	do_shadow,      POS_STANDING,    0,  NORMAL, NONE },
    { "credits",        "credits",	do_credits,     POS_DEAD,        0,  NORMAL, OOC },
    { "czas",           "time",		do_time,        POS_DEAD,        0,  NORMAL, OOC },
    { "czy`s`c",	"clear",	do_clear,	POS_DEAD,	 0,  NORMAL, OOC },
    { "czytaj",         "read",		do_czytaj,	POS_RESTING,     0,  NORMAL, NONE },
    { "`cwicz",		"practice",	do_practice,    POS_SLEEPING,    0,  NORMAL, OOC },
    { "daj",		"give",		do_give,        POS_RESTING,     0,  NORMAL, NONE },
    { "dealiasuj",	"unalias",	do_unalias,     POS_DEAD,        0,  NORMAL, OOC },
    { "dob`ad`x",	"",		do_wear,        POS_RESTING,     0,  NORMAL, NONE },
    { "dostosuj",	"",		do_dostosuj,	POS_STANDING,    0,  NORMAL, NONE },
    { "dzia`lanie",	"",		do_affects,     POS_DEAD,        0,  NORMAL, OOC },
    { "ekwipunek",      "equipment",	do_equipment,   POS_DEAD,        0,  NORMAL, OOC },
    { "emocja",		"emote",	do_emote,       POS_RESTING,     0,  NORMAL, NONE },
    { "adres",          "email",	do_email,       POS_DEAD,        0,  NORMAL, OOC },
    { "gaw`ed`x",	"chat",		do_chat,        POS_SLEEPING,    0,  NORMAL, NOIC | OOC },
    { "gdzie",		"where",	do_where,       POS_RESTING,     0,  NORMAL, NONE },
    { "grupa",		"group",	do_group,       POS_SLEEPING,    0,  NORMAL, NONE },
    { "gratuluj",	"grats",	do_grats,       POS_SLEEPING,    0,  NORMAL, NOIC | OOC },
    { "graj",		"",		do_graj,	POS_STANDING,	 0,  NORMAL, NONE },
    { "gm`ow",		"gtell",	do_gtell,       POS_DEAD,        0,  NORMAL, NONE },
    { "has`l",		"",		do_hasl,	POS_DEAD,	 0,  NEVER,  OOC | ALLFORCE | NOTLISTED },
    { "has`lo",		"password",	do_haslo,	POS_DEAD,	 0,  NEVER,  OOC | ALLFORCE },
    { "inwentarz",	"inventory",	do_inventory,   POS_DEAD,        0,  NORMAL, OOC },
    { "ignoruj",	"ignore",	do_ignore,	POS_DEAD,	 0,  NORMAL, OOC },
#if defined( IMUD )
    { "imud",		"imud",		do_imud,	POS_SLEEPING,	 0,  NORMAL, OOC },
#endif
    { "jedz",		"eat",		do_eat,         POS_RESTING,     0,  NORMAL, NONE },
    { "kto",            "who",		do_who,         POS_DEAD,        0,  NORMAL, OOC },
    { "kalendarz",	"",		do_kalendarz,	POS_DEAD,	 0,  NORMAL, OOC },
    { "kana`ly",	"channels",	do_channels,    POS_DEAD,        0,  NORMAL, OOC },
    { "kameleon",	"chameleon",	do_chameleon,   POS_RESTING,     0,  NORMAL, NONE },
    { "karta",		"",		do_punktacja,	POS_DEAD,        0,  NORMAL, OOC },
    { "kimjest",        "whois",	do_whois,       POS_DEAD,        0,  NORMAL, OOC },
/* Malven, 17.12.2003: klan dozwolone PP i WP, zaleznie od polecenia klanowego - sprawdzane w do_clan */
    { "klan",		"clan",		do_clan,        POS_DEAD,        0,  NORMAL, OOC },
    { "klnij",		"flame",	do_flame,       POS_SLEEPING,    0,  NORMAL, NOIC | OOC },
    { "km`ow",          "cchat",	do_clan_chat,   POS_SLEEPING,    0,  NORMAL, NOIC | OOC },
    { "kop",            "kick",		do_kick,        POS_FIGHTING,    0,  NORMAL, NONE },
    { "kolor",		"colour",	do_colour,      POS_DEAD,        0,  NORMAL, OOC },
    { "komendy",        "commands",	do_commands,    POS_DEAD,        0,  NORMAL, OOC },
    { "koncentruj",     "",             do_concentrate, POS_STANDING,    0,  NORMAL, NONE },
    { "konfiguracja",	"config",	do_config,      POS_DEAD,        0,  NORMAL, OOC },
    { "krainy",         "areas",	do_areas,       POS_DEAD,        0,  NORMAL, OOC },
    { "kradnij",	"steal",	do_steal,       POS_STANDING,    0,  NORMAL, NONE },
    { "kup",		"buy",		do_buy,         POS_STANDING,    0,  NORMAL, NONE },
    { "kumuluj",        "",             do_cumulate_strength,POS_STANDING,0, NORMAL, NONE },
    { "lista",		"list",		do_list,        POS_STANDING,    0,  NORMAL, NONE },
    { "licz",		"",		do_licz,        POS_DEAD,        0,  NORMAL, OOC },
    { "liter`owka",     "typo",		do_typo,        POS_DEAD,        0,  NORMAL, OOC },
    { "morduj",         "murder",	do_murder,      POS_FIGHTING,    0,  NORMAL, ALLFORCE },
    { "magia",		"spells",	do_spells,      POS_SLEEPING,    0,  NORMAL, OOC },
    { "maskuj",         "",             do_disguise,    POS_STANDING,    0,  NORMAL, NONE },
    { "md",		"",		do_mowdo,	POS_RESTING,	 0,  NORMAL, NONE },
    { "m`lynek",        "",             do_whirl,       POS_FIGHTING,    0,  NORMAL, NONE },
    { "moc",            "",             do_power_tech,  POS_FIGHTING,    0,  NORMAL, NONE },
    { "monitoruj",	"",		do_monitor,     POS_RESTING,     0,  NORMAL, NONE },
    { "m`ow",		"say",		do_say,         POS_RESTING,     0,  NORMAL, NONE },
    { "m`owdo",		"sayto",	do_mowdo,	POS_RESTING,	 0,  NORMAL, NONE },
    { "nakarm",		"",		do_nakarm,	POS_RESTING,	 0,  NORMAL, NONE },
    { "nape`lnij",	"fill",		do_fill,        POS_RESTING,     0,  NORMAL, NONE },
    { "nap`oj",		"",		do_napoj,	POS_RESTING,	 0,  NORMAL, NONE },
    { "nietoperz",	"",		do_nietoperz,	POS_STANDING,    0,  NORMAL, NONE },
    { "nota",           "note",		do_note,        POS_SLEEPING,    0,  NORMAL, NOIC | OOC },
    { "nowo`sci",	"",		do_nowosci,	POS_DEAD,	 0,  NORMAL, OOC },
    { "otw`orz",	"open",		do_open,        POS_RESTING,     0,  NORMAL, NONE },
    { "obejrzyj",       "examine",	do_examine,     POS_RESTING,     0,  NORMAL, NONE },
    { "oblicz",		"",		do_licz,        POS_DEAD,        0,  NORMAL, OOC },
    { "obud`x",		"wake",		do_wake,        POS_SLEEPING,    0,  NORMAL, NONE },
    { "obuch",          "",             do_stun,        POS_FIGHTING,    0,  NORMAL, NONE },
    { "oce`n",          "",             do_appraise,    POS_RESTING,     0,  NORMAL, NONE },
    { "odklucz",	"unlock",	do_unlock,      POS_RESTING,     0,  NORMAL, NONE },
    { "odkop",		"dig",		do_dig,         POS_STANDING,    0,  NORMAL, NONE },
    { "odbierz",	"",		do_odbierz,     POS_STANDING,    0,  NORMAL, NONE },
    { "odmiana",	"",		do_odmiana,	POS_DEAD,	 0,  NORMAL, OOC },
    { "odpowiedz",	"reply",	do_reply,       POS_RESTING,     0,  NORMAL, OOC },
    { "ognisko",        "",             do_ognisko,	POS_STANDING,    0,  NORMAL, NONE },
    { "okr`a`z",        "circle",	do_circle_skill,POS_FIGHTING,    0,  NORMAL, NONE },
    { "okno",		"",		do_miw,		POS_DEAD,	 0,  NORMAL, OOC },
    { "onw",		"miw",		do_miw,		POS_DEAD,	 0,  NORMAL, OOC },
    { "ook",            "afk",		do_afk,         POS_SLEEPING,    0,  NORMAL, OOC },
    { "opcje",		"config",	do_config,      POS_DEAD,        0,  NORMAL, OOC },
    { "opis",		"description",	do_description, POS_DEAD,        0,  NORMAL, OOC },
    { "ostatnie",	"",		do_ostatnie,	POS_DEAD,        0,  NORMAL, OOC },
    { "patrz",		"look",		do_look,        POS_RESTING,     0,  NORMAL, OOC },
    { "pchnij",		"push",		do_push,	POS_STANDING,    0,  NORMAL, NONE },
    { "pij",		"drink",	do_drink,       POS_RESTING,     0,  NORMAL, NONE },
    { "pomoc",          "help",		do_help,        POS_DEAD,        0,  NORMAL, OOC },
    { "pochwy`c",	"snare",	do_snare,       POS_FIGHTING,    0,  NORMAL, NONE },
    { "pod`a`zaj",	"follow",	do_follow,      POS_RESTING,     0,  NORMAL, NONE },
    { "podgl`adaj",	"",		do_podgladaj,	POS_RESTING,	 0,  NORMAL, NONE },
    { "podnie`s",	"take",		do_get,         POS_RESTING,     0,  NORMAL, NONE },
    { "pod`swietlaj",	"",		do_podswietlaj,	POS_DEAD,        0,  NORMAL, OOC },
    { "pogoda",         "weather",	do_weather,     POS_RESTING,     0,  NORMAL, NONE },
    { "pojedynek",	"",		do_pojedynek,	POS_DEAD,	 0,  NORMAL, OOC | PROG },
    { "poluj",		"hunt",		do_hunt,        POS_STANDING,    0,  NORMAL, NONE },
    { "polecenia",	"",		do_commands,    POS_DEAD,        0,  NORMAL, OOC },
    { "polskie",        "",		do_polskie,     POS_DEAD,        0,  NORMAL, OOC },
    { "po`lknij",	"quaff",	do_drink,       POS_RESTING,     0,  NORMAL, NONE },
    { "pomys`l",        "idea",		do_idea,        POS_DEAD,        0,  NORMAL, OOC },
    { "pon`ow",		"retell",	do_retell,	POS_RESTING,	 0,  NORMAL, OOC },
    { "por`ownaj",      "compare",	do_compare,     POS_RESTING,     0,  NORMAL, NONE },
    { "po`swi`e`c",	"sacrifice",	do_sacrifice,   POS_RESTING,     0,  NORMAL, NONE },
    { "postaw",		"bet",		do_postaw,	POS_STANDING,    0,  NORMAL, NONE },
    { "powiedz",	"tell",		do_tell,        POS_RESTING,     0,  NORMAL, OOC },
    { "poza",           "pose",		do_pose,        POS_RESTING,     0,  NORMAL, NONE },
    { "poziomy",	"",		do_poziomy,	POS_DEAD,	 0,  NORMAL, OOC },
    { "po`zy",          "",		do_fee,         POS_FIGHTING,    0,  NORMAL, NOTLISTED },
    { "po`zyw",         "feed",		do_feed,        POS_FIGHTING,    0,  NORMAL, NONE },
    { "przechowaj",	"",		do_przechowaj,  POS_STANDING,    0,  NORMAL, NONE },
    { "przealiasuj",	"",		do_realias,	POS_DEAD,	 0,  NORMAL, OOC },
    { "przeszukuj",     "",             do_search,      POS_STANDING,    0,  NORMAL, NONE },
    { "przywo`lanie",	"recall",	do_recall,      POS_FIGHTING,    0,  NORMAL, NONE },
    { "punktacja",      "score",	do_punktacja,	POS_DEAD,        0,  NORMAL, OOC },
/*    { "pytaj",	"question",	do_question,    POS_SLEEPING,    0,  NORMAL, NOIC | OOC }, */
    { "recytuj",	"recite",	do_recite,      POS_RESTING,     0,  NORMAL, NONE },
    { "raport",		"report",	do_report,      POS_RESTING,     0,  NORMAL, OOC },
    { "restar",		"",		do_restar,	POS_DEAD,        0,  NORMAL, OOC | ALLFORCE | NOTLISTED },
    { "restart",	"",		do_restart,	POS_DEAD,        0,  ALWAYS, OOC | ALLFORCE },
    { "rejestruj",	"register",	do_register,    POS_STANDING,    0,  NORMAL, NONE },
    { "rozbr`oj",       "disarm",	do_disarm,      POS_FIGHTING,    0,  NORMAL, NONE },
    { "rozb`lysk",      "rozblysk",	do_rozblysk,    POS_FIGHTING,    0,  NORMAL, NONE },
    { "rozdziel",	"split",	do_split,       POS_RESTING,     0,  NORMAL, NONE },
    { "rozka`z",	"order",	do_order,       POS_RESTING,     0,  ALWAYS, NONE },
    { "rozwa`z",	"consider",	do_consider,    POS_RESTING,     0,  NORMAL, NONE },
    { "runa",           "",             do_runes,       POS_FIGHTING,    0,  NORMAL, NONE },
    { "separator",	"",		do_separator,   POS_DEAD,        0,  NORMAL, OOC },
    { "skanuj",		"scan",		do_scan,        POS_RESTING,     0,  NORMAL, NONE },
/* 20.8.2003: ponizsze dwa polecenia byly od poziomu 2, ale juz nie beda
   24.3.2006: wylaczone */
#if defined( MAM_SKASUJ )
    { "skasu",		"",		do_delet,	POS_DEAD,	 0,  NORMAL, OOC | ALLFORCE | NOTLISTED },
    { "skasuj",		"delete",	do_delete,	POS_DEAD,	 0,  ALWAYS, OOC | ALLFORCE },
#else
    { "skasuj",		"delete",	do_delete_disabled,POS_DEAD,     0,  NORMAL, OOC | NOTLISTED },
#endif
    { "sklep",		"",		do_sklep,	POS_DEAD,	 0,  NORMAL, OOC },
    { "skradaj",	"sneak",	do_sneak,       POS_STANDING,    0,  NORMAL, NONE },
    { "smok",           "",             do_dragon_tech, POS_FIGHTING,    0,  NORMAL, NONE },
    { "socjalne",	"socials",	do_socials,     POS_DEAD,        0,  NORMAL, OOC },
    { "sp`ojrz",	"look",		do_look,        POS_RESTING,     0,  NORMAL, OOC },
    { "sprzedaj",	"sell",		do_sell,        POS_STANDING,    0,  NORMAL, NONE },
    { "strona",         "pagelength",	do_pagelen,     POS_DEAD,        0,  NORMAL, OOC },
    { "szepcz",		"whisper",	do_whisper,	POS_RESTING,	 0,  NORMAL, NONE },
    { "sza`l",          "berserk",	do_berserk,     POS_FIGHTING,    0,  NORMAL, NONE },
    { "sztyletuj",      "backstab",	do_backstab,    POS_STANDING,    0,  NORMAL, NONE },
    { "szybko`s`c",     "",             do_speed_tech,  POS_FIGHTING,    0,  NORMAL, NONE },
    { "`spij",		"sleep",	do_sleep,       POS_SLEEPING,    0,  NORMAL, NONE },
    { "`smier`c",       "",             do_death_tech,  POS_FIGHTING,    0,  NORMAL, NONE },
    { "`spiewaj",	"music",	do_music,       POS_RESTING,     0,  NORMAL, NONE },
    { "`swiadomo`s`c",  "",             do_awareness,   POS_STANDING,    0,  NORMAL, NONE },
    { "trzymaj",	"hold",		do_hold,        POS_RESTING,     0,  NORMAL, NONE },
    { "taranuj",	"headbutt",	do_taranuj,	POS_FIGHTING,	 0,  NORMAL, NONE },
    { "tch`orz",        "wimpy",	do_wimpy,       POS_DEAD,        0,  NORMAL, OOC },
    { "trenuj",		"train",	do_train,       POS_STANDING,    0,  NORMAL, NONE },
    { "truj",		"poison weapon",do_poison_weapon,POS_SLEEPING,   0,  NORMAL, NONE },
    { "tytu`l",		"title",	do_title,       POS_DEAD,        0,  NORMAL, OOC },
    { "ubierz",		"wear",		do_wear,        POS_RESTING,     0,  NORMAL, NONE },
    { "uciekaj",        "flee",		do_flee,        POS_FIGHTING,    0,  NORMAL, NONE },
    { "ukryj",		"hide",		do_hide,        POS_RESTING,     0,  NORMAL, NONE },
    { "umiej`etno`sci", "slist",	do_slist,       POS_DEAD,        0,  NORMAL, OOC },
    { "upu`s`c",	"drop",		do_drop,        POS_RESTING,     0,  NORMAL, NONE },
    { "ustawienia",	"",		do_config,      POS_DEAD,        0,  NORMAL, OOC },
    { "uwolnij",	"untangle",	do_untangle,    POS_FIGHTING,    0,  NORMAL, NONE },
/* uzyj to brandish i zap w jednym, ale zap to skrot od zapisz */
    { "u`zyj",		"brandish",	do_uzyj,	POS_RESTING,     0,  NORMAL, NONE },
    { "wbij",           "stake",	do_stake,       POS_STANDING,    0,  NORMAL, NONE },
    { "wciel",		"",		do_wciel,	POS_DEAD,	 0,  NORMAL, NOIC | OOC },
    { "wdzianko",	"outfit",	do_outfit,	POS_RESTING,	 0,  NORMAL, NONE },
    { "we`x",		"get",		do_get,         POS_RESTING,     0,  NORMAL, NONE },
    { "wejd`x",		"enter",	do_enter,       POS_STANDING,    0,  NORMAL, NONE },
    { "wersja",		"version",	do_version,	POS_DEAD,	 0,  NORMAL, OOC },
    { "widzialno`s`c",	"visible",	do_visible,     POS_SLEEPING,    0,  NORMAL, NONE },
    { "widmo",          "",             do_wraith_morph,POS_STANDING,    0,  NORMAL, NONE },
    { "w`lam",		"pick",		do_pick,        POS_RESTING,     0,  NORMAL, NONE },
    { "w`l`o`z",	"put",		do_put,         POS_RESTING,     0,  NORMAL, NONE },
    { "wp`lywy",	"affects",	do_affects,	POS_DEAD,	 0,  NORMAL, OOC },
    { "wp`la`c",	"deposit",	do_tobank,      POS_STANDING,    0,  NORMAL, NONE },
    { "wrzeszcz",	"shout",	do_shout,       POS_RESTING,     0,  NORMAL, NOIC | OOC },
    { "wsta`n",		"stand",	do_stand,       POS_SLEEPING,    0,  NORMAL, NONE },
/*    { "wsi`ad`x",	"",		do_wsiadz,	POS_STANDING,	 0,  NORMAL, NONE }, */
    { "w`scieknij",	"",		do_fury,	POS_FIGHTING,    0,  NORMAL, NONE },
    { "wyj`scia",	"exits",	do_exits,       POS_RESTING,     0,  NORMAL, NONE },
    { "wyci`agnij",     "",             do_pull,        POS_STANDING,    0,  NORMAL, NONE },
    { "wylej",		"pour",		do_wylej,	POS_RESTING,	 0,  NORMAL, NONE },
    { "wy`slij",	"donate",	do_donate,      POS_RESTING,     0,  NORMAL, NONE },
/*    { "wysi`ad`x",	"",		do_wysiadz,	POS_STANDING,	 0,  NORMAL, NONE }, */
    { "wybierz",	"choose",	do_change,      POS_FIGHTING,    0,  NORMAL, NONE },
    { "wyce`n",		"value",	do_value,       POS_STANDING,    0,  NORMAL, NONE },
    { "wyjd",		"",		do_qui,         POS_DEAD,        0,  NORMAL, OOC | NOTLISTED },
    { "wyjd`x",		"quit",		do_quit,        POS_DEAD,        0,  NORMAL, OOC | PROG | ORDER },
    { "wymiotuj",	"vomit",	do_vomit,	POS_FIGHTING,	 0,  NORMAL, NONE },
    { "wynajmij",	"rent",		do_rent,        POS_DEAD,        0,  NORMAL, OOC | NOTLISTED },
    { "wyostrz",	"heighten",	do_heighten,    POS_STANDING,    0,  NORMAL, NONE },
    { "wyp`la`c",	"withdraw",	do_frombank,    POS_STANDING,    0,  NORMAL, NONE },
    { "wypoczywaj",	"rest",		do_rest,        POS_SLEEPING,    0,  NORMAL, NONE },
    { "wyratuj",        "rescue",	do_rescue,      POS_FIGHTING,    0,  NORMAL, NONE },
    { "wywa`z",		"bash",		do_bash,        POS_STANDING,    0,  NORMAL, NONE },
    { "wzmocnij",       "",             do_reinforce_blade,POS_RESTING,  0,  NORMAL, NONE },
    { "krzycz",		"yell",		do_yell,        POS_RESTING,     0,  NORMAL, NONE },
    { "zabij",		"kill",		do_kill,        POS_FIGHTING,    0,  NORMAL, NONE },
    { "zach`eta",	"prompt",	do_prompt,      POS_DEAD,        0,  NORMAL, OOC },
    { "zadanie",	"quest",	do_quest,	POS_DEAD,	 0,  NORMAL, OOC },
    { "zaklucz",	"lock",		do_lock,        POS_RESTING,     0,  NORMAL, NONE },
    { "za`l`o`z",	"",		do_wear,	POS_RESTING,	 0,  NORMAL, NONE },
    { "zamknij",	"close",	do_close,       POS_RESTING,     0,  NORMAL, NONE },
    { "zapisz",		"save",		do_save,        POS_DEAD,        0,  NORMAL, OOC },
    { "zdejmij",	"remove",	do_remove,      POS_RESTING,     0,  NORMAL, NONE },
    { "zjedz",		"",		do_eat,         POS_RESTING,     0,  NORMAL, NONE },
/*  { "answer",         "answer",	do_answer,      POS_SLEEPING,    0,  NORMAL, NOIC | OOC }, */

    { "pd",		"",		do_south,       POS_STANDING,    0,  NORMAL, NONE },
    { "pd-wsch",	"",		do_southeast,	POS_STANDING,	 0,  NORMAL, NONE },
    { "pd-zach",	"",		do_southwest,	POS_STANDING,	 0,  NORMAL, NONE },
    { "pn",		"",		do_north,       POS_STANDING,    0,  NORMAL, NONE },
    { "pn-wsch",	"",		do_northeast,	POS_STANDING,	 0,  NORMAL, NONE },
    { "pn-zach",	"",		do_northwest,	POS_STANDING,	 0,  NORMAL, NONE },
    { "p`o`lnoc",	"",		do_north,       POS_STANDING,    0,  NORMAL, NONE },
    { "po`ludnie",	"",		do_south,       POS_STANDING,    0,  NORMAL, NONE },
    { "wsch`od",	"",		do_east,        POS_STANDING,    0,  NORMAL, NONE },
    { "zach`od",	"",		do_west,        POS_STANDING,    0,  NORMAL, NONE },


    /*
     * Immortal commands.
     */
    { ":",              "immtalk",	do_immtalk,     POS_DEAD,      100,  NORMAL, NOIC | OOC },
    { "bumi",		"wizhelp",	do_wizhelp,     POS_DEAD,      100,  NORMAL, OOC },

    { "bpol",		"immcmd",	do_immcmd,	POS_DEAD,      101,  NORMAL, OOC },
    { "le`cdo",		"goto",		do_goto,        POS_DEAD,      101,  NORMAL, PROG },
    { "le`cnowy",	"",		do_lecnowy,	POS_DEAD,      101,  NORMAL, PROG },
    { "nadwzrok",	"holylight",	do_holylight,   POS_DEAD,      101,  NORMAL, OOC | PROG },
    { "wskok",		"bamfin",	do_bamfin,      POS_DEAD,      101,  NORMAL, OOC | PROG },
    { "wymu`s",		"",		do_wymus,	POS_DEAD,      101,  ALWAYS, OOC | PROG },
    { "wyskok",		"bamfout",	do_bamfout,     POS_DEAD,      101,  NORMAL, OOC | PROG },

    { "rekord",		"",		do_rekord,	POS_DEAD,      102,  NORMAL, OOC | PROG },
    { "rhist",		"",		do_rhist,	POS_DEAD,      102,  NORMAL, OOC },
    { "szpicel",        "wiznet",	do_wiznet,      POS_DEAD,      102,  NORMAL, OOC | PROG },
    { "wpom",		"at",		do_at,          POS_DEAD,      102,  NORMAL, PROG },

    { ">",              "",		do_qwertalk,	POS_DEAD,      103,  NORMAL, NOIC | OOC },
    { "bniewidka",	"wizinvis",	do_invis,       POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "echo",           "",		do_echo,        POS_DEAD,      103,  ALWAYS, OOC | PROG },
    { "ilo`s`c",	"count",	do_count,	POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "kecho",		"cecho",	do_cecho,	POS_DEAD,      103,  ALWAYS, OOC | PROG },
    { "kstat",		"astat",	do_astat,       POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "mgdzie",		"mwhere",	do_mwhere,      POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "mistat",		"",		do_mistat,	POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "m`laduj",	"mload",	do_mload,       POS_DEAD,      103,  ALWAYS, PROG },
    { "mstat",          "",		do_mstat,       POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "mszukaj",	"mfind",	do_mfind,       POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "op`etaj",	"switch",	do_switch,      POS_DEAD,      103,  ALWAYS, PROG },
    { "pecho",		"recho",	do_recho,       POS_DEAD,      103,  ALWAYS, PROG },
    { "pgdzie",		"owhere",	do_owhere,      POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "p`laduj",	"oload",	do_oload,       POS_DEAD,      103,  ALWAYS, PROG },
    { "pok`oj",		"peace",	do_peace,       POS_DEAD,      103,  NORMAL, PROG },
    { "po`l`aczenia",	"users",	do_users,       POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "pomstat",	"rstat",	do_rstat,       POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "pomszukaj",	"",		do_pomszukaj,	POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "pomustaw",	"rset",		do_rset,        POS_DEAD,      103,  ALWAYS, PROG },
    { "pstat",          "ostat",	do_ostat,       POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "pszukaj",	"ofind",	do_ofind,       POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "teleciap",	"",		do_teleciap,	POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "teleecho",	"",		do_teleecho,	POS_DEAD,      103,  ALWAYS, OOC | PROG },
    { "uszukaj",	"slookup",	do_slookup,     POS_DEAD,      103,  NORMAL, OOC | PROG },
    { "wr`o`c",		"return",	do_return,      POS_DEAD,      103,  NORMAL, PROG },

    { "kwyj`scia",	"aexits",	do_aexits,	POS_DEAD,      104,  NORMAL, PROG },
    { "fstat",		"",		do_fstat,	POS_DEAD,      104,  NORMAL, OOC | PROG },
    { "mklon",		"",		do_mklon,	POS_RESTING,   104,  ALWAYS, PROG },
    { "mlista",		"",		do_mlista,	POS_DEAD,      104,  NORMAL, OOC | PROG },
    { "pklon",		"oklon",	do_oklon,	POS_RESTING,   104,  ALWAYS, PROG },
    { "plista",		"",		do_plista,	POS_DEAD,      104,  NORMAL, OOC | PROG },
    { "pomlista",	"",		do_pomlista,	POS_DEAD,      104,  NORMAL, OOC | PROG },
    { "rstat",		"",		do_racestat,	POS_DEAD,      104,  NORMAL, OOC | PROG },
    { "ulista",		"",		do_ulista,	POS_DEAD,      104,  NORMAL, OOC | PROG },

    { "zmu`s",		"force",	do_force,       POS_DEAD,      105,  ALWAYS, OOC | ALLFORCE },
    { "mustaw",		"mset",		do_mset,        POS_DEAD,      105,  ALWAYS, PROG },
    { "pdzia`l",	"oaff",		do_oaff,	POS_DEAD,      105,  ALWAYS, PROG },
    { "pustaw",		"oset",		do_oset,        POS_DEAD,      105,  ALWAYS, PROG },
    { "wymie`c",	"purge",	do_purge,       POS_DEAD,      105,  NORMAL, PROG },
    { "odn`ow",		"restore",	do_restore,     POS_DEAD,      105,  ALWAYS, PROG },
    { "zar`zni",	"",		do_sla,         POS_DEAD,      105,  NORMAL, PROG },
    { "zar`znij",	"slay",		do_slay,        POS_DEAD,      105,  ALWAYS, PROG },
    { "przenie`s",	"transfer",	do_transfer,    POS_DEAD,      105,  ALWAYS, PROG },
    { "vnumy",		"",		do_vnumy,	POS_DEAD,      105,  NORMAL, OOC | PROG },
    { "laguj",		"",		do_laguj,	POS_DEAD,      105,  ALWAYS, OOC | PROG },

    /* Lam: na 106 nie ma i tak znaczenia NOIC */
    /* Uwaga: nazwa "%" jest wpisana na sztywno w kilku miejscach, zmiana tu nie wystarczy */
    { "%",		"",		do_many,        POS_DEAD,      106,  NORMAL, OOC },
    { "-",		"",		do_godtalk,	POS_DEAD,      106,  NORMAL, OOC | PROG },
    { "+",              "",             do_oglos,       POS_DEAD,      106,  NORMAL, OOC | PROG },
    { "og`lo`s",        "",             do_oglos,       POS_DEAD,      106,  NORMAL, OOC | PROG },
#if defined( MAM_SKASUJ )
    { "ukochaj",	"",		do_love,        POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
#endif
    { "dopu`s`c",	"allow",	do_allow,       POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "banuj",		"ban",		do_ban,         POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "wyrzu`c",	"deny",		do_deny,        POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "roz`l`acz",	"disconnect",	do_disconnect,  POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "mro`zonka",	"freeze",	do_freeze,      POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "log",            "",		do_log,         POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "pami`e`c",	"memory",	do_memory,      POS_DEAD,      106,  NORMAL, OOC | PROG },
    { "bezemocji",	"noemote",	do_noemote,     POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "bezkoloru",	"",		do_bezkoloru,	POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "bezmowy",	"notell",	do_notell,      POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "blokpoziom",	"numlock",	do_numlock,     POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "bloknowi",	"newlock",	do_newlock,     POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "wybacz",		"pardon",	do_pardon,      POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "jebu",		"",		do_reboo,       POS_DEAD,      106,  NORMAL, OOC | ALLFORCE },
    { "jebut",		"reboot",	do_reboot,      POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "wy`l`ac",	"",		do_shutdow,     POS_DEAD,      106,  NORMAL, OOC | ALLFORCE },
    { "wy`l`acz",	"shutdown",	do_shutdown,    POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "ucisz",		"silence",	do_silence,     POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "pluskwa",	"snoop",	do_snoop,       POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "bkuk`la",	"",		do_bkukla,      POS_DEAD,      106,  NORMAL, OOC | PROG },
    { "uustaw",		"sset",		do_sset,        POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "jebustaw",	"sstime",	do_sstime,      POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "bloknie`sm",	"wizlock",	do_wizlock,     POS_DEAD,      106,  ALWAYS, OOC | PROG },
    { "odm`ow",		"",		do_refuse,	POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "odwampirz",      "",             do_odwampirz,   POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },
    { "kogobi`c",	"",		do_kogobic,	POS_DEAD,      106,  ALWAYS, OOC | ALLFORCE },

    { "awans",		"advance",	do_advance,     POS_DEAD,      108,  ALWAYS, OOC | ALLFORCE },
    { "kzapisz",	"asave",	do_asave,	POS_DEAD,      108,  ALWAYS, ALLFORCE },
    { "do`swiadcz",	"exp",		do_exp,		POS_DEAD,      108,  ALWAYS, OOC | ALLFORCE },
    { "g`laduj",	"",		do_gladuj,	POS_DEAD,      108,  ALWAYS, ALLFORCE },
    { "imster",		"imctl",	do_imctl,	POS_DEAD,      108,  ALWAYS, OOC | ALLFORCE },
    { "info",		"",		do_info,        POS_DEAD,      108,  ALWAYS, OOC | ALLFORCE },
    { "bumustaw",	"imtlset",	do_imtlset,     POS_DEAD,      108,  ALWAYS, OOC | PROG },
    { "przemianuj",	"rename",	do_rename,	POS_DEAD,      108,  ALWAYS, OOC | ALLFORCE },
    { "stopie`n",	"",		do_stopien,	POS_DEAD,      108,  ALWAYS, OOC | ALLFORCE },
    { "test",		"",		do_test,	POS_DEAD,      108,  ALWAYS, OOC | ALLFORCE },
    { "zaufaj",		"trust",	do_trust,       POS_DEAD,      108,  ALWAYS, OOC | ALLFORCE },
    { "prze`laduj",	"",		do_przeladuj,	POS_DEAD,      108,  ALWAYS, OOC | ALLFORCE },


    /*
     * MOBprogram commands.
     */
    { "mpstat",         "",		do_mpstat,	POS_DEAD,      103,  NORMAL, OOC },
    { "mpodpluskw",	"",		do_mpodpluskw,	POS_DEAD,      104,  ALWAYS, OOC },
    { "pompstat",       "rpstat",	do_rpstat,	POS_DEAD,      103,  NORMAL, OOC },
    { "ppstat",         "opstat",	do_opstat,	POS_DEAD,      103,  NORMAL, OOC },
    { "wpstat",         "epstat",	do_epstat,	POS_DEAD,      103,  NORMAL, OOC },
    { "zadstat",        "",             do_zadstat,     POS_DEAD,      103,  NORMAL, OOC },
    { "zadustaw",       "",             do_zadustaw,    POS_DEAD,      108,  ALWAYS, OOC },

    /*
     * End of list.
     */
    { "",               "",		0,              POS_DEAD,        0,  NORMAL, NONE }
};



/*
 * Zabawka dla ludzi, ktorzy sie nudza.
 * Zrezygnowalem z tego... nie przyjelo sie i tyle.
 */
/*
void nuda( CHAR_DATA *ch )
{
    const char *teksty[ 16 ] =
    {
	"Chcesz mi co`s powiedzie`c?\n\r",
	"Nudzi ci si`e?\n\r",
	"Czemu niczego nie m`owisz?\n\r",
	"Czemu niczego nie wklepiesz?\n\r",
	"No wys`l`ow si`e!\n\r",
	"Namy`sl si`e - grasz czy si`e patrzysz?\n\r",
	"Nie masz mi niczego do powiedzenia?\n\r",
	"Wysil si`e i napisz co`s czasem!\n\r",

	"Connection closed by foreign host. (`Zartuj`e)\n\r",
	"Od bezsensownego klepania w Enter nabawisz si`e chor`ob nadgarstk`ow!\n\r",
	"Je`sli chcesz mi co`s przekaza`c, napisz to!\n\r",
	"Napisz co`s wreszcie, bo zaczynam si`e nudzi`c.\n\r",
	"Wyci`ag z instrukcji obs`lugi muda:\n\r\
	\"Je`sli chcesz co`s zrobi`c, napisz to PRZED wci`sni`eciem Entera!\"\n\r",
	"Daj wytchn`a`c biednemu Enterowi, u`zywaj te`z innych klawiszy...\n\r",
	"Nie masz ju`z pomys`lu na gr`e, prawda? Lepiej klepa`c w Enter bez sensu?\n\r",
	"Machnij par`e literek, po co wciskasz sam Enter?\n\r"
    };

    send_to_char( teksty[ number_bits( 4 ) ], ch );

    return;
}
*/


void zastepstwo( CHAR_DATA *ch, int numer )
{
    const char * const komunikaty[ 16 ] =
    {
	"Polecenie \"%s\" zosta`lo usuni`ete. W zamian napisz \"%s\".\n\r",
	"Polecenie \"%s\" zosta`lo zabite! Na szcz`e`scie wiesz, `ze trzeba napisa`c \"%s\".\n\r",
	"Polecenie \"%s\" nie istnieje. Co powiesz na \"%s\" w zamian?\n\r",
	"Zamiast \"%s\" proponuj`e ci napisanie \"%s\".\n\r",
	"Podejrzewam `ze \"%s\" nie zadzia`la, ale \"%s\" powinno.\n\r",
	"Polecenie \"%s\" zosta`lo drog`a selekcji naturalnej zast`apione przez \"%s\".\n\r",
	"Ech... Nie pisz ju`z \"%s\", teraz wszyscy pisz`a \"%s\". Nie b`ad`x gorszy!\n\r",
	"Polecenie \"%s\" zosta`lo wyparte przez \"%s\".\n\r",

	"M`ow`ze po ludzku! Mo`ze nie chodzi ci o polecenie \"%s\" tylko o \"%s\"...?\n\r",
	"Najwy`zszy czas, `zeby zamiast \"%s\" zacz`a`c pisa`c \"%s\".\n\r",
	"Czy nie s`adzisz `ze \"%s\" brzmi gorzej ni`z \"%s\"?\n\r",
	"Nie pisz ju`z \"%s\", bo tylko \"%s\" mo`ze tu pom`oc.\n\r",
	"Czy wci`a`z nie wiesz, `ze polecenie \"%s\" jest archaizmem zast`apionym przez \"%s\"?\n\r",
	"Znowu zapominasz, `ze \"%s\" ust`api`lo miejsca \"%s\"?\n\r",
	"Czy masz trudno`sci z zapami`etaniem, `ze zamiast \"%s\" pisze si`e \"%s\"?\n\r",
	"Przesta`n si`e myli`c i zamiast \"%s\" pisz \"%s\".\n\r"
    };

    ch_printf( ch, komunikaty[ number_bits( 4 ) ],
		cmd_table[ numer ].old_name,
		cmd_table[ numer ].name );

    return;
}


/*
 * Funkcja ta zwraca wskaznik do slowa o numerze 'ktory' lub NULL, jesli jest
 * mniej slow lub ktory == 0 (pierwszy wyraz na numer 1). Rozumie cudzyslowy
 * i apostrofy ('"' i '''), wiec wyrazy w nich zawarte traktowane sa jako jedno
 * slowo. Zajmuje nowa pamiec dla znalezionego slowa, wiec po uzyciu trzeba
 * zwolnic pamiec!
 */
char *ity_argument( const char *argument, int ktory )
{
    char granica, *wynik = NULL, *tymczas = NULL;
    int i = 1;

    /* jesli argument jest pusty */
    if ( !argument || argument[ 0 ] == '\0' )
	return NULL;

    /* petla przechodzi po kolejnych wyrazach */
    while ( i <= ktory )
    {
	/* pomija spacje */
	while ( isspace( (int) *argument ) )
	    argument++;

	/* ustala ogranicznik (czy spacja, czy '"' lub ''') */
	granica = ' ';

	if ( *argument == '\'' || *argument == '"' )
	    granica = *argument++;

	/* zajmuje pamiec, jesli to odpowiedni wyraz */
	if ( i == ktory )
	    tymczas = wynik = (char *) malloc( strlen( argument ) + 1 );

	/* przebiega po wyrazie */
	while ( *argument != '\0' && *argument != granica )
	{
	    /* kopiuje, jesli to ten wyraz */
	    if ( i == ktory )
		*wynik++ = *argument;
	    argument++;
	}

	/* zakancza wyraz, przycina pamiec i zwraca */
	if ( i == ktory )
	{
	    *wynik = '\0';
	    wynik = (char *) realloc( tymczas, strlen( tymczas ) + 1 );
	    return wynik;
	}

	/* jesli to ostatni wyraz */
	if ( *argument++ == '\0' || *argument == '\0' )
	    return NULL;

	/* zwieksza licznik wyrazow */
	i++;
    }

    /* jesli ktory == 0 (wiec i > ktory), zwraca NULL */
    return NULL;
}


/*
 * Funkcja zwraca wskaznik do czesci 'argumentu' znajdujacego sie za slowem
 * o numerze 'ktory' lub NULL, jesli jest mniej slow. Dla ktory == 0 zwraca caly
 * argument. Rozumie cudzyslowy i apostrofy ('"' i '''), wiec wyrazy w nich
 * zawarte traktowane sa jako jedno slowo.
 */
char *po_itym_arg( const char *argument, int ktory )
{
    char granica;
    int i = 1;

    /* jesli argument jest pusty */
    if ( !argument || !*argument )
	return NULL;

    /* petla ta przebiega po kolejnych wyrazach */
    while ( i <= ktory )
    {
	/* pomija spacje */
	while ( isspace( (int) *argument ) )
	    argument++;

	/* ustala ogranicznik slowa (spacja, '"' lub ''') */
	granica = ' ';

	if ( *argument == '\'' || *argument == '"' )
	    granica = *argument++;

	/* przechodzi po wyrazie */
	while ( *argument != '\0' && *argument != granica )
	    argument++;

	/* jesli koniec argumentu, zwraca NULL */
	if ( *argument++ == '\0' || *argument == '\0' )
	    return NULL;

	/* jesli to ten wyraz, to koniec petli */
	if ( i == ktory )
	    break;

	/* zwieksza licznik wyrazow */
	i++;
    }

    /* pomija spacje */
    while ( isspace( (int) *argument ) )
	argument++;

    /* zwraca wskaznik (rzutowanie, zeby uniknac ostrzezen kompilatora) */
    return (char *) argument;
}


/*
 * Alandar: ta funkcja sprawdza ktore parametry wystepuja w aliasie i jaki jest
 * nawyzszy uzyty parametr. Modyfikuje tablice int, na miejscach 1-9 ma 0 lub 1
 * w zaleznosci od tego, czy parametr wystapil czy nie, na miejscu 0 ma numer
 * najwyzszego.
 */
void sprawdz_parametry( const char *alias, int *parametry )
{
    int i;

    /* zeruje tablice parametrow */
    for ( i = 0; i < 10; i++ )
	parametry[ i ] = 0;

    /* petla przebiega caly ciag */
    while ( *alias != '\0' )
	/* jesli po '$' wytapi cyfra, zapisuje 1 w tablicy */
	if ( *alias++ == '$' && isdigit( (int) *alias ) )
	    parametry[ *alias - '0' ] = 1;

    /* sprawdza jaki jest najwyzszy uzyty parametr */
    for ( i = 1; i < 10; i++ )
	if ( parametry[ i ] == 1 )
	    /* zapisuje jego numer na pozycji 0 */
	    parametry[ 0 ] = i;

    return;
}


const CMD_TYPE *znajdz_polecenie( char *argument, int poziom )
{
    int cmd;

    for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	if ( !str_prefix( argument, cmd_table[ cmd ].name )
	  && cmd_table[ cmd ].level <= poziom )
	{
	    return &cmd_table[ cmd ];
	}

    return NULL;
}


/*
 * Zeby nie powtarzac w 3 miejscach
 */
void nowe_instrukcje( CHAR_DATA *ch )
{
    if ( !ch || !ch->desc )
	return;

    if ( strlen( ch->desc->inbuf ) >= BUF_LIMIT_INBUF )
	bug( "NOWE_INSTRUKCJE, strlen( ch->desc->inbuf ) = %d", strlen( ch->desc->inbuf ) );

    /* nie wiem czy zawsze tak moge */
    ch->desc->incomm[ 0 ] = '\0';
    read_from_buffer( ch->desc, FALSE );
    if ( ch->desc->incomm[ 0 ] )
	interpret( ch, ch->desc->incomm, INTERP_NORMAL );

    /* Sprawdzenie, czy ch->desc jest konieczne, bo opetywanie zmienia ch->desc,
       zeby pokazywal na nowa postac, nowa postac ma ustawiany nch->desc na
       ch->desc, a ch->desc ma NULL. Odkryte przez Grzecha, ktory napisal alias
       opetujacy moba i rzucajacy nim nieprzyjemne czary na gracza. Alias nie
       dzialal, wywalajac muda. */
    if ( ch->desc )
	ch->desc->incomm[ 0 ] = '\0';

    return;
}


/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 * Lam 29.5.98: wprowadzenie int zastos do przyszlych zabezpieczen
 *     16.6.98: wprowadzenie int restr w tablicy, to te zabezpieczenia
 *     11.2.99: rprog_do_trigger
 */
bool interpret( CHAR_DATA *ch, char *argument, int zastos )
{
    char 	command[ MIL ];
    char 	logline[ MIL ];
    char 	buffer [ MSL ];
    char	bufor  [ BUF_LIMIT_INBUF ];
    int  	cmd = 0; /* Lam: pozbywam sie ostrzezenia kompilatora */
    int  	trust;
    bool 	found;
    int  	count;
    int  	count_buf;
    bool 	byl_srednik;
    ALIAS_DATA *alias = NULL;
    bool 	dont_alias = FALSE;
    bool 	found_alias = FALSE;

    while ( isspace( (int) *argument ) )
	argument++;

    if ( argument[ 0 ] == '\0' )
    {
/*        nuda( ch ); */
	return FALSE;
    }

    /* Lam: & powoduje, ze aliasy nie sa sprawdzane */
    if ( *argument == '&' )
    {
	dont_alias = TRUE;
	argument++;
    }

    /* Lam 22.5.2000: tylko te dwa przypadki moga wykonac alias, inne nie! */
    if ( zastos != INTERP_NORMAL && zastos != INTERP_FORCE )
	dont_alias = TRUE;

    /*
     * Strip leading spaces.
     */
    while ( isspace( (int) *argument ) )
	argument++;

    if ( argument[ 0 ] == '\0' )
	return FALSE;

    /*
     * No hiding and remove AFK
     */

    /* Bit HIDE teraz obslugiwany nizej i inaczej. */
/*    REMOVE_BIT( ch->affected_by, AFF_HIDE ); */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AFK ) )
    {
	TELL_DATA *tell;

	REMOVE_BIT( ch->act, PLR_AFK );
	send_to_char( "{YPowracasz przed klawiatur`e.{x\n\r", ch );

	if ( ch->tell )
	{
	    send_to_char( "{ROto, co si`e nagra`lo na sekretarce:{x\n\r", ch );

	    for ( tell = ch->tell; tell; tell = tell->next )
		send_to_char( tell->tell, ch );

	    /* usuniecie wyswietlonych wiadomosci */
	    while ( ch->tell )
		del_tell( ch, ch->tell );
	}
	else
	    send_to_char( "{GPodczas twej nieobecno`sci nikt do ciebie niczego nie m`owi`l.{x\n\r", ch );

	send_to_char( "\n\r", ch );
    }

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_FREEZE ) )
    {
	ASTC( "Jeste`s zamro`zon$y!", ch );
	return FALSE;
    }

    /* Lam: uwaga, oto jedna z dwoch rzeczy ktore napisal dla nas Albi! :) */
    /*
     * Implement multiple command - by Albi
     */
    /* Lam 7.5.98: ponizszy warunek. Jesli gracz nie chce normalnych
       separatorow, to ich nie ma, ale w aliasach mu zostaja. % tez je
       ignoruje. */
    if ( argument[ 0 ] != '%' && ch->desc
      && ( !CZY_OPCJA( ch, OPC_NO_SEPARATOR ) ) )
    {
	count_buf = 0;
	byl_srednik = FALSE;
	bufor[ 0 ] = '\0';

	if ( dont_alias )
	{
	    buffer[ 0 ] = '&';
	    count_buf = 1;
	}

	for ( count = 0; argument[ count ] != '\0'; count++ )
	    if ( argument[ count ] != ch->separator )
	    {
		buffer[ count_buf ] = argument[ count ];
		count_buf++;
	    }
	    else
	    {
		byl_srednik = TRUE;
		buffer[ count_buf ] = '\n';
		buffer[ count_buf + 1 ] = '\0';
		strcat( bufor, buffer );
		count_buf = 0;
	    }

	if ( byl_srednik )
	{
	    buffer[ count_buf ] = '\n';
	    buffer[ count_buf + 1 ] = '\0';
	    strcat( bufor, buffer );
	    /* tu bufor moze miec najwyzej MIL */
	    if ( strlen( bufor ) + strlen( ch->desc->inbuf ) >= BUF_LIMIT_INBUF )
	    {
		STC( "Nie mog`e doda`c do bufora.\n\r", ch );
		return FALSE;
	    }
	    strcat( bufor, ch->desc->inbuf );
	    strcpy( ch->desc->inbuf, bufor );
	    nowe_instrukcje( ch );
	    return TRUE;
	}
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    /* osobiste dzienniki graczy, podgladanie graczy */
    strcpy( logline, argument );

    /* przypadek: komenda zaczyna sie od kropek, przecinkow itp. */
    /* Lam 10.9.98: wyjatek stanowi ` - od tego zaczynaja sie polecenia, jesli
       pierwsza litera jest polska (spij, cwicz) */
    if ( !isalpha( (int) argument[ 0 ] ) && !isdigit( (int) argument[ 0 ] )
		&& argument[ 0 ] != '&' && argument[ 0 ] != '`' )
    {
	command[ 0 ] = argument[ 0 ];
	command[ 1 ] = '\0';
	argument++;
	while ( isspace( (int) *argument ) )
	    argument++;
    }
    else

    /* przypadek: szybkie chodzenie np. 8e = e|e|e|e|e|e|e|e */
    if ( ch->desc /* moby! */
      && isdigit( (int) argument[ 0 ] )
      && argument[ 2 ] == '\0'
      && argument[ 0 ] != '0'
      && ( argument[ 1 ] == 'e' || argument[ 1 ] == 'w'
	|| argument[ 1 ] == 'n' || argument[ 1 ] == 's'
	|| argument[ 1 ] == 'g' || argument[ 1 ] == 'd' ) )
    {
	char tymczas[ MAX_STRING_LENGTH ];
	char tymtim[ MAX_STRING_LENGTH ];
	int  tymtom;
	int  tymtam;

	tymczas[ 0 ] = '\0';
	tymtim[ 0 ] = argument[ 0 ];
	tymtim[ 1 ] = '\0';
	tymtom = atoi( tymtim );

	for ( tymtam = 0; tymtam < tymtom; tymtam++ )
	{
	    /* Lam 25.5.99: dodalem '&' przed kierunek */
	    sprintf( tymtim, "&%c\n", argument[ 1 ] );
	    strcat( tymczas, tymtim );
	}

	if ( strlen( tymczas ) + strlen( ch->desc->inbuf ) >= BUF_LIMIT_INBUF )
	{
	    STC( "Nie mog`e doda`c do bufora.\n\r", ch );
	    return FALSE;
	}
	strcat( tymczas, ch->desc->inbuf );
	strcpy( ch->desc->inbuf, tymczas );
	nowe_instrukcje( ch );
	return TRUE;
    }
    else
        /* przypadek: cala reszta, czyli komenda, potem spacja, potem argumenty */
	argument = one_argument( argument, command );

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );

    /* Lam: wykonanie aliasu */
    if ( ch->desc && !dont_alias && ( alias = alias_lookup( ch, command ) ) )
    {
	char tymczas[ MSL ];
	char *pbuf;
	char *palias;
	int dlugosc;
	bool proc;

	pbuf = tymczas;
	palias = alias->todo;
	proc = FALSE;

	/* boska umiejetnosc '%' jest traktowana specjalnie */
	if ( *palias == '%' )
	{
	    palias++;
	    *pbuf = '%';
	    pbuf++;
	    *pbuf = '&';
	    proc = TRUE;
	}
	else
	    /* & zapobiega zapetleniu sie aliasu */
	    *pbuf = '&';

	pbuf++;

	/* ta petla kopiuje alias do inbufa, zapobiegajac zapetlaniu sie aliasow */
	for ( ; ; )
	{
	    /* \r wstawiam w alias, zeby wyswietlac, a teraz pomijam */
	    if ( *palias == '\r' )
	    {
		palias++;
		continue;
	    }

	    *pbuf = *palias;

	    /* Lam 26.9.2004: zeby strlen( ) ponizej dzialalo */
	    *( pbuf + 1 ) = '\0';

	    dlugosc = strlen( tymczas );

	    /* po '\n' i separatorze bedzie nastepna komenda. Nie chcemy, zeby
	       mogl byc wykonany alias (zapetlanie sie) */
	    if ( *pbuf == '\n' || ( *pbuf == ch->separator && !proc ) )
	    {
		*pbuf++ = '\n';

		/* na skroty: pominiecie \n\r\n\r, || i kombinacji. */
		while ( *palias == '\n' || *palias == '\r'
		     || *palias == ch->separator )
		    palias++;

		/* znow sprawdzam komende % - to wazne, jesli ktokolwiek tego
		   kiedykolwiek uzyje */
		proc = FALSE;
		if ( *palias == '%' )
		{
		    palias++;
		    *pbuf = '%';
		    pbuf++;
		    proc = TRUE;
		}
		palias--;
		*pbuf = '&';
	    }

	    /* Alandar 27.12.2002: aliasy wieloparametrowe */
	    else if ( *pbuf == '$' && isdigit( (int) *( palias + 1 ) ) && dlugosc < 2000 )
	    {
		char *parametr;

		/* $0 zawiera wszystkie nieuzyte parametry */
		if ( *++palias == '0' )
		{
		    int parametry[ 10 ];
		    int i;
		    bool byl = FALSE;

		    /* sprawdz_parametry( ) nie zmieni alias->todo */
		    sprawdz_parametry( alias->todo, parametry );

		    /* petla przechodzi po wszystkich argumentach */
		    for ( i = 1; i < parametry[ 0 ]; i++ )
		    {
			/* jesli parametr nie byl uzyty i jest niepusty, to go
			   kopiuje */
			if ( parametry[ i ] == 0 && ( parametr = ity_argument( argument, i ) ) != NULL )
			{
			    char *poczatek = parametr;

			    /* oddziela parametry spacja */
			    if ( byl == TRUE )
				*pbuf++ = ' ';
			    else
				byl = TRUE;

			    while ( *parametr != '\0' )
				*pbuf++ = *parametr++;

			    /* funkcja ity_argument( ) zajmuje pamiec, wiec tu
			       ja zwalniam */
			    free( poczatek );
			}
		    }

		    /* tu kopiuje koncowke za ostatnim uzytym argumentem */
		    if ( ( parametr = po_itym_arg( argument, i ) ) != NULL )
		    {
			if ( byl == TRUE )
			    *pbuf++ = ' ';

			while ( *parametr != '\0' )
			    *pbuf++ = *parametr++;
		    }
		}
		/* jesli to nie $0, po prostu kopiujemy odpowiedni argument */
		else if ( ( parametr = ity_argument( argument, *palias - '0' ) ) != NULL )
		{
		    char *poczatek = parametr;

		    while ( *parametr != '\0' )
			*pbuf++ = *parametr++;

		    /* ity_argument( ) zajmuje pamiec, wiec tu ja zwalniam */
		    free( poczatek );
		}
		*pbuf = '\0';
		pbuf--;
	    }
	    /* koniec fragmentu "aliasy wieloparametrowe" */
	    else if ( *pbuf == '$' && dlugosc < 2000 )
	    {
		char *old_argum;

		old_argum = argument;
		if ( *argument != '\0' )
		    for ( ; ; )
		    {
			*pbuf = *argument;
			pbuf++;
			argument++;
			if ( *argument == '\0' ) break;
		    }
		pbuf--;
		argument = old_argum;
	    }

	    /* zeby nastepna komenda po aliasie nie zaczynala sie od & - wtedy
	       nie mozna byloby wykonac 2 aliasow pod rzad */
	    if ( *pbuf == '\0' )
	    {
		if ( pbuf > tymczas && *( pbuf - 1 ) == '&' )
		{
		    pbuf--;
		    *pbuf = '\0';
		}
		break;
	    }
	    pbuf++;
	    palias++;
	}
	/* dokopiowanie starego inbufa i wrzucenie tego wszystkiego razem do
	   nowego inbufa, z ktorego bedzie to nastepnie brane. */
	/* linia odpluskwiajaca - odkomentuj w razie potrzeby */
	/* send_to_char( tymczas, ch ); */
	if ( strlen( tymczas ) + strlen( ch->desc->inbuf ) >= BUF_LIMIT_INBUF )
	{
	    ch_printf( ch, "Nie mog`e rozwin`a`c aliasu \"%s\" do bufora.\n\r", command );
	    return FALSE;
	}
	strcat( tymczas, ch->desc->inbuf );
	strcpy( ch->desc->inbuf, tymczas );

	found_alias = TRUE;
    }
    /* koniec napisanej przez Lama czesci dot. wykonania aliasu */
    else
	for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	{
	    if ( !str_prefix( command, cmd_table[ cmd ].name )
		&& cmd_table[ cmd ].level <= trust
		/* ograniczenia, zabezpieczenia */
		&& ( IS_SET( cmd_table[ cmd ].restr, FORCE ) ?
			     ( zastos != INTERP_FORCE
			    && zastos != INTERP_MPFORCE
			    && zastos != INTERP_ORDER ) : TRUE )
		&& ( !IS_SET( cmd_table[ cmd ].restr, PROG )
		  || zastos < INTERP_MPROG ) )
		/* Lam: jednak nie, bo za duze bylyby roznice w poleceniach i skrotach
		&& ( !IS_SET( cmd_table[ cmd ].restr, NOIC )
		  || PP( ch ) )
		&& ( IS_SET( cmd_table[ cmd ].restr, OOC )
		  || WP( ch ) ) ) */
	    {
		found = TRUE;
		break;
	    }
	}

    /* skasowanie danej linii \E[2K, przejscie na poczatek \E[1G */
    /* jednak \E[1G nie zawsze dziala (kolejny raz CUTE...), wiec jest tak: */
#if !defined( MSDOS )
    if ( dont_alias && ch->desc )
	write_to_buffer( ch->desc, ESCS "[100D" ESCS "[2K", 0 );
#endif

    if ( !found_alias && found )
    {
	if ( IS_SET( cmd_table[ cmd ].restr, NOIC )
	    && WP( ch ) && ch->level < 106 )
	{
	    STC( "Nie mo`zesz wykonywa`c tego polecenia w postaci.\n\r", ch );
	    return FALSE;
	}

	if ( !IS_SET( cmd_table[ cmd ].restr, OOC )
	  && PP( ch ) )
	{
	    STC( "Nie mo`zesz wykonywa`c tego polecenia poza postaci`a.\n\r", ch );
	    return FALSE;
	}
    }

    /*
     * Log and snoop.
     */
    /* co alias, to nie komenda */
    if ( !found_alias )
    {
	CHAR_DATA *och = ( ch->desc && ch->desc->original ) ?
				ch->desc->original : ch;

	if ( cmd_table[ cmd ].log == NEVER )
	    strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

	if ( ( !IS_NPC( och ) && IS_SET( och->act, PLR_LOG ) )
	  || fLogAll
	  || cmd_table[ cmd ].log == ALWAYS )
	{
	    sprintf( log_buf, "Log %s: %s", ch->name, logline );
	    log_string_ch( ch, log_buf );
	}

	if ( cmd_table[ cmd ].log != NEVER )
	{
/*	    sprintf( log_buf, "{G[{g*{G] {Y$N{x: %s", logline ); */
	    wiznet( "{G[{g*{G] {Y$N{x: $t", ch, (OBJ_DATA *) logline,
			(void *) &cmd_table[ cmd ], WIZ_IMMCMD,
			0, get_trust( ch ) );
	}
    }

    if ( ch->desc && ch->desc->snoop_by )
    {
	sprintf( log_buf, "%%%s%%> ", ch->name );
	write_to_buffer( ch->desc->snoop_by, log_buf, 0 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    /* Lam 25.5.99: mam nadzieje, ze nie ma mozliwosci zapetlenia. Teraz funkcja
       nowe_instrukcje wystepuje tu i w dwoch innych miejscach. Mozliwa
       rekurencja, ale tylko 3 poziomy wglab. */
    if ( found_alias )
    {
	nowe_instrukcje( ch );
	return TRUE;
    }

    if ( !found )
    {
	if ( CZY_OPCJA( ch, OPC_ANGIELSKIE ) )
	    /* szukanie angielskiego odpowiednika (skopiowane z normalnego szukania) */
	    for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
		if ( command[ 0 ] == cmd_table[ cmd ].old_name[ 0 ]
		  && !str_prefix( command, cmd_table[ cmd ].old_name )
		  && cmd_table[ cmd ].level <= trust
		/* ograniczenia, zabezpieczenia */
		  && ( IS_SET( cmd_table[ cmd ].restr, FORCE ) ?
		       ( zastos != INTERP_FORCE
		      && zastos != INTERP_MPFORCE
		      && zastos != INTERP_ORDER ) : TRUE )
		  && ( IS_SET( cmd_table[ cmd ].restr, PROG ) ?
					zastos < INTERP_MPROG : TRUE )
		  && ( IS_SET( cmd_table[ cmd ].restr, ORDER ) ?
					zastos != INTERP_ORDER : TRUE ) )
		{
		    zastepstwo( ch, cmd );
		    return FALSE;
		}

	/* jesli nie ma takiej komendy, mimo wszystko moze byc do_prog, a jesli
	   do_proga tez nie ma, pewnie jest polecenie socjalne */
	if ( !rprog_do_trigger( ch, command, argument, TRUE ) )
	    if ( !ch->deleted
	      && ch->in_room
	      && !check_social( ch, command, argument ) )
	    {
		ch_printf( ch, "Nie znam polecenia \"%s\".\n\r", command );
		return FALSE;
	    }

	return TRUE;
    } /* if ( !found ) */

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[ cmd ].position )
    {
	switch ( ch->position )
	{
	case POS_DEAD:
	    STC( "Le`z spokojnie, przecie`z nie `zyjesz!\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    ASTC( "Jeste`s tak rann$y, `ze nie jeste`s w stanie tego zrobi`c.", ch );
	    break;

	case POS_STUNNED:
	    ASTC( "Jeste`s na to zbyt poharatan$y.", ch );
	    break;

	case POS_SLEEPING:
	    STC( "Podczas snu?\n\r", ch );
	    break;

	case POS_RESTING:
	    ASTC( "Nie... Jeste`s zbyt odpr`e`zon$y... Najpierw przesta`n wypoczywa`c.", ch );
	    break;

	case POS_FIGHTING:
	    STC( "No co ty, przecie`z ci`agle walczysz!\n\r", ch );
	    break;

	}
	return FALSE;
    }

    /* Lam 7.5.98 */
    if ( WP( ch )
      && IS_AFFECTED( ch, AFF_HIDE )
      && cmd_table[ cmd ].position != POS_DEAD )
    {
	send_to_char( "Przestajesz si`e ukrywa`c.\n\r", ch );
	REMOVE_BIT_V( ch->affected_by, AFF_HIDE );
    }

    /* Moze byc do_prog taki sam jak polecenie.
       Lam 16.4.2000: mpforce umozliwia wykonanie oryginalnego polecenia (np.
       w krainie Qwerta rpstat zablokowany jest dla wszystkich, ale mnie mozna
       zmusic do zrobienia oryginalnego rpstat). Oczywiscie nie da sie w ten
       sposob odblokowac polecen typu 'awansuj'. */
    if ( zastos != INTERP_MPFORCE
      && rprog_do_trigger( ch, cmd_table[ cmd ].name, argument, FALSE ) )
	return TRUE;

    if ( ch->deleted )
	return FALSE;

    /* Gimza: sprawdzam czy komenda nie jest zablokowana */
    if ( refuse_lookup( ch, cmd_table[ cmd ].name ) )
    {
	send_to_char( "Jaka`s wielka si`la sprawia, `ze nie mo`zesz tego uczyni`c.\n\r", ch );
	return FALSE;
    }

    /* dopiero po tym wszystkim mozemy wykonac funkcje polecenia */
    ( *cmd_table[ cmd ].do_fun )( ch, argument );

    tail_chain( );

    return TRUE;
}


ALIAS_DATA *alias_lookup( CHAR_DATA *ch, char *arg )
{
    ALIAS_DATA *alias;

    if ( !arg || !*arg )
    {
	bug( "alias_lookup: brak argumentu", 0 );
	return NULL;
    }

    if ( !ch->alias )
	return NULL;

    for ( alias = ch->alias; alias; alias = alias->next )
    {
	if ( !alias->name )
	{
	    bug( "alias_lookup: pusty alias", 0 );
	    return NULL;
	}

	if ( !*alias->name )
	{
	    bug( "alias_lookup: alias z pusta nazwa", 0 );
	    return NULL;
	}

	if ( !str_cmp( arg, alias->name ) )
	    return alias;
    }

    return NULL;
}


/*
 * Lam 2.6.99: przepuscilem liste aliasow przez bufor stron
 */
KOMENDA( do_alias )
{
    char       arg [ MIL ];
    char       buf1[ MSL * 2 ];
    char       buf [ MSL * 2 ];
    char       buf2[ MIL ];
    /* buf byl max_input_length do 12.5.98 */
    ALIAS_DATA *alias;
    ALIAS_DATA *alias2;
    int        i;

    smash_tilde( argument );
    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	if ( !ch->alias )
	{
	    send_to_char( "Nie zdefiniowano `zadnych alias`ow.\n\r", ch );
	    return;
	}

	strcpy( buf1, "Zdefiniowano aliasy:\n\r" );
	for ( alias = ch->alias; alias; alias = alias->next )
	{
	    sprintf( buf2, "%s: ", wyrownaj( alias->name, 12 ) );
	    for ( i = 0; alias->todo[ i ]
		      && alias->todo[ i ] != '\n'
		      && alias->todo[ i ] != '\r'
		      && strlen_pl( buf2 ) + 1 < 74; i++ )
		sprintf( buf2, "%s%c", buf2, alias->todo[ i ] );
	    if ( alias->todo[ i ] && ( alias->todo[ i ] != '\n'
	      || alias->todo[ i + 2 ] != '\0' ) )
		strcat( buf2, " (...)" );

	    /* 130 ma wystarczyc na tekst, ze za duzo i "Napisz alias..." */
	    if ( strlen( buf1 ) + strlen( buf2 ) + 130 > MSL * 2 )
	    {
		strcat( buf1, "  (za du`zo alias`ow!)\n\r" );
		break;
	    }
	    sprintf( buf, "%s\r\n", buf2 );
	    strcat( buf1, buf );
	}

	strcat( buf1,
		"Napisz alias <nazwa aliasu>, aby dowiedzie`c si`e, co dany alias robi.\n\r" );
	send_to_char_bw( buf1, ch );
	return;
    }

    if ( argument[ 0 ] == '\0' )
    {
	if ( ( alias = alias_lookup( ch, arg ) ) )
	{
	    sprintf( buf, "\"%s\" zdefiniowano jako:\n\r%s", arg, alias->todo );
	    send_to_char_bw( buf, ch );
	    return;
	}

	sprintf( buf, "Nie zdefiniowano \"%s\".\n\r", arg );
	send_to_char( buf, ch );
	return;
    }

    if ( argument[ 0 ] == '+' )
    {
	if ( !( alias = alias_lookup( ch, arg ) ) )
	{
	    send_to_char( "Takiego aliasu jeszcze nie masz.\n\r", ch );
	    return;
	}

	/* MAX_INPUT_LENGTH - 3 na "\n\r\0" */
	if ( strlen( alias->todo ) > MAX_STRING_LENGTH / 2 - MAX_INPUT_LENGTH - 3 )
	{
	    send_to_char( "Za d`lugi alias.\n\r", ch );
	    return;
	}
	argument[ 0 ] = ' ';

	while ( isspace( (int) *argument ) )
	    argument++;

	sprintf( buf, "Do definicji \"%s\" dodajesz \"%s\".\n\r", arg, argument );
	send_to_char_bw( buf, ch );
	sprintf( buf, "%s\n\r", argument );
	strcpy( buf1, alias->todo );
	strcat( buf1, buf );
	free_string( alias->todo );
	alias->todo = str_dup( buf1 );

	return;
    }

    if ( *argument == '-' )
    {
	int last;
	int pos;

	if ( !( alias = alias_lookup( ch, arg ) ) )
	{
	    send_to_char( "Takiego aliasu jeszcze nie masz.\n\r", ch );
	    return;
	}

	strcpy( buf, alias->todo );
	if ( !buf[ 0 ] )
	{
	    send_to_char( "Nie masz sk`ad usuwa`c.\n\r", ch );
	    return;
	}

	last = 0;
	for ( pos = 0; buf[ pos ]; pos++ )
	    if ( buf[ pos ] == '\r' && buf[ pos + 1 ] != '\0' )
		last = pos;

	buf[ last + 1 ] = '\0';
	if ( !last )
	{
	    send_to_char( "Do usuwania alias`ow s`lu`zy polecenie dealiasuj.\n\r", ch );
	    return;
	}

	free_string( alias->todo );
	alias->todo = str_dup( buf );

	if ( ch->sex == 2 )
	    STC( "Wyci`e`la`s linijk`e aliasu.\n\r", ch );
	else
	    STC( "Wyci`a`le`s linijk`e aliasu.\n\r", ch );

	return;
    }

    /* ` glownie dlatego, ze nie pokazuje sie potem na liscie */
    if ( arg[ 0 ] == '&'
      || arg[ 0 ] == '!'
      || ( arg[ 0 ] == '`' && !arg[ 1 ] ) )
    {
	send_to_char( "Takiego aliasu nie mo`zesz mie`c.\n\r", ch );
	return;
    }

    if ( !isalpha( (int) arg[ 0 ] )
      && !isdigit( (int) arg[ 0 ] )
      && arg[ 1 ] )
    {
	send_to_char( "Aliasu o takiej nazwie nie da si`e wykona`c.\n\r", ch );
	return;
    }

    sprintf( buf, "'%s' definiujesz jako '%s'.\n\r", arg, argument );
    send_to_char_bw( buf, ch );

    if ( ( alias = alias_lookup( ch, arg ) ) )
    {
	sprintf( buf, "%s\n\r", argument );
	free_string( alias->todo );
	alias->todo = str_dup( buf );
	return;
    }

    alias = new_alias( );
    alias->name = str_dup( arg );
    sprintf( buf, "%s\n\r", argument );
    alias->todo = str_dup( buf );

    /* jezeli nie ma w ogole aliasow */
    if ( !ch->alias )
    {
	ch->alias = alias;
	alias->next = NULL;
	return;
    }

    /* dzieki temu aliasy dodaja sie na koncu, a nie na poczatku listy */
    alias2 = ch->alias;
    while ( alias2->next )
	alias2 = alias2->next;
    alias->next = NULL;
    alias2->next = alias;

    return;
}


KOMENDA( do_unalias )
{
    char       arg[ MAX_INPUT_LENGTH ];
    char       buf[ MAX_STRING_LENGTH ];
    ALIAS_DATA *alias;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
	if ( !ch->alias )
	{
	    send_to_char( "Nie zdefiniowano `zadnych alias`ow.\n\r", ch );
	    return;
	}
	send_to_char( "Kt`ory ze swych alias`ow pragniesz usun`a`c?\n\r", ch );
	return;
    }

    if ( ( alias = alias_lookup( ch, arg ) ) )
    {
	sprintf( buf, "Usuwasz alias \"%s\".\n\r", arg );
	send_to_char( buf, ch );
	del_alias( ch, alias );
	return;
    }

    sprintf( buf, "Nie zdefiniowano \"%s\".\n\r", arg );
    send_to_char( buf, ch );

    return;
}


KOMENDA( do_realias )
{
    char       arg [ MIL ];
    char       arg2[ MIL ];
    char       buf [ MSL ];
    ALIAS_DATA *alias;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( !*arg || !*arg2 )
    {
	if ( !ch->alias )
	{
	    send_to_char( "Nie zdefiniowano `zadnych alias`ow.\n\r", ch );
	    return;
	}
	send_to_char( "Kt`ory ze swych alias`ow pragniesz przemianowa`c i na jak`a nazw`e?\n\r", ch );
	return;
    }

    if ( ( alias = alias_lookup( ch, arg ) ) )
    {
	if ( alias_lookup( ch, arg2 ) )
	{
	    STC( "Nie mo`zesz stworzy`c dw`och alias`ow o tej samej nazwie.\n\r", ch );
	    return;
	}

	if ( arg[ 0 ] == '&'
	  || arg[ 0 ] == '!'
	  || ( arg[ 0 ] == '`' && !arg[ 1 ] ) )
	{
	    send_to_char( "Takiego aliasu nie mo`zesz mie`c.\n\r", ch );
	    return;
	}

	if ( !isalpha( (int) arg[ 0 ] )
	  && !isdigit( (int) arg[ 0 ] )
	  && arg[ 1 ] )
	{
	    send_to_char( "Aliasu o takiej nazwie nie da si`e wykona`c.\n\r", ch );
	    return;
	}

	sprintf( buf, "Alias \"%s\" przemianowujesz na \"%s\".\n\r", arg, arg2 );
	send_to_char_bw( buf, ch );

	free_string( alias->name );
	alias->name = str_dup( arg2 );
	return;
    }

    sprintf( buf, "Nie zdefiniowano \"%s\".\n\r", arg );
    send_to_char( buf, ch );

    return;
}
