20142314.out:20142314.o stringlist.o util.o
	g++ -o 20142314.out 20142314.o stringlist.o util.o
20142314.o:20142314.cpp
	g++ -c 20142314.cpp
stringlist.o:stringlist.cpp stringlist.h
	g++ -c stringlist.cpp
util.o:util.cpp util.h
	g++ -c util.cpp
clear:
	clear
clean:
	rm *.o 20142314.out