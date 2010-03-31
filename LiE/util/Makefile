CINCLUDES=-I.. -I../box

.SUFFIXES:

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(all-C-flags) $<

all: 	../infoind ../learnind

../learnind: learnind.o
	$(CC) -o ../learnind learnind.o

../infoind: infoind.o
	$(CC) -o ../infoind infoind.o

