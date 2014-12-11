/*
 * L1TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1TriggerProcessor.h"
#include <options/Logging.h>

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>

#include "data_decoder/CedarData.h"
//#include "data_decoder/CedarHits.h"
#include "cedar_algorithm/tdcb_buffer.h"

namespace na62 {

<<<<<<< HEAD
uint_fast8_t L1TriggerProcessor::bypassTriggerWord;
double L1TriggerProcessor::bypassProbability;

void L1TriggerProcessor::initialize(double _bypassProbability,
		uint _bypassTriggerWord) {
	// Seed for rand()
	srand (time(NULL));

	bypassProbability = _bypassProbability;
	bypassTriggerWord = _bypassTriggerWord;
}

=======

  L1TriggerProcessor::L1TriggerProcessor(){
    //nWordsPerFPGA = (uint*) calloc(maxNFPGA, sizeof(uint));
    //noFrame = (uint8_t*) calloc(maxNFPGA, sizeof(uint8_t));
    //noNonEmptyFrame = (uint8_t*) calloc(maxNFPGA, sizeof(uint8_t));
    //FPGAID = (uint8_t*) calloc(maxNFPGA, sizeof(uint8_t));
    //errFlags = (uint8_t*) calloc(maxNFPGA, sizeof(uint8_t));
    
    //coarseFrameTime = (uint16_t**) calloc(maxNFPGA, sizeof(uint16_t*));
    //nWordsPerFrame = (uint16_t**) calloc(maxNFPGA, sizeof(uint16_t*));
    //for(int j=0; j<maxNFPGA; j++){
    //coarseFrameTime[j] = (uint16_t*) calloc(maxNFrame, sizeof(uint16_t));
    //nWordsPerFrame[j] = (uint16_t*) calloc(maxNFrame, sizeof(uint16_t));
    //}
  }
  
  //L1TriggerProcessor::~L1TriggerProcessor(){
    // TODO Auto-generated destructor stub
    //free(nWordsPerFPGA);
    //free(noFrame);
    //free(noNonEmptyFrame);
    //free(FPGAID);
    //free(errFlags);
    //free(coarseFrameTime);
    //free(nWordsPerFrame);
    //free(Time);
    //free(chID);
    //free(tdcID);
    //free(ID);
  //}
  
>>>>>>> On branch feature/ktag
uint8_t L1TriggerProcessor::compute(Event* event) {
  using namespace l0;

  CedarData cedarPacket[32];
  int nhits_perTrb[32];
  int sector_occupancy[8];
  int nSectors = 0;
  //CedarHits cedarHits[maxNhit];

  uint nWordsPerFPGA[maxNFPGA];
  //uint8_t noFrame[maxNFPGA];
  //uint8_t noNonEmptyFrame[maxNFPGA];
  //uint8_t FPGAID[maxNFPGA];
  //uint8_t errFlags[maxNFPGA];
  uint16_t coarseFrameTime[maxNFPGA][maxNFrame];
  uint16_t nWordsPerFrame[maxNFPGA][maxNFrame];
  uint32_t time[maxNhit];
  uint8_t chID[maxNhit];
  uint8_t tdcID[maxNhit];
  uint8_t ID[maxNhit];
  int pp[maxNhit];
  int tdc[maxNhit];
  int box[maxNhit];

  for(int i=0; i<maxNhit; i++){
    if(i<8) sector_occupancy[i]=0;
    time[i] = 0;
    chID[i] = 0;
    tdcID[i] = 0;
    ID[i] = 0;
    pp[i] = -1;
    tdc[i] = -1;
    box[i] = -1;
  }

  uint nWords = 0;
  uint nhits = 0;
  uint nhits_tot = 0;
  uint nhits_tot_check = 0;

  for(int iFPGA=0; iFPGA<maxNFPGA; iFPGA++){
    nWordsPerFPGA[iFPGA] = 0; 
  }

  uint chkmax = 0;

  //std::cout << std::endl;
  //std::cout << "........................event number = " << event->getEventNumber() << std::endl;
  //printf("timestamp = %x\n",event->getTimestamp());

  l0::Subevent* cedarSubevent = event->getCEDARSubevent();
  
  for (int trbNum = 0; trbNum != cedarSubevent->getNumberOfFragments() && chkmax == 0; trbNum++){
    l0::MEPFragment* trbDataFragment = cedarSubevent->getFragment(trbNum);
    
    cedarPacket[trbNum].SetHits(trbDataFragment);
    nhits_perTrb[trbNum] = 0;

<<<<<<< HEAD
	/*
	 * Check if the event should bypass the processing
	 */
	if (bypassEvent()) {
		return bypassTriggerWord;
	}

	event->setProcessingID(0); // 0 indicates raw data as collected from the detector
	return 1;

}

