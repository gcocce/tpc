#ifndef LOCKFILE_H_
#define LOCKFILE_H_

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

class LockFile {
private:
	struct flock fl;
	int fd;
	char nombre [ 255 ];

	int escribir ( char* buffer,int buffsize );
	int leer ( char* buffer, int buffsize);

public:

	LockFile ( char* nombre );
	~LockFile();

	int crearRecurso();
	int conectarRecurso();
	int eliminarRecurso();

	void setPID();
	void setNombre(char* nombre);

	int tomarLockLectura ();
	int tomarLockEscritura ();
	int liberarLock ();

	int escribirEntero(int valor);
	int leerEntero();

	int escribirDouble(double valor);
	double leerDouble();
};

#endif /* LOCKFILE_H_ */
