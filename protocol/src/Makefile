IDIR =../include
CC=gcc
CFLAGS=-Wall -O3 -I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lm

_DEPS = *.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o graph.o state.o queue.o compar.o memory.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

protocol: $(OBJ)
	$(CC) -o $@ $^ ../../nauty26r7/nauty1.a $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
