/*
 * common.h
 *
 *  Created on: Jun 10, 2013
 *      Author: schernikov
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

// trim from both ends
inline std::string trim(const std::string &s) {
	int len = s.size();
	int epos = len-1;
	const char* name = s.c_str();
	while(name[epos] <= 0x20){
		if(epos == 0){
			return "";
		}
		epos -= 1;
	}
	int pos = 0;
	while(name[pos] <= 0x20){
		pos += 1;
	}
	std::string ss(name+pos, epos+1-pos);
	return ss;
}

inline bool startswith(const std::string& src, const std::string& dst){
	return src.compare(0, dst.size(), dst) == 0;
}

inline bool startswith(size_t pos, const std::string& src, const std::string& dst){
	return src.compare(pos, dst.size(), dst) == 0;
}

inline bool endswith(const std::string& src, const std::string& dst){
	size_t dsz = dst.size();
	if(src.size() < dsz) return false;
	return src.compare(src.size()-dsz, dsz, dst) == 0;
}

inline std::string selectafter(const std::string& src, const std::string& dst){
	std::string cn = src.substr(dst.size());
	return trim(cn);
}

inline std::string selectafter(const std::string& src, const std::string& dst, size_t skip){
	std::string cn = src.substr(dst.size()+skip);
	return trim(cn);
}

struct CruncherException : public std::exception {
   std::string s;
   CruncherException(const std::string& ss) : s(ss) {}
   const char* what() const throw() { return s.c_str(); }
   ~CruncherException() throw() {};
};

#define MAX_NAME_LENGTH 256

#endif /* COMMON_H_ */
