# Project name & sources

PROJECT := aim

PROJECT_SOURCES := $(wildcard ./src/*.c)
PROJECT_OBJECTS := $(PROJECT_SOURCES:.c=.o)

# Compiler & linker

CC := gcc

CFLAGS := -fno-common                  \
          -fno-omit-frame-pointer      \
          -fsanitize=address,undefined \
          -fwrapv                      \
          -ggdb                        \
          -std=c23                     \
          -O2                          \
          -Wall                        \
          -Wconversion                 \
          -Wdouble-promotion           \
          -Werror                      \
          -Wextra                      \
          -Wpedantic                   \
          -Wshadow                     \
          -Wsign-conversion            \
          -Wundef

LDFLAGS := -lm -lSDL3 -lSDL3_ttf

# Targets

all: $(PROJECT)

$(PROJECT): $(PROJECT_OBJECTS)
	$(CC) $(CFLAGS) -o $(PROJECT) $(PROJECT_OBJECTS) $(LDFLAGS)

run:
	LSAN_OPTIONS=suppressions=suppressions ./$(PROJECT)

clean:
	rm -rf $(PROJECT)
	rm -rf $(PROJECT_OBJECTS)

.PHONY: all run clean
