// SPDX-License-Identifier: MIT
/*
 * Copyright © 2020 Intel Corporation
 */

#include "display/intel_audio_regs.h"
#include "display/intel_backlight_regs.h"
#include "display/intel_color_regs.h"
#include "display/intel_display_types.h"
#include "display/intel_dmc_regs.h"
#include "display/intel_dp_aux_regs.h"
#include "display/intel_dpio_phy.h"
#include "display/intel_fdi_regs.h"
#include "display/intel_lvds_regs.h"
#include "display/intel_psr_regs.h"
#include "display/skl_watermark_regs.h"
#include "display/vlv_dsi_pll_regs.h"
#include "gt/intel_gt_regs.h"
#include "gvt/gvt.h"

#include "i915_drv.h"
#include "i915_pvinfo.h"
#include "i915_reg.h"
#include "intel_gvt.h"
#include "intel_mchbar_regs.h"

#define MMIO_F(reg, s) do { \
	int ret; \
	ret = iter->handle_mmio_cb(iter, i915_mmio_reg_offset(reg), s); \
	if (ret) \
		return ret; \
} while (0)

#define MMIO_D(reg) MMIO_F(reg, 4)

#define MMIO_RING_F(prefix, s) do { \
	MMIO_F(prefix(RENDER_RING_BASE), s); \
	MMIO_F(prefix(BLT_RING_BASE), s); \
	MMIO_F(prefix(GEN6_BSD_RING_BASE), s); \
	MMIO_F(prefix(VEBOX_RING_BASE), s); \
	if (HAS_ENGINE(to_gt(iter->i915), VCS1)) \
		MMIO_F(prefix(GEN8_BSD2_RING_BASE), s); \
} while (0)

#define MMIO_RING_D(prefix) \
	MMIO_RING_F(prefix, 4)

