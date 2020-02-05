VERSION := 0.3

DEBUG := -g #-fprofile-arcs -ftest-coverage # -pg -g
DEBUG_CPPFLAGS := #-D_DEBUG
CPPFLAGS := -DVERSION=\"${VERSION}\" $(DEBUG_CPPFLAGS) $(CPPFLAGS)
CFLAGS := -O2 $(DEBUG) $(CFLAGS)
LDLIBS := -lm

OBJS := error.o kernel.o main.o math.o

all: timer_entropyd

prefix ?= /usr/local
sbindir ?= $(prefix)/sbin
DESTDIR ?=

timer_entropyd: $(OBJS)
	$(CC) $(CFLAGS) -Wall -W $^ $(LDFLAGS) -o $@ $(LDLIBS)

install: timer_entropyd
	mkdir -p $(DESTDIR)$(sbindir)
	install -m0755 timer_entropyd $(DESTDIR)$(sbindir)

clean:
	rm -f $(OBJS) timer_entropyd core *.da *.gcov *.bb*

package: clean
	mkdir timer_entropyd-$(VERSION)
	cp *.c* *.h Makefile Changes readme.txt license.txt timer_entropyd-$(VERSION)
	tar czf timer_entropyd-$(VERSION).tgz timer_entropyd-$(VERSION)
	rm -rf timer_entropyd-$(VERSION)
