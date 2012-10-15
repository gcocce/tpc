/*
 * cargenerator.cpp
 *
 *  Created on: 04/10/2012
 *      Author: gk
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include "genautos.h"
#include "auto.h"
#include "SIGINT_Handler.h"
#include "SignalHandler.h"

/* El generador de auto genera un auto cada t tiempo
 * donde t tiene una distribucion uniforme de probabilidad
 * entre 0 y RANGO_TIEMPO
*/
#define 	RANGO_TIEMPO 	10

using namespace std;

extern bool debug;

int generarAutos(pid_t vent[6]){

	Logger log(debug);

	log.flush("Inicia el generador de autos.");

	// event handler para la senial SIGINT (2)
	SIGINT_Handler sigint_handler;

	// se registra el event handler declarado antes
	SignalHandler::getInstance()->registrarHandler ( SIGINT,&sigint_handler );

	int dormir=0;

	// Variable para contar la cantidad de autos creados
	int autos=0;
	int autos_liberados=0;
	int finalizados_bien=0;
	int finalizados_mal=0;

	pid_t auto_pid;
	int status=0;

	// Mientras no se reciba la senial SIGINT, el proceso realiza su trabajo
	while (sigint_handler.getGracefulQuit()==0){
		// Se crea un auto
		pid_t auto_id = fork ();
		if (auto_id == 0 ) {
			// Hace falta eliminar el handler heredado en el stack
			SignalHandler::destruir ();
			int res = manejarAuto(vent);
			exit (res);
		} else {
			if (debug){
				char buffer [100];
				sprintf (buffer, "Se crea un auto con proces id %d", auto_id);
				log.flush(buffer);
			}

			autos++;

			srand(time(NULL) + autos);
			dormir = rand() % RANGO_TIEMPO; // devuelve un valor entre 0 y RANGO_TIEMPO
			if (debug){
				char buffer [100];
				sprintf (buffer, "El proceso duerme por %d segundos.", dormir);
				log.flush(buffer);
			}
			sleep(dormir);

			// Pruebo limpiar la estructura de un auto cuyo proceso haya terminado
			// Si no hay registro en la tabla de proceos se continua la ejecución
			auto_pid= waitpid((pid_t)-1,&status,WNOHANG);
			if (auto_pid>0){
				if (debug){
					char buffer [100];
					sprintf (buffer, "Se borra el registro del auto con pid %d con estado %d.", auto_pid,status);
					log.flush(buffer);
				}

				autos_liberados++;
				if (status==0){
					finalizados_bien++;
				}else{
					finalizados_mal++;
				}
			}
		}
	}

	// Se recibio la senial SIGINT, el proceso termina
	SignalHandler::getInstance()->removerHandler ( SIGINT);
	SignalHandler::destruir ();

	log.debug("Inicia la finalización del proceso");

	if (debug){
		char buffer [100];
		sprintf (buffer, "Autos creados %d, autos liberados %d.", autos, autos_liberados);
		log.debug(buffer);
	}

	if(autos_liberados < autos){
		log.debug("Se espera que finalicen los procesos de los autos que faltan.");
	}

	// Se espera a que finalice el resto de los hijos
	while (autos_liberados < autos){
		auto_pid= waitpid((pid_t)-1,&status,0);
		if (auto_pid>0){
			autos_liberados++;
			if (status==0){
				finalizados_bien++;
			}else{
				finalizados_mal++;
			}

			if (debug){
				char buffer [100];
				sprintf (buffer, "Se borra el registro del auto con pid %d con estado %d.", auto_pid,status);
				log.debug(buffer);
			}
		}
	}

	if (debug){
		char buffer [100];
		sprintf (buffer, "Finalizaron correctamente %d, incorrectamente %d", finalizados_bien, finalizados_mal);
		log.debug(buffer);
	}

	log.debug("Finaliza el generador de autos.");

	return 0;
}
