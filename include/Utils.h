/** ************************************************************************************
 *
 *          @file  utils.h
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 14:27:48
 *
 *        @author  Sudheer Divakaran, Gadgeon Smart Systems Pvt. Ltd, Kochi, Kerala, INDIA.
 *
 *		 @copyright Licensed under GPLv2, see the file LICENSE in this source tree.
 *
 *\if NODOC
 *       Revision History:
 *				27.05.2020 - Sudheer Divakaran - Created
 *\endif
 ***************************************************************************************/

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <string>
#include <algorithm>
#include <Types.h>


extern void Log(const char* format, ...);



// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

StringVector TokenizeString(String& rString, bool bSplitAtComma=false);
StringVector SplitArguments(String& rString);


#endif // UTILS_H_INCLUDED
