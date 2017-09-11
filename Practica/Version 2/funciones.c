/*ESQUELETO DEL FICHERO func.c donde se define el código de las funciones a utilizar*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include "funciones.h"
 
extern int jugadores_contestados;
 
//MANEJADORES ------------------------------------------------
//PADRE - MANEJADOR PARA LAS SEÑALES EN TIEMPO REAL ------  
void HijoApuesta(int sig, siginfo_t *siginfo, void *context){
// Esta función es el manejador de señales en tiempo real.
// Cada hijo enviará al padre una señal diciendo que ha apostado
// Aquí contaremos cuántos hijos han contestado en una variable global que habrá que definir
//Cada vez que llega la señal se aumenta en 1 el numero de jugadores contestados.
jugadores_contestados++;
}
 
//HIJOS - MANEJADOR DE SEÑALES NORMALES ------  
void SorteoRealizado (int sig, siginfo_t *siginfo, void *context){
//Esta función es el manejador de la señal que reciben los hijos para enterarse que el padre
//ya ha generado la combinación ganadora, es decir, que se ha realizado el sorteo.
if (sig==SIGUSR1){ 
//Señal recibida por el hijo
printf ("Hijo %d recibe ACK del padre usando la señal %d.\n",getpid(),sig);
		}
}
 
//FUNCIONES  --------------------------------------------------
//FUNCIONES SOLO PADRE //////////////////////////////////////////////////////
int ValidarArgumentos(int argc, char** argv){
//Se podría utilizar esta función para validar los argumentos que recibe el programa sorteo.
//Los parámetros que recibe son los que recibe el propio main.
//Controlara los posibles errores.
  if(argc != 3){     
    printf("Error a la hora de introducir el numero de parametros.Deben de ser 2\n");
	return -1;
  }else {
    int arg1= atoi(argv[1]);
    int arg2= atoi(argv[2]);
    if( arg1 < 1 || arg1 > 15){
      printf("Error a la hora de introducir el parametro de sorteos. min: 1 y max: 15\n");
	  return -1;
    }
    else if( arg2 < 1 || arg2 > 10){
      	printf("Error a la hora de introducir el parametro de jugadores. min: 1 y max: 10\n");
		return -1;
    }
  }
}

//Funcion incluida directamente en el main 
//void CrearPipes(int numHijos){
//Esta función sirve para que el padre, antes de crear los hijos y sabiendo cuántos son 
//Cree todas los pipes.
//Una forma es crear un array de pipes global (que heredarán los hijos)
//}
 
//Funcion incluida directamente en el main 
//void ComunicarApuesta(int numHijo, unsigned int *ganadora){
//Esta función servirá al padre para enviar la combinación ganadora por el pipe correspondiente
//al hijo, siendo numHijo el índice del hijo en el array de tuberías.	
//}
 
 
//FUNCIONES SOLO HIJOS //////////////////////////////////////////////////////

//Funcion incluida directamente en el main 
//void ApuestaRealizada(){
//Esta función la utilzarán los hijos para enviar una señal al padre de forma que éste
//sepa que el hijo ha realizado su apuesta.	
//}
 
int ComprobarCombinacion(unsigned int *ganadora, unsigned int *apuesta){
    
//Esta función la usarán los hijos para comprobar el número de aciertos de su combinación
int j,k,aciertos=0;

//Recorre los vectores de la apuesta y la combinacion ganadora, si son iguales el numero de aciertos se incrementara.
 for(j=0;j<6;j++)
  for(k=0;k<6;k++)
   if(apuesta[j]==ganadora[k]) {
        aciertos++;
        }
   
//La función devuelve el número de aciertos que tendrá el jugador	
return aciertos;
}
 
int CalcularPremio(int numAciertos){
//Esta función servirá para calcular el importe del premio en función de la cantidad de números acertados
//Devuelve el premio en euros
    int premio;

 if(numAciertos==6){
         premio=10000; 
        }
 
 else if (numAciertos==5){
        premio=500;
        } 
 else if (numAciertos==4){
        premio=50;
        } 
 else if (numAciertos==3){
        premio=10;
        } 
 else {
        premio=0;
        }
        
return premio;
}

//Funcion incluida directamente en el main  
//void LeerGanadora(int numHijo, unsigned int *ganadora){
//Esta función servirá al hijo para leer la combinación ganadora de su pipe, que vendrá indicada
//por el número de hijo numHijo que es el índice que el hijo tiene en el array de tuberías.
//La combinación ganadora se depositará en el array ganadora.
//}

//Funcion para ordenar un vector, la utilizaremos para ordenar las apuestas y la combinacion ganadora.
void ordenarApuesta(int *combinacion)
{
int i,j,aux;
for (i=0;i<6;i++)
 for (j=0;j<5;j++)
  if (combinacion[j]>combinacion[j+1]) 
   {
   aux=combinacion[j];
   combinacion[j]=combinacion[j+1];
   combinacion[j+1]=aux;
   }
}
 
//FUNCIONES COMUNES PADRE HIJOS
void GenerarCombinacion(unsigned int *combinacion){
//Esta función la utilizarán el padre y el hijo para generar la combinación
//El padre generará la combinación ganadora y el hijo su apuesta.
int i,j;
//Hay que inicializar el generador de números aleatorios con valores distintos para el padre y para cada hijo
srand (getpid());
//Vector de tamaño 6 con numeros enteros aleatorios entre 1-15.
for(i=0;i<6;i++) 
 {             
 combinacion[i] = rand() %15+1;
 for(j=0;j<6;j++)
 //Controla que no se repita el numero aleatorio, si se repite volvera a buscar otro numero nuevo.
  if(combinacion[i]==combinacion[j] && i!=j)
   {
        i--;
    }
 }
 //Ordena el vector con la apuesta de menor a mayor 
 ordenarApuesta(combinacion);	
}	

void fichero(int Numerosorteo, int jugador, int premios[])
{
FILE *f;
int i;
char s[5]="S";
sprintf(s,"S%dR", Numerosorteo);
//strcat(s,itoa(Numerosorteo));
//strcat(s,"R");

f = fopen (s,"w");

if(f==NULL) printf("\n* ERROR: No se pudo crear el fichero '%s'\n",s);
else
 {
 fprintf(f,"#Resultados Sorteo %s\n",Numerosorteo);

 for(i=0;i<jugador;i++)
  fprintf(f,"%li\n",premios[i]);
 }

if(fclose(f)) printf( "\n* ERROR: Fichero '%s' NO CERRADO\n",s);
}


void transformarTextoaNumero (char ganT[], int *ganN)
{
int i,j=0,k=0;
char T[2];

 for(i=0;i<strlen(ganT);i++)
  {
  if(ganT[i]!=' ')
   {
    T[k]=ganT[i];
    k++;
   }

   else
    {
     ganN[j]=atoi(T);
     j++;
     k=0;
    }
  }
}
