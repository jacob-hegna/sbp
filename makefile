BUILDDIR    = ./bin/
SOURCEDIR   = ./src/

DEFINES     =

SDIRS       = $(wildcard $(SOURCEDIR)*/)
VPATH       = $(SOURCEDIR):$(SDIRS):$(foreach dir, $(SDIRS), $(wildcard $(dir)*/))

IFILES      = $(shell find $(SOURCEDIR) -name '*.cpp')

OFILES      = $(subst $(SOURCEDIR), $(BUILDDIR), $(addsuffix .o, $(notdir $(shell find $(SOURCEDIR)  -name '*.cpp'))))

CC          = g++
LINKER      = $(CC)
ARCH        = 64

CCFLAGS     = -c -g -w -O2 -m$(ARCH) -std=c++11
LINKFLAGS   = -m$(ARCH)


TARGET = sbp

.PHONY: all clean

all: $(BUILDDIR) $(TARGET)

$(TARGET): $(foreach file, $(OFILES), $(BUILDDIR)$(file))
	$(LINKER) $^ $(LINKFLAGS) -o $(BUILDDIR)$@

$(BUILDDIR)%.cpp.o: %.cpp
	$(CC) $(foreach def, $(DEFINES), -D $(def)) $(CCFLAGS) $< -o $@

clean:
	rm $(BUILDDIR)*