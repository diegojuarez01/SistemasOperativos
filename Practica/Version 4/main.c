#include <stdlib.h>
#include <stdio.h>
#include <time.h> // para srand
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "funciones.h"
#include "sem.h"

// DEFINES Y CONSTANTES GLOBALES

#define SSEED 99
#define LEER 	 0
#define ESCRIBIR 1

int jugadores_contestados;
int main(int argc, char *argv[]){

//Validara los parametros introducidos si todo esta bien seguira sino se mostrara mensaje de error.
ValidarArgumentos(argc,argv);

//Declaración e inicialización de otras variables globales


	int i,numHijos,sorteos;
	char combinacionGanadoraTexto[18]="";
	char T[2];
	int combinacionGanadora[6]; //Vector donde se guardara la combinacion ganadora.
	int apuestaHijo[6];
	int aciertos[10]; //Vector de 10 que guardara el numero de aciertos de cada jugador.
	int premios[10];
	int descriptor[10][2]; //Descriptores de archivo para lectura/escritura en el pipe lectura 0 escritura 1.
    int pid[10];    //10 jugadores/hijos.


//Declaración e inicialización de los semáforos usados para la sincronización,
//un semáforo que actuará como barrera compartida entre padre-hijos.

//Declaramos el identificador del semáforo que juega el rol de barrera del padre
	int sBarreraPadre;
	//Creamos el semáforo inicializado a cero
	sBarreraPadre=sCreate(SSEED,0);

	sorteos=atoi(argv[1]);	 // parametro 1 numero de sorteo

	numHijos=atoi(argv[2]); // parametro 2 numero de jugadores

 	HIJO *hijo;
	hijo = (HIJO*)malloc(numHijos*sizeof(HIJO)); //reserva memoria para hijo

//Creamos el pipe o los pipes y controlamos que se haya creado correctamente.
//Determinar el protocolo a usar tanto si empleamos un único pipe para todos los hijos o un pipe por hijo


//Creamos las estrucutras hijos
HIJO hijos[numHijos]; // Declaramos el vector de estructuras HIJO

for(i=0; i<numHijos;i++){ //Inicializamos las estructuras de hijos
  hijos[i].pid=0;
  hijos[i].num=0;
  hijos[i].premio=0;

  //Añadimos el pipe correspondiente al hijo a la estructura si hemos decidido crear un pipe por hijo
  // hijos[i].pipe(fd[i])=0;
}

//Creamos el bucle principal donde se llama a fork en cada iteración del padre.
//Los hijos hacen exit en su código de forma que no iteran.
for(i=0;i<numHijos;i++){

    pipe(descriptor[i]); //Se crea un pipe

     //Código del padre
	 //mantenimiento de la estructura de hijos
             pid[i] = fork(); //el proceso padre crea un proceso hijo

			if (pid[i] == -1) //error al crear el proceso hijo
				{
				printf ("No se ha podido crear el jugador %d\n",i);
				exit(1);
				}

	       //Código del hijo
			if (pid[i] == 0) //El proceso hijo
				{
            //Generamos la apuesta del hijo.
				GenerarCombinacion(apuestaHijo);

 //Señalizamos la barrera que compartimos con el padre
        //El hijo indica que ha llegado a la barrera
		   sSignal(sBarreraPadre);

				close(descriptor[i][ESCRIBIR]);
		      	read(descriptor[i][LEER], combinacionGanadoraTexto, 18);
				close(descriptor[i][LEER]);
				transformarTextoaNumero(combinacionGanadoraTexto,combinacionGanadora);
				//Calculamos los aciertos para cada hijo(jugador).
				aciertos[i]=ComprobarCombinacion(apuestaHijo,combinacionGanadora);
//printf ("\n--- Jugador %d termina de comprobar su apuesta\n",i+1);

         //Salimos comunicando aciertos
				exit(aciertos[i]);
				}

				else //continuacion del proceso padre
					{
						hijo[i].pid=pid[i];
						hijo[i].num=i+1;
					}
}

//Este código sólo lo ejecuta el padre tras todas las iteraciones...
 //El padre espera a que todos los hijos lleguen a la barrera (que todos los hijos apuesten).
	for (i=0; i<numHijos; i++){
		sWait(sBarreraPadre);
		}

//El padre genera la combinacion ganadora
GenerarCombinacion(combinacionGanadora);

for(i=0;i<6;i++)
	{
		sprintf(T, "%d", combinacionGanadora[i]); ///Transforma la combinacion ganadora de entero a texto
		strcat(combinacionGanadoraTexto,T);
		strcat(combinacionGanadoraTexto," ");
	}

//Escribimos la combinación ganadora en el/los pipes
for(i=0;i<numHijos;i++)
	{
		close(descriptor[i][LEER]);
		write(descriptor[i][ESCRIBIR], combinacionGanadoraTexto, strlen(combinacionGanadoraTexto)); //el padre escribe la combinacion ganadora en el pipe
		close(descriptor[i][ESCRIBIR]);
	}


//Esperamos a que todos los hijos lleguen a su barrera, lo que indica que han apostado
	//Obenenemos el valor de retorno de cada hijo y calculamos el importe de su premio
	for(i=0;i<numHijos;i++)
	{
		waitpid(pid[i],&aciertos[i],0);    //El padre espera la terminacion de todos los hijos
		aciertos[i] = WEXITSTATUS(aciertos[i]);
		hijo[i].num_aciertos=aciertos[i];
		hijo[i].premio=CalcularPremio(aciertos[i]);
		premios[i]=hijo[i].premio;
	}

//Generamos el fichero de resultados del sorteo en curso y salimos
fichero(sorteos,numHijos,premios);
exit(1);
}
