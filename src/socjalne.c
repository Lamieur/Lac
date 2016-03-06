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
#include "const.h"


typedef void SOC_FUN	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
#define SOCJALNE( nazwa ) void nazwa( CHAR_DATA *ch, CHAR_DATA *victim )

SOCJALNE( telepatyczne_obrazy );
SOCJALNE( soc_ballada );
SOCJALNE( soc_blagaj );
SOCJALNE( soc_bohater );
SOCJALNE( soc_brak );
SOCJALNE( soc_brew );
SOCJALNE( soc_browar );
SOCJALNE( soc_buziak );
SOCJALNE( soc_cierp );
SOCJALNE( soc_czesc );
SOCJALNE( soc_czulosc );
SOCJALNE( soc_doping );
SOCJALNE( soc_drzyj );
SOCJALNE( soc_dziekuje );
SOCJALNE( soc_eskimos );
SOCJALNE( soc_fajka );
SOCJALNE( soc_glaszcz );
SOCJALNE( soc_grozba );
SOCJALNE( soc_grupka );
SOCJALNE( soc_gwizd );
SOCJALNE( soc_hmm );
SOCJALNE( soc_hologram );
SOCJALNE( soc_ideal );
SOCJALNE( soc_jezyczek );
SOCJALNE( soc_jezyk );
SOCJALNE( soc_kac );
SOCJALNE( soc_klaszcz );
SOCJALNE( soc_klatwa );
SOCJALNE( soc_kochaj );
SOCJALNE( soc_kopniak );
SOCJALNE( soc_krytykuj );
SOCJALNE( soc_kucnij );
SOCJALNE( soc_kwiaty );
SOCJALNE( soc_lituj );
SOCJALNE( soc_laskocz );
SOCJALNE( soc_marzenie );
SOCJALNE( soc_masuj );
SOCJALNE( soc_miziaj );
SOCJALNE( soc_modlitwa );
SOCJALNE( soc_mrugnij );
SOCJALNE( soc_mudoholik );
SOCJALNE( soc_muhaha );
SOCJALNE( soc_niepal );
SOCJALNE( soc_noga );
SOCJALNE( soc_obliz );
SOCJALNE( soc_omdlenie );
SOCJALNE( soc_ocieraj );
SOCJALNE( soc_otrzyj );
SOCJALNE( soc_oswiadcz );
SOCJALNE( soc_papa );
SOCJALNE( soc_paw );
SOCJALNE( soc_ping );
SOCJALNE( soc_placz );
SOCJALNE( soc_pobudka );
SOCJALNE( soc_podziw );
SOCJALNE( soc_pogroz );
SOCJALNE( soc_policzkuj );
SOCJALNE( soc_przepros );
SOCJALNE( soc_prztyczek );
SOCJALNE( soc_przytaknij );
SOCJALNE( soc_przytul );
SOCJALNE( soc_pukaj );
SOCJALNE( soc_racja );
SOCJALNE( soc_reka );
SOCJALNE( soc_rumieniec );
SOCJALNE( soc_salutuj );
SOCJALNE( soc_skrec );
SOCJALNE( soc_sluchaj );
SOCJALNE( soc_smutek );
SOCJALNE( soc_sraport );
SOCJALNE( soc_szturchnij );
SOCJALNE( soc_sciemniaj );
SOCJALNE( soc_smiech );
SOCJALNE( soc_taniec );
SOCJALNE( soc_toast );
SOCJALNE( soc_udawaj );
SOCJALNE( soc_uklon );
SOCJALNE( soc_ukolysz );
SOCJALNE( soc_unies );
SOCJALNE( soc_usteczka );
SOCJALNE( soc_uscisk );
SOCJALNE( soc_usmiech );
SOCJALNE( soc_warcz );
SOCJALNE( soc_wazelina );
SOCJALNE( soc_wlm );
SOCJALNE( soc_wskaz );
SOCJALNE( soc_wtul );
SOCJALNE( soc_wykrzyw );
SOCJALNE( soc_wypij );
SOCJALNE( soc_wyzwij );
SOCJALNE( soc_zawod );
SOCJALNE( soc_zgadula );
SOCJALNE( soc_zgniec );
SOCJALNE( soc_zaprzecz );
SOCJALNE( soc_ziewaj );


/*
 * Uzywane (albo nie) przez kazde z polecen socjalnych. Polecenia socjalne moga
 * wypisywac cos innego niz te komunikaty albo w ogole nie umozliwiac uzywania
 * na odleglosc.
 */
