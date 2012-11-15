#ifndef VENTANILLAENTRADA_H_
#define VENTANILLAENTRADA_H_

#include "Semaforo.h"
#include "BufferSincronizado.h"
#include "Estacionamiento.h"
#include "SignalHandler.h"
#include "Message.h"
#include "logger.h"
#include "ConcPipe.h"

class VentanillaEntrada : public EventHandler {

private:
	bool abierta;
	int estacionamiento;
	char numeroVentanilla;
	Semaforo barrera;
	BufferSincronizado<message> canalEntrada;
	BufferSincronizado<message> canalSalida;

	BufferSincronizado<MsgFString> canalEAdmin;

	Logger* log;
	ConcPipe* cpipe;

public:
	VentanillaEntrada(Logger* log, char *path, int est, char numeroVentanilla, ConcPipe* cp);

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
