main: clean glad.o main.o shader.o
	g++ -o main main.o glad.o shader.o -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl
	rm -rf glad.o main.o

main.o: main.cpp
	g++ -c main.cpp

shader.o: shader.cpp
	g++ -c shader.cpp

glad.o: glad.c
	gcc -c glad.c
clean:
	rm -rf glad.o main.o main shader.o
