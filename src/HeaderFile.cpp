/**************************************************************************************
 *
 *	@file  HeaderFile.cpp
 *
 *    	@brief
 *
 *      @version  1.0
 *      @date  27.05.2020 14:34:25
 *
 *      @author  Sudheer Divakaran, Gadgeon Smart Systems Pvt. Ltd, Kochi, Kerala, INDIA.
 *
 *	@copyright Licensed under GPLv2, see the file LICENSE in this source tree.
 *
 *\if NODOC
 *       Revision History:
 *		27.05.2020 - Sudheer Divakaran - Created
 *\endif
 ***************************************************************************************/



#include <HeaderFile.h>
#include <Utils.h>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::fstream;

regex RE::CPP_STYLE_COMMENT("^([^/]*)//.*$");
regex RE::ONE_LINE_COMMENT("^([^/]*)/\\*([^\\*]*)\\*/(.*)$");
String RE::COMMENT_START("/*");
String RE::COMMENT_END("*/");
Vector<regex> RE::IgnoreList = {
                                regex("^\\s*#.*$"),
                               };
regex RE::INCLUDE_FILE("^\\s*#include\\s+[<|\"]([^>\"]+)[>|\"]$");
regex RE::NAMESPACE("^\\s*namespace\\s+(\\w*)\\s*(\\{?)");
regex RE::VARIABLE("^[\\w\\s_=,:\\[\\]\\*<>]+[=\\w()^\\s\\|\\&\\!]*;$");
regex RE::FUNCTION_DECL("^(([\\w\\s\\*\\&]*?)([\\w\\s_]+)\\(([\\w\\s_,\\*\\&=]*)\\)(\\s*[a-z]*\\s*))\\;$");
regex RE::FUNCTION_DECL_START("^([\\w\\s\\*\\&]*?)([\\w\\s_]+)\\(");
regex RE::PARSED_FN("^([\\w_:\\*\\s\\&]*?)\\s*([\\w_~:]+)\\(([^\\)]*)\\)$");
regex RE::CLASS("^\\s*(class|struct)\\s+(\\w+)\\s*([\\:|\\{|\\;]*)");
regex RE::PRIVATE("^\\s*private\\s*:$");
regex RE::PROTECTED("^\\s*protected\\s*:$");
regex RE::PUBLIC("^\\s*public\\s*:$");
regex RE::ENUM("\\s*enum\\s*(class)?([^\\;\\{]*)(\\;|\\{)?");
regex RE::ENUM_MEMBER("\\s*(\\w*)\\s*(=)?\\s*([\\w\\s]*)(,|})?");

regex RE::FN_ATTR_STATIC("static\\s+");
regex RE::FN_ATTR_INLINE("inline\\s+");
regex RE::FN_ATTR_CONST("\\)\\s+const\\s*$");
regex RE::FN_ATTR_MUTABLE("\\)\\s+mutable\\s*$");
regex RE::FN_ATTR_VIRTUAL("\\s*virtual\\s+");
regex RE::FN_ATTR_EXPLICIT("\\s*explicit\\s+");

bool Namespace::CreateTestCaseHeader(fstream& rFile, int indent, AppContext& rCtx)
{
    String sIndent;
    if (indent) {
        sIndent = String(indent, CHR_TAB);
    }
    String sMemberIndent(sIndent);


    std::size_t i;
    rCtx.vNamespaceNames.push_back(Name);
    if (!Settings::WithoutNameSpace) {
        rFile << sIndent << "namespace " << Name << " {" << endl;
    }

    for (i=0; i < Classes.size(); i++) {
        Class& rClass = Classes[i];
        rClass.CreateTestCaseHeader(rFile, indent, rCtx);
    }

    for (i=0; i < NameSpaces.size(); i++) {
        Namespace& rNs = NameSpaces[i];
        rNs.CreateTestCaseHeader(rFile, indent, rCtx);
    }

    if (!Settings::WithoutNameSpace) {
        rFile << sIndent << "} //End " << Name  << endl;
    }
    rCtx.vNamespaceNames.pop_back();

    return true;
}

