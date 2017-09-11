CC=gcc
CFLAGS=-I.
INC = funciones.h
OBJECTS= main.o funciones.o

sorteo: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o sorteo

sorteo.o: main.c $(INC)
	$(CC) -c -o main.o main.c $(CFLAGS)

lib_sorteo.o: funciones.c $(INC)
	$(CC) -c -o funciones.o  funciones.c $(CFLAGS)

clean:
	rm -i sorteo $(OBJECTS)
