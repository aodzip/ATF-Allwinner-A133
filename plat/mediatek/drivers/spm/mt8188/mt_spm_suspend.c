/*
 * Copyright (c) 2023, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#ifndef MTK_PLAT_SPM_UART_UNSUPPORT
#include <drivers/uart.h>
#endif
#include <lib/mmio.h>
#ifndef MTK_PLAT_CIRQ_UNSUPPORT
#include <mtk_cirq.h>
#endif
#include <constraints/mt_spm_rc_internal.h>
#include <drivers/spm/mt_spm_resource_req.h>
#include <lib/pm/mtk_pm.h>
#include <lpm/mt_lp_api.h>
#include <mt_spm.h>
#include <mt_spm_conservation.h>
#include <mt_spm_internal.h>
#include <mt_spm_reg.h>
#include <mt_spm_suspend.h>
#include <pcm_def.h>

#define SPM_SUSPEND_SLEEP_PCM_FLAG \
	(SPM_FLAG_DISABLE_INFRA_PDN | \
	 SPM_FLAG_DISABLE_VCORE_DVS | \
	 SPM_FLAG_DISABLE_VCORE_DFS | \
	 SPM_FLAG_KEEP_CSYSPWRACK_HIGH | \
	 SPM_FLAG_DISABLE_DRAMC_MCU_SRAM_SLEEP | \
	 SPM_FLAG_SRAM_SLEEP_CTRL)

#define SPM_SUSPEND_SLEEP_PCM_FLAG1	(SPM_FLAG1_DISABLE_PWRAP_CLK_SWITCH)

#define SPM_SUSPEND_PCM_FLAG \
	(SPM_FLAG_DISABLE_VCORE_DVS | \
	 SPM_FLAG_DISABLE_VCORE_DFS | \
	 SPM_FLAG_DISABLE_DRAMC_MCU_SRAM_SLEEP | \
	 SPM_FLAG_SRAM_SLEEP_CTRL)

#define SPM_SUSPEND_PCM_FLAG1	(SPM_FLAG1_DISABLE_PWRAP_CLK_SWITCH)

/* Suspend spm power control */
#define __WAKE_SRC_FOR_SUSPEND_COMMON__ ( \
	(R12_PCM_TIMER) | \
	(R12_KP_IRQ_B) | \
	(R12_APWDT_EVENT_B) | \
	(R12_MSDC_WAKEUP_B) | \
	(R12_EINT_EVENT_B) | \
	(R12_SBD_INTR_WAKEUP_B) | \
	(R12_SSPM2SPM_WAKEUP_B) | \
	(R12_SCP2SPM_WAKEUP_B) | \
	(R12_ADSP2SPM_WAKEUP_B) | \
	(R12_USBX_CDSC_B) | \
	(R12_USBX_POWERDWN_B) | \
	(R12_SYS_TIMER_EVENT_B) | \
	(R12_EINT_EVENT_SECURE_B) | \
	(R12_ECE_INT_HDMI_B) | \
	(R12_SYS_CIRQ_IRQ_B) | \
	(R12_PCIE_WAKEUPEVENT_B) | \
	(R12_SPM_CPU_WAKEUPEVENT_B) | \
	(R12_APUSYS_WAKE_HOST_B))

#if defined(CFG_MICROTRUST_TEE_SUPPORT)
#define WAKE_SRC_FOR_SUSPEND	(__WAKE_SRC_FOR_SUSPEND_COMMON__)
#else
#define WAKE_SRC_FOR_SUSPEND	(__WAKE_SRC_FOR_SUSPEND_COMMON__ | R12_SEJ_EVENT_B)
#endif

static struct pwr_ctrl suspend_ctrl = {
	.wake_src = WAKE_SRC_FOR_SUSPEND,

	/* SPM_AP_STANDBY_CON */
	/* [0] */
	.reg_wfi_op = 0,
	/* [1] */
	.reg_wfi_type = 0,
	/* [2] */
	.reg_mp0_cputop_idle_mask = 0,
	/* [3] */
	.reg_mp1_cputop_idle_mask = 0,
	/* [4] */
	.reg_mcusys_idle_mask = 0,
	/* [25] */
	.reg_md_apsrc_1_sel = 0,
	/* [26] */
	.reg_md_apsrc_0_sel = 0,
	/* [29] */
	.reg_conn_apsrc_sel = 0,

