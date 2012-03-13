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


/*
 * Nowosci w Lacu. Teraz przeniesione tutaj do uzycia w do_nowosci i html_news
 */
char * const lac_nowosci =
"{YMog`ace ci`e zainteresowa`c aktualne nowo`sci w kodzie Laca:{x\n\r"
" {R*{x \"wyce`n\" w przechowalni pokazuje koszt przechowania przedmiotu\n\r"
" {R*{x nowe opcje \"ruch\", \"autowciel\" i \"czaspp\"\n\r"
" {R*{x nowe polecenie \"poziomy\" i zmiany w `cwiczeniu BWWU\n\r"
" {R*{x polecenie \"inwentarz pojedynczo\"\n\r"
" {R*{x naprawd`e szale`nczy sza`l bitewny\n\r"
" {R*{x mo`zliwo`s`c prowadzenia sklep`ow przez graczy\n\r"
" {R*{x nowy system walki (pomoc walka, KP, OPC)\n\r"
"{GNapisz tak`ze \"pomoc nowo`sci\"{x\n\r";


/*
 * Lam 22.1.2004
 * tablica sluzaca do zmiany {* na kody kolorow, obslugujaca oprocz kolorow
 * ANSI takze odpowiedniki HTML
 * uwaga: colourconv( ) wymaga znajomosci najdluzszej dlugosci kodu koloru,
 * w razie mieszania z kolorami html, uaktualnic kod tam
 */
const char *tablica_kolorow[ 4 ][ MAX_COLOUR ] =
{
    {
	/* kody kolorow po {, nie uzywaj "-" */
	"0", "x",
	"r", "g", "y", "b",
	"m", "c", "w",
	"R", "G", "Y", "B",
	"M", "C", "W",
	"{"
    },
    {
	/* namiastki ANSI */
	ESCS "[0m", ESCS "[0m",
	ESCS "[0m", ESCS "[0m", ESCS "[0m", ESCS "[0m",
	ESCS "[0m", ESCS "[0m", ESCS "[0m",
	ESCS "[1m", ESCS "[1m", ESCS "[1m", ESCS "[1m",
	ESCS "[1m", ESCS "[1m", ESCS "[1m",
	"{"
    },
    {
	/* kolory ANSI */
	ESCS "[0m", ESCS "[0m",
	ESCS "[0;31m", ESCS "[0;32m", ESCS "[0;33m", ESCS "[0;34m",
	ESCS "[0;35m", ESCS "[0;36m", ESCS "[0;37m",
	ESCS "[1;31m", ESCS "[1;32m", ESCS "[1;33m", ESCS "[1;34m",
	ESCS "[1;35m", ESCS "[1;36m", ESCS "[1;37m",
	"{"
    },
    {
	/* kolory HTML */
	"</span><span style=\"color:#aaaaaa;\">",
	"</span><span style=\"color:#aaaaaa;\">",
	"</span><span style=\"color:#aa0000;\">",
	"</span><span style=\"color:#00aa00;\">",
	"</span><span style=\"color:#aa5500;\">",
	"</span><span style=\"color:#0000aa;\">",
	"</span><span style=\"color:#aa00aa;\">",
	"</span><span style=\"color:#00aaaa;\">",
	"</span><span style=\"color:#aaaaaa;\">",
	"</span><span style=\"color:#ff5555;\">",
	"</span><span style=\"color:#55ff55;\">",
	"</span><span style=\"color:#ffff55;\">",
	"</span><span style=\"color:#5555ff;\">",
	"</span><span style=\"color:#ff55ff;\">",
	"</span><span style=\"color:#55ffff;\">",
	"</span><span style=\"color:#ffffff;\">",
	"{"
    }
};



/*
 * Tablica sluzaca do obslugi roznych kodowan znakow.
 * Nalezy napisac kod konwertujacy takie tablice (ktore trzeba przeszukiwac) na
 * tablice typu: tablica[ o ] = 'Û'.
 */
const char *polska_tablica[ 5 ][ 18 ] =
{
    {
	/* Odpowiedniki angielskie */
	"A", "a", "C", "c", "E", "e",
	"L", "l", "N", "n", "O", "o",
	"S", "s", "X", "x", "Z", "z"
    },
    {
	/* DOS, CP 852 */
	"§", "•", "è", "Ü", "®", "©",
	"ù", "à", "„", "‰", "‡", "¢",
	"ó", "ò", "ç", "´", "Ω", "æ"
    },
    {
	/* Linux, ISO 8859-2 */
	"°", "±", "∆", "Ê", " ", "Í",
	"£", "≥", "—", "Ò", "”", "Û",
	"¶", "∂", "¨", "º", "Ø", "ø"
    },
    {
	/* Windows, CP 1250 */
	"•", "π", "∆", "Ê", " ", "Í",
	"£", "≥", "—", "Ò", "”", "Û",
	"å", "ú", "è", "ü", "Ø", "ø"
    },
    {
	/* za xterm -u8 */
	"ƒÑ", "ƒÖ", "ƒÜ", "ƒá", "ƒò", "ƒô",
	"≈Å", "≈Ç", "≈É", "≈Ñ", "√ì", "√≥",
	"≈ö", "≈õ", "≈π", "≈∫", "≈ª", "≈º"
    }
};


/* nazwy dni, koniecznie 7 */
const char *day_name[ ] =
{
    "ksi`e`zyca", "byka", "k`lamstwa", "gromu", "wolno`sci",
    "wielkich bog`ow", "s`lo`nca"
};


/* nazwy miesiecy, koniecznie 17 */
const char *month_name[ ] =
{
    /* zima */ "zimy", "wilka", "lodowego olbrzyma",
    /* przedwiosnie */ "dawnej pot`egi", "wielkiej walki",
    /* wiosna */ "wiosny", "przyrody", "b`laho`sci",
    /* lato */ "smoka", "s`lo`nca", "ciep`la", "bitwy",
    /* jesien */ "ciemnych odcieni", "cieni", "d`lugich cieni",
    /* przedzimie */ "dawnej ciemno`sci", "wielkiego z`la"
};


const struct holiday_type holiday_table[ ] =
{
    {  1,  1, "Dniem Stworzenia `Swiata", "{YRozpoczynamy obchody Dnia Stworzenia `Swiata! W tym dniu wszyscy powinni\n\rcieszy`c si`e swym `zyciem nadanym im przez bog`ow, s`lawi`c ich imiona i chwali`c\n\rdokonane przez nich dzie`lo stworzenia. `Swi`etujmy!{x\n\r" },
    {  2, 33, "Dniem Chwa`ly Ulryka", "{YRozpocz`a`l si`e Dzie`n Chwa`ly Ulryka. Kto nie wypije jego zdrowia, temu p`lacz\n\ri zgrzytanie z`eb`ow, piekielne m`eki i przypalanie ogniem po wsze dni.\n\r`Swi`etujmy!{x\n\r" },
    { 10,  8, "Dniem Czci Lama", "{YRozpocz`a`l si`e w`la`snie Dzie`n Czci Lama! W tym dniu nale`zy bezwzgl`ednie s`lawi`c\n\rimi`e Lama nad wszystkie inne i post`epowa`c wedle jego woli! A teraz `swi`etujmy!{x\n\r" },
    {  6, 13, "`Swi`etem Mi`lo`sci", "{YW dniu dzisiejszym Naja, bogini mi`lo`sci, b`logos`lawi wszystkich szcz`e`sliwych\n\rkochank`ow. Porzu`ccie zatem or`e`z i przy`l`aczcie si`e do wsp`olnej zabawy.{x\n\r" },
    {  0,  0, NULL, NULL }
};


/*
 * Class table.
 */
const	struct	class_type	class_table	[ MAX_CLASS ]	=
{
    {
	"mag", "mag", "mag", "mag",
	APPLY_INT,  &OBJ_VNUM_SCHOOL_DAGGER,
	75,  25,  33,   6,  8, 0.85, TRUE,
/*	"human elf halfelf gnome sectoid" */
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
    },

    {
	"kle", "kle", "kleryk", "cle",
	APPLY_WIS,  &OBJ_VNUM_SCHOOL_MACE,
	75,  25,  30,  7,  10, 1.0, TRUE,
/*	"human elf halfelf drow" */
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
    },

    {
	"zlo", "z`lo", "z`lodziej", "thi",
	APPLY_DEX,  &OBJ_VNUM_SCHOOL_DAGGER,
	75,  25,  40,  8,  13, 0.9, FALSE,
/*	"human hobbit halfdwarf drow gnome" */
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
    },

    {
	"woj", "woj", "wojownik", "war",
	APPLY_STR,  &OBJ_VNUM_SCHOOL_SWORD,
	75,  25,  45,  11, 15, 0.8, FALSE,
/*	"human drow dwarf halfdwarf" */
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
    },

    {
	"psi", "psi", "psionik", "psi",
	APPLY_WIS,  &OBJ_VNUM_SCHOOL_DAGGER,
	75,  25,  28,   6,  9, 0.9, TRUE,
/*	"human elf halfelf sectoid gnome" */
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
    },

    {
	"szp", "szp", "szpieg", "spy",
	APPLY_DEX,  &OBJ_VNUM_SCHOOL_DAGGER,
	75,  25,  35,  7,  12, 0.75, TRUE,
/*	"human halfelf hobbit halfdwarf drow gnome" */
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
    },

    {
	"sza", "sza", "szaman", "sha",
	APPLY_INT,  &OBJ_VNUM_SCHOOL_DAGGER,
	75,  25,  33,  6,  10, 0.9, TRUE,
/*	"human drow elf halfelf gnome sectoid" */
	""
    },

    {
	"skr", "skr", "skrytob`ojca", "asn",
	APPLY_CON,  &OBJ_VNUM_SCHOOL_SWORD,
	75,  25,  40,  9, 14, 0.8, FALSE,
/*	"human drow dwarf halfdwarf hobbit gnome" */
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
/*	"" */
    },

    {
	"mni", "mni", "mnich", "mon",
	APPLY_CON,  &OBJ_VNUM_SCHOOL_DAGGER,
	75,  25,  35,  7, 12, 0.75, TRUE,
/*	"human drow elf halfelf dwarf halfdwarf hobbit gnome" */
/*	"" */
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
    },

    {
	"nek", "nek", "nekromanta", "nec",
	APPLY_WIS,  &OBJ_VNUM_SCHOOL_DAGGER,
	75,  25,  33,  6,  9, 0.85, TRUE,
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
    },

    {
	"pal", "pal", "paladyn", "pal",
	APPLY_WIS,  &OBJ_VNUM_SCHOOL_SWORD,
	75,  25,  35,  10,  14, 0.75, TRUE,
	"human drow elf halfelf dwarf halfdwarf hobbit gnome sectoid vampire"
    }
};


/*
 * Immort Levels
 */
#define L_HER		 LEVEL_HERO


/*
 * Titles.
 */
char *const  title_table [ MAX_CLASS ] /* [ MAX_LEVEL + 1 ] */ [ 2 ] =
{
	{ "mag",			"czarodziejka"		},
	{ "kleryk",			"kleryczka"		},
	{ "z`lodziej",			"z`lodziejka"		},
	{ "wojownik",			"wojowniczka"		},
	{ "psionik",			"psioniczka"		},
	{ "szpieg",			"kobieta szpieg"	},
	{ "szaman",			"szamanka"		},
	{ "skrytob`ojca",		"skrytob`ojczyni"	},
	{ "mnich",			"mniszka"		},
	{ "nekromanta",                 "nekromantka"           },
	{ "paladyn",			"paladynka"		}
};


/* por. #263 z bledomatu */
const	struct	material_type	material_table	[ MAX_MATERIAL ] =
{
    /* nazwa			dopelniacz
		min max  ac val wght vamp */
    {	"nic",			"z niczego",
		  0, 99,  8,    0,  8, FALSE	},/* nie uzywac w krainach */
    {	"drewno",		"z drewna",
		  0,  5,  6,   21,  3, FALSE	},
    {	"materia`l",		"z materia`lu",
		  0,  5,  2,  165,  1, FALSE	},
    {	"sk`ora",		"ze sk`ory",
		  2, 25,  8,  134,  2, FALSE	},
    {	"futro",		"z futra",
		  3, 10,  5,  306,  5, FALSE	},
    {	"kamie`n",		"z kamienia",
		  3,  8,  7,   44, 16, FALSE	},
    {	"ko`s`c",		"z ko`sci",
		  2, 12,  8,   65,  4, FALSE	},
    {	"obsydian",		"z obsydianu",
		  4, 10,  8,  121, 14, FALSE	},
    {	"br`az",		"z br`azu",
		  5, 15,  8,  980,  7, FALSE	},
    {	"r`og",			"z rogu",
		  6, 15,  7,  100,  5, FALSE	},
    {	"o`l`ow",		"z o`lowiu",
		  6, 16,  6,  232, 20, FALSE	},
    {	"stal ork`ow",		"ze stali ork`ow",
		  5, 20,  8, 1044, 15, FALSE	},
    {	"mosi`adz",		"z mosi`adzu",
		  6, 22,  8,  975,  7, FALSE	},
    {	"ko`s`c s`loniowa",	"z ko`sci s`loniowej",
		  7, 30,  8,  811,  6, FALSE	},
    {	"`zelazo",		"z `zelaza",
		  8, 30,  8, 1016,  8, FALSE	},
    {	"stal",			"ze stali",
		 15, 50, 10, 1035,  7, FALSE	},
    {	"srebro",		"ze srebra",
		 10, 50,  7, 1069, 12, TRUE	},
    {	"stal elf`ow",		"ze stali elf`ow",
		 25, 60, 12, 1050,  6, FALSE	},
    {	"stal krasnali",	"ze stali krasnali",
		 30, 70, 15, 1066, 10, FALSE	},
    {	"z`loto",		"ze z`lota",
		 30, 70,  7, 1088, 10, FALSE	},
    {	"smocza `luska",	"ze smoczej `luski",
		 32, 85, 16, 1020, 12, FALSE	},
    {	"tytan",		"z tytanu",
		 35, 65, 16, 1473, 10, FALSE	},
    {	"heban",		"z hebanu",
		 40, 75, 18, 691,  4, FALSE	},
    {	"platyna",		"z platyny",
		 42, 80, 12, 1395, 11, FALSE	},
    {	"mithril",		"z mithrilu",
		 50, 90, 22, 1533,  6, FALSE	},
    {	"spacze`n",		"ze spaczenia",
		 40,100, 18, 1070, 15, FALSE	},
    {	"eternuum",		"z eternuum",
		 60, 90, 22, 1788,  3, FALSE	},
    {	"demonicum",		"z demonicum",
		 80,100, 25, 1666,  2, FALSE	},
    {	"infinitum",		"z infinitum",
		101,111,100, 1630,  1, FALSE	},
    {	"papier",		"z papieru",
		  0, 99,  8,  183,  8, FALSE	},
    {	"we`lna",		"z we`lny",
		  0, 99,  8,  275,  8, FALSE	},
    {	"guma",			"z gumy",
		  0, 99,  8,  963,  8, FALSE	},
    {	"pergamin",		"z pergaminu",
		  0, 99,  8,  200,  8, FALSE	},
    {	"porcelana",		"z porcelany",
		  0, 99,  8,  586,  8, FALSE	},
    {	"masa per`lowa",	"z masy per`lowej",
		  0, 99,  8,  429,  8, FALSE	},
    {	"marmur",		"z marmuru",
		  0, 99,  8,  730,  8, FALSE	},
    {	"muszle",		"z muszli",
		  0, 99,  8,   96,  8, FALSE	},
    {	"sk`ora w`e`za",	"ze sk`ory w`e`za",
		  0, 99,  8,  365,  8, FALSE	},
    {	"at`las",		"z at`lasu",
		  0, 99,  8,  635,  8, FALSE	},
    {	"aksamit",		"z aksamitu",
		  0, 99,  8,  500,  8, FALSE	},
    {	"jedwab",		"z jedwabiu",
		  0, 99,  8,  830,  8, FALSE	},
    {	"szk`lo",		"ze szk`la",
		  0, 99,  8,  870,  8, FALSE	},
    {	"p`l`otno",		"z p`l`otna",
		  0, 99,  8,  263,  8, FALSE	},
    {	"bawe`lna",		"z bawe`lny",
		  0, 99,  8,  371,  8, FALSE	},
    {	"perkal",		"z perkalu",
		  0, 99,  8,  420,  8, FALSE	},
    {	"brokat",		"z brokatu",
		  0, 99,  8, 1000,  8, FALSE	},
    {	"filc",			"z filcu",
		  0, 99,  8,  172,  8, FALSE	},
    {	"kaszmir",		"z kaszmiru",
		  0, 99,  8,  900,  8, FALSE	},
    {	"mu`slin",		"z mu`slinu",
		  0, 99,  8,  677,  8, FALSE	},
    {	"popelina",		"z popeliny",
		  0, 99,  8,  336,  8, FALSE	},
    {	"satyna",		"z satyny",
		  0, 99,  8,  614,  8, FALSE	},
    {	"sukno",		"z sukna",
		  0, 99,  8,  301,  8, FALSE	},
    {	"tafta",		"z tafty",
		  0, 99,  8,  378,  8, FALSE	},
    {	"woal",			"z woalu",
		  0, 99,  8,  310,  8, FALSE	},
    {	"zamsz",		"z zamszu",
		  0, 99,  8,  435,  8, FALSE	},
    {	"bursztyn",		"z bursztynu",
		  0, 99,  8, 1168,  8, FALSE	},
    {	"diament",		"z diamentu",
		  0, 99,  8, 1346,  8, FALSE	},
    {	"ametyst",		"z ametystu",
		  0, 99,  8, 1195,  8, FALSE	},
    {	"rubin",		"z rubinu",
		  0, 99,  8, 1310,  8, FALSE	},
    {	"lazuryt",		"z lazurytu",
		  0, 99,  8, 1142,  8, FALSE	},
    {	"agat",			"z agatu",
		  0, 99,  8, 1149,  8, FALSE	},
    {	"szmaragd",		"ze szmaragdu",
		  0, 99,  8, 1263,  8, FALSE	},
    {	"turkus",		"z turkusu",
		  0, 99,  8, 1245,  8, FALSE	},
    {	"opal",			"z opalu",
		  0, 99,  8, 1221,  8, FALSE	},
    {	"szafir",		"z szafiru",
		  0, 99,  8, 1290,  8, FALSE	},
    {	"rodochrozyt",		"z rodochrozytu",
		  0, 99,  8, 1120,  8, FALSE	},
    {	"rodonit",		"z rodonitu",
		  0, 99,  8, 1127,  8, FALSE	},
    {	"chryzolit",		"z chryzolitu",
		  0, 99,  8, 1135,  8, FALSE	},
    {	"turmalin",		"z turmalinu",
		  0, 99,  8, 1156,  8, FALSE	},
    {	"kwarc",		"z kwarcu",
		  0, 99,  8, 1179,  8, FALSE	},
    {	"granat",		"z granatu",
		  0, 99,  8, 1279,  8, FALSE	},
    {	"almandyn",		"z almandynu",
		  0, 99,  8, 1114,  8, FALSE	},
    {	"kamie`n ksi`e`zycowy",	"z kamienia ksi`e`zycowego",
		  0, 99,  8, 1100,  8, FALSE	},
    {	"kamie`n s`loneczny",	"z kamienia s`lonecznego",
		  0, 99,  8, 1098,  8, FALSE	},
    {	"kamie`n szlachetny",	"z kamienia szlachetnego",
		  0, 99,  8, 1093,  8, FALSE	}
};


/*
 * Vigud 24.10.2010: kolejnosc wyswietlania kierunkow. Byli tacy, co mysleli, ze
 * E jest na zachodzie, bo bylo po lewej ("zachodniej") stronie S w ciagu NESW
 */
const int dir_order[ MAX_DIR ] =
{
    0, 3, 2, 1, 4, 5, 8, 9, 6, 7
};


/*
 * Alandar 09.12.2004 - jedna tablica kierunkow zamiast kilku
 */
const	struct	dir_type	kierunki[ MAX_DIR ]	=
{
/*   name	 skrot	skrotd	krotki		 biernik		 z_dopelniacz		odwrotny na_krew */
  { "north",	"n",	"N",	"p`o`lnoc",	"p`o`lnoc",		"z p`o`lnocy",		2,	"na p`o`lnoc" },
  { "east",	"e",	"E",	"wsch`od",	"wsch`od",		"ze wschodu",		3,	"na wsch`od" },
  { "south",	"s",	"S",	"po`ludnie",	"po`ludnie",		"z po`ludnia",		0,	"na po`ludnie" },
  { "west",	"w",	"W",	"zach`od",	"zach`od",		"z zachodu",		1,	"na zach`od" },
  { "up",	"g",	"G",	"g`ora",	"g`or`e",		"z g`ory",		5,	"powy`zej" },
  { "down",	"d",	"D",	"d`o`l",	"d`o`l",		"z do`lu",		4,	"poni`zej" },
  { "northeast","ne",	"NE",	"pn.-wsch.",	"p`o`lnocny wsch`od",	"z p`o`lnocnego wschodu", 9,	"na p`o`lnocny wsch`od" },
  { "southeast","se",	"SE",	"pd.-wsch.",	"po`ludniowy wsch`od",	"z po`ludniowego wschodu", 8,	"na po`ludniowy wsch`od" },
  { "northwest","nw",	"NW",	"pn.-zach.",	"p`o`lnocny zach`od",	"z p`o`lnocnego zachodu", 7,	"na p`o`lnocny zach`od" },
  { "southwest","sw",	"SW",	"pd.-zach.",	"po`ludniowy zach`od",	"z po`ludniowego zachodu", 6,	"na po`ludniowy zach`od" }
};


/*
 * Race types
 */
