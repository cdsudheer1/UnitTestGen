/** ************************************************************************************
 *
 *          @file  UnitTestCreator.cpp
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 14:34:39
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



#include <UnitTestCreator.h>
#include <HeaderFileParser.h>



String Settings::SourceFile;
StringVector Settings::IncludeDirs;
String Settings::OutputDir;
bool Settings::Debug = false;
bool Settings::WithoutNameSpace = false;
String Settings::CommentHeaderFile;




UnitTestCreator::UnitTestCreator()
{

}

UnitTestCreator::~UnitTestCreator()
{

}

bool UnitTestCreator::GenerateTestCases()
{
    HeaderFile oHeaderFile(Settings::SourceFile);
    m_Context.pFile = &oHeaderFile;
    m_Context.HeaderFileIndex++;

    oHeaderFile.Parsed = HeaderFileParser::Parse(m_Context);
    if (oHeaderFile.Parsed) {
        oHeaderFile.CreateUnitTestFiles(m_Context);
    }

    return false;
}
