/*
 * File:   main.cpp
 * Author: LSX
 *
 * Created on 2012年6月3日, 上午12:47
 */
#include<stdio.h>
#include <cstdlib>
#include<unistd.h>
#include <string>
#include<iostream>
#include<signal.h>
#include<sys/time.h>
#include<errno.h>
#include<pthread.h>
#include "xmlParser.h"
#include "LFormatConvert.h"
#include "FileOper.h"
#include"Queue.h"

using namespace std;

string SFolder, DFolder, BinPath, InFileBefore, InFileAfter, OutFileSuffix;
string Thread_Num, BeforeTime, FinishDelete, FinishNoDelete, DFileExist;

Queue Q;
pthread_mutex_t mutex;
int str_replace(char* str, char* str_src, char* str_des);
void createCMD(char* runCMD, fileinfo file);

void XmlParse() {
    XMLNode xMainNode = XMLNode::openFileHelper("config.xml", "head");
    SFolder = xMainNode.getChildNode("LFormatConvert").getChildNode("Directory").getChildNode("Source").getAttribute("Folder");
    cout << "SourceFolder:" << SFolder << endl;
    DFolder = xMainNode.getChildNode("LFormatConvert").getChildNode("Directory").getChildNode("Destination").getAttribute("Folder");
    cout << "DestinationFolder:" << DFolder << endl;
    BinPath = xMainNode.getChildNode("LFormatConvert").getAttribute("Path");
    cout << "BinPath:" << BinPath << endl;
    InFileBefore = xMainNode.getChildNode("LFormatConvert").getChildNode("Parameter").getChildNode("InFileBefore").getAttribute("Value");
    cout << "InFileBefore:" << InFileBefore << endl;
    InFileAfter = xMainNode.getChildNode("LFormatConvert").getChildNode("Parameter").getChildNode("InFileAfter").getAttribute("Value");
    cout << "InFileAfter:" << InFileAfter << endl;
    OutFileSuffix = xMainNode.getChildNode("LFormatConvert").getChildNode("Parameter").getChildNode("OutFileSuffix").getAttribute("Value");
    cout << "OutFileSuffix:" << OutFileSuffix << endl;
    Thread_Num = xMainNode.getChildNode("LFormatConvert").getChildNode("Parameter").getChildNode("Thread_Num").getAttribute("Value");
    cout << "Thread_Num:" << Thread_Num << endl;
    BeforeTime = xMainNode.getChildNode("LFormatConvert").getChildNode("Parameter").getChildNode("BeforeTime").getAttribute("Value");
    cout << "BeforeTime:" << BeforeTime << endl;
    FinishDelete = xMainNode.getChildNode("LFormatConvert").getChildNode("Parameter").getChildNode("FinishDelete").getAttribute("Value");
    cout << "FinishDelete:" << FinishDelete << endl;
    DFileExist = xMainNode.getChildNode("LFormatConvert").getChildNode("Parameter").getChildNode("DFileExist").getAttribute("Value");
    cout << "DFileExist:" << DFileExist << endl;
    FinishNoDelete = xMainNode.getChildNode("LFormatConvert").getChildNode("Directory").getChildNode("FinishNoDelete").getAttribute("Folder");
    cout << "FinishNoDelete:" << FinishNoDelete << endl;
}

