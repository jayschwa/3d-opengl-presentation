BINS = \
    0_empty_window.bin \
    1_triangle.bin \
    2_texture.bin

all: $(BINS)

%.bin: %.o shared_main.o shared_utils.o
	$(CC) -o $@ $^ -lGL -lglfw

clean:
	rm -rf *.bin *.o
.PHONY: clean

