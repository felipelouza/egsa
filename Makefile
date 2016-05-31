CC = gcc
LIB_DIR=lib

CWARNING =  -Wall -Wno-implicit-function-declaration  
#VLIB= -g -O0
MY_CXX_OPT_FLAGS= -O3 -m64 -D_FILE_OFFSET_BITS=64
#-fomit-frame-pointer -ffast-math -funroll-loops 

LFLAGS = -lm -ldl

LIBOBJ = external/malloc_count/malloc_count.o\
		external/gsaca-k.o\
		external/sais-lite-LCP.o\
		$(LIB_DIR)/utils.o\
		$(LIB_DIR)/file.o\
		$(LIB_DIR)/heap.o\
		$(LIB_DIR)/esa.o\
		$(LIB_DIR)/lcp.o\
		src/egsa.o\
##
BWT = 0
DEBUG = 0
##

DEFINES = -DDEBUG=$(DEBUG) -DBWT=$(BWT) 

CFLAGS = $(MY_CXX_OPT_FLAGS) $(CWARNING) $(LFLAGS) $(VLIB) $(DEFINES)

########################################################################

CHECK = 1
PRE = 1

DIR = dataset/

INPUT = proteins-10000.fasta
#INPUT = input-10000.txt
#INPUT = reads-10000.fastq

K =  100

#4GB
MEMLIMIT = 2048 

########################################################################

all: compile

flush:
	sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
	
clean:
	rm -f $(LIB_DIR)/*.o
	rm -f src/*.o
	rm -f external/*.o
	rm -f egsa-1.2

remove:
	rm -f $(DIR)*.gesa
	rm -f $(DIR)*.bin
	rm -Rf $(DIR)*.esa
	rm -Rf $(DIR)partition/*
	rm -Rf $(DIR)tmp/*	

##
LIBH = $(LIB_DIR)/defines.h $(LIB_DIR)/utils.h
$(LIB_DIR)/esa.o 	: $(LIBH)
$(LIB_DIR)/heap.o	: $(LIBH)
$(LIB_DIR)/file.o	: $(LIBH)
$(LIB_DIR)/utils.o	: $(LIBH)
$(LIB_DIR)/lcp.o	: $(LIBH)

##

compile: main.c ${LIBOBJ} $(LIB_DIR)/defines.h $(LIB_DIR)/utils.h
	$(CC) $(LIBOBJ) $(CFLAGS) main.c -o egsa-1.2

run: egsa-1.2
	./egsa-1.2 $(PRE) $(DIR) $(INPUT) $(K) $(MEMLIMIT) $(CHECK)

valgrind:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./egsa-1.2 $(PRE) $(DIR) $(INPUT) $(K) $(MEMLIMIT) $(CHECK)

esais_compile: external/esais_input.c ${LIBOBJ} $(LIB_DIR)/defines.h $(LIB_DIR)/utils.h
	$(CC) $(LIBOBJ) $(CFLAGS) external/esais_input.c -o external/esais_input

esais_input: external/esais_input
	external/esais_input $(DIR) $(INPUT) $(K) 
