#include "Parser.h"
#include <iostream>
using namespace Hack::Assembler;

ParserMessage Parser::error_message(const std::string &msg){
    ParserMessage message;
    message.status = -1;
    message.error_msg = msg;
    return message;
}

bool char_in_string(const std::string &s, char ch){
    for(int i=0; i<s.length(); i++)
        if(s[i]==ch)  
            return true;

    return false;
}

// Last element of array is "end"
bool check_string_in_array(std::string s, std::string arr[]){
    int i=0;
    while(arr[i]!="end"){
        if(s==arr[i])   return true;
        ++i;
    }

    return false;
}

bool is_digit(char ch){
    if(ch>=48 && ch<=57)
        return true;
    return false;
}

bool is_lowercase_char(char ch){
    if(ch>=97 && ch<=122)
        return true;
    return false;
}

bool is_uppercase_char(char ch){
    if(ch>=65 && ch<=90)
        return true;
    return false;
}

bool Parser::validate_a_type_instruction(const std::string &instruction){
    if(instruction.length()<=1 || instruction[0]!='@')
        return false;

    if(is_digit(instruction[1])){
        for(int i=2; i<instruction.length(); i++)
            if(!is_digit(instruction[i]))
                return false;
    }
    else{
        std::string allowed_extra_chars = "_.$:";
        for(int i=2; i<instruction.length(); i++)
            if(!(is_digit(instruction[i]) || is_uppercase_char(instruction[i]) || is_lowercase_char(instruction[i]) || char_in_string(allowed_extra_chars, instruction[i])))
                return false;
    }

    return true;
}

bool Parser::validate_l_type_instruction(const std::string &instruction){
    if(instruction.length()<=2 || instruction[0]!='(' || instruction[instruction.length()-1]!=')')
        return false;

    std::string allowed_extra_chars = "_.$:";
    for(int i=1; i<instruction.length()-1; i++)
        if(!(is_digit(instruction[i]) || is_uppercase_char(instruction[i]) || is_lowercase_char(instruction[i]) || char_in_string(allowed_extra_chars, instruction[i])))
            return false;

    return true;
}

bool Parser::validate_c_type_instruction(const std::string &instruction){
    std::string allowed_jumps[] = {"", "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP", "end"};
    std::string allowed_comps[] = {"", "0", "1", "-1", "D", "A", "!D", "!A", "~D", "~A", "D+1", "A+1", "D-1", "A-1", "D+A", "D-A", "A-D", "D&A", "D|A", "M", "!M", "~M", "M+1", "M-1", "D+M", "D-M", "M-D", "D&M", "D|M", "end"};
    std::string allowed_dests[] = {"", "M", "D", "MD", "A", "AM", "AD", "AMD", "end"};
    
    unsigned int last_index_comp = instruction.length()-1;
    if(char_in_string(instruction, ';')){
        int i = instruction.length()-1;
        for(; i>=0; i--)
            if(instruction[i]==';')
                break;
        std::string jmp = instruction.substr(i+1, instruction.length()-i-1);
        if(!check_string_in_array(jmp, allowed_jumps))
            return false;

        last_index_comp = i-1;
    }
    
    unsigned int first_index_comp = 0;
    if(char_in_string(instruction, '=')){
        int i = 0;
        for(; i<=last_index_comp; i++)
            if(instruction[i]=='=')
                break;
        std::string dest = instruction.substr(0, i);
        if(!check_string_in_array(dest, allowed_dests))
            return false;

        first_index_comp = i+1;
    }

    std::string comp = instruction.substr(first_index_comp, last_index_comp-first_index_comp+1);
    if(!check_string_in_array(comp, allowed_comps))
            return false;

    return true;
}

void Parser::tokenize_c_instruction(const std::string &instruction, std::string tokens[3]){
    tokens[0] = "";
    unsigned int i=0;
    unsigned int mark=i;
    for(; i<instruction.length(); i++)
        if(instruction[i] == '=')
            break;
    if(i==instruction.length())  i=0;
    tokens[0] = instruction.substr(mark, i-mark);


    tokens[1] = "";
    if(instruction[i] == '=')  i++;
    mark = i;
    for(; i<instruction.length(); i++)
        if(instruction[i] == ';')
            break;
    tokens[1] = instruction.substr(mark, i-mark);


    tokens[2] = "";
    if(i==instruction.length()) return;
    mark=i+1;
    i=instruction.length();
    tokens[2] = instruction.substr(mark, i-mark);
}

ParserMessage Parser::parse(const std::string &line, unsigned int instruction_num){
    std::string filtered_line = "";     // will contain instruction with spaces and comments removed

    for(int i=0; i<line.length(); i++){
        if(line[i] == ' ')   continue;
        if(line[i] == '/'){
            if((i+1) == line.length() || line[i+1] != '/'){
                return error_message("Invalid literal");
            }
            // rest of the line is comment    
            break;
        }

        filtered_line += std::string(1, line[i]);
    }

    ParserMessage message;

    // Blank line or commented line
    if(filtered_line.length() == 0){
        message.status = 0;
        message.next_instruction_num = instruction_num;
        message.instruction_type = 'B';
        return message;
    }

    if(filtered_line[0]=='@'){
        // A-instruction
        if(!validate_a_type_instruction(filtered_line))
            return error_message("Invalid A-instruction");

        message.status = 0;
        if(is_digit(filtered_line[1])){
            message.next_instruction_num = instruction_num + 1;
            message.instruction_type = 'A';
            message.symbol_type  = "decimal";
            message.symbol_value = stoi(filtered_line.substr(1, filtered_line.length()-1));
        }
        else{
            message.next_instruction_num = instruction_num + 1;
            message.instruction_type = 'A';
            message.symbol_type  = "symbol";
            message.symbol = filtered_line.substr(1, filtered_line.length()-1);
        }
    }
    else if(filtered_line[0]=='('){
        // L type instruction
        if(!validate_l_type_instruction(filtered_line))
            return error_message("Invalid label defining");

        message.status = 0;
        message.next_instruction_num = instruction_num;
        message.instruction_type = 'L';
        message.symbol_type  = "symbol";
        message.symbol = filtered_line.substr(1, filtered_line.length()-2);
    }
    else{
        // C-instruction
        if(!validate_c_type_instruction(filtered_line))
            return error_message("Invalid C-instruction");

        message.next_instruction_num = instruction_num + 1;
        message.instruction_type = 'C';

        std::string tokens[3];
        tokenize_c_instruction(filtered_line, tokens);
        message.dest = tokens[0];
        message.comp = tokens[1];
        message.jump = tokens[2];
    }

    return message;
}