* $Id: mahntor.prg,v 1.8 2006/08/04 10:07:31 lam Exp $

>greet_prog 100~
if ispc($n)
  'Witaj w moim kr`olestwie, $NW.
endif
~
>death_prog 100~
if not doingquest($n,600)
or queststage($n,600) != 1
  'Umieram! Nie zapomnijcie o mnie, bogowie!
else
  if cansee($n)
    'Zosta`lem zabity przez $NB! To niemo`zliwe... aaargh...
  else
    'Zosta`lem zabity! To niemo`zliwe... aaargh...
  endif
  mpquest $n 600 2
endif
~
|
