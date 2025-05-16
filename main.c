#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sfs.h"

int main() {
    const char *diskname = "virtual_disk.img";
    char command[256];
    char data[BLOCK_SIZE];

    sfs_mkfs(diskname);
    sfs_mount(diskname);

    while (1) {
        printf("\n");
        sfs_pwdm();
        //printf("\nВведите команду (c, d, w, r, e, mkdir, rm, mv, ls, rmdir, cd, pwd): ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Убираем символ новой строки

        char *cmd = strtok(command, " ");  // Получаем команду (например, c)
        char *arg = strtok(NULL, " ");    // Получаем аргумент (например, kaka21)

        if (cmd == NULL) {
            printf("Неверный формат команды.(Для справки - help)\n");
            continue;
        }

        if (*cmd != 'l' && *cmd != 'e' && strcmp(cmd, "pwd") && strcmp(cmd, "help") && arg == NULL) {
            printf("Неверный формат команды.(Для справки - help)\n");
            continue;
        }

        if (strcmp(cmd, "c") == 0) {
            sfs_create(arg);
        } else if (strcmp(cmd, "d") == 0) {
            sfs_delete(arg);
        } else if (strcmp(cmd, "w") == 0) {
            printf("Введите данные для записи в файл '%s': ", arg);
            fgets(data, sizeof(data), stdin);
            sfs_write(arg, data);
        } else if (strcmp(cmd, "r") == 0) {
            sfs_read(arg);
        } else if (strcmp(cmd, "e") == 0) {
            sfs_umount();
            break;
        } else if (strcmp(cmd, "mkdir") == 0) {
            sfs_create_dir(arg);
        } else if (strcmp(cmd, "mv") == 0) {
            //char *filename = strtok(NULL, " ");
            char *dirname = strtok(NULL, " ");
            sfs_move_to_dir(arg, dirname);
        } else if (strcmp(cmd, "ls") == 0) {
            sfs_ls_dir(arg);
        } else if (strcmp(cmd, "rmdir") == 0) {
            sfs_delete_dir(arg);
        } else if (strcmp(cmd, "cd") == 0) {
            if (arg != NULL) {
                sfs_cd(arg);
            } else {
                printf("Необходимо указать директорию для перехода.\n");
            }
        } else if (strcmp(cmd, "pwd") == 0) {
            sfs_pwd();
        } else if (strcmp(cmd, "rm") == 0) {
            sfs_delete_dir_recursive(arg);
        }else if (strcmp(cmd, "help") == 0) {
            help ();
        } else {
            printf("Неизвестная команда.(Для справки - help)\n");
        }
    }

    return 0;
}