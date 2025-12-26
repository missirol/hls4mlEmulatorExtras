CXX := g++
CXX_STANDARD := c++2a
CXXFLAGS := -O3 -fPIC -std=$(CXX_STANDARD)
INCLUDES := -Iinclude/hls4ml
PREFIX := .

CURDIR_FULLPATH := $(shell readlink -m ${PWD})
PREFIX_FULLPATH := $(shell readlink -m $(PREFIX))

.DEFAULT_GOAL := all
.PHONY: all install test clean

# -----------------------------------------------------------------------------
# Shared Library
# -----------------------------------------------------------------------------
SHAREDLIB_DIR := $(PREFIX)/lib64
SHAREDLIB_NAME := emulator_interface
SHAREDLIB := $(SHAREDLIB_DIR)/lib$(SHAREDLIB_NAME).so

# -----------------------------------------------------------------------------
# Unit Tests
# -----------------------------------------------------------------------------
TEST_MODEL_PREFIX := test/TestModel
TEST_MODEL_SRC := $(wildcard $(TEST_MODEL_PREFIX)*.cc)
TEST_MODEL_SO := $(TEST_MODEL_SRC:.cc=.so)

TEST_MODELWRAPPER := test/testModelWrapper

# -----------------------------------------------------------------------------
# Make Rules
# -----------------------------------------------------------------------------
$(SHAREDLIB): src/hls4ml/ModelWrapper.cc
	mkdir -p $(SHAREDLIB_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -shared -ldl $< -o $@

$(TEST_MODEL_PREFIX)%.so: $(TEST_MODEL_PREFIX)%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -shared $< -o $@

$(TEST_MODELWRAPPER): $(TEST_MODELWRAPPER).cc $(SHAREDLIB)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -L$(SHAREDLIB_DIR) -l$(SHAREDLIB_NAME) -o $@

# -----------------------------------------------------------------------------
# Make Targets
# -----------------------------------------------------------------------------
all: $(SHAREDLIB)

install: $(SHAREDLIB)
ifneq ("$(PREFIX_FULLPATH)","$(CURDIR_FULLPATH)")
	rm -rf $(PREFIX)/include && cp -r include $(PREFIX)/include
endif

test: $(TEST_MODELWRAPPER) $(TEST_MODEL_SO)
	LD_LIBRARY_PATH=$(SHAREDLIB_DIR):test:${LD_LIBRARY_PATH} $(TEST_MODELWRAPPER)

clean:
	rm -rf $(SHAREDLIB_DIR) $(TEST_MODEL_SO) $(TEST_MODELWRAPPER)
ifneq ("$(PREFIX_FULLPATH)","$(CURDIR_FULLPATH)")
	rm -rf $(PREFIX)/include
endif