static int iterate_generic_mmio(struct intel_gvt_mmio_table_iter *iter)
{
	struct drm_i915_private *dev_priv = iter->i915;

	MMIO_RING_D(RING_IMR);
	MMIO_D(SDEIMR);
	MMIO_D(SDEIER);
	MMIO_D(SDEIIR);
	MMIO_D(SDEISR);
	MMIO_RING_D(RING_HWSTAM);
	MMIO_D(BSD_HWS_PGA_GEN7);
	MMIO_D(BLT_HWS_PGA_GEN7);
	MMIO_D(VEBOX_HWS_PGA_GEN7);

#define RING_REG(base) _MMIO((base) + 0x28)
	MMIO_RING_D(RING_REG);
#undef RING_REG

#define RING_REG(base) _MMIO((base) + 0x134)
	MMIO_RING_D(RING_REG);
#undef RING_REG

#define RING_REG(base) _MMIO((base) + 0x6c)
	MMIO_RING_D(RING_REG);
#undef RING_REG
	MMIO_D(_MMIO(0x2148));
	MMIO_D(CCID(RENDER_RING_BASE));
	MMIO_D(_MMIO(0x12198));
	MMIO_D(GEN7_CXT_SIZE);
	MMIO_RING_D(RING_TAIL);
	MMIO_RING_D(RING_HEAD);
	MMIO_RING_D(RING_CTL);
	MMIO_RING_D(RING_ACTHD);
	MMIO_RING_D(RING_START);

	/* RING MODE */
#define RING_REG(base) _MMIO((base) + 0x29c)
	MMIO_RING_D(RING_REG);
#undef RING_REG

	MMIO_RING_D(RING_MI_MODE);
	MMIO_RING_D(RING_INSTPM);
	MMIO_RING_D(RING_TIMESTAMP);
	MMIO_RING_D(RING_TIMESTAMP_UDW);
	MMIO_D(GEN7_GT_MODE);
	MMIO_D(CACHE_MODE_0_GEN7);
	MMIO_D(CACHE_MODE_1);
	MMIO_D(CACHE_MODE_0);
	MMIO_D(_MMIO(0x2124));
	MMIO_D(_MMIO(0x20dc));
	MMIO_D(_3D_CHICKEN3);
	MMIO_D(_MMIO(0x2088));
	MMIO_D(FF_SLICE_CS_CHICKEN2);
	MMIO_D(_MMIO(0x2470));
	MMIO_D(GAM_ECOCHK);
	MMIO_D(GEN7_COMMON_SLICE_CHICKEN1);
	MMIO_D(COMMON_SLICE_CHICKEN2);
	MMIO_D(_MMIO(0x9030));
	MMIO_D(_MMIO(0x20a0));
	MMIO_D(_MMIO(0x2420));
	MMIO_D(_MMIO(0x2430));
	MMIO_D(_MMIO(0x2434));
	MMIO_D(_MMIO(0x2438));
	MMIO_D(_MMIO(0x243c));
	MMIO_D(_MMIO(0x7018));
	MMIO_D(HSW_HALF_SLICE_CHICKEN3);
	MMIO_D(GEN7_HALF_SLICE_CHICKEN1);
	/* display */
	MMIO_F(_MMIO(0x60220), 0x20);
	MMIO_D(_MMIO(0x602a0));
	MMIO_D(_MMIO(0x65050));
	MMIO_D(_MMIO(0x650b4));
	MMIO_D(_MMIO(0xc4040));
	MMIO_D(DERRMR);
	MMIO_D(PIPEDSL(PIPE_A));
	MMIO_D(PIPEDSL(PIPE_B));
	MMIO_D(PIPEDSL(PIPE_C));
	MMIO_D(PIPEDSL(_PIPE_EDP));
	MMIO_D(TRANSCONF(TRANSCODER_A));
	MMIO_D(TRANSCONF(TRANSCODER_B));
	MMIO_D(TRANSCONF(TRANSCODER_C));
	MMIO_D(TRANSCONF(TRANSCODER_EDP));
	MMIO_D(PIPESTAT(PIPE_A));
	MMIO_D(PIPESTAT(PIPE_B));
	MMIO_D(PIPESTAT(PIPE_C));
	MMIO_D(PIPESTAT(_PIPE_EDP));
	MMIO_D(PIPE_FLIPCOUNT_G4X(PIPE_A));
	MMIO_D(PIPE_FLIPCOUNT_G4X(PIPE_B));
	MMIO_D(PIPE_FLIPCOUNT_G4X(PIPE_C));
	MMIO_D(PIPE_FLIPCOUNT_G4X(_PIPE_EDP));
	MMIO_D(PIPE_FRMCOUNT_G4X(PIPE_A));
	MMIO_D(PIPE_FRMCOUNT_G4X(PIPE_B));
	MMIO_D(PIPE_FRMCOUNT_G4X(PIPE_C));
	MMIO_D(PIPE_FRMCOUNT_G4X(_PIPE_EDP));
	MMIO_D(CURCNTR(PIPE_A));
	MMIO_D(CURCNTR(PIPE_B));
	MMIO_D(CURCNTR(PIPE_C));
	MMIO_D(CURPOS(PIPE_A));
	MMIO_D(CURPOS(PIPE_B));
	MMIO_D(CURPOS(PIPE_C));
	MMIO_D(CURBASE(PIPE_A));
	MMIO_D(CURBASE(PIPE_B));
	MMIO_D(CURBASE(PIPE_C));
	MMIO_D(CUR_FBC_CTL(PIPE_A));
	MMIO_D(CUR_FBC_CTL(PIPE_B));
	MMIO_D(CUR_FBC_CTL(PIPE_C));
	MMIO_D(_MMIO(0x700ac));
	MMIO_D(_MMIO(0x710ac));
	MMIO_D(_MMIO(0x720ac));
	MMIO_D(_MMIO(0x70090));
	MMIO_D(_MMIO(0x70094));
	MMIO_D(_MMIO(0x70098));
	MMIO_D(_MMIO(0x7009c));
	MMIO_D(DSPCNTR(PIPE_A));
	MMIO_D(DSPADDR(PIPE_A));
	MMIO_D(DSPSTRIDE(PIPE_A));
	MMIO_D(DSPPOS(PIPE_A));
	MMIO_D(DSPSIZE(PIPE_A));
	MMIO_D(DSPSURF(PIPE_A));
	MMIO_D(DSPOFFSET(PIPE_A));
	MMIO_D(DSPSURFLIVE(PIPE_A));
	MMIO_D(REG_50080(PIPE_A, PLANE_PRIMARY));
	MMIO_D(DSPCNTR(PIPE_B));
	MMIO_D(DSPADDR(PIPE_B));
	MMIO_D(DSPSTRIDE(PIPE_B));
	MMIO_D(DSPPOS(PIPE_B));
	MMIO_D(DSPSIZE(PIPE_B));
	MMIO_D(DSPSURF(PIPE_B));
	MMIO_D(DSPOFFSET(PIPE_B));
	MMIO_D(DSPSURFLIVE(PIPE_B));
	MMIO_D(REG_50080(PIPE_B, PLANE_PRIMARY));
	MMIO_D(DSPCNTR(PIPE_C));
	MMIO_D(DSPADDR(PIPE_C));
	MMIO_D(DSPSTRIDE(PIPE_C));
	MMIO_D(DSPPOS(PIPE_C));
	MMIO_D(DSPSIZE(PIPE_C));
	MMIO_D(DSPSURF(PIPE_C));
	MMIO_D(DSPOFFSET(PIPE_C));
	MMIO_D(DSPSURFLIVE(PIPE_C));
	MMIO_D(REG_50080(PIPE_C, PLANE_PRIMARY));
	MMIO_D(SPRCTL(PIPE_A));
	MMIO_D(SPRLINOFF(PIPE_A));
	MMIO_D(SPRSTRIDE(PIPE_A));
	MMIO_D(SPRPOS(PIPE_A));
	MMIO_D(SPRSIZE(PIPE_A));
	MMIO_D(SPRKEYVAL(PIPE_A));
	MMIO_D(SPRKEYMSK(PIPE_A));
	MMIO_D(SPRSURF(PIPE_A));
	MMIO_D(SPRKEYMAX(PIPE_A));
	MMIO_D(SPROFFSET(PIPE_A));
	MMIO_D(SPRSCALE(PIPE_A));
	MMIO_D(SPRSURFLIVE(PIPE_A));
	MMIO_D(REG_50080(PIPE_A, PLANE_SPRITE0));
	MMIO_D(SPRCTL(PIPE_B));
	MMIO_D(SPRLINOFF(PIPE_B));
	MMIO_D(SPRSTRIDE(PIPE_B));
	MMIO_D(SPRPOS(PIPE_B));
	MMIO_D(SPRSIZE(PIPE_B));
	MMIO_D(SPRKEYVAL(PIPE_B));
	MMIO_D(SPRKEYMSK(PIPE_B));
	MMIO_D(SPRSURF(PIPE_B));
	MMIO_D(SPRKEYMAX(PIPE_B));
	MMIO_D(SPROFFSET(PIPE_B));
	MMIO_D(SPRSCALE(PIPE_B));
	MMIO_D(SPRSURFLIVE(PIPE_B));
	MMIO_D(REG_50080(PIPE_B, PLANE_SPRITE0));
	MMIO_D(SPRCTL(PIPE_C));
	MMIO_D(SPRLINOFF(PIPE_C));
	MMIO_D(SPRSTRIDE(PIPE_C));
	MMIO_D(SPRPOS(PIPE_C));
	MMIO_D(SPRSIZE(PIPE_C));
	MMIO_D(SPRKEYVAL(PIPE_C));
	MMIO_D(SPRKEYMSK(PIPE_C));
	MMIO_D(SPRSURF(PIPE_C));
	MMIO_D(SPRKEYMAX(PIPE_C));
	MMIO_D(SPROFFSET(PIPE_C));
	MMIO_D(SPRSCALE(PIPE_C));
	MMIO_D(SPRSURFLIVE(PIPE_C));
	MMIO_D(REG_50080(PIPE_C, PLANE_SPRITE0));
	MMIO_D(TRANS_HTOTAL(TRANSCODER_A));
	MMIO_D(TRANS_HBLANK(TRANSCODER_A));
	MMIO_D(TRANS_HSYNC(TRANSCODER_A));
	MMIO_D(TRANS_VTOTAL(TRANSCODER_A));
	MMIO_D(TRANS_VBLANK(TRANSCODER_A));
	MMIO_D(TRANS_VSYNC(TRANSCODER_A));
	MMIO_D(BCLRPAT(TRANSCODER_A));
	MMIO_D(TRANS_VSYNCSHIFT(TRANSCODER_A));
	MMIO_D(PIPESRC(TRANSCODER_A));
	MMIO_D(TRANS_HTOTAL(TRANSCODER_B));
	MMIO_D(TRANS_HBLANK(TRANSCODER_B));
	MMIO_D(TRANS_HSYNC(TRANSCODER_B));
	MMIO_D(TRANS_VTOTAL(TRANSCODER_B));
	MMIO_D(TRANS_VBLANK(TRANSCODER_B));
	MMIO_D(TRANS_VSYNC(TRANSCODER_B));
	MMIO_D(BCLRPAT(TRANSCODER_B));
	MMIO_D(TRANS_VSYNCSHIFT(TRANSCODER_B));
	MMIO_D(PIPESRC(TRANSCODER_B));
	MMIO_D(TRANS_HTOTAL(TRANSCODER_C));
	MMIO_D(TRANS_HBLANK(TRANSCODER_C));
	MMIO_D(TRANS_HSYNC(TRANSCODER_C));
	MMIO_D(TRANS_VTOTAL(TRANSCODER_C));
	MMIO_D(TRANS_VBLANK(TRANSCODER_C));
	MMIO_D(TRANS_VSYNC(TRANSCODER_C));
	MMIO_D(BCLRPAT(TRANSCODER_C));
	MMIO_D(TRANS_VSYNCSHIFT(TRANSCODER_C));
	MMIO_D(PIPESRC(TRANSCODER_C));
	MMIO_D(TRANS_HTOTAL(TRANSCODER_EDP));
	MMIO_D(TRANS_HBLANK(TRANSCODER_EDP));
	MMIO_D(TRANS_HSYNC(TRANSCODER_EDP));
	MMIO_D(TRANS_VTOTAL(TRANSCODER_EDP));
	MMIO_D(TRANS_VBLANK(TRANSCODER_EDP));
	MMIO_D(TRANS_VSYNC(TRANSCODER_EDP));
	MMIO_D(BCLRPAT(TRANSCODER_EDP));
	MMIO_D(TRANS_VSYNCSHIFT(TRANSCODER_EDP));
	MMIO_D(PIPE_DATA_M1(TRANSCODER_A));
	MMIO_D(PIPE_DATA_N1(TRANSCODER_A));
	MMIO_D(PIPE_DATA_M2(TRANSCODER_A));
	MMIO_D(PIPE_DATA_N2(TRANSCODER_A));
	MMIO_D(PIPE_LINK_M1(TRANSCODER_A));
	MMIO_D(PIPE_LINK_N1(TRANSCODER_A));
	MMIO_D(PIPE_LINK_M2(TRANSCODER_A));
	MMIO_D(PIPE_LINK_N2(TRANSCODER_A));
	MMIO_D(PIPE_DATA_M1(TRANSCODER_B));
	MMIO_D(PIPE_DATA_N1(TRANSCODER_B));
	MMIO_D(PIPE_DATA_M2(TRANSCODER_B));
	MMIO_D(PIPE_DATA_N2(TRANSCODER_B));
	MMIO_D(PIPE_LINK_M1(TRANSCODER_B));
	MMIO_D(PIPE_LINK_N1(TRANSCODER_B));
	MMIO_D(PIPE_LINK_M2(TRANSCODER_B));
	MMIO_D(PIPE_LINK_N2(TRANSCODER_B));
	MMIO_D(PIPE_DATA_M1(TRANSCODER_C));
	MMIO_D(PIPE_DATA_N1(TRANSCODER_C));
	MMIO_D(PIPE_DATA_M2(TRANSCODER_C));
	MMIO_D(PIPE_DATA_N2(TRANSCODER_C));
	MMIO_D(PIPE_LINK_M1(TRANSCODER_C));
	MMIO_D(PIPE_LINK_N1(TRANSCODER_C));
	MMIO_D(PIPE_LINK_M2(TRANSCODER_C));
	MMIO_D(PIPE_LINK_N2(TRANSCODER_C));
	MMIO_D(PIPE_DATA_M1(TRANSCODER_EDP));
	MMIO_D(PIPE_DATA_N1(TRANSCODER_EDP));
	MMIO_D(PIPE_DATA_M2(TRANSCODER_EDP));
	MMIO_D(PIPE_DATA_N2(TRANSCODER_EDP));
	MMIO_D(PIPE_LINK_M1(TRANSCODER_EDP));
	MMIO_D(PIPE_LINK_N1(TRANSCODER_EDP));
	MMIO_D(PIPE_LINK_M2(TRANSCODER_EDP));
	MMIO_D(PIPE_LINK_N2(TRANSCODER_EDP));
	MMIO_D(PF_CTL(PIPE_A));
	MMIO_D(PF_WIN_SZ(PIPE_A));
	MMIO_D(PF_WIN_POS(PIPE_A));
	MMIO_D(PF_VSCALE(PIPE_A));
	MMIO_D(PF_HSCALE(PIPE_A));
	MMIO_D(PF_CTL(PIPE_B));
	MMIO_D(PF_WIN_SZ(PIPE_B));
	MMIO_D(PF_WIN_POS(PIPE_B));
	MMIO_D(PF_VSCALE(PIPE_B));
	MMIO_D(PF_HSCALE(PIPE_B));
	MMIO_D(PF_CTL(PIPE_C));
	MMIO_D(PF_WIN_SZ(PIPE_C));
	MMIO_D(PF_WIN_POS(PIPE_C));
	MMIO_D(PF_VSCALE(PIPE_C));
	MMIO_D(PF_HSCALE(PIPE_C));
	MMIO_D(WM0_PIPE_ILK(PIPE_A));
	MMIO_D(WM0_PIPE_ILK(PIPE_B));
	MMIO_D(WM0_PIPE_ILK(PIPE_C));
	MMIO_D(WM1_LP_ILK);
	MMIO_D(WM2_LP_ILK);
	MMIO_D(WM3_LP_ILK);
	MMIO_D(WM1S_LP_ILK);
	MMIO_D(WM2S_LP_IVB);
	MMIO_D(WM3S_LP_IVB);
	MMIO_D(BLC_PWM_CPU_CTL2);
	MMIO_D(BLC_PWM_CPU_CTL);
	MMIO_D(BLC_PWM_PCH_CTL1);
	MMIO_D(BLC_PWM_PCH_CTL2);
	MMIO_D(_MMIO(0x48268));
	MMIO_F(PCH_GMBUS0, 4 * 4);
	MMIO_F(PCH_GPIO_BASE, 6 * 4);
	MMIO_F(_MMIO(0xe4f00), 0x28);
	MMIO_D(_MMIO(_PCH_TRANSACONF));
	MMIO_D(_MMIO(_PCH_TRANSBCONF));
	MMIO_D(FDI_RX_IIR(PIPE_A));
	MMIO_D(FDI_RX_IIR(PIPE_B));
	MMIO_D(FDI_RX_IIR(PIPE_C));
	MMIO_D(FDI_RX_IMR(PIPE_A));
	MMIO_D(FDI_RX_IMR(PIPE_B));
	MMIO_D(FDI_RX_IMR(PIPE_C));
	MMIO_D(FDI_RX_CTL(PIPE_A));
	MMIO_D(FDI_RX_CTL(PIPE_B));
	MMIO_D(FDI_RX_CTL(PIPE_C));
	MMIO_D(_MMIO(_PCH_TRANS_HTOTAL_A));
	MMIO_D(_MMIO(_PCH_TRANS_HBLANK_A));
	MMIO_D(_MMIO(_PCH_TRANS_HSYNC_A));
	MMIO_D(_MMIO(_PCH_TRANS_VTOTAL_A));
	MMIO_D(_MMIO(_PCH_TRANS_VBLANK_A));
	MMIO_D(_MMIO(_PCH_TRANS_VSYNC_A));
	MMIO_D(_MMIO(_PCH_TRANS_VSYNCSHIFT_A));
	MMIO_D(_MMIO(_PCH_TRANS_HTOTAL_B));
	MMIO_D(_MMIO(_PCH_TRANS_HBLANK_B));
	MMIO_D(_MMIO(_PCH_TRANS_HSYNC_B));
	MMIO_D(_MMIO(_PCH_TRANS_VTOTAL_B));
	MMIO_D(_MMIO(_PCH_TRANS_VBLANK_B));
	MMIO_D(_MMIO(_PCH_TRANS_VSYNC_B));
	MMIO_D(_MMIO(_PCH_TRANS_VSYNCSHIFT_B));
	MMIO_D(_MMIO(_PCH_TRANSA_DATA_M1));
	MMIO_D(_MMIO(_PCH_TRANSA_DATA_N1));
	MMIO_D(_MMIO(_PCH_TRANSA_DATA_M2));
	MMIO_D(_MMIO(_PCH_TRANSA_DATA_N2));
	MMIO_D(_MMIO(_PCH_TRANSA_LINK_M1));
	MMIO_D(_MMIO(_PCH_TRANSA_LINK_N1));
	MMIO_D(_MMIO(_PCH_TRANSA_LINK_M2));
	MMIO_D(_MMIO(_PCH_TRANSA_LINK_N2));
	MMIO_D(TRANS_DP_CTL(PIPE_A));
	MMIO_D(TRANS_DP_CTL(PIPE_B));
	MMIO_D(TRANS_DP_CTL(PIPE_C));
	MMIO_D(TVIDEO_DIP_CTL(PIPE_A));
	MMIO_D(TVIDEO_DIP_DATA(PIPE_A));
	MMIO_D(TVIDEO_DIP_GCP(PIPE_A));
	MMIO_D(TVIDEO_DIP_CTL(PIPE_B));
	MMIO_D(TVIDEO_DIP_DATA(PIPE_B));
	MMIO_D(TVIDEO_DIP_GCP(PIPE_B));
	MMIO_D(TVIDEO_DIP_CTL(PIPE_C));
	MMIO_D(TVIDEO_DIP_DATA(PIPE_C));
	MMIO_D(TVIDEO_DIP_GCP(PIPE_C));
	MMIO_D(_MMIO(_FDI_RXA_MISC));
	MMIO_D(_MMIO(_FDI_RXB_MISC));
	MMIO_D(_MMIO(_FDI_RXA_TUSIZE1));
	MMIO_D(_MMIO(_FDI_RXA_TUSIZE2));
	MMIO_D(_MMIO(_FDI_RXB_TUSIZE1));
	MMIO_D(_MMIO(_FDI_RXB_TUSIZE2));
	MMIO_D(PCH_PP_CONTROL);
	MMIO_D(PCH_PP_DIVISOR);
	MMIO_D(PCH_PP_STATUS);
	MMIO_D(PCH_LVDS);
	MMIO_D(_MMIO(_PCH_DPLL_A));
	MMIO_D(_MMIO(_PCH_DPLL_B));
	MMIO_D(_MMIO(_PCH_FPA0));
	MMIO_D(_MMIO(_PCH_FPA1));
	MMIO_D(_MMIO(_PCH_FPB0));
	MMIO_D(_MMIO(_PCH_FPB1));
	MMIO_D(PCH_DREF_CONTROL);
	MMIO_D(PCH_RAWCLK_FREQ);
	MMIO_D(PCH_DPLL_SEL);
	MMIO_D(_MMIO(0x61208));
	MMIO_D(_MMIO(0x6120c));
	MMIO_D(PCH_PP_ON_DELAYS);
	MMIO_D(PCH_PP_OFF_DELAYS);
	MMIO_D(_MMIO(0xe651c));
	MMIO_D(_MMIO(0xe661c));
	MMIO_D(_MMIO(0xe671c));
	MMIO_D(_MMIO(0xe681c));
	MMIO_D(_MMIO(0xe6c04));
	MMIO_D(_MMIO(0xe6e1c));
	MMIO_D(PCH_PORT_HOTPLUG);
	MMIO_D(LCPLL_CTL);
	MMIO_D(FUSE_STRAP);
	MMIO_D(DIGITAL_PORT_HOTPLUG_CNTRL);
	MMIO_D(DISP_ARB_CTL);
	MMIO_D(DISP_ARB_CTL2);
	MMIO_D(ILK_DISPLAY_CHICKEN1);
	MMIO_D(ILK_DISPLAY_CHICKEN2);
	MMIO_D(ILK_DSPCLK_GATE_D);
	MMIO_D(SOUTH_CHICKEN1);
	MMIO_D(SOUTH_CHICKEN2);
	MMIO_D(_MMIO(_TRANSA_CHICKEN1));
	MMIO_D(_MMIO(_TRANSB_CHICKEN1));
	MMIO_D(SOUTH_DSPCLK_GATE_D);
	MMIO_D(_MMIO(_TRANSA_CHICKEN2));
	MMIO_D(_MMIO(_TRANSB_CHICKEN2));
	MMIO_D(ILK_DPFC_CB_BASE(INTEL_FBC_A));
	MMIO_D(ILK_DPFC_CONTROL(INTEL_FBC_A));
	MMIO_D(ILK_DPFC_RECOMP_CTL(INTEL_FBC_A));
	MMIO_D(ILK_DPFC_STATUS(INTEL_FBC_A));
	MMIO_D(ILK_DPFC_FENCE_YOFF(INTEL_FBC_A));
	MMIO_D(ILK_DPFC_CHICKEN(INTEL_FBC_A));
	MMIO_D(ILK_FBC_RT_BASE);
	MMIO_D(IPS_CTL);
	MMIO_D(PIPE_CSC_COEFF_RY_GY(PIPE_A));
	MMIO_D(PIPE_CSC_COEFF_BY(PIPE_A));
	MMIO_D(PIPE_CSC_COEFF_RU_GU(PIPE_A));
	MMIO_D(PIPE_CSC_COEFF_BU(PIPE_A));
	MMIO_D(PIPE_CSC_COEFF_RV_GV(PIPE_A));
	MMIO_D(PIPE_CSC_COEFF_BV(PIPE_A));
	MMIO_D(PIPE_CSC_MODE(PIPE_A));
	MMIO_D(PIPE_CSC_PREOFF_HI(PIPE_A));
	MMIO_D(PIPE_CSC_PREOFF_ME(PIPE_A));
	MMIO_D(PIPE_CSC_PREOFF_LO(PIPE_A));
	MMIO_D(PIPE_CSC_POSTOFF_HI(PIPE_A));
	MMIO_D(PIPE_CSC_POSTOFF_ME(PIPE_A));
	MMIO_D(PIPE_CSC_POSTOFF_LO(PIPE_A));
	MMIO_D(PIPE_CSC_COEFF_RY_GY(PIPE_B));
	MMIO_D(PIPE_CSC_COEFF_BY(PIPE_B));
	MMIO_D(PIPE_CSC_COEFF_RU_GU(PIPE_B));
	MMIO_D(PIPE_CSC_COEFF_BU(PIPE_B));
	MMIO_D(PIPE_CSC_COEFF_RV_GV(PIPE_B));
	MMIO_D(PIPE_CSC_COEFF_BV(PIPE_B));
	MMIO_D(PIPE_CSC_MODE(PIPE_B));
	MMIO_D(PIPE_CSC_PREOFF_HI(PIPE_B));
	MMIO_D(PIPE_CSC_PREOFF_ME(PIPE_B));
	MMIO_D(PIPE_CSC_PREOFF_LO(PIPE_B));
	MMIO_D(PIPE_CSC_POSTOFF_HI(PIPE_B));
	MMIO_D(PIPE_CSC_POSTOFF_ME(PIPE_B));
	MMIO_D(PIPE_CSC_POSTOFF_LO(PIPE_B));
	MMIO_D(PIPE_CSC_COEFF_RY_GY(PIPE_C));
	MMIO_D(PIPE_CSC_COEFF_BY(PIPE_C));
	MMIO_D(PIPE_CSC_COEFF_RU_GU(PIPE_C));
	MMIO_D(PIPE_CSC_COEFF_BU(PIPE_C));
	MMIO_D(PIPE_CSC_COEFF_RV_GV(PIPE_C));
	MMIO_D(PIPE_CSC_COEFF_BV(PIPE_C));
	MMIO_D(PIPE_CSC_MODE(PIPE_C));
	MMIO_D(PIPE_CSC_PREOFF_HI(PIPE_C));
	MMIO_D(PIPE_CSC_PREOFF_ME(PIPE_C));
	MMIO_D(PIPE_CSC_PREOFF_LO(PIPE_C));
	MMIO_D(PIPE_CSC_POSTOFF_HI(PIPE_C));
	MMIO_D(PIPE_CSC_POSTOFF_ME(PIPE_C));
	MMIO_D(PIPE_CSC_POSTOFF_LO(PIPE_C));
	MMIO_D(PREC_PAL_INDEX(PIPE_A));
	MMIO_D(PREC_PAL_DATA(PIPE_A));
	MMIO_F(PREC_PAL_GC_MAX(PIPE_A, 0), 4 * 3);
	MMIO_D(PREC_PAL_INDEX(PIPE_B));
	MMIO_D(PREC_PAL_DATA(PIPE_B));
	MMIO_F(PREC_PAL_GC_MAX(PIPE_B, 0), 4 * 3);
	MMIO_D(PREC_PAL_INDEX(PIPE_C));
	MMIO_D(PREC_PAL_DATA(PIPE_C));
	MMIO_F(PREC_PAL_GC_MAX(PIPE_C, 0), 4 * 3);
	MMIO_D(_MMIO(0x60110));
	MMIO_D(_MMIO(0x61110));
	MMIO_F(_MMIO(0x70400), 0x40);
	MMIO_F(_MMIO(0x71400), 0x40);
	MMIO_F(_MMIO(0x72400), 0x40);
	MMIO_D(WM_LINETIME(PIPE_A));
	MMIO_D(WM_LINETIME(PIPE_B));
	MMIO_D(WM_LINETIME(PIPE_C));
	MMIO_D(SPLL_CTL);
	MMIO_D(_MMIO(_WRPLL_CTL1));
	MMIO_D(_MMIO(_WRPLL_CTL2));
	MMIO_D(PORT_CLK_SEL(PORT_A));
	MMIO_D(PORT_CLK_SEL(PORT_B));
	MMIO_D(PORT_CLK_SEL(PORT_C));
	MMIO_D(PORT_CLK_SEL(PORT_D));
	MMIO_D(PORT_CLK_SEL(PORT_E));
	MMIO_D(TRANS_CLK_SEL(TRANSCODER_A));
	MMIO_D(TRANS_CLK_SEL(TRANSCODER_B));
	MMIO_D(TRANS_CLK_SEL(TRANSCODER_C));
	MMIO_D(HSW_NDE_RSTWRN_OPT);
	MMIO_D(_MMIO(0x46508));
	MMIO_D(_MMIO(0x49080));
	MMIO_D(_MMIO(0x49180));
	MMIO_D(_MMIO(0x49280));
	MMIO_F(_MMIO(0x49090), 0x14);
	MMIO_F(_MMIO(0x49190), 0x14);
	MMIO_F(_MMIO(0x49290), 0x14);
	MMIO_D(GAMMA_MODE(PIPE_A));
	MMIO_D(GAMMA_MODE(PIPE_B));
	MMIO_D(GAMMA_MODE(PIPE_C));
	MMIO_D(TRANS_MULT(TRANSCODER_A));
	MMIO_D(TRANS_MULT(TRANSCODER_B));
	MMIO_D(TRANS_MULT(TRANSCODER_C));
	MMIO_D(HSW_TVIDEO_DIP_CTL(TRANSCODER_A));
	MMIO_D(HSW_TVIDEO_DIP_CTL(TRANSCODER_B));
	MMIO_D(HSW_TVIDEO_DIP_CTL(TRANSCODER_C));
	MMIO_D(SFUSE_STRAP);
	MMIO_D(SBI_ADDR);
	MMIO_D(SBI_DATA);
	MMIO_D(SBI_CTL_STAT);
	MMIO_D(PIXCLK_GATE);
	MMIO_F(_MMIO(_DPA_AUX_CH_CTL), 6 * 4);
	MMIO_D(DDI_BUF_CTL(PORT_A));
	MMIO_D(DDI_BUF_CTL(PORT_B));
	MMIO_D(DDI_BUF_CTL(PORT_C));
	MMIO_D(DDI_BUF_CTL(PORT_D));
	MMIO_D(DDI_BUF_CTL(PORT_E));
	MMIO_D(DP_TP_CTL(PORT_A));
	MMIO_D(DP_TP_CTL(PORT_B));
	MMIO_D(DP_TP_CTL(PORT_C));
	MMIO_D(DP_TP_CTL(PORT_D));
	MMIO_D(DP_TP_CTL(PORT_E));
	MMIO_D(DP_TP_STATUS(PORT_A));
	MMIO_D(DP_TP_STATUS(PORT_B));
	MMIO_D(DP_TP_STATUS(PORT_C));
	MMIO_D(DP_TP_STATUS(PORT_D));
	MMIO_D(DP_TP_STATUS(PORT_E));
	MMIO_F(_MMIO(_DDI_BUF_TRANS_A), 0x50);
	MMIO_F(_MMIO(0x64e60), 0x50);
	MMIO_F(_MMIO(0x64eC0), 0x50);
	MMIO_F(_MMIO(0x64f20), 0x50);
	MMIO_F(_MMIO(0x64f80), 0x50);
	MMIO_D(HSW_AUD_CFG(PIPE_A));
	MMIO_D(HSW_AUD_PIN_ELD_CP_VLD);
	MMIO_D(HSW_AUD_MISC_CTRL(PIPE_A));
	MMIO_D(_MMIO(_TRANS_DDI_FUNC_CTL_A));
	MMIO_D(_MMIO(_TRANS_DDI_FUNC_CTL_B));
	MMIO_D(_MMIO(_TRANS_DDI_FUNC_CTL_C));
	MMIO_D(_MMIO(_TRANS_DDI_FUNC_CTL_EDP));
	MMIO_D(_MMIO(_TRANSA_MSA_MISC));
	MMIO_D(_MMIO(_TRANSB_MSA_MISC));
	MMIO_D(_MMIO(_TRANSC_MSA_MISC));
	MMIO_D(_MMIO(_TRANS_EDP_MSA_MISC));
	MMIO_D(FORCEWAKE);
	MMIO_D(FORCEWAKE_ACK);
	MMIO_D(GEN6_GT_CORE_STATUS);
	MMIO_D(GEN6_GT_THREAD_STATUS_REG);
	MMIO_D(GTFIFODBG);
	MMIO_D(GTFIFOCTL);
	MMIO_D(ECOBUS);
	MMIO_D(GEN6_RC_CONTROL);
	MMIO_D(GEN6_RC_STATE);
	MMIO_D(GEN6_RPNSWREQ);
	MMIO_D(GEN6_RC_VIDEO_FREQ);
	MMIO_D(GEN6_RP_DOWN_TIMEOUT);
	MMIO_D(GEN6_RP_INTERRUPT_LIMITS);
	MMIO_D(GEN6_RPSTAT1);
	MMIO_D(GEN6_RP_CONTROL);
	MMIO_D(GEN6_RP_UP_THRESHOLD);
	MMIO_D(GEN6_RP_DOWN_THRESHOLD);
	MMIO_D(GEN6_RP_CUR_UP_EI);
	MMIO_D(GEN6_RP_CUR_UP);
	MMIO_D(GEN6_RP_PREV_UP);
	MMIO_D(GEN6_RP_CUR_DOWN_EI);
	MMIO_D(GEN6_RP_CUR_DOWN);
	MMIO_D(GEN6_RP_PREV_DOWN);
	MMIO_D(GEN6_RP_UP_EI);
	MMIO_D(GEN6_RP_DOWN_EI);
	MMIO_D(GEN6_RP_IDLE_HYSTERSIS);
	MMIO_D(GEN6_RC1_WAKE_RATE_LIMIT);
	MMIO_D(GEN6_RC6_WAKE_RATE_LIMIT);
	MMIO_D(GEN6_RC6pp_WAKE_RATE_LIMIT);
	MMIO_D(GEN6_RC_EVALUATION_INTERVAL);
	MMIO_D(GEN6_RC_IDLE_HYSTERSIS);
	MMIO_D(GEN6_RC_SLEEP);
	MMIO_D(GEN6_RC1e_THRESHOLD);
	MMIO_D(GEN6_RC6_THRESHOLD);
	MMIO_D(GEN6_RC6p_THRESHOLD);
	MMIO_D(GEN6_RC6pp_THRESHOLD);
	MMIO_D(GEN6_PMINTRMSK);

	MMIO_D(RSTDBYCTL);
	MMIO_D(GEN6_GDRST);
	MMIO_F(FENCE_REG_GEN6_LO(0), 0x80);
	MMIO_D(CPU_VGACNTRL);
	MMIO_D(TILECTL);
	MMIO_D(GEN6_UCGCTL1);
	MMIO_D(GEN6_UCGCTL2);
	MMIO_F(_MMIO(0x4f000), 0x90);
	MMIO_D(GEN6_PCODE_DATA);
	MMIO_D(_MMIO(0x13812c));
	MMIO_D(GEN7_ERR_INT);
	MMIO_D(HSW_EDRAM_CAP);
	MMIO_D(HSW_IDICR);
	MMIO_D(GFX_FLSH_CNTL_GEN6);
	MMIO_D(_MMIO(0x3c));
	MMIO_D(_MMIO(0x860));
	MMIO_D(ECOSKPD(RENDER_RING_BASE));
	MMIO_D(_MMIO(0x121d0));
	MMIO_D(ECOSKPD(BLT_RING_BASE));
	MMIO_D(_MMIO(0x41d0));
	MMIO_D(GAC_ECO_BITS);
	MMIO_D(_MMIO(0x6200));
	MMIO_D(_MMIO(0x6204));
	MMIO_D(_MMIO(0x6208));
	MMIO_D(_MMIO(0x7118));
	MMIO_D(_MMIO(0x7180));
	MMIO_D(_MMIO(0x7408));
	MMIO_D(_MMIO(0x7c00));
	MMIO_D(GEN6_MBCTL);
	MMIO_D(_MMIO(0x911c));
	MMIO_D(_MMIO(0x9120));
	MMIO_D(GEN7_UCGCTL4);
	MMIO_D(GAB_CTL);
	MMIO_D(_MMIO(0x48800));
	MMIO_D(_MMIO(0xce044));
	MMIO_D(_MMIO(0xe6500));
	MMIO_D(_MMIO(0xe6504));
	MMIO_D(_MMIO(0xe6600));
	MMIO_D(_MMIO(0xe6604));
	MMIO_D(_MMIO(0xe6700));
	MMIO_D(_MMIO(0xe6704));
	MMIO_D(_MMIO(0xe6800));
	MMIO_D(_MMIO(0xe6804));
	MMIO_D(PCH_GMBUS4);
	MMIO_D(PCH_GMBUS5);
	MMIO_D(_MMIO(0x902c));
	MMIO_D(_MMIO(0xec008));
	MMIO_D(_MMIO(0xec00c));
	MMIO_D(_MMIO(0xec008 + 0x18));
	MMIO_D(_MMIO(0xec00c + 0x18));
	MMIO_D(_MMIO(0xec008 + 0x18 * 2));
	MMIO_D(_MMIO(0xec00c + 0x18 * 2));
	MMIO_D(_MMIO(0xec008 + 0x18 * 3));
	MMIO_D(_MMIO(0xec00c + 0x18 * 3));
	MMIO_D(_MMIO(0xec408));
	MMIO_D(_MMIO(0xec40c));
	MMIO_D(_MMIO(0xec408 + 0x18));
	MMIO_D(_MMIO(0xec40c + 0x18));
	MMIO_D(_MMIO(0xec408 + 0x18 * 2));
	MMIO_D(_MMIO(0xec40c + 0x18 * 2));
	MMIO_D(_MMIO(0xec408 + 0x18 * 3));
	MMIO_D(_MMIO(0xec40c + 0x18 * 3));
	MMIO_D(_MMIO(0xfc810));
	MMIO_D(_MMIO(0xfc81c));
	MMIO_D(_MMIO(0xfc828));
	MMIO_D(_MMIO(0xfc834));
	MMIO_D(_MMIO(0xfcc00));
	MMIO_D(_MMIO(0xfcc0c));
	MMIO_D(_MMIO(0xfcc18));
	MMIO_D(_MMIO(0xfcc24));
	MMIO_D(_MMIO(0xfd000));
	MMIO_D(_MMIO(0xfd00c));
	MMIO_D(_MMIO(0xfd018));
	MMIO_D(_MMIO(0xfd024));
	MMIO_D(_MMIO(0xfd034));
	MMIO_D(FPGA_DBG);
	MMIO_D(_MMIO(0x2054));
	MMIO_D(_MMIO(0x12054));
	MMIO_D(_MMIO(0x22054));
	MMIO_D(_MMIO(0x1a054));
	MMIO_D(_MMIO(0x44070));
	MMIO_D(_MMIO(0x2178));
	MMIO_D(_MMIO(0x217c));
	MMIO_D(_MMIO(0x12178));
	MMIO_D(_MMIO(0x1217c));
	MMIO_F(_MMIO(0x5200), 32);
	MMIO_F(_MMIO(0x5240), 32);
	MMIO_F(_MMIO(0x5280), 16);
	MMIO_D(BCS_SWCTRL);
	MMIO_F(HS_INVOCATION_COUNT, 8);
	MMIO_F(DS_INVOCATION_COUNT, 8);
	MMIO_F(IA_VERTICES_COUNT, 8);
	MMIO_F(IA_PRIMITIVES_COUNT, 8);
	MMIO_F(VS_INVOCATION_COUNT, 8);
	MMIO_F(GS_INVOCATION_COUNT, 8);
	MMIO_F(GS_PRIMITIVES_COUNT, 8);
	MMIO_F(CL_INVOCATION_COUNT, 8);
	MMIO_F(CL_PRIMITIVES_COUNT, 8);
	MMIO_F(PS_INVOCATION_COUNT, 8);
	MMIO_F(PS_DEPTH_COUNT, 8);
	MMIO_D(ARB_MODE);
	MMIO_RING_D(RING_BBADDR);
	MMIO_D(_MMIO(0x2220));
	MMIO_D(_MMIO(0x12220));
	MMIO_D(_MMIO(0x22220));
	MMIO_RING_D(RING_SYNC_1);
	MMIO_RING_D(RING_SYNC_0);
	MMIO_D(GUC_STATUS);

	MMIO_F(_MMIO(MCHBAR_MIRROR_BASE_SNB), 0x40000);
	MMIO_F(_MMIO(VGT_PVINFO_PAGE), VGT_PVINFO_SIZE);
	MMIO_F(LGC_PALETTE(PIPE_A, 0), 1024);
	MMIO_F(LGC_PALETTE(PIPE_B, 0), 1024);
	MMIO_F(LGC_PALETTE(PIPE_C, 0), 1024);

	return 0;
}

