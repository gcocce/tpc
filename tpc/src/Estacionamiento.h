/*
 * Estacionamiento.h
 *
 *  Created on: Oct 15, 2012
 *      Author: plucadei
 */

#ifndef ESTACIONAMIENTO_H_
#define ESTACIONAMIENTO_H_

#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "LockFile.h"
#include "LUResource.h"
#include "MemoriaCompartida.h"
#include "EventHandler.h"
#include "logger.h"
#include <iostream>


class Estacionamiento  : public EventHandler {

private:
	pid_t ventanillasEntrada[3];
	pid_t ventanillasSalida[2];
	int espacios;
	float costo;
//	MemoriaCompartida<int> espaciosOcupados;
//	MemoriaCompartida<float> dineroCobrado;
	LUResource espaciosOcupados;//("../../pruebaLock/Debug/autos.lok");
	LUResource dineroCobrado;//("../../pruebaLock/Debug/monto.lok");
	LockFile lugares;
	char path[255];
	Logger log;

public:
	Estacionamiento(char* path, int espacios, float costo);

	virtual ~Estacionamiento();

	void iniciar();

	void finalizar();

	char findPlace();

	void freePlace(char ubicacion , char horas);

	int getEspaciosOcupados();

	void eliminarRecursos();

	virtual int handleSignal ( int signum ) {
		if( signum == SIGINT ){
			std::cout << "Estacionamiento manejando SIGINT" << std::endl;
			this->finalizar();
		}
		return 0;
	}

};

#endif /* ESTACIONAMIENTO_H_ */
