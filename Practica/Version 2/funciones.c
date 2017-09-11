/*ESQUELETO DEL FICHERO func.c donde se define el c�digo de las funciones a utilizar*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include "funciones.h"
 
extern int jugadores_contestados;
 
//MANEJADORES ------------------------------------------------
//PADRE - MANEJADOR PARA LAS SE�ALES EN TIEMPO REAL ------  
void HijoApuesta(int sig, siginfo_t *siginfo, void *context){
// Esta funci�n es el manejador de se�ales en tiempo real.
// Cada hijo enviar� al padre una se�al diciendo que ha apostado
// Aqu� contaremos cu�ntos hijos han contestado en una variable global que habr� que definir
//Cada vez que llega la se�al se aumenta en 1 el numero de jugadores contestados.
jugadores_contestados++;
}
 
//HIJOS - MANEJADOR DE SE�ALES NORMALES ------  
void SorteoRealizado (int sig, siginfo_t *siginfo, void *context){
//Esta funci�n es el manejador de la se�al que reciben los hijos para enterarse que el padre
//ya ha generado la combinaci�n ganadora, es decir, que se ha realizado el sorteo.
if (sig==SIGUSR1){ 
//Se�al recibida por el hijo
printf ("Hijo %d recibe ACK del padre usando la se�al %d.\n",getpid(),sig);
		}
}
 
//FUNCIONES  --------------------------------------------------
//FUNCIONES SOLO PADRE //////////////////////////////////////////////////////
int ValidarArgumentos(int argc, char** argv){
//Se podr�a utilizar esta funci�n para validar los argumentos que recibe el programa sorteo.
//Los par�metros que recibe son los que recibe el propio main.
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
//Esta funci�n sirve para que el padre, antes de crear los hijos y sabiendo cu�ntos son 
//Cree todas los pipes.
//Una forma es crear un array de pipes global (que heredar�n los hijos)
//}
 
//Funcion incluida directamente en el main 
//void ComunicarApuesta(int numHijo, unsigned int *ganadora){
//Esta funci�n servir� al padre para enviar la combinaci�n ganadora por el pipe correspondiente
//al hijo, siendo numHijo el �ndice del hijo en el array de tuber�as.	
//}
 
 
//FUNCIONES SOLO HIJOS //////////////////////////////////////////////////////

//Funcion incluida directamente en el main 
//void ApuestaRealizada(){
//Esta funci�n la utilzar�n los hijos para enviar una se�al al padre de forma que �ste
//sepa que el hijo ha realizado su apuesta.	
//}
 
int ComprobarCombinacion(unsigned int *ganadora, unsigned int *apuesta){
    
//Esta funci�n la usar�n los hijos para comprobar el n�mero de aciertos de su combinaci�n
int j,k,aciertos=0;

//Recorre los vectores de la apuesta y la combinacion ganadora, si son iguales el numero de aciertos se incrementara.
 for(j=0;j<6;j++)
  for(k=0;k<6;k++)
   if(apuesta[j]==ganadora[k]) {
        aciertos++;
        }
   
//La funci�n devuelve el n�mero de aciertos que tendr� el jugador	
return aciertos;
}
 
int CalcularPremio(int numAciertos){
//Esta funci�n servir� para calcular el importe del premio en funci�n de la cantidad de n�meros acertados
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
//Esta funci�n servir� al hijo para leer la combinaci�n ganadora de su pipe, que vendr� indicada
//por el n�mero de hijo numHijo que es el �ndice que el hijo tiene en el array de tuber�as.
//La combinaci�n ganadora se depositar� en el array ganadora.
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
//Esta funci�n la utilizar�n el padre y el hijo para generar la combinaci�n
//El padre generar� la combinaci�n ganadora y el hijo su apuesta.
int i,j;
//Hay que inicializar el generador de n�meros aleatorios con valores distintos para el padre y para cada hijo
srand (getpid());
//Vector de tama�o 6 con numeros enteros aleatorios entre 1-15.
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
