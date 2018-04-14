PATH = E:/Development/mingw64/bin
CXX = $(PATH)/g++.exe
CC = $(PATH)/gcc.exe
INCLUDE_PATH = ./include
TARGET = nes.exe

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj

CXX_FLAGS = -g -Wall -I$(INCLUDE_PATH)
LD_FLAGS = -L./lib -opengl32 -lglad -lglfw3

all: $(BUILD_DIR)/$(TARGET)

NES_SRCS = $(wildcard ./src/nes/*.cpp)
NES_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(NES_SRCS:%.cpp=%.o)))

$(NES_OBJS): $(OBJ_DIR)/%.o: ./src/nes/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

UI_SRCS = $(wildcard ./src/ui/*.cpp)
UI_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(UI_SRCS:%.cpp=%.o)))

$(UI_OBJS): $(OBJ_DIR)/%.o: ./src/ui/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

SRCS = $(wildcard ./src/*.cpp)
OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:%.cpp=%.o)))

$(OBJS): $(OBJ_DIR)/%.o: ./src/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

$(BUILD_DIR)/$(TARGET): $(NES_OBJS) $(UI_OBJS) $(OBJS) 
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) -o $@ $^

clean:
	del build\obj\*.o
	del build\*.exe