#ifndef XMLSTRUCT_H
#define XMLSTRUCT_H

#include <vector>
#include <string>
#include <ostream>
#include <cstring>
#include <exception>

#include <rapidxml.hpp>

#ifdef XMLSTRUCT_NO_TRACE
#define XML_STRUCT_TRACE(os, str1, str2)
#else
#define XML_STRUCT_TRACE(os, str1, str2) ::xmlstruct::detail::trace trace_object(os, str1, str2); (void)trace_object;
#endif

#ifdef XML_STRUCT_NO_EXCEPTION
extern void xml_struct_parse_error(const char *, const char *)
#define XML_STRUCT_PARSE_ERROR(what, where)  xml_struct_parse_error(what, where)
#else
#define XML_STRUCT_PARSE_ERROR(what, where) throw ::xmlstruct::parse_error(what, where);
#endif


namespace xmlstruct
{

class parse_error: public std::exception
{
public:

    parse_error(const char *what, const char * where)
        : what_(what)
        , where_(where)
    {
    }

    virtual const char *what() const throw()
    {
        return what_;
    }

    const char *where() const
    {
        return where_;
    }

private:
    const char * what_;
    const char * where_;
};

namespace detail
{

const char * const xml_node_type = "rapidxml::xml_node<> *";
const char * const xml_document_type =  "rapidxml::xml_document<>";

class trace
{
public:
    trace(std::ostream & os, const std::string & str1, const std::string & str2)
        : os_(os)
        , str1_(str1)
        , str2_(str2)
    {
        os_ << "/* @generate [ " << str1_ << ":" << str2_ << " ] begin @ */\n\n";
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
    object(const std::string & def, const std::string & name, const std::string & desc)
        : def_(def)
        , name_(name)
        , desc_(desc)
    {
    }

    virtual void genDef(std::ostream & ) {}
    virtual void genCodeXs(std::ostream & ) {}
    virtual void genCodeSx(std::ostream & ) {}
    virtual ~object() {}
protected:
    std::string def_;
    std::string name_;
    std::string desc_;
};

class item : public object
{
public:
    item(const std::string & def, const std::string & name, const std::string & desc)
        : object(def, name, desc)
    {
    }

    virtual void genDef(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, def_);

        os << "// " << desc_ << "\n";
        os << def_ << "    " << name_ << ";\n\n";
    }
};

class base : public item
{
public:
    base(const std::string & def, const std::string & name, const std::string & desc)
        : item(def, name, desc)
    {
    }

    virtual void genCodeXs(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, def_);

        os << "if(!node || std::strcmp(node->name(), \"" << name_ << "\"))\n"
              "return false;\n"
              "if(!xml_struct(node, out." << name_ << "))\n"
              "return false;\n"
              "node = node->next_sibling();\n"
              "\n";
    }

    virtual void genCodeSx(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, def_);

        os << "{\n"
              "auto node1 = doc.allocate_node(rapidxml::node_element, \"" << name_ << "\", doc.allocate_string(get_string(in." << name_ << ").c_str()));\n"
              "node->append_node(node1);\n"
              "}\n";
    }
};

// Concept requirements:
// vector::value_type
// vector::push_back(const vector::value_type &)
class vector: public item
{
public:
    vector(const std::string & def, const std::string & name, const std::string & desc)
        : item(std::string("std::vector< ") + def + std::string(" >"), name, desc)
    {
    }

    virtual void genCodeXs(std::ostream & os)
    {
        //  problems about the name of element in the vector
        //  current unname, for struct type ok,
        //  but for base type,it must have a name to create a xml node,
        //  so the genCodeSx will generate a defalut name "element".

        XML_STRUCT_TRACE(os, name_, def_);

        os << "if(!node || std::strcmp(node->name(), \"" << name_ << "\"))\n"
              "return false;\n"
              "{\n"
              "auto tmp = node->first_node();\n"
              "if(!tmp || std::strcmp(tmp->name(), \"Vector\"))\n"
              "return false;\n"
              "for(auto i = tmp->first_node(); i; i = i->next_sibling())\n"
              "{\n"
           << def_ << "::value_type tmp;\n"
              "if(!xml_struct(i, tmp))\n"
              "return false;\n"
              "out." << name_ << ".push_back(tmp);\n"
              "}\n"
              "}\n"
              "node = node->next_sibling();\n";
    }

    virtual void genCodeSx(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, def_);

