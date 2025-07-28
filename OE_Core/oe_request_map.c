/**
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#include "oe_request_map.h"
#include <string.h>

/**
 * @brief Place a handler in the request map.
 *
 * @param Node The pointer to the map node that will store the handler.
 *
 * @param RequestHandler The message handler to be placed.
 */
static inline void OE_RequestMap_placeHandler(
    OE_RequestMapNode_t *Node,
    OE_MessageHandler_t RequestHandler);

/**
 * @brief Remove a handler from the request map.
 *
 * @param Node The pointer to the map node that will delete the handler.
 *
 * @param RequestHandler The message handler to be removed.
 */
static inline void OE_RequestMap_removeHandler(
    OE_RequestMapNode_t *Node,
    OE_MessageHandler_t RequestHandler);

/**
 * @brief Check if a request handler is already registered in a map node.
 * 
 * @param Node The node to be checked.
 * @param RequestHandler The searched request handler. 
 * @return true If the request handler is registered.
 * @return false If the request handler is not registered.
 */
static inline bool OE_RequestMap_handlerRegistered(
    OE_RequestMapNode_t *Node,
    OE_MessageHandler_t RequestHandler);

void OE_RequestMap_staticInit(
    OE_RequestMap_t *RequestMap)
{
    for (size_t Node = 0; Node < OE_NUMBER_OF_REQUESTS; Node++)
    {
        for (size_t Handler = 0; Handler < OE_REQUEST_HANDLER_LIMIT; Handler++)
        {
            RequestMap->MapNodes[Node].RequestHandlers[Handler] = OE_NO_HANDLER;
        }

        RequestMap->MapNodes[Node].NumberOfHandlers = 0;
    }
}

OE_Error_t OE_RequestMap_registerHandlers(
    OE_RequestMap_t *RequestMap,
    OE_RequestID_t *RequestIDs,
    OE_MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests)
{
    size_t Count;
    OE_RequestMapNode_t *Node;
    
    /* Check if a handler is already registered and if the passed data ist valid. */
    for (Count = 0; Count < NumberOfRequests; Count++)
    {
        Node = &(RequestMap->MapNodes[RequestIDs[Count]]);
        if (OE_RequestMap_handlerRegistered(Node, RequestHandlers[Count]))
        {
            continue;
        }

        if (RequestIDs[Count] >= OE_NUMBER_OF_REQUESTS)
        {
            return OE_ERROR_REQUEST_ID_INVALID;
        }

        /* Can the map store another handler? */
        Node = &(RequestMap->MapNodes[RequestIDs[Count]]);

        if (Node->NumberOfHandlers >= OE_REQUEST_HANDLER_LIMIT)
        {
            /* There's no more space in the map */
            return OE_ERROR_HANDLER_LIMIT_REACHED;
        }

        /* is the handler valid? */
        if (RequestHandlers[Count] == NULL)
        {
            return OE_ERROR_PARAMETER_INVALID;
        }
    }

    for (Count = 0; Count < NumberOfRequests; Count++)
    {
        Node = &(RequestMap->MapNodes[RequestIDs[Count]]);

        OE_RequestMap_placeHandler(
            Node,
            RequestHandlers[Count]);
    }

    return OE_ERROR_NONE;
}

void OE_RequestMap_unregisterHandlers(
    OE_RequestMap_t *RequestMap,
    OE_RequestID_t *RequestIDs,
    OE_MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests)
{
    size_t Count;

    /* Check if the given request IDs are all valid. */
    for (Count = 0; Count < NumberOfRequests; Count++)
    {
        if (RequestIDs[Count] >= OE_NUMBER_OF_REQUESTS)
        {
            return;
        }
    }

    /* Remove the handlers from the request map. */
    for (Count = 0; Count < NumberOfRequests; Count++)
    {
        OE_RequestMap_removeHandler(
            &(RequestMap->MapNodes[RequestIDs[Count]]),
            RequestHandlers[Count]);
    }
}

OE_RequestMapNode_t* OE_RequestMap_getHandlers(
    OE_RequestMap_t *RequestMap,
    OE_RequestID_t RequestID)
{
    /* Give the caller access to the map memory */
    return &(RequestMap->MapNodes[RequestID]);
}

void OE_RequestMap_placeHandler(
    OE_RequestMapNode_t *Node,
    OE_MessageHandler_t EventHandler)
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

void OE_RequestMap_removeHandler(
    OE_RequestMapNode_t *Node,
    OE_MessageHandler_t EventHandler)
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
                sizeof(OE_MessageHandler_t) * (Node->NumberOfHandlers - Count - 1));

            /* Decrease the handler count. */
            Node->NumberOfHandlers--;

            /* The last handler in the array is reset. */
            Node->RequestHandlers[Node->NumberOfHandlers] = OE_NO_HANDLER;
            /* That's it. Easy, right? */
            return;
        }
    }
}

bool OE_RequestMap_handlerRegistered(
    OE_RequestMapNode_t *Node,
    OE_MessageHandler_t RequestHandler)
{
    /* This function assumes that the passed data is valid! */
    
    /* Search for the handler in the map node. */
    for (size_t Count = 0; Count < Node->NumberOfHandlers; Count++)
    {
        /* Did we find the handler? */
        if (Node->RequestHandlers[Count] == RequestHandler)
        {
            return true;
        }
    }

    return false;
}