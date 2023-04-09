#to activate graphic mode :
#	uncomment the line below
	G = "GRAPHICS_ON"
#	do the same in Echeckisen.c before compilation




#executable & program
ifeq ($(G),"GRAPHICS_ON")

run: Echeckisen.o Utilities.o S2DE.o files.o
	@echo "\nCompiling in GRAPHIC mode :\n"
	gcc -Wall -o run Echeckisen.o Utilities.o S2DE.o files.o -O2 -lm -lglut -lGL -lX11
	rm -f *.o

Echeckisen.o: Echeckisen.c Utilities.h src/files.h src/S2DE.h src/img.c
	gcc -Wall -c Echeckisen.c -O2 -I/usr/include/GL

else

run: Echeckisen.o Utilities.o files.o
	@echo "\nCompiling in CONSOLE mode :\n"
	gcc -Wall -o run Utilities.o Echeckisen.o files.o -O2 -lpthread
	rm -f *.o

Echeckisen.o: Echeckisen.c Utilities.h src/files.h
	gcc -Wall -c Echeckisen.c -O2

endif




#utilities
Utilities.o: Utilities.c Utilities.h
	gcc -Wall -c Utilities.c -O2

#graphic library
S2DE.o: src/S2DE.c src/S2DE.h
	gcc -Wall -c src/S2DE.c -I/usr/include/GL -O2

#file library
files.o: src/files.c src/files.h
	gcc -Wall -c src/files.c -O2