static int iterate_bdw_only_mmio(struct intel_gvt_mmio_table_iter *iter)
{
	MMIO_D(HSW_PWR_WELL_CTL1);
	MMIO_D(HSW_PWR_WELL_CTL2);
	MMIO_D(HSW_PWR_WELL_CTL3);
	MMIO_D(HSW_PWR_WELL_CTL4);
	MMIO_D(HSW_PWR_WELL_CTL5);
	MMIO_D(HSW_PWR_WELL_CTL6);

	MMIO_D(WM_MISC);
	MMIO_D(_MMIO(_SRD_CTL_EDP));

	MMIO_D(_MMIO(0xb1f0));
	MMIO_D(_MMIO(0xb1c0));
	MMIO_D(_MMIO(0xb100));
	MMIO_D(_MMIO(0xb10c));
	MMIO_D(_MMIO(0xb110));
	MMIO_D(_MMIO(0x83a4));
	MMIO_D(_MMIO(0x8430));
	MMIO_D(_MMIO(0x2248));
	MMIO_D(FORCEWAKE_ACK_HSW);

	return 0;
}

static int iterate_bdw_plus_mmio(struct intel_gvt_mmio_table_iter *iter)
{
	struct drm_i915_private *dev_priv = iter->i915;

	MMIO_D(GEN8_GT_IMR(0));
	MMIO_D(GEN8_GT_IER(0));
	MMIO_D(GEN8_GT_IIR(0));
	MMIO_D(GEN8_GT_ISR(0));
	MMIO_D(GEN8_GT_IMR(1));
	MMIO_D(GEN8_GT_IER(1));
	MMIO_D(GEN8_GT_IIR(1));
	MMIO_D(GEN8_GT_ISR(1));
	MMIO_D(GEN8_GT_IMR(2));
	MMIO_D(GEN8_GT_IER(2));
	MMIO_D(GEN8_GT_IIR(2));
	MMIO_D(GEN8_GT_ISR(2));
	MMIO_D(GEN8_GT_IMR(3));
	MMIO_D(GEN8_GT_IER(3));
	MMIO_D(GEN8_GT_IIR(3));
	MMIO_D(GEN8_GT_ISR(3));
	MMIO_D(GEN8_DE_PIPE_IMR(PIPE_A));
	MMIO_D(GEN8_DE_PIPE_IER(PIPE_A));
	MMIO_D(GEN8_DE_PIPE_IIR(PIPE_A));
	MMIO_D(GEN8_DE_PIPE_ISR(PIPE_A));
	MMIO_D(GEN8_DE_PIPE_IMR(PIPE_B));
	MMIO_D(GEN8_DE_PIPE_IER(PIPE_B));
	MMIO_D(GEN8_DE_PIPE_IIR(PIPE_B));
	MMIO_D(GEN8_DE_PIPE_ISR(PIPE_B));
	MMIO_D(GEN8_DE_PIPE_IMR(PIPE_C));
	MMIO_D(GEN8_DE_PIPE_IER(PIPE_C));
	MMIO_D(GEN8_DE_PIPE_IIR(PIPE_C));
	MMIO_D(GEN8_DE_PIPE_ISR(PIPE_C));
	MMIO_D(GEN8_DE_PORT_IMR);
	MMIO_D(GEN8_DE_PORT_IER);
	MMIO_D(GEN8_DE_PORT_IIR);
	MMIO_D(GEN8_DE_PORT_ISR);
	MMIO_D(GEN8_DE_MISC_IMR);
	MMIO_D(GEN8_DE_MISC_IER);
	MMIO_D(GEN8_DE_MISC_IIR);
	MMIO_D(GEN8_DE_MISC_ISR);
	MMIO_D(GEN8_PCU_IMR);
	MMIO_D(GEN8_PCU_IER);
	MMIO_D(GEN8_PCU_IIR);
	MMIO_D(GEN8_PCU_ISR);
	MMIO_D(GEN8_MASTER_IRQ);
	MMIO_RING_D(RING_ACTHD_UDW);

#define RING_REG(base) _MMIO((base) + 0xd0)
	MMIO_RING_D(RING_REG);
#undef RING_REG

#define RING_REG(base) _MMIO((base) + 0x230)
	MMIO_RING_D(RING_REG);
#undef RING_REG

#define RING_REG(base) _MMIO((base) + 0x234)
	MMIO_RING_F(RING_REG, 8);
#undef RING_REG

#define RING_REG(base) _MMIO((base) + 0x244)
	MMIO_RING_D(RING_REG);
#undef RING_REG

#define RING_REG(base) _MMIO((base) + 0x370)
	MMIO_RING_F(RING_REG, 48);
#undef RING_REG

#define RING_REG(base) _MMIO((base) + 0x3a0)
	MMIO_RING_D(RING_REG);
#undef RING_REG

	MMIO_D(PIPE_MISC(PIPE_A));
	MMIO_D(PIPE_MISC(PIPE_B));
	MMIO_D(PIPE_MISC(PIPE_C));
	MMIO_D(_MMIO(0x1c1d0));
	MMIO_D(GEN6_MBCUNIT_SNPCR);
	MMIO_D(GEN7_MISCCPCTL);
	MMIO_D(_MMIO(0x1c054));
	MMIO_D(GEN6_PCODE_MAILBOX);
	if (!IS_BROXTON(dev_priv))
		MMIO_D(GEN8_PRIVATE_PAT_LO);
	MMIO_D(GEN8_PRIVATE_PAT_HI);
	MMIO_D(GAMTARBMODE);

#define RING_REG(base) _MMIO((base) + 0x270)
	MMIO_RING_F(RING_REG, 32);
#undef RING_REG

	MMIO_RING_D(RING_HWS_PGA);
	MMIO_D(HDC_CHICKEN0);
	MMIO_D(CHICKEN_PIPESL_1(PIPE_A));
	MMIO_D(CHICKEN_PIPESL_1(PIPE_B));
	MMIO_D(CHICKEN_PIPESL_1(PIPE_C));
	MMIO_D(_MMIO(0x6671c));
	MMIO_D(_MMIO(0x66c00));
	MMIO_D(_MMIO(0x66c04));
	MMIO_D(HSW_GTT_CACHE_EN);
	MMIO_D(GEN8_EU_DISABLE0);
	MMIO_D(GEN8_EU_DISABLE1);
	MMIO_D(GEN8_EU_DISABLE2);
	MMIO_D(_MMIO(0xfdc));
	MMIO_D(GEN8_ROW_CHICKEN);
	MMIO_D(GEN7_ROW_CHICKEN2);
	MMIO_D(GEN8_UCGCTL6);
	MMIO_D(GEN8_L3SQCREG4);
	MMIO_D(GEN9_SCRATCH_LNCF1);
	MMIO_F(_MMIO(0x24d0), 48);
	MMIO_D(_MMIO(0x44484));
	MMIO_D(_MMIO(0x4448c));
	MMIO_D(GEN8_L3_LRA_1_GPGPU);
	MMIO_D(_MMIO(0x110000));
	MMIO_D(_MMIO(0x48400));
	MMIO_D(_MMIO(0x6e570));
	MMIO_D(_MMIO(0x65f10));
	MMIO_D(_MMIO(0xe194));
	MMIO_D(_MMIO(0xe188));
	MMIO_D(HALF_SLICE_CHICKEN2);
	MMIO_D(_MMIO(0x2580));
	MMIO_D(_MMIO(0xe220));
	MMIO_D(_MMIO(0xe230));
	MMIO_D(_MMIO(0xe240));
	MMIO_D(_MMIO(0xe260));
	MMIO_D(_MMIO(0xe270));
	MMIO_D(_MMIO(0xe280));
	MMIO_D(_MMIO(0xe2a0));
	MMIO_D(_MMIO(0xe2b0));
	MMIO_D(_MMIO(0xe2c0));
	MMIO_D(_MMIO(0x21f0));
	MMIO_D(GEN8_GAMW_ECO_DEV_RW_IA);
	MMIO_D(_MMIO(0x215c));
	MMIO_F(_MMIO(0x2290), 8);
	MMIO_D(_MMIO(0x2b00));
	MMIO_D(_MMIO(0x2360));
	MMIO_D(_MMIO(0x1c17c));
	MMIO_D(_MMIO(0x1c178));
	MMIO_D(_MMIO(0x4260));
	MMIO_D(_MMIO(0x4264));
	MMIO_D(_MMIO(0x4268));
	MMIO_D(_MMIO(0x426c));
	MMIO_D(_MMIO(0x4270));
	MMIO_D(_MMIO(0x4094));
	MMIO_D(_MMIO(0x22178));
	MMIO_D(_MMIO(0x1a178));
	MMIO_D(_MMIO(0x1a17c));
	MMIO_D(_MMIO(0x2217c));
	MMIO_D(EDP_PSR_IMR);
	MMIO_D(EDP_PSR_IIR);
	MMIO_D(_MMIO(0xe4cc));
	MMIO_D(GEN7_SC_INSTDONE);

	return 0;
}

