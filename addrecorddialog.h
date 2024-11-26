#ifndef ADDRECORDDIALOG_H
#define ADDRECORDDIALOG_H

#include <QDialog>

namespace Ui {
class AddRecordDialog;
}

class AddRecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddRecordDialog(QWidget *parent = nullptr);
    ~AddRecordDialog();

    QString getSurname() const;
    QString getName() const;
    QString getPassport() const;

private:
    Ui::AddRecordDialog *ui;
};

#endif // ADDRECORDDIALOG_H
