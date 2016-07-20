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
#define lmagnete 3000
#define zmagnete 197645

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
//int StrawAlgo::cutcluster = 20; //10

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
double StrawAlgo::t0_main_shift = 5991.01;

double StrawAlgo::cutlowleading = 0.0; //-10
double StrawAlgo::cuthighleading = 175.0; //165
double StrawAlgo::cutlowtrailing = 55.0; //-50
double StrawAlgo::cuthightrailing = 250.0;
int StrawAlgo::cutcluster = 6;
double StrawAlgo::m1leadtrail = 1.35;
double StrawAlgo::q1leadtrail = -50.0; //-50
double StrawAlgo::m2leadtrail = 4.8;
double StrawAlgo::q2leadtrail = -1150.0; //-1200
double StrawAlgo::hit3low = 8.6;
double StrawAlgo::hit3high = 9.0;
double StrawAlgo::hit2low = 2.9;
double StrawAlgo::hit2high = 5.8;

Point StrawAlgo::strawPointTemp_[4][5000];
Point StrawAlgo::strawPointTempbis_[4][2000];
Point StrawAlgo::strawPointFinal_[4][2000];
Cluster StrawAlgo::strawCluster_[4][4][500];
Straw StrawAlgo::strawPrecluster_[4][4][2][500];
Track StrawAlgo::strawFirstTempTrk_[3000];
Track StrawAlgo::strawTempTrk_[4000];
Track StrawAlgo::strawTrkIntermedie_[1000];

std::mutex StrawAlgo::strawMutex;

float StrawAlgo::fChamberZPosition[4] = { (0.5 * (183311.1 + 183704.9)), (0.5 * (193864.1 + 194262.9) + 2.5), (0.5 * (204262.1 + 204655.9)),
		(0.5 * (218688.1 + 219081.9)) };

Point StrawAlgo::qbeam;
Point StrawAlgo::mbeam;

void StrawAlgo::initialize(uint i, l1Straw &l1StrawStruct) {

	AlgoID_ = l1StrawStruct.configParams.l1TrigMaskID;
	AlgoLogic_[i] = l1StrawStruct.configParams.l1TrigLogic;
	AlgoRefTimeSourceID_[i] = l1StrawStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	AlgoOnlineTimeWindow_[i] = l1StrawStruct.configParams.l1TrigOnlineTimeWindow;

	qbeam.setPoint(0.0, 114.0, 0.0, 0.0, 0.0, 0, 0);
	mbeam.setPoint(1.0, 0.0012, 0.0, 0.0, 0.0, 0, 0);

	//	LOG_INFO("Straw mask: " << i << " logic " << AlgoLogic_[i] << " refTimeSourceID " << AlgoRefTimeSourceID_[i] << " online time window " << AlgoOnlineTimeWindow_[i]);
}

uint_fast8_t StrawAlgo::processStrawTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info) {

	std::lock_guard<std::mutex> lock(strawMutex);

//	printf("\nnevent = %d, raw timestamp = %u, fine timestamp = %u, ClockPeriod %lf \n", decoder.getDecodedEvent()->getEventNumber(),
//			decoder.getDecodedEvent()->getTimestamp(), decoder.getDecodedEvent()->getFinetime(), CLOCK_PERIOD);

	struct timeval time[30];
	//gettimeofday(&time[0], 0);
	//LOG_INFO( "Initial Time - Start " << time[0].tv_sec << " " << time[0].tv_usec );

	using namespace l0;

	int flag_l1 = 0;
	int flag_l1_exotic = 0;
	int flag_l1_limit[1000] = { 0 };
	int flag_l1_three[1000] = { 0 };
	int flag_l1_tretracks = 0;

	double sq2 = sqrt(2);
	double invsq2 = 1. / sq2;

	uint nEdges_tot = 0;
	uint nHits = 0;
	bool tl_flag = 0;
	bool skip_flag = 0;
	int nChambersHit = 0;

	int ntotalhit = 0; //just for debug
	int ntotalviewcluster = 0; //just for debug
	int ntotalPreclusters = 0; //just for debug

	int ntracletcondivisi;
	int ntrkintermedie;
	int ntrkfinali;

	Track trkintermedietemp;
	float trkintermedietemp_my;
	float trkintermedietemp_qy;
	float trkintermedietemp_m1x;
	float trkintermedietemp_q1x;
	float trkintermedietemp_m2x;
	float trkintermedietemp_q2x;

	int tempcondivise;

	int chamberID = -1;
	int viewID = -1;
	int halfviewID = -1;
	int planeID = -1;
	int strawID = -1;
	double leading = -100000.;
	double trailing = -100000.;
	float position = 0.0;
	float wireDistance = -100.0;

	int nStrawPointsTemp[4];
	int nStrawPointsTempbis[4];
	int nStrawPointsFinal[4];
	int nStrawClusters[4][4];
	int nStrawPreclusters[4][4][2];

	for (int i = 0; i != 4; ++i) {
		nStrawPointsTemp[i] = 0;
		nStrawPointsTempbis[i] = 0;
		nStrawPointsFinal[i] = 0;
		for (int j = 0; j != 4; ++j) {
			nStrawClusters[i][j] = 0;
			for (int h = 0; h != 2; ++h) {
				nStrawPreclusters[i][j][h] = 0;
			}
		}
	}

	STRAWChannelID strawChannel_;
	DigiManager strawDigiMan_;

	Point qtrack;
	Point mtrack;
	Point vertex;
	long long hought[rangem][rangeq];

	for (int a = 0; a < rangem; a++)
		for (int b = 0; b < rangeq; b++)
			hought[a][b] = 0;

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
			LOG_ERROR("STRAWAlgo: This looks like a Bad Packet!!!! ");
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

		ntotalhit += numberOfEdgesOfCurrentBoard;

		//gettimeofday(&time[4], 0);
		//		LOG_INFO( "Access Packets - Stop " << time[4].tv_sec << " " << time[4].tv_usec );
		//		LOG_INFO( "Access SRB Packets " << ((time[4].tv_sec - time[3].tv_sec)*1e6 + time[4].tv_usec) - time[3].tv_usec );
		//LOG_INFO( "nhit " << numberOfEdgesOfCurrentBoard);

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {

			//LOG_INFO("srb_add = " << (uint)srbAddr[iEdge] << " straw_add = " << (uint)strawAddr[iEdge] << " edge(lead)= " << edgeIsLeading[iEdge]);
			//	gettimeofday(&time[5], 0);
			//	LOG_INFO( "Read Config File and Assign ChannelID - Start " << time[5].tv_sec << " " << time[5].tv_usec );

			tl_flag = 0;
			strawChannel_.resetChannelID();

			const int roChID = 256 * srbAddr[iEdge] + strawAddr[iEdge];
			strawChannel_.decodeChannelID(StrawGeo_[roChID]);

			chamberID = strawChannel_.getChamberID();
			viewID = strawChannel_.getViewID();
			halfviewID = strawChannel_.getHalfViewID();
			planeID = strawChannel_.getPlaneID();
			strawID = strawChannel_.getStrawID();
			leading = -100000.;
			trailing = -100000.;
			position = 0.0;
			wireDistance = -100.0;

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
				leading = (double) edgeTime[iEdge] + (double) StationT0_ - (double) ROMezzaninesT0_[srbAddr[iEdge] * 16 + coverAddr]
						+ (double) INVISIBLE_SHIFT - (((double) decoder.getDecodedEvent()->getFinetime() * CLOCK_PERIOD) / 256 + 0.5);
				//			if(edgeIsLeading[iEdge]) leading = (double)edgeTime[iEdge];
//				printf("time components %lf %lf %lf %lf %lf\n", edgeTime[iEdge], StationT0_,
//						ROMezzaninesT0_[srbAddr[iEdge] * 16 + coverAddr], (double) INVISIBLE_SHIFT,
//						(((double) decoder.getDecodedEvent()->getFinetime() * CLOCK_PERIOD) / 256 + 0.5));
			}
			if (!edgeIsLeading[iEdge]) {
				trailing = (double) edgeTime[iEdge] + (double) StationT0_ - (double) ROMezzaninesT0_[srbAddr[iEdge] * 16 + coverAddr]
						+ (double) INVISIBLE_SHIFT - (((double) decoder.getDecodedEvent()->getFinetime() * CLOCK_PERIOD) / 256 + 0.5);
				//			if(!edgeIsLeading[iEdge]) trailing = (double)edgeTime[iEdge];
//				printf("time components %lf %lf %lf %lf %lf\n", edgeTime[iEdge], StationT0_,
//						ROMezzaninesT0_[srbAddr[iEdge] * 16 + coverAddr], (double) INVISIBLE_SHIFT,
//						(((double) decoder.getDecodedEvent()->getFinetime() * CLOCK_PERIOD) / 256 + 0.5));
			}
			/*			printf("number hit %d hit: %d %d %d %d %d %lf %lf %d %d %d\n",
			 iEdge, chamberID, viewID, halfviewID, planeID, strawID,
			 leading, trailing, edgeIsLeading[iEdge], srbAddr[iEdge],
			 strawAddr[iEdge]);*/
			//gettimeofday(&time[6], 0);
			//LOG_INFO( "Read Config File and Assign ChannelID - Stop " << time[6].tv_sec << " " << time[6].tv_usec );
			//LOG_INFO( "Read Conf file and Assign ChannelID and time" << ((time[6].tv_sec - time[5].tv_sec)*1e6 + time[6].tv_usec) - time[5].tv_usec );
			//gettimeofday(&time[7], 0);
			//	LOG_INFO( "Access Straw Map - Start " << time[7].tv_sec << " " << time[7].tv_usec );
			position = posTubNew(chamberID, viewID, halfviewID * 2 + planeID, strawID);

			//gettimeofday(&time[8], 0);
			//	LOG_INFO( "Access Straw Map - Stop " << time[8].tv_sec << " " << time[8].tv_usec );
			//	LOG_INFO( "Access Straw Map and give position of the tube" << ((time[8].tv_sec - time[7].tv_sec)*1e6 + time[8].tv_usec) - time[7].tv_usec );

			//////////////PRECLUSTERING, first leading and last trailing//////////////////////////////
			//			printf ("\n PRECLUSTERING <n nhit of (%d,%d,%d) are %d \n",chamberID,viewID,halfviewID,nStrawPreclusters[chamberID][viewID][halfviewID]);

			//gettimeofday(&time[9], 0);
			//	LOG_INFO( "Preclustering - Start " << time[9].tv_sec << " " << time[9].tv_usec );
			//LOG_INFO( "Preparazione degli hit " << ((time[9].tv_sec - time[0].tv_sec)*1e6 + time[9].tv_usec) - time[0].tv_usec );

			for (int j = 0; j != nStrawPreclusters[chamberID][viewID][halfviewID]; ++j) {
				//				printf ("inside loop: j=%d \n",j);
				if ((strawPrecluster_[chamberID][viewID][halfviewID][j].plane == planeID)
						&& (strawPrecluster_[chamberID][viewID][halfviewID][j].tube == strawID)) {
					tl_flag = 1;
					if ((edgeIsLeading[iEdge])
							&& (leading < strawPrecluster_[chamberID][viewID][halfviewID][j].leading
									|| strawPrecluster_[chamberID][viewID][halfviewID][j].leading <= -10)
							&& (leading > -10 && leading < 300)) {
						if (leading < 1)
							wireDistance = strawDigiMan_.rTDependenceData(1.0);
						else
							wireDistance = strawDigiMan_.rTDependenceData(leading / 1000);

						strawPrecluster_[chamberID][viewID][halfviewID][j].leading = leading;
						strawPrecluster_[chamberID][viewID][halfviewID][j].wiredistance = wireDistance;
						//						printf ("Straw completed\n");
					} else if ((!edgeIsLeading[iEdge])
							&& (strawPrecluster_[chamberID][viewID][halfviewID][j].trailing < -100
									|| strawPrecluster_[chamberID][viewID][halfviewID][j].trailing < trailing)
							&& (trailing > -100 && trailing < 300)) {
						strawPrecluster_[chamberID][viewID][halfviewID][j].trailing = trailing;
						//						printf ("Straw completed\n");
					}
					//					printf("aggiorno l'hit: %d\n", j);
					//					strawPrecluster_[chamberID][viewID][halfviewID][j].printStraw();
				}
			}
			//			printf ("tl_flag = %d \n",tl_flag);
			if (!tl_flag) {
				//				printf ("New straw:  ");
				if (leading > -100)
					if (leading < 1)
						wireDistance = 0.0;
					else
						wireDistance = strawDigiMan_.rTDependenceData(leading / 1000);
				else
					wireDistance = -100.0;

				strawPrecluster_[chamberID][viewID][halfviewID][nStrawPreclusters[chamberID][viewID][halfviewID]].setStraw(chamberID,
						viewID, halfviewID, planeID, strawID, leading, trailing, 0, srbAddr[iEdge], position, wireDistance);
				//				printf("creo un nuovo hit:\n");
				//				strawPrecluster_[chamberID][viewID][halfviewID][nStrawPreclusters[chamberID][viewID][halfviewID]].printStraw();
				nStrawPreclusters[chamberID][viewID][halfviewID]++;
				//				printf(
				//						"Incrementing nStrawPreclusters[chamber %d] [view %d] [halfview %d] : nhits = %d \n",
				//						chamberID, viewID, halfviewID,
				//						nStrawPreclusters[chamberID][viewID][halfviewID]);
			}

			nHits++;
			//			printf ("NHits: %d\n",nHits);
			//gettimeofday(&time[10], 0);
			//			LOG_INFO( "Preclustering - Stop " << time[10].tv_sec << " " << time[10].tv_usec );
			//LOG_INFO( "Preclustering (xDigi) " << ((time[10].tv_sec - time[9].tv_sec)*1e6 + time[10].tv_usec) - time[9].tv_usec );
		}
		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

	if (!nEdges_tot)
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

