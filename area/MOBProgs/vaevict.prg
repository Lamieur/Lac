* $Id: vaevict.prg,v 1.19 2006/07/11 16:21:06 lam Exp $

>speech_prog witam cze`s`c cze hejka heja hej~
'Witam w moim mie`scie. Jakby kto`s jeszcze nie wiedzia`l to ja jestem tu burmistrzem!
~
>speech_prog burmistrz miasto~
,unosi dumnie g`low`e.
'Tak, to ja jestem tutaj od pilnowania porz`adku!
~
>speech_prog plotki plotka porz`adek~
'To miasto zosta`lo zbudowane w kr`olestwie magii. Przez nie prowadzi przej`scie na Pustkowia Chaosu. Dlatego spoczywa na mnie taka odpowiedzialno`s`c.
~
>speech_prog magia pustkowia chaos kr`olestwo odpowiedzialno`s`c~
'M`owi si`e, `ze na naszym mie`scie wci`a`z spoczywa Pi`etno Chaosu!
~
>speech_prog pi`etno~
'Przykro mi, nic wi`ecej nie wiem.
~
>speech_prog bye `zegnam zegnam papa pa~
'`Zegnaj $NW! I prosz`e ci`e bardzo, nie szukaj tu k`lopot`ow.
~
>all_greet_prog 33~
* Sorry, ale konstrukcja ponizszego proga jest pojebana, chujowa i bez ladu ni
* skladu, po prostu lamerska... :(((
* W dodatku pomylily ci sie and i or, albo kompletnie nie wiesz co piszesz
* Ok, proga zostawiam na pamiatke i pisze na nowo - jak sie skapuje
* o co chodzi... ;>
if !cansee($n) || isnpc($n)
  break
endif
if ( level($n) > 50 ) && ( !doingquest($n, 17704) )
* and !doingquest($n, 17704) - and? - Ulryk
  if sex($n) == 2
    pow $n Witaj kokietko!
  else
    pow $n Witaj stary druhu!
  endif
  break
endif
if ( level($n) > 25 ) && ( !doingquest($n, 17703) )
  pow $n Witam.
  break
endif
if ( level($n) > 12 ) && ( !doingquest($n, 17702) )
  pow $n O, jak ci`e mi`lo widzie`c $NW
  break
endif
if ( level($n) > 6 ) && ( !doingquest($n, 17701) )
  pow $n Witaj $NW.
  break
endif
if ( level($n) > 1 ) && ( !doingquest($n, 17700) )
  if sex($n) == 2
    pow $n Witaj nowicjuszko.
  else
    pow $n Witaj nowicjuszu.
  endif
  pow $n Je`zeli wykonasz dla mnie pewne zadanie, nie ominie ci`e nagroda. Wystarczy, `ze powiesz zadanie, a wyja`sni`e ci o co chodzi.
endif
~
>speech_prog p zadanie~
if !cansee($n)
  'Przykro mi, ale wol`e widzie`c rozm`owc`e...
  break
endif
if doingquest($n,17700)
 if doingquest($n,17701)
  if doingquest($n,17702)
   if doingquest($n,17703)
    if doingquest($n,17704)
      pow $n Na razie nie mam dla ciebie wi`ecej zada`n.
*      pow $n Przykro mi z tego powodu, ale jest ma`lo r`ak do pracy,
*      pow $n a RenTor sam sobie nie da rady ze wszystkim.
*      pow $n Krainki, progi, nowy mud i jeszcze studia, `zal mi go :(
      break
    endif
   endif
  endif
 endif
endif
if ( level($n) > 12 ) && ( !doingquest($n, 17702) )
 pow $n Mam dla ciebie propozycj`e. Gdy ostatnio wychodzi`lem z Tolarii na ma`l`a przechadzk`e, gdzie`s na drodze ko`lo miasta zgubi`lem pami`atkowy pier`scie`n. Nie ma on wiekszej warto`sci handlowej, ale jest pami`atk`a z podr`o`zy do pewnego odleg`lego kraju.
 patrz $n
 pow $n Poszukaj go i przynie`s mi prosz`e, mo`ze gdzie`s by`c zakopany...
 mpquest $n 17702 1
 mpquesttitle $n 17702 Pami`atkowy pier`scie`n
 mpquestdescr $n 17702 Burmistrz Vaevictus poprosi`l ci`e o odnalezienie pami`atkowego pier`scienia.
 mpquestdescr $n 17702 + Zgubi`l go gdzie`s na drodze ko`lo miasta. Musisz si`e liczy`c z tym, `ze pier`scie`n
 mpquestdescr $n 17702 + zosta`l wdeptany w piach i b`loto i odnalezienie go b`edzie wymaga`lo kopania.
// Kurde rentor, zapomiales o czyms takim?
// Ponizej zjebales... co to za nawiasy??
// mpquestdata $n 17702 1 %<17760;17771>
// Bedzie tak (troche dodaje lokacji) :
 mpquestdata $n 17702 0 177%{60;61;62;65;69;71}
// Tak, tylko szkoda, ze zadna lokacja nie ma dig_proga, ktorego
// zrobiles w tol2.prg. Moze nie zdazyles?  Ja poprawiam - G.
// A tak w ogole, to dig_prog sie nie nadaje do #ROOMS i zmienilem go
// na use_prog - odkop
 break
endif
if ( level($n) > 6 ) && ( !doingquest($n, 17701) )
 pow $n Mam dla ciebie propozycj`e.
 pow $n Podskocz do mistrzyni napraw, ma ona kawa`lek z`lota, kt`ory trzeba zanie`s`c jubilerowi.
 mpquest $n 17701 1
 mpquesttitle $n 17701 Z`lota sprawa
 mpquestdescr $n 17701 Burmistrz Vaevictus poprosi`l ci`e o za`latwienie drobnej sprawy. P`ojd`x do
 mpquestdescr $n 17701 + mistrzyni napraw po kawa`lek z`lota, a nast`epnie odnie`s go jubilerowi.
 break
endif
if ( level($n) > 1 ) && ( !doingquest($n, 17700) )
 pow $n Mam dla ciebie propozycj`e. Znajd`x mi w Tolarii skrzyde`lka kurczaka. S`a one do kupienia w kt`orym`s sklepie. Bardzo jestem g`lodny, wi`ec uciesz`e si`e, je`sli zgodzisz si`e wykona`c to dla mnie...
 mpquest $n 17700 1
 mpquesttitle $n 17700 Wyprawa na zakupy
 mpquestdescr $n 17700 Burmistrz Vaevictus poprosi`l ci`e o kupienie skrzyde`lek kurczaka. Poszukaj
 mpquestdescr $n 17700 + w`la`sciwego sklepu i dokonaj zakupu, a nast`epnie zanie`s jedzenie Vaevictusowi.
 break
endif
~
>speech_prog pier`scie`n~
'Zgin`a`l mi pier`scie`n. Ostatnio jak pami`etam to mia`lem go na drodze ko`lo Tolarii.
~
>give_prog #17713~
if !cansee($n)
or !doingquest($n, 17700)
or queststage ($n, 17700) != 1
  'Po co mi to?
  upu $o
  break
else
  jedz skrz
*bez mpjunk nie mogl potem uniesc nic innego (np pierscienia)
*on w ogole nie moze uniesc nic o wadze wiekszej niz 3
  mpjunk skrz
  if sex($n) == 2
    usm $n
    pow $n Spisa`la`s si`e nad wyraz dobrze! Oto twoja nagroda, na kt`or`a zas`lu`zy`la`s.
  else
    pow $n Spisa`le`s si`e nad wyraz dobrze! Oto twoja nagroda, na kt`or`a zas`lu`zy`le`s.
  endif
endif
mpechoat $n zawsze Dostajesz w nagrod`e 5 punkt`ow zada`n.
mpqp $n 5
daj 1000 mon $n
mpquest $n 17700 2
mpquesttitle $n 17700 clear
~
>give_prog #18006~
if !cansee($n)
 'Fajnie, ale ja nie widz`e od kogo to dosta`lem.
* 'Mimo wszystko dzi`ekuj`e.  B`ed`e mia`l wi`ecej pieni`a`zk`ow... hiehie.
  upu $o
*O sorry :D Wszystkich do myslenie namawiacie tu...
*Przeciez !cansee($n)... to jak chcecie skonczyc zadanie ktosiowi???
*W ogole to zadanie jest cale do przepisania :D
*  if queststage($n, 17702) != 3
*     mpendquest $n 17702
*  endif
* A dajmy gupim jeszcze szanse :)
* ta jeszcze jedna szansa pozwala robic to zadanie w nieskonczonosc..., taka
* fabryka punktow zadan. Dodaje warunek, ze queststage != 3 - Ulryk
 break
endif
// tego ponizej nie dales, Rentor, a potrzeba
if !doingquest($n, 17702)
or queststage($n, 17702) != 2
// to powyzej nie wystarczy, bo jak nie robi questa, to nie ma przeciez qstage!
  'Dzi`ekuj`e ci za pier`scie`n, $NW.
  mpjunk $o
  break
else
  mpjunk $o
  mpechoat $n wzrok+wid_i $I chowa pier`scie`n.
  mpechoaround $n wzrok+wid_i+wid_n $I chowa pier`scie`n, kt`ory dosta`l od $ND.
  if sex($n) == 2
    usm $n
    pow $n Spisa`la`s si`e nad wyraz dobrze!
    pow $n Oto twoja nagroda, na kt`or`a zas`lu`zy`la`s.
  else
    pow $n Spisa`le`s si`e nad wyraz dobrze!
    pow $n Oto twoja nagroda, na kt`or`a zas`lu`zy`le`s.
  endif
  mpechoat $n zawsze Dostajesz 16 punkt`ow zada`n.
  mpqp $n 16
  daj 8000 mon $n
  mpquest $n 17702 3
  mpquesttitle $n 17702 clear
endif
~

>give_prog all~
if cansee($n)
   'Czy ty mnie przypadkiem nie pr`obujesz przekupi`c, $NW?
   mpechoat $n wzrok+wid_i Burmistrz przygl`ada ci si`e podejrzliwie.
   mpechoaround $n wzrok+wid_i+wid_n Burmistrz przygl`ada si`e podejrzliwie $NC.
else
   'A sk`ad tu si`e wzi`a`l $O?
endif
wlo $o urna
~
|
