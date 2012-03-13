* $Id: marynarz.prg,v 1.17 2006/05/26 11:59:59 lam Exp $

>all_greet_prog 100~
* Sprawdzone i poprawione
* Przy okazji wycialem te wszystkie bezsensowne breaki, oraz
* zablokowalem dawanie nagrody pienieznej (albo stworzyc do tego
* przedmiot trzeba, albo to olac w ogole, wybralem ta druga opcje)
* -----------------------------------[29-03-02, Ulryk]------------
if isnpc($n)
or ( !cansee($n) ) || ( isimmort($n) )
  break
endif
if doingquest($n, 3000)
  int etap = queststage($n, 3000)
  if ( ( 2 * ( etap / 2 ) ) != etap ) || ( etap == 8 )
     break
  endif
endif
if ( level($n) > 50) && ( queststage($n, 3000) < 10 )
  if sex($n) == 2
    pow $n Witaj kokietko!
  else
    pow $n Witaj stary druhu!
  endif
  pow $n Je`zeli wykonasz dla mnie pewne zadanie to nie ominie ciebie nagroda.
  pow $n Wystarczy, `ze powiesz "zadanie" a wyja`sni`e ci o co chodzi.
else
  if ( level($n) > 30 ) && ( queststage($n, 3000) < 10 )
    pow $n Witam.
    pow $n Je`zeli wykonasz dla mnie pewne zadanie to nie ominie ciebie nagroda.
    pow $n Wystarczy, `ze powiesz "zadanie" a wyja`sni`e ci o co chodzi.
  else
    if ( level($n) > 15 ) && ( queststage($n, 3000) < 6 )
      pow $n O jak ci`e mi`lo widzie`c, $NW
      pow $n Je`zeli wykonasz dla mnie pewne zadanie to nie ominie ciebie nagroda.
      pow $n Wystarczy, `ze powiesz "zadanie" a wyja`sni`e ci o co chodzi.
    else
      if ( level($n) > 5 ) && ( queststage($n, 3000) < 4 )
        pow $n Witaj $NW.
        pow $n Je`zeli wykonasz dla mnie pewne zadanie to nie ominie ciebie nagroda.
        pow $n Wystarczy, `ze powiesz "zadanie" a wyja`sni`e ci o co chodzi.
      else
        if ( level($n) > 1 ) && ( !doingquest($n, 3000) )
          if sex($n) == 2
            pow $n Witaj nowicjuszko.
          else
            pow $n Witaj nowicjuszu.
          endif
          pow $n Je`zeli wykonasz dla mnie pewne zadanie to nie ominie ciebie nagroda.
          pow $n Wystarczy, `ze powiesz "zadanie" a wyja`sni`e ci o co chodzi.
        endif
      endif
    endif
  endif
endif
~
>speech_prog p zadanie~
if ( !cansee($n) ) || ( isimmort($n) )
  break
endif
if doingquest($n, 3000)
 if queststage($n ,3000) == 9
    pow $n Niestety, nie mam na razie `zadnych zada`n dla ciebie...
 endif
 if ( queststage($n, 3000) == 7 ) || ( queststage($n, 3000) == 8 )
    pow $n Nadal czekam na `zelazn`a koron`e...
 endif
 if ( queststage($n, 3000) == 6 ) && ( level($n) > 30 )
    pow $n Tak wi`ec, chcesz mi pom`oc? Hmmm. Chyba si`e do tego nadajesz. S`luchaj, sprawa jest pilna, wi`ec powiem kr`otko. Zabij smoka maj`acego legowisko w starym lesie i przynie`s mi jego koron`e. P`ed`x szybko i wype`lnij swoj`a misj`e godnie!
    mpquest $n 3000 7
 endif
 if queststage($n, 3000) == 5
    pow $n Nadal czekam na dzikie kwiaty...
 endif
 if ( queststage($n, 3000) == 4 ) && ( level($n) > 15 )
    pow $n A wiesz, `ze mo`zesz co`s dla mnie zrobi`c? Tak sobie stoj`e tutaj samotnie od wielu godzin, ma`lo sypiam...
    ,wzdycha.
    pow $n Ale taka bezczynno`s`c i zm`eczenie przywodz`a mi na my`sl niezliczone wyprawy, jakie odby`lem w swoim niekr`otkim `zyciu. Szczeg`olnie jedna z wypraw zapad`la mi w pami`eci. By`lo to jakie`s %{23;34;42;32;33;24} lata temu... Tak...
    ,rozmarzy`l si`e zupe`lnie.
    pow $n By`la to wyprawa maj`aca na celu...
    hmm
    pow $n A niech to diabe`l morski poch`lonie! Zapomnia`lem... Stary ju`z jestem...
    ,prostuje si`e w jednej chwili.
    pow $n No dobra, starczy tego sm`ecenia. Ju`z przechodz`e do rzeczy. Potrzeba mi zapachu dzikich kwiat`ow, kt`ore przez chwil`e dadz`a mi mo`zliwo`s`c przeniesienia si`e do starych czas`ow... Znajdziesz je gdzie`s na R`owninach. Powodzenia.
    ,zamy`sla si`e g`l`eboko.
    mpquest $n 3000 5
 endif
 if queststage($n, 3000) == 3
    pow $n Czy masz ju`z dla mnie chusteczk`e?
 endif
 if ( queststage($n, 3000) == 2 ) && ( level($n) > 5 )
    pow $n Mam dla ciebie propozycj`e. Przyda`laby mi si`e chusteczka. Nabawi`lem si`e ostatnio kataru. Miejski p`laczek zawsze ma kilka czystych, wi`ec nie sprawi ci chyba `zadnego problemu je`zeli mi jedn`a od niego przyniesiesz.
    mpquest $n 3000 3
 endif
 if queststage($n, 3000) == 1
   if sex($n) == 2
      pow $n Nie znalaz`la`s jeszcze kubka wody?
   else
      pow $n Nie znalaz`le`s jeszcze kubka wody?
   endif
 endif
else
  pow $n Mam dla ciebie propozycj`e. Poszukaj mi w Midgaardzie kubka wody. Ma go pewna osoba znajduj`aca si`e w parku. Jestem bardzo spragniony, wi`ec uciesz`e si`e je`sli zgodzisz si`e wykona`c to dla mnie.
  mpquest $n 3000 1
