/*
 * logger.cpp
 *
 *  Created on: 13/10/2012
 *      Author: gk
 */

#include "logger.h"
#include <string.h>

using namespace std;

Logger::Logger(){
	this->log=false;
	this->estado=false;
	this->ppid=0;
	this->fd=-1;
	this->sem=NULL;
}

Logger::Logger(bool debug){
	this->log=debug;
	this->estado=true;
	this->ppid=(int)getpid();
	this->fd=-1;
	this->sem=NULL;

	if (this->log){
		char filename[20];
		sprintf(filename,"%u.log",(int)getpid());
		this->fd=open(filename, O_CREAT | O_RDWR, 0700);

		this->sem=new Semaforo(filename, 'a');
		if (this->sem->crear(1)!=0){
			this->estado=false;
		}
	}
}


Logger::~Logger(){
	if(this->fd!=-1){
		close(this->fd);
		this->fd=-1;
	}
	if(this->sem!=NULL){
		delete (this->sem);
		this->sem=NULL;
	}
}

bool Logger::getEstado(){
	return this->estado;
}

void Logger::eliminarSemaforo(){
	if(this->sem!=NULL){
		this->sem->eliminar();
	}
}

void Logger::debug(const char* msg){
	if (this->log){
		this->sem->wait();
		if (this->fd!=-1){
			char buffer[128];
			timeval tp;
			gettimeofday(&tp,NULL);

			struct tm * timeinfo;
			timeinfo = localtime ( &tp.tv_sec );

			sprintf(buffer,"%02dhs %02dm %02ds %06dus pid:%d %s\n\0", (int)timeinfo->tm_hour,(int)timeinfo->tm_min,(int)timeinfo->tm_sec, (long)tp.tv_usec, (int)getpid(), msg);
			write(this->fd,buffer,strlen(buffer));
		}
		this->sem->signal();
	}
}

