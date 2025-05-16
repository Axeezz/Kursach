#include "sfs.h"

void sfs_cd(const char *dirname) {
    if (dirname == NULL || strlen(dirname) == 0) {
        printf("Ошибка: путь не указан.\n");
        return;
    }

    char basename[MAX_FILENAME_LENGTH];
    int parent_inode;
    int target_inode = resolve_path_to_inode(dirname, &parent_inode, basename);

    // Проверяем, что директория существует
    if (target_inode == -1) {
        printf("Директория '%s' не найдена.\n", dirname);
        return;
    }

    // Проверяем, что это действительно директория
    if (!inode_table[target_inode].is_directory) {
        printf("'%s' не является директорией.\n", dirname);
        return;
    }

    // Обновляем текущую директорию
    current_directory_inode = target_inode;

    // Обновляем строковое представление пути
    if (target_inode == 0) {
        strcpy(current_directory, "/");
    } else {
        // Строим полный путь по inode
        build_path_from_inode(target_inode, current_directory, MAX_FILENAME_LENGTH);
    }

    printf("Текущая директория: %s\n", current_directory);
}

void sfs_create_dir(const char *dirname) {
    // Проверка длины имени
    if (strlen(dirname) >= MAX_FILENAME_LENGTH) {
        printf("Слишком длинное имя директории.\n");
        return;
    }

    // Находим родительскую директорию (текущую или корневую)
    int parent_inode = current_directory_inode;
    if (strcmp(dirname, "home") == 0) {
        parent_inode = 0; // Корень для /home
    }

    // Проверяем, нет ли уже такой директории
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index >= 0 &&
            inode_table[directory[i].inode_index].directory_inode_index == parent_inode &&
            strcmp(directory[i].filename, dirname) == 0) {
            printf("Директория '%s' уже существует.\n", dirname);
            return;
        }
    }

    // Ищем свободный inode
    int inode_index = find_free_inode();
    if (inode_index == -1) {
        printf("Нет свободных inode для новой директории.\n");
        return;
    }

    // Ищем свободный блок
    int block_index = find_free_block();
    if (block_index == -1) {
        printf("Нет свободных блоков для хранения данных.\n");
        return;
    }

    // Создаем inode для директории
    Inode *inode = &inode_table[inode_index];
    inode->is_used = 1;
    inode->is_directory = 1;
    inode->directory_inode_index = parent_inode; // Устанавливаем родителя
    inode->size = 0;
    inode->block_count = 1;
    inode->blocks[0] = block_index;

    // Находим свободную запись в directory
    int dir_entry_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index == -1) {
            dir_entry_index = i;
            break;
        }
    }

    if (dir_entry_index == -1) {
        printf("Нет свободных записей в directory.\n");
        return;
    }

    // Заполняем запись в directory
    DirectoryEntry *entry = &directory[dir_entry_index];
    strncpy(entry->filename, dirname, MAX_FILENAME_LENGTH);
    entry->inode_index = inode_index;

    superblock.free_inodes--;
    allocate_block(block_index);

    // Обновляем данные на диске
    fseek(disk, 0, SEEK_SET);
    fwrite(&superblock, sizeof(Superblock), 1, disk);
    fseek(disk, sizeof(Superblock), SEEK_SET);
    fwrite(inode_table, sizeof(Inode), MAX_FILES, disk);
    fseek(disk, sizeof(Superblock) + sizeof(Inode) * MAX_FILES, SEEK_SET);
    fwrite(directory, sizeof(DirectoryEntry), MAX_FILES, disk);
    fflush(disk);

    printf("Директория '%s' успешно создана.\n", dirname);
}

