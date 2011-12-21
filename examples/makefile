# Simple GNU make makefile
#
# Run from examples directory.
#
# Comment out / in the settings applicable to your system.

# Ogre 3d installed from source
OGRE_INCDIR=-I/usr/local/include
OGRE_RENDERER=/usr/local/lib/OGRE/RenderSystem_GL.so

# Ogre 3d installed from distro package
#OGRE_RENDERER=/usr/lib/OGRE/RenderSystem_GL.so

# Ogre 3d installed from 64bit distro package
#OGRE_RENDERER=/usr/lib64/OGRE/RenderSystem_GL.so

LIBS=-lOgreMain -lOIS
CXXFLAGS= -g -I.. -I../extensions ${OGRE_INCDIR} -DOGRE_RENDERER="\"${OGRE_RENDERER}\""

vpath %.cpp ..
vpath %.cpp ../extensions

.PHONY: all
all:	../console ../playpen ../gorilla3d ../tests

../console:	gorilla_ogreconsole.o OgreConsoleForGorilla.o Gorilla.o
	${CXX} $^ ${LIBS} -o $@

../playpen:	gorilla_playpen.o Gorilla.o
	${CXX} $^ ${LIBS} -o $@

../gorilla3d:	gorilla_3d.o Gorilla.o
	${CXX} $^ ${LIBS} -o $@

../tests:	gorilla_tests.o Gorilla.o
	${CXX} $^ ${LIBS} -o $@

.PHONY: clean
clean:
	-rm *.o
