
#ifndef COMMON_DECODING_DECODERHANDLER_H_
#define COMMON_DECODING_DECODERHANDLER_H_

//#include <vector>
//#include <boost/noncopyable.hpp>

#include <sys/types.h>
#include <functional>
#include <memory>
#include <l0/offline/Subevent.h>
#include <common/decoding/DecoderRange.h>
#include <common/decoding/TrbFragmentDecoder.h>
#include <common/decoding/SrbFragmentDecoder.h>

class EventIndexer {
private:
	na62::l0::Subevent* fsubevent;
	uint_fast32_t ftimestamp;
	uint_fast8_t ffinetime;
public:
	EventIndexer(na62::l0::Subevent* subevent, uint_fast32_t timestamp) : ffinetime(0)  {
		fsubevent = subevent;
		ftimestamp = timestamp;
	}
	 uint_fast32_t getTimestamp() const {
		 return ftimestamp;
	 }
	 void setFinetime(const uint_fast8_t finetime) {
		 ffinetime = finetime;
	 }
	 uint_fast8_t getFinetime() const {
		 return ffinetime;
	 }

	 inline const na62::l0::Subevent* getCEDARSubevent() const {
		 return fsubevent;
	 }
	 inline const na62::l0::Subevent* getSTRAWSubevent() const {
		 return fsubevent;
	 }
	 inline const na62::l0::Subevent* getLAVSubevent() const {
		 return fsubevent;
	 }

//	 inline const na62::l0::Subevent* getCHODSubevent() const {
//		 return fsubevent;
//	 }
//	 inline const na62::l0::Subevent* getMUV3Subevent() const {
//		 return fsubevent;
//	 }
//	 inline const na62::l0::Subevent* getRICHSubevent() const {
//		 return fsubevent;
//	 }

//	 inline const l0::Subevent* getL0GTKSubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x08)];
//	 }
//	 inline const l0::Subevent* getCHANTISubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x0C)];
//	 }

//	 inline const l0::Subevent* getNewCHODSubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x20)];
//	 }

//	 inline const l0::Subevent* getIRCSubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x20)];
//	 }

//	 inline const l0::Subevent* getSACSubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x34)];
//	 }
//	 inline const l0::Subevent* getL0TPSubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x40)];
//	 }
//	 inline const l0::Subevent* getL1ResultSubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x44)];
//	 }
//	 inline const l0::Subevent* getL2ResultSubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x48)];
//	 }
//	 inline const l0::Subevent* getNSTDSubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x4C)];
//	 }
//	 inline const l0::Subevent* getHASCSubevent() const {
//	  return L0Subevents[SourceIDManager::sourceIDToNum(0x3C)];
//	 }
//	// L1 dets
//	 inline const l1::Subevent* getL1GTKSubevent() const {
//	  return L1Subevents[SourceIDManager::l1SourceIDToNum(0x08)];
//	 }
//	 inline const l1::Subevent* getLKrSubevent() const {
//	  return L1Subevents[SourceIDManager::l1SourceIDToNum(0x24)];
//	 }
//	 inline l1::Subevent* getMuv1Subevent() const {
//	  return L1Subevents[SourceIDManager::sourceIDToNum(0x28)];
//	 }
//	 inline l1::Subevent* getMuv2Subevent() const {
//	  return L1Subevents[SourceIDManager::sourceIDToNum(0x2C)];
//	 }
};



//namespace na62 {
//class EventIndexer;
//} /* namespace na62 */

/*
 * Define a macro to add functionality for a detector
 */
#define ADD_TRB(DETECTOR)																										\
																																\
private: 																														\
                                																								\
    std::unique_ptr<TrbFragmentDecoder, std::function<void(TrbFragmentDecoder*)>> DETECTOR##Decoders; /* One SrbFragmentDecoder for every MEP fragment 	*/ \
    /**																															\
	 * This method must be called before you access the DETECTOR##Decoders														\
	 * It prepares the decoding if it has not already been done (idempotence)													\
	 */																								 							\
	void prepare##DETECTOR##Usage() {																							\
		if ( !DETECTOR##Decoders) {																								\
			/* initialize all Decoders. They will be in "unready" state for now so you still									\
			 * have to call readData() for all of them before accessing the decoded data										\
			 */ 																												\
			auto deleter = [](TrbFragmentDecoder* p) { delete[] p; };															\
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

#define ADD_SRB(DETECTOR)												\
																		\
private: 																\
                                										\
    std::unique_ptr<SrbFragmentDecoder, std::function<void(SrbFragmentDecoder*)>> DETECTOR##Decoders; /* One TrbFragmentDecoder for every MEP fragment 	*/ \
    /**																															\
	 * This method must be called before you access the DETECTOR##Decoders														\
	 * It prepares the decoding if it has not already been done (idempotence)													\
	 */																								 							\
	void prepare##DETECTOR##Usage() {																							\
		if ( !DETECTOR##Decoders) {																								\
			/* initialize all Decoders. They will be in "unready" state for now so you still									\
			 * have to call readData() for all of them before accessing the decoded data										\
			 */ 																												\
			auto deleter = [](SrbFragmentDecoder* p) { delete[] p; };															\
			DETECTOR##Decoders = std::unique_ptr<SrbFragmentDecoder, decltype(deleter)>(new SrbFragmentDecoder[getNumberOf##DETECTOR##Fragments()], deleter );		\
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
	const SrbFragmentDecoder& getDecoded##DETECTOR##Fragment(			\
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
	DecoderRange<SrbFragmentDecoder> get##DETECTOR##DecoderRange() {	\
		prepare##DETECTOR##Usage();										\
		SrbFragmentDecoder* first = &DETECTOR##Decoders.get()[0];		\
																		\
		return DecoderRange<SrbFragmentDecoder>(first,					\
				first + getNumberOf##DETECTOR##Fragments(), this);		\
																		\
	}																	\

namespace na62 {

class DecoderHandler {
	//friend class DecoderTest;

public:
//	DecoderHandler(EventIndexer* const event);
//	virtual ~DecoderHandler();

	DecoderHandler(EventIndexer* const event) :
			event_(event) {

	}


	~DecoderHandler() {
	}


	EventIndexer* getDecodedEvent() {
		return event_;
	}

	/*
	 * Add functionality for all detectors
	 */

//ADD_TRB(CHOD)
ADD_TRB(CEDAR)
//ADD_TRB(RICH)

ADD_TRB(LAV)
ADD_SRB(STRAW)
//ADD_TRB(MUV3)
//ADD_TRB(IRC)

private:
	EventIndexer* const event_;

};

} /* namespace na62 */

#endif /* COMMON_DECODING_DECODERHANDLER_H_ */
