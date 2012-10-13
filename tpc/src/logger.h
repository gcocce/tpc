#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>


class Logger {
public:
	Logger(bool debug);

    void debug(const char* msg);
    void flush(const char* msg);

    //void count_time(string);
    //void write_time();

    ~Logger();

private:
    FILE* fplog;
    bool log;

    //time_t rawtime;
    //struct tm * timeinfo;
};

#endif
