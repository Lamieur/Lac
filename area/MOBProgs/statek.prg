* $Id: statek.prg,v 1.13 2009/02/28 09:47:58 lam Exp $

>time_prog 100~
if varexists("czy_tonie")
  if czy_tonie == 2
    mpecho przyt Woda z szumem wdar`la si`e na pok`lad poch`laniaj`ac wszystkich i wszystko.
    mpecho wzrok Na domiar z`lego, wyg`lodnia`le rekiny przyby`ly, by ucztowa`c twoim kosztem.
    mpecho zawsze {R`Smier`c przysz`la upomnie`c si`e o ciebie.{x
    mpmassslay
    mpecho zawsze Masz wielkie szcz`e`scie, `ze nie mo`zesz zgin`a`c.
    mpecho przyt Fale unosz`a ci`e do brzegu.
    mptransfer all 239 p
    mpecho wzrok Woda zmywa resztki masakry.
    mppurge
  endif
  if czy_tonie == 3
    mpecho wzrok Statek do po`lowy zanurzy`l si`e w wodzie!
    mpecho wzrok Teraz ju`z nie ma w`atpliwo`sci, toniemy!
  endif
  if czy_tonie == 4
    mpecho przyt Nag`ly wstrz`as zachwia`l ca`lym statkiem!
    mpecho przyt Nag`ly przechy`l daje ci powody do niepokoju, czy aby na pewno nic si`e nie sta`lo.
  endif
  if inroom( $i ) == 15122 && czy_tonie > 1
    aint czy_tonie = czy_tonie - 1
  endif
* `zeby ruszy`l po rozbiciu, musz`a si`e odnowi`c kapitan i sternik
  if inroom( $i ) == 15122 && czy_tonie == 1
    if varexists("kapitan_nie_zyje")
      if kapitan_nie_zyje == 1
	break
      endif
    endif
    if varexists("sternik_nie_zyje")
      if sternik_nie_zyje == 1
	break
      endif
    endif
    aint czy_tonie = czy_tonie - 1
  endif
endif

if inroom( $i ) != 15122
  break
endif

if varexists( "czy_tonie" )
  if czy_tonie != 0
    break
  endif
endif

if !varexists( "gdzie_statek" )
  aint gdzie_statek = 8
  break
endif

int szansa_na_rozbicie = 0
int nie_odbijaj = 0
aint maruderzy_wysiadac = 0

* modyfikatory pogodowe
if sky() == 2
  int szansa_na_rozbicie += 1
else
  if sky() == 3
    int szansa_na_rozbicie += 3
  endif
endif

* je`sli kto`s zabije kapitana
if varexists("kapitan_nie_zyje")
  if kapitan_nie_zyje == 1
    int szansa_na_rozbicie += 30
    int nie_odbijaj = 1
  endif
endif

* je`sli kto`s zabije sternika
if varexists("sternik_nie_zyje")
  if sternik_nie_zyje == 1
    int szansa_na_rozbicie += 60
    int nie_odbijaj = 1
  endif
endif

* czy wszystko to razem doprowadzi do katastrofy?
if random() <= szansa_na_rozbicie
  if gdzie_statek != 0 && gdzie_statek != 5 && gdzie_statek != 6 && gdzie_statek != 11
    aint czy_tonie = 4
    break
  endif
endif

* tu znajdzie si`e tylko, je`sli nie tonie, czyli stoi w porcie lub p`lynie.
if gdzie_statek == 0 && nie_odbijaj == 1
or gdzie_statek == 6 && nie_odbijaj == 1
  break
endif

* w`la`sciwy ruch statku
aint gdzie_statek = gdzie_statek + 1
if gdzie_statek == 12
  aint gdzie_statek = 0
endif

* dobijanie, odbijanie...
if gdzie_statek == 1
  mpgoto 3049
  mpdelexit s
  mpmakeexit s 3203 Widzisz rzek`e, p`lyn`ac`a na zach`od.
  mpecho wzrok Statek odbija od brzegu.
  if ismobhere( 3006 )
    mptrigger #3006 0 odbijanie_statku
  endif
  mpgoto 3203
  mpdelexit n
  mpmakeexit n 3049 Woda dobija do brzegu.
  mpecho wzrok Statek odbija od brzegu.
  mpgoto 15105
  mpdelexit n
  mpecho wzrok Statek odbija od brzegu.
endif

if gdzie_statek == 5
  mpgoto 17759
  mpmakeexit n 15107 Trap prowadzi na statek.
  mpecho wzrok Do brzegu dobija statek.
  mpgoto 15107
  mpmakeexit s 17759 Trap prowadzi na brzeg.
  mpecho wzrok Statek dobija do brzegu.
  aint maruderzy_wysiadac = 1
endif

if gdzie_statek == 7
  mpgoto 17759
  mpdelexit n
  mpecho wzrok Statek odbija od brzegu.
  if ismobhere( 17749 )
    mptrigger #17749 0 odbijanie_statku
  endif
  mpgoto 15107
  mpdelexit s
  mpecho wzrok Statek odbija od brzegu.
endif

if gdzie_statek == 11
  mpgoto 3049
  mpdelexit s
  mpmakeexit s 15105 Trap prowadzi na statek.
  mpecho wzrok Do brzegu dobija statek.
  mpgoto 3203
  mpdelexit n
  mpmakeexit n 0 Statek blokuje drog`e wodn`a.
  mpecho wzrok Statek dobija do brzegu, blokuj`ac drog`e wodn`a do przystani.
  mpgoto 15105
  mpmakeexit n 3049 Trap prowadzi na brzeg.
  mpecho wzrok Statek dobija do brzegu.
  aint maruderzy_wysiadac = 1
endif
~
|
