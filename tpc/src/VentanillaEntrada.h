/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */

#ifndef VENTANILLAENTRADA_H_
#define VENTANILLAENTRADA_H_

#include "Semaforo.h"
#include "BufferSincronizado.h"
#include "Estacionamiento.h"
#include "SignalHandler.h"
#include "Message.h"
#include "logger.h"

//class Estacionamiento;

class VentanillaEntrada : public EventHandler {

private:
	bool abierta;
	char numeroVentanilla;
	Semaforo barrera;
	BufferSincronizado<message> canalEntrada;
	BufferSincronizado<message> canalSalida;
	Estacionamiento *estacionamiento;
	Logger log;

public:
	VentanillaEntrada(Estacionamiento *estacionamiento, char *path, char numeroVentanilla);

	~VentanillaEntrada();

	void crear();
	void eliminar();
	void abrir();
	void cerrar();
	void iniciar();
	void finalizar();
	virtual int handleSignal ( int signum );
};

#endif /* VENTANILLAENTRADA_H_ */
