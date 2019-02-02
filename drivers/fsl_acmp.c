/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_acmp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.acmp"
#endif


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the ACMP instance from the peripheral base address.
 *
 * @param base ACMP peripheral base address.
 * @return ACMP instance.
 */
static uint32_t ACMP_GetInstance(CMP_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Array of ACMP peripheral base address. */
static CMP_Type *const s_acmpBases[] = CMP_BASE_PTRS;
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Clock name of ACMP. */
static const clock_ip_name_t s_acmpClock[] = CMP_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Codes
 ******************************************************************************/
static uint32_t ACMP_GetInstance(CMP_Type *base)
{
    uint32_t instance = 0U;
    uint32_t acmpArrayCount = (sizeof(s_acmpBases) / sizeof(s_acmpBases[0]));

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < acmpArrayCount; instance++)
    {
        if (s_acmpBases[instance] == base)
        {
            break;
        }
    }

    return instance;
}

void ACMP_Init(CMP_Type *base, const acmp_config_t *config)
{
    assert(NULL != config);

    uint32_t tmp32;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Open clock gate. */
    CLOCK_EnableClock(s_acmpClock[ACMP_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Disable the module before configuring it. */
    ACMP_Enable(base, false);

    /* CMPx_C0
     * Set control bit. Avoid clearing status flags at the same time.
     */
    tmp32 = (base->C0 & (~(CMP_C0_PMODE_MASK | CMP_C0_INVT_MASK | CMP_C0_COS_MASK | CMP_C0_OPE_MASK |
                           CMP_C0_HYSTCTR_MASK | CMP_C0_CFx_MASK)));
#if defined(FSL_FEATURE_ACMP_HAS_C0_OFFSET_BIT) && (FSL_FEATURE_ACMP_HAS_C0_OFFSET_BIT == 1U)
    tmp32 &= ~CMP_C0_OFFSET_MASK;
#endif /* FSL_FEATURE_ACMP_HAS_C0_OFFSET_BIT */
    if (config->enableHighSpeed)
    {
        tmp32 |= CMP_C0_PMODE_MASK;
    }
    if (config->enableInvertOutput)
    {
        tmp32 |= CMP_C0_INVT_MASK;
    }
    if (config->useUnfilteredOutput)
    {
        tmp32 |= CMP_C0_COS_MASK;
    }
    if (config->enablePinOut)
    {
        tmp32 |= CMP_C0_OPE_MASK;
    }
    tmp32 |= CMP_C0_HYSTCTR(config->hysteresisMode);
#if defined(FSL_FEATURE_ACMP_HAS_C0_OFFSET_BIT) && (FSL_FEATURE_ACMP_HAS_C0_OFFSET_BIT == 1U)
    tmp32 |= CMP_C0_OFFSET(config->offsetMode);
#endif /* FSL_FEATURE_ACMP_HAS_C0_OFFSET_BIT */
    base->C0 = tmp32;
}

void ACMP_Deinit(CMP_Type *base)
{
    /* Disable the module. */
    ACMP_Enable(base, false);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable clock gate. */
    CLOCK_DisableClock(s_acmpClock[ACMP_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

void ACMP_GetDefaultConfig(acmp_config_t *config)
{
    assert(NULL != config);

    /* Fill default configuration */
    config->enableHighSpeed = false;
    config->enableInvertOutput = false;
    config->useUnfilteredOutput = false;
    config->enablePinOut = false;
#if defined(FSL_FEATURE_ACMP_HAS_C0_OFFSET_BIT) && (FSL_FEATURE_ACMP_HAS_C0_OFFSET_BIT == 1U)
    config->offsetMode = kACMP_OffsetLevel0;
#endif /* FSL_FEATURE_ACMP_HAS_C0_OFFSET_BIT */
    config->hysteresisMode = kACMP_HysteresisLevel0;
}

void ACMP_Enable(CMP_Type *base, bool enable)
{
    /* CMPx_C0
     * Set control bit. Avoid clearing status flags at the same time.
     */
    if (enable)
    {
        base->C0 = ((base->C0 | CMP_C0_EN_MASK) & ~CMP_C0_CFx_MASK);
    }
    else
    {
        base->C0 &= ~(CMP_C0_EN_MASK | CMP_C0_CFx_MASK);
    }
}

#if defined(FSL_FEATURE_ACMP_HAS_C0_LINKEN_BIT) && (FSL_FEATURE_ACMP_HAS_C0_LINKEN_BIT == 1U)
void ACMP_EnableLinkToDAC(CMP_Type *base, bool enable)
{
    /* CMPx_C0_LINKEN
     * Set control bit. Avoid clearing status flags at the same time.
     */
    if (enable)
    {
        base->C0 = ((base->C0 | CMP_C0_LINKEN_MASK) & ~CMP_C0_CFx_MASK);
    }
    else
    {
        base->C0 &= ~(CMP_C0_LINKEN_MASK | CMP_C0_CFx_MASK);
    }
}
#endif /* FSL_FEATURE_ACMP_HAS_C0_LINKEN_BIT */

void ACMP_SetChannelConfig(CMP_Type *base, const acmp_channel_config_t *config)
{
    assert(NULL != config);

    uint32_t tmp32 = (base->C1 & (~(CMP_C1_PSEL_MASK | CMP_C1_MSEL_MASK)));

/* CMPx_C1
 * Set the input of CMP's positive port.
 */
#if (defined(FSL_FEATURE_ACMP_HAS_C1_INPSEL_BIT) && (FSL_FEATURE_ACMP_HAS_C1_INPSEL_BIT == 1U))
    tmp32 &= ~CMP_C1_INPSEL_MASK;
    tmp32 |= CMP_C1_INPSEL(config->positivePortInput);
#endif /* FSL_FEATURE_ACMP_HAS_C1_INPSEL_BIT */

#if (defined(FSL_FEATURE_ACMP_HAS_C1_INNSEL_BIT) && (FSL_FEATURE_ACMP_HAS_C1_INNSEL_BIT == 1U))
    tmp32 &= ~CMP_C1_INNSEL_MASK;
    tmp32 |= CMP_C1_INNSEL(config->negativePortInput);
#endif /* FSL_FEATURE_ACMP_HAS_C1_INPSEL_BIT */

    tmp32 |= CMP_C1_PSEL(config->plusMuxInput) | CMP_C1_MSEL(config->minusMuxInput);

    base->C1 = tmp32;
}

void ACMP_EnableDMA(CMP_Type *base, bool enable)
{
    /* CMPx_C0
     * Set control bit. Avoid clearing status flags at the same time.
     */
    if (enable)
    {
        base->C0 = ((base->C0 | CMP_C0_DMAEN_MASK) & ~CMP_C0_CFx_MASK);
    }
    else
    {
        base->C0 &= ~(CMP_C0_DMAEN_MASK | CMP_C0_CFx_MASK);
    }
}

void ACMP_EnableWindowMode(CMP_Type *base, bool enable)
{
    /* CMPx_C0
     * Set control bit. Avoid clearing status flags at the same time.
     */
    if (enable)
    {
        base->C0 = ((base->C0 | CMP_C0_WE_MASK) & ~CMP_C0_CFx_MASK);
    }
    else
    {
        base->C0 &= ~(CMP_C0_WE_MASK | CMP_C0_CFx_MASK);
    }
}

void ACMP_SetFilterConfig(CMP_Type *base, const acmp_filter_config_t *config)
{
    assert(NULL != config);

    /* CMPx_C0
     * Set control bit. Avoid clearing status flags at the same time.
     */
    uint32_t tmp32 = (base->C0 & (~(CMP_C0_FILTER_CNT_MASK | CMP_C0_FPR_MASK | CMP_C0_SE_MASK | CMP_C0_CFx_MASK)));

    if (config->enableSample)
    {
        tmp32 |= CMP_C0_SE_MASK;
    }
    tmp32 |= (CMP_C0_FILTER_CNT(config->filterCount) | CMP_C0_FPR(config->filterPeriod));
    base->C0 = tmp32;
}

void ACMP_SetDACConfig(CMP_Type *base, const acmp_dac_config_t *config)
{
    uint32_t tmp32;

    /* CMPx_C1
     * NULL configuration means to disable the feature.
     */
    if (NULL == config)
    {
        base->C1 &= ~CMP_C1_DACEN_MASK;
        return;
    }

    tmp32 = (base->C1 & (~(CMP_C1_VRSEL_MASK | CMP_C1_VOSEL_MASK)));
    /* Set configuration and enable the feature. */
    tmp32 |= (CMP_C1_VRSEL(config->referenceVoltageSource) | CMP_C1_VOSEL(config->DACValue) | CMP_C1_DACEN_MASK);

#if defined(FSL_FEATURE_ACMP_HAS_C1_DACOE_BIT) && (FSL_FEATURE_ACMP_HAS_C1_DACOE_BIT == 1U)
    tmp32 &= ~CMP_C1_DACOE_MASK;
    if (config->enableOutput)
    {
        tmp32 |= CMP_C1_DACOE_MASK;
    }
#endif /* FSL_FEATURE_ACMP_HAS_C1_DACOE_BIT */

#if defined(FSL_FEATURE_ACMP_HAS_C1_DMODE_BIT) && (FSL_FEATURE_ACMP_HAS_C1_DMODE_BIT == 1U)
    switch (config->workMode)
    {
        case kACMP_DACWorkLowSpeedMode:
            tmp32 &= ~CMP_C1_DMODE_MASK;
            break;
        case kACMP_DACWorkHighSpeedMode:
            tmp32 |= CMP_C1_DMODE_MASK;
            break;
        default:
            break;
    }
#endif /* FSL_FEATURE_ACMP_HAS_C1_DMODE_BIT */

    base->C1 = tmp32;
}

void ACMP_SetRoundRobinConfig(CMP_Type *base, const acmp_round_robin_config_t *config)
{
    uint32_t tmp32;

    /* CMPx_C2
     * Set control bit. Avoid clearing status flags at the same time.
     * NULL configuration means to disable the feature.
     */
    if (NULL == config)
    {
        tmp32 = CMP_C2_CHnF_MASK;
#if defined(FSL_FEATURE_ACMP_HAS_C2_RRE_BIT) && (FSL_FEATURE_ACMP_HAS_C2_RRE_BIT == 1U)
        tmp32 |= CMP_C2_RRE_MASK;
#endif /* FSL_FEATURE_ACMP_HAS_C2_RRE_BIT */
        base->C2 &= ~(tmp32);
        return;
    }

    /* CMPx_C1
     * Set all channel's round robin checker enable mask.
     */
    tmp32 = (base->C1 & ~(CMP_C1_CHNn_MASK));
    tmp32 |= ((config->checkerChannelMask) << CMP_C1_CHN0_SHIFT);
    base->C1 = tmp32;

    /* CMPx_C2
     * Set configuration and enable the feature.
     */
    tmp32 = (base->C2 &
             (~(CMP_C2_FXMP_MASK | CMP_C2_FXMXCH_MASK | CMP_C2_NSAM_MASK | CMP_C2_INITMOD_MASK | CMP_C2_CHnF_MASK)));
    tmp32 |= (CMP_C2_FXMP(config->fixedPort) | CMP_C2_FXMXCH(config->fixedChannelNumber) |
              CMP_C2_NSAM(config->sampleClockCount) | CMP_C2_INITMOD(config->delayModulus));
#if defined(FSL_FEATURE_ACMP_HAS_C2_RRE_BIT) && (FSL_FEATURE_ACMP_HAS_C2_RRE_BIT == 1U)
    tmp32 |= CMP_C2_RRE_MASK;
#endif /* FSL_FEATURE_ACMP_HAS_C2_RRE_BIT */
    base->C2 = tmp32;
}

void ACMP_SetRoundRobinPreState(CMP_Type *base, uint32_t mask)
{
    /* CMPx_C2
     * Set control bit. Avoid clearing status flags at the same time.
     */
    uint32_t tmp32 = (base->C2 & (~CMP_C2_ACOn_MASK | CMP_C2_CHnF_MASK));

    tmp32 |= (mask << CMP_C2_ACOn_SHIFT);
    base->C2 = tmp32;
}

void ACMP_ClearRoundRobinStatusFlags(CMP_Type *base, uint32_t mask)
{
    /* CMPx_C2 */
    uint32_t tmp32 = (base->C2 & (~CMP_C2_CHnF_MASK));

    tmp32 |= (mask << CMP_C2_CH0F_SHIFT);
    base->C2 = tmp32;
}

void ACMP_EnableInterrupts(CMP_Type *base, uint32_t mask)
{
    uint32_t tmp32;

    /* CMPx_C0
     * Set control bit. Avoid clearing status flags at the same time.
     * Set CMP interrupt enable flag.
     */
    tmp32 = base->C0 & ~CMP_C0_CFx_MASK; /* To protect the W1C flags. */
    if (kACMP_OutputRisingInterruptEnable == (mask & kACMP_OutputRisingInterruptEnable))
    {
        tmp32 = ((tmp32 | CMP_C0_IER_MASK) & ~CMP_C0_CFx_MASK);
    }
    if (kACMP_OutputFallingInterruptEnable == (mask & kACMP_OutputFallingInterruptEnable))
    {
        tmp32 = ((tmp32 | CMP_C0_IEF_MASK) & ~CMP_C0_CFx_MASK);
    }
    base->C0 = tmp32;

    /* CMPx_C2
     * Set round robin interrupt enable flag.
     */
    if (kACMP_RoundRobinInterruptEnable == (mask & kACMP_RoundRobinInterruptEnable))
    {
        tmp32 = base->C2;
        /* Set control bit. Avoid clearing status flags at the same time. */
        tmp32 = ((tmp32 | CMP_C2_RRIE_MASK) & ~CMP_C2_CHnF_MASK);
        base->C2 = tmp32;
    }
}

void ACMP_DisableInterrupts(CMP_Type *base, uint32_t mask)
{
    uint32_t tmp32;

    /* CMPx_C0
     * Set control bit. Avoid clearing status flags at the same time.
     * Clear CMP interrupt enable flag.
     */
    tmp32 = base->C0;
    if (kACMP_OutputRisingInterruptEnable == (mask & kACMP_OutputRisingInterruptEnable))
    {
        tmp32 &= ~(CMP_C0_IER_MASK | CMP_C0_CFx_MASK);
    }
    if (kACMP_OutputFallingInterruptEnable == (mask & kACMP_OutputFallingInterruptEnable))
    {
        tmp32 &= ~(CMP_C0_IEF_MASK | CMP_C0_CFx_MASK);
    }
    base->C0 = tmp32;

    /* CMPx_C2
     * Clear round robin interrupt enable flag.
     */
    if (kACMP_RoundRobinInterruptEnable == (mask & kACMP_RoundRobinInterruptEnable))
    {
        tmp32 = base->C2;
        /* Set control bit. Avoid clearing status flags at the same time. */
        tmp32 &= ~(CMP_C2_RRIE_MASK | CMP_C2_CHnF_MASK);
        base->C2 = tmp32;
    }
}

uint32_t ACMP_GetStatusFlags(CMP_Type *base)
{
    uint32_t status = 0U;
    uint32_t tmp32 = base->C0;

    /* CMPx_C0
     * Check if each flag is set.
     */
    if (CMP_C0_CFR_MASK == (tmp32 & CMP_C0_CFR_MASK))
    {
        status |= kACMP_OutputRisingEventFlag;
    }
    if (CMP_C0_CFF_MASK == (tmp32 & CMP_C0_CFF_MASK))
    {
        status |= kACMP_OutputFallingEventFlag;
    }
    if (CMP_C0_COUT_MASK == (tmp32 & CMP_C0_COUT_MASK))
    {
        status |= kACMP_OutputAssertEventFlag;
    }

    return status;
}

void ACMP_ClearStatusFlags(CMP_Type *base, uint32_t mask)
{
    /* CMPx_C0 */
    uint32_t tmp32 = (base->C0 & (~(CMP_C0_CFR_MASK | CMP_C0_CFF_MASK)));

    /* Clear flag according to mask. */
    if (kACMP_OutputRisingEventFlag == (mask & kACMP_OutputRisingEventFlag))
    {
        tmp32 |= CMP_C0_CFR_MASK;
    }
    if (kACMP_OutputFallingEventFlag == (mask & kACMP_OutputFallingEventFlag))
    {
        tmp32 |= CMP_C0_CFF_MASK;
    }
    base->C0 = tmp32;
}

#if defined(FSL_FEATURE_ACMP_HAS_C3_REG) && (FSL_FEATURE_ACMP_HAS_C3_REG == 1U)
void ACMP_SetDiscreteModeConfig(CMP_Type *base, const acmp_discrete_mode_config_t *config)
{
    uint32_t tmp32 = 0U;

    if (!config->enablePositiveChannelDiscreteMode)
    {
        tmp32 |= CMP_C3_PCHCTEN_MASK;
    }
    if (!config->enableNegativeChannelDiscreteMode)
    {
        tmp32 |= CMP_C3_NCHCTEN_MASK;
    }
    if (config->enableResistorDivider)
    {
        tmp32 |= CMP_C3_RDIVE_MASK;
    }

    tmp32 |= CMP_C3_DMCS(config->clockSource)      /* Select the clock. */
             | CMP_C3_ACSAT(config->sampleTime)    /* Sample time period. */
             | CMP_C3_ACPH1TC(config->phase1Time)  /* Phase 1 sample time. */
             | CMP_C3_ACPH2TC(config->phase2Time); /* Phase 2 sample time. */

    base->C3 = tmp32;
}

void ACMP_GetDefaultDiscreteModeConfig(acmp_discrete_mode_config_t *config)
{
    assert(NULL != config);

    config->enablePositiveChannelDiscreteMode = false;
    config->enableNegativeChannelDiscreteMode = false;
    config->enableResistorDivider = false;
    config->clockSource = kACMP_DiscreteClockSlow;
    config->sampleTime = kACMP_DiscreteSampleTimeAs1T;
    config->phase1Time = kACMP_DiscretePhaseTimeAlt0;
    config->phase2Time = kACMP_DiscretePhaseTimeAlt0;
}

#endif /* FSL_FEATURE_ACMP_HAS_C3_REG */
