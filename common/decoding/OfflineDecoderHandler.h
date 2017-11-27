
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




#include <vector>








class EventIndexer {
private:
	//na62::l0::Subevent* fsubevent;
	na62::l0::Subevent ** L0Subevents;
	uint_fast32_t ftimestamp;
	uint_fast8_t ffinetime;
	uint_fast8_t fnumber_of_detector;
	uint_fast8_t * L0_DATA_SOURCE_ID_TO_NUM;

	std::vector<std::pair<uint_fast8_t, uint_fast8_t>> fid_boards;
public:
//	EventIndexer2(na62::l0::Subevent* subevent, uint_fast32_t timestamp) : ffinetime(0)  {
//		fsubevent = subevent;
//		ftimestamp = timestamp;
//	}
	EventIndexer(): fnumber_of_detector(0), ffinetime(0) {
	}
	void addDetector(std::string name, uint fragment_number) {

		if (name == "Cedar") {
			// SOURCE_ID_CEDAR 0x04
			fid_boards.push_back(std::make_pair(0x4, fragment_number));
			++fnumber_of_detector;

			return;
		}
//		}else if (name == "LAV") {
//			++fnumber_of_detector;
//			// SOURCE_ID_LAV 0x10
//			fid_boards.push_back(std::make_pair(0x10, fragment_number));
//			return;
//
//		} else if (name == "Spectrometer") {
//			++fnumber_of_detector;
//			// SOURCE_ID_STRAW 0x14
//			fid_boards.push_back(std::make_pair(0x14, fragment_number));
//			return;
//		}
		return;
	}




	void initialize() {
		uint_fast8_t biggest_source_id = 0;
		for (auto&& id_board: fid_boards) {
			if (biggest_source_id < id_board.first) {
				biggest_source_id = id_board.first;
			}
		}
		L0_DATA_SOURCE_ID_TO_NUM = new uint_fast8_t[biggest_source_id + 1];

		L0Subevents = new na62::l0::Subevent*[fid_boards.size()];
		for (uint index = 0; index < fid_boards.size(); ++index) {
			L0_DATA_SOURCE_ID_TO_NUM[fid_boards[index].first] = index;
			L0Subevents[index] = new na62::l0::Subevent(fid_boards[index].second, fid_boards[index].first);
		}
	}

	uint_fast8_t fromDataSourceToNum(uint_fast8_t source_id) const {
		return L0_DATA_SOURCE_ID_TO_NUM[source_id];
	}

	void addSubEnvent(uint* start_data, uint* end_data, uint_fast8_t source_id) {
		bool is_present = 0;
		for (auto&& fid_board: fid_boards){
			if (fid_board.first == source_id) {
				is_present = 1;
				break;
			}
		}
		if (!is_present){
			return;
		}

		std::cout << "Number of Fragmented: " <<L0Subevents[fromDataSourceToNum(source_id)]->getNumberOfFragments() << std::endl;


	   uint* rolling = start_data;
		//Subevent(const uint_fast16_t expectedPacketsNum, const uint_fast8_t sourceID)

		//uint_fast8_t sourceID = 2;
		//na62::l0::Subevent* subevent = new na62::l0::Subevent(fragment_number, sourceID);

		uint_fast32_t timestamp;
		//uint_fast32_t event_number = 2;
		while(rolling < end_data) { // LOOP ON ALL SUBDETECTOR BLOCKS
			na62::l0::MEPFragment_HDR* fragment_header = (na62::l0::MEPFragment_HDR*) rolling;
			std::cout << "Lenght: "<< fragment_header->eventLength_ << std::endl;
			na62::l0::MEPFragment* fragment = new na62::l0::MEPFragment(fragment_header);

			std::cout << "INPUT eventLength_   : " << fragment_header->eventLength_ << std::endl;
			//std::cout << "INPUT eventNumberLSB_: " << fragment_header->eventNumberLSB_<< std::endl;
			std::cout << "INPUT timestamp_     : " << fragment_header->timestamp_<< std::endl;



			std::cout << "getPayloadLenght: " << fragment->getPayloadLength() << std::endl;








			timestamp = fragment_header->timestamp_;
			std::cout << "writing to : " << fromDataSourceToNum(source_id) << std::endl;
			if(!L0Subevents[fromDataSourceToNum(source_id)]->addFragment(fragment)) {
				std::cout << "Cannot add the fragment" << std::endl;
			}
			rolling += fragment_header->eventLength_ / 4;
		}
		std::cout << "Number of Fragmented: " <<L0Subevents[fromDataSourceToNum(source_id)]->getNumberOfFragments() << " / " << L0Subevents[fromDataSourceToNum(source_id)]->getNumberOfExpectedFragments() << std::endl;
	}

	 void setFinetime(const uint_fast8_t finetime) {
		 ffinetime = finetime;
	 }
	 uint_fast8_t getFinetime() const {
		 return ffinetime;
	 }
	 uint_fast32_t getTimestamp() const {
		 return ftimestamp;
	 }

