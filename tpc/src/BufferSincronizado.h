#ifndef BUFFERSINCRONIZADO_H_
#define BUFFERSINCRONIZADO_H_

#include "Semaforo.h"
#include "MemoriaCompartida.h"

template <class T> class BufferSincronizado {

private:
	Semaforo semRead;
	Semaforo semWrite;
	MemoriaCompartida<T> mem;

public:
	BufferSincronizado (char* path, int code);
	~BufferSincronizado();

	int crear(int valorRead, int valorWrite);
	int abrir();
	int waitRead();
	int signalRead();
	int waitWrite();
	int signalWrite();
	void escribir ( T dato );
	T leer ();
	void eliminar();
	void cerrar();
};

template <class T> BufferSincronizado<T> :: BufferSincronizado (char* path, int code) : semRead(path, code), semWrite(path, code+1) {
	mem.crear(path,(char)code);
}

template <class T> BufferSincronizado<T> :: ~BufferSincronizado (){
	mem.liberar();
}

template <class T> int BufferSincronizado<T> :: crear(int valorRead, int valorWrite ) {
	int result=this->semRead.crear(valorRead);
	if (result == SEM_OK){
		result=this->semWrite.crear(valorWrite);
		if (result==SEM_OK){
			return SEM_OK;
		}else{
			this->semRead.eliminar();
			return result;
		}
	}else{
		return result;
	}
}

template <class T> int BufferSincronizado<T> :: abrir() {
	int result=this->semRead.abrir();
	if (result == SEM_OK){
		result=this->semWrite.abrir();
		if (result==SEM_OK){
			return SEM_OK;
		}else{
			this->semRead.eliminar();
			return result;
		}
	}else{
		return result;
	}
}

template <class T> int BufferSincronizado<T> :: waitRead() {
	return this->semRead.wait();
}

template <class T> int BufferSincronizado<T> :: waitWrite() {
	return this->semWrite.wait();
}

template <class T> int BufferSincronizado<T> :: signalRead() {
	return this->semRead.signal();
}

template <class T> int BufferSincronizado<T> :: signalWrite() {
	return this->semWrite.signal();
}

template <class T> void BufferSincronizado<T> :: escribir ( T dato ){
	this->mem.escribir(dato);
}

template <class T> T BufferSincronizado<T> :: leer (){
	return this->mem.leer();
}

template <class T> void BufferSincronizado<T> :: cerrar (){
	this->semRead.cerrar();
	this->semWrite.cerrar();
}

template <class T> void BufferSincronizado<T> :: eliminar (){
	this->semRead.eliminar();
	this->semWrite.eliminar();
}


#endif /* BUFFERSINCRONIZADO_H_ */
