CC = cc

SRC = src/picuni.c src/application.c src/files.c src/imagelist.c src/stringutils.c src/hashprocess.c

NAME = picunic
PREFIX = /usr/local

CFLAGS ::= -O2 -s -Wall -Wextra -Wpedantic  -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE
LDFLAGS ::=
INCLUDE ::= -I./include

all: clean gm

options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "INCLUDE  = ${INCLUDE}"
	@echo "CC       = ${CC}"

stb:	CFLAGS += --std=c99
stb:	SRC += src/image_stb.c
stb:	SUFFIX = -stb
stb:	options compile

gm:	CFLAGS += --std=c89
gm:	SRC += src/image_gm.c
gm:	INCLUDE +=  $(shell GraphicsMagick-config --cppflags)
gm:	LDFLAGS += -lGraphicsMagick
gm:	SUFFIX = -gm
gm:	options compile

compile:
	${CC} ${SRC} ${INCLUDE} ${CFLAGS} ${LDFLAGS} -o ${NAME}${SUFFIX}

clean:
	rm -f ${NAME}

install:
	cp -i ${NAME} ${PREFIX}/bin

uninstall:
	rm -i ${PREFIX}/bin/${NAME}

.PHONY: all install uninstall clean
