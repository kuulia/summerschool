# SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

FC=ftn -fopenmp -O3 -hipa0

main.x: kernels.o main.o
	$(FC) $^ -o $@

%.o: %.F90
	$(FC) -c $<

.PHONY: clean
clean:
	rm -vf *.x *.o *.mod