void *Thrd_ConvertRun(void* arg) {
    ThreadArgs Args = *(struct ThreadArgs*) arg;
    fileinfo file;
    char TmpRename[1024], runCMD[2048];
    Task CurTask;
    while (1) {
        if (!isEmpty(Q)) {
            //对互斥锁上锁
            if (pthread_mutex_lock(&mutex) != 0) {
                printf("Thread %d lock failed!\n", Args.thread_Num);
                pthread_exit((void*) - 1);
            }
            file = queueFront(Q);
            popQueue(Q);
            pthread_mutex_unlock(&mutex); // 解锁
            //判断文件是否存在
            if (access(file.fullname, F_OK) != 0) continue;
            //生成重命名后的名称
            memset(TmpRename, 0x00, sizeof (TmpRename));
            strcpy(CurTask.Rname, file.fullname);
            str_replace(CurTask.Rname, "/", "_");
            strcpy(TmpRename, FinishNoDelete.c_str());
            strcat(TmpRename, CurTask.Rname);
            strcpy(CurTask.Rname, TmpRename);
            //重命名成中间转换文件
            memset(TmpRename, 0x00, sizeof (TmpRename));
            strcpy(TmpRename, file.fullname);
            strcat(TmpRename, ".convert");
            rename(file.fullname, TmpRename);
            strcpy(CurTask.Sname, TmpRename);
            strcpy(file.fullname, TmpRename);
            CurTask.Num = Args.thread_Num;

            string tmp(file.name);
            tmp.replace(tmp.length() - 4, 4, OutFileSuffix);
            strcat(CurTask.Dname, DFolder.c_str());
            strcat(CurTask.Dname, tmp.c_str());
            int i = 0;
            while (access(CurTask.Dname, F_OK) == 0) {
                i++;
                if (strcmp(DFileExist.c_str(), "1") == 0) {
                    if (remove(CurTask.Dname) != 0)
                        printf(".......Cover file Fail\n");
                } else {
                    tmp = CurTask.Dname;
                    char tmpi[5];
                    char buf[5] = {0};
                      sprintf(buf, "%d", i);
                    strcpy(tmpi, "(");
                    strcat(tmpi,buf);
                    strcat(tmpi, ")");
                    tmp.insert(tmp.find_last_of("."), tmpi);
                    strcpy(file.name,  tmp.substr(tmp.find_last_of("/")+1,tmp.length()-tmp.find_last_of("/")).c_str());
                    strcpy(CurTask.Dname, tmp.c_str());
                }
            }

            createCMD(runCMD, file);
            pthread_mutex_lock(&mutex);
            *Args.CurTask = CurTask;
            pthread_mutex_unlock(&mutex);
            printf("............runCMD:%s......................\n", runCMD);
            pthread_exit((void*) system(runCMD));
        } else {
            sleep(60);
        }
    }
    pthread_exit(NULL);
}

void *Wait_Thrd(void *arg) {
    ThreadArgs Args = *(struct ThreadArgs*) arg;
    void *tret;
    if (pthread_join(Args.thread_Handle, &tret) != 0) { // 等待thread[i]线程结束，线程函数返回值放在tret中
        printf("Join thread %d error!\n", Args.thread_Num);
        exit(1);
    } else {
        printf("............Join thread %d success,exit code %d !..............\n", Args.thread_Num, (void*) tret);
        if (tret == 0) {
            if (strcmp(FinishDelete.c_str(), "0") == 0) {
                if (access(FinishNoDelete.c_str(), F_OK) != 0) {
                    mkdir(FinishNoDelete.c_str(), 0755);
                }
                if (rename(Args.CurTask->Sname, Args.CurTask->Rname) != 0)
                    printf("..............Move %s Fail!.................\n", Args.CurTask->Sname);
                else
                    printf("................move %s Succeed!.............\n", Args.CurTask->Sname);
            } else {
                if (remove(Args.CurTask->Sname) != 0)
                    printf(".............Delete %s Fail!..............\n", Args.CurTask->Sname);
                else
                    printf("..............Delete %s Succeed!...............\n", Args.CurTask->Sname);
            }
            printf("...............%s To %s Convert Succeed!...............\n", Args.CurTask->Sname, Args.CurTask->Dname);
        } else {
            if (remove(Args.CurTask->Dname) != 0)
                printf(".................Delete Convert %s file fail!............\n", Args.CurTask->Sname);
            printf("................%s To %s Convert Fail!.........\n", Args.CurTask->Sname, Args.CurTask->Dname);
        }
    }
}

int str_replace(char* str, char* str_src, char* str_des) {
    char *ptr = NULL;
    char buff[256];
    char buff2[256];
    int i = 0;

    if (str != NULL) {
        strcpy(buff2, str);
    } else {
        printf("str_replace err!\n");
        return -1;
    }
    memset(buff, 0x00, sizeof (buff));
    while ((ptr = strstr(buff2, str_src)) != 0) {
        if (ptr - buff2 != 0) memcpy(&buff[i], buff2, ptr - buff2);
        memcpy(&buff[i + ptr - buff2], str_des, strlen(str_des));
        i += ptr - buff2 + strlen(str_des);
        strcpy(buff2, ptr + strlen(str_src));
    }
    strcat(buff, buff2);
    strcpy(str, buff);
    return 0;
}

