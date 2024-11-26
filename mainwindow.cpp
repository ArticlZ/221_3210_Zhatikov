#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);  // Настройка интерфейса из .ui файла

    // Загружаем данные из JSON файла
    loadData("C:/221_3210_Zhatikov/221_3210_Zhatikov/data.json");
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
    for (const auto &value : array) {
        if (!value.isObject()) continue;

        QJsonObject obj = value.toObject();

        // Формируем строку для отображения записи
        QString entry = QString("Фамилия: %1\nИмя: %2\nПаспорт: %3\nХеш: %4")
                            .arg(obj["surname"].toString())
                            .arg(obj["name"].toString())
                            .arg(obj["passport"].toString())
                            .arg(obj["hash"].toString());

        // Добавляем запись в список
        ui->listWidget->addItem(entry);  // Добавляем элемент в listWidget, созданный в Designer
    }
}
