################OPTION###################
CCOMPILE = arm-hisiv600-linux-gnueabi-gcc
CPPCOMPILE = arm-hisiv600-linux-gnueabi-g++
COMPILEOPTION = -c
INCLUDEDIR = -I -I. -Iinclude -I../../include/
LINK = arm-hisiv600-linux-gnueabi-gcc
LINKOPTION = -o bvcsptest
LIBDIRS = -L../lib/

ifeq ($(OS), PUTEST)
OBJS = ./AVDialog.o ./Command.o ./FileTransfer.o ./GPSDialog.o  ./loginout.o ./main.o
endif 
OUTPUT = bvcsptest
SHAREDLIB = -lm -lpthread -lBVCSP -lstdc++  -ldl
APPENDLIB = 
PROC_OPTION = DEFINE=_PROC_ MODE=ORACLE LINES=true CODE=CPP
ESQL_OPTION = -g
################OPTION END################
ESQL = esql
PROC = proc
$(OUTPUT):$(OBJS) $(APPENDLIB)
	$(LINK) $(LINKOPTION) $(LIBDIRS)   $(OBJS) $(SHAREDLIB) $(APPENDLIB) 

clean: 
	rm -f *.o
	rm -f $(OBJS)
	rm -f $(OUTPUT)
all: clean $(OUTPUT)
.PRECIOUS:%.cpp %.c %.C
.SUFFIXES:
.SUFFIXES:  .c .o .cpp .ecpp .pc .ec .C .cc .cxx

.cpp.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp
	
.cc.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp

.cxx.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp

.c.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.c

.C.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.C	

.ecpp.C:
	$(ESQL) -e $(ESQL_OPTION) $(INCLUDEDIR) $*.ecpp 
	
.ec.c:
	$(ESQL) -e $(ESQL_OPTION) $(INCLUDEDIR) $*.ec
	
.pc.cpp:
	$(PROC)  CPP_SUFFIX=cpp $(PROC_OPTION)  $*.pc
