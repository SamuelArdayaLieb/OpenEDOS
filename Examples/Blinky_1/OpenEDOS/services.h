/**
* OpenEDOS Kernel v1.1.0
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

#ifndef SERVICES_H
#define SERVICES_H

/**
 * This file is used to create a unique ID for each service that is offered by a 
 * module. To create the ID, simply list the service in the following enum. All 
 * services have to be listed here.
 */

enum ServiceIDs_e {
    /**
     * List your services here by giving them a meaningful name.
     * For example:
     * SID_MyCoolService,
     */

    /* GPIO Driver */
    SID_Toggle_GPIO_Output,

    /* Timer Driver */
    SID_Start_Timer,

    /**
     * The LAST element in this enum MUST be "NUMBER_OF_SERVICES", as it stores the total
     * number of services.
     */
    NUMBER_OF_SERVICES
};

#endif