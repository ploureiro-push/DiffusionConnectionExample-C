#ifndef _diffusion_topic_views_h_
#define _diffusion_topic_views_h_ 1

/*
 * Copyright © 2019 Push Technology Ltd., All Rights Reserved.
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
 * @file topic-views.h
 *
 * @brief This feature allows a client session to manage topic views.
 *
 * A topic view maps one part of the server's topic tree to another. It
 * dynamically creates a set of <em>reference topics</em> from a set of
 * <em>source topics</em>, based on a declarative <em>topic view
 * specification</em>. The capabilities of topic views range from simple
 * mirroring of topics within the topic tree to advanced capabilities that
 * include publication of partial values and throttling the rate of publication.
 *
 * Each reference topic has a single source topic and has the same topic type as
 * its source topic. Reference topics are read-only (they cannot be updated),
 * nor can they be created or removed directly. Otherwise, they behave just like
 * standard topics. A client session can subscribe to a reference topic, and can
 * fetch the reference topic's current value if it has one.
 *
 * The source topics of a topic view are defined by a topic selector. A
 * reference topic is created for each source topic, according to the topic
 * view. If a source topic is removed, reference topics that are derived from it
 * will automatically be removed. If a topic is added that matches the source
 * topic selector of a topic view, a corresponding reference topic will be
 * created. Removing a topic view will remove all of its reference topics.
 *
 * <h4>Topic view specifications</h4>
 *
 * The following is a simple topic view specification that mirrors all topics
 * below the path `a` to reference topics below the path
 * `b`.
 *
 * `map ?a// to b/<path(1)>`
 *
 * A topic view with this specification will map a source topic at the path
 * `a/x/y/z` to a reference topic at the path `b/x/y/z`.
 * The specification is simple, so the reference topic will exactly mirror the
 * source topic.
 *
 * A general topic view specification comprises several parts:
 *
 * <ul>
 * <li>The <em>source topic</em> clause identifies the source topics.
 * <li>The <em>path mapping</em> clause determines how reference topic paths are
 * derived from the source topic paths, and when expanding to more than one
 * reference topic, from where the values are obtained.
 * <li>The optional <em>topic property mapping</em> clause determines how
 * reference topic properties are derived from source topic properties.
 * <li>The optional <em>value mapping</em> clause determines how reference topic
 * values are derived from source topic values.
 * <li>The optional <em>throttle</em> clause constrains the rate at which each
 * reference topic is updated when its source topic is updated.
 * </ul>
 *
 * <h4>Topic view specification</h4>
 *
 * Topic view specifications are described using a domain specific language
 * (DSL). The DSL has two required clauses: the source clause and the path
 * mapping clause. The source clause identifies which topics should be the
 * source topics for the view. The path mapping clause describes how the paths
 * of reference topics should be derived. A specification begins with the
 * source clause, followed by the path mapping clause.
 *
 * The source clause begins with the `map` keyword and is followed by a
 * topic selector. These topic selectors follow the same as other topic selectors.
 *
 * The path mapping clause begins with the `to` keyword and is followed
 * by a path mapping template. A path mapping template is the topic path with
 * embedded directives. Directives are evaluated when creating the topic
 * reference and substituted into the topic path. Directives are surrounded by
 * angle brackets, they contain the name of the directive and the parameters.
 * The parameter list begins with an opening round bracket, ends with a closing
 * round bracket and parameters in the list should be separated by colons.
 *
 * Topic selectors and path mapping templates can be quoted or unquoted. They
 * are quoted using the single quote mark. To include whitespace, single quotes
 * or literal opening angle brackets they must be quoted. In quoted selectors
 * and templates single quotes, literal opening angle brackets and backslashes
 * must be escaped with a single backslash. In templates the opening angle
 * bracket should be unescaped when beginning a directive. Characters in
 * unquoted selectors and templates can't be escaped.
 *
 * Any whitespace can be used to separate keywords, statements and clauses.
 *
 * The following example creates reference topics with 'prefix/' prepended to
 * the paths of the source topics.
 *
 * `map ?a/path/ to prefix/<path(0)>`
 *
 * <h4>Source topic clause</h4>
 *
 * The source topic clause begins with the `map` keyword and is followed
 * by a topic selector. These topic selectors follow the same
 * parsing rules as other topic selectors.
 *
 * When evaluating a topic view, topics in the topic tree that match the source
 * topic selector are considered, with the following exceptions:
 * <ul>
 * <li>Topics created through the Publisher API;
 * <li>`TOPIC_TYPE_ROUTING` topics.
 * </ul>
 *
 * Both `TOPIC_TYPE_SLAVE` and reference topics are valid source
 * topics. In particular, chaining of topic views is supported; that is, a
 * reference topic created by one topic view can be the source topic of another
 * topic view. Additionally, a reference topic can be the master topic of a
 * slave topic, or the source topic of a routing topic subscription.
 *
 * <blockquote><em>Prefer topic views to slave topics which are now
 * deprecated</em>. Individual topics can be mirrored by creating a slave topic,
 * but maintaining slave topics for a branch of the topic tree quickly becomes
 * tedious. A topic view will maintain such a branch automatically, and provides
 * more sophisticated mapping options. </blockquote>
 *
 * <h4>Path mapping clause</h4>
 *
 * The path of a reference topic is derived from the source topic according to
 * the topic view path mapping. The path mapping allows the source topic path
 * and the value of the source topic to determine the path of the reference
 * topic. In addition the path mapping can include `expand` directives which
 * allow objects and arrays in JSON source topic values to be expanded to
 * produce many reference topics.
 *
 * A path mapping clause begins with the `to` keyword and is followed by a
 * path mapping template. A path mapping template is a topic path with embedded
 * <em>directives</em>. Directives are evaluated when creating the topic
 * reference and substituted into the topic path. Directives are delimited by
 * angle brackets (`<`, `>`) and consist of the name of the
 * directive and a list of parameters. The parameter list is comma-separated and
 * surrounded by parentheses (`(`, `)`).
 *
 * Two path mapping directives are supported:
 *
 * <dl>
 * <dt>Source path directives</dt>
 * <dd>Source path directives extract a portion of the source path and are
 * parameterized by the index of the start part of the source path and the
 * number of parts to include. The number of parts parameter is optional – if it
 * is missing, the selection extends to the end of the source path. The syntax
 * is <code>&lt;path(<em>start</em>, <em>number</em>)&gt;</code>, or
 * <code>&lt;path(<em>start</em>)&gt;</code> when the number of parts parameter
 * is omitted.
 * <p>
 * For example, given the source path `a/b/c/d`, the source path directive
 * `<path(1, 2)>` is mapped to the reference topic path `b/c`, and
 * the source path directive `<path(2)>` is mapped to the reference topic
 * path `c/d`.</dd>
 *
 * <dt>Source value directives</dt>
 * <dd>Source value directives are only applied to `TOPIC_TYPE_JSON`
 * source topics; if the path mapping contains a source value directive,
 * non-JSON topics matching the source topic selector are ignored. Source value
 * directives use the keyword `scalar` and are parameterized by a single
 * JSON pointer that extracts a scalar value from the source value. A scalar
 * value is a string, a number, `true`, `false`, or `null`,
 * that is, anything other than an array or a object. If the JSON pointer does
 * not refer to a scalar value in the source value, no reference topic will be
 * created. This includes cases where the JSON pointer refers to an array or
 * an object), or when no part of the source value is selected.
 *
 * Deriving the reference topic paths from part of the source topic value
 * effectively creates a secondary index on the value. For source value
 * directives to work efficiently, the selected scalar values should be
 * relatively stable. If an update to the source topic changes the selected
 * scalar value, the corresponding reference topic will be removed and a new
 * reference topic will be created.
 *
 * For example, given a source value of
 *
 * <pre>
 * {
 *   "account" : "1234",
 *   "balance" : { "amount" : 12.57, "currency" : "USD" }
 * }
 * </pre>
 *
 * and the source value directive
 * `currency/<scalar(/balance/currency)>/account/<scalar(/account)>`, the
 * reference topic path will be `currency/USD/account/1234`.
 *
 * If the extracted value is a string, it is copied literally to the reference
 * topic path. A value that contains path separators (`/`) will create a
 * reference topic path with more levels than the path mapping template.
 *
 * An extracted value of `null` will be copied to the reference topic path
 * as the string `"null"`.</dd>
 * </dl>
 *
 * <h4>Expand value directives</h4>
 *
 * <dd>Expand value directives are only applied to `TOPIC_TYPE_JSON`
 * source topics; if the path mapping contains an expand value directive,
 * non-JSON topics matching the source topic selector are ignored.
 *
 * Expand value directives use the keyword `expand` and are parameterized
 * by one or two JSON pointers.
 *
 * The first pointer indicates the element within the value to be expanded, and
 * if omitted, the value is expanded from the root. Expansion of a source topic
 * indicates that every direct child of the element pointed to by the expand
 * pointer will be used to create a new reference topic (or provide input to
 * later expand or scalar directives). For example `<expand()>` would
 * expand every child item in the source value and `<expand(/Account)>`
 * would expand every child of the `Account` value in the source value.
 * The specified value could be an object, an array or even a scalar value, but
 * a scalar value would expand to only a single new value.
 *
 * The optional second parameter of the expand directive specifies a pointer to
 * a scalar value within the expanded value which will be used to derive the
 * path fragment of the reference topic path. If the second pointer is not
 * specified or no scalar value is found for the pointer, the path fragment is
 * taken from the key (if the child value is an object) or the index (if the
 * child value is an array). Scalar child values will expand to a reference
 * topic but will not add anything to the generated path. For example
 * `<expand(,/name)>` would expand from the root of the source value and
 * each child value path fragment would be obtained from the scalar value with
 * the key `name`.
 *
 * So if a source topic had a value of
 *
 * <pre>
 * {
 *     "values": [1, 5, 7]
 * }
 * </pre>
 *
 * a path mapping of `value<expand(/values)>` would expand the value to the following
 * reference topics:-
 *
 * path "value0" with a value of 1<br>
 * path "value1" with a value of 5<br>
 * path "value2" with a value of 7<br>
 *
 * Expand directives can be nested (i.e. there can be more than one expand
 * directive in a path mapping). In this case a second expand directive will use
 * the value from the previous expand as its source (root) value and not the
 * value of the source topic. This also applies to scalar directives that follow
 * an expand directive.
 *
 * If expansion causes more than one mapping to the same topic path, only the
 * first encountered will be created and updated.
 *
 * Expanding source topic values effectively creates secondary indices on the
 * value. For expanded value directives to work efficiently, the value selected
 * for expansion should be relatively stable in terms of the children it
 * contains. If an update to the source topic changes the children of the
 * expanded value, then corresponding reference topics will be removed and
 * created. Updates should generally be limited to changing values within the
 * expanded values.</dd>
 * </dl>
 *
 * <h4>Topic property mapping clause</h4>
 *
 * The topic specification of a reference topic is derived from the topic
 * specification of the source topics. A reference topic has the same
 * topic type as its source topic. The topic properties are mapped according
 * to the following table:
 *
 * <table>
 * <tr>
 * <th style= "text-align:left;">Source topic property</th>
 * <th>Reference topic specification default</th>
 * <th>Can be set by topic property mapping?</th>
 * <th>Notes</th>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_COMPRESSION`</th>
 * <td>Copied from source topic specification</td>
 * <td>Yes</td>
 * <td></td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_CONFLATION`</th>
 * <td>Copied from source topic specification</td>
 * <td>Yes</td>
 * <td></td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_DONT_RETAIN_VALUE`</th>
 * <td>Copied from source topic specification</td>
 * <td>Yes</td>
 * <td></td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_OWNER`</th>
 * <td>Not set</td>
 * <td>No</td>
 * <td></td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_PERSISTENT`</th>
 * <td>Not set</td>
 * <td>No</td>
 * <td>Reference topics are not persisted. Topic views are persisted, so a
 * reference topic will be recreated on server restart if its source is
 * persistent.</td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_PRIORITY`</th>
 * <td>Copied from source topic specification</td>
 * <td>Yes</td>
 * <td></td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_PUBLISH_VALUES_ONLY`</th>
 * <td>Copied from source topic specification</td>
 * <td>Yes</td>
 * <td></td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_REMOVAL`</th>
 * <td>Not set</td>
 * <td>No</td>
 * <td>Reference topics cannot be removed directly.</td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_SCHEMA`</th>
 * <td>Copied from source topic specification</td>
 * <td>No</td>
 * <td>A `TOPIC_TYPE_RECORDV2` reference topic has the same
 * schema as its source topic.</td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_SLAVE_MASTER_TOPIC`</th>
 * <td>Not set</td>
 * <td>No</td>
 * <td>If a reference topic has a slave topic as its source topic, it indirectly
 * references the slave's master topic.</td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_TIDY_ON_UNSUBSCRIBE`</th>
 * <td>Copied from source topic specification</td>
 * <td>Yes</td>
 * <td></td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_TIME_SERIES_EVENT_VALUE_TYPE`</th>
 * <td>Copied from source topic specification</td>
 * <td>No</td>
 * <td>A `TOPIC_TYPE_TIME_SERIES` reference topic has the same
 * value type as its source topic.</td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_TIME_SERIES_RETAINED_RANGE`</th>
 * <td>Copied from source topic specification</td>
 * <td>Yes, with restrictions</td>
 * <td>A topic property mapping cannot increase the time series retained range
 * by overriding the `DIFFUSION_TIME_SERIES_RETAINED_RANGE` property. The retained
 * range of a reference time series topic will be constrained to be no greater
 * than that of its source topic.</td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_TIME_SERIES_SUBSCRIPTION_RANGE`</th>
 * <td>Copied from source topic specification</td>
 * <td>Yes</td>
 * <td></td>
 * </tr>
 * <tr>
 * <th style="text-align:left;">`DIFFUSION_VALIDATE_VALUES`</th>
 * <td>Not set</td>
 * <td>No</td>
 * <td>A reference topic reflects updates to its source topic. It cannot reject
 * updates.</td>
 * </tr>
 * </table>
 *
 * A topic property mapping clause is optional. It begins
 * `with properties` and consists of a comma-separated list of topic
 * property keys and values, each separated by a comma. For example, the
 * following topic view specification maps all topics below the path
 * `a` to reference topics below the path `b`, and disables both conflation
 * and compression for the reference topics.
 *
 * `map ?a// to b/<path(1)> with properties 'CONFLATION':'off', 'COMPRESSION':'false'`
 *
 * <h4>Topic value mapping</h4>
 *
 * By default, a reference topic's value is a copy of the source topic
 * value, or part of the source value produced by an expand path
 * mapping directive. For `JSON` source topics, the optional topic
 * value mapping clause can be applied to extract part of the source
 * value, or to further refine the value produced by the expand
 * directive.
 *
 * A topic value mapping begins the keyword `as` and is followed by a
 * value directive. A value directive is delimited by angle brackets
 * (`<`, `>`), and consists of the `value` keywords and a single JSON
 * pointer parameter. The JSON pointer selects the part of the source
 * value to copy.
 *
 * For example, given a source value of
 *
 * <pre>
 * {
 *     "account" : "1234",
 *     "balance" : { "amount" : 12.57, "currency" : "USD" }
 * }
 * </pre>
 *
 * and the value mapping clause `as <value(/balance)>`, the
 * reference topic value will be
 *
 * <pre>
 * {
 *     "amount" : 12.57,
 *     "currency" : "USD"
 * }
 * </pre>
 *
 * Value mappings that follow expand directives apply to the expanded
 * value and not the source topic value.
 *
 * Topic value mappings only alter the reference topic value; only the
 * path mapping determines whether a reference topic should exist. If
 * the topic value mapping's JSON pointer fails to select anything
 * from the source topic value, the reference topic will have the JSON
 * value `null`.
 *
 * Topic value mappings are often used with path value mappings to
 * avoid repeating information in the path and the value. For example:
 *
 * `map ?accounts// to balances/<scalar(/account)> as <value(/balance)>;`
 *
 * <h4>Throttle clause</h4>
 *
 * The optional throttle clause can be used to constrain the rate at which a
 * reference topic is updated when its source topic is updated. The primary
 * application of a throttle clause is to restrict the number of updates sent to
 * reference topic subscribers, reducing network utilization or the processing
 * each subscriber must do. Throttling also restricts the rate at which client
 * sessions can observe changes to reference topic values using the fetch API.
 *
 * The throttle clause has the form
 * `throttle to X updates every period`, where
 * <em>X</em> is a positive integer, and <em>period</em> is a positive integer
 * followed by a time unit which is one of `seconds`,
 * `minutes`, or `hours`.
 *
 * For example, the following topic view specification maps all topics below the
 * path `a` to reference topics below the path `b`, but
 * updates the value of each reference topic at most twice every five seconds:
 *
 * `map ?a// to b/<path(1)> throttle to 2 updates every 5 seconds`
 *
 * To improve readability, the throttling clause allows `1 update` as
 * an alternative to `1 updates`, and `every second` as an
 * alternative to `every 1 seconds` (and so on, for other time
 * units). For example, the following topic view specification maps all topics
 * below the path `a` to reference topics below the path
 * `b`, but updates the value of each reference topic at most once
 * every hour:
 *
 * `map ?a// to b/<path(1)> throttle to 1 update every minute`
 *
 * The throttle clause is only applied when a source topic is updated more
 * frequently than the configured rate. If a source topic is updated less
 * frequently, updates are passed on unconstrained. If the rate is exceeded, a
 * reference topic will not be updated again until the configured period has
 * expired. At this time, the reference topic will be updated based on the
 * source topic updates that happened in the interim, and a single value will be
 * published. Thus, a throttle clause provides <em>topic-scoped conflation</em>.
 *
 * The throttle clause is ignored for time series topics because time series
 * updates do not support efficient conflation. Updates to source time series
 * topics are passed on immediately to the corresponding reference topics,
 * regardless of any throttle clause.
 *
 * <h4>Escaping and quoting special characters</h4>
 *
 * Each part of a topic view expression has characters with special
 * significance. Source topic clauses and path mapping clauses are delimited by
 * white space. Directives in path and topic property mapping clauses are
 * delimited by the `<` and `>` characters, and each directive
 * parameter is terminated by `,` or `)`. Topic property mapping
 * clauses are delimited by white space, and the `:` and `,`
 * characters.
 *
 * Sometimes a topic view must refer to or generate topics with paths that
 * containing special characters, or use a JSON pointer containing special
 * characters. The escape sequence `\x` can be used to literally insert
 * any character `x`, with a one exception: `\/` cannot be used in
 * path fragments since the path delimiter `/` is always significant.
 *
 * Here is an example topic view expression containing escape sequences. It maps
 * the topic path `a topic` a reference topic with the path
 * `another topic`.
 *
 * `map a\ topic to another\ topic`
 *
 * Here is an example with a source value directive that uses the JSON pointer
 * `/x()/y` to extract the target path from the source value. The
 * `)` character in the JSON pointer must be escaped so it is not treated
 * as the end of the parameter list.
 *
 * `map ?a// to &lt;scalar(/x(\)/y)&gt;`
 *
 * To insert `\`, the escape sequence `\\` must be used.
 *
 * There is no need to escape white space in JSON pointers directive parameters.
 * However, white space is significant. For example, the following expressions
 * have different topic value mapping clauses since the JSON pointer in the
 * second expression is `/x `; that is, it has a trailing space:
 *
 * <pre>
 * map a to b as &lt;value(/x)&gt;
 * map a to b as &lt;value(/x )&gt;
 * </pre>
 *
 * Instead of using escape sequences, white space characters can be included in
 * source topic clauses and path mapping clauses using quotes. A clause is
 * quoted by wrapping it in single quote (`'`) or double quote (`"`)
 * characters. For example:
 *
 * `map "a topic" to "another topic"`
 *
 * Within a quoted clause, quotes of the same type must be escaped:
 *
 * <code>map 'alice\'s topic' to 'bob\'s topic'</code>
 *
 * For consistency, the values in topic property mapping clauses can be escaped
 * or quoted. However, there is no need to do so because none of the valid
 * values for the mappable properties contain special characters.
 *
 * <h4>Dealing with topic path conflicts</h4>
 *
 * Reference topics have a lower priority than normal topics created through the
 * API, including replicas of normal topics created by topic replication or
 * fan-out. A reference topic will only be created if no topic or reference
 * topic is already bound to its derived topic path.
 *
 * Topic views have a precedence based on order of creation. If two topic views
 * define mappings the same topic path, the earliest-created topic view will
 * create a reference topic. If a topic view is updated, it retains its original
 * precedence.
 *
 * <h4>Topic view persistence and replication</h4>
 *
 * Reference topics are neither replicated nor persisted. They are created and
 * removed based on their source topics. However, topic views are replicated and
 * persisted. A server that restarts will restore topic views during recovery.
 * Each topic view will then create reference topics based on the source topics
 * that have been recovered.
 *
 * The server records all changes to topic views in a persistent store. Topic
 * views are restored if the server is started.
 *
 * If a server belongs to a cluster, topic views will be replicated to each
 * server in the cluster. Topic views are evaluated locally within a server.
 * Replicated topic views that select non-replicated source topics can create
 * different reference topics on each server in the cluster.
 *
 * <h4>Access control</h4>
 *
 * The following access control restrictions are applied:
 *
 * <ul>
 * <li>To list the topic views, a session needs the `READ_TOPIC_VIEWS`
 * global permission.
 *
 * <li>To create, replace, or remove a topic view, a session needs the
 * `MODIFY_TOPIC_VIEWS` global permission and `SELECT_TOPIC` permission
 * for the path prefix of the source topic selector.
 *
 * <li>Each topic view records the principal and security roles of the
 * session that created it as the <em>topic view security
 * context</em>. When a topic view is evaluated, this security context
 * is used to constrain the creation of reference topics. A reference
 * topic will only be created if the security context has `READ_TOPIC`
 * permission for the source topic path, and `MODIFY_TOPIC` permission
 * for the reference topic path. The topic view security context is
 * copied from the creating session at the time the topic view is
 * created or replaced, and is persisted with the topic view. The
 * topic view security context is not updated if the roles associated
 * with the session are changed.
 *
 * </ul>
 */

