* $Id: zolnierz.prg,v 1.17 2006/07/25 19:42:20 lam Exp $

>greet_prog 100~
if not cansee($n)
or isnpc($n)
  break
endif
if !doingquest($n, 11103)
  ,szuka kogo`s do wykonania zadania...
  mpechoat $n wzrok+wid_i `Zo`lnierz patrzy na ciebie.
  hmm $n
  pow $n Mam wa`zn`a pro`sb`e! Je`zeli dostarczysz pewn`a paczk`e egzorcy`scie z Nydogrodu le`z`acego w Leanderze, a tak`ze zrobisz to, co on ci naka`ze, otrzymasz nagrod`e. Je`sli chcesz to zadanie, powiedz mi o tym.
  break
endif
if queststage($n,11103) == 17
  usm $n
*  'To zadanie zosta`lo ju`z przez ciebie wykonane, $NW.
  break
endif
~
>tell_prog zadanie~
if not cansee($n)
  'Nie b`ed`e gada`l z tch`orzami, boj`acymi si`e pokaza`c!
  break
endif
if not doingquest($n,11103)
  if isfollow($n)
    'Z tch`orzami nie rozmawiam!
    'Przesta`n si`e chowa`c za plecami...
    break
  endif
  pow $n S`luchaj, $NW. Potrzebna mi jest twoja pomoc. Mam piln`a paczk`e dla egzorcysty z Nydogrodu le`z`acego w Leanderze. Sam bym mu j`a zawi`oz`l, ale musz`e tu przez jaki`s czas zosta`c.
  mpoload 11133
  daj paczk $n
  pow $n Mam nadziej`e, `ze wykonasz dobrze to zadanie. `Zycz`e powodzenia.
  mpquest $n 11103 1
  hmm
  pow $n A w`la`snie! By`lbym zapomnia`l! Po drodze pozdr`ow ode mnie bosmana w porcie Tolarii. Gdyby pami`e`c ci`e zawiod`la, zawsze mog`e powt`orzy`c... Mo`zesz te`z prosi`c mnie o pomoc.
  break
endif
if queststage($n,11103)==17
  pow $n Chwilowo nie mo`zesz niczego wi`ecej dla mnie zrobi`c.
  break
endif
pow $n No ju`z da`lem ci zadanie... A gdyby pami`e`c ci`e zawiod`la, zawsze mog`e powt`orzy`c...  Mo`zesz te`z prosi`c mnie o pomoc.
~
>give_prog #3503~
if not cansee($n)
  '?!
  upu $o
  break
endif
if not doingquest($n,11103)
  daj $o $n
  pow $n A po co mi to?
  break
endif
if queststage($n,11103)==11
or queststage($n,11103)==12
  mpechoat $n wzrok+wid_i `Zo`lnierz chowa kawa`lek mi`esa za pazuch`e.
  mpjunk $o
  if sex($n)==2
    pow $n  Wspaniale, `ze ci si`e uda`lo! Mam nadziej`e, `ze nie przynios`la`s mi wstydu? Skocz mi po jakiego`s szczura na z`ab to pogadamy o interesach. A! Gdyby pami`e`c ci`e zawiod`la w tym momencie, to mog`e ci jeszcze powt`orzy`c...
  else
    pow $n  Wspaniale, `ze ci si`e uda`lo! Mam nadziej`e, `ze nie przynios`le`s mi wstydu?! Skocz mi po jakiego`s szczura na z`ab to pogadamy o interesach. A! Gdyby pami`e`c ci`e zawiod`la w tym momencie, to mog`e ci jeszcze powt`orzy`c...
  endif
  mpquest $n 11103 16
  break
endif
,ogl`ada ze zdziwieniem paczk`e.
pow $n A po co mi to?
daj $o $n
~
>give_prog #11112~
if not cansee($n)
  'Hej, kto mi tu jakie`s paskudztwo wciska?
  upu $o
  break
endif
if not doingquest($n,11103)
or queststage($n,11103)!=16
  daj $o $n
  pow $n A po co mi to?
  break
