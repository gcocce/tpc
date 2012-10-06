//============================================================================
// Name        : tpc.cpp
// Version     :
// Description : Proceso principal
//============================================================================

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#include "genautos.h"

using namespace std;

void mostrar_ayuda(){
	cout << "Uso del programa:" << endl;
	cout << "comando tiempo lugares costo [opciones]" << endl;
	cout << "" << endl;
	cout << "  tiempo: duracion de la simulacion en minutos (entero)." << endl;
	cout << "  lugares: lugares disponibles en el estacionamiento (entero)." << endl;
	cout << "  costo: costo de la hora en el estacionamiento (decimal)." << endl;
	cout << "" << endl;
	cout << "opciones: -d  ejecucion en modo debug." << endl;
}

/* Parametros obligatorios
 *	1) Duración de la simulación (int)
 *	2) Cantidad de espacios en el estacionamiento (int)
 * 	3) Costo de la hora del estacionamiento (float)
 * 	Opcional: modo debug
*/
int main(int argc, char* argv[]){
	int tiempo=0;
	int cantidad=0;
	float costo=0;
	bool debug=false;

	// Sección dedicada a los parametros
	if (argc < 4 || argc > 5){
		mostrar_ayuda();
		exit ( 0 );
	}else{
		// TODO: comprobar que sean numeros
		tiempo=atoi(argv[1]);
		cantidad=atoi(argv[2]);
		costo=atof(argv[3]);
		if (argc==5){
			if (strcmp("-d",argv[5])==0){
				debug=true;
			}else{
				mostrar_ayuda();
				exit(0);
			}
		}
	}

	//Los parametros son válidos, entonces se inicia el programa
	cout << "Trabajo practico de concurrentes." << endl;
	if (debug){
		cout << "Modo Debug." << endl;
	}else{
		cout << "Modo Normal. " << endl;
	}


	int status=0;
	pid_t wpid;
	pid_t genid = fork ();
	if ( genid == 0 ) {
		// Lanzar el generador de autos
		int res = generarAutos();
		exit ( res );
	} else {
		cout << "Padre: Espero. Process id: " << getpid() << endl;
		sleep(tiempo);
		cout << "Padre: Envio SIGINT." << endl;
		int res= kill(genid,SIGINT);
		cout << "Padre: Resultado de la señal: " << res << endl;

		// Esperamos que finalice el generador de autos
		wpid = waitpid(genid, &status,0);

		cout << "Padre: Finalizó el generados con estado: "<< status << endl;
		cout << "Padre: FIN"<< endl;
		exit ( 0 );
	}
}

