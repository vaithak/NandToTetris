#include "Code.h"
#include <bitset>

using namespace Hack::Assembler;

Code::Code(){
    // Filling dest_map
    dest_map[""]    = "000";
    dest_map["M"]   = "001";
    dest_map["D"]   = "010";
    dest_map["MD"]  = "011";
    dest_map["A"]   = "100";
    dest_map["AM"]  = "101";
    dest_map["AD"]  = "110";
    dest_map["AMD"] = "111";

    // Filling jump_map
    jmp_map[""]    = "000";
    jmp_map["JGT"] = "001";
    jmp_map["JEQ"] = "010";
    jmp_map["JGE"] = "011";
    jmp_map["JLT"] = "100";
    jmp_map["JNE"] = "101";
    jmp_map["JLE"] = "110";
    jmp_map["JMP"] = "111";

    // Filling comp_map
    comp_map["0"]   = "0101010";
    comp_map["1"]   = "0111111";
    comp_map["-1"]  = "0111010";
    comp_map["D"]   = "0001100";
    comp_map["A"]   = "0110000";
    comp_map["!D"]  = "0001101";
    comp_map["!A"]  = "0110001";
    comp_map["~D"]  = "0001111";
    comp_map["~A"]  = "0110011";
    comp_map["D+1"] = "0011111";
    comp_map["A+1"] = "0110111";
    comp_map["D-1"] = "0001110";
    comp_map["A-1"] = "0110010";
    comp_map["D+A"] = "0000010";
    comp_map["D-A"] = "0010011";
    comp_map["A-D"] = "0000111";
    comp_map["D&A"] = "0000000";
    comp_map["D|A"] = "0010101";

    comp_map["M"]   = "1110000";
    comp_map["!M"]  = "1110001";
    comp_map["~M"]  = "1110011";
    comp_map["M+1"] = "1110111";
    comp_map["M-1"] = "1110010";
    comp_map["D+M"] = "1000010";
    comp_map["D-M"] = "1010011";
    comp_map["M-D"] = "1000111";
    comp_map["D&M"] = "1000000";
    comp_map["D|M"] = "1010101";
}

std::string Code::dest(std::string mnemonic){
    return dest_map[mnemonic];
}

std::string Code::jump(std::string mnemonic){
    return jmp_map[mnemonic];
}

std::string Code::comp(std::string mnemonic){
    return comp_map[mnemonic];
}

std::string Code::decimalToAddress(int decimalValue){
    return std::bitset<15>(decimalValue).to_string();
}