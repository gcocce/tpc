/*
 * estacionamiento.h
 *
 *  Created on: 10/10/2012
 *      Author: gk
 */

#ifndef ESTACIONAMIENTO_H_
#define ESTACIONAMIENTO_H_


class Estacionamiento {

private:
	int lugares;

public:
	Estacionamiento(int cantidad);
	virtual ~Estacionamiento();

	int getLugares();
	void setLugares(int cantidad);
};



#endif /* ESTACIONAMIENTO_H_ */
