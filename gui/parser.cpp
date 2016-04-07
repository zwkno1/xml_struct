#include "parser.h"

#include <rapidxml.h>
#include <rapidxml_print.h>

#include <QFile>
#include <QMessageBox>
#include <fstream>

void Parser::parseFromFile(const QString & path, Root & r)
{
    try
    {
        QFile file(path);
        if(!file.exists() || !file.open(QFile::ReadOnly | QFile::Text))
        {
            throw std::string("Open file failed: ") + path.toStdString();
        }

        QByteArray buf;
        buf.resize(file.size() + 1);
        file.read(buf.data(), file.size());
        buf[buf.size() - 1 ] = '\0';

        r = Root();
        rapidxml::xml_document<> doc;
        doc.parse<0>(buf.data());

        auto root = doc.first_node();
        if(!root || std::strcmp(root->name(), "root"))
            throw std::string("no root");
        auto name = root->first_attribute("name");
        auto desc = root->first_attribute("desc");
        if(!name || !desc)
            throw std::string("bad root name or desc");

        r.name = name->value();
        r.type = "@root";
        r.def = "namespace";
        r.desc = desc->value();

        for(auto node = root->first_node(); node; node = node->next_sibling())
        {
            if(std::strcmp(node->name(), "entry"))
                throw std::string("expect node name entry");

            auto name = node->first_attribute("name");
            auto desc = node->first_attribute("desc");
            if(!name || !desc)
                throw std::string("bad entry name or desc");

            Entry entry;
            entry.name = name->value();
            entry.type = "@entry";
            entry.def = "struct";
            entry.desc = desc->value();

            for(auto node1 = node->first_node(); node1; node1 = node1->next_sibling())
            {
                if(std::strcmp(node1->name(),"item"))
                    throw std::string("expect node name item");

                auto name = node1->first_attribute("name");
                auto type = node1->first_attribute("type");
                auto def = node1->first_attribute("def");
                auto desc = node1->first_attribute("desc");

                if(!name || !type || !def || !desc)
                    throw std::string("bad item: name or type or def or desc");

                Item item;
                item.name = name->value();
                item.type = type->value();
                item.def = def->value();
                item.desc = desc->value();

                entry.items.push_back(item);
            }
            r.entries.push_back(entry);
        }
    }
    catch(const std::string & err)
    {
        QMessageBox::warning(0, "Warnning", path + QString(" parse error: ") + QString::fromStdString(err));
    }
    catch(const rapidxml::parse_error & e)
    {
        QMessageBox::warning(0, "Warnning", path + QString(" parse error: ") + QString::fromStdString(e.what()));
    }
}

void Parser::saveToFile(const QString & path, const Root & r)
{
    QStringList errors;

    if(r.type != "@root" && r.def != "namespace")
        errors.push_back(QString("bad root: name(%1), type(%2), def(%3), desc(%4)").arg(r.name).arg(r.type).arg(r.def).arg(r.desc));

    rapidxml::xml_document<> doc;
    auto root = doc.allocate_node(rapidxml::node_element, doc.allocate_string("root"));
    doc.append_node(root);
    root->append_attribute(doc.allocate_attribute("name", doc.allocate_string(r.name.toStdString().c_str())));
    root->append_attribute(doc.allocate_attribute("desc", doc.allocate_string(r.desc.toStdString().c_str())));

    for(auto & i : r.entries)
    {
        if(i.type != "@entry" && i.def != "struct")
            errors.push_back(QString("bad entry: name(%1), type(%2), def(%3), desc(%4)").arg(i.name).arg(i.type).arg(i.def).arg(i.desc));
        auto entry = doc.allocate_node(rapidxml::node_element, doc.allocate_string("entry"));
        root->append_node(entry);
        entry->append_attribute(doc.allocate_attribute("name", doc.allocate_string(i.name.toStdString().c_str())));
        entry->append_attribute(doc.allocate_attribute("desc", doc.allocate_string(i.desc.toStdString().c_str())));
        for(auto & j : i.items)
        {
            if(j.type != "base" && j.type != "struct" && j.type != "vector")
                errors.push_back(QString("bad item: name(%1), type(%2), def(%3), desc(%4)").arg(j.name).arg(j.type).arg(j.def).arg(j.desc));

            auto item = doc.allocate_node(rapidxml::node_element, doc.allocate_string("item"));
            entry->append_node(item);
            item->append_attribute(doc.allocate_attribute("name", doc.allocate_string(j.name.toStdString().c_str())));
            item->append_attribute(doc.allocate_attribute("type", doc.allocate_string(j.type.toStdString().c_str())));
            item->append_attribute(doc.allocate_attribute("def", doc.allocate_string(j.def.toStdString().c_str())));
            item->append_attribute(doc.allocate_attribute("desc", doc.allocate_string(j.desc.toStdString().c_str())));
        }
    }

    std::string text;
    rapidxml::print(std::back_inserter(text),doc,0);

    QFile file(path);
    if(file.open(QFile::Text|QFile::ReadWrite|QFile::Truncate));
    {
        if(file.write(text.data(), text.size()) == text.size())
        {
            QMessageBox::information(0, "OK: ", QString("Save to %1 success!").arg(path));
            goto print_errors;
        }
    }

    QMessageBox::critical(0, "Errors: ", QString("Save to %1 failed!").arg(path));

print_errors:
    if(!errors.empty())
    {
        QString w;
        for(auto & i : errors)
            w += i + "\n";
        QMessageBox::warning(0, "Warnning", w);
    }
}

