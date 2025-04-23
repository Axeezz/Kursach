#include "sfs.h"
#include <stdio.h>
#include <string.h>

// Глобальные переменные
FILE *disk = NULL;
Superblock superblock;
Inode inode_table[MAX_FILES];
DirectoryEntry directory[MAX_FILES];
int current_directory_inode = 0;
char current_directory[MAX_FILENAME_LENGTH] = "";

// Основные функции файловой системы

void sfs_mkfs(const char *diskname) {
    FILE *test = fopen(diskname, "rb");
    if (test) {
        fclose(test);
        printf("Файл '%s' уже существует. Используйте mount для доступа.\n", diskname);
        return;
    }

    disk = fopen(diskname, "wb+");
    if (!disk) {
        printf("Не удалось создать файл диска.\n");
        return;
    }

    // Initialize superblock
    superblock.total_blocks = MAX_BLOCKS;
    superblock.free_blocks = MAX_BLOCKS;
    superblock.total_inodes = MAX_FILES;
    superblock.free_inodes = MAX_FILES - 1;
    memset(superblock.block_bitmap, 0x00, sizeof(superblock.block_bitmap));

    // Initialize inodes and directory
    for (int i = 0; i < MAX_FILES; i++) {
        inode_table[i].is_used = 0;
        inode_table[i].is_directory = 0;
        inode_table[i].directory_inode_index = -1;
        directory[i].inode_index = -1;
        memset(directory[i].filename, 0, MAX_FILENAME_LENGTH);
    }

    // Create root directory
    inode_table[0].is_used = 1;
    inode_table[0].is_directory = 1;
    inode_table[0].directory_inode_index = -1;
    strncpy(inode_table[0].filename, "/", MAX_FILENAME_LENGTH);
    directory[0].inode_index = 0;
    strncpy(directory[0].filename, "/", MAX_FILENAME_LENGTH);

    // Write to disk
    fseek(disk, 0, SEEK_SET);
    fwrite(&superblock, sizeof(Superblock), 1, disk);
    fwrite(inode_table, sizeof(Inode), MAX_FILES, disk);
    fwrite(directory, sizeof(DirectoryEntry), MAX_FILES, disk);
    fflush(disk);

    printf("Файловая система отформатирована. Корневая директория создана.\n");
}

void sfs_mount(const char *diskname) {
    disk = fopen(diskname, "r+b");
    if (!disk) {
        printf("Файл диска не существует. Создать новую файловую систему? (y/n): ");
        int answer = getchar();
        while (getchar() != '\n'); // Clear input buffer

        if (answer == 'y' || answer == 'Y') {
            sfs_mkfs(diskname);
            disk = fopen(diskname, "r+b");
            if (!disk) {
                printf("Ошибка при создании файловой системы.\n");
                return;
            }
        } else {
            return;
        }
    }

    // Check filesystem validity
    if (!is_valid_filesystem(disk)) {
        printf("Файл не содержит валидной ФС. Инициализировать? (y/n): ");
        int answer = getchar();
        while (getchar() != '\n');

        if (answer == 'y' || answer == 'Y') {
            fclose(disk);
            sfs_mkfs(diskname);
            disk = fopen(diskname, "r+b");
            if (!disk) {
                printf("Ошибка при создании файловой системы.\n");
                return;
            }
        } else {
            fclose(disk);
            disk = NULL;
            return;
        }
    }

    // Read structures
    fseek(disk, 0, SEEK_SET);
    if (fread(&superblock, sizeof(Superblock), 1, disk) != 1) {
        printf("Ошибка чтения суперблока.\n");
        fclose(disk);
        disk = NULL;
        return;
    }

    fseek(disk, sizeof(Superblock), SEEK_SET);
    if (fread(inode_table, sizeof(Inode), MAX_FILES, disk) != MAX_FILES) {
        printf("Ошибка чтения таблицы inode.\n");
        fclose(disk);
        disk = NULL;
        return;
    }

    fseek(disk, sizeof(Superblock) + sizeof(Inode) * MAX_FILES, SEEK_SET);
    if (fread(directory, sizeof(DirectoryEntry), MAX_FILES, disk) != MAX_FILES) {
        printf("Ошибка чтения директории.\n");
        fclose(disk);
        disk = NULL;
        return;
    }

    // Set current directory
    current_directory_inode = 0;
    strcpy(current_directory, "/");

    // Try to find /home
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index != -1 &&
            strcmp(directory[i].filename, "home") == 0 &&
            inode_table[directory[i].inode_index].is_directory) {
            current_directory_inode = directory[i].inode_index;
            strcpy(current_directory, "/home");
            break;
        }
    }

    printf("Файловая система смонтирована. Текущая директория: %s\n", current_directory);
}

