/*
 * L1InfoToStorage.h
 *
 *  Created on: 2 Nov 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#pragma once
#ifndef L1_INFO_TO_STORAGE_H_
#define L1_INFO_TO_STORAGE_H_

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class L1InfoToStorage {
public:
	L1InfoToStorage();
	~L1InfoToStorage();

	static L1InfoToStorage* GetInstance();

	double getCHODAverageTime();
	void setCHODAverageTime(double time);

	void setL1CHODProcessed() {
		L1CHODProcessed_ = true;
	}

	bool isL1CHODProcessed() const {
		return L1CHODProcessed_;
	}

	void resetL1CHODProcessed() {
		L1CHODProcessed_ = false;
	}

	void setL1KTAGProcessed() {
		L1KTAGProcessed_ = true;
	}

	bool isL1KTAGProcessed() const {
		return L1KTAGProcessed_;
	}

	void resetL1KTAGProcessed() {
		L1KTAGProcessed_ = false;
	}

	void setL1LAVProcessed() {
		L1LAVProcessed_ = true;
	}

	bool isL1LAVProcessed() const {
		return L1LAVProcessed_;
	}

	void resetL1LAVProcessed() {
		L1LAVProcessed_ = false;
	}

	void setL1MUV3Processed() {
		L1MUV3Processed_ = true;
	}

	bool isL1MUV3Processed() const {
		return L1MUV3Processed_;
	}

	void resetL1MUV3Processed() {
		L1MUV3Processed_ = false;
	}
private:

	static L1InfoToStorage* theInstance;  // singleton instance

	double chodTime;
	bool L1CHODProcessed_;
	bool L1KTAGProcessed_;
	bool L1LAVProcessed_;
	bool L1MUV3Processed_;

};

#endif /* L1_INFO_TO_STORAGE_H_ */

