BINS = \
    0_empty_window.bin \
    1_triangle.bin \
    2_texture.bin

SHARED_OBJS = \
    shared_main.o \
    shared_tex.o \
    shared_utils.o

all: $(BINS)

%.bin: %.o $(SHARED_OBJS)
	$(CC) -o $@ $^ -lGL -lglfw

clean:
	rm -rf *.bin *.o
.PHONY: clean

