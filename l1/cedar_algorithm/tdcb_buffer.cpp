/*
 * tdcb_buffer.cpp
 *
 *  Created on: Nov 24, 2014
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#include "tdcb_buffer.h"

#include <stdlib.h>
#include <stdio.h>        
#include <math.h>
#include <string.h>
#include <stdarg.h>

// Board Header 1
int DetectorSubType (unsigned long datum) {return (datum & DATA_SOURCE_SUB_ID_MASK)>>DATA_SOURCE_SUB_ID_MASK_RSHIFT;}
int DataBlockSize (unsigned long datum) {return (datum & DATA_BLOCK_SIZE_MASK)>>DATA_BLOCK_SIZE_MASK_RSHIFT;}
//unsigned long DetectorType (unsigned long datum) {return (datum & DATA_SOURCE_ID_MASK)>>DATA_SOURCE_ID_MASK_RSHIFT;}
// Board Header 2
int FormatIdentifier (unsigned long datum) {return (datum & FORMAT_IDENTIFIER_MASK) >> FORMAT_IDENTIFIER_RSHIFT;}
int L0TriggerType (unsigned long datum) {return (datum & L0_TRIGGER_TYPE_MASK)>>L0_TRIGGER_TYPE_MASK_RSHIFT;}
unsigned long FPGAIDBoardHeader (unsigned long datum) {return (datum & FPGA_ID_BOARD_HEADER_MASK)>>FPGA_ID_BOARD_HEADER_RSHIFT;}
int NumberOfSpecialWords (unsigned long datum) {return (datum & SPECIAL_TRIGGER_NWORDS_MASK)>>SPECIAL_TRIGGER_NWORDS_RSHIFT;}
// FPGA Header
int FlagError (unsigned long datum) {return (datum & FLAG_ERROR_MASK)>>FLAG_ERROR_RSHIFT;}
unsigned long FPGAID (unsigned long datum) {return (datum & FPGA_ID_MASK)>>FPGA_ID_RSHIFT;}
int NFrames (unsigned long datum) {return ((datum & NUMBER_OF_FRAME_MASK)>>NUMBER_OF_FRAME_MASK_RSHIFT);}
// Frame Header
int NFrameWords (unsigned long datum) {return ((datum & NUMBER_OF_WORDS)>>NUMBER_OF_WORDS_RSHIFT);}
int FrameTimeStamp (unsigned long datum) {return ((datum & FRAME_TIME_STAMP)>>FRAME_TIME_STAMP_RSHIFT);}
// Frame Data
bool isTDCBLeading (unsigned long datum) {return ((datum & TDCB_TYPE_MASK)==TDCB_LEADING);}
bool isTDCBTrailing (unsigned long datum) {return ((datum & TDCB_TYPE_MASK)==TDCB_TRAILING);}
bool isTDCBError (unsigned long datum) {return ((datum & TDCB_TYPE_MASK)==TDCB_ERROR);}
int TDCID (unsigned long datum) {return (((datum & TDCB_TDC_MASK)>>TDCB_TDC_RSHIFT) );}
int TDCBChannelNumber (unsigned long datum) {return (((datum & TDCB_CHANNEL_MASK)>>TDCB_CHANNEL_RSHIFT) | ((datum & TDCB_TDC_MASK)>>TDCB_CHANNEL_RSHIFT) );}
long TDCBChannelValue (unsigned long datum) {return (datum & TDCB_DATA_MASK);}
// FPGA IDs
int FlagPPFPGA0 (unsigned long datum) {return (datum & FLAG_PP_FPGA0_MASK)>>PP_FPGA0_FLAG_RSHIFT;}
int FlagPPFPGA1 (unsigned long datum) {return (datum & FLAG_PP_FPGA1_MASK)>>PP_FPGA1_FLAG_RSHIFT;}
int FlagPPFPGA2 (unsigned long datum) {return (datum & FLAG_PP_FPGA2_MASK)>>PP_FPGA2_FLAG_RSHIFT;}
int FlagPPFPGA3 (unsigned long datum) {return (datum & FLAG_PP_FPGA3_MASK)>>PP_FPGA3_FLAG_RSHIFT;}
int FlagSLFPGA (unsigned long datum) {return (datum & FLAG_SL_FPGA_MASK)>>SL_FPGA_FLAG_RSHIFT;}
// Special triggers
bool isL0SpecialTrigger (int datum) {return ((datum & L0_SPECIAL_TRIGGER_MASK)!=0);}
bool isL0SOB (int datum) {return ((datum==L0_SOB));}
bool isL0EOB (int datum) {return ((datum==L0_EOB));}
//Special trigger Dim EOB header
bool isDIMEOB (int datum) {return ((datum & SPECIAL_TRIGGER_DIMEOB_DATABLOCK)==SPECIAL_TRIGGER_DIMEOB_DATABLOCK);}
int DIMEOBSubDetID (unsigned long datum) {return (datum & SPECIAL_TRIGGER_DIMEOB_SUBID)>>SPECIAL_TRIGGER_DIMEOB_SUBID_RSHIFT;}
int DIMEOBNumberOfWords (unsigned long datum) {return (datum & SPECIAL_TRIGGER_DIMEOB_NUMBER_OF_WORDS)>>SPECIAL_TRIGGER_DIMEOB_NUMBER_OF_WORDS_RSHIFT;}
