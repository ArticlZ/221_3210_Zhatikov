#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);  // Настройка интерфейса из .ui файла

    // Загружаем данные из JSON файла
    loadData("C:/221_3210_Zhatikov/221_3210_Zhatikov/dataRED.json");
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

    QJsonArray array = doc.array();
    QString previousHash; // Хеш предыдущей записи (для первой записи пусто)

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();

        QString surname = obj["surname"].toString().trimmed();
        QString name = obj["name"].toString().trimmed();
        QString passport = obj["passport"].toString().trimmed();
        QString hashFromFile = obj["hash"].toString().trimmed();

        // Формируем строку для вычисления хеша
        QString concatenatedData;
        if (i == 0) {
            // Для первой записи учитываем только её поля
            concatenatedData = surname + name + passport;
        } else {
            // Для остальных записей добавляем previousHash
            concatenatedData = surname + name + passport + previousHash;
        }

        // Вычисляем хеш
        QByteArray computedHash = QCryptographicHash::hash(concatenatedData.toUtf8(), QCryptographicHash::Sha256).toBase64();

        // Формируем строку для отображения записи
        QString entry = QString("Фамилия: %1\nИмя: %2\nПаспорт: %3\nХеш: %4")
                            .arg(surname)
                            .arg(name)
                            .arg(passport)
                            .arg(hashFromFile);

        // Создаём элемент списка
        QListWidgetItem *item = new QListWidgetItem(entry);

        // Проверяем, совпадает ли вычисленный хеш с хешем из файла
        if (computedHash != hashFromFile) {
            // Если ошибка, выделяем элемент красным
            item->setBackground(Qt::red);
            item->setForeground(Qt::white);
        }

        // Добавляем элемент в QListWidget
        ui->listWidget->addItem(item);

        // Обновляем previousHash только если хеш корректный
        if (computedHash == hashFromFile) {
            previousHash = hashFromFile;
        } else {
            // Если хеш не совпадает, останавливаем обновление цепочки
            previousHash = ""; // Или можно сохранить старое значение, если это требуется
        }
    }
}


