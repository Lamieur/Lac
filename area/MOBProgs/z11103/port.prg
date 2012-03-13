* $Id: port.prg,v 1.11 2006/05/27 16:21:11 lam Exp $

>do_prog o wsi`ad`x~
if not cansee( $n )
  mpechoat $n zawsze No co`s ty? Jak chcesz si`e pobawi`c, wy`l`acz bosk`a niewidk`e!
  break
endif
if number( $o ) != 18008
  mpechoat $n zawsze Nie mo`zesz do tego wsi`a`s`c!
  break
endif
if not doingquest( $n, 11103 )
or queststage( $n, 11103 ) != 1 && queststage( $n, 11103 ) != 3
  mpechoat $n wzrok Stary `zeglarz na trapie spogl`ada na ciebie gro`xnie.
  mpechoat $n zawsze {TStary `zeglarz m`owi ci: "A ty gdzie si`e pchasz, szczurze l`adowy?!"{x
  break
endif
mpechoat $n zawsze Wsiadasz na statek.
mpechoaround $n wzrok+wid_n $N wsiada na statek.
mpecho wzrok Statek odbija od brzegu, rozwija `zagle i wyp`lywa na pe`lne morze.
mpechoat $n zawsze Podr`o`z trwa kr`otko. Statek przybija do brzegu.
if queststage( $n, 11103 ) == 3
  mpechoat $n zawsze {TKapitan statku m`owi ci: "Je`sli si`e szybko uwiniesz to b`edziemy mogli ci`e zabra`c w dalszy rejs do Midgaardu."{x
else
  mpechoat $n zawsze {TKapitan statku m`owi ci: "Mam nadziej`e, `ze podr`o`z by`la przyjemna."{x
endif
mpechoat $n zawsze T`lum wysiadaj`acych porywa ci`e ze sob`a.
if queststage( $n, 11103 ) == 1
  mptransfer $n 3049
  mppurge statek
  mpat 3049 mpoload 18008
  mpat 3049 mpforce $n sp
  mpat 3049 mpquest $n 11103 2
  mpat 3049 mpquestdata $n 11103 1 0
  mpat 3049 mpechoaround $n wzrok Statek przybija do brzegu.
  mpat 3049 mpechoaround $n wzrok+wid_n $N wysiada ze statku.
else
  mptransfer $n 239
  mppurge statek
  mpat 239 mpoload 18008
  mpat 239 mpforce $n sp
  mpat 239 mpquest $n 11103 4
  mpat 239 mpechoaround $n wzrok Statek przybija do brzegu.
  mpat 239 mpechoaround $n wzrok+wid_n $N wysiada ze statku.
endif
~
>time_prog 100~
if isobjhere( 3182 )
  mpecho wzrok `Zagl`owka odbija od kei i wychodzi w morze.
  mppurge #3182
endif
if isobjhere( 18008 )
  if hour( ) == 0
  or hour( ) == 6
  or hour( ) == 12
  or hour( ) == 18
    mpecho wzrok Statek odbija od brzegu, rozwija `zagle i wyp`lywa na pe`lne morze.
    mppurge statek
  endif
endif
if not varexists( "ile_statkow" )
or ile_statkow == 0
  break
endif
if not varexists( "opoznienie_statku" )
  aint opoznienie_statku = 0
endif
if hour( ) == 0
or hour( ) == 6
or hour( ) == 12
or hour( ) == 18
or opoznienie_statku == 1
  mpoload 18008
  mpecho wzrok Statek przybija do brzegu.
  aint ile_statkow = ile_statkow - 1
  if ile_statkow > 0
    aint opoznienie_statku = 1
  else
    aint opoznienie_statku = 0
  endif
endif
~
|