bool Namespace::CreateTestCaseSource(fstream& rFile, int indent, AppContext& rCtx)
{
    String sIndent;
    if (indent) {
        sIndent = String(indent, CHR_TAB);
    }
    String sMemberIndent(sIndent);


    std::size_t i;
    rCtx.vNamespaceNames.push_back(Name);
    if (!Settings::WithoutNameSpace) {
        rFile << sIndent << "namespace " << Name << " {" << endl;
    }

    for (i=0; i < Classes.size(); i++) {
        Class& rClass = Classes[i];
        rClass.CreateTestCaseSource(rFile, indent, rCtx);
    }

    for (i=0; i < NameSpaces.size(); i++) {
        Namespace& rNs = NameSpaces[i];
        rNs.CreateTestCaseSource(rFile, indent, rCtx);
    }
    if (!Settings::WithoutNameSpace) {
        rFile << sIndent << "} //End " << Name  << endl;
    }
    rCtx.vNamespaceNames.pop_back();


    return true;
}


AppContext::AppContext()
{
    Init();
}

void AppContext::Reset()
{
    pFile = nullptr;
    LineNo = 0;
    FunctionName.clear();
    Blocks.clear();
    BlockIndex = -1;
    CommentBlockStarted = false;
    ReProcessLine = false;
    State = STATE_NONE;
    MultiLineExpr.clear();
    BracketCount=0;

    Enum.Clear();
}

void AppContext::Init()
{
    Reset();
    HeaderFileIndex = -1;
    SymbolTable.clear();
}

void* AppContext::GetActiveNameSpace()
{
    void* pResult = nullptr;
    std::size_t i, size = Blocks.size();
    if (size > 0) {
        for (i=size-1; size; i--) {
            if (BLOCK_NAMESPACE == Blocks[i].Type) {
                pResult = Blocks[i].pBlock;
                break;
            }
            size--;
        }
    }
    return pResult;
}



void CodeGenParams::Prepare(String& rsOutputDir, String& rFile)
{
    String::size_type pos;

    sPath = rsOutputDir;
    sFile = rFile;

    trim(sPath);
    if (!sPath.empty()) {
        if (sPath[sPath.length()-1] != PATH_SEPARATOR) {
            sPath.push_back(PATH_SEPARATOR);
        }
    }


    pos = sFile.rfind(PATH_SEPARATOR);
    if (String::npos != pos) {
        sFile.erase(0, pos+1);
    }

    sSourceFile = sFile;

    pos = sSourceFile.find(CHR_DOT);
    if (String::npos != pos) {
        sSourceFile.erase(pos);
    }

    sHeaderFile = sSourceFile + "_Test.h";
    sSourceFile += "_Test.cpp";
    sHeaderGuard = "__" + sHeaderFile + "__";
    std::transform(sHeaderGuard.begin(), sHeaderGuard.end(), sHeaderGuard.begin(), ::toupper);
    std::replace(sHeaderGuard.begin(), sHeaderGuard.end(), CHR_DOT, CHR_UNDERSCORE);
}


HeaderFile::HeaderFile(String& rFileName)
{
    Name = rFileName;
}

HeaderFile::~HeaderFile()
{

}

void DumpNs(Namespace& rNs, int indent)
{
    std::size_t i;
    String sIndent;
    if (indent) {
        sIndent = String(indent, CHR_TAB);
    }

    cout << sIndent << "NameSpace:" << rNs.Name << endl;
    sIndent.push_back(CHR_TAB);


    for (i=0; i < rNs.Functions.size(); i++) {
        cout << sIndent<< "Fn:" << rNs.Functions[i].Name << endl;
    }

    for (i=0; i < rNs.Enums.size(); i++) {
        cout << sIndent<< "Enum:" << rNs.Enums[i].Name << endl;
    }

    for (i=0; i < rNs.Classes.size(); i++) {
        cout << sIndent<< "Class:" << rNs.Classes[i].Name << endl;
        cout << sIndent << CHR_TAB << "Functions:" << rNs.Classes[i].Functions.size() << endl;
    }

    for (i=0; i < rNs.NameSpaces.size(); i++) {
        DumpNs(rNs.NameSpaces[i], indent + 1);
    }
}

