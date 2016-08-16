
#ifndef _DAL_RB_H
#define _DAL_RB_H

#include <stdio.h>

/** 环形缓冲区变量定义 **/
typedef struct {
    size_t rbCapacity;
    char  *rbHead;
    char  *rbTail;
    char  *rbBuff;
}dal_rb_t;

void DalRbCreate(dal_rb_t* rb);

void DalRbDelete(dal_rb_t* rb);

size_t DalRbCapacity(dal_rb_t *rb);

size_t DalRbCanRead(dal_rb_t *rb);

size_t DalRbCanWrite(dal_rb_t *rb);

size_t DalRbRead(dal_rb_t *rb, void *data, size_t count);

size_t DalRbWrite(dal_rb_t *rb, const void *data, size_t count);


#endif
