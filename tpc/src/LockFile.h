#ifndef LOCKFILE_H_
#define LOCKFILE_H_

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

class LockFile {

private:

	struct flock fl;
	int fd;
	char nombre [ 255 ];

public:

	LockFile ( char* nombre );
	virtual ~LockFile();

	int tomarLock ( int pos);
	int liberarLock ( int pos);
	int escribir (int pos, char value );
	int leer(int pos, char *value);
};

#endif /* LOCKFILE_H_ */
