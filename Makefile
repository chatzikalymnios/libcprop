PREFIX = /usr/local
CC = gcc
AR = ar
CFLAGS = -Wall -Werror

# Compiler flags for automatic dependency generation
DEPFLAGS = -MT $@ -MMD

# Source file extension
SRCEXT = c

LIBNAME = cprop
SONAME = lib$(LIBNAME).so
ANAME = lib$(LIBNAME).a

# Lib directories
SRCDIR = src
INCDIR = include
LIBDIR = lib
BUILDDIR = build
SHARED_BUILDDIR = $(BUILDDIR)/shared
STATIC_BUILDDIR = $(BUILDDIR)/static

# Lib sources/objects/dependencies
SOURCES = $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
SHARED_OBJECTS = $(patsubst $(SRCDIR)/%,$(SHARED_BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
STATIC_OBJECTS = $(patsubst $(SRCDIR)/%,$(STATIC_BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
SHARED_DEPS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.d))
STATIC_DEPS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.d))

# Libraries to link with
LIBS = 

# Example directories
EXAMPLE_DIR = example
EXAMPLE_SRCDIR = $(EXAMPLE_DIR)/src
EXAMPLE_BUILDDIR = $(EXAMPLE_DIR)/build
EXAMPLE_TARGET = $(EXAMPLE_DIR)/run

# Example sources/objects/dependencies
EXAMPLE_SOURCES = $(shell find $(EXAMPLE_SRCDIR) -type f -name "*.$(SRCEXT)")
EXAMPLE_OBJECTS = $(patsubst $(EXAMPLE_SRCDIR)/%,$(EXAMPLE_BUILDDIR)/%,$(EXAMPLE_SOURCES:.$(SRCEXT)=.o))
EXAMPLE_DEPS = $(patsubst $(EXAMPLE_SRCDIR)/%,$(EXAMPLE_BUILDDIR)/%,$(EXAMPLE_SOURCES:.$(SRCEXT)=.d))

# Libarries to link example program with
EXAMPLE_LIBS = -l$(LIBNAME)

# Test directories
TEST_DIR = tests
TEST_SRCDIR = $(TEST_DIR)/src
TEST_BUILDDIR = $(TEST_DIR)/build
TEST_TARGET = $(TEST_DIR)/run

# Test sources/objects/dependencies
TEST_SOURCES = $(shell find $(TEST_SRCDIR) -type f -name "*.$(SRCEXT)")
TEST_OBJECTS = $(patsubst $(TEST_SRCDIR)/%,$(TEST_BUILDDIR)/%,$(TEST_SOURCES:.$(SRCEXT)=.o))
TEST_DEPS = $(patsubst $(TEST_SRCDIR)/%,$(TEST_BUILDDIR)/%,$(TEST_SOURCES:.$(SRCEXT)=.d))

# Test lib/inc flags
TEST_LIBS = -lcheck -l$(LIBNAME)

# Lib targets

all: $(LIBDIR)/$(SONAME) $(LIBDIR)/$(ANAME)

$(LIBDIR)/$(SONAME): $(SHARED_OBJECTS)
	@mkdir -p $(@D)
	$(CC) -shared $< -o $@ $(LIB)

$(LIBDIR)/$(ANAME): $(STATIC_OBJECTS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $< $(LIB)

$(SHARED_BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(DEPFLAGS) -fpic $(CFLAGS) -I$(INCDIR) -c -o $@ $<

$(STATIC_BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(DEPFLAGS) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

# Example Targets

# Build and run example program using the static library
.PHONY: example-static

example-static: $(LIBDIR)/$(ANAME) $(EXAMPLE_OBJECTS)
	$(CC) -static -L$(LIBDIR) $(EXAMPLE_OBJECTS) -o $(EXAMPLE_TARGET) $(EXAMPLE_LIBS)
	@$(EXAMPLE_TARGET)

# Build and run example program using the shared library
.PHONY: example-shared

example-shared: $(LIBDIR)/$(SONAME) $(EXAMPLE_OBJECTS)
	$(CC) -L$(LIBDIR) $(EXAMPLE_OBJECTS) -o $(EXAMPLE_TARGET) $(EXAMPLE_LIBS)
	@LD_LIBRARY_PATH=$(LIBDIR) $(EXAMPLE_TARGET)

$(EXAMPLE_BUILDDIR)/%.o: $(EXAMPLE_SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(DEPFLAGS) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

# Test Targets

# Build and run unit tests using the static library
.PHONY: test

test: $(LIBDIR)/$(ANAME) $(TEST_OBJECTS)
	$(CC) -lcheck -L$(LIBDIR) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LIBDIR)/$(ANAME)
	@$(TEST_TARGET)

$(TEST_BUILDDIR)/%.o: $(TEST_SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(DEPFLAGS) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

# Misc Targets

.PHONY: clean

clean:
	rm -rf $(BUILDDIR) $(LIBDIR)
	rm -rf $(EXAMPLE_BUILDDIR) $(EXAMPLE_TARGET)
	rm -rf $(TEST_BUILDDIR) $(TEST_TARGET)
	
.PHONY: install

install: $(LIBDIR)/$(SONAME)
	@mkdir -p $(DESTDIR)$(PREFIX)/lib
	@mkdir -p $(DESTDIR)$(PREFIX)/include
	cp $(LIBDIR)/$(SONAME) $(DESTDIR)$(PREFIX)/lib/$(SONAME)
	cp $(INCDIR)/$(LIBNAME).h $(DESTDIR)$(PREFIX)/include/$(LIBNAME).h
	
.PHONY: uninstall

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/$(SONAME)
	rm -f $(DESTDIR)$(PREFIX)/include/$(LIBNAME).h

# Autogenerated Targets

-include $(SHARED_DEPS)
-include $(STATIC_DEPS)
-include $(EXAMPLE_DEPS)
-include $(TEST_DEPS)
