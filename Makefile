CC = cc

SRC = src/picuni.c src/application.c

NAME = picunic
PREFIX = /usr/local

CFLAGS = -O2 -g -Wall -Wextra -Wpedantic --std=c99 -D_DEFAULT_SOURCE -I./include
LDFLAGS =

all: clean options ${NAME}

options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

${NAME}:
	${CC} ${SRC} ${CFLAGS} ${LDFLAGS} -o ${NAME}

config:
	cp -n config.def.h config.h

clean:
	rm -f ${NAME}

install:
	cp -i ${NAME} ${PREFIX}/bin

uninstall:
	rm -i ${PREFIX}/bin/${NAME}

.PHONY: all install uninstall clean
