/**
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#ifndef OE_DEFINES_H
#define OE_DEFINES_H

/**
 * @brief In this file you can find some general defines and type definitions 
 * used by the OpenEDOS Core.
 */

/* Application specific configurations */
#include "oe_config.h"

/* Enum of the request IDs and the total number of requests */
#include "oe_requests.h"

/* Hardware/RTOS specific functions */
#include "oe_port.h"

/* Needed for size_t */
#include <stddef.h>

/* Needed for type definitions */
#include <stdint.h>

/* Needed for booleans */
#include <stdbool.h>

/* Check if definitions are correct */
#ifndef OE_NUMBER_OF_KERNELS
#error Missing definition: OE_NUMBER_OF_KERNELS. Please define the number of kernels you want to use in oe_config.h.
#endif

#ifndef OE_REQUEST_HANDLER_LIMIT
#error Missing definition: OE_REQUEST_HANDLER_LIMIT. Please define the maximum amount of handlers per request in oe_config.h.
#endif

#ifndef OE_MESSAGE_DATA_SIZE
#error Missing definition: OE_MESSAGE_DATA_SIZE. Please define the size of the message data field in oe_config.h.
#endif

#if OE_USE_REQUEST_LIMIT
#ifndef OE_REQUEST_LIMIT
#error Missing definition: OE_REQUEST_LIMIT. Please define the maximum amount of requests in oe_config.h.
#endif
#endif

#if OE_MESSAGE_QUEUE_LENGTH < 1
#error Invalid definition: OE_MESSAGE_QUEUE_LENGTH. Please make sure that OE_MESSAGE_QUEUE_LENGTH is at least 1.
#endif

#if OE_NUMBER_OF_REQUESTS < 0
#error Invalid definition: OE_NUMBER_OF_REQUESTS. Please make sure that OE_NUMBER_OF_REQUESTS is the last entry of the enum in oe_requests.h.
#endif

/**
 * A kernel ID type is defined depending on the number of kernels.
 */
#if OE_NUMBER_OF_KERNELS >= 0xFF
typedef uint16_t OE_KernelID_t;
#define OE_NO_KERNEL 0xFFFF
#else
typedef uint8_t OE_KernelID_t;
#define OE_NO_KERNEL 0xFF
#endif

/**
 * OE_Kernel_t is a struct that holds the data of a kernel.
 */
typedef struct OE_Kernel_s OE_Kernel_t;

/**
 * OE_Core_t is a struct that holds the data of the core.
 */
typedef struct module_OE_Core_s OE_Core_t;

/**
 * The struct OE_Message_t holds the data of a request message.
 */
typedef struct OE_Message_s OE_Message_t;

/* A function pointer for message handlers. */
typedef void (*OE_MessageHandler_t)();

#define OE_NO_HANDLER NULL

/* A data type to store message information is defined. */
typedef uint8_t OE_MessageInformation_t;

/**
 * @note Each message is either a request or a response.
 * The system handles these types differently.
 *
 * OE_MESSAGE_TYPE_REQUEST: This type is used to send a request.
 * In a request message, the request ID of the message is used by the core
 * to forward the message to the corresponding kernels. The kernels then use the
 * request ID to read the corresponding message handlers from the request map.
 * The payload is interpreted as the associated request parameters.
 *
 * If OE_MESSAGE_TYPE_REQUEST is not set, the message is handled as a response.
 * A response message is always a reaction to a request. Unlike for requests,
 * the corresponding message handler is not read from a map, but is passed in the
 * request message with the kernel ID of the original requester.
 * The payload is interpreted as the associated response parameters.
 *
 * The message type is stored in the first bit of the message information byte in
 * the message header.
 */
#define OE_MESSAGE_TYPE_REQUEST 0b01