#include "session.h"
#include "diffusion-api-error.h"

/**
 * Opaque topic view struct
 */
typedef struct DIFFUSION_TOPIC_VIEW_T DIFFUSION_TOPIC_VIEW_T;

/**
 * @ingroup PublicAPI_TopicViews
 *
 * @brief Return a memory allocated copy of the topic view's name. `free` should be called
 *        on this pointer when no longer needed.
 *
 * @param topic_view the topic view
 *
 * @return the topic view's name.
 */
char *diffusion_topic_view_get_name(const DIFFUSION_TOPIC_VIEW_T *topic_view);

/**
 * @ingroup PublicAPI_TopicViews
 *
 * @brief Return a memory allocated copy of the topic view's specification. `free` should be called
 *        on this pointer when no longer needed.
 *
 * @param topic_view the topic view
 *
 * @return the topic view's specification.
 */
char *diffusion_topic_view_get_specification(const DIFFUSION_TOPIC_VIEW_T *topic_view);

/**
 * @ingroup PublicAPI_TopicViews
 *
 * @brief Return a memory allocated `SET_T` of the topic view's roles. `set_free` should be called
 *        on this pointer when no longer needed.
 *
 * @param topic_view the topic view
 *
 * @return the topic view's roles used when evaluating permissions.
 */
