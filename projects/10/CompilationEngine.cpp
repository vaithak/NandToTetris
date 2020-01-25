#include <iostream>
#include "CompilationEngine.h"

using namespace Hack::Compiler;

static void error(const std::string msg){
	throw msg;
}

CompilationEngine::CompilationEngine(const std::string input_file, const std::string output_file) : tokenizer(input_file){
	this->inp_file = input_file;
	this->fout.open(output_file);
}

int CompilationEngine::parse(){
	try{
		this->compileClass();
		return 0;
	}
	catch(const std::string excp){
		// Ideally, Should also empty the output file but not doing it now
		std::cerr<<"Some error occured in file: "<<inp_file<<"\n";
		std::cerr<<"Caught "<<excp<<"\n";
		std::cerr<<tokenizer.tokenType()<<"\n";
		fout.close();
		return -1;
	}
}

bool CompilationEngine::checkUnaryOp(){
	if(!checkCurrentTokenType(SYMBOL))
		return false;
	
	char curr_ch = tokenizer.symbol();
	if(curr_ch == '-' || curr_ch == '~')
		return true;
	
	return false;
}

bool CompilationEngine::checkKeywordConstant(){
	if(!checkCurrentTokenType(KEYWORD))
		return false;
	
	std::string curr_str = tokenizer.keyWord();
	if(curr_str == "true" || curr_str == "false" || curr_str == "null" || curr_str == "this")
		return true;
	
	return false;
}

bool CompilationEngine::checkBinaryOp(){
	if(!checkCurrentTokenType(SYMBOL))
		return false;
	
	char curr_ch = tokenizer.symbol();
	if(curr_ch == '+' || curr_ch == '-' || curr_ch == '*' || curr_ch == '/')
		return true;
	
	if(curr_ch == '&' || curr_ch == '|')
		return true;
		
	if(curr_ch == '<' || curr_ch == '>' || curr_ch == '=')
		return true;
	
	return false;
}

bool CompilationEngine::checkCurrentTokenType(int expected){
	if(tokenizer.tokenType()!=expected)
		return false;
	
	return true;
}

void CompilationEngine::writeXML(std::string text){
	fout<<text;
}

void CompilationEngine::compileClass(){
	tokenizer.advance();
	if(!checkCurrentTokenType(KEYWORD) || tokenizer.keyWord() != "class")
		error("Syntax Error, expected class keyword in start of file");
	writeXML("<class>\n<keyword> class </keyword>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(IDENTIFIER))
		error("Syntax Error, Invalid class name");
	writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol() != '{')
		error("Syntax Error in class declaration, expected '{'");
	writeXML("<symbol> { </symbol>\n");	
	
	tokenizer.advance();
	while(checkCurrentTokenType(KEYWORD) && (tokenizer.keyWord() == "static" || tokenizer.keyWord() == "field")){
		compileClassVarDec();
		tokenizer.advance();
	}
	
	while(checkCurrentTokenType(KEYWORD) && (tokenizer.keyWord() == "constructor" || tokenizer.keyWord() == "function" || tokenizer.keyWord() == "method")){
		compileSubroutine();
		tokenizer.advance();
	}
	
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol() != '}')
		error("Syntax Error in class declaration, expected '}'");
	writeXML("<symbol> } </symbol>\n");
	
	writeXML("</class>");
	return;
}

// the first token is already advanced before calling this function
void CompilationEngine::compileClassVarDec(){
	writeXML("<classVarDec>\n");
	writeXML("<keyword> " + tokenizer.keyWord() + " </keyword>\n");
	compileBodyVarDec();
	writeXML("</classVarDec>\n");
}

// the first token is already advanced before calling this function
void CompilationEngine::compileVarDec(){
	writeXML("<varDec>\n");
	writeXML("<keyword> var </keyword>\n");
	compileBodyVarDec();
	writeXML("</varDec>\n");
}

void CompilationEngine::compileVarType(bool alreadyAdvanced=false){
	if(!alreadyAdvanced)
		tokenizer.advance();
		
	if(checkCurrentTokenType(KEYWORD)){
		std::string curr_token = tokenizer.keyWord();
		if(curr_token != "int" && curr_token!= "char" && curr_token != "boolean")
			error("Invalid variable type");
			
		writeXML("<keyword> " + curr_token + " </keyword>\n");
	}
	else if(checkCurrentTokenType(IDENTIFIER)){
		std::string curr_token = tokenizer.identifier();
		writeXML("<identifier> " + curr_token + " </identifier>\n");
	}
	else{
		error("Invalid variable type");
	}
}

