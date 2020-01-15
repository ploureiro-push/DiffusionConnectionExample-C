/**
 * @file topic-details.h
 *
 * Functions used for working with topic details (TOPIC_DETAILS_T).
 * Topic details describe the structure of a topic.
 *
 * If you intend to create many topics which have the same type of
 * topic details, e.g. single value string topics, then it is
 * recommended that you only create a single instance of topic details
 * to describe them. This will allow the server to cache the details,
 * significantly improving performance and reducing memory load.
 *
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
#ifndef _diffusion_topic_details_h_
#define _diffusion_topic_details_h_ 1

#include "misc/deprecate.h"
#include "types/topic_details_types.h"

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Create a TOPIC_DETAILS_T which describes a topic containing binary data.
 */
DEPRECATED(TOPIC_DETAILS_T *create_topic_details_binary(void));

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Crate a TOPIC_DETAILS_T which describes a topic containing JSON data.
 */
DEPRECATED(TOPIC_DETAILS_T *create_topic_details_json(void));

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Create a TOPIC_DETAILS_T for a slave topic.
 *
 * @param master_topic    The full path of the topic to which the slave topic
 *                        shall be an alias.
 */
DEPRECATED(TOPIC_DETAILS_T *create_topic_details_slave(char *master_topic));

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Creates an XML document that represents the topic details that has
 * been passed as an argument. The XML is serialised to a BUF_T for
 * transmission to Diffusion when creating new topics.
 */
DEPRECATED(BUF_T *topic_details_schema_build(const TOPIC_DETAILS_T *details));

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Take existing topic details (with an associated schema) and
 * serialise into a form suitable for transmitting over the wire to
 * Diffusion.
 */
DEPRECATED(BUF_T *topic_details_marshal(TOPIC_DETAILS_T *details));

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Create a duplicate (copy) of an existing TOPIC_DETAILS_T.
 */
DEPRECATED(TOPIC_DETAILS_T *topic_details_dup(const TOPIC_DETAILS_T *src));

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Free memory associated with a TOPIC_DETAILS_T.
 */
DEPRECATED(void topic_details_free(TOPIC_DETAILS_T *details));

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Internal
 */
DEPRECATED(void topic_id_details_free(TOPIC_ID_DETAILS_T *val));

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * Internal
 * Read topic details into a structure.
 */
DEPRECATED(char *buf_read_topic_details(const char *data, TOPIC_DETAILS_T *details));

#endif
