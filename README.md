Lac 1.4
=======

Witaj w publicznym repozytorium kodu Laca!

Lac jest polskim mudem, dostępnym pod adresem [http://lac.pl/](http://lac.pl/). W tym miejscu publikujemy jego pełne źródła wraz z minimalnym zestawem plików potrzebnych do uruchomienia przykładowego muda.


Licencja
--------

Lac objęty jest licencjami Diku, Merc i Envy, ze swojej strony dokładając tylko dwupunktową licencję w stylu BSD. Plik license.lac mówi dokładnie to samo, tylko że po angielsku i uproszczonym językiem prawniczym.


Pierwsze uruchomienie
---------------------

Po ściągnięciu źródeł lub sklonowaniu repozytorium, musisz skompilować serwer. Robi się to w kilku prostych krokach:


### W systemie Windows:

1. Zainstaluj [Microsoft Visual C++ Express Edition](http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-cpp-express) lub [Pelles C IDE](http://www.pellesc.de/) lub [Dev-C++](http://sourceforge.net/projects/orwelldevcpp/). Wszystkie te narzędzia są darmowe.

2. Z katalogu MSVC uruchom _Lac.sln_. Dla Pellesa jest to _Lac.ppj_ w katalogu PellesC. W przypadku Dev-C++ jest to katalog Dev-Cpp, plik _Lac.dev_.

3. W menu wybierz _Debug_, a w nim _Start without debugging_. W Pellesie jest to odpowiednio: _Project_ i _Execute Lac.exe_. Jeśli używasz Dev-C++, z menu wybierz _Execute_, a potem _Compile & Run_.


### W systemach uniksopodobnych:

1. `cd src`

2. `./configure`

3. `make`

4. `./start`


Znane problemy:
---------------

* "./configure" nie może znaleźć kompilatora

  Zainstaluj kompilator w systemie. Zalecamy aktualną wersję gcc.

* Chcę skompilować Laca innym kompilatorem niż gcc.

  Przed uruchomieniem ./configure ustaw zmienną CC na odpowiedni kompilator, na przykład jeśli chcesz użyć systemowego "cc", wpisz:

  (dla sh) `export CC=cc`  
  (dla csh) `setenv CC cc`

* "make" zgłasza problem w Makefile.cmn, linii 24

  Użyj GNU make (wykonując polecenie "gmake" zamiast "make", o ile jest zainstalowany równolegle).

  Możesz też po prostu usunąć tę linię z pliku Makefile.cmn - jest ona wymagana do kompilacji Glacy, ale sam Lac jej nie wymaga.

* "./configure" kończy się błędami:

    `config.status: creating Makefile`  
    `awk: string too long near line 37`  
    `awk: syntax error near line 37`  
    `awk: illegal statement near line 37`  
    `awk: newline in string near line 37`  
    `config.status: error: could not create Makefile`

  Prawdopodobnie używasz niezgodnej wersji AWK. Przed ponownym uruchomieniem ./configure, ustaw zmienną AWK na gawk:

  (dla sh) `export AWK=gawk`  
  (dla csh) `setenv AWK gawk`

  Jeśli nie posiadasz gawk, musisz go zainstalować.

* Kompilacja MinGW32 kończy się powodzeniem, ale pojawia się błąd, że nie można znaleźć pliku.

  Uważamy, że jest to błąd tej konkretnej wersji programu Make. Ponieważ Lac działa, pozostaje nam to zignorować.
