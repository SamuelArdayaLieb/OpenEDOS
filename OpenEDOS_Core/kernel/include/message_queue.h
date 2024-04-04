/**
 * OpenEDOS Core v0.2.1
 *
 * Copyright (c) 2022-2024 Samuel Ardaya-Lieb
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

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

/**
 * This file is the header of the message queue, a FIFO queue used for sending
 * messages from the kernel switch to the kernels. The kernel switch holds one
 * message queue for each kernel. The message queue uses static memory allocation
 * and is implemented as a circular buffer. The macro MESSAGE_QUEUE_LENGTH defines
 * how many messages can be stored in the message queue. The memory however is one
 * message larger than that. This is done so that the head never overwrites the tail,
 * leaving the memory of the last read message protected until another message is
 * read.
 */

#include "defines.h"
#include <stddef.h>

typedef struct MessageQueue_s
{
    /* An array is used to store the messages in the queue. */
    Message_t Messages[MESSAGE_QUEUE_LENGTH + 1];
    /* The head stores the index of the next free message in the queue. */
    size_t Head;
    /* The tail stores the index of the oldest message in the queue. */
    size_t Tail;
    /* This holds the number of currently stored messages. */
    size_t NumberOfMessages;
} MessageQueue_t;

/**
 * @brief Initialize the message queue statically.
 *
 * @param MessageQueue The pointer to the message queue to be initialized.
 */
void MessageQueue_staticInit(
    MessageQueue_t *MessageQueue);

/**
 * @brief Checks if the message queue is full.
 *
 * @param MessageQueue The message queue to be checked.
 *
 * @return true The message queue is full.
 * @return false The message queue is not full.
 */
bool MessageQueue_isFull(
    MessageQueue_t *MessageQueue);

/**
 * @brief Allocate the next free message in the message queue.
 *
 * @param MessageQueue The pointer to the message queue.
 *
 * @return Message_t* The pointer to the next free message is returned.
 * Returns NULL if the queue is full.
 */
Message_t* MessageQueue_allocateMessage(
    MessageQueue_t *MessageQueue);

/**
 * @brief Get the oldest message in the message queue.
 *
 * @param MessageQueue The pointer to the message queue.
 *
 * @return Message_t* The pointer to the oldest message is returned.
 * Returns NULL if the queue is empty.
 */
Message_t* MessageQueue_getMessage(
    MessageQueue_t *MessageQueue);

#endif // MESSAGE_QUEUE_H