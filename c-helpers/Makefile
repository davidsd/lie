py-dir = /Library/Frameworks/Python.framework/Versions/3.1
includes = -I.. -I../LiE -I../LiE/box -I. -I$(py-dir)/include/python3.1
sl-flags= -m32 -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5
all-C-flags:= -ansi -D_ANSI_SOURCE $(includes) $(CFLAGS) $(sl-flags) -fPIC
$(CC) = gcc

.SUFFIXES:

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(all-C-flags) $<

objects=from-static1.o from-static2.o from-static3.o error.o gc.o

.PHONY: all clean

all:	.last_compiled

.last_compiled:	$(objects)
	touch .last_compiled

clean:
	rm -f *~ *.o .last_compiled
