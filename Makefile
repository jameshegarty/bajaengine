SRCS := $(wildcard src/*.cpp)
SRCS += $(wildcard src/*.mm)
OBJS := $(patsubst src/%.cpp,build/%.o, $(SRCS))
OBJS := $(patsubst src/%.mm,build/%.o, $(OBJS))
DEFS := -arch i386 -DFEATURE_MULTITEXTURE -DFEATURE_SPECULAR -DFEATURE_DISPLACEMENT -DFEATURE_REFLECT -DFEATURE_GLOW -DFEATURE_BLUR -DFEATURE_WATER -DFEATURE_OGG_STREAM -DFEATURE_OGG -DOGG_STREAM -DDEVELOPER
LIBS := libjpg6bmac/libjpeg.a  -framework IOKit -framework Cocoa -framework carbon -framework corefoundation -framework OpenGL -framework GLUT -framework OpenAL  lua-5.1.5/src/liblua.a myvorbis/myvorbis.a /Users/research/Downloads/OculusSDK/LibOVR/Lib/Mac/Release/libovr.a 
INC := -I /Users/research/Documents/bajaengine/libjpg6bmac  -I /Users/research/Downloads/OculusSDK/LibOVR/Include  -I /Users/research/Downloads/OculusSDK/LibOVR/Src
CFLAGS := -g -O0 -fmessage-length=0 -fdiagnostics-show-note-include-stack -fmacro-backtrace-limit=0 -Wno-trigraphs -fpascal-strings -Wno-missing-field-initializers -Wno-missing-prototypes -Wno-non-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-missing-braces -Wparentheses -Wswitch -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wunused-variable -Wunused-value -Wno-empty-body -Wno-uninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wno-constant-conversion -Wno-int-conversion -Wno-bool-conversion -Wno-enum-conversion -Wno-shorten-64-to-32 -Wno-newline-eof -Wno-c++11-extensions -DFEATURE_MULTITEXTURE -DFEATURE_SPECULAR -DFEATURE_DISPLACEMENT -DFEATURE_REFLECT -DFEATURE_GLOW -DFEATURE_BLUR -DFEATURE_WATER -DFEATURE_OGG_STREAM -DFEATURE_OGG -DOGG_STREAM -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk -fasm-blocks -fstrict-aliasing -Wdeprecated-declarations -Winvalid-offsetof -mmacosx-version-min=10.9 -g -fvisibility=hidden -fvisibility-inlines-hidden -Wno-sign-conversion 
#
all: bajaengine

build/%.o : src/%.cpp
	clang -x c++ $(CFLAGS) $(DEFS) -Wno-deprecated $(INC)  -I lua-5.1.5/src $(shell pkg-config --cflags vorbis) $(shell pkg-config --cflags ogg)  -I src $(shell pkg-config --cflags openal) -c -o $@ $<

build/%.o : src/%.mm
	clang -x objective-c++ $(CFLAGS) $(DEFS) -Wno-deprecated $(INC) -I lua-5.1.5/src $(shell pkg-config --cflags vorbis) $(shell pkg-config --cflags ogg) -I src $(shell pkg-config --cflags openal) -c -o $@ $<

bajaengine: $(OBJS)
	g++ $(CFLAGS) $(DEFS) $(LIBS) -o bajaengine $^

clean:
	rm build/*