const	struct	  race_type	  race_table	  [ MAX_RACE ]	  =
{
    { "cz`lowiek", "cz`lowieka", "cz`lowiekowi", "cz`lowieka", "cz`lowiekiem",
	"cz`lowieku", "cz`lowieku", "czlowiek", "human",
	RACE_PC_AVAIL | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	4, 320, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, "pi`e`s`c", 2,
	"Githyanki Vampire Werewolf Mindflayer",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "elf", "elfa", "elfowi", "elfa", "elfem",
	"elfie", "elfie", "elf", "elf",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN
	 | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	4, 240, -2, 3, 2, 1, -2,
	0, 4, 0, 0, 0, "pi`e`s`c", 2,
	"Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold Duergar",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "p`o`lelf", "p`o`lelfa", "p`o`lelfowi", "p`o`lelfa", "p`o`lelfem",
	"p`o`lelfie", "p`o`lelfie", "polelf", "halfelf",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	4, 280, 0, 2, 1, 0, -1,
	0, 2, 0, 0, 0, "pi`e`s`c", 2,
	"Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin Duergar",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "ciemny elf", "ciemnego elfa", "ciemnemu elfowi", "ciemnego elfa", "ciemnym elfem",
	"ciemnym elfie", "ciemny elfie", "ciemny elf", "drow",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN
	 | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	4, 240, -1, 1, 2, 1, 0,
	0, 4, 0, 0, 0, "pi`e`s`c", 2,
	"Elf Halfelf Hobbit Githyanki Vampire Werewolf Duergar",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "krasnolud", "krasnoluda", "krasnoludowi", "krasnoluda", "krasnoludem",
	"krasnoludzie", "krasnoludzie", "krasnolud", "dwarf",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN
	 | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 320, 2, 0, 0, -1, 1,
	0, 0, 0, 0, 1, "pi`e`s`c", 2,
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "p`o`lkrasnal", "p`o`lkrasnala", "p`o`lkrasnalowi", "p`o`lkrasnala", "p`o`lkrasnalem",
	"p`o`lkrasnalu", "p`o`lkrasnalu", "polkrasnal", "halfdwarf",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 320, 1, 0, 0, 0, 1,
	0, 0, 0, 0, 0, "pi`e`s`c", 2,
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "hobbit", "hobbita", "hobbitowi", "hobbita", "hobbitem",
	"hobbicie", "hobbicie", "hobbit", "hobbit",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN
	 | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 240, 0, 1, 0, 3, -1,
	0, 0, 0, 0, 0, "pi`e`s`c", 2,
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold",
	"", "",
	WRL_STOPY, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "gnom", "gnoma", "gnomowi", "gnoma", "gnomem",
	"gnomie", "gnomie", "gnom", "gnome",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 240, -1, 1, 1, 2, -1,
	0, 4, 0, 0, 0, "pi`e`s`c", 2,
	"Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin Duergar",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "p`o`lkobold", "p`o`lkobolda", "p`o`lkoboldowi", "p`o`lkobolda", "p`o`lkoboldem",
	"p`o`lkoboldzie", "p`o`lkoboldzie", "polkobold", "halfkobold",
	RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 240, -2, -1, -2, 3, -2,
	0, 0, 0, 0, 0, "pi`e`s`c", 2,
	"Ogre Orc Giant Troll Hobgoblin",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "olbrzym", "olbrzyma", "olbrzymowi", "olbrzyma", "olbrzymem",
	"olbrzymie", "olbrzymie", "olbrzym", "giant",
	RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	8, 2000, 4, -4, -4, -1, 2,
	3, -5, -2, 2, 4, "pi`e`s`c", 2,
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "ogr", "ogra", "ogrowi", "ogra", "ogrem",
	"ogrze", "ogrze", "ogr", "ogre",
	RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	7, 1400, 2, -3, -2, -1, 1,
	2, -3, -1, 0, 2, "pi`e`s`c", 2,
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_HS, 0
    },

    { "ork", "orka", "okrowi", "orka", "orkiem",
	"orku", "orku", "ork", "orc",
	RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS
	 | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD,
	5, 500, 1, -2, -1, 0, 0,
	2, -1, -1, 0, 1, "pi`e`s`c", 2,
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_HS, 0
    },

    { "kobold", "kobolda", "koboldowi", "kobolda", "koboldem",
	"koboldzie", "koboldzie","kobold", "kobold",
	RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 240, -1, -1, 0, 1, 0,
	0, 0, 0, 0, 0, "pi`e`s`c", 2,
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome Halfkobold",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "minotaur", "minotaura", "minotaurowi", "minotaura", "minotaurem",
	"minotaurze", "minotaurze", "minotaur","minotaur",
	RACE_DETECT_HIDDEN  | RACE_WEAPON_WIELD | RACE_OPENS_DOORS | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD,
	5, 2000, 2, -1, -1, -1, 1,
	3, 0, -1, 0, 3, "pi`e`s`c", 2,
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	"", "",
	WRL_STOPY | WRL_GLOWA, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_HS | P_ROG, &zr_minotaur
    },

    { "troll", "trolla", "trollowi", "trolla", "trollem",
	"trollu", "trollu", "troll", "troll",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	7, 1400, 2, -3, -2, 0, 1,
	10, 0, 0, 0, 8, "pi`e`s`c", 2,
	"Human Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_HS, &zr_troll
    },

    { "hobgoblin", "hobgoblina", "hobgoblinowi", "hobgoblina", "hobgoblinem",
	"hobgoblinie", "hobgoblinie", "hobgoblin", "hobgoblin",
	RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	4, 240, 1, -1, -2, 0, 1,
	0, 0, 0, 0, 0, "pi`e`s`c", 2,
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "owad", "owada", "owadowi", "owada", "owadem",
	"owadzie", "owadzie", "owad", "insect",
	RACE_MUTE | RACE_NO_BLOOD | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	0, 1, -15, -15, -15, 5, -15,
	0, 0, 5, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, /* zadne */ PAPU_DOWOLNE,
	P_NONE, 0
    },

    { "insektoid", "insektoida", "insektoidowi", "insektoida", "insektoidem",
	"insektoidzie", "insektoidzie", "insektoid", "insectoid",
	RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD,
	4, 240, 1, -5, -5, 0, 1,
	0, 0, 5, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_NONE, 0
    },

    { "smok", "smoka", "smokowi", "smoka", "smokiem",
	"smoku", "smoku", "smok", "dragon",
	RACE_INFRAVISION | RACE_DETECT_EVIL | RACE_DETECT_INVIS
	 | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	10, 20000, 4, 2, 3, -3, 4,
	15, 0, -10, 0, 10, "zadrapanie", 0,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_FLAKI | P_SERCE | P_OGON | P_SKRZYDLA | P_LAPA | P_LEB | P_ZAB, &zr_smok
    },

    { "zwierz`e", "zwierz`ecia", "zwierz`eciu", "zwierz`e", "zwierz`eciem",
	"zwierz`eciu", "zwierzu", "zwierze", "animal",
	RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	2, 100, -5, -10, -10, 1, -8,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"Kobold Halfkobold",
	"", "",
	WRL_TARCZA | WRL_NADGARSTEK | WRL_TRZYMANE, PAPU_DOWOLNE,
	P_FLAKI | P_SERCE, 0
    },

    { "b`og", "boga", "bogu", "boga", "bogiem",
	"bogu", "bo`ze", "bog", "god",
	 RACE_PASSDOOR | RACE_INFRAVISION | RACE_DETECT_EVIL
	 | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_PROTECTION
	 | RACE_SANCT | RACE_WEAPON_WIELD | RACE_POISONIMM | RACE_OPENS_DOORS
	 | RACE_NO_BLOOD | RACE_PROTECT_GOOD,
	ODD_POWIETRZEM | ODD_WODA, POR_POD_WODA | POR_LAD | POR_POWIETRZE,
	10, 320, 4, 4, 4, 4, 4,
	20, 0, 10, 0, 0, "pot`ega", 2,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, &zr_bog
    },

    { "martwiak", "martwiaka", "martwiakowi", "martwiaka", "martwiakiem",
	"martwiaku", "martwiaku", "martwiak", "undead",
	RACE_INFRAVISION | RACE_DETECT_EVIL
	 | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD | RACE_POISONIMM | RACE_OPENS_DOORS
	 | RACE_NO_BLOOD,
	ODD_POWIETRZEM, POR_LAD,
	4, 320, 2, -5, -5, -2, 1,
	0, 0, 0, -1, -1, "dotyk", 1,
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Goblin Faerie Gnome Duergar",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_NONE, &zr_martwiak
    },

    { "harpia", "harpii", "harpii", "harpi`e", "harpi`a",
	"harpii", "harpio", "harpia", "harpy",
	RACE_DETECT_HIDDEN | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	4, 320, 0, -5, -5, 2, 0,
	0, 0, 6, 0, 0, "pazur", 1,
	"Human Elf Halfelf Dwarf Halfdwarf Hobbit Gnome",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_SKRZYDLA | P_JEZYK | P_LAPA | P_LEB | P_ZAB,
	&zr_harpia
    },

    { "nied`xwied`x", "nied`xwiedzia", "nied`xwiedziowi", "nied`xwiedzia", "nied`xwiedziem",
	"nied`xwiedziu", "nied`xwiedziu", "niedzwiedz", "bear",
	RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_POD_WODA | POR_PO_WODZIE | POR_LAD,
	7, 1400, 3, -10, -10, -1, 3,
	0, 0, 0, 0, 3, "`lapa", 2,
	"",
	"", "",
	WRL_PALEC | WRL_TRZYMANE | WRL_STOPY | WRL_DLONIE | WRL_TARCZA | WRL_CIALO,
	PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_LAPA | P_LEB | P_KIEL | P_ZAB, &zr_niedzwiedz
    },

    { "githyanki", "githyankiego", "githyankiemu", "githyankiego", "githyankim",
	"githyankim", "githyanki", "githyanki", "githyanki",
	RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	5, 500, 0, 1, 0, 0, 0,
	0, 4, 0, 0, 0, "uderzenie", 0,
	"Mindflayer",
	"", "",
	WRL_HS, PAPU_ROSLINNE,
	P_HS, 0
    },

    { "`zywio`l", "`zywio`lu", "`zywio`lowi", "`zywio`l", "`zywio`lem",
	"`zywiole", "`zywiole", "zywiol", "elemental",
	RACE_MUTE | RACE_NO_MOUTH | RACE_POISONIMM | RACE_NO_BLOOD
	 | RACE_NO_CORPSE | RACE_NO_EYES | RACE_INFRAVISION,
	ODD_POWIETRZEM | ODD_WODA, POR_POD_WODA | POR_LAD,
	5, 800, 2, -5, -5, 0, 1,
	0, 0, 0, -1, -1, "moc", 2,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_NONE, 0
    },

    { "nietoperz", "nietoperza", "nietoperzowi", "nietoperza", "nietoperzem",
	"nietoperzu", "nietoperzu", "nietoperz", "bat",
	RACE_PASSDOOR | RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_POWIETRZE,
	1, 4, -8, -8, -8, 5, -3,
	0, 0, 4, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_DOWOLNE,
	P_FLAKI | P_SKRZYDLA | P_LEB | P_ZAB | P_KIEL | P_SERCE, &zr_nietoperz
    },

    { "ro`slina", "ro`sliny", "ro`slinie", "ro`slin`e", "ro`slin`a",
	"ro`slinie", "ro`slino", "roslina", "plant",
	RACE_MUTE | RACE_NO_MOUTH | RACE_POISONIMM | RACE_NO_BLOOD
	 | RACE_NO_EYES | RACE_INFRAVISION,
	ODD_WODA | ODD_POWIETRZEM, POR_LAD, /* dylemat oddychania i poruszania */
	5, 160, -1, -15, -15, -8, 0,
	0, 0, 0, 0, -1, "ruch", 1,
	"",
	"", "",
	~WRL_HS, PAPU_ZADNE,
	P_NONE, 0
    },

    { "szczur", "szczura", "szczurowi", "szczura", "szczurem",
	"szczurze", "szczurze", "szczur", "rat",
	RACE_PASSDOOR | RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_PO_WODZIE | POR_LAD,
	1, 4, -10, -10, -10, 4, -10,
	1, 0, 0, 0, 0, "ugryzienie", 0,
	"cat",
	"", "",
	~WRL_HS, PAPU_DOWOLNE,
	P_FLAKI | P_SERCE | P_OGON | P_LEB | P_ZAB, 0
    },

    { "wampir", "wampira", "wampirowi", "wampira", "wampirem",
	"wampirze", "wampirze", "wampir", "vampire",
	RACE_INFRAVISION | RACE_DETECT_EVIL | RACE_DETECT_INVIS
	 | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD | RACE_POISONIMM
	 | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	5, 500, 1, 2, 0, 1, 2,
	10, 3, 3, -1, 5, "drapni`ecie", 0,
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Werewolf Goblin Faerie Gnome Duergar",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_HS | P_KIEL, &zr_wampir
    },

    { "wilko`lak", "wilko`laka", "wilko`lakowi", "wilko`laka", "wilko`lakiem",
	"wilko`laku", "wilko`laku", "wilkolak", "werewolf",
	RACE_INFRAVISION | RACE_DETECT_EVIL | RACE_DETECT_INVIS
	 | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD | RACE_POISONIMM
	 | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	5, 800, 2, 0, 1, 2, 3,
	10, 0, 5, 0, 5, "drapni`ecie", 0,
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Vampire Goblin Faerie Gnome Duergar",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_NOGI | P_SERCE | P_USZY | P_RECE | P_LEB | P_KIEL | P_ZAB, 0
    },

    { "goblin", "goblina", "goblinowi", "goblina", "goblinem",
	"goblinie", "goblinie", "goblin", "goblin",
	RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 200, -1, -1, -1, 1, 0,
	0, 0, 0, 0, 0, "pi`e`s`c", 2,
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "driada", "driady", "driadzie", "driad`e", "driad`a",
	"driadzie", "driado", "driada", "faerie",
	RACE_INFRAVISION | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN
	 | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	3, 240, -2, 1, 1, 1, -1,
	0, 0, 7, 0, 0, "uderzenie", 0,
	"",
	"", "",
	WRL_HS, PAPU_ROSLINNE,
	P_HS, 0
    },

    { "paj`eczak", "paj`eczaka", "paj`eczakowi", "paj`eczaka", "paj`eczakiem",
	"paj`eczaku", "paj`eczaku", "pajeczak", "arachnid",
	RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD,
	0, 1, -12, -12, -12, 1, -10,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_DOWOLNE,
	P_NONE, &zr_pajeczak
    },

    { "illithid", "illithida", "illithidowi", "illithida", "illithidem",
	"illithidzie", "illithidzie", "illithid", "mindflayer",
	RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	5, 500, 1, 4, 2, -1, 0,
	0, 6, 0, 0, 0, "uderzenie", 0,
	"Githyanki",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_GLOWA | P_RECE | P_NOGI | P_SERCE | P_JEZYK | P_ZAB, 0
    },

    { "przedmiot", "przedmiotu", "przedmiotowi", "przedmiot", "przedmiotem",
	"przedmiocie", "przedmiocie", "przedmiot", "object",
	RACE_MUTE | RACE_NO_MOUTH | RACE_POISONIMM | RACE_NO_BLOOD
	 | RACE_NO_EYES | RACE_INFRAVISION | RACE_NO_CORPSE,
	ODD_WODA | ODD_POWIETRZEM, POR_PO_WODZIE | POR_LAD,
	5, 500, 4, -15, -15, -15, 5,
	0, 0, 0, -1, -1, "ruch", 1,
	"",
	"", "",
	~WRL_HS, PAPU_DOWOLNE,
	P_NONE, &zr_przedmiot
    },

    { "mg`la", "mg`ly", "mgle", "mg`l`e", "mg`l`a",
	"mgle", "mg`lo", "mgla", "mist",
	RACE_PASSDOOR | RACE_MUTE | RACE_NO_MOUTH | RACE_POISONIMM
	 | RACE_NO_BLOOD | RACE_NO_CORPSE | RACE_NO_EYES | RACE_INFRAVISION,
	ODD_WODA | ODD_POWIETRZEM, POR_POWIETRZE,
	3, 1, -1, -7, -7, 3, 0,
	0, 0, 20, -1, -1, "opary", 3,
	"",
	"", "",
	~WRL_HS, PAPU_DOWOLNE,
	P_NONE, 0
    },

    { "w`a`z", "w`e`za", "w`e`zowi", "w`e`za", "w`e`zem",
	"w`e`zu", "w`e`zu", "waz", "snake",
	RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_POD_WODA | POR_LAD,
	1, 20, -6, -10, -10, 1, -7,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"przype`lza", "odpe`lza",
	WRL_TARCZA | WRL_TRZYMANE | WRL_DLONIE | WRL_PALEC | WRL_RAMIONA
	 | WRL_NOGI | WRL_STOPY | WRL_NADGARSTEK | WRL_BRON,
	PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_OGON | P_JEZYK | P_LEB | P_KIEL | P_SERCE, &zr_waz
    },

    { "robak", "robaka", "robakowi", "robaka", "robakiem",
	"robaku", "robaku", "robak","worm",
	RACE_PASSDOOR | RACE_MUTE | RACE_NO_BLOOD | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD,
	0, 1, -15, -15, -15, -5, -15,
	0, 0, 0, 0, 0, "`sluz", 1,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_NONE, 0
    },

    { "ryba", "ryby", "rybie", "ryb`e", "ryb`a",
	"rybie", "rybo", "ryba","fish",
	RACE_MUTE | RACE_INFRAVISION,
	ODD_WODA, POR_POD_WODA,
	1, 20, -10, -10, -10, 2, -10,
	0, 0, 0, -1, 0, "p`letwa", 2,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_SERCE | P_FLAKI | P_PLETWA, 0
    },

    { "hydra", "hydry", "hydrze", "hydr`e", "hydr`a",
	"hydrze", "hydro", "hydra", "hydra",
	RACE_DETECT_HIDDEN | RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD,
	8, 10000, 2, -5, -5, -1, 2,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_GLOWA | P_LAPA | P_SERCE | P_JEZYK | P_USZY | P_ZAB
	 | P_OGON, 0
    },

    { "jaszczurka", "jaszczurki", "jaszczurce", "jaszczurk`e", "jaszczurk`a",
	"jaszczurce", "jaszczurko", "jaszczurka", "lizard",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	1, 4, -10, -10, -10, 1, -10,
	0, 0, 0, 0, 0, "uderzenie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_JEZYK | P_KIEL | P_ZAB, 0
    },

    { "sektoid", "sektoida", "sektoidowi", "sektoida", "sektoidem",
	"sektoidzie", "sektoidzie", "sektoid", "sectoid",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD
	 | RACE_DETECT_HIDDEN | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 240, -2, 3, 4, 0, -3,
	0, 10, 0, 0, 0, "uderzenie", 0,
	"Human",
	"", "",
	WRL_HS, PAPU_WSZYSTKO,
	P_FLAKI | P_GLOWA | P_RECE | P_NOGI | P_SERCE | P_JEZYK, 0
    },

    { "shambler", "shamblera", "shamblerowi", "shamblera", "shamblerem",
	"shamblerze", "shamblerze", "shambler", "shambler",
	RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_DETECT_HIDDEN
	 | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	5, 800, 2, -1, 0, 1, 4,
	10, 0, 0, 0, 0, "uderzenie", 0,
	"Human",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_HS, &zr_shambler
    },

    { "duch", "ducha", "duchowi", "ducha", "duchem",
	"duchu", "duchu", "duch", "ghost",
	RACE_PASSDOOR | RACE_NO_MOUTH | RACE_POISONIMM | RACE_NO_BLOOD
	 | RACE_INFRAVISION | RACE_NO_CORPSE | RACE_WEAPON_WIELD
	 | RACE_OPENS_DOORS,
	ODD_WODA | ODD_POWIETRZEM, POR_POD_WODA | POR_POWIETRZE,
	3, 1, -1, 0, 0, 3, 0,
	0, 0, 20, -1, -1, "oddzia`lywanie", 0,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_NONE, 0
    },
/* Ulryk */
    { "demon", "demona", "demonowi", "demona", "demonem",
	"demonie", "demonie", "demon", "demon",
	RACE_INFRAVISION | RACE_DETECT_EVIL
	 | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_PROTECT_GOOD
	 | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	7, 2000, 2, 0, 1, -1, 3,
	10, 2, 0, 0, 0, "pot`ega", 2,
	"Human Elf Halfelf Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Troll Hobgoblin Goblin Faerie Gnome Sektoid",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS | P_SKRZYDLA | P_KIEL, 0
    },

    { "jednoro`zec", "jednoro`zca", "jednoro`zcowi", "jednoro`zca", "jednoro`zcem",
	"jednoro`zcu","jednoro`zcu","jednorozec", "unicorn",
	RACE_INFRAVISION | RACE_DETECT_EVIL | RACE_DETECT_HIDDEN
	 | RACE_PROTECTION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	6, 3000, -2, 1, 2, 0, 1,
	0, 0, 10, 0, 0, "uderzenie rogu", 0,
	"Giant Ogre Orc Kobold Troll Hobgoblin Goblin Faerie Dragon",
	"", "",
	~( WRL_GLOWA | WRL_NADGARSTEK ),  /* tylko peciny i glowa - Ulryk */
	PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB | P_ZAB | P_ROG,
	&zr_jednorozec
    },

    { "kot", "kota", "kotu", "kota", "kotem",
	"kocie", "kocie", "kot", "cat",
	RACE_INFRAVISION | RACE_DETECT_EVIL | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	2, 20, -6, -9, -9, 2, -7,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"Vampire Dog",
	"", "",
	~WRL_SZYJA, PAPU_MIESNE | PAPU_RYBNE | PAPU_MLECZNE,
	P_FLAKI | P_SERCE | P_OGON | P_LEB | P_ZAB | P_KIEL, 0
    },

    { "krowa", "krowy", "krowie", "krow`e", "krow`a",
	"krowie", "krowo", "krowa", "cow",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	6, 3000, 1, -11, -11, -1, 1,
	0, 0,  0,  0,  0, "uderzenie rog`ow", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB | P_ZAB | P_ROG, 0
    },

    { "kr`olik", "kr`olika", "kr`olikowi", "kr`olika", "kr`olkiem",
	"kr`oliku", "kr`oliku", "krolik", "rabbit",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	2, 4, -10, -10, -10, 2, -7,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_USZY | P_LAPA | P_ZAB, 0
    },

    { "kura", "kury", "kurze", "kur`e", "kur`a",
	"kurze", "kuro", "kura", "chicken",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	2, 20, -10, -10, -10, 0, -8,
	0, 0, 0, 0, 0, "dziobni`ecie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB, 0
    },

    { "ko`n", "konia", "koniowi", "konia", "koniem",
	"koniu", "koniu", "kon", "horse",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	6, 1840, 1, -10, -10, 0, 1,
	0, 0, 0, 0, 0, "kopni`ecie", 0,
	"Wolf",
	"", "",
	~( WRL_GLOWA | WRL_NADGARSTEK ), /* jak u jednorozca */
	PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB | P_ZAB, 0
    },

    { "`swinia", "`swini", "`swini", "`swini`e", "`swini`a",
	"`swini", "`swinio", "swinia", "pig",
	RACE_DETECT_EVIL | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD | POR_PO_WODZIE,
	3, 800, 0, -11, -11, -5, 0,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO,
	P_FLAKI | P_NOGI | P_SERCE | P_LEB | P_ZAB, 0
    },

    { "osio`l", "os`la", "os`lu", "os`la", "os`lem",
	"o`sle", "o`sle", "osiol", "donkey",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	5, 800, 0, -10, -10, -1, 0,
	0, 0, 0, 0, 0, "kopni`ecie", 0,
	"",
	"", "",
	~WRL_CIALO, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB | P_ZAB, 0
    },

    { "byk", "byka", "bykowi", "byka", "bykiem",
	"byku", "byku", "byk", "bull",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	6, 4000, 1, -10, -10, -1, 1,
	0, 0, 0, 0, 0, "uderzenie rog`ow", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB | P_ZAB | P_ROG, &zr_byk
    },

    { "koza", "kozy", "kozie", "koz`e", "koz`a",
	"kozie", "kozo", "koza", "goat",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	4, 240, -3, -10, -10, 0, -1,
	0, 0, 0, 0, 0, "uderzenie rog`ow", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB | P_ZAB | P_ROG, 0
    },

    { "ptak", "ptaka", "ptakowi", "ptaka", "ptakiem",
	"ptaku", "ptaku", "ptak", "bird",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_POWIETRZE,
	2, 20, -10, -10, -10, 0, -8,
	0, 0, 0, 0, 0, "dziobni`ecie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA, 0
    },

    { "pies", "psa", "psu", "psa", "psem",
	"psie", "psie", "pies", "dog",
	RACE_DETECT_EVIL | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD | POR_PO_WODZIE,
	2, 100, -5, -10, -10, 0, -1,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_SZYJA, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_LAPA | P_LEB | P_ZAB | P_KIEL, 0
    },

    { "chimera", "chimery", "chimerze", "chimer`e", "chimer`a",
	"chimerze", "chimero", "chimera", "chimera",
	RACE_INFRAVISION | RACE_DETECT_EVIL
	 | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_MUTE | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	7, 4000, 2, -5, -5, 1, 1,
	5, 0, 0, 2, 2, "ugryzienie", 0,
	"Human Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO,
	P_FLAKI | P_SERCE | P_OGON | P_SKRZYDLA | P_JEZYK | P_LAPA
	 | P_LEB | P_ZAB | P_KIEL, 0
    },

    { "orze`l", "or`la", "or`lu", "or`la", "or`lem",
	"or`le", "or`le", "orzel", "eagle",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	3, 40, 1, -7, -7, 0, 0,
	0, 0, 0, 0, 0, "szarpni`ecie szpon`ow", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB | P_SZPON, 0
    },

    { "pegaz", "pegaza", "pegazowi", "pegaza", "pegazem",
	"pegazie", "pegazie", "pegaz", "pegasus",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	6, 4000, 1, -5, -5, 0, 1,
	2, 0, 0, 0, 0, "kopni`ecie", 0,
	"Vampire Werewolf",
	"", "",
	~( WRL_GLOWA | WRL_NADGARSTEK ), /* jak u jednorozca */
	PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_SKRZYDLA | P_LEB, 0
    },

    { "rak", "raka", "rakowi", "raka", "rakiem",
	"raku", "raku", "rak", "crayfish",
	RACE_MUTE | RACE_NO_BLOOD,
	ODD_WODA, POR_POD_WODA | POR_LAD, /* rak sie dusi na ladzie, ale moze wyjsc? */
	1, 4, -12, -15, -15, -7, -12,
	0, 0, 0, 0, 0, "szczypce", 3,
	"",
	"", "",
	~WRL_HS, PAPU_RYBNE | PAPU_ROSLINNE,
	P_FLAKI | P_GLOWA | P_NOGI, 0
    },

    { "`zaba", "`zaby", "`zabie", "`zab`e", "`zab`a",
	"`zabie", "`zabo", "zaba", "frog",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_PO_WODZIE | POR_LAD,
	1, 4, -15, -15, -15, -8, -15,
	0, 0, 0, 0, 0, "jad", 1,
	"",
	"", "",
	~WRL_HS, PAPU_MIESNE,
	P_NONE, 0
    },

    { "centaur", "centaura", "centaurowi", "centaura", "centaurem",
	"centaurze", "centaurze", "centaur", "centaur",
	RACE_DETECT_EVIL | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	5, 4000, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, "uderzenie", 0,
	"Vampire Werewolf Orc Troll Goblin Ogre Giant",
	"", "",
	WRL_CIALO | WRL_NOGI | WRL_STOPY | WRL_WOKOL, PAPU_DOWOLNE,
	P_FLAKI  | P_GLOWA | P_RECE | P_NOGI | P_SERCE | P_OGON | P_ZAB, 0
    },

    { "s`ep", "s`epa", "s`epowi", "s`epa", "s`epem",
	"s`epie", "s`epie", "sep", "vulture",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_POWIETRZE,
	3, 40, -1, -10, -10, -6, -2,
	0, 0, 0, 0, 0, "szarpni`ecie dzioba", 0,
	"",
	"", "",
	~WRL_HS, PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB | P_SZPON, 0
    },

    { "owca", "owcy", "owcy", "owc`e", "owc`a",
	"owcy", "owco", "owca", "sheep",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	3, 240, 0, -13, -13, -5, -2,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_LEB, 0
    },

    { "kaczka", "kaczki", "kaczce", "kaczk`e", "kaczk`a",
	"kaczce", "kaczko", "kaczka", "duck",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_PO_WODZIE | POR_LAD,
	2, 20, -10, -12, -13, -10, -8,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB, 0
    },

    { "wilk", "wilka", "wilkowi", "wilka", "wilkiem",
	"wilku", "wilku", "wilk", "wolf",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	3, 200, 1, -9, -10, 2, 0,
	1, 0, 0, 1, 1, "ugryzienie", 0,
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Sektoid",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_LAPA | P_LEB | P_ZAB | P_KIEL, 0
    },

    { "puma", "pumy", "pumie", "pum`e", "pum`a",
	"pumie", "pumo", "puma", "puma",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	4, 500, 0, -9, -10, 3, 0,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_LAPA | P_LEB | P_KIEL, 0
    },

    { "gepard", "geparda", "gepardowi", "geparda", "gepardem",
	"gepardzie", "gepardzie", "gepard", "cheetah",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	4, 320, 1, -10, -10, 2, 0,
	0, 0, 3, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_LAPA | P_LEB | P_KIEL, 0
    },

    { "`lab`ed`x", "`lab`edzia", "`lab`edziowi", "`lab`edzia", "`lab`edziem",
	"`lab`edziu", "`lab`edziu", "labedz", "swan",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_PO_WODZIE | POR_LAD | POR_POWIETRZE,
	2, 40, -10, -10, -10, -8, -5,
	0, 0, 0, 0, 0, "szczypni`ecie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB, 0
    },

    { "zaj`ac", "zaj`aca", "zaj`acowi", "zaj`aca", "zaj`acem",
	"zaj`acu", "zaj`acu", "zajac", "hare",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	2, 20, -10, -10, -10, 4, -10,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_USZY | P_LAPA | P_LEB, 0
    },

    { "dzik", "dzika", "dzikowi", "dzika", "dzikiem",
	"dziku", "dziku", "dzik", "boar",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD | POR_PO_WODZIE,
	3, 500, 2, -10, -10, 0, 0,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_DOWOLNE,
	P_FLAKI | P_NOGI | P_SERCE | P_LEB | P_KIEL, 0
    },

    { "lis", "lisa", "lisowi", "lisa", "lisem",
	"lisie", "lisie", "lis", "fox",
	RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD,
	2, 40, -5, -9, -10, 1, -1,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_LAPA | P_LEB | P_ZAB | P_KIEL, 0
    },

    { "lew", "lwa", "lwu", "lwa", "lwem",
	"lwie", "lwie", "lew", "lion",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	6, 1400, 2, -9, -9, 1, 1,
	1, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_LAPA | P_LEB | P_ZAB | P_KIEL, 0
    },

    { "s`lo`n", "s`lonia", "s`loniowi", "s`lonia", "s`loniem",
	"s`loniu", "s`loniu", "slon", "elephant",
	RACE_PROTECTION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	8, 1200, 4, -10, -8, -8, 4,
	5, 0, 0, 2,  2, "uderzenie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_USZY | P_LEB | P_KIEL, 0
    },

    { "wielb`l`ad", "wielb`l`ada", "wielb`l`adowi", "wielb`l`ada", "wielb`l`adem",
	"wielb`l`adzie", "wielb`l`adzie", "wielblad", "camel",
	RACE_PROTECTION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	7, 3200, 2, -10, -10, -2, 2,
	0, 0, 0,-5, 0, "kopni`ecie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB | P_ZAB, 0
    },

    { "sfinks", "sfinksa", "sfinksowi", "sfinksa", "sfinksem",
	"sfinksie", "sfinksie", "sfinks", "sphinx",
	RACE_INFRAVISION | RACE_DETECT_EVIL | RACE_DETECT_INVIS
	 | RACE_DETECT_HIDDEN | RACE_PROTECTION | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	7, 1400, 1, 4, 4, 0, 1,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_GLOWA | P_SERCE | P_OGON | P_LAPA | P_ZAB, 0
    },

    { "wiewi`orka", "wiewi`orki", "wiewi`orce", "wiewi`ork`e", "wiewi`ork`a",
	"wiewi`orce", "wiewi`orko", "wiewiorka", "squirrel",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	1, 4, -9, -10, -10, 2, -9,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE, 0
    },

    { "jele`n", "jelenia", "jeleniowi", "jelenia", "jeleniem",
	"jeleniu", "jeleniu", "jelen", "deer",
	RACE_PROTECTION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	5, 800, 1, -10, -10, 0, 1,
	0, 0, 1, 0, 0, "uderzenie rog`ow", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_LEB, 0
    },

    { "sok`o`l", "soko`la", "soko`lowi", "soko`la", "soko`lem",
	"sokole", "sokole", "sokol", "falcon",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	3, 40, -3, -10, -10, 1, -4,
	0, 0, 0, 0, 0, "szarpni`ecie szpon`ow", 0,
	"",
	"", "",
	~WRL_HS, PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB | P_SZPON, 0
    },

    { "antylopa", "antylopy", "antylopie", "antylop`e", "antylop`a",
	"antylopie", "antylopo", "antylopa", "antelope",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	4, 800, 0, -10, -10, 2, 0,
	0, 0, 1, 0, 0, "uderzenie rog`ow", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_LEB | P_ROG, 0
    },

    { "sowa", "sowy", "sowie", "sow`e", "sow`a",
	"sowie", "sowo", "sowa", "owl",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_POWIETRZE,
	2, 20, -6, -7, 0, 0, -6,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB, 0
    },

    { "gryf", "gryfa", "gryfowi", "gryfa", "gryfem",
	"gryfie", "gryfie", "gryf", "griffin",
	RACE_INFRAVISION | RACE_DETECT_EVIL
	 | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	7, 2000, 2, 0, 0, 1, 1,
	5, 0, 0, 2, 2, "uderzenie", 0,
	"Vampire Werewolf",
	"", "",
	~WRL_HS, PAPU_MIESNE | PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_OGON | P_SKRZYDLA | P_JEZYK | P_LAPA
	 | P_LEB | P_ZAB | P_KIEL, 0
    },

    { "chomik", "chomika", "chomikowi", "chomika", "chomikiem",
	"chomiku", "chomika", "chomik", "hamster",
	RACE_PASSDOOR | RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	1, 4, -12, -12, -10, 2, -12,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"cat",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_OGON | P_LEB | P_ZAB, 0
    },

    { "sarna", "sarny", "sarnie", "sarn`e", "sarn`a",
	"sarnie", "sarno", "sarna", "roe",
	RACE_PROTECTION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	5, 800, 0, -10, -10, 1, 0,
	0, 0, 1, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_LEB, 0
    },

    { "g`e`s", "g`esi", "g`esi", "g`e`s", "g`esi`a",
	"g`esi", "g`esi", "ges", "goose",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_PO_WODZIE | POR_LAD | POR_POWIETRZE,
	2, 40, -10, -13, -13, -7, -8,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB, 0
    },

    { "papuga", "papugi", "papudze", "papug`e", "papug`a",
	"papudze", "papugo", "papuga", "parrot",
	RACE_NO_ABILITIES,
	ODD_POWIETRZEM, POR_POWIETRZE,
	2, 4, -10, -5, -5, -2, -10,
	0, 0, 0, 0, 0, "atak", 1,
	"Wampire Werewolf",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB, &zr_papuga
    },

    { "jastrz`ab", "jastrz`ebia", "jastrz`ebiowi", "jastrz`ebia", "jastrz`ebiem",
	"jastrz`ebiu", "jastrz`ebiu", "jastrzab", "hawk",
	RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_POWIETRZE,
	3, 40, 0, -10, -10, 0, 0,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB | P_SZPON, 0
    },

    { "tygrys", "tygrysa", "tygrysowi", "tygrysa", "tygrysem",
	"tygrysie", "tygrysie", "tygrys", "tiger",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	4, 500, 2, -10, -10, 1, 1,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_LAPA | P_LEB | P_KIEL, 0
    },

    { "lampart", "lamparta", "lampartowi", "lamparta", "lampartem",
	"lamparcie", "lamparcie", "lampart", "leopard",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	4, 320, 1, -10, -10, 2, 0,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_LAPA | P_LEB | P_KIEL, 0
    },

    { "pantera", "pantery", "panterze", "panter`e", "panter`a",
	"panterze", "pantero", "pantera", "panther",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	4, 500, 1, -10, -10, 2, 1,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_LAPA | P_LEB | P_KIEL, 0
    },

    { "mysz", "myszy", "myszy", "mysz", "mysz`a",
	"myszy", "myszy", "mysz", "mouse",
	RACE_PASSDOOR | RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD,
	1, 1, -15, -10, -10, 5, -10,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"Cat",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_LEB, 0
    },

    { "bazyliszek", "bazyliszka", "bazyliszkowi", "bazyliszka", "bazyliszkiem",
	"bazyliszku", "bazyliszku", "bazyliszek", "basilisk",
	RACE_INFRAVISION | RACE_DETECT_EVIL | RACE_DETECT_INVIS
	 | RACE_DETECT_HIDDEN | RACE_PROTECTION | RACE_MUTE | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	9, 10000, 2, 1, 1, 2, 2,
	2, 2, 0, 2, 2, "atak", 1,
	"Human Elf Halfelf Drow Dwarf Halfdwarf Gnome Hobbit Sektoid Duergar",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_SKRZYDLA | P_LAPA | P_SZPON, 0
    },

    { "gremlin", "gremlina", "gremlinowi", "gremlina", "gremlinem",
	"gremlinie", "gremlinie", "gremlin", "gremlin",
	RACE_INFRAVISION | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 240, -1, -2, 0, 2, -1,
	0, 0, 0, 0, 0, "pi`e`s`c", 2,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "mantykora", "mantykory", "mantykorze", "mantykor`e", "mantykor`a",
	"mantykorze", "mantykoro", "mantykora", "manticore",
	RACE_INFRAVISION | RACE_DETECT_EVIL
	 | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	6, 4000, 4, -5, -5, 2, 1,
	0, 0, 0, 2, 2, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_SKRZYDLA | P_JEZYK | P_LAPA
	 | P_LEB | P_ZAB | P_KIEL, 0
    },

    { "wywerna", "wywerny", "wywernie", "wywern`e", "wywern`a",
	"wywernie", "wywerno", "wywerna", "wyvern",
	RACE_WEAPON_WIELD | RACE_INFRAVISION | RACE_DETECT_EVIL
	 | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_MUTE | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	9, 10000, 3, -5, 1, 3, 2,
	0, 0, 0, 2, 2, "uderzenie ogona", 0,
	"",
	"", "",
	WRL_STOPY, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_SKRZYDLA | P_JEZYK | P_LAPA
	 | P_LEB | P_ZAB | P_KIEL, 0
    },

    { "czapla", "czapli", "czapli", "czapl`e", "czapl`a",
	"czapli", "czaplo", "czapla", "heron",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	3, 40, -5, -10, -10, 0, -5,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB, 0
    },

    { "mewa", "mewy", "mewie", "mew`e", "mew`a",
	"mewie", "mewo", "mewa", "gull",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_POWIETRZE,
	2, 20, -10, -10, -10, 0, -10,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB, 0
    },

    { "`zuraw", "`zurawia", "`zurawiowi", "`zurawia", "`zurawiem",
	"`zurawiu", "`zurawiu", "zuraw", "crane",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD | POR_POWIETRZE,
	3, 40, -5, -10, -10, -2, -5,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_SKRZYDLA | P_LEB, 0
    },

    { "o`smiornica", "o`smiornicy", "o`smiornicy", "o`smiornic`e", "o`smiornic`a",
	"o`smiornicy", "o`smiornico", "osmiornica", "octopus",
	RACE_PROTECTION | RACE_MUTE | RACE_OPENS_DOORS,
	ODD_WODA, POR_POD_WODA,
	7, 1000, 2, -15, -10, 0, 1,
	0, 0, 0, 0, 0, "uderzenie macki", 0,
	"",
	"", "",
	~WRL_HS, PAPU_RYBNE,
	P_FLAKI | P_SERCE, 0
    },

    { "delfin", "delfina", "delfinowi", "delfina", "delfinem",
	"delfinie", "delfinie", "delfin", "dolphin",
	RACE_MUTE | RACE_INFRAVISION,
	ODD_WODA | ODD_POWIETRZEM, POR_POD_WODA, /* dlugo wstrzymuja oddech pod woda, chociaz nia nie oddychaja... */
	6, 3000, 0, -3, -3, 1, 0,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_PLETWA, 0
    },

    { "zebra", "zebry", "zebrze", "zebr`e", "zebr`a",
	"zebrze", "zebro", "zebra", "zebra",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	6, 2400, 1, -10, -10, -2, 0,
	0, 0, 0, 0, 0, "kopni`ecie", 0,
	"Lion Puma Cheetah Tiger Leopard Panther",
	"", "",
	~( WRL_GLOWA | WRL_NADGARSTEK ), /* jak u jednorozca */
	PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB, 0
    },

    { "goryl", "goryla", "gorylowi", "goryla", "gorylem",
	"gorylu", "gorylu", "goryl", "gorilla",
	RACE_WEAPON_WIELD | RACE_MUTE | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	4, 2000, 2, -8, -5, 0, 2,
	0, 0, 0, 0, 0, "uderzenie", 0,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "szympans", "szympansa", "szympansowi", "szympansa", "szympansem",
	"szympansie", "szympansie", "szympans", "chimpanzee",
	RACE_WEAPON_WIELD | RACE_MUTE | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	3, 500, 0, -5, -5, 2, 0,
	0, 0, 0, 0, 0, "uderzenie", 0,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "`zyrafa", "`zyrafy", "`zyrafie", "`zyraf`e", "`zyraf`a",
	"`zyrafie", "`zyrafo", "zyrafa", "giraffe",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	7, 4000, 0, -12, -12, -5, 1,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~( WRL_GLOWA | WRL_NADGARSTEK | WRL_SZYJA ), /* jak u jednorozca + szyja */
	PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB, 0
    },

    { "orka", "orki", "orce", "ork`e", "ork`a",
	"orce", "orko", "orka", "orca",
	RACE_DETECT_HIDDEN | RACE_MUTE | RACE_INFRAVISION,
	ODD_WODA | ODD_POWIETRZEM, POR_POD_WODA, /* komentarz jak u delfina */
	7, 20000, 1, -5, -5, 1, 1,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_ZAB | P_PLETWA, 0
    },

    { "hiena", "hieny", "hienie", "hien`e", "hien`a",
	"hienie", "hieno", "hiena", "hyena",
	RACE_INFRAVISION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	3, 100, 0, -10, -10, 1, 0,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_OGON | P_JEZYK | P_USZY | P_LAPA | P_LEB | P_ZAB, 0
    },

    { "rekin", "rekina", "rekinowi", "rekina", "rekinem",
	"rekinie", "rekinie", "rekin", "shark",
	RACE_PROTECTION | RACE_MUTE | RACE_INFRAVISION,
	ODD_WODA, POR_POD_WODA,
	7, 4000, 1, -10, -10, 1, 0,
	1, 0, 0, 0, 1, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_MIESNE | PAPU_RYBNE,
	P_FLAKI | P_SERCE | P_ZAB | P_PLETWA, 0
    },

    { "hipopotam", "hipopotama", "hipopotamowi", "hipopotama", "hipopotamem",
	"hipopotamie", "hipopotamie", "hipopotam", "hippopotamus",
	RACE_PROTECTION | RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	7, 10000, 2, -13, -10, -10, 2,
	0, 0, 0, 2,  2, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_KIEL, 0
    },

    { "wieloryb", "wieloryba", "wielorybowi", "wieloryba", "wielorybem",
	"wielorybie", "wielorybie", "wieloryb", "whale",
	RACE_PROTECTION | RACE_MUTE | RACE_INFRAVISION,
	ODD_WODA | ODD_POWIETRZEM, POR_POD_WODA, /* jak delfin */
	12, 40000, 5, -15, -10, -15, 5,
	 0, 0, 0, 5, 5, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_RYBNE | PAPU_ROSLINNE,
	P_NONE, 0
    },

    { "anio`l", "anio`la", "anio`lowi", "anio`la", "anio`lem",
	"aniele", "aniele", "aniol", "angel",
	RACE_INFRAVISION | RACE_DETECT_EVIL
	 | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_PASSDOOR
	 | RACE_PROTECTION | RACE_SANCT | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_WODA | ODD_POWIETRZEM, POR_POD_WODA | POR_LAD | POR_POWIETRZE, /* jak bog */
	7, 320, 2, 2, 2, 3, 1,
	10, 15, 5, 0, 0, "pot`ega", 2,
	"Giant Ogre Orc Kobold Troll Hobgoblin Goblin Githyanki Vampire Werewolf Mindflayer",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS | P_SKRZYDLA, 0
    },

    { "cyklop", "cyklopa", "cyklopowi", "cyklopa", "cyklopem",
	"cyklopie", "cyklopie", "cyklop", "cyclopes",
	RACE_DETECT_EVIL | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD | RACE_OPENS_DOORS,
	ODD_POWIETRZEM, POR_LAD,
	7, 2000, 3, -1, 1, 2, 2,
	5, 0, 0, 3, 3, "walni`ecie", 0,
	"Human Elf Halfelf Hobbit Gnome Halfdwarf",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "tryton", "trytona", "trytonowi", "trytona", "trytonem",
	"trytonie", "trytonie", "tryton", "triton",
	RACE_WEAPON_WIELD | RACE_OPENS_DOORS | RACE_INFRAVISION,
	ODD_WODA, POR_POD_WODA | POR_PO_WODZIE,
	4, 320, 1, -2, -1, 0, 0,
	0, 0, 0, 0, 0, "uderzenie", 0,
	"",
	"", "",
	WRL_NOGI | WRL_STOPY, PAPU_DOWOLNE,
	P_FLAKI | P_GLOWA | P_RECE | P_SERCE | P_JEZYK | P_PLETWA, 0
    },

    { "baatezu", "baatezu", "baatezu", "baatezu", "baatezu",
	"baatezu", "baatezu", "baatezu", "baatezu",
	RACE_INFRAVISION | RACE_DETECT_EVIL
	 | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_PROTECTION
	 | RACE_WEAPON_WIELD | RACE_POISONIMM | RACE_OPENS_DOORS
	 | RACE_PROTECT_GOOD,
	ODD_WODA | ODD_POWIETRZEM, POR_POD_WODA | POR_LAD | POR_POWIETRZE, /* bog/aniol */
	8, 2000, 3, 2, 2, 3, 3,
	10, 10, 5, 0, 0, "pot`ega", 2,
	"Human Elf Halfelf Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Troll Hobgoblin Goblin Faerie Gnome Sektoid",
	"", "",
	WRL_HS, PAPU_WSZYSTKO | PAPU_MIESNE | PAPU_RYBNE,
	P_HS | P_SKRZYDLA | P_KIEL, 0
    },

    { "golem", "golema", "golemowi", "golema", "golemem",
	"golemie", "golemie", "golem", "golem",
	RACE_WEAPON_WIELD | RACE_OPENS_DOORS | RACE_INFRAVISION
	 | RACE_DETECT_HIDDEN | RACE_POISONIMM | RACE_NO_BLOOD,
	ODD_POWIETRZEM | ODD_WODA, POR_LAD,
	8, 4000, 4, 0, 0, -3, 5,
	6, 0, 0, -1, -1, "pi`e`s`c", 2,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_GLOWA | P_RECE | P_NOGI, 0
    },

    { "duergar", "duergara", "duergarowi", "duergara", "duergarem",
	"duergarze", "duergarze", "duergar", "duergar",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD
	| RACE_OPENS_DOORS | RACE_POISONIMM,
	ODD_POWIETRZEM, POR_LAD,
	3, 320, 2, 0, 0, 1, 2,
	3, -1, 3, 0, 0, "pi`e`s`c", 2,
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold Dwarf Drow Elf Human Halfelf Halfdwarf Hobbit Sektoid Faerie Mindflayer",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS, 0
    },

    { "smokowiec", "smokowca", "smokowcowi", "smokowca", "smokowcem",
	"smokowcu", "smokowcu", "smokowiec", "draconian",
	RACE_WEAPON_WIELD | RACE_OPENS_DOORS | RACE_INFRAVISION
	 | RACE_DETECT_HIDDEN,
	ODD_POWIETRZEM, POR_LAD,
	6, 1400, 3, 2, 1, 0, 3,
	5, 5, 0, 0, 1, "pi`e`s`c", 2,
	"",
	"", "",
	WRL_HS, PAPU_DOWOLNE,
	P_HS | P_SKRZYDLA, 0
    },

    { "ryj`owka", "ryj`owki", "ryj`owce", "ryj`owk`e", "ryj`owk`a",
	"ryj`owce", "ryj`owko", "ryjowka", "shrew",
	RACE_PASSDOOR | RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM | ODD_WODA, POR_LAD | POR_POD_WODA,
	1, 20, -14, -10, -10, 5, -10,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_LEB | P_OGON, 0
    },

    { "b`obr", "bobra", "bobrowi", "bobra", "bobrem",
	"bobrze", "bobrze", "bobr", "beaver",
	RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM | ODD_WODA, POR_LAD | POR_POD_WODA,
	2, 40, -9, -10, -10, 3, -5,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_LEB | P_OGON, 0
    },

    { "je`z", "je`za", "je`zowi", "je`za", "je`zem",
	"je`zu", "je`zu", "jez", "hedgehog",
	RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD,
	1, 12, -15, -10, -10, 3, -7,
	0, 0, 0, 0, 0, "atak", 1,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_LEB, &zr_jez
    },

    { "mangusta", "mangusty", "mangu`scie", "mangust`e", "mangust`a",
	"mangu`scie", "mangusto", "mangusta", "mongoose",
	RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD | POR_PO_WODZIE,
	2, 20, -8, -10, -10, 3, -5,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_MIESNE,
	P_FLAKI | P_SERCE | P_LEB | P_OGON, 0
    },

    { "`lasica", "`lasicy", "`lasicy", "`lasic`e", "`lasic`a",
	"`lasicy", "`lasico", "lasica", "weasel",
	RACE_MUTE | RACE_INFRAVISION,
	ODD_POWIETRZEM, POR_LAD | POR_PO_WODZIE,
	2, 20, -8, -10, -10, 5, -6,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_MIESNE,
	P_FLAKI | P_SERCE | P_LEB | P_OGON, 0
    },

    { "suse`l", "sus`la", "sus`lowi", "sus`la", "sus`lem",
	"su`sle", "su`sle", "susel", "gopher",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	1, 4, -9, -10, -10, 2, -8,
	0, 0, 0, 0, 0, "ugryzienie", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_SERCE | P_LEB | P_OGON, 0
    },

    { "kozica", "kozicy", "kozicy", "kozic`e", "kozic`a",
	"kozicy", "kozico", "kozica", "chamois",
	RACE_MUTE,
	ODD_POWIETRZEM, POR_LAD,
	4, 240, -2, -10, -10, 2, -2,
	0, 0, 0, 0, 0, "uderzenie rog`ow", 0,
	"",
	"", "",
	~WRL_HS, PAPU_ROSLINNE,
	P_FLAKI | P_NOGI | P_SERCE | P_OGON | P_LEB | P_ZAB | P_ROG, 0
    }
};


