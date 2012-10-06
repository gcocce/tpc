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

int manejarAuto(){
	int tiempo_estacionado;
	tiempo_estacionado = rand() % 24 + 1;

	cout << "Auto: id= " << getpid() << " tiempo estacionado: " << tiempo_estacionado << endl;

	sleep(tiempo_estacionado);

	return 0;
}