/**
 * @note Some messages transport parameters in their data field and some
 * don't. In order to distinguish between these two cases, a bit is set
 * in the message information.
 *
 * OE_MESSAGE_DATA_EMPTY: There is no data passed in the message. In this
 * case the kernel will not pass a pointer to the data field to the handler
 * function.
 *
 * If there is relevant data in the message, OE_MESSAGE_DATA_EMPTY is not set.
 * In this case the kernel will pass a pointer to the data field to the handler
 * function.
 */
#define OE_MESSAGE_DATA_EMPTY 0b10

/**
 * The message header is used to store the general message data. The core
 * uses the header to forward messages to kernels. The kernels then forward those messages
 * to message handlers.
 */
typedef struct OE_MessageHeader_s
{
    /**
     * The message information contains the message type 
     * and information about the data field. Additional information might be added in the future.
     */
    OE_MessageInformation_t Information;

    /**
     * The ID of the request that is transported in this message.
     * The core and kernel use this ID to identify the message handlers.
     */
    OE_RequestID_t RequestID;

    /**
     * The response handler is needed for requests which expect a response.
     * If a response is sent, it is handled by this message handler.
     */
    OE_MessageHandler_t ResponseHandler;

    /**
     * This is the ID of the kernel from which a request is sent. 
     * The response handler is executed in this kernel.
     */
    OE_KernelID_t KernelID;
} OE_MessageHeader_t;

#define OE_EMPTY_HEADER (OE_MessageHeader_t){ \
    .Information=OE_MESSAGE_TYPE_REQUEST|OE_MESSAGE_DATA_EMPTY, \
    .KernelID=OE_NO_KERNEL, \
    .RequestID=OE_NUMBER_OF_REQUESTS, \
    .ResponseHandler=OE_NO_HANDLER}

/**
 * A struct is used to store the data of a message. It consists of the
 * message header and the data field.
 */
struct OE_Message_s
{
    /* The header holds the general message data. */
    OE_MessageHeader_t Header;

    /* The data field holds the request/response parameters. */
    uint8_t Data[OE_MESSAGE_DATA_SIZE];
};

#if OE_USE_REQUEST_LIMIT
/**
 * To limit the amount of queued request messages at a time the core works with
 * a request register. This register consists of the following entries.
 */
typedef struct OE_RequestRegisterEntry_s
{
    /**
     * The number of requests stores how many times a specific request is currently queued.
     */
    size_t NumberOfRequests;

    /**
     * The request limit determines how many requests of the same type can be queued at most.
     * This is useful to stop an external flooding of requests which could stop the kernels
     * from processing other types of messages.
     *
     * The request limit is currently defined globally be the macro REQUEST_LIMIT. This might
     * be changed in the future in order to set an individual limit for each request.
     */
    size_t RequestLimit;

} OE_RequestRegisterEntry_t;
#endif // OE_USE_REQUEST_LIMIT

/**
 * There are a couple of different errors that can occure during kernel operation.
 * These errors are listed here.
 */
typedef enum OE_Error_e
{
    /*No error*/
    OE_ERROR_NONE,
    /*Errors related to memory management*/
    OE_ERROR_KERNEL_LIMIT_REACHED,
    OE_ERROR_HANDLER_LIMIT_REACHED,
/*Errors related to request allocation*/
#if OE_USE_REQUEST_LIMIT
    OE_ERROR_REQUEST_LIMIT_REACHED,
#endif // OE_USE_REQUEST_LIMIT
    OE_ERROR_MESSAGE_QUEUE_FULL,
    /*Errors related to invalid IDs*/
    OE_ERROR_REQUEST_ID_INVALID,
    OE_ERROR_KERNEL_ID_INVALID,
    /*General purpose errors*/
    OE_ERROR_PARAMETER_INVALID,
    
    /**
     * The last element in this enum MUST be "OE_NUMBER_OF_ERRORS".
     */
    OE_NUMBER_OF_ERRORS
} OE_Error_t;

#endif // OE_DEFINES_H