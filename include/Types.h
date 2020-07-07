/** ************************************************************************************
 *
 *          @file  Types.h
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 14:30:59
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

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <set>

#include <Constants.h>

#define String std::string
#define Vector std::vector
#define Map std::map
#define Set std::set
#define OStringStream std::ostringstream
typedef Vector<String> StringVector;
typedef Vector<int> IntVector;


struct Settings
{
    static String SourceFile;
    static StringVector IncludeDirs;
    static String OutputDir;
    static bool Debug;
    static bool WithoutNameSpace;
    static String CommentHeaderFile;
};

#endif // TYPES_H_INCLUDED