static int iterate_pre_skl_mmio(struct intel_gvt_mmio_table_iter *iter)
{
	MMIO_D(FORCEWAKE_MT);

	MMIO_D(PCH_ADPA);
	MMIO_F(_MMIO(_PCH_DPB_AUX_CH_CTL), 6 * 4);
	MMIO_F(_MMIO(_PCH_DPC_AUX_CH_CTL), 6 * 4);
	MMIO_F(_MMIO(_PCH_DPD_AUX_CH_CTL), 6 * 4);

	MMIO_F(_MMIO(0x70440), 0xc);
	MMIO_F(_MMIO(0x71440), 0xc);
	MMIO_F(_MMIO(0x72440), 0xc);
	MMIO_F(_MMIO(0x7044c), 0xc);
	MMIO_F(_MMIO(0x7144c), 0xc);
	MMIO_F(_MMIO(0x7244c), 0xc);

	return 0;
}

static int iterate_skl_plus_mmio(struct intel_gvt_mmio_table_iter *iter)
{
	struct drm_i915_private *dev_priv = iter->i915;

	MMIO_D(FORCEWAKE_RENDER_GEN9);
	MMIO_D(FORCEWAKE_ACK_RENDER_GEN9);
	MMIO_D(FORCEWAKE_GT_GEN9);
	MMIO_D(FORCEWAKE_ACK_GT_GEN9);
	MMIO_D(FORCEWAKE_MEDIA_GEN9);
	MMIO_D(FORCEWAKE_ACK_MEDIA_GEN9);
	MMIO_F(DP_AUX_CH_CTL(AUX_CH_B), 6 * 4);
	MMIO_F(DP_AUX_CH_CTL(AUX_CH_C), 6 * 4);
	MMIO_F(DP_AUX_CH_CTL(AUX_CH_D), 6 * 4);
	MMIO_D(HSW_PWR_WELL_CTL1);
	MMIO_D(HSW_PWR_WELL_CTL2);
	MMIO_D(DBUF_CTL_S(0));
	MMIO_D(GEN9_PG_ENABLE);
	MMIO_D(GEN9_MEDIA_PG_IDLE_HYSTERESIS);
	MMIO_D(GEN9_RENDER_PG_IDLE_HYSTERESIS);
	MMIO_D(GEN9_GAMT_ECO_REG_RW_IA);
	MMIO_D(MMCD_MISC_CTRL);
	MMIO_D(CHICKEN_PAR1_1);
	MMIO_D(DC_STATE_EN);
	MMIO_D(DC_STATE_DEBUG);
	MMIO_D(CDCLK_CTL);
	MMIO_D(LCPLL1_CTL);
	MMIO_D(LCPLL2_CTL);
	MMIO_D(_MMIO(_DPLL1_CFGCR1));
	MMIO_D(_MMIO(_DPLL2_CFGCR1));
	MMIO_D(_MMIO(_DPLL3_CFGCR1));
	MMIO_D(_MMIO(_DPLL1_CFGCR2));
	MMIO_D(_MMIO(_DPLL2_CFGCR2));
	MMIO_D(_MMIO(_DPLL3_CFGCR2));
	MMIO_D(DPLL_CTRL1);
	MMIO_D(DPLL_CTRL2);
	MMIO_D(DPLL_STATUS);
	MMIO_D(SKL_PS_WIN_POS(PIPE_A, 0));
	MMIO_D(SKL_PS_WIN_POS(PIPE_A, 1));
	MMIO_D(SKL_PS_WIN_POS(PIPE_B, 0));
	MMIO_D(SKL_PS_WIN_POS(PIPE_B, 1));
	MMIO_D(SKL_PS_WIN_POS(PIPE_C, 0));
	MMIO_D(SKL_PS_WIN_POS(PIPE_C, 1));
	MMIO_D(SKL_PS_WIN_SZ(PIPE_A, 0));
	MMIO_D(SKL_PS_WIN_SZ(PIPE_A, 1));
	MMIO_D(SKL_PS_WIN_SZ(PIPE_B, 0));
	MMIO_D(SKL_PS_WIN_SZ(PIPE_B, 1));
	MMIO_D(SKL_PS_WIN_SZ(PIPE_C, 0));
	MMIO_D(SKL_PS_WIN_SZ(PIPE_C, 1));
	MMIO_D(SKL_PS_CTRL(PIPE_A, 0));
	MMIO_D(SKL_PS_CTRL(PIPE_A, 1));
	MMIO_D(SKL_PS_CTRL(PIPE_B, 0));
	MMIO_D(SKL_PS_CTRL(PIPE_B, 1));
	MMIO_D(SKL_PS_CTRL(PIPE_C, 0));
	MMIO_D(SKL_PS_CTRL(PIPE_C, 1));
	MMIO_D(PLANE_BUF_CFG(PIPE_A, 0));
	MMIO_D(PLANE_BUF_CFG(PIPE_A, 1));
	MMIO_D(PLANE_BUF_CFG(PIPE_A, 2));
	MMIO_D(PLANE_BUF_CFG(PIPE_A, 3));
	MMIO_D(PLANE_BUF_CFG(PIPE_B, 0));
	MMIO_D(PLANE_BUF_CFG(PIPE_B, 1));
	MMIO_D(PLANE_BUF_CFG(PIPE_B, 2));
	MMIO_D(PLANE_BUF_CFG(PIPE_B, 3));
	MMIO_D(PLANE_BUF_CFG(PIPE_C, 0));
	MMIO_D(PLANE_BUF_CFG(PIPE_C, 1));
	MMIO_D(PLANE_BUF_CFG(PIPE_C, 2));
	MMIO_D(PLANE_BUF_CFG(PIPE_C, 3));
	MMIO_D(CUR_BUF_CFG(PIPE_A));
	MMIO_D(CUR_BUF_CFG(PIPE_B));
	MMIO_D(CUR_BUF_CFG(PIPE_C));
	MMIO_F(PLANE_WM(PIPE_A, 0, 0), 4 * 8);
	MMIO_F(PLANE_WM(PIPE_A, 1, 0), 4 * 8);
	MMIO_F(PLANE_WM(PIPE_A, 2, 0), 4 * 8);
	MMIO_F(PLANE_WM(PIPE_B, 0, 0), 4 * 8);
	MMIO_F(PLANE_WM(PIPE_B, 1, 0), 4 * 8);
	MMIO_F(PLANE_WM(PIPE_B, 2, 0), 4 * 8);
	MMIO_F(PLANE_WM(PIPE_C, 0, 0), 4 * 8);
	MMIO_F(PLANE_WM(PIPE_C, 1, 0), 4 * 8);
	MMIO_F(PLANE_WM(PIPE_C, 2, 0), 4 * 8);
	MMIO_F(CUR_WM(PIPE_A, 0), 4 * 8);
	MMIO_F(CUR_WM(PIPE_B, 0), 4 * 8);
	MMIO_F(CUR_WM(PIPE_C, 0), 4 * 8);
	MMIO_D(PLANE_WM_TRANS(PIPE_A, 0));
	MMIO_D(PLANE_WM_TRANS(PIPE_A, 1));
	MMIO_D(PLANE_WM_TRANS(PIPE_A, 2));
	MMIO_D(PLANE_WM_TRANS(PIPE_B, 0));
	MMIO_D(PLANE_WM_TRANS(PIPE_B, 1));
	MMIO_D(PLANE_WM_TRANS(PIPE_B, 2));
	MMIO_D(PLANE_WM_TRANS(PIPE_C, 0));
	MMIO_D(PLANE_WM_TRANS(PIPE_C, 1));
	MMIO_D(PLANE_WM_TRANS(PIPE_C, 2));
	MMIO_D(CUR_WM_TRANS(PIPE_A));
	MMIO_D(CUR_WM_TRANS(PIPE_B));
	MMIO_D(CUR_WM_TRANS(PIPE_C));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_A, 0));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_A, 1));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_A, 2));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_A, 3));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_B, 0));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_B, 1));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_B, 2));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_B, 3));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_C, 0));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_C, 1));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_C, 2));
	MMIO_D(PLANE_NV12_BUF_CFG(PIPE_C, 3));
	MMIO_D(_MMIO(_REG_701C0(PIPE_A, 1)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_A, 2)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_A, 3)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_A, 4)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_B, 1)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_B, 2)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_B, 3)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_B, 4)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_C, 1)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_C, 2)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_C, 3)));
	MMIO_D(_MMIO(_REG_701C0(PIPE_C, 4)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_A, 1)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_A, 2)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_A, 3)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_A, 4)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_B, 1)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_B, 2)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_B, 3)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_B, 4)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_C, 1)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_C, 2)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_C, 3)));
	MMIO_D(_MMIO(_REG_701C4(PIPE_C, 4)));
	MMIO_D(_MMIO(_PLANE_CTL_3_A));
	MMIO_D(_MMIO(_PLANE_CTL_3_B));
	MMIO_D(_MMIO(0x72380));
	MMIO_D(_MMIO(0x7239c));
	MMIO_D(_MMIO(_PLANE_SURF_3_A));
	MMIO_D(_MMIO(_PLANE_SURF_3_B));
	MMIO_D(DMC_SSP_BASE);
	MMIO_D(DMC_HTP_SKL);
	MMIO_D(DMC_LAST_WRITE);
	MMIO_D(BDW_SCRATCH1);
	MMIO_D(SKL_DFSM);
	MMIO_D(DISPIO_CR_TX_BMU_CR0);
	MMIO_F(GEN9_GFX_MOCS(0), 0x7f8);
	MMIO_F(GEN7_L3CNTLREG2, 0x80);
	MMIO_D(RPM_CONFIG0);
	MMIO_D(_MMIO(0xd08));
	MMIO_D(RC6_LOCATION);
	MMIO_D(GEN7_FF_SLICE_CS_CHICKEN1);
	MMIO_D(GEN9_CS_DEBUG_MODE1);
	/* TRTT */
	MMIO_D(TRVATTL3PTRDW(0));
	MMIO_D(TRVATTL3PTRDW(1));
	MMIO_D(TRVATTL3PTRDW(2));
	MMIO_D(TRVATTL3PTRDW(3));
	MMIO_D(TRVADR);
	MMIO_D(TRTTE);
	MMIO_D(_MMIO(0x4dfc));
	MMIO_D(_MMIO(0x46430));
	MMIO_D(_MMIO(0x46520));
	MMIO_D(_MMIO(0xc403c));
	MMIO_D(GEN8_GARBCNTL);
	MMIO_D(DMA_CTRL);
	MMIO_D(_MMIO(0x65900));
	MMIO_D(GEN6_STOLEN_RESERVED);
	MMIO_D(_MMIO(0x4068));
	MMIO_D(_MMIO(0x67054));
	MMIO_D(_MMIO(0x6e560));
	MMIO_D(_MMIO(0x6e554));
	MMIO_D(_MMIO(0x2b20));
	MMIO_D(_MMIO(0x65f00));
	MMIO_D(_MMIO(0x65f08));
	MMIO_D(_MMIO(0x320f0));
	MMIO_D(_MMIO(0x70034));
	MMIO_D(_MMIO(0x71034));
	MMIO_D(_MMIO(0x72034));
	MMIO_D(_MMIO(_PLANE_KEYVAL_1(PIPE_A)));
	MMIO_D(_MMIO(_PLANE_KEYVAL_1(PIPE_B)));
	MMIO_D(_MMIO(_PLANE_KEYVAL_1(PIPE_C)));
	MMIO_D(_MMIO(_PLANE_KEYMAX_1(PIPE_A)));
	MMIO_D(_MMIO(_PLANE_KEYMAX_1(PIPE_B)));
	MMIO_D(_MMIO(_PLANE_KEYMAX_1(PIPE_C)));
	MMIO_D(_MMIO(_PLANE_KEYMSK_1(PIPE_A)));
	MMIO_D(_MMIO(_PLANE_KEYMSK_1(PIPE_B)));
	MMIO_D(_MMIO(_PLANE_KEYMSK_1(PIPE_C)));
	MMIO_D(_MMIO(0x44500));
