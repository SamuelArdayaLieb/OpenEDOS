/**
* OpenEDOS Kernel v1.0.0
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

#ifndef QUEUE_H
#define QUEUE_H

/**
 * This file is the class header of the queue class. A queue object always implements 
 * fifo behaviour. A queue doesn't actually allocate memory itself. Instead memory has
 * to be provided and passed to the queue object during initialization. This means 
 * that the queue class basically turns an array into a fifo queue. These fifos are
 * implemented as circular buffers. The kernel uses two different queues: the event
 * queue and the message queue. Queues can of course also be used by the application. 
 */

/* Needed for size_t */
#include <stddef.h>

class Queue_c
{
public:
    /**
     * @brief Initialize the queue.
     * 
     * The initialization simply sets all queue attributes to initial values.
     * As the queue class doesn't allocate memory itself, memory has to be provided.
     * This is done by passing a pointer to each the first address and the last 
     * address of the memory that can be used by the queue. 
     * 
     * @param FirstAddress This points to the first address of the memory the queue 
     * will be using.
     * 
     * @param LastAddress This points to the last address of the memory the queue
     * will be using.
     * 
     * @param QueueLength This is the length (max number of items) of the queue.
     * 
     * @param ItemSize This is the size of the items that this queue will store. 
     */
    void init(void* FirstAddress, void* LastAddress, size_t QueueLength, size_t ItemSize);

    /**
     * @brief Allocate memory inside the queue.
     * 
     * With this function queue memory can be allocated. When calling this function
     * the item count of the queue is immediately increased. Whatever data the 
     * caller of this function wants to place in the queue has to be copied to the
     * pointer that this function returns. Example usage is shown below:
     * 
     * //We want to store bytes (size = 1) in a queue
     * //This is our queue memory
     * uint8_t QueueMemory[6];
     * 
     * //This is our queue
     * Queue_c Queue;
     * 
     * //Initialize the queue
     * Queue.init(QueueMemory, 6, 1);
     * 
     * //This is the data we want to put into the queue
     * uint8_t FirstByte = 1;
     * uint8_t SecondByte = 2;
     * uint8_t ThirdBye = 3;
     * 
     * //Allocate space in the queue
     * uint8_t *pItem = Queue.allocateItem();
     * 
     * //Now the queue already holds one item
     * //We still have to fill it with our data though
     * if(pItem != nullptr)
     * {
     *     *pItem = FirstByte;    
     * }
     * 
     * pItem = Queue.allocateItem();
     * 
     * *pItem = SecondByte;
     * 
     * pItem = Queue.allocateItem();
     * 
     * *pItem = ThirdByte;
     * 
     * //Our data is now queued
     * 
     * @return void* If the queue is not full the returned pointer points to the
     * allocated queue memory. 
     * Otherwise a nullptr is returned.
     */
    void* allocateItem(void);

    /**
     * @brief Copy an item into the queue.
     * 
     * This function places an item into the queue. This is done by allocating 
     * queue memory and copying the provided item right away. If the item already
     * exists this simplifies queueing data. The function returns true if pushing
     * the item was possible and false otherwise. Example usage is shown below:
     * 
     * //We want to store bytes (size = 1) in a queue
     * //This is our queue memory
     * uint8_t QueueMemory[6];
     * 
     * //This is our queue
     * Queue_c Queue;
     * 
     * //Initialize the queue
     * Queue.init(QueueMemory, 6, 1);
     * 
     * //This is the data we want to put into the queue
     * uint8_t FirstByte = 1;
     * uint8_t SecondByte = 2;
     * uint8_t ThirdBye = 3;
     * 
     * //Push the data
     * Queue.pushItem(&FirstByte);
     * Queue.pushItem(&SecondByte);
     * Queue.pushItem(&ThirdByte);
     * 
     * //Our data is now queued
     * 
     * @param Item Pointer to the item that will be queued. The size of this item
     * has to equal the item size set during queue initialization.
     *  
     * @return true Returned if pushing the item was successful.
     * 
     * @return false Returned if the queue is full.
     */
    bool pushItem(const void *Item);

    /**
     * @brief Copy an item out of the queue.
     * 
     * This function is basically the opposite of pushItem(). Accoridng to fifo
     * behaviour, the oldest item in the queue is copied to the given pointer. The
     * item count of the queue is then decreased and the memory is freed. The function
     * returns false if the queue is empty, true otherwise. Example usage is shown 
     * below:
     * 
     * //We already have a queue that holds 3 bytes
     * //This is where we store the popped items
     * uint8_t FirstByte;
     * uint8_t SecondByte;
     * uint8_t ThirdByte;
     * 
     * Queue.popItem(&FirstByte);
     * Queue.popItem(&SecondByte);
     * Queue.popItem(&ThirdByte);
     * 
     * //The queue is now empty
     * 
     * @param Item Pointer to a variable that will store the popped item.
     *  
     * @return true Returned if popping an item was successful.  
     * 
     * @return false Returned if the queue is empty.
     */
    bool popItem(void *Item);
    
private:
    /* The size of the items stored in the queue in bytes */
    size_t ItemSize;

    /* The length (max number of items) of the queue */
    size_t QueueLength; 

    /* The number of items currently stored in the queue */
    size_t ItemCount;

    /* The first address of the queue memory (i.e. of the circular buffer) */
    void* FirstAddress;

    /* The last address of the queue memory (i.e. of the circular buffer)*/
    void* LastAddress;

    /* The current head of the circular buffer */
    void* Head;

    /* The current tail of the circular buffer */
    void* Tail;   
};

#endif