IDIR =../include
CC=gcc
CFLAGS=-Wall -O3 -I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lm ../../nauty26r7/nauty.a

_DEPS = *.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o graph.o state.o queue.o compar.o memory.o test.o simulations.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

protocol: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
