* $Id: zebr.prg,v 1.9 2006/09/19 21:52:20 skifr Exp $

>greet_prog 30~
if isnpc ( $n )
  break
endif
mpechoat $n wzrok+wid_i $I rzuca si`e na kolana wprost pod twoje nogi.
if goldamt( $i ) < 10
  if sex( $n ) == 2
    md $n Nie ma pani mo`ze 10 monet?
  else
    md $n Nie ma pan mo`ze 10 monet?
  endif
  break
endif
md $n Daj na bu`lk`e, daj na bu`lk`e...
~
>greet_prog 50~
if number( $n ) == 3143
  'O, burmistrz!
  ,wyci`aga jajko z r`ekawa.
  ,rzuca jajkiem w burmistrza!
endif
~
>bribe_prog 0~
if not cansee( $n )
  'Pieni`adze znik`ad! Ojej.
  break
else
  if inroom( $i ) != 3044
    break
  endif
  if goldamt( $i ) > 10 && rand( 20 )
    md $n Wieeelkie dzi`eki!
    ,odchodzi na wsch`od.
    mpdontcomeback
    mptransfer sie 3010
    ,nadchodzi z po`ludnia.
    mptrigger sie 3 w0
    break
  endif
  if goldamt( $i ) > 5 && rand( 20 )
    md $n Wieeelkie dzi`eki!
    ,odchodzi na wsch`od.
    mpdontcomeback
    mptransfer sie 3009
    ,nadchodzi z po`ludnia.
    mptrigger sie 3 p0
  endif
endif
~
>trig_prog w0~
kup buk`lak
mptrigger sie 2 w1
~
>trig_prog w1~
,odchodzi na po`ludnie.
mptransfer sie 3044
mpcomeback
,nadchodzi z zachodu.
mptrigger sie 2 w2
~
>trig_prog w2~
pij buk`lak
mpjunk buk`lak
~
>trig_prog p0~
kup bu`lka
mptrigger sie 2 p1
~
>trig_prog p1~
,odchodzi na po`ludnie.
mptransfer sie 3044
mpcomeback
,nadchodzi z zachodu.
mptrigger sie 2 p2
~
>trig_prog p2~
jedz bu`lka
mpjunk bu`lka
~
|
