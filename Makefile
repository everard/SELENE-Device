ENGINE_ROOT = Source/Engine
ENGINE_SRC = $(wildcard $(ENGINE_ROOT)/*.cpp $(ENGINE_ROOT)/*/*.cpp $(ENGINE_ROOT)/*/*/*.cpp $(ENGINE_ROOT)/*/*/*/*.cpp $(ENGINE_ROOT)/*/*/*/*/*.cpp $(ENGINE_ROOT)/*/*/*/*/*/*.cpp)
ENGINE_OBJ = $(subst Obj!,Obj/,$(subst /,., $(patsubst %.cpp, Obj!%.o, $(ENGINE_SRC))))
ENGINE_LIBS = -lrt

EXPORTER_ROOT = Source/Exporter
EXPORTER_SRC = $(wildcard $(EXPORTER_ROOT)/*.cpp)
EXPORTER_LIBS = -lrt $(ENGINE_TARGET)

OPTIMIZE = -Os
CFLAGS   = -Wall $(OPTIMIZE) -std=c++0x -pthread -ggdb

CC   = g++

ENGINE_TARGET = Obj/Engine.lib
EXPORTER_TARGET = Obj/Exporter

exporter:
	$(CC) $(CFLAGS) $(EXPORTER_SRC) $(EXPORTER_LIBS) -o $(EXPORTER_TARGET)

engine: directory $(ENGINE_OBJ)
	ar rvs $(ENGINE_TARGET) $(ENGINE_OBJ)

directory:
	mkdir -p Obj

Obj/%.o:
	@echo "Building $@ ..."
	@$(CC) $(CFLAGS) $(subst !cpp,.cpp, $(subst .,/, $(patsubst Obj/%.o, %!cpp, $@))) $(ENGINE_LIBS) -c -o $@

clean:
	rm $(ENGINE_OBJ)
	rm $(ENGINE_TARGET)
	rm $(EXPORTER_TARGET)

all: engine exporter
