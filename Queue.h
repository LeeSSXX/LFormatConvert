/*
 * File:   Queue.h
 * Author: warrior
 *
 * Created on 2012年6月13日, 下午11:23
 */

#ifndef QUEUE_H
#define	QUEUE_H
/*
（1）initQueue（Q）
　 置空队。构造一个空队列Q。
（2）isEmpty（Q）
　 判断队列是否空。若队列Q为空，则返回真值，否则返回假值。
（3）isFull(Q)
   判断队列是否以满, 以满返回true, 没满则返回flase
（4) pushQueue（Q，x）
　 若队列Q非满，则将元素x插入Q的队尾。此操作简称 入队 。
（5）popQueue（Q）
　 若队列Q非空，则删去Q的队头元素，并返回该元素。此操作简称 出队 。
（6）queueFront（Q）
　 若队列Q非空，则返回队头元素，但不改变队列Q的状态。
（7）queueDisplay（Q）
　 显示队列中的元素。
 */

#include "iostream.h"
#include"FileOper.h"

#define maxSize 100  // 存储数据大小, 可以随便设定值

struct Queue {
    fileinfo file[maxSize];
    fileinfo front; // 队首
    fileinfo rear; // 队尾
};

void initQueue(Queue &Q);
bool isEmpty(Queue &Q);
bool isFull(Queue &Q);
bool pushQueue(Queue &Q, fileinfo x);
bool popQueue(Queue &Q);
fileinfo queueFront(Queue &Q);
bool queueDisplay(Queue &Q);



#endif	/* QUEUE_H */

