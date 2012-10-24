#include "LockFile.h"
#include <iostream>
#include <errno.h>
#include <string.h>
using namespace std;

LockFile :: LockFile ( char* nombre ) {

	strcpy ( this->nombre,nombre );

	this->fl.l_type = F_WRLCK;
	this->fl.l_whence = SEEK_SET;
	this->fl.l_start = 0;
	this->fl.l_len = sizeof(char);
	this->fl.l_pid = getpid ();
	this->fd = open ( this->nombre,O_CREAT|O_RDWR,0777 );
	//cout << getpid() << " LockFile " << this->nombre << " created" << endl;
}

int LockFile :: tomarLock (int pos) {
	this->fl.l_type = F_WRLCK;
	this->fl.l_start = pos * sizeof(char);
	//cout << getpid() << " LockFile " << this->nombre << " tomando lock pos " << pos << endl;
	int resultado = fcntl ( this->fd,F_SETLKW,&(this->fl) );
	return resultado;
}

int LockFile :: liberarLock (int pos) {
	this->fl.l_type = F_UNLCK;
	this->fl.l_start = pos * sizeof(char);
	//cout << getpid() << " LockFile " << this->nombre << " liberandoo lock pos " << pos << endl;
	int resultado = fcntl ( this->fd,F_SETLK,&(this->fl) );
	return resultado;
}

int LockFile :: escribir (int pos, char value ) {
	lseek ( this->fd,pos * sizeof(char),SEEK_SET );
<<<<<<< HEAD
	//cout << getpid() << " LockFile " << this->nombre << " escribiendo pos " << pos  << " el valor " << (int)value << endl;
=======
	cout << getpid() << " LockFile " << this->nombre << " escribiendo pos " << pos  << " el valor " << (int)value << endl;
>>>>>>> 34180b7b62f62971c1aabbf9666c69541d41d15c
	int resultado = write ( this->fd,&value,1);
	return resultado;
}

int LockFile :: leer (int pos, char *value ) {
	lseek ( this->fd,pos * sizeof(char),SEEK_SET );
	int resultado = read( this->fd,value,sizeof(char));
	if(resultado==-1){
		//cout <<  strerror(errno)  << endl;
	}
	//cout << getpid() << " LockFile " << this->nombre << " leyendo pos " << pos << " valor " << value << endl;
	return resultado;
}

LockFile :: ~LockFile () {
	close ( this->fd );
	if (unlink(this->nombre) == -1) {
		cout << getpid() << " LockFile " << this->nombre << " fallo eliminado" << endl;
	}else{
		cout << getpid() << " LockFile " << this->nombre << " eliminado" << endl;
	}
}
