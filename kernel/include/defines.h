/**
* OpenEDOS Kernel v1.0.0
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
 * kernel uses.
 */

/* Application specific configurations */
#include "../../OpenEDOSConfig.h"

/* Enum of the event IDs and the total number of events */
#include "../../events.h"

/* Enum of the service IDs and the total number of services */
#include "../../services.h"

/* Needed for size_t */
#include <stddef.h>

/* Needed for type definitions */
#include <stdint.h>

/* Check if definitions are correct */
#ifndef NUMBER_OF_MODULES   
    #error Missing definition: NUMBER_OF_MODULES. Please define the number of modules you want to use in your application in the OpenEDOSConfig header.
#endif

#ifndef MESSAGE_QUEUE_LENGTH
    #error Missing definition: MESSAGE_QUEUE_LENGTH. Please define the length of the message queue you want to use in your application in the OpenEDOSConfig header.
#endif

#ifndef MESSAGE_DATA_SIZE
    #error Missing definition: MESSAGE_DATA_SIZE. Please define the size of the message data field you want to use in your application in the OpenEDOSConfig header.
#endif  

#ifndef EVENT_QUEUE_LENGTH
    #error Missing definition: EVENT_QUEUE_LENGTH. Please define the length of the event queue you want to use in your application in the OpenEDOSConfig header.
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
 * Include the correct port header according to the port defined in the OpenEDOSConfig
 * file. If you provide your own port header just include it here.
 */
#ifdef PORT_MSP430
    #include "../port/MSP430/port.h"
#endif

#ifdef PORT_STM32
	#include "../port/STM32/port.h"
#endif

/**
 * The data types used for ID variables depend on the total number of unique IDs.
 * The highest possible values that those variables can hold are used as "None" types. 
 * These values depend on the actual data type.
 */
#if NUMBER_OF_SERIVES < 0xFF
    typedef uint8_t ServiceID_t;
    #define NO_SERVICE 0xFF
#else
    typedef uint16_t ServiceID_t;
    #define NO_SERVICE 0xFFFF
#endif

#if NUMBER_OF_EVENTS < 0xFF
    typedef uint8_t EventID_t;
    #define NO_EVENT 0xFF
#else
    typedef uint16_t EventID_t;
    #define NO_EVENT 0xFFFF
#endif

/* A type to count modules is defined depending on the number of modules */
#if NUMBER_OF_MODULES < 0xFF
    typedef uint8_t ModuleID_t;
    typedef uint8_t ModuleCount_t;
    #define NO_MODULE 0xFF
#else
    typedef uint16_t ModuleID_t;
    typedef uint16_t ModuleCount_t;
    #define NO_MODULE 0xFFFF
#endif

#define NO_DESTINATION NO_MODULE

/**
 * Each message can be of one of three types.
 * The WRITE type is used when no response to the message is expected. 
 * For example: setOutput()
 * The READ type is used when a response is expected.
 * For example: getInput()
 * The RETURN type is used for responses to READ type messages.
 * 
 * This means that WRITE messages are not answered but each READ message 
 * is answered by a RETURN message.
 * 
 * A NONE type is declared just for initializing new messages. Besides that
 * it is not used by the kernel.
 */
enum MessageType_e {
    MT_WRITE,
    MT_READ,
    MT_RETURN,
    MT_NONE
};

/* A data type to store the message type is defined. */
typedef uint8_t MessageType_t;

/**
 * The message ID can be used by modules to identify messages. If necessary, 
 * change the data type according to the amount of unique message IDs that your modules need.
 */
typedef uint8_t MessageID_t;

/**
 * The current size of the data stored in a message is also stored in its
 * own type. It is signed because of certain calculations that are done during
 * error checking.
 */
#if MESSAGE_DATA_SIZE <= 0x7F
    typedef int8_t MessageDataSize_t;
#else
    typedef int16_t MessageDataSize_t;
#endif

/**
 * The message header is used to store the basic message information. The kernel
 * uses the header to forward messages to modules. 
 */
