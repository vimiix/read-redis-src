/* adlist.c - A generic doubly linked list implementation
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
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


#include <stdlib.h>
#include "adlist.h"
#include "zmalloc.h"

/* Create a new list. The created list can be freed with
 * AlFreeList(), but private value of every node need to be freed
 * by the user before to call AlFreeList().
 *
 * On error, NULL is returned. Otherwise the pointer to the new list. */
// 创建一个空链表
list *listCreate(void)
{
    struct list *list;

    if ((list = zmalloc(sizeof(*list))) == NULL) // 申请内存
        return NULL;
    list->head = list->tail = NULL; // 空链表的头指针和尾指针均为空
    list->len = 0;       // 设定长度
    list->dup = NULL;    // 自定义复制函数初始化
    list->free = NULL;   // 自定义释放函数初始化
    list->match = NULL;  // 自定义匹配函数初始化
    return list;
}

/* Free the whole list.
 *
 * This function can't fail. */
// 释放一个链表
void listRelease(list *list)
{
    unsigned long len;
    listNode *current, *next;

    current = list->head; // 获取头节点指针
    len = list->len; // 获取链表长度
    while(len--) {
        next = current->next; //保存下一个节点指针
        if (list->free) list->free(current->value); // 如果存在自定义的释放节点值函数，就调用
        zfree(current); // 释放当前节点内存
        current = next; // 获取下一个节点的指针
    }
    zfree(list); // 释放链表头
}

/* Add a new node to the list, to head, containing the specified 'value'
 * pointer as value.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the 'list' pointer you pass to the function is returned. */