void sfs_umount() {
    if (disk) {
        // Save all changes
        fseek(disk, 0, SEEK_SET);
        fwrite(&superblock, sizeof(Superblock), 1, disk);

        fseek(disk, sizeof(Superblock), SEEK_SET);
        fwrite(inode_table, sizeof(Inode), MAX_FILES, disk);

        fseek(disk, sizeof(Superblock) + sizeof(Inode) * MAX_FILES, SEEK_SET);
        fwrite(directory, sizeof(DirectoryEntry), MAX_FILES, disk);

        fflush(disk);
        fclose(disk);
        disk = NULL;

        printf("Файловая система размонтирована. Все данные сохранены.\n");
    }
}

// Вспомогательные функции
int find_free_inode() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (inode_table[i].is_used == 0) {
            return i;
        }
    }
    return -1;
}

int find_free_block() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if ((superblock.block_bitmap[i / 8] & (1 << (i % 8))) == 0) {
            return i;
        }
    }
    return -1;
}

void allocate_block(int block_index) {
    superblock.block_bitmap[block_index / 8] &= ~(1 << (block_index % 8));
    superblock.free_blocks--;
}

void free_block(int block_index) {
    superblock.block_bitmap[block_index / 8] |= (1 << (block_index % 8));
    superblock.free_blocks++;
}

void print_current_directory() {
    if (current_directory_inode == 0) {
        printf("/\n");
        return;
    }
    char path[MAX_FILENAME_LENGTH];
    build_path_from_inode(current_directory_inode, path, MAX_FILENAME_LENGTH);
    printf("%s\n", path);
}

void build_path_from_inode(int inode, char *path, size_t path_size) {
    if (inode == 0) {
        strncpy(path, "/", path_size);
        return;
    }
    char parent_path[MAX_FILENAME_LENGTH] = "";
    int parent_inode = inode_table[inode].directory_inode_index;
    if (parent_inode != 0) {
        build_path_from_inode(parent_inode, parent_path, sizeof(parent_path));
    }
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index == inode) {
            if (strcmp(parent_path, "/") == 0) {
                snprintf(path, path_size, "/%s", directory[i].filename);
            } else {
                snprintf(path, path_size, "%s/%s", parent_path, directory[i].filename);
            }
            break;
        }
    }
}

void sfs_pwd() {
    printf("Текущая директория: %s\n", current_directory);
}

void sfs_pwdm() {
    printf("%s: ", current_directory);
}

int sfs_check_integrity() {
    if (!disk) return 0;

    // Проверяем корневую директорию
    if (!inode_table[0].is_used || !inode_table[0].is_directory) {
        printf("Ошибка целостности: повреждена корневая директория.\n");
        return 0;
    }

    // Проверяем согласованность inode и directory
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index != -1) {
            int inode_idx = directory[i].inode_index;
            if (inode_idx < 0 || inode_idx >= MAX_FILES || !inode_table[inode_idx].is_used) {
                printf("Ошибка целостности: несоответствие directory и inode таблицы.\n");
                return 0;
            }
        }
    }

    return 1; // Все проверки пройдены
}

int is_valid_filesystem(FILE *f) {
    if (!f) return 0;

    // 1. Проверяем размер файла
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size < sizeof(Superblock) + sizeof(Inode)*MAX_FILES) {
        return 0;
    }

    // 2. Проверяем сигнатуру суперблока
    Superblock sb;
    if (fread(&sb, sizeof(Superblock), 1, f) != 1) {
        return 0;
    }

    // Простые проверки (можно добавить magic number)
    if (sb.total_blocks != MAX_BLOCKS || sb.total_inodes != MAX_FILES) {
        return 0;
    }

    // 3. Проверяем корневую директорию
    Inode root_inode;
    fseek(f, sizeof(Superblock), SEEK_SET);
    if (fread(&root_inode, sizeof(Inode), 1, f) != 1) {
        return 0;
    }

    if (!root_inode.is_used || !root_inode.is_directory) {
        return 0;
    }

    return 1;
}

void help() {
    printf("\n\n\nc <filename> - создание файла с именем filename\n");
    printf("d <filename>            - удаление файла с именем filename\n");
    printf("w <filename>            - открытие файла с именем filename для записи\n");
    printf("к <filename>            - чтение файла с именем filename\n");
    printf("mkdir <dirname>         - создание директории с именем dirname\n");
    printf("rmdir <dirname>         - удаление директории с именем dirname\n");
    printf("rm <dirname>            - рекурсивное удаление директории с именем dirname\n");
    printf("cd <dirname>            - переход в директорию dirname (переход в предыдущую - ..)\n");
    printf("ls [dirname]            - просмотр текущей директории(* - опционально) или директории с именем dirname\n");
    printf("mv <filename> <dirname> - перемещение файла filename в директорию dirname\n");
    printf("pwd                     - получение пути к текущей директории\n");
    printf("е                       - выход из файловой системы\n\n");
    printf("Для <filename> и <dirname> возможно указание как полного, так и относительного пути в формате:\n dirname\n ./dirname\n ../dirname\n ./dirname1/dirname2\n /home/.../dirname\n\n\n");
}