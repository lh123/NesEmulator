PATH = E:/Development/mingw64/bin
CXX = $(PATH)/g++.exe
CC = $(PATH)/gcc.exe
INCLUDE_PATH = -I./include -IE:/VSCode/CommonLib/include
LIB_PATH = -LE:/VSCode/CommonLib/lib
TARGET = nes.exe

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj

CXX_FLAGS = -g -Wall $(INCLUDE_PATH)
LD_FLAGS = $(LIB_PATH) -static -Wl,-Bstatic -lglfw3 -lglad -limgui -Wl,-Bdynamic -lSDL2 -lws2_32 -lcomdlg32 -Wl,-Bstatic -lopengl32 -lgdi32

all: $(BUILD_DIR)/$(TARGET)

NES_SRCS = $(wildcard ./src/nes/*.cpp)
NES_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(NES_SRCS:%.cpp=%.o)))

$(NES_OBJS): $(OBJ_DIR)/%.o: ./src/nes/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

UI_SRCS = $(wildcard ./src/ui/*.cpp)
UI_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(UI_SRCS:%.cpp=%.o)))

$(UI_OBJS): $(OBJ_DIR)/%.o: ./src/ui/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

NET_SRCS = $(wildcard ./src/net/*.cpp)
NET_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(NET_SRCS:%.cpp=%.o)))

$(NET_OBJS): $(OBJ_DIR)/%.o: ./src/net/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

SRCS = $(wildcard ./src/*.cpp)
OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:%.cpp=%.o)))

$(OBJS): $(OBJ_DIR)/%.o: ./src/%.cpp
	$(CXX) -c $(CXX_FLAGS) -o $@ $<

$(BUILD_DIR)/$(TARGET): $(NES_OBJS) $(UI_OBJS) $(NET_OBJS) $(OBJS) 
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	del build\obj\*.o
	del build\*.exe