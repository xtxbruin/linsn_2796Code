/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2011>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : RL6410_Series_MHL3Rx2Interface.h No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#if((_D2_INPUT_PORT_TYPE == _D2_MHL_PORT) && (_D2_MHL_VERSION >= _MHL_VERSION_3_0))

//****************************************************************************
// INTERFACE DEFINITIONS / MACROS
//****************************************************************************


//****************************************************************************
// INTERFACE VARIABLE DECLARATIONS
//****************************************************************************


//****************************************************************************
// INTERFACE FUNCTIONS
//****************************************************************************
extern void ScalerMHL3Rx2ECBUSIntHandler_EXINT0(void);
extern bit ScalerMHL3Rx2ECBUSHandler(void);
extern void ScalerMHL3Rx2BISTHandler(void);

extern BYTE ScalerMHL3Rx2ECBUSEMSCRxHandler(void);
extern bit ScalerMHL3Rx2ECBUSEMSCTxHandler(void);
extern bit ScalerMHL3Rx2FakePowerSavingCheck(void);

#endif // End of #if((_D2_INPUT_PORT_TYPE == _D2_MHL_PORT) && (_D2_MHL_VERSION >= _MHL_VERSION_3_0))