/*
 * Lam 31.12.2007: tablica, optymalizacja, wiecej czesci ciala, obsluga
 *  podstawiania nazw ras, obsluga ciezaru czesci ciala.
 * Konieczne zachowanie dokladnej kolejnosci bitow z race_table[ ].parts!
 */
struct czesc_ciala czesci_ciala[ ] =
{
    { &OBJ_VNUM_FLAKI,	  0, 200, TRUE,  NULL },
    { &OBJ_VNUM_GLOWA,	  0,  80, FALSE, "Ostatni cios str`aci`l $2 g`low`e." },
    { &OBJ_VNUM_REKA,	  0, 112, FALSE, "Ostatni cios oderwa`l r`ek`e $1 od $c cia`la." },
    { &OBJ_VNUM_NOGA,	  0, 180, FALSE, "Odr`abana noga $1 odpada od $c cia`la." },
    { &OBJ_VNUM_SERCE,	  0,  30, TRUE,  "Serce $1 wyrywa si`e z $c klatki piersiowej." },
    { &OBJ_VNUM_OGON,	  0,  40, FALSE, "Odci`ety ogon $1 odpada od $c cia`la." },
    { &OBJ_VNUM_SKRZYDLO, 0, 170, FALSE, "Odci`ete skrzyd`lo $1 odpada od $c cia`la." },
    { &OBJ_VNUM_UCHO,	  0,   5, FALSE, "Ostatni cios oderwa`l $2 ucho." },
    { &OBJ_VNUM_LAPA,	  0, 120, FALSE, "Ostatni cios oderwa`l `lap`e $1 od $c cia`la." },
    { &OBJ_VNUM_LEB,	  0,  80, FALSE, "Ostatni cios str`aci`l $2 `leb." },
    { &OBJ_VNUM_ZAB,	  0,   2, TRUE,  "Ostatni cios wybi`l $2 z`ab." },
    { &OBJ_VNUM_KIEL,	  0,   2, TRUE,  "Ostatni cios wybi`l $2 kie`l." },
    { &OBJ_VNUM_ROG,	  0,  20, FALSE, "Dogorywaj`ac, $n zgubi`l$a r`og." },
    { &OBJ_VNUM_SZPON,	  0,   3, TRUE,  "Dogorywaj`ac, $n zgubi`l$a szpon." },
    { &OBJ_VNUM_PLETWA,	  0,  80, FALSE, "P`letwa $1 odpada od $c cia`la." },
/*  { &OBJ_VNUM_JEZYK,	  0,  16, TRUE,  "J`ezyk $1..." }, */
    { NULL,		  0,   0, FALSE, NULL }
};


