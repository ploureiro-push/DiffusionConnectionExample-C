#ifndef _diffusion_session_h_
#define _diffusion_session_h_ 1

/*
 * Copyright © 2014, 2015 Push Technology Ltd., All Rights Reserved.
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
 * @file session.h
 *
 * Functions relating to a session and session management.
 *
 * A privileged client can monitor other sessions, including changes to their
 * session properties, using a `session_properties_listener_register`. When
 * registering to receive session properties, special key values of
 * `ALL_FIXED_PROPERTIES` and `ALL_USER_PROPERTIES` can be used.
 *
 * Each property is identified by a key. Most properties have a single string
 * value. The exception is the `$Roles` fixed property which has a set of string
 * values.
 *
 * Fixed properties are identified by keys with a '$' prefix. The available
 * fixed session properties are:
 *
 * `$ClientIP`
 * The Internet address of the client in string format.
 *
 * `$ClientType`
 * The client type of the session. One of `ANDROID`, `C`,
 * `DOTNET`, `IOS`, `JAVA`, `JAVASCRIPT_BROWSER`, or
 * `OTHER`.
 *
 * `$Connector`
 * The configuration name of the server connector that the client connected
 * to.
 *
 * `$Country`
 * The country code for the country where the client's Internet address was
 * allocated (for example, `NZ` for New Zealand). Country codes are as
 * defined by locale. If the country code could not be determined, this
 * will be a zero length string.
 *
 * `$Language`
 * The language code for the official language of the country where the
 * client's Internet address was allocated (for example, `en` for
 * English). Language codes are as defined by locale. If the language
 * could not be determined or is not applicable, this will be a zero length
 * string.
 *
 * `$Latitude`
 * The client's latitude, if available. This will be the string
 * representation of a floating point number and will be `NaN` if not
 * available.
 *
 * `$Longitude`
 * The client's longitude, if available. This will be the string
 * representation of a floating point number and will be `NaN` if not
 * available.
 *
 * `$Principal`
 * The security principal associated with the client session.
 *
 * `$Roles`
 * Authorisation roles assigned to the session. This is a set of roles
 * represented as quoted strings (for example, `"role1","role2"`). The
 * utility method `diffusion_string_to_roles` can be used to parse
 * the string value into a set of roles.
 *
 * `$ServerName`
 * The name of the server to which the session is connected.
 *
 * `$SessionId`
 * The session identifier. Equivalent to `session_id_to_string`.
 *
 * `$StartTime`
 * The session's start time in milliseconds since the epoch.
 *
 * `$Transport`
 * The session transport type. One of `WEBSOCKET` or
 * `HTTP_LONG_POLL` or `OTHER`
 *
 * All user-defined property keys are non-empty strings. The characters ' ', '\t',
 * '\r', '\n', '"', ''', '(', ')' are not allowed.
 */

#include <apr_pools.h>
#include <apr_atomic.h>

#include "session-factory.h"

#include "hash.h"
#include "reconnect.h"

#include "types/error_types.h"
#include "types/security_types.h"
#include "types/topic_types.h"
#include "types/session_types.h"

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session and synchronously open a connection to the
 * Diffusion server at the endpoint specified by the URL.
 *
 * @note This call retains a reference to the memory allocated for the {@link
 * CREDENTIALS_T credentials} and it is the callers' responsibility to ensure
 * that this memory is not freed or overwritten for the lifetime of the
 * session structure.
 *
 * @note The error structure contains the result of the session creation
 * attempt and will not be overwritten if the session is subsequently
 * reconnected or fails over. Once session_create() returns, it is safe to
 * release any memory owned by the error structure.
 *
 * @param url_str		A URL describing the endpoint to connect to.
 * @param principal		Typically, the username of the connecting user or application.
 *				NULL indicates that the principal will not be passed on connection.
 * @param credentials		The credentials associated with the principal, or NULL.
 *                              \em MUST be available for the duration of the session.
 * @param listener		A pointer to a {@link SESSION_LISTENER_T} structure containing callbacks
 *				to be called on session-specific events.
 * @param reconnection_strategy How to handle reconnection situations when the server is initially
 *				unavailable. If NULL, a default strategy is provided that
 *				attempts to connect every 5 seconds for up to 1 minute.
 * @param error			A pointer to an error structure, initialized to zero, which is populated in 
 *				case of error, or NULL if not required.
 * @retval "SESSION_T *"	A pointer to a session handle.
 * @retval NULL			If the session could not be created.
 */
