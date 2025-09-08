SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c, src/%.o, $(SOURCES))

VERSION := 0.0.1

LDLIBS += -lappimage -lm

debug: CFLAGS += -g -Og -Wall
debug: nep

release: CFLAGS += -O3
release: nep
	strip $<

src/version.h:
	echo "#define VERSION_NUMBER \"$(VERSION)\"" > src/version.h

nep: src/version.h $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDLIBS) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

clean:
	rm -f $(OBJECTS)
	rm -f nep
	rm -f src/version.h

install: release
	install -Dm755 nep $(DESTDIR)/usr/bin/nep

uninstall:
	rm /usr/bin/nep
