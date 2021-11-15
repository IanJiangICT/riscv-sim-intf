SRC_DIR = ../src

CC = gcc

CFLAGS += -Wall
CFLAGS += -I $(SRC_DIR)

tests = sc_test

default: all

all: $(tests)

clean:
	rm -rf $(tests)

sc_test: sc_test.c $(SRC_DIR)/sc_func.c
	$(CC) $(CFLAGS) $(SRC_DIR)/sc_func.c sc_test.c -o $@


