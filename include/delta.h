/**
 * @file delta.h
 *
 * Functions for generating deltas describing the changes to data, and
 * applying deltas to data.
 *
 * Copyright © 2016 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */
#ifndef _diffusion_delta_h_
#define _diffusion_delta_h_ 1

#include "types/session_types.h"
#include "buf.h"

/**
 * When generating a delta, a chunk of memory is set aside for working
 * storage. This is the default value if not explicity specified.
 */
#define DIFF_DEFAULT_MAX_STORAGE INT_MAX

/**
 * In order to prevent complex delta generation from using up too much CPU
 * (especially with larger messages), a bailout factor is referenced to abort
 * the process early. If not explicitly specified, this is the value used. If
 * the process does bail out, then a delta may still be generated but may not
 * be of optimal length.
 */
#define DIFF_DEFAULT_BAILOUT_FACTOR 10000

/**
 * @brief Apply a delta to a value, producing a new value. `buf_free` should be called
 *        on the pointer when no longer needed.
 *
 * @param old_value	The original value
 * @param diff          The delta (diff) to apply to the original value.
 * @retval "BUF_T *" A pointer to a BUF_T containing the result of applying
 * the delta to the original value.
 */
BUF_T *diff_apply_binary(BUF_T *old_value, BUF_T *diff);

/**
 * @brief Generate a delta between two binary values. `buf_free` should be called
 *        on the pointer when no longer needed.
 *
 * @param old_value	The source value
 * @param new_value     The target value
 * @retval "BUF_T *" A pointer to a BUF_T containing a delta describing how to
 * transform the source value into the target value. If there are no
 * differences or an error occurs, then NULL is returned.
 */
BUF_T *diff_generate_binary(BUF_T *old_value, BUF_T *new_value);

/**
 * @brief Generate a delta between two binary values, with additional control over variables that influence the process.
 *        `buf_free` should be called on the pointer when no longer needed.
 *
 * @param old_value	The source value
 * @param new_value     The target value
 * @param max_storage   The maximum amount of internal storage to use.
 * @param bailout_factor Influences when to abort delta generation; smaller
 * values will abort sooner. This is useful with larger messages where the
 * process may use a significant amount of CPU time.
 * @retval "BUF_T *" A pointer to a BUF_T containing a delta describing how to
 * transform the source value into the target value. If there are no
 * differences or an error occurs, then NULL is returned.
 */
BUF_T *diff_generate_binary_ex(BUF_T *old_value, BUF_T *new_value, int max_storage, int bailout_factor);

/**
 * @brief Returns the cached value for a given topic path.
 *
 * Cached values are only maintained for topics that are updated using
 * update_value(). When an updater sends an update, the latest value is cached
 * so that more efficient delta updates can be performed subsequently.
 *
 * @param session	The session which is performing the updates.
 * @param topic_path    Specifies the topic to obtain the current value for.
 *
 * @retval "BUF_T *"    A pointer to the latest value, or NULL if there is no
 *                      value for the given path. This might be because no
 *                      value has been sent, or the value has been removed
 *                      from the cache. Note that this does not return a copy
 *                      of the data, but a pointer to the data in the cache;
 *                      it should not be directly freed (or modified) by the
 *                      user.
 */
BUF_T *update_cache_get_value(SESSION_T *session, const char *topic_path);

/**
 * @brief Removes values from the cache.
 *
 * Cached values are only maintained for topics that are updated using
 * update_value(). When an updater sends an update, the latest value is cached
 * so that more efficient delta updates can be performed subsequently.
 *
 * If a cached value is removed, the next update for a topic will transmit the
 * whole value to the server rather than a delta.
 *
 * @param session	The session which is performing the updates.
 * @param topic_selector	All values that match the selector will be
 *                              removed.
 */
void update_cache_remove_values(SESSION_T *session, const char *topic_selector);

/**
 * @brief Clears the update value cache.
 *
 * update_value() caches the last value sent to the server so that subsequent
 * updates can calculate deltas to send for efficiency. This allows the cache
 * to be cleared.
 *
 * If a cached value is removed, the next update for a topic will transmit the
 * whole value to the server rather than a delta.
 *
 * @param session	The session which is performing the updates.
 */
void update_cache_clear_values(SESSION_T *session);

#endif
