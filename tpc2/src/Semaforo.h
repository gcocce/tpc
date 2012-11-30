#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#define SEM_OK			0
#define	ERROR_FTOK		-1
#define ERROR_SEMGET	-2
#define	ERROR_SEMCTL	-3

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

class Semaforo {

private:
	int id;
	char path[30];
	int code;

public:
	Semaforo (char* path, int code);
	Semaforo(Semaforo &sem);
	virtual ~Semaforo();

	int crear(int valorInicial );
	int abrir();
	int wait(); // decrementa
	int signal(); // incrementa
	void eliminar();
};

#endif /* SEMAFORO_H_ */
