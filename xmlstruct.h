#ifndef XMLSTRUCT_H
#define XMLSTRUCT_H

#include <rapidxml.h>

#include <vector>
#include <string>
#include <ostream>
#include <cstring>

namespace xmlstruct
{

const std::string xml_node = "rapidxml::xml_node<> *";
const std::string xml_document = "rapidxml::xml_document<>";

class object
{
public:
    virtual void genDef(std::ostream & os) {}
    virtual void genCodeXs(std::ostream & os) {}
    virtual void genCodeSx(std::ostream & os) {}
protected:
    virtual ~object() {}
};

class item : public object
{
public:

    virtual void genDef(std::ostream & os)
    {
        os << "// " << desc << "\n";
        os << def << "    " << name << ";\n\n";
    }

    virtual void genCodeXs(std::ostream & os)
    {
        os << "/* @generate [ " << name << ":" << def << " ] begin @ */\n"
              "if(!node || std::strcmp(node->name(), \"" << name << "\"))\n"
              "return false;\n"
              "if(!xml_struct(node, out." << name << "))\n"
              "return false;\n"
              "node = node->next_sibling();\n"
              "/* @generate [ " << name << ":" << def << " ] end @ */\n"
              "\n";
    }

    virtual void genCodeSx(std::ostream & os)
    {
        os << "/* @generate [ " << name << ":" << def << " ] begin @ */\n"
              "{\n"
              "auto node1 = doc.allocate_node(rapidxml::node_element, \"" << name << "\", doc.allocate_string(getStr(in." << name << ").c_str()));\n"
              "node->append_node(node1);\n"
              "}\n"
              "/* @generate [ " << name << ":" << def << " ] end @ */\n"
              "\n";
    }

    std::string type;
    std::string def;
    std::string name;
    std::string desc;
};

class base : public item
{
public:
    base()
    {
        type = "base";
    }
};

class vector: public item
{
public:
    vector()
    {
        type = "vector";
    }

    virtual void genDef(std::ostream & os)
    {
        os << "// " << desc << "\n";
        os << "::std::vector< " << def << " >    " << name << ";\n\n";
    }

    virtual void genCodeXs(std::ostream & os)
    {
        //  problems about the name of element in the vector
        //  current unname, for struct type ok,
        //  but for base type,it must have a name to create a xml node,
        //  so the genCodeSx will generate a defalut name element.

        os << "/* @generate [ " << name << ":std::vector< " << def << " > ] begin @ */\n"
              "if(!node || std::strcmp(node->name(), \"" << name << "\"))\n"
              "return false;\n"
              "{\n"
              "auto tmp = node->first_node();\n"
              "if(!tmp || std::strcmp(tmp->name(), \"Vector\"))\n"
              "return false;\n"
              "for(auto i = tmp->first_node(); i; i = i->next_sibling())\n"
              "{\n"
           << def << " tmp;\n"
              "if(!xml_struct(i, tmp))\n"
              "return false;\n"
              "out." << name << ".push_back(tmp);\n"
              "}\n"
              "}\n"
              "node = node->next_sibling();\n"
              "/* @generate [ " << name << ":std::vector< " << def << " > ] end @ */\n"
              "\n";
    }

    virtual void genCodeSx(std::ostream & os)
    {
        os << "/* @generate [ " << name << ":std::vector< " << def << " > ] begin @ */\n"
              "{\n"
              "auto node1 = doc.allocate_node(rapidxml::node_element, doc.allocate_string(\"" << name << "\"));\n"
              "node->append_node(node1);\n"
              "auto node2 = doc.allocate_node(rapidxml::node_element, doc.allocate_string(\"Vector\"));\n"
              "node1->append_node(node2);\n"
              "for(auto & i : in." << name << ")\n"
              "{\n"
           << xml_node << " tmp = 0;\n"
              "if(!struct_xml(tmp, i, doc))\n"
              "return false;\n"
              "node2->append_node(tmp);\n"
              "}\n"
              "}\n"
              "/* @generate [ " << name << ":std::vector< " << def << " > ] end @ */\n"
              "\n";
    }
};

class structure : public item
{
public:
    structure()
    {
        type = "struct";
    }

    virtual void genCodeXs(std::ostream & os)
    {
        os << "/* @generate [ " << name << ":" << def << " ] begin @ */\n"
              "if(!node || std::strcmp(node->name(), \"" << name << "\"))\n"
              "return false;\n"
              "{\n"
              "auto tmp = node->first_node();\n"
              "if(!xml_struct(tmp, out." << name << "))\n"
              "return false;\n"
              "}\n"
              "node = node->next_sibling();\n"
              "/* @generate [ " << name << ":" << def << " ] end @ */\n"
              "\n";
    }

