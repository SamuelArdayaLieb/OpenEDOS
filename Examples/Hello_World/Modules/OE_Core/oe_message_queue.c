/**
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#include "oe_message_queue.h"
#include "oe_defines.h"
#include <string.h>

void OE_MessageQueue_staticInit(
    OE_MessageQueue_t *MessageQueue)
{
    memset(MessageQueue->Messages, 0, (OE_MESSAGE_QUEUE_LENGTH + 1) * sizeof(OE_Message_t));

    MessageQueue->Head = 0;
    MessageQueue->Tail = 0;
    MessageQueue->NumberOfMessages = 0;
}

bool OE_MessageQueue_isFull(
    OE_MessageQueue_t *MessageQueue)
{
    if (MessageQueue->NumberOfMessages >= OE_MESSAGE_QUEUE_LENGTH)
    {
        return true;
    }

    return false;
}

OE_Message_t* OE_MessageQueue_allocateMessage(
    OE_MessageQueue_t *MessageQueue)
{
    OE_Message_t *Message;

    if (MessageQueue->NumberOfMessages >= OE_MESSAGE_QUEUE_LENGTH)
    {
        return NULL;
    }

    Message = &(MessageQueue->Messages[MessageQueue->Head]);

    if (MessageQueue->Head == OE_MESSAGE_QUEUE_LENGTH)
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

OE_Message_t* OE_MessageQueue_getMessage(
    OE_MessageQueue_t *MessageQueue)
{
    OE_Message_t *Message;

    if (MessageQueue->NumberOfMessages == 0)
    {
        return NULL;
    }

    Message = &(MessageQueue->Messages[MessageQueue->Tail]);

    if (MessageQueue->Tail == OE_MESSAGE_QUEUE_LENGTH)
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