#ifndef GESTORCONSULTA_H_
#define GESTORCONSULTA_H_

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#include "BufferSincronizado.h"
#include "ConcPipe.h"
#include "MsgF.h"
#include "logger.h"
#include "proceso.h"

class GestorConsulta: public Proceso{
public:
	GestorConsulta(Logger* log, char *path, ConcPipe* cp);
	~GestorConsulta();
	void run();
	void crear();
	void abrir();
	void eliminar();

private:
	Logger* log;

	ConcPipe* cpipe;
	BufferSincronizado<MsgFST> canalEAdmin;

	int estado;
};


#endif /* GESTORCONSULTA_H_ */
