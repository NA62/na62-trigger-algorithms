#include "HLTConfParams.h"

#ifndef HLTCONFPARAMSPROXY_H_
#define HLTCONFPARAMSPROXY_H_

#ifdef __cplusplus
#include <string>
extern "C" {
#endif
int xml_read_file_HLTStruct(const char* fileName);
int xml_apply_HLTStruct(HLTStruct *ptr);
int xml_test_HLTStruct();
void* xml_start_compare_HLTStruct(HLTStruct *ptr);
void* xml_next_compare_HLTStruct(HLTStruct *ptr);
int xml_create_HLTStruct(HLTStruct *ptr, const char* fileName);
const char* xml_getLastFatalError_HLTStruct();

#ifdef __cplusplus
}
#endif

#endif /* HLTCONFPARAMSPROXY_H_ */

