/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */

#ifndef VENTANILLAENTRADA_H_
#define VENTANILLAENTRADA_H_

#include "Semaforo.h"

class VentanillaEntrada{

private:
	Semaforo barrera;
	BufferSincronizado<int> canalEntrada;
	BufferSincronizado<int> canalSalida;
public:
	VentanillaEntrada(){

	}

	~VentanillaEntrada(){

	}

	void crear(){

	}

	void eliminar(){

	}

	void abrir(){

	}

	void cerrar(){

	}
};


#endif /* VENTANILLAENTRADA_H_ */
