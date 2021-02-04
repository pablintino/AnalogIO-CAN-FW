/**
 * MIT License
 *
 * Copyright (c) 2020 Pablo Rodriguez Nava, @pablintino
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **/


#include "bsp_io.h"
#include "bsp_i2c.h"
#include "bsp_tick.h"
#include "bsp_clocks.h"
#include "bsp_common_utils.h"


/**
 * This matrix allows I2C peripheral to be configured based on pre-calculated speeds for various well known frequencies.
 * The first index indicates the peripheral speed whereas the second one represents the speed mode (standard, full or fast modes)
 */
const uint32_t __TIMING_REGISTER_CONFIG_VALUES[7][3] = {{0x2000090EU, 0x0000020BU, 0x00000001U}, /** 8 MHz */
                                                        {0x00303D5BU, 0x0010061AU, 0x00000107U}, /** 16 MHz */
                                                        {0x00506682U, 0x00200C28U, 0x0010030DU}, /** 24 MHz */
                                                        {0x20303E5DU, 0x2010091AU, 0x20000209U}, /** 48 MHz */
                                                        {0x10B0DCFBU, 0x009032AEU, 0x0040163AU}, /** 96 MHz */
                                                        {0x20A0C4DFU, 0x2040184BU, 0x00601C51U}, /** 128 MHz */
                                                        {0x30A0A7FBU, 0x10802D9BU, 0x00802172U}, /** 170 MHz */

};


static ret_status __get_i2c_input_frequency(BSP_I2C_Instance *i2c, uint32_t *freq);

static ret_status __get_i2c_clk_mux_position(BSP_I2C_Instance *i2c, uint8_t *position, uint32_t **ccipr);

static ret_status __configure_i2c_timmings(BSP_I2C_Instance *i2c, bsp_i2c_master_config_t *config);

static uint8_t
__start_i2c_transfer(BSP_I2C_Instance *i2c, uint16_t address, uint16_t transfer_size, bool write_transfer,
                     bool is_reload_transfer);

static ret_status
__wait_for_isr_flag_or_nack(BSP_I2C_Instance *i2c, uint32_t flag, uint32_t timeout, uint32_t start_tick);

static ret_status
__wait_for_empty_rx(BSP_I2C_Instance *i2c, uint32_t timeout, uint32_t start_tick, uint32_t pending_bytes);

static ret_status __is_nack_condition_present(BSP_I2C_Instance *i2c, uint32_t timeout, uint32_t start_tick);

static void __clean_txd_txis_flag(BSP_I2C_Instance *hi2c);


ret_status BSP_I2C_master_conf(BSP_I2C_Instance *i2c, bsp_i2c_master_config_t *config) {

    /* Most configurations cannot be done if the I2C peripheral is enabled */
    if (__BSP_IS_FLAG_SET(i2c->CR1, I2C_CR1_PE)) {
        return STATUS_ERR;
    }

    /** Configure I2C timming based on actual frequency or custom timing field */
    if (__configure_i2c_timmings(i2c, config) != STATUS_OK) {
        return STATUS_ERR;
    }

    /** Before changed our own address disable OA1 register enable as said in datasheet*/
    __BSP_CLEAR_MASKED_REG(i2c->OAR1, I2C_OAR1_OA1EN);
    if (config->AddressingMode == BSP_I2C_ADDRESSING_MODE_7) {
        i2c->OAR1 = (I2C_OAR1_OA1EN | (config->SelfAddress & I2C_OAR1_OA1));
    } else {
        i2c->OAR1 = (I2C_OAR1_OA1MODE | I2C_OAR1_OA1EN | config->SelfAddress);
    }

    __BSP_SET_MASKED_REG_VALUE(i2c->CR2, I2C_CR2_ADD10, (config->AddressingMode & I2C_CR2_ADD10));

    /** Master should do clock stretching always. General call disabled when acting as master too */
    __BSP_CLEAR_MASKED_REG(i2c->CR1, I2C_CR1_NOSTRETCH | I2C_CR1_GCEN);
    __BSP_SET_MASKED_REG(i2c->CR2, I2C_CR2_AUTOEND | I2C_CR2_NACK);

    __BSP_SET_MASKED_REG_VALUE(i2c->CR1, I2C_CR1_DNF | I2C_CR1_ANFOFF, config->DigitalFilter & I2C_CR1_DNF);

    /** Enable analog analog SDA/SCL filter if needed */
    if (config->AnalogFilterEnabled) {
        __BSP_CLEAR_MASKED_REG(i2c->CR1, I2C_CR1_ANFOFF);
    } else {
        __BSP_SET_MASKED_REG(i2c->CR1, I2C_CR1_ANFOFF);
    }

    __BSP_CLEAR_MASKED_REG(i2c->OAR2, I2C_OAR2_OA2EN);

    return STATUS_OK;
}


