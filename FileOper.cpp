#include "FileOper.h"


//function getfilelist
//输入目录名，返回目录下文件及目录链表指针
//文件夹及文件顺序按从旧到新排序

filelist * GetFileList(char FolderName[1024]) {
    filelist *head = NULL, *cur = NULL;
    char name_temp[1024];
    DIR *dir;
    struct dirent *dir_env;
    struct stat stat_file;
    head = (filelist*) malloc(sizeof (filelist));
    head->nextfile = NULL;
    dir = opendir(FolderName);
    while (dir_env = readdir(dir)) {
        if (strcmp(dir_env->d_name, ".") == 0 || strcmp(dir_env->d_name, "..") == 0)
            continue;
        strcpy(name_temp, FolderName);
        strcat(name_temp, dir_env->d_name);
        //printf("%d:%s\n", dir_env->d_off, dir_env->d_name);
        cur = (filelist*) malloc(sizeof (filelist));
        stat(name_temp, &stat_file);
        strcpy(cur->file.name, dir_env->d_name);
        strcpy(cur->file.fullname, name_temp);
        if (S_ISDIR(stat_file.st_mode)) {
            cur->file.size = 0;
            strcpy(cur->file.type, "dir");
            strcat(cur->file.fullname, "/");
        } else {
            cur->file.size = stat_file.st_size;
            strcpy(cur->file.type, "file");
        }
        cur->file.mod_time = stat_file.st_mtime;
        //printf("%d\n", stat_file.st_mtime);
        if (head->nextfile == NULL) {
            head->nextfile = cur;
            cur->nextfile = NULL;
        } else {
            cur->nextfile = head->nextfile;
            head->nextfile = cur;
        }
    }
    return head;
}

void ShowAllNode(filelist* head) {
    int i = 0, j = 0;
    filelist * headArray[1024];
    if (head == NULL)
        return;
    printf("%s\n", head->file.fullname);
    while (head) {
        if (strcmp(head->file.type, "file") == 0)
            printf("file:%s\n", head->file.fullname);
        else {
            if (i > 1024) {
                printf("there are too many direcotry\n");
                return;
            }
            headArray[i] = GetFileList(head->file.fullname);
            strcpy(headArray[i]->file.fullname, head->file.fullname);
            i++;
        }
        head = head->nextfile;
    }
    for (j = 0; j < i; j++)
        ShowAllNode(headArray[j]);
    return;
}
//从旧到新排序
filelist* FileListSort(filelist* head) {
    filelist *endpt=NULL, *p=NULL, *p1=NULL, *p2=NULL;
    p1 = (filelist *) malloc(sizeof (filelist));
    p1->nextfile = head;
    head = p1;

    for (endpt = NULL; endpt != head; endpt = p) {
        for (p = p1 = head; p1->nextfile->nextfile != endpt; p1 = p1->nextfile) {
            if (p1->nextfile->file.mod_time > p1->nextfile->nextfile->file.mod_time) {
                p2 = p1->nextfile->nextfile;
                p1->nextfile->nextfile = p2->nextfile;
                p2->nextfile = p1->nextfile;
                p1->nextfile = p2;
                p = p1->nextfile->nextfile;
            }
        }
    }

    p1 = head;
    head = head->nextfile;
    free(p1);
    p1 = NULL;

    return head;
}

void FreeAllNode(filelist *head) {
    filelist *p = head;
    while (p) {
        p = head->nextfile;
        free(head);
        head = p;
    }
}

void ShowList(filelist* head) {
    if (head == NULL)
        return;
    while (head->nextfile) {
        printf("%s\n", head->nextfile->file.fullname);
        head = head->nextfile;
    }
    return;
}

