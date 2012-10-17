//============================================================================
// Name        : tpc.cpp
// Version     :
// Description : Proceso principal
//============================================================================
#ifdef pp
#include <iostream>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include "logger.h"
#include "LUResource.h"

#include "arraymemcomp.h"
#include "genautos.h"
#include "ventanilla_entrada.h"
#include "ventanilla_salida.h"
#include "definiciones.h"

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
bool debug=false;

int main(int argc, char* argv[]){
	int tiempo=0;
	int cantidad=0;
	float costo=0;

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
			cout << "Error: El primer argumento debe ser numerico." << endl;
			param_error=true;
		}
		if(isdigit(*argv[2])){
			cantidad=atoi(argv[2]);
		}else{
			cout << "Error: El segundo argumento debe ser numerico." << endl;
			param_error=true;
		}
		if(isdigit(*argv[3])){
			costo=atof(argv[3]);
		}else{
			cout << "Error: El tercer argumento debe ser numerico." << endl;
			param_error=true;
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
	cout << "Se inicia la simulacion practico de concurrentes..." << endl;

	Logger log(debug);
	if (debug){
		char buffer [100];
		sprintf (buffer, "Se inicia la simulación, duracion: %d, cocheras: %d, costo: %f", tiempo,cantidad,costo);
		log.flush(buffer);
	}

	LUResource lockAutos("autos.lok");

	if (lockAutos.crearRecurso()<=0){
		log.flush("Main: se produce un error al crear el recurso auto.lok");
		return -1;
	}

	// Inicializamos el valor
	lockAutos.tomarLockEscritura();
	lockAutos.escribirEntero(0);
	lockAutos.liberarLock();

	LUResource lockMonto("monto.lok");

	if (lockMonto.crearRecurso()<=0){
		log.flush("Main: se produce un error al crear el recurso monto.lok");
		return -1;
	}

	// Inicializamos el valor
	lockMonto.tomarLockEscritura();
	lockMonto.escribirDouble(0);
	lockMonto.liberarLock();

	log.flush ("Se crearon los objeto LockFile y se inicializaron a cero.");

	// Se crea el objeto estacionamiento que permite gestionar los lugares
	log.flush("Se crea el objeto de memoria compartida que modela el estacionamiento.");
	ArrayMemComp<int>	estacionamiento(cantidad);

	if (!estacionamiento.getEstado()){
		log.flush("Error: El objeto de memoria comp conc no tiene un estado valido!");
		cout << "Error: El objeto de memoria comp conc no tiene un estado valido!"<< endl;
		return -1;
	}

	int estadoMemoria = estacionamiento.crear();
	if ( estadoMemoria != SHM_OK ) {
		if (debug){
			char buffer [100];
			sprintf (buffer, "Error al crear el espacio de memoria compartida: %d!", estadoMemoria);
			log.flush(buffer);
		}
		cout << "Error al crear el espacio de memoria compartida: " << estadoMemoria << endl;
		return -1;
	}else{
		//Se inicializa el estado de las cocheras
		log.flush("Se inicializa el estado de las cocheras.");
		for (int i=0;i<estacionamiento.getSize();i++){
			estacionamiento.escribir (0 ,i);
		}
	}

	pid_t vent[6]; // Almacena el id de proceso de las ventanillas
	// Se inician los procesos de las ventanillas
	int status=0;
	pid_t wpid;

	log.flush("Se crean las ventanillas.");
	vent[1] = fork();
	if ( vent[1] == 0 ) {
		// Proceso ventanilla 1
		int res=m_ventanilla_entrada(1,estacionamiento);
		exit(res);
	}else{
		vent[2]=fork();
		if(vent[2]==0){
			// Proceso ventanilla 2
			int res=m_ventanilla_entrada(2,estacionamiento);
			exit(res);
		}else{
			vent[3]=fork();
			if (vent[3]==0){
				// Proceso ventanilla 3
				int res=m_ventanilla_entrada(3,estacionamiento);
				exit(res);
			}else{
				vent[4]=fork();
				if (vent[4]==0){
					// Proceso ventanilla 4
					int res=m_ventanilla_salida(4,estacionamiento);
					exit (res);
				}else{
					vent[5]=fork();
					if(vent[5]==0){
						// Proceso ventanilla 5
						int res=m_ventanilla_salida(5,estacionamiento);
						exit(res);
					}else{
						// Proceso padre continua más abajo


					}
				}
			}
		}
	}

	// El proceso principal continua por aquí.
	log.flush("Ventanillas creadas.");

	// Lanzar el generador de autos
	log.flush("Se forkea el proceso para crear el generador de autos.");
	pid_t genid = fork ();
	if ( genid == 0 ) {
		// Liberamos los recursos del estacionamiento para que no sean hereados
		estacionamiento.deleteSemaforos();
		int res = generarAutos(vent);
		exit ( res );
	} else {
		log.debug("Simulación en marcha...");
		sleep(tiempo);

		log.debug("Se envia la señal de finalización SIGINT");

		kill(genid,SIGINT);

		// Esperamos que finalice el generador de autos
		wpid = waitpid(genid, &status,0);
		log.debug("Finalizó el generador de autos.");

		if (debug){
			char buffer [100];
			sprintf (buffer, "El generador de autos finalizó con estado %d", status);
			log.debug(buffer);
		}

		// Cerramos las ventanillas
		log.debug("Padre: Envio SIGINT a las ventanillas.");
		for (int i=1;i<6;i++){
			int res = kill(vent[i],SIGINT);

			if (debug){
				char buffer [100];
				sprintf (buffer, "Resultados de la señal a ventanilla %d :%d", i, res);
				log.debug(buffer);
			}
		}

		for (int i=1;i<6;i++){
			wpid = waitpid(vent[i], &status,0);
			if (debug){
				char buffer [100];
				sprintf (buffer, "Ventanilla %d finalizó con estado %d", i, status);
				log.debug(buffer);
			}
		}

		// Aseguramos que no queden recursos tomados
		estacionamiento.liberarMemoria();
		estacionamiento.eliminarSemaforos();

		cout << "Finaliza la simulacion"<< endl;
		log.debug("Finaliza la simulación");
		return 0;
	}
}

#endif
