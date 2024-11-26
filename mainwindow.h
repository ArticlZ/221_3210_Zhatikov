#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

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
    QListWidget *listWidget;
    QString currentJsonFilePath;
    void loadData(const QString &fileName);

private slots:
    void onOpenButtonClicked();
    void onAddRecordButtonClicked();


};

#endif // MAINWINDOW_H
