c_sources=static1.c static2.c static4.c static5.c static6.c static7.c

.SUFFIXES:

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(all-C-flags) $<

.last_compiled:	$(c_sources:.c=.o)
	touch .last_compiled

all:	.last_compiled

# TAGS file with identifier definitions for emacs perusal.

TAGS:	$(c_sources)
	etags $(c_sources)

# standard targets

clean:
	rm -f *.o .last_compiled

