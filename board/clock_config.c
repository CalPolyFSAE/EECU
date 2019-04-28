/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/
/*
 * How to setup clock using clock driver functions:
 *
 * 1. Call CLOCK_InitXXX() to configure corresponding SCG clock source.
 *    Note: The clock could not be set when it is being used as system clock.
 *    In default out of reset, the CPU is clocked from FIRC(IRC48M),
 *    so before setting FIRC, change to use another avaliable clock source.
 *
 * 2. Call CLOCK_SetXtal0Freq() to set XTAL0 frequency based on board settings.
 *
 * 3. Call CLOCK_SetXxxModeSysClkConfig() to set SCG mode for Xxx run mode.
 *    Wait until the system clock source is changed to target source.
 *
 * 4. If power mode change is needed, call SMC_SetPowerModeProtection() to allow
 *    corresponding power mode and SMC_SetPowerModeXxx() to change to Xxx mode.
 *    Supported run mode and clock restrictions could be found in Reference Manual.
 */

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v5.0
processor: MKE18F512xxx16
package_id: MKE18F512VLH16
mcu_data: ksdk2_0
processor_version: 5.0.0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SCG_SOSC_DISABLE                                  0U  /*!< System OSC disabled */

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_CONFIG_FircSafeConfig
 * Description   : This function is used to safely configure FIRC clock.
 *                 In default out of reset, the CPU is clocked from FIRC(IRC48M).
 *                 Before setting FIRC, change to use SIRC as system clock,
 *                 then configure FIRC. After FIRC is set, change back to use FIRC
 *                 in case SIRC need to be configured.
 * Param fircConfig  : FIRC configuration.
 *
 *END**************************************************************************/
static void CLOCK_CONFIG_FircSafeConfig(const scg_firc_config_t *fircConfig)
{
    scg_sys_clk_config_t curConfig;
    const scg_sirc_config_t scgSircConfig = {.enableMode = kSCG_SircEnable,
                                             .div1 = kSCG_AsyncClkDisable,
                                             .div2 = kSCG_AsyncClkDivBy2,
                                             .range = kSCG_SircRangeHigh};
    scg_sys_clk_config_t sysClkSafeConfigSource = {
         .divSlow = kSCG_SysClkDivBy4, /* Slow clock divider */
         .divBus = kSCG_SysClkDivBy1,  /* Bus clock divider */
         .divCore = kSCG_SysClkDivBy1, /* Core clock divider */
         .src = kSCG_SysClkSrcSirc     /* System clock source */
    };
    /* Init Sirc. */
    CLOCK_InitSirc(&scgSircConfig);
    /* Change to use SIRC as system clock source to prepare to change FIRCCFG register. */
    CLOCK_SetRunModeSysClkConfig(&sysClkSafeConfigSource);
    /* Wait for clock source switch finished. */
    do
    {
         CLOCK_GetCurSysClkConfig(&curConfig);
    } while (curConfig.src != sysClkSafeConfigSource.src);

    /* Init Firc. */
    CLOCK_InitFirc(fircConfig);
    /* Change back to use FIRC as system clock source in order to configure SIRC if needed. */
    sysClkSafeConfigSource.src = kSCG_SysClkSrcFirc;
    CLOCK_SetRunModeSysClkConfig(&sysClkSafeConfigSource);
    /* Wait for clock source switch finished. */
    do
    {
         CLOCK_GetCurSysClkConfig(&curConfig);
    } while (curConfig.src != sysClkSafeConfigSource.src);
}