// 该函数向list的头部插入一个节点
list *listAddNodeHead(list *list, void *value)
{
    listNode *node;

    if ((node = zmalloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;
    if (list->len == 0) { // 如果链表为空
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else { // 如果链表非空
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    list->len++; // 长度+1
    return list;
}

/* Add a new node to the list, to tail, containing the specified 'value'
 * pointer as value.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the 'list' pointer you pass to the function is returned. */
// 该函数向list的尾部插入一个节点
// 注释同上
list *listAddNodeTail(list *list, void *value)
{
    listNode *node;

    if ((node = zmalloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;
    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
    list->len++;
    return list;
}

// 向任意位置插入节点
// 其中，old_node为插入位置
//      value为插入节点的值
//      after为0时表示插在old_node前面，为1时表示插在old_node后面
list *listInsertNode(list *list, listNode *old_node, void *value, int after) {
    listNode *node;

    if ((node = zmalloc(sizeof(*node))) == NULL)
        return NULL;
    node->value = value;
    if (after) { // 向后插入
        node->prev = old_node;
        node->next = old_node->next;
        // 如果old_node为尾节点的话需要改变tail
        if (list->tail == old_node) {
            list->tail = node;
        }
    } else { // 向前插入
        node->next = old_node;
        node->prev = old_node->prev;
        // 如果old_node为头节点的话需要改变head
        if (list->head == old_node) {
            list->head = node;
        }
    }
    if (node->prev != NULL) {
        node->prev->next = node;
    }
    if (node->next != NULL) {
        node->next->prev = node;
    }
    list->len++;
    return list;
}

/* Remove the specified node from the specified list.
 * It's up to the caller to free the private value of the node.
 *
 * This function can't fail. */
// 删除节点
void listDelNode(list *list, listNode *node)
{
    if (node->prev) // 删除节点不为头节点
        node->prev->next = node->next;
    else // 删除节点为头节点需要改变head的指向
        list->head = node->next;
    if (node->next) // 删除节点不为尾节点
        node->next->prev = node->prev;
    else // 删除节点为尾节点需要改变tail的指向
        list->tail = node->prev;
    if (list->free) list->free(node->value); // 释放节点值
    zfree(node); // 释放节点
    list->len--; // 长度 -1
}

/* Returns a list iterator 'iter'. After the initialization every
 * call to listNext() will return the next element of the list.
 *
 * This function can't fail. */
// 获取迭代器 
listIter *listGetIterator(list *list, int direction)
{
    listIter *iter; // 声明迭代器

    if ((iter = zmalloc(sizeof(*iter))) == NULL) return NULL;
    // 根据迭代方向来初始化iter
    if (direction == AL_START_HEAD)
        iter->next = list->head;
    else
        iter->next = list->tail;
    iter->direction = direction;
    return iter;
}

/* Release the iterator memory */
// 释放迭代器
void listReleaseIterator(listIter *iter) {
    zfree(iter); // 直接调用zfree来释放
}

/* Create an iterator in the list private iterator structure */
// 重置为正向迭代器
void listRewind(list *list, listIter *li) {
    li->next = list->head;
    li->direction = AL_START_HEAD;
}

// 重置为逆向迭代器
void listRewindTail(list *list, listIter *li) {
    li->next = list->tail;
    li->direction = AL_START_TAIL;
}

/* Return the next element of an iterator.
 * It's valid to remove the currently returned element using
 * listDelNode(), but not to remove other elements.
 *
 * The function returns a pointer to the next element of the list,
 * or NULL if there are no more elements, so the classical usage patter
 * is:
 *
 * iter = listGetIterator(list,<direction>);
 * while ((node = listNext(iter)) != NULL) {
 *     doSomethingWith(listNodeValue(node));
 * }
 *
 * */
// 根据direction属性来获取下一个迭代器
listNode *listNext(listIter *iter)
{
    listNode *current = iter->next;

    if (current != NULL) {
        if (iter->direction == AL_START_HEAD)
            iter->next = current->next;
        else
            iter->next = current->prev;
    }
    return current;
}

/* Duplicate the whole list. On out of memory NULL is returned.
 * On success a copy of the original list is returned.
 *
 * The 'Dup' method set with listSetDupMethod() function is used
 * to copy the node value. Otherwise the same pointer value of
 * the original node is used as value of the copied node.
 *
 * The original list both on success or error is never modified. */
// 复制整个链表
list *listDup(list *orig)
{
    list *copy;
    listIter iter;
    listNode *node;

    if ((copy = listCreate()) == NULL)
        return NULL;
    // 复制节点值操作函数
    copy->dup = orig->dup;
    copy->free = orig->free;
    copy->match = orig->match;
    // 重置迭代器
    listRewind(orig, &iter);
    while((node = listNext(&iter)) != NULL) {
        void *value;

        // 复制节点
        // 如果定义了dup函数，则按照dup函数来复制节点值
        if (copy->dup) {
            value = copy->dup(node->value);
            if (value == NULL) {
                listRelease(copy);
                return NULL;
            }
        } else // 如果没有则直接赋值
            value = node->value;
         // 依次向尾部添加节点
        if (listAddNodeTail(copy, value) == NULL) {
            listRelease(copy);
            return NULL;
        }
    }
    return copy;
}

/* Search the list for a node matching a given key.
 * The match is performed using the 'match' method
 * set with listSetMatchMethod(). If no 'match' method
 * is set, the 'value' pointer of every node is directly
 * compared with the 'key' pointer.
 *
 * On success the first matching node pointer is returned
 * (search starts from head). If no matching node exists
 * NULL is returned. */
// 根据给定节点值，在链表中查找该节点
listNode *listSearchKey(list *list, void *key)
{
    listIter iter;
    listNode *node;

    listRewind(list, &iter);
    while((node = listNext(&iter)) != NULL) {
        if (list->match) { // 如果定义了match匹配函数，则利用该函数进行节点匹配
            if (list->match(node->value, key)) {
                return node;
            }
        } else { // 如果没有定义match，则直接比较节点值
            if (key == node->value) { // 找到该节点
                return node;
            }
        }
    }
    // 没有找到就返回NULL
    return NULL;
}

/* Return the element at the specified zero-based index
 * where 0 is the head, 1 is the element next to head
 * and so on. Negative integers are used in order to count
 * from the tail, -1 is the last element, -2 the penultimate
 * and so on. If the index is out of range NULL is returned. */
// 根据序号查找节点
listNode *listIndex(list *list, long index) {
    listNode *n;

    if (index < 0) {  // 序号为负，则倒序查找
        index = (-index)-1;
        n = list->tail;
        while(index-- && n) n = n->prev;
    } else { // 正序查找
        n = list->head;
        while(index-- && n) n = n->next;
    }
    return n;
}

/* Rotate the list removing the tail node and inserting it to the head. */
// 旋转操作，就是讲表尾节点移除，然后插入到表头，成为新的表头
void listRotate(list *list) {
    listNode *tail = list->tail;

    if (listLength(list) <= 1) return;

    /* Detach current tail */
    // 取出表尾指针
    list->tail = tail->prev;
    list->tail->next = NULL;
    /* Move it as head */
    // 将其移动到表头并成为新的表头指针
    list->head->prev = tail;
    tail->prev = NULL;
    tail->next = list->head;
    list->head = tail;
}
