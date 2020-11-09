#to activate graphic mode :
#	uncomment the line below
	G = "GRAPHICS_ON"
#	do the same in Echeckisen.c and in Echeckisen.h before compilation



ifeq ($(G),"GRAPHICS_ON")

run: Echeckisen.o Utilities.o Utilities.h libisentlib.a
	gcc -Wall -o run Echeckisen.o Utilities.o libisentlib.a -lm -lglut -lGL -lX11

else

Echeckisen: Echeckisen.o Utilities.o Utilities.h
	gcc -Wall -o Echeckisen Utilities.o Echeckisen.o -lpthread

endif

Utilities.o: Utilities.c Utilities.h
	gcc -Wall -O2 -c Utilities.c

libisentlib.a: ESLib.o GfxLib.o
	ar r libisentlib.a ESLib.o GfxLib.o
	ranlib libisentlib.a

ESLib.o: ESLib.c ESLib.h ErreurLib.h
	gcc -Wall -O2 -c ESLib.c

GfxLib.o: GfxLib.c GfxLib.h ESLib.h
	gcc -Wall -O2 -c GfxLib.c -I/usr/include/GL

clean:
	rm -f *~ *.o

deepclean: clean
	rm -f Echeckisen libisentlib.a

