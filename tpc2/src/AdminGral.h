#ifndef ADMINGRAL_H_
#define ADMINGRAL_H_

#include <list>
#include <vector>
#include "Estacionamiento.h"
#include "proceso.h"
#include "SignalHandler.h"
#include "logger.h"
#include "GestorConsult.h"

using namespace std;

class AdminGral:public Proceso {

public:
	AdminGral(Logger* log, int estacionamientos, Semaforo* semCrear, int espacios, float costo);
	~AdminGral();
	void run();

private:

	int estado;
	int estacionamientos;
	int espacios;
	float costo;

	Logger* log;
	Semaforo* semInicio;

	vector<Estacionamiento*> vEstacionamientos;

	BufferSincronizado<MsgFST>* pvBufferGestor;
	pid_t pgestor;
};


#endif /* ADMINGRAL_H_ */
