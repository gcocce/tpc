#ifndef BUFFERSINCRONIZADO_H_
#define BUFFERSINCRONIZADO_H_

#include "Semaforo.h"
#include "MemoriaCompartida.h"

template <class T> class BufferSincronizado {

private:
	Semaforo semRead;
	MemoriaCompartida<T> mem;

public:
	BufferSincronizado (char* path, int code);
	~BufferSincronizado();

	int crear(int valorRead);
	int abrir();
	int waitRead();
	int signalRead();
	void escribir ( T dato );
	T leer ();
	void eliminar();
	void cerrar();
};

template <class T> BufferSincronizado<T> :: BufferSincronizado (char* path, int code) : semRead(path, code){
	mem.crear(path,(char)code);
}

template <class T> BufferSincronizado<T> :: ~BufferSincronizado (){
	//mem.liberar();
}

template <class T> int BufferSincronizado<T> :: crear(int valorRead) {
	int result=this->semRead.crear(valorRead);
	return result;
}

template <class T> int BufferSincronizado<T> :: abrir() {
	int result=this->semRead.abrir();
	return result;
}

template <class T> int BufferSincronizado<T> :: waitRead() {
	return this->semRead.wait();
}

template <class T> int BufferSincronizado<T> :: signalRead() {
	return this->semRead.signal();
}

template <class T> void BufferSincronizado<T> :: escribir ( T dato ){
	this->mem.escribir(dato);
}

template <class T> T BufferSincronizado<T> :: leer (){
	return this->mem.leer();
}

template <class T> void BufferSincronizado<T> :: cerrar (){
	//this->semRead.cerrar();
}

template <class T> void BufferSincronizado<T> :: eliminar (){
	this->semRead.eliminar();
	this->mem.liberar();
}


#endif /* BUFFERSINCRONIZADO_H_ */