//	LOG_INFO("\n PRECLUSTER, n ="<<ntotalhit);
//
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			for (int h = 0; h < 2; h++) {
//				printf("chamber %d view %d halfview %d : nhits = %d \n", i, j, h, nStrawPreclusters[i][j][h]);
//				for (int k = 0; k < nStrawPreclusters[i][j][h]; k++) {
//					strawPrecluster_[i][j][h][k].printStraw();
//				}
//			}
//		}
//	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int h = 0; h < 2; h++) {
				ntotalPreclusters += nStrawPreclusters[i][j][h];
			}
		}
	}

	///////////////////////////////////////Start Clustering inside the view///////////////////////////////////////////////////////
	//LOG_INFO("\n CLUSTERING INSIDE THE VIEW \n");
	//gettimeofday(&time[12], 0);
	//	LOG_INFO( "Clustering inside the view - Start " << time[12].tv_sec << " " << time[12].tv_usec );

	float positionh = 0.0;
	float positionj = 0.0;
	float meandistance = 0.0;
	float deltadistance = 0.0;
	float deltadistance_triplets = 0.0;
	float temp_distance = 0.0;
	double trailing_cluster = 0.0;

	//printf("cicli triplette:\n");

	for (int i = 0; i < 4; i++) {
		for (int g = 0; g < 4; g++) {
			//triplette 1
			//printf("camera %d, vista %d, triplette a:\n", i, g);

			for (int j = 0; j < nStrawPreclusters[i][g][0]; j++) { //hit loop
//				printf("hit 1 (%d): ", j);
//				strawPrecluster_[i][g][0][j].printStraw();

				// cluster con 3 hit (2 della stessa mezza vista)
				if (((strawPrecluster_[i][g][0][j].leading < (m1leadtrail * strawPrecluster_[i][g][0][j].trailing + q1leadtrail)
						&& strawPrecluster_[i][g][0][j].leading > (m2leadtrail * strawPrecluster_[i][g][0][j].trailing + q2leadtrail)
						&& (strawPrecluster_[i][g][0][j].trailing > cutlowtrailing
								&& strawPrecluster_[i][g][0][j].trailing < cuthightrailing))
						|| strawPrecluster_[i][g][0][j].trailing < -99999)
						&& (strawPrecluster_[i][g][0][j].leading > cutlowleading && strawPrecluster_[i][g][0][j].leading < cuthighleading)) {
					for (int h = j + 1; h < nStrawPreclusters[i][g][0] && !strawPrecluster_[i][g][0][j].used; h++) //prendo un hit appertenente al piano 2 o 3
							{
//						printf("hit 2 (%d): ", h);
//						strawPrecluster_[i][g][0][h].printStraw();

						if (((strawPrecluster_[i][g][0][h].leading < (m1leadtrail * strawPrecluster_[i][g][0][h].trailing + q1leadtrail)
								&& strawPrecluster_[i][g][0][h].leading
										> (m2leadtrail * strawPrecluster_[i][g][0][h].trailing + q2leadtrail)
								&& (strawPrecluster_[i][g][0][h].trailing > cutlowtrailing
										&& strawPrecluster_[i][g][0][h].trailing < cuthightrailing))
								|| strawPrecluster_[i][g][0][h].trailing < -99999)
								&& (strawPrecluster_[i][g][0][h].leading > cutlowleading
										&& strawPrecluster_[i][g][0][h].leading < cuthighleading)) {
							temp_distance = strawPrecluster_[i][g][0][j].position - strawPrecluster_[i][g][0][h].position;
							//printf("distance = %f  \n", temp_distance);
							if (temp_distance < 9 && temp_distance > -9 && !strawPrecluster_[i][g][0][h].used) {
								if (temp_distance > 0) {
									positionh = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
									positionj = strawPrecluster_[i][g][0][j].position - strawPrecluster_[i][g][0][j].wiredistance;
								} else {
									positionh = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
									positionj = strawPrecluster_[i][g][0][j].position + strawPrecluster_[i][g][0][j].wiredistance;
								}

								meandistance = (positionh + positionj) / 2;

								deltadistance_triplets = strawPrecluster_[i][g][0][h].wiredistance
										+ strawPrecluster_[i][g][0][j].wiredistance;

								if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][0][j].trailing > -200)
									trailing_cluster = (strawPrecluster_[i][g][0][h].trailing + strawPrecluster_[i][g][0][j].trailing) / 2;
								else {
									if (strawPrecluster_[i][g][0][h].trailing <= -200)
										trailing_cluster = strawPrecluster_[i][g][0][j].trailing;
									else
										trailing_cluster = strawPrecluster_[i][g][0][h].trailing;
								}

								//printf(" delta distance = %f \n", deltadistance_triplets);

								if (deltadistance_triplets > hit3low && deltadistance_triplets < hit3high) {
									strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
											strawPrecluster_[i][g][0][h].view, meandistance, trailing_cluster, deltadistance_triplets, 0);
//									printf(
//											"cluster vista fatto con 3 piani:\n    finale:  ");
//									strawCluster_[i][g][nStrawClusters[i][g]].printCluster2();
//									printf(
//											" ottenuto da questi 2:\n      (a)    ");
//									strawPrecluster_[i][g][0][j].printStraw();
//									printf("      (b)    ");
//									strawPrecluster_[i][g][0][h].printStraw();

									nStrawClusters[i][g]++;
									strawPrecluster_[i][g][0][h].used = 1;
									strawPrecluster_[i][g][0][j].used = 1;

									for (int l = 0; l < nStrawPreclusters[i][g][1]; l++) //cerco lo hit 3 negli altri 2 piani
											{
										temp_distance = fabs(meandistance - strawPrecluster_[i][g][1][l].position);
										if (temp_distance < 5 && !strawPrecluster_[i][g][1][l].used)
											strawPrecluster_[i][g][1][l].used = 1;
									}
								}
							}
						} else {
							//							printf(
							//									"taglio triangolo secondo(h) precluster:\n  ");
							//							strawPrecluster_[i][g][0][h].printStraw();
							//							printf("lead < %f and > %f\n",
							//									(m1leadtrail
							//											* strawPrecluster_[i][g][0][h].trailing
							//											+ q1leadtrail),
							//									(m2leadtrail
							//											* strawPrecluster_[i][g][0][h].trailing
							//											+ q2leadtrail));
						}
					}
				} else {
					//					printf("taglio triangolo primo(j) precluster:\n  ");
					//					strawPrecluster_[i][g][0][j].printStraw();
					//					printf("lead < %f and > %f\n",
					//							(m1leadtrail * strawPrecluster_[i][g][0][j].trailing
					//									+ q1leadtrail),
					//							(m2leadtrail * strawPrecluster_[i][g][0][j].trailing
					//									+ q2leadtrail));
				}
			}
			//triplette 2, cambio mezza vista
			//printf("camera %d, vista %d, triplette b:\n", i, g);

			for (int j = 0; j < nStrawPreclusters[i][g][1]; j++) //hit loop
					{
				//				printf("hit 1 (%d): ", j);
				//				strawPrecluster_[i][g][1][j].printStraw();
				// cluster con 3 hit (due della stessa mezza vista)
				if (((strawPrecluster_[i][g][1][j].leading < (m1leadtrail * strawPrecluster_[i][g][1][j].trailing + q1leadtrail)
						&& strawPrecluster_[i][g][1][j].leading > (m2leadtrail * strawPrecluster_[i][g][1][j].trailing + q2leadtrail)
						&& (strawPrecluster_[i][g][1][j].trailing > cutlowtrailing
								&& strawPrecluster_[i][g][1][j].trailing < cuthightrailing))
						or strawPrecluster_[i][g][1][j].trailing < -99999)
						&& (strawPrecluster_[i][g][1][j].leading > cutlowleading && strawPrecluster_[i][g][1][j].leading < cuthighleading)) {
					for (int h = j + 1; h < nStrawPreclusters[i][g][1] && !strawPrecluster_[i][g][1][j].used; h++) //prendo un hit appertenente al piano 2 o 3
							{
//						printf("hit 2 (%d): ", h);
//						strawPrecluster_[i][g][1][h].printStraw();
						if (((strawPrecluster_[i][g][1][h].leading < (m1leadtrail * strawPrecluster_[i][g][1][h].trailing + q1leadtrail)
								&& strawPrecluster_[i][g][1][h].leading
										> (m2leadtrail * strawPrecluster_[i][g][1][h].trailing + q2leadtrail)
								&& (strawPrecluster_[i][g][1][h].trailing > cutlowtrailing
										&& strawPrecluster_[i][g][1][h].trailing < cuthightrailing))
								|| strawPrecluster_[i][g][1][h].trailing < -99999)
								&& (strawPrecluster_[i][g][1][h].leading > cutlowleading
										&& strawPrecluster_[i][g][1][h].leading < cuthighleading)) {
							temp_distance = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][h].position;
							//							printf("distance = %f \n", temp_distance);

							if (temp_distance < 9 && temp_distance > -9 && !strawPrecluster_[i][g][1][h].used) {
								if (temp_distance > 0) {
									positionh = strawPrecluster_[i][g][1][h].position + strawPrecluster_[i][g][1][h].wiredistance;
									positionj = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
								} else {
									positionh = strawPrecluster_[i][g][1][h].position - strawPrecluster_[i][g][1][h].wiredistance;
									positionj = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
								}

								meandistance = (positionh + positionj) / 2;

								deltadistance_triplets = strawPrecluster_[i][g][1][h].wiredistance
										+ strawPrecluster_[i][g][1][j].wiredistance;

								if (strawPrecluster_[i][g][1][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
									trailing_cluster = (strawPrecluster_[i][g][1][h].trailing + strawPrecluster_[i][g][1][j].trailing) / 2;
								else {
									if (strawPrecluster_[i][g][1][h].trailing <= -200)
										trailing_cluster = strawPrecluster_[i][g][1][j].trailing;
									else
										trailing_cluster = strawPrecluster_[i][g][1][h].trailing;
								}

								//								printf(" delta distance = %f \n",
								//										deltadistance_triplets);

								if (deltadistance_triplets > hit3low && deltadistance_triplets < hit3high) {
									strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][1][h].chamber,
											strawPrecluster_[i][g][1][h].view, meandistance, trailing_cluster, deltadistance_triplets, 0);

//									printf(
//											"cluster dentro la vista fatto con 3 view:\n    finale:  ");
//									strawCluster_[i][g][nStrawClusters[i][g]].printCluster2();
//									printf(
//											" ottenuto da questi 2:\n      (a)    ");
//									strawPrecluster_[i][g][1][j].printStraw();
//									printf("      (b)    ");
//									strawPrecluster_[i][g][1][h].printStraw();

									nStrawClusters[i][g]++;
									strawPrecluster_[i][g][1][h].used = 1;
									strawPrecluster_[i][g][1][j].used = 1;

									for (int l = 0; l < nStrawPreclusters[i][g][0]; l++) //cerco il 3 hit negli altri 2 piani
											{
										temp_distance = fabs(meandistance - strawPrecluster_[i][g][0][l].position);
										if (temp_distance < 5 && !strawPrecluster_[i][g][0][l].used)
											strawPrecluster_[i][g][0][l].used = 1;
									}
								}
							}
						} else {
							//							printf(
							//									"taglio triangolo secondo(h) precluster:\n  ");
							//							strawPrecluster_[i][g][1][h].printStraw();
							//							printf("lead < %f and > %f\n",
							//									(m1leadtrail
							//											* strawPrecluster_[i][g][1][h].trailing
							//											+ q1leadtrail),
							//									(m2leadtrail
							//											* strawPrecluster_[i][g][1][h].trailing
							//											+ q2leadtrail));
						}
					}

					//cluster con 2 hit
					//printf("cluster con 2 hit :\n");

					for (int h = 0; h < nStrawPreclusters[i][g][0] && !strawPrecluster_[i][g][1][j].used; h++) //prendo un hit appertenente al piano 2 o 3
							{
//						printf("hit 2 (%d): ", h);
//						strawPrecluster_[i][g][1][h].printStraw();
						if (((strawPrecluster_[i][g][0][h].leading < (m1leadtrail * strawPrecluster_[i][g][0][h].trailing + q1leadtrail)
								&& strawPrecluster_[i][g][0][h].leading
										> (m2leadtrail * strawPrecluster_[i][g][0][h].trailing + q2leadtrail)
								&& (strawPrecluster_[i][g][0][h].trailing > cutlowtrailing
										&& strawPrecluster_[i][g][0][h].trailing < cuthightrailing))
								|| strawPrecluster_[i][g][0][h].trailing < -99999)
								&& (strawPrecluster_[i][g][0][h].leading > cutlowleading
										&& strawPrecluster_[i][g][0][h].leading < cuthighleading)) {
							temp_distance = fabs(strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][0][h].position);
							//							printf("distance = %f \n", temp_distance);
							if (temp_distance < 9 && !strawPrecluster_[i][g][0][h].used) {
								switch (strawPrecluster_[i][g][1][j].plane) {
								case (0):
									if (!strawPrecluster_[i][g][0][h].plane) {
										if (!g || g == 2) {
											positionh = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											positionj = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										} else {
											positionh = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											positionj = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										}

										meandistance = (positionh + positionj) / 2;

										deltadistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											trailing_cluster = (strawPrecluster_[i][g][0][h].trailing
													+ strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200)
												trailing_cluster = strawPrecluster_[i][g][1][j].trailing;
											else
												trailing_cluster = strawPrecluster_[i][g][0][h].trailing;
										}

										//										printf(" delta distance = %f \n",
										//												deltadistance_triplets);

										if (deltadistance > hit2low && deltadistance < hit2high) {
											strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, meandistance, trailing_cluster, deltadistance, 0);

//											printf(
//													"cluster dentro la vista fatto con 2 view:\n    finale:  ");
//											strawCluster_[i][g][nStrawClusters[i][g]].printCluster2();
//											printf(
//													" ottenuto da questi 2:\n      (a)    ");
//											strawPrecluster_[i][g][1][j].printStraw();
//											printf("      (b)    ");
//											strawPrecluster_[i][g][0][h].printStraw();

											nStrawClusters[i][g]++;
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									} else {
										if (!g || g == 2) {
											positionh = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											positionj = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										} else {
											positionh = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											positionj = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										}

										meandistance = (positionh + positionj) / 2;

										deltadistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											trailing_cluster = (strawPrecluster_[i][g][0][h].trailing
													+ strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200)
												trailing_cluster = strawPrecluster_[i][g][1][j].trailing;
											else
												trailing_cluster = strawPrecluster_[i][g][0][h].trailing;
										}

										//										printf(" delta distance = %f \n",
										//												deltadistance_triplets);

										if (deltadistance > hit2low && deltadistance < hit2high) {
											strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, meandistance, trailing_cluster, deltadistance, 0);

//																					printf(
//																							"cluster dentro la vista fatto con 2 view:\n    finale:  ");
//																					strawCluster_[i][g][nStrawClusters[i][g]].printCluster2();
//																					printf(
//																							" ottenuto da questi 2:\n      (a)    ");
//																					strawPrecluster_[i][g][1][j].printStraw();
//																					printf("      (b)    ");
//																					strawPrecluster_[i][g][0][h].printStraw();

											nStrawClusters[i][g]++;
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									}

									break;
								case (1):
									if (!strawPrecluster_[i][g][0][h].plane) {
										if (!g or g == 2) {
											positionh = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											positionj = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										} else {
											positionh = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											positionj = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										}

										meandistance = (positionh + positionj) / 2;

										deltadistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											trailing_cluster = (strawPrecluster_[i][g][0][h].trailing
													+ strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200)
												trailing_cluster = strawPrecluster_[i][g][1][j].trailing;
											else
												trailing_cluster = strawPrecluster_[i][g][0][h].trailing;
										}

										//										printf(" delta distance = %f \n",
										//												deltadistance_triplets);

										if (deltadistance > hit2low && deltadistance < hit2high) {
											strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, meandistance, trailing_cluster, deltadistance, 0);

//																					printf(
//																							"cluster dentro la vista fatto con 2 view:\n    finale:  ");
//																					strawCluster_[i][g][nStrawClusters[i][g]].printCluster2();
//																					printf(
//																							" ottenuto da questi 2:\n      (a)    ");
//																					strawPrecluster_[i][g][1][j].printStraw();
//																					printf("      (b)    ");
//																					strawPrecluster_[i][g][0][h].printStraw();

											nStrawClusters[i][g]++;
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									} else {
										if (!g or g == 2) {
											positionh = strawPrecluster_[i][g][0][h].position + strawPrecluster_[i][g][0][h].wiredistance;
											positionj = strawPrecluster_[i][g][1][j].position - strawPrecluster_[i][g][1][j].wiredistance;
										} else {
											positionh = strawPrecluster_[i][g][0][h].position - strawPrecluster_[i][g][0][h].wiredistance;
											positionj = strawPrecluster_[i][g][1][j].position + strawPrecluster_[i][g][1][j].wiredistance;
										}

										meandistance = (positionh + positionj) / 2;

										deltadistance = strawPrecluster_[i][g][0][h].wiredistance
												+ strawPrecluster_[i][g][1][j].wiredistance;

										if (strawPrecluster_[i][g][0][h].trailing > -200 && strawPrecluster_[i][g][1][j].trailing > -200)
											trailing_cluster = (strawPrecluster_[i][g][0][h].trailing
													+ strawPrecluster_[i][g][1][j].trailing) / 2;
										else {
											if (strawPrecluster_[i][g][0][h].trailing <= -200)
												trailing_cluster = strawPrecluster_[i][g][1][j].trailing;
											else
												trailing_cluster = strawPrecluster_[i][g][0][h].trailing;
										}

										//										printf(" delta distance = %f \n",
										//												deltadistance_triplets);

										if (deltadistance > hit2low && deltadistance < hit2high) {
											strawCluster_[i][g][nStrawClusters[i][g]].setCluster(strawPrecluster_[i][g][0][h].chamber,
													strawPrecluster_[i][g][0][h].view, meandistance, trailing_cluster, deltadistance, 0);

//																					printf(
//																							"cluster dentro la vista fatto con 2 view:\n    finale:  ");
//																					strawCluster_[i][g][nStrawClusters[i][g]].printCluster2();
//																					printf(
//																							" ottenuto da questi 2:\n      (a)    ");
//																					strawPrecluster_[i][g][1][j].printStraw();
//																					printf("      (b)    ");
//																					strawPrecluster_[i][g][0][h].printStraw();

											nStrawClusters[i][g]++;
											strawPrecluster_[i][g][0][h].used = 1;
											strawPrecluster_[i][g][1][j].used = 1;
										}
									}
									break;
								}
							}
						} else {
							//							printf(
							//									"taglio doppietto secondo(h) precluster:\n  ");
							//							strawPrecluster_[i][g][0][h].printStraw();
							//							printf("lead < %f and > %f\n",
							//									(m1leadtrail
							//											* strawPrecluster_[i][g][0][h].trailing
							//											+ q1leadtrail),
							//									(m2leadtrail
							//											* strawPrecluster_[i][g][0][h].trailing
							//											+ q2leadtrail));
						}
					}
				} else {

					//					printf(
					//							"taglio triangolo e doppietto primo(j) precluster:\n  ");
					//					strawPrecluster_[i][g][1][j].printStraw();
					//					printf("lead < %f and > %f\n",
					//							(m1leadtrail * strawPrecluster_[i][g][1][j].trailing
					//									+ q1leadtrail),
					//							(m2leadtrail * strawPrecluster_[i][g][1][j].trailing
					//									+ q2leadtrail));
				}
			}
		}
	}
	//gettimeofday(&time[13], 0);
	//	LOG_INFO( "Clustering inside the view - Stop " << time[13].tv_sec << " " << time[13].tv_usec );
	//	LOG_INFO( "Clustering inside the view " << ((time[13].tv_sec - time[12].tv_sec)*1e6 + time[13].tv_usec) - time[12].tv_usec );
	//	LOG_INFO( "Clustering inside the view fino all'inizio" << ((time[13].tv_sec - time[0].tv_sec)*1e6 + time[13].tv_usec) - time[0].tv_usec );

