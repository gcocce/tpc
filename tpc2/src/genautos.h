/*
 * cargenerator.h
 *
  */
#ifndef CARGENERATOR_H_
#define CARGENERATOR_H_

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include "logger.h"

#include "proceso.h"

class GenAutos: public Proceso{

public:
	GenAutos(Logger* log, int est);
	~GenAutos();
	void run();
private:
	Logger* log;
	int estacionamientos;
	int estado;
};



#endif /* CARGENERATOR_H_ */
