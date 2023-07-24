#to activate graphic mode :
#	uncomment the line below
	G = "GRAPHICS_ON"
#	do the same in Echeckisen.c and in Echeckisen.h before compilation






#executable
ifeq ($(G),"GRAPHICS_ON")

run: Echeckisen.o Utilities.o libisentlib.a
	@echo "\nCompiling in GRAPHIC mode :\n"
	gcc -Wall -o run Echeckisen.o Utilities.o libisentlib.a -O2 -lm -lglut -lGL -lX11
	rm -f *.o libisentlib.a

else

run: Echeckisen.o Utilities.o
	@echo "\nCompiling in CONSOLE mode :\n"
	gcc -Wall -o run Utilities.o Echeckisen.o -O2 -lpthread
	rm -f *.o libisentlib.a

endif






#echeckisen
Echeckisen.o: Echeckisen.c Echeckisen.h Utilities.h libisentlib.a
	gcc -Wall -c Echeckisen.c -O2

#utilities
Utilities.o: Utilities.c Utilities.h
	gcc -Wall -c Utilities.c -O2






#GfxLib libraries
libisentlib.a: ESLib.o GfxLib.o
	ar r libisentlib.a ESLib.o GfxLib.o
	ranlib libisentlib.a

ESLib.o: src/ESLib.c src/ESLib.h src/ErreurLib.h
	gcc -Wall -c src/ESLib.c -O2

GfxLib.o: src/GfxLib.c src/GfxLib.h src/ESLib.h
	gcc -Wall -c src/GfxLib.c -I/usr/include/GL -O2
