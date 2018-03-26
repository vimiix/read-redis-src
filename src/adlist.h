/* adlist.h - A generic doubly linked list implementation
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ADLIST_H__
#define __ADLIST_H__
// 双端链表头文件

/* Node, List, and Iterator are the only data structures used currently. */


// 链表节点结构体定义
typedef struct listNode {
    struct listNode *prev; // 前置节点
    struct listNode *next; // 后置节点
    void *value; // 节点值
} listNode;

// Redis为双端链表定义了一个迭代器结构，其能正序和逆序的访问list结构。
typedef struct listIter {
    listNode *next; // 下一个节点的指针
    int direction; //方向，正序和逆序
} listIter;

// 链表的结构体
typedef struct list {
    listNode *head; // 链表头节点
    listNode *tail; // 链表尾节点
    void *(*dup)(void *ptr); // 自定义节点值复制函数
    void (*free)(void *ptr); // 自定义节点值释放函数
    int (*match)(void *ptr, void *key); // 自定义节点值匹配函数
    unsigned long len; // 链表长度
} list;

/* Functions implemented as macros */
// Redis对链表结构体提供了一系列的宏定义函数，方便操作其结构体参数
#define listLength(l) ((l)->len) // 获取list长度
#define listFirst(l) ((l)->head) // 获取list头节点指针
#define listLast(l) ((l)->tail) // 获取list尾节点指针
#define listPrevNode(n) ((n)->prev) // 获取当前节点的前一个节点
#define listNextNode(n) ((n)->next) // 获取当前节点的后一个节点
#define listNodeValue(n) ((n)->value) // 获取当前节点的值

#define listSetDupMethod(l,m) ((l)->dup = (m)) // 设定节点值复制函数
#define listSetFreeMethod(l,m) ((l)->free = (m)) // 设定节点值释放函数
#define listSetMatchMethod(l,m) ((l)->match = (m)) // 设定节点值匹配函数

#define listGetDupMethod(l) ((l)->dup) // 获取节点值赋值函数
#define listGetFree(l) ((l)->free) // 获取节点值释放函数
#define listGetMatchMethod(l) ((l)->match) // 获取节点值匹配函数

/* Prototypes */
list *listCreate(void);
void listRelease(list *list);
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
void listDelNode(list *list, listNode *node);
listIter *listGetIterator(list *list, int direction);
listNode *listNext(listIter *iter);
void listReleaseIterator(listIter *iter);
list *listDup(list *orig);
listNode *listSearchKey(list *list, void *key);
listNode *listIndex(list *list, long index);
void listRewind(list *list, listIter *li);
void listRewindTail(list *list, listIter *li);
void listRotate(list *list);

/* Directions for iterators */
// 链表迭代器的方向宏定义
#define AL_START_HEAD 0  // 从头到尾
#define AL_START_TAIL 1  // 从尾到头

#endif /* __ADLIST_H__ */
