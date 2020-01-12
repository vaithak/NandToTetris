#include "CodeWriter.h"

using namespace Hack::VMTranslator;

CodeWriter::CodeWriter(const std::string &file){
    this->filename = file;
    int index = 0;
    for(int i=this->filename.length()-1; i>=0; i--){
        if(this->filename[i] == '/'){
            index = i;
            break;
        }
    }
    this->filename = this->filename.substr(index+1);

    this->labelId = 0;
    fout.open(file + ".asm");

    unary_operators["not"] = "!";
    unary_operators["neg"] = "-";

    binary_operators["add"] = "+";
    binary_operators["and"] = "&";
    binary_operators["or"]  = "|";
    binary_operators["sub"] = "-";

    compare_operators["eq"] = "JEQ";
    compare_operators["lt"] = "JLT";
    compare_operators["gt"] = "JGT";

    segment_code_map["local"] = "LCL";
    segment_code_map["argument"] = "ARG";
    segment_code_map["this"] = "THIS";
    segment_code_map["that"] = "THAT";
}


std::string CodeWriter::generateLabel(std::string name){
    labelId++;
    return (filename + name + std::to_string(labelId));
}


std::string CodeWriter::generateStaticSymbol(std::string str_index){
    return (filename + "." + str_index);
}


void CodeWriter::writeArithmetic(std::string command){
    fout<<"//"<<command<<"\n";

    if(unary_operators.find(command) != unary_operators.end()){
        fout<<"@SP"<<"\n";
        fout<<"A=M-1"<<"\n";
        fout<<("M=" + unary_operators[command] + "M\n");
    }
    else if(binary_operators.find(command) != binary_operators.end()){
        // extract top value as well as fix stack pointer
        fout<<"@SP"<<"\n";
        fout<<"AM=M-1"<<"\n";
        fout<<"D=M"<<"\n";

        // extract second value
        fout<<"@SP"<<"\n";
        fout<<"A=M-1"<<"\n";

        if(command == "sub")
            fout<<("M=M" + binary_operators[command] + "D\n");
        else
            fout<<("M=D" + binary_operators[command] + "M\n");
    }
    else if(compare_operators.find(command) != compare_operators.end()){
        fout<<"@SP"<<"\n";
        fout<<"A=M-1"<<"\n";
        fout<<"D=M"<<"\n";
        fout<<"A=A-1"<<"\n";

        fout<<"M=M-D"<<"\n";

        // Fix stack pointer
        fout<<"D=A+1"<<"\n";
        fout<<"@SP"<<"\n";
        fout<<"M=D"<<"\n";

        // check topmost value
        fout<<"A=D-1"<<"\n";
        fout<<"D=M"<<"\n";
        std::string curr_true_label = generateLabel("TRUE");
        fout<<"@" + curr_true_label<<"\n";
        fout<<("D;" + compare_operators[command])<<"\n";

        // No need for false label as it will execute if no jump, but would have to jump over TRUE
        fout<<"@SP"<<"\n";
        fout<<"A=M"<<"\n";
        fout<<"A=A-1"<<"\n";
        fout<<"M=0"<<"\n";
        std::string curr_end_label = generateLabel("END");
        fout<<"@" + curr_end_label<<"\n";
        fout<<"0;JMP"<<"\n";

        // TRUE label
        fout<<"(" + curr_true_label + ")"<<"\n";
        fout<<"@SP"<<"\n";
        fout<<"A=M"<<"\n";
        fout<<"A=A-1"<<"\n";
        fout<<"M=-1"<<"\n";

        // END of comparison label
        fout<<"(" + curr_end_label + ")"<<"\n";
    }

    fout<<std::endl;
}


