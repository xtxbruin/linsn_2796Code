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
// ID Code      : RL6193_Series_DPRx1.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include "ScalerFunctionInclude.h"

#if(_D1_DP_EXIST == _ON)
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
BYTE g_pucDpRx1Tap0MaxTemp[4];
BYTE g_pucDpRx1LEMinTemp[4];
BYTE g_pucDpRx1LEMaxTemp[4];
BYTE g_pucDpRx1Tap1MaxTemp[4];
WORD g_usDpRx1ClockCount;
WORD g_usDpRx1ClockCount1;
BYTE g_ucDpRx1PhyCtsCtrl;
BYTE g_pucDpRx1Backup[3];

//****************************************************************************
// FUNCTION DECLARATIONS
//****************************************************************************
void ScalerDpRx1ValidSignalDetection(void);
bit ScalerDpRx1DisplayFormatSetting(void);
bit ScalerDpRx1FifoCheck(EnumDpFifoCheckCondition enumFifoCheckCondition);
void ScalerDpRx1SetFifoWD(bit bEn);
void ScalerDpRx1SetFifoIRQ(bit bEn);
void ScalerDpRx1CMUNFCodeSetting(EnumDpNFCodeRef enumDpNFCodeRef, EnumDpLinkRate enumLinkRate);
void ScalerDpRx1PhyCTS(void);
void ScalerDpRx1PhyCtsACDR(void);
bit ScalerDpRx1PhyCtsTp1Check(void);
void ScalerDpRx1PhyCtsTp1SetPhy(EnumDpNFCodeRef enumDpNFCodeRef);
bit ScalerDpRx1PhyCtsTp2Check(void);
void ScalerDpRx1PhyCtsTp2SetPhy(void);
void ScalerDpRx1PhyInitial(void);
void ScalerDpRx1RebuildPhy(BYTE ucDpLinkRate, BYTE ucDpcdLane);
void ScalerDpRx1SetDFEInitial(EnumDpLinkRate enumDpLinkRate);
void ScalerDpRx1AuxIntInitial(void);
bit ScalerDpRx1CDRCheck(BYTE ucDpLinkRate, BYTE ucDpcdLane);
void ScalerDpRx1SignalDetectInitial(BYTE ucDpLinkRate);
bit ScalerDpRx1MeasureLaneCDRClk(BYTE ucDpLinkRate, BYTE ucDpRx1LaneNumber);
void ScalerDpRx1HdcpCapableModify(void);

#if(_DP_TX_SUPPORT == _ON)
BYTE ScalerDpRx1MstToSstScanInputPort(void);
bit ScalerDpRx1MstToSstGetVideoStream(void);
bit ScalerDpRx1MstToSstMSACheck(void);
bit ScalerDpRx1MstToSstMSAActiveChange(void);

bit ScalerDpRx1MstToSstGetVideoInfo(void);
void ScalerDpRx1MstToSstVideoSetting(void);
bit ScalerDpRx1MstToSstMiscCheck(void);

#if(_DP_VSC_SDP_EXT_FOR_COLORIMETRY_SUPPORT == _ON)
bit ScalerDpRx1MstToSstVscCheck(void);
#endif

bit ScalerDpRx1MstToSstStreamClkRegenerate(void);
bit ScalerDpRx1MstToSstInterlaceCheck(WORD *pusVTotal, BYTE *pucOddFlg);
DWORD ScalerDpRx1MstToSstGetBStoVSDelay(void);
bit ScalerDpRx1MstToSstDisplayFormatSetting(void);
bit ScalerDpRx1MstToSstFifoCheck(EnumDpFifoCheckCondition enumFifoCheckCondition);
void ScalerDpRx1MstToSstAdjustVsyncDelay(void);
bit ScalerDpRx1MstToSstMeasureInput(void);

#if(_DP_FREESYNC_SUPPORT == _ON)
WORD ScalerDpRx1MstToSstGetBStoHSDelay(WORD usDpHtotal);
#endif
#endif

//****************************************************************************
// FUNCTION DEFINITIONS
//****************************************************************************
//--------------------------------------------------
// Description  : Check Valid Video Data
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1ValidSignalDetection(void)
{
    BYTE ucDpLinkRate = 0;
    BYTE ucDpcdLane = 0;
    BYTE ucLane0Vth = 0;
    BYTE ucLane0LE = 0;
    BYTE ucLane1Vth = 0;
    BYTE ucLane1LE = 0;
    BYTE ucLane2Vth = 0;
    BYTE ucLane2LE = 0;
    BYTE ucLane3Vth = 0;
    BYTE ucLane3LE = 0;

    do
    {
        if(ucDpLinkRate == 0)
        {
            ucDpLinkRate = _DP_LINK_RBR;

            DebugMessageRx1("7. VBIOS : RBR", 0);
        }
        else if(ucDpLinkRate == _DP_LINK_RBR)
        {
            ucDpLinkRate = _DP_LINK_HBR;

            DebugMessageRx1("7. VBIOS : HBR", 0);
        }
        else if(ucDpLinkRate == _DP_LINK_HBR)
        {
            ucDpLinkRate = _DP_LINK_HBR2;

            DebugMessageRx1("7. VBIOS : HBR2", 0);
        }

        SET_DP_RX1_AUX_MANUAL_MODE();

#if(_DP_MST_SUPPORT == _ON)
        if((GET_DP_RX1_LINK_TRAINING_STATUS() == _DP_LINK_TRAINING_NONE) && (ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) == 0x00) && ((ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x11) & 0x03) != 0x03))
#else
        if((GET_DP_RX1_LINK_TRAINING_STATUS() == _DP_LINK_TRAINING_NONE) && (ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) == 0x00))
#endif
        {
            ScalerDpRx1SignalDetectInitial(ucDpLinkRate);

            if(ScalerDpRx1SignalCheck(ucDpLinkRate, _DP_FOUR_LANE) == _TRUE)
            {
                ucDpcdLane = _DP_FOUR_LANE;
            }
            else if(ScalerDpRx1SignalCheck(ucDpLinkRate, _DP_TWO_LANE) == _TRUE)
            {
                ucDpcdLane = _DP_TWO_LANE;
            }
            else if(ScalerDpRx1SignalCheck(ucDpLinkRate, _DP_ONE_LANE) == _TRUE)
            {
                ucDpcdLane = _DP_ONE_LANE;
            }
            else
            {
                ucDpcdLane = 0xFF;
            }

            ScalerDpRx1RebuildPhy(ucDpLinkRate, 0x84);
        }
        else
        {
            SET_DP_RX1_AUX_AUTO_MODE();

            return;
        }

        SET_DP_RX1_AUX_AUTO_MODE();

        // De-Skew Circuit Reset
        ScalerSetBit(PB0_0E_DESKEW_PHY, ~(_BIT7 | _BIT6), 0x00);
        ScalerSetBit(PB0_0E_DESKEW_PHY, ~(_BIT7 | _BIT6), _BIT6);

        // Mac Reset After Link Clock Stable
        ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT1, _BIT1);
        ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT1, 0x00);

        // Read Lane0
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_D1_DP_LANE0 << 6));

        // Read Vth
        ScalerSetByte(P7A_F0_COEF_1, 0x00);
        ucLane0Vth = (ScalerGetByte(P7A_F1_COEF_2) & 0x0F);

        // Read LE
        ScalerSetByte(P7A_F0_COEF_1, 0x09);
        ucLane0LE = (ScalerGetByte(P7A_F1_COEF_2) & 0x1F);

        // Read Lane1
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_D1_DP_LANE1 << 6));

        // Read Vth
        ScalerSetByte(P7A_F0_COEF_1, 0x00);
        ucLane1Vth = (ScalerGetByte(P7A_F1_COEF_2) & 0x0F);

        // Read LE
        ScalerSetByte(P7A_F0_COEF_1, 0x09);
        ucLane1LE = (ScalerGetByte(P7A_F1_COEF_2) & 0x1F);

        // Read Lane2
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_D1_DP_LANE2 << 6));

        // Read Vth
        ScalerSetByte(P7A_F0_COEF_1, 0x00);
        ucLane2Vth = (ScalerGetByte(P7A_F1_COEF_2) & 0x0F);

        // Read LE
        ScalerSetByte(P7A_F0_COEF_1, 0x09);
        ucLane2LE = (ScalerGetByte(P7A_F1_COEF_2) & 0x1F);

        // Read Lane3
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_D1_DP_LANE3 << 6));

        // Read Vth
        ScalerSetByte(P7A_F0_COEF_1, 0x00);
        ucLane3Vth = (ScalerGetByte(P7A_F1_COEF_2) & 0x0F);

        // Read LE
        ScalerSetByte(P7A_F0_COEF_1, 0x09);
        ucLane3LE = (ScalerGetByte(P7A_F1_COEF_2) & 0x1F);

        DebugMessageRx1("7. Lane0 Vth --> ", ucLane0Vth);
        DebugMessageRx1("7. Lane0 LE --> ", ucLane0LE);
        DebugMessageRx1("7. Lane1 Vth --> ", ucLane1Vth);
        DebugMessageRx1("7. Lane1 LE --> ", ucLane1LE);
        DebugMessageRx1("7. Lane2 Vth --> ", ucLane2Vth);
        DebugMessageRx1("7. Lane2 LE --> ", ucLane2LE);
        DebugMessageRx1("7. Lane3 Vth --> ", ucLane3Vth);
        DebugMessageRx1("7. Lane3 LE --> ", ucLane3LE);

        ScalerTimerDelayXms(5);

        // Clear 8b/10b Decode Error Flag
        ScalerSetBit(PB0_06_DECODE_10B8B_ERROR, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));

        ScalerTimerDelayXms(2);

        SET_DP_RX1_AUX_MANUAL_MODE();

#if(_DP_MST_SUPPORT == _ON)
        if((GET_DP_RX1_LINK_TRAINING_STATUS() == _DP_LINK_TRAINING_NONE) && (ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) == 0x00) && ((ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x11) & 0x03) != 0x03))
#else
        if((GET_DP_RX1_LINK_TRAINING_STATUS() == _DP_LINK_TRAINING_NONE) && (ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) == 0x00))
#endif
        {
            if(((ucLane0Vth != 0) && (ucLane0LE <= 15)) && ((ucLane1Vth != 0) && (ucLane1LE <= 15)) &&
               ((ucLane2Vth != 0) && (ucLane2LE <= 15)) && ((ucLane3Vth != 0) && (ucLane3LE <= 15)) &&
               (ucDpcdLane == _DP_FOUR_LANE) && (ScalerDpRx1CDRCheck(ucDpLinkRate, _DP_FOUR_LANE) == _TRUE) &&
               (ScalerGetBit(PB0_06_DECODE_10B8B_ERROR, (_BIT3 | _BIT2 | _BIT1 | _BIT0)) == 0x00))
            {
                ScalerDpRx1SetDpcdValue(0x00, 0x01, 0x00, ucDpLinkRate);
                ScalerDpRx1SetDpcdValue(0x00, 0x01, 0x01, ((ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x01) & 0x60) | 0x84));
                ScalerDpRx1SetDpcdValue(0x00, 0x02, 0x02, 0x77);
                ScalerDpRx1SetDpcdValue(0x00, 0x02, 0x03, 0x77);
                ScalerDpRx1SetDpcdValue(0x00, 0x02, 0x04, 0x01);

                SET_DP_RX1_LINK_TRAINING_STATUS(_DP_FAKE_LINK_TRAINING_PASS_VBIOS);

                DebugMessageRx1("7. 4 Lane", 0);

                break;
            }
            else if(((ucLane0Vth != 0) && (ucLane0LE <= 15)) && ((ucLane1Vth != 0) && (ucLane1LE <= 15)) &&
                    (ucDpcdLane == _DP_TWO_LANE) && (ScalerDpRx1CDRCheck(ucDpLinkRate, _DP_TWO_LANE) == _TRUE) &&
                    (ScalerGetBit(PB0_06_DECODE_10B8B_ERROR, (_BIT1 | _BIT0)) == 0x00))
            {
                ScalerDpRx1SetDpcdValue(0x00, 0x01, 0x00, ucDpLinkRate);
                ScalerDpRx1SetDpcdValue(0x00, 0x01, 0x01, ((ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x01) & 0x60) | 0x82));
                ScalerDpRx1SetDpcdValue(0x00, 0x02, 0x02, 0x77);
                ScalerDpRx1SetDpcdValue(0x00, 0x02, 0x04, 0x01);

                SET_DP_RX1_LINK_TRAINING_STATUS(_DP_FAKE_LINK_TRAINING_PASS_VBIOS);

                DebugMessageRx1("7. 2 Lane", 0);

                break;
            }
            else if((ucLane0Vth != 0) && (ucLane0LE <= 15) &&
                    (ucDpcdLane == _DP_ONE_LANE) && (ScalerDpRx1CDRCheck(ucDpLinkRate, _DP_ONE_LANE) == _TRUE) &&
                    (ScalerGetBit(PB0_06_DECODE_10B8B_ERROR, _BIT0) == 0x00))
            {
                ScalerDpRx1SetDpcdValue(0x00, 0x01, 0x00, ucDpLinkRate);
                ScalerDpRx1SetDpcdValue(0x00, 0x01, 0x01, ((ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x01) & 0x60) | 0x81));
                ScalerDpRx1SetDpcdValue(0x00, 0x02, 0x02, 0x07);
                ScalerDpRx1SetDpcdValue(0x00, 0x02, 0x04, 0x01);

                SET_DP_RX1_LINK_TRAINING_STATUS(_DP_FAKE_LINK_TRAINING_PASS_VBIOS);

                DebugMessageRx1("7. 1 lane", 0);

                break;
            }
        }
        else
        {
            // Set Aux HW Control Mode
            SET_DP_RX1_AUX_AUTO_MODE();

            return;
        }

        // Set Aux HW Control Mode
        SET_DP_RX1_AUX_AUTO_MODE();
    }while(ucDpLinkRate < ScalerDpRx1GetDpcdInfo(0x00, 0x00, 0x01));

#if(_DP_MST_SUPPORT == _ON)
    if(GET_DP_RX1_LINK_TRAINING_STATUS() == _DP_FAKE_LINK_TRAINING_PASS_VBIOS)
    {
        ScalerDpRx1MstInfoBackup(_DP_MST_BACKUP_INFO_RESTORE);
    }
#endif

    // Set Aux HW Control Mode
    SET_DP_RX1_AUX_AUTO_MODE();
}

//--------------------------------------------------
// Description  : DP Display Format Generator Setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
bit ScalerDpRx1DisplayFormatSetting(void)
{
    BYTE ucOddFlg = 0;
    WORD usDpVtotal = 0;
    WORD usDpHactive = 0;
    DWORD ulStreamClkValue = 0;
    DWORD ulVBs2BsCount = 0;

#if(_DP_FREESYNC_SUPPORT == _ON)
    WORD usHBs2BsCount = 0;
    DWORD ulDpMvidValue = 0;
    DWORD ulDpNvidValue = 0;
#endif

    // Measurement Enable
    ScalerSetBit(PB8_58_MN_MEAS_CTRL, ~_BIT7, 0x00);
    ScalerSetBit(PB8_58_MN_MEAS_CTRL, ~_BIT7, _BIT7);

    if(ScalerTimerPollingFlagProc(_DP_MEASURE_POLLING_TIMEOUT, PB8_58_MN_MEAS_CTRL, _BIT6, _TRUE) == _FALSE)
    {
        return _FALSE;
    }

    // Pop up The result
    ScalerSetBit(PB8_58_MN_MEAS_CTRL, ~_BIT5, _BIT5);

    // Measurement Disable
    ScalerSetBit(PB8_58_MN_MEAS_CTRL, ~_BIT7, 0x00);

    // VBs2BS count in GDI clk
    ulVBs2BsCount = ((((DWORD)(ScalerGetByte(PB8_58_MN_MEAS_CTRL) & 0x1F)) << 16) | (((DWORD)ScalerGetByte(PB8_59_MN_MEAS_VLN_M)) << 8) | ((DWORD)(ScalerGetByte(PB8_5A_MN_MEAS_VLN_L))));

#if(_DP_FREESYNC_SUPPORT == _ON)
    if(GET_FREESYNC_ENABLED() == _TRUE)
    {
        // Get IVfreq
        PDATA_DWORD(0) = ((DWORD)_GDI_CLK_KHZ * 1000 / (ulVBs2BsCount / 10));

        if((PDATA_DWORD(0) < (_PANEL_FREESYNC_MIN_FRAME_RATE - _FRAME_SYNC_MARGIN)) || (PDATA_DWORD(0) > (_PANEL_FREESYNC_MAX_FRAME_RATE + _FRAME_SYNC_MARGIN)))
        {
            return _FALSE;
        }

        // HBs2Bs count in Link Clk / 2
        usHBs2BsCount = (((DWORD)ScalerGetByte(PB8_5B_MN_MEAS_HLN_M)) << 8) | ((DWORD)ScalerGetByte(PB8_5C_MN_MEAS_HLN_L));

        // VTotal in line, Link Clk / 2 : (Link Rate * 27 / 2)
        usDpVtotal = ulVBs2BsCount * (ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) * 27 / 2) / usHBs2BsCount * 1000 / _GDI_CLK_KHZ;

        ulDpMvidValue = ((((DWORD)ScalerGetByte(PB9_1A_MSA_MVID_0)) << 16) | (((DWORD)ScalerGetByte(PB9_1B_MSA_MVID_1)) << 8) | ((DWORD)ScalerGetByte(PB9_1C_MSA_MVID_2)));

        ulDpNvidValue = ((((DWORD)ScalerGetByte(PB9_1D_MSA_NVID_0)) << 16) | (((DWORD)ScalerGetByte(PB9_1E_MSA_NVID_1)) << 8) | ((DWORD)ScalerGetByte(PB9_1F_MSA_NVID_2)));

        // [5] ln_ck_sel = 1'b1 -> CDR Clock
        // [3:0] DP_XTAL_CYCLE = 4'b0100 -> XTAL Counter Cycles = 2000
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~(_BIT5 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT2));

        // [1:0] freqdet_lane_sel = 2'b00 -> Lane0
        ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), _D1_DP_LANE0);

        // [6] LANE_COUNT_CLEAR = 0 -> Keep the LANE_COUNT value
        ScalerSetBit(PB0_52_DP_SIG_DET_2, ~_BIT6, 0x00);

        // [7] DP_SIG_DET_EN = 1'b1 -> Enable Signal Detection
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, _BIT7);

        // Waiting for Measure Done
        ScalerTimerDelayXms(2);

        PDATA_WORD(0) = ((((WORD)ScalerGetByte(PB0_53_DP_SIG_DET_3)) << 8) | ScalerGetByte(PB0_54_DP_SIG_DET_4));

        // [7] DP_SIG_DET_EN = 0 -> Disable Signal Detection
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);

        PDATA_DWORD(0) = ((DWORD)PDATA_WORD(0)) * _GDI_CLK_KHZ;

        // Calculate streamclk
        ulStreamClkValue = ((DWORD)ulDpMvidValue * (PDATA_DWORD(0) / ulDpNvidValue) + (PDATA_DWORD(0) % ulDpNvidValue * 100 / ulDpNvidValue * (DWORD)ulDpMvidValue / 100));
    }
    else
#endif
    {
        usDpVtotal = ((((WORD)ScalerGetByte(PB9_10_MSA_VTTE_0)) << 8) | ((WORD)(ScalerGetByte(PB9_11_MSA_VTTE_1))));

        // Calculate streamclk (Unit is Hz)
        ulStreamClkValue = (((((((DWORD)ScalerGetByte(PB9_08_MSA_HTT_0)) << 8) | ((WORD)(ScalerGetByte(PB9_09_MSA_HTT_1)))) * ((DWORD)usDpVtotal) * 100) / ulVBs2BsCount) * _GDI_CLK_KHZ * 10);
    }

    // Input Pixel CLK Unit is 0.1 M
    SET_D1_INPUT_PIXEL_CLK(ulStreamClkValue / 100000);

    // Disable Generate Display Format
    ScalerSetBit(PB8_30_DPF_CTRL_0, ~_BIT7, 0x00);

    // DP Display Format Generator Reset
    ScalerSetBit(PB8_00_MAC_DIG_RESET_CTRL, ~_BIT7, _BIT7);
    ScalerSetBit(PB8_00_MAC_DIG_RESET_CTRL, ~_BIT7, 0x00);

    // Set Vactive
    ScalerRead(PB9_16_MSA_VHT_0, 2, &pData[4], _AUTOINC);
    ScalerSetByte(PB8_4D_MN_DPF_VHT_M, pData[4]);
    ScalerSetByte(PB8_4E_MN_DPF_VHT_L, pData[5]);

    // Set Hactive
    ScalerRead(PB9_0C_MSA_HWD_0, 2, pData, _AUTOINC);
    ScalerSetByte(PB8_45_MN_DPF_HWD_M, pData[0]);
    ScalerSetByte(PB8_46_MN_DPF_HWD_L, pData[1]);
    usDpHactive = PDATA_WORD(0);

#if(_DP_FREESYNC_SUPPORT == _ON)
    if(GET_FREESYNC_ENABLED() == _TRUE)
    {
        // HTotal in Pixel Clk, Link Clk / 2 : (Link Rate * 27 / 2)
        WORD usDpHtotal = (DWORD)usHBs2BsCount * (GET_D1_INPUT_PIXEL_CLK() / 10) / (ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) * 27 / 2);

        // Set Htotal
        ScalerSetByte(PB8_41_MN_DPF_HTT_M, HIBYTE(usDpHtotal));
        ScalerSetByte(PB8_42_MN_DPF_HTT_L, LOBYTE(usDpHtotal));

        // Set Hstart
        ScalerSetByte(PB8_43_MN_DPF_HST_M, HIBYTE(_DP_DE_ONLY_MODE_HSTART));
        ScalerSetByte(PB8_44_MN_DPF_HST_L, LOBYTE(_DP_DE_ONLY_MODE_HSTART));

        // Set HSW
        ScalerSetByte(PB8_47_MN_DPF_HSW_M, HIBYTE(_DP_DE_ONLY_MODE_HSW));
        ScalerSetByte(PB8_48_MN_DPF_HSW_L, LOBYTE(_DP_DE_ONLY_MODE_HSW));

        // Set Vstart
        ScalerSetByte(PB8_4B_MN_DPF_VST_M, HIBYTE(_DP_DE_ONLY_MODE_VSTART));
        ScalerSetByte(PB8_4C_MN_DPF_VST_L, LOBYTE(_DP_DE_ONLY_MODE_VSTART));

        // Set VSW; Set VSP => Positive
        ScalerSetByte(PB8_4F_MN_DPF_VSW_M, (HIBYTE(_DP_DE_ONLY_MODE_VSW) & ~_BIT7));
        ScalerSetByte(PB8_50_MN_DPF_VSW_L, LOBYTE(_DP_DE_ONLY_MODE_VSW));

        PDATA_WORD(0) = ScalerDpRx1GetBStoHSDelay(usDpHtotal);

        // Set HBs2Bs
        ScalerSetByte(PB8_38_BS2HS_0, pData[0]);
        ScalerSetByte(PB8_39_BS2HS_1, pData[1]);

        ScalerDpRx1InterlaceCheck(&usDpVtotal, &ucOddFlg);

        // Set DE Only Mode Vsync Front Porch
        ScalerSetByte(PB8_40_VS_FRONT_PORCH, GET_DP_RX1_VFRONT_PORCH());

        if(ScalerDpRx1GetAmdSpdInfo(_SPD_INFO_FREESYNC_SUPPORT) == _ENABLE)
        {
            // Get DP DRR SPD Vmax
            g_usFREESYNCVFreq = (WORD)ScalerDpRx1GetAmdSpdInfo(_SPD_INFO_FREESYNC_MAX_VFREQ) * 10;
        }
        else
        {
            g_usFREESYNCVFreq = _PANEL_FREESYNC_MAX_FRAME_RATE;
            DebugMessageRx1("DP DRR without SPD info", 0x00);
        }

        // Set DE Only Mode
        ScalerSetBit(PB8_30_DPF_CTRL_0, ~(_BIT7 | _BIT5 | _BIT4), _BIT4);

        if(ScalerGlobalGetIDCode() <= _RL6193_VER_C_ID_CODE)
        {
            // Choose Frame start BE to Reset FIFO
            ScalerSetBit(PB8_20_PG_CTRL_0, ~_BIT5, _BIT5);
        }
        else
        {
            // Choose Frame start BE to Reset FIFO
            ScalerSetBit(PB8_20_PG_CTRL_0, ~_BIT5, 0x00);
        }

        // Set DP FREESYNC Mode Enable
        ScalerSetBit(PB8_F0_DP_RSV0, ~_BIT0, _BIT0);

        DebugMessageRx1("FREESYNC Mode Enabled!!", ScalerGetByte(PB8_30_DPF_CTRL_0));
    }
    else
#endif
    {
        DWORD ulBs2VsDelay = 0;

        // Set Htotal
        ScalerRead(PB9_08_MSA_HTT_0, 2, &pData[2], _AUTOINC);
        ScalerSetByte(PB8_41_MN_DPF_HTT_M, pData[2]);
        ScalerSetByte(PB8_42_MN_DPF_HTT_L, pData[3]);

        // Get HSW
        pData[4] = ScalerGetBit(PB9_0E_MSA_HSW_0, (_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0));
        pData[5] = ScalerGetByte(PB9_0F_MSA_HSW_1);

        // Get HSW min by Measure clock
        PDATA_WORD(3) = ((DWORD) _DP_HSYNC_WIDTH_MEASURE_COUNTER * GET_D1_INPUT_PIXEL_CLK() * 100) / _MEASURE_CLK + 1;

        // Check HSW <= HSW min
        if(PDATA_WORD(2) <= PDATA_WORD(3))
        {
            // Check if H porch is more than HSW min
            if((PDATA_WORD(1) - usDpHactive) >= PDATA_WORD(3))
            {
                // HSW = HSW min
                PDATA_WORD(2) = PDATA_WORD(3);
            }
            else
            {
                // HSW = Max H Porch
                PDATA_WORD(2) = (PDATA_WORD(1) - usDpHactive);
            }
        }

        // DPF H Front Porch must not be less than 8 (HW constraint)
        // Check if H porch is more than 8
        if((PDATA_WORD(1) - usDpHactive - PDATA_WORD(2)) <= _DP_HSYNC_FRONT_PORCH)
        {
            // HSW < HSW min
            if(PDATA_WORD(2) < PDATA_WORD(3))
            {
                // Hstart = HSW
                PDATA_WORD(4) = PDATA_WORD(2);
            }
            else if((PDATA_WORD(1) - usDpHactive - PDATA_WORD(3) >= _DP_HSYNC_FRONT_PORCH))
            {
                // HSW = H porch - _DP_HSYNC_FRONT_PORCH
                PDATA_WORD(2) = PDATA_WORD(1) - usDpHactive - _DP_HSYNC_FRONT_PORCH;

                // Hstart = HSW
                PDATA_WORD(4) = PDATA_WORD(2);
            }
            else
            {
                // HSW = HSW min
                PDATA_WORD(2) = PDATA_WORD(3);

                // Hstart = HSW min
                PDATA_WORD(4) = PDATA_WORD(3);
            }
        }
        else
        {
            // Hstart = Htotal - Hactive - _DP_HSYNC_FRONT_PORCH
            PDATA_WORD(4) = PDATA_WORD(1) - usDpHactive - _DP_HSYNC_FRONT_PORCH;
        }

        // Set HSW and HSP => positive
        ScalerSetByte(PB8_47_MN_DPF_HSW_M, (pData[4] & ~_BIT7));
        ScalerSetByte(PB8_48_MN_DPF_HSW_L, pData[5]);

        // Set Hstart
        ScalerSetByte(PB8_43_MN_DPF_HST_M, pData[8]);
        ScalerSetByte(PB8_44_MN_DPF_HST_L, pData[9]);

        DebugMessageRx1("###Origin usVTotal###", usDpVtotal);

        if(ScalerDpRx1InterlaceCheck(&usDpVtotal, &ucOddFlg) == _TRUE) // Interlace
        {
            DebugMessageRx1("7. IIIIIInterLace Vtotal", usDpVtotal);
        }
        else // Progressive
        {
            DebugMessageRx1("7. PPPPProgressive", 0);
        }

        // Set Vtotal
        ScalerSetByte(PB8_49_MN_DPF_VTT_M, (usDpVtotal >> 8));
        ScalerSetByte(PB8_4A_MN_DPF_VTT_L, (usDpVtotal & 0xFF));

        // Get Vactive
        ScalerRead(PB9_16_MSA_VHT_0, 2, &pData[4], _AUTOINC);

        // V Front porch must not be less than 2 because of V scaling down.
        // Set Vstart -> (Vtotal - Vactive - Frontporch)
        PDATA_WORD(0) = usDpVtotal - PDATA_WORD(2) - GET_DP_RX1_VFRONT_PORCH();

        // Set Vstart
        ScalerSetByte(PB8_4B_MN_DPF_VST_M, pData[0]);
        ScalerSetByte(PB8_4C_MN_DPF_VST_L, pData[1]);

        // Set VSW; Set VSP => Positive
        ScalerSetByte(PB8_4F_MN_DPF_VSW_M, (HIBYTE(_DP_VSYNC_WIDTH) & ~_BIT7));
        ScalerSetByte(PB8_50_MN_DPF_VSW_L, LOBYTE(_DP_VSYNC_WIDTH));

        ulBs2VsDelay = ScalerDpRx1GetBStoVSDelay();

        // Set Odd Field BS to VS Delay
        ScalerSetByte(PB8_35_OVBLK2VS_H, (BYTE)(ulBs2VsDelay >> 16));
        ScalerSetByte(PB8_36_OVBLK2VS_M, (BYTE)(ulBs2VsDelay >> 8));
        ScalerSetByte(PB8_37_OVBLK2VS_L, (BYTE)(ulBs2VsDelay >> 0));

        if((bit)ucOddFlg == _TRUE)
        {
            // Get Htotal
            ScalerRead(PB9_08_MSA_HTT_0, 2, &pData[10], _AUTOINC);

            // BStoVSDelay = BStoVSDelay + (Link Clk * 256 / Stream Clk) * ((DWORD)Htotal) / 256;
            ulBs2VsDelay += ((((DWORD)ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00)) * 270UL) * 256) / ((DWORD)GET_D1_INPUT_PIXEL_CLK()) * ((DWORD)PDATA_WORD(5)) / 256 / 2;
        }

        // Set Even Field BS to VS Delay
        ScalerSetByte(PB8_32_EVBLK2VS_H, (BYTE)(ulBs2VsDelay >> 16));
        ScalerSetByte(PB8_33_EVBLK2VS_M, (BYTE)(ulBs2VsDelay >> 8));
        ScalerSetByte(PB8_34_EVBLK2VS_L, (BYTE)(ulBs2VsDelay >> 0));

        // Choose VS Rising to Reset FIFO
        ScalerSetBit(PB8_20_PG_CTRL_0, ~_BIT5, 0x00);

        // Set DP FREESYNC Mode Disable
        ScalerSetBit(PB8_F0_DP_RSV0, ~_BIT0, 0x00);

        if((ScalerGetBit(PB8_57_INTERLACE_MODE_CONFIG, _BIT7) == _BIT7) && (ScalerGetBit(PB9_01_DP_VBID, _BIT2) == _BIT2) && (ScalerGetBit(PB8_57_INTERLACE_MODE_CONFIG, _BIT0) != ScalerGetBit(PB9_03_MN_STRM_ATTR_MISC1, _BIT0)))
        {
            // set frame sync mode, For VG870 1075:1080i50Hz timing 8bit -> 10bit issue
            ScalerSetBit(PB8_30_DPF_CTRL_0, ~(_BIT7 | _BIT5 | _BIT4), _BIT5);
        }
        else
        {
            // Set full last line Mode
            ScalerSetBit(PB8_30_DPF_CTRL_0, ~(_BIT7 | _BIT5 | _BIT4), 0x00);
        }

        DebugMessageRx1("FREESYNC Mode Disabled", ScalerGetByte(PB8_30_DPF_CTRL_0));
    }

    // Double Buffer to Apply Display Format Setting
    ScalerSetBit(PB8_30_DPF_CTRL_0, ~_BIT6, _BIT6);

    ScalerDpRx1VideoSetting();

    return _TRUE;
}

