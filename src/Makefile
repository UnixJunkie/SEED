# Makefile for SEED 4.0.0
# clangini 2016
#SHELL = /bin/bash
# Folders: 
#SRC_DIR = .
MKFILE_PATH = $(abspath $(firstword $(MAKEFILE_LIST)))
CURR_DIR = $(dir $(MKFILE_PATH))
BIN_DIR = $(CURR_DIR)../bin

BUILD_DIR = $(CURR_DIR)build
BUILD_DIR_MPI = $(CURR_DIR)build/mpi
BUILD_DIR_DEBUG = $(CURR_DIR)build/dbg

TARGET_EXEC = $(BIN_DIR)/seed_4_mc_lb
TARGET_EXEC_MPI = $(BIN_DIR)/seed_4_mpi_mc_lb
TARGET_EXEC_DEBUG = $(BIN_DIR)/seed_4_mc_lsb_debug

# Flags:
CXX = g++
CXXFLAGS = -std=c++11 -O3 -pedantic -Wall -march=native
LDFLAGS = -lm -O3 -static

INC_FLAGS = -I$(CURR_DIR) -I$(CURR_DIR)../lib/

# For seed MPI:
MPICXX = mpic++
MPICXXFLAGS = -std=c++11 -O3 -pedantic -Wall -march=native
MPILDFLAGS = -lm -O3

DBGCXXFLAGS = -g3 -std=c++11 -O3 -pedantic -Wall -march=native

include $(CURR_DIR)Makefile.local

DEP_FLAGS = $(INC_FLAGS) -MMD -MP

PRESRCLIST = align_frag assire checkresn cofren cogrep conubu convert2pp count_heavy_atom extoutnam featres fialhy findfrsym fracdo geomcent geomfu hybstat listchres main makbsatlist makspv montecarlo nrutil otfunc Parameter psspee psspma reacdo reduc_polvectre refrfi_coo_mol2 refrfi_mol2_new reinfi relaic relaic_en rerefi_mol2 rnd_namespace rosefr seedfr_ap seedfr simila solv_frag solv_frag_fast solv_lookup solv_util sort_all sort sqdisfrre_ps vwfren vwgrep write_charmm write_chm_clus write_mol2
SRCLIST = $(PRESRCLIST:%=%.cpp)

SRCS = $(SRCLIST)
OBJS = $(SRCLIST:%.cpp=$(BUILD_DIR)/%.o)
OBJS_DEBUG = $(SRCLIST:%.cpp=$(BUILD_DIR_DEBUG)/%.o)
OBJS_MPI = $(SRCLIST:%.cpp=$(BUILD_DIR_MPI)/%.o)
DEPS = $(OBJS:%.o=%.d)
DEPS_DEBUG = $(OBJS_DEBUG:%.o=%.d)
DEPS_MPI = $(OBJS_MPI:%.o=%.d)

# Commands:
RM = rm
MKDIR_P = mkdir -p
RMDIR = rmdir
ECHO = @echo
# PWD = shell pwd

# Targets:
.PHONY: seed
seed: $(TARGET_EXEC)

.PHONY: seed_debug
seed_debug: $(TARGET_EXEC_DEBUG)

.PHONY: seed_mpi
seed_mpi: $(TARGET_EXEC_MPI)

$(TARGET_EXEC): $(OBJS)
	$(MKDIR_P) $(BIN_DIR)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(TARGET_EXEC_DEBUG): $(OBJS_DEBUG)
	$(CXX) $(OBJS_DEBUG) $(LDFLAGS) -o $@

$(TARGET_EXEC_MPI): $(OBJS_MPI)
	$(MPICXX) $(OBJS_MPI) $(MPILDFLAGS) -o $@

# Using order-only prerequisite to build the directory
$(BUILD_DIR)/%.o: $(CURR_DIR)%.cpp | $(BUILD_DIR)
	$(CXX) -c $(CXXFLAGS) $(DEP_FLAGS) $< -o $@

$(BUILD_DIR_DEBUG)/%.o: $(CURR_DIR)%.cpp | $(BUILD_DIR_DEBUG)
	$(CXX) -c $(DBGCXXFLAGS) $(DEP_FLAGS) $< -o $@

$(BUILD_DIR_MPI)/%.o: $(CURR_DIR)%.cpp | $(BUILD_DIR_MPI)
	$(MPICXX) -c $(MPICXXFLAGS) -DENABLE_MPI $(DEP_FLAGS) $< -o $@

$(BUILD_DIR):
	$(MKDIR_P) $@

$(BUILD_DIR_DEBUG):
	$(MKDIR_P) $@

$(BUILD_DIR_MPI): $(BUILD_DIR)
	$(MKDIR_P) $@

-include $(DEPS)
-include $(DEPS_DEBUG)
-include $(DEPS_MPI)

.PHONY: print
print:
	$(ECHO) $(PRESRCLIST)
	$(ECHO) $(SRCLIST)
	$(ECHO) $(SRCS)
	$(ECHO) $(OBJS)
	$(ECHO) $(DEPS)
	$(ECHO)
	$(ECHO)
	$(ECHO) $(MAKEFILE_LIST)
	$(ECHO) $(MKFILE_PATH)
	$(ECHO) $(CURR_DIR)

.PHONY: objclean
objclean:
	for i in $(OBJS_MPI); do (if [ -e $${i} ]; then $(RM) $${i}; fi;) done
	for i in $(DEPS_MPI); do (if [ -e $${i} ]; then $(RM) $${i}; fi;) done
	for i in $(OBJS_DEBUG); do (if [ -e $${i} ]; then $(RM) $${i}; fi;) done
	for i in $(DEPS_DEBUG); do (if [ -e $${i} ]; then $(RM) $${i}; fi;) done
	for i in $(OBJS); do (if [ -e $${i} ]; then $(RM) $${i}; fi;) done
	for i in $(DEPS); do (if [ -e $${i} ]; then $(RM) $${i}; fi;) done
	if [ -d $(BUILD_DIR_MPI) ]; then $(RMDIR) $(BUILD_DIR_MPI); fi
	if [ -d $(BUILD_DIR_DEBUG) ]; then $(RMDIR) $(BUILD_DIR_DEBUG); fi
	if [ -d $(BUILD_DIR) ]; then $(RMDIR) $(BUILD_DIR); fi

.PHONY: clean
clean: objclean
	if [ -e $(TARGET_EXEC) ]; then $(RM) $(TARGET_EXEC); fi
	if [ -e $(TARGET_EXEC_DEBUG) ]; then $(RM) $(TARGET_EXEC_DEBUG); fi
	if [ -e $(TARGET_EXEC_MPI) ]; then $(RM) $(TARGET_EXEC_MPI); fi
