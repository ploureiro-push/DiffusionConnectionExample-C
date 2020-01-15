#ifndef _diffusion_topic_update_control_h_
#define _diffusion_topic_update_control_h_ 1

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
 * @file topic-update-control.h
 *
 * Topic update control feature.
 */

#include "session.h"
#include "datatypes.h"
#include "types/update_types.h"

/**
 * @brief Service states that an update source may be in.
 *
 * @remarks Updates should only be performed when the registered
 * source is an ACTIVE state.
 */
typedef enum {
        /// Initial source state, before state is received from the
        /// server.
        UPDATE_SOURCE_STATE_INIT = 0,
        /// Active state - the topic may be updated by this source.
        UPDATE_SOURCE_STATE_ACTIVE = 1,
        /// Terminal state; no further state changes may take place.
        UPDATE_SOURCE_STATE_CLOSED = 2,
        /// Inactive state that permits being set to Active or Closed.
        UPDATE_SOURCE_STATE_STANDBY = 3
} UPDATE_SOURCE_STATE_T;

/**
 * @brief Structure describing the response to a topic update
 * registration request.
 */
typedef struct svc_update_registration_response_s {
        /// The state that the updater is in.
        UPDATE_SOURCE_STATE_T state;
} SVC_UPDATE_REGISTRATION_RESPONSE_T;

/**
 * @brief Callback for register_update_source().
 *
 * @param session The current active session.
 * @param updater_id The updater_id identifying this updater.
 * @param response The registration response message from Diffusion.
 * @param context User-supplied context from the initial registration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_update_source_registration_cb)(SESSION_T *session,
                                                const CONVERSATION_ID_T *updater_id,
                                                const SVC_UPDATE_REGISTRATION_RESPONSE_T *response,
                                                void *context);

/**
 * @brief User-supplied handlers that may be invoked in response to a
 * request to register a topic source.
 */
#define UPDATE_SOURCE_REGISTRATION_HANDLERS                             \
        /** Topic has no state yet. */                                  \
        on_update_source_registration_cb on_init;                       \
        /** Update handler registered. */                               \
        on_update_source_registration_cb on_registered;                 \
        /** Update handler is active. */                                \
        on_update_source_registration_cb on_active;                     \
        /** Update handler is in standby mode. */                       \
        on_update_source_registration_cb on_standby;                    \
        /** Update handler is closed. */                                \
        on_update_source_registration_cb on_close;                      \
        /** Standard service error handler. */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler. */                        \
        DISCARD_HANDLER_T on_discard;

typedef struct update_source_registration_handlers_s {
        UPDATE_SOURCE_REGISTRATION_HANDLERS
} UPDATE_SOURCE_REGISTRATION_HANDLERS_T;

/**
 * @brief Structure passed when registering an update source.
 */
typedef struct update_source_registration_params_s {
        /// Callback handlers.
        UPDATE_SOURCE_REGISTRATION_HANDLERS
        /// Path under which to register as an update source.
        const char *topic_path;
        /// User-supplied context returned to callbacks.
        void *context;
} UPDATE_SOURCE_REGISTRATION_PARAMS_T;

/**
 * @brief Callback for deregiser_update_source().
 *
 * @param session The current active session.
 * @param response The deregistration response message from Diffusion.
 * @param context User-supplied context from the deregistration call.
 *
 * @return HANDLER_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_update_source_deregistration_cb)(SESSION_T *session,
                                                  const CONVERSATION_ID_T *updater_id,
                                                  void *context);

#define UPDATE_SOURCE_DEREGISTRATION_HANDLERS                           \
        on_update_source_deregistration_cb on_deregistered;             \
        /** Standard service error handler. */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler. */                        \
        DISCARD_HANDLER_T on_discard;

typedef struct update_source_deregistration_handlers_s {
        UPDATE_SOURCE_DEREGISTRATION_HANDLERS
} UPDATE_SOURCE_DEREGISTRATION_HANDLERS_T;

/**
 * @brief Structure passed when deregistering an update source.
 */
typedef struct update_source_deregistration_params_s {
        /// Callback handlers.
        UPDATE_SOURCE_DEREGISTRATION_HANDLERS
        /// Updater ID to deregister.
        const CONVERSATION_ID_T *updater_id;
        /// User-supplied context returned to callbacks.
        void *context;
} UPDATE_SOURCE_DEREGISTRATION_PARAMS_T;

