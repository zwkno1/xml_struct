#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "inputdialog.h"

#include <QTreeWidget>
#include <QTreeWidgetItemIterator>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <iostream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , filePath_("")
    , currentItem_(0)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Generator");
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::loadFromFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveToFile);
    connect(ui->actionSaveTo, &QAction::triggered, this, &MainWindow::saveToNewFile);
    connect(ui->actionGenCode, &QAction::triggered, this, &MainWindow::genCode);
    connect(ui->actionAdd, &QAction::triggered, this, &MainWindow::Addtem);
    connect(ui->actionRemove, &QAction::triggered, this, &MainWindow::removeItem);
    connect(ui->actionModify, &QAction::triggered, this, &MainWindow::modifyItem);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFromFile()
{
    QString path = QFileDialog::getOpenFileName();
    if(path.size())
    {
        filePath_ = path;
        parser_.parseFromFile(path, root_);
        updateTree();
    }
}

void MainWindow::saveToFile()
{
    saveTreeToRoot();
    if(filePath_.size())
    {
        parser_.saveToFile(filePath_, root_);
    }
    else
    {
        saveToNewFile();
    }
}

void MainWindow::saveToNewFile()
{
    saveTreeToRoot();
    QString path = QFileDialog::getSaveFileName();
    if(path.size())
    {
        parser_.saveToFile(path, root_);
    }
}

void MainWindow::updateTree()
{
    loadTreeFromRoot();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    currentItem_ = ui->treeWidget->currentItem();

    if(currentItem_ && currentItem_->columnCount() == 4)
    {
        auto menu = new QMenu(this);
        if(currentItem_->text(1) == "@root" || currentItem_->text(1) == "@entry")
        {
            menu->addAction(ui->actionAdd);
        }
        menu->addAction(ui->actionRemove);
        menu->addAction(ui->actionModify);
        menu->exec(QCursor::pos());
    }

    if(!currentItem_)
    {
        auto menu = new QMenu(this);
        menu->addAction(ui->actionAdd);
        menu->exec(QCursor::pos());
    }
}

void MainWindow::Addtem()
{
    if(currentItem_ && currentItem_->columnCount() == 4)
    {
        QStringList curr;
        for(int i = 0; i < currentItem_->columnCount(); ++i)
            curr.append(currentItem_->text(i));

        auto ret = InputDialog::getInputAdd(curr);
        if(ret.size() == 4)
            currentItem_->addChild(new QTreeWidgetItem(ret));
    }
    if(!currentItem_)
    {
        auto ret = InputDialog::getInputAddRoot();
        if(ret.size() == 4)
            new QTreeWidgetItem(ui->treeWidget, ret);
    }
}

void MainWindow::removeItem()
{
    if(currentItem_)
    {
        auto parent = currentItem_->parent();
        if(parent)
            parent->removeChild(currentItem_);
        else
            ui->treeWidget->takeTopLevelItem(ui->treeWidget->currentIndex().row());
    }
}

void MainWindow::modifyItem()
{
    if(currentItem_ && currentItem_->columnCount() == 4)
    {
        QStringList curr;
        for(int i = 0; i < currentItem_->columnCount(); ++i)
            curr.append(currentItem_->text(i));
        auto ret = InputDialog::getInputMod(curr);
        if(ret.size() == 4)
        {
            for(int i = 0; i < ret.size(); ++i)
            {
                currentItem_->setText(i, ret.at(i));
            }
        }
    }
}

void MainWindow::loadTreeFromRoot()
{
    ui->treeWidget->clear();
    auto root = new QTreeWidgetItem(ui->treeWidget, QStringList{ root_.name, root_.type, root_.def, root_.desc });

    for(auto & i : root_.entries)
    {
        auto entry = new QTreeWidgetItem(root, QStringList{i.name, i.type, i.def, i.desc});
        for(auto & j : i.items)
        {
            new QTreeWidgetItem(entry, QStringList{j.name, j.type, j.def, j.desc});
        }
    }
}

void MainWindow::saveTreeToRoot()
{
    root_.name.clear();
    root_.type.clear();
    root_.def.clear();
    root_.desc.clear();
    root_.entries.clear();

    auto root = *QTreeWidgetItemIterator(ui->treeWidget);
    if(root)
    {
        root_.name = root->text(0);
        root_.type = root->text(1);
        root_.def = root->text(2);
        root_.desc = root->text(3);
        for(auto i = 0; i < root->childCount(); ++i)
        {
            auto entry = root->child(i);
            Entry en;
            en.name = entry->text(0);
            en.type = entry->text(1);
            en.def = entry->text(2);
            en.desc = entry->text(3);

            for(int j = 0; j < entry->childCount(); ++j)
            {
                auto item = entry->child(j);
                Item it;
                it.name = item->text(0);
                it.type = item->text(1);
                it.def = item->text(2);
                it.desc = item->text(3);
                en.items.push_back(it);
            }

            root_.entries.push_back(en);
        }
    }
}

void MainWindow::genCode()
{
    saveTreeToRoot();
    QString dir = QFileDialog::getExistingDirectory();
    if(dir.size() != 0)
        parser_.genCode(root_, dir, "xxx");
}
