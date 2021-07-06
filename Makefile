#*******************************************************
 # This file is part of the Blaz library
 # @Name ........ : Makefile
 # @Role ........ : Compile blaz sources
 # @Author ...... : Matthieu Martel
 # @Version ..... : V1.1 05/07/2021
 # @Licence ..... : GPL V3
 # @Link ........ : https://github.com/mmartel66/blaz.git
#*******************************************************/


CC = gcc

CURDIR = $(shell pwd)

SRCDIR = $(CURDIR)/src

ODIR = $(CURDIR)/obj

BINDIR = $(CURDIR)/bin

EXDIR = $(CURDIR)/examples

LIBS = -lm

CFLAGS := -I$(CURDIR)/src -O2


OBJ = $(ODIR)/block_delta.o $(ODIR)/block_slope.o $(ODIR)/dct.o $(ODIR)/io.o $(ODIR)/compress.o $(ODIR)/operations.o $(ODIR)/compressed_operations.o

OBJADD = $(OBJ) $(ODIR)/add.o
OBJMULCST = $(OBJ) $(ODIR)/mul_cst.o
OBJDOTPROD = $(OBJ) $(ODIR)/dot_product.o
OBJFILES = $(OBJ) $(ODIR)/files.o


add: $(OBJADD)
		$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LIBS)

mul_cst: $(OBJMULCST)
		$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LIBS)

dot_product: $(OBJDOTPROD)
		$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LIBS)

files: $(OBJFILES)
		$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LIBS)


$(ODIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/add.o: $(EXDIR)/add.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/mul_cst.o: $(EXDIR)/mul_cst.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/dot_product.o: $(EXDIR)/dot_product.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/files.o: $(EXDIR)/files.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(ODIR)/*.o
	rm -rf $(BINDIR)/*

all: clean add mul_cst dot_product files
