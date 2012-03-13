* $Id: wieza.prg,v 1.7 2006/04/20 00:10:56 lam Exp $

>rand_prog 10~
if hour() > 5 && hour() < 20
  if !deleted( $r )
    if !savesspell( $r, 75 )
      mpechoaround $r wzrok+wid_r $R nagle znika!
      if !isawake( $r )
        mpechoat $r zawsze Wiercisz si`e przez sen.
        mptransfer $r 13273
      else
        mpechoat $r zawsze Nagle wszystko znika, a ty budzisz si`e na `l`ace!
        mptransfer $r 13273 p
      endif
      mpat $r mpechoaround $r wzrok+wid_r $R pojawia si`e nie wiadomo sk`ad!
    endif
  endif
endif
~
|
