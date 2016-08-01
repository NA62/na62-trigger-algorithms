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

#define INVISIBLE_SHIFT 25.2 //28.4 //27
#define CLOCK_PERIOD 24.951059536
//#define rangem 160//160
#define passo 0.0004//0.0002
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

void StrawAlgo::initialize(uint I, l1Straw &L1StrawStruct) {

	AlgoID_ = L1StrawStruct.configParams.l1TrigMaskID;
	AlgoLogic_[I] = L1StrawStruct.configParams.l1TrigLogic;
	AlgoRefTimeSourceID_[I] = L1StrawStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	AlgoOnlineTimeWindow_[I] = L1StrawStruct.configParams.l1TrigOnlineTimeWindow;

	//	LOG_INFO("Straw mask: " << i << " logic " << AlgoLogic_[i] << " refTimeSourceID " << AlgoRefTimeSourceID_[i] << " online time window " << AlgoOnlineTimeWindow_[i]);
}

uint_fast8_t StrawAlgo::processStrawTrigger(uint L0MaskID, DecoderHandler& Decoder, L1InfoToStorage* L1Info) {

	struct timeval time[30];
	//gettimeofday(&time[0], 0);
	//LOG_INFO( "Initial Time - Start " << time[0].tv_sec << " " << time[0].tv_usec );

	using namespace l0;

	int FlagL1 = 0;
	int FlagL1Pnn = 0;
	int FlagL1Exotic = 0;
	int FlagL1Limit[1000];
	int FlagL1Three[1000] = { 0 };
	int FlagL1TreTracks = 0;

	uint NEdgesTotal = 0;
	uint NHits = 0;
	bool TLFlags = 0;
	int NChambersHit = 0;

	int NTotalHit = 0; //just for debug
	int NTotalViewCluster = 0; //just for debug
	int NTotalPreclusters = 0; //just for debug

	int NTracletCondivisi;
	int NTrackIntermedie;

	Track TrackIntermedieTemp;
	float TrackIntermedieTemp_my;
	float TrackIntermedieTemp_qy;
	float TrackIntermedieTemp_m1x;
	float TrackIntermedieTemp_q1x;
	float TrackIntermedieTemp_m2x;
	float TrackIntermedieTemp_q2x;

	int TempCondivise;

	int NStrawPointsTemp[4] = { 0 };
	int NStrawPointsTempBis[4] = { 0 };
	int NStrawPointsFinal[4] = { 0 };
	int NStrawClusters[4][4];
	int NStrawPreclusters[4][4][2];

	for (int i = 0; i != 4; ++i) {
		for (int j = 0; j != 4; ++j) {
			NStrawClusters[i][j] = 0;
			// These two are addressed directly to avoid adding another loop
			// for just two items
			NStrawPreclusters[i][j][0] = 0;
			NStrawPreclusters[i][j][1] = 0;
		}
	}

	Point QTrack;
	Point MTrack;
	Point Vertex;
	long long Hought[RANGEM][RANGEQ];

	for (int a = 0; a < RANGEM; a++)
		for (int b = 0; b < RANGEQ; b++)
			Hought[a][b] = 0;

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
	for (SrbFragmentDecoder* strawPacket_ : Decoder.getSTRAWDecoderRange()) {
		//	gettimeofday(&time[3], 0);
		//	LOG_INFO( "Access Packets - Start " << time[3].tv_sec << " " << time[3].tv_usec );

		if (!strawPacket_->isReady() || strawPacket_->isBadFragment()) {
			LOG_ERROR("STRAWAlgo: This looks like a Bad Packet!!!! ");
			L1Info->setL1StrawBadData();
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

		NTotalHit += numberOfEdgesOfCurrentBoard;

		//gettimeofday(&time[4], 0);
		//		LOG_INFO( "Access Packets - Stop " << time[4].tv_sec << " " << time[4].tv_usec );
		//		LOG_INFO( "Access SRB Packets " << ((time[4].tv_sec - time[3].tv_sec)*1e6 + time[4].tv_usec) - time[3].tv_usec );
		//LOG_INFO( "nhit " << numberOfEdgesOfCurrentBoard);

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {

			//LOG_INFO("srb_add = " << (uint)srbAddr[iEdge] << " straw_add = " << (uint)strawAddr[iEdge] << " edge(lead)= " << edgeIsLeading[iEdge]);
			//	gettimeofday(&time[5], 0);
			//	LOG_INFO( "Read Config File and Assign ChannelID - Start " << time[5].tv_sec << " " << time[5].tv_usec );

			TLFlags = 0;

			const int roChID = 256 * srbAddr[iEdge] + strawAddr[iEdge];

			int ChamberID = StrawGeo_[roChID] / 1952;
			int ViewID = (StrawGeo_[roChID] % 1952) / 488;
			int HalfViewID = (StrawGeo_[roChID] % 488) / 244;
			int PlaneID = (StrawGeo_[roChID] % 244) / 122;
			int StrawID = StrawGeo_[roChID] % 122;
			float Leading = -100000.;
			float Trailing = -100000.;
			float WireDistance = -100.0;

			int coverAddr = ((strawAddr[iEdge] & 0xf0) >> 4);
			//LOG_INFO( "SrbAddr " << (uint)srbAddr[iEdge] << " StrawAddr "<< (uint)strawAddr[iEdge] << " CoverAddr " << coverAddr << " fR0Mezz Index " << srbAddr[iEdge] * 16 + coverAddr );

			//LOG_INFO(chRO[nHits] << " " << strawGeo[chRO[nHits]]);
			/* LOG_INFO("ChamberID " << chamberID
			 << " ViewID " << viewID
			 << " HalfViewID " << halfviewID
			 << " PlaneID " << planeID
			 << " StrawID " << strawID
			 << " IsALeading " << edgeIsLeading[iEdge]);
			 */

			if (edgeIsLeading[iEdge]) {
				Leading = (double) edgeTime[iEdge] - (double) StationT0_ - (double) ROMezzaninesT0_[srbAddr[iEdge] * 16 + coverAddr]
						+ (double) INVISIBLE_SHIFT - (((double) Decoder.getDecodedEvent()->getFinetime() * CLOCK_PERIOD) / 256 + 0.5);
			}
			if (!edgeIsLeading[iEdge]) {
				Trailing = (double) edgeTime[iEdge] - (double) StationT0_ - (double) ROMezzaninesT0_[srbAddr[iEdge] * 16 + coverAddr]
						+ (double) INVISIBLE_SHIFT - (((double) Decoder.getDecodedEvent()->getFinetime() * CLOCK_PERIOD) / 256 + 0.5);
			}
			//gettimeofday(&time[6], 0);
			//LOG_INFO( "Read Config File and Assign ChannelID - Stop " << time[6].tv_sec << " " << time[6].tv_usec );
			//LOG_INFO( "Read Conf file and Assign ChannelID and time" << ((time[6].tv_sec - time[5].tv_sec)*1e6 + time[6].tv_usec) - time[5].tv_usec );
			//gettimeofday(&time[7], 0);
			//	LOG_INFO( "Access Straw Map - Start " << time[7].tv_sec << " " << time[7].tv_usec );
			float Position = posTubNew(ChamberID, ViewID, HalfViewID * 2 + PlaneID, StrawID);

			//gettimeofday(&time[8], 0);
			//	LOG_INFO( "Access Straw Map - Stop " << time[8].tv_sec << " " << time[8].tv_usec );
			//	LOG_INFO( "Access Straw Map and give position of the tube" << ((time[8].tv_sec - time[7].tv_sec)*1e6 + time[8].tv_usec) - time[7].tv_usec );

			//////////////PRECLUSTERING, first leading and last trailing//////////////////////////////

			//gettimeofday(&time[9], 0);
			//	LOG_INFO( "Preclustering - Start " << time[9].tv_sec << " " << time[9].tv_usec );
			//LOG_INFO( "Preparazione degli hit " << ((time[9].tv_sec - time[0].tv_sec)*1e6 + time[9].tv_usec) - time[0].tv_usec );

			for (int j = 0; j != NStrawPreclusters[ChamberID][ViewID][HalfViewID]; ++j) {
				if ((strawPrecluster_[ChamberID][ViewID][HalfViewID][j].plane == PlaneID)
						&& (strawPrecluster_[ChamberID][ViewID][HalfViewID][j].tube == StrawID)) {
					TLFlags = 1;
					if ((edgeIsLeading[iEdge])
							&& (Leading < strawPrecluster_[ChamberID][ViewID][HalfViewID][j].leading
									|| strawPrecluster_[ChamberID][ViewID][HalfViewID][j].leading <= -10)
							&& (Leading > -10 && Leading < 300)) {
						if (Leading < 1)
							WireDistance = DigiManager::rTDependenceData(1.0);
						else
							WireDistance = DigiManager::rTDependenceData(Leading / 1000);

						strawPrecluster_[ChamberID][ViewID][HalfViewID][j].leading = Leading;
						strawPrecluster_[ChamberID][ViewID][HalfViewID][j].wiredistance = WireDistance;
					}
					else if ((!edgeIsLeading[iEdge])
							&& (strawPrecluster_[ChamberID][ViewID][HalfViewID][j].trailing < -100
									|| strawPrecluster_[ChamberID][ViewID][HalfViewID][j].trailing < Trailing)
							&& (Trailing > -100 && Trailing < 300)) {
						strawPrecluster_[ChamberID][ViewID][HalfViewID][j].trailing = Trailing;
					}
				}
			}
			if (!TLFlags) {
				if (Leading > -100)
					if (Leading < 1)
						WireDistance = 0.0;
					else
						WireDistance = DigiManager::rTDependenceData(Leading / 1000);
				else
					WireDistance = -100.0;

				strawPrecluster_[ChamberID][ViewID][HalfViewID][NStrawPreclusters[ChamberID][ViewID][HalfViewID]].setStraw(ChamberID,
						ViewID, HalfViewID, PlaneID, StrawID, Leading, Trailing, 0, srbAddr[iEdge], Position, WireDistance);
				NStrawPreclusters[ChamberID][ViewID][HalfViewID]++;
			}

			NHits++;
			//gettimeofday(&time[10], 0);
			//			LOG_INFO( "Preclustering - Stop " << time[10].tv_sec << " " << time[10].tv_usec );
			//LOG_INFO( "Preclustering (xDigi) " << ((time[10].tv_sec - time[9].tv_sec)*1e6 + time[10].tv_usec) - time[9].tv_usec );
		}
		NEdgesTotal += numberOfEdgesOfCurrentBoard;
	}

	if (!NEdgesTotal)
		L1Info->setL1StrawEmptyPacket();

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
				NTotalPreclusters += NStrawPreclusters[i][j][h];
			}
		}
	}

	///////////////////////////////////////Start Clustering inside the view///////////////////////////////////////////////////////
	//LOG_INFO("\n CLUSTERING INSIDE THE VIEW \n");
	//gettimeofday(&time[12], 0);
	//	LOG_INFO( "Clustering inside the view - Start " << time[12].tv_sec << " " << time[12].tv_usec );

	float PositionH = 0.0;
	float PositionJ = 0.0;
	float MeanDistance = 0.0;
	float DeltaDistance = 0.0;
	float DeltaDistanceTriplets = 0.0;
	float TempDistance = 0.0;
	double TrailingCluster = 0.0;

	for (int i = 0; i < 4; i++) {
		for (int g = 0; g < 4; g++) {
			//triplette 1

			for (int j = 0; j < NStrawPreclusters[i][g][0]; j++) { //hit loop

				// cluster con 3 hit (2 della stessa mezza vista)
				if (((strawPrecluster_[i][g][0][j].leading < (M1LeadTrail_ * strawPrecluster_[i][g][0][j].trailing + Q1LeadTrail_)
						&& strawPrecluster_[i][g][0][j].leading > (M2LeadTrail_ * strawPrecluster_[i][g][0][j].trailing + Q2LeadTrail_)
						&& (strawPrecluster_[i][g][0][j].trailing > CutLowTrailing_
								&& strawPrecluster_[i][g][0][j].trailing < CutHighTrailing_))
						|| strawPrecluster_[i][g][0][j].trailing < -99999)
						&& (strawPrecluster_[i][g][0][j].leading > CutLowLeading_ && strawPrecluster_[i][g][0][j].leading < CutHighLeading_)) {
					for (int h = j + 1; h < NStrawPreclusters[i][g][0] && !strawPrecluster_[i][g][0][j].used; h++) {
						//prendo un hit appertenente al piano 2 o 3

						if (((strawPrecluster_[i][g][0][h].leading < (M1LeadTrail_ * strawPrecluster_[i][g][0][h].trailing + Q1LeadTrail_)
								&& strawPrecluster_[i][g][0][h].leading
										> (M2LeadTrail_ * strawPrecluster_[i][g][0][h].trailing + Q2LeadTrail_)
								&& (strawPrecluster_[i][g][0][h].trailing > CutLowTrailing_
										&& strawPrecluster_[i][g][0][h].trailing < CutHighTrailing_))
								|| strawPrecluster_[i][g][0][h].trailing < -99999)
								&& (strawPrecluster_[i][g][0][h].leading > CutLowLeading_
										&& strawPrecluster_[i][g][0][h].leading < CutHighLeading_)) {
							TempDistance = strawPrecluster_[i][g][0][j].position - strawPrecluster_[i][g][0][h].position;
							if (TempDistance < 9 && TempDistance > -9 && !strawPrecluster_[i][g][0][h].used) {
								if (TempDistance > 0) {
									PositionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
									PositionJ = strawPrecluster_[i][g][0][j].position - strawPrecluster_[i][g][0][j].wiredistance;
								}
								else {
									PositionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
									PositionJ = strawPrecluster_[i][g][0][j].position + strawPrecluster_[i][g][0][j].wiredistance;
								}

								MeanDistance = (PositionH + PositionJ) / 2;

								DeltaDistanceTriplets = strawPrecluster_[i][g][0][h].wiredistance
										+ strawPrecluster_[i][g][0][j].wiredistance;

								if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][0][j].trailing > -200)
									TrailingCluster = (strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][0][j].trailing) / 2;
								else {
									if (strawPrecluster_[i][g][0][h].trailing <= -200)
										TrailingCluster = strawPrecluster_[i][g][0][j].trailing;
									else
										TrailingCluster = strawPrecluster_[i][g][0][h].trailing;
								}

								if (DeltaDistanceTriplets > Hit3Low_ && DeltaDistanceTriplets < Hit3High_) {
									strawCluster_[i][g][NStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
											strawPrecluster_[i][g][0][h].view, MeanDistance, TrailingCluster, DeltaDistanceTriplets, 0);

									NStrawClusters[i][g]++;
									strawPrecluster_[i][g][0][h].used = 1;
									strawPrecluster_[i][g][0][j].used = 1;

									for (int l = 0; l < NStrawPreclusters[i][g][1]; l++) {
										//cerco lo hit 3 negli altri 2 piani

										TempDistance = fabs(MeanDistance - strawPrecluster_[i][g][1][l].position);
										if (TempDistance < 5 && !strawPrecluster_[i][g][1][l].used) {
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

			for (int j = 0; j < NStrawPreclusters[i][g][1]; j++) {
				//hit loop

				// cluster con 3 hit (due della stessa mezza vista)
				if (((strawPrecluster_[i][g][1][j].leading < (M1LeadTrail_ * strawPrecluster_[i][g][1][j].trailing + Q1LeadTrail_)
						&& strawPrecluster_[i][g][1][j].leading > (M2LeadTrail_ * strawPrecluster_[i][g][1][j].trailing + Q2LeadTrail_)
						&& (strawPrecluster_[i][g][1][j].trailing > CutLowTrailing_
								&& strawPrecluster_[i][g][1][j].trailing < CutHighTrailing_))
						|| strawPrecluster_[i][g][1][j].trailing < -99999)
						&& (strawPrecluster_[i][g][1][j].leading > CutLowLeading_ && strawPrecluster_[i][g][1][j].leading < CutHighLeading_)) {
					// End of conditional logic

					for (int h = j + 1; h < NStrawPreclusters[i][g][1] && !strawPrecluster_[i][g][1][j].used; h++) {
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

							TempDistance = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][h].position;

							if (TempDistance < 9 && TempDistance > -9 && !strawPrecluster_[i][g][1][h].used) {
								if (TempDistance > 0) {
									PositionH = strawPrecluster_[i][g][1][h].position + strawPrecluster_[i][g][1][h].wiredistance;
									PositionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
								}
								else {
									PositionH = strawPrecluster_[i][g][1][h].position - strawPrecluster_[i][g][1][h].wiredistance;
									PositionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
								}

								MeanDistance = (PositionH + PositionJ) / 2;

								DeltaDistanceTriplets = strawPrecluster_[i][g][1][h].wiredistance
										+ strawPrecluster_[i][g][1][j].wiredistance;

								if (strawPrecluster_[i][g][1][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200) {
									TrailingCluster = (strawPrecluster_[i][g][1][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
								}
								else {
									if (strawPrecluster_[i][g][1][h].trailing <= -200) {
										TrailingCluster = strawPrecluster_[i][g][1][j].trailing;
									}
									else {
										TrailingCluster = strawPrecluster_[i][g][1][h].trailing;
									}
								}

								if (DeltaDistanceTriplets > Hit3Low_ && DeltaDistanceTriplets < Hit3High_) {
									strawCluster_[i][g][NStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][1][h].chamber,
											strawPrecluster_[i][g][1][h].view, MeanDistance, TrailingCluster, DeltaDistanceTriplets, 0);

									NStrawClusters[i][g]++;
									strawPrecluster_[i][g][1][h].used = 1;
									strawPrecluster_[i][g][1][j].used = 1;

									for (int l = 0; l < NStrawPreclusters[i][g][0]; l++) {
										//cerco il 3 hit negli altri 2 piani

										TempDistance = fabs(MeanDistance - strawPrecluster_[i][g][0][l].position);
										if (TempDistance < 5 && !strawPrecluster_[i][g][0][l].used) {
											strawPrecluster_[i][g][0][l].used = 1;
										}
									}
								}
							}
						}
					}

					//cluster con 2 hit
					for (int h = 0; h < NStrawPreclusters[i][g][0] && !strawPrecluster_[i][g][1][j].used; h++) {
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

							TempDistance = fabs(strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][0][h].position);
							if (TempDistance < 9 && !strawPrecluster_[i][g][0][h].used) {
								switch (strawPrecluster_[i][g][1][j].plane) {
								case (0):
									if (!strawPrecluster_[i][g][0][h].plane) {
										if (!g || g == 2) {
											PositionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											PositionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										}
										else {
											PositionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											PositionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										}

										MeanDistance = (PositionH + PositionJ) / 2;

										DeltaDistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200) {
											TrailingCluster =
													(strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
										}
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200) {
												TrailingCluster = strawPrecluster_[i][g][1][j].trailing;
											}
											else {
												TrailingCluster = strawPrecluster_[i][g][0][h].trailing;
											}
										}

										if (DeltaDistance > Hit2Low_ && DeltaDistance < Hit2High_) {
											strawCluster_[i][g][NStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, MeanDistance, TrailingCluster, DeltaDistance, 0);
											NStrawClusters[i][g]++;
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									}
									else {
										if (!g || g == 2) {
											PositionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											PositionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										}
										else {
											PositionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											PositionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										}

										MeanDistance = (PositionH + PositionJ) / 2;

										DeltaDistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											TrailingCluster =
													(strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200) {
												TrailingCluster = strawPrecluster_[i][g][1][j].trailing;
											}
											else {
												TrailingCluster = strawPrecluster_[i][g][0][h].trailing;
											}
										}

										if (DeltaDistance > Hit2Low_ && DeltaDistance < Hit2High_) {
											strawCluster_[i][g][NStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, MeanDistance, TrailingCluster, DeltaDistance, 0);
											NStrawClusters[i][g]++;
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									}

									break;
								case (1):
									if (!strawPrecluster_[i][g][0][h].plane) {
										if (!g || g == 2) {
											PositionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											PositionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										}
										else {
											PositionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											PositionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										}

										MeanDistance = (PositionH + PositionJ) / 2;

										DeltaDistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											TrailingCluster =
													(strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200)
												TrailingCluster = strawPrecluster_[i][g][1][j].trailing;
											else
												TrailingCluster = strawPrecluster_[i][g][0][h].trailing;
										}

										if (DeltaDistance > Hit2Low_ && DeltaDistance < Hit2High_) {
											strawCluster_[i][g][NStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, MeanDistance, TrailingCluster, DeltaDistance, 0);
											NStrawClusters[i][g]++;
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									}
									else {
										if (!g || g == 2) {
											PositionH = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											PositionJ = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										}
										else {
											PositionH = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											PositionJ = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										}

										MeanDistance = (PositionH + PositionJ) / 2;

										DeltaDistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											TrailingCluster =
													(strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200) {
												TrailingCluster = strawPrecluster_[i][g][1][j].trailing;
											}
											else {
												TrailingCluster = strawPrecluster_[i][g][0][h].trailing;
											}
										}

										if (DeltaDistance > Hit2Low_ && DeltaDistance < Hit2High_) {
											strawCluster_[i][g][NStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, MeanDistance, TrailingCluster, DeltaDistance, 0);
											NStrawClusters[i][g]++;
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

	float CoordinateTemp = 0.0;
	float ViewDistance = 0.0;
	float ViewDistance4 = 0.0;
	float XTemp = 0.0;
	float YTemp = 0.0;

	//tracks reco
	float QY = 0.0;
	double Coordinate[2];

	for (int i = 0; i < 4; i++) {
		////////////ciclo dei punti a 4 viste//////////////////
		//gettimeofday(&time[15], 0);
		NTotalViewCluster += NStrawClusters[i][3] + NStrawClusters[i][2] + NStrawClusters[i][1] + NStrawClusters[i][0];
		for (int a = 0; a < NStrawClusters[i][3]; a++) { //clusters [a] is inside y

			for (int b = 0; b < NStrawClusters[i][2]; b++) { //we loop on x view

				for (int c = 0; c < NStrawClusters[i][1]; c++) { //we loop on u  views
					CoordinateTemp = (strawCluster_[i][3][a].coordinate + strawCluster_[i][2][b].coordinate) / Sq2_;
					ViewDistance = fabs(strawCluster_[i][1][c].coordinate - CoordinateTemp);

					if (ViewDistance < CutCluster_) {
						for (int d = 0; d < NStrawClusters[i][0]; d++) { //v views
							CoordinateTemp = (strawCluster_[i][2][b].coordinate - strawCluster_[i][3][a].coordinate) / Sq2_;
							ViewDistance4 = fabs(strawCluster_[i][0][d].coordinate - CoordinateTemp);

							if (ViewDistance4 < CutCluster_) {
								XTemp = strawCluster_[i][2][b].coordinate;
								YTemp = strawCluster_[i][3][a].coordinate;

								if (ViewDistance < ViewDistance4) {
									strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], XTemp, YTemp,
											(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing
													+ strawCluster_[i][1][c].trailing + strawCluster_[i][0][d].trailing) / 4, ViewDistance,
											4, 0);
								}
								else {
									strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], XTemp, YTemp,
											(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing
													+ strawCluster_[i][1][c].trailing + strawCluster_[i][0][d].trailing) / 4, ViewDistance4,
											4, 0);
								}
								strawCluster_[i][0][d].used = 1;
								strawCluster_[i][1][c].used = 1;
								strawCluster_[i][2][b].used = 1;
								strawCluster_[i][3][a].used = 1;

								NStrawPointsTemp[i]++;

							}
						}
					}
				}
			}
		}
		//gettimeofday(&time[16], 0);
		//////////////////////////// primo ciclo dei punti a 3 viste prima yx cercando in v o u, poi uv cercando in x o y
		//gettimeofday(&time[17], 0);
		for (int a = 0; a < NStrawClusters[i][3]; a++) { //clusters [a] is inside y
			for (int b = 0; b < NStrawClusters[i][2]; b++) { //we loop on x view
				for (int c = 0; c < NStrawClusters[i][1]; c++) { //we loop on u  views
					if ((strawCluster_[i][1][c].used != 1 && strawCluster_[i][2][b].used != 1)
							|| (strawCluster_[i][1][c].used != 1 && strawCluster_[i][3][a].used != 1)
							|| (strawCluster_[i][2][b].used != 1 && strawCluster_[i][3][a].used != 1)) {
						// End of conditional logic

						CoordinateTemp = (strawCluster_[i][3][a].coordinate + strawCluster_[i][2][b].coordinate) / Sq2_;
						ViewDistance = fabs(strawCluster_[i][1][c].coordinate - CoordinateTemp);

						if (ViewDistance < CutCluster_) {
							XTemp = strawCluster_[i][2][b].coordinate;
							YTemp = strawCluster_[i][3][a].coordinate;

							strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], XTemp, YTemp,
									(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing + strawCluster_[i][1][c].trailing)
											/ 3, ViewDistance, 3, 0);

							strawCluster_[i][1][c].used = 2;
							strawCluster_[i][2][b].used = 2;
							strawCluster_[i][3][a].used = 2;

							NStrawPointsTemp[i]++;
						}
					}
				}
				for (int c = 0; c < NStrawClusters[i][0]; c++) { //we loop on v views
					if ((strawCluster_[i][0][c].used != 1 && strawCluster_[i][2][b].used != 1)
							|| (strawCluster_[i][0][c].used != 1 && strawCluster_[i][3][a].used != 1)
							|| (strawCluster_[i][2][b].used != 1 && strawCluster_[i][3][a].used != 1)) {
						// End of conditional logic

						CoordinateTemp = (strawCluster_[i][2][b].coordinate - strawCluster_[i][3][a].coordinate) / Sq2_;
						ViewDistance = fabs(strawCluster_[i][0][c].coordinate - CoordinateTemp);

						if (ViewDistance < CutCluster_) {
							XTemp = strawCluster_[i][2][b].coordinate;
							YTemp = strawCluster_[i][3][a].coordinate;

							strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], XTemp, YTemp,
									(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing + strawCluster_[i][0][c].trailing)
											/ 3, ViewDistance, 3, 0);

							strawCluster_[i][0][c].used = 2;
							strawCluster_[i][2][b].used = 2;
							strawCluster_[i][3][a].used = 2;

							NStrawPointsTemp[i]++;
						}
					}
				}
			}
		}
		//gettimeofday(&time[18], 0);
		/////////////////////////3 viste seconda clusterizzazione partendo da u e v

		//gettimeofday(&time[19], 0);
		//		LOG_INFO( nStrawClusters[i][0] );
		for (int a = 0; a < NStrawClusters[i][0]; a++) { //v
			// if(strawCluster_[i][0][a].used != 1)
			// {
			//			LOG_INFO( nStrawClusters[i][1] );
			// }
			for (int b = 0; b < NStrawClusters[i][1]; b++) { //u
				// if(strawCluster_[i][1][b].used != 1)
				// {
				// 			LOG_INFO( nStrawClusters[i][2] );
				// }
				for (int c = 0; c < NStrawClusters[i][2]; c++) { //x
					if ((strawCluster_[i][0][a].used != 1 && strawCluster_[i][1][b].used != 1)
							|| (strawCluster_[i][0][a].used != 1 && strawCluster_[i][2][c].used != 1)
							|| (strawCluster_[i][1][b].used != 1 && strawCluster_[i][2][c].used != 1)) {
						// End of conditional logic

						CoordinateTemp = (strawCluster_[i][0][a].coordinate + strawCluster_[i][1][b].coordinate) / Sq2_;
						ViewDistance = fabs(strawCluster_[i][2][c].coordinate - CoordinateTemp);

						if (ViewDistance < CutCluster_) {
							XTemp = strawCluster_[i][2][c].coordinate;
							YTemp = (strawCluster_[i][1][b].coordinate - strawCluster_[i][0][a].coordinate) / Sq2_;

							strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], XTemp, YTemp,
									(strawCluster_[i][0][a].trailing + strawCluster_[i][1][b].trailing + strawCluster_[i][2][c].trailing)
											/ 3, ViewDistance, 3, 0);

							strawCluster_[i][0][a].used = 2;
							strawCluster_[i][1][b].used = 2;
							strawCluster_[i][2][c].used = 2;

							NStrawPointsTemp[i]++;
						}
					}
				}
				//				LOG_INFO( nStrawClusters[i][3] );
				for (int c = 0; c < NStrawClusters[i][3]; c++) { //y
					if ((strawCluster_[i][0][a].used != 1 && strawCluster_[i][1][b].used != 1)
							|| (strawCluster_[i][0][a].used != 1 && strawCluster_[i][3][c].used != 1)
							|| (strawCluster_[i][1][b].used != 1 && strawCluster_[i][3][c].used != 1)) {
						// End of condititional logic

						CoordinateTemp = (strawCluster_[i][1][b].coordinate - strawCluster_[i][0][a].coordinate) / Sq2_;
						ViewDistance = fabs(strawCluster_[i][3][c].coordinate - CoordinateTemp);

						if (ViewDistance < CutCluster_) {
							XTemp = (strawCluster_[i][0][a].coordinate + strawCluster_[i][1][b].coordinate) / Sq2_;
							YTemp = strawCluster_[i][3][c].coordinate;

							strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], XTemp, YTemp,
									(strawCluster_[i][0][a].trailing + strawCluster_[i][1][b].trailing + strawCluster_[i][3][c].trailing)
											/ 3, ViewDistance, 3, 0);

							strawCluster_[i][0][a].used = 2;
							strawCluster_[i][1][b].used = 2;
							strawCluster_[i][3][c].used = 2;

							NStrawPointsTemp[i]++;
						}
					}
				}
			}
		}
		//gettimeofday(&time[20], 0);
		//clusterizzazione nelle camere con sole 2 viste
		//gettimeofday(&time[21], 0);
		for (int a = 0; a < NStrawClusters[i][0]; a++) { //v

			for (int b = 0; b < NStrawClusters[i][1]; b++) { //u (v,u)

				if (strawCluster_[i][1][b].used == 0 && strawCluster_[i][0][a].used == 0) {
					Coordinate[0] = (strawCluster_[i][0][a].coordinate + strawCluster_[i][1][b].coordinate) / Sq2_;
					Coordinate[1] = (strawCluster_[i][1][b].coordinate - strawCluster_[i][0][a].coordinate) / Sq2_;

					if (strawAcceptance(i, Coordinate, 12) == 1) {
						strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], Coordinate[0], Coordinate[1],
								(strawCluster_[i][0][a].trailing + strawCluster_[i][1][b].trailing) / 2, 0, 2, 0);
						NStrawPointsTemp[i]++;
					}
				}
			}
			for (int b = 0; b < NStrawClusters[i][2]; b++) { //x (v,x)

				if (strawCluster_[i][2][b].used == 0 && strawCluster_[i][0][a].used == 0) {
					Coordinate[0] = strawCluster_[i][2][b].coordinate;
					Coordinate[1] = strawCluster_[i][2][b].coordinate - (Sq2_ * strawCluster_[i][0][a].coordinate);

					if (strawAcceptance(i, Coordinate, 12) == 1) {
						strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], Coordinate[0], Coordinate[1],
								(strawCluster_[i][0][a].trailing + strawCluster_[i][2][b].trailing) / 2, 0, 2, 0);
						NStrawPointsTemp[i]++;
					}
				}
			}
			for (int b = 0; b < NStrawClusters[i][3]; b++) { //y (v,y)

				if (strawCluster_[i][3][b].used == 0 && strawCluster_[i][0][a].used == 0) {
					Coordinate[0] = (Sq2_ * strawCluster_[i][0][a].coordinate) + strawCluster_[i][3][b].coordinate;
					Coordinate[1] = strawCluster_[i][3][b].coordinate;

					if (strawAcceptance(i, Coordinate, 12) == 1) {
						strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], Coordinate[0], Coordinate[1],
								(strawCluster_[i][0][a].trailing + strawCluster_[i][3][b].trailing) / 2, 0, 2, 0);
						NStrawPointsTemp[i]++;
					}
				}
			}
		}
		//gettimeofday(&time[22], 0);
		//gettimeofday(&time[23], 0);
		for (int a = 0; a < NStrawClusters[i][1]; a++) { //u

			for (int b = 0; b < NStrawClusters[i][2]; b++) { //x (u,x)

				if (strawCluster_[i][2][b].used == 0 && strawCluster_[i][1][a].used == 0) {
					Coordinate[0] = strawCluster_[i][2][b].coordinate;
					Coordinate[1] = (Sq2_ * strawCluster_[i][1][a].coordinate) - strawCluster_[i][2][b].coordinate;

					if (strawAcceptance(i, Coordinate, 12) == 1) {
						strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], Coordinate[0], Coordinate[1],
								(strawCluster_[i][1][a].trailing + strawCluster_[i][2][b].trailing) / 2, 0, 2, 0);
						NStrawPointsTemp[i]++;
					}
				}
			}
			for (int b = 0; b < NStrawClusters[i][3]; b++) { //y (u,y)

				if (strawCluster_[i][3][b].used == 0 && strawCluster_[i][1][a].used == 0) {
					Coordinate[0] = (Sq2_ * strawCluster_[i][1][a].coordinate) - strawCluster_[i][3][b].coordinate;
					Coordinate[1] = strawCluster_[i][3][b].coordinate;

					if (strawAcceptance(i, Coordinate, 12) == 1) {
						strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], Coordinate[0], Coordinate[1],
								(strawCluster_[i][1][a].trailing + strawCluster_[i][3][b].trailing) / 2, 0, 2, 0);
						NStrawPointsTemp[i]++;
					}
				}
			}
		}
		//gettimeofday(&time[24], 0);
		//gettimeofday(&time[25], 0);
		for (int a = 0; a < NStrawClusters[i][2]; a++) { //x

			for (int b = 0; b < NStrawClusters[i][3]; b++) { //y (x,y)

				if (strawCluster_[i][3][b].used == 0 && strawCluster_[i][2][a].used == 0) {
					Coordinate[0] = strawCluster_[i][2][a].coordinate;
					Coordinate[1] = strawCluster_[i][3][b].coordinate;

					if (strawAcceptance(i, Coordinate, 12) == 1) {
						strawPointTemp_[i][NStrawPointsTemp[i]].setPoint(ChamberZPosition_[i], Coordinate[0], Coordinate[1],
								(strawCluster_[i][2][a].trailing + strawCluster_[i][3][b].trailing) / 2, 0, 2, 0);
						NStrawPointsTemp[i]++;
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
	float viewdistance_delta = 0.0;

	for (int i = 0; i < 4; i++) {
		if (NStrawPointsTemp[i] < 7) {
			//			LOG_INFO( "!!!!!!!!!!!!!! Sto entrando qui !!!!!! " );
			NStrawPointsFinal[i] = NStrawPointsTemp[i];
			for (int j = 0; j < NStrawPointsFinal[i]; j++) { //loop over point
				strawPointFinal_[i][j] = strawPointTemp_[i][j];
			}
			continue;
		}
		for (int j = 0; j < NStrawPointsTemp[i]; j++) { //loop over point

			if (strawPointTemp_[i][j].nViews == 4 && !strawPointTemp_[i][j].used) {
				pointTemp.clonePoint(strawPointTemp_[i][j]);
				strawPointTemp_[i][j].used = 1;
				for (int k = j + 1; k < NStrawPointsTemp[i]; k++) { //loop over point

					if (strawPointTemp_[i][k].nViews == 4 && !strawPointTemp_[i][k].used) {
						//						point_dx = fabs(strawPointTemp_[i][j].x-strawPointTemp_[i][k].x);
						point_dy = fabs(strawPointTemp_[i][j].y - strawPointTemp_[i][k].y);
						if (point_dy < 4.0) {
							viewdistance_delta = pointTemp.viewDistance - strawPointTemp_[i][k].viewDistance;
							if (viewdistance_delta > 1.0 && pointTemp.viewDistance > 3.0) {
								pointTemp.clonePoint(strawPointTemp_[i][k]);
								strawPointTemp_[i][k].used = 1;
							}
							if (viewdistance_delta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTemp_[i][k].used = 1;
							}
						}
					}
				}
				strawPointTempbis_[i][NStrawPointsTempBis[i]].clonePoint(pointTemp);
				NStrawPointsTempBis[i]++;
			}
			if (strawPointTemp_[i][j].nViews == 3 && !strawPointTemp_[i][j].used) {
				pointTemp.clonePoint(strawPointTemp_[i][j]);
				strawPointTemp_[i][j].used = 1;
				for (int k = j + 1; k < NStrawPointsTemp[i]; k++) //loop over point
						{
					if (strawPointTemp_[i][k].nViews == 3 && !strawPointTemp_[i][k].used) {
						//						point_dx = fabs(strawPointTemp_[i][j].x-strawPointTemp_[i][k].x);
						point_dy = fabs(strawPointTemp_[i][j].y - strawPointTemp_[i][k].y);
						if (point_dy < 4.0) {
							viewdistance_delta = pointTemp.viewDistance - strawPointTemp_[i][k].viewDistance;
							if (viewdistance_delta > 1.0 && pointTemp.viewDistance > 3.0) {
								pointTemp.clonePoint(strawPointTemp_[i][k]);
								strawPointTemp_[i][k].used = 1;
							}
							if (viewdistance_delta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTemp_[i][k].used = 1;
							}
						}
					}
				}
				strawPointTempbis_[i][NStrawPointsTempBis[i]].clonePoint(pointTemp);
				NStrawPointsTempBis[i]++;
			}
			if (strawPointTemp_[i][j].nViews == 2) {
				strawPointTempbis_[i][NStrawPointsTempBis[i]].clonePoint(strawPointTemp_[i][j]);
				NStrawPointsTempBis[i]++;
			}
		}

		////////second SELECTION

		for (int j = 0; j < NStrawPointsTempBis[i]; j++) { //loop over point

			if (strawPointTempbis_[i][j].nViews == 4 && !strawPointTempbis_[i][j].used) {
				pointTemp.clonePoint(strawPointTempbis_[i][j]);
				strawPointTempbis_[i][j].used = 1;
				for (int k = j + 1; k < NStrawPointsTempBis[i]; k++) { //loop over point

					if (strawPointTempbis_[i][k].nViews == 4 && !strawPointTempbis_[i][k].used) {
						point_dx = fabs(strawPointTempbis_[i][j].x - strawPointTempbis_[i][k].x);
						//						point_dy = fabs(strawPointTempbis_[i][j].y-strawPointTempbis_[i][k].y);
						if (point_dx < 4.0) {
							viewdistance_delta = pointTemp.viewDistance - strawPointTempbis_[i][k].viewDistance;
							if (viewdistance_delta > 1.0 && pointTemp.viewDistance > 3.0) {
								//								pointTemp.setPoint(strawPointTempbis_[i][k].z,strawPointTempbis_[i][k].x,strawPointTempbis_[i][k].y,strawPointTempbis_[i][k].trailing,strawPointTempbis_[i][k].viewDistance,strawPointTempbis_[i][k].nViews,strawPointTempbis_[i][k].used);
								pointTemp.clonePoint(strawPointTempbis_[i][k]);
								strawPointTempbis_[i][k].used = 1;
							}
							if (viewdistance_delta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTempbis_[i][k].used = 1;
							}
						}
					}
				}
				strawPointFinal_[i][NStrawPointsFinal[i]].clonePoint(pointTemp);
				NStrawPointsFinal[i]++;
			}
			if (strawPointTempbis_[i][j].nViews == 3 && !strawPointTempbis_[i][j].used) {
				//				pointTemp.setPoint(strawPointTempbis_[i][j].z,strawPointTempbis_[i][j].x,strawPointTempbis_[i][j].y,strawPointTempbis_[i][j].trailing,strawPointTempbis_[i][j].viewDistance,strawPointTempbis_[i][j].nViews,strawPointTempbis_[i][j].used);
				pointTemp.clonePoint(strawPointTempbis_[i][j]);
				strawPointTempbis_[i][j].used = 1;
				for (int k = j + 1; k < NStrawPointsTempBis[i]; k++) { //loop over point
					if (strawPointTempbis_[i][k].nViews == 3 && !strawPointTempbis_[i][k].used) {
						point_dx = fabs(strawPointTempbis_[i][j].x - strawPointTempbis_[i][k].x);
						//						point_dy = fabs(strawPointTempbis_[i][j].y-strawPointTempbis_[i][k].y);
						if (point_dx < 4.0) {
							viewdistance_delta = pointTemp.viewDistance - strawPointTempbis_[i][k].viewDistance;
							if (viewdistance_delta > 1.0 && pointTemp.viewDistance > 3.0) {
								//								pointTemp.setPoint(strawPointTempbis_[i][k].z,strawPointTempbis_[i][k].x,strawPointTempbis_[i][k].y,strawPointTempbis_[i][k].trailing,strawPointTempbis_[i][k].viewDistance,strawPointTempbis_[i][k].nViews,strawPointTempbis_[i][k].used);
								pointTemp.clonePoint(strawPointTempbis_[i][k]);
								strawPointTempbis_[i][k].used = 1;
							}
							if (viewdistance_delta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTempbis_[i][k].used = 1;
							}
						}
					}
				}
				strawPointFinal_[i][NStrawPointsFinal[i]].clonePoint(pointTemp);
				NStrawPointsFinal[i]++;
			}
			if (strawPointTempbis_[i][j].nViews == 2) {
				strawPointFinal_[i][NStrawPointsFinal[i]].clonePoint(strawPointTempbis_[i][j]);
				NStrawPointsFinal[i]++;
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < NStrawPointsFinal[i]; j++) {
			if (NStrawPointsFinal[i] > 0)
				NChambersHit++;
		}
	}

	////////////////////// TRACK RECONSTRUCTION ////////////////////////////////////
//	LOG_INFO("Track Reconstruction - Hought ");

	float CDA = 0.;

	float MTemp;

	int NCam = 0;
	int QyHist = 0;
	int ChkCamera = 0;
	int ChkCam = 0;
	int NChkCam = 0;
	int AddCam1 = 0;
	int AddCam2 = 0;

	//verificare
	int NFirstTrack = 0;
	int NFirstTrackCentrali = 0;
	int TempNHitC = 0;
	int TempNHitL = 0;
	int Temp2NHitC = 0;

	int NAddHit1 = 0;
	int NAddHit2 = 0;
	int TempHit = 0;
	int TempCamera = 0;
	int ChkHit = 0; //chk per verificare se un hit o una camera trovata in una casella laterale è gia stata usata nella casella centrale

	float PointMY = 0.0;
	float PointQY = 0.0;

	int NTrack = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < NStrawPointsFinal[i]; j++) //loop over point
				{
			NCam = i;
			for (int a = 0; a < RANGEM; a++) {
				MTemp = ((float) a - ((float) RANGEM + 0.1) / 2) * passo;
				QY = strawPointFinal_[i][j].y - MTemp * (strawPointFinal_[i][j].z - ZMAGNET); //con 0.03 mrad di angolo max qy va da -1424 a 1637, quindi metto 1500 come limite (posso tagliare un poco prima di 0.03 per l'ultima camera)
				//qx = point[i][j].x - mtemp * point[i][j].z; //se si volesse fare un hough con x ma e' complicato con il magnete

				QyHist = QY * RANGEQ / LMAGNET + RANGEQ / 2; //provo a mettere 3000

				if (QyHist > 0 && QyHist < RANGEQ && (Hought[a][QyHist] >> 60) < 6) {
					Hought[a][QyHist] |= ((long long) (0x3 & NCam) << ((Hought[a][QyHist] >> 60) * 2 + 48)); //12 bits from 48 to 59 with the point chamber number (2 bit for chamber)
					Hought[a][QyHist] |= ((((long long) (0xFF & (j))) << ((Hought[a][QyHist] >> 60) * 8)) & 0XFFFFFFFFFFFF); //the first 48 bits with up to 6 point of 8 bits ( 255 )
					Hought[a][QyHist] += ((long long) 1 << 60); //the 4 most significant bits with the number of points
				}
			}
		}
	}

	NFirstTrack = 0;

	for (int a = 0; a < RANGEM; a++) {
		for (int b = 0; b < RANGEQ; b++) {
			if ((Hought[a][b] >> 60) > 1) //looking for bin with at least 2 points
					{
				ChkCam = 0;
				NChkCam = 4;
				AddCam1 = -1;
				AddCam2 = -1;
				ChkCamera = 0;

				//verificare
				NFirstTrack = 0;
				NFirstTrackCentrali = 0;
				TempNHitC = 0;
				TempNHitL = 0;
				NAddHit1 = 0;

				NAddHit1 = 0;
				NAddHit2 = 0;
				TempHit = 0;
				TempCamera = 0;
				ChkHit = 0; //chk per verificare se un hit o una camera trovata in una casella laterale è gia stata usata nella casella centrale

				PointMY = 0.0;
				PointQY = 0.0;

				for (int c = 0; c < (int) (Hought[a][b] >> 60); c++) // check if there are at least 2 hits belonging to different chambers
					ChkCam |= 1 << (0X3 & (Hought[a][b] >> (48 + 2 * c)));

				for (int d = 0; d < 4; d++)
					if ((0X1 & (ChkCam >> d)) == 0) {
						NChkCam--;

						if (AddCam1 == -1)
							AddCam1 = d;
						else
							AddCam2 = d;
					}

				if (NChkCam > 1) {
					for (int d = 0; d < (int) (Hought[a][b] >> 60); d++) {
						// si creano più tracklet con tutte le combinazioni di hit con camere diverse

						if (((int) pow(2, (int) (0X3 & (Hought[a][b] >> (48 + 2 * d)))) & ChkCamera) == 0) {
							// verifica se la camera è gia stata usata per questo tracklet: se non è stata
							// usata la aggiungo ad in tracklet esistenti (all'inizio ce ne e' uno vuoto)

							for (int j = 0; j < NFirstTrack + 1; j++) {
								strawFirstTempTrk_[j].hitc[TempNHitC] = (int) (0XFF & (Hought[a][b] >> (8 * d)));
								strawFirstTempTrk_[j].camerec[TempNHitC] = (int) (0X3 & (Hought[a][b] >> (48 + 2 * d)));

								ChkCamera |= (int) pow(2, strawFirstTempTrk_[NFirstTrack].camerec[TempNHitC]);

								strawFirstTempTrk_[j].ncentrali = TempNHitC + 1;
								strawFirstTempTrk_[j].nlaterali = 0;
							}
							TempNHitC++;
						}
						else //se è gia stata usata si crea un nuovo tracklet con la nuova camera al posto di quell'altra (se soddisfa dei requisiti)
						{
							Temp2NHitC = 0;
							NFirstTrack++;
							for (int j = 0; j < TempNHitC; j++) {

								if (strawFirstTempTrk_[NFirstTrack - 1].camerec[j] != (int) (0X3 & (Hought[a][b] >> (48 + 2 * d)))) {
									strawFirstTempTrk_[NFirstTrack].hitc[Temp2NHitC] = strawFirstTempTrk_[NFirstTrack - 1].hitc[j];
									strawFirstTempTrk_[NFirstTrack].camerec[Temp2NHitC] = strawFirstTempTrk_[NFirstTrack - 1].camerec[j];
								}
								else {
									if (strawPointFinal_[(int) (0X3 & (Hought[a][b] >> (48 + 2 * d)))][(int) (0XFF
											& (Hought[a][b] >> (8 * d)))].nViews
											> strawPointFinal_[strawFirstTempTrk_[NFirstTrack - 1].camerec[Temp2NHitC]][strawFirstTempTrk_[NFirstTrack
													- 1].hitc[Temp2NHitC]].nViews) {
										NFirstTrack--;
										strawFirstTempTrk_[NFirstTrack].hitc[Temp2NHitC] = (int) (0XFF & (Hought[a][b] >> (8 * d)));
										strawFirstTempTrk_[NFirstTrack].camerec[Temp2NHitC] = (int) (0X3 & (Hought[a][b] >> (48 + 2 * d)));
									}
									else if (strawPointFinal_[(int) (0X3 & (Hought[a][b] >> (48 + 2 * d)))][(int) (0XFF
											& (Hought[a][b] >> (8 * d)))].nViews
											== strawPointFinal_[strawFirstTempTrk_[NFirstTrack - 1].camerec[Temp2NHitC]][strawFirstTempTrk_[NFirstTrack
													- 1].hitc[Temp2NHitC]].nViews) {
										strawFirstTempTrk_[NFirstTrack].hitc[Temp2NHitC] = (int) (0XFF & (Hought[a][b] >> (8 * d)));
										strawFirstTempTrk_[NFirstTrack].camerec[Temp2NHitC] = (int) (0X3 & (Hought[a][b] >> (48 + 2 * d)));
									}
									else {
										NFirstTrack--;
									}
								}

								Temp2NHitC++;
							}
							strawFirstTempTrk_[NFirstTrack].ncentrali = Temp2NHitC;
							strawFirstTempTrk_[NFirstTrack].nlaterali = 0;
						}
					}
					NFirstTrackCentrali = NFirstTrack;

					if (TempNHitC > 1 && TempNHitC < 4) //se ci sono meno di 4 hit nella casella centrale, si cercano gli hit mancanti in quelle intorno
							{
						NAddHit1 = 0;
						NAddHit2 = 0;
						int *addhit1 = new int[50];
						int *addhit2 = new int[50];

						for (int h = -1; h < 2; h++) {
							for (int k = -1; k < 2; k++) {
								if (a + h > -1 && a + h < RANGEM && b + k > -1 && b + k < 200) {
									for (int l = 0; l < (Hought[a + h][b + k] >> 60); l++) {
										TempHit = (int) (0XFF & (Hought[a + h][b + k] >> (8 * l)));
										TempCamera = (int) (0X3 & (Hought[a + h][b + k] >> (48 + 2 * l)));
										ChkHit = 0;

										//terza e nuova soluzione

										if (TempCamera == AddCam1) {
											//look if there already are the hit in addhit1[]

											for (int d = 0; d < NAddHit1; d++) {
												if (addhit1[d] == TempHit) {
													ChkHit = 1;
												}
											}

											if (ChkHit == 0) {
												addhit1[NAddHit1] = TempHit;
												NAddHit1++;
											}
										}
										if (TempCamera == AddCam2) {
											for (int d = 0; d < NAddHit2; d++) {
												if (addhit2[d] == TempHit) {
													ChkHit = 1;
												}
											}

											if (ChkHit == 0) {
												addhit2[NAddHit2] = TempHit;
												NAddHit2++;
											}
										}
									}
								}
							}
						}
						if (NAddHit1 > 0) {
							for (int j = 0; j < NFirstTrackCentrali + 1; j++) {
								strawFirstTempTrk_[j].hitl[TempNHitL] = addhit1[0];
								strawFirstTempTrk_[j].camerel[TempNHitL] = AddCam1;
								strawFirstTempTrk_[j].nlaterali = TempNHitL + 1;
							}

							for (int d = 1; d < NAddHit1; d++) {
								for (int j = 0; j < NFirstTrackCentrali + 1; j++) {
									NFirstTrack++;
									strawFirstTempTrk_[NFirstTrack].copyTrack(strawFirstTempTrk_[j]);
									strawFirstTempTrk_[NFirstTrack].hitl[TempNHitL] = addhit1[d];
								}
							}
							TempNHitL++;
						}
						NFirstTrackCentrali = NFirstTrack; //sono aumentati in seguito all'addhit1

						if (NAddHit2 > 0) {
							for (int j = 0; j < NFirstTrackCentrali + 1; j++) {
								strawFirstTempTrk_[j].hitl[TempNHitL] = addhit2[0];
								strawFirstTempTrk_[j].camerel[TempNHitL] = AddCam2;
								strawFirstTempTrk_[j].nlaterali = TempNHitL + 1;
							}

							for (int d = 1; d < NAddHit2; d++) {
								for (int j = 0; j < NFirstTrackCentrali + 1; j++) {
									NFirstTrack++;
									strawFirstTempTrk_[NFirstTrack].copyTrack(strawFirstTempTrk_[j]);
									strawFirstTempTrk_[NFirstTrack].hitl[TempNHitL] = addhit2[d];
								}
							}
							TempNHitL++;
						}

						delete[] addhit1;
						delete[] addhit2;
					}

					//now I have all the tracklet find in the bin, I have to select only the real one

					PointMY = ((float) a - ((float) RANGEM + 0.1) / 2) * passo;
					PointQY = b * LMAGNET / RANGEQ - LMAGNET / 2;

					//	zvertex = 197645 - (PointQY / PointMY); //oppure shiftati perchè zvertex = 197645-((PointQY+4.762)/(PointMY+0.0001989));

					for (int j = 0; j < NFirstTrack + 1; j++) { //parte X

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
						double trailingtemp = 0.0;

						if (strawFirstTempTrk_[j].ncentrali + strawFirstTempTrk_[j].nlaterali > 2) {
							for (int z = 0; z < TempNHitC; z++) {
								trailingtemp += strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].trailing;

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
							for (int z = 0; z < TempNHitL; z++) {
								trailingtemp += strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].trailing;

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
								}
								else {
									strawFirstTempTrk_[j].m1x = (x0 - q01) / z0;
								}
							}
							else if (z2 == 0 || z3 == 0) {
								q01 = x0 - z0 * (x1 - x0) / (z1 - z0);
								q23 = q01;

								strawFirstTempTrk_[j].m1x = (x1 - x0) / (z1 - z0);
								if (z2 == 0) {
									strawFirstTempTrk_[j].m2x = (x3 - q23) / z3;
								}
								else {
									strawFirstTempTrk_[j].m2x = (x2 - q23) / z2;
								}
							}
							else {
								q01 = x0 - z0 * (x1 - x0) / (z1 - z0);
								q23 = x2 - z2 * (x3 - x2) / (z3 - z2);
								strawFirstTempTrk_[j].m1x = (x1 - x0) / (z1 - z0);
								strawFirstTempTrk_[j].m2x = (x3 - x2) / (z3 - z2);
							}

							dqx = q23 - q01;

							strawFirstTempTrk_[j].q1x = q01;
							strawFirstTempTrk_[j].q2x = q23;
							strawFirstTempTrk_[j].pz = 270.0 / (fabs(strawFirstTempTrk_[j].m2x - strawFirstTempTrk_[j].m1x));

							strawFirstTempTrk_[j].my = PointMY;
							strawFirstTempTrk_[j].qy = PointQY;
							strawFirstTempTrk_[j].trailing = trailingtemp / (TempNHitC + TempNHitL);

							QTrack.setPoint(0.0, strawFirstTempTrk_[j].q1x, strawFirstTempTrk_[j].qy, 0.0, 0.0, 0, 0);
							MTrack.setPoint(1.0, strawFirstTempTrk_[j].m1x, strawFirstTempTrk_[j].my, 0.0, 0.0, 0, 0);

							cdaVertex(QBeam_, QTrack, MBeam_, MTrack, CDA, Vertex);

							strawFirstTempTrk_[j].zvertex = Vertex.z;
							strawFirstTempTrk_[j].cda = CDA;

							//pz>3 Gev <100Gev, my e mx1 < 0.020
							if (strawFirstTempTrk_[j].pz > 3000 && strawFirstTempTrk_[j].pz < 100000 && strawFirstTempTrk_[j].my < 0.020
									&& strawFirstTempTrk_[j].m1x < 0.020) {
								strawTempTrk_[NTrack].copyTrack(strawFirstTempTrk_[j]);
								NTrack++;
							}
						}
					}

				}

			}
		}
	}

	NTracletCondivisi = 0;
	NTrackIntermedie = 0;

	//prima uso le tracce con 4 hit
	for (int e = 0; e < NTrack; e++) {
		TrackIntermedieTemp_my = 0.0;
		TrackIntermedieTemp_qy = 0.0;
		TrackIntermedieTemp_m1x = 0.0;
		TrackIntermedieTemp_q1x = 0.0;
		TrackIntermedieTemp_m2x = 0.0;
		TrackIntermedieTemp_q2x = 0.0;

		//select the best temporary track between the similar track with 4 point
		if (strawTempTrk_[e].usato == 0 && strawTempTrk_[e].ncentrali + strawTempTrk_[e].nlaterali == 4) {
			TrackIntermedieTemp.copyTrack(strawTempTrk_[e]);
			if ((strawTempTrk_[e].q2x - strawTempTrk_[e].q1x) < -13 || (strawTempTrk_[e].q2x - strawTempTrk_[e].q1x) > 0)
				for (int f = e; f < NTrack; f++) {
					TempCondivise = 0;
					if (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali == 4) {
						for (int g = 0; g < TrackIntermedieTemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
							for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

								if (TrackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitc[h]
										&& TrackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerec[h]) {
									TempCondivise++;
								}
							}
							for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
								if (TrackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitl[h]
										&& TrackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerel[h]) {
									TempCondivise++;
								}
							}
						}

						for (int g = 0; g < TrackIntermedieTemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
							for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

								if (TrackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitc[h]
										&& TrackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerec[h]) {
									TempCondivise++;
								}
							}
							for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

								if (TrackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitl[h]
										&& TrackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerel[h]) {
									TempCondivise++;
								}
							}
						}

						if (TempCondivise > 1
								&& (((strawTempTrk_[f].q2x - strawTempTrk_[f].q1x) > -13
										&& (strawTempTrk_[f].q2x - strawTempTrk_[f].q1x) < 0)
										|| strawTempTrk_[f].ncentrali > TrackIntermedieTemp.ncentrali
										|| (strawTempTrk_[f].ncentrali == TrackIntermedieTemp.ncentrali
												&& fabs(strawTempTrk_[f].q2x - strawTempTrk_[f].q1x + 3)
														< fabs(TrackIntermedieTemp.q2x - TrackIntermedieTemp.q1x + 3)))) {
							//  End of conditional logic

							TrackIntermedieTemp.copyTrack(strawTempTrk_[f]);
						}
					}
				}

			NTracletCondivisi = 0;

			for (int f = 0; f < NTrack; f++) { //the tracks with the same point will be combined

				TempCondivise = 0;
				for (int g = 0; g < TrackIntermedieTemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (TrackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitc[h]
								&& TrackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerec[h]) {
							TempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (TrackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitl[h]
								&& TrackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerel[h]) {
							TempCondivise++;
						}
					}
				}

				for (int g = 0; g < TrackIntermedieTemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (TrackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitc[h]
								&& TrackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerec[h]) {
							TempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (TrackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitl[h]
								&& TrackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerel[h]) {
							TempCondivise++;
						}
					}
				}

				if (TempCondivise == (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali)) {
					NTracletCondivisi++;
					TrackIntermedieTemp_my += strawTempTrk_[f].my;
					TrackIntermedieTemp_qy += strawTempTrk_[f].qy;
					TrackIntermedieTemp_m1x += strawTempTrk_[f].m1x;
					TrackIntermedieTemp_q1x += strawTempTrk_[f].q1x;
					TrackIntermedieTemp_m2x += strawTempTrk_[f].m2x;
					TrackIntermedieTemp_q2x += strawTempTrk_[f].q2x;

					strawTempTrk_[f].usato = 1;
				}
				else {
					if (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali == 4 && TempCondivise > 1) {
						strawTempTrk_[f].usato = 1;
					}
					else if (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali == 3 && TempCondivise > 0) {
						strawTempTrk_[f].usato = 1;
					}
				}
			}

			TrackIntermedieTemp.my = TrackIntermedieTemp_my / NTracletCondivisi;
			TrackIntermedieTemp.qy = TrackIntermedieTemp_qy / NTracletCondivisi;
			TrackIntermedieTemp.m1x = TrackIntermedieTemp_m1x / NTracletCondivisi;
			TrackIntermedieTemp.q1x = TrackIntermedieTemp_q1x / NTracletCondivisi;
			TrackIntermedieTemp.m2x = TrackIntermedieTemp_m2x / NTracletCondivisi;
			TrackIntermedieTemp.q2x = TrackIntermedieTemp_q2x / NTracletCondivisi;

			strawTrkIntermedie_[NTrackIntermedie].copyTrack(TrackIntermedieTemp);
			strawTrkIntermedie_[NTrackIntermedie].pz = 270 / (fabs(TrackIntermedieTemp.m2x - TrackIntermedieTemp.m1x));

			QTrack.setPoint(0.0, TrackIntermedieTemp.q1x, TrackIntermedieTemp.qy, 0.0, 0.0, 0, 0);
			MTrack.setPoint(1.0, TrackIntermedieTemp.m1x, TrackIntermedieTemp.my, 0.0, 0.0, 0, 0);

			cdaVertex(QBeam_, QTrack, MBeam_, MTrack, CDA, Vertex);

			strawTrkIntermedie_[NTrackIntermedie].zvertex = Vertex.z;
			strawTrkIntermedie_[NTrackIntermedie].cda = CDA;
			strawTrkIntermedie_[NTrackIntermedie].ncondivisi = 0;
			strawTrkIntermedie_[NTrackIntermedie].ncamcondivise = 0;
			strawTrkIntermedie_[NTrackIntermedie].usato = 0;

			NTrackIntermedie++;

		}
	}

	//the same with tracks with only 3 points
	for (int e = 0; e < NTrack; e++) {

		TrackIntermedieTemp_my = 0.0;
		TrackIntermedieTemp_qy = 0.0;
		TrackIntermedieTemp_m1x = 0.0;
		TrackIntermedieTemp_q1x = 0.0;
		TrackIntermedieTemp_m2x = 0.0;
		TrackIntermedieTemp_q2x = 0.0;

		if (strawTempTrk_[e].usato == 0 && strawTempTrk_[e].ncentrali + strawTempTrk_[e].nlaterali == 3) {
			TrackIntermedieTemp.copyTrack(strawTempTrk_[e]);

			for (int f = 0; f < NTrack; f++) {
				TempCondivise = 0;
				for (int g = 0; g < TrackIntermedieTemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (TrackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitc[h]
								&& TrackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerec[h]) {
							TempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (TrackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitl[h]
								&& TrackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerel[h]) {
							TempCondivise++;
						}
					}
				}

				for (int g = 0; g < TrackIntermedieTemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (TrackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitc[h]
								&& TrackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerec[h]) {
							TempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (TrackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitl[h]
								&& TrackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerel[h]) {
							TempCondivise++;
						}
					}
				}

				if (TempCondivise > 1
						&& (strawTempTrk_[f].ncentrali > TrackIntermedieTemp.ncentrali
								|| (strawTempTrk_[f].cda < TrackIntermedieTemp.cda
										&& strawTempTrk_[f].ncentrali == TrackIntermedieTemp.ncentrali))) {
					// End of conditional logic
					TrackIntermedieTemp.copyTrack(strawTempTrk_[f]);
				}
			}

			NTracletCondivisi = 0;

			for (int f = 0; f < NTrack; f++) {
				TempCondivise = 0;
				for (int g = 0; g < TrackIntermedieTemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (TrackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitc[h]
								&& TrackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerec[h]) {
							TempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (TrackIntermedieTemp.hitc[g] == strawTempTrk_[f].hitl[h]
								&& TrackIntermedieTemp.camerec[g] == strawTempTrk_[f].camerel[h]) {
							TempCondivise++;
						}
					}
				}

				for (int g = 0; g < TrackIntermedieTemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						if (TrackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitc[h]
								&& TrackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerec[h]) {
							TempCondivise++;
						}
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {
						if (TrackIntermedieTemp.hitl[g] == strawTempTrk_[f].hitl[h]
								&& TrackIntermedieTemp.camerel[g] == strawTempTrk_[f].camerel[h]) {
							TempCondivise++;
						}
					}
				}

				if (TempCondivise == (TrackIntermedieTemp.ncentrali + TrackIntermedieTemp.nlaterali)) {
					NTracletCondivisi++;
					TrackIntermedieTemp_my += strawTempTrk_[f].my;
					TrackIntermedieTemp_qy += strawTempTrk_[f].qy;
					TrackIntermedieTemp_m1x += strawTempTrk_[f].m1x;
					TrackIntermedieTemp_q1x += strawTempTrk_[f].q1x;
					TrackIntermedieTemp_m2x += strawTempTrk_[f].m2x;
					TrackIntermedieTemp_q2x += strawTempTrk_[f].q2x;

					strawTempTrk_[f].usato = 1;
				}
				else {
					if (TrackIntermedieTemp.ncentrali == 3 && strawTempTrk_[f].ncentrali == 2 && TempCondivise > 0) {
						strawTempTrk_[f].usato = 1;
					}
					else if (TempCondivise > 1) {
						strawTempTrk_[f].usato = 1;
					}
				}
			}

			TrackIntermedieTemp.my = TrackIntermedieTemp_my / NTracletCondivisi;
			TrackIntermedieTemp.qy = TrackIntermedieTemp_qy / NTracletCondivisi;
			TrackIntermedieTemp.m1x = TrackIntermedieTemp_m1x / NTracletCondivisi;
			TrackIntermedieTemp.q1x = TrackIntermedieTemp_q1x / NTracletCondivisi;
			TrackIntermedieTemp.m2x = TrackIntermedieTemp_m2x / NTracletCondivisi;
			TrackIntermedieTemp.q2x = TrackIntermedieTemp_q2x / NTracletCondivisi;

			strawTrkIntermedie_[NTrackIntermedie].copyTrack(TrackIntermedieTemp);
			strawTrkIntermedie_[NTrackIntermedie].pz = 270 / (fabs(TrackIntermedieTemp.m2x - TrackIntermedieTemp.m1x));

			QTrack.setPoint(0.0, TrackIntermedieTemp.q1x, TrackIntermedieTemp.qy, 0.0, 0.0, 0, 0);
			MTrack.setPoint(1.0, TrackIntermedieTemp.m1x, TrackIntermedieTemp.my, 0.0, 0.0, 0, 0);

			cdaVertex(QBeam_, QTrack, MBeam_, MTrack, CDA, Vertex);

			strawTrkIntermedie_[NTrackIntermedie].zvertex = Vertex.z;
			strawTrkIntermedie_[NTrackIntermedie].cda = CDA;
			strawTrkIntermedie_[NTrackIntermedie].ncondivisi = 0;
			strawTrkIntermedie_[NTrackIntermedie].ncamcondivise = 0;
			strawTrkIntermedie_[NTrackIntermedie].usato = 0;

			NTrackIntermedie++;

		}
	}

	//casi a 3 tracce
	Point MTrack1;
	Point QTrack1;
	Point MTrack2;
	Point QTrack2;

	for (int e = 0; e < NTrackIntermedie; e++) {
		// We zero FlagL1Limit as needed rather than zeroing the entire array
		FlagL1Limit[e] = 0;

		if (e < 5) {
			L1Info->setL1StrawTrack_P(e, strawTrkIntermedie_[e].pz);
			L1Info->setL1StrawTrack_Vz(e, strawTrkIntermedie_[e].zvertex);
		}
		if (strawTrkIntermedie_[e].zvertex > -100000 && strawTrkIntermedie_[e].zvertex < 180000 && strawTrkIntermedie_[e].cda < 200
				&& strawTrkIntermedie_[e].pz < 50000) {
			FlagL1Limit[e]++;
		}

		QTrack1.setPoint(0.0, strawTrkIntermedie_[e].q1x, strawTrkIntermedie_[e].qy, 0.0, 0.0, 0, 0);
		MTrack1.setPoint(1.0, strawTrkIntermedie_[e].m1x, strawTrkIntermedie_[e].my, 0.0, 0.0, 0, 0);

		for (int f = e + 1; f < NTrackIntermedie; f++) {
			QTrack2.setPoint(0.0, strawTrkIntermedie_[f].q1x, strawTrkIntermedie_[f].qy, 0.0, 0.0, 0, 0);
			MTrack2.setPoint(1.0, strawTrkIntermedie_[f].m1x, strawTrkIntermedie_[f].my, 0.0, 0.0, 0, 0);

			cdaVertex(QTrack1, QTrack2, MTrack1, MTrack2, CDA, Vertex);
			if (CDA < 30) {
				FlagL1Three[e]++;
				FlagL1Three[f]++;
			}
		}

		if (strawTrkIntermedie_[e].m1x - strawTrkIntermedie_[e].m2x < 0) {
			FlagL1Exotic = 1;
		}
		if (FlagL1Limit[e] > 0 && FlagL1Three[e] == 0) {
			FlagL1Pnn = 1;
		}
		if (FlagL1Limit[e] > 0 && FlagL1Three[e] > 0) {
			FlagL1TreTracks = 1;
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

	L1Info->setL1StrawNTracks(NTrackIntermedie);
	L1Info->setL1StrawProcessed();
	FlagL1 = ((FlagL1Exotic & 0x1) << 1) | (FlagL1Pnn & 0x1);
	return FlagL1;
}

float StrawAlgo::posTubNew(int chamber, int view, int plane, int jstraw) {
	// Straw spacing = 17.6
	return 17.6 * jstraw + XOffset_[view][plane] + XOffCh_[chamber][view];
}

int StrawAlgo::strawAcceptance(int N, double* Coordinate, int Zone) {

	float ViewSize = 2100.0;
	float StrawSpacing = 17.6;
	float StrawInnerRadius = 4.875;
	float CopperThickness = 0.00005;
	float MylarThickness = 0.036;
	float GoldThickness = 0.00002;
	double StrawRadius = StrawInnerRadius + 2 * CopperThickness + MylarThickness + GoldThickness;
	double StrawDiameter = 2 * StrawRadius;

	double ViewPlaneTransverseSize = (120 - 1) * StrawSpacing + StrawDiameter;

	// View definition
	double A[4] = { InvSq2_, InvSq2_, 1, 0 };
	double B[4] = { InvSq2_, -InvSq2_, 0, 1 };
	double C[4] = { InvSq2_, InvSq2_, 0, 1 };
	double D[4] = { -InvSq2_, InvSq2_, 1, 0 };
	int Viewflag[4] = { 0, 0, 0, 0 };
	for (int jView = 0; jView < 4; jView++) {
		double PosView = A[jView] * Coordinate[0] + B[jView] * Coordinate[1];
		double PosAlongStraw = C[jView] * Coordinate[0] + D[jView] * Coordinate[1];
		if (((PosView > HoleChamberMax_[N][jView] && PosView < 0.5 * ViewPlaneTransverseSize)
				|| (PosView < HoleChamberMin_[N][jView] && PosView > -0.5 * ViewPlaneTransverseSize))
				&& fabs(PosAlongStraw) < 0.5 * ViewSize) {
			Viewflag[jView] = 1;
		}
	}
	int Vu = Viewflag[0];
	int Vv = Viewflag[1];
	int Vx = Viewflag[2];
	int Vy = Viewflag[3];

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
	if (Vx + Vy + Vu + Vv == 2)
		return 1;
	return 0;
}

void StrawAlgo::cdaVertex(const Point& QBeam, Point& QTrack, const Point& MBeam, Point& MTrack, float& CDA, Point& Vertex) {

	Point R12;
	float T1, T2, AA, BB, CC, DD, EE, Det;
	Point Q1, Q2;

	R12.z = QBeam.z - QTrack.z;
	R12.x = QBeam.x - QTrack.x;
	R12.y = QBeam.y - QTrack.y;

	AA = MBeam.x * MBeam.x + MBeam.y * MBeam.y + MBeam.z * MBeam.z;
	BB = MTrack.x * MTrack.x + MTrack.y * MTrack.y + MTrack.z * MTrack.z;
	CC = MBeam.x * MTrack.x + MBeam.y * MTrack.y + MBeam.z * MTrack.z;
	DD = R12.x * MBeam.x + R12.y * MBeam.y + R12.z * MBeam.z;
	EE = R12.x * MTrack.x + R12.y * MTrack.y + R12.z * MTrack.z;
	Det = CC * CC - AA * BB;

	T1 = (BB * DD - CC * EE) / Det;
	T2 = (CC * DD - AA * EE) / Det;

	Q1.z = QBeam.z + T1 * MBeam.z;
	Q1.x = QBeam.x + T1 * MBeam.x;
	Q1.y = QBeam.y + T1 * MBeam.y;
	Q2.z = QTrack.z + T2 * MTrack.z;
	Q2.x = QTrack.x + T2 * MTrack.x;
	Q2.y = QTrack.y + T2 * MTrack.y;

	Vertex.setPoint(ZMAGNET + (Q1.z + Q2.z) / 2, (Q1.x + Q2.x) / 2, (Q1.y + Q2.y) / 2, 0.0, 0.0, 0, 0);
	R12.setPoint(Q1.z - Q2.z, Q1.x - Q2.x, Q1.y - Q2.y, 0.0, 0.0, 0, 0);

	CDA = sqrt(R12.x * R12.x + R12.y * R12.y + R12.z * R12.z);

}

void StrawAlgo::writeData(L1StrawAlgo* AlgoPacket, uint L0MaskID, L1InfoToStorage* L1Info) {

	if (AlgoID_ != AlgoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID written within the packet!");
	AlgoPacket->algoID = AlgoID_;
	AlgoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[L0MaskID];
	AlgoPacket->qualityFlags = (L1Info->isL1StrawProcessed() << 6) | (L1Info->isL1StrawEmptyPacket() << 4)
			| (L1Info->isL1StrawBadData() << 2) | ((uint) L1Info->getL1StrawTrgWrd());
	for (uint iTrk = 0; iTrk != 5; iTrk++) {
//		LOG_INFO("track index " << iTrk << " momentum " << l1Info->getL1StrawTrack_P(iTrk));
//		LOG_INFO("track index " << iTrk << " vertex " << l1Info->getL1StrawTrack_Vz(iTrk));
		AlgoPacket->l1Data[iTrk] = ((uint) L1Info->getL1StrawTrack_P(iTrk) << 16) | (uint) L1Info->getL1StrawTrack_Vz(iTrk);
	}
	AlgoPacket->l1Data[5] = L1Info->getL1StrawNTracks();
	AlgoPacket->numberOfWords = (sizeof(L1StrawAlgo) / 4.);
//	LOG_INFO("l0MaskID " << l0MaskID);
//	LOG_INFO("algoID " << (uint)algoPacket->algoID);
//	LOG_INFO("quality Flags " << (uint)algoPacket->qualityFlags);
//	LOG_INFO("online TW " << (uint)algoPacket->onlineTimeWindow);
//	LOG_INFO("Data Words " << algoPacket->l1Data[0] << " " << algoPacket->l1Data[1]);

}

} /* namespace na62 */

