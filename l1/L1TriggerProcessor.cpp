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
#include "straw_algorithm/cut.h"

namespace na62
    {

    int counter;
    uint16_t L1TriggerProcessor::compute(Event* event)
	{
	using namespace l0;

	StrawData strawPacket[32];
	PlaneHit hitPlane[16];
//	ViewCluster ViewC_0V,ViewC_0U,ViewC_0X,ViewC_0Y;
//	ViewCluster ViewC_1V,ViewC_1U,ViewC_1X,ViewC_2Y;
//	ViewCluster ViewC_2V,ViewC_2U,ViewC_2X,ViewC_3Y;
//	ViewCluster ViewC_3V,ViewC_3U,ViewC_3X,ViewC_4Y;

	ViewCluster ViewC_0[4];
	ViewCluster ViewC_1[4];
	ViewCluster ViewC_2[4];
	ViewCluster ViewC_3[4];

	uint chkmax = 0;
	uint32_t triggerCoarse = event->getTimestamp();
	uint8_t triggerFine = event->getFinetime();

	std::cout << "event number = " << event->getEventNumber() << std::endl;
	std::cout << "timestamp = " << event->getTimestamp() << std::endl;

	l0::Subevent* strawSubevent = event->getSTRAWSubevent();

	for (int srbNum = 0; srbNum != strawSubevent->getNumberOfFragments() && chkmax == 0; srbNum++)
	    {
	    l0::MEPFragment* srbDataFragment = strawSubevent->getFragment(srbNum);

	    strawPacket[srbNum].SetHits(srbDataFragment);

	    std::cout << "packet length = " << (int) strawPacket[srbNum].strawHeader->packetLength << " nhit in the packet = " << strawPacket[srbNum].nhits << std::endl;
	    std::cout << "coarse time = " << (int) strawPacket[srbNum].strawHeader->coarseTime << std::endl;
	    std::cout << "hit0: err= " << (int) strawPacket[srbNum].hits[0].err << " straw id= " << (int) strawPacket[srbNum].hits[0].strawID << " edge= " << (int) strawPacket[srbNum].hits[0].edge
		    << " fine time= " << (int) strawPacket[srbNum].hits[0].fineTime << std::endl;
	    std::cout << "hit1: err= " << (int) strawPacket[srbNum].hits[1].err << " straw id= " << (int) strawPacket[srbNum].hits[1].strawID << " edge= " << (int) strawPacket[srbNum].hits[1].edge
		    << " fine time= " << (int) strawPacket[srbNum].hits[1].fineTime << std::endl;

	    if (strawPacket[srbNum].nhits < maxNhit)
		{

		hitPlane[srbNum / 2].SetHits(strawPacket[srbNum], triggerCoarse, triggerFine); //To add subtraction  with temporal shift plane position dependent, what do we do about the hits with only a leading or a trailing

		}
	    else
		chkmax = 1;

	    if (chkmax == 0)
		{

		for (int view = 0; view < 4; view++)
		    ViewC_0[view].SetCluster(hitPlane[view].halfViewZero, hitPlane[view].halfViewOne);
		for (int view = 0; view < 4; view++)
		    ViewC_1[view].SetCluster(hitPlane[view + 4].halfViewZero, hitPlane[view + 4].halfViewOne);
		for (int view = 0; view < 4; view++)
		    ViewC_2[view].SetCluster(hitPlane[view + 8].halfViewZero, hitPlane[view + 8].halfViewOne);
		for (int view = 0; view < 4; view++)
		    ViewC_3[view].SetCluster(hitPlane[view + 12].halfViewZero, hitPlane[view + 12].halfViewOne);

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
