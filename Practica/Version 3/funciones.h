/*ESQUELETO DEL FICHERO func.c donde se define el código de las funciones a utilizar*/
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

typedef struct {
    int pid ; // Pid del hijo
    int num ; // Numero de orden del hijo
    int num_aciertos; // Numero de aciertos.
    long premio; //En mi caso el premio se escribe directamente en el fichero.
} HIJO ;

int ValidarArgumentos(int argc, char** argv);
int ComprobarCombinacion(unsigned int *ganadora, unsigned int *apuesta);
int CalcularPremio(int numAciertos);
void GenerarCombinacion(unsigned int *combinacion);
void fichero(char [], int, int []);
void transformarTextoaNumero (char ganT[], int *ganN);
