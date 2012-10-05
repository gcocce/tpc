/*
 * cargenerator.cpp
 *
 *  Created on: 04/10/2012
 *      Author: gk
 */

#include "genautos.h"
#include "auto.h"

using namespace std;

int generarAutos(){
	bool continuar=true;
	int i=0;

	cout << "Generador de Autos: Hola, soy el generador.  Mi process ID es " << getpid() << endl;

	while (continuar==true){
		// Se crea un auto
		pid_t id = fork ();
		if ( id == 0 ) {
			// Auto creado
			int res = manejarAuto();
			exit (res);
		} else {
			//wait(NULL);
			sleep(1);
			i++;
			if (i==30) continuar=false;
		}
	}

	i=0;
	while (i<30){
		cout << "Generador de Autos: Espero por un auto " << i << endl;
		wait(NULL);
		i++;
	}

	return 0;
}
