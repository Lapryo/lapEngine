### Build static library libcore.a from source files
# Variables
SRCDIR := source
INCDIR := include
LIBDIR := ../lib
BUILDDIR := ../build/core
CC := g++
AR := ar
CFLAGS := -I$(INCDIR) -O2 -fPIC
ARFLAGS := rcs

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
TARGET := $(LIBDIR)/libcore.a

.PHONY: all clean

all: $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) | $(LIBDIR)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)
