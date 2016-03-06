act_comm.o: act_comm.c merc.h config.h handler.h const.h comm.h
act_info.o: act_info.c merc.h config.h update.h interp.h const.h \
 act_info.h comm.h
act_move.o: act_move.c merc.h config.h handler.h const.h act_move.h
act_obj.o: act_obj.c merc.h config.h mp_wyzw.h magic.h handler.h const.h \
 act_info.h act_move.h comm.h
act_wiz.o: act_wiz.c merc.h config.h special.h mp_wyzw.h interp.h \
 handler.h const.h act_wiz.h comm.h
clan.o: clan.c merc.h config.h const.h comm.h
comm.o: comm.c merc.h config.h update.h const.h act_wiz.h comm.h
const.o: const.c merc.h config.h
db.o: db.c merc.h config.h ssm.h handler.h const.h act_wiz.h comm.h
fight.o: fight.c merc.h config.h update.h handler.h const.h comm.h
gry.o: gry.c merc.h config.h gry.h const.h
handler.o: handler.c merc.h config.h update.h mp_pol.h handler.h const.h \
 comm.h
httpd.o: httpd.c merc.h config.h interp.h const.h comm.h
hunt.o: hunt.c merc.h config.h const.h
interp.o: interp.c merc.h config.h interp.h
kzapisz.o: kzapisz.c merc.h config.h kzapisz.h const.h comm.h
lac.o: lac.c merc.h config.h
lanclicz.o: lanclicz.c merc.h config.h comm.h
magic.o: magic.c merc.h config.h magic.h handler.h const.h
mp_pol.o: mp_pol.c merc.h config.h mp_wyzw.h mp_pol.h magic.h handler.h \
 const.h
mp_wczyt.o: mp_wczyt.c merc.h config.h mp_wczyt.h mp_pol.h
mp_wyr.o: mp_wyr.c merc.h config.h const.h
mp_wyzw.o: mp_wyzw.c merc.h config.h mp_wyzw.h comm.h
nanny.o: nanny.c merc.h config.h const.h comm.h
necro.o: necro.c merc.h config.h magic.h handler.h const.h
palad.o: palad.c merc.h config.h handler.h const.h
qmagic.o: qmagic.c merc.h config.h magic.h handler.h const.h
save.o: save.c merc.h config.h const.h comm.h
screen.o: screen.c merc.h config.h
skr.o: skr.c merc.h config.h handler.h const.h
socjalne.o: socjalne.c merc.h config.h const.h
special.o: special.c merc.h config.h special.h const.h
ssm.o: ssm.c merc.h config.h ssm.h comm.h
um_ras.o: um_ras.c merc.h config.h const.h
update.o: update.c merc.h config.h update.h mp_pol.h handler.h const.h \
 act_move.h act_wiz.h comm.h
gla/combo.o: gla/combo.c gla/glaca.h merc.h config.h
gla/glaca.o: gla/glaca.c gla/glaca.h merc.h config.h mp_wczyt.h const.h comm.h
gla/kraina.o: gla/kraina.c gla/glaca.h merc.h config.h kzapisz.h
gla/lpl.o: gla/lpl.c gla/glaca.h merc.h config.h const.h
gla/mapa.o: gla/mapa.c gla/glaca.h merc.h config.h const.h
gla/mob.o: gla/mob.c gla/glaca.h merc.h config.h special.h handler.h gry.h \
 const.h
gla/pomieszczenie.o: gla/pomieszczenie.c gla/glaca.h merc.h config.h \
 handler.h const.h
gla/progi.o: gla/progi.c gla/glaca.h merc.h config.h mp_wczyt.h
gla/przedmiot.o: gla/przedmiot.c gla/glaca.h merc.h config.h handler.h const.h
gla/reset.o: gla/reset.c gla/glaca.h merc.h config.h kzapisz.h const.h
gla/vnum.o: gla/vnum.c gla/glaca.h merc.h config.h
gla/wyjscie.o: gla/wyjscie.c gla/glaca.h merc.h config.h handler.h const.h
