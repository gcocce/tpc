#ifndef ESTACIONAMIENTO_H_
#define ESTACIONAMIENTO_H_

#include <iostream>
#include <sstream>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <sys/wait.h>

#include "logger.h"

using namespace std;

class Estacionamiento {
private:
	pid_t ventanillasEntrada[3];
	pid_t ventanillasSalida[2];
	int espacios;
	int espaciosOcupados;
	float costo;
	float recaudacion;

	vector<int> vCocheras;

	int id;
	int fd;
	char path[32];

	Logger* log;

public:
	Estacionamiento(int id, int espacios, float costo, Logger* log);
	virtual ~Estacionamiento();

	int iniciar();
	void finalizar();

	int getEspaciosOcupados();
	int obtenerEspacio();
	int liberarEspacio(int espacio);

};

#endif /* ESTACIONAMIENTO_H_ */
