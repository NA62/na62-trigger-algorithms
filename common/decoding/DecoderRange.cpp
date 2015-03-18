#include "DecoderRange.h"

#include "DecoderHandler.h"
#include "TrbFragmentDecoder.h"

namespace na62 {

template<class T>
T* ElementIterator<T>::operator *() const {
	//onNextElement_(current_);
	handler_->onNextCEDARElement(current_);
	return current_;
}

// Explicit template instantiation
template class ElementIterator<TrbFragmentDecoder>;

}
