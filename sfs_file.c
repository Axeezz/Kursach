#include "sfs.h"

// Функции для работы с файлами
void sfs_create(const char *filename) {
    // Проверка длины имени
    if (strlen(filename) >= MAX_FILENAME_LENGTH) {
        printf("Слишком длинное имя файла.\n");
        return;
    }

    // Проверка существования файла
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index != -1 &&
            inode_table[directory[i].inode_index].directory_inode_index == current_directory_inode &&
            strcmp(directory[i].filename, filename) == 0) {
            printf("Файл '%s' уже существует.\n", filename);
            return;
        }
    }

    // Создание файла
    int inode_index = find_free_inode();
    if (inode_index == -1) {
        printf("Нет свободных inode.\n");
        return;
    }

    int block_index = find_free_block();
    if (block_index == -1) {
        printf("Нет свободных блоков.\n");
        return;
    }

    int dir_entry_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index == -1) {
            dir_entry_index = i;
            break;
        }
    }

    if (dir_entry_index == -1) {
        printf("Нет места в директории.\n");
        return;
    }

    // Заполнение структур
    inode_table[inode_index].is_used = 1;
    inode_table[inode_index].is_directory = 0;
    inode_table[inode_index].directory_inode_index = current_directory_inode;
    strncpy(inode_table[inode_index].filename, filename, MAX_FILENAME_LENGTH);
    inode_table[inode_index].size = 0;
    inode_table[inode_index].block_count = 1;
    inode_table[inode_index].blocks[0] = block_index;

    directory[dir_entry_index].inode_index = inode_index;
    strncpy(directory[dir_entry_index].filename, filename, MAX_FILENAME_LENGTH);

    superblock.free_inodes--;
    allocate_block(block_index);

    // Сохранение на диск
    fseek(disk, 0, SEEK_SET);
    fwrite(&superblock, sizeof(Superblock), 1, disk);
    fseek(disk, sizeof(Superblock), SEEK_SET);
    fwrite(inode_table, sizeof(Inode), MAX_FILES, disk);
    fseek(disk, sizeof(Superblock) + sizeof(Inode) * MAX_FILES, SEEK_SET);
    fwrite(directory, sizeof(DirectoryEntry), MAX_FILES, disk);

    print_current_directory();

    printf("Файл '%s' создан.\n", filename);
}

void sfs_write(const char *filename, const char *data) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strncmp(directory[i].filename, filename, MAX_FILENAME_LENGTH) == 0) {
            Inode *inode = &inode_table[directory[i].inode_index];

            int data_written = 0;
            int size = strlen(data);

            for (int j = 0; j < inode->block_count; j++) {
                int block_size = (size - data_written > BLOCK_SIZE) ? BLOCK_SIZE : size - data_written;

                fseek(disk, get_block_offset(inode->blocks[j]), SEEK_SET);
                fwrite(data + data_written, 1, block_size, disk);

                data_written += block_size;
                if (data_written == size) break;
            }
            fflush(disk);

            inode->size = size;

            fseek(disk, sizeof(Superblock), SEEK_SET);
            fwrite(inode_table, sizeof(Inode), MAX_FILES, disk);

            fflush(disk);

            printf("Данные в файл '%s' успешно записаны.\n", filename);
            return;
        }
    }
    printf("Файл '%s' не найден.\n", filename);
}

void sfs_read(const char *filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strncmp(directory[i].filename, filename, MAX_FILENAME_LENGTH) == 0) {
            Inode *inode = &inode_table[directory[i].inode_index];

            int data_read = 0;
            int size = inode->size;
            char buffer[size + 1];

            for (int j = 0; j < inode->block_count; j++) {
                int block_size = (size - data_read > BLOCK_SIZE) ? BLOCK_SIZE : size - data_read;

                fseek(disk, get_block_offset(inode->blocks[j]), SEEK_SET);
                fread(buffer + data_read, 1, block_size, disk);

                data_read += block_size;
                if (data_read == size) break;
            }

            buffer[size] = '\0';

            printf("Данные из файла '%s':\n%s\n", filename, buffer);
            return;
        }
    }
    printf("Файл '%s' не найден.\n", filename);
}

void sfs_delete(const char *filename) {
    if (disk == NULL) {
        fprintf(stderr, "Ошибка: файловая система не смонтирована.\n");
        return;
    }

    if (filename == NULL || strlen(filename) == 0) {
        printf("Ошибка: не указано имя файла.\n");
        return;
    }

    // Ищем файл в текущей директории
    int dir_entry_index = -1;
    int file_inode_index = -1;

    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index != -1 &&
            inode_table[directory[i].inode_index].directory_inode_index == current_directory_inode &&
            strcmp(directory[i].filename, filename) == 0) {

            // Проверяем, что это не директория
            if (inode_table[directory[i].inode_index].is_directory) {
                printf("Ошибка: '%s' является директорией. Используйте ddir для удаления директорий.\n", filename);
                return;
            }

            dir_entry_index = i;
            file_inode_index = directory[i].inode_index;
            break;
        }
    }

    if (dir_entry_index == -1) {
        printf("Файл '%s' не найден в текущей директории.\n", filename);
        return;
    }

    Inode *file_inode = &inode_table[file_inode_index];

    // Освобождаем все блоки файла
    for (int i = 0; i < file_inode->block_count; i++) {
        int block_index = file_inode->blocks[i];

        if (block_index < 0 || block_index >= MAX_BLOCKS) {
            printf("Предупреждение: некорректный индекс блока %d, пропускаем.\n", block_index);
            continue;
        }

        // Очищаем блок на диске
        char zero_block[BLOCK_SIZE] = {0};
        fseek(disk, block_index * BLOCK_SIZE, SEEK_SET);
        fwrite(zero_block, BLOCK_SIZE, 1, disk);

        // Освобождаем блок в битовой карте
        free_block(block_index);
    }

    // Освобождаем inode
    memset(file_inode, 0, sizeof(Inode));
    superblock.free_inodes++;

    // Удаляем запись из директории
    directory[dir_entry_index].inode_index = -1;
    memset(directory[dir_entry_index].filename, 0, MAX_FILENAME_LENGTH);

    // Сохраняем изменения на диск
    fseek(disk, 0, SEEK_SET);
    fwrite(&superblock, sizeof(Superblock), 1, disk);

    fseek(disk, sizeof(Superblock), SEEK_SET);
    fwrite(inode_table, sizeof(Inode), MAX_FILES, disk);

    fseek(disk, sizeof(Superblock) + sizeof(Inode) * MAX_FILES, SEEK_SET);
    fwrite(directory, sizeof(DirectoryEntry), MAX_FILES, disk);

    fflush(disk); // Гарантируем запись на диск

    printf("Файл '%s' успешно удален.\n", filename);
}