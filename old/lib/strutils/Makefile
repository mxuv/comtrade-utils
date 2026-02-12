.PHONY: all clean

AR = ar
CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic
ARFLAGS = crf

SRC = char2int.c ishexdigit.c stringcopy.c stringcopy_c.c stringcopy_ec.c \
	  stringlen.c stringmatch.c upcase_letter.c upcase_string.c match_char.c \
	  getstring.c

HDR = strutils.h
OBJ = $(SRC:.c=.o)

LIBNAME = libstrutils.a

%.o: %.c $(HDR) 
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBNAME): $(OBJ)
	$(AR) $(ARFLAGS) -o $@ $^

all: $(LIBNAME)

clean:
	rm -f $(LIBNAME) $(OBJ)