SET_T *diffusion_topic_view_get_roles(const DIFFUSION_TOPIC_VIEW_T *topic_view);

/**
 * @ingroup PublicAPI_TopicViews
 *
 * @brief Returns a memory allocated copy of a `DIFFUSION_TOPIC_VIEW_T`. `diffusion_topic_view_free` should
 *        be called on this pointer when no longer needed.
 *
 * @param topic_view the topic_view to be copied.
 *
 * @return a copy of a `DIFFUSION_TOPIC_VIEW_T`. NULL, if `event` is NULL.
 */
DIFFUSION_TOPIC_VIEW_T *diffusion_topic_view_dup(const DIFFUSION_TOPIC_VIEW_T *topic_view);

/**
 * @ingroup PublicAPI_TopicViews
 *
 * @brief Free a memory allocated `DIFFUSION_TOPIC_VIEW_T`
 *
 * @param topic_view the `DIFFUSION_TOPIC_VIEW_T` to be freed.
 */
void diffusion_topic_view_free(DIFFUSION_TOPIC_VIEW_T *topic_view);

/**
 * @brief Callback when a topic view has successfully been created.
 *
 * @param topic_view The created topic view.
 * @param context    User supplied context.
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_view_created_cb)(const DIFFUSION_TOPIC_VIEW_T *topic_view, void *context);

/**
 * @brief Callback when a response is received from the server regarding a topic view operation.
 *
 * @param context User supplied context.
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_view_response_cb)(void *context);

/**
 * @brief Callback when a response is received from the server regarding a
 *        `diffusion_topic_views_list_topic_views` operation.
 *
 * @param topic_views A `LIST_T` containing `DIFFUSION_TOPIC_VIEW_T`s
 * @param context     User supplied context.
 *
 * @return HANDLERS_SUCCESS or HANDLER_FAILURE.
 */
