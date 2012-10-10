//============================================================================
// Name        : tpc.cpp
// Version     :
// Description : Proceso principal
//============================================================================

#include <iostream>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#include "genautos.h"
#include "ventanilla_entrada.h"
#include "ventanilla_salida.h"

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
		// Comprobamos que los parametros sean numericos.
		bool param_error=false;
		if(isdigit(*argv[1])){
			tiempo=atoi(argv[1]);
			if (tiempo==0){
				cout << "Error: el tiempo de la simulacion no puede ser cero." << endl;
				param_error=true;
			}
		}else{
			param_error=true;
			cout << "Error: El primer argumento debe ser numerico." << endl;
		}
		if(isdigit(*argv[2])){
			cantidad=atoi(argv[2]);
		}else{
			param_error=true;
			cout << "Error: El segundo argumento debe ser numerico." << endl;
		}
		if(isdigit(*argv[3])){
			costo=atof(argv[3]);
		}else{
			param_error=true;
			cout << "Error: El tercer argumento debe ser numerico." << endl;
		}
		if (param_error){
			mostrar_ayuda();
			exit(0);
		}
		// Comprobamos si está la opción del modo debug
		if (argc==5){
			if (strcmp("-d",argv[4])==0){
				debug=true;
			}else{
				cout << "Error: El cuarto argumento debe ser: -d" << endl;
				mostrar_ayuda();
				exit(0);
			}
		}
	}

	//Los parametros son válidos, entonces se inicia el programa
	cout << "Trabajo practico de concurrentes..." << endl;
	if (debug){
		cout << "Modo Debug." << endl;
		// TODO: Crear el objeto LOG.
	}else{
		cout << "Modo Normal. " << endl;
	}

	// Se inician los procesos de las ventanillas
	int status=0;
	pid_t wpid;
	pid_t vent_1, vent_2, vent_3, vent_4, vent_5;
	vent_1= fork();
	if ( vent_1 == 0 ) {
		// Proceso ventanilla 1
		int res=m_ventanilla_entrada(1);
		exit(res);
	}else{
		vent_2=fork();
		if(vent_2==0){
			// Proceso ventanilla 2
			int res=m_ventanilla_entrada(2);
			exit(res);
		}else{
			vent_3=fork();
			if (vent_3==0){
				// Proceso ventanilla 3
				int res=m_ventanilla_entrada(3);
				exit(res);
			}else{
				vent_4=fork();
				if (vent_4==0){
					// Proceso ventanilla 4
					int res=m_ventanilla_salida(4);
					exit (res);
				}else{
					vent_5=fork();
					if(vent_5==0){
						// Proceso ventanilla 5
						int res=m_ventanilla_salida(5);
						exit(res);
					}else{
						// Proceso padre continua más abajo


					}
				}
			}
		}
	}

	// El proceso principal continua por aquí.
	// Se inicia el proceso del generador de autos
	pid_t genid = fork ();
	if ( genid == 0 ) {
		// Lanzar el generador de autos
		int res = generarAutos();
		exit ( res );
	} else {
		cout << "Padre: Espero. Process id= " << getpid() << endl;
		sleep(tiempo);
		cout << "Padre: Envio SIGINT al generador de autos." << endl;
		int res= kill(genid,SIGINT);
		cout << "Padre: Resultado de la señal= " << res << endl;

		// Esperamos que finalice el generador de autos
		wpid = waitpid(genid, &status,0);
		cout << "Padre: Finalizó el generador de autos con estado= "<< status << endl;

		// Cerramos las ventanillas
		cout << "Padre: Envio SIGINT a las ventanillas." << endl;
		int res_v1 = kill(vent_1,SIGINT);
		int res_v2 = kill(vent_2,SIGINT);
		int res_v3 = kill(vent_3,SIGINT);
		int res_v4 = kill(vent_4,SIGINT);
		int res_v5 = kill(vent_5,SIGINT);
		cout << "Padre: Resultados de las señales = " << res_v1 << "," << res_v2 << "," << res_v3 << "," << res_v4 << "," << res_v5 << endl;

		wpid = waitpid(vent_1, &status,0);
		cout << "Padre: Ventanilla 1 finalizó con estado= "<< status << endl;
		wpid = waitpid(vent_2, &status,0);
		cout << "Padre: Ventanilla 2 finalizó con estado= "<< status << endl;
		wpid = waitpid(vent_3, &status,0);
		cout << "Padre: Ventanilla 3 finalizó con estado= "<< status << endl;
		wpid = waitpid(vent_4, &status,0);
		cout << "Padre: Ventanilla 4 finalizó con estado= "<< status << endl;
		wpid = waitpid(vent_5, &status,0);
		cout << "Padre: Ventanilla 5 finalizó con estado= "<< status << endl;

		cout << "Padre: FIN"<< endl;
		exit ( 0 );
	}
}

