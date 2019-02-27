#ifndef CONFIG_H_GUARD_
#define CONFIG_H_GUARD_

/* Typedef */
struct config
{
	char * cmdline;
	char * regexp;
	float precision;
	unsigned avghash_side;
	unsigned square;
	unsigned recursive : 1;
	unsigned customcmd : 1;
} config;

#endif
