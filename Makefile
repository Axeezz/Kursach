# Компилятор
CC = gcc

# Опции компилятора
CFLAGS = -Wall -Wextra -g

# Имена файлов
SRC = main.c sfs.c sfs_file.c sfs_dir.c
OBJ = $(SRC:.c=.o)
EXEC = sfs

# Правила
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# Правила для компиляции .c файлов в .o
%.o: %.c sfs.h
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка промежуточных файлов
clean:
	rm -f $(OBJ) $(EXEC)

# Удаление всех файлов, включая сгенерированные файлы
fclean: clean

# Правило для повторной компиляции
re: fclean all