=======
    //if(nWords>=250){
    //std::cout << "Flags = " << (int) cedarPacket[trbNum].cedarHeader->flags << std::endl;
    //std::cout << "L0 trigger type = " << (int) cedarPacket[trbNum].cedarHeader->triggerType << std::endl;
    //std::cout << "Source sub-ID = " << (int) cedarPacket[trbNum].cedarHeader->sourceSubID << std::endl;
    //std::cout << "Format = " << (int) cedarPacket[trbNum].cedarHeader->format << std::endl;
    //}

    for(int iFPGA=0; iFPGA<maxNFPGA; iFPGA++){      
      //noFrame[iFPGA] = (uint8_t) (cedarPacket[trbNum].cedar_fpgaHeader[iFPGA]->noFrame);
      //noNonEmptyFrame[iFPGA] = (uint8_t)cedarPacket[trbNum].cedar_fpgaHeader[iFPGA]->noNonEmptyFrame;
      //FPGAID[iFPGA] = (uint8_t) cedarPacket[trbNum].cedar_fpgaHeader[iFPGA]->FPGAID;
      //errFlags[iFPGA] = (uint8_t) cedarPacket[trbNum].cedar_fpgaHeader[iFPGA]->errFlags;

      //if(nWords>=250){
      //printf("noFrame[%d] %08x\n",iFPGA,noFrame[iFPGA]);
      //printf("noNonEmptyFrame[%d] %08x\n",iFPGA,noNonEmptyFrame[iFPGA]);
      //printf("FPGAID[%d] %08x\n",iFPGA,FPGAID[iFPGA]);
      //printf("errFlags[%d] %08x\n",iFPGA,errFlags[iFPGA]);
      //}
      for(int iFrame=0; iFrame<maxNFrame; iFrame++){
	
	coarseFrameTime[iFPGA][iFrame] = (uint16_t) cedarPacket[trbNum].cedar_frameHeader[iFPGA][iFrame]->coarseFrameTime;
	nWordsPerFrame[iFPGA][iFrame] = (uint16_t) cedarPacket[trbNum].cedar_frameHeader[iFPGA][iFrame]->nWordsPerFrame;
	
	coarseFrameTime[iFPGA][iFrame] += (event->getTimestamp() & 0xFFFF0000);
	if((event->getTimestamp()&0xf000)==0xf000 && (coarseFrameTime[iFPGA][iFrame]&0xf000)==0x0000) coarseFrameTime[iFPGA][iFrame] += 0x10000; //16 bits overflow
	if((event->getTimestamp()&0xf000)==0x0000 && (coarseFrameTime[iFPGA][iFrame]&0xf000)==0xf000) coarseFrameTime[iFPGA][iFrame] -= 0x10000; //16 bits overflow
	
	nWordsPerFPGA[iFPGA] += nWordsPerFrame[iFPGA][iFrame];
	nWords += nWordsPerFrame[iFPGA][iFrame];
	
	//if(nWords>=250){
      	//	printf("Frame TimeStamp[%d][%d] = %04x\n",iFPGA,iFrame,coarseFrameTime[iFPGA][iFrame]);
      	//	printf("nWordsPerFrame[%d][%d] %d\n",iFPGA,iFrame,(int)nWordsPerFrame[iFPGA][iFrame]);
      	//	printf("nWordsPerFPGA[%d] %d\n",iFPGA,(int)nWordsPerFPGA[iFPGA]);
      	//	printf("nWords %d\n",(int)nWords);
      	//}
	
      	nhits = nWordsPerFrame[iFPGA][iFrame] - 1;
      	if(nhits){
	  for(int ihit=0; ihit<nhits; ihit++){
	    time[ihit+nhits_tot] = (uint32_t) cedarPacket[trbNum].tdc_data[ihit+nhits_perTrb[trbNum]]->Time;
	    chID[ihit+nhits_tot] = (uint8_t) cedarPacket[trbNum].tdc_data[ihit+nhits_perTrb[trbNum]]->chID;
	    tdcID[ihit+nhits_tot] = (uint8_t) cedarPacket[trbNum].tdc_data[ihit+nhits_perTrb[trbNum]]->tdcID;
	    ID[ihit+nhits_tot] = (uint8_t) cedarPacket[trbNum].tdc_data[ihit+nhits_perTrb[trbNum]]->ID;
	    
	    //printf("ID[%d] %x \n",ihit+nhits_tot,ID[ihit+nhits_tot]);
	    //printf("Time[%d] %08x \n",ihit+nhits_tot,time[ihit+nhits_tot]);
	    //printf("ChID[%d] %x \n",ihit+nhits_tot,chID[ihit+nhits_tot]);
	    //printf("TDCID[%d] %x \n",ihit+nhits_tot,tdcID[ihit+nhits_tot]);
	    //printf("TrbNum %d\n",trbNum);
	    
	    if(ihit==(nhits-1)){
	      nhits_tot+=nhits;
	      nhits_perTrb[trbNum]+=nhits;
	    }
	  }
      	}
      }
    }
    
    for(uint ihit=nhits_tot_check; ihit<maxNhit; ihit++){
      if(ID[ihit]){ 
	//printf("hit[%d] ID %d\n",ihit,(uint) ID[ihit]); 
	//printf("hit[%d] time %08x\n",ihit,(uint32_t) time[ihit]);
	//printf("hit[%d] chID %d\n",ihit,(uint) chID[ihit]);
	//printf("hit[%d] tdcID %x\n",ihit,(uint8_t) tdcID[ihit]);
	pp[ihit] = tdcID[ihit]/4;
	tdc[ihit] = tdcID[ihit]%4;
	//printf("pp[%d] %d\n",ihit,pp[ihit]);
	//printf("tdc[%d] %d\n",ihit,tdc[ihit]);
	
	if((trbNum==0) && ((pp[ihit]==0) || (pp[ihit]==1) || (pp[ihit]==2))) box[ihit]=1;
	else if((trbNum==0) && (pp[ihit]==3)) box[ihit]=2;
	else if((trbNum==1) && ((pp[ihit]==0) || (pp[ihit]==1))) box[ihit]=2;
	else if((trbNum==1) && ((pp[ihit]==2) || (pp[ihit]==3))) box[ihit]=3;
	else if((trbNum==2) && (pp[ihit]==0)) box[ihit]=3;
	else if((trbNum==2) && ((pp[ihit]==1) || (pp[ihit]==2) || (pp[ihit]==3))) box[ihit]=4;
	else if((trbNum==3) && ((pp[ihit]==0) || (pp[ihit]==1) || (pp[ihit]==2))) box[ihit]=5;
	else if((trbNum==3) && (pp[ihit]==3)) box[ihit]=6;
	else if((trbNum==4) && ((pp[ihit]==0) || (pp[ihit]==1))) box[ihit]=6;
	else if((trbNum==4) && ((pp[ihit]==2) || (pp[ihit]==3))) box[ihit]=7;
	else if((trbNum==5) && (pp[ihit]==0)) box[ihit]=7;
	else if((trbNum==5) && ((pp[ihit]==1) || (pp[ihit]==2) || (pp[ihit]==3))) box[ihit]=8;
	else printf("Combination KTAG %d, PP %d not found \n",trbNum,pp[ihit]);
	//printf("Found Sector %d\n",box[ihit]);

	sector_occupancy[box[ihit]-1]++;
	nhits_tot_check++;

	printf("%d\t%x\t%d\t%d\t%d\t%d\n",event->getEventNumber(),event->getTimestamp(),ID[ihit],chID[ihit],tdcID[ihit],box[ihit]);

      }
    }
  }
  
  for(int iSec=0; iSec<8; iSec++){
    if(sector_occupancy[iSec]) nSectors++;
  }

  //std::cout << "NSectors " << nSectors << std::endl;

  event->setProcessingID(0); // 0 indicates raw data as collected from the detector
  
  return 1;
}



>>>>>>> On branch feature/ktag
} /* namespace na62 */
