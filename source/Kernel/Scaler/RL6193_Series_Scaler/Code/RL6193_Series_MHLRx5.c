/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2012>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : RL6193_Series_MHLRx5.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#define __RL6193_SERIES_MHLRX5__

#include "ScalerFunctionInclude.h"

#if(_D5_INPUT_PORT_TYPE == _D5_MHL_PORT)

//****************************************************************************
// DEFINITIONS / MACROS
//****************************************************************************


//****************************************************************************
// STRUCT / TYPE / ENUM DEFINITTIONS
//****************************************************************************


//****************************************************************************
// CODE TABLES
//****************************************************************************


//****************************************************************************
// VARIABLE DECLARATIONS
//****************************************************************************
bit g_bMHLRx5WriteBurstProcessDone = _TRUE;


//****************************************************************************
// FUNCTION DECLARATIONS
//****************************************************************************
void ScalerMHLRx5ModeMHLSetting(void);
void ScalerMHLRx5ModeHdmiSetting(void);
void ScalerMHLRx5InitialSettings(void);
void ScalerMHLRx5CBUSPhySwitch(bit bSelect);
void ScalerMHLRx5CBUSSwitchClk(EnumClkSel enumClockSel);
void ScalerMHLRx5CBUSResetProc(void);
void ScalerMHLRx5DataPortInitial(void);
void ScalerMHLRx5DataPortInitial_EXINT0(void);
void ScalerMHLRx5PowerOffProc(void);
void ScalerMHLRx5IntHandler_EXINT0(void);
bit ScalerMHLRx5MscReadDevCapabilities(EnumMHLMscOffset enumMSCOffset, BYTE *pucData, BYTE ucExtended);
BYTE ScalerMHLRx5MscFIFOSendCommand(EnumMHLMscCommand enumMSCCommand, EnumMHLMscOffset enumMSCOffset, EnumMHLMscData enumMSCValue, BYTE ucDataLength, BYTE *pucData);
void ScalerMHLRx5MscCheckDeviceINT(void);

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)
void ScalerMHLRx5CheckWriteBurst(void);
#endif

bit ScalerMHLRx5MscHandler(EnumDisplayRegion enumMappedRegion, BYTE ucModeState);
bit ScalerMHLRx5MscRAPHandler(void);

#if(_MHL2_UCP_SUPPORT == _ON)
BYTE ScalerMHLRx5MscUCPGetCommand(BYTE *pucKeyCode);
#endif

bit ScalerMHLRx5PSPreDetect(void);
bit ScalerMHLRx5NormalPreDetect(void);

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//--------------------------------------------------
// Description  : TMDS Settings when MHL Mode for D5 Port
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5ModeMHLSetting(void)
{
    CLR_DIGITAL_PORT_SWITCH_TO_D5();

    // Clear CBUS Reset Flag for HTC Butterfly Debug
    CLR_MHL_RX5_CBUS_RESET_FOR_HTC_DONE();

    // Clear CBUS Reset Flag for Other Port Discovery Done
    CLR_MHL_RX5_SWITCH_PORT_DISCOVERY_DONE();

    PCB_D5_HDMI_MHL_SWITCH(_TMDS_SWITCH_MHL_CBUS);
    PCB_D5_MHL_VBUS_SWITCH(_MHL_VBUS_ON);

    // Port 5 Z0 Disable
    ScalerSetBit(PB4_A2_CMU_02, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), 0x00);

    // Disable Auto K-offset Auto Mode
    ScalerSetBit(PB4_AD_CDR_04, ~_BIT2, 0x00);

    // Enable MHL Mode
    ScalerSetBit(PB4_A0_CMU_00, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));

    // [7] PSM = 1'b0 --> Disable TMDS Lane3 Clock Switch
    ScalerSetBit(PB4_AB_CDR_02, ~_BIT7, 0x00);

#if(_D5_CBUS_CHANNEL_SEL == _CBUS0)
    // GPIO Select to CBUS
    ScalerSetBit(P27_A1_CBUS_EN_1, ~_BIT2, 0x00);

    // Set 1k Manual Code
    ScalerSetByte(P27_A2_CBUS_RES_AUTOK_0, (((ScalerGetByte(PB_B7_BANDGAP_03) & 0xF0) >> 4) - 0x01));

    // Set 100k Manual Code
    ScalerSetByte(P27_A3_CBUS_RES_AUTOK_1, (ScalerGetByte(PB_B7_BANDGAP_03) & 0x0F));

#elif(_D5_CBUS_CHANNEL_SEL == _CBUS1)
    // GPIO Select to CBUS
    ScalerSetBit(P27_A1_CBUS_EN_1, ~_BIT3, 0x00);

    // Set 1k Manual Code
    ScalerSetByte(P27_A2_CBUS_RES_AUTOK_0, (((ScalerGetByte(PB_B7_BANDGAP_03) & 0xF0) >> 4) - 0x01));

    // Set 100k Manual Code
    ScalerSetByte(P27_A3_CBUS_RES_AUTOK_1, (ScalerGetByte(PB_B7_BANDGAP_03) & 0x0F));

#elif(_D5_CBUS_CHANNEL_SEL == _CBUS2)
    // GPIO Select to CBUS
    ScalerSetBit(P67_A1_CBUS_EN_1, ~_BIT2, 0x00);

    // Set 1k Manual Code
    ScalerSetByte(P67_A2_CBUS_RES_AUTOK_0, (((ScalerGetByte(PB_B7_BANDGAP_03) & 0xF0) >> 4) - 0x01));

    // Set 100k Manual Code
    ScalerSetByte(P67_A3_CBUS_RES_AUTOK_1, (ScalerGetByte(PB_B7_BANDGAP_03) & 0x0F));

#elif(_D5_CBUS_CHANNEL_SEL == _CBUS3)
    // GPIO Select to CBUS
    ScalerSetBit(P67_A1_CBUS_EN_1, ~_BIT3, 0x00);

    // Set 1k Manual Code
    ScalerSetByte(P67_A2_CBUS_RES_AUTOK_0, (((ScalerGetByte(PB_B7_BANDGAP_03) & 0xF0) >> 4) - 0x01));

    // Set 100k Manual Code
    ScalerSetByte(P67_A3_CBUS_RES_AUTOK_1, (ScalerGetByte(PB_B7_BANDGAP_03) & 0x0F));

#endif // End of #if(_D5_CBUS_CHANNEL_SEL == _CBUS0)

#if(_D5_DDC_CHANNEL_SEL == _DDC0)
    // Disable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC0_SWITCH_GPIO);

    // Switch DDCRAM To MHL EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS |= _BIT0;

    // DDC0 Disable Force Nack and disable DDC Toggle IRQ
    MCU_FFE4_DDC0_EDID_IRQ &= ~(_BIT5 | _BIT3 | _BIT1);
#elif(_D5_DDC_CHANNEL_SEL == _DDC1)
    // Disable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC1_SWITCH_GPIO);

    // Switch DDCRAM To MHL EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS |= _BIT1;

    // DDC1 Disable Force Nack and disable DDC Toggle IRQ
    MCU_FFE5_DDC1_EDID_IRQ &= ~(_BIT5 | _BIT3 | _BIT1);
#elif(_D5_DDC_CHANNEL_SEL == _DDC2)
    // Disable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC2_SWITCH_GPIO);

    // Switch DDCRAM To MHL EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS |= _BIT2;

    // DDC2 Disable Force Nack and disable DDC Toggle IRQ
    MCU_FE69_DDC2_EDID_IRQ &= ~(_BIT5 | _BIT3 | _BIT1);
#elif(_D5_DDC_CHANNEL_SEL == _DDC3)
    // Disable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC3_SWITCH_GPIO);

    // Switch DDCRAM To MHL EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS |= _BIT3;

    // DDC3 Disable Force Nack and disable DDC Toggle IRQ
    MCU_FE72_DDC3_EDID_IRQ &= ~(_BIT5 | _BIT3 | _BIT1);
#elif(_D5_DDC_CHANNEL_SEL == _DDC4)
    // Disable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC4_SWITCH_GPIO);

    // Switch DDCRAM To MHL EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS |= _BIT4;

    // DDC4 Disable Force Nack and disable DDC Toggle IRQ
    MCU_FE7B_DDC4_EDID_IRQ &= ~(_BIT5 | _BIT3 | _BIT1);
#elif(_D5_DDC_CHANNEL_SEL == _DDC5)
    // Disable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC5_SWITCH_GPIO);

    // Switch DDCRAM To MHL EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS |= _BIT5;

    // DDC5 Disable Force Nack and disable DDC Toggle IRQ
    MCU_FE84_DDC5_EDID_IRQ &= ~(_BIT5 | _BIT3 | _BIT1);
#endif
}

//--------------------------------------------------
// Description  : TMDS Settings when HDMI Mode for D5 Port
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5ModeHdmiSetting(void)
{
    ScalerMHLRx5CBUSPhySwitch(_OFF);

    PCB_D5_HDMI_MHL_SWITCH(_TMDS_SWITCH_HDMI_HPD);
    PCB_D5_MHL_VBUS_SWITCH(_MHL_VBUS_OFF);

    // Disable MHL Mode
    ScalerSetBit(PB4_A0_CMU_00, ~(_BIT7 | _BIT6), 0x00);

    // [7] PSM = 1 --> Enable TMDS Lane3 Clock Switch
    ScalerSetBit(PB4_AB_CDR_02, ~_BIT7, _BIT7);

#if(_D5_CBUS_CHANNEL_SEL == _CBUS0)
    // GPIO Select to Hot Plug
    ScalerSetBit(P27_A1_CBUS_EN_1, ~_BIT2, _BIT2);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS1)
    // GPIO Select to Hot Plug
    ScalerSetBit(P27_A1_CBUS_EN_1, ~_BIT3, _BIT3);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS2)
    // GPIO Select to Hot Plug
    ScalerSetBit(P67_A1_CBUS_EN_1, ~_BIT2, _BIT2);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS3)
    // GPIO Select to Hot Plug
    ScalerSetBit(P67_A1_CBUS_EN_1, ~_BIT3, _BIT3);
#endif  // End of #if(_D5_CBUS_CHANNEL_SEL == _CBUS0)

#if(_D5_DDC_CHANNEL_SEL == _DDC0)
    // Enable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC0_SWITCH_DDC);

    // Switch DDCRAM To HDMI EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS &= ~_BIT0;

#if(_D5_EMBEDDED_DDCRAM_SIZE == _EDID_SIZE_NONE)
    // DDC0 Enable Force Nack
    MCU_FFE4_DDC0_EDID_IRQ |= _BIT5;
#endif

#elif(_D5_DDC_CHANNEL_SEL == _DDC1)
    // Enable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC1_SWITCH_DDC);

    // Switch DDCRAM To HDMI EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS &= ~_BIT1;

#if(_D5_EMBEDDED_DDCRAM_SIZE == _EDID_SIZE_NONE)
    // DDC1 Enable Force Nack
    MCU_FFE5_DDC1_EDID_IRQ |= _BIT5;
#endif

#elif(_D5_DDC_CHANNEL_SEL == _DDC2)
    // Enable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC2_SWITCH_DDC);

    // Switch DDCRAM To HDMI EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS &= ~_BIT2;

#if(_D5_EMBEDDED_DDCRAM_SIZE == _EDID_SIZE_NONE)
    // DDC2 Enable Force Nack
    MCU_FE69_DDC2_EDID_IRQ |= _BIT5;
#endif

#elif(_D5_DDC_CHANNEL_SEL == _DDC3)
    // Enable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC3_SWITCH_DDC);

    // Switch DDCRAM To HDMI EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS &= ~_BIT3;

#if(_D5_EMBEDDED_DDCRAM_SIZE == _EDID_SIZE_NONE)
    // DDC3 Enable Force Nack
    MCU_FE72_DDC3_EDID_IRQ |= _BIT5;
#endif

#elif(_D5_DDC_CHANNEL_SEL == _DDC4)
    // Enable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC4_SWITCH_DDC);

    // Switch DDCRAM To HDMI EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS &= ~_BIT4;

#if(_D5_EMBEDDED_DDCRAM_SIZE == _EDID_SIZE_NONE)
    // DDC4 Enable Force Nack
    MCU_FE7B_DDC4_EDID_IRQ |= _BIT5;
#endif

#elif(_D5_DDC_CHANNEL_SEL == _DDC5)
    // Enable DDC Channel
    ScalerPinshareGpioDdcPinSwitch(_DDC5_SWITCH_DDC);

    // Switch DDCRAM To HDMI EDID
    MCU_FEB1_DDCRAM_ST_ADDRESS &= ~_BIT5;

#if(_D5_EMBEDDED_DDCRAM_SIZE == _EDID_SIZE_NONE)
    // DDC5 Enable Force Nack
    MCU_FE84_DDC5_EDID_IRQ |= _BIT5;
#endif

#endif // End of #if(_D5_DDC_CHANNEL_SEL == _DDC0)

}

