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

VentanillaSalida :: VentanillaSalida(Logger* log, char *path, int est, char numeroVentanilla, ConcPipe* cp) : barrera(path, numeroVentanilla * 10 + 6), canalEntrada(path, numeroVentanilla * 10 + 7),canalEAdmin(path, 110 + numeroVentanilla * 2){
		this->estacionamiento=est;
		this->cpipe=cp;
		this->log= log;
		this->numeroVentanilla= numeroVentanilla;
		this->abierto=false;
		this->interumpido=false;
		std::ostringstream stringStream;
		stringStream << "Vent Sal num " << (int)numeroVentanilla << ": Se llamo al constructor.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

VentanillaSalida :: ~VentanillaSalida(){
	std::ostringstream stringStream;
	stringStream << "Vent Sal num " << (int)numeroVentanilla << ": Se llamo al destructor.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	this->log->~Logger();
}


void VentanillaSalida :: crear(){
	if (this->barrera.crear(0)!=SEM_OK){
		std::ostringstream stringStream;
		stringStream << "Vent Sal num " << (int)numeroVentanilla << ": Error al crear barrera.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit(1);
	}
	if (this->canalEntrada.crear(0,1)!=SEM_OK){
		this->barrera.eliminar();
		std::ostringstream stringStream;
		stringStream << "Vent Sal num " << (int)numeroVentanilla << ": Error al crear canalEntrada.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit(1);
	}

	std::ostringstream stringStream;
	stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": Ventanilla creada.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
}

void VentanillaSalida :: eliminar(){
		std::ostringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": Se llamo al metodo eliminar.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());

		this->barrera.eliminar();
		this->canalEntrada.eliminar();
		this->canalEAdmin.eliminar();
	}

void VentanillaSalida :: abrir(){
	if(this->barrera.abrir()!=SEM_OK){
		std::ostringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": Error al abrir barrera.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit(1);
	}
	if(this->canalEntrada.abrir()!=SEM_OK){
		//this->barrera.cerrar();

		std::ostringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": Error al abrir canalEntrada.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());

		exit(1);
	}

	if(this->canalEAdmin.abrir()!=SEM_OK){
		std::ostringstream stringStream;
		stringStream << "Vent SAl num " << (int)numeroVentanilla << ": Error al abrir canalEAdmin.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		exit(1);
	}

	this->cpipe->iniciar(ESCRITURA);

	std::ostringstream stringStream;
	stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": Ventanilla abierta.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());

}

void VentanillaSalida :: cerrar(){
		//this->barrera.cerrar();
		this->canalEntrada.cerrar();
		std::ostringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": Ventanilla cerrada.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

void VentanillaSalida :: iniciar(){
	SignalHandler::getInstance()->registrarHandler( SIGINT,this );
	this->abierto=true;
	this->barrera.signal();

	{
	std::ostringstream stringStream;
	stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": Ventanilla inicia Actividad iniciar().";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}

	//while(abierto == true || this->estacionamiento->getEspaciosOcupados() >0){
	while(abierto == true ){
		{
		std::stringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": canalEntrada.waitRead().";
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		this->canalEntrada.waitRead();

		bloquearSigint();

		{
		std::stringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": canalEntrada.leer().";
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		message msg= this->canalEntrada.leer();

		{
		std::stringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": recibe mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << (int)msg.time;
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		this->canalEntrada.signalWrite();

		{
		std::stringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": consultar AdminGral.";
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		//this->estacionamiento->freePlace(msg.place,msg.time);
		MsgF msgf;
		msgf.setConsulta(MsgF::liberarLugar);
		msgf.setEstacionamiento(this->estacionamiento);
		msgf.setVentanilla(this->numeroVentanilla);
		msgf.setLugar((int)msg.place);

		string str=msgf.toString();
		{
		std::stringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": consulta enviada:" << str;
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}
		this->cpipe->escribir((char*)str.c_str(),MsgF::DATASIZE);

		// Se espera la respuesta
		MsgFString mensajeE;
		this->canalEAdmin.waitRead();
		mensajeE=this->canalEAdmin.leer();
		this->canalEAdmin.signalWrite();
		{
		std::stringstream stringStream;
		stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": respueta recibida:" << mensajeE.dato;
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		MsgF msgr(string(mensajeE.dato));

		{
		std::stringstream stringStream;
		stringStream << "Vent Sal num " << (int)numeroVentanilla << ": resultado : " << msgr.getLugar();
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		desbloquearSigint();
		barrera.signal();
	}

	{
	std::stringstream stringStream;
	stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": se cierra la persiana.";
	string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	}

	this->eliminar();
}

void VentanillaSalida :: finalizar(){
	std::ostringstream stringStream;
	stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": metodo finalizar().";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());

	this->abierto=false;
	this->interumpido=true;
	//if(this->estacionamiento->getEspaciosOcupados()==0){
	this->eliminar();
	this->~VentanillaSalida();
	exit(0);
	//}else{
	//	this->abierto=false;
	//	this->canalEntrada.waitRead();
	//}
}

int VentanillaSalida ::  handleSignal ( int signum ) {
	std::ostringstream stringStream;
	stringStream << "Vent Sal num " << (int)this->numeroVentanilla << ": metodo handleSignal().";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());

	if( signum == SIGINT ){
		this->finalizar();
	}
	return 0;
}

