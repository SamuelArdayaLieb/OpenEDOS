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

#ifndef DEFINES_H
#define DEFINES_H

/**
 * In this file you can find some general defines and type definitions that the
 * system uses.
 */

/* Application specific configurations */
#include "../../OpenEDOSConfig.h"

/* Enum of the request IDs and the total number of requests */
#include "../../requests.h"

/* Hardware/RTOS specific functions */
#include "../../port.h"

/* Needed for size_t */
#include <stddef.h>

/* Needed for type definitions */
#include <stdint.h>

/* Needed for booleans */
#include <stdbool.h>

/* Check if definitions are correct */
#ifndef NUMBER_OF_KERNELS
#error Missing definition: NUMBER_OF_KERNELS. Please define the number of kernels you want to use in the OpenEDOSConfig header.
#endif

#ifndef REQUEST_HANDLER_LIMIT
#error Missing definition: REQUEST_HANDLER_LIMIT. Please define the maximum amount of handlers per request in the OpenEDOSConfig header.
#endif

#ifndef MESSAGE_DATA_SIZE
#error Missing definition: MESSAGE_DATA_SIZE. Please define the size of the message data field in the OpenEDOSConfig header.
#endif

#if USE_REQUEST_LIMIT
#ifndef REQUEST_LIMIT
#error Missing definition: REQUEST_LIMIT. Please define the maximum amount of requests in the OpenEDOSConfig header.
#endif
#endif

#if MESSAGE_QUEUE_LENGTH < 1
#error Invalid definition: MESSAGE_QUEUE_LENGTH. Please make sure that MESSAGE_QUEUE_LENGTH is at least 1.
#endif

#if NUMBER_OF_REQUESTS < 0
#error Invalid definition: NUMBER_OF_REQUESTS. Please make sure that NUMBER_OF_REQUESTS is the last entry of the enum in the requests header.
#endif

/**
 * A kernel ID type is defined depending on the number of kernels.
 */
#if NUMBER_OF_KERNELS >= 0xFF
typedef uint16_t KernelID_t;
#define NO_KERNEL 0xFFFF
#else
typedef uint8_t KernelID_t;
#define NO_KERNEL 0xFF
#endif

/**
 * Kernel_t is a struct that holds the data of a kernel.
 */
typedef struct Kernel_s Kernel_t;

/**
 * KernelSwitch_t is a struct that holds the data of the kernel switch.
 */
typedef struct KernelSwitch_s KernelSwitch_t;

/**
 * The struct Message_t holds the data of a request message.
 */
typedef struct Message_s Message_t;

/* A function pointer for message handlers. */
typedef void (*MessageHandler_t)();

#define NO_HANDLER NULL

/**
 * The message ID may be used by application logic to identify messages.
 * It is not used by the kernel.
 */
typedef uint8_t MessageID_t;

/* A data type to store message information is defined. */
typedef uint8_t MessageInformation_t;

/**
 * @note Each message is either a request or a response.
 * The system handles these types differently.
 *
 * MESSAGE_TYPE_REQUEST: This type is used to send a request.
 * In a request message, the request ID of the message is used by the kernel switch
 * to forward the message to the corresponding kernels. The kernels then use the
 * request ID to read the corresponding message handlers from the request map.
 * The payload is interpreted as the associated request parameters.
 *
 * If MESSAGE_TYPE_REQUEST is not set, the message is handled as a response.
 * A response message is always a reaction to a request. Unlike for requests,
 * the corresponding message handler is not read from a map, but is passed in the
 * request message with the kernel ID of the original requester.
 * The payload is interpreted as the associated response parameters.
 *
 * The message type is stored in the first bit of the message information byte in
 * the message header.
 */
#define MESSAGE_TYPE_REQUEST 0b01

/**
 * @note Some messages transport parameters in their data field and some
 * don't. In order to distinguish between these two cases, a bit is set
 * in the message information.
 *
 * MESSAGE_DATA_EMPTY: There is no data passed in the message. In this
 * case the kernel will not pass a pointer to the data field to the handler
 * function.
 *
 * If there is relevant data in the message, MESSAGE_DATA_EMPTY is not set.
 * In this case the kernel will pass a pointer to the data field to the handler
 * function.
 */
#define MESSAGE_DATA_EMPTY 0b10

/**
 * The message header is used to store the general message data. The kernel switch
 * uses the header to forward messages to kernels. The kernels then forward those messages
 * to message handlers.
 */
typedef struct MessageHeader_s
{
    /**
     * The message information contains the message type 
     * and information about the data field. Additional information might be added in the future.
     */
    MessageInformation_t Information;

    /**
     * The ID of the request that is transported in this message.
     * The kernel switch and kernel use this ID to identify the message handlers.
     */
    RequestID_t RequestID;

    /**
     * The response handler is needed for requests which expect a response.
     * If a response is sent, it is handled by this message handler.
     */
    MessageHandler_t ResponseHandler;

    /**
     * This is the ID of the kernel from which a request is sent. 
     * The response handler is executed in this kernel.
     */
    KernelID_t KernelID;

    /**
     * The message ID might be useful for application logic.
     * This ID is optional and not used by the kernel.
     */
    MessageID_t MessageID;

} MessageHeader_t;

/**
 * A struct is used to store the data of a message. It consists of the
 * message header and the data field.
 */
struct Message_s
{
    /* The header holds the general message data. */
    MessageHeader_t Header;

    /* The data field holds the request/response parameters. */
    uint8_t Data[MESSAGE_DATA_SIZE];
};

#if USE_REQUEST_LIMIT
/**
 * To limit the amount of queued request messages at a time the kernel switch works with
 * a request register. This register consists of the following entries.
 */
typedef struct RequestRegisterEntry_s
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

} RequestRegisterEntry_t;
#endif // USE_REQUEST_LIMIT

/**
 * There are a couple of different errors that can occure during kernel operation.
 * These errors are listed here.
 */
typedef enum Error_e
{
    /*No error*/
    ERROR_NONE,
    /*Errors related to memory management*/
    ERROR_KERNEL_LIMIT_REACHED,
    ERROR_HANDLER_LIMIT_REACHED,
/*Errors related to request allocation*/
#if USE_REQUEST_LIMIT
    ERROR_REQUEST_LIMIT_REACHED,
#endif // USE_REQUEST_LIMIT
    ERROR_MESSAGE_QUEUE_FULL,
    /*Errors related to invalid IDs*/
    ERROR_REQUEST_ID_INVALID,
    ERROR_KERNEL_ID_INVALID,
    /*General purpose errors*/
    ERROR_PARAMETER_INVALID,
    /**
     * The last element in this enum MUST be "NUMBER_OF_ERRORS", as it stores the total
     * number of errors.
     */
    NUMBER_OF_ERRORS
} Error_t;

#endif // DEFINES_H