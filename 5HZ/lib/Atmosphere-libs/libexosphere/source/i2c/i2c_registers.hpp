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
#include <exosphere.hpp>

namespace ams::i2c {

    #define I2C_I2C_CNFG                  (0x000)
    #define I2C_I2C_CMD_ADDR0             (0x004)
    #define I2C_I2C_CMD_DATA1             (0x00C)
    #define I2C_I2C_STATUS                (0x01C)
    #define I2C_INTERRUPT_STATUS_REGISTER (0x068)
    #define I2C_CLK_DIVISOR_REGISTER      (0x06C)
    #define I2C_BUS_CLEAR_CONFIG          (0x084)
    #define I2C_BUS_CLEAR_STATUS          (0x088)
    #define I2C_CONFIG_LOAD               (0x08C)

    #define I2C_REG_BITS_MASK(NAME)                                      REG_NAMED_BITS_MASK    (I2C, NAME)
    #define I2C_REG_BITS_VALUE(NAME, VALUE)                              REG_NAMED_BITS_VALUE   (I2C, NAME, VALUE)
    #define I2C_REG_BITS_ENUM(NAME, ENUM)                                REG_NAMED_BITS_ENUM    (I2C, NAME, ENUM)
    #define I2C_REG_BITS_ENUM_SEL(NAME, __COND__, TRUE_ENUM, FALSE_ENUM) REG_NAMED_BITS_ENUM_SEL(I2C, NAME, __COND__, TRUE_ENUM, FALSE_ENUM)

    #define DEFINE_I2C_REG(NAME, __OFFSET__, __WIDTH__)                                                                                                                  REG_DEFINE_NAMED_REG           (I2C, NAME, __OFFSET__, __WIDTH__)
    #define DEFINE_I2C_REG_BIT_ENUM(NAME, __OFFSET__, ZERO, ONE)                                                                                                         REG_DEFINE_NAMED_BIT_ENUM      (I2C, NAME, __OFFSET__, ZERO, ONE)
    #define DEFINE_I2C_REG_TWO_BIT_ENUM(NAME, __OFFSET__, ZERO, ONE, TWO, THREE)                                                                                         REG_DEFINE_NAMED_TWO_BIT_ENUM  (I2C, NAME, __OFFSET__, ZERO, ONE, TWO, THREE)
    #define DEFINE_I2C_REG_THREE_BIT_ENUM(NAME, __OFFSET__, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN)                                                               REG_DEFINE_NAMED_THREE_BIT_ENUM(I2C, NAME, __OFFSET__, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN)
    #define DEFINE_I2C_REG_FOUR_BIT_ENUM(NAME, __OFFSET__, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, ELEVEN, TWELVE, THIRTEEN, FOURTEEN, FIFTEEN) REG_DEFINE_NAMED_FOUR_BIT_ENUM (I2C, NAME, __OFFSET__, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, ELEVEN, TWELVE, THIRTEEN, FOURTEEN, FIFTEEN)

    /* I2C_CNFG */
    DEFINE_I2C_REG(I2C_CNFG_LENGTH, 1, 3);
    DEFINE_I2C_REG_BIT_ENUM(I2C_CNFG_CMD1, 6, WRITE, READ);
    DEFINE_I2C_REG_BIT_ENUM(I2C_CNFG_SEND, 9, NOP, GO);
    DEFINE_I2C_REG_BIT_ENUM(I2C_CNFG_NEW_MASTER_FSM, 11, DISABLE, ENABLE);
    DEFINE_I2C_REG_THREE_BIT_ENUM(I2C_CNFG_DEBOUNCE_CNT, 12, NO_DEBOUNCE, DEBOUNCE_2T, DEBOUNCE_4T, DEBOUNCE_6T, DEBOUNCE_8T, DEBOUNCE_10T, DEBOUNCE_12T, DEBOUNCE_14T);

    /* I2C_CMD_ADDR0 */
    DEFINE_I2C_REG_BIT_ENUM(I2C_CMD_ADDR0_7BIT_RW, 0, WRITE, READ);
    DEFINE_I2C_REG(I2C_CMD_ADDR0_7BIT_ADDR, 1, 7);

    /* I2C_STATUS */
    DEFINE_I2C_REG_FOUR_BIT_ENUM(I2C_STATUS_CMD1_STAT, 0, SL1_XFER_SUCCESSFUL, SL1_NOACK_FOR_BYTE1, SL1_NOACK_FOR_BYTE2, SL1_NOACK_FOR_BYTE3, SL1_NOACK_FOR_BYTE4, SL1_NOACK_FOR_BYTE5, SL1_NOACK_FOR_BYTE6, SL1_NOACK_FOR_BYTE7, SL1_NOACK_FOR_BYTE8, SL1_NOACK_FOR_BYTE9, SL1_NOACK_FOR_BYTE10, RESERVED11, RESERVED12, RESERVED13, RESERVED14, RESERVED15);
    DEFINE_I2C_REG_FOUR_BIT_ENUM(I2C_STATUS_CMD2_STAT, 4, SL2_XFER_SUCCESSFUL, SL2_NOACK_FOR_BYTE1, SL2_NOACK_FOR_BYTE2, SL2_NOACK_FOR_BYTE3, SL2_NOACK_FOR_BYTE4, SL2_NOACK_FOR_BYTE5, SL2_NOACK_FOR_BYTE6, SL2_NOACK_FOR_BYTE7, SL2_NOACK_FOR_BYTE8, SL2_NOACK_FOR_BYTE9, SL2_NOACK_FOR_BYTE10, RESERVED11, RESERVED12, RESERVED13, RESERVED14, RESERVED15);
    DEFINE_I2C_REG_BIT_ENUM(I2C_STATUS_BUSY, 8, NOT_BUSY, BUSY);

    /* INTERRUPT_STATUS_REGISTER */
    DEFINE_I2C_REG_BIT_ENUM(INTERRUPT_STATUS_REGISTER_BUS_CLEAR_DONE, 11, UNSET, SET);

    /* CLK_DIVISOR_REGISTER */
    DEFINE_I2C_REG(CLK_DIVISOR_REGISTER_HSMODE, 0, 16);
    DEFINE_I2C_REG(CLK_DIVISOR_REGISTER_STD_FAST_MODE, 16, 16);

    /* BUS_CLEAR_CONFIG */
    DEFINE_I2C_REG_BIT_ENUM(BUS_CLEAR_CONFIG_BC_ENABLE,    0, DISABLE, ENABLE);
    DEFINE_I2C_REG_BIT_ENUM(BUS_CLEAR_CONFIG_BC_TERMINATE, 1, THRESHOLD, IMMEDIATE);
    DEFINE_I2C_REG_BIT_ENUM(BUS_CLEAR_CONFIG_BC_STOP_COND, 2, NO_STOP, STOP);
    DEFINE_I2C_REG(BUS_CLEAR_CONFIG_BC_SCLK_THRESHOLD, 16, 8);

    /* CONFIG_LOAD */
    DEFINE_I2C_REG_BIT_ENUM(CONFIG_LOAD_MSTR_CONFIG_LOAD,    0, DISABLE, ENABLE);
    DEFINE_I2C_REG_BIT_ENUM(CONFIG_LOAD_SLV_CONFIG_LOAD,     1, DISABLE, ENABLE);
    DEFINE_I2C_REG_BIT_ENUM(CONFIG_LOAD_TIMEOUT_CONFIG_LOAD, 2, DISABLE, ENABLE);
    DEFINE_I2C_REG(CONFIG_LOAD_RESERVED_BIT_5, 5, 1);


}
