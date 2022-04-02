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

#ifndef INTERFACE_H
#define INTERFACE_H

/**
 * This file is the class header of the interface class. Interfaces offer methods 
 * for requesting services. There can be one interface per module, but several modules 
 * can also be grouped in one interface. The methods an interface offers are expressing 
 * via their function parameters which parameters a service expects. The same appplies 
 * to event interface methods. The interface has a private function that squashes any 
 * combination of parameters into one message data field as long as the total data size 
 * doesn't exceed its maximum. Interface methods should be offered for both writing and 
 * reading messages. The interface has a connection to the kernel that is used to send 
 * messages to the message queue. 
 */

/* Needed for various defines and type definitions */
#include "defines.h"

/* Interfaces are related to the kernel */
#include "kernel.h"


class Interface_c
{
public:
    /**
     * @brief Connect the interface to the kernel
     * 
     * @param Kernel Pointer to the kernel object
     */
    void connect(Kernel_c *Kernel);

private:
    /**
     * @brief Send a message to the kernel.
     * 
     * This function takes a message header and any amount of data of any data type 
     * and uses this to create and queue a new message.
     * Example usage is shown below:
     * 
     *  //This is a random service interface method
     *  bool intf_PinController::Service_setPinOutput(MessageID_t MID, ModuleID_t Source, 
     *                                                uint8_t Port, uint8_t Pin, 
     *                                                uint8_t Level)
     *  {
     *      MessageHeader_t MessageHeader = {
     *      SID_setPinOutput,
     *      MID,
     *      Source,
     *      NO_MODULE,
     *      MT_WRITE,
     *      (sizeof(Port) + sizeof(Pin) + sizeof(Level))
     *      };
     * 
     *      void *Data[] = {
     *          &Port,
     *          &Pin,
     *          &Level,
     *      };
     * 
     *      uint8_t DataSizes[] = {
     *          sizeof(Port),
     *          sizeof(Pin),
     *          sizeof(Level),
     *      };
     * 
     *      uint8_t DataCount = 3;
     * 
     *      this->sendMessage(&MessageHeader, Data, DataSizes, DataCount);
     *  }
     * 
     * @param MessageHeader Pointer to the message header.
     * 
     * @param Data Pointer to an array of void pointers, containing the pointers to
     * the parameters.
     * 
     * @param DataSizes Pointer to an array of data sizes, containing the sizes of
     * each parameter.
     * 
     * @param DataCount Number of parameters.
     * 
     * @return Error_t An error is returned if 
     * - sending the message was not successful.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t sendMessage(MessageHeader_t *MessageHeader, void *Data[], 
                        uint8_t DataSizes[], uint8_t DataCount);
    
    Kernel_c *Kernel;
};

#endif