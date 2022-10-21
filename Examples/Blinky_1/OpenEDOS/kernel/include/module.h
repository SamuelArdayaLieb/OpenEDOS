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

#ifndef MODULE_H
#define MODULE_H

/**
 * This file is the class header of the module class. In an OpenEDOS application,
 * modules encapsulate the funcionality that the sytem is supposed to provide 
 * according to the software requirements. What seperates modules in an 
 * OpenEDOS application from common C++ classes in usual software development is
 * the fact, that OpenEDOS modules are not related to each other in any way.
 * Any sort of object relationship that can be found in a typical UML class diagram
 * (such as association, aggregation, composition, etc.) is not needed and not wanted
 * for OpenEDOS modules. Instead, modules offer *services* and generate *events* 
 * according to their responsibility. In order to fulfill their responsibility and
 * get their job done, modules listen to events. They also might need the services of 
 * other modules. In this case the needed services are requested via messages. Messages
 * are sent to and received from the kernel switch. The kernel switch is responsible for
 * queueing and forwarding those messages to the right kernels. The kernels are then 
 * forwarding the messages to connected modules. This means that the only relationship 
 * a module has is the one to the kernel.        
 */

/* Needed for various defines and type definitions */
#include "defines.h"

/* Make the kernel class known to the compiler */
class Kernel_c;

/** 
 * Note that this class is mostly virtual. The concrete modules used in the 
 * application will inherit from this class.
 */
class Module_c
{
    /**
     * Encapsulation is strictly enforced in modules. This is why *everything*
     * a module has is private. The only allowed relationship is the one to the kernel.
     * That's why the kernel class is a friend.
     */
    friend class Kernel_c;

protected:
    /**
     * @brief Connect a module to the kernel.
     * 
     * In this function the module is supposed to subscribe to events and offer services
     * according to its responsibility. 
     * 
     * @return Error_t An error is returned if
     * - a function call inside this function returns an error. 
     * Otherwise ERROR_NONE is returned.
     */
    virtual Error_t connect(
        Kernel_c *Kernel) = 0;

    /**
     * @brief Initialize the module.
     * 
     * This function is called by the kernel upon initializing the system.
     * It should be used by the concrete module to initialize memory, hardware
     * periphery or anything else that is needed to make the module ready for
     * operation.
     * 
     * @return Error_t An error is returned if
     * - anything goes wrong during initializing the module.
     * Otherwise ERROR_NONE is returned.
     */
    virtual Error_t init(void) = 0;

    /**
     * @brief Start the operation of the module.
     * 
     * Sometimes certain actions have to be taken by modules at the very beginning
     * of operation. This can be done inside this method. It is called by the kernel 
     * right before it enters its main routine.
     * 
     * This method defaults to NOP() if not needed/not inherited.
     */
    virtual void start(void);

    /**
     * While processing a message, the kernel takes a look at the message type.
     * There are three types: event, request and response. When forwarding the
     * message to a module, the associated interface method is chosen by the kernel.
     * 
     * If not inherited, the three handling methods default to NOP().
     */

    /**
     * @brief Let the module handle a new event.
     * 
     * If a module has subscribed to a specific event, the kernel will call this 
     * method everytime the said event occures. The reaction to the event by the 
     * concrete module should be implemented in this method.
     * 
     * @param Message The pointer to the message that transports the event. 
     */
    virtual void handleEvent(
        Message_t *Message);

    /**
     * @brief Let the module handle a new service request.
     * 
     * If a module offers a specific service, the kernel will call this 
     * method everytime said service is requested. The reaction to the request by the 
     * concrete module should be implemented in this method.
     * 
     * @param Message The pointer to the message that transports the request. 
     */
    virtual void handleRequest(
        Message_t *Message);

    /**
     * @brief Let the module handle a new service response.
     * 
     * If a module has requested a service which includes a response, the kernel will 
     * call this method if that response is sent. The reaction to the response by the 
     * concrete module should be implemented in this method.
     * 
     * @param Message The pointer to the message that transports the response. 
     */
    virtual void handleResponse(
        Message_t *Message);

    /**
     * @brief Set the address of a module.
     * 
     * When a module connects to a kernel, the kernel will provide and set a unique address
     * for the module.
     * 
     * @param ModuleAddress The address that this module will have during operation.
     */
    void setAddress(
        ModuleAddress_t *ModuleAddress);

    /**
     * @brief Get the address of a module.
     * 
     * @return ModuleAddress_t A pointer to the address of the module.
     */
    ModuleAddress_t* getAddress(void);
    
    /* The address that the module will have during operation. It is set by the kernel. */
    ModuleAddress_t ModuleAddress;

    /* Connection to the kernel */
    Kernel_c *Kernel;
};

#endif//MODULE_H