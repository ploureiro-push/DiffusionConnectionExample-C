#ifndef _diffusion_messaging_control_h_
#define _diffusion_messaging_control_h_ 1

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
 * @file messaging-control.h
 *
 * Messaging control feature.
 */

#include "session.h"
#include "misc/deprecate.h"

#include "datatypes.h"
#include "responder.h"
#include "registration.h"
#include "types/messaging_types.h"

/**
 * @brief Context information of the request received.
 * Request path, session ID and session properties
 */
typedef struct DIFFUSION_REQUEST_CONTEXT_T DIFFUSION_REQUEST_CONTEXT_T;

typedef int (*on_send_msg_to_session_cb)(SESSION_T *session,
                                         void *context);

#define SEND_MSG_TO_SESSION_HANDLERS       \
        on_send_msg_to_session_cb on_send; \
        ERROR_HANDLER_T on_error;          \
        DISCARD_HANDLER_T on_discard;

typedef struct send_msg_to_session_params_s {
        SEND_MSG_TO_SESSION_HANDLERS
        const char *topic_path;
        const CONTENT_T content;
        const SEND_OPTIONS_T options;
        const SESSION_ID_T session_id;
        void *context;
} SEND_MSG_TO_SESSION_PARAMS_T;

typedef struct svc_send_to_filter_response_s {
        uint32_t sent_count;
        LIST_T *error_reports;
} SVC_SEND_MSG_TO_FILTER_RESPONSE_T;

typedef int (*on_send_msg_to_filter_cb)(SESSION_T *session,
                                        const SVC_SEND_MSG_TO_FILTER_RESPONSE_T *response,
                                        void *context);

#define SEND_MSG_TO_FILTER_HANDLERS       \
        on_send_msg_to_filter_cb on_send; \
        ERROR_HANDLER_T on_error;         \
        DISCARD_HANDLER_T on_discard;

typedef struct send_msg_to_filter_params_s {
        SEND_MSG_TO_FILTER_HANDLERS
        const char *topic_path;
        const CONTENT_T content;
        const SEND_OPTIONS_T options;
        const char *filter;
        void *context;
} SEND_MSG_TO_FILTER_PARAMS_T;

/**
 * @brief Structure containing the information received and passed to
 * a handler registered on a message path, including the message content.
 */
typedef struct svc_send_receiver_client_request_s {
        /// Path from which this message was received.
        char *topic_path;
        /// Content of message.
        CONTENT_T *content;
        /// Session ID of sending client.
        SESSION_ID_T session_id;
        /// Additional options relating to message delivery.
        SEND_OPTIONS_T send_options;
        /// Properties associated with the message handler.
        HASH_T *session_properties;
        /// Conversation ID of sender, if response required.
        CONVERSATION_ID_T conversation_id;
} SVC_SEND_RECEIVER_CLIENT_REQUEST_T;

/**
 * @brief Callback when a handler is registered with the server.
 *
 * @param session The currently active session.
 * @param context User-supplied context from the initial
 *        register_msg_handler() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_msg_receiver_registration_cb)(SESSION_T *session,
                                               void *context);

/**
 * @brief Callback when a message is received by a registered handler
 * on a message path.
 *
 * @param session The currently active session.
 * @param request Details about the incoming message, and the message
 *        itself.
 * @param context User-supplied context from the initial
 *        register_msg_handler() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_send_receiver_client_msg_cb)(SESSION_T *session,
                                              const SVC_SEND_RECEIVER_CLIENT_REQUEST_T *request,
                                              void *context);

/**
 * @brief Handlers for notification of registration of handler
 *        and message receipt.
 */
#define MSG_RECEIVER_REGISTRATION_HANDLERS             \
        on_msg_receiver_registration_cb on_registered; \
        on_send_receiver_client_msg_cb on_message;     \
        ERROR_HANDLER_T on_error;                      \
        DISCARD_HANDLER_T on_discard;

/**
 * @brief Structure supplied when issuing a register_msg_handler() call.
 */
typedef struct msg_receiver_registration_params_s {
        MSG_RECEIVER_REGISTRATION_HANDLERS
        /// Path used for registration.
        const char *topic_path;
        /// Control group. Leave as NULL to use a default value.
        const char *control_group;
        /// Properties associated with this message handler.
        LIST_T *session_properties;
        /// User-supplied context.
        void *context;
} MSG_RECEIVER_REGISTRATION_PARAMS_T;

