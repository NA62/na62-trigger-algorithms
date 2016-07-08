/*
 * SearchPMT.cpp
 *
 *  Created on: Nov 24, 2014
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

///////////////////////////////////////////////////////////////////////////////
///Input Parameters: tel62 index, PP index, TDC index, TDC channel ID(0,8,16,24)
///////////////////////////////////////////////////////////////////////////////

#include "SearchPMT.h"

#include <stdlib.h>
#include <stdio.h>        
#include <math.h>
#include <string.h>
#include <stdarg.h>

//int SearchPMT::search_pmt(int ktag, int pp, int tdc, int ch){
int SearchPMT(int ktag, int pp, int tdc, int ch){

  int box = -1;
  //printf("KTAG %d\n",ktag);
  //printf("PP %d\n",pp);
  //printf("TDC %d\n",tdc);
  //printf("Ch %d\n",ch);

  printf("Request for combination KTAG %d, PP %d, TDC %d, Ch %d \n",ktag,pp,tdc,ch);

  if((ktag<0) || (ktag>5)){
    printf("Tel62 index %d out of range: ktag 0-5 allowed \n",ktag);
    return box;
  }
  else if((pp<0) || (pp>3)){
    printf("PP index %d out of range: pp 0-3 allowed \n",pp);
    return box;
  }
  else if((tdc<0) || (tdc>3)){
    printf("TDC index %d out of range: tdc 0-3 allowed \n",tdc);
    return box;
  }
  else if((ch!=0) && (ch!=8) && (ch!=16) && (ch!=24)){
    printf("Channel %d out of range: chs 0-8-16-24 only allowed \n",ch);
    return box;
  }

  if((ktag==0) && ((pp==0) || (pp==1) || (pp==2))) box=1;
  else if((ktag==0) && (pp==3)) box=2;
  else if((ktag==1) && ((pp==0) || (pp==1))) box=2;
  else if((ktag==1) && ((pp==2) || (pp==3))) box=3;
  else if((ktag==2) && (pp==0)) box=3;
  else if((ktag==2) && ((pp==1) || (pp==2) || (pp==3))) box=4;
  else if((ktag==3) && ((pp==0) || (pp==1) || (pp==2))) box=5;
  else if((ktag==3) && (pp==3)) box=6;
  else if((ktag==4) && ((pp==0) || (pp==1))) box=6;
  else if((ktag==4) && ((pp==2) || (pp==3))) box=7;
  else if((ktag==5) && (pp==0)) box=7;
  else if((ktag==5) && ((pp==1) || (pp==2) || (pp==3))) box=8;
  else printf("Combination KTAG %d, PP %d not found \n",ktag,pp);

  printf("Found Box %d\n",box);

  int array[4][48];
  int rh_pmt[48] = {66,
		      67,
		      87,
		      58,
		      65,
		      77,
		      78,
		      68,
		      74,
		      53,
		      52,
		      72,
		      64,
		      73,
		      82,
		      61,
		      75,
		      57,
		      63,
		      59,
		      55,
		      62,
		      76,
		      51,
		      45,
		      43,
		      37,
		      48,
		      54,
		      46,
		      47,
		      27,
		      35,
		      36,
		      26,
		      38,
		      33,
		      44,
		      25,
		      22,
		      34,
		      23,
		      32,
		      15,
		      56,
		      24,
		      42,
		      41};

  int lh_pmt[48] = {56,
		      24,
		      42,
		      41,
		      34,
		      23,
		      32,
		      15,
		      33,
		      44,
		      25,
		      22,
		      35,
		      36,
		      26,
		      38,
		      54,
		      46,
		      47,
		      27,
		      45,
		      43,
		      37,
		      48,
		      55,
		      62,
		      76,
		      51,
		      75,
		      57,
		      63,
		      59,
		      65,
		      77,
		      78,
		      68,
		      66,
		      67,
		      87,
		      58,
		      64,
		      73,
		      82,
		      61,
		      74,
		      53,
		      52,
		      72};
 
  int j=0;
  int k=0;
  if(!(box%2)){
    printf("Using Left-Handed (LH) readout mapping for box %d\n",box);
  }
  else{
    printf("Using Right-Handed (RH) readout mapping for box %d\n",box);
  } 

  for(int i_array=0; i_array<48; i_array++){
    if(!(i_array%4) && j){
      j=0;
      k++;
    }
    array[0][i_array] = k; //splitter_cable_id
    array[1][i_array] = 8*j; //splitter_chID
    if(i_array < 24) array[2][i_array] = i_array; //front-end chID (chs 0-23)
    else array[2][i_array] = i_array + 16; //front-end chID (chs 40-63)
    if(!(box%2)) array[3][i_array] = lh_pmt[i_array]; //select LH mapping for boxes (0-2-4-6)
    else array[3][i_array] = rh_pmt[i_array]; //select RH mapping for boxes (1-3-5-7)
    j++;
    //printf("Array[0][%d] %d\n",i_array,array[0][i_array]);
    //printf("Array[1][%d] %d\n",i_array,array[1][i_array]);
    //printf("Array[2][%d] %d\n",i_array,array[2][i_array]);
    //printf("Array[3][%d] %d\n",i_array,array[3][i_array]);
  }

  int splitter_cable_id = -1;
  int FO_chID = -1;
  int PMT_geomID = -1;

  if(((ktag==0) || (ktag==3)) && ((pp==0) || (pp==1) || (pp==2))) splitter_cable_id = tdc + pp*4;
  else if(((ktag==0) || (ktag==3)) && (pp==3)) splitter_cable_id = tdc;
  else if(((ktag==1) || (ktag==4)) && ((pp==0) || (pp==1))) splitter_cable_id = tdc + (pp+1)*4;
  else if(((ktag==1) || (ktag==4)) && ((pp==2) || (pp==3))) splitter_cable_id = tdc + (pp-2)*4;
  else if(((ktag==2) || (ktag==5)) && (pp==0)) splitter_cable_id = tdc + (pp+2)*4;
  else if(((ktag==2) || (ktag==5)) && ((pp==1) || (pp==2) || (pp==3))) splitter_cable_id = tdc + (pp-1)*4;
  else {
    printf("Combination KTAG %d, PP %d, TDC %d, Ch %d not found \n",ktag,pp,tdc,ch);
    return splitter_cable_id;
  }

  //cout << "Splitter CableID" << "\t" << "Splitter ChID" << "\t" << "Front-End ChID" << "\t" << "PMT Geom ID\n" << endl; 
  for(int i_table=0; i_table<48; ++i_table){
    if((splitter_cable_id==array[0][i_table]) && (ch==array[1][i_table])){
      FO_chID = array[2][i_table];
      PMT_geomID = array[3][i_table];

      printf("\nFront-end chID j%d\n",FO_chID);
      printf("PMT GeomID %d\n",PMT_geomID);

    } 
    for(int j_table=0; j_table<4; ++j_table){
      //cout << array[j_table][i_table] << "\t";
    }
  }

  if((FO_chID==-1) || (PMT_geomID==-1)){
    printf("TDC %d and Channel %d: found no matching in the mapping\n",tdc,ch);
    return PMT_geomID;
  }

  PMT_geomID += box*100;
  return PMT_geomID;

}
