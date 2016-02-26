#ifndef XMLSTRUCT_H
#define XMLSTRUCT_H

#include <rapidxml.h>

#include <vector>
#include <string>
#include <ostream>
#include <cstring>

namespace xmlstruct
{

namespace detail
{

const std::string xml_node = "rapidxml::xml_node<> *";
const std::string xml_document = "rapidxml::xml_document<>";

class trace
{
public:
    trace(std::ostream & os, const std::string & str1, const std::string & str2)
        : os_(os)
        , str1_(str1)
        , str2_(str2)
    {
        os_ << "/* @generate [ " << str1_ << ":" << str2_ << " ] begin @ */\n";
    }

    ~trace()
    {
        os_ << "/* @generate [ " << str1_ << ":" << str2_ << " ] end @ */\n\n";
    }

private:
    std::ostream & os_;
    const std::string & str1_;
    const std::string & str2_;
};

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
protected:
    item(const std::string & d, const std::string & n, const std::string & c)
        : def(d)
        , name(n)
        , desc(c)
    {
    }

    std::string def;
    std::string name;
    std::string desc;
};

class base : public item
{
public:
    base(const std::string & def, const std::string & name, const std::string & desc)
        : item(def, name, desc)
    {
    }

    virtual void genDef(std::ostream & os)
    {
        trace traceObj(os, name, def);
        (void)traceObj;

        os << "// " << desc << "\n";
        os << def << "    " << name << ";\n\n";
    }

    virtual void genCodeXs(std::ostream & os)
    {
        trace traceObj(os, name, def);
        (void)traceObj;

        os << "if(!node || std::strcmp(node->name(), \"" << name << "\"))\n"
              "return false;\n"
              "if(!xml_struct(node, out." << name << "))\n"
              "return false;\n"
              "node = node->next_sibling();\n"
              "\n";

    }

    virtual void genCodeSx(std::ostream & os)
    {
        trace traceObj(os, name, def);
        (void)traceObj;

        os << "{\n"
              "auto node1 = doc.allocate_node(rapidxml::node_element, \"" << name << "\", doc.allocate_string(get_string(in." << name << ").c_str()));\n"
              "node->append_node(node1);\n"
              "}\n";
    }
};

class vector: public item
{
public:
    vector(const std::string & def, const std::string & name, const std::string & desc)
        : item(def, name, desc)
        , trace_(std::string("std::vector< ") + def + " >")
    {
    }

    virtual void genDef(std::ostream & os)
    {
        trace traceObj(os, name, trace_);
        (void)traceObj;
        os << "// " << desc << "\n";
        os << "::std::vector< " << def << " >    " << name << ";\n";
    }

    virtual void genCodeXs(std::ostream & os)
    {
        //  problems about the name of element in the vector
        //  current unname, for struct type ok,
        //  but for base type,it must have a name to create a xml node,
        //  so the genCodeSx will generate a defalut name element.

        trace traceObj(os, name, trace_);
        (void)traceObj;

        os << "if(!node || std::strcmp(node->name(), \"" << name << "\"))\n"
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
              "node = node->next_sibling();\n";
    }

    virtual void genCodeSx(std::ostream & os)
    {
        trace traceObj(os, name, trace_);
        (void)traceObj;

        os << "{\n"
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
              "}\n";
    }
private:
    std::string trace_;
};

class structure : public item
{
public:
    structure(const std::string & def, const std::string & name, const std::string & desc)
        : item(def, name, desc)
    {
    }

    virtual void genDef(std::ostream & os)
    {
        trace traceObj(os, name, def);
        (void)traceObj;

        os << "// " << desc << "\n";
        os << def << "    " << name << ";\n\n";
    }

    virtual void genCodeXs(std::ostream & os)
    {
        trace traceObj(os, name, def);
        (void)traceObj;

        os << "if(!node || std::strcmp(node->name(), \"" << name << "\"))\n"
              "return false;\n"
              "{\n"
              "auto tmp = node->first_node();\n"
              "if(!xml_struct(tmp, out." << name << "))\n"
              "return false;\n"
              "}\n"
              "node = node->next_sibling();\n";
    }