void CompilationEngine::compileSubroutineType(){
	tokenizer.advance();
	if(checkCurrentTokenType(KEYWORD)){
		std::string curr_token = tokenizer.keyWord();
		if(curr_token != "void" && curr_token != "int" && curr_token != "char" && curr_token != "boolean")
			error("Invalid subroutine return type");
			
		writeXML("<keyword> " + curr_token + " </keyword>\n");
	}
	else if(checkCurrentTokenType(IDENTIFIER)){
		std::string curr_token = tokenizer.identifier();
		writeXML("<identifier> " + curr_token + " </identifier>\n");
	}
	else{
		error("Invalid subroutine return type");
	}
}

void CompilationEngine::compileBodyVarDec(){
	compileVarType();
	
	tokenizer.advance();
	if(!checkCurrentTokenType(IDENTIFIER))
		error("Invalid identifier");
	writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL))
		error("Expected a symbol(, or ;)");
		
	while(tokenizer.symbol() == ','){
		writeXML("<symbol> , </symbol>\n");
		
		tokenizer.advance();
		if(!checkCurrentTokenType(IDENTIFIER))
			error("Invalid identifier");
		writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
		
		tokenizer.advance();
		if(!checkCurrentTokenType(SYMBOL))
			error("Expected a symbol");
	}
	
	if(tokenizer.symbol() != ';')
		error("Expected ; at end of variable declaration");
	
	writeXML("<symbol> ; </symbol>\n");
}

// the first token is already advanced before calling this function
void CompilationEngine::compileSubroutine(){
	writeXML("<subroutineDec>\n");
	writeXML("<keyword> " + tokenizer.keyWord() + " </keyword>\n");
	
	compileSubroutineType();
	
	tokenizer.advance();
	if(!checkCurrentTokenType(IDENTIFIER))
		error("Invalid identifier for subroutine name");
	writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol() != '(')
		error("Syntax Error in class declaration, expected '('");
	writeXML("<symbol> ( </symbol>\n");
	
	compileParameterList();	
	// compileParameterList will advance till closing bracket
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol() != ')')
		error("Syntax Error in class declaration, expected ')'");
	writeXML("<symbol> ) </symbol>\n");
	
	compileSubroutineBody();
	
	writeXML("</subroutineDec>\n");
}

void CompilationEngine::compileParameterList(){
	writeXML("<parameterList>\n");
	tokenizer.advance();
	
	// 0 parameters
	if(checkCurrentTokenType(SYMBOL)){
		writeXML("</parameterList>\n");
		return;
	}
	
	bool first = true;
	do{
		compileVarType(first);
		
		tokenizer.advance();
		if(!checkCurrentTokenType(IDENTIFIER))
			error("Invalid identifier for parameter name");
		writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
		
		tokenizer.advance();
		if(!checkCurrentTokenType(SYMBOL))
			error("Syntax Error in parameter list, expected a symbol");
			
		if(tokenizer.symbol() == ',')
			writeXML("<symbol> , </symbol>\n");
		
		first = false;
	}
	while(tokenizer.symbol() != ')');
	
	writeXML("</parameterList>\n");
}

void CompilationEngine::compileSubroutineBody(){
	writeXML("<subroutineBody>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol() != '{')
		error("Syntax Error in function body, expected '{'");
	writeXML("<symbol> { </symbol>\n");	
	
	tokenizer.advance();
	if(!checkCurrentTokenType(KEYWORD))
		error("Expected variable declaration or statement");
	
	std::string curr_keyword = tokenizer.keyWord();
	while(curr_keyword=="var"){
		compileVarDec();
		
		tokenizer.advance();
		if(!checkCurrentTokenType(KEYWORD))
			error("Expected variable declaration or statement");
		curr_keyword = tokenizer.keyWord();
	}
	
	compileStatements();
	
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='}')
		error("Syntax Error in function body, expected '}' at the end");
	writeXML("<symbol> } </symbol>\n");	
	
	writeXML("</subroutineBody>\n");
}

