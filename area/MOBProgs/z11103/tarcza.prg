* $Id: tarcza.prg,v 1.2 2006/03/01 18:19:06 lam Exp $

>dig_prog 100~
if doingquest($n,11103)
  if queststage($n,11103) == 9
    mpquest $n 11103 10
  endif
endif
~
|