typedef int (*on_topic_views_list_cb)(const LIST_T *topic_views, void *context);

/**
 * @brief Structure describing a request to create a topic view
 */
typedef struct diffusion_create_topic_view_params_s {
        /// The name of the view to be created.
        const char *view;
        /// The specification of the view using the DSL.
        const char *specification;
        /// Callback when the topic view is created.
        on_topic_view_created_cb on_topic_view_created;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context returned to callbacks.
        void *context;
} DIFFUSION_CREATE_TOPIC_VIEW_PARAMS_T;

/**
 * @brief Structure describing a request to list topic views
 */
typedef struct diffusion_topic_views_list_params_s {
        /// Callback when a list of topic views is
        /// received.
        on_topic_views_list_cb on_topic_views_list;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context returned to callbacks.
        void *context;
} DIFFUSION_TOPIC_VIEWS_LIST_PARAMS_T;

/**
 * @brief Structure describing a request to remove a topic view
 */
typedef struct diffusion_remove_topic_view_params_s {
        /// The name of the view to be removed.
        const char *view;
        /// Callback when the topic view is removed.
        on_topic_view_response_cb on_topic_view_removed;
        /// Callback to handle errors. Can be NULL.
        ERROR_HANDLER_T on_error;
        /// Callback to handle. Can be NULL.
        DISCARD_HANDLER_T on_discard;
        /// User-supplied context returned to callbacks.
        void *context;
} DIFFUSION_REMOVE_TOPIC_VIEW_PARAMS_T;

