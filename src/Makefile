# Lam: standardowy Makefile przed configure'owaniem
# Lam 19.10.2001: automatyczne ./configure ; make
#                 zmiany dla GLACY
all:
	./configure ; make

glaca:
	./configure ; make glaca

love:
	@echo NOT WAR?

tgz:	mrproper
	cd .. ; tar czvf src.tar.gz src

tbz2:	mrproper
	cd .. ; tar cjvf src.tar.bz2 src

dos:
	make -f Makefile.dos

clean:
	rm -f *.o core *~ gla/*.o gla/*~ gla/*.bak

mrproper:
	rm -f *.o *.h.gch core *~ *.orig gla/*.o gla/*~ gla/*.bak gla/*.orig lac glaca lac.exe glaca.exe DEADJOE config.log config.status config.cache mingw32-config.cache
	cp -f Makefile.lac Makefile

