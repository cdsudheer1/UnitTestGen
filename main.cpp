/** ************************************************************************************
 *
 *	@file  main.cpp
 *
 *	@brief
 *
 *	@version  1.0
 *	@date  27.05.2020 14:35:11
 *
 *	@author  Sudheer Divakaran, Gadgeon Smart Systems Pvt. Ltd, Kochi, Kerala, INDIA.
 *
 *
 *
 *	@copyright Licensed under GPLv2, see the file LICENSE in this source tree.
 *
 *\if NODOC
 *       Revision History:
 *				27.05.2020 - Sudheer Divakaran - Created
 *\endif
 ***************************************************************************************/



#include <stdio.h>
#include <types.h>
#include <XGetopt.h>
#include <UnitTestCreator.h>


using namespace std;

void Usage()
{
printf("Usage: %s [options]\n\
Options can be:\n\
\t-f <input_header_file>\n\
\t[-o output_folder]\n\
\t[-C CommentFile]\n\
\t[-I include_path]\n\
\t[-N (Omit namespaces)]\n\
\t[-d (Debug Mode)]\n", APP_NAME);
}

int main(int argc, char* argv[])
{
    int c;

    printf("%s Version:%s Built on %s %s\n", APP_NAME, APP_VERSION, __DATE__, __TIME__);
    printf("%s\n\n", APP_COPYRIGHT);

    while ((c = getopt(argc, argv, "C:df:I:o:N")) != EOF)
    {
        switch (c) {
            case 'C':
                Settings::CommentHeaderFile = optarg;
                break;
            case 'd':
                Settings::Debug = true;
                break;
            case 'f':
                Settings::SourceFile=optarg;
                break;
            case 'I':
                Settings::IncludeDirs.push_back(optarg);
                break;
            case 'o':
                Settings::OutputDir = optarg;
                break;
            case 'N':
                Settings::WithoutNameSpace = true;
                break;
            default:
                printf("Unsupported option found!!\n\n");
                Usage();
                return -1;
                break;
        }
    }

    if (false == Settings::SourceFile.empty()) {
        UnitTestCreator *pUnitTestCreator = new UnitTestCreator();
        pUnitTestCreator->GenerateTestCases();
        delete pUnitTestCreator;
    } else {
        Usage();
        return -1;
    }
    return 0;
}