void BSP_I2C_enable(BSP_I2C_Instance *i2c) {
    __BSP_SET_MASKED_REG(i2c->CR1, I2C_CR1_PE);
}


void BSP_I2C_disable(BSP_I2C_Instance *i2c) {
    __BSP_CLEAR_MASKED_REG(i2c->CR1, I2C_CR1_PE);
}


ret_status
BSP_I2C_master_transfer(BSP_I2C_Instance *i2c, uint16_t address, uint8_t *pData, uint16_t size, bool is_write,
                        uint32_t timeout) {
    ret_status status;
    uint32_t tickstart = BSP_TICK_get_ticks();
    uint8_t *pBuffPtr = pData;

    status = BSP_UTIL_wait_flag_status(&i2c->ISR, I2C_ISR_BUSY, 0x00U, tickstart, 25U);
    if (status != STATUS_OK) {
        return status;
    }

    /* Start the transfer (generate START condition and send slave address) */
    uint16_t transfer_block_size = __start_i2c_transfer(i2c, address, size, is_write, false);

    for (int16_t transfer_count = size; transfer_count > 0; transfer_count--) {
        if (is_write) {
            /* Wait until the current TXDR data has been sent and we can write the next byte */
            status = __wait_for_isr_flag_or_nack(i2c, I2C_ISR_TXIS, timeout, tickstart);
            if (status == STATUS_OK) {
                i2c->TXDR = *pBuffPtr;
            } else {
                return status;
            }
        } else {
            status = __wait_for_empty_rx(i2c, timeout, tickstart, transfer_block_size);
            if (status == STATUS_OK) {
                *pBuffPtr = (uint8_t) (i2c->RXDR & 0xFF);
            } else {
                return status;
            }
        }

        /* Move the data pointer to the next position for the next element **/
        pBuffPtr++;

        /* Subtract the pending bytes of the current reload transfer */
        transfer_block_size--;

        if ((transfer_count != 0U) && (transfer_block_size == 0U)) {
            /* Wait until TCR flag is set */
            status = BSP_UTIL_wait_flag_status(&i2c->ISR, I2C_ISR_TCR, 0x00U, tickstart, 25U);
            if (status != STATUS_OK) {
                return status;
            }

            /* Signal another transfer (without start condition) by flagging the reload bit */
            transfer_block_size = __start_i2c_transfer(i2c, address, transfer_count, is_write, true);
        }
    }

    /* Wait until the end of the transfer by waiting for the STOPF that the HW should rise
     * If the transfer fails a NACKF will arrive and the wait should return an error.
     */
    status = __wait_for_isr_flag_or_nack(i2c, I2C_ISR_STOPF, timeout, tickstart);
    if (status != STATUS_OK) {
        return status;
    }

    /* Clear STOP Flag */
    __BSP_SET_MASKED_REG(i2c->ICR, I2C_ICR_STOPCF);

    /* Clear Configuration Register 2 */
    __BSP_CLEAR_MASKED_REG(i2c->CR2, I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN);

    return status;
}


