CC = gcc
O_FLAGS = -c -g
DO_FLAGS = -c -g -O3

d : utilities.o key_schedule.o aes_core.o distinguisher.o
	$(CC) -o $@  utilities.o key_schedule.o aes_core.o distinguisher.o

distinguisher.o : distinguisher.c aes.h param_and_types.h compact_tables.h
	$(CC) -o $@ $(O_FLAGS) distinguisher.c

aes_core_test : aes_core.o key_schedule.o utilities.o
	$(CC) -o $@ aes_core.o  key_schedule.o utilities.o

aes_core.o : aes_core.c aes.h param_and_types.h compact_tables.h
	$(CC) -o $@ $(O_FLAGS) aes_core.c

key_schedule.o : key_schedule.c aes.h param_and_types.h compact_tables.h
	$(CC) -o $@ $(O_FLAGS) key_schedule.c

utilities.o : utilities.c aes.h param_and_types.h compact_tables.h
	$(CC) -o $@ $(O_FLAGS) utilities.c

compact_tables.h : tables_generator_exe
	./$<

tables_generator_exe : tables_generator.c param_and_types.h
	$(CC) -o $@ tables_generator.c

clean :
	rm -f d distinguisher.o aes_core.o key_schedule.o compact_tables.h




