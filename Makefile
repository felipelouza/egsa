CC = gcc
SRC_DIR=src

CWARNING =  -Wall -Wno-unused-result -Wno-implicit-function-declaration -Wno-comment -Wno-format 
#VLIB= -g -O0
MY_CXX_OPT_FLAGS= -O3 -ffast-math -funroll-loops -m64 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64

LFLAGS = -lm -ldl

LIBOBJ = external/malloc_count/malloc_count.o\
		external/sais-lite-LCP.o\
		external/sa-is.o\
		$(SRC_DIR)/heap.o\
		$(SRC_DIR)/utils.o\
		$(SRC_DIR)/file.o\
		$(SRC_DIR)/esa.o\
		$(SRC_DIR)/lcp.o\

##

BWT = 0
DEBUG = 0
MODE = 2 #DNA = 1; READ = 2


DEFINES = -DDEBUG=$(DEBUG) -DMODE=$(MODE) -DBWT=$(BWT)
CFLAGS = $(MY_CXX_OPT_FLAGS) $(CWARNING) $(LFLAGS) $(VLIB) $(DEFINES)

########################################################################

CHECK = 1
PRE = 1

DIR = example/fastq/
#DIR = example/fasta/

INPUT = reads-10000.fastq
#INPUT = all.in

K =  5000
#K =  5

#1024MB
MEMLIMIT = 1 

########################################################################

all: compile

flush:
	sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
	
clean:

	rm $(SRC_DIR)/*.o
#	rm external/*.o
	rm egsa

erase:
	rm $(DIR)*.gesa
	rm $(DIR)*.bin
	rm -Rf $(DIR)*.esa
	rm -Rf $(DIR)partition/*
	rm -Rf $(DIR)tmp/*
	
compile: $(SRC_DIR)/main.c ${LIBOBJ} $(SRC_DIR)/defines.h $(SRC_DIR)/utils.h
	$(CC) $(LIBOBJ) $(CFLAGS) $(SRC_DIR)/main.c -o egsa 

##
LIBH = $(SRC_DIR)/defines.h $(SRC_DIR)/utils.h
esa.o 	: $(LIBH)
heap.o	: $(LIBH)
file.o	: $(LIBH)
utils.o	: $(LIBH)
lcp.o	: $(LIBH)

##

run: egsa
	./egsa $(PRE) $(DIR) $(INPUT) $(K) $(MEMLIMIT) $(CHECK)

valgrind:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./egsa2 $(PRE) $(DIR) $(INPUT) $(K) $(MEMLIMIT) $(CHECK)