        os << "{\n"
              "auto node1 = doc.allocate_node(rapidxml::node_element, doc.allocate_string(\"" << name_ << "\"));\n"
              "node->append_node(node1);\n"
              "auto node2 = doc.allocate_node(rapidxml::node_element, doc.allocate_string(\"Vector\"));\n"
              "node1->append_node(node2);\n"
              "for(auto i = in." << name_ << ".begin(); i != in." << name_ << ".end(); ++i)\n"
              "{\n"
           << xml_node_type << " tmp = 0;\n"
              "if(!struct_xml(tmp, *i, doc))\n"
              "return false;\n"
              "node2->append_node(tmp);\n"
              "}\n"
              "}\n";
    }
};

class structure : public item
{
public:
    structure(const std::string & def, const std::string & name, const std::string & desc)
        : item(def, name, desc)
    {
    }

    virtual void genCodeXs(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, def_);

        os << "if(!node || std::strcmp(node->name(), \"" << name_ << "\"))\n"
              "return false;\n"
              "{\n"
              "auto tmp = node->first_node();\n"
              "if(!xml_struct(tmp, out." << name_ << "))\n"
              "return false;\n"
              "}\n"
              "node = node->next_sibling();\n";
    }

    virtual void genCodeSx(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, def_);

        os << "{\n"
              "auto node1 = doc.allocate_node(rapidxml::node_element, \"" << name_ << "\");\n"
              "node->append_node(node1);\n"
           << xml_node_type << " node2 = 0;\n"
              "if(!struct_xml(node2, in." << name_ << ", doc))\n"
              "return false;\n"
              "node1->append_node(node2);\n"
              "}\n";
    }
};

class entry : public object
{
public:
    entry(const std::string & name, const std::string & desc, const std::string & cmd, const std::string & ns)
        : object("struct", name, desc)
        , cmd_(cmd)
        , namespace_(ns)
    {
    }

    void append(item * i)
    {
        items_.push_back(i);
    }

    void genDef(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, def_);

        os << "// " << desc_ << "\n";
        os << def_ << " " << name_ << "\n{\n";

        if(!cmd_.empty())
        {
            os << "enum { cmd = " << cmd_ << " };\n\n";
        }

        for(auto i : items_)
        {
            i->genDef(os);
        }
        os << "};\n\n";
    }

    void genFuncDef(std::ostream & os)
    {
        os << "bool xml_struct(const " << xml_node_type << " , " << namespace_ << "::" << name_ << " & );\n"
              "bool struct_xml(" << xml_node_type << " &, const " << namespace_ << "::" << name_ << " & ," << xml_document_type << " & );\n\n";
    }

    void genCodeXs(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, def_);

        os << "bool xml_struct(const " << xml_node_type << " node, " << name_ << " & out)\n{\n"
              "if(!node || std::strcmp(node->name(), \"" << name_ << "\")) \n"
              "return false;\n"
              "node = node->first_node();\n\n";

        for(auto i : items_)
        {
            i->genCodeXs(os);
        }

        os << "return true;\n"
              "} \n"
              "\n";
    }

    void genCodeSx(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, def_);

        os << "bool struct_xml(" << xml_node_type << " & node, const " << name_ << " & in, " << xml_document_type << " & doc)\n"
              "{\n"
              "node = doc.allocate_node(rapidxml::node_element, doc.allocate_string(\"" << name_ << "\"));\n";
        for(auto i : items_)
        {
            i->genCodeSx(os);
        }
        os << "return true;\n"
              "}\n"
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
    std::string cmd_;
    std::string namespace_;
    std::vector<item *> items_;
};

class object_factory
{
public:
    static item * create_item(rapidxml::xml_node<> *node)
    {
        if(std::strcmp(node->name(), "item"))
            XML_STRUCT_PARSE_ERROR("expect item", node->name());
        const char * type = get_attribute(node, "type");
        const char * def  = get_attribute(node, "def");
        const char * name = get_attribute(node, "name");
        const char * desc = get_attribute(node, "desc");

        if(!std::strcmp(type, "base"))
        {
            return new base(def, name, desc);
        }
        else if(!std::strcmp(type, "vector"))
        {
            return new vector(def, name, desc);
        }
        else if(!std::strcmp(type, "struct"))
        {
            return new structure(def, name, desc);
        }
        else
        {
            XML_STRUCT_PARSE_ERROR("expect item(base or vector or struct)", type);
        }
    }

