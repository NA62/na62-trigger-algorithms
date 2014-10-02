/*
 * StrawHits.h
 *
 *  Created on: Sep 25, 2014
 *      Author: jpinzino
 */

#ifndef STRAWHITS_H_
#define STRAWHITS_H_

#define maxnhit 200

namespace na62 {
class StrawData;
}


namespace na62 {

class StrawHits {
public:
	int nhit;
	int* strawID;
	int* leading;
	int* trailing;
	int* used;

	StrawHits();
	virtual ~StrawHits();

	void SetHits(StrawData);
};

}
#endif /* STRAWHITS_H_ */
