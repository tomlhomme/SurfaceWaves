

app: main.o screen.o ./Numerica/array.o
	g++ -g screen.o main.o surface.o ./Numerica/array.o -o app -lSDL2main -lSDL2 -lm -lblas -llapacke -Ofast

./Numerica/array.o:
	cd ./Numerica && make objects && cd ..

main.o: main.cpp surface.o
	g++ -g -c main.cpp -o main.o -lSDL2main -lSDL2 -Wall -Ofast

surface.o: surface.cpp
	g++ -g -c surface.cpp -o surface.o -lSDL2main -lSDL2 -Wall -Ofast

screen.o: screen.cpp
	g++ -g -c screen.cpp -o screen.o -lSDL2main -lSDL2 -Wall -Ofast

clean: 
	cd ./Numerica && make clean && cd ..
	rm *.o app

