#include <iostream>
#include <fstream>
#include <string>
#include "Parser.h"
#include "CodeWriter.h"
#include <cassert>
#include <filesystem>

using namespace Hack::VMTranslator;

// check command line arguments
bool verify_arguments(int argc, char const *argv[], std::string &base_filename, bool &bool_dir){
    if (argc != 2)
        return false;

    std::string filename(argv[1]);
    if(filename.length() <= 2)
        return false;

    if(std::filesystem::is_directory(filename)){
        bool_dir = true;
        base_filename = filename;

        if(base_filename[base_filename.length()-1]=='/')
            base_filename.pop_back();

        return true;
    }

    bool_dir = false;
    base_filename  = filename.substr(0, filename.length()-3);
    std::string file_extension = filename.substr(filename.length()-3, 3);

    if(file_extension!=".vm")
        return false;

    return true;
}


// Translate the given fileand write with provided CodeWriter
int translateFile(const std::string &filename, CodeWriter &writer){
    std::string line;
    Parser parser;
    unsigned int line_num = 0;
    unsigned int command_num = 0;
    std::string function_name = "";

    std::ifstream fin(filename);
    if(!fin){
        std::cerr<<"Error opening file "<<filename<<std::endl;
        fin.close();
        return -1;
    }

    while(getline(fin, line)){
        if(line.length() && line[line.length()-1]=='\r')
            line.pop_back();

        ParserMessage curr_res = parser.parse(line, command_num);
        if(curr_res.status == -1){
            std::cerr<<"Error in file: "<<filename<<", line number: "<<line_num<<", command number: "<<command_num<<std::endl;
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

        if(curr_res.command_type == "C_LABEL")
            writer.writeLabel(function_name + ":" + curr_res.arg1);

        if(curr_res.command_type == "C_GOTO")
            writer.writeGotoLabel(function_name + ":" + curr_res.arg1);

        if(curr_res.command_type == "C_IF")
            writer.writeIfGotoLabel(function_name + ":" + curr_res.arg1);

        if(curr_res.command_type == "C_FUNCTION"){
            function_name = curr_res.arg1;
            writer.writeFunction(curr_res.arg1, curr_res.arg2);
        }

        if(curr_res.command_type == "C_RETURN"){
            writer.writeReturn();
        }

        if(curr_res.command_type == "C_CALL")
            writer.writeCall(curr_res.arg1, curr_res.arg2);

        line_num++;
    }

    fin.close();
    return 0;
}


int main(int argc, char const *argv[]){
    std::string base_filename = "";
    bool bool_dir = false;

    if (!verify_arguments(argc, argv, base_filename, bool_dir)){
        std::cerr<<"Usage: VMTranslator [vm_filename]"<<std::endl;
        return -1;
    }

    int status = 0;
    if(bool_dir == true){
        CodeWriter writer(base_filename, true);
        std::string directory_name(argv[1]);
        writer.writeInit();
        writer.writeCall("Sys.init", 0);
        std::filesystem::path current_dir(directory_name);
        for (auto &file : std::filesystem::recursive_directory_iterator(current_dir)) {
            if (file.path().extension() == ".vm")
                translateFile(file.path().string(), writer);
        }
        writer.close();
    }
    else{
        CodeWriter writer(base_filename, false);
        status = translateFile(base_filename + ".vm", writer);
        writer.close();
    }
    
    return status;
}