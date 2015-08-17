
CC = gcc
CCFLAGS = -O2 -std=gnu11 -Wall -pedantic -Wextra -fno-builtin

SRC= \
ntpc.c \
sntp.c \


OBJ = $(SRC:.c=.o)

OUTPUT=ntpc

$(OUTPUT): $(SRC)
	$(CC) $(CCFLAGS) $(SRC) -o $(OUTPUT)

clean:
	@rm -f $(OUTPUT)
	@rm -f $(OBJ)