//--------------------------------------------------
// Description  : Calibration for 1K/100K and LDO Level Adjust
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5InitialSettings(void)
{
#if(_D5_CBUS_CHANNEL_SEL == _CBUS0)
    // Adjust CBUS Input Comparator VIH = 0.9V and VIL = 0.7V
    ScalerSetByte(P27_A6_CBUS_INPUT_CONTROL, 0x09);

    // Set CBUS Min Vih
    ScalerSetBit(P27_A6_CBUS_INPUT_CONTROL, ~(_BIT3 | _BIT2), (_BIT3 | _BIT2));

#if(_MHL_CBUS_LDO_HIGH == _ENABLE)
    // Set CBUS Max Driving Strength, Set LDO to 1.8V Level
    ScalerSetBit(P27_A7_CBUS_OUTPUT_CONTROL_0, ~(_BIT5 | _BIT4 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT4 | _BIT2));
#else
    // Set CBUS Max Driving Strength, Set LDO to 1.65V Level
    ScalerSetBit(P27_A7_CBUS_OUTPUT_CONTROL_0, ~(_BIT5 | _BIT4 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT4 | _BIT0));
#endif

    // DDC5 CBUS Swap Settings
    ScalerSetBit(P6F_04_CBUS0_MAP_DDC, ~(_BIT2 | _BIT1 | _BIT0), 0x05);

#elif(_D5_CBUS_CHANNEL_SEL == _CBUS1)
    // Adjust CBUS Input Comparator VIH = 0.9V and VIL = 0.7V
    ScalerSetByte(P27_A6_CBUS_INPUT_CONTROL, 0x09);

    // Set CBUS Min Vih
    ScalerSetBit(P27_A6_CBUS_INPUT_CONTROL, ~(_BIT3 | _BIT2), (_BIT3 | _BIT2));

#if(_MHL_CBUS_LDO_HIGH == _ENABLE)
    // Set CBUS Max Driving Strength, Set LDO to 1.8V Level
    ScalerSetBit(P27_A7_CBUS_OUTPUT_CONTROL_0, ~(_BIT5 | _BIT4 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT4 | _BIT2));
#else
    // Set CBUS Max Driving Strength, Set LDO to 1.65V Level
    ScalerSetBit(P27_A7_CBUS_OUTPUT_CONTROL_0, ~(_BIT5 | _BIT4 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT4 | _BIT0));
#endif

    // DDC5 CBUS Swap Settings
    ScalerSetBit(P6F_05_CBUS1_MAP_DDC, ~(_BIT2 | _BIT1 | _BIT0), 0x05);

#elif(_D5_CBUS_CHANNEL_SEL == _CBUS2)
    // Adjust CBUS Input Comparator VIH = 0.9V and VIL = 0.7V
    ScalerSetByte(P67_A6_CBUS_INPUT_CONTROL, 0x09);

    // Set CBUS Min Vih
    ScalerSetBit(P67_A6_CBUS_INPUT_CONTROL, ~(_BIT3 | _BIT2), (_BIT3 | _BIT2));

#if(_MHL_CBUS_LDO_HIGH == _ENABLE)
    // Set CBUS Max Driving Strength, Set LDO to 1.8V Level
    ScalerSetBit(P67_A7_CBUS_OUTPUT_CONTROL_0, ~(_BIT5 | _BIT4 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT4 | _BIT2));
#else
    // Set CBUS Max Driving Strength, Set LDO to 1.65V Level
    ScalerSetBit(P67_A7_CBUS_OUTPUT_CONTROL_0, ~(_BIT5 | _BIT4 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT4 | _BIT0));
#endif

    // DDC5 CBUS Swap Settings
    ScalerSetBit(P6F_06_CBUS2_MAP_DDC, ~(_BIT2 | _BIT1 | _BIT0), 0x05);

#elif(_D5_CBUS_CHANNEL_SEL == _CBUS3)
    // Adjust CBUS Input Comparator VIH = 0.9V and VIL = 0.7V
    ScalerSetByte(P67_A6_CBUS_INPUT_CONTROL, 0x09);

    // Set CBUS Min Vih
    ScalerSetBit(P67_A6_CBUS_INPUT_CONTROL, ~(_BIT3 | _BIT2), (_BIT3 | _BIT2));

#if(_MHL_CBUS_LDO_HIGH == _ENABLE)
    // Set CBUS Max Driving Strength, Set LDO to 1.8V Level
    ScalerSetBit(P67_A7_CBUS_OUTPUT_CONTROL_0, ~(_BIT5 | _BIT4 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT4 | _BIT2));
#else
    // Set CBUS Max Driving Strength, Set LDO to 1.65V Level
    ScalerSetBit(P67_A7_CBUS_OUTPUT_CONTROL_0, ~(_BIT5 | _BIT4 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT4 | _BIT0));
#endif

    // DDC5 CBUS Swap Settings
    ScalerSetBit(P6F_07_CBUS3_MAP_DDC, ~(_BIT2 | _BIT1 | _BIT0), 0x05);

#endif

    // Discovery Function Power Down and System Clk Gated
    ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT6 | _BIT5 | _BIT3 | _BIT2), _BIT6);

    // Set MHL 1MHz Clk Divider
    ScalerSetBit(P6A_A1_CBUS_CTRL_01, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT4 | _BIT3 | _BIT2));

    // Set MHL 1KHz Clk Divider and Set Stuck Low Time to 150us
    ScalerSetBit(P6A_A2_CBUS_CTRL_02, ~(_BIT4 | _BIT3 | _BIT1 | _BIT0), _BIT0);
    ScalerSetByte(P6A_A3_CBUS_CTRL_03, 0xE2);

    // Set HW Bypass Wake Up Pulses Mode
    ScalerSetBit(P6A_A4_CBUS_CTRL_04, ~_BIT7, _BIT7);

    // Set MHL Wake pulse boundary 14~26
    ScalerSetBit(P6A_A4_CBUS_CTRL_04, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT4 | _BIT3));

    // Set Discovery Upper/Lower Bound
    ScalerSetBit(P6A_A5_CBUS_CTRL_05, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), 0x5A);

    // Set Wake Up Pulse Number to 4
    ScalerSetBit(P6A_A6_CBUS_CTRL_06, ~(_BIT7 | _BIT6 | _BIT5), _BIT7);

    // Set CBUS (PHY) Power Saving Mode to Manual Mode
    ScalerSetByte(P6A_A9_CBUS_CTRL_09, 0x00);
}

//--------------------------------------------------
// Description  : Power Process for CBUS Phy for D5 Port
// Input Value  : bSelect --> _ON/_OFF
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5CBUSPhySwitch(bit bSelect)
{
    if(bSelect == _ON)
    {
        if(ScalerGetBit(P6A_A8_CBUS_CTRL_08, _BIT5) == 0x00)
        {
#if(_D5_CBUS_CHANNEL_SEL == _CBUS0)
            // GPIO Select to CBUS
            ScalerSetBit(P27_A1_CBUS_EN_1, ~_BIT2, 0x00);

            // Enable CBUS Phy, LDO, Input Comparator and Output Driver
            ScalerSetBit(P27_A0_CBUS_EN_0, ~(_BIT6 | _BIT4 | _BIT2 | _BIT0), (_BIT6 | _BIT4 | _BIT2 | _BIT0));
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS1)
            // GPIO Select to CBUS
            ScalerSetBit(P27_A1_CBUS_EN_1, ~_BIT3, 0x00);

            // Enable CBUS Phy, LDO and Input Comparator
            ScalerSetBit(P27_A0_CBUS_EN_0, ~(_BIT7 | _BIT5 | _BIT3 | _BIT1), (_BIT7 | _BIT5 | _BIT3 | _BIT1));
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS2)
            // GPIO Select to CBUS
            ScalerSetBit(P67_A1_CBUS_EN_1, ~_BIT2, 0x00);

            // Enable CBUS Phy, LDO, Input Comparator and Output Driver
            ScalerSetBit(P67_A0_CBUS_EN_0, ~(_BIT6 | _BIT4 | _BIT2 | _BIT0), (_BIT6 | _BIT4 | _BIT2 | _BIT0));
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS3)
            // GPIO Select to CBUS
            ScalerSetBit(P67_A1_CBUS_EN_1, ~_BIT3, 0x00);

            // Enable CBUS Phy, LDO and Input Comparator
            ScalerSetBit(P67_A0_CBUS_EN_0, ~(_BIT7 | _BIT5 | _BIT3 | _BIT1), (_BIT7 | _BIT5 | _BIT3 | _BIT1));
#endif
            // Discovery Function Power On and Enable System Clk
            ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT6 | _BIT5), _BIT5);

            // Enable Discovery IRQ
            ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT2 | _BIT0), (_BIT2 | _BIT0));

            // MHL Attached
            ScalerSetBit(P6A_A8_CBUS_CTRL_08, ~_BIT5, _BIT5);

            // Internal IRQ Enable (Global)
            ScalerGlobalIRQControl(_ENABLE);

            EX0 = 1;
        }
    }
    else
    {
        if(ScalerGetBit(P6A_A8_CBUS_CTRL_08, _BIT5) == _BIT5)
        {
#if(_D5_CBUS_CHANNEL_SEL == _CBUS0)
            // Disable CBUS Phy, LDO, Input Comparator
            ScalerSetBit(P27_A0_CBUS_EN_0, ~(_BIT6 | _BIT4 | _BIT2 | _BIT0), 0x00);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS1)
            // Disable CBUS Phy, LDO, Input Comparator
            ScalerSetBit(P27_A0_CBUS_EN_0, ~(_BIT7 | _BIT5 | _BIT3 | _BIT1), 0x00);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS2)
            // Disable CBUS Phy, LDO, Input Comparator
            ScalerSetBit(P67_A0_CBUS_EN_0, ~(_BIT6 | _BIT4 | _BIT2 | _BIT0), 0x00);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS3)
            // Disable CBUS Phy, LDO, Input Comparator
            ScalerSetBit(P67_A0_CBUS_EN_0, ~(_BIT7 | _BIT5 | _BIT3 | _BIT1), 0x00);
#endif
            // Discovery Function Power Down and System Clk Gated
            ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT6 | _BIT5), _BIT6);

            // Disable Discovery IRQ
            ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT2 | _BIT0), _BIT2);

            // MHL Unattached
            ScalerSetBit(P6A_A8_CBUS_CTRL_08, ~_BIT5, 0x00);
        }
    }
}

