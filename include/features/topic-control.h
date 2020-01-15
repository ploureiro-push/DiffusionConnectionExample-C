#ifndef _diffusion_topic_control_h_
#define _diffusion_topic_control_h_ 1

/*
 * Copyright © 2014, 2019 Push Technology Ltd., All Rights Reserved.
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
 * @file topic-control.h
 *
 * Topic control feature.
 */

#include "session.h"

#include "misc/deprecate.h"
#include "types/content_types.h"
#include "types/topic_details_types.h"
#include "topic-details.h"
#include "topic-specification.h"
#include "hash.h"

typedef enum {
        /// No error while adding topic.
        ADD_TOPIC_FAILURE_REASON_SUCCESS = 0,
        /// Topic already existed with exactly the same details.
        ADD_TOPIC_FAILURE_REASON_EXISTS = 1,
        /// Topic already exists with the same name but different
        /// details.
        ADD_TOPIC_FAILURE_REASON_EXISTS_MISMATCH =  2,
        /// The name of the topic was found to be invalid.
        ADD_TOPIC_FAILURE_REASON_INVALID_NAME =  3,
        /// Some aspect of the details failed validation.
        ADD_TOPIC_FAILURE_REASON_INVALID_DETAILS =  4,
        /// @deprecated This failure reason is deprecated from version 6.2
        /// onwards and will be removed in a future release.
        ///
        /// A user supplied class could either not be found or could
        /// not be instantiated at the server.
        ADD_TOPIC_FAILURE_REASON_USER_CODE_ERROR =  5,
        /// The client was denied permission to add a topic of the
        /// specified type a the given point in the topic hierarchy.
        ADD_TOPIC_FAILURE_REASON_PERMISSIONS_FAILURE =  7,
        /// Initialisation of topic with the supplied content failed,
        /// possibly because it was of the wrong format.
        ADD_TOPIC_FAILURE_REASON_INITIALISE_ERROR =  8,
        /// An unexpected error occurred whilst attempting to create
        /// the topic.
        ADD_TOPIC_FAILURE_REASON_UNEXPECTED_ERROR =  9
} SVC_ADD_TOPIC_FAILURE_REASON_T;

typedef enum {
        // Result code denoting the topic was
        // sucessfully added.
        TOPIC_ADD_RESULT_CREATED = 0,
        // Result code denoting the topic
        // already exists.
        TOPIC_ADD_RESULT_EXISTS = 1
} TOPIC_ADD_RESULT_CODE;

typedef enum {
        /// A topic already exists with the same path but a different specification.
        TOPIC_ADD_FAIL_EXISTS_MISMATCH = 1,
        /// The supplied topic path is invalid.
        TOPIC_ADD_FAIL_INVALID_NAME = 2,
        /// Some aspect of the topic specification failed validation.
        TOPIC_ADD_FAIL_INVALID_DETAILS = 3,
        /// The client was denied permission to add a topic of the specified type at
        /// the given point in the topic hierarchy.
        TOPIC_ADD_FAIL_PERMISSIONS_FAILURE = 4,
        /// An unexpected error occurred whilst attempting to create the topic. See
        /// the server log for more details.
        TOPIC_ADD_FAIL_UNEXPECTED_ERROR = 5,
        /// Adding the topic failed because the cluster was repartitioning due to a
        /// server starting, stopping, or failing. The operation can be retried.
        TOPIC_ADD_FAIL_CLUSTER_REPARTITION = 6,
        /// Adding the topic failed because of a license limit.
        TOPIC_ADD_FAIL_EXCEEDED_LICENSE_LIMIT = 7,
        /// Adding the topic failed because a topic owned by a publisher is already
        /// bound to the parent path.
        TOPIC_ADD_FAIL_INCOMPATIBLE_PARENT = 8,
        /// @deprecated Slave topics are deprecated. This value will be removed in
        /// a future release.
        /// Adding a slave topic failed because a topic owned by a publisher is
        /// already bound to the specified master path.
        TOPIC_ADD_FAIL_INCOMPATIBLE_MASTER = 9,
        /// Adding the topic failed because a topic is already bound to the specified
        /// path but the caller does not have the rights to manage it.
        TOPIC_ADD_FAIL_EXISTS_INCOMPATIBLE = 10
} TOPIC_ADD_FAIL_RESULT_CODE;

