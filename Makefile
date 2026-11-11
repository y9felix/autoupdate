PROG = autoupdate
VERSION = 1.0
PREFIX = /usr/local

CC = cc
INCS =
LIBS =

CPPFLAGS = -DVERSION=\"$(VERSION)\" $(INCS)
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Werror -O2
LDFLAGS = -s $(LIBS)

SRC = $(PROG).c
OBJ = $(SRC:.c=.o)

all: $(PROG)

$(OBJ): config.h

config.h:
	cp config.def.h $@
	chmod 644 $@

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<
	chmod 644 $@

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)
	chmod 755 $@

clean:
	rm -f $(PROG) $(OBJ)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(PROG) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(PROG)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PROG)

.PHONY: all clean install uninstall
