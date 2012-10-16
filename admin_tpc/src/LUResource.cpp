#include "LUResource.h"
#include <stdio.h>

LUResource :: LUResource ( char* nombre ) {
	strcpy ( this->nombre,nombre );
	this->fd=0;
}

int LUResource :: crearRecurso() {
	this->fl.l_type = F_WRLCK;
	this->fl.l_whence = SEEK_SET;
	this->fl.l_start = 0;
	this->fl.l_len = 0;
	this->fl.l_pid = getpid ();
	this->fd = open ( this->nombre,O_CREAT|O_RDWR,0777 );
	return this->fd;
}

int LUResource :: conectarRecurso() {
	this->fl.l_type = F_WRLCK;
	this->fl.l_whence = SEEK_SET;
	this->fl.l_start = 0;
	this->fl.l_len = 0;
	this->fl.l_pid = getpid ();
	this->fd = open ( this->nombre,O_RDWR,0777 );
	return this->fd;
}


int LUResource :: eliminarRecurso() {
	if (unlink(this->nombre) == -1) {
		return errno;
	}
	return 0;
}


void LUResource :: setPID() {
	this->fl.l_pid= getpid();
}

void LUResource :: setNombre(char* nombre) {
	strcpy ( this->nombre,nombre );
}

int LUResource :: tomarLockLectura () {
	this->fl.l_type = F_RDLCK;
	int resultado = fcntl ( this->fd,F_SETLKW,&(this->fl) );
	return resultado;
}

int LUResource :: tomarLockEscritura () {
	this->fl.l_type = F_WRLCK;
	int resultado = fcntl ( this->fd,F_SETLKW,&(this->fl) );
	return resultado;
}

int LUResource :: liberarLock () {
	this->fl.l_type = F_UNLCK;
	int resultado = fcntl ( this->fd,F_SETLK,&(this->fl) );
	return resultado;
}

int LUResource :: leer ( char* buffer, int buffsize){
	lseek ( this->fd,0,SEEK_SET );
	int resultado = read (this->fd, buffer, buffsize);
	if (resultado == -1) {
			int myerr = errno;
			printf("ERROR: read errno(%d): %s\n", errno,strerror(myerr));
	}
	return resultado;
}

int LUResource :: escribir ( char* buffer,int buffsize ) {
	lseek ( this->fd,0,SEEK_SET);
	int resultado = write ( this->fd, buffer,buffsize );
	if (resultado == -1) {
			int myerr = errno;
			printf("ERROR: read errno(%d): %s\n", errno,strerror(myerr));
	}
	return resultado;
}

int LUResource :: leerEntero (){
	int resultado=0;
	char buffer[50];
	memset(buffer, '\0', 50);
	int n=this->leer(buffer,50);
	if (n<0){
		printf("ERROR: leerEntero (%d)\n", n);
		return n;
	}else if (n==0){
		printf("ERROR: leerEntero, leyo cero caracteres.\n");
		return 0;
	}else{
		resultado=atoi(buffer);
		return resultado;
	}
}

int LUResource :: escribirEntero ( int valor) {
	char buffer[50];
	memset(buffer, '\0', 50);
	int n=sprintf (buffer, "%d                        ", valor);
	if (n<=0){
		printf("ERROR: escribirEntero (%d)\n", n);
		return n;
	}else{
		this->escribir(buffer,n);
		return 0;
	}
}

double LUResource :: leerDouble(){
	double resultado=0;
	char buffer[50];
	memset(buffer, '\0', 50);
	int n=this->leer(buffer,50);
	if (n<0){
		printf("ERROR: leerDouble(%d)\n", n);
		return n;
	}else if (n==0){
		printf("ERROR: leerEntero, leyo cero caracteres.\n");
		return 0;
	}else{
		resultado=atof(buffer);
		return resultado;
	}
}

int LUResource :: escribirDouble ( double valor) {
	char buffer[50];
	memset(buffer, '\0', 50);
	int n=sprintf (buffer, "%f                        ", valor);
	if (n<=0){
		printf("ERROR: escribirDouble (%d)\n", n);
		return n;
	}else{
		this->escribir(buffer,n);
		return 0;
	}
}

LUResource :: ~LUResource () {
	if (this->fd>0){
		close ( this->fd );
	}
}
