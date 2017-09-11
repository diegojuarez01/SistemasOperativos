#include <stdlib.h>
#include <stdio.h>
#include <time.h> // para srand
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include "funciones.h"
 
// DEFINES Y CONSTANTES GLOBALES
 
#define LEER 	 0
#define ESCRIBIR 1

int jugadores_contestados;

//*****************************************************************
//FUNCIÓN que envía el ACK al hijo
//*****************************************************************
void EnviarAck(int pid){
	//Enviamos el ACK al hijo.
	kill(pid,SIGUSR1); //usamos kill para enviar señales normales
	printf("Enviado ACK a hijo %d\n",pid);
}

int main(int argc, char *argv[]){
    
//Validara los parametros introducidos si todo esta bien seguira sino se mostrara mensaje de error.
ValidarArgumentos(argc,argv);    
    
//Se define la estructura para el manejador de las señales que recibe el padre
struct sigaction manejador;
 
//Se inicializa el contador de mensajes recibidos, que han sido enviados por los hijos al haber realizado su apuesta
manejador.sa_sigaction =  HijoApuesta;//Nombre de la funcion manejadora
manejador.sa_flags = SA_SIGINFO; //Esto lo definimos para enviar informacion adicional junto a la señal
sigaction(SIGRTMIN, &manejador , NULL); //Establecemos el manejador de la señal a la función manejadora
 
//Instanciamos en el mismo manejador la señal que usará el hijo en la estrucutra Action y registramos el manejador de esa señal
	manejador.sa_sigaction = SorteoRealizado;
	manejador.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &manejador, NULL); //Usaremos una señal normal 
 
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

//for(y=0;y<6;y++) 
//printf("\n apuesta del jugador %i (PID = %d) NÂº%i -> %i\n",i+1,getpid(),y+1,apuesta[i][y]);
			
	 //Comunicamos al padre que hemos apostado
	    //Enviamos una señal al padre con el pid del hijo indicando que se ha apostado
				value.sival_int = getpid(); //asignamos valor al dato
				sigqueue(getppid(),SIGRTMIN,value); //Usamos sigqueue para enviar señales en tiempo real
				
				close(descriptor[i][ESCRIBIR]);	
		 //Espera el aviso del padre de que la combinación ganadora ha sido generada
	       //pause y si recibimos la señal del padre seguimos.
	       	 //Leemos del Pipe, ganadoraT tamaño 18
	       	 
	       	  printf("Hijo %d esperando ACK\n", getpid());
		       pause();
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
//Esperamos a que todos los hijos apuesten
while(jugadores_contestados<numHijos) sleep(1); //el padre espera la confirmacion de apuesta de todos los hijos

//El padre genera la combinacion ganadora
GenerarCombinacion(combinacionGanadora);

//Enviamos los ACKS
	for (i=0; i<numHijos; i++)
		EnviarAck(pid[i]);
		
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
	
//Esperamos a la terminación de todos los hijos en un bucle 
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