void telepatyczne_obrazy( CHAR_DATA *ch, CHAR_DATA *victim )
{
    act( ZM_ZAWSZE, "Telepatycznie pokazujesz $@ obraz siebie.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
    act( ZM_ZAWSZE, "$n telepatycznie pokazuje ci obraz siebie.", ch, NULL, victim, TO_VICT | ACT_TELEP );

    return;
}


/*
 * Na poczatek wlasciwe polecenia socjalne.
 */
SOCJALNE( soc_ballada )
{
    /* Ciekawe, czy ktos zglosi blad, ze dla telepatii powinno dzialac :) --Lam */
    if ( !CAN_TALK( ch ) )
    {
	STC( "Nie masz jak za`spiewa`c.\n\r", ch );
	return;
    }

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Ton twego czystego g`losu wznosi si`e ku niebu.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n `spiewa pi`ekn`a ballad`e.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pod`spiewujesz sobie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n sobie pod`spiewuje.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "`spiewasz $@ pi`ekn`a ballad`e.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n `spiewa ci pi`ekn`a ballad`e, jakie`z to romantyczne!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "`spiewasz $@ pi`ekn`a ballad`e.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n `spiewa ci pi`ekn`a ballad`e, jakie`z to romantyczne!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n `spiewa $@ pi`ekn`a ballad`e, jakie`z to romantyczne!", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_blagaj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "B`lagasz bog`ow o przebaczenie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Bogowie wy`smiewaj`a b`laganie o przebaczenie dla $1.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "B`laganie siebie samego tu nie pomo`ze.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zdaje si`e do siebie co`s m`owi`c.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "usi`lujesz wyd`ebi`c troch`e z`lota od $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n usi`luje wyb`laga`c od ciebie troch`e kasy.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "usi`lujesz wyd`ebi`c troch`e z`lota od $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje wyb`laga`c od ciebie troch`e kasy.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n b`laga $3 o dofinansowanie.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_bohater )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Pozujesz na bohatera, heroicznie ryzykuj`acego swe `zycie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje zrobi`c co`s bohaterskiego, aby ci`e zadziwi`c.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Starasz si`e wygl`ada`c bohatersko.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje wygl`ada`c bohatersko.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "m`owisz $@: \"Jeste`s moim bohaterem!\"", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci: \"Jeste`s moim bohaterem!\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "m`owisz $@: \"Jeste`s moim bohaterem!\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci: \"Jeste`s moim bohaterem!\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n m`owi $@: \"Jeste`s moim bohaterem!\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_brak )
{
    if ( !CAN_TALK( ch ) )
    {
	STC( "Nie masz jak narzeka`c.\n\r", ch );
	return;
    }

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo ci tak bardzo brakuje?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Brak ci ciebie. Nie masz dla siebie w og`ole czasu.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n narzeka, `ze nie ma dla siebie ani chwili.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "m`owisz $@, jak bardzo t`esknisz i jak bardzo ci $M brakuje.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci jak bardzo $u ciebie brakuje i jak strasznie t`eskni za tob`a.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "m`owisz $@, jak bardzo t`esknisz i jak bardzo ci $M brakuje.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci jak bardzo $u ciebie brakuje i jak strasznie t`eskni za tob`a.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n m`owi $@ jak bardzo $u $M brakuje.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_brew )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Unosisz brew.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n unosi brew.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Unosisz brew nad siebie. To boli!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje podnie`s`c si`e za brew. Ale`z to musi bole`c!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "unosisz brew, patrz`ac na $# znacz`aco.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n unosi brew, patrz`ac na ciebie znacz`aco.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "unosisz brew, patrz`ac na $# znacz`aco.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n unosi brew, patrz`ac na ciebie znacz`aco.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n unosi brew, patrz`ac na $# znacz`aco.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_browar )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Otwierasz butelk`e zimnego browca.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n otwiera butelk`e zimnego browca.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Otwierasz sobie butelk`e zimnego browca.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n otwiera sobie butelk`e zimnego browca.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "otwierasz dla $B butelk`e zimnego browca.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n otwiera ci butelk`e zimnego browca.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "otwierasz dla $B butelk`e zimnego browca.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n otwiera ci butelk`e zimnego browca.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n otwiera butelk`e zimnego browca dla $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_buziak )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Chcesz kogo`s poca`lowa`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Biedny samotnik...", ch, NULL, NULL, TO_CHAR );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "ca`lujesz $V.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n ci`e ca`luje.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "ca`lujesz $V.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n ci`e ca`luje.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n ca`luje $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_cierp )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Znowu zero do`swiadczenia? Prosisz niebiosa o interwencj`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n strasznie cierpi. Chyba nie jest w stanie zdoby`c poziomu.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Cierpisz ze swojego powodu.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n cierpi ze swojego powodu i nie powie nikomu, o co chodzi.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "m`owisz $@, jak bardzo cierpisz b`ed`ac z dala od $B.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci, jak bardzo cierpi b`ed`ac z dala od ciebie.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "m`owisz $@, jak bardzo cierpisz b`ed`ac z dala od $B.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci, jak bardzo cierpi b`ed`ac z dala od ciebie.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n m`owi $@, jak bardzo cierpi b`ed`ac z dala od $B.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_czesc )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Witasz si`e ze wszystkimi wok`o`l.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wita si`e ze wszystkimi wok`o`l.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Po d`lugiej przerwie, nareszcie witasz si`e ze sob`a.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rado`snie si`e ze sob`a wita. Czy to nie dziwaczne?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "o jak dobrze si`e znowu widzie`c!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci jak si`e cieszy, `ze ci`e widzi.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "o jak dobrze si`e znowu widzie`c!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci jak si`e cieszy, `ze ci`e widzi.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n m`owi $@: \"Cze`s`c!\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_czulosc )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Lepiej by`loby wybra`c kogo`s do pary.", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Nie r`ob tego, co by powiedzieli inni?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "przytulasz si`e do $!, ca`luj`ac $V mi`ekko i czule.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n przytula si`e do ciebie, ca`luj`ac ci`e mi`ekko i czule.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "przytulasz si`e do $!, ca`luj`ac $V mi`ekko i czule.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n przytula si`e do ciebie, ca`luj`ac ci`e mi`ekko i czule.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n przytula si`e do $!, ca`luj`ac $V mi`ekko i czule.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_doping )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz zdopingowa`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Dopingujesz si`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n podnosi si`e na duchu.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "dopingujesz $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n dopinguje ci`e: \"Do boju!\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "dopingujesz $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n dopinguje ci`e: \"Do boju!\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n dopinguje $#: \"Do boju!\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_drzyj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Brrrrrrrrr.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n dr`zy, zapewne ze strachu.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Dr`zysz ze strachu przed sob`a?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n dr`zy ze strachu przed sob`a.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "dr`zysz na my`sl o walce z $$.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n dr`zy na my`sl o walce z tob`a.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "dr`zysz na my`sl o walce z $$.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n dr`zy na my`sl o walce z tob`a.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n dr`zy na my`sl o walce z $$.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_dziekuje )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Nie ma za co!", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Dzi`ekujesz sobie, bo nikt inny nie chce.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n sam$a sobie dzi`ekuje, bo ty nie chcesz.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "dzi`ekujesz $@ z g`l`ebi serca.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n dzi`ekuje ci z g`l`ebi serca.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "dzi`ekujesz $@ z g`l`ebi serca.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n dzi`ekuje ci z g`l`ebi serca.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n dzi`ekuje $@ z g`l`ebi serca.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_eskimos )
{
    if ( victim == NULL )
    {
	STC( "Z kim chcesz si`e poca`lowa`c po eskimosku?\n\r", ch );
	/* act( ZM_WZROK | ZM_WID_CHAR, "$n chce si`e z kim`s pociera`c nosami. Jacy`s ch`etni?", ch, NULL, NULL, TO_ROOM ); */
    }
    else if ( victim == ch )
    {
	STC( "Tego nie da si`e zrobi`c w pojedynk`e.\n\r", ch );
	/*
	act( ZM_ZAWSZE, "Rozmy`slasz nad problemem tarcia swojego nosa w`lasnym nosem.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rozmy`sla nad problemem tarcia swojego nosa w`lasnym nosem.", ch, NULL, NULL, TO_ROOM );
	*/
    }
    else if ( !CZY_MA_TWARZ( ch ) || !CZY_MA_TWARZ( victim ) )
    {
	STC( "Nie mo`zecie poca`lowa`c si`e po eskimosku.\n\r", ch );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "pocieracie si`e nosami.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n i ty pocieracie si`e nosami.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "pocieracie si`e nosami.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n i ty pocieracie si`e nosami.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n i $N pocieraj`a si`e nosami.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_fajka )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Komu chcesz wr`eczy`c fajk`e pokoju?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Ech, nie ma to jak fajeczka na dzie`n dobry...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zapala sobie fajk`e.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "dajesz $@ fajk`e na znak pokoju.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n daje ci fajk`e na znak pokoju. Palisz?", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "dajesz $@ fajk`e na znak pokoju.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n daje ci fajk`e na znak pokoju. Palisz?", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n daje $@ fajk`e na znak pokoju.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_glaszcz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo pog`laska`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Oj biedactwo, nie ma kto ci`e pog`laska`c?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Nie ma kto pog`laska`c $1. Mo`ze ty?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "g`laszczesz $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n g`laszcze ci`e.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "g`laszczesz $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n g`laszcze ci`e.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n g`laszcze $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_grozba )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Grozisz, `ze wybijesz wszystkich po kolei.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n si`e odgra`za.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Grozisz, `ze si`e zabijesz. Mo`ze kto`s pomo`ze?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n grozi sobie `smierci`a. Mo`ze pomo`zesz?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "w`sciekle wrzeszczysz na $#: \"Ju`z nie `zyjesz!\"", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n w`sciekle odgra`za ci si`e: \"Ju`z nie `zyjesz!\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	if ( !CAN_TALK( ch ) )
	{
	    STC( "Nie mo`zesz na nikogo nakrzycze`c.\n\r", ch );
	    return;
	}
	act_pp( ZM_ZAWSZE, "w`sciekle wrzeszczysz na $#: \"Ju`z nie `zyjesz!\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n w`sciekle odgra`za ci si`e: \"Ju`z nie `zyjesz!\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n w`sciekle odgra`za si`e $@: \"Ju`z nie `zyjesz!\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_grupka )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Komu chcesz proponowa`c grupk`e?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pr`obujesz si`e zgrupowa`c ze sob`a. Hmm...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje si`e ze sob`a zgrupowa`c. Hmm...", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "proponujesz $@ grupk`e. Ciekawe czy si`e zgodzi?", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n proponuje ci grupk`e. Co ty na to?", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "proponujesz $@ grupk`e. Ciekawe czy si`e zgodzi?", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n proponuje ci grupk`e. Co ty na to?", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n proponuje $@ grupk`e. Co on$A na to?", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_gwizd )
{
    if ( !CAN_TALK( ch ) )
    {
	STC( "Nie masz jak zagwizda`c.\n\r", ch );
	return;
    }

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Pogwizdujesz sobie niewinnie, pr`obuj`ac skry`c sw`a win`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pogwizduje sobie niewinnie.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pogwizdujesz sobie niewinnie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pogwizduje sobie niewinnie, staraj`ac si`e ukry`c swoj`a win`e.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "Pogwizdujesz sobie niewinnie, unikaj`ac odpowiedzi na pytanie $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n pogwizduje sobie niewinnie zamiast odpowiedzie`c... hmmmmm.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "Pogwizdujesz sobie niewinnie, unikaj`ac odpowiedzi na pytanie $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n pogwizduje sobie niewinnie zamiast odpowiedzie`c... hmmmmm.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n pogwizduje sobie niewinnie - oczywiste przyznanie si`e do winy.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_hmm )
{
    if ( !CAN_TALK( ch ) )
    {
	STC( "Nie masz jak wyda`c z siebie \"hmmm\".\n\r", ch );
	return;
    }

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Wydajesz z siebie g`lo`sne: \"Hmmm.\"", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n my`sli: \"Hmmmm.\"", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "G`lo`sno my`slisz.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n g`lo`sno my`sli: \"Hmmmm.\"", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "patrzysz na $# i m`owisz: \"Hmmm.\"", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n spogl`ada na ciebie i powiada: \"Hmmm.\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "patrzysz na $# i m`owisz: \"Hmmm.\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n spogl`ada na ciebie i powiada: \"Hmmm.\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n patrzy na $# i powiada: \"Hmmm.\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_hologram )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Pstrykasz palcami tworz`ac z`ludzenie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n odchodzi na po`ludnie.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pstrykasz palcami tworz`ac drugiego siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Nagle okazuje si`e `ze $n si`e rozdwoi`l$a!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "pstrykasz palcami tworz`ac z`ludzenie.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n macha ci i odchodzi na po`ludnie.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "pstrykasz palcami tworz`ac z`ludzenie.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n macha ci i odchodzi na po`ludnie.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n odchodzi na po`ludnie.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_ideal )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kto jest twoim idea`lem?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Zachwycasz si`e pi`eknem swego cia`la.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n ogl`ada si`e z wielkim zainteresowaniem.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "patrzysz $@ nami`etnie w oczy i szepczesz: \"Jeste`s moim idea`lem.\"", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n patrzy ci nami`etnie w oczy szepcz`ac: \"Jeste`s moim idea`lem.\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	if ( !CAN_TALK( ch ) )
	{
	    STC( "Nie masz jak wyrazi`c swojego podziwu.\n\r", ch );
	    return;
	}
	act_pp( ZM_ZAWSZE, "patrzysz $@ nami`etnie w oczy i szepczesz: \"Jeste`s moim idea`lem.\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n patrzy ci nami`etnie w oczy szepcz`ac: \"Jeste`s moim idea`lem.\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n patrzy nami`etnie w oczy $! szepcz`ac $U co`s do ucha.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_jezyczek )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo poca`lowa`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Obejmujesz si`e i ca`lujesz.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje si`e poca`lowa`c.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "obdarowujesz $# d`lugim i nami`etnym poca`lunkiem.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n ca`luje ci`e d`lugo i nami`etnie.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "obdarowujesz $# d`lugim i nami`etnym poca`lunkiem.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n ca`luje ci`e d`lugo i nami`etnie.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n ca`luje $# nami`etnie.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_jezyk )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Nie wystawiaj j`ezyka, bo ci krowa nasika!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wystawia j`ezyk. Ble, ohyda.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Sobie chcesz pokaza`c j`ezyk? Id`x poszukaj lustra.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n robi jakie`s dziwne miny.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "wystawiasz j`ezyk w kierunku $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n pokazuje ci sw`oj wielki j`ezyk. Chyba ci`e nie lubi.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "wystawiasz j`ezyk w kierunku $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n pokazuje ci sw`oj wielki j`ezyk. Chyba ci`e nie lubi.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n wystawia j`ezyk do $!, chyba si`e nie lubi`a.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_kac )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Po`lykasz aspiryn`e i zak`ladasz ciemne okulary. Ale`z ci`e boli g`lowa!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n `lapie si`e za g`low`e i m`owi: \"Przesta`n tak g`lo`sno oddycha`c!\"", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Uciszasz si`e. Z tym kacem nie wytrzymujesz w`lasnego g`losu.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n ucisza si`e. Z tym kacem pewnie nie mo`ze wytrzyma`c w`lasnego g`losu.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "usi`lujesz uciszy`c $#, bo p`eka ci g`lowa.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n ucisza ci`e: \"Cicho! Mam kaca!\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "usi`lujesz uciszy`c $#, bo p`eka ci g`lowa.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n ucisza ci`e: \"Cicho! Mam kaca!\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n ucisza $#: \"Cicho! Mam kaca!\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_klaszcz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Klap, klap, klap.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n klaszcze.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Klaszczesz sam$a sobie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n klaszcze sam$a sobie!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "klaszczesz na widok poczyna`n $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n klaszcze na widok twych poczyna`n!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "klaszczesz na widok poczyna`n $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n klaszcze na widok twych poczyna`n!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n klaszcze na widok poczyna`n $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_klatwa )
{
    if ( !CAN_TALK( ch ) )
    {
	STC( "Nie masz jak zakl`a`c.\n\r", ch );
	return;
    }

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Klniesz siarczy`scie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n klnie: @*&^%@*&!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Klniesz sam$a na siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zaczyna kl`a`c na siebie. Mo`ze pomo`zesz?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "klniesz na $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n klnie na ciebie! Gdzie $c maniery?", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "klniesz na $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n klnie na ciebie! Gdzie $c maniery?", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n klnie na $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_kochaj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kochasz ca`ly `swiat.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n kocha ca`ly `swiat.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Tak tak, wiemy, kochasz si`e...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n si`e kocha, kto by si`e domy`sli`l?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "wyjawiasz $@ swoje uczucia.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n szepcze ci do ucha s`lodkie s`lowa mi`lo`sci.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "wyjawiasz $@ swoje uczucia.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n szepcze ci do ucha s`lodkie s`lowa mi`lo`sci.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n szepcze co`s $@.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_kopniak )
{
    char nieprzytomnego_a_e[ MAX_STRING_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz kopn`a`c?", ch, NULL, NULL, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pr`obujesz si`e kopn`a`c.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje si`e kopn`a`c.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    if ( ch->in_room != victim->in_room
      || !can_see( ch, victim )
      || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "w`la`snie gdy $N si`e schyli`l$A, wymierzasz $U pot`e`znego kopniaka!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "kiedy si`e schylasz, nagle $n wymierza ci pot`e`znego kopniaka!", ch, NULL, victim, TO_VICT | ACT_TELEP );
	return;
    }

    if ( PP( ch ) )
    {
	act_pp( ZM_ZAWSZE, "w`la`snie gdy $N si`e schyli`l$A, wymierzasz $U pot`e`znego kopniaka!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "kiedy si`e schylasz, nagle $n wymierza ci pot`e`znego kopniaka!", ch, NULL, victim, TO_VICT );
	return;
    }

    if ( !IS_SET( race_table[ ch->race ].parts, P_NOGI ) )
    {
	act( ZM_ZAWSZE, "I tak nie masz n`og.", ch, NULL, NULL, TO_CHAR );
	return;
    }

    if ( !IS_AWAKE( victim ) )
    {
	sprintf( nieprzytomnego_a_e,
		    " nieprzytomn%s $#, kt`or$Y nic sobie z tego nie robi.",
		    victim->sex == 1
			? "ego" :
		    victim->sex == 2
			? "`a" : "e" );
	strcpy( buf, "kopiesz" );
	strcat( buf, nieprzytomnego_a_e );
	act( ZM_ZAWSZE, buf, ch, NULL, victim, TO_CHAR );
	strcpy( buf, "$n kopie" );
	strcat( buf, nieprzytomnego_a_e );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, buf, ch, NULL, victim, TO_NOTVICT );
	act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT, "$n macha nog`a, jakby chcia`l$a co`s kopn`a`c.", ch, NULL, victim, TO_NOTVICT );
	return;
    }

    act( ZM_ZAWSZE, "w`la`snie gdy $N si`e schyli`l$A, wymierzasz $U pot`e`znego kopniaka!", ch, NULL, victim, TO_CHAR );
    act( ZM_WZROK | ZM_WID_CHAR, "kiedy si`e schylasz, nagle $n wymierza ci pot`e`znego kopniaka!", ch, NULL, victim, TO_VICT );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "w`la`snie gdy $N si`e schyli`l$A, $n wymierza $U pot`e`znego kopniaka!", ch, NULL, victim, TO_NOTVICT );
    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT, "$n macha nog`a, jakby chcia`l$a co`s kopn`a`c.", ch, NULL, victim, TO_NOTVICT );

    return;
}


