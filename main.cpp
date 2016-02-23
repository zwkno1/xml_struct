#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <rapidxml.h>
#include <cstring>

class base
{
public:
    std::string type;
    std::string def;
    std::string name;
    std::string desc;
    virtual void genDef(std::ostream & os)
    {
        os << "// " << desc << "\n";
        os << def << "    " << name << ";\n\n";
    }

    virtual void genCode(std::ostream & os)
    {
        os << "if(!node || std::strcmp(node->name(), \"" << name << "\"))\n"
              "return false;\n"
              "if(!xml_get(node, out." << name << "))\n"
              "return false;\n"
              "node = node->next_sibling();\n\n";
    }
};

class sequnece : public base
{
public:
    sequnece(const std::string & base_type)
        : base_type_(base_type)
    {
        type = "seq";
    }

    virtual void genCode(std::ostream & os)
    {
        os << "if(!node || std::strcmp(node->name(), \"Sequence\"))\n"
              "return false;\n"
              "auto attr = node->first_attribute(\"name\");\n"
              "if(!attr || std::strcmp(attr->value(), \"" << name <<"\"))\n"
              "return false;\n"
              "for(auto i = node->first_node(); i; i = i->next_sibling())\n"
              "{\n"
           << base_type_ << " tmp;\n"
              "if(!xml_get(i, tmp))\n"
              "return false;\n"
              "out." << name << ".push_back(tmp);\n"
              "}\n"
              "node = node->next_sibling();\n\n";
    }
private:
    std::string base_type_;
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
        os << "// " << desc << "\n";
        os << def << " " << name << "\n{\n";
        for(auto i : children_)
        {
            i->genDef(os);
        }
        os << "};\n\n";
        os << "bool xml_get(const rapidxml::xml_node<> * , " << name << " & );\n";
        os << "bool xml_put(rapidxml::xml_node<> * &, const " << name << " & );\n\n";
    }

    virtual void genCode(std::ostream & os)
    {
        os << "bool xml_get(const rapidxml::xml_node<> * node, " << name << " & out)\n{\n";
        os << "if(!node || std::strcmp(node->name(), \"" << name << "\")) \n"
              "return false;\n"
              "node = node->first_node();\n\n";
        for(auto i : children_)
        {
            i->genCode(os);
        }
        os << "return true;\n";
        os << "} \n\n";
        os << "bool xml_put(rapidxml::xml_node<> * & node, const " << name << " & in)\n{\n";
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
        std::string base_type;
        getAttr(node, type, def, name, desc, base_type);
        return create(type, def, name, desc, base_type);
    }

private:
    static base * create(const std::string & type, const std::string & def, const std::string & name, const std::string & desc, const std::string & base_type = "")
    {
        base * ret;
        if(type == "base")
        {
            ret = new base;
        }
        else if(type == "seq")
        {
            if(base_type.empty())
                throw std::string("bad sequence base");
            ret = new sequnece(base_type);
        }
        else if(type == "struct")
        {
            ret = new structure;
        }
        else
        {
            throw std::string("bad node type");
        }
        ret->type = type;
        ret->def = def;
        ret->name = name;
        ret->desc = desc;
        return ret;
    }

    static void getAttr(rapidxml::xml_node<> * node, std::string & type, std::string & def, std::string & name, std::string & desc, std::string & base_type)
    {
        static const std::vector<std::string> attrNames{ "type", "def", "name", "desc", "base_type"};
        for(int i = 0; i < attrNames.size(); ++i)
        {
            auto attr = node->first_attribute(attrNames[i].c_str());
            if(!attr && attrNames[i] != "base_type")
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
            else if(i == 4)
            {
                if(attr)
                    base_type = attr->value();
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
            if(std::strcmp(node->name(), "struct"))
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

// for test
#ifdef TEST
#include "test.h"
void do_test()
{
    std::string xml = "<RetDeviceInfo>"
                       "<userId>123</userId>"
                       "<deviceId>123243</deviceId>"
                       "<Sequence name='deviceInfo'>"
                            "<ReqDeviceInfo>"
                            "<userId>7234</userId>"
                            "<deviceId>22334</deviceId>"
                            "</ReqDeviceInfo>"
                            "<ReqDeviceInfo>"
                            "<userId>6234</userId>"
                            "<deviceId>22334</deviceId>"
                            "</ReqDeviceInfo>"
                            "<ReqDeviceInfo>"
                            "<userId>5234</userId>"
                            "<deviceId>22334</deviceId>"
                            "</ReqDeviceInfo>"
                            "<ReqDeviceInfo>"
                            "<userId>4234</userId>"
                            "<deviceId>22334</deviceId>"
                            "</ReqDeviceInfo>"
                       "</Sequence>"
                       "</ReqDeviceInfo>";
    rapidxml::xml_document<> doc;
    doc.parse<0>((char *)xml.data());
    auto node = doc.first_node();
    test::RetDeviceInfo info;
    if(test::xml_get(node, info))
        std::cout << "ok" << std::endl;
    //template
#if 0
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
#endif

}
#endif


int main(int argc, char *argv[])
{
#ifdef TEST
    do_test();
#endif
    std::string xml("<struct name='ReqDeviceInfo' type='struct' def='struct' desc='qing qiu shebei xingxi'> "
                    "<item name='userId' type='base' def='int' desc='yong hu id'></item>"
                    "<item name='deviceId' type='base' def='int' desc='she bei id'></item>"
                    "</struct>"
                    "<struct name='RetDeviceInfo' type='struct' def='struct' desc='fan hui shebei xingxi'> "
                    "<item name='userId' type='base' def='int' desc='yong hu id'></item>"
                    "<item name='deviceId' type='base' def='int' desc='she bei id'></item>"
                    "<item name='deviceInfo' type='seq' def='std::vector<ReqDeviceInfo>' base_type='ReqDeviceInfo' desc='she bei zhuang tai'></item>"
                    "</struct>"
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