*  pow $n Niestety, chyba nie ob`edzie si`e bez u`zycia si`ly.
*  ^ wycinam, bo s`lu`z`aca ma 80 poziom i jest sprzedawc`a - Ulryk
endif
~
>give_prog #3102~
if ( !cansee($n) ) || ( isimmort($n) )
or !doingquest($n, 3000)
or queststage($n, 3000) != 1
  'Po co mi to?
  upu $o
else
  pij kub
  if sex($n) == 2
    usm $n
    pow $n Spisa`la`s si`e nad wyraz dobrze! Oto twoja nagroda, na kt`or`a zas`lu`zy`la`s.
  else
    pow $n Spisa`le`s si`e nad wyraz dobrze! Oto twoja nagroda, na kt`or`a zas`lu`zy`le`s.
  endif
  mpechoat $n zawsze Dostajesz w nagrod`e 6 punkt`ow zada`n.
  mpqp $n 6
*  daj 1000 mon $n
  mpquest $n 3000 2
endif
~
>give_prog #3103~
if ( !cansee($n) ) || ( isimmort($n) )
or !doingquest($n, 3000)
or queststage($n, 3000) != 3
or questdata0($n, 3000) != 1
  'Po co mi to?
  upu $o
  if ( queststage($n, 3000) == 3 ) && ( questdata0($n, 3000) != 1 )
    'Nawet nie chcia`lo ci si`e pofatygowa`c do p`laczka... Ech, ty leniu!
  endif
else
  ,smarka.
  ,wyciera chusteczk`a nos.
  mpjunk $o
  mpechoat $n wzrok+wid_i $I chowa chusteczk`e.
  mpechoaround $n wzrok+wid_i+wid_n $I chowa chusteczk`e, kt`or`a dosta`l od $ND.
  if sex($n) == 2
    usm $n
    pow $n Spisa`la`s si`e nad wyraz dobrze! Oto twoja nagroda, na kt`or`a zas`lu`zy`la`s.
  else
    pow $n Spisa`le`s si`e nad wyraz dobrze! Oto twoja nagroda, na kt`or`a zas`lu`zy`le`s.
  endif
  mpechoat $n zawsze Dostajesz w nagrod`e 12 punkt`ow zada`n.
  mpqp $n 12
*  daj 9000 mon $n
  mpquest $n 3000 4
endif
~
>give_prog #304~
if ( !cansee($n) ) || ( isimmort($n) )
or !doingquest($n, 3000)
or queststage ($n, 3000) != 5
  'Po co mi to?
  upu $o
else
  ,bierze kwiaty w swoje r`ece z czci`a i uwielbieniem.
  ,wzdycha.
  if sex($n) == 2
    usm $n
    pow $n Spisa`la`s si`e nad wyraz dobrze! Oto twoja nagroda, na kt`or`a zas`lu`zy`la`s.
  else
    pow $n Spisa`le`s si`e nad wyraz dobrze! Oto twoja nagroda, na kt`or`a zas`lu`zy`le`s.
  endif
     mpechoat $n zawsze Dostajesz w nagrod`e 12 punkt`ow zada`n.
     mpqp $n 12
*  daj 7000 mon $n
  mpquest $n 3000 6
endif
~
>give_prog #6112~
if ( !cansee($n) ) || ( isimmort($n) )
or !doingquest($n, 3000)
or ( queststage($n, 3000) != 7 ) && ( queststage($n, 3000) != 8 )
  'Po co mi to?
  upu $o
  break
endif
if queststage($n, 3000) == 7
 if sex ($n) == 2
  pow $n Ty ma`la oszustko! To nie ty zabi`la`s smoka! Zab`ojc`a mia`la`s by`c w`la`snie ty!
 else
  pow $n Ty ma`ly oszu`scie! To nie ty zabi`le`s smoka! Zab`ojc`a mia`le`s by`c w`la`snie ty!
 endif
 pow $n Korony ci nie oddam, ale masz jeszcze jedn`a szans`e. Powtarzam: masz zabi`c smoka i przynie`s`c mi koron`e!
 break
endif
mpecho wzrok+wid_i $I bierze koron`e w swoje, przypominaj`ace dwa bochny chleba, r`ece i ogl`ada j`a.
hmm
,pr`obuje w`lo`zy`c koron`e na swoj`a g`low`e.
zal koro
usm
pow $n Tak... to jest to!
if sex($n) == 2
    usm $n
    pow $n Wielkie dzi`eki. Zadanie wykona`la`s wspaniale!
else
    pow $n Wielkie dzi`eki. Zadanie wykona`le`s wspaniale!
endif
pow $n A oto zas`lu`zona nagroda:
mpechoat $n zawsze Dostajesz w nagrod`e 20 punkt`ow zada`n.
mpqp $n 20
* daj 50000 mon $n
mpquest $n 3000 9
~
|
