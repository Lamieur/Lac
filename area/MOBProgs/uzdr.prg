* $Id: uzdr.prg,v 1.19 2009/06/28 17:02:31 lam Exp $

>trig_prog przerwa~
int licznik = licznik + 1

if deleted( $n ) || !cansee( $n ) || inroom( $i ) != inroom( $n )
  if plecosla == 2
    'No! Dobrze, `ze mnie pos`lucha`la ta wied`xma!
  else
    'No! Dobrze, `ze pos`lucha`l mnie ten barbarzy`nca!
  endif
  int licznik = 0
  break
endif

if licznik == 6
 'Raz...
else
 if licznik == 11
  'Dwa...
 else
   if licznik == 16
     'Trzy!
     // Lam 3.6.2004: je`sli `spi, zobaczy, `ze uzdrowiciel go budzi,
     if !isawake( $n )
       obud`x $n
     endif
     'Ostrzega`lem! Kara ci`e nie minie!
     // a je`sli wypoczywa`l, sam musi wsta`c.
     if position( $n ) < 6
       mpforce $n wsta`n
     endif
     cz rozprosz $n
     mpechoat $n wzrok+wid_i $I rozkazuje ci zdj`a`c ca`ly sw`oj ekwipunek.
     mpechoat $n wzrok+wid_i Ton jego wypowiedzi sprawia, `ze wykonujesz polecenie w jednej chwili.
     mpechoaround $n wzrok+wid_i+wid_n $I rozkazuje $NC zdj`a`c wszystko z siebie.
     mpforce $n zde wsz
     mpechoat $n przyt Jakby pod wp`lywem hipnozy wysy`lasz wszystko do urny.
     mpforce $n wy`slij wsz
     mppolymorph $n osio`l 96
   endif
 endif
endif
if licznik < 16
  mptrigger sie 4 przerwa
else
  int licznik = 0
endif
~
>speech_prog *~
if !cansee( $n )
  break
endif
if !czybluzg( _argument )
  break
endif
if sex( $n ) == 2
 'Co za obrzydliwa $N!? Jak ona `smie tak si`e wyra`za`c w `swi`atyni?
else
 'Co za obrzydliwy $N!? Jak on `smie tak si`e wyra`za`c w `swi`atyni?
endif
if level( $n ) > 102
 md $n Taki dajesz przyk`lad z g`ory? Wstyd!
 break
endif
if !varexists( "licznik" )
 int licznik = 0
endif
if licznik != 0
  md $n Masz szcz`e`scie, `ze jestem teraz zaj`ety, nast`epnym razem uwa`zaj, bo po`za`lujesz!
  break
endif
int licznik = 1
int plecosla = sex( $n )
md $n Licz`e do trzech i ma ci`e tu nie by`c, bo inaczej po`za`lujesz!
mptrigger sie 4 przerwa
~
>give_prog all~
'Nie przyjmuj`e `zadnych %{`lap`owek;prezent`ow;podark`ow;podarunk`ow;dowod`ow wdzi`eczno`sci}!
mpjunk $o
// tu by`lo wk`ladanie do urn, zasadne i w og`ole, ale on i tak zmienia poziom,
// wi`ec urna odrzuci
~
>greet_prog 100~
if hitprcnt( $n ) < 8
  if zone( $n ) != zone( $i )
    break
  endif
 '%{Biedne dziecko;Biedactwo;Nieboraku}...
 c uzdrow $n
endif
~
>death_prog 100~
if !cansee( $n )
  break
endif
if isnpc( $n )
   powiedz $n Ty wstr`etny mobie! Jak mog`le`s mnie zabi`c?
else
   'To by`lo niez`le $NW, tylko kto teraz b`edzie rzuca`l czary?!
endif
~
>rand_prog 100~
// Nowy prog uzdrawiacza napisany przez Lama

// jesli nie bylo zmiennej (po jebucie, a przed smieceniem graczy)
if !varexists( "uzdrawiaczu_zabierz_to" )
  aint uzdrawiaczu_zabierz_to = 0
endif
if uzdrawiaczu_zabierz_to
  aint uzdrawiaczu_zabierz_to = 0
  if rand( 20 )
    'Oduczcie si`e wreszcie rzuca`c rzeczy pod moje nogi...
  endif
  wez wsz
// w Vergardzie urna u uzdrowiciela, w Midgaardzie obok
  if isobjhere( 10200 )
    wloz wsz #urna_na_dary
  else
    wysl wsz
  endif
// #346, uwaga - to zak`lada, `ze uzdrowiciel niczego przy sobie nie nosi!
  if carry_number( $i ) > 0
//    'Co`s si`e do mnie przyklei`lo!
    ,wznosi oczy ku niebu szepcz`ac s`lowa oczyszczaj`acego zakl`ecia.
    mpjunk all
  endif
endif
~
|
