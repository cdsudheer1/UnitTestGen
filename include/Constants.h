#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#define APP_NAME "UnitTestGen"
#define APP_VERSION "0.07"
#define APP_COPYRIGHT "UnitTestGen developed by Sudheer Divakaran, Gadgeon Smart Systems Pvt. Ltd., Kochi, Kerala, INDIA."

#define STR_CLASS  "class"
#define STR_STRUCT "struct"
#define STR_SCOPE "::"
#define STR_NEW "new"
#define STR_CONST "const"
#define STR_STATIC "static"
#define STR_VIRTUAL "virtual"
#define STR_INLINE "inline"
#define STR_PTR_MEMBER "->"
#define STR_DELETE "delete"
#define STR_NAME_CLASS_VARIABLE "pClass"
#define STR_NAME_RETURN_VALUE "retVal"
#define STR_FN_ARGUMENT_PREFIX "fnArg"
#define STR_CLASS_ARGUMENT_PREFIX "classArg"

#define STR_MSG_INITIALIZE_VARIABLE " //TODO: Initialize the variable"


#define CHR_BRACE_OPEN '{'
#define CHR_BRACE_CLOSE '}'
#define CHR_BRACKET_OPEN '('
#define CHR_BRACKET_CLOSE ')'
#define CHR_SEMI_COLUMN ';'
#define CHR_COLUMN ':'
#define CHR_COMMA ','
#define CHR_DOT '.'
#define CHR_UNDERSCORE '_'
#define CHR_TAB '\t'
#define CHR_TILD '~'
#define CHR_STAR '*'
#define CHR_REF '&'
#define CHR_SPACE ' '
#define CHR_EQUALTO '='

#if defined (_WIN32) || defined(_WIN64)
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif


#endif // CONSTANTS_H_INCLUDED
