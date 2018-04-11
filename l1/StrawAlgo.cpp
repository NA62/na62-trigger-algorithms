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
#include "L1TriggerProcessor.h"

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/SrbFragmentDecoder.h"
#include "straw_algorithm/ParsConfFile.h"
#include "straw_algorithm/ChannelID.h"
#include "straw_algorithm/DigiManager.h"
#include "straw_algorithm/Straw.h"
#include "straw_algorithm/Cluster.h"
#include "straw_algorithm/Point.h"
#include "straw_algorithm/Track.h"

#define MAX_N_HIT_C 6
#define MAX_N_HIT_L 6
#define MAX_N_ADD_HIT 50

//#define INVISIBLE_SHIFT 25.2 //28.4 //27
#define CLOCK_PERIOD 24.951059536
//#define rangem 160//160
#define PASSO 0.0004//0.0002
//#define rangeq 200//200
#define LMAGNET 3000
#define ZMAGNET 197645

namespace na62 {

uint StrawAlgo::AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
uint StrawAlgo::AlgoLogic_[16];
uint StrawAlgo::AlgoRefTimeSourceID_[16];
double StrawAlgo::AlgoOnlineTimeWindow_[16];

STRAWParsConfFile* StrawAlgo::InfoSTRAW_ = STRAWParsConfFile::GetInstance();
int* StrawAlgo::StrawGeo_ = InfoSTRAW_->getGeoMap();
double* StrawAlgo::ROMezzaninesT0_ = InfoSTRAW_->getT0();
double StrawAlgo::StationT0_ = InfoSTRAW_->getStationT0();
double StrawAlgo::MagicT0_ = InfoSTRAW_->getMagicT0();

//Defualt Values - TO BE REMOVED
//double StrawAlgo::t0_main_shift = 6056.19;
//double StrawAlgo::cutlowleading = -100.0; //0
//double StrawAlgo::cuthighleading = 300.0; //170
//double StrawAlgo::cutlowtrailing = -100.0; //-70
//double StrawAlgo::cuthightrailing = 300.0; //
//double StrawAlgo::m1leadtrail = 1.25;
//double StrawAlgo::q1leadtrail = 150.0; //-50
//double StrawAlgo::m2leadtrail = 4.7;
//double StrawAlgo::q2leadtrail = -1400.0;
//double StrawAlgo::hit3low = 0.; //7.9
//double StrawAlgo::hit3high = 12.0; //9.5
//double StrawAlgo::hit2low = 0.; //2.8
//double StrawAlgo::hit2high = 12.0; //5.6
//int StrawAlgo::CutCluster_ = 20; //10

////////// Run 3809 - 1% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5997.000;
////////// Run 5358 - 1% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5986.95;
////////// Run 5495 - 3% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5989.12;
////////// Run 5525 - 3% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5992.36;
////////// Run 5552 - 3% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5989.60;
////////// Run 5565 - 3% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5991.45;
////////// Run 5586 - 3% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5996.65;
////////// Run 5617 - 16% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5990.55;
////////// Run 5646 - 16% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5992.89;
////////// Run 5670 - 16% Beam Intensity ///////
//double StrawAlgo::t0_main_shift = 5991.01;

const double StrawAlgo::CutLowLeading_ = 0.0; //-10
const double StrawAlgo::CutHighLeading_ = 175.0; //165
const double StrawAlgo::CutLowTrailing_ = 55.0; //-50
const double StrawAlgo::CutHighTrailing_ = 250.0;
const int StrawAlgo::CutCluster_ = 6;
const double StrawAlgo::M1LeadTrail_ = 1.35;
const double StrawAlgo::Q1LeadTrail_ = -50.0;
const double StrawAlgo::M2LeadTrail_ = 4.8;
const double StrawAlgo::Q2LeadTrail_ = -1150.0; //-1200
const double StrawAlgo::Hit3Low_ = 8.6;
const double StrawAlgo::Hit3High_ = 9.0;
const double StrawAlgo::Hit2Low_ = 2.9;
const double StrawAlgo::Hit2High_ = 5.8;

const float StrawAlgo::ChamberZPosition_[4] = { (0.5 * (183311.1 + 183704.9)), (0.5 * (193864.1 + 194262.9) + 2.5), (0.5
		* (204262.1 + 204655.9)), (0.5 * (218688.1 + 219081.9)) };

// X coordinate of the planes
const double StrawAlgo::XOffset_[4][4] = { { -1058.2, -1067.0, -1071.4, -1062.6 }, { -1062.6, -1071.4, -1067.0, -1058.2 }, { -1058.2,
		-1067.0, -1071.4, -1062.6 }, { -1080.2, -1089.0, -1084.6, -1075.8 } };
const double StrawAlgo::XOffCh_[4][4] = { { -0.0538315, 0.0857196, -0.1735152, 0.0267571 },
		{ 0.2585159, 0.0453096, 0.1563879, -0.01980598 }, { -0.00973369, 0.0212925, -0.3323366, 0.046397608 }, { 0.03975005, 0.1124206,
				-0.02243413, -0.02177299 } };

const float StrawAlgo::HoleChamberMax_[4][4] = { { 134.2, 134.2, 165.0, 63.8 }, { 143.0, 143.0, 178.2, 63.8 },
		{ 129.8, 129.8, 156.2, 63.8 }, { 103.4, 103.4, 116.6, 63.8 } };

const float StrawAlgo::HoleChamberMin_[4][4] = { { 6.6, 6.6, 37.4, -63.8 }, { 15.4, 15.4, 50.6, -63.8 }, { 2.2, 2.2, 28.6, -63.8 }, { -24.2,
		-24.2, -11.0, -63.8 } };

const Point StrawAlgo::QBeam_(0.0, 114.0, 0.0, 0.0, 0.0, 0, 0);
const Point StrawAlgo::MBeam_(1.0, 0.0012, 0.0, 0.0, 0.0, 0, 0);
const double StrawAlgo::Sq2_ = sqrt(2);
const double StrawAlgo::InvSq2_ = 1. / Sq2_;

StrawAlgo::StrawAlgo() {
	// Nothing to do at this time
}

void StrawAlgo::initialize(uint i, l1Straw& l1StrawStruct) {

	AlgoID_ = l1StrawStruct.configParams.l1TrigMaskID;
	AlgoLogic_[i] = l1StrawStruct.configParams.l1TrigLogic;
	AlgoRefTimeSourceID_[i] = l1StrawStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	AlgoOnlineTimeWindow_[i] = l1StrawStruct.configParams.l1TrigOnlineTimeWindow;

	//	LOG_INFO("Straw mask: " << i << " logic " << AlgoLogic_[i] << " refTimeSourceID " << AlgoRefTimeSourceID_[i] << " online time window " << AlgoOnlineTimeWindow_[i]);
}

uint_fast8_t StrawAlgo::processStrawTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info) {

	//struct timeval time[30];
	//gettimeofday(&time[0], 0);
	//LOG_INFO( "Initial Time - Start " << time[0].tv_sec << " " << time[0].tv_usec );

	using namespace l0;

	int flagL1 = 0;
	int flagL1Pnn = 0;
	int flagL1Exotic = 0;
	int flagL1Limit[1000];
	int flagL1Three[1000] = { 0 };
	int flagL1TreTracks = 0;

	uint nEdgesTotal = 0;
	uint nHits = 0;
	bool tlFlags = 0;
	int nChambersHit = 0;

	int nTotalHit = 0; //just for debug
	int nTotalViewCluster = 0; //just for debug
	int nTotalPreclusters = 0; //just for debug

	int nTracletCondivisi;
	int nTrackIntermedie;

	Track trackIntermedieTemp;
	float trackIntermedieTemp_my;
	float trackIntermedieTemp_qy;
	float trackIntermedieTemp_m1x;
	float trackIntermedieTemp_q1x;
	float trackIntermedieTemp_m2x;
	float trackIntermedieTemp_q2x;

	int tempCondivise;

	int nStrawPointsTemp[4] = { 0 };
	int nStrawPointsTempBis[4] = { 0 };
	int nStrawPointsFinal[4] = { 0 };
	int nStrawClusters[4][4] = { 0 };
	int nStrawPreclusters[4][4][2] = { 0 };

	Point qTrack;
	Point mTrack;
	Point vertex;
	long long hought[RANGEM][RANGEQ] = { 0 };

	//gettimeofday(&time[1], 0);
	//	LOG_INFO( "Preparazione Vettori - Stop " << time[1].tv_sec << " " << time[1].tv_usec );
	//	LOG_INFO( "Preparazione Vettori " << ((time[1].tv_sec - time[0].tv_sec)*1e6 + time[1].tv_usec) - time[0].tv_usec );

	//  TODO: chkmax need to be USED

	/*
	 * Pre-clustering: looping over SRB readout boards - read all edges and perform first stage of clustering
	 *
	 */
	//gettimeofday(&time[2], 0);
	//	LOG_INFO( "Access All Straw Data Packets & PreClustering - Start " << time[2].tv_sec << " " << time[2].tv_usec );
	for (SrbFragmentDecoder* strawPacket_ : decoder.getSTRAWDecoderRange()) {
		//	gettimeofday(&time[3], 0);
		//	LOG_INFO( "Access Packets - Start " << time[3].tv_sec << " " << time[3].tv_usec );

		if (!strawPacket_->isReady() || strawPacket_->isBadFragment()) {
			LOG_ERROR("STRAW: This looks like a Bad Packet!!!! ");
			l1Info->setL1StrawBadData();
			return 0;
		}

		/**
		 * Get Arrays with hit Info
		 */
		const uint_fast8_t* strawAddr = strawPacket_->getStrawIDs();
		const double* edgeTime = strawPacket_->getTimes();
		const bool* edgeIsLeading = strawPacket_->getIsLeadings();
		const uint_fast8_t* srbAddr = strawPacket_->getSrbIDs();

		uint numberOfEdgesOfCurrentBoard = strawPacket_->getNumberOfEdgesStored();

		nTotalHit += numberOfEdgesOfCurrentBoard;

		//gettimeofday(&time[4], 0);
		//		LOG_INFO( "Access Packets - Stop " << time[4].tv_sec << " " << time[4].tv_usec );
		//		LOG_INFO( "Access SRB Packets " << ((time[4].tv_sec - time[3].tv_sec)*1e6 + time[4].tv_usec) - time[3].tv_usec );
		//LOG_INFO( "nhit " << numberOfEdgesOfCurrentBoard);

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {

			//LOG_INFO("srb_add = " << (uint)srbAddr[iEdge] << " straw_add = " << (uint)strawAddr[iEdge] << " edge(lead)= " << edgeIsLeading[iEdge]);
			//	gettimeofday(&time[5], 0);
			//	LOG_INFO( "Read Config File and Assign ChannelID - Start " << time[5].tv_sec << " " << time[5].tv_usec );

			tlFlags = 0;

			const int roChID = 256 * srbAddr[iEdge] + strawAddr[iEdge];

			int chamberID = StrawGeo_[roChID] / 1952;
			int viewID = (StrawGeo_[roChID] % 1952) / 488;
			int halfViewID = (StrawGeo_[roChID] % 488) / 244;
			int planeID = (StrawGeo_[roChID] % 244) / 122;
			int strawID = StrawGeo_[roChID] % 122;
			float leading = -100000.;
			float trailing = -100000.;
			float wireDistance = -100.0;

			int coverAddr = ((strawAddr[iEdge] & 0xf0) >> 4);
			//LOG_INFO( "SrbAddr " << (uint)srbAddr[iEdge] << " StrawAddr "<< (uint)strawAddr[iEdge] << " CoverAddr " << coverAddr << " fR0Mezz Index " << srbAddr[iEdge] * 16 + coverAddr );

			//LOG_INFO(chRO[nHits] << " " << strawGeo[chRO[nHits]]);
			//LOG_INFO("ChamberID " << chamberID << " ViewID " << viewID << " HalfViewID " << halfViewID << " PlaneID " << planeID);
			//LOG_INFO(" StrawID " << strawID << " IsALeading " << edgeIsLeading[iEdge]);

			if (edgeIsLeading[iEdge]) {
				leading = (double) edgeTime[iEdge] - (double) StationT0_ - (double) ROMezzaninesT0_[srbAddr[iEdge] * 16 + coverAddr]
						+ (double) MagicT0_ - (((double) decoder.getDecodedEvent()->getFinetime() * CLOCK_PERIOD) / 256 + 0.5);
			}
			if (!edgeIsLeading[iEdge]) {
				trailing = (double) edgeTime[iEdge] - (double) StationT0_ - (double) ROMezzaninesT0_[srbAddr[iEdge] * 16 + coverAddr]
						+ (double) MagicT0_ - (((double) decoder.getDecodedEvent()->getFinetime() * CLOCK_PERIOD) / 256 + 0.5);
			}
			//gettimeofday(&time[6], 0);
			//LOG_INFO( "Read Config File and Assign ChannelID - Stop " << time[6].tv_sec << " " << time[6].tv_usec );
			//LOG_INFO( "Read Conf file and Assign ChannelID and time" << ((time[6].tv_sec - time[5].tv_sec)*1e6 + time[6].tv_usec) - time[5].tv_usec );
			//gettimeofday(&time[7], 0);
			//	LOG_INFO( "Access Straw Map - Start " << time[7].tv_sec << " " << time[7].tv_usec );
			float position = posTubNew(chamberID, viewID, halfViewID * 2 + planeID, strawID);

			//gettimeofday(&time[8], 0);
			//	LOG_INFO( "Access Straw Map - Stop " << time[8].tv_sec << " " << time[8].tv_usec );
			//	LOG_INFO( "Access Straw Map and give position of the tube" << ((time[8].tv_sec - time[7].tv_sec)*1e6 + time[8].tv_usec) - time[7].tv_usec );

			//////////////PRECLUSTERING, first leading and last trailing//////////////////////////////

			//gettimeofday(&time[9], 0);
			//	LOG_INFO( "Preclustering - Start " << time[9].tv_sec << " " << time[9].tv_usec );
			//LOG_INFO( "Preparazione degli hit " << ((time[9].tv_sec - time[0].tv_sec)*1e6 + time[9].tv_usec) - time[0].tv_usec );

			for (int j = 0; j != nStrawPreclusters[chamberID][viewID][halfViewID]; ++j) {
				if ((strawPrecluster_[chamberID][viewID][halfViewID][j].plane == planeID)
						&& (strawPrecluster_[chamberID][viewID][halfViewID][j].tube == strawID)) {
					tlFlags = 1;
					if ((edgeIsLeading[iEdge])
							&& (leading < strawPrecluster_[chamberID][viewID][halfViewID][j].leading
									|| strawPrecluster_[chamberID][viewID][halfViewID][j].leading <= -10)
							&& (leading > -10 && leading < 300)) {
						if (leading < 1)
							wireDistance = DigiManager::rTDependenceData(1.0);
						else
							wireDistance = DigiManager::rTDependenceData(leading / 1000);

						strawPrecluster_[chamberID][viewID][halfViewID][j].leading = leading;
						strawPrecluster_[chamberID][viewID][halfViewID][j].wiredistance = wireDistance;
					} else if ((!edgeIsLeading[iEdge])
							&& (strawPrecluster_[chamberID][viewID][halfViewID][j].trailing < -100
									|| strawPrecluster_[chamberID][viewID][halfViewID][j].trailing < trailing)
							&& (trailing > -100 && trailing < 300)) {
						strawPrecluster_[chamberID][viewID][halfViewID][j].trailing = trailing;
					}
				}
			}
			if (!tlFlags) {
				if (leading > -100)
					if (leading < 1)
						wireDistance = 0.0;
					else
						wireDistance = DigiManager::rTDependenceData(leading / 1000);
				else
					wireDistance = -100.0;

				strawPrecluster_[chamberID][viewID][halfViewID][nStrawPreclusters[chamberID][viewID][halfViewID]].setStraw(chamberID,
						viewID, halfViewID, planeID, strawID, leading, trailing, 0, srbAddr[iEdge], position, wireDistance);
				nStrawPreclusters[chamberID][viewID][halfViewID]++;
				if (nStrawPreclusters[chamberID][viewID][halfViewID] >= MAX_N_PRECLUSTER) {
					return StrawAlgo::abortProcessing(l1Info);
				}
			}

			nHits++;
			if (nHits >= MAX_N_HITS) {
				return StrawAlgo::abortProcessing(l1Info);
			}
			//gettimeofday(&time[10], 0);
			//			LOG_INFO( "Preclustering - Stop " << time[10].tv_sec << " " << time[10].tv_usec );
			//LOG_INFO( "Preclustering (xDigi) " << ((time[10].tv_sec - time[9].tv_sec)*1e6 + time[10].tv_usec) - time[9].tv_usec );
		}
		nEdgesTotal += numberOfEdgesOfCurrentBoard;
	}

	if (!nEdgesTotal)
		l1Info->setL1StrawEmptyPacket();

	//gettimeofday(&time[11], 0);
	//	LOG_INFO( "Access All Straw Data Packets & PreClustering - Stop " << time[11].tv_sec << " " << time[11].tv_usec );
	//////////////////////////////// End of loop for Pre-clustering ///////////////////////////////////////////////
	//	LOG_INFO( "Angela: " << decoder.getDecodedEvent()->getEventNumber() << "\t" << decoder.getDecodedEvent()->getTimestamp() << "\t" << nEdges_tot );

	//	if (nHits > 0) {
	//		for (int i = 0; i < 7; i++) {
	//			LOG_INFO( ((time[i+1].tv_usec + time[i+1].tv_sec*1e6)-(time[i].tv_usec + time[i].tv_sec*1e6))*0.7 << " ";
	//		}
	//		LOG_INFO(ENDL;
	//	}

	//LOG_INFO( " Preclustering " << ((time[11].tv_sec - time[9].tv_sec)*1e6 + time[11].tv_usec) - time[9].tv_usec );
	//LOG_INFO( " End Preclustering - initial time " << ((time[11].tv_sec - time[0].tv_sec)*1e6 + time[11].tv_usec) - time[0].tv_usec );

	//LOG_INFO("\n PRECLUSTER, n ="<<ntotalhit);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int h = 0; h < 2; h++) {
				nTotalPreclusters += nStrawPreclusters[i][j][h];
			}
		}
	}

	///////////////////////////////////////Start Clustering inside the view///////////////////////////////////////////////////////
	//LOG_INFO("\n CLUSTERING INSIDE THE VIEW \n");
	//gettimeofday(&time[12], 0);
	//	LOG_INFO( "Clustering inside the view - Start " << time[12].tv_sec << " " << time[12].tv_usec );

	float positionH = 0.0;
	float positionJ = 0.0;
	float meanDistance = 0.0;
	float deltaDistance = 0.0;
	float deltaDistanceTriplets = 0.0;
	float tempDistance = 0.0;
	double trailingCluster = 0.0;

	for (int i = 0; i < 4; i++) {
		for (int g = 0; g < 4; g++) {
			//triplette 1

			for (int j = 0; j < nStrawPreclusters[i][g][0]; j++) { //hit loop

				// cluster con 3 hit (2 della stessa mezza vista)
				if (((strawPrecluster_[i][g][0][j].leading < (M1LeadTrail_ * strawPrecluster_[i][g][0][j].trailing + Q1LeadTrail_)
						&& strawPrecluster_[i][g][0][j].leading > (M2LeadTrail_ * strawPrecluster_[i][g][0][j].trailing + Q2LeadTrail_)
						&& (strawPrecluster_[i][g][0][j].trailing > CutLowTrailing_
								&& strawPrecluster_[i][g][0][j].trailing < CutHighTrailing_))
						|| strawPrecluster_[i][g][0][j].trailing < -99999)
						&& (strawPrecluster_[i][g][0][j].leading > CutLowLeading_ && strawPrecluster_[i][g][0][j].leading < CutHighLeading_)) {
					for (int h = j + 1; h < nStrawPreclusters[i][g][0] && !strawPrecluster_[i][g][0][j].used; h++) {
						//prendo un hit appertenente al piano 2 o 3

						if (((strawPrecluster_[i][g][0][h].leading < (M1LeadTrail_ * strawPrecluster_[i][g][0][h].trailing + Q1LeadTrail_)
								&& strawPrecluster_[i][g][0][h].leading
										> (M2LeadTrail_ * strawPrecluster_[i][g][0][h].trailing + Q2LeadTrail_)
								&& (strawPrecluster_[i][g][0][h].trailing > CutLowTrailing_
										&& strawPrecluster_[i][g][0][h].trailing < CutHighTrailing_))
								|| strawPrecluster_[i][g][0][h].trailing < -99999)
								&& (strawPrecluster_[i][g][0][h].leading > CutLowLeading_
										&& strawPrecluster_[i][g][0][h].leading < CutHighLeading_)) {
							tempDistance = strawPrecluster_[i][g][0][j].position - strawPrecluster_[i][g][0][h].position;
							if (tempDistance < 9 && tempDistance > -9 && !strawPrecluster_[i][g][0][h].used) {
								if (tempDistance > 0) {
									positionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
									positionJ = strawPrecluster_[i][g][0][j].position - strawPrecluster_[i][g][0][j].wiredistance;
								} else {
									positionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
									positionJ = strawPrecluster_[i][g][0][j].position + strawPrecluster_[i][g][0][j].wiredistance;
								}

								meanDistance = (positionH + positionJ) / 2;

								deltaDistanceTriplets = strawPrecluster_[i][g][0][h].wiredistance
										+ strawPrecluster_[i][g][0][j].wiredistance;

								if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][0][j].trailing > -200)
									trailingCluster = (strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][0][j].trailing) / 2;
								else {
									if (strawPrecluster_[i][g][0][h].trailing <= -200)
										trailingCluster = strawPrecluster_[i][g][0][j].trailing;
									else
										trailingCluster = strawPrecluster_[i][g][0][h].trailing;
								}

								if (deltaDistanceTriplets > Hit3Low_ && deltaDistanceTriplets < Hit3High_) {
									strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
											strawPrecluster_[i][g][0][h].view, meanDistance, trailingCluster, deltaDistanceTriplets, 0);

									nStrawClusters[i][g]++;
									if (nStrawClusters[i][g] >= MAX_N_CLUSTER) {
										return StrawAlgo::abortProcessing(l1Info);
									}
									strawPrecluster_[i][g][0][h].used = 1;
									strawPrecluster_[i][g][0][j].used = 1;

									for (int l = 0; l < nStrawPreclusters[i][g][1]; l++) {
										//cerco lo hit 3 negli altri 2 piani

										tempDistance = fabs(meanDistance - strawPrecluster_[i][g][1][l].position);
										if (tempDistance < 5 && !strawPrecluster_[i][g][1][l].used) {
											strawPrecluster_[i][g][1][l].used = 1;
										}
									}
								}
							}
						}
					}
				}
			}
			//triplette 2, cambio mezza vista

			for (int j = 0; j < nStrawPreclusters[i][g][1]; j++) {
				//hit loop

				// cluster con 3 hit (due della stessa mezza vista)
				if (((strawPrecluster_[i][g][1][j].leading < (M1LeadTrail_ * strawPrecluster_[i][g][1][j].trailing + Q1LeadTrail_)
						&& strawPrecluster_[i][g][1][j].leading > (M2LeadTrail_ * strawPrecluster_[i][g][1][j].trailing + Q2LeadTrail_)
						&& (strawPrecluster_[i][g][1][j].trailing > CutLowTrailing_
								&& strawPrecluster_[i][g][1][j].trailing < CutHighTrailing_))
						|| strawPrecluster_[i][g][1][j].trailing < -99999)
						&& (strawPrecluster_[i][g][1][j].leading > CutLowLeading_ && strawPrecluster_[i][g][1][j].leading < CutHighLeading_)) {
					// End of conditional logic

					for (int h = j + 1; h < nStrawPreclusters[i][g][1] && !strawPrecluster_[i][g][1][j].used; h++) {
						//prendo un hit appertenente al piano 2 o 3

						if (((strawPrecluster_[i][g][1][h].leading < (M1LeadTrail_ * strawPrecluster_[i][g][1][h].trailing + Q1LeadTrail_)
								&& strawPrecluster_[i][g][1][h].leading
										> (M2LeadTrail_ * strawPrecluster_[i][g][1][h].trailing + Q2LeadTrail_)
								&& (strawPrecluster_[i][g][1][h].trailing > CutLowTrailing_
										&& strawPrecluster_[i][g][1][h].trailing < CutHighTrailing_))
								|| strawPrecluster_[i][g][1][h].trailing < -99999)
								&& (strawPrecluster_[i][g][1][h].leading > CutLowLeading_
										&& strawPrecluster_[i][g][1][h].leading < CutHighLeading_)) {
							// End of conditional logic

							tempDistance = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][h].position;

							if (tempDistance < 9 && tempDistance > -9 && !strawPrecluster_[i][g][1][h].used) {
								if (tempDistance > 0) {
									positionH = strawPrecluster_[i][g][1][h].position + strawPrecluster_[i][g][1][h].wiredistance;
									positionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
								} else {
									positionH = strawPrecluster_[i][g][1][h].position - strawPrecluster_[i][g][1][h].wiredistance;
									positionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
								}

								meanDistance = (positionH + positionJ) / 2;

								deltaDistanceTriplets = strawPrecluster_[i][g][1][h].wiredistance
										+ strawPrecluster_[i][g][1][j].wiredistance;

								if (strawPrecluster_[i][g][1][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200) {
									trailingCluster = (strawPrecluster_[i][g][1][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
								} else {
									if (strawPrecluster_[i][g][1][h].trailing <= -200) {
										trailingCluster = strawPrecluster_[i][g][1][j].trailing;
									} else {
										trailingCluster = strawPrecluster_[i][g][1][h].trailing;
									}
								}

								if (deltaDistanceTriplets > Hit3Low_ && deltaDistanceTriplets < Hit3High_) {
									strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][1][h].chamber,
											strawPrecluster_[i][g][1][h].view, meanDistance, trailingCluster, deltaDistanceTriplets, 0);

									nStrawClusters[i][g]++;
									if (nStrawClusters[i][g] >= MAX_N_CLUSTER) {
										return StrawAlgo::abortProcessing(l1Info);
									}
									strawPrecluster_[i][g][1][h].used = 1;
									strawPrecluster_[i][g][1][j].used = 1;

									for (int l = 0; l < nStrawPreclusters[i][g][0]; l++) {
										//cerco il 3 hit negli altri 2 piani

										tempDistance = fabs(meanDistance - strawPrecluster_[i][g][0][l].position);
										if (tempDistance < 5 && !strawPrecluster_[i][g][0][l].used) {
											strawPrecluster_[i][g][0][l].used = 1;
										}
									}
								}
							}
						}
					}

					//cluster con 2 hit
					for (int h = 0; h < nStrawPreclusters[i][g][0] && !strawPrecluster_[i][g][1][j].used; h++) {
						//prendo un hit appertenente al piano 2 o 3

						if (((strawPrecluster_[i][g][0][h].leading < (M1LeadTrail_ * strawPrecluster_[i][g][0][h].trailing + Q1LeadTrail_)
								&& strawPrecluster_[i][g][0][h].leading
										> (M2LeadTrail_ * strawPrecluster_[i][g][0][h].trailing + Q2LeadTrail_)
								&& (strawPrecluster_[i][g][0][h].trailing > CutLowTrailing_
										&& strawPrecluster_[i][g][0][h].trailing < CutHighTrailing_))
								|| strawPrecluster_[i][g][0][h].trailing < -99999)
								&& (strawPrecluster_[i][g][0][h].leading > CutLowLeading_
										&& strawPrecluster_[i][g][0][h].leading < CutHighLeading_)) {
							// End of conditional logic

							tempDistance = fabs(strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][0][h].position);
							if (tempDistance < 9 && !strawPrecluster_[i][g][0][h].used) {
								switch (strawPrecluster_[i][g][1][j].plane) {
								case (0):
									if (!strawPrecluster_[i][g][0][h].plane) {
										if (!g || g == 2) {
											positionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											positionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										} else {
											positionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											positionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										}

										meanDistance = (positionH + positionJ) / 2;

										deltaDistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200) {
											trailingCluster =
													(strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
										} else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200) {
												trailingCluster = strawPrecluster_[i][g][1][j].trailing;
											} else {
												trailingCluster = strawPrecluster_[i][g][0][h].trailing;
											}
										}

										if (deltaDistance > Hit2Low_ && deltaDistance < Hit2High_) {
											strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, meanDistance, trailingCluster, deltaDistance, 0);
											nStrawClusters[i][g]++;
											if (nStrawClusters[i][g] >= MAX_N_CLUSTER) {
												return StrawAlgo::abortProcessing(l1Info);
											}
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									} else {
										if (!g || g == 2) {
											positionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											positionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										} else {
											positionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											positionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										}

										meanDistance = (positionH + positionJ) / 2;

										deltaDistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											trailingCluster =
													(strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200) {
												trailingCluster = strawPrecluster_[i][g][1][j].trailing;
											} else {
												trailingCluster = strawPrecluster_[i][g][0][h].trailing;
											}
										}

										if (deltaDistance > Hit2Low_ && deltaDistance < Hit2High_) {
											strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, meanDistance, trailingCluster, deltaDistance, 0);
											nStrawClusters[i][g]++;
											if (nStrawClusters[i][g] >= MAX_N_CLUSTER) {
												return StrawAlgo::abortProcessing(l1Info);
											}
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									}

									break;
								case (1):
									if (!strawPrecluster_[i][g][0][h].plane) {
										if (!g || g == 2) {
											positionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											positionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										} else {
											positionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											positionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										}

										meanDistance = (positionH + positionJ) / 2;

										deltaDistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											trailingCluster =
													(strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200)
												trailingCluster = strawPrecluster_[i][g][1][j].trailing;
											else
												trailingCluster = strawPrecluster_[i][g][0][h].trailing;
										}

										if (deltaDistance > Hit2Low_ && deltaDistance < Hit2High_) {
											strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, meanDistance, trailingCluster, deltaDistance, 0);
											nStrawClusters[i][g]++;
											if (nStrawClusters[i][g] >= MAX_N_CLUSTER) {
												return StrawAlgo::abortProcessing(l1Info);
											}
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									} else {
										if (!g || g == 2) {
											positionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											positionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										} else {
											positionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											positionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										}

										meanDistance = (positionH + positionJ) / 2;

										deltaDistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											trailingCluster =
													(strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200) {
												trailingCluster = strawPrecluster_[i][g][1][j].trailing;
											} else {
												trailingCluster = strawPrecluster_[i][g][0][h].trailing;
											}
										}

										if (deltaDistance > Hit2Low_ && deltaDistance < Hit2High_) {
											strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, meanDistance, trailingCluster, deltaDistance, 0);
											nStrawClusters[i][g]++;
											if (nStrawClusters[i][g] >= MAX_N_CLUSTER) {
												return StrawAlgo::abortProcessing(l1Info);
											}
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									}
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	//gettimeofday(&time[13], 0);
	//	LOG_INFO( "Clustering inside the view - Stop " << time[13].tv_sec << " " << time[13].tv_usec );
	//	LOG_INFO( "Clustering inside the view " << ((time[13].tv_sec - time[12].tv_sec)*1e6 + time[13].tv_usec) - time[12].tv_usec );
	//	LOG_INFO( "Clustering inside the view fino all'inizio" << ((time[13].tv_sec - time[0].tv_sec)*1e6 + time[13].tv_usec) - time[0].tv_usec );

	/////////////////////////////////////// Start Clustering inside the chamber ///////////////////////////////////////////////////////
	/////////////////////////////////////// 0=v, 1=u, 2=x, 3=y
	//LOG_INFO("CLUSTERING INSIDE THE CHAMBER");

	//gettimeofday(&time[14], 0);
	//	LOG_INFO( "Clustering inside the chamber - Start " << time[14].tv_sec << " " << time[14].tv_usec );

	float coordinateTemp = 0.0;
	float viewDistance = 0.0;
	float viewDistance4 = 0.0;
	float xTemp = 0.0;
	float yTemp = 0.0;

	//tracks reco
	float qy = 0.0;
	double coordinate[2];

	for (int i = 0; i < 4; i++) {
		////////////ciclo dei punti a 4 viste//////////////////
		//gettimeofday(&time[15], 0);
		nTotalViewCluster += nStrawClusters[i][0] + nStrawClusters[i][1] + nStrawClusters[i][2] + nStrawClusters[i][3];
		for (int a = 0; a < nStrawClusters[i][3]; a++) { //clusters [a] is inside y

			for (int b = 0; b < nStrawClusters[i][2]; b++) { //we loop on x view

				for (int c = 0; c < nStrawClusters[i][1]; c++) { //we loop on u  views
					coordinateTemp = (strawCluster_[i][3][a].coordinate + strawCluster_[i][2][b].coordinate) / Sq2_;
					viewDistance = fabs(strawCluster_[i][1][c].coordinate - coordinateTemp);

					if (viewDistance < CutCluster_) {
						for (int d = 0; d < nStrawClusters[i][0]; d++) { //v views
							coordinateTemp = (strawCluster_[i][2][b].coordinate - strawCluster_[i][3][a].coordinate) / Sq2_;
							viewDistance4 = fabs(strawCluster_[i][0][d].coordinate - coordinateTemp);

							if (viewDistance4 < CutCluster_) {
								xTemp = strawCluster_[i][2][b].coordinate;
								yTemp = strawCluster_[i][3][a].coordinate;

								// Decide which of the two we want to actually save
								viewDistance = viewDistance < viewDistance4 ? viewDistance : viewDistance4;

								strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], xTemp, yTemp,
										(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing + strawCluster_[i][1][c].trailing
												+ strawCluster_[i][0][d].trailing) / 4, viewDistance, 4, 0);

								strawCluster_[i][0][d].used = 1;
								strawCluster_[i][1][c].used = 1;
								strawCluster_[i][2][b].used = 1;
								strawCluster_[i][3][a].used = 1;

								nStrawPointsTemp[i]++;
								if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
									return StrawAlgo::abortProcessing(l1Info);
								}
							}
						} // End of d/v loop
					} // End of cluster cut conditional
				} // End of c/u loop
			} // End of b/x loop
		} // End of a loop

		//gettimeofday(&time[16], 0);
		//////////////////////////// primo ciclo dei punti a 3 viste prima yx cercando in v o u, poi uv cercando in x o y
		//gettimeofday(&time[17], 0);

		for (int a = 0; a < nStrawClusters[i][3]; a++) { //clusters [a] is inside y
			for (int b = 0; b < nStrawClusters[i][2]; b++) { //we loop on x view
				for (int c = 0; c < nStrawClusters[i][1]; c++) { //we loop on u  views
					if ((strawCluster_[i][1][c].used != 1 && strawCluster_[i][2][b].used != 1)
							|| (strawCluster_[i][1][c].used != 1 && strawCluster_[i][3][a].used != 1)
							|| (strawCluster_[i][2][b].used != 1 && strawCluster_[i][3][a].used != 1)) {
						// End of conditional logic

						coordinateTemp = (strawCluster_[i][3][a].coordinate + strawCluster_[i][2][b].coordinate) / Sq2_;
						viewDistance = fabs(strawCluster_[i][1][c].coordinate - coordinateTemp);

						if (viewDistance < CutCluster_) {
							xTemp = strawCluster_[i][2][b].coordinate;
							yTemp = strawCluster_[i][3][a].coordinate;

							strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], xTemp, yTemp,
									(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing + strawCluster_[i][1][c].trailing)
											/ 3, viewDistance, 3, 0);

							strawCluster_[i][1][c].used = 2;
							strawCluster_[i][2][b].used = 2;
							strawCluster_[i][3][a].used = 2;

							nStrawPointsTemp[i]++;
							if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
								return StrawAlgo::abortProcessing(l1Info);
							}
						}
					}
				} // End of c/u loop
				for (int c = 0; c < nStrawClusters[i][0]; c++) { //we loop on v views
					if ((strawCluster_[i][0][c].used != 1 && strawCluster_[i][2][b].used != 1)
							|| (strawCluster_[i][0][c].used != 1 && strawCluster_[i][3][a].used != 1)
							|| (strawCluster_[i][2][b].used != 1 && strawCluster_[i][3][a].used != 1)) {
						// End of conditional logic

						coordinateTemp = (strawCluster_[i][2][b].coordinate - strawCluster_[i][3][a].coordinate) / Sq2_;
						viewDistance = fabs(strawCluster_[i][0][c].coordinate - coordinateTemp);

						if (viewDistance < CutCluster_) {
							xTemp = strawCluster_[i][2][b].coordinate;
							yTemp = strawCluster_[i][3][a].coordinate;

							strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], xTemp, yTemp,
									(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing + strawCluster_[i][0][c].trailing)
											/ 3, viewDistance, 3, 0);

							strawCluster_[i][0][c].used = 2;
							strawCluster_[i][2][b].used = 2;
							strawCluster_[i][3][a].used = 2;

							nStrawPointsTemp[i]++;
							if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
								return StrawAlgo::abortProcessing(l1Info);
							}
						}
					}
				} // End of c/v loop
			} // End of b/x loop
		} // End of a loop

		//gettimeofday(&time[18], 0);
		/////////////////////////3 viste seconda clusterizzazione partendo da u e v

		//gettimeofday(&time[19], 0);
		//		LOG_INFO( nStrawClusters[i][0] );
		for (int a = 0; a < nStrawClusters[i][0]; a++) { //v
			// if(strawCluster_[i][0][a].used != 1)
			// {
			//			LOG_INFO( nStrawClusters[i][1] );
			// }
			for (int b = 0; b < nStrawClusters[i][1]; b++) { //u
				// if(strawCluster_[i][1][b].used != 1)
				// {
				// 			LOG_INFO( nStrawClusters[i][2] );
				// }
				for (int c = 0; c < nStrawClusters[i][2]; c++) { //x
					if ((strawCluster_[i][0][a].used != 1 && strawCluster_[i][1][b].used != 1)
							|| (strawCluster_[i][0][a].used != 1 && strawCluster_[i][2][c].used != 1)
							|| (strawCluster_[i][1][b].used != 1 && strawCluster_[i][2][c].used != 1)) {
						// End of conditional logic

						coordinateTemp = (strawCluster_[i][0][a].coordinate + strawCluster_[i][1][b].coordinate) / Sq2_;
						viewDistance = fabs(strawCluster_[i][2][c].coordinate - coordinateTemp);

						if (viewDistance < CutCluster_) {
							xTemp = strawCluster_[i][2][c].coordinate;
							yTemp = (strawCluster_[i][1][b].coordinate - strawCluster_[i][0][a].coordinate) / Sq2_;

							strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], xTemp, yTemp,
									(strawCluster_[i][0][a].trailing + strawCluster_[i][1][b].trailing + strawCluster_[i][2][c].trailing)
											/ 3, viewDistance, 3, 0);

							strawCluster_[i][0][a].used = 2;
							strawCluster_[i][1][b].used = 2;
							strawCluster_[i][2][c].used = 2;

							nStrawPointsTemp[i]++;
							if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
								return StrawAlgo::abortProcessing(l1Info);
							}
						}
					}
				}
				//				LOG_INFO( nStrawClusters[i][3] );
				for (int c = 0; c < nStrawClusters[i][3]; c++) { //y
					if ((strawCluster_[i][0][a].used != 1 && strawCluster_[i][1][b].used != 1)
							|| (strawCluster_[i][0][a].used != 1 && strawCluster_[i][3][c].used != 1)
							|| (strawCluster_[i][1][b].used != 1 && strawCluster_[i][3][c].used != 1)) {
						// End of condititional logic

						coordinateTemp = (strawCluster_[i][1][b].coordinate - strawCluster_[i][0][a].coordinate) / Sq2_;
						viewDistance = fabs(strawCluster_[i][3][c].coordinate - coordinateTemp);

						if (viewDistance < CutCluster_) {
							xTemp = (strawCluster_[i][0][a].coordinate + strawCluster_[i][1][b].coordinate) / Sq2_;
							yTemp = strawCluster_[i][3][c].coordinate;

							strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], xTemp, yTemp,
									(strawCluster_[i][0][a].trailing + strawCluster_[i][1][b].trailing + strawCluster_[i][3][c].trailing)
											/ 3, viewDistance, 3, 0);

							strawCluster_[i][0][a].used = 2;
							strawCluster_[i][1][b].used = 2;
							strawCluster_[i][3][c].used = 2;

							nStrawPointsTemp[i]++;
							if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
								return StrawAlgo::abortProcessing(l1Info);
							}
						}
					}
				}
			}
		}
		//gettimeofday(&time[20], 0);
		//clusterizzazione nelle camere con sole 2 viste
		//gettimeofday(&time[21], 0);
		for (int a = 0; a < nStrawClusters[i][0]; a++) { //v

			for (int b = 0; b < nStrawClusters[i][1]; b++) { //u (v,u)

				if (strawCluster_[i][1][b].used == 0 && strawCluster_[i][0][a].used == 0) {
					coordinate[0] = (strawCluster_[i][0][a].coordinate + strawCluster_[i][1][b].coordinate) / Sq2_;
					coordinate[1] = (strawCluster_[i][1][b].coordinate - strawCluster_[i][0][a].coordinate) / Sq2_;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], coordinate[0], coordinate[1],
								(strawCluster_[i][0][a].trailing + strawCluster_[i][1][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;
						if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
							return StrawAlgo::abortProcessing(l1Info);
						}
					}
				}
			}
			for (int b = 0; b < nStrawClusters[i][2]; b++) { //x (v,x)

				if (strawCluster_[i][2][b].used == 0 && strawCluster_[i][0][a].used == 0) {
					coordinate[0] = strawCluster_[i][2][b].coordinate;
					coordinate[1] = strawCluster_[i][2][b].coordinate - (Sq2_ * strawCluster_[i][0][a].coordinate);

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], coordinate[0], coordinate[1],
								(strawCluster_[i][0][a].trailing + strawCluster_[i][2][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;
						if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
							return StrawAlgo::abortProcessing(l1Info);
						}
					}
				}
			}
			for (int b = 0; b < nStrawClusters[i][3]; b++) { //y (v,y)

				if (strawCluster_[i][3][b].used == 0 && strawCluster_[i][0][a].used == 0) {
					coordinate[0] = (Sq2_ * strawCluster_[i][0][a].coordinate) + strawCluster_[i][3][b].coordinate;
					coordinate[1] = strawCluster_[i][3][b].coordinate;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], coordinate[0], coordinate[1],
								(strawCluster_[i][0][a].trailing + strawCluster_[i][3][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;
						if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
							return StrawAlgo::abortProcessing(l1Info);
						}
					}
				}
			}
		}
		//gettimeofday(&time[22], 0);
		//gettimeofday(&time[23], 0);
		for (int a = 0; a < nStrawClusters[i][1]; a++) { //u

			for (int b = 0; b < nStrawClusters[i][2]; b++) { //x (u,x)

				if (strawCluster_[i][2][b].used == 0 && strawCluster_[i][1][a].used == 0) {
					coordinate[0] = strawCluster_[i][2][b].coordinate;
					coordinate[1] = (Sq2_ * strawCluster_[i][1][a].coordinate) - strawCluster_[i][2][b].coordinate;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], coordinate[0], coordinate[1],
								(strawCluster_[i][1][a].trailing + strawCluster_[i][2][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;
						if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
							return StrawAlgo::abortProcessing(l1Info);
						}
					}
				}
			}
			for (int b = 0; b < nStrawClusters[i][3]; b++) { //y (u,y)

				if (strawCluster_[i][3][b].used == 0 && strawCluster_[i][1][a].used == 0) {
					coordinate[0] = (Sq2_ * strawCluster_[i][1][a].coordinate) - strawCluster_[i][3][b].coordinate;
					coordinate[1] = strawCluster_[i][3][b].coordinate;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], coordinate[0], coordinate[1],
								(strawCluster_[i][1][a].trailing + strawCluster_[i][3][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;
						if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
							return StrawAlgo::abortProcessing(l1Info);
						}
					}
				}
			}
		}
		//gettimeofday(&time[24], 0);
		//gettimeofday(&time[25], 0);
		for (int a = 0; a < nStrawClusters[i][2]; a++) { //x

			for (int b = 0; b < nStrawClusters[i][3]; b++) { //y (x,y)

				if (strawCluster_[i][3][b].used == 0 && strawCluster_[i][2][a].used == 0) {
					coordinate[0] = strawCluster_[i][2][a].coordinate;
					coordinate[1] = strawCluster_[i][3][b].coordinate;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], coordinate[0], coordinate[1],
								(strawCluster_[i][2][a].trailing + strawCluster_[i][3][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;
						if (nStrawPointsTemp[i] >= MAX_N_POINT_TEMP) {
							return StrawAlgo::abortProcessing(l1Info);
						}
					}
				}
			}
		}
		//gettimeofday(&time[26], 0);
	}
	//gettimeofday(&time[27], 0);
	//	LOG_INFO( "Clustering inside the chamber - Stop " << time[15].tv_sec << " " << time[15].tv_usec );
	//	LOG_INFO( "Clustering inside the chamber " << ((time[27].tv_sec - time[14].tv_sec)*1e6 + time[27].tv_usec) - time[14].tv_usec );
	//	LOG_INFO( "Clustering inside the chamber - initial " << ((time[27].tv_sec - time[0].tv_sec)*1e6 + time[27].tv_usec) - time[0].tv_usec );
	//	if (nEdges_tot && nEdges_tot < 1500) LOG_INFO(((time[1].tv_sec - time[0].tv_sec)*1e6 + time[1].tv_usec) - time[0].tv_usec << "\t" << ((time[4].tv_sec - time[3].tv_sec)*1e6 + time[4].tv_usec) - time[3].tv_usec << "\t" << ((time[6].tv_sec - time[5].tv_sec)*1e6 + time[6].tv_usec) - time[5].tv_usec << "\t" << ((time[8].tv_sec - time[7].tv_sec)*1e6 + time[8].tv_usec) - time[7].tv_usec << "\t" << ((time[10].tv_sec - time[9].tv_sec)*1e6 + time[10].tv_usec) - time[9].tv_usec << "\t" << ((time[11].tv_sec - time[2].tv_sec)*1e6 + time[11].tv_usec) - time[2].tv_usec << "\t" << ((time[13].tv_sec - time[12].tv_sec)*1e6 + time[13].tv_usec) - time[12].tv_usec << "\t" << nEdges_tot << "\t" << ((time[27].tv_sec - time[14].tv_sec)*1e6 + time[27].tv_usec) - time[14].tv_usec << "\t" << ((time[27].tv_sec - time[0].tv_sec)*1e6 + time[27].tv_usec) - time[0].tv_usec << "\t" << ((time[16].tv_sec - time[15].tv_sec)*1e6 + time[16].tv_usec) - time[15].tv_usec << "\t" << ((time[18].tv_sec - time[17].tv_sec)*1e6 + time[18].tv_usec) - time[17].tv_usec << "\t" << ((time[20].tv_sec - time[19].tv_sec)*1e6 + time[20].tv_usec) - time[19].tv_usec << "\t" << ((time[22].tv_sec - time[21].tv_sec)*1e6 + time[22].tv_usec) - time[21].tv_usec << "\t" << ((time[24].tv_sec - time[23].tv_sec)*1e6 + time[24].tv_usec) - time[23].tv_usec << "\t" << ((time[26].tv_sec - time[25].tv_sec)*1e6 + time[26].tv_usec) - time[25].tv_usec);

	////////////////////////////////////////POINT SELECTION/////////////////////////////////////////////////////////
	//	LOG_INFO( "POINT SELECTION...!!! " );
	float point_dx = 0.0;
	float point_dy = 0.0;
	Point pointTemp;
	float viewDistanceDelta = 0.0;

	for (int i = 0; i < 4; i++) {
		if (nStrawPointsTemp[i] < 7) {
			//			LOG_INFO( "!!!!!!!!!!!!!! Sto entrando qui !!!!!! " );
			nStrawPointsFinal[i] = nStrawPointsTemp[i];
			for (int j = 0; j < nStrawPointsFinal[i]; j++) { //loop over point
				strawPointFinal_[i][j] = strawPointTemp_[i][j];
			}
			continue;
		}
		for (int j = 0; j < nStrawPointsTemp[i]; j++) { //loop over point

			// First loop over points with 4 views since they are more useful
			if (strawPointTemp_[i][j].nViews == 4 && !strawPointTemp_[i][j].used) {
				pointTemp.clonePoint(strawPointTemp_[i][j]);
				strawPointTemp_[i][j].used = 1;
				for (int k = j + 1; k < nStrawPointsTemp[i]; k++) { //loop over point

					if (strawPointTemp_[i][k].nViews == 4 && !strawPointTemp_[i][k].used) {
						//						point_dx = fabs(strawPointTemp_[i][j].x-strawPointTemp_[i][k].x);
						point_dy = fabs(strawPointTemp_[i][j].y - strawPointTemp_[i][k].y);
						if (point_dy < 4.0) {
							viewDistanceDelta = pointTemp.viewDistance - strawPointTemp_[i][k].viewDistance;
							if (viewDistanceDelta > 1.0 && pointTemp.viewDistance > 3.0) {
								pointTemp.clonePoint(strawPointTemp_[i][k]);
								strawPointTemp_[i][k].used = 1;
							}
							if (viewDistanceDelta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTemp_[i][k].used = 1;
							}
						}
					}
				}
				strawPointTempBis_[i][nStrawPointsTempBis[i]].clonePoint(pointTemp);
				nStrawPointsTempBis[i]++;
				if (nStrawPointsTempBis[i] >= MAX_N_POINT_TEMP_BIS) {
					return StrawAlgo::abortProcessing(l1Info);
				}
			}

			// Next loop over points with 3 views
			if (strawPointTemp_[i][j].nViews == 3 && !strawPointTemp_[i][j].used) {
				pointTemp.clonePoint(strawPointTemp_[i][j]);
				strawPointTemp_[i][j].used = 1;
				for (int k = j + 1; k < nStrawPointsTemp[i]; k++) { //loop over point
					if (strawPointTemp_[i][k].nViews == 3 && !strawPointTemp_[i][k].used) {
						//						point_dx = fabs(strawPointTemp_[i][j].x-strawPointTemp_[i][k].x);
						point_dy = fabs(strawPointTemp_[i][j].y - strawPointTemp_[i][k].y);
						if (point_dy < 4.0) {
							viewDistanceDelta = pointTemp.viewDistance - strawPointTemp_[i][k].viewDistance;
							if (viewDistanceDelta > 1.0 && pointTemp.viewDistance > 3.0) {
								pointTemp.clonePoint(strawPointTemp_[i][k]);
								strawPointTemp_[i][k].used = 1;
							}
							if (viewDistanceDelta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTemp_[i][k].used = 1;
							}
						}
					}
				}
				strawPointTempBis_[i][nStrawPointsTempBis[i]].clonePoint(pointTemp);
				nStrawPointsTempBis[i]++;
				if (nStrawPointsTempBis[i] >= MAX_N_POINT_TEMP_BIS) {
					return StrawAlgo::abortProcessing(l1Info);
				}
			}
			if (strawPointTemp_[i][j].nViews == 2) {
				strawPointTempBis_[i][nStrawPointsTempBis[i]].clonePoint(strawPointTemp_[i][j]);
				nStrawPointsTempBis[i]++;
				if (nStrawPointsTempBis[i] >= MAX_N_POINT_TEMP_BIS) {
					return StrawAlgo::abortProcessing(l1Info);
				}
			}
		}

		////////second SELECTION

		for (int j = 0; j < nStrawPointsTempBis[i]; j++) { //loop over point

			if (strawPointTempBis_[i][j].nViews == 4 && !strawPointTempBis_[i][j].used) {
				pointTemp.clonePoint(strawPointTempBis_[i][j]);
				strawPointTempBis_[i][j].used = 1;
				for (int k = j + 1; k < nStrawPointsTempBis[i]; k++) { //loop over point

					if (strawPointTempBis_[i][k].nViews == 4 && !strawPointTempBis_[i][k].used) {
						point_dx = fabs(strawPointTempBis_[i][j].x - strawPointTempBis_[i][k].x);
						//						point_dy = fabs(strawPointTempbis_[i][j].y-strawPointTempbis_[i][k].y);
						if (point_dx < 4.0) {
							viewDistanceDelta = pointTemp.viewDistance - strawPointTempBis_[i][k].viewDistance;
							if (viewDistanceDelta > 1.0 && pointTemp.viewDistance > 3.0) {
								//								pointTemp.setPoint(strawPointTempbis_[i][k].z,strawPointTempbis_[i][k].x,strawPointTempbis_[i][k].y,strawPointTempbis_[i][k].trailing,strawPointTempbis_[i][k].viewDistance,strawPointTempbis_[i][k].nViews,strawPointTempbis_[i][k].used);
								pointTemp.clonePoint(strawPointTempBis_[i][k]);
								strawPointTempBis_[i][k].used = 1;
							}
							if (viewDistanceDelta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTempBis_[i][k].used = 1;
							}
						}
					}
				}
				strawPointFinal_[i][nStrawPointsFinal[i]].clonePoint(pointTemp);
				nStrawPointsFinal[i]++;
				if (nStrawPointsFinal[i] >= MAX_N_POINT_FINAL) {
					return StrawAlgo::abortProcessing(l1Info);
				}
			}
			if (strawPointTempBis_[i][j].nViews == 3 && !strawPointTempBis_[i][j].used) {
				//				pointTemp.setPoint(strawPointTempbis_[i][j].z,strawPointTempbis_[i][j].x,strawPointTempbis_[i][j].y,strawPointTempbis_[i][j].trailing,strawPointTempbis_[i][j].viewDistance,strawPointTempbis_[i][j].nViews,strawPointTempbis_[i][j].used);
				pointTemp.clonePoint(strawPointTempBis_[i][j]);
				strawPointTempBis_[i][j].used = 1;
				for (int k = j + 1; k < nStrawPointsTempBis[i]; k++) { //loop over point
					if (strawPointTempBis_[i][k].nViews == 3 && !strawPointTempBis_[i][k].used) {
						point_dx = fabs(strawPointTempBis_[i][j].x - strawPointTempBis_[i][k].x);
						//						point_dy = fabs(strawPointTempbis_[i][j].y-strawPointTempbis_[i][k].y);
						if (point_dx < 4.0) {
							viewDistanceDelta = pointTemp.viewDistance - strawPointTempBis_[i][k].viewDistance;
							if (viewDistanceDelta > 1.0 && pointTemp.viewDistance > 3.0) {
								//								pointTemp.setPoint(strawPointTempbis_[i][k].z,strawPointTempbis_[i][k].x,strawPointTempbis_[i][k].y,strawPointTempbis_[i][k].trailing,strawPointTempbis_[i][k].viewDistance,strawPointTempbis_[i][k].nViews,strawPointTempbis_[i][k].used);
								pointTemp.clonePoint(strawPointTempBis_[i][k]);
								strawPointTempBis_[i][k].used = 1;
							}
							if (viewDistanceDelta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTempBis_[i][k].used = 1;
							}
						}
					}
				}
				strawPointFinal_[i][nStrawPointsFinal[i]].clonePoint(pointTemp);
				nStrawPointsFinal[i]++;
				if (nStrawPointsFinal[i] >= MAX_N_POINT_FINAL) {
					return StrawAlgo::abortProcessing(l1Info);
				}
			}
			if (strawPointTempBis_[i][j].nViews == 2) {
				strawPointFinal_[i][nStrawPointsFinal[i]].clonePoint(strawPointTempBis_[i][j]);
				nStrawPointsFinal[i]++;
				if (nStrawPointsFinal[i] >= MAX_N_POINT_FINAL) {
					return StrawAlgo::abortProcessing(l1Info);
				}
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < nStrawPointsFinal[i]; j++) {
			if (nStrawPointsFinal[i] > 0)
				nChambersHit++;
		}
	}

	////////////////////// TRACK RECONSTRUCTION ////////////////////////////////////
//	LOG_INFO("Track Reconstruction - Hought ");

	float cda = 0.;

	float mTemp;

	int nCam = 0;
	int qyHist = 0;
	int chkCamera = 0;
	int chkCam = 0;
	int nChkCam = 0;
	int addCam1 = 0;
	int addCam2 = 0;

	// Check
	int nFirstTrack = 0;
	int nFirstTrackCentrali = 0;
	int tempNHitC = 0;
	int tempNHitL = 0;
	int temp2NHitC = 0;

	int nAddHit1 = 0;
	int nAddHit2 = 0;
	int tempHit = 0;
	int tempCamera = 0;
	int chkHit = 0; //chk per verificare se un hit o una camera trovata in una casella laterale è gia stata usata nella casella centrale

	float pointMY = 0.0;
	float pointQY = 0.0;

	int nTrack = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < nStrawPointsFinal[i]; j++) //loop over point
				{
			nCam = i;
			for (int a = 0; a < RANGEM; a++) {
				mTemp = ((float) a - ((float) RANGEM + 0.1) / 2) * PASSO;
				qy = strawPointFinal_[i][j].y - mTemp * (strawPointFinal_[i][j].z - ZMAGNET); //con 0.03 mrad di angolo max qy va da -1424 a 1637, quindi metto 1500 come limite (posso tagliare un poco prima di 0.03 per l'ultima camera)
				//qx = point[i][j].x - mtemp * point[i][j].z; //se si volesse fare un hough con x ma e' complicato con il magnete

				qyHist = qy * RANGEQ / LMAGNET + RANGEQ / 2; //provo a mettere 3000

				if (qyHist > 0 && qyHist < RANGEQ && (hought[a][qyHist] >> 60) < 6) {
					hought[a][qyHist] |= ((long long) (0x3 & nCam) << ((hought[a][qyHist] >> 60) * 2 + 48)); //12 bits from 48 to 59 with the point chamber number (2 bit for chamber)
					hought[a][qyHist] |= ((((long long) (0xFF & (j))) << ((hought[a][qyHist] >> 60) * 8)) & 0XFFFFFFFFFFFF); //the first 48 bits with up to 6 point of 8 bits ( 255 )
					hought[a][qyHist] += ((long long) 1 << 60); //the 4 most significant bits with the number of points
				}
			}
		}
	}

	nFirstTrack = 0;

	for (int a = 0; a < RANGEM; a++) {
		for (int b = 0; b < RANGEQ; b++) {
			if ((hought[a][b] >> 60) > 1) //looking for bin with at least 2 points
					{
				chkCam = 0;
				nChkCam = 4;
				addCam1 = -1;
				addCam2 = -1;
				chkCamera = 0;

				//verificare
				nFirstTrack = 0;
				nFirstTrackCentrali = 0;
				tempNHitC = 0;
				tempNHitL = 0;
				nAddHit1 = 0;

				nAddHit1 = 0;
				nAddHit2 = 0;
				tempHit = 0;
				tempCamera = 0;
				chkHit = 0; //chk per verificare se un hit o una camera trovata in una casella laterale è gia stata usata nella casella centrale

				pointMY = 0.0;
				pointQY = 0.0;

				for (int c = 0; c < (int) (hought[a][b] >> 60); c++) { // check if there are at least 2 hits belonging to different chambers
					chkCam |= 1 << (0X3 & (hought[a][b] >> (48 + 2 * c)));
				}
				for (int d = 0; d < 4; d++) {
					if ((0X1 & (chkCam >> d)) == 0) {
						nChkCam--;

						if (addCam1 == -1)
							addCam1 = d;
						else
							addCam2 = d;
					}
				}
				if (nChkCam > 1) {
					for (int d = 0; d < (int) (hought[a][b] >> 60); d++) {
						// si creano più tracklet con tutte le combinazioni di hit con camere diverse

						if (((int) pow(2, (int) (0X3 & (hought[a][b] >> (48 + 2 * d)))) & chkCamera) == 0) {
							// verifica se la camera è gia stata usata per questo tracklet: se non è stata
							// usata la aggiungo ad in tracklet esistenti (all'inizio ce ne e' uno vuoto)

							for (int j = 0; j < nFirstTrack + 1; j++) {
								strawFirstTempTrk_[j].hitc[tempNHitC] = (int) (0XFF & (hought[a][b] >> (8 * d)));
								strawFirstTempTrk_[j].camerec[tempNHitC] = (int) (0X3 & (hought[a][b] >> (48 + 2 * d)));

								chkCamera |= (int) pow(2, strawFirstTempTrk_[nFirstTrack].camerec[tempNHitC]);

								strawFirstTempTrk_[j].ncentrali = tempNHitC + 1;
								strawFirstTempTrk_[j].nlaterali = 0;
							}
							tempNHitC++;
							if (tempNHitC >= MAX_N_HIT_C) {
								return StrawAlgo::abortProcessing(l1Info);
							}
						} else //se è gia stata usata si crea un nuovo tracklet con la nuova camera al posto di quell'altra (se soddisfa dei requisiti)
						{
							temp2NHitC = 0;
							nFirstTrack++;
							if (nFirstTrack >= MAX_N_FIRST_TEMP_TRACK) {
								return StrawAlgo::abortProcessing(l1Info);
							}
							for (int j = 0; j < tempNHitC; j++) {

								if (strawFirstTempTrk_[nFirstTrack - 1].camerec[j] != (int) (0X3 & (hought[a][b] >> (48 + 2 * d)))) {
									strawFirstTempTrk_[nFirstTrack].hitc[temp2NHitC] = strawFirstTempTrk_[nFirstTrack - 1].hitc[j];
									strawFirstTempTrk_[nFirstTrack].camerec[temp2NHitC] = strawFirstTempTrk_[nFirstTrack - 1].camerec[j];
								} else {
									if (strawPointFinal_[(int) (0X3 & (hought[a][b] >> (48 + 2 * d)))][(int) (0XFF
											& (hought[a][b] >> (8 * d)))].nViews
											> strawPointFinal_[strawFirstTempTrk_[nFirstTrack - 1].camerec[temp2NHitC]][strawFirstTempTrk_[nFirstTrack
													- 1].hitc[temp2NHitC]].nViews) {
										nFirstTrack--;
										strawFirstTempTrk_[nFirstTrack].hitc[temp2NHitC] = (int) (0XFF & (hought[a][b] >> (8 * d)));
										strawFirstTempTrk_[nFirstTrack].camerec[temp2NHitC] = (int) (0X3 & (hought[a][b] >> (48 + 2 * d)));
									} else if (strawPointFinal_[(int) (0X3 & (hought[a][b] >> (48 + 2 * d)))][(int) (0XFF
											& (hought[a][b] >> (8 * d)))].nViews
											== strawPointFinal_[strawFirstTempTrk_[nFirstTrack - 1].camerec[temp2NHitC]][strawFirstTempTrk_[nFirstTrack
													- 1].hitc[temp2NHitC]].nViews) {
										strawFirstTempTrk_[nFirstTrack].hitc[temp2NHitC] = (int) (0XFF & (hought[a][b] >> (8 * d)));
										strawFirstTempTrk_[nFirstTrack].camerec[temp2NHitC] = (int) (0X3 & (hought[a][b] >> (48 + 2 * d)));
									} else {
										nFirstTrack--;
									}
								}

								temp2NHitC++;
								if (temp2NHitC >= MAX_N_HIT_C) {
									return StrawAlgo::abortProcessing(l1Info);
								}
							}
							strawFirstTempTrk_[nFirstTrack].ncentrali = temp2NHitC;
							strawFirstTempTrk_[nFirstTrack].nlaterali = 0;
						}
					}
					nFirstTrackCentrali = nFirstTrack;

					if (tempNHitC > 1 && tempNHitC < 4) //se ci sono meno di 4 hit nella casella centrale, si cercano gli hit mancanti in quelle intorno
							{
						nAddHit1 = 0;
						nAddHit2 = 0;
						int *addhit1 = new int[50];
						int *addhit2 = new int[50];

						for (int h = -1; h < 2; h++) {
							for (int k = -1; k < 2; k++) {
								if (a + h > -1 && a + h < RANGEM && b + k > -1 && b + k < 200) {
									for (int l = 0; l < (hought[a + h][b + k] >> 60); l++) {
										tempHit = (int) (0XFF & (hought[a + h][b + k] >> (8 * l)));
										tempCamera = (int) (0X3 & (hought[a + h][b + k] >> (48 + 2 * l)));
										chkHit = 0;

										//terza e nuova soluzione

										if (tempCamera == addCam1) {
											//look if there already are the hit in addhit1[]

											for (int d = 0; d < nAddHit1; d++) {
												if (addhit1[d] == tempHit) {
													chkHit = 1;
												}
											}

											if (chkHit == 0) {
												addhit1[nAddHit1] = tempHit;
												nAddHit1++;
												if (nAddHit1 >= MAX_N_ADD_HIT) {
													return StrawAlgo::abortProcessing(l1Info);
												}
											}
										}
										if (tempCamera == addCam2) {
											for (int d = 0; d < nAddHit2; d++) {
												if (addhit2[d] == tempHit) {
													chkHit = 1;
												}
											}

											if (chkHit == 0) {
												addhit2[nAddHit2] = tempHit;
												nAddHit2++;
												if (nAddHit2 >= MAX_N_ADD_HIT) {
													return StrawAlgo::abortProcessing(l1Info);
												}
											}
										}
									}
								}
							}
						}
						if (nAddHit1 > 0) {
							for (int j = 0; j < nFirstTrackCentrali + 1; j++) {
								strawFirstTempTrk_[j].hitl[tempNHitL] = addhit1[0];
								strawFirstTempTrk_[j].camerel[tempNHitL] = addCam1;
								strawFirstTempTrk_[j].nlaterali = tempNHitL + 1;
							}

							for (int d = 1; d < nAddHit1; d++) {
								for (int j = 0; j < nFirstTrackCentrali + 1; j++) {
									nFirstTrack++;
									if (nFirstTrack >= MAX_N_FIRST_TEMP_TRACK) {
										return StrawAlgo::abortProcessing(l1Info);
									}
									strawFirstTempTrk_[nFirstTrack].copyTrack(strawFirstTempTrk_[j]);
									strawFirstTempTrk_[nFirstTrack].hitl[tempNHitL] = addhit1[d];
								}
							}
							tempNHitL++;
							if (tempNHitL >= MAX_N_HIT_L) {
								return StrawAlgo::abortProcessing(l1Info);
							}
						}
						nFirstTrackCentrali = nFirstTrack; //sono aumentati in seguito all'addhit1

						if (nAddHit2 > 0) {
							for (int j = 0; j < nFirstTrackCentrali + 1; j++) {
								strawFirstTempTrk_[j].hitl[tempNHitL] = addhit2[0];
								strawFirstTempTrk_[j].camerel[tempNHitL] = addCam2;
								strawFirstTempTrk_[j].nlaterali = tempNHitL + 1;
							}

							for (int d = 1; d < nAddHit2; d++) {
								for (int j = 0; j < nFirstTrackCentrali + 1; j++) {
									nFirstTrack++;
									if (nFirstTrack >= MAX_N_FIRST_TEMP_TRACK) {
										return StrawAlgo::abortProcessing(l1Info);
									}
									strawFirstTempTrk_[nFirstTrack].copyTrack(strawFirstTempTrk_[j]);
									strawFirstTempTrk_[nFirstTrack].hitl[tempNHitL] = addhit2[d];
								}
							}
							tempNHitL++;
							if (tempNHitL >= MAX_N_HIT_L) {
								return StrawAlgo::abortProcessing(l1Info);
							}
						}

						delete[] addhit1;
						delete[] addhit2;
					}

					//now I have all the tracklet find in the bin, I have to select only the real one

					pointMY = ((float) a - ((float) RANGEM + 0.1) / 2) * PASSO;
					pointQY = b * LMAGNET / RANGEQ - LMAGNET / 2;

					//	zvertex = 197645 - (PointQY / PointMY); //oppure shiftati perchè zvertex = 197645-((PointQY+4.762)/(PointMY+0.0001989));

					for (int j = 0; j < nFirstTrack + 1; j++) { //parte X

						float x0 = 0.0;
						float x1 = 0.0;
						float x2 = 0.0;
						float x3 = 0.0;
						float y0 = 0.0;
						float y1 = 0.0;
						float y2 = 0.0;
						float y3 = 0.0;
						float z0 = 0.0;
						float z1 = 0.0;
						float z2 = 0.0;
						float z3 = 0.0;
						float q01 = 0.0;
						float q23 = 0.0;
						float dqx = 1000000.0;
						double trailingTemp = 0.0;

						if (strawFirstTempTrk_[j].ncentrali + strawFirstTempTrk_[j].nlaterali > 2) {
							for (int z = 0; z < tempNHitC; z++) {
								trailingTemp += strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].trailing;

								if (strawFirstTempTrk_[j].camerec[z] == 0) {
									x0 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].x;
									y0 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z0 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].z - ZMAGNET;
								}
								if (strawFirstTempTrk_[j].camerec[z] == 1) {
									x1 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].x;
									y1 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z1 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].z - ZMAGNET;
								}
								if (strawFirstTempTrk_[j].camerec[z] == 2) {
									x2 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].x;
									y2 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z2 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].z - ZMAGNET;
								}
								if (strawFirstTempTrk_[j].camerec[z] == 3) {
									x3 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].x;
									y3 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z3 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].z - ZMAGNET;
								}
							}
							for (int z = 0; z < tempNHitL; z++) {
								trailingTemp += strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].trailing;

								if (strawFirstTempTrk_[j].camerel[z] == 0) {
									x0 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].x;
									y0 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z0 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].z - ZMAGNET;
								}
								if (strawFirstTempTrk_[j].camerel[z] == 1) {
									x1 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].x;
									y1 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z1 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].z - ZMAGNET;
								}
								if (strawFirstTempTrk_[j].camerel[z] == 2) {
									x2 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].x;
									y2 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z2 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].z - ZMAGNET;
								}
								if (strawFirstTempTrk_[j].camerel[z] == 3) {
									x3 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].x;
									y3 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z3 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].z - ZMAGNET;
								}
							}

							if (z0 == 0 || z1 == 0) {

								q23 = x2 - z2 * (x3 - x2) / (z3 - z2);
								q01 = q23;

								strawFirstTempTrk_[j].m2x = (x3 - x2) / (z3 - z2);
								if (z0 == 0) {
									strawFirstTempTrk_[j].m1x = (x1 - q01) / z1;
								} else {
									strawFirstTempTrk_[j].m1x = (x0 - q01) / z0;
								}
							} else if (z2 == 0 || z3 == 0) {
								q01 = x0 - z0 * (x1 - x0) / (z1 - z0);
								q23 = q01;

								strawFirstTempTrk_[j].m1x = (x1 - x0) / (z1 - z0);
								if (z2 == 0) {
									strawFirstTempTrk_[j].m2x = (x3 - q23) / z3;
								} else {
									strawFirstTempTrk_[j].m2x = (x2 - q23) / z2;
								}
							} else {
								q01 = x0 - z0 * (x1 - x0) / (z1 - z0);
								q23 = x2 - z2 * (x3 - x2) / (z3 - z2);
								strawFirstTempTrk_[j].m1x = (x1 - x0) / (z1 - z0);
								strawFirstTempTrk_[j].m2x = (x3 - x2) / (z3 - z2);
							}

							dqx = q23 - q01;

							strawFirstTempTrk_[j].q1x = q01;
							strawFirstTempTrk_[j].q2x = q23;
							strawFirstTempTrk_[j].pz = 270.0 / (fabs(strawFirstTempTrk_[j].m2x - strawFirstTempTrk_[j].m1x));

							strawFirstTempTrk_[j].my = pointMY;
							strawFirstTempTrk_[j].qy = pointQY;
							strawFirstTempTrk_[j].trailing = trailingTemp / (tempNHitC + tempNHitL);

							qTrack.setPoint(0.0, strawFirstTempTrk_[j].q1x, strawFirstTempTrk_[j].qy, 0.0, 0.0, 0, 0);
							mTrack.setPoint(1.0, strawFirstTempTrk_[j].m1x, strawFirstTempTrk_[j].my, 0.0, 0.0, 0, 0);

							cdaVertex(QBeam_, qTrack, MBeam_, mTrack, cda, vertex);

							strawFirstTempTrk_[j].zvertex = vertex.z;
							strawFirstTempTrk_[j].cda = cda;

							//pz>3 Gev <100Gev, my e mx1 < 0.020
							if (strawFirstTempTrk_[j].pz > 3000 && strawFirstTempTrk_[j].pz < 100000 && strawFirstTempTrk_[j].my < 0.020
									&& strawFirstTempTrk_[j].m1x < 0.020) {
								strawTempTrk_[nTrack].copyTrack(strawFirstTempTrk_[j]);
								nTrack++;
								if (nTrack >= MAX_N_TRACKS) {
									return StrawAlgo::abortProcessing(l1Info);
								}
							}
						}
					}

				}

			}
		}
	}

	nTracletCondivisi = 0;
	nTrackIntermedie = 0;

	//prima uso le tracce con 4 hit
	for (int e = 0; e < nTrack; e++) {
		trackIntermedieTemp_my = 0.0;
		trackIntermedieTemp_qy = 0.0;
		trackIntermedieTemp_m1x = 0.0;
		trackIntermedieTemp_q1x = 0.0;
		trackIntermedieTemp_m2x = 0.0;
		trackIntermedieTemp_q2x = 0.0;

		//select the best temporary track between the similar track with 4 point
		if (strawTempTrk_[e].usato == 0 && strawTempTrk_[e].ncentrali + strawTempTrk_[e].nlaterali == 4) {
			trackIntermedieTemp.copyTrack(strawTempTrk_[e]);
			if ((strawTempTrk_[e].q2x - strawTempTrk_[e].q1x) < -13 || (strawTempTrk_[e].q2x - strawTempTrk_[e].q1x) > 0)
				for (int f = e; f < nTrack; f++) {
					tempCondivise = 0;
					if (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali == 4) {
						for (int g = 0; g < trackIntermedieTemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
							for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

								if (trackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitc[h]
										&& trackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerec[h]) {
									tempCondivise++;
								}
							}
							for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
								if (trackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitl[h]
										&& trackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerel[h]) {
									tempCondivise++;
								}
							}
						}

						for (int g = 0; g < trackIntermedieTemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
							for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

								if (trackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitc[h]
										&& trackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerec[h]) {
									tempCondivise++;
								}
							}
							for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

								if (trackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitl[h]
										&& trackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerel[h]) {
									tempCondivise++;
								}
							}
						}

						if (tempCondivise > 1
								&& (((strawTempTrk_[f].q2x - strawTempTrk_[f].q1x) > -13
										&& (strawTempTrk_[f].q2x - strawTempTrk_[f].q1x) < 0)
										|| strawTempTrk_[f].ncentrali > trackIntermedieTemp.ncentrali
										|| (strawTempTrk_[f].ncentrali == trackIntermedieTemp.ncentrali
												&& fabs(strawTempTrk_[f].q2x - strawTempTrk_[f].q1x + 3)
														< fabs(trackIntermedieTemp.q2x - trackIntermedieTemp.q1x + 3)))) {
							//  End of conditional logic

							trackIntermedieTemp.copyTrack(strawTempTrk_[f]);
						}
					}
				}

			nTracletCondivisi = 0;

			for (int f = 0; f < nTrack; f++) { //the tracks with the same point will be combined

				tempCondivise = 0;
				for (int g = 0; g < trackIntermedieTemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (trackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitc[h]
								&& trackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerec[h]) {
							tempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (trackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitl[h]
								&& trackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerel[h]) {
							tempCondivise++;
						}
					}
				}

				for (int g = 0; g < trackIntermedieTemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (trackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitc[h]
								&& trackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerec[h]) {
							tempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (trackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitl[h]
								&& trackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerel[h]) {
							tempCondivise++;
						}
					}
				}

				if (tempCondivise == (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali)) {
					nTracletCondivisi++;
					trackIntermedieTemp_my += strawTempTrk_[f].my;
					trackIntermedieTemp_qy += strawTempTrk_[f].qy;
					trackIntermedieTemp_m1x += strawTempTrk_[f].m1x;
					trackIntermedieTemp_q1x += strawTempTrk_[f].q1x;
					trackIntermedieTemp_m2x += strawTempTrk_[f].m2x;
					trackIntermedieTemp_q2x += strawTempTrk_[f].q2x;

					strawTempTrk_[f].usato = 1;
				} else {
					if (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali == 4 && tempCondivise > 1) {
						strawTempTrk_[f].usato = 1;
					} else if (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali == 3 && tempCondivise > 0) {
						strawTempTrk_[f].usato = 1;
					}
				}
			}

			trackIntermedieTemp.my = trackIntermedieTemp_my / nTracletCondivisi;
			trackIntermedieTemp.qy = trackIntermedieTemp_qy / nTracletCondivisi;
			trackIntermedieTemp.m1x = trackIntermedieTemp_m1x / nTracletCondivisi;
			trackIntermedieTemp.q1x = trackIntermedieTemp_q1x / nTracletCondivisi;
			trackIntermedieTemp.m2x = trackIntermedieTemp_m2x / nTracletCondivisi;
			trackIntermedieTemp.q2x = trackIntermedieTemp_q2x / nTracletCondivisi;

			strawTrkIntermedie_[nTrackIntermedie].copyTrack(trackIntermedieTemp);
			strawTrkIntermedie_[nTrackIntermedie].pz = 270 / (fabs(trackIntermedieTemp.m2x - trackIntermedieTemp.m1x));

			qTrack.setPoint(0.0, trackIntermedieTemp.q1x, trackIntermedieTemp.qy, 0.0, 0.0, 0, 0);
			mTrack.setPoint(1.0, trackIntermedieTemp.m1x, trackIntermedieTemp.my, 0.0, 0.0, 0, 0);

			cdaVertex(QBeam_, qTrack, MBeam_, mTrack, cda, vertex);

			strawTrkIntermedie_[nTrackIntermedie].zvertex = vertex.z;
			strawTrkIntermedie_[nTrackIntermedie].cda = cda;
			strawTrkIntermedie_[nTrackIntermedie].ncondivisi = 0;
			strawTrkIntermedie_[nTrackIntermedie].ncamcondivise = 0;
			strawTrkIntermedie_[nTrackIntermedie].usato = 0;

			nTrackIntermedie++;
			if (nTrackIntermedie >= MAX_N_TRACK_INTERMEDIE) {
				return StrawAlgo::abortProcessing(l1Info);
			}
		}
	}

	//the same with tracks with only 3 points
	for (int e = 0; e < nTrack; e++) {

		trackIntermedieTemp_my = 0.0;
		trackIntermedieTemp_qy = 0.0;
		trackIntermedieTemp_m1x = 0.0;
		trackIntermedieTemp_q1x = 0.0;
		trackIntermedieTemp_m2x = 0.0;
		trackIntermedieTemp_q2x = 0.0;

		if (strawTempTrk_[e].usato == 0 && strawTempTrk_[e].ncentrali + strawTempTrk_[e].nlaterali == 3) {
			trackIntermedieTemp.copyTrack(strawTempTrk_[e]);

			for (int f = 0; f < nTrack; f++) {
				tempCondivise = 0;
				for (int g = 0; g < trackIntermedieTemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (trackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitc[h]
								&& trackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerec[h]) {
							tempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (trackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitl[h]
								&& trackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerel[h]) {
							tempCondivise++;
						}
					}
				}

				for (int g = 0; g < trackIntermedieTemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (trackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitc[h]
								&& trackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerec[h]) {
							tempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (trackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitl[h]
								&& trackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerel[h]) {
							tempCondivise++;
						}
					}
				}

				if (tempCondivise > 1
						&& (strawTempTrk_[f].ncentrali > trackIntermedieTemp.ncentrali
								|| (strawTempTrk_[f].cda < trackIntermedieTemp.cda
										&& strawTempTrk_[f].ncentrali == trackIntermedieTemp.ncentrali))) {
					// End of conditional logic
					trackIntermedieTemp.copyTrack(strawTempTrk_[f]);
				}
			}

			nTracletCondivisi = 0;

			for (int f = 0; f < nTrack; f++) {
				tempCondivise = 0;
				for (int g = 0; g < trackIntermedieTemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (trackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitc[h]
								&& trackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerec[h]) {
							tempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (trackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitl[h]
								&& trackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerel[h]) {
							tempCondivise++;
						}
					}
				}

				for (int g = 0; g < trackIntermedieTemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (trackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitc[h]
								&& trackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerec[h]) {
							tempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (trackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitl[h]
								&& trackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerel[h]) {
							tempCondivise++;
						}
					}
				}

				if (tempCondivise == (trackIntermedieTemp.ncentrali + trackIntermedieTemp.nlaterali)) {
					nTracletCondivisi++;
					trackIntermedieTemp_my += strawTempTrk_[f].my;
					trackIntermedieTemp_qy += strawTempTrk_[f].qy;
					trackIntermedieTemp_m1x += strawTempTrk_[f].m1x;
					trackIntermedieTemp_q1x += strawTempTrk_[f].q1x;
					trackIntermedieTemp_m2x += strawTempTrk_[f].m2x;
					trackIntermedieTemp_q2x += strawTempTrk_[f].q2x;

					strawTempTrk_[f].usato = 1;
				} else {
					if (trackIntermedieTemp.ncentrali == 3 && strawTempTrk_[f].ncentrali == 2 && tempCondivise > 0) {
						strawTempTrk_[f].usato = 1;
					} else if (tempCondivise > 1) {
						strawTempTrk_[f].usato = 1;
					}
				}
			}

			trackIntermedieTemp.my = trackIntermedieTemp_my / nTracletCondivisi;
			trackIntermedieTemp.qy = trackIntermedieTemp_qy / nTracletCondivisi;
			trackIntermedieTemp.m1x = trackIntermedieTemp_m1x / nTracletCondivisi;
			trackIntermedieTemp.q1x = trackIntermedieTemp_q1x / nTracletCondivisi;
			trackIntermedieTemp.m2x = trackIntermedieTemp_m2x / nTracletCondivisi;
			trackIntermedieTemp.q2x = trackIntermedieTemp_q2x / nTracletCondivisi;

			strawTrkIntermedie_[nTrackIntermedie].copyTrack(trackIntermedieTemp);
			strawTrkIntermedie_[nTrackIntermedie].pz = 270 / (fabs(trackIntermedieTemp.m2x - trackIntermedieTemp.m1x));

			qTrack.setPoint(0.0, trackIntermedieTemp.q1x, trackIntermedieTemp.qy, 0.0, 0.0, 0, 0);
			mTrack.setPoint(1.0, trackIntermedieTemp.m1x, trackIntermedieTemp.my, 0.0, 0.0, 0, 0);

			cdaVertex(QBeam_, qTrack, MBeam_, mTrack, cda, vertex);

			strawTrkIntermedie_[nTrackIntermedie].zvertex = vertex.z;
			strawTrkIntermedie_[nTrackIntermedie].cda = cda;
			strawTrkIntermedie_[nTrackIntermedie].ncondivisi = 0;
			strawTrkIntermedie_[nTrackIntermedie].ncamcondivise = 0;
			strawTrkIntermedie_[nTrackIntermedie].usato = 0;

			nTrackIntermedie++;
			if (nTrackIntermedie >= MAX_N_TRACK_INTERMEDIE) {
				return StrawAlgo::abortProcessing(l1Info);
			}
		}
	}

	//casi a 3 tracce
	Point mTrack1;
	Point qTrack1;
	Point mTrack2;
	Point qTrack2;

	for (int e = 0; e < nTrackIntermedie; e++) {
		// We zero FlagL1Limit as needed rather than zeroing the entire array
		flagL1Limit[e] = 0;

		if (e < 5) {
			l1Info->setL1StrawTrackP(e, strawTrkIntermedie_[e].pz);
			l1Info->setL1StrawTrackVz(e, strawTrkIntermedie_[e].zvertex);
		}
		if (strawTrkIntermedie_[e].zvertex > -100000 && strawTrkIntermedie_[e].zvertex < 180000 && strawTrkIntermedie_[e].cda < 200
				&& strawTrkIntermedie_[e].pz < 50000) {
			flagL1Limit[e]++;
		}

		qTrack1.setPoint(0.0, strawTrkIntermedie_[e].q1x, strawTrkIntermedie_[e].qy, 0.0, 0.0, 0, 0);
		mTrack1.setPoint(1.0, strawTrkIntermedie_[e].m1x, strawTrkIntermedie_[e].my, 0.0, 0.0, 0, 0);

		for (int f = e + 1; f < nTrackIntermedie; f++) {
			qTrack2.setPoint(0.0, strawTrkIntermedie_[f].q1x, strawTrkIntermedie_[f].qy, 0.0, 0.0, 0, 0);
			mTrack2.setPoint(1.0, strawTrkIntermedie_[f].m1x, strawTrkIntermedie_[f].my, 0.0, 0.0, 0, 0);

			cdaVertex(qTrack1, qTrack2, mTrack1, mTrack2, cda, vertex);
			if (cda < 30) {
				flagL1Three[e]++;
				flagL1Three[f]++;
			}
		}

		if (strawTrkIntermedie_[e].m1x - strawTrkIntermedie_[e].m2x < 0) {
			flagL1Exotic = 1;
		}
		if (flagL1Limit[e] > 0 && flagL1Three[e] == 0) {
			flagL1Pnn = 1;
		}
		if (flagL1Limit[e] > 0 && flagL1Three[e] > 0) {
			flagL1TreTracks = 1;
		}
	}

