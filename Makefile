CC=gcc
CFLAGS=-Wall -pg
SRC=raid_five.c
OBJ=$(SRC:.c=.o)

raid_five: $(OBJ)
	$(CC) $(CFLAGS) -D__LINUX__ -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -D__LINUX__ -c -o $@ $<

clean:
	rm -f raid_five $(OBJ)
	rm drive_*.dat