void HeaderFile::Dump()
{

    cout << "In Dump" << endl;
    std::size_t i;
    for (i=0; i < Classes.size(); i++) {
        cout << "Class:" << Classes[i].Name << endl;
        cout <<  CHR_TAB << "Functions:" << Classes[i].Functions.size() << endl;
    }

    for (i=0; i < Functions.size(); i++) {
        cout << "Fn:" << Functions[i].Name << endl;
    }
    for (i=0; i < Enums.size(); i++) {
        cout << "Enum:" << Enums[i].Name << endl;
    }

    for (i=0; i < NameSpaces.size(); i++) {
        DumpNs(NameSpaces[i], 0);
    }


}

void HeaderFile::CreateUnitTestFiles(AppContext& rCtx)
{
    rCtx.Params.Prepare(Settings::OutputDir, Name);
    CreateUnitTestHeaderFile(rCtx);
    CreateUnitTestSourceFile(rCtx);
}

void HeaderFile::PrintAppInfo(fstream& rFile)
{
    rFile << "//Auto generated by " << APP_NAME << " Version:" << APP_VERSION << endl << endl;
}

void HeaderFile::AddCustomCommentHeader(fstream& rFile)
{
    if (Settings::CommentHeaderFile.empty()) {
        return;
    }

    fstream File(Settings::CommentHeaderFile, std::ios::in);

    if (!File.is_open()) {
        Log("Couldn't open File:%s\n", Settings::CommentHeaderFile.c_str());
        return;
    }

    String line;

    while (std::getline(File, line)) {
        rFile << line << endl;
    }
    File.close();
}



bool HeaderFile::CreateUnitTestHeaderFile(AppContext& rCtx)
{
    String sFile = rCtx.Params.sPath + rCtx.Params.sHeaderFile;

    cout << "Generating " << sFile << endl;

    fstream File(sFile, std::ios::out | std::ios::trunc);


    if (!File.is_open()) {
        Log("Couldn't open File:%s\n", sFile.c_str());
        return false;
    }

    PrintAppInfo(File);
    AddCustomCommentHeader(File);
    File << "#include <" << rCtx.Params.sFile << ">" << endl << endl;
    File << "#ifndef " << rCtx.Params.sHeaderGuard << endl;
    File << "#define " << rCtx.Params.sHeaderGuard << endl << endl;

    File << "#include <cppunit" << PATH_SEPARATOR << "extensions" << PATH_SEPARATOR << "HelperMacros.h>" << endl << endl;

    std::size_t i;
    for (i = 0; i < Classes.size(); i++) {
        Class& rClass = Classes[i];
        rClass.CreateTestCaseHeader(File, 0, rCtx);
    }

    for (i = 0; i < NameSpaces.size(); i++) {
        Namespace& rNs = NameSpaces[i];
        rNs.CreateTestCaseHeader(File, 0, rCtx);
    }

    File << endl << "#endif" << endl;

    return true;
}

bool HeaderFile::CreateUnitTestSourceFile(AppContext& rCtx)
{
    String sFile = rCtx.Params.sPath + rCtx.Params.sSourceFile;

    cout << "Generating " << sFile << endl;

    fstream File(sFile, std::ios::out | std::ios::trunc);


    if (!File.is_open()) {
        Log("Couldn't open File:%s\n", sFile.c_str());
        return false;
    }

    PrintAppInfo(File);
    AddCustomCommentHeader(File);

    File << "#include <" << rCtx.Params.sHeaderFile << ">" << endl << endl;

    std::size_t i;
    for (i = 0; i < Classes.size(); i++) {
        Class& rClass = Classes[i];
        rClass.CreateTestCaseSource(File, 0, rCtx);
    }

    for (i = 0; i < NameSpaces.size(); i++) {
        Namespace& rNs = NameSpaces[i];
        rNs.CreateTestCaseSource(File, 0, rCtx);
    }

    return true;
}

