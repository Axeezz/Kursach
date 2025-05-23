#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sfs.h"

int main() {
    const char *diskname = "virtual_disk.img";
    char command[256];

    sfs_mkfs(diskname);
    sfs_mount(diskname);

    while (1) {
        printf("\n");
        sfs_pwdm();
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Убираем символ новой строки

        char *cmd = strtok(command, " ");
        char *arg = strtok(NULL, " ");

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
            sfs_write(arg);
        } else if (strcmp(cmd, "r") == 0) {
            sfs_read(arg);
        } else if (strcmp(cmd, "e") == 0) {
            sfs_umount();
            break;
        } else if (strcmp(cmd, "mkdir") == 0) {
            sfs_create_dir(arg);
        } else if (strcmp(cmd, "mv") == 0) {
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