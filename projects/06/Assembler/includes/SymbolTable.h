#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

namespace Hack{
    namespace Assembler{
        class SymbolTable{
        private:
            std::unordered_map <std::string, unsigned int> symbol_table;

        public:
            // Add predefined symbols
            SymbolTable();

            void addEntry(const std::string &symbol, unsigned int address);
            bool contains(const std::string &symbol);

            // Ensure the symbol is present in symbol_table before calling this function
            unsigned int getAddress(const std::string &symbol);
        };
    }
}


#endif