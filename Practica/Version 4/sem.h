//Creamos un sem�foro con un valor inicial
int sCreate(int seed, int value);
 
//Operaciones de incremento y decremento del sem�foro
void sWait(int semID);
void sSignal(int semID);