void sfs_ls_dir(const char *dirname) {
    // Определяем директорию для вывода содержимого
    int target_inode;
    char basename[MAX_FILENAME_LENGTH];
    int parent_inode;

    if (dirname == NULL || strlen(dirname) == 0) {
        // Если путь не указан - используем текущую директорию
        target_inode = current_directory_inode;
    } else {
        // Разрешаем переданный путь
        target_inode = resolve_path_to_inode(dirname, &parent_inode, basename);
    }

    // Проверяем, что объект существует
    if (target_inode == -1) {
        printf("'%s' не найдено.\n", dirname ? dirname : "текущий путь");
        return;
    }

    // Проверяем, что это директория (если не корень)
    if (target_inode != 0 && !inode_table[target_inode].is_directory) {
        printf("'%s' не является директорией.\n", dirname ? dirname : "текущий путь");
        return;
    }

    // Получаем полное имя директории для вывода
    char dir_path[MAX_FILENAME_LENGTH];
    if (target_inode == 0) {
        strcpy(dir_path, "/");
    } else {
        build_path_from_inode(target_inode, dir_path, MAX_FILENAME_LENGTH);
    }

    printf("Содержимое директории '%s':\n", dir_path);

    // Выводим содержимое директории
    int empty = 1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index != -1) {
            // Для файлов и поддиректорий этой директории
            if (inode_table[directory[i].inode_index].directory_inode_index == target_inode) {
                printf("- %s", directory[i].filename);
                if (inode_table[directory[i].inode_index].is_directory) {
                    printf(" (директория)");
                } else {
                    printf(" (файл, размер: %d)", inode_table[directory[i].inode_index].size);
                }
                printf("\n");
                empty = 0;
            }
        }
    }

    if (empty) {
        printf("Директория пуста.\n");
    }
}

void sfs_delete_dir(const char *dirname) {
    if (disk == NULL) {
        fprintf(stderr, "Ошибка: файловая система не смонтирована.\n");
        return;
    }

    if (dirname == NULL || strlen(dirname) == 0) {
        printf("Ошибка: не указано имя директории.\n");
        return;
    }

    // Разрешаем путь к директории
    int parent_inode;
    char basename[MAX_FILENAME_LENGTH];
    int dir_inode = resolve_path_to_inode(dirname, &parent_inode, basename);

    // Проверяем, что директория существует
    if (dir_inode == -1) {
        printf("Директория '%s' не найдена.\n", dirname);
        return;
    }

    // Нельзя удалить корневую директорию
    if (dir_inode == 0) {
        printf("Ошибка: нельзя удалить корневую директорию.\n");
        return;
    }

    // Проверяем, что это действительно директория
    if (!inode_table[dir_inode].is_directory) {
        printf("Ошибка: '%s' не является директорией.\n", dirname);
        return;
    }

    // Проверяем, не пытаемся ли удалить текущую директорию
    if (dir_inode == current_directory_inode) {
        printf("Ошибка: нельзя удалить текущую директорию.\n");
        return;
    }

    // Проверяем, пуста ли директория
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index != -1 &&
            inode_table[directory[i].inode_index].directory_inode_index == dir_inode) {
            printf("Директория '%s' не пуста, невозможно удалить.\n", dirname);
            return;
        }
    }

    // Находим запись в directory для удаляемой директории
    int dir_entry_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index == dir_inode) {
            dir_entry_index = i;
            break;
        }
    }

    if (dir_entry_index == -1) {
        printf("Ошибка: не найдена запись в директории.\n");
        return;
    }

    // Получаем полный путь для сообщения
    char dir_path[MAX_FILENAME_LENGTH];
    build_path_from_inode(dir_inode, dir_path, sizeof(dir_path));

    // Освобождаем inode
    inode_table[dir_inode].is_used = 0;
    superblock.free_inodes++;

    // Удаляем запись из directory
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

    printf("Директория '%s' успешно удалена.\n", dir_path);
}

