/**
 * @file topic-specification.h
 *
 * Functions used for working with topic specifications (TOPIC_SPECIFICATION_T).
 *
 * Copyright © 2018, 2019 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

#ifndef _diffusion_topic_specification_h_
#define _diffusion_topic_specification_h_ 1

#include "types/topic_details_types.h"
#include "hash.h"

typedef struct TOPIC_SPECIFICATION_T TOPIC_SPECIFICATION_T;

/**
 * Key of the topic property that specifies whether a topic should publish
 * only values.
 *
 * Setting `PUBLISH_VALUES_ONLY` to "true" disables this behavior
 * so that deltas are never published. Doing so is not recommended because
 * it will result in more data being transmitted and less efficient use of
 * network resources. This property is "false" by default.
 */
extern const char *const DIFFUSION_PUBLISH_VALUES_ONLY;

/**
 * @deprecated Slave topics are deprecated. This value will be removed in
 * a future release.
 *
 * Key of the topic property that specifies the master topic path for a
 * `TOPIC_TYPE_SLAVE` topic.
 *
 * When creating a slave topic using a topic specification then this must be
 * specified. For all other topic types it is ignored.
 */
extern const char *const DIFFUSION_SLAVE_MASTER_TOPIC;

/**
 * Key of the topic property that specifies a schema which constrains topic
 * values.
 *
 * This property is only used by `TOPIC_TYPE_RECORDV2` topics.
 */
extern const char *const DIFFUSION_SCHEMA;

/**
 * Key of the topic property that specifies the 'tidy on unsubscribe' option
 * for a topic.
 *
 * By default, if a session unsubscribes from a topic, it will receive any
 * updates for that topic that were previously queued but not sent.
 * If this property is set to "true", when a session unsubscribes from the
 * topic, any updates for the topic that are still queued for the session
 * are removed. This property is "false" by default.
 */
extern const char *const DIFFUSION_TIDY_ON_UNSUBSCRIBE;

/**
 * Key of the topic property indicating whether a topic should validate
 * inbound values.
 *
 * By default, the server does not validate received values before sending
 * them on to client sessions. If this property is set to "true",
 * the server will perform additional validation on values to check that
 * they are valid instances of the data type, and if it is not then it will
 * return an error to the updater and not update the topic.
 * This property is "false" by default.
 */
extern const char *const DIFFUSION_VALIDATE_VALUES;

/**
 * Key of the topic property that specifies a topic should not retain its
 * last value.
 *
 * This property is false by default.
 */
extern const char *const DIFFUSION_DONT_RETAIN_VALUE;

/**
 * Key of the topic property that can be used to prevent a topic from being
 * persisted when the server is configured to enable persistence.
 *
 * By default, a topic will be persisted if persistence is enabled at the
 * server and the topic type supports persistence. Setting `PERSISTENT` to "false"
 * will prevent the topic from being persisted. This property is "true" by default.
 */
extern const char *const DIFFUSION_PERSISTENT;