// the first token is already advanced before calling this function
// will consume one extra token after it's end
void CompilationEngine::compileStatements(){
	writeXML("<statements>\n");
	
	while(checkCurrentTokenType(KEYWORD)){
		if(tokenizer.keyWord() == "let")
			compileLet();
		else if(tokenizer.keyWord() == "if")
			compileIf();
		else if(tokenizer.keyWord() == "while")
			compileWhile();
		else if(tokenizer.keyWord() == "do")
			compileDo();
		else if(tokenizer.keyWord() == "return")
			compileReturn();
		else
			error("Invalid statement");
	}
	
	writeXML("</statements>\n");
}

// the first token is already advanced before calling this function
// will consume one extra token after it's end
void CompilationEngine::compileLet(){
	writeXML("<letStatement>\n");
	writeXML("<keyword> let </keyword>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(IDENTIFIER))
		error("Invalid identifier for let statement");
	writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL))
		error("Expected a symbol");
	
	if(tokenizer.symbol() == '['){
		writeXML("<symbol> [ </symbol>\n");
		compileExpression();
		
		if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!=']')
			error("Expected ']'");
		writeXML("<symbol> ] </symbol>\n");
		
		tokenizer.advance();
	}
	
	if(tokenizer.symbol() != '=')
		error("Invalid let statement, expected '=' or '['");
	writeXML("<symbol> = </symbol>\n");
		
	compileExpression();
	
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!=';')
		error("Expected ';'");
	writeXML("<symbol> ; </symbol>\n");
		
	writeXML("</letStatement>\n");
	tokenizer.advance();
}

// the first token is already advanced before calling this function
// will consume one extra token after it's end
void CompilationEngine::compileIf(){
	writeXML("<ifStatement>\n");
	writeXML("<keyword> if </keyword>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='(')
		error("Expected '('");
	writeXML("<symbol> ( </symbol>\n");
	
	compileExpression();
	
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!=')')
		error("Expected ')'");
	writeXML("<symbol> ) </symbol>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='{')
		error("Expected '{'");
	writeXML("<symbol> { </symbol>\n");
	
	tokenizer.advance();
	compileStatements();
	
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='}')
		error("Expected '}'");
	writeXML("<symbol> } </symbol>\n");
	
	tokenizer.advance();
	if(checkCurrentTokenType(KEYWORD) && tokenizer.keyWord() == "else"){
		writeXML("<keyword> else </keyword>\n");
		
		tokenizer.advance();
		if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='{')
			error("Expected '{'");
		writeXML("<symbol> { </symbol>\n");
		
		tokenizer.advance();
		compileStatements();
		
		if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='}')
			error("Expected '}'");
		writeXML("<symbol> } </symbol>\n");
		
		tokenizer.advance();
	}
	
	writeXML("</ifStatement>\n");
}

// the first token is already advanced before calling this function
// will consume one extra token after it's end
void CompilationEngine::compileWhile(){
	writeXML("<whileStatement>\n");
	writeXML("<keyword> while </keyword>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='(')
		error("Expected '('");
	writeXML("<symbol> ( </symbol>\n");
	
	compileExpression();
	
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!=')')
		error("Expected ')'");
	writeXML("<symbol> ) </symbol>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='{')
		error("Expected '{'");
	writeXML("<symbol> { </symbol>\n");
	
	tokenizer.advance();
	compileStatements();
	
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='}')
		error("Expected '}'");
	writeXML("<symbol> } </symbol>\n");
	
	writeXML("</whileStatement>\n");
	tokenizer.advance();
}

// the first token is already advanced before calling this function
// will consume one extra token after it's end
void CompilationEngine::compileDo(){
	writeXML("<doStatement>\n");
	writeXML("<keyword> do </keyword>\n");
	
	compileSubroutineCall();
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!=';')
		error("Expected ';'");
	writeXML("<symbol> ; </symbol>\n");
	
	writeXML("</doStatement>\n");
	tokenizer.advance();
}

// the first token is already advanced before calling this function
// will consume one extra token after it's end
void CompilationEngine::compileReturn(){
	writeXML("<returnStatement>\n");
	writeXML("<keyword> return </keyword>\n");
	
	compileExpression();
	
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!=';')
		error("Expected ';'");
	writeXML("<symbol> ; </symbol>\n");
	
	writeXML("</returnStatement>\n");
	tokenizer.advance();
}

