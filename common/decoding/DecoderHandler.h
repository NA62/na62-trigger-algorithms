/*
 * Decoder.h
 *
 *  Created on: Mar 17, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 * Description: This class stores all decoders potentially used for one single event. It provides one
 * 				iterator for every detector that can be used in range based loops that automatically
 * 				trigger the decoding on demand (lazy decoding)
 */

#ifndef COMMON_DECODING_DECODERHANDLER_H_
#define COMMON_DECODING_DECODERHANDLER_H_

#include <vector>
#include <boost/noncopyable.hpp>
#include <eventBuilding/Event.h>
#include <l0/Subevent.h>
#include <sys/types.h>
#include <functional>
#include <memory>

#include "DecoderRange.h"
#include "TrbFragmentDecoder.h"

namespace na62 {
class Event;
} /* namespace na62 */

/*
 * Define a macro to add functionality for a detector
 */
#define ADD_TRB(DETECTOR)																										\
private: 																														\
																																\
																																\
	std::unique_ptr<TrbFragmentDecoder, std::function<void(TrbFragmentDecoder*)>> DETECTOR##Decoders; /* One TrbFragmentDecoder for every MEP fragment 	*/ 				\
																									 							\
	/**																															\
	 * This method must be called before you access the DETECTOR##Decoders														\
	 * It prepares the decoding if it has not already been done (idempotence)													\
	 */																								 							\
	void prepare##DETECTOR##Usage() {																							\
		if ( !DETECTOR##Decoders) {																								\
			/* initialize all Decoders. They will be in "unready" state for now so you still									\
			 * have to call readData() for all of them before accessing the decoded data										\
			 */ 																												\
			auto deleter = [](TrbFragmentDecoder* p) { delete[] p; };	\
			DETECTOR##Decoders = std::unique_ptr<TrbFragmentDecoder, decltype(deleter)>(new TrbFragmentDecoder[getNumberOf##DETECTOR##Fragments()], deleter );		\
			const l0::Subevent* const subevent = event_->get##DETECTOR##Subevent(); 											\
			for (uint i = 0; i != getNumberOf##DETECTOR##Fragments(); i++) {													\
				DETECTOR##Decoders.get()[i].setDataSource(subevent, i);															\
			}																													\
		}																														\
	}																															\
																																\
public: 																														\
			 																													\
	/**																															\
	 * Returns the decoded data of the <fragmentNumber>th fragment of DETECTOR## data											\
	 */																	\
	const TrbFragmentDecoder& getDecoded##DETECTOR##Fragment(			\
			const uint fragmentNumber) {								\
		prepare##DETECTOR##Usage();										\
																		\
		/*readData is idempotent so just call it every time*/			\
		DETECTOR##Decoders.get()[fragmentNumber].readData(event_->getTimestamp());	\
		return DETECTOR##Decoders.get()[fragmentNumber];							\
	} 																	\
																		\
	/**																	\
	 * Returns the number of available fragments for the DETECTOR##		\
	 */																	\
	uint getNumberOf##DETECTOR##Fragments() const { 					\
		const l0::Subevent* const subevent = event_->get##DETECTOR##Subevent();\
		return subevent->getNumberOfFragments(); 						\
	} 																	\
																		\
	/**  																\
	 * Returns an iterator for range based loops which automatically decodes data in a lazy way 						\
	 */																	\
	DecoderRange<TrbFragmentDecoder> get##DETECTOR##DecoderRange() {	\
		prepare##DETECTOR##Usage();										\
		TrbFragmentDecoder* first = &DETECTOR##Decoders.get()[0];		\
																		\
		return DecoderRange<TrbFragmentDecoder>(first,					\
				first + getNumberOf##DETECTOR##Fragments(), this);		\
																		\
	}																	\


namespace na62 {

class DecoderHandler {
	friend class DecoderTest;

public:
	DecoderHandler(Event* const event);
	virtual ~DecoderHandler();

	Event* getDecodedEvent() {
		return event_;
	}

	/*
	 * Add functionality for all detectors
	 */
ADD_TRB(CEDAR)

ADD_TRB(RICH)

ADD_TRB(CHOD)

private:
	Event* const event_;

};

} /* namespace na62 */

#endif /* COMMON_DECODING_DECODERHANDLER_H_ */
