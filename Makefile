SRCS := $(wildcard src/*.cpp)
SRCS += $(wildcard src/*.mm)
OBJS := $(patsubst src/%.cpp,build/%.o, $(SRCS))
OBJS := $(patsubst src/%.mm,build/%.o, $(OBJS))
DEFS := -arch i386 -DFEATURE_MULTITEXTURE -DFEATURE_SPECULAR -DFEATURE_DISPLACEMENT -DFEATURE_REFLECT -DFEATURE_GLOW -DFEATURE_BLUR -DFEATURE_WATER -DFEATURE_OGG_STREAM -DFEATURE_OGG -DOGG_STREAM -DDEVELOPER
LIBS := libjpg6bmac/libjpeg.a  -framework IOKit -framework Cocoa -framework carbon -framework corefoundation -framework OpenGL -framework GLUT -framework OpenAL  lua-5.1.5/src/liblua.a myvorbis/myvorbis.a /Users/research/Downloads/OculusSDK/LibOVR/Lib/Mac/Release/libovr.a 
INC := -I /Users/research/Documents/bajaengine/libjpg6bmac  -I /Users/research/Downloads/OculusSDK/LibOVR/Include  -I /Users/research/Downloads/OculusSDK/LibOVR/Src
#
all: bajaengine

build/%.o : src/%.cpp
	g++ $(DEFS) -Wno-deprecated $(INC)  -I lua-5.1.5/src $(shell pkg-config --cflags vorbis) $(shell pkg-config --cflags ogg)  -I src $(shell pkg-config --cflags openal) -c -o $@ $<

build/%.o : src/%.mm
	g++ $(DEFS) -Wno-deprecated $(INC) -I lua-5.1.5/src $(shell pkg-config --cflags vorbis) $(shell pkg-config --cflags ogg) -I src $(shell pkg-config --cflags openal) -c -o $@ $<

bajaengine: $(OBJS)
	g++ $(DEFS) $(LIBS) -o bajaengine $^

clean:
	rm build/*