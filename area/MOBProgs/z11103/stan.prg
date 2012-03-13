* $Id: stan.prg,v 1.7 2006/05/27 16:21:11 lam Exp $

>greet_prog 100~
if not cansee($n)
  break
endif
if queststage($n,11103)==6
  pow $n Widz`e, `ze przybywasz z daleka...
  , przy`swieca sobie latarni`a w poszukiwaniu robak`ow.
endif
if queststage($n,11103)==7
  pow $n To znowu Ty, $NW... Co tu robisz?
endif
~
>tell_prog latarnia latarni`e `xr`od`lo `swiat`la~
if not cansee($n)
  'Nie widz`e ci`e, ktosiu.
  break
endif
if not doingquest($n,11103)
   break
endif
if queststage($n,11103)==6
  mpoload 10860 45
  , spogl`ada na sw`a latarni`e z rozrzewnieniem.
  pow $n I oto latarnia...
  daj latar $n
  mpquest $n 11103 7
  break
endif
if queststage($n,11103)==7
  mpoload 10860 45
  , odchodzi na po`ludnie.
  , nadchodzi z po`ludnia.
  pow $n Uda`lo mi si`e odnale`x`c jeszcze jedn`a, ale to na pewno ostatnia. Uwa`zaj wi`ec na ni`a. Pozostaje mi `zyczy`c ci powodzenia.
  daj latar $n
  mpquest $n 11103 8
endif
~
>tell_prog dzi`ekuj`e~
if not cansee($n)
or not doingquest($n,11103)
or queststage($n,11103)!=8
  'Prosz`e bardzo.
endif
pow $n No! nareszcie jaki`s uprzejmy podr`o`znik. Wi`ecej takich nam potrzeba w dzisiejszych trudnych czasach.
usm $n
~
|