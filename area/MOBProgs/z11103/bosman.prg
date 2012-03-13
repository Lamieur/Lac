* $Id: bosman.prg,v 1.12 2006/05/27 16:21:11 lam Exp $

>greet_prog 100~
if doingquest( $n, 11103 )
  usm $n
endif
~
>tell_prog 'pozdrowienia od `zo`lnierza' 'pozdrawiam od `zo`lnierza' 'pozdrawia ci`e `zo`lnierz' '`zo`lnierz ci`e pozdrawia'~
mptrigger sie 0 pozdro
~
>tell_prog 'od `zo`lnierza'~
if varexists( "tylko_pozdrowil" )
  if tylko_pozdrowil == 1
    int tylko_pozdrowil = 0
    mptrigger sie 0 pozdro
  endif
endif
~
>tell_prog od~
if varexists( "tylko_pozdrowil" )
  if tylko_pozdrowil == 1
    int tylko_pozdrowil = 0
  endif
endif
pow $n Nie znam, ale dzi`ekuj`e.
~
>trig_prog pozdro~
if deleted( $n )
  break
endif
if not doingquest( $n, 11103 )
or queststage( $n, 11103 ) != 1 && queststage( $n, 11103 ) != 3
  pow $n A dzi`ekuj`e, dzi`ekuj`e.
  break
endif
if sex( $n ) == 1
  pow $n Dzi`ekuj`e bardzo. Jak b`edziesz si`e widzia`l z `zo`lnierzem, to przeka`z mu r`ownie`z moje pozdrowienia.
else
  pow $n Dzi`ekuj`e bardzo. Jak b`edziesz si`e widzia`la z `zo`lnierzem, to przeka`z mu r`ownie`z moje pozdrowienia.
endif
if questdata1( $n, 11103 ) == 1
  break
else
  if varexists( "opoznienie_statku" )
    if opoznienie_statku == 1
      pow $n Niestety tw`oj statek przyb`edzie z niewielkim op`o`xnieniem... Przepraszam, poczekaj jeszcze troch`e.
      mpechoat $n wzrok+wid_i Bosman wydaje si`e by`c nieco zmieszany.
    endif
  endif
endif
if queststage( $n,11103 ) == 1
  pow $n S`lysza`lem, `ze `zo`lnierz potrzebuje dostarczy`c jak`a`s paczk`e w okolice Midgaardu.
else
  pow $n S`lysza`lem, `ze `zo`lnierz potrzebuje jakiej`s paczki z Vergardu.
endif
if hour( ) >= 0 && hour( ) < 6
  pow $n Je`sli potrzebujesz si`e tam dosta`c, to poczekaj do rana i wsi`ad`x na statek, kt`ory tu przyp`lynie. On ci`e tam zabierze.
else
  if hour( ) >= 6 && hour( ) < 12
    pow $n Je`sli potrzebujesz si`e tam dosta`c, to poczekaj do po`ludnia i wsi`ad`x na statek, kt`ory tu przyp`lynie. On ci`e tam zabierze.
  else
    if hour( ) >= 12 && hour( ) < 18
      pow $n Je`sli potrzebujesz si`e tam dosta`c, to poczekaj do wieczora i wsi`ad`x na statek, kt`ory tu przyp`lynie. On ci`e tam zabierze.
    else
      pow $n Je`sli potrzebujesz si`e tam dosta`c, to poczekaj do p`o`lnocy i wsi`ad`x na statek, kt`ory tu przyp`lynie. On ci`e tam zabierze.
    endif
  endif
endif
if not varexists( "ile_statkow" )
  aint ile_statkow = 0
endif
aint ile_statkow = ile_statkow + 1
mpquestdata $n 11103 1 1
~
>tell_prog p pozdrowienia~
pow $n Ooo... a od kogo?
if doingquest( $n, 11103 )
  int tylko_pozdrowil = 1
endif
~
>soc_prog usmiech~
if not cansee( $n )
  break
endif
usm $n~
|