/*
 * Drunkeness string changes.
 */
const	struct	struckdrunk	drunk		[  ]		=
{
    { 3, 10,
	{ "a", "a", "a", "A", "aa", "ah", "Ah", "ao", "au", "oa", "ahhhh" }  },
    { 8, 5,
	{ "b", "b", "b", "B", "B", "vb" }				     },
    { 3, 5,
	{ "c", "c", "C", "cz", "s", "z" }				     },
    { 5, 2,
	{ "d", "d", "D" }						     },
    { 3, 3,
	{ "e", "e", "eh", "E" }						     },
    { 4, 5,
	{ "f", "f", "ff", "fff", "fFf", "F" }				     },
    { 8, 3,
	{ "g", "g", "G", "Ghg" }					     },
    { 9, 6,
	{ "h", "h", "hh", "hhh", "Hhh", "HhH", "H" }			     },
    { 7, 6,
	{ "i", "i", "Iii", "ii", "iI", "Ii", "I" }			     },
    { 9, 5,
	{ "j", "j", "jj", "Jj", "jJ", "J" }				     },
    { 7, 3,
	{ "k", "k", "K", "kah" }					     },
    { 3, 2,
	{ "l", "l", "L" }						     },
    { 5, 8,
	{ "m", "m", "mm", "mmm", "mmmm", "mmmmm", "MmM", "mM", "M" }	     },
    { 6, 6,
	{ "n", "n", "nn", "Nn", "nnn", "nNn", "N" }			     },
    { 3, 6,
	{ "o", "o", "ooo", "ao", "aOoo", "Ooo", "ooOo" }		     },
    { 3, 2,
	{ "p", "p", "P" }						     },
    { 5, 5,
	{ "q", "q", "Q", "ku", "ququ", "kukeleku" }			     },
    { 4, 2,
	{ "r", "r", "R" }						     },
    { 2, 5,
	{ "s", "ss", "zZss", "ZSss", "sSzss", "sSss" }			     },
    { 5, 2,
	{ "t", "t", "T" }						     },
    { 3, 6,
	{ "u", "u", "uh", "Uh", "Uhhuh", "uhU", "uhhu" }		     },
    { 4, 2,
	{ "v", "v", "V" }						     },
    { 4, 2,
	{ "w", "w", "W" }						     },
    { 5, 6,
	{ "x", "x", "X", "ks", "iks", "kz", "xz" }			     },
    { 3, 2,
	{ "y", "y", "Y" }						     },
    { 2, 8,
	{ "z", "z", "ZzzZz", "Zzz", "Zsszzsz", "szz", "sZZz", "ZSz", "zZ" }  }
};


/*
 * Attribute bonus tables.
 */
/* todam, carry, wield, uwalnianie */
const	struct	str_app_type	str_app		[ 31 ]		=
{
    {-15,   0,  0, 0 },  /* 0	*/
    {-12,   4,  1, 0 },  /* 1	*/
    {-10,   8,  2, 0 },
    { -8,  16,  3, 0 },  /* 3	*/
    { -5,  24,  4, 0 },
    { -4,  32,  5, 0 },  /* 5	*/
    { -3,  40,  6, 0 },
    { -2,  64,  7, 0 },
    { -1,  96,  8, 0 },
    {  0, 112,  9, 0 },
    {  0, 128, 10, 0 }, /* 10	*/
    {  0, 144, 11, 0 },
    {  0, 160, 12, 0 },
    {  0, 176, 13, 1 }, /* 13	*/
    {  1, 192, 14, 1 },
    {  1, 208, 15, 2 }, /* 15	*/
    {  2, 224, 16, 2 },
    {  3, 240, 22, 4 },
    {  5, 256, 25, 4 }, /* 18	*/
    {  8, 320, 30, 7 },
    { 10, 384, 35, 7 }, /* 20	*/
    { 13, 448, 40, 10 },
    { 16, 576, 45, 10 },
    { 18, 700, 50, 15 },
    { 20, 800, 55, 15 },
    { 25, 900, 60, 20 }, /* 25 */
    { 30,1000, 65, 25 },
    { 35,1100, 70, 30 },
    { 42,1200, 80, 50 },
    { 50,1300, 90, 80 },
    { 60,1500,100, 99 }  /* 30 */
};


/* learn (%), mana */
const	struct	int_app_type	int_app		[ 31 ]		=
{
    {  3, 0 },	/*  0 */
    {  5, 0 },	/*  1 */
    {  7, 0 },
    {  8, 0 },	/*  3 */
    {  9, 0 },
    { 10, 0 },	/*  5 */
    { 11, 0 },
    { 12, 0 },
    { 13, 0 },
    { 15, 0 },
    { 17, 0 },	/* 10 */
    { 19, 1 },
    { 22, 1 },
    { 25, 1 },
    { 28, 1 },
    { 31, 2 },	/* 15 */
    { 33, 2 },
    { 35, 2 },
    { 37, 3 },	/* 18 */
    { 40, 3 },
    { 44, 4 },	/* 20 */
    { 48, 4 },
    { 53, 5 },
    { 59, 5 },
    { 66, 6 },
    { 75, 6 }, /* 25 */
    { 80, 7 },
    { 85, 7 },
    { 90, 8 },
    { 95, 8 },
    { 99, 9 }  /* 30 */
};


/* prac/lv, podstawa OPC */
const	struct	wis_app_type	wis_app		[ 31 ]		=
{
    { 0, -50 },	/*  0 */
    { 0, -50 },	/*  1 */
    { 0, -50 },
    { 0, -40 },	/*  3 */
    { 0, -30 },
    { 1, -20 },	/*  5 */
    { 1, -10 },
    { 1,   0 },
    { 1,   0 },
    { 2,   2 },
    { 2,   4 },	/* 10 */
    { 2,   6 },
    { 2,  10 },
    { 2,  10 },
    { 2,  15 },
    { 3,  15 },	/* 15 */
    { 3,  20 },
    { 4,  20 },
    { 4,  25 },	/* 18 */
    { 5,  25 },
    { 5,  30 },	/* 20 */
    { 6,  35 },
    { 6,  40 },
    { 6,  45 },
    { 7,  50 },
    { 7,  55 },	/* 25 */
    { 8,  60 },
    { 8,  65 },
    { 9,  70 },
    { 9,  75 },
    { 10, 80 }	/* 30 */
};


/* AC, hitroll */
const	struct	dex_app_type	dex_app		[ 31 ]		=
{
    { - 20, -30 },	/* 0 */
    { - 20, -25 },	/* 1 */
    { - 20, -20 },
    { -	20, -15 },
    { -	20, -12 },
    { -	20,  -8 },	/* 5 */
    { -	15,  -6 },
    { - 10,  -4 },
    { -  6,  -2 },
    { -  3,  -1 },
    {	 0,   0 },	/* 10 */
    {	 0,   0 },
    {	 0,   0 },
    {	 0,   0 },
    {	 1,   1 },
    {    1,   3 },	/* 15 */
    {    3,   5 },
    {    6,   7 },
    {   10,   9 },
    {   15,  12 },
    {   20,  15 },	/* 20 */
    {   25,  18 },
    {   30,  21 },
    {   35,  24 },
    {   40,  27 },
    {   45,  30 },	/* 25 */
    {   50,  33 },
    {   55,  36 },
    {   60,  40 },
    {   65,  45 },
    {   70,  50 }	/* 30 */
};


/* hitp, wyparowanie, shock (nigdy niczemu nie sluzyl) -Lam */
const	struct	con_app_type	con_app		[ 31 ]		=
{
    { -4, 0, 20 },   /*  0 */
    { -3, 0, 25 },   /*  1 */
    { -2, 0, 30 },
    { -2, 0, 35 },   /*  3 */
    { -1, 0, 40 },
    { -1, 0, 45 },   /*  5 */
    { -1, 0, 50 },
    {  0, 0, 55 },
    {  0, 0, 60 },
    {  0, 0, 65 },
    {  0, 0, 70 },   /* 10 */
    {  0, 0, 75 },
    {  0, 0, 80 },
    {  0, 0, 85 },
    {  0, 0, 88 },
    {  1, 0, 90 },   /* 15 */
    {  2, 0, 95 },
    {  2, 0, 97 },
    {  3, 0, 99 },   /* 18 */
    {  3, 0.01, 99 },
    {  4, 0.02, 99 },   /* 20 */
    {  4, 0.03, 99 },
    {  5, 0.04, 99 },
    {  6, 0.05, 99 },
    {  7, 0.06, 99 },
    {  8, 0.08, 99 },   /* 25 */
    {  8, 0.10, 99 },
    {  9, 0.12, 99 },
    {  9, 0.14, 99 },
    { 10, 0.16, 99 },
    { 10, 0.20, 99 }	  /* 30 */
};


