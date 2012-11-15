#ifndef ADMINGRAL_H_
#define ADMINGRAL_H_

#include <list>
#include <vector>
#include "Estacionamiento.h"
#include "proceso.h"
#include "SignalHandler.h"
#include "logger.h"

using namespace std;

class AdminGral:public Proceso {

public:
	AdminGral(Logger* log, int estacionamientos, Semaforo* semCrear, int espacios, float costo);
	~AdminGral();
	void run();

private:
/*
	int obtenerLugar(int estacionamiento);
	int liberarLugar(int estacionamiento, int pos);
	int lugaresOcupados(int estacionamiento);
	float montoEstacionamiento(int estacionamiento);
	float montoTotal();
*/

	int estado;
	int estacionamientos;
	int espacios;
	float costo;

	Logger* log;
	Semaforo* semInicio;

	vector<Estacionamiento*> vEstacionamientos;
};


#endif /* ADMINGRAL_H_ */