/**
 * @brief Values indicating the result of an update request.
 */
typedef enum {
        /// The update request has been processed successfully.
        UPDATE_RESPONSE_OK = 0,
        /// The update was of a type that is not compatible with the topic type or
        /// the topic does not support updating.
        UPDATE_RESPONSE_INCOMPATIBLE_UPDATE = 1,
        /// The update failed, probably because the content sent with the update was
        /// invalid/incompatible with the topic type/data format.
        UPDATE_RESPONSE_UPDATE_FAILED = 2,
        /// The updater is not active.
        UPDATE_RESPONSE_INVALID_UPDATER = 3,
        /// The topic being updated does not exist.
        UPDATE_RESPONSE_MISSING_TOPIC = 4,
        /// Attempt to perform a non exclusive update on a topic branch that already
        /// has an update source.
        UPDATE_RESPONSE_EXCLUSIVE_UPDATER_CONFLICT = 7,
        /// An update has been attempted on a topic that has no value.
        UPDATE_RESPONSE_DELTA_WITHOUT_VALUE = 8,
        /// An update could not be applied because the cluster was repartitioning.
        UPDATE_RESPONSE_CLUSTER_REPARTITION = 9,
        /// An update could not be performed because the topic is managed by a
        /// component (e.g fan-out) that prohibits updates from the caller.
        UPDATE_RESPONSE_INCOMPATIBLE_STATE = 10
} UPDATE_RESPONSE_STATUS_T;

/**
 * @brief Structure describing the response to a topic update request.
 */
typedef struct svc_update_response_s {
        /// The result of the update.
        UPDATE_RESPONSE_STATUS_T status;
} SVC_UPDATE_RESPONSE_T;

/**
 * @brief User-supplied handlers that are invoked in response to
 * sending an update request to a Diffusion topic.
 */
#define UPDATE_SOURCE_HANDLERS                                          \
        /** Callback indicating that the update succeeded. */           \
        int (*on_success)(SESSION_T *session, const CONVERSATION_ID_T *updater_id, const SVC_UPDATE_RESPONSE_T *response, void *context); \
        /** Callback indicating that the update failed. */              \
        int (*on_failure)(SESSION_T *session, const CONVERSATION_ID_T *updater_id, const SVC_UPDATE_RESPONSE_T *response, void *context); \
        /** Standard service error handler. */                          \
        ERROR_HANDLER_T on_error;                                       \
        /** Standard service discard handler. */                        \
        DISCARD_HANDLER_T on_discard;

typedef struct update_source_handlers_s {
        UPDATE_SOURCE_HANDLERS
} UPDATE_SOURCE_HANDLERS_T;

/**
 * @brief Structure passed when updating a topic.
 */
typedef struct update_source_params_s {
        UPDATE_SOURCE_HANDLERS
        /// Updater ID returned from a successful registration.
        const CONVERSATION_ID_T *updater_id;
        /// Name of the topic to update.
        const char *topic_path;
        /// The update to apply to the topic.
        const UPDATE_T *update;
        /// User-supplied context returned to callbacks.
        void *context;
} UPDATE_SOURCE_PARAMS_T;

typedef struct update_value_params_s {
        UPDATE_SOURCE_HANDLERS
        CONVERSATION_ID_T *updater_id;
        char *topic_path;
        BUF_T *data;
        void *context;
} UPDATE_VALUE_PARAMS_T;

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * Create a new UPDATE_T structure for updating topic content.
 *
 * @param action        The operation that will be performed on the target topic data
 *                      (refresh, update, match).
 * @param type          The type of update being performed (typically, Content change).
 * @param content       The data to be sent to Diffusion.
 * @return              A new UPDATE_T structure, or NULL on error.
 */
DEPRECATED(UPDATE_T *update_create(const UPDATE_ACTION_T action, const UPDATE_TYPE_T type, const CONTENT_T *content));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * Free all memory associated with an UPDATE_T.
 *
 * @param update        The UPDATE_T to be freed.
 */
