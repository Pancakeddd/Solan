%{
//extern int yylex();
#include "solan.tab.h"
%}

%option noyywrap

%%

[ \t\n]         ;

"="				{return SOLAN_EQL;}
"+"				{return SOLAN_ADD;}
"-"				{return SOLAN_SUB;}
"*"				{return SOLAN_MUL;}
"/"				{return SOLAN_DIV;}

"let"			{return SOLAN_LET;}
":"				{return SOLAN_COLON;}

[a-zA-Z][_a-zA-Z0-9]*		{
	yylval.text = _strdup(yytext);
	return SOLAN_WORD;}

[0-9]+						{
	yylval.num = strtod(_strdup(yytext), NULL);
	return SOLAN_NUMBER;}

. {return yytext[0];}
%%