//--------------------------------------------------
// Description  : Clock Select For MHL for D5 Port
// Input Value  : ucClockSel --> System refernce clock select.
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5CBUSSwitchClk(EnumClkSel enumClockSel)
{
    if((enumClockSel == _M2PLL_CLK) && (ScalerGetBit(P6A_A0_CBUS_CTRL_00, _BIT4) != 0x00))
    {
        // Gate CBUS System Clk First (In Case of Digital Clk Glitch)
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT5, 0x00);

        // MHL System Clk select to M2PLL
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT4 | _BIT3 | _BIT2), 0x00);

        // Clear Wake Up Pulses Flag
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT3, _BIT3);

        // Sys Clk Divider
        ScalerSetBit(P6A_A1_CBUS_CTRL_01, ~(_BIT7 | _BIT6 | _BIT5), (_BIT6 | _BIT5));

        // Set Debounce For Core Function Set to 2 cycles
        ScalerSetBit(P6A_A2_CBUS_CTRL_02, ~(_BIT7 | _BIT6 | _BIT5), _BIT6);

        // Upper Bound of SYNC Pulse Low Time Period
        ScalerSetByte(P6A_AF_CBUS_CTRL_0F, 0xE1);

        // Lower Bound of SYNC Pulse Low Time Period
        ScalerSetByte(P6A_B0_CBUS_CTRL_10, 0x83);

        // Upper Bound of SYNC Pulse High Time Period
        ScalerSetByte(P6A_B1_CBUS_CTRL_11, 0x55);

        // Lower Bound of SYNC Pulse High Time Period
        ScalerSetByte(P6A_B2_CBUS_CTRL_12, 0x25);

        // Absolute Threshold Time
        ScalerSetByte(P6A_B4_CBUS_CTRL_14, 0x5A);

        // Parity Bit Time
        ScalerSetByte(P6A_B5_CBUS_CTRL_15, 0x75);

        // Parity Error Limit
        ScalerSetBit(P6A_B6_CBUS_CTRL_16, ~(_BIT7 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x0E);

        // Ack Bit Initial Falling Edge
        ScalerSetBit(P6A_B7_CBUS_CTRL_17, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x04);

        // Ack Bit Drive Low Time
        ScalerSetBit(P6A_B8_CBUS_CTRL_18, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x3A);

        // Requester Bit Time
        ScalerSetByte(P6A_BA_CBUS_CTRL_1A, 0x75);

        // Check Received Ack Bit's Falling Edge
        ScalerSetBit(P6A_BC_CBUS_CTRL_1C, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x7A);

        // Check Received Ack Bit's Driving Low Period Upper Bound
        ScalerSetBit(P6A_BD_CBUS_CTRL_1D, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x4C);

        // Check Received Ack Bit's Driving Low Period Lower Bound
        ScalerSetBit(P6A_BE_CBUS_CTRL_1E, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x24);

        // Actively Driving High Time for CBUS
        ScalerSetBit(P6A_C0_CBUS_CTRL_20, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x18);

        // CBUS Requester Transmit Opportunity after Arbitration
        ScalerSetByte(P6A_C2_CBUS_CTRL_22, 0x75);

        // CBUS Requester Continue After Ack
        ScalerSetByte(P6A_C3_CBUS_CTRL_23, 0x3A);

        // Link Layer Timeout
        ScalerSetBit(P6A_C5_CBUS_CTRL_25, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT4 | _BIT3 | _BIT2));

        // Set IIC SCL Clock Frequency
        ScalerSetByte(P6A_F9_CBUS_CTRL_59, 0x11);

        // Reset CBUS Core Function After Clk Switch
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT7, _BIT7);
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT7, 0x00);

        // Enable CBUS System Clk
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT5, _BIT5);
    }
    else if((enumClockSel == _IOSC_CLK) && (ScalerGetBit(P6A_A0_CBUS_CTRL_00, _BIT4) != _BIT4))
    {
        // Gate CBUS System Clk First (In Case of Digital Clk Glitch)
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT5, 0x00);

        // MHL System Clk select to IOSC (Bypass GDI Clk)
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT4 | _BIT3 | _BIT2), _BIT4);
        ScalerSetBit(P6A_A2_CBUS_CTRL_02, ~_BIT2, 0x00);

        // Clear Wake Up Pulses Flag
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT3, _BIT3);

        // Sys Clk Divider
        ScalerSetBit(P6A_A1_CBUS_CTRL_01, ~(_BIT7 | _BIT6 | _BIT5), 0x00);

        // Disable Debounce For Core Function
        ScalerSetBit(P6A_A2_CBUS_CTRL_02, ~(_BIT7 | _BIT6 | _BIT5), 0x00);

        // Upper Bound of SYNC Pulse Low Time Period
        ScalerSetByte(P6A_AF_CBUS_CTRL_0F, 0x35);

        // Lower Bound of SYNC Pulse Low Time Period
        ScalerSetByte(P6A_B0_CBUS_CTRL_10, 0x20);

        // Upper Bound of SYNC Pulse High Time Period
        ScalerSetByte(P6A_B1_CBUS_CTRL_11, 0x14);

        // Lower Bound of SYNC Pulse High Time Period
        ScalerSetByte(P6A_B2_CBUS_CTRL_12, 0x09);

        // Absolute Threshold Time
        ScalerSetByte(P6A_B4_CBUS_CTRL_14, 0x15);

        // Parity Bit Time
        ScalerSetByte(P6A_B5_CBUS_CTRL_15, 0x1C);

        // Parity Error Limit
        ScalerSetBit(P6A_B6_CBUS_CTRL_16, ~(_BIT7 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT2);

        // Ack Bit Initial Falling Edge
        ScalerSetBit(P6A_B7_CBUS_CTRL_17, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x01);

        // Ack Bit Drive Low Time
        ScalerSetBit(P6A_B8_CBUS_CTRL_18, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x0D);

        // Requester Bit Time
        ScalerSetByte(P6A_BA_CBUS_CTRL_1A, 0x1C);

        // Check Received Ack Bit's Falling Edge
        ScalerSetBit(P6A_BC_CBUS_CTRL_1C, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x1C);

        // Check Received Ack Bit's Driving Low Period Upper Bound
        ScalerSetBit(P6A_BD_CBUS_CTRL_1D, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x12);

        // Check Received Ack Bit's Driving Low Period Lower Bound
        ScalerSetBit(P6A_BE_CBUS_CTRL_1E, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x05);

        // Actively Driving High Time for CBUS
        ScalerSetBit(P6A_C0_CBUS_CTRL_20, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT2 | _BIT0));

        // CBUS Requester Transmit Opportunity after Arbitration
        ScalerSetByte(P6A_C2_CBUS_CTRL_22, 0x1C);

        // CBUS Requester Continue After Ack
        ScalerSetByte(P6A_C3_CBUS_CTRL_23, 0x0D);

        // Link Layer Timeout
        ScalerSetBit(P6A_C5_CBUS_CTRL_25, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT2 | _BIT1));

        // Set IIC SCL Clock Frequency
        ScalerSetByte(P6A_F9_CBUS_CTRL_59, 0x03);

        // Reset CBUS Core Function After Clk Switch
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT7, _BIT7);
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT7, 0x00);

        // Enable CBUS System Clk
        ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~_BIT5, _BIT5);
    }
}

//--------------------------------------------------
// Description  : MHL Cbus Reset Proc for D5 Port
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5CBUSResetProc(void)
{
    // Disable Stuck Low IRQ and Clear Stuck Low Flags
    ScalerSetBit(P6A_A4_CBUS_CTRL_04, ~(_BIT1 | _BIT0), _BIT1);

    // Port 5 Z0 Disable
    ScalerSetBit(PB4_A2_CMU_02, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), 0x00);

    // Disable Auto K-offset Auto Mode
    ScalerSetBit(PB4_AD_CDR_04, ~_BIT2, 0x00);

    // Manual Enable CBUS PHY PSM
#if(_D5_CBUS_CHANNEL_SEL == _CBUS0)
    ScalerSetBit(P27_A1_CBUS_EN_1, ~_BIT0, _BIT0);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS1)
    ScalerSetBit(P27_A1_CBUS_EN_1, ~_BIT1, _BIT1);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS2)
    ScalerSetBit(P67_A1_CBUS_EN_1, ~_BIT0, _BIT0);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS3)
    ScalerSetBit(P67_A1_CBUS_EN_1, ~_BIT1, _BIT1);
#endif  // End of #if(_D5_CBUS_CHANNEL_SEL == _CBUS0)

    // Force CBUS Floating
    ScalerSetBit(P6A_A8_CBUS_CTRL_08, ~_BIT7, _BIT7);

    // Enable Discovery Debounce
    ScalerSetBit(P6A_A8_CBUS_CTRL_08, ~(_BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));

    // Reset CBUS Core Function
    ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT7), _BIT7);
    ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT7), 0x00);

    SET_MHL_RX5_PROCESS_STATE(_MHL_STATE_INITIAL);
    CLR_MHL_RX5_ADOPTER_ID_HBYTE();
    CLR_MHL_RX5_ADOPTER_ID_LBYTE();
    CLR_MHL_RX5_ADOPTER_ID_DONE();
    CLR_MHL_RX5_CBUS_RESET_FOR_HTC_DONE();
    CLR_MHL_RX5_MHL_VERSION();

    ScalerMHLRx5DataPortInitial();

    // Enable Discovery Done IRQ and Clear Flag
    ScalerSetBit(P6A_A0_CBUS_CTRL_00, ~(_BIT2 | _BIT0), (_BIT2 | _BIT0));
}

//--------------------------------------------------
// Description  : MHL Cbus Mac Data Port Reset Proc for D5 Port
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5DataPortInitial(void)
{
    // MSC channel manual mode enable
    ScalerSetBit(P6A_C8_CBUS_CTRL_28, ~_BIT7, _BIT7);

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

    // Initial the value in data port
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_80_ECBUS_SPEEDS, 0x03);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_81_TMDS_SPEEDS, 0x04);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_82_ECBUS_DEV_ROLES, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_83_LOG_DEV_MAPX, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_84_ACRFS_DIV_SUPPORT, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_90_CURR_ECBUS_MODE, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_91_AVLINK_MODE_STATUS, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_92_AVLINK_MODE_CONTROL, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_93_MULTI_SINK_STATUS, 0x00);

#endif // End of #if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_DCHANGE_INT, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_CONNECTED_RDY, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_LINK_MODE, 0x00);
    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_MHL_VERSION_STAT, 0x00);

    // MSC channel manual mode disable
    ScalerSetBit(P6A_C8_CBUS_CTRL_28, ~_BIT7, 0x00);
}

//--------------------------------------------------
// Description  : MHL Cbus Mac Data Port Reset Proc for D5 Port (For Interrupt)
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5DataPortInitial_EXINT0(void) using 1
{
    // MSC channel manual mode enable
    ScalerSetBit_EXINT(P6A_C8_CBUS_CTRL_28, ~_BIT7, _BIT7);

    pData_EXINT[0] = ScalerGetByte_EXINT(P6A_AB_CBUS_CTRL_0B);

    // Disable Port Auto-Inc
    ScalerSetBit_EXINT(P6A_AB_CBUS_CTRL_0B, ~_BIT0, 0x00);

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

    // Reset _MSC_80_ECBUS_SPEEDS
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x80);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x03);

    // Reset _MSC_81_TMDS_SPEEDS
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x81);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x04);

    // Reset _MSC_82_ECBUS_DEV_ROLES
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x82);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset _MSC_83_LOG_DEV_MAPX
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x83);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset _MSC_84_ACRFS_DIV_SUPPORT
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x84);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset CURR_ECBUS_MODE
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x90);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset _MSC_91_AVLINK_MODE_STATUS
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x91);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset _MSC_92_AVLINK_MODE_CONTROL
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x92);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset _MSC_93_MULTI_SINK_STATUS
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x93);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

#endif // End of #if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

    // Reset _MSC_RCHANGE_INT
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x20);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset _MSC_DCHANGE_INT
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x21);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset _MSC_CONNECTED_READY
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x30);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset _MSC_LINK_MODE
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x31);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Reset _MSC_MHL_VERSION_STAT
    ScalerSetByte_EXINT(P6A_AC_CBUS_CTRL_0C, 0x32);
    ScalerSetByte_EXINT(P6A_AD_CBUS_CTRL_0D, 0x00);

    // Enable Port Auto-Inc
    ScalerSetByte_EXINT(P6A_AB_CBUS_CTRL_0B, pData_EXINT[0]);

    // MSC channel manual mode disable
    ScalerSetBit_EXINT(P6A_C8_CBUS_CTRL_28, ~_BIT7, 0x00);
}

//--------------------------------------------------
// Description  : MHL Power Off Process for D5 Port
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5PowerOffProc(void)
{
    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL_RX5_READY_TO_TRANSMIT);
    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL_RX5_RECONNECT_1K);
    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL_RX5_SWITCH_PORT_DISCOVERY_DONE);

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL3_RX5_WAIT_SRC_MHL_VERSION);
    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL3_RX5_WAIT_SRC_RAP_CBUS_UP);

    if((GET_MHL3_RX5_ECBUS_MODE() == _ON) || (GET_MHL3_RX5_BIST_TESTING() == _ON))
    {
        ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL3_RX5_WAIT_FWD_CLK);
        ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL3_RX5_ECBUS_START_UP);
        ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL3_RX5_ECBUS_BIST_DURATION);
    }
#endif
}

//--------------------------------------------------
// Description  : Int Process for MHL for D5 Port
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5IntHandler_EXINT0(void) using 1
{
    // Check Discovery Pulse
    if(ScalerGetBit_EXINT(P6A_A0_CBUS_CTRL_00, (_BIT2 | _BIT0)) == (_BIT2 | _BIT0))
    {
        // Disable Discovery Done IRQ and Clear Discovery Flags
        ScalerSetBit_EXINT(P6A_A0_CBUS_CTRL_00, ~(_BIT2 | _BIT0), (_BIT2));

        // Disable Discovery Debounce
        ScalerSetBit_EXINT(P6A_A8_CBUS_CTRL_08, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

        // Enable Stuck Low IRQ and Clear Flag
        ScalerSetBit_EXINT(P6A_A4_CBUS_CTRL_04, ~(_BIT1 | _BIT0), (_BIT1 | _BIT0));

        if(ScalerGetBit_EXINT(PB4_A2_CMU_02, _BIT4) == 0x00)
        {
            // Suspend Key Calculation before load key
            ScalerSetBit_EXINT(P74_AE_TMDS_RPS, ~_BIT1, 0x00);
        }

        // Enable GDI BandGap
        ScalerSetBit_EXINT(PB_B4_BANDGAP_00, ~_BIT7, _BIT7);

        // Manual Disable CBUS PHY PSM
#if(_D5_CBUS_CHANNEL_SEL == _CBUS0)
        ScalerSetBit_EXINT(P27_A1_CBUS_EN_1, ~_BIT0, 0x00);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS1)
        ScalerSetBit_EXINT(P27_A1_CBUS_EN_1, ~_BIT1, 0x00);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS2)
        ScalerSetBit_EXINT(P67_A1_CBUS_EN_1, ~_BIT0, 0x00);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS3)
        ScalerSetBit_EXINT(P67_A1_CBUS_EN_1, ~_BIT1, 0x00);
#endif  // End of #if(_D5_CBUS_CHANNEL_SEL == _CBUS0)

        // Enable RX_EN
        ScalerSetBit_EXINT(PB4_A1_CMU_01, ~_BIT7, _BIT7);

        // Port 5 Z0 Enable
        ScalerSetBit_EXINT(PB4_A2_CMU_02, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));

        // Clear HDCP Flags
        ScalerSetBit_EXINT(P74_A9_TMDS_ABC0, ~(_BIT7 | _BIT6 | _BIT5), (_BIT7 | _BIT6 | _BIT5));

        SET_MHL_RX5_PROCESS_STATE(_MHL_STATE_DISCOVERY_DONE);
        SET_MHL_RX5_READY_TO_TRANSMIT();
    }

    // Check Stuck Low Flag
    if(ScalerGetBit_EXINT(P6A_A4_CBUS_CTRL_04, (_BIT1 | _BIT0)) == (_BIT1 | _BIT0))
    {
        // Disable Stuck Low IRQ and Clear Stuck Low Flags
        ScalerSetBit_EXINT(P6A_A4_CBUS_CTRL_04, ~(_BIT1 | _BIT0), _BIT1);

        // Enable Discovery Done IRQ and Clear Flag
        ScalerSetBit_EXINT(P6A_A0_CBUS_CTRL_00, ~(_BIT2 | _BIT0), (_BIT2 | _BIT0));

        CLR_DIGITAL_PORT_SWITCH_TO_D5();

        // Port 5 Z0 Disable
        ScalerSetBit_EXINT(PB4_A2_CMU_02, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), 0x00);

        // Disable Auto K-offset Auto Mode
        ScalerSetBit_EXINT(PB4_AD_CDR_04, ~_BIT2, 0x00);

        // Manual Enable CBUS PHY PSM
