/*
 * File:   FileOper.h
 * Author: warrior
 *
 * Created on 2012年6月5日, 下午3:01
 */

#ifndef FILEOPER_H
#define	FILEOPER_H
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string>
#include <ctime>
//文件信息
typedef struct FileInfo_Type{
    char name[256];                     //不带路径文件名
    char fullname[1024];                //带路径文件名
    int size;                           //文件大小
    time_t mod_time;                    //文件更改时间
    char type[10];                      //文件类型
}fileinfo;
typedef struct FileList_Type{
    fileinfo file;
    struct FileList_Type* nextfile;
}filelist;

filelist* GetFileList(char FolderName[1024]);
filelist* FileListSort(filelist* head);
void FreeAllNode(filelist *head);
void ShowAllNode(filelist* head);
void ShowList(filelist* head);
#endif	/* FILEOPER_H */

