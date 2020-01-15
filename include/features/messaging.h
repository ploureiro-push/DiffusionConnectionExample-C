#ifndef _diffusion_messaging_h_
#define _diffusion_messaging_h_ 1

/*
 * Copyright © 2014, 2018 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

/**
 * @file messaging.h
 *
 * Messaging feature.
 */

#include "session.h"
#include "misc/deprecate.h"

#include "datatypes.h"
#include "responder.h"
#include "features/messaging-control.h"

/**
 * @brief Callback for send_msg()
 *
 * @param session The current active session.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_send_msg_cb)(SESSION_T *session, void *context);

/**
 * @brief User-supplied handlers that may be invoked in response to a
 * send_msg() call.
 */
#define SEND_MSG_HANDLERS                       \
        on_send_msg_cb on_send;                 \
        ERROR_HANDLER_T on_error;               \
        DISCARD_HANDLER_T on_discard;

/**
 * @brief Structure supplied to a send_msg() call.
 */
typedef struct send_msg_params_s {
        SEND_MSG_HANDLERS
        /// The name of the path used when sending
        /// messages to Diffusion.
        char *topic_path;
        /// The message to be sent.
        BUF_T payload;
        /// Any headers to be sent.
        LIST_T *headers;
        /// The priority of the message when sent to clients.
        CLIENT_SEND_PRIORITY_T priority;
        /// User-supplied context returned to callbacks.
        void *context;
} SEND_MSG_PARAMS_T;

/**
 * @brief Structure supplied to a send_request() call.
 */
typedef struct send_request_params_s {
        /// Path to send the request to
        const char *path;
        /// Request to be sent
        BUF_T *request;
        /// The datatype of the request
        DIFFUSION_DATATYPE request_datatype;
        /// The datatype of the response to be received
        DIFFUSION_DATATYPE response_datatype;
        /// Callback to handle the response
        request_on_response_cb on_response;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context. Can be NULL.
        void *context;
} SEND_REQUEST_PARAMS_T;

/**
 * @brief Callback for a request stream when a request has been received.
 *
 * @param session          The session.
 * @param path             Path the request was received on.
 * @param request_datatype The request's datatype.
 * @param request          The request.
 * @param responder_handle The responder handle used when dispatching a response with
 *                         `respond_to_request`. If the responder handle is required
 *                         outside the scope of this function, use `responder_handle_dup`
 *                         to copy it and use where necessary. This can be freed with
 *                         `responder_handle_free`.
 * @param context          User supplied context passed in from the request stream.
 * 
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_stream_on_request_cb)(SESSION_T *session, const char *path, DIFFUSION_DATATYPE request_datatype,
                                            const DIFFUSION_VALUE_T *request, const DIFFUSION_RESPONDER_HANDLE_T *responder_handle, void *context);

/**
 * @brief Callback when a request stream encounters an error.
 *
 * @param error The error received on the request stream.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_stream_on_error_cb)(const DIFFUSION_ERROR_T *error);

/**
 * @brief Callback for a request stream that has been closed.
 */
typedef void (*request_stream_on_close_cb)(void);

/**
 * @brief Structure supplied when adding a request stream.
 */
typedef struct diffusion_request_stream_s {
        /// Callback on receiving a request
        request_stream_on_request_cb on_request;
        /// Callback when an error is encountered
        request_stream_on_error_cb on_error;
        /// Callback when a request stream is closed
        request_stream_on_close_cb on_close;
        /// User supplied context. 
        void *context;
} DIFFUSION_REQUEST_STREAM_T;

/**
 * @brief Structure supplied when registering a message listener.
 */
typedef struct msg_listener_registration_params_s {
        /// Path
        char *topic_path;
        /// Function to be invoked when a message is received on the path.
        STREAM_MESSAGE_LISTENER_T listener;
        /// User-supplied context returned to callbacks.
        void *context;
} MSG_LISTENER_REGISTRATION_PARAMS_T;

/**
 * @brief Structure supplied when deregistering a message listener.
 */
typedef struct msg_listener_deregistration_params_s {
        /// Path
        char *topic_path;
} MSG_LISTENER_DEREGISTRATION_PARAMS_T;

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * @brief Synchronously send a message to Diffusion via a path.
 *
 * Messages are sent for a particular path. The message path acts
 * as a context for the recipient which will see the message as having
 * come 'from' the path as opposed to the actual originator. A topic
 * with the path need not exist on the server; if one does exist, it
 * is unaffected by messaging.
 *
 * The calling session must have the SEND_TO_CLIENT permission for the
 * message path.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the message to be sent.
 */
DEPRECATED(void send_msg(SESSION_T *session, SEND_MSG_PARAMS_T params));

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Send a request to a path.
 * 
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the request to be sent.
 */
void send_request(SESSION_T *session, SEND_REQUEST_PARAMS_T params);

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Set a request stream to handle requests to a specified path.
 *
 * @param session           The session handle. If NULL, this function returns immediately.
 * @param path              Path to receive requests on.
 * @param request_datatype  The request datatype.
 * @param response_datatype The response datatype.
 * @param request_stream    Request stream to handle requests to this path
 * 
 * @return the current REQUEST_STREAM_T at this path. NULL, if this is the
 *         first stream to be set to the path.
 */
DIFFUSION_REQUEST_STREAM_T *set_request_stream(SESSION_T *session, const char *path,
                        DIFFUSION_DATATYPE request_datatype, DIFFUSION_DATATYPE response_datatype, const DIFFUSION_REQUEST_STREAM_T *request_stream);

/**
 * @ingroup PublicAPI_Messaging
 *
 * @brief Remove a request stream bound to a specified path.
 *
 * @param session      The session handle. If NULL, this function returns immediately.
 * @param request_path The path at which to remove the request stream. If NULL,
 *                     this function returns immediately.
 *
 * @return the request stream that was removed. NULL, if there is no request stream
 *         bound to this path.
 */
DIFFUSION_REQUEST_STREAM_T *remove_request_stream(SESSION_T *session, const char *request_path);

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * @brief Register to accept messages (not topic updates) that are
 * sent to this session via a path.
 *
 * Control clients may send messages directly to a client (or a group
 * of clients) via a message path. Registering a listener for that
 * path allows a client to receive these messages and have them be
 * handled in a user-supplied function.
 *
 * Specifying a NULL message path is a special case that catches all
 * messages that do not have an explicit listener registered for it.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the listener registration.
 */
DEPRECATED(void register_msg_listener(SESSION_T *session, MSG_LISTENER_REGISTRATION_PARAMS_T params));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * @brief Deregister a listener for a path.
 *
 * If the message path specified in the params does not have a listener
 * registered at it, then this function has no effect.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters describing the listener to be
 *                      deregistered.
 */
DEPRECATED(void deregister_msg_listener(SESSION_T *session, MSG_LISTENER_DEREGISTRATION_PARAMS_T params));

#endif
