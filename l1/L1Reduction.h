/*
 * L1Reduction.h

 *
 *  Created on: June 24, 2015
 *      Author: Angela Romano (angela.romano@cern.ch)
 */

#ifndef L1_L1REDUCTION_H_
#define L1_L1REDUCTION_H_

#include <sys/types.h>
#include <atomic>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

namespace na62 {


class L1Reduction {
public:

	/**
	 * Registers a new algorithm that should be reduced and returns it's ID. This method
	 * may only be called once at startup before the Options are initialized!
	 *
	 * The parameter algorithmTitle will be used to initialize the Options so that the option
	 * --`algoreduction_algoName` exists as parameter for the PC-farm and test framework with the default
	 * value of 1 (no reduction)
	 *
	 * The returned algorithm ID should be used to decide whether or not the algorithm should be
	 * processed in the following way:
	 *
	 * uint myAlgoID = L1Reduction::registerAlgorithm()
	 * ...
	 * if(L1Reduction::processAlgorithm(myAlgoID){
	 * 		processAlgo();
	 * 	}
	 */
	static uint registerAlgorithm(std::string algorithmTitle);

	static uint getNumberOfRegisteredAlgorithms();

	static void initialize();

	/**
	 * Returns true if the algorithm defined by sourceID and algorithmNumber should be
	 * processed or false if it should be reduced. This means that true is returned
	 * every Nth time being called with the same parameter where N is the corresponding
	 * reduction factor of the given algorithmID.
	 */
	static inline bool processAlgorithm(uint algorithmID) {
#ifdef DEBUGGING
		if (algorithmID >= reductionFactors_.size()) {
			std::cerr << "Call of processAlgorithm for algorithm "
					<< algorithmID << " even though it is not registered yet."
					<< std::endl;
			exit(1);
		}
#endif
		return ++(eventCountersByAlgoID_[algorithmID])
				% reductionFactors_[algorithmID] == 0;
	}

private:
	static std::vector<std::string> algorithmTitles_;
	static std::atomic<int>* eventCountersByAlgoID_; // cannot store atomics in vector->use simple array
	static std::vector<uint> reductionFactors_;
};

} /* namespace na62 */

#endif /* L1_L1REDUCTION_H_ */
