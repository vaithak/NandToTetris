%{
#include "TokenizerHelper.h"
#include <stdio.h>

int multi_line_comments();
%}

KEYWDS    class|constructor|function|method|field|static|var|int|char|boolean|void|true|false|null|this|let|do|if|else|while|return
SINGLE_LINE_COMMENTS  ("//"[^\n]*)

%%
[ \n\t\r]+						return SKIP;
{SINGLE_LINE_COMMENTS}			return SKIP;
"/**"							return multi_line_comments();
\"[^"]*\"						return STRING_CONST;
{KEYWDS}						return KEYWORD;
[][{}().,;+\-*/&|<>=~]			return SYMBOL;
([0-9]{1,4}|[0-3][0-9]{4})		return INT_CONST;
[a-zA-Z_][a-zA-Z0-9_]*  		return IDENTIFIER;
%%

int multi_line_comments(){
	char c, c1;
loop:
	while((c=input()) != '*' && c!=0);
	
	if((c1 = input()) != '/' && c != 0){
		unput(c1);
		goto loop;
	}
		
	return SKIP;
}

int yywrap(void){
	return 1;
}
