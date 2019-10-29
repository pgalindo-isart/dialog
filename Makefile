MAKEFLAGS+=--no-builtin-rules

CXXFLAGS=-O0 -g -Wall -MMD -Wno-unused-function
CXX?=g++

# System config (à changer notamment là où vous avez installé les fichiers de dev de la SFML)
LDFLAGS+=-L$(HOME)/user-sysroot/lib/
CXXFLAGS+=-I$(HOME)/user-sysroot/include/

COMMON_OBJS=src/main.o src/gui.o src/font.o # src/dialog.o
SFML_OBJS=src/platform_sfml.o src/renderer_sfml.o
SDL_OBJS=src/platform_sdl.o src/renderer_sdl.o

OBJS=$(COMMON_OBJS) $(SFML_OBJS) $(SDL_OBJS)
DEPS=$(OBJS:.o=.d)

.PHONY: all clean

# Change to dialog_sfml to build sfml version by default
all: dialog_sdl

-include $(DEPS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

# SFML version
dialog_sfml: CXXFLAGS+=-DPLATFORM_SFML
dialog_sfml: private LDLIBS+=-lsfml-system -lsfml-window -lsfml-graphics
dialog_sfml: $(COMMON_OBJS) $(SFML_OBJS) src/dialog.o
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)

# SDL version
dialog_sdl: CXXFLAGS+=-DPLATFORM_SDL
dialog_sdl: private LDLIBS+=-lSDL2
dialog_sdl: $(COMMON_OBJS) $(SDL_OBJS) src/dialog.o
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f dialog_sdl dialog_sfml