SOCJALNE( soc_krytykuj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Czyje zachowanie chcesz skrytykowa`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Krytykujesz swoje w`lasne zachowanie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n krytykuje swoje zachowanie.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "krytykujesz zachowanie $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n krytykuje twoje zachowanie.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "krytykujesz zachowanie $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n krytykuje twoje zachowanie.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n krytykuje zachowanie $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_kucnij )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Uff! W ostatniej chwili!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Nisko lec`acy smok prawie potr`aci`l $3.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Usi`lujesz kucn`a`c za sob`a. Au, to boli!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje za sob`a kucn`a`c. Mo`ze pomo`zesz $u si`e rozwi`aza`c?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "przykucasz za $$. Uff! W ostatniej chwili!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n przykuca za tob`a w celu unikni`ecia walki.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "przykucasz za $$. Uff! W ostatniej chwili!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n przykuca za tob`a w celu unikni`ecia walki.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n przykuca za $$, aby unikn`a`c walki.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_kwiaty )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kombinujesz, co by tu zrobi`c z tymi chwastami...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n dziwnie wymachuje bukietem kwiat`ow.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Odurzasz si`e zapachem kwiat`ow.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n odurza si`e zapachem kwiat`ow.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "wr`eczasz $U bukiet kwiat`ow.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n wr`ecza ci bukiet kwiat`ow.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "wr`eczasz $U bukiet kwiat`ow.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n wr`ecza ci bukiet kwiat`ow.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n wr`ecza $@ bukiet kwiat`ow.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_lituj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Litujesz si`e nad ca`lym `swiatem.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rozgl`ada si`e z politowaniem.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Litujesz si`e nad sob`a.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n lituje si`e nad sob`a.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "patrzysz na $# i m`owisz: \"Biedaczyno...\"", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n patrzy na ciebie i m`owi: \"Biedaczyno...\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "patrzysz na $# i m`owisz: \"Biedaczyno...\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n patrzy na ciebie i m`owi: \"Biedaczyno...\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n patrzy na $# i m`owi: \"Biedaczyno...\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_laskocz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Rozgl`adasz si`e w poszukiwaniu osoby, kt`ora ma `laskotki.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rozgl`ada si`e w poszukiwaniu osoby, kt`ora ma `laskotki.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pr`obujesz po`laskota`c sam$a siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje po`laskota`c sam$a siebie. Mo`ze $u pomo`zesz?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "rzucasz si`e na $# i zaczynasz $V `laskota`c.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n zaczyna ci`e `laskota`c! Nie wytrzymujesz i wybuchasz `smiechem.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "rzucasz si`e na $# i zaczynasz $V `laskota`c.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n zaczyna ci`e `laskota`c! Nie wytrzymujesz i wybuchasz `smiechem.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n nagle zaczyna `laskota`c $#, a $Z wybucha `smiechem!", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_marzenie )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Marzysz sobie o niebieskich migda`lach.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rozmy`sla nad czym`s.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Rozmy`slasz o swych wielkich wyczynach.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rozmy`sla nad czym`s.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "marzysz o $%.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n marzy o tobie.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "marzysz o $%.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n marzy o tobie.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n marzy o $%.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_masuj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Co masowa`c? Powietrze?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "`Cwiczysz jog`e usi`luj`ac si`e masowa`c.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n masuje si`e, daj`ac pokaz jogi.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "`lagodnie masujesz ramiona $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n `lagodnie rozmasowuje ci ramiona. Mmmmmmmmmmm...", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "`lagodnie masujesz ramiona $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n `lagodnie rozmasowuje ci ramiona. Mmmmmmmmmmm...", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n masuje ramiona $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_miziaj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz pomizia`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Chce ci si`e mizia`c! Kto ci pomo`ze?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n potrzebuje miziania!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "czule miziasz $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n mizia ci`e czule.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "czule miziasz $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n mizia ci`e czule.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n mizia $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_modlitwa )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Nagle rzucasz si`e na kolana, modl`ac si`e gor`aco.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Znienacka, $n rzuca si`e na kolana, modl`ac si`e gor`aco.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Modlisz si`e do siebie. Panowie w bia`lych kitlach ju`z czekaja...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n modli si`e do siebie. Lekarza!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "rzucasz si`e na kolana, modl`ac si`e do $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n rzuca si`e na kolana, modl`ac si`e do ciebie.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "rzucasz si`e na kolana, modl`ac si`e do $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n rzuca si`e na kolana, modl`ac si`e do ciebie.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n rzuca si`e na kolana, modl`ac si`e do $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_mrugnij )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Do kogo chcesz mrugn`a`c?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n mruga. Czy`zby co`s $u wpad`lo do oka?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Mrugasz do siebie?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje mrugn`a`c sam$a do siebie.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "weso`lo mrugasz do $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n mruga do ciebie weso`lo. Czy`zby ci`e podrywa`l$a?", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "weso`lo mrugasz do $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n mruga do ciebie weso`lo. Czy`zby ci`e podrywa`l$a?", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n weso`lo mruga do $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_mudoholik )
{
    if ( !CAN_TALK( ch ) )
    {
	STC( "Nie masz jak z`lo`zy`c tego wyznania.\n\r", ch );
	return;
    }

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Stajesz na `srodku i wyznajesz: \"Cze`s`c, mam na imi`e $n, jestem uzale`znion$y od mud`ow.\"", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n staje na `srodku i wyznaje: \"Cze`s`c, mam na imi`e $n, jestem uzale`znion$y od mud`ow.\"", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Stajesz na `srodku i wyznajesz: \"Cze`s`c, mam na imi`e $n, jestem uzale`znion$y od mud`ow.\"", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n staje na `srodku i wyznaje: \"Cze`s`c, mam na imi`e $n, jestem uzale`znion$y od mud`ow.\"", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "m`owisz $@ `ze jeste`s uzale`znion$y od mud`ow.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci, `ze jest uzale`znion$y od mud`ow.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "m`owisz $@ `ze jeste`s uzale`znion$y od mud`ow.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci, `ze jest uzale`znion$y od mud`ow.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n m`owi $@, `ze jest uzale`znion$y od mud`ow.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_muhaha )
{
    if ( !CAN_TALK( ch ) )
    {
	STC( "Nie mo`zesz si`e g`lo`sno za`smia`c.\n\r", ch );
	return;
    }

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "`Smiejesz si`e diabolicznie. MUHAHAHAHAHAHA!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n `smieje si`e diabolicznie. MUHAHAHAHAHAHA!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Muhaha na siebie? Tak si`e nie da.", ch, NULL, NULL, TO_CHAR );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "`smiejesz si`e na $# diabolicznie. MUHAHAHAHAHAHA!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n `smieje si`e na ciebie diabolicznie. MUHAHAHAHAHAHA!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "`smiejesz si`e na $# diabolicznie. MUHAHAHAHAHAHA!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n `smieje si`e na ciebie diabolicznie. MUHAHAHAHAHAHA!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n `smieje si`e na $# diabolicznie. MUHAHAHAHAHAHA!", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_niepal )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz oduczy`c palenia?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Powtarzasz sobie g`lo`sno: \"Nie pal`e, nie pal`e, nie...\"", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje sobie wm`owi`c, `ze wi`ecej ju`z nie zapali.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "przypominasz $@, `ze ma wi`ecej nie pali`c!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n przypomina ci jak bardzo zgubne jest palenie! NIE PAL!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "przypominasz $@, `ze ma wi`ecej nie pali`c!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n przypomina ci jak bardzo zgubne jest palenie! NIE PAL!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n przypomina $@ jak bardzo palenie szkodzi zdrowiu.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_noga )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz podci`a`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pr`obujesz si`e podci`a`c.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n pr`obuje si`e podci`a`c.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "nagle wystawiasz nog`e i $N upada na twarz!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "nagle $n wystawia nog`e i upadasz na twarz!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "nagle wystawiasz nog`e i $N upada na twarz!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "nagle $n wystawia nog`e i upadasz na twarz!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "nagle $n wystawia nog`e i $N upada na twarz!", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_obliz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Oblizujesz si`e ze smakiem.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n oblizuje si`e ze smakiem.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Li`zesz si`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n si`e li`ze. A fe!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "li`zesz $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n li`ze ci`e.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "li`zesz $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n li`ze ci`e.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n li`ze $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_omdlenie )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Nagle przewracasz si`e na ziemi`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n znienacka przewraca si`e na ziemi`e.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Patrzysz na sw`oj stan i mdlejesz.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n patrzy na sw`oj stan i mdleje.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "omdlewasz w $C ramiona.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n omdlewa prosto w twoje ramiona. Jakie`z to romantyczne!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "omdlewasz w $C ramiona.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n omdlewa prosto w twoje ramiona. Jakie`z to romantyczne!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n omdlewa prosto w ramiona $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_ocieraj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "O kogo chcesz si`e otrze`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Zacierasz r`ece.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zaciera r`ece.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "subtelnie ocierasz si`e o $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n subtelnie ociera si`e o ciebie.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "subtelnie ocierasz si`e o $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n subtelnie ociera si`e o ciebie.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n ociera si`e o $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_otrzyj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Ocierasz `lzy.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n ociera `lzy.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Ocierasz `lzy.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n ociera `lzy.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "ocierasz $@ `lzy.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n ociera ci `lzy.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "ocierasz $@ `lzy.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n ociera ci `lzy.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n ociera $@ `lzy.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_oswiadcz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "O`swiadczasz wszem wobec... tylko co?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n chce co`s o`swiadczy`c", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Chyba nawet ty nie chcesz si`e po`slubi`c.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n odrzuca w`lasne o`swiadczyny.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "kl`ekasz na kolanie i o`swiadczasz si`e $@.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n kl`eka na kolanie i prosi ci`e o r`ek`e.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "kl`ekasz na kolanie i o`swiadczasz si`e $@.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n kl`eka na kolanie i prosi ci`e o r`ek`e.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n kl`eka na kolanie i o`swiadcza si`e $@.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_papa )
{
    if ( !CAN_TALK( ch ) )
    {
	STC( "Nie mo`zesz powiedzie`c \"papa\".\n\r", ch );
	return;
    }

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "M`owisz \"papa\" wszystkim wok`o`l.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n m`owi \"papa\" do wszystkich wok`o`l.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "M`owisz sobie: \"Papa!\" Czy rozwa`zasz samob`ojstwo?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n m`owi \"papa\" do siebie. Czy`zby w gr`e wchodzi`lo samob`ojstwo?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "m`owisz $@: \"Papa!\"", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci: \"Papa!\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "m`owisz $@: \"Papa!\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci: \"Papa!\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n m`owi $@: \"Papa!\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_paw )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Puszczasz kolorowego pawia.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n puszcza kolorowego pawia.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Puszczasz na siebie kolorowego pawia.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n puszcza na siebie kolorowego pawia.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "puszczasz kolorowego pawia na $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n puszcza kolorowego pawia wprost na ciebie!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "puszczasz kolorowego pawia na $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n puszcza kolorowego pawia wprost na ciebie!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n puszcza kolorowego pawia na $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_ping )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Komu pos`la`c pinga?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Nie ma sensu s`la`c sobie ping`ow.", ch, NULL, NULL, TO_CHAR );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "`slesz pinga do $!. $N nie odpowiada.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n `sle do ciebie pinga. Czy twoje `l`acze jest sprawne?", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "`slesz pinga do $!. $N nie odpowiada.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n `sle do ciebie pinga. Czy twoje `l`acze jest sprawne?", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n `sle pinga do $!. $N nie odpowiada.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_placz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Buuuuu... Chlip...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zalewa si`e `lzami.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Siadasz na ziemi i cichutko pop`lakujesz.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n siada na ziemi i cichutko pop`lakuje.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "wyp`lakujesz si`e na $C ramieniu.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n wyp`lakuje si`e na twoim ramieniu.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "wyp`lakujesz si`e na $C ramieniu.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n wyp`lakuje si`e na twoim ramieniu.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n wyp`lakuje si`e na ramieniu $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_pobudka )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz zmusi`c do powrotu na ziemi`e?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Budzisz si`e zlan$y zimnym potem i z r`ek`a w nocniku...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n budzi si`e zlan$y zimnym potem i z r`ek`a w nocniku...", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "wrzeszczysz na $#, `zeby w ko`ncu si`e obudzi`l$A!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n trz`esie tob`a i wrzeszczy: \"Wstawaj! Obud`x si`e. POBUDKA!\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "wrzeszczysz na $#, `zeby w ko`ncu si`e obudzi`l$A!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n trz`esie tob`a i wrzeszczy: \"Wstawaj! Obud`x si`e. POBUDKA!\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n poszturchuje $# i wrzeszczy: \"Pobudka!\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_podziw )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Podziwiasz ca`le otoczenie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n podziwia wszystko woko`lo.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Podziwiasz sam$a siebie. Niez`l$y jeste`s.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n podziwia sam$a siebie. Narcyz.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "podziwiasz $#. Nie`xle, nie`xle.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n podziwia ci`e. Mo`zesz by`c dumn$Y!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "podziwiasz $#. Nie`xle, nie`xle.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n podziwia ci`e. Mo`zesz by`c dumn$Y!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n podziwia $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_pogroz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Komu chcesz pogrozi`c palcem?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Grozisz sobie palcem.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n grozi sobie palcem.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "grozisz $@ palcem.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n grozi ci palcem.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "grozisz $@ palcem.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n grozi ci palcem.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n grozi $@ palcem.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_policzkuj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Rozgl`adasz si`e po pomieszczeniu. Kogo by tu spoliczkowa`c?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n bacznie przygl`ada si`e wszystkim obecnym w pomieszczeniu.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Policzkujesz sam$a siebie. Lubisz zadawa`c sobie b`ol?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n klepie si`e w twarz.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "policzkujesz $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n ci`e policzkuje. Nale`za`lo ci si`e.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "policzkujesz $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n ci`e policzkuje. Nale`za`lo ci si`e.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n policzkuje $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_przepros )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "przepraszasz za swoje zachowanie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przeprasza za swoje zachowanie.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "przepraszasz si`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n si`e przeprasza. Hmmm...", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "przepraszasz $V.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n ci`e przeprasza.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "przepraszasz $V.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n ci`e przeprasza.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n przeprasza $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_prztyczek )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Komu chcesz da`c prztyczka w nos?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	if ( !CZY_MA_TWARZ( ch ) )
	{
	    STC( "Nie mo`zesz da`c sobie prztyczka.\n\r", ch );
	}
	else
	{
	    act( ZM_ZAWSZE, "Dajesz sobie prztyczka w nos!", ch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n daje sobie prztyczka w nos!", ch, NULL, NULL, TO_ROOM );
	}
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	if ( !CZY_MA_TWARZ( victim ) )
	{
	    act( ZM_ZAWSZE, "Nie mo`zesz da`c $U prztyczka w nos.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	}
	else
	{
	    telepatyczne_obrazy( ch, victim );
	    act( ZM_ZAWSZE, /*"Z oddali, "*/ "dajesz $@ prztyczka w nos!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	    act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n daje ci prztyczka w nos!", ch, NULL, victim, TO_VICT | ACT_TELEP );
	}
    }
    else if ( !CZY_MA_TWARZ( victim ) )
    {
	    act_pp( ZM_ZAWSZE, "Nie mo`zesz da`c $U prztyczka w nos.", ch, NULL, victim, TO_CHAR );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "dajesz $@ prztyczka w nos!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n daje ci prztyczka w nos!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n daje $@ prztyczka w nos!", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_przytaknij )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Przytakujesz.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przytakuje.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Przytakujesz sam$a sobie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n kiwa g`low`a, potwierdzaj`ac swoje s`lowa.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "kiwasz g`low`a i przyznajesz $@ racj`e.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n kiwa g`low`a i przyznaje ci racj`e.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "kiwasz g`low`a i przyznajesz $@ racj`e.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n kiwa g`low`a i przyznaje ci racj`e.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n kiwa g`low`a i przytakuje $@.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_przytul )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz przytuli`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Twoje r`ece spotykaj`a si`e na plecach...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przytula si`e sam$a do siebie... biedactwo.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "tulisz $# do siebie tak delikatnie i czule, jak tylko potrafisz.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n tuli ci`e do siebie tak delikatnie i czule, jak tylko potrafi.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "tulisz $# do siebie tak delikatnie i czule, jak tylko potrafisz.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n tuli ci`e do siebie tak delikatnie i czule, jak tylko potrafi.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n tuli do siebie $# delikatnie i czule.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_pukaj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Pukasz si`e w czo`lo.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n puka si`e w czo`lo.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pukasz si`e w czo`lo.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n puka si`e w czo`lo.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "patrzysz na poczynania $! i z niedowierzaniem pukasz si`e w czo`lo.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n patrzy na ciebie znacz`aco i puka si`e w czo`lo.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "patrzysz na poczynania $! i z niedowierzaniem pukasz si`e w czo`lo.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n patrzy na ciebie znacz`aco i puka si`e w czo`lo.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n puka si`e w czo`lo patrz`ac znacz`aco na $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_racja )
{
    if ( !CAN_TALK( ch ) )
    {
	STC( "Nie mo`zesz nikomu przyzna`c racji wystarczaj`aco dobitnie.\n\r", ch );
	return;
    }

    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Komu chcesz przyzna`c absolutn`a racj`e?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Wmawiasz sobie, `ze masz racj`e. \"Tak, JA WIEM LEPIEJ!\"", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wmawia sobie, `ze ma absolutn`a racj`e.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "m`owisz $@, `ze ma absolutn`a racj`e.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci: \"Tak, oczywi`scie, jasne, pewnie, absolutnie, w rzeczy samej!\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "m`owisz $@, `ze ma absolutn`a racj`e.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci: \"Tak, oczywi`scie, jasne, pewnie, absolutnie, w rzeczy samej!\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n m`owi $@, `ze ma absolutn`a racj`e.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_reka )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo poca`lowa`c w r`ek`e?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Ca`lujesz swoj`a d`lo`n.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n ca`luje w`lasn`a d`lo`n.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "ca`lujesz $# w r`ek`e.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n ca`luje ci`e w r`ek`e.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "ca`lujesz $# w r`ek`e.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n ca`luje ci`e w r`ek`e.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n ca`luje r`ek`e $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_rumieniec )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Rumienisz si`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n si`e rumieni.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Rumienisz si`e, ale ci g`lupio.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rumieni si`e, wspominaj`ac swoje s`lowa.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "rumienisz si`e, s`luchaj`ac s`l`ow $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n rumieni si`e pod wp`lywem twoich s`l`ow.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "rumienisz si`e, s`luchaj`ac s`l`ow $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n rumieni si`e pod wp`lywem twoich s`l`ow.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n rumieni si`e, s`luchaj`ac s`l`ow $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_salutuj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Stajesz na baczno`s`c i salutujesz.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n staje na baczno`s`c i salutuje.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Salutujesz sobie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nagle staje na baczno`s`c i salutuje nie wiadomo, komu.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "stajesz na baczno`s`c i salutujesz $@.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n staje przed tob`a na baczno`s`c i salutuje.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "stajesz na baczno`s`c i salutujesz $@.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n staje przed tob`a na baczno`s`c i salutuje.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n staje na baczno`s`c i salutuje $@.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_skrec )
{
    if ( victim == NULL )
    {
	if ( CZY_ZIEMIA( ch->in_room ) )
	{
	    act( ZM_ZAWSZE, "Padasz na pod`log`e skr`ecaj`ac si`e ze `smiechu.", ch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n pada na pod`log`e skr`ecaj`ac si`e ze `smiechu.", ch, NULL, NULL, TO_ROOM );
	}
	else
	{
	    act( ZM_ZAWSZE, "Skr`ecasz si`e ze `smiechu.", ch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n skr`eca si`e ze `smiechu.", ch, NULL, NULL, TO_ROOM );
	}
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Padasz na pod`log`e `smiej`ac si`e z siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n `smieje si`e z siebie. Mo`ze si`e przy`l`aczysz?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "skr`ecasz si`e ze `smiechu z powodu $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n skr`eca si`e ze `smiechu z twojego powodu.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "skr`ecasz si`e ze `smiechu z powodu $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n skr`eca si`e ze `smiechu z twojego powodu.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n skr`eca si`e ze `smiechu z powodu $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_sluchaj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Nas`luchujesz uwa`znie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n uwa`znie nas`luchuje.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Dok`ladnie ws`luchujesz si`e w swe s`lowa, aby unikn`a`c pope`lnienia gafy.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n ws`luchuje si`e w sw`oj g`los.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "uwa`znie s`luchasz s`l`ow $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n uwa`znie s`lucha twych s`l`ow.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "uwa`znie s`luchasz s`l`ow $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n uwa`znie s`lucha twych s`l`ow.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n uwa`znie s`lucha s`l`ow $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_smutek )
{
    if ( victim == NULL )
    {
	if ( CZY_MA_TWARZ( ch ) )
	{
	    act( ZM_ZAWSZE, "Smutno poci`agasz nosem.", ch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n smutno poci`aga nosem.", ch, NULL, NULL, TO_ROOM );
	}
	else
	{
	    act( ZM_ZAWSZE, "Smucisz si`e.", ch, NULL, NULL, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n wyra`xnie si`e czym`s smuci.", ch, NULL, NULL, TO_ROOM );
	}
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Zrobi`l$o`s co`s, przez co jeste`s bardzo smutn$y?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zrobi`l$a co`s, przez co bardzo si`e smuci.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "m`owisz $U, `ze jeste`s bardzo smutn$y z powodu $C zachowania.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci, jak bardzo si`e smuci z twojego powodu.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "m`owisz $U, `ze jeste`s bardzo smutn$y z powodu $C zachowania.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci, jak bardzo si`e smuci z twojego powodu.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n m`owi $@, jak bardzo si`e smuci z $C powodu.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_sraport )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "{YPodajesz: 12874/13103 `z 9238/10230 m 2483/3451 r 2.31E13 do`sw.{x", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "{Y$n podaje: 12874/13103 `z 9238/10230 m 2483/3451 r 2.31E13 do`sw.{x", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "{YPodajesz: 12874/13103 `z 9238/10230 m 2483/3451 r 2.31E13 do`sw.{x", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "{Y$n podaje: 12874/13103 `z 9238/10230 m 2483/3451 r 2.31E13 do`sw.{x", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "{Ypodajesz: 12874/13103 `z 9238/10230 m 2483/3451 r 2.31E13 do`sw.{x", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "{Y$n podaje: 12874/13103 `z 9238/10230 m 2483/3451 r 2.31E13 do`sw.{x", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "{Ypodajesz: 12874/13103 `z 9238/10230 m 2483/3451 r 2.31E13 do`sw.{x", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "{Y$n podaje: 12874/13103 `z 9238/10230 m 2483/3451 r 2.31E13 do`sw.{x", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "{Y$n podaje: 12874/13103 `z 9238/10230 m 2483/3451 r 2.31E13 do`sw.{x", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_szturchnij )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo masz ochot`e szturchn`a`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pr`obujesz szturchn`a`c sam$a siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n szarpie si`e za rami`e.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "bierzesz zamach i mocno szturchasz $# w rami`e.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n szturcha ci`e w rami`e. Ocknij si`e!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "bierzesz zamach i mocno szturchasz $# w rami`e.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n szturcha ci`e w rami`e. Ocknij si`e!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n mocno szturcha $# w rami`e.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_sciemniaj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Komu chcesz `sciemnia`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Oszukujesz sam$a siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n co`s sobie wmawia.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "m`owisz $U: \"Eee, aaa, yyy, to nie moja wina... Tak jako`s wysz`lo...\"", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci: \"Eee, aaa, yyy, to nie moja wina... Tak jako`s wysz`lo...\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "m`owisz $U: \"Eee, aaa, yyy, to nie moja wina... Tak jako`s wysz`lo...\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci: \"Eee, aaa, yyy, to nie moja wina... Tak jako`s wysz`lo...\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n w`la`snie `sciemnia $@.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_smiech )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "`Smiejesz si`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n `smieje si`e.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "`Smiejesz si`e z siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n `smieje si`e z siebie. Mo`ze si`e przy`l`aczysz?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "wy`smiewasz $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n na`smiewa si`e z ciebie. Wrr...", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "wy`smiewasz $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n na`smiewa si`e z ciebie. Wrr...", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n wy`smiewa $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_taniec )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Ta`nczysz, robi`ac z siebie idiot`e.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n usi`luje ta`nczy`c, ale niezbyt $u to wychodzi.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Udajesz `ze kto`s z tob`a ta`nczy, ale i tak nikt nie uwierzy.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n ta`nczy z wyimaginowanym partnerem.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "porywasz $V do romantycznego walca.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n porywa ci`e do romantycznego walca.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "porywasz $V do romantycznego walca.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n porywa ci`e do romantycznego walca.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n porywa $# do romantycznego walca.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_toast )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Na czyj`a cze`s`c chcesz wznie`s`c toast?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Opijasz w`lasne zdrowie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wzosi toast na w`lasn`a cze`s`c. Hmm...", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "wznosisz toast na $C cze`s`c.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n wznosi toast na twoj`a cze`s`c.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "wznosisz toast na $C cze`s`c.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n wznosi toast na twoj`a cze`s`c.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n wznosi toast na cze`s`c $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_udawaj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Udajesz, `ze masz nadludzk`a moc i zabijasz wszystkich pierwszym ci`eciem!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n znowu udaje, `ze ma jak`a`s nadludzk`a moc. `Zenada...", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Sprowadzasz si`e do poziomu pierwszego... a jak teraz wr`ocisz?", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n sw`a MOC`A sprowadza si`e do poziomu pierwszego... a jak teraz wr`oci?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "udajesz, `ze masz bosk`a moc i sprowadzasz $# do poziomu pierwszego!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n udaje, `ze ma bosk`a moc i sprowadza ci`e do poziomu pierwszego!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "udajesz, `ze masz bosk`a moc i sprowadzasz $# do poziomu pierwszego!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n udaje, `ze ma bosk`a moc i sprowadza ci`e do poziomu pierwszego!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n udaje, `ze ma bosk`a moc i sprowadza $# do poziomu pierwszego!", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_uklon )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Zginasz si`e wp`o`l.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zgina si`e wp`o`l.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Ca`lujesz swoje stopy.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n sk`lada si`e jak scyzoryk ca`luj`ac swe stopy.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "k`laniasz si`e przed $$.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n k`lania ci si`e.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "k`laniasz si`e przed $$.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n k`lania ci si`e.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n k`lania si`e $@.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_ukolysz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz uko`lysa`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Ko`lyszesz si`e do snu.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n ko`lysze si`e do snu. Mo`ze za`spiewaj $u ko`lysank`e?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "bierzesz $# w ramiona i delikatnie ko`lyszesz $V do snu.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n bierze ci`e w ramiona, przytula i ko`lysze do snu.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "bierzesz $# w ramiona i delikatnie ko`lyszesz $V do snu.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n bierze ci`e w ramiona, przytula i ko`lysze do snu.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n bierze $# w ramiona i delikatnie ko`lysze.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_unies )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz unie`s`c do g`ory?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pr`obujesz wzi`a`c si`e na r`ece, lecz nie za bardzo ci to wychodzi.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n chcia`l$aby si`e wzi`a`c na r`ece. `LUBUDU!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "bierzesz $# na r`ece, unosisz do g`ory i obracasz si`e kilkakrotnie.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n niespodziewanie bierze ci`e na r`ece i unosi do g`ory.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else if ( WP( ch ) )
    {
	if ( !IS_SET( race_table[ ch->race ].parts, P_RECE ) )
	    send_to_char( "I tak nie masz r`ak.\n\r", ch );
	else if ( victim->carry_weight + race_table[ victim->race ].weight > can_carry_w( ch ) )
	    send_to_char( "Nie ud`xwigniesz takiego ci`e`zaru.\n\r", ch );
	else
	{
	    act( ZM_ZAWSZE, "bierzesz $# na r`ece, unosisz do g`ory i obracasz si`e kilkakrotnie.", ch, NULL, victim, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n niespodziewanie bierze ci`e na r`ece i unosi do g`ory.", ch, NULL, victim, TO_VICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n bierze na r`ece $#, unosi do g`ory i obraca kilkakrotnie.", ch, NULL, victim, TO_NOTVICT );
	}
    }
    else
    {
	act_pp( ZM_ZAWSZE, "bierzesz $# na r`ece, unosisz do g`ory i obracasz si`e kilkakrotnie.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n niespodziewanie bierze ci`e na r`ece i unosi do g`ory.", ch, NULL, victim, TO_VICT );
    }

    return;
}


SOCJALNE( soc_usteczka )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo poca`lowa`c w usta?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Ca`lujesz nami`etnie swoje odbicie w lustrze.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n ca`luje nami`etnie swoje odbicie w lustrze. Hmmmm.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "bierzesz $# w swoje ramiona ca`luj`ac $V s`lodko w usta.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n bierze ci`e w swoje ramiona i ca`luje s`lodko w usta.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "bierzesz $# w swoje ramiona ca`luj`ac $V s`lodko w usta.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n bierze ci`e w swoje ramiona i ca`luje s`lodko w usta.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n bierze $# w swoje ramiona ca`luj`ac $V s`lodko w usta.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_uscisk )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo u`scisn`a`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Nie mo`zesz przytuli`c si`e do siebie.", ch, NULL, NULL, TO_CHAR );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "przytulasz si`e do $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n przytula si`e do ciebie.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "przytulasz si`e do $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n przytula si`e do ciebie.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n przytula si`e do $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_usmiech )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "u`smiechasz si`e szeroko.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n u`smiecha si`e szeroko.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "U`smiechasz si`e do siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n u`smiecha si`e do siebie.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "u`smiechasz si`e do $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n u`smiecha si`e do ciebie.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "u`smiechasz si`e do $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n u`smiecha si`e do ciebie.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	{
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n u`smiecha si`e do $!.", ch, NULL, victim, TO_NOTVICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT, "$n si`e u`smiecha.", ch, NULL, victim, TO_NOTVICT );
	}
    }

    return;
}


SOCJALNE( soc_warcz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Wydajesz z siebie g`lo`sne warkni`ecie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n warczy.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Warczysz na siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n warczy.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "g`lo`sno warczysz na $#.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n warczy na ciebie! Lepiej uwa`zaj!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "g`lo`sno warczysz na $#.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n warczy na ciebie! Lepiej uwa`zaj!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n warczy na $#.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_wazelina )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Komu chcesz si`e podliza`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Zastanawiasz si`e, jak by si`e sobie bardziej podliza`c...", ch, NULL, NULL, TO_CHAR );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "m`owisz $@, `ze jest siln$Y, m`adr$Y, pi`ekn$Y i wspania`l$Y.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n m`owi ci, `ze jeste`s siln$Y, m`adr$Y, pi`ekn$Y i wspania`l$Y.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "m`owisz $@, `ze jest siln$Y, m`adr$Y, pi`ekn$Y i wspania`l$Y.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n m`owi ci, `ze jeste`s siln$Y, m`adr$Y, pi`ekn$Y i wspania`l$Y.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n w`la`snie podlizuje si`e $@.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_wlm )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Zapowiadasz sw`oj rych`ly powr`ot.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zapowiada, `ze wraca lada moment.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Wmawiasz sobie, `ze zaraz wr`ocisz.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "S`lyszysz, jak $n wmawia sobie, `ze zaraz wr`oci.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "oznajmiasz $@, `ze wracasz lada moment.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n oznajmia ci, `ze wraca lada moment.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "oznajmiasz $@, `ze wracasz lada moment.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n oznajmia ci, `ze wraca lada moment.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n oznajmia $@, `ze wraca lada moment.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_wskaz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Na kogo wskaza`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Wskazujesz palcem siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wskazuje palcem siebie.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "wskazujesz na $# palcem.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n wskazuje na ciebie palcem.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "wskazujesz na $# palcem.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n wskazuje na ciebie palcem.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n wskazuje na $# palcem.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_wtul )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "W kogo chcesz si`e wtuli`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Pr`obujesz wtuli`c si`e w siebie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n obejmuje si`e r`ekami i lekko ko`lysze.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "delikatnie wtulasz si`e w ramiona $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n delikatnie wtula si`e w twoje ramiona.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "delikatnie wtulasz si`e w ramiona $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n delikatnie wtula si`e w twoje ramiona.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n wtula si`e w ramiona $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_wykrzyw )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Wykrzywiasz twarz w grymasie obrzydzenia.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wykrzywia twarz w grymasie obrzydzenia.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Wykrzywiasz twarz w grymasie obrzydzenia sob`a.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wykrzywia twarz w grymasie obrzydzenia sob`a.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "wykrzywiasz twarz w grymasie obrzydzenia na widok $!.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n wykrzywia twarz w grymasie obrzydzenia na tw`oj widok.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "wykrzywiasz twarz w grymasie obrzydzenia na widok $!.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n wykrzywia twarz w grymasie obrzydzenia na tw`oj widok.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n wykrzywia twarz w grymasie obrzydzenia na widok $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_wypij )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Pytasz si`e wszystkich wok`o`l, czy kto`s si`e z tob`a napije.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Kto si`e napije z$e $4?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Nikt nie chce si`e z tob`a napi`c? Napij si`e sam$a!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "Biedn$y $n. Do czego to dosz`lo? `Zeby pi`c w samotno`sci?", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "pytasz $#, czy si`e z tob`a napije.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n pyta ci`e, czy nie masz ochoty si`e napi`c.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "pytasz $#, czy si`e z tob`a napije.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n pyta ci`e, czy nie masz ochoty si`e napi`c.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n pyta $#, czy si`e razem napij`a.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_wyzwij )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Szukasz kogo`s ch`etnego do pojedynku...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n szuka kogo`s ch`etnego do pojedynku...", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Wyzywasz si`e od najgorszych!", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n wyzywa si`e od najgorszych!", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "rzucasz $@ r`ekawic`a w twarz, wrzeszcz`ac: \"Stawaj do walki tch`orzu!\"", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n rzuca ci r`ekawic`a w twarz, wrzeszcz`ac: \"Stawaj do walki tch`orzu!\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "rzucasz $@ r`ekawic`a w twarz, wrzeszcz`ac: \"Stawaj do walki tch`orzu!\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n rzuca ci r`ekawic`a w twarz, wrzeszcz`ac: \"Stawaj do walki tch`orzu!\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n rzuca $@ r`ekawic`a w twarz, wrzeszcz`ac: \"Stawaj do walki tch`orzu!\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_zawod )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Komu chcesz powiedzie`c, jak bardzo zawiod`l$o`s?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Przepraszasz sam$a siebie, `ze tak mog`l$o`s da`c cia`la...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n przyznaje, `ze da`l$a cia`la jak nigdy...", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "ze `lzami w oczach przepraszasz $# za to, jak bardzo $V zawiod`l$o`s.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n ze `lzami w oczach przeprasza ci`e za wielki zaw`od, jaki sprawi`l$a.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "ze `lzami w oczach przepraszasz $# za to, jak bardzo $V zawiod`l$o`s.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n ze `lzami w oczach przeprasza ci`e za wielki zaw`od, jaki sprawi`l$a.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n przeprasza $# za zaw`od jaki $U sprawi`l$a.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_zgadula )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Kogo chcesz zaskoczy`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Zakrywasz sobie oczy i m`owisz do siebie. Do`s`c dziwnie to wygl`ada.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zakrywa sobie oczy i m`owi do siebie: \"Zgadnij kto to!\" Biedn$y.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	STC( "Nie mo`zesz tego zrobi`c na odleg`lo`s`c.\n\r", ch );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "podchodzisz na paluszkach do $!, zakrywasz $U oczy i m`owisz: \"Zgadnij kto to!\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "kto`s nagle zas`lania ci oczy i m`owi: \"Zgadnij kto to!\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n podchodzi do $!, zas`lania $U oczy i m`owi: \"Zgadnij kto to!\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_zgniec )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Rozgniatasz jakiego`s robaka.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n rozdeptuje jakie`s robactwo.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Niby w jaki spos`ob chcesz si`e zgnie`s`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "podnosisz nog`e i rozgniatasz $# jak robaka!", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n podnosi nog`e i rozgniata ci`e jak robaka!", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "podnosisz nog`e i rozgniatasz $# jak robaka!", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n podnosi nog`e i rozgniata ci`e jak robaka!", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n podnosi nog`e i rozgniata $# jak robaka!", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_zaprzecz )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Czyim s`lowom chcesz zaprzeczy`c?", ch, NULL, NULL, TO_CHAR );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Zaprzeczasz sam$a sobie.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n zaprzecza sam$a sobie.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "zaprzeczasz temu, co w`la`snie powiedzia`l$A $N.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n zaprzecza twoim s`lowom.", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "zaprzeczasz temu, co w`la`snie powiedzia`l$A $N.", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n zaprzecza twoim s`lowom.", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n zaprzecza s`lowom $!.", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


SOCJALNE( soc_ziewaj )
{
    if ( victim == NULL )
    {
	act( ZM_ZAWSZE, "Buueee, ale tu nudno.", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n strasznie ziewa. Chyba nudzi si`e okropnie.", ch, NULL, NULL, TO_ROOM );
    }
    else if ( victim == ch )
    {
	act( ZM_ZAWSZE, "Nudzisz si`e...", ch, NULL, NULL, TO_CHAR );
	act( ZM_WZROK | ZM_WID_CHAR, "$n nudzi si`e...", ch, NULL, NULL, TO_ROOM );
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	telepatyczne_obrazy( ch, victim );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "ziewasz $U prosto w twarz m`owi`ac: \"Ale`s ty nudn$Y.\"", ch, NULL, victim, TO_CHAR | ACT_TELEP );
	act( ZM_ZAWSZE, /*"Z oddali, "*/ "$n ziewa ci prosto w twarz m`owi`ac: \"Ale`s ty nudn$Y.\"", ch, NULL, victim, TO_VICT | ACT_TELEP );
    }
    else
    {
	act_pp( ZM_ZAWSZE, "ziewasz $U prosto w twarz m`owi`ac: \"Ale`s ty nudn$Y.\"", ch, NULL, victim, TO_CHAR );
	act_pp( ZM_WZROK | ZM_WID_CHAR, "$n ziewa ci prosto w twarz m`owi`ac: \"Ale`s ty nudn$Y.\"", ch, NULL, victim, TO_VICT );
	if ( WP( ch ) )
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n ziewa $@ prosto w twarz m`owi`ac: \"Ale`s ty nudn$Y.\"", ch, NULL, victim, TO_NOTVICT );
    }

    return;
}


/*
 * Lam 4.6.2005: typy polecen socjalnych decydujace o sposobie reakcji mobow na
 * nie. Klasyfikacja polecen do grup - Gerino
 */
#define SOC_OFENSYWNE	1
#define SOC_MILE	2
#define SOC_MILE_PLEC	3
#define SOC_NEUTRALNE	4
#define	SOC_USMIECH	5

/*
 * tablica
 */
struct {
    char * const	name;
    SOC_FUN *		fun;
    int			typ;
    char * const	opis;
} social_table[ ] =
{
    { "ballada",	soc_ballada,	SOC_USMIECH,
			"`Spiewanie ballad." },
    { "b`lagaj",	soc_blagaj,	SOC_OFENSYWNE,
			"B`laganie bog`ow o przebaczenie, b`laganie graczy o z`loto." },
    { "bohater",	soc_bohater,	SOC_USMIECH,
			"Pozowanie na bohatera, m`owienie innym, `ze s`a bohaterscy." },
    { "brak",		soc_brak,	SOC_MILE_PLEC,
			"M`owienie komu`s, jak bardzo ci go brakuje." },
    { "brew",		soc_brew,	SOC_NEUTRALNE,
			"Uniesienie brwi." },
    { "browar",		soc_browar,	SOC_MILE,
			"Otwarcie dla kogo`s piwka." },
    { "buziak",		soc_buziak,	SOC_MILE_PLEC,
			"Poca`lunek." },
    { "cierp",		soc_cierp,	SOC_MILE_PLEC,
			"Cierpienie z braku do`swiadczenia lub innej postaci." },
    { "cze`s`c",	soc_czesc,	SOC_MILE,
			"Witanie si`e z przyjaci`o`lmi." },
    { "czu`lo`s`c",	soc_czulosc,	SOC_MILE_PLEC,
			"Czu`le pieszczoty." },
    { "doping",		soc_doping,	SOC_USMIECH,
			"Dopingowanie postaci." },
    { "dr`zyj",		soc_drzyj,	SOC_NEUTRALNE,
			"Dr`zenie ze strachu na my`sl o walce z postaciami." },
    { "dzi`ekuj`e",	soc_dziekuje,	SOC_USMIECH,
			"Serdeczne podzi`ekowania postaciom." },
    { "eskimos",	soc_eskimos,	SOC_MILE_PLEC,
			"Pocieranie si`e nosami." },
    { "fajka",		soc_fajka,	SOC_MILE,
			"Wr`eczanie fajki na znak pokoju." },
    { "g`laszcz",	soc_glaszcz,	SOC_MILE_PLEC,
			"G`laskanie postaci." },
    { "gro`xba",	soc_grozba,	SOC_OFENSYWNE,
			"Gro`zenie wrogom." },
    { "grupka",		soc_grupka,	SOC_NEUTRALNE,
			"Proponowanie grupki." },
    { "gwizd",		soc_gwizd,	SOC_NEUTRALNE,
			"Niewinne pogwizdywanie." },
    { "hmm",		soc_hmm,	SOC_NEUTRALNE,
			"Po prostu: \"Hmmm.\"" },
    { "hologram",	soc_hologram,	SOC_NEUTRALNE,
			"Hologram odchodz`acy na po`ludnie." },
    { "idea`l",		soc_ideal,	SOC_MILE_PLEC,
			"Wyznanie, kto jest twoim idea`lem." },
    { "j`ezyczek",	soc_jezyczek,	SOC_MILE_PLEC,
			"Nami`etny poca`lunek z j`ezyczkiem." },
    { "j`ezyk",		soc_jezyk,	SOC_OFENSYWNE,
			"Wystawienie j`ezyka do kogo`s nielubianego." },
    { "kac",		soc_kac,	SOC_NEUTRALNE,
			"Uciszanie postaci, kiedy masz kaca." },
    { "klaszcz",	soc_klaszcz,	SOC_USMIECH,
			"Oklaski." },
    { "kl`atwa",	soc_klatwa,	SOC_OFENSYWNE,
			"Kl`atwa, \"rzucanie mi`esem\"." },
    { "kochaj",		soc_kochaj,	SOC_MILE_PLEC,
			"Wyznawanie mi`lo`sci." },
    { "kopniak",	soc_kopniak,	SOC_OFENSYWNE,
			"Kopniak w zadek." },
    { "krytykuj",	soc_krytykuj,	SOC_OFENSYWNE,
			"Krytyka czyjego`s zachowania." },
    { "kucnij",		soc_kucnij,	SOC_OFENSYWNE,
			"Przykucanie (za kim`s), aby unikn`a`c walki/nisko lec`acego smoka." },
    { "kwiaty",		soc_kwiaty,	SOC_MILE_PLEC,
			"Wr`eczanie bukietu kwiat`ow." },
    { "lituj",		soc_lituj,	SOC_NEUTRALNE,
			"Litowanie si`e nad pokrzywdzonymi." },
    { "`laskocz",	soc_laskocz,	SOC_MILE_PLEC,
			"`Laskotki!" },
    { "marzenie",	soc_marzenie,	SOC_MILE_PLEC,
			"Marzenie o niebieskich migda`lach lub konkretnych postaciach." },
    { "masuj",		soc_masuj,	SOC_MILE_PLEC,
			"Masowanie ramion postaci." },
    { "miziaj",		soc_miziaj,	SOC_MILE_PLEC,
			"Mizianie postaci." },
    { "modlitwa",	soc_modlitwa,	SOC_USMIECH,
			"`Zarliwe mod`ly do bog`ow lub postaci." },
    { "mrugnij",	soc_mrugnij,	SOC_MILE_PLEC,
			"Zaczepne mruganie, spos`ob na podryw." },
    { "mudoholik",	soc_mudoholik,	SOC_NEUTRALNE,
			"Wyznanie uzale`znienia od mud`ow." },
    { "muhaha",		soc_muhaha,	SOC_OFENSYWNE,
			"Diaboliczny `smiech." },
    { "niepal",		soc_niepal,	SOC_USMIECH,
			"Przypominanie palaczom, jak zgubne jest palenie." },
    { "noga",		soc_noga,	SOC_OFENSYWNE,
			"Podcinanie postaci nog`a." },
    { "obli`z",		soc_obliz,	SOC_MILE_PLEC,
			"Oblizanie si`e ze smakiem, lizanie postaci." },
    { "omdlenie",	soc_omdlenie,	SOC_MILE_PLEC,
			"Romantyczne omdlenie w czyje`s ramiona." },
    { "ocieraj",	soc_ocieraj,	SOC_MILE_PLEC,
			"Ocieranie si`e o postacie, zacieranie r`ak." },
    { "otrzyj",		soc_otrzyj,	SOC_USMIECH,
			"Otarcie `lez beksom." },
    { "o`swiadcz",	soc_oswiadcz,	SOC_MILE_PLEC,
			"O`swiadczyny." },
    { "papa",		soc_papa,	SOC_MILE,
			"Po`zegnanie ze zgromadzonymi." },
    { "paw",		soc_paw,	SOC_OFENSYWNE,
			"Puszczanie pawia (w znaczeniu przeno`snym)." },
    { "ping",		soc_ping,	SOC_NEUTRALNE,
			"\"Pingowanie\" postaci." },
    { "p`lacz",		soc_placz,	SOC_USMIECH,
			"Wyp`lakiwanie si`e." },
    { "pobudka",	soc_pobudka,	SOC_NEUTRALNE,
			"Budzenie postaci." },
    { "podziw",		soc_podziw,	SOC_USMIECH,
			"Podziwianie `swiata i innych postaci." },
    { "pogro`x",	soc_pogroz,	SOC_OFENSYWNE,
			"Gro`xba palcem." },
    { "policzkuj",	soc_policzkuj,	SOC_OFENSYWNE,
			"Policzkowanie bezczelnych postaci." },
    { "przepro`s",	soc_przepros,	SOC_USMIECH,
			"Przeprosiny." },
    { "prztyczek",	soc_prztyczek,	SOC_OFENSYWNE,
			"Prztyczek w nos." },
    { "przytaknij",	soc_przytaknij,	SOC_NEUTRALNE,
			"Przyznanie komu`s racji." },
    { "przytul",	soc_przytul,	SOC_MILE_PLEC,
			"Delikatne przytulenie." },
    { "pukaj",		soc_pukaj,	SOC_OFENSYWNE,
			"Wymowne pukanie si`e w czo`lo." },
    { "racja",		soc_racja,	SOC_USMIECH,
			"Przyznawanie absolutnej racji." },
    { "r`eka",		soc_reka,	SOC_USMIECH,
			"Poca`lunek w r`ek`e." },
    { "rumieniec",	soc_rumieniec,	SOC_NEUTRALNE,
			"Rumienienie si`e." },
    { "salutuj",	soc_salutuj,	SOC_USMIECH,
			"Salutowanie." },
    { "skr`e`c",	soc_skrec,	SOC_OFENSYWNE,
			"Skr`ecanie si`e ze `smiechu." },
    { "s`luchaj",	soc_sluchaj,	SOC_NEUTRALNE,
			"Nas`luchiwanie, uwa`zne s`luchanie czyich`s s`l`ow." },
    { "smutek",		soc_smutek,	SOC_NEUTRALNE,
			"Wyra`zenie smutku z powodu czyjego`s zachowania." },
    { "sraport",	soc_sraport,	SOC_NEUTRALNE,
			"Sfingowany, sfa`lszowany, sztuczny raport." },
    { "szturchnij",	soc_szturchnij,	SOC_NEUTRALNE,
			"Szturchanie postaci w rami`e." },
    { "`sciemniaj",	soc_sciemniaj,	SOC_NEUTRALNE,
			"`Sciemnianie, `ze \"to nie twoja wina\"." },
    { "`smiech",	soc_smiech,	SOC_OFENSYWNE,
			"`Smiech, na`smiewanie si`e z kogo`s." },
    { "taniec",		soc_taniec,	SOC_MILE_PLEC,
			"Taniec, romantyczny walc z inn`a postaci`a." },
    { "toast",		soc_toast,	SOC_MILE,
			"Wzniesienie toastu na czyj`a`s cze`s`c." },
    { "udawaj",		soc_udawaj,	SOC_OFENSYWNE,
			"Udawanie boskiej mocy." },
    { "uk`lon",		soc_uklon,	SOC_MILE,
			"Uk`lony, k`lanianie si`e postaciom." },
    { "uko`lysz",	soc_ukolysz,	SOC_USMIECH,
			"Uko`lysanie kogo`s do snu." },
    { "unie`s",		soc_unies,	SOC_MILE_PLEC,
			"Uniesienie ofiary na r`ekach." },
    { "usteczka",	soc_usteczka,	SOC_MILE_PLEC,
			"S`lodki poca`lunek w usta." },
    { "u`scisk",	soc_uscisk,	SOC_MILE_PLEC,
			"Przytulanie si`e." },
    { "u`smiech",	soc_usmiech,	SOC_MILE,
			"Szczere u`smiechy." },
    { "warcz",		soc_warcz,	SOC_OFENSYWNE,
			"Warczenie pod nosem i na inne postacie." },
    { "wazelina",	soc_wazelina,	SOC_USMIECH,
			"Podlizywanie si`e innym." },
    { "wlm",		soc_wlm,	SOC_NEUTRALNE,
			"\"Wracam lada moment.\"" },
    { "wska`z",		soc_wskaz,	SOC_NEUTRALNE,
			"Wskazanie kogo`s palcem." },
    { "wtul",		soc_wtul,	SOC_MILE_PLEC,
			"Wtulenie si`e w czyje`s ramiona." },
    { "wykrzyw",	soc_wykrzyw,	SOC_OFENSYWNE,
			"Wykrzywienie si`e w grymasie obrzydzenia (na czyj`s widok)." },
    { "wypij",		soc_wypij,	SOC_MILE,
			"Poszukiwania kompan`ow do pijatyki." },
    { "wyzwij",		soc_wyzwij,	SOC_OFENSYWNE,
			"Wyzywanie wrog`ow na pojedynek." },
    { "zaw`od",		soc_zawod,	SOC_NEUTRALNE,
			"Przeprosiny za to, jaki sprawiasz zaw`od." },
    { "zgadula",	soc_zgadula,	SOC_MILE_PLEC,
			"Zakrycie komu`s oczu m`owi`ac: \"Zgadnij kto to!\"" },
    { "zgnie`c",	soc_zgniec,	SOC_OFENSYWNE,
			"Rozgniatanie robak`ow." },
    { "zaprzecz",	soc_zaprzecz,	SOC_NEUTRALNE,
			"Zaprzeczanie s`lowom innych." },
    { "ziewaj",		soc_ziewaj,	SOC_OFENSYWNE,
			"Ziewanie z powodu nudy." },
    { "",		NULL,		SOC_NEUTRALNE,
			NULL }
};


bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        cmd;
    bool       found;

    found = FALSE;
    for ( cmd = 0; social_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
    {
	if ( !str_prefix( command, social_table[ cmd ].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
	return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
    {
	ASTC( "Jeste`s aspo`leczn$y!", ch );
	return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	STC( "Le`z spokojnie, przecie`z nie `zyjesz!\n\r", ch );
	return TRUE;

    case POS_MORTAL:
    case POS_INCAP:
	ASTC( "Jeste`s tak rann$y, `ze nie jeste`s w stanie tego zrobi`c.", ch );
	return TRUE;

    case POS_STUNNED:
	ASTC( "Jeste`s na to zbyt poharatan$y.", ch );
	return TRUE;

    case POS_SLEEPING:
	send_to_char( "Podczas snu?\n\r", ch );
	return TRUE;

    }

    /* Lam 7.5.98 */
    if ( IS_AFFECTED( ch, AFF_HIDE ) )
    {
	send_to_char( "Przestajesz si`e ukrywa`c.\n\r", ch );
	REMOVE_BIT_V( ch->affected_by, AFF_HIDE );
    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[ 0 ] == '\0' )
    {
	if ( PP( ch ) )
	    STC( "Nie mo`zesz tego robi`c poza postaci`a.\n\r", ch );
	else
	    ( *social_table[ cmd ].fun )( ch, NULL );
	return TRUE;
    }
    else if ( !( victim = get_char_world_pc_tele( ch, arg ) ) )
    {
	send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	return TRUE;
    }
    else if ( victim == ch )
    {
	if ( PP( ch ) )
	    STC( "Nie mo`zesz tego robi`c poza postaci`a.\n\r", ch );
	else
	    ( *social_table[ cmd ].fun )( ch, ch );
	return TRUE;
    }
    else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) || !can_see( victim, ch ) )
    {
	/* Lam: sprawdzam tylko WP( ), bo PP i tak nie moga byc w roznych
	   pomieszczeniach */
	if ( !IS_NPC( ch ) && !IS_NPC( victim ) && WP( ch ) && WP( victim ) )
	{
	    if ( is_name2( ch->name, victim->pcdata->ignore ) )
	    {
		act_pp( ZM_ZAWSZE, "$N nie `zyczy sobie, aby`s $U si`e narzuca`l$a.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
		return TRUE;
	    }

	    if ( ch->pcdata->learned[ gsn_telepathy ] < number_percent( ) )
	    {
		STC( "Nie uda`lo ci si`e porozumie`c telepatycznie.\n\r", ch );
		return TRUE;
	    }

	    if ( !IS_AWAKE( victim ) )
	    {
		act( ZM_ZAWSZE, "$Z nie mo`ze zauwa`zy`c twoich dzia`la`n.", ch, NULL, victim, TO_CHAR | ACT_TELEP );
		return TRUE;
	    }

	    ( *social_table[ cmd ].fun )( ch, victim );
	}
	else if ( ch->in_room != victim->in_room || !can_see( ch, victim ) )
	    send_to_char( "Nie ma tu takiej postaci.\n\r", ch );
	else
	    act( ZM_ZAWSZE, "$Z nie mo`ze zauwa`zy`c twoich dzia`la`n.", ch, NULL, victim, TO_CHAR );

	return TRUE;
    }

    if ( ( WP( ch ) && PP( victim ) )
      || ( PP( ch ) && WP( victim ) ) )
    {
	STC( "Nie ma tu takiej postaci.\n\r", ch );
	return TRUE;
    }

    if ( !IS_NPC( victim ) && is_name2( ch->name, victim->pcdata->ignore ) )
    {
	act_pp( ZM_ZAWSZE, "$N nie `zyczy sobie, aby`s $U si`e narzuca`l$a.", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

    ( *social_table[ cmd ].fun )( ch, victim );

    if ( IS_NPC( ch )
      || !IS_NPC( victim )
      || IS_AFFECTED( victim, AFF_CHARM )
      || !IS_AWAKE( victim ) )
    {
	return TRUE;
    }

    /* jesli ma choc jednego soc_proga, nigdy nie odpowiada automatem */
    if ( victim->pIndexData->progtypes & M_SOC_PROG )
    {
	mprog_soc_trigger( social_table[ cmd ].name, victim, ch );
	return TRUE;
    }

    /* Lam: zrobilem test "musi byc przynajmniej humanoidalny" (na wypadek np.
       wampirow P_HS | P_KIEL), do tego musza umiec mowic, bo niektore socjalne
       emuluja mowe. Prawidlowe byloby okreslanie, ktore polecenia socjalne
       wymagaja jakiej inteligencji, aby zrozumiec i odpowiedziec, a takze,
       jakich czesci ciala */
    if ( race_table[ victim->race ].parts < P_HS
      || IS_MUTE( victim )
      || !can_see( victim, ch ) )
    {
	return TRUE;
    }

    switch ( social_table[ cmd ].typ )
    {
	default:
	    bug( "nieznany typ polecenia socjalnego.", 0 );
	    break;
	case SOC_OFENSYWNE:
	    if ( victim->level > ch->level + 10 )
	    {
		act( ZM_ZAWSZE, "Spogl`adasz na $# z politowaniem i oszcz`edzasz $U `zycie.", victim, NULL, ch, TO_CHAR );
		act( ZM_WZROK | ZM_WID_CHAR, "$n spogl`ada na ciebie z politowaniem i oszcz`edza ci `zycie.", victim, NULL, ch, TO_VICT );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n spogl`ada na $# z politowaniem i oszcz`edza $U `zycie.", victim, NULL, ch, TO_NOTVICT );
		break;
	    }
	    if ( !victim->fighting
	      && !is_safe( victim, ch, FALSE )
	      && !is_safe( ch, victim, FALSE ) )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
	    }
	    else if ( ch->sex + victim->sex == 3 )
	    {
		act( ZM_ZAWSZE, "Policzkujesz $#.", victim, NULL, ch, TO_CHAR );
		act( ZM_PRZYT, "$n ci`e policzkuje.", victim, NULL, ch, TO_VICT );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n policzkuje $#.", victim, NULL, ch, TO_NOTVICT );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT, "$n macha r`ek`a.", victim, NULL, ch, TO_NOTVICT );
	    }
	    else
	    {
		act( ZM_ZAWSZE, "Odpychasz $# mocnym uderzeniem.", victim, NULL, ch, TO_CHAR );
		act( ZM_PRZYT, "$n odpycha ci`e mocnym uderzeniem.", victim, NULL, ch, TO_VICT );
		act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n odpycha $# mocnym uderzeniem.", victim, NULL, ch, TO_NOTVICT );
	    }

	    break;
	case SOC_MILE_PLEC:
	    if ( ch->race != victim->race
	      || ch->sex + victim->sex != 3 )
	    {
		if ( ch->sex + victim->sex == 3 )
		{
		    act( ZM_ZAWSZE, "Policzkujesz $#.", victim, NULL, ch, TO_CHAR );
		    act( ZM_PRZYT, "$n ci`e policzkuje.", victim, NULL, ch, TO_VICT );
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n policzkuje $#.", victim, NULL, ch, TO_NOTVICT );
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT, "$n macha r`ek`a.", victim, NULL, ch, TO_NOTVICT );
		}
		else
		{
		    act( ZM_ZAWSZE, "Odpychasz $# mocnym uderzeniem.", victim, NULL, ch, TO_CHAR );
		    act( ZM_PRZYT, "$n odpycha ci`e mocnym uderzeniem.", victim, NULL, ch, TO_VICT );
		    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n odpycha $# mocnym uderzeniem.", victim, NULL, ch, TO_NOTVICT );
		}
		break;
	    }
	    /* jesli tu doszlismy, dla danej osoby to mile */
	case SOC_MILE:
	    ( *social_table[ cmd ].fun )( victim, ch );
	    break;
	case SOC_USMIECH:
	    act( ZM_ZAWSZE, "U`smiechasz si`e do $!.", victim, NULL, ch, TO_CHAR );
	    act( ZM_WZROK | ZM_WID_CHAR, "$n u`smiecha si`e do ciebie.", victim, NULL, ch, TO_VICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_WID_VICT, "$n u`smiecha si`e do $!.", victim, NULL, ch, TO_NOTVICT );
	    act( ZM_WZROK | ZM_WID_CHAR | ZM_W_WID_VICT, "$n si`e u`smiecha.", victim, NULL, ch, TO_NOTVICT );
	    break;
	case SOC_NEUTRALNE:
	    break;
    }

    return TRUE;
}


KOMENDA( do_socials )
{
    char buf[ MAX_STRING_LENGTH ];
    char buf1[ MAX_STRING_LENGTH * 4 ];
    int  iSocial;
    int  col;
    bool opis = ( *argument && !str_prefix( argument, "opis" ) );

    buf1[ 0 ] = '\0';
    col = 0;
    for ( iSocial = 0; social_table[ iSocial ].name[ 0 ] != '\0'; iSocial++ )
    {
	sprintf( buf, "%s", wyrownaj( social_table[ iSocial ].name, -12 ) );
	strcat( buf1, buf );
	if ( opis )
	{
	    strcat( buf1, social_table[ iSocial ].opis );
	    strcat( buf1, "\n\r" );
	}
	else if ( ++col % 6 == 0 )
	    strcat( buf1, "\n\r" );
    }

    if ( !opis )
    {
	if ( col % 6 != 0 )
	    strcat( buf1, "\n\r" );
	strcat( buf1, "Wpisz \"socjalne opis\", aby zobaczy`c opis dzia`lania ka`zdego z polece`n.\n\r" );
    }

    send_to_char( buf1, ch );

    return;
}
