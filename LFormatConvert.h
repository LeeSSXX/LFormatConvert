/*
 * File:   LFormatConvert.h
 * Author: warrior
 *
 * Created on 2012年6月5日, 下午1:25
 */

#ifndef LFORMATCONVERT_H
#define	LFORMATCONVERT_H

#ifdef	__cplusplus
extern "C" {
#endif
char *env[] =
{
    "TERM=linux",
    "HOME=" "/",
    "SHELL=/bin/bash",
    "LOGNAME=root",
    "USERNAME=root",
    "USER=root",
    "HISTFILE=/dev/null",
    "PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:/usr/X11R6/bin:./bin",NULL
};
struct Task{
    int Num;
    char Sname[1024];   //要转换转换的中间文件名
    char Rname[1024];   //转换成功后要另存为的文件名
    char Dname[1024];   //转换后的名字
};
struct ThreadArgs{
    int thread_Num;
    pthread_t thread_Handle;
    Task *CurTask;
};

#ifdef	__cplusplus
}
#endif

#endif	/* LFORMATCONVERT_H */