//	printf("\n  CLUSTER OTTENUTI  \n");
//	for (int i = 0; i < 4; i++) {
//		for (int g = 0; g < 4; g++) {
//			printf("  N CLUSTER camera %d vista %d = %d\n", i, g, nStrawClusters[i][g]);
//			for (int j = 0; j < nStrawClusters[i][g]; j++) {
//				strawCluster_[i][g][j].printCluster2();
//			}
//		}
//	}

	/////////////////////////////////////// Start Clustering inside the chamber ///////////////////////////////////////////////////////
	/////////////////////////////////////// 0=v, 1=u, 2=x, 3=y
	//LOG_INFO("CLUSTERING INSIDE THE CHAMBER");

	//gettimeofday(&time[14], 0);
	//	LOG_INFO( "Clustering inside the chamber - Start " << time[14].tv_sec << " " << time[14].tv_usec );

	float coordinate_temp = 0.0;
	float viewdistance = 0.0;
	float viewdistance4 = 0.0;
	float xtemp = 0.0;
	float ytemp = 0.0;
	int pointflag = 0;

	//tracks reco
	float mx = 0.0;
	float my = 0.0;
	float qx = 0.0;
	float qy = 0.0;
	double coordinate[2];

	for (int i = 0; i < 4; i++) {
		////////////ciclo dei punti a 4 viste//////////////////
		//gettimeofday(&time[15], 0);
		ntotalviewcluster += nStrawClusters[i][3] + nStrawClusters[i][2] + nStrawClusters[i][1] + nStrawClusters[i][0];
		for (int a = 0; a < nStrawClusters[i][3]; a++) //clusters [a] is inside y
				{
			for (int b = 0; b < nStrawClusters[i][2]; b++)  //we loop on x view
					{
				for (int c = 0; c < nStrawClusters[i][1]; c++) //we loop on u  views
						{
					coordinate_temp = (strawCluster_[i][3][a].coordinate + strawCluster_[i][2][b].coordinate) / sq2;
					viewdistance = fabs(strawCluster_[i][1][c].coordinate - coordinate_temp);

					if (viewdistance < cutcluster) // and ( strawCluster_[i][3][a].used == 0 or strawCluster_[i][2][b].used == 0 or strawCluster_[i][1][c].used == 0))
							{
						for (int d = 0; d < nStrawClusters[i][0]; d++) //v views
								{
							coordinate_temp = (strawCluster_[i][2][b].coordinate - strawCluster_[i][3][a].coordinate) / sq2;
							viewdistance4 = fabs(strawCluster_[i][0][d].coordinate - coordinate_temp);

							if (viewdistance4 < cutcluster) {
								xtemp = strawCluster_[i][2][b].coordinate;
								ytemp = strawCluster_[i][3][a].coordinate;

								//	printf("viewdistance = %f, cut = %f\n", viewdistance, viewdistance4);

								if (viewdistance < viewdistance4)
									strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], xtemp, ytemp,
											(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing
													+ strawCluster_[i][1][c].trailing + strawCluster_[i][0][d].trailing) / 4, viewdistance,
											4, 0);
								else
									strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], xtemp, ytemp,
											(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing
													+ strawCluster_[i][1][c].trailing + strawCluster_[i][0][d].trailing) / 4, viewdistance4,
											4, 0);

								/*printf(
								 "4 viste: camera %d, viewdistance = %f, viewdistance4 %f: cluster v%d, u%d, x%d, y%d\n",
								 i, viewdistance, viewdistance4, d, c, b,
								 a);
								 strawCluster_[i][0][d].printCluster();
								 strawCluster_[i][1][c].printCluster();
								 strawCluster_[i][2][b].printCluster();
								 strawCluster_[i][3][a].printCluster();
								 printf("punto aggiunto:\n");
								 strawPointTemp_[i][nStrawPointsTemp[i]].printPoint();*/

								strawCluster_[i][0][d].used = 1;
								strawCluster_[i][1][c].used = 1;
								strawCluster_[i][2][b].used = 1;
								strawCluster_[i][3][a].used = 1;

								nStrawPointsTemp[i]++;

							}
						}
					}
				}
			}
		}
		//gettimeofday(&time[16], 0);
		//////////////////////////// primo ciclo dei punti a 3 viste prima yx cercando in v o u, poi uv cercando in x o y
		//gettimeofday(&time[17], 0);
		for (int a = 0; a < nStrawClusters[i][3]; a++) //clusters [a] is inside y
				{
			//				if(strawCluster_[i][3][a].used != 1)
			//				{
			for (int b = 0; b < nStrawClusters[i][2]; b++)  //we loop on x view
					{
				//					if(strawCluster_[i][2][b].used != 1)
				//					{
				for (int c = 0; c < nStrawClusters[i][1]; c++) //we loop on u  views
						{
					//	if(strawCluster_[i][1][c].used != 1)
					if ((strawCluster_[i][1][c].used != 1 && strawCluster_[i][2][b].used != 1)
							or (strawCluster_[i][1][c].used != 1 && strawCluster_[i][3][a].used != 1)
							or (strawCluster_[i][2][b].used != 1 && strawCluster_[i][3][a].used != 1)) {
						coordinate_temp = (strawCluster_[i][3][a].coordinate + strawCluster_[i][2][b].coordinate) / sq2;
						viewdistance = fabs(strawCluster_[i][1][c].coordinate - coordinate_temp);

						if (viewdistance < cutcluster) // && ( strawCluster_[i][3][a].used == 0 or strawCluster_[i][2][b].used == 0 or strawCluster_[i][1][c].used == 0))
								{
							xtemp = strawCluster_[i][2][b].coordinate;
							ytemp = strawCluster_[i][3][a].coordinate;

							strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], xtemp, ytemp,
									(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing + strawCluster_[i][1][c].trailing)
											/ 3, viewdistance, 3, 0);

							/*printf(
							 "3 viste: camera %d, viewdistance = %f: cluster u%d, x%d, y%d\n",
							 i, viewdistance, c, b, a);
							 strawCluster_[i][1][c].printCluster();
							 strawCluster_[i][2][b].printCluster();
							 strawCluster_[i][3][a].printCluster();
							 printf("punto aggiunto:\n");
							 strawPointTemp_[i][nStrawPointsTemp[i]].printPoint();*/

							strawCluster_[i][1][c].used = 2;
							strawCluster_[i][2][b].used = 2;
							strawCluster_[i][3][a].used = 2;

							nStrawPointsTemp[i]++;
						}
					}
				}
				for (int c = 0; c < nStrawClusters[i][0]; c++) //we loop on v views
						{
					//		if(strawCluster_[i][0][c].used != 1)
					if ((strawCluster_[i][0][c].used != 1 && strawCluster_[i][2][b].used != 1)
							or (strawCluster_[i][0][c].used != 1 && strawCluster_[i][3][a].used != 1)
							or (strawCluster_[i][2][b].used != 1 && strawCluster_[i][3][a].used != 1)) {
						coordinate_temp = (strawCluster_[i][2][b].coordinate - strawCluster_[i][3][a].coordinate) / sq2;
						viewdistance = fabs(strawCluster_[i][0][c].coordinate - coordinate_temp);

						if (viewdistance < cutcluster) // && (strawCluster_[i][3][a].used == 0 or strawCluster_[i][2][b].used == 0 or strawCluster_[i][0][c].used == 0))
								{
							xtemp = strawCluster_[i][2][b].coordinate;
							ytemp = strawCluster_[i][3][a].coordinate;

							strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], xtemp, ytemp,
									(strawCluster_[i][3][a].trailing + strawCluster_[i][2][b].trailing + strawCluster_[i][0][c].trailing)
											/ 3, viewdistance, 3, 0);

							/*printf(
							 "3 viste: camera %d, viewdistance = %f: cluster v%d, x%d, y%d\n",
							 i, viewdistance, c, b, a);
							 strawCluster_[i][0][c].printCluster();
							 strawCluster_[i][2][b].printCluster();
							 strawCluster_[i][3][a].printCluster();
							 printf("punto aggiunto:\n");
							 strawPointTemp_[i][nStrawPointsTemp[i]].printPoint();*/

							strawCluster_[i][0][c].used = 2;
							strawCluster_[i][2][b].used = 2;
							strawCluster_[i][3][a].used = 2;

							nStrawPointsTemp[i]++;
						}
					}
				}
			}
			//	}
		}
		//	}
		//gettimeofday(&time[18], 0);
		/////////////////////////3 viste seconda clusterizzazione partendo da u e v

		//gettimeofday(&time[19], 0);
		//		LOG_INFO( nStrawClusters[i][0] );
		for (int a = 0; a < nStrawClusters[i][0]; a++) //v
				{
			// if(strawCluster_[i][0][a].used != 1)
			// {
			//			LOG_INFO( nStrawClusters[i][1] );
			for (int b = 0; b < nStrawClusters[i][1]; b++) //u
					{
				// if(strawCluster_[i][1][b].used != 1)
				// {
				//				LOG_INFO( nStrawClusters[i][2] );
				for (int c = 0; c < nStrawClusters[i][2]; c++) //x
						{
					// if(strawCluster_[i][2][c].used != 1)
					if ((strawCluster_[i][0][a].used != 1 && strawCluster_[i][1][b].used != 1)
							or (strawCluster_[i][0][a].used != 1 && strawCluster_[i][2][c].used != 1)
							or (strawCluster_[i][1][b].used != 1 && strawCluster_[i][2][c].used != 1)) {
						coordinate_temp = (strawCluster_[i][0][a].coordinate + strawCluster_[i][1][b].coordinate) / sq2;
						viewdistance = fabs(strawCluster_[i][2][c].coordinate - coordinate_temp);

						if (viewdistance < cutcluster) {
							xtemp = strawCluster_[i][2][c].coordinate;
							ytemp = (strawCluster_[i][1][b].coordinate - strawCluster_[i][0][a].coordinate) / sq2;

							strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], xtemp, ytemp,
									(strawCluster_[i][0][a].trailing + strawCluster_[i][1][b].trailing + strawCluster_[i][2][c].trailing)
											/ 3, viewdistance, 3, 0);

							/*printf(
							 "3 viste: camera %d, viewdistance = %f: cluster v%d, u%d, x%d\n",
							 i, viewdistance, a, b, c);
							 strawCluster_[i][0][a].printCluster();
							 strawCluster_[i][1][b].printCluster();
							 strawCluster_[i][2][c].printCluster();
							 printf("punto aggiunto1:\n");
							 strawPointTemp_[i][nStrawPointsTemp[i]].printPoint();*/

							strawCluster_[i][0][a].used = 2;
							strawCluster_[i][1][b].used = 2;
							strawCluster_[i][2][c].used = 2;

							nStrawPointsTemp[i]++;
						}
					}
				}
				//				LOG_INFO( nStrawClusters[i][3] );
				for (int c = 0; c < nStrawClusters[i][3]; c++) //y
						{
					//	if(strawCluster_[i][3][c].used != 1)
					if ((strawCluster_[i][0][a].used != 1 && strawCluster_[i][1][b].used != 1)
							or (strawCluster_[i][0][a].used != 1 && strawCluster_[i][3][c].used != 1)
							or (strawCluster_[i][1][b].used != 1 && strawCluster_[i][3][c].used != 1)) {
						coordinate_temp = (strawCluster_[i][1][b].coordinate - strawCluster_[i][0][a].coordinate) / sq2;
						viewdistance = fabs(strawCluster_[i][3][c].coordinate - coordinate_temp);

						if (viewdistance < cutcluster) {
							xtemp = (strawCluster_[i][0][a].coordinate + strawCluster_[i][1][b].coordinate) / sq2;
							ytemp = strawCluster_[i][3][c].coordinate;

							strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], xtemp, ytemp,
									(strawCluster_[i][0][a].trailing + strawCluster_[i][1][b].trailing + strawCluster_[i][3][c].trailing)
											/ 3, viewdistance, 3, 0);

							/*printf(
							 "3 viste: camera %d, viewdistance = %f: cluster v%d, u%d, y%d\n",
							 i, viewdistance, a, b, c);
							 strawCluster_[i][0][a].printCluster();
							 strawCluster_[i][1][b].printCluster();
							 strawCluster_[i][3][c].printCluster();
							 printf("punto aggiunto2:\n");
							 strawPointTemp_[i][nStrawPointsTemp[i]].printPoint();*/

							strawCluster_[i][0][a].used = 2;
							strawCluster_[i][1][b].used = 2;
							strawCluster_[i][3][c].used = 2;

							nStrawPointsTemp[i]++;
						}
					}
				}
			}
			//	}
			//	}
		}
		//gettimeofday(&time[20], 0);
		//clusterizzazione nelle camere con sole 2 viste
		//gettimeofday(&time[21], 0);
		for (int a = 0; a < nStrawClusters[i][0]; a++)  //v
				{
			//	if(strawCluster_[i][0][a].used == 0 )
			//{
			for (int b = 0; b < nStrawClusters[i][1]; b++) //u (v,u)
					{
				if (strawCluster_[i][1][b].used == 0 && strawCluster_[i][0][a].used == 0) {
					coordinate[0] = (strawCluster_[i][0][a].coordinate + strawCluster_[i][1][b].coordinate) / sq2;
					coordinate[1] = (strawCluster_[i][1][b].coordinate - strawCluster_[i][0][a].coordinate) / sq2;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], coordinate[0], coordinate[1],
								(strawCluster_[i][0][a].trailing + strawCluster_[i][1][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;

						/*printf(
						 "punto con soli due cluster (u,v) %d nella camera %d dai cluster %d, %d \n",
						 nStrawPointsTemp[i], i, a, b);*/

					}
				}
			}
			for (int b = 0; b < nStrawClusters[i][2]; b++) //x (v,x)
					{
				if (strawCluster_[i][2][b].used == 0 && strawCluster_[i][0][a].used == 0) {
					coordinate[0] = strawCluster_[i][2][b].coordinate;
					coordinate[1] = strawCluster_[i][2][b].coordinate - (sq2 * strawCluster_[i][0][a].coordinate);

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], coordinate[0], coordinate[1],
								(strawCluster_[i][0][a].trailing + strawCluster_[i][2][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;

						/*printf(
						 "punto con soli due cluster (u,v) %d nella camera %d dai cluster %d, %d \n",
						 nStrawPointsTemp[i], i, a, b);*/

					}
				}
			}
			for (int b = 0; b < nStrawClusters[i][3]; b++) //y (v,y)
					{
				if (strawCluster_[i][3][b].used == 0 && strawCluster_[i][0][a].used == 0) {
					coordinate[0] = (sq2 * strawCluster_[i][0][a].coordinate) + strawCluster_[i][3][b].coordinate;
					coordinate[1] = strawCluster_[i][3][b].coordinate;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], coordinate[0], coordinate[1],
								(strawCluster_[i][0][a].trailing + strawCluster_[i][3][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;

						/*printf(
						 "punto con soli due cluster (u,v) %d nella camera %d dai cluster %d, %d \n",
						 nStrawPointsTemp[i], i, a, b);*/

					}
				}
			}
			//}
		}
		//gettimeofday(&time[22], 0);
		//gettimeofday(&time[23], 0);
		for (int a = 0; a < nStrawClusters[i][1]; a++)  //u
				{
			//	if(strawCluster_[i][1][a].used == 0)
			//{
			for (int b = 0; b < nStrawClusters[i][2]; b++) //x (u,x)
					{
				if (strawCluster_[i][2][b].used == 0 && strawCluster_[i][1][a].used == 0) {
					coordinate[0] = strawCluster_[i][2][b].coordinate;
					coordinate[1] = (sq2 * strawCluster_[i][1][a].coordinate) - strawCluster_[i][2][b].coordinate;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], coordinate[0], coordinate[1],
								(strawCluster_[i][1][a].trailing + strawCluster_[i][2][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;

						/*printf(
						 "punto con soli due cluster (u,v) %d nella camera %d dai cluster %d, %d \n",
						 nStrawPointsTemp[i], i, a, b);*/

					}
				}
			}
			for (int b = 0; b < nStrawClusters[i][3]; b++) //y (u,y)
					{
				if (strawCluster_[i][3][b].used == 0 && strawCluster_[i][1][a].used == 0) {
					coordinate[0] = (sq2 * strawCluster_[i][1][a].coordinate) - strawCluster_[i][3][b].coordinate;
					coordinate[1] = strawCluster_[i][3][b].coordinate;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], coordinate[0], coordinate[1],
								(strawCluster_[i][1][a].trailing + strawCluster_[i][3][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;

						/*printf(
						 "punto con soli due cluster (u,v) %d nella camera %d dai cluster %d, %d \n",
						 nStrawPointsTemp[i], i, a, b);*/

					}
				}
			}
			//}
		}
		//gettimeofday(&time[24], 0);
		//gettimeofday(&time[25], 0);
		for (int a = 0; a < nStrawClusters[i][2]; a++)  //x
				{
			//	if(strawCluster_[i][2][a].used == 0)
			//{
			for (int b = 0; b < nStrawClusters[i][3]; b++) //y (x,y)
					{
				if (strawCluster_[i][3][b].used == 0 && strawCluster_[i][2][a].used == 0) {
					coordinate[0] = strawCluster_[i][2][a].coordinate;
					coordinate[1] = strawCluster_[i][3][b].coordinate;

					if (strawAcceptance(i, coordinate, 12) == 1) {
						strawPointTemp_[i][nStrawPointsTemp[i]].setPoint(fChamberZPosition[i], coordinate[0], coordinate[1],
								(strawCluster_[i][2][a].trailing + strawCluster_[i][3][b].trailing) / 2, 0, 2, 0);
						nStrawPointsTemp[i]++;

						/*printf(
						 "punto con soli due cluster (u,v) %d nella camera %d dai cluster %d, %d \n",
						 nStrawPointsTemp[i], i, a, b);*/

					}
				}
			}
			//	}
		}
		//gettimeofday(&time[26], 0);
	}
	//gettimeofday(&time[27], 0);
	//	LOG_INFO( "Clustering inside the chamber - Stop " << time[15].tv_sec << " " << time[15].tv_usec );
	//	LOG_INFO( "Clustering inside the chamber " << ((time[27].tv_sec - time[14].tv_sec)*1e6 + time[27].tv_usec) - time[14].tv_usec );
	//	LOG_INFO( "Clustering inside the chamber - initial " << ((time[27].tv_sec - time[0].tv_sec)*1e6 + time[27].tv_usec) - time[0].tv_usec );
	//	if (nEdges_tot && nEdges_tot < 1500) LOG_INFO(((time[1].tv_sec - time[0].tv_sec)*1e6 + time[1].tv_usec) - time[0].tv_usec << "\t" << ((time[4].tv_sec - time[3].tv_sec)*1e6 + time[4].tv_usec) - time[3].tv_usec << "\t" << ((time[6].tv_sec - time[5].tv_sec)*1e6 + time[6].tv_usec) - time[5].tv_usec << "\t" << ((time[8].tv_sec - time[7].tv_sec)*1e6 + time[8].tv_usec) - time[7].tv_usec << "\t" << ((time[10].tv_sec - time[9].tv_sec)*1e6 + time[10].tv_usec) - time[9].tv_usec << "\t" << ((time[11].tv_sec - time[2].tv_sec)*1e6 + time[11].tv_usec) - time[2].tv_usec << "\t" << ((time[13].tv_sec - time[12].tv_sec)*1e6 + time[13].tv_usec) - time[12].tv_usec << "\t" << nEdges_tot << "\t" << ((time[27].tv_sec - time[14].tv_sec)*1e6 + time[27].tv_usec) - time[14].tv_usec << "\t" << ((time[27].tv_sec - time[0].tv_sec)*1e6 + time[27].tv_usec) - time[0].tv_usec << "\t" << ((time[16].tv_sec - time[15].tv_sec)*1e6 + time[16].tv_usec) - time[15].tv_usec << "\t" << ((time[18].tv_sec - time[17].tv_sec)*1e6 + time[18].tv_usec) - time[17].tv_usec << "\t" << ((time[20].tv_sec - time[19].tv_sec)*1e6 + time[20].tv_usec) - time[19].tv_usec << "\t" << ((time[22].tv_sec - time[21].tv_sec)*1e6 + time[22].tv_usec) - time[21].tv_usec << "\t" << ((time[24].tv_sec - time[23].tv_sec)*1e6 + time[24].tv_usec) - time[23].tv_usec << "\t" << ((time[26].tv_sec - time[25].tv_sec)*1e6 + time[26].tv_usec) - time[25].tv_usec);
	/*
	 for (int i = 0; i < 4; i++) {
	 printf("camera: %d, n punti= %d \n", i, nStrawPointsTemp[i]);
	 for (int j = 0; j < nStrawPointsTemp[i]; j++) {
	 strawPointTemp_[i][j].printPoint2();
	 }
	 }
	 */
	////////////////////////////////////////POINT SELECTION/////////////////////////////////////////////////////////
	//	LOG_INFO( "POINT SELECTION...!!! " );
	float point_dx = 0.0;
	float point_dy = 0.0;
	Point pointTemp;
	float viewdistance_delta = 0.0;

	for (int i = 0; i < 4; i++) {
		if (nStrawPointsTemp[i] < 7) {
			//			LOG_INFO( "!!!!!!!!!!!!!! Sto entrando qui !!!!!! " );
			nStrawPointsFinal[i] = nStrawPointsTemp[i];
			for (int j = 0; j < nStrawPointsFinal[i]; j++) { //loop over point
				strawPointFinal_[i][j] = strawPointTemp_[i][j];
			}
			continue;
		}
		for (int j = 0; j < nStrawPointsTemp[i]; j++) //loop over point
				{
			if (strawPointTemp_[i][j].nViews == 4 && !strawPointTemp_[i][j].used) {
				//				strawPointTempbis_[i][nStrawPointsTempbis[i]].clonePoint(strawPointTemp_[i][j]);
				//				nStrawPointsTempbis[i]++;
				pointTemp.clonePoint(strawPointTemp_[i][j]);
				strawPointTemp_[i][j].used = 1;
				for (int k = j + 1; k < nStrawPointsTemp[i]; k++) //loop over point
						{
					if (strawPointTemp_[i][k].nViews == 4 && !strawPointTemp_[i][k].used) {
						//						point_dx = fabs(strawPointTemp_[i][j].x-strawPointTemp_[i][k].x);
						point_dy = fabs(strawPointTemp_[i][j].y - strawPointTemp_[i][k].y);
						if (point_dy < 4.0) //point_dx<5.0 or
								{
							viewdistance_delta = pointTemp.viewDistance - strawPointTemp_[i][k].viewDistance;
							if (viewdistance_delta > 1.0 && pointTemp.viewDistance > 3.0) {
								//								pointTemp.setPoint(strawPointTemp_[i][k].z,strawPointTemp_[i][k].x,strawPointTemp_[i][k].y,strawPointTemp_[i][k].trailing,strawPointTemp_[i][k].viewDistance,strawPointTemp_[i][k].nViews,strawPointTemp_[i][k].used);
								pointTemp.clonePoint(strawPointTemp_[i][k]);
								strawPointTemp_[i][k].used = 1;
							}
							if (viewdistance_delta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTemp_[i][k].used = 1;
							}
						}
					}
				}
				strawPointTempbis_[i][nStrawPointsTempbis[i]].clonePoint(pointTemp);
				nStrawPointsTempbis[i]++;
			}
			if (strawPointTemp_[i][j].nViews == 3 && !strawPointTemp_[i][j].used) {
				//				pointTemp.setPoint(strawPointTemp_[i][j].z,strawPointTemp_[i][j].x,strawPointTemp_[i][j].y,strawPointTemp_[i][j].trailing,strawPointTemp_[i][j].viewDistance,strawPointTemp_[i][j].nViews,strawPointTemp_[i][j].used);
				pointTemp.clonePoint(strawPointTemp_[i][j]);
				strawPointTemp_[i][j].used = 1;
				for (int k = j + 1; k < nStrawPointsTemp[i]; k++) //loop over point
						{
					if (strawPointTemp_[i][k].nViews == 3 && !strawPointTemp_[i][k].used) {
						//						point_dx = fabs(strawPointTemp_[i][j].x-strawPointTemp_[i][k].x);
						point_dy = fabs(strawPointTemp_[i][j].y - strawPointTemp_[i][k].y);
						if (point_dy < 4.0) //point_dx<5.0 or
								{
							viewdistance_delta = pointTemp.viewDistance - strawPointTemp_[i][k].viewDistance;
							if (viewdistance_delta > 1.0 && pointTemp.viewDistance > 3.0) {
								//								pointTemp.setPoint(strawPointTemp_[i][k].z,strawPointTemp_[i][k].x,strawPointTemp_[i][k].y,strawPointTemp_[i][k].trailing,strawPointTemp_[i][k].viewDistance,strawPointTemp_[i][k].nViews,strawPointTemp_[i][k].used);
								pointTemp.clonePoint(strawPointTemp_[i][k]);
								strawPointTemp_[i][k].used = 1;
							}
							if (viewdistance_delta < -1.0 && strawPointTemp_[i][k].viewDistance > 3.0) {
								strawPointTemp_[i][k].used = 1;
							}
						}
					}
				}
				strawPointTempbis_[i][nStrawPointsTempbis[i]].clonePoint(pointTemp);
				nStrawPointsTempbis[i]++;
			}
			if (strawPointTemp_[i][j].nViews == 2) {
				strawPointTempbis_[i][nStrawPointsTempbis[i]].clonePoint(strawPointTemp_[i][j]);
				nStrawPointsTempbis[i]++;
			}
		}

		////////second SELECTION

		for (int j = 0; j < nStrawPointsTempbis[i]; j++) //loop over point
				{
			if (strawPointTempbis_[i][j].nViews == 4 && !strawPointTempbis_[i][j].used) {
				pointTemp.clonePoint(strawPointTempbis_[i][j]);
				strawPointTempbis_[i][j].used = 1;
				for (int k = j + 1; k < nStrawPointsTempbis[i]; k++) //loop over point
						{
					if (strawPointTempbis_[i][k].nViews == 4 && !strawPointTempbis_[i][k].used) {
						point_dx = fabs(strawPointTempbis_[i][j].x - strawPointTempbis_[i][k].x);
						//						point_dy = fabs(strawPointTempbis_[i][j].y-strawPointTempbis_[i][k].y);
						if (point_dx < 4.0) //point_dx<5.0 or
								{
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
				strawPointFinal_[i][nStrawPointsFinal[i]].clonePoint(pointTemp);
				nStrawPointsFinal[i]++;
			}
			if (strawPointTempbis_[i][j].nViews == 3 && !strawPointTempbis_[i][j].used) {
				//				pointTemp.setPoint(strawPointTempbis_[i][j].z,strawPointTempbis_[i][j].x,strawPointTempbis_[i][j].y,strawPointTempbis_[i][j].trailing,strawPointTempbis_[i][j].viewDistance,strawPointTempbis_[i][j].nViews,strawPointTempbis_[i][j].used);
				pointTemp.clonePoint(strawPointTempbis_[i][j]);
				strawPointTempbis_[i][j].used = 1;
				for (int k = j + 1; k < nStrawPointsTempbis[i]; k++) //loop over point
						{
					if (strawPointTempbis_[i][k].nViews == 3 && !strawPointTempbis_[i][k].used) {
						point_dx = fabs(strawPointTempbis_[i][j].x - strawPointTempbis_[i][k].x);
						//						point_dy = fabs(strawPointTempbis_[i][j].y-strawPointTempbis_[i][k].y);
						if (point_dx < 4.0) //point_dx<5.0 or
								{
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
				strawPointFinal_[i][nStrawPointsFinal[i]].clonePoint(pointTemp);
				nStrawPointsFinal[i]++;
			}
			if (strawPointTempbis_[i][j].nViews == 2) {
				strawPointFinal_[i][nStrawPointsFinal[i]].clonePoint(strawPointTempbis_[i][j]);
				nStrawPointsFinal[i]++;
			}
		}
	}

//	printf("DOPO SELEZIONE PUNTI\n");
	for (int i = 0; i < 4; i++) {
//		printf("camera: %d, n punti= %d \n", i, nStrawPointsFinal[i]);
		for (int j = 0; j < nStrawPointsFinal[i]; j++) {
//			strawPointFinal_[i][j].printPoint2();
			if (nStrawPointsFinal[i] > 0)
				nChambersHit++;
		}
	}
	//		printf("npoint chamber 0 = %d, npoint chamber 1 = %d npoint chamber 3 = %d, npoint chamber 4 = %d\n",
	//				nStrawPointsFinal[0], nStrawPointsFinal[1], nStrawPointsFinal[2],
	//				nStrawPointsFinal[3]);

	////////////////////// TRACK RECONSTRUCTION ////////////////////////////////////
//	LOG_INFO("Track Reconstruction - Hought ");

	float cda = 0.;

	float mtemp;

	int ncam = 0;
	int qyhist = 0;
	int chkcamera = 0;
	int chkcam = 0;
	int nchkcam = 0;
	int addcam1 = 0;
	int addcam2 = 0;

	//verificare
	int nfirsttrk = 0;
	int nfirsttrkcentrali = 0;
	int tempnhitc = 0;
	int tempnhitl = 0;
	int temp2nhitc = 0;

	int naddhit1 = 0;
	int naddhit2 = 0;
	int temphit = 0;
	int tempcamera = 0;
	int chkhit = 0; //chk per verificare se un hit o una camera trovata in una casella laterale è gia stata usata nella casella centrale

	float mpunto = 0.0;
	float qypunto = 0.0;

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

	int ntrk = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < nStrawPointsFinal[i]; j++) //loop over point
				{
			ncam = i;
			for (int a = 0; a < rangem; a++) {
				mtemp = ((float) a - ((float) rangem + 0.1) / 2) * passo;
				qy = strawPointFinal_[i][j].y - mtemp * (strawPointFinal_[i][j].z - zmagnete); //con 0.03 mrad di angolo max qy va da -1424 a 1637, quindi metto 1500 come limite (posso tagliare un poco prima di 0.03 per l'ultima camera)
				//qx = point[i][j].x - mtemp * point[i][j].z; //se si volesse fare un hough con x ma e' complicato con il magnete

				qyhist = qy * rangeq / lmagnete + rangeq / 2; //provo a mettere 3000

				if (qyhist > 0 and qyhist < rangeq and (hought[a][qyhist] >> 60) < 6) {

					//printf("prova pre-incremento a= %d, qyhist= %d, hought[a][qyhist]= %lld\n",a, qyhist, hought[a][qyhist]);
					hought[a][qyhist] |= ((long long) (0x3 & ncam) << ((hought[a][qyhist] >> 60) * 2 + 48)); //12 bits from 48 to 59 with the point chamber number (2 bit for chamber)
					hought[a][qyhist] |= ((((long long) (0xFF & (j))) << ((hought[a][qyhist] >> 60) * 8)) & 0XFFFFFFFFFFFF); //the first 48 bits with up to 6 point of 8 bits ( 255 )
					hought[a][qyhist] += ((long long) 1 << 60); //the 4 most significant bits with the number of points
					//printf("a= %d, qyhist= %d, hought[a][qyhist]= %lld,  ncam=  %d, qy= %f, j= %d, mtemp = %f\n",a, qyhist, hought[a][qyhist], ncam, qy, j, mtemp);

				}
			}
		}
	}

	nfirsttrk = 0;

	for (int a = 0; a < rangem; a++) {
		//	printf("\n");
		for (int b = 0; b < rangeq; b++) {
			//printf (" %lld ",hought[a][b] >> 60);
			if ((hought[a][b] >> 60) > 1) //looking for bin with at least 2 points
					{
				chkcam = 0;
				nchkcam = 4;
				addcam1 = -1;
				addcam2 = -1;
				chkcamera = 0;

				//verificare
				nfirsttrk = 0;
				nfirsttrkcentrali = 0;
				tempnhitc = 0;
				tempnhitl = 0;
				temp2nhitc = 0;

				naddhit1 = 0;
				naddhit2 = 0;
				temphit = 0;
				tempcamera = 0;
				chkhit = 0; //chk per verificare se un hit o una camera trovata in una casella laterale è gia stata usata nella casella centrale

				mpunto = 0.0;
				qypunto = 0.0;

				x0 = 0.0;
				x1 = 0.0;
				x2 = 0.0;
				x3 = 0.0;
				y0 = 0.0;
				y1 = 0.0;
				y2 = 0.0;
				y3 = 0.0;
				z0 = 0.0;
				z1 = 0.0;
				z2 = 0.0;
				z3 = 0.0;
				q01 = 0.0;
				q23 = 0.0;
				dqx = 1000000.0;
				trailingtemp = 0.0;

				for (int c = 0; c < (int) (hought[a][b] >> 60); c++) // check if there are at least 2 hits belonging to different chambers
					chkcam |= 1 << (0X3 & (hought[a][b] >> (48 + 2 * c)));

				for (int d = 0; d < 4; d++)
					if ((0X1 & (chkcam >> d)) == 0) {
						nchkcam--;

						if (addcam1 == -1)
							addcam1 = d;
						else
							addcam2 = d;
					}

				//printf("a=%d, b=%d, nhit=%lld, nchkcam=%d, addcam1=%d, addcam2=%d \n",a, b, hought[a][b] >> 60, nchkcam, addcam1, addcam2);

				if (nchkcam > 1) {
					for (int d = 0; d < (int) (hought[a][b] >> 60); d++) // si creano più tracklet con tutte le combinazioni di hit con camere diverse
							{

						if (((int) pow(2, (int) (0X3 & (hought[a][b] >> (48 + 2 * d)))) & chkcamera) == 0) //verifica se la camera è gia stata usata per questo tracklet: se non è stata usata la aggiungo ad in tracklet esistenti (all'inizio ce ne e' uno vuoto)
								{

							for (int j = 0; j < nfirsttrk + 1; j++) {
								//								strawFirstTempTrk_[j].setHitc(tempnhitc,(int) (0XFF & (hought[a][b] >> (8 * d))));
								//								strawFirstTempTrk_[j].setChamberc(tempnhitc,(int) (0X3 & (hought[a][b] >> (48 + 2 * d))));
								//								strawFirstTempTrk_[j]->Track();

								//								cout<<endl;
								//								cout<<"camera non usata j="<<j<<", tempnhitc="<<tempnhitc<<", a="<<a<<", b="<<b<<", camera="<<(int) (0X3 & (hought[a][b] >> (48 + 2 * d)))<<", hit="<<(int) (0XFF & (hought[a][b] >> (8 * d)))<<endl;

								strawFirstTempTrk_[j].hitc[tempnhitc] = (int) (0XFF & (hought[a][b] >> (8 * d)));
								strawFirstTempTrk_[j].camerec[tempnhitc] = (int) (0X3 & (hought[a][b] >> (48 + 2 * d)));
								//cout << "PIPPOOOOOOOO " << endl;

								//								chkcamera |= (int) pow(2,strawFirstTempTrk_[nfirsttrk].getChamberc(tempnhitc));
								chkcamera |= (int) pow(2, strawFirstTempTrk_[nfirsttrk].camerec[tempnhitc]);

								strawFirstTempTrk_[j].ncentrali = tempnhitc + 1;
								strawFirstTempTrk_[j].nlaterali = 0;

								//								printf("0: j=%d, tempnhitc=%d, firsttemptrk: ",
								//										j, tempnhitc);
								//								strawFirstTempTrk_[j].printTrack();
							}
							tempnhitc++;
						} else //se è gia stata usata si crea un nuovo tracklet con la nuova camera al posto di quell'altra (se soddisfa dei requisiti)
						{
							temp2nhitc = 0;
							nfirsttrk++;
							for (int j = 0; j < tempnhitc; j++) {

								//								cout<<"camera usata nfirsttrk="<<nfirsttrk<<", temp2nhitc="<<temp2nhitc<<", j="<<j<<", tempnhitc="<<tempnhitc<<", camera="<<(int) (0X3 & (hought[a][b] >> (48 + 2 * d)))<<", hit="<<(int) (0XFF & (hought[a][b] >> (8 * d)))<<endl;

								if (strawFirstTempTrk_[nfirsttrk - 1].camerec[j] != (int) (0X3 & (hought[a][b] >> (48 + 2 * d)))) {
									strawFirstTempTrk_[nfirsttrk].hitc[temp2nhitc] = strawFirstTempTrk_[nfirsttrk - 1].hitc[j];
									strawFirstTempTrk_[nfirsttrk].camerec[temp2nhitc] = strawFirstTempTrk_[nfirsttrk - 1].camerec[j];
								} else {
									if (strawPointFinal_[(int) (0X3 & (hought[a][b] >> (48 + 2 * d)))][(int) (0XFF
											& (hought[a][b] >> (8 * d)))].nViews
											> strawPointFinal_[strawFirstTempTrk_[nfirsttrk - 1].camerec[temp2nhitc]][strawFirstTempTrk_[nfirsttrk
													- 1].hitc[temp2nhitc]].nViews) {
										nfirsttrk--;
										strawFirstTempTrk_[nfirsttrk].hitc[temp2nhitc] = (int) (0XFF & (hought[a][b] >> (8 * d)));
										strawFirstTempTrk_[nfirsttrk].camerec[temp2nhitc] = (int) (0X3 & (hought[a][b] >> (48 + 2 * d)));
									} else if (strawPointFinal_[(int) (0X3 & (hought[a][b] >> (48 + 2 * d)))][(int) (0XFF
											& (hought[a][b] >> (8 * d)))].nViews
											== strawPointFinal_[strawFirstTempTrk_[nfirsttrk - 1].camerec[temp2nhitc]][strawFirstTempTrk_[nfirsttrk
													- 1].hitc[temp2nhitc]].nViews) {
										strawFirstTempTrk_[nfirsttrk].hitc[temp2nhitc] = (int) (0XFF & (hought[a][b] >> (8 * d)));
										strawFirstTempTrk_[nfirsttrk].camerec[temp2nhitc] = (int) (0X3 & (hought[a][b] >> (48 + 2 * d)));
									} else
										nfirsttrk--;
								}

								temp2nhitc++;
							}
							strawFirstTempTrk_[nfirsttrk].ncentrali = temp2nhitc;
							strawFirstTempTrk_[nfirsttrk].nlaterali = 0;
						}

						//						printf("\n1 solo casella centrale: nfirsttrk=%d, firsttemptrk: \n",nfirsttrk);
						//						for (int j = 0; j < nfirsttrk + 1; j++)
						//							strawFirstTempTrk_[j].printTrack();

					}
					nfirsttrkcentrali = nfirsttrk;

					//					printf("2: nfirsttrkcentrali=%d, tempnhitc=%d\n",
					//							nfirsttrkcentrali, tempnhitc);
					//Primo step di ricerca di tutte le coincidenze di più camere

					if (tempnhitc > 1 && tempnhitc < 4) //se ci sono meno di 4 hit nella casella centrale, si cercano gli hit mancanti in quelle intorno
							{
						naddhit1 = 0;
						naddhit2 = 0;
						int *addhit1 = new int[50];
						int *addhit2 = new int[50];

						for (int h = -1; h < 2; h++)
							for (int k = -1; k < 2; k++)
								if (a + h > -1 && a + h < rangem && b + k > -1 && b + k < 200)
									for (int l = 0; l < (hought[a + h][b + k] >> 60); l++) {
										temphit = (int) (0XFF & (hought[a + h][b + k] >> (8 * l)));
										tempcamera = (int) (0X3 & (hought[a + h][b + k] >> (48 + 2 * l)));
										chkhit = 0;

										//terza e nuova soluzione

										if (tempcamera == addcam1) //look if there already are the hit in addhit1[]
												{
											for (int d = 0; d < naddhit1; d++)
												if (addhit1[d] == temphit)
													chkhit = 1;

											if (chkhit == 0) {
												addhit1[naddhit1] = temphit;
												naddhit1++;
											}
										}
										if (tempcamera == addcam2) {
											for (int d = 0; d < naddhit2; d++)
												if (addhit2[d] == temphit)
													chkhit = 1;

											if (chkhit == 0) {
												addhit2[naddhit2] = temphit;
												naddhit2++;
											}
										}
									}

						//						printf("3: nfirsttrkcentrali=%d, naddhit1=%d, naddhit2=%d\n",
						//								nfirsttrkcentrali, naddhit1, naddhit2);

						if (naddhit1 > 0) {
							for (int j = 0; j < nfirsttrkcentrali + 1; j++) {
								strawFirstTempTrk_[j].hitl[tempnhitl] = addhit1[0];
								strawFirstTempTrk_[j].camerel[tempnhitl] = addcam1;
								strawFirstTempTrk_[j].nlaterali = tempnhitl + 1;
							}

							for (int d = 1; d < naddhit1; d++)
								for (int j = 0; j < nfirsttrkcentrali + 1; j++) {
									nfirsttrk++;
									strawFirstTempTrk_[nfirsttrk].copyTrack(strawFirstTempTrk_[j]);
									strawFirstTempTrk_[nfirsttrk].hitl[tempnhitl] = addhit1[d];

									//									printf("4: j=%d, nfirsttrk=%d, firsttemptrk: \n",j, nfirsttrk);
									//									strawFirstTempTrk_[nfirsttrk].printTrack();
								}
							tempnhitl++;
						}
						nfirsttrkcentrali = nfirsttrk; //sono aumentati in seguito all'addhit1

						if (naddhit2 > 0) {
							for (int j = 0; j < nfirsttrkcentrali + 1; j++) {
								strawFirstTempTrk_[j].hitl[tempnhitl] = addhit2[0];
								strawFirstTempTrk_[j].camerel[tempnhitl] = addcam2;
								strawFirstTempTrk_[j].nlaterali = tempnhitl + 1;
							}

							for (int d = 1; d < naddhit2; d++)
								for (int j = 0; j < nfirsttrkcentrali + 1; j++) {
									nfirsttrk++;
									strawFirstTempTrk_[nfirsttrk].copyTrack(strawFirstTempTrk_[j]);
									strawFirstTempTrk_[nfirsttrk].hitl[tempnhitl] = addhit2[d];

									//									printf("5: j=%d, nfirsttrk=%d, firsttemptrk: \n",j, nfirsttrk);
									//									strawFirstTempTrk_[nfirsttrk].printTrack();
								}
							tempnhitl++;
						}

						delete[] addhit1;
						delete[] addhit2;
					}

					//now I have all the tracklet find in the bin, I have to select only the real one

					mpunto = ((float) a - ((float) rangem + 0.1) / 2) * passo;
					//qypunto = b * 10 - 1000;
					qypunto = b * lmagnete / rangeq - lmagnete / 2;

					//	zvertex = 197645 - (qypunto / mpunto); //oppure shiftati perchè zvertex = 197645-((qypunto+4.762)/(mpunto+0.0001989));

					for (int j = 0; j < nfirsttrk + 1; j++) {
						//parte X

						x0 = 0.0;
						x1 = 0.0;
						x2 = 0.0;
						x3 = 0.0;
						y0 = 0.0;
						y1 = 0.0;
						y2 = 0.0;
						y3 = 0.0;
						z0 = 0.0;
						z1 = 0.0;
						z2 = 0.0;
						z3 = 0.0;
						q01 = 0.0;
						q23 = 0.0;
						dqx = 1000000.0;
						trailingtemp = 0.0;

						//						printf("\n 6: ciclo tracce per aggiungerci le proprieta': a= %d, b= %d, j= %d\n", a, b, j);
						//							strawFirstTempTrk_[j].printTrack();

						if (strawFirstTempTrk_[j].ncentrali + strawFirstTempTrk_[j].nlaterali > 2) {
							for (int z = 0; z < tempnhitc; z++) {
								trailingtemp += strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].trailing;

								if (strawFirstTempTrk_[j].camerec[z] == 0) {
									x0 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].x;
									y0 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z0 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].z - zmagnete;
								}
								if (strawFirstTempTrk_[j].camerec[z] == 1) {
									x1 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].x;
									y1 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z1 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].z - zmagnete;
								}
								if (strawFirstTempTrk_[j].camerec[z] == 2) {
									x2 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].x;
									y2 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z2 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].z - zmagnete;
								}
								if (strawFirstTempTrk_[j].camerec[z] == 3) {
									x3 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].x;
									y3 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z3 = strawPointFinal_[strawFirstTempTrk_[j].camerec[z]][strawFirstTempTrk_[j].hitc[z]].z - zmagnete;
								}
							}
							for (int z = 0; z < tempnhitl; z++) {
								trailingtemp += strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].trailing;

								if (strawFirstTempTrk_[j].camerel[z] == 0) {
									x0 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].x;
									y0 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z0 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].z - zmagnete;
								}
								if (strawFirstTempTrk_[j].camerel[z] == 1) {
									x1 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].x;
									y1 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z1 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].z - zmagnete;
								}
								if (strawFirstTempTrk_[j].camerel[z] == 2) {
									x2 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].x;
									y2 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z2 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].z - zmagnete;
								}
								if (strawFirstTempTrk_[j].camerel[z] == 3) {
									x3 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].x;
									y3 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitc[z]].y;
									z3 = strawPointFinal_[strawFirstTempTrk_[j].camerel[z]][strawFirstTempTrk_[j].hitl[z]].z - zmagnete;
								}
							}

							if (z0 == 0 || z1 == 0) {

								q23 = x2 - z2 * (x3 - x2) / (z3 - z2);
								q01 = q23;

								strawFirstTempTrk_[j].m2x = (x3 - x2) / (z3 - z2);
								if (z0 == 0)
									strawFirstTempTrk_[j].m1x = (x1 - q01) / z1;
								else
									strawFirstTempTrk_[j].m1x = (x0 - q01) / z0;
							} else if (z2 == 0 || z3 == 0) {
								q01 = x0 - z0 * (x1 - x0) / (z1 - z0);
								q23 = q01;

								strawFirstTempTrk_[j].m1x = (x1 - x0) / (z1 - z0);
								if (z2 == 0)
									strawFirstTempTrk_[j].m2x = (x3 - q23) / z3;
								else
									strawFirstTempTrk_[j].m2x = (x2 - q23) / z2;
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

							strawFirstTempTrk_[j].my = mpunto;
							strawFirstTempTrk_[j].qy = qypunto;
							strawFirstTempTrk_[j].trailing = trailingtemp / (tempnhitc + tempnhitl);

							qtrack.setPoint(0.0, strawFirstTempTrk_[j].q1x, strawFirstTempTrk_[j].qy, 0.0, 0.0, 0, 0);
							mtrack.setPoint(1.0, strawFirstTempTrk_[j].m1x, strawFirstTempTrk_[j].my, 0.0, 0.0, 0, 0);

							cdaVertex(qbeam, qtrack, mbeam, mtrack, &cda, &vertex);

							strawFirstTempTrk_[j].zvertex = vertex.z;
							strawFirstTempTrk_[j].cda = cda;

							//							printf("\nfine creazione tracce: a= %d, b= %d, j= %d\n", a, b, j);
							//							strawFirstTempTrk_[j].printTrack();

							//tagli sulle tracce per sfoltirle
							//	if((strawFirstTempTrk_[j].q2x - strawFirstTempTrk_[j].q1x) > tagliodqx1 && (strawFirstTempTrk_[j].q2x - strawFirstTempTrk_[j].q1x) < tagliodqx2)
							//                                 if(strawFirstTempTrk_[j].zvertex > zlowcut && strawFirstTempTrk_[j].zvertex < zhighcut)
							//		if((strawFirstTempTrk_[j].m2x * muv2z + strawFirstTempTrk_[j].q2x) > -muv2x && (strawFirstTempTrk_[j].m2x * muv2z + strawFirstTempTrk_[j].q2x) < muv2x && (strawFirstTempTrk_[j].my * muv2z + strawFirstTempTrk_[j].qy) > -muv2y && (strawFirstTempTrk_[j].my * muv2z + strawFirstTempTrk_[j].qy) < muv2y)
							//                                     if(strawFirstTempTrk_[j].ncentrali + strawFirstTempTrk_[j].nlaterali == 3 || (strawFirstTempTrk_[j].pz > lowpz && strawFirstTempTrk_[j].pz < highpz))
							//                                       if(strawFirstTempTrk_[j].cda < cdacut)
							//if(strawFirstTempTrk_[j].trailing > tracklowtrailing && strawFirstTempTrk_[j].trailing < trackhightrailing)
							//	{

							//pz>3 Gev <100Gev, my e mx1 < 0.020
							if (strawFirstTempTrk_[j].pz > 3000 and strawFirstTempTrk_[j].pz < 100000 and strawFirstTempTrk_[j].my < 0.020
									and strawFirstTempTrk_[j].m1x < 0.020) {
								strawTempTrk_[ntrk].copyTrack(strawFirstTempTrk_[j]);
								ntrk++;
							}

							//	}

						}
					}

				}

				//					printf ("a= %d, b= %d, hought[a][b]= %lld,  chkcam=  %d, nchkcam= %d, addcam1= %d, addcam2= %d\n",a,b,hought[a][b],chkcam,nchkcam,addcam1,addcam2);
				//					cout<<"ievent= "<<iEvent<<" a= "<<a<<" b= "<<b<<" hought[a][b]= "<<hought[a][b]<<", chkcam= "<<chkcam<<", nchkcam= "<<nchkcam<<", addcam1= "<<addcam1<<", addcam2= "<<addcam2<<endl;
			}
			//					cout<<"event="<<iEvent<<" a="<<a<<" b="<<b<<" "<<nfirsttrk<<" ntrk="<<ntrk<<endl;
		}
		//			cout<<"event="<<iEvent<<" a="<<a<<" "<<nfirsttrk<<" ntrk="<<ntrk<<endl;
	}