void CodeWriter::writePushPop(std::string command, std::string segment, int index){
    std::string str_index = std::to_string(index);

    if(command == "C_PUSH"){
        fout<<"//"<<"push " + segment + " " + str_index<<"\n";

        if(segment == "constant"){
            // push value on stack
            fout<<"@" + str_index<<"\n";
            fout<<"D=A"<<"\n";
            fout<<"@SP"<<"\n";
            fout<<"A=M"<<"\n";
            fout<<"M=D"<<"\n";
            
            // update stack pointer
            fout<<"@SP"<<"\n";
            fout<<"M=M+1"<<"\n";
        }
        else if(segment == "local" || segment == "argument" || segment == "this" || segment == "that"){
            // extracting value from segment area
            fout<<"@" + str_index<<"\n";
            fout<<"D=A"<<"\n";
            fout<<"@" + segment_code_map[segment]<<"\n";
            fout<<"A=M+D"<<"\n";
            fout<<"D=M"<<"\n";

            // updating it in stack
            fout<<"@SP"<<"\n";
            fout<<"A=M"<<"\n";
            fout<<"M=D"<<"\n";

            // update stack pointer
            fout<<"@SP"<<"\n";
            fout<<"M=M+1"<<"\n";
        }
        else if(segment == "temp" || segment == "pointer"){
            // extract value from segment area
            fout<<"@" + str_index<<"\n";
            fout<<"D=A"<<"\n";
            
            if(segment == "temp")
                fout<<"@5"<<"\n";
            else
                fout<<"@3"<<"\n";

            fout<<"A=D+A"<<"\n";
            fout<<"D=M"<<"\n";

            // push that value onto stack
            fout<<"@SP"<<"\n";
            fout<<"A=M"<<"\n";
            fout<<"M=D"<<"\n";

            // update stack pointer
            fout<<"@SP"<<"\n";
            fout<<"M=M+1"<<"\n";
        }
        else{
            // extract value from segment area
            fout<<("@" + generateStaticSymbol(str_index))<<"\n";
            fout<<"D=M"<<"\n";

            // push that value onto stack
            fout<<"@SP"<<"\n";
            fout<<"A=M"<<"\n";
            fout<<"M=D"<<"\n";

            // update stack pointer
            fout<<"@SP"<<"\n";
            fout<<"M=M+1"<<"\n";
        }
    }
    else{
        fout<<"//"<<"pop " + segment + " " + std::to_string(index)<<"\n";

        if(segment == "local" || segment == "argument" || segment == "this" || segment == "that"){
            // storing address of segment location into R13
            fout<<"@" + str_index<<"\n";
            fout<<"D=A"<<"\n";
            fout<<"@" + segment_code_map[segment]<<"\n";
            fout<<"D=M+D"<<"\n";
            fout<<"@R13"<<"\n";
            fout<<"M=D"<<"\n";

            // update stack pointer and extract value
            fout<<"@SP"<<"\n";
            fout<<"AM=M-1"<<"\n";
            fout<<"D=M"<<"\n";

            // updating it in segment area
            fout<<"@R13"<<"\n";
            fout<<"A=M"<<"\n";
            fout<<"M=D"<<"\n";
        }
        else if(segment == "temp" || segment == "pointer"){
            // extract address of segment location and store into R13
            fout<<"@" + str_index<<"\n";
            fout<<"D=A"<<"\n";
            
            if(segment == "temp")
                fout<<"@5"<<"\n";
            else
                fout<<"@3"<<"\n";

            fout<<"D=D+A"<<"\n";
            fout<<"@R13"<<"\n";
            fout<<"M=D"<<"\n";

            // pop value from stack and update stack pointer
            fout<<"@SP"<<"\n";
            fout<<"AM=M-1"<<"\n";
            fout<<"D=M"<<"\n";

            // Store popped value into segment location
            fout<<"@R13"<<"\n";
            fout<<"A=M"<<"\n";
            fout<<"M=D"<<"\n";
        }
        else{
            // pop value from stack and update stack pointer
            fout<<"@SP"<<"\n";
            fout<<"AM=M-1"<<"\n";
            fout<<"D=M"<<"\n";

            // Store popped value into static location
            fout<<("@" + generateStaticSymbol(str_index))<<"\n";
            fout<<"M=D"<<"\n";
        }
    }

    fout<<std::endl;
}


void CodeWriter::close(){
    fout.close();
}