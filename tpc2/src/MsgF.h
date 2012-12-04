#ifndef MSGF_H_
#define MSGF_H_

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

struct MsgFST{
	int consulta;
	int ventanilla;
	int estacionamiento;
	int lugar;
	float monto;
	int tiempo;
};


class MsgF{
public:
	MsgF();
	MsgF(string std);
	MsgF(MsgFST st);
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
	void setTiempo(int tiempo);

	int getConsulta();
	int getLugar();
	float getMonto();
	int getTiempo();
	int getEstacionamiento();
	int getVentanilla();

	string toString();
	MsgFST toStruct();


	static const int DATASIZE=32;
private:
	int estado;

	int consulta;
	int ventanilla;
	int estacionamiento;
	int lugar;
	float monto;
	int tiempo;

	string completarString(string str);

};

struct MsgFString{
	char	dato[MsgF::DATASIZE];
};

#endif /* MSGF_H_ */
