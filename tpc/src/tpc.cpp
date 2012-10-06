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

int main() {
	cout << "Trabajo practico de concurrentes." << endl;

	pid_t id = fork ();
	if ( id == 0 ) {
		// Lanzar el generador de autos
		int res = generarAutos();
		exit ( res );
	} else {
		// Esperamos que muera el generador de autos
		cout << "Padre: Espero. Process id: " << getpid() << endl;
		sleep(60);
		cout << "Padre: Envio SIGINT." << endl;
		int res= kill(id,SIGINT);
		cout << "Padre: Resultado: " << res << endl;
		//wait(NULL);
		cout << "Padre: Fin"<< endl;

		exit ( 0 );
	}
}