/**
 * @brief Callback when a response to a request is received.
 *
 * @param response_datatype  Datatype of the response.
 * @param response           Response as a `DIFFUSION_VALUE_T`.
 * @param context            User supplied context passed in from `SEND_REQUEST_TO_SESSION_PARAMS_T`.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_on_response_cb)(DIFFUSION_DATATYPE response_datatype, const DIFFUSION_VALUE_T *response, void *context);

/**
 * @brief Structure supplied when issuing a send_request_to_session() call
 */
typedef struct send_request_to_session_params_s {
        /// The session id of the session to receive the request.
        SESSION_ID_T *recipient_session;
        /// The request path used by the recipient to select an appropriate handler.
        const char *path;
        /// The request to send
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
} SEND_REQUEST_TO_SESSION_PARAMS_T;

/**
 * @brief Callback when a request to a request handler has been received.
 *
 * @param session            The currently active session.
 * @param request_datatype   The request datatype.
 * @param request            The request.
 * @param request_context    The request context.
 * @param handle             The responder handle. Used when a response is dispatched with
 *                           `respond_to_request`.
 * @param context            User supplied context.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_handler_on_request)(SESSION_T *session,  DIFFUSION_DATATYPE request_datatype, const DIFFUSION_VALUE_T *request,
                                          const DIFFUSION_REQUEST_CONTEXT_T *request_context, const DIFFUSION_RESPONDER_HANDLE_T *handle, void *context);

/**
 * @brief Callback when a request handler is active.
 *
 * @param session            The currently active session.
 * @param path               The path request handler is bound to.
 * @param registered_handler The registered handler
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_handler_on_active)(SESSION_T *session, const char *path, const DIFFUSION_REGISTRATION_T *registered_handler);

/**
 * @brief Callback when a request handler is closed.
 */
typedef void (*request_handler_on_close)(void);

/**
 * @brief Callback when a request handler has encountered an error.
 *
 * @param error              The received error.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*request_handler_on_error)(const DIFFUSION_ERROR_T *error);

/**
 * @brief Request handler structure required when creating an `ADD_REQUEST_HANDLER_PARAMS_T`
 */
typedef struct diffusion_request_handler_s {
        /// The datatype of the request
        DIFFUSION_DATATYPE request_datatype;
        /// The datatype of the response to be received
        DIFFUSION_DATATYPE response_datatype;
        /// Callback when a request handler has been registered.
        request_handler_on_active on_active;
        /// Callback when a request has been received by the handler.
        request_handler_on_request on_request;
        /// Callback when an error occurs.
        request_handler_on_error on_error;
        /// Callback when a request handler is closed.
        request_handler_on_close on_close;
} DIFFUSION_REQUEST_HANDLER_T;

/**
 * @brief Structure supplied when issuing a add_request_handler() call.
 */
