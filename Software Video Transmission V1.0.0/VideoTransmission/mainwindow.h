#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hardwaredevice.h"
#include <QMainWindow>
#include <QAction>
#include <QSharedMemory>
#include <QCameraInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

/*FUNCIONAMIENTO:
 * Basicamente hay 6 objetos involucrados. El objeto principal es el mainwindows que nuclea a todos los demas objetos.
 * Luego hay 5 objetos independientes:
 * 1- el "UI" conformado por 3 formas mas el mainwindows
 * 2- el modulo TCP
 * 3- El modulo UDP
 * 4- El Modulo SHM (memoria compartida del SO)
 * 5- El hardware
 * Cada modulo tiene una instancia de 3 memorias compartidas identicas :    QSharedMemory *shmDataIn;QSharedMemory *shmDataOut;    QSharedMemory *shmConfig;
 * Cada modulo lee y escribe en esas memorias compartidas cuando recibe una señal. cada modulo envia y recibe 3 señales para leer/escribir
 * */


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, int PID=0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    HardwareDevice *device;
    HardwareConfigurations configs;
    //Buffers Para intercambio de datos INTERNO
    QSharedMemory *shmDataIn;
    QSharedMemory *shmDataOut;
    QSharedMemory *shmConfig;
    //Buffers Para intercambio de datos EXTERNO (en revision)
    QSharedMemory *shmDataIn_native;
    QSharedMemory *shmDataOut_native;
    QSharedMemory *shmConfig_native;
    void startExternalSharedMemory();
    void stopExternalSharedMemory();
    //Timers Para intercambio de datos
    QTimer *shmTimer;
    QTimer *tcpTimer;
    QTimer *udpTimer;
    QTcpServer tcpServer;
    QTcpSocket *tcpClientConnection;
    QUdpSocket *udpSocket;
private slots:
    void on_lineEdit_shm_baseName_textChanged(const QString &arg1);
    void on_spinBox_shm_number_valueChanged(int arg1);
    //void on_checkBox_shm_enable_toggled(bool checked);
    void updateDataInput();
    void updateDataOutput();
    void updateConfiguration();
    void updateExternalSharedMemory();
    void startWorking();
    void stopWorking();
    void paintData();
    void newTcpClient();

    void tcpIncomingData();
    void writeTcpData();
     void writeUdpData();
    void readPendingUdpDatagrams();

private:
    void *inputData;
    void *outputData;
    //void *configuration;
    //Interface Handle Functions
    void refresh_SHM_names(const QString &base_name, int number);

    //Data blok buffers between threads.
    void setInputData(const void *);
    void setOutputputData(const void *);
    void getInputData(const void *);
    void getOutputputData(const void *);
    void getConfiguration(const void *);

    void initUdpSocket();
    QImage convertOpenCVMatToQtQImage(cv::Mat mat);


};

#endif // MAINWINDOW_H