#define CSFE_CHICKEN1_REG(base) _MMIO((base) + 0xD4)
	MMIO_RING_D(CSFE_CHICKEN1_REG);
#undef CSFE_CHICKEN1_REG
	MMIO_D(GEN8_HDC_CHICKEN1);
	MMIO_D(GEN9_WM_CHICKEN3);

	if (IS_KABYLAKE(dev_priv) ||
	    IS_COFFEELAKE(dev_priv) || IS_COMETLAKE(dev_priv))
		MMIO_D(GAMT_CHKN_BIT_REG);
	if (!IS_BROXTON(dev_priv))
		MMIO_D(GEN9_CTX_PREEMPT_REG);
	MMIO_F(_MMIO(DMC_MMIO_START_RANGE), 0x3000);
	return 0;
}

static int iterate_bxt_mmio(struct intel_gvt_mmio_table_iter *iter)
{
	struct drm_i915_private *dev_priv = iter->i915;

	MMIO_F(_MMIO(0x80000), 0x3000);
	MMIO_D(GEN7_SAMPLER_INSTDONE);
	MMIO_D(GEN7_ROW_INSTDONE);
	MMIO_D(GEN8_FAULT_TLB_DATA0);
	MMIO_D(GEN8_FAULT_TLB_DATA1);
	MMIO_D(ERROR_GEN6);
	MMIO_D(DONE_REG);
	MMIO_D(EIR);
	MMIO_D(PGTBL_ER);
	MMIO_D(_MMIO(0x4194));
	MMIO_D(_MMIO(0x4294));
	MMIO_D(_MMIO(0x4494));
	MMIO_RING_D(RING_PSMI_CTL);
	MMIO_RING_D(RING_DMA_FADD);
	MMIO_RING_D(RING_DMA_FADD_UDW);
	MMIO_RING_D(RING_IPEHR);
	MMIO_RING_D(RING_INSTPS);
	MMIO_RING_D(RING_BBADDR_UDW);
	MMIO_RING_D(RING_BBSTATE);
	MMIO_RING_D(RING_IPEIR);
	MMIO_F(SOFT_SCRATCH(0), 16 * 4);
	MMIO_D(BXT_P_CR_GT_DISP_PWRON);
	MMIO_D(BXT_RP_STATE_CAP);
	MMIO_D(BXT_PHY_CTL_FAMILY(DPIO_PHY0));
	MMIO_D(BXT_PHY_CTL_FAMILY(DPIO_PHY1));
	MMIO_D(BXT_PHY_CTL(PORT_A));
	MMIO_D(BXT_PHY_CTL(PORT_B));
	MMIO_D(BXT_PHY_CTL(PORT_C));
	MMIO_D(BXT_PORT_PLL_ENABLE(PORT_A));
	MMIO_D(BXT_PORT_PLL_ENABLE(PORT_B));
	MMIO_D(BXT_PORT_PLL_ENABLE(PORT_C));
	MMIO_D(BXT_PORT_CL1CM_DW0(DPIO_PHY0));
	MMIO_D(BXT_PORT_CL1CM_DW9(DPIO_PHY0));
	MMIO_D(BXT_PORT_CL1CM_DW10(DPIO_PHY0));
	MMIO_D(BXT_PORT_CL1CM_DW28(DPIO_PHY0));
	MMIO_D(BXT_PORT_CL1CM_DW30(DPIO_PHY0));
	MMIO_D(BXT_PORT_CL2CM_DW6(DPIO_PHY0));
	MMIO_D(BXT_PORT_REF_DW3(DPIO_PHY0));
	MMIO_D(BXT_PORT_REF_DW6(DPIO_PHY0));
	MMIO_D(BXT_PORT_REF_DW8(DPIO_PHY0));
	MMIO_D(BXT_PORT_CL1CM_DW0(DPIO_PHY1));
	MMIO_D(BXT_PORT_CL1CM_DW9(DPIO_PHY1));
	MMIO_D(BXT_PORT_CL1CM_DW10(DPIO_PHY1));
	MMIO_D(BXT_PORT_CL1CM_DW28(DPIO_PHY1));
	MMIO_D(BXT_PORT_CL1CM_DW30(DPIO_PHY1));
	MMIO_D(BXT_PORT_CL2CM_DW6(DPIO_PHY1));
	MMIO_D(BXT_PORT_REF_DW3(DPIO_PHY1));
	MMIO_D(BXT_PORT_REF_DW6(DPIO_PHY1));
	MMIO_D(BXT_PORT_REF_DW8(DPIO_PHY1));
	MMIO_D(BXT_PORT_PLL_EBB_0(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_PLL_EBB_4(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW10_LN01(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW10_GRP(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW12_LN01(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW12_LN23(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW12_GRP(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW2_LN0(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW2_GRP(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW3_LN0(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW3_GRP(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW4_LN0(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW4_GRP(DPIO_PHY0, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY0, DPIO_CH0, 0));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY0, DPIO_CH0, 1));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY0, DPIO_CH0, 2));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY0, DPIO_CH0, 3));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH0, 0));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH0, 1));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH0, 2));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH0, 3));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH0, 6));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH0, 8));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH0, 9));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH0, 10));
	MMIO_D(BXT_PORT_PLL_EBB_0(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_PLL_EBB_4(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_PCS_DW10_LN01(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_PCS_DW10_GRP(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_PCS_DW12_LN01(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_PCS_DW12_LN23(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_PCS_DW12_GRP(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_TX_DW2_LN0(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_TX_DW2_GRP(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_TX_DW3_LN0(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_TX_DW3_GRP(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_TX_DW4_LN0(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_TX_DW4_GRP(DPIO_PHY0, DPIO_CH1));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY0, DPIO_CH1, 0));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY0, DPIO_CH1, 1));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY0, DPIO_CH1, 2));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY0, DPIO_CH1, 3));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH1, 0));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH1, 1));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH1, 2));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH1, 3));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH1, 6));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH1, 8));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH1, 9));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY0, DPIO_CH1, 10));
	MMIO_D(BXT_PORT_PLL_EBB_0(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_PLL_EBB_4(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW10_LN01(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW10_GRP(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW12_LN01(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW12_LN23(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_PCS_DW12_GRP(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW2_LN0(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW2_GRP(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW3_LN0(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW3_GRP(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW4_LN0(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW4_GRP(DPIO_PHY1, DPIO_CH0));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY1, DPIO_CH0, 0));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY1, DPIO_CH0, 1));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY1, DPIO_CH0, 2));
	MMIO_D(BXT_PORT_TX_DW14_LN(DPIO_PHY1, DPIO_CH0, 3));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY1, DPIO_CH0, 0));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY1, DPIO_CH0, 1));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY1, DPIO_CH0, 2));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY1, DPIO_CH0, 3));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY1, DPIO_CH0, 6));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY1, DPIO_CH0, 8));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY1, DPIO_CH0, 9));
	MMIO_D(BXT_PORT_PLL(DPIO_PHY1, DPIO_CH0, 10));
	MMIO_D(BXT_DE_PLL_CTL);
	MMIO_D(BXT_DE_PLL_ENABLE);
	MMIO_D(BXT_DSI_PLL_CTL);
	MMIO_D(BXT_DSI_PLL_ENABLE);
	MMIO_D(GEN9_CLKGATE_DIS_0);
	MMIO_D(GEN9_CLKGATE_DIS_4);
	MMIO_D(HSW_TVIDEO_DIP_GCP(TRANSCODER_A));
	MMIO_D(HSW_TVIDEO_DIP_GCP(TRANSCODER_B));
	MMIO_D(HSW_TVIDEO_DIP_GCP(TRANSCODER_C));
	MMIO_D(RC6_CTX_BASE);
	MMIO_D(GEN8_PUSHBUS_CONTROL);
	MMIO_D(GEN8_PUSHBUS_ENABLE);
	MMIO_D(GEN8_PUSHBUS_SHIFT);
	MMIO_D(GEN6_GFXPAUSE);
	MMIO_D(GEN8_L3SQCREG1);
	MMIO_D(GEN8_L3CNTLREG);
	MMIO_D(_MMIO(0x20D8));
	MMIO_F(GEN8_RING_CS_GPR(RENDER_RING_BASE, 0), 0x40);
	MMIO_F(GEN8_RING_CS_GPR(GEN6_BSD_RING_BASE, 0), 0x40);
	MMIO_F(GEN8_RING_CS_GPR(BLT_RING_BASE, 0), 0x40);
	MMIO_F(GEN8_RING_CS_GPR(VEBOX_RING_BASE, 0), 0x40);
	MMIO_D(GEN9_CTX_PREEMPT_REG);
	MMIO_D(GEN8_PRIVATE_PAT_LO);

	return 0;
}

