/**
 * OpenEDOS Kernel v2.0.0
 *
 * Copyright (c) 2022-2024 Samuel Ardaya-Lieb
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

#include "include/request_map.h"
#include <string.h>

/**
 * @brief Place a handler in the request map.
 *
 * @param Node The pointer to the map node that will store the handler.
 *
 * @param RequestHandler The message handler to be placed.
 */
static inline void RequestMap_placeHandler(
    RequestMapNode_t *Node,
    MessageHandler_t RequestHandler);

/**
 * @brief Remove a handler from the request map.
 *
 * @param Node The pointer to the map node that will delete the handler.
 *
 * @param RequestHandler The message handler to be removed.
 */
static inline void RequestMap_removeHandler(
    RequestMapNode_t *Node,
    MessageHandler_t RequestHandler);

void RequestMap_staticInit(
    RequestMap_t *RequestMap)
{
    for (size_t Node = 0; Node < NUMBER_OF_REQUESTS; Node++)
    {
        for (size_t Handler = 0; Handler < REQUEST_HANDLER_LIMIT; Handler++)
        {
            RequestMap->MapNodes[Node].RequestHandlers[Handler] = NO_HANDLER;
        }

        RequestMap->MapNodes[Node].NumberOfHandlers = 0;
    }
}

Error_t RequestMap_registerHandlers(
    RequestMap_t *RequestMap,
    RequestID_t *RequestIDs,
    MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests)
{
    size_t Count;
    RequestMapNode_t *Node;

    /* First, check if all the passed request IDs and handlers are valid. */
    for (Count = 0; Count < NumberOfRequests; Count++)
    {
        if (RequestIDs[Count] >= NUMBER_OF_REQUESTS)
        {
            return ERROR_REQUEST_ID_INVALID;
        }

        /* Can the map store another handler? */
        Node = &(RequestMap->MapNodes[RequestIDs[Count]]);

        if (Node->NumberOfHandlers >= REQUEST_HANDLER_LIMIT)
        {
            /* There's no more space in the map */
            return ERROR_HANDLER_LIMIT_REACHED;
        }

        /* is the handler valid? */
        if (RequestHandlers[Count] == NULL)
        {
            return ERROR_PARAMETER_INVALID;
        }
    }

    for (Count = 0; Count < NumberOfRequests; Count++)
    {
        Node = &(RequestMap->MapNodes[RequestIDs[Count]]);

        RequestMap_placeHandler(
            Node,
            RequestHandlers[Count]);
    }

    return ERROR_NONE;
}

void RequestMap_unregisterHandlers(
    RequestMap_t *RequestMap,
    RequestID_t *RequestIDs,
    MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests)
{
    size_t Count;

    /* Check if the given request IDs are all valid. */
    for (Count = 0; Count < NumberOfRequests; Count++)
    {
        if (RequestIDs[Count] >= NUMBER_OF_REQUESTS)
        {
            return;
        }
    }

    /* Remove the handlers from the request map. */
    for (Count = 0; Count < NumberOfRequests; Count++)
    {
        RequestMap_removeHandler(
            &(RequestMap->MapNodes[RequestIDs[Count]]),
            RequestHandlers[Count]);
    }
}

RequestMapNode_t* RequestMap_getHandlers(
    RequestMap_t *RequestMap,
    RequestID_t RequestID)
{
    /* Give the caller access to the map memory */
    return &(RequestMap->MapNodes[RequestID]);
}

void RequestMap_placeHandler(
    RequestMapNode_t *Node,
    MessageHandler_t EventHandler)
{
    /* This function assumes that the passed data is valid! */

    /* Check if the handler is already placed */
    for (size_t Count = 0; Count < Node->NumberOfHandlers; Count++)
    {
        if (Node->RequestHandlers[Count] == EventHandler)
        {
            /* The handler is already registered. */
            return;
        }
    }

    /* Append the handler to the map node. */
    Node->RequestHandlers[Node->NumberOfHandlers] = EventHandler;

    /* Increase the number of handlers. */
    Node->NumberOfHandlers++;
}

void RequestMap_removeHandler(
    RequestMapNode_t *Node,
    MessageHandler_t EventHandler)
{
    /* This function assumes that the passed data is valid! */

    /* Search for the handler to be removed in the map node. */
    for (size_t Count = 0; Count < Node->NumberOfHandlers; Count++)
    {
        /* Did we find the handler? */
        if (Node->RequestHandlers[Count] == EventHandler)
        {
            /**
             * To remove the handler from the map node array, we simply move down the
             * rest of the array by one element so that the handler gets overwritten.
             * This way the array will not get fragmented. To make this more clear it
             * is illustrated below:
             *
             *   ____0__________1__________2__________3___________4_____________
             *  |          |          |          |          |            |
             *  | handler  | handler  | handler  | handler  | NO_HANDLER | ...
             *  |__________|__________|__________|__________|____________|______
             *              ´|`         .                .
             *           We want to     .                .
             *           delete this    .                .
             *           handler.       .                .
             *                         /|\              /|\
             *                          |----------------|
             *                          | So we just move
             *              /|\         | the rest of the array...
             *               |__________|
             *              ...here,
             *              overwriting the handler
             *              we want to delete.
             */
            memmove(
                &(Node->RequestHandlers[Count]),
                &(Node->RequestHandlers[Count + 1]),
                sizeof(MessageHandler_t) * (Node->NumberOfHandlers - Count - 1));

            /* Decrease the handler count. */
            Node->NumberOfHandlers--;

            /* The last handler in the array is reset. */
            Node->RequestHandlers[Node->NumberOfHandlers] = NO_HANDLER;

            /* That's it. Easy, right? */
            return;
        }
    }
}