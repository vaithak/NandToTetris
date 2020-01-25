#include "Tokenizer.h"
#include <cstdio>
#include <cassert>
#include <cstring>
#include <string>
#include <algorithm>
#include <iostream>

#define stringify( name ) # name

using namespace Hack::Compiler;

extern "C" char* yytext;
extern "C" FILE* yyin;
extern "C" int yylex(void);

JackTokenizer::JackTokenizer(const std::string filename){
	char inp_file[filename.length() + 1];
	strcpy(inp_file, filename.c_str());
	yyin = fopen(inp_file, "r");
	populateKeywordMap();
}

void JackTokenizer::populateKeywordMap(){
	keyword_map[stringify( CLASS )] 		= CLASS;
	keyword_map[stringify( CONSTRUCTOR )] 	= CONSTRUCTOR;
	keyword_map[stringify( FUNCTION )] 		= FUNCTION;
	keyword_map[stringify( METHOD )] 		= METHOD;
	keyword_map[stringify( FIELD )] 		= FIELD;
	keyword_map[stringify( STATIC )] 		= STATIC;
	keyword_map[stringify( VAR )] 			= VAR;
	keyword_map[stringify( INT )]			= INT;
	keyword_map[stringify( CHAR )] 			= CHAR;
	keyword_map[stringify( BOOLEAN )]		= BOOLEAN;
	keyword_map[stringify( VOID )]			= VOID;
	keyword_map[stringify( TRUE )]			= TRUE;
	keyword_map[stringify( FALSE )] 		= FALSE;
	keyword_map[stringify( THIS )]			= THIS;
	keyword_map[stringify( LET )]			= LET;
	keyword_map[stringify( DO )]			= DO;
	keyword_map[stringify( IF )]			= IF;
	keyword_map[stringify( ELSE )]			= ELSE;
	keyword_map[stringify( WHILE )]			= LET;
	keyword_map[stringify( RETURN )]		= LET;
	keyword_map["NULL"]						= NLL;
}

bool JackTokenizer::advance(){
	current_token_type = yylex();
	while(current_token_type == SKIP){
		current_token_type = yylex();
	}
		
	if(current_token_type == 0)
		return false;
	return true;
}

int JackTokenizer::tokenType(){
	return current_token_type;
}

std::string JackTokenizer::keyWord(){
	assert(current_token_type == KEYWORD);
	std::string word(yytext);
	//transform(word.begin(), word.end(), word.begin(), ::toupper);
	return word;
}

char JackTokenizer::symbol(){
	assert(current_token_type == SYMBOL);
	return yytext[0];
}

std::string JackTokenizer::identifier(){
	assert(current_token_type == IDENTIFIER);
	return std::string(yytext);
}

int JackTokenizer::intVal(){
	assert(current_token_type == INT_CONST);
	return stoi(std::string(yytext));
}

std::string JackTokenizer::stringVal(){
	assert(current_token_type == STRING_CONST);
	std::string curr_const = std::string(yytext);
	return curr_const.substr(1, curr_const.length()-2);
}

void close(){
	fclose(yyin);
}
