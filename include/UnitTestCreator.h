/** ************************************************************************************
 *
 *          @file  UnitTestCreator.h
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 14:31:45
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


#ifndef UNIT_TEST_CREATOR_H
#define UNIT_TEST_CREATOR_H

#include <Types.h>
#include <HeaderFile.h>

using namespace std;





class UnitTestCreator
{
    public:
        UnitTestCreator();
        virtual ~UnitTestCreator();
        bool GenerateTestCases();


    private:
        AppContext m_Context;
        Vector<HeaderFile> HeaderFiles;

};

#endif // UNIT_TEST_CREATOR_H