//	printf("\n   prime tracce: n=%d\n", ntrk);
//	for (int e = 0; e < ntrk; e++) {
//		printf("\n traccia: n = %d\n", e);
//		strawTempTrk_[e].printTrack();
	//			if(strawTempTrk_[e].ncentrali+strawTempTrk_[e].nlaterali==4)
	//				FillHisto("track_deltaqx",(strawTempTrk_[e].q2x-strawTempTrk_[e].q1x));
//	}

	ntracletcondivisi = 0;
	ntrkintermedie = 0;
	ntrkfinali = 0;

//	cout << endl;
//	cout << " SELEZIONE DEI TRACKLETS" << endl;

	//prima uso le tracce con 4 hit
	for (int e = 0; e < ntrk; e++) {
		//printf ("prendo il tracklet %d\n",e);
		//strawTempTrk_[e].printTrack();

		trkintermedietemp_my = 0.0;
		trkintermedietemp_qy = 0.0;
		trkintermedietemp_m1x = 0.0;
		trkintermedietemp_q1x = 0.0;
		trkintermedietemp_m2x = 0.0;
		trkintermedietemp_q2x = 0.0;

		if (strawTempTrk_[e].usato == 0 && strawTempTrk_[e].ncentrali + strawTempTrk_[e].nlaterali == 4) //select the best temporary track between the similar track with 4 point
				{
			trkintermedietemp.copyTrack(strawTempTrk_[e]);

			//					printf ("visto che la traccia %d ha 4 hit faccio copia temporanea\n",e);
			//					trkintermedietemp.printTrack();

			if ((strawTempTrk_[e].q2x - strawTempTrk_[e].q1x) < -13 or (strawTempTrk_[e].q2x - strawTempTrk_[e].q1x) > 0)
				for (int f = e; f < ntrk; f++) {

					//						printf ( "\n controllo con la traccia: n = %d\n",f);
					//						strawTempTrk_[f].printTrack();

					tempcondivise = 0;
					if (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali == 4) {
						for (int g = 0; g < trkintermedietemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
							for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

								//printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

								if (trkintermedietemp.hitc[g] == strawTempTrk_[f].hitc[h]
										and trkintermedietemp.camerec[g] == strawTempTrk_[f].camerec[h])
									tempcondivise++;
							}
							for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

								//printf ( "hitc[g]=%d, hitl[h]=%d, camerec[g]=%d, camerel[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitl[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerel[h],tempcondivise);

								if (trkintermedietemp.hitc[g] == strawTempTrk_[f].hitl[h]
										and trkintermedietemp.camerec[g] == strawTempTrk_[f].camerel[h])
									tempcondivise++;
							}
						}

						for (int g = 0; g < trkintermedietemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
							for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

								//printf ( "hitl[g]=%d, hitc[h]=%d, camerel[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitl[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerel[g], strawTempTrk_[f].camerec[h],tempcondivise);

								if (trkintermedietemp.hitl[g] == strawTempTrk_[f].hitc[h]
										and trkintermedietemp.camerel[g] == strawTempTrk_[f].camerec[h])
									tempcondivise++;
							}
							for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

								//printf ( "hitl[g]=%d, hitl[h]=%d, camerel[g]=%d, camerel[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitl[g], strawTempTrk_[f].hitl[h], trkintermedietemp.camerel[g], strawTempTrk_[f].camerel[h],tempcondivise);

								if (trkintermedietemp.hitl[g] == strawTempTrk_[f].hitl[h]
										and trkintermedietemp.camerel[g] == strawTempTrk_[f].camerel[h])
									tempcondivise++;
							}
						}

