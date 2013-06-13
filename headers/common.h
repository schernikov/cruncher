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

// trim from start
inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
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
