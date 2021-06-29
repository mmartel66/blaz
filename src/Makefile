
CC = gcc

ODIR = obj

BINDIR = bin

EXDIR = ../examples

LIBS = -lm

CFLAGS := -I.

OBJ = $(ODIR)/block_delta.o $(ODIR)/block_slope.o $(ODIR)/dct.o $(ODIR)/io.o $(ODIR)/compress.o $(ODIR)/operations.o

OBJADD = $(OBJ) $(ODIR)/add.o

compress: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

add: $(OBJADD)
		$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LIBS)


$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/add.o: $(EXDIR)/add.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(ODIR)/*.o
	rm -rf $(BINDIR)/*

all: clean add
