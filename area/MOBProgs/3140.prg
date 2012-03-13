* $Id: 3140.prg,v 1.7 2006/04/20 00:10:56 lam Exp $

>time_prog 100~
if hour( ) != 6
or isfight( $i )
or inroom( $i ) != 3142
  break
endif
if !isobjinv( 3137 )
  if !isaffected( $i, 0 )
    mpoload 3137 49
    mpecho wzrok+wid_i $I gmera w szufladzie biurka, po chwili wyci`agaj`ac co`s i szybko chowaj`ac.
  else
    break
  endif
endif
odklucz s
otworz s
s
if inroom( $i ) != 3143
  break
endif
'Min`e`la sz`osta, czas wypu`sci`c wi`e`xni`ow.
mptransfer all 3052
n
zamknij s
zaklucz s
~
|
