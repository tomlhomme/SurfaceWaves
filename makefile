

app: main.o screen.o
	g++ -g screen.o main.o -o app -lSDL2main -lSDL2 -O3


main.o: main.cpp
	g++ -g -c main.cpp -o main.o -lSDL2main -lSDL2 -O3

screen.o: screen.cpp
	g++ -g -c screen.cpp -o screen.o -lSDL2main -lSDL2 -O3

clean: 
	rm *.o app
