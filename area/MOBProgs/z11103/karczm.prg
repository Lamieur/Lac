* $Id: karczm.prg,v 1.5 2006/09/18 20:55:17 skifr Exp $

>give_prog #3502~
if not cansee($n)
  'Hej, kto mi tu jak`a`s tarcz`e daje?
  upu $o
  break
endif
if not doingquest($n,11103)
or queststage($n,11103)!=10
  daj $o $n
  pow $n A po co mi to?
  break
endif
mpjunk $o
,patrzy z niedowierzaniem na tarcz`e.
pow $n Niemo`zliwe! Znalaz`la si`e moja stara tarcza! Tyle lat jej bezskutecznie szuka`lem. Masz, we`x w zamian ode mnie ten kawa`lek mi`esiwa.
mpoload 3503
daj kawalek $n
mpquest $n 11103 11
pow $n Wybierz si`e teraz do starej gospody i daj go `zo`lnierzowi. Je`zeli zgubisz ten kawa`lek mi`esa, wr`o`c tu i popro`s mnie o pomoc. Mog`e ci te`z powt`orzy`c co masz teraz zrobi`c. A w`la`snie! Upu`s`c ten kufel obok emerytowanego kapitana z Midgaardu.
mpoload 3504
daj kufel $n
~
>give_prog #29970~
if not cansee( $n )
  'Hej, kto mi tu jaki`s list daje?
  upu $o
  break
endif
if !doingquest( $n, 11103 )
or queststage( $n, 11103 ) != 9 && questdata0( $n, 11103 ) == 1
  daj $o $n
  pow $n A po co mi to?
  break
endif
mpecho wzrok+wid_i $I czyta list polecaj`acy.
usmiech $n
pow $n Widz`e, `ze znasz egzorcyst`e. Napisa`l mi tu w li`scie, `ze mo`zesz dla mnie wykona`c ma`le zadanie. Znajd`x moj`a tarcz`e, kt`or`a zgubilem gdzie`s ko`lo karczmy. By`lo to do`s`c dawno, wi`ec mo`ze by`c ona zakopana. Zawsze mog`e powt`orzy`c lub pom`oc.
mpat %{3570;3571;3572;3573;3575} mpoload 3502
mpquestdata $n 11103 0 1
~
>tell_prog powt`orz~
if not cansee($n)
  break
endif
if not doingquest($n,11103)
  pow $n Co powt`orzy`c?
  break
endif
if queststage( $n, 11103 ) == 9 && questdata0( $n, 11103 ) == 1
or queststage( $n, 11103 ) == 10
  pow $n Znajd`x moj`a tarcz`e, kt`ora le`zy gdzie`s zakopana w okolicach karczmy.
  break
endif
if queststage($n,11103)==11
  pow $n Zanie`s kawa`lek mi`esa `zo`lnierzowi, a kufel upu`s`c obok kapitana.
else
  if queststage($n,11103)==12
    pow $n Masz zanie`s`c kawa`lek mi`esa `zo`lnierzowi...
  else
    if queststage($n,11103)==13
    or queststage($n,11103)==14
      pow $n Prosi`lem ci`e o pr`obki napoj`ow od karczmarza, kt`ory prowadzi karczm`e w Vergardzie.
    else
      pow $n Nie przypominam sobie niczego, co mia`lbym ci powt`orzy`c.
      break
    endif
  endif
endif
~
>tell_prog zawiod`lem zawiod`lam~
if not cansee($n)
  break
endif
if not doingquest($n,11103)
  pow $n Jestem bardzo zaj`ety i w niczym ci nie pomog`e.
  break
endif
if queststage( $n, 11103 ) == 9 && questdata0( $n, 11103 ) == 1
or queststage($n,11103)==10
or queststage($n,11103)==11
or queststage($n,11103)==12
  pow $n A jednak co`s si`e sta`lo! Lecz nie wszystko stracone! Nie tra`c g`lowy... Przynie`s mi pr`obki trzech napoj`ow od karczmarza z karczmy "Ostatni Dom" w Vergardzie. Wtedy dam ci kawa`lek mi`esa dla `zo`lnierza.
  mpquest $n 11103 13
  break
endif
~
>tell_prog pomoc~
if not cansee($n)
  break
endif
if not doingquest($n,11103)
  pow $n Jestem bardzo zaj`ety i w niczym ci nie pomog`e.
  break
