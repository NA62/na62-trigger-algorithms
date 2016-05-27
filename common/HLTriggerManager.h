/*
 * HLTriggerManager.h
 *
 *  Created on: 19 Apr 2016
 *      Author: romano
 */

#ifndef COMMON_HLTRIGGERMANAGER_H_
#define COMMON_HLTRIGGERMANAGER_H_

#include "../struct/HLTConfParams.h"
#include "../struct/HLTConfParamsProxy.h"

namespace na62 {

class HLTriggerManager {
public:
	HLTriggerManager();
	virtual ~HLTriggerManager();

	static void createXMLFile();
	static void fillStructFromXMLFile(HLTStruct &HLTStruct);

private:

	static std::string xmlTriggerFile;
};

} /* namespace na62 */

#endif /* COMMON_HLTRIGGERMANAGER_H_ */