    virtual void genCodeSx(std::ostream & os)
    {
        trace traceObj(os, name, def);
        (void)traceObj;

        os << "{\n"
              "auto node1 = doc.allocate_node(rapidxml::node_element, \"" << name << "\");\n"
              "node->append_node(node1);\n"
           << xml_node << " node2 = 0;\n"
              "if(!struct_xml(node2, in." << name << ", doc))\n"
              "return false;\n"
              "node1->append_node(node2);\n"
              "}\n";
    }

};

class entry : public object
{
public:
    entry(const std::string & name, const std::string & desc)
        : name_(name)
        , desc_(desc)
        , trace_("@entry")
    {
    }

    void append(item * i)
    {
        items_.push_back(i);
    }

    void genDef(std::ostream & os)
    {
        trace traceObj(os, name_, trace_);
        (void)traceObj;

        os << "// " << desc_ << "\n";
        os << "struct " << name_ << "\n{\n";
        for(auto i : items_)
        {
            i->genDef(os);
        }
        os << "};\n\n"
              "bool xml_struct(const " << xml_node << " , " << name_ << " & );\n"
              "bool struct_xml(" << xml_node << " &, const " << name_ << " & ," << xml_document << " & );\n";
    }

    void genCodeXs(std::ostream & os)
    {
        trace traceObj(os, name_, trace_);
        (void)traceObj;

        os << "bool xml_struct(const " << xml_node << " node, " << name_ << " & out)\n{\n"
              "if(!node || std::strcmp(node->name(), \"" << name_ << "\")) \n"
              "return false;\n"
              "node = node->first_node();\n\n";

        for(auto i : items_)
        {
            i->genCodeXs(os);
        }

        os << "return true;\n"
              "} \n";
    }

    void genCodeSx(std::ostream & os)
    {
        trace traceObj(os, name_, trace_);
        (void)traceObj;

        os << "bool struct_xml(rapidxml::xml_node<> * & node, const " << name_ << " & in, " << xml_document << " & doc)\n"
              "{\n"
              "node = doc.allocate_node(rapidxml::node_element, doc.allocate_string(\"" << name_ << "\"));\n";
        for(auto i : items_)
        {
            //os << "{\n";
            i->genCodeSx(os);
            //os << "}\n";
        }
        os << "return true;\n"
              "}\n";

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
    std::string trace_;
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
            ret = new base(def, name, desc);
        }
        else if(type == "vector")
        {
            ret = new vector(def, name, desc);
        }
        else if(type == "struct")
        {
            ret = new structure(def, name, desc);
        }
        else
        {
            throw std::string("bad node type");
        }
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

} //detail

class parser : public detail::object
{
public:
    parser(const std::string & xml)
        : trace_("@namespace")
    {
        buf_.resize(xml.size() + 1);
        std::copy(xml.begin(), xml.end(), buf_.begin());
        buf_[buf_.size() - 1] = '\0';
        parse();
    }

    ~parser()
    {
        clear();
    }

    void genAll(std::ostream & os)
    {
        genDef(os);
        genBaseFunc(os);
        genCodeSx(os);
        genCodeXs(os);
    }

    void genBaseFunc(std::ostream & os)
    {
        detail::trace traceObj(os, name_, trace_);
        (void)traceObj;
        begin_namespace(os);
        base_code(os);
        end_namespace(os);
    }

    void genDef(std::ostream & os)
    {
        detail::trace traceObj(os, name_, trace_);
        (void)traceObj;
        begin_namespace(os);
        for(auto i : entries_)
        {
            i->genDef(os);
        }
        end_namespace(os);
    }

    void genCodeXs(std::ostream & os)
    {
        detail::trace traceObj(os, name_, trace_);
        (void)traceObj;
        begin_namespace(os);
        for(auto i : entries_)
        {
            i->genCodeXs(os);
        }
        end_namespace(os);
    }

    void genCodeSx(std::ostream & os)
    {
        detail::trace traceObj(os, name_, trace_);
        (void)traceObj;
        begin_namespace(os);
        for(auto i : entries_)
        {
            i->genCodeSx(os);
        }
        end_namespace(os);
    }

