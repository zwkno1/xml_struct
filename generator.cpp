#include "xmlstruct.h"

#include <fstream>
#include <iostream>
#include <sstream>

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
            xmlstruct::parser parser(xml);

            std::ofstream head(std::string(argv[i]) + ".h");
            std::ofstream source(std::string(argv[i]) + ".cpp");
            parser.genNobug(head);
            head << "\n\n";
            head << "#include <rapidxml.h>\n";
            head << "\n";
            head << "#include <vector>\n";
            head << "\n\n";
            parser.genDef(head);
            head.flush();
            head.close();

            parser.genNobug(source);

            source << "\n";
            source << "#include \"" << argv[i] << ".h\"\n";
            source << "#include <cstring>\n";
            source << "#include <sstream>\n";
            source << "\n";
            source << "\n";
            parser.genBaseFunc(source);
            parser.genCodeSx(source);
            parser.genCodeXs(source);
            source.flush();
            source.close();
            std::cout << argv[i] << ":  ok..." << std::endl;
        }
        catch(const std::string & err)
        {
            std::cout << argv[i] << ":  " << err << std::endl;
        }

    }
}