	/* SPM_SRC_REQ */
	/* [0] */
	.reg_spm_apsrc_req = 0,
	/* [1] */
	.reg_spm_f26m_req = 0,
	/* [3] */
	.reg_spm_infra_req = 0,
	/* [4] */
	.reg_spm_vrf18_req = 0,
	/* [7] */
	.reg_spm_ddr_en_req = 0,
	/* [8] */
	.reg_spm_dvfs_req = 0,
	/* [9] */
	.reg_spm_sw_mailbox_req = 0,
	/* [10] */
	.reg_spm_sspm_mailbox_req = 0,
	/* [11] */
	.reg_spm_adsp_mailbox_req = 0,
	/* [12] */
	.reg_spm_scp_mailbox_req = 0,

	/* SPM_SRC_MASK */
	/* [0] */
	.reg_sspm_srcclkena_0_mask_b = 1,
	/* [1] */
	.reg_sspm_infra_req_0_mask_b = 1,
	/* [2] */
	.reg_sspm_apsrc_req_0_mask_b = 0,
	/* [3] */
	.reg_sspm_vrf18_req_0_mask_b = 0,
	/* [4] */
	.reg_sspm_ddr_en_0_mask_b = 0,
	/* [5] */
	.reg_scp_srcclkena_mask_b = 1,
	/* [6] */
	.reg_scp_infra_req_mask_b = 1,
	/* [7] */
	.reg_scp_apsrc_req_mask_b = 1,
	/* [8] */
	.reg_scp_vrf18_req_mask_b = 1,
	/* [9] */
	.reg_scp_ddr_en_mask_b = 1,
	/* [10] */
	.reg_audio_dsp_srcclkena_mask_b = 1,
	/* [11] */
	.reg_audio_dsp_infra_req_mask_b = 1,
	/* [12] */
	.reg_audio_dsp_apsrc_req_mask_b = 1,
	/* [13] */
	.reg_audio_dsp_vrf18_req_mask_b = 1,
	/* [14] */
	.reg_audio_dsp_ddr_en_mask_b = 1,
	/* [15] */
	.reg_apu_srcclkena_mask_b = 1,
	/* [16] */
	.reg_apu_infra_req_mask_b = 1,
	/* [17] */
	.reg_apu_apsrc_req_mask_b = 0,
	/* [18] */
	.reg_apu_vrf18_req_mask_b = 1,
	/* [19] */
	.reg_apu_ddr_en_mask_b = 1,
	/* [20] */
	.reg_cpueb_srcclkena_mask_b = 1,
	/* [21] */
	.reg_cpueb_infra_req_mask_b = 1,
	/* [22] */
	.reg_cpueb_apsrc_req_mask_b = 1,
	/* [23] */
	.reg_cpueb_vrf18_req_mask_b = 1,
	/* [24] */
	.reg_cpueb_ddr_en_mask_b = 1,
	/* [25] */
	.reg_bak_psri_srcclkena_mask_b = 0,
	/* [26] */
	.reg_bak_psri_infra_req_mask_b = 0,
	/* [27] */
	.reg_bak_psri_apsrc_req_mask_b = 0,
	/* [28] */
	.reg_bak_psri_vrf18_req_mask_b = 0,
	/* [29] */
	.reg_bak_psri_ddr_en_mask_b = 0,
	/* [30] */
	.reg_cam_ddren_req_mask_b = 0,
	/* [31] */
	.reg_img_ddren_req_mask_b = 0,

