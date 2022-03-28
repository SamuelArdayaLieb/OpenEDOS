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

#include "include/message.h"
#include <string.h>

void Message_c::init(Message_t *MessageMemory)
{
    this->MessageMemory = MessageMemory;
}

Error_t Message_c::reset(void)
{
    if(this->MessageMemory == nullptr)
    {
        return ERROR_NO_MEMORY_AVAILABLE;
    }

    /* Create a new message header with default values */
    MessageHeader_t MessageHeader = {};
    MessageHeader.ServiceID = NO_SERVICE;
    MessageHeader.MessageID = 0;
    MessageHeader.SourceID = NO_MODULE;
    MessageHeader.DestinationID = NO_MODULE;
    MessageHeader.MessageType = MT_NONE;
    MessageHeader.MessageDataSize = 0;

    /* Set the new message header */
    this->MessageMemory->MessageHeader = MessageHeader;

    /* Reset the data field */
    memset(this->MessageMemory->Data, 0, MESSAGE_DATA_SIZE);

    return ERROR_NONE;
}

Error_t Message_c::setMessageHeader(MessageHeader_t *MessageHeader)
{
    if(this->MessageMemory == nullptr)
    {
        return ERROR_NO_MEMORY_AVAILABLE;
    }

    /* Check if the service ID is valid */
    if(MessageHeader->ServiceID >= NUMBER_OF_SERVICES)
    {
        return ERROR_SERVICE_ID_INVALID;
    }

    /* Check if the source ID is valid */
    if(MessageHeader->SourceID >= NUMBER_OF_MODULES)
    {
        return ERROR_MODULE_ID_INVALID;
    }

    /* Check if the destination ID is valid. NO_MODULE is valid here */
    if((MessageHeader->DestinationID >= NUMBER_OF_MODULES) &&
       (MessageHeader->DestinationID != NO_MODULE))
    {
        return ERROR_MODULE_ID_INVALID;
    }

    if(MessageHeader->MessageDataSize > MESSAGE_DATA_SIZE)
    {
        return ERROR_PARAMETER_INVALID;
    }

    /* Assign the new message header to our message memory */
    this->MessageMemory->MessageHeader = *MessageHeader;

    return ERROR_NONE;
}

Error_t Message_c::setMessageHeader(ServiceID_t ServiceID, MessageID_t MessageID, 
                                    ModuleID_t SourceID, ModuleID_t DestinationID, 
                                    MessageType_t MessageType, 
                                    MessageDataSize_t MessageDataSize)
{
    if(this->MessageMemory == nullptr)
    {
        return ERROR_NO_MEMORY_AVAILABLE;
    } 

    /* Check if the service ID is valid */
    if(ServiceID >= NUMBER_OF_SERVICES)
    {
        return ERROR_SERVICE_ID_INVALID;
    }

    /* Check if the source ID is valid */
    if(SourceID >= NUMBER_OF_MODULES)
    {
        return ERROR_MODULE_ID_INVALID;
    }

    /* Check if the destination ID is valid. NO_MODULE is valid here */
    if((DestinationID >= NUMBER_OF_MODULES) &&
       (DestinationID != NO_MODULE))
    {
        return ERROR_MODULE_ID_INVALID;
    }

    if(MessageDataSize > MESSAGE_DATA_SIZE)
    {
        return ERROR_PARAMETER_INVALID;
    }

    /* Create the message header */
    MessageHeader_t MessageHeader = {};
    MessageHeader.ServiceID = ServiceID;
    MessageHeader.MessageID = MessageID;
    MessageHeader.SourceID = SourceID;
    MessageHeader.DestinationID = DestinationID;
    MessageHeader.MessageType = MessageType;
    MessageHeader.MessageDataSize = MessageDataSize;

    /* Assign the new message header to our message memory */
    this->MessageMemory->MessageHeader = MessageHeader;

    return ERROR_NONE;
}

Error_t Message_c::getMessageHeader(MessageHeader_t **MessageHeader)
{
    if(this->MessageMemory == nullptr)
    {
        return ERROR_NO_MEMORY_AVAILABLE;
    }

    /**
     * The caller of the function wants access to the message header.
     * In order to provide that, we don't have to copy the message header.
     * Giving them a pointer to the memory is sufficient.  
     */
    *MessageHeader = &this->MessageMemory->MessageHeader;

    return ERROR_NONE; 
}

Error_t Message_c::addData(const void *Data, uint8_t DataSize)
{
    if(this->MessageMemory == nullptr)
    {
        return ERROR_NO_MEMORY_AVAILABLE;
    }

    MessageDataSize_t CurrentDataSize = this->MessageMemory->MessageHeader.MessageDataSize;

    /* Check if there is enough space in the data field to store the new data */
    if(DataSize > MESSAGE_DATA_SIZE - CurrentDataSize)
    {
        return ERROR_MESSAGE_DATA_FIELD_FULL;
    }

    /* Stack the new data on top of the existing data in the data field */
    memcpy(&this->MessageMemory->Data[CurrentDataSize], Data, DataSize);

    /* Update the data size in the message */
    this->MessageMemory->MessageHeader.MessageDataSize += DataSize;

    return ERROR_NONE;    
}

Error_t Message_c::retrieveData(void* Data, uint8_t DataSize)
{
    if(this->MessageMemory == nullptr)
    {
        return ERROR_NO_MEMORY_AVAILABLE;
    }
    
    MessageDataSize_t CurrentDataSize = this->MessageMemory->MessageHeader.MessageDataSize;
    
    /* Check if there is enough data in the data field to retrive the given data size */
    if(CurrentDataSize - DataSize < 0)
    {
        return ERROR_MESSAGE_DATA_FIELD_EMPTY;
    }

    /* Retrieve data from the top of the data field */
    memcpy(Data, &this->MessageMemory->Data[CurrentDataSize - DataSize], DataSize);

    /* Update the data size in the message header */
    this->MessageMemory->MessageHeader.MessageDataSize -= DataSize;

    return ERROR_NONE;  
}