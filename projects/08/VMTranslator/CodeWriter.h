#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include <fstream>
#include <string>
#include <unordered_map>

namespace Hack{
    namespace VMTranslator{
        class CodeWriter{
        private:
            int labelId;
            std::ofstream fout;
            std::string filename;

            std::unordered_map<std::string, std::string> binary_operators;
            std::unordered_map<std::string, std::string> unary_operators;
            std::unordered_map<std::string, std::string> compare_operators;

            std::unordered_map<std::string, std::string> segment_code_map;

            std::string generateLabel(std::string name);
            std::string generateStaticSymbol(std::string str_index);

        public:
            CodeWriter(const std::string &file, bool is_dir);
            void writeInit();
            void changeFilename(std::string);
            void writeArithmetic(std::string command);
            void writePushPop(std::string command, std::string segment, int index);
            void writeLabel(std::string labelName);
            void writeGotoLabel(std::string labelName);
            void writeGoto(std::string labelName);
            void writeIfGotoLabel(std::string labelName);
            void writeFunction(std::string functionName, int numLocal);
            void writeCall(std::string functionName, int numArgs);
            void writeReturn();
            void close();
        };
    }
}

#endif