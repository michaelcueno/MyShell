statsh: main.o helpers.o history.o
	g++ -o statsh main.o helpers.o history.o

main.o: main.cpp helpers.h history.h
	g++ -Wall -c main.cpp

helpers.o: helpers.cpp 
	g++ -Wall -c helpers.cpp

history.o: history.cpp
	g++ -Wall -c history.cpp

clean: 
	rm *.o statsh *.gch
