/**
 * OpenEDOS, (c) 2022-2025 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#ifndef OE_CONFIG_H
#define OE_CONFIG_H

/**
 * This macro defines the total amount of kernels (threads) in the application.
 */
#define OE_NUMBER_OF_KERNELS 1

/**
 * This macro defines the maximum amount of request handlers (n_requests * n_request_handlers).
 */
#define OE_REQUEST_HANDLER_LIMIT 1

/**
 * This macro defines how many request messages the message queue of one kernel
 * can store.
 */
#define OE_MESSAGE_QUEUE_LENGTH 1

/**
 * The message data size defines the number of bytes in the data field of a message.
 * This amount of bytes is being copied with each request message.
 */
#define OE_MESSAGE_DATA_SIZE 0

/**
 * If this macro is set to 1, a request limit is used. This means that the amount
 * of request messages in the message queue is controlled and limited by the kernel.
 * This is useful to prevent an overflow of requests.
 */
#define OE_USE_REQUEST_LIMIT 0

/**
 * This macro defines how many requests of the same type (same ID) can be
 * stored in the messages queue at a time. This macro only has an effect if
 * OE_USE_REQUEST_LIMIT is set to 1.
 */
#define OE_REQUEST_LIMIT 0

/**
 * If this macro is set to 1, the OpenEDOS Core will send requests
 * related to the system. For example, if a kernel enters its main routine,
 * this information is shared in a request.
 */
#define OE_USE_SYSTEM_REQUESTS 1

#endif // OE_CONFIG_H