	/* SPM_SRC2_MASK */
	/* [0] */
	.reg_msdc0_srcclkena_mask_b = 1,
	/* [1] */
	.reg_msdc0_infra_req_mask_b = 1,
	/* [2] */
	.reg_msdc0_apsrc_req_mask_b = 1,
	/* [3] */
	.reg_msdc0_vrf18_req_mask_b = 1,
	/* [4] */
	.reg_msdc0_ddr_en_mask_b = 1,
	/* [5] */
	.reg_msdc1_srcclkena_mask_b = 1,
	/* [6] */
	.reg_msdc1_infra_req_mask_b = 1,
	/* [7] */
	.reg_msdc1_apsrc_req_mask_b = 1,
	/* [8] */
	.reg_msdc1_vrf18_req_mask_b = 1,
	/* [9] */
	.reg_msdc1_ddr_en_mask_b = 1,
	/* [10] */
	.reg_msdc2_srcclkena_mask_b = 1,
	/* [11] */
	.reg_msdc2_infra_req_mask_b = 1,
	/* [12] */
	.reg_msdc2_apsrc_req_mask_b = 1,
	/* [13] */
	.reg_msdc2_vrf18_req_mask_b = 1,
	/* [14] */
	.reg_msdc2_ddr_en_mask_b = 1,
	/* [15] */
	.reg_ufs_srcclkena_mask_b = 1,
	/* [16] */
	.reg_ufs_infra_req_mask_b = 1,
	/* [17] */
	.reg_ufs_apsrc_req_mask_b = 1,
	/* [18] */
	.reg_ufs_vrf18_req_mask_b = 1,
	/* [19] */
	.reg_ufs_ddr_en_mask_b = 1,
	/* [20] */
	.reg_usb_srcclkena_mask_b = 1,
	/* [21] */
	.reg_usb_infra_req_mask_b = 1,
	/* [22] */
	.reg_usb_apsrc_req_mask_b = 1,
	/* [23] */
	.reg_usb_vrf18_req_mask_b = 1,
	/* [24] */
	.reg_usb_ddr_en_mask_b = 1,
	/* [25] */
	.reg_pextp_p0_srcclkena_mask_b = 1,
	/* [26] */
	.reg_pextp_p0_infra_req_mask_b = 1,
	/* [27] */
	.reg_pextp_p0_apsrc_req_mask_b = 1,
	/* [28] */
	.reg_pextp_p0_vrf18_req_mask_b = 1,
	/* [29] */
	.reg_pextp_p0_ddr_en_mask_b = 1,

	/* SPM_SRC3_MASK */
	/* [0] */
	.reg_pextp_p1_srcclkena_mask_b = 1,
	/* [1] */
	.reg_pextp_p1_infra_req_mask_b = 1,
	/* [2] */
	.reg_pextp_p1_apsrc_req_mask_b = 1,
	/* [3] */
	.reg_pextp_p1_vrf18_req_mask_b = 1,
	/* [4] */
	.reg_pextp_p1_ddr_en_mask_b = 1,
	/* [5] */
	.reg_gce0_infra_req_mask_b = 1,
	/* [6] */
	.reg_gce0_apsrc_req_mask_b = 1,
	/* [7] */
	.reg_gce0_vrf18_req_mask_b = 1,
	/* [8] */
	.reg_gce0_ddr_en_mask_b = 1,
	/* [9] */
	.reg_gce1_infra_req_mask_b = 1,
	/* [10] */
	.reg_gce1_apsrc_req_mask_b = 1,
	/* [11] */
	.reg_gce1_vrf18_req_mask_b = 1,
	/* [12] */
	.reg_gce1_ddr_en_mask_b = 1,
	/* [13] */
	.reg_spm_srcclkena_reserved_mask_b = 1,
	/* [14] */
	.reg_spm_infra_req_reserved_mask_b = 1,
	/* [15] */
	.reg_spm_apsrc_req_reserved_mask_b = 1,
	/* [16] */
	.reg_spm_vrf18_req_reserved_mask_b = 1,
	/* [17] */
	.reg_spm_ddr_en_reserved_mask_b = 1,
	/* [18] */
	.reg_disp0_apsrc_req_mask_b = 1,
	/* [19] */
	.reg_disp0_ddr_en_mask_b = 1,
	/* [20] */
	.reg_disp1_apsrc_req_mask_b = 1,
	/* [21] */
	.reg_disp1_ddr_en_mask_b = 1,
	/* [22] */
	.reg_disp2_apsrc_req_mask_b = 1,
	/* [23] */
	.reg_disp2_ddr_en_mask_b = 1,
	/* [24] */
	.reg_disp3_apsrc_req_mask_b = 1,
	/* [25] */
	.reg_disp3_ddr_en_mask_b = 1,
	/* [26] */
	.reg_infrasys_apsrc_req_mask_b = 0,
	/* [27] */
	.reg_infrasys_ddr_en_mask_b = 1,

	/* [28] */
	.reg_cg_check_srcclkena_mask_b = 1,
	/* [29] */
	.reg_cg_check_apsrc_req_mask_b = 1,
	/* [30] */
	.reg_cg_check_vrf18_req_mask_b = 1,
	/* [31] */
	.reg_cg_check_ddr_en_mask_b = 1,

	/* SPM_SRC4_MASK */
	/* [8:0] */
	.reg_mcusys_merge_apsrc_req_mask_b = 0,
	/* [17:9] */
	.reg_mcusys_merge_ddr_en_mask_b = 0,
	/* [19:18] */
	.reg_dramc_md32_infra_req_mask_b = 3,
	/* [21:20] */
	.reg_dramc_md32_vrf18_req_mask_b = 3,
	/* [23:22] */
	.reg_dramc_md32_ddr_en_mask_b = 0,
	/* [24] */
	.reg_dvfsrc_event_trigger_mask_b = 1,

