/** ************************************************************************************
 *
 *          @file  Utils.cpp
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 15:03:45
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

#include <stdarg.h>
#include <stdio.h>
#include <Utils.h>

void Log(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

StringVector TokenizeString(String& rString, bool bSplitAtComma)
{
    char prev, ch;
    String str;
    StringVector v;
    std::size_t i, len;
    bool bStringLiteral=false;

    len = rString.length();
    prev = ch= 0;
    for (i=0; i < len; i++) {
        prev = ch;
        ch = rString[i];
        if(std::isspace(ch) || (bSplitAtComma && (','==ch))) {
            if (bStringLiteral) {
                str.push_back(ch);
                continue;
            } else {
                if (!str.empty()) {
                    v.push_back(str);
                    str.clear();
                    if (','==ch) {
                        v.push_back(",");
                    }
                }
            }
        } else if ('"' == ch) {
            if (bStringLiteral) {
                if ('\\' == prev) {
                    str.push_back(ch);
                    continue;
                } else {
                    bStringLiteral = false;
                    v.push_back(str);
                    str.clear();
                    continue;
                }
            } else {
                bStringLiteral = true;
                continue;
            }
        } else {
            str.push_back(ch);
        }
    }
    if (!str.empty()) {
        v.push_back(str);
    }
    return v;
}

StringVector SplitArguments(String& rString)
{
    char prev, ch;
    String str;
    StringVector v;
    std::size_t i, len;
    bool bStringLiteral=false;
    trim(rString);
    len = rString.length();
    prev = ch= 32;
    for (i=0; i < len; i++) {
        prev = ch;
        ch = rString[i];
        if(std::isspace(ch) || (',' == ch)) {
            if (bStringLiteral) {
                str.push_back(ch);
                continue;
            } else if (',' == ch){
                ch = 32;
                trim(str);
                if (!str.empty()) {
                    v.push_back(str);
                    str.clear();
                }
            } else if (!std::isspace(prev)) {
                str.push_back(ch);
            }
        } else if ('"' == ch) {
            if (bStringLiteral) {
                if ('\\' == prev) {
                    str.push_back(ch);
                    continue;
                } else {
                    bStringLiteral = false;
                    trim(str);
                    v.push_back(str);
                    str.clear();
                    continue;
                }
            } else {
                bStringLiteral = true;
                continue;
            }
        } else {
            str.push_back(ch);
        }
    }
    trim(str);
    if (!str.empty()) {
        v.push_back(str);
    }
    return v;
}
