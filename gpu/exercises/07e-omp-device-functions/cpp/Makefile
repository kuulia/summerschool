# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

CC=CC -fopenmp -O3 -Wall

main.x: main.o kernels.o
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) -c $<

.PHONY: clean
clean:
	rm -vf *.x *.o
