/** ************************************************************************************
 *
 *          @file  Class.h
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 14:20:00
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

#ifndef CPP_CLASS_H
#define CPP_CLASS_H

#include <Types.h>
#include <utility>
#include <regex>
#include <fstream>


using std::regex;
using std::pair;
using std::fstream;


enum ACCESS_LEVEL
{
    ACCESS_PRIVATE=0,
    ACCESS_PROTECTED,
    ACCESS_PUBLIC
};

enum VARIABLE_TYPE
{
    VT_UNKNOWN=0,

    VT_CLASS,
    VT_STRUCT,

    VT_VOID,
    VT_BOOL,
    VT_CHAR,
    VT_UCHAR,
    VT_INT,
    VT_UINT,
    VT_SHORT,
    VT_USHORT,
    VT_LONG,
    VT_ULONG,
    VT_LONG_LONG,
    VT_ULONG_LONG,
    VT_INT8,
    VT_UINT8,
    VT_INT16,
    VT_UINT16,
    VT_INT32,
    VT_UINT32,
    VT_INT64,
    VT_UINT64,
    VT_WCHAR,
    VT_FLOAT,
    VT_DOUBLE,
    VT_LONG_DOUBLE
};

typedef Vector<pair<regex, VARIABLE_TYPE>> VariableTypeVector;
typedef Map<VARIABLE_TYPE, String> VariableTypeStrMap;

enum CLASS_TYPE
{
    CLASS_TYPE_CLASS=0,
    CLASS_TYPE_STRUCT
};

struct Enum_t
{
    String Name;
    StringVector Values;
    void Clear();
};

#define V_FLAG_CONST                (1<<0)
#define V_FLAG_PTR                  (1<<1)
#define V_FLAG_REF                  (1<<2)
#define V_FLAG_HAS_DEFAULT_VALUE    (1<<3)

struct Variable
{
    VARIABLE_TYPE Type=VT_UNKNOWN;
    unsigned Flags=0;
    String Name;

    String TypeString();
    static const VariableTypeVector VTList;
    static VariableTypeStrMap VTNames;
};

struct MemberVariable: public Variable
{
    ACCESS_LEVEL AccessLevel=ACCESS_PRIVATE;
};

#define FN_CONSTRUCTOR  (1<<0)
#define FN_DESTRUCTOR   (1<<1)
#define FN_STATIC       (1<<2)
#define FN_INLINE       (1<<3)
#define FN_CONST        (1<<4)
#define FN_MUTABLE      (1<<5)
#define FN_VIRTUAL      (1<<6)


struct AppContext;
struct Class;

struct Function
{
    ACCESS_LEVEL AccessLevel=ACCESS_PRIVATE;
    Variable RetVal;
    String Name;
    unsigned Flags=0;
    Vector<Variable> Arguments;

public:
    void WriteTestCase(fstream& rFile, String& sIndent, AppContext& rCtx, const Class& rClass);
    String ToString(String& rClassName);

    void GetFunctionArgumentVariables(StringVector& vVariableStmts, StringVector& vArgs, String& rRetVal,
                                      String sVariablePrefix);
    void GenerateVariableDeclarationAndName(Variable &rV, String& sType, String& sName,
                                      unsigned ArgumentIndex, bool bReturnValue, String sVariablePrefix);
};


typedef Vector<pair<int, String>> TestFunctionVector;

struct Class
{
    CLASS_TYPE Type;
    String Name;
    ACCESS_LEVEL AccessLevel = ACCESS_PRIVATE;
    unsigned Flags=0;
    Vector<Function> Functions;
    Vector<MemberVariable> Variables;
    Vector<Enum_t> Enums;



    Class();
    bool CreateTestCaseHeader(fstream& rFile, int indent, AppContext& rCtx);
    bool CreateTestCaseSource(fstream& rFile, int indent, AppContext& rCtx);

protected:
    TestFunctionVector TestFunctions;

    void FindTestFunctions(AppContext& rCtx);
protected:
    String TestClassName;
};


#endif // CPP_CLASS_H
