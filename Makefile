SRCDIR=src
INCLUDEDIR=include
PREFIX=/usr
LIB=neural++
CC=g++
CFLAGS=-Wall -pedantic -pedantic-errors

all:
	${CC} -I${INCLUDEDIR} ${CFLAGS} -fPIC -g -c ${SRCDIR}/neuralnet.cpp
	${CC} -I${INCLUDEDIR} ${CFLAGS} -fPIC -g -c ${SRCDIR}/layer.cpp
	${CC} -I${INCLUDEDIR} ${CFLAGS} -fPIC -g -c ${SRCDIR}/neuron.cpp
	${CC} -I${INCLUDEDIR} ${CFLAGS} -fPIC -g -c ${SRCDIR}/synapsis.cpp
	${CC} -I${INCLUDEDIR} ${CFLAGS} -fPIC -g -c ${SRCDIR}/Markup.cpp
	${CC} -shared -Wl,-soname,lib$(LIB).so.0 -o lib${LIB}.so.0.0.0 neuralnet.o layer.o neuron.o synapsis.o Markup.o
	ar rcs lib${LIB}.a neuralnet.o layer.o neuron.o synapsis.o Markup.o

install:
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/${INCLUDEDIR}
	install -m 0755 lib${LIB}.so.0.0.0 ${PREFIX}/lib/lib${LIB}.so.0.0.0
	install -m 0644 lib${LIB}.a ${PREFIX}/lib/lib${LIB}.a
	install -m 0644 ${INCLUDEDIR}/${LIB}.hpp ${PREFIX}/${INCLUDEDIR}
	install -m 0644 ${INCLUDEDIR}/${LIB}_exception.hpp ${PREFIX}/${INCLUDEDIR}
	ln -sf ${PREFIX}/lib/lib${LIB}.so.0.0.0 ${PREFIX}/lib/lib${LIB}.so.0

uninstall:
	rm ${PREFIX}/lib/lib${LIB}.a
	rm ${PREFIX}/${INCLUDEDIR}/${LIB}.hpp
	rm ${PREFIX}/${INCLUDEDIR}/${LIB}_exception.hpp
	rm ${PREFIX}/lib/lib${LIB}.so.0.0.0
	rm ${PREFIX}/lib/lib${LIB}.so.0

clean:
	rm *.o
	rm lib${LIB}.so.0.0.0
	rm lib${LIB}.a
