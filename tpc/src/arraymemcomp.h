/*
 * arraymemcomp.h
 *
 *  Created on: 11/10/2012
 *      Author: gk
 */

#ifndef ARRAYMEMCOMP_H_
#define ARRAYMEMCOMP_H_

#define SHM_OK			0
#define	ERROR_FTOK		-1
#define ERROR_SHMGET	-2
#define	ERROR_SHMAT		-3

#define NOMBRE_ARCHIVO "arraymemcompf.dat"
#define LETRA_MEM_T	't'	// Mem compartida para datos tipo T
#define LETRA_MEM_S	's' // Mem compartida para variables lectores
#define LETRA_SEM_L	'l' // Semaforos de lectura
#define LETRA_SEM_E	'e' // Semaforos de escrituas

#define MODO_LECTURA 1
#define MODO_ESCRITURA 2

#include 	"SemaforoSB.h"
#include 	<stdio.h>
#include 	<unistd.h>
#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/shm.h>

using namespace std;

template <class T> class ArrayMemComp{
private:
	bool	valido;
	bool	inicializado;

	int 	size;

	int 	shmId;
	T*		ptrDatos;

	int		shmIdL;
	int*	ptrLectores;

	SemaforoSB*	mutexLectura;
	SemaforoSB*	mutexEscritura;

	int cantidadProcesosAdosados ();

	// Pre y post protocolos para acceder a los recursos de la posicion pos
	void open(int pos, int modo);
	void close(int pos, int modo);

	// crear memoria compartida para variables lectores
	int crear_mem_lectores();
	void liberar_mem_lectores();

	// crear memoria compartida para datos tipo T
	int crear_mem_t();

	void	inicializar();
public:

	ArrayMemComp (int posiciones);
	~ArrayMemComp ();

	// Para consultar el estado
	bool	getEstado();
	int		getSize();

	// crear() se tiene que ejecutar inicialmente en el proceso padre antes de hacer fork()
	int 	crear ();
	void	liberar ();
	int		escribir ( T dato, int pos );
	T 		leer (int pos);
};

template <class T> ArrayMemComp<T>::ArrayMemComp (int posiciones) {
	this->valido=true;
	this->inicializado=false;
	this->size=posiciones;
	this->shmId=0;
	this->shmIdL=0;
	this->ptrLectores=NULL;
	this->ptrDatos=NULL;
	this->mutexLectura=NULL;
	this->mutexEscritura=NULL;

	// Se crea el archivo usado para generar la memoria compartida y los semáforos.
	FILE *pFile;
	pFile = fopen(NOMBRE_ARCHIVO,"w");
	if (pFile!=NULL)
	{
	    fclose (pFile);
	}else{
	    printf( "ArrayMemCom: No se pudo crear el archivo!!!.\n");
		this->valido=false;
	}

	if (this->valido){
		// Creamos lista de semáforos de lectura
		this->mutexLectura=new SemaforoSB(NOMBRE_ARCHIVO , LETRA_SEM_L , this->size, 1);

		if (this->mutexLectura->getId()<=0){
			printf("ArrayMemComp: Falla al crear conjunto de semaforos de Lectura.\n");
			this->valido=false;
		}
	}
	if(this->valido){
		// Creamos lista de semáforos de escritura
		this->mutexEscritura=new SemaforoSB(NOMBRE_ARCHIVO, LETRA_SEM_E, this->size, 1);

		if (this->mutexEscritura->getId()<=0){
			printf( "ArrayMemComp: Falla al crear conjunto de semaforos de Escritura.\n");
			this->valido=false;
		}
	}
}

template <class T> ArrayMemComp<T> :: ~ArrayMemComp () {
	// Eliminamos lista de semáforos
	if (this->mutexLectura!=NULL){
		this->mutexLectura->eliminar();
		delete(this->mutexLectura);
	}
	if (this->mutexEscritura!=NULL){
		this->mutexEscritura->eliminar();
		delete(this->mutexEscritura);
	}

	//TODO Hace falta poner un lock para borrar el archivo
	FILE *fp = fopen(NOMBRE_ARCHIVO,"r");
	if( fp ) {
		fclose(fp);
		// Eliminamos el archivo
		 if( remove( NOMBRE_ARCHIVO ) == -1 ){
		    //printf("ArrayMemComp: Error al intentar eliminar el archivo.\n");
		 }
	}

}


template <class T> int ArrayMemComp<T> :: crear () {
	int res=this->crear_mem_lectores();

	if(res==SHM_OK){
		res=this->crear_mem_t();
		if(res!=SHM_OK){
			this->liberar_mem_lectores();
			return res;
		}else{
			// Se inicializa una sola vez
			if (!this->inicializado){
				this->inicializar();
				this->inicializado=true;
			}
			return res;
		}
	}else{
		return res;
	}

}


template <class T> int ArrayMemComp<T>::crear_mem_lectores () {
	// Creación de la memoria compartida para variables lectores
	key_t clave_l = ftok (NOMBRE_ARCHIVO, LETRA_MEM_S);
	if ( clave_l == -1 )
		return ERROR_FTOK;
	else {
		// Creacion de la memoria compartida
		this->shmIdL = shmget ( clave_l, sizeof(int) * size, 0644|IPC_CREAT );

		if ( this->shmIdL == -1 )
			return ERROR_SHMGET;
		else {
			// Attach del bloque de memoria al espacio de direcciones del proceso
			void* ptrTemporal = shmat ( this->shmIdL,NULL,0 );

			if ( ptrTemporal == (void *) -1 ) {
				return ERROR_SHMAT;
			} else {
				this->ptrLectores = (int*) ptrTemporal;

				return SHM_OK;
			}
		}
	}
}


