#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>

namespace Ui {
class InputDialog;
}

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputDialog(QWidget *parent = 0);
    ~InputDialog();
    static QStringList getInputAdd(const QStringList & parent);
    static QStringList getInputMod(const QStringList & self);
    static QStringList getInputAddRoot();
private:
    Ui::InputDialog *ui;
};

#endif // INPUTDIALOG_H