/**
 * Key of the topic property that specifies a removal policy for automatic
 * removal of the topic (and/or other topics).
 *
 * This property is specified as an expression which defines one or more
 * conditions that are to be satisfied before automatic removal occurs.
 * The expression takes the form:-
 *
 * <code>when 'conditions' remove 'selector'</code>
 *
 * At least one condition must be supplied. If more than one is supplied,
 * they must be separated by logical operators 'and' or 'or'.
 * The natural evaluation order of the operators may be changed by
 * surrounding with parentheses
 *
 * The 'remove' clause is optional. It provides a
 * topic selector expression representing the topics to be removed.
 * If a 'remove' clause is specified, the topic with the removal
 * policy will only be removed if its path matches the selector expression.
 * The selector must be surrounded by either double or single quotes.
 *
 * The permissions that are applied at the time of removal are those defined
 * by the roles of the principal that created the topic at the time of
 * creation. The roles of that principal may therefore change before the
 * removal with no effect, but if the permissions given to the roles change
 * it may have an effect upon the final removal.
 *
 * Only one occurrence of each of the following 4 condition types may be
 * included within the expression:-
 *
 * 1) `time after <absoluteTime>`

 * Removal should occur after a specified absolute time. The <absoluteTime>
 * may be specified as as a number of milliseconds since the epoch (00:00:00
 * on 1 January 1970) or as a quoted date and time formatted in RFC_1123 date
 * time format. Either single or double quotes may be used.
 *
 * 2) `subscriptions < n for <period> [after <period>]`

 * Removal should occur when the topic has had less than the specified
 * number (n) of subscriptions for a given period of time. Optionally, an
 * 'after' period may be specified by which to delay the initial checking of
 * this condition. See below for period formats.
 *
 * 3) `no updates for <period> [after <period>]`
 *
 * Removal should occur when the topic has had no updates for a given period
 * of time. Optionally, an 'after' period may be specified by which to delay
 * the initial checking of this condition. See below for period formats.
 *
 * 4) `no session has "criteria" [for <period>] [after <period>]`
 *
 * Removal should occur when there are no sessions satisfying certain
 * criteria. Optionally, the criteria can be required to be satisfied 'for' a
 * period of time. Optionally, an 'after' period can be specified to delay the
 * initial check of the criteria.
 *
 * Session selection criteria are specified as defined for session filters and
 * must be surrounded by single or double quotes. See below for period formats.
 * A shorthand form of this is 'this session closes' which may be used to
 * indicate that the topic is to be removed when the session that created it
 * closes.
 *
 * Time periods are specified as a number followed (with no intermediate space)
 * by a single letter representing the time unit. The time unit may be 's'
 * (seconds), 'm' (minutes), 'h' (hours) or 'd' (days). For example 10 minutes
 * would be specified as 10m.
 *
 * If quotes or backslashes (\) are required within quoted values such as
 * selectors or session criteria then they may be escaped by preceding with \.
 *
 * The expression is validated only by the server and therefore if an invalid
 * expression is specified it will be reported as an invalid topic specification
 * error when creating the topic.
 *
 * Examples:
 *
 * `when time after 1518780068112`
 *
 * The topic will be removed when the date and time indicated by the
 * specified number of milliseconds since the epoch has passed.
 *
 * `when time after "Tue, 3 Jun 2018 11:05:30 GMT"`
 *
 * The topic will be removed when the specified date and time has passed.
 *
 * `when time after "Tue, 3 Jun 2018 11:05:30 GMT" remove "*alpha/beta//"`
 *
 * The topic alpha/beta and all topics subordinate to it will be removed
 * when the specified date and time has passed.
 *
 * `when subscriptions < 1 for 20m`
 *
 * The topic will be removed when it has had no subscriptions for a
 * continuous period of 20 minutes.
 *
 * `when subscriptions < 2 for 20m after 1h`
 *
 * The topic will be removed when it has had less than 2 subscriptions for a
 * continuous period of 20 minutes after one hour has passed since its
 * creation.
 *
 * `when no updates for 3h`
 *
 * The topic will be removed when it has had no updates for a continuous
 * period of 3 hours.
 *
 * `when no updates for 15m after 1d`
 *
 * The topic will be removed when it has had no updates for a continuous
 * period of 15 minutes after one day has passed since its creation.
 *
 * `when this session closes`
 *
 * The topic will be removed when the session that created it is closed.
 *
 * <code>when no session has "$Principal is 'Alice'"</code>
 *
 * The topic will be removed when there have been no sessions with the
 * principal 'Alice' for a continuous period of 10 minutes.
 *
 * <code>when no session has "$Principal is 'Alice'" for 10m</code>
 *
 * The topic will be removed when there have been no sessions with the
 * principal 'Alice' for a continuous period of 10 minutes.
 *
 * <code>when no session has 'Department is "Accounts"' for 30m after 2h</code>
 *
 * The topic will be removed when there have been no sessions from the
 * Accounts department for a continuous period of 30 minutes after 2 hours
 * have passed since its creation.
 *
 * `when time after "Tue, 3 Jun 2018 11:05:30 GMT" and subscriptions < 1 for 30m`
 *
 * The topic will be removed when the specified date and time has passed and
 * the topic has had no subscriptions for a continuous period of 30 minutes
 * after that time.
 *
 * `when time after "Tue, 3 Jun 2018 11:05:30 GMT" and subscriptions < 2 for 10m after 1h`
 *
 * The topic will be removed when the specified date and time has passed and
 * the topic has had less than 2 subscriptions for a continuous period of 10
 * minutes after that time plus one hour.
 *
 * `when time after "Tue, 3 Jun 2018 11:05:30 GMT" or subscriptions < 2 for 10m after 1h`
 *
 * The topic will be removed when the specified date and time has passed or
 * the topic has had less than 2 subscriptions for a continuous period of 10
 * minutes after one hour from its creation.
 *
 * `when time after "Tue, 3 Jun 2018 11:05:30 GMT" and (subscriptions < 2 for 10m after 1h or no updates for 20m)`
 *
 * The topic will be removed when the specified date and time has passed and
 * either the topic has had less than 2 subscriptions for a continuous
 * period of 10 minutes after that time plus one hour or it has had no
 * updates for a continuous period of 20 minutes. Note that the parentheses
 * are significant here as without them the topic would be removed if it had
 * had no updates for 20 minutes regardless of the time and subscriptions
 * clause.
 *
 * Notes and restrictions on use:
 *
 * The '`after`' time periods refer to the period since the topic was created or
 * restored from persistence store after a server is restarted. They are designed
 * as a 'grace' period after the topic comes into existence before the related
 * condition starts to be evaluated. When not specified the conditions start to be
 * evaluated as soon as the topic is created or restored.
 *
 * The server will evaluate conditions on a periodic basis (every few seconds) so
 * the exact removal time will not be precise for low periodic granularity.
 *
 * The meaning of the '`for`' period on '`no session has`' conditions is subtly
 * different from on other conditions. It does not guarantee that there has been
 * no session satisfying the condition at some point between evaluations, only that
 * when evaluated the given period of time has passed since it was last evaluated
 * and found to have no matching sessions.
 *
 * Subscriptions is the number of subscriptions to a topic, including those that
 * occur through routing or slave topics.
 *
 * Automatic topic removal is supported for both replicated topics and topics
 * fanned out to secondary servers using the fan-out feature. A 'subscriptions
 * less than' condition for such topics will be evaluated against the total number
 * of subscriptions to the topic across the cluster and on all downstream secondary
 * servers. A 'no session has' condition will consider all sessions hosted across a
 * cluster and hosted by downstream secondary servers.
 */