typedef struct add_request_handler_params_s {
        /// The request path to set a handler to.
        const char *path;
        /// Request handler
        DIFFUSION_REQUEST_HANDLER_T *request_handler;
        /// The session properties
        char **session_properties;
        // Callback if an error occurs during request handler
        // registration or deregistration. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} ADD_REQUEST_HANDLER_PARAMS_T;

/**
 * @brief Callback when a filtered request has been dispatched to a number
 * of sessions
 *
 * @param number_sent        The number of requests sent to sessions.
 * @param context            User supplied context passed in from `SEND_REQUEST_TO_FILTER_PARAMS_T`.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*filtered_request_number_sent)(int number_sent, void *context);

/**
 * @brief Callback when a response to a filtered request has been received.
 *
 * @param response_datatype  Datatype of the response.
 * @param response           Response as a `DIFFUSION_VALUE_T`.
 * @param context            User supplied context passed in from `SEND_REQUEST_TO_FILTER_PARAMS_T`.
 * 
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*filtered_request_on_response)(DIFFUSION_DATATYPE response_datatype, const DIFFUSION_VALUE_T *response, void *context);

/**
 * @brief Callback when a response results in an error.
 *
 * @param session_id         The session id of the session which responded with
 *                           an error.
 * @param error              The response error. The context will be the user supplied context passed in from
 *                           `SEND_REQUEST_TO_FILTER_PARAMS_T`.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*filtered_request_on_response_error)(const SESSION_ID_T *session_id, const DIFFUSION_ERROR_T *error);

/**
 * Structure supplied when issuing a send_request_to_filter() call.
 */
typedef struct send_request_to_filter_params_s {
        /// The request path to send the request to.
        const char *path;
        /// The session filter expression.
        const char *filter;
        /// The datatype of the request
        DIFFUSION_DATATYPE request_datatype;
        /// The datatype of the response to be received
        DIFFUSION_DATATYPE response_datatype;
        /// Request to send
        BUF_T *request;
        /// Callback when the request has been dispatched to
        /// all matching sessions.
        filtered_request_number_sent on_number_sent;
        /// Callback when a response is received
        filtered_request_on_response on_response;
        /// Callback when an error response is received
        filtered_request_on_response_error on_response_error;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User supplied context. Can be NULL.
        void *context;
} SEND_REQUEST_TO_FILTER_PARAMS_T;

/**
 * @ingroup PublicAPI_MessagingControl
 *
 * @brief Return a copy of the request context's session ID. `session_id_free` should be
 * called on this pointer when no longer needed.
 *
 * @param request_context the request context to retrieve the session ID from.
 * 
 * @return a copy of the request context's session ID. NULL, if `request_context` is NULL.
 */
SESSION_ID_T *diffusion_request_context_get_session_id(const DIFFUSION_REQUEST_CONTEXT_T *request_context);

/**
 * @ingroup PublicAPI_MessagingControl
 *
 * @brief Return a copy of the request context's request path. `free` should be called on
 * this pointer when no longer needed.
 *
 * @param request_context the request context to retrieve the request path from.
 *
 * @return a copy of the request context's request path. NULL, if `request_context` is NULL.
 */
char *diffusion_request_context_get_path(const DIFFUSION_REQUEST_CONTEXT_T *request_context);

/**
 * @ingroup PublicAPI_MessagingControl
 *
 * @brief Return a copy of the request context's session properties. `hash_free` should be called
 * on this pointer when no longer needed.
 *
 * @param request_context the request context to retrieve session properties from.
 *
 * @return a copy of the request context's session properties. NULL, if `request_context` is NULL.
 */
HASH_T *diffusion_request_context_get_session_properties(const DIFFUSION_REQUEST_CONTEXT_T *request_context);

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * The preferred alternative is `send_request_to_session`.
 *
 * @brief Send a message to a specific session.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the request to send a message to a specific
 *                      session.
 */
DEPRECATED(void send_msg_to_session(SESSION_T *session, SEND_MSG_TO_SESSION_PARAMS_T params));

/**
 * @ingroup PublicAPI_MessagingControl
 *
 * @brief Send a request to a specific session.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the request to send a request to a specific
 *                      session.
 */
void send_request_to_session(SESSION_T *session, SEND_REQUEST_TO_SESSION_PARAMS_T params);

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * The preferred alternative is `send_request_to_filter`.
 *
 * @brief Send a message via a filter.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the request to send a message via a filter.
 */
DEPRECATED(void send_msg_to_filter(SESSION_T *session, SEND_MSG_TO_FILTER_PARAMS_T params));

/**
 * @brief Send a request via a filter.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the request to send a request via a filter.
 */
void send_request_to_filter(SESSION_T *session, SEND_REQUEST_TO_FILTER_PARAMS_T params);

/**
 * @ingroup PublicAPI_MessagingControl
 *
 * @brief Add a server registered request handler
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the request handler to be added.
 */
void add_request_handler(SESSION_T *session, ADD_REQUEST_HANDLER_PARAMS_T params);

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * The preferred alternative is `add_request_handler`.
 *
 * @brief Register a message handler.
 *
 * Message handlers receive messages which are sent from clients via
 * paths. Only one handler for a particular path may be
 * active at any one time, as determined by the server.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the request to register a
 *                      message handler.
 */
DEPRECATED(void register_msg_handler(SESSION_T *session, const MSG_RECEIVER_REGISTRATION_PARAMS_T params));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * @brief Deregister a message handler.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        The same parameters passed when registering the
 *                      message handler.
 */
DEPRECATED(void deregister_msg_handler(SESSION_T *session, const MSG_RECEIVER_REGISTRATION_PARAMS_T params));

#endif
