
#include <fstream>
#include <iostream>
#include <sstream>

#include "xmlstruct.hpp"

int main(int argc, char *argv[])
{
    for(int i = 1; i < argc; ++i)
    {
        std::ifstream ifs(argv[i]);
        if(!ifs)
        {
            std::cout << "file not found: " << argv[i];
            return -1;
        }
        ifs.seekg(0, std::ios::end);
        auto size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        if(size <= 0)
        {
            std::cout << "file size error: " << argv[i];
            return -1;
        }

        std::string xml;
        xml.resize(size);
        ifs.read(&xml[0], size);

        try
        {
            xmlstruct::parser parser;
			parser.parse(&xml.front());

            std::ofstream head(std::string(argv[i]) + ".h");
            std::ofstream source(std::string(argv[i]) + ".cpp");
            head << "#pragma once\n";
            head << "\n";
            head << "#include <vector>\n";
            head << "#include <string>\n";
            head << "\n";
            head << "#include <rapidxml.hpp>\n";
            head << "\n\n";
            parser.genDef(head);

            source << "\n";
            source << "#include \"" << argv[i] << ".h\"\n";
            source << "#include <cstring>\n";
            source << "#include <sstream>\n";
            source << "\n";
            source << "\n";
            parser.genBaseFunc(source);
            parser.genCodeSx(source);
            parser.genCodeXs(source);
            std::cout << argv[i] << ":  ok..." << std::endl;
        }
        catch(const xmlstruct::parse_error& err)
        {
            std::cout << argv[i] << ">> " << err.where() << ":" << err.what() << std::endl;

        }

    }
}
