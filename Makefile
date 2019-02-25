CC = cc

SRC = src/picuni.c src/application.c src/files.c src/image.c src/imagelist.c src/stringutils.c

NAME = picunic
PREFIX = /usr/local

CFLAGS = -O2 -g -Wall -Wextra -Wpedantic --std=c89 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE
LDFLAGS = -lGraphicsMagick
INCLUDE := -I./include $(shell GraphicsMagick-config --cppflags)

all: clean options ${NAME}

options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "INCLUDE  = ${INCLUDE}"
	@echo "CC       = ${CC}"

${NAME}:
	${CC} ${SRC} ${INCLUDE} ${CFLAGS} ${LDFLAGS} -o ${NAME}

config:
	cp -n config.def.h config.h

clean:
	rm -f ${NAME}

install:
	cp -i ${NAME} ${PREFIX}/bin

uninstall:
	rm -i ${PREFIX}/bin/${NAME}

.PHONY: all install uninstall clean