#if(_D5_CBUS_CHANNEL_SEL == _CBUS0)
        ScalerSetBit_EXINT(P27_A1_CBUS_EN_1, ~_BIT0, _BIT0);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS1)
        ScalerSetBit_EXINT(P27_A1_CBUS_EN_1, ~_BIT1, _BIT1);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS2)
        ScalerSetBit_EXINT(P67_A1_CBUS_EN_1, ~_BIT0, _BIT0);
#elif(_D5_CBUS_CHANNEL_SEL == _CBUS3)
        ScalerSetBit_EXINT(P67_A1_CBUS_EN_1, ~_BIT1, _BIT1);
#endif  // End of #if(_D5_CBUS_CHANNEL_SEL == _CBUS0)

        // Enable Discovery Debounce
        ScalerSetBit_EXINT(P6A_A8_CBUS_CTRL_08, ~(_BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));

#if(_MHL_CBUS_CTS_MODE == _ON)
        // Disable DDC Manual
        ScalerSetBit_EXINT(P6A_C8_CBUS_CTRL_28, ~_BIT6, 0x00);
#endif
        // Reset CBUS Core Function
        ScalerSetBit_EXINT(P6A_A0_CBUS_CTRL_00, ~(_BIT7), _BIT7);
        ScalerSetBit_EXINT(P6A_A0_CBUS_CTRL_00, ~(_BIT7), 0x00);

        SET_MHL_RX5_PROCESS_STATE(_MHL_STATE_INITIAL);
        CLR_MHL_RX5_ADOPTER_ID_HBYTE();
        CLR_MHL_RX5_ADOPTER_ID_LBYTE();
        CLR_MHL_RX5_ADOPTER_ID_DONE();
        CLR_MHL_RX5_CBUS_RESET_FOR_HTC_DONE();
        CLR_MHL_RX5_SWITCH_PORT_DISCOVERY_DONE();
        CLR_MHL_RX5_MHL_VERSION();

        ScalerMHLRx5DataPortInitial_EXINT0();
    }
}

//--------------------------------------------------
// Description  : Read Device Capabilities for D5 Port
// Input Value  : enumMSCOffset --> Device Capability Offset
//                pucData --> Data to be Read
// Output Value : Success or Different Fail Situations
//--------------------------------------------------
bit ScalerMHLRx5MscReadDevCapabilities(EnumMHLMscOffset enumMSCOffset, BYTE *pucData, BYTE ucExtended)
{
    BYTE ucResultPacket = 0;

    ucExtended = ucExtended;

    DebugMessageMHL("ReadDev at Offset", enumMSCOffset);

    if(GET_MHL_RX5_READY_TO_TRANSMIT() == _TRUE)
    {
#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

        if(ucExtended == _MSC_READ_XDEVCAP)
        {
            ucResultPacket = ScalerMHLRx5MscFIFOSendCommand(_MSC_READ_XDEVCAP, enumMSCOffset, _MSC_NULL_VALUE, 0, pucData);
        }
        else
#endif
        {
            ucResultPacket = ScalerMHLRx5MscFIFOSendCommand(_MSC_READ_DEVCAP, enumMSCOffset, _MSC_NULL_VALUE, 0, pucData);
        }

        switch(ucResultPacket)
        {
            case _MHL_SUCCESS: // Source reply ACK Packet

                SET_MHL_RX5_READY_TO_TRANSMIT();

                DebugMessageMHL("Rx5 ReadDev MHL_SUCCESS", ScalerGetByte(P6A_D2_CBUS_CTRL_32));

                return _SUCCESS;

                break;

            case _MHL_ABORT_FAIL: // Source reply ABORT Packet

                CLR_MHL_RX5_READY_TO_TRANSMIT();
                ScalerTimerActiveTimerEvent(SEC(2.5), _SCALER_TIMER_EVENT_MHL_RX5_READY_TO_TRANSMIT);

                DebugMessageMHL("Rx5 ReadDev MHL_ABORT_FAIL", ScalerGetByte(P6A_D2_CBUS_CTRL_32));

                return _FAIL;
                break;

            case _MHL_PROTOCOL_ERROR: // Source Reply Data Packet Instead of Control Packet

                ScalerMHLRx5MscFIFOSendCommand(_MSC_ABORT, _MSC_NULL_ADDRESS, _MSC_NULL_VALUE, 0, _NULL_POINTER);

                DebugMessageMHL("Rx5 ReadDev MHL_PROTOCOL_ERROR", ScalerGetByte(P6A_D2_CBUS_CTRL_32));

                return _FAIL;
                break;

            default: // Source Reply No Packet(Timeout) or NACK

                return _FAIL;
                break;
        }
    }
    else
    {
        return _FAIL;
    }

    return _FAIL;
}

//--------------------------------------------------
// Description  : Send MSC HPD Operation for D5 Port
// Input Value  : enumMSCType --> MSC Command
//                enumMSCOffset --> MSC Device Register
//                enumMSCValue --> MSC Device Register Field
// Output Value : Success or Different Fail Situations
//--------------------------------------------------
BYTE ScalerMHLRx5MscFIFOSendCommand(EnumMHLMscCommand enumMSCCommand, EnumMHLMscOffset enumMSCOffset, EnumMHLMscData enumMSCValue, BYTE ucDataLength, BYTE *pucData)
{
    BYTE ucTimeOut = 100;
    BYTE ucSendType = 0;
    BYTE ucWaitType = 0;
    BYTE ucI = 0;

    DebugMessageMHL("RX5_MHL_READY_TO_TRANSMIT", 0x02);

    switch(enumMSCCommand)
    {
        case _MSC_SET_INT:
        case _MSC_MSG:
        case _MSC_WRITE_XSTAT:

            ucSendType = _MSC_SEND_COM_OFF_DAT;
            ucWaitType = _MSC_WAIT_COMMAND;
            break;

        case _MSC_READ_DEVCAP:
        case _MSC_READ_XDEVCAP:

            ucSendType = _MSC_SEND_COM_OFF;
            ucWaitType = _MSC_WAIT_COM_DAT;
            break;

        case _MSC_SET_HPD:

            ucSendType = _MSC_SEND_COMMAND;
            ucWaitType = _MSC_WAIT_COMMAND;
            break;

        case _MSC_ABORT:

            ucSendType = _MSC_SEND_COMMAND;
            ucWaitType = _MSC_WAIT_NULL;
            break;

        case _MSC_WRITE_BURST:

            ucSendType = _MSC_SEND_COM_OFF_DAT_COM;
            ucWaitType = _MSC_WAIT_COMMAND;
            break;

        default:

            break;
    }

    // Check If Source Has Sent An ABORT Packet
    if(ScalerGetBit(P6A_CF_CBUS_CTRL_2F, _BIT2) == _BIT2)
    {
        ScalerSetByte(P6A_CF_CBUS_CTRL_2F, _BIT2);

        ScalerTimerActiveTimerEvent(SEC(2.5), _SCALER_TIMER_EVENT_MHL_RX5_READY_TO_TRANSMIT);

        CLR_MHL_RX5_READY_TO_TRANSMIT();

        return _MHL_FAIL;
    }

    // Clear FIFO and Send Command Only
    ScalerSetByte(P6A_DE_CBUS_CTRL_3E, (_BIT6 | ucSendType));

    // Set MSC Command
    ScalerSetByte(P6A_DF_CBUS_CTRL_3F, enumMSCCommand);

    // Set MSC Offset
    ScalerSetByte(P6A_E1_CBUS_CTRL_41, enumMSCOffset);

    if(enumMSCCommand == _MSC_WRITE_BURST)
    {
        // Set MSC Last Command
        ScalerSetByte(P6A_E0_CBUS_CTRL_40, _MSC_EOF_PACKET);

        // Set MSC Data
        for(ucI = 0; ucI < ucDataLength; ucI++)
        {
            ScalerSetByte(P6A_E2_CBUS_CTRL_42, pucData[ucI]);
        }
    }
    else if((enumMSCCommand != _MSC_READ_DEVCAP) && (enumMSCCommand != _MSC_READ_XDEVCAP))
    {
        // Set MSC Value
        ScalerSetByte(P6A_E2_CBUS_CTRL_42, enumMSCValue);
    }

    // MSC Packet and Wait Type
    ScalerSetBit(P6A_E3_CBUS_CTRL_43, ~(_BIT7 | _BIT6 | _BIT1 | _BIT0), ((_MSC_PACKET << 6) | ucWaitType));

    // Send Request
    ScalerSetBit(P6A_DE_CBUS_CTRL_3E, ~(_BIT7), _BIT7);

    // Check if Process is finished
    while(--ucTimeOut != 0)
    {
        if(ScalerGetBit(P6A_CF_CBUS_CTRL_2F, _BIT2) == _BIT2)
        {
            // Clear Flag
            ScalerSetBit(P6A_CF_CBUS_CTRL_2F, ~_BIT2, _BIT2);

            return _MHL_ABORT_FAIL;
        }
        else if((ScalerGetBit(P6A_F6_CBUS_CTRL_56, _BIT7) == _BIT7) && (ScalerGetBit(P6A_F7_CBUS_CTRL_57, _BIT7) == 0x00))
        {
            // Clear Flag
            ScalerSetBit(P6A_F6_CBUS_CTRL_56, ~_BIT7, _BIT7);

            if(ucWaitType == _MSC_WAIT_NULL)
            {
                return _MHL_SUCCESS;
            }
            else
            {
                // Check ACK Packet
                if(ScalerGetByte(P6A_E4_CBUS_CTRL_44) == _MSC_ACK_PACKET)
                {
                    if((enumMSCCommand == _MSC_READ_DEVCAP) || (enumMSCCommand == _MSC_READ_XDEVCAP))
                    {
                        *pucData = ScalerGetByte(P6A_E5_CBUS_CTRL_45);
                    }

                    return _MHL_SUCCESS;
                }
                else
                {
                    return _MHL_FAIL;
                }
            }
        }
        else if((ScalerGetBit(P6A_F7_CBUS_CTRL_57, _BIT7) == _BIT7) && (ScalerGetByte(P6A_D2_CBUS_CTRL_32) == _MSC_PROTOCOL_ERROR))
        {
            // Clear Flags
            ScalerSetBit(P6A_F6_CBUS_CTRL_56, ~_BIT7, _BIT7);
            ScalerSetBit(P6A_F7_CBUS_CTRL_57, ~_BIT7, _BIT7);
            ScalerSetByte(P6A_D2_CBUS_CTRL_32, 0xFF);

            return _MHL_PROTOCOL_ERROR;
        }

        ScalerTimerDelayXms(1);
    }

    // Clear Flag
    ScalerSetBit(P6A_F6_CBUS_CTRL_56, ~_BIT7, _BIT7);
    ScalerSetBit(P6A_F7_CBUS_CTRL_57, ~_BIT7, _BIT7);
    ScalerSetByte(P6A_D2_CBUS_CTRL_32, 0xFF);

    return _MHL_FAIL;
}