const struct object_type typy_przedmiotow[ ITEM_MAX ] =
{
    {
	"", "",		/* 0 */
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"light", "`swiat`lo",
	{ "", "czy p`lomie`n", "czas", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"scroll", "pergamin",
	{ "poziom", "czar 1", "czar 2", "czar 3", "", "" },
	{ 0, VAL_SPELL, VAL_SPELL, VAL_SPELL, 0, 0 }
    },
    {
	"wand", "r`o`zd`zka",
	{ "poziom", "maks. `ladunki", "`ladunki", "czar", "", "" },
	{ 0, 0, 0, VAL_SPELL, 0, 0 }
    },
    {
	"staff", "bu`lawa",
	{ "poziom", "maks. `ladunki", "`ladunki", "czar", "", "" },
	{ 0, 0, 0, VAL_SPELL, 0, 0 }
    },
    {
	"weapon", "bro`n",	/* 5 */
	{ "", "min. obra`zenia", "maks. obra`zenia", "typ obra`ze`n", "", "materia`l" },
	{ 0, 0, 0, 0, 0, VAL_MATERIAL }
    },
    {
	"", "",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"", "",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"treasure", "skarb",
	{ "", "", "", "", "", "materia`l" },
	{ 0, 0, 0, 0, 0, VAL_MATERIAL }
    },
    {
	"armor", "zbroja",
	{ "klasa pancerza", "", "", "", "", "materia`l" },
	{ 0, 0, 0, 0, 0, VAL_MATERIAL }
    },
    {
	"potion", "mikstura",	/* 10 */
	{ "poziom", "czar 1", "czar 2", "czar 3", "gdy po`lykana", "" },
	{ 0, VAL_SPELL, VAL_SPELL, VAL_SPELL, VAL_STRING, 0 }
    },
    {
	"", "",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"furniture", "mebel",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"trash", "`smie`c",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"", "",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"container", "pojemnik",	/* 15 */
	{ "mie`sci (waga)", "flagi", "vnum klucza", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"note", "`swistek",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"drink", "pojemnik na p`lyn",
	{ "maks. pojemno`s`c", "ilo`s`c", "nap`oj", "trucizna", "jak znika", "" },
	{ 0, 0, VAL_LIQUID, 0, VAL_STRING, 0 }
    },
    {
	"key", "klucz",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"food", "jedzenie",
	{ "najedzenie", "", "", "trucizna", "kiedy zjadane", "" },
	{ 0, 0, 0, 0, VAL_STRING, 0 }
    },
    {
	"money", "pieni`adze",	/* 20 */
	{ "ilo`s`c", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"", "",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"boat", "`l`od`x",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"npc corpse", "cia`lo moba",
	{ "balsamowane", "flagi", "vnum moba", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"pc corpse", "cia`lo gracza",
	{ "", "flagi", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"fountain", "fontanna",	/* 25 */
	{ "", "", "", "", "trucizna", "nap`oj" },
	{ 0, 0, 0, 0, 0, VAL_LIQUID }
    },
    {
	"pill", "magiczne jedzenie",
	{ "poziom", "czar 1", "czar 2", "czar 3", "gdy po`lykane", "" },
	{ 0, VAL_SPELL, VAL_SPELL, VAL_SPELL, VAL_STRING, 0 }
    },
    {
	"", "",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"", "",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"", "",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"portal", "portal",	/* 30 */
	{ "poziom", "vnum celu", "chaos", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"spellbook", "ksi`ega",
	{ "czar", "nauka", "poziom", "", "", "" },
	{ VAL_SPELL, 0, 0, 0, 0, 0 }
    },
    {
	"beacon", "drogowskaz",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"shovel", "`lopata",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"device", "urz`adzenie",
	{ "", "", "", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    },
    {
	"slot machine", "jednor`eki bandyta", /* 35 */
	{ "z`loto", "okienka", "typ gry", "", "", "" },
	{ 0, 0, 0, 0, 0, 0 }
    }
};



/*
 * Liquid properties.
 * Used in world.obj.
 */

const	struct	liq_type	liq_table_pl	[ LIQ_MAX ]	=
{
    /* 0: */
    { "woda",			"wod`e",		"wod`a",
	"czystym",		{  0, 0, 10 }	},
    { "piwo",			"piwo",			"piwem",
	"jasnobr`azowym",	{  3, 2,  5 }	},
    { "czerwone wino",		"czerwone wino",	"czerwonym winem",
	"czerwonym",		{  5, 2,  5 }	},
    { "piwo j`eczmienne",	"piwo j`eczmienne",	"piwem j`eczmiennym",
	"br`azowym",		{  3, 2,  5 }	},
    { "ciemne piwo j`eczmienne","ciemne piwo j`eczmienne","ciemnym piwem j`eczmiennym",
	"ciemnym",		{  4, 2,  5 }	},
    /* 5: */
    { "whisky",			"whisky",		"whisky",
	"z`lotym",		{  6, 1,  4 }	},
    { "oran`zada",		"oran`zad`e",		"oran`zad`a",
	"r`o`zowym",		{  0, 1,  8 }	},
    { "bimber",			"bimber",		"bimbrem",
	"bulgocz`acym",		{ 10, 0,  0 }	},
    { "czysta",			"czyst`a",		"czyst`a",
	"przezroczystym",	{  5, 3,  3 }	},
    { "szlam",			"szlam",		"szlamem",
	"zielonym",		{  0, 4, -8 }	},
    /* 10: */
    { "mleko",			"mleko",		"mlekiem",
	"bia`lym",		{  0, 3,  6 }	},
    { "herbata",		"herbat`e",		"herbat`a",
	"`z`o`ltobr`azowym",	{  0, 1,  6 }	},
    { "kawa",			"kaw`e",		"kaw`a",
	"czarnym",		{  0, 1,  6 }	},
    { "krew",			"krew",			"krwi`a",
	"czerwonym",		{  0, 2, -1 }	},
    { "s`lona woda",		"s`lon`a wod`e",	"s`lon`a wod`a",
	"czystym",		{  0, 1, -2 }	},
    /* 15: */
    { "cola",			"col`e",		"col`a",
	"wi`sniowoczerwonym",	{  0, 2,  5 }	},
    { "lemoniada",		"lemoniad`e",		"lemoniad`a",
	"`z`o`ltym",		{  0, 1, 8 }	},
    { "kwas chlebowy",		"kwas chlebowy",	"kwasem chlebowym",
	"br`azowym",		{  0, 2, 7 }	},
    { "bia`le wino",		"bia`le wino",		"bia`lym winem",
	"lekko `z`o`ltym",	{  5, 2, 5 }	},
    { "wino Darwiniona",	"wino Darwiniona",	"winem Darwiniona",
	"czerwonym",		{  5, 2, 5 }	},
    /* 20: */
    { "wywar z zi`o`l",		"wywar z zi`o`l",	"wywarem z zi`o`l",
	"zielonkawobr`azowym",	{  0, 1, 7 }	},
    { "bawarka",		"bawark`e",		"bawark`a",
	"m`etnym br`azowym",	{  0, 3, 5 }	},
    { "mazagran",		"mazagran",		"mazagranem",
	"ciemnym",		{  2, 1, 7 }	},
    { "brandy",			"brandy",		"brandy",
	"ciemnoczerwonym",	{  5, 1, 5 }	},
    { "ja`lowc`owka",		"ja`lowc`owk`e",	"ja`lowc`owk`a",
	"przezroczystym",	{  6, 1, 5 }	},
    /* 25: */
    { "koniak",			"koniak",		"koniakiem",
	"`z`o`ltawym",		{  5, 1, 6 }	},
    { "mleko kokosowe",		"mleko kokosowe",	"mlekiem kokosowym",
	"bia`lym",		{  0, 3, 7 }	},
    { "koktajl \"P`lon`acy go`l`ab\"","koktajl \"P`lon`acy go`l`ab\"","koktajlem \"P`lon`acy go`l`ab\"",
	"bulgocz`acym",		{  6, 2, 6 }	},
    { "koktajl \"`Zar s`lo`nca\"","koktajl \"`Zar s`lo`nca\"","koktajlem \"`Zar s`lo`nca\"",
	"jasnym",		{  6, 2, 6 }	},
    { "koktajl \"Krwawa g`lowa\"","koktajl \"Krwawa g`lowa\"","koktajlem \"Krwawa g`lowa\"",
	"krwistoczerwonym",	{  6, 2, 6 }	},
    /* 30: */
    { "sok jab`lkowy",		"sok jab`lkowy",	"sokiem jab`lkowym",
	"br`azowawym",		{  0, 3, 8 }	},
    { "sok pomara`nczowy",	"sok pomara`nczowy",	"sokiem pomara`nczowym",
	"pomara`nczowym",	{  0, 3, 8 }	},
    { "sok wieloowocowy",	"sok wieloowocowy",	"sokiem wieloowocowym",
	"r`o`zowym",		{  0, 3, 8 }	},
    { "sok warzywny",		"sok warzywny",		"sokiem warzywnym",
	"zielonkawym",		{  0, 5, 3 }	},
    { "sok marchwiowy",		"sok marchwiowy",	"sokiem marchwiowym",
	"pomara`nczowym",	{  0, 4, 7 }	},
    /* 35: */
    { "sok porzeczkowy",	"sok porzeczkowy",	"sokiem porzeczkowym",
	"fioletowym",		{  0, 4, 7 }	},
    { "kompot truskawkowy",	"kompot truskawkowy",	"kompotem truskawkowym",
	"r`o`zowym",		{  0, 2, 8 }	},
    { "kompot wi`sniowy",	"kompot wi`sniowy",	"kompotem wi`sniowym",
	"czerwonym",		{  0, 2, 8 }	},
    { "kompot `sliwkowy",	"kompot `sliwkowy",	"kompotem `sliwkowym",
	"rubinowym",		{  0, 2, 8 }	},
    { "kompot jab`lkowy",	"kompot jab`lkowy",	"kompotem jab`lkowym",
	"`z`o`ltawym",		{  0, 2, 8 }	},
    /* 40: */
    { "kompot gruszkowy",	"kompot gruszkowy",	"kompotem gruszkowym",
	"`z`o`ltawym",		{  0, 2, 8 }	},
    { "ma`slanka",		"ma`slank`e",		"ma`slank`a",
	"m`etnym",		{  0, 2, 7 }	},
    { "arak",			"arak",			"arakiem",
	"br`azowym",		{  6, 1, 6 }	},
    { "maliniak",		"maliniak",		"maliniakiem",
	"ciemnor`o`zowym",	{  6, 2, 6 }	},
    { "wi`sni`owka",		"wi`sni`owk`e",		"wi`sni`owk`a",
	"bordowym",		{  6, 2, 6 }	},
    /* 45: */
    { "`sliwowica",		"`sliwowic`e",		"`sliwowic`a",
	"s`lomkowym",		{  6, 2, 6 }	},
    { "mi`od",			"mi`od",		"miodem",
	"bursztynowym",		{  6, 2, 6 }	},
    { "poncz",			"poncz",		"ponczem",
	"r`o`zowym",		{  4, 1, 7 }	},
    { "`z`etyca",		"`z`etyc`e",		"`z`etyc`a",
	"m`etnym",		{  0, 2, 8 }	},
    { "porter",			"portera",		"porterem",
	"ciemnym",		{  4, 2, 5 }	}
};


const	char	*typy_podloza	[ ]    =
{
    "pomieszczenie",    "miasto",    "pole",    "las",
    "wzg`orza",    "g`ory",    "p`lytka_woda",    "g`l`eboka_woda",
    "pod_wod`a",    "w_powietrzu",    "pustynia",    "maks"
};


/*
 * To bylo const, ale teraz klasy czytaja sie z dysku i nie moze juz byc stala
 * tablica. Lepszym rozwiazaniem byloby zrobienie zmiennej oddzielnie, ale:
 * 1. to duzo pracy,
 * 2. poziomy i tak musza byc zmienne,
 * 3. dopoki nikt nie napisze czegos nie tak, wszystko bedzie dzialac poprawnie.
 * Te klamerki kiedys byly pelne liczb, ale dla kilku klas nie warto.
 * Teraz gcc sam wypelnia to zerami.
 */
/* const */	struct	skill_type	skill_table	[ MAX_SKILL ]	=
{
/*
    {
	"nazwa umiej`etno`sci",	"skr`ot",
	"english version",	{ 0 },
	"requires",		{ 0 },
	spell_fun,		TARGET,			POSITION,
	&gsn,			(min_mana),	(wait_state),
	(cw),	(ucz),	(rodz),	"komunikat walki",
	"komunikat spadni`ecia","komunikat spadni`ecia dla pomieszczenia",
	wearof_fun,		(flagi)
    },
*/

    {
	"zarezerwowane",	"rezerwa",
	"reserved",		{ 0 },
	NULL,                   { 0 },
	NULL,			TAR_IGNORE,		POS_STANDING,
	NULL,			0,	 0,
	0,	99,	0,	"",
	"",			"",
	NULL,			SKILL_NORMAL
    },

    {
	"wybuch kwasu",		"kwas",
	"acid blast",		{ 0 },
	NULL,                   { 0 },
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	0,	40,	1,	"wybuch kwasu",
	"!Acid Blast!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"b`logos`lawie`nstwo",	"",
	"bless",		{ 0 },
	NULL,                   { 0 },
	spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_bless,		5,	8,
	0,	40,	0,	"",
	"Bogowie przestaj`a ci`e b`logos`lawi`c.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"o`slepienie",		"",
	"blindness",		{ 0 },
	NULL,                   { 0 },
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		5,	12,
	0,	30,	0,	"",
	"",			"",
	wear_off_blindness,	SKILL_NORMAL
    },

    {
	"oddychanie pod wod`a",	"",
	"breathe water",	{ 0 },
	NULL,                   { 0 },
	spell_breathe_water,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_breathe_water,	 5,	8,
	0,	30,	3,	"bol`ace p`luca",
	"Nie mo`zesz ju`z d`lu`zej oddycha`c pod wod`a.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"p`lon`ace d`lonie",	"",
	"burning hands",	{ 0 },
	NULL,                   { 0 },
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_burning_hands,	15,	12,
	0,	40,	3,	"p`lon`ace d`lonie",
	"!Burning Hands!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wezwanie b`lyskawicy",	"",
	"call lightning",	{ 0 },
	NULL,                   { 0 },
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	0,	40,	2,	"b`lyskawica",
	"!Call Lightning!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"mocne uszkodzenie",	"",
	"cause critical",	{ 0 },
	NULL,                   { 0 },
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	0,	40,	1,	"wp`lyw",
	"!Cause Critical!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"lekkie uszkodzenie",	"",
	"cause light",		{ 0 },
	NULL,                   { 0 },
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_cause_light,	15,	12,
	0,	40,	1,	"wp`lyw",
	"!Cause Light!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"powa`zne uszkodzenie",	"",
	"cause serious",	{ 0 },
	NULL,                   { 0 },
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			17,	12,
	0,	40,	1,	"wp`lyw",
	"!Cause Serious!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wyb`or celu",		"",
	"choose target",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_change,		 0,	12,
	0,	30,	0,	"",
	"!Change!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"zmiana p`lci",		"",
	"change sex",		{ 0 },
	NULL,                   { 0 },
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	0,
	0,	40,	0,	"",
	"Masz znowu swoje cia`lo.",	"$n powraca do swojego cia`la.",
	NULL,			SKILL_NPC
    },

    {
	"oczarowanie",		"oswojenie",
	"charm person",		{ 0 },
	NULL,                   { 0 },
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	25,	8,
	0,	30,	0,	"",
	"Czujesz, `ze masz ju`z siebie pod kontrol`a.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"mro`xny dotyk",	"",
	"chill touch",		{ 0 },
	NULL,                   { 0 },
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	35,	1,	"mro`xny dotyk",
	"Czujesz, `ze jest ci du`zo cieplej.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"kolorowe p`lomienie",	"",
	"colour spray",		{ 0 },
	NULL,                   { 0 },
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	40,	3,	"kolorowe p`lomienie",
	"!Colour Spray!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"strefa ciszy",		"cisza",
	"cone of silence",	{ 0 },
	NULL,                   { 0 },
	spell_cone_of_silence,	TAR_IGNORE,		POS_FIGHTING,
	&gsn_cone_of_silence,	35,	16,
	0,	30,	0,	"",
	"D`xwi`ek znowu rozbrzmiewa w tym pomieszczeniu.", "",
	NULL,			SKILL_NORMAL
    },

    {
	"stworzenie `swiat`la",	"`swiat`lo",
	"continual light",	{ 0 },
	NULL,                   { 0 },
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			7,	4,
	0,	30,	0,	"",
	"!Continual Light!",	"",
	NULL,			SKILL_KUPOWALNE
    },

    {
	"zmiana pogody",	"pogoda",
	"control weather",	{ 0 },
	NULL,                   { 0 },
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			25,	4,
	0,	30,	0,	"",
	"!Control Weather!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"tworzenie jedzenia",	"jedzenie",
	"create food",		{ 0 },
	NULL,                   { 0 },
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	4,
	0,	30,	0,	"",
	"!Create Food!",	"",
	NULL,			SKILL_KUPOWALNE | SKILL_NO_COST
    },

    {
	"tworzenie `xr`od`la",	"`xr`od`lo",
	"create spring",	{ 0 },
	NULL,                   { 0 },
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	4,
	0,	30,	0,	"",
	"!Create Spring!",	"",
	NULL,			SKILL_KUPOWALNE
    },

    {
	"tworzenie wody",	"woda",
	"create water",		{ 0 },
	NULL,                   { 0 },
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	4,
	0,	20,	0,	"",
	"!Create Water!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"leczenie `slepoty",	"",
	"cure blindness",	{ 0 },
	NULL,                   { 0 },
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			5,	8,
	0,	30,	0,	"",
	"!Cure Blindness!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"porz`adne leczenie",	"",
	"cure critical",	{ 0 },
	NULL,                   { 0 },
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	8,
	0,	35,	0,	"",
	"!Cure Critical!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"lekkie leczenie",	"",
	"cure light",		{ 0 },
	NULL,                   { 0 },
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_cure_light,	10,	8,
	0,	30,	0,	"",
	"!Cure Light!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"leczenie trucizny",	"",
	"cure poison",		{ 0 },
	NULL,                   { 0 },
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			5,	8,
	0,	20,	0,	"",
	"!Cure Poison!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"`srednie leczenie",	"",
	"cure serious",		{ 0 },
	NULL,                   { 0 },
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	8,
	0,	30,	0,	"",
	"!Cure Serious!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"kl`atwa",		"",
	"curse",		{ 0 },
	NULL,                   { 0 },
	spell_curse,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_curse,		20,	12,
	0,	30,	2,	"kl`atwa",
	"Przekle`nstwo spada.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"zniszcz przekl`ete",	"",
	"destroy cursed",	{ 0 },
	NULL,                   { 0 },
	spell_destroy_cursed,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	8,
	0,	30,	0,	"",
	"!destroy cursed!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wykrycie z`la",	"",
	"detect evil",		{ 0 },
	NULL,                   { 0 },
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	4,
	0,	30,	0,	"",
	"",			"",
	wear_off_detect_evil,	SKILL_NORMAL
    },

    {
	"wykrycie ukrytych",	"",
	"detect hidden",	{ 0 },
	NULL,                   { 0 },
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	4,
	0,	30,	0,	"",
	"",			"",
	wear_off_detect_hidden,	SKILL_NORMAL
    },

    {
	"wykrycie niewidki",	"",
	"detect invis",		{ 0 },
	NULL,                   { 0 },
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	4,
	0,	30,	0,	"",
	"",			"",
	wear_off_detect_invis,	SKILL_NORMAL
    },

    {
	"wykrycie magii",	"",
	"detect magic",		{ 0 },
	NULL,                   { 0 },
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	4,
	0,	30,	0,	"",
	"Przestajesz wykrywa`c magi`e.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wykrycie trucizny",	"",
	"detect poison",	{ 0 },
	NULL,                   { 0 },
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	4,
	0,	30,	0,	"",
	"!Detect Poison!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"odp`edzenie z`la",	"",
	"dispel evil",		{ 0 },
	NULL,                   { 0 },
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	35,	1,	"`swi`ety ogie`n",
	"!Dispel Evil!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"rozproszenie magii",	"",
	"dispel magic",		{ 0 },
	NULL,                   { 0 },
	spell_dispel_magic,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			50,	12,
	0,	35,	0,	"",
	"!Dispel Magic!",	"",
	NULL,			SKILL_KUPOWALNE | SKILL_NO_COST
    },

    {
	"sen",			"",
	"dream",		{ 0 },
	NULL,                   { 0 },
	spell_dream,		TAR_IGNORE,		POS_STANDING,
	NULL,			5,	8,
	0,	30,	0,	"",
	"!Dream!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"trz`esienie ziemi",	"wstrz`as",
	"earthquake",		{ 0 },
	NULL,                   { 0 },
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	0,	40,	0,	"trz`esienie ziemi",
	"!Earthquake!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"zaczarowanie broni",	"",
	"enchant weapon",	{ 0 },
	NULL,                   { 0 },
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			100,	20,
	0,	30,	0,	"",
	"!Enchant Weapon!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wysysanie energii",	"",
	"energy drain",		{ 0 },
	NULL,                   { 0 },
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	0,	40,	0,	"ssanie energii",
	"!Energy Drain!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"egzorcyzm",		"",
	"exorcise",		{ 0 },
	NULL,                   { 0 },
	spell_exorcise,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			35,	12,
	0,	30,	0,	"",
	"!Exorcise!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"pogorszenie ochrony",	"",
	"faerie fire",		{ 0 },
	NULL,                   { 0 },
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			5,	12,
	0,	30,	0,	"pogorszenie ochrony",
	"R`o`zowa otoczka spada.",	"R`o`zowa otoczka spada z $1.",
	NULL,			SKILL_NORMAL
    },

    {
	"demaskowanie",		"",
	"faerie fog",		{ 0 },
	NULL,                   { 0 },
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			12,	8,
	0,	30,	0,	"demaskowanie",
	"!Faerie Fog!",		"",
	NULL,			SKILL_KUPOWALNE
    },

    {
	"palec `smierci",	"",
	"finger of death",	{ 0 },
	NULL,                   { 0 },
	spell_finger_of_death,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	40,	1,	"palec `smierci",
	"!Finger of death!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"kula ognia",		"ogie`n",
	"fireball",		{ 0 },
	NULL,                   { 0 },
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	40,	2,	"kula ognia",
	"!Fireball!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"uderzenie p`lomieni",	"",
	"flamestrike",		{ 0 },
	NULL,                   { 0 },
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_flamestrike,	20,	12,
	0,	40,	0,	"uderzenie p`lomieni",
	"!Flamestrike!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"p`lon`aca tarcza",	"",
	"flaming shield",	{ 0 },
	NULL,                   { 0 },
	spell_flaming,		TAR_CHAR_SELF,		POS_STANDING,
	NULL,			100,	24,
	0,	30,	2,	"p`lon`aca tarcza",
	"P`lon`aca tarcza ga`snie.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"latanie",		"",
	"fly",			{ 0 },
	NULL,                   { 0 },
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			10,	8,
	0,	30,	0,	"",
	"Tracisz zdolno`s`c magicznego latania.",	"$n traci zdolno`s`c magicznego latania.",
	NULL,			SKILL_PSI_SELF
    },

    {
	"sprowadzenie wampira",		"wampir",
	"gate",			{ 0 },
	NULL,                   { 0 },
	spell_gate,		TAR_IGNORE,	POS_FIGHTING,
	NULL,			50,	12,
	0,	30,	0,	"",
	"!Gate!",		"",
	NULL,			SKILL_NPC
    },

    {
	"si`la olbrzyma",	"",
	"giant strength",	{ 0 },
	NULL,                   { 0 },
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	8,
	0,	30,	0,	"",
	"Czujesz si`e s`labiej.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"krzywda",		"",
	"harm",			{ 0 },
	NULL,                   { 0 },
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	0,	40,	2,	"wewn`etrzna moc",
	"!Harm!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"uzdrowienie",		"",
	"heal",			{ 0 },
	NULL,                   { 0 },
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	0,	40,	0,	"",
	"!Heal!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"polowanie",		"",
	"hunt",			{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_hunt,		0,	6,
	0,	30,	0,	"",
	"!Hunt!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"identyfikacja",	"",
	"identify",		{ 0 },
	NULL,                   { 0 },
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			12,	12,
	0,	30,	0,	"",
	"!Identify!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"widzenie w mroku",	"",
	"infravision",		{ 0 },
	NULL,                   { 0 },
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	8,
	0,	30,	0,	"",
	"",			"",
	wear_off_infravision,	SKILL_NORMAL
    },

    {
	"niewidzialno`s`c",	"",
	"invis",		{ 0 },
	NULL,                   { 0 },
	spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invis,		5,	8,
	0,	30,	0,	"",
	"Przestajesz by`c niewidzialn$y.",	"$n staje si`e widzialn$y.",
	NULL,			SKILL_PSI_SELF
    },

    {
	"odkrycie moralno`sci",	"",
	"know alignment",	{ 0 },
	NULL,                   { 0 },
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			9,	8,
	0,	30,	0,	"",
	"!Know Alignment!",	"",
	NULL,			SKILL_DEF_VICT
    },

    {
	"p`lomienne oczy",	"oczy",
	"laser eyes",		{ 0 },
	NULL,                   { 0 },
	spell_laser_eyes,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	13,
	0,	40,	3,	"p`lomienne oczy",
	"!Laser Eyes!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"uderzenie pioruna",	"piorun",
	"lightning bolt",	{ 0 },
	NULL,                   { 0 },
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	40,	2,	"b`lyskawica",
	"!Lightning Bolt!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"lokalizowanie",	"",
	"locate object",	{ 0 },
	NULL,                   { 0 },
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	12,
	0,	30,	0,	"",
	"!Locate Object!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"magiczny pocisk",	"",
	"magic missile",	{ 0 },
	NULL,                   { 0 },
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	30,	1,	"magiczny pocisk",
	"!Magic Missile!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"magiczna os`lona",	"oslona",
	"armor",		{ 0 },
	NULL,                   { 0 },
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_armor,		5,	8,
	0,	40,	0,	"",
	"Nie czujesz si`e ju`z chronion$y.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"podarunek many",	"mana",
	"mana gift",		{ 0 },
	NULL,                   { 0 },
	spell_mana_gift,    TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
	&gsn_mana_gift,		50,	8,
	0,	40,	0,	"",
	"!Mana Gift!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"grupowe uzdrowienie",	"",
	"mass heal",		{ 0 },
	NULL,                   { 0 },
	spell_mass_heal,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			50,	24,
	0,	40,	0,	"",
	"!Mass Heal!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"grupowe od`swie`zenie","",
	"mass refresh",		{ 0 },
	NULL,                   { 0 },
	spell_mass_refresh,	TAR_IGNORE,		POS_STANDING,
	NULL,			25,	12,
	0,	30,	0,	"",
	"!Mass Refresh!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"grupowa niewidka",	"",
	"mass invis",		{ 0 },
	NULL,                   { 0 },
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	20,	12,
	0,	30,	0,	"",
	"Przestajesz by`c niewidzialn$y.",	"$n staje si`e widzialn$y.",
	NULL,			SKILL_NORMAL
    },

    {
	"grupowa ochrona",	"",
	"mass protection",	{ 0 },
	NULL,                   { 0 },
	spell_mass_protection,	TAR_IGNORE,		POS_STANDING,
	NULL,			25,	12,
	0,	30,	0,	"",
	"Przestajesz odczuwa`c magiczn`a ochron`e.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"grupowa tarcza",	"",
	"mass shield",		{ 0 },
	NULL,                   { 0 },
	spell_mass_shield,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	12,
	0,	30,	0,	"",
	"Spada z ciebie magiczna tarcza.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"strza`la ksi`e`zyca",	"ksi`e`zyc",
	"moon arrow",		{ 0 },
	NULL,                   { 0 },
	spell_moon_arrow,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	15,
	0,	50,	2,	"ksi`e`zycowa strza`la",
	"!Moon Arrow!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"wyciszenie",		"",
	"mute",			{ 0 },
	NULL,                   { 0 },
	spell_mute,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_mute,		20,	12,
	0,	30,	0,	"",
	"",			"",
	wear_off_mute,		SKILL_NORMAL
    },

    {
	"przenikanie",		"",
	"pass door",		{ 0 },
	NULL,                   { 0 },
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			20,	8,
	0,	30,	0,	"",
	"Tracisz elastyczno`s`c.",	"$n traci elastyczno`s`c.",
	NULL,			SKILL_NORMAL
    },

    {
	"wybuch plazmowy",	"",
	"plasma blast",		{ 0 },
	NULL,                   { 0 },
	spell_plasma_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	13,
	0,	50,	1,	"wybuch plazmowy",
	"!Plasma Blast!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"otrucie",		"",
	"poison",		{ 0 },
	NULL,                   { 0 },
	spell_poison,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_poison,		10,	12,
	0,	30,	2,	"kipi`aca krew",
	"Czujesz si`e zdrowiej.",	"$n zdaje si`e dochodzi`c do siebie.",
	NULL,			SKILL_NO_DISPEL
    },

    {
	"przemiana",		"",
	"polymorph other",	{ 0 },
	NULL,                   { 0 },
	spell_polymorph_other,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_polymorph_other,	20,	6,
	0,	30,	0,	"",
	"Masz znowu swoje cia`lo.",	"$n powraca do swojego pierwotnego kszta`ltu.",
	NULL,			SKILL_NO_DISPEL | SKILL_FRIENDLY_DISPEL | SKILL_NPC
    },

    {
	"portal",		"",
	"portal",		{ 0 },
	NULL,                   { 0 },
	spell_portal,		TAR_IGNORE,		POS_STANDING,
	NULL,			50,	8,
	0,	50,	0,	"",
	"!Portal!",		"",
	NULL,			SKILL_KUPOWALNE
    },

    {
	"ochrona",		"",
	"protection",		{ 0 },
	NULL,                   { 0 },
	spell_protection,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	8,
	0,	30,	0,	"",
	"Czujesz si`e mniej chronion$y.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"promie`n prawdy",	"prawda",
	"ray of truth",		{ 0 },
	NULL,                   { 0 },
	spell_ray_of_truth,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	13,
	0,	40,	1,	"promie`n prawdy",
	"!Ray Of Truth!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"na`ladowanie",		"",
	"recharge item",	{ 0 },
	NULL,                   { 0 },
	spell_recharge_item,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			25,	8,
	0,	30,	2,	"pomy`lka",
	"!Recharge Item!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"od`swie`zenie",	"",
	"refresh",		{ 0 },
	NULL,                   { 0 },
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_refresh,		12,	8,
	0,	30,	0,	"od`swie`zenie",
	"!Refresh!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"zdj`ecie moralno`sci",	"",
	"remove alignment",	{ 0 },
	NULL,                   { 0 },
	spell_remove_alignment, TAR_OBJ_INV,		POS_STANDING,
	NULL,			10,	8,
	0,	30,	0,	"",
	"!Remove Alignment!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"zdj`ecie kl`atwy",	"",
	"remove curse",		{ 0 },
	NULL,                   { 0 },
	spell_remove_curse,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			5,	8,
	0,	30,	0,	"",
	"!Remove Curse!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"zdj`ecie niewidki",	"",
	"remove invis",		{ 0 },
	NULL,                   { 0 },
	spell_remove_invis,	TAR_IGNORE,	POS_STANDING,
	NULL,			15,	8,
	0,	30,	0,	"",
	"!Remove Invis!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"zdj`ecie ciszy",	"",
	"remove silence",	{ 0 },
	NULL,                   { 0 },
	spell_remove_silence,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	8,
	0,	30,	0,	"",
	"!Remove Silence!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"u`swi`ecenie",		"",
	"sanctuary",		{ 0 },
	NULL,                   { 0 },
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			35,	12,
	0,	40,	0,	"",
	"Bia`la aura opada z ciebie.",	"Bia`la aura opada z $1.",
	NULL,			SKILL_NORMAL
    },

    {
	"oparzenie",		"",
	"scald",		{ 0 },
	NULL,                   { 0 },
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	0,	40,	0,	"oparzenie",
	"!Scald!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"magiczna tarcza",	"tarcza",
	"shield",		{ 0 },
	NULL,                   { 0 },
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_shield,		12,	8,
	0,	30,	0,	"",
	"Magiczna tarcza opada.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wstrz`asaj`acy u`scisk",	"",
	"shocking grasp",	{ 0 },
	NULL,                   { 0 },
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	30,	1,	"wstrz`asaj`acy u`scisk",
	"!Shocking Grasp!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"u`spienie",		"",
	"sleep",		{ 0 },
	NULL,                   { 0 },
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		15,	12,
	0,	30,	0,	"",
	"Czujesz si`e mniej `spi`ac$y.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"kamienna sk`ora",	"",
	"stone skin",		{ 0 },
	NULL,                   { 0 },
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			12,	8,
	0,	30,	0,	"",
	"Twoja sk`ora staje si`e znowu mi`ekka.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"og`luszenie",		"",
	"stun",			{ 0 },
	NULL,                   { 0 },
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	40,	0,	"og`luszenie",
	"!Stun!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"przywo`lanie",		"",
	"summon",		{ 0 },
	NULL,                   { 0 },
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			50,	8,
	0,	30,	0,	"",
	"!Summon!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"promie`n s`lo`nca",	"s`lo`nce",
	"sunbeam",		{ 0 },
	NULL,                   { 0 },
	spell_sunbeam,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	15,
	0,	50,	1,	"s`loneczny promie`n",
	"!Stun!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"teleportacja",		"",
	"teleport",		{ 0 },
	NULL,                   { 0 },
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	&gsn_teleport,		35,	8,
	0,	30,	0,	"",
	"!Teleport!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"grupowa teleportacja",	"",
	"teleport group",	{ 0 },
	NULL,                   { 0 },
	spell_teleport_group,	TAR_IGNORE,		POS_STANDING,
	NULL,			80,	12,
	0,	30,	0,	"",
	"!Teleport Group!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"odp`edzenie `smierci",	"",
	"turn undead",		{ 0 },
	NULL,                   { 0 },
	spell_turn_undead,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_turn_undead,	10,	12,
	0,	30,	3,	"`swi`ete s`lowa",
	"!Turn undead!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"brzuchom`owstwo",	"",
	"ventriloquate",	{ 0 },
	NULL,                   { 0 },
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	8,
	0,	30,	0,	"",
	"!Ventriloquate!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"os`labienie",		"",
	"weaken",		{ 0 },
	NULL,                   { 0 },
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	0,	30,	1,	"os`labiaj`acy czar",
	"Odzyska`l$o`s swoj`a si`l`e.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"powr`ot",		"",
	"word of recall",	{ 0 },
	NULL,                   { 0 },
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,			5,	4,
	0,	30,	0,	"",
	"!Word of Recall!",	"",
	NULL,			SKILL_NORMAL
    },

/*
 * Fighter and thief skills, as well as magic item skills.
 */
    {
	"sztyletowanie",	"",
	"backstab",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_backstab,		 0,	24,
	0,	40,	1,	"podst`epny cios w plecy",
	"!Backstab!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"wywa`zanie drzwi",	"",
	"bash door",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_bash,		 0,	12,
	0,	30,	1,	"pot`e`zny wyjazd z bara",
	"!Bash Door!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"sza`l bitewny",	"",
	"berserk",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_berserk,		 0,	12,
	0,	30,	0,	"",
	"Sza`l bojowy przemija.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"okr`a`zenie",		"",
	"circle",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_circle,		 0,	24,
	0,	30,	1,	"podst`epny atak",
	"!Circle!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"rozbrojenie",		"",
	"disarm",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_disarm,		 0,	18,
	0,	35,	0,	"",
	"!Disarm!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"uniki",		"",
	"dodge",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dodge,		 0,	 0,
	0,	80,	0,	"",
	"!Dodge!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"dwie bronie",		"",
	"dual",			{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dual,		 0,	 0,
	0,	40,	0,	"",
	"!Dual!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"wi`eksze obra`zenia",	"",
	"enhanced damage",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_enhanced_damage,	0,	 0,
	0,	80,	0,	"",
	"!Enhanced Damage!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"szybsze zdrowienie",	"",
	"fast healing",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_DEAD,
	&gsn_fast_healing,	 0,	 0,
	0,	10000,	0,	"",
	"!Fast Healing!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"czwarty atak",		"",
	"fourth attack",	{ 0 },
	"trzeci atak",		{ 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_fourth_attack,	  0,	  0,
	0,	50,	0,	"",
	"!Fourth Attack!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"krycie si`e",		"",
	"hide",			{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		 0,	8,
	0,	30,	0,	"",
	"!Hide!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"kopanie",		"",
	"kick",			{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_kick,		 0,	8,
	0,	60,	1,	"mocny kop",
	"!Kick!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"sokoli wzrok",		"",
	"long scan",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_DEAD,
	&gsn_longscan,           0,     0,
	0,	50,	0,	"",
	"!Longscan!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"medytacja",		"",
	"meditation",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_DEAD,
	&gsn_meditation,        0,      0,
	0,	12000,	0,	"",
	"!Meditation!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"parowanie",		"",
	"parry",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_parry,		 0,	 0,
	0,	80,	0,	"",
	"!Parry!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"blokowanie tarcz`a",	"",
	"blokowanie tarcz`a",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_blokowanie_tarcza,	 0,	 0,
	0,	80,	0,	"",
	"!Blokowanie tarcz`a!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wgl`ad w inwentarz",	"",
	"peek",			{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		 0,	 0,
	0,	30,	0,	"",
	"!Peek!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"`lamanie zamk`ow",	"",
	"pick lock",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		 0,	12,
	0,	35,	0,	"",
	"!Pick!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"trucie broni",		"",
	"poison weapon",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_OBJ_INV,		POS_STANDING,
	&gsn_poison_weapon,	 0,	12,
	0,	30,	2,	"zatruta mieszanka",
	"!Poison Weapon!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"ratowanie",		"",
	"rescue",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_rescue,		 0,	12,
	0,	40,	0,	"",
	"!Rescue!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"czytanie pergamin`ow",	"",
	"scrolls",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_scrolls,		 0,	0,
	0,	20,	1,	"p`lon`acy pergamin",
	"!Scrolls!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"drugi atak",		"",
	"second attack",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_second_attack,	 0,	 0,
	0,	80,	0,	"",
	"!Second Attack!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"pochwycenie",		"",
	"snare",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_snare,		 0,	12,
	0,	30,	0,	"",
	"Uwalniasz si`e z side`l.",	"$n uwalnia si`e z side`l.",
	NULL,			SKILL_NORMAL
    },

    {
	"skradanie si`e",	"",
	"sneak",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		 0,	8,
	0,	30,	0,	"",
	"Przestajesz si`e skrada`c.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"u`zywanie bu`law",	"",
	"staves",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_staves,		 0,	0,
	0,	20,	2,	"popsuta bu`lawa",
	"!Staves!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"okradanie",		"",
	"steal",		{ 0 },
	"wgl`ad w inwentarz",	{ 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		 0,	12,
	0,	30,	0,	"",
	"!Steal!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"trzeci atak",		"",
	"third attack",		{ 0 },
	"drugi atak",		{ 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_third_attack,	 0,	 0,
	0,	50,	0,	"",
	"!Third Attack!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"uwolnienie",		"",
	"untangle",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_untangle,		 0,	18,
	0,	30,	0,	"",
	"!Untangle!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"u`zywanie r`o`zd`zek",	"",
	"wands",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_wands,		 0,	0,
	0,	20,	2,	"eksploduj`aca r`o`zd`zka",
	"!Wands!",		"",
	NULL,			SKILL_NORMAL
    },

    /*
     * Psionicist spell/skill definitions
     * Psi class conceived, created, and coded by Thelonius (EnvyMud)
     */

    {
	"adrenalina",		"",
	"adrenaline control",	{ 0 },
	NULL,                   { 0 },
	spell_adrenaline_control,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,				6,	8,
	0,	30,	0,	"",
	"Poziom adrenaliny si`e normalizuje.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wstrz`a`snienie",	"",
	"agitation",		{ 0 },
	NULL,                   { 0 },
	spell_agitation,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			10,	12,
	0,	30,	0,	"molekularne wstrz`a`snienie",
	"!Agitation!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"widzenie po`swiaty",	"",
	"aura sight",		{ 0 },
	NULL,                   { 0 },
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			9,     8,
	0,	30,	0,	"",
	"!Aura Sight!",		"",
	NULL,			SKILL_DEF_VICT
    },

    {
	"wzbudzanie podziwu",	"podziw",
	"awe",			{ 0 },
	NULL,                   { 0 },
	spell_awe,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			35,	8,
	0,	30,	0,	"",
	"!Awe!",		"",
	NULL,			SKILL_DEF_VICT
    },

    {
	"atak powietrzny",	"",
	"ballistic attack",	{ 0 },
	NULL,                   { 0 },
	spell_ballistic_attack, TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			5,     12,
	0,	20,	1,	"atak z powietrza",
	"!Ballistic Attack!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"kontrola nad cia`lem",	"",
	"biofeedback",		{ 0 },
	NULL,                   { 0 },
	spell_sanctuary,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			75,	12,
	0,	30,	0,	"",
	"Twoja biologiczna ochrona ju`z nie dzia`la.",	"Biologiczna ochrona $1 przestaje dzia`la`c.",
	NULL,			SKILL_NORMAL
    },

    {
	"przystosowanie",	"",
	"cell adjustment",	{ 0 },
	NULL,                   { 0 },
	spell_cell_adjustment,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			8,     8,
	0,	30,	0,	"",
	"!Cell Adjustment!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"kameleon",		"",
	"chameleon power",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_chameleon,		0,     8,
	0,	20,	0,	"",
	"!Chameleon Power!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"taktyki bitewne",	"",
	"combat mind",		{ 0 },
	NULL,                   { 0 },
	spell_combat_mind,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_combat_mind,	15,	8,
	0,	30,	0,	"",
	"Tw`oj instynkt bojowy zanikn`a`l.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"zupe`lne uzdrowienie",	"",
	"complete healing",	{ 0 },
	NULL,                   { 0 },
	spell_complete_healing, TAR_CHAR_SELF,		POS_STANDING,
	NULL,			100,	18,
	0,	30,	0,	"",
	"!Complete Healing!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"kontrola nad ogniem",	"ogie`n",
	"control flames",	{ 0 },
	NULL,                   { 0 },
	spell_control_flames,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	0,	30,	1,	"j`ezyk ognia",
	"!Control Flames!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"tworzenie d`xwi`eku",	"",
	"create sound",		{ 0 },
	NULL,                   { 0 },
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,     8,
	0,	30,	0,	"",
	"!Create Sound!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"pole `smierci",	"",
	"death field",		{ 0 },
	NULL,                   { 0 },
	spell_death_field,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			200,	18,
	0,	40,	0,	"pole `smierci",
	"!Death Field!",	"",
	NULL,			SKILL_NORMAL | SKILL_NO_COST
    },

    {
	"detonacja",		"",
	"detonate",		{ 0 },
	NULL,                   { 0 },
	spell_detonate,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			30,	12,
	0,	35,	2,	"detonacja",
	"!Detonate!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"dezintegracja",	"",
	"disintegrate",		{ 0 },
	NULL,                   { 0 },
	spell_disintegrate,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			100,	18,
	0,	40,	2,	"dezintegracja",
	"!Disintegrate!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"przemieszczenie",	"",
	"displacement",		{ 0 },
	NULL,                   { 0 },
	spell_displacement,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			10,	8,
	0,	35,	0,	"",
	"Nie czujesz si`e przemieszczon$y.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"dominacja",		"zdominowanie",
	"domination",		{ 0 },
	NULL,                   { 0 },
	spell_domination,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_domination,	25,    12,
	0,	30,	0,	"",
	"Odzyskujesz kontrol`e nad w`lasnym cia`lem.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"ektoplazma",		"",
	"ectoplasmic form",	{ 0 },
	NULL,                   { 0 },
	spell_ectoplasmic_form, TAR_CHAR_SELF,		POS_STANDING,
	NULL,			20,	8,
	0,	30,	0,	"",
	"Tracisz elastyczno`s`c.",	"$n traci elastyczno`s`c.",
	NULL,			SKILL_NORMAL
    },

    {
	"z`le wspomnienia",	"wspomnienia",
	"ego whip",		{ 0 },
	NULL,                   { 0 },
	spell_ego_whip,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	0,	30,	0,	"",
	"Czujesz si`e mniej spi`et$y.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"przej`ecie energii",	"",
	"energy containment",	{ 0 },
	NULL,                   { 0 },
	spell_energy_containment,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,			10,	8,
	0,	30,	0,	"",
	"Nie pobierasz ju`z wi`ecej energii.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"ulepszanie zbroi",	"",
	"enhance armor",	{ 0 },
	NULL,                   { 0 },
	spell_enhance_armor,	TAR_OBJ_INV,	POS_STANDING,
	NULL,			100,	18,
	0,	30,	0,	"",
	"!Enhance Armor!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"powi`ekszona si`la",	"",
	"enhanced strength",	{ 0 },
	NULL,                   { 0 },
	spell_enhanced_strength,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,			20,	8,
	0,	30,	0,	"",
	"Nie czujesz si`e ju`z siln$y.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"twarda sk`ora",	"",
	"flesh armor",		{ 0 },
	NULL,                   { 0 },
	spell_flesh_armor,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			15,	8,
	0,	30,	0,	"",
	"Twoja sk`ora przestaje by`c twarda jak stal.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wyostrzenie zmys`l`ow","",
	"heighten senses",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_CHAR_SELF,		POS_STANDING,
	&gsn_heighten,		0,	8,
	0,	30,	0,	"",
	"Twe zmys`ly s`labn`a.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wewn`etrzna bariera",	"",
	"inertial barrier",	{ 0 },
	NULL,                   { 0 },
	spell_inertial_barrier, TAR_IGNORE,		POS_STANDING,
	NULL,			40,	18,
	0,	30,	0,	"",
	"Twoja wewn`etrzna bariera opada.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"bariera z`la",		"",
	"evil barrier",		{ 0 },
	NULL,                   { 0 },
	spell_bariera_zla,	TAR_IGNORE,		POS_STANDING,
	NULL,			40,	18,
	0,	30,	0,	"",
	"Opada z ciebie bariera z`la.",		"",
	NULL,			SKILL_NORMAL | SKILL_NO_COST
    },

    {
	"wywo`lanie b`olu",	"",
	"inflict pain",		{ 0 },
	NULL,                   { 0 },
	spell_inflict_pain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			10,	12,
	0,	40,	2,	"wewn`etrzna energia",
	"!Inflict Pain!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"bariera wirtualna",	"",
	"intellect fortress",	{ 0 },
	NULL,                   { 0 },
	spell_intellect_fortress,	TAR_IGNORE,	POS_STANDING,
	NULL,			25,	18,
	0,	30,	0,	"",
	"Twoja bariera wirtualna kruszy si`e i znika.",	"Bariera wirtualna $1 kruszy si`e i znika.",
	NULL,			SKILL_NORMAL
    },

    {
	"u`zyczenie zdrowia",	"",
	"lend health",		{ 0 },
	NULL,                   { 0 },
	spell_lend_health,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			10,	12,
	0,	30,	0,	"",
	"!Lend Health!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"lewitacja",		"",
	"levitation",		{ 0 },
	NULL,                   { 0 },
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			10,	8,
	0,	30,	0,	"",
	"Tracisz umiej`etno`s`c lewitacji.",	"$n traci umiej`etno`s`c lewitacji.",
	NULL,			SKILL_PSI_SELF
    },

    {
	"bariera mentalna",	"",
	"mental barrier",	{ 0 },
	NULL,                   { 0 },
	spell_mental_barrier,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			8,     8,
	0,	30,	0,	"",
	"Bariera mentalna zanika.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"atak my`slowy",	"",
	"mind thrust",		{ 0 },
	NULL,                   { 0 },
	spell_mind_thrust,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			8,     12,
	0,	30,	1,	"atak my`slowy",
	"!Mind Thrust!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"pokaz si`ly",		"",
	"project force",	{ 0 },
	NULL,                   { 0 },
	spell_project_force,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			18,	12,
	0,	40,	1,	"pokaz si`ly",
	"!Project Force!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"wybuch psioniczny",	"",
	"psionic blast",	{ 0 },
	NULL,                   { 0 },
	spell_psionic_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	12,
	0,	40,	2,	"energia psioniczna",
	"!Psionic Blast!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"mia`zd`zenie umys`lu",	"",
	"psychic crush",	{ 0 },
	NULL,                   { 0 },
	spell_psychic_crush,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	18,
	0,	40,	0,	"psychiczne zmia`zd`zenie",
	"!Psychic Crush!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"psychiczne wyssanie",	"",
	"psychic drain",	{ 0 },
	NULL,                   { 0 },
	spell_psychic_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	0,	30,	0,	"",
	"Przestajesz by`c zm`eczon$y.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"psychiczne leczenie",	"",
	"psychic healing",	{ 0 },
	NULL,                   { 0 },
	spell_psychic_healing,	TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,			20,	 12,
	0,	30,	0,	"",
	"!Psychic Healing!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"posta`c cienia",	"cie`n",
	"shadow form",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_shadow,		0,     8,
	0,	30,	0,	"",
	"Przestajesz porusza`c si`e jak cie`n.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"podzia`l si`ly",	"",
	"share strength",	{ 0 },
	NULL,                   { 0 },
	spell_share_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			8,     8,
	0,	30,	0,	"",
	"Nie wsp`o`ldzielisz ju`z z nikim swojej si`ly.",	"",
	NULL,			SKILL_NO_DISPEL
    },

    {
	"tarcza my`slowa",	"",
	"thought shield",	{ 0 },
	NULL,                   { 0 },
	spell_thought_shield,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,     8,
	0,	20,	0,	"",
	"Nie czujesz si`e ju`z tak dobrze chronion$y.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"pot`e`zny wybuch",	"",
	"ultrablast",		{ 0 },
	NULL,                   { 0 },
	spell_ultrablast,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			75,	18,
	0,	40,	1,	"pot`e`zny wybuch",
	"!Ultrablast!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"wbijanie ko`lka",	"",
	"stake",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_stake,		0,	 8,
	0,	30,	1,	"wymierzony cios",
	"Kl`atwa gula opada z ciebie.", "Kl`atwa gula opada z $1.",
	NULL,			SKILL_NO_DISPEL
    },

/* Race ability spells */
    { /* uzywane obecnie chyba tylko w spec_poison */
	"ugryzienie wampira",	"",
	"vampiric bite",	{ 0 },
	NULL,                   { 0 },
	NULL, /*spell_vampiric_bite*/	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_vampiric_bite,	0,     12,
	0,	100,	0,	"ugryzienie",
	"",	"",
	NULL,			SKILL_NO_DISPEL | SKILL_NPC
    },

    {
	"posta`c nietoperza",	"",
	"posta`c nietoperza",	{ 0 },
	NULL,                   { 0 },
	NULL,			TAR_IGNORE,	POS_STANDING,
	&gsn_postac_nietoperza,	0,     12,
	0,	0,	0,	"",
	"Wracasz do pierwotnej postaci.",	"$n powraca do pierwotnej postaci.",
	NULL,			SKILL_NO_DISPEL | SKILL_NPC
    },

    {
	"taranowanie",		"",
	"headbutt",		{ 0 },
	NULL,                   { 0 },
	NULL,			TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_taranowanie,	0,     12,
	0,	100,	2,	"twarda g`lowa",
	"",	"",
	NULL,			SKILL_NPC
    },

/*
 * Dragon breath
 */
    {
	"kwa`sny oddech",	"",
	"acid breath",		{ 0 },
	NULL,                   { 0 },
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_acid_breath,	50,	12,
	0,	40,	1,	"kwa`sny oddech",
	"!Acid Breath!",	"",
	NULL,			SKILL_NPC
    },

    {
	"p`lomienny oddech",	"",
	"fire breath",		{ 0 },
	NULL,                   { 0 },
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_fire_breath,	50,	12,
	0,	40,	1,	"p`lomienny oddech",
	"!Fire Breath!",	"",
	NULL,			SKILL_NPC
    },

    {
	"mro`xny oddech",	"",
	"frost breath",		{ 0 },
	NULL,                   { 0 },
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_frost_breath,	50,	12,
	0,	40,	1,	"mro`xny oddech",
	"!Frost Breath!",	"",
	NULL,			SKILL_NPC
    },

    {
	"gazowy oddech",	"",
	"gas breath",		{ 0 },
	NULL,                   { 0 },
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	&gsn_gas_breath,	50,	12,
	0,	40,	1,	"gazowy oddech",
	"!Gas Breath!",		"",
	NULL,			SKILL_NPC
    },

    {
	"pora`zaj`acy oddech",	"",
	"lightning breath",	{ 0 },
	NULL,                   { 0 },
	spell_lightning_breath, TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_lightning_breath,	50,	12,
	0,	40,	1,	"pora`zaj`acy oddech",
	"!Lightning Breath!",	"",
	NULL,			SKILL_NPC
    },

/*
 * Odkad okazalo sie, ze czasami powoduje to problemy, przestalem umieszczac
 * nowe umiejetnosci w srodek pliku, a zaczalem dopisywac na koniec.
 * Nie ma przedmiotow z tymi czarami, wiec moga byc w srodku.
 *
 * Lam: przedmioty z czarami juz poprawione - to byl kolejny blad w Envy
 */
    {
	"alchemia",		"",
	"alchemy",		{ 0 },
	NULL,                   { 0 },
	spell_alchemy,		TAR_OBJ_INV,	POS_STANDING,
	NULL,			50,	12,
	0,	40,	0,	"",
	"!Alchemy!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"rehabilitacja",	"",
	"recover",		{ 0 },
	NULL,                   { 0 },
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	8,
	0,	35,	0,	"",
	"!Recover!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"`slepota",		"",
	"dazzle",		{ 0 },
	NULL,                   { 0 },
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_dazzle,		5,	12,
	0,	30,	0,	"",
	"",			"",
	wear_off_blindness,	SKILL_NORMAL
    },

    {
	"wi`ecej informacji",	"",
	"enhanced whois",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_DEAD,
	&gsn_enhanced_whois,	0,	 0,
	0,	40,	0,	"",
	"!Enhanced Whois!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"szybka ucieczka",	"",
	"fast flee",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_fast_flee,		0,	 0,
	0,	40,	0,	"",
	"!Fast Flee!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"wciskanie",		"",
	"fetch",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_fetch,		0,	 0,
	0,	30,	0,	"",
	"!Fetch!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"targowanie",		"",
	"haggle",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_haggle,		0,	 0,
	0,	30,	0,	"",
	"!Haggle!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"r`ownowaga",		"",
	"no trip",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_no_trip,		0,	 0,
	0,	40,	0,	"",
	"!No Trip!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"pods`luchiwanie",	"",
	"overhear",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_overhear,		0,	 0,
	0,	80,	0,	"",
	"!Overhear!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"telepatia",		"",
	"telepathy",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_DEAD,
	&gsn_telepathy,		0,	0,
	100,	100,	0,	"",
	"!Telepathy!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"mocne dzier`zenie",	"",
	"strong wield",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_strong_wield,	0,	 0,
	0,	40,	0,	"",
	"!Strong Wield!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"czarna aura",		"",
	"black aura",		{ 0 },
	NULL,			{ 0 },
	spell_black_aura,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			75,	12,
	0,	40,	0,	"",
	"Czarna aura ulega rozproszeniu.",	"Czarna aura opada z $1.",
	NULL,			SKILL_NORMAL
    },

    {
	"uspokajanie",		"spok`oj",
	"calm",			{ 0 },
	NULL,                   { 0 },
	spell_awe,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	0,	30,	0,	"",
	"!Awe!",		"",
	NULL,			SKILL_DEF_VICT
    },

    {
	"drogowskaz",		"",
	"drogowskaz",		{ 0 },
	NULL,			{ 0 },
	spell_drogowskaz,	TAR_IGNORE,	POS_STANDING,
	NULL,			30,	8,
	0,	40,	0,	"",
	"!Drogowskaz!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"brama",		"",
	"brama",		{ 0 },
	NULL,			{ 0 },
	spell_brama,		TAR_IGNORE,	POS_STANDING,
	NULL,			50,	12,
	0,	40,	0,	"",
	"!Brama!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"okno",			"",
	"okno",			{ 0 },
	NULL,			{ 0 },
	spell_okno,		TAR_IGNORE,	POS_STANDING,
	NULL,			20,	8,
	0,	40,	0,	"",
	"!Okno!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"krwawe `lzy",		"`lzy",
	"krwawe `lzy",		{ 0 },
	NULL,                   { 0 },
	spell_krwawe_lzy,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_krwawe_lzy,	5,	12,
	0,	30,	0,	"",
	"",			"",
	wear_off_blindness,	SKILL_NORMAL
    },

    {
	"magiczna paj`eczyna",	"paj`eczyna",
	"magiczna paj`eczyna",	{ 0 },
	NULL,                   { 0 },
	spell_pajeczyna,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_pajeczyna,		5,	12,
	0,	30,	0,	"",
	"Czujesz si`e mniej skr`epowan$y.",	"$n uwalnia si`e z magicznej paj`eczyny.",
	NULL,			SKILL_NORMAL
    },

    {
	"podgl`adanie",		"",
	"podgl`adanie",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_podglad,		0,	 0,
	0,	80,	0,	"",
	"!Overhear!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"w`sciek`lo`s`c",		"",
	"fury",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_fury,		0,	 12,
	0,	40,	0,	"",
	"Uspokajasz si`e.",	"",
	NULL,			SKILL_NORMAL
    },

/*
 * Fuyara: skrytobojca
 */
    {
	"anatomia",		"",
	"anatomy",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_anatomy,		0,	0,
	0,	80,	1,	"",
	"!Anatomy!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"ocenianie",		"",
	"appraise",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_appraise,		0,	12,
	0,	35,	0,	"",
	"!Ocena!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"zab`ojcza celno`s`c",	"",
	"assasinate",		{ 0 },
	"podst`epny atak",      { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_assasinate,	0,	0,
	0,	45,	1,	"",
	"!Assasinate!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"kontrola chi",		"chi",
	"assemble",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_assemble,		0,	18,
	0,	50,	0,	"",
	"Przestajesz kontrolowa`c przep`lyw chi.","",
	NULL,			SKILL_NO_DISPEL
    },
    {
	"`swiadomo`s`c",	"",
	"awareness",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_awareness,		0,	18,
	0,	40,	0,	"",
	"Twoja `swiadomo`s`c otoczenia maleje.","",
	NULL,			SKILL_NO_DISPEL
    },
    {
	"koncentracja",		"",
	"concentrate",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_concentrate,	0,	18,
	0,	60,	0,	"",
	"Twoja koncentracja maleje.",	"",
	NULL,			SKILL_NO_DISPEL
    },
    {
	"krytyczne trafienie",	"",
	"critical hit",		{ 0 },
	"anatomia",             { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_critical_hit,	0,	0,
	0,	80,	1,	"",
	"!Critical!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"kumulacja si`ly",	"",
	"cumulate strength",	{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_cumulate_strength,	0,	18,
	0,	60,	0,	"",
	"Efekt skumulowanej si`ly zanika.","",
	NULL,			SKILL_NO_DISPEL
    },
    {
	"technika `smierci",	"`smier`c",
	"death tech",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_death_tech,	0,	24,
	0,	60,	1,	"cios `smierci",
	"!Cios!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"maskowanie",		"",
	"disguise",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_disguise,		0,	20,
	0,	60,	0,	"",
	"Przestajesz si`e maskowa`c.","$n nagle wy`lania si`e nie wiadomo sk`ad.",
	NULL,			SKILL_NO_DISPEL
    },
    {
	"technika smoka",	"smok",
	"dragon tech",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_dragon_tech,	0,	16,
	0,	60,	1,	"cios smoka",
	"!Cios!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"wzmocnione kopni`ecie","",
	"enhanced kick",	{ 0 },
	"kopanie",              { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_enhanced_kick,	0,	0,
	0,	60,	0,	"",
	"!Kick enczanced!",	"",
	NULL,			SKILL_NORMAL
    },
    {
	"b`lysk",		"",
	"flash",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_flash,		0,	3,
	0,	30,	1,	"",
	"!Blysk!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"wysoka efektywno`s`c",	"",
	"high efficiency",	{ 0 },
	"uniki",                { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_high_efficiency,	0,	0,
	0,	180,	0,	"",
	"!High!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"wtajemniczenie",	"inicjacja",
	"initiation",		{ 0 },
	NULL,			{ 0 },
	spell_null,		TAR_IGNORE,		POS_DEAD,
	&gsn_initiation,	0,	0,
	0,	120,	0,	"",
	"!Initiation!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"podskok",		"",
	"jump",			{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_jump,		0,	0,
	0,	60,	1,	"",
	"!Jump!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"pot`e`zne r`ece",	"",
	"mighty hands",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_mighty_hands,	0,	0,
	0,	120,	0,	"",
	"!Mighty!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"technika mocy",	"moc",
	"power tech",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_power_tech,	0,	20,
	0,	65,	1,	"mocny cios",
	"!Cios!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"wyci`aganie",		"",
	"pull",			{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_pull,		0,	5,
	0,	40,	0,	"",
	"!Pull!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"wzmacnianie broni",	"",
	"reinforce blade",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_reinforce_blade,	0,	30,
	0,	20,	0,	"",
	"!Reinforce!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"runy",			"",
	"runes",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_IGNORE,		POS_DEAD,
	&gsn_runes,		0,	0,
	0,	100,	0,	"",
	"!Runy!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"runa `swiat`la",	"",
	"rune:flare",		{ 0 },
	"runy",                 { 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_rune_flare,	0,	10,
	0,	60,	0,	"",
	"",			"",
	wear_off_blindness,	SKILL_NORMAL
    },
    {
	"runa ognia",		"",
	"rune:fire",		{ 0 },
	"runy",                 { 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_rune_fire,		0,	18,
	0,	60,	2,	"ognista kula",
	"!Runa ognia!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"runa lodu",		"",
	"rune:ice",		{ 0 },
	"runy",                 { 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_rune_ice,		0,	16,
	0,	60,	1,	"sopel lodu",
	"Przenikliwy ch`l`od mija.",	"",
	NULL,			SKILL_NORMAL
    },
    {
	"runa trucizny",	"",
	"rune:poison",		{ 0 },
	"runy",                 { 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_rune_poison,	0,	14,
	0,	60,	0,	"",
	"Os`labienie i md`lo`sci ustaj`a.",	"",
	NULL,			SKILL_NORMAL
    },
    {
	"zwi`ekszona uwaga",	"",
	"search",		{ 0 },
	"`swiadomo`s`c",        { 0 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_search,		0,	12,
	0,	80,	0,	"",
	"Przestajesz zwraca`c wi`eksz`a uwag`e na otoczenie.","",
	NULL,			SKILL_NO_DISPEL
    },
    {
	"technika szybko`sci",	"szybko`s`c",
	"speed tech",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_speed_tech,	0,	4,
	0,	65,	1,	"szybki cios",
	"Zm`eczenie mija.",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"podst`epny atak",	"",
	"strike",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_strike,		0,	24,
	0,	45,	1,	"podst`epny atak",
	"!Strike!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"obuch",		"",
	"obuch",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_stun,		0,	16,
	0,	60,	1,	"cios w g`low`e",
	"!Stun!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"m`lynek",		"",
	"whirl",		{ 0 },
	"dwie bronie",          { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_whirl,		0,	24,
	0,	60,	1,	"cios z obrotu",
	"!Mlynek!",		"",
	NULL,			SKILL_NORMAL
    },
    {
	"przeobra`zenie w widmo","widmo",
	"wraith morph",		{ 0 },
	"wtajemniczenie",	{ 0 },
	spell_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_wraith_morph,	0,	12,
	0,	40,	1,	"",
	"Efekt widma zanika.",	"$n przestaje przypomina`c widmo.",
	NULL,			SKILL_NO_DISPEL
    },

    {
	"walka na o`slep",	"",
	"walka na o`slep",	{ 0 },
	NULL,			{ 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_walka_na_oslep,	0,	0,
	0,     100,	0,	"",
	"!walka na `slepo!",	"",
	NULL,			SKILL_NORMAL
    },

/*
 * Qwert: mnich
 */
    {
	"aura pokoju",		"",
	"aura pokoju",		{ 0 },
	NULL,                   { 0 },
	spell_aura_pokoju,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_aura_pokoju,	30,     12,
	0,	30,	0,	"",
	"Odczuwasz przyp`lyw niepokoju.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"duchowe wsparcie",	"",
	"duchowe wsparcie",	{ 0 },
	NULL,                   { 0 },
	spell_duchowe_wsparcie,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_duchowe_wsparcie,	15,     10,
	0,	30,	0,	"",
	"Duchy przesta`ly ci`e b`logos`lawi`c.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"bio przyspieszenie",	"",
	"bio przyspieszenie",	{ 0 },
	NULL,			{ 0 },
	spell_bio_przyspieszenie,TAR_CHAR_SELF,		POS_STANDING,
	&gsn_bio_przyspieszenie,20,     8,
	0,	35,	0,	"",
	"Nagle tw`oj organizm zwolni`l sw`oj bieg.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"przeniesienie si`ly",	"",
	"przeniesienie si`ly",	{ 0 },
	NULL,                   { 0 },
	spell_przeniesienie_sily,TAR_CHAR_SELF,		POS_STANDING,
	&gsn_przeniesienie_sily,15,     12,
	0,	35,	0,	"",
	"Si`ly fizyczne wr`oci`ly do normy.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"ostrze ognia",		"",
	"ostrze ognia",		{ 0 },
	NULL,                   { 0 },
	spell_ostrze_ognia,	TAR_OBJ_INV,		POS_STANDING,
	&gsn_ostrze_ognia,	50,     15,
	0,	40,	0,	"",
	"!Ostrze Ognia!",	"",
	NULL,			SKILL_NORMAL
    },


    {
	"ostrze lodu",		"",
	"ostrze lodu",		{ 0 },
	NULL,                   { 0 },
	spell_ostrze_lodu,	TAR_OBJ_INV,		POS_STANDING,
	&gsn_ostrze_lodu,	50,     15,
	0,	40,	0,	"",
	"!Ostrze Lodu!",	"",
	NULL,			SKILL_NORMAL
    },


    {
	"ostrze chaosu",	"",
	"ostrze chaosu",	{ 0 },
	NULL,                   { 0 },
	spell_ostrze_chaosu,	TAR_OBJ_INV,		POS_STANDING,
	&gsn_ostrze_chaosu,	70,     15,
	0,	40,	0,	"",
	"Czujesz si`e lepiej.",	"",
	NULL,			SKILL_FRIENDLY_DISPEL
    },

    {
	"przeniesienie umys`lu","",
	"przeniesienie umys`lu",{ 0 },
	NULL,                   { 0 },
	spell_przeniesienie_umyslu,TAR_CHAR_SELF,	POS_STANDING,
	&gsn_przeniesienie_umyslu,15,     8,
	0,	30,	0,	"",
	"Czujesz powr`ot umys`lu.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"lodowa tarcza",	"",
	"lodowa tarcza",	{ 0 },
	NULL,                   { 0 },
	spell_lodowa_tarcza,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_lodowa_tarcza,	45,     12,
	0,	40,	0,	"",
	"Czujesz nag`ly przyp`lyw gor`aca.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"sztorm lodowy",	"",
	"sztorm lodowy",	{ 0 },
	NULL,                   { 0 },
	spell_sztorm_lodowy,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,     12,
	0,	30,	0,	"uderzenie lodu",
	"!Sztorm Lodowy!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"przekonanie",		"",
	"przekonanie",		{ 0 },
	NULL,                   { 0 },
	spell_przekonanie,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			100,     20,
	0,	35,	0,	"",
	"!Przekonanie!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"dezorientacja",	"",
	"dezorientacja",	{ 0 },
	NULL,                   { 0 },
	spell_dezorientacja,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_dezorientacja,	15,     12,
	0,	30,	0,	"",
	"Czujesz powr`ot zmys`l`ow.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"oczyszczenie",		"",
	"oczyszczenie",		{ 0 },
	NULL,                   { 0 },
	spell_oczyszczenie,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			30,     12,
	0,	30,	0,	"",
	"!Oczyszczenie!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"przekle`nstwo natury",	"",
	"przekle`nstwo natury",	{ 0 },
	NULL,                   { 0 },
	spell_przeklenstwo_natury,TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_przeklenstwo_natury,18,     8,
	0,	30,	0,	"",
	"Natura zn`ow darzy ci`e sympati`a.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"przyspieszenie",	"",
	"przyspieszenie",	{ 0 },
	NULL,                   { 0 },
	spell_przyspieszenie,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_przyspieszenie,	30,     8,
	0,	30,	0,	"",
	"Twoje ruchy staj`a si`e wolniejsze.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"spowolnienie",		"",
	"spowolnienie",		{ 0 },
	NULL,                   { 0 },
	spell_spowolnienie,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_spowolnienie,	15,     12,
	0,	30,	0,	"",
	"Organizm przyspieszy`l sw`oj bieg.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"zaka`zenie",		"",
	"zaka`zenie",		{ 0 },
	NULL,                   { 0 },
	spell_zakazenie,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_zakazenie,		40,     15,
	0,	30,	0,	"",
	"Tw`oj organizm oczy`sci`l si`e.",	"",
	NULL,			SKILL_NORMAL
    },


    {
	"rytua`l `smierci",	"",
	"rytua`l `smierci",	{ 0 },
	NULL,                   { 0 },
	spell_rytual_smierci,	TAR_IGNORE,		POS_STANDING,
	NULL,			1,     8,
	0,	40,	0,	"",
	"!Rytual Smierci!",	"",
	NULL,			SKILL_NORMAL
    },


    {
	"`sciana ognia",	"",
	"`sciana ognia",	{ 0 },
	NULL,                   { 0 },
	spell_sciana_ognia,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			80,     18,
	0,	35,	2,	"`sciana ognia",
	"!Sciana Ognia!",	"",
	NULL,			SKILL_NORMAL
    },


    {
	"kuk`la",		"",
	"kuk`la",		{ 0 },
	NULL,                   { 0 },
	spell_kukla,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_kukla,		15,     8,
	0,	30,	0,	"",
	"Zn`ow mo`zesz tworzy`c duplikaty.",	"",
	NULL,			SKILL_NO_DISPEL | SKILL_NPC
    },


    {
	"przekazanie przedmiotu","",
	"przekazanie przedmiotu",{ 0 },
	NULL,                   { 0 },
	spell_przekazanie_przedmiotu,TAR_OBJ_INV,	POS_STANDING,
	NULL,			40,     8,
	0,	30,	0,	"",
	"!Przesylka!",		"",
	NULL,			SKILL_NORMAL
    },


    {
	"regeneracja",		"",
	"regeneracja",		{ 0 },
	NULL,                   { 0 },
	spell_regeneracja,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_regeneracja,	30,     8,
	0,	40,	0,	"",
	"Tempo regeneracji twego cia`la maleje.",	"",
	NULL,			SKILL_NORMAL
    },


    {
	"degeneracja",		"",
	"degeneracja",		{ 0 },
	NULL,                   { 0 },
	spell_degeneracja,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_degeneracja,	20,     12,
	0,	30,	0,	"",
	"Twoje cia`lo zaczyna normaln`a prac`e.",	"",
	NULL,			SKILL_NORMAL
    },


    {
	"pora`zenie",		"",
	"pora`zenie",		{ 0 },
	NULL,                   { 0 },
	spell_porazenie,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			2,     12,
	0,	30,	0,	"elektryczne pora`zenie",
	"!Porazenie!",		"",
	NULL,			SKILL_NORMAL
    },


    {
	"wywo`lanie strachu",	"",
	"wywo`lanie strachu",	{ 0 },
	NULL,                   { 0 },
	spell_wywolanie_strachu,TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,     8,
	0,	30,	1,	"przera`zaj`acy obraz",
	"!Fear!",		"",
	NULL,			SKILL_NORMAL
    },


    {
	"hipnoza",		"",
	"hipnoza",		{ 0 },
	NULL,                   { 0 },
	spell_hipnoza,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_hipnoza,		35,     4,
	0,	30,	0,	"",
	"Nareszcie masz woln`a wol`e.",	"",
	NULL,			SKILL_NORMAL
    },


    {
	"rewers",		"",
	"rewers",		{ 0 },
	NULL,                   { 0 },
	spell_rewers,		TAR_CHAR_SELF,		POS_STANDING,
	&gsn_rewers,		5,     8,
	0,	30,	0,	"",
	"Si`ly zn`ow biegn`a swoim torem.",	"",
	NULL,			SKILL_NO_DISPEL
    },


    {
	"przekazanie b`olu",	"",
	"przekazanie b`olu",	{ 0 },
	NULL,                   { 0 },
	spell_przekazanie_bolu,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,     12,
	0,	30,	1,	"b`ol",
	"!Pain!",		"",
	NULL,			SKILL_NORMAL
    },


    {
	"uwolnienie umys`l`ow",	"",
	"uwolnienie umys`l`ow",	{ 0 },
	NULL,                   { 0 },
	spell_uwolnienie_umyslow,TAR_IGNORE,		POS_STANDING,
	NULL,			30,     12,
	0,	30,	0,	"",
	"!Free Minds!",		"",
	NULL,			SKILL_KUPOWALNE
    },


    {
	"znami`e `smierci",	"",
	"znami`e `smierci",	{ 0 },
	NULL,                   { 0 },
	spell_znamie_smierci,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_znamie_smierci,	35,     8,
	0,	30,	0,	"",
	"!Ju`z nie `zyjesz!",	"",
	NULL,			SKILL_NO_DISPEL
    },

    {
	"przej`scie",		"",
	"przej`scie",		{ 0 },
	NULL,                   { 0 },
	spell_przejscie,	TAR_IGNORE,		POS_STANDING,
	NULL,			50,     15,
	0,	40,	0,	"",
	"!Przemieszczenie!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"lodowy kolec",		"",
	"lodowy kolec",		{ 0 },
	NULL,                   { 0 },
	spell_lodowy_kolec,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_lodowy_kolec,	15,	12,
	0,	40,	0,	"uk`lucie lodu",
	"!Icestrike!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"wyzwolenie mocy",      "",
	"wyzwolenie mocy",      { 0 },
	NULL,                   { 0 },
	spell_wyzwolenie_mocy,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   50,     12,
	0,      30,     2,	"POT`EGA",
	"!Wyzwolenie mocy!",	"",
	NULL,			SKILL_NORMAL
    },


    {
	"kumulacja mocy",       "",
	"kumulacja mocy",       { 0 },
	NULL,                   { 0 },
	spell_kumulacja_mocy,   TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
	&gsn_kumulacja_mocy,	15,     8,
	0,      40,     0,	"",
	"Odp`lyn`e`la z ciebie ca`la pot`ega.",	"",
	NULL,			SKILL_NORMAL
    },

/*
 * Ulryk: nekromanta
 */
    {
	"pora`zaj`acy dotyk",   "",
	"pora`zaj`acy dotyk",   { 0 },
	NULL,                   { 0 },
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_porazajacy_dotyk,  0,      0,
	0,      80,     1,      "pora`zaj`acy dotyk",
	"!porazajacy dotyk!",   "",
	NULL,			SKILL_NORMAL
    },

    {
	"mroczny znak",         "",
	"mroczny znak",         { 0 },
	NULL,                   { 0 },
	spell_mroczny_znak,     TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   10,     10,
	0,      30,     0,      "",
	"Tw`oj mroczny znak przemija.", "Mroczny znak $1 przemija.",
	NULL,			SKILL_NORMAL
    },

    {
	"balsamowanie",         "",
	"balsamowanie",         { 0 },
	NULL,                   { 0 },
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_balsamowanie,      0,      10,
	0,      40,     0,      "",
	"!balsamowanie!",       "",
	NULL,			SKILL_NORMAL
    },

    {
	"sprowadzenie szkieletu", "szkielet",
	"sprowadzenie szkieletu", { 0 },
	NULL,                   { 0 },
	spell_sprowadzenie_szkieletu, TAR_IGNORE,       POS_STANDING,
	&gsn_sprowadzenie_szkieletu, 25, 18,
	0,      40,     0,      "",
	"Twe ko`sci zaczynaj`a si`e kruszy`c.",
	"Ko`sci $1 rozsypuj`a si`e w proch.",
	NULL,			SKILL_NORMAL
    },

    {
	"odes`lanie",           "",
	"odes`lanie",           { 0 },
	NULL,                   { 0 },
	spell_odeslanie,        TAR_CHAR_DEFENSIVE,    POS_FIGHTING,
	NULL,                   5,      10,
	0,      30,      0,     "",
	"!odeslanie!",          "",
	NULL,			SKILL_NORMAL
    },

    {
	"wejrzenie",            "",
	"wejrzenie",            { 0 },
	NULL,                   { 0 },
	spell_wejrzenie,        TAR_IGNORE,            POS_STANDING,
	NULL,                   20,     5,
	0,      40,      0,     "",
	"!wejrzenie!",          "",
	NULL,			SKILL_NORMAL
    },

    {
	"wzmocnienie umys`lu",  "",
	"wzmocnienie umys`lu",  { 0 },
	NULL,                   { 0 },
	spell_wzmocnienie_umyslu, TAR_CHAR_SELF,       POS_STANDING,
	NULL,                   20,    15,
	0,      30,      0,     "",
	"Tw`oj umys`l przestaje by`c wzmocniony.", "",
	NULL,			SKILL_NORMAL
    },

    {
	"nekromantyczna zbroja", "",
	"nekromantyczna zbroja", { 0 },
	NULL,                   { 0 },
	spell_nekromantyczna_zbroja, TAR_CHAR_DEFENSIVE, POS_STANDING,
	NULL,                  15,  10,
	0,      35,      0,     "",
	"Nekromantyczna zbroja opada z ciebie.",
	"Nekromantyczna zbroja opada z $1.",
	NULL,			SKILL_NORMAL
    },

    {
	"mroczny p`lomie`n",    "",
	"mroczny p`lomie`n",    { 0 },
	NULL,                   { 0 },
	spell_mroczny_plomien,  TAR_CHAR_OFFENSIVE,    POS_FIGHTING,
	&gsn_mroczny_plomien,   15,     12,
	0,      40,      1,     "p`lomie`n `smierci",
	"!mroczny p`lomie`n",   "",
	NULL,			SKILL_NORMAL
    },

    {
	"kontrola umys`lu",     "",
	"kontrola umys`lu",     { 0 },
	NULL,                   { 0 },
	spell_kontrola_umyslu,  TAR_CHAR_OFFENSIVE,    POS_STANDING,
	&gsn_kontrola_umyslu,   25,      10,
	0,      30,      0,     "",
	"Odzyskujesz kontrol`e nad swoim umys`lem.", "",
	NULL,			SKILL_NORMAL
    },

    {
	"wyssanie duszy",       "",
	"wyssanie duszy",       { 0 },
	NULL,                   { 0 },
	spell_wyssanie_duszy,   TAR_CHAR_OFFENSIVE,    POS_FIGHTING,
	NULL,                   30,     15,
	0,      40,      0,     "wysysanie duszy",
	"Czujesz si`e ju`z mniej ot`epia`l$y.", "$n odzyska`l$a zdrowy wygl`ad.",
	NULL,			SKILL_NORMAL
    },

    {
	"wskrzeszenie",         "",
	"wskrzeszenie",         { 0 },
	NULL,                   { 0 },
	spell_wskrzeszenie,     TAR_IGNORE,            POS_STANDING,
	&gsn_wskrzeszenie,      30,     20,
	0,      40,      0,     "",
	"`Zycie ulatuje z ciebie.", "$n rozpada si`e na twoich oczach.",
	NULL,			SKILL_NORMAL
    },

    {
	"poca`lunek `smierci",  "",
	"poca`lunek `smierci",  { 0 },
	NULL,                   { 0 },
	spell_pocalunek_smierci, TAR_IGNORE,           POS_STANDING,
	&gsn_pocalunek_smierci, 1,      15,
	0,      40,      0,     "",
	"!pocalunek smierci!",  "",
	NULL,			SKILL_NORMAL
    },

    {
	"eksplozja",            "",
	"eksplozja",            { 0 },
	NULL,                   { 0 },
	spell_eksplozja,        TAR_IGNORE,            POS_FIGHTING,
	&gsn_eksplozja,         50,     20,
	0,      30,      2,     "eksplozja zw`lok",
	"!eksplozja!",          "",
	NULL,			SKILL_NORMAL
    },

    {
	"kradzie`z cienia",     "",
	"kradzie`z cienia",     { 0 },
	NULL,                   { 0 },
	spell_kradziez_cienia,  TAR_CHAR_OFFENSIVE,    POS_FIGHTING,
	NULL,                   15,     12,
	0,      40,      2,     "kradzie`z cienia",
	"Tw`oj cie`n powraca do ciebie.", "Cie`n $1 powraca do $b.",
	NULL,			SKILL_NORMAL
    },

    {
	"aura strachu",         "",
	"aura strachu",         { 0 },
	NULL,                   { 0 },
	spell_aura_strachu,     TAR_CHAR_SELF,         POS_STANDING,
	NULL,                   30,     10,
	0,      35,      0,     "",
	"Przestajesz budzi`c groz`e.", "$n przestaje budzi`c groz`e.",
	NULL,			SKILL_NORMAL
    },

    {
	"strefa cienia",        "",
	"strefa cienia",        { 0 },
	NULL,                   { 0 },
	spell_strefa_cienia,    TAR_CHAR_SELF,         POS_STANDING,
	NULL,                   35,     15,
	0,      30,      0,     "",
	"",			"Mrok wok`o`l $1 rozst`epuje si`e.",
	wear_off_strefa_cienia,	SKILL_NORMAL
    },

    {
	"stworzenie mumii",     "mumia",
	"stworzenie mumii",     { 0 },
	"balsamowanie",         { 0 },
	spell_stworzenie_mumii, TAR_IGNORE,            POS_STANDING,
	&gsn_stworzenie_mumii,  50,     20,
	0,      30,      0,     "",
	"`Zycie ulatuje z ciebie.",	"$n rozpada si`e na twoich oczach.",
	NULL,			SKILL_NORMAL
    },

    {
	"koszmar",              "",
	"koszmar",              { 0 },
	NULL,                   { 0 },
	spell_koszmar,          TAR_CHAR_OFFENSIVE,    POS_STANDING,
	NULL,                   30,     20,
	0,      30,      1,     "koszmar",
	"!koszmar!",            "",
	NULL,			SKILL_NORMAL
    },

    {
	"przekl`ety deszcz",    "",
	"przekl`ety deszcz",    { 0 },
	NULL,                   { 0 },
	spell_przeklety_deszcz, TAR_IGNORE,            POS_FIGHTING,
	NULL,                   20,     15,
	0,      30,      1,     "cienisty opad",
	"!przeklety deszcz!",   "",
	NULL,			SKILL_NORMAL
    },

    {
	"tarcza umar`lych",     "",
	"tarcza umar`lych",     { 0 },
	"mroczny p`lomie`n",    { 0 },
	spell_tarcza_umarlych,  TAR_CHAR_SELF,         POS_STANDING,
	NULL,                   50,     15,
	0,      30,     0,      "",
	"Tarcza umar`lych opada z ciebie.", "Tarcza umar`lych opada z $1.",
	NULL,			SKILL_NORMAL
    },

    {
	"wycie`nczenie",        "",
	"wycie`nczenie",        { 0 },
	NULL,                   { 0 },
	spell_wycienczenie,     TAR_CHAR_OFFENSIVE,    POS_FIGHTING,
	NULL,                   25,     15,
	0,      30,     0,      "przenikaj`ace spojrzenie",
	"Odzyskujesz si`ly.",   "$n odzyskuje si`ly.",
	NULL,			SKILL_NORMAL
    },

    {
	"cienisty pocisk",      "",
	"cienisty pocisk",      { 0 },
	NULL,                   { 0 },
	spell_cienisty_pocisk,  TAR_CHAR_OFFENSIVE,    POS_FIGHTING,
	NULL,                   20,     15,
	0,      40,     1,      "cienisty pocisk",
	"!cienisty pocisk!",    "",
	NULL,			SKILL_NORMAL
    },

    {
	"tarcza z`la",		"",
	"tarcza z`la",		{ 0 },
	NULL,                   { 0 },
	spell_tarcza_zla,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	8,
	0,	30,	0,	"",
	"Czujesz si`e mniej chronion$y przed dobrem.",	"",
	NULL,			SKILL_NORMAL | SKILL_NO_COST
    },

    {
	"rozpalanie ognisk",	"",
	"rozpalanie ognisk",	{ 0 },
	NULL,			{ 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_rozpalanie_ognisk, 0,	30,
	0,	30,	0,	"",
	"Ognisko dogasa.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"uderzenie o ziemi`e",	"",
	"uderzenie o ziemi`e",	{ 0 },
	NULL,		{ 0 },
	NULL,		TAR_IGNORE,	POS_FIGHTING,
	&gsn_uderzenie_o,	0,		0,
	0,	0,	0,		"uderzenie o ziemi`e",
	"",	"",
	NULL,			SKILL_NO_DISPEL | SKILL_NPC
    },

    {
	"uderzenie w wod`e",	"",
	"uderzenie w wod`e",	{ 0 },
	NULL,		{ 0 },
	NULL,		TAR_IGNORE,	POS_FIGHTING,
	&gsn_uderzenie_w,	0,		0,
	0,	0,	0,		"uderzenie w wod`e",
	"",	"",
	NULL,			SKILL_NO_DISPEL | SKILL_NPC
    },

    {
	"upadek",	"",
	"upadek",	{ 0 },
	NULL,		{ 0 },
	NULL,		TAR_IGNORE,	POS_FIGHTING,
	&gsn_upadek,	0,		0,
	0,	0,	1,		"upadek",
	"",	"",
	NULL,			SKILL_NO_DISPEL | SKILL_NPC
    },

    {
	"pozytywne do`ladowanie",	"",
	"pozytywne do`ladowanie",	{ 0 },
	NULL,           { 0 },
	spell_pozytywne_doladowanie,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,	20,	 15,
	0,	30,	 0,	"",
	"",	"",
	NULL,			SKILL_NO_DISPEL
    },

    {
	"cud",		"",
	"cud",		{ 0 },
	NULL,		{ 0 },
	spell_cud,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,	5,	10,
	0,	30,	1,	"",
	"",	"",
	NULL,			SKILL_NO_DISPEL | SKILL_INVISIBLE
    },

    {
	"na`lo`zenie r`ak",	"",
	"na`lo`zenie r`ak",	{ 0 },
	NULL,                   { 0 },
	spell_nalozenie_rak,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			10,	8,
	0,	20,	0,	"",
	"",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"tarcza archanio`l`ow",	"",
	"tarcza archanio`l`ow",	{ 0 },
	NULL,                   { 0 },
	spell_tarcza_archaniolow, TAR_CHAR_SELF,	POS_STANDING,
	NULL,			20,	8,
	0,	30,	0,	"",
	"Anielska tarcza zanika.", "Anielska tarcza $1 zanika.",
	NULL,			SKILL_NORMAL
    },

    {
	"si`la jedno`sci",	"",
	"si`la jedno`sci",	{ 0 },
	NULL,                   { 0 },
	spell_sila_jednosci,	TAR_IGNORE,	POS_STANDING,
	NULL,			25,	8,
	0,	30,	2,	"",
	"Czujesz si`e s`labiej.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"boski py`l",		"",
	"boski py`l",		{ 0 },
	NULL,                   { 0 },
	spell_boski_pyl,	TAR_IGNORE,		POS_STANDING,
	NULL,			12,	10,
	0,	30,	1,	"",
	"",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"spadaj`ace gwiazdy",	"",
	"spadaj`ace gwiazdy",	{ 0 },
	NULL,                   { 0 },
	spell_spadajace_gwiazdy, TAR_IGNORE,		POS_FIGHTING,
	NULL,			20,	10,
	0,	30,	2,	"spadaj`aca gwiazda",
	"",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"anio`l str`o`z",	"",
	"anio`l str`o`z",	{ 0 },
	NULL,			{ 0 },
	spell_aniol_stroz,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_aniol_stroz,       30,	8,
	0,	30,	1,	"",
	"Anio`l ju`z ci`e nie ochrania.","",
	NULL,			SKILL_NO_DISPEL
    },

    {
	"stworzenie m`lota",	"",
	"stworzenie m`lota",	{ 0 },
	NULL,                   { 0 },
	spell_stworzenie_mlota,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	15,
	0,	30,	0,	"",
	"",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"rozb`lysk",		"",
	"rozb`lysk",		{ 0 },
	NULL,			{ 0 },
	spell_null,          TAR_CHAR_OFFENSIVE,        POS_FIGHTING,
	&gsn_rozblysk,      0,      10,
	0,      40,     1,   "",
	"",        "",
	NULL,			SKILL_NORMAL
    },

    {
	"niez`lomna wiara",	"",
	"niez`lomna wiara",	{ 0 },
	NULL,			{ 0 },
	spell_niezlomna_wiara, TAR_CHAR_SELF,           POS_FIGHTING,
	NULL,                   15,     5,
	0,      40,      0,     "",
	"Czujesz, `ze bogowie zn`ow ci pomog`a.", "",
	NULL,			SKILL_INVISIBLE
    },

    {
	"`swi`eta aura",	"",
	"`swi`eta aura",	{ 0 },
	NULL,			{ 0 },
	spell_swieta_aura,     TAR_CHAR_SELF,           POS_STANDING,
	NULL,                   35,     10,
	0,      30,      2,     "",
	"`Swi`eta aura opada z ciebie.", "`Swi`eta aura opada z $1.",
	NULL,			SKILL_NORMAL
    },

    {
	"zag`lada",		"",
	"zag`lada",		{ 0 },
	NULL,			{ 0 },
	spell_null,   TAR_IGNORE,             POS_FIGHTING,
	&gsn_zaglada,   0,      0,
	0,      30,     2,      "",
	"",   "",
	NULL,			SKILL_NORMAL
    },

    {
	"wrz`aca krew",		"",
	"wrz`aca krew",		{ 0 },
	NULL,                   { 0 },
	spell_wrzaca_krew,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	12,
	0,	30,	2,	"",
	"Krew przesta`la si`e w tobie gotowa`c.",	"",
	NULL,			SKILL_NO_DISPEL
    },

    {
	"stra`zniczy glif",	"",
	"stra`zniczy glif",	{ 0 },
	NULL,                   { 0 },
	spell_strazniczy_glif,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			30,     15,
	0,	40,	1,	"",
	"",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"jedno`s`c obronna",	"",
	"jedno`s`c obronna",	{ 0 },
	NULL,                   { 0 },
	spell_jednosc_obronna,  TAR_IGNORE,		POS_STANDING,
	NULL,			30,	15,
	0,	30,	2,	"",
	"Czujesz si`e mniej pewnie.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"kradzie`z `zycia",	"",
	"kradzie`z `zycia",	{ 0 },
	NULL,                   { 0 },
	spell_kradziez_zycia,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			40,	12,
	0,	40,	2,	"",
	"",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"stworzenie grabie`zcy",	"",
	"stworzenie grabie`zcy",	{ 0 },
	NULL,                   { 0 },
	spell_stworzenie_grabiezcy,	TAR_IGNORE,	POS_STANDING,
	NULL,			20,	15,
	0,	30,	0,	"",
	"",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"`swi`ety m`sciciel",	"",
	"`swi`ety m`sciciel",	{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_swiety_msciciel,	0,	 0,
	0,	40,	1,	"",
	"",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"jedno`s`c bojowa",	"",
	"jedno`s`c bojowa",	{ 0 },
	NULL,                   { 0 },
	spell_jednosc_bojowa,   TAR_IGNORE,		POS_STANDING,
	NULL,			50,	15,
	0,	30,	2,	"",
	"Jedno`s`c bojowa przemija.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"duchowa zbroja",	"",
	"duchowa zbroja",	{ 0 },
	NULL,                   { 0 },
	spell_duchowa_zbroja,   TAR_CHAR_SELF,		POS_STANDING,
	NULL,			25,	10,
	0,	30,	2,	"",
	"Duchowa zbroja zanika.", "Duchowa zbroja $1 zanika.",
	NULL,			SKILL_NORMAL
    },

    {
	"kocie ruchy",		"",
	"kocie ruchy",		{ 0 },
	NULL,                   { 0 },
	spell_kocie_ruchy,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			20,	8,
	0,	30,	3,	"",
	"Czujesz si`e mniej zr`eczn$y.",	"",
	NULL,			SKILL_NPC
    },

    {
	"iluzja",		"",
	"iluzja",		{ 0 },
	NULL,			{ 0 },
	spell_iluzja,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			80,	15,
	0,	30,	3,	"",
	"Okrywaj`aca ci`e iluzja rozprasza si`e.", "",
	NULL,			SKILL_NORMAL
    },

    {
	"zaburzenie energii",	"",
	"zaburzenie energii",	{ 0 },
	NULL,			{ 0 },
	spell_zaburzenie_energii, TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	8,
	0,	30,	3,	"",
	"Twoja energia stabilizuje si`e.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"profesjonalizm",	"",
	"profesjonalizm",	{ 0 },
	NULL,			{ 0 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_profesjonalizm,	0,	0,
	0,     100,	0,	"",
	"!profesjonalizm!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"blokada mentalna",	"",
	"blokada mentalna",	{ 0 },
	NULL,			{ 0 },
	spell_blokada_mentalna,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blokada_mentalna,	20,	12,
	0,	30,	3,	"",
	"Blokada mentalna mija.",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"odblokowanie mentalne", "",
	"odblokowanie mentalne", { 0 },
	NULL,			{ 0 },
	spell_odblokowanie_mentalne, TAR_CHAR_DEFENSIVE, POS_STANDING,
	NULL,			30,	12,
	0,	30,	3,	"",
	"!odblokowanie mentalne!",	"",
	NULL,			SKILL_NORMAL
    },

    {
	"kontrola nad wod`a",	"",
	"control water",	{ 0 },
	NULL,                   { 0 },
	spell_kontrola_nad_woda,TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_kontrola_nad_woda,	 10,	8,
	0,	30,	3,	"",
	"Tracisz kontrol`e nad wod`a.",	"",
	NULL,			SKILL_PSI_SELF
    },

    {
	"p`lywanie",		"",
	"swimming",		{ 0 },
	NULL,                   { 0 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_plywanie,		 0,	 0,
	0,	80,	0,	"",
	"!p`lywanie!",		"",
	NULL,			SKILL_NORMAL
    },

    {
	"rytua`l `zycia",	"",
	"rytua`l `zycia",	{ 0 },
	NULL,                   { 0 },
	spell_rytual_zycia,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			50,	12,
	0,	30,	0,	"",
	"!Rytua`l `zycia!",	"",
	NULL,			SKILL_NPC | SKILL_KUPOWALNE
    },

    {
	"splot many",		"",
	"cone of silence",	{ 0 },
	NULL,                   { 0 },
	spell_splot_many,	TAR_IGNORE,		POS_STANDING,
	NULL,			60,	16,
	0,	30,	0,	"",
	"Kana`ly many zmieniaj`a swoje tory.", "",
	NULL,			SKILL_NORMAL
    }

/*
    {
	NULL,			NULL,
	NULL,			{ 0 },
	NULL,			{ 0 },
	NULL,			0,			0,
	NULL,			0,	0,
	0,	0,	0,	NULL,
	NULL,			NULL,
	NULL,			0
    }
*/

};


/*
 * Domyslne nazwy stopni klanowych - Malven
 */
const char * clan_lev_name_mm[ 10 ] =	/* meski mianownik */
{
    "kandydat",		"nowicjusz",		"giermek",
    "rycerz",		"kap`lan",		"bohater",
    "namiestnik",	"mistrz",		"wielki mistrz",
    "za`lo`zyciel"
};


const char * clan_lev_name_zm[ 10 ] =	/* zenski mianownik */
{
    "kandydatka",	"nowicjuszka",		"s`lu`zebna",
    "dama",		"kap`lanka",		"bohaterka",
    "namiestniczka",	"mistrzyni",		"wielka mistrzyni",
    "za`lo`zycielka"
};


const char * clan_lev_name_md[ 10 ] =	/* meski dopelniacz */
{
    "kandydata",	"nowicjusza",		"giermka",
    "rycerza",		"kap`lana",		"bohatera",
    "namiestnika",	"mistrza",		"wielkiego mistrza",
    "za`lo`zyciela"
};


const char * clan_lev_name_zd[ 10 ] =	/* zenski dopelniacz */
{
    "kandydatki",	"nowicjuszki",		"s`lu`zebnej",
    "damy",		"kap`lanki",		"bohaterki",
    "namiestniczki",	"mistrzyni",		"wielkiej mistrzyni",
    "za`lo`zycielki"
};


const char * clan_lev_name_mn[ 10 ] =	/* meski narzednik */
{
    "kandydatem",	"nowicjuszem",		"giermkiem",
    "rycerzem",		"kap`lanem",		"bohaterem",
    "namiestnikiem",	"mistrzem",		"wielkim mistrzem",
    "za`lo`zycielem"
};


const char * clan_lev_name_zn[ 10 ] =	/* zenski narzednik */
{
    "kandydatk`a",	"nowicjuszk`a",		"s`lu`zebn`a",
    "dam`a",		"kap`lank`a",		"bohaterk`a",
    "namiestniczk`a",	"mistrzyni`a",		"wielk`a mistrzyni`a",
    "za`lo`zycielk`a"
};


/*
 * Nazwy stanow klanow - Malven
 */
const char * clan_stat_name[ 5 ] =
{
    "uszkodzony", "usuni`ety", "zawieszony", "nowy", "aktywny"
};


/*
 * Ilosc czlonkow kazdego stopnia, ktorych klan moze pomiescic - Malven
 */
const int max_clan_level_members[ 10 ] =
{
    0, 256, 128, 64, 32, 16, 8, 4, 2, 1
};


/*
 * Dwie ponizsze tabelki do "splaszczenia" systemu monetarnego. Stworzone
 * przez Lama 14.9.2004, w oparciu o wzory opracowane wraz z Muzgusem i dzieki
 * symulacji stworzonej przez Muzgusa. Oprocz obnizenia wplywow i wydatkow,
 * z biegiem poziomow wzrasta proporcja wydatkow do wplywow, co ma zapobiegac
 * rozrzutnosci graczy z wyzszych poziomow mimo, ze latwiej bedzie im gromadzic
 * male fortuny na slabszych od siebie mobach.
 */
/*
 * Obliczona wartosc ( sqrt( poziom ) ^ 3 ) * 1024 dla poziomow 0-111
 * poziom 0 jak 1
 */
const int przelicznik_zysku[ 112 ] =
{
	1024,
	1024,	2896,	5321,	8192,	11449,	15050,	18965,	23170,	27648,	32382,
	37358,	42567,	47997,	53640,	59489,	65536,	71775,	78200,	84807,	91589,
	98544,	105666,	112951,	120397,	128000,	135756,	143663,	151718,	159918,	168260,
	176743,	185364,	194120,	203010,	212032,	221184,	230464,	239870,	249400,	259054,
	268829,	278724,	288737,	298868,	309114,	319475,	329949,	340535,	351232,	362039,
	372954,	383977,	395106,	406341,	417680,	429123,	440669,	452316,	464064,	475912,
	487859,	499905,	512048,	524288,	536624,	549055,	561581,	574200,	586913,	599718,
	612615,	625603,	638681,	651850,	665108,	678454,	691888,	705410,	719019,	732715,
	746496,	760363,	774314,	788350,	802469,	816672,	830958,	845325,	859775,	874307,
	888919,	903611,	918384,	933237,	948168,	963179,	978267,	993434,	1008678,1024000,
	1039398,1054873,1070424,1086050,1101752,1117529,1133380,1149306,1165305,1181378,
	1197525
};


/*
 * To samo dla ( sqrt( poziom ) ^ 3,6 ) * 1024
 */
const int przelicznik_wydatkow[ 112 ] =
{
	1024,
	1024,	3566,	7398,	12417,	18554,	25762,	34000,	43238,	53449,	64610,
	76702,	89707,	103609,	118394,	134049,	150562,	167922,	186119,	205143,	224985,
	245638,	267092,	289341,	312378,	336196,	360788,	386149,	412273,	439154,	466786,
	495166,	524288,	554147,	584739,	616059,	648104,	680868,	714349,	748542,	783444,
	819051,	855359,	892366,	930068,	968462,	1007544,1047312,1087763,1128893,1170701,
	1213183,1256336,1300159,1344648,1389802,1435616,1482090,1529221,1577007,1625445,
	1674533,1724269,1774651,1825677,1877345,1929652,1982598,2036179,2090395,2145243,
	2200721,2256828,2313562,2370921,2428903,2487508,2546732,2606575,2667035,2728110,
	2789799,2852101,2915013,2978535,3042664,3107400,3172741,3238685,3305232,3372380,
	3440127,3508472,3577415,3646952,3717085,3787810,3859127,3931035,4003532,4076617,
	4150290,4224548,4299391,4374818,4450827,4527417,4604588,4682338,4760665,4839570,
	4919051
};
