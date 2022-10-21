/**
* OpenEDOS Kernel v1.2.0
* 
* Copyright (c) 2022 Samuel Ardaya-Lieb
* 
* MIT License
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* 
* https://github.com/SamuelArdayaLieb/OpenEDOS
* 
*/

#include "include/queue.h"
#include "include/defines.h"
#include <stdint.h>
#include <string.h>

void Queue_c::init(
    void* FirstAddress, 
    void* LastAddress, 
    size_t QueueLength, 
    size_t ItemSize)
{
    /* The size of a single item in bytes */
    this->ItemSize = ItemSize;
    /* The maximum amount of items the queue can store */
    this->QueueLength = QueueLength;
    /* The current amount of items stored in the queue */
    this->ItemCount = 0;
    /* The first address of the queue memory */
    this->FirstAddress = FirstAddress;
    /* The last address of the queue memory */
    this->LastAddress = LastAddress;
    /* The head of the circular buffer */
    this->Head = FirstAddress;
    /* The tail of the circular buffer */
    this->Tail = FirstAddress;
    
    /* Set the queue memory to 0 */
    memset(
        this->Head, 
        0, 
        QueueLength * ItemSize);  
}

void* Queue_c::allocateItem(void)
{    
    /* Is there space left in the queue? */
    if(this->ItemCount >= this->QueueLength)
    {        
        return nullptr;
    }

    /* The head points to the next free queue item */
    void *Item = this->Head;

    /* Update the head */
    if(this->Head == this->LastAddress)
    {
        /* At the end of the circular buffer the head jumps back to the first address */
        this->Head = this->FirstAddress;
    }
    else
    {
        /* Increase the head by the item size */
        this->Head = (void*)((uint8_t*)this->Head + this->ItemSize);
    }

    /* Increase the item count */
    this->ItemCount++;

    /* Return the pointer to the allocated item */
    return Item; 
}

bool Queue_c::pushItem(const void *Item)
{
    /* Allocate space for the new item */
    void *NewItem = this->allocateItem();

    /* Did the allocation go wrong? */
    if(NewItem == nullptr)
    {
        return false;
    } 

    /* Copy the item into the queue */
    memcpy(
        NewItem, 
        Item, 
        this->ItemSize);

    return true;  
}

bool Queue_c::popItem(void *Item)
{
    /* Are there items in the queue? */
    if(this->ItemCount == 0)
    {        
        return false;
    }

    /* The tail points to the oldest item */
    memcpy(
        Item, 
        this->Tail, 
        this->ItemSize);

    /* Update the tail */
    if(this->Tail == this->LastAddress)
    {
        /* At the end of the circular buffer the tail jumps back to the first address */
        this->Tail = this->FirstAddress;
    }
    else
    {
        /* Increase the tail by the item size */
        this->Tail = (void*)((uint8_t*)this->Tail + this->ItemSize);
    }

    /* Decrease the item count */
    this->ItemCount--;

    return true;   
}