/**
 * @ingroup PublicAPI_TopicViews
 *
 * @brief Create a new named topic view.
 *
 * If a view with the same name already exists the new view will replace
 * the existing view.
 *
 * @param session   The current session. If NULL, this function returns immediately.
 * @param params    Parameters defining the `diffusion_topic_views_create_topic_view` request
 *                  and callbacks.
 * @param api_error Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_topic_views_create_topic_view(SESSION_T *session, const DIFFUSION_CREATE_TOPIC_VIEW_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_TopicViews
 *
 * @brief List all the topic views that have been created.
 *
 * @param session   The current session. If NULL, this function returns immediately.
 * @param params    Parameters defining the `diffusion_topic_views_list_topic_views` request
 *                  and callbacks.
 * @param api_error Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_topic_views_list_topic_views(SESSION_T *session, const DIFFUSION_TOPIC_VIEWS_LIST_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

/**
 * @ingroup PublicAPI_TopicViews
 *
 * @brief Remove a named topic view if it exists.
 *
 * If the named view does not exist the completable future will complete
 * successfully.
 *
 * @param session   The current session. If NULL, this function returns immediately.
 * @param params    Parameters defining the `diffusion_topic_views_list_topic_views` request
 *                  and callbacks.
 * @param api_error Populated on API error. Can be NULL.
 *
 * @return true if the operation was successful. False, otherwise. In this case, if
 *         a non-NULL `api_error` pointer has been provided, this will be populated
 *         with the error information and should be freed with `diffusion_api_error_free`.
 */
bool diffusion_topic_views_remove_topic_view(SESSION_T *session, const DIFFUSION_REMOVE_TOPIC_VIEW_PARAMS_T params, DIFFUSION_API_ERROR *api_error);

#endif
