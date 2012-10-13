/*
 * car.cpp
 *
 *  Created on: 04/10/2012
 *      Author: gk
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "auto.h"
#include "logger.h"

using namespace std;

extern bool debug;

int manejarAuto(pid_t vent[6]){
	int tiempo_estacionado=0;
	int ventanilla_entrada=0;

	Logger log(debug);


	tiempo_estacionado = rand() % 24 + 1;
	ventanilla_entrada = rand() % 3 + 1;

	if (debug){
		char buffer [100];
		sprintf (buffer, "El auto selecciona la ventanilla de entrada numero %d \n", ventanilla_entrada);
		log.debug(buffer);

		sprintf (buffer, "Tiempo estacionado %d \n", tiempo_estacionado);
		log.debug(buffer);
	}

	sleep(tiempo_estacionado);

	return 0;
}

