.PHONY: all clean
CC = gcc

CFLAGS = -g -Wall -Wextra -Wpedantic

NAME1 = comtrade-info
SRC1 = cmi.c cfg.c cmi_msg.c strutils.c
OBJ1 = $(SRC1:.c=.o)

CLN = *.mk *.o $(NAME1)

all: $(NAME1)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME1): $(OBJ1)
	$(CC) $(CFLAGS) -o $@ $^

deps.mk: $(SRC1)
	$(CC) -MM $^ > $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

clean:
	rm -f $(CLN)
