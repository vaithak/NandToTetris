#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <fstream>
#include "Tokenizer.h"

namespace Hack{
	namespace Compiler{
		class CompilationEngine{
		private:
			JackTokenizer tokenizer; 
			std::ofstream fout;
			std::string inp_file;
			void writeXML(std::string text);
			bool checkCurrentTokenType(int expected);
			void compileBodyVarDec();
			void compileVarType(bool alreadyAdvanced);
			void compileSubroutineType();
			void compileSubroutineBody();
			void compileSubroutineCall();
			bool checkBinaryOp();
			bool checkUnaryOp();
			bool checkKeywordConstant();
		
		public:
			CompilationEngine(const std::string input_file, const std::string output_file);
			int parse();
			void compileClass();
			void compileClassVarDec();
			void compileSubroutine();
			void compileParameterList();
			void compileVarDec();
			void compileStatements();
			void compileDo();
			void compileLet();
			void compileWhile();
			void compileReturn();
			void compileIf();
			void compileExpression();
			void compileTerm();
			void compileExpressionList();
			void close();
		};
	}
}

#endif
