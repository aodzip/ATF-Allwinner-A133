/*
 * Copyright (c) 2023, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MT_SPM_SSPM_INTC_H
#define MT_SPM_SSPM_INTC_H

#include <mt_spm_reg.h>

#define MT_SPM_SSPM_INTC_SEL_0	(0x10)
#define MT_SPM_SSPM_INTC_SEL_1	(0x20)
#define MT_SPM_SSPM_INTC_SEL_2	(0x40)
#define MT_SPM_SSPM_INTC_SEL_3	(0x80)

#define MT_SPM_SSPM_INTC_TRIGGER(id, sg)	(((0x10 << (id)) | (sg << (id))) & 0xFF)

#define MT_SPM_SSPM_INTC0_HIGH	MT_SPM_SSPM_INTC_TRIGGER(0, 1)
#define MT_SPM_SSPM_INTC0_LOW	MT_SPM_SSPM_INTC_TRIGGER(0, 0)

#define MT_SPM_SSPM_INTC1_HIGH	MT_SPM_SSPM_INTC_TRIGGER(1, 1)
#define MT_SPM_SSPM_INTC1_LOW	MT_SPM_SSPM_INTC_TRIGGER(1, 0)

#define MT_SPM_SSPM_INTC2_HIGH	MT_SPM_SSPM_INTC_TRIGGER(2, 1)
#define MT_SPM_SSPM_INTC2_LOW	MT_SPM_SSPM_INTC_TRIGGER(2, 0)

#define MT_SPM_SSPM_INTC3_HIGH	MT_SPM_SSPM_INTC_TRIGGER(3, 1)
#define MT_SPM_SSPM_INTC3_LOW	MT_SPM_SSPM_INTC_TRIGGER(3, 0)

#define DO_SPM_SSPM_LP_SUSPEND()	mmio_write_32(SPM_MD32_IRQ, MT_SPM_SSPM_INTC0_HIGH)

#define DO_SPM_SSPM_LP_RESUME()		mmio_write_32(SPM_MD32_IRQ, MT_SPM_SSPM_INTC0_LOW)

#endif /* MT_SPM_SSPM_INTC_H */