endif
if queststage( $n, 11103 ) == 9 && questdata0( $n, 11103 ) == 1
or queststage( $n, 11103 ) == 10
or queststage( $n, 11103 ) == 11
or queststage( $n, 11103 ) == 12
  if sex( $n ) == 1
    pow $n Je`zeli zgubi`le`s przedmiot potrzebny do wykonana zadania, lub nie mo`zesz go znale`x`c, powiedz, `ze zawiod`le`s.
  endif
  if sex( $n ) == 2
    pow $n Je`zeli zgubi`la`s przedmiot potrzebny do wykonana zadania, lub nie mo`zesz go znale`x`c, powiedz, `ze zawiod`la`s.
  endif
  break
endif
if queststage($n,11103)==13
or queststage($n,11103)==14
  pow $n No ju`z w tym ci nie pomog`e!
  pow $n Przynie`s mi do spr`obowania trzy napoje od karczmarza z Vergardu.
  break
endif
pow $n Jestem bardzo zaj`ety i w niczym ci nie pomog`e.
~
>give_prog #10307~
if not objval2($o) == 27
  'Tego napoju nie szukam.
  upu $o
  break
endif
if not cansee($n)
  'Kto`s tu chce mnie chyba rozpi`c...
  upu $o
  break
endif
if not doingquest($n,11103)
or queststage($n,11103)!=13 && queststage($n,11103)!=14
  ,ogl`ada nap`oj z zainteresowaniem.
  pow $n Ciekawe, ciekawe.... ale chwilowo nie poszukuj`e nowych trunk`ow.
  daj $o $n
  break
endif
if questdata4($n,11103)==0
  ,zbli`za kieliszek do nosa delektuj`ac si`e zapachem.
  ,upija odrobin`e napoju w zamy`sleniu.
  pow $n Ten trunek przywo`la`l mi na my`sl wiele wspomnie`n.
  ,patrzy niewidz`acym wzrokiem w dal.
  mpquestdata $n 11103 4 1
  break
endif
if questdata4($n,11103)==2
  ,zbli`za kieliszek do nosa delektuj`ac si`e zapachem.
  ,upija odrobin`e napoju w zamy`sleniu.
  pow $n Ten trunek przywo`la`l mi na my`sl wiele wspomnie`n.
  ,patrzy niewidz`acym wzrokiem w dal.
  mpquestdata $n 11103 4 4
  break
endif
if questdata4($n,11103)==3
  ,zbli`za kieliszek do nosa delektuj`ac si`e zapachem.
  ,upija odrobin`e napoju w zamy`sleniu.
  pow $n Ten trunek przywo`la`l mi na my`sl wiele wspomnie`n.
  ,patrzy niewidz`acym wzrokiem w dal.
  mpquestdata $n 11103 4 5
  break
endif
if questdata4($n,11103)==6
  ,podziwia pod `swiat`lo barw`e napoju.
  ,ostro`znie w`acha trunek.
  ,z wyrazem skupienia na twarzy zamacza usta w kieliszku.
  pow $n Taaak, bardzo ciekawe po`l`aczenie. A, no tak, to ju`z wszystkie, oto mi`eso.
  mpoload 3503
  daj kawalek $n
  if sex($n)==2
    pow $n Wybierz si`e teraz do starej gospody i daj je `zo`lnierzowi. Tym razem b`ad`x ostro`zniejsza.
  else
    pow $n Wybierz si`e teraz do starej gospody i daj je `zo`lnierzowi. Tym razem b`ad`x ostro`zniejszy.
  endif
  mpquestdata $n 11103 4 0
  mpquest $n 11103 11
  break
endif
pow $n Nie, nie, "P`lon`acego go`l`ebia" ju`z pr`obowa`lem...
daj $o $n
~
>give_prog #10308~
if not objval2($o) == 28
  'Tego napoju nie szukam.
  upu $o
  break
endif
if not cansee($n)
  'Kto`s tu chce mnie chyba rozpi`c...
  upu $o
  break
endif
if not doingquest($n,11103)
or queststage($n,11103)!=13 && queststage($n,11103)!=14
  ,ogl`ada nap`oj z zainteresowaniem.
  pow $n Ciekawe, ciekawe.... ale chwilowo nie poszukuj`e nowych trunk`ow.
  daj $o $n
  break