    static entry * create_entry(rapidxml::xml_node<> *node, const std::string & ns)
    {
        if(std::strcmp(node->name(), "entry"))
            XML_STRUCT_PARSE_ERROR("expect entry", node->name());
        const char * name = get_attribute(node, "name");
        const char * desc = get_attribute(node, "desc");
        const char * cmd = get_attribute(node, "cmd");

        if(*name == '\0')
            XML_STRUCT_PARSE_ERROR("expect entry name", node->name());

        return new entry(name, desc, cmd, ns);
    }

private:
    static const char * get_attribute(rapidxml::xml_node<> * node, const char * name)
    {
        auto attr = node->first_attribute(name);
        if(attr)
            return attr->value();
        return "";
    }
};

} //detail

class parser : public detail::object
{
public:
    parser()
        : object("", "", "")
        , trace_("@namespace")
    {
    }

    ~parser()
    {
        clear();
    }

    void parse(const std::string & xml)
    {
        buf_.resize(xml.size() + 1);
        std::copy(xml.begin(), xml.end(), buf_.begin());
        buf_[buf_.size() - 1] = '\0';
        parse(buf_.data());
    }

    void parse(char * data)
    {
        rapidxml::xml_document<> doc;
        try
        {
            doc.parse<0>(data);
        }
        catch (const rapidxml::parse_error & err)
        {
            XML_STRUCT_PARSE_ERROR(err.what(), err.where<char>());
        }

        auto root = doc.first_node();
        parse_root(root);

        for(auto node = root->first_node(); node; node = node->next_sibling())
        {
            detail::entry * e = detail::object_factory::create_entry(node, this->name_);
            entries_.push_back(e);
            for(auto node1 = node->first_node(); node1; node1 = node1->next_sibling())
            {
                detail::item * i  = detail::object_factory::create_item(node1);
                e->append(i);
            }
        }
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
        XML_STRUCT_TRACE(os, name_, trace_);

        os << "namespace \n"
              "{\n"
              "\n"
              "template<class T>"
              "bool xml_struct(const " << detail::xml_node_type << " node, T & out)\n"
              "{\n"
              "    std::stringstream ss;\n"
              "    ss << node->value();\n"
              "    ss >> out;\n"
              "    return ss.eof();\n"
              "}\n"
              "\n"
              "template<>"
              "bool xml_struct(const " << detail::xml_node_type << " node, std::string & out)\n"
              "{\n"
              "    out = std::string(node->value()); \n"
              "    return true;\n"
              "}\n"
              "\n"
              "//for vector\n"
              "template<class T>\n"
              "bool struct_xml(" << detail::xml_node_type << " & node, const T & in, " << detail::xml_document_type << " & doc)\n"
              "{\n"
              "    std::stringstream ss;\n"
              "    ss << in;\n"
              "    node = doc.allocate_node(rapidxml::node_element, \"element\", doc.allocate_string(ss.str().c_str()));\n"
              "    return true;\n"
              "}\n"
              "\n"
              "// for base item\n"
              "// char or uchar is treated as interger\n"
              "template<class T>\n"
              "std::string get_string(const T & in)\n"
              "{\n"
              "    std::stringstream ss;\n"
              "    ss << in;\n"
              "    return ss.str();\n"
              "}\n"
              "\n"
              "} // namespace \n\n";

        os << "using namespace " << name_ << ";\n\n";
    }

    void genDef(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, trace_);

        begin_namespace(os);
        for(auto i : entries_)
        {
            i->genDef(os);
        }
        end_namespace(os);

        for(auto i : entries_)
        {
            i->genFuncDef(os);
        }
    }

    void genCodeXs(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, trace_);

        for(auto i : entries_)
        {
            i->genCodeXs(os);
        }
    }

    void genCodeSx(std::ostream & os)
    {
        XML_STRUCT_TRACE(os, name_, trace_);

        for(auto i : entries_)
        {
            i->genCodeSx(os);
        }
    }

private:

    void parse_root(rapidxml::xml_node<> * root)
    {
        if(!root || std::strcmp(root->name(), "root"))
            throw parse_error("expect root", "");

        auto name = root->first_attribute("name");
        if(!name)
            XML_STRUCT_PARSE_ERROR("expect name", root->name());

        auto desc = root->first_attribute("desc");
        if(!desc)
            XML_STRUCT_PARSE_ERROR("expect desc", root->name());

        name_ = name->value();
        desc_ = desc->value();
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

    std::string name_;
    std::string desc_;
    std::string trace_;

    std::vector<char> buf_;
    std::vector<detail::entry *> entries_;
};

} //namespace xmlstruct

#endif // XMLSTRUCT_H
