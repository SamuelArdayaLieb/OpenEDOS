#ifndef OPEN_EDOS_CONFIG_H
#define OPEN_EDOS_CONFIG_H

/**
 * The macro defines the total amount of kernels in the application.
 */
#define NUMBER_OF_KERNELS 3

/**
 * This macro should be defined according to the total amount of modules
 * that are part of your application.
 */
#define NUMBER_OF_MODULES 1

/**
 * This macro defines how many messages the message queue can store.
 */
#define MESSAGE_QUEUE_LENGTH 2

/**
 * The message data size defines the size of the message data field in bytes.
 */
#define MESSAGE_DATA_SIZE 6

/**
 * This macro defines how many messages of the same type (same ID) can be
 * stored in the event queue at a time.
 */
#define MAX_EVENT_COUNT 2

#endif
