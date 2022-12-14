%{
//extern int yylex();
#include "solan.tab.h"


#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line; \
    yylloc.first_column = yylloc.last_column; \
    for(int i = 0; yytext[i] != '\0'; i++) { \
        if(yytext[i] == '\n') { \
            yylloc.last_line++; \
            yylloc.last_column = 0; \
        } \
        else { \
            yylloc.last_column++; \
        } \
    }
%}

%option yylineno
%option noyywrap

%%

[ \t\n]         ;

"="				{return SOLAN_EQL;}
"+"				{return SOLAN_ADD;}
"-"				{return SOLAN_SUB;}
"*"				{return SOLAN_MUL;}
"/"             {return SOLAN_DIV;}

":"				{return SOLAN_COLON;}
","             {return SOLAN_COMMA;}

"let"           {return SOLAN_LET;}
"null"          {return SOLAN_NULL;}
"type"          {return SOLAN_TYPE;}
"function"      {return SOLAN_FUNCTION;}
"return"        {return SOLAN_RETURN;}
"end"           {return SOLAN_END;}

[a-zA-Z][_a-zA-Z0-9]*		{
	yylval.text = _strdup(yytext);
	return SOLAN_WORD;}

[0-9]+						{
	yylval.num = strtod(_strdup(yytext), NULL);
	return SOLAN_NUMBER;}

. {return yytext[0];}
%%