endif
,zjada szczura.
mpjunk $o
,ociera krew z brody...
if sex($n)==2
  pow $n Teraz mo`zemy pogada`c o interesach. Jestem z ciebie dumny! Dobrze si`e spisa`la`s. W nagrod`e otrzymujesz 45 punkt`ow zada`n.
else
  pow $n Teraz mo`zemy pogada`c o interesach. Jestem z ciebie dumny! Dobrze si`e spisa`le`s. W nagrod`e otrzymujesz 45 punkt`ow zada`n.
endif
mpqp $n 45
mpquest $n 11103 17
*KONIEC ZADANIA - RenTor, 6.VI.2000
*Spore poprawki Alandar - 11.X.2003
~
>tell_prog zawiod`lem zawiod`lam~
if not cansee($n)
  'Nie gadam z niewidzialnymi!
  break
endif
if not doingquest($n,11103)
  pow $n A w czym mog`e pom`oc?
  break
endif
if queststage($n,11103)==1
or queststage($n,11103)==2
or queststage($n,11103)==3
or queststage($n,11103)==4
or queststage($n,11103)==5
  pow $n Tak... Potwierdzi`ly si`e moje najgorsze przypuszczenia... Lecz nie wszystko stracone! Nie tra`c g`lowy... Musisz teraz uda`c si`e do sklepu sprzedawcy pami`atek na rynku w Vergardzie. W zamian za 5000 monet otrzymasz now`a paczk`e.
  mpquest $n 11103 3
  break
endif
~
>tell_prog pomoc pomocy pom`o`z~
if queststage($n,11103)==16
  pow $n O nie! W polowaniu na szczury to ci nie b`ed`e pomaga`l!
  break
endif
if queststage($n,11103)==17
  if sex($n) == 2
    pow $n Dosta`la`s nagrod`e za prac`e wykonan`a dla mnie... Nie oczekuj `zadnej dodatkowej pomocy, bo jej nie otrzymasz.
  else
    pow $n Dosta`le`s nagrod`e za prac`e wykonan`a dla mnie... Nie oczekuj `zadnej dodatkowej pomocy, bo jej nie otrzymasz.
  endif
  break
endif
if queststage($n,11103)==1
or queststage($n,11103)==2
or queststage($n,11103)==3
or queststage($n,11103)==4
or queststage($n,11103)==5
  if sex( $n ) == 1
    pow $n Je`zeli zgubi`le`s paczk`e, powiedz, `ze zawiod`le`s.
  endif
  if sex( $n ) == 2
    pow $n Je`zeli zgubi`la`s paczk`e, powiedz, `ze zawiod`la`s.
  endif
  break
endif
pow $n W tym momencie to nie u mnie masz szuka`c pomocy...
~
>tell_prog powt`orz~
if not cansee($n)
  break
endif
if not doingquest($n,11103)
  pow $n Co powt`orzy`c?
  break
endif
if queststage($n,11103)==1
or queststage($n,11103)==2
or queststage($n,11103)==5
  pow $n Zanie`s wreszcie paczk`e egzorcy`scie. Mieszka on w Nydogrodzie znajduj`acym si`e w Leanderze.
  break
endif
if queststage($n,11103)==3
or queststage($n,11103)==4
  if sex($n)==2
    pow $n Mia`la`s skoczy`c do Vergardu po now`a paczk`e, bo star`a zgubi`la`s, nieprawda`z?! Daj 5000 monet sprzedawcy ze sklepu z pami`atkami przy rynku.
  else
    pow $n Mia`le`s skoczy`c do Vergardu po now`a paczk`e, bo star`a zgubi`le`s, nieprawda`z?! Daj 5000 monet sprzedawcy ze sklepu z pami`atkami przy rynku.
  endif
  break
endif
if queststage($n,11103)==16
  pow $n Przynie`s mi szczura do jedzenia.
  break
endif
if queststage($n,11103)==17
  pow $n Powtarzam! To ju`z KONIEC zadania.
  break
endif
~
|
