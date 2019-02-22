#ifndef IMAGE_H_
#define IMAGE_H_

/* Typedef */
typedef struct img
{
	char * path;
	unsigned long long hash;
} img;

void I_process ( char * path );

#endif