//--------------------------------------------------
// Description  : DP Video Fifo Check
// Input Value  : Fifo Check Condition
// Output Value : True --> Fifo not over/underflow
//--------------------------------------------------
bit ScalerDpRx1FifoCheck(EnumDpFifoCheckCondition enumFifoCheckCondition)
{
#if(_DP_FREESYNC_SUPPORT == _ON)
    if((GET_FREESYNC_ENABLED() == _TRUE) && (ScalerGlobalGetIDCode() <= _RL6193_VER_C_ID_CODE))
    {
        return _TRUE;
    }
#endif

    if(enumFifoCheckCondition == _DP_FIFO_DELAY_CHECK)
    {
        ScalerSetBit(PB8_21_PG_CTRL_1, ~(_BIT4 | _BIT2 | _BIT0), (_BIT4 | _BIT2 | _BIT0));

        ScalerTimerDelayXms(_DP_TWO_FRAME_TIME_MAX);

        pData[0] = 0;

        while(pData[0] < 6)
        {
            ScalerTimerDelayXms(10);

            if((ScalerGetBit(PB8_21_PG_CTRL_1, _BIT4) == _BIT4) || (ScalerGetBit(PB8_21_PG_CTRL_1, _BIT2) == _BIT2))
            {
                return _FALSE;
            }

            pData[0]++;
        }
    }
    else
    {
        if((ScalerGetBit(PB8_21_PG_CTRL_1, _BIT4) == _BIT4) || (ScalerGetBit(PB8_21_PG_CTRL_1, _BIT2) == _BIT2))
        {
            return _FALSE;
        }
    }

    return _TRUE;
}

//--------------------------------------------------
// Description  : Enable DP Fifo Under/OverFlow Watch Dog
// Input Value  : _ENABLE/ DISABLE
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1SetFifoWD(bit bEn)
{
#if(_DP_FREESYNC_SUPPORT == _ON)
    if((GET_FREESYNC_ENABLED() == _TRUE) && (ScalerGlobalGetIDCode() <= _RL6193_VER_C_ID_CODE))
    {
        bEn = _DISABLE;
    }
#endif

    ScalerSetBit(PB9_38_DP_VWD_CTRL, ~(_BIT5 | _BIT4), ((bEn == _ENABLE) ? (_BIT5 | _BIT4) : 0x00));
}

//--------------------------------------------------
// Description  : Enable DP Fifo Under/OverFlow IRQ
// Input Value  : _ENABLE/ DISABLE
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1SetFifoIRQ(bit bEn)
{
    if(ScalerGlobalGetIDCode() <= _RL6193_VER_C_ID_CODE)
    {
        bEn = _DISABLE;
    }

    ScalerSetBit(PB9_3A_DP_IRQ_CTRL0, ~(_BIT1 | _BIT0), ((bEn == _ENABLE) ? (_BIT1 | _BIT0) : 0x00));
}

//--------------------------------------------------
// Description  : DP CDR N.F. Code Setting
// Input Value  : Link Rate and N.F. Code Clock Ref.
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1CMUNFCodeSetting(EnumDpNFCodeRef enumDpNFCodeRef, EnumDpLinkRate enumLinkRate)
{
    WORD usCMUNCode = 0;
    WORD usCMUFCode = 0;
    WORD usCount = 0;
    DWORD ulFreqDetect = 0;

    if(enumDpNFCodeRef == _DP_NF_REF_XTAL)
    {
        if(enumLinkRate == _DP_LINK_RBR)
        {
            usCMUNCode = 109;
            usCMUFCode = 591;
        }
        else  // HBR HBR2
        {
            usCMUNCode = 184;
            usCMUFCode = 2350;
        }
    }
    else
    {
        switch(g_ucDpRx1PhyCtsCtrl & 0x30)
        {
            case 0x10: // Lane1 Test

                // [1:0] FREQDET_LANE_SEL = 2'b01 -> Select Lane1
                ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), _D1_DP_LANE1);

                break;

            case 0x20: // Lane2 Test

                // [1:0] FREQDET_LANE_SEL = 2'b10 -> Select Lane2
                ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), _D1_DP_LANE2);

                break;

            case 0x30: // Lane3 Test

                // [1:0] FREQDET_LANE_SEL = 2'b11 -> Select Lane3
                ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), _D1_DP_LANE3);

                break;

            default:
            case 0x00: // Lane0 Test

                // [1:0] FREQDET_LANE_SEL = 2'b00 -> Select Lane0
                ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), _D1_DP_LANE0);

                break;
        }

        // [5] ln_ck_sel = 1'b0 -> DP Raw Data
        // [3:0] DP_XTAL_CYCLE = 4'b0011 -> XTAL Counter Cycles = 1000
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~(_BIT5 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));

        // [6] LANE_COUNT_CLEAR = 0 -> Keep the LANE_COUNT value
        ScalerSetBit(PB0_52_DP_SIG_DET_2, ~_BIT6, 0x00);

        // [7] DP_SIG_DET_EN = 1'b1 -> Enable Signal Detection
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, _BIT7);

        // Waiting for Measure Done -> 50us
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        switch(enumLinkRate)
        {
            case _DP_LINK_HBR2:

                if(ScalerGetBit(PB0_50_DP_SIG_DET_0, _BIT6) == _BIT6)
                {
                    usCount = ((((WORD)ScalerGetByte(PB0_53_DP_SIG_DET_3)) << 8) | ScalerGetByte(PB0_54_DP_SIG_DET_4));

                    ulFreqDetect = (((DWORD)usCount * 2700) / 10006) * 100000;

                    usCMUNCode = ((ulFreqDetect / 14318) - 400) / 100;
                    usCMUFCode = (((ulFreqDetect / 14318) - 400) - (usCMUNCode * 100)) * 4096 / 100;
                }
                else
                {
                    usCMUNCode = 184;
                    usCMUFCode = 2350;
                }

                break;

            case _DP_LINK_HBR:

                if(ScalerGetBit(PB0_50_DP_SIG_DET_0, _BIT6) == _BIT6)
                {
                    usCount = ((((WORD)ScalerGetByte(PB0_53_DP_SIG_DET_3)) << 8) | ScalerGetByte(PB0_54_DP_SIG_DET_4));

                    ulFreqDetect = (((DWORD)usCount * 2700) / 5003) * 100000;

                    usCMUNCode = ((ulFreqDetect / 14318) - 400) / 100;
                    usCMUFCode = (((ulFreqDetect / 14318) - 400) - (usCMUNCode * 100)) * 4096 / 100;
                }
                else
                {
                    usCMUNCode = 184;
                    usCMUFCode = 2350;
                }

                break;

            case _DP_LINK_RBR:
            default:

                if(ScalerGetBit(PB0_50_DP_SIG_DET_0, _BIT6) == _BIT6)
                {
                    usCount = ((((WORD)ScalerGetByte(PB0_53_DP_SIG_DET_3)) << 8) | ScalerGetByte(PB0_54_DP_SIG_DET_4));

                    ulFreqDetect = (((DWORD)usCount * 1620) / 3002) * 100000;

                    usCMUNCode = ((ulFreqDetect / 14318) - 400) / 100;

                    usCMUFCode = (((ulFreqDetect / 14318) - 400) - (usCMUNCode * 100)) * 4096 / 100;
                }
                else
                {
                    usCMUNCode = 109;
                    usCMUFCode = 591;
                }

                break;
        }
    }

    // [7] DP_SIG_DET_EN = 0 -> Disable Signal Detection
    ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);

    // [5] N_CODE[8]
    ScalerSetBit(PB0_F1_SDM_00, ~_BIT5, ((usCMUNCode >> 3) & _BIT5));

    // [7:0] N_CODE[7:0]
    ScalerSetByte(PB0_F2_SDM_01, (BYTE)usCMUNCode);

    // [4:1] F_CODE[11:8]
    ScalerSetBit(PB0_F1_SDM_00, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1), ((usCMUFCode >> 7) & (_BIT4 | _BIT3 | _BIT2 | _BIT1)));

    // [7:0] F_CODE[7:0]
    ScalerSetByte(PB0_F3_SDM_02, (BYTE)usCMUFCode);
}

//--------------------------------------------------
// Description  : DP PHY CTS Process
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1PhyCTS(void)
{
    WORD usTemp = 0;

    // g_ucDpRx1PhyCtsCtrl(Page0xB0 A7[0] = 0 --> PHY CTS Enable Start
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[2:0]) = 0x01 --> CDR
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[2:0]) = 0x02 --> EQ CRC
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[2:0]) = 0x03 --> PRBS7 Error Counter Reset and Counter Start
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[2:0]) = 0x04 --> PRBS7 Error Counter Read
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[2:0]) = 0x05 --> PHY CTS Disable
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[5:4]) = 00 --> lane0
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[5:4]) = 01 --> lane1
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[5:4]) = 10 --> lane2
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[5:4]) = 11 --> lane3
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[7:6]) = 00 --> RBR
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[7:6]) = 01 --> HBR
    // g_ucDpRx1PhyCtsCtrl(Page0xBA A5[7:6]) = 10 --> HBR2
    g_ucDpRx1PhyCtsCtrl = 0x00;

    EX0 = 0;

    while(GET_DP_RX1_PHY_CTS_FLG() == _TRUE)
    {
        ScalerDebug();

        ScalerRead(PBA_A5_AUX_RESERVE5, 1, &g_ucDpRx1PhyCtsCtrl, _NON_AUTOINC);

        DebugMessageRx1("7. PHY CTS Loop", g_ucDpRx1PhyCtsCtrl);

        switch(g_ucDpRx1PhyCtsCtrl & 0x07)
        {
            case 0x01: // CDR Lock Check

                ScalerSetByte(PBA_A5_AUX_RESERVE5, (g_ucDpRx1PhyCtsCtrl & ~(_BIT2 | _BIT1 | _BIT0))); // Clear DP PHY CTS Phase Info

                g_ucDpRx1Temp = 50;

                while(g_ucDpRx1Temp > 0)
                {
                    if((g_ucDpRx1PhyCtsCtrl & 0xB0) == 0x80)   // Only for lane0 HBR2
                    {
                        ScalerDpRx1PhyCtsACDR();
                    }
                    else
                    {
                        ScalerDpRx1PhyCtsTp1SetPhy(_DP_NF_REF_XTAL);
                    }

                    if(ScalerDpRx1PhyCtsTp1Check() == _TRUE)
                    {
                        ScalerSetByte(PBA_A4_AUX_RESERVE4, 0x01);
                        g_ucDpRx1Temp = 0;
                    }
                    else
                    {
                        ScalerSetByte(PBA_A4_AUX_RESERVE4, 0xFF);
                        g_ucDpRx1Temp--;
                    }
                }

                break;

            case 0x02: // Symbol Lock Check

                ScalerSetByte(PBA_A5_AUX_RESERVE5, (g_ucDpRx1PhyCtsCtrl & ~(_BIT2 | _BIT1 | _BIT0)));

                g_ucDpRx1Temp = 50;

                while(g_ucDpRx1Temp > 0)
                {
                    if((g_ucDpRx1PhyCtsCtrl & 0xB0) == 0x80)   // Only for lane0 HBR2
                    {
                        ScalerDpRx1PhyCtsACDR();
                    }
                    else
                    {
                        ScalerDpRx1PhyCtsTp2SetPhy();
                    }

                    if(ScalerDpRx1PhyCtsTp2Check() == _TRUE)
                    {
                        ScalerSetByte(PBA_A4_AUX_RESERVE4, 0x01);
                        g_ucDpRx1Temp = 0;
                    }
                    else
                    {
                        ScalerSetByte(PBA_A4_AUX_RESERVE4, 0xFF);
                        g_ucDpRx1Temp--;
                    }
                }

                break;

            case 0x03:

                ScalerSetByte(PBA_A5_AUX_RESERVE5, (g_ucDpRx1PhyCtsCtrl & ~(_BIT2 | _BIT1 | _BIT0)));

                switch(g_ucDpRx1PhyCtsCtrl & 0x30)
                {
                    case 0x00: // Lane0 Test

                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT4 | _BIT3), 0x00);

                        break;

                    case 0x10: // Lane1 Test

                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT4 | _BIT3), _BIT3);

                        break;

                    case 0x20: // Lane2 Test

                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT4 | _BIT3), _BIT4);

                        break;

                    case 0x30: // Lane3 Test

                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));

                        break;

                    default:
                        break;
                }

                // Disable Scramble
                ScalerSetBit(PB0_07_SCRAMBLE_CTRL, ~_BIT5, _BIT5);

                // Reverse PRBS7 Pattern Gen
                ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~_BIT6, _BIT6);

                // Disable Error Detection
                ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                if((g_ucDpRx1PhyCtsCtrl & 0x80) == 0x80)
                {
                    if((g_ucDpRx1PhyCtsCtrl & 0x30) == 0x00)   // Only for lane0 HBR2
                    {
                        // SetPhy for HBR2 Lane0 ACDR
                        ScalerDpRx1PhyCtsACDR();

                        // Waiting for PHY Stable
                        ScalerTimerDelayXms(10);

                        // HBR2 Compliance Eye Pattern
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                        // Waiting for PHY Stable
                        ScalerTimerDelayXms(10);

                        if(((((WORD)ScalerGetByte(PB0_0B_BIST_PATTERN3)) << 8) | ScalerGetByte(PB0_0C_BIST_PATTERN4)) < 1000)
                        {
                            DebugMessageRx1("CDR No Reset", 0x00);
                        }
                        else // Reset CDR to Re-allocate 10bit Raw Data
                        {
                            // [4] Disable Comma Detection
                            ScalerSetBit(PB0_05_SAMPLE_EDGE, ~_BIT4, _BIT4);

                            for(usTemp = 0; usTemp <= 1000; usTemp++)
                            {
                                // [6] reg_adp_en_manual = 1'b0 --> CDR Open Loop
                                // [5] reg_cp_en_manual = 1'b0 --> CDR Open Loop
                                ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~(_BIT3 | _BIT2), 0x00);

                                // Waiting for VCO Stable
                                for(pData[0] = 0; pData[0] <= 2; pData[0]++)
                                {
                                    Delay5us(g_ucDelay5usN);
                                }

                                // [6] reg_adp_en_manual = 1'b1 --> CDR Close Loop
                                // [5] reg_cp_en_manual = 1'b1 --> CDR Close Loop
                                ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~(_BIT3 | _BIT2), (_BIT3 | _BIT2));

                                ScalerTimerDelayXms(2);

                                // Disable Error Detection
                                ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                                // HBR2 Compliance Eye Pattern
                                ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                                ScalerTimerDelayXms(2);

                                DebugMessageRx1("CDR Reset", usTemp);

                                if(((((WORD)ScalerGetByte(PB0_0B_BIST_PATTERN3)) << 8) | ScalerGetByte(PB0_0C_BIST_PATTERN4)) < 1000)
                                {
                                    break;
                                }
                            }

                            DebugMessageRx1("CDR Reset Done", 0xFF);
                        }
                    }
                    else
                    {
                        // SetPhy for HBR2
                        ScalerDpRx1PhyCtsTp2SetPhy();

                        // KI = 3'b011 --> KI = 1/4
                        ScalerSetByte(PB0_D4_KI_0, 0x6C);
                        ScalerSetByte(PB0_D5_KI_1, 0x6C);

                        // KP = 0x07 --> KP = 1.75
                        ScalerSetByte(PB0_D6_KP_0, 0x07);
                        ScalerSetByte(PB0_D7_KP_1, 0x07);
                        ScalerSetByte(PB0_D8_KP_2, 0x07);
                        ScalerSetByte(PB0_D9_KP_3, 0x07);

                        g_ucDpRx1Temp = 50;

                        while(g_ucDpRx1Temp > 0)  // Reset CDR
                        {
                            // Reset Digital CDR
                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            // Enable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                            // Waiting 1ms for Error Counter
                            ScalerTimerDelayXms(2);

                            if(ScalerGetByte(PB0_0B_BIST_PATTERN3) < 0x7E)
                            {
                                g_ucDpRx1Temp = 0;
                            }
                            else
                            {
                                g_ucDpRx1Temp--;
                            }

                            // Disable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
                        }

                        // Enable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                        // Waiting 2s for Error Counter
                        ScalerTimerDelayXms(2000);

                        pData[1] = ScalerGetByte(PB0_0C_BIST_PATTERN4);
                        pData[0] = ScalerGetByte(PB0_0B_BIST_PATTERN3);

                        // Disable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                        // KI = 3'b100 --> KI = 1/2
                        ScalerSetByte(PB0_D4_KI_0, 0x90);
                        ScalerSetByte(PB0_D5_KI_1, 0x90);

                        // KP = 0x07 --> KP = 1.75
                        ScalerSetByte(PB0_D6_KP_0, 0x07);
                        ScalerSetByte(PB0_D7_KP_1, 0x07);
                        ScalerSetByte(PB0_D8_KP_2, 0x07);
                        ScalerSetByte(PB0_D9_KP_3, 0x07);

                        g_ucDpRx1Temp = 50;

                        while(g_ucDpRx1Temp > 0)  // Reset CDR
                        {
                            // Reset Digital CDR
                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            // Enable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                            // Waiting 1ms for Error Counter
                            ScalerTimerDelayXms(2);

                            if(ScalerGetByte(PB0_0B_BIST_PATTERN3) < 0x7E)
                            {
                                g_ucDpRx1Temp = 0;
                            }
                            else
                            {
                                g_ucDpRx1Temp--;
                            }

                            // Disable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
                        }

                        // Enable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                        // Waiting 2s for Error Counter
                        ScalerTimerDelayXms(2000);

                        pData[3] = ScalerGetByte(PB0_0C_BIST_PATTERN4);
                        pData[2] = ScalerGetByte(PB0_0B_BIST_PATTERN3);

                        // Disable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                        // KI = 3'b011 --> KI = 1/4
                        ScalerSetByte(PB0_D4_KI_0, 0x6C);
                        ScalerSetByte(PB0_D5_KI_1, 0x6C);

                        // KP = 0x03 --> KP = 3.75
                        ScalerSetByte(PB0_D6_KP_0, 0x0F);
                        ScalerSetByte(PB0_D7_KP_1, 0x0F);
                        ScalerSetByte(PB0_D8_KP_2, 0x0F);
                        ScalerSetByte(PB0_D9_KP_3, 0x0F);

                        g_ucDpRx1Temp = 50;

                        while(g_ucDpRx1Temp > 0)  // Reset CDR
                        {
                            // Reset Digital CDR
                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            // Enable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                            // Waiting 1ms for Error Counter
                            ScalerTimerDelayXms(2);

                            if(ScalerGetByte(PB0_0B_BIST_PATTERN3) < 0x7E)
                            {
                                g_ucDpRx1Temp = 0;
                            }
                            else
                            {
                                g_ucDpRx1Temp--;
                            }

                            // Disable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
                        }

                        // Enable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                        // Waiting 2s for Error Counter
                        ScalerTimerDelayXms(2000);

                        pData[5] = ScalerGetByte(PB0_0C_BIST_PATTERN4);
                        pData[4] = ScalerGetByte(PB0_0B_BIST_PATTERN3);

                        // Disable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                        // KI = 3'b100 --> KI = 1/2
                        ScalerSetByte(PB0_D4_KI_0, 0x90);
                        ScalerSetByte(PB0_D5_KI_1, 0x90);

                        // KP = 0x0F --> KP = 3.75
                        ScalerSetByte(PB0_D6_KP_0, 0x0F);
                        ScalerSetByte(PB0_D7_KP_1, 0x0F);
                        ScalerSetByte(PB0_D8_KP_2, 0x0F);
                        ScalerSetByte(PB0_D9_KP_3, 0x0F);

                        g_ucDpRx1Temp = 50;

                        while(g_ucDpRx1Temp > 0)  // Reset CDR
                        {
                            // Reset Digital CDR
                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            // Enable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                            // Waiting 1ms for Error Counter
                            ScalerTimerDelayXms(2);

                            if(ScalerGetByte(PB0_0B_BIST_PATTERN3) < 0x7E)
                            {
                                g_ucDpRx1Temp = 0;
                            }
                            else
                            {
                                g_ucDpRx1Temp--;
                            }

                            // Disable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
                        }

                        // Enable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                        // Waiting 2s for Error Counter
                        ScalerTimerDelayXms(2000);

                        pData[7] = ScalerGetByte(PB0_0C_BIST_PATTERN4);
                        pData[6] = ScalerGetByte(PB0_0B_BIST_PATTERN3);

                        // Disable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                        if((PDATA_WORD(0) <= PDATA_WORD(1)) && (PDATA_WORD(0) <= PDATA_WORD(2)) && (PDATA_WORD(0) <= PDATA_WORD(3)))
                        {
                            // KI = 3'b011 --> KI = 1/4
                            ScalerSetByte(PB0_D4_KI_0, 0x6C);
                            ScalerSetByte(PB0_D5_KI_1, 0x6C);

                            // KP = 0x07 --> KP = 1.75
                            ScalerSetByte(PB0_D6_KP_0, 0x07);
                            ScalerSetByte(PB0_D7_KP_1, 0x07);
                            ScalerSetByte(PB0_D8_KP_2, 0x07);
                            ScalerSetByte(PB0_D9_KP_3, 0x07);
                        }
                        else if((PDATA_WORD(1) <= PDATA_WORD(2)) && (PDATA_WORD(1) <= PDATA_WORD(3)))
                        {
                            // KI = 3'b100 --> KI = 1/2
                            ScalerSetByte(PB0_D4_KI_0, 0x90);
                            ScalerSetByte(PB0_D5_KI_1, 0x90);

                            // KP = 0x07 --> KP = 1.75
                            ScalerSetByte(PB0_D6_KP_0, 0x07);
                            ScalerSetByte(PB0_D7_KP_1, 0x07);
                            ScalerSetByte(PB0_D8_KP_2, 0x07);
                            ScalerSetByte(PB0_D9_KP_3, 0x07);
                        }
                        else if(PDATA_WORD(2) <= PDATA_WORD(3))
                        {
                            // KI = 3'b011 --> KI = 1/4
                            ScalerSetByte(PB0_D4_KI_0, 0x6C);
                            ScalerSetByte(PB0_D5_KI_1, 0x6C);

                            // KP = 0x03 --> KP = 3.75
                            ScalerSetByte(PB0_D6_KP_0, 0x0F);
                            ScalerSetByte(PB0_D7_KP_1, 0x0F);
                            ScalerSetByte(PB0_D8_KP_2, 0x0F);
                            ScalerSetByte(PB0_D9_KP_3, 0x0F);
                        }
                        else
                        {
                            // KI = 3'b100 --> KI = 1/2
                            ScalerSetByte(PB0_D4_KI_0, 0x90);
                            ScalerSetByte(PB0_D5_KI_1, 0x90);

                            // KP = 0x0F --> KP = 3.75
                            ScalerSetByte(PB0_D6_KP_0, 0x0F);
                            ScalerSetByte(PB0_D7_KP_1, 0x0F);
                            ScalerSetByte(PB0_D8_KP_2, 0x0F);
                            ScalerSetByte(PB0_D9_KP_3, 0x0F);
                        }

                        g_ucDpRx1Temp = 50;

                        while(g_ucDpRx1Temp > 0)  // Reset CDR
                        {
                            // Reset Digital CDR
                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

                            // Waiting for CDR Stable
                            ScalerTimerDelayXms(10);

                            // Enable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                            // Waiting 1ms for Error Counter
                            ScalerTimerDelayXms(2);

                            if(ScalerGetByte(PB0_0B_BIST_PATTERN3) < 0x7E)
                            {
                                g_ucDpRx1Temp = 0;
                            }
                            else
                            {
                                g_ucDpRx1Temp--;
                            }

                            // Disable Error Counter
                            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
                        }

                        // Read Back LE
                        ScalerSetByte(P7A_F0_COEF_1, 0x09);

                        pData[1] = 0xFF;
                        pData[0] = 0xFF;

                        switch(g_ucDpRx1PhyCtsCtrl & 0x30)
                        {
                            case 0x00: // Lane0 LE

                                // LE (Lane0)
                                ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);
                                g_pucDpRx1LEMinTemp[0] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;  // The adaptive LE
                                g_pucDpRx1LEMaxTemp[0] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;  // The Best LE Value

                                for(g_ucDpRx1Temp = 0; g_ucDpRx1Temp < 3; g_ucDpRx1Temp++)
                                {
                                    // Reload LE (Lane0)
                                    ScalerSetBit(P7A_A2_L0_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ((g_pucDpRx1LEMinTemp[0] + 1 - g_ucDpRx1Temp) & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
                                    ScalerSetBit(P7A_A3_L0_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[0] + 1 - g_ucDpRx1Temp) << 2) & _BIT6);

                                    // Reload LE
                                    ScalerSetByte(P7A_AA_L0_INIT_8, 0x80);
                                    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);

                                    // Enable Tap0~4 Adaptation for HBR2 0x01F
                                    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x1F);

                                    // Disable Sumamp High Gain Mode
                                    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, 0x00);

                                    // [0] en_eqen_manual = 1 --> Enable Adapation
                                    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

                                    // Waiting for Adaptation
                                    for(pData[4] = 0; pData[4] <= 20; pData[4]++)
                                    {
                                        Delay5us(g_ucDelay5usN);
                                    }

                                    // Disable Adaptation
                                    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

                                    // Disable Adaptation   0x000
                                    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);

                                    // Enable Sumamp High Gain Mode
                                    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);

                                    // Reload Tap0 to 21.5
                                    ScalerSetByte(P7A_A4_L0_INIT_2, 0x15);
                                    ScalerSetByte(P7A_AA_L0_INIT_8, 0x01);
                                    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);

                                    ScalerTimerDelayXms(10);

                                    // Enable Error Counter
                                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                                    // Waiting 1s for Error Counter
                                    ScalerTimerDelayXms(1000);

                                    pData[3] = ScalerGetByte(PB0_0C_BIST_PATTERN4);
                                    pData[2] = ScalerGetByte(PB0_0B_BIST_PATTERN3);

                                    // Disable Error Counter
                                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                                    if(PDATA_WORD(1) <= PDATA_WORD(0))
                                    {
                                        // LE (Lane0)
                                        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);
                                        g_pucDpRx1LEMaxTemp[0] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;

                                        pData[1] = pData[3];
                                        pData[0] = pData[2];
                                    }
                                }

                                // Reload LE (Lane0)
                                ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);
                                ScalerSetBit(P7A_A2_L0_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMaxTemp[0] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
                                ScalerSetBit(P7A_A3_L0_INIT_1, ~_BIT6, ((g_pucDpRx1LEMaxTemp[0] << 2) & _BIT6));

                                // Reload LE
                                ScalerSetByte(P7A_AA_L0_INIT_8, 0x80);
                                ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);

                                // Enable Tap0~4 Adaptation for HBR2 0x01F
                                ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x1F);

                                break;


                            case 0x10: // Lane1 LE

                                // LE (Lane1)
                                ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);
                                g_pucDpRx1LEMinTemp[1] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;
                                g_pucDpRx1LEMaxTemp[1] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;

                                for(g_ucDpRx1Temp = 0; g_ucDpRx1Temp < 3; g_ucDpRx1Temp++)
                                {
                                    // Reload LE (Lane1)
                                    ScalerSetBit(P7A_B2_L1_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ((g_pucDpRx1LEMinTemp[1] + 1 - g_ucDpRx1Temp) & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
                                    ScalerSetBit(P7A_B3_L1_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[1] + 1 - g_ucDpRx1Temp) << 2) & _BIT6);

                                    // Reload LE
                                    ScalerSetByte(P7A_BA_L1_INIT_8, 0x80);
                                    ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);

                                    // Enable Tap0~4 Adaptation for HBR2 0x01F
                                    ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                    ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x1F);

                                    // Disable Sumamp High Gain Mode
                                    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, 0x00);

                                    // [0] en_eqen_manual = 1 --> Enable Adapation
                                    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

                                    // Waiting for Adaptation
                                    for(pData[4] = 0; pData[4] <= 20; pData[4]++)
                                    {
                                        Delay5us(g_ucDelay5usN);
                                    }

                                    // Disable Adaptation
                                    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

                                    // Disable Adaptation   0x000
                                    ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                    ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x00);

                                    // Enable Sumamp High Gain Mode
                                    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);

                                    // Reload Tap0 to 21.5
                                    ScalerSetByte(P7A_B4_L1_INIT_2, 0x15);
                                    ScalerSetByte(P7A_BA_L1_INIT_8, 0x01);
                                    ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);

                                    ScalerTimerDelayXms(10);

                                    // Enable Error Counter
                                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                                    // Waiting 1s for Error Counter
                                    ScalerTimerDelayXms(1000);

                                    pData[3] = ScalerGetByte(PB0_0C_BIST_PATTERN4);
                                    pData[2] = ScalerGetByte(PB0_0B_BIST_PATTERN3);

                                    // Disable Error Counter
                                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                                    if(PDATA_WORD(1) <= PDATA_WORD(0))
                                    {
                                        // LE (Lane1)
                                        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);
                                        g_pucDpRx1LEMaxTemp[1] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;

                                        pData[1] = pData[3];
                                        pData[0] = pData[2];
                                    }
                                }

                                // Reload LE (Lane1)
                                ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);
                                ScalerSetBit(P7A_B2_L1_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMaxTemp[1] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
                                ScalerSetBit(P7A_B3_L1_INIT_1, ~_BIT6, ((g_pucDpRx1LEMaxTemp[1] << 2) & _BIT6));

                                // Reload LE
                                ScalerSetByte(P7A_BA_L1_INIT_8, 0x80);
                                ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);

                                // Enable Tap0~4 Adaptation for HBR2 0x01F
                                ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x1F);

                                break;


                            case 0x20: // Lane2 LE

                                // LE (Lane2)
                                ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);
                                g_pucDpRx1LEMinTemp[2] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;
                                g_pucDpRx1LEMaxTemp[2] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;

                                for(g_ucDpRx1Temp = 0; g_ucDpRx1Temp < 3; g_ucDpRx1Temp++)
                                {
                                    // Reload LE (Lane2)
                                    ScalerSetBit(P7A_C2_L2_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ((g_pucDpRx1LEMinTemp[2] + 1 - g_ucDpRx1Temp) & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
                                    ScalerSetBit(P7A_C3_L2_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[2] + 1 - g_ucDpRx1Temp) << 2) & _BIT6);

                                    // Reload LE
                                    ScalerSetByte(P7A_CA_L2_INIT_8, 0x80);
                                    ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);

                                    // Enable Tap0~4 Adaptation for HBR2 0x01F
                                    ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                    ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x1F);

                                    // Disable Sumamp High Gain Mode
                                    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, 0x00);

                                    // [0] en_eqen_manual = 1 --> Enable Adapation
                                    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

                                    // Waiting for Adaptation
                                    for(pData[4] = 0; pData[4] <= 20; pData[4]++)
                                    {
                                        Delay5us(g_ucDelay5usN);
                                    }

                                    // Disable Adaptation
                                    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

                                    // Disable Adaptation   0x000
                                    ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                    ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x00);

                                    // Enable Sumamp High Gain Mode
                                    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);

                                    // Reload Tap0 to 21.5
                                    ScalerSetByte(P7A_C4_L2_INIT_2, 0x15);
                                    ScalerSetByte(P7A_CA_L2_INIT_8, 0x01);
                                    ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);

                                    ScalerTimerDelayXms(10);

                                    // Enable Error Counter
                                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                                    // Waiting 1s for Error Counter
                                    ScalerTimerDelayXms(1000);

                                    pData[3] = ScalerGetByte(PB0_0C_BIST_PATTERN4);
                                    pData[2] = ScalerGetByte(PB0_0B_BIST_PATTERN3);

                                    // Disable Error Counter
                                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                                    if(PDATA_WORD(1) <= PDATA_WORD(0))
                                    {
                                        // LE (Lane2)
                                        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);
                                        g_pucDpRx1LEMaxTemp[2] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;

                                        pData[1] = pData[3];
                                        pData[0] = pData[2];
                                    }
                                }

                                // Reload LE (Lane2)
                                ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);
                                ScalerSetBit(P7A_C2_L2_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMaxTemp[2] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
                                ScalerSetBit(P7A_C3_L2_INIT_1, ~_BIT6, ((g_pucDpRx1LEMaxTemp[2] << 2) & _BIT6));

                                // Reload LE
                                ScalerSetByte(P7A_CA_L2_INIT_8, 0x80);
                                ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);

                                // Enable Tap0~4 Adaptation for HBR2 0x01F
                                ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x1F);

                                break;


                            case 0x30: // Lane3 LE

                                // LE (Lane3)
                                ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
                                g_pucDpRx1LEMinTemp[3] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;
                                g_pucDpRx1LEMaxTemp[3] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;

                                for(g_ucDpRx1Temp = 0; g_ucDpRx1Temp < 3; g_ucDpRx1Temp++)
                                {
                                    // Reload LE (Lane3)
                                    ScalerSetBit(P7A_D2_L3_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), ((g_pucDpRx1LEMinTemp[3] + 1 - g_ucDpRx1Temp) & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
                                    ScalerSetBit(P7A_D3_L3_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[3] + 1 - g_ucDpRx1Temp) << 2) & _BIT6);

                                    // Reload LE
                                    ScalerSetByte(P7A_DA_L3_INIT_8, 0x80);
                                    ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

                                    // Enable Tap0~4 Adaptation for HBR2 0x01F
                                    ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                    ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x1F);

                                    // Disable Sumamp High Gain Mode
                                    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, 0x00);

                                    // [0] en_eqen_manual = 1 --> Enable Adapation
                                    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

                                    // Waiting for Adaptation
                                    for(pData[4] = 0; pData[4] <= 20; pData[4]++)
                                    {
                                        Delay5us(g_ucDelay5usN);
                                    }

                                    // Disable Adaptation
                                    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

                                    // Disable Adaptation   0x000
                                    ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                    ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x00);

                                    // Enable Sumamp High Gain Mode
                                    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);

                                    // Reload Tap0 to 21.5
                                    ScalerSetByte(P7A_D4_L3_INIT_2, 0x15);
                                    ScalerSetByte(P7A_DA_L3_INIT_8, 0x01);
                                    ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

                                    ScalerTimerDelayXms(10);

                                    // Enable Error Counter
                                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                                    // Waiting 1s for Error Counter
                                    ScalerTimerDelayXms(1000);

                                    pData[3] = ScalerGetByte(PB0_0C_BIST_PATTERN4);
                                    pData[2] = ScalerGetByte(PB0_0B_BIST_PATTERN3);

                                    // Disable Error Counter
                                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                                    if(PDATA_WORD(1) <= PDATA_WORD(0))
                                    {
                                        // LE (Lane3)
                                        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
                                        g_pucDpRx1LEMaxTemp[3] = ScalerGetByte(P7A_F1_COEF_2) & 0x1F;

                                        pData[1] = pData[3];
                                        pData[0] = pData[2];
                                    }
                                }

                                // Reload LE (Lane3)
                                ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
                                ScalerSetBit(P7A_D2_L3_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMaxTemp[3] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
                                ScalerSetBit(P7A_D3_L3_INIT_1, ~_BIT6, ((g_pucDpRx1LEMaxTemp[3] << 2) & _BIT6));

                                // Reload LE
                                ScalerSetByte(P7A_DA_L3_INIT_8, 0x80);
                                ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

                                // Enable Tap0~4 Adaptation for HBR2 0x01F
                                ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                                ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x1F);

                                break;

                            default:
                                break;
                        }

                        // Disable Sumamp High Gain Mode
                        ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, 0x00);

                        // [0] en_eqen_manual = 1 --> Enable Adapation
                        ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

                        // Waiting for Adaptation
                        for(pData[0] = 0; pData[0] <= 20; pData[0]++)
                        {
                            Delay5us(g_ucDelay5usN);
                        }

                        // Disable Adaptation
                        ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

                        // Disable Adaptation   0x000
                        ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                        ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                        ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                        ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
                        ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);
                        ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x00);
                        ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x00);
                        ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x00);

                        // Enable Sumamp High Gain Mode
                        ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);

                        // Reload Tap0 to 21.5
                        ScalerSetByte(P7A_A4_L0_INIT_2, 0x15);
                        ScalerSetByte(P7A_B4_L1_INIT_2, 0x15);
                        ScalerSetByte(P7A_C4_L2_INIT_2, 0x15);
                        ScalerSetByte(P7A_D4_L3_INIT_2, 0x15);
                        ScalerSetByte(P7A_AA_L0_INIT_8, 0x01);
                        ScalerSetByte(P7A_BA_L1_INIT_8, 0x01);
                        ScalerSetByte(P7A_CA_L2_INIT_8, 0x01);
                        ScalerSetByte(P7A_DA_L3_INIT_8, 0x01);
                        ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
                        ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
                        ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
                        ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

                        // Waiting for PHY Stable
                        ScalerTimerDelayXms(10);
                    }

                    g_ucDpRx1Temp = 50;

                    while(g_ucDpRx1Temp > 0)  // Reset CDR
                    {
                        // Reset Digital CDR
                        ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);

                        // Waiting for CDR Stable
                        ScalerTimerDelayXms(10);

                        ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

                        // Waiting for CDR Stable
                        ScalerTimerDelayXms(10);

                        // Enable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);

                        // Waiting 1ms for Error Counter
                        ScalerTimerDelayXms(2);

                        if(ScalerGetByte(PB0_0B_BIST_PATTERN3) < 0x7E)
                        {
                            g_ucDpRx1Temp = 0;
                        }
                        else
                        {
                            g_ucDpRx1Temp--;
                        }

                        // Disable Error Counter
                        ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
                    }

                    // HBR2 Compliance Eye Pattern
                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT1);
                }
                else
                {
                    // PRBS7 Pattern for RBR/HBR
                    ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), _BIT0);
                }

                ScalerSetByte(PBA_A1_AUX_RESERVE1, 0x00);
                ScalerSetByte(PBA_A2_AUX_RESERVE2, 0x00);
                ScalerSetByte(PBA_A3_AUX_RESERVE3, 0x00);
                ScalerSetByte(PBA_A4_AUX_RESERVE4, 0x00);

                break;

            case 0x04:

                ScalerSetByte(PBA_A5_AUX_RESERVE5, (g_ucDpRx1PhyCtsCtrl & ~(_BIT2 | _BIT1 | _BIT0)));

                ScalerTimerDelayXms(10);

                ScalerSetByte(PBA_A1_AUX_RESERVE1, ScalerGetByte(PB0_09_BIST_PATTERN1));
                ScalerSetByte(PBA_A2_AUX_RESERVE2, ScalerGetByte(PB0_0A_BIST_PATTERN2));

                if((ScalerGetByte(PB0_09_BIST_PATTERN1) | ScalerGetByte(PB0_0A_BIST_PATTERN2)) != 0x00)
                {
                    ScalerSetByte(PBA_A3_AUX_RESERVE3, ScalerGetByte(PB0_0B_BIST_PATTERN3));
                    ScalerSetByte(PBA_A4_AUX_RESERVE4, ScalerGetByte(PB0_0C_BIST_PATTERN4));
                }
                else
                {
                    ScalerSetByte(PBA_A3_AUX_RESERVE3, 0x0F);
                    ScalerSetByte(PBA_A4_AUX_RESERVE4, 0x0F);
                }

                break;

            case 0x05:

                g_ucDpRx1PhyCtsCtrl = 0x00;

                // Clear DP PHY CTS Result
                ScalerSetByte(PBA_A0_AUX_RESERVE0, 0x99);
                ScalerSetByte(PBA_A1_AUX_RESERVE1, 0x00);
                ScalerSetByte(PBA_A2_AUX_RESERVE2, 0x00);
                ScalerSetByte(PBA_A3_AUX_RESERVE3, 0x00);
                ScalerSetByte(PBA_A4_AUX_RESERVE4, 0x00);

                // Disable PRBS7 Error Detection
                ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT2 | _BIT1 | _BIT0), 0x00);

                // Clear DP PHY CTS Control Register
                ScalerSetByte(PBA_A5_AUX_RESERVE5, 0x00);

                CLR_DP_RX1_PHY_CTS_FLG();

                break;

            default:

                ScalerSetByte(PBA_A0_AUX_RESERVE0, 0x99);
                ScalerSetByte(PBA_A5_AUX_RESERVE5, 0x00);

                break;
        }
    }
}

