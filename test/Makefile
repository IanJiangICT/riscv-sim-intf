SRC_DIR = ../src

CC = gcc

CFLAGS += -Wall
CFLAGS += -I $(SRC_DIR)

tests = sc_test0
tests += sc_test1
tests += sc_test2
tests += sc_test3

default: all

all: $(tests)

clean:
	rm -rf $(tests)

sc_test0: sc_test0.c $(SRC_DIR)/sc_func.c
	$(CC) $(CFLAGS) $(SRC_DIR)/sc_func.c sc_test0.c -o $@

sc_test1: sc_test1.c $(SRC_DIR)/sc_func.c
	$(CC) $(CFLAGS) $(SRC_DIR)/sc_func.c sc_test1.c -o $@

sc_test2: sc_test2.c $(SRC_DIR)/sc_func.c
	$(CC) $(CFLAGS) $(SRC_DIR)/sc_func.c sc_test2.c -o $@

sc_test3: sc_test3.c $(SRC_DIR)/sc_func.c
	$(CC) $(CFLAGS) $(SRC_DIR)/sc_func.c sc_test3.c -o $@