    virtual void genCodeSx(std::ostream & os)
    {
        os << "/* @generate [ " << name << ":" << def << " ] begin @ */\n"
              "{\n"
           << xml_node << " node1 = 0;\n"
              "if(!struct_xml(node1, in." << name << ", doc))\n"
              "return false;\n"
              "node->append_node(node1)\n"
              "}\n"
              "/* @generate [ " << name << ":" << def << " ] end @ */\n"
              "\n";
    }

};

class entry : public object
{
public:
    entry(const std::string & name, const std::string & desc)
        : name_(name)
        , desc_(desc)
    {
    }

    void append(item * i)
    {
        items_.push_back(i);
    }

    void genDef(std::ostream & os)
    {
        os << "// " << desc_ << "\n";
        os << "struct " << name_ << "\n{\n";
        for(auto i : items_)
        {
            i->genDef(os);
        }
        os << "};\n\n";
        os << "bool xml_struct(const " << xml_node << " , " << name_ << " & );\n";
        os << "bool struct_xml(" << xml_node << " &, const " << name_ << " & );\n"
              "\n";
    }

    void genCodeXs(std::ostream & os)
    {
        os << "bool xml_struct(const " << xml_node << " node, " << name_ << " & out)\n{\n"
              "if(!node || std::strcmp(node->name(), \"" << name_ << "\")) \n"
              "return false;\n"
              "node = node->first_node();\n\n";

        for(auto i : items_)
        {
            i->genCodeXs(os);
        }

        os << "return true;\n"
              "} \n\n";
    }

    void genCodeSx(std::ostream & os)
    {
        os << "/* @generate [ " << name_ << ": struct ] begin @ */\n"
              "bool struct_xml(rapidxml::xml_node<> * & node, const " << name_ << " & in, " << xml_document << " & doc)\n"
              "{\n"
              "node = doc.allocate_node(rapidxml::node_element, doc.allocate_string(" << name_ <<"));\n";
        for(auto i : items_)
        {
            //os << "{\n";
            i->genCodeSx(os);
            //os << "}\n";
        }
        os << "return true;\n"
              "}\n"
              "/* @generate [ " << name_ << ": struct ] end @ */\n"
              "\n";
    }
    ~entry()
    {
        for(auto i : items_)
        {
            delete i;
        }
    }

private:
    std::string name_;
    std::string desc_;
    std::vector<item *> items_;
};

class object_factory
{
public:
    static object * create(rapidxml::xml_node<> *node)
    {
        std::string node_name = node->name();
        if(node_name == "entry")
        {
            std::string name = getAttr(node, "name");
            std::string desc = getAttr(node, "desc");
            if(name.empty())
                throw std::string("null entry name");
            return new entry(name, desc);
        }
        else if(node_name == "item")
        {
            std::string type = getAttr(node, "type");
            std::string def  = getAttr(node, "def");
            std::string name = getAttr(node, "name");
            std::string desc = getAttr(node, "desc");
            return create_item(type, def, name, desc);
        }
        throw std::string("bad node name: ") + node_name;
    }

private:
    static item * create_item(const std::string & type, const std::string & def, const std::string & name, const std::string & desc)
    {
        item * ret;
        if(type == "base")
        {
            ret = new base;
        }
        else if(type == "vector")
        {
            ret = new vector;
        }
        else if(type == "struct")
        {
            ret = new structure;
        }
        else
        {
            throw std::string("bad node type");
        }
        ret->def = def;
        ret->name = name;
        ret->desc = desc;
        return ret;
    }

    static std::string getAttr(rapidxml::xml_node<> * node, const std::string & name)
    {
        auto attr = node->first_attribute(name.c_str());
        if(attr)
            return attr->value();
        return "";
    }
};

class parser : public object
{
public:
    parser(const std::string & xml)
    {
        buf_.resize(xml.size() + 1);
        std::copy(xml.begin(), xml.end(), buf_.begin());
        buf_[buf_.size() - 1] = '\0';
    }

    void parse()
    {
        rapidxml::xml_document<> doc;
        doc.parse<0>(buf_.data());

        for(auto node = doc.first_node(); node; node = node->next_sibling())
        {
            if(std::strcmp(node->name(), "entry"))
                throw std::string("expect node name entry");

            entry * e = dynamic_cast<entry *>(object_factory::create(node));
            entries_.push_back(e);
            for(auto node1 = node->first_node(); node1; node1 = node1->next_sibling())
            {
                if(std::strcmp(node1->name(),"item"))
                    throw std::string("expect node name item");
                item * i = dynamic_cast<item *>(object_factory::create(node1));
                e->append(i);
            }
        }
    }

    void genDef(std::ostream & os)
    {
        for(auto i : entries_)
        {
            i->genDef(os);
        }
    }

    void genCodeXs(std::ostream & os)
    {
        for(auto i : entries_)
        {
            i->genCodeXs(os);
        }
    }

    void genCodeSx(std::ostream & os)
    {
        for(auto i : entries_)
        {
            i->genCodeSx(os);
        }
    }

    ~parser()
    {
        for(auto i : entries_)
        {
            delete i;
        }
    }

private:
    std::vector<char> buf_;
    std::vector<entry *> entries_;
};

} //namespace xmlstruct

#endif // XMLSTRUCT_H
