CC	= icc
#CFLAGS	= -lm -O3 -lcudnn -lcublas -lcudart_static -lrt -lpthread -ldl -fopenmp -I/usr/local/cuda/include -L/usr/local/cuda/lib64/stubs -L/usr/local/cuda/lib64 -lhdf5
CFLAGS	= -O3 -lcudnn -lcublas -lcudart_static -lrt -lpthread -ldl -fopenmp -I/usr/local/cuda/include -L/usr/local/cuda/lib64/stubs -L/usr/local/cuda/lib64 -lhdf5 -g

TAR1	= bin/NoinHDF5_test
OBJS1	= src/NoinHDF5.o src/NoinHDF5_test.o


all: $(TAR1)
	rm src/*.o

NoinHDF5_test: $(TAR1)
	rm src/*.o


$(TAR1): $(OBJS1)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE) $(LIBPATH)

clean:
	rm src/*.o
