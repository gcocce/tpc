/*
 * AdminGral.cpp
 *
 *  Created on: 10/11/2012
 *      Author: gk
 */

#include "AdminGral.h"

AdminGral::AdminGral(Logger* log, int estacionamientos){
	this->log=log;
	this->estado=0;
}

AdminGral::~AdminGral(){
	this->log->~Logger();
	exit(this->estado);
}

void AdminGral::run(){

	log->debug("Admin: se inicia el metodo run.");

	while(!this->getFinalizar()){




	}

	log->debug("Admin: finaliza el metodo run.");
}