//--------------------------------------------------
// Description  : Check Write Status for D5 Port
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5MscCheckDeviceINT(void)
{
    WORD usTmp = 0;
    BYTE ucBirstLength = 0;
    BYTE pucWriteBurstTmp[16];
#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)
    BYTE ucCheckSum = 0;
    BYTE ucAVLinkNCode = 0xFF;
#endif

    /***************************************************************/
    /************* Get MSC_MSG(0x68) Invalid Command ***************/
    /***************************************************************/

    if(((bit)ScalerGetBit(P6A_CE_CBUS_CTRL_2E, _BIT3) == _TRUE) && ((bit)ScalerGetBit(P6A_D4_CBUS_CTRL_34, _BIT1) == _TRUE))
    {
        if(ScalerMHLRx5MscSendCommand(_MSC_MSG, _MSC_MSGE, _MSCE_INVALID_SUB_CMD) == _TRUE)
        {
            // Clear Falgs
            ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT3);
            ScalerSetBit(P6A_D4_CBUS_CTRL_34, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), _BIT1);
        }
    }


    /**************************************************************/
    /******** Get MSC Command WRITE_STAT or SET_INT(0x60) *********/
    /**************************************************************/

    if((bit)ScalerGetBit(P6A_CE_CBUS_CTRL_2E, _BIT7) == _TRUE)
    {
        //----------------------------------------------------------//
        //-------- Check DevCap Ready (DCAP_RDY, DCAP_CHG) ---------//
        //----------------------------------------------------------//
        if((((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_CONNECTED_RDY, _MSC_DCAP_RDY) == _TRUE) ||
            ((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_DCAP_CHG) == _TRUE)))
        {
            if(GET_MHL_RX5_MHL_VERSION() == _MHL_VERSION_NULL)
            {
                // Read Source Device MHL Version
                if(ScalerMHLRx5MscReadDevCapabilities(_MSC_MHL_VERSION_01, &g_ucMHLRx5MhlVersion, _MSC_READ_DEVCAP) == _TRUE)
                {
                    if((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_DCAP_CHG) == _TRUE)
                    {
                        // Write 1 Clear DevCap_Chg Flag
                        ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_DCAP_CHG);
                    }

                    // Clear Flag
                    ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT7);
                }
            }

            // Read Source Adopter ID
            if((GET_MHL_RX5_ADOPTER_ID_DONE() == _FALSE) && (GET_MHL_RX5_READY_TO_TRANSMIT() == _TRUE))
            {
                if((ScalerMHLRx5MscReadDevCapabilities(_MSC_ADOPTER_ID_H_03, &g_ucMHLRx5AdopterIDH, _MSC_READ_DEVCAP) == _TRUE) &&
                   (ScalerMHLRx5MscReadDevCapabilities(_MSC_ADOPTER_ID_L_04, &g_ucMHLRx5AdopterIDL, _MSC_READ_DEVCAP) == _TRUE))
                {
                    DebugMessageRx5("D5 MHL Adopter IDH", g_ucMHLRx5AdopterIDH);
                    DebugMessageRx5("D5 MHL Adopter IDL", g_ucMHLRx5AdopterIDL);

                    SET_MHL_RX5_ADOPTER_ID_DONE();

                    if((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_DCAP_CHG) == _TRUE)
                    {
                        // Write 1 Clear DevCap_Chg Flag
                        ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_DCAP_CHG);
                    }

                    // Clear Flag
                    ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT7 | _BIT6));
                }
            }
        }

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)
        if((GET_MHL3_RX5_SOURCE_XDEVCAP_SUPP() == _TRUE) || (GET_MHL3_RX5_STATUS_VERSION_3_0() == _TRUE))
        {
            if((ScalerMHLRx5MscReadDevCapabilities(_MSC_80_ECBUS_SPEEDS, &pData[0], _MSC_READ_XDEVCAP) == _TRUE) &&
               (ScalerMHLRx5MscReadDevCapabilities(_MSC_81_TMDS_SPEEDS, &pData[1], _MSC_READ_XDEVCAP) == _TRUE) &&
               (ScalerMHLRx5MscReadDevCapabilities(_MSC_82_ECBUS_DEV_ROLES, &pData[2], _MSC_READ_XDEVCAP) == _TRUE) &&
               (ScalerMHLRx5MscReadDevCapabilities(_MSC_83_LOG_DEV_MAPX, &pData[3], _MSC_READ_XDEVCAP) == _TRUE) &&
               (ScalerMHLRx5MscReadDevCapabilities(_MSC_84_ACRFS_DIV_SUPPORT, &pData[4], _MSC_READ_XDEVCAP) == _TRUE))
            {
                if((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_DCAP_CHG) == _TRUE)
                {
                    // Write 1 Clear DevCap_Chg Flag
                    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_DCAP_CHG);
                }

                // Clear Flag
                ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT7);
            }
        }
#endif

        //----------------------------------------------//
        //------ Check Request To Write (REQ_WRT) ------//
        //----------------------------------------------//
        if(((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_REQ_WRT) == _TRUE) && (GET_MHL_RX5_WRITE_BURST_PROCESS_DONE() == _TRUE))
        {
            if(ScalerMHLRx5MscSendCommand(_MSC_SET_INT, _MSC_RCHANGE_INT, _MSC_GRT_WRT) == _SUCCESS)
            {
                CLR_MHL_RX5_WRITE_BURST_PROCESS_DONE();

                // Clear Flag
                ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT7);

                // Clear _MSC_REQ_WRT Flag
                ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_REQ_WRT);
            }
        }

        //-----------------------------------------------------------//
        //-- Check 3D Request and Reply No Support For All Timings --//
        //-----------------------------------------------------------//
        if((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_3D_REQ) == _TRUE)
        {
            // 3D_VIC Header
            pucWriteBurstTmp[0] = _MHL_3D_VIC_HB;
            pucWriteBurstTmp[1] = _MHL_3D_VIC_LB;

            // Total Entries -> 0
            pucWriteBurstTmp[3] = 0x00;

            // SEQ -> 1
            pucWriteBurstTmp[4] = 0x01;

            // NUM_ENT -> 0
            pucWriteBurstTmp[5] = 0x00;

            // Checksum -> 256 - Sum{pucWriteBurstTmp[i]}
            pucWriteBurstTmp[2] = 256 - (pucWriteBurstTmp[0] + pucWriteBurstTmp[1] + pucWriteBurstTmp[3] + pucWriteBurstTmp[4] + pucWriteBurstTmp[5]);

            // 3D_DTD Header
            pucWriteBurstTmp[6] = _MHL_3D_DTD_HB;
            pucWriteBurstTmp[7] = _MHL_3D_DTD_LB;

            // Total Entries -> 0
            pucWriteBurstTmp[9] = 0x00;

            // SEQ -> 1
            pucWriteBurstTmp[10] = 0x01;

            // NUM_ENT -> 0
            pucWriteBurstTmp[11] = 0x00;

            // Checksum -> 256 - Sum{pucWriteBurstTmp[i]}
            pucWriteBurstTmp[8] = 256 - (pucWriteBurstTmp[6] + pucWriteBurstTmp[7] + pucWriteBurstTmp[9] + pucWriteBurstTmp[10] + pucWriteBurstTmp[11]);

            ucBirstLength = 6;

            if(ScalerMHLRx5MscSendCommand(_MSC_SET_INT, _MSC_RCHANGE_INT, _MSC_REQ_WRT) == _SUCCESS)
            {
                usTmp = 400;

                while((usTmp-- > 0) && ((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_GRT_WRT) != _TRUE))
                {
                    ScalerTimerDelayXms(5);
                }

                if(ScalerMHLRx5MscSendWriteBurst(0x40, ucBirstLength, &pucWriteBurstTmp[0], _MHL_WRITE_BURST_WITH_REQ) == _SUCCESS)
                {
                    if(ScalerMHLRx5MscSendCommand(_MSC_SET_INT, _MSC_RCHANGE_INT, _MSC_REQ_WRT) == _SUCCESS)
                    {
                        usTmp = 400;

                        while((usTmp-- > 0) && ((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_GRT_WRT) != _TRUE))
                        {
                            ScalerTimerDelayXms(5);
                        }

                        if(ScalerMHLRx5MscSendWriteBurst(0x40, ucBirstLength, &pucWriteBurstTmp[ucBirstLength], _MHL_WRITE_BURST_WITH_REQ) == _SUCCESS)
                        {
                            ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_3D_REQ);

                            // Clear Flag
                            ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT7);
                        }
                    }
                }
            }
        }

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

        /*------------------- Get FEAT_REQ Set_Int -----------------------/
            Generally, devices should not issue any other MSC commands
            between SET_INT{FEAT_REQ} and SET_INT{FEAT_COMPLETE}
        /--------------------------------------------------------------- */
        if((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_FEAT_REQ) == _TRUE)
        {
            SET_MHL3_RX5_MSC_FEAT_REQ_UNCOMPLETE();

            //-------------------- eMSC Support --------------------------//

            // eMSC Support Data Structure Header
            pucWriteBurstTmp[0] = _MHL_WRITE_BURST_ECBUS_EMSC_SUPPORT_HBYTE;
            pucWriteBurstTmp[1] = _MHL_WRITE_BURST_ECBUS_EMSC_SUPPORT_LBYTE;

            // Total Entries -> 0
            pucWriteBurstTmp[3] = 0x00;

            // SEQ -> 0
            pucWriteBurstTmp[4] = 0x00;

            // Number of Entries in this WRITE_BURST-> 0
            pucWriteBurstTmp[5] = 0x00;

            pucWriteBurstTmp[6] = _MHL_WRITE_BURST_ECBUS_BLK_RCV_BUFFER_INFO_HBYTE;
            pucWriteBurstTmp[7] = _MHL_WRITE_BURST_ECBUS_BLK_RCV_BUFFER_INFO_LBYTE;

            ucBirstLength = 8;

            // Compute Check Sum of Write_Burst
            for(usTmp = 0; usTmp < ucBirstLength; usTmp++)
            {
                if(usTmp != 0x02)
                {
                    ucCheckSum = pucWriteBurstTmp[usTmp] + ucCheckSum;
                }
            }

            pucWriteBurstTmp[2] = 256 - ucCheckSum;

            if(ScalerMHLRx5MscSendCommand(_MSC_SET_INT, _MSC_RCHANGE_INT, _MSC_REQ_WRT) == _SUCCESS)
            {
                usTmp = 100;

                while((usTmp-- > 0) && ((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_GRT_WRT) != _TRUE))
                {
                    ScalerTimerDelayXms(5);
                }

                if((ScalerMHLRx5MscSendWriteBurst(0x40, ucBirstLength, &pucWriteBurstTmp[0], _MHL_WRITE_BURST_WITH_REQ) == _SUCCESS) &&
                   (ScalerMHLRx5MscSendCommand(_MSC_SET_INT, _MSC_RCHANGE_INT, _MSC_FEAT_COMPLETE) == _SUCCESS))
                {
                    ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_FEAT_REQ);

                    CLR_MHL3_RX5_MSC_FEAT_REQ_UNCOMPLETE();
#if(_MHL3_ECBUS_STARTUP_DEBUG_MESSAGE == _ON)
                    DebugMessageMHL("3-- Get FEAT_REQ Set_Int", 0x02);
#endif
                    // Clear Flag
                    ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT7);
                }
            }
        }


        //-----------------------------------------------//
        //---- Get XDEVCAP_SUPP(0x30[1]) Write_State ----//
        //-----------------------------------------------//
        if(((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_CONNECTED_RDY, _MSC_XDEVCAP_SUPP) == _TRUE) && (GET_MHL3_RX5_SOURCE_XDEVCAP_SUPP() == _FALSE))
        {
            SET_MHL3_RX5_SOURCE_XDEVCAP_SUPP();

            // Source Deveice Support MHL Version 3.0 --> Cancel Wait WAIT_MHL_VERSION TimerEvent
            ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL3_RX5_WAIT_SRC_MHL_VERSION);

#if(_MHL3_ECBUS_STARTUP_DEBUG_MESSAGE == _ON)
            DebugMessageMHL("Source XDEVCAP_SUPP", 0x02);
#endif
            if((GET_MHL_RX5_MHL_VERSION() >= _MHL_VERSION_3_0) && (ScalerGetBit(P6A_00_OCBUS_CTRL_00, _BIT7) == 0x00))
            {
                // Enable MHL 3.0 CBUS Extended MSC Function
                ScalerSetBit(P6A_00_OCBUS_CTRL_00, ~_BIT7, _BIT7);

                // Read XDEVCAP Reg. When Receive XDEVCAP_SUPP, For 3.0 CTS
                if(ScalerMHLRx5MscReadDevCapabilities(_MSC_80_ECBUS_SPEEDS, pData, _MSC_READ_XDEVCAP) == _SUCCESS)
                {
                    // Clear Flag
                    ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT7);
                }
            }
        }


        //--------------------------------------------------//
        //-- Get Source MHL Version from Status Reg(0x32) --//
        //--------------------------------------------------//
        ScalerGetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_MHL_VERSION_STAT, 1, &pData[0], _AUTOINC);

        if((pData[0] >= 0x30) && (GET_MHL3_RX5_STATUS_VERSION_3_0() == _FALSE) && (ScalerGetBit(P6A_00_OCBUS_CTRL_00, _BIT7) == _BIT7))
        {
            SET_MHL3_RX5_STATUS_VERSION_3_0();

            // Source Deveice Support MHL Version 3.0 --> Cancel Wait WAIT_MHL_VERSION TimerEvent
            ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL3_RX5_WAIT_SRC_MHL_VERSION);

#if(_MHL3_ECBUS_STARTUP_DEBUG_MESSAGE == _ON)
            DebugMessageMHL("Source MHL3 Version", 0x02);
#endif
            // Write XState Reg. When Receive _MSC_MHL_VERSION_STAT, For 3.0 CTS
            if(ScalerMHLRx5MscSendCommand(_MSC_WRITE_XSTAT, _MSC_90_CURR_ECBUS_MODE, _MSC_ECBUS_S_MODE) == _SUCCESS)
            {
                // Clear Flag
                ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT7);
            }
        }
