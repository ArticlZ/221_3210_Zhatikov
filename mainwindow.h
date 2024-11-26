#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget> // Подключаем класс QListWidget

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QListWidget *listWidget; // Объявляем переменную listWidget
    QString currentJsonFilePath; // Путь к открытому JSON-файлу
    void loadData(const QString &fileName); // Объявляем метод loadData

private slots:
    void onOpenButtonClicked();
    void onAddRecordButtonClicked();


};

#endif // MAINWINDOW_H
