/*
 * Copyright (c) 2020-2023, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <plat/arm/common/plat_arm.h>
#include <plat/arm/css/common/css_pm.h>

/******************************************************************************
 * The power domain tree descriptor.
 ******************************************************************************/
const unsigned char rd_n2_pd_tree_desc[] = {
	(PLAT_ARM_CLUSTER_COUNT) * (CSS_SGI_CHIP_COUNT),
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
#if (PLAT_ARM_CLUSTER_COUNT > 4 || \
	(CSS_SGI_PLATFORM_VARIANT == 2 && CSS_SGI_CHIP_COUNT > 1))
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
#endif
#if (PLAT_ARM_CLUSTER_COUNT > 8 || \
	(CSS_SGI_PLATFORM_VARIANT == 2 && CSS_SGI_CHIP_COUNT > 2))
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
#endif
#if (PLAT_ARM_CLUSTER_COUNT > 8 || \
	(CSS_SGI_PLATFORM_VARIANT == 2 && CSS_SGI_CHIP_COUNT > 3))
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
	CSS_SGI_MAX_CPUS_PER_CLUSTER,
#endif
};

/*******************************************************************************
 * This function returns the topology tree information.
 ******************************************************************************/
const unsigned char *plat_get_power_domain_tree_desc(void)
{
	return rd_n2_pd_tree_desc;
}

/*******************************************************************************
 * The array mapping platform core position (implemented by plat_my_core_pos())
 * to the SCMI power domain ID implemented by SCP.
 ******************************************************************************/
#if (CSS_SGI_PLATFORM_VARIANT == 2)
const uint32_t plat_css_core_pos_to_scmi_dmn_id_map[] = {
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x0)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x1)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x2)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x3)),
#if (CSS_SGI_CHIP_COUNT > 1)
	(SET_SCMI_CHANNEL_ID(0x1) | SET_SCMI_DOMAIN_ID(0x0)),
	(SET_SCMI_CHANNEL_ID(0x1) | SET_SCMI_DOMAIN_ID(0x1)),
	(SET_SCMI_CHANNEL_ID(0x1) | SET_SCMI_DOMAIN_ID(0x2)),
	(SET_SCMI_CHANNEL_ID(0x1) | SET_SCMI_DOMAIN_ID(0x3)),
#endif
#if (CSS_SGI_CHIP_COUNT > 2)
	(SET_SCMI_CHANNEL_ID(0x2) | SET_SCMI_DOMAIN_ID(0x0)),
	(SET_SCMI_CHANNEL_ID(0x2) | SET_SCMI_DOMAIN_ID(0x1)),
	(SET_SCMI_CHANNEL_ID(0x2) | SET_SCMI_DOMAIN_ID(0x2)),
	(SET_SCMI_CHANNEL_ID(0x2) | SET_SCMI_DOMAIN_ID(0x3)),
#endif
#if (CSS_SGI_CHIP_COUNT > 3)
	(SET_SCMI_CHANNEL_ID(0x3) | SET_SCMI_DOMAIN_ID(0x0)),
	(SET_SCMI_CHANNEL_ID(0x3) | SET_SCMI_DOMAIN_ID(0x1)),
	(SET_SCMI_CHANNEL_ID(0x3) | SET_SCMI_DOMAIN_ID(0x2)),
	(SET_SCMI_CHANNEL_ID(0x3) | SET_SCMI_DOMAIN_ID(0x3)),
#endif
};
#else
const uint32_t plat_css_core_pos_to_scmi_dmn_id_map[] = {
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x0)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x1)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x2)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x3)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x4)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x5)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x6)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x7)),
#if (PLAT_ARM_CLUSTER_COUNT > 8)
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x8)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0x9)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0xA)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0xB)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0xC)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0xD)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0xE)),
	(SET_SCMI_CHANNEL_ID(0x0) | SET_SCMI_DOMAIN_ID(0xF)),
#endif
};
#endif
