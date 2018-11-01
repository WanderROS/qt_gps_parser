#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QSerialPort mSerial;
    void SerialPortRead();
    void SerialPortOpen();
    void SerialPortClose();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
//    void on_pushButton_Open_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_test_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