//--------------------------------------------------
// Description  : DP PHY CTS Lane0 ACDR
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1PhyCtsACDR(void)
{
    BYTE ucBestVCOBand = 0;
    WORD usVCOBandDiff = 0xFFFF;
    BYTE ucTemp = 0;


    ///////////////////////////
    // RX CMU EN & Mac Power //
    ///////////////////////////

    // [7] cmu_en = 1'b0 --> Disable CMU PLL
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT7, 0x00);

    // Disable RX_EN, CDR_EN
    ScalerSetByte(PB0_A1_CMU_01, 0x00);

    // Enable Lane0 RX_EN
    ScalerSetBit(PB0_A1_CMU_01, ~_BIT4, _BIT4);

    // Enable Local Bias LPF
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT5, _BIT5);

    for(pData[0] = 0; pData[0] <= 2; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // Enable Lane0 CDR_EN
    ScalerSetBit(PB0_A1_CMU_01, ~_BIT0, _BIT0);

    // One Lane Test
    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~(_BIT4 | _BIT3), _BIT3);


    /////////////////
    // CMU Setting //
    /////////////////

    // [7:5] rate_sel = 3'b000 --> 5.4GHz
    ScalerSetBit(PB0_DB_ST_0, ~(_BIT7 | _BIT6 | _BIT5), 0x00);

    // [7:5] SEL_DIV = 3'b000 --> 5.4GHz
    // [4:2] CMU_PI_I_SEL = 3'b011 --> 12.5uA
    // [1:0] CMU_SELPREDIV = 2'b00 --> /1
    ScalerSetByte(PB0_A3_CMU_03, 0x0C);

    // [7:4] CMU_SEL_PUMP_I = 4'b0011 --> Icp = 10uA
    // [3:1] CMU_SEL_RI = 3'b010 --> Rs = 10KOhm
    // [0] CMU_SEL_CP = 1'b1 --> Cp = 2pF
    ScalerSetByte(PB0_A4_CMU_04, 0x35);

    // [5] CMU_BYPASS_PI = 1'b1 --> Integer N PLL
    ScalerSetBit(PB0_A5_CMU_05, ~_BIT5, _BIT5);

    // [7] BYPASS_PI = 1'b1 --> Use Integer N Code
    ScalerSetBit(PB0_F1_SDM_00, ~_BIT7, _BIT7);

    // Digital CDR Ref Clock = Analog CDR / 10
    ScalerSetBit(PB0_AE_CDR_05, ~_BIT0, 0x00);
    ScalerSetBit(PB0_AF_CDR_06, ~_BIT1, 0x00);

    // [7:0] N_CODE = 8'b0001 0000 --> 6 + 4 = 10
    ScalerSetByte(PB0_F2_SDM_01, 0x06);


    /////////////////////
    // A/D CDR Setting //
    /////////////////////

    // KD = 4'b0000 --> KD = 0
    ScalerSetBit(PB0_CC_ERROR_COUNT_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);

    // KI = 3'b000 --> KI = 0
    ScalerSetByte(PB0_D4_KI_0, 0x00);
    ScalerSetByte(PB0_D5_KI_1, 0x00);

    // KP = 0x0B --> KP = 2.25
    ScalerSetByte(PB0_D6_KP_0, 0x09);
    ScalerSetByte(PB0_D7_KP_1, 0x09);
    ScalerSetByte(PB0_D8_KP_2, 0x09);
    ScalerSetByte(PB0_D9_KP_3, 0x09);

    // [5:4] reg_cdr_kvco = 2'b01 --> Analog CDR KVCO = 550M/v
    ScalerSetBit(PB0_B9_LANE0_A_CDR_00, ~(_BIT5 | _BIT4), _BIT4);

    // [4:2] reg_sel_cdr_res = 3'b011 --> Analog CDR Rs = 4K
    ScalerSetBit(PB0_FD_LANE0_A_CDR_07, ~(_BIT4 | _BIT3 | _BIT2), (_BIT3 | _BIT2));

    // [3:0] reg_cdr_cp_in = 4'b0011 --> Analog CDR Icp = 10.5uA
    ScalerSetBit(PB0_FE_LANE0_A_CDR_08, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));


    /////////////////////
    // Rx Misc Setting //
    /////////////////////

    // Enable Comma Detection
    ScalerSetBit(PB0_05_SAMPLE_EDGE, ~_BIT4, 0x00);

    // Enable Enhancement Control Mode --> MAC
    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT2, _BIT2);

    // Disable De-Scrambling
    ScalerSetBit(PB0_07_SCRAMBLE_CTRL, ~_BIT5, _BIT5);

    // [0] en_clkout_manual = 0 --> Disable Clkout
    ScalerSetBit(PB0_F5_WD_01, ~_BIT4, 0x00);

    // [0] en_eqen_manual = 0 --> Disable Clkout
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

    // [0] en_data_manual = 0 --> Disable Dataout
    ScalerSetBit(PB0_F7_DATA_TIMER, ~_BIT0, 0x00);

    // Disable Hybrid Mode
    ScalerSetBit(PB0_AF_CDR_06, ~_BIT0, 0x00);

    // [3:0] RST_N = 4'b0000 --> Digital Circuit Reset
    ScalerSetBit(PB0_DA_PI_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);

    // [3:0] RST_N = 4'b1111 --> Digital Circuit Normal
    ScalerSetBit(PB0_DA_PI_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));


    /////////////////
    // DFE Setting //
    /////////////////

    // Set DFE Initial
    ScalerDpRx1SetDFEInitial(_DP_LINK_HBR2);

    // [0] Tap2~6 Adaptation Revise
    ScalerSetBit(PB0_B2_EQ_00, ~_BIT0, 0x00);

    // LE Max = 20, Initial = 16, Servo Initial = 16
    ScalerSetByte(P7A_A2_L0_LIMIT_INIT, 0x40);
    ScalerSetByte(P7A_A3_L0_INIT_1, 0xD0);
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x80);
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);

    // [7:5] LE_ISEL = 3'b011
    // [4:2] LE_RLSEL = 3'b000
    ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT6 | _BIT5));

    // [7:5] LE_lowgain_highboost = 3'b000 --> LE LowBoost Mode
    ScalerSetBit(PB0_A8_CMU_08, ~(_BIT7 | _BIT6 | _BIT5), 0x00);

    // Enable SumAmp Bias
    ScalerSetBit(PB0_A6_CMU_06, ~_BIT5, _BIT5);


    //////////////
    // K Offset //
    //////////////

    // Enable Sumamp High Gain Mode
    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);

    // Fix Tap0 = 21
    ScalerSetByte(P7A_A4_L0_INIT_2, 0x15);
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x01);
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);

    // [2] Enable Foreground Calibration Auto Mode
    ScalerSetBit(PB0_AD_CDR_04, ~_BIT2, _BIT2);

    // [5] Toggle Foreground Calibration
    ScalerSetBit(PB0_AA_CDR_01, ~_BIT5, _BIT5);
    ScalerSetBit(PB0_AA_CDR_01, ~_BIT5, 0x00);
    ScalerSetBit(PB0_AA_CDR_01, ~_BIT5, _BIT5);

    // Waiting for K Offset
    for(pData[0] = 0; pData[0] <= 10; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }


    ///////////////////
    // FLD Lock Band //
    ///////////////////

    // Disable Sumamp High Gain Mode
    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, 0x00);

    // [7] cmu_en = 1'b1 --> Enable CMU PLL
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT7, _BIT7);

    // Enable Tap1~4
    ScalerSetBit(PB0_B2_EQ_00, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT5 | _BIT4 | _BIT3 | _BIT2));

    // [4] reg_en_data_manual_l0 = 1'b0
    // [3] reg_bypass_data_rdy_l0 = 1'b0
    ScalerSetBit(PB0_FF_LANE0_A_CDR_09, ~(_BIT4 | _BIT3), 0x00);

    // [0] en_clkout_manual = 0 --> Disable Clkout
    ScalerSetBit(PB0_F5_WD_01, ~_BIT4, 0x00);

    // [5] reg_adp_eq_off = 1'b0 --> Enable Analog CDR DFE Adaptation
    ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT5, 0x00);

    // [4] reg_auto_mode = 1'b1 --> FLD Select Auto Mode
    ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT4, _BIT4);

    // [0] reg_acdr_l0_en = 1'b1 --> Enable Lane0 ACDR
    ScalerSetBit(PB0_FF_LANE0_A_CDR_09, ~_BIT0, _BIT0);

    // [6] reg_xtal_en = 1'b1 --> Using Xtal
    ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT6, _BIT6);

    // Analog CDR Auto FLD Counter
    ScalerSetByte(PB0_FA_LANE0_A_CDR_04, 0xF9);
    ScalerSetByte(PB0_FB_LANE0_A_CDR_05, 0x0C);
    ScalerSetByte(PB0_FC_LANE0_A_CDR_06, 0x85);

    // [0] en_clkout_manual = 1'b1 --> Enable Clkout
    ScalerSetBit(PB0_F5_WD_01, ~_BIT4, _BIT4);

    // [7] reg_en_ana_cdr = 1'b1 --> Enable Analog CDR
    // [6] reg_en_vco_lpf = 1'b0 --> Disable Analog CDR Multi Band VCO's LPF
    // [3] reg_cp_adj_en = 1'b1 --> Enable Adjust Icp When Tracking
    // [2] reg_enhance_bias_boost = 1'b0 --> Disable Enhance LEQ Boost Bias
    // [1] reg_enhance_bias_gain = 1'b0 --> Disable Enhance LEQ Gain Bias
    // [0] reg_rstb_fsm --> Toggle for FLD reset
    ScalerSetBit(PB0_B9_LANE0_A_CDR_00, ~(_BIT7 | _BIT6 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT7 | _BIT3));
    ScalerSetBit(PB0_B9_LANE0_A_CDR_00, ~_BIT0, _BIT0);

    // Waiting for FLD Lock Band
    for(pData[0] = 0; pData[0] <= 10; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // [3] reg_cp_en_manual = 1'b0 -->  Disable cp_en_manual
    // [2] reg_adp_en_manual = 1'b0 --> Disable adp_en_manual
    ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~(_BIT3 | _BIT2), 0x00);

    // [4] reg_auto_mode = 1'b0 --> FLD Manual Mode
    ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT4, 0x00);

    // [0] en_clkout_manual = 1'b1 --> Enable Clkout
    ScalerSetBit(PB0_F5_WD_01, ~_BIT4, _BIT4);

    // [2] reg_adp_en_manual = 1'b1 --> Enable adp_en_manual
    ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT2, _BIT2);

    ucBestVCOBand = ScalerGetByte(PB0_BF_LANE0_A_CDR_02);

    g_usDpRx1ClockCount = 0x2710; // ideal 5.4G

    for(ucTemp = 0; ucTemp <= 5; ucTemp++)
    {
        // Manually Reload FLD Band
        ScalerSetByte(PB0_BE_LANE0_A_CDR_01, (ScalerGetByte(PB0_BF_LANE0_A_CDR_02) - 2 + ucTemp));

        // Waiting for VCO Stable
        for(pData[0] = 0; pData[0] <= 2; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // [5] ln_ck_sel = 1'b1 -> CDR Clock
        // [3:0] DP_XTAL_CYCLE = 4'b0011 -> XTAL Counter Cycles = 1000
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~(_BIT5 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT1 | _BIT0));

        // [1:0] freqdet_lane_sel = 2'b00 -> Lane0
        ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), 0x00);

        // [6] LANE_COUNT_CLEAR = 0 -> Keep the LANE_COUNT value
        ScalerSetBit(PB0_52_DP_SIG_DET_2, ~_BIT6, 0x00);

        // [7] DP_SIG_DET_EN = 1'b1 -> Enable Signal Detection
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, _BIT7);

        // Waiting for Measure Done -> 50us
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        if(ScalerGetBit(PB0_50_DP_SIG_DET_0, _BIT6) == _BIT6)
        {
            g_usDpRx1ClockCount1 = ((((WORD)ScalerGetByte(PB0_53_DP_SIG_DET_3)) << 8) | ScalerGetByte(PB0_54_DP_SIG_DET_4));
        }
        else
        {
            ucBestVCOBand = ScalerGetByte(PB0_BF_LANE0_A_CDR_02);
            continue;
        }

        // [7] DP_SIG_DET_EN = 0 -> Disable Signal Detection
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);

        if((abs(g_usDpRx1ClockCount1 - g_usDpRx1ClockCount) < usVCOBandDiff) && (g_usDpRx1ClockCount1 >= g_usDpRx1ClockCount))
        {
            usVCOBandDiff = abs(g_usDpRx1ClockCount1 - g_usDpRx1ClockCount);
            ucBestVCOBand = (ScalerGetByte(PB0_BF_LANE0_A_CDR_02) - 2 + ucTemp);
        }
    }

    // Manually Reload FLD Band
    ScalerSetByte(PB0_BE_LANE0_A_CDR_01, ucBestVCOBand);

    // Waiting for VCO Stable
    for(pData[0] = 0; pData[0] <= 2; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // [3] reg_cp_en_manual = 1'b1 -->  Enaable cp_en_manual
    ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT3, _BIT3);


    /////////////////////
    // Adaptation Loop //
    /////////////////////

    // Enable Tap0 Adaptation
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x41);

    // Waiting for DFE Adaptation
    for(pData[0] = 0; pData[0] <= 10; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // Start to Record Tap0 Max/Min
    ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, _BIT0);

    // Waiting for DFE Adaptation
    for(pData[0] = 0; pData[0] <= 10; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // Stop To Record Tap0 Max/Min
    ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, 0x00);

    // Disable Vth/Tap0 Adaptation
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);

    // Read Back Lane0
    ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);

    // Read Back Tap0 Max
    ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), 0x00);

    g_pucDpRx1Tap0MaxTemp[0] = ScalerGetByte(P7A_F3_COEF_4);

    // Reload Max Value of Tap0 (Lane0)
    ScalerSetByte(P7A_A4_L0_INIT_2, g_pucDpRx1Tap0MaxTemp[0]);

    // Reload Tap0
    ScalerSetBit(P7A_AA_L0_INIT_8, ~_BIT0, _BIT0);
    ScalerSetBit(P7A_AA_L0_INIT_8, ~_BIT0, 0x00);

    // Read Back Vth
    ScalerSetByte(P7A_F0_COEF_1, 0x00);

    // Reload Vth - 1 (Lane0)
    ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);
    ScalerSetByte(P7A_A9_L0_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) - 1);

    // Reload Vth
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x40);
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);

    // Enable LE Adaptation
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x80);

    // Waiting for DFE Adaptation
    for(pData[0] = 0; pData[0] <= 10; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // Start to Record LE Max/Min
    ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, _BIT0);

    // Waiting for DFE Adaptation
    for(pData[0] = 0; pData[0] <= 10; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // Stop To Record LE Max/Min
    ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, 0x00);

    // Disable Adaptation
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);

    // Read Back Lane0
    ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);

    // Read Back LE Min
    ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT3 | _BIT1));

    g_pucDpRx1LEMinTemp[0] = ScalerGetByte(P7A_F3_COEF_4);

    // Reload LE to Min
    ScalerSetBit(P7A_A2_L0_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMinTemp[0] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
    ScalerSetBit(P7A_A3_L0_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[0] << 2) & _BIT6));

    // Reload LE
    ScalerSetBit(P7A_AA_L0_INIT_8, ~_BIT7, _BIT7);
    ScalerSetBit(P7A_AA_L0_INIT_8, ~_BIT7, 0x00);

    // Read Back Vth
    ScalerSetByte(P7A_F0_COEF_1, 0x00);

    // Reload Vth + 1 (Lane0)
    ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);
    ScalerSetByte(P7A_A9_L0_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) + 1);

    // Reload Vth
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x40);
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);

    // Enable Tap0~4 Adaptation for HBR2 0x01F
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x1F);

    // [0] en_eqen_manual = 1 --> Enable Adapation
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

    // Waiting for Adaptation
    for(pData[0] = 0; pData[0] <= 20; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // Disable Adaptation
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

    // Disable Adaptation   0x000
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);


    ////////////////
    // SetPHY End //
    ////////////////

    // Enable Sumamp High Gain Mode
    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);

    // Reload Tap0 to 21.5
    ScalerSetByte(P7A_A4_L0_INIT_2, 0x15);
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x01);
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);

    // Reset Digital CDR
    ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);
    ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

    // [5] reg_adp_eq_off = 1'b1 --> Disable Adaptation for Analog CDR
    ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT5, _BIT5);

    // [4] reg_en_data_manual_l0 = 1'b1
    // [3] reg_bypass_data_rdy_l0 = 1'b1
    ScalerSetBit(PB0_FF_LANE0_A_CDR_09, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));

    // DP Mac Clock Select to Link Clock
    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT6, 0x00);
}

//--------------------------------------------------
// Description  : Check Dp Training Pattern 1 for PHY CTS
// Input Value  : None
// Output Value : TRUE --> TP1 Pass
//--------------------------------------------------
bit ScalerDpRx1PhyCtsTp1Check(void)
{
    // Wait for 10ms
    ScalerTimerDelayXms(10);

    // Clear 8B/10B Decode Error Flags
    ScalerSetBit(PB0_06_DECODE_10B8B_ERROR, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));

    // Wait for 2ms
    ScalerTimerDelayXms(2);

    ScalerSetByte(PBA_A1_AUX_RESERVE1, ScalerGetByte(PB0_06_DECODE_10B8B_ERROR));
    ScalerSetByte(PBA_A2_AUX_RESERVE2, ScalerGetByte(PB0_14_EQ_CRC_3));
    ScalerSetByte(PBA_A3_AUX_RESERVE3, g_ucDpRx1PhyCtsCtrl & 0x30);

    switch(g_ucDpRx1PhyCtsCtrl & 0x30)
    {
        case 0x00: // Lane0 Test

            // EQ auto K,Check lane0 CRC
            pData[0] = 0x10;

            break;

        case 0x10: // Lane1 Test

            // EQ auto K,Check lane1 CRC
            pData[0] = 0x20;

            break;

        case 0x20: // Lane2 Test

            // EQ auto K,Check lane2 CRC
            pData[0] = 0x40;

            break;

        case 0x30: // Lane3 Test

            // EQ auto K,Check lane3 CRC
            pData[0] = 0x80;

            break;

        default:

            pData[0] = 0x00;

            break;
    }

    if(((ScalerGetByte(PBA_A1_AUX_RESERVE1) & 0x0F) == 0x00) &&
       ((ScalerGetByte(PBA_A2_AUX_RESERVE2) & pData[0]) != 0x00))
    {
        return _TRUE;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : DP Phy Setting in TP1 for PHY CTS
// Input Value  : FLD Reference Clock
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1PhyCtsTp1SetPhy(EnumDpNFCodeRef enumDpNFCodeRef)
{
    ///////////////////////////
    // RX CMU EN & Mac Power //
    ///////////////////////////

    // [0] cmu_en = 1'b0 --> Disable CMU PLL
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT7, 0x00);

    // Disable RX_EN, CDR_EN
    ScalerSetByte(PB0_A1_CMU_01, 0x00);

    switch(g_ucDpRx1PhyCtsCtrl & 0x30)
    {
        case 0x00: // Lane0 Test

            ScalerSetByte(PB0_A1_CMU_01, 0x01);

            // Enable Local Bias LPF
            ScalerSetBit(PB0_A7_CMU_07, ~_BIT5, _BIT5);

            // Waiting Bias Settling
            for(pData[0] = 0; pData[0] <= 2; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            ScalerSetByte(PB0_A1_CMU_01, 0x11);

            break;

        case 0x10: // Lane1 Test

            ScalerSetByte(PB0_A1_CMU_01, 0x02);

            // Enable Local Bias LPF
            ScalerSetBit(PB0_A7_CMU_07, ~_BIT5, _BIT5);

            // Waiting Bias Settling
            for(pData[0] = 0; pData[0] <= 2; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            ScalerSetByte(PB0_A1_CMU_01, 0x22);

            break;

        case 0x20: // Lane2 Test

            ScalerSetByte(PB0_A1_CMU_01, 0x04);

            // Enable Local Bias LPF
            ScalerSetBit(PB0_A7_CMU_07, ~_BIT5, _BIT5);

            // Waiting Bias Settling
            for(pData[0] = 0; pData[0] <= 2; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            ScalerSetByte(PB0_A1_CMU_01, 0x44);

            break;

        case 0x30: // Lane3 Test

            ScalerSetByte(PB0_A1_CMU_01, 0x08);

            // Enable Local Bias LPF
            ScalerSetBit(PB0_A7_CMU_07, ~_BIT5, _BIT5);

            // Waiting Bias Settling
            for(pData[0] = 0; pData[0] <= 2; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            ScalerSetByte(PB0_A1_CMU_01, 0x88);

            break;

        default:

            break;
    }

    // 4 Lane DP CTS
    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));


    /////////////////
    // CMU Setting //
    /////////////////

    // [5] CMU_BYPASS_PI = 1'b0 --> Fractional N PLL
    // [3] CMU_CKSEL = 1'b0 --> Ref. XTAL
    ScalerSetBit(PB0_A5_CMU_05, ~(_BIT5 | _BIT3), 0x00);

    // [7] BYPASS_PI = 1'b0 --> Fractional N PLL
    ScalerSetBit(PB0_F1_SDM_00, ~_BIT7, 0x00);

    switch(g_ucDpRx1PhyCtsCtrl & (_BIT7 | _BIT6))
    {
        case _BIT7: // HBR2

            // [7:5] rate_sel = 3'b000 --> 5.4GHz
            ScalerSetBit(PB0_DB_ST_0, ~(_BIT7 | _BIT6 | _BIT5), 0x00);

            // [7:5] SEL_DIV = 3'b000 --> 5.4GHz
            // [4:2] CMU_PI_I_SEL = 3'b011 --> 12.5uA
            // [1:0] CMU_SELPREDIV = 2'b00 --> /1
            ScalerSetByte(PB0_A3_CMU_03, 0x0C);

            // [7:4] CMU_SEL_PUMP_I = 4'b0101 --> Icp = 15uA
            // [3:1] CMU_SEL_RI = 3'b010 --> Rs = 10KOhm
            // [0] CMU_SEL_CP = 1'b1 --> Cp = 2pF
            ScalerSetByte(PB0_A4_CMU_04, 0x55);

            ScalerDpRx1CMUNFCodeSetting(enumDpNFCodeRef, _DP_LINK_HBR2);

            break;

        case _BIT6: // HBR

            // [7:5] rate_sel = 3'b001 --> 2.7GHz
            ScalerSetBit(PB0_DB_ST_0, ~(_BIT7 | _BIT6 | _BIT5), _BIT5);

            // [7:5] SEL_DIV = 3'b001 --> 2.7GHz
            // [4:2] CMU_PI_I_SEL = 3'b011 --> 12.5uA
            // [1:0] CMU_SELPREDIV = 2'b00 --> /1
            ScalerSetByte(PB0_A3_CMU_03, 0x2C);

            // [7:4] CMU_SEL_PUMP_I = 4'b0101 --> Icp = 15uA
            // [3:1] CMU_SEL_RI = 3'b011 --> Rs = 12KOhm
            // [0] CMU_SEL_CP = 1'b1 --> Cp = 2pF
            ScalerSetByte(PB0_A4_CMU_04, 0x57);

            ScalerDpRx1CMUNFCodeSetting(enumDpNFCodeRef, _DP_LINK_HBR);

            break;

        case 0x00: // RBR
        default:

            // [7:5] rate_sel = 3'b001 --> 1.62GHz
            ScalerSetBit(PB0_DB_ST_0, ~(_BIT7 | _BIT6 | _BIT5), _BIT5);

            // [7:5] SEL_DIV = 3'b001 --> 1.62GHz
            // [4:2] CMU_PI_I_SEL = 3'b011 --> 12.5uA
            // [1:0] CMU_SELPREDIV = 2'b00 --> /1
            ScalerSetByte(PB0_A3_CMU_03, 0x2C);

            // [7:4] CMU_SEL_PUMP_I = 4'b0101 --> Icp = 15uA
            // [3:1] CMU_SEL_RI = 3'b011 --> Rs = 12KOhm
            // [0] CMU_SEL_CP = 1'b1 --> Cp = 2pF
            ScalerSetByte(PB0_A4_CMU_04, 0x57);

            ScalerDpRx1CMUNFCodeSetting(enumDpNFCodeRef, _DP_LINK_RBR);

            break;
    }


    /////////////////
    // CDR Setting //
    /////////////////

    // KI = 3'b011 --> KI = 1/4
    ScalerSetByte(PB0_D4_KI_0, 0x6C);
    ScalerSetByte(PB0_D5_KI_1, 0x6C);

    // [3:0] KD = 4'b0000 --> KD = 0
    ScalerSetBit(PB0_CC_ERROR_COUNT_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);

    switch(g_ucDpRx1PhyCtsCtrl & (_BIT7 | _BIT6))
    {
        case 0x00: // RBR

            // KP = 0x37 --> KP = 13.75
            ScalerSetByte(PB0_D6_KP_0, 0x37);
            ScalerSetByte(PB0_D7_KP_1, 0x37);
            ScalerSetByte(PB0_D8_KP_2, 0x37);
            ScalerSetByte(PB0_D9_KP_3, 0x37);

            break;

        case _BIT7: // HBR2

            // KP = 0x08 --> KP = 2
            ScalerSetByte(PB0_D6_KP_0, 0x08);
            ScalerSetByte(PB0_D7_KP_1, 0x08);
            ScalerSetByte(PB0_D8_KP_2, 0x08);
            ScalerSetByte(PB0_D9_KP_3, 0x08);

            break;

        default:
        case _BIT6: // HBR

            // KP = 0x1F --> KP = 7.75
            ScalerSetByte(PB0_D6_KP_0, 0x1F);
            ScalerSetByte(PB0_D7_KP_1, 0x1F);
            ScalerSetByte(PB0_D8_KP_2, 0x1F);
            ScalerSetByte(PB0_D9_KP_3, 0x1F);

            break;
    }


    /////////////////////
    // Rx Misc Setting //
    /////////////////////

    // Disable Comma Detection
    ScalerSetBit(PB0_05_SAMPLE_EDGE, ~_BIT4, _BIT4);

    // Disable De-Scrambling
    ScalerSetBit(PB0_07_SCRAMBLE_CTRL, ~_BIT5, _BIT5);

    // Enable Enhancement Control Mode --> MAC
    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT2, _BIT2);

    // [7] reg_en_ana_cdr = 1'b0 --> Disable Analog CDR
    ScalerSetBit(PB0_B9_LANE0_A_CDR_00, ~_BIT7, 0x00);

    // [0] reg_acdr_l0_en = 1'b0 --> Disable Lane0 ACDR
    ScalerSetBit(PB0_FF_LANE0_A_CDR_09, ~_BIT0, 0x00);

    // Disable Hybrid Mode
    ScalerSetBit(PB0_AF_CDR_06, ~_BIT0, 0x00);

    // [1] bypass_clk_rdy = 1 --> Manual Mode
    // [0] en_clkout_manual = 0 --> Disable Clkout
    ScalerSetBit(PB0_F5_WD_01, ~(_BIT5 | _BIT4), _BIT5);

    // [1] bypass_eqen_rdy = 1 --> Manual Mode
    // [0] en_eqen_manual = 0 --> Disable Clkout
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~(_BIT1 | _BIT0), _BIT1);

    // [1] bypass_data_rdy = 1 --> Manual Mode
    // [0] en_data_manual = 0 --> Disable Dataout
    ScalerSetBit(PB0_F7_DATA_TIMER, ~(_BIT1 | _BIT0), _BIT1);

    // [3:0] RST_N = 4'b0000 --> Digital Circuit Reset
    ScalerSetBit(PB0_DA_PI_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);

    // [3:0] RST_N = 4'b1111 --> Digital Circuit Normal
    ScalerSetBit(PB0_DA_PI_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));


    /////////////////
    // DFE Setting //
    /////////////////

    switch(g_ucDpRx1PhyCtsCtrl & (_BIT7 | _BIT6))
    {
        case _BIT7: // HBR2

            // Set DFE Initial
            ScalerDpRx1SetDFEInitial(_DP_LINK_HBR2);

            // LE Max = 24, Initial = 18, Servo Initial = 16
            ScalerSetByte(P7A_A2_L0_LIMIT_INIT, 0x82);
            ScalerSetByte(P7A_B2_L1_LIMIT_INIT, 0x82);
            ScalerSetByte(P7A_C2_L2_LIMIT_INIT, 0x82);
            ScalerSetByte(P7A_D2_L3_LIMIT_INIT, 0x82);
            ScalerSetByte(P7A_A3_L0_INIT_1, 0xD0);
            ScalerSetByte(P7A_B3_L1_INIT_1, 0xD0);
            ScalerSetByte(P7A_C3_L2_INIT_1, 0xD0);
            ScalerSetByte(P7A_D3_L3_INIT_1, 0xD0);

            ScalerSetByte(P7A_AA_L0_INIT_8, 0x80);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x80);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x80);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x80);
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

            // [7:5] LE_ISEL = 3'b011
            // [4:2] LE_RLSEL = 3'b000
            ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT6 | _BIT5));

            // [7:5] LE_lowgain_highboost = 3'b000 --> LE LowBoost Mode
            ScalerSetBit(PB0_A8_CMU_08, ~(_BIT7 | _BIT6 | _BIT5), 0x00);

            // Enable SumAmp Bias
            ScalerSetBit(PB0_A6_CMU_06, ~_BIT5, _BIT5);

            break;

        case _BIT6: // HBR

            // Set DFE Initial
            ScalerDpRx1SetDFEInitial(_DP_LINK_HBR);

            // [7:5] LE_ISEL = 3'b010
            // [4:2] LE_RLSEL = 3'b001
            ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT6 | _BIT2));

            // [7:5] LE_lowgain_highboost = 3'b100 --> LE HighBoost Mode
            ScalerSetBit(PB0_A8_CMU_08, ~(_BIT7 | _BIT6 | _BIT5), _BIT7);

            // Disable SumAmp Bias
            ScalerSetBit(PB0_A6_CMU_06, ~_BIT5, 0x00);

            break;

        case 0x00: // RBR
        default:

            // Set DFE Initial
            ScalerDpRx1SetDFEInitial(_DP_LINK_RBR);

            // [7:5] LE_ISEL = 3'b000
            // [4:2] LE_RLSEL = 3'b011
            ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT3 | _BIT2));

            // [7:5] LE_lowgain_highboost = 3'b100 --> LE HighBoost Mode
            ScalerSetBit(PB0_A8_CMU_08, ~(_BIT7 | _BIT6 | _BIT5), _BIT7);

            // Disable SumAmp Bias
            ScalerSetBit(PB0_A6_CMU_06, ~_BIT5, 0x00);

            break;
    }


    //////////////
    // K Offset //
    //////////////

    // Enable Sumamp High Gain Mode
    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);

    // Fix Tap0 = 21
    ScalerSetByte(P7A_A4_L0_INIT_2, 0x15);
    ScalerSetByte(P7A_B4_L1_INIT_2, 0x15);
    ScalerSetByte(P7A_C4_L2_INIT_2, 0x15);
    ScalerSetByte(P7A_D4_L3_INIT_2, 0x15);

    ScalerSetByte(P7A_AA_L0_INIT_8, 0x01);
    ScalerSetByte(P7A_BA_L1_INIT_8, 0x01);
    ScalerSetByte(P7A_CA_L2_INIT_8, 0x01);
    ScalerSetByte(P7A_DA_L3_INIT_8, 0x01);

    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
    ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
    ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
    ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

    // [2] Enable Foreground Calibration Auto Mode
    ScalerSetBit(PB0_AD_CDR_04, ~_BIT2, _BIT2);

    // [5] Toggle Foreground Calibration
    ScalerSetBit(PB0_AA_CDR_01, ~_BIT5, _BIT5);
    ScalerSetBit(PB0_AA_CDR_01, ~_BIT5, 0x00);
    ScalerSetBit(PB0_AA_CDR_01, ~_BIT5, _BIT5);

    // Waiting for K Offset
    for(pData[0] = 0; pData[0] <= 6; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }


    /////////////////////
    // Adaptation Loop //
    /////////////////////

    // Reset Digital CDR
    ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);
    ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

    // Disable Tap1~6
    ScalerSetBit(PB0_B2_EQ_00, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), 0x00);

    // Disable Adaptation
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

    // [7] cmu_en = 1'b1 --> Enable CMU PLL
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT7, _BIT7);

    // Waiting for CMU PLL Stable
    for(pData[0] = 0; pData[0] <= 4; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // [0] en_clkout_manual = 1 --> Enable Clkout
    ScalerSetBit(PB0_F5_WD_01, ~_BIT4, _BIT4);

    // [0] en_data_manual = 1 --> Enable Dataout
    ScalerSetBit(PB0_F7_DATA_TIMER, ~_BIT0, _BIT0);

    // DP Mac Clock Select to Link Clock
    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT6, 0x00);
}

