#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

extern bool debug;
void DEBUG(char *Txt) {
		if(debug==true){
			char filename[20];
			sprintf(filename,"%u.log",getpid());
			FILE *fp=fopen(filename,"at");
			if (fp) {
				fprintf(fp,"%d, %s\n",time(NULL),Txt);
				fclose(fp);
			}
		}
	}
#endif
