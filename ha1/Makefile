all: matrices

matrices: main.o matrices.o
	g++ -std=c++11 main.o matrices.o -o matrices

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

matrices.o: matrices.cpp
	g++ -std=c++11 -c matrices.cpp

clean:
	rm -rf *.o matrices
