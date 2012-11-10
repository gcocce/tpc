/*
 * AdminGral.h
 *
 *  Created on: 10/11/2012
 *      Author: gk
 */

#ifndef ADMINGRAL_H_
#define ADMINGRAL_H_

#include "proceso.h"
#include "logger.h"

class AdminGral:public Proceso {

public:
	AdminGral(Logger* log, int estacionamientos);
	~AdminGral();
	void run();

private:
	int obtenerLugar(int estacionamiento);
	int liberarLugar(int estacionamiento, int pos);

	int lugaresOcupados(int estacionamiento);

	float montoEstacionamiento(int estacionamiento);
	float montoTotal();

	int estado;
	Logger* log;
};


#endif /* ADMINGRAL_H_ */
