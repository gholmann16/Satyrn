SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c, src/%.o, $(SOURCES))

VERSION := 0.0.1

LDLIBS += -lappimage

debug: CFLAGS += -g -Og -Wall
debug: sat

release: CFLAGS += -O3
release: sat
	strip $<

src/version.h:
	echo "#define VERSION_NUMBER \"$(VERSION)\"" > src/version.h

sat: src/version.h $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDLIBS) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

clean:
	rm -f $(OBJECTS)
	rm -f sat
	rm -f src/version.h

install: release
	install -Dm755 sat $(DESTDIR)/usr/bin/sat

uninstall:
	rm /usr/bin/sat