endif
if questdata4($n,11103)==0
  ,przygl`ada si`e kieliszkowi z zaciekawieniem.
  ,wypija nap`oj jednym haustem.
  pow $n Ech... to jest to! W`la`snie takiego trunku szuka`lem.
  mpquestdata $n 11103 4 2
  break
endif
if questdata4($n,11103)==1
  ,przygl`ada si`e kieliszkowi z zaciekawieniem.
  ,wypija nap`oj jednym haustem.
  pow $n Ech... to jest to! W`la`snie takiego trunku szuka`lem.
  mpquestdata $n 11103 4 4
  break
endif
if questdata4($n,11103)==3
  ,przygl`ada si`e kieliszkowi z zaciekawieniem.
  ,wypija nap`oj jednym haustem.
  pow $n Ech... to jest to! W`la`snie takiego trunku szuka`lem.
  mpquestdata $n 11103 4 6
  break
endif
if questdata4($n,11103)==5
  ,podziwia pod `swiat`lo barw`e napoju.
  ,ostro`znie w`acha trunek.
  ,z wyrazem skupienia na twarzy zamacza usta w kieliszku.
  pow $n Taaak, bardzo ciekawe po`l`aczenie. A, no tak, to ju`z wszystkie, oto mi`eso.
  mpoload 3503
  daj kawalek $n
  if sex($n)==2
    pow $n Wybierz si`e teraz do starej gospody i daj je `zo`lnierzowi. Tym razem b`ad`x ostro`zniejsza.
  else
    pow $n Wybierz si`e teraz do starej gospody i daj je `zo`lnierzowi. Tym razem b`ad`x ostro`zniejszy.
  endif
  mpquestdata $n 11103 4 0
  mpquest $n 11103 11
  break
endif
pow $n Nie, nie, "`Zar s`lo`nca" ju`z pr`obowa`lem...
daj $o $n
~
>give_prog #10309~
if not objval2($o) == 29
  'Tego napoju nie szukam.
  upu $o
  break
endif
if not cansee($n)
  'Kto`s tu chce mnie chyba rozpi`c...
  upu $o
  break
endif
if not doingquest($n,11103)
or queststage($n,11103)!=13 && queststage($n,11103)!=14
  ,ogl`ada nap`oj z zainteresowaniem.
  pow $n Ciekawe, ciekawe.... ale chwilowo nie poszukuj`e nowych trunk`ow.
  daj $o $n
  break
endif
if questdata4($n,11103)==0
  ,z wyrazem b`logo`sci na twarzy wdycha wo`n napoju.
  ,ostro`znie bierze niewielki `lyk z kieliszka.
  pow $n Mocny skubaniec, takie w`la`snie lubi`e!
  usm
  mpquestdata $n 11103 4 3
  break
endif
if questdata4($n,11103)==1
  ,z wyrazem b`logo`sci na twarzy wdycha wo`n napoju.
  ,ostro`znie bierze niewielki `lyk z kieliszka.
  pow $n Mocny skubaniec, takie w`la`snie lubi`e!
  usm
  mpquestdata $n 11103 4 5
  break
endif
if questdata4($n,11103)==2
  ,z wyrazem b`logo`sci na twarzy wdycha wo`n napoju.
  ,ostro`znie bierze niewielki `lyk z kieliszka.
  pow $n Mocny skubaniec, takie w`la`snie lubi`e!
  usm
  mpquestdata $n 11103 4 6
  break
endif
if questdata4($n,11103)==4
  ,podziwia pod `swiat`lo barw`e napoju.
  ,ostro`znie w`acha trunek.
  ,z wyrazem skupienia na twarzy zamacza usta w kieliszku.
  pow $n Taaak, bardzo ciekawe po`l`aczenie. A, no tak, to ju`z wszystkie, oto mi`eso.
  mpoload 3503
  daj kawalek $n
  if sex($n)==2
    pow $n Wybierz si`e teraz do starej gospody i daj je `zo`lnierzowi. Tym razem b`ad`x ostro`zniejsza.
  else
    pow $n Wybierz si`e teraz do starej gospody i daj je `zo`lnierzowi. Tym razem b`ad`x ostro`zniejszy.
  endif
  mpquestdata $n 11103 4 0
  mpquest $n 11103 11
  break
endif
pow $n Nie, nie, "Krwaw`a g`low`e" ju`z pr`obowa`lem...
daj $o $n
~
|
