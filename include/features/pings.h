#ifndef _diffusion_pings_h_
#define _diffusion_pings_h_ 1

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
 * @file pings.h
 *
 * Pings feature.
 */

#include "session.h"

/**
 * @brief Structure of a system ping request.
 */
typedef struct svc_ping_system_request_s {
        /// @cond HIDDEN
        void *_reserved;
        /// @endcond
} SVC_PING_SYSTEM_REQUEST_T;

/**
 * @brief Structure of a user ping request.
 */
typedef struct svc_ping_user_request_s {
        /// @cond HIDDEN
        void *_reserved;
        /// @endcond
} SVC_PING_USER_REQUEST_T;

/**
 * @brief Callback for ping_system_register_handler().
 *
 * @param session The current active session.
 * @param request The incoming ping request.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_ping_system_request_cb)(SESSION_T *session,
                                         const SVC_PING_SYSTEM_REQUEST_T *request,
                                         void *context);

/**
 * @brief Callback for ping_system().
 *
 * @param session The current active session.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_ping_system_response_cb)(SESSION_T *session,
                                          void *context);

/**
 * @brief Callback handlers that can be registered for dealing with
 * system ping responses.
 */
#define PING_SYSTEM_HANDLERS                                            \
        /** Callback for handling a system ping request from            \
         * Diffusion. */                                                \
        on_ping_system_request_cb on_ping_request;                      \
        /** Callback for handling a system ping response that was       \
         * initiated by this application. */                            \
        on_ping_system_response_cb on_ping_response;                    \
        /** Standard service error callback. */                         \
        ERROR_HANDLER_T on_error;

typedef struct ping_system_handlers_s {
        PING_SYSTEM_HANDLERS
} PING_SYSTEM_HANDLERS_T;

/**
 * @brief Structure supplied when registering a system ping handler or
 * sending a system ping request.
 */
typedef struct ping_system_params_s {
        PING_SYSTEM_HANDLERS
        /// User-supplied context to callbacks.
        void *context;
} PING_SYSTEM_PARAMS_T;

/**
 * @brief Callback for ping_user_register_handler().
 *
 * @param session The current active session.
 * @param request The incoming ping request.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_ping_user_request_cb)(SESSION_T *session,
                                       const SVC_PING_USER_REQUEST_T *request,
                                       void *context);

/**
 * @brief Callback for ping_user().
 *
 * @param session The current active session.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_ping_user_response_cb)(SESSION_T *session,
                                        void *context);

/**
 * @brief Callback handlers that can be registered for dealing with
 * user ping responses.
 */
#define PING_USER_HANDLERS                                              \
        /** Callback for handling a user ping request from Diffusion. */ \
        on_ping_user_request_cb on_ping_request;                        \
        /** Callback for handling a user ping response that initiated   \
            initiated by this application. */                           \
        on_ping_user_response_cb on_ping_response;                      \
        /** Standard service error callback. */                         \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard conversation discard callback */                   \
        DISCARD_HANDLER_T on_discard;

typedef struct ping_user_handlers_s {
        PING_USER_HANDLERS
} PING_USER_HANDLERS_T;

/**
 * @brief Structure supplied when registering a user ping handler or
 * sending a user ping request.
 */
typedef struct ping_user_params_s {
        PING_USER_HANDLERS
        /// User-supplied context returned to callbacks.
        void *context;
} PING_USER_PARAMS_T;

/**
 * @ingroup PublicAPI_Ping
 *
 * @brief Register a user-defined handler for system pings.
 *
 * If you don't register a handler, then the default behaviour is to
 * automatically respond to the ping.
 *
 * The handler can be removed by passing the handlers with the {@link
 * PING_SYSTEM_PARAMS_T on_ping_request} function set to NULL.
 *
 * @param session          A session handle.  If NULL, this function returns immediately.
 * @param params           Callbacks and other parameters. {@link PING_SYSTEM_PARAMS_T on_ping_request}
 *                         should be set.
 */
void ping_system_register_handler(SESSION_T *session, const PING_SYSTEM_PARAMS_T params);

/**
 * @ingroup PublicAPI_Ping
 *
 * @brief Register a user-defined handler for user pings.
 *
 * If you don't register a handler, then the default behaviour is to
 * automatically respond to the ping.
 *
 * The handler can be removed by passing the handlers with the {@link
 * PING_USER_PARAMS_T on_ping_request} function set to NULL.
 *
 * @param session          A session handle. If NULL, this function returns immediately.
 * @param params           Callbacks and other parameters. @{link PING_USER_PARAMS_T on_ping_request}
 *                         should be set.
 */
void ping_user_register_handler(SESSION_T *session, const PING_USER_PARAMS_T params);

/**
 * @ingroup PublicAPI_Ping
 *
 * @brief Explicitly send a user ping to Diffusion.
 *
 * @param session          A session handle. If NULL, this function returns immediately.
 * @param params           Callbacks and other parameters. {@link PING_USER_PARAMS_T on_ping_response}
 *                         should be set.
 */
void ping_user(SESSION_T *session, const PING_USER_PARAMS_T params);

#endif