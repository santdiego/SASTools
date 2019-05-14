#ifndef HARDWAREDEVICE_H
#define HARDWAREDEVICE_H

#include <QtCore>
#include <QMessageBox>
#include <qdebug.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace std;
struct HardwareConfigurations{
    int dataTotalSize;
    char latestWhoWrote; //0= Device; 1= Ui; 2=Shared Memory; 3= TCP; 4= UDP
    int rows;
    int cols;
    int selectCamera;
    bool resizeTool; //Activa la herramienta de cambiar Tamaño
    float resizeFactor; //1 a 100 NO PUEDE SER 0!!!
    bool compressionTool; //Activa la herramienta de compresion
    int compressionType; //0: Sin compresion , 1: JPG, 2: PNG
    int compressionFactor; // 1 a 100 NO PUEDE SER 0!!!
    int stepSize; //Periodo de muestreo de la camara (no es el periodo de salida de datos)
};

struct HardwareInfo{
    static const int maxInputDataSize=100;
    static const int maxOutputDataSize=921600;
    static const int configSize=sizeof(HardwareConfigurations);
};

class HardwareDevice : public QObject
{
    Q_OBJECT

    /*ATRIBUTOS Y METODOS PROPIOS DEL TEMPLATE
     * Priorice utilizar los atributos propios del template.
     * Complete los métodos propios del template.
     * Si requiere agregar métodos o atributos personalizados hágalo en la sección inferior.
    */
public:
    explicit HardwareDevice(QObject *parent = 0, int PID=0);

private:
    //Buffers Para intercambio de datos
    QSharedMemory *shmDataIn;
    QSharedMemory *shmDataOut;
    QSharedMemory *shmConfig;
    void createInternalSharedMemory(int PID);
    //Recursos de Timer / Hilos
    QTimer *timer;
    //Estructura de la memoria de configuracion
    HardwareConfigurations configuration;

signals:
    void newDeviceDataOutput(); //Indica Nuevo Dato en memoria
    void deviceStateChanged(); //Indica que el estado del hardware ha cambiado, por ejemplo nueva configuracion.

public slots:
    void start();   //Esta funcion es llamada al inicio del procesamiento.
    void update();  //Esta funcion es llamada por el timer en cada periodo de muestreo.
    void stop();    //Esta funcion es llamada cada vez que se detiene el Hardware.
    void newDeviceConfiguration(); //Esta funcion es llamada cuando la memoria de configuraciones tiene nuevos datos (eje. modificados por el UI)
    void newDeviceDataInput();     //Esta funcion es llamada cuando la memoria de entrada al dispositivo tiene nuevos datos (eje. datos desde otro proceso)


/* ATRIBUTOS Y METODOS PERSONALIZADOS
 * Agregue aquí los métodos y atributos personalizados para la aplicación */

private:
    vector<uchar> outputData;
    vector<uchar> originalData;
    cv::VideoCapture capWebcam;         // Capture object to use with webcam
    cv::Mat imgOriginal;                // input image
    cv::Mat imgReduced;
    int compressedImage(const cv::Mat &raw, vector<uchar> &buf, int compressType, int quality);
};

#endif // HARDWAREDEVICE_H