DEPRECATED(void update_free(UPDATE_T *update));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * Serialise an UPDATE_T to bytes for transmission to Diffusion.
 *
 * @param update        The UPDATE_T to be mashalled.
 * @return              A pointer to a newly allocated BUF_T containing the
 *                      serialised data, or NULL if an error occurs.
 */
DEPRECATED(BUF_T *update_marshal(const UPDATE_T *update));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * Read an UPDATE_T structure from a char array.
 *
 * @param data          The char array containing the serialised UPDATE_T
 * @param update        The location to store a newly allocated UPDATE_T
 * @return              A pointer to the first char following the serialised
 *                      UPDATE_T in the char array, or NULL on error.
 */
DEPRECATED(char *buf_read_update(const char *data, UPDATE_T **update));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * Deserialise bytes from Diffusion to an UPDATE_T structure.
 *
 * @param str           The bytes to be unmarshalled.
 * @return              A pointer to a newly allcoated UPDATE_T, or NULL if an
 *                      error occurs.
 */
DEPRECATED(UPDATE_T *update_unmarshal(const char *str));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * Create a deep copy of an UPDATE_T structure.
 *
 * @param src           The UPDATE_T to copy.
 * @return              A newly allocated UPDATE_T, or NULL on error.
 */
DEPRECATED(UPDATE_T *update_dup(const UPDATE_T *src));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * @brief Register a topic update source.
 *
 * Registers an update source with Diffusion, requesting
 * responsibility for managing topic state for all topics from a given
 * point and below.
 *
 * @param session       The session handle. If NULL, this function returns
 *                      immediately with a NULL value.
 * @param params        Parameter structure describing the registration
 *                      request.
 * @return              A conversation ID that can be used as a handle to
 *                      future topic updates by this update source, or NULL
 *                      if the supplied session is NULL.
 */
DEPRECATED(CONVERSATION_ID_T *register_update_source(SESSION_T *session, const UPDATE_SOURCE_REGISTRATION_PARAMS_T params));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release.
 *
 * @brief Deregister a topic update source.
 *
 * Deregisters an existing update source from Diffusion.
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameter structure describing the deregistration
 *                      request.
 */
DEPRECATED(void deregister_update_source(SESSION_T *session, const UPDATE_SOURCE_DEREGISTRATION_PARAMS_T params));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release. Applications should now use the preferred
 * update API in topic-update.h
 *
 * @brief Update topic content.
 *
 * Sends new topic content to Diffusion, for transmission to subscribed
 * clients. Where possible, prefer update_value() to update().
 *
 * @param session       The session handle. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which topic to update,
 *                      the new content, how the update is to be applied, and
 *                      callbacks to handle success or failure notifications.
 */
DEPRECATED(void update(SESSION_T *session, const UPDATE_SOURCE_PARAMS_T params));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release. Applications should now use the preferred
 * update API in topic-update.h
 *
 * @brief Update topic content
 *
 * Updates the content of a topic of a certain type (binary, JSON) but unlike
 * update(), this may transparently send delta updates which require less
 * network bandwidth. If possible, prefer this function to update().
 *
 * @param session	The session handle. If NULL, this function returns immediately.
 * @param params        Parameter structure describing which topic to update, the value
 *                      that the topic on the server will contain and callbacks to
 *                      handle success or failure notifications.
 */
DEPRECATED(void update_value(SESSION_T *session, const UPDATE_VALUE_PARAMS_T params));

/**
 * @deprecated This function is deprecated from version 6.2 onwards, and will
 * be removed in a future release. Applications should now use the preferred
 * update API in topic-update.h
 *
 * @brief Update topic content
 *
 * Updates the content of a topic of a provided datatype. This is the preferred function
 * for updating datatype topics (Binary, JSON, String, Int64, Double and Recordv2)
 *
 * @param session	The session handle. If NULL, this function returns immediately.
 * @param datatype      `DIFFUSION_DATATYPE` of the topic to be updated.
 * @param params        Parameter structure describing which topic to update, the value
 *                      that the topic on the server will contain and callbacks to
 *                      handle success or failure notifications.
 */
DEPRECATED(void update_value_with_datatype(SESSION_T *session, DIFFUSION_DATATYPE datatype, UPDATE_VALUE_PARAMS_T params));

#endif
