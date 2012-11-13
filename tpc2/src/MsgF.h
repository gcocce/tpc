#ifndef MSGF_H_
#define MSGF_H_

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

class MsgF{
public:
	MsgF();
	MsgF(string std);
	~MsgF();

	enum FUNCION{
		obtenerLugar=1,
		liberarLugar,
		lugaresOcupados,
		montoRecaudado,
	};

	int getEstado();
	void setConsulta(FUNCION);
	void setEstacionamiento(int est);
	void setVentanilla(int vent);
	void setLugar(int lugar);
	void setMonto(float monto);

	int getConsulta();
	int getLugar();
	float getMonto();
	int getEstacionamiento();
	int getVentanilla();

	string toString();

	static const int DATASIZE=32;
private:
	int estado;

	int consulta;
	int ventanilla;
	int estacionamiento;
	int lugar;

	//int espacios;
	float monto;

	string completarString(string str);

};


#endif /* MSGF_H_ */
