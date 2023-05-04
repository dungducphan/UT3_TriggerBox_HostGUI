#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "delaysettings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_pushButton_GET_DelaySettings_clicked();
    void on_pushButton_SET_DelaySettings_clicked();
    void on_pushButton_StartTrigger_clicked();
    void on_pushButton_StopTrigger_clicked();

private:
    Ui::MainWindow *ui;
    void OpenUART();
    void CloseUART();
    void GetDelay(unsigned int ID);
    void SetDelay(unsigned int ID);
    int m_SerialPort{};
    std::shared_ptr<DelaySettings> m_DelaySettings;
};
#endif // MAINWINDOW_H
