* $Id: tol2.prg,v 1.9 2006/04/20 00:10:56 lam Exp $

>undug_prog 100~
/Wg mnie spowoduje to taki efekt: Kopiesz, dostajesz komunikat,  ze
/nie znajdujesz nic - No bo pierscienia tam nie ma. A potem czytasz,
/ze jednak odkopales pierscien, a w dodatku lezy on na ziemi...
/Ja bym to zrobil tak, ze jak juz gosc zaczyna questa, to sie laduje
/gdzies pierscien z flaga BURIED i po prostu jak ktos toto odkopie,
/to dopiero burmistrz sprawdzi czy ktos robi questa i bedzie ok.
/Ale ok, sprobujemy w ten sposob... moze sie uda :)
// Komentarz Lama: to niby slabo wyglada, ale restart gry ani inny gracz
// nie zepsuja zadania.
if not doingquest( $n, 17702 )
or queststage( $n, 17702 ) != 1
or isblind( $n )
  break
endif

// oslepiony nie odkopie, wiec pozwalam sobie na wyswietlanie "zawsze"
int num_p = %(n.room.vnum)
if questdata0($n, 17702) == num_p
  mpechoat $n zawsze .
  mpechoat $n zawsze {MNie poddajesz si`e tak `latwo i z wigorem kontynuujesz kopanie...{x
  mpechoat $n zawsze W ko`ncu odkopujesz jaki`s {Ypier`scie`n{x!
  mpechoat $n zawsze Wpatrujesz si`e z podziwem na t`e pi`ekn`a robot`e krasnoludzkich mistrz`ow.
  mpechoat $n zawsze Lepiej go podnie`s, bo kto`s got`ow odebra`c ci znalezisko!
  mpoload 18006
  mpdrop piers
  mpquest $n 17702 2
  mpquestdescr $n 17702 Burmistrz Vaevictus poprosi`l ci`e o odnalezienie pami`atkowego pier`scienia.
if sex($n) == 2
  mpquestdescr $n 17702 + Teraz, gdy ju`z go odnalaz`la`s, mo`zesz go odnie`s`c burmistrzowi.
else
  mpquestdescr $n 17702 + Teraz, gdy ju`z go odnalaz`le`s, mo`zesz go odnie`s`c burmistrzowi.
endif
endif
~
|
