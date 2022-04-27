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

#include "include/interface.h"
#include "include/message.h"

void Interface_c::connect(Kernel_c *Kernel)
{
    this->Kernel = Kernel;
}

Error_t Interface_c::sendMessage(MessageHeader_t *MessageHeader, void *Data[], 
                                 uint8_t DataSizes[], uint8_t DataCount)
{
    /* Create a new message */
    Message_c Message = this->Kernel->newMessage();

    Error_t Error = Message.setMessageHeader(MessageHeader); 

    /**
     * Check if setting the message header returned an error.
     * This is the case if the message queue is full or the header contains invalid data.
     */
    if(Error != ERROR_NONE)
    {
        return Error;
    }
    
    /* Add all the data to the message */
    for(uint8_t Count = 0; Count < DataCount; Count++)
    {
        Error = Message.addData(Data[Count], DataSizes[Count]);

        /* Could the data be added? */
        if(Error != ERROR_NONE)
        {
            Message.reset();
            
            return Error;
        }
    }

    return ERROR_NONE;
}

