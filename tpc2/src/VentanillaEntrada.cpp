/*
 * VentanillaEntrada.h
 *
 *  Created on: Oct 16, 2012
 *      Author: plucadei
 */
#include "VentanillaEntrada.h"
#include <iostream>
#include <sstream>
#include <string>
#include "MsgF.h"

using namespace std;

VentanillaEntrada :: VentanillaEntrada(Logger* log, char *path, int est, char numeroVentanilla,ConcPipe* cp) : barrera(path, 10 * numeroVentanilla + 1), canalEntrada(path, numeroVentanilla * 10 + 2), canalSalida(path, numeroVentanilla * 10 + 4),canalEAdmin(path, 90 + numeroVentanilla * 2){
		this->estacionamiento=est;
		this->cpipe=cp;
		this->log= log;
		this->numeroVentanilla= numeroVentanilla;
		this->abierta=false;

		std::ostringstream stringStream;
		stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se llamo al constructor.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

VentanillaEntrada :: ~VentanillaEntrada(){
	std::ostringstream stringStream;
	stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se llamo al destructor.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());
	this->log->~Logger();
}


void VentanillaEntrada :: crear(){
		if (this->barrera.crear(0)!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al crear barrera.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if (this->canalEntrada.crear(0,1)!=SEM_OK){
			this->barrera.eliminar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al crear canalEntrada.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if (this->canalSalida.crear(0,1)!=SEM_OK){
			this->barrera.eliminar();
			this->canalEntrada.eliminar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al crear canalSalida.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}

		std::ostringstream stringStream;
		stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Ventanilla Creada.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

void VentanillaEntrada :: eliminar(){
		std::ostringstream stringStream;
		stringStream << "Vent Ent num " << (int)this->numeroVentanilla << ": Se llamo al metodo eliminar.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());

		this->barrera.eliminar();
		this->canalEntrada.eliminar();
		this->canalSalida.eliminar();
		this->canalEAdmin.eliminar();
	}

void VentanillaEntrada :: abrir(){
		if(this->barrera.abrir()!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al abrir barrera.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if(this->canalEntrada.abrir()!=SEM_OK){
			//this->barrera.cerrar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al abrir canalEntrada.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}
		if(this->canalSalida.abrir()!=SEM_OK){
			//this->barrera.cerrar();
			this->canalEntrada.cerrar();
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al abrir canalSalida.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}

		if(this->canalEAdmin.abrir()!=SEM_OK){
			std::ostringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Error al abrir canalEAdmin.";
			std::string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			exit(1);
		}

		this->cpipe->iniciar(ESCRITURA);

		std::ostringstream stringStream;
		stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se ha abierto la barrera.";
		std::string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
	}

void VentanillaEntrada :: cerrar(){
		//this->barrera.cerrar();
		this->canalEntrada.cerrar();
		this->canalSalida.cerrar();
	}

void VentanillaEntrada :: iniciar(){
		SignalHandler::getInstance()->registrarHandler( SIGINT,this );

		this->abierta=true;
		this->barrera.signal();

		{
		stringstream stringStream;
		stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Ventanilla abierta, esperando autos.";
		string copyOfStr = stringStream.str();
		this->log->debug(copyOfStr.c_str());
		}

		while(this->abierta==true){
			{
			stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": canalEntrada.waitRead().";
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalEntrada.waitRead();
			bloquearSigint();

			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": mensaje recibido, canalEntrada.leer().";
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			message msg= this->canalEntrada.leer();

			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << (int)msg.time;
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalEntrada.signalWrite();
			msg.pid = getpid();
			if(this->abierta==true){
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent num " << (int)numeroVentanilla << ": consultar AdminGral";
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}

				MsgF msgf;
				msgf.setConsulta(MsgF::obtenerLugar);
				msgf.setEstacionamiento(this->estacionamiento);
				msgf.setVentanilla(this->numeroVentanilla);
				msgf.setLugar(0);

				string str=msgf.toString();
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent num " << (int)numeroVentanilla << ": consulta enviada: " << str;
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}

				// Se envia la consulta al administrador general
				this->cpipe->escribir((char*)str.c_str(),MsgF::DATASIZE);

				// Se espera la respuesta
				cout << "Vent Ent num " << (int)numeroVentanilla << " se espera respuesta." << endl;
				MsgFString mensajeE;
				cout << "Vent Ent canalEAdmin.waitRead()" << endl;
				this->canalEAdmin.waitRead();
				mensajeE=this->canalEAdmin.leer();
				cout << "Vent Ent canalEAdmin.signalWrite()" << endl;
				this->canalEAdmin.signalWrite();
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent num " << (int)numeroVentanilla << ": respuesta recibida: " << mensajeE.dato;
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}
				MsgF msgr(string(mensajeE.dato));

				{
				std::stringstream stringStream;
				stringStream << "Vent Ent num " << (int)numeroVentanilla << ": resultado : " << msgr.getLugar();
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}
				msg.place=(char)msgr.getLugar();
			}else{
				{
				std::stringstream stringStream;
				stringStream << "Vent Ent num " << (int)numeroVentanilla << ": el estacionamiento está cerrado.";
				string copyOfStr = stringStream.str();
				this->log->debug(copyOfStr.c_str());
				}

				msg.place= -1;
			}


			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": canalSalida.waitWrite().";
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalSalida.waitWrite();

			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Escribe mensaje, pid: " << msg.pid << " lugar: " << (int)msg.place << " tiempo: " << (int)msg.time;
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalSalida.escribir(msg);

			{
			std::stringstream stringStream;
			stringStream << "Vent Ent num " << (int)numeroVentanilla << ": canalEntrada.signalRead().";
			string copyOfStr = stringStream.str();
			this->log->debug(copyOfStr.c_str());
			}

			this->canalSalida.signalRead();
			desbloquearSigint();
			this->barrera.signal();
		}
	this->eliminar();
}

void VentanillaEntrada :: finalizar(){
	std::ostringstream stringStream;
	stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se llamo al metodo finalizar.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());

	this->abierta=false;
	this->eliminar();
	this->~VentanillaEntrada();
	exit(0);
}

int VentanillaEntrada ::  handleSignal ( int signum ) {
	std::ostringstream stringStream;
	stringStream << "Vent Ent num " << (int)numeroVentanilla << ": Se llamo al metodo handSignal.";
	std::string copyOfStr = stringStream.str();
	this->log->debug(copyOfStr.c_str());

	if( signum == SIGINT ){
		this->finalizar();
	}
	return 0;
}

