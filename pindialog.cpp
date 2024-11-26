#include "pindialog.h"
#include "ui_pindialog.h"

PinDialog::PinDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PinDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    setWindowTitle("Введите PIN-код");
}

PinDialog::~PinDialog()
{
    delete ui;
}

QString PinDialog::getPin() const
{
    return ui->pinLineEdit->text().trimmed();
}
