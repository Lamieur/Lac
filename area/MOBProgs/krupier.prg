* $Id: krupier.prg,v 1.2 2006/09/05 16:52:45 lam Exp $

>spell_victim_prog niewidzialno`s`c~
widzialno`s`c
mpecho wzrok+wid_i $I otrz`asa si`e z niewidzialno`sci.
// wszyscy krupierzy z tym progiem to faceci
if cansee( $n )
  mpechoat $n wzrok+wid_i $I spogl`ada na ciebie z oburzeniem.
  mpechoaround $n wzrok+wid_i+wid_n $I spogl`ada na $NB z oburzeniem.
  mpechoaround $n wzrok+wid_i+w_wid_n $I wygl`ada na oburzonego.
else
  mpecho wzrok+wid_i $I wygl`ada na oburzonego.
endif
~
|
