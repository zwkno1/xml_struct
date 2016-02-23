#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <rapidxml.h>
#include <cstring>

std::string end_def() { return ";\n\n"; }
std::string space() { return " "; }
std::string tab() { return "    "; }

std::string end_line() { return "\n"; }
std::string end_line2() { return "\n\n"; }

//description
std::string start_desc() { return "//"; }
std::string end_desc() { return "\n"; }

//structure
std::string start_struct() { return " \n{ \n\n"; }
std::string end_struct() { return "};\n\n"; }

//sequence
std::string start_seq() { return "std::vector< "; }
std::string end_seq() { return " >"; }


class base
{
public:
    std::string type;
    std::string def;
    std::string name;
    std::string desc;
    virtual void genDef(std::ostream & os)
    {
        os << start_desc() << desc << end_line();
        os << def << tab() << name << end_def();
    }

    virtual void genCode(std::ostream & os)
    {
        os << "if(!node || std::strcmp(node->name(), \"" << name << "\"))\n"
              "throw std::string(\"unexpect node: \") + node->name();\n"
              "ret." << name << " = getValue<" << def << ">(node->value());\n"
              "node = node->next_sibling();\n\n";
    }
};

class sequnece : public base
{
public:
    sequnece()
    {
        type = "seq";
    }
    virtual void genDef(std::ostream & os)
    {
        os << start_desc() << desc << end_line();
        os << start_seq() << def << end_seq()
           << space() << name << end_def();
    }

    virtual void genCode(std::ostream & os)
    {
        os << "if(!node || std::strcmp(node->name(), \"Sequence\"))\n"
              "throw std::string(\"unexpect node: \") + node->name();\n"
              "auto attr = node->first_attribute(\"name\");\n"
              "if(!attr || std::strcmp(attr->value(), \"" << name <<"\"))\n"
              "throw std::string(\"unexpect node: \") + node->name();\n"
              "for(auto i = node->first_node(); i; i = i->next_sibling())"
              "{\n"
           << def << " tmp;\n"
              "tmp << node;\n"
              "}\n"
              "node = node->next_sibling();\n";
    }
};

class structure : public base
{
public:
    structure()
    {
        type = "struct";
    }
    void append(base * child)
    {
        children_.push_back(child);
    }

    virtual void genDef(std::ostream & os)
    {
        os << start_desc() << desc << end_line();
        os << def << space() << name << start_struct();
        for(auto i : children_)
        {
            i->genDef(os);
        }
        os << end_struct();
        os << "rapidxml::xml_node<> * operator <<(rapidxml::xml_node<> * , const " << name << " & );" << end_line();
        os << "rapidxml::xml_node<> * operator >>(rapidxml::xml_node<> * , " << name << " & );" << end_line2();
    }

    virtual void genCode(std::ostream & os)
    {
        os << name << " & operator <<(rapidxml::xml_node<> * node, const " << name << " & src) \n{ \n" ;
        os << "if(!node || std::strcmp(node->name(), \"" << name << "\")) \n"
              "throw std::string(\"unexpect node: \") + node->name();\n"
              "node = node->first_node();\n\n";
        for(auto i : children_)
        {
            i->genCode(os);
        }
        os << "} \n\n";
        os << name << " & operator >>( " << name << " & src, std::string & str) \n{ \n";
        os << "} \n\n";
    }
private:

    std::vector<base *> children_;
};

class factory
{
public:
    static base * create(rapidxml::xml_node<> *node)
    {
        std::string type;
        std::string def;
        std::string name;
        std::string desc;
        getAttr(node, type, def, name, desc);
        return create(type, def, name, desc);
    }

    static base * create(const std::string & type, const std::string & def, const std::string & name, const std::string & desc)
    {
        base * ret;
        if(type == "base")
        {
            ret = new base;
        }
        else if(type == "seq")
        {
            ret = new sequnece;
        }
        else if(type == "struct")
        {
            ret = new structure;
        }
        else
        {
            throw "bad node type";
        }
        ret->type = type;
        ret->def = def;
        ret->name = name;
        ret->desc = desc;
        return ret;
    }

    static void getAttr(rapidxml::xml_node<> * node, std::string & type, std::string & def, std::string & name, std::string & desc)
    {
        static const std::vector<std::string> attrNames{ "type", "def", "name", "desc"};
        for(int i = 0; i < attrNames.size(); ++i)
        {
            auto attr = node->first_attribute(attrNames[i].c_str());
            if(!attr)
                throw std::string("attr not exsit: ") + "node: " + node->name() + attrNames[i];
            if(i == 0)
            {
                type = attr->value();
            }
            else if(i == 1)
            {
                def = attr->value();
            }
            else if(i == 2)
            {
                name = attr->value();
            }
            else if(i == 3)
            {
                desc = attr->value();
            }
        }
    }

};

