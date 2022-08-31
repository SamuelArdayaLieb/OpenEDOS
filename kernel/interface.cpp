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

#include "include/interface.h"
#include <string.h>

void Interface_c::connect(
    KernelSwitch_c *KernelSwitch)
{
    this->KernelSwitch = KernelSwitch;
}

Error_t Interface_c::sendMessage(
    MessageHeader_t *MessageHeader, 
    void *Data[], 
    uint8_t DataSizes[], 
    uint8_t DataCount)
{
    /* Create a new message */
    Message_t Message;
    size_t CurrentDataSize = 0;
    
    /* Add all the data to the message */
    for(uint8_t Count = 0; Count < DataCount; Count++)
    {
        /* Check if there is enough space in the data field to store the new data */
        if(DataSizes[Count] > MESSAGE_DATA_SIZE - CurrentDataSize)
        {
            return ERROR_MESSAGE_DATA_FIELD_FULL;
        }

        /* Stack the new data on top of the existing data in the data field */
        memcpy(
            &Message.Data[CurrentDataSize], 
            Data[Count], 
            DataSizes[Count]);

        CurrentDataSize += DataSizes[Count];
    }

    /* Copy the message header to the new message */
    Message.MessageHeader = *MessageHeader;

    return this->KernelSwitch->sendMessage(
        &Message);
}