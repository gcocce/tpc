#ifndef SEMAFOROSB_H_
#define SEMAFOROSB_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

class SemaforoSB {

private:
	int id;
	int size;
	int valorInicial;

	int inicializar ();

public:
	SemaforoSB ( char* path, char code, int size, int value );
	~SemaforoSB();

	int getId();

	int p (int sem); // decrementa
	int v (int sem); // incrementa
	void eliminar ();
};

#endif /* SEMAFOROSB_H_ */