#endif // End of #if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)
    }

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

    /********************************************************/
    /************* Get MSC Command WRITE_XSTAT **************/
    /********************************************************/

    if(((bit)ScalerGetBit(P6A_10_OCBUS_CTRL_10, _BIT1) == _TRUE) && (ScalerGetBit(P6A_00_OCBUS_CTRL_00, _BIT7) == _BIT7))
    {
        //--------------------------------------//
        //---- Get Source MHL AV Link Rate -----//
        //--------------------------------------//
        ScalerGetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_92_AVLINK_MODE_CONTROL, 1, &ucAVLinkNCode, _AUTOINC);

        if(ucAVLinkNCode != GET_MHL3_RX5_AV_LINK_RATE())
        {
            switch(ucAVLinkNCode & (_BIT2 | _BIT1 | _BIT0))
            {
                case _ECBUS_LINK_RATE_1_5:

                    SET_MHL3_RX5_AV_LINK_RATE(_ECBUS_LINK_RATE_1_5);
                    SET_MHL3_RX5_AV_LINK_N_CODE(_ECBUS_N_CODE_1_5);

                    ScalerMHLRx5MscSendCommand(_MSC_WRITE_XSTAT, _MSC_91_AVLINK_MODE_STATUS, _MSC_CRU_LOCKED);

                    break;

                case _ECBUS_LINK_RATE_3_0:

                    SET_MHL3_RX5_AV_LINK_RATE(_ECBUS_LINK_RATE_3_0);
                    SET_MHL3_RX5_AV_LINK_N_CODE(_ECBUS_N_CODE_3_0);

                    ScalerMHLRx5MscSendCommand(_MSC_WRITE_XSTAT, _MSC_91_AVLINK_MODE_STATUS, _MSC_CRU_LOCKED);

                    break;

                case _ECBUS_LINK_RATE_6_0:

                    SET_MHL3_RX5_AV_LINK_RATE(_ECBUS_LINK_RATE_6_0);
                    SET_MHL3_RX5_AV_LINK_N_CODE(_ECBUS_N_CODE_6_0);

                    ScalerMHLRx5MscSendCommand(_MSC_WRITE_XSTAT, _MSC_91_AVLINK_MODE_STATUS, _MSC_CRU_LOCKED);

                    break;

                default:

                    SET_MHL3_RX5_AV_LINK_RATE(_ECBUS_LINK_RATE_NULL);
                    SET_MHL3_RX5_AV_LINK_N_CODE(_ECBUS_N_CODE_NULL);

                    break;
            }

            usTmp = 0;

            while((usTmp < 10) && (ScalerMHL3Rx5DataSignalDetect() == _FALSE))
            {
                ScalerTimerDelayXms(usTmp * 10);

                usTmp++;
            }

            if((GET_MHL3_RX5_ECBUS_MODE() == _ON) &&
               (GET_MHL3_RX5_AV_LINK_RATE() != _ECBUS_LINK_RATE_NULL) &&
               (GET_MHL3_RX5_AV_LINK_N_CODE() != _ECBUS_N_CODE_NULL))
            {
#if(_MHL3_ECBUS_AVLINK_DEBUG_MESSAGE == _ON)
                DebugMessageMHL("4-- Rx5 Xstat Written AV Link Rate = ", ucAVLinkNCode);
#endif

#if(_MHL3_ECBUS_AVLINK_CDR_MODE == _DCDR_MODE)
                // ECBUS Data Link Set PHY
                ScalerTMDSRx5MHL3DcdrSetPhy();
#elif(_MHL3_ECBUS_AVLINK_CDR_MODE == _ACDR_MODE)
                // ECBUS Data Link Set PHY
                ScalerTMDSRx5MHL3AcdrSetPhy();
#endif
                SET_TMDS_RX5_PHY_SET();

                // Clear Flag
                ScalerSetBit(P6A_10_OCBUS_CTRL_10, ~(_BIT1 | _BIT0), _BIT1);
            }
        }
    }

#endif // End of #if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)



    /****************************************************************/
    /********* Source Device Reading Sink MSC Error Code ************/
    /****************************************************************/

    if((bit)ScalerGetBit(P6A_CE_CBUS_CTRL_2E, _BIT0) == _TRUE)
    {
        // Clear Flag
        ScalerSetBit(P6A_CE_CBUS_CTRL_2E, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT0);
    }

    // Sent Command to Triger VBUS for HTC M8
    if(((GET_MHL_RX5_ADOPTER_ID_HBYTE() == _HTC_BUTTERFLY_ADOPTER_ID_HBYTE) && (GET_MHL_RX5_ADOPTER_ID_LBYTE() == _HTC_BUTTERFLY_ADOPTER_ID_LBYTE)) &&
       (GET_MHL_RX5_SENT_MSC_DCAP_RDY() == _FALSE) && (GET_MHL_RX5_PROCESS_STATE() == _MHL_STATE_SET_DEV_CAP_DONE))
    {
        ScalerMHLRx5MscSendCommand(_MSC_WRITE_STAT, _MSC_CONNECTED_RDY, _MSC_DCAP_RDY);
        DebugMessageRx5("D5 MHL Sent Devies Capabilitly Ready", 0);
        SET_MHL_RX5_SENT_MSC_DCAP_RDY();
    }

#if(_D5_MHL_VERSION < _MHL_VERSION_3_0)

    // MHL Version Switch to 1.2 For Samsung Note4
    if(((GET_MHL_RX5_ADOPTER_ID_HBYTE() == _SAMSUNG_NOTE2_ADOPTER_ID_HBYTE) && (GET_MHL_RX5_ADOPTER_ID_LBYTE() == _SAMSUNG_NOTE2_ADOPTER_ID_LBYTE)) &&
       (GET_MHL_RX5_MHL_VERSION() >= _MHL_VERSION_3_0) && (GET_MHL_RX5_CBUS_RESET_FOR_HTC_DONE() == _FALSE))
    {
        ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_MHL_VERSION_01, _MHL_VERSION_1_2);

        ScalerMHLRx5MscSendCommand(_MSC_WRITE_STAT, _MSC_CONNECTED_RDY, _MSC_DCAP_RDY);

        SET_MHL_RX5_CBUS_RESET_FOR_HTC_DONE();
    }

#endif // End of #if(_D5_MHL_VERSION < _MHL_VERSION_3_0)

#if(_MHL_CBUS_CTS_MODE == _ON)

    if(ScalerGetBit(P6A_CE_CBUS_CTRL_2E, _BIT5) == _BIT5)
    {
        ScalerSetByte(P6A_C8_CBUS_CTRL_28, _BIT6);

        ScalerSetByte(P6A_CE_CBUS_CTRL_2E, _BIT5);
    }

    if(ScalerGetBit(P6A_CE_CBUS_CTRL_2E, _BIT4) == _BIT4)
    {
        ScalerSetByte(P6A_C8_CBUS_CTRL_28, 0x00);

        ScalerSetByte(P6A_CE_CBUS_CTRL_2E, _BIT4);
    }

