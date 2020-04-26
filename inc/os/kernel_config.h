/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _KERNEL_CONFIG_H_
#define _KERNEL_CONFIG_H_

#include "hal_config.h"

// Auto generated. Don't edit it manually!

#define CONFIG_KERNEL_SYSNV_ENABLED

/* #undef CONFIG_KERNEL_LOG_IN_CRITICAL */
#define CONFIG_KERNEL_TICK_HZ 50

#define CONFIG_KERNEL_ASSERT_ENABLED

#define CONFIG_KERNEL_HIGH_PRIO_WQ_ENABLED
#define CONFIG_KERNEL_LOW_PRIO_WQ_ENABLED
#define CONFIG_KERNEL_FILE_WRITE_WQ_ENABLED
#define CONFIG_KERNEL_HIGH_PRIO_WQ_STACKSIZE 4096
#define CONFIG_KERNEL_LOW_PRIO_WQ_STACKSIZE 4096
#define CONFIG_KERNEL_FILE_WRITE_WQ_STACKSIZE 2048

/* #undef CONFIG_KERNEL_PROFILE_CLOCK32K */
#define CONFIG_KERNEL_PROFILE_CLOCK2M

/* #undef CONFIG_KERNEL_TRACE_HOST97 */
#define CONFIG_KERNEL_TRACE_HOST98
/* #undef CONFIG_KERNEL_TRACE_BBC8 */
/* #undef CONFIG_KERNEL_DISABLE_TRACEID */

#define CONFIG_KERNEL_TRACE_BUF_SIZE 0x8000
#define CONFIG_KERNEL_TRACE_BUF_COUNT 2

#define CONFIG_KERNEL_MEM_RECORD_COUNT 1024

#endif
