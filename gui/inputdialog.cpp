#include "inputdialog.h"
#include "ui_inputdialog.h"
#include <QSharedPointer>

InputDialog::InputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDialog)
{
    ui->setupUi(this);
}

InputDialog::~InputDialog()
{
    delete ui;
}

QStringList InputDialog::getInputAdd(const QStringList & parent)
{
    auto dialog = QSharedPointer<InputDialog>(new InputDialog);
    if(parent.at(1) == "@root")
    {
        dialog->ui->comboBoxType->addItem("@entry");
        dialog->ui->lineEditDef->setText("struct");
        dialog->ui->lineEditDef->setEnabled(false);
    }
    else if(parent.at(1) == "@entry")
    {
        dialog->ui->comboBoxType->addItem("base");
        dialog->ui->comboBoxType->addItem("struct");
        dialog->ui->comboBoxType->addItem("vector");
    }
    if(dialog->exec())
    {
            return QStringList{dialog->ui->lineEditName->text(),
                    dialog->ui->comboBoxType->currentText(),
                    dialog->ui->lineEditDef->text(),
                    dialog->ui->lineEditDesc->text()};
    }
    return QStringList{};
}

QStringList InputDialog::getInputMod(const QStringList & self)
{
    auto dialog = QSharedPointer<InputDialog>(new InputDialog);
    dialog->ui->lineEditName->setText(self.at(0));
    dialog->ui->comboBoxType->addItem(self.at(1));
    dialog->ui->lineEditDef->setText(self.at(2));
    dialog->ui->lineEditDesc->setText(self.at(3));
    if(self.at(1) == "@root")
    {
        dialog->ui->lineEditDef->setEnabled(false);
    }
    else if(self.at(1) == "@entry")
    {
        dialog->ui->lineEditDef->setEnabled(false);
    }
    else if(self.at(1) == "base")
    {
        dialog->ui->comboBoxType->addItem("struct");
        dialog->ui->comboBoxType->addItem("vector");
    }
    else if(self.at(1) == "struct")
    {
        dialog->ui->comboBoxType->addItem("base");
        dialog->ui->comboBoxType->addItem("vector");
    }
    else if(self.at(1) == "vector")
    {
        dialog->ui->comboBoxType->addItem("base");
        dialog->ui->comboBoxType->addItem("struct");
    }
    if(dialog->exec())
    {
            return QStringList{dialog->ui->lineEditName->text(),
                    dialog->ui->comboBoxType->currentText(),
                    dialog->ui->lineEditDef->text(),
                    dialog->ui->lineEditDesc->text()};
    }
    return QStringList{};

}

QStringList InputDialog::getInputAddRoot()
{
    auto dialog = QSharedPointer<InputDialog>(new InputDialog);

    dialog->ui->comboBoxType->addItem("@root");
    dialog->ui->lineEditDef->setText("namespace");
    dialog->ui->lineEditDef->setEnabled(false);

    if(dialog->exec())
    {
            return QStringList{dialog->ui->lineEditName->text(),
                    dialog->ui->comboBoxType->currentText(),
                    dialog->ui->lineEditDef->text(),
                    dialog->ui->lineEditDesc->text()};
    }
    return QStringList{};
}