/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/
void BOARD_InitBootClocks(void)
{
    BOARD_BootClockRUN();
}

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockRUN
called_from_default_init: true
outputs:
- {id: Bus_clock.outFreq, value: 60 MHz}
- {id: Core_clock.outFreq, value: 60 MHz}
- {id: FIRCDIV1_CLK.outFreq, value: 60 MHz}
- {id: FIRCDIV2_CLK.outFreq, value: 30 MHz}
- {id: Flash_clock.outFreq, value: 12 MHz}
- {id: LPO1KCLK.outFreq, value: 1 kHz}
- {id: LPO_clock.outFreq, value: 128 kHz}
- {id: PCC.PCC_ADC0_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_ADC1_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_ADC2_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_FLEXIO_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_FTM0_CLK.outFreq, value: 60 MHz}
- {id: PCC.PCC_FTM1_CLK.outFreq, value: 60 MHz}
- {id: PCC.PCC_FTM2_CLK.outFreq, value: 60 MHz}
- {id: PCC.PCC_FTM3_CLK.outFreq, value: 60 MHz}
- {id: PCC.PCC_LPI2C0_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_LPI2C1_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_LPIT0_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_LPSPI0_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_LPSPI1_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_LPTMR0_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_LPUART0_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_LPUART1_CLK.outFreq, value: 30 MHz}
- {id: PCC.PCC_LPUART2_CLK.outFreq, value: 30 MHz}
- {id: PLLDIV1_CLK.outFreq, value: 180 MHz}
- {id: PLLDIV2_CLK.outFreq, value: 90 MHz}
- {id: SIRCDIV1_CLK.outFreq, value: 8 MHz}
- {id: SIRCDIV2_CLK.outFreq, value: 4 MHz}
- {id: SIRC_CLK.outFreq, value: 8 MHz}
- {id: System_clock.outFreq, value: 60 MHz}
settings:
- {id: PCC.PCC_ADC0_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_ADC1_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_ADC2_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_FLEXIO_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_FTM0_SEL.sel, value: SCG.FIRCDIV1_CLK}
- {id: PCC.PCC_FTM1_SEL.sel, value: SCG.FIRCDIV1_CLK}
- {id: PCC.PCC_FTM2_SEL.sel, value: SCG.FIRCDIV1_CLK}
- {id: PCC.PCC_FTM3_SEL.sel, value: SCG.FIRCDIV1_CLK}
- {id: PCC.PCC_LPI2C0_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_LPI2C1_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_LPIT0_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_LPSPI0_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_LPSPI1_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_LPTMR0_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_LPUART0_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_LPUART1_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: PCC.PCC_LPUART2_SEL.sel, value: SCG.FIRCDIV2_CLK}
- {id: SCG.DIVCORE.scale, value: '1', locked: true}
- {id: SCG.DIVSLOW.scale, value: '5'}
- {id: SCG.FIRCDIV1.scale, value: '1', locked: true}
- {id: SCG.FIRCDIV2.scale, value: '2', locked: true}
- {id: SCG.PREDIV.scale, value: '3', locked: true}
- {id: SCG.SIRCDIV1.scale, value: '1', locked: true}
- {id: SCG.SIRCDIV2.scale, value: '2', locked: true}
- {id: SCG.SPLLDIV1.scale, value: '1', locked: true}
- {id: SCG.SPLLDIV2.scale, value: '2', locked: true}
- {id: SCG.SPLLSRCSEL.sel, value: SCG.FIRC}
- {id: SCG.SPLL_mul.scale, value: '18', locked: true}
- {id: SCG_FIRCCSR_FIRCLPEN_CFG, value: Enabled}
- {id: SCG_FIRCCSR_FIRCSTEN_CFG, value: Enabled}
- {id: SCG_SOSCCFG_OSC_MODE_CFG, value: ModeOscLowPower}
- {id: SCG_SPLLCSR_SPLLEN_CFG, value: Enabled}
sources:
- {id: SCG.FIRC.outFreq, value: 60 MHz}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockRUN configuration
 ******************************************************************************/
const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockRUN =
    {
        .divSlow = kSCG_SysClkDivBy5,             /* Slow Clock Divider: divided by 5 */
        .divBus = kSCG_SysClkDivBy1,              /* Bus Clock Divider: divided by 1 */
        .divCore = kSCG_SysClkDivBy1,             /* Core Clock Divider: divided by 1 */
        .src = kSCG_SysClkSrcFirc,                /* Fast IRC is selected as System Clock Source */
    };
const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockRUN =
    {
        .freq = 0U,                               /* System Oscillator frequency: 0Hz */
        .enableMode = SCG_SOSC_DISABLE,           /* System OSC disabled */
        .monitorMode = kSCG_SysOscMonitorDisable, /* Monitor disabled */
        .div1 = kSCG_AsyncClkDisable,             /* System OSC Clock Divider 1: Clock output is disabled */
        .div2 = kSCG_AsyncClkDisable,             /* System OSC Clock Divider 2: Clock output is disabled */
        .workMode = kSCG_SysOscModeOscLowPower,   /* Oscillator low power */
    };
const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockRUN =
    {
        .enableMode = kSCG_SircEnable | kSCG_SircEnableInLowPower,/* Enable SIRC clock, Enable SIRC in low power mode */
        .div1 = kSCG_AsyncClkDivBy1,              /* Slow IRC Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy2,              /* Slow IRC Clock Divider 2: divided by 2 */
        .range = kSCG_SircRangeHigh,              /* Slow IRC high range clock (8 MHz) */
    };
