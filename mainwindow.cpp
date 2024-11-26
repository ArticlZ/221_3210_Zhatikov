#include "mainwindow.h"
#include "ui_mainwindow.h"
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
    ui->setupUi(this);  // Настройка интерфейса из .ui файла

    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::onOpenButtonClicked);

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

        QString entry = QString("Фамилия: %1\nИмя: %2\nПаспорт: %3\nХеш: %4")
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
