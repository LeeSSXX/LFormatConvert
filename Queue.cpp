#include"Queue.h"

/* 初始化队列 */
void initQueue(Queue &Q) {
    int i;

    for (i = 0; i < maxSize; i++) {
        Q.file[i].size = 0; // 初始文件大小都为0
        Q.front.size = 0;
        Q.rear.size = 0;
    }
}

/* 判断队列是否为空, 为空返回true, 不为空则返回flase */
bool isEmpty(Queue &Q) {
    // 如果队头不等于0,则表示不为空
    if (Q.front.size != 0) {
        return false;
    }
    return true;
}

/* 判断队列是否以满, 以满返回true, 没满则返回flase */
bool isFull(Queue &Q) {
    // 如果队尾等于0,则表示队列没满
    if (Q.file[maxSize - 1].size == 0) {
        return false;
    }
    return true;
}

/* 若队列Q非满,则将元素x插入Q的队尾.此操作简称 入队 */
bool pushQueue(Queue &Q, fileinfo x) {
    // 检测队列是否以满
    if (isFull(Q) == true) {
        return false;
    }
    int i;
    for (i = 0; i < maxSize; i++) {
        // 当为0时则表示此位置没被赋值,即可在此位置入队,且该值变为队尾
        if (Q.file[i].size == 0) {
            // 设队头
            if (i == 0) {
                Q.front = x;
            }
            Q.file[i] = x;
            Q.rear = x; // 设队尾,每添加一个值,则该值即为队尾
            return true;
        }
    }
}

/* 若队列Q非空,则删去Q的队头元素,并返回该元素. 此操作简称 出队 */
bool popQueue(Queue &Q) {
    // 检测队列是否为空
    if (isEmpty(Q) == true) {
        return false;
    }
    int i;
    /* 删除队头元素，并将所有队列元素提前一个位置 */
    for (i = 0; i < maxSize; i++) {
        // 判断队列中元素是否以全部提前
        if (Q.file[i].size == 0 || i == maxSize - 1) {
            Q.file[i].size = 0;
            Q.front = Q.file[0]; // 设队头,每出队一个值,则原来第二个值变成队头
            return true;
        }
        Q.file[i] = Q.file[i + 1]; // 将队列元素提前
    }
}

/* 若队列Q非空,则返回队头元素,但不改变队列Q的状态. */
fileinfo queueFront(Queue &Q) {
    // 检测队列是否为空
    if (isEmpty(Q) == true) {
        return Q.front;
    }
    return Q.front; // 返回队头元素

}

/* 显示队列中的元素 */
bool queueDisplay(Queue &Q) {
    // 检测队列是否为空
    if (isEmpty(Q) == true) {
        return false;
    }
    int i;
    for (i = 0; i < maxSize; i++) {
        // 判断队列中元素是否以全部显示
        if (Q.file[i].size == 0) {
            return true;
        }
        printf("The %d is:%s\n",i,Q.file[i].fullname);
    }
}
