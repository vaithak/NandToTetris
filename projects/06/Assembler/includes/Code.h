#ifndef CODE_H
#define CODE_H

#include <string>
#include <unordered_map>

namespace Hack{
    namespace Assembler{
        class Code{
        private:
            std::unordered_map <std::string, std::string> dest_map;
            std::unordered_map <std::string, std::string> comp_map;
            std::unordered_map <std::string, std::string> jmp_map;

        public:
            // Initialize the maps
            Code();

            // Converts passed decimal to 15 bit binary string address
            std::string decimalToAddress(int decimalValue);

            // returns binary form of passed mnemonics
            std::string dest(std::string mnemonic);
            std::string jump(std::string mnemonic);
            std::string comp(std::string mnemonic);
        };
    }
}


#endif