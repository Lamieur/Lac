* $Id: wrozka.prg,v 1.12 2006/07/26 00:17:49 lam Exp $

>all_greet_prog 100~
if cansee($n) && ispc($n)
  if doingquest($n,600)
    'Witaj ponownie w Ofcolu, w`edrowcze.
    usm $n
  else
    pow $n Witaj $NW. Je`zeli chcesz, mo`zesz wykona`c zadanie.
  endif
endif
~
>speech_prog p zadanie~
if not cansee($n)
 'Rozmawiam tylko z tymi, kt`orych da si`e zobaczy`c.
 break
endif
if doingquest($n,600)
  'Ju`z ci m`owi`lam o zadaniu...
else
  if level($n) > 35
     pow $n Mam pewn`a pro`sb`e do ciebie. Czy zechcesz mnie wys`lucha`c?
  else
     mpechoat $n wzrok+wid_i Wr`o`zka przygl`ada ci si`e uwa`znie.
     if sex($n) == 2
       pow $n Wybacz, ale wydaje mi si`e, `ze nie jeste`s jeszcze wystarczaj`aco do`swiadczona.
     else
       pow $n Wybacz, ale wydaje mi si`e, `ze nie jeste`s jeszcze wystarczaj`aco do`swiadczony.
     endif
  endif
endif
~
>speech_prog p nie~
if cansee($n)
  pow $n To nic, dzi`ekuj`e za odpowied`x.
  usm $n
endif
~
>speech_prog tak chc`e jasne pewnie oczywi`scie~
if not cansee($n)
  break
endif
if level($n) > 35
 if not doingquest($n,600)
  'Oto jakie mam dla ciebie zadanie: w`ladca kr`olestwa Mahn-Tor posiada magiczn`a sfer`e niewra`zliwo`sci. Przynie`s mi j`a na znak, `ze chcesz mi s`lu`zy`c, a dostaniesz nagrod`e. I pami`etaj, `ze on nie poczeka na tw`oj atak! Powodzenia.
  mpquest $n 600 1
 else
  'Ile razy chcesz robi`c to zadanie?
 endif
else
  pow $n Przykro mi, ale nie masz wystarczaj`aco du`zo do`swiadczenia, by wykona`c to zadanie.
endif
~
>give_prog #2340~
if not cansee($n)
  'Nie widz`e kto to mi daje!
  upu $o
  break
endif
if not doingquest($n,600)
or queststage($n,600) != 2
  'Po co mi to dajesz?
  daj $o $n
  break
endif
mpjunk $o
'Dzi`ekuj`e za t`e sfer`e. Widz`e, `ze uda`lo ci si`e zabi`c wielkiego mistrza minotaur`ow. A oto i nagroda:
mpechoat $n zawsze Otrzymujesz w nagrod`e 15 punkt`ow zada`n.
mpqp $n 15
mpquest $n 600 3
~
|
