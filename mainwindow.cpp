#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addrecorddialog.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QFileDialog> // Для диалога выбора файла

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Кнопка "Add" недоступна, пока не открыт файл
    ui->addButton->setEnabled(false);

    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::onOpenButtonClicked);
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddRecordButtonClicked);
}



void MainWindow::onOpenButtonClicked()
{
    // Открываем диалог выбора файла
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Выберите файл JSON"),
        "",
        tr("JSON Files (*.json);;All Files (*)")
        );

    // Если пользователь выбрал файл, загружаем данные
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

    // Очищаем предыдущие записи из списка
    ui->listWidget->clear();

    QJsonArray array = doc.array();
    QString previousHash; // Хеш предыдущей записи (для первой записи пусто)
    bool chainBroken = false; // Флаг нарушения цепочки

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();

        QString surname = obj["surname"].toString().trimmed();
        QString name = obj["name"].toString().trimmed();
        QString passport = obj["passport"].toString().trimmed();
        QString hashFromFile = obj["hash"].toString().trimmed();

        QString concatenatedData;
        if (i == 0) {
            concatenatedData = surname + name + passport; // Первая запись
        } else {
            concatenatedData = surname + name + passport + previousHash; // Остальные записи
        }

        QByteArray computedHashBytes = QCryptographicHash::hash(concatenatedData.toUtf8(), QCryptographicHash::Sha256);
        QString computedHash = computedHashBytes.toBase64();

        QString entry = QString("Фамилия: %1\nИмя: %2\nПаспорт: %3\nХеш (из файла): %4")
                            .arg(surname)
                            .arg(name)
                            .arg(passport)
                            .arg(hashFromFile);

        QListWidgetItem *item = new QListWidgetItem(entry);

        if (computedHash != hashFromFile || chainBroken) {
            item->setBackground(Qt::red);
            item->setForeground(Qt::white);
            chainBroken = true; // Все последующие записи также отмечаются как некорректные
        }

        ui->listWidget->addItem(item);

        if (!chainBroken) {
            previousHash = hashFromFile;
        }
    }
}

void MainWindow::onAddRecordButtonClicked()
{
    // Проверяем, открыт ли файл
    if (currentJsonFilePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала откройте JSON-файл.");
        return;
    }

    AddRecordDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        QString surname = dialog.getSurname();
        QString name = dialog.getName();
        QString passport = dialog.getPassport();

        if (surname.isEmpty() || name.isEmpty() || passport.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены.");
            return;
        }

        // Текущая дата и время
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        // Хеш для новой записи
        QString concatenatedData = surname + name + passport;
        QByteArray computedHashBytes = QCryptographicHash::hash(concatenatedData.toUtf8(), QCryptographicHash::Sha256);
        QString computedHash = computedHashBytes.toBase64();

        // Формируем новую запись
        QJsonObject newRecord;
        newRecord["surname"] = surname;
        newRecord["name"] = name;
        newRecord["passport"] = passport;
        newRecord["date"] = currentDateTime;
        newRecord["hash"] = computedHash;

        // Открываем текущий JSON-файл
        QFile file(currentJsonFilePath);
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи.");
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();
        array.append(newRecord);

        // Обновляем JSON-документ и перезаписываем файл
        doc.setArray(array);
        file.resize(0); // Очистка файла перед записью
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();

        // Добавляем новую запись в список
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



