* $Id: helm2.prg,v 1.7 2006/04/20 00:10:56 lam Exp $

>greet_prog 100~
if doingquest($n, 17708) && ( queststage($n, 17708) == 1 )
   if ( inroom($n) == questdata0($n, 17708) ) && ( !isobjhere(3914) )
      mpoload 3914
      mpdrop helm
      mpechoat $n wzrok .
      mpechoat $n wzrok Nagle na swej drodze spostrzegasz jaki`s he`lm.
   endif
endif
~
|