const scg_firc_config_t g_scgFircConfig_BOARD_BootClockRUN =
    {
        .enableMode = kSCG_FircEnable | kSCG_FircEnableInStop | kSCG_FircEnableInLowPower,/* Enable FIRC clock, Enable FIRC in stop mode, Enable FIRC in low power mode */
        .div1 = kSCG_AsyncClkDivBy1,              /* Fast IRC Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy2,              /* Fast IRC Clock Divider 2: divided by 2 */
        .range = kSCG_FircRange60M,               /* Fast IRC is trimmed to 60MHz */
        .trimConfig = NULL,                       /* Fast IRC Trim disabled */
    };
const scg_spll_config_t g_scgSysPllConfig_BOARD_BootClockRUN =
    {
        .enableMode = kSCG_SysPllEnable,          /* Enable SPLL clock */
        .monitorMode = kSCG_SysPllMonitorDisable, /* Monitor disabled */
        .div1 = kSCG_AsyncClkDivBy1,              /* System PLL Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy2,              /* System PLL Clock Divider 2: divided by 2 */
        .src = kSCG_SysPllSrcFirc,                /* System PLL clock source is Fast IRC */
        .prediv = 2,                              /* Divided by 3 */
        .mult = 2,                                /* Multiply Factor is 18 */
    };
/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void)
{
    scg_sys_clk_config_t curConfig;

    /* Init FIRC. */
    CLOCK_CONFIG_FircSafeConfig(&g_scgFircConfig_BOARD_BootClockRUN);
    /* Init SIRC. */
    CLOCK_InitSirc(&g_scgSircConfig_BOARD_BootClockRUN);
    /* Init SysPll. */
    CLOCK_InitSysPll(&g_scgSysPllConfig_BOARD_BootClockRUN);
    /* Set SCG to FIRC mode. */
    CLOCK_SetRunModeSysClkConfig(&g_sysClkConfig_BOARD_BootClockRUN);
    /* Wait for clock source switch finished. */
    do
    {
         CLOCK_GetCurSysClkConfig(&curConfig);
    } while (curConfig.src != g_sysClkConfig_BOARD_BootClockRUN.src);
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
    /* Set PCC ADC0 selection */
    CLOCK_SetIpSrc(kCLOCK_Adc0, kCLOCK_IpSrcFircAsync);
    /* Set PCC ADC1 selection */
    CLOCK_SetIpSrc(kCLOCK_Adc1, kCLOCK_IpSrcFircAsync);
    /* Set PCC ADC2 selection */
    CLOCK_SetIpSrc(kCLOCK_Adc2, kCLOCK_IpSrcFircAsync);
    /* Set PCC LPSPI0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpspi0, kCLOCK_IpSrcFircAsync);
    /* Set PCC LPSPI1 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpspi1, kCLOCK_IpSrcFircAsync);
    /* Set PCC LPI2C0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpi2c0, kCLOCK_IpSrcFircAsync);
    /* Set PCC LPI2C1 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpi2c1, kCLOCK_IpSrcFircAsync);
    /* Set PCC LPUART0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpuart0, kCLOCK_IpSrcFircAsync);
    /* Set PCC LPUART1 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpuart1, kCLOCK_IpSrcFircAsync);
    /* Set PCC LPUART2 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpuart2, kCLOCK_IpSrcFircAsync);
    /* Set PCC FLEXIO selection */
    CLOCK_SetIpSrc(kCLOCK_Flexio0, kCLOCK_IpSrcFircAsync);
    /* Set PCC LPTMR0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lptmr0, kCLOCK_IpSrcFircAsync);
    /* Set PCC LPIT0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpit0, kCLOCK_IpSrcFircAsync);
    /* Set PCC FTM0 selection */
    CLOCK_SetIpSrc(kCLOCK_Ftm0, kCLOCK_IpSrcFircAsync);
    /* Set PCC FTM1 selection */
    CLOCK_SetIpSrc(kCLOCK_Ftm1, kCLOCK_IpSrcFircAsync);
    /* Set PCC FTM2 selection */
    CLOCK_SetIpSrc(kCLOCK_Ftm2, kCLOCK_IpSrcFircAsync);
    /* Set PCC FTM3 selection */
    CLOCK_SetIpSrc(kCLOCK_Ftm3, kCLOCK_IpSrcFircAsync);
}

