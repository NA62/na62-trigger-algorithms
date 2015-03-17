/*
 * Decoder.h
 *
 *  Created on: Mar 17, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#ifndef COMMON_DECODING_DECODER_H_
#define COMMON_DECODING_DECODER_H_

#include <vector>
#include <boost/noncopyable.hpp>
#include <eventBuilding/Event.h>
#include <l0/Subevent.h>
#include <sys/types.h>

#include "TrbDecoder.h"

/*
 * Define a macro to add functionality for a detector
 */
#define ADD_TRB(DETECTOR) 														\
private: 																		\
	std::vector<TrbDecoder> DETECTOR##Decoders; /* One TrbDecoder for every MEP fragment 	*/		\
																				\
public: 																		\
	/**						\
	 * This method must be called before you call any getter method like get##DETECTOR##NumberOfEdgesPerTrb						\
	 * It prepares the decoding if it has not already been done (idempotence)						\
	 */						\
	void register##DETECTOR##Usage(){						\
		if(DETECTOR##Decoders.empty()) {						\
			const l0::Subevent* const subevent = event_->get##DETECTOR##Subevent();	\
			/* initialize all Decoders. They will be in "unready" state for now so you still	\
			 * have to call readData() for all of them before accessing the decoded data	\
			 */ 	\
			DETECTOR##Decoders.resize(subevent->getNumberOfFragments());	\
		}						\
	}						\
	/**				\
	 * Returns the decoded data of the <fragmentNumber>th fragment of the ##DETECTOR##				\
	 */				\
	TrbDecoder getDecoded##DETECTOR##Fragment(const uint fragmentNumber){						\
		if(!DETECTOR##Decoders[fragmentNumber].isReady()){						\
			const l0::Subevent* const subevent = event_->get##DETECTOR##Subevent();		\
			DETECTOR##Decoders[fragmentNumber].readData(subevent->getFragment(fragmentNumber), event_->getTimestamp());					\
		}						\
		return DETECTOR##Decoders[fragmentNumber];		\
	} \
	uint getNumberOf##DETECTOR##Fragments() const { \
		const l0::Subevent* const subevent = event_->get##DETECTOR##Subevent();		\
		return subevent->getNumberOfFragments(); \
	} \
	std::vector<TrbDecoder> getAll##DETECTOR##DecodedFragments() const { \
		return DETECTOR##Decoders;\
	}

namespace na62 {
class Event;
} /* namespace na62 */

namespace na62 {

class Decoder {
	friend class DecoderTest;

public:
	Decoder(Event* const event);
	virtual ~Decoder(){}

	ADD_TRB(CEDAR)
	ADD_TRB(RICH)

private:
	Event* const event_;

	const TrbDecoder cedarDecoder_;

};

} /* namespace na62 */

#endif /* COMMON_DECODING_DECODER_H_ */
