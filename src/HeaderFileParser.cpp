/** ************************************************************************************
 *
 *          @file  HeaderFileParser.cpp
 *
 *    	   @brief
 *
 *       @version  1.0
 *          @date  27.05.2020 16:33:24
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

#include <HeaderFileParser.h>
#include <Utils.h>
#include <iostream>
#include <fstream>

using std::regex;
using std::fstream;
using std::regex_match;
using std::regex_replace;
using std::regex_search;
using std::cout;
using std::endl;



bool HeaderFileParser::Parse(AppContext& rCtx)
{
    HeaderFile& rFile = *rCtx.pFile;
    fstream File(rFile.Name, std::ios::in);

    cout << "Parsing File:" << rFile.Name << endl << endl;

    if (!File.is_open()) {
        Log("Couldn't open File:%s\n", rFile.Name.c_str());
        return false;
    }

    String line;

    String::size_type pos;
    String sTemp;
    std::size_t i;
    bool bProcessLine;

    while (std::getline(File, line)) {
        rCtx.LineNo++;
        bProcessLine = true;
        line = regex_replace(line, RE::ONE_LINE_COMMENT,"$1 $3");
        if (line.empty()) {
            continue;
        }
        while(true) {
            if (rCtx.CommentBlockStarted)
            {
                pos = line.find(RE::COMMENT_END);
                if (String::npos != pos) {
                    line = line.substr(pos+2);
                    rCtx.CommentBlockStarted=false;
                    if (line.empty()) {
                        bProcessLine = false;
                        break;
                    }
                } else {
                    bProcessLine = false;
                    break;
                }
            } else {
                pos = line.find(RE::COMMENT_START);
                if (String::npos != pos) {
                    rCtx.CommentBlockStarted = true;
                    if (0==pos) {
                        bProcessLine = false;
                    } else {
                        line = line.substr(0,pos);
                    }
                    break;
                } else {
                    break;
                }
            }
        }

        if (false == bProcessLine) {
            continue;
        }
        line = regex_replace(line, RE::CPP_STYLE_COMMENT, "$1");

        trim(line);
        if (line.empty()) {
            continue;
        }

        for (i=0; i < RE::IgnoreList.size(); i++) {
            if(regex_match(line,RE::IgnoreList[i])) {
                bProcessLine = false;
                break;
            }
        }

        if (false == bProcessLine) {
            continue;
        }

        if (regex_match(line, rCtx.oMatch, RE::INCLUDE_FILE)) {
            rFile.IncludeFiles.push_back(rCtx.oMatch[1]);
            continue;
        }

        do
        {
            if (line.empty()) {
                break;
            }

            rCtx.ReProcessLine = false;
            switch (rCtx.State) {
                case STATE_NONE:
                    if (regex_search(line, rCtx.oMatch, RE::NAMESPACE)) {
                        CodeBlock blk;
                        blk.Name = rCtx.oMatch[1];
                        blk.Type = BLOCK_NAMESPACE;
                        blk.BraceFound = rCtx.oMatch[2].length() != 0;
                        if (blk.BraceFound) {
                            blk.BraceCount = 1;
                        }
                        Namespace ns;
                        ns.Name = blk.Name;
                        if (Settings::Debug) {
                            cout << "NameSpace:" << ns.Name << endl;
                        }

                        Namespace* pNameSpace = static_cast<Namespace*>(rCtx.GetActiveNameSpace());
                        if (nullptr != pNameSpace) {
                            pNameSpace->NameSpaces.push_back(ns);
                            Namespace& rNs = pNameSpace->NameSpaces[pNameSpace->NameSpaces.size()-1];
                            blk.pBlock = &rNs;
                        } else {
                            rFile.NameSpaces.push_back(ns);
                            Namespace& rNs = rFile.NameSpaces[rFile.NameSpaces.size()-1];
                            blk.pBlock = &rNs;
                        }

                        rCtx.Blocks.push_back(blk);
                        rCtx.BlockIndex++;
                        line = rCtx.oMatch.suffix();
                        trim(line);
                        rCtx.ReProcessLine = true;
                        break;
                    } else if (regex_search(line, rCtx.oMatch, RE::CLASS)) {
                        if (Settings::Debug) {
                            cout << "Class:" << line << endl;
                        }

                        if (CHR_SEMI_COLUMN != rCtx.oMatch[3]) {
                            CodeBlock blk;
                            Class c;
                            SymbolInfo s;
                            blk.Name = rCtx.oMatch[2];
                            c.Name = blk.Name;
                            s.Name = blk.Name;
                            s.HeaderFile = rCtx.HeaderFileIndex;

                            if (STR_STRUCT == rCtx.oMatch[1]) {
                                blk.Type = BLOCK_STRUCT;
                                c.Type = CLASS_TYPE_STRUCT;
                                c.AccessLevel = ACCESS_PUBLIC;
                                s.Type = ST_STRUCT;
                            } else {
                                blk.Type = BLOCK_CLASS;
                                c.Type = CLASS_TYPE_CLASS;
                                c.AccessLevel = ACCESS_PRIVATE;
                                s.Type = ST_CLASS;
                            }
                            rCtx.SymbolTable.push_back(s);

                            if (CHR_BRACE_OPEN == rCtx.oMatch[3]) {
                                blk.BraceFound = true;
                            } else if (String::npos != rCtx.oMatch[4].str().find(CHR_BRACE_OPEN)) {
                                blk.BraceFound = true;
                            }

                            if (blk.BraceFound) {
                                blk.BraceCount++;
                            } else {
                                rCtx.State = STATE_FIND_OPENING_BRACE;
                            }

                            Namespace* pNameSpace = static_cast<Namespace*>(rCtx.GetActiveNameSpace());
                            if (nullptr != pNameSpace) {
                                pNameSpace->Classes.push_back(c);
                                pNameSpace->ClassIndexes.push_back(pNameSpace->Classes.size()-1);
                            } else {
                                rFile.Classes.push_back(c);
                                rFile.ClassIndexes.push_back(rFile.Classes.size()-1);
                            }
                            rCtx.Blocks.push_back(blk);
                            rCtx.BlockIndex++;
                            line = rCtx.oMatch.suffix();
                            trim(line);
                            rCtx.ReProcessLine = true;
                        }
                        break;
                    } else if (regex_search(line, rCtx.oMatch, RE::ENUM)) {
                        if (Settings::Debug) {
                            cout << "Enum:" << line << endl;
                        }

                        String sName = rCtx.oMatch[2];
                        String sLastMatch = rCtx.oMatch[3];

                        pos = sName.find(CHR_COLUMN);
                        if (String::npos != pos) {
                            sName.erase(pos);
                        }
                        trim(sName);

                        if (sLastMatch.empty() || (String(1, CHR_BRACE_OPEN) == sLastMatch)) {
                            rCtx.Enum.Clear();
                            rCtx.Enum.Name = sName;
                            rCtx.State = STATE_FIND_ENUM_END;
                            CodeBlock b;
                            b.Name = sName;
                            if (String(1, CHR_BRACE_OPEN)==sLastMatch) {
                                b.BraceFound = true;
                                b.BraceCount = 1;
                            }
                            rCtx.Blocks.push_back(b);
                            rCtx.BlockIndex++;
                        }

                        line = rCtx.oMatch.suffix();
                        rCtx.ReProcessLine = true;
                        break;
                    } else if (regex_match(line, rCtx.oMatch, RE::FUNCTION_DECL)) {
                        if (Settings::Debug) {
                            cout << "Function:" << line << endl;
                        }
                        sTemp = rCtx.oMatch[1].str();
                        ParseFunction(rCtx, sTemp);
                        break;
                    } else if (regex_match(line, rCtx.oMatch, RE::VARIABLE)) {
                        if (Settings::Debug) {
                            cout << "Variable:" << line << endl;
                        }
                        break;
                    } else if (regex_search(line, rCtx.oMatch, RE::FUNCTION_DECL_START)) {
                        if (Settings::Debug) {
                            cout << "Function Start:" << line << endl;
                        }
                        rCtx.MultiLineExpr = rCtx.oMatch.prefix().str() + rCtx.oMatch[0].str();
                        sTemp = rCtx.oMatch.suffix();
                        rCtx.BracketCount = 1;
                        int j=0;
                        for (String::size_type i=0; i < sTemp.length(); i++, j++) {
                            char ch = sTemp[i];
                            rCtx.MultiLineExpr.push_back(ch);
                            if (CHR_BRACKET_OPEN == ch) {
                                rCtx.BracketCount++;
                            } else if (CHR_BRACKET_CLOSE == ch) {
                                rCtx.BracketCount--;
                                if (0 == rCtx.BracketCount) {
                                    sTemp.erase(0, i+1);
                                    line = sTemp;
                                    rCtx.State = STATE_FIND_FN_DECLARATION_TYPE;
                                    rCtx.ReProcessLine = true;
                                    break;
                                }
                            }
                        }
                        if (rCtx.BracketCount > 0) {
                            sTemp.erase(0, j);
                            line = sTemp;
                            rCtx.State = STATE_FIND_FN_CLOSING_BRACKET;
                            rCtx.ReProcessLine = true;
                        }
                    } else if (regex_match(line, rCtx.oMatch, RE::PRIVATE)) {
                        Namespace* pNameSpace = static_cast<Namespace*>(rCtx.GetActiveNameSpace());
                        int iClassIndex;
                        if (nullptr != pNameSpace) {
                            if (!pNameSpace->ClassIndexes.empty()) {
                                iClassIndex = pNameSpace->ClassIndexes[pNameSpace->ClassIndexes.size()-1];
                                pNameSpace->Classes[iClassIndex].AccessLevel = ACCESS_PRIVATE;
                            }
                        } else if (!rFile.ClassIndexes.empty()) {
                            iClassIndex = rFile.ClassIndexes[rFile.ClassIndexes.size()-1];
                            rFile.Classes[iClassIndex].AccessLevel = ACCESS_PRIVATE;
                        }
                    } else if (regex_match(line, rCtx.oMatch, RE::PROTECTED)) {
                        Namespace* pNameSpace = static_cast<Namespace*>(rCtx.GetActiveNameSpace());
                        int iClassIndex;
                        if (nullptr != pNameSpace) {
                            if (!pNameSpace->ClassIndexes.empty()) {
                                iClassIndex = pNameSpace->ClassIndexes[pNameSpace->ClassIndexes.size()-1];
                                pNameSpace->Classes[iClassIndex].AccessLevel = ACCESS_PROTECTED;
                            }
                        } else if (!rFile.ClassIndexes.empty()) {
                            iClassIndex = rFile.ClassIndexes[rFile.ClassIndexes.size()-1];
                            rFile.Classes[iClassIndex].AccessLevel = ACCESS_PROTECTED;
                        }
                    } else if (regex_match(line, rCtx.oMatch, RE::PUBLIC)) {
                        Namespace* pNameSpace = static_cast<Namespace*>(rCtx.GetActiveNameSpace());
                        int iClassIndex;
                        if (nullptr != pNameSpace) {
                            if (!pNameSpace->ClassIndexes.empty()) {
                                iClassIndex = pNameSpace->ClassIndexes[pNameSpace->ClassIndexes.size()-1];
                                pNameSpace->Classes[iClassIndex].AccessLevel = ACCESS_PUBLIC;
                            }
                        } else if (!rFile.ClassIndexes.empty()) {
                            iClassIndex = rFile.ClassIndexes[rFile.ClassIndexes.size()-1];
                            rFile.Classes[iClassIndex].AccessLevel = ACCESS_PUBLIC;
                        }
                    } else {
                        bool bLinePrinted = false;
                        for (i=0; i < line.length(); i++)
                        {
                            char ch = line[i];
                            if (CHR_BRACE_OPEN == ch) {
                                if (-1 != rCtx.BlockIndex) {
                                    rCtx.Blocks[rCtx.BlockIndex].BraceCount++;
                                }
                            } else if (CHR_BRACE_CLOSE == ch) {
                                if (-1 != rCtx.BlockIndex) {
                                    CodeBlock& rBlk = rCtx.Blocks[rCtx.BlockIndex];
                                    BLOCK_TYPE BlkType = rBlk.Type;
                                    if (rBlk.BraceCount > 0) {
                                        rBlk.BraceCount--;
                                        if (0==rBlk.BraceCount) {
                                            if (Settings::Debug)
                                            {
                                                cout << "Block:" << rBlk.Name << ", Type:"
                                                            << rBlk.Type << " Closed" << endl;
                                            }

                                            if ((BLOCK_FUNCTION != BlkType) && (BLOCK_NAMESPACE != BlkType)) {
                                                Namespace* pNameSpace = static_cast<Namespace*>(rCtx.GetActiveNameSpace());
                                                if (nullptr != pNameSpace) {
                                                    if (!rFile.ClassIndexes.empty()) {
                                                        pNameSpace->ClassIndexes.pop_back();
                                                    }
                                                } else if (!rFile.ClassIndexes.empty()) {
                                                    rFile.ClassIndexes.pop_back();
                                                }
                                            }

                                            rCtx.Blocks.pop_back();
                                            rCtx.BlockIndex--;
                                            if ((BLOCK_CLASS==BlkType) || (BLOCK_STRUCT==BlkType) || (BLOCK_ENUM==BlkType)) {
                                                line.erase(0, i+1);
                                                rCtx.State = STATE_FIND_SEMI_COLUMN;
                                                rCtx.ReProcessLine = true;
                                                break;
                                            }
                                        }
                                    }
                                }
                            } else if (!bLinePrinted) {
                                if (!std::isspace(ch)) {
                                    bLinePrinted = true;
                                    cout << "DH:" << line << endl;
                                }
                            }
                        }
                    }
                    break;
                case STATE_FIND_FN_CLOSING_BRACKET:
                    for (i=0; i < line.length(); i++)
                    {
                        char ch = line[i];
                        rCtx.MultiLineExpr.push_back(ch);
                        if (CHR_BRACKET_OPEN == ch)
                        {
                            rCtx.BracketCount++;
                        }
                        else if (CHR_BRACKET_CLOSE == ch)
                        {
                            rCtx.BracketCount--;
                            if (0 == rCtx.BracketCount) {
                                line.erase(0, i+1);
                                rCtx.ReProcessLine = true;
                                rCtx.State = STATE_FIND_FN_DECLARATION_TYPE;
                                break;
                            }
                        }
                    }
                    break;
               case STATE_FIND_FN_DECLARATION_TYPE:
                    for (i=0; i < line.length(); i++) {
                        char ch = line[i];
                        if ((CHR_BRACE_OPEN == ch) || (CHR_SEMI_COLUMN==ch)) {
                            ParseFunction(rCtx, rCtx.MultiLineExpr);
                            if (CHR_BRACE_OPEN==ch) {
                                CodeBlock blk;
                                blk.Type = BLOCK_FUNCTION;
                                blk.BraceCount = 1;
                                rCtx.Blocks.push_back(blk);
                                rCtx.BlockIndex++;
                                rCtx.State = STATE_FIND_CLOSING_BRACE;
                            } else {
                                rCtx.State = STATE_NONE;
                            }
                            if (Settings::Debug) {
                                cout << rCtx.MultiLineExpr << endl;
                            }

                            line.erase(0, i+1);
                            rCtx.ReProcessLine = true;
                            break;
                        } else {
                            rCtx.MultiLineExpr.push_back(ch);
                        }
                    }
                    break;
                case STATE_FIND_OPENING_BRACE:
                    pos = line.find(CHR_BRACE_OPEN);
                    if (String::npos != pos) {
                        rCtx.State = STATE_NONE;
                        line.erase(0, pos+1);
                        if (-1 != rCtx.BlockIndex) {
                            rCtx.Blocks[rCtx.BlockIndex].BraceCount++;
                            rCtx.Blocks[rCtx.BlockIndex].BraceFound = true;
                        }
                        rCtx.ReProcessLine = true;
                    }
                    break;
                case STATE_FIND_CLOSING_BRACE:
                    for (i=0; i < line.length(); i++)
                    {
                        char ch = line[i];
                        if (CHR_BRACE_OPEN == ch)
                        {
                            if (-1 != rCtx.BlockIndex) {
                                rCtx.Blocks[rCtx.BlockIndex].BraceCount++;
                            }
                        }
                        else if (CHR_BRACE_CLOSE == ch)
                        {
                            if (-1 != rCtx.BlockIndex) {
                                rCtx.Blocks[rCtx.BlockIndex].BraceCount--;
                                if (0 == rCtx.Blocks[rCtx.BlockIndex].BraceCount) {
                                    if (Settings::Debug) {
                                        cout << "Function Ended" << endl;
                                    }
                                    rCtx.Blocks.pop_back();
                                    rCtx.BlockIndex--;
                                    rCtx.State = STATE_NONE;
                                }
                            }
                        }
                    }
                    break;
                case STATE_FIND_ENUM_END:
                    if (rCtx.Blocks[rCtx.BlockIndex].BraceFound) {
                        if (regex_search(line, rCtx.oMatch, RE::ENUM_MEMBER)) {
                            sTemp = rCtx.oMatch[1];
                            if (!sTemp.empty()) {
                                rCtx.Enum.Values.push_back(sTemp);
                            }
                            sTemp = rCtx.oMatch[4];
                            if (!sTemp.empty()) {
                                if (String(1,CHR_BRACE_CLOSE) == sTemp) {
                                    rCtx.Blocks[rCtx.BlockIndex].BraceCount--;
                                    rCtx.Blocks.pop_back();
                                    rCtx.BlockIndex--;
                                    rCtx.State = STATE_FIND_SEMI_COLUMN;
                                    SymbolInfo s;
                                    s.HeaderFile = rCtx.HeaderFileIndex;
                                    s.Name = rCtx.Enum.Name;
                                    s.Type = ST_ENUM;
                                    rCtx.SymbolTable.push_back(s);
                                    Namespace* pNameSpace = static_cast<Namespace*>(rCtx.GetActiveNameSpace());
                                    if (nullptr != pNameSpace) {
                                        pNameSpace->Enums.push_back(rCtx.Enum);
                                    } else {
                                        rFile.Enums.push_back(rCtx.Enum);
                                    }
                                }
                            }
                            line = rCtx.oMatch.suffix();
                            rCtx.ReProcessLine = true;
                        } else {
                            for (i=0; i < line.length(); i++) {
                                if (CHR_BRACE_CLOSE == line[i]) {
                                    rCtx.Blocks[rCtx.BlockIndex].BraceCount--;
                                    rCtx.Blocks.pop_back();
                                    rCtx.BlockIndex--;
                                    rCtx.State = STATE_FIND_SEMI_COLUMN;
                                    line.erase(0, i+1);
                                    rCtx.ReProcessLine = true;

                                    Namespace* pNameSpace = static_cast<Namespace*>(rCtx.GetActiveNameSpace());
                                    if (nullptr != pNameSpace) {
                                        pNameSpace->Enums.push_back(rCtx.Enum);
                                    } else {
                                        rFile.Enums.push_back(rCtx.Enum);
                                    }
                                    break;
                                }
                            }
                        }
                    } else {
                        for (i=0; i < line.length(); i++) {
                            char ch = line[i];
                            if (CHR_BRACE_OPEN == ch) {
                                rCtx.Blocks[rCtx.BlockIndex].BraceFound = true;
                                rCtx.Blocks[rCtx.BlockIndex].BraceCount++;
                                line.erase(0, i+1);
                                rCtx.ReProcessLine = true;
                                break;
                            }
                        }
                    }
                    break;
                case STATE_FIND_SEMI_COLUMN:
                    for (i=0; i < line.length(); i++) {
                        if (CHR_SEMI_COLUMN == line[i]) {
                            rCtx.State = STATE_NONE;
                            line.erase(0, i+1);
                            rCtx.ReProcessLine = true;
                            break;
                        }
                    }
                    break;
                default:
                    break;

            }
        } while(rCtx.ReProcessLine);
    }

    if (!rCtx.Blocks.empty()) {
        cout << "Error:" << rCtx.Blocks.size() << " Blocks are to be closed" << endl;
        return false;
    }

    return true;
}

void HeaderFileParser::ParseFunction(AppContext& rCtx, String& rLine)
{
    if (Settings::Debug) {
        cout <<"FN::"<< rLine << endl;
    }

    unsigned nFlags = 0;

    if (regex_search(rLine, rCtx.oMatch, RE::FN_ATTR_STATIC)) {
        rLine = rCtx.oMatch.prefix().str() + " " + rCtx.oMatch.suffix().str();
        nFlags |= FN_STATIC;
    }

    if (regex_search(rLine, rCtx.oMatch, RE::FN_ATTR_INLINE)) {
        rLine = rCtx.oMatch.prefix().str() + " " + rCtx.oMatch.suffix().str();
        nFlags |= FN_INLINE;
    }

    if (regex_search(rLine, rCtx.oMatch, RE::FN_ATTR_CONST)) {
        rLine = rCtx.oMatch.prefix().str() + ")" + rCtx.oMatch.suffix().str();
        nFlags |= FN_CONST;
    }

    if (regex_search(rLine, rCtx.oMatch, RE::FN_ATTR_MUTABLE)) {
        rLine = rCtx.oMatch.prefix().str() + ")" + rCtx.oMatch.suffix().str();
        nFlags |= FN_MUTABLE;
    }

    if (regex_search(rLine, rCtx.oMatch, RE::FN_ATTR_VIRTUAL)) {
        rLine = rCtx.oMatch.prefix().str() + " " + rCtx.oMatch.suffix().str();
        nFlags |= FN_VIRTUAL;
    }

    if (regex_search(rLine, rCtx.oMatch, RE::FN_ATTR_EXPLICIT)) {
        rLine = rCtx.oMatch.prefix().str() + " " + rCtx.oMatch.suffix().str();
    }

    trim(rLine);

   if (regex_match(rLine, rCtx.oMatch, RE::PARSED_FN)) {
        String sRetval = rCtx.oMatch[1];
        String sName = rCtx.oMatch[2];
        String sArgs = rCtx.oMatch[3];
        trim(sRetval);
        trim(sName);
        trim(sArgs);

        if (String::npos != sName.find(STR_SCOPE)) {
            if (Settings::Debug)
            {
                cout << "Ignoring Class Function Definition:" << rLine << endl;
                return;
            }
        }

        if (Settings::Debug)
        {
            cout <<"NEWFN::"<< rLine <<" ATTRS:" << nFlags << endl;
            cout <<"\tRET_TYPE:" << sRetval << endl;
            cout <<"\tNAME:" << sName << endl;
            cout <<"\tARGS:" << sArgs << endl;
        }


        if (0==sRetval.length()) {
            //Constructor OR Destructor
            if (String::npos != sName.find("~")) {
                nFlags |= FN_DESTRUCTOR;
            } else {
                nFlags |= FN_CONSTRUCTOR;
            }
        }

        Function f;
        f.Flags = nFlags;
        f.Name = sName;
        if (ParseFunctionArgs(rCtx, f, sArgs)) {
            if (!(nFlags & (FN_CONSTRUCTOR|FN_DESTRUCTOR))) {
                Variable v;
                if (ParseVariable(rCtx, v, sRetval, true)) {
                    f.RetVal = v;
                } else {
                    cout <<"FN_RETVAL_PARSE_ERROR:" << rLine << endl;
                    return;
                }
            }
            HeaderFile& rFile = *rCtx.pFile;
            //Parsing was successful
            Namespace* pNameSpace = static_cast<Namespace*>(rCtx.GetActiveNameSpace());
            if (nullptr != pNameSpace) {
                int iClassIndex;
                if (!pNameSpace->ClassIndexes.empty()) {
                    iClassIndex = pNameSpace->ClassIndexes[pNameSpace->ClassIndexes.size()-1];
                    f.AccessLevel = pNameSpace->Classes[iClassIndex].AccessLevel;
                    pNameSpace->Classes[iClassIndex].Functions.push_back(f);
                } else {
                    pNameSpace->Functions.push_back(f);
                }
            } else {
                int iClassIndex;
                if (!rFile.ClassIndexes.empty()) {
                    iClassIndex = rFile.ClassIndexes[rFile.ClassIndexes.size()-1];
                    f.AccessLevel = rFile.Classes[iClassIndex].AccessLevel;
                    rFile.Classes[iClassIndex].Functions.push_back(f);
                } else {
                    rFile.Functions.push_back(f);
                }
            }

        } else {
            cout <<"FN_ARGS_PARSE_ERROR:" << rLine << endl;
        }

   } else {
        cout <<"FN_PARSE_ERROR:" << rLine << endl;
   }
}


bool HeaderFileParser::ParseVariable(AppContext& rCtx, Variable &rVariable, String& rExpr, bool bRetVal)
{
    bool bResult = false;
    String::size_type pos;
    unsigned nFlags = 0;
    std::size_t i;

    String CONST("const ");
    String PTR("*");
    String REF("&");
    String EQUAL_SIGN("=");

    pos = rExpr.find(CONST);
    if (String::npos != pos) {
        nFlags |= V_FLAG_CONST;
        rExpr.erase(pos, CONST.length());
    }

    pos = rExpr.find(PTR);
    if (String::npos != pos) {
        nFlags |= V_FLAG_PTR;
        rExpr.erase(pos, PTR.length());
    }

    pos = rExpr.find(REF);
    if (String::npos != pos) {
        nFlags |= V_FLAG_REF;
        rExpr.erase(pos, REF.length());
    }

    pos = rExpr.find(EQUAL_SIGN);
    if (String::npos != pos) {
        rExpr.erase(pos);
        nFlags |= V_FLAG_HAS_DEFAULT_VALUE;
    }

    rVariable.Flags = nFlags;

    trim(rExpr);

    bool bVTypeIdentified = false;
    for (i=0; i < Variable::VTList.size(); i++) {
        if (regex_search(rExpr, rCtx.oMatch, Variable::VTList[i].first)) {
            bVTypeIdentified = true;
            rVariable.Type = Variable::VTList[i].second;
            String sTemp = rCtx.oMatch[0].str();
            pos = rExpr.find(sTemp);
            rVariable.Name = Variable::VTNames[rVariable.Type];
            if (String::npos != pos) {
                rExpr.erase(pos, sTemp.length());
            }
            break;
        }
    }

    bResult = bVTypeIdentified;
    if (!bVTypeIdentified) {
        trim(rExpr);
        StringVector vTokens = TokenizeString(rExpr);
        if (vTokens.size() > 0) {
            rVariable.Name = vTokens[0];
            bResult = true;

        }
    }

    return bResult;
}

bool HeaderFileParser::ParseFunctionArgs(AppContext& rCtx, Function& rFn, String& rArgs)
{
    if (rArgs.empty()) {
        return true;
    }

    std::size_t i;

    regex SIGNED("\\s+signed\\s+");
    while (true) {
        if (regex_search(rArgs, rCtx.oMatch, SIGNED)) {
            rArgs = rCtx.oMatch.prefix().str() + " " + rCtx.oMatch.suffix().str();
        } else {
            break;
        }
    }

    StringVector vArgs = SplitArguments(rArgs);
    if (vArgs.size()) {
        for (i=0; i < vArgs.size(); i++) {
            Variable v;
            if (ParseVariable(rCtx, v, vArgs[i], false)) {
                rFn.Arguments.push_back(v);
            } else {
                return false;
            }
        }
    }
    return true;
}
