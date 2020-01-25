#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <unordered_map>
#include "TokenizerHelper.h"

namespace Hack{
	namespace Compiler{
		class JackTokenizer{
		private:
			int current_token_type;
			std::unordered_map<std::string, Keywords> keyword_map;
			void populateKeywordMap();
		
		public:
			JackTokenizer(const std::string filename);
			bool advance();
			int tokenType();
			std::string keyWord();
			char symbol();
			std::string identifier();
			int intVal();
			std::string stringVal();
			void close();
		};
	}
}

#endif
