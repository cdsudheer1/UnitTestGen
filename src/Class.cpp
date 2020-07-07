/** ************************************************************************************
 *
 *          @file  Class.cpp
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 14:34:13
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

#include <Class.h>
#include <HeaderFile.h>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;

using std::endl;

const VariableTypeVector  Variable::VTList = {
    {
        regex("void\\s*"), VT_VOID
        //void
    },
    {
        regex("bool\\s*"), VT_BOOL
        //bool
    },
    {
        regex("unsigned\\s+char\\s*"), VT_UCHAR
        //unsigned char
    },
    {
        regex("char\\s*"), VT_CHAR
        //char
    },
    {
        regex("unsigned\\s+long\\s+long\\s+int\\s*"), VT_ULONG_LONG
        //unsigned long long int
    },
    {
        regex("unsigned\\s+long\\s+long\\s*"), VT_ULONG_LONG
        //unsigned long long
    },
    {
        regex("unsigned\\s+short\\s+int\\s*"), VT_USHORT
        //unsigned short int
    },
    {
        regex("unsigned\\s+short\\s*"), VT_USHORT
        //unsigned short
    },
    {
        regex("unsigned\\s+long\\s+int\\s*"), VT_ULONG
        //unsigned long int
    },
    {
        regex("unsigned\\s+long\\s*"), VT_ULONG
        //unsigned long
    },
    {
        regex("long\\s+long\\s+int\\s*"), VT_LONG_LONG
        //long long int
    },
    {
        regex("long\\s+double\\s*"), VT_LONG_DOUBLE
        //long double
    },
    {
        regex("double\\s*"), VT_DOUBLE
        //double
    },
    {
        regex("float\\s*"), VT_FLOAT
        //float
    },
    {
        regex("unsigned\\s+int\\s*"), VT_UINT
        //unsigned int
    },
    {
        regex("long\\s+int\\s*"), VT_LONG
        //long int
    },
    {
        regex("short\\s+int\\s*"), VT_SHORT
        //short int
    },
    {
        regex("unsigned\\s*"), VT_UINT
        //unsigned
    },
    {
        regex("wchar_t\\s*"), VT_WCHAR
        //wchar_t
    },

    {
        regex("uint8_t\\s*"), VT_UINT8
        //uint8_t
    },
    {
        regex("int8_t\\s*"), VT_INT8
        //int8_t
    },
    {
        regex("uint16_t\\s*"), VT_UINT16
        //uint16_t
    },
    {
        regex("int16_t\\s*"), VT_INT16
        //int16_t
    },
    {
        regex("uint32_t\\s*"), VT_UINT32
        //uint32_t
    },
    {
        regex("int32_t\\s*"), VT_INT32
        //int32_t
    },
    {
        regex("uint64_t\\s*"), VT_UINT64
        //uint64_t
    },
    {
        regex("int64_t\\s*"), VT_INT64
        //int64_t
    },

    {
        regex("short\\s*"), VT_SHORT
        //short
    },
    {
        regex("long\\s*"), VT_LONG
        //long
    },
    {
        regex("int\\s*"), VT_INT
        //int
    }
};


VariableTypeStrMap Variable::VTNames = {
    { VT_VOID, "void" },
    { VT_BOOL, "bool" },
    { VT_CHAR, "char" },
    { VT_UCHAR, "unsigned char" },
    { VT_INT, "int" },
    { VT_UINT, "unsigned" },
    { VT_SHORT, "short" },
    { VT_USHORT, "unsigned short" },
    { VT_LONG, "long" },
    { VT_ULONG, "unsigned long" },
    { VT_LONG_LONG, "long long" },
    { VT_ULONG_LONG, "unsigned long long" },
    { VT_INT8, "int8_t" },
    { VT_UINT8, "uint8_t" },
    { VT_INT16, "int16_t" },
    { VT_UINT16, "uint16_t" },
    { VT_INT32, "int32_t" },
    { VT_UINT32, "uint32_t" },
    { VT_INT64, "int64_t" },
    { VT_UINT64, "uint64_t" },
    { VT_WCHAR, "wchar_t" },
    { VT_FLOAT, "float" },
    { VT_DOUBLE, "double" },
    { VT_LONG_DOUBLE, "long double" }
};


String Variable::TypeString()
{
    OStringStream vtype;

    if (Flags & V_FLAG_CONST) {
        vtype << STR_CONST << CHR_SPACE;
    }

    vtype << Name;

    if (Flags & V_FLAG_PTR) {
        vtype << CHR_STAR;
    }

    if (Flags & V_FLAG_REF) {
        vtype << CHR_REF;
    }

    return vtype.str();
}

void Enum_t::Clear()
{
    Name.clear();
    Values.clear();
}


Class::Class():
    Type(CLASS_TYPE_CLASS)
{

}


void Class::FindTestFunctions(AppContext& rCtx)
{
    TestFunctions.clear();
    if (CLASS_TYPE_STRUCT == Type) {
        return;
    }

    std::size_t i,j;
    Set<String> TestFnSet;
    String sTemp;
    TestClassName = Name + "_Test";
    for (i=0; i < Functions.size(); i++) {
        Function& rFn = Functions[i];
        if (ACCESS_PRIVATE == rFn.AccessLevel) {
            cout << "Skipping Private Function:" << rFn.Name << endl;
            continue;
        }

        if (rFn.Flags & FN_DESTRUCTOR) {
            continue;
        }

        j = 0;
        do {
            sTemp = "Test_";
            if (rFn.Flags & FN_CONSTRUCTOR) {
                sTemp.append("Constructor");
                if (1==j) {
                    j++;
                }
            } else {
                sTemp.append(rFn.Name);
            }
            if (j) {
                sTemp.append(std::to_string(j));
            }

            if (TestFnSet.find(sTemp) == TestFnSet.end()) {
                break;
            }
            j++;
        } while (true);
        TestFunctions.push_back(std::make_pair(i, sTemp));
        TestFnSet.insert(sTemp);

    }

    bool bConstructorVarsCreated = false;
    rCtx.Params.vActiveClassConstructorArgsDecls.clear();
    rCtx.Params.vActiveClassConstructorArgNames.clear();
    if (!TestFunctions.empty()) {
        for (i=0; i < TestFunctions.size(); i++) {
            Function& rFn = Functions[TestFunctions[i].first];
            if (rFn.Flags & FN_CONSTRUCTOR) {
                bConstructorVarsCreated = true;
                rFn.GetFunctionArgumentVariables(rCtx.Params.vActiveClassConstructorArgsDecls,
                                                rCtx.Params.vActiveClassConstructorArgNames,
                                                 rCtx.Params.sActiveClassVarName, STR_CLASS_ARGUMENT_PREFIX);
                break;
            }
        }
    }
    if (!bConstructorVarsCreated) {
        OStringStream var;
        var << Name << CHR_STAR << CHR_SPACE << STR_NAME_CLASS_VARIABLE;
        rCtx.Params.sActiveClassVarName = var.str();

    }
}

bool Class::CreateTestCaseHeader(fstream& rFile, int indent, AppContext& rCtx)
{
    if (CLASS_TYPE_STRUCT == Type) {
        return true;
    }

    FindTestFunctions(rCtx);

    if (TestFunctions.empty()) {
        return true;
    }

    String sIndent;
    if (indent) {
        sIndent = String(indent, CHR_TAB);
    }
    String sMemberIndent(sIndent);
    sMemberIndent.push_back(CHR_TAB);

    std::size_t i;
    String sFnName;

    if (Settings::WithoutNameSpace) {
        if(!rCtx.vNamespaceNames.empty()) {
            rFile << sIndent << "using ";
            for (i=0; i < rCtx.vNamespaceNames.size(); i++) {
                rFile << rCtx.vNamespaceNames[i] << STR_SCOPE;
            }
            rFile << Name << CHR_SEMI_COLUMN << endl << endl;
        }
    }

    rFile << sIndent << "class " << TestClassName << ": public CppUnit::TestFixture, public " << Name
        << endl << CHR_BRACE_OPEN << endl << endl;

    rFile << sMemberIndent << "CPPUNIT_TEST_SUITE(" << TestClassName << ");" << endl;
    for (i=0; i < TestFunctions.size(); i++) {
        sFnName = TestFunctions[i].second;
        rFile << sMemberIndent << "CPPUNIT_TEST(" << sFnName << ");" << endl;
    }
    rFile << sMemberIndent << "CPPUNIT_TEST_SUITE_END();" << endl << endl;
    rFile << sIndent << "public:" << endl;
    rFile << sMemberIndent << TestClassName << "();" << endl;
    rFile << sMemberIndent << CHR_TILD << TestClassName << "();" << endl << endl;
    rFile << sMemberIndent << "virtual void setUp();" << endl;
    rFile << sMemberIndent << "virtual void tearDown();" << endl << endl;

    for (i=0; i < TestFunctions.size(); i++) {
        sFnName = TestFunctions[i].second;
        rFile << sMemberIndent << "void " << sFnName << "();" << endl;
    }
    rFile << sIndent << CHR_BRACE_CLOSE << CHR_SEMI_COLUMN << " //End class " << TestClassName << endl << endl;

    return true;
}

bool Class::CreateTestCaseSource(fstream& rFile, int indent, AppContext& rCtx)
{
    if (CLASS_TYPE_STRUCT == Type) {
        return true;
    }

    if (TestFunctions.empty()) {
        return true;
    }

    String sIndent;
    if (indent) {
        sIndent = String(indent, CHR_TAB);
    }
    String sMemberIndent(sIndent);
    sMemberIndent.push_back(CHR_TAB);

    std::size_t i,index;
    String sFnName;
    rFile << endl << sIndent << "CPPUNIT_TEST_SUITE_REGISTRATION(" << TestClassName << ");" << endl << endl;

    //Constructor
    rFile << "//TODO: Validate Baseclass Initialization (if any)" << endl;
    rFile << TestClassName << STR_SCOPE << TestClassName <<
     "()" << endl << CHR_BRACE_OPEN << endl << CHR_BRACE_CLOSE << endl << endl;

    rFile << TestClassName << STR_SCOPE << CHR_TILD << TestClassName <<
     "()" << endl << CHR_BRACE_OPEN << endl << CHR_BRACE_CLOSE << endl << endl;

    rFile << TestClassName << STR_SCOPE << TestClassName <<
     "()" << endl << CHR_BRACE_OPEN << endl << CHR_BRACE_CLOSE << endl << endl;

    rFile << Variable::VTNames[VT_VOID] << " " << TestClassName << STR_SCOPE << "setUp()" << endl << CHR_BRACE_OPEN << endl;
    rFile << CHR_TAB << "CppUnit::TestFixture::setUp();" << endl << CHR_BRACE_CLOSE << endl << endl;

    rFile << Variable::VTNames[VT_VOID] << " " << TestClassName << STR_SCOPE << "tearDown()" << endl << CHR_BRACE_OPEN << endl;
    rFile << CHR_TAB << "CppUnit::TestFixture::tearDown();" << endl << CHR_BRACE_CLOSE << endl << endl;


    for (i=0; i < TestFunctions.size(); i++) {
        index = TestFunctions[i].first;
        sFnName = TestFunctions[i].second;
        Function& rFn = Functions[index];
        rFile << "//Test " << rFn.ToString(Name) << endl;
        rFile << "void " << TestClassName << STR_SCOPE << sFnName << "()" << endl << CHR_BRACE_OPEN << endl;
        rFn.WriteTestCase(rFile, sMemberIndent, rCtx, *this);
        rFile << CHR_BRACE_CLOSE << endl << endl;
    }

    return true;
}

void Function::GenerateVariableDeclarationAndName(Variable &rV, String& sDecl, String& sName,
                                      unsigned ArgumentIndex, bool bReturnValue, String sVariablePrefix)
{
    OStringStream decl, arg;
    sDecl.clear();
    sName.clear();


    if (rV.Flags & V_FLAG_CONST) {
        decl << STR_CONST << CHR_SPACE;
    }

    if (VT_VOID == rV.Type) {
        return;
    }

    decl << rV.Name;

    if (rV.Flags & V_FLAG_PTR) {
        decl << CHR_STAR;
    }

    if (rV.Flags & V_FLAG_REF) {
        decl << CHR_REF;
    }

    decl << CHR_SPACE;

    if (bReturnValue) {
        decl << STR_NAME_RETURN_VALUE;
        arg << STR_NAME_RETURN_VALUE;
    } else {
        decl << sVariablePrefix << (ArgumentIndex+1);
        arg << sVariablePrefix << (ArgumentIndex+1);
    }

    sDecl = decl.str();
    sName = arg.str();
}

void Function::GetFunctionArgumentVariables(StringVector& vVariableStmts,
                                StringVector& vNames, String& rRetVal, String sVariablePrefix)
{
    vNames.clear();
    vVariableStmts.clear();
    rRetVal.clear();

    String sVarDeclaration, sVarName;

    if (Flags & FN_CONSTRUCTOR) {
        OStringStream var;
        var << Name << CHR_STAR << CHR_SPACE << STR_NAME_CLASS_VARIABLE;
        rRetVal = var.str();

    } else {
        GenerateVariableDeclarationAndName(RetVal, rRetVal, sVarName,
                                      0, true, sVariablePrefix);
    }

    std::size_t i;
    for (i=0; i < Arguments.size(); i++) {
        GenerateVariableDeclarationAndName(Arguments[i], sVarDeclaration, sVarName,
                                      i, false, sVariablePrefix);
        vVariableStmts.push_back(sVarDeclaration);
        vNames.push_back(sVarName);
    }
}


void Function::WriteTestCase(fstream& rFile, String& sIndent, AppContext& rCtx, const Class& rClass)
{
    String sRetVal;
    StringVector vVariableStmts, vNames;

    bool bEmptyClassArgs = true;

    std::size_t i;

    if (!((FN_CONSTRUCTOR | FN_STATIC) & Flags)) {
        StringVector& vDecls = rCtx.Params.vActiveClassConstructorArgsDecls;
        StringVector& vNames = rCtx.Params.vActiveClassConstructorArgNames;
        if (!vDecls.empty()) {
            bEmptyClassArgs = false;
            for (i=0; i < vDecls.size(); i++) {
                rFile << sIndent << vDecls[i] << CHR_SPACE << CHR_EQUALTO << CHR_SPACE
                    << CHR_SEMI_COLUMN << STR_MSG_INITIALIZE_VARIABLE << endl;
            }
            rFile << endl;
        }
        rFile << sIndent << rCtx.Params.sActiveClassVarName << CHR_SPACE << CHR_EQUALTO
            << CHR_SPACE << STR_NEW << CHR_SPACE << rClass.Name << CHR_BRACKET_OPEN;
        if (!bEmptyClassArgs) {
            for (i=0; i < vNames.size(); i++) {
                if (i) {
                    rFile << CHR_COMMA << CHR_SPACE;
                }
                rFile << vNames[i];
            }
        }
        rFile << CHR_BRACKET_CLOSE << CHR_SEMI_COLUMN << endl << endl;
    }

    GetFunctionArgumentVariables(vVariableStmts, vNames, sRetVal, (FN_CONSTRUCTOR & Flags) ? STR_CLASS_ARGUMENT_PREFIX : STR_FN_ARGUMENT_PREFIX);

    if (!vVariableStmts.empty()) {
        for (i=0; i < vVariableStmts.size(); i++) {
            rFile << sIndent <<  vVariableStmts[i]  << CHR_SPACE <<
                CHR_EQUALTO << CHR_SPACE << CHR_SEMI_COLUMN << STR_MSG_INITIALIZE_VARIABLE << endl;
        }
    }

    if (!sRetVal.empty()) {
        rFile << sIndent << sRetVal << CHR_SPACE << CHR_EQUALTO  << CHR_SPACE ;
    } else {
        rFile << sIndent;
    }

    if (Flags & FN_CONSTRUCTOR) {
        rFile << STR_NEW << CHR_SPACE;
    } else if (Flags & FN_STATIC) {
        rFile << rClass.Name << STR_SCOPE;
    } else {
        rFile << STR_NAME_CLASS_VARIABLE << STR_PTR_MEMBER;
    }

    rFile << Name << CHR_BRACKET_OPEN;

    for (i=0; i < vNames.size(); i++) {
        if (i) {
            rFile << CHR_COMMA << CHR_SPACE;
        }
        rFile << vNames[i];
    }
    rFile << CHR_BRACKET_CLOSE << CHR_SEMI_COLUMN << endl;

    rFile << sIndent << "//TODO: Add Various CPPUNIT_ASSERTs (e.g. CPPUNIT_ASSERT_EQUAL) with different function args" << endl << endl;

    if ((!(FN_CONSTRUCTOR & Flags)) && (!vNames.empty())) {
        rFile << sIndent << "//TODO: Cleanup Function Argument variables (if any)" << endl << endl;
    }

    if (!(Flags & FN_STATIC)) {
        rFile << sIndent << STR_DELETE << CHR_SPACE << STR_NAME_CLASS_VARIABLE << CHR_SEMI_COLUMN << endl;
        if ((!bEmptyClassArgs) || ((FN_CONSTRUCTOR & Flags) && (!vNames.empty())))  {
            rFile << sIndent << "//TODO: Cleanup class argument variables (if any)" << endl;
        }
    }
}

String Function::ToString(String& rClassName)
{
    OStringStream decl;


    if (Flags & FN_STATIC) {
        decl << STR_STATIC << CHR_SPACE;
    }

    if (Flags & FN_VIRTUAL) {
        decl << STR_VIRTUAL << CHR_SPACE;
    }

    if (Flags & FN_INLINE) {
        decl << STR_INLINE << CHR_SPACE;
    }

    if (!(Flags & (FN_CONSTRUCTOR | FN_DESTRUCTOR))) {
        decl << RetVal.TypeString() << CHR_SPACE;
    }

    decl << rClassName << STR_SCOPE;
    if (FN_DESTRUCTOR & Flags) {
        decl << CHR_TILD;
    }
    decl << Name << CHR_BRACKET_OPEN;

    if (!Arguments.empty()) {
        std::size_t i;
        for (i=0; i < Arguments.size(); i++) {
            if (i) {
                decl << CHR_COMMA << CHR_SPACE;
            }
            decl << Arguments[i].TypeString();
        }
    }

    decl << CHR_BRACKET_CLOSE;

    if (Flags & FN_CONST) {
        decl << CHR_SPACE << STR_CONST;
    }
    decl << CHR_SEMI_COLUMN;

    return decl.str();
}