typedef struct MessageHeader_t {
    /** 
     * The ID of the service that is requested via this message. The kernel
     * uses this ID to identify the destination module. 
     */
    ServiceID_t ServiceID;
    /** 
     * The message ID that the module may use to handle the message.
     * This ID is not used by the kernel.
     */
    MessageID_t MessageID;
    /**
     * The ID of the module that sent the message. This is not used by
     * the kernel. It is used by modules to set the destination ID in 
     * RETURN messages. 
     */
    ModuleID_t SourceID;
    /**
     * The ID of the module that will receive the message. This ID is usually 
     * set by the kernel as modules don't know which other modules exist. 
     * It is only set by a module in a RETURN type message, where the source ID
     * of the previously received READ message can be set as destination ID.
     */
    ModuleID_t DestinationID;
    /**
     * The message type as explained above.  
     * The message type is not important for the kernel. Like the message ID 
     * it serves for easier and faster handling of the message inside a module.
     */
    MessageType_t MessageType;
    /**
     * The current size of the data that is stored in the data field of a message.
     * It is important for methods that are adding or retrieving data to or from
     * the message.
     */
    MessageDataSize_t MessageDataSize;
} MessageHeader_t;

/**
 * A struct is used to store the information of a message. It consists of the 
 * message header and the data field.
 */
typedef struct Message_t {
    MessageHeader_t MessageHeader;
    uint8_t Data[MESSAGE_DATA_SIZE];
} Message_t;

/* Events can come with parameters. */
typedef BaseType_t EventParameter_t;

/**
 * Events are stored in a struct consisting of the event ID 
 * and two parameters. This should be sufficient for most events
 * but can be adjusted if necessary.
 */
typedef struct Event_t {
    EventID_t EventID;
    EventParameter_t Parameter1;
    EventParameter_t Parameter2;
} Event_t;

/**
 * To limit the amount of queued events at a time the kernel works with an event register. 
 * This register consists of the following entries.
 */
typedef struct EventRegisterEntry_t {
    /** 
     * The current event count stores how many times a specific event is currently
     * processed or queued. It is increased when a new event occures and decreased 
     * when the processing of an event is finished.
     */
    uint8_t CurrentEventCount;
    /**
     * The max event count determines how many events of the same type can be queued at most.
     * This is useful to stop an external flooding of events which could stop the kernel
     * from processing other types of events.  
     */
    uint8_t MaxEventCount;
} EventRegisterEntry_t;

/**
 * To register modules in the kernel, the following struct is used.
 * It is passed from the module to the kernel during the registration.
 */
typedef struct ModuleRegistration_t {
    /**
     * This pointer points to the array of service IDs that the module wants to offer.
     * The kernel will save this information and forward all requests of the listed services 
     * to the module. Each service ID can be registered only once (single responsibility). 
     */
    ServiceID_t *ServiceIDs;
    /**
     * The service ID count holds the number of services that the module wants to offer.
     */
    size_t ServiceIDCount;
    /**
     * This pointer points to the array of event IDs that the module wants to listen to.
     * The kernel will save this information and forward all listed events to the module. 
     * A specific event ID can be registered for different modules. 
     */
    EventID_t *EventIDs;
    /**
     * The event ID count holds the number of events that the module wants to listen to.
     */
    size_t EventIDCount;
} ModuleRegistration_t;

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
    ERROR_MODULE_ALREADY_REGISTERED,
    ERROR_SERVICE_ALREADY_REGISTERED,
    ERROR_SERVICE_NOT_REGISTERED,
    /*Errors related to event allocation*/
    ERROR_EVENT_REGISTER_FULL,
    ERROR_EVENT_REGISTER_EMPTY,
    ERROR_EVENT_QUEUE_FULL,
    /*Errors related to the message data field*/
    ERROR_MESSAGE_DATA_FIELD_FULL,
    ERROR_MESSAGE_DATA_FIELD_EMPTY,
    /*Errors related to invalid IDs*/
    ERROR_SERVICE_ID_INVALID,
    ERROR_EVENT_ID_INVALID,
    ERROR_MODULE_ID_INVALID,
    /*General purpose errors*/
    ERROR_PARAMETER_INVALID,
    /**
     * The LAST element in this enum MUST be "NUMBER_OF_ERRORS", as it stores the total
     * number of errors.
     */
    NUMBER_OF_ERRORS
};

#if NUMBER_OF_ERRORS < 0xFF
    typedef uint8_t Error_t;
#else
    typedef uint16_t Error_t;
#endif

#endif