static ret_status __configure_i2c_timmings(BSP_I2C_Instance *i2c, bsp_i2c_master_config_t *config) {
    uint32_t freq = 0;

    if (config->FixedSpeed == BSP_I2C_SPEED_NONE) {
        i2c->TIMINGR = config->CustomTimming;
    } else if ((config->FixedSpeed != BSP_I2C_SPEED_NONE) && __get_i2c_input_frequency(i2c, &freq) != STATUS_ERR) {
        switch (freq) {
            case 8000000U:
                i2c->TIMINGR = __TIMING_REGISTER_CONFIG_VALUES[0][(config->FixedSpeed - 1) & 0x03U];
                break;
            case 16000000U:
                i2c->TIMINGR = __TIMING_REGISTER_CONFIG_VALUES[1][(config->FixedSpeed - 1) & 0x03U];
                break;
            case 24000000U:
                i2c->TIMINGR = __TIMING_REGISTER_CONFIG_VALUES[2][(config->FixedSpeed - 1) & 0x03U];
                break;
            case 48000000U:
                i2c->TIMINGR = __TIMING_REGISTER_CONFIG_VALUES[3][(config->FixedSpeed - 1) & 0x03U];
                break;
            case 96000000U:
                i2c->TIMINGR = __TIMING_REGISTER_CONFIG_VALUES[4][(config->FixedSpeed - 1) & 0x03U];
                break;
            case 128000000U:
                i2c->TIMINGR = __TIMING_REGISTER_CONFIG_VALUES[5][(config->FixedSpeed - 1) & 0x03U];
                break;
            case 170000000U:
                i2c->TIMINGR = __TIMING_REGISTER_CONFIG_VALUES[6][(config->FixedSpeed - 1) & 0x03U];
                break;
            default:
                return STATUS_ERR;
        }

    } else {
        return STATUS_ERR;
    }
    return STATUS_OK;
}


static ret_status __get_i2c_clk_mux_position(BSP_I2C_Instance *i2c, uint8_t *position, uint32_t **ccipr) {
    *ccipr = RCC->CCIPR;
    if (i2c == I2C1) {
        *position = RCC_CCIPR_I2C1SEL_Pos;
    } else if (i2c == I2C2) {
        *position = RCC_CCIPR_I2C2SEL_Pos;
    } else if (i2c == I2C3) {
        *position = RCC_CCIPR_I2C3SEL_Pos;
#if defined(I2C4)
        }else if (i2c == I2C4){
        position = RCC_CCIPR2_I2C4SEL_Pos;
        *ccipr = RCC->CCIPR2;
#endif
    } else {
        return STATUS_ERR;
    }
    return STATUS_OK;
}


static ret_status __get_i2c_input_frequency(BSP_I2C_Instance *i2c, uint32_t *freq) {
    uint8_t position = 0;
    uint32_t *ccipr;
    if (__get_i2c_clk_mux_position(i2c, &position, &ccipr) != STATUS_OK) {
        return STATUS_ERR;
    }

    if (((*ccipr) & (0x03U << position)) == (BSP_I2C_CLK_HSI << position)) {
        *freq = BSP_CLK_HSI_VALUE;
    } else if (((*ccipr) & (0x03U << position)) == (BSP_I2C_CLK_PCLK << position)) {
        *freq = BSP_CLK_get_pclk1_freq();
    } else if (((*ccipr) & (0x03U << position)) == (BSP_I2C_CLK_SYSCLK << position)) {
        *freq = BSP_CLK_get_sysclk_freq();
    } else {
        return STATUS_ERR;
    }
    return STATUS_OK;
}


static uint8_t
__start_i2c_transfer(BSP_I2C_Instance *i2c, uint16_t address, uint16_t transfer_size, bool write_transfer,
                     bool is_reload_transfer) {
    /** Peripheral allows only 255 transfers. Bigger transfers can be accomplished by manually reloading the whole peripheral
     * using the proper RELOAD bit in CR2.
     */
    uint8_t actual_transfer_size = transfer_size > 255 ? 255 : transfer_size;

    __BSP_SET_MASKED_REG_VALUE(i2c->CR2,
    /** If the transfer is a write one clear RD_WRN */
                               (write_transfer ? I2C_CR2_RD_WRN : 0x00) |
                               I2C_CR2_STOP | I2C_CR2_START | I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD |
                               I2C_CR2_AUTOEND,
    /** Write the slave address in which we want to read/write */
                               ((address << I2C_CR2_SADD_Pos) & I2C_CR2_SADD_Msk) |
                               /** Simply indicate the number of bytes to transfer (always less than 256) */
                               ((actual_transfer_size << I2C_CR2_NBYTES_Pos) & I2C_CR2_NBYTES_Msk) |
                               /** If we want to read ensure to set RD_WRN */
                               (write_transfer ? 0x00 : I2C_CR2_RD_WRN) |
                               /** Flag start to indicate HW that start condition should be placed on bus just now.
                                * Note that if this transfer is not the first one (can be part of a transfer of more than 255 bytes
                                * START should not be performed on bus as it can break the whole transfer*/
                               (is_reload_transfer ? 0x00 : I2C_CR2_START) |
                               /** If transfer size is bigger that 255 the peripheral shouldn't end the transfer with a
                                * STOP cause we'll reload with the remaining data
                                */
                               (transfer_size > 255 ? I2C_CR2_RELOAD : I2C_CR2_AUTOEND));


    return actual_transfer_size;
}


