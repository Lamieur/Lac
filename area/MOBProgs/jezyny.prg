* $Id: jezyny.prg,v 1.4 2006/04/20 00:10:56 lam Exp $

>do_prog - zerwij~
if isblind( $n )
  mpechoat $n zawsze Nie mo`zesz tego odnale`x`c.
else
  if ( can_carry_n( $n ) > carry_number( $n ) ) && ( can_carry_w( $n ) > carry_weight( $n ) )
    mpechoat $n zawsze Zrywasz je`zyn`e.
    mpechoaround $n wzrok+wid_n $N zrywa je`zyn`e.
    mpoload 14211 1
    mpgive je`zyna $n
  else
    mpechoat $n zawsze Nie uniesiesz tego.
  endif
endif
~
>do_prog - zrywaj~
if isblind( $n )
  mpechoat $n zawsze Nie mo`zesz tego odnale`x`c.
else
  if ( can_carry_n( $n ) > carry_number( $n ) ) && ( can_carry_w( $n ) > carry_weight( $n ) )
    mpechoat $n zawsze Zrywasz je`zyn`e.
    mpechoaround $n wzrok+wid_n $N zrywa je`zyn`e.
    mpoload 14211 1
    mpgive je`zyna $n
  else
    mpechoat $n zawsze Nie uniesiesz tego.
  endif
endif
~
|
