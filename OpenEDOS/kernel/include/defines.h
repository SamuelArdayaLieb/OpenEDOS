/**
* OpenEDOS Kernel v1.2.0
* 
* Copyright (c) 2022 Samuel Ardaya-Lieb
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

/* Enum of the event IDs and the total number of events */
#include "../../events.h"

/* Enum of the service IDs and the total number of services */
#include "../../services.h"

/* Hardware/RTOS specific functions */
#include "../../port.h"

/* Needed for size_t */
#include <stddef.h>

/* Needed for type definitions */
#include <stdint.h>

/* Check if definitions are correct */
#ifndef NUMBER_OF_KERNELS   
    #error Missing definition: NUMBER_OF_KERNELS. Please define the number of kernels you want to use in your application in the OpenEDOSConfig header.
#endif

#ifndef NUMBER_OF_MODULES   
    #error Missing definition: NUMBER_OF_MODULES. Please define the number of modules you want to use per kernel in the OpenEDOSConfig header.
#endif

#ifndef MESSAGE_QUEUE_LENGTH
    #error Missing definition: MESSAGE_QUEUE_LENGTH. Please define the length of the message queue you want to use in your application in the OpenEDOSConfig header.
#endif

#ifndef MESSAGE_DATA_SIZE
    #error Missing definition: MESSAGE_DATA_SIZE. Please define the size of the message data field you want to use in your application in the OpenEDOSConfig header.
#endif  

#ifndef MAX_EVENT_COUNT
    #error Missing definition: MAX_EVENT_COUNT. Please define the maximum amount of events you want to use in your application in the OpenEDOSConfig header.
#endif

#if NUMBER_OF_EVENTS < 0
    #error Invalid definition: NUMBER_OF_EVENTS. Please make sure that NUMBER_OF_EVENTS is the last entry of the EventIDs_e enum in the events header.
#endif

#if NUMBER_OF_SERVICES < 0
    #error Invalid definition: NUMBER_OF_SERVICES. Please make sure that NUMBER_OF_SERVICES is the last entry of the ServiceIDs_e enum in the services header.
#endif

/**
 * The data types used for ID variables depend on the total number of unique IDs.
 * The highest possible values that those variables can hold are used as "None" types. 
 */
#if NUMBER_OF_SERIVES >= 0xFF
    typedef uint16_t ServiceID_t;
    #define NO_SERVICE 0xFFFF
#else
    typedef uint8_t ServiceID_t;
    #define NO_SERVICE 0xFF
#endif

#if NUMBER_OF_EVENTS >= 0xFF
    typedef uint16_t EventID_t;
    #define NO_EVENT 0xFFFF
#else
    typedef uint8_t EventID_t;
    #define NO_EVENT 0xFF
#endif

/**
 * The content ID is used in the message header. It stores either an event ID or 
 * a service ID depending on the message type.
 */
#if NUMBER_OF_SERIVES >= 0xFF || NUMBER_OF_EVENTS >= 0xFF
    typedef uint16_t ContentID_t;
    #define NO_CONTENT 0xFFFF
#else
    typedef uint8_t ContentID_t;
    #define NO_CONTENT 0xFF
#endif

/** 
 * A module ID and a type to count modules are defined 
 * depending on the number of modules. 
 */
#if NUMBER_OF_MODULES >= 0xFF
    typedef uint16_t ModuleID_t;
    typedef uint16_t ModuleCount_t;
    #define NO_MODULE 0xFFFF
#else
    typedef uint8_t ModuleID_t;
    typedef uint8_t ModuleCount_t;
    #define NO_MODULE 0xFF
#endif

#define NO_DESTINATION NO_MODULE

/** 
 * A kernel ID and a type to count kernels are defined 
 * depending on the number of modules. 
 */
#if NUMBER_OF_KERNELS >= 0xFF
    typedef uint16_t KernelID_t;
    typedef uint16_t KernelCount_t;
    #define NO_KERNEL 0xFFFF
#else
    typedef uint8_t KernelID_t;
    typedef uint8_t KernelCount_t;
    #define NO_KERNEL 0xFF
#endif

/**
 * A common ID type is defined that is large enough to store
 * other IDs.
 */
#if NUMBER_OF_MODULES >= 0xFF || NUMBER_OF_KERNELS >= 0xFF
    typedef uint16_t Identifier_t;
    #define NO_ID 0xFFFF
#else
    typedef uint8_t Identifier_t;
    #define NO_ID 0xFF
#endif

/**
 * The module address is part of the message header.
 * It consists of a kernel ID and a module ID.
 */
typedef struct ModuleAddress_t {
    KernelID_t KernelID;
    ModuleID_t ModuleID;
} ModuleAddress_t;

#define NO_ADDRESS {NO_KERNEL, NO_MODULE}

/**
 * The message ID can be used by modules to identify messages. It is not used by 
 * the kernel in any way.
 */
typedef uint8_t MessageID_t;

/* A data type to store message information is defined. */
typedef uint8_t MessageInformation_t;

