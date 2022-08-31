#ifndef OPEN_EDOS_CONFIG_H
#define OPEN_EDOS_CONFIG_H

/**
 * The macro defines the total amount of kernels in the application.
 */
#define NUMBER_OF_KERNELS   

/**
 * This macro should be defined according to the total amount of modules
 * that each single kernel can manage.
 */
#define NUMBER_OF_MODULES    

/**
 * This macro defines how many messages the message queue of one kernel 
 * can store.
 */
#define MESSAGE_QUEUE_LENGTH 

/**
 * The message data size defines the size of the message data field in bytes.
 */
#define MESSAGE_DATA_SIZE     

/**
 * This macro defines how many event messages of the same type (same ID) can be
 * stored in the messages queue at a time.
 */
#define MAX_EVENT_COUNT      

#endif