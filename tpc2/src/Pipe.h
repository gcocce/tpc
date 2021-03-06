#ifndef PIPE_H_
#define PIPE_H_

#include <unistd.h>
#include <fcntl.h>

#define	LECTURA		0
#define	ESCRITURA	1

class Pipe {

private:

	int descriptores[2];
	bool lectura;
	bool escritura;

public:
	Pipe();
	virtual ~Pipe();

	void setearModo ( int modo );

	int escribir ( void* dato,int datoSize );
	int leer ( void* buffer,int buffSize );

	int getFdLectura ();
	int getFdEscritura ();

	void cerrar ();
};

#endif /* PIPE_H_ */
