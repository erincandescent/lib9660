.POSIX:
.SUFFIXES:
.SUFFIXES: .o .c
.PHONY: all clean install

PREFIX = /usr/local
CFLAGS = -DL9660_HAVE_STDIO -g -DDEBUG

OBJS := lib9660.o
ALL  := lib9660.a tb9660

all: $(ALL)

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS)

lib9660.a: $(OBJS)
	ar cru lib9660.a $(OBJS)

tb9660: tb9660.c lib9660.a
	$(CC) -g -o tb9660 tb9660.c lib9660.a $(CFLAGS)

clean:
	rm $(ALL) $(OBJS)

install:
	install -m644 lib9660.a $(DESTDIR)$(PREFIX)/lib
	install -m644 lib9660.h $(DESTDIR)$(PREFIX)/include