// will consume one extra token after it's end
void CompilationEngine::compileExpression(){
	tokenizer.advance();
	if(checkCurrentTokenType(SYMBOL) && !checkUnaryOp() && tokenizer.symbol()!='(')
		return;
	
	writeXML("<expression>\n");	
	
	compileTerm();
	while(checkBinaryOp()){
		if(tokenizer.symbol() == '<')
			writeXML("<symbol> &lt; </symbol>\n");
		else if(tokenizer.symbol() == '>')
			writeXML("<symbol> &gt; </symbol>\n");
		else if(tokenizer.symbol() == '&')
			writeXML("<symbol> &amp; </symbol>\n");
		else
			writeXML("<symbol> " + std::string(1, tokenizer.symbol()) + " </symbol>\n");
		tokenizer.advance();
		compileTerm();
	}
	
	writeXML("</expression>\n");	
}

// the first token is already advanced before calling this function
// will consume one extra token after it's end
void CompilationEngine::compileTerm(){
	writeXML("<term>\n");
	
	if(checkCurrentTokenType(INT_CONST)){
		writeXML("<integerConstant> " + std::to_string(tokenizer.intVal()) + " </integerConstant>\n");
		tokenizer.advance();
	}
	
	if(checkCurrentTokenType(STRING_CONST)){
		writeXML("<stringConstant> " + tokenizer.stringVal() + " </stringConstant>\n");
		tokenizer.advance();
	}
		
	if(checkKeywordConstant()){
		writeXML("<keyword> " + tokenizer.keyWord() + " </keyword>\n");
		tokenizer.advance();
	}
		
	if(checkUnaryOp()){
		writeXML("<symbol> " + std::string(1, tokenizer.symbol()) + " </symbol>\n");
		tokenizer.advance();
		compileTerm();
	}

	if(checkCurrentTokenType(SYMBOL) && tokenizer.symbol() == '('){
		writeXML("<symbol> ( </symbol>\n");
		compileExpression();
		writeXML("<symbol> ) </symbol>\n");
		tokenizer.advance();
	}
	
	if(checkCurrentTokenType(IDENTIFIER)){
		writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
		tokenizer.advance();
		
		if(checkCurrentTokenType(SYMBOL)){	
			if(tokenizer.symbol() == '['){
				writeXML("<symbol> [ </symbol>\n");
				compileExpression();
				writeXML("<symbol> ] </symbol>\n");
				
				tokenizer.advance();
			}
			else if(tokenizer.symbol() == '('){
				writeXML("<symbol> ( </symbol>\n");
				compileExpressionList();
				writeXML("<symbol> ) </symbol>\n");
				
				tokenizer.advance();
			}
			else if(tokenizer.symbol() == '.'){
				writeXML("<symbol> . </symbol>\n");
				
				tokenizer.advance();
				writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
				
				tokenizer.advance();
				writeXML("<symbol> ( </symbol>\n");
				compileExpressionList();
				writeXML("<symbol> ) </symbol>\n");
				
				tokenizer.advance();
			}
		}	
	}
	
	writeXML("</term>\n");
}

void CompilationEngine::compileSubroutineCall(){
	tokenizer.advance();
	
	if(!checkCurrentTokenType(IDENTIFIER))
		error("Invalid subroutine call");
	writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
	
	tokenizer.advance();
	if(!checkCurrentTokenType(SYMBOL))
		error("Invalid subroutine call");
	
	if(tokenizer.symbol() == '.'){
		writeXML("<symbol> . </symbol>\n");
		
		tokenizer.advance();
		if(!checkCurrentTokenType(IDENTIFIER))
			error("Invalid subroutine call");
		writeXML("<identifier> " + tokenizer.identifier() + " </identifier>\n");
		
		tokenizer.advance();
	}
	
	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!='(')
		error("Expected '('");
	writeXML("<symbol> ( </symbol>\n");

	compileExpressionList();

	if(!checkCurrentTokenType(SYMBOL) || tokenizer.symbol()!=')')
		error("Expected ')' at the end of subroutine call");
	writeXML("<symbol> ) </symbol>\n");
}

// will consume one extra token after it's end
void CompilationEngine::compileExpressionList(){
	writeXML("<expressionList>\n");
	
	do{
		compileExpression();
		if(tokenizer.symbol() == ',')
			writeXML("<symbol> , </symbol>\n");
	}
	while(checkCurrentTokenType(SYMBOL) && tokenizer.symbol() == ',');
	
	writeXML("</expressionList>\n");
}

void CompilationEngine::close(){
	fout.close();
}
