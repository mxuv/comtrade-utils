.PHONY: all clean

CC = gcc

CFLAGS = -g -Wall -Wextra -Wpedantic -I $(LINC)

NAME1 = comtrade-info
SRC1 = cmi.c cfg.c cmi_msg.c
OBJ1 = $(SRC1:.c=.o)

LINC = lib/

LPATH = lib/strutils/
LNAME = strutils
LDEPS = lib/strutils/libstrutils.a

CLN = *.mk *.o $(NAME1)

all: $(NAME1)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LDEPS):
	cd $(LPATH) && $(MAKE)

$(NAME1): $(OBJ1) $(LDEPS)
	$(CC) $(CFLAGS) -o $@ $(OBJ1) -L $(LPATH) -l $(LNAME)

deps.mk: $(SRC1)
	$(CC) -MM $^ > $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

clean:
	rm -f $(CLN)
	cd $(LPATH) && $(MAKE) clean
