#include "mainwindow.h"
#include "Leo_GPS_Decode.h"
#include "QMessageBox"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>
struct Leo_gps_buffer G_GPS_Bubffer;
extern struct minmea_sentence_rmc G_GPS_RMC;
extern uint8_t G_GPS_RMC_IsValide;
extern struct minmea_sentence_gga G_GPS_GGA;
extern uint8_t G_GPS_GGA_IsValide;

void MainWindow::SerialPortRead() {
  //读取串口数据
  QByteArray readComData = mSerial.readAll();

  //将读到的数据显示到数据接收区的te中
  if (!readComData.isEmpty()) {
    ui->textBrowser_GPSData->append(readComData);
  }

  uint8_t error = 0;
  for (int i = 0; i < readComData.size(); i++) {
    error = Leo_GPS_Decode(&G_GPS_Bubffer, readComData.at(i));
    if (error == 0) {
      //解析成功，输出显示
      if (G_GPS_GGA_IsValide == 1) {
        QString mGGA;
        mGGA.sprintf("$GGA 有效:%d;时间:分%d秒%d;海拔高%d;水平高:%d",
                     G_GPS_GGA.fix_quality, G_GPS_GGA.time.minutes,
                     G_GPS_GGA.time.seconds, G_GPS_GGA.altitude,
                     G_GPS_GGA.height);
        ui->textBrowser_GPSDecode->append(mGGA);
        G_GPS_GGA_IsValide = 0;
      }
      if (G_GPS_RMC_IsValide == 1) {
        QString mRMC;
        mRMC.sprintf("$RMC 有效:%d;时间:月%d日%d;纬度:%d;经度:%d",
                     G_GPS_RMC.valid, G_GPS_RMC.date.month, G_GPS_RMC.date.day,
                     G_GPS_RMC.latitude.value, G_GPS_RMC.longitude.value);
        struct minmea_longlat data;
        data = NemaTransform(G_GPS_RMC);
        qDebug() << "latitude: " << data.latitude;
        qDebug() << "longitude: " << data.longitude;
        ui->textBrowser_GPSDecode->append(mRMC);
        G_GPS_RMC_IsValide = 0;
      }
    }
    if (error == 2) {
      ui->textBrowser_GPSDecode->append(
          "数据解析错误，有可能奇偶检校错误 或 数据判断错误！");
    }
  }

  //清除缓冲区
  readComData.clear();
}

void MainWindow::SerialPortOpen() {
  mSerial.setPortName("cu.wchusbserial1420");
  if (mSerial.open(QIODevice::ReadWrite)) {
    QMessageBox::about(NULL, "提示", "串口打开 成功！");
    //设置波特率
    mSerial.setBaudRate(QSerialPort::Baud9600);
    //设置数据位
    mSerial.setDataBits(QSerialPort::Data8);
    //设置校验位
    mSerial.setParity(QSerialPort::NoParity);
    //设置流控制
    mSerial.setFlowControl(QSerialPort::NoFlowControl);
    //设置停止位
    mSerial.setStopBits(QSerialPort::OneStop);
  } else {
    QMessageBox::about(NULL, "提示", "串口打开 失败！");
  }

  Leo_GPS_BUFFER_Initial(&G_GPS_Bubffer);
}

void MainWindow::SerialPortClose() {
  mSerial.close();
  QMessageBox::about(NULL, "提示", "串口已关闭！");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(&mSerial, &QIODevice::readyRead, this, &MainWindow::SerialPortRead);
  connect(ui->pushButton_Open, &QAbstractButton::clicked, this,
          &MainWindow::SerialPortOpen);

  Leo_GPS_BUFFER_Initial(&G_GPS_Bubffer);
}

MainWindow::~MainWindow() {
  delete ui;
  mSerial.close();
}

// void MainWindow::on_pushButton_Open_clicked()
//{
//    SerialPortOpen();
//}

void MainWindow::on_pushButton_Close_clicked() { SerialPortClose(); }

void MainWindow::on_pushButton_test_clicked() {
  char *mTEST_RMC = "$GPRMC,031921.00,V,,,,,,,231018,,,N*7C\r\n";
  char *mTEST_GGA = "$GPGGA,031921.00,,,,,0,00,99.99,,,,,,*6E\r\n";
  int mCharLength = 42;
  uint8_t error = 0;
  for (int i = 0; i < mCharLength; i++) {
    error = Leo_GPS_Decode(&G_GPS_Bubffer, mTEST_GGA[i]);
    if (error == 0) {
      //解析成功，输出显示
      if (G_GPS_GGA_IsValide == 1) {
        QString mGGA;
        mGGA.sprintf("$GGA 有效:%d;时间:分%d秒%d;海拔高%d;水平高:%d",
                     G_GPS_GGA.fix_quality, G_GPS_GGA.time.minutes,
                     G_GPS_GGA.time.seconds, G_GPS_GGA.latitude,
                     G_GPS_GGA.height);

        ui->textBrowser_GPSDecode->append(mGGA);
        G_GPS_GGA_IsValide = 0;
      }
      if (G_GPS_RMC_IsValide == 1) {
        QString mRMC;
        mRMC.sprintf("$RMC 有效:%d;时间:月%d日%d;纬度:%d;经度:%d",
                     G_GPS_RMC.valid, G_GPS_RMC.date.month, G_GPS_RMC.date.day,
                     G_GPS_RMC.latitude.value, G_GPS_RMC.longitude.value);

        ui->textBrowser_GPSDecode->append(mRMC);
        G_GPS_RMC_IsValide = 0;
      }
      //有完整的GPS语句了
      //                int mNum = G_GPS_Bubffer.pSave - G_GPS_Bubffer.buffer;
      //                char mGps_Sentence[mNum];
      //                memcpy(mGps_Sentence,G_GPS_Bubffer.buffer,mNum);
      //                G_GPS_Bubffer.pSave = G_GPS_Bubffer.buffer;
      //                QString mDecode(mGps_Sentence);
      //                mDecode.append(QString::number(mDecode.size()));
      //                ui->textBrowser_GPSDecode->append(mDecode);
      //                ui->textBrowser_GPSDecode->append(QString::number(sizeof(mGps_Sentence)));
    }
    if (error == 2) {
      ui->textBrowser_GPSDecode->append(
          "数据解析错误，有可能奇偶检校错误 或 数据判断错误！");
    }
  }
}
