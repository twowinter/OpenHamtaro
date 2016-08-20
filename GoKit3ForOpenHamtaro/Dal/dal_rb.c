#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dal_rb.h"
#include "gizwits_protocol.h"


#define min(a, b) (a)<(b)?(a):(b)


void DalRbCreate(dal_rb_t* rb)
{
    if(NULL == rb)
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        #endif
        return;
    }

    rb->rbHead = rb->rbBuff;
    rb->rbTail = rb->rbBuff;
}

void DalRbDelete(dal_rb_t* rb)
{
    if(NULL == rb)
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        #endif
        return;
    }

    rb->rbBuff = NULL;
    rb->rbHead = NULL;
    rb->rbTail = NULL;
    rb->rbCapacity = 0;
}

size_t DalRbCapacity(dal_rb_t *rb)
{
    if(NULL == rb)
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        #endif
        return -1;
    }

    return rb->rbCapacity;
}

size_t DalRbCanRead(dal_rb_t *rb)
{
    if(NULL == rb)
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        #endif
        return -1;
    }

    if (rb->rbHead == rb->rbTail)
    {
        return 0;
    }

    if (rb->rbHead < rb->rbTail)
    {
        return rb->rbTail - rb->rbHead;
    }

    return DalRbCapacity(rb) - (rb->rbHead - rb->rbTail);
}

size_t DalRbCanWrite(dal_rb_t *rb)
{
    if(NULL == rb)
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        #endif
        return -1;
    }

    return DalRbCapacity(rb) - DalRbCanRead(rb);
}

size_t DalRbRead(dal_rb_t *rb, void *data, size_t count)
{
    int copySz = 0;

    if(NULL == rb)
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: input rb is NULL\r\n");
        #endif
        return -1;
    }

    if(NULL == data)
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: input data is NULL\r\n");
        #endif
        return -1;
    }

    if (rb->rbHead < rb->rbTail)
    {
        copySz = min(count, DalRbCanRead(rb));
        memcpy(data, rb->rbHead, copySz);
        rb->rbHead += copySz;
        return copySz;
    }
    else
    {
        if (count < DalRbCapacity(rb)-(rb->rbHead - rb->rbBuff))
        {
            copySz = count;
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead += copySz;
            return copySz;
        }
        else
        {
            copySz = DalRbCapacity(rb) - (rb->rbHead - rb->rbBuff);
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead = rb->rbBuff;
            copySz += DalRbRead(rb, (char*)data+copySz, count-copySz);
            return copySz;
        }
    }
}

size_t DalRbWrite(dal_rb_t *rb, const void *data, size_t count)
{
    int tailAvailSz = 0;

    if(NULL == rb)
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: rb is empty \r\n");
        #endif
        return -1;
    }

    if(NULL == data)
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: data is empty \r\n");
        #endif
        return -1;
    }

    if (count >= DalRbCanWrite(rb))
    {
        #if EN_DEBUG > 0
        GIZWITS_LOG("ERROR: no memory %d \r\n", DalRbCanWrite(rb));
        #endif
        return -1;
    }

    if (rb->rbHead <= rb->rbTail)
    {
        tailAvailSz = DalRbCapacity(rb) - (rb->rbTail - rb->rbBuff);
        if (count <= tailAvailSz)
        {
            memcpy(rb->rbTail, data, count);
            rb->rbTail += count;
            if (rb->rbTail == rb->rbBuff+DalRbCapacity(rb))
            {
                rb->rbTail = rb->rbBuff;
            }
            return count;
        }
        else
        {
            memcpy(rb->rbTail, data, tailAvailSz);
            rb->rbTail = rb->rbBuff;

            return tailAvailSz + DalRbWrite(rb, (char*)data+tailAvailSz, count-tailAvailSz);
        }
    }
    else
    {
        memcpy(rb->rbTail, data, count);
        rb->rbTail += count;
        return count;
    }
}