/**
 * intel_gvt_iterate_mmio_table - Iterate the GVT MMIO table
 * @iter: the interator
 *
 * This function is called for iterating the GVT MMIO table when i915 is
 * taking the snapshot of the HW and GVT is building MMIO tracking table.
 */
int intel_gvt_iterate_mmio_table(struct intel_gvt_mmio_table_iter *iter)
{
	struct drm_i915_private *i915 = iter->i915;
	int ret;

	ret = iterate_generic_mmio(iter);
	if (ret)
		goto err;

	if (IS_BROADWELL(i915)) {
		ret = iterate_bdw_only_mmio(iter);
		if (ret)
			goto err;
		ret = iterate_bdw_plus_mmio(iter);
		if (ret)
			goto err;
		ret = iterate_pre_skl_mmio(iter);
		if (ret)
			goto err;
	} else if (IS_SKYLAKE(i915) ||
		   IS_KABYLAKE(i915) ||
		   IS_COFFEELAKE(i915) ||
		   IS_COMETLAKE(i915)) {
		ret = iterate_bdw_plus_mmio(iter);
		if (ret)
			goto err;
		ret = iterate_skl_plus_mmio(iter);
		if (ret)
			goto err;
	} else if (IS_BROXTON(i915)) {
		ret = iterate_bdw_plus_mmio(iter);
		if (ret)
			goto err;
		ret = iterate_skl_plus_mmio(iter);
		if (ret)
			goto err;
		ret = iterate_bxt_mmio(iter);
		if (ret)
			goto err;
	}

	return 0;
err:
	return ret;
}
EXPORT_SYMBOL_NS_GPL(intel_gvt_iterate_mmio_table, I915_GVT);
