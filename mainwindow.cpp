#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addrecorddialog.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    ui->addButton->setEnabled(false);

    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::onOpenButtonClicked);
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddRecordButtonClicked);
}



void MainWindow::onOpenButtonClicked()
{

    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Выберите файл JSON"),
        "",
        tr("JSON Files (*.json);;All Files (*)")
        );


    if (!fileName.isEmpty()) {
        currentJsonFilePath = fileName;
        ui->addButton->setEnabled(true);
        loadData(fileName);
    }
}




MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadData(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл: " + fileName);
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        QMessageBox::critical(this, "Ошибка", "Неверный формат файла JSON");
        return;
    }


    ui->listWidget->clear();

    QJsonArray array = doc.array();
    QString previousHash;
    bool chainBroken = false;

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();

        QString surname = obj["surname"].toString().trimmed();
        QString name = obj["name"].toString().trimmed();
        QString passport = obj["passport"].toString().trimmed();
        QString date = obj.contains("date") ? obj["date"].toString().trimmed() : "";
        QString hashFromFile = obj["hash"].toString().trimmed();

        QString concatenatedData;
        if (i == 0) {
            if (date.isEmpty()) {
                concatenatedData = surname + name + passport;
            } else {
                concatenatedData = surname + name + passport + date;
            }
        } else {
            if (date.isEmpty()) {
                concatenatedData = surname + name + passport + previousHash;
            } else {
                concatenatedData = surname + name + passport + date + previousHash;
            }
        }

        QByteArray computedHashBytes = QCryptographicHash::hash(concatenatedData.toUtf8(), QCryptographicHash::Sha256);
        QString computedHash = computedHashBytes.toBase64();

        QString entry = QString("Фамилия: %1\nИмя: %2\nПаспорт: %3\nДата: %4\nХеш: %5")
                            .arg(surname)
                            .arg(name)
                            .arg(passport)
                            .arg(date.isEmpty() ? "Нет данных" : date)
                            .arg(hashFromFile);

        QListWidgetItem *item = new QListWidgetItem(entry);

        if (computedHash != hashFromFile || chainBroken) {
            item->setBackground(Qt::red);
            item->setForeground(Qt::white);
            chainBroken = true;
        }

        ui->listWidget->addItem(item);

        if (!chainBroken) {
            previousHash = hashFromFile;
        }
    }
}



void MainWindow::onAddRecordButtonClicked()
{
    AddRecordDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        QString surname = dialog.getSurname();
        QString name = dialog.getName();
        QString passport = dialog.getPassport();

        if (surname.isEmpty() || name.isEmpty() || passport.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены.");
            return;
        }


        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");


        QString previousHash;
        if (ui->listWidget->count() > 0) {
            QListWidgetItem *lastItem = ui->listWidget->item(ui->listWidget->count() - 1);
            QString lastItemText = lastItem->text();


            QRegularExpression regex("Хеш: (.+)");
            QRegularExpressionMatch match = regex.match(lastItemText);
            qDebug() << match;

            if (match.hasMatch()) {
                previousHash = match.captured(1);
            }
        }


        QString concatenatedData = surname + name + passport + currentDateTime + previousHash;


        QByteArray computedHashBytes = QCryptographicHash::hash(concatenatedData.toUtf8(), QCryptographicHash::Sha256);
        QString computedHash = computedHashBytes.toBase64();


        QJsonObject newRecord;
        newRecord["surname"] = surname;
        newRecord["name"] = name;
        newRecord["passport"] = passport;
        newRecord["date"] = currentDateTime;
        newRecord["hash"] = computedHash;


        QFile file(currentJsonFilePath);
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи.");
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();
        array.append(newRecord);


        doc.setArray(array);
        file.resize(0);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();


        QString entry = QString("Фамилия: %1\nИмя: %2\nПаспорт: %3\nДата: %4\nХеш: %5")
                            .arg(surname)
                            .arg(name)
                            .arg(passport)
                            .arg(currentDateTime)
                            .arg(computedHash);

        QListWidgetItem *item = new QListWidgetItem(entry);
        ui->listWidget->addItem(item);

        QMessageBox::information(this, "Успех", "Запись успешно добавлена.");
    }
}