/**
 * @brief Callback for add_topic_from_specification().
 *
 * @param session     The current active session.
 * @param result_code The result code from the topic add request.
 * @param context     User-supplied context from the initial
 *                    add_topic_from_specification() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_add_cb)(SESSION_T *session,
                               TOPIC_ADD_RESULT_CODE result_code,
                               void *context);

/**
 * @brief Callback for add_topic_from_specification().
 *
 * @param session     The current active session.
 * @param result_code The topic add failure result code
 * @param error       The error reason. This will only be set
 *                    if this function is called through the
 *                    callback
 * @param context     User-supplied context from the initial
 *                    add_topic_from_specification() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_add_failed_cb)(SESSION_T *session,
                                      TOPIC_ADD_FAIL_RESULT_CODE result_code,
                                      const DIFFUSION_ERROR_T *error,
                                      void *context);

/**
 * add_topic_from_specification callback
 */
typedef struct add_topic_callback_s {
        /**
         * Topic added successfully (or already exists).
         */
        on_topic_add_cb on_topic_added_with_specification;
        /**
         * Topic failed to be added.
         */
        on_topic_add_failed_cb on_topic_add_failed_with_specification;
        /** 
         * Standard service error handler.
         */
        ERROR_HANDLER_T on_error;
        /**
         * Standard conversation discard callback.
         */
        DISCARD_HANDLER_T on_discard;
        /**
         * Context. Can be NULL.
         */
        void *context;
} ADD_TOPIC_CALLBACK_T;

/**
 * @brief Structure describing a "topic removal" response.
 */
typedef struct svc_topic_removal_response_s {
        /// @cond HIDDEN
        void *_reserved;
        /// @endcond
} SVC_TOPIC_REMOVAL_RESPONSE_T;

/**
 * @brief Callback for topic_removal() handlers.
 *
 * @param session The current active session.
 * @param response The response message from Diffusion.
 * @param context User-supplied context from the topic_removal() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_removal_cb)(SESSION_T *session,
                                   const SVC_TOPIC_REMOVAL_RESPONSE_T *response,
                                   void *context);

/**
 * @brief User-specified callbacks that may be invoked in response to
 * issuing a "topic removal" request to Diffusion.
 */
#define TOPIC_REMOVAL_HANDLERS \
        /** Topic removed. */                                           \
        on_topic_removal_cb on_removed;                                 \
        /** Standard service error handler. */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler. */                        \
        DISCARD_HANDLER_T on_discard;

typedef struct topic_removal_handlers_s {
        TOPIC_REMOVAL_HANDLERS
} TOPIC_REMOVAL_HANDLERS_T;

/**
 * @brief Structure passed to topic_removal().
 */
typedef struct topic_removal_params_s {
        TOPIC_REMOVAL_HANDLERS
        /// Selector describing topics to remove.
        const char *topic_selector;
        /// User-supplied context returned to callbacks.
        void *context;
} TOPIC_REMOVAL_PARAMS_T;

/**
 * @brief Structure describing a "remove topic" response.
 */
typedef struct svc_remove_topics_response_s {
        /// @cond HIDDEN
        void *_reserved;
        /// @endcond
} SVC_REMOVE_TOPICS_RESPONSE_T;

/**
 * @brief Callback for remove_topics() handlers.
 *
 * @param session The current active session.
 * @param response The response message from Diffusion.
 * @param context User-supplied context from the remove_topics() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_remove_topics_cb)(SESSION_T *session,
                                   const SVC_REMOVE_TOPICS_RESPONSE_T *response,
                                   void *context);

/**
 * @brief User-specified callbacks that may be invoked in response to
 * issuing a "remove topic" request to Diffusion.
 */
