/** ************************************************************************************
 *
 *          @file  HeaderFile.h
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 14:20:45
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

#ifndef CPP_HEADER_FILE_H
#define CPP_HEADER_FILE_H

#include <Class.h>


class Namespace
{
public:
    String Name;
    //int ClassIndex = -1;
    IntVector ClassIndexes;
    Vector<Enum_t> Enums;
    Vector<Class> Classes;
    Vector<Function> Functions;
    Vector<Variable> Variables;
    Vector<Namespace> NameSpaces;
public:
   bool CreateTestCaseHeader(fstream& rFile, int indent, AppContext& rCtx);
   bool CreateTestCaseSource(fstream& rFile, int indent, AppContext& rCtx);

};
enum SYMBOL_TYPE_t
{
    ST_CLASS=0,
    ST_STRUCT,
    ST_ENUM
};

struct SymbolInfo
{
    String Name;
    SYMBOL_TYPE_t Type=ST_CLASS;
    int HeaderFile=-1;
};

typedef Vector<SymbolInfo> SymbolTable_t;

struct CodeGenParams
{
    String sPath;
    String sFile;
    String sHeaderFile;
    String sSourceFile;
    String sHeaderGuard;

    StringVector vActiveClassConstructorArgsDecls;
    StringVector vActiveClassConstructorArgNames;
    String sActiveClassVarName;
public:
    void Prepare(String& rsOutputDir, String& rFile);
};



class HeaderFile: public Namespace
{
public:
    HeaderFile(String& rFileName);
    virtual ~HeaderFile();
    void Dump();
    void CreateUnitTestFiles(AppContext& rCtx);

public:

    bool Parsed=false;
    StringVector IncludeFiles;

private:
    void AddCustomCommentHeader(fstream& rFile);
    bool CreateUnitTestHeaderFile(AppContext& rCtx);
    bool CreateUnitTestSourceFile(AppContext& rCtx);
    void PrintAppInfo(fstream& rFile);

};

enum BLOCK_TYPE
{
    BLOCK_NAMESPACE,
    BLOCK_CLASS,
    BLOCK_STRUCT,
    BLOCK_FUNCTION,
    BLOCK_ENUM
};

struct CodeBlock
{
    BLOCK_TYPE Type;
    String Name;
    unsigned BraceCount=0;
    bool BraceFound = false;
    void* pBlock = nullptr;
};

enum PARSING_STATE
{
    STATE_NONE=0,
    STATE_FIND_FN_CLOSING_BRACKET,
    STATE_FIND_OPENING_BRACE,
    STATE_FIND_CLOSING_BRACE,
    STATE_PROCESS_FN_DEFINITION,
    STATE_FIND_FN_DECLARATION_TYPE,
    STATE_FIND_ENUM_END,
    STATE_FIND_SEMI_COLUMN
};



struct RE
{
    static regex CPP_STYLE_COMMENT;
    static regex ONE_LINE_COMMENT;
    static String COMMENT_START;
    static String COMMENT_END;
    static Vector<regex> IgnoreList;
    static regex INCLUDE_FILE;
    static regex NAMESPACE;
    static regex VARIABLE;
    static regex FUNCTION_DECL;
    static regex FUNCTION_DECL_START;
    static regex PARSED_FN;
    static regex CLASS;
    static regex PRIVATE;
    static regex PROTECTED;
    static regex PUBLIC;
    static regex ENUM;
    static regex ENUM_MEMBER;

    static regex FN_ATTR_STATIC;
    static regex FN_ATTR_INLINE;
    static regex FN_ATTR_CONST;
    static regex FN_ATTR_MUTABLE;
    static regex FN_ATTR_VIRTUAL;
    static regex FN_ATTR_EXPLICIT;

};


struct AppContext
{
    HeaderFile* pFile;
    int HeaderFileIndex;
    unsigned LineNo;
    String FunctionName;
    Vector<CodeBlock> Blocks;
    int BlockIndex;
    bool CommentBlockStarted;
    bool ReProcessLine = false;
    PARSING_STATE State = STATE_NONE;
    std::smatch oMatch;
    String MultiLineExpr;
    unsigned BracketCount=0;
    Enum_t Enum;

    SymbolTable_t SymbolTable;
    CodeGenParams Params;
    StringVector vNamespaceNames; //Stores the Namespace hierarchy for printing using directive

    AppContext();
    void Init();
    void Reset();
    void* GetActiveNameSpace();
};


#endif // CPP_HEADER_FILE_H
