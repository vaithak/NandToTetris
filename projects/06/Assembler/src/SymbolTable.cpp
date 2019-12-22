#include "SymbolTable.h"

using namespace Hack::Assembler;

SymbolTable::SymbolTable(){
    // Add R0 to R15
    for (unsigned int i=0; i<16; ++i){
        symbol_table["R" + std::to_string(i)] = i;
    }

    symbol_table["SP"]     = 0;
    symbol_table["LCL"]    = 1;
    symbol_table["ARG"]    = 2;
    symbol_table["THIS"]   = 3;
    symbol_table["THAT"]   = 4;
    symbol_table["SCREEN"] = 16384;
    symbol_table["KBD"]    = 24576;
}

void SymbolTable::addEntry(const std::string &symbol, unsigned int address){
    symbol_table[symbol] = address;
}

bool SymbolTable::contains(const std::string &symbol){
    if(symbol_table.find(symbol) == symbol_table.end())
        return false;
    
    return true;
}

unsigned int SymbolTable::getAddress(const std::string &symbol){
    return symbol_table[symbol];
}