SESSION_T *session_create(const char *url_str,
                          const char *principal,
                          CREDENTIALS_T *credentials,
                          SESSION_LISTENER_T *listener,
                          RECONNECTION_STRATEGY_T *reconnection_strategy,
                          DIFFUSION_ERROR_T *error);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session and synchronously open a connection to the
 * Diffusion server with a `DIFFUSION_SESSION_FACTORY_T`.
 *
 * @param session_factory       The session factory to initiate the session from
 * @param url_str               A URL describing the endpoint to connect to. This can be NULL
 *                              if the supplied `session_factory` sufficiently describes the
 *                              end point to connect to with `diffusion_session_factory_server_host`,
 *                              `diffusion_session_factory_server_port` and `diffusion_session_factory_secure_transport`
 *
 * @retval "SESSION_T *"        A pointer to a session handle.
 * @retval NULL                 If the session could not be created.
 */
SESSION_T *session_create_with_session_factory(const DIFFUSION_SESSION_FACTORY_T *session_factory, const char *url_str);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session, but do not wait for the connection handshake
 * to be performed.
 *
 * @deprecated The error parameter is deprecated and will be removed
 * in a future release.
 *
 * @note This call retains a reference to the memory allocated for the {@link
 * CREDENTIALS_T credentials}, and it is the callers' responsibility to ensure
 * that this memory is not freed or overwritten for the lifetime of the
 * session structure.
 *
 * @note The error structure contains the result of the session creation
 * attempt and will not be overwritten if the session is subsequently
 * reconnected or fails over. Since session_create_async() returns
 * immediately, you must ensure the memory that this parameter points to is
 * available until the session has been created, or the creation has
 * failed. It is highly recommended that the error callback (see the {@link
 * SESSION_CREATE_CALLBACK_T callbacks} parameter) is used instead, and NULL
 * passed here.
 *
 * @param url_str		A URL describing the endpoint to connect to.
 * @param principal		Typically, the username of the connecting user or application.
 *				NULL indicates that the principal will not be passed on connection.
 * @param credentials		The credentials associated with the principal, or NULL.
 *                              \em MUST be available for the duration of the session.
 * @param listener		A pointer to a \ref SESSION_LISTENER_T structure containing callbacks
 *				to be called on session-specific events.
 * @param reconnection_strategy How to handle reconnection situations when the server is initially
 *				unavailable. If NULL, a default strategy is provided that
 *				attempts to connect every 5 seconds for up to 1 minute.
 * @param callbacks		A pointer to a structure containing callbacks for successful
 *				connection or error reporting.
 * @param error			A pointer to an error structure to be asynchronously populated if
 *                              an error occurs while the session is established, or NULL if not required.
 *                              \em MUST be available for the duration of the session.
 * @retval "SESSION_T *" A pointer to a session handle that should be closed
 *                              and freed on exit.  The session is not open
 *                              when returned and you should use the {@link
 *                              SESSION_CREATE_CALLBACK_T on_connected}
 *                              callback to obtain a valid session. This
 *                              handle is merely provided for proper cleanup.
 */
SESSION_T *
session_create_async(const char *url_str,
                     const char *principal,
                     CREDENTIALS_T *credentials,
                     SESSION_LISTENER_T *listener,
                     RECONNECTION_STRATEGY_T *reconnection_strategy,
                     SESSION_CREATE_CALLBACK_T *callbacks,
                     DIFFUSION_ERROR_T *error);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Create a new session and asynchronously open a connection to the
 * Diffusion server with a `DIFFUSION_SESSION_FACTORY_T`.
 *
 * @param session_factory       The session factory to initiate the session from
 * @param callbacks             A pointer to a structure containing callbacks for successful
 *                              connection or error reporting.
 * @param url_str               A URL describing the endpoint to connect to. This can be NULL
 *                              if the supplied `session_factory` sufficiently describes the
 *                              end point to connect to with `diffusion_session_factory_server_host`,
 *                              `diffusion_session_factory_server_port` and `diffusion_session_factory_secure_transport`
 *
 * @retval "SESSION_T *"        A pointer to a session handle.
 * @retval NULL                 If the session could not be created.
 */
SESSION_T *session_create_async_with_session_factory(const DIFFUSION_SESSION_FACTORY_T *session_factory,
                                                     SESSION_CREATE_CALLBACK_T *callbacks,
                                                     const char *url_str);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Free memory associated with a session.
 *
 * The session must be closed with session_close() prior to calling
 * this function. If session is NULL, the function returns
 * immediately.
 *
 * @param session	The session to be freed.
 */
void session_free(SESSION_T *session);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Stop accepting messages from a Diffusion server and close the connection.
 *
 * @param session	The session handle. If NULL, an error is returned.
 * @param error		A structure for storing error messages, or NULL if
 *			detailed error reporting is not required.
 * @retval int		0 on error
 * @retval int		1 on success
 */