extern const char *const DIFFUSION_REMOVAL;

/**
 * Key of the topic property that allows the creator of a topic to extend
 * `READ_TOPIC`, `MODIFY_TOPIC`, and `UPDATE_TOPIC` permissions to a specific
 * principal, in addition to the permissions granted by the authorisation
 * rules in the security store.
 *
 * A session that has authenticated using the principal can update and
 * remove the topic, so the principal can be considered the topic owner. To
 * fetch or subscribe to the topic, the principal must also be granted
 * `SELECT_TOPIC` by the security store rules.
 *
 * This may be used in the following cases:
 * 1) A session creates a topic and makes its own principal the owner.
 * 2) A session creates a topic and makes another principal the owner.
 *
 * The format of the property value is:-
 *
 * `$Principal is "name"`
 *
 * where 'name' is the name of the principal. Single quotes may be used
 * instead of double quotes.
 *
 * The purpose of this property is to allow a client to create topics on
 * behalf of other users. This can be used in conjunction with the
 * `DIFFUSION_REMOVAL` property so that such topics are removed when there are
 * no longer any sessions for the named principal.
 */
extern const char *const DIFFUSION_OWNER;

/**
 * Key of the topic property that describes the conflation policy of the
 * topic. The policy specifies how the server manages queued topic updates.
 * Conflation is applied individually to each session queue.
 *
 * Conflation is the process of merging or discarding topic updates queued
 * for a session to reduce the server memory footprint and network data. The
 * server will conflate sessions that have a large number of queued messages
 * to meet configured queue size targets. The sessions with the largest
 * queues are typically slow consumers or have been disconnected – both will
 * benefit from conflation. This property allows conflation behaviour to be
 * tuned on a topic-by-topic basis.
 *
 * Supported policies are:
 *
 * - `off`
 * - `conflate`
 * - `unsubscribe`
 * - `always`
 *
 * The default policy used when the property is not specified and the topic
 * type is not time series is `conflate`. The default policy used when the
 * property is not specified and the topic type is time series is `off`.
 *
 * The policy `off` disables conflation for the topic. This policy disables
 * all conflation for the topic, so topic updates will never be merged or
 * discarded.
 *
 * The policy `conflate` automatically conflates topic updates when back
 * pressure is detected by the server. This policy is ignored for stateless,
 * single value and record topics, and for slave and routing topics that
 * reference stateless, single value and record topics.
 *
 * The policy `unsubscribe` automatically unsubscribes the topic when back
 * pressure is detected by the server. The unsubscription is not persisted
 * to the cluster, if a session fails over to a different server it will be
 * resubscribed to the topic.
 *
 * The policy `always` automatically conflates topic updates as they are
 * queued for the session. This is an eager policy that ensures only the
 * latest update is queued for the topic, minimising the server memory and
 * network bandwidth used by the session. This policy is ignored for
 * stateless, single value and record topics, and for slave and routing
 * topics that reference stateless, single value and record topics.
 *
 * The `conflate` and `unsubscribe` policies are applied when the server
 * detects back pressure for a session. The server configuration places
 * limits on the data queued for each session. If these limits are breached,
 * the server will conflate the session queue to attempt to reduce its size.
 * If the session queue still exceeds the limits after conflation, the
 * session will be terminated.
 *
 * Conflation of stateless, single value and record topics is configured
 * using server-side configuration. This configuration describes how topic
 * updates should be merged. Like the `always` policy conflation specified
 * this way is applied when queuing a topic update. The policy `off`
 * prevents this conflation being applied. All other policies allow
 * conflation specified this way to happen. The `unsubscribe` policy will
 * still unsubscribe topics that use conflation specified this way.
 *
 * The policies `conflate` and `always` are not supported for
 * time series topics as they would cause missing events. Attempts to enable
 * these policies with time series topics will cause the creation of the
 * topic to fail, reporting that the specification is invalid.
 */
