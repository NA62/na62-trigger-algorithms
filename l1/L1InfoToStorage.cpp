/*
 * L1InfoToStorage.cpp
 *
 *  Created on: 2 Nov 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1InfoToStorage.h"

#include <iostream>
#include <vector>
#include <options/Logging.h>
#include <string.h>

#include <sys/time.h>

L1InfoToStorage* L1InfoToStorage::theInstance = nullptr;

L1InfoToStorage::L1InfoToStorage() {
	LOG_INFO("********************In L1 InfoToStorage");
	chodTime = 0.;
	L1CHODProcessed_ = false;
	L1KTAGProcessed_ = false;
	L1LAVProcessed_ = false;
	L1MUV3Processed_ = false;
}

L1InfoToStorage::~L1InfoToStorage() {
}

L1InfoToStorage* L1InfoToStorage::GetInstance() {

	if (theInstance == nullptr) {
		theInstance = new L1InfoToStorage();
	}
	return theInstance;

}

double L1InfoToStorage::getCHODAverageTime(){
	return chodTime;
}

void L1InfoToStorage::setCHODAverageTime(double time){
	chodTime = time;
}
