/*
 * DecoderIterator.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 * Description: Range Iterator used by the DecoderHandler that triggers a lambda function just before
 * 				an element is acquired. This can be used for the decoder to implement a lazy decoding
 */
#pragma once
#ifndef COMMON_DECODING_DECODERRANGE_H_
#define COMMON_DECODING_DECODERRANGE_H_

#include <sys/types.h>
#include <functional>

using namespace std;

namespace na62 {

class DecoderHandler;

template<class T>
class ElementIterator {

public:
	ElementIterator(T* start, DecoderHandler* handler) :
			current_(start), handler_(handler) {
	}

	T* operator *() const;

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
	DecoderHandler* handler_;
};

template<class T>
class DecoderRange {
public:
	DecoderRange(T* begin, T* end, DecoderHandler* handler) :
			begin_(begin, handler), end_(end, nullptr) {
	}

	const ElementIterator<T> begin() const {
		return begin_;
	}

	const ElementIterator<T> end() const {
		return end_;
	}

private:
	ElementIterator<T> begin_;
	ElementIterator<T> end_;
};

} /* namespace na62 */

#endif /* COMMON_DECODING_DECODERRANGE_H_ */