extern const char *const DIFFUSION_CONFLATION;

/**
 * Key of the topic property that allows the compression policy to be set
 * on a per-topic basis.
 *
 * Compression reduces the bandwidth required to broadcast topic updates to
 * subscribed sessions, at the cost of increased server CPU.
 *
 * Changes to a topic's value are published to each subscribed session as a
 * sequence of topic messages. A topic message can carry the latest value or
 * the difference between the latest value and the previous value (a delta).
 * The compression policy determines if and how published topic messages
 * are compressed. Topic messages are not exposed through the client API;
 * the client library handles decompression and decodes deltas
 * automatically, passing reconstructed values to the application.
 *
 * The compression policy for a topic is specified by setting this property
 * to one of several values:
 *
 * - `off`
 * - `low`
 * - `medium`
 * - `high`
 *
 * The policies are listed in the order of increasing compression and
 * increasing CPU cost. `off` disables compression completely for the
 * topic and requires no additional CPU; `high` compresses the topic
 * messages to the smallest number of bytes, but has the highest CPU cost.
 * Generally some compression is beneficial, so the default value for this
 * property is `low`.
 *
 * Prior to version 6.4, only two values were allowed: `true`
 * (equivalent to `medium`, and the previous default policy) and
 * `false` (equivalent to `off`). These values are still
 * supported.
 *
 * This property is only one factor that determines whether a topic message
 * will be compressed. Other factors include:
 *
 * - Compression must be enabled in the server configuration.
 *
 * - The client library must support the server's compression
 *   scheme. In this release, the server supports zlib
 *   compression, and also allows compression to be disabled on a
 *   per-connector basis. From 6.4, all client libraries are
 *   capable of zlib compression. A JavaScript client may or may
 *   not support zlib compression, depending on whether the zlib
 *   library can be loaded. The zlib library is packaged
 *   separately to reduce the download size of the core library.
 */
extern const char *const DIFFUSION_COMPRESSION;

/**
 * Key of the topic property that specifies the event data type for a time
 * series topic.
 *
 * The value is the `DIFFUSION_DATATYPE` name of a data type.
 */
extern const char *const DIFFUSION_TIME_SERIES_EVENT_VALUE_TYPE;

/**
 * Key of the topic property that specifies the range of events retained by
 * a time series topic.
 *
 * When a new event is added to the time series, older events that fall
 * outside of the range are discarded.
 *
 * If the property is not specified, a time series topic will retain the ten
 * most recent events.
 *
 * Time series range expressions
 *
 * The property value is a time series `range expression` string
 * composed of one or more constraint clauses. Constraints are combined to
 * provide a range of events from the end of the time series.
 *
 * limit constraint
 * - A limit constraint specifies the maximum number of events from the
 * end of the time series.
 *
 * last clause
 * - A last constraint specifies the maximum duration of events from the
 * end of the time series. The duration is expressed as an integer followed
 * by one of the following time units.
 *
 * `MS` - milliseconds;
 * `S` - seconds;
 * `H` - hours.
 *
 * If a range expression contains multiple constraints, the constraint that
 * selects the smallest range is used.
 *
 * `limit 5`
 * - The five most recent events
 *
 * `last 10s`
 * - All events that are no more than ten seconds older than the latest
 * event
 *
 * `last 10s limit 5`
 * - The five most recent events that are no more than ten seconds older
 * than the latest event
 *
 * Range expressions are not case sensitive: `limit 5 last 10s` is
 * equivalent to `LIMIT 5 LAST 10S`.
 */
extern const char *const DIFFUSION_TIME_SERIES_RETAINED_RANGE;

