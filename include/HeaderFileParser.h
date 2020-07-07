/** ************************************************************************************
 *
 *          @file  HeaderFileParser.h
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 16:33:43
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

#ifndef HEADER_FILE_PARSER_H
#define HEADER_FILE_PARSER_H

#include <Types.h>
#include <HeaderFile.h>
#include <regex>

using std::regex;


class HeaderFileParser
{
private:
    static void ParseFunction(AppContext& rCtx, String& rLine);
    static bool ParseFunctionArgs(AppContext& rCtx, Function& rFn, String& rArgs);
    static bool ParseVariable(AppContext& rCtx, Variable &rVariable, String& rExpr, bool bRetVal);

public :
    static bool Parse(AppContext& rCtx);

};

#endif // HEADER_FILE_PARSER_H
