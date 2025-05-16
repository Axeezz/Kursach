#ifndef SFS_H
#define SFS_H

#include <stdio.h>
#include <string.h>

// Константы
#define MAX_FILENAME_LENGTH 256
#define MAX_BLOCKS 1024
#define MAX_FILES 128
#define BLOCK_SIZE 4096

// Структуры
typedef struct {
    int total_blocks;
    int free_blocks;
    int total_inodes;
    int free_inodes;
    unsigned char block_bitmap[MAX_BLOCKS/8];
} Superblock;

typedef struct {
    int is_used;
    int is_directory;
    int directory_inode_index;
    char filename[MAX_FILENAME_LENGTH];
    int size;
    int block_count;
    int blocks[16]; // Максимум 16 блоков на файл
} Inode;

typedef struct {
    int inode_index;
    char filename[MAX_FILENAME_LENGTH];
} DirectoryEntry;

// Глобальные переменные (объявлены как extern)
extern FILE *disk;
extern Superblock superblock;
extern Inode inode_table[MAX_FILES];
extern DirectoryEntry directory[MAX_FILES];
extern int current_directory_inode;
extern char current_directory[MAX_FILENAME_LENGTH];

// Прототипы функций
void sfs_mkfs(const char *diskname);
void sfs_mount(const char *diskname);
void sfs_umount();
void sfs_pwdm();
void sfs_pwd();
void help();
int is_valid_filesystem(FILE *f);
long get_block_offset(int block_index);

// Функции для файлов
void sfs_create(const char *filename);
void sfs_write(const char *filename, const char *data);
void sfs_read(const char *filename);
void sfs_delete(const char *filename);

// Функции для директорий
void sfs_cd(const char *dirname);
void sfs_create_dir(const char *dirname);
void sfs_ls_dir(const char *dirname);
void sfs_delete_dir(const char *dirname);
void sfs_delete_dir_recursive(const char *dirname);

// Вспомогательные функции
int find_free_inode();
int find_free_block();
void allocate_block(int block_index);
void free_block(int block_index);
void print_current_directory();
void build_path_from_inode(int inode, char *path, size_t path_size);
int resolve_path_to_inode(const char *path, int *parent_inode_index, char *basename);
void sfs_move_to_dir(const char *file_input, const char *dir_input);

#endif