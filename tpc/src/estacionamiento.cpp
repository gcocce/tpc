/*
 * estacionamiento.cpp
 *
 *  Created on: 10/10/2012
 *      Author: gk
 */

#include "estacionamiento.h"

Estacionamiento::Estacionamiento(int cantidad){
	this->lugares = cantidad;
}

Estacionamiento::~Estacionamiento() {
}

int Estacionamiento :: getLugares() {

	return this->lugares;
}

void Estacionamiento :: setLugares(int cantidad) {

	this->lugares=cantidad;
}