//--------------------------------------------------
// Description  : Check Dp Training Pattern 2 for PHY CTS
// Input Value  : None
// Output Value : TRUE --> TP2 Pass
//--------------------------------------------------
bit ScalerDpRx1PhyCtsTp2Check(void)
{
    ScalerTimerDelayXms(10);

    switch(g_ucDpRx1PhyCtsCtrl & 0x30)
    {
        case 0x00: // Lane0 Test

            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT4 | _BIT3), 0x00);

            break;

        case 0x10: // Lane1 Test

            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT4 | _BIT3), _BIT3);

            break;

        case 0x20: // Lane2 Test

            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT4 | _BIT3), _BIT4);

            break;

        case 0x30: // Lane3 Test

            ScalerSetBit(PB0_08_BIST_PATTERN_SEL, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));

            break;

        default:
            break;
    }

    // Mac Use Xtal Clk
    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT6, _BIT6);

    // Clear EQ-CRC
    ScalerSetBit(PB0_12_EQ_CRC_1, ~(_BIT7 | _BIT6), _BIT6);

    // Start Calculate EQ-CRC
    ScalerSetBit(PB0_12_EQ_CRC_1, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));

    ScalerTimerDelayXms(2);

    ScalerSetByte(PBA_A1_AUX_RESERVE1, ScalerGetByte(PB0_12_EQ_CRC_1));
    ScalerSetByte(PBA_A2_AUX_RESERVE2, ScalerGetByte(PB0_13_EQ_CRC_2));
    ScalerSetByte(PBA_A3_AUX_RESERVE3, g_ucDpRx1PhyCtsCtrl & 0x30);

    // Stop Calculate EQ-CRC
    ScalerSetBit(PB0_12_EQ_CRC_1, ~(_BIT7 | _BIT6), _BIT6);

    switch(g_ucDpRx1PhyCtsCtrl & 0x30)
    {
        case 0x00: // Lane0 Test

            pData[0] = 0x01;

            break;

        case 0x10: // Lane1 Test

            pData[0] = 0x02;

            break;

        case 0x20: // Lane2 Test

            pData[0] = 0x04;

            break;

        case 0x30: // Lane3 Test

            pData[0] = 0x08;

            break;

        default:

            pData[0] = 0x00;

            break;
    }

    if((ScalerGetByte(PBA_A1_AUX_RESERVE1) & pData[0]) != 0x00)
    {
        return _TRUE;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : DP Phy Setting in TP2 for PHY CTS
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1PhyCtsTp2SetPhy(void)
{
    /////////////////////
    // Rx Misc Setting //
    /////////////////////

    // Enable Commoa Detection
    ScalerSetBit(PB0_05_SAMPLE_EDGE, ~_BIT4, 0x00);

    // Disable De-Scrambling
    ScalerSetBit(PB0_07_SCRAMBLE_CTRL, ~_BIT5, _BIT5);

    // Enable Enhancement Control Mode --> MAC
    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT2, _BIT2);

    // [0] cmu_en = 1'b0 --> Disable CMU PLL
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT7, 0x00);

    // [1] bypass_clk_rdy = 1 --> Manual Mode
    // [0] en_clkout_manual = 0 --> Disable Clkout
    ScalerSetBit(PB0_F5_WD_01, ~(_BIT5 | _BIT4), _BIT5);

    // [1] bypass_eqen_rdy = 1 --> Manual Mode
    // [0] en_eqen_manual = 0 --> Disable Clkout
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~(_BIT1 | _BIT0), _BIT1);

    // [1] bypass_data_rdy = 1 --> Manual Mode
    // [0] en_data_manual = 0 --> Disable Dataout
    ScalerSetBit(PB0_F7_DATA_TIMER, ~(_BIT1 | _BIT0), _BIT1);

    // [7] reg_en_ana_cdr = 1'b0 --> Disable Analog CDR
    ScalerSetBit(PB0_B9_LANE0_A_CDR_00, ~_BIT7, 0x00);

    // [0] reg_acdr_l0_en = 1'b0 --> Disable Lane0 ACDR
    ScalerSetBit(PB0_FF_LANE0_A_CDR_09, ~_BIT0, 0x00);

    // Disable Hybrid Mode
    ScalerSetBit(PB0_AF_CDR_06, ~_BIT0, 0x00);

    // [3:0] RST_N = 4'b0000 --> Digital Circuit Reset
    ScalerSetBit(PB0_DA_PI_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);

    // [3:0] RST_N = 4'b1111 --> Digital Circuit Normal
    ScalerSetBit(PB0_DA_PI_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));


    /////////////////
    // DFE Setting //
    /////////////////

    switch(g_ucDpRx1PhyCtsCtrl & (_BIT7 | _BIT6))
    {
        case _BIT7: // HBR2

            // Set DFE Initial
            ScalerDpRx1SetDFEInitial(_DP_LINK_HBR2);

            // TPS3 Pattern is Uesed as Training Pattern 2 in HBR2
            if((g_ucDpRx1PhyCtsCtrl & 0x07) == 0x02)
            {
                // Tap Divider = 34 for TPS3 Patterns
                ScalerSetByte(P7A_ED_LOOP_DIV_2, 0xE2);
            }
            else
            {
                // Tap Divider = 10 for Other Patterns
                ScalerSetByte(P7A_ED_LOOP_DIV_2, 0xCA);
            }

            // LE Max = 20, Initial = 16, Servo Initial = 16
            ScalerSetByte(P7A_A2_L0_LIMIT_INIT, 0x40);
            ScalerSetByte(P7A_B2_L1_LIMIT_INIT, 0x40);
            ScalerSetByte(P7A_C2_L2_LIMIT_INIT, 0x40);
            ScalerSetByte(P7A_D2_L3_LIMIT_INIT, 0x40);
            ScalerSetByte(P7A_A3_L0_INIT_1, 0xD0);
            ScalerSetByte(P7A_B3_L1_INIT_1, 0xD0);
            ScalerSetByte(P7A_C3_L2_INIT_1, 0xD0);
            ScalerSetByte(P7A_D3_L3_INIT_1, 0xD0);

            ScalerSetByte(P7A_AA_L0_INIT_8, 0x80);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x80);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x80);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x80);
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

            // [0] Tap2~6 Adaptation Revise
            ScalerSetBit(PB0_B2_EQ_00, ~_BIT0, 0x00);

            break;

        case _BIT6: // HBR

            // Set DFE Initial
            ScalerDpRx1SetDFEInitial(_DP_LINK_HBR);

            break;

        case 0x00: // RBR
        default:

            // Set DFE Initial
            ScalerDpRx1SetDFEInitial(_DP_LINK_RBR);

            break;
    }

    // Disable Sumamp High Gain Mode
    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, 0x00);

    // Enable Tap1~4
    ScalerSetBit(PB0_B2_EQ_00, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT5 | _BIT4 | _BIT3 | _BIT2));

    // Reset Digital CDR
    ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);
    ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);


    /////////////////////
    // Adaptation Loop //
    /////////////////////

    // Enable Vth, Tap0 Adaptation   0x041
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x41);
    ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x41);
    ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x41);
    ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x41);

    // [7] cmu_en = 1'b1 --> Enable CMU PLL
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT7, _BIT7);

    // Waiting for CMU PLL Stable
    for(pData[0] = 0; pData[0] <= 10; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // [0] en_clkout_manual = 1 --> Enable Clkout
    ScalerSetBit(PB0_F5_WD_01, ~_BIT4, _BIT4);

    // [0] en_eqen_manual = 1 --> Enable Adapation
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

    // Waiting for Adaptation
    for(pData[0] = 0; pData[0] <= 20; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    if((g_ucDpRx1PhyCtsCtrl & 0x80) == 0x80)
    {
        if((g_ucDpRx1PhyCtsCtrl & 0x07) == 0x02)  // TPS3 Pattern
        {
            // Disable Adaptation
            ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

            // Disable Adaptation for HBR2 TPS3 Pattern
            ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);
            ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x00);
            ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x00);
            ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x00);

            // Reload LE = 16
            ScalerSetBit(P7A_A2_L0_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_A3_L0_INIT_1, ~_BIT6, _BIT6);
            ScalerSetBit(P7A_B2_L1_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_B3_L1_INIT_1, ~_BIT6, _BIT6);
            ScalerSetBit(P7A_C2_L2_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_C3_L2_INIT_1, ~_BIT6, _BIT6);
            ScalerSetBit(P7A_D2_L3_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_D3_L3_INIT_1, ~_BIT6, _BIT6);

            // Reload LE
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x80);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x80);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x80);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x80);
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);
        }
        else
        {
            // Start to Record Tap0 Max/Min
            ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, _BIT0);

            // Waiting for Adaptation
            for(pData[0] = 0; pData[0] <= 20; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            // Stop To Record Tap0 Max/Min
            ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, 0x00);

            // Disable Adaptation
            ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

            // Read Back Tap0 Max
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), 0x00);

            // Reload Max Value of Tap0 (Lane0)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);
            ScalerSetByte(P7A_A4_L0_INIT_2, ScalerGetByte(P7A_F3_COEF_4));

            // Reload Max Value of Tap0 (Lane1)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);
            ScalerSetByte(P7A_B4_L1_INIT_2, ScalerGetByte(P7A_F3_COEF_4));

            // Reload Max Value of Tap0 (Lane2)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);
            ScalerSetByte(P7A_C4_L2_INIT_2, ScalerGetByte(P7A_F3_COEF_4));

            // Reload Max Value of Tap0 (Lane3)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
            ScalerSetByte(P7A_D4_L3_INIT_2, ScalerGetByte(P7A_F3_COEF_4));

            // Reload Tap0
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x01);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x01);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x01);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x01);
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

            // Read Back Vth
            ScalerSetByte(P7A_F0_COEF_1, 0x00);

            // Reload Vth - 1 (Lane0)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);
            ScalerSetByte(P7A_A9_L0_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) - 1);

            // Reload Vth - 1 (Lane1)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);
            ScalerSetByte(P7A_B9_L1_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) - 1);

            // Reload Vth - 1 (Lane2)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);
            ScalerSetByte(P7A_C9_L2_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) - 1);

            // Reload Vth - 1 (Lane3)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
            ScalerSetByte(P7A_D9_L3_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) - 1);

            // Reload Vth
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x40);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x40);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x40);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x40);
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

            // Enable LE Adaptation   0x080
            ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x80);
            ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x80);
            ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x80);
            ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x80);

            // [0] en_eqen_manual = 1 --> Enable Adapation
            ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

            // Waiting for Adaptation
            for(pData[0] = 0; pData[0] <= 20; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            // Start to Record LE Max/Min
            ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, _BIT0);

            // Waiting for Adaptation
            for(pData[0] = 0; pData[0] <= 20; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            // Stop To Record LE Max/Min
            ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, 0x00);

            // Disable Adaptation
            ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

            // Read Back LE Min
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT3 | _BIT1));

            // Reload Min Value of LE (Lane0)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);
            ScalerSetBit(P7A_A2_L0_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (ScalerGetByte(P7A_F3_COEF_4) & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_A3_L0_INIT_1, ~_BIT6, ((ScalerGetByte(P7A_F3_COEF_4) << 2) & _BIT6));

            // Reload Min Value of LE (Lane1)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);
            ScalerSetBit(P7A_B2_L1_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (ScalerGetByte(P7A_F3_COEF_4) & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_B3_L1_INIT_1, ~_BIT6, ((ScalerGetByte(P7A_F3_COEF_4) << 2) & _BIT6));

            // Reload Min Value of LE (Lane2)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);
            ScalerSetBit(P7A_C2_L2_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (ScalerGetByte(P7A_F3_COEF_4) & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_C3_L2_INIT_1, ~_BIT6, ((ScalerGetByte(P7A_F3_COEF_4) << 2) & _BIT6));

            // Reload Min Value of LE (Lane3)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
            ScalerSetBit(P7A_D2_L3_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (ScalerGetByte(P7A_F3_COEF_4) & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_D3_L3_INIT_1, ~_BIT6, ((ScalerGetByte(P7A_F3_COEF_4) << 2) & _BIT6));

            // Reload LE
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x80);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x80);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x80);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x80);
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

            // Read Back Vth
            ScalerSetByte(P7A_F0_COEF_1, 0x00);

            // Reload Vth + 1 (Lane0)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);
            ScalerSetByte(P7A_A9_L0_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) + 1);

            // Reload Vth + 1 (Lane1)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);
            ScalerSetByte(P7A_B9_L1_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) + 1);

            // Reload Vth + 1 (Lane2)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);
            ScalerSetByte(P7A_C9_L2_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) + 1);

            // Reload Vth + 1 (Lane3)
            ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
            ScalerSetByte(P7A_D9_L3_INIT_7, (ScalerGetByte(P7A_F1_COEF_2) & 0x0F) + 1);

            // Reload Vth
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x40);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x40);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x40);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x40);
            ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
            ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
            ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
            ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

            // Enable Tap0~4 Adaptation for HBR2 0x01F
            ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
            ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x1F);
            ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x1F);
            ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x1F);
            ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x1F);
        }
    }
    else
    {
        // Disable Adaptation
        ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

        // Enable LE, Tap1~4 Adaptation for RBR/HBR 0x09E
        ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x9E);
        ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x9E);
        ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x9E);
        ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x9E);
    }

    // [0] en_eqen_manual = 1 --> Enable Adapation
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

    // Waiting for Adaptation
    for(pData[0] = 0; pData[0] <= 20; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    // Disable Adaptation
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

    // Disable Adaptation   0x000
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);
    ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x00);
    ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x00);
    ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x00);


    ////////////////
    // SetPHY End //
    ////////////////

    // Enable Sumamp High Gain Mode
    ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);

    // Reload Tap0 to 21.5
    ScalerSetByte(P7A_A4_L0_INIT_2, 0x15);
    ScalerSetByte(P7A_B4_L1_INIT_2, 0x15);
    ScalerSetByte(P7A_C4_L2_INIT_2, 0x15);
    ScalerSetByte(P7A_D4_L3_INIT_2, 0x15);

    ScalerSetByte(P7A_AA_L0_INIT_8, 0x01);
    ScalerSetByte(P7A_BA_L1_INIT_8, 0x01);
    ScalerSetByte(P7A_CA_L2_INIT_8, 0x01);
    ScalerSetByte(P7A_DA_L3_INIT_8, 0x01);

    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
    ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
    ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
    ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

    // Reset Digital CDR
    ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);
    ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

    // [0] en_data_manual = 1 --> Enable Dataout
    ScalerSetBit(PB0_F7_DATA_TIMER, ~_BIT0, _BIT0);
}

//--------------------------------------------------
// Description  : DP Phy Initial Setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1PhyInitial(void)
{
    /////////////
    // Rx Misc //
    /////////////

    // [7] mhl_mode = 1'b0 --> Disable MHL Mode
    // [6] en_mhl_op = 1'b0 --> Disable D1 Port MHL OP
    ScalerSetBit(PB0_A0_CMU_00, ~(_BIT7 | _BIT6), 0x00);

    /////////////////////////
    // CMU CDR DFE Setting //
    /////////////////////////

    // [4] CMU_en_awdog = 1'b0 --> Disable CMU WatchDog
    // [3] CMU_CKSEL = 1'b0 --> Ref. XTAL
    // [2] CMU_Sel_D4 = 1'b0 --> CMU Feedback Loop Divider = 1
    ScalerSetBit(PB0_A5_CMU_05, ~(_BIT4 | _BIT3 | _BIT2), 0x00);

    // [0] PLL_WD_EN = 1'b0 --> Disable WD
    ScalerSetBit(PB0_F4_WD_00, ~_BIT0, 0x00);

    // [4] SEL_DFE_DELAY = 1'b0 --> Normal DFE Delay
    ScalerSetBit(PB0_A6_CMU_06, ~_BIT4, 0x00);

    // [0] De_brank_en = 1'b0 --> Disable Blank Function
    ScalerSetBit(P7A_AF_L0_GDI_DE, ~_BIT0, 0x00);
    ScalerSetBit(P7A_BF_L1_GDI_DE, ~_BIT0, 0x00);
    ScalerSetBit(P7A_CF_L2_GDI_DE, ~_BIT0, 0x00);
    ScalerSetBit(P7A_DF_L3_GDI_DE, ~_BIT0, 0x00);

    // Disable Tap2 Delay
    ScalerSetBit(PB0_B2_EQ_00, ~_BIT0, 0x00);

    // [7:5] LE_lowgain_highboost = 3'b100 --> LE HighBoost Mode
    ScalerSetBit(PB0_A8_CMU_08, ~(_BIT7 | _BIT6 | _BIT5), _BIT7);

    // Set Digital PHY Sampling Edge
    ScalerSetBit(PB0_05_SAMPLE_EDGE, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);

    // en_wide_temp = 1'b1
    ScalerSetBit(PB0_AB_CDR_02, ~_BIT0, _BIT0);

    // [7:4] Enable Data Stream Signal Detection
    ScalerSetBit(PB0_BA_DP_SIG_DET_00, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));
}

//--------------------------------------------------
// Description  : Rebuilding DP PHY
// Input Value  : DPCD Bit Rate and Lane Count
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1RebuildPhy(BYTE ucDpLinkRate, BYTE ucDpcdLane)
{
    BYTE ucBestVCOBand = 0;
    WORD usVCOBandDiff = 0xFFFF;
    BYTE ucTemp = 0;

    g_ucDpRx1PhyCtsCtrl = 0x00;


    ///////////////////////////
    // RX CMU EN & Mac Power //
    ///////////////////////////

    // [7] cmu_en = 1'b0 --> Disable CMU PLL
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT7, 0x00);

    // Disable RX_EN, CDR_EN
    ScalerSetByte(PB0_A1_CMU_01, 0x00);

    switch(ucDpcdLane & 0x1F)
    {
        case _DP_ONE_LANE:

            ScalerSetBit(PB0_A1_CMU_01, ~_BIT4, _BIT4);

            // Enable Local Bias LPF
            ScalerSetBit(PB0_A7_CMU_07, ~_BIT5, _BIT5);

            for(pData[0] = 0; pData[0] <= 2; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            ScalerSetBit(PB0_A1_CMU_01, ~_BIT0, _BIT0);

            ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~(_BIT4 | _BIT3), _BIT3);

            break;

        case _DP_TWO_LANE:

            ScalerSetBit(PB0_A1_CMU_01, ~(_BIT5 | _BIT4), (_BIT5 | _BIT4));

            // Enable Local Bias LPF
            ScalerSetBit(PB0_A7_CMU_07, ~_BIT5, _BIT5);

            for(pData[0] = 0; pData[0] <= 2; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            ScalerSetBit(PB0_A1_CMU_01, ~(_BIT1 | _BIT0), _BIT0);
            ScalerSetBit(PB0_A1_CMU_01, ~(_BIT1 | _BIT0), (_BIT1 | _BIT0));

            ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~(_BIT4 | _BIT3), _BIT4);

            break;

        case _DP_FOUR_LANE:
        default:

            ScalerSetBit(PB0_A1_CMU_01, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));

            // Enable Local Bias LPF
            ScalerSetBit(PB0_A7_CMU_07, ~_BIT5, _BIT5);

            for(pData[0] = 0; pData[0] <= 2; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            ScalerSetBit(PB0_A1_CMU_01, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT0);
            ScalerSetBit(PB0_A1_CMU_01, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));
            ScalerSetBit(PB0_A1_CMU_01, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT2 | _BIT1 | _BIT0));
            ScalerSetBit(PB0_A1_CMU_01, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));

            ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));

            break;
    }


    /////////////////
    // CMU Setting //
    /////////////////

    switch(ucDpLinkRate)
    {
        case _DP_LINK_HBR2:

            // [7:5] rate_sel = 3'b000 --> 5.4GHz
            ScalerSetBit(PB0_DB_ST_0, ~(_BIT7 | _BIT6 | _BIT5), 0x00);

            // [7:5] SEL_DIV = 3'b000 --> 5.4GHz
            // [4:2] CMU_PI_I_SEL = 3'b011 --> 12.5uA
            // [1:0] CMU_SELPREDIV = 2'b00 --> /1
            ScalerSetByte(PB0_A3_CMU_03, 0x0C);

            // [7:4] CMU_SEL_PUMP_I = 4'b1100 --> Icp = 35uA
            // [3:1] CMU_SEL_RI = 3'b000 --> Rs = 6KOhm
            // [0] CMU_SEL_CP = 1'b0 --> Cp = 1pF
            ScalerSetByte(PB0_A4_CMU_04, 0xC0);

            // [5] CMU_BYPASS_PI = 1'b1 --> Integer N PLL
            ScalerSetBit(PB0_A5_CMU_05, ~_BIT5, _BIT5);

            // [7] BYPASS_PI = 1'b1 --> Use Integer N Code
            ScalerSetBit(PB0_F1_SDM_00, ~_BIT7, _BIT7);

            // Digital CDR Ref Clock = Analog CDR / 20
            ScalerSetBit(PB0_AE_CDR_05, ~_BIT0, 0x00);
            ScalerSetBit(PB0_AF_CDR_06, ~_BIT1, _BIT1);

            // [7:0] N_CODE = 8'b0001 0000 --> 16 + 4 = 20
            ScalerSetByte(PB0_F2_SDM_01, 0x10);

            break;

        case _DP_LINK_HBR:

            // [7:5] rate_sel = 3'b001 --> 2.7GHz
            ScalerSetBit(PB0_DB_ST_0, ~(_BIT7 | _BIT6 | _BIT5), _BIT5);

            // [7:5] SEL_DIV = 3'b001 --> 2.7GHz
            // [4:2] CMU_PI_I_SEL = 3'b011 --> 12.5uA
            // [1:0] CMU_SELPREDIV = 2'b00 --> /1
            ScalerSetByte(PB0_A3_CMU_03, 0x2C);

            // [7:4] CMU_SEL_PUMP_I = 4'b0101 --> Icp = 15uA
            // [3:1] CMU_SEL_RI = 3'b011 --> Rs = 12KOhm
            // [0] CMU_SEL_CP = 1'b0 --> Cp = 1pF
            ScalerSetByte(PB0_A4_CMU_04, 0x56);

            // [5] CMU_BYPASS_PI = 1'b0 --> Fractional N PLL
            ScalerSetBit(PB0_A5_CMU_05, ~_BIT5, 0x00);

            // [7] BYPASS_PI = 1'b0 --> Use Fractional N_F_CODE
            ScalerSetBit(PB0_F1_SDM_00, ~_BIT7, 0x00);

            ScalerDpRx1CMUNFCodeSetting(_DP_NF_REF_XTAL, _DP_LINK_HBR);

            break;

        case _DP_LINK_RBR:
        default:

            // [7:5] rate_sel = 3'b001 --> 1.62GHz
            ScalerSetBit(PB0_DB_ST_0, ~(_BIT7 | _BIT6 | _BIT5), _BIT5);

            // [7:5] SEL_DIV = 3'b001 --> 1.62GHz
            // [4:2] CMU_PI_I_SEL = 3'b011 --> 12.5uA
            // [1:0] CMU_SELPREDIV = 2'b00 --> /1
            ScalerSetByte(PB0_A3_CMU_03, 0x2C);

            // [7:4] CMU_SEL_PUMP_I = 4'b0101 --> Icp = 15uA
            // [3:1] CMU_SEL_RI = 3'b011 --> Rs = 12KOhm
            // [0] CMU_SEL_CP = 1'b0 --> Cp = 1pF
            ScalerSetByte(PB0_A4_CMU_04, 0x56);

            // [5] CMU_BYPASS_PI = 1'b0 --> Fractional N PLL
            ScalerSetBit(PB0_A5_CMU_05, ~_BIT5, 0x00);

            // [7] BYPASS_PI = 1'b0 --> Use Fractional N_F_CODE
            ScalerSetBit(PB0_F1_SDM_00, ~_BIT7, 0x00);

            ScalerDpRx1CMUNFCodeSetting(_DP_NF_REF_XTAL, _DP_LINK_RBR);

            break;
    }


    /////////////////////
    // A/D CDR Setting //
    /////////////////////

    // [3:0] KD = 4'b0000 --> KD = 0
    ScalerSetBit(PB0_CC_ERROR_COUNT_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);

    switch(ucDpLinkRate)
    {
        case _DP_LINK_HBR2:

            // [5:4] reg_cdr_kvco = 2'b01 --> Analog CDR KVCO = 550M/v
            ScalerSetBit(PB0_B9_LANE0_A_CDR_00, ~(_BIT5 | _BIT4), _BIT4);

            // [4:2] reg_sel_cdr_res = 3'b011 --> Analog CDR Rs = 4K
            ScalerSetBit(PB0_FD_LANE0_A_CDR_07, ~(_BIT4 | _BIT3 | _BIT2), (_BIT3 | _BIT2));

            // [3:0] reg_cdr_cp_in = 4'b0011 --> Analog CDR Icp = 10.5uA
            ScalerSetBit(PB0_FE_LANE0_A_CDR_08, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));

            // KI = 3'b000 --> KI = 0
            ScalerSetByte(PB0_D4_KI_0, 0x00);
            ScalerSetByte(PB0_D5_KI_1, 0x00);

            // KP = 0x07 --> KP = 1.75
            ScalerSetByte(PB0_D6_KP_0, 0x07);
            ScalerSetByte(PB0_D7_KP_1, 0x07);
            ScalerSetByte(PB0_D8_KP_2, 0x07);
            ScalerSetByte(PB0_D9_KP_3, 0x07);

            break;

        case _DP_LINK_HBR:

            // KI = 3'b010 --> KI = 1/8
            ScalerSetByte(PB0_D4_KI_0, 0x48);
            ScalerSetByte(PB0_D5_KI_1, 0x48);

            // KP = 0x1F --> KP = 7.75
            ScalerSetByte(PB0_D6_KP_0, 0x1F);
            ScalerSetByte(PB0_D7_KP_1, 0x1F);
            ScalerSetByte(PB0_D8_KP_2, 0x1F);
            ScalerSetByte(PB0_D9_KP_3, 0x1F);

            break;

        case _DP_LINK_RBR:
        default:

            // KI = 3'b010 --> KI = 1/8
            ScalerSetByte(PB0_D4_KI_0, 0x48);
            ScalerSetByte(PB0_D5_KI_1, 0x48);

            // KP = 0x37 --> KP = 13.75
            ScalerSetByte(PB0_D6_KP_0, 0x37);
            ScalerSetByte(PB0_D7_KP_1, 0x37);
            ScalerSetByte(PB0_D8_KP_2, 0x37);
            ScalerSetByte(PB0_D9_KP_3, 0x37);

            break;
    }


    /////////////////////
    // Rx Misc Setting //
    /////////////////////

    // Enable Comma Detection
    ScalerSetBit(PB0_05_SAMPLE_EDGE, ~_BIT4, 0x00);

    if((ucDpcdLane & _BIT7) == _BIT7)
    {
        // Enable Enhancement Control Mode --> MAC
        ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT2, _BIT2);
    }
    else
    {
        // Disable Enhancement Control Mode --> MAC
        ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT2, 0x00);
    }

    if((ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x02) & _BIT5) == _BIT5)
    {
        // Disable Scrambling
        ScalerSetBit(PB0_07_SCRAMBLE_CTRL, ~_BIT5, _BIT5);
    }
    else
    {
        // Enable Scrambling
        ScalerSetBit(PB0_07_SCRAMBLE_CTRL, ~_BIT5, 0x00);
    }

    // [0] en_clkout_manual = 0 --> Disable Clkout
    ScalerSetBit(PB0_F5_WD_01, ~_BIT4, 0x00);

    // [0] en_eqen_manual = 0 --> Disable Clkout
    ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);

    // [0] en_data_manual = 0 --> Disable Dataout
    ScalerSetBit(PB0_F7_DATA_TIMER, ~_BIT0, 0x00);

    // Disable Hybrid Mode
    ScalerSetBit(PB0_AF_CDR_06, ~_BIT0, 0x00);

    // [3:0] RST_N = 4'b0000 --> Digital Circuit Reset
    ScalerSetBit(PB0_DA_PI_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);

    // [3:0] RST_N = 4'b1111 --> Digital Circuit Normal
    ScalerSetBit(PB0_DA_PI_0, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));


    /////////////////
    // DFE Setting //
    /////////////////

    // Set DFE Initial
    ScalerDpRx1SetDFEInitial(ucDpLinkRate);

    switch(ucDpLinkRate)
    {
        case _DP_LINK_HBR2:

            // [7:5] LE_ISEL = 3'b011
            // [4:2] LE_RLSEL = 3'b000
            ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT6 | _BIT5));

            break;

        case _DP_LINK_HBR:

            // [7:5] LE_ISEL = 3'b010
            // [4:2] LE_RLSEL = 3'b001
            ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT6 | _BIT2));

            break;

        case _DP_LINK_RBR:
        default:

            // [7:5] LE_ISEL = 3'b000
            // [4:2] LE_RLSEL = 3'b011
            ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT3 | _BIT2));

            break;
    }


    //////////////
    // K Offset //
    //////////////

