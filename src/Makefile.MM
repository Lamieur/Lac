act_comm.o: act_comm.c merc.h config.h handler.h const.h comm.h db.h
act_info.o: act_info.c merc.h config.h update.h interp.h const.h \
 act_info.h comm.h db.h
act_move.o: act_move.c merc.h config.h handler.h const.h act_move.h db.h
act_obj.o: act_obj.c merc.h config.h mp_wyzw.h magic.h handler.h const.h \
 act_info.h act_move.h comm.h db.h
act_wiz.o: act_wiz.c merc.h config.h special.h mp_wyzw.h interp.h \
 handler.h const.h act_wiz.h comm.h db.h
clan.o: clan.c merc.h config.h const.h comm.h db.h
comm.o: comm.c merc.h config.h update.h const.h act_wiz.h comm.h db.h
const.o: const.c merc.h config.h db.h
db.o: db.c merc.h config.h ssm.h handler.h const.h act_wiz.h comm.h db.h
fight.o: fight.c merc.h config.h update.h handler.h const.h comm.h db.h
gry.o: gry.c merc.h config.h gry.h const.h db.h
handler.o: handler.c merc.h config.h update.h mp_pol.h handler.h const.h \
 comm.h db.h
httpd.o: httpd.c merc.h config.h interp.h const.h comm.h db.h
hunt.o: hunt.c merc.h config.h const.h db.h
interp.o: interp.c merc.h config.h interp.h db.h
kzapisz.o: kzapisz.c merc.h config.h kzapisz.h const.h comm.h db.h
lac.o: lac.c merc.h config.h
lanclicz.o: lanclicz.c merc.h config.h comm.h db.h
magic.o: magic.c merc.h config.h magic.h handler.h const.h db.h
mp_pol.o: mp_pol.c merc.h config.h mp_wyzw.h mp_pol.h magic.h handler.h \
 const.h db.h
mp_wczyt.o: mp_wczyt.c merc.h config.h mp_wczyt.h mp_pol.h db.h
mp_wyr.o: mp_wyr.c merc.h config.h const.h comm.h db.h
mp_wyzw.o: mp_wyzw.c merc.h config.h mp_wyzw.h comm.h db.h
nanny.o: nanny.c merc.h config.h const.h comm.h db.h
necro.o: necro.c merc.h config.h magic.h handler.h const.h db.h
palad.o: palad.c merc.h config.h handler.h const.h db.h
qmagic.o: qmagic.c merc.h config.h magic.h handler.h const.h db.h
save.o: save.c merc.h config.h const.h comm.h db.h
screen.o: screen.c merc.h config.h db.h
skr.o: skr.c merc.h config.h handler.h const.h db.h
socjalne.o: socjalne.c merc.h config.h const.h db.h
special.o: special.c merc.h config.h special.h const.h db.h
ssm.o: ssm.c merc.h config.h ssm.h comm.h db.h
um_ras.o: um_ras.c merc.h config.h const.h db.h
update.o: update.c merc.h config.h update.h mp_pol.h handler.h const.h \
 act_move.h act_wiz.h comm.h db.h
gla/combo.o: gla/combo.c gla/glaca.h merc.h config.h
gla/glaca.o: gla/glaca.c gla/glaca.h merc.h config.h mp_wczyt.h const.h comm.h db.h
gla/kraina.o: gla/kraina.c gla/glaca.h merc.h config.h kzapisz.h db.h
gla/lpl.o: gla/lpl.c gla/glaca.h merc.h config.h const.h
gla/mapa.o: gla/mapa.c gla/glaca.h merc.h config.h const.h
gla/mob.o: gla/mob.c gla/glaca.h merc.h config.h special.h handler.h gry.h \
 const.h db.h
gla/pomieszczenie.o: gla/pomieszczenie.c gla/glaca.h merc.h config.h \
 handler.h const.h db.h
gla/progi.o: gla/progi.c gla/glaca.h merc.h config.h mp_wczyt.h
gla/przedmiot.o: gla/przedmiot.c gla/glaca.h merc.h config.h handler.h const.h db.h
gla/reset.o: gla/reset.c gla/glaca.h merc.h config.h kzapisz.h const.h db.h
gla/vnum.o: gla/vnum.c gla/glaca.h merc.h config.h
gla/wyjscie.o: gla/wyjscie.c gla/glaca.h merc.h config.h handler.h const.h