//						printf("risultato: tempcondivise=%d\n", tempcondivise);

						//if((tempcondivise > 1 && fabs(strawTempTrk_[f].q2x - strawTempTrk_[f].q1x) < fabs(trkintermedietemp.q2x - trkintermedietemp.q1x)) || (tempcondivise == 4 && strawTempTrk_[f].ncentrali > trkintermedietemp.ncentrali))
						//if(tempcondivise > 1 && (strawTempTrk_[f].ncentrali > trkintermedietemp.ncentrali or (strawTempTrk_[f].ncentrali == trkintermedietemp.ncentrali and fabs(strawTempTrk_[f].q2x - strawTempTrk_[f].q1x) < fabs(trkintermedietemp.q2x - trkintermedietemp.q1x))))
						if (tempcondivise > 1
								&& (((strawTempTrk_[f].q2x - strawTempTrk_[f].q1x) > -13
										and (strawTempTrk_[f].q2x - strawTempTrk_[f].q1x) < 0)
										or strawTempTrk_[f].ncentrali > trkintermedietemp.ncentrali
										or (strawTempTrk_[f].ncentrali == trkintermedietemp.ncentrali
												and fabs(strawTempTrk_[f].q2x - strawTempTrk_[f].q1x + 3)
														< fabs(trkintermedietemp.q2x - trkintermedietemp.q1x + 3)))) {
							trkintermedietemp.copyTrack(strawTempTrk_[f]);

							//								printf ( "nuova traccia intermedia temporanea copia della n=%d\n",f);
							//								trkintermedietemp.printTrack();

						}
					}
				}

			ntracletcondivisi = 0;

			for (int f = 0; f < ntrk; f++) //the tracks with the same point will be combined
					{
				tempcondivise = 0;
				for (int g = 0; g < trkintermedietemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

						//printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitc[g] == strawTempTrk_[f].hitc[h]
								and trkintermedietemp.camerec[g] == strawTempTrk_[f].camerec[h])
							tempcondivise++;
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

						//printf ( "hitc[g]=%d, hitl[h]=%d, camerec[g]=%d, camerel[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitl[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerel[h],tempcondivise);

						if (trkintermedietemp.hitc[g] == strawTempTrk_[f].hitl[h]
								and trkintermedietemp.camerec[g] == strawTempTrk_[f].camerel[h])
							tempcondivise++;
					}
				}

				for (int g = 0; g < trkintermedietemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

						//printf ( "hitc[g]=%d, hitc[h]=%d, camerel[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitl[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerel[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitl[g] == strawTempTrk_[f].hitc[h]
								and trkintermedietemp.camerel[g] == strawTempTrk_[f].camerec[h])
							tempcondivise++;
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

						//printf ( "hitl[g]=%d, hitl[h]=%d, camerel[g]=%d, camerel[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitl[g], strawTempTrk_[f].hitl[h], trkintermedietemp.camerel[g], strawTempTrk_[f].camerel[h],tempcondivise);

						if (trkintermedietemp.hitl[g] == strawTempTrk_[f].hitl[h]
								and trkintermedietemp.camerel[g] == strawTempTrk_[f].camerel[h])
							tempcondivise++;
					}
				}

				//printf ( "tempcondivise=%d, nhit=%d\n",tempcondivise,(strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali));

				if (tempcondivise == (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali)) {
					ntracletcondivisi++;
					trkintermedietemp_my += strawTempTrk_[f].my;
					trkintermedietemp_qy += strawTempTrk_[f].qy;
					trkintermedietemp_m1x += strawTempTrk_[f].m1x;
					trkintermedietemp_q1x += strawTempTrk_[f].q1x;
					trkintermedietemp_m2x += strawTempTrk_[f].m2x;
					trkintermedietemp_q2x += strawTempTrk_[f].q2x;

					strawTempTrk_[f].usato = 1;
				} else {
					if (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali == 4 && tempcondivise > 1) {
						strawTempTrk_[f].usato = 1;
					} else if (strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali == 3 && tempcondivise > 0) {
						strawTempTrk_[f].usato = 1;
					}
				}
			}