//	LOG_INFO("\n RISULTATO:");
//	if (flag_l1_pnn == 1)
//		LOG_INFO("                Evento buono \n");
//
//	else
//		LOG_INFO("                Evento tagliato \n");
//
//	if (flag_l1_tretracks == 1)
//		LOG_INFO("                Evento a tre traccie \n");

	l1Info->setL1StrawNTracks(nTrackIntermedie);
	l1Info->setL1StrawProcessed();
	flagL1 = ((flagL1Exotic & 0x1) << 1) | (flagL1Pnn & 0x1);
	return flagL1;
}

uint_fast8_t StrawAlgo::abortProcessing(L1InfoToStorage* l1Info) {
	l1Info->setL1StrawOverflow();
	l1Info->setL1StrawProcessed();
	// 3 for event fulfilled both L1 straw triggers for Kpnn and exotics data streams
	return 3;
}

float StrawAlgo::posTubNew(int chamber, int view, int plane, int jstraw) {
	// Straw spacing = 17.6
	return 17.6 * jstraw + XOffset_[view][plane] + XOffCh_[chamber][view];
}

int StrawAlgo::strawAcceptance(int n, double* coordinate, int zone) {

	float viewSize = 2100.0;
	float strawSpacing = 17.6;
	float strawInnerRadius = 4.875;
	float copperThickness = 0.00005;
	float mylarThickness = 0.036;
	float goldThickness = 0.00002;
	double strawRadius = strawInnerRadius + 2 * copperThickness + mylarThickness + goldThickness;
	double strawDiameter = 2 * strawRadius;

	double viewPlaneTransverseSize = (120 - 1) * strawSpacing + strawDiameter;

	// View definition
	double a[4] = { InvSq2_, InvSq2_, 1, 0 };
	double b[4] = { InvSq2_, -InvSq2_, 0, 1 };
	double c[4] = { InvSq2_, InvSq2_, 0, 1 };
	double d[4] = { -InvSq2_, InvSq2_, 1, 0 };
	int viewFlag[4] = { 0, 0, 0, 0 };
	for (int jView = 0; jView < 4; jView++) {
		double posView = a[jView] * coordinate[0] + b[jView] * coordinate[1];
		double posAlongStraw = c[jView] * coordinate[0] + d[jView] * coordinate[1];
		if (((posView > HoleChamberMax_[n][jView] && posView < 0.5 * viewPlaneTransverseSize)
				|| (posView < HoleChamberMin_[n][jView] && posView > -0.5 * viewPlaneTransverseSize))
				&& fabs(posAlongStraw) < 0.5 * viewSize) {
			viewFlag[jView] = 1;
		}
	}
	int vu = viewFlag[0];
	int vv = viewFlag[1];
	int vx = viewFlag[2];
	int vy = viewFlag[3];

// Zones
//  This code has been deactivated since we always require
//	only two views
//
//	switch (zone) {
//	case 1:  // At least 1 view
//		if (Vx + Vy + Vu + Vv >= 1)
//			return 1;
//		return 0;
//	case 2:  // At least 2 views
//		if (Vx + Vy + Vu + Vv >= 2)
//			return 1;
//		return 0;
//	case 3:  // At least 3 views
//		if (Vx + Vy + Vu + Vv >= 3)
//			return 1;
//		return 0;
//	case 4:  // Four views only
//		if (Vx + Vy + Vu + Vv == 4)
//			return 1;
//		return 0;
//	case 11:  // One view only
//		if (Vx + Vy + Vu + Vv == 1)
//			return 1;
//		return 0;
//	case 12:  // Two views only
//		if (Vx + Vy + Vu + Vv == 2)
//			return 1;
//		return 0;
//	case 13:  // Three views only
//		if (Vx + Vy + Vu + Vv == 3)
//			return 1;
//		break;
//	default:
//		return 0;
//	}
	if (vx + vy + vu + vv == 2)
		return 1;
	return 0;
}

