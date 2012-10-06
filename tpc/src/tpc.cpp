//============================================================================
// Name        : tpc.cpp
// Version     :
// Description : Proceso principal
//============================================================================

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#include "genautos.h"

using namespace std;

int main(int argc, char* argv[]){
	cout << "Trabajo practico de concurrentes." << endl;

	int status=0;
	pid_t wpid;
	pid_t genid = fork ();
	if ( genid == 0 ) {
		// Lanzar el generador de autos
		int res = generarAutos();
		exit ( res );
	} else {
		// Esperamos que muera el generador de autos
		cout << "Padre: Espero. Process id: " << getpid() << endl;
		sleep(60);
		cout << "Padre: Envio SIGINT." << endl;
		int res= kill(genid,SIGINT);
		cout << "Padre: Resultado de la señal: " << res << endl;

		// Esperamos que finalice el generador de autos
		wpid = waitpid(genid, &status,0);

		cout << "Padre: Fin"<< endl;
		exit ( 0 );
	}
}

void help(){
	cout << "Uso del programa." << endl;



}
