/*
 * Decoder.cpp
 *
 *  Created on: Mar 17, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "DecoderHandler.h"

namespace na62 {

DecoderHandler::DecoderHandler(Event* const event) :
		event_(event) {

}

DecoderHandler::~DecoderHandler() {
	delete[] CEDARDecoders;
}

} /* namespace na62 */
