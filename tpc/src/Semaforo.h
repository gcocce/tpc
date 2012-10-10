#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

class Semaforo {

private:
	int id;
	int valorInicial;

	int inicializar ();

public:
	Semaforo ( char* path, char code, int value );
	virtual ~Semaforo();

	int p (); // decrementa
	int v (); // incrementa
	void eliminar ();
};

#endif /* SEMAFORO_H_ */