    /*
     * This promise the generator code actually never has any bug
     */
    void genNobug(std::ostream & os)
    {
        detail::trace traceObj(os, name_, trace_);
        (void)traceObj;
        os <<
        "/*                                                    \n"
        "                      _ooOoo_                         \n"
        "                     o8888888o                        \n"
        "                     88\" . \"88                        \n"
        "                     (| -_- |)                        \n"
        "                      O\\ = /O                         \n"
        "                  ____/`---'\\____                     \n"
        "                .   ' \\\\| |// `.                      \n"
        "                 / \\\\||| : |||// \\                    \n"
        "               / _||||| -:- |||||- \\                  \n"
        "                 | | \\\\\\ - /// | |                    \n"
        "               | \\_| ''\\---/'' | |                    \n"
        "                \\ .-\\__ `-` ___/-. /                  \n"
        "             ___`. .' /--.--\\ `. . __                 \n"
        "          ."" '< `.___\\_<|>_/___.' >'"".              \n"
        "         | | : `- \\`.;`\\ _ /`;.`/ - ` : | |           \n"
        "           \\ \\ `-. \\_ __\\ /__ _/ .-` / /              \n"
        "   ======`-.____`-.___\\_____/___.-`____.-'======      \n"
        "                      `=---='                         \n"
        "                                                      \n"
        "   .............................................      \n"
        "              God bless        No bug                 \n"
        "                 zwkno1@gmail.com                     \n"
        "*/                                                    \n";
    }

private:
    void parse()
    {
        rapidxml::xml_document<> doc;
        try
        {
            doc.parse<0>(buf_.data());
        }
        catch (const rapidxml::parse_error & err)
        {
            throw std::string(err.what());
        }

        auto root = doc.first_node();
        if(!root || std::strcmp(root->name(), "root"))
            throw std::string("no root");
        auto name = root->first_attribute("name");
        auto desc = root->first_attribute("desc");
        if(!name || !desc)
            throw std::string("bad root name or desc");
        name_ = name->value();
        desc_ = desc->value();

        for(auto node = root->first_node(); node; node = node->next_sibling())
        {
            if(std::strcmp(node->name(), "entry"))
                throw std::string("expect node name entry");

            detail::entry * e = dynamic_cast<detail::entry *>(detail::object_factory::create(node));
            entries_.push_back(e);
            for(auto node1 = node->first_node(); node1; node1 = node1->next_sibling())
            {
                if(std::strcmp(node1->name(),"item"))
                    throw std::string("expect node name item");
                detail::item * i = dynamic_cast<detail::item *>(detail::object_factory::create(node1));
                e->append(i);
            }
        }
    }

    void begin_namespace(std::ostream & os)
    {
        if(!name_.empty())
            os << "namespace " << name_ << "\n"
                  "{\n";
    }

    void end_namespace(std::ostream & os)
    {
        if(!name_.empty())
            os << "} // namespace " << name_ << "\n"
                  "\n";
    }

    void clear()
    {
        for(auto i : entries_)
            delete i;
        entries_.clear();
    }

    void base_code(std::ostream & os)
    {
        os <<
        "\n"
        "template<class T>"
        "bool xml_struct(const rapidxml::xml_node<> * node, T & out)\n"
        "{\n"
        "    std::stringstream ss;\n"
        "    ss << node->value();\n"
        "    ss >> out;\n"
        "    return ss.eof();\n"
        "}\n"
        "\n"
        "template<>\n"
        "bool xml_struct(const rapidxml::xml_node<> * node, char & out)\n"
        "{\n"
        "    if(std::strlen(node->value()) != 1)\n"
        "        return false;\n"
        "    out = node->value()[0];\n"
        "    return true;\n"
        "}\n"
        "\n"
        "//for vector\n"
        "template<class T>\n"
        "bool struct_xml(rapidxml::xml_node<> * & node, const T & in, rapidxml::xml_document<> & doc)\n"
        "{\n"
        "    std::stringstream ss;\n"
        "    ss << in;\n"
        "    node = doc.allocate_node(rapidxml::node_element, \"element\", doc.allocate_string(ss.str().c_str()));\n"
        "    return true;\n"
        "}\n"
        "\n"
        "// for base item\n"
        "template<class T>\n"
        "std::string get_string(const T & in)\n"
        "{\n"
        "    std::stringstream ss;\n"
        "    ss << in;\n"
        "    return ss.str();\n"
        "}\n"
        "\n";
    }

    std::string name_;
    std::string desc_;
    std::string trace_;

    std::vector<char> buf_;
    std::vector<detail::entry *> entries_;
};

} //namespace xmlstruct

#endif // XMLSTRUCT_H