/*创建一个转换的命令字符串*/
void createCMD(char* runCMD, fileinfo file) {
    char RunCMD[2048];
    memset(RunCMD, 0x00, sizeof (RunCMD));
    if (strcmp(file.type, "file") == 0 && file.mod_time < atoi(BeforeTime.c_str())) {
        strcpy(RunCMD, BinPath.c_str());
        strcat(RunCMD, " ");
        strcat(RunCMD, InFileBefore.c_str());
        strcat(RunCMD, " ");
        strcat(RunCMD, file.fullname);
        strcat(RunCMD, " ");
        strcat(RunCMD, InFileAfter.c_str());
        strcat(RunCMD, " ");
        strcat(RunCMD, DFolder.c_str());
        string tmp(file.name);
        tmp.replace(tmp.length() - 4, 4, OutFileSuffix);
        strcat(RunCMD, tmp.c_str());
        strcpy(runCMD, RunCMD);
    }
}

void fillQueue(int num) {
    filelist* Sfilelist = NULL;
    char FolderName[254];

    if (isEmpty(Q)) {
        strcpy(FolderName, SFolder.c_str());
        Sfilelist = FileListSort(GetFileList(FolderName));
        ShowList(Sfilelist);
        if (Sfilelist == NULL) {
            printf("...........Read file list Fail!!..............\n");
            return;
        }
        while (Sfilelist->nextfile) {
            if (strcmp(Sfilelist->nextfile->file.type, "file") == 0 && strstr(Sfilelist->nextfile->file.fullname, "convert") == NULL && Sfilelist->nextfile->file.mod_time < atoi(BeforeTime.c_str())) {
                pushQueue(Q, Sfilelist->nextfile->file);
            } else {
                printf(".........Not effective file:%s.........\n", Sfilelist->nextfile->file.fullname);
            }
            Sfilelist = Sfilelist->nextfile;
        }
        FreeAllNode(Sfilelist);
    } else {
        printf(".......Wait Queueing......\n");
    }
}

int main(int argc, char** argv) {
    int res = 0;
    ThreadArgs waitArgs, runArgs;

    XmlParse();
    if (access(SFolder.c_str(), F_OK) != 0) {
        printf("...............SFolder No exist!................\n");
        return -1;
    }
    if (access(DFolder.c_str(), F_OK) != 0) {
        mkdir(DFolder.c_str(), 0755);
    }
    int RunCount = atoi(Thread_Num.c_str());
    pthread_t Runthread[RunCount], Waithread[RunCount];
    Task waitTask[RunCount];

    pthread_mutex_init(&mutex, NULL); //创建互斥锁

    signal(SIGALRM, fillQueue);
    struct itimerval tick;
    memset(&tick, 0, sizeof (tick));
    tick.it_value.tv_sec = 1;
    tick.it_value.tv_usec = 0;
    tick.it_interval.tv_sec = 60;
    tick.it_interval.tv_usec = 0;
    res = setitimer(ITIMER_REAL, &tick, NULL);
    if (res) printf("..........Set timer failed!!................\n");

    for (int i = 0; i < RunCount; i++) {
        runArgs.CurTask = &waitTask[i];
        runArgs.thread_Num = i;
        if (pthread_create(&Runthread[i], NULL, Thrd_ConvertRun, (void*) &runArgs) != 0) {
            printf("..................Create Run thread %d error!.....................\n", i);
            exit(1);
        } else {
            printf("..................Create Run thread %d success!...............\n", i);
            sleep(1);
            waitArgs.CurTask = &waitTask[i];
            waitArgs.thread_Handle = Runthread[i];
            waitArgs.thread_Num = i;
            if (pthread_create(&Waithread[i], NULL, Wait_Thrd, (void*) &waitArgs) != 0) {
                printf(".....................Create Wait thread %d error!!.............\n", i);
                exit(1);
            } else {
                printf("....................Create Wait thread %d Success !!...........\n", i);
            }
        }
    }
    while (1) {
        sleep(10);
    }
    pthread_mutex_destroy(&mutex); //清除互斥锁
    return 0;
}

