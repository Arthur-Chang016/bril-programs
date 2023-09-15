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
	bril2json < test/add.bril | brili
	bril2json < test/add.bril | build/LVN | build/TDCE | brili

lvn: all
	bril2json < test/add.bril | brili
	bril2json < test/add.bril | build/LVN | brili
	
live: all
	bril2json < test/add.bril | build/Liveness

tdce: all
	bril2json < test/simple.bril | brili
	bril2json < test/simple.bril | build/TDCE | brili
	bril2json < test/reassign.bril | brili
	bril2json < test/reassign.bril | build/TDCE | brili

# Clean target to remove generated executables
clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean

# .SILENT: run