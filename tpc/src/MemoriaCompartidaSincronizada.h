#ifndef MEMORIACOMPARTIDASINCRONIZADA_H_
#define MEMORIACOMPARTIDASINCRONIZADA_H_

#include	"MemoriaCompartida.h"
#include	"Semaforo.h"

template <class T> class MemoriaCompartidaSincronizada {

private:
	Semaforo sem;
	MemoriaCompartida<T> mem;

	int inicializar ();

public:
	MemoriaCompartidaSincronizada ( char* path,char code );
	virtual ~MemoriaCompartidaSincronizada();

	int esperarLectura();
	int listoParaLectura();
	void escribir ( T dato );
	T leer ();
	void eliminar ();
};

#endif /* MEMORIACOMPARTIDASINCRONIZADA_H_ */
