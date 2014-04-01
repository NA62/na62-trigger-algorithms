# Code structure and data flow

## Data flow
An introductory overview of the data flow can be found [[here|https://github.com/NA62/na26-farm/blob/master/README.md]].

The L1 trigger algorithms are executed with all data sent by all detectors except the LKr. The result of the L1 is a 16-bit word including the L0 trigger type word (8 bit) and the L1 trigger type word (8 bit).

If the L1 result is not zero the remaining data from the LKr is requested and the L2 algorithms are executed. The result of L2 is the L2 trigger type word (8 bit). If the L2 trigger is not zero the event will be sent to the merger PC.

### TriggerProcessor
L1 and L2 trigger algorithms must be implemented in the compute method of the respective TriggerProcessors: l1/L1TriggerProcessor::compute(Event* event) and l2/L2TriggerProcessor::compute(Event* event)

These methods are called after each event building (L1 and L2) with the Event object containing all the raw data at this state. Following sections show how you can access the raw data as it has been sent within MEPs.

### Accessing L0 data
```C++
for (int i = SourceIDManager::NUMBER_OF_L0_DATA_SOURCES - 1; i >= 0; i--) {
Subevent* subevent = event->getL0SubeventBySourceIDNum(i);
for (int i = SourceIDManager::NUMBER_OF_L0_DATA_SOURCES - 1; i >= 0; i--) {
	Subevent* subevent = event->getL0SubeventBySourceIDNum(i);

	for (int i = subevent->getNumberOfParts() - 1; i >= 0; i--) {
		MEPEvent* e = subevent->getPart(i);

		const char* data = e->getDataWithHeader();
		const uint dataSize = e->getEventLength();
	}
}
```

### Accessing LKr data
```C++
for (int localCreamID = event->getNumberOfZSuppressedLKrEvents() - 1;
		localCreamID != -1; localCreamID--) {
	LKREvent* lkrEvent = event->getZSuppressedLKrEvent(localCreamID);
	localCreamIDsToRequestNonZSuppressedData.push_back(
			lkrEvent->getCrateCREAMID());
	const char* data = lkrEvent->getDataWithHeader();
	const uint dataSize = lkrEvent->getEventLength();
}
```