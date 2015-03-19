#include "DecoderRange.h"

#include "DecoderHandler.h"
#include "TrbFragmentDecoder.h"

namespace na62 {

template<class T>
T* ElementIterator<T>::operator *() const {
	current_->readData(handler_->getDecodedEvent()->getTimestamp());
	return current_;
}

// Explicit template instantiation
template class ElementIterator<TrbFragmentDecoder> ;

}
