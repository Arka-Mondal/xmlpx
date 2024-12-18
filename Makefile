# Copyright (c) 2024, Arka Mondal. All rights reserved.
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

BIN := xmlpx
LIBDIR := lib
CC := gcc
CCFLAG := -g -Wall -Wextra

OBJS = main.o reader.o lexer.o $(LIBDIR)/vector.o

$(BIN) : $(OBJS)
	$(CC) -o $@ $+

main.o : main.c
	$(CC) -o $@ -c $< $(CCFLAG)

reader.o : reader.c
	$(CC) -o $@ -c $< $(CCFLAG)

lexer.o : lexer.c
	$(CC) -o $@ -c $< $(CCFLAG)

$(LIBDIR)/vector.o : $(LIBDIR)/vector.c
	$(CC) -o $@ -c $< $(CCFLAG)

.PHONY : clean
clean :
	rm -vf $(OBJS) $(BIN)