#endif // Enf of #if(_MHL_CBUS_CTS_MODE == _ON)
}

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)
//--------------------------------------------------
// Description  : Check Write Burst for D5 Port
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerMHLRx5CheckWriteBurst(void)
{
    // Receive Write_Burst
    if(((bit)ScalerGetBit(P6A_CF_CBUS_CTRL_2F, _BIT7) == _TRUE) && (GET_MHL_RX5_WRITE_BURST_PROCESS_DONE() == _FALSE))
    {
        BYTE ucTmp = 0;
        BYTE ucTmp2 = 0;

        BYTE ucFifoLength = 0;
        BYTE pucFifoRxArray[16];
        BYTE pucFifoTxArray[16];
        BYTE ucTotEnt = 0;
        BYTE ucSeq = 0;
        BYTE ucNumEnt = 0;
        BYTE ucCheckSum = 0;

        BYTE ucECBUSBISTReady = 0;

        // Get Rx FIFO Length
        ucFifoLength = 16;

        // Get Rx FIFO Data (Write_Burst)
        ScalerGetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_40_SCRATCHPAD_OFFSET, ucFifoLength, pucFifoRxArray, _AUTOINC);

#if(_MHL3_ECBUS_WRITEBURST_DEBUG_MESSAGE == _ON)

        DebugMessageMHL("WB Rx5 Seq", pucFifoRxArray[4]);
        DebugMessageMHL("WB Rx5 Tot_Seq", pucFifoRxArray[3]);

        for(ucTmp2 = 0; ucTmp2 < ucFifoLength; ucTmp2++)
        {
            DebugMessageMHL("WB Rx5", pucFifoRxArray[ucTmp2]);
        }
#endif

        //---------------------------------------//
        //---- If is VC Assign Write_Burst ------//
        //---------------------------------------//
        if((((WORD)pucFifoRxArray[0] << 8) | ((WORD)pucFifoRxArray[1] << 0)) == (((WORD)_MHL_WRITE_BURST_ECBUS_VC_ASSIGN_HBYTE << 8) | ((WORD)_MHL_WRITE_BURST_ECBUS_VC_ASSIGN_LBYTE << 0)))
        {
#if(_MHL3_ECBUS_STARTUP_DEBUG_MESSAGE == _ON)
            DebugMessageMHL("Get VC Assign Write_Burst Form Source", 0x00);
#endif
            ucTotEnt = pucFifoRxArray[3];
            ucSeq = pucFifoRxArray[4];

            SET_MHL3_RX5_ECBUS_VC_ASSIGN_NUM_ENT(ucSeq, pucFifoRxArray[5]);
            SET_MHL3_RX5_ECBUS_VC_ASSIGN_WB_LENGTH(ucSeq, ucFifoLength);

            // CBUS1 Size = 1
            SET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_SIZE(_ECBUS_CBUS1_VC_NUM, _ECBUS_CBUS1_VC_SIZE);

            // Scan Assigened VC Size
            for(ucTmp = 0; ucTmp < GET_MHL3_RX5_ECBUS_VC_ASSIGN_NUM_ENT(ucSeq); ucTmp++)
            {
                SET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_NUM((ucTmp + 1), pucFifoRxArray[(3 * ucTmp) + 6]);
                SET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_ID((ucTmp + 1), pucFifoRxArray[(3 * ucTmp) + 6 + 1]);
                SET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_SIZE((ucTmp + 1), pucFifoRxArray[(3 * ucTmp) + 6 + 2]);
                SET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_COUNT(GET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_COUNT() + 1);
            }

            // Check Entry Count of Received VC Assign
            for(ucTmp = 1; ucTmp <= ucSeq; ucTmp++)
            {
                ucNumEnt = ucNumEnt + GET_MHL3_RX5_ECBUS_VC_ASSIGN_NUM_ENT(ucTmp);
            }

            // If All VC Assign Write_Burst Have been Scanned
            if(ucNumEnt == ucTotEnt)
            {
                // If VC Size of eMSC != 0
                if(GET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_SIZE(_ECBUS_EMSC_VC_NUM) != 0x00)
                {
                    // Reallocate eMSC Virtual Channel with Assigened eMSC VC Size
                    if(ScalerMHL3Rx5ECBUSTdmReallocation() == _SUCCESS)
                    {
                        // Encode VC_CONFIRM Write_Burst
                        for(ucTmp = 1; ucTmp <= ucSeq; ucTmp++)
                        {
                            // Set VC_CONFIRM Header
                            pucFifoTxArray[0] = pucFifoRxArray[0];
                            pucFifoTxArray[1] = pucFifoRxArray[1] + 1;

                            pucFifoTxArray[3] = ucTotEnt;
                            pucFifoTxArray[4] = ucTmp;
                            pucFifoTxArray[5] = GET_MHL3_RX5_ECBUS_VC_ASSIGN_NUM_ENT(ucTmp);

                            for(ucTmp2 = 0; ucTmp2 < GET_MHL3_RX5_ECBUS_VC_ASSIGN_NUM_ENT(ucTmp); ucTmp2++)
                            {
                                pucFifoTxArray[(3 * ucTmp2) + 6] = GET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_NUM(ucTmp2 + 1);
                                pucFifoTxArray[(3 * ucTmp2) + 6 + 1] = GET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_ID(ucTmp2 + 1);

                                if(pucFifoTxArray[(3 * ucTmp2) + 6 + 1] == 0x00)
                                {
                                    pucFifoTxArray[(3 * ucTmp2) + 6 + 2] = 0x00;
                                }
                                else
                                {
                                    pucFifoTxArray[(3 * ucTmp2) + 6 + 2] = 0x00;
                                }
                            }

                            // Compute Check Sum of Write_Burst
                            for(ucTmp2 = 0; ucTmp2 < GET_MHL3_RX5_ECBUS_VC_ASSIGN_WB_LENGTH(ucTmp); ucTmp2++)
                            {
                                if(ucTmp2 != 0x02)
                                {
                                    ucCheckSum = pucFifoTxArray[ucTmp2] + ucCheckSum;
                                }
                            }
                            pucFifoTxArray[2] = 256 - ucCheckSum;

                            if(ScalerMHLRx5MscSendCommand(_MSC_SET_INT, _MSC_RCHANGE_INT, _MSC_REQ_WRT) == _SUCCESS)
                            {
                                ucTmp2 = 100;

                                while((ucTmp2-- > 0) && ((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_GRT_WRT) != _TRUE))
                                {
                                    ScalerTimerDelayXms(5);
                                }

                                if(ScalerMHLRx5MscSendWriteBurst(0x40, GET_MHL3_RX5_ECBUS_VC_ASSIGN_WB_LENGTH(ucTmp), &pucFifoTxArray, _MHL_WRITE_BURST_WITH_REQ) == _SUCCESS)
                                {
                                    // Clear Flag
                                    ScalerSetBit(P6A_CF_CBUS_CTRL_2F, ~_BIT7, _BIT7);

                                    SET_MHL_RX5_WRITE_BURST_PROCESS_DONE();
                                }
                            }
                        }
                    }
                }
                else  // if(GET_MHL3_ECBUS_VC_ASSIGN_VC_SIZE(_ECBUS_EMSC_VC_NUM) == 0x00)
                {
                    // Encode VC_CONFIRM Write_Burst
                    for(ucTmp = 1; ucTmp <= ucSeq; ucTmp++)
                    {
                        // Set VC_CONFIRM Header
                        pucFifoTxArray[0] = pucFifoRxArray[0];
                        pucFifoTxArray[1] = pucFifoRxArray[1] + 1;

                        pucFifoTxArray[3] = ucTotEnt;
                        pucFifoTxArray[4] = ucTmp;
                        pucFifoTxArray[5] = GET_MHL3_RX5_ECBUS_VC_ASSIGN_NUM_ENT(ucTmp);

                        for(ucTmp2 = 0; ucTmp2 < GET_MHL3_RX5_ECBUS_VC_ASSIGN_NUM_ENT(ucTmp); ucTmp2++)
                        {
                            pucFifoTxArray[(3 * ucTmp2) + 6] = GET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_NUM(ucTmp2 + 1);
                            pucFifoTxArray[(3 * ucTmp2) + 6 + 1] = GET_MHL3_RX5_ECBUS_VC_ASSIGN_VC_ID(ucTmp2 + 1);
                            pucFifoTxArray[(3 * ucTmp2) + 6 + 2] = 0x00;
                        }

                        // Compute Check Sum of Write_Burst
                        for(ucTmp2 = 0; ucTmp2 < GET_MHL3_RX5_ECBUS_VC_ASSIGN_WB_LENGTH(ucTmp); ucTmp2++)
                        {
                            if(ucTmp2 != 0x02)
                            {
                                ucCheckSum = pucFifoTxArray[ucTmp2] + ucCheckSum;
                            }
                        }
                        pucFifoTxArray[2] = 256 - ucCheckSum;

                        if(ScalerMHLRx5MscSendCommand(_MSC_SET_INT, _MSC_RCHANGE_INT, _MSC_REQ_WRT) == _SUCCESS)
                        {
                            ucTmp2 = 100;

                            while((ucTmp2-- > 0) && ((bit)ScalerGetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_RCHANGE_INT, _MSC_GRT_WRT) != _TRUE))
                            {
                                ScalerTimerDelayXms(5);
                            }

                            if(ScalerMHLRx5MscSendWriteBurst(0x40, GET_MHL3_RX5_ECBUS_VC_ASSIGN_WB_LENGTH(ucTmp), &pucFifoTxArray, _MHL_WRITE_BURST_WITH_REQ) == _SUCCESS)
                            {
                                // Clear Flag
                                ScalerSetBit(P6A_CF_CBUS_CTRL_2F, ~_BIT7, _BIT7);

                                SET_MHL_RX5_WRITE_BURST_PROCESS_DONE();
                            }
                        }

#if(_MHL3_ECBUS_WRITEBURST_DEBUG_MESSAGE == _ON)
                        if(GET_MHL3_RX5_ECBUS_VC_ASSIGN_WB_LENGTH(ucTmp) > 0x00)
                        {
                            DebugMessageMHL("WB Tx Seq", ucTmp);
                            DebugMessageMHL("WB Tx Length", GET_MHL3_RX5_ECBUS_VC_ASSIGN_WB_LENGTH(ucTmp));

                            for(ucTmp2 = 0; ucTmp2 < GET_MHL3_RX5_ECBUS_VC_ASSIGN_WB_LENGTH(ucTmp); ucTmp2++)
                            {
                                DebugMessageMHL("WB Tx", pucFifoTxArray[ucTmp2]);
                            }
                        }
#endif
                    }
                }
            }
        }


        //----------------------------------------//
        //---- If is BIST Setup Write_Burst ------//
        //----------------------------------------//
        else if((((WORD)pucFifoRxArray[0] << 8) | ((WORD)pucFifoRxArray[1] << 0)) == ((((WORD)_MHL_WRITE_BURST_ECBUS_BIST_SETUP_HBYTE << 8)) | ((WORD)_MHL_WRITE_BURST_ECBUS_BIST_SETUP_LBYTE << 0)))
        {
#if(_MHL3_BIST_FUNCTION_DEBUG_MESSAGE == _ON)
            for(ucTmp = 0; ucTmp < 16; ucTmp++)
            {
                DebugMessageMHL("1 BIST SETUP", pucFifoRxArray[ucTmp]);
            }
#endif
            // Set BIST Duration (Seconds)
            SET_MHL3_RX5_BIST_ECBUS_DURATION(pucFifoRxArray[3]);

            if(pucFifoRxArray[3] != 0x00)
            {
                if(pucFifoRxArray[4] == 0x01) // ECBUS PRBS
                {
                    ScalerSetBit(P6C_35_ECBUS_BIST_CONFIG, ~_BIT3, 0x00);
                    ucECBUSBISTReady = ucECBUSBISTReady | _BIT0;
                }
                else if(pucFifoRxArray[4] == 0x02) // ECBUS Fixed 8
                {
                    ScalerSetBit(P6C_35_ECBUS_BIST_CONFIG, ~_BIT3, _BIT3);
                    ScalerSetByte(P6C_36_ECBUS_BIST_FIXED8, pucFifoRxArray[6]);
                    ucECBUSBISTReady = ucECBUSBISTReady | _BIT0;
                }
                else
                {
                    ucECBUSBISTReady = ucECBUSBISTReady | _BIT4;
                }
            }
            else
            {
                ucECBUSBISTReady = ucECBUSBISTReady | _BIT4;
            }

            // Set AV Link Duration (Frames)
            if(pucFifoRxArray[11] != 0x00)
            {
                // Set AV Link Duration to Duration Mode
                ScalerSetBit(P79_02_MHL_BIST_CTRL, ~_BIT4, _BIT4);

                // Set AV Link Duration = ucFifoRxArray[11] x 32 Frames
                ScalerSetByte(P79_0D_MHL_BIST_DURATION, pucFifoRxArray[11]);
            }
            else
            {
                // Set AV Link Duration to Continuous Mode
                ScalerSetBit(P79_02_MHL_BIST_CTRL, ~_BIT4, 0x00);
            }

            // Set AV Link Rate
            switch(pucFifoRxArray[8])
            {
                case _ECBUS_LINK_RATE_1_5:

                    SET_MHL3_RX5_AV_LINK_RATE(_ECBUS_LINK_RATE_1_5);
                    SET_MHL3_RX5_AV_LINK_N_CODE(_ECBUS_N_CODE_1_5);

                    break;

                case _ECBUS_LINK_RATE_3_0:

                    SET_MHL3_RX5_AV_LINK_RATE(_ECBUS_LINK_RATE_3_0);
                    SET_MHL3_RX5_AV_LINK_N_CODE(_ECBUS_N_CODE_3_0);

                    break;

                case _ECBUS_LINK_RATE_6_0:

                    SET_MHL3_RX5_AV_LINK_RATE(_ECBUS_LINK_RATE_6_0);
                    SET_MHL3_RX5_AV_LINK_N_CODE(_ECBUS_N_CODE_6_0);

                    break;

                default:

                    SET_MHL3_RX5_AV_LINK_RATE(_ECBUS_LINK_RATE_NULL);
                    SET_MHL3_RX5_AV_LINK_N_CODE(_ECBUS_N_CODE_NULL);

                    break;
            }

            if(pucFifoRxArray[9] == 0x01) // AV PRBS
            {
                ScalerSetBit(P79_02_MHL_BIST_CTRL, ~(_BIT6 | _BIT5), _BIT5);

                // Switch Align Mode for Fixed 8/ PRBS
                ScalerSetBit(P79_03_MHL_BIST_CONFIG_1, ~_BIT4, 0X00);

                ucECBUSBISTReady = ucECBUSBISTReady | _BIT1;
            }
            else if(pucFifoRxArray[9] == 0x02) // AV Fixed 8
            {
                ScalerSetBit(P79_02_MHL_BIST_CTRL, ~(_BIT6 | _BIT5), _BIT6);

                // Switch Align Mode for Fixed 8/ PRBS
                ScalerSetBit(P79_03_MHL_BIST_CONFIG_1, ~_BIT4, 0X00);

                // Set Fixed 8 Patten
                ScalerSetByte(P79_04_MHL_BIST_CONFIG_2, pucFifoRxArray[13]);

                ucECBUSBISTReady = ucECBUSBISTReady | _BIT1;
            }
            else if(pucFifoRxArray[9] == 0x03) // AV Fixed 10
            {
                ScalerSetBit(P79_02_MHL_BIST_CTRL, ~(_BIT6 | _BIT5), (_BIT6 | _BIT5));

                // Set Fixed 10 Patten Inversion
                // Switch Align Mode for Fixed 10
                ScalerSetBit(P79_03_MHL_BIST_CONFIG_1, ~(_BIT5 | _BIT4), (((pucFifoRxArray[12] & _BIT7) >> 2) | _BIT4));

                // Set Fixed 10 Patten
                ScalerSetBit(P79_03_MHL_BIST_CONFIG_1, ~(_BIT1 | _BIT0), (pucFifoRxArray[12] & 0x11));
                ScalerSetByte(P79_04_MHL_BIST_CONFIG_2, pucFifoRxArray[13]);

                ucECBUSBISTReady = ucECBUSBISTReady | _BIT1;
            }
            else
            {
                ucECBUSBISTReady = ucECBUSBISTReady | _BIT5;
            }

            // Set AV Link Randomizer
            ScalerSetBit(P79_02_MHL_BIST_CTRL, ~_BIT2, (((~pucFifoRxArray[14]) & _BIT0) << 2));

            // ----- Impedance BIST ------ //
            switch(pucFifoRxArray[15])
            {
                case _AV_LINK_RX:

                    // Re-Connect AV Link Z0
                    ScalerSetBit(P7E_18_IMPEDANCE_00, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
                    ScalerSetBit(P7E_18_IMPEDANCE_00, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));

                    SET_MHL3_RX5_BIST_IMPEDENCE_MODE(_AV_LINK_RX);

                    ucECBUSBISTReady = ucECBUSBISTReady | _BIT2;

                    break;

                case _ECBUS_S_TX_LOW:

                    // Re-Connect ECBUS 50 Ohm
                    ScalerSetBit(P5E_06_ECBUS_Z0_CTRL_0, ~_BIT2, 0x00);
                    ScalerSetBit(P5E_06_ECBUS_Z0_CTRL_0, ~_BIT2, _BIT2);

                    SET_MHL3_RX5_BIST_IMPEDENCE_MODE(_ECBUS_S_TX_LOW);

                    ucECBUSBISTReady = ucECBUSBISTReady | _BIT2;

                    break;

                case _ECBUS_S_TX_HIGH:

                    // Re-Connect ECBUS 50 Ohm
                    ScalerSetBit(P5E_06_ECBUS_Z0_CTRL_0, ~_BIT2, 0x00);
                    ScalerSetBit(P5E_06_ECBUS_Z0_CTRL_0, ~_BIT2, _BIT2);

                    SET_MHL3_RX5_BIST_IMPEDENCE_MODE(_ECBUS_S_TX_HIGH);

                    ucECBUSBISTReady = ucECBUSBISTReady | _BIT2;

                    break;

                case _ECBUS_S_RX:

                    // Re-Connect ECBUS 50 Ohm
                    ScalerSetBit(P5E_06_ECBUS_Z0_CTRL_0, ~_BIT2, 0x00);
                    ScalerSetBit(P5E_06_ECBUS_Z0_CTRL_0, ~_BIT2, _BIT2);

                    SET_MHL3_RX5_BIST_IMPEDENCE_MODE(_ECBUS_S_RX);

                    ucECBUSBISTReady = ucECBUSBISTReady | _BIT2;

                    break;

                default:

                    break;
            }

            if(ScalerMHLRx5MscSendCommand(_MSC_MSG, _MSC_BIST_READY, ucECBUSBISTReady) == _SUCCESS)
            {
#if(_MHL3_BIST_FUNCTION_DEBUG_MESSAGE == _ON)
                // Set AV Link Video Mode
                DebugMessageMHL("2 BIST Ready", ucECBUSBISTReady);
#endif
                // Clear Flag
                ScalerSetBit(P6A_CF_CBUS_CTRL_2F, ~_BIT7, _BIT7);

                SET_MHL_RX5_WRITE_BURST_PROCESS_DONE();
            }
        }


        //---------------------------------------//
        //---- If Receive Other Write_Burst -----//
        //---------------------------------------//
        else
        {
            ScalerSetBit(P6A_CF_CBUS_CTRL_2F, ~_BIT7, _BIT7);

            SET_MHL_RX5_WRITE_BURST_PROCESS_DONE();
        }
    }
}
#endif // End of #if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)


