#ifndef ESTACIONAMIENTO_H_
#define ESTACIONAMIENTO_H_

#include <iostream>
#include <sstream>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <sys/wait.h>

#include "logger.h"
#include "ConcPipe.h"
#include "BufferSincronizado.h"
#include "MsgF.h"

using namespace std;

class Estacionamiento {
private:
	pid_t ventanillasEntrada[3];
	pid_t ventanillasSalida[2];
	int espacios;
	int espaciosOcupados;
	int autosTotales;
	float costo;
	float recaudacion;

	vector<int> vCocheras;

	//vector<BufferSincronizado<MsgFString>*> vBuffersE;
	//vector<BufferSincronizado<MsgFString>*> vBuffersS;

	vector<BufferSincronizado<MsgFST>*> vBuffersE;
	vector<BufferSincronizado<MsgFST>*> vBuffersS;


	int id;
	int fd;
	char path[32];

	//Semaforo* semEstAbiertos;
	ConcPipe* cPipe;
	Logger* log;

public:
	Estacionamiento(int id, int espacios, float costo, Logger* log, ConcPipe* cp);
	virtual ~Estacionamiento();

	int iniciar();
	int abrirMemorias();

	void finalizar();

	int getEspaciosOcupados();

	//BufferSincronizado<MsgFString>* getBufferEntrada(int ventanilla);
	//BufferSincronizado<MsgFString>* getBufferSalida(int ventanilla);

	BufferSincronizado<MsgFST>* getBufferEntrada(int ventanilla);
	BufferSincronizado<MsgFST>* getBufferSalida(int ventanilla);

	int obtenerEspacio();
	void liberarEspacio(int espacio, int tiempo);

};

#endif /* ESTACIONAMIENTO_H_ */
