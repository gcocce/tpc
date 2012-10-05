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
		cout << "Padre: Espero que muera el generador.  " << getpid() << endl;
		wait(NULL);
		cout << "Padre: Hola, soy el proceso padre.  Mi process ID es " << getpid() << endl;

		exit ( 0 );
	}
}
