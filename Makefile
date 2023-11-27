CC = gcc
CFLAGS = -Wall -Wextra -O0 -Wno-unused-function		## -O0, so no optimizations from gcc

SRC = prog.c phods.c phods_opt.c
OBJ = $(SRC:.c=.o)
EXECUTABLE = phodsO0 

all: $(EXECUTABLE) 

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(EXECUTABLE)