//			printf("ntracletcondivisi=%d\n", ntracletcondivisi);

			trkintermedietemp.my = trkintermedietemp_my / ntracletcondivisi;
			trkintermedietemp.qy = trkintermedietemp_qy / ntracletcondivisi;
			trkintermedietemp.m1x = trkintermedietemp_m1x / ntracletcondivisi;
			trkintermedietemp.q1x = trkintermedietemp_q1x / ntracletcondivisi;
			trkintermedietemp.m2x = trkintermedietemp_m2x / ntracletcondivisi;
			trkintermedietemp.q2x = trkintermedietemp_q2x / ntracletcondivisi;

			strawTrkIntermedie_[ntrkintermedie].copyTrack(trkintermedietemp);
			strawTrkIntermedie_[ntrkintermedie].pz = 270 / (fabs(trkintermedietemp.m2x - trkintermedietemp.m1x));

			qtrack.setPoint(0.0, trkintermedietemp.q1x, trkintermedietemp.qy, 0.0, 0.0, 0, 0);
			mtrack.setPoint(1.0, trkintermedietemp.m1x, trkintermedietemp.my, 0.0, 0.0, 0, 0);

			cdaVertex(qbeam, qtrack, mbeam, mtrack, &cda, &vertex);

			strawTrkIntermedie_[ntrkintermedie].zvertex = vertex.z;
			strawTrkIntermedie_[ntrkintermedie].cda = cda;
			strawTrkIntermedie_[ntrkintermedie].ncondivisi = 0;
			strawTrkIntermedie_[ntrkintermedie].ncamcondivise = 0;
			strawTrkIntermedie_[ntrkintermedie].usato = 0;

			//printf ( "traccia intermedia (4) my=%f, qy=%f\n",trkintermedietemp.my,trkintermedietemp.qy);
			//printf ( "traccia intermedia (4) %d, copia della temporanea\n",ntrkintermedie);
			//strawTrkIntermedie_[ntrkintermedie].printTrack();

			ntrkintermedie++;

		}
	}

