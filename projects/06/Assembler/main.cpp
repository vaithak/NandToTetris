#include <iostream>
#include <fstream>
#include <string>
#include "Parser.h"
#include "Code.h"
#include "SymbolTable.h"

using namespace Hack::Assembler;

// check command line arguments
bool verify_arguments(int argc, char const *argv[], std::string &base_filename){
    if (argc != 2)
        return false;

    std::string filename(argv[1]);
    if(filename.length() <= 3)
        return false;

    base_filename  = filename.substr(0, filename.length()-4);
    std::string file_extension = filename.substr(filename.length()-4, 4);
    if(file_extension!=".asm")
        return false;

    return true;
}


int main(int argc, char const *argv[]){
    std::string base_filename;
    if (!verify_arguments(argc, argv, base_filename)){
        std::cerr<<"Usage: assembler [asm_filename]"<<std::endl;
        return -1;
    }

    std::ifstream fin(base_filename + ".asm");
    if(!fin){
        std::cerr<<"Error opening file "<<(base_filename + ".asm")<<std::endl;
        fin.close();
        return -1;
    }

    std::string line;
    SymbolTable sym_table;
    Parser parser;
    unsigned int line_num = 0;
    unsigned int instruction_num = 0;

    // First pass -> for filling symbol table with label symbols and checking error in program
    while(getline(fin, line)){
        if(line.length() && line[line.length()-1]=='\r')
            line.pop_back();

        ParserMessage curr_res = parser.parse(line, instruction_num);
        if(curr_res.status == -1){
            std::cerr<<"Error in line number: "<<line_num<<", instruction number: "<<instruction_num<<std::endl;
            std::cerr<<"Error msg: "<<curr_res.error_msg<<std::endl;
            fin.close();
            return -1;
        }

        // when curr_res.status == 0
        assert((curr_res.next_instruction_num == instruction_num) || (curr_res.next_instruction_num == (instruction_num+1)));

        // The current instruction was a valid new instruction
        instruction_num = curr_res.next_instruction_num;
        switch(curr_res.instruction_type){
            case 'A':  break;         // will process in second pass
            case 'C':  break;         // will process in second pass
            case 'L':{
                std::string label = curr_res.symbol;
                sym_table.addEntry(label, instruction_num);
                break;
            }
            case 'B':{
                // blank line or comment
                break;
            }
            default:{
                std::cerr<<"Fatal error: Wrong instruction type"<<std::endl;
                fin.close();
                return -1;
            }
        }
        line_num++;
    }
    
    fin.clear();
    fin.seekg(0, fin.beg);
    std::ofstream fout(base_filename + ".hack");
    Code code;
    unsigned int data_memory = 16;
    instruction_num = 0;

    // Second pass -> for converting instructions to binary form
    while(getline(fin, line)){
        if(line.length() && line[line.length()-1]=='\r')
            line.pop_back();

        ParserMessage curr_res = parser.parse(line, instruction_num);

        // we know curr_res.status == 0
        assert((curr_res.next_instruction_num == instruction_num) || (curr_res.next_instruction_num == (instruction_num+1)));
        instruction_num = curr_res.next_instruction_num;

        switch(curr_res.instruction_type){
            case 'A': {
                unsigned int curr_address;

                if(curr_res.symbol_type == "symbol"){
                    if(sym_table.contains(curr_res.symbol)){
                        curr_address = sym_table.getAddress(curr_res.symbol);
                    }
                    else{
                        sym_table.addEntry(curr_res.symbol, data_memory);
                        curr_address = data_memory;
                        data_memory += 1;
                    }
                }
                else if(curr_res.symbol_type == "decimal"){
                    curr_address = curr_res.symbol_value;
                }

                std::string curr_instruction = "0" + code.decimalToAddress(curr_address);
                fout<<curr_instruction<<"\n";
                break;
            };
            case 'C':{
                std::string curr_instruction = "111" + code.comp(curr_res.comp) +  code.dest(curr_res.dest) + code.jump(curr_res.jump);
                fout<<curr_instruction<<"\n";
                break;
            };
            case 'B':{
                // blank line or comment
                break;
            }
            case 'L': break;        // already processed in first pass
            default:{
                std::cerr<<"Fatal error: Wrong instruction type"<<std::endl;
                fin.close();
                return -1;
            }
        }
        line_num++;
    }

    fout.close();
    fin.close();
    return 0;
}