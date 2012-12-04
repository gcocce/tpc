/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */

#ifndef VENTANILLASALIDA_H_
#define VENTANILLASALIDA_H_

#include "Semaforo.h"
#include "BufferSincronizado.h"
#include "Estacionamiento.h"
#include "SignalHandler.h"
#include "Message.h"
#include "logger.h"
#include "ConcPipe.h"
#include "MsgF.h"

//class Estacionamiento;

class VentanillaSalida : public EventHandler {

private:
	bool abierto;
	char numeroVentanilla;
	int estacionamiento;
	Semaforo barrera;
	BufferSincronizado<message> canalEntrada;

	//BufferSincronizado<MsgFString> canalEAdmin;
	BufferSincronizado<MsgFST> canalEAdmin;

	Logger* log;
	ConcPipe* cpipe;

public:
	VentanillaSalida(Logger* log, char *path, int est, char numeroVentanilla, ConcPipe* cp);
	~VentanillaSalida();


	void crear();
	void eliminar();
	void abrir();
	void cerrar();
	void iniciar();
	void finalizar();
	virtual int handleSignal ( int signum );

};

#endif /* VENTANILLASALIDA_H_ */
