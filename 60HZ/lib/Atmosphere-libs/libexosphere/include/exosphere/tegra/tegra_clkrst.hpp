/*
 * Copyright (c) 2018-2020 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <vapours.hpp>

/* Clock source enums. */
#define CLK_RST_REG_BITS_MASK(NAME)                                      REG_NAMED_BITS_MASK    (CLK_RST_CONTROLLER, NAME)
#define CLK_RST_REG_BITS_VALUE(NAME, VALUE)                              REG_NAMED_BITS_VALUE   (CLK_RST_CONTROLLER, NAME, VALUE)
#define CLK_RST_REG_BITS_ENUM(NAME, ENUM)                                REG_NAMED_BITS_ENUM    (CLK_RST_CONTROLLER, NAME, ENUM)
#define CLK_RST_REG_BITS_ENUM_SEL(NAME, __COND__, TRUE_ENUM, FALSE_ENUM) REG_NAMED_BITS_ENUM_SEL(CLK_RST_CONTROLLER, NAME, __COND__, TRUE_ENUM, FALSE_ENUM)

#define DEFINE_CLK_RST_REG(NAME, __OFFSET__, __WIDTH__)                                                                                                                  REG_DEFINE_NAMED_REG           (CLK_RST_CONTROLLER, NAME, __OFFSET__, __WIDTH__)
#define DEFINE_CLK_RST_REG_BIT_ENUM(NAME, __OFFSET__, ZERO, ONE)                                                                                                         REG_DEFINE_NAMED_BIT_ENUM      (CLK_RST_CONTROLLER, NAME, __OFFSET__, ZERO, ONE)
#define DEFINE_CLK_RST_REG_TWO_BIT_ENUM(NAME, __OFFSET__, ZERO, ONE, TWO, THREE)                                                                                         REG_DEFINE_NAMED_TWO_BIT_ENUM  (CLK_RST_CONTROLLER, NAME, __OFFSET__, ZERO, ONE, TWO, THREE)
#define DEFINE_CLK_RST_REG_THREE_BIT_ENUM(NAME, __OFFSET__, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN)                                                               REG_DEFINE_NAMED_THREE_BIT_ENUM(CLK_RST_CONTROLLER, NAME, __OFFSET__, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN)
#define DEFINE_CLK_RST_REG_FOUR_BIT_ENUM(NAME, __OFFSET__, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, ELEVEN, TWELVE, THIRTEEN, FOURTEEN, FIFTEEN) REG_DEFINE_NAMED_FOUR_BIT_ENUM (CLK_RST_CONTROLLER, NAME, __OFFSET__, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, ELEVEN, TWELVE, THIRTEEN, FOURTEEN, FIFTEEN)


#define CLK_RST_CONTROLLER_RST_SOURCE            (0x000)

#define CLK_RST_CONTROLLER_MISC_CLK_ENB          (0x048)
#define CLK_RST_CONTROLLER_OSC_CTRL              (0x050)
#define CLK_RST_CONTROLLER_PLLX_BASE             (0x0E0)
#define CLK_RST_CONTROLLER_CCLKG_BURST_POLICY    (0x368)
#define CLK_RST_CONTROLLER_SUPER_CCLKG_DIVIDER   (0x36C)
#define CLK_RST_CONTROLLER_CCLKLP_BURST_POLICY   (0x370)
#define CLK_RST_CONTROLLER_SUPER_CCLKLP_DIVIDER  (0x374)
#define CLK_RST_CONTROLLER_CPU_SOFTRST_CTRL2     (0x388)
#define CLK_RST_CONTROLLER_SPARE_REG0            (0x55C)

#define CLK_RST_CONTROLLER_LVL2_CLK_GATE_OVRA (0x0F8)
#define CLK_RST_CONTROLLER_LVL2_CLK_GATE_OVRB (0x0FC)
#define CLK_RST_CONTROLLER_LVL2_CLK_GATE_OVRC (0x3A0)
#define CLK_RST_CONTROLLER_LVL2_CLK_GATE_OVRD (0x3A4)
#define CLK_RST_CONTROLLER_LVL2_CLK_GATE_OVRE (0x554)