int session_close(SESSION_T *session, DIFFUSION_ERROR_T *error);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Returns the current session state.
 *
 * @param session The session handle. If NULL, \ref SESSION_STATE_UNKNOWN
 *                is returned.
 *
 * @retval SESSION_STATE_T The session state, or \ref SESSION_STATE_UNKNOWN
 *                         if session is NULL.
 */
SESSION_STATE_T session_state_get(SESSION_T *session);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Returns the last connection response code.
 *
 * @param session       The session handle. May not be NULL.
 * @retval CONNECTION_RESPONSE_CODE_T The last connection response code.
 */
CONNECTION_RESPONSE_CODE_T session_connection_response_code(SESSION_T *session);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Returns a human-readable representation of the session state.
 *
 * @param state		A session state value.
 * @return		A textual representation of the session state.
 */
const char *session_state_as_string(const SESSION_STATE_T state);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Determines if a session is connected to a server.
 *
 * @param session A session handle. A NULL value returns
 *                \ref DIFFUSION_FALSE.
 * @retval DIFFUSION_TRUE	If the session is connected.
 * @retval DIFFUSION_FALSE	If the session is not connected.
 */
int session_is_connected(const SESSION_T *session);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Determines if a session is in a recovering state, where it is trying
 * to reconnect to a server.
 *
 * @param session		A session handle. A NULL value returns \ref DIFFUSION_FALSE.
 * @retval DIFFUSION_TRUE	If the session is recovering.
 * @retval DIFFUSION_FALSE	If the session is not recovering.
 */
int session_is_recovering(const SESSION_T *session);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Determines if a session is closed.
 *
 * @param session		A session handle. A NULL value returns \ref DIFFUSION_TRUE.
 * @retval DIFFUSION_TRUE	If the session is closed.
 * @retval DIFFUSION_FALSE	If the session is not closed.
 */
int session_is_closed(const SESSION_T *session);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Takes a session ID and produces a human-readable string. It is the
 * responsibility of the caller to free the memory allocated for the returned
 * string.
 *
 * @param session_id	A \ref SESSION_ID_T. If the session ID is NULL, this function
 *                      returns NULL.
 * @retval "char *"	A string containing a textual representation of the
 *			session id.
 * @retval NULL		If an error occurs.
 */
char *session_id_to_string(const SESSION_ID_T *session_id);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Given a session id in string form, this function returns it to the
 * internal structure form.
 *
 * @param str			The session id as a string. If NULL, this function
 *                              returns NULL.
 * @retval "SESSION_ID_T *"	A populated session id structure.
 * @retval NULL			If an error occurs.
 */
SESSION_ID_T *session_id_create_from_string(const char *str);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Free memory associated with a session ID structure.
 *
 * @param session_id            The session id structure. If NULL, this function
 *                              has no effect.
 */
void session_id_free(SESSION_ID_T *session_id);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Compare two session IDs.
 *
 * @param s1 The first session ID to compare.
 * @param s2 The second session ID to compare.
 * @retval 0 if the session IDs are equal.
 * @retval 1 if only their server instances are equal.
 * @retval -1 otherwise.
 */
int session_id_cmp(const SESSION_ID_T s1, const SESSION_ID_T s2);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Sets the handler for topics received but not subscribed to.
 *
 * If a topic message is received that does not have a registered
 * handler, it is passed to the global topic handler. This function
 * allows you to override the default handler (which does nothing).
 *
 * @param session       The session handle. <em>MUST NOT</em> be NULL.
 * @param handler       Pointer to a function which will receive the
 *                      unhandled messages, or NULL to reset to the default
 *                      handler.
 * @retval              The previous topic handler.
 */
TOPIC_HANDLER_T set_global_topic_handler(SESSION_T *session, const TOPIC_HANDLER_T handler);

/**
 * @ingroup PublicAPI_Session
 *
 * @brief Sets the maximum size of the outbound queue.
 *
 * The session maintains a queue of messages which have been submitted
 * but not yet sent on the wire to the server. This function allows
 * the user to override the maximum size of this queue. If the
 * supplied queue size is smaller than the number of messages
 * currently on the queue, any further additions to the queue will
 * block until space becomes available.
 *
 * @param session       The session handle. <em>MUST NOT</em> be NULL.
 * @param size          The new queue capacity (in messages), or -1 for
 *                      unbounded.
 *
 * @retval The previous capacity.
 */
int session_set_maximum_outbound_queue_size(SESSION_T *session, int size);

#endif
