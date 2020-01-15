/*
 * @file deprecate.h
 *
 * Macros to be used when deprecating functions.
 *
 * Copyright © 2018 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

#ifndef _diffusion_deprecate_h_
#define _diffusion_deprecate_h_ 1

#if (defined __GNUC__) || (defined __llvm__)
#	define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#	define DEPRECATED(func) __declspec(deprecated) func
#else
#	define DEPRECATED(func) func /* passthrough */
#endif

#endif