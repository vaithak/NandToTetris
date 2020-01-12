#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

namespace Hack{
    namespace VMTranslator{
        struct ParserMessage{
            // Error checking
            int status;
            std::string error_msg;

            unsigned int next_command_num;
            std::string command_type;

            std::string arg1;
            int arg2;

            // Initializing values
            ParserMessage(){
                error_msg = "";
                status    = 0;
                arg1      = "";
            }
        };

        class Parser{
        private:
            std::vector<std::string> allowedArithmetic;
            std::vector<std::string> allowedSegments;
            std::vector<std::string> allowedMemoryAccess;
            std::vector<std::string> allowedProgramFlow;
            std::vector<std::string> allowedFunctionCalls;
            ParserMessage error_message(const std::string &msg);
            ParserMessage blankOrCommentMessage(const int command_num);
            bool updateMessage(ParserMessage &message, const std::string &token, int token_num);

        public:
            Parser();
            ParserMessage parse(const std::string &line, unsigned int command_num);
        };
    }
}


#endif