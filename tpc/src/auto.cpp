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

using namespace std;

int manejarAuto(pid_t vent[6]){
	int tiempo_estacionado=0;
	int ventanilla_entrada=0;

	tiempo_estacionado = rand() % 24 + 1;
	ventanilla_entrada = rand() % 3 + 1;

	cout << "Auto: id= " << getpid() << " tiempo estacionado: " << tiempo_estacionado << endl;

	cout << "Auto: id= " << getpid() << " venanilla " << ventanilla_entrada << " pid= " << vent[ventanilla_entrada] << endl;

	sleep(tiempo_estacionado);

	return 0;
}

