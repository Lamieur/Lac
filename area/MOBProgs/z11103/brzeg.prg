* $Id: brzeg.prg,v 1.13 2006/05/27 16:21:11 lam Exp $

>time_prog 100~
if isobjhere(18008)
  mpecho wzrok Statek rozwija `zagle i wyp`lywa z przystani.
  mppurge statek
endif
if varexists("do_odplyniecia")
  if isobjhere(3182) && do_odplyniecia>0
    mpecho wzrok `Zagl`owka odbija od kei i wyp`lywa na morze.
    mppurge #3182
    aint do_odplyniecia=do_odplyniecia-1
  endif
endif
if rand(66)
  if not varexists("zaglowka")
  or zaglowka==0
    break
  endif
  mpoload 3182
  mpecho wzrok `Zagl`owka wp`lywa do przystani.
  aint zaglowka=zaglowka-1
endif
if rand(10) && isobjhere( 3182 )
  mpecho wzrok Kapitan zaczyna si`e krz`ata`c na `zagl`owce.
  if not varexists( "do_odplyniecia" )
    aint do_odplyniecia = 1
  else
    aint do_odplyniecia = do_odplyniecia + 1
  endif
endif
~
>do_prog o wsi`ad`x~
if number($o)!=3182
  mpechoat $n zawsze Nie mo`zesz do tego wsi`a`s`c!
  break
endif
mpechoat $n zawsze Wsiadasz na `zagl`owk`e.
mpechoaround $n wzrok+wid_n $N wsiada na `zagl`owk`e.
if not doingquest($n,11103)
or queststage($n,11103)!=11 && queststage($n,11103)!=13
  if sex($n)==2
    mpechoat $n przyt {TKapitan `zagl`owki m`owi ci: "Przepraszam, a pani w jakiej sprawie?!"{x
  else
    mpechoat $n przyt {TKapitan `zagl`owki m`owi ci: "Przepraszam, a pan w jakiej sprawie?!"{x
  endif
  mpechoat $n wzrok Kapitan `zagl`owki wypycha ci`e z `l`odki i wyciera dok`ladnie pozostawione przez ciebie `slady.
  mpechoat $n przyt+w_wzrok Kto`s wypycha ci`e sk`ad`s.
  mpechoaround $n wzrok+wid_n Kapitan `zagl`owki wypycha $ND z `l`odki.
  break
endif
mpechoat $n wzrok Kapitan stawia `zagle i wyp`lywa z portu.
mpechoat $n wzrok Fala przelewa si`e przez burt`e.
if sex($n)==2
  mpechoat $n przyt Jeste`s ca`la mokra!
else
  mpechoat $n przyt Jeste`s ca`ly mokry!
endif
mpechoat $n wzrok Kapitan sprawnie dobija do pomostu.
mpechoat $n zawsze {TKapitan `zagl`owki m`owi ci: "No ju`z, koniec podr`o`zy, musz`e zrobi`c tu porz`adek."{x
if queststage($n,11103)==13
  mpechoat $n zawsze {TKapitan `zagl`owki m`owi ci: "Jak tylko uzupe`lni`e zapasy to wracam. Je`sli si`e pospieszysz, to ci`e zabior`e ze sob`a."{x
else
  mpechoat $n zawsze {TKapitan `zagl`owki m`owi ci: "Dzi`ekuj`e za wsp`olny rejs."{x
endif
mpechoat $n wzrok Kapitan `zagl`owki wypycha ci`e z `l`odki.
mpechoat $n przyt+w_wzrok Kto`s wypycha ci`e sk`ad`s.
if queststage($n,11103)==11
  mptransfer $n 17759
  mppurge #3182
  mpat 17759 mpoload 3182
  mpat 17759 mpforce $n sp
  mpat 17759 mpquest $n 11103 12
  mpat 17759 mpquestdata $n 11103 2 0
  mpat 17759 mpechoaround $n wzrok `Zagl`owka przybija do pomostu.
  mpat 17759 mpechoaround $n wzrok+wid_n Kapitan `zagl`owki wypycha $NB z `l`odki.
else
  mptransfer $n 239
  mppurge #3182
  mpat 239 mpoload 3182
  mpat 239 mpforce $n sp
  mpat 239 mpquest $n 11103 14
  mpat 239 mpechoaround $n wzrok `Zagl`owka przybija do pomostu.
  mpat 239 mpechoaround $n wzrok+wid_n Kapitan `zagl`owki wypycha $NB z `l`odki.
endif
~
|
