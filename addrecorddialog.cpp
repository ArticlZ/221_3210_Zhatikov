#include "addrecorddialog.h"
#include "ui_addrecorddialog.h"


AddRecordDialog::AddRecordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddRecordDialog)
{
    ui->setupUi(this);
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);


}

AddRecordDialog::~AddRecordDialog()
{
    delete ui;
}

QString AddRecordDialog::getSurname() const
{
    return ui->surnameLineEdit->text().trimmed();
}

QString AddRecordDialog::getName() const
{
    return ui->nameLineEdit->text().trimmed();
}

QString AddRecordDialog::getPassport() const
{
    return ui->passportLineEdit->text().trimmed();
}