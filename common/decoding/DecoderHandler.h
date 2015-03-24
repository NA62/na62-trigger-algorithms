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

#include "DecoderRange.h"
#include "TrbFragmentDecoder.h"

namespace na62 {
class Event;
} /* namespace na62 */

/*
 * Define a macro to add functionality for a detector
 */
#define ADD_TRB(DETECTOR)																										\
private:																														\
																																\
	std::vector<TrbFragmentDecoder> DETECTOR##Decoders; /* One TrbFragmentDecoder for every MEP fragment 	*/					\
																																\
																																\
	/**																															\
	 * This method must be called before you access the DETECTOR##Decoders														\
	 * It prepares the decoding if it has not already been done (idempotence)													\
	 */																															\
	void prepare##DETECTOR##Usage() {																							\
		if ( DETECTOR##Decoders.empty() ) {																						\
			/* initialize all Decoders. They will be in "unready" state for now so you still									\
			 * have to call readData() for all of them before accessing the decoded data										\
			 */ 																												\
			DETECTOR##Decoders.resize(getNumberOf##DETECTOR##Fragments());														\
		}																														\
	}																															\
																																\
public:																															\
	/**																															\
	 * Returns the decoded data of the <fragmentNumber>th fragment of ##DETECTOR##	data										\
	 */																															\
	const TrbFragmentDecoder getDecoded##DETECTOR##Fragment(const uint fragmentNumber) {										\
		prepare##DETECTOR##Usage();																								\
		if (!DETECTOR##Decoders[fragmentNumber].isReady()) {																	\
			const l0::Subevent* const subevent = event_->get##DETECTOR##Subevent();												\
			DETECTOR##Decoders[fragmentNumber].readData( fragmentNumber,														\
					subevent->getFragment(fragmentNumber),																		\
					event_->getTimestamp());																					\
		}																														\
		return DETECTOR##Decoders[fragmentNumber];																				\
	}																															\
																																\
	/**																															\
	 * Returns the number of available fragments for the ##DETECTOR##															\
	 */																															\
	uint getNumberOf##DETECTOR##Fragments() const { 																			\
		const l0::Subevent* const subevent = event_->get##DETECTOR##Subevent(); 												\
		return subevent->getNumberOfFragments(); 																				\
	} 																															\
	  																															\
	/**  																														\
	 * Returns an iterator for range based loops which automatically decodes data in a lazy way 								\
	 */   																														\
	DecoderRange<TrbFragmentDecoder> get##DETECTOR##DecoderRange() {															\
		prepare##DETECTOR##Usage();																								\
		TrbFragmentDecoder* first = &DETECTOR##Decoders[0];																		\
																																\
		return DecoderRange<TrbFragmentDecoder>(first, first + DETECTOR##Decoders.size(), 										\
				[this, first](TrbFragmentDecoder* decoder)																		\
				{																												\
					if(!decoder->isReady()) {																					\
						const uint_fast16_t fragmentID = decoder - first;														\
						const l0::Subevent* const subevent = event_->get##DETECTOR##Subevent();									\
						DETECTOR##Decoders[fragmentID].readData(fragmentID, 													\
									subevent->getFragment(fragmentID), event_->getTimestamp());									\
					} 																											\
				}); 																											\
	}

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
//ADD_TRB(CEDAR)
private:
	// #\#
	// #\#
	TrbFragmentDecoder* CEDARDecoders = nullptr; /* One TrbFragmentDecoder for every MEP fragment 	*/// #\#
																									 // #\#
	/**																															// #\#
	 * This method must be called before you access the CEDARDecoders														// #\#
	 * It prepares the decoding if it has not already been done (idempotence)													// #\#
	 */																								 // #\#
	void prepareCEDARUsage() {											// #\#
		if (CEDARDecoders == nullptr) {									// #\#
			/* initialize all Decoders. They will be in "unready" state for now so you still									// #\#
			 * have to call readData() for all of them before accessing the decoded data										// #\#
			 */ 															// #\#
			CEDARDecoders = new TrbFragmentDecoder[getNumberOfCEDARFragments()]; // #\#
			const l0::Subevent* const subevent = event_->getCEDARSubevent(); // #\#
			for (uint i = 0; i != getNumberOfCEDARFragments(); i++) {
				CEDARDecoders[i].setDataSource(subevent, i);
			}
		}																// #\#
	}																	// #\#
																		// #\#
public:
	// #\#
	/**																															// #\#
	 * Returns the decoded data of the <fragmentNumber>th fragment of CEDAR	data										// #\#
	 */// #\#
	const TrbFragmentDecoder& getDecodedCEDARFragment(
			const uint fragmentNumber) {								// #\#
		prepareCEDARUsage();											// #\#

		// readData is idempotent so just call it every time
		CEDARDecoders[fragmentNumber].readData(event_->getTimestamp());	// #\#
		return CEDARDecoders[fragmentNumber];							// #\#
	} 																	// #\#
																		// #\#
	/**																															// #\#
	 * Returns the number of available fragments for the CEDAR															// #\#
	 */																	// #\#
	uint getNumberOfCEDARFragments() const { 							// #\#
		const l0::Subevent* const subevent = event_->getCEDARSubevent(); // #\#
		return subevent->getNumberOfFragments(); 						// #\#
	} 																	// #\#
																		// #\#
	/**  																														// #\#
	 * Returns an iterator for range based loops which automatically decodes data in a lazy way 								// #\#
	 */																	// #\#
	DecoderRange<TrbFragmentDecoder> getCEDARDecoderRange() {			// #\#
		prepareCEDARUsage();											// #\#
		TrbFragmentDecoder* first = &CEDARDecoders[0];					// #\#

		return DecoderRange<TrbFragmentDecoder>(first,
				first + getNumberOfCEDARFragments(), this);
		// #\#
	}		// #\#
	// #\#

//ADD_TRB(CHOD)

private:
	Event* const event_;

};

} /* namespace na62 */

#endif /* COMMON_DECODING_DECODERHANDLER_H_ */
