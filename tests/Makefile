CC = gcc
LD = gcc

# -O0 désactive les optimisations à la compilation
# C'est utile pour débugger, par contre en "production"
# on active au moins les optimisations de niveau 3 (-O3).
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O3 -g 
LDFLAGS = -lm -flto

# Par défaut, on compile tous les fichiers source (.c) qui se trouvent dans le
# répertoire src_tests/
SRC_FILES=$(wildcard src_tests/*.c)

# Par défaut, la compilation de src_tests/toto.c génère le fichier objet obj_tests/toto.o
OBJ_FILES=$(patsubst src_tests/%.c,obj_tests/%.o,$(SRC_FILES))

all: tests 

tests: $(OBJ_FILES) 
	$(LD) $(OBJ_FILES) $(LDFLAGS) -o $@

test_zigzag: $(OBJ_FILES) 
	$(LD) $(OBJ_FILES) $(LDFLAGS) -o $@

test_IQ: $(OBJ_FILES) 
	$(LD) $(OBJ_FILES) $(LDFLAGS) -o $@

obj_tests/%.o: src_tests/%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf tests  $(OBJ_FILES)