#if(_PCB_TYPE_FOR_DP_HBR2 == _DP_HBR2_AIO_PCB)
    if(ucDpLinkRate == _DP_LINK_HBR2)
    {
        // Enable Sumamp High Gain Mode
        ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, _BIT1);
    }
    else
#endif
    {
        // Disable Sumamp High Gain Mode
        ScalerSetBit(PB0_B0_CDR_07, ~_BIT1, 0x00);
    }

    // [2] Enable Foreground Calibration Auto Mode
    ScalerSetBit(PB0_AD_CDR_04, ~_BIT2, _BIT2);

    // [5] Toggle Foreground Calibration
    ScalerSetBit(PB0_AA_CDR_01, ~_BIT5, _BIT5);
    ScalerSetBit(PB0_AA_CDR_01, ~_BIT5, 0x00);
    ScalerSetBit(PB0_AA_CDR_01, ~_BIT5, _BIT5);

    // Waiting for K Offset
    for(pData[0] = 0; pData[0] <= 10; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }


    /////////////////////
    // Adaptation Loop //
    /////////////////////

    // [7] cmu_en = 1'b1 --> Enable CMU PLL
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT7, _BIT7);

    // Enable Tap1/2
    ScalerSetBit(PB0_B2_EQ_00, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT5 | _BIT4 | _BIT3 | _BIT2));

    if(ucDpLinkRate == _DP_LINK_HBR2)
    {
        // [4] reg_en_data_manual_l0 = 1'b0
        // [3] reg_bypass_data_rdy_l0 = 1'b0
        ScalerSetBit(PB0_FF_LANE0_A_CDR_09, ~(_BIT4 | _BIT3), 0x00);

        // [0] en_clkout_manual = 0 --> Disable Clkout
        ScalerSetBit(PB0_F5_WD_01, ~_BIT4, 0x00);

        // [5] reg_adp_eq_off = 1'b0 --> Enable Analog CDR DFE Adaptation
        ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT5, 0x00);

        // [4] reg_auto_mode = 1'b1 --> FLD Select Auto Mode
        ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT4, _BIT4);

        // [0] reg_acdr_l0_en = 1'b1 --> Enable Lane0 ACDR
        ScalerSetBit(PB0_FF_LANE0_A_CDR_09, ~_BIT0, _BIT0);

        // [6] reg_xtal_en = 1'b1 --> Using Xtal
        ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT6, _BIT6);

        // Analog CDR Auto FLD Counter
        ScalerSetByte(PB0_FA_LANE0_A_CDR_04, 0xF9);
        ScalerSetByte(PB0_FB_LANE0_A_CDR_05, 0x0C);
        ScalerSetByte(PB0_FC_LANE0_A_CDR_06, 0x85);

        // [0] en_clkout_manual = 1'b1 --> Enable Clkout
        ScalerSetBit(PB0_F5_WD_01, ~_BIT4, _BIT4);

        // [7] reg_en_ana_cdr = 1'b1 --> Enable Analog CDR
        // [6] reg_en_vco_lpf = 1'b0 --> Disable Analog CDR Multi Band VCO's LPF
        // [3] reg_cp_adj_en = 1'b1 --> Enable Adjust Icp When Tracking
        // [2] reg_enhance_bias_boost = 1'b0 --> Disable Enhance LEQ Boost Bias
        // [1] reg_enhance_bias_gain = 1'b0 --> Disable Enhance LEQ Gain Bias
        // [0] reg_rstb_fsm --> Toggle for FLD reset
        ScalerSetBit(PB0_B9_LANE0_A_CDR_00, ~(_BIT7 | _BIT6 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT7 | _BIT3));
        ScalerSetBit(PB0_B9_LANE0_A_CDR_00, ~_BIT0, _BIT0);

        // Waiting for FLD Lock Band
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // [3] reg_cp_en_manual = 1'b0 -->  Disable cp_en_manual
        // [2] reg_adp_en_manual = 1'b0 --> Disable adp_en_manual
        ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~(_BIT3 | _BIT2), 0x00);

        // [4] reg_auto_mode = 1'b0 --> FLD Manual Mode
        ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT4, 0x00);

        // [0] en_clkout_manual = 1'b1 --> Enable Clkout
        ScalerSetBit(PB0_F5_WD_01, ~_BIT4, _BIT4);

        // [2] reg_adp_en_manual = 1'b1 --> Enable adp_en_manual
        ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT2, _BIT2);

        ucBestVCOBand = ScalerGetByte(PB0_BF_LANE0_A_CDR_02);

        g_usDpRx1ClockCount = 0x2710; // ideal 5.4G

        for(ucTemp = 0; ucTemp <= 5; ucTemp++)
        {
            // Manually Reload FLD Band
            ScalerSetByte(PB0_BE_LANE0_A_CDR_01, (ScalerGetByte(PB0_BF_LANE0_A_CDR_02) - 2 + ucTemp));

            // Waiting for VCO Stable
            for(pData[0] = 0; pData[0] <= 2; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            // [5] ln_ck_sel = 1'b1 -> CDR Clock
            // [3:0] DP_XTAL_CYCLE = 4'b0011 -> XTAL Counter Cycles = 1000
            ScalerSetBit(PB0_50_DP_SIG_DET_0, ~(_BIT5 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT1 | _BIT0));

            // [1:0] freqdet_lane_sel = 2'b00 -> Lane0
            ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), 0x00);

            // [6] LANE_COUNT_CLEAR = 0 -> Keep the LANE_COUNT value
            ScalerSetBit(PB0_52_DP_SIG_DET_2, ~_BIT6, 0x00);

            // [7] DP_SIG_DET_EN = 1'b1 -> Enable Signal Detection
            ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);
            ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, _BIT7);

            // Waiting for Measure Done -> 50us
            for(pData[0] = 0; pData[0] <= 10; pData[0]++)
            {
                Delay5us(g_ucDelay5usN);
            }

            if(ScalerGetBit(PB0_50_DP_SIG_DET_0, _BIT6) == _BIT6)
            {
                g_usDpRx1ClockCount1 = ((((WORD)ScalerGetByte(PB0_53_DP_SIG_DET_3)) << 8) | ScalerGetByte(PB0_54_DP_SIG_DET_4));
            }
            else
            {
                ucBestVCOBand = ScalerGetByte(PB0_BF_LANE0_A_CDR_02);
                continue;
            }

            // [7] DP_SIG_DET_EN = 0 -> Disable Signal Detection
            ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);

            if((abs(g_usDpRx1ClockCount1 - g_usDpRx1ClockCount) < usVCOBandDiff) && (g_usDpRx1ClockCount1 >= g_usDpRx1ClockCount))
            {
                usVCOBandDiff = abs(g_usDpRx1ClockCount1 - g_usDpRx1ClockCount);
                ucBestVCOBand = (ScalerGetByte(PB0_BF_LANE0_A_CDR_02) - 2 + ucTemp);
            }
        }

        // Manually Reload FLD Band
        ScalerSetByte(PB0_BE_LANE0_A_CDR_01, ucBestVCOBand);

        // Waiting for VCO Stable
        for(pData[0] = 0; pData[0] <= 2; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // [3] reg_cp_en_manual = 1'b1 -->  Enaable cp_en_manual
        ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT3, _BIT3);

        // Enable Tap0 Adaptation
        ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x41);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Start to Record Tap0 Max/Min
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, _BIT0);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Stop To Record Tap0 Max/Min
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, 0x00);

        // Disable Vth/Tap0 Adaptation
        ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);

        // Read Back Lane0
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);

        // Read Back Tap0 Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), 0x00);

        g_pucDpRx1Tap0MaxTemp[0] = ScalerGetByte(P7A_F3_COEF_4);

        // Reload Max Value of Tap0 (Lane0)
        ScalerSetByte(P7A_A4_L0_INIT_2, g_pucDpRx1Tap0MaxTemp[0]);

        // Reload Tap0
        ScalerSetBit(P7A_AA_L0_INIT_8, ~_BIT0, _BIT0);
        ScalerSetBit(P7A_AA_L0_INIT_8, ~_BIT0, 0x00);

        // Enable LE Adaptation
        ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x80);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Start to Record LE Max/Min
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, _BIT0);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Stop To Record LE Max/Min
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, 0x00);

        // Disable Adaptation
        ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);

        // Read Back Lane0
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);

        // Read Back LE Min
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT3 | _BIT1));

        g_pucDpRx1LEMinTemp[0] = ScalerGetByte(P7A_F3_COEF_4);

        // Read Back LE Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), _BIT3);

        g_pucDpRx1LEMaxTemp[0] = ScalerGetByte(P7A_F3_COEF_4);

        pData[0] = 0;

        if(g_pucDpRx1LEMaxTemp[pData[0]] > g_pucDpRx1LEMinTemp[pData[0]])
        {
            if(g_pucDpRx1LEMinTemp[pData[0]] < 10)
            {
                g_pucDpRx1LEMinTemp[pData[0]] = g_pucDpRx1LEMinTemp[pData[0]] + 2;
            }
            else if(g_pucDpRx1LEMinTemp[pData[0]] < 12)
            {
                g_pucDpRx1LEMinTemp[pData[0]]++;
            }
        }

        // Reload LE to Min
        ScalerSetBit(P7A_A2_L0_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMinTemp[0] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
        ScalerSetBit(P7A_A3_L0_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[0] << 2) & _BIT6));

        // Reload LE
        ScalerSetBit(P7A_AA_L0_INIT_8, ~_BIT7, _BIT7);
        ScalerSetBit(P7A_AA_L0_INIT_8, ~_BIT7, 0x00);

        // [0] en_clkout_manual = 0 --> Disable Clkout
        ScalerSetBit(PB0_F5_WD_01, ~_BIT4, 0x00);

        // Enable Hybrid Mode
        ScalerSetBit(PB0_AF_CDR_06, ~_BIT0, _BIT0);

        // Enable Vth/Tap0 Adaptation
        ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x41);
        ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x41);
        ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x41);

        // Reset Digital CDR
        ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);
        ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

        // Waiting for CMU PLL Stable
        for(pData[0] = 0; pData[0] <= 2; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // [0] en_clkout_manual = 1 --> Enable Clkout
        ScalerSetBit(PB0_F5_WD_01, ~_BIT4, _BIT4);

        // [0] en_eqen_manual = 1 --> Enable Adapation
        ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Start to Record Tap0 Max/Min
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, _BIT0);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Stop To Record Tap0 Max/Min
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, 0x00);

        // Disable All Adapation Loop
        ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x00);
        ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x00);
        ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x00);

        // Read Back Lane1
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);

        // Read Back Tap0 Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), 0x00);

        g_pucDpRx1Tap0MaxTemp[1] = ScalerGetByte(P7A_F3_COEF_4);

        // Read Back Lane2
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);

        // Read Back Tap0 Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), 0x00);

        g_pucDpRx1Tap0MaxTemp[2] = ScalerGetByte(P7A_F3_COEF_4);

        // Read Back Lane3
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));

        // Read Back Tap0 Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), 0x00);

        g_pucDpRx1Tap0MaxTemp[3] = ScalerGetByte(P7A_F3_COEF_4);

        // Reload Max Value of Tap0 (Lane1)
        ScalerSetByte(P7A_B4_L1_INIT_2, g_pucDpRx1Tap0MaxTemp[1]);

        // Reload Max Value of Tap0 (Lane2)
        ScalerSetByte(P7A_C4_L2_INIT_2, g_pucDpRx1Tap0MaxTemp[2]);

        // Reload Max Value of Tap0 (Lane3)
        ScalerSetByte(P7A_D4_L3_INIT_2, g_pucDpRx1Tap0MaxTemp[3]);

        // Reload Tap0 Max
        ScalerSetBit(P7A_BA_L1_INIT_8, ~_BIT0, _BIT0);
        ScalerSetBit(P7A_CA_L2_INIT_8, ~_BIT0, _BIT0);
        ScalerSetBit(P7A_DA_L3_INIT_8, ~_BIT0, _BIT0);
        ScalerSetBit(P7A_BA_L1_INIT_8, ~_BIT0, 0x00);
        ScalerSetBit(P7A_CA_L2_INIT_8, ~_BIT0, 0x00);
        ScalerSetBit(P7A_DA_L3_INIT_8, ~_BIT0, 0x00);

        // Enable LE Adaptation
        ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x80);
        ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x80);
        ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x80);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Start to Record Tap0 Max/Min
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, _BIT0);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Stop To Record Tap0 Max/Min
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, 0x00);

        // Disable All Adapation Loop
        ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x00);
        ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x00);
        ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x00);

        // Read Back Lane1
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);

        // Read Back LE Min
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT3 | _BIT1));

        g_pucDpRx1LEMinTemp[1] = ScalerGetByte(P7A_F3_COEF_4);

        // Read Back LE Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), _BIT3);

        g_pucDpRx1LEMaxTemp[1] = ScalerGetByte(P7A_F3_COEF_4);

        // Read Back Lane2
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);

        // Read Back LE Min
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT3 | _BIT1));

        g_pucDpRx1LEMinTemp[2] = ScalerGetByte(P7A_F3_COEF_4);

        // Read Back LE Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), _BIT3);

        g_pucDpRx1LEMaxTemp[2] = ScalerGetByte(P7A_F3_COEF_4);

        // Read Back Lane3
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));

        // Read Back LE Min
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT3 | _BIT1));

        g_pucDpRx1LEMinTemp[3] = ScalerGetByte(P7A_F3_COEF_4);

        // Read Back LE Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), _BIT3);

        g_pucDpRx1LEMaxTemp[3] = ScalerGetByte(P7A_F3_COEF_4);

        for(pData[0] = 1; pData[0] <= 3; pData[0]++)
        {
            if(abs(g_pucDpRx1LEMaxTemp[pData[0]] - g_pucDpRx1LEMinTemp[pData[0]]) == 1)
            {
                if(g_pucDpRx1LEMinTemp[pData[0]] < 10)
                {
                    g_pucDpRx1LEMinTemp[pData[0]] = g_pucDpRx1LEMinTemp[pData[0]] + 2;
                }
                else if(g_pucDpRx1LEMinTemp[pData[0]] < 13)
                {
                    g_pucDpRx1LEMinTemp[pData[0]]++;
                }
            }
            else if(abs(g_pucDpRx1LEMaxTemp[pData[0]] - g_pucDpRx1LEMinTemp[pData[0]]) > 1)
            {
                if(g_pucDpRx1LEMinTemp[pData[0]] < 12)
                {
                    g_pucDpRx1LEMinTemp[pData[0]] = g_pucDpRx1LEMinTemp[pData[0]] + 2;
                }
                else if(g_pucDpRx1LEMinTemp[pData[0]] < 13)
                {
                    g_pucDpRx1LEMinTemp[pData[0]]++;
                }
            }
        }

        // Reload LE to Min
        ScalerSetBit(P7A_B2_L1_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMinTemp[1] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
        ScalerSetBit(P7A_B3_L1_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[1] << 2) & _BIT6));

        // Reload LE to Min
        ScalerSetBit(P7A_C2_L2_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMinTemp[2] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
        ScalerSetBit(P7A_C3_L2_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[2] << 2) & _BIT6));

        // Reload LE to Min
        ScalerSetBit(P7A_D2_L3_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMinTemp[3] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
        ScalerSetBit(P7A_D3_L3_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[3] << 2) & _BIT6));

        // Reload LE
        ScalerSetBit(P7A_BA_L1_INIT_8, ~_BIT7, _BIT7);
        ScalerSetBit(P7A_CA_L2_INIT_8, ~_BIT7, _BIT7);
        ScalerSetBit(P7A_DA_L3_INIT_8, ~_BIT7, _BIT7);
        ScalerSetBit(P7A_BA_L1_INIT_8, ~_BIT7, 0x00);
        ScalerSetBit(P7A_CA_L2_INIT_8, ~_BIT7, 0x00);
        ScalerSetBit(P7A_DA_L3_INIT_8, ~_BIT7, 0x00);

        // [5] servo_notrans = 1'b0 --> Servo is Effective Each Bit
        // [4] tap0_notrans = 1'b0 --> Tap0 is Effective Each Bit
        ScalerSetBit(P7A_F8_GRAY_DEC_2, ~(_BIT5 | _BIT4), 0x00);

        // Enable Tap0/1 Adapt
        ScalerSetBit(P7A_A1_L0_DFE_EN_2, ~(_BIT1 | _BIT0), (_BIT1 | _BIT0));
        ScalerSetBit(P7A_B1_L1_DFE_EN_2, ~(_BIT1 | _BIT0), (_BIT1 | _BIT0));
        ScalerSetBit(P7A_C1_L2_DFE_EN_2, ~(_BIT1 | _BIT0), (_BIT1 | _BIT0));
        ScalerSetBit(P7A_D1_L3_DFE_EN_2, ~(_BIT1 | _BIT0), (_BIT1 | _BIT0));

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Start to Record DFE Coef.
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, _BIT0);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 20; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Stop to Record DFE Coef.
        ScalerSetBit(P7A_F2_COEF_3, ~_BIT0, 0x00);

        // Disable Tap0/1 Adapt
        ScalerSetBit(P7A_A1_L0_DFE_EN_2, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_B1_L1_DFE_EN_2, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_C1_L2_DFE_EN_2, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_D1_L3_DFE_EN_2, ~(_BIT1 | _BIT0), 0x00);


        ///////////
        // Lane0 //
        ///////////

        // Read Back Lane0
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), 0x00);

        // Read Back Tap1 Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), _BIT2);

        g_pucDpRx1Tap1MaxTemp[0] = ScalerGetByte(P7A_F3_COEF_4);


        ///////////
        // Lane1 //
        ///////////

        // Read Back Lane1
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT6);

        // Read Back Tap1 Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), _BIT2);

        g_pucDpRx1Tap1MaxTemp[1] = ScalerGetByte(P7A_F3_COEF_4);


        ///////////
        // Lane2 //
        ///////////

        // Read Back Lane2
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), _BIT7);

        // Read Back Tap1 Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), _BIT2);

        g_pucDpRx1Tap1MaxTemp[2] = ScalerGetByte(P7A_F3_COEF_4);


        ///////////
        // Lane3 //
        ///////////

        // Read Back Lane3
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));

        // Read Back Tap1 Max
        ScalerSetBit(P7A_F2_COEF_3, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), _BIT2);

        g_pucDpRx1Tap1MaxTemp[3] = ScalerGetByte(P7A_F3_COEF_4);

        for(pData[0] = 0; pData[0] <= 3; pData[0]++)
        {
            if(g_pucDpRx1Tap1MaxTemp[pData[0]] > 10)
            {
                g_pucDpRx1Tap1MaxTemp[pData[0]] = 0x06;

                if(g_pucDpRx1LEMinTemp[pData[0]] < 15)
                {
                    g_pucDpRx1LEMinTemp[pData[0]]++;
                }
            }
        }

        // Reload LE
        if(ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x03) == 0x06)
        {
            ScalerSetBit(P7A_A2_L0_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMaxTemp[0] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_A3_L0_INIT_1, ~_BIT6, ((g_pucDpRx1LEMaxTemp[0] << 2) & _BIT6));
        }
        else
        {
            ScalerSetBit(P7A_A2_L0_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMinTemp[0] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_A3_L0_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[0] << 2) & _BIT6));
        }

        if(ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x04) == 0x06)
        {
            ScalerSetBit(P7A_B2_L1_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMaxTemp[1] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_B3_L1_INIT_1, ~_BIT6, ((g_pucDpRx1LEMaxTemp[1] << 2) & _BIT6));
        }
        else
        {
            ScalerSetBit(P7A_B2_L1_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMinTemp[1] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_B3_L1_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[1] << 2) & _BIT6));
        }

        if(ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x05) == 0x06)
        {
            ScalerSetBit(P7A_C2_L2_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMaxTemp[2] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_C3_L2_INIT_1, ~_BIT6, ((g_pucDpRx1LEMaxTemp[2] << 2) & _BIT6));
        }
        else
        {
            ScalerSetBit(P7A_C2_L2_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMinTemp[2] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_C3_L2_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[2] << 2) & _BIT6));
        }

        if(ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x06) == 0x06)
        {
            ScalerSetBit(P7A_D2_L3_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMaxTemp[3] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_D3_L3_INIT_1, ~_BIT6, ((g_pucDpRx1LEMaxTemp[3] << 2) & _BIT6));
        }
        else
        {
            ScalerSetBit(P7A_D2_L3_LIMIT_INIT, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (g_pucDpRx1LEMinTemp[3] & (_BIT3 | _BIT2 | _BIT1 | _BIT0)));
            ScalerSetBit(P7A_D3_L3_INIT_1, ~_BIT6, ((g_pucDpRx1LEMinTemp[3] << 2) & _BIT6));
        }

        // Reload Max Value of Tap0
        ScalerSetByte(P7A_A4_L0_INIT_2, g_pucDpRx1Tap0MaxTemp[0]);
        ScalerSetByte(P7A_B4_L1_INIT_2, g_pucDpRx1Tap0MaxTemp[1]);
        ScalerSetByte(P7A_C4_L2_INIT_2, g_pucDpRx1Tap0MaxTemp[2]);
        ScalerSetByte(P7A_D4_L3_INIT_2, g_pucDpRx1Tap0MaxTemp[3]);

        // Reload Tap1
        ScalerSetByte(P7A_A5_L0_INIT_3, g_pucDpRx1Tap1MaxTemp[0]);
        ScalerSetByte(P7A_B5_L1_INIT_3, g_pucDpRx1Tap1MaxTemp[1]);
        ScalerSetByte(P7A_C5_L2_INIT_3, g_pucDpRx1Tap1MaxTemp[2]);
        ScalerSetByte(P7A_D5_L3_INIT_3, g_pucDpRx1Tap1MaxTemp[3]);

        // Load Tap0/Tap1
        ScalerSetBit(P7A_AA_L0_INIT_8, ~(_BIT7 | _BIT1 | _BIT0), (_BIT7 | _BIT1 | _BIT0));
        ScalerSetBit(P7A_BA_L1_INIT_8, ~(_BIT7 | _BIT1 | _BIT0), (_BIT7 | _BIT1 | _BIT0));
        ScalerSetBit(P7A_CA_L2_INIT_8, ~(_BIT7 | _BIT1 | _BIT0), (_BIT7 | _BIT1 | _BIT0));
        ScalerSetBit(P7A_DA_L3_INIT_8, ~(_BIT7 | _BIT1 | _BIT0), (_BIT7 | _BIT1 | _BIT0));
        ScalerSetBit(P7A_AA_L0_INIT_8, ~(_BIT7 | _BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_BA_L1_INIT_8, ~(_BIT7 | _BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_CA_L2_INIT_8, ~(_BIT7 | _BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_DA_L3_INIT_8, ~(_BIT7 | _BIT1 | _BIT0), 0x00);

        // [4] transition_only = 1 --> Each Bit is Effective
        ScalerSetBit(P7A_E0_MODE_TIMER, ~_BIT4, 0x00);

        // Enable Tap 3/4 Adapt
        ScalerSetBit(P7A_A1_L0_DFE_EN_2, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));
        ScalerSetBit(P7A_B1_L1_DFE_EN_2, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));
        ScalerSetBit(P7A_C1_L2_DFE_EN_2, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));
        ScalerSetBit(P7A_D1_L3_DFE_EN_2, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));

        // Waiting for DFE Adapt
        for(pData[0] = 0; pData[0] <= 10; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // [0] reg_en_eqen_manual = 1'b0 --> Disable Analog DFE Adapt
        ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, 0x00);
    }
    else
    {
        // [7] reg_en_ana_cdr = 1'b0 --> Disable Analog CDR
        ScalerSetBit(PB0_B9_LANE0_A_CDR_00, ~_BIT7, 0x00);

        // [0] reg_acdr_l0_en = 1'b0 --> Disable Lane0 ACDR
        ScalerSetBit(PB0_FF_LANE0_A_CDR_09, ~_BIT0, 0x00);

        // Enable Tap0/Vth/LE/Tap1/Tap2
        ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
        ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0xC7);
        ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0xC7);
        ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0xC7);
        ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0xC7);

        // Reset Digital CDR
        ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x00);
        ScalerSetByte(PB0_C1_TMDS_CONT_1, 0x78);

        // Waiting for CMU PLL Stable
        for(pData[0] = 0; pData[0] <= 2; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // [0] en_clkout_manual = 1 --> Enable Clkout
        ScalerSetBit(PB0_F5_WD_01, ~_BIT4, _BIT4);

        // [0] en_eqen_manual = 1 --> Enable Adapation
        ScalerSetBit(PB0_F6_EQEN_TIMER, ~_BIT0, _BIT0);

        // Waiting for DFE Adaptation
        for(pData[0] = 0; pData[0] <= 40; pData[0]++)
        {
            Delay5us(g_ucDelay5usN);
        }

        // Enable Vth/Tap0 Adaptation
        ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x41);
        ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x41);
        ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x41);
        ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x41);

        // Set Vth Min = 0
        ScalerSetBit(P7A_F9_LIMIT_10, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
    }

    // [0] en_data_manual = 1 --> Enable Dataout
    ScalerSetBit(PB0_F7_DATA_TIMER, ~_BIT0, _BIT0);

    if(ScalerGetBit(PB0_FF_LANE0_A_CDR_09, _BIT0) == _BIT0)
    {
        // [5] reg_adp_eq_off = 1'b1 --> Disable Adaptation for Analog CDR
        ScalerSetBit(PB0_F9_LANE0_A_CDR_03, ~_BIT5, _BIT5);

        // [4] reg_en_data_manual_l0 = 1'b1
        // [3] reg_bypass_data_rdy_l0 = 1'b1
        ScalerSetBit(PB0_FF_LANE0_A_CDR_09, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));
    }

    // DP Mac Clock Select to Link Clock
    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~_BIT6, 0x00);
}

