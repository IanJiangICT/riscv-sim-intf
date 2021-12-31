SRC_DIR = ../src

CC = gcc

CFLAGS += -Wall
CFLAGS += -I $(SRC_DIR)

src = $(SRC_DIR)/sc_func.c
header = $(SRC_DIR)/sc_types.h

tests = sc_test0
tests += sc_test1
tests += sc_test2
tests += sc_test3
tests += sc_test4

default: all

all: $(tests)

clean:
	rm -rf $(tests)

sc_test0: sc_test0.c $(src) $(header)
	$(CC) $(CFLAGS) $(src) sc_test0.c -o $@

sc_test1: sc_test1.c $(src) $(header)
	$(CC) $(CFLAGS) $(src) sc_test1.c -o $@

sc_test2: sc_test2.c $(src) $(header)
	$(CC) $(CFLAGS) $(src) sc_test2.c -o $@

sc_test3: sc_test3.c $(src) $(header)
	$(CC) $(CFLAGS) -fno-stack-protector $(src) sc_test3.c -o $@

sc_test4: sc_test4.c $(src) $(header)
	$(CC) $(CFLAGS) $(src) sc_test4.c -o $@
