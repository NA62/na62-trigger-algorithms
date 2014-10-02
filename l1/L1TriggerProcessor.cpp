/*
 * L1TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1TriggerProcessor.h"

#include <eventBuilding/Event.h>
#include <l0/Subevent.h>

#include "data_decrypter/StrawData.h"
#include "data_decrypter/StrawHits.h"
#include "straw_algorithm/ViewCluster.h"

namespace na62
    {

    int counter;
    uint16_t L1TriggerProcessor::compute(Event* event)
	{
	using namespace l0;

	StrawData strawPacket[32];
	StrawHits hitPlane[32];
//	ViewCluster ViewC_0V,ViewC_0U,ViewC_0X,ViewC_0Y;
//	ViewCluster ViewC_1V,ViewC_1U,ViewC_1X,ViewC_2Y;
//	ViewCluster ViewC_2V,ViewC_2U,ViewC_2X,ViewC_3Y;
//	ViewCluster ViewC_3V,ViewC_3U,ViewC_3X,ViewC_4Y;

	ViewCluster ViewC_0[4];
	ViewCluster ViewC_1[4];
	ViewCluster ViewC_2[4];
	ViewCluster ViewC_3[4];

	uint chkmax = 0;

	std::cout << "event number = " << event->getEventNumber() << std::endl;
	std::cout << "timestamp = " << event->getTimestamp() << std::endl;

	l0::Subevent* strawSubevent = event->getSTRAWSubevent();

	for (int srbNum = 0; srbNum != strawSubevent->getNumberOfFragments() && chkmax == 0; srbNum++)
	    {
	    l0::MEPFragment* srbDataFragment = strawSubevent->getFragment(srbNum); //to add counter reading

	    strawPacket[srbNum].SetHits(srbDataFragment);

	    std::cout << "packet length = " << (int) strawPacket[srbNum].strawHeader->packetLength << " nhit in the packet = " << strawPacket[srbNum].nhits << std::endl;
	    std::cout << "coarse time = " << (int) strawPacket[srbNum].strawHeader->coarseTime << std::endl;
	    std::cout << "hit0: err= " << (int) strawPacket[srbNum].hits[0].err << " straw id= " << (int) strawPacket[srbNum].hits[0].strawID << " edge= " << (int) strawPacket[srbNum].hits[0].edge
		    << " fine time= " << (int) strawPacket[srbNum].hits[0].fineTime << std::endl;
	    std::cout << "hit1: err= " << (int) strawPacket[srbNum].hits[1].err << " straw id= " << (int) strawPacket[srbNum].hits[1].strawID << " edge= " << (int) strawPacket[srbNum].hits[1].edge
		    << " fine time= " << (int) strawPacket[srbNum].hits[1].fineTime << std::endl;

	    if (strawPacket[srbNum].nhits < maxnhit)
		{

		hitPlane[srbNum].SetHits(strawPacket[srbNum]); //To add subtraction with trigger time and temporal shift plane position dependent and leading time windows cut, what do we do about the hits with only a leading o a trailing

		}
	    else
		chkmax = 1;

	    if (chkmax == 0)
		{

		for(int view=0;view < 4; view ++)
		ViewC_0[view].SetCluster(hitPlane[2*view], hitPlane[2*view+1]);
		for(int view=0;view < 4; view ++)
		ViewC_1[view].SetCluster(hitPlane[2*view+8], hitPlane[2*view+9]);
		for(int view=0;view < 4; view ++)
		ViewC_2[view].SetCluster(hitPlane[2*view+16], hitPlane[2*view+17]);
		for(int view=0;view < 4; view ++)
		ViewC_3[view].SetCluster(hitPlane[2*view+24], hitPlane[2*view+25]);


		}

	    }

//	long sum = 0;
//	for (int sourceIDNum = 0;
//			sourceIDNum != SourceIDManager::NUMBER_OF_L0_DATA_SOURCES; sourceIDNum++) {
//		Subevent* subevent = event->getL0SubeventBySourceIDNum(sourceIDNum);

//		Subevent* lav = event->getLAVSubevent();
//		for (int p = lav->getNumberOfParts() - 1; p >= 0; p--) {
//			MEPFragment* fragment = lav->getPart(p);
//			const MEPFragment_HDR* data = fragment->getData();
//
//			for(int i=0; i<fragment->getDataLength(); i++){
//				sum+=((char*)data)[i];
//			}
//		}

//	}
	/*
	 * Accessing the raw data of one Detector (MUV):
	 */
//	Subevent* muv = event->getMUVSubevent();
//	for (int p = muv->getNumberOfParts() - 1; p >= 0; p--) {
//		MEPEvent* mepData = muv->getPart(p);
//		mepData->getSourceID();
//		mepData->getData()
//		mepData->getEventLength()
//		mepData->getSourceIDNum()
//	}
	/*
	 * The following values have to be calculated by the L0TP-packet
	 * L0TP_RAW is to be defined
	 */
//	l0::MEPEvent* L0TPEvent = event->getL0TPSubevent()->getPart(0);
//	L0TP_RAW* L0TPData = (L0TP_RAW*) L0TPEvent->getData();
//	event->setFinetime(L0TPData->fineTime);
	event->setProcessingID(0); // 0 indicates raw data as collected from the detector

//	return (sum%2)+1;

	return 0;
	}
    } /* namespace na62 */