//--------------------------------------------------
// Description  : DP PHY DFE Initial Setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1SetDFEInitial(EnumDpLinkRate enumDpLinkRate)
{
    enumDpLinkRate = enumDpLinkRate;

    // [7] Positive Symbol-Rate Clock
    // [6] Enable Timer Control DFE Loop
    // [5:4] Reduce Gain to 1/4
    // [3] Disable Running Length Detection
    // [2] bypass Blank out Case-3 Symbol
    ScalerSetBit(P7A_A0_L0_DFE_EN_1, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT7 | _BIT6 | _BIT5));
    ScalerSetBit(P7A_B0_L1_DFE_EN_1, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT7 | _BIT6 | _BIT5));
    ScalerSetBit(P7A_C0_L2_DFE_EN_1, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT7 | _BIT6 | _BIT5));
    ScalerSetBit(P7A_D0_L3_DFE_EN_1, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT7 | _BIT6 | _BIT5));

    // [7:6] adapt_mode[1:0] = 2'b10 --> Amplitude-based Adaptation with Independent LE Loop(Loop Mode 2)
    // [5] edge_det_mode = 1 --> Edge Lag Data
    // [4] transition_only = 1 --> Only Transition Bit is Effective
    // [3] le_auto_reload = 0 --> Disable Auto-reload LE Coefficient
    // [2:0] tap1_delay[2:0] = 3'b010 --> Tap1+LE Delay 2us after Tap0
    ScalerSetByte(P7A_E0_MODE_TIMER, 0xB2);

    // Set DFE Delay Time
    ScalerSetByte(P7A_E1_TIMER, 0x01);

    // Tap0,Tap1 Loop Gain 1/512
    ScalerSetBit(P7A_E2_GAIN_1, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT5 | _BIT2));

    // Tap2,Tap3 Loop Gain 1/512
    ScalerSetBit(P7A_E3_GAIN_2, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT5 | _BIT2));

    // Tap4,Servo Loop Gain 1/512
    ScalerSetBit(P7A_E4_GAIN_3, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT5 | _BIT3));

    // LE Gain1 Loop Gain 1/256, LE Gain2 Loop Gain 0, LE Pre-Courser Gain 0
    ScalerSetBit(P7A_E5_GAIN_4, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1), (_BIT6 | _BIT4 | _BIT3));
    ScalerSetBit(P7A_FA_GAIN_PREC, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT2 | _BIT1));

    // Tap5,Tap6 Loop Gain 1/512
    ScalerSetBit(P7A_F7_GAIN_5, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT5 | _BIT2));

    // Tap0 Max is 21
    ScalerSetByte(P7A_E6_LIMIT_1, 0x35);

    // Tap1 Min is 31
    ScalerSetBit(P7A_E7_LIMIT_2, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0));

    // Tap2 Max is 6, Min is -6
    ScalerSetBit(P7A_E8_LIMIT_3, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), (_BIT5 | _BIT4));
    ScalerSetBit(P7A_E8_LIMIT_3, ~(_BIT2 | _BIT1 | _BIT0), _BIT0);
    ScalerSetBit(P7A_E9_LIMIT_4, ~(_BIT7 | _BIT6), _BIT7);

    // Tap3 Max is 0, Min is -3
    ScalerSetBit(P7A_E9_LIMIT_4, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_EA_LIMIT_5, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), (_BIT4 | _BIT3));

    // Tap4 Max is 0, Min is -3
    ScalerSetBit(P7A_EA_LIMIT_5, ~(_BIT2 | _BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_EB_LIMIT_6, ~(_BIT7 | _BIT6), 0x00);
    ScalerSetBit(P7A_EB_LIMIT_6, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));

    // Tap0 Threshold
    ScalerSetBit(P7A_EF_RESET_1, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT5));

    if(ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) == 0x00)
    {
        // Set Vth Max = 6, Min = 0
        ScalerSetByte(P7A_F9_LIMIT_10, 0x60);
    }
    else if(enumDpLinkRate == _DP_LINK_HBR2)
    {
        // Set Vth Max = 6, Min = 4
        ScalerSetByte(P7A_F9_LIMIT_10, 0x64);
    }
    else
    {
        // Set Vth Max = 6, Min = 0
        ScalerSetByte(P7A_F9_LIMIT_10, 0x62);
    }

    // Set Vth/Servo-loop/Tap Divider
    ScalerSetByte(P7A_EC_LOOP_DIV_1, 0xA3);

    // Tap Divider = 10 for TP2
    ScalerSetByte(P7A_ED_LOOP_DIV_2, 0xCA);

    // [5] servo_notrans = 1'b1 --> Servo is Effective when Non-Tran
    // [4] tap0_notrans = 1'b1 --> Tap0 is Effective when Non-Tran
    ScalerSetBit(P7A_F8_GRAY_DEC_2, ~(_BIT5 | _BIT4), (_BIT5 | _BIT4));


    /////////////////////////
    // DFE Code Initialize //
    /////////////////////////

    if(enumDpLinkRate == _DP_LINK_HBR2)
    {
        // LE Max = 15, Initial = 12, Servo Initial = 16
        ScalerSetByte(P7A_A2_L0_LIMIT_INIT, 0xFC);
        ScalerSetByte(P7A_B2_L1_LIMIT_INIT, 0xFC);
        ScalerSetByte(P7A_C2_L2_LIMIT_INIT, 0xFC);
        ScalerSetByte(P7A_D2_L3_LIMIT_INIT, 0xFC);
        ScalerSetByte(P7A_A3_L0_INIT_1, 0x10);
        ScalerSetByte(P7A_B3_L1_INIT_1, 0x10);
        ScalerSetByte(P7A_C3_L2_INIT_1, 0x10);
        ScalerSetByte(P7A_D3_L3_INIT_1, 0x10);
    }
    else
    {
        // LE Max = 31, Initial = 12, Servo Initial = 16
        ScalerSetByte(P7A_A2_L0_LIMIT_INIT, 0xFC);
        ScalerSetByte(P7A_B2_L1_LIMIT_INIT, 0xFC);
        ScalerSetByte(P7A_C2_L2_LIMIT_INIT, 0xFC);
        ScalerSetByte(P7A_D2_L3_LIMIT_INIT, 0xFC);
        ScalerSetByte(P7A_A3_L0_INIT_1, 0x90);
        ScalerSetByte(P7A_B3_L1_INIT_1, 0x90);
        ScalerSetByte(P7A_C3_L2_INIT_1, 0x90);
        ScalerSetByte(P7A_D3_L3_INIT_1, 0x90);
    }

    // Tap0 Initial Value = 12
    ScalerSetByte(P7A_A4_L0_INIT_2, 0x0C);
    ScalerSetByte(P7A_B4_L1_INIT_2, 0x0C);
    ScalerSetByte(P7A_C4_L2_INIT_2, 0x0C);
    ScalerSetByte(P7A_D4_L3_INIT_2, 0x0C);

    // Tap1 Initial Value = 0
    ScalerSetByte(P7A_A5_L0_INIT_3, 0x00);
    ScalerSetByte(P7A_B5_L1_INIT_3, 0x00);
    ScalerSetByte(P7A_C5_L2_INIT_3, 0x00);
    ScalerSetByte(P7A_D5_L3_INIT_3, 0x00);

    // Tap2 Initial Value = 0
    ScalerSetByte(P7A_A6_L0_INIT_4, 0x00);
    ScalerSetByte(P7A_B6_L1_INIT_4, 0x00);
    ScalerSetByte(P7A_C6_L2_INIT_4, 0x00);
    ScalerSetByte(P7A_D6_L3_INIT_4, 0x00);

    // Vth Initial Value = 4
    ScalerSetByte(P7A_A9_L0_INIT_7, 0x04);
    ScalerSetByte(P7A_B9_L1_INIT_7, 0x04);
    ScalerSetByte(P7A_C9_L2_INIT_7, 0x04);
    ScalerSetByte(P7A_D9_L3_INIT_7, 0x04);

    // Load Intial DFE Code
    ScalerSetByte(P7A_AA_L0_INIT_8, 0xFF);
    ScalerSetByte(P7A_BA_L1_INIT_8, 0xFF);
    ScalerSetByte(P7A_CA_L2_INIT_8, 0xFF);
    ScalerSetByte(P7A_DA_L3_INIT_8, 0xFF);
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT5 | _BIT4), (_BIT5 | _BIT4));
    ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT5 | _BIT4), (_BIT5 | _BIT4));
    ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT5 | _BIT4), (_BIT5 | _BIT4));
    ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT5 | _BIT4), (_BIT5 | _BIT4));

    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
    ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
    ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
    ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT5 | _BIT4), 0x00);
    ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT5 | _BIT4), 0x00);
    ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT5 | _BIT4), 0x00);
    ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT5 | _BIT4), 0x00);

    // DFE Loop Normal
    ScalerSetBit(P7A_EF_RESET_1, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_EF_RESET_1, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));

    // Disable All Adaptation Loop
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);
    ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x00);
    ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x00);
    ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x00);
}

//--------------------------------------------------
// Description  : Initial Setting for DP Digital Aux, Analog Aux, Aux INT
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1AuxIntInitial(void)
{
    // Digital Aux Power On, DPCD REG Power On, MSG Power On.
    ScalerSetBit(PBA_C0_DPCD_CTRL, ~(_BIT7 | _BIT6 | _BIT5 | _BIT1 | _BIT0), 0x00);

    // Average 8 cycles as Start Postion, Aux Clk Select to 27MHz, no Swap
    ScalerSetByte(PBA_72_AUX_DIG_PHY2, 0xC0);

    // Aux Clk Select Manual Mode, Aux New Mode
    ScalerSetBit(PBA_73_AUX_DIG_PHY3, ~(_BIT7 | _BIT6), _BIT6);

    // HW Fake Link Training Disable.
    ScalerSetBit(PBA_C6_AUX_PHY_DIG2, ~_BIT4, 0x00);

    // Set Aux Precharge Number.
    ScalerSetBit(PBA_74_AUX_DIG_PHY4, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT0));

    // Aux Auto Mode, Using Default Average Number.
    ScalerSetByte(PBA_75_AUX_DIG_PHY5, 0x0D);

    // Enble Aux Channel.
    ScalerSetBit(PBA_D0_AUX_MODE_SET, ~(_BIT3 | _BIT2 | _BIT0), _BIT0);

    // Fast IIC Clock.
    ScalerSetByte(PBA_D1_DP_IIC_SET, 0x02);

    // Enable 068xxx INT
    ScalerSetBit(PBA_DA_AUX_FIFO_RST, ~(_BIT3 | _BIT2), (_BIT3 | _BIT2));

    // Not Reply when Aux Error.
    ScalerSetBit(PBA_DB_AUX_STATUS, ~(_BIT5 | _BIT4), (_BIT5 | _BIT4));
    ScalerSetBit(PBA_76_AUX_DIG_PHY6, ~(_BIT7 | _BIT6), _BIT6);

    // Disable other DPCD, Aux Timeout, Receiving Aux INT.
    ScalerSetBit(PBA_DC_AUX_IRQ_STATUS, ~(_BIT5 | _BIT3 | _BIT2), 0x00);

    // Disable Aux Phy Int
    ScalerSetBit(PBA_77_AUX_DIG_PHY7, ~_BIT7, 0x00);

    // Enable DPCD INT
    ScalerSetByte(PBA_DE_AUX_DPCD_IRQ_EN, 0xE5);

    // Aux Ack Timer.
    ScalerSetBit(PBA_F0_AUX_TX_TIMER, ~(_BIT5 | _BIT2), (_BIT5 | _BIT2));

    // Set Timeout Target
    ScalerSetBit(PBA_F1_AUX_TX_TIMER_2, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT0));

    // Disable Reply IIC Defer Before Data Ready.
    ScalerSetBit(PBA_F4_MCUIIC, ~(_BIT6 | _BIT5), 0x00);

    // Enable Aux Error Handler.
    ScalerSetBit(PBA_78_AUX_DIG_PHY8, ~(_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT4));

    // Toggle Aux Auto K
    ScalerSetBit(PBA_65_AUX_5, ~_BIT7, 0x00);
    ScalerSetBit(PBA_65_AUX_5, ~_BIT7, _BIT7);

#if(_DP_AUX_DIFF_MODE == _OFF)
    // Set VLDO, AUX ADJR, Rx common mode from 1.2V
    ScalerSetByte(PBA_61_AUX_1, 0xF0);

    // Aux 50ohm auto K, Aux Vth-->50mV, Disable Enlarge Rx Termination R
    ScalerSetByte(PBA_62_AUX_2, 0x29);
    ScalerSetBit(PBA_66_AUX_6, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT4);
#else
    // Set VLDO, AUX ADJR, Rx common mode from 1.2V
    ScalerSetByte(PBA_61_AUX_1, 0xFA);

    // Aux 50ohm auto K, Aux Vth-->50mV, Disable Enlarge Rx Termination R
    ScalerSetByte(PBA_62_AUX_2, 0x09);

    ScalerSetBit(PBA_66_AUX_6, ~(_BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT4 | _BIT3 | _BIT1));
#endif

    // Set Aux Talk Mode ADJR
    ScalerSetByte(PBA_67_DIG_TX_03, 0xFF);

    ScalerSetBit(PBA_60_DIG_TX_04, ~(_BIT1 | _BIT0), _BIT0);

    // Aux comm current select max
    ScalerSetBit(PBA_64_AUX_4, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));

    // Set Aux D_2 = ~D_1 for Aux Single Ended Mode
    ScalerSetBit(PBA_79_AUX_DIG_PHY9, ~_BIT7, _BIT7);

    // Internal IRQ Enable (Global)
    ScalerGlobalIRQControl(_ENABLE);

    // Aux ps clk from IOSC
    ScalerSetBit(P0_0B_POWER_CTRL, ~(_BIT5 | _BIT4), 0x00);

    if(ScalerGlobalGetIDCode() <= _RL6193_VER_C_ID_CODE)
    {
        // Clear 2002 INT Status Flag
        ScalerSetBit(PBA_FB_DUMMY_1, ~_BIT7, _BIT7);

        // Enable DPCD 0x2002 INT
        ScalerSetBit(PBA_FB_DUMMY_1, ~(_BIT7 | _BIT6), _BIT6);
    }

    // Enable INT0
    EX0 = 1;
}

//--------------------------------------------------
// Description  : Check Valid Lane CDR
// Input Value  : None
// Output Value : True --> CDR Lock
//--------------------------------------------------
bit ScalerDpRx1CDRCheck(BYTE ucDpLinkRate, BYTE ucDpcdLane)
{
    if(ucDpLinkRate <= _DP_LINK_HBR)
    {
        return _TRUE;
    }

    switch(ucDpcdLane)
    {
        case _DP_ONE_LANE:

            if(ScalerDpRx1MeasureLaneCDRClk(ucDpLinkRate, _D1_DP_LANE0) == _TRUE)
            {
                return _TRUE;
            }

            break;

        case _DP_TWO_LANE:

            if(ScalerDpRx1MeasureLaneCDRClk(ucDpLinkRate, _D1_DP_LANE0) == _TRUE)
            {
                return _TRUE;
            }

            break;

        case _DP_FOUR_LANE:
        default:

            if(ScalerDpRx1MeasureLaneCDRClk(ucDpLinkRate, _D1_DP_LANE0) == _TRUE)
            {
                return _TRUE;
            }

            break;
    }

    DebugMessageRx1("7. wuzanne test : DP CDR Unlock", 0);

    return _FALSE;
}

//--------------------------------------------------
// Description : Initial Signal check
// Input Value  : DP Link Rate
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1SignalDetectInitial(BYTE ucDpLinkRate)
{
    ///////////////////////////
    // RX CMU EN & Mac Power //
    ///////////////////////////

    // [7] cmu_en = 1'b0 --> Disable CMU PLL
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT7, 0x00);

    // Disable RX_EN, CDR_EN
    ScalerSetByte(PB0_A1_CMU_01, 0x00);

    ScalerSetBit(PB0_A1_CMU_01, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT7 | _BIT6 | _BIT5 | _BIT4));

    // Enable Local Bias LPF
    ScalerSetBit(PB0_A7_CMU_07, ~_BIT5, _BIT5);

    for(pData[0] = 0; pData[0] <= 2; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    ScalerSetBit(PB0_A1_CMU_01, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), _BIT0);
    ScalerSetBit(PB0_A1_CMU_01, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT1 | _BIT0));
    ScalerSetBit(PB0_A1_CMU_01, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT2 | _BIT1 | _BIT0));
    ScalerSetBit(PB0_A1_CMU_01, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));

    ScalerSetBit(PB0_01_PHY_DIG_RESET_CTRL, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));


    ////////////////
    // LE Setting //
    ////////////////

    // [7] Positive Symbol-Rate Clock
    ScalerSetBit(P7A_A0_L0_DFE_EN_1, ~_BIT7, _BIT7);
    ScalerSetBit(P7A_B0_L1_DFE_EN_1, ~_BIT7, _BIT7);
    ScalerSetBit(P7A_C0_L2_DFE_EN_1, ~_BIT7, _BIT7);
    ScalerSetBit(P7A_D0_L3_DFE_EN_1, ~_BIT7, _BIT7);

    // [7:6] adapt_mode[1:0] = 2'b10 --> Amplitude-based Adaptation with Independent LE Loop(Loop Mode 2)
    // [5] edge_det_mode = 1 --> Edge Lag Data
    // [4] transition_only = 1 --> Only Transition Bit is Effective
    // [3] le_auto_reload = 0 --> Disable Auto-reload LE Coefficient
    // [2:0] tap1_delay[2:0] = 3'b010 --> Tap1+LE Delay 2us after Tap0
    ScalerSetByte(P7A_E0_MODE_TIMER, 0xB2);

    // LE Max = 31, Initial = 18, Servo Initial = 16
    ScalerSetByte(P7A_A2_L0_LIMIT_INIT, 0xF2);
    ScalerSetByte(P7A_B2_L1_LIMIT_INIT, 0xF2);
    ScalerSetByte(P7A_C2_L2_LIMIT_INIT, 0xF2);
    ScalerSetByte(P7A_D2_L3_LIMIT_INIT, 0xF2);
    ScalerSetByte(P7A_A3_L0_INIT_1, 0xD0);
    ScalerSetByte(P7A_B3_L1_INIT_1, 0xD0);
    ScalerSetByte(P7A_C3_L2_INIT_1, 0xD0);
    ScalerSetByte(P7A_D3_L3_INIT_1, 0xD0);

    // Load Initial LE Code
    ScalerSetByte(P7A_AA_L0_INIT_8, 0x80);
    ScalerSetByte(P7A_BA_L1_INIT_8, 0x80);
    ScalerSetByte(P7A_CA_L2_INIT_8, 0x80);
    ScalerSetByte(P7A_DA_L3_INIT_8, 0x80);

    ScalerSetByte(P7A_AA_L0_INIT_8, 0x00);
    ScalerSetByte(P7A_BA_L1_INIT_8, 0x00);
    ScalerSetByte(P7A_CA_L2_INIT_8, 0x00);
    ScalerSetByte(P7A_DA_L3_INIT_8, 0x00);

    // DFE Loop Normal
    ScalerSetBit(P7A_EF_RESET_1, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_EF_RESET_1, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT3 | _BIT2 | _BIT1 | _BIT0));

    // Disable All Adaptation Loop
    ScalerSetBit(P7A_AC_L0_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_BC_L1_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_CC_L2_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetBit(P7A_DC_L3_DFE_EN_3, ~(_BIT1 | _BIT0), 0x00);
    ScalerSetByte(P7A_A1_L0_DFE_EN_2, 0x00);
    ScalerSetByte(P7A_B1_L1_DFE_EN_2, 0x00);
    ScalerSetByte(P7A_C1_L2_DFE_EN_2, 0x00);
    ScalerSetByte(P7A_D1_L3_DFE_EN_2, 0x00);

    switch(ucDpLinkRate)
    {
        case _DP_LINK_HBR2:

            // [7:5] LE_ISEL = 3'b011
            // [4:2] LE_RLSEL = 3'b000
            ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT6 | _BIT5));

            break;

        case _DP_LINK_HBR:

            // [7:5] LE_ISEL = 3'b010
            // [4:2] LE_RLSEL = 3'b001
            ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT6 | _BIT2));

            break;

        case _DP_LINK_RBR:
        default:

            // [7:5] LE_ISEL = 3'b000
            // [4:2] LE_RLSEL = 3'b011
            ScalerSetBit(PB0_A9_CMU_09, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2), (_BIT3 | _BIT2));

            break;
    }
}

//--------------------------------------------------
// Description : check DP CDR locked or unlocked
// Input Value  : DP Lane
// Output Value : True --> locked; False --> unlocked
//--------------------------------------------------
bit ScalerDpRx1MeasureLaneCDRClk(BYTE ucDpLinkRate, BYTE ucDpRx1LaneNumber)
{
    WORD usCDRClockCount = 0;
    ucDpLinkRate = ucDpLinkRate;

    if((abs(g_usDpRx1ClockCount - 0x2710)) > (0x2710 >> 4))
    {
        return _TRUE;
    }

    // Select Measure CDR Clock & XTAL 27M Count 1000 Times
    ScalerSetByte(PB0_50_DP_SIG_DET_0, 0x23);

    // Select CDR Lane
    ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), ucDpRx1LaneNumber);

    // [6] LANE_COUNT_CLEAR = 0 -> Keep the LANE_COUNT value
    ScalerSetBit(PB0_52_DP_SIG_DET_2, ~_BIT6, 0x00);

    // Toggle Start Measure
    ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);
    ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, _BIT7);

    // Waiting for Measure Done -> 50us
    for(pData[0] = 0; pData[0] <= 10; pData[0]++)
    {
        Delay5us(g_ucDelay5usN);
    }

    if(ScalerGetBit(PB0_50_DP_SIG_DET_0, _BIT6) == 0x00)
    {
        return _FALSE;
    }

    usCDRClockCount = ((((WORD)ScalerGetByte(PB0_53_DP_SIG_DET_3)) << 8) | ScalerGetByte(PB0_54_DP_SIG_DET_4));

    if(abs(g_usDpRx1ClockCount - usCDRClockCount) > (g_usDpRx1ClockCount >> 7))
    {
        return _FALSE;
    }

    return _TRUE;
}

//--------------------------------------------------
// Description  : DP Hdcp Capable Modify(Useless in RL6193)
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1HdcpCapableModify(void)
{
}

#if(_DP_TX_SUPPORT == _ON)
//--------------------------------------------------
// Description  : DP Port Source/Sync Scanning and Setting
// Input Value  : None
// Output Value : Source DP --> DP Sync Stable
//                Source None --> No Stable Sync
//--------------------------------------------------
BYTE ScalerDpRx1MstToSstScanInputPort(void)
{
    if(ScalerDpRx1MstToSstGetVideoStream() == _FALSE)
    {
        return _FALSE;
    }

#if(_DP_FREESYNC_SUPPORT == _ON)
    ScalerDpTxAuxNativeRead(0x00, 0x01, 0x07, 1, pData);

    if((pData[0] & _BIT7) == _BIT7)
    {
        SET_DP_MST_FREESYNC_ENABLED();
    }
    else
    {
        CLR_DP_MST_FREESYNC_ENABLED();
    }
#endif

    if(ScalerDpRx1MstToSstMSACheck() == _FALSE)
    {
        return _FALSE;
    }

    if(ScalerDpRx1MstToSstGetVideoInfo() == _FALSE)
    {
        return _FALSE;
    }
    else
    {
        ScalerDpRx1MstToSstSetColorInfo();
    }

    if(ScalerDpRx1MstToSstDisplayFormatSetting() == _FALSE)
    {
        return _FALSE;
    }

    if(ScalerDpRx1MstToSstStreamClkRegenerate() == _FALSE)
    {
        return _FALSE;
    }

    if(ScalerDpRx1MstToSstFifoCheck(_DP_FIFO_DELAY_CHECK) == _FALSE)
    {
        ScalerDpRx1MstToSstAdjustVsyncDelay();

        if(ScalerDpRx1MstToSstFifoCheck(_DP_FIFO_DELAY_CHECK) == _FALSE)
        {
            return _FALSE;
        }
    }

    // DP MST2SST RGB Output Enable
    ScalerSetBit(P4A_36_DP_OUTPUT_CTRL, ~(_BIT2 | _BIT1 | _BIT0), (_BIT2 | _BIT1 | _BIT0));

    // DP MST2SST CLK Output Enable
    ScalerSetBit(P4A_B8_MN_SCLKG_SDM_CTRL, ~_BIT6, _BIT6);

    // Enable Under/OverFlow WatchDog
    SET_REG_DP_RX_MST2SST_WATCH_DOG_ENABLE();

    return _TRUE;
}

//--------------------------------------------------
// Description  : Check Dp Video Straam VBID
// Input Value  : None
// Output Value : True --> Get Video Stream
//--------------------------------------------------
bit ScalerDpRx1MstToSstGetVideoStream(void)
{
    if(ScalerGetBit(P4A_29_MST2SST_VBID, _BIT3) == 0x00)
    {
        return _TRUE;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : Check Dp MST2SSST MSA info.
// Input Value  : None
// Output Value : True --> Get MSA
//--------------------------------------------------
bit ScalerDpRx1MstToSstMSACheck(void)
{
    // Pop up Main Stream Attributes
    ScalerSetBit(P4A_00_MST2SST_RESET, ~_BIT3, _BIT3);

    // Get Hactive
    ScalerRead(P4A_14_MSA_HWD_0, 2, &pData[4], _AUTOINC);

    // Get Vactive
    ScalerRead(P4A_1C_MSA_VHT_0, 2, &pData[12], _AUTOINC);

#if(_DP_FREESYNC_SUPPORT == _ON)
    if(GET_DP_MST_FREESYNC_ENABLED() == _TRUE)
    {
        if((PDATA_WORD(2) == 0x00) || (PDATA_WORD(6) == 0x00))
        {
            return _FALSE;
        }
    }
    else
#endif
    {
        // Get Htotal
        ScalerRead(P4A_10_MSA_HTT_0, 2, &pData[0], _AUTOINC);

        // Get Hstart
        ScalerRead(P4A_12_MSA_HST_0, 2, &pData[2], _AUTOINC);

        // Get HSW
        ScalerRead(P4A_16_MSA_HSW_0, 2, &pData[6], _AUTOINC);

        // Get Vtotal
        ScalerRead(P4A_18_MSA_VTT_0, 2, &pData[8], _AUTOINC);

        // Get Vstart
        ScalerRead(P4A_1A_MSA_VST_0, 2, &pData[10], _AUTOINC);

        // Get VSW
        ScalerRead(P4A_1E_MSA_VSW_0, 2, &pData[14], _AUTOINC);

        if((PDATA_WORD(0) == 0x00) || (PDATA_WORD(1) == 0x00) || (PDATA_WORD(2) == 0x00) || ((PDATA_WORD(3) & 0x7FFF) == 0x00) ||
           (PDATA_WORD(4) == 0x00) || (PDATA_WORD(5) == 0x00) || (PDATA_WORD(6) == 0x00) || ((PDATA_WORD(7) & 0x7FFF) == 0x00))
        {
            return _FALSE;
        }
    }

    // Get Mvid
    ScalerRead(P4A_20_MSA_MVID_00, 3, pData, _AUTOINC);

    if((((DWORD)pData[0] << 16) | ((DWORD)pData[1] << 8) | ((DWORD)pData[2] << 0)) == 0x00)
    {
        return _FALSE;
    }

    // Get Nvid
    ScalerRead(P4A_23_MSA_NVID_00, 3, pData, _AUTOINC);

    if((((DWORD)pData[0] << 16) | ((DWORD)pData[1] << 8) | ((DWORD)pData[2] << 0)) == 0x00)
    {
        return _FALSE;
    }

    return _TRUE;
}

//--------------------------------------------------
// Description  : Check DP MSA Vactive / Hactive Change
// Input Value  : None
// Output Value : True --> DP Vactive / Hactive Information Check Ok
//--------------------------------------------------
bit ScalerDpRx1MstToSstMSAActiveChange(void)
{
    // Pop up Main Stream Attributes
    ScalerSetBit(P4A_00_MST2SST_RESET, ~_BIT3, _BIT3);

    // Get Display Format Hactive
    ScalerRead(P4A_44_MN_DPF_HWD_M, 2, &pData[0], _AUTOINC);

    // Get MSA Hactive
    ScalerRead(P4A_14_MSA_HWD_0, 2, &pData[2], _AUTOINC);

    // Get Display Format Vactive
    ScalerRead(P4A_4C_MN_DPF_VHT_M, 2, &pData[4], _AUTOINC);

    // Get MSA Vactive
    ScalerRead(P4A_1C_MSA_VHT_0, 2, &pData[6], _AUTOINC);

    if((PDATA_WORD(0) != PDATA_WORD(1)) || (PDATA_WORD(2) != PDATA_WORD(3)))
    {
        return _TRUE;
    }

    return _FALSE;
}

//--------------------------------------------------
// Description  : DP MST2SST Get Video Info
// Input Value  : None
// Output Value : _TRUE --> Get Successfully
//--------------------------------------------------
bit ScalerDpRx1MstToSstGetVideoInfo(void)
{
#if(_DP_VSC_SDP_EXT_FOR_COLORIMETRY_SUPPORT == _ON)

    CLR_DP_TX_VSC_SDP_COLOR_MODE();

    // Check MISC1[6]
    if((ScalerGetBit(P4A_27_MSA_MISC_1, _BIT6)) == _BIT6)
    {
        SET_DP_TX_VSC_SDP_COLOR_MODE();

#if(_HW_DP_VSC_SDP_REG_SUPPORT == _ON)

        // Read DB16~18 --> pData[0:2]
        ScalerRead(PBC_59_VSC_RX_DB16, 3, pData, _AUTOINC);
#else
        // Delay 2 frames for SRAM ready (worst case)
        ScalerTimerDelayXms(_DP_ONE_FRAME_TIME_MAX * 2);

        SET_DP_RX1_MST2SST_RSV1_DETECTING();

        ScalerTimerActiveTimerEvent((_DP_ONE_FRAME_TIME_MAX * 3), _SCALER_TIMER_EVENT_DP_RX1_MST2SST_RSV1_PACKET_DETECT);

        // Read DB16~18 --> pData[0:2]
        ScalerGetDataPortByte(PBC_55_DP_INFO_FM_ADR, _DP_INFO_RSV1_DB16, 3, pData, _AUTOINC);
#endif
        // Set Color Info PreValue
        g_stDpTxColorInfo.ucColorSpacePreValue = (pData[0] & (_BIT7 | _BIT6 | _BIT5 | _BIT4));
        g_stDpTxColorInfo.b3ColorDepthPreValue = (pData[1] & (_BIT2 | _BIT1 | _BIT0));
    }
    else
#endif
    {
        // Get MISC0 Info
        pData[0] = ScalerGetByte(P4A_26_MSA_MISC_0);

        // Set Color Info PreValue
        g_stDpTxColorInfo.ucColorSpacePreValue = ((ScalerGetBit(P4A_27_MSA_MISC_1, _BIT7)) | (pData[0] & (_BIT2 | _BIT1)));
        g_stDpTxColorInfo.b3ColorDepthPreValue = ((pData[0] & (_BIT7 | _BIT6 | _BIT5)) >> 5);
    }

    return _TRUE;
}

//--------------------------------------------------
// Description  : DP MST2SST Video Setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1MstToSstVideoSetting(void)
{
    // Set Color Space for Display Format Gen.
    switch(GET_DP_TX_INPUT_COLOR_SPACE())
    {
        case _COLOR_SPACE_RGB:
            ScalerSetBit(P4A_30_PG_CTRL_0, ~(_BIT4 | _BIT3), 0x00);
            break;

        case _COLOR_SPACE_YCBCR444:
            ScalerSetBit(P4A_30_PG_CTRL_0, ~(_BIT4 | _BIT3), _BIT4);
            break;

        case _COLOR_SPACE_YCBCR422:
            ScalerSetBit(P4A_30_PG_CTRL_0, ~(_BIT4 | _BIT3), _BIT3);
            break;

        case _COLOR_SPACE_YCBCR420:
            ScalerSetBit(P4A_30_PG_CTRL_0, ~(_BIT4 | _BIT3), 0x00);
            break;

        case _COLOR_SPACE_Y_ONLY:
            ScalerSetBit(P4A_30_PG_CTRL_0, ~(_BIT4 | _BIT3), (_BIT4 | _BIT3));
            break;

        case _COLOR_SPACE_RAW:
            ScalerSetBit(P4A_30_PG_CTRL_0, ~(_BIT4 | _BIT3), 0x00);
            break;

        default:
            // RGB
            ScalerSetBit(P4A_30_PG_CTRL_0, ~(_BIT4 | _BIT3), 0x00);
            break;
    }

    // Set Color depth for Display Format Gen.
    ScalerSetBit(P4A_30_PG_CTRL_0, ~(_BIT2 | _BIT1 | _BIT0), (g_stDpTxColorInfo.b3ColorDepthPreValue));
}

//--------------------------------------------------
// Description  : Check DP MST2SST Misc Information
// Input Value  : None
// Output Value : True --> DP Misc Check Ok
//--------------------------------------------------
bit ScalerDpRx1MstToSstMiscCheck(void)
{
#if(_DP_VSC_SDP_EXT_FOR_COLORIMETRY_SUPPORT == _ON)
    // Check MISC1[6]
    if((ScalerGetBit(P4A_27_MSA_MISC_1, _BIT6)) == _BIT6)
    {
        return ScalerDpRx1MstToSstVscCheck();
    }
    else
#endif
    {
        // Get MISC0 Info
        BYTE ucMisc0 = ScalerGetByte(P4A_26_MSA_MISC_0);
        BYTE ucColorSpaceValue = (ScalerGetBit(P4A_27_MSA_MISC_1, _BIT7)) | (ucMisc0 & (_BIT2 | _BIT1));

        // Color Space Check
#if(_DP_VSC_SDP_EXT_FOR_COLORIMETRY_SUPPORT == _ON)

        // Color format info source change from VSC --> MISC
        if(GET_DP_TX_VSC_SDP_COLOR_MODE() == _TRUE)
        {
            switch(ucColorSpaceValue)
            {
                case _BIT1:
                    if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_YCBCR422)
                    {
                        return _FALSE;
                    }
                    break;

                case _BIT2:
                    if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_YCBCR444)
                    {
                        return _FALSE;
                    }
                    break;

                case _BIT7:
                    if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_Y_ONLY)
                    {
                        return _FALSE;
                    }
                    break;

                case (_BIT7 | _BIT1):
                    if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_RAW)
                    {
                        return _FALSE;
                    }
                    break;

                default:
                    if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_RGB)
                    {
                        return _FALSE;
                    }
                    break;
            }
        }
        else
#endif
        {
            if(ucColorSpaceValue != g_stDpTxColorInfo.ucColorSpacePreValue)
            {
                return _FALSE;
            }
        }

        // Color Depth Check
        if(ScalerGetBit(P4A_28_MSA_COLOR_BIT, (_BIT2 | _BIT1 | _BIT0)) != (g_stDpTxColorInfo.b3ColorDepthPreValue))
        {
            return _FALSE;
        }

        // Get HSW
        ScalerRead(P4A_16_MSA_HSW_0, 2, pData, _AUTOINC);

        // Get VSW
        ScalerRead(P4A_1E_MSA_VSW_0, 2, &pData[2], _AUTOINC);

        if(((PDATA_WORD(0) & 0x7FFF) == 0x00) || ((PDATA_WORD(1) & 0x7FFF) == 0x00))
        {
            return _FALSE;
        }

        return _TRUE;
    }
}