void StrawAlgo::cdaVertex(const Point& qBeam, Point& qTrack, const Point& mBeam, Point& mTrack, float& cda, Point& vertex) {

	Point r12;
	float t1, t2, aa, bb, cc, dd, ee, det;
	Point q1, q2;

	r12.z = qBeam.z - qTrack.z;
	r12.x = qBeam.x - qTrack.x;
	r12.y = qBeam.y - qTrack.y;

	aa = mBeam.x * mBeam.x + mBeam.y * mBeam.y + mBeam.z * mBeam.z;
	bb = mTrack.x * mTrack.x + mTrack.y * mTrack.y + mTrack.z * mTrack.z;
	cc = mBeam.x * mTrack.x + mBeam.y * mTrack.y + mBeam.z * mTrack.z;
	dd = r12.x * mBeam.x + r12.y * mBeam.y + r12.z * mBeam.z;
	ee = r12.x * mTrack.x + r12.y * mTrack.y + r12.z * mTrack.z;
	det = cc * cc - aa * bb;

	t1 = (bb * dd - cc * ee) / det;
	t2 = (cc * dd - aa * ee) / det;

	q1.z = qBeam.z + t1 * mBeam.z;
	q1.x = qBeam.x + t1 * mBeam.x;
	q1.y = qBeam.y + t1 * mBeam.y;
	q2.z = qTrack.z + t2 * mTrack.z;
	q2.x = qTrack.x + t2 * mTrack.x;
	q2.y = qTrack.y + t2 * mTrack.y;

	vertex.setPoint(ZMAGNET + (q1.z + q2.z) / 2, (q1.x + q2.x) / 2, (q1.y + q2.y) / 2, 0.0, 0.0, 0, 0);
	r12.setPoint(q1.z - q2.z, q1.x - q2.x, q1.y - q2.y, 0.0, 0.0, 0, 0);

	cda = sqrt(r12.x * r12.x + r12.y * r12.y + r12.z * r12.z);

}

