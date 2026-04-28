CC      ?= gcc
CFLAGS  ?= -std=c11 -O0 -Wall -Wextra -pedantic
LDFLAGS ?= -lm
TARGET  := cuav_mtic_sim
SRC     := main.c mtic.c utils.c
OBJ     := $(SRC:.c=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)
