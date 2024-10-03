CC := g++
CFLAGS := -std=c++17 -fdiagnostics-color=always -Wall -g -I$(CURDIR)/dependencies/include
LDFLAGS := -L$(CURDIR)/dependencies/library
# GLFW := $(CURDIR)/dependencies/library/libglfw.3.3.dylib
GLAD := $(CURDIR)/glad.c
LDLIBS := -lglfw.3.3 -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation -Wno-deprecated

app: main.cpp 
	$(CC) $(CFLAGS) $(LDFLAGS) $(GLAD) $^ -o $@ $(LDLIBS)