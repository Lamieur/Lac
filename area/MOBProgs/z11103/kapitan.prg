* $Id: kapitan.prg,v 1.10 2006/05/26 23:29:37 lam Exp $

>greet_prog 100~
if not cansee($n)
or not doingquest( $n, 11103 )
or queststage($n,11103)!=13 && queststage($n,11103)!=11
or questdata3($n,11103)==0
  break
endif
usm $n
pow $n Co tam s`lycha`c $NW?
~
>trig_prog kufel~
if not cansee($n)
  'Ale dziwo! Kufel pojawia si`e znik`ad...
  break
endif
if not doingquest($n,11103)
*or queststage($n,11103)!=11
or questdata3($n,11103)!=0
  ,kr`eci smutno g`low`a.
  if sex($n) == 2
    pow $n Uwa`zaj! O ma`ly w`los, a zbi`laby`s ca`lkiem dobry kufel.
  else
    pow $n Uwa`zaj! O ma`ly w`los, a zbi`lby`s ca`lkiem dobry kufel.
  endif
  wez kufel
  daj kufel $n
  break
endif
, rzuca si`e pod twoje nogi i `lapie kufel w ostatniej chwili.
mppurge kufel
if sex($n) == 2
  pow $n Och! To m`oj ulubiony kufel. Musia`l ci go da`c karczmarz z Miden'niru. Skoro go dosta`la`s, to musisz by`c jego przyjaci`o`lk`a, a przyjaciele moich przyjaci`o`l s`a moimi przyjaci`o`lmi.
else
  pow $n Och! To m`oj ulubiony kufel. Musia`l ci go da`c karczmarz z Miden'niru. Skoro go dosta`le`s, to musisz by`c jego przyjacielem, a przyjaciele moich przyjaci`o`l s`a moimi przyjaci`o`lmi.
endif
usm $n
pow $n Mo`ze potrzebujesz si`e dok`ad`s uda`c?
mpquestdata $n 11103 3 1
~
>tell_prog tak oczywi`scie ch`etnie jasne pewnie~
if not cansee($n)
or not doingquest($n,11103)
  break
endif
pow $n A dok`ad?
~
>trig_prog ustaw_do_odplyniecia~
if not varexists("do_odplyniecia")
  aint do_odplyniecia=0
endif
aint do_odplyniecia=do_odplyniecia+1
~
>tell_prog Valisandria Vergard Valisandrii Vergardu~
if not cansee($n)
or not doingquest($n,11103)
or queststage($n,11103)!=13
or questdata3($n,11103)==0
  pow $n Tak, to pi`ekne miasto... Cz`esto tam p`lywa`lem za m`lodu.
  break
endif
if questdata3($n,11103)>5
  pow $n Niestety, ostatnio burmistrz wezwa`l mnie do siebie na dywanik i jasno da`l do zrozumienia, `ze koniec z darmowymi kursami dla znajomych. Miasto te`z musi zarobi`c.
  break
endif
if questdata2($n,11103)==1
  pow $n Spokojnie, poczekaj jeszcze chwil`e, je`sli nie przyp`lynie to znaczy, `ze si`e co`s sta`lo. Zaczynam si`e martwi`c...
  break
endif
hmm
pow $n Zaczekaj chwil`e. Nied`lugo ma t`edy przep`lywa`c m`oj znajomy prywatn`a `zagl`owk`a. Wybiera si`e w tym kierunku, wi`ec b`edzie m`og`l ci`e zabra`c.
if not varexists("zaglowka")
  aint zaglowka=0
endif
aint zaglowka=zaglowka+1
mpquestdata $n 11103 2 1
int tymczas=questdata3($n,11103)
int tymczas=tymczas+1
mpquestdata $n 11103 3 %(tymczas)
~
>tell_prog Drimith Tolaria Tolarii~
if not cansee($n)
or not doingquest($n,11103)
or queststage($n,11103)!=11
or questdata3($n,11103)==0
  pow $n Tak, to pi`ekne strony... Cz`esto tam p`lywa`lem za m`lodu.
  break
endif
if questdata3($n,11103)>5
  pow $n Niestety, ostatnio burmistrz wezwa`l mnie do siebie na dywanik i jasno da`l do zrozumienia, `ze koniec z darmowymi kursami dla znajomych. Miasto te`z musi zarobi`c.
  break
endif
if questdata2($n,11103)==1
  pow $n Spokojnie, poczekaj jeszcze chwil`e, je`sli nie przyp`lynie to znaczy, `ze si`e co`s sta`lo. Zaczynam si`e martwic...
  break
endif
hmm
pow $n Zaczekaj chwil`e. Nied`lugo ma t`edy przep`lywa`c m`oj znajomy prywatn`a `zagl`owk`a. Wybiera si`e w tym kierunku, wi`ec b`edzie m`og`l ci`e zabra`c.
if not varexists("zaglowka")
  aint zaglowka=0
endif
aint zaglowka=zaglowka+1
mpquestdata $n 11103 2 1
int tymczas=questdata3($n,11103)
int tymczas=tymczas+1
mpquestdata $n 11103 3 %(tymczas)
~
>tell_prog do~
if not cansee($n)
or not doingquest($n,11103)
  break
endif
pow $n Niestety, w podr`o`zy tam nie mog`e ci pom`oc...
~
|
