# Project: ChipmunkDB

CPP      = g++
OBJ      = Obj/main.o Obj/memory_dispatcher.o Obj/memory_bank.o Obj/logger.o Obj/helpers.o
LINKOBJ  = Obj/main.o Obj/memory_dispatcher.o Obj/memory_bank.o Obj/logger.o Obj/helpers.o
LIBS     = -L"/lib" -static "/lib/libboost_system.dll.a" "/lib/libboost_thread.dll.a"
INCS     = -I"/usr/include"
BIN      = ./Bin/ChipmunkDB
CXXFLAGS = $(CXXINCS) -lpthread
CFLAGS   = $(INCS)
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

Obj/main.o: Source/main.cpp
	$(CPP) -c Source/main.cpp -o Obj/main.o $(CXXFLAGS)

Obj/memory_dispatcher.o: Source/memory_dispatcher.cpp Headers/memory_dispatcher.h
	$(CPP) -c Source/memory_dispatcher.cpp -o Obj/memory_dispatcher.o

Obj/memory_bank.o: Source/memory_bank.cpp Headers/memory_bank.h
	$(CPP) -c Source/memory_bank.cpp -o Obj/memory_bank.o

Obj/logger.o: Source/logger.cpp Headers/logger.h
	$(CPP) -c Source/logger.cpp -o Obj/logger.o

Obj/helpers.o: Source/helpers.cpp Headers/helpers.h
	$(CPP) -c Source/helpers.cpp -o Obj/helpers.o