/**
 * Each message can be of one of three types.
 * A kernel handles each type differently.
 * 
 * MESSAGE_TYPE_EVENT: This type is used to transport an event. 
 * In an event message, the content ID of the message is interpreted as an event ID.
 * The payload is interpreted as the associated event parameters.
 * 
 * MESSAGE_TYPE_REQUEST: This type is used for service requests.
 * In a request message, the content ID of the message is interpreted as a service ID.
 * The payload is interpreted as the associated request parameters.
 * 
 * MESSAGE_TYPE_RESPONSE: This type is used for service responses.
 * In a response message, the content ID of the message is interpreted as a service ID.
 * The payload is interpreted as the associated response parameters.
 * 
 * The message type is stored in the first 2 bits of the message information byte in 
 * the message header.
 */
#define MESSAGE_TYPE_EVENT     0b00
#define MESSAGE_TYPE_REQUEST   0b01
#define MESSAGE_TYPE_RESPONSE  0b10
#define MESSAGE_TYPE_NONE      0b11
#define MESSAGE_TYPE_MASK      0b11

/**
 * The message header is used to store the basic message information. The kernel switch
 * uses the header to forward messages to kernels. The kernels then forward those messages
 * to modules. 
 */
typedef struct MessageHeader_t {
    /**
     * The address of the module that is receiving the message. This address is usually 
     * set by the kernel switch or kernel as modules don't know which other modules exist. 
     * It is only set by a module in a response type message, where the source address
     * of the previously received request message can be set as destination address.
     */
    ModuleAddress_t DestinationAddress;

    /**
     * The address of the module that sent the message. This is not used by
     * the kernel. It is used by modules to set the destination address in 
     * response messages. 
     */
    ModuleAddress_t SourceAddress;

    /**
     * The message information containing the message type.
     * Additional information might be added in the future.  
     */
    MessageInformation_t MessageInformation;

    /** 
     * The message ID that a module may use to handle the message.
     * This ID is optional and not used by the kernel.
     */
    MessageID_t MessageID;

    /** 
     * The ID of the service or event that is transported in this message. 
     * The kernel switch and kernel use this ID to identify the destination modules. 
     */
    ContentID_t ContentID;

} MessageHeader_t;

/**
 * A struct is used to store the information of a message. It consists of the 
 * message header and the data field.
 */
typedef struct Message_t {
    MessageHeader_t MessageHeader;
    uint8_t Data[MESSAGE_DATA_SIZE];
} Message_t;

/**
 * To limit the amount of queued event messages at a time the kernel switch works with 
 * an event register. This register consists of the following entries.
 */
typedef struct EventRegisterEntry_t {
    /** 
     * The current event count stores how many times a specific event is currently queued. 
     */
#if MAX_EVENT_COUNT >= 0xFF
    uint16_t CurrentEventCount;
#else
    uint8_t CurrentEventCount;
#endif

    /**
     * The max event count determines how many events of the same type can be queued at most.
     * This is useful to stop an external flooding of events which could stop the kernels
     * from processing other types of messages.  
     */
#if MAX_EVENT_COUNT >= 0xFF
    uint16_t MaxEventCount;
#else
    uint8_t MaxEventCount;
#endif

} EventRegisterEntry_t;

/**
 * There are a couple of different errors that can occure during kernel operation.
 * These errors are listed here.
 */
enum Error_e {
    /*No error*/
    ERROR_NONE,
    /*Errors related to memory management*/
    ERROR_NO_MEMORY_AVAILABLE,
    ERROR_NO_MAP_SPACE_AVAILABLE, 
    ERROR_NO_MODULE_SPACE_AVAILABLE,
    /*Errors related to module registration*/ 
    ERROR_MODULE_NOT_REGISTERED,
    ERROR_MODULE_ALREADY_REGISTERED,
    ERROR_SERVICE_ALREADY_REGISTERED,
    ERROR_SERVICE_NOT_REGISTERED,
    /*Errors related to event allocation*/
    ERROR_EVENT_REGISTER_FULL,
    ERROR_MESSAGE_QUEUE_FULL,
    /*Errors related to the message data field*/
    ERROR_MESSAGE_DATA_FIELD_FULL,
    ERROR_MESSAGE_DATA_FIELD_EMPTY,
    /*Errors related to invalid IDs*/
    ERROR_SERVICE_ID_INVALID,
    ERROR_EVENT_ID_INVALID,
    ERROR_MODULE_ID_INVALID,
    ERROR_KERNEL_ID_INVALID,
    /*General purpose errors*/
    ERROR_PARAMETER_INVALID,
    ERROR_ILLEGAL_ACTION,
    /**
     * The LAST element in this enum MUST be "NUMBER_OF_ERRORS", as it stores the total
     * number of errors.
     */
    NUMBER_OF_ERRORS
};

#if NUMBER_OF_ERRORS >= 0xFF
    typedef uint16_t Error_t;
#else
    typedef uint8_t Error_t;
#endif

#endif//DEFINES_H