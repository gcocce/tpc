#ifndef MENSAJES_H_
#define MENSAJES_H_

#define	PETICION	1
#define	RESPUESTA	2
#define TEXTO_SIZE	255


typedef struct mensaje {
	long mtype;
	int id;
	int type;
	float value;
} mensaje;

#endif /* MENSAJES_H_ */
