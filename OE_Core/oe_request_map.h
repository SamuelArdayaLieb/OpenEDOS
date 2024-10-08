/**
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#ifndef OE_REQUEST_MAP_H
#define OE_REQUEST_MAP_H

/**
 * @brief This is the header file of the request map. The request map is used by a
 * kernel to store the message handlers for each request. When a request is sent,
 * the kernel gets the list of message handlers using the request ID. The request
 * map is a two dimensional array where each row represents a request ID and stores
 * the corresponding handlers. Additionally, map nodes are used for easier access
 * to the map. A map node consists of a pointer to the row in the request map
 * (which is an array of handlers) and the number of currently registered handlers.
 *
 * To make this more clear the request map is illustrated below:
 *
 *  Array index         Array of                        Two dimensional array
 *  = request ID        map nodes                       of message handlers
 *      |                  |                                      |                 REQUEST_HANDLER_
 *      v         _________v__________                _____0______v______1_____ _____ __LIMIT - 1_
 *     | |       |*RequestHandlers    |--points to-->|            |            |     |            |
 *     |0|       |NumberOfHandlers: 1 |              |  handler   | NO_HANDLER | ... | NO_HANDLER |
 *     |_|       |____________________|              |____________|____________|_____|____________|
 *     | |       |*RequestHandlers    |--points to-->|            |            |     |            |
 *     |1|       |NumberOfHandlers: 2 |              |  handler   |   handler  | ... | NO_HANDLER |
 *     |_|       |____________________|              |____________|____________|_____|____________|
 *      .                  .                               .            .                  .
 *      .                  .                               .            .                  .
 *      .                  .                               .            .                  .
 *      _         ____________________                ____________ ____________ _____ ____________
 *     | |       |*RequestHandlers    |--points to-->|            |            |     |            |
 *     |N|       |NumberOfHandlers: 0 |              | NO_HANDLER | NO_HANDLER | ... | NO_HANDLER |
 *     |_|       |____________________|              |____________|____________|_____|____________|
 *
 *      N =
 *  OE_NUMBER_OF_REQUESTS - 1
 */

#include "oe_defines.h"

typedef struct OE_RequestMapNode_s
{
    /* This is one row of the map memory. */
    OE_MessageHandler_t RequestHandlers[OE_REQUEST_HANDLER_LIMIT];
    /* This holds the number of currently registered handlers. */
    size_t NumberOfHandlers;
} OE_RequestMapNode_t;

typedef struct OE_RequestMap_s
{
    /* We need one map node for each request. */
    OE_RequestMapNode_t MapNodes[OE_NUMBER_OF_REQUESTS];
} OE_RequestMap_t;

/**
 * @brief Initialize the request map statically.
 *
 * @param RequestMap The pointer to the request map to be initialized.
 */
void OE_RequestMap_staticInit(
    OE_RequestMap_t *RequestMap);

/**
 * @brief Register a list of handlers for a list of request IDs.
 *
 * The handlers to be registered are placed in the request map.
 * The number of request IDs and the number of request handlers have to match and
 * are passed to the function. The first handler will be registered for the first
 * request ID, the second handler for the second ID and so on. Trying to register
 * a handler that is already registered does not have any effect.
 *
 * @param RequestMap Pointer to the request map that will store the handlers.
 *
 * @param RequestIDs Pointer to the array of request IDs.
 *
 * @param RequestHandlers Pointer to the array of message handlers to be registered.
 *
 * @param NumberOfHandlers The number of request IDs and handlers.
 *
 * @return OE_Error_t An error is returned if
 * - one of the request IDs is invalid.
 * - one of the message handlers is invalid.
 * - there is no space left in the request map for one of the request IDs.
 * Otherwise OE_ERROR_NONE is returned.
 */
OE_Error_t OE_RequestMap_registerHandlers(
    OE_RequestMap_t *RequestMap,
    OE_RequestID_t *RequestIDs,
    OE_MessageHandler_t *RequestHandlers,
    size_t NumberOfHandlers);

/**
 * @brief Unregister a list of handlers for a list of request IDs.
 *
 * The handlers to be unregistered will be removed from the request map.
 * The number of request IDs and the number of request handlers have to match and
 * are passed to the function. The first handler will be unregistered for the first
 * request ID, the second handler for the second ID and so on. Trying to unregister
 * a handler that is not registered does not have any effect.
 *
 * @param RequestMap Pointer to the request map that will remove the handlers.
 *
 * @param RequestIDs Pointer to the array of request IDs.
 *
 * @param RequestHandlers Pointer to the array of message handlers to be unregistered.
 *
 * @param NumberOfHandlers The number of request IDs and handlers.
 */
void OE_RequestMap_unregisterHandlers(
    OE_RequestMap_t *RequestMap,
    OE_RequestID_t *RequestIDs,
    OE_MessageHandler_t *RequestHandlers,
    size_t NumberOfHandlers);

/**
 * @brief Get the map node associated to a given request ID.
 *
 * @param RequestMap Pointer to the request map.
 *
 * @param RequestID The request ID of the handlers to be read from the map.
 *
 * @return OE_RequestMapNode_t* A pointer to the map node is returned.
 * If the given request ID is invalid, NULL is returned.
 */
OE_RequestMapNode_t* OE_RequestMap_getHandlers(
    OE_RequestMap_t *RequestMap,
    OE_RequestID_t RequestID);

#endif // OE_REQUEST_MAP_H