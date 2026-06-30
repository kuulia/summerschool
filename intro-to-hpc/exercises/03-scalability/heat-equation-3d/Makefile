# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

.DEFAULT_GOAL := all

# default to LUMI configuration
ifeq ($(PLATFORM),)
PLATFORM=lumi
endif

$(info PLATFORM: $(PLATFORM))

DFLAGS=-DMPI_NEIGHBORHOOD

# PLATFORM=generic is intended for laptops and local workstations. Should work on any Unix-like system provided that MPI is available
ifeq ($(PLATFORM),generic)
CXX=mpicxx
CC=mpicc
CCFLAGS=-fopenmp -g -O3 -march=native -Wall -DNDEBUG $(DFLAGS)
LDFLAGS=-fopenmp
LIBS=
endif

ifeq ($(PLATFORM),mahti)
CXX=mpicxx
CC=mpicc
CCFLAGS=-fopenmp -O3 -march=native -Wall -DNDEBUG $(DFLAGS)
LDFLAGS=-fopenmp
LIBS=
endif

ifeq ($(PLATFORM),lumi)
CXX=CC
CC=cc
CCFLAGS=-fopenmp -O3 -Wall $(DFLAGS)
LDFLAGS=-fopenmp
LIBS=
endif

BUILDDIR := build

# Override before including so heat-shared artifacts land in our build tree
HEAT_COMMON_LIB := $(BUILDDIR)/heat_common.a

HEAT_SHARED_DIR := ../../../../heat-shared
include $(HEAT_SHARED_DIR)/Makefile-common
CCFLAGS += $(HEAT_SHARED_CFLAGS)

EXE=heat3d
EXE_FULL=$(BUILDDIR)/$(EXE)
OBJS=$(addprefix $(BUILDDIR)/,main.o heat.o core.o setup.o io.o utilities.o)
DEPS=$(OBJS:.o=.d)

all: $(EXE)

$(BUILDDIR):
	@mkdir -p $@

# Just copies the produced executable to Makefile directory
$(EXE): $(EXE_FULL)
	cp $< $@

$(EXE_FULL): $(OBJS) $(HEAT_COMMON_LIB)
	$(CXX) $(LDFLAGS) $(OBJS) $(HEAT_COMMON_LIB) $(LIBS) -o $@

$(BUILDDIR)/%.o: %.cpp | $(BUILDDIR)
	$(CXX) $(CCFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) $(CCFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

.PHONY: clean
clean: heat-common-clean
	-/bin/rm -rf build $(EXE)
