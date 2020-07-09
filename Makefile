LIBS=-lGLESv2 -lm -lglfw -lrt -lm -ldl -lX11 -lpthread -lxcb -lXau -lXdmcp
CC := g++
LN := g++
CFLAGS += -g -std=c++17 -Wall -Os $(LIBS)

# "compress" GLSL code and embed it in a C++ header to be #include'd like this:
# GLchar vShaderStr[] =
#     #include "basic.vert.h"
#     ;
# GLSL2H_DBG doesn't compress the source so line numbers are retained for debugging
GLSL2H := sed -e '/^\s*\/\//d' -e 's/^\s*//' -e '/^\s*$$/d' -e 's/^/\"/' -e 's/$$/\\n\"/'
GLSL2H_DBG := sed -e 's/^/\"/' -e 's/$$/\\n\"/'

default: all

all: test

clean:
	rm -f test test.o *.vert.h *.frag.h

test: test.o 
	$(LN) $(CFLAGS) -o test test.o

test.o: background.vert.h background.frag.h simple.vert.h simple.frag.h

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

%.vert.h: %.vert
	$(GLSL2H) $^ > $@

%.frag.h: %.frag
	$(GLSL2H) $^ > $@