DEFINE_CLK_RST_REG(MISC_CLK_ENB_CFG_ALL_VISIBLE, 28, 1);

DEFINE_CLK_RST_REG_BIT_ENUM(OSC_CTRL_XOE, 0, DISABLE, ENABLE);
DEFINE_CLK_RST_REG(OSC_CTRL_XOFS, 4, 6);

DEFINE_CLK_RST_REG_BIT_ENUM(PLLX_BASE_PLLX_ENABLE, 30, DISABLE, ENABLE);

DEFINE_CLK_RST_REG(SUPER_CCLK_DIVIDER_SUPER_CDIV_DIVISOR,  0, 8);
DEFINE_CLK_RST_REG(SUPER_CCLK_DIVIDER_SUPER_CDIV_DIVIDEND, 8, 8);
DEFINE_CLK_RST_REG_BIT_ENUM(SUPER_CCLK_DIVIDER_SUPER_CDIV_DIS_FROM_CPU_IRQ, 24, NO_IMPACT, DISABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(SUPER_CCLK_DIVIDER_SUPER_CDIV_DIS_FROM_COP_IRQ, 25, NO_IMPACT, DISABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(SUPER_CCLK_DIVIDER_SUPER_CDIV_DIS_FROM_CPU_FIQ, 26, NO_IMPACT, DISABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(SUPER_CCLK_DIVIDER_SUPER_CDIV_DIS_FROM_COP_FIQ, 27, NO_IMPACT, DISABLE);

DEFINE_CLK_RST_REG_BIT_ENUM(SUPER_CCLKG_DIVIDER_SUPER_CDIV_ENB, 31, DISABLE, ENABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(SUPER_CCLKLP_DIVIDER_SUPER_CDIV_ENB, 31, DISABLE, ENABLE);

DEFINE_CLK_RST_REG_FOUR_BIT_ENUM(CCLK_BURST_POLICY_CWAKEUP_IDLE_SOURCE,  0, CLKM, RSVD1, CLKS, RSVD3, PLLP_OUT0, PLLP_OUT4, RSVD6, RSVD7, PLLX_OUT0_LJ, DVFS_CPU_CLK, RSVD10, RSVD11, RSVD12, RSVD13, PLLX_OUT0, DVFS_CPU_CLK_LJ);
DEFINE_CLK_RST_REG_FOUR_BIT_ENUM(CCLK_BURST_POLICY_CWAKEUP_RUN_SOURCE,   4, CLKM, RSVD1, CLKS, RSVD3, PLLP_OUT0, PLLP_OUT4, RSVD6, RSVD7, PLLX_OUT0_LJ, DVFS_CPU_CLK, RSVD10, RSVD11, RSVD12, RSVD13, PLLX_OUT0, DVFS_CPU_CLK_LJ);
DEFINE_CLK_RST_REG_FOUR_BIT_ENUM(CCLK_BURST_POLICY_CWAKEUP_IRQ_SOURCE,   8, CLKM, RSVD1, CLKS, RSVD3, PLLP_OUT0, PLLP_OUT4, RSVD6, RSVD7, PLLX_OUT0_LJ, DVFS_CPU_CLK, RSVD10, RSVD11, RSVD12, RSVD13, PLLX_OUT0, DVFS_CPU_CLK_LJ);
DEFINE_CLK_RST_REG_FOUR_BIT_ENUM(CCLK_BURST_POLICY_CWAKEUP_FIQ_SOURCE,  12, CLKM, RSVD1, CLKS, RSVD3, PLLP_OUT0, PLLP_OUT4, RSVD6, RSVD7, PLLX_OUT0_LJ, DVFS_CPU_CLK, RSVD10, RSVD11, RSVD12, RSVD13, PLLX_OUT0, DVFS_CPU_CLK_LJ);
DEFINE_CLK_RST_REG_FOUR_BIT_ENUM(CCLK_BURST_POLICY_CPU_STATE,           28, STDBY, IDLE, RUN, RSVD3, IRQ, RSVD5, RSVD6, RSVD7, FIQ, RSVD9, RSVD10, RSVD11, RSVD12, RSVD13, RSVD14, RSVD15);

DEFINE_CLK_RST_REG(CPU_SOFTRST_CTRL2_CAR2PMC_CPU_ACK_WIDTH, 0, 12);

DEFINE_CLK_RST_REG_TWO_BIT_ENUM(SPARE_REG0_CLK_M_DIVISOR, 2, CLK_M_DIVISOR1, CLK_M_DIVISOR2, CLK_M_DIVISOR3, CLK_M_DIVISOR4);

/* RST_DEVICES */
#define CLK_RST_CONTROLLER_RST_DEVICES_L (0x004)
#define CLK_RST_CONTROLLER_RST_DEVICES_H (0x008)
#define CLK_RST_CONTROLLER_RST_DEVICES_U (0x00C)
#define CLK_RST_CONTROLLER_RST_DEVICES_X (0x28C)
#define CLK_RST_CONTROLLER_RST_DEVICES_Y (0x2A4)
#define CLK_RST_CONTROLLER_RST_DEVICES_V (0x358)
#define CLK_RST_CONTROLLER_RST_DEVICES_W (0x35C)

/* CLK_OUT_ENB */
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_L (0x010)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_H (0x014)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_U (0x018)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_X (0x280)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_Y (0x298)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_V (0x360)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_W (0x364)

/* CLK_SOURCE */
#define CLK_RST_CONTROLLER_CLK_SOURCE_I2C1     (0x124)
#define CLK_RST_CONTROLLER_CLK_SOURCE_I2C5     (0x128)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTA    (0x178)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTB    (0x17C)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTC    (0x1A0)
#define CLK_RST_CONTROLLER_CLK_SOURCE_MSELECT  (0x3B4)
#define CLK_RST_CONTROLLER_CLK_SOURCE_ACTMON   (0x3E8)
#define CLK_RST_CONTROLLER_CLK_SOURCE_DVFS_REF (0x62C)
#define CLK_RST_CONTROLLER_CLK_SOURCE_DVFS_SOC (0x630)

/* RST_DEV_*_SET */
#define CLK_RST_CONTROLLER_RST_DEV_L_SET (0x300)
#define CLK_RST_CONTROLLER_RST_DEV_H_SET (0x308)
#define CLK_RST_CONTROLLER_RST_DEV_U_SET (0x310)
#define CLK_RST_CONTROLLER_RST_DEV_V_SET (0x430)

/* RST_DEV_*_CLR */
#define CLK_RST_CONTROLLER_RST_DEV_L_CLR (0x304)
#define CLK_RST_CONTROLLER_RST_DEV_H_CLR (0x30C)
#define CLK_RST_CONTROLLER_RST_DEV_U_CLR (0x314)
#define CLK_RST_CONTROLLER_RST_DEV_V_CLR (0x434)

/* CLK_ENB_*_SET */
#define CLK_RST_CONTROLLER_CLK_ENB_L_SET (0x320)
#define CLK_RST_CONTROLLER_CLK_ENB_H_SET (0x328)
#define CLK_RST_CONTROLLER_CLK_ENB_U_SET (0x330)
#define CLK_RST_CONTROLLER_CLK_ENB_V_SET (0x440)
#define CLK_RST_CONTROLLER_CLK_ENB_W_SET (0x448)
#define CLK_RST_CONTROLLER_CLK_ENB_X_SET (0x284)
#define CLK_RST_CONTROLLER_CLK_ENB_Y_SET (0x29C)

/* CLK_ENB_*_CLR */
#define CLK_RST_CONTROLLER_CLK_ENB_L_CLR (0x324)
#define CLK_RST_CONTROLLER_CLK_ENB_H_CLR (0x32C)
#define CLK_RST_CONTROLLER_CLK_ENB_U_CLR (0x334)
#define CLK_RST_CONTROLLER_CLK_ENB_X_CLR (0x288)
#define CLK_RST_CONTROLLER_CLK_ENB_Y_CLR (0x2A0)
#define CLK_RST_CONTROLLER_CLK_ENB_V_CLR (0x444)
#define CLK_RST_CONTROLLER_CLK_ENB_W_CLR (0x44C)

/* CLK_ENB_*_INDEX */
#define CLK_RST_CONTROLLER_CLK_ENB_I2C1_INDEX   (0x0C)
#define CLK_RST_CONTROLLER_CLK_ENB_I2C5_INDEX   (0x0F)
#define CLK_RST_CONTROLLER_CLK_ENB_UARTA_INDEX  (0x06)
#define CLK_RST_CONTROLLER_CLK_ENB_UARTB_INDEX  (0x07)
#define CLK_RST_CONTROLLER_CLK_ENB_UARTC_INDEX  (0x17)
#define CLK_RST_CONTROLLER_CLK_ENB_ACTMON_INDEX (0x17)

/* RST_CPUG_CMPLX_* */
#define CLK_RST_CONTROLLER_RST_CPUG_CMPLX_SET   (0x450)
#define CLK_RST_CONTROLLER_RST_CPUG_CMPLX_CLR   (0x454)

DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_ARC_CLK_OVR_ON,             19, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_TSEC_CLK_OVR_ON,            20, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_TSECB_CLK_OVR_ON,           21, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_ISPB_CLK_OVR_ON,            22, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_TZRAM_CLK_OVR_ON,           23, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_QSPI_CLK_OVR_ON,            24, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_A9AVP_CLK_OVR_ON,           26, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_MPCORE_MSELECT_CLK_OVR_ON,  27, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_SDMMC1_LEGACY_TMCLK_OVR_ON, 28, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_SDMMC2_LEGACY_TMCLK_OVR_ON, 29, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_SDMMC3_LEGACY_TMCLK_OVR_ON, 30, OFF, ON);
DEFINE_CLK_RST_REG_BIT_ENUM(LVL2_CLK_GATE_OVRD_SDMMC4_LEGACY_TMCLK_OVR_ON, 31, OFF, ON);

DEFINE_CLK_RST_REG_THREE_BIT_ENUM(CLK_SOURCE_I2C1_I2C1_CLK_SRC, 29, PLLP_OUT0, PLLC2_OUT0, PLLC_OUT0, PLLC4_OUT0, RESERVED4, PLLC4_OUT1, CLK_M, PLLC4_OUT2);
DEFINE_CLK_RST_REG(CLK_SOURCE_I2C1_I2C1_CLK_DIVISOR, 0, 8);
DEFINE_CLK_RST_REG_THREE_BIT_ENUM(CLK_SOURCE_I2C5_I2C5_CLK_SRC, 29, PLLP_OUT0, PLLC2_OUT0, PLLC_OUT0, PLLC4_OUT0, RESERVED4, PLLC4_OUT1, CLK_M, PLLC4_OUT2);
DEFINE_CLK_RST_REG(CLK_SOURCE_I2C5_I2C5_CLK_DIVISOR, 0, 8);

DEFINE_CLK_RST_REG_THREE_BIT_ENUM(CLK_SOURCE_UARTA_UARTA_CLK_SRC, 29, PLLP_OUT0, PLLC2_OUT0, PLLC_OUT0, PLLC4_OUT0, RESERVED4, PLLC4_OUT1, CLK_M, PLLC4_OUT2);
DEFINE_CLK_RST_REG_THREE_BIT_ENUM(CLK_SOURCE_UARTB_UARTB_CLK_SRC, 29, PLLP_OUT0, PLLC2_OUT0, PLLC_OUT0, PLLC4_OUT0, RESERVED4, PLLC4_OUT1, CLK_M, PLLC4_OUT2);
DEFINE_CLK_RST_REG_THREE_BIT_ENUM(CLK_SOURCE_UARTC_UARTC_CLK_SRC, 29, PLLP_OUT0, PLLC2_OUT0, PLLC_OUT0, PLLC4_OUT0, RESERVED4, PLLC4_OUT1, CLK_M, PLLC4_OUT2);

DEFINE_CLK_RST_REG_THREE_BIT_ENUM(CLK_SOURCE_MSELECT_MSELECT_CLK_SRC, 29, PLLP_OUT0, PLLC2_OUT0, PLLC_OUT0, PLLC4_OUT2, PLLC4_OUT1, CLK_S, CLK_M, PLLC4_OUT0);
DEFINE_CLK_RST_REG(CLK_SOURCE_MSELECT_MSELECT_CLK_DIVISOR, 0, 8);

DEFINE_CLK_RST_REG_THREE_BIT_ENUM(CLK_SOURCE_ACTMON_ACTMON_CLK_SRC, 29, PLLP_OUT0, PLLC2_OUT0, PLLC_OUT0, PLLC4_OUT0, CLK_S, PLLC4_OUT1, CLK_M, PLLC4_OUT2);

DEFINE_CLK_RST_REG(CLK_SOURCE_DVFS_REF_DVFS_REF_DIVISOR, 0, 8);
DEFINE_CLK_RST_REG_THREE_BIT_ENUM(CLK_SOURCE_DVFS_REF_DVFS_REF_CLK_SRC, 29, PLLP_OUT0, PLLC2_OUT0, PLLC_OUT0, PLLC4_OUT0, RESERVED4, PLLC4_OUT1, CLK_M, PLLC4_OUT2);

DEFINE_CLK_RST_REG(CLK_SOURCE_DVFS_SOC_DVFS_SOC_DIVISOR, 0, 8);
DEFINE_CLK_RST_REG_THREE_BIT_ENUM(CLK_SOURCE_DVFS_SOC_DVFS_SOC_CLK_SRC, 29, PLLP_OUT0, PLLC2_OUT0, PLLC_OUT0, PLLC4_OUT0, RESERVED4, PLLC4_OUT1, CLK_M, PLLC4_OUT2);

DEFINE_CLK_RST_REG_BIT_ENUM(RST_DEV_L_SET_SET_COP_RST, 1, DISABLE, ENABLE);

DEFINE_CLK_RST_REG_BIT_ENUM(RST_DEV_L_CLR_CLR_COP_RST, 1, DISABLE, ENABLE);

DEFINE_CLK_RST_REG_BIT_ENUM(RST_CPUG_CMPLX_CLR_CLR_CPURESET0,    0, DISABLE, ENABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(RST_CPUG_CMPLX_CLR_CLR_CPURESET1,    1, DISABLE, ENABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(RST_CPUG_CMPLX_CLR_CLR_CPURESET2,    2, DISABLE, ENABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(RST_CPUG_CMPLX_CLR_CLR_CPURESET3,    3, DISABLE, ENABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(RST_CPUG_CMPLX_CLR_CLR_CORERESET0,  16, DISABLE, ENABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(RST_CPUG_CMPLX_CLR_CLR_CORERESET1,  17, DISABLE, ENABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(RST_CPUG_CMPLX_CLR_CLR_CORERESET2,  18, DISABLE, ENABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(RST_CPUG_CMPLX_CLR_CLR_CORERESET3,  19, DISABLE, ENABLE);
DEFINE_CLK_RST_REG_BIT_ENUM(RST_CPUG_CMPLX_CLR_CLR_NONCPURESET, 29, DISABLE, ENABLE);

/* TODO: Actually include all devices. */
#define CLK_RST_FOREACH_DEVICE(HANDLER) \
    HANDLER(L, CPU,           0,  0)    \
    HANDLER(L, RTC,           0,  4)    \
    HANDLER(L, TMR,           0,  5)    \
    HANDLER(L, GPIO,          0,  8)    \
    HANDLER(L, USBD,          0, 22)    \
    HANDLER(L, CACHE2,        0, 31)    \
    HANDLER(H, MEM,           1,  0)    \
    HANDLER(H, AHBDMA,        1,  1)    \
    HANDLER(H, APBDMA,        1,  2)    \
    HANDLER(H, USB2,          1, 26)    \
    HANDLER(H, PMC,           1,  6)    \
    HANDLER(H, FUSE,          1,  7)    \
    HANDLER(H, KFUSE,         1,  8)    \
    HANDLER(H, I2C5,          1, 15)    \
    HANDLER(H, EMC,           1, 25)    \
    HANDLER(U, CSITE,         2,  9)    \
    HANDLER(U, IRAMA,         2, 20)    \
    HANDLER(U, IRAMB,         2, 21)    \
    HANDLER(U, IRAMC,         2, 22)    \
    HANDLER(U, IRAMD,         2, 23)    \
    HANDLER(U, CRAM2,         2, 24)    \
    HANDLER(V, CPUG,          3,  0)    \
    HANDLER(V, MSELECT,       3,  3)    \
    HANDLER(V, SPDIF_DOUBLER, 3, 22)    \
    HANDLER(V, ACTMON,        3, 23)    \
    HANDLER(V, TZRAM,         3, 30)    \
    HANDLER(V, SE,            3, 31)    \
    HANDLER(W, PCIERX0,       4,  2)    \
    HANDLER(W, PCIERX1,       4,  3)    \
    HANDLER(W, PCIERX2,       4,  4)    \
    HANDLER(W, PCIERX3,       4,  5)    \
    HANDLER(W, PCIERX4,       4,  6)    \
    HANDLER(W, PCIERX5,       4,  7)    \
    HANDLER(W, ENTROPY,       4, 21)    \
    HANDLER(W, DVFS,          4, 27)    \
    HANDLER(W, MC1,           4, 30)    \
    HANDLER(X, MC_CAPA,       5,  7)    \
    HANDLER(X, MC_CBPA,       5,  8)    \
    HANDLER(X, MC_CPU,        5,  9)    \
    HANDLER(X, MC_BBC,        5, 10)    \
    HANDLER(X, EMC_DLL,       5, 14)    \
    HANDLER(X, GPU,           5, 24)    \
    HANDLER(X, DBGAPB,        5, 25)    \
    HANDLER(X, PLLG_REF,      5, 29)    \
    HANDLER(Y, MC_CCPA,       6,  8)    \
    HANDLER(Y, MC_CDPA,       6,  9)    \
    HANDLER(Y, PLLP_OUT_CPU,  6, 31)

#define CLK_RST_DEFINE_SET_CLR_REG(REGISTER, DEVICE, REGISTER_INDEX, DEVICE_INDEX)                             \
    DEFINE_CLK_RST_REG_BIT_ENUM(CLK_ENB_##REGISTER##_SET_SET_CLK_ENB_##DEVICE, DEVICE_INDEX, DISABLE, ENABLE); \
    DEFINE_CLK_RST_REG_BIT_ENUM(CLK_ENB_##REGISTER##_CLR_CLR_CLK_ENB_##DEVICE, DEVICE_INDEX, DISABLE, ENABLE); \
    DEFINE_CLK_RST_REG_BIT_ENUM(CLK_OUT_ENB_##REGISTER##_CLK_ENB_##DEVICE,     DEVICE_INDEX, DISABLE, ENABLE); \
    DEFINE_CLK_RST_REG_BIT_ENUM(CLK_ENB_##REGISTER##_CLK_ENB_##DEVICE,         DEVICE_INDEX, DISABLE, ENABLE); \
    DEFINE_CLK_RST_REG_BIT_ENUM(RST_DEV_##REGISTER##_SET_SET_##DEVICE##_RST,   DEVICE_INDEX, DISABLE, ENABLE); \
    DEFINE_CLK_RST_REG_BIT_ENUM(RST_DEV_##REGISTER##_CLR_CLR_##DEVICE##_RST,   DEVICE_INDEX, DISABLE, ENABLE); \
    DEFINE_CLK_RST_REG_BIT_ENUM(RST_DEV_##REGISTER##_##DEVICE##_RST,           DEVICE_INDEX, DISABLE, ENABLE); \
    DEFINE_CLK_RST_REG_BIT_ENUM(RST_DEVICES_##REGISTER##_SWR_##DEVICE##_RST,   DEVICE_INDEX, DISABLE, ENABLE);

CLK_RST_FOREACH_DEVICE(CLK_RST_DEFINE_SET_CLR_REG)

#undef CLK_RST_DEFINE_SET_CLR_REG