	void reset() {
		for (uint index = 0; index < fnumber_of_detector; ++index) {
			L0Subevents[index]->reset();
		}
	}

	inline const na62::l0::Subevent* getCEDARSubevent() const {
		std::cout << "reading from: " << fromDataSourceToNum(0x04) << std::endl;
		return L0Subevents[0];
		return L0Subevents[fromDataSourceToNum(0x04)];
	}

	inline const na62::l0::Subevent* getLAVSubevent() const {
		std::cout << "reading from: " << fromDataSourceToNum(0x10) << std::endl;
		return L0Subevents[fromDataSourceToNum(0x10)];
	}

	inline const na62::l0::Subevent* getSTRAWSubevent() const {
		std::cout << "reading from: " << fromDataSourceToNum(0x14) << std::endl;
		return L0Subevents[fromDataSourceToNum(0x14)];
	}
};
























//class EventIndexer {
//private:
//	na62::l0::Subevent* fsubevent;
//	uint_fast32_t ftimestamp;
//	uint_fast8_t ffinetime;
//public:
//	EventIndexer(na62::l0::Subevent* subevent, uint_fast32_t timestamp) : ffinetime(0)  {
//		fsubevent = subevent;
//		ftimestamp = timestamp;
//	}
//	 uint_fast32_t getTimestamp() const {
//		 return ftimestamp;
//	 }
//	 void setFinetime(const uint_fast8_t finetime) {
//		 ffinetime = finetime;
//	 }
//	 uint_fast8_t getFinetime() const {
//		 return ffinetime;
//	 }
//
//
////#define SOURCE_ID_CEDAR 0x04
////#define SOURCE_ID_LAV 0x10
////#define SOURCE_ID_STRAW 0x14
////#define SOURCE_ID_CHOD 0x18
////#define SOURCE_ID_L0TP 0x40
//
////		l0::Subevent ** L0Subevents;
////		L0Subevents[i] = new l0::Subevent(SourceIDManager::getExpectedPacksBySourceNum(i), SourceIDManager::sourceNumToID(i));
////
////		L0Subevents = new l0::Subevent*[SourceIDManager::NUMBER_OF_L0_DATA_SOURCES];
////		for (int i = SourceIDManager::NUMBER_OF_L0_DATA_SOURCES - 1; i >= 0; i--) {
////			L0Subevents[i] = new l0::Subevent(SourceIDManager::getExpectedPacksBySourceNum(i), SourceIDManager::sourceNumToID(i));
////		}
//
//
//	 inline const na62::l0::Subevent* getCEDARSubevent() const {
//		 return fsubevent;
//	 }
//	 inline const na62::l0::Subevent* getSTRAWSubevent() const {
//		 return fsubevent;
//	 }
//	 inline const na62::l0::Subevent* getLAVSubevent() const {
//		 return fsubevent;
//	 }
//
////	 inline const na62::l0::Subevent* getCHODSubevent() const {
////		 return fsubevent;
////	 }
////	 inline const na62::l0::Subevent* getMUV3Subevent() const {
////		 return fsubevent;
////	 }
////	 inline const na62::l0::Subevent* getRICHSubevent() const {
////		 return fsubevent;
////	 }
//
////	 inline const l0::Subevent* getL0GTKSubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x08)];
////	 }
////	 inline const l0::Subevent* getCHANTISubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x0C)];
////	 }
//
////	 inline const l0::Subevent* getNewCHODSubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x20)];
////	 }
//
////	 inline const l0::Subevent* getIRCSubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x20)];
////	 }
//
////	 inline const l0::Subevent* getSACSubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x34)];
////	 }
////	 inline const l0::Subevent* getL0TPSubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x40)];
////	 }
////	 inline const l0::Subevent* getL1ResultSubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x44)];
////	 }
////	 inline const l0::Subevent* getL2ResultSubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x48)];
////	 }
////	 inline const l0::Subevent* getNSTDSubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x4C)];
////	 }
////	 inline const l0::Subevent* getHASCSubevent() const {
////	  return L0Subevents[SourceIDManager::sourceIDToNum(0x3C)];
////	 }
////	// L1 dets
////	 inline const l1::Subevent* getL1GTKSubevent() const {
////	  return L1Subevents[SourceIDManager::l1SourceIDToNum(0x08)];
////	 }
////	 inline const l1::Subevent* getLKrSubevent() const {
////	  return L1Subevents[SourceIDManager::l1SourceIDToNum(0x24)];
////	 }
////	 inline l1::Subevent* getMuv1Subevent() const {
////	  return L1Subevents[SourceIDManager::sourceIDToNum(0x28)];
////	 }
////	 inline l1::Subevent* getMuv2Subevent() const {
////	  return L1Subevents[SourceIDManager::sourceIDToNum(0x2C)];
////	 }
//};



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
