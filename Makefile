# Master Makefile for TSGL

# *****************************************************
# Variables to control Makefile operation

AR=ar
CC=g++
RM=rm -f
INSTALL=/usr/bin/install
PREFIX=/usr/local

SRC_PATH=src/TSGL/
TESTS_PATH=src/tests/
EXAMPLES_PATH=src/examples/
OBJ_PATH=build/
VPATH=SRC_PATH:TESTS_PATH:OBJ_PATH

HEADERS  := $(wildcard src/TSGL/*.h)
SOURCES  := $(wildcard src/TSGL/*.cpp)
OBJS     := $(patsubst src/TSGL/%.cpp,build/TSGL/%.o,${SOURCES})
NOWARN   := -Wno-unused-parameter -Wno-unused-function -Wno-narrowing
UNAME    := $(shell uname)

ifeq ($(UNAME), Linux)
	OS_LFLAGS := -lpthread
	OS_LDIRS := -L/opt/AMDAPP/lib/x86_64/
	OS_EXTRA_LIB := -L/usr/lib
	OS_GL := -lGL
	OS_OMP := -fopenmp -lomp
	OS_COMPILER := -std=c++0x
endif

ifeq ($(UNAME), CYGWIN_NT-10.0)
	OS_LFLAGS := -lpthread
	OS_LDIRS := -L/opt/AMDAPP/lib/x86_64/
	OS_EXTRA_LIB := -L/usr/lib
	OS_GL := -lGL
	OS_OMP := -lgomp -fopenmp
	OS_COMPILER := -std=gnu++11
endif

ifeq ($(UNAME), Darwin)
	OS_LFLAGS := -framework Cocoa -framework OpenGl -framework IOKit -framework Corevideo
	OS_LDIRS :=
	OS_EXTRA_LIB :=
	OS_GL :=
	OS_OMP := -fopenmp -lomp
	OS_COMPILER := -std=c++0x
endif

CXXFLAGS=-O3 -g3 -ggdb3 \
	-Wall -Wextra \
	-D__GXX_EXPERIMENTAL_CXX0X__ \
	-I/usr/local/include/Cellar/glfw3/3.3/include/ \
	-I${SRC_PATH}/ \
	-I/usr/include/ \
	-I/usr/local/include/ \
	-I/usr/local/include/freetype2 \
	-I/usr/local/include/freetype2/freetype \
	-I/opt/AMDAPP/include/ \
	-I/usr/include/c++/4.6/ \
	-I/usr/include/c++/4.6/x86_64-linux-gnu/ \
	-I/usr/lib/gcc/x86_64-linux-gnu/9/include/ \
	-I/usr/include/freetype2  \
	-I/usr/include/freetype2/freetype  \
	-I./ \
  $(OS_COMPILER) -fopenmp \
  ${NOWARN} -fpermissive
  # -pedantic-errors

LFLAGS=-Llib/ \
	-L/usr/local/lib \
	${OS_EXTRA_LIB} \
	-L/usr/X11/lib/ \
	${OS_LDIRS} \
	-ltsgl -lfreetype -lGLEW -lglfw \
	-lX11 ${OS_GL} -lXrandr -Xpreprocessor $(OS_OMP) -I"$(brew --prefix libomp)/include" \
	${OS_LFLAGS} 

DEPFLAGS=-MMD -MP

TESTFLAGS = -lsrc/TSGL/

# ****************************************************
# Targets needed to bring all files up to date

#Use make tsgl to make only the library files
all: dif tsgl docs tutorial

debug: dif tsgl tests

dif: build/build

#This may change (for the Mac installer)!
#tsgl
ifeq ($(UNAME), Linux)
tsgl: lib/libtsgl.a lib/libtsgl.so
endif
ifeq ($(UNAME), CYGWIN_NT-10.0)
tsgl: lib/libtsgl.a lib/libtsgl.dll
endif
ifeq ($(UNAME), Darwin)
tsgl: lib/libtsgl.a lib/libtsgl.so
endif

# must run 'make' before 'make tests' and 'make examples'
tests: $(TESTS_PATH) lib/libtsgl.a
	$(MAKE) -C $<
	@touch build/build

examples: $(EXAMPLES_PATH) lib/libtsgl.a
	$(MAKE) -C $<
	@touch build/build

docs: docs/html/index.html

tutorial: tutorial/docs/html/index.html

cleanall: clean cleantests cleanexamples cleandocs

clean:
	$(RM) -r bin/* build/* lib/* tutorial/docs/html/* *~ *# *.tmp
	$(MAKE) cleantests
	$(MAKE) cleanexamples

cleantests:
	(cd $(TESTS_PATH) && $(MAKE) clean)

cleanexamples:
	(cd $(EXAMPLES_PATH) && $(MAKE) clean)

cleandocs:
	$(RM) -r docs/html/*

# -include build/*.d

#install
ifeq ($(UNAME), Linux)
install:
	test -d $(PREFIX) || mkdir $(PREFIX)
	test -d $(PREFIX)/lib || mkdir $(PREFIX)
	test -d $(PREFIX)/include || mkdir $(PREFIX)
	install -m 0644 lib/libtsgl.a $(PREFIX)/lib
	install -m 0755 lib/libtsgl.so $(PREFIX)/lib
	cp -r src/TSGL $(PREFIX)/include
endif
ifeq ($(UNAME), CYGWIN_NT-10.0)
install:
	test -d $(PREFIX) || mkdir $(PREFIX)
	test -d $(PREFIX)/lib || mkdir $(PREFIX)
	test -d $(PREFIX)/include || mkdir $(PREFIX)
	install -m 0644 lib/libtsgl.a $(PREFIX)/lib
	install -m 0755 lib/libtsgl.dll $(PREFIX)/lib
	cp -r src/TSGL $(PREFIX)/include
endif
ifeq ($(UNAME), Darwin)
install:
	test -d $(PREFIX) || mkdir $(PREFIX)
	test -d $(PREFIX)/lib || mkdir $(PREFIX)
	test -d $(PREFIX)/include || mkdir $(PREFIX)
	install -m 0644 lib/libtsgl.a $(PREFIX)/lib
	install -m 0755 lib/libtsgl.so $(PREFIX)/lib
	cp -r src/TSGL $(PREFIX)/include
	cp -r stb $(PREFIX)/include
endif

build/build: ${HEADERS} ${SOURCES} ${TESTS}
	@echo 'Files that changed:'
	@echo $(patsubst src/%,%,$?)

#lib/libtsgl.*
ifeq ($(UNAME), Linux)
lib/libtsgl.so: ${OBJS}
	@echo 'Building $(patsubst lib/%,%,$@)'
	$(CC) -shared -o $@ $?
	@touch build/build
endif
ifeq ($(UNAME), CYGWIN_NT-10.0)
lib/libtsgl.dll: ${OBJS}
	@echo 'Building $(patsubst lib/%,%,$@)'
	$(CC) -shared -o $@ $? $(LFLAGS)
	@touch build/build
endif
ifeq ($(UNAME), Darwin)
lib/libtsgl.so: ${OBJS}
	@echo 'Building $(pathsubst lib/%,%,$@)'
	$(CC) -shared -undefined suppress -flat_namespace -o $@ $?
	@touch build/build
endif

lib/libtsgl.a: ${OBJS}
	@echo 'Building $(patsubst lib/%,%,$@)'
	mkdir -p lib
	$(AR) -r $@ $?
	@touch build/build

build/%.o: src/%.cpp
	@echo ""
	@tput setaf 3;
	@echo "+++++++++++++++++++ Building $@ +++++++++++++++++++"
	@tput sgr0;
	@echo ""
	mkdir -p ${@D}
	$(CC) -c -fpic $(CXXFLAGS) $(DEPFLAGS) -o "$@" "$<"


#Doxygen stuff
docs/html/index.html: ${HEADERS} doxyfile
	@echo ""
	@tput setaf 3;
	@echo "+++++++++++++++++++ Generating Doxygen +++++++++++++++++++"
	@tput sgr0;
	@echo ""
	mkdir -p docs
	@doxygen doxyfile

tutorial/docs/html/index.html: ${HEADERS} tutDoxyFile
	@echo ""
	@tput setaf 3;
	@echo "+++++++++++++++++++ Generating Doxygen +++++++++++++++++++"
	@tput sgr0;
	@echo ""
	mkdir -p tutorial/docs
	doxygen tutDoxyFile

.PHONY: all debug clean tsgl docs tutorial dif
.SECONDARY: ${OBJS} ${TESTOBJS} $(OBJS:%.o=%.d)