//	printf("\n  secondo ciclo per i casi a 3 hit\n");

	for (int e = 0; e < ntrk; e++) //the same with tracks with only 3 points
			{

		//printf ("prendo il tracklet %d\n",e);
		//strawTempTrk_[e].printTrack();

		trkintermedietemp_my = 0.0;
		trkintermedietemp_qy = 0.0;
		trkintermedietemp_m1x = 0.0;
		trkintermedietemp_q1x = 0.0;
		trkintermedietemp_m2x = 0.0;
		trkintermedietemp_q2x = 0.0;

		if (strawTempTrk_[e].usato == 0 && strawTempTrk_[e].ncentrali + strawTempTrk_[e].nlaterali == 3) {
			trkintermedietemp.copyTrack(strawTempTrk_[e]);

			//			printf ( "traccia intermedia temporanea copia della n=%d\n",e);
			//			trkintermedietemp.printTrack();

			for (int f = 0; f < ntrk; f++) {
				tempcondivise = 0;
				for (int g = 0; g < trkintermedietemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {
						//						printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitc[g] == strawTempTrk_[f].hitc[h]
								and trkintermedietemp.camerec[g] == strawTempTrk_[f].camerec[h])
							tempcondivise++;
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

						//						printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitc[g] == strawTempTrk_[f].hitl[h]
								and trkintermedietemp.camerec[g] == strawTempTrk_[f].camerel[h])
							tempcondivise++;
					}
				}

				for (int g = 0; g < trkintermedietemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

						//						printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitl[g] == strawTempTrk_[f].hitc[h]
								and trkintermedietemp.camerel[g] == strawTempTrk_[f].camerec[h])
							tempcondivise++;
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

						//						printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitl[g] == strawTempTrk_[f].hitl[h]
								and trkintermedietemp.camerel[g] == strawTempTrk_[f].camerel[h])
							tempcondivise++;
					}
				}

				if (tempcondivise > 1
						&& (strawTempTrk_[f].ncentrali > trkintermedietemp.ncentrali
								|| (strawTempTrk_[f].cda < trkintermedietemp.cda
										&& strawTempTrk_[f].ncentrali == trkintermedietemp.ncentrali)))
					trkintermedietemp.copyTrack(strawTempTrk_[f]);
			}

			ntracletcondivisi = 0;

			for (int f = 0; f < ntrk; f++) {
				tempcondivise = 0;
				for (int g = 0; g < trkintermedietemp.ncentrali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

						//						printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitc[g] == strawTempTrk_[f].hitc[h]
								and trkintermedietemp.camerec[g] == strawTempTrk_[f].camerec[h])
							tempcondivise++;
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

						//						printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitc[g] == strawTempTrk_[f].hitl[h]
								and trkintermedietemp.camerec[g] == strawTempTrk_[f].camerel[h])
							tempcondivise++;
					}
				}

				for (int g = 0; g < trkintermedietemp.nlaterali && strawTempTrk_[f].usato == 0; g++) {
					for (int h = 0; h < strawTempTrk_[f].ncentrali; h++) {

						//						printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitl[g] == strawTempTrk_[f].hitc[h]
								and trkintermedietemp.camerel[g] == strawTempTrk_[f].camerec[h])
							tempcondivise++;
					}
					for (int h = 0; h < strawTempTrk_[f].nlaterali; h++) {

						//						printf ( "hitc[g]=%d, hitc[h]=%d, camerec[g]=%d, camerec[h]=%d, tempcondivise=%d\n",trkintermedietemp.hitc[g], strawTempTrk_[f].hitc[h], trkintermedietemp.camerec[g], strawTempTrk_[f].camerec[h],tempcondivise);

						if (trkintermedietemp.hitl[g] == strawTempTrk_[f].hitl[h]
								and trkintermedietemp.camerel[g] == strawTempTrk_[f].camerel[h])
							tempcondivise++;
					}
				}

				//				printf ( "tempcondivise=%d, nhit=%d\n",tempcondivise,(strawTempTrk_[f].ncentrali + strawTempTrk_[f].nlaterali));

				if (tempcondivise == (trkintermedietemp.ncentrali + trkintermedietemp.nlaterali)) {
					ntracletcondivisi++;
					trkintermedietemp_my += strawTempTrk_[f].my;
					trkintermedietemp_qy += strawTempTrk_[f].qy;
					trkintermedietemp_m1x += strawTempTrk_[f].m1x;
					trkintermedietemp_q1x += strawTempTrk_[f].q1x;
					trkintermedietemp_m2x += strawTempTrk_[f].m2x;
					trkintermedietemp_q2x += strawTempTrk_[f].q2x;

					strawTempTrk_[f].usato = 1;
				} else {
					if (trkintermedietemp.ncentrali == 3 && strawTempTrk_[f].ncentrali == 2 && tempcondivise > 0) {
						strawTempTrk_[f].usato = 1;

					} else if (tempcondivise > 1) {
						strawTempTrk_[f].usato = 1;

					}
				}
			}

			//			printf ( "ntracletcondivisi=%d\n",ntracletcondivisi);

			trkintermedietemp.my = trkintermedietemp_my / ntracletcondivisi;
			trkintermedietemp.qy = trkintermedietemp_qy / ntracletcondivisi;
			trkintermedietemp.m1x = trkintermedietemp_m1x / ntracletcondivisi;
			trkintermedietemp.q1x = trkintermedietemp_q1x / ntracletcondivisi;
			trkintermedietemp.m2x = trkintermedietemp_m2x / ntracletcondivisi;
			trkintermedietemp.q2x = trkintermedietemp_q2x / ntracletcondivisi;

			strawTrkIntermedie_[ntrkintermedie].copyTrack(trkintermedietemp);
			strawTrkIntermedie_[ntrkintermedie].pz = 270 / (fabs(trkintermedietemp.m2x - trkintermedietemp.m1x));

			qtrack.setPoint(0.0, trkintermedietemp.q1x, trkintermedietemp.qy, 0.0, 0.0, 0, 0);
			mtrack.setPoint(1.0, trkintermedietemp.m1x, trkintermedietemp.my, 0.0, 0.0, 0, 0);

			cdaVertex(qbeam, qtrack, mbeam, mtrack, &cda, &vertex);

			strawTrkIntermedie_[ntrkintermedie].zvertex = vertex.z;
			strawTrkIntermedie_[ntrkintermedie].cda = cda;
			strawTrkIntermedie_[ntrkintermedie].ncondivisi = 0;
			strawTrkIntermedie_[ntrkintermedie].ncamcondivise = 0;
			strawTrkIntermedie_[ntrkintermedie].usato = 0;

			//printf ( "traccia intermedia (3) my=%f, qy=%f\n",trkintermedietemp.my,trkintermedietemp.qy);
			//printf ( "traccia intermedia (3) %d, copia della temporanea\n",ntrkintermedie);
			//strawTrkIntermedie_[ntrkintermedie].printTrack();

			ntrkintermedie++;

		}
	}

