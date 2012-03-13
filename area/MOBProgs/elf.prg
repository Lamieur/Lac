* $Id: elf.prg,v 1.5 2006/03/01 18:19:06 lam Exp $

>greet_prog 100~
if race($n) == 'elf'
or race($n) == 'halfelf'
or !cantalk($n)
or isimmort($n)
or is_safe($n)
  break
endif
if sex($n) == 2
  krz $N wtargn`e`la do doliny!
  mpkill $n
  break
endif
if sex($n) == 1
  krz $N wtargn`a`l do doliny!
  mpkill $n
  break
endif
if sex($n) == 0
  krz $N wtargn`e`lo do doliny!
  mpkill $n
  break
endif
~
|
