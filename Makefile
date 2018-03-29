CC = gcc
O_FLAGS = -c -O3
DO_FLAGS = -c -g

HEADERS = aes.h param_and_types.h compact_tables.h
OBJECTS = utilities.o key_schedule.o aes_core.o

DISTING_O = $(OBJECTS) distinguisher.o
DISTING_DO = $(DISTING_O:.o=.do)

AES_O = $(OBJECTS) aes_test.o
AES_DO = $(AES_O:.o=.do)



.SUFFIXES : .c .o .do
.c.o :; $(CC) -o $@ $(O_FLAGS) $*.c
.c.do :; $(CC) -o $@ $(DO_FLAGS) $*.c




disting : $(DISTING_O)
	$(CC) -o $@ $^
disting.db : $(DISTING_DO)
	$(CC) -o $@ $^

aes_test : $(AES_O)
	$(CC) -o $@ $^
aes_test.db : $(AES_DO)
	$(CC) -o $@ $^




distinguisher.o distinguisher.do : distinguisher.c $(HEADERS)

aes_test.o      aes_test.do      : aes_test.c      $(HEADERS)

aes_core.o      aes_core.do      : aes_core.c      $(HEADERS)

key_schedule.o  key_schedule.do  : key_schedule.c  $(HEADERS)

utilities.o     utilities.do     : utilities.c     $(HEADERS)





compact_tables.h : tables_generator_exe
	./$<

tables_generator_exe : tables_generator.o
	$(CC) -o $@ $^
tables_generator_exe.db : tables_generator.do
	$(CC) -o $@ $^

tables_generator.o tables_generator.do : tables_generator.c param_and_types.h




clean :
	rm -f disting aes_test tables_generator_exe *.db *.o *.do compact_tables.h




