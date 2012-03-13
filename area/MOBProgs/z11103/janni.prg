* $Id: janni.prg,v 1.6 2006/03/01 18:19:06 lam Exp $

>greet_prog 100~
if not cansee($n)
or not doingquest($n,11103)
  break
endif
if queststage($n,11103)==3
or queststage($n,11103)==4
  , patrzy na ciebie z zaciekawieniem...
endif
~
>bribe_prog 5000~
if not cansee($n)
or not doingquest($n,11103)
  break
endif
if queststage($n,11103)==3
or queststage($n,11103)==4
  if sex($n)==2
    pow $n Od razu gdy wesz`la`s pomy`sla`lem, `ze to ciebie `zo`lnierz przys`la`l po paczk`e.
  else
    pow $n Od razu gdy wszed`le`s pomy`sla`lem, `ze to ciebie `zo`lnierz przys`la`l po paczk`e.
  endif
  mpoload 11133
  daj paczk $n
  pow $n Prosz`e, oto ona.
  mpquest $n 11103 5
  if isobjinv( 11133 )
    mpjunk paczka
  endif
endif
~
|
