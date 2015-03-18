/*
 * DecoderIterator.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 * Description: Range Iterator used by the DecoderHandler that triggers a lambda function just before
 * 				an element is acquired. This can be used for the decoder to implement a lazy decoding
 */

#ifndef COMMON_DECODING_DECODERRANGE_H_
#define COMMON_DECODING_DECODERRANGE_H_

#include <sys/types.h>
#include <functional>

using namespace std;

namespace na62 {

template<class T>
class DecoderRange {
public:
	DecoderRange(T* begin, T* end, std::function<void(T*)> onNextElement) :
			begin_(begin, onNextElement), end_(end, [](T*) {}) {
	}

	class ElementIterator {
		friend class DecoderRange;
	protected:
		ElementIterator(T* start, std::function<void(T*)> onNextElement) :
				current_(start), onNextElement_(onNextElement) {
		}

	public:
		T* operator *() const {
			onNextElement_(current_);
			return current_;
		}

		const ElementIterator &operator ++() {
			++current_;
			return *this;
		}

		ElementIterator operator ++(int i) {
			ElementIterator copy(*this);
			++current_;
			return copy;
		}

		bool operator ==(const ElementIterator &other) const {
			return current_ == other.current_;
		}

		bool operator !=(const ElementIterator &other) const {
			return current_ != other.current_;
		}

	private:
		T* current_;
		const std::function<void(T*)> onNextElement_;
	};

	const ElementIterator begin() const {
		return begin_;
	}

	const ElementIterator end() const {
		return end_;
	}

private:
	ElementIterator begin_;
	ElementIterator end_;
};

} /* namespace na62 */

#endif /* COMMON_DECODING_DECODERRANGE_H_ */
