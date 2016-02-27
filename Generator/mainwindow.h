#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "parser.h"

#include <QMainWindow>
#include <QTreeWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void loadFromFile();
    void saveToFile();
    void saveToNewFile();
    void Addtem();
    void removeItem();
    void modifyItem();

    void loadTreeFromRoot();
    void saveTreeToRoot();
    void genCode();
private:
    void updateTree();

    Parser parser_;
    Root root_;

    QString filePath_;

    QTreeWidgetItem *currentItem_;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
