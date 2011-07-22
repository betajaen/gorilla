# Simple GNU make makefile
#
# Run from examples directory.

OGRE_LIBDIR=/usr/local/OGRE
OGRE_INCDIR=/usr/local/include

LIBS=-lOgreMain -lOIS -L${OGRE_LIBDIR}
CXXFLAGS=-g -Wall -I.. -I../extensions -I${OGRE_INCDIR}

vpath %.cpp ..
vpath %.cpp ../extensions

.PHONY: all
all:	../console ../playpen ../gorilla3d ../tests

#%.o: %.cpp
#	${CXX} ${CXXFLAGS} -c $< -o $@

../console:	gorilla_ogreconsole.o OgreConsoleForGorilla.o Gorilla.o
	${CXX} ${CXXFLAGS} ${LIBS} $^ -o $@

../playpen:	gorilla_playpen.o Gorilla.o
	${CXX} ${CXXFLAGS} ${LIBS} $^ -o $@

../gorilla3d:	gorilla_3d.o Gorilla.o
	${CXX} ${CXXFLAGS} ${LIBS} $^ -o $@

../tests:	gorilla_tests.o Gorilla.o
	${CXX} ${CXXFLAGS} ${LIBS} $^ -o $@

.PHONY: clean
clean:
	-rm *.o
