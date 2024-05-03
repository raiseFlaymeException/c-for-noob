CC = gcc
CXX = g++
DEBUG = gdb

STANDARD = -std=c11

OPTI = -O3

WARNINGS = -Wall -Wextra -Wpedantic -Werror

HEADERDIR = src/include
LIBDIR = src/lib

LIBC_CPP = -static-libstdc++ -static-libgcc
HIDEWINDOW = -mwindows

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

HEADERS = $(call rwildcard,src/include,*.h)
EXTERNAL_HEADERS = $(call rwildcard,src/include/external,*.h)
INTERNAL_HEADERS = $(filter-out $(EXTERNAL_HEADERS),$(HEADERS))
SOURCES = $(call rwildcard,src/c,*.c)
COMPILED = $(subst .c,.o,$(SOURCES)) 

QUIET = > nul 2>&1

LIB =

all: bin/main.exe

bin/main.exe: main.o $(COMPILED)
	$(CC) main.o $(COMPILED) -o bin/main.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: main_debug
main_debug: main.c $(SOURCES)
	$(CC) -ggdb3 main.c $(SOURCES) -o bin/main_debug.exe $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD)

.PHONY: main_preprocess
main_preprocess: main.c $(COMPILED) $(SOURCES)
	$(CC) -E $(SOURCES) $(COMPILED) $(LIB) -I $(HEADERDIR) -L $(LIBDIR) $(WARNING) $(STANDARD) $(OPTI) > bin/main.ipp

.PHONY: debug
debug: main_debug
	$(DEBUG) bin/main_debug.exe

.PHONY: release_bin
release_bin: main.c $(SOURCES)
	$(CC) main.c $(SOURCES) -o bin/main_release.exe $(LIB) $(HIDEWINDOW) $(LIBC_CPP) -I $(HEADERDIR) -L $(LIBDIR) -D RELEASE $(WARNING) $(STANDARD) $(OPTI)

.PHONY: run
run: bin/main.exe
	./bin/main.exe

.PHONY: rebuild
rebuild: clean bin/main.exe

.PHONY: rerun
rerun: clean run

.PHONY: clang-tidy
clang-tidy: $(INTERNAL_HEADERS) $(SOURCES)
	clang-tidy $(INTERNAL_HEADERS) $(SOURCES)

code-action: $(INTERNAL_HEADERS) $(SOURCES)
	clang-tidy $(INTERNAL_HEADERS) $(SOURCES) -fix -fix-errors

format: $(INTERNAL_HEADERS) $(SOURCES)
	clang-format $(INTERNAL_HEADERS) $(SOURCES) -i

main.o: main.c
	$(CC) -c main.c -o main.o -I $(HEADERDIR) $(WARNING) $(STANDARD) $(OPTI)

src/c/%.o: src/c/%.c
	$(CC) -c $< -o $@ -I $(HEADERDIR) $(WARNINGS) $(STANDARD) $(OPTI)

.PHONY: clean
.SILENT: clean
clean:
	-del main.o $(QUIET)
	-del /S src\c\*.o $(QUIET)

.PHONY: cleanmore
.SILENT: cleanmore
cleanmore: clean
	-del bin\*.exe $(QUIET)

	-del bin\main.ipp $(QUIET)