/**
 * Key of the topic property that specifies the range of time series topic
 * events to send to new subscribers.
 *
 * The property value is a time series range expression, following the
 * format used for `DIFFUSION_TIME_SERIES_RETAINED_RANGE`.
 *
 * If the property is not specified, new subscribers will be sent the latest
 * event if delta streams are enabled and no events if delta streams are
 * disabled. See the description of Subscription range in the
 * time series feature documentation.
 */
extern const char *const DIFFUSION_TIME_SERIES_SUBSCRIPTION_RANGE;

/**
 * Key of the topic property that specifies the topic delivery priority.
 *
 * The supported delivery priorities are:
 * - `low`
 * - `default`
 * - `high`
 *
 * The delivery priority affects the order of topic updates sent to a
 * subscribed client session. When there are multiple topic updates for
 * topics with different priorities in a session's outbound queue, updates
 * for `high` priority topics will be delivered first, followed by
 * updates for `default` priority topics, followed by updates for
 * `low` priority topics. Topic subscription and unsubscription
 * notifications are also delivered according to the topic delivery
 * priority.
 *
 * Using different delivery priorities is most beneficial when there is a
 * large backlog of queued updates to deliver to a client session. On
 * lightly loaded systems, updates typically remain in the outbound queue
 * for a few milliseconds and so there is a lower chance of topic updates
 * being reordered based on their priority. The backlog will be larger if
 * the topic update rate is higher; the server or the client are more
 * heavily loaded; the client session becomes temporarily disconnected; or
 * if there is poor network connectivity between the server and the client.
 *
 * Messages from the server to the client that are not topic updates, for
 * example ping requests and responses, are queued with the
 * `default` delivery priority.
 */
extern const char *const DIFFUSION_PRIORITY;

/**
 * @brief Default constructor. Creates a topic specification of a particular topic type with no properties.
 *        `topic_specification_free` should be called on the pointer when no longer needed.
 *
 * @param topic_type the topic type
 *
 * @return a topic specification initialised with the provided topic type.
 */
TOPIC_SPECIFICATION_T *topic_specification_init(TOPIC_TYPE_T topic_type);

/**
 * @brief Creates a topic specification of a particular topic type and properties. `topic_specification_free` should
 *        be called on the pointer when no longer needed.
 *
 * @param topic_type the topic type
 * @param properties the specification properties
 *
 * @return a topic specification initialised with the provided topic type and
 *         properties.
 */
TOPIC_SPECIFICATION_T *topic_specification_init_with_properties(TOPIC_TYPE_T topic_type, HASH_T *properties);

/**
 * @brief Returns the `TOPIC_TYPE_T` assigned to the specification.
 *
 * @param specification the topic specification
 *
 * @return the topic type of the topic specification. -1 is returned if
 *         the specification is NULL.
 */
TOPIC_TYPE_T topic_specification_get_topic_type(const TOPIC_SPECIFICATION_T *specification);

/**
 * @brief Returns a copy `HASH_T` of the properties assigned to the specification. `hash_free`
 *        should be called on the pointer when no longer needed.
 *
 * @param specification the topic specification
 *
 * @return a copy of the topic specification's properties. NULL is returned if
 *         the specification or the specification's properties hash map is NULL.
 */
HASH_T *topic_specification_get_properties(const TOPIC_SPECIFICATION_T *specification);

/**
 * @brief Set the topic type for a topic specification.
 *
 * @param specification the topic specification
 * @param topic_type    the topic type to set the specification to
 */
void topic_specification_set_topic_type(TOPIC_SPECIFICATION_T *specification, TOPIC_TYPE_T topic_type);

/**
 * @brief Set the properties for a topic specification.
 *
 * @param specification the topic specification
 * @param properties    the properties to set the topic specification to
 */
void topic_specification_set_properties(TOPIC_SPECIFICATION_T *specification, const HASH_T *properties);

/**
 * @brief Create a duplicate (copy) of an existing `TOPIC_SPECIFICATION_T`.
 *        `topic_specification_free` should be called on the pointer when no
 *        longer needed.
 *
 * @param src the topic specification to copy
 *
 * @return a copy of the topic specification provided.
 */
TOPIC_SPECIFICATION_T *topic_specification_dup(const TOPIC_SPECIFICATION_T *src);

/**
 * @brief Free memory associated with a `TOPIC_SPECIFICATION_T`.
 *
 * @param specification the topic specification to be freed.
 */
void topic_specification_free(TOPIC_SPECIFICATION_T *specification);

#endif
