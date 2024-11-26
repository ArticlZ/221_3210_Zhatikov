#include "mainwindow.h"
#include "pindialog.h"
#include <QApplication>
#include <QCryptographicHash>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QByteArray storedHash = QCryptographicHash::hash("1234", QCryptographicHash::Sha256);


    PinDialog pinDialog;
    if (pinDialog.exec() == QDialog::Accepted) {
        QString enteredPin = pinDialog.getPin();


        QByteArray enteredHash = QCryptographicHash::hash(enteredPin.toUtf8(), QCryptographicHash::Sha256);

        if (enteredHash != storedHash) {
            QMessageBox::critical(nullptr, "Ошибка", "Неверный PIN-код. Приложение будет закрыто.");
            return 0;
        }
    } else {
        return 0;
    }


    MainWindow w;
    w.show();

    return a.exec();
}
