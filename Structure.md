<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**
- [Code structure and data flow](#code-structure-and-data-flow)
  - [Data flow](#data-flow)
    - [TriggerProcessor](#triggerprocessor)
      - [Guideline](#guideline)
      - [Accessing L0 data](#accessing-l0-data)
      - [Accessing LKr data](#accessing-lkr-data)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Code structure and data flow
The code is optimized to be used within IDEs like Eclipse. So please use the autocompletion (ctrl+space) and read the list of available methods together with their documentation within your IDE!

## Data flow
An introductory overview of the data flow can be found [[here|https://github.com/NA62/na26-farm/blob/master/README.md]].

The L1 trigger algorithms are executed with all data sent by all detectors except the LKr. The result of the L1 is a 16-bit word including the L0 trigger type word (8 bit) and the L1 trigger type word (8 bit).

If the L1 result is not zero the remaining data from the LKr is requested and the L2 algorithms are executed. The result of L2 is the L2 trigger type word (8 bit). If the L2 trigger is not zero the event will be sent to the merger PC.

### TriggerProcessor
L1 and L2 trigger algorithms must be implemented in the compute method of the respective TriggerProcessors: (L1/L2)TriggerProcessor::compute(Event* event)

These methods are called after each event building (L1 and L2 event building) with the Event object containing all the raw data at this stage.

#### Guideline
Please keep the TriggerProcessor classes clean and outsource your code into the "l1", "l2" and "general" folders. Also feel free to provide external libraries to be used within the TrigerProcessor implementations.

Following sections show how you can access the raw data as it has been sent within MEPs. Please see also example codes in the [[examples|https://github.com/NA62/na62-trigger-algorithms/tree/examples]] branch.
#### Accessing L0 data
```C++
// Access a specific detector:

Subevent* lav = event->getLAVSubevent();
for (int p = lav->getNumberOfParts() - 1; p >= 0; p--) {
	MEPFragment* fragment = lav->getPart(p);
	const MEPFragment_HDR* data = fragment->getData();

	for(int i=0; i<fragment->getDataLength(); i++){
		sum+=((char*)data)[i];
	}
}

// Access all detectors:
for (int i = 0; i != SourceIDManager::NUMBER_OF_L0_DATA_SOURCES; i++) {
	Subevent* subevent = event->getL0SubeventBySourceIDNum(i);

	for (int j = subevent->getNumberOfParts() - 1; j >= 0; j--) {
		MEPEvent* e = subevent->getPart(j);

		const char* data = e->getDataWithHeader();
		const uint dataSize = e->getEventLength();
	}
}
```

#### Accessing LKr data
```C++
std::vector<uint_fast16_t> localCreamIDsToRequestNonZSuppressedData;
for (int localCreamID = event->getNumberOfZSuppressedLkrFragments() - 1;
		localCreamID != -1; localCreamID--) {
	LkrFragment* fragment = event->getZSuppressedLkrFragment(localCreamID);
	localCreamIDsToRequestNonZSuppressedData.push_back(
			fragment->getCrateCREAMID());
	const char* data = fragment->getDataWithHeader();
	const uint dataSize = fragment->getEventLength();
}
```