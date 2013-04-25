BINS = \
    0_empty_window.bin \
    1_triangle.bin \
    2_texture.bin \
    3_cube.bin \
    4_light.bin \
    5_bump.bin

SHARED_OBJS = \
    shared_main.o \
    shared_tex.o \
    shared_utils.o

LIBS = -lglfw

OS := $(shell uname -s)
ifeq ($(OS),Darwin)
	LIBS += -framework OpenGL
else
	LIBS += -lGL
endif

all: $(BINS)

%.bin: %.o $(SHARED_OBJS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -rf *.bin *.o
.PHONY: clean