#define REMOVE_TOPICS_HANDLERS \
        /** Topic removed. */                                           \
        on_remove_topics_cb on_removed;                                 \
        /** Standard service error handler. */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler. */                        \
        DISCARD_HANDLER_T on_discard;

typedef struct remove_topics_handlers_s {
        REMOVE_TOPICS_HANDLERS
} REMOVE_TOPICS_HANDLERS_T;

/**
 * @brief Structure passed to remove_topics().
 */
typedef struct remove_topics_params_s {
        REMOVE_TOPICS_HANDLERS
        /// Selector describing topics to remove.
        const char *topic_selector;
        /// User-supplied context returned to callbacks.
        void *context;
} REMOVE_TOPICS_PARAMS_T;

/**
 * @brief callback for session_wills_remove_topics().
 *
 * @param session The current active session.
 * @param topic_path The path on which the will has been registered.
 * @param context User-supplied context from the initial session_wills_remove_topics() call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_session_will_registration_cb)(SESSION_T *session,
                                               const char *topic_path,
                                               void *context);

/**
 * @brief User callbacks that may be defined when registering a session will
 * pertaining to topic removal.
 */
#define SESSION_WILLS_REMOVE_TOPIC_HANDLERS                             \
        /** Callback invoked on successful registration of the session will. */ \
        on_session_will_registration_cb on_registered;                  \
        /** Callback invoked if the session is closed during registration. */ \
        on_session_will_registration_cb on_close;                       \
        /** Standard service error callback. */                         \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard callback. */                       \
        DISCARD_HANDLER_T on_discard;

typedef struct session_wills_remove_topic_handlers_s {
        SESSION_WILLS_REMOVE_TOPIC_HANDLERS
} SESSION_WILLS_REMOVE_TOPIC_HANDLERS_T;

/**
 * @brief Structure supplied when registering a topic will to remove topics.
 */
typedef struct session_wills_remove_topic_params_s {
        SESSION_WILLS_REMOVE_TOPIC_HANDLERS
        /// Path to remove when this session will is invoked.
        const char *topic_path;
        /// User-supplied context returned to callbacks.
        void *context;
} SESSION_WILLS_REMOVE_TOPIC_PARAMS_T;

/**
 * @brief Structure of a request to register for missing topic notifications.
 */
typedef struct svc_missing_topic_request_s {
        /// The session id of the client making the request for the topic.
        SESSION_ID_T *session_id;
        /// Register for notifications of missing topics that match
        /// this topic selector.
        char *topic_selector;
        /// The conversation ID to use in the response message.
        CONVERSATION_ID_T *conversation_id;
        /// Internal - action to take on this request.
        int action;
} SVC_MISSING_TOPIC_REQUEST_T;

/**
 * @brief Callback for missing_topic_register_handler().
 *
 * @param session The current active session.
 * @param request The incoming notification message.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_missing_topic_cb)(SESSION_T *session,
                                   const SVC_MISSING_TOPIC_REQUEST_T *request,
                                   void *context);

/**
 * @brief Callback handlers for missing topic notification messages.
 */
#define MISSING_TOPIC_HANDLERS                                          \
        /** Callback when a client requests a topic that doesn't exist. */ \
        on_missing_topic_cb on_missing_topic;                           \
        /** Standard service error handler. */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler. */                        \
        DISCARD_HANDLER_T on_discard;

typedef struct missing_topic_handlers_s {
        MISSING_TOPIC_HANDLERS
} MISSING_TOPIC_HANDLERS_T;

/**
 * @brief Structure supplied when registering to receive missing topic
 * notifications.
 */
typedef struct missing_topic_params_s {
        MISSING_TOPIC_HANDLERS
        /// Receive notifications for missing topics under this topic.
        const char *topic_path;
        /// User-supplied context return to callbacks.
        void *context;
} MISSING_TOPIC_PARAMS_T;

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Add a topic to Diffusion.
 * 
 * @param session       The current session. If NULL, this function returns immediately.
 * @param topic_path    The topic path to be added.
 * @param specification The topic specification
 * @param callback      Struct containing pointers to callback functions will be called when
 *                      a response to the request is received from the server.
 */