int resolve_path_to_inode(const char *path, int *parent_inode_index, char *basename) {
    if (!path || !parent_inode_index || !basename) {
        printf("[ERROR] Неверные аргументы resolve_path_to_inode.\n");
        return -1;
    }

    // Создаем копию пути для безопасной работы
    char temp_path[MAX_FILENAME_LENGTH * 2];  // Увеличиваем буфер для сложных путей
    strncpy(temp_path, path, sizeof(temp_path));
    temp_path[sizeof(temp_path) - 1] = '\0';

    //printf("[DEBUG] resolve_path_to_inode: path = '%s'\n", temp_path);

    char *token;
    char *rest = temp_path;
    int current_inode = (path[0] == '/') ? 0 : current_directory_inode;
    *parent_inode_index = -1;
    char last_token[MAX_FILENAME_LENGTH] = "";

    // Обрабатываем случай корневого пути
    if (strcmp(temp_path, "/") == 0) {
        strncpy(basename, "/", MAX_FILENAME_LENGTH);
        *parent_inode_index = -1;
        return 0;  // Корневой inode
    }

    // Разбираем путь по токенам
    while ((token = strtok_r(rest, "/", &rest))) {
       // printf("[DEBUG] token = '%s'\n", token);

        // Пропускаем пустые токены и текущую директорию
        if (strlen(token) == 0 || strcmp(token, ".") == 0) {
            continue;
        }

        // Обработка перехода в родительскую директорию
        if (strcmp(token, "..") == 0) {
            if (current_inode != 0) {  // Не выходим за пределы корня
                *parent_inode_index = inode_table[current_inode].directory_inode_index;
                current_inode = *parent_inode_index;
            }
            continue;
        }

        // Поиск токена в текущей директории
        int found = 0;
        for (int i = 0; i < MAX_FILES; i++) {
            if (directory[i].inode_index >= 0 &&
                inode_table[directory[i].inode_index].directory_inode_index == current_inode &&
                strncmp(directory[i].filename, token, MAX_FILENAME_LENGTH) == 0) {

                *parent_inode_index = current_inode;
                current_inode = directory[i].inode_index;
                found = 1;
                strncpy(last_token, token, MAX_FILENAME_LENGTH);
                break;
            }
        }

        if (!found) {
           // printf("[DEBUG] '%s' не найден в inode %d\n", token, current_inode);
            strncpy(basename, token, MAX_FILENAME_LENGTH);
            return -1;
        }
    }

    // Обработка случая, когда путь заканчивается слешем (например, "dir/")
    if (strlen(last_token) == 0 && path[strlen(path)-1] == '/') {
        strncpy(basename, "", MAX_FILENAME_LENGTH);
        return current_inode;
    }

    // Сохраняем результат
    strncpy(basename, last_token, MAX_FILENAME_LENGTH);
    //printf("[DEBUG] Успешно: basename='%s', inode=%d, parent_inode=%d\n",
     //      basename, current_inode, *parent_inode_index);

    return current_inode;
}

void sfs_move_to_dir(const char *file_input, const char *dir_input) {
    int file_parent_inode, dir_parent_inode;
    char file_name[MAX_FILENAME_LENGTH];
    char dir_name[MAX_FILENAME_LENGTH];

    // Получаем inode файла и родительской директории
    int file_inode_index = resolve_path_to_inode(file_input, &file_parent_inode, file_name);
    if (file_inode_index == -1) {
        printf("Файл '%s' не найден.\n", file_input);
        return;
    }

    // Получаем inode целевой директории
    int dir_inode_index = resolve_path_to_inode(dir_input, &dir_parent_inode, dir_name);
    if (dir_inode_index == -1 || !inode_table[dir_inode_index].is_directory) {
        printf("Директория '%s' не найдена или это не директория.\n", dir_input);
        return;
    }

    // Проверяем, нет ли файла с таким именем в целевой директории
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index >= 0 &&
            inode_table[directory[i].inode_index].directory_inode_index == dir_inode_index &&
            strcmp(directory[i].filename, file_name) == 0) {
            printf("Файл с именем '%s' уже существует в директории '%s'.\n", file_name, dir_input);
            return;
        }
    }

    // Удаляем запись из старой директории
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index == file_inode_index &&
            inode_table[directory[i].inode_index].directory_inode_index == file_parent_inode) {
            directory[i].inode_index = -1; // Помечаем как свободную запись
            break;
        }
    }

    // Добавляем запись в новую директорию
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index == -1) { // Находим свободную запись
            directory[i].inode_index = file_inode_index;
            strncpy(directory[i].filename, file_name, MAX_FILENAME_LENGTH);
            inode_table[file_inode_index].directory_inode_index = dir_inode_index;
            break;
        }
    }

    // Сохраняем изменения
    fseek(disk, sizeof(Superblock) + sizeof(Inode) * MAX_FILES, SEEK_SET);
    fwrite(directory, sizeof(DirectoryEntry), MAX_FILES, disk);

    printf("Файл '%s' перемещён в '%s'.\n", file_input, dir_input);
}