void Parser::genCode(const Root &r, const QString & dir, const QString & name)
{
    std::string text;
    if(genText(r, text))
    {
        xmlstruct::parser parser;
        try
        {
            parser.parse(&text[0]);
        }
        catch(const std::string & err)
        {
            QMessageBox::critical(0, "Errors: ", QString("Generate code err: %1").arg(err.c_str()));
            return;
        }
        std::ofstream head(std::string((dir + "/" + name).toStdString()) + ".h");
        std::ofstream source(std::string((dir + "/" + name).toStdString()) + ".cpp");

        head << "\n\n";
        head << "#include <rapidxml.h>\n";
        head << "\n";
        head << "#include <vector>\n";
        head << "\n\n";
        parser.genDef(head);
        head.flush();
        head.close();

        source << "\n";
        source << "#include \"" << name.toStdString() << ".h\"\n";
        source << "#include <cstring>\n";
        source << "#include <sstream>\n";
        source << "\n";
        source << "\n";
        parser.genBaseFunc(source);
        parser.genCodeSx(source);
        parser.genCodeXs(source);
        source.flush();
        source.close();

        QMessageBox::information(0, ":-)   ","Generate code Success!");
        return;
    }

    QMessageBox::critical(0, ":-(", QString("Generate code err: "));
}

bool Parser::genText(const Root &r, std::string &text)
{
    rapidxml::xml_document<> doc;
    auto root = doc.allocate_node(rapidxml::node_element, doc.allocate_string("root"));
    doc.append_node(root);
    root->append_attribute(doc.allocate_attribute("name", doc.allocate_string(r.name.toStdString().c_str())));
    root->append_attribute(doc.allocate_attribute("desc", doc.allocate_string(r.desc.toStdString().c_str())));

    for(auto & i : r.entries)
    {
        if(i.type != "@entry" && i.def != "struct")
            return false;
        auto entry = doc.allocate_node(rapidxml::node_element, doc.allocate_string("entry"));
        root->append_node(entry);
        entry->append_attribute(doc.allocate_attribute("name", doc.allocate_string(i.name.toStdString().c_str())));
        entry->append_attribute(doc.allocate_attribute("desc", doc.allocate_string(i.desc.toStdString().c_str())));
        for(auto & j : i.items)
        {
            if(j.type != "base" && j.type != "struct" && j.type != "vector")
                return false;

            auto item = doc.allocate_node(rapidxml::node_element, doc.allocate_string("item"));
            entry->append_node(item);
            item->append_attribute(doc.allocate_attribute("name", doc.allocate_string(j.name.toStdString().c_str())));
            item->append_attribute(doc.allocate_attribute("type", doc.allocate_string(j.type.toStdString().c_str())));
            item->append_attribute(doc.allocate_attribute("def", doc.allocate_string(j.def.toStdString().c_str())));
            item->append_attribute(doc.allocate_attribute("desc", doc.allocate_string(j.desc.toStdString().c_str())));
        }
    }

    rapidxml::print(std::back_inserter(text),doc,0);
    return true;
}
