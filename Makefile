main: clean glad.o main.o
	g++ -o main main.o glad.o -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl
	rm -rf glad.o main.o

main.o: main.cpp
	g++ -c main.cpp

glad.o: glad.c
	gcc -c glad.c
clean:
	rm -rf glad.o main.o main
