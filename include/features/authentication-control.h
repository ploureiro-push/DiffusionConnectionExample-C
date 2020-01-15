#ifndef _diffusion_authentication_control_h_
#define _diffusion_authentication_control_h_ 1

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
 * @file authentication-control.h
 *
 * Authentication control feature.
 * 
 * Functions required for registering and deregistering an 
 * authentication service in a control client.
 */

#include "authenticator.h"
#include "registration.h"
#include "session.h"
#include "hash.h"
#include "types/client_details_types.h"

/**
 * If not specified, the group name "default" is used when registering
 * authentication handlers.
 */
#define DEFAULT_AUTH_GROUP_NAME "default"

/**
 * @brief Degree of session detail required.
 *
 * When registering an authentication handler, you can request various levels
 * of information about connecting clients to enable you to make a decision
 * about the validity of the client. This enumeration lists the available
 * details that can be requested.
 */
typedef enum {
        /// Request summary information only.
        SESSION_DETAIL_SUMMARY = 0,
        /// Also request GeoIP information.
        SESSION_DETAIL_LOCATION = 1,
        /// Request information about the connector which the client
        /// has used while connecting to Diffusion.
        SESSION_DETAIL_CONNECTOR_NAME = 2
} SESSION_DETAIL_TYPE_T;

/**
 * @brief Structure containing the information required when sending a
 * request to register as an authentication service.
 */
typedef struct svc_authentication_register_request_s {
        /// Service ID, at the moment this is always SVC_AUTHENTICATION
        uint32_t service_id;

        /// The control group name to register with; "default" if not
        /// specified.
        char *control_group;

        /// A name which this handler is known by, and which matches a
        /// name defined in Server.xml.
        char *handler_name;

        /// A hashmap of the types of information requested when the
        /// authorisation handler is called.
        HASH_T *session_detail_type_set;

        /// A conversation ID to correlate requests and responses
        /// associated to this request.
        CONVERSATION_ID_T *conversation_id;
} SVC_AUTHENTICATION_REGISTER_REQUEST_T;

/**
 * @brief The request to deregister an authentication handler. The
 * same as the registration request.
 */
typedef SVC_AUTHENTICATION_REGISTER_REQUEST_T SVC_AUTHENTICATION_DEREGISTER_REQUEST_T;

/**
 * @brief This structure is populated in the authentication request
 * when the detail type SESSION_DETAIL_CONNECTOR_NAME has been
 * requested.
 */
typedef struct connector_details_s {
        /// The name of the connector to which the client has connected.
        char *name;
} CONNECTOR_DETAILS_T;

/**
 * @brief The action to take for the authentication request.
 *
 * On receipt of an authentication request, the service may either
 * deny the connecting client, allow it or abstain from answering
 * which passes the request to the next handler in the chain.
 */
typedef enum {
        /// Authentication failed.
        AUTHENTICATION_DENY = 0,
        /// Authentication has neither passed nor failed.
        AUTHENTICATION_ABSTAIN = 1,
        /// Authentication passed, no result supplied.
        AUTHENTICATION_ALLOW = 2,
        /// Authentication passed, result supplied.
        AUTHENTICATION_ALLOW_WITH_RESULT = 3
} SVC_AUTHENTICATION_RESPONSE_VALUE_T;

/**
 * @brief The structure passed to an authentication handler.
 */
typedef struct svc_authentication_request_s {
        /// The principal of the connecting client.
        char *principal;

        /// Credentials of the connecting client.
        CREDENTIALS_T *credentials;

        /// If requested by the authentication handler, these are the
        /// session details of the connecting client.
        SESSION_DETAILS_SUMMARY_T *session_details;

        /// If requested by the authentication handler, these are the
        /// GeoIP details of the connecting client.
        LOCATION_DETAILS_T *location_details;

        /// If requested by the authentication handler, these are the
        /// details of the Diffusion connector through which the
        /// client is connecting.
        CONNECTOR_DETAILS_T *connector_details;

        /// The conversation ID to be used in the response so
        /// Diffusion can correlate it to the originating request.
        CONVERSATION_ID_T conversation_id;
} SVC_AUTHENTICATION_REQUEST_T;