//--------------------------------------------------
// Description  : MSC Handler for D5 Port
// Input Value  : enumMappedRegion --> Current Active Port
//                ucModeState --> Current Mode State
// Output Value : True or False
//--------------------------------------------------
bit ScalerMHLRx5MscHandler(EnumDisplayRegion enumMappedRegion, BYTE ucModeState)
{
    // Check if Discovery State is done and send HPD & Device Capabilities Ready
    switch(GET_MHL_RX5_PROCESS_STATE())
    {
        case _MHL_STATE_INITIAL:

            ScalerTimerActiveTimerEvent(SEC(5), _SCALER_TIMER_EVENT_MHL_RX5_RECONNECT_1K);

            return _FALSE;
            break;

        case _MHL_STATE_DISCOVERY_DONE:

            ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL_RX5_RECONNECT_1K);

            // Enable oCBUS Priority Option for MHL2.2 CTS
            ScalerSetBit(P6A_AB_CBUS_CTRL_0B, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT7 | _BIT6));

            // Modify Device Capabilities according to MHL Version
            ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_MHL_VERSION_01, _D5_MHL_VERSION);

            // Modify RAP/RCP/UCP Support according to MHL Version
            ScalerSetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_FEATURE_FLAG_0A, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (((BYTE)_MHL2_UCP_SUPPORT << 4) | (_MHL2_UCP_SUPPORT << 3) | (_BIT2 | _BIT1 | _BIT0)));

            // Adopter ID
            ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_ADOPTER_ID_H_03, (BYTE)(((WORD)_MHL2_ADOPTER_ID >> 8) & 0x00FF));
            ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_ADOPTER_ID_L_04, (BYTE)(_MHL2_ADOPTER_ID & 0x00FF));

            // Packed Pixel Mode Support
            ScalerSetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_VID_LINK_MODE_05, ~_BIT3, (BYTE)_MHL2_PP_MODE_SUPPORT << 3);

            // Audio Link Mode Support
            ScalerSetDataPortBit(P6A_AC_CBUS_CTRL_0C, _MSC_AUD_LINK_MODE_06, ~(_BIT1 | _BIT0), _MHL2_AUD_CHANNEL);

            // Display Support
            ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_LOG_DEV_MAP_08, 0x01);

            // Set POWLIM DEVCAP Reg.
            ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_DEV_CAT_02, 0x31);

            // Device ID
            ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_DEVICE_ID_H_0B, (BYTE)(((WORD)_MHL2_DEVICE_ID >> 8) & 0x00FF));
            ScalerSetDataPortByte(P6A_AC_CBUS_CTRL_0C, _MSC_DEVICE_ID_L_0C, (BYTE)(_MHL2_DEVICE_ID & 0x00FF));

            if(ScalerMHLRx5MscSendCommand(_MSC_WRITE_STAT, _MSC_CONNECTED_RDY, _MSC_DCAP_RDY) == _SUCCESS)
            {
                ScalerTimerActiveTimerEvent(SEC(1), _SCALER_TIMER_EVENT_MHL_RX5_SWITCH_PORT_DISCOVERY_DONE);

                DebugMessageDigital("D5 MHL Dev Cap RDY", 0x01);
                SET_MHL_RX5_PROCESS_STATE(_MHL_STATE_DEV_CAP_RDY);
            }

            break;

        default:
            break;
    }

    // Send HPD, PATH_EN and Device Capabilities Ready For Source
    if((enumMappedRegion != _DISPLAY_RGN_NONE) && (ucModeState != _MODE_STATUS_INITIAL) && (ucModeState != _MODE_STATUS_POWER_OFF))
    {
        switch(GET_MHL_RX5_PROCESS_STATE())
        {
            case _MHL_STATE_DEV_CAP_RDY:

                ScalerTimerDelayXms(150);

                if(ScalerMHLRx5MscSendCommand(_MSC_SET_HPD, _MSC_NULL_ADDRESS, _MSC_NULL_VALUE) == _SUCCESS)
                {
                    DebugMessageDigital("D5 MHL HPD Done", 0x01);
                    SET_MHL_RX5_PROCESS_STATE(_MHL_STATE_HPD_DONE);
                }

                return _TRUE;
                break;

            case _MHL_STATE_HPD_DONE:

                if((GET_MHL_RX5_ADOPTER_ID_HBYTE() == _ZTE_NUBIA_Z5_ADOPTER_ID_HBYTE) && (GET_MHL_RX5_ADOPTER_ID_LBYTE() == _ZTE_NUBIA_Z5_ADOPTER_ID_LBYTE))
                {
                    ScalerTimerDelayXms(50);
                }

                if(ScalerMHLRx5MscSendCommand(_MSC_WRITE_STAT, _MSC_LINK_MODE, _MSC_PATH_EN) == _SUCCESS)
                {
                    DebugMessageDigital("D5 MHL Path Enabled", 0x01);
                    SET_MHL_RX5_PROCESS_STATE(_MHL_STATE_PATH_EN_DONE);
                    SET_MHL_RX5_READY_TO_RESEND();

                    ScalerTimerCancelTimerEvent(_SCALER_TIMER_EVENT_MHL_RX5_SWITCH_PORT_DISCOVERY_DONE);
                }

                return _TRUE;
                break;

            case _MHL_STATE_PATH_EN_DONE:

                if(GET_TMDS_RX5_PHY_SET() == _TRUE)
                {
                    SET_MHL_RX5_PROCESS_STATE(_MHL_STATE_DONE);
                }

                return _TRUE;
                break;

            case _MHL_STATE_DONE:
            default:

                return _TRUE;
                break;
        }
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : RAP Process for D5 Port
// Input Value  : None
// Output Value : True or False (False if Content Off)
//--------------------------------------------------
bit ScalerMHLRx5MscRAPHandler(void)
{
    // Check RAP Command
    if((bit)ScalerGetBit(P6A_D4_CBUS_CTRL_34, _BIT3) == _TRUE)
    {
        if((ScalerGetByte(P6A_DC_CBUS_CTRL_3C) == _MSC_RAP_POLL) ||
           (ScalerGetByte(P6A_DC_CBUS_CTRL_3C) == _MSC_RAP_CONTENT_ON) ||
           (ScalerGetByte(P6A_DC_CBUS_CTRL_3C) == _MSC_RAP_CONTENT_OFF))
        {
            // Reply RAPK and No Error
            ScalerMHLRx5MscSendCommand(_MSC_MSG, _MSC_RAPK, _MSC_RAPK_NO_ERROR);
        }
        else
        {
            // Reply RAPK and Ineffective Code
            ScalerMHLRx5MscSendCommand(_MSC_MSG, _MSC_RAPK, _MSC_RAPK_UNRECOGNIZE_CODE);
        }

        // Clear Flag
        ScalerSetByte(P6A_D4_CBUS_CTRL_34, (BYTE)_BIT3);

        if(ScalerGetByte(P6A_DC_CBUS_CTRL_3C) == _MSC_RAP_CONTENT_OFF)
        {
            return _FALSE;
        }
    }

    return _TRUE;
}

#if(_MHL_UCP_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Get UCP Key Code for D5 Port
// Input Value  : UCP Key Code
// Output Value : True or False
//--------------------------------------------------
BYTE ScalerMHLRx5MscUCPGetCommand(BYTE *pucKeyCode)
{
    // Check UCP Command
    if(((bit)ScalerGetBit(P6A_D4_CBUS_CTRL_34, _BIT1) == _TRUE) && (ScalerGetByte(P6A_D6_CBUS_CTRL_36) == _MSC_UCP))
    {
        // Clear Flag
        ScalerSetByte(P6A_D4_CBUS_CTRL_34, (BYTE)_BIT1);

        pucKeyCode[0] = ScalerGetByte(P6A_D7_CBUS_CTRL_37);

        return _TRUE;
    }

    return _FALSE;
}
#endif  // End of #if(_MHL2_UCP_SUPPORT == _ON)


//--------------------------------------------------
// Description  : Signal PreDetection for MHL(Power Saving)
// Input Value  : Input Port
// Output Value : True : TMDS Signal Detected
//              : False : No Signal
//--------------------------------------------------
bit ScalerMHLRx5PSPreDetect(void)
{
    //---------------------------------------//
    //-------- Check RAP Content OFF --------//
    //---------------------------------------//
    if(ScalerGetByte(P6A_DC_CBUS_CTRL_3C) == _MSC_RAP_CONTENT_OFF)
    {
        return _FAIL;
    }

    //------------------------------------------------------------//
    //-------- PSM --> PNM While Wake Up Pulses Received ---------//
    //------------------------------------------------------------//
    if(ScalerGetBit(P6A_A0_CBUS_CTRL_00, (_BIT5 | _BIT4 | _BIT3)) == (_BIT5 | _BIT4 | _BIT3))
    {
        return _TRUE;
    }

    //------------------------------------------------------------//
    //------- Pre-Detect True By Switch Back to MHL Port ---------//
    //------------------------------------------------------------//
    if(GET_MHL_RX5_SWITCH_PORT_DISCOVERY_DONE() == _TRUE)
    {
        CLR_MHL_RX5_SWITCH_PORT_DISCOVERY_DONE();

        return _TRUE;
    }

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

    if((GET_MHL3_RX5_ECBUS_MODE() == _ON) || (GET_MHL3_RX5_BIST_TESTING() == _ON))
    {
        return _TRUE;
    }
#endif

#if(_MHL_PS_WITHOUT_POWERCUT_SUPPORT == _ON)
    // [7] PSM = 1 --> Enable TMDS Lane3 Clock Switch
    ScalerSetBit(PB4_AB_CDR_02, ~_BIT7, _BIT7);

    // Disable clk lane Z0
    ScalerSetBit(PB4_A2_CMU_02, ~_BIT7, 0x00);
#endif


    //------------------------------------------------------------//
    //-------- Pre-Detect True By MHL Clk Measure Stable ---------//
    //------------------------------------------------------------//

    // Reset Freq. Detect Counter(Toggle)
    ScalerSetBit(P74_E6_HDMI_FREQDET_CTRL, ~(_BIT7 | _BIT5 | _BIT4), 0x00);
    ScalerSetBit(P74_E6_HDMI_FREQDET_CTRL, ~_BIT7, _BIT7);

    // Clear Freq. Stable Flag
    ScalerSetBit(P74_EF_HDMI_ERROR_TH, ~_BIT6, _BIT6);

    // Clear Unstable Flag
    ScalerSetBit(P74_E6_HDMI_FREQDET_CTRL, ~_BIT2, _BIT2);

    // Delay enough time for freq. measure (calculated for measure 5 times)
    ScalerTimerDelayXms(5);

    // Detect Freq. Measure Stable
    if((ScalerGetBit(P74_E6_HDMI_FREQDET_CTRL, (_BIT6 | _BIT2)) == _BIT6) && (ScalerGetBit(P74_EF_HDMI_ERROR_TH, _BIT6) == _BIT6))
    {
#if(_MHL_PS_WITHOUT_POWERCUT_SUPPORT == _ON)
        // [7] PSM = 1'b0 --> Disable TMDS Lane3 Clock Switch
        ScalerSetBit(PB4_AB_CDR_02, ~_BIT7, 0x00);

        // Enable clk lane Z0
        ScalerSetBit(PB4_A2_CMU_02, ~_BIT7, _BIT7);
#endif

        return _TRUE;
    }
    else
    {
        // Disable Freq. Detection
        ScalerSetBit(P74_E6_HDMI_FREQDET_CTRL, ~_BIT7, 0x00);

#if(_MHL_PS_WITHOUT_POWERCUT_SUPPORT == _ON)
        // [7] PSM = 1'b0 --> Disable TMDS Lane3 Clock Switch
        ScalerSetBit(PB4_AB_CDR_02, ~_BIT7, 0x00);

        // Enable clk lane Z0
        ScalerSetBit(PB4_A2_CMU_02, ~_BIT7, _BIT7);
#endif
    }

    switch(GET_MHL_RX5_PROCESS_STATE())
    {
        case _MHL_STATE_INITIAL:
        case _MHL_STATE_PATH_EN_DONE:
        case _MHL_STATE_DONE:

            return _FAIL;
            break;

        default:

            return _SUCCESS;
            break;
    }

    return _FAIL;
}

//--------------------------------------------------
// Description  : Signal PreDetection for MHL(Power Normal)
// Input Value  : Input Port
// Output Value : True : TMDS Signal Detected
//              : False : No Signal
//--------------------------------------------------
bit ScalerMHLRx5NormalPreDetect(void)
{
    //---------------------------------------//
    //-------- Check RAP Content OFF --------//
    //---------------------------------------//
    if(ScalerGetByte(P6A_DC_CBUS_CTRL_3C) == _MSC_RAP_CONTENT_OFF)
    {
        return _FAIL;
    }

    //------------------------------------------------------------//
    //------- Pre-Detect True By Switch Back to MHL Port ---------//
    //------------------------------------------------------------//
    if(GET_MHL_RX5_SWITCH_PORT_DISCOVERY_DONE() == _TRUE)
    {
        CLR_MHL_RX5_SWITCH_PORT_DISCOVERY_DONE();

        return _TRUE;
    }

#if(_D5_MHL_VERSION >= _MHL_VERSION_3_0)

    // Check if BIST Testing
    if(GET_MHL3_RX5_BIST_TESTING() == _ON)
    {
        return _FAIL;
    }

    if(GET_MHL3_RX5_ECBUS_MODE() == _ON)
    {
        return _SUCCESS;
    }

#endif

    ScalerTimerDelayXms(6);

    if(GET_TMDS_RX5_PHY_SET() == _TRUE)
    {
        return _SUCCESS;
    }

    switch(GET_MHL_RX5_PROCESS_STATE())
    {
        case _MHL_STATE_INITIAL:
        case _MHL_STATE_PATH_EN_DONE:

            return _FAIL;
            break;

        default:

            return _SUCCESS;
            break;
    }

    return _FAIL;
}
#endif // End of #if(_D5_INPUT_PORT_TYPE == _D5_MHL_PORT)

