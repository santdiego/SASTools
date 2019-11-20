#include "hardwaredevice.h"
using namespace cv;

HardwareDevice::HardwareDevice(QObject *parent, int PID) : QObject(parent)
{
    //qDebug() <<"HardwareDevice::HardwareDevice()";
    timer= new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    shmDataIn = new QSharedMemory(QString::number(PID)+"shmDataIn");
    shmDataOut= new QSharedMemory(QString::number(PID)+"shmDataOut");
    shmConfig = new QSharedMemory(QString::number(PID)+"shmConfig");

}

void HardwareDevice::update(){
    //qDebug()<<"HardwareDevice::update()";
    try{
    outputData.clear();
    shmConfig->lock();
    memcpy(&configuration,shmConfig->data(),sizeof(HardwareConfigurations));

    //int finalSize;
    bool blnFrameReadSuccessfully = capWebcam.read(imgOriginal);                    // get next frame from the webcam
    if (!blnFrameReadSuccessfully || imgOriginal.empty()) {                            // if we did not get a frame
        //QMessageBox::information(this, "", "unable to read from webcam \n\n exiting program\n");        // show error via message box
        //stop();                                                                              // and exit program
        //return;
        qDebug()<<"ERROR!!";
    }


    float resizeFactor =    configuration.resizeFactor /100;
    if(configuration.resizeTool)
        cv::resize(imgOriginal,imgReduced,cv::Size((int)(imgOriginal.cols*resizeFactor),(int)(imgOriginal.rows*resizeFactor)),0,0,INTER_LINEAR);
    else{

        imgOriginal.copyTo(imgReduced);
    }
    //cv::cvtColor(imgReduced,imgReduced,CV_BGR2RGB); //CAMBIA DE COLOR POR PROBLEMA EN WINDOWS
    if (configuration.compressionTool)
       // finalSize=compressedImage(imgReduced,outputData,configuration.compressionType,configuration.compressionFactor);
        compressedImage(imgReduced,outputData,configuration.compressionType,configuration.compressionFactor);
    else{
        if(imgReduced.isContinuous() && !imgReduced.empty()){
            outputData.assign(imgReduced.datastart,imgReduced.dataend);
        }else{
            for (int i = 0; i < imgReduced.rows; ++i) {
              outputData.insert(outputData.end(), imgReduced.ptr<uchar>(i), imgReduced.ptr<uchar>(i)+imgReduced.cols);
            }
        }

    }

    //finalSize=outputData.size();

    configuration.cols=imgReduced.cols; //Actualiza el tamaño de la imagen
    configuration.rows=imgReduced.rows;
    configuration.dataTotalSize=outputData.size(); //Actualiza el tamaño total de datos
    //Actualiza Configuraciones
    memcpy(shmConfig->data(),&configuration,sizeof(HardwareConfigurations));


    //Actualiza Buffer de Salida
    shmDataOut->lock();
    memcpy(shmDataOut->data(),outputData.data(),outputData.size());//outputData.size()<=shmDataOut-size();
    shmDataOut->unlock();
    shmConfig->unlock();

    emit newDeviceDataOutput();//Envia imagen procesada
    }
    catch (exception& e)
    {
      qDebug() << e.what() << '\n';
    }
}

void HardwareDevice::start(){
    //qDebug() <<"HardwareDevice::start()";
    if (shmDataIn->isAttached())
        shmDataIn->detach();
    if (shmDataOut->isAttached())
        shmDataOut->detach();
    if (shmConfig->isAttached())
        shmConfig->detach();
    if (!shmDataIn->attach()) {
        qDebug()<<"Unable to attach shared memory segment shmDataIn HardwareDevice::.";
        return;
    }
    if (!shmDataOut->attach()) {
        qDebug()<<"Unable to attach shared memory segment shmDataOut HardwareDevice::.";
        return;
    }
    if (!shmConfig->attach()) {
        qDebug()<<"Unable to attach shared memory segment shmConfig HardwareDevice::.";
        return;
    }
    shmConfig->lock();
    memcpy(&configuration,shmConfig->data(),sizeof(HardwareConfigurations));
    shmConfig->unlock();
   if(capWebcam.isOpened()==true)
       capWebcam.release();
   capWebcam.open(configuration.selectCamera);              // associate the capture object to the default webcam
   if(capWebcam.isOpened() == false) {                 // if unsuccessful
       //QMessageBox::information(this, "", "error: capWebcam not accessed successfully \n\n exiting program\n",StandarButton btt);        // show error message
       //exitProgram();                          // and exit program
       return;                                 //
   }

   timer->start(configuration.stepSize);

}

void HardwareDevice::stop(){
    //qDebug() <<"HardwareDevice::stop()";
   if(timer->isActive()) timer->stop();
   if(capWebcam.isOpened()==true)
       capWebcam.release();
   if (shmDataIn->isAttached())
       shmDataIn->detach();
   if (shmDataOut->isAttached())
       shmDataOut->detach();
   if (shmConfig->isAttached())
       shmConfig->detach();
}

int HardwareDevice::compressedImage(const cv::Mat &raw, vector<uchar> &buf,int compressType, int quality){
    //qDebug() <<"HardwareDevice::compressedImage";
    QString comprType;
    int compr;
    if(compressType==1){
        comprType=".jpg";
        compr = cv::IMWRITE_JPEG_QUALITY;
    }
    else if (compressType==2)
    {
        comprType = ".png";
        quality = qMin( 9, quality/10 ); // PNG quality is in [0,9] range
        compr = cv::IMWRITE_PNG_COMPRESSION;
    }
    else{
            comprType = ".jpg";
            compr =cv::IMWRITE_JPEG_QUALITY;
    }

    if( !raw.empty() )
    {
        // >>>>> Jpeg compression in memory
        vector<int> params;
        params.push_back( compr );
        params.push_back( quality ); // Quality of compression
        cv::imencode( comprType.toLocal8Bit().data(), raw, buf, params );
        return buf.size();
    }
    return 0;
}

void HardwareDevice::newDeviceConfiguration(){
    qDebug() <<"HardwareDevice::newDeviceConfiguration";

}

void HardwareDevice::newDeviceDataInput(){
    qDebug() <<"HardwareDevice::newDeviceConfiguration";
}