//	printf("\n   tracce medie: n=%d\n", ntrkintermedie);
//	for (int e = 0; e < ntrkintermedie; e++) {
//		printf("\n traccia: n = %d\n", e);
//		strawTrkIntermedie_[e].printTrack();
//	}

	//casi a 3 tracce
	Point mtrack1;
	Point qtrack1;
	Point mtrack2;
	Point qtrack2;

	/*for (int e = 0; e < ntrkintermedie; e++) {

	 flag_l1_limit[e] = 0;
	 flag_l1_three[e] = 0;
	 }*/

	for (int e = 0; e < ntrkintermedie; e++) {
		if (e < 5) {
			l1Info->setL1StrawTrack_P(e, strawTrkIntermedie_[e].pz);
			l1Info->setL1StrawTrack_Vz(e, strawTrkIntermedie_[e].zvertex);
		}
		if (strawTrkIntermedie_[e].zvertex > -100000 and strawTrkIntermedie_[e].zvertex < 180000 and strawTrkIntermedie_[e].cda < 200
				and strawTrkIntermedie_[e].pz < 50000) {
//			printf("cut: traccia %d ha passato i tagli\n", e);
			flag_l1_limit[e]++;
		} else
//			printf("cut: traccia %d è stata taglia via\n", e);

			qtrack1.setPoint(0.0, strawTrkIntermedie_[e].q1x, strawTrkIntermedie_[e].qy, 0.0, 0.0, 0, 0);
		mtrack1.setPoint(1.0, strawTrkIntermedie_[e].m1x, strawTrkIntermedie_[e].my, 0.0, 0.0, 0, 0);

		for (int f = e + 1; f < ntrkintermedie; f++) {
			qtrack2.setPoint(0.0, strawTrkIntermedie_[f].q1x, strawTrkIntermedie_[f].qy, 0.0, 0.0, 0, 0);
			mtrack2.setPoint(1.0, strawTrkIntermedie_[f].m1x, strawTrkIntermedie_[f].my, 0.0, 0.0, 0, 0);

			cdaVertex(qtrack1, qtrack2, mtrack1, mtrack2, &cda, &vertex);
			if (cda < 30) {
				flag_l1_three[e]++;
				flag_l1_three[f]++;
//				printf("evento a 3 traccie: traccia %d e %d, cda = %f, vertice (%f, %f, %f)\n", e, f, cda, vertex.z, vertex.x, vertex.y);
			}
		}

		if (strawTrkIntermedie_[e].m1x - strawTrkIntermedie_[e].m2x < 0)
			flag_l1_exotic = 1;

		if (flag_l1_limit[e] > 0 and flag_l1_three[e] == 0)
			flag_l1 = 1;

		if (flag_l1_limit[e] > 0 and flag_l1_three[e] > 0)
			flag_l1_tretracks = 1;
	}

//	LOG_INFO("\n RISULTATO:");
//	if (flag_l1 == 1)
//		LOG_INFO("                Evento buono \n");
//
//	else
//		LOG_INFO("                Evento tagliato \n");
//
//	if (flag_l1_tretracks == 1)
//		LOG_INFO("                Evento a tre traccie \n");

	l1Info->setL1StrawNTracks(ntrkintermedie);
	l1Info->setL1StrawProcessed();
	return flag_l1; //return the Straw Trigger word!
	//return flag_l1_exotic; //return the Straw exotic Trigger word!
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

	posizione = fStrawSpacing * jstraw + xOffset[view][plane] + xoffch[chamber][view];

	return posizione;

}

int StrawAlgo::strawAcceptance(int n, double *coor, int zone) {

	float fViewSize = 2100.0;
	float fStrawSpacing = 17.6;
	double StrawInnerRadius = 4.875;
	double CopperThickness = 0.00005;
	double MylarThickness = 0.036;
	double GoldThickness = 0.00002;
	double StrawRadius = StrawInnerRadius + 2 * CopperThickness + MylarThickness + GoldThickness;
	double fStrawDiameter = 2 * StrawRadius;

	float fHoleChamberMax[4][4];
	float fHoleChamberMin[4][4];

	fHoleChamberMax[0][0] = 134.2;
	fHoleChamberMax[0][1] = 134.2;
	fHoleChamberMax[0][2] = 165.0;
	fHoleChamberMax[0][3] = 63.8;
	fHoleChamberMin[0][0] = 6.6;
	fHoleChamberMin[0][1] = 6.6;
	fHoleChamberMin[0][2] = 37.4;
	fHoleChamberMin[0][3] = -63.8;
	fHoleChamberMax[1][0] = 143.0;
	fHoleChamberMax[1][1] = 143.0;
	fHoleChamberMax[1][2] = 178.2;
	fHoleChamberMax[1][3] = 63.8;
	fHoleChamberMin[1][0] = 15.4;
	fHoleChamberMin[1][1] = 15.4;
	fHoleChamberMin[1][2] = 50.6;
	fHoleChamberMin[1][3] = -63.8;
	fHoleChamberMax[2][0] = 129.8;
	fHoleChamberMax[2][1] = 129.8;
	fHoleChamberMax[2][2] = 156.2;
	fHoleChamberMax[2][3] = 63.8;
	fHoleChamberMin[2][0] = 2.2;
	fHoleChamberMin[2][1] = 2.2;
	fHoleChamberMin[2][2] = 28.6;
	fHoleChamberMin[2][3] = -63.8;
	fHoleChamberMax[3][0] = 103.4;
	fHoleChamberMax[3][1] = 103.4;
	fHoleChamberMax[3][2] = 116.6;
	fHoleChamberMax[3][3] = 63.8;
	fHoleChamberMin[3][0] = -24.2;
	fHoleChamberMin[3][1] = -24.2;
	fHoleChamberMin[3][2] = -11.0;
	fHoleChamberMin[3][3] = -63.8;

	float fViewPlaneTransverseSize = (120 - 1) * fStrawSpacing + fStrawDiameter;

// View definition
	double invsq2 = 1. / sqrt(2.);
	double a[4] = { invsq2, invsq2, 1, 0 };
	double b[4] = { invsq2, -invsq2, 0, 1 };
	double c[4] = { invsq2, invsq2, 0, 1 };
	double d[4] = { -invsq2, invsq2, 1, 0 };
	int viewflag[4] = { 0, 0, 0, 0 };
	for (int jView = 0; jView < 4; jView++) {
		double posView = a[jView] * coor[0] + b[jView] * coor[1];
		double posAlongStraw = c[jView] * coor[0] + d[jView] * coor[1];
		if (((posView > fHoleChamberMax[n][jView] && posView < 0.5 * fViewPlaneTransverseSize)
				|| (posView < fHoleChamberMin[n][jView] && posView > -0.5 * fViewPlaneTransverseSize))
				&& fabs(posAlongStraw) < 0.5 * fViewSize)
			viewflag[jView] = 1;
	}
	int Vu = viewflag[0];
	int Vv = viewflag[1];
	int Vx = viewflag[2];
	int Vy = viewflag[3];

// Zones
	switch (zone) {
	case 1:  // At least 1 view
		if (Vx || Vy || Vu || Vv)
			return 1;
		return 0;
	case 2:  // At least 2 views
		if ((Vx && Vy) || (Vx && Vu) || (Vx && Vv) || (Vy && Vu) || (Vy && Vv) || (Vu && Vv))
			return 1;
		return 0;
	case 3:  // At least 3 views
		if ((Vx && Vy && Vu) || (Vx && Vy && Vv) || (Vx && Vu && Vv) || (Vy && Vu && Vv))
			return 1;
		return 0;
	case 4:  // Four views only
		if (Vx && Vy && Vu && Vv)
			return 1;
		return 0;
	case 11:  // One view only
		if (((Vx && !Vy && !Vu && !Vv) || (!Vx && Vy && !Vu && !Vv) || (!Vx && !Vy && Vu && !Vv) || (!Vx && !Vy && !Vu && Vv)))
			return 1;
		return 0;
	case 12:  // Two views only
		if (((Vx && Vy && !Vu && !Vv) || (Vx && !Vy && Vu && !Vv) || (Vx && !Vy && !Vu && Vv) || (!Vx && Vy && Vu && !Vv)
				|| (!Vx && Vy && !Vu && Vv) || (!Vx && !Vy && Vu && Vv)))
			return 1;
		return 0;
	case 13:  // Three views only
		if (((Vx && Vy && Vu && !Vv) || (Vx && Vy && !Vu && Vv) || (Vx && !Vy && Vu && Vv) || (!Vx && Vy && Vu && Vv)))
			return 1;
		break;
	default:
		return 0;
	}

	return 0;
}

int StrawAlgo::cdaVertex(Point qbeam, Point qtrack, Point mbeam, Point mtrack, float* cda, Point* vertex) {

	Point r12;
	float t1, t2, aa, bb, cc, dd, ee, det;
	Point q1, q2;

	r12.z = qbeam.z - qtrack.z;
	r12.x = qbeam.x - qtrack.x;
	r12.y = qbeam.y - qtrack.y;

	aa = mbeam.x * mbeam.x + mbeam.y * mbeam.y + mbeam.z * mbeam.z;
	bb = mtrack.x * mtrack.x + mtrack.y * mtrack.y + mtrack.z * mtrack.z;
	cc = mbeam.x * mtrack.x + mbeam.y * mtrack.y + mbeam.z * mtrack.z;
	dd = r12.x * mbeam.x + r12.y * mbeam.y + r12.z * mbeam.z;
	ee = r12.x * mtrack.x + r12.y * mtrack.y + r12.z * mtrack.z;
	det = cc * cc - aa * bb;

	t1 = (bb * dd - cc * ee) / det;
	t2 = (cc * dd - aa * ee) / det;

	q1.z = qbeam.z + t1 * mbeam.z;
	q1.x = qbeam.x + t1 * mbeam.x;
	q1.y = qbeam.y + t1 * mbeam.y;
	q2.z = qtrack.z + t2 * mtrack.z;
	q2.x = qtrack.x + t2 * mtrack.x;
	q2.y = qtrack.y + t2 * mtrack.y;

	vertex->setPoint(zmagnete + (q1.z + q2.z) / 2, (q1.x + q2.x) / 2, (q1.y + q2.y) / 2, 0.0, 0.0, 0, 0);
	r12.setPoint(q1.z - q2.z, q1.x - q2.x, q1.y - q2.y, 0.0, 0.0, 0, 0);

	*cda = sqrt(r12.x * r12.x + r12.y * r12.y + r12.z * r12.z);

	return 0;

}

void StrawAlgo::writeData(L1StrawAlgo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID written within the packet!");
	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1StrawProcessed() << 6) | (l1Info->isL1StrawEmptyPacket() << 4) | (l1Info->isL1StrawBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1StrawProcessed() << 6) | (l1Info->isL1StrawEmptyPacket() << 4)
			| (l1Info->isL1StrawBadData() << 2) | ((uint) l1Info->getL1StrawTrgWrd());
	for(uint iTrk=0; iTrk!=5; iTrk++){
//		LOG_INFO("track index " << iTrk << " momentum " << l1Info->getL1StrawTrack_P(iTrk));
//		LOG_INFO("track index " << iTrk << " vertex " << l1Info->getL1StrawTrack_Vz(iTrk));
		algoPacket->l1Data[iTrk] = ((uint)l1Info->getL1StrawTrack_P(iTrk) << 16) |  (uint)l1Info->getL1StrawTrack_Vz(iTrk);
	}
	algoPacket->l1Data[5] = l1Info->getL1StrawNTracks();
//	if (AlgoRefTimeSourceID_[l0MaskID] == 1)
//		algoPacket->l1Data[5] = l1Info->getCHODAverageTime();
//	else
//		algoPacket->l1Data[5] = 0;
	algoPacket->numberOfWords = (sizeof(L1StrawAlgo) / 4.);
//	LOG_INFO("l0MaskID " << l0MaskID);
//	LOG_INFO("algoID " << (uint)algoPacket->algoID);
//	LOG_INFO("quality Flags " << (uint)algoPacket->qualityFlags);
//	LOG_INFO("online TW " << (uint)algoPacket->onlineTimeWindow);
//	LOG_INFO("Data Words " << algoPacket->l1Data[0] << " " << algoPacket->l1Data[1]);

}

} /* namespace na62 */

