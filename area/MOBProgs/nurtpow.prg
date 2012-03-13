* $Id: nurtpow.prg,v 1.6 2006/04/20 00:10:56 lam Exp $

>pass_prog 100~
if isflying( $n )
  mpechoat $n zawsze Porywa ci`e bardzo silny wiatr!
  if sex( $n ) == 2
    mpechoaround $n wzrok+wid_n $N udaje si`e na po`ludnie, gdy nagle porywa j`a wiatr!
  else
    mpechoaround $n wzrok+wid_n $N udaje si`e na po`ludnie, gdy nagle porywa go wiatr!
  endif
else
  mpechoat $n zawsze Porywa ci`e nurt wody!
  if sex( $n ) == 2
    mpechoaround $n wzrok+wid_n $N udaje si`e na po`ludnie, gdzie porywa j`a nurt wody!
  else
    mpechoaround $n wzrok+wid_n $N udaje si`e na po`ludnie, gdzie porywa go nurt wody!
  endif
endif
mptransfer $n 6498 p
mplag $n 4
mpgoto 1834
chr duch = mload( 1800 )
mptrigger duch 3 nurtpow
~
|

prog realizuje wpadanie w pr`ad p`o`lnocny. Lam 26.3.2005
