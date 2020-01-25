#include <iostream>
#include <vector>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "Tokenizer.h"
#include "CompilationEngine.h"

using namespace Hack::Compiler;


void listdir(const char *name, std::vector<std::string> &ans){
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
 
    if (!(dir = opendir(name))){
        std::cout<<"Couldn't open the file or dir "<<name<<"\n";
        return;
    }

    if (!(entry = readdir(dir))){
        std::cout<<"Couldn't read the file or dir "<<name<<"\n";
        return;
    }

    do{
        std::string parent(name);
        std::string final;
        std::string file(entry->d_name);

        if(parent[parent.length()-1]=='/')
            final = parent + file;
        else
            final = parent + "/" + file;

        if(stat(final.c_str(), &statbuf)==-1){
            std::cout<<"Couldn't get the stat info of file or dir: "<<final<<"\n";
            return;
        }

        ans.push_back(final);
    } while (entry = readdir(dir));
    closedir(dir);
}


int main(int argc, char const *argv[]){
	if(argc!=2){
	    std::cout<<"Invalid usage\n";
	    return -1;
	}
	
	// assuming that path input will be valid
	// because I am lazy to handle errors properly
	bool isDir = false;
	std::string pathInp(argv[1]);
	if(pathInp.length()<=5 || pathInp.substr(pathInp.length()-5)!=".jack")
		isDir=true;
	
	int status = -1;
	if(isDir){
	    std::vector<std::string> ans;
	    listdir(argv[1], ans);
	    for(int i=0; i<ans.size(); i++){
		if(ans[i].size()>=5 && (ans[i].substr(ans[i].length()-5) == ".jack" )){
		    std::string outputfile = ans[i].substr(0,ans[i].length()-5) + ".xml";
		    CompilationEngine engine(ans[i], outputfile);
		    status = engine.parse();
		    engine.close();
		    
		    if(status == -1)
			break;
		}
	    }
	}
	else{
	    if(pathInp.length()>=5 && (pathInp.substr(pathInp.length()-5) == ".jack" )){
		std::string outputfile = pathInp.substr(0,pathInp.length()-5) + ".xml";
		CompilationEngine engine(pathInp, outputfile);
		status = engine.parse();
		engine.close();
	    }
	    else{
		std::cerr<<"Wrong filename! \n";
		status = -1;
	    }
	}
	
	return status;
	
	/*
	JackTokenizer tokenizer(argv[1]);
	while(tokenizer.advance()){
		int type = tokenizer.tokenType();
		switch(type){
			case KEYWORD:{
				std::cout<<"<keyword> ";
				std::cout<<tokenizer.keyWord();
				std::cout<<" </keyword>";
				break;
			}
			case SYMBOL:{
				std::cout<<"<symbol> ";
				std::cout<<tokenizer.symbol();
				std::cout<<" </symbol>";
				break;
			}
			case IDENTIFIER:{
				std::cout<<"<identifier> ";
				std::cout<<tokenizer.identifier();
				std::cout<<" </identifier>";
				break;
			}
			case INT_CONST:{
				std::cout<<"<integerConstant> ";
				std::cout<<tokenizer.intVal();
				std::cout<<" </integerConstant>";
				break;
			}
			case STRING_CONST:{
				std::cout<<"<stringConstant> ";
				std::cout<<tokenizer.stringVal();
				std::cout<<" </stringConstant>";
				break;
			}
		}
		std::cout<<"\n";
	}
	std::cout<<"</tokens>";
	*/
	return 0;
}