void StrawAlgo::writeData(L1StrawAlgo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID written within the packet!");

	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
	algoPacket->qualityFlags = (AlgoRefTimeSourceID_[l0MaskID] << 7) | (l1Info->isL1StrawProcessed() << 6) | (l1Info->isL1StrawEmptyPacket() << 4)
			| (l1Info->isL1StrawBadData() << 2) | (l1Info->isL1StrawOverflow() << 1) | ((uint) l1Info->getL1StrawTrgWrd(l0MaskID));

	for (uint iTrk = 0; iTrk != 5; iTrk++) {
//		LOG_INFO("track index " << iTrk << " momentum " << l1Info->getL1StrawTrack_P(iTrk));
//		LOG_INFO("track index " << iTrk << " vertex " << l1Info->getL1StrawTrack_Vz(iTrk));
		algoPacket->l1Data[iTrk] = (uint) l1Info->getL1StrawTrackVz(iTrk);
		algoPacket->l1Data[iTrk + 5] = (uint) l1Info->getL1StrawTrackP(iTrk);
	}
	algoPacket->l1Data[10] = l1Info->getL1StrawNTracks();

	algoPacket->numberOfWords = (sizeof(L1StrawAlgo) / 4.);
//	LOG_INFO("l0MaskID " << l0MaskID);
//	LOG_INFO("algoID " << (uint)algoPacket->algoID);
//	LOG_INFO("quality Flags " << (uint)algoPacket->qualityFlags);
//	LOG_INFO("online TW " << (uint)algoPacket->onlineTimeWindow);
//	LOG_INFO("Data Words " << algoPacket->l1Data[0] << " " << algoPacket->l1Data[1]);

}

} /* namespace na62 */

