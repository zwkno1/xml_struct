#ifndef XMLSTRUCT_H
#define XMLSTRUCT_H

#include <rapidxml.h>

#include <vector>
#include <string>
#include <ostream>
#include <cstring>

namespace xmlstruct
{

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

    virtual void genCodeI(std::ostream & os)
    {
        os << "if(!node || std::strcmp(node->name(), \"" << name << "\"))\n"
              "return false;\n"
              "if(!xml_get(node, out." << name << "))\n"
              "return false;\n"
              "node = node->next_sibling();\n\n";
    }

    virtual void genCodeO(std::ostream & os, int parent, int & index)
    {
        os << "auto node" << index << " = doc.allocate_node(rapidxml::node_element, \"" << name << "\", doc.allocate_string(getStr(in." << name << ").c_str()));\n"
              "node" << parent << "->append_node(node" << index << ");\n\n";
        ++index;
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

    virtual void genCodeI(std::ostream & os)
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

    virtual void genCodeO(std::ostream & os, int parent, int & index)
    {
        os << "auto node" << index << " = doc.allocate_node(rapidxml::node_element, doc.allocate_string(\"Sequence\"));\n"
              "node" << parent << ".append_node(node" << index << ");\n"
              "for(auto & i : in." << name << ")\n{\n"
              "rapidxml::xml_node<> * tmp = 0;\n"
              "if(!xml_put(tmp, i))\n"
              "return false;\n"
              "node" << index << ".append_node(tmp);\n"
              "}\n";
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

    virtual void genCodeI(std::ostream & os)
    {
        os << "bool xml_get(const rapidxml::xml_node<> * node, " << name << " & out)\n{\n";
        os << "if(!node || std::strcmp(node->name(), \"" << name << "\")) \n"
              "return false;\n"
              "node = node->first_node();\n\n";
        for(auto i : children_)
        {
            i->genCodeI(os);
        }
        os << "return true;\n";
        os << "} \n\n";
    }

    virtual void genCodeO(std::ostream & os, int parent, int & index)
    {

        auto my_index = index++;
        for(auto i : children_)
        {
            i->genCodeO(os, my_index, index);
        }
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
        static const std::vector<std::string> attrNames{"type", "def", "name", "desc", "base_type"};
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

    void genCodeI(std::ostream & os)
    {
        for(auto i : elements_)
        {
            i->genCodeI(os);
        }
    }

    void genCodeO(std::ostream & os)
    {
        for(auto i : elements_)
        {
            int index = 0;
            os << "bool xml_put(rapidxml::xml_node<> * & node, const " << i->name << " & in)\n{\n"
               << "auto node" << index << " = doc.allocate_node(rapidxml::node_element, doc.allocate_string(" << name <<"));\n\n";
            i->genCodeO(os, 0, index);
            os << "node = node0" << ";\n"
                  "return true;\n"
                  "} \n\n";
        }
    }

private:
    std::vector<char> buf_;
    std::vector<structure *> elements_;
};

} //namespace xmlstruct

#endif // XMLSTRUCT_H