void add_topic_from_specification(SESSION_T *session,
                                  const char *topic_path,
                                  const TOPIC_SPECIFICATION_T *specification,
                                  const ADD_TOPIC_CALLBACK_T callback);

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Remove topics from Diffusion.
 *
 * Send a request to remove one or more topics at the server.  The topics to
 * remove will depend upon the nature of the topic selector specified. If the
 * selector does not have descendant pattern qualifiers (i.e. / or //), only
 * those topics that exist at paths indicated by the selector will be removed
 * and not their descendants. If a single / qualifier is specified, all
 * descendants of the matching topic paths will be removed. If // is
 * specified, all branches of the topic tree that match the selector (i.e
 * topics at the selected paths and all descendants of the selected paths)
 * will be removed.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which topics to remove
 *                      and callbacks to handle success or failure
 *                      notifications.
 */
void topic_removal(SESSION_T *session, const TOPIC_REMOVAL_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Remove topics from Diffusion.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which topics to remove
 *                      and callbacks to handle success or failure
 *                      notifications.
 */
void remove_topics(SESSION_T *session, const REMOVE_TOPICS_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Register a topic removal session will.
 *
 * An application should call this function to register a session
 * will, whereby the specified topics are removed on session
 * disconnection.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param params        Parameters describing the session will registration request
 *                      and the callback handlers which may be invoked in response.
 */
void session_wills_remove_topics(SESSION_T *session, const SESSION_WILLS_REMOVE_TOPIC_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Register to receive notifications about missing topics that
 * clients attempt to fetch/subscribe.
 *
 * Missing topic notifications from a 'fetch' operation only occur when using the deprecated
 * `fetch` mechanism. The newer `diffusion_fetch_request_fetch` mechanism does not
 * support missing topic notifications.
 *
 * Register a handler for receiving missing topic notifications when a client
 * attempts to fetch or subscribe to a topic underneath a given root topic, but
 * that requested topic doesn't exist.
 *
 * @param session       The current session. If NULL, this function returns immediately
 *                      with the value NULL.
 * @param params        Parameters defining the registration request.
 * @return              A conversation id that can be used to deregister the
 *                      handler at a later time, or NULL if the supplied session is NULL.
 */
CONVERSATION_ID_T *missing_topic_register_handler(SESSION_T *session, const MISSING_TOPIC_PARAMS_T params);

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Cease receiving missing topic notifications.
 *
 * @param session       The current session. If NULL, this function returns immediately.
 * @param conversation_id       The conversation id returned from a previous
 *                              call to missing_topic_register_handler().
 */
void missing_topic_deregister_handler(SESSION_T *session,
                                      const CONVERSATION_ID_T *conversation_id);

/**
 * @ingroup PublicAPI_TopicControl
 *
 * @brief Proceed (retry) after a processing a missing topic notification.
 *
 * To be called in a missing topic notification handler, this notifies
 * Diffusion that it may complete processing of the original request.
 *
 * @param session       The current session.
 * @param request       The request which gave rise to the missing topic
 *                      notification.
 */
void missing_topic_proceed(SESSION_T *session,
                           SVC_MISSING_TOPIC_REQUEST_T *request);

/**
 * @deprecated Since 6.4. This is only useful when using the deprecated
 * `fetch` mechanism. It will be removed when that mechanism is removed.
 *
 * @brief Cancel the operation leading to this missing topic notification.
 *
 * To be called in a missing topic notification handler, this notifies
 * Diffusion that it should not continue processing the original request.
 *
 * @param session       The current session.
 * @param request       The request which gave rise to the missing topic
 *                      notification.
 */
DEPRECATED(void missing_topic_cancel(SESSION_T *session,
                          SVC_MISSING_TOPIC_REQUEST_T *request));

#endif