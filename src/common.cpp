/*
 * common.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: schernikov
 */

#include <cstdio>
#include <stdarg.h>
#include <vector>

#include "common.h"

static std::string vformat (const char *fmt, va_list ap){
    // Allocate a buffer on the stack that's big enough for us almost
    // all the time.  Be prepared to allocate dynamically if it doesn't fit.
    char stackbuf[1024];
    size_t size = sizeof(stackbuf);
    std::vector<char> dynamicbuf;
    char *buf = &stackbuf[0];

    while (1) {
        // Try to vsnprintf into our buffer.
        int needed = vsnprintf (buf, size, fmt, ap);
        // NB. C99 (which modern Linux and OS X follow) says vsnprintf
        // failure returns the length it would have needed.  But older
        // glibc and current Windows return -1 for failure, i.e., not
        // telling us how much was needed.

        if (needed <= (int)size && needed >= 0) {
            // It fit fine so we're done.
            return std::string (buf, (size_t) needed);
        }

        // vsnprintf reported that it wanted to write more characters
        // than we allotted.  So try again using a dynamic buffer.  This
        // doesn't happen very often if we chose our initial size well.
        size = (needed > 0) ? (needed+1) : (size*2);
        dynamicbuf.resize (size);
        buf = &dynamicbuf[0];
    }
    return "";
}

CruncherException::CruncherException(const char * format, ... ){
  va_list args;
  va_start (args, format);
  s = vformat (format, args);
  va_end (args);
}