// Рекурсивное удаление директории
void sfs_delete_dir_recursive(const char *dirname) {
    if (disk == NULL) {
        fprintf(stderr, "Ошибка: файловая система не смонтирована.\n");
        return;
    }

    if (dirname == NULL || strlen(dirname) == 0) {
        printf("Ошибка: не указано имя директории.\n");
        return;
    }

    // Разрешаем путь к директории
    int parent_inode;
    char basename[MAX_FILENAME_LENGTH];
    int dir_inode = resolve_path_to_inode(dirname, &parent_inode, basename);

    // Проверяем, что директория существует
    if (dir_inode == -1) {
        printf("Директория '%s' не найдена.\n", dirname);
        return;
    }

    // Нельзя удалить корневую директорию
    if (dir_inode == 0) {
        printf("Ошибка: нельзя удалить корневую директорию.\n");
        return;
    }

    // Проверяем, что это действительно директория
    if (!inode_table[dir_inode].is_directory) {
        printf("Ошибка: '%s' не является директорией.\n", dirname);
        return;
    }

    // Проверяем, не пытаемся ли удалить текущую директорию
    if (dir_inode == current_directory_inode) {
        printf("Ошибка: нельзя удалить текущую директорию.\n");
        return;
    }

    // Получаем полный путь для сообщений
    char dir_path[MAX_FILENAME_LENGTH];
    build_path_from_inode(dir_inode, dir_path, sizeof(dir_path));

    // Рекурсивно удаляем все содержимое директории
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index != -1 &&
            inode_table[directory[i].inode_index].directory_inode_index == dir_inode) {

            // Для поддиректорий вызываем рекурсивное удаление
            if (inode_table[directory[i].inode_index].is_directory) {
                char subdir_path[MAX_FILENAME_LENGTH];
                snprintf(subdir_path, MAX_FILENAME_LENGTH, "%s/%s", dir_path, directory[i].filename);
                sfs_delete_dir_recursive(subdir_path);
            }
                // Для файлов вызываем обычное удаление
            else {
                sfs_delete(directory[i].filename);
            }
        }
    }

    // Находим запись в directory для удаляемой директории
    int dir_entry_index = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (directory[i].inode_index == dir_inode) {
            dir_entry_index = i;
            break;
        }
    }

    if (dir_entry_index == -1) {
        printf("Ошибка: не найдена запись в директории.\n");
        return;
    }

    // Освобождаем блоки директории (если они есть)
    for (int i = 0; i < inode_table[dir_inode].block_count; i++) {
        int block_index = inode_table[dir_inode].blocks[i];
        if (block_index >= 0 && block_index < MAX_BLOCKS) {
            free_block(block_index);
        }
    }

    // Освобождаем inode
    inode_table[dir_inode].is_used = 0;
    superblock.free_inodes++;

    // Удаляем запись из directory
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

    printf("Директория '%s' и все её содержимое успешно удалены.\n", dir_path);
}