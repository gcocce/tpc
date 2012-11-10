#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "Semaforo.h"

class Logger {
public:
	Logger();
	Logger(bool debug);

    void debug(const char* msg);
    void eliminarSemaforo();
    bool getEstado();

    ~Logger();

private:
    int fd;
    bool log;
    bool estado;
    int	ppid;
    Semaforo* sem;

};

#endif
