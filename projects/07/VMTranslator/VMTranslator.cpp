#include <iostream>
#include <fstream>
#include <string>
#include "Parser.h"
#include "CodeWriter.h"
#include <cassert>

using namespace Hack::VMTranslator;

// check command line arguments
bool verify_arguments(int argc, char const *argv[], std::string &base_filename){
    if (argc != 2)
        return false;

    std::string filename(argv[1]);
    if(filename.length() <= 2)
        return false;

    base_filename  = filename.substr(0, filename.length()-3);
    std::string file_extension = filename.substr(filename.length()-3, 3);
    if(file_extension!=".vm")
        return false;

    return true;
}


int main(int argc, char const *argv[]){
    std::string base_filename = "";
    if (!verify_arguments(argc, argv, base_filename)){
        std::cerr<<"Usage: VMTranslator [vm_filename]"<<std::endl;
        return -1;
    }

    std::ifstream fin(base_filename + ".vm");
    if(!fin){
        std::cerr<<"Error opening file "<<(base_filename + ".vm")<<std::endl;
        fin.close();
        return -1;
    }


    Parser parser;
    std::string line;
    unsigned int line_num = 0;
    unsigned int command_num = 0;
    CodeWriter writer(base_filename);

    while(getline(fin, line)){
        if(line.length() && line[line.length()-1]=='\r')
            line.pop_back();

        ParserMessage curr_res = parser.parse(line, command_num);
        if(curr_res.status == -1){
            std::cerr<<"Error in line number: "<<line_num<<", command number: "<<command_num<<std::endl;
            std::cerr<<"Error msg: "<<curr_res.error_msg<<std::endl;
            fin.close();
            return -1;
        }

        // when curr_res.status == 0
        assert((curr_res.next_command_num == command_num) || (curr_res.next_command_num == (command_num+1)));

        // The current instruction was a valid new instruction
        command_num = curr_res.next_command_num;
        if(curr_res.command_type == "C_ARITHMETIC")
            writer.writeArithmetic(curr_res.arg1);

        if(curr_res.command_type == "C_PUSH" || curr_res.command_type == "C_POP")
            writer.writePushPop(curr_res.command_type, curr_res.arg1, curr_res.arg2);

        // will process in eighth assignment
        // if(curr_res.command_type == "C_LABEL")
        // if(curr_res.command_type == "C_GOTO")
        // if(curr_res.command_type == "C_IF")
        // if(curr_res.command_type == "C_FUNCTION")
        // if(curr_res.command_type == "C_RETURN")
        // if(curr_res.command_type == "C_CALL")

        line_num++;
    }

    writer.close();
    fin.close();
    return 0;
}