/**
 * @brief Structure holding the response message from the
 * authentication handler.
 */
typedef struct svc_authentication_response_s {
        /// Indicates whether to deny, abstain or allow the request.
        SVC_AUTHENTICATION_RESPONSE_VALUE_T value;
        /// List of additional roles which the principal has (when
        /// value is AUTHENTICATION_ALLOW_WITH_RESULT).
        LIST_T *roles;
        /// Hashmap of additional properties (when value is
        /// AUTHENTICATION_ALLOW_WITH_RESULT).
        HASH_T *properties;
} SVC_AUTHENTICATION_RESPONSE_T;

/**
 * @brief Callback for on_authentication().
 *
 * @param session The current active session.
 * @param request The incoming authentication request.
 * @param response The response to be returned from the handler.
 * @param context User-supplied context from initial authentication handler registration.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_authentication_cb)(SESSION_T *session,
                                    const SVC_AUTHENTICATION_REQUEST_T *request,
                                    SVC_AUTHENTICATION_RESPONSE_T *response,
                                    void *context);

/**
 * @brief Callbacks that can be supplied by an authentication service.
 */
#define AUTHENTICATION_HANDLERS                         \
        /** Authentication request callback. */         \
        on_authentication_cb on_authentication;         \
        /** Standard service error handler callback. */ \
        ERROR_HANDLER_T on_error;

/**
 * @brief Callback for on_registration().
 *
 * @param session The currently active session.
 * @param context User-supplied context from the initial authentication_register() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_authentication_registration_cb)(SESSION_T *session,
                                                 void *context);

/**
 * @brief Handlers for notification of registration of an
 * authentication handler.
 */
#define AUTHENTICATION_REGISTRATION_HANDLERS                            \
        /** Callback when Diffusion acknowledges the request for the service to be registered. */ \
        on_authentication_registration_cb on_registration;              \
        /**  Standard service error handler callback. */                \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard conversation discard callback */                   \
        DISCARD_HANDLER_T on_discard;

/**
 * @brief Structure supplied when issuing an authentication_register() request.
 */
typedef struct authentication_registration_params_s {
        AUTHENTICATION_REGISTRATION_HANDLERS
        struct {
                AUTHENTICATION_HANDLERS
        } authentication_handlers;
        /// Name of the authentication handler, as specified in
        /// Diffusion's Server.xml configuration file.
        const char *name;
        /// A set of detail types to be request on a registration
        /// request. The hash should contain keys which map to
        /// SESSION_DETAILS_TYPE_T values, converted to
        /// NULL-terminated strings.
        const HASH_T *detail_set;
        /// User-supplied context to return to callbacks.
        void *context;
} AUTHENTICATION_REGISTRATION_PARAMS_T;

/**
 * @brief Callback for on_deregistration().
 *
 * @param session The currently active session.
 * @param context User-supplied context from the initial authentication_deregister() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_authentication_deregistration_cb)(SESSION_T *session, void *context);

/**
 * @brief Callback when an authentication handler is active.
 *
 * @param session            The currently active session.
 * @param registered_handler The registered handler
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*authenticator_on_active)(SESSION_T *session, const DIFFUSION_REGISTRATION_T *registered_handler);

/**
 * @brief Callback when an authentication handler has received a connection request to be
 * authenticated.
 *
 * @param session                     The currently active session.
 * @param principal                   The principal of the connection request to be authenticated
 * @param credentials                 The credentials of the connection request to be authenticated
 * @param session_properties          The session properties of the connection request to be authenticated
 * @param proposed_session_properties The proposed properties from the connection request to be authenticated
 * @param authenticator               The authenticator. Used to authenticate connection requests (see authenticator.h)
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*authenticator_on_authenticate)(SESSION_T *session, const char *principal, const CREDENTIALS_T *credentials,
                                             const HASH_T *session_properties, const HASH_T *proposed_session_properties,
                                             const DIFFUSION_AUTHENTICATOR_T *authenticator);

/**
 * @brief Callback when an authenticator has encountered an error.
 *
 * @param error The received error.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*authenticator_on_error)(const DIFFUSION_ERROR_T *error);

/**
 * @brief Callback when an authentication handler is closed.
 */
