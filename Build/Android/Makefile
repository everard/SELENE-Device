DEBUG_OUTPUT_DIR   = Debug/$(shell echo $(ANDROID_NDK_ABI_TARGET))
RELEASE_OUTPUT_DIR = Release/$(shell echo $(ANDROID_NDK_ABI_TARGET))
SOURCE_DIR = ../../Source

# Engine compilation setup
ENGINE_ROOT = $(SOURCE_DIR)/Engine
ENGINE_SRC  = $(wildcard $(ENGINE_ROOT)/*.cpp $(ENGINE_ROOT)/*/*.cpp $(ENGINE_ROOT)/*/*/*.cpp $(ENGINE_ROOT)/*/*/*/*.cpp $(ENGINE_ROOT)/*/*/*/*/*.cpp $(ENGINE_ROOT)/*/*/*/*/*/*.cpp)
DEBUG_ENGINE_OBJ    = $(addprefix $(DEBUG_OUTPUT_DIR)/,$(subst /,., $(patsubst $(SOURCE_DIR)/%.cpp, %.o, $(ENGINE_SRC))))
RELEASE_ENGINE_OBJ  = $(addprefix $(RELEASE_OUTPUT_DIR)/,$(subst /,., $(patsubst $(SOURCE_DIR)/%.cpp, %.o, $(ENGINE_SRC))))
DEBUG_ENGINE_LIBS   = -lstdc++
RELEASE_ENGINE_LIBS = -lstdc++
DEBUG_ENGINE_TARGET   = $(DEBUG_OUTPUT_DIR)/Engine.a
RELEASE_ENGINE_TARGET = $(RELEASE_OUTPUT_DIR)/Engine.a

INCLUDE_DIRECTORIES = -I$(shell echo $(ANDROID_NDK_HOME))sources/cxx-stl/gnu-libstdc++/4.6/include/
DEBUG_OPTIMIZE   = -O0
RELEASE_OPTIMIZE = -O3
DEBUG_CFLAGS     = $(INCLUDE_DIRECTORIES) -Wall $(DEBUG_OPTIMIZE) -frtti -std=c++0x -pthread -ggdb
RELEASE_CFLAGS   = $(INCLUDE_DIRECTORIES) -Wall $(RELEASE_OPTIMIZE) -frtti -std=c++0x -pthread

CXX = $(shell echo $(ANDROID_NDK_CXX))
AR  = $(shell echo $(ANDROID_NDK_AR))

$(DEBUG_ENGINE_TARGET): $(DEBUG_ENGINE_OBJ)
	@$(AR) rvs $(DEBUG_ENGINE_TARGET) $(DEBUG_ENGINE_OBJ)

$(RELEASE_ENGINE_TARGET): $(RELEASE_ENGINE_OBJ)
	@$(AR) rvs $(RELEASE_ENGINE_TARGET) $(RELEASE_ENGINE_OBJ)

Debug: $(DEBUG_ENGINE_TARGET)

Release: $(RELEASE_ENGINE_TARGET)

cleanDebug:
	@rm -f $(DEBUG_OUTPUT_DIR)/*

cleanRelease:
	@rm -f $(RELEASE_OUTPUT_DIR)/*

clean: cleanDebug cleanRelease

all: Debug Release


#	@echo "ar: $(AR)"
#	@echo "g++: $(CXX)"
#	@echo "dbg: $(DEBUG_OUTPUT_DIR)"
#	@echo "rls: $(RELEASE_OUTPUT_DIR)"
#	@echo "dbg engine obj: $(DEBUG_ENGINE_OBJ)"

.SECONDEXPANSION:
$(DEBUG_ENGINE_OBJ): $$(addprefix $(SOURCE_DIR)/,$$(subst !cpp,.cpp, $$(subst .,/, $$(patsubst $(DEBUG_OUTPUT_DIR)/%.o, %!cpp, $$@))))
	@mkdir -p $(DEBUG_OUTPUT_DIR)
	@echo "Compiling [DEBUG] $(patsubst $(SOURCE_DIR)/%,%, $?)"
	$(CXX) $(DEBUG_CFLAGS) $? $(DEBUG_ENGINE_LIBS) -c -o $@

$(RELEASE_ENGINE_OBJ): $$(addprefix $(SOURCE_DIR)/,$$(subst !cpp,.cpp, $$(subst .,/, $$(patsubst $(RELEASE_OUTPUT_DIR)/%.o, %!cpp, $$@))))
	@mkdir -p $(RELEASE_OUTPUT_DIR)
	@echo "Compiling [RELEASE] $(patsubst $(SOURCE_DIR)/%,%, $?)"
	$(CXX) $(RELEASE_CFLAGS) $? $(RELEASE_ENGINE_LIBS) -c -o $@
