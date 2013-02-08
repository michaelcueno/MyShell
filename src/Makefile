mikes_shell: statsh.o helpers.o
	g++ -o statsh statsh.o helpers.o; rm *.o

statsh.o: statsh.h statsh.cpp
	g++ -Wall -c statsh.cpp

helpers.o: helpers.cpp statsh.h
	g++ -Wall -c helpers.cpp

clean: 
	rm *.o statsh
