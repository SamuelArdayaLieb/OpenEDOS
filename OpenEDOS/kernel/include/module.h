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
 * are sent to and received from the kernel. The kernel is responsible for forwarding 
 * those messages to the right modules. This means that the only relationship a module
 * has is the one to the kernel. This way the principle of loose coupling is pushed to 
 * extremes, leading to a high degree of changeability.        
 */

/* Needed for various defines and type definitions */
#include "defines.h"

/* Objects of the message class are used during updating the module */
#include "message.h"

/* Make the kernel class known to the compiler */
class Kernel_c;

/** 
 * Note that this class is mostly virtual. The concrete modules needed in the 
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
     * This function is part of the process of registrating a module at the kernel.
     * This process is started by calling the kernel's connectModule() function.
     * The kernel will then call this function. In this function the module is 
     * supposed to create its module registration and pass it to the kernel by
     * calling loadModule(). This function is a pure virtual function as the module
     * registration has to be created by each concrete module individually.
     * 
     * @return Error_t An error is returned if
     * - loading the module registration results in an error.
     * Otherwise ERROR_NONE is returned.
     */
    virtual Error_t connect(Kernel_c *Kernel) = 0;

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
     * @brief Inform the module about a new event.
     * 
     * If a module has been registered for ("is listening to") a specific event,
     * the kernel will call this function everytime this event occures. The 
     * reaction to the event by the concrete module should be implemented in this 
     * function. This can include sending new messages or generating other events.
     * 
     * @param Event Pointer to an event struct that holds the event ID and one or
     * more parameters of a recently occured event. 
     */
    virtual void update(Event_t *Event) = 0;

    /**
     * @brief Inform the module about a new message.
     * 
     * If a module has been registered for ("is offering") a specific service,
     * the kernel will forward all messages containing the ID of that service to
     * this module. This is done by callig this function. The fulfillment of the
     * service should be done in this function. This can include sending new 
     * messages or generating events.
     * 
     * @param Message Pointer to a message class that holds the message information.
     */
    virtual void update(Message_c *Message) = 0;

    /**
     * @brief Set the ID of a module.
     * 
     * This function is part of the module registration. When a module connects to
     * the kernel, the kernel will provide and set a unique ID for the module.
     * 
     * @param ModuleID The ID that this module will have during operation.
     */
    void setID(ModuleID_t ModuleID);
    
    /* The ID that the module will have during operation. It is set by the kernel. */
    ModuleID_t ModuleID;

    /* Connection to the kernel. This is necessary for the registration of the module */
    Kernel_c *Kernel;
};

#endif