class parser : public base
{
public:
    parser(const std::string & xml)
    {
        buf_.resize(xml.size() + 1);
        std::copy(xml.begin(), xml.end(), buf_.begin());
        buf_[buf_.size() - 1] = '\0';
        rapidxml::xml_document<> doc;
        doc.parse<0>(buf_.data());

        for(auto node = doc.first_node(); node; node = node->next_sibling())
        {
            if(std::strcmp(node->name(), "cmd"))
                throw std::string("unexpect node: ") + node->name();

            structure * s = dynamic_cast<structure *>(factory::create(node));
            elements_.push_back(s);
            for(auto node1 = node->first_node(); node1; node1 = node1->next_sibling())
            {
                if(std::strcmp(node1->name(),"item"))
                    throw std::string("unexpect node: ") + node->name();
                base * b = factory::create(node1);
                s->append(b);
            }
        }
    }

    void genDef(std::ostream & os)
    {
        for(auto i : elements_)
        {
            i->genDef(os);
        }
    }

    void genCode(std::ostream & os)
    {
        for(auto i : elements_)
        {
            i->genCode(os);
        }
    }

private:
    std::vector<char> buf_;
    std::vector<structure *> elements_;
};

struct ReqDeviceInfo
{
    int userId;
    int deviceId;
};

struct RetDeviceInfo
{
    int userId;
    int deviceId;
    std::vector<char> deviceInfo;
};

template<typename Dst, typename Src>
Dst getValue(Src src)
{
    Dst ret;
    std::stringstream ss;
    ss << src;
    ss >> ret;
    if(!ss.eof())
        throw std::string("cast err");
    return ret;
}

void test()
{
    std::string xml = "<RetDeviceInfo>"
                       "<userId>123</userId>"
                       "<deviceId>123243</deviceId>"
                       "<Sequence name='deviceInfo'>"
                            "<element>1</element>"
                            "<element>1</element>"
                       "</Sequence>"
                       "</ReqDeviceInfo>";
    ReqDeviceInfo ret;
    rapidxml::xml_document<> doc;
    doc.parse<0>((char *)xml.data());
    auto node = doc.first_node();
    std::cout << node->value() <<std::endl;
    if(!node || std::strcmp(node->name(), "RetDeviceInfo"))
        throw std::string("unexpect node: ") + node->name();
    node = node->first_node();

    if(!node || std::strcmp(node->name(), "userId"))
        throw std::string("unexpect node: ") + node->name();
    ret.userId = getValue<int>(node->value());
    node = node->next_sibling();

    if(!node || std::strcmp(node->name(), "deviceId"))
        throw std::string("unexpect node: ") + node->name();
    ret.deviceId = getValue<int>(node->value());
    node = node->next_sibling();

    if(!node || std::strcmp(node->name(), "Sequence"))
        throw std::string("unexpect node: ") + node->name();
    auto attr = node->first_attribute("name");
    if(!attr || std::strcmp(attr->value(), "deviceInfo"))
        throw std::string("unexpect node: ") + node->name();
    for(auto i = node->first_node(); i; i = i->next_sibling())
    {
        char a;
        a = getValue<char>(node->value());
    }
    node = node->next_sibling();
}



int main(int argc, char *argv[])
{
    test();
    std::string xml("<cmd name='ReqDeviceInfo' type='struct' def='struct' desc='qing qiu shebei xingxi'> "
                    "<item name='userId' type='base' def='int' desc='yong hu id'></item>"
                    "<item name='deviceId' type='base' def='int' desc='she bei id'></item>"
                    "</cmd>"
                    "<cmd name='RetDeviceInfo' type='struct' def='struct' desc='fan hui shebei xingxi'> "
                    "<item name='userId' type='base' def='int' desc='yong hu id'></item>"
                    "<item name='deviceId' type='base' def='int' desc='she bei id'></item>"
                    "<item name='deviceInfo' type='seq' def='char' desc='she bei zhuang tai'></item>"
                    "</cmd>"
                    );
    std::stringstream ss;
    try{
        parser p(xml);
        p.genDef(ss);
        p.genCode(ss);
        std::cout << ss.str();
    }
    catch(const std::string & str)
    {
        std::cout << str << std::endl;
        return -1;
    }

    return 0;
}
