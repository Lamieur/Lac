* $Id: przystan.prg,v 1.11 2006/05/27 16:21:11 lam Exp $

>time_prog 100~
if varexists( "statek_ma_odplynac" )
  if statek_ma_odplynac > 0 && hour( ) == 14
    mpecho wzrok Statek odbija od brzegu, rozwija `zagle i wyp`lywa na pe`lne morze.
    mppurge statek
  endif
endif
if hour( ) == 12
  if isobjhere( 18008 )
    mpecho wzrok Za`ladunek na statku zdaje si`e dobiega`c ko`nca.
    if varexists( "statek_ma_odplynac" )
      aint statek_ma_odplynac = statek_ma_odplynac + 1
    else
      aint statek_ma_odplynac = 1
    endif
  endif
endif
~
>do_prog o wsi`ad`x~
if not cansee($n)
  mpechoat $n zawsze No co`s ty? Jak chcesz si`e pobawi`c, to si`e poka`z!
  break
endif
if number($o)==18008
  if not doingquest($n,11103)
  or queststage($n,11103)!=4 && queststage($n,11103)!=5
    mpechoat $n wzrok Stary `zeglarz na trapie spogl`ada na ciebie gro`xnie.
    mpechoat $n zawsze {TStary `zeglarz m`owi ci: "A ty gdzie si`e pchasz, szczurze l`adowy?!"{x
    break
  endif
  mpechoat $n zawsze Wsiadasz na statek.
  mpechoaround $n wzrok+wid_n $N wsiada na statek.
  mpecho wzrok Statek odbija od brzegu, rozwija `zagle i wyp`lywa na pe`lne morze.
  mpechoat $n przyt Podr`o`z trwa kr`otko. Statek przybija do brzegu.
  mpechoat $n przyt T`lum wysiadaj`acych porywa ci`e ze sob`a.
  mptransfer $n 3049
  mppurge statek
  mpat 3049 mpoload 18008
  mpat 3049 mpforce $n sp
  mpat 3049 mpquest $n 11103 2
  mpat 3049 mpquestdata $n 11103 1 0
  mpat 3049 mpechoaround $n wzrok Statek przybija do brzegu.
  mpat 3049 mpechoaround $n wzrok+wid_n $N wysiada ze statku.
  break
endif
if number($o)==3182
  mpechoat $n zawsze Wsiadasz na `zagl`owk`e.
  mpechoaround $n wzrok+wid_n $N wsiada na `zagl`owk`e.
  if not doingquest($n,11103)
  or queststage($n,11103)!=14
    if sex($n)==2
      mpechoat $n zawsze {TKapitan `zagl`owki m`owi ci: "Przepraszam, a pani w jakiej sprawie?!"{x
    else
      mpechoat $n zawsze {TKapitan `zagl`owki m`owi ci: "Przepraszam, a pan w jakiej sprawie?!"{x
    endif
    mpechoat $n wzrok Kapitan `zagl`owki wypycha ci`e z `l`odki, po czym dok`ladnie wyciera pozostawione przez ciebie `slady.
    mpechoat $n przyt+w_wzrok Kto`s wypycha ci`e sk`ad`s.
    mpechoaround $n wzrok+wid_n Kapitan `zagl`owki wypycha $NB z `l`odki.
    break
  endif
  mpechoat $n wzrok Kapitan stawia `zagle i wychodzi z portu.
  mpechoat $n wzrok Fala przelewa si`e przez burt`e.
  mpechoat $n sluch S`lyszysz plusk wody.
  if sex($n)==2
    mpechoat $n przyt Jeste`s ca`la mokra!
  else
    mpechoat $n przyt Jeste`s ca`ly mokry!
  endif
  mpechoat $n wzrok Kapitan sprawnie dobija do pomostu.
  mpechoat $n zawsze {TKapitan `zagl`owki m`owi ci: "To ju`z koniec podr`o`zy. Musz`e umy`c pok`lad. Dzi`ekuj`e za wsp`olny rejs."{x
  mpechoat $n wzrok Kapitan `zagl`owki wypycha ci`e z `l`odki.
  mpechoat $n przyt+w_wzrok Kto`s wypycha ci`e sk`ad`s.
  mptransfer $n 3049
  mppurge `zagl`owka
  mpat 3049 mpoload 3182
  mpat 3049 mpforce $n sp
  mpat 3049 mpquestdata $n 11103 2 0
  mpat 3049 mptrigger emerytowany 0 ustaw_do_odplyniecia
  mpat 3049 mpechoaround $n wzrok `Zagl`owka przybija do pomostu.
  mpat 3049 mpechoaround $n wzrok+wid_n Kapitan `zagl`owki wypycha $NB z `l`odki.
  break
endif
mpechoat $n zawsze Nie mo`zesz do tego wsi`a`s`c!
~
|
