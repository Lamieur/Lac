* $Id: egzorc.prg,v 1.1 2006/05/31 20:10:35 lam Exp $

>greet_prog 100~
if not cansee($n)
or isnpc($n)
  break
endif
usm $n
'Witam ci`e, $NW.
if queststage($n,11103)==1
or queststage($n,11103)==2
or queststage($n,11103)==5
  pow $n Widz`e, `ze przybywasz z daleka... Masz mo`ze jak`a`s paczk`e dla mnie? Je`zeli tak, to daj mi j`a.
endif
~
>give_prog #11133~
if not cansee($n)
  'Hej, kto mi tu jak`a`s paczk`e daje?
  upu $o
  break
endif
if not doingquest($n,11103)
  daj $o $n
  pow $n A po co mi to?
  break
endif
if queststage($n,11103)==1
or queststage($n,11103)==2
or queststage($n,11103)==5
  mpjunk $o
  pow $n Nareszcie dotar`la do mnie ta przesy`lka. Nie wiem, sk`ad ci`e wytrzasn`a`l m`oj znajomy, ale ciesz`e si`e, `ze nie musz`e ju`z czeka`c. Teraz id`x na nawiedzony cmentarz znajduj`acy si`e niedaleko Nydogrodu. Od tamtejszego grabarza przynie`s mi `xr`od`lo `swiat`la.
  usm $n
  pow $n W ka`zdej chwili mog`e ci powt`orzy`c, co masz zrobi`c.
  mpquest $n 11103 6
  break
endif
daj $o $n
pow $n No, dzi`eki ci $NW!
~
>give_prog #10860~
if not cansee($n)
  'Hej, co mi tu dajesz ktosiu?
  upu $o
  break
endif
if not doingquest($n,11103)
  daj $o $n
  pow $n A po co mi to?
  break
endif
if queststage($n,11103)==7
or queststage($n,11103)==8
  zal $o
  pow $n W porz`adku, $NW. Id`x teraz do Miden'niru i poszukaj karczmy. Gdy j`a znajdziesz, daj ten list polecaj`acy karczmarzowi. Je`zeli go zgubisz, popro`s mnie o pomoc. Zawsze mog`e te`z powt`orzy`c.
  mpquest $n 11103 9
  mpquestdata $n 11103 0 0
  mpoload 29970 1
  daj #29970 $n
  break
endif
daj $o $n
pow $n Nie, ta latarnia nie jest mi w tej chwili potrzebna. Ale dziekuj`e ci za fatyg`e.
~
>tell_prog powt`orz~
if not cansee($n)
  break
endif
if not doingquest($n,11103)
  pow $n Co powt`orzy`c?
  break
endif
if queststage($n,11103)==6
or queststage($n,11103)==7
or queststage($n,11103)==8
  pow $n Od grabarza z nawiedzonego cmentarza przynie`s mi `xr`od`lo `swiat`la.
  break
endif
if queststage( $n, 11103 ) == 9 && questdata0( $n, 11103 ) == 0
  pow $n Dostarcz list polecaj`acy, ktr`ory ci da`lem, do karczmarza w Miden'nirze.
  break
endif
pow $n Co mam powt`orzy`c?
~
>tell_prog pom`o`z pomagaj pomocy pomoc~
if not cansee( $n )
  break;
endif
if queststage($n,11103)==9 && questdata0( $n, 11103 ) == 0
  if sex( $n ) == 1
    pow $n Je`zeli zgubi`le`s list polecaj`acy, powiedz, `ze zawiod`le`s.
  endif
  if sex( $n ) == 2
    pow $n Je`zeli zgubi`la`s list polecaj`acy, powiedz, `ze zawiod`la`s.
  endif
endif
~
>tell_prog zawiod`lem zawiod`lam~
if not cansee( $n )
  break;
endif
if queststage($n,11103)==9 && questdata0( $n, 11103 ) == 0
  pow $n No trudno, dam ci jeszcze jeden list. Ale tym razem postaraj si`e go nie zgubi`c!
  mpoload 29970 1
  daj #29970 $n
endif
~
|