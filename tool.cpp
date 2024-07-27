#include "tool.hpp"
#include <fstream>
#include <iostream>


std::string ReadModuleFile(const std::string & path)
{
    std::ifstream file(path,std::ios::binary|std::ios::ate);

    if(!file.is_open()){
        std::cout << "read "<< path <<" failed!"<< std::endl;
        return std::string();
    }
    auto size = file.tellg();
    std::string content;
    content.resize(size);
    file.seekg(0);
    file.read(content.data(),size);
    return content;
}