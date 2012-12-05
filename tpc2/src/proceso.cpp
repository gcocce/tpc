#include <iostream>
#include "proceso.h"
#include "SignalHandler.h"

using namespace std;

Proceso::Proceso(){
	this->pid=getpid();
	this->ppid=getppid();
	this->gpid=getpgrp();

	this->fin=0;
}

int Proceso::iniciar(){
	int res=0;
	this->regSignal(SIGINT);

	return res;
}

int Proceso::terminar(){
	SignalHandler::getInstance()->destruir();
	return 0;
}

sig_atomic_t Proceso::getFinalizar(){
	return this->fin;
}

void Proceso::setTerminar(sig_atomic_t value){
	this->fin = value;
}

EventHandler* Proceso::regSignal(int signal){
	return SignalHandler::getInstance()->registrarHandler( signal ,this );
}

Proceso::~Proceso(){
}
