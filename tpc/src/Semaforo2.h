#ifndef Semaforo2_H_
#define Semaforo2_H_

#define SEM_OK			0
#define	ERROR_FTOK		-1
#define ERROR_SEMGET	-2
#define	ERROR_SEMCTL	-3

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

class Semaforo2 {

private:
	int id;
	char* path;
	int code;

public:
	Semaforo2 (char* path, int code);
	virtual ~Semaforo2();

	int crear(int valorInicial );
	int abrir();
	int wait(); // decrementa
	int signal(); // incrementa
	void eliminar();
	void cerrar();
};

#endif /* Semaforo2_H_ */