static ret_status
__wait_for_isr_flag_or_nack(BSP_I2C_Instance *i2c, uint32_t flag, uint32_t timeout, uint32_t start_tick) {
    ret_status status;
    while (!__BSP_IS_FLAG_SET(i2c->ISR, flag)) {
        /* Just check if the other party has sent us a NACK cause that means the transfer has been aborted */
        status = __is_nack_condition_present(i2c, timeout, start_tick);
        if (status != STATUS_OK) {
            /* NACK received when waiting for the STOP. The transfer has failed */
            return status;
        } else if (((BSP_TICK_get_ticks() - start_tick) > timeout) || (start_tick == 0U)) {
            /* Timeout waiting for the given flag to be set */
            return STATUS_TMT;
        }
    }
    return STATUS_OK;
}


static ret_status
__wait_for_empty_rx(BSP_I2C_Instance *i2c, uint32_t timeout, uint32_t start_tick, uint32_t pending_bytes) {
    ret_status status;
    while (!__BSP_IS_FLAG_SET(i2c->ISR, I2C_ISR_RXNE)) {
        /* Check if a NACK is detected */
        status = __is_nack_condition_present(i2c, timeout, start_tick);
        if (status != STATUS_OK) {
            return status;
        }

        /* Check if a STOPF is detected */
        if (__BSP_IS_FLAG_SET(i2c->ISR, I2C_ISR_STOPF)) {
            /* Check if an RXNE is pending */
            /* Store Last receive data if any */
            if (__BSP_IS_FLAG_SET(i2c->ISR, I2C_ISR_RXNE) && (pending_bytes > 0U)) {
                /* Return HAL_OK */
                /* The Reading of data from RXDR will be done in caller function
                 * */
                return STATUS_OK;
            } else {

                /* RXNE not set and STOPF has been flagged... Transfer aborted.
                 * Clean CR2 and clear STOPF for future transfers
                 * */
                __BSP_SET_MASKED_REG(i2c->ICR, I2C_ICR_STOPCF);
                __BSP_CLEAR_MASKED_REG(i2c->CR2, I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN);

                return STATUS_ERR;
            }
        }

        /* Check for the timeout */
        if (((BSP_TICK_get_ticks() - start_tick) > timeout) || (timeout == 0U)) {
            return STATUS_TMT;
        }
    }
    return STATUS_OK;
}


static ret_status __is_nack_condition_present(BSP_I2C_Instance *i2c, uint32_t timeout, uint32_t start_tick) {
    ret_status status;
    /* Check if a NACK condition has been detected */
    if (__BSP_IS_FLAG_SET(i2c->ISR, I2C_ISR_NACKF)) {
        /* If a NACK arrives STOPF should be flagged by HW. Just wait for it */
        status = BSP_UTIL_wait_flag_status(&i2c->ISR, I2C_ISR_STOPF, I2C_ISR_STOPF, start_tick, timeout);
        if (status != STATUS_OK) {
            return status;
        }

        /* Clear the detected NACKF and STOPF flags */
        __BSP_SET_MASKED_REG(i2c->ICR, I2C_ICR_NACKCF | I2C_ICR_STOPCF);

        /* Prepare the peripheral for future transfers by cleaning the TX buffer and the control register */
        __clean_txd_txis_flag(i2c);
        __BSP_CLEAR_MASKED_REG(i2c->CR2, I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN);

        return STATUS_ERR;
    }
    return STATUS_OK;
}


static void __clean_txd_txis_flag(BSP_I2C_Instance *hi2c) {
    /* If a pending TXIS flag is set */
    /* Write a dummy data in TXDR to clear it */
    if (__BSP_IS_FLAG_SET(hi2c->ISR, I2C_ISR_TXIS)) {
        hi2c->TXDR = 0x00U;
    }

    /* Flush TX register if not empty */
    if (!__BSP_IS_FLAG_SET(hi2c->ISR, I2C_ISR_TXE)) {
        __BSP_SET_MASKED_REG(hi2c->ISR, I2C_ISR_TXE);
    }
}
