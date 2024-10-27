/**
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#ifndef OE_MESSAGE_QUEUE_H
#define OE_MESSAGE_QUEUE_H

/**
 * @brief This file is the header of the message queue, a FIFO queue used for sending
 * messages from the core to the kernels. The core holds one message queue for each kernel. 
 * The message queue uses static memory allocation and is implemented as a circular buffer. 
 * The macro OE_MESSAGE_QUEUE_LENGTH defines how many messages can be stored in the message 
 * queue. The memory however is one message larger than that. This is done so that the head 
 * never overwrites the tail, leaving the memory of the last read message protected until 
 * another message is read.
 */

#include "oe_defines.h"
#include <stddef.h>

typedef struct OE_MessageQueue_s
{
    /* An array is used to store the messages in the queue. */
    OE_Message_t Messages[OE_MESSAGE_QUEUE_LENGTH + 1];
    /* The head stores the index of the next free message in the queue. */
    size_t Head;
    /* The tail stores the index of the oldest message in the queue. */
    size_t Tail;
    /* This holds the number of currently stored messages. */
    size_t NumberOfMessages;
} OE_MessageQueue_t;

/**
 * @brief Initialize the message queue statically.
 *
 * @param MessageQueue The pointer to the message queue to be initialized.
 */
void OE_MessageQueue_staticInit(
    OE_MessageQueue_t *MessageQueue);

/**
 * @brief Checks if the message queue is full.
 *
 * @param MessageQueue The message queue to be checked.
 *
 * @return true The message queue is full.
 * @return false The message queue is not full.
 */
bool OE_MessageQueue_isFull(
    OE_MessageQueue_t *MessageQueue);

/**
 * @brief Allocate the next free message in the message queue.
 *
 * @param MessageQueue The pointer to the message queue.
 *
 * @return OE_Message_t* The pointer to the next free message is returned.
 * Returns NULL if the queue is full.
 */
OE_Message_t* OE_MessageQueue_allocateMessage(
    OE_MessageQueue_t *MessageQueue);

/**
 * @brief Get the oldest message in the message queue.
 *
 * @param MessageQueue The pointer to the message queue.
 *
 * @return OE_Message_t* The pointer to the oldest message is returned.
 * Returns NULL if the queue is empty.
 */
OE_Message_t* OE_MessageQueue_getMessage(
    OE_MessageQueue_t *MessageQueue);

#endif // OE_MESSAGE_QUEUE_H