typedef void (*authenticator_on_close)(void);

/**
 * @brief Callbacks invoked in response to a deregistration request.
 */
#define AUTHENTICATION_DEREGISTRATION_HANDLERS                          \
        /** Callback when an authorisation handler deregistration response has been received. */ \
        on_authentication_deregistration_cb on_deregistration;          \
        /** Standard service error handler callback */                  \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard conversation discard callback */                   \
        DISCARD_HANDLER_T on_discard;

typedef struct authentication_deregistration_handlers_s {
        AUTHENTICATION_DEREGISTRATION_HANDLERS
} AUTHENTICATION_DEREGISTRATION_HANDLERS_T;

/**
 * @brief Structure supplied when issuing an authentication_deregister() request.
 */
typedef struct authentication_deregistration_params_s {
        AUTHENTICATION_DEREGISTRATION_HANDLERS
        /// The original request as returned by svc_authentication_register().
        const SVC_AUTHENTICATION_REGISTER_REQUEST_T *original_request;
        /// User-supplied context to return to callbacks.
        void *context;
} AUTHENTICATION_DEREGISTRATION_PARAMS_T;

/**
 * @brief Structure supplied as part of a `diffusion_set_authentication_handler` request.
 */
typedef struct diffusion_authentication_handler_s {
        /// Name of the authentication handler
        char *handler_name;
        /// Callback when the authentication handler is
        /// active.
        authenticator_on_active on_active;
        /// Callback when a connection request to be
        /// authenticated has been received.
        authenticator_on_authenticate on_authenticate;
        /// Callback when the authentication handler
        /// encounters an error.
        authenticator_on_error on_error;
        /// Callback when an authentication handler
        /// is closed.
        authenticator_on_close on_close;
} DIFFUSION_AUTHENTICATION_HANDLER_T;

/**
 * @brief Structure supplied when issuing an `diffusion_set_authentication_handler` request.
 */
typedef struct diffusion_authentication_handler_params_s {
        /// Handler
        DIFFUSION_AUTHENTICATION_HANDLER_T *handler;
        /** Standard service error handler callback */                  \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard conversation discard callback */                   \
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context to return to callbacks.
        void *context;
} DIFFUSION_AUTHENTICATION_HANDLER_PARAMS_T;

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release. Applications should now use the preferred
 * function `diffusion_set_authentication_handler`.
 *
 * @brief Register an authentication handler.
 *
 * @param session       The session handle. If NULL, this function returns immediately
 *                      with the value NULL.
 * @param params        Parameters defining the request to register an
 *                      authentication handler.
 *
 * @return              The request structure which is required for later
 *                      deregistering the authorisation handler, or NULL if the session
 *                      handle was NULL.
 */
DEPRECATED(SVC_AUTHENTICATION_REGISTER_REQUEST_T *authentication_register(SESSION_T *session, const AUTHENTICATION_REGISTRATION_PARAMS_T params));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * @brief Deregister an authentication handler
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameters defining the request to deregister an
 *                      existing authentication handler.
 */
DEPRECATED(void authentication_deregister(SESSION_T *session, const AUTHENTICATION_DEREGISTRATION_PARAMS_T params));

/**
 * @ingroup PublicAPI_AuthenticationControl
 *
 * @brief Register an authentication handler for client authentication events.
 *
 * @param session       The session handle. If NULL, this function returns immediately
 *                      with the value NULL.
 * @param params        Parameters defining the request to register an
 *                      authentication handler.
 */
void diffusion_set_authentication_handler(SESSION_T *session, const DIFFUSION_AUTHENTICATION_HANDLER_PARAMS_T params);

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * @brief Free a SVC_AUTHENTICATION_REGISTER_REQUEST_T.
 *
 * @param session The currently active session.
 * @param request The request to be freed.
 */
DEPRECATED(void svc_authentication_register_request_free(SESSION_T *session, SVC_AUTHENTICATION_REGISTER_REQUEST_T *request));

#endif
