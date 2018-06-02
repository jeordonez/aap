CC = mpicc
CFLAGS = -g -Wall

PROGRAMS = master reduce map

all: $(PROGRAMS)

% : %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean

clean:
	rm *~ $(PROGRAMS)

