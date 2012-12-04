/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */
#include "VentanillaSalida.h"
#include <iostream>
#include <sstream>
#include <string>
#include "MsgF.h"

using namespace std;

VentanillaSalida :: VentanillaSalida(Logger* log, char *path, int est, char numeroVentanilla, ConcPipe* cp) : barrera(path, numeroVentanilla * 10 + 4), canalEntrada(path, numeroVentanilla * 10 + 5),canalEAdmin(path, 7 + numeroVentanilla * 10){
		this->estacionamiento=est;
		this->cpipe=cp;
		this->log= log;
		this->numeroVentanilla= numeroVentanilla;
		this->abierto=false;
		std::ostringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Se llamo al constructor.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

VentanillaSalida :: ~VentanillaSalida(){
	{
	std::ostringstream stringStream;
	stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Se llamo al destructor.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}
	this->log->~Logger();
	this->cpipe->~ConcPipe();
	SignalHandler::getInstance()->destruir();
}


void VentanillaSalida :: crear(){
	if (this->barrera.crear(0)!=SEM_OK){
		std::ostringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Error al crear barrera.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit(1);
	}
	if (this->canalEntrada.crear(0)!=SEM_OK){
		this->barrera.eliminar();
		std::ostringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Error al crear canalEntrada.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit(1);
	}
	if (this->canalEAdmin.crear(0)!=SEM_OK){
		this->barrera.eliminar();
		this->canalEntrada.eliminar();
		this->canalEAdmin.eliminar();
		std::ostringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Error al crear canalEAdmin.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit (1);
	}

	std::ostringstream stringStream;
	stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Ventanilla creada.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
}

void VentanillaSalida :: eliminar(){
		std::ostringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Se llamo al metodo eliminar.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());

		this->barrera.eliminar();
		this->canalEntrada.eliminar();
		this->canalEAdmin.eliminar();
	}

void VentanillaSalida :: abrir(){
	if(this->barrera.abrir()!=SEM_OK){
		std::ostringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Error al abrir barrera.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit(1);
	}
	if(this->canalEntrada.abrir()!=SEM_OK){
		//this->barrera.cerrar();

		std::ostringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Error al abrir canalEntrada.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());

		exit(1);
	}

	if(this->canalEAdmin.abrir()!=SEM_OK){
		std::ostringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Error al abrir canalEAdmin.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit(1);
	}

	this->cpipe->iniciar(ESCRITURA);

	std::ostringstream stringStream;
	stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Ventanilla abierta.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());

}

void VentanillaSalida :: cerrar(){
		//this->barrera.cerrar();
		this->canalEntrada.cerrar();
		std::ostringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Ventanilla cerrada.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

void VentanillaSalida :: iniciar(){
	SignalHandler::getInstance()->registrarHandler( SIGINT,this );
	this->abierto=true;
	this->barrera.signal();

	{
	std::ostringstream stringStream;
	stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": Ventanilla inicia Actividad iniciar().";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}

	while(abierto == true ){
		{
		//std::stringstream stringStream;
		//stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": canalEntrada.waitRead().";
		//string copyOfStr = stringStream.str();
		//this->log->debug(copyOfStr.c_str());
		}

		this->canalEntrada.waitRead();

		bloquearSigint();

		{
		//std::stringstream stringStream;
		//stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": canalEntrada.leer().";
		//string copyOfStr = stringStream.str();
		//this->log->debug(copyOfStr.c_str());
		}

		message msg= this->canalEntrada.leer();

		{
		std::stringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": recibe mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << (int)msg.time;
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		{
		std::stringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": consultar AdminGral.";
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		//this->estacionamiento->freePlace(msg.place,msg.time);
		MsgF msgf;
		msgf.setConsulta(MsgF::liberarLugar);
		msgf.setEstacionamiento(this->estacionamiento);
		msgf.setVentanilla(this->numeroVentanilla);
		msgf.setTiempo((int)msg.time);
		msgf.setLugar((int)msg.place);

		string str=msgf.toString();
		{
		std::stringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": consulta enviada:" << str;
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		MsgFST st=msgf.toStruct();
		//this->cpipe->escribir((char*)str.c_str(),MsgF::DATASIZE);
		this->cpipe->escribir((void*)&st,sizeof(st));

		// Se espera la respuesta
		/*
		MsgFString mensajeE;
		for(int i=0; i<MsgF::DATASIZE;i++){
			mensajeE.dato[i]='0';
		}*/

		MsgFST st2;

		this->canalEAdmin.waitRead();
		st2=this->canalEAdmin.leer();

		MsgF msgr(st2);
		{
		std::stringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": respueta recibida:" << msgr.toString();
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		{
		std::stringstream stringStream;
		stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": resultado : " << msgr.getLugar();
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		desbloquearSigint();
		barrera.signal();
	}

	{
	std::stringstream stringStream;
	stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": se cierra la persiana.";
	string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}

	this->eliminar();
}

void VentanillaSalida :: finalizar(){
	{
	std::ostringstream stringStream;
	stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": metodo finalizar().";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}

	this->abierto=false;
	this->eliminar();
	this->~VentanillaSalida();
	exit(0);
}

int VentanillaSalida ::  handleSignal ( int signum ) {
	std::ostringstream stringStream;
	stringStream << "Vent Sal " << (int)numeroVentanilla << " Est " << this->estacionamiento << ": metodo handleSignal().";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());

	if( signum == SIGINT ){
		this->finalizar();
	}
	return 0;
}

