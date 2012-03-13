* $Id: kat.prg,v 1.24 2006/09/12 11:49:33 lam Exp $

>greet_prog 100~
if isnpc($n)
   powiedz $n Cze`s`c mobie!
else
   if isgood($n)
        '%{Jak leci, $NW?;Hej, $NW!;Jak tam?;Co tam u ciebie, $NW?}
        if age($n) == 17
           glaszcz $n
        else
           browar $n
        endif
   else
     if !isimmort($n)
        if cansee($n)
           mpechoat $n wzrok+wid_i $I przygl`ada ci si`e podejrzliwie.
        endif
     endif
   endif
   if level($n) > 100
        uklon $n
        usm $n
   endif
   if goldamt($n) > 15000000
        if level($n) < 105
           if trust($n) < 108
             'O kurde! $N ma ponad pi`etna`scie baniek w z`locie!
             '$NW, mo`ze odpalisz troch`e dla mnie?
             if cansee($n)
                mpechoat $n wzrok Wiruje ci przed oczami, podczas gdy kat hipnotyzuje ci`e swo%{im toporem;imi skarpetkami;im wahade`lkiem}.
                mpechoaround $n wzrok+wid_i $I hipnotyzuje $ND swo%{im toporem;imi skarpetkami;im wahade`lkiem}.
                mpforce $n daj 14000000 mon kat
                mpechoat $n wzrok Nagle wracasz do normalnego `zycia jak gdyby nigdy nic si`e nie sta`lo.
             endif
           endif
        endif
   endif
*   if isfollow($n)
*       'Jeszcze kto`s ci`e musi prowadzi`c za r`aczk`e $NW?
*   endif
   if hitprcnt($i) < 6
        '$NW, pom`o`z, prosz`e, mnie biednemu, bo zdycham...
   else
     if hitprcnt($n) < 6
        'Mo`ze ci`e dobi`c, $NW?
	,u`smiecha si`e szyderczo.
     endif
   endif
endif
~
>death_prog 100~
'Umieram... O bogowie! Przebaczcie mi, `ze was zawiod`lem...
if isnpc($n)
   powiedz $n Ty wstr`etny mobie! Jak mog`le`s mnie zabi`c?
else
   '$NW! M`oj pogromca, jako najsilniejsza posta`c w Midgaardzie, musi zaj`a`c me miejsce na posterunku... Prosz`e...
endif
~
>rand_prog 1~
if rand(50)
  if !isobjeq(3005)
    ,ziewa przeci`agle
    break
  endif
  ,drapie si`e toporem po plecach
  break
  'Zeby bylo po rowno, powinien miec if rand(66), ale ja chce tak - Grz.
else
  if !isobjeq(3005)
    break
  endif
  '%{Nudy na pudy... trza co`s robi`c.;Ale tu nudno, chyba co`s porobi`e.;Musz`e co`s zrobi`c, bo umr`e z nud`ow.;Ile tak mo`zna sta`c i niczego nie robi`c?}
  ,wyjmuje szmatk`e i p`lynny wosk.
  ,nas`acza szmatk`e.
  zdej topor
  ,czy`sci %{sw`a bro`n;top`or;swojego najlepszego przyjaciela;ostre jak brzytwa ostrze topora}.
  'No! Teraz to jest top`or!
  zaloz topor
  usm
endif
~
>greet_prog 25~
if !cansee($n)
or !ispc($n)
 break
 'Tutaj moze krotkie info o tym greet_progu.
 'Otoz sprawdza on relacje doswiadczenia i poziomu gracza.
 'Glownie chce wyjasnic uzycie mpquest w tym dziwnym, zakreconym
 'progu, ktorego szanse zrozumiec ma chyba tylko Lam ;)
 'Tak wiec mpquest jest tylko po to, aby kat nie gratulowal
 'za kazdym razem, gdy gracz z rownym tysiacem dsw do niego podejdzie,
 'a tylko za pierwszym razem. Potem ustala questa nr 3009 graczowi i
 'pozniej widzi, ze gracz niby robi tego questa i nie gratuluje.
 'Tenze mpquest jest wylaczany (mpendquest) nizej, gdy gracz juz nie ma
 'okraglego doswiadczenia.
 'A po co tyle zachodu dla takiej bzdury mozecie spytac?
 'Po prostu to taki pierwszy test nowego kodu 1.4.-3.
 'Testuje oczywiscie Grzechu :)
 ' 2001 - musialem dodac warunek czy nie drugi poziom, bo ludzie po
 ' restarcie sie lapali na komunikat :/
endif
int chck = level($n) * 1000 - %(n.exp)
int minus_chck = chck * ( 1 - 2 )
if chck == 0
   if not level($n) > 99 || doingquest($n, 3009)
      if level($n) < 3
        break
      else
         pow $n Masz dok`ladnie tyle do`swiadczenia, co poziom * 1000. Brawo!
         ,gratuluje $NC tego, `ze ma dok`ladnie %(n.exp) do`swiadczenia!
         mpquest $n 3009 1
      endif
   endif
else
 if doingquest ($n, 3009)
  mpendquest $n 3009
 endif
 if minus_chck < 1000 && minus_chck >= 950
  'No, wspaniale, $NW. Zaraz zdob`edziesz poziom!
  pow $n Tylko uwa`zaj, nie zgi`n teraz, bo stracisz bardzo du`zo!
  break
 endif
 if minus_chck >= 1000
  pow $n A to dziwne, masz %(minus_chck) d`s wi`ecej, ni`z powiniene`s wg twego poziomu.
// Lam: to jest mozliwe w przypadku mordercow, ktorzy traca poziom za atak,
// z mniejsza utrata doswiadczenia
//  mpforce $n blad Mam %(n.exp) d`s, a tylko %(n.level) poziom!
  break
 endif
 if chck > 0 && chck <= 100
  'Pe`len tysi`ac ju`z tu`z tu`z, niestety poziomu tym razem nie b`edzie.
  usm $n
  break
 endif
 if chck > 100 && chck < 1000
    if sex($n)==2
     pow $n Chyba kilka razy ucieka`la`s z walki...?
    else
     pow $n Chyba kilka razy ucieka`le`s z walki...?
    endif
    break
 endif
 if chck >=1000
  if sex($n)==2
    pow $n Ile razy musia`la`s ucieka`c, `zeby straci`c a`z %(chck) do`swiadczenia?
  else
    pow $n Ile razy musia`le`s ucieka`c, `zeby straci`c a`z %(chck) do`swiadczenia?
  endif
 endif
endif
~
|