template <class T> int ArrayMemComp<T>::crear_mem_t () {
	// Generacion de la clave
	key_t clave = ftok (NOMBRE_ARCHIVO, LETRA_MEM_T);
	if ( clave == -1 )
		return ERROR_FTOK;
	else {
		// Creacion de la memoria compartida
		this->shmId = shmget ( clave,sizeof(T) * size, 0644|IPC_CREAT );

		if ( this->shmId == -1 )
			return ERROR_SHMGET;
		else {
			// Attach del bloque de memoria al espacio de direcciones del proceso
			void* ptrTemporal = shmat ( this->shmId,NULL,0 );

			if ( ptrTemporal == (void *) -1 ) {
				return ERROR_SHMAT;
			} else {
				this->ptrDatos = (T *) ptrTemporal;
				return SHM_OK;
			}
		}
	}
}

// Se inicializa el valor de las variables lectores
template <class T> void ArrayMemComp<T>::inicializar () {
	for(int i=0; i<this->size; i++){
		*(this->ptrLectores + i) = 0;
	}
}

template <class T> void ArrayMemComp<T> :: liberar () {
	// detach del bloque de memoria de datos tipo T
	shmdt ((void*)this->ptrDatos );
	// detach del bloque de memoria de variables lectores
	shmdt ((void*)this->ptrLectores );

	int procAdosados = this->cantidadProcesosAdosados();

	if ( procAdosados == 0 ) {
		shmctl ( this->shmId,IPC_RMID,NULL );
		shmctl ( this->shmIdL, IPC_RMID, NULL);
	}

	if (this->mutexLectura!=NULL){
		this->mutexLectura->eliminar();
		delete(this->mutexLectura);
		this->mutexLectura=NULL;
	}
	if (this->mutexEscritura!=NULL){
		this->mutexEscritura->eliminar();
		delete(this->mutexEscritura);
		this->mutexEscritura=NULL;
	}
}


template <class T> void ArrayMemComp<T> :: liberar_mem_lectores () {
	// detach del bloque de memoria de variables lectores
	shmdt ((void*)this->ptrLectores );

	shmid_ds estado;
	shmctl (this->shmIdL,IPC_STAT,&estado );
	int procAdosados = estado.shm_nattch;

	if (procAdosados == 0) {
		shmctl ( this->shmIdL, IPC_RMID, NULL);
	}
}

/* Permite escribir un dato de tipo T en la posicion pos
 * Retorno:
 * 		0 OK
 * 		-1 pos está fuera del rango (Entre 0 y posiciones-1).
 * */
template <class T> int ArrayMemComp<T> :: escribir ( T dato , int pos) {
	if( pos>=0 && pos<this->size){
		this->open(pos,MODO_ESCRITURA);

		*(this->ptrDatos + pos)=dato;

		this->close(pos,MODO_ESCRITURA);
		return 0;
	}else{
		return -1;
	}
}

/* Permite leer un dato de tipo T en la posicion pos
 * Retorno:
 * 		Tipo de dato T
 * 		NULL si pos está fuera del rango (Entre 0 y posiciones-1).
 * */
template <class T> T ArrayMemComp<T> :: leer (int pos) {
	if( pos>=0 && pos<this->size){
		T* ptrT;
		this->open(pos,MODO_LECTURA);

		ptrT=(this->ptrDatos + pos);

		this->close(pos,MODO_LECTURA);
		return *ptrT;
	}else{
		return NULL;
	}
}

template <class T> void ArrayMemComp<T> :: open (int pos, int modo) {
	if (modo==MODO_LECTURA){
		// wait(mutex)
		this->mutexLectura->p(pos);
		//lectores=lectores+1
		*(this->ptrLectores + pos)=*(this->ptrLectores + pos)+1;
		// if (lectores==1)
		if (*(this->ptrLectores + pos)==1){
			// wait(escritura)
			this->mutexEscritura->p(pos);
		}
		// signal(mutex)
		this->mutexLectura->v(pos);
	}else{
		// wait(escritura)
		this->mutexEscritura->p(pos);
	}
}

template <class T> void ArrayMemComp<T> :: close(int pos, int modo) {
	if (modo==MODO_LECTURA){
		// wait(mutex)
		this->mutexLectura->p(pos);
		//lectores=lectores-1
		*(this->ptrLectores + pos)=*(this->ptrLectores + pos)-1;
		// if (lectores==0)
		if (*(this->ptrLectores + pos)==0){
			// signal(escritura)
			this->mutexEscritura->v(pos);
		}
		// signal(mutex)
		this->mutexLectura->v(pos);
	}else{
		// signal(escritura)
		this->mutexEscritura->v(pos);
	}

}

template <class T> int ArrayMemComp<T> :: cantidadProcesosAdosados () {
	shmid_ds estado;
	shmctl ( this->shmId,IPC_STAT,&estado );
	return estado.shm_nattch;
}

template <class T> bool ArrayMemComp<T> :: getEstado() {
	return this->valido;
}

template <class T> int ArrayMemComp<T> :: getSize() {
	return this->size;
}

#endif /* ARRAYMEMCOMP_H_ */
