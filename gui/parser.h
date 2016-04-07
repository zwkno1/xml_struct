#ifndef PARSER_H
#define PARSER_H

#include "xmlstruct.h"

#include <QObject>

struct Item
{
    QString name;
    QString type;
    QString def;
    QString desc;
};

struct Entry
{
    QString name;
    QString type;
    QString def;
    QString desc;
    QList<Item> items;
};

struct Root
{
    QString name;
    QString type;
    QString def;
    QString desc;
    QList<Entry> entries;
};

class Parser : public QObject
{
    Q_OBJECT

public:
    void parseFromFile(const QString & path, Root & r);
    void saveToFile(const QString & path, const Root & r);
    void genCode(const Root &r, const QString & dir, const QString & name);
private:
    bool genText(const Root & r, std::string & text);
    Root root_;
    xmlstruct::parser parser_;
};

#endif // PARSER_H
