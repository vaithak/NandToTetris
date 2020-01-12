#include "Parser.h"
#include <sstream>

using namespace Hack::VMTranslator;

Parser::Parser() : allowedArithmetic(9), allowedSegments(8), allowedMemoryAccess(2), allowedProgramFlow(3), allowedFunctionCalls(3){
    allowedArithmetic[0] = "add";
    allowedArithmetic[1] = "sub";
    allowedArithmetic[2] = "neg";
    allowedArithmetic[3] = "eq";
    allowedArithmetic[4] = "gt";
    allowedArithmetic[5] = "lt";
    allowedArithmetic[6] = "and";
    allowedArithmetic[7] = "or";
    allowedArithmetic[8] = "not";

    allowedSegments[0] = "argument";
    allowedSegments[1] = "local";
    allowedSegments[2] = "static";
    allowedSegments[3] = "constant";
    allowedSegments[4] = "this";
    allowedSegments[5] = "that";
    allowedSegments[6] = "pointer";
    allowedSegments[7] = "temp";

    allowedMemoryAccess[0] = "push";
    allowedMemoryAccess[1] = "pop";

    allowedProgramFlow[0] = "label";
    allowedProgramFlow[1] = "goto";
    allowedProgramFlow[2] = "if-goto";

    allowedFunctionCalls[0] = "function";
    allowedFunctionCalls[1] = "call";
    allowedFunctionCalls[2] = "return";
}


static bool presentInArray(const std::string &s, const std::vector<std::string> array){
    for(int i=0; i<array.size(); i++){
        if(array[i] == s) return true;
    }

    return false;
}


static bool valid_string(const std::string &s){
    return true;
}


ParserMessage Parser::error_message(const std::string &msg){
    ParserMessage message;
    message.status = -1;
    message.error_msg = msg;
    return message;
}


bool Parser::updateMessage(ParserMessage &message, const std::string &token, int token_num){
    if(token_num > 2){
        message = error_message("Too many arguments");
        return false;
    }

    // handling token number 1
    if(token_num == 1){
        if(message.command_type == "C_ARITHMETIC" || message.command_type == "C_RETURN"){
            message = error_message("Too many arguments");
            return false;
        }

        if(message.command_type == "C_PUSH"){
            if(presentInArray(token, allowedSegments)){
                message.arg1 = token;
                return true;
            }
            else{
                message = error_message("Invalid argument: " + token);
                return false;
            }
        }
        else if(message.command_type == "C_POP"){
            if(presentInArray(token, allowedSegments) && token!="constant"){
                message.arg1 = token;
                return true;
            }
            else{
                message = error_message("Invalid argument: " + token);
                return false;    
            }
        }
        else{
            if(valid_string(token)){
                message.arg1 = token;
                return true;
            }
            else{
                message = error_message("Invalid argument: " + token);
                return false;
            }    
        }
    }

    // handling token number 2
    if(token_num == 2){
        std::vector <std::string> three_argument_commands(4);
        three_argument_commands[0] = "C_PUSH";
        three_argument_commands[1] = "C_POP";
        three_argument_commands[2] = "C_FUNCTION";
        three_argument_commands[2] = "C_CALL";
        
        if(!presentInArray(message.command_type, three_argument_commands)){
            message = error_message("Too many arguments");
            return false;
        }

        try {
            int x = stoi(token);
            message.arg2 = x;
            return true;
        }
        catch(...){
            message = error_message("Invalid Argument: " + token);
            return false;
        }
    }

    return false;
}


ParserMessage Parser::blankOrCommentMessage(const int command_num){
    ParserMessage message;
    message.status = 0;
    message.command_type = "C_BLANK";
    message.next_command_num = command_num;
    return message;
}


ParserMessage Parser::parse(const std::string &line, unsigned int command_num){
    std::stringstream inp(line);
    int token_num = -1;
    ParserMessage message;

    while(inp){
        std::string token; 
        inp >> token; 
        ++token_num;

        if(token.length() == 0){
            if (token_num == 0)
                return blankOrCommentMessage(command_num);
            else
                continue;
        }

        if(token == "//"){
            if(token_num == 0)  return blankOrCommentMessage(command_num);
            else  break;
        }

        switch(token_num){
            case 0: {
                // arithmetic command
                if(presentInArray(token, allowedArithmetic)){
                    message.status = 0;
                    message.command_type = "C_ARITHMETIC";
                    message.arg1 = token;
                    message.next_command_num = command_num+1;
                }

                // push pop command
                if(presentInArray(token, allowedMemoryAccess)){
                    message.status = 0;

                    if(token == "push")
                        message.command_type = "C_PUSH";
                    else
                        message.command_type = "C_POP";

                    message.next_command_num = command_num+1;
                }

                // program flow command
                if(presentInArray(token, allowedProgramFlow)){
                    message.status = 0;

                    if(token == "label")
                        message.command_type = "C_LABEL";
                    else if(token == "goto")
                        message.command_type = "C_GOTO";
                    else
                        message.command_type = "C_IF";

                    message.next_command_num = command_num+1;
                }

                // function call or return command
                if(presentInArray(token, allowedFunctionCalls)){
                    message.status = 0;

                    if(token == "function")
                        message.command_type = "C_FUNCTION";
                    else if(token == "call")
                        message.command_type = "C_CALL";
                    else
                        message.command_type = "C_RETURN";

                    message.next_command_num = command_num+1;
                }

                break;
            }
            default:{
                bool status = updateMessage(message, token, token_num);
                if(!status)
                    return message;
                break;
            }
        }
    }

    if(token_num == -1)
        return blankOrCommentMessage(command_num);

    return message;
}