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

#ifndef MESSAGE_H
#define MESSAGE_H

/**
 * This file is the class header of the message class. In an OpenEDOS application
 * messages serve as the main transport unit of information. They are used by 
 * modules for service requests and responses. They are in fact the only way a 
 * module is able to communicate with the rest of the system (through the kernel). 
 * The message is defined in two different ways. There is the type Message_t, which 
 * is a struct that holds the actual information a message carries. Then there is 
 * the class Message_c, which provides methods to manipulate that information but 
 * doesn't actually store any information itself (it just points to the memory). 
 * This way messages can be stored and queued more efficiently while convinient 
 * memory handling is still available. 
 */

/* Needed for various defines and type definitions */
#include "defines.h"

class Message_c
{
public:
    /**
     * @brief Initialize the message object.
     * 
     * The message object is initialized by passing a pointer to the 
     * message struct (i.e. memory that holds the information). The
     * struct can be empty (for example if a new message is created).
     * It can also be the prefilled struct from a previously built message.
     * 
     * @param MessageMemory Pointer to the message struct that this object
     * will be using. 
     */
    void init(Message_t *MessageMemory);

    /**
     * @brief Resets the memory pointed to by this object.
     * 
     * The memory (i. e. the message struct) that this object points to
     * is set to initial values upon calling this function.
     * 
     * @return Error_t An error is returned if
     * - the pointer to the struct is a nullptr.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t reset(void);

    /**
     * @brief Set the header of the message struct that this object points to. 
     * 
     * This function is typically used in the process of building
     * a new message. 
     * 
     * @param MessageHeader Pointer to the message header struct that
     * the message memory pointed to by this object will be set to.
     *   
     * @return Error_t An error is returned if
     * - the private pointer to the message memory is a nullptr..
     * - any of the parameters of the message header is invalid.
     * Otherwise ERROR_NONE is returned. 
     */
    Error_t setMessageHeader(MessageHeader_t *MessageHeader);

    /**
     * @brief Set the header of the message struct that this object points to.
     * 
     * This function is overloaded in case a MessageHeader_t is not yet created
     * when calling this function. In this case it is more convinient to pass
     * each parameter individually as the programmer doesn't have to know the
     * elements and the order of the elements by heart. 
     * 
     * @param ServiceID The ID of the service that this message will refer to
     * in a request or response.
     *  
     * @param MessageID The ID of this message. Using this ID can simplify the
     * handling of messages inside a module.
     *  
     * @param SourceID The ID of the module that sends this message. Modules have
     * to set this ID themselves as only they know their own ID.
     * 
     * @param DestinationID The ID of the module that this message will be sent to.
     * This is usually set by the kernel depending on the service ID. It is only 
     * set by the module that sends this message in case of a RETURN type message.
     * In this case the destination ID is just the source ID of the previously
     * received READ message.
     *  
     * @param MessageType The type of this message. There are three possible types
     * for messages that are passing through the system: MT_WRITE, MT_READ and 
     * MT_RETURN. The fourth option MT_NONE is only used during the reset of a message.
     *  
     * @param MessageDataSize The size of the data that is currently stored in the
     * message data field. This must never exceed the MESSAGE_DATA_SIZE macro which
     * defines the maximum size a data field can hold.
     * 
     * @return Error_t An error is returned if
     * - the private pointer to the message memory is a nullptr.
     * - any of the parameters is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t setMessageHeader(ServiceID_t ServiceID, MessageID_t MessageID, 
                             ModuleID_t SourceID, ModuleID_t DestinationID, 
                             MessageType_t MessageType, 
                             MessageDataSize_t MessageDataSize);

    /**
     * @brief Get the message header of the message struct that this object points to.
     * 
     * @param MessageHeader Pointer to a pointer to a message header struct. The 
     * message header this object points to will be assigned to the passed pointer.
     * 
     * @return Error_t An error is returned if
     * - the private pointer to the message memory is a nullptr.
     * Otherwise ERROR_NONE is returned. 
     */
    Error_t getMessageHeader(MessageHeader_t **MessageHeader);

    /**
     * @brief Add data to the data field of the message memory this object points to.
     * 
     * The data field of the message struct is a uint8_t array. This data type however
     * has no meaning. The array just serves as general purpose memory to store any
     * sort of information. Each service demands its own individual parameters. Hence 
     * the amount and types of variables stored in the data field depend solely on the 
     * service that is requested via the message. This function is provided so that
     * data of different types and sizes can be easily written into the data field.
     * In other words, this function is a wrapper for memcpy().  
     * 
     * @param Data This points to the data that is copied into the data field of the
     * message. As in functions like memcpy() a void pointer is used as the actual 
     * type of the data is not relevant.
     * 
     * @param DataSize The size of the copied data in bytes. (Can be determined with
     * sizeof()).
     * 
     * @return Error_t An error is returned if
     * - the private pointer to the message memory is a nullptr.
     * - copying the given data to the data field would exceed the max data size.
     * Otherwise ERROR_NONE is returned. 
     */
    Error_t addData(const void *Data, uint8_t DataSize);

    /**
     * @brief Retrieve data from the data field of the message memory this object
     * points to.
     * 
     * This function is basically the opposite of addData(). It can be used to copy
     * data of variable size from the message data field to a given void pointer. 
     * It is important to consider that this will retrieve data in reverse order of 
     * adding data. This is shown in an example:
     *  
     *  //This is our message 
     *  Message_c Message;
     * 
     *  //We want to add the following data to the message
     *  uint8_t CharData = 0;
     *  uint16_t ShortData = 1;
     *  float FloatData = 1.0;
     * 
     *  //Add the data
     *  Message.addData(&CharData, sizeof(CharData));
     *  Message.addData(&ShortData, sizeof(ShortData));
     *  Message.addData(&FloatData, sizeof(FloatData));
     * 
     *  //Now we want to retrieve the data
     *  //This has to be done in reverse order
     *  Message.retrieveData(&FloatData, sizeof(FloatData));
     *  Message.retrieveData(&ShortData, sizeof(ShortData));
     *  Message.retrieveData(&CharData, sizeof(CharData));
     * 
     * @param Data Points to a variable that the retrieved data will be copied to.
     * 
     * @param DataSize This is the size of the copied data in bytes. 
     * 
     * @return Error_t An error is returned if
     * - the private pointer to the message memory is a nullptr.
     * - copying the data from the data field would make the message data size 
     *   fall below 0.
     * Otherwise ERROR_NONE is returned. 
     */
    Error_t retrieveData(void* Data, uint8_t DataSize);

private:
    /* This points to the message struct holding the actual message information */
    Message_t *MessageMemory = nullptr;
};

#endif