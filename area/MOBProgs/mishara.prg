* $Id: mishara.prg,v 1.18 2006/07/11 18:37:45 lam Exp $

>greet_prog 100~
if inroom($i) != 17741
  break
endif
if isnpc($n)
  break
endif
* Gwoli informacji, quest 17701 jest uruchamiany w vaevict.prg - Grzechu
if !doingquest($n, 17701)
or queststage($n, 17701) == 3
  if doingquest($n, 17708)
    if queststage($n, 17708) == 3
      pow $n Witaj ponownie w moim warsztacie. W czym mog`e pom`oc?
    else
      pow $n Witaj ponownie w moim warsztacie. Jak posz`ly poszukiwania he`lmu?
    endif
  else
    pow $n Witaj $NW. Mo`ze masz woln`a chwil`e, `zeby wykona`c drobne zadanie?
  endif
  break
else
  if !doingquest($n, 17701)
  or queststage ($n, 17701) > 1
    pow $n Witaj w mym punkcie us`lugowym! W czym mog`e pom`oc?
    break
  endif
endif
pow $n Witaj w moim warsztacie $NW. Mam dla ciebie kawa`lek z`lota, kt`ory trzeba zanie`s`c jubilerowi.
if carry_number( $n ) >= can_carry_n( $n )
   pow $n Niestety masz przy sobie zbyt du`zo przedmiot`ow, by go dosta`c.
   break
endif
if carry_weight( $n ) + 9 > can_carry_w( $n )
   pow $n Niestety jest on dla ciebie za ci`e`zki, przyjd`x, kiedy b`edziesz w stanie go unie`s`c.
   break
endif
mpoload 18005 10
daj kaw $n
pow $n Id`x, zanie`s mu go.
mpquest $n 17701 2
mpquestdescr $n 17701 Burmistrz Vaevictus poprosi`l ci`e o za`latwienie drobnej sprawy.
if sex($n) == 2
  mpquestdescr $n 17701 + Uda`la`s si`e ju`z do mistrzyni napraw po kawa`lek z`lota, a teraz musisz go zanie`s`c
else
  mpquestdescr $n 17701 + Uda`le`s si`e ju`z do mistrzyni napraw po kawa`lek z`lota, a teraz musisz go zanie`s`c
endif
mpquestdescr $n 17701 + jubilerowi.
~
>speech_prog zadanie? zadanie~
if inroom($i) != 17741
  break
endif
if !cansee($n)
or doingquest($n, 17708)
or doingquest($n, 17701) && queststage($n, 17701) != 3
  break
else
  pow $n Mam pewn`a pro`sb`e do ciebie. Czy zechcesz mnie wys`lucha`c?
endif
~
>speech_prog p nie~
if inroom($i) != 17741
  break
endif
if cansee($n)
  if doingquest($n, 17701) && queststage($n, 17701) != 3
    break
  endif
  if not doingquest($n, 17708)
    pow $n To nic, dzi`ekuj`e za odpowied`x.
    usm $n
  endif
endif
~
>speech_prog p tak~
if inroom($i) != 17741
or not cansee($n)
  break
endif
if doingquest($n, 17701) && queststage($n, 17701) != 3
  break
endif
if not doingquest($n, 17708)
  pow $n Gdzie`s w twierdzy znajduje si`e he`lm. Zostawi`lam go, gdy by`lam tam ostatnio. Poszukaj go i przynie`s mi, prosz`e.
  mpquest $n 17708 1
  mpquesttitle $n 17708 Zaginiony he`lm
  mpquestdescr $n 17708 Mistrzyni napraw zgubi`la gdzie`s w twierdzy he`lm, kt`ory mia`la naprawi`c.
  mpquestdescr $n 17708 + Id`x tam i poszukaj go.
  mpquestdata $n 17708 0 39%{40;37;33;24;00;03;46;51}
endif
~
>give_prog #3914~
if not cansee($n)
  'Nie widz`e, kto mi to daje!
  upus $o
  break
endif
if not doingquest($n, 17708)
or     queststage($n, 17708) != 2
  pow $n Po co mi to dajesz?
  daj $o $n
  break
endif
pow $n Dzi`ekuj`e ci za ten he`lm. Widz`e, `ze chcia`lo ci si`e nachodzi`c.
mpecho wzrok+wid_i+wid_o $I wysy`la $O do przechowalni.
mpjunk $o
pow $n Oto nagroda! Dostajesz ode mnie 14 punkt`ow zada`n.
mpqp $n 14
mpquest $n 17708 3
mpquesttitle $n 17708 clear
~
>give_prog all~
wy`sl $o
~
>speech_prog witaj cze`s`c cze~
if cansee($n)
  pow $n Witam, witam...
endif
~
>say_prog punkt us`lugowy naprawa punkcie~
'Tak. Chocia`z aktualnie zamiast naprawia`c, g`l`ownie dostosowuj`e przedmioty do nowych w`la`scicieli.
u`sm
~
>speech_prog papa `zegnaj pap pa~
if cansee($n)
  pow $n Bezpiecznej podr`o`zy!
endif
~
|
