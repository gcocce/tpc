#ifndef CONCPIPE_H_
#define CONCPIPE_H_

#include "Pipe.h"
#include "Semaforo.h"

#define	LECTURA		0
#define	ESCRITURA	1

#define LETRA_A 'A'
#define LETRA_B 'B'

class ConcPipe {

public:
	ConcPipe(char * nombre);
	~ConcPipe();

	void iniciar(int Modo);
	void eliminar();

	int leer(void* buffer,int datoSize);
	int escribir(void* dato,int datoSize);
	int getEstado();


private:
	int modo;
	int estado;
	Pipe tuberia;
	Semaforo* semAcceso;
	Semaforo* semItemListos;

};


#endif /* CONCPIPE_H_ */
