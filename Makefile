SRCS := $(wildcard src/*.cpp)
SRCS += $(wildcard src/*.mm)
OBJS := $(patsubst src/%.cpp,build/%.o, $(SRCS))
OBJS := $(patsubst src/%.mm,build/%.o, $(OBJS))
DEFS := -arch i386 -DFEATURE_MULTITEXTURE -DFEATURE_SPECULAR -DFEATURE_DISPLACEMENT -DFEATURE_REFLECT -DFEATURE_GLOW -DFEATURE_BLUR -DFEATURE_WATER -DFEATURE_OGG_STREAM -DFEATURE_OGG -DOGG_STREAM -DDEVELOPER

all: bajaengine

build/%.o : src/%.cpp
	g++ $(DEFS) -Wno-deprecated  -I lua-5.1.5/src $(shell pkg-config --cflags vorbis) $(shell pkg-config --cflags ogg)  -I src $(shell pkg-config --cflags openal) -c -o $@ $<

build/%.o : src/%.mm
	g++ $(DEFS) -Wno-deprecated  -I lua-5.1.5/src $(shell pkg-config --cflags vorbis) $(shell pkg-config --cflags ogg) -I src $(shell pkg-config --cflags openal) -c -o $@ $<

bajaengine: $(OBJS)
	g++ $(DEFS) -framework Cocoa -framework carbon -framework corefoundation -framework OpenGL -framework GLUT $(shell pkg-config --libs openal) $(shell pkg-config --libs vorbis) $(shell pkg-config --libs ogg) $(shell pkg-config --libs vorbisfile) lua-5.1.5/src/liblua.a -o bajaengine $^

clean:
	rm build/*