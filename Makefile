# Compiler
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -g

# Target executable name
TARGET  := my_bc

# Source and object files
SRC     := my_bc.c stack.c queue.c utils.c
OBJ     := $(SRC:.c=.o)

# Headers
HEADERS := stack.h queue.h utils.h

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

fclean: clean
	rm -f $(TARGET)

re: fclean all