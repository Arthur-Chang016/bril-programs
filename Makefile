# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++11 -Wall -I./lib -I./include

# Directories
SRCDIR := opt
BUILDDIR := build

# Get all CPP files in the source directory
SRCS := $(wildcard $(SRCDIR)/*.cpp)

# Generate a list of executable targets based on source file names
EXECS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%,$(SRCS))

# Build all targets
all: $(EXECS)

# Rule to build an executable from a CPP source file
$(BUILDDIR)/%: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Create the build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)
	
	
# Miscellaneous
run: all
	# bril2json < test/print/add.bril | brili
	# build/LVN < test/add.json
	# bril2json < test/add.bril | build/LVN
	bril2json < test/eight-queens.bril | build/LVN
	


# Clean target to remove generated executables
clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean
