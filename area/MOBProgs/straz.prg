* $Id: straz.prg,v 1.6 2006/03/01 18:19:06 lam Exp $

>all_greet_prog 100~
if isimmort($n)
 break
endif
if hour() >= 22
or hour() <= 5
  if race($n) == "orc"
    if not cansee($n)
     'Czuj`e tu smr`od jakiego`s orka! Gdzie jeste`s, `smieciu?
    else
     'Przekl`ety orku!
     'Wiesz dobrze, `ze o tej porze nie wolno ci chodzi`c po mie`scie!
     'Za kar`e l`adujesz w areszcie.
     mptransfer $n 3143 p
    endif
  endif
  if race($n) == "troll"
    if not cansee($n)
     'Czuj`e tu smr`od jakiego`s trolla! Gdzie jeste`s, `smieciu?
    else
     'Przekl`ety trollu!
     'Wiesz dobrze `ze o tej porze nie wolno ci chodzi`c po mie`scie!
     'Za kar`e l`adujesz w areszcie.
     mptransfer $n 3143 p
    endif
  endif
endif
~
|
