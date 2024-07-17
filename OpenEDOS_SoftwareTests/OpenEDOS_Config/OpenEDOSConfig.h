#ifndef OPEN_EDOS_CONFIG_H
#define OPEN_EDOS_CONFIG_H

/**
 * This macro defines the total amount of kernels in the application.
 */
#define NUMBER_OF_KERNELS 3

/**
 * This macro defines the maximum amount of request handlers per request.
 */
#define REQUEST_HANDLER_LIMIT 1

/**
 * This macro defines how many request messages the message queue of one kernel
 * can store.
 */
#define MESSAGE_QUEUE_LENGTH 1

/**
 * The message data size defines the number of bytes in the data field of a message.
 */
#define MESSAGE_DATA_SIZE 2

/**
 * If this macro is set to 1, a request limit is used. This means that the amount
 * of request messages in the message queue is controlled and limited by the kernel.
 * This is useful to prevent an overflow of requests.
 */
#define USE_REQUEST_LIMIT 1

/**
 * This macro defines how many requests of the same type (same ID) can be
 * stored in the messages queue at a time. This macro only has an effect if
 * USE_REQUEST_LIMIT is set to 1.
 */
#define REQUEST_LIMIT 1

/**
 * If this macro is set to 1, the OpenEDOS core will send requests
 * related to the system. For example, if a kernel enters its main routine,
 * this information is shared in a request.
 */
#define USE_SYSTEM_REQUESTS 1

#endif // OPEN_EDOS_CONFIG_H