	/* SPM_WAKEUP_EVENT_MASK2 */
	/* [3:0] */
	.reg_sc_sw2spm_wakeup_mask_b = 0,
	/* [4] */
	.reg_sc_adsp2spm_wakeup_mask_b = 0,
	/* [8:5] */
	.reg_sc_sspm2spm_wakeup_mask_b = 0,
	/* [9] */
	.reg_sc_scp2spm_wakeup_mask_b = 0,
	/* [10] */
	.reg_csyspwrup_ack_mask = 0,
	/* [11] */
	.reg_csyspwrup_req_mask = 1,

	/* SPM_WAKEUP_EVENT_MASK */
	/* [31:0] */
	.reg_wakeup_event_mask = 0xC1382213,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	/* [31:0] */
	.reg_ext_wakeup_event_mask = 0xFFFFFFFF,

	/*sw flag setting */
	.pcm_flags = SPM_SUSPEND_PCM_FLAG,
	.pcm_flags1 = SPM_SUSPEND_PCM_FLAG1,
};

struct spm_lp_scen __spm_suspend = {
	.pwrctrl = &suspend_ctrl,
};

int mt_spm_suspend_mode_set(int mode, void *prv)
{
	if (mode == MT_SPM_SUSPEND_SLEEP) {
		suspend_ctrl.pcm_flags = SPM_SUSPEND_SLEEP_PCM_FLAG;
		suspend_ctrl.pcm_flags1 = SPM_SUSPEND_SLEEP_PCM_FLAG1;
	} else {
		suspend_ctrl.pcm_flags = SPM_SUSPEND_PCM_FLAG;
		suspend_ctrl.pcm_flags1 = SPM_SUSPEND_PCM_FLAG1;
	}
	return 0;
}

int mt_spm_suspend_enter(int state_id, unsigned int ext_opand, unsigned int reosuce_req)
{
	int ret = 0;

	/* if FMAudio, ADSP is active, change to sleep suspend mode */
	if ((ext_opand & MT_SPM_EX_OP_SET_SUSPEND_MODE) != 0U) {
		mt_spm_suspend_mode_set(MT_SPM_SUSPEND_SLEEP, NULL);
	}

	if ((ext_opand & MT_SPM_EX_OP_PERI_ON) != 0U) {
		suspend_ctrl.pcm_flags |= SPM_FLAG_PERI_ON_IN_SUSPEND;
	} else {
		suspend_ctrl.pcm_flags &= ~SPM_FLAG_PERI_ON_IN_SUSPEND;
	}

	if ((ext_opand & MT_SPM_EX_OP_INFRA_ON) != 0U) {
		suspend_ctrl.pcm_flags |= SPM_FLAG_DISABLE_INFRA_PDN;
	} else {
		suspend_ctrl.pcm_flags &= ~SPM_FLAG_DISABLE_INFRA_PDN;
	}

#ifndef MTK_PLAT_SPM_UART_UNSUPPORT
	/* Notify UART to sleep */
	mtk_uart_save();
#endif

	ret = spm_conservation(state_id, ext_opand, &__spm_suspend, reosuce_req);
	if (ret == 0) {
		struct mt_lp_publish_event event = {
			.id = MT_LPM_PUBEVENTS_SYS_POWER_OFF,
			.val.u32 = 0U,
		};

		MT_LP_SUSPEND_PUBLISH_EVENT(&event);
	}
	return ret;
}

void mt_spm_suspend_resume(int state_id, unsigned int ext_opand, struct wake_status **status)
{
	struct mt_lp_publish_event event = {
		.id = MT_LPM_PUBEVENTS_SYS_POWER_ON,
		.val.u32 = 0U,
	};

	struct wake_status *st = NULL;

	spm_conservation_finish(state_id, ext_opand, &__spm_suspend, &st);

#ifndef MTK_PLAT_SPM_UART_UNSUPPORT
	/* Notify UART to wakeup */
	mtk_uart_restore();
#endif

	/* If FMAudio, ADSP is active, change back to suspend mode and counting in resume */
	if ((ext_opand & MT_SPM_EX_OP_SET_SUSPEND_MODE) != 0U) {
		mt_spm_suspend_mode_set(MT_SPM_SUSPEND_SYSTEM_PDN, NULL);
	}

	if (status != NULL) {
		*status = st;
	}
	MT_LP_SUSPEND_PUBLISH_EVENT(&event);
}
