# SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

# LUMI
#CC=CC -O3 -Wall
# Mahti
CC=mpicxx -O3 -Wall

prog.x: prog.o util.o
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) -c $<

.PHONY: clean
clean:
	rm -vf *.x *.o
