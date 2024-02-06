/**
 * OpenEDOS Kernel v2.0.0
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

#include "include/message_queue.h"
#include "include/defines.h"
#include <string.h>

void MessageQueue_staticInit(
    MessageQueue_t *MessageQueue)
{
    memset(MessageQueue->Messages, 0, (MESSAGE_QUEUE_LENGTH + 1) * sizeof(Message_t));

    MessageQueue->Head = 0;
    MessageQueue->Tail = 0;
    MessageQueue->NumberOfMessages = 0;
}

bool MessageQueue_isFull(
    MessageQueue_t *MessageQueue)
{
    if (MessageQueue->NumberOfMessages >= MESSAGE_QUEUE_LENGTH)
    {
        return true;
    }

    return false;
}

Message_t* MessageQueue_allocateMessage(
    MessageQueue_t *MessageQueue)
{
    Message_t *Message;

    if (MessageQueue->NumberOfMessages >= MESSAGE_QUEUE_LENGTH)
    {
        return NULL;
    }

    Message = &(MessageQueue->Messages[MessageQueue->Head]);

    if (MessageQueue->Head == MESSAGE_QUEUE_LENGTH)
    {
        MessageQueue->Head = 0;
    }
    else
    {
        MessageQueue->Head++;
    }

    MessageQueue->NumberOfMessages++;

    return Message;
}

Message_t* MessageQueue_getMessage(
    MessageQueue_t *MessageQueue)
{
    Message_t *Message;

    if (MessageQueue->NumberOfMessages == 0)
    {
        return NULL;
    }

    Message = &(MessageQueue->Messages[MessageQueue->Tail]);

    if (MessageQueue->Tail == MESSAGE_QUEUE_LENGTH)
    {
        MessageQueue->Tail = 0;
    }
    else
    {
        MessageQueue->Tail++;
    }

    MessageQueue->NumberOfMessages--;

    return Message;
}