OUTPUT_DIR = Release
SOURCE_DIR = ../Source

ENGINE_ROOT = $(SOURCE_DIR)/Engine
ENGINE_SRC  = $(wildcard $(ENGINE_ROOT)/*.cpp $(ENGINE_ROOT)/*/*.cpp $(ENGINE_ROOT)/*/*/*.cpp $(ENGINE_ROOT)/*/*/*/*.cpp $(ENGINE_ROOT)/*/*/*/*/*.cpp $(ENGINE_ROOT)/*/*/*/*/*/*.cpp)
ENGINE_OBJ  = $(subst $(OUTPUT_DIR)!,$(OUTPUT_DIR)/,$(subst /,., $(patsubst $(SOURCE_DIR)/%.cpp, $(OUTPUT_DIR)!%.o, $(ENGINE_SRC))))
ENGINE_LIBS = -lrt

EXPORTER_ROOT = $(SOURCE_DIR)/Exporter
EXPORTER_SRC  = $(wildcard $(EXPORTER_ROOT)/*.cpp)
EXPORTER_LIBS = -lrt $(ENGINE_TARGET)

OPTIMIZE = -O3
CFLAGS   = -Wall $(OPTIMIZE) -std=c++0x -pthread -ggdb

CXX = g++

ENGINE_TARGET = $(OUTPUT_DIR)/Engine.a
EXPORTER_TARGET = $(OUTPUT_DIR)/Exporter

$(EXPORTER_TARGET): $(EXPORTER_SRC) $(ENGINE_TARGET)
	$(CXX) $(CFLAGS) $(EXPORTER_SRC) $(EXPORTER_LIBS) -o $(EXPORTER_TARGET)

$(ENGINE_TARGET): $(ENGINE_OBJ)
	@ar rvs $(ENGINE_TARGET) $(ENGINE_OBJ)

exporter: $(EXPORTER_TARGET)

engine: $(ENGINE_TARGET)

clean:
	rm -f $(ENGINE_OBJ)
	rm -f $(ENGINE_TARGET)
	rm -f $(EXPORTER_TARGET)

all: engine exporter

.SECONDEXPANSION:
$(ENGINE_OBJ): $$(addprefix $(SOURCE_DIR)/,$$(subst !cpp,.cpp, $$(subst .,/, $$(patsubst $(OUTPUT_DIR)/%.o, %!cpp, $$@))))
	@mkdir -p $(OUTPUT_DIR)
	@echo "Compiling $(patsubst $(SOURCE_DIR)/%,%, $?)"
	@$(CXX) $(CFLAGS) $? $(ENGINE_LIBS) -c -o $@
