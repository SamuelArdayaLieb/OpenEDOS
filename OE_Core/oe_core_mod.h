/**
 * This file was autogenerated with OpenEDOS v2.0.
 * Sections inside USER CODE BEGIN and USER CODE END will be left untouched 
 * when rerunning the code generation. Happy coding!
 * 
 * @file oe_core_mod.h
 * @author Samuel Ardaya-Lieb
 * @version 2.0
 * @date 2024-07-24
 * 
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

/* USER CODE FILE HEADER BEGIN */
/**
 * @brief This file describes the core. The core is responsible for
 * receiving messages and forwarding them to the correct kernels. It also checks
 * the request limit for each request. The core holds one message queue
 * for each kernel and sends messages addressed to a kernel to its queue.
 */
/* USER CODE FILE HEADER END */

#ifndef OE_CORE_MOD_H
#define OE_CORE_MOD_H

#include "oe_defines.h"
#include "oe_core_intf.h"

/* Includes, typedefs, globals, etc. */
/* USER CODE MODULE GLOBALS BEGIN */
#include "oe_message_queue.h"
/* USER CODE MODULE GLOBALS END */

//~~~~~~~~~~~~~~~~~~~~~~~~ Module definition ~~~~~~~~~~~~~~~~~~~~~~~~//

typedef struct module_OE_Core_s {
	/* The connection to the kernel. */
	OE_Kernel_t *Kernel;

	/* Module data. */
	/* USER CODE MODULE DATA BEGIN */
    /* Kernel connections */
    OE_Kernel_t *Kernels[OE_NUMBER_OF_KERNELS];
    size_t NumberOfKernels;

#if OE_USE_REQUEST_LIMIT
    /**
     * The request register serves to keep track of how many requests are queued at a time.
     * There is one register entry for each request ID.
     */
    OE_RequestRegisterEntry_t RequestRegisters[OE_NUMBER_OF_KERNELS][OE_NUMBER_OF_REQUESTS];
#endif // OE_USE_REQUEST_LIMIT

    /* Message queues */
    OE_MessageQueue_t MessageQueues[OE_NUMBER_OF_KERNELS];
	/* USER CODE MODULE DATA END */
} module_OE_Core_t;

//~~~~~~~~~~~~~~~~~~~~~~~~~~ Init prototype ~~~~~~~~~~~~~~~~~~~~~~~~~//

/**
 * @brief Initialize the module OE_Core.
 * 
 * This function registers the request handlers of the module
 * and connects the module to the kernel. It then calls 
 * the specific init function of the module.
 * 
 * @param OE_Core A pointer to the module to be initialized.
 * 
 * @param Kernel A pointer to the kernel to be connected.
 * 
 * @return OE_Error_t An error is returned if
 * - initializing the module results in an error.
 * Otherwise OE_ERROR_NONE is returned.
 */
OE_Error_t initModule_OE_Core(
	module_OE_Core_t *OE_Core,
	OE_Kernel_t *Kernel);

//~~~~~~~~~~~~~~~~~~~~~~~~~~ User prototypes ~~~~~~~~~~~~~~~~~~~~~~~~//

/* USER CODE MODULE PROTOTYPES BEGIN */
/**
 * @brief Connect a kernel to the core.
 *
 * @param Kernel Pointer to the kernel to be connected with the core.
 *
 * @return OE_Error_t An error is returned if
 * - the number of kernels has reached its limit.
 * Otherwise OE_ERROR_NONE is returned.
 */
OE_Error_t OE_Core_connectKernel(
    OE_Kernel_t *Kernel);

/**
 * @brief Send a request message to the core.
 *
 * This function sends a request message to the core.
 * The core checks which kernels receive the request.
 * It then allocates a message in each message queue and copies
 * the given data. A valid message header has to be provided.
 *
 * @param Header Pointer to the message header.
 *
 * @param Parameters Pointer to the paramters transported in the message.
 *
 * @return OE_Error_t An error is returned if
 * - the kernel ID is invalid.
 * - the request ID is invalid.
 * - the request register is full.
 * - the message queue is full.
 * Otherwise OE_ERROR_NONE is returned.
 */
OE_Error_t OE_Core_sendRequest(
    OE_MessageHeader_t *Header,
    const void *Parameters);

/**
 * @brief Send a response message to the core.
 *
 * This function sends a response message to the core.
 * The core allocates a message in the message queue of the
 * destination kernel and writes the the given data. A valid message
 * header has to be provided.
 *
 * @param Header Pointer to the message header.
 *
 * @param Parameters Pointer to the paramters transported in the message.
 *
 * @return OE_Error_t An error is returned if
 * - the kernel ID is invalid.
 * - the request ID is invalid.
 * - the response handler is invalid.
 * - the message queue is full.
 * Otherwise OE_ERROR_NONE is returned.
 */
OE_Error_t OE_Core_sendResponse(
    OE_MessageHeader_t *Header,
    const void *Parameters);

/**
 * @brief Get the next message from the core.
 *
 * This method is called inside the kernel main routine.
 * It is used to get the next message from the message queue.
 *
 * @param KernelID The ID of the kernel that calls the function.
 *
 * @return OE_Message_t* A pointer to the next message is returned.
 * If the message queue is empty, NULL is returned.
 */
OE_Message_t* OE_Core_getMessage(
    OE_KernelID_t KernelID);
/* USER CODE MODULE PROTOTYPES END */

#endif//OE_CORE_MOD_H