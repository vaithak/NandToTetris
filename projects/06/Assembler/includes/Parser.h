#ifndef PARSER_H
#define PARSER_H

#include <string>

namespace Hack{
    namespace Assembler{
        struct ParserMessage{
            // Error checking
            int status;
            std::string error_msg;

            // Instruction meta-data
            unsigned int next_instruction_num;

            // A: A-instruction
            // C: C-instruction
            // B: Blank line or comment
            // L: (Xxx) declaration
            char instruction_type;

            // For A-type and L-type instructions
            std::string symbol;
            std::string symbol_type;
            int symbol_value;

            // For C-type instructions
            std::string dest;
            std::string comp;
            std::string jump;

            // Initializing values
            ParserMessage(){
                error_msg   = "";
                symbol_type = "";
                symbol = "";
                dest   = "";
                comp   = "";
                jump   = "";
            }
        };

        class Parser{
        private:
            ParserMessage error_message(const std::string &msg);
            bool validate_a_type_instruction(const std::string &instruction);
            bool validate_l_type_instruction(const std::string &instruction);
            bool validate_c_type_instruction(const std::string &instruction);

            // Returns dest, comp and jmp mnemonics
            // Note: Run this on a valid c instruction only
            void tokenize_c_instruction(const std::string &instruction, std::string tokens[3]);

        public:
            ParserMessage parse(const std::string &line, unsigned int instruction_num);
        };
    }
}


#endif