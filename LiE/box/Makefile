CINCLUDES= -I. -I..

.SUFFIXES:

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(all-C-flags) $<

objects=\
 altdom.o branch.o centr.o closure.o contragr.o coxeter.o decomp.o defs.o\
 diagram.o domchar.o factor.o grpdata.o lr.o matrix.o orbit.o\
 plethysm.o sorting.o static3.o symg.o tensor.o weyl.o weylloop.o

.PHONY: all clean

all:	.last_compiled

.last_compiled:	$(objects)
	touch .last_compiled

defs.o:		defs.h
# standard targets

clean:
	rm -f *~ *.o .last_compiled
