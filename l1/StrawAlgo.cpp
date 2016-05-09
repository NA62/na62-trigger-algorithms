/*
 * StrawAlgo.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk 
 */

#include "StrawAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <options/Logging.h>
#include <string.h>
#include <math.h>

#include <sys/time.h>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/SrbFragmentDecoder.h"
#include "straw_algorithm/ParsConfFile.h"
#include "straw_algorithm/ChannelID.h"
#include "straw_algorithm/DigiManager.h"
#include "straw_algorithm/Straw.h"

#define INVISIBLE_SHIFT 27 //28.4 //27
#define CLOCK_PERIOD 24.951059536

namespace na62 {

STRAWParsConfFile* StrawAlgo::infoSTRAW_ = STRAWParsConfFile::GetInstance();
int* StrawAlgo::strawGeo = infoSTRAW_->getGeoMap();
double* StrawAlgo::fROMezzaninesT0 = infoSTRAW_->getT0();
uint StrawAlgo::chRO[MAXNHITS];
int StrawAlgo::nHits = 0;
int StrawAlgo::chamberID = -1;
int StrawAlgo::viewID = -1;
int StrawAlgo::halfviewID = -1;
int StrawAlgo::planeID = -1;
int StrawAlgo::strawID = -1;
double StrawAlgo::leading = -100000;
double StrawAlgo::trailing = -100000;
float StrawAlgo::position = 0.0;
float StrawAlgo::wireDistance = -100.0;
bool StrawAlgo::tl_flag = 0;
STRAWChannelID StrawAlgo::strawChannel_;
DigiManager StrawAlgo::strawDigiMan_;
Straw StrawAlgo::strawPrecluster_[4][4][2][500];
int StrawAlgo::nStrawPrecluster[4][4][2];

uint_fast8_t StrawAlgo::processStrawTrigger(DecoderHandler& decoder) {

//  LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);

	using namespace l0;

	uint nEdges_tot = 0;
	nHits = 0;
	tl_flag = 0;
	double t0_main_shift = 6056.19; //TO BE REMOVED

	for (int i = 0; i != 4; ++i) {
		for (int j = 0; j != 4; ++j) {
			for (int h = 0; h != 2; ++h) {
				nStrawPrecluster[i][j][h] = 0;
			}
		}
	}

//	printf ("\nnevent = %d, event timestamp = %u, ClockPeriod %lf \n",decoder.getDecodedEvent()->getEventNumber(),decoder.getDecodedEvent()->getTimestamp(),CLOCK_PERIOD);

	//  TODO: chkmax need to be USED
//	LOG_INFO("receiving straw packet:");
	for (SrbFragmentDecoder* strawPacket_ : decoder.getSTRAWDecoderRange()) {

		const uint_fast8_t* strawAddr = strawPacket_->getStrawIDs();
		const double* edgeTime = strawPacket_->getTimes();
		const bool* edgeIsLeading = strawPacket_->getIsLeadings();
		const uint_fast8_t* srbAddr = strawPacket_->getSrbIDs();

		uint numberOfEdgesOfCurrentBoard =
				strawPacket_->getNumberOfEdgesStored();

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO((uint)srbAddr[iEdge] << " " << (uint)strawAddr[iEdge] << " " << time[iEdge] << " " << edgeIsLeading[iEdge]);
			chRO[nHits] = 256 * srbAddr[iEdge] + strawAddr[iEdge];

			strawChannel_.resetChannelID();
			strawChannel_.decodeChannelID(strawGeo[chRO[nHits]]);

			chamberID = strawChannel_.getChamberID();
			viewID = strawChannel_.getViewID();
			halfviewID = strawChannel_.getHalfViewID();
			planeID = strawChannel_.getPlaneID();
			strawID = strawChannel_.getStrawID();
			leading = -100000;
			trailing = -100000;
			position = 0.0;
			wireDistance = -100.0;

			int coverAddr = ((strawAddr[iEdge]&0xf0)>>4);

//			LOG_INFO(chRO[nHits] << " " << strawGeo[chRO[nHits]]);
//			LOG_INFO("ChamberID " << chamberID
//			<< " ViewID " << viewID
//			<< " HalfViewID " << halfviewID
//			<< " PlaneID " << planeID
//			<< " StrawID " << strawID);

			if(edgeIsLeading[iEdge]) leading = (double)edgeTime[iEdge] + (double)t0_main_shift - (double)fROMezzaninesT0[srbAddr[iEdge]*16+coverAddr] + (double)INVISIBLE_SHIFT - (((double)decoder.getDecodedEvent()->getFinetime()*CLOCK_PERIOD)/256 + 0.5);
//			if(edgeIsLeading[iEdge]) leading = (double)edgeTime[iEdge];
			if(!edgeIsLeading[iEdge]) trailing = (double)edgeTime[iEdge] + (double)t0_main_shift - (double)fROMezzaninesT0[srbAddr[iEdge]*16+coverAddr] + (double)INVISIBLE_SHIFT - (((double)decoder.getDecodedEvent()->getFinetime()*CLOCK_PERIOD)/256 + 0.5);
//			if(!edgeIsLeading[iEdge]) trailing = (double)edgeTime[iEdge];

//			printf ("number hit %d hit: %d %d %d %d %d %lf %lf %d %d %d\n", iEdge, chamberID, viewID, halfviewID, planeID, strawID, leading, trailing, edgeIsLeading[iEdge], srbAddr[iEdge], strawAddr[iEdge]);
			position = posTubNew(chamberID, viewID, halfviewID*2+planeID, strawID);

			//////////////PRECLUSTERING, first leading and last trailing//////////////////////////////
//			printf ("nhit of that half view %d \n",nStrawPrecluster[chamberID][viewID][halfviewID]);
			for(int j=0; j != nStrawPrecluster[chamberID][viewID][halfviewID]; ++j)
			{
//				printf ("inside loop: j=%d \n",j);
				if((strawPrecluster_[chamberID][viewID][halfviewID][j].plane == planeID) && (strawPrecluster_[chamberID][viewID][halfviewID][j].tube == strawID))
				{
					tl_flag = 1;
					if((edgeIsLeading[iEdge]) && (leading<strawPrecluster_[chamberID][viewID][halfviewID][j].leading || strawPrecluster_[chamberID][viewID][halfviewID][j].leading<=-10) && (leading>-10 && leading<300))
					{
						if(leading < 1) wireDistance = strawDigiMan_.rTDependenceData (1.0);
						else wireDistance = strawDigiMan_.rTDependenceData (leading / 1000 );

						strawPrecluster_[chamberID][viewID][halfviewID][j].leading=leading;
						strawPrecluster_[chamberID][viewID][halfviewID][j].wiredistance = wireDistance;
//						printf ("Straw completed\n");
					}
					else if((!edgeIsLeading[iEdge]) && (strawPrecluster_[chamberID][viewID][halfviewID][j].trailing<-100 || strawPrecluster_[chamberID][viewID][halfviewID][j].trailing<trailing) && (trailing>-100 && trailing<300))
					{
						strawPrecluster_[chamberID][viewID][halfviewID][j].trailing=trailing;
//						printf ("Straw completed\n");
					}
//					printf ("aggiorno l'hit: %d\n", j);
//					strawPrecluster_[chamberID][viewID][halfviewID][j].printStraw();
				}
			}
//			printf ("tl_flag = %d \n",tl_flag);
			if(!tl_flag)
			{
//				printf ("New straw:  ");
				if (leading > -100)
				if(leading < 1)
				wireDistance = 0.0;
				else
				wireDistance = strawDigiMan_.rTDependenceData (leading / 1000 );
				else
				wireDistance = -100.0;

				strawPrecluster_[chamberID][viewID][halfviewID][nStrawPrecluster[chamberID][viewID][halfviewID]].setStraw(chamberID,viewID,halfviewID,planeID,strawID,leading,trailing,0,srbAddr[iEdge],position,wireDistance);
//				printf ("creo un nuovo hit:\n");
//				strawPrecluster_[chamberID][viewID][halfviewID][nStrawPrecluster[chamberID][viewID][halfviewID]].printStraw();
				nStrawPrecluster[chamberID][viewID][halfviewID]++;
			}

			nHits++;
//			printf ("NHits: %d\n",nHits);
		}

		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

//	LOG_INFO("Angela: " << decoder.getDecodedEvent()->getEventNumber() << "\t" << decoder.getDecodedEvent()->getTimestamp() << "\t" << nEdges_tot);
	return 0; //return the Straw Trigger word!
}

float StrawAlgo::posTubNew(int chamber, int view, int plane, int jstraw) {

	float fStrawSpacing = 17.6;
	float posizione = 0;

	// X coordinate of the planes
	double_t xOffset[4][4];
	xOffset[0][0] = -1058.2;
	xOffset[0][1] = -1067.0;
	xOffset[0][2] = -1071.4;
	xOffset[0][3] = -1062.6;
	xOffset[1][0] = -1062.6;
	xOffset[1][1] = -1071.4;
	xOffset[1][2] = -1067.0;
	xOffset[1][3] = -1058.2;
	xOffset[2][0] = -1058.2;
	xOffset[2][1] = -1067.0;
	xOffset[2][2] = -1071.4;
	xOffset[2][3] = -1062.6;
	xOffset[3][0] = -1080.2;
	xOffset[3][1] = -1089.0;
	xOffset[3][2] = -1084.6;
	xOffset[3][3] = -1075.8;

	double_t xoffch[4][4];
	xoffch[0][0] = -0.0538315;
	xoffch[0][1] = 0.0857196;
	xoffch[0][2] = -0.1735152;
	xoffch[0][3] = 0.0267571;
	xoffch[1][0] = 0.2585159;
	xoffch[1][1] = 0.0453096;
	xoffch[1][2] = 0.1563879;
	xoffch[1][3] = -0.01980598;
	xoffch[2][0] = -0.00973369;
	xoffch[2][1] = 0.0212925;
	xoffch[2][2] = -0.3323366;
	xoffch[2][3] = 0.046397608;
	xoffch[3][0] = 0.03975005;
	xoffch[3][1] = 0.1124206;
	xoffch[3][2] = -0.02243413;
	xoffch[3][3] = -0.02177299;

	posizione = fStrawSpacing * jstraw + xOffset[view][plane]
			+ xoffch[chamber][view];

	return posizione;

}

} /* namespace na62 */

