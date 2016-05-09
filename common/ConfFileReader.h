/*
 * ConfFileReader.h
 *
 *  Created on: 17 Apr 2015
 *      Author: vfascian
 *  Modified on: 11 May 2015
 *      Author: romano
 */

#ifndef COMMON_CONFFILEREADER_H_
#define COMMON_CONFFILEREADER_H_

#include <fstream>
#include <sstream>
#include <string>

class ConfFileReader {
public:
	ConfFileReader(const char* filename);
	ConfFileReader(const std::string& filename);

	~ConfFileReader();

	bool nextLine();

	template<typename T>
	T getField(const int n);

	template<typename T>
	T getLine();

	bool inputFailed() const;
	bool isValid() const;

private:
	void skip_fields(std::istringstream& ist, const int n);

	std::ifstream file;
	std::string line;
	bool failed;
};

// Definition of FileReader::get template
template<typename T>
T ConfFileReader::getField(const int n) {
	failed = false;
	std::istringstream ist(line);
	this->skip_fields(ist, n - 1);
	T rval;
	ist >> rval;
	if (ist.fail()) {
		failed = true;
		return T();
	} else {
		return rval;
	}
}

template<typename T>
T ConfFileReader::getLine() {
	failed = false;
	std::istringstream ist(line);
	T rval;
	ist >> rval;
	if (ist.fail()) {
		failed = true;
		return T();
	} else {
		return rval;
	}
}

#endif /* COMMON_CONFFILEREADER_H_ */
