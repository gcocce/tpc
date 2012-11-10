/*
 * car.h
 *
 *  Created on: 04/10/2012
 *      Author: gk
 */

#ifndef CAR_H_
#define CAR_H_

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#include "logger.h"
#include "proceso.h"

class Auto: public Proceso{
public:
	Auto(Logger* log, int estacionamientos);
	~Auto();
	void run();
private:
	Logger* log;
	int estado;
	int estacionamientos;
};


#endif /* CAR_H_ */