#if(_DP_VSC_SDP_EXT_FOR_COLORIMETRY_SUPPORT == _ON)
//--------------------------------------------------
// Description  : Check DP MST2SST VSC Information
// Input Value  : None
// Output Value : True --> DP VSC Check Ok
//--------------------------------------------------
bit ScalerDpRx1MstToSstVscCheck(void)
{
    BYTE pucVscColorFormat[3];

    memset(pucVscColorFormat, 0, sizeof(pucVscColorFormat));

#if(_HW_DP_VSC_SDP_REG_SUPPORT == _ON)

    // Read DB16~18 --> pucVscColorFormat[0:2]
    ScalerRead(PBC_59_VSC_RX_DB16, 3, pucVscColorFormat, _AUTOINC);
#else
    if(GET_DP_RX1_MST2SST_RSV1_DETECTING() == _FALSE)
    {
        // Read DB16~18 --> pucVscColorFormat[0:2]
        ScalerGetDataPortByte(PBC_55_DP_INFO_FM_ADR, _DP_INFO_RSV1_DB16, 3, pucVscColorFormat, _AUTOINC);

        SET_DP_RX1_MST2SST_RSV1_DETECTING();

        ScalerTimerActiveTimerEvent((_DP_ONE_FRAME_TIME_MAX * 3), _SCALER_TIMER_EVENT_DP_RX1_MST2SST_RSV1_PACKET_DETECT);
    }
    else
    {
        return _TRUE;
    }
#endif

    // Color Space Check
    if(GET_DP_TX_VSC_SDP_COLOR_MODE() == _FALSE)
    {
        // Color format info source change from MISC --> VSC
        switch(pucVscColorFormat[0] & (_BIT7 | _BIT6 | _BIT5 | _BIT4))
        {
            case _VSC_COLOR_SPACE_0:
                if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_RGB)
                {
                    return _FALSE;
                }

                break;

            case _VSC_COLOR_SPACE_1:
                if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_YCBCR444)
                {
                    return _FALSE;
                }

                break;

            case _VSC_COLOR_SPACE_2:
                if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_YCBCR422)
                {
                    return _FALSE;
                }

                break;

            case _VSC_COLOR_SPACE_3:
                if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_YCBCR420)
                {
                    return _FALSE;
                }

                break;

            case _VSC_COLOR_SPACE_4:
                if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_Y_ONLY)
                {
                    return _FALSE;
                }

                break;

            case _VSC_COLOR_SPACE_5:
                if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_RAW)
                {
                    return _FALSE;
                }

                break;

            default:
                if(GET_DP_TX_INPUT_COLOR_SPACE() != _COLOR_SPACE_RGB)
                {
                    return _FALSE;
                }

                break;
        }
    }
    else
    {
        if((pucVscColorFormat[0] & (_BIT7 | _BIT6 | _BIT5 | _BIT4)) != g_stDpTxColorInfo.ucColorSpacePreValue)
        {
            return _FALSE;
        }
    }

    // Color Depth Check
    if((pucVscColorFormat[1] & (_BIT2 | _BIT1 | _BIT0)) != (ScalerGetBit(P4A_30_PG_CTRL_0, (_BIT2 | _BIT1 | _BIT0))))
    {
        return _FALSE;
    }

    return _TRUE;
}
#endif // End of #if(_DP_VSC_SDP_EXT_FOR_COLORIMETRY_SUPPORT == _ON)

//--------------------------------------------------
// Description  : DP MST2SST Stream Clk PLL Setting
// Input Value  : None
// Output Value : True --> Stream CLK Regen. ok
//--------------------------------------------------
bit ScalerDpRx1MstToSstStreamClkRegenerate(void)
{
    DWORD ulStreamClkValue = 0;
    SDWORD lOffsetValue = 0;
    DWORD ulpllm = 0;
    DWORD ulXtal_clk = 0;
    WORD usDpHtotal = 0;
    WORD usDpVtotal = 0;
    DWORD ulVBs2BsCount = 0;
    DWORD ulDpNvidValue = 0;
    BYTE ucpllo = 0;
    BYTE ucdivs = 0;
    BYTE ucdiv2 = 0;
    BYTE ucplln = 0;
    BYTE ucBuf = 0;
    BYTE ucdiv_pllo = 0;
    BYTE ucdiv_divs = 0;

#if(_DP_FREESYNC_SUPPORT == _ON)
    DWORD ulDpMvidValue = 0;
#endif

    // PLL Input Clock select to Link Clock, Output Clock Select to PLL Clock, Double Buffer.
    ScalerSetBit(P4A_A0_MN_SCLKG_CTRL, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_BIT6 | _BIT4));

    ulDpNvidValue = ((((DWORD)ScalerGetByte(P4A_23_MSA_NVID_00)) << 16) | (((DWORD)ScalerGetByte(P4A_24_MSA_NVID_01)) << 8) | ((DWORD)ScalerGetByte(P4A_25_MSA_NVID_02)));

    usDpHtotal = ((((WORD)ScalerGetByte(P4A_40_MN_DPF_HTT_M)) << 8) | ((WORD)ScalerGetByte(P4A_41_MN_DPF_HTT_L)));

    usDpVtotal = ((((WORD)ScalerGetByte(P4A_48_MN_DPF_VTT_M)) << 8) | ((WORD)ScalerGetByte(P4A_49_MN_DPF_VTT_L)));

    // [5] ln_ck_sel = 1'b1 -> CDR Clock
    // [3:0] DP_XTAL_CYCLE = 4'b0100 -> XTAL Counter Cycles = 2000
    ScalerSetBit(PB0_50_DP_SIG_DET_0, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT2));

    // [1:0] freqdet_lane_sel = 2'b00 -> Lane0
    ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), _D1_DP_LANE0);

    // [6] LANE_COUNT_CLEAR = 0 -> Keep the LANE_COUNT value
    ScalerSetBit(PB0_52_DP_SIG_DET_2, ~_BIT6, 0x00);

    // [7] DP_SIG_DET_EN = 1'b1 -> Enable Signal Detection
    ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);
    ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, _BIT7);

    // Waiting for Measure Done
    ScalerTimerDelayXms(2);

    if(ScalerGetBit(PB0_50_DP_SIG_DET_0, _BIT6) == _BIT6)
    {
        PDATA_WORD(0) = ((((WORD)ScalerGetByte(PB0_53_DP_SIG_DET_3)) << 8) | ScalerGetByte(PB0_54_DP_SIG_DET_4));
    }
    else
    {
        if(ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) == _DP_LINK_HBR2)
        {
            PDATA_WORD(0) = 0x4E20;
        }
        else if(ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) == _DP_LINK_HBR)
        {
            PDATA_WORD(0) = 0x2710;
        }
        else // Both Default && _DP_LINK_RBR Case
        {
            PDATA_WORD(0) = 0x1770;
        }
    }

    // [7] DP_SIG_DET_EN = 0 -> Disable Signal Detection
    ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);

    PDATA_DWORD(0) = ((DWORD)PDATA_WORD(0)) * _GDI_CLK_KHZ;

    ulXtal_clk = (PDATA_DWORD(0) / 1000);

    // Measurement Enable
    ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT7, 0x00);
    ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT7, _BIT7);

    if(ScalerTimerPollingFlagProc(_DP_MEASURE_POLLING_TIMEOUT, P4A_58_MN_MEAS_CTRL, _BIT6, _TRUE) == _FALSE)
    {
        return _FALSE;
    }

    // Pop up The result
    ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT5, _BIT5);

    ulVBs2BsCount = ((((DWORD)(ScalerGetByte(P4A_58_MN_MEAS_CTRL) & 0x1F)) << 16) | (((DWORD)ScalerGetByte(P4A_59_MN_MEAS_VLN_M)) << 8) | ((DWORD)ScalerGetByte(P4A_5A_MN_MEAS_VLN_L)));

#if(_DP_FREESYNC_SUPPORT == _ON)
    if(GET_DP_MST_FREESYNC_ENABLED() == _TRUE)
    {
        ulDpMvidValue = ((((DWORD)ScalerGetByte(P4A_20_MSA_MVID_00)) << 16) | (((DWORD)ScalerGetByte(P4A_21_MSA_MVID_01)) << 8) | ((DWORD)ScalerGetByte(P4A_22_MSA_MVID_02)));

        // Calculate streamclk
        ulStreamClkValue = ((DWORD)ulDpMvidValue * (PDATA_DWORD(0) / ulDpNvidValue) + (PDATA_DWORD(0) % ulDpNvidValue * 100 / ulDpNvidValue * (DWORD)ulDpMvidValue / 100));
    }
    else
#endif
    {
        // Calculate streamclk (Unit is Hz)
        ulStreamClkValue = (((((DWORD)usDpHtotal) * ((DWORD)usDpVtotal) * 100) / ulVBs2BsCount) * _GDI_CLK_KHZ * 10);
    }

    SET_DP_TX_INPUT_PIXEL_CLK(ulStreamClkValue / 100000);

    ucplln = ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00);

    // Get DPLL parameter
    // If "600Mhz > ulStreamClkValue > 270Mhz"
    if((600000000 > ulStreamClkValue) && (ulStreamClkValue >= 270000000))
    {
        ucpllo = 0;
        ucdivs = 0;

        ucdiv_divs = 1;
        ucdiv_pllo = 1;

        ulpllm = ((((ulStreamClkValue / 4) * ucdiv_divs * ucdiv_pllo) * ucplln / 250 + (ulXtal_clk / 4)) / (ulXtal_clk));

        if(ulpllm > 257)
        {
            return _FALSE;
        }
    }
    // If "270MHz > ulStreamClkValue > 4.21875Mhz"
    else if((ulStreamClkValue < 270000000) && (ulStreamClkValue >= 4218750))
    {
        ucpllo = 1;

        if((ulStreamClkValue < 270000000) && (ulStreamClkValue >= 135000000))
        {
            ucdivs = 0;
        }
        else if((ulStreamClkValue < 135000000) && (ulStreamClkValue >= 67500000))
        {
            ucdivs = 1;
        }
        else if((ulStreamClkValue < 67500000) && (ulStreamClkValue >= 33750000))
        {
            ucdivs = 2;
        }
        else if((ulStreamClkValue < 33750000) && (ulStreamClkValue >= 16875000))
        {
            ucdivs = 4;
        }
        else if((ulStreamClkValue < 16875000) && (ulStreamClkValue >= 8437500))
        {
            ucdivs = 8;
        }
        else
        {
            ucdivs = 16;
        }

        // Because set 17 get minimum error for recovery stream clk , close to fact clk.
        if(ucdivs == 0)
        {
            ucdiv_divs = 1;
        }
        else
        {
            ucdiv_divs = 2 * ucdivs;
        }

        ucdiv_pllo = 1;

        if(ucpllo != 0)
        {
            ucdiv_pllo = 1;

            for(ucBuf = 0; ucBuf < ucpllo; ucBuf++)
            {
                ucdiv_pllo = 2 * ucdiv_pllo;
            }
        }

        ulpllm = ((((ulStreamClkValue / 4) * ucdiv_divs * ucdiv_pllo) * ucplln / 250 + (ulXtal_clk / 4)) / (ulXtal_clk));

        if(ulpllm > 257)
        {
            return _FALSE;
        }
    }
    else
    {
        return _FALSE;
    }

    ulpllm++;

    // offset = (m/n*f_ref - f)/(m/n*f_ref)*2^21
    // seperate *2^21 to *32*16384*4
    lOffsetValue = ((SDWORD)((ulXtal_clk * 1000 / ucplln) * ulpllm) - (SDWORD)(ulStreamClkValue * ucdiv_pllo * ucdiv_divs)) * 32 / ulpllm * ucplln / ulXtal_clk * 16384 / 1000 * 4;

    lOffsetValue = 0x7FFFF - lOffsetValue + 1;

    // DPPLL Power Down
    ScalerSetBit(P4A_BD_MN_SCLKG_PLL_PWR, ~_BIT7, _BIT7);

    /*
    // Disable M/N Tracking
    ScalerSetBit(P4A_A7_MN_SCLKG_TRK_CTRL, ~_BIT7, 0x00);
    */

    // Disable HSync Tracking
    ScalerSetBit(P4A_A7_MN_SCLKG_TRK_CTRL, ~_BIT6, _BIT6);

    // Set VCOMD, phase Swallow Circuit Clock Select, SCLKGSTOP
    ScalerSetBit(P4A_BF_MN_SCLKG_PLL_WD, ~(0x7F), 0x16);

    // Set DPPLL Div_2 & Div_s
    ScalerSetByte(P4A_A3_MN_SCLKG_DIVS, (((ucdiv2) << 7) | ((ucdivs) << 0)));

    // Set DPPLL Div_O.
    ScalerSetBit(P4A_BD_MN_SCLKG_PLL_PWR, ~(_BIT1 | _BIT0), ucpllo);

    // Offset Value Setting
    ScalerSetByte(P4A_A6_MN_SCLKG_OFFS_L, (BYTE)lOffsetValue);
    ScalerSetByte(P4A_A5_MN_SCLKG_OFFS_M, (BYTE)(lOffsetValue >> 8));
    ScalerSetBit(P4A_A4_MN_SCLKG_OFFS_H, ~(_BIT2 | _BIT1 | _BIT0), ((BYTE)(lOffsetValue >> 16)) & (_BIT2 | _BIT1 | _BIT0));

    // Enable phase swallow
    ScalerSetBit(P4A_B8_MN_SCLKG_SDM_CTRL, ~(_BIT5 | _BIT4), 0x00);
    ScalerSetBit(P4A_B8_MN_SCLKG_SDM_CTRL, ~(_BIT5 | _BIT4), _BIT4);

    // Set DPPLL M Code
    ScalerSetBit(P4A_A0_MN_SCLKG_CTRL, ~(_BIT1 | _BIT0), (((ulpllm - 2) >> 8) & 0x03));
    ScalerSetByte(P4A_A1_MN_SCLKG_DIVM, ((ulpllm - 2) & 0xFF));

    // Set DPPLL N Code
    ScalerSetByte(P4A_A2_MN_SCLKG_DIVN, (ucplln - 2));

    // Load offset value
    ScalerSetBit(P4A_A4_MN_SCLKG_OFFS_H, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4 | _BIT3), _BIT7);

    // Double Buffer for M/N Tracking
    ScalerSetBit(P4A_A0_MN_SCLKG_CTRL, ~_BIT4, _BIT4);

    // DPLL VCORSTB Set to Normal
    ScalerSetBit(P4A_C0_MN_SCLKG_PLL_INSEL, ~_BIT0, _DPLL_VCORSTB_NORMAL_VALUE);

    // DPLL Power On
    ScalerSetBit(P4A_BD_MN_SCLKG_PLL_PWR, ~_BIT7, 0x00);

    ScalerTimerDelayXms(2);

    ////////////////////////////////
    // DPPLL VCO Band Calibration //
    ////////////////////////////////

    // Reg DPLL_CMPEN
    ScalerSetBit(P4A_BD_MN_SCLKG_PLL_PWR, ~_BIT2, _BIT2);

    ScalerTimerDelayXms(2);

    // Reg DPLL_CALLCH
    ScalerSetBit(P4A_BD_MN_SCLKG_PLL_PWR, ~_BIT3, _BIT3);

    ScalerTimerDelayXms(2);

    // Reg DPLL_CALSW
    ScalerSetBit(P4A_BD_MN_SCLKG_PLL_PWR, ~_BIT4, _BIT4);

    // VCO Band : 01 ---> KVCO = 1340
    // VCO Band : 10 ---> KVCO = 1880
    // VCO Band : 11 ---> KVCO = 2340
    // R : 13
    // Icp = 6.28 * 700 * M / KVCO / R
    // Icp   = 2.5u + D0[0] * 2.5u + D0[1] * 5u + D0[2] * 10u (A)
    // Icp = 2.5uA, 5uA, 7.5uA, 10uA......
    if(ScalerGetBit(P4A_C0_MN_SCLKG_PLL_INSEL, (_BIT5 | _BIT4)) == _BIT5)
    {
        pData[5] = 4396 * ulpllm / 1880 / 13;
    }
    else if(ScalerGetBit(P4A_C0_MN_SCLKG_PLL_INSEL, (_BIT5 | _BIT4)) == (_BIT5 | _BIT4))
    {
        pData[5] = 4396 * ulpllm / 2340 / 13;
    }
    else
    {
        // For ICP default Setting
        pData[5] = 4396 * ulpllm / 1340 / 13;
    }

    pData[5] = ((20 * pData[5]) + 25) / 50;
    pData[5] -= (pData[5] == 0) ? 0 : 1;

    // Set PLL Charge Pump Current
    ScalerSetByte(P4A_BE_MN_SCLKG_PLL_CHP, (0x78 | (pData[5] & 0x07)));

    /*
    ////////////////////////////////
    // DPPLL M/N Tracking Setting //
    ////////////////////////////////

    // Don't Use M/N Tracking for Nvid > 0x40000 or Nvid < 0xFF

    //////////////////// Enable M/N auto tracking /////////////////////////
    // I = (2^22)*( (Tdds / Tmn) * (1 / n), n = 8
    // 1. Fdds = 1/4 * Fvco = (ulLinkClkValue / 8) * (ulpllm / ucplln) / 4 --> Tdds = (32 / ulLinkClkValue) * (plln / ulpllm)
    // 2. Tmn = Nvid * (1 / ulLinkClkValue)
    // from1,2 we can get : I = (2^27) * (ucplln / ulpllm) / Nvid / n = (2^24) * (ucplln / ulpllm) / Nvid

    ulIcodeValue = (16777216 / ulDpNvidValue) * ucplln / ulpllm;  //I = (2^24) * (ucplln / ulpllm) / Nvid

    ScalerSetBit(P4A_A8_MN_SCLKG_TRK_MN_I_H, ~(_BIT2 | _BIT1 | _BIT0), ((BYTE)(ulIcodeValue >> 16) & 0x03));
    ScalerSetByte(P4A_A9_MN_SCLKG_TRK_MN_I_M, (BYTE)(ulIcodeValue >> 8));
    ScalerSetByte(P4A_AA_MN_SCLKG_TRK_MN_I_L, (BYTE)ulIcodeValue);

    // Set P Code
    ScalerSetBit(P4A_AB_MN_SCLKG_TRK_MN_P_H, ~(_BIT2 | _BIT1 | _BIT0), (_BIT2 | _BIT1 | _BIT0));
    ScalerSetByte(P4A_AC_MN_SCLKG_TRK_MN_P_M, 0xFF);
    ScalerSetByte(P4A_AD_MN_SCLKG_TRK_MN_P_L, 0xFF);
    */

    ////////////////////////////////////////
    // DPPLL HS Tracking Tracking Setting //
    ////////////////////////////////////////

    // Disable Tracking New Mode
    ScalerSetBit(P4A_B8_MN_SCLKG_SDM_CTRL, ~(_BIT2 | _BIT1), _BIT1);

    // Set Initial I/P Code
    ScalerSetByte(P4A_B1_MN_SCLKG_TRK_VS_I_M, 0x00);
    ScalerSetByte(P4A_B2_MN_SCLKG_TRK_VS_I_L, 0x04);
    ScalerSetByte(P4A_B3_MN_SCLKG_TRK_VS_P_H, 0x07);
    ScalerSetByte(P4A_B4_MN_SCLKG_TRK_VS_P_M, 0xFF);
    ScalerSetByte(P4A_B5_MN_SCLKG_TRK_VS_P_L, 0xFF);

    // Get How Many Lines per SSC Period
    pData[0] = ulStreamClkValue / usDpHtotal / 33000;

    if(pData[0] > 15)
    {
        pData[0] = 15;
    }

    // Set Tracking Freq
    ScalerSetBit(P4A_A8_MN_SCLKG_TRK_MN_I_H, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (pData[0] << 4));

    // Set Tracking Lock PE
    ScalerSetByte(P4A_B7_MN_SCLKG_TRK_VS_NLOCK, 0x7F);

    // Enable H Sync Tracking
    ScalerSetByte(P4A_A7_MN_SCLKG_TRK_CTRL, 0x58);

    // Double Buffer for H Sync Tracking
    ScalerSetBit(P4A_A0_MN_SCLKG_CTRL, ~_BIT4, _BIT4);

    // Get Frame Period
    pData[15] = 1000 / (ulStreamClkValue / usDpHtotal / usDpVtotal) + 1;

    pData[0] = 0;
    // Waiting for Stream Clk Stable
    do
    {
        // Clear PE Flag
        ScalerSetBit(P4A_B7_MN_SCLKG_TRK_VS_NLOCK, ~_BIT7, _BIT7);
        ScalerTimerDelayXms(pData[15]);

        pData[0]++;
    }
    while((ScalerGetBit(P4A_B7_MN_SCLKG_TRK_VS_NLOCK, _BIT7) == _BIT7) && (pData[0] < 10));

    // Set Final I/P Code
    ScalerSetByte(P4A_B1_MN_SCLKG_TRK_VS_I_M, 0x00);
    ScalerSetByte(P4A_B2_MN_SCLKG_TRK_VS_I_L, 0x02);

    // Start Generate Display Format
    ScalerSetBit(P4A_35_DPF_CTRL_0, ~_BIT7, _BIT7);

    // Polling Vertical BS
    ScalerTimerPollingFlagProc(50, P4A_29_MST2SST_VBID, _BIT0, _TRUE);

    // Only Enable Tracking at Active Region when PE is Small
    PDATA_WORD(0) = ulVBs2BsCount / usDpVtotal / 2;

    ScalerSetBit(P4A_C2_HS_TRACKING_NEW_MODE1, ~(_BIT3 | _BIT2 | _BIT1 | _BIT0), (pData[0] & 0x0F));
    ScalerSetByte(P4A_C3_HS_TRACKING_NEW_MODE2, pData[1]);

    // Enable Tracking New Mode
    ScalerSetBit(P4A_B8_MN_SCLKG_SDM_CTRL, ~(_BIT2 | _BIT1), _BIT2);

    return _TRUE;
}

//--------------------------------------------------
// Description  : Check Dp Interlace by VBID or Measure
// Input Value  : None
// Output Value : True --> Interlace
//--------------------------------------------------
bit ScalerDpRx1MstToSstInterlaceCheck(WORD *pusVTotal, BYTE *pucOddFlg)
{
    DWORD ulLinkClk = 0;
    WORD usTempVTotal = *pusVTotal;
    WORD usHTotalCount = 0;
    WORD usVsyncWidth = 0;
    WORD usVstart = 0;
    WORD usVactive = 0;

    // Set Vfront Porch
    SET_DP_RX1_MST_TO_SST_VFRONT_PORCH(_DP_VSYNC_FRONT_PORCH);

#if(_DP_FREESYNC_SUPPORT == _ON)
    if(GET_DP_MST_FREESYNC_ENABLED() == _TRUE)
    {
        // Field inverse disable
        ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~_BIT4, 0x00);

        if(ScalerGetBit(P4A_29_MST2SST_VBID, _BIT2) == _BIT2) // Interlace
        {
            // Start to Measure Vertical BS to BS Counter by GDI Clock
            ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT7, 0x00);
            ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT7, _BIT7);

            if(ScalerTimerPollingFlagProc(_DP_MEASURE_POLLING_TIMEOUT, P4A_58_MN_MEAS_CTRL, _BIT6, _TRUE) == _FALSE)
            {
                return _FALSE;
            }

            // Pop up The result
            ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT5, _BIT5);

            // Get Measure Result
            ScalerRead(P4A_58_MN_MEAS_CTRL, 3, &pData[5], _AUTOINC);

            // Delay 2ms To Get VBID
            ScalerTimerDelayXms(2);

            // Get VBID
            ScalerRead(P4A_29_MST2SST_VBID, 1, &pData[1], _NON_AUTOINC);

            // Delay 1 frame
            ScalerTimerDelayXms((((PDATA_DWORD(1) & 0x001FFFFF) / (_GDI_CLK_KHZ)) + 2));

            // Pop up Measure Result
            ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT5, _BIT5);

            // Get Measure Result
            ScalerRead(P4A_58_MN_MEAS_CTRL, 3, &pData[9], _AUTOINC);

            // Get VBID
            ScalerRead(P4A_29_MST2SST_VBID, 1, &pData[2], _NON_AUTOINC);

            PDATA_DWORD(1) = (PDATA_DWORD(1) & 0x001FFFFF);
            PDATA_DWORD(2) = (PDATA_DWORD(2) & 0x001FFFFF);

            if((pData[1] & _BIT1) != (pData[2] & _BIT1))
            {
                // Enable DP Interlace, Field Information reference to VBID
                ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~(_BIT7 | _BIT6), _BIT7);

                if((ScalerGetBit(P4A_56_INTERLACE_MODE_CONFIG, _BIT0) == 0x00) &&
                   ((((pData[1] & _BIT1) == _BIT1) && ((pData[2] & _BIT1) == 0x00) && (PDATA_DWORD(1) < PDATA_DWORD(2))) ||
                    (((pData[1] & _BIT1) == 0x00) && ((pData[2] & _BIT1) == _BIT1) && (PDATA_DWORD(1) > PDATA_DWORD(2)))))
                {
                    // Field inverse enable
                    ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~_BIT4, _BIT4);
                }
            }
            else
            {
                // Enable interlace mode, field reference HW
                ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
            }

            *pusVTotal = usTempVTotal;

            return _TRUE;
        }
    }
    else
#endif
    {
        // [5] ln_ck_sel = 1'b1 -> CDR Clock
        // [3:0] DP_XTAL_CYCLE = 4'b0100 -> XTAL Counter Cycles = 2000
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~(_BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT2));

        // [1:0] freqdet_lane_sel = 2'b00 -> Lane0
        ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), _D1_DP_LANE0);

        // [6] LANE_COUNT_CLEAR = 0 -> Keep the LANE_COUNT value
        ScalerSetBit(PB0_52_DP_SIG_DET_2, ~_BIT6, 0x00);

        // [7] DP_SIG_DET_EN = 1'b1 -> Enable Signal Detection
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, _BIT7);

        // Waiting for Measure Done
        ScalerTimerDelayXms(2);

        if(ScalerGetBit(PB0_50_DP_SIG_DET_0, _BIT6) == _BIT6)
        {
            PDATA_WORD(0) = ((((WORD)ScalerGetByte(PB0_53_DP_SIG_DET_3)) << 8) | ScalerGetByte(PB0_54_DP_SIG_DET_4));
        }
        else
        {
            if(ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) == _DP_LINK_HBR2)
            {
                PDATA_WORD(0) = 0x4E20;
            }
            else if(ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) == _DP_LINK_HBR)
            {
                PDATA_WORD(0) = 0x2710;
            }
            else // Both Default && _DP_LINK_RBR Case
            {
                PDATA_WORD(0) = 0x1770;
            }
        }

        // [7] DP_SIG_DET_EN = 0 -> Disable Signal Detection
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);

        PDATA_DWORD(0) = (((DWORD)PDATA_WORD(0)) * _GDI_CLK_KHZ);

        // Link Clk in KHz
        ulLinkClk = (PDATA_DWORD(0) / 1000);

        DebugMessageRx1("Current Link Clk", ulLinkClk);

        // Field inverse disable
        ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~_BIT4, 0x00);

        // Start to Measure Vertical BS to BS Counter by GDI Clock
        ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT7, 0x00);
        ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT7, _BIT7);

        if(ScalerTimerPollingFlagProc(_DP_MEASURE_POLLING_TIMEOUT, P4A_58_MN_MEAS_CTRL, _BIT6, _TRUE) == _FALSE)
        {
            return _FALSE;
        }

        // Pop up The result
        ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT5, _BIT5);

        // Get Measure Result
        ScalerRead(P4A_58_MN_MEAS_CTRL, 3, &pData[5], _AUTOINC);

        // Delay 2ms To Get VBID
        ScalerTimerDelayXms(2);

        // Get VBID
        ScalerRead(P4A_29_MST2SST_VBID, 1, &pData[1], _NON_AUTOINC);

        // Delay 1 frame
        ScalerTimerDelayXms((((PDATA_DWORD(1) & 0x001FFFFF) / (_GDI_CLK_KHZ)) + 2));

        // Pop up Measure Result
        ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT5, _BIT5);

        // Get Measure Result
        ScalerRead(P4A_58_MN_MEAS_CTRL, 3, &pData[9], _AUTOINC);

        // Get Measure Htotal Counts
        ScalerRead(P4A_5B_MN_MEAS_HLN_M, 2, &pData[12], _AUTOINC);

        // Get VBID
        ScalerRead(P4A_29_MST2SST_VBID, 1, &pData[2], _NON_AUTOINC);

        PDATA_DWORD(1) = (PDATA_DWORD(1) & 0x001FFFFF);
        PDATA_DWORD(2) = (PDATA_DWORD(2) & 0x001FFFFF);

        // Convert HTotal Count from 1/2 Link Clk to GDI Clk
        usHTotalCount = ((DWORD)PDATA_WORD(6) * _GDI_CLK_KHZ * 2 + (ulLinkClk >> 1)) / ulLinkClk;

        DebugMessageRx1("HTotal Count", usHTotalCount);

        // if VTotal Difference > 1/2 HTotal
        if((ABSDWORD(PDATA_DWORD(1), PDATA_DWORD(2))) > (usHTotalCount / 2))
        {
            if(PDATA_DWORD(1) < PDATA_DWORD(2))
            {
                usTempVTotal = ((PDATA_DWORD(1) * 8 + (usHTotalCount >> 1)) / usHTotalCount);
            }
            else
            {
                usTempVTotal = ((PDATA_DWORD(2) * 8 + (usHTotalCount >> 1)) / usHTotalCount);
            }

            usTempVTotal = (usTempVTotal + 4) / 8;

            *pucOddFlg = _TRUE;
        }
        else
        {
            *pucOddFlg = _FALSE;
        }

        DebugMessageRx1("####Calculated usVTotal####", usTempVTotal);

        DebugMessageRx1("7. DelayTime", (((PDATA_DWORD(1) & 0x000FFFFF) / (_GDI_CLK_KHZ)) + 2));
        DebugMessageRx1("7. PDATA_DWORD(1)", PDATA_DWORD(1));
        DebugMessageRx1("7. PDATA_DWORD(2)", PDATA_DWORD(2));
        DebugMessageRx1("7. PDATA_DWORD(ABS)", (ABSDWORD(PDATA_DWORD(1), PDATA_DWORD(2))));

        if(ScalerGetBit(P4A_29_MST2SST_VBID, _BIT2) == _BIT2) // Interlace
        {
            // Get VSW
            usVsyncWidth = ((WORD)ScalerGetByte(P4A_1E_MSA_VSW_0) << 8) | ScalerGetByte(P4A_1F_MSA_VSW_1);
            usVsyncWidth &= 0x7FFF;

            // Get VStart
            usVstart = ((WORD)ScalerGetByte(P4A_1A_MSA_VST_0) << 8) | ScalerGetByte(P4A_1B_MSA_VST_1);

            // usVbackporch = usVstart - Fixed V-Sync Pulse Width, usVbackporch > usVsyncWidth + 2 lines, here 2 lines is for V-Sync Falling edge to DEN Rising Edge distance margion
            if(((pData[1] & _BIT1) != (pData[2] & _BIT1)) && (usVstart > (usVsyncWidth + _DP_VSYNC_WIDTH + 2)))
            {
                // Enable DP Interlace, Field Information reference to VBID
                ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~(_BIT7 | _BIT6), _BIT7);

                // Get Vactive
                usVactive = ((WORD)ScalerGetByte(P4A_1C_MSA_VHT_0) << 8) | ScalerGetByte(P4A_1D_MSA_VHT_1);

                // Set Front Porch = VTotal - Active height - MSA V Start + V Sync Pulse Width
                SET_DP_RX1_MST_TO_SST_VFRONT_PORCH(usTempVTotal - usVactive - usVstart + usVsyncWidth);

                if((ScalerGetBit(P4A_56_INTERLACE_MODE_CONFIG, _BIT0) == 0x00) &&
                   ((((pData[1] & _BIT1) == _BIT1) && ((pData[2] & _BIT1) == 0x00) && (PDATA_DWORD(1) < PDATA_DWORD(2))) ||
                    (((pData[1] & _BIT1) == 0x00) && ((pData[2] & _BIT1) == _BIT1) && (PDATA_DWORD(1) > PDATA_DWORD(2)))))
                {
                    // Field inverse enable
                    ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~_BIT4, _BIT4);
                }
            }
            else
            {
                // Enable interlace mode, field reference HW
                ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));
            }

            *pusVTotal = usTempVTotal;

            return _TRUE;
        }
        else if((ABSDWORD(PDATA_DWORD(1), PDATA_DWORD(2))) > (usHTotalCount / 2))
        {
            // Enable interlace mode, field reference HW
            ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~(_BIT7 | _BIT6), (_BIT7 | _BIT6));

            *pusVTotal = usTempVTotal;

            return _TRUE;
        }
    }

    // Disable interlace mode, field reference HW
    ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~(_BIT7 | _BIT6), 0x00);

    return _FALSE;
}

