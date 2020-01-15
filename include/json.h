/**
 * @file json.h
 *
 * Utilities for converting CBOR-encoded data to JSON string representation.
 *
 * Copyright © 2017 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

#ifndef _diffusion_json_h_
#define _diffusion_json_h_ 1

#include "misc/deprecate.h"
#include "buf.h"

/**
 * @deprecated This function is deprecated from version 6.1 onwards, and may
 * be removed.
 *
 * @brief Convert binary (CBOR) data to a JSON string.
 *
 * This function is useful for converting data received on a JSON topic from
 * CBOR representation to a JSON string suitable for passing to a 3rd party
 * JSON parser.
 *
 * <p/>
 *
 * Note that this is introducing an extra step (i.e., CBOR -> string -> JSON)
 * and if possible, you should read the CBOR tokens and convert them directly
 * to JSON representation using facilities provided by your JSON library.
 *
 * @param cbor A pointer to memory containing CBOR-encoded data.
 * @param len  The length of the CBOR-encoded data.
 * @returns A pointer to a BUF_T containing a JSON string, or NULL on error.
 */
DEPRECATED(BUF_T *cbor_to_json(const char *cbor, size_t len));

#endif
