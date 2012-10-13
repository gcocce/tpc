/*
 * logger.cpp
 *
 *  Created on: 13/10/2012
 *      Author: gk
 */

#include "logger.h"

Logger::Logger(bool debug){
	this->log=debug;

	this->fplog=NULL;

	if (this->log){
		char filename[20];
		sprintf(filename,"%u.log",getpid());
		this->fplog=fopen(filename,"at");

		if (this->fplog) {
			//fprintf(this->fplog,"%d, %s\n",time(NULL),"Inicia el proceso de loggeo...");
			timeval tp;
			gettimeofday(&tp,NULL);
			fprintf(this->fplog,"%d%06d %s\n", (long)tp.tv_sec, (long)tp.tv_usec, "Inicia el proceso de loggeo...");

		}
	}
}


Logger::~Logger(){
	if(this->fplog!=NULL){
		fclose(this->fplog);
		this->fplog=NULL;
	}
}

void Logger::debug(const char* msg){
	if (this->log){
		if(this->fplog==NULL){
			char filename[20];
			sprintf(filename,"%u.log",getpid());
			this->fplog=fopen(filename,"at");

		}

		if (this->fplog){
			//fprintf(this->fplog,"%d, %s\n",time(NULL),msg);
			timeval tp;
			gettimeofday(&tp,NULL);
			fprintf(this->fplog,"%d%06d %s\n", (long)tp.tv_sec, (long)tp.tv_usec, msg);
		}
	}
}

void Logger::flush(const char* msg){
	if (this->log){
		if(this->fplog==NULL){
			char filename[20];
			sprintf(filename,"%u.log",getpid());
			this->fplog=fopen(filename,"at");

		}

		if (this->fplog){
			//fprintf(this->fplog,"%d, %s\n",time(NULL),msg);
			timeval tp;
			gettimeofday(&tp,NULL);
			fprintf(this->fplog,"%d%06d %s\n", (long)tp.tv_sec, (long)tp.tv_usec, msg);
			fclose(this->fplog);
			this->fplog=NULL;
		}
	}
}




