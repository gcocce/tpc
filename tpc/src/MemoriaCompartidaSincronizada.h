#ifndef MEMORIACOMPARTIDASINCRONIZADA_H_
#define MEMORIACOMPARTIDASINCRONIZADA_H_

#include	"MemoriaCompartida.h"
#include	"Semaforo.h"

template <class T> class MemoriaCompartidaSincronizada {

private:
	Semaforo sem;
	MemoriaCompartida<T> mem;

public:
	MemoriaCompartidaSincronizada ( char* path,char code );
	virtual ~MemoriaCompartidaSincronizada();

	void inicializar ( T dato){
		this->mem.escribir(dato);
	};

	void escribir ( T dato ){
		this->sem.p();
		this->mem.escribir(dato);
		this->sem.q();
	};

	T leer (){
		this->sem.p();
		T dato= this->mem.leer();
		this->sem.q();
		return dato;
	};

	void eliminar (){
		this->sem.eliminar();
	};
};

#endif /* MEMORIACOMPARTIDASINCRONIZADA_H_ */