//--------------------------------------------------
// Description  : DP MST2SST BS to VS Delay Calculation
// Input Value  : None
// Output Value : BS to VS Delay
//--------------------------------------------------
DWORD ScalerDpRx1MstToSstGetBStoVSDelay(void)
{
    DWORD ulVFrontPorch = 0;
    DWORD ulHFrontPorch = 0;
    WORD usFifoOffset = 0;

    // Get Htotal
    ScalerRead(P4A_10_MSA_HTT_0, 2, &pData[2], _AUTOINC);

    // VFrontPorch = _DP_VSYNC_FRONT_PORCH * Htotal * (Link Clk / Stream CLK)
    ulVFrontPorch = ((DWORD)GET_DP_RX1_MST_TO_SST_VFRONT_PORCH()) * ((DWORD)PDATA_WORD(1) * 256) / ((DWORD)GET_DP_TX_INPUT_PIXEL_CLK()) *
                    (((DWORD)ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00)) * 270) / 256;

    // Get Hactive
    ScalerRead(P4A_14_MSA_HWD_0, 2, pData, _AUTOINC);

    // Get HSW
    pData[4] = ScalerGetBit(P4A_16_MSA_HSW_0, (_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0));
    pData[5] = ScalerGetByte(P4A_17_MSA_HSW_1);

    // Get HSW min by Measure clock
    PDATA_WORD(3) = ((DWORD) _DP_HSYNC_WIDTH_MEASURE_COUNTER * GET_DP_TX_INPUT_PIXEL_CLK() * 100) / _MEASURE_CLK + 1;

    // Check HSW <= HSW min
    if(PDATA_WORD(2) <= PDATA_WORD(3))
    {
        // Check if H porch is more than HSW min
        if((PDATA_WORD(1) - PDATA_WORD(0)) >= PDATA_WORD(3))
        {
            // HSW = HSW min
            PDATA_WORD(2) = PDATA_WORD(3);
        }
        else
        {
            // HSW = Max H Porch
            PDATA_WORD(2) = (PDATA_WORD(1) - PDATA_WORD(0));
        }
    }

    // DPF H Front Porch must not be less than 8 (HW constraint)
    // Check if H porch is more than 8
    if((PDATA_WORD(1) - PDATA_WORD(0) - PDATA_WORD(2)) <= _DP_HSYNC_FRONT_PORCH)
    {
        // HSW < HSW min
        if(PDATA_WORD(2) < PDATA_WORD(3))
        {
            // Hstart = HSW
            PDATA_WORD(4) = PDATA_WORD(2);
        }
        else if((PDATA_WORD(1) - PDATA_WORD(0) - PDATA_WORD(3) >= _DP_HSYNC_FRONT_PORCH))
        {
            // HSW = H porch - _DP_HSYNC_FRONT_PORCH
            PDATA_WORD(2) = PDATA_WORD(1) - PDATA_WORD(0) - _DP_HSYNC_FRONT_PORCH;

            // Hstart = HSW
            PDATA_WORD(4) = PDATA_WORD(2);
        }
        else
        {
            // HSW = HSW min
            PDATA_WORD(2) = PDATA_WORD(3);

            // Hstart = HSW min
            PDATA_WORD(4) = PDATA_WORD(3);
        }
    }
    else
    {
        // Hstart = Htotal - Hactive - _DP_HSYNC_FRONT_PORCH
        PDATA_WORD(4) = PDATA_WORD(1) - PDATA_WORD(0) - _DP_HSYNC_FRONT_PORCH;
    }

    // HFrontPorch = (Htotal - Hactive - Hstart) * (Link Clk / Stream CLK)
    ulHFrontPorch = (((DWORD)(PDATA_WORD(1) - PDATA_WORD(0) - PDATA_WORD(4))) * (((DWORD)ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00)) * 270) / ((DWORD)GET_DP_TX_INPUT_PIXEL_CLK()));

    // Get MISC Info
    ScalerRead(P4A_28_MSA_COLOR_BIT, 1, &pData[14], _NON_AUTOINC);

    switch(pData[14] & (_BIT2 | _BIT1 | _BIT0))
    {
        case 0x00:
            pData[15] = 6;
            break;

        case (_BIT0):
            pData[15] = 8;
            break;

        case (_BIT1):
            pData[15] = 10;
            break;

        case (_BIT1 | _BIT0):
            pData[15] = 12;
            break;

        case (_BIT2):
            pData[15] = 16;
            break;

        default:
            break;
    }

    // DP fifo size = 256 x 96bit
    if(ScalerGetBit(P4A_28_MSA_COLOR_BIT, (_BIT4 | _BIT3)) == _BIT3)
    {
        // FifoOffset = (1/2 * (256 * 96) / (2 * depth per color)), color format is YCbCr 4:2:2
        usFifoOffset = _HW_DP_RX_VIDEO_FIFO_SIZE / (4 * (WORD)pData[15]);
    }
    else
    {
        // FifoOffset = (1/2 * (256 * 96) / (3 * depth per color)), color format is RGB or YCbCr 4:4:4 or others
        usFifoOffset = _HW_DP_RX_VIDEO_FIFO_SIZE / (6 * (WORD)pData[15]);
    }

    // FifoOffset = (1/2 * (256 * 96) / (3 * depth per color)) * (Link Clk / Stream CLK))
    usFifoOffset = (WORD)(((DWORD)usFifoOffset) * (((DWORD)ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00)) * 270) / ((DWORD)GET_DP_TX_INPUT_PIXEL_CLK()));

    return ((ulVFrontPorch + ulHFrontPorch + (DWORD)usFifoOffset) / 2);
}

//--------------------------------------------------
// Description  : DP MST2SST Display Format Generator Setting
// Input Value  : None
// Output Value : None
//--------------------------------------------------
bit ScalerDpRx1MstToSstDisplayFormatSetting(void)
{
    BYTE ucOddFlg = 0;
    WORD usDpVtotal = 0;
    WORD usDpHactive = 0;
    DWORD ulStreamClkValue = 0;
    DWORD ulVBs2BsCount = 0;

#if(_DP_FREESYNC_SUPPORT == _ON)
    WORD usHBs2BsCount = 0;
    DWORD ulDpMvidValue = 0;
    DWORD ulDpNvidValue = 0;
#endif

    // Measurement Enable
    ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT7, 0x00);
    ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT7, _BIT7);

    if(ScalerTimerPollingFlagProc(_DP_MEASURE_POLLING_TIMEOUT, P4A_58_MN_MEAS_CTRL, _BIT6, _TRUE) == _FALSE)
    {
        return _FALSE;
    }

    // Pop up The result
    ScalerSetBit(P4A_58_MN_MEAS_CTRL, ~_BIT5, _BIT5);

    ulVBs2BsCount = ((((DWORD)(ScalerGetByte(P4A_58_MN_MEAS_CTRL) & 0x1F)) << 16) | (((DWORD)ScalerGetByte(P4A_59_MN_MEAS_VLN_M)) << 8) | ((DWORD)ScalerGetByte(P4A_5A_MN_MEAS_VLN_L)));

#if(_DP_FREESYNC_SUPPORT == _ON)
    if(GET_DP_MST_FREESYNC_ENABLED() == _TRUE)
    {
        // HBs2Bs count in Link Clk / 2
        usHBs2BsCount = (((DWORD)ScalerGetByte(P4A_5B_MN_MEAS_HLN_M)) << 8) | ((DWORD)ScalerGetByte(P4A_5C_MN_MEAS_HLN_L));

        // VTotal in line, Link Clk / 2 : (Link Rate * 27 / 2)
        usDpVtotal = ulVBs2BsCount * (ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) * 27 / 2) / usHBs2BsCount * 1000 / _GDI_CLK_KHZ;

        ulDpMvidValue = ((((DWORD)ScalerGetByte(P4A_20_MSA_MVID_00)) << 16) | (((DWORD)ScalerGetByte(P4A_21_MSA_MVID_01)) << 8) | ((DWORD)ScalerGetByte(P4A_22_MSA_MVID_02)));

        ulDpNvidValue = ((((DWORD)ScalerGetByte(P4A_23_MSA_NVID_00)) << 16) | (((DWORD)ScalerGetByte(P4A_24_MSA_NVID_01)) << 8) | ((DWORD)ScalerGetByte(P4A_25_MSA_NVID_02)));

        // [5] ln_ck_sel = 1'b1 -> CDR Clock
        // [3:0] DP_XTAL_CYCLE = 4'b0100 -> XTAL Counter Cycles = 2000
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~(_BIT5 | _BIT3 | _BIT2 | _BIT1 | _BIT0), (_BIT5 | _BIT2));

        // [1:0] freqdet_lane_sel = 2'b00 -> Lane0
        ScalerSetBit(PB0_51_DP_SIG_DET_1, ~(_BIT1 | _BIT0), _D1_DP_LANE0);

        // [6] LANE_COUNT_CLEAR = 0 -> Keep the LANE_COUNT value
        ScalerSetBit(PB0_52_DP_SIG_DET_2, ~_BIT6, 0x00);

        // [7] DP_SIG_DET_EN = 1'b1 -> Enable Signal Detection
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, _BIT7);

        // Waiting for Measure Done
        ScalerTimerDelayXms(2);

        PDATA_DWORD(0) = (((DWORD)(ScalerGetByte(PB0_52_DP_SIG_DET_2) & 0x3F) << 16) |
                          (ScalerGetByte(PB0_53_DP_SIG_DET_3) << 8) |
                          (ScalerGetByte(PB0_54_DP_SIG_DET_4)));

        // [7] DP_SIG_DET_EN = 0 -> Disable Signal Detection
        ScalerSetBit(PB0_50_DP_SIG_DET_0, ~_BIT7, 0x00);

        PDATA_DWORD(0) = ((DWORD)PDATA_DWORD(0)) * _GDI_CLK_KHZ;

        // Calculate streamclk
        ulStreamClkValue = ((DWORD)ulDpMvidValue * (PDATA_DWORD(0) / ulDpNvidValue) + (PDATA_DWORD(0) % ulDpNvidValue * 100 / ulDpNvidValue * (DWORD)ulDpMvidValue / 100));
    }
    else
#endif
    {
        usDpVtotal = ((((WORD)ScalerGetByte(P4A_18_MSA_VTT_0)) << 8) | ((WORD)ScalerGetByte(P4A_19_MSA_VTT_1)));

        // Calculate streamclk (Unit is Hz)
        ulStreamClkValue = (((((((DWORD)ScalerGetByte(P4A_10_MSA_HTT_0)) << 8) | ((WORD)ScalerGetByte(P4A_11_MSA_HTT_1))) * ((DWORD)usDpVtotal) * 100) / ulVBs2BsCount) * _GDI_CLK_KHZ * 10);
    }

    // Input Pixel CLK Unit is 0.1 M
    SET_DP_TX_INPUT_PIXEL_CLK(ulStreamClkValue / 100000);

    // Disable Generate Display Format
    ScalerSetBit(P4A_35_DPF_CTRL_0, ~_BIT7, 0x00);

    // DP Display Format Generator Reset
    ScalerSetBit(P4A_00_MST2SST_RESET, ~_BIT0, _BIT0);
    ScalerSetBit(P4A_00_MST2SST_RESET, ~_BIT0, 0x00);

    // Set Vactive
    ScalerRead(P4A_1C_MSA_VHT_0, 2, &pData[4], _AUTOINC);
    ScalerSetByte(P4A_4C_MN_DPF_VHT_M, pData[4]);
    ScalerSetByte(P4A_4D_MN_DPF_VHT_L, pData[5]);

    // Set Hactive
    ScalerRead(P4A_14_MSA_HWD_0, 2, pData, _AUTOINC);
    ScalerSetByte(P4A_44_MN_DPF_HWD_M, pData[0]);
    ScalerSetByte(P4A_45_MN_DPF_HWD_L, pData[1]);
    usDpHactive = PDATA_WORD(0);

#if(_DP_FREESYNC_SUPPORT == _ON)
    if(GET_DP_MST_FREESYNC_ENABLED() == _TRUE)
    {
        // HTotal in Pixel Clk, Link Clk / 2 : (Link Rate * 27 / 2)
        WORD usDpHtotal = (DWORD)usHBs2BsCount * (GET_DP_TX_INPUT_PIXEL_CLK() / 10) / (ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) * 27 / 2);

        // Set Htotal
        ScalerSetByte(P4A_40_MN_DPF_HTT_M, HIBYTE(usDpHtotal));
        ScalerSetByte(P4A_41_MN_DPF_HTT_L, LOBYTE(usDpHtotal));

        // Set Hstart
        ScalerSetByte(P4A_42_MN_DPF_HST_M, HIBYTE(_DP_DE_ONLY_MODE_HSTART));
        ScalerSetByte(P4A_43_MN_DPF_HST_L, LOBYTE(_DP_DE_ONLY_MODE_HSTART));

        // Set HSW
        ScalerSetByte(P4A_46_MN_DPF_HSW_M, HIBYTE(_DP_DE_ONLY_MODE_HSW));
        ScalerSetByte(P4A_47_MN_DPF_HSW_L, LOBYTE(_DP_DE_ONLY_MODE_HSW));

        // Set Vstart
        ScalerSetByte(P4A_4A_MN_DPF_VST_M, HIBYTE(_DP_DE_ONLY_MODE_VSTART));
        ScalerSetByte(P4A_4B_MN_DPF_VST_L, LOBYTE(_DP_DE_ONLY_MODE_VSTART));

        // Set VSW
        ScalerSetByte(P4A_4E_MN_DPF_VSW_M, HIBYTE(_DP_DE_ONLY_MODE_VSW));
        ScalerSetByte(P4A_4F_MN_DPF_VSW_L, LOBYTE(_DP_DE_ONLY_MODE_VSW));

        PDATA_WORD(0) = ScalerDpRx1MstToSstGetBStoHSDelay(usDpHtotal);

        // Set HBs2Bs
        ScalerSetByte(P4A_3D_BS2HS_0, pData[0]);
        ScalerSetByte(P4A_3E_BS2HS_1, pData[1]);

        ScalerDpRx1MstToSstInterlaceCheck(&usDpVtotal, &ucOddFlg);

        // Set DE Only Mode Vsync Front Porch
        ScalerSetByte(P4A_3F_VS_FRONT_PORCH, GET_DP_RX1_VFRONT_PORCH());

        // Set DE Only Mode
        ScalerSetBit(P4A_35_DPF_CTRL_0, ~(_BIT7 | _BIT5 | _BIT4), _BIT4);

        if(ScalerGlobalGetIDCode() <= _RL6193_VER_C_ID_CODE)
        {
            // Choose Frame start BE to Reset FIFO
            ScalerSetBit(P4A_30_PG_CTRL_0, ~_BIT5, _BIT5);
        }
        else
        {
            // Choose VS Rising to Reset FIFO
            ScalerSetBit(P4A_30_PG_CTRL_0, ~_BIT5, 0x00);
        }

        // Set DP FREESYNC Mode Enable
        ScalerSetBit(P4A_F0_DP_RSV0, ~_BIT0, _BIT0);

        DebugMessageRx1("MST FREESYNC Mode Enabled!!", 0);
    }
    else
#endif
    {
        DWORD ulBs2VsDelay = 0;

        // Set Htotal
        ScalerRead(P4A_10_MSA_HTT_0, 2, &pData[2], _AUTOINC);
        ScalerSetByte(P4A_40_MN_DPF_HTT_M, pData[2]);
        ScalerSetByte(P4A_41_MN_DPF_HTT_L, pData[3]);

        // Get HSW
        pData[4] = ScalerGetBit(P4A_16_MSA_HSW_0, (_BIT6 | _BIT5 | _BIT4 | _BIT3 | _BIT2 | _BIT1 | _BIT0));
        pData[5] = ScalerGetByte(P4A_17_MSA_HSW_1);

        // Get HSW min by Measure clock
        PDATA_WORD(3) = ((DWORD) _DP_HSYNC_WIDTH_MEASURE_COUNTER * GET_DP_TX_INPUT_PIXEL_CLK() * 100) / _MEASURE_CLK + 1;

        // Check HSW <= HSW min
        if(PDATA_WORD(2) <= PDATA_WORD(3))
        {
            // Check if H porch is more than HSW min
            if((PDATA_WORD(1) - usDpHactive) >= PDATA_WORD(3))
            {
                // HSW = HSW min
                PDATA_WORD(2) = PDATA_WORD(3);
            }
            else
            {
                // HSW = Max H Porch
                PDATA_WORD(2) = (PDATA_WORD(1) - usDpHactive);
            }
        }

        // DPF H Front Porch must not be less than 8 (HW constraint)
        // Check if H porch is more than 8
        if((PDATA_WORD(1) - usDpHactive - PDATA_WORD(2)) <= _DP_HSYNC_FRONT_PORCH)
        {
            // HSW < HSW min
            if(PDATA_WORD(2) < PDATA_WORD(3))
            {
                // Hstart = HSW
                PDATA_WORD(4) = PDATA_WORD(2);
            }
            else if((PDATA_WORD(1) - usDpHactive - PDATA_WORD(3) >= _DP_HSYNC_FRONT_PORCH))
            {
                // HSW = H porch - _DP_HSYNC_FRONT_PORCH
                PDATA_WORD(2) = PDATA_WORD(1) - usDpHactive - _DP_HSYNC_FRONT_PORCH;

                // Hstart = HSW
                PDATA_WORD(4) = PDATA_WORD(2);
            }
            else
            {
                // HSW = HSW min
                PDATA_WORD(2) = PDATA_WORD(3);

                // Hstart = HSW min
                PDATA_WORD(4) = PDATA_WORD(3);
            }
        }
        else
        {
            // Hstart = Htotal - Hactive - _DP_HSYNC_FRONT_PORCH
            PDATA_WORD(4) = PDATA_WORD(1) - usDpHactive - _DP_HSYNC_FRONT_PORCH;
        }

        // Set HSW and HSP => positive
        ScalerSetByte(P4A_46_MN_DPF_HSW_M, (pData[4] & ~_BIT7));
        ScalerSetByte(P4A_47_MN_DPF_HSW_L, pData[5]);

        // Set Hstart
        ScalerSetByte(P4A_42_MN_DPF_HST_M, pData[8]);
        ScalerSetByte(P4A_43_MN_DPF_HST_L, pData[9]);

        DebugMessageRx1("###Origin usVTotal###", usDpVtotal);

        if(ScalerDpRx1MstToSstInterlaceCheck(&usDpVtotal, &ucOddFlg) == _TRUE) // Interlace
        {
            SET_DP_TX_INPUT_INTERLACE_MODE(_ON);
        }
        else // Progressive
        {
            // Disable DP Interlace
            ScalerSetBit(P4A_56_INTERLACE_MODE_CONFIG, ~(_BIT7 | _BIT6), 0x00);

            SET_DP_TX_INPUT_INTERLACE_MODE(_OFF);
        }

        // Set Vtotal
        ScalerSetByte(P4A_48_MN_DPF_VTT_M, (usDpVtotal >> 8));
        ScalerSetByte(P4A_49_MN_DPF_VTT_L, (usDpVtotal & 0xFF));

        // Get Vactive
        ScalerRead(P4A_1C_MSA_VHT_0, 2, &pData[4], _AUTOINC);

        // V Front porch must not be less than 2 because of V scaling down.
        // Set Vstart -> (Vtotal - Vactive - Frontporch)
        PDATA_WORD(0) = usDpVtotal - PDATA_WORD(2) - GET_DP_RX1_MST_TO_SST_VFRONT_PORCH();
        ScalerSetByte(P4A_4A_MN_DPF_VST_M, pData[0]);
        ScalerSetByte(P4A_4B_MN_DPF_VST_L, pData[1]);

        // Set VSW; Set VSP => Positive
        ScalerSetByte(P4A_4E_MN_DPF_VSW_M, (HIBYTE(_DP_VSYNC_WIDTH) & ~_BIT7));
        ScalerSetByte(P4A_4F_MN_DPF_VSW_L, LOBYTE(_DP_VSYNC_WIDTH));

        ulBs2VsDelay = ScalerDpRx1MstToSstGetBStoVSDelay();

        // Set Odd Field BS to VS Delay
        ScalerSetByte(P4A_3A_OVBLK2VS_H, (BYTE)(ulBs2VsDelay >> 16));
        ScalerSetByte(P4A_3B_OVBLK2VS_M, (BYTE)(ulBs2VsDelay >> 8));
        ScalerSetByte(P4A_3C_OVBLK2VS_L, (BYTE)(ulBs2VsDelay >> 0));

        if((bit)ucOddFlg == _TRUE)
        {
            // Get Htotal
            ScalerRead(P4A_10_MSA_HTT_0, 2, &pData[0], _AUTOINC);

            // BStoVSDelay = BStoVSDelay + (Link Clk * 256 / Stream Clk) * ((DWORD)Htotal) / 256;
            ulBs2VsDelay += ((((DWORD)ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00)) * 270UL) * 256) / ((DWORD)GET_DP_TX_INPUT_PIXEL_CLK()) * ((DWORD)PDATA_WORD(0)) / 256 / 2;

            SET_DP_TX_INPUT_INTERLACE_VTOTAL_NUMBER(_ODD);
        }
        else
        {
            SET_DP_TX_INPUT_INTERLACE_VTOTAL_NUMBER(_EVEN);
        }

        // Set Even Field BS to VS Delay
        ScalerSetByte(P4A_37_EVBLK2VS_H, (BYTE)(ulBs2VsDelay >> 16));
        ScalerSetByte(P4A_38_EVBLK2VS_M, (BYTE)(ulBs2VsDelay >> 8));
        ScalerSetByte(P4A_39_EVBLK2VS_L, (BYTE)(ulBs2VsDelay >> 0));

        // Choose VS Rising to Reset FIFO
        ScalerSetBit(P4A_30_PG_CTRL_0, ~_BIT5, 0x00);

        // Set DP FREESYNC Mode Disable
        ScalerSetBit(P4A_F0_DP_RSV0, ~_BIT0, 0x00);

        if((ScalerGetBit(P4A_56_INTERLACE_MODE_CONFIG, _BIT7) == _BIT7) && (ScalerGetBit(P4A_56_INTERLACE_MODE_CONFIG, _BIT0) != ScalerGetBit(P4A_27_MSA_MISC_1, _BIT0)))
        {
            // set frame sync mode, For VG870 1075:1080i50Hz timing 8bit -> 10bit issue
            ScalerSetBit(P4A_35_DPF_CTRL_0, ~(_BIT7 | _BIT5 | _BIT4), _BIT5);
        }
        else
        {
            // Set full last line Mode
            ScalerSetBit(P4A_35_DPF_CTRL_0, ~(_BIT7 | _BIT5 | _BIT4), 0x00);
        }

        DebugMessageRx1("MST FREESYNC Mode Disabled!!", 0);
    }

    // Double Buffer to Apply Display Format Setting
    ScalerSetBit(P4A_35_DPF_CTRL_0, ~_BIT6, _BIT6);

    ScalerDpRx1MstToSstVideoSetting();

    return _TRUE;
}

//--------------------------------------------------
// Description  : DP MST2SST Video Fifo Check
// Input Value  : Fifo Check Condition
// Output Value : True --> Fifo not over/underflow
//--------------------------------------------------
bit ScalerDpRx1MstToSstFifoCheck(EnumDpFifoCheckCondition enumFifoCheckCondition)
{
#if(_DP_FREESYNC_SUPPORT == _ON)
    // Temp for C Version Bug
    if((GET_DP_MST_FREESYNC_ENABLED() == _TRUE) && (ScalerGlobalGetIDCode() <= _RL6193_VER_C_ID_CODE))
    {
        return _TRUE;
    }
#endif

    if(enumFifoCheckCondition == _DP_FIFO_DELAY_CHECK)
    {
        ScalerSetBit(P4A_31_PG_CTRL_1, ~_BIT0, _BIT0);

        ScalerTimerDelayXms(30);

        pData[0] = 0;

        while(pData[0] < 6)
        {
            ScalerTimerDelayXms(10);

            if((ScalerGetBit(P4A_31_PG_CTRL_1, _BIT1) == _BIT1) || (ScalerGetBit(P4A_31_PG_CTRL_1, _BIT0) == _BIT0))
            {
                return _FALSE;
            }

            pData[0]++;
        }
    }
    else
    {
        if((ScalerGetBit(P4A_31_PG_CTRL_1, _BIT1) == _BIT1) || (ScalerGetBit(P4A_31_PG_CTRL_1, _BIT0) == _BIT0))
        {
            return _FALSE;
        }
    }

    return _TRUE;
}

//--------------------------------------------------
// Description  : DP Adjust Vsync Delay
// Input Value  : None
// Output Value : None
//--------------------------------------------------
void ScalerDpRx1MstToSstAdjustVsyncDelay(void)
{
    DWORD ulBStoVSDelayDeltaOverflow = 0;
    DWORD ulBStoVSDelayDeltaUnderflow = 0;

    if(ScalerGetBit(P4A_31_PG_CTRL_1, (_BIT1 | _BIT0)) == 0x00)// Fifo Ok
    {
        return;
    }
    else if(ScalerGetBit(P4A_31_PG_CTRL_1, _BIT0) == _BIT0) // Fifo Overflow
    {
        // Get Htotal
        ScalerRead(P4A_10_MSA_HTT_0, 2, &pData[0], _AUTOINC);

        // ((Link Clk * 256)/ Stream CLK) * Htotal / 256 --> 1 Line
        ulBStoVSDelayDeltaOverflow = (((DWORD)ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) * 270UL) * 256) / ((DWORD)GET_D1_INPUT_PIXEL_CLK()) * ((DWORD)PDATA_WORD(0)) / 256;

        DebugMessageRx1("DP MST2SST : Fifo Overflow --> nVidia Case", ulBStoVSDelayDeltaOverflow);
    }
    else if(ScalerGetBit(P4A_31_PG_CTRL_1, _BIT1) == _BIT1) // Fifo Underflow
    {
        // Get Hactive
        ScalerRead(P4A_14_MSA_HWD_0, 2, &pData[0], _AUTOINC);

        if(PDATA_WORD(0) != 1366)
        {
            return;
        }

        PDATA_WORD(0) = PDATA_WORD(0) - 2;

        // Set Hactive
        ScalerSetByte(P4A_44_MN_DPF_HWD_M, pData[0]);
        ScalerSetByte(P4A_45_MN_DPF_HWD_L, pData[1]);

        // ((Link Clk * 2)/ Stream CLK) --> 2 pixels
        ulBStoVSDelayDeltaUnderflow = (((DWORD)ScalerDpRx1GetDpcdInfo(0x00, 0x01, 0x00) * 270) * 2) / ((DWORD)GET_D1_INPUT_PIXEL_CLK());

        DebugMessageRx1("DP MST2SST : Fifo Underflow --> QD882 Case", ulBStoVSDelayDeltaUnderflow);
    }

    // For Initial Value Before Use PDATA_DWORD(0)
    pData[0] = 0;

    // Get BStoVSDelay Even
    ScalerRead(P4A_37_EVBLK2VS_H, 3, &pData[1], _AUTOINC);

    // For Initial Value Before Use PDATA_DWORD(1)
    pData[4] = 0;

    // Get BStoVSDelay Odd
    ScalerRead(P4A_3A_OVBLK2VS_H, 3, &pData[5], _AUTOINC);

    // BStoVSDelay = BStoVSDelay + (Nvid * 256 / Mvid) * ((DWORD)Htotal) / 256;
    PDATA_DWORD(0) = PDATA_DWORD(0) - ((ulBStoVSDelayDeltaOverflow + ulBStoVSDelayDeltaUnderflow) / 2);

    // BStoVSDelay = BStoVSDelay + (Nvid * 256 / Mvid) * ((DWORD)Htotal) / 256;
    PDATA_DWORD(1) = PDATA_DWORD(1) - ((ulBStoVSDelayDeltaOverflow + ulBStoVSDelayDeltaUnderflow) / 2);

    // Set Even Field BS to VS Delay
    ScalerSetByte(P4A_37_EVBLK2VS_H, pData[1]);
    ScalerSetByte(P4A_38_EVBLK2VS_M, pData[2]);
    ScalerSetByte(P4A_39_EVBLK2VS_L, pData[3]);

    // Set Odd Field BS to VS Delay
    ScalerSetByte(P4A_3A_OVBLK2VS_H, pData[5]);
    ScalerSetByte(P4A_3B_OVBLK2VS_M, pData[6]);
    ScalerSetByte(P4A_3C_OVBLK2VS_L, pData[7]);

    // Enable Double Buffer
    ScalerSetBit(P4A_35_DPF_CTRL_0, ~_BIT6, _BIT6);

    // Reset Display Format Gen
    ScalerSetBit(P4A_35_DPF_CTRL_0, ~_BIT7, 0x00);
    ScalerSetBit(P4A_35_DPF_CTRL_0, ~_BIT7, _BIT7);

    ScalerTimerDelayXms(30);
}

//--------------------------------------------------
// Description  : DP Rx1 MST to SST Measure
// Input Value  : None
// Output Value : None
//--------------------------------------------------
bit ScalerDpRx1MstToSstMeasureInput(void)
{
    // Format Conversion Select to SST_D1
    ScalerSetBit(P31_02_FC_INPUT_DPTX_SEL, ~(_BIT7 | _BIT6 | _BIT5 | _BIT4), (_FC_SOURCE_SST_D1 << 4));

    return ScalerDpTxMeasureInputInfo();
}

#if(_DP_FREESYNC_SUPPORT == _ON)
//--------------------------------------------------
// Description  : DP BS to HS Delay Calculation
// Input Value  : None
// Output Value : BS to VS Delay
//--------------------------------------------------
WORD ScalerDpRx1MstToSstGetBStoHSDelay(WORD usDpHtotal)
{
    WORD usFifoOffset = 0;

    // Get MISC Info
    ScalerRead(P4A_28_MSA_COLOR_BIT, 1, &pData[14], _NON_AUTOINC);

    switch(pData[14] & (_BIT2 | _BIT1 | _BIT0))
    {
        case 0x00:
            pData[15] = 6;
            break;

        case (_BIT0):
            pData[15] = 8;
            break;

        case (_BIT1):
            pData[15] = 10;
            break;

        case (_BIT1 | _BIT0):
            pData[15] = 12;
            break;

        case (_BIT2):
            pData[15] = 16;
            break;

        default:
            pData[15] = 8;
            break;
    }

    // DP fifo size = 256 x 96bit
    if(ScalerGetBit(P4A_28_MSA_COLOR_BIT, (_BIT4 | _BIT3)) == _BIT3)
    {
        // FifoOffset = (1/2 * (256 * 96) / (2 * depth per color)), color format is YCbCr 4:2:2
        usFifoOffset = _HW_DP_RX_VIDEO_FIFO_SIZE / (4 * (WORD)pData[15]);
    }
    else
    {
        // FifoOffset = (1/2 * (256 * 96) / (3 * depth per color)), color format is RGB or YCbCr 4:4:4 or others
        usFifoOffset = _HW_DP_RX_VIDEO_FIFO_SIZE / (6 * (WORD)pData[15]);
    }

    // Get Hstart
    ScalerRead(P4A_42_MN_DPF_HST_M, 2, &pData[0], _AUTOINC);

    // Get Hactive
    ScalerRead(P4A_44_MN_DPF_HWD_M, 2, &pData[2], _AUTOINC);

    // H Blanking = HTotal - HActive
    PDATA_WORD(2) = usDpHtotal - PDATA_WORD(1);

    // Set BS to HS delay to (HBlanking + 1/2 FIFO - HStart), BS2HS delay: 2 pixel mode
    return ((WORD)((PDATA_WORD(2) + usFifoOffset - PDATA_WORD(0)) >> 1));
}
#endif // End of #if(_DP_FREESYNC_SUPPORT == _ON)
#endif // End of #if(_DP_TX_SUPPORT == _ON)
#endif // End of #if(_D1